#ifndef DDB_H
#define DDB_H

/*
This is the public export header for ddb (Dewpsi Database).
This is a Tcl extension compiled with Tcl stub 8.6. This extension
was created by John Russell.
*/

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <tcl.h>
#include <cassert>

#include "begin_code.h"

/*
==========================================================
*/

struct __DDB_FileHeader {
    uint32_t id;
    uint64_t numEntries;
    uint64_t blockSize;
    uint64_t numColumns;
    uint64_t columnSize;
    uint64_t offset;
};

typedef struct __DDB_FileHeader DDB_FileHeader;

#include "close_code.h"

#endif /* DDB_H */
