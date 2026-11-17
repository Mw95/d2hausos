#define D2_THIS_FILE "C:\\D2\\Source\\D2DDraw\\Src\\DllMain.cpp"
#include <windows.h>
#include "StormImports.h"

extern const char *const gpszModuleName = "D2DDRAW.DLL";

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	return TRUE;
}

void __cdecl operator delete(void *pMemory)
{
	if (pMemory)
		StormSMemFree(pMemory, "delete", -1, 0);
}

void * __cdecl operator new(unsigned int nSize)
{
	return StormSMemAlloc(nSize, "new", -1, 0);
}
