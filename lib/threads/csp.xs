#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "channel.h"
#include "mthread.h"

#define slurp_arguments(offset) av_make(items - offset, PL_stack_base + ax + offset)

MODULE = threads::csp              PACKAGE = threads::csp

BOOT:
	global_init(aTHX);

void spawn(SV* class, SV* module, SV* function, ...)
	CODE:
		thread_spawn(slurp_arguments(1));

MODULE = threads::csp              PACKAGE = threads::csp::channel  PREFIX = channel_

SV* channel_new(SV* class)

void channel_send(channel_t* channel, SV* argument)

SV* channel_receive(channel_t* channel)
