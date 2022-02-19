#ifndef __EVENTS_
#define __EVENTS_

#include "global.h"

void WMCreate(HWND hwnd, GlobalStruct* global, LPARAM lParam);
void WMSize(HWND hwnd, GlobalStruct* global, WPARAM wParam, LPARAM lParam);
void WMPaint(HWND hwnd, GlobalStruct* global);
void WMVScroll(HWND hwnd, GlobalStruct* global, WPARAM wParam);
void WMHScroll(HWND hwnd, GlobalStruct* global, WPARAM wParam);
void WMMouseWheel(HWND hwnd, GlobalStruct* global, WPARAM wParam);
void WMKeyDown(HWND hwnd, GlobalStruct* global, WPARAM wParam);
void WMCommand(HWND hwnd, GlobalStruct* global, WPARAM wParam);
void WMSetFocus(HWND hwnd, GlobalStruct* global);
void WMKillFocus(HWND hwnd);
void WMDestroy(GlobalStruct* global);


#endif // __EVENTS_


