#include "d3dCaches.h"
#include "StormImports.h"
#include "D2Direct3DImports.h"
extern const char *const gpszModuleName = "D2DIRECT3D.DLL";

struct D2CELCONTEXT;
struct D3DTILEPOINT;
struct D3DSMACKVIDEO;

BOOL WINAPI D3DDetectBoards(void);
BOOL __fastcall D3DSetup(D3DDRAWCTX *ptDrawCtx, D3DBLITTABLE *ptBlitTable);
BOOL __fastcall D3DTeardown(void);
BOOL __fastcall D3DInit(HWND hWnd, int nResolution);
BOOL WINAPI D3DCloseWindowSlot(void);
void __fastcall D3DSetSuspended(HWND hWnd, int nResolution, BOOL fSuspend);
BOOL __fastcall D3DBeginFrame(void);
BOOL WINAPI D3DEndFrame(void);
BOOL WINAPI D3DPresent(void);
BOOL __fastcall D3DReinit(HWND hWnd, int nResolution);
BOOL __fastcall D3DExpandToRgb(BYTE *pbDest);
BOOL WINAPI D3DAlwaysFalse(void);
BOOL WINAPI D3DGetFrameOpenQuery(void);
BOOL WINAPI D3DOpenCutscene(void);
void __fastcall D3DPlayCutscene(const char *pszFileName, int nMode, void (__stdcall *pfnTick)(void));
BOOL WINAPI D3DIsCutscene(void);
DWORD * WINAPI D3DGetMemoryBudget(void);
void __fastcall D3DGetScreenSize(int *pnWidth, int *pnHeight);
void __fastcall D3DSetUnreadDword(DWORD dwValue);
void __fastcall D3DSetGamma(int nGamma);
DWORD WINAPI D3DHasGamma(void);
void __fastcall D3DSetPaletteEntries(const PALETTEENTRY *ptEntries);
void __fastcall D3DSetEnvInfo(const D3DENVINFO *ptEnvInfo);
void WINAPI D3DFlushCaches(void);
void __fastcall D3DDrawDebugText(int nX, int nY, const char *pszFormat, va_list vaArgs);
void WINAPI D3DNothingDebugLightGrid(DWORD dwValue1, DWORD dwValue2);
void __fastcall D3DDrawTile(D3DTILE *pTile, const D3DTILEPOINT *ptPoints, DWORD dwX, DWORD dwY,
	DWORD dwTileX, DWORD dwTileY, BYTE bAlpha, DWORD dwBoundaryMode, int nFilter);
void __fastcall D3DDrawDiamond(const RECT *prc, BYTE bColor);
void __fastcall D3DDrawFrame(const RECT *prc, BYTE bColor);
void __fastcall D3DFillRect(const RECT *prc, BYTE bColor);
void __fastcall D3DFillSquare(const POINT *ptCenter, BYTE bSize, BYTE bColor);
void __fastcall D3DFillSpan(int nX1, int nY1, int nX2, int nY2, BYTE bColor, int nMode);
void __fastcall D3DFillSpanAlpha(int nX1, int nY1, int nX2, int nY2, BYTE bColor, BYTE bAlpha);
void __fastcall D3DDrawLine(int nX1, int nY1, int nX2, int nY2, BYTE bColor, BYTE bAlpha);
void __fastcall D3DSetPerspectiveOrigin(int nX, int nY);
void __fastcall D3DPerspectiveProjectDepth(int nX, int nY, int nZ, int *pnX, int *pnY, BOOL fFar);
void WINAPI D3DResetPerspectiveDepth(void);
void __fastcall D3DSmackOpen(const char *pszFileName, D3DSMACKVIDEO *ptVideo);
void __fastcall D3DSmackUpdate(D3DSMACKVIDEO *ptVideo);
void __fastcall D3DSmackClose(D3DSMACKVIDEO *ptVideo);
BOOL __fastcall D3DDrawCelPerspective(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	int nBoundary, const BYTE *pbPreMap);
BOOL __fastcall D3DDrawCel(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	const BYTE *pbPreMap);
BOOL __fastcall D3DDrawCelPalette(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	int nPalette);
void __fastcall D3DSpriteDrawRows(D2CELCONTEXT *ptCelContext, int nX, int nY, int nFromBottom, int nRows,
	int nMode);
void __fastcall D3DSpriteDrawShadow(D2CELCONTEXT *ptCelContext, int nX, int nY);
void __fastcall D3DSpriteDrawHighlight(D2CELCONTEXT *ptCelContext, int nX, int nY, BYTE bColor);
void __fastcall D3DSpriteWarp(int nX, int nY);
BOOL __fastcall D3DDrawWallOpaque(D3DTILE *pTile, int nX, int nY, const DWORD *pdwLight, int nBoundary);
BOOL __fastcall D3DDrawWallLit(D3DTILE *pTile, int nX, int nY, const DWORD *pdwLight, int nBoundary,
	BYTE bAlpha);
BOOL __fastcall D3DDrawWallLight(D3DTILE *pTile, int nX, int nY, BYTE bLight, int nBoundary);

DWORD __fastcall D3DGetInterface(void)
{
	static void *sgapfnInterface[53] =
	{
		(void *)D3DDetectBoards,
		(void *)D3DSetup,
		(void *)D3DTeardown,
		(void *)D3DInit,
		(void *)D3DCloseWindowSlot,
		(void *)D3DSetSuspended,
		(void *)D3DBeginFrame,
		(void *)D3DEndFrame,
		(void *)D3DPresent,
		(void *)D3DReinit,
		(void *)D3DExpandToRgb,
		(void *)D3DAlwaysFalse,
		(void *)D3DGetFrameOpenQuery,
		(void *)D3DOpenCutscene,
		(void *)D3DPlayCutscene,
		(void *)D3DIsCutscene,
		(void *)D3DSmackOpen,
		(void *)D3DSmackUpdate,
		(void *)D3DSmackClose,
		(void *)D3DGetMemoryBudget,
		(void *)D3DGetScreenSize,
		(void *)D3DSetUnreadDword,
		(void *)D3DSetGamma,
		(void *)D3DHasGamma,
		(void *)D3DSetPerspectiveOrigin,
		(void *)D3DPerspectiveProject,
		(void *)D3DPerspectiveProjectDepth,
		(void *)D3DResetPerspectiveDepth,
		(void *)D3DSetPaletteEntries,
		(void *)D3DSetEnvInfo,
		(void *)D3DSetVertexColor,
		(void *)D3DDrawTile,
		(void *)D3DDrawCelPerspective,
		(void *)D3DDrawCel,
		(void *)D3DDrawCelPalette,
		(void *)D3DSpriteDrawRows,
		(void *)D3DSpriteDrawShadow,
		(void *)D3DSpriteDrawHighlight,
		(void *)D3DDrawWallOpaque,
		(void *)D3DDrawWallLit,
		(void *)D3DDrawWallLight,
		(void *)D3DDrawDiamond,
		(void *)D3DDrawFrame,
		(void *)D3DFillRect,
		(void *)D3DFillSquare,
		(void *)D3DFillSpan,
		(void *)D3DFillSpanAlpha,
		(void *)D3DDrawLine,
		(void *)D3DEngineClear,
		(void *)D3DDrawDebugText,
		(void *)D3DNothingDebugLightGrid,
		(void *)D3DSpriteWarp,
		(void *)D3DFlushCaches,
	};

	return (DWORD)sgapfnInterface;
}

char gszRegistrationTable[128] = "REGISTRATION_TABLE";
char gszRegistrationBlock[128] = "REGISTRATION_BLOCK";

static DWORD PLACEHOLDER_UnreferencedData_100241d0[1024] = { 0 };

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
