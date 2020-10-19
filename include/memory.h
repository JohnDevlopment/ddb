#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#include "begin_code.h"

enum TclValueType {
    DDB_TCL_NULL_VALUE,
    DDB_TCL_INT_VALUE,
    DDB_TCL_LONG_VALUE,
    DDB_TCL_DOUBLE_VALUE,
    DDB_TCL_STRING,
    DDB_TCL_OBJECT,
    DDB_TCL_POINTER_VALUE
};

/* Type of the value being reference by the pointer. */
typedef enum PointerObjType {
    UNSET = -1,
    FILE_HEADER,
    FILE_COLUMN_HEADER
} PointerObjType;

/* Data heled by a pointer object. */
typedef struct PointerObj {
    void*          ptr;
    PointerObjType type;
} PointerObj;

#include "close_code.h"

#endif /* MEMORY_H */
