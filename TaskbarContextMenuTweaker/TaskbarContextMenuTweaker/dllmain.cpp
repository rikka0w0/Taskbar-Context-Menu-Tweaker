// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

static HMODULE gLibModule = 0;

static LONG_PTR OldWndProc_TaskBar = 0;

static HWND hWnd_TaskBar = 0;

/*
static DWORD WINAPI FreeSelf(LPVOID param) {
FreeLibraryAndExitThread(gLibModule, EXIT_SUCCESS);
}
*/


void RestoreWndProc() {
	if (OldWndProc_TaskBar != NULL)
		SetWindowLongPtr(hWnd_TaskBar, GWLP_WNDPROC, OldWndProc_TaskBar);
}

void CloseBackground() {
	MyHook_Destroy();
	MyIcons_Free();
	RestoreWndProc();

	//CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)FreeSelf, nullptr, 0, nullptr));
	CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)FreeLibraryAndExitThread, gLibModule, 0, nullptr));
}



LRESULT CALLBACK WndProc_TaskBar(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static HWND menuOwner;

	switch (uMsg) {

	case WM_TWEAKER: {
		if (wParam == TWEAKER_EXIT)
			CloseBackground();
		return 0;
	}


	case WM_INITMENUPOPUP: {
		LRESULT ret = WNDPROC(OldWndProc_TaskBar)(hwnd, uMsg, wParam, lParam);

		if (!UseImmersiveMenu())
			ClassicMenu((HMENU)wParam);

		if (!HasIcon())
			return ret;

		HWND hWnd_NotifyWnd = FindWindowEx(hwnd, NULL, TEXT("TrayNotifyWnd"), NULL);
		HWND hWnd_Clock = FindWindowEx(hWnd_NotifyWnd, NULL, TEXT("TrayClockWClass"), NULL);


		if (menuOwner == hWnd_NotifyWnd) {
			SetMenuItemBitmaps((HMENU)wParam, 413, MF_BYCOMMAND, MyIcons_Get(MYICON_SETTING), MyIcons_Get(MYICON_SETTING));
			SetMenuItemBitmaps((HMENU)wParam, 420, MF_BYCOMMAND, MyIcons_Get(MYICON_TASKMGR), MyIcons_Get(MYICON_TASKMGR));
			SetMenuItemBitmaps((HMENU)wParam, 407, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));
		}
		else {
			RECT rect;
			GetWindowRect(hWnd_Clock, &rect);
			POINT pt;
			GetCursorPos(&pt);

			if ((pt.x > rect.left)&(pt.x<rect.right)&(pt.y>rect.top)&(pt.y < rect.bottom)) {
				SetMenuItemBitmaps((HMENU)wParam, 413, MF_BYCOMMAND, MyIcons_Get(MYICON_SETTING), MyIcons_Get(MYICON_SETTING));
				SetMenuItemBitmaps((HMENU)wParam, 420, MF_BYCOMMAND, MyIcons_Get(MYICON_TASKMGR), MyIcons_Get(MYICON_TASKMGR));
				SetMenuItemBitmaps((HMENU)wParam, 407, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));
			}
			else {
				SetMenuItemBitmaps((HMENU)wParam, 0x019E, MF_BYCOMMAND, MyIcons_Get(MYICON_SETTING), MyIcons_Get(MYICON_SETTING));
				SetMenuItemBitmaps((HMENU)wParam, 0x01A5, MF_BYCOMMAND, MyIcons_Get(MYICON_TASKMGR), MyIcons_Get(MYICON_TASKMGR));
				SetMenuItemBitmaps((HMENU)wParam, 0x0198, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));
			}
		}

		return ret;
	}

	case WM_INITMENU: {
		HMENU hMenu = (HMENU)wParam;

		if (HideToggle())
			break;

		LPWSTR title = GetToggleMenuTitle();
		if (title == NULL)
			InsertMenu(hMenu, 0, MF_STRING, MENUID_TOGGLE, L"Use immersive menu");
		else {
			InsertMenu(hMenu, 0, MF_STRING, MENUID_TOGGLE, title);
			free(title);
		}

		CheckMenuItem(hMenu, MENUID_TOGGLE, UseImmersiveMenu() ? MF_CHECKED : MF_UNCHECKED);
		break;
	}

	case WM_CONTEXTMENU:
		menuOwner = (HWND)wParam;
		break;
	}



	return WNDPROC(OldWndProc_TaskBar)(hwnd, uMsg, wParam, lParam);
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		gLibModule = hModule;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		//RestoreWndProc();
		break;
	}
	return TRUE;
}





//DWORD WINAPI ThreadProc(LPVOID lpParameter);


extern "C" _declspec(dllexport) DWORD  __cdecl  __TweakerInit(LPVOID unused) {
	MyHook_Initialize();
	MyIcons_Load();

	hWnd_TaskBar = FindWindow(TEXT("Shell_TrayWnd"), nullptr);
	if (IsWindow(hWnd_TaskBar)) {
		OldWndProc_TaskBar = GetWindowLongPtr(hWnd_TaskBar, GWLP_WNDPROC);
		if (OldWndProc_TaskBar != 0)
			SetWindowLongPtr(hWnd_TaskBar, GWLP_WNDPROC, (LONG_PTR)&WndProc_TaskBar);
	}

	return 0;
}