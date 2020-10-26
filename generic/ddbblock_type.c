#include "ddb.h"
#include "ddbint.h"
#include "ddbblock_type.h"
#include "memory.h"

#include <string.h>

static const size_t PtrStrLen = 16U;

/* Custom Tcl value type structure. */
static const Tcl_ObjType tclBlockType = {
    "ddbblock",
    DdbBlock_FreeInternalRepProc,
    DdbBlock_DupIntRepProc,
    DdbBlock_StringUpdateProc,
    NULL
};

void DdbBlock_DupIntRepProc(Tcl_Obj* srcPtr, register Tcl_Obj* dupPtr)
{
    size_t blksize = 0;

    DDB_TRACE_PRINT_FUNCTION();

    switch (BLOCK_INT_REP(srcPtr).value)
    {
        case DDB_FILE_HEADER:
            blksize = sizeof(DDB_FileHeader);
            BLOCK_INT_REP(dupPtr).ptr = DDB_TCL_ALLOC(DDB_FileHeader, 1);
            break;

        case DDB_COLUMN_HEADER:
            break;

        case DDB_RECORD:
            break;

        default:
            Tcl_Panic(
                "invalid data block type %d, must be one of %d (DDB_FILE_HEADER), %d (DDB_COLUMN_HEADER), or %d (DDB_RECORD)",
                (int) BLOCK_INT_REP(srcPtr).value,
                DDB_FILE_HEADER,
                DDB_COLUMN_HEADER,
                DDB_RECORD
            );
            break;
    }

    /* Copy source data to destination block. */
    BLOCK_INT_REP(dupPtr).value = BLOCK_INT_REP(srcPtr).value;
    memcpy(BLOCK_INT_REP(dupPtr).ptr, BLOCK_INT_REP(srcPtr).ptr, blksize);
}

void DdbBlock_FreeInternalRepProc(register Tcl_Obj* objPtr)
{
    DDB_TRACE_PRINT_FUNCTION();

    switch (BLOCK_INT_REP(objPtr).value)
    {
        case DDB_FILE_HEADER:
            ckfree(BLOCK_INT_REP(objPtr).ptr);
        case DDB_COLUMN_HEADER:
        case DDB_RECORD:
            break;

        default: break;
    }

    BLOCK_INT_REP(objPtr).ptr   = NULL;
    BLOCK_INT_REP(objPtr).value = 0;
    objPtr->typePtr = NULL;
}

void DdbBlock_StringUpdateProc(register Tcl_Obj* objPtr)
{
    const char* prefix = NULL;
    char name[50];
    size_t len;

    DDB_TRACE_PRINT_FUNCTION();

    /* Format a string with */
    sprintf(name, "DDBBlock0x%lx", (unsigned long) BLOCK_INT_REP(objPtr).ptr);
    len = strlen(name);
    objPtr->bytes = ckalloc(len + 1);
    memcpy(objPtr->bytes, name, len + 1);
    objPtr->length = (int) len;
}

const Tcl_ObjType* Ddb_GetObjType()
{
    return &tclBlockType;
}
