#define D2_THIS_FILE "C:\\D2\\Source\\D2DDraw\\Src\\ddBink.cpp"
#include "ddCore.h"
#include "StormImports.h"
#include "D2DDrawImports.h"
#include "D2Macros.h"

struct BINK
{
 	DWORD		dwWidth;
 	DWORD		dwHeight;
 	DWORD		field_08;
 	DWORD		field_0c;
};

static DWORD sgnBinkX = 0;
static DWORD sgnBinkY = 0;
static BINK *sgptBink = NULL;
static void *sgpBinkBuffer = NULL;
static DWORD sgdwBinkSurfaceType = 0;
static BOOL sgfBinkStop = FALSE;
static BOOL sgfBinkQuit = FALSE;
static DWORD sgdwBinkSkipTick = 0;

void __fastcall DDrawPlayBinkFile(const char *pszFileName, int nMode, void (__stdcall *pfnTick)(void))
{
	HANDLE hFile;
	BOOL fSkip;
	DWORD dwEndTick;
	DWORD dwQuality;
	DWORD dwFlags;
	MSG tMsg;
	DDSURFACEDESC tDesc;

	fSkip = FALSE;
	sgfBinkStop = FALSE;
	if (sgptBink || gfSuspended)
		return;
	sgdwBinkSurfaceType = BinkDDSurfaceType((DWORD)glpddsPrimary);
	BinkSetSoundSystem((DWORD)BinkOpenDirectSound, 0);
	dwQuality = 4;
	if (FogSafesockGetFileOption())
		dwQuality = 5;
	StormSFileOpenFileEx(0, pszFileName, dwQuality, &hFile);
	if (!hFile)
		FogErrorManagerLog("DDrawPlayBinkFile: *** Couldn't load Bink movie '%s'.", pszFileName);
	else
	{
		dwFlags = 8388608;
		switch (nMode)
		{
		case 1:
			dwFlags = 813694976;
			break;

		case 2:
			dwFlags = 276824064;
			break;

		case 3:
			dwFlags = 1082130432;
			break;
		}
		sgptBink = (BINK *)BinkOpen((DWORD)hFile, dwFlags);
		if (!sgptBink)
		{
			FogErrorManagerLog("BinkOpen failed on '%'.", pszFileName);
			CloseHandle(hFile);
		}
		else
		{
			sgnBinkX = (640 - sgptBink->dwWidth) >> 1;
			if (sgnBinkX & 3)
				sgnBinkX = (sgnBinkX | 3) + 1;
			sgnBinkY = (480 - sgptBink->dwHeight) >> 1;
			dwEndTick = GetTickCount() + 500;
			for (;;)
			{
				if (sgfBinkStop && GetTickCount() >= dwEndTick)
					break;
				if (gfSuspended)
					break;
				if (pfnTick)
					pfnTick();
				if (fSkip)
					break;
				if (PeekMessageA(&tMsg, NULL, 0, 0, PM_REMOVE))
				{
					switch (tMsg.message)
					{
					case WM_QUIT:
						sgfBinkStop = TRUE;
						sgfBinkQuit = TRUE;
						break;

					case WM_CHAR:
					case WM_LBUTTONDOWN:
					case WM_RBUTTONDOWN:
					case WM_MBUTTONDOWN:
						fSkip = TRUE;
						sgdwBinkSkipTick = GetTickCount();
						break;
					}
					TranslateMessage(&tMsg);
					DispatchMessageA(&tMsg);
					continue;
				}
				if (!BinkWait((DWORD)sgptBink))
				{
					if (!ghWnd)
						continue;
					BinkDoFrame((DWORD)sgptBink);
#line 69
					D2ASSERT(pcsPause);
					EnterCriticalSection(pcsPause);
					tDesc.dwSize = sizeof(tDesc);
					memset(&tDesc.dwFlags, 0, sizeof(tDesc) - sizeof(DWORD));
					if (!glpddsPrimary->Lock(NULL, &tDesc, 33, NULL))
					{
						BinkCopyToBuffer((DWORD)sgptBink, (DWORD)tDesc.lpSurface, tDesc.lPitch,
							sgptBink->dwHeight, sgnBinkX, sgnBinkY, sgdwBinkSurfaceType);
						glpddsPrimary->Unlock(NULL);
					}
					LeaveCriticalSection(pcsPause);
					if (sgptBink->field_0c == sgptBink->field_08)
						sgfBinkStop = TRUE;
					else
						BinkNextFrame((DWORD)sgptBink);
				}
				else
					Sleep(1);
			}
			if (sgptBink)
			{
				D2SoundSmpSetMasterVolume(0);
				BinkClose((DWORD)sgptBink);
				CloseHandle(hFile);
				D2SoundSmpSetMasterVolume(255);
				sgptBink = NULL;
			}
			if (sgfBinkQuit)
				exit(-1);
			return;
		}
	}
	if (sgptBink)
	{
		BinkClose((DWORD)sgptBink);
		sgptBink = NULL;
	}
}
