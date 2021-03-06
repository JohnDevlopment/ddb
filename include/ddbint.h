#ifndef DDB_INT_H
#define DDB_INT_H

#include <stdint.h>

#include "begin_code.h"

enum TclValueType;

enum DdbBlockType { DDB_FILE_HEADER, DDB_COLUMN_HEADER, DDB_RECORD, DDB_ANY };

/* Helper macros */

/* Print an error message. */
#define DDB_EPRINTF(...) fprintf(stderr, "DDB Error: " __VA_ARGS__)

/* Debug trace messages. */
#ifdef DDB_DEBUG
# define DDB_TRACE_PRINTF(...) printf(__VA_ARGS__)
#else
# define DDB_TRACE_PRINTF(...)
#endif

/* Prints the name of the calling function. */
#define DDB_TRACE_PRINT_FUNCTION() DDB_TRACE_PRINTF("%s\n", __FUNCTION__);

/*
DDB_JUMP_SUBCOMMAND - Call subcommand
    Called with a DdbCommand_* function to access a subcommand. For example, it is used
    within DdbCommand_Ddb to implement "ddb init" as DdbSubcommand_Init. The ANSI C
    prototype for the subcommand is as follows:
    int DdbSubcommand_NAME(ClientData, Tcl_Interp*, int, Tcl_Obj* const*);

    For an example of how this is used, see DdbCommand_Ddb.
*/
#define DDB_JUMP_SUBCOMMAND(cmd, subcmd) cmd##Subcommand_##subcmd(cd, interp, objc - 2, objv + 2)

/*
DDB_SUBCOMMAND_WRONG_ARGS - Generate a "wrong argument" error
    Generates an error message indicating the wrong number of arguments for a subcommand. It calls
    Tcl_WrongNumArgs which generates the standard "wrong # args" messages you're use to seeing.
    It assumes the caller has a local 'interp' variable (Tcl_Interp*), and the objc and objv
    variables seen in all Tcl_ObjCommandProc's.

    Because this is meant to be called in a subcommand, this assumes that the objv array is offset
    by two bytes, in which case it deoffsets objv and instructs Tcl_WrongNumArgs to obtain the first
    two arguments in objv. Supposing we were in DdbSubcommand_Init, the following message could be
    generated: "wrong # args: should be ddb init".
*/
#define DDB_SUBCOMMAND_WRONG_ARGS(msg) \
    Tcl_WrongNumArgs(interp, 2, objv-2, msg)

/*
DDB_SET_STRING_RESULT - Set string result for interpreter
    Sets the string denoted in STR as the result of the interpreter INTERP. It calls
    Ddb_SetResult with the appropriate arguments to make this work.
*/
#define DDB_SET_STRING_RESULT(interp, str) \
    do { \
        SString setStringResultStruct = Ddb_NewSString(str); \
        Ddb_SetResult(interp, DDB_TCL_STRING, (void*) &setStringResultStruct); \
    } while(0)

#define DDB_PRINTF_RESULT(interp, ...) \
    do { \
        Tcl_Obj* ddbPrintfResultPtr = Tcl_ObjPrintf(__VA_ARGS__); \
        Tcl_SetObjResult(interp, ddbPrintfResultPtr); \
    } while (0)

#if 0
#define DDB_SET_INT_RESULT(interp, val) \
    do { \
        int setIntResultVar = val; \
        Ddb_SetResult(interp, DDB_TCL_INT_VALUE, &setIntResultVar); \
    } while(0)

#define DDB_SET_LONG_RESULT(interp, val) \
    do { \
        int setLongResultVar = val; \
        Ddb_SetResult(interp, DDB_TCL_LONG_VALUE, &setLongResultVar); \
    } while(0)
#endif

/* Module functions */

/* Sets the result of INTERP with the type specified in TYPE. Accepts a pointer to the value. */
void Ddb_SetResult(Tcl_Interp* interp, enum TclValueType type, const void* ptr);

/* Get the "ddbblock" type pointer. */
const Tcl_ObjType* Ddb_GetObjType();

#include "close_code.h"

#endif /* DDB_INT_H */
