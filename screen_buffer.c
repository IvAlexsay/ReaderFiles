#include "screen_buffer.h"

#include <stdio.h>
#include <limits.h>

/*
Print all ScreenBuffer fields to console
IN:
    ScreenBuffer* screenBuffer - screen data struct
OUT:
    ---
*/
void ShowScreenBufferContent(ScreenBuffer* screenBuffer)
{
    printf("\nScreen buffer lines count: %d\n", screenBuffer->linesCount);
    printf("Screen buffer linesMaxLen: %d\n", screenBuffer->linesMaxLen);
    printf("Screen buffer hasHScroll: %d\n", screenBuffer->hasHScroll);
    printf("Screen buffer height: %d\n", screenBuffer->height);
    printf("Screen buffer width: %d\n", screenBuffer->width);
    printf("Screen buffer heightSymbols: %d\n", screenBuffer->heightSymbols);
    printf("Screen buffer widthSymbols: %d\n", screenBuffer->widthSymbols);
    printf("Screen buffer vScrollPos: %d\n", screenBuffer->vScrollPos);
    printf("Screen buffer hScrollPos: %d\n", screenBuffer->hScrollPos);
    printf("Screen buffer vScrollLimit: %d\n", screenBuffer->vScrollLimit);
    printf("Screen buffer hScrollLimit: %d\n", screenBuffer->hScrollLimit);
    printf("Screen buffer xCaretPos: %d\n", screenBuffer->xCaretPos);
    printf("Screen buffer yCaretPos: %d\n", screenBuffer->yCaretPos);
}

/*
Zeros all ScreenBuffer fields
IN:
    ScreenBuffer* screenBuffer - screen data struct
OUT:
    ScreenBuffer* screenBuffer - screen data struct with zeroed values
*/
void InitScreenBuffer(ScreenBuffer* screenBuffer)
{
    screenBuffer->lines = NULL;
    screenBuffer->linesCount = 0;
    screenBuffer->linesMaxLen = 0;
    screenBuffer->height = 0;
    screenBuffer->width = 0;
    screenBuffer->heightSymbols = 0;
    screenBuffer->widthSymbols = 0;
    screenBuffer->hScrollPos = 0;
    screenBuffer->vScrollPos = 0;
    screenBuffer->vScrollLimit = 0;
    screenBuffer->hScrollLimit = 0;
    screenBuffer->xCaretPos = 0;
    screenBuffer->yCaretPos = 0;
    screenBuffer->hasHScroll = TRUE;
}

/*
Extract size of window working area and sets it in output-model struct
IN:
    HWND hwnd - window handle
    ScreenBuffer* screenBuffer - screen data struct
OUT:
    ScreenBuffer* screenBuffer - screen data struct with info about the sizes of the client window
*/
void SetWindowSizeScreenBuffer(HWND hwnd, ScreenBuffer* screenBuffer)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    screenBuffer->height = rect.bottom - rect.top;
    screenBuffer->width = rect.right - rect.left;
}

/*
Set size of window working area in symbols of chosen font
IN:
    ScreenBuffer* screenBuffer - screen data struct
    FontInfo* fontInfo - font info struct
OUT:
    ScreenBuffer* screenBuffer - screen data struct with info about the sizes in symbols of the client window
*/
void SetWindowSymbolsSizeScreenBuffer(ScreenBuffer* screenBuffer, FontInfo* fontInfo)
{
    screenBuffer->heightSymbols = screenBuffer->height / fontInfo->fontHeight;
    if (screenBuffer->heightSymbols == 0)
        screenBuffer->heightSymbols = 1;

    screenBuffer->widthSymbols = screenBuffer->width / fontInfo->fontWidth;
    if (screenBuffer->widthSymbols == 0)
        screenBuffer->widthSymbols = 1;
}

/*
Fills ScreenBuffer struct from FileBuffer struct
IN:
    ScreenBuffer* screenBuffer - screen data struct
    FileBuffer* fileBuffer - file data struct
OUT:
    ScreenBuffer* screenBuffer - filled ScreenBuffer struct
*/
void BuildScreenBuffer(FileBuffer* fileBuffer, ScreenBuffer* screenBuffer)
{
    //if there is a horizontal scroll, then just copy the entire contents of the structure fileBuffer to screenBuffer
    if (screenBuffer->hasHScroll == TRUE)
    {
        screenBuffer->linesCount = fileBuffer->linesCount;
        screenBuffer->linesMaxLen = fileBuffer->linesMaxLen;

        screenBuffer->lines = (char**)malloc(screenBuffer->linesCount * sizeof(char*));
        if (screenBuffer->lines == NULL)
            return;

        for (int i = 0; i < fileBuffer->linesCount; i++)
            screenBuffer->lines[i] = fileBuffer->lines[i];

        return;
    }

    //count the number of lines in screenBuffer
    screenBuffer->linesMaxLen = fileBuffer->linesMaxLen;
    screenBuffer->linesCount = 0;
    for (int i = 0; i < fileBuffer->linesCount; i++)
    {
        int len = strlen(fileBuffer->lines[i]);
        int lines = len / screenBuffer->widthSymbols;
        if (len % screenBuffer->widthSymbols != 0 || len == 0)
            lines++;

        screenBuffer->linesCount += lines;
    }

    //allocate memory for pointers to strings
    screenBuffer->lines = (char**)malloc(sizeof(char*) * screenBuffer->linesCount);
    if (screenBuffer->lines == NULL)
        return;

    //Set pointers to the beginning of lines in the structure based on the size of the screen
    int screenBufferLineIndex = 0;
    for (int i = 0; i < fileBuffer->linesCount; i++)
    {
        char* beginOfLine = fileBuffer->lines[i];
        char* currentChar = beginOfLine;

        while (*currentChar != '\0')
        {
            if (currentChar - beginOfLine < screenBuffer->widthSymbols)
                currentChar++;
            else
            {
                screenBuffer->lines[screenBufferLineIndex++] = beginOfLine;
                beginOfLine = currentChar;
            }
        }

        screenBuffer->lines[screenBufferLineIndex++] = beginOfLine;
    }
}

/*
Binary search for new vertical scroll position after resize
IN:
    char** lines - buffer of lines that we display
    int linesCount - number of lines
    char* firstSymbol - ptr to first displayed symbol
OUT:
    int vScrollPos - new correct vertical scroll position
*/
int SearchVScrollPos(char** lines, int linesCount, char* firstSymbol)
{
    int vScrollPos = 0;
    if (firstSymbol != NULL)
    {
        int right = linesCount;
        int left = 0;

        while (TRUE)
        {
            vScrollPos = (right + left) / 2;
            if (firstSymbol - lines[vScrollPos] > lines[vScrollPos + 1] - lines[vScrollPos] - 1)
                left = vScrollPos + 1;
            else if (lines[vScrollPos] - firstSymbol > 0)
                right = vScrollPos - 1;
            else
                break;
        }
    }
    return vScrollPos;
}

/*
Binary search for new caret position after resize
IN:
    ScreenBuffer* screenBuffer - screen data struct
    char* symbolAfterCaret - ptr to the symbol after caret
OUT:
    ScreenBuffer* screenBuffer - screen data struct with right position of caret
*/
void SearchCaretPos(ScreenBuffer* screenBuffer, char* symbolAfterCaret)
{
    int yCaretPos = 0;
    int xCaretPos = 0;
    if (symbolAfterCaret != NULL)
    {
        int right = screenBuffer->linesCount;
        int left = 0;

        while (TRUE)
        {
            yCaretPos = (right + left) / 2;
            if (symbolAfterCaret - screenBuffer->lines[yCaretPos] > screenBuffer->lines[yCaretPos + 1] - screenBuffer->lines[yCaretPos] - 1)
                left = yCaretPos + 1;
            else if (screenBuffer->lines[yCaretPos] - symbolAfterCaret > 0)
                right = yCaretPos - 1;
            else
                break;
        }
    }

    for (char* c = screenBuffer->lines[yCaretPos]; c < symbolAfterCaret; c++)
        xCaretPos++;

    screenBuffer->yCaretPos = yCaretPos;
    screenBuffer->xCaretPos = xCaretPos;
}

/*
Rebuilds screen data sctruct with new screen size
IN:
    HWND hWnd - window descriptor
    ScreenBuffer* screenBuffer - screen data struct
    FileBuffer* fileBuffer - file data struct
    FontInfo* fontInfo - font info struct
OUT:
    ScreenBuffer* screenBuffer - rebuilded screen sttuct
*/
void ResizeScreenBuffer(HWND hwnd, ScreenBuffer* screenBuffer, FileBuffer* fileBuffer, FontInfo* fontInfo)
{
    //remember information about the position of scrolls and caret
    int vScrollPos = screenBuffer->vScrollPos;
    int hScrollPos = screenBuffer->hScrollPos;
    int xCaretPos = screenBuffer->xCaretPos;
    int yCaretPos = screenBuffer->yCaretPos;

    char* firstSymbol = NULL;
    char* symbolAfterCaret = NULL;

    //position of the first symbol on the screen
    if (screenBuffer->lines != NULL)
        firstSymbol = screenBuffer->lines[vScrollPos] + hScrollPos;

    //position of symbol after caret
    if (screenBuffer->lines != NULL)
        symbolAfterCaret = screenBuffer->lines[yCaretPos] + xCaretPos;

    ClearScreenBuffer(screenBuffer);

    SetWindowSizeScreenBuffer(hwnd, screenBuffer);
    SetWindowSymbolsSizeScreenBuffer(screenBuffer, fontInfo);
    BuildScreenBuffer(fileBuffer, screenBuffer);
    SetScrollLimits(screenBuffer);

    if (screenBuffer->vScrollLimit <= SHRT_MAX)
        SetScrollRange(hwnd, SB_VERT, 0, screenBuffer->vScrollLimit, FALSE);
    else
        SetScrollRange(hwnd, SB_VERT, 0, SHRT_MAX, FALSE);

    if (screenBuffer->hScrollLimit <= SHRT_MAX)
        SetScrollRange(hwnd, SB_HORZ, 0, screenBuffer->hScrollLimit, FALSE);
    else
        SetScrollRange(hwnd, SB_HORZ, 0, SHRT_MAX, FALSE);

    if (screenBuffer->hasHScroll == TRUE)
    {
        vScrollPos = SearchVScrollPos(screenBuffer->lines, screenBuffer->linesCount, firstSymbol);
        SetVScroll(hwnd, screenBuffer, vScrollPos);
        SetHScroll(hwnd, screenBuffer, hScrollPos);
        screenBuffer->xCaretPos = xCaretPos;
        screenBuffer->yCaretPos = yCaretPos;
    }
    else
    {
        vScrollPos = SearchVScrollPos(screenBuffer->lines, screenBuffer->linesCount, firstSymbol);
        SetVScroll(hwnd, screenBuffer, vScrollPos);
        SearchCaretPos(screenBuffer, symbolAfterCaret);
    }

    InvalidateRect(hwnd, NULL, TRUE);
}

/*
Calculate length of line screenBuffer->lines[lineIndex]
IN:
    ScreenBuffer* screenBuffer - screen data struct
    int lineIndex - the index of line in the array screenBuffer->lines, the length of which needs to be calculated
OUT:
    int len - the length of line screenBuffer->lines[lineIndex]
*/
int GetLineLen(ScreenBuffer* screenBuffer, int lineIndex)
{
    int len;

    if (lineIndex >= screenBuffer->linesCount)
        return 0;

    if (lineIndex != screenBuffer->linesCount - 1)
        len = screenBuffer->lines[lineIndex + 1] - screenBuffer->lines[lineIndex];
    else
        len = strlen(screenBuffer->lines[lineIndex]);

    if (screenBuffer->lines[lineIndex][len - 1] == '\r' || screenBuffer->lines[lineIndex][len - 1] == '\0')
        len--;

    return len;
}

/*
Sets max values of scroll positions
IN:
    ScreenBuffer* screenBuffer - screen data struct
OUT:
    ScreenBuffer* screenBuffer - screen data struct with info about the maximum value of scrolls
*/
void SetScrollLimits(ScreenBuffer* screenBuffer)
{
    if (screenBuffer->linesMaxLen > screenBuffer->widthSymbols && screenBuffer->hasHScroll == TRUE)
        screenBuffer->hScrollLimit = screenBuffer->linesMaxLen - screenBuffer->widthSymbols;
    else
        screenBuffer->hScrollLimit = 0;

    if (screenBuffer->hasHScroll == TRUE)
    {
        if (screenBuffer->linesCount > screenBuffer->heightSymbols)
            screenBuffer->vScrollLimit = screenBuffer->linesCount - screenBuffer->heightSymbols;
        else
            screenBuffer->vScrollLimit = 0;
    }
    else
    {
        if (screenBuffer->linesCount > screenBuffer->heightSymbols)
            screenBuffer->vScrollLimit = screenBuffer->linesCount - screenBuffer->heightSymbols;
        else
            screenBuffer->vScrollLimit = 0;
    }
}

/*
Sets vertical scroll position
IN:
    HWND hwnd - window descriptor
    ScreenBuffer* screenBuffer - screen data struct
    int position - position vertical scroll that should be set
OUT:
    ScreenBuffer* screenBuffer - screen data struct with new vertical scroll position
*/
void SetVScroll(HWND hwnd, ScreenBuffer* screenBuffer, int position)
{
    if (position > screenBuffer->vScrollLimit)
        screenBuffer->vScrollPos = screenBuffer->vScrollLimit;
    else
        screenBuffer->vScrollPos = position;

    InvalidateRect(hwnd, NULL, TRUE);

    if (screenBuffer->vScrollLimit > SHRT_MAX)
        SetScrollPos(hwnd, SB_VERT, (int)((double)screenBuffer->vScrollPos * SHRT_MAX / screenBuffer->vScrollLimit), TRUE);
    else
        SetScrollPos(hwnd, SB_VERT, screenBuffer->vScrollPos, TRUE);
}

/*
Sets horizontal scroll position
IN:
    HWND hwnd - window descriptor
    ScreenBuffer* screenBuffer - screen data struct
    int position - position horizontal scroll that should be set
OUT:
    ScreenBuffer* screenBuffer - screen data struct with new horizontal scroll position
*/
void SetHScroll(HWND hwnd, ScreenBuffer* screenBuffer, int position)
{
    if (position > screenBuffer->hScrollLimit)
        screenBuffer->hScrollPos = screenBuffer->hScrollLimit;
    else
        screenBuffer->hScrollPos = position;

    InvalidateRect(hwnd, NULL, TRUE);

    if (screenBuffer->hScrollLimit > SHRT_MAX)
        SetScrollPos(hwnd, SB_VERT, (int)((double)screenBuffer->hScrollPos * SHRT_MAX / screenBuffer->hScrollLimit), TRUE);
    else
        SetScrollPos(hwnd, SB_HORZ, screenBuffer->hScrollPos, TRUE);
}

/*
Changes the vertical position of scroll
IN:
    HWND hwnd - window handle
    ScreenBuffer* screenBuffer - screen data struct
    int delta - change of vertical scroll position
OUT:
    ScreenBuffer* screenBuffer - screen data struct with new vertical scroll position
*/
void ChangeVScroll(HWND hwnd, ScreenBuffer* screenBuffer, int delta)
{
    if ((double)screenBuffer->vScrollPos + (double)delta < 0)
        SetVScroll(hwnd, screenBuffer, 0);
    else
        SetVScroll(hwnd, screenBuffer, (int)(screenBuffer->vScrollPos + delta));
}

/*
Changes the horizontal position of scroll
IN:
    HWND hwnd - window handle
    ScreenBuffer* screenBuffer - screen data struct
    int delta - change of horizontal scroll position
OUT:
    ScreenBuffer* screenBuffer - screen data struct with new horizontal scroll position
*/
void ChangeHScroll(HWND hwnd, ScreenBuffer* screenBuffer, int delta)
{
    if ((double)screenBuffer->hScrollPos + (double)delta < 0)
        SetHScroll(hwnd, screenBuffer, 0);
    else
        SetHScroll(hwnd, screenBuffer, (int)(screenBuffer->hScrollPos + delta));
}

/*
Changes the horizontal position of scroll after moving the caret, if needed
IN:
    HWND hwnd - window handle
    ScreenBuffer* screenBuffer - screen data struct
OUT:
    ScreenBuffer* screenBuffer - screen data struct with right horizontal scroll position after moving the caret
*/
void SearchHScrollPosForCaret(HWND hwnd, ScreenBuffer* screenBuffer)
{
    int relativePos = screenBuffer->xCaretPos - screenBuffer->hScrollPos;

    if (relativePos < 0)
        ChangeHScroll(hwnd, screenBuffer, relativePos - 1);

    if (relativePos >= screenBuffer->widthSymbols)
        ChangeHScroll(hwnd, screenBuffer, relativePos - screenBuffer->widthSymbols);
}

/*
Changes the vertical position of scroll after moving the caret, if needed
IN:
    HWND hwnd - window handle
    ScreenBuffer* screenBuffer - screen data struct
OUT:
    ScreenBuffer* screenBuffer - screen data struct with right vertical scroll position after moving the caret
*/
void SearchVScrollPosForCaret(HWND hwnd, ScreenBuffer* screenBuffer)
{
    int relativePos = screenBuffer->yCaretPos - screenBuffer->vScrollPos;

    if (relativePos < 0)
        ChangeVScroll(hwnd, screenBuffer, relativePos);

    if (relativePos >= screenBuffer->heightSymbols - 1)
        ChangeVScroll(hwnd, screenBuffer, relativePos - screenBuffer->heightSymbols + 1);
}

/*
Changes the horizontal position of the caret
IN:
    HWND hwnd - window handle
    ScreenBuffer* screenBuffer - screen data struct
    int delta - horizontal change of caret position
OUT:
    ScreenBuffer* screenBuffer - screen data struct with new caret position
*/
void ChangeXCaretPos(HWND hwnd, ScreenBuffer* screenBuffer, int delta)
{
    int lineIndex = screenBuffer->yCaretPos;
    int len = GetLineLen(screenBuffer, lineIndex);

    if (screenBuffer->xCaretPos + delta < 0)
    {
        if (lineIndex > 0)
        {
            lineIndex--;
            screenBuffer->yCaretPos--;
            len = GetLineLen(screenBuffer, lineIndex);
            delta = len - screenBuffer->xCaretPos;
        }
        else
        {
            delta = 0;
        }
    }
    else if (screenBuffer->xCaretPos + delta > len)
    {
        if (lineIndex < screenBuffer->linesCount - 1)
        {
            screenBuffer->yCaretPos++;
            delta = -screenBuffer->xCaretPos;
        }
        else
        {
            delta = 0;
        }
    }

    screenBuffer->xCaretPos += delta;
    SearchHScrollPosForCaret(hwnd, screenBuffer);
    SearchVScrollPosForCaret(hwnd, screenBuffer);

    InvalidateRect(hwnd, NULL, TRUE);

}

/*
Changes the vertical position of the caret
IN:
    HWND hwnd - window handle
    ScreenBuffer* screenBuffer - screen data struct
    int delta - vertical change of caret position
OUT:
    ScreenBuffer* screenBuffer - screen data struct with new caret position
*/
void ChangeYCaretPos(HWND hwnd, ScreenBuffer* screenBuffer, int delta)
{
    if (screenBuffer->yCaretPos + delta < 0 || screenBuffer->yCaretPos + delta >= screenBuffer->linesCount)
        delta = 0;

    screenBuffer->yCaretPos += delta;

    int lineIndex = screenBuffer->yCaretPos;
    int len = GetLineLen(screenBuffer, lineIndex);

    if (screenBuffer->xCaretPos > len)
        screenBuffer->xCaretPos = len;


    SearchVScrollPosForCaret(hwnd, screenBuffer);
    SearchHScrollPosForCaret(hwnd, screenBuffer);

    InvalidateRect(hwnd, NULL, TRUE);
}

/*
Draws all visible content on the user's screen
IN:
    HWND hwnd - window handle
    ScreenBuffer* screenBuffer - screen data struct
    FontInfo* fontInfo - struct with info about font (height and width)
OUT:
    ---
*/
void DrawScreenBuffer(HWND hwnd, ScreenBuffer* screenBuffer, FontInfo* fontInfo)
{
    if (screenBuffer == NULL || screenBuffer->lines == NULL || screenBuffer->linesCount == 0)
        return;

    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;

    hdc = BeginPaint(hwnd, &ps);
    GetClientRect(hwnd, &rect);
    SelectObject(hdc, fontInfo->hFont);

    for (int i = 0; i < screenBuffer->heightSymbols; i++)
    {
        int lineIndex = i + screenBuffer->vScrollPos;
        int len;

        if (lineIndex == screenBuffer->linesCount)
            break;

        if (lineIndex >= screenBuffer->linesCount)
        return 0;

        if (lineIndex != screenBuffer->linesCount - 1)
            len = screenBuffer->lines[lineIndex + 1] - screenBuffer->lines[lineIndex] - screenBuffer->hScrollPos;
        else
            len = strlen(screenBuffer->lines[lineIndex] + screenBuffer->hScrollPos);

        if ((screenBuffer->lines[lineIndex] + screenBuffer->hScrollPos)[len - 1] == '\r' || (screenBuffer->lines[lineIndex] + screenBuffer->hScrollPos)[len - 1] == '\0')
            len--;

        if (len > 0)
            TextOut(hdc, rect.left, rect.top + i * fontInfo->fontHeight, &screenBuffer->lines[lineIndex][screenBuffer->hScrollPos], len);
    }

    SetCaretPos((screenBuffer->xCaretPos - screenBuffer->hScrollPos) * fontInfo->fontWidth, (screenBuffer->yCaretPos - screenBuffer->vScrollPos) * fontInfo->fontHeight);

    EndPaint(hwnd, &ps);
}

/*
Zeros all ScreenBuffer fields and frees memory
IN:
    ScreenBuffer* screenBuffer - screen data struct
OUT:
    ScreenBuffer* screenBuffer - screen data struct with zeroed values
*/
void ClearScreenBuffer(ScreenBuffer* screenBuffer)
{
    if (screenBuffer == NULL)
        return;

    if (screenBuffer->lines != NULL)
        free(screenBuffer->lines);

    screenBuffer->lines = NULL;
    screenBuffer->linesCount = 0;
    screenBuffer->linesMaxLen = 0;
    screenBuffer->height = 0;
    screenBuffer->width = 0;
    screenBuffer->heightSymbols = 0;
    screenBuffer->widthSymbols = 0;
    screenBuffer->hScrollPos = 0;
    screenBuffer->vScrollPos = 0;
    screenBuffer->vScrollLimit = 0;
    screenBuffer->hScrollLimit = 0;
    screenBuffer->xCaretPos = 0;
    screenBuffer->yCaretPos = 0;
}
