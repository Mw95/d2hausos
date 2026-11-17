#define D2_THIS_FILE "C:\\D2\\Source\\D2Glide\\Src\\glBlocks.cpp"
#include "glCore.h"
#include "glCaches.h"
#include "StormImports.h"
#include "D2GlideImports.h"
#include "D2Macros.h"

#define INVALID_HARDWARE_ADDRESS	0

GLTEXINFO gtSubtileTexInfo = { 5, 5, 0, 5, NULL };
GLTEXINFO gtFloorTexInfo = { 8, 8, 1, 5, NULL };

static DWORD sgdwTmu = 0;
static BYTE *sgpbSubtileBuffer = 0;
static BYTE *sgpbFloorBuffer = 0;

void __fastcall GlideBlocksOpen(void)
{
#line 51
	sgpbSubtileBuffer = (BYTE *)D2ALLOC2(1024);
#line 52
	sgpbFloorBuffer = (BYTE *)D2ALLOC2(32768);
	gtSubtileTexInfo.pbPixels = sgpbSubtileBuffer;
	gtFloorTexInfo.pbPixels = sgpbFloorBuffer;
}

void __fastcall GlideBlocksClose(void)
{
	if (sgpbSubtileBuffer)
	{
#line 61
		D2FREE2(sgpbSubtileBuffer);
	}
	if (sgpbFloorBuffer)
	{
#line 62
		D2FREE2(sgpbFloorBuffer);
	}
}

BOOL __fastcall GlideBindTile(GLTILE *pTile)
{
	GLTILECOMP *ptTileComp;
	const BYTE *pbSrc;
	BYTE *pbRow;
	BYTE *pbOut;
	BYTE *pbTile;
	DWORD *pdwClear;
	DWORD dwAddress;
	BYTE bSkip;
	BYTE bRun;
	int nRows;
	int nWidth;
	int i;
	BOOL fResult;

	fResult = FALSE;
	if (D2CMPTileCacheLoadTile(pTile, 1))
	{
#line 199
	D2ASSERT(pTile->sCacheIndex >= INVALID_HARDWARE_ADDRESS);
	if (pTile->sCacheIndex == INVALID_HARDWARE_ADDRESS)
	{
		pdwClear = (DWORD *)sgpbFloorBuffer;
		memset(pdwClear, 0, 32768);
		for (i = 0; i < pTile->nCompCount; i++)
		{
			ptTileComp = &pTile->ptCompArray[i];
			pbTile = (BYTE *)pdwClear + (ptTileComp->sOffsetY << 8) + ptTileComp->sOffsetX;
			if (!(ptTileComp->wFlags & 1))
				continue;
			if (ptTileComp->wFlags & 4)
			{
				nRows = 15;
				pbSrc = ptTileComp->ptData;
				pbRow = pbTile;
				pbOut = pbTile;
				do
				{
					bSkip = *pbSrc++;
					bRun = *pbSrc++;
					if (!bSkip && !bRun)
					{
						nRows--;
						pbRow += 256;
						pbOut = pbRow;
					}
					else
					{
						pbOut += bSkip;
						while (bRun--)
							*pbOut++ = *pbSrc++;
					}
				}
				while (nRows);
			}
			else
			{
				nWidth = 4;
				pbSrc = ptTileComp->ptData;
				pbOut = pbTile + 14;
				for (nRows = 0; nRows < 15; nRows++)
				{
					memcpy(pbOut, pbSrc, nWidth);
					pbSrc += nWidth;
					if (nRows < 7)
					{
						pbOut += 254;
						nWidth += 4;
					}
					else
					{
						pbOut += 258;
						nWidth -= 4;
					}
				}
			}
		}
		pTile->sCacheIndex = (short)GlideFloorCacheAlloc(&gtFloorCache);
#line 212
		D2ASSERT(pTile->sCacheIndex-1 >= 0);
#line 213
		D2ASSERT(pTile->sCacheIndex-1 < gtFloorCache.nMaxNumItems);
		((GLCACHEITEM *)gtFloorCache.tItems)[pTile->sCacheIndex - 1].pTile = pTile;
		dwAddress = ((GLCACHEITEM *)gtFloorCache.tItems)[pTile->sCacheIndex - 1].dwAddress;
		grTexDownloadMipMap(sgdwTmu, dwAddress, 3, &gtFloorTexInfo);
	}
	else
	{
		GlideFloorCacheTouch(&gtFloorCache, pTile->sCacheIndex);
		dwAddress = ((GLCACHEITEM *)gtFloorCache.tItems)[pTile->sCacheIndex - 1].dwAddress;
	}
	grTexSource(sgdwTmu, dwAddress, 3, &gtFloorTexInfo);
	GlideSetTexCombine(sgdwTmu);
	fResult = TRUE;
	}
	return fResult;
}

void __fastcall GlideBindTileComp(GLTILECOMP *ptTileComp)
{
	const BYTE *pbSrc;
	BYTE *pbRow;
	BYTE *pbOut;
	DWORD *pdwClear;
	DWORD dwAddress;
	BYTE bSkip;
	BYTE bRun;
	int nRows;
	int i;

#line 244
	D2ASSERT(ptTileComp->sCacheIndex >= INVALID_HARDWARE_ADDRESS);
	if (ptTileComp->sCacheIndex == INVALID_HARDWARE_ADDRESS)
	{
		nRows = 32;
		pbOut = pbRow = sgpbSubtileBuffer;
		pdwClear = (DWORD *)pbRow;
		pbSrc = ptTileComp->ptData;
		for (i = 0; i < 256; i++)
			pdwClear[i] = 0;
		do
		{
			bSkip = *pbSrc++;
			bRun = *pbSrc++;
			if (!bSkip && !bRun)
			{
				nRows--;
				pbRow += 32;
				pbOut = pbRow;
			}
			else
			{
				pbOut += bSkip;
				while (bRun--)
					*pbOut++ = *pbSrc++;
			}
		}
		while (nRows);
		ptTileComp->sCacheIndex = (short)GlideSubtileCacheAlloc(&gtSubtileCache);
#line 257
		D2ASSERT(ptTileComp->sCacheIndex-1 >= 0);
#line 258
		D2ASSERT(ptTileComp->sCacheIndex-1 < gtSubtileCache.nMaxNumItems);
		((GLCACHEITEM *)gtSubtileCache.tItems)[ptTileComp->sCacheIndex - 1].pTile = ptTileComp;
		dwAddress = ((GLCACHEITEM *)gtSubtileCache.tItems)[ptTileComp->sCacheIndex - 1].dwAddress;
		grTexDownloadMipMap(sgdwTmu, dwAddress, 3, &gtSubtileTexInfo);
	}
	else
	{
		GlideSubtileCacheTouch(&gtSubtileCache, ptTileComp->sCacheIndex);
		dwAddress = ((GLCACHEITEM *)gtSubtileCache.tItems)[ptTileComp->sCacheIndex - 1].dwAddress;
	}
	grTexSource(sgdwTmu, dwAddress, 3, &gtSubtileTexInfo);
	GlideSetTexCombine(sgdwTmu);
}

void __fastcall GlideTileFreed(GLTILE *pTile)
{
	int i;

	if (pTile->sCacheIndex > 0)
		GlideFloorCacheFree(&gtFloorCache, (short)(pTile->sCacheIndex - 1));
	for (i = 0; i < pTile->nCompCount; i++)
	{
		if (pTile->ptCompArray[i].sCacheIndex > 0)
			GlideSubtileCacheFree(&gtSubtileCache, (short)(pTile->ptCompArray[i].sCacheIndex - 1));
	}
}
