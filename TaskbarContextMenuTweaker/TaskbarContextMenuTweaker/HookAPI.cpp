#include "stdafx.h"
#include "HookAPI.h"


typedef struct APIHOOK {
#ifdef _AMD64_
	BYTE OldCode[12];
	BYTE NewCode[12];
#else
	BYTE OldCode[5];
	BYTE NewCode[5];
#endif	
	FARPROC oldProc;
	HANDLE hP;
	bool status;
}APIHOOK;

void APIHook_TurnOnHook(LPAPIHOOK hook) {
	if (hook == NULL)
		return;

	DWORD dwTemp = 0;
	DWORD dwOldProtect;

#ifdef _AMD64_
	VirtualProtectEx(hook->hP, hook->oldProc, 12, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hook->hP, hook->oldProc, hook->NewCode, 12, 0);
	VirtualProtectEx(hook->hP, hook->oldProc, 12, dwOldProtect, &dwTemp);
#else
	VirtualProtectEx(hook->hP, hook->oldProc, 5, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hook->hP, hook->oldProc, hook->NewCode, 5, 0);
	VirtualProtectEx(hook->hP, hook->oldProc, 5, dwOldProtect, &dwTemp);
#endif

	hook->status = true;
}

void APIHook_TurnOffHook(LPAPIHOOK hook) {
	if (hook == NULL)
		return;

	DWORD dwTemp = 0;
	DWORD dwOldProtect;


#ifdef _AMD64_
	VirtualProtectEx(hook->hP, hook->oldProc, 12, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hook->hP, hook->oldProc, hook->OldCode, 12, 0);
	VirtualProtectEx(hook->hP, hook->oldProc, 12, dwOldProtect, &dwTemp);
#else
	VirtualProtectEx(hook->hP, hook->oldProc, 5, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hook->hP, hook->oldProc, hook->OldCode, 5, 0);
	VirtualProtectEx(hook->hP, hook->oldProc, 5, dwOldProtect, &dwTemp);
#endif

	hook->status = false;
}

LPAPIHOOK APIHook_CreateHook(HANDLE hProc, FARPROC oldFunc, void* newFunc) {
	LPAPIHOOK ret = (LPAPIHOOK)malloc(sizeof(APIHOOK));
	ret->hP = hProc;
	ret->oldProc = oldFunc;

	if (ret->oldProc == NULL)
		return NULL;


#ifdef _AMD64_
	memcpy(ret->OldCode, (BYTE*)(ret->oldProc), 12);

	ret->NewCode[0] = 0x48;
	ret->NewCode[1] = 0xB8;
	ULONG64 addr = (ULONG64)newFunc;
	memcpy(ret->NewCode + 2, &addr, 8);
	ret->NewCode[10] = 0x50;
	ret->NewCode[11] = 0xC3;
#else
	memcpy(ret->OldCode, (BYTE*)(ret->oldProc), 5);

	ret->NewCode[0] = 0xe9; //jmp
	ULONG32 aNew = (ULONG32)newFunc - (ULONG32)(void*)(ret->oldProc) - 5;
	memcpy(ret->NewCode + 1, &aNew, 4);
#endif

	ret->status = false;

	return ret;
}

void APIHook_DestoryHook(LPAPIHOOK hook) {
	if (hook == NULL)
		return;
	if (hook->status)
		APIHook_TurnOffHook(hook);
	free(hook);
}