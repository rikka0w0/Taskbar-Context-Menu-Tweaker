// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#ifdef _DEBUG
#pragma comment (linker, "/nodefaultlib:msvcrtd.lib")
#else
#pragma comment (linker, "/nodefaultlib:msvcrt.lib")
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <shellapi.h>
//#include <stdlib.h>



// TODO: reference additional headers your program requires here
//Custom messages
#define WM_TWEAKER 0x0409
#define TWEAKER_EXIT 0x90

#define MENUID_TOGGLE 666


//Utils func
#define STANDARD_DPI 96
#define DPI_SCALE(in) in * GetDPI() / STANDARD_DPI
HBITMAP IconToBitmap(HICON, INT);
void ClassicMenu(HMENU hMenu);
int GetDPI();
DWORD GetMyConfig();
void SetMyConfig(DWORD d);
LPWSTR GetToggleMenuTitle();
#define UseImmersiveMenu() (GetMyConfig()&0x01)
#define HasIcon() (GetMyConfig()&0x02)
#define HideToggle() (GetMyConfig()&0x04)

//API HOOK
void MyHook_Initialize();
void MyHook_Destroy();

//Icons
void MyIcons_Load();
HBITMAP MyIcons_Get(unsigned char index);
void MyIcons_Free();
#define MYICON_SETTING 0
#define MYICON_TASKMGR 1
#define MYICON_SHOWDESKTOP 2
#define MyIcons_Count 3

void* __cdecl malloc(ULONG_PTR uSize);
void __cdecl free(LPVOID pMemBlock);

//void * __cdecl memset(void *, int, size_t);
//#pragma intrinsic(memset)
//int __cdecl strcmp(const char* src, const char* dst);
//#pragma intrinsic(strcmp)
//size_t __cdecl strlen(char const* _Str);
//#pragma intrinsic(strlen)