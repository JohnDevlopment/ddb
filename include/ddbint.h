#ifndef DDB_INT_H
#define DDB_INT_H

#include <stdint.h>

#include "begin_code.h"

#define DDB_EPRINTF(...) fprintf(stderr, "DDB Error: " __VA_ARGS__)

#ifdef DDB_DEBUG
# define DDB_TRACE_PRINTF(...) printf(__VA_ARGS__)
#else
# define DDB_TRACE_PRINTF(...)
#endif

#define DDB_ALLOC(type, count)      (type *) Ddb_Alloc(sizeof(type), count)
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

#include "close_code.h"

#endif /* DDB_INT_H */
