SV* S_clone_value(pTHX_ SV* original);
#define clone_value(original) S_clone_value(aTHX_ original)

SV* S_object_to_sv(pTHX_ void* object, HV* stash, const MGVTBL* magic_table, UV flags);
#define object_to_sv(object, stash, magic_table, flags) S_object_to_sv(aTHX_ object, stash, magic_table, flags)
void* S_sv_to_object(pTHX_ SV* sv, const char* name, const MGVTBL* magic_table);
#define sv_to_object(sv, name, magic_table) S_sv_to_object(aTHX_ sv, name, magic_table)

#define mark_clonable_stash(stash) (SvFLAGS(stash) |= SVphv_CLONEABLE)
#define mark_clonable_const(classname) mark_clonable_stash(gv_stashpvs(classname, GV_ADD))
