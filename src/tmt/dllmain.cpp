#include "tmt.h"
#include "../../include/MinHook.h"

static HMODULE gLibModule = 0;

static LONG_PTR OldWndProc_TaskBar = 0;

static HWND hWnd_TaskBar = 0;

void RestoreWndProc() {
	if (OldWndProc_TaskBar != NULL)
		SetWindowLongPtr(hWnd_TaskBar, GWLP_WNDPROC, OldWndProc_TaskBar);
}

void CloseBackground() {
	// Disable the hooks
	BOOL flag = MH_DisableHook(&TrackPopupMenu);
	flag |= MH_DisableHook(&TrackPopupMenuEx);
	flag |= MH_Uninitialize();	// Uninitialize MinHook.

	if (flag != MH_OK)
		MessageBoxW(0, L"An error occured while unloading hooks", L"Fatal Error", MB_ICONERROR);

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


void ProcessResultIfPossible(BOOL ret, HMENU hMenu) {
	MENUITEMINFO info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_STATE;
	GetMenuItemInfo(hMenu, ret, MF_BYCOMMAND, &info);

	if (ret == MENUID_TOGGLE) {
		DWORD s = GetMyConfig();
		if (info.fState & MFS_CHECKED)
			s &= ~0x01;
		else
			s |= 0x01;
		SetMyConfig(s);
	}
}


//DWORD WINAPI ThreadProc(LPVOID lpParameter);

typedef BOOL (WINAPI *FPT_TrackPopupMenu)(HMENU, UINT, int, int, int, HWND, CONST RECT*);
typedef BOOL(WINAPI *FPT_TrackPopupMenuEx)(HMENU, UINT, int, int, HWND, LPTPMPARAMS);

// Pointer for calling original MessageBoxW.
FPT_TrackPopupMenu fpTrackPopupMenu;
FPT_TrackPopupMenuEx fpTrackPopupMenuEx;

// Detour function which overrides TrackPopupMenu.
BOOL WINAPI HookedTrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, CONST RECT* prcRect) {
	if (IsWindow(hWnd))
		ClassicMenuIfPossible(hWnd, hMenu);

	BOOL ret = fpTrackPopupMenu(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);

	ProcessResultIfPossible(ret, hMenu);

	return ret;
}

BOOL WINAPI HookedTrackPopupMenuEx(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm) {
	if (IsWindow(hWnd))
		ClassicMenuIfPossible(hWnd, hMenu);
	//
	BOOL ret = fpTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);

	ProcessResultIfPossible(ret, hMenu);

	return ret;
}

extern "C" _declspec(dllexport) DWORD  __cdecl  __TweakerInit(LPVOID unused) {
	//MyHook_Initialize();
	if (MH_Initialize() != MH_OK)
		MessageBoxW(0, L"Unable to initialize disassembler!", L"Fatal Error", MB_ICONERROR);
	
	// Create a hook for MessageBoxW, in disabled state.
	BOOL flag = MH_CreateHook(&TrackPopupMenu, &HookedTrackPopupMenu,
		reinterpret_cast<LPVOID*>(&fpTrackPopupMenu));
	flag |= MH_CreateHook(&TrackPopupMenuEx, &HookedTrackPopupMenuEx,
		reinterpret_cast<LPVOID*>(&fpTrackPopupMenuEx));
	if (flag != MH_OK)
		MessageBoxW(0, L"Unable to create hooks!", L"Fatal Error", MB_ICONERROR);

	// Enable the hook
	flag = MH_EnableHook(&TrackPopupMenu);
	flag |= MH_EnableHook(&TrackPopupMenuEx);
	if (flag != MH_OK)
		MessageBoxW(0, L"Failed to enable hooks!", L"Fatal Error", MB_ICONERROR);

	MyIcons_Load();

	hWnd_TaskBar = FindWindow(TEXT("Shell_TrayWnd"), nullptr);
	if (IsWindow(hWnd_TaskBar)) {
		OldWndProc_TaskBar = GetWindowLongPtr(hWnd_TaskBar, GWLP_WNDPROC);
		if (OldWndProc_TaskBar != 0)
			SetWindowLongPtr(hWnd_TaskBar, GWLP_WNDPROC, (LONG_PTR)&WndProc_TaskBar);
	}

	return 0;
}