#ifndef DDBBLOCK_TYPE_H
#define DDBBLOCK_TYPE_H

#include <tcl.h>

#define BLOCK_INT_REP(objPtr) objPtr->internalRep.ptrAndLongRep

void DdbBlock_DupIntRepProc(Tcl_Obj* srcPtr, Tcl_Obj* dupPtr);
void DdbBlock_FreeInternalRepProc(Tcl_Obj* objPtr);
void DdbBlock_StringUpdateProc(Tcl_Obj* objPtr);

#endif /* DDBBLOCK_TYPE_H */
