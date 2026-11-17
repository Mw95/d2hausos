#define D2_THIS_FILE "C:\\D2\\Source\\D2Glide\\Src\\glCore.cpp"
#include "glCore.h"
#include "glCaches.h"
#include "StormImports.h"
#include "D2GlideImports.h"
#include "D2Macros.h"

struct GLMEMBUDGET
{
 	DWORD		dwFloorBytes;
 	DWORD		unobserved_04[2];
 	DWORD		dwSubtileBytes;
 	DWORD		unobserved_10[2];
 	DWORD		dwTexture0Bytes;
 	DWORD		dwTexture1Bytes;
 	DWORD		dwTexture2Bytes;
};

static DWORD sgadwPalette[256] = { 0 };
GLENVINFO *gptEnvInfo = 0;
HWND ghWnd = 0;
static GLENVINFO sgtEnvInfo = { 0 };
static DWORD sgdwSlot21 = 0;
DWORD gadwTexMinAddress[3] = { 0 };
GLDRAWCTX *gptDrawCtx = 0;
int gnScreenWidth = 0;
GLCACHE gtSubtileCache = { 0 };
static DWORD sgdwGlideContext = 0;
static GLMEMBUDGET sgtMemoryBudget = { 0 };
static DWORD PLACEHOLDER_UnreferencedData_1001dd94[19] = { 0 };
GLCACHE gtFloorCache = { 0 };
static GLBLITTABLE *sgptBlitTable = 0;
int gnScreenHeight = 0;
static DWORD sgadwTexMaxAddress[3] = { 0 };
int gnTmuCount = 0;
BOOL gfUnifiedMemory = 0;
static DWORD sgdwTexAlign = 0;
static BOOL sgfSli = 0;
static int sgnBoards = 0;
static BOOL sgfWindowOpen = 0;
static BOOL sgfInitialized = 0;
static BOOL sgfInCutscene = 0;
static int sgnColorMode = 0;
static int sgnAlphaMode = 0;
static int sgnFilterMode = 0;
BOOL gfSuspended = 0;
CRITICAL_SECTION *pcsPause = 0;
static BOOL sgfInFrame = 0;

char gszRegistrationBlock2[128] = "REGISTRATION_BLOCK";
static BOOL sgfFirstOpen = TRUE;
static int sgnTexCombineMode = -1;

#define GL_MIN_TEX_MEMORY	3670016.0f
#define GL_BYTES_TO_MEGS	(1.0f / 1048576.0f)
#define GL_GAMMA_SCALE	0.01f

static BOOL __fastcall GlideOpenWindow(HWND hWnd, int nResolution);
static BOOL __fastcall GlidePartitionTexMem(void);
static void __cdecl GlideShutdownAtExit(void);
BOOL __fastcall GlideCloseWindow(void);

BOOL __cdecl GlideDetectBoards(void)
{
	grGet(15, 4, &sgnBoards);
	FogErrorManagerLog("Detecting boards which support Glide...");
	FogErrorManagerLog("\t%d boards found", sgnBoards);
	return sgnBoards > 0;
}

BOOL __fastcall GlideSetup(GLDRAWCTX *ptDrawCtx, GLBLITTABLE *ptBlitTable)
{
#line 416
	D2ASSERT(!sgfInitialized);
	gptDrawCtx = ptDrawCtx;
	sgptBlitTable = ptBlitTable;
	FogErrorManagerLog("Initializing for Glide...");
	GlideBlocksOpen();
	GlidePerspectiveOpen();
	GlideVertexOpen();
	grGlideInit();
	sgfInitialized = TRUE;
	grSstSelect(0);
	FogErrorManagerLog("\tHardware: %s", grGetString(161));
	FogErrorManagerLog("\tVendor: %s", grGetString(163));
	FogErrorManagerLog("\tRenderer: %s", grGetString(162));
	FogErrorManagerLog("\tVersion: %s", grGetString(164));
	atexit(GlideShutdownAtExit);
	GlideBuildLightTable();
	GlidePerspectiveInit();
	GlideWallStartup();
	GlideFloorGridInit();
	GlideSmackStartup();
	FogErrorManagerLog("\n----------------------------------------------\n");
#line 456
	pcsPause = (CRITICAL_SECTION *)D2ALLOC2(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection(pcsPause);
	return TRUE;
}

static void __cdecl GlideShutdownAtExit(void)
{
	if (sgfInitialized)
	{
		GlideSmackShutdown();
		GlideCloseWindow();
		FogErrorManagerLog("Shutting down Glide...");
		grGlideShutdown();
		FogErrorManagerLog("...Glide shut down.");
		GlideBlocksClose();
		GlidePerspectiveClose();
		GlideVertexClose();
		sgfInitialized = FALSE;
	}
	if (pcsPause)
	{
		DeleteCriticalSection(pcsPause);
#line 499
		D2FREE2(pcsPause);
		pcsPause = NULL;
	}
}

BOOL __fastcall GlideTeardown(void)
{
	GlideShutdownAtExit();
	return TRUE;
}

BOOL __fastcall GlideInit(HWND hWnd, int nResolution)
{
	if (sgfWindowOpen)
		return TRUE;
	if (!GlideOpenWindow(hWnd, nResolution))
		return FALSE;
	grBufferClear(0, 0, 0);
	if (gptDrawCtx->dwVSync)
	{
		grBufferSwap(1);
		return TRUE;
	}
	grBufferSwap(0);
	return TRUE;
}

static void __fastcall GlideReadBoardInfo(void)
{
	int nSli;
	int nMaxTexSize;
	int nMaxAspect;
	int nUnified;

	nSli = 0;
	sgfFirstOpen = FALSE;
	grGet(10, 4, &nMaxTexSize);
	FogErrorManagerLog("\tMax Texture Size: %d", nMaxTexSize);
	grGet(11, 4, &nMaxAspect);
	FogErrorManagerLog("\tMax Aspect Ratio: %d", nMaxAspect);
	grGet(17, 4, &nSli);
	if (nSli > 1)
	{
		sgfSli = TRUE;
		FogErrorManagerLog("\tSLI configured: Yes");
	}
	else
	{
		sgfSli = FALSE;
		FogErrorManagerLog("\tSLI configured: No");
	}
	grGet(19, 4, &gnTmuCount);
	FogErrorManagerLog("\tTMU Count: %d", gnTmuCount);
	grGet(36, 4, &sgdwTexAlign);
	FogErrorManagerLog("\tTexture Alignment: %d bytes", sgdwTexAlign);
	grGet(14, 4, &nUnified);
	if (nUnified)
	{
		FogErrorManagerLog("\tUnified Memory Architecture: Yes");
		gfUnifiedMemory = TRUE;
	}
	else
	{
		FogErrorManagerLog("\tUnified Memory Architecture: No");
		gfUnifiedMemory = FALSE;
	}
}

static BOOL __fastcall GlideOpenWindow(HWND hWnd, int nResolution)
{
	int nMode;
	BOOL fResult = FALSE;

#line 129
	D2ASSERT(!sgfWindowOpen);
	GlideCreateDebugFont();
	if (!nResolution)
	{
		gnScreenWidth = 640;
		nMode = 7;
		gnScreenHeight = 480;
	}
	else
	{
		gnScreenWidth = 800;
		nMode = 8;
		gnScreenHeight = 600;
	}
	FogErrorManagerLog("Opening Glide window at %dx%d...", gnScreenWidth, gnScreenHeight);
	ghWnd = hWnd;
	sgdwGlideContext = grSstWinOpen((DWORD)hWnd, nMode, 0, 2, 0, 2, 0);
	if (sgdwGlideContext)
	{
		sgfWindowOpen = TRUE;
		if (sgfFirstOpen)
		{
			GlideReadBoardInfo();
			if (!gnTmuCount)
				return FALSE;
		}
		grCoordinateSpace(0);
		grVertexLayout(1, 0, 1);
		grVertexLayout(80, 12, 1);
		grVertexLayout(64, 16, 1);
		if (gnTmuCount > 1)
		{
			grVertexLayout(81, 12, 1);
			grVertexLayout(65, 16, 1);
		}
		grVertexLayout(48, 8, 1);
		grDepthMask(0);
		grColorMask(1, 0);
		GlideSetTexCombine(0);
		grTexFilterMode(0, 0, 0);
		grTexCombine(0, 1, 0, 1, 0, 0, 0);
		if (gnTmuCount > 1)
		{
			grTexFilterMode(1, 0, 0);
			grTexCombine(1, 1, 0, 1, 0, 0, 0);
		}
		grChromakeyValue(255);
		if (GlidePartitionTexMem())
		{
			fResult = TRUE;
			FogErrorManagerLog("Opening Glide window succeeded, context= %d", sgdwGlideContext);
		}
	}
	else
		FogErrorManagerLog("Opening Glide window failed!");
	return fResult;
}

static BOOL __fastcall GlidePartitionTexMem(void)
{
	DWORD dwMask;
	DWORD dwTex0Bytes;
	DWORD dwTex1Bytes;
	DWORD dwTex2Bytes;
	DWORD dwSubtileBytes;
	DWORD dwFloorBytes;
	DWORD dwTotal;
	DWORD dwSize;
	DWORD dwAddress;
	DWORD dwFree;
	DWORD dwBase;
	DWORD dwRest;
	int nFloorBudget;
	int nSubtileBudget;
	int nQuarter;
	DWORD dwEnd;
	int nTmu;
	int i;

	dwTex0Bytes = 65536;
	dwMask = sgdwTexAlign - 1;
	dwTotal = 0;
	dwTex1Bytes = 16384;
	dwTex2Bytes = 4096;
	if (dwMask & 65536)
		dwTex0Bytes = (dwMask | 65536) + 1;
	if (dwMask & 16384)
		dwTex1Bytes = (dwMask | 16384) + 1;
	if (dwMask & 4096)
		dwTex2Bytes = (dwMask | 4096) + 1;
	for (i = 0; i < gnTmuCount; i++)
	{
		gadwTexMinAddress[i] = grTexMinAddress(i);
		dwAddress = grTexMaxAddress(i);
		dwSize = dwAddress - gadwTexMinAddress[i];
		sgadwTexMaxAddress[i] = dwAddress;
		dwTotal += dwSize;
		FogErrorManagerLog("\tTMU(%d) MinAddress= %d, MaxAddress= %d, Size= %3.2f MB", i,
			gadwTexMinAddress[i], dwAddress, dwSize * GL_BYTES_TO_MEGS);
	}
	if (dwTotal < GL_MIN_TEX_MEMORY)
	{
		FogErrorManagerLog("\t\tDoesn't have enough video memory (3.5 MB) to adequately run Diablo II!\n");
		return FALSE;
	}
	nTmu = 0;
	dwAddress = gadwTexMinAddress[0];
	if (gnScreenWidth == 640 && gnScreenHeight == 480)
	{
		dwMask = sgdwTexAlign - 1;
		dwSubtileBytes = 1024;
		if (dwMask & 1024)
			dwSubtileBytes = (dwMask | 1024) + 1;
		dwFloorBytes = 32768;
		if (dwMask & 32768)
			dwFloorBytes = (dwMask | 32768) + 1;
		dwAddress += GlideDownloadDebugFont(dwAddress, sgdwTexAlign);
		dwFree = sgadwTexMaxAddress[0] - dwAddress;
		if (gnTmuCount > 1)
		{
			nFloorBudget = (dwFree * 3) >> 2;
			if (nFloorBudget < 1887436)
				nFloorBudget = 1887436;
			sgtMemoryBudget.dwFloorBytes = nFloorBudget;
			dwAddress = GlideFloorCacheInit(&gtFloorCache, dwAddress, nFloorBudget + dwAddress, dwFloorBytes);
			sgtMemoryBudget.dwSubtileBytes = sgadwTexMaxAddress[0] - dwAddress;
			dwAddress = GlideSubtileCacheInit(&gtSubtileCache, dwAddress, sgadwTexMaxAddress[0], dwSubtileBytes);
		}
		else
		{
			nFloorBudget = dwFree >> 1;
			if (nFloorBudget >= 3145728)
				nFloorBudget = 3145728;
			nQuarter = dwFree >> 2;
			nSubtileBudget = nQuarter < 1048576 ? nQuarter : 1048576;
			sgtMemoryBudget.dwFloorBytes = nFloorBudget;
			dwAddress = GlideFloorCacheInit(&gtFloorCache, dwAddress, nFloorBudget + dwAddress, dwFloorBytes);
			sgtMemoryBudget.dwSubtileBytes = nSubtileBudget;
			dwAddress = GlideSubtileCacheInit(&gtSubtileCache, dwAddress, nSubtileBudget + dwAddress, dwSubtileBytes);
		}
		if (gnTmuCount > 1)
		{
			dwAddress = gadwTexMinAddress[1];
			nTmu = 1;
		}
		dwBase = dwAddress;
		dwRest = sgadwTexMaxAddress[nTmu] - dwAddress;
		dwAddress = GlideTextureCacheInit(&gatTextureCache[0], dwAddress, dwRest * 25 / 100 + dwAddress, dwTex0Bytes);
		sgtMemoryBudget.dwTexture0Bytes = gatTextureCache[0].nMaxNumItems * dwTex0Bytes;
		dwAddress = GlideTextureCacheInit(&gatTextureCache[1], dwAddress, dwRest * 65 / 100 + dwBase, dwTex1Bytes);
		sgtMemoryBudget.dwTexture1Bytes = gatTextureCache[1].nMaxNumItems * dwTex1Bytes;
		GlideTextureCacheInit(&gatTextureCache[2], dwAddress, sgadwTexMaxAddress[nTmu], dwTex2Bytes);
		sgtMemoryBudget.dwTexture2Bytes = gatTextureCache[2].nMaxNumItems * dwTex2Bytes;
	}
	else if (gnScreenWidth == 800 && gnScreenHeight == 600)
	{
		dwAddress += GlideSmackCreateBuffer(dwAddress, sgdwTexAlign);
		dwFree = sgadwTexMaxAddress[0] - dwAddress;
		if (gnTmuCount > 1)
		{
			dwEnd = GlideTextureCacheInit(&gatTextureCache[1], dwAddress, dwFree * 50 / 100 + dwAddress,
				dwTex1Bytes);
			sgtMemoryBudget.dwTexture1Bytes = gatTextureCache[1].nMaxNumItems * dwTex1Bytes;
			GlideTextureCacheInit(&gatTextureCache[2], dwEnd, sgadwTexMaxAddress[0], dwTex2Bytes);
			sgtMemoryBudget.dwTexture2Bytes = gatTextureCache[2].nMaxNumItems * dwTex2Bytes;
			GlideTextureCacheInit(&gatTextureCache[0], gadwTexMinAddress[1], sgadwTexMaxAddress[1],
				dwTex0Bytes);
			sgtMemoryBudget.dwTexture0Bytes = gatTextureCache[0].nMaxNumItems * dwTex0Bytes;
		}
		else
		{
			dwEnd = GlideTextureCacheInit(&gatTextureCache[0], dwAddress, dwFree * 50 / 100 + dwAddress,
				dwTex0Bytes);
			sgtMemoryBudget.dwTexture0Bytes = gatTextureCache[0].nMaxNumItems * dwTex0Bytes;
			dwEnd = GlideTextureCacheInit(&gatTextureCache[1], dwEnd, dwFree * 75 / 100 + dwAddress,
				dwTex1Bytes);
			sgtMemoryBudget.dwTexture1Bytes = gatTextureCache[1].nMaxNumItems * dwTex1Bytes;
			GlideTextureCacheInit(&gatTextureCache[2], dwEnd, sgadwTexMaxAddress[0], dwTex2Bytes);
			sgtMemoryBudget.dwTexture2Bytes = gatTextureCache[2].nMaxNumItems * dwTex2Bytes;
		}
	}
	GlideSpriteOpen();
	D2CMPTileLibrarySetHardwareFree(GlideTileFreed);
	return TRUE;
}

BOOL __fastcall GlideCloseWindow(void)
{
	int i;

	if (sgfWindowOpen)
	{
		sgfWindowOpen = FALSE;
		FogErrorManagerLog("Closing Glide window....");
		grFinish();
		if (gnScreenWidth == 640)
		{
			if (gnScreenHeight == 480)
			{
				GlideFloorCacheFlush(&gtFloorCache, TRUE);
				GlideSubtileCacheFlush(&gtSubtileCache, TRUE);
			}
		}
		else if (gnScreenWidth == 800 && gnScreenHeight == 600)
			GlideSmackFreeBuffer();
		GlideTextureCacheFlush(&gatTextureCache[0], TRUE);
		GlideTextureCacheFlush(&gatTextureCache[1], TRUE);
		GlideTextureCacheFlush(&gatTextureCache[2], TRUE);
		GlideSpriteClose();
		i = 0;
		while (1)
		{
			if (i >= 100)
				break;
			if (grSstWinClose(sgdwGlideContext) == 1)
				break;
			i++;
		}
		if (i >= 100)
			FogErrorManagerLog("grSstWinClose failed 100 times, context= %d", sgdwGlideContext);
		GlideFreeDebugFont();
		ghWnd = NULL;
		FogErrorManagerLog("... Glide window closed.");
		return TRUE; }
	return TRUE;
}

void __cdecl GlideFlushCaches(void)
{
	GlideFloorCacheFlush(&gtFloorCache, FALSE);
	GlideSubtileCacheFlush(&gtSubtileCache, FALSE);
	GlideTextureCacheFlush(&gatTextureCache[0], FALSE);
	GlideTextureCacheFlush(&gatTextureCache[1], FALSE);
	GlideTextureCacheFlush(&gatTextureCache[2], FALSE);
}

void __fastcall GlideSetSuspended(HWND hWnd, int nResolution, BOOL fSuspend)
{
#line 625
	D2ASSERT(pcsPause);
	EnterCriticalSection(pcsPause);
	if (fSuspend)
	{
		GlideCloseWindow();
		gfSuspended = TRUE;
	}
	else
	{
		if (!sgfWindowOpen && GlideOpenWindow(hWnd, nResolution))
		{
			grBufferClear(0, 0, 0);
			if (gptDrawCtx->dwVSync) grBufferSwap(1); else grBufferSwap(0);
		}
		gfSuspended = FALSE;
	}
	LeaveCriticalSection(pcsPause);
}

BOOL __cdecl GlideIsActive(void)
{
	return !gfSuspended;
}

void __fastcall GlidePlayCutscene(const char *pszFileName, int nMode, void (__stdcall *pfnTick)(void))
{
	if (gfSuspended)
		return;
	grBufferClear(0, 0, 0);
	if (gptDrawCtx->dwVSync) grBufferSwap(1); else grBufferSwap(0);
	sgfInCutscene = TRUE;
	GlidePlayBinkFile(pszFileName, nMode, pfnTick);
	sgfInCutscene = FALSE;
}

BOOL __cdecl GlideIsCutscene(void)
{
	return sgfInCutscene;
}

BOOL __fastcall GlideBeginFrame(void)
{
	if (gfSuspended)
		return FALSE;
	sgfInFrame = TRUE;
#line 715
	D2ASSERT(pcsPause);
	EnterCriticalSection(pcsPause);
	grBufferClear(0, 0, 0);
	sgnColorMode = 0;
	sgnAlphaMode = 0;
	sgnFilterMode = 0;
	grChromakeyMode(1);
	GlideSetVertexColor(255, 255, 255);
	return TRUE;
}

void __fastcall GlideSetUnreadDword(DWORD dwValue)
{
	sgdwSlot21 = dwValue;
}

void __fastcall GlideSetGamma(int nGamma)
{
	float fGamma;

	fGamma = (float)nGamma * GL_GAMMA_SCALE;
	GL_FLOAT_STORE(fGamma);
	guGammaCorrectionRGB(fGamma, fGamma, fGamma);
}

BOOL __cdecl GlideHasGamma(void)
{
	return TRUE;
}

BOOL __cdecl GlideEndFrame(void)
{
#line 770
	D2ASSERT(pcsPause);
	LeaveCriticalSection(pcsPause);
	sgfInFrame = FALSE;
	return TRUE;
}

BOOL __cdecl GlidePresent(void)
{
	if (gptDrawCtx->dwVSync)
	{
		grBufferSwap(1);
		return TRUE;
	}
	grBufferSwap(0);
	return TRUE;
}

BOOL __fastcall GlideExpandToRgb(BYTE *pbDest)
{
	GLLFBINFO tInfo;
	const WORD *pwRow;
	const WORD *pwSrc;
	BYTE *pbOut;
	WORD wPixel;
	int x;
	int y;

	if (!grLfbLock(0, 1, 255, 0, 0, &tInfo))
		return FALSE;
	pwRow = (const WORD *)tInfo.pbBuffer;
	for (y = 0; y < gnScreenHeight; y++)
	{
		pbOut = pbDest;
		pwSrc = pwRow;
		for (x = 0; x < gnScreenWidth; x++)
		{
			wPixel = *pwSrc++;
			*pbOut++ = (BYTE)((WORD)(wPixel >> 11) << 3);
			*pbOut++ = (BYTE)((WORD)(wPixel >> 5) << 2);
			*pbOut++ = (BYTE)(wPixel << 3);
		}
		pwRow = (const WORD *)((const BYTE *)pwRow + tInfo.nPitch);
		pbDest += gnScreenWidth * 3;
	}
	grLfbUnlock(0, 1);
	return TRUE;
}

void __fastcall GlideGetScreenSize(int *pnWidth, int *pnHeight)
{
	*pnWidth = gnScreenWidth;
	*pnHeight = gnScreenHeight;
}

DWORD __fastcall GlideSetPaletteEntries(const PALETTEENTRY *ptEntries)
{
	int i;
	DWORD dwResult;

	dwResult = sgfWindowOpen;
	if (dwResult)
	{
		for (i = 0; i < 256; i++)
		{
			sgadwPalette[i] = ((((ptEntries[i].peRed + 65280) << 8) + ptEntries[i].peGreen) << 8)
				+ ptEntries[i].peBlue; }
		dwResult = grTexDownloadTable(2, sgadwPalette); }
	return dwResult;
}

DWORD __fastcall GlidePaletteColor(int nIndex)
{
	return sgadwPalette[nIndex] << 8;
}

void __fastcall GlideSetEnvInfo(const GLENVINFO *ptEnvInfo)
{
	sgtEnvInfo = *ptEnvInfo;
	gptEnvInfo = &sgtEnvInfo;
}

BOOL __fastcall GlideReinit(HWND hWnd, int nResolution)
{
	GlideCloseWindow();
	grGlideShutdown();
	grGlideInit();
	grSstSelect(0);
	if (!GlideOpenWindow(hWnd, nResolution))
		return FALSE;
	grBufferClear(0, 0, 0);
	if (gptDrawCtx->dwVSync)
	{
		grBufferSwap(1);
		return TRUE;
	}
	grBufferSwap(0);
	return TRUE;
}

BOOL __cdecl GlideAlwaysFalse(void)
{
	return FALSE;
}

BOOL __fastcall GlideGetFrameOpenQuery(int nQuery, DWORD *pdwValue)
{
	if (nQuery != 10)
		return FALSE;
	*pdwValue = sgfInFrame;
	return TRUE;
}

void __fastcall GlideSetTexCombine(DWORD nMode)
{
	if ((int)nMode == sgnTexCombineMode)
		return;
	if (nMode == 1)
	{
		grTexCombine(0, 3, 8, 3, 8, 0, 0);
		sgnTexCombineMode = nMode;
		return;
	}
	if (nMode == 0)
		grTexCombine(0, 1, 0, 1, 0, 0, 0);
	sgnTexCombineMode = nMode;
}

void __fastcall GlideSetColorCombine(int nMode)
{
	if (sgnColorMode && nMode == sgnColorMode)
		return;
	switch (nMode)
	{
	case 1:
		grColorCombine(3, 1, 0, 1, 0);
		break;

	case 2:
		grColorCombine(1, 0, 1, 2, 0);
		break;
	}
	sgnColorMode = nMode;
}

void __fastcall GlideSetAlphaMode(int nMode)
{
	if (sgnAlphaMode && nMode == sgnAlphaMode)
		return;
	switch (nMode)
	{
	case 1:
		grAlphaCombine(0, 0, 1, 2, 0);
		grAlphaBlendFunction(4, 0, 0, 0);
		break;

	case 2:
		grAlphaCombine(1, 0, 1, 2, 0);
		grAlphaBlendFunction(4, 4, 0, 0);
		break;

	case 3:
		grAlphaCombine(1, 0, 1, 2, 0);
		grAlphaBlendFunction(0, 2, 0, 0);
		break;

	case 4:
		grAlphaCombine(1, 0, 1, 2, 0);
		grAlphaBlendFunction(1, 5, 0, 0);
		break;
	}
	sgnAlphaMode = nMode;
}

void __fastcall GlideSetFilterMode(int nMode)
{
	if (sgnFilterMode && nMode == sgnFilterMode)
		return;
	switch (nMode)
	{
	case 1:
		grTexFilterMode(0, 0, 0);
		if (gnTmuCount > 1)
			grTexFilterMode(1, 0, 0);
		break;

	case 2:
		grTexFilterMode(0, 1, 1);
		if (gnTmuCount > 1)
			grTexFilterMode(1, 1, 1);
		break;
	}
	sgnFilterMode = nMode;
}

DWORD *__cdecl GlideGetMemoryBudget(void)
{
	return (DWORD *)&sgtMemoryBudget;
}
