#include "ddb.h"
#include "ddbint.h"
#include "ddbstring.h"
#include "ddbblock.h"
#include "ddbblock_type.h"
#include "memory.h"

#include <stdbool.h>

static int CheckObjectType(register Tcl_Obj* objPtr, enum DdbBlockType prefType)
{
    uint16_t objType;

    if (objPtr->typePtr != Ddb_GetObjType())
        return false;

    objType = DDB_LOWORD(BLOCK_INT_REP(objPtr).value);

    if (prefType != DDB_ANY)
    {
        if (objType != (uint16_t) prefType)
            return false;
    }

    return true;
}

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
    block = DDB_ALLOC(DDB_FileHeader, 1);
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

int DdbSubcommand_Columns(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
{
    Tcl_Obj* headObj, **elements, *result;
    DDB_ColumnHeader* columns;
    int length, code;

    if (objc != 2)
    {
        DDB_SUBCOMMAND_WRONG_ARGS("fileHeader columns");
        return TCL_ERROR;
    }

    headObj = objv[0];

    /* Invalid block type. */
    if (! CheckObjectType(headObj, DDB_FILE_HEADER))
    {
        DDB_SET_STRING_RESULT(interp, "first argument is not a valid file header");
        return TCL_ERROR;
    }

    /* Extrapolate list elements from argument 2. */
    code = Tcl_ListObjGetElements(interp, objv[1], &length, &elements);

    if (code != TCL_OK)
        return TCL_ERROR;

    /* Zero-length list. */
    if (! length)
    {
        DDB_SET_STRING_RESULT(interp, "zero length list argument");
        return TCL_ERROR;
    }

    /* Object result with the column headers. */
    columns = DDB_ALLOC(DDB_ColumnHeader, length);
    result  = Tcl_NewObj();

    BLOCK_INT_REP(result).ptr   = columns;
    BLOCK_INT_REP(result).value = DDB_PACK_DWORD(length, DDB_COLUMN_HEADER);

    result->typePtr = Ddb_GetObjType();
    DDB_ASSERT(result->typePtr, "null type pointer\n");
    Tcl_IncrRefCount(result);

    /* Extract elements from list argument. */
    for (int i = 0; i < length; ++i)
    {
        Tcl_Obj** subelements; /* Each element is itself a list, hence subelements. */
        int sublen; /* Length of each sublist. */

        /* Attempt to extrapolate elements from this sublist. */
        code = Tcl_ListObjGetElements(interp, elements[i], &sublen, &subelements);
        if (code != TCL_OK) break;

        /* Error if not exactly 3 count. */
        if (sublen != 3)
        {
            DDB_PRINTF_RESULT(interp, "malformed list; sublist %d must be 3 elements", i);
            return TCL_ERROR;
        }

        strncpy(columns[i].name, Tcl_GetStringFromObj(subelements[i], NULL), sizeof(columns[i].name));
        {
            int temp_int;
            const char* temp_str = Tcl_GetStringFromObj(subelements[2], NULL);
            code = Tcl_GetIntFromObj(interp, subelements[1], &temp_int);
            if (code != TCL_OK) break;
            columns[i].length = (uint32_t) temp_int;
            columns[i].type = *temp_str;
        }
    }

    /* Update file header based on the columns. */
    Ddb_UpdateFileHeader(DDB_STATIC_CAST(DDB_FileHeader*, BLOCK_INT_REP(headObj).ptr),
        columns, length);

    if (code == TCL_OK)
    {
        Tcl_SetObjResult(interp, result);
        Tcl_DecrRefCount(result);
        Tcl_InvalidateStringRep(result);
    }

    return code;
}

int DdbSubcommand_Print(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
{
    DDB_DWord ddbTypeValue;

    if (objc != 1)
    {
        DDB_SUBCOMMAND_WRONG_ARGS("ddb print val");
        return TCL_ERROR;
    }

    if (! CheckObjectType(objv[0], DDB_ANY))
    {
        DDB_SET_STRING_RESULT(interp, "first argument is not a valid ddb block");
        return TCL_ERROR;
    }

    DDB_UNPACK_DWORD(ddbTypeValue, BLOCK_INT_REP(objv[0]).value);

    switch (ddbTypeValue.lo)
    {
        case DDB_FILE_HEADER:
        {
            DDB_FileHeader* temp_ptr = (DDB_FileHeader*) BLOCK_INT_REP(objv[0]).ptr;
            printf("number of entries: %u\nblock size: %u\nnumber of columns: %u\n",
                temp_ptr->numEntries, temp_ptr->blockSize, temp_ptr->numColumns);
            printf("offset to first entry: %u\n", temp_ptr->offset);
            break;
        }

        case DDB_COLUMN_HEADER:
        {
            DDB_ColumnHeader* temp_ptr = (DDB_ColumnHeader*) BLOCK_INT_REP(objv[0]).ptr;
            char buffer[50];
            for (uint16_t i = 0; i < ddbTypeValue.hi; ++i)
            {
                strcpy(buffer, temp_ptr[i].name);
                printf("column %u\nname: %s\nlength: %u bytes\ntype: %c\n\n",
                    i, buffer, temp_ptr[i].length, temp_ptr[i].type);
            }
            break;
        }

        default:
            DDB_SET_STRING_RESULT(interp, "unknown object type");
            return TCL_ERROR;
    }

    return TCL_OK;
}
