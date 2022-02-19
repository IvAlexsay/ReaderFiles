#include "events.h"
#include "menu.h"

#include <commdlg.h>
#include <limits.h>

/*
Callback on message WM_CREATE
IN:
    HWND hwnd - window handle
    GlobalStruct* global - all application data, that stored as static variable field in WndProc
    LPARAM lParam - data that was sent to WndProc
*/
void WMCreate(HWND hwnd, GlobalStruct* global, LPARAM lParam)
{
    InitFileBuffer(&global->fileBuffer);
    InitScreenBuffer(&global->screenBuffer);
    InitFontInfo(hwnd, &global->fontInfo);

    global->fileLoaded = ReadToFileBuffer((char*)((CREATESTRUCT*)lParam)->lpCreateParams, &global->fileBuffer);

    CheckMenuItem(GetMenu(hwnd), MENU_CHANGE_MODE, MF_BYCOMMAND | MF_UNCHECKED);
}

/*
Callback on message WM_SIZE
IN:
    HWND hwnd - window handle
    GlobalStruct* global - all application data, that stored as static variable field in WndProc
    WPARAM wParam - data that was sent to WndProc
    LPARAM lParam - data that was sent to WndProc
*/
void WMSize(HWND hwnd, GlobalStruct* global, WPARAM wParam, LPARAM lParam)
{
    if (global->fileLoaded == TRUE)
        ResizeScreenBuffer(hwnd, &global->screenBuffer, &global->fileBuffer, &global->fontInfo);
}

/*
Callback on message WM_VSCROLL
IN:
    HWND hwnd - window handle
    GlobalStruct* global - all application data, that stored as static variable field in WndProc
    WPARAM wParam - data that was sent to WndProc
*/
void WMVScroll(HWND hwnd, GlobalStruct* global, WPARAM wParam)
{
    if (global->fileLoaded == FALSE)
        return;

    switch (LOWORD(wParam))
    {
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
    {
        int position;
        if (global->screenBuffer.vScrollLimit > SHRT_MAX)
            position = (int)(HIWORD(wParam) / (double)SHRT_MAX * global->screenBuffer.vScrollLimit);
        else
            position = HIWORD(wParam);

        SetVScroll(hwnd, &global->screenBuffer, position);
    }
    break;
    case SB_LINEDOWN:
        ChangeVScroll(hwnd, &global->screenBuffer, 1);
        break;
    case SB_LINEUP:
        ChangeVScroll(hwnd, &global->screenBuffer, -1);
        break;
    default:
        break;
    }
}

/*
Callback on message WM_HSCROLL
IN:
    HWND hwnd - window handle
    GlobalStruct* global - all application data, that stored as static variable field in WndProc
    WPARAM wParam - data that was sent to WndProc
*/
void WMHScroll(HWND hwnd, GlobalStruct* global, WPARAM wParam)
{
    if (global->fileLoaded == FALSE)
        return;

    switch (LOWORD(wParam))
    {
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
    {
        int position;
        if (global->screenBuffer.hScrollLimit > SHRT_MAX)
            position = (int)((double)HIWORD(wParam) / SHRT_MAX * global->screenBuffer.hScrollLimit);
        else
            position = HIWORD(wParam);

        SetHScroll(hwnd, &global->screenBuffer, position);
    }
    break;
    case SB_LINERIGHT:
        ChangeHScroll(hwnd, &global->screenBuffer, 1);
        break;
    case SB_LINELEFT:
        ChangeHScroll(hwnd, &global->screenBuffer, -1);
        break;
    default:
        break;
    }
}

/*
Callback on message WM_MOUSEWHEEL
IN:
    HWND hwnd - window handle
    GlobalStruct* global - all application data, that stored as static variable field in WndProc
    WPARAM wParam - data that was sent to WndProc
*/
void WMMouseWheel(HWND hwnd, GlobalStruct* global, WPARAM wParam)
{
    if (global->fileLoaded == FALSE)
        return;

    int delta = -GET_WHEEL_DELTA_WPARAM(wParam) / 120;
    ChangeVScroll(hwnd, &global->screenBuffer, delta);
}

/*
Callback on message WM_KEYDOWN
IN:
    HWND hwnd - window handle
    GlobalStruct* global - all application data, that stored as static variable field in WndProc
    WPARAM wParam - data that was sent to WndProc
*/
void WMKeyDown(HWND hwnd, GlobalStruct* global, WPARAM wParam)
{
    if (global->fileLoaded == FALSE)
        return;

    switch (wParam)
    {
    case VK_UP:
        ChangeYCaretPos(hwnd, &global->screenBuffer, -1);
        break;
    case VK_DOWN:
        ChangeYCaretPos(hwnd, &global->screenBuffer, 1);
        break;
    case VK_LEFT:
        ChangeXCaretPos(hwnd, &global->screenBuffer, -1);
        break;
    case VK_RIGHT:
        ChangeXCaretPos(hwnd, &global->screenBuffer, 1);
        break;
    case VK_PRIOR:
        ChangeVScroll(hwnd, &global->screenBuffer, -global->screenBuffer.heightSymbols);
        break;
    case VK_NEXT:
        ChangeVScroll(hwnd, &global->screenBuffer, global->screenBuffer.heightSymbols);
        break;
    default:
        break;
    }
}

/*
Callback on message WM_COMMAND
IN:
    HWND hwnd - window handle
    GlobalStruct* global - all application data, that stored as static variable field in WndProc
    WPARAM wParam - data that was sent to WndProc
*/
void WMCommand(HWND hwnd, GlobalStruct* global, WPARAM wParam)
{
    switch (LOWORD(wParam))
    {
    case MENU_FILE_OPEN:
    {
        OPENFILENAME ofn;       // common dialog box structure
        char filename[255];     // buffer for file name

        // Initialize OPENFILENAME
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = filename;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(filename);
        ofn.lpstrFilter = "*.TXT\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileName(&ofn) == TRUE)
        {
            printf("\nfile opened\n");                  //debug info

            ClearFileBuffer(&global->fileBuffer);
            ClearScreenBuffer(&global->screenBuffer);

            global->fileLoaded = ReadToFileBuffer(ofn.lpstrFile, &global->fileBuffer);
            if (global->fileLoaded == TRUE)
                ResizeScreenBuffer(hwnd, &global->screenBuffer, &global->fileBuffer, &global->fontInfo);
        }
    }
    break;
    case MENU_FILE_CLOSE:
    {
        ClearFileBuffer(&global->fileBuffer);
        ClearScreenBuffer(&global->screenBuffer);
        global->fileLoaded = FALSE;
        InvalidateRect(hwnd, NULL, TRUE);
        ShowScrollBar(hwnd, SB_HORZ, FALSE);
        ShowScrollBar(hwnd, SB_VERT, FALSE);
    }
    break;
    case MENU_FILE_EXIT:
        SendMessage(hwnd, WM_CLOSE, 0, 0);
        break;
    case MENU_CHANGE_MODE:
    {
        int mode = GetMenuState(GetMenu(hwnd), MENU_CHANGE_MODE, MF_BYCOMMAND);
        if (mode & MF_CHECKED)
        {
            CheckMenuItem(GetMenu(hwnd), MENU_CHANGE_MODE, MF_BYCOMMAND | MF_UNCHECKED);
            global->screenBuffer.hasHScroll = TRUE;
        }
        else
        {
            CheckMenuItem(GetMenu(hwnd), MENU_CHANGE_MODE, MF_BYCOMMAND | MF_CHECKED);
            global->screenBuffer.hasHScroll = FALSE;
        }
        ResizeScreenBuffer(hwnd, &global->screenBuffer, &global->fileBuffer, &global->fontInfo);
    }
    break;
    }
}

/*
Callback on message WM_PAINT
IN:
    HWND hwnd - window handle
    GlobalStruct* global - all application data, that stored as static variable field in WndProc
*/
void WMPaint(HWND hwnd, GlobalStruct* global)
{
    if (global->fileLoaded == TRUE)
        DrawScreenBuffer(hwnd, &global->screenBuffer, &global->fontInfo);
}

/*
Callback on message WM_FOCUS
IN:
    HWND hwnd - window handle
    GlobalStruct* global - all application data, that stored as static variable field in WndProc
*/
void WMSetFocus(HWND hwnd, GlobalStruct* global)
{
    CreateCaret(hwnd, NULL, 1, global->fontInfo.fontHeight);
    ShowCaret(hwnd);
}

/*
Callback on message WM_KILLFOCUS
IN:
    HWND hwnd - window handle
*/
void WMKillFocus(HWND hwnd)
{
    DestroyCaret();
}

/*
Callback on message WM_DESTROY
IN:
    GlobalStruct* global - all application data, that stored as static variable field in WndProc
*/
void WMDestroy(GlobalStruct* global)
{
    ClearFileBuffer(&global->fileBuffer);
    ClearScreenBuffer(&global->screenBuffer);
    ClearFontInfo(&global->fontInfo);
}
