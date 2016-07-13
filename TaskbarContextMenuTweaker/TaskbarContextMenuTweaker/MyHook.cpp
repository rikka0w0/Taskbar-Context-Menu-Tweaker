#include "stdafx.h"
#include "HookAPI.h"

LPAPIHOOK apiHook_TPMEx;
LPAPIHOOK apiHook_TPM;


void ClassicMenuIfPossible(HWND hWnd, HMENU hMenu) {
	if (UseImmersiveMenu())
		return;

	char clsName[256];
	GetClassNameA(hWnd, clsName, 256);

	if (strcmp(clsName, "TrayShowDesktopButtonWClass") == 0) {
		if (HasIcon())
			SetMenuItemBitmaps(hMenu, 0x1A2D, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));
		ClassicMenu(hMenu);
	}

	if (strcmp(clsName, "NotificationsMenuOwner") == 0)
		ClassicMenu(hMenu);

	if (strcmp(clsName, "LauncherTipWnd") == 0) {
		ClassicMenu(hMenu);
		for (int i = 0; i < GetMenuItemCount(hMenu); i++)
			ClassicMenu(GetSubMenu(hMenu, i));
	}


	if (strcmp(clsName, "MultitaskingViewFrame") == 0) {
		ClassicMenu(hMenu);
		for (int i = 0; i < GetMenuItemCount(hMenu); i++)
			ClassicMenu(GetSubMenu(hMenu, i));
	}
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

BOOL WINAPI MyTrackPopupMenu(HMENU hMenu, UINT  uFlags, int x, int y, int nReserved, HWND  hWnd, const RECT  *prcRect) {
	APIHook_TurnOffHook(apiHook_TPM);

	if (IsWindow(hWnd))
		ClassicMenuIfPossible(hWnd, hMenu);


	BOOL ret = TrackPopupMenu(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
	APIHook_TurnOnHook(apiHook_TPM);

	ProcessResultIfPossible(ret, hMenu);

	return ret;
}


BOOL WINAPI MyTrackPopupMenuEx(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm)
{
	APIHook_TurnOffHook(apiHook_TPMEx);

	if (IsWindow(hWnd))
		ClassicMenuIfPossible(hWnd, hMenu);


	BOOL ret = TrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
	APIHook_TurnOnHook(apiHook_TPMEx);

	ProcessResultIfPossible(ret, hMenu);
	

	return ret;
}


void MyHook_Initialize()
{
	HMODULE hmod = LoadLibrary(TEXT("User32.dll"));


	apiHook_TPMEx = APIHook_CreateHook(OpenProcess(PROCESS_ALL_ACCESS, 0, GetCurrentProcessId()),
		GetProcAddress(hmod, "TrackPopupMenuEx"),
		MyTrackPopupMenuEx);


	apiHook_TPM = APIHook_CreateHook(OpenProcess(PROCESS_ALL_ACCESS, 0, GetCurrentProcessId()),
		GetProcAddress(hmod, "TrackPopupMenu"),
		MyTrackPopupMenu);


	APIHook_TurnOnHook(apiHook_TPMEx);
	APIHook_TurnOnHook(apiHook_TPM);
}

void MyHook_Destroy() {
	APIHook_DestoryHook(apiHook_TPMEx);
	APIHook_DestoryHook(apiHook_TPM);
}