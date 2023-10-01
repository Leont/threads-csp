struct promise;
typedef struct promise Promise;

Promise* S_promise_alloc(pTHX_ UV);
#define promise_alloc(count) S_promise_alloc(aTHX_ count)
SV* S_promise_get(pTHX_ Promise* promise);
#define promise_get(promise) S_promise_get(aTHX_ promise)
void promise_set_value(Promise* promise, SV* value);
void promise_set_exception(Promise* promise, SV* value);
bool promise_is_finished(Promise*);
void S_promise_refcount_dec(pTHX_ Promise* promise);
#define promise_refcount_dec(promise) S_promise_refcount_dec(aTHX_ promise)
SV* S_promise_finished_fh(pTHX_ Promise* promise);
#define promise_finished_fh(promise) S_promise_finished_fh(aTHX_ promise)

SV* S_promise_to_sv(pTHX_ Promise* promise);
#define promise_to_sv(promise) S_promise_to_sv(aTHX_ promise)
Promise* S_sv_to_promise(pTHX_ SV* sv);
#define sv_to_promise(sv) S_sv_to_promise(aTHX_ sv)
