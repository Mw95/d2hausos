#define D2_THIS_FILE "C:\\D2\\Source\\D2Glide\\Src\\glFloorCache.cpp"
#include "glCaches.h"
#include "StormImports.h"
#include "D2GlideImports.h"
#include "D2Macros.h"

#define INVALID_HARDWARE_ADDRESS	0

static inline void GlideFloorCacheEvict(GLCACHE *tCache);
static inline int GlideFloorCacheTake(GLCACHE *tCache);

static inline GLCACHEITEM *GlideFloorUsedPopFront(GLCACHE *tCache)
{
	GLCACHEITEM *ptItem;

	if (!tCache->pUsedHead)
		return NULL;
	ptItem = (GLCACHEITEM *)tCache->pUsedHead;
	tCache->pUsedHead = ptItem->ptNext;
	if (tCache->pUsedHead)
		((GLCACHEITEM *)tCache->pUsedHead)->ptPrev = NULL;
	else
		tCache->pUsedTail = NULL;
	ptItem->ptNext = NULL;
	ptItem->ptPrev = NULL;
	return ptItem;
}

static inline GLCACHEITEM *GlideFloorFreePopFront(GLCACHE *tCache)
{
	GLCACHEITEM *ptItem;

	if (!tCache->pFreeHead)
		return NULL;
	ptItem = (GLCACHEITEM *)tCache->pFreeHead;
	tCache->pFreeHead = ptItem->ptNext;
	if (tCache->pFreeHead)
		((GLCACHEITEM *)tCache->pFreeHead)->ptPrev = NULL;
	else
		tCache->pFreeTail = NULL;
	ptItem->ptNext = NULL;
	ptItem->ptPrev = NULL;
	return ptItem;
}

static inline void GlideFloorUsedPushBack(GLCACHE *tCache, GLCACHEITEM *ptItem)
{
	if (!tCache->pUsedHead)
		tCache->pUsedHead = ptItem;
	else
	{
		((GLCACHEITEM *)tCache->pUsedTail)->ptNext = ptItem;
		ptItem->ptPrev = (GLCACHEITEM *)tCache->pUsedTail;
	}
	tCache->pUsedTail = ptItem;
}

static inline void GlideFloorFreePushBack(GLCACHE *tCache, GLCACHEITEM *ptItem)
{
	if (!tCache->pFreeHead)
		tCache->pFreeHead = ptItem;
	else
	{
		((GLCACHEITEM *)tCache->pFreeTail)->ptNext = ptItem;
		ptItem->ptPrev = (GLCACHEITEM *)tCache->pFreeTail;
	}
	tCache->pFreeTail = ptItem;
}

static inline GLCACHEITEM *GlideFloorListRemove(GLCACHE *tCache, int nIndex)
{
	GLCACHEITEM *ptItem;

	if (!tCache->pUsedHead)
		return NULL;
	ptItem = (GLCACHEITEM *)tCache->tItems + nIndex;
	if (ptItem == (GLCACHEITEM *)tCache->pUsedHead)
		return GlideFloorUsedPopFront(tCache);
	if (ptItem == (GLCACHEITEM *)tCache->pUsedTail)
	{
		tCache->pUsedTail = ((GLCACHEITEM *)tCache->pUsedTail)->ptPrev;
		((GLCACHEITEM *)tCache->pUsedTail)->ptNext = NULL;
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

DWORD __fastcall GlideFloorCacheInit(GLCACHE *tCache, DWORD dwBase, DWORD dwLimit, DWORD dwItemBytes)
{
	GLCACHEITEM *ptItem;
	DWORD dwArrayBytes;
	int i;

	tCache->dwItemBytes = dwItemBytes;
	tCache->nMaxNumItems = (dwLimit - dwBase) / dwItemBytes;
	dwArrayBytes = tCache->nMaxNumItems * sizeof(GLCACHEITEM);
#line 41
	tCache->tItems = D2ALLOC2(dwArrayBytes);
#line 42
	D2ASSERT(tCache->tItems);
	memset(tCache->tItems, 0, dwArrayBytes);
	for (i = 0; i <= tCache->nMaxNumItems - 1; i++)
	{
		ptItem = (GLCACHEITEM *)tCache->tItems + i;
		ptItem->nIndex = i;
		if (i < tCache->nMaxNumItems - 1)
			ptItem->ptNext = (GLCACHEITEM *)tCache->tItems + (i + 1);
		if (i > 0)
			ptItem->ptPrev = (GLCACHEITEM *)tCache->tItems + (i - 1);
		ptItem->dwAddress = i * dwItemBytes + dwBase;
	}
	tCache->pFreeHead = tCache->tItems;
	tCache->nNumItems = 0;
	tCache->pUsedHead = NULL;
	tCache->pUsedTail = NULL;
	tCache->pFreeTail = (GLCACHEITEM *)tCache->tItems + (tCache->nMaxNumItems - 1);
	if (!gfUnifiedMemory)
	{
		for (i = 1; i < tCache->nMaxNumItems - 1; i++)
		{
			if (((GLCACHEITEM *)tCache->tItems)[i].dwAddress < GLIDE_TEXMEM_BOUNDARY
				&& tCache->dwItemBytes + ((GLCACHEITEM *)tCache->tItems)[i].dwAddress > GLIDE_TEXMEM_BOUNDARY)
			{
				((GLCACHEITEM *)tCache->tItems)[i - 1].ptNext = &((GLCACHEITEM *)tCache->tItems)[i + 1];
				((GLCACHEITEM *)tCache->tItems)[i + 1].ptPrev = &((GLCACHEITEM *)tCache->tItems)[i - 1];
				((GLCACHEITEM *)tCache->tItems)[i].ptPrev = NULL;
				((GLCACHEITEM *)tCache->tItems)[i].ptNext = NULL;
				tCache->nNumItems++;
				break;
			}
		}
		if (((GLCACHEITEM *)tCache->tItems)[tCache->nMaxNumItems - 1].dwAddress < GLIDE_TEXMEM_BOUNDARY
			&& tCache->dwItemBytes + ((GLCACHEITEM *)tCache->tItems)[tCache->nMaxNumItems - 1].dwAddress
				> GLIDE_TEXMEM_BOUNDARY)
		{
			((GLCACHEITEM *)tCache->tItems)[tCache->nMaxNumItems - 1].ptPrev = NULL;
			((GLCACHEITEM *)tCache->tItems)[tCache->nMaxNumItems - 2].ptNext = NULL;
			tCache->pFreeTail = (GLCACHEITEM *)tCache->tItems + (tCache->nMaxNumItems - 2);
			tCache->nMaxNumItems--;
		}
	}
	return tCache->nMaxNumItems * dwItemBytes + dwBase;
}

void __fastcall GlideFloorCacheFlush(GLCACHE *tCache, BOOL fWhole)
{
	if (fWhole)
	{
		while (tCache->nNumItems)
			GlideFloorCacheEvict(tCache);
	}
	if (tCache->tItems)
	{
#line 116
		D2FREE2(tCache->tItems);
		tCache->tItems = NULL;
	}
	memset(tCache, 0, sizeof(GLCACHE));
}

int __fastcall GlideFloorCacheAlloc(GLCACHE *tCache)
{
	int nIndex;

	nIndex = GlideFloorCacheTake(tCache);
	if (nIndex == INVALID_HARDWARE_ADDRESS)
	{
		GlideFloorCacheEvict(tCache);
		nIndex = GlideFloorCacheTake(tCache);
#line 139
		D2ASSERT(nIndex != INVALID_HARDWARE_ADDRESS);
	}
	return nIndex;
}

static inline int GlideFloorCacheTake(GLCACHE *tCache)
{
	GLCACHEITEM *tTempItem;

	if (tCache->nNumItems == tCache->nMaxNumItems)
		return INVALID_HARDWARE_ADDRESS;
	tTempItem = GlideFloorFreePopFront(tCache);
#line 159
	D2ASSERT(tTempItem);
	GlideFloorUsedPushBack(tCache, tTempItem);
	tCache->nNumItems++;
	return tTempItem->nIndex + 1;
}

void __fastcall GlideFloorCacheTouch(GLCACHE *tCache, int nIndex)
{
	GLCACHEITEM *ptItem;

	if (tCache->nNumItems <= 1)
		return;
#line 188
	D2ASSERT(nIndex!= INVALID_HARDWARE_ADDRESS);
#line 189
	D2ASSERT(nIndex <= tCache->nMaxNumItems);
#line 190
	D2ASSERT(nIndex >= 1);
	ptItem = GlideFloorListRemove(tCache, nIndex - 1);
	GlideFloorUsedPushBack(tCache, ptItem);
}

static inline void GlideFloorCacheEvict(GLCACHE *tCache)
{
	GLCACHEITEM *tTempItem;

	if (!tCache->nNumItems)
		return;
	tTempItem = GlideFloorUsedPopFront(tCache);
	if (tTempItem)
	{
#line 211
		D2ASSERT(tTempItem->pTile);
		((GLTILE *)tTempItem->pTile)->sCacheIndex = 0;
		tTempItem->pTile = NULL;
		GlideFloorFreePushBack(tCache, tTempItem);
	}
	tCache->nNumItems--;
}

void __fastcall GlideFloorCacheFree(GLCACHE *tCache, int nIndex)
{
	GLCACHEITEM *tTempItem;

	if (!tCache->nNumItems)
		return;
	tTempItem = GlideFloorListRemove(tCache, nIndex);
	if (tTempItem)
	{
#line 241
		D2ASSERT(tTempItem->pTile);
		((GLTILE *)tTempItem->pTile)->sCacheIndex = 0;
		tTempItem->pTile = NULL;
		GlideFloorFreePushBack(tCache, tTempItem);
	}
	tCache->nNumItems--;
}
