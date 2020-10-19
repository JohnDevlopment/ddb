#ifndef DDB_INT_H
#define DDB_INT_H

#include <stdint.h>

#include "begin_code.h"

enum TclValueType;

/* Module functions */

/* Print an error message. */
#define DDB_EPRINTF(...) fprintf(stderr, "DDB Error: " __VA_ARGS__)

/* Debug trace messages. */
#ifdef DDB_DEBUG
# define DDB_TRACE_PRINTF(...) printf(__VA_ARGS__)
#else
# define DDB_TRACE_PRINTF(...)
#endif

//DdbSubcommand_Hash(cd, interp, int objc - 2, objv + 2)

#define DDB_JUMP_SUBCOMMAND(cmd, subcmd) cmd##Subcommand_##subcmd(cd, interp, objc - 2, objv + 2)

/*
DDB_ALLOC - Allocate a block of memory
    This function returns a pointer to a block of memory that's large enough to hold
    COUNT instances of TYPE in continguous memory. If it returns NULL, that means no memory
    block large enough could be found.
*/
#define DDB_ALLOC(type, count) (type *) Ddb_Alloc(sizeof(type), count)

/*
DDB_REALLOC - Reallocate a block of memory
    Reallocate a block of memory using P as the source block. COUNT elements of type TYPE
    are allocated. Memory is carried over from the source block as far as it can fit in the
    new block of data. If the returned pointer points to an area of memory that's larger than
    the old one, the new memory is not initialized.
*/
#define DDB_REALLOC(p, type, count) (type *) Ddb_Realloc(p, sizeof(type), count)


#define DDB_ARRAY_SIZE(array)       (sizeof(array) / sizeof(array[0]))

#define DDB_SUBCOMMAND_WRONG_ARG_MESSAGE(cmd) \
    "wrong # args: should be " cmd " subcommand ?arg ...?"

#define DDB_COMMAND_WRONG_ARG_MESSAGE(cmdline) \
    "wrong # args: should be " cmdline

#define DDB_SET_STRING_RESULT(interp, str) \
    do { \
        SString setStringResultStruct = Ddb_NewSString(str); \
        Ddb_SetResult(interp, DDB_TCL_STRING, (void*) &setStringResultStruct); \
    } while(0)

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

/* Sets the result of INTERP with the type specified in TYPE. Accepts a pointer to the value. */
void Ddb_SetResult(Tcl_Interp* interp, enum TclValueType type, const void* ptr);


#include "close_code.h"

#endif /* DDB_INT_H */
