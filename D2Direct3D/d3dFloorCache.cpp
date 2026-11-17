#define D2_THIS_FILE "C:\\D2\\Source\\D2Direct3D\\Src\\d3dFloorCache.cpp"
#include "d3dCaches.h"
#include "StormImports.h"
#include "D2Direct3DImports.h"
#include "D2Macros.h"

#define INVALID_HARDWARE_ADDRESS	0

static inline D3DCACHEITEM *D3DFloorUsedPopFront(D3DCACHE *tCache)
{
	D3DCACHEITEM *ptItem;

	if (!tCache->pUsedHead)
		return NULL;
	ptItem = (D3DCACHEITEM *)tCache->pUsedHead;
	tCache->pUsedHead = ptItem->ptNext;
	if (tCache->pUsedHead)
		((D3DCACHEITEM *)tCache->pUsedHead)->ptPrev = NULL;
	else
		tCache->pUsedTail = NULL;
	ptItem->ptNext = NULL;
	ptItem->ptPrev = NULL;
	return ptItem;
}

static inline D3DCACHEITEM *D3DFloorFreePopFront(D3DCACHE *tCache)
{
	D3DCACHEITEM *ptItem;

	if (!tCache->pFreeHead)
		return NULL;
	ptItem = (D3DCACHEITEM *)tCache->pFreeHead;
	tCache->pFreeHead = ptItem->ptNext;
	if (tCache->pFreeHead)
		((D3DCACHEITEM *)tCache->pFreeHead)->ptPrev = NULL;
	else
		tCache->pFreeTail = NULL;
	ptItem->ptNext = NULL;
	ptItem->ptPrev = NULL;
	return ptItem;
}

static inline void D3DFloorUsedPushBack(D3DCACHE *tCache, D3DCACHEITEM *ptItem)
{
	if (!tCache->pUsedHead)
		tCache->pUsedHead = ptItem;
	else
	{
		((D3DCACHEITEM *)tCache->pUsedTail)->ptNext = ptItem;
		ptItem->ptPrev = (D3DCACHEITEM *)tCache->pUsedTail;
	}
	tCache->pUsedTail = ptItem;
}

static inline void D3DFloorFreePushBack(D3DCACHE *tCache, D3DCACHEITEM *ptItem)
{
	if (!tCache->pFreeHead)
		tCache->pFreeHead = ptItem;
	else
	{
		((D3DCACHEITEM *)tCache->pFreeTail)->ptNext = ptItem;
		ptItem->ptPrev = (D3DCACHEITEM *)tCache->pFreeTail;
	}
	tCache->pFreeTail = ptItem;
}

static inline D3DCACHEITEM *D3DFloorListRemove(D3DCACHE *tCache, int nIndex)
{
	D3DCACHEITEM *ptItem;

	if (!tCache->pUsedHead)
		return NULL;
	ptItem = (D3DCACHEITEM *)tCache->tItems + nIndex;
	if (ptItem == (D3DCACHEITEM *)tCache->pUsedHead)
		return D3DFloorUsedPopFront(tCache);
	if (ptItem == (D3DCACHEITEM *)tCache->pUsedTail)
	{
		tCache->pUsedTail = ((D3DCACHEITEM *)tCache->pUsedTail)->ptPrev;
		((D3DCACHEITEM *)tCache->pUsedTail)->ptNext = NULL;
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

DWORD __fastcall D3DFloorCacheInit(D3DCACHE *tCache, DWORD dwBase, DWORD dwLimit, DWORD dwItemBytes)
{
	D3DCACHEITEM *ptItem;
	DWORD dwArrayBytes;
	int i;

	tCache->nMaxNumItems = (dwLimit - dwBase) / dwItemBytes;
	dwArrayBytes = tCache->nMaxNumItems * sizeof(D3DCACHEITEM);
#line 41
	tCache->tItems = D2ALLOC2(dwArrayBytes);
#line 42
	D2ASSERT(tCache->tItems);
	memset(tCache->tItems, 0, dwArrayBytes);
	for (i = 0; i <= tCache->nMaxNumItems - 1; i++)
	{
		ptItem = (D3DCACHEITEM *)tCache->tItems + i;
		ptItem->nIndex = i;
		if (i < tCache->nMaxNumItems - 1)
			ptItem->ptNext = (D3DCACHEITEM *)tCache->tItems + (i + 1);
		if (i > 0)
			ptItem->ptPrev = (D3DCACHEITEM *)tCache->tItems + (i - 1);
		if (!D3DCreateDeviceTexture(&ptItem->lpddsDeviceSurface, &ptItem->lptexDevice, gnFloorCells,
			gnFloorRows))
		{
			if (ptItem->ptPrev)
				ptItem->ptPrev->ptNext = NULL;
			tCache->nMaxNumItems = i;
#line 69
			D2ASSERT(tCache->nMaxNumItems);
			break;
		}
	}
	tCache->nNumItems = 0;
	tCache->pUsedHead = NULL;
	tCache->pUsedTail = NULL;
	tCache->pFreeHead = tCache->tItems;
	tCache->pFreeTail = (D3DCACHEITEM *)tCache->tItems + (tCache->nMaxNumItems - 1);
	return tCache->nMaxNumItems * dwItemBytes + dwBase;
}

static inline int D3DFloorCacheTake(D3DCACHE *tCache)
{
	D3DCACHEITEM *ptItem;

	if (tCache->nNumItems == tCache->nMaxNumItems)
		return 0;
	ptItem = D3DFloorFreePopFront(tCache);
	D3DFloorUsedPushBack(tCache, ptItem);
	tCache->nNumItems++;
	return ptItem->nIndex + 1;
}

static inline void D3DFloorCacheEvict(D3DCACHE *tCache)
{
	D3DCACHEITEM *ptItem;

	if (!tCache->nNumItems)
		return;
	ptItem = D3DFloorUsedPopFront(tCache);
	if (ptItem)
	{
		if (ptItem->pOwner)
		{
			((D3DTILE *)ptItem->pOwner)->sCacheIndex = 0;
			ptItem->pOwner = NULL;
		}
		D3DFloorFreePushBack(tCache, ptItem);
	}
	tCache->nNumItems--;
}

void __fastcall D3DFloorCacheFlush(D3DCACHE *tCache, BOOL fWhole)
{
	D3DCACHEITEM *ptItem;
	int i;

#line 92
	D2ASSERT(tCache);
	if (!tCache->tItems)
	{
#line 95
		D2ASSERT(tCache->nNumItems == 0);
		return;
	}
	for (i = 1; i < tCache->nMaxNumItems - 1; i++)
	{
		ptItem = (D3DCACHEITEM *)tCache->tItems + i;
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
			D3DFloorCacheEvict(tCache);
	}
	if (tCache->tItems)
	{
#line 127
		D2FREE2(tCache->tItems);
		tCache->tItems = NULL;
	}
	memset(tCache, 0, sizeof(D3DCACHE));
}

int __fastcall D3DFloorCacheAlloc(D3DCACHE *tCache)
{
	int nIndex;

	nIndex = D3DFloorCacheTake(tCache);
	while (nIndex == 0)
	{
		D3DFloorCacheEvict(tCache);
		nIndex = D3DFloorCacheTake(tCache);
	}
	return nIndex;
}
void __fastcall D3DFloorCacheTouch(D3DCACHE *tCache, int nIndex)
{
	D3DCACHEITEM *ptItem;
	if (tCache->nNumItems <= 1)
		return;
#line 195
	D2ASSERT(nIndex!= INVALID_HARDWARE_ADDRESS);
#line 196
	D2ASSERT(nIndex <= tCache->nMaxNumItems);
#line 197
	D2ASSERT(nIndex >= 1);
	ptItem = D3DFloorListRemove(tCache, nIndex - 1);
	D3DFloorUsedPushBack(tCache, ptItem);
}

void __fastcall D3DFloorCacheFree(D3DCACHE *tCache, int nIndex)
{
	D3DCACHEITEM *ptItem;

	if (!tCache->nNumItems)
		return;
	ptItem = D3DFloorListRemove(tCache, nIndex);
	if (ptItem)
	{
		if (ptItem->pOwner)
		{
			((D3DTILE *)ptItem->pOwner)->sCacheIndex = 0;
			ptItem->pOwner = NULL;
		}
		D3DFloorFreePushBack(tCache, ptItem);
	}
	tCache->nNumItems--;
}

void __fastcall D3DFloorCacheSetPalette(D3DCACHE *tCache, void *ptPalette)
{
	D3DCACHEITEM *ptItem;
	HRESULT hResult;
	int i;

	if (!ptPalette)
		return;
	for (i = 0; i < tCache->nMaxNumItems - 1; i++)
	{
		ptItem = (D3DCACHEITEM *)tCache->tItems + i;
		if (!ptItem->lpddsDeviceSurface)
			continue;
		hResult = ptItem->lpddsDeviceSurface->lpVtbl->pfnSetPalette(ptItem->lpddsDeviceSurface, ptPalette);
		if (hResult)
		{
			D3DLogDDError("SetPalette (floor surface)", hResult);
			return;
		}
	}
}
