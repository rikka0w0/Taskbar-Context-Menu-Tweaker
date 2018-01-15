#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

typedef struct MENU98_INIT_T {
	HWND hWnd_TaskBar;
	void* TrackPopupMenuEx;
	LPSTR cmdLine;
} MENU98_INIT;

typedef DWORD(__cdecl *FPT___Menu98Init)(MENU98_INIT*);
typedef DWORD(__cdecl *FPT___Menu98Unload)(LPVOID);