#ifndef __GLOBAL_
#define __GLOBAL_

#include <windows.h>

#include "file_buffer.h"
#include "screen_buffer.h"
#include "font.h"

//struct containing all static WNDPROC variables
typedef struct GlobalStruct
{
    ScreenBuffer screenBuffer;      //struct with screen information
    FileBuffer fileBuffer;          //struct with file information
    FontInfo fontInfo;              //struct with font information

    BOOL fileLoaded;                //flag that is true if file was loaded and fileContent is filled
} GlobalStruct;


#endif // __GLOBAL_

