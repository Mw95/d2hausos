#include "D2Launch.h"

static HINSTANCE sghInstance = 0;

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	sghInstance = hInstance;
	return TRUE;
}

static _STDINT sgtInterface =
{
	MainMenusMain
};

_STDINT * __cdecl QueryInterface(void)
{
	return &sgtInterface;
}

extern const char *const gpszD2LaunchModuleName = "D2Launch.dll";
