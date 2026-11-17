#define D2_THIS_FILE "C:\\D2\\Source\\D2Direct3D\\Src\\d3dBink.cpp"
#include "d3dCore.h"
#include "StormImports.h"
#include "D2Direct3DImports.h"
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
static BINK *sgptBink = 0;
static DWORD PLACEHOLDER_UnreferencedData_10025510 = 0;
static DWORD sgdwBinkSurfaceType = 0;
static BOOL sgfBinkStop = 0;
static BOOL sgfBinkQuit = 0;
static DWORD sgdwBinkSkipTick = 0;

void __fastcall Direct3DPlayBinkFile(const char *pszFileName, int nMode, void (__stdcall *pfnTick)(void))
{
	HANDLE hFile;
	BOOL fSkip;
	DWORD dwEndTick;
	DWORD dwQuality;
	DWORD dwFlags;
	MSG tMsg;
	D3DSURFDESC tDesc;

	fSkip = FALSE;
	sgfBinkStop = FALSE;
	FogErrorManagerLog("About to play bink file");
	if (sgptBink || gfSuspended)
		return;
	sgdwBinkSurfaceType = BinkDDSurfaceType((DWORD)glpddsPrimary);
	BinkSetSoundSystem((DWORD)BinkOpenDirectSound, 0);
	dwQuality = 4;
	if (FogSafesockGetFileOption())
		dwQuality = 5;
	StormSFileOpenFileEx(0, pszFileName, dwQuality, &hFile);
	if (!hFile)
		FogErrorManagerLog("Direct3DPlayBinkFile: *** Couldn't load Bink movie \"%s\".", pszFileName);
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
			while (!(sgfBinkStop && GetTickCount() >= dwEndTick) && !gfSuspended && !fSkip)
			{
				if (pfnTick)
					pfnTick();
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
					if (ghWnd)
					{
						tDesc.dwSize = sizeof(tDesc);
						memset(&tDesc.dwFlags, 0, sizeof(tDesc) - sizeof(DWORD));
						BinkDoFrame((DWORD)sgptBink);
#line 64
						D2ASSERT(pcsPause);
						EnterCriticalSection(pcsPause);
						if (!glpddsPrimary->lpVtbl->pfnLock(glpddsPrimary, NULL, &tDesc, 33, NULL))
						{
							BinkCopyToBuffer((DWORD)sgptBink, (DWORD)tDesc.pbSurface, tDesc.nPitch,
								sgptBink->dwHeight, sgnBinkX, sgnBinkY, sgdwBinkSurfaceType);
							glpddsPrimary->lpVtbl->pfnUnlock(glpddsPrimary, NULL);
						}
						LeaveCriticalSection(pcsPause);
						if (sgptBink->field_0c == sgptBink->field_08)
							sgfBinkStop = TRUE;
						else
							BinkNextFrame((DWORD)sgptBink);
					}
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
