#include "ddb.h"
#include "ddb-int.h"

static const char* tclVersion = NULL;

int Ddb_Init(Tcl_Interp* interp)
{
    /* Initialize stub library. */
    tclVersion = Tcl_InitStubs(interp, "0.1", 0);
    if (tclVersion == NULL)
    {
        DDB_EPRINTF("failed to initialize ddb.");
        return TCL_ERROR;
    }

    return TCL_OK;
}
