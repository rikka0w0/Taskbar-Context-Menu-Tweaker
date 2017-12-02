
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>

//msvcrt.lib;msvcrtd.lib
#ifdef _DEBUG
#pragma comment (linker, "/nodefaultlib:msvcrtd.lib")
#else
#pragma comment (linker, "/nodefaultlib:msvcrt.lib")
#endif

extern "C" void* __cdecl malloc(ULONG_PTR uSize);
extern "C" void __cdecl free(LPVOID pMemBlock);
