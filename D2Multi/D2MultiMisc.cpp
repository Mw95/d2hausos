#define D2_THIS_FILE "C:\\D2\\Source\\D2Multi\\Src\\D2MultiMisc.cpp"
#include "D2Multi.h"

static HINSTANCE sghInstance = 0;
static DWORD sgadw10028934[3] = { 0 };

static _STDINT sgtInterface =
{
	ChatDlgRun,
};

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	sghInstance = hInstance;
	return TRUE;
}

_STDINT * __cdecl QueryInterface(void)
{
	return &sgtInterface;
}

extern const char *const gpszD2MultiModuleName = "D2Multi";
