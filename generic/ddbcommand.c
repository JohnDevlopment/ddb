#include "ddb.h"
#include "ddbint.h"
#include "ddbstring.h"
#include "ddbblock.h"
#include "ddbblock_type.h"
#include "memory.h"

#include <alloca.h>
#include <stdbool.h>

enum ValueCopyType { COPY_INT = 'I', COPY_DOUBLE = 'D', COPY_STRING = 'S' };

static const size_t ColumnNameMemberLength = DDB_SIZEOF_MEMBER(DDB_ColumnHeader, name);

static void PrintFormattedString(const char* prefix, const char* str, int type)
{
    DDB_ASSERT(prefix && *prefix != '\0', "invalid string argument 1\n");
    DDB_ASSERT(str && *str != '\0', "invalid string argument 2\n");

    switch (type)
    {
        case COPY_INT:
        {
            const int* temp_ptr = (const int*) str;
            printf("%s: %d\n  ", prefix, *temp_ptr);
            break;
        }

        case COPY_DOUBLE:
        {
            const double* temp_ptr = (const double*) str;
            printf("%s: %f\n  ", prefix, *temp_ptr);
            break;
        }

        case COPY_STRING:
            printf("%s: %s\n  ", prefix, str);
            break;
    }
}

static char* CopyValueToBuffer(char* dataPtr, int length, Tcl_Obj* objPtr, int type)
{
    int temp_int;
    double temp_double;
    void* copyBuffer = NULL;
    unsigned char ucType = (unsigned char) type;

    switch (ucType)
    {
        case COPY_INT:
            if (Tcl_GetIntFromObj(NULL, objPtr, &temp_int) == TCL_OK)
                copyBuffer = &temp_int;
            break;

        case COPY_DOUBLE:
            if (Tcl_GetDoubleFromObj(NULL, objPtr, &temp_double) == TCL_OK)
                copyBuffer = &temp_double;
            break;

        case COPY_STRING:
            copyBuffer = Tcl_GetStringFromObj(objPtr, NULL);
            break;
    }

    DDB_ASSERT(copyBuffer, "copyBuffer is NULL\n");
    memcpy(dataPtr, copyBuffer, (size_t) length);

    return dataPtr + length;
}

static int CheckObjectType(register Tcl_Obj* objPtr, enum DdbBlockType prefType)
{
    uint16_t objType;

    if (objPtr->typePtr != Ddb_GetObjType())
        return false;

    objType = DDB_LOWORD(BLOCK_INT_REP(objPtr).value);

    if (prefType != DDB_ANY)
    {
        if (objType != (uint16_t) prefType)
            return false;
    }

    return true;
}

int DdbSubcommand_Hash(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
{
    Tcl_Obj* result = NULL;
    const char* param = NULL;

    if (objc != 1)
    {
        /*DDB_SET_STRING_RESULT(interp, "wrong # args: should be ddb hash aString");*/
        DDB_SUBCOMMAND_WRONG_ARGS(DdbEmptyString);
        return TCL_ERROR;
    }

    /* Retrieve string. Return if empty. */
    param = Tcl_GetStringFromObj(objv[0], NULL);
    if (! strlen(param))
        return TCL_OK;
    else
    {
        /* Calculate hash of string. Place it in long value. */
        char buffer[19];
        unsigned long hash = Ddb_Hash(param);
        int length;

        /* Format string. */
        sprintf(buffer, "0x%lx", (long) hash);
        length = (int) strlen(buffer);

        /* Wrap string in object. */
        result = Tcl_NewStringObj(buffer, length);
        Tcl_IncrRefCount(result);
    }

    if (result)
        Tcl_SetObjResult(interp, result);

    return TCL_OK;
}

int DdbSubcommand_Init(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
{
    Tcl_Obj* result;
    DDB_FileHeader* block;

    if (objc)
    {
        /*DDB_SET_STRING_RESULT(interp, DDB_COMMAND_WRONG_ARG_MESSAGE("ddb init"));*/
        DDB_SUBCOMMAND_WRONG_ARGS(DdbEmptyString);
        return TCL_ERROR;
    }

    /* Allocate block. */
    /*block = (DDB_FileHeader*) Tcl_Alloc(sizeof(DDB_FileHeader));*/
    block = DDB_ALLOC(DDB_FileHeader, 1);
    if (! block)
    {
        DDB_SET_STRING_RESULT(interp, "failed to allocate pointer");
        return TCL_ERROR;
    }
    Ddb_InitFileHeader(block);

    /* New object, place pointer in there. */
    result = Tcl_NewObj();
    DDB_ASSERT(result->refCount == 0, "result pointer refcount is nonzero after creation: %d\n",
        result->refCount);
    /*Tcl_IncrRefCount(result);*/

    /* Set internal rep. */
    BLOCK_INT_REP(result).ptr   = block;
    BLOCK_INT_REP(result).value = DDB_FILE_HEADER;

    /* Set type of object. Set as result of function. Give it a valid string rep. */
    result->typePtr = Ddb_GetObjType();
    DDB_ASSERT(result->typePtr, "type pointer NULL");
    Tcl_InvalidateStringRep(result);
    Tcl_SetObjResult(interp, result);

    return TCL_OK;
}

int DdbSubcommand_Columns(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
{
    Tcl_Obj* headObj, **elements, *result;
    DDB_ColumnHeader* columns;
    int length, code;

    if (objc != 2)
    {
        DDB_SUBCOMMAND_WRONG_ARGS("fileHeader columns");
        return TCL_ERROR;
    }

    headObj = objv[0];

    /* Invalid block type. */
    if (! CheckObjectType(headObj, DDB_FILE_HEADER))
    {
        DDB_SET_STRING_RESULT(interp, "first argument is not a valid file header");
        return TCL_ERROR;
    }

    /* Extrapolate list elements from argument 2. */
    code = Tcl_ListObjGetElements(interp, objv[1], &length, &elements);

    if (code != TCL_OK)
        return TCL_ERROR;

    /* Zero-length list. */
    if (! length)
    {
        DDB_SET_STRING_RESULT(interp, "zero length list argument");
        return TCL_ERROR;
    }

    /* Object result with the column headers. */
    columns = DDB_ALLOC(DDB_ColumnHeader, length);
    result  = Tcl_NewObj();

    BLOCK_INT_REP(result).ptr   = columns;
    BLOCK_INT_REP(result).value = DDB_PACK_DWORD(length, DDB_COLUMN_HEADER);

    result->typePtr = Ddb_GetObjType();
    DDB_ASSERT(result->typePtr, "NULL type pointer\n");
    Tcl_IncrRefCount(result);

    /* Extract elements from list argument. */
    for (int i = 0; i < length; ++i)
    {
        Tcl_Obj** subelements; /* Each element is itself a list, hence subelements. */
        int sublen; /* Length of each sublist. */

        /* Attempt to extrapolate elements from this sublist. */
        code = Tcl_ListObjGetElements(interp, elements[i], &sublen, &subelements);
        if (code != TCL_OK) break;

        /* Error if not exactly 3 count. */
        if (sublen != 3)
        {
            DDB_PRINTF_RESULT(interp, "malformed list; sublist %d must be 3 elements", i);
            return TCL_ERROR;
        }

        strncpy(columns[i].name, Tcl_GetStringFromObj(subelements[0], NULL), sizeof(columns[i].name));
        {
            int temp_int;
            const char* temp_str = Tcl_GetStringFromObj(subelements[2], NULL);
            code = Tcl_GetIntFromObj(interp, subelements[1], &temp_int);
            if (code != TCL_OK) break;
            columns[i].length = (uint32_t) temp_int;
            columns[i].type = *temp_str;
        }
    }

    /* Update file header based on the columns. */
    Ddb_UpdateFileHeader(DDB_STATIC_CAST(DDB_FileHeader*, BLOCK_INT_REP(headObj).ptr),
        columns, length);

    if (code == TCL_OK)
    {
        Tcl_SetObjResult(interp, result);
        Tcl_DecrRefCount(result);
        Tcl_InvalidateStringRep(result);
    }

    return code;
}

int DdbSubcommand_Print(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
{
    DDB_DWord ddbTypeValue;
    DDB_ColumnHeader* colheader;
    int index = -1;
    char isRecord = CheckObjectType(objv[0], DDB_RECORD);

    if (objc != 1 && objc != 3)
    {
        DDB_SUBCOMMAND_WRONG_ARGS("val ?columnHeader index?");
        return TCL_ERROR;
    }

    /* If first argument is a record, three arguments are required. */
    if (isRecord && objc == 1)
    {
        DDB_SUBCOMMAND_WRONG_ARGS("record columnHeader index");
        return TCL_ERROR;
    }

    /* Accept column header and index. */
    if (objc == 3)
    {
        if (Tcl_GetIntFromObj(interp, objv[2], &index) != TCL_OK)
            return TCL_ERROR;

        /* First argument must be a record. */
        if (! isRecord)
        {
            DDB_SET_STRING_RESULT(interp, "first argument is not a DDB record block");
            return TCL_ERROR;
        }

        /* Second argument, if provided, must be a column header */
        if (! CheckObjectType(objv[1], DDB_COLUMN_HEADER))
        {
            DDB_SET_STRING_RESULT(interp, "second argument is not a ddb column header");
            return TCL_ERROR;
        }
        colheader = BLOCK_INT_PTR(DDB_ColumnHeader*, objv[1]);
    }
    else
    {
        /* Check that first argument is a valid DDB block */
        if (! CheckObjectType(objv[0], DDB_ANY))
        {
            DDB_SET_STRING_RESULT(interp, "first argument is not a DDB block");
            return TCL_ERROR;
        }
    }

    DDB_UNPACK_DWORD(ddbTypeValue, BLOCK_INT_REP(objv[0]).value);

    switch (ddbTypeValue.lo)
    {
        case DDB_FILE_HEADER:
        {
            DDB_FileHeader* temp_ptr = BLOCK_INT_PTR(DDB_FileHeader*, objv[0]);
            printf("number of entries: %u\nblock size: %u\nnumber of columns: %u\n",
                temp_ptr->numEntries, temp_ptr->blockSize, temp_ptr->numColumns);
            printf("offset to first entry: %u\n", temp_ptr->offset);
            break;
        }

        case DDB_COLUMN_HEADER:
        {
            DDB_ColumnHeader* temp_ptr = BLOCK_INT_PTR(DDB_ColumnHeader*, objv[0]);
            char buffer[50];
            for (uint16_t i = 0; i < ddbTypeValue.hi; ++i)
            {
                strcpy(buffer, temp_ptr[i].name);
                printf("column %u\n  name: %s\n  length: %u bytes\n  type: %c\n\n",
                    i, buffer, temp_ptr[i].length, temp_ptr[i].type);
            }
            break;
        }

        case DDB_RECORD:
        {
            const char* recordData;
            const uint16_t numColumns = DDB_HIWORD(BLOCK_INT_REP(objv[1]).value);
            DString* buffer = Ddb_AllocString(NULL, 50);

            if (index < 0 || index >= (int) ddbTypeValue.hi)
            {
                DDB_PRINTF_RESULT(interp, "invalid index %d", index);
                return TCL_ERROR;
            }

            /* Get a pointer to the record data. */
            {
                DDB_Record* temp_record =
                    DDB_STATIC_CAST(DDB_Record**, BLOCK_INT_REP(objv[0]).ptr)[index];
                recordData = temp_record->data;
            }

            printf("record %d\n  ", index);
            for (uint16_t i = 0; i < numColumns; ++i)
            {
                char colname[ColumnNameMemberLength + 1];
                /* Copy column name to buffer. */
                strncpy(colname, colheader->name, ColumnNameMemberLength);

                /* Copy column data to dynamic string buffer. */
                buffer = Ddb_AllocStringIfNeeded(buffer, colheader->length);
                Ddb_CopyString(buffer, recordData, colheader->length);

                /* Print buffer to string in specific format according to field type. */
                PrintFormattedString(colname, buffer->ptr, colheader->type);

                /* Advance string pointer. */
                recordData += colheader->length;
            }
            printf("end of record\n");

            Ddb_FreeString(buffer);

            break;
        }

        default:
            DDB_SET_STRING_RESULT(interp, "unknown object type");
            return TCL_ERROR;
    }

    return TCL_OK;
}

int DdbSubcommand_Records(ClientData cd, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
{
    const int listCount = objc - 2;
    int code = TCL_OK;
    DDB_FileHeader* fileHeader;
    DDB_Record** records;
    Tcl_Obj* result;
    Tcl_Obj* const* curArg;

    if (objc < 3)
    {
        DDB_SUBCOMMAND_WRONG_ARGS("fileHeader columnHeaders record ?record ...?");
        return TCL_ERROR;
    }

    /* Check arguments 1 and 2 on whether they're the correct type. */
    if (! CheckObjectType(objv[0], DDB_FILE_HEADER))
    {
        DDB_PRINTF_RESULT(interp, "\"%s\" is not a valid file header",
            Tcl_GetStringFromObj(objv[0], NULL));
        return TCL_ERROR;
    }
    if (! CheckObjectType(objv[1], DDB_COLUMN_HEADER))
    {
        DDB_PRINTF_RESULT(interp, "\"%s\" is not a valid column header",
            Tcl_GetStringFromObj(objv[1], NULL));
        return TCL_ERROR;
    }

    /* Retrieve file header. */
    fileHeader = (DDB_FileHeader*) BLOCK_INT_REP(objv[0]).ptr;
    DDB_ASSERT(fileHeader, "NULL pointer\n");

    /* RECORDS is a pointer to an array of pointers, each holding an individual record. */
    records = DDB_ALLOC(DDB_Record*, DDB_STATIC_CAST(size_t, listCount));

    /* Wrap RECORD in Tcl object and initialize. */
    result = Tcl_NewObj();
    Tcl_IncrRefCount(result);
    result->typePtr             = Ddb_GetObjType();
    BLOCK_INT_REP(result).ptr   = (void*) records;
    BLOCK_INT_REP(result).value = DDB_PACK_DWORD(listCount, DDB_RECORD);

    /* Fill the records with their corresponding data in the list argument. */
    curArg = objv + 2;

    for (int i = 0; i < listCount; ++i)
    {
        const uint32_t numColumns = fileHeader->numColumns;
        Tcl_Obj** elements;
        DDB_Record* temp_record;
        char* dataPtr;

        /* Break argument down into a list. */
        {
            int length;
            code = Tcl_ListObjGetElements(interp, *curArg++, &length, &elements);
            if (code != TCL_OK) /* Failed to construct list from string. */
                break;

            /* List does not include all the column fields. */
            if ((size_t) length < numColumns)
            {
                DDB_PRINTF_RESULT(interp, "arg %d is %d length, should be %d\n",
                    i + 4, length, (int) numColumns);
                code = TCL_ERROR;
                break;
            }
        }

        /* Copy arguments to data buffer. */
        temp_record = Ddb_NewRecordStruct(fileHeader);
        dataPtr = temp_record->data;
        for (uint32_t ii = 0; ii < numColumns; ++ii)
        {
            DDB_ColumnHeader* currentColumn =
                DDB_STATIC_CAST(DDB_ColumnHeader*, BLOCK_INT_REP(objv[1]).ptr) + ii;
            dataPtr = CopyValueToBuffer(dataPtr, currentColumn->length, *elements++,
                currentColumn->type);
        }

        records[i] = temp_record; /* Save to array. */
    }

    /* Update number of entries. */
    fileHeader->numEntries = listCount;

    /* Set as result of function. */
    if (code == TCL_OK)
    {
        Tcl_InvalidateStringRep(result);
        Tcl_SetObjResult(interp, result);
    }

    Tcl_DecrRefCount(result);

    return code;
}
