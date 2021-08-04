#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "ppport.h"

#include "refcount.h"
#include "values.h"

#include "channel.h"

/*
 * Message channels
 */

enum state { HAS_NOTHING, HAS_READER, HAS_WRITER, HAS_MESSAGE };
static const char* state_names[] = { "nothing", "has-reader", "has-writer", "has-message" };

struct channel {
	perl_mutex data_mutex;
	perl_mutex reader_mutex;
	perl_mutex writer_mutex;
	perl_cond  data_condvar;

	refcount_t refcount;
	enum state state;
	SV* message;
};

channel_t* channel_alloc(UV refcount) {
	channel_t* ret = PerlMemShared_calloc(1, sizeof(channel_t));
	MUTEX_INIT(&ret->data_mutex);
	MUTEX_INIT(&ret->reader_mutex);
	MUTEX_INIT(&ret->writer_mutex);
	COND_INIT(&ret->data_condvar);
	refcount_init(&ret->refcount, refcount);
	return ret;
}

void channel_send(channel_t* channel, SV* message) {
	MUTEX_LOCK(&channel->writer_mutex);
	MUTEX_LOCK(&channel->data_mutex);

	channel->message = message;
	if (channel->state == HAS_READER) {
		channel->state = HAS_MESSAGE;
		COND_SIGNAL(&channel->data_condvar);
	}
	else {
		assert(channel->state == HAS_NOTHING);
		channel->state = HAS_WRITER;
	}

	do COND_WAIT(&channel->data_condvar, &channel->data_mutex);
	while (channel->state != HAS_NOTHING && channel->state != HAS_READER);

	MUTEX_UNLOCK(&channel->data_mutex);
	MUTEX_UNLOCK(&channel->writer_mutex);
}

SV* S_channel_receive(pTHX_ channel_t* channel) {
	MUTEX_LOCK(&channel->reader_mutex);
	MUTEX_LOCK(&channel->data_mutex);

	if (channel->state == HAS_NOTHING) {
		channel->state = HAS_READER;
		do COND_WAIT(&channel->data_condvar, &channel->data_mutex);
		while (channel->state != HAS_MESSAGE);
	}
	else
		assert(channel->state == HAS_WRITER);

	SV* result = clone_value(channel->message);

	channel->message = NULL;
	channel->state = HAS_NOTHING;
	COND_SIGNAL(&channel->data_condvar);

	MUTEX_UNLOCK(&channel->data_mutex);
	MUTEX_UNLOCK(&channel->reader_mutex);

	return result;
}

void channel_refcount_dec(channel_t* channel) {
	if (refcount_dec(&channel->refcount) == 1) {
		COND_DESTROY(&channel->data_condvar);
		MUTEX_DESTROY(&channel->writer_mutex);
		MUTEX_DESTROY(&channel->reader_mutex);
		MUTEX_DESTROY(&channel->data_mutex);
		PerlMemShared_free(channel);
	}
}

static int channel_magic_destroy(pTHX_ SV* sv, MAGIC* magic) {
	channel_refcount_dec((channel_t*)magic->mg_ptr);
	return 0;
}

static int channel_magic_dup(pTHX_ MAGIC* magic, CLONE_PARAMS* param) {
	channel_t* channel = (channel_t*)magic->mg_ptr;
	refcount_inc(&channel->refcount);
	return 0;
}

static const MGVTBL channel_magic = { 0, 0, 0, 0, channel_magic_destroy, 0, channel_magic_dup };

SV* S_channel_to_sv(pTHX_ channel_t* channel, SV* stash_name) {
	object_to_sv(channel, gv_stashsv(stash_name, 0), &channel_magic, MGf_DUP);
}

channel_t* S_sv_to_channel(pTHX_ SV* sv) {
	return sv_to_object(sv, "threads::csp::channel", &channel_magic);
}
