#ifndef D2RE_D2DDRAW_DDCORE_H
#define D2RE_D2DDRAW_DDCORE_H

#include <windows.h>
#include <stdarg.h>

struct DDDRAWCTX
{
 	BYTE		unobserved_000[20];
 	DWORD		dwBlendShadow;
};

struct DDBLITTABLE
{
 	void		(__fastcall *pfnSetSurface)(BYTE *pbSurface, int nWidth, int nHeight,
					DWORD dwZero);
 	DWORD		(__fastcall *pfnDrawBlock)(void *pData, int nX, int nY);
 	void		*unobserved_008;
 	DWORD		(__fastcall *pfnDrawTile)(void *ptTile, int nX, DWORD dwY, DWORD dwVertices);
 	DWORD		(__fastcall *pfnDrawBlockShaded)(void *pData, int nX, int nY, const void *pShade);
 	DWORD		(__fastcall *pfnDrawBlockFlags)(void *pData, int nX, int nY, DWORD dwFlags);
 	DWORD		(__fastcall *pfnDrawBlockShadedArg)(void *pData, int nX, int nY,
					const void *pShade, DWORD dwArg);
};

struct DDENVINFO
{
 	BYTE		*pbBlendMode2;
 	BYTE		*pbBlendMode1;
 	BYTE		*pbBlendMode0;
 	BYTE		*apbColorMaps[32];
 	BYTE		unobserved_08c[208 - 140];
 	BYTE		*apbPreMaps[13];
 	BYTE		*pbBlendMode3;
 	BYTE		*pbBlendMode4;
 	BYTE		*pbBlendMode6;
 	BYTE		unobserved_110[280 - 272];
 	BYTE		*pbBrightColorMap;
 	BYTE		unobserved_11c[288 - 284];
};

typedef struct D2CEL D2CEL;

struct D2CELCONTEXT
{
 	D2CEL		*hCel;
};

struct D2DDTILEPART
{
 	short		sOffsetX;
 	short		sOffsetY;
 	BYTE		unobserved_04[12];
 	void		*ptData;
};

struct D2DDTILE
{
 	BYTE		unobserved_00[80];
 	int			nCompCount;
 	D2DDTILEPART *ptCompArray;
};

#include <ddraw.h>

extern BYTE *gpbSurface;
extern int gnBufferWidth;
extern int gnBufferHeight;
extern DDBLITTABLE *gptBlitTable;
extern DDDRAWCTX *gptDrawCtx;
extern DDENVINFO *gptEnvInfo;
extern HWND ghWnd;
extern PALETTEENTRY gatPaletteEntries[256];
extern DWORD gfSuspended;
extern CRITICAL_SECTION *pcsPause;
extern LPDIRECTDRAWSURFACE glpddsPrimary;
extern LPDIRECTDRAWSURFACE sglpddsRender;
struct DDVIDEOCONFIG
{ DWORD dwFlags; BYTE abGuid[16]; char szDeviceName[256]; };
extern DDVIDEOCONFIG gtVideoConfig;
extern int gnDdClipRight;
extern int gnDdClipLeft;
extern BYTE *gpbDebugFont;

void __fastcall DDrawPlayBinkFile(const char *pszFileName, int nMode,
	void (__stdcall *pfnTick)(void));

void __fastcall DdLogError(const char *pszMessage, HRESULT hResult);

void __fastcall DdCreateDebugFont(void);
void WINAPI DdFreeDebugFont(void);
void __fastcall DdDrawDebugText(int nX, int nY, const char *pszFormat, va_list vaArgs);

void WINAPI DdNothingDebugLightGrid(DWORD dwValue1, DWORD dwValue2);
DWORD WINAPI DdGetDrawCtxFlag(void);
void __fastcall DdFillSpan(int nX1, int nY1, int nX2, int nY2, BYTE bValue, int nEnvIndex);
void __fastcall DdFillSpanSelect(int nX1, int nY1, int nX2, int nY2, BYTE bValue, BYTE bSelector);
void __fastcall DdDrawLine(int nX1, int nY1, int nX2, int nY2, BYTE bColor, DWORD dwUnused);
void __fastcall DdDrawDiamond(const RECT *prc, BYTE bColor);
void __fastcall DdDrawFrame(const RECT *prc, BYTE bColor);
void __fastcall DdFillRect(RECT *prc, BYTE bColor);
void __fastcall DdFillSquare(const POINT *ptCenter, BYTE bSize, BYTE bColor);
void __fastcall DdClearRender(BOOL fAbovePanel);

struct DDSMACKVIDEO;
DWORD __fastcall DdDrawTile(D2DDTILE *ptTile, DWORD dwVertices, int nX, DWORD dwY, DWORD dwTileX,
	DWORD dwTileY, DWORD dwAlpha, int nBoundaryMode, DWORD dwFilter);
void __fastcall DdSmackOpen(const char *pszFileName, DDSMACKVIDEO *ptVideo);
void __fastcall DdSmackUpdate(DDSMACKVIDEO *ptVideo);
void __fastcall DdSmackClose(DDSMACKVIDEO *ptVideo);
BYTE * __fastcall DdGetEnvValue(int nIndex, DWORD *pdwKind);
void __fastcall DdSmackInit(BOOL fWide);
BOOL __fastcall DdDrawCelSkip(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	DWORD dwUnused, const BYTE *pbPreMap);
BOOL __fastcall DdDrawCel(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	const BYTE *pbPreMap);
BOOL __fastcall DdDrawCelPreMap(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	int nPreMap);

void __fastcall DdSpriteDrawRows(D2CELCONTEXT *ptCelContext, int nX, int nY, int nFromBottom, int nRows,
	int nMode);
void __fastcall DdSpriteDrawShadow(D2CELCONTEXT *ptCelContext, int nX, int nY);
void WINAPI DdSpriteStub(DWORD dwY, DWORD dwColor);
void __fastcall DdSpriteWarp(int nX, int nY);
BOOL __fastcall DdSpriteDrawUnitBlocks(D2DDTILE *ptTile, int nX, int nY, DWORD dwUnused,
	int nBoundaryMode);

BOOL __fastcall DdSpriteDrawWallShaded(D2DDTILE *ptTile, int nX, int nY, const BYTE *pbShadeTable,
	int nBoundaryMode, DWORD dwAlpha);

BOOL __fastcall DdSpriteDrawWallLow(D2DDTILE *ptTile, int nX, int nY, const BYTE *pbShadeTable,
	int nBoundaryMode);

#endif
