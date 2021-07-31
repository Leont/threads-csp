#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#define NO_XSLOCKS
#include "XSUB.h"

#include "mthread.h"

#ifdef WIN32
#  include <windows.h>
#  include <win32thread.h>
#else
#  include <pthread.h>
#  include <thread.h>
#endif

static perl_mutex counter_mutex;
static perl_cond counter_condvar;
static UV thread_counter;

static int (*old_hook)(pTHX);

static int S_threadhook(pTHX) {
	MUTEX_LOCK(&counter_mutex);
	while (thread_counter > 1)
		COND_WAIT(&counter_condvar, &counter_mutex);

	MUTEX_UNLOCK(&counter_mutex);
	MUTEX_DESTROY(&counter_mutex);
	COND_DESTROY(&counter_condvar);

	return old_hook(aTHX);
}

void global_init(pTHX) {
	if (thread_counter == 0) {
		MUTEX_INIT(&counter_mutex);
		COND_INIT(&counter_condvar);
		thread_counter = 1;

		old_hook = PL_threadhook;
		PL_threadhook = S_threadhook;
	}
	if (!PL_perl_destruct_level)
		PL_perl_destruct_level = 1;

	HV* channel_stash = gv_stashpvs("threads::csp::channel", GV_ADD);
	SvFLAGS(channel_stash) |= SVphv_CLONEABLE;
}

static void thread_count_inc() {
	MUTEX_LOCK(&counter_mutex);
	thread_counter++;
	MUTEX_UNLOCK(&counter_mutex);
}

static void thread_count_dec() {
	MUTEX_LOCK(&counter_mutex);
	if (--thread_counter == 0);
		COND_SIGNAL(&counter_condvar);
	MUTEX_UNLOCK(&counter_mutex);
}

void boot_DynaLoader(pTHX_ CV* cv);

static void xs_init(pTHX) {
	dXSUB_SYS;
	newXS((char*)"DynaLoader::boot_DynaLoader", boot_DynaLoader, (char*)__FILE__);
}

typedef struct mthread {
	Promise* input;
	Promise* output;
} mthread;

static void* run_thread(void* arg) {
	static const char* argv[] = { "perl", "-Mthreads::csp", "-e", "0", NULL };
	static const int argc = sizeof argv / sizeof *argv - 1;

	thread_count_inc();

	mthread* thread = (mthread*)arg;
	Promise* output = thread->output;

	PerlInterpreter* my_perl = perl_alloc();
	perl_construct(my_perl);
	PERL_SET_CONTEXT(my_perl);
	PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
	perl_parse(my_perl, xs_init, argc, (char**)argv, NULL);

	AV* to_run = (AV*)sv_2mortal(promise_get(thread->input));
	promise_abandon(thread->input);
	promise_refcount_dec(thread->input);
	PerlMemShared_free(thread);

	dXCPT;
	XCPT_TRY_START {
		SV* module = *av_fetch(to_run, 0, FALSE);
		load_module(PERL_LOADMOD_NOIMPORT, SvREFCNT_inc(module), NULL);
	} XCPT_TRY_END
	XCPT_CATCH {
		promise_set_exception(output, ERRSV);
	}
	else {
		dSP;
		PUSHMARK(SP);
		IV len = av_len(to_run) + 1;
		for(int i = 2; i < len; i++) {
			SV** entry = av_fetch(to_run, i, FALSE);
			XPUSHs(*entry);
		}
		PUTBACK;

		SV** call_ptr = av_fetch(to_run, 1, FALSE);
		call_sv(*call_ptr, G_SCALAR | G_EVAL);
		SPAGAIN;

		if (SvTRUE(ERRSV))
			promise_set_exception(output, ERRSV);
		else
			promise_set_value(output, POPs);
	}
	promise_refcount_dec(output);

	perl_destruct(my_perl);
	perl_free(my_perl);

	thread_count_dec();

	return NULL;
}

Promise* thread_spawn(AV* to_run) {
	static const size_t stack_size = 512 * 1024;

	mthread* mthread = PerlMemShared_calloc(1, sizeof(mthread));
	Promise* input = promise_alloc(2);
	mthread->input = input;
	Promise* output = promise_alloc(2);
	mthread->output = output;

#ifdef WIN32
	CreateThread(NULL, (DWORD)stack_size, run_thread, (LPVOID)mthread, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);

#else
	pthread_attr_t attr;
	pthread_attr_init(&attr);

#ifdef PTHREAD_ATTR_SETDETACHSTATE
	PTHREAD_ATTR_SETDETACHSTATE(&attr, PTHREAD_CREATE_DETACHED);
#endif

#ifdef _POSIX_THREAD_ATTR_STACKSIZE
	pthread_attr_setstacksize(&attr, stack_size);
#endif

#if defined(HAS_PTHREAD_ATTR_SETSCOPE) && defined(PTHREAD_SCOPE_SYSTEM)
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
#endif

	/* Create the thread */
	pthread_t thr;
#ifdef OLD_PTHREADS_API
	pthread_create(&thr, attr, run_thread, (void *)mthread);
#else
	pthread_create(&thr, &attr, run_thread, (void *)mthread);
#endif

#endif

	/* This blocks on the other thread, so must run last */
	promise_set_value(input, (SV*)to_run);
	promise_refcount_dec(input);

	return output;
}
