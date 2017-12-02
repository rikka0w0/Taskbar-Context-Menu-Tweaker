#include "MinCRT.h"

#pragma comment (linker, "/entry:_DllMainCRTStartup")
int __cdecl _DllMainCRTStartup(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved);
	return DllMain(hModule, ul_reason_for_call, lpReserved);
}


extern "C" void* __cdecl malloc(ULONG_PTR uSize) {
	return (void *)HeapAlloc(GetProcessHeap(), 0, uSize);
}

extern "C" void __cdecl free(LPVOID pMemBlock) {
	HeapFree(GetProcessHeap(), 0, (LPVOID)pMemBlock);
}

#pragma function(memcpy)
void* __cdecl memcpy(void * dst, const void * src, size_t count)
{
	void * ret = dst;

#if defined (_M_IA64)
	{
		extern void RtlMoveMemory(void *, const void *, size_t count);

		RtlMoveMemory(dst, src, count);
	}
#else /* defined (_M_IA64) */
	/*
	* copy from lower addresses to higher addresses
	*/
	while (count--) {
		*(char *)dst = *(char *)src;
		dst = (char *)dst + 1;
		src = (char *)src + 1;
	}
#endif /* defined (_M_IA64) */

	return(ret);
}

#pragma function(memset)
void* __cdecl memset(void *src, int c, size_t count)
{
	char *tmpsrc = (char*)src;
	while (count--)
		*tmpsrc++ = (char)c;
	return src;
}

#pragma function(strcmp)
int __cdecl strcmp(const char* src, const char* dst)
{
	int   ret = 0;

	while (!(ret = *(unsigned   char   *)src - *(unsigned   char   *)dst) && *dst)
		++src, ++dst;

	if (ret   <   0)
		ret = -1;
	else   if (ret   >   0)
		ret = 1;

	return(ret);
}

#pragma function(strlen)
size_t __cdecl strlen(char const* _Str) {
	if (_Str == NULL)
		return 0;

	size_t len = 0;
	for (; *_Str++ != '\0';)
		len++;

	return len;
}