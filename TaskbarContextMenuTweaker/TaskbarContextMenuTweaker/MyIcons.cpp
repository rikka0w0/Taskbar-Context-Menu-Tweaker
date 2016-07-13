#include "stdafx.h"

#define ICON_SETTING_INDEX 315
#define ICON_SHOWDESKTOP_INDEX 34

HBITMAP MyIcons[MyIcons_Count];

HBITMAP MyIcons_Get(unsigned char index) {
	return MyIcons[index];
}

void MyIcons_Load() {
	HICON hIcon = NULL;
	ExtractIconEx(L"shell32.dll", ICON_SETTING_INDEX, NULL, &hIcon, 1);
	MyIcons[MYICON_SETTING] = IconToBitmap(hIcon, DPI_SCALE(16));
	DestroyIcon(hIcon);

	ExtractIconEx(L"taskmgr.exe", 0, NULL, &hIcon, 1);
	MyIcons[MYICON_TASKMGR] = IconToBitmap(hIcon, DPI_SCALE(16));
	DestroyIcon(hIcon);

	ExtractIconEx(L"shell32.dll", ICON_SHOWDESKTOP_INDEX, NULL, &hIcon, 1);
	MyIcons[MYICON_SHOWDESKTOP] = IconToBitmap(hIcon, DPI_SCALE(16));
	DestroyIcon(hIcon);
}

void MyIcons_Free() {
	for (int i = 0; i < MyIcons_Count; i++)
		if (MyIcons[i] != NULL)
			DeleteObject(MyIcons[i]);
}