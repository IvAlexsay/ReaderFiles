#include "file_buffer.h"

#include <stdio.h>

/*
Print all FileBuffer fields to console
IN:
    FileBuffer* fileBuffer - file data struct
OUT:
    ---
*/
void ShowFileBufferContent(FileBuffer* fileBuffer)
{
    printf("\nFile buffer size: %d\n", fileBuffer->size);
    printf("File buffer linesCount: %d\n", fileBuffer->linesCount);
    printf("File buffer linesMaxLen: %d\n", fileBuffer->linesMaxLen);
}

/*
Zeros all FileBuffer fields
IN:
    FileBuffer* fileBuffer - file data struct
OUT:
    FileBuffer* fileBuffer - file data struct with zeroed values
*/
void InitFileBuffer(FileBuffer* fileBuffer)
{
    fileBuffer->text = NULL;
    fileBuffer->size = 0;
    fileBuffer->lines = NULL;
    fileBuffer->linesCount = 0;
    fileBuffer->linesMaxLen = 0;
}

/*
try open file and read it into FileBuffer
IN:
    const char* fileName - name of file to open and read
    FileBuffer* fileBuffer - struct to write file content
OUT:
    FileBuffer* fileBuffer - data-filled structure
RETURN:
    BOOL - return true if success, false if something went wrong
*/
BOOL ReadToFileBuffer(const char* fileName, FileBuffer* fileBuffer)
{
    if (fileName == NULL)
        return FALSE;

    FILE* file = fopen(fileName, "r");
    if (file == NULL)
        return FALSE;

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* text = (char*)malloc(sizeof(char) * (size + 1));
    if (text == NULL)
    {
        fclose(file);
        return FALSE;
    }

    size = fread(text, sizeof(char), size, file);
    text[size] = '\0';

    int linesCount = 1;
    for (unsigned long i = 0; i < size; i++)
    {
        if (text[i] == '\n')
            linesCount++;
    }

    char** lines = (char**)malloc(sizeof(char*) * linesCount);
    if (lines == NULL)
    {
        free(text);
        return FALSE;
    }

    lines[0] = text;
    int index = 1;
    for (int i = 0; i < size; i++)
    {
        if (text[i] == '\n')
        {
            lines[index++] = text + i + 1;
            text[i] = '\0';
        }
    }

    int linesMaxLen = 0;
    for (int i = 0; i < linesCount; i++)
    {
        int len = strlen(lines[i]);
        if (len > linesMaxLen)
            linesMaxLen = len;
    }

    fclose(file);

    fileBuffer->text = text;
    fileBuffer->lines = lines;
    fileBuffer->linesCount = linesCount;
    fileBuffer->size = size;
    fileBuffer->linesMaxLen = linesMaxLen;

    return TRUE;
}

/*
Zeros all FileBuffer fields and frees memory
IN:
    FileBuffer* fileBuffer - file data struct
OUT:
    FileBuffer* fileBuffer - file data struct with zeroed values
*/
void ClearFileBuffer(FileBuffer* fileBuffer)
{
    if (fileBuffer == NULL)
        return;

    if (fileBuffer->text != NULL)
        free(fileBuffer->text);
    if (fileBuffer->lines != NULL)
        free(fileBuffer->lines);

    fileBuffer->text = NULL;
    fileBuffer->lines = NULL;
    fileBuffer->size = 0;
    fileBuffer->linesCount = 0;
    fileBuffer->linesMaxLen = 0;
}
