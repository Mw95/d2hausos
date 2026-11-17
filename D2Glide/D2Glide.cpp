#include "glCore.h"
#include "StormImports.h"
#include "D2GlideImports.h"
extern const char *const gpszModuleName = "D2GLIDE.DLL";

struct GLTILE;
struct GLTILEPOINT;
struct GLSMACKVIDEO;
struct D2CELCONTEXT;

BOOL __cdecl GlideDetectBoards(void);
BOOL __fastcall GlideSetup(GLDRAWCTX *ptDrawCtx, GLBLITTABLE *ptBlitTable);
BOOL __fastcall GlideTeardown(void);
BOOL __fastcall GlideInit(HWND hWnd, int nResolution);
void __fastcall GlideSetSuspended(HWND hWnd, int nResolution, BOOL fSuspend);
BOOL __fastcall GlideBeginFrame(void);
BOOL __cdecl GlideEndFrame(void);
BOOL __cdecl GlidePresent(void);
BOOL __fastcall GlideReinit(HWND hWnd, int nResolution);
BOOL __fastcall GlideExpandToRgb(BYTE *pbDest);
BOOL __cdecl GlideAlwaysFalse(void);
BOOL __fastcall GlideGetFrameOpenQuery(int nQuery, DWORD *pdwValue);
BOOL __cdecl GlideIsActive(void);
void __fastcall GlidePlayCutscene(const char *pszFileName, int nMode, void (__stdcall *pfnTick)(void));
BOOL __cdecl GlideIsCutscene(void);
DWORD *__cdecl GlideGetMemoryBudget(void);
void __fastcall GlideGetScreenSize(int *pnWidth, int *pnHeight);
void __fastcall GlideSetUnreadDword(DWORD dwValue);
void __fastcall GlideSetGamma(int nGamma);
BOOL __cdecl GlideHasGamma(void);
DWORD __fastcall GlideSetPaletteEntries(const PALETTEENTRY *ptEntries);
void __fastcall GlideSetEnvInfo(const GLENVINFO *ptEnvInfo);
void __cdecl GlideFlushCaches(void);
void __fastcall GlideDrawDebugText(int nX, int nY, const char *pszFormat, va_list vaArgs);
void __fastcall GlideSetAndDrawDebugLightGrid(const DWORD *pdwPalette, const DWORD *pdwGrid, DWORD dwLightX,
	DWORD dwLightY);
void __fastcall GlideDrawTile(GLTILE *pTile, const GLTILEPOINT *ptPoints, DWORD dwX, DWORD dwY,
	DWORD dwTileX, DWORD dwTileY, BYTE bAlpha, DWORD dwBoundaryMode, int nFilter);
void __fastcall GlideDrawDiamond(const RECT *prc, BYTE bColor);
void __fastcall GlideDrawFrame(const RECT *prc, BYTE bColor);
void __fastcall GlideFillRect(const RECT *prc, BYTE bColor);
void __fastcall GlideFillSquare(const POINT *ptCenter, BYTE bSize, BYTE bColor);
void __fastcall GlideFillSpan(int nX1, int nY1, int nX2, int nY2, BYTE bColor, int nMode);
void __fastcall GlideFillSpanAlpha(int nX1, int nY1, int nX2, int nY2, BYTE bColor, BYTE bAlpha);
void __fastcall GlideDrawLine(int nX1, int nY1, int nX2, int nY2, BYTE bColor, BYTE bAlpha);
void __fastcall GlideEngineClear(BOOL fAbovePanel);
void __fastcall GlideSetPerspectiveOrigin(int nX, int nY);
void __fastcall GlidePerspectiveProjectDepth(int nX, int nY, int nZ, int *pnX, int *pnY, BOOL fFar);
void __cdecl GlideResetPerspectiveDepth(void);
void __fastcall GlideSmackOpen(const char *pszFileName, GLSMACKVIDEO *ptVideo);
void __fastcall GlideSmackUpdate(GLSMACKVIDEO *ptVideo);
void __fastcall GlideSmackClose(GLSMACKVIDEO *ptVideo);
BOOL __fastcall GlideDrawCelPerspective(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	int nBoundary, const BYTE *pbPreMap);
BOOL __fastcall GlideDrawCel(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	const BYTE *pbPreMap);
BOOL __fastcall GlideDrawCelPalette(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	int nPalette);
void __fastcall GlideSpriteDrawRows(D2CELCONTEXT *ptCelContext, int nX, int nY, int nFromBottom, int nRows,
	int nMode);
void __fastcall GlideSpriteDrawShadow(D2CELCONTEXT *ptCelContext, int nX, int nY);
void __fastcall GlideSpriteDrawHighlight(D2CELCONTEXT *ptCelContext, int nX, int nY, BYTE bColor);
void __fastcall GlideSpriteWarp(int nX, int nY);
BOOL __fastcall GlideDrawWallOpaque(GLTILE *pTile, int nX, int nY, const DWORD *pdwLight, int nBoundary);
BOOL __fastcall GlideDrawWallLit(GLTILE *pTile, int nX, int nY, const DWORD *pdwLight, int nBoundary,
	BYTE bAlpha);
BOOL __fastcall GlideDrawWallLight(GLTILE *pTile, int nX, int nY, BYTE bLight, int nBoundary);

DWORD __fastcall GlideGetInterface(void)
{
	static void *sgapfnInterface[53] =
	{
		(void *)GlideDetectBoards,
		(void *)GlideSetup,
		(void *)GlideTeardown,
		(void *)GlideInit,
		(void *)GlideCloseWindow,
		(void *)GlideSetSuspended,
		(void *)GlideBeginFrame,
		(void *)GlideEndFrame,
		(void *)GlidePresent,
		(void *)GlideReinit,
		(void *)GlideExpandToRgb,
		(void *)GlideAlwaysFalse,
		(void *)GlideGetFrameOpenQuery,
		(void *)GlideIsActive,
		(void *)GlidePlayCutscene,
		(void *)GlideIsCutscene,
		(void *)GlideSmackOpen,
		(void *)GlideSmackUpdate,
		(void *)GlideSmackClose,
		(void *)GlideGetMemoryBudget,
		(void *)GlideGetScreenSize,
		(void *)GlideSetUnreadDword,
		(void *)GlideSetGamma,
		(void *)GlideHasGamma,
		(void *)GlideSetPerspectiveOrigin,
		(void *)GlidePerspectiveProject,
		(void *)GlidePerspectiveProjectDepth,
		(void *)GlideResetPerspectiveDepth,
		(void *)GlideSetPaletteEntries,
		(void *)GlideSetEnvInfo,
		(void *)GlideSetVertexColor,
		(void *)GlideDrawTile,
		(void *)GlideDrawCelPerspective,
		(void *)GlideDrawCel,
		(void *)GlideDrawCelPalette,
		(void *)GlideSpriteDrawRows,
		(void *)GlideSpriteDrawShadow,
		(void *)GlideSpriteDrawHighlight,
		(void *)GlideDrawWallOpaque,
		(void *)GlideDrawWallLit,
		(void *)GlideDrawWallLight,
		(void *)GlideDrawDiamond,
		(void *)GlideDrawFrame,
		(void *)GlideFillRect,
		(void *)GlideFillSquare,
		(void *)GlideFillSpan,
		(void *)GlideFillSpanAlpha,
		(void *)GlideDrawLine,
		(void *)GlideEngineClear,
		(void *)GlideDrawDebugText,
		(void *)GlideSetAndDrawDebugLightGrid,
		(void *)GlideSpriteWarp,
		(void *)GlideFlushCaches,
	};

	return (DWORD)sgapfnInterface;
}

char gszRegistrationTable[128] = "REGISTRATION_TABLE";
char gszRegistrationBlock[128] = "REGISTRATION_BLOCK";

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
