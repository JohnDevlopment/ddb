#include "ddb.h"
#include "ddbint.h"
#include "memory.h"

#include <string.h>

void* Ddb_Alloc(size_t size, size_t count)
{
    void* ptr;

    if (! size || ! count)
        return NULL;

    /* Allocate and initialize memory. */
    ptr = (count > 1) ? calloc(count, size) : malloc(size);
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

    /* Reallocate pointer. */
    result = realloc(ptr, size * count);
    DDB_ASSERT(result, "ran out of memory\n");

    return result;
}

#ifdef DDB_DEBUG
void Ddb_Free(void* ptr)
{
    printf("Free memory at %p\n", ptr);
    free(ptr);
}
#endif
