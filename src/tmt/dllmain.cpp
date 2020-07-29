#include "tmt.h"
#include "../../include/MinHook.h"
#include "../Menu98.h"

static HMODULE gLibModule = 0;

static LONG_PTR OldWndProc_TaskBar = NULL;
static LONG_PTR OldWndProc_TaskBar_SecondScreen = NULL;

static HWND hWnd_TaskBar = 0;
static HWND hWnd_TaskBar_SecondScreen = NULL;
static HWND hWnd_ATL1 = 0;
static HWND hWnd_ATL2 = 0;

static HMODULE menu98Module = 0;

void ClassicMenu(HMENU hMenu) {
	MENUINFO info;
	info.cbSize = sizeof(MENUINFO);
	info.fMask = MIM_BACKGROUND;
	GetMenuInfo(hMenu, &info);
	info.hbrBack = NULL;
	SetMenuInfo(hMenu, &info);

	for (int i = 0; i < GetMenuItemCount(hMenu); i++) {
		MENUITEMINFO menuInfo;
		menuInfo.cbSize = sizeof(MENUITEMINFO);
		menuInfo.fMask = MIIM_FTYPE | MIIM_SUBMENU;
		GetMenuItemInfo(hMenu, i, true, &menuInfo);
		menuInfo.fType &= ~MFT_OWNERDRAW;
		menuInfo.fMask = MIIM_FTYPE;
		SetMenuItemInfo(hMenu, i, true, &menuInfo);
	}
}

void ClassicMenuIfPossible(HWND hWnd, HMENU hMenu) {
	if (UseImmersiveMenu())
		return;

	char clsName[256];
	GetClassNameA(hWnd, clsName, 256);

	if (strcmp(clsName, "TrayShowDesktopButtonWClass") == 0) {
		if (HasIcon())
			SetMenuItemBitmaps(hMenu, 0x1A2D, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));
		ClassicMenu(hMenu);
	} else if (
		strcmp(clsName, "NotificationsMenuOwner") == 0 ||		// Notification Button
		strcmp(clsName, "LauncherTipWnd") == 0 ||				// Win+X menu
		strcmp(clsName, "MultitaskingViewFrame") == 0 ||		// Multitask Button
		hWnd == hWnd_ATL1 || hWnd == hWnd_ATL2) {				// Network Icon and Volumn Icon
		ClassicMenu(hMenu);
		for (int i = 0; i < GetMenuItemCount(hMenu); i++)
			ClassicMenu(GetSubMenu(hMenu, i));
	}
}

void RestoreWndProc() {
	if (OldWndProc_TaskBar != NULL)
		SetWindowLongPtr(hWnd_TaskBar, GWLP_WNDPROC, OldWndProc_TaskBar);

	if (OldWndProc_TaskBar_SecondScreen != NULL)
		SetWindowLongPtr(hWnd_TaskBar_SecondScreen, GWLP_WNDPROC, OldWndProc_TaskBar_SecondScreen);
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

	if (menu98Module != NULL) {
		FPT___Menu98Unload fpMenu98Unload = (FPT___Menu98Unload)GetProcAddress(menu98Module, "__Menu98Unload");
		if (fpMenu98Unload != NULL) {
			fpMenu98Unload(NULL);
		} else {
			MessageBoxW(0, L"Failed to unload Menu98!", L"Fatal Error", MB_ICONERROR);
		}
	}


	CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FreeLibraryAndExitThread, gLibModule, 0, NULL));
}

LRESULT CALLBACK WndProc_TaskBar(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	WNDPROC fpWndProcOld = WNDPROC(OldWndProc_TaskBar);

	switch (uMsg) {
	case WM_TWEAKER: {
		if (wParam == TWEAKER_EXIT)
			CloseBackground();
		return 0;
	}

	case WM_INITMENUPOPUP: {
		LRESULT ret = fpWndProcOld(hwnd, uMsg, wParam, lParam);

		if (!UseImmersiveMenu())
			ClassicMenu((HMENU)wParam);

		if (!HasIcon())
			return ret;

		HWND hWnd_NotifyWnd = FindWindowEx(hwnd, NULL, TEXT("TrayNotifyWnd"), NULL);
		HWND hWnd_Clock = FindWindowEx(hWnd_NotifyWnd, NULL, TEXT("TrayClockWClass"), NULL);

		RECT rect;
		GetWindowRect(hWnd_Clock, &rect);
		POINT pt;
		GetCursorPos(&pt);

		if (((pt.x > rect.left)&(pt.x < rect.right)&(pt.y > rect.top)&(pt.y < rect.bottom))) {
			SetMenuItemBitmaps((HMENU)wParam, 413, MF_BYCOMMAND, MyIcons_Get(MYICON_SETTING), MyIcons_Get(MYICON_SETTING));
			SetMenuItemBitmaps((HMENU)wParam, 420, MF_BYCOMMAND, MyIcons_Get(MYICON_TASKMGR), MyIcons_Get(MYICON_TASKMGR));
			SetMenuItemBitmaps((HMENU)wParam, 407, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));
		} else if (hWnd_NotifyWnd == hwnd || hWnd_TaskBar == hwnd) {
			SetMenuItemBitmaps((HMENU)wParam, 414, MF_BYCOMMAND, MyIcons_Get(MYICON_SETTING), MyIcons_Get(MYICON_SETTING));
			SetMenuItemBitmaps((HMENU)wParam, 421, MF_BYCOMMAND, MyIcons_Get(MYICON_TASKMGR), MyIcons_Get(MYICON_TASKMGR));
			SetMenuItemBitmaps((HMENU)wParam, 408, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));

			SetMenuItemBitmaps((HMENU)wParam, 413, MF_BYCOMMAND, MyIcons_Get(MYICON_SETTING), MyIcons_Get(MYICON_SETTING));
			SetMenuItemBitmaps((HMENU)wParam, 420, MF_BYCOMMAND, MyIcons_Get(MYICON_TASKMGR), MyIcons_Get(MYICON_TASKMGR));
			SetMenuItemBitmaps((HMENU)wParam, 407, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));
		} else {
			SetMenuItemBitmaps((HMENU)wParam, 0x019E, MF_BYCOMMAND, MyIcons_Get(MYICON_SETTING), MyIcons_Get(MYICON_SETTING));
			SetMenuItemBitmaps((HMENU)wParam, 0x01A5, MF_BYCOMMAND, MyIcons_Get(MYICON_TASKMGR), MyIcons_Get(MYICON_TASKMGR));
			SetMenuItemBitmaps((HMENU)wParam, 0x0198, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));
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
	}

	return fpWndProcOld(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WndProc_TaskBar_SecondScreen(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	WNDPROC fpWndProcOld = WNDPROC(OldWndProc_TaskBar_SecondScreen);

	if (uMsg == WM_INITMENUPOPUP) {
		LRESULT ret = fpWndProcOld(hwnd, uMsg, wParam, lParam);

		if (!UseImmersiveMenu())
			ClassicMenu((HMENU)wParam);

		if (!HasIcon())
			return ret;

		SetMenuItemBitmaps((HMENU)wParam, 0x19d, MF_BYCOMMAND, MyIcons_Get(MYICON_SETTING), MyIcons_Get(MYICON_SETTING));
		SetMenuItemBitmaps((HMENU)wParam, 0x1a4, MF_BYCOMMAND, MyIcons_Get(MYICON_TASKMGR), MyIcons_Get(MYICON_TASKMGR));
		SetMenuItemBitmaps((HMENU)wParam, 0x197, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));

		return ret;
	}

	return fpWndProcOld(hwnd, uMsg, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
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

BOOL CALLBACK EnumWindowsCallBack(HWND hwnd, LPARAM lParam) {
	if (GetWindowThreadProcessId(hwnd, NULL) == (DWORD)lParam) {
		// Window belong to the same thread
		char className[255];
		GetClassNameA(hwnd, className, sizeof(className));

		if (className[0] == 'A' && className[1] == 'T' && className[2] == 'L') {		
			if (GetWindowTextLengthA(hwnd) == 0) {
				hWnd_ATL1 = hwnd;
			} else {
				hWnd_ATL2 = hwnd;
			}
		}
	}

	return TRUE;
}

extern "C" _declspec(dllexport) DWORD  __cdecl  __TweakerInit(LPVOID param) {
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

	//hWnd_TaskBar = FindWindow(TEXT("Shell_TrayWnd"), NULL);
	hWnd_TaskBar = *(HWND*)param;
	if (IsWindow(hWnd_TaskBar)) {
		OldWndProc_TaskBar = GetWindowLongPtr(hWnd_TaskBar, GWLP_WNDPROC);
		if (OldWndProc_TaskBar != NULL)
			SetWindowLongPtr(hWnd_TaskBar, GWLP_WNDPROC, (LONG_PTR)&WndProc_TaskBar);
	}

	// The taskbar on the second screen
	hWnd_TaskBar_SecondScreen = FindWindowA("Shell_SecondaryTrayWnd", NULL);
	if (IsWindow(hWnd_TaskBar_SecondScreen)) {
		OldWndProc_TaskBar_SecondScreen = GetWindowLongPtr(hWnd_TaskBar_SecondScreen, GWLP_WNDPROC);
		if (OldWndProc_TaskBar_SecondScreen != NULL)
			SetWindowLongPtr(hWnd_TaskBar_SecondScreen, GWLP_WNDPROC, (LONG_PTR)&WndProc_TaskBar_SecondScreen);
	}

	menu98Module = LoadLibraryA("menu98.dll");
	if (menu98Module != NULL) {
		FPT___Menu98Init fpMenu98Init = (FPT___Menu98Init)GetProcAddress(menu98Module, "__Menu98Init");
		if (fpMenu98Init != NULL) {
			MENU98_INIT menu98InitInfo;
			menu98InitInfo.hWnd_TaskBar = hWnd_TaskBar;
			menu98InitInfo.TrackPopupMenuEx = fpTrackPopupMenuEx;
			menu98InitInfo.cmdLine = (char*)param + sizeof(HWND);
			fpMenu98Init(&menu98InitInfo);
		}
	}

	// Network & Volumn
	HWND hPNIHiddenWnd = FindWindowA("PNIHiddenWnd", NULL);
	EnumWindows(EnumWindowsCallBack, GetWindowThreadProcessId(hPNIHiddenWnd, NULL));

	// Notification
	HWND hNotificationWindow = FindWindowA("NotifyIconOverflowWindow", NULL); // EnumChildWindows 
	return 0;
}