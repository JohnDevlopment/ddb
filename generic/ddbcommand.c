#include "ddb.h"
#include "ddbint.h"
#include "ddbstring.h"
#include "memory.h"

int DdbSubcommand_Hash(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
{
    Tcl_Obj* result = NULL;
    SString param;

    if (objc != 1)
    {
        DDB_SET_STRING_RESULT(interp, "wrong # args: should be ddb hash aString");
        return TCL_ERROR;
    }

    /* Retrieve string. */
    param.ptr = Tcl_GetStringFromObj(objv[0], &param.length);
    if (! param.ptr || param.length < 1)
    {
        DDB_SET_STRING_RESULT(interp, "invalid string argument passed to ddb hash");
        return TCL_ERROR;
    }

    {
        /* Calculate hash of string. Place it in long value. */
        long temp = (long) Ddb_Hash(param.ptr);
        result = Tcl_NewLongObj(temp);
        if (result)
        {
            /* Replace string version of the value. */
            Tcl_InvalidateStringRep(result);
            result->bytes = DDB_ALLOC(char, 17);
            sprintf(result->bytes, "0x%lx", temp);
            result->length = (int) strlen(result->bytes);
            DDB_TRACE_PRINTF("length of resulting string: %d\n", result->length);
        }
    }

    /* Set return value. */
    if (result)
        Tcl_SetObjResult(interp, result);

    return TCL_OK;
}