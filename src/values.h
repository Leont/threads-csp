SV* S_clone_value(pTHX_ SV* original);
#define clone_value(original) S_clone_value(aTHX_ original)
