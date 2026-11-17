#ifndef D2RE_D2GLIDE_GLCORE_H
#define D2RE_D2GLIDE_GLCORE_H

#include <windows.h>
#include <stdarg.h>
#include "glide3x.h"

struct GLTILE;
struct GLTILECOMP;

struct GLDRAWCTX
{
 	DWORD		dwPerspective;
 	DWORD		dwPerspectiveAllowed;
 	DWORD		dwFlatLighting;
 	DWORD		unobserved_00c;
 	DWORD		dwVSync;
 	DWORD		dwBlendShadow;
};

struct GLBLITTABLE
{
 	void		*unobserved_000[7];
};

struct GLENVINFO
{
 	DWORD		field_000[69];
 	const BYTE	*pbPalette;
 	const BYTE	*pbColorMap;
 	DWORD		field_11c;
};

struct GLTEXINFO
{
 	DWORD		field_00;
 	DWORD		field_04;
 	DWORD		dwAspectLog2;
 	DWORD		field_0c;
 	BYTE		*pbPixels;
};

struct GLVERTEX
{
 	float		flX;
 	float		flY;
 	DWORD		dwColor;
 	float		field_0c;
 	float		flS;
 	float		flT;
 	DWORD		unobserved_018;
};

struct GLLFBINFO
{
 	DWORD		field_00;
 	BYTE		*pbBuffer;
 	int			nPitch;
 	DWORD		field_0c;
 	DWORD		field_10;
};

extern GLDRAWCTX *gptDrawCtx;
extern int gnScreenWidth;
extern int gnScreenHeight;
extern HWND ghWnd;
extern BOOL gfSuspended;
extern CRITICAL_SECTION *pcsPause;
extern int gnTmuCount;
extern GLENVINFO *gptEnvInfo;
extern BOOL gfUnifiedMemory;
BOOL __fastcall GlideCloseWindow(void);
void __fastcall GlideSetTexCombine(DWORD nMode);
void __fastcall GlideSetColorCombine(int nMode);
void __fastcall GlideSetAlphaMode(int nMode);
void __fastcall GlideSetFilterMode(int nMode);
DWORD __fastcall GlidePaletteColor(int nIndex);

void __fastcall GlidePlayBinkFile(const char *pszFileName, int nMode, void (__stdcall *pfnTick)(void));
void __fastcall GlideBlocksOpen(void);
void __fastcall GlideBlocksClose(void);
BOOL __fastcall GlideBindTile(GLTILE *pTile);
void __fastcall GlideBindTileComp(GLTILECOMP *ptTileComp);
void __fastcall GlideTileFreed(GLTILE *pTile);
DWORD __fastcall GlideDownloadDebugFont(DWORD dwBase, DWORD dwAlign);
void __fastcall GlideCreateDebugFont(void);
void __fastcall GlideFreeDebugFont(void);
DWORD __fastcall GlideGetDrawCtxFlag(void);
void __fastcall GlideFloorGridInit(void);
void __fastcall GlidePerspectiveOpen(void);
void __fastcall GlidePerspectiveClose(void);
void __fastcall GlidePerspectiveInit(void);
DWORD __fastcall GlideGetPerspective(void);
extern int gnPerspDepth;
void __fastcall GlidePerspectiveProject(int nX, int nY, int nZ, int *pnX, int *pnY);
void __fastcall GlideSmackStartup(void);
DWORD __fastcall GlideSmackCreateBuffer(DWORD dwBase, DWORD dwAlign);
void __fastcall GlideSmackShutdown(void);
void __fastcall GlideSmackFreeBuffer(void);
void __fastcall GlideSpriteOpen(void);
void __fastcall GlideSpriteClose(void);
extern BYTE *gpbLightTable;
extern BYTE gbVertexRed;
extern BYTE gbVertexGreen;
extern BYTE gbVertexBlue;
void __fastcall GlideVertexOpen(void);
void __fastcall GlideVertexClose(void);
void __fastcall GlideBuildLightTable(void);
void __fastcall GlideSetVertexColor(int bRed, int bGreen, int bBlue);
void __fastcall GlideWallStartup(void);
void __fastcall GlideSetVertexLightRGB(GLVERTEX *ptVertex, BYTE bLight);
void __fastcall GlideSetVertexLight(GLVERTEX *ptVertex, DWORD dwColor);

#define GL_FLOAT_STORE(f)	((void)0)

#endif
