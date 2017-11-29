#include "tmt.h"



void ClassicMenu(HMENU hMenu) {
	MENUINFO info;
	info.cbSize = sizeof(MENUINFO);
	info.fMask = MIM_BACKGROUND;
	GetMenuInfo(hMenu, &info);
	info.hbrBack = nullptr;
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
	} else if (strcmp(clsName, "NotificationsMenuOwner") == 0) {
		ClassicMenu(hMenu);
	} else if (strcmp(clsName, "LauncherTipWnd") == 0) {
		ClassicMenu(hMenu);
		for (int i = 0; i < GetMenuItemCount(hMenu); i++)
			ClassicMenu(GetSubMenu(hMenu, i));
	}
	else if (strcmp(clsName, "MultitaskingViewFrame") == 0) {
		ClassicMenu(hMenu);
		for (int i = 0; i < GetMenuItemCount(hMenu); i++)
			ClassicMenu(GetSubMenu(hMenu, i));
	} else {
		ClassicMenu(hMenu);
	}
}
