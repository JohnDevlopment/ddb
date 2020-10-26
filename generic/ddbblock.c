#include "ddbblock.h"

void Ddb_InitFileHeader(DDB_FileHeader* p)
{
    p->id         = DDB_FILE_ID;
    p->numEntries = 0;
    p->blockSize  = 0;
    p->numColumns = 0;
    p->columnSize = 0;
    p->offset     = 0;
}
