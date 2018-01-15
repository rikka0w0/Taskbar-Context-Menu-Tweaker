#include "tmt.h"


#define CONFIG_REG_KEY "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer"
#define CONFIG_REG_CMC "ContextMenuConfig"
#define CONFIG_REG_TMTITLE "ToggleMenuTitle"

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