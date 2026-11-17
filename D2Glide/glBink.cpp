#define D2_THIS_FILE "C:\\D2\\Source\\D2Glide\\Src\\glBink.cpp"
#include "glCore.h"
#include "StormImports.h"
#include "D2GlideImports.h"
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
static DWORD PLACEHOLDER_UnreferencedData_1001d7dc = 0;
static DWORD sgdwBinkSurfaceType = 0;
static BOOL sgfBinkStop = 0;
static DWORD sgdwBinkSkipTick = 0;

void __fastcall GlidePlayBinkFile(const char *pszFileName, int nMode, void (__stdcall *pfnTick)(void))
{
	HANDLE hFile;
	BOOL fSkip;
	DWORD dwEndTick;
	DWORD dwQuality;
	DWORD dwFlags;
	GLLFBINFO tInfo;
	MSG tMsg;

	fSkip = FALSE;
	sgfBinkStop = FALSE;
	if (sgptBink || gfSuspended)
		return;
	sgdwBinkSurfaceType = 10;
	BinkSetSoundSystem((DWORD)BinkOpenDirectSound, 0);
	dwQuality = 4;
	if (FogSafesockGetFileOption())
		dwQuality = 5;
	StormSFileOpenFileEx(0, pszFileName, dwQuality, &hFile);
	if (!hFile)
		FogErrorManagerLog("GlidePlayBinkFile: *** Couldn't load Bink movie \"%s\".", pszFileName);
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
						BinkDoFrame((DWORD)sgptBink);
#line 60
						D2ASSERT(pcsPause);
						EnterCriticalSection(pcsPause);
						if (grLfbLock(1, 0, 0, 0, 0, &tInfo))
						{
							BinkCopyToBuffer((DWORD)sgptBink, (DWORD)tInfo.pbBuffer, tInfo.nPitch,
								sgptBink->dwHeight, sgnBinkX, sgnBinkY, sgdwBinkSurfaceType);
							grLfbUnlock(1, 0);
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
			return;
		}
	}
	if (sgptBink)
	{
		BinkClose((DWORD)sgptBink);
		sgptBink = NULL;
	}
}
