#include "ddbblock.h"
#include "ddbint.h"

#include <string.h>

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

    DDB_ASSERT(headPtr && colPtr, "null pointer argument");

    headPtr->numColumns = cols;

    /* Sum of column lengths. */
    for (size_t i = 0; i < cols; ++i)
        blockSize += colPtr[i].length;

    headPtr->blockSize = blockSize;
    headPtr->offset    = sizeof(DDB_FileHeader) + blockSize;
}

DDB_Record* Ddb_NewRecordStruct(DDB_FileHeader* headPtr)
{
    DDB_Record* result;
    size_t blockSize, structSize;

    DDB_ASSERT(headPtr, "null pointer argument");

    /* Size of record. */
    blockSize = headPtr->blockSize;
    structSize = sizeof(DDB_Record) + blockSize;

    /* Initialize data to zero. */
    result = ckalloc(structSize);
    result->structSize = structSize;
    result->blockSize  = blockSize;
    result->deleted    = 0;
    memset(result->data, 0, blockSize);

    return result;
}
