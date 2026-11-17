#define D2_THIS_FILE "C:\\D2\\Source\\D2DDraw\\Src\\ddCore.cpp"
#include "ddCore.h"
#include "StormImports.h"
#include "D2DDrawImports.h"
#include "D2Macros.h"

char gszRegistrationBlock2[128] = "REGISTRATION_BLOCK";
static int sganScreenWidth[2] = { 640, 800 };
static int sganScreenHeight[2] = { 480, 600 };
static int sgnGamma = 100;

static PALETTEENTRY sgatPaletteSource[256] = { 0 };
DDENVINFO *gptEnvInfo = NULL;
HWND ghWnd = NULL;
static DDENVINFO sgtEnvInfo = { 0 };
DDVIDEOCONFIG gtVideoConfig = { 0 };
DDDRAWCTX *gptDrawCtx = NULL;
static int sgnModeHeight = 0;
BYTE *gpbSurface = NULL;
static int sgnModeWidth = 0;
int gnBufferWidth = 0;
PALETTEENTRY gatPaletteEntries[256] = { 0 };
DDBLITTABLE *gptBlitTable = NULL;
int gnBufferHeight = 0;
static BOOL sgfDirectX3 = FALSE;
static BOOL sgfInitialized = FALSE;
static BOOL sgfWindowOpen = FALSE;
static BOOL sgfWindowState = FALSE;
static BOOL sgfCutscene = FALSE;
static LPDIRECTDRAW sglpDD = NULL;
static IUnknown *sglpUnknown = NULL;
LPDIRECTDRAWSURFACE glpddsPrimary = NULL;
static LPDIRECTDRAWSURFACE sglpddsBack = NULL;
LPDIRECTDRAWSURFACE sglpddsRender = NULL;
static LPDIRECTDRAWSURFACE PLACEHOLDER_UnreferencedData_10019510 = NULL;
static LPDIRECTDRAWSURFACE PLACEHOLDER_UnreferencedData_10019514 = NULL;
static LPDIRECTDRAWCLIPPER PLACEHOLDER_UnreferencedData_10019518 = NULL;
static LPDIRECTDRAWPALETTE sglpPalette = NULL;
static int sgnBackBuffers = 0;
DWORD gfSuspended = FALSE;
CRITICAL_SECTION *pcsPause = NULL;
static BOOL sgfPaletteDirty = FALSE;

static void __cdecl DdShutdown(void);
static BOOL __fastcall DdOpenWindow(int nResolution);
static void WINAPI DdCloseWindow(void);
BOOL WINAPI DdCloseWindowAndPalette(void);
BOOL __fastcall DdPresent(void);

BOOL WINAPI DdDetectBoards(void)
{
	ERRORMANAGERSYSINFO *tSystemInfo;
	DWORD dwSize;

	tSystemInfo = (ERRORMANAGERSYSINFO *)FogErrorManagerGetSysInfo();
#line 95
	D2ASSERT(tSystemInfo);
	if (tSystemInfo->dwOsType == 3 && !(tSystemInfo->flVersion >= 5.0f))
		sgfDirectX3 = TRUE;
	if (sgfDirectX3)
		FogErrorManagerLog("Restricted to DirectX 3 support.");
	FogErrorManagerLog("Detecting boards which support DirectDraw...");
	char szKey[] = "Diablo II\\VideoConfig";
	if (StormSRegLoadString(szKey, "DeviceName", 0, gtVideoConfig.szDeviceName, sizeof(gtVideoConfig.szDeviceName))
		&& StormSRegLoadValue(szKey, "dwFlags", 0, &gtVideoConfig.dwFlags))
	{
		if (gdwBitMasks[0] & gtVideoConfig.dwFlags)
			return TRUE;
		dwSize = sizeof(gtVideoConfig.abGuid);
		if (StormSRegLoadData(szKey, "GUID", 0, gtVideoConfig.abGuid, sizeof(gtVideoConfig.abGuid), &dwSize)
			&& dwSize == sizeof(gtVideoConfig.abGuid))
			return TRUE;
	}
	strcpy(gtVideoConfig.szDeviceName, "Fallback DirectDraw Device");
	gtVideoConfig.dwFlags = 0;
	gtVideoConfig.dwFlags = gdwBitMasks[0];
	return TRUE;
}

BOOL __fastcall DdSetup(DDDRAWCTX *ptDrawCtx, DDBLITTABLE *ptBlitTable)
{
	HRESULT hResult;

	gptDrawCtx = ptDrawCtx;
	gptBlitTable = ptBlitTable;
	DdCreateDebugFont();
	FogErrorManagerLog("Initializing for DirectDraw on %s", gtVideoConfig.szDeviceName);
	if (gtVideoConfig.dwFlags & gdwBitMasks[1])
		FogErrorManagerLog("\t%s: Yes", "32 Bit Cutscenes");
	else
		FogErrorManagerLog("\t%s: No", "32 Bit Cutscenes");
	if (gtVideoConfig.dwFlags & gdwBitMasks[2])
		FogErrorManagerLog("\t%s: Yes", "24 Bit Cutscenes");
	else
		FogErrorManagerLog("\t%s: No", "24 Bit Cutscenes");
	if (gdwBitMasks[0] & gtVideoConfig.dwFlags)
		hResult = DirectDrawCreate(NULL, &sglpDD, NULL);
	else
		hResult = DirectDrawCreate((GUID *)gtVideoConfig.abGuid, &sglpDD, NULL);
	if (hResult)
	{
		DdLogError("DirectDrawCreate failed!", hResult);
		return FALSE;
	}
	atexit(DdShutdown);
	sgfInitialized = TRUE;
	FogErrorManagerLog("\n----------------------------------------------\n");
#line 221
	pcsPause = (CRITICAL_SECTION *)D2ALLOC2(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection(pcsPause);
	return TRUE;
}

static void __cdecl DdShutdown(void)
{
	if (!sgfInitialized)
		return;
	DdCloseWindowAndPalette();
	FogErrorManagerLog("Shutting down DirectDraw...");
	do { if (sglpUnknown) { sglpUnknown->Release(); sglpUnknown = NULL; } } while (0);
	do { if (sglpDD) { sglpDD->Release(); sglpDD = NULL; } } while (0);
	FogErrorManagerLog("...DirectDraw shut down.");
	if (pcsPause)
	{
		DeleteCriticalSection(pcsPause);
#line 264
		D2FREE2(pcsPause);
		pcsPause = NULL;
	}
	DdFreeDebugFont();
	sgfInitialized = FALSE;
}

BOOL __fastcall DdTeardown(void)
{
	DdShutdown();
	return TRUE;
}

void WINAPI DdNothingFlushCaches(void)
{
}

BOOL __fastcall DdInit(HWND hWnd, int nResolution)
{
	HRESULT hResult;

	ghWnd = hWnd;
	hResult = sglpDD->SetCooperativeLevel(hWnd, 17);
	if (hResult)
	{
		DdLogError("SetCooperativeLevel failed!", hResult);
		return FALSE;
	}
	memset(gatPaletteEntries, 0, sizeof(gatPaletteEntries));
	hResult = sglpDD->CreatePalette(4, gatPaletteEntries, &sglpPalette, NULL);
	if (hResult)
	{
		DdLogError("CreatePalette failed!", hResult);
		return FALSE;
	}
	return DdOpenWindow(nResolution);
}

static BOOL __fastcall DdOpenWindow(int nResolution)
{
	DDSURFACEDESC tDesc;
	DDSCAPS tCaps;
	HRESULT hResult;
	int nWidth;
	int nHeight;

#line 293
	D2ASSERT(!sgfWindowOpen);
	if (!nResolution)
	{
		nWidth = 640;
		gnBufferWidth = nWidth;
		nHeight = 480;
		gnBufferHeight = 480;
	}
	else
	{
		nWidth = 800;
		gnBufferWidth = nWidth;
		nHeight = 600;
		gnBufferHeight = 600;
	}
	FogErrorManagerLog("Opening DirectDraw window at %dx%d...", nWidth, nHeight);
	DdSmackInit(nResolution);
	sgnModeWidth = sganScreenWidth[nResolution];
	sgnModeHeight = sganScreenHeight[nResolution];
	hResult = sglpDD->SetCooperativeLevel(ghWnd, 17);
	if (hResult && hResult != (HRESULT)0x88760245)
	{
		DdLogError("SetCooperativeLevel failed!", hResult);
		return FALSE;
	}
#line 439
	D2ASSERT(sglpDD);
	hResult = sglpDD->SetDisplayMode(sgnModeWidth, sgnModeHeight, 8);
	if (hResult)
		DdLogError("SetDisplayMode failed!", hResult);
	else
	{
		memset(&tDesc, 0, sizeof(tDesc));
		tDesc.dwSize = sizeof(tDesc);
		tDesc.dwFlags = 33;
		tDesc.ddsCaps.dwCaps = 16920;
		tDesc.dwBackBufferCount = 2;
		sgnBackBuffers = 2;
		hResult = sglpDD->CreateSurface(&tDesc, &glpddsPrimary, NULL);
		if (hResult)
		{
			memset(&tDesc, 0, sizeof(tDesc));
			tDesc.dwSize = sizeof(tDesc);
			tDesc.dwFlags = 33;
			tDesc.ddsCaps.dwCaps = 16920;
			tDesc.dwBackBufferCount = 1;
			sgnBackBuffers = 1;
			hResult = sglpDD->CreateSurface(&tDesc, &glpddsPrimary, NULL);
			if (hResult)
			{
				memset(&tDesc, 0, sizeof(tDesc));
				sgnBackBuffers = 0;
				tDesc.dwSize = sizeof(tDesc);
				tDesc.dwFlags = 1;
				tDesc.ddsCaps.dwCaps = 16896;
				hResult = sglpDD->CreateSurface(&tDesc, &glpddsPrimary, NULL);
			}
		}
		if (hResult)
			DdLogError("CreateSurface (primary single buffered) failed!", hResult);
		else
		{
			if (sgnBackBuffers)
			{
				tCaps.dwCaps = 4;
				hResult = glpddsPrimary->GetAttachedSurface(&tCaps, &sglpddsBack);
			}
			if (hResult)
				DdLogError("GetAttachedSurface (backbuffer) failed!", hResult);
			else
			{
				memset(&tDesc, 0, sizeof(tDesc));
				tDesc.dwSize = sizeof(tDesc);
				hResult = glpddsPrimary->GetSurfaceDesc(&tDesc);
				if (hResult)
					DdLogError("GetSurfaceDesc (primary) failed!", hResult);
				else
				{
					tDesc.dwFlags = 7;
					tDesc.ddsCaps.dwCaps = 2112;
					hResult = sglpDD->CreateSurface(&tDesc, &sglpddsRender, NULL);
					if (hResult)
						DdLogError("CreateSurface (render) failed!", hResult);
					else
					{
						hResult = glpddsPrimary->SetPalette(sglpPalette);
						if (hResult)
							DdLogError("SetPalette failed!", hResult);
						else
						{
							sgfWindowOpen = TRUE;
							sgfWindowState = FALSE;
							switch (sgnBackBuffers)
							{
							case 0:
								FogErrorManagerLog("Opening DirectDraw window succeeded. (Single buffering)");
								return TRUE;

							case 1:
								FogErrorManagerLog("Opening DirectDraw window succeeded. (Double buffering)");
								return TRUE;

							case 2:
								FogErrorManagerLog("Opening DirectDraw window succeeded. (Triple buffering)");
								return TRUE;
							}
							FogErrorManagerLog("Opening DirectDraw window succeeded (somehow).");
							return TRUE;
						}
					}
				}
			}
		}
	}
	FogErrorManagerLog("Opening DirectDraw window failed!");
	return FALSE;
}

void __fastcall DdSetSuspended(HWND hWnd, int nResolution, BOOL fSuspend)
{
	HRESULT hResult;

#line 714
	D2ASSERT(pcsPause);
	EnterCriticalSection(pcsPause);
	if (fSuspend)
	{
		gfSuspended = TRUE;
		DdCloseWindowAndPalette();
	}
	else
	{
		gfSuspended = FALSE;
		ghWnd = hWnd;
		hResult = sglpDD->SetCooperativeLevel(hWnd, 17);
		if (hResult)
			DdLogError("SetCooperativeLevel failed!", hResult);
		else
		{
			memset(gatPaletteEntries, 0, sizeof(gatPaletteEntries));
			hResult = sglpDD->CreatePalette(4, gatPaletteEntries, &sglpPalette, NULL);
			if (hResult)
				DdLogError("CreatePalette failed!", hResult);
			else
				DdOpenWindow(nResolution);
		}
	}
	LeaveCriticalSection(pcsPause);
}

static void WINAPI DdCloseWindow(void)
{
	int i;

	if (!sgfWindowOpen)
		return;
	FogErrorManagerLog("Closing DirectDraw window...");
	for (i = 3; i; i--)
	{
		DdClearRender(FALSE);
		DdPresent();
	}
	do { if (sglpddsRender) { sglpddsRender->Release(); sglpddsRender = NULL; } } while (0);
	do { if (sglpddsBack) { sglpddsBack->Release(); sglpddsBack = NULL; } } while (0);
	do { if (glpddsPrimary) { glpddsPrimary->Release(); glpddsPrimary = NULL; } } while (0);
	FogErrorManagerLog("... DirectDraw window closed.");
	gptBlitTable->pfnSetSurface(NULL, 0, 0, 0);
	sgfWindowOpen = FALSE;
	sgfWindowState = FALSE;
}

static BOOL DdOpenCutsceneWindow(void)
{
	DDSURFACEDESC tDesc;
	int nBits;
	HRESULT hResult;

	gnBufferWidth = 640;
	sgnModeWidth = 640;
	gnBufferHeight = 480;
	sgnModeHeight = 480;
	nBits = 16;
	if (gdwBitMasks[1] & gtVideoConfig.dwFlags)
		nBits = 32;
	else if (gdwBitMasks[2] & gtVideoConfig.dwFlags)
		nBits = 24;
	FogErrorManagerLog("Opening DirectDraw window at %dx%d at %d bit for cutscenes...", 640, 480, nBits);
	hResult = sglpDD->SetDisplayMode(sgnModeWidth, sgnModeHeight, nBits);
	if (hResult)
		DdLogError("SetDisplayMode failed!", hResult);
	else
	{
		tDesc.dwSize = sizeof(tDesc);
		memset(&tDesc.dwFlags, 0, sizeof(tDesc) - sizeof(DWORD));
		tDesc.dwFlags = 1;
		tDesc.ddsCaps.dwCaps = 16896;
		hResult = sglpDD->CreateSurface(&tDesc, &glpddsPrimary, NULL);
		if (hResult)
			DdLogError("Failed to create primary surface for cutscenes", hResult);
		else
		{
			sgfWindowOpen = TRUE;
			sgfWindowState = TRUE;
			FogErrorManagerLog("Opening DirectDraw window for cutscenes succeeded.");
			return TRUE;
		}
	}
	FogErrorManagerLog("Opening DirectDraw window for cutscenes failed!");
	return FALSE;
}

BOOL WINAPI DdOpenCutscene(void)
{
	if (gfSuspended)
		return FALSE;
	DdCloseWindow();
#line 577
	D2ASSERT(!sgfWindowOpen);
	return DdOpenCutsceneWindow();
}

void __fastcall DdPlayCutscene(const char *pszFileName, int nMode, void (__stdcall *pfnTick)(void))
{
	DDBLTFX tFx;
	HRESULT hResult;

	if (gfSuspended)
		return;
	tFx.dwSize = sizeof(tFx);
	memset(&tFx.dwDDFX, 0, sizeof(tFx) - sizeof(DWORD));
	sgfCutscene = TRUE;
	tFx.dwFillColor = 0;
#line 854
	D2ASSERT(glpddsPrimary);
	hResult = glpddsPrimary->Blt(NULL, NULL, NULL, 1024, &tFx);
	if (hResult)
	{
		DdLogError("Cutscene clearing blit failed!", hResult);
		return;
	}
	DDrawPlayBinkFile(pszFileName, nMode, pfnTick);
	sgfCutscene = FALSE;
}

DWORD WINAPI DdIsCutscene(void)
{
	return sgfCutscene;
}

BOOL WINAPI DdCloseWindowAndPalette(void)
{
	DdCloseWindow();
	do { if (sglpPalette) { sglpPalette->Release(); sglpPalette = NULL; } } while (0);
	ghWnd = NULL;
	return TRUE;
}

BOOL WINAPI DdLockRender(void)
{
	DDSURFACEDESC tDesc;
	HRESULT hResult;

	if (gfSuspended)
		return FALSE;
#line 910
	D2ASSERT(pcsPause);
	EnterCriticalSection(pcsPause);
	DdClearRender(TRUE);
	tDesc.dwSize = sizeof(tDesc);
	memset(&tDesc.dwFlags, 0, sizeof(tDesc) - sizeof(DWORD));
	hResult = sglpddsRender->Lock(NULL, &tDesc, 33, NULL);
	if (hResult)
	{
		DdLogError("Lock failed!", hResult);
		LeaveCriticalSection(pcsPause);
		return FALSE;
	}
	gpbSurface = (BYTE *)tDesc.lpSurface;
	gnBufferWidth = tDesc.lPitch;
	gptBlitTable->pfnSetSurface(gpbSurface, gnBufferWidth, gnBufferHeight, 0);
	return TRUE;
}

BOOL WINAPI DdUnlockRender(void)
{
	HRESULT hResult;

	hResult = sglpddsRender->Unlock(NULL);
	if (hResult)
	{
		DdLogError("Unlock failed!", hResult);
		return FALSE;
	}
	gpbSurface = NULL;
	gptBlitTable->pfnSetSurface(NULL, 0, 0, 0);
#line 987
	D2ASSERT(pcsPause);
	LeaveCriticalSection(pcsPause);
	return TRUE;
}

BOOL __fastcall DdPresent(void)
{
	RECT rc;
	HRESULT hResult;

	if (sgnBackBuffers)
	{
		if (sglpddsRender && sglpddsBack && sglpddsRender != sglpddsBack)
		{
			rc.right = gnBufferWidth;
			rc.left = 0;
			rc.top = 0;
			rc.bottom = gnBufferHeight;
			hResult = sglpddsBack->BltFast(0, 0, sglpddsRender, &rc, 16);
			if (hResult)
			{
				DdLogError("BltFast failed!", hResult);
				return FALSE;
			}
		}
		glpddsPrimary->Flip(NULL, 1);
	}
	else if (sglpddsRender)
	{
		rc.left = 0;
		rc.top = 0;
		rc.right = gnBufferWidth;
		rc.bottom = gnBufferHeight;
		hResult = glpddsPrimary->BltFast(0, 0, sglpddsRender, &rc, 16);
		if (hResult)
		{
			DdLogError("BltFast failed!", hResult);
			return FALSE;
		}
	}
	if (sgfPaletteDirty && sglpPalette)
	{
		sgfPaletteDirty = FALSE;
		hResult = sglpPalette->SetEntries(0, 1, 255, &gatPaletteEntries[1]);
		if (hResult)
		{
			DdLogError("SetEntries failed!", hResult);
			return FALSE;
		}
	}
	return TRUE;
}

BOOL __fastcall DdExpandToRgb(BYTE *pbDest)
{
	DDSURFACEDESC tDesc;
	HRESULT hResult;
	BYTE *pbRow;
	BYTE *pbSrc;
	BYTE *pbOut;
	PALETTEENTRY *ptEntry;
	int x;
	int y;

	tDesc.dwSize = sizeof(tDesc);
	memset(&tDesc.dwFlags, 0, sizeof(tDesc) - sizeof(DWORD));
	if (!sglpddsBack)
		return FALSE;
	hResult = sglpddsBack->Lock(NULL, &tDesc, 17, NULL);
	if (hResult)
	{
		DdLogError("Lock failed!", hResult);
		return FALSE;
	}
	pbRow = (BYTE *)tDesc.lpSurface;
	for (y = 0; y < gnBufferHeight; y++)
	{
		pbOut = pbDest;
		pbSrc = pbRow;
		for (x = 0; x < gnBufferWidth; x++)
		{
			ptEntry = &gatPaletteEntries[*pbSrc++];
			*pbOut++ = ptEntry->peRed;
			*pbOut++ = ptEntry->peGreen;
			*pbOut++ = ptEntry->peBlue;
		}
		pbRow += tDesc.lPitch;
		pbDest += gnBufferWidth * 3;
	}
	hResult = sglpddsBack->Unlock(NULL);
	if (hResult)
	{
		DdLogError("Unlock failed!", hResult);
		return FALSE;
	}
	return TRUE;
}

void __fastcall DdGetScreenSize(int *pnWidth, int *pnHeight)
{
	*pnWidth = gnBufferWidth;
	*pnHeight = gnBufferHeight;
}

int __fastcall DdSetPaletteEntries(const PALETTEENTRY *ptEntries)
{
	int nRed;
	int nGreen;
	int nBlue;
	int nCap;
	int i;

	memcpy(sgatPaletteSource, ptEntries, sizeof(sgatPaletteSource));
	for (i = 0; i < 256; i++)
	{
		nRed = sgatPaletteSource[i].peRed * sgnGamma / 100;
		nGreen = sgatPaletteSource[i].peGreen * sgnGamma / 100;
		nBlue = sgatPaletteSource[i].peBlue * sgnGamma / 100;
		gatPaletteEntries[i].peRed = (BYTE)(nRed < 255 ? nRed : 255);
		gatPaletteEntries[i].peGreen = (BYTE)(nGreen < 255 ? nGreen : 255);
		nCap = nBlue < 255 ? nBlue : 255;
		gatPaletteEntries[i].peBlue = (BYTE)nCap;
	}
	sgfPaletteDirty = TRUE;
	return nCap;
}

void __fastcall DdSetEnvInfo(const DDENVINFO *ptEnvInfo)
{
	sgtEnvInfo = *ptEnvInfo;
	gptEnvInfo = &sgtEnvInfo;
}

void WINAPI DdNothingSetUnreadDword(void)
{
}

int __fastcall DdSetGamma(int nGamma)
{
	int nRed;
	int nGreen;
	int nBlue;
	int nCap;
	int i;

	sgnGamma = nGamma;
	for (i = 0; i < 256; i++)
	{
		nRed = sgatPaletteSource[i].peRed * nGamma / 100;
		nGreen = sgatPaletteSource[i].peGreen * nGamma / 100;
		nBlue = sgatPaletteSource[i].peBlue * nGamma / 100;
		gatPaletteEntries[i].peRed = (BYTE)(nRed < 255 ? nRed : 255);
		if (nGreen >= 255)
			nGreen = 255;
		gatPaletteEntries[i].peGreen = (BYTE)nGreen;
		nCap = nBlue < 255 ? nBlue : 255;
		gatPaletteEntries[i].peBlue = (BYTE)nCap;
	}
	sgfPaletteDirty = TRUE;
	return nCap;
}

BOOL WINAPI DdHasGamma(void)
{
	return TRUE;
}

void WINAPI DdNothingSetVertexColor(DWORD dwValue)
{
}

void WINAPI DdNothingSetPerspOrigin(void)
{
}

void WINAPI DdNothingPerspProject(DWORD dwValue1, DWORD dwValue2, DWORD dwValue3)
{
}

void WINAPI DdNothingPerspProjectDepth(DWORD dwValue1, DWORD dwValue2, DWORD dwValue3, DWORD dwValue4)
{
}

void WINAPI DdNothingResetPerspDepth(void)
{
}

BOOL __fastcall DdReinit(HWND hWnd, int nResolution)
{
	if (!gfSuspended)
	{
		DdCloseWindow();
		return DdOpenWindow(nResolution);
	}
	return TRUE;
}

BOOL WINAPI DdGetMemoryBudget(void)
{
	return FALSE;
}

BOOL WINAPI DdAlwaysFalse(void)
{
	return FALSE;
}

BOOL WINAPI DdGetFrameOpenQuery(void)
{
	return FALSE;
}

void __fastcall DdLogError(const char *pszMessage, HRESULT hResult)
{
	const char *pszName;

	if (hResult == 0x887601AE || hResult == 0x887601C2)
		return;
	switch (hResult)
	{
	case 0:
		pszName = "DD_OK";
		break;

	case 0x80004001:
		pszName = "DDERR_UNSUPPORTED";
		break;

	case 0x80004005:
		pszName = "DDERR_GENERIC";
		break;

	case 0x800401F0:
		pszName = "DDERR_NOTINITIALIZED";
		break;

	case 0x8007000E:
		pszName = "DDERR_OUTOFMEMORY";
		break;

	case 0x80070057:
		pszName = "DDERR_INVALIDPARAMS";
		break;

	case 0x88760005:
		pszName = "DDERR_ALREADYINITIALIZED";
		break;

	case 0x8876000A:
		pszName = "DDERR_CANNOTATTACHSURFACE";
		break;

	case 0x88760014:
		pszName = "DDERR_CANNOTDETACHSURFACE";
		break;

	case 0x88760028:
		pszName = "DDERR_CURRENTLYNOTAVAIL";
		break;

	case 0x88760037:
		pszName = "DDERR_EXCEPTION";
		break;

	case 0x8876005A:
		pszName = "DDERR_HEIGHTALIGN";
		break;

	case 0x8876005F:
		pszName = "DDERR_INCOMPATIBLEPRIMARY";
		break;

	case 0x88760064:
		pszName = "DDERR_INVALIDCAPS";
		break;

	case 0x8876006E:
		pszName = "DDERR_INVALIDCLIPLIST";
		break;

	case 0x88760078:
		pszName = "DDERR_INVALIDMODE";
		break;

	case 0x88760082:
		pszName = "DDERR_INVALIDOBJECT";
		break;

	case 0x88760091:
		pszName = "DDERR_INVALIDPIXELFORMAT";
		break;

	case 0x88760096:
		pszName = "DDERR_INVALIDRECT";
		break;

	case 0x887600A0:
		pszName = "DDERR_LOCKEDSURFACES";
		break;

	case 0x887600AA:
		pszName = "DDERR_NO3D";
		break;

	case 0x887600B4:
		pszName = "DDERR_NOALPHAHW";
		break;

	case 0x887600CD:
		pszName = "DDERR_NOCLIPLIST";
		break;

	case 0x887600D2:
		pszName = "DDERR_NOCOLORCONVHW";
		break;

	case 0x887600D4:
		pszName = "DDERR_NOCOOPERATIVELEVELSET";
		break;

	case 0x887600D7:
		pszName = "DDERR_NOCOLORKEY";
		break;

	case 0x887600DC:
		pszName = "DDERR_NOCOLORKEYHW";
		break;

	case 0x887600DE:
		pszName = "DDERR_NODIRECTDRAWSUPPORT";
		break;

	case 0x887600E1:
		pszName = "DDERR_NOEXCLUSIVEMODE";
		break;

	case 0x887600E6:
		pszName = "DDERR_NOFLIPHW";
		break;

	case 0x887600F0:
		pszName = "DDERR_NOGDI";
		break;

	case 0x887600FA:
		pszName = "DDERR_NOMIRRORHW";
		break;

	case 0x887600FF:
		pszName = "DDERR_NOTFOUND";
		break;

	case 0x88760104:
		pszName = "DDERR_NOOVERLAYHW";
		break;

	case 0x88760118:
		pszName = "DDERR_NORASTEROPHW";
		break;

	case 0x88760122:
		pszName = "DDERR_NOROTATIONHW";
		break;

	case 0x88760136:
		pszName = "DDERR_NOSTRETCHHW";
		break;

	case 0x8876013C:
		pszName = "DDERR_NOT4BITCOLOR";
		break;

	case 0x8876013D:
		pszName = "DDERR_NOT4BITCOLORINDEX";
		break;

	case 0x88760140:
		pszName = "DDERR_NOT8BITCOLOR";
		break;

	case 0x8876014A:
		pszName = "DDERR_NOTEXTUREHW";
		break;

	case 0x8876014F:
		pszName = "DDERR_NOVSYNCHW";
		break;

	case 0x88760154:
		pszName = "DDERR_NOZBUFFERHW";
		break;

	case 0x8876015E:
		pszName = "DDERR_NOZOVERLAYHW";
		break;

	case 0x88760168:
		pszName = "DDERR_OUTOFCAPS";
		break;

	case 0x8876017C:
		pszName = "DDERR_OUTOFVIDEOMEMORY";
		break;

	case 0x8876017E:
		pszName = "DDERR_OVERLAYCANTCLIP";
		break;

	case 0x88760180:
		pszName = "DDERR_OVERLAYCOLORKEYONLYONEACTIVE";
		break;

	case 0x88760183:
		pszName = "DDERR_PALETTEBUSY";
		break;

	case 0x88760190:
		pszName = "DDERR_COLORKEYNOTSET";
		break;

	case 0x8876019A:
		pszName = "DDERR_SURFACEALREADYATTACHED";
		break;

	case 0x887601A4:
		pszName = "DDERR_SURFACEALREADYDEPENDENT";
		break;

	case 0x887601AE:
		pszName = "DDERR_SURFACEBUSY";
		break;

	case 0x887601B3:
		pszName = "DDERR_CANTLOCKSURFACE";
		break;

	case 0x887601B8:
		pszName = "DDERR_SURFACEISOBSCURED";
		break;

	case 0x887601C2:
		pszName = "DDERR_SURFACELOST";
		break;

	case 0x887601CC:
		pszName = "DDERR_SURFACENOTATTACHED";
		break;

	case 0x887601D6:
		pszName = "DDERR_TOOBIGHEIGHT";
		break;

	case 0x887601E0:
		pszName = "DDERR_TOOBIGSIZE";
		break;

	case 0x887601EA:
		pszName = "DDERR_TOOBIGWIDTH";
		break;

	case 0x887601FE:
		pszName = "DDERR_UNSUPPORTEDFORMAT";
		break;

	case 0x88760208:
		pszName = "DDERR_UNSUPPORTEDMASK";
		break;

	case 0x88760219:
		pszName = "DDERR_VERTICALBLANKINPROGRESS";
		break;

	case 0x8876021C:
		pszName = "DDERR_WASSTILLDRAWING";
		break;

	case 0x88760230:
		pszName = "DDERR_XALIGN";
		break;

	case 0x88760231:
		pszName = "DDERR_INVALIDDIRECTDRAWGUID";
		break;

	case 0x88760232:
		pszName = "DDERR_DIRECTDRAWALREADYCREATED";
		break;

	case 0x88760233:
		pszName = "DDERR_NODIRECTDRAWHW";
		break;

	case 0x88760234:
		pszName = "DDERR_PRIMARYSURFACEALREADYEXISTS";
		break;

	case 0x88760235:
		pszName = "DDERR_NOEMULATION";
		break;

	case 0x88760236:
		pszName = "DDERR_REGIONTOOSMALL";
		break;

	case 0x88760237:
		pszName = "DDERR_CLIPPERISUSINGHWND";
		break;

	case 0x88760238:
		pszName = "DDERR_NOCLIPPERATTACHED";
		break;

	case 0x88760239:
		pszName = "DDERR_NOHWND";
		break;

	case 0x8876023A:
		pszName = "DDERR_HWNDSUBCLASSED";
		break;

	case 0x8876023B:
		pszName = "DDERR_HWNDALREADYSET";
		break;

	case 0x8876023C:
		pszName = "DDERR_NOPALETTEATTACHED";
		break;

	case 0x8876023D:
		pszName = "DDERR_NOPALETTEHW";
		break;

	case 0x8876023E:
		pszName = "DDERR_BLTFASTCANTCLIP";
		break;

	case 0x8876023F:
		pszName = "DDERR_NOBLTHW";
		break;

	case 0x88760240:
		pszName = "DDERR_NODDROPSHW";
		break;

	case 0x88760241:
		pszName = "DDERR_OVERLAYNOTVISIBLE";
		break;

	case 0x88760242:
		pszName = "DDERR_NOOVERLAYDEST";
		break;

	case 0x88760243:
		pszName = "DDERR_INVALIDPOSITION";
		break;

	case 0x88760244:
		pszName = "DDERR_NOTAOVERLAYSURFACE";
		break;

	case 0x88760245:
		pszName = "DDERR_EXCLUSIVEMODEALREADYSET";
		break;

	case 0x88760246:
		pszName = "DDERR_NOTFLIPPABLE";
		break;

	case 0x88760247:
		pszName = "DDERR_CANTDUPLICATE";
		break;

	case 0x88760248:
		pszName = "DDERR_NOTLOCKED";
		break;

	case 0x88760249:
		pszName = "DDERR_CANTCREATEDC";
		break;

	case 0x8876024A:
		pszName = "DDERR_NODC";
		break;

	case 0x8876024B:
		pszName = "DDERR_WRONGMODE";
		break;

	case 0x8876024C:
		pszName = "DDERR_IMPLICITLYCREATED";
		break;

	case 0x8876024D:
		pszName = "DDERR_NOTPALETTIZED";
		break;

	case 0x8876024E:
		pszName = "DDERR_UNSUPPORTEDMODE";
		break;

	case 0x8876024F:
		pszName = "DDERR_NOMIPMAPHW";
		break;

	case 0x88760250:
		pszName = "DDERR_INVALIDSURFACETYPE";
		break;

	case 0x88760258:
		pszName = "DDERR_NOOPTIMIZEHW";
		break;

	case 0x88760259:
		pszName = "DDERR_NOTLOADED";
		break;

	case 0x8876026C:
		pszName = "DDERR_DCALREADYCREATED";
		break;

	case 0x88760276:
		pszName = "DDERR_NONONLOCALVIDMEM";
		break;

	case 0x88760280:
		pszName = "DDERR_CANTPAGELOCK";
		break;

	case 0x88760294:
		pszName = "DDERR_CANTPAGEUNLOCK";
		break;

	case 0x887602A8:
		pszName = "DDERR_NOTPAGELOCKED";
		break;

	case 0x887602B2:
		pszName = "DDERR_MOREDATA";
		break;

	case 0x887602B7:
		pszName = "DDERR_VIDEONOTACTIVE";
		break;

	case 0x887602BB:
		pszName = "DDERR_DEVICEDOESNTOWNSURFACE";
		break;

	default:
		pszName = "< unknown >";
		break;
	}
	FogErrorManagerLog("***** DirectDraw error: %s - %s (0x%x)", pszMessage, pszName, hResult);
}
