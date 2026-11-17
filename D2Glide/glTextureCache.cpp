#define D2_THIS_FILE "C:\\D2\\Source\\D2Glide\\Src\\glTextureCache.cpp"
#include "glCaches.h"
#include "StormImports.h"
#include "D2GlideImports.h"
#include "D2Macros.h"

#define INVALID_HARDWARE_ADDRESS	0

static inline void GlideTextureCacheEvict(GLCACHE *tCache);
static inline int GlideTextureCacheTake(GLCACHE *tCache);

static inline GLTEXCACHEITEM *GlideTextureUsedPopFront(GLCACHE *tCache)
{
	GLTEXCACHEITEM *ptItem;

	if (!tCache->pUsedHead)
		return NULL;
	ptItem = (GLTEXCACHEITEM *)tCache->pUsedHead;
	tCache->pUsedHead = ptItem->ptNext;
	if (tCache->pUsedHead)
		((GLTEXCACHEITEM *)tCache->pUsedHead)->ptPrev = NULL;
	else
		tCache->pUsedTail = NULL;
	ptItem->ptNext = NULL;
	ptItem->ptPrev = NULL;
	return ptItem;
}

static inline GLTEXCACHEITEM *GlideTextureFreePopFront(GLCACHE *tCache)
{
	GLTEXCACHEITEM *ptItem;

	if (!tCache->pFreeHead)
		return NULL;
	ptItem = (GLTEXCACHEITEM *)tCache->pFreeHead;
	tCache->pFreeHead = ptItem->ptNext;
	if (tCache->pFreeHead)
		((GLTEXCACHEITEM *)tCache->pFreeHead)->ptPrev = NULL;
	else
		tCache->pFreeTail = NULL;
	ptItem->ptNext = NULL;
	ptItem->ptPrev = NULL;
	return ptItem;
}

static inline void GlideTextureUsedPushBack(GLCACHE *tCache, GLTEXCACHEITEM *ptItem)
{
	if (!tCache->pUsedHead)
		tCache->pUsedHead = ptItem;
	else
	{
		((GLTEXCACHEITEM *)tCache->pUsedTail)->ptNext = ptItem;
		ptItem->ptPrev = (GLTEXCACHEITEM *)tCache->pUsedTail;
	}
	tCache->pUsedTail = ptItem;
}

static inline void GlideTextureFreePushBack(GLCACHE *tCache, GLTEXCACHEITEM *ptItem)
{
	if (!tCache->pFreeHead)
		tCache->pFreeHead = ptItem;
	else
	{
		((GLTEXCACHEITEM *)tCache->pFreeTail)->ptNext = ptItem;
		ptItem->ptPrev = (GLTEXCACHEITEM *)tCache->pFreeTail;
	}
	tCache->pFreeTail = ptItem;
}

static inline GLTEXCACHEITEM *GlideTextureListRemove(GLCACHE *tCache, int nIndex)
{
	GLTEXCACHEITEM *ptItem;

	if (!tCache->pUsedHead)
		return NULL;
	ptItem = (GLTEXCACHEITEM *)tCache->tItems + nIndex;
	if (ptItem == (GLTEXCACHEITEM *)tCache->pUsedHead)
		return GlideTextureUsedPopFront(tCache);
	if (ptItem == (GLTEXCACHEITEM *)tCache->pUsedTail)
	{
		tCache->pUsedTail = ((GLTEXCACHEITEM *)tCache->pUsedTail)->ptPrev;
		((GLTEXCACHEITEM *)tCache->pUsedTail)->ptNext = NULL;
	}
	else
	{
		ptItem->ptPrev->ptNext = ptItem->ptNext;
		ptItem->ptNext->ptPrev = ptItem->ptPrev;
	}
	ptItem->ptNext = NULL;
	ptItem->ptPrev = NULL;
	return ptItem;
}

DWORD __fastcall GlideTextureCacheInit(GLCACHE *tCache, DWORD dwBase, DWORD dwLimit, DWORD dwItemBytes)
{
	GLTEXCACHEITEM *ptItem;
	DWORD dwArrayBytes;
	int i;

	tCache->dwItemBytes = dwItemBytes;
	tCache->nMaxNumItems = (dwLimit - dwBase) / dwItemBytes;
	dwArrayBytes = tCache->nMaxNumItems * sizeof(GLTEXCACHEITEM);
#line 44
	tCache->tItems = D2ALLOC2(dwArrayBytes);
	memset(tCache->tItems, 0, dwArrayBytes);
	for (i = 0; i <= tCache->nMaxNumItems - 1; i++)
	{
		ptItem = (GLTEXCACHEITEM *)tCache->tItems + i;
		ptItem->nIndex = i;
		if (i < tCache->nMaxNumItems - 1)
			ptItem->ptNext = (GLTEXCACHEITEM *)tCache->tItems + (i + 1);
		if (i > 0)
			ptItem->ptPrev = (GLTEXCACHEITEM *)tCache->tItems + (i - 1);
		ptItem->dwAddress = i * dwItemBytes + dwBase;
	}
	tCache->nNumItems = 0;
	tCache->pUsedHead = NULL;
	tCache->pUsedTail = NULL;
	tCache->pFreeHead = tCache->tItems;
	tCache->pFreeTail = (GLTEXCACHEITEM *)tCache->tItems + (tCache->nMaxNumItems - 1);
	if (!gfUnifiedMemory)
	{
		for (i = 1; i < tCache->nMaxNumItems - 1; i++)
		{
			if (((GLTEXCACHEITEM *)tCache->tItems)[i].dwAddress < GLIDE_TEXMEM_BOUNDARY
				&& tCache->dwItemBytes + ((GLTEXCACHEITEM *)tCache->tItems)[i].dwAddress > GLIDE_TEXMEM_BOUNDARY)
			{
				((GLTEXCACHEITEM *)tCache->tItems)[i - 1].ptNext = &((GLTEXCACHEITEM *)tCache->tItems)[i + 1];
				((GLTEXCACHEITEM *)tCache->tItems)[i + 1].ptPrev = &((GLTEXCACHEITEM *)tCache->tItems)[i - 1];
				((GLTEXCACHEITEM *)tCache->tItems)[i].ptPrev = NULL;
				((GLTEXCACHEITEM *)tCache->tItems)[i].ptNext = NULL;
				tCache->nNumItems++;
				break;
			}
		}
		if (((GLTEXCACHEITEM *)tCache->tItems)[tCache->nMaxNumItems - 1].dwAddress < GLIDE_TEXMEM_BOUNDARY
			&& tCache->dwItemBytes + ((GLTEXCACHEITEM *)tCache->tItems)[tCache->nMaxNumItems - 1].dwAddress
				> GLIDE_TEXMEM_BOUNDARY)
		{
			((GLTEXCACHEITEM *)tCache->tItems)[tCache->nMaxNumItems - 1].ptPrev = NULL;
			((GLTEXCACHEITEM *)tCache->tItems)[tCache->nMaxNumItems - 2].ptNext = NULL;
			tCache->pFreeTail = (GLTEXCACHEITEM *)tCache->tItems + (tCache->nMaxNumItems - 2);
			tCache->nMaxNumItems--;
		}
	}
	return tCache->nMaxNumItems * dwItemBytes + dwBase;
}

void __fastcall GlideTextureCacheFlush(GLCACHE *tCache, BOOL fWhole)
{
	if (fWhole)
	{
		while (tCache->nNumItems)
			GlideTextureCacheEvict(tCache);
	}
	if (tCache->tItems)
	{
#line 126
		D2FREE2(tCache->tItems);
		tCache->tItems = NULL;
	}
	memset(tCache, 0, sizeof(GLCACHE));
}

int __fastcall GlideTextureCacheAlloc(GLCACHE *tCache)
{
	int nIndex;

	nIndex = GlideTextureCacheTake(tCache);
	while (nIndex == INVALID_HARDWARE_ADDRESS)
	{
		GlideTextureCacheEvict(tCache);
		nIndex = GlideTextureCacheTake(tCache);
	}
	return nIndex;
}

static inline int GlideTextureCacheTake(GLCACHE *tCache)
{
	GLTEXCACHEITEM *tTempItem;

	if (tCache->nNumItems == tCache->nMaxNumItems)
		return INVALID_HARDWARE_ADDRESS;
	tTempItem = GlideTextureFreePopFront(tCache);
#line 192
	D2ASSERT(tTempItem);
	GlideTextureUsedPushBack(tCache, tTempItem);
	tCache->nNumItems++;
	return tTempItem->nIndex + 1;
}

void __fastcall GlideTextureCacheTouch(GLCACHE *tCache, int nIndex)
{
	GLTEXCACHEITEM *ptItem;

	if (tCache->nNumItems <= 1)
		return;
#line 221
	D2ASSERT(nIndex!= INVALID_HARDWARE_ADDRESS);
#line 222
	D2ASSERT(nIndex <= tCache->nMaxNumItems);
#line 223
	D2ASSERT(nIndex >= 1);
	ptItem = GlideTextureListRemove(tCache, nIndex - 1);
	GlideTextureUsedPushBack(tCache, ptItem);
}

static inline void GlideTextureCacheEvict(GLCACHE *tCache)
{
	GLTEXCACHEITEM *tTempItem;

	if (!tCache->nNumItems)
		return;
	tTempItem = GlideTextureUsedPopFront(tCache);
	if (tTempItem)
	{
		if (tTempItem->ptLocations)
		{
			tTempItem->ptLocations[tTempItem->nSlot].nWhichCache = 0;
			tTempItem->ptLocations[tTempItem->nSlot].nIndex = 0;
			tTempItem->ptLocations = NULL;
		}
		GlideTextureFreePushBack(tCache, tTempItem);
	}
	tCache->nNumItems--;
}
