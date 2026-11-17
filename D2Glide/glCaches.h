#ifndef D2RE_D2GLIDE_GLCACHES_H
#define D2RE_D2GLIDE_GLCACHES_H

#include <windows.h>

#define GLIDE_TEXMEM_BOUNDARY	2097152

struct GLTILECOMP
{
 	short		sOffsetX;
 	short		sOffsetY;
 	short		sCacheIndex;
 	BYTE		unobserved_06[2];
 	WORD		wFlags;
 	BYTE		unobserved_0a[6];
 	BYTE		*ptData;
};

struct GLTILE
{
 	int			nType;
 	BYTE		unobserved_04[66 - 4];
 	short		sCacheIndex;
 	BYTE		unobserved_44[80 - 68];
 	int			nCompCount;
 	GLTILECOMP	*ptCompArray;
};

struct GLCACHEITEM
{
 	int			nIndex;
 	DWORD		dwAddress;
 	void		*pTile;
 	GLCACHEITEM	*ptNext;
 	GLCACHEITEM	*ptPrev;
};

struct GLTEXLOCATION
{
 	int			nWhichCache;
 	int			nIndex;
};

struct GLTEXCACHEITEM
{
 	int			nIndex;
 	DWORD		dwAddress;
 	GLTEXLOCATION *ptLocations;
 	int			nSlot;
 	const BYTE	*pbPreMap;
 	GLTEXCACHEITEM *ptNext;
 	GLTEXCACHEITEM *ptPrev;
};

struct GLCACHE
{
 	DWORD		dwItemBytes;
 	int			nMaxNumItems;
 	int			nNumItems;
 	void		*pUsedHead;
 	void		*pUsedTail;
 	void		*pFreeHead;
 	void		*pFreeTail;
 	void		*tItems;
};

extern BOOL gfUnifiedMemory;

extern GLCACHE gtFloorCache;
DWORD __fastcall GlideFloorCacheInit(GLCACHE *tCache, DWORD dwBase, DWORD dwLimit,
	DWORD dwItemBytes);
void __fastcall GlideFloorCacheFlush(GLCACHE *tCache, BOOL fWhole);
int __fastcall GlideFloorCacheAlloc(GLCACHE *tCache);
void __fastcall GlideFloorCacheTouch(GLCACHE *tCache, int nIndex);
void __fastcall GlideFloorCacheFree(GLCACHE *tCache, int nIndex);

extern GLCACHE gtSubtileCache;
DWORD __fastcall GlideSubtileCacheInit(GLCACHE *tCache, DWORD dwBase, DWORD dwLimit,
	DWORD dwItemBytes);
void __fastcall GlideSubtileCacheFlush(GLCACHE *tCache, BOOL fWhole);
int __fastcall GlideSubtileCacheAlloc(GLCACHE *tCache);
void __fastcall GlideSubtileCacheTouch(GLCACHE *tCache, int nIndex);
void __fastcall GlideSubtileCacheFree(GLCACHE *tCache, int nIndex);

extern GLCACHE gatTextureCache[3];
DWORD __fastcall GlideTextureCacheInit(GLCACHE *tCache, DWORD dwBase, DWORD dwLimit,
	DWORD dwItemBytes);
void __fastcall GlideTextureCacheFlush(GLCACHE *tCache, BOOL fWhole);
int __fastcall GlideTextureCacheAlloc(GLCACHE *tCache);
void __fastcall GlideTextureCacheTouch(GLCACHE *tCache, int nIndex);

#endif
