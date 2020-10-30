#ifndef DDBBLOCK_H
#define DDBBLOCK_H

#include "ddb.h"

/* Initialize file header. */
void Ddb_InitFileHeader(DDB_FileHeader* p);

/* Update file header based on columns. */
void Ddb_UpdateFileHeader(DDB_FileHeader* headPtr, const DDB_ColumnHeader* colPtr, size_t cols);

/* Allocate a record struct based on size defined in header. */
DDB_Record* Ddb_NewRecordStruct(DDB_FileHeader* headPtr);

#endif /* DDBBLOCK_H */
