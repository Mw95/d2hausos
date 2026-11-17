#ifndef D2RE_D2DIRECT3D_D3DCACHES_H
#define D2RE_D2DIRECT3D_D3DCACHES_H

#include "d3dCore.h"
struct D3DCACHE
{
 	int			nMaxNumItems;
 	int			nNumItems;
 	void		*pUsedHead;
 	void		*pUsedTail;
 	void		*pFreeHead;
 	void		*pFreeTail;
 	void		*tItems;
};

struct D3DCACHEITEM
{
 	int			nIndex;
 	void		*pOwner;
 	D3DSURFACE	*lpddsDeviceSurface;
 	D3DTEXTURE	*lptexDevice;
 	D3DCACHEITEM *ptNext;
 	D3DCACHEITEM *ptPrev;
};

struct D3DTEXLOCATION
{
 	int			nWhichCache;
 	int			nIndex;
};

struct D3DTEXCACHEITEM
{
 	int			nIndex;
 	D3DTEXLOCATION *ptLocations;
 	int			nSlot;
 	const BYTE	*pbPreMap;
 	D3DSURFACE	*lpddsDeviceSurface;
 	D3DTEXTURE	*lptexDevice;
 	D3DTEXCACHEITEM *ptNext;
 	D3DTEXCACHEITEM *ptPrev;
};

struct D3DTILECOMP
{
 	short		sOffsetX;
 	short		sOffsetY;
 	short		sCacheIndex;
 	BYTE		unobserved_06[2];
 	WORD		wFlags;
 	BYTE		unobserved_0a[6];
 	BYTE		*ptData;
};

struct D3DTILE
{
        int                     nType;
 	BYTE		unobserved_04[66 - 4];
 	short		sCacheIndex;
 	BYTE		unobserved_44[80 - 68];
 	int			nCompCount;
 	D3DTILECOMP	*ptCompArray;
};

extern D3DCACHE gtFloorCache;
DWORD __fastcall D3DFloorCacheInit(D3DCACHE *tCache, DWORD dwBase, DWORD dwLimit,
	DWORD dwItemBytes);
void __fastcall D3DFloorCacheFlush(D3DCACHE *tCache, BOOL fWhole);
int __fastcall D3DFloorCacheAlloc(D3DCACHE *tCache);
void __fastcall D3DFloorCacheTouch(D3DCACHE *tCache, int nIndex);
void __fastcall D3DFloorCacheFree(D3DCACHE *tCache, int nIndex);

extern D3DCACHE gtSubtileCache;
DWORD __fastcall D3DSubtileCacheInit(D3DCACHE *tCache, DWORD dwBase, DWORD dwLimit,
	DWORD dwItemBytes);
void __fastcall D3DSubtileCacheFlush(D3DCACHE *tCache, BOOL fWhole);
int __fastcall D3DSubtileCacheAlloc(D3DCACHE *tCache);
void __fastcall D3DSubtileCacheTouch(D3DCACHE *tCache, int nIndex);
void __fastcall D3DSubtileCacheFree(D3DCACHE *tCache, int nIndex);

extern D3DCACHE gatTextureCache[3];
DWORD __fastcall D3DTextureCacheInit(D3DCACHE *tCache, DWORD dwBase, DWORD dwLimit, int nWidth,
	int nHeight);
void __fastcall D3DTextureCacheFlush(D3DCACHE *tCache, BOOL fWhole);
int __fastcall D3DTextureCacheAlloc(D3DCACHE *tCache);
void __fastcall D3DTextureCacheTouch(D3DCACHE *tCache, int nIndex);
void WINAPI D3DBuildLightTable(void);
extern BYTE gbVertexRed;
extern BYTE gbVertexGreen;
extern BYTE gbVertexBlue;

void WINAPI D3DBlocksOpen(void);
void WINAPI D3DBlocksClose(void);
void __fastcall D3DTileFreed(D3DTILE *pTile);
BOOL __fastcall D3DBindTile(D3DTILE *pTile);
void __fastcall D3DBindTileComp(D3DTILECOMP *ptTileComp);
DWORD __fastcall D3DCreateDebugFont(DWORD dwBase);
void WINAPI D3DFreeDebugFont(void);
DWORD WINAPI D3DGetDrawCtxFlag(void);
void __fastcall D3DEngineClear(BOOL fAbovePanel);
void WINAPI D3DEngineOpen(void);
void WINAPI D3DEngineClose(void);
extern int gnPerspDepth;
void WINAPI D3DPerspectiveInit(void);
DWORD WINAPI D3DGetPerspective(void);
void __fastcall D3DPerspectiveProject(int nX, int nY, int nZ, int *pnX, int *pnY);
void WINAPI D3DSmackStartup(void);
DWORD __fastcall D3DSmackCreateBuffer(DWORD dwBase);
void WINAPI D3DSmackShutdown(void);
void WINAPI D3DSmackFreeBuffer(void);
void __fastcall D3DSpriteOpen(void);
void WINAPI D3DSpriteClose(void);
void WINAPI D3DWallStartup(void);
void __fastcall D3DSetVertexColor(int bRed, int bGreen, int bBlue);
void __fastcall D3DSetVertexLightRGB(D3DTLVERTEX *ptVertex, BYTE bLight, BYTE bAlpha);
void __fastcall D3DSetVertexLight(D3DTLVERTEX *ptVertex, DWORD dwColor, BYTE bAlpha);

#endif
