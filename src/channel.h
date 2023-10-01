struct channel;
typedef struct channel Channel;

Channel* S_channel_alloc(pTHX_ UV);
#define channel_alloc(refcount) S_channel_alloc(aTHX_ refcount)
void channel_send(Channel* channel, SV* message);
SV* S_channel_receive(pTHX_ Channel* channel);
#define channel_receive(channel) S_channel_receive(aTHX_ channel)
SV* S_channel_receive_ready_fh(pTHX_ Channel*);
#define channel_receive_ready_fh(channel) S_channel_receive_ready_fh(aTHX_ channel)
SV* S_channel_send_ready_fh(pTHX_ Channel*);
#define channel_send_ready_fh(channel) S_channel_send_ready_fh(aTHX_ channel)
void channel_close(Channel*);

void S_channel_refcount_dec(pTHX_ Channel* channel);
#define channel_refcount_dec(channel) S_channel_refcount_dec(aTHX_ channel)

SV* S_channel_to_sv(pTHX_ Channel* channel, SV* stash_name);
#define channel_to_sv(channel, stash_name) S_channel_to_sv(aTHX_ channel, stash_name)
#define channel_new(class) channel_to_sv(channel_alloc(1), class)
Channel* S_sv_to_channel(pTHX_ SV* sv);
#define sv_to_channel(sv) S_sv_to_channel(aTHX_ sv)
