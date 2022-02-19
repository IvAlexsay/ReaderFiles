#ifndef __FILE_BUFFER_
#define __FILE_BUFFER_

#include <windows.h>

//struct of information about readed file
typedef struct FileBuffer
{
    char* text;                     //text readed from file
    int size;                       //text size in chars
    char** lines;                   //array of pointers on every line in file
    int linesCount;                 //count of lines ('\n') in file
    int linesMaxLen;                //maximum length of line in file
} FileBuffer;

void InitFileBuffer(FileBuffer* fileBuffer);

BOOL ReadToFileBuffer(const char* filename, FileBuffer* fileBuffer);

void ClearFileBuffer(FileBuffer* fileBuffer);


#endif // __FILE_BUFFER_

