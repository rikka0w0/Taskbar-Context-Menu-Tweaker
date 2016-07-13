#pragma once

typedef struct APIHOOK *LPAPIHOOK;

LPAPIHOOK APIHook_CreateHook(HANDLE hProc, FARPROC oldFunc, void* newFunc);
void APIHook_DestoryHook(LPAPIHOOK hook);
void APIHook_TurnOnHook(LPAPIHOOK hook);
void APIHook_TurnOffHook(LPAPIHOOK hook);