#include <windows.h>

static DWORD sgdwOrdinal10001Value;

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason,
	LPVOID lpReserved)
{
	return TRUE;
}

DWORD __cdecl ArenaGetUnwrittenValue(void)
{
	return sgdwOrdinal10001Value;
}
