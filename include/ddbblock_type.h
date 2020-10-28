#ifndef DDBBLOCK_TYPE_H
#define DDBBLOCK_TYPE_H

#include <tcl.h>

/*
BLOCK_INT_REP - References the internal represenation DDB type Tcl value
    Assuming that OBJPTR is of the custom type "ddbblock", this references
    the internal representation of that type. There are two members:
    'value' (unsigned long) and 'ptr' (void*). 'value' contains the type
    of ddb block, which occupies the lower 16 bits. Retrieve the value
    with DDB_LOWORD. The higher 16 bits is dependent on the type of ddb block.
    Possible options are listed below:

    Low Word            High Word
    -------------------|------------
    DDB_FILE_HEADER    | Zero
    DDB_COLUMN_HEADER  | Number of columns
    DDB_RECORD         | Zero
*/
#define BLOCK_INT_REP(objPtr) objPtr->internalRep.ptrAndLongRep

void DdbBlock_DupIntRepProc(Tcl_Obj* srcPtr, Tcl_Obj* dupPtr);
void DdbBlock_FreeInternalRepProc(Tcl_Obj* objPtr);
void DdbBlock_StringUpdateProc(Tcl_Obj* objPtr);

#endif /* DDBBLOCK_TYPE_H */
