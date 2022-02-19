#ifndef __MENU_
#define __MENU_

#include "global.h"
#include <commdlg.h>

#define MENU_ID             255

#define MENU_FILE_OPEN      1
#define MENU_FILE_CLOSE     2
#define MENU_FILE_EXIT      3
#define MENU_CHANGE_MODE    4

void MenuFileOpen(HWND hwnd, GlobalStruct* global);

void MenuFileClose(HWND hwnd, GlobalStruct* global);

void MenuChangeMode(HWND hwnd, GlobalStruct* global);

#endif // __MENU_
