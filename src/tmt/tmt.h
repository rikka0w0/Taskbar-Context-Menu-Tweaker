#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <shellapi.h>
#include "../MinCRT.h"

//Icons
void MyIcons_Load();
HBITMAP MyIcons_Get(unsigned char index);
void MyIcons_Free();
#define MYICON_SETTING 0
#define MYICON_TASKMGR 1
#define MYICON_SHOWDESKTOP 2
#define MyIcons_Count 3

//Menu Hook
void ClassicMenu(HMENU hMenu);
void ClassicMenuIfPossible(HWND hWnd, HMENU hMenu);


//Config
DWORD GetMyConfig();
void SetMyConfig(DWORD d);
LPWSTR GetToggleMenuTitle();
#define UseImmersiveMenu() (GetMyConfig()&0x01)
#define HasIcon() (GetMyConfig()&0x02)
#define HideToggle() (GetMyConfig()&0x04)

//Icon
#define STANDARD_DPI 96
#define DPI_SCALE(in) in * GetDPI() / STANDARD_DPI
HBITMAP IconToBitmap(HICON, INT);
int GetDPI();


//Custom Messages
#define WM_TWEAKER 0x0409
#define TWEAKER_EXIT 0x90

#define MENUID_TOGGLE 666