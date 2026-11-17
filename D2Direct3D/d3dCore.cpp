#define D2_THIS_FILE "C:\\D2\\Source\\D2Direct3D\\Src\\d3dCore.cpp"
#include "d3dCore.h"
#include "d3dCaches.h"
#include "StormImports.h"
#include "D2Direct3DImports.h"
#include "D2Macros.h"

struct D3DMEMBUDGET
{
 	DWORD		dwFloorBase;
 	DWORD		unobserved_04[2];
 	DWORD		dwSubtileBase;
 	DWORD		unobserved_10[2];
 	DWORD		dwTexture256Bytes;
 	DWORD		dwTexture128Bytes;
 	DWORD		dwTexture32Bytes;
};

D3DENVINFO *gptEnvInfo = 0;
static D3DENVINFO sgtEnvInfo = { 0 };
D3DVIDEOCONFIG gtVideoConfig = { 0 };
static DWORD PLACEHOLDER_UnreferencedData_10025c4c[2] = { 0 };
static DWORD sgdwTextureTotal = 0;
static DWORD sgdwTextureFree = 0;
static DWORD sgdwLocalTotal = 0;
static DWORD sgdwLocalFree = 0;
static DWORD sgdwAgpTotal = 0;
static int sgnAgpFree = 0;
DWORD gdwCoreSlot21 = 0;
static D3DGAMMARAMP sgtGammaOriginal = { 0 };
D3DDRAWCTX *gptDrawCtx = 0;
DWORD gadwPalette[256] = { 0 };
int gnScreenWidth = 0;
D3DCACHE gtSubtileCache = { 0 };
int gnFloorCells = 0;
WORD gawPalette[256] = { 0 };
int gnFloorRows = 0;
D3DMEMBUDGET gtMemoryBudget = { 0 };
static DWORD PLACEHOLDER_UnreferencedData_100268c4[19] = { 0 };
D3DCACHE gtFloorCache = { 0 };
D3DBLITTABLE *gptBlitTable = 0;
int gnScreenHeight = 0;
static DWORD PLACEHOLDER_UnreferencedData_10026934[256] = { 0 };
static BOOL sgfWindowOpen = 0;
static BOOL sgfCutsceneWindow = 0;
static BOOL sgfInitialized = 0;
BOOL gfUseAgp = 0;
static BOOL sgfInCutscene = 0;
static int sgnBlendMode = 0;
static int sgnAlphaMode = 0;
static int sgnFilterMode = 0;
D3DDDOBJECT *glpDD = 0;
D3DOBJECT *glpD3D = 0;
D3DSURFACE *glpddsPrimary = 0;
D3DSURFACE *glpddsBack = 0;
static D3DSURFACE *sglpddsUnused = 0;
static D3DGAMMA *sglpGamma = 0;
D3DDEVICE *glpDevice = 0;
D3DVIEWPORT *glpViewport = 0;
DWORD gfSuspended = 0;
CRITICAL_SECTION *pcsPause = 0;
HWND ghWnd = 0;
int gnTexelBytes = 0;

char gszRegistrationBlock2[128] = "REGISTRATION_BLOCK";
static int PLACEHOLDER_UnreferencedData_1001ffc8[4] = { 640, 800, 480, 600 };

extern "C" const GUID IID_IDirectDraw4;
extern "C" const GUID IID_IDirect3D3;
extern "C" const GUID IID_IDirectDrawGammaControl;
extern "C" const GUID IID_IDirect3DHALDevice;
extern "C" const GUID IID_IDirect3DTexture2;
static D3DPIXELFORMAT sgtTexturePixelFormat = {
	{ 32, 65, 0, 16, 31744, 992, 31, 32768 } };

static void __cdecl D3DShutdown(void);
static BOOL __fastcall D3DOpenWindow(HWND hWnd, int nResolution);
static void WINAPI D3DCloseWindow(void);
BOOL WINAPI D3DPresent(void);
void WINAPI D3DClearRender(void);

BOOL WINAPI D3DDetectBoards(void)
{
	ERRORMANAGERSYSINFO *tSystemInfo;
	DWORD dwSize;

	tSystemInfo = (ERRORMANAGERSYSINFO *)FogErrorManagerGetSysInfo();
#line 158
	D2ASSERT(tSystemInfo);
	if (tSystemInfo->dwOsType == 3 && !(tSystemInfo->flVersion >= 5.0f))
	{
		FogErrorManagerLog("***** Windows NT versions less than 5.0 don't support Direct3D acceleration!");
		return FALSE;
	}
	FogErrorManagerLog("Detecting boards which support Direct3D...");
	{ char szKey[] = "Diablo II\\VideoConfig";
	if (!StormSRegLoadString(szKey, "DeviceName", 0, gtVideoConfig.szDeviceName, sizeof(gtVideoConfig.szDeviceName)))
		return FALSE;
	if (!StormSRegLoadValue(szKey, "dwFlags", 0, &gtVideoConfig.dwFlags))
		return FALSE;
	if (!(gdwBitMasks[0] & gtVideoConfig.dwFlags)) {
		dwSize = sizeof(gtVideoConfig.abGuid);
		if (!StormSRegLoadData(szKey, "GUID", 0, gtVideoConfig.abGuid, sizeof(gtVideoConfig.abGuid), &dwSize)
			|| dwSize != sizeof(gtVideoConfig.abGuid))
			return FALSE;
		return TRUE; } }
	return TRUE;
}

BOOL __fastcall D3DSetup(D3DDRAWCTX *ptDrawCtx, D3DBLITTABLE *ptBlitTable)
{
	D3DDDOBJECT *lpDD;
	BYTE abIdentifier[1064];
	WORD wProduct;
	WORD wVersion;
	WORD wSubVersion;
	WORD wBuild;
	HRESULT hResult;

	lpDD = NULL;
#line 713
	D2ASSERT(!sgfInitialized);
	gptDrawCtx = ptDrawCtx;
	gptBlitTable = ptBlitTable;
	FogErrorManagerLog("Initializing for Direct3D on %s", gtVideoConfig.szDeviceName);
	if (gdwBitMasks[0] & gtVideoConfig.dwFlags)
		hResult = DirectDrawCreate(NULL, &lpDD, NULL);
	else
		hResult = DirectDrawCreate(gtVideoConfig.abGuid, &lpDD, NULL);
	if (hResult)
	{
		D3DLogDDError("DirectDrawCreate failed!", hResult);
		return FALSE;
	}
	hResult = lpDD->lpVtbl->field_00(lpDD, &IID_IDirectDraw4, (void **)&glpDD);
	if (hResult)
	{
		D3DLogDDError("Failed to get DirectDraw4 interface!", hResult);
		return FALSE;
	}
	lpDD->lpVtbl->field_08(lpDD);
	lpDD = NULL;
	hResult = glpDD->lpVtbl->field_00(glpDD, &IID_IDirect3D3, (void **)&glpD3D);
	if (hResult)
	{
		D3DLogDDError("Failed to get D3D interface!", hResult);
		return FALSE;
	}
	hResult = glpDD->lpVtbl->field_6c(glpDD, abIdentifier, 0);
	if (hResult)
		D3DLogDDError("Failed to get device identifier interface!", hResult);
	else
	{
		wProduct = HIWORD(*(DWORD *)&abIdentifier[1028]);
		wVersion = LOWORD(*(DWORD *)&abIdentifier[1028]);
		wSubVersion = HIWORD(*(DWORD *)&abIdentifier[1024]);
		wBuild = LOWORD(*(DWORD *)&abIdentifier[1024]);
		FogErrorManagerLog("\tDriver: %s", abIdentifier);
		FogErrorManagerLog("\tVersion: %d.%d.%d.%d", wProduct, wVersion, wSubVersion, wBuild);
	}
	if (gdwBitMasks[1] & gtVideoConfig.dwFlags)
		FogErrorManagerLog("\t%s: Yes", "Supports AGP texturing");
	else
		FogErrorManagerLog("\t%s: No", "Supports AGP texturing");
	if (gdwBitMasks[3] & gtVideoConfig.dwFlags)
		FogErrorManagerLog("\t%s: Yes", "Supports mipmaps");
	else
		FogErrorManagerLog("\t%s: No", "Supports mipmaps");
	if (gdwBitMasks[4] & gtVideoConfig.dwFlags)
		FogErrorManagerLog("\t%s: Yes", "Supports gamma control");
	else
		FogErrorManagerLog("\t%s: No", "Supports gamma control");
	if (gdwBitMasks[5] & gtVideoConfig.dwFlags)
		FogErrorManagerLog("\t%s: Yes", "Supports gamma calibration");
	else
		FogErrorManagerLog("\t%s: No", "Supports gamma calibration");
	if (gdwBitMasks[7] & gtVideoConfig.dwFlags)
		FogErrorManagerLog("\t%s: Yes", "Supports 32 Bit Cutscenes");
	else
		FogErrorManagerLog("\t%s: No", "Supports 32 Bit Cutscenes");
	if (gdwBitMasks[8] & gtVideoConfig.dwFlags)
		FogErrorManagerLog("\t%s: Yes", "Supports 24 Bit Cutscenes");
	else
		FogErrorManagerLog("\t%s: No", "Supports 24 Bit Cutscenes");
	atexit(D3DShutdown);
	sgfInitialized = TRUE;
	D3DBuildLightTable();
	D3DPerspectiveInit();
	D3DWallStartup();
	D3DSmackStartup();
	FogErrorManagerLog("\n----------------------------------------------\n");
#line 797
	pcsPause = (CRITICAL_SECTION *)D2ALLOC2(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection(pcsPause);
	return TRUE;
}

static void __cdecl D3DShutdown(void)
{
	if (!sgfInitialized)
		return;
	D3DSmackShutdown();
	D3DCloseWindow();
	ghWnd = NULL;
	FogErrorManagerLog("Shutting down Direct3D...");
	if (glpD3D)
	{
		glpD3D->lpVtbl->field_08(glpD3D);
		glpD3D = NULL;
	}
	if (glpDD)
	{
		glpDD->lpVtbl->field_08(glpDD);
		glpDD = NULL;
	}
	FogErrorManagerLog("...Direct3D shut down.");
	if (pcsPause)
	{
		DeleteCriticalSection(pcsPause);
#line 844
		D2FREE2(pcsPause);
		pcsPause = NULL;
	}
	sgfInitialized = FALSE;
}

BOOL __fastcall D3DTeardown(void)
{
	D3DShutdown();
	return TRUE;
}

BOOL __fastcall D3DInit(HWND hWnd, int nResolution)
{
	if (!sgfWindowOpen)
	{
		if (!D3DOpenWindow(hWnd, nResolution))
			return FALSE;
		D3DClearRender();
		D3DPresent();
	}
	return TRUE;
}

static inline int D3DMin(int nA, int nB)
{
	return nA < nB ? nA : nB;
}

static BOOL __fastcall D3DOpenWindow(HWND hWnd, int nResolution)
{
	D3DSURFDESC tDesc;
	D3DCAPSBLOCK tCaps;
	D3DVIEWDATA tView;
	HRESULT hResult;
	DWORD dwMemory;
	DWORD dwFloorBytes;
	DWORD dwBase;
	DWORD dwRest;
	DWORD dwFloor;
	DWORD dwSubtile;
	DWORD dwSubtileBytes;
	DWORD dwFirst;
	DWORD dwLimit;

#line 369
	D2ASSERT(!sgfWindowOpen);
	ghWnd = hWnd;
	if (!nResolution)
	{
		gnScreenWidth = 640;
		gnScreenHeight = 480;
		FogErrorManagerLog("Opening Direct3D window at 640x480...");
	}
	else
	{
		gnScreenWidth = 800;
		gnScreenHeight = 600;
		FogErrorManagerLog("Opening Direct3D window at 800x600...");
	}
	hResult = glpDD->lpVtbl->pfnSetCooperativeLevel(glpDD, ghWnd, 1041);
	if (hResult)
	{
		D3DLogDDError("SetCooperativeLevel failed!", hResult);
		D3DCloseWindow();
		ghWnd = NULL;
		return FALSE;
	}
	hResult = glpDD->lpVtbl->pfnSetDisplayMode(glpDD, gnScreenWidth, gnScreenHeight, 16, 0, 0);
	if (hResult)
	{
		D3DLogDDError("SetDisplayMode failed!", hResult);
		return FALSE;
	}
	MoveWindow(hWnd, 0, 0, gnScreenWidth, gnScreenHeight, TRUE);
	tDesc.dwSize = sizeof(tDesc);
	memset(&tDesc.dwFlags, 0, sizeof(tDesc) - sizeof(DWORD));
	sgfWindowOpen = TRUE;
	sgfCutsceneWindow = FALSE;
	tDesc.dwFlags = 33;
	tDesc.field_68 = 25112;
	tDesc.field_14 = 1;
	hResult = glpDD->lpVtbl->pfnCreateSurface(glpDD, &tDesc, &glpddsPrimary, NULL);
	if (hResult)
	{
		D3DLogDDError("CreateSurface failed!", hResult);
		D3DCloseWindow();
		ghWnd = NULL;
		return FALSE;
	}
	memset(&tCaps, 0, sizeof(tCaps));
	tCaps.field_00 = 4;

	hResult = glpddsPrimary->lpVtbl->pfnGetAttachedSurface(glpddsPrimary, &tCaps, &glpddsBack);
	if (hResult)
	{
		D3DLogDDError("GetAttachedSurface failed!", hResult);
		D3DCloseWindow();
		ghWnd = NULL;
		return FALSE;
	}
	if (gtVideoConfig.dwFlags & gdwBitMasks[4])
	{
		hResult = glpddsPrimary->lpVtbl->field_00(glpddsPrimary, &IID_IDirectDrawGammaControl, (void **)&sglpGamma);
		if (hResult)
		{
			D3DLogDDError("Failed to get gamma control interface!", hResult);
			D3DCloseWindow();
			ghWnd = NULL;
			return FALSE;
		}
		hResult = sglpGamma->lpVtbl->field_0c(sglpGamma, 0, &sgtGammaOriginal);
		if (hResult)
		{
			D3DLogDDError("Failed to get original gamma ramp", hResult);
			D3DCloseWindow();
			ghWnd = NULL;
			return FALSE;
		}
	}
	hResult = glpD3D->lpVtbl->field_20(glpD3D, &IID_IDirect3DHALDevice, glpddsBack, &glpDevice, NULL);
	if (hResult)
	{
		D3DLogError("Failed to create D3D device!", hResult);
		D3DCloseWindow();
		ghWnd = NULL;
		return FALSE;
	}
	hResult = glpD3D->lpVtbl->field_18(glpD3D, &glpViewport, NULL);
	if (hResult)
	{
		D3DLogError("Failed to create viewport!", hResult);
		D3DCloseWindow();
		ghWnd = NULL;
		return FALSE;
	}
	hResult = glpDevice->lpVtbl->field_14(glpDevice, glpViewport);
	if (hResult)
	{
		D3DLogError("Failed to add viewport to device!", hResult);
		D3DCloseWindow();
		ghWnd = NULL;
		return FALSE;
	}
	memset(&tView, 0, sizeof(tView));
	tView.dwWidth = gnScreenWidth;
	tView.dwSize = sizeof(tView);
	tView.field_04 = 0;
	tView.field_08 = 0;
	tView.dwHeight = gnScreenHeight;
	tView.flWidth = (float)(DWORD)gnScreenWidth;
	tView.flHeight = (float)(DWORD)gnScreenHeight;
	tView.field_14 = 0.0f;
	tView.field_18 = 0.0f;
	tView.field_24 = 0.0f;
	tView.field_28 = 1.0f;
	hResult = glpViewport->lpVtbl->pfnSetViewport(glpViewport, &tView);
	if (hResult)
	{
		D3DLogError("SetViewport failed!", hResult);
		D3DCloseWindow();
		ghWnd = NULL;
		return FALSE;
	}
	hResult = glpDevice->lpVtbl->pfnSetCurrentViewport(glpDevice, glpViewport);
	if (hResult)
	{
		D3DLogError("SetCurrentViewport failed!", hResult);
		D3DCloseWindow();
		ghWnd = NULL;
		return FALSE;
	}
	memset(&tCaps, 0, sizeof(tCaps));
	tCaps.field_00 = 20480;

	hResult = glpDD->lpVtbl->field_5c(glpDD, &tCaps, &sgdwTextureTotal, &sgdwTextureFree);
	if (hResult)
		D3DLogDDError("Failed to get available texture memory", hResult);
	else
		FogErrorManagerLog("\t\tTexture Memory Total= %2.3f MB, Free= %2.3f MB",
			(double)(sgdwTextureTotal * 9.5367431640625e-07f),
				(double)(sgdwTextureFree * 9.5367431640625e-07f));
	memset(&tCaps, 0, sizeof(tCaps));
	tCaps.field_00 = 268455936;

	hResult = glpDD->lpVtbl->field_5c(glpDD, &tCaps, &sgdwLocalTotal, &sgdwLocalFree);
	if (hResult)
		D3DLogDDError("Failed to get available local texture memory", hResult);
	else
		FogErrorManagerLog("\t\t\tLocal Total= %2.3f MB, Free= %2.3f MB",
			(double)(sgdwLocalTotal * 9.5367431640625e-07f), (double)(sgdwLocalFree * 9.5367431640625e-07f));
	if (gdwBitMasks[1] & gtVideoConfig.dwFlags)
	{
		memset(&tCaps, 0, sizeof(tCaps));
		tCaps.field_00 = 536891392;

		hResult = glpDD->lpVtbl->field_5c(glpDD, &tCaps, &sgdwAgpTotal, (DWORD *)&sgnAgpFree);
		if (hResult)
			D3DLogDDError("Failed to get available AGP texture memory", hResult);
		else
			FogErrorManagerLog("\t\t\tAGP Total= %2.3f MB, Free= %2.3f MB",
				(double)(sgdwAgpTotal * 9.5367431640625e-07f),
					(double)((DWORD)sgnAgpFree * 9.5367431640625e-07f));
	}
	else
		FogErrorManagerLog("\t\t\tDoes not support texturing from AGP memory.");
	glpDevice->lpVtbl->field_58(glpDevice, 29, 0);
	glpDevice->lpVtbl->field_58(glpDevice, 22, 1);
	glpDevice->lpVtbl->field_58(glpDevice, 4, 0);
	glpDevice->lpVtbl->field_58(glpDevice, 7, 0);
	glpDevice->lpVtbl->field_58(glpDevice, 14, 0);
	glpDevice->lpVtbl->field_58(glpDevice, 26, 1);
	glpDevice->lpVtbl->field_58(glpDevice, 25, 6);
	glpDevice->lpVtbl->field_58(glpDevice, 24, 0);
	glpDevice->lpVtbl->field_a0(glpDevice, 0, 12, 3);
	D3DSetFilterMode(1);
	glpDevice->lpVtbl->field_a0(glpDevice, 0, 2, 2);
	glpDevice->lpVtbl->field_a0(glpDevice, 0, 3, 0);
	glpDevice->lpVtbl->field_a0(glpDevice, 0, 5, 2);
	glpDevice->lpVtbl->field_a0(glpDevice, 0, 6, 0);
	gfUseAgp = FALSE;
	gnTexelBytes = 2;
	if (gtVideoConfig.dwFlags & gdwBitMasks[1])
	{
		if (sgdwLocalFree < 13631488)
		{
			FogErrorManagerLog("Using AGP Memory");
			gfUseAgp = TRUE;
		}
		else
			FogErrorManagerLog("Ignoring AGP Memory");
	}

	if (gfUseAgp)
	{
		if (sgnAgpFree >= 33554432)
			sgnAgpFree = 33554432;
		if ((int)sgdwLocalFree > sgnAgpFree)
			dwMemory = sgdwLocalFree;
		else
			dwMemory = sgnAgpFree;
	}
	else
		dwMemory = sgdwLocalFree;
	if (!(gnScreenWidth == 640 && gnScreenHeight == 480))
	{
		gtMemoryBudget.dwFloorBase = 0;
		gtMemoryBudget.dwSubtileBase = 0;
		dwBase = D3DSmackCreateBuffer(0);
		dwFirst = dwBase;
		dwRest = dwMemory - dwBase;
		dwLimit = dwRest * 70 / 100 + dwBase;
		dwBase = D3DTextureCacheInit(&gatTextureCache[0], dwBase, dwLimit, 256, 256);
		gtMemoryBudget.dwTexture256Bytes = (gatTextureCache[0].nMaxNumItems * gnTexelBytes) << 16;
		dwLimit = dwRest * 80 / 100 + dwFirst;
		dwBase = D3DTextureCacheInit(&gatTextureCache[1], dwBase, dwLimit, 128, 128);
		gtMemoryBudget.dwTexture128Bytes = (gatTextureCache[1].nMaxNumItems * gnTexelBytes) << 14;
		D3DTextureCacheInit(&gatTextureCache[2], dwBase, dwMemory, 32, 32);
		gtMemoryBudget.dwTexture32Bytes = (gatTextureCache[2].nMaxNumItems * gnTexelBytes) << 10;
	}
	else
	{
		if (gdwBitMasks[6] & gtVideoConfig.dwFlags)
		{
			gnFloorCells = 160;
			gnFloorRows = 80;
		}
		else
		{
			gnFloorCells = 256;
			gnFloorRows = 128;
		}
		dwFloorBytes = gnFloorRows * gnFloorCells * gnTexelBytes;
		dwSubtileBytes = gnTexelBytes << 10;
		dwFloor = D3DMin(dwMemory >> 1, (gnTexelBytes * 3) << 20);
		dwSubtile = D3DMin(dwMemory >> 3, gnTexelBytes << 20);
		dwBase = D3DCreateDebugFont(0);
		gtMemoryBudget.dwFloorBase = dwFloor;
		dwBase = D3DFloorCacheInit(&gtFloorCache, dwBase, dwFloor + dwBase, dwFloorBytes);
		gtMemoryBudget.dwSubtileBase = dwSubtile;
		dwBase = D3DSubtileCacheInit(&gtSubtileCache, dwBase, dwSubtile + dwBase, dwSubtileBytes);
		dwRest = dwMemory - dwBase;
		dwFirst = dwBase;
		dwLimit = dwRest * 20 / 100 + dwBase;
		dwBase = D3DTextureCacheInit(&gatTextureCache[0], dwBase, dwLimit, 256, 256);
		gtMemoryBudget.dwTexture256Bytes = (gatTextureCache[0].nMaxNumItems * gnTexelBytes) << 16;
		dwLimit = dwRest * 75 / 100 + dwFirst;
		dwBase = D3DTextureCacheInit(&gatTextureCache[1], dwBase, dwLimit, 128, 128);
		gtMemoryBudget.dwTexture128Bytes = (gatTextureCache[1].nMaxNumItems * gnTexelBytes) << 14;
		D3DTextureCacheInit(&gatTextureCache[2], dwBase, dwMemory, 32, 32);
		gtMemoryBudget.dwTexture32Bytes = (gatTextureCache[2].nMaxNumItems * gnTexelBytes) << 10;
	}
	D3DSpriteOpen();
	D2CMPTileLibrarySetHardwareFree(D3DTileFreed);
	D3DBlocksOpen();
	D3DEngineOpen();
	FogErrorManagerLog("Opening Direct3D window succeeded.");
	return TRUE;
}

static void WINAPI D3DCloseWindow(void)
{
	HRESULT hResult;

	if (!sgfWindowOpen)
		return;
	FogErrorManagerLog("Closing Direct3D window....");
	D3DEngineClose();
	D3DBlocksClose();
	if (gnScreenWidth == 640)
	{
		if (gnScreenHeight == 480)
		{
			D3DFreeDebugFont();
			D3DFloorCacheFlush(&gtFloorCache, TRUE);
			D3DSubtileCacheFlush(&gtSubtileCache, TRUE);
		}
	}
	else if (gnScreenWidth == 800 && gnScreenHeight == 600)
		D3DSmackFreeBuffer();
	D3DTextureCacheFlush(&gatTextureCache[0], TRUE);
	D3DTextureCacheFlush(&gatTextureCache[1], TRUE);
	D3DTextureCacheFlush(&gatTextureCache[2], TRUE);
	D3DSpriteClose();
	if (glpViewport)
	{
		glpViewport->lpVtbl->field_08(glpViewport);
		glpViewport = NULL;
	}
	if (glpDevice)
	{
		glpDevice->lpVtbl->field_08(glpDevice);
		glpDevice = NULL;
	}
	if (sglpGamma)
	{
		hResult = sglpGamma->lpVtbl->field_10(sglpGamma, 0, &sgtGammaOriginal);
		if (hResult && hResult != (HRESULT)0x887601C2)
			D3DLogDDError("Failed to restore original gamma ramp", hResult);
		sglpGamma->lpVtbl->field_08(sglpGamma);
		sglpGamma = NULL;
	}
	if (sglpddsUnused)
	{
		sglpddsUnused->lpVtbl->field_08(sglpddsUnused);
		sglpddsUnused = NULL;
	}
	if (glpddsBack)
	{
		glpddsBack->lpVtbl->field_08(glpddsBack);
		glpddsBack = NULL;
	}
	if (glpddsPrimary)
	{
		glpddsPrimary->lpVtbl->field_08(glpddsPrimary);
		glpddsPrimary = NULL;
	}
	FogErrorManagerLog("... Direct3D window closed.");
	sgfWindowOpen = FALSE;
	sgfCutsceneWindow = FALSE;
}

BOOL WINAPI D3DCloseWindowSlot(void)
{
	D3DCloseWindow();
	ghWnd = NULL;
	return TRUE;
}

void WINAPI D3DFlushCaches(void)
{
	D3DFloorCacheFlush(&gtFloorCache, FALSE);
	D3DSubtileCacheFlush(&gtSubtileCache, FALSE);
	D3DTextureCacheFlush(&gatTextureCache[0], FALSE);
	D3DTextureCacheFlush(&gatTextureCache[1], FALSE);
	D3DTextureCacheFlush(&gatTextureCache[2], FALSE);
}

void __fastcall D3DSetSuspended(HWND hWnd, int nResolution, BOOL fSuspend)
{
#line 987
	D2ASSERT(pcsPause);
	EnterCriticalSection(pcsPause);
	if (fSuspend)
	{
		gfSuspended = TRUE;
		D3DCloseWindow();
		ghWnd = NULL;
	}
	else
	{
		gfSuspended = FALSE;
		if (!sgfWindowOpen && D3DOpenWindow(hWnd, nResolution))
		{
			D3DClearRender();
			D3DPresent();
		}
	}
	LeaveCriticalSection(pcsPause);
}

static BOOL D3DOpenCutsceneWindow(void)
{
	D3DSURFDESC tDesc;
	HRESULT hResult;
	int nBits;

	gnScreenWidth = 640;
	gnScreenHeight = 480;
	nBits = 16;
	if (gdwBitMasks[7] & gtVideoConfig.dwFlags)
		nBits = 32;
	else if (gdwBitMasks[8] & gtVideoConfig.dwFlags)
		nBits = 24;
	FogErrorManagerLog("Opening Direct3D window at %dx%d at %d bit for cutscenes...", 640, 480, nBits);
	hResult = glpDD->lpVtbl->pfnSetDisplayMode(glpDD, gnScreenWidth, gnScreenHeight, nBits, 0, 0);
	if (hResult)
		D3DLogDDError("SetDisplayMode failed!", hResult);
	else
	{
		tDesc.dwSize = sizeof(tDesc);
		memset(&tDesc.dwFlags, 0, sizeof(tDesc) - 4);
		tDesc.dwFlags = 1;
		tDesc.field_68 = 16896;
		hResult = glpDD->lpVtbl->pfnCreateSurface(glpDD, &tDesc, &glpddsPrimary, NULL);
		if (hResult)
			D3DLogDDError("Failed to create primary surface for cutscenes", hResult);
		else
		{
			sgfWindowOpen = TRUE;
			sgfCutsceneWindow = TRUE;
			FogErrorManagerLog("Opening Direct3D window for cutscenes succeeded.");
			return TRUE;
		}
	}
	FogErrorManagerLog("Opening Direct3D window for cutscenes failed!");
	return FALSE;
}

BOOL WINAPI D3DOpenCutscene(void)
{
	if (gfSuspended)
		return FALSE;
	if (!(gdwBitMasks[7] & gtVideoConfig.dwFlags) && !(gdwBitMasks[8] & gtVideoConfig.dwFlags))
		return TRUE;
	D3DCloseWindow();
#line 1014
	D2ASSERT(!sgfWindowOpen);
	return D3DOpenCutsceneWindow();
}

void __fastcall D3DPlayCutscene(const char *pszFileName, int nMode, void (__stdcall *pfnTick)(void))
{
	FogErrorManagerLog("In Direct3DPlayCutscene");
	if (gfSuspended)
		return;
	sgfInCutscene = TRUE;
	D3DClearRender();
	D3DPresent();
	Direct3DPlayBinkFile(pszFileName, nMode, pfnTick);
	sgfInCutscene = FALSE;
}

BOOL WINAPI D3DIsCutscene(void)
{
	return sgfInCutscene;
}

BOOL __fastcall D3DBeginFrame(void)
{
	HRESULT hResult;

	if (gfSuspended)
		return FALSE;
#line 1127
	D2ASSERT(pcsPause);
	EnterCriticalSection(pcsPause);
	D3DEngineClear(FALSE);
	sgnBlendMode = 0;
	sgnAlphaMode = 0;
	sgnFilterMode = 0;
	D3DSetVertexColor(255, 255, 255);
	hResult = glpDevice->lpVtbl->pfnBeginScene(glpDevice);
	if (hResult)
		D3DLogError("BeginScene failed!", hResult);
	return TRUE;
}

void __fastcall D3DSetUnreadDword(DWORD dwValue)
{
	gdwCoreSlot21 = dwValue;
}

void __fastcall D3DSetGamma(int nGamma)
{
	D3DGAMMARAMP tRamp;
	int nValue;
	int nCap;
	int i;
	HRESULT hResult;

	if (!(gtVideoConfig.dwFlags & gdwBitMasks[4]))
		return;
	for (i = 0; i < 256; i++)
	{
		nValue = (i << 8) * nGamma / 100;
		nCap = nValue < 65535 ? nValue : 65535;
		tRamp.field_200[i] = (WORD)nCap;
		tRamp.field_400[i] = (WORD)nCap;
		tRamp.field_000[i] = (WORD)nCap;
	}
	if (sglpGamma)
	{
		hResult = sglpGamma->lpVtbl->field_10(sglpGamma, 0, &tRamp);
		if (hResult)
			D3DLogDDError("Failed to set gamma ramp", hResult);
	}
}

DWORD WINAPI D3DHasGamma(void)
{
	return gdwBitMasks[4] & gtVideoConfig.dwFlags;
}

BOOL WINAPI D3DEndFrame(void)
{
	HRESULT hResult;

	hResult = glpDevice->lpVtbl->pfnEndScene(glpDevice);
	if (hResult)
		D3DLogError("EndScene failed!", hResult);
#line 1226
	D2ASSERT(pcsPause);
	LeaveCriticalSection(pcsPause);
	return TRUE;
}

BOOL WINAPI D3DPresent(void)
{
	if (gptDrawCtx->dwVSync)
		glpddsPrimary->lpVtbl->field_2c(glpddsPrimary, NULL, 1);
	else
		glpddsPrimary->lpVtbl->field_2c(glpddsPrimary, NULL, 9);
	return TRUE;
}

BOOL __fastcall D3DExpandToRgb(BYTE *pbDest)
{
	D3DSURFDESC tDesc;
	HRESULT hResult;
	const BYTE *pbRow;
	const WORD *pwPixel;
	BYTE *pbOut;
	WORD wPixel;
	int x;
	int y;

	tDesc.dwSize = sizeof(tDesc);
	memset(&tDesc.dwFlags, 0, sizeof(tDesc) - sizeof(DWORD));
	if (!glpddsBack)
		return FALSE;
	hResult = glpddsBack->lpVtbl->pfnLock(glpddsBack, NULL, &tDesc, 17, NULL);
	if (hResult)
	{
		D3DLogDDError("Lock failed!", hResult);
		return FALSE;
	}
	pbRow = tDesc.pbSurface;
	for (y = 0; y < gnScreenHeight; y++)
	{
		pbOut = pbDest;
		pwPixel = (const WORD *)pbRow;
		for (x = 0; x < gnScreenWidth; x++)
		{
			wPixel = *pwPixel++;
			*pbOut++ = (BYTE)((BYTE)(wPixel >> 11) << 3);
			*pbOut++ = (BYTE)((BYTE)(wPixel >> 5) << 2);
			*pbOut++ = (BYTE)((BYTE)wPixel << 3);
		}
		pbRow += tDesc.nPitch;
		pbDest += gnScreenWidth * 3;
	}
	hResult = glpddsBack->lpVtbl->pfnUnlock(glpddsBack, NULL);
	if (hResult)
	{
		D3DLogDDError("Unlock failed!", hResult);
		return FALSE;
	}
	return TRUE;
}

void __fastcall D3DGetScreenSize(int *pnWidth, int *pnHeight)
{
	*pnWidth = gnScreenWidth;
	*pnHeight = gnScreenHeight;
}

void __fastcall D3DSetPaletteEntries(const PALETTEENTRY *ptEntries)
{
	DWORD dwWord;
	DWORD dwRed;
	DWORD dwGreen;
	DWORD dwBlue;
	int i;

	for (i = 0; i < 256; i++)
	{
		gadwPalette[i] = ((((DWORD)ptEntries[i].peRed | 0xFFFFFF00) << 8 | ptEntries[i].peGreen) << 8)
			| ptEntries[i].peBlue;
		dwRed = ptEntries[i].peRed >> 3;
		dwBlue = ptEntries[i].peBlue >> 3;
		dwGreen = ptEntries[i].peGreen >> 3;
		if ((int)dwRed < 31 && (BYTE)(ptEntries[i].peRed & 7) >= 4)
			dwRed++;
		if ((int)dwBlue < 31 && (BYTE)(ptEntries[i].peBlue & 7) >= 4)
			dwBlue++;
		if ((int)dwGreen < 31 && (BYTE)(ptEntries[i].peGreen & 7) >= 4)
			dwGreen++;
		dwWord = ((dwRed | 65504) << 5 | dwGreen) << 5 | dwBlue;
		gawPalette[i] = (WORD)dwWord;
		if (!ptEntries[i].peRed && !ptEntries[i].peGreen && !ptEntries[i].peBlue)
		{
			gadwPalette[i] = 0;
			gawPalette[i] = 0;
		}
	}
}

DWORD __fastcall D3DGetPaletteColor(DWORD bIndex)
{
	return gadwPalette[bIndex];
}

void __fastcall D3DSetEnvInfo(const D3DENVINFO *ptEnvInfo)
{
	sgtEnvInfo = *ptEnvInfo;
	gptEnvInfo = &sgtEnvInfo;
}

void __fastcall D3DCopyRows(const BYTE *pbSource, BYTE *pbDest, int nWidth, int nRows, int nDestPitch,
	DWORD dwUnused)
{
	int i;

	if (nRows <= 0)
		return;
	for (i = nRows; i != 0; i--)
	{
		memcpy(pbDest, pbSource, nWidth);
		pbSource += nWidth;
		pbDest += nDestPitch;
	}
}

void __fastcall D3DExpandToWords(const BYTE *pbSource, WORD *pwDest, int nWidth, int nRows, int nDestPitch,
	DWORD dwUnused)
{
	WORD *pwPixel;
	const BYTE *pbRow;
	WORD *pwRow = pwDest;
	const BYTE *pbSrc = pbSource;
	int x;

	for (; nRows > 0; nRows--)
	{
		pwPixel = pwRow;
		pbRow = pbSrc;
		for (x = nWidth; x > 0; x--)
			*pwPixel++ = gawPalette[*pbRow++];
		pbSrc += nWidth;
		pwRow += nDestPitch;
	}

}

void __fastcall D3DExpandToDwords(const BYTE *pbSource, DWORD *pdwDest, int nWidth, int nRows, int nDestPitch,
	DWORD dwUnused)
{
	DWORD *pdwPixel;
	const BYTE *pbRow;
	DWORD *pdwRow = pdwDest;
	const BYTE *pbSrc = pbSource;
	int x;

	for (; nRows > 0; nRows--)
	{
		pdwPixel = pdwRow;
		pbRow = pbSrc;
		for (x = nWidth; x > 0; x--)
			*pdwPixel++ = gadwPalette[*pbRow++];
		pbSrc += nWidth;
		pdwRow += nDestPitch;
	}

}

BOOL __fastcall D3DReinit(HWND hWnd, int nResolution)
{
	if (!gfSuspended)
	{
		D3DCloseWindow();
		ghWnd = NULL;
		if (!D3DOpenWindow(hWnd, nResolution))
			return FALSE;
		D3DClearRender();
		D3DPresent();
	}
	return TRUE;
}

void __fastcall D3DSetBlendMode(int nMode)
{
	if (sgnBlendMode && nMode == sgnBlendMode)
		return;
	switch (nMode)
	{
	case 1:
		glpDevice->lpVtbl->field_a0(glpDevice, 0, 1, 4);
		glpDevice->lpVtbl->field_a0(glpDevice, 0, 4, 4);
		glpDevice->lpVtbl->field_58(glpDevice, 15, 1);
		break;

	case 2:
		glpDevice->lpVtbl->field_a0(glpDevice, 0, 1, 3);
		glpDevice->lpVtbl->field_a0(glpDevice, 0, 4, 3);
		glpDevice->lpVtbl->field_58(glpDevice, 15, 0);
		break;
	}
	sgnBlendMode = nMode;
}

void __fastcall D3DSetAlphaMode(int nMode)
{
	if (sgnAlphaMode && nMode == sgnAlphaMode)
		return;
	switch (nMode)
	{
	case 1:
		glpDevice->lpVtbl->field_58(glpDevice, 27, 0);
		glpDevice->lpVtbl->field_58(glpDevice, 19, 2);
		glpDevice->lpVtbl->field_58(glpDevice, 20, 1);
		break;

	case 2:
		glpDevice->lpVtbl->field_58(glpDevice, 27, 1);
		glpDevice->lpVtbl->field_58(glpDevice, 19, 2);
		glpDevice->lpVtbl->field_58(glpDevice, 20, 2);
		break;

	case 3:
		glpDevice->lpVtbl->field_58(glpDevice, 27, 1);
		glpDevice->lpVtbl->field_58(glpDevice, 19, 1);
		glpDevice->lpVtbl->field_58(glpDevice, 20, 3);
		break;

	case 4:
		glpDevice->lpVtbl->field_58(glpDevice, 27, 1);
		glpDevice->lpVtbl->field_58(glpDevice, 19, 5);
		glpDevice->lpVtbl->field_58(glpDevice, 20, 6);
		break;
	}
	sgnAlphaMode = nMode;
}

void __fastcall D3DSetFilterMode(int nMode)
{
	if (sgnFilterMode && nMode == sgnFilterMode)
		return;
	switch (nMode)
	{
	case 1:
		glpDevice->lpVtbl->field_a0(glpDevice, 0, 16, 1);
		glpDevice->lpVtbl->field_a0(glpDevice, 0, 17, 1);
		break;

	case 2:
		glpDevice->lpVtbl->field_a0(glpDevice, 0, 16, 2);
		glpDevice->lpVtbl->field_a0(glpDevice, 0, 17, 2);
		break;
	}
	sgnFilterMode = nMode;
}

BOOL __fastcall D3DCreateMemoryTexture(D3DSURFACE **pptSurface, D3DTEXTURE **pptTexture, int nWidth,
	int nHeight)
{
	D3DSURFDESC tDesc;
	HRESULT hResult;

	tDesc.dwSize = sizeof(tDesc);
	memset(&tDesc.dwFlags, 0, sizeof(tDesc) - sizeof(DWORD));
	tDesc.dwFlags = 4103;
	tDesc.field_68 = 2112;
	memcpy(tDesc.adwPixelFormat, &sgtTexturePixelFormat, sizeof(tDesc.adwPixelFormat));
	tDesc.dwWidth = nWidth;
	tDesc.dwHeight = nHeight;
	hResult = glpDD->lpVtbl->pfnCreateSurface(glpDD, &tDesc, pptSurface, NULL);
	if (hResult)
	{
		D3DLogDDError("CreateSurface (memory surface)", hResult);
		return FALSE;
	}
	hResult = (*pptSurface)->lpVtbl->field_00(*pptSurface, &IID_IDirect3DTexture2, (void **)pptTexture);
	if (hResult)
	{
		D3DLogDDError("Failed to get texture interface for memory surface", hResult);
		(*pptSurface)->lpVtbl->field_08(*pptSurface);
		return FALSE;
	}
	return TRUE;
}

BOOL __fastcall D3DCreateDeviceTexture(D3DSURFACE **pptSurface, D3DTEXTURE **pptTexture, int nWidth,
	int nHeight)
{
	D3DSURFDESC tDesc;
	HRESULT hResult;

	tDesc.dwSize = sizeof(tDesc);
	memset(&tDesc.dwFlags, 0, sizeof(tDesc) - sizeof(DWORD));
	tDesc.dwFlags = 4103;
	tDesc.field_6c = 128;
	memcpy(tDesc.adwPixelFormat, &sgtTexturePixelFormat, sizeof(tDesc.adwPixelFormat));
	tDesc.dwWidth = nWidth;
	tDesc.dwHeight = nHeight;
	tDesc.field_68 = gfUseAgp ? 20480 : 268455936;
	hResult = glpDD->lpVtbl->pfnCreateSurface(glpDD, &tDesc, pptSurface, NULL);
	if (hResult)
	{
		if (hResult != (HRESULT)0x8876017C)
			D3DLogDDError("CreateSurface (device surface)", hResult);
		return FALSE;
	}
	hResult = (*pptSurface)->lpVtbl->field_00(*pptSurface, &IID_IDirect3DTexture2, (void **)pptTexture);
	if (hResult)
	{
		D3DLogError("Failed to get texture interface for device surface", hResult);
		(*pptSurface)->lpVtbl->field_08(*pptSurface);
		return FALSE;
	}
	return TRUE;
}

DWORD * WINAPI D3DGetMemoryBudget(void)
{
	return &gtMemoryBudget.dwFloorBase;
}

BOOL WINAPI D3DAlwaysFalse(void)
{
	return FALSE;
}

BOOL WINAPI D3DGetFrameOpenQuery(void)
{
	return FALSE;
}

void WINAPI D3DClearRender(void)
{
	D3DSURFDESC tDesc;
	D3DSURFACE *lpdds;
	HRESULT hResult;
	BYTE *pbRow;
	DWORD dwRows;   int nPitch;

	lpdds = glpddsBack ? glpddsBack : glpddsPrimary;

	tDesc.dwSize = sizeof(tDesc);
	memset(&tDesc.dwFlags, 0, sizeof(tDesc) - sizeof(DWORD));
	hResult = lpdds->lpVtbl->pfnLock(lpdds, NULL, &tDesc, 33, NULL);
	if (hResult)
	{
		D3DLogDDError("Failed to lock render surface for clear", hResult);
		return;
	}
	pbRow = tDesc.pbSurface;   nPitch = tDesc.nPitch;
	dwRows = gnScreenHeight;
	while (dwRows-- != 0)
	{
		memset(pbRow, 0, nPitch);
		pbRow += nPitch;
	}
	lpdds->lpVtbl->pfnUnlock(lpdds, NULL);
}

void __fastcall D3DCopySurface(D3DSURFACE *ptDest, D3DSURFACE *ptSource, int nRows)
{
	D3DSURFDESC tSource = { sizeof(tSource) };
	HRESULT hResult;
	const BYTE *pbFrom;
	BYTE *pbTo;
	int nFromPitch;
	D3DSURFDESC *ptFrom = &tSource;
	int nToPitch;
	int nCopy;

	hResult = ptSource->lpVtbl->pfnLock(ptSource, NULL, &tSource, 33, NULL);
	if (hResult)
		D3DLogDDError("Failed to lock source surface", hResult);
	else
	{
		pbFrom = tSource.pbSurface;
		D3DSURFDESC tDest = { sizeof(tDest) };

		hResult = ptDest->lpVtbl->pfnLock(ptDest, NULL, &tDest, 33, NULL);
		if (hResult)
			D3DLogDDError("Failed to lock dest surface", hResult);
		else
		{
			nToPitch = tDest.nPitch;
			pbTo = tDest.pbSurface;
			nFromPitch = ptFrom->nPitch;
			nCopy = nFromPitch;
			if (nCopy >= nToPitch)
				nCopy = nToPitch;
			while (nRows--)
			{
				memcpy(pbTo, pbFrom, nCopy);
				pbTo += nToPitch;
				pbFrom += nFromPitch;
			}
		}
	}
	ptSource->lpVtbl->pfnUnlock(ptSource, NULL);
	ptDest->lpVtbl->pfnUnlock(ptDest, NULL);
}

void __fastcall D3DLogError(const char *pszMessage, HRESULT hResult)
{
	const char *pszName;

	switch (hResult)
	{
	case 0:
		pszName = "D3D_OK";
		break;

	case 0x887602BC:
		pszName = "D3DERR_BADMAJORVERSION";
		break;

	case 0x887602BD:
		pszName = "D3DERR_BADMINORVERSION";
		break;

	case 0x887602C1:
		pszName = "D3DERR_INVALID_DEVICE";
		break;

	case 0x887602C2:
		pszName = "D3DERR_INITFAILED";
		break;

	case 0x887602C3:
		pszName = "D3DERR_DEVICEAGGREGATED";
		break;

	case 0x887602C6:
		pszName = "D3DERR_EXECUTE_CREATE_FAILED";
		break;

	case 0x887602C7:
		pszName = "D3DERR_EXECUTE_DESTROY_FAILED";
		break;

	case 0x887602C8:
		pszName = "D3DERR_EXECUTE_LOCK_FAILED";
		break;

	case 0x887602C9:
		pszName = "D3DERR_EXECUTE_UNLOCK_FAILED";
		break;

	case 0x887602CA:
		pszName = "D3DERR_EXECUTE_LOCKED";
		break;

	case 0x887602CB:
		pszName = "D3DERR_EXECUTE_NOT_LOCKED";
		break;

	case 0x887602CC:
		pszName = "D3DERR_EXECUTE_FAILED";
		break;

	case 0x887602CD:
		pszName = "D3DERR_EXECUTE_CLIPPED_FAILED";
		break;

	case 0x887602D0:
		pszName = "D3DERR_TEXTURE_NO_SUPPORT";
		break;

	case 0x887602D1:
		pszName = "D3DERR_TEXTURE_CREATE_FAILED";
		break;

	case 0x887602D2:
		pszName = "D3DERR_TEXTURE_DESTROY_FAILED";
		break;

	case 0x887602D3:
		pszName = "D3DERR_TEXTURE_LOCK_FAILED";
		break;

	case 0x887602D4:
		pszName = "D3DERR_TEXTURE_UNLOCK_FAILED";
		break;

	case 0x887602D5:
		pszName = "D3DERR_TEXTURE_LOAD_FAILED";
		break;

	case 0x887602D6:
		pszName = "D3DERR_TEXTURE_SWAP_FAILED";
		break;

	case 0x887602D7:
		pszName = "D3DERR_TEXTURE_LOCKED";
		break;

	case 0x887602D8:
		pszName = "D3DERR_TEXTURE_NOT_LOCKED";
		break;

	case 0x887602D9:
		pszName = "D3DERR_TEXTURE_GETSURF_FAILED";
		break;

	case 0x887602DA:
		pszName = "D3DERR_MATRIX_CREATE_FAILED";
		break;

	case 0x887602DB:
		pszName = "D3DERR_MATRIX_DESTROY_FAILED";
		break;

	case 0x887602DC:
		pszName = "D3DERR_MATRIX_SETDATA_FAILED";
		break;

	case 0x887602DD:
		pszName = "D3DERR_MATRIX_GETDATA_FAILED";
		break;

	case 0x887602DE:
		pszName = "D3DERR_SETVIEWPORTDATA_FAILED";
		break;

	case 0x887602DF:
		pszName = "D3DERR_INVALIDCURRENTVIEWPORT";
		break;

	case 0x887602E0:
		pszName = "D3DERR_INVALIDPRIMITIVETYPE";
		break;

	case 0x887602E1:
		pszName = "D3DERR_INVALIDVERTEXTYPE";
		break;

	case 0x887602E2:
		pszName = "D3DERR_TEXTURE_BADSIZE";
		break;

	case 0x887602E3:
		pszName = "D3DERR_INVALIDRAMPTEXTURE";
		break;

	case 0x887602E4:
		pszName = "D3DERR_MATERIAL_CREATE_FAILED";
		break;

	case 0x887602E5:
		pszName = "D3DERR_MATERIAL_DESTROY_FAILED";
		break;

	case 0x887602E6:
		pszName = "D3DERR_MATERIAL_SETDATA_FAILED";
		break;

	case 0x887602E7:
		pszName = "D3DERR_MATERIAL_GETDATA_FAILED";
		break;

	case 0x887602E8:
		pszName = "D3DERR_INVALIDPALETTE";
		break;

	case 0x887602E9:
		pszName = "D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY";
		break;

	case 0x887602EA:
		pszName = "D3DERR_ZBUFF_NEEDS_VIDEOMEMORY";
		break;

	case 0x887602EB:
		pszName = "D3DERR_SURFACENOTINVIDMEM";
		break;

	case 0x887602EE:
		pszName = "D3DERR_LIGHT_SET_FAILED";
		break;

	case 0x887602EF:
		pszName = "D3DERR_LIGHTHASVIEWPORT";
		break;

	case 0x887602F0:
		pszName = "D3DERR_LIGHTNOTINTHISVIEWPORT";
		break;

	case 0x887602F8:
		pszName = "D3DERR_SCENE_IN_SCENE";
		break;

	case 0x887602F9:
		pszName = "D3DERR_SCENE_NOT_IN_SCENE";
		break;

	case 0x887602FA:
		pszName = "D3DERR_SCENE_BEGIN_FAILED";
		break;

	case 0x887602FB:
		pszName = "D3DERR_SCENE_END_FAILED";
		break;

	case 0x88760302:
		pszName = "D3DERR_INBEGIN";
		break;

	case 0x88760303:
		pszName = "D3DERR_NOTINBEGIN";
		break;

	case 0x88760304:
		pszName = "D3DERR_NOVIEWPORTS";
		break;

	case 0x88760305:
		pszName = "D3DERR_VIEWPORTDATANOTSET";
		break;

	case 0x88760306:
		pszName = "D3DERR_VIEWPORTHASNODEVICE";
		break;

	case 0x88760307:
		pszName = "D3DERR_NOCURRENTVIEWPORT";
		break;
	default:
		D3DLogDDError(pszMessage, hResult);
		return;
	}
	FogErrorManagerLog("***** Direct3D error: %s - %s (0x%x)", pszMessage, pszName, hResult);
}

void __fastcall D3DLogDDError(const char *pszMessage, HRESULT hResult)
{
	const char *pszName;

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
