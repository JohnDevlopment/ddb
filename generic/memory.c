#include "ddb.h"
#include "ddbint.h"

#include <string.h>

void* Ddb_Alloc(size_t size, size_t count)
{
    void* ptr;

    if (! size || ! count)
        return NULL;

    /* Allocate and initialize memory. */
    ptr = ckalloc(size * count);
    DDB_ASSERT(ptr, "ran out of memory\n");
    if (ptr)
        memset(ptr, 0, size * count);

    return ptr;
}

void* Ddb_Realloc(void* ptr, size_t size, size_t count)
{
    void* result;

    DDB_ASSERT(size && count, "'size' and/or 'count' parameters are zero\n");

    /* Invalid parameters. */
    if (! size || ! count)
        return NULL;

    /* Allocate pointer. */
    if (! ptr)
        return Ddb_Alloc(size, count);

    /* Reallocate pointer. */
    result = ckrealloc(ptr, size * count);
    DDB_ASSERT(ptr, "ran out of memory\n");

    return ptr;
}
