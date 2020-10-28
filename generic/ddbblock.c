#include "ddbblock.h"
#include "ddbint.h"

void Ddb_InitFileHeader(DDB_FileHeader* p)
{
    p->id         = DDB_FILE_ID;
    p->numEntries = 0;
    p->blockSize  = 0;
    p->numColumns = 0;
    p->offset     = 0;
}

void Ddb_UpdateFileHeader(DDB_FileHeader* headPtr, const DDB_ColumnHeader* colPtr, size_t cols)
{
    size_t blockSize = 0;

    DDB_TRACE_PRINT_FUNCTION();
    DDB_ASSERT(headPtr && colPtr, "null pointer argument");

    headPtr->numColumns = cols;

    /* Sum of column lengths. */
    for (size_t i = 0; i < cols; ++i)
        blockSize += colPtr[i].length;

    headPtr->blockSize = blockSize;
    headPtr->offset    = sizeof(DDB_FileHeader) + blockSize;

    DDB_TRACE_PRINTF("record size: %u bytes\noffset of first record: %u\n",
        headPtr->blockSize, headPtr->offset);

    DDB_TRACE_PRINTF("number of columns: %u\n", headPtr->numColumns);
}
