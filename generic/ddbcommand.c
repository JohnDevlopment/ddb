#include "ddb.h"
#include "ddbint.h"
#include "ddbstring.h"
#include "ddbblock.h"
#include "ddbblock_type.h"
#include "memory.h"

int DdbSubcommand_Hash(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
{
    Tcl_Obj* result = NULL;
    const char* param = NULL;

    if (objc != 1)
    {
        /*DDB_SET_STRING_RESULT(interp, "wrong # args: should be ddb hash aString");*/
        DDB_SUBCOMMAND_WRONG_ARGS(DdbEmptyString);
        return TCL_ERROR;
    }

    /* Retrieve string. Return if empty. */
    param = Tcl_GetStringFromObj(objv[0], NULL);
    if (! strlen(param))
        return TCL_OK;
    else
    {
        /* Calculate hash of string. Place it in long value. */
        char buffer[19];
        unsigned long hash = Ddb_Hash(param);
        int length;

        /* Format string. */
        sprintf(buffer, "0x%lx", (long) hash);
        length = (int) strlen(buffer);

        /* Wrap string in object. */
        result = Tcl_NewStringObj(buffer, length);
        Tcl_IncrRefCount(result);
    }

    if (result)
        Tcl_SetObjResult(interp, result);

    return TCL_OK;
}

int DdbSubcommand_Init(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
{
    Tcl_Obj* result;
    DDB_FileHeader* block;

    if (objc)
    {
        /*DDB_SET_STRING_RESULT(interp, DDB_COMMAND_WRONG_ARG_MESSAGE("ddb init"));*/
        DDB_SUBCOMMAND_WRONG_ARGS(DdbEmptyString);
        return TCL_ERROR;
    }

    /* Allocate block. */
    /*block = (DDB_FileHeader*) Tcl_Alloc(sizeof(DDB_FileHeader));*/
    block = DDB_TCL_ALLOC(DDB_FileHeader, 1);
    if (! block)
    {
        DDB_SET_STRING_RESULT(interp, "failed to allocate pointer");
        return TCL_ERROR;
    }
    Ddb_InitFileHeader(block);

    /* New object, place pointer in there. */
    result = Tcl_NewObj();
    DDB_ASSERT(result->refCount == 0, "result pointer refcount is nonzero after creation: %d\n",
        result->refCount);
    /*Tcl_IncrRefCount(result);*/

    /* Set internal rep. */
    BLOCK_INT_REP(result).ptr   = block;
    BLOCK_INT_REP(result).value = DDB_FILE_HEADER;

    /* Set type of object. Set as result of function. Give it a valid string rep. */
    result->typePtr = Ddb_GetObjType();
    DDB_ASSERT(result->typePtr, "type pointer NULL");
    Tcl_InvalidateStringRep(result);
    Tcl_SetObjResult(interp, result);
    DDB_TRACE_PRINTF("result->refCount = %d\n", result->refCount);

    return TCL_OK;
}

#if 0
int DdbSubcommand_Free(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
{
    Tcl_Obj* obj;
    DDB_FileHeader* block;

    if (objc != 1)
    {
        /*DDB_SET_STRING_RESULT(interp, DDB_COMMAND_WRONG_ARG_MESSAGE("ddb init obj"));*/
        DDB_SUBCOMMAND_WRONG_ARGS("obj");
        return TCL_ERROR;
    }

    obj = objv[0];

    /* Not a block. */
    if (obj->typePtr != Ddb_GetObjType())
    {
        DDB_SET_STRING_RESULT(interp, "argument is not a ddb block");
        return TCL_ERROR;
    }

    /* Not a valid block type. */
    if (obj->internalRep.ptrAndLongRep.value != DDB_FILE_HEADER)
    {
        DDB_SET_STRING_RESULT(interp, "invalid block type\n");
        return TCL_ERROR;
    }

    DDB_TRACE_PRINTF("DdbSubcommand_Free: refcount = %d\n", obj->refCount);
    Tcl_DecrRefCount(obj);
    DDB_TRACE_PRINTF("DdbSubcommand_Free: refcount = %d\n", obj->refCount);
    if (! obj->refCount)
    {
        DDB_TRACE_PRINTF("string obj");
        Tcl_SetStringObj(obj, DdbEmptyString, -1);
    }

    return TCL_OK;
}
#endif
