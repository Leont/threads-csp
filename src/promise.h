struct promise;
typedef struct promise promise_t;

promise_t* promise_alloc(UV);
SV* S_promise_get(pTHX_ promise_t* promise);
#define promise_get(promise) S_promise_get(aTHX_ promise)
void S_promise_abandon(pTHX_ promise_t* promise);
#define promise_abandon(promise) S_promise_abandon(aTHX_ promise)
void promise_set_value(promise_t* promise, SV* value);
void promise_set_exception(promise_t* promise, SV* value);
void promise_refcount_dec(promise_t* promise);

SV* S_promise_to_sv(pTHX_ promise_t* promise);
#define promise_to_sv(promise) S_promise_to_sv(aTHX_ promise)
promise_t* S_sv_to_promise(pTHX_ SV* sv);
#define sv_to_promise(sv) S_sv_to_promise(aTHX_ sv)
