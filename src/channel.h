struct channel;
typedef struct channel channel_t;

channel_t* channel_alloc(UV);
void channel_send(channel_t* channel, SV* message);
SV* S_channel_receive(pTHX_ channel_t* channel);
#define channel_receive(channel) S_channel_receive(aTHX_ channel)

void channel_refcount_dec(channel_t* channel);

SV* S_channel_to_sv(pTHX_ channel_t* channel, SV* stash_name);
#define channel_to_sv(channel, stash_name) S_channel_to_sv(aTHX_ channel, stash_name)
#define channel_new(class) channel_to_sv(channel_alloc(1), class)
channel_t* S_sv_to_channel(pTHX_ SV* sv);
#define sv_to_channel(sv) S_sv_to_channel(aTHX_ sv)
