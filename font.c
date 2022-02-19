#include "font.h"

#include <stdio.h>

/*
Creates new font, calculates font metrics and writes it in font info struct
IN:
    HWND hWnd - window descriptor
    FontInfo* fontInfo - font info struct
OUT:
    FontInfo* fontInfo - font info struct with new font and font metrics
*/
void InitFontInfo(HWND hWnd, FontInfo* fontInfo)
{
    fontInfo->fontHeight = 35;

    fontInfo->hFont = CreateFont(fontInfo->fontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_DECORATIVE, NULL);

    HDC hdc = GetDC(hWnd);
    TEXTMETRIC tm;
    int otmSize;

    SelectObject(hdc, fontInfo->hFont);
    GetTextMetrics(hdc, &tm);

    fontInfo->fontHeight = tm.tmHeight + tm.tmExternalLeading;
    fontInfo->fontWidth = tm.tmAveCharWidth;
}

/*
Zeros all FontInfo fields
IN:
    FontInfo* fontInfo - font info struct
OUT:
    FontInfo* fontInfo - font info struct with zeroed values
*/
void ClearFontInfo(FontInfo* fontInfo)
{
    DeleteObject(fontInfo->hFont);
    fontInfo->fontHeight = 0;
    fontInfo->fontWidth = 0;
}
