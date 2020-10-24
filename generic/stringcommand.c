#include "ddb.h"
#include "ddbint.h"
#include "memory.h"
#include "ddbstring.h"

int StringSubcommand_Charcount(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
{
    Tcl_Obj* result = NULL;
    const char *c, *str;

    if (objc != 2)
    {
        DDB_SET_STRING_RESULT(interp, DDB_COMMAND_WRONG_ARG_MESSAGE("_string charcount c str"));
        return TCL_ERROR;
    }

    /* Get arguments. */
    c   = Tcl_GetStringFromObj(objv[0], NULL);
    str = Tcl_GetStringFromObj(objv[1], NULL);

    DDB_ASSERT(*c != '\0' && *str != '\0', "invalid parameters: either 'c' or 'str' are empty strings");

    result = Tcl_NewIntObj((int) Ddb_strchrcnt(str, *c));
    Tcl_SetObjResult(interp, result);

    return TCL_OK;
}
