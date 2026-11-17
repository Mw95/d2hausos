#define D2_THIS_FILE "C:\\D2\\Source\\D2Direct3D\\Src\\d3dBlocks.cpp"
#include "d3dCaches.h"
#include "StormImports.h"
#include "D2Direct3DImports.h"
#include "D2Macros.h"

#define INVALID_HARDWARE_ADDRESS	0
#define DDSD_PITCH					8

static BYTE sgabSubtileBuffer[32 * 32] = { 0 };
static D3DSURFACE *sglpddsFloorMemory = 0;
static D3DTEXTURE *sglptexFloorMemory = 0;
static D3DSURFACE *sglpddsSubtileMemory = 0;
static D3DTEXTURE *sglptexSubtileMemory = 0;

void WINAPI D3DBlocksOpen(void)
{
	BOOL success;

	success = D3DCreateMemoryTexture(&sglpddsFloorMemory, &sglptexFloorMemory, gnFloorCells, gnFloorRows);
#line 39
	D2ASSERT(success);
	success = D3DCreateMemoryTexture(&sglpddsSubtileMemory, &sglptexSubtileMemory, 32, 32);
#line 42
	D2ASSERT(success);
}

void WINAPI D3DBlocksClose(void)
{
	if (sglpddsFloorMemory)
	{
		sglpddsFloorMemory->lpVtbl->field_08(sglpddsFloorMemory);
		sglpddsFloorMemory = NULL;
	}
	if (sglptexFloorMemory)
	{
		sglptexFloorMemory->lpVtbl->field_08(sglptexFloorMemory);
		sglptexFloorMemory = NULL;
	}
	if (sglpddsSubtileMemory)
	{
		sglpddsSubtileMemory->lpVtbl->field_08(sglpddsSubtileMemory);
		sglpddsSubtileMemory = NULL;
	}
	if (sglptexSubtileMemory)
	{
		sglptexSubtileMemory->lpVtbl->field_08(sglptexSubtileMemory);
		sglptexSubtileMemory = NULL;
	}
}

BOOL __fastcall D3DBindTile(D3DTILE *pTile)
{
	D3DSURFDESC ddsd;
	D3DCACHEITEM *ptItem;
	D3DTILECOMP *ptTileComp;
	HRESULT hResult;
	const BYTE *pbData;
	const BYTE *pbSrc;
	WORD *pwSurface;
	WORD *pwRow;
	WORD *pwPixel;
	WORD *pwTile;
	int nPitch;
	int nRows;
	int nWidth;
	int i;
	int x;
	BYTE bSkip;
	BYTE bRun;
	BOOL fResult;

	fResult = FALSE;
	if (!(!D2CMPTileCacheLoadTile(pTile, 1)))
	{
#line 397
		D2ASSERT(pTile->sCacheIndex >= INVALID_HARDWARE_ADDRESS);
		if (pTile->sCacheIndex == 0)
		{
			pTile->sCacheIndex = D3DFloorCacheAlloc(&gtFloorCache);
#line 408
			D2ASSERT(pTile->sCacheIndex-1 >= 0);
#line 409
			D2ASSERT(pTile->sCacheIndex-1 < gtFloorCache.nMaxNumItems);
			ptItem = (D3DCACHEITEM *)gtFloorCache.tItems + (pTile->sCacheIndex - 1);
			ddsd.dwSize = sizeof(ddsd);
			memset(&ddsd.dwFlags, 0, sizeof(ddsd) - sizeof(DWORD));
			ptItem->pOwner = pTile;
			hResult = sglpddsFloorMemory->lpVtbl->pfnLock(sglpddsFloorMemory, NULL, &ddsd, 33, NULL);
			if (hResult)
				D3DLogDDError("Failed to lock memory surface", hResult);
#line 425
			D2ASSERT(ddsd.dwFlags & DDSD_PITCH);
			nPitch = ddsd.nPitch >> 1;
			pwSurface = (WORD *)ddsd.pbSurface;
			memset(pwSurface, 0, (DWORD)(gnFloorRows * nPitch) * 2);
			for (i = 0; i < pTile->nCompCount; i++)
			{
				ptTileComp = &pTile->ptCompArray[i];
				pwTile = pwSurface + (ptTileComp->sOffsetY * nPitch + ptTileComp->sOffsetX);
				if (!(ptTileComp->wFlags & 1))
					continue;
				if (ptTileComp->wFlags & 4)
				{
					nRows = 15;
					pbData = ptTileComp->ptData;
					pwRow = pwTile;
					pwPixel = pwTile;
					do
					{
						bSkip = *pbData++;
						bRun = *pbData++;
						if (!bSkip && !bRun)
						{
							pwRow += nPitch;
							nRows--;
							pwPixel = pwRow;
						}
						else
						{
							pwPixel += bSkip;
							while (bRun--)
								*pwPixel++ = gawPalette[*pbData++];
						}
					}
					while (nRows);
				}
				else
				{
					nWidth = 4;
					pbData = ptTileComp->ptData;
					pwPixel = pwTile + 14;
					for (nRows = 0; nRows < 15; nRows++)
					{
						pwRow = pwPixel;
						pbSrc = pbData;
						for (x = nWidth; x > 0; x--)
							*pwRow++ = gawPalette[*pbSrc++];
						pbData += nWidth;
						if (nRows < 7)
						{
							pwPixel += nPitch - 2;
							nWidth += 4;
						}
						else
						{
							pwPixel += nPitch + 2;
							nWidth -= 4;
						}
					}
				}
			}
			hResult = sglpddsFloorMemory->lpVtbl->pfnUnlock(sglpddsFloorMemory, NULL);
			if (hResult)
				D3DLogDDError("Failed to unlock memory surface", hResult);
			if (gtVideoConfig.dwFlags & gdwBitMasks[10])
				D3DCopySurface(ptItem->lpddsDeviceSurface, sglpddsFloorMemory, gnFloorRows);
			else
				ptItem->lptexDevice->lpVtbl->field_14(ptItem->lptexDevice, sglptexFloorMemory); glpDevice->lpVtbl->field_98(glpDevice, 0, ptItem->lptexDevice);
		}
		else
		{
			ptItem = (D3DCACHEITEM *)gtFloorCache.tItems + (pTile->sCacheIndex - 1);
			D3DFloorCacheTouch(&gtFloorCache, pTile->sCacheIndex); glpDevice->lpVtbl->field_98(glpDevice, 0, ptItem->lptexDevice); }

		fResult = TRUE; }
	return fResult;
}

void __fastcall D3DBindTileComp(D3DTILECOMP *ptTileComp)
{
	D3DCACHEITEM *ptItem;
	HRESULT hResult;
	const BYTE *pbData;
	BYTE *pbRow;
	BYTE *pbPixel;
	int nRows;
	BYTE bSkip;
	BYTE bRun;

#line 491
	D2ASSERT(ptTileComp->sCacheIndex >= INVALID_HARDWARE_ADDRESS);
	if (ptTileComp->sCacheIndex == 0)
	{
		pbData = ptTileComp->ptData;
		pbRow = sgabSubtileBuffer;
		nRows = 32;
		pbPixel = pbRow;
		memset(sgabSubtileBuffer, 0, sizeof(sgabSubtileBuffer));
		do
		{
			bSkip = *pbData++;
			bRun = *pbData++;
			if (!bSkip && !bRun)
			{
				nRows--;
				pbRow += 32;
				pbPixel = pbRow;
			}
			else
			{
				pbPixel += bSkip;
				while (bRun--)
					*pbPixel++ = *pbData++;
			}
		}
		while (nRows);
		ptTileComp->sCacheIndex = D3DSubtileCacheAlloc(&gtSubtileCache);
#line 504
		D2ASSERT(ptTileComp->sCacheIndex-1 >= 0);
#line 505
		D2ASSERT(ptTileComp->sCacheIndex-1 < gtSubtileCache.nMaxNumItems);
		ptItem = (D3DCACHEITEM *)gtSubtileCache.tItems + (ptTileComp->sCacheIndex - 1);
		ptItem->pOwner = ptTileComp;
		D3DSURFDESC ddsd = {sizeof(ddsd)};
		hResult = sglpddsSubtileMemory->lpVtbl->pfnLock(sglpddsSubtileMemory, NULL, &ddsd, 33, NULL);
		if (hResult)
			D3DLogDDError("Failed to lock memory surface", hResult);
#line 519
		D2ASSERT(ddsd.dwFlags & DDSD_PITCH);
		D3DExpandToWords(sgabSubtileBuffer, (WORD *)ddsd.pbSurface, 32, 32, ddsd.nPitch >> 1, 32);
		hResult = sglpddsSubtileMemory->lpVtbl->pfnUnlock(sglpddsSubtileMemory, NULL);
		if (hResult)
			D3DLogDDError("Failed to unlock memory surface", hResult);
		if (gtVideoConfig.dwFlags & gdwBitMasks[10])
			D3DCopySurface(ptItem->lpddsDeviceSurface, sglpddsSubtileMemory, 32);
		else
			ptItem->lptexDevice->lpVtbl->field_14(ptItem->lptexDevice, sglptexSubtileMemory); glpDevice->lpVtbl->field_98(glpDevice, 0, ptItem->lptexDevice);
	}
	else
	{
		ptItem = (D3DCACHEITEM *)gtSubtileCache.tItems + (ptTileComp->sCacheIndex - 1);
		D3DSubtileCacheTouch(&gtSubtileCache, ptTileComp->sCacheIndex); glpDevice->lpVtbl->field_98(glpDevice, 0, ptItem->lptexDevice);
	}

}

void __fastcall D3DTileFreed(D3DTILE *pTile)
{
	int i;

	if (pTile->sCacheIndex > 0)
		D3DFloorCacheFree(&gtFloorCache, (short)(pTile->sCacheIndex - 1));
	for (i = 0; i < pTile->nCompCount; i++)
		if (pTile->ptCompArray[i].sCacheIndex > 0)
			D3DSubtileCacheFree(&gtSubtileCache, (short)(pTile->ptCompArray[i].sCacheIndex - 1));
}
