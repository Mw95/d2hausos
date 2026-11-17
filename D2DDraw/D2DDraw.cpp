#define D2_THIS_FILE "C:\\D2\\Source\\D2DDraw\\Src\\D2DDraw.cpp"
#include "ddCore.h"

BOOL WINAPI DdDetectBoards(void);
BOOL __fastcall DdSetup(DDDRAWCTX *ptDrawCtx, DDBLITTABLE *ptBlitTable);
BOOL __fastcall DdTeardown(void);
void WINAPI DdNothingFlushCaches(void);
BOOL __fastcall DdInit(HWND hWnd, int nResolution);
void __fastcall DdSetSuspended(HWND hWnd, int nResolution, BOOL fSuspend);
BOOL WINAPI DdOpenCutscene(void);
void __fastcall DdPlayCutscene(const char *pszFileName, int nMode, void (__stdcall *pfnTick)(void));
DWORD WINAPI DdIsCutscene(void);
BOOL WINAPI DdCloseWindowAndPalette(void);
BOOL WINAPI DdLockRender(void);
BOOL WINAPI DdUnlockRender(void);
BOOL __fastcall DdPresent(void);
BOOL __fastcall DdExpandToRgb(BYTE *pbDest);
void __fastcall DdGetScreenSize(int *pnWidth, int *pnHeight);
int __fastcall DdSetPaletteEntries(const PALETTEENTRY *ptEntries);
void __fastcall DdSetEnvInfo(const DDENVINFO *ptEnvInfo);
void WINAPI DdNothingSetUnreadDword(void);
int __fastcall DdSetGamma(int nGamma);
BOOL WINAPI DdHasGamma(void);
void WINAPI DdNothingSetVertexColor(DWORD dwValue);
void WINAPI DdNothingSetPerspOrigin(void);
void WINAPI DdNothingPerspProject(DWORD dwValue1, DWORD dwValue2, DWORD dwValue3);
void WINAPI DdNothingPerspProjectDepth(DWORD dwValue1, DWORD dwValue2, DWORD dwValue3, DWORD dwValue4);
void WINAPI DdNothingResetPerspDepth(void);
BOOL __fastcall DdReinit(HWND hWnd, int nResolution);
BOOL WINAPI DdGetMemoryBudget(void);
BOOL WINAPI DdAlwaysFalse(void);
BOOL WINAPI DdGetFrameOpenQuery(void);

DWORD __fastcall DdGetInterface(void)
{
	static void *sgapfnInterface[53] =
	{
		(void *)DdDetectBoards,
		(void *)DdSetup,
		(void *)DdTeardown,
		(void *)DdInit,
		(void *)DdCloseWindowAndPalette,
		(void *)DdSetSuspended,
		(void *)DdLockRender,
		(void *)DdUnlockRender,
		(void *)DdPresent,
		(void *)DdReinit,
		(void *)DdExpandToRgb,
		(void *)DdAlwaysFalse,
		(void *)DdGetFrameOpenQuery,
		(void *)DdOpenCutscene,
		(void *)DdPlayCutscene,
		(void *)DdIsCutscene,
		(void *)DdSmackOpen,
		(void *)DdSmackUpdate,
		(void *)DdSmackClose,
		(void *)DdGetMemoryBudget,
		(void *)DdGetScreenSize,
		(void *)DdNothingSetUnreadDword,
		(void *)DdSetGamma,
		(void *)DdHasGamma,
		(void *)DdNothingSetPerspOrigin,
		(void *)DdNothingPerspProject,
		(void *)DdNothingPerspProjectDepth,
		(void *)DdNothingResetPerspDepth,
		(void *)DdSetPaletteEntries,
		(void *)DdSetEnvInfo,
		(void *)DdNothingSetVertexColor,
		(void *)DdDrawTile,
		(void *)DdDrawCelSkip,
		(void *)DdDrawCel,
		(void *)DdDrawCelPreMap,
		(void *)DdSpriteDrawRows,
		(void *)DdSpriteDrawShadow,
		(void *)DdSpriteStub,
		(void *)DdSpriteDrawWallLow,
		(void *)DdSpriteDrawWallShaded,
		(void *)DdSpriteDrawUnitBlocks,
		(void *)DdDrawDiamond,
		(void *)DdDrawFrame,
		(void *)DdFillRect,
		(void *)DdFillSquare,
		(void *)DdFillSpan,
		(void *)DdFillSpanSelect,
		(void *)DdDrawLine,
		(void *)DdClearRender,
		(void *)DdDrawDebugText,
		(void *)DdNothingDebugLightGrid,
		(void *)DdSpriteWarp,
		(void *)DdNothingFlushCaches,
	};

	return (DWORD)sgapfnInterface;
}

char gszRegistrationTable[128] = "REGISTRATION_TABLE";
char gszRegistrationBlock[128] = "REGISTRATION_BLOCK";
