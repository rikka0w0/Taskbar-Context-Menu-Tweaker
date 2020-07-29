#include "tmt.h"

#include <TlHelp32.h>

#ifndef PSAPI_VERSION
#define PSAPI_VERSION 1
#endif
#include <psapi.h>
#pragma  comment(lib,"Psapi.lib")


#if _MSC_VER >= 1300    // for VC 7.0
// from ATL 7.0 sources
#ifndef _delayimp_h
extern "C" IMAGE_DOS_HEADER __ImageBase;
#endif
#endif

HMODULE GetCurrentModule()
{
#if _MSC_VER < 1300    // earlier than .NET compiler (VC 6.0)

    // Here's a trick that will get you the handle of the module
    // you're running in without any a-priori knowledge:
    // http://www.dotnet247.com/247reference/msgs/13/65259.aspx

    MEMORY_BASIC_INFORMATION mbi;
    static int dummy;
    VirtualQuery(&dummy, &mbi, sizeof(mbi));

    return reinterpret_cast(mbi.AllocationBase);

#else    // VC 7.0

    // from ATL 7.0 sources

    return reinterpret_cast<HMODULE>(&__ImageBase);
#endif
}

char* GetDLLPath() { //Caller should free the allocated memory!
    char* dllPath = (char*)malloc(sizeof(char) * MAX_PATH);
    GetModuleFileNameA(GetCurrentModule(), dllPath, MAX_PATH);
    return dllPath;
}

bool InjectDLL(HWND targetHwnd, LPCSTR lpDllName, LPCSTR lpszCmdLine) {
    DWORD ProcessID;
    DWORD ThreadID = GetWindowThreadProcessId(targetHwnd, &ProcessID);

    HANDLE hProcess = OpenProcess(0xFFF, FALSE, ProcessID);	//Full Access
    if (hProcess == NULL) {
        MessageBox(0, TEXT("Unable to obtain the explorer handler"), TEXT("Error during injection"), MB_ICONERROR);
        return false;
    }


#ifdef _M_IX86
    DWORD dwSize;
    DWORD dwHasWrite;
#endif

#ifdef _M_X64
    SIZE_T dwSize;
    SIZE_T dwHasWrite;
#endif

    dwSize = strlen(lpDllName) + 1;

    LPVOID lpRemoteBuf = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
    if (WriteProcessMemory(hProcess, lpRemoteBuf, lpDllName, dwSize, &dwHasWrite)) {
        if (dwHasWrite != dwSize) {
            VirtualFreeEx(hProcess, lpRemoteBuf, dwSize, MEM_COMMIT);
            MessageBox(0, L"Unrecognized error during remote injection!", L"Error during injection", MB_ICONERROR);
            CloseHandle(hProcess);
            return false;
        }
    }
    else {
        MessageBox(0, L"Fail to write the remote process memory", L"Error during injection", MB_ICONERROR);
        CloseHandle(hProcess);
        return false;
    }

    DWORD dwNewThreadId;
    LPVOID lpLoadDll = LoadLibraryA;
    HANDLE hNewRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpLoadDll, lpRemoteBuf, 0, &dwNewThreadId);
    if (hNewRemoteThread == NULL) {
        MessageBox(0, L"Fail to build the remote thread \"LoadLibraryA\"", L"Error during injection", MB_ICONERROR);
        CloseHandle(hProcess);
        return false;
    }

    WaitForSingleObject(hNewRemoteThread, INFINITE);
    CloseHandle(hNewRemoteThread);


    //Initialize ---------------------------------------------------------------------------------------------
    lpLoadDll = GetProcAddress(GetCurrentModule(), "__TweakerInit");
    if (lpLoadDll == NULL) {
        MessageBox(0, L"Unable to locate the remote thread \"__TweakerInit\"", L"Error during injection", MB_ICONERROR);
        CloseHandle(hProcess);
        return false;
    }

	size_t cmdLineLen = strlen(lpszCmdLine);
	LPVOID paramsCall = VirtualAllocEx(hProcess, NULL, sizeof(HWND) + sizeof(char) * (cmdLineLen + 1), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (paramsCall == NULL) {
		MessageBox(0, L"Unable to locate remote memory for parameters", L"Error during injection", MB_ICONERROR);
		CloseHandle(hProcess);
		return false;
	}

	if (!WriteProcessMemory(hProcess, paramsCall, &targetHwnd, sizeof(HWND), NULL)
		||
		!WriteProcessMemory(hProcess, (char*)paramsCall + sizeof(HWND), lpszCmdLine, sizeof(CHAR) * (cmdLineLen + 1), NULL)) {
		MessageBox(0, L"Unable to write parameters to remote memory", L"Error during injection", MB_ICONERROR);
		CloseHandle(hProcess);
		return false;
	}

    hNewRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpLoadDll, paramsCall, 0, &dwNewThreadId);
    if (hNewRemoteThread == NULL) {
        MessageBox(0, L"Fail to start the remote thread \"__TweakerInit\"", L"Error during injection", MB_ICONERROR);
        CloseHandle(hProcess);
        return false;
    }

    WaitForSingleObject(hNewRemoteThread, INFINITE);
    CloseHandle(hNewRemoteThread);
    CloseHandle(hProcess);
    return true;
}

extern "C" _declspec(dllexport) void __cdecl Inject(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
    HWND targethWnd = NULL;


    targethWnd = FindWindow(TEXT("Shell_TrayWnd"), NULL);
    int t = 0;

    while (targethWnd == NULL) {
        Sleep(500);
        targethWnd = FindWindow(TEXT("Shell_TrayWnd"), NULL);
        t++;
        if (t > 60 && targethWnd == NULL) {
            MessageBox(0, TEXT("Fail to inject the dll into explorer.exe, please check the injection parameters!"), TEXT("Error during injection"), MB_ICONERROR);
            return;
        }
    }


    char* dllPath = GetDLLPath();

    InjectDLL(targethWnd, dllPath, lpszCmdLine);
    free(dllPath);

    return;
}

extern "C" _declspec(dllexport) void __cdecl Release(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	HWND hWndTaskBar = FindWindow(TEXT("Shell_TrayWnd"), NULL);

	if (hWndTaskBar == NULL) {
		MessageBox(0, L"Fail to locate the dll from explorer.exe", L"Error", MB_ICONERROR);
		return;
	}

	SendMessage(hWndTaskBar, WM_TWEAKER, TWEAKER_EXIT, 0);

	return;
}