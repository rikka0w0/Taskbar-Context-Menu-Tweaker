#include "stdafx.h"

#define CONFIG_REG_KEY "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer"
#define CONFIG_REG_CMC "ContextMenuConfig"
#define CONFIG_REG_TMTITLE "ToggleMenuTitle"

#define CONFIG_USEIMMERSIVEMENU 0x01
#define CONFIG_HASICON 0x02

LPWSTR GetToggleMenuTitle() {
	HKEY hKey;
	RegOpenKeyEx(HKEY_CURRENT_USER, TEXT(CONFIG_REG_KEY), 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
	DWORD dwType;
	DWORD dwSize;
	bool ret = RegQueryValueEx(hKey, TEXT(CONFIG_REG_TMTITLE), 0, &dwType, NULL, &dwSize) == ERROR_SUCCESS;

	if (!ret)
		return NULL;

	LPWSTR title = (LPWSTR)malloc(sizeof(WCHAR) * dwSize);

	RegQueryValueEx(hKey, TEXT(CONFIG_REG_TMTITLE), 0, &dwType, (LPBYTE)title, &dwSize);
	return title;
}

void SetMyConfig(DWORD d) {
	HKEY hKey;
	RegOpenKeyEx(HKEY_CURRENT_USER, TEXT(CONFIG_REG_KEY), 0, KEY_SET_VALUE, &hKey);
	RegSetValueEx(hKey, TEXT(CONFIG_REG_CMC), 0, REG_DWORD, (LPBYTE)&d, sizeof(d));
	RegCloseKey(hKey);
}

DWORD GetMyConfig() {
	HKEY hKey;
	RegOpenKeyEx(HKEY_CURRENT_USER, TEXT(CONFIG_REG_KEY), 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
	DWORD dwType = REG_DWORD;
	DWORD dwSize = 4;
	DWORD data;
	bool ret = RegQueryValueEx(hKey, TEXT(CONFIG_REG_CMC), 0, &dwType, (LPBYTE)&data, &dwSize) == ERROR_SUCCESS;

	if (!ret) {
		data = 0;
		RegSetValueEx(hKey, TEXT(CONFIG_REG_CMC), 0, REG_DWORD, (LPBYTE)&data, sizeof(data));
	}

	RegCloseKey(hKey);
	return data;
}


int GetDPI() {
	HDC hdc = GetDC(NULL);
	int DPI = GetDeviceCaps(hdc, LOGPIXELSX);
	ReleaseDC(NULL, hdc);
	return DPI;
}

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

HBITMAP IconToBitmap(HICON hIcon, INT size = 0)
{
	ICONINFO info = { 0 };
	if (hIcon == NULL
		|| !GetIconInfo(hIcon, &info)
		|| !info.fIcon)
	{
		//TO-DO: Free the hbmColor and hbmMask 
		return NULL;
	}

	INT nWidth = 0;
	INT nHeight = 0;
	if (size > 0)
	{
		nWidth = size;
		nHeight = size;
	}
	else
	{
		if (info.hbmColor != NULL)
		{
			BITMAP bmp = { 0 };
			GetObject(info.hbmColor, sizeof(bmp), &bmp);

			nWidth = bmp.bmWidth;
			nHeight = bmp.bmHeight;
		}
	}


	if (info.hbmColor != NULL)

	{

		DeleteObject(info.hbmColor);

		info.hbmColor = NULL;

	}



	if (info.hbmMask != NULL)

	{

		DeleteObject(info.hbmMask);

		info.hbmMask = NULL;

	}


	if (nWidth <= 0
		|| nHeight <= 0)
	{
		return NULL;
	}

	INT nPixelCount = nWidth * nHeight;

	HDC dc = GetDC(NULL);
	INT* pData = NULL;
	HDC dcMem = NULL;
	HBITMAP hBmpOld = NULL;
	bool* pOpaque = NULL;
	HBITMAP dib = NULL;
	BOOL bSuccess = FALSE;

	do
	{
		BITMAPINFOHEADER bi = { 0 };
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = nWidth;
		bi.biHeight = -nHeight;
		bi.biPlanes = 1;
		bi.biBitCount = 32;
		bi.biCompression = BI_RGB;
		dib = CreateDIBSection(dc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (VOID**)&pData, NULL, 0);
		if (dib == NULL) break;

		memset(pData, 0, nPixelCount * 4);

		dcMem = CreateCompatibleDC(dc);
		if (dcMem == NULL) break;

		hBmpOld = (HBITMAP)SelectObject(dcMem, dib);
		::DrawIconEx(dcMem, 0, 0, hIcon, nWidth, nHeight, 0, NULL, DI_MASK);

		pOpaque = (bool*)malloc(sizeof(bool)*nPixelCount);
		if (pOpaque == NULL) break;
		for (INT i = 0; i < nPixelCount; ++i)
		{
			pOpaque[i] = !pData[i];
		}

		memset(pData, 0, nPixelCount * 4);
		::DrawIconEx(dcMem, 0, 0, hIcon, nWidth, nHeight, 0, NULL, DI_NORMAL);

		BOOL bPixelHasAlpha = FALSE;
		UINT* pPixel = (UINT*)pData;
		for (INT i = 0; i<nPixelCount; ++i, ++pPixel)
		{
			if ((*pPixel & 0xff000000) != 0)
			{
				bPixelHasAlpha = TRUE;
				break;
			}
		}

		if (!bPixelHasAlpha)
		{
			pPixel = (UINT*)pData;
			for (INT i = 0; i <nPixelCount; ++i, ++pPixel)
			{
				if (pOpaque[i])
				{
					*pPixel |= 0xFF000000;
				}
				else
				{
					*pPixel &= 0x00FFFFFF;
				}
			}
		}

		bSuccess = TRUE;

	} while (FALSE);


	if (pOpaque != NULL)
	{
		free(pOpaque);
		pOpaque = NULL;
	}

	if (dcMem != NULL)
	{
		SelectObject(dcMem, hBmpOld);
		DeleteDC(dcMem);
	}

	ReleaseDC(NULL, dc);

	if (!bSuccess)
	{
		if (dib != NULL)
		{
			DeleteObject(dib);
			dib = NULL;
		}
	}

	return dib;
}














