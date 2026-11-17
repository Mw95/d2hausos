#define D2_THIS_FILE "C:\\D2\\Source\\D2Glide\\Src\\glSubtileCache.cpp"
#include "glCaches.h"
#include "StormImports.h"
#include "D2GlideImports.h"
#include "D2Macros.h"

#define INVALID_HARDWARE_ADDRESS	0

static inline void GlideSubtileCacheEvict(GLCACHE *tCache);
static inline int GlideSubtileCacheTake(GLCACHE *tCache);
static GLCACHEITEM *__fastcall GlideSubtileListRemove(GLCACHE *tCache, int nIndex);

static inline GLCACHEITEM *GlideSubtileUsedPopFront(GLCACHE *tCache)
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

static inline GLCACHEITEM *GlideSubtileFreePopFront(GLCACHE *tCache)
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

static inline void GlideSubtileUsedPushBack(GLCACHE *tCache, GLCACHEITEM *ptItem)
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

static inline void GlideSubtileFreePushBack(GLCACHE *tCache, GLCACHEITEM *ptItem)
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

DWORD __fastcall GlideSubtileCacheInit(GLCACHE *tCache, DWORD dwBase, DWORD dwLimit, DWORD dwItemBytes)
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

void __fastcall GlideSubtileCacheFlush(GLCACHE *tCache, BOOL fWhole)
{
	if (fWhole)
	{
		while (tCache->nNumItems)
			GlideSubtileCacheEvict(tCache);
	}
	if (tCache->tItems)
	{
#line 116
		D2FREE2(tCache->tItems);
		tCache->tItems = NULL;
	}
	memset(tCache, 0, sizeof(GLCACHE));
}

int __fastcall GlideSubtileCacheAlloc(GLCACHE *tCache)
{
	int nIndex;

	nIndex = GlideSubtileCacheTake(tCache);
	while (nIndex == INVALID_HARDWARE_ADDRESS)
	{
		GlideSubtileCacheEvict(tCache);
		nIndex = GlideSubtileCacheTake(tCache);
	}
	return nIndex;
}

static inline int GlideSubtileCacheTake(GLCACHE *tCache)
{
	GLCACHEITEM *tTempItem;

	if (tCache->nNumItems == tCache->nMaxNumItems)
		return INVALID_HARDWARE_ADDRESS;
	tTempItem = GlideSubtileFreePopFront(tCache);
#line 157
	D2ASSERT(tTempItem);
	GlideSubtileUsedPushBack(tCache, tTempItem);
	tCache->nNumItems++;
	return tTempItem->nIndex + 1;
}

void __fastcall GlideSubtileCacheTouch(GLCACHE *tCache, int nIndex)
{
	GLCACHEITEM *ptItem;

	if (tCache->nNumItems <= 1)
		return;
#line 186
	D2ASSERT(nIndex!= INVALID_HARDWARE_ADDRESS);
#line 187
	D2ASSERT(nIndex <= tCache->nMaxNumItems);
#line 188
	D2ASSERT(nIndex >= 1);
	ptItem = GlideSubtileListRemove(tCache, nIndex - 1);
	GlideSubtileUsedPushBack(tCache, ptItem);
}

static inline void GlideSubtileCacheEvict(GLCACHE *tCache)
{
	GLCACHEITEM *tTempItem;

	if (!tCache->nNumItems)
		return;
	tTempItem = GlideSubtileUsedPopFront(tCache);
	if (tTempItem)
	{
		if (tTempItem->pTile)
		{
			((GLTILECOMP *)tTempItem->pTile)->sCacheIndex = 0;
			tTempItem->pTile = NULL;
		}
		GlideSubtileFreePushBack(tCache, tTempItem);
	}
	tCache->nNumItems--;
}

void __fastcall GlideSubtileCacheFree(GLCACHE *tCache, int nIndex)
{
	GLCACHEITEM *tTempItem;

	if (!tCache->nNumItems)
		return;
	tTempItem = GlideSubtileListRemove(tCache, nIndex);
	if (tTempItem)
	{
		if (tTempItem->pTile)
		{
			((GLTILECOMP *)tTempItem->pTile)->sCacheIndex = 0;
			tTempItem->pTile = NULL;
		}
		GlideSubtileFreePushBack(tCache, tTempItem);
	}
	tCache->nNumItems--;
}

static GLCACHEITEM *__fastcall GlideSubtileListRemove(GLCACHE *tCache, int nIndex)
{
	GLCACHEITEM *ptItem;
	GLCACHEITEM *ptHead;

	ptHead = (GLCACHEITEM *)tCache->pUsedHead;
	if (!ptHead)
		return NULL;
#line 381
	D2ASSERT(tCache->tItems);
	ptItem = (GLCACHEITEM *)tCache->tItems + nIndex;
	if (ptItem == ptHead)
	{
		tCache->pUsedHead = ptHead->ptNext;
		if (tCache->pUsedHead)
			((GLCACHEITEM *)tCache->pUsedHead)->ptPrev = NULL;
		else
			tCache->pUsedTail = NULL;
		ptHead->ptNext = NULL;
		ptHead->ptPrev = NULL;
		return ptHead;
	}
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
