#ifndef __SCREEN_BUFFER_
#define __SCREEN_BUFFER_

#include "file_buffer.h"
#include "font.h"

//struct of information about screen and text
typedef struct ScreenBuffer
{
    char** lines;                   //array of pointers on lines beginnings
    int linesCount;                 //count of lines (pointers in array)
    int linesMaxLen;                //maximum length of line on the screen

    int height;                     //window width in pixels
    int width;                      //window height in pixels
    int heightSymbols;              //the number of lines to fit in the window
    int widthSymbols;               //the number of characters that fit in the line

    int hScrollPos;                 //horizontal scroll position
    int vScrollPos;                 //vertical scroll position
    int hScrollLimit;               //horizontal scroll limit
    int vScrollLimit;               //vertical scroll limit

    int xCaretPos;                  //horizontal caret position
    int yCaretPos;                  //vertical caret position

    BOOL hasHScroll;                //flag that is true if mode without splitting lines selected
} ScreenBuffer;

void InitScreenBuffer(ScreenBuffer* screenBuffer);

void ChangeVScroll(HWND hwnd, ScreenBuffer* screenBuffer, int delta);
void ChangeHScroll(HWND hwnd, ScreenBuffer* screenBuffer, int delta);

void SetVScroll(HWND hwnd, ScreenBuffer* screenBuffer, int position);
void SetHScroll(HWND hwnd, ScreenBuffer* screenBuffer, int position);

void ChangeXCaretPos(HWND hwnd, ScreenBuffer* screenBuffer, int delta);
void ChangeYCaretPos(HWND hwnd, ScreenBuffer* screenBuffer, int delta);

void ResizeScreenBuffer(HWND hWnd, ScreenBuffer* screenBuffer, FileBuffer* fileBuffer, FontInfo* fontInfo);

void DrawScreenBuffer(HWND hwnd, ScreenBuffer* screenBuffer, FontInfo* fontInfo);

void ClearScreenBuffer(ScreenBuffer* screenBuffer);

#endif // __SCREEN_BUFFER_
