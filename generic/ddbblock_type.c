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
    DDB_DWord dw;

    DDB_TRACE_PRINT_FUNCTION();

    DDB_UNPACK_DWORD(dw, BLOCK_INT_REP(srcPtr).value);

    switch (dw.lo)
    {
        case DDB_FILE_HEADER:
            blksize = sizeof(DDB_FileHeader);
            BLOCK_INT_REP(dupPtr).ptr = DDB_ALLOC(DDB_FileHeader, 1);
            break;

        case DDB_COLUMN_HEADER:
            blksize = sizeof(DDB_ColumnHeader);
            BLOCK_INT_REP(dupPtr).ptr = DDB_ALLOC(DDB_ColumnHeader, dw.hi);
            break;

        case DDB_RECORD:
            Tcl_Panic("records are not currently supported");
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
    const unsigned long type = BLOCK_INT_REP(objPtr).value;

    switch (DDB_LOWORD(type))
    {
        case DDB_FILE_HEADER:
        case DDB_COLUMN_HEADER:
            Ddb_Free(BLOCK_INT_REP(objPtr).ptr);
            break;
        case DDB_RECORD:
        {
            const size_t length = DDB_HIWORD(type);
            DDB_Record** ptr = (DDB_Record**) BLOCK_INT_REP(objPtr).ptr;
            for (size_t i = 0; i < length; ++i)
                Ddb_Free(ptr[i]);
            Ddb_Free((void*) ptr);
            break;
        }

        default: break;
    }

    BLOCK_INT_REP(objPtr).ptr   = NULL;
    BLOCK_INT_REP(objPtr).value = 0;
    objPtr->typePtr = NULL;
}

void DdbBlock_StringUpdateProc(register Tcl_Obj* objPtr)
{
    char name[50];
    size_t len;
    const char* prefix = NULL;

    switch (DDB_LOWORD(BLOCK_INT_REP(objPtr).value))
    {
        case DDB_FILE_HEADER:
            prefix = "DDBFileHeader";
            break;

        case DDB_COLUMN_HEADER:
            prefix = "DDBColumnHeader";
            break;

        case DDB_RECORD:
            prefix = "DDBRecords";
            break;

        default:
            prefix = "unknown";
            break;
    }

    /* Format a string with */
    sprintf(name, "%s0x%lx", prefix, (unsigned long) BLOCK_INT_REP(objPtr).ptr);
    len = strlen(name);
    objPtr->bytes = ckalloc(len + 1);
    memcpy(objPtr->bytes, name, len + 1);
    objPtr->length = (int) len;
}

const Tcl_ObjType* Ddb_GetObjType()
{
    return &tclBlockType;
}
