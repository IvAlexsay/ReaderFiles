#ifndef __FONT_
#define __FONT_

#include <windows.h>

//struct of information about font
typedef struct FontInfo
{
    HFONT hFont;                    //font handle
    int fontHeight;                 //height of character in pixels
    int fontWidth;                  //width of character in pixels
} FontInfo;

void InitFontInfo(HWND hWnd, FontInfo* fontInfo);

void ClearFontInfo(FontInfo* fontInfo);    //need to do this


#endif // __FONT_
