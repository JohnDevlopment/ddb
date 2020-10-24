#include "ddb.h"
#include "ddbint.h"
#include "ddbstring.h"
#include "memory.h"
#include "ddbcommand.h"
#include "stringcommand.h"

static int DdbCommand_Ddb(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]);
static int DdbCommand_String(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]);

/* Minimum required Tcl version as a string. */
static const char* minTclVersion = "8.6";

static Tcl_Command DdbCommand;

/* Actual Tcl version. */
static const char* tclVersion = NULL;

/* Custom Tcl value type structure. */
static Tcl_ObjType tclBlockType;

#define DdbDeleteProc_String DdbDeleteProc_Ddb

static void DdbDeleteProc_Ddb(ClientData cd)
{
    Ddb_FreeString((DString*) cd);
}

int Ddb_Init(Tcl_Interp* interp)
{
    char commandName[20];

    /* Initialize stub library. */
    tclVersion = Tcl_InitStubs(interp, minTclVersion, 0);
    if (tclVersion == NULL)
    {
        DDB_EPRINTF("failed to initialize ddb.\n");
        return TCL_ERROR;
    }

    /* Create and register new type. */
    tclBlockType.name = "block";
    tclBlockType.setFromAnyProc = NULL;
    tclBlockType.updateStringProc = NULL;
    tclBlockType.dupIntRepProc = NULL;
    tclBlockType.freeIntRepProc = NULL;
    Tcl_RegisterObjType(&tclBlockType);

    /* Register commands. */
    strcpy(commandName, "ddb");
    DdbCommand = Tcl_CreateObjCommand(interp, commandName, DdbCommand_Ddb,
        (ClientData) Ddb_AllocString(NULL, 50), DdbDeleteProc_Ddb);
    if (! DdbCommand)
    {
        DDB_EPRINTF("failed to create object command\n");
        return TCL_ERROR;
    }

    strcpy(commandName, "_string");
    DdbCommand = Tcl_CreateObjCommand(interp, commandName, DdbCommand_String,
        (ClientData) Ddb_AllocString(NULL, 50), DdbDeleteProc_String);
    if (! DdbCommand)
    {
        DDB_EPRINTF("failed to create object command\n");
        return TCL_ERROR;
    }

    DDB_TRACE_PRINTF("Tcl version %s; load module ddb. Command is ddb. 'ddb list' for a list of subcommands.\n",
        tclVersion);

    return TCL_OK;
}

/* Internal functions */

int DdbCommand_String(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
{
    const char* subcommand;
    long int hash;

    if (objc < 2)
    {
        DDB_SET_STRING_RESULT(interp, DDB_SUBCOMMAND_WRONG_ARG_MESSAGE("_string"));
        return TCL_ERROR;
    }

    /* Subcommand: cap its length to 30 characters. */
    subcommand = Tcl_GetStringFromObj(objv[1], NULL);
    if (strlen(subcommand) > 30)
    {
        DDB_SET_STRING_RESULT(interp, "invalid 'subcommand' argument: exceeds 30 character cap");
        return TCL_ERROR;
    }

    hash = (long) Ddb_Hash(subcommand);

    switch (hash)
    {
        case 0x377C42B34F62E8C: /* charcount */
            return DDB_JUMP_SUBCOMMAND(String, Charcount);

        default:
        {
            const SString fmt = Ddb_NewSString("unknown or ambiguous subcommand \"%s\": must be charcount");
            DString* dstr = (DString*) cd;
            /* FMT length - 2 (replacing "%s") + SUBCOMMAND length + 1. */
            dstr = Ddb_AllocStringIfNeeded(dstr, fmt.length + strlen(subcommand) - 1);
            sprintf(dstr->ptr, fmt.ptr, subcommand);
            DDB_SET_STRING_RESULT(interp, dstr->ptr);
            break;
        }
    }

    return TCL_ERROR;
}

int DdbCommand_Ddb(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
{
    const char* subcommand;
    long int hash;

    if (objc < 2)
    {
        DDB_SET_STRING_RESULT(interp, DDB_SUBCOMMAND_WRONG_ARG_MESSAGE("ddb"));
        return TCL_ERROR;
    }

    /* Subcommand: cap its length to 30 characters. */
    subcommand = Tcl_GetStringFromObj(objv[1], NULL);
    if (strlen(subcommand) > 30)
    {
        DDB_SET_STRING_RESULT(interp, "invalid 'subcommand' argument: exceeds 30 character cap");
        return TCL_ERROR;
    }

    hash = (long) Ddb_Hash(subcommand);

    switch (hash)
    {
        case 0x17C97C2C9L: /* "hash" */
            return DDB_JUMP_SUBCOMMAND(Ddb, Hash);

        case 0x17C9A1661L: /* "list" */
            DDB_SET_STRING_RESULT(interp, "subcommands: hash list");
            return TCL_OK;

        default:
        {
            const SString fmt = Ddb_NewSString("unknown or ambiguous subcommand \"%s\": must be hash list");
            DString* dstr = (DString*) cd;
            /* FMT length - 2 (replacing "%s") + SUBCOMMAND length + 1. */
            dstr = Ddb_AllocStringIfNeeded(dstr, (size_t) fmt.length + strlen(subcommand) - 1);
            sprintf(dstr->ptr, fmt.ptr, subcommand);
            DDB_SET_STRING_RESULT(interp, dstr->ptr);
            break;
        }
    }

    return TCL_ERROR;
}

void Ddb_SetResult(Tcl_Interp* interp, enum TclValueType type, const void* ptr)
{
    Tcl_Obj* result = NULL;

    DDB_ASSERT(interp, "'interp' parameter not set\n");

    switch (type)
    {
        case DDB_TCL_INT_VALUE:
        {
            const int* temp = (const int*) ptr;
            result = Tcl_NewIntObj(*temp);
            break;
        }

        case DDB_TCL_LONG_VALUE:
        {
            const long* temp = (const long*) ptr;
            result = Tcl_NewLongObj(*temp);
            break;
        }

        case DDB_TCL_DOUBLE_VALUE:
        {
            const double* temp = (const double*) ptr;
            result = Tcl_NewDoubleObj(*temp);
            break;
        }

        case DDB_TCL_STRING:
        {
            const SString* temp = (const SString*) ptr;
            result = Tcl_NewStringObj(temp->ptr, temp->length);
            break;
        }
    }

    if (result)
        Tcl_SetObjResult(interp, result);
}
