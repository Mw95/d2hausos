#define D2_THIS_FILE "C:\\D2\\Source\\D2Direct3D\\Src\\d3dTextureCache.cpp"
#include "d3dCaches.h"
#include "StormImports.h"
#include "D2Direct3DImports.h"
#include "D2Macros.h"

#define INVALID_HARDWARE_ADDRESS	0

static inline D3DTEXCACHEITEM *D3DTexUsedPopFront(D3DCACHE *tCache)
{
	D3DTEXCACHEITEM *ptItem;

	if (!tCache->pUsedHead)
		return NULL;
	ptItem = (D3DTEXCACHEITEM *)tCache->pUsedHead;
	tCache->pUsedHead = ptItem->ptNext;
	if (tCache->pUsedHead)
		((D3DTEXCACHEITEM *)tCache->pUsedHead)->ptPrev = NULL;
	else
		tCache->pUsedTail = NULL;
	ptItem->ptNext = NULL;
	ptItem->ptPrev = NULL;
	return ptItem;
}

static inline D3DTEXCACHEITEM *D3DTexFreePopFront(D3DCACHE *tCache)
{
	D3DTEXCACHEITEM *ptItem;

	if (!tCache->pFreeHead)
		return NULL;
	ptItem = (D3DTEXCACHEITEM *)tCache->pFreeHead;
	tCache->pFreeHead = ptItem->ptNext;
	if (tCache->pFreeHead)
		((D3DTEXCACHEITEM *)tCache->pFreeHead)->ptPrev = NULL;
	else
		tCache->pFreeTail = NULL;
	ptItem->ptNext = NULL;
	ptItem->ptPrev = NULL;
	return ptItem;
}

static inline void D3DTexUsedPushBack(D3DCACHE *tCache, D3DTEXCACHEITEM *ptItem)
{
	if (!tCache->pUsedHead)
		tCache->pUsedHead = ptItem;
	else
	{
		((D3DTEXCACHEITEM *)tCache->pUsedTail)->ptNext = ptItem;
		ptItem->ptPrev = (D3DTEXCACHEITEM *)tCache->pUsedTail;
	}
	tCache->pUsedTail = ptItem;
}

static inline void D3DTexFreePushBack(D3DCACHE *tCache, D3DTEXCACHEITEM *ptItem)
{
	if (!tCache->pFreeHead)
		tCache->pFreeHead = ptItem;
	else
	{
		((D3DTEXCACHEITEM *)tCache->pFreeTail)->ptNext = ptItem;
		ptItem->ptPrev = (D3DTEXCACHEITEM *)tCache->pFreeTail;
	}
	tCache->pFreeTail = ptItem;
}

static inline D3DTEXCACHEITEM *D3DTexListRemove(D3DCACHE *tCache, int nIndex)
{
	D3DTEXCACHEITEM *ptItem;

	if (!tCache->pUsedHead)
		return NULL;
	ptItem = (D3DTEXCACHEITEM *)tCache->tItems + nIndex;
	if (ptItem == (D3DTEXCACHEITEM *)tCache->pUsedHead)
		return D3DTexUsedPopFront(tCache);
	if (ptItem == (D3DTEXCACHEITEM *)tCache->pUsedTail)
	{
		tCache->pUsedTail = ((D3DTEXCACHEITEM *)tCache->pUsedTail)->ptPrev;
		((D3DTEXCACHEITEM *)tCache->pUsedTail)->ptNext = NULL;
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

static inline void D3DTexItemForget(D3DTEXCACHEITEM *ptItem)
{
	if (ptItem->ptLocations)
	{
		ptItem->ptLocations[ptItem->nSlot].nWhichCache = 0;
		ptItem->ptLocations[ptItem->nSlot].nIndex = 0;
		ptItem->ptLocations = NULL;
	}
}

static inline void D3DTexCacheEvict(D3DCACHE *tCache)
{
	D3DTEXCACHEITEM *ptItem;

	if (!tCache->nNumItems)
		return;
	ptItem = D3DTexUsedPopFront(tCache);
	if (ptItem)
	{
		D3DTexItemForget(ptItem);
		D3DTexFreePushBack(tCache, ptItem);
	}
	tCache->nNumItems--;
}

DWORD __fastcall D3DTextureCacheInit(D3DCACHE *tCache, DWORD dwBase, DWORD dwLimit, int nWidth, int nHeight)
{
	D3DTEXCACHEITEM *ptItem;
	int nItemBytes;
	int nArrayBytes;
	int i;

	nItemBytes = gnTexelBytes * nWidth * nHeight;
	tCache->nMaxNumItems = (int)(dwLimit - dwBase) / nItemBytes;
	nArrayBytes = tCache->nMaxNumItems * sizeof(D3DTEXCACHEITEM);
#line 47
	tCache->tItems = D2ALLOC2(nArrayBytes);
	memset(tCache->tItems, 0, nArrayBytes);
	for (i = 0; i <= tCache->nMaxNumItems - 1; i++)
	{
		ptItem = (D3DTEXCACHEITEM *)tCache->tItems + i;
		ptItem->nIndex = i;
		if (i < tCache->nMaxNumItems - 1)
			ptItem->ptNext = (D3DTEXCACHEITEM *)tCache->tItems + (i + 1);
		if (i > 0)
			ptItem->ptPrev = (D3DTEXCACHEITEM *)tCache->tItems + (i - 1);
		if (!D3DCreateDeviceTexture(&ptItem->lpddsDeviceSurface, &ptItem->lptexDevice, nWidth, nHeight))
		{
			if (ptItem->ptPrev)
				ptItem->ptPrev->ptNext = NULL;
			tCache->nMaxNumItems = i;
#line 82
			D2ASSERT(tCache->nMaxNumItems);
			break;
		}
	}
	tCache->nNumItems = 0;
	tCache->pUsedHead = NULL;
	tCache->pUsedTail = NULL;
	tCache->pFreeHead = tCache->tItems;
	tCache->pFreeTail = (D3DTEXCACHEITEM *)tCache->tItems + (tCache->nMaxNumItems - 1);
	return tCache->nMaxNumItems * nItemBytes + dwBase;
}

void __fastcall D3DTextureCacheFlush(D3DCACHE *tCache, BOOL fWhole)
{
	D3DTEXCACHEITEM *ptItem;
	int i;

#line 105
	D2ASSERT(tCache);
	if (!tCache->tItems)
	{
#line 108
		D2ASSERT(tCache->nNumItems == 0);
		return;
	}
	for (i = 1; i < tCache->nMaxNumItems - 1; i++)
	{
		ptItem = (D3DTEXCACHEITEM *)tCache->tItems + i;
		if (ptItem->lpddsDeviceSurface)
		{
			ptItem->lpddsDeviceSurface->lpVtbl->field_08(ptItem->lpddsDeviceSurface);
			ptItem->lpddsDeviceSurface = NULL;
		}
		if (ptItem->lptexDevice)
		{
			ptItem->lptexDevice->lpVtbl->field_08(ptItem->lptexDevice);
			ptItem->lptexDevice = NULL;
		}
	}
	if (fWhole)
	{
		while (tCache->nNumItems)
			D3DTexCacheEvict(tCache);
	}
	if (tCache->tItems)
	{
#line 141
		D2FREE2(tCache->tItems);
		tCache->tItems = NULL;
	}
	memset(tCache, 0, sizeof(D3DCACHE));
}

static inline int D3DTexCacheTake(D3DCACHE *tCache)
{
	D3DTEXCACHEITEM *tTempItem;

	if (tCache->nNumItems == tCache->nMaxNumItems)
		return 0;
	tTempItem = D3DTexFreePopFront(tCache);
#line 208
	D2ASSERT(tTempItem);
#line 209
	D2ASSERT(tTempItem->nIndex < tCache->nMaxNumItems);
	D3DTexUsedPushBack(tCache, tTempItem);
	tCache->nNumItems++;
	return tTempItem->nIndex + 1;
}

int __fastcall D3DTextureCacheAlloc(D3DCACHE *tCache)
{
	int nIndex;

	nIndex = D3DTexCacheTake(tCache);
	while (nIndex == 0)
	{
		D3DTexCacheEvict(tCache);
		nIndex = D3DTexCacheTake(tCache);
	}
	return nIndex;
}

void __fastcall D3DTextureCacheTouch(D3DCACHE *tCache, int nIndex)
{
	D3DTEXCACHEITEM *ptItem;

	if (tCache->nNumItems <= 1)
		return;
#line 238
	D2ASSERT(nIndex!= INVALID_HARDWARE_ADDRESS);
#line 239
	D2ASSERT(nIndex <= tCache->nMaxNumItems);
#line 241
	D2ASSERT(nIndex >= 1);
	ptItem = D3DTexListRemove(tCache, nIndex - 1);
	D3DTexUsedPushBack(tCache, ptItem);
}

void __fastcall D3DTextureCacheSetPalette(D3DCACHE *tCache, void *ptPalette)
{
	D3DTEXCACHEITEM *ptItem;
	HRESULT hResult;
	int i;

	if (!ptPalette)
		return;
	for (i = 0; i < tCache->nMaxNumItems - 1; i++)
	{
		ptItem = (D3DTEXCACHEITEM *)tCache->tItems + i;
		if (!ptItem->lpddsDeviceSurface)
			continue;
		hResult = ptItem->lpddsDeviceSurface->lpVtbl->pfnSetPalette(ptItem->lpddsDeviceSurface, ptPalette);
		if (hResult)
		{
			D3DLogDDError("SetPalette (texture surface)", hResult);
			return;
		}
	}
}
