#define D2_THIS_FILE "C:\\D2\\Source\\D2Direct3D\\Src\\d3dSprite.cpp"
#include "d3dCaches.h"
#include "StormImports.h"
#include "D2Direct3DImports.h"
#include "D2Macros.h"

#define DECOMPRESSION_BUFFER_SIZE	65536
#define DDSD_PITCH			8

struct D2CELCONTEXT
{
 	void		*hCel;
};

struct D3DSPRITEINFO
{
 	D2CELCONTEXT *ptCelContext;
 	int			nWidth;
 	int			nHeight;
 	int			nXOffset;
 	int			nYOffset;
 	int			nTexWidth;
 	int			nTexHeight;
 	int			nTexSize;
 	float		flLeftU;
 	float		flRightU;
 	float		flTopV;
 	float		flBottomV;
};

D3DCACHE gatTextureCache[3] = { 0 };
static D3DSURFACE *sgalpddsSpriteMemory[3] = { 0 };
static D3DTEXTURE *sgalptexSpriteMemory[3] = { 0 };
static DWORD PLACEHOLDER_UnreferencedData_1003ae0c[64] = { 0 };
static D3DTLVERTEX sgatSpriteVertex[4] = { 0 };
static BYTE *sgpbDecompressionBuffer = 0;
static BYTE *sgpbHighlightBuffer = 0;

static void __fastcall D3DSpriteCelFreed(D3DTEXLOCATION *ptLocations);

void __fastcall D3DSpriteOpen(void)
{
	BOOL success;

#line 76
	D2ASSERT(!sgpbDecompressionBuffer);
#line 77
	D2ASSERT(!sgpbHighlightBuffer);
#line 78
	sgpbDecompressionBuffer = (BYTE *)D2ALLOC2(DECOMPRESSION_BUFFER_SIZE);
#line 79
	sgpbHighlightBuffer = (BYTE *)D2ALLOC2(DECOMPRESSION_BUFFER_SIZE);
	success = D3DCreateMemoryTexture(&sgalpddsSpriteMemory[0], &sgalptexSpriteMemory[0], 256, 256);
#line 85
	D2ASSERT(success);
	success = D3DCreateMemoryTexture(&sgalpddsSpriteMemory[1], &sgalptexSpriteMemory[1], 128, 128);
#line 88
	D2ASSERT(success);
	success = D3DCreateMemoryTexture(&sgalpddsSpriteMemory[2], &sgalptexSpriteMemory[2], 32, 32);
#line 91
	D2ASSERT(success);
	D2CMPCelSetHardwareFree(D3DSpriteCelFreed);
}

void WINAPI D3DSpriteClose(void)
{
	int i;

	if (sgpbDecompressionBuffer)
	{
#line 106
		D2FREE2(sgpbDecompressionBuffer);
		sgpbDecompressionBuffer = NULL;
	}
	if (sgpbHighlightBuffer)
	{
#line 112
		D2FREE2(sgpbHighlightBuffer);
		sgpbHighlightBuffer = NULL;
	}
	for (i = 0; i < 3; i++)
	{
		if (sgalptexSpriteMemory[i])
		{
			sgalptexSpriteMemory[i]->lpVtbl->field_08(sgalptexSpriteMemory[i]);
			sgalptexSpriteMemory[i] = NULL;
		}
		if (sgalpddsSpriteMemory[i])
		{
			sgalpddsSpriteMemory[i]->lpVtbl->field_08(sgalpddsSpriteMemory[i]);
			sgalpddsSpriteMemory[i] = NULL;
		}
	}
}

static void __fastcall D3DSpriteCelFreed(D3DTEXLOCATION *ptLocations)
{
	int nCache;
	int nItem;
	int i;

	for (i = 5; i; i--, ptLocations++)
	{
		nCache = ptLocations->nWhichCache - 1;
		if (nCache < 0)
			continue;
		nItem = ptLocations->nIndex - 1;
		if (nItem < 0)
			continue;
		((D3DTEXCACHEITEM *)gatTextureCache[nCache].tItems)[nItem].ptLocations = NULL;
	}
}

static inline void D3DSpriteGetInfo(D3DSPRITEINFO *ptInfo, D2CELCONTEXT *ptCelContext)
{
#line 182
	D2ASSERT(ptCelContext->hCel);
	ptInfo->ptCelContext = ptCelContext;
	ptInfo->nWidth = D2CMPCelGetWidth(ptCelContext->hCel);
	ptInfo->nXOffset = D2CMPCelGetOffsetX(ptCelContext->hCel);
	ptInfo->nHeight = D2CMPCelGetHeight(ptCelContext->hCel);
	ptInfo->nYOffset = D2CMPCelGetOffsetY(ptCelContext->hCel);
}

static inline BOOL D3DSpriteLoadInfo(D3DSPRITEINFO *ptInfo, D2CELCONTEXT *ptCelContext)
{
	if (!D2CMPSpriteCacheGetContextCel(ptCelContext, 0, 1))
		return FALSE;
	D3DSpriteGetInfo(ptInfo, ptCelContext);
	return TRUE;
}

static inline BYTE D3DSpriteSetAlpha(int nMode)
{
	BYTE bAlpha;

	bAlpha = 255;
	switch (nMode)
	{
	case 5:
	case 7:
		D3DSetAlphaMode(1);
		break;
	case 3:
		D3DSetAlphaMode(2);
		break;
	case 4:
		D3DSetAlphaMode(3);
		break;
	default:
		D3DSetAlphaMode(4);
		if (nMode == 0)
			bAlpha = 64;
		else if (nMode == 1)
			bAlpha = 128;
		else if (nMode == 2)
			bAlpha = 192;
		break;
	}
	return bAlpha;
}

static inline BOOL D3DSpriteOnScreen(const D3DTLVERTEX *ptVertex)
{
	return ptVertex->flX >= 0.0f && ptVertex->flX < 640.0f && ptVertex->flY >= 0.0f
		&& ptVertex->flY < 433.0f;
}

static void __fastcall D3DSpriteSetTexSize(D3DSPRITEINFO *ptInfo);
static BOOL __fastcall D3DSpriteLoadTexture(D3DSPRITEINFO *ptInfo, const BYTE *pbPreMap, BYTE bColor, int nSlot);

BOOL __fastcall D3DDrawCelPerspective(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	int nBoundary, const BYTE *pbPreMap)
{
	D3DSPRITEINFO tInfo;
	int i;
	int nScreenX;
	int nScreenY;
	int nRight;
	int nUp;
	int nDown;
	int nAcross;
	float fX;
	float fY;
	float flLeft;
	float flRight;
	float flTop;
	float flBottom;
	BYTE bAlpha;

	if (!D3DSpriteLoadInfo(&tInfo, ptCelContext))
		return FALSE;
	D3DSpriteSetTexSize(&tInfo);
	flLeft = tInfo.flLeftU;
	flRight = tInfo.flRightU;
	flTop = tInfo.flTopV;
	flBottom = tInfo.flBottomV;
	nAcross = tInfo.nTexWidth << 11;
	nDown = tInfo.nTexHeight << 12;
	nRight = tInfo.nXOffset << 11;
	nUp = tInfo.nYOffset << 12;
	nX += nUp + nRight;
	nY += nUp - nRight;
	D3DPerspectiveProject(nX - nDown, nY - nDown, 0, &nScreenX, &nScreenY);
	fX = (float)nScreenX;
	fY = (float)nScreenY;
	sgatSpriteVertex[0].flX = fX;
	sgatSpriteVertex[0].flY = fY;
	sgatSpriteVertex[0].field_08 = 0.5f;
	sgatSpriteVertex[0].field_0c = 1.0f;
	sgatSpriteVertex[0].flU = flLeft;
	sgatSpriteVertex[0].flV = flTop;
	D3DPerspectiveProject(nX, nY, 0, &nScreenX, &nScreenY);
	fX = (float)nScreenX;
	fY = (float)nScreenY;
	sgatSpriteVertex[1].flX = fX;
	sgatSpriteVertex[1].flY = fY;
	sgatSpriteVertex[1].field_08 = 0.5f;
	sgatSpriteVertex[1].field_0c = 1.0f;
	sgatSpriteVertex[1].flU = flLeft;
	sgatSpriteVertex[1].flV = flBottom;
	D3DPerspectiveProject(nX + nAcross, nY - nAcross, 0, &nScreenX, &nScreenY);
	fX = (float)nScreenX;
	fY = (float)nScreenY;
	sgatSpriteVertex[2].flX = fX;
	sgatSpriteVertex[2].flY = fY;
	sgatSpriteVertex[2].field_08 = 0.5f;
	sgatSpriteVertex[2].field_0c = 1.0f;
	sgatSpriteVertex[2].flU = flRight;
	sgatSpriteVertex[2].flV = flBottom;
	D3DPerspectiveProject(nX + nAcross - nDown, nY - nDown - nAcross, 0, &nScreenX, &nScreenY);
	fX = (float)nScreenX;
	fY = (float)nScreenY;
	sgatSpriteVertex[3].flX = fX;
	sgatSpriteVertex[3].flY = fY;
	sgatSpriteVertex[3].field_08 = 0.5f;
	sgatSpriteVertex[3].field_0c = 1.0f;
	sgatSpriteVertex[3].flU = flRight;
	sgatSpriteVertex[3].flV = flTop;
	if (nBoundary == -160)
	{
		for (i = 0; i < 4; i++)
			sgatSpriteVertex[i].flX -= 160.0f;
	}
	else if (nBoundary == 160)
	{
		for (i = 0; i < 4; i++)
			sgatSpriteVertex[i].flX += 160.0f;
	}
	if (!D3DSpriteOnScreen(&sgatSpriteVertex[0]) && !D3DSpriteOnScreen(&sgatSpriteVertex[1])
		&& !D3DSpriteOnScreen(&sgatSpriteVertex[2]) && !D3DSpriteOnScreen(&sgatSpriteVertex[3]))
		return TRUE;
	bAlpha = D3DSpriteSetAlpha(nMode);
	if (!D3DSpriteLoadTexture(&tInfo, pbPreMap, 0, 1))
		return TRUE;
	D3DSetBlendMode(1);
	D3DSetFilterMode(1);
	D3DSetVertexLight(&sgatSpriteVertex[0], dwColor, bAlpha);
	sgatSpriteVertex[1].dwColor = sgatSpriteVertex[0].dwColor;
	sgatSpriteVertex[2].dwColor = sgatSpriteVertex[0].dwColor;
	sgatSpriteVertex[3].dwColor = sgatSpriteVertex[0].dwColor;
	glpDevice->lpVtbl->field_70(glpDevice, 6, 452, sgatSpriteVertex, 4, 28);
	return TRUE;
}

static void __fastcall D3DSpriteSetTexSize(D3DSPRITEINFO *ptInfo)
{
	if (ptInfo->nWidth <= 32 && ptInfo->nHeight <= 32)
	{
		ptInfo->nTexWidth = 32;
		ptInfo->nTexHeight = 32;
	}
	else if (ptInfo->nWidth <= 128 && ptInfo->nHeight <= 128)
	{
		ptInfo->nTexWidth = 128;
		ptInfo->nTexHeight = 128;
	}
	else if (ptInfo->nWidth <= 256 && ptInfo->nHeight <= 256)
	{
		ptInfo->nTexWidth = 256;
		ptInfo->nTexHeight = 256;
	}
	ptInfo->flLeftU = 0.0f;
	ptInfo->flRightU = 1.0f;
	ptInfo->flTopV = 0.0f;
	ptInfo->flBottomV = 1.0f;
	ptInfo->nTexSize = ptInfo->nTexHeight * ptInfo->nTexWidth;
}

static BOOL __fastcall D3DSpriteLoadTexture(D3DSPRITEINFO *ptInfo, const BYTE *pbPreMap, BYTE bColor, int nSlot)
{
	D3DTEXLOCATION *ptLocations;
	D3DTEXLOCATION *tLocation;
	D3DCACHE *ptWhichCache;
	D3DTEXCACHEITEM *tCacheItem;
	D3DSURFACE *lpddsMemory;
	D3DTEXTURE *lptexMemory;
	HRESULT hResult;
	const BYTE *pbSource;
	const BYTE *pbMap;
	const BYTE *pbFrom;
	BYTE *pbTo;
	int nAsked;
	int n;

#line 248
	D2ASSERT(ptInfo->ptCelContext->hCel);
	ptLocations = (D3DTEXLOCATION *)D2CMPCelGetSlot(ptInfo->ptCelContext->hCel);
	nAsked = nSlot;
	if (nSlot == 4)
		nSlot = 1;
	tLocation = &ptLocations[nSlot];
	if (!tLocation->nWhichCache)
	{
		if (ptInfo->nWidth <= 32 && ptInfo->nHeight <= 32)
			tLocation->nWhichCache = 3;
		else if (ptInfo->nWidth <= 128 && ptInfo->nHeight <= 128)
			tLocation->nWhichCache = 2;
		else if (ptInfo->nWidth <= 256 && ptInfo->nHeight <= 256)
			tLocation->nWhichCache = 1;
		else
		{
#line 282
			D2FATAL("Unhandled sprite size");
		}
	}
	ptWhichCache = &gatTextureCache[tLocation->nWhichCache - 1];
	if (tLocation->nIndex)
	{
		tCacheItem = (D3DTEXCACHEITEM *)ptWhichCache->tItems + (tLocation->nIndex - 1);
		if (tCacheItem->pbPreMap == pbPreMap || nAsked == 4)
		{
			D3DTextureCacheTouch(ptWhichCache, tLocation->nIndex);
			tCacheItem = (D3DTEXCACHEITEM *)ptWhichCache->tItems + (tLocation->nIndex - 1);
			hResult = glpDevice->lpVtbl->field_98(glpDevice, 0, tCacheItem->lptexDevice);
			if (hResult)
				D3DLogError("Failed to set texture", hResult);
			return TRUE;
		}
	}
	lpddsMemory = sgalpddsSpriteMemory[tLocation->nWhichCache - 1];
	lptexMemory = sgalptexSpriteMemory[tLocation->nWhichCache - 1];
	if (!tLocation->nIndex)
		tLocation->nIndex = D3DTextureCacheAlloc(ptWhichCache);
#line 313
	D2ASSERT(tLocation->nIndex-1 >= 0);
#line 314
	D2ASSERT(tLocation->nIndex-1 < ptWhichCache->nMaxNumItems);
	pbSource = sgpbDecompressionBuffer;
	tCacheItem = (D3DTEXCACHEITEM *)ptWhichCache->tItems + (tLocation->nIndex - 1);
	tCacheItem->ptLocations = ptLocations;
	tCacheItem->nSlot = nSlot;
	tCacheItem->pbPreMap = pbPreMap;
#line 322
	D2ASSERT(tCacheItem->lpddsDeviceSurface);
#line 323
	D2ASSERT(tCacheItem->lptexDevice);
#line 326
	D2ASSERT(sgpbDecompressionBuffer);
#line 327
	D2ASSERT(ptInfo->nTexSize <= DECOMPRESSION_BUFFER_SIZE);
	memset(sgpbDecompressionBuffer, 0, ptInfo->nTexSize);
	switch (nSlot)
	{
	case 1:
	case 4:
		D2CMPCelContextDraw(ptInfo->ptCelContext, -ptInfo->nXOffset,
			ptInfo->nTexHeight - ptInfo->nYOffset - 1, 0, 0, sgpbDecompressionBuffer,
			ptInfo->nTexHeight, ptInfo->nTexWidth, 0, 0, pbPreMap);
		break;
	case 2:
		D2CMPCelContextDraw(ptInfo->ptCelContext, -ptInfo->nXOffset,
			ptInfo->nTexHeight - ptInfo->nYOffset - 1, 0, 0, sgpbDecompressionBuffer,
			ptInfo->nTexHeight, ptInfo->nTexWidth, 0, 0, pbPreMap);
		pbTo = sgpbDecompressionBuffer;
		pbMap = gptEnvInfo->pbColorMap;
		for (n = 0; n < ptInfo->nTexSize; pbTo++, n++)
		{
			if (*pbTo)
				*pbTo = pbMap[*pbTo];
		}
		break;
	case 3:
		D2CMPCelContextDraw(ptInfo->ptCelContext, 1 - ptInfo->nXOffset,
			ptInfo->nTexHeight - ptInfo->nYOffset - 2, 0, 0, sgpbDecompressionBuffer,
			ptInfo->nTexHeight, ptInfo->nTexWidth, 0, 0, 0);
#line 367
		D2ASSERT(sgpbHighlightBuffer);
#line 368
		D2ASSERT(ptInfo->nTexSize <= DECOMPRESSION_BUFFER_SIZE);
		memset(sgpbHighlightBuffer, 0, ptInfo->nTexSize);
		pbFrom = sgpbDecompressionBuffer + ptInfo->nTexWidth;
		pbTo = sgpbHighlightBuffer + ptInfo->nTexWidth;
		for (n = ptInfo->nTexSize - (ptInfo->nTexWidth << 1); n > 0; n--, pbTo++)
		{
			if (*pbFrom++)
			{
				pbTo[-ptInfo->nTexWidth] = bColor;
				pbTo[-1] = bColor;
				pbTo[1] = bColor;
				pbTo[ptInfo->nTexWidth] = bColor;
			}
		}
		pbSource = sgpbHighlightBuffer;
		break;
	}
#line 393
	D2ASSERT(lpddsMemory);
#line 394
	D2ASSERT(lptexMemory);
	D3DSURFDESC ddsd = { sizeof(ddsd) };

	hResult = lpddsMemory->lpVtbl->pfnLock(lpddsMemory, NULL, &ddsd, 33, NULL);
	if (hResult)
		D3DLogDDError("Failed to lock memory surface", hResult);
#line 405
	D2ASSERT(ddsd.dwFlags & DDSD_PITCH);
	D3DExpandToWords(pbSource, (WORD *)ddsd.pbSurface, ptInfo->nTexWidth, ptInfo->nTexHeight,
		ddsd.nPitch >> 1,
		ptInfo->nTexHeight);
	hResult = lpddsMemory->lpVtbl->pfnUnlock(lpddsMemory, NULL);
	if (hResult)
		D3DLogDDError("Failed to unlock memory surface", hResult);
	if (gtVideoConfig.dwFlags & gdwBitMasks[10])
		D3DCopySurface(tCacheItem->lpddsDeviceSurface, lpddsMemory, ptInfo->nTexHeight);
	else
		tCacheItem->lptexDevice->lpVtbl->field_14(tCacheItem->lptexDevice, lptexMemory);
	hResult = glpDevice->lpVtbl->field_98(glpDevice, 0, tCacheItem->lptexDevice);
	if (hResult)
		D3DLogError("Failed to set texture", hResult);
	return TRUE;
}

static void __fastcall D3DSpriteDrawQuad(D3DSPRITEINFO *ptInfo, int nX, int nY, DWORD dwColor, int nPalette,
	BYTE bAlpha,
	BOOL fPalette, DWORD dwUnused);

static inline BOOL D3DSpriteOffScreen(const D3DSPRITEINFO *ptInfo, int nX, int nY)
{
	int nLeft;
	int nBottom;

	nLeft = ptInfo->nXOffset + nX;
	if (nLeft > gnScreenWidth)
		return TRUE;
	if (ptInfo->nWidth + nLeft < 0)
		return TRUE;
	nBottom = ptInfo->nYOffset + nY;
	if (nBottom < 0)
		return TRUE;
	if (nBottom - ptInfo->nHeight >= gnScreenHeight)
		return TRUE;
	return FALSE;
}

BOOL __fastcall D3DDrawCel(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	const BYTE *pbPreMap)
{
	D3DSPRITEINFO tInfo;
	BYTE bAlpha;

	if (!D3DSpriteLoadInfo(&tInfo, ptCelContext))
		return FALSE;
	if (D3DSpriteOffScreen(&tInfo, nX, nY))
		return FALSE;
	D3DSpriteSetTexSize(&tInfo);
	bAlpha = D3DSpriteSetAlpha(nMode);
	if (nMode == 7)
	{
		if (D3DSpriteLoadTexture(&tInfo, pbPreMap, 0, 2))
			D3DSpriteDrawQuad(&tInfo, nX, nY, dwColor, 0, bAlpha, FALSE, 0);
	}
	else if (D3DSpriteLoadTexture(&tInfo, pbPreMap, 0, 1))
		D3DSpriteDrawQuad(&tInfo, nX, nY, dwColor, 0, bAlpha, FALSE, 0);
	return TRUE;
}

static void __fastcall D3DSpriteDrawQuad(D3DSPRITEINFO *ptInfo, int nX, int nY, DWORD dwColor, int nPalette,
	BYTE bAlpha,
	BOOL fPalette, DWORD dwUnused)
{
	const BYTE *pbEntry;
	int nLeft;
	int nTop;
	int nWidth;
	int nHeight;
	int nScreenX;
	int nScreenY;
	float flLeft;
	float flRight;
	float flTop;
	float flBottom;
	BOOL fPerspective;
	float fLeft;
	float fRight;
	float fTop;
	float fBottom;

	D3DSetBlendMode(1);
	D3DSetFilterMode(1);
	if (!nPalette && !fPalette)
		D3DSetVertexLight(&sgatSpriteVertex[0], dwColor, bAlpha);
	else
	{
		pbEntry = gptEnvInfo->pbPalette + nPalette * 3;
		sgatSpriteVertex[0].dwColor = (((((DWORD)bAlpha << 8) | pbEntry[0]) << 8) | pbEntry[1]) << 8
			| pbEntry[2];
		sgatSpriteVertex[0].field_14 = 0;
	}
	sgatSpriteVertex[1].dwColor = sgatSpriteVertex[0].dwColor;
	sgatSpriteVertex[2].dwColor = sgatSpriteVertex[0].dwColor;
	sgatSpriteVertex[3].dwColor = sgatSpriteVertex[0].dwColor;
	flLeft = ptInfo->flLeftU;
	flRight = ptInfo->flRightU;
	flTop = ptInfo->flTopV;
	flBottom = ptInfo->flBottomV;
	nLeft = ptInfo->nXOffset;
	fPerspective = D3DGetPerspective();
	nWidth = ptInfo->nTexWidth;
	nHeight = ptInfo->nTexHeight;
	nTop = ptInfo->nYOffset - nHeight;
	if (fPerspective && gnPerspDepth != 256)
	{
		nLeft = gnPerspDepth * nLeft / 256;
		nTop = gnPerspDepth * nTop / 256;
		nWidth = gnPerspDepth * nWidth / 256;
		nHeight = gnPerspDepth * nHeight / 256;
	}
	nScreenX = nLeft + nX;
	fLeft = (float)nScreenX;
	fRight = (float)(nScreenX + nWidth);
	nScreenY = nTop + nY;
	fTop = (float)nScreenY;
	fBottom = (float)(nScreenY + nHeight);
	sgatSpriteVertex[0].flX = fLeft;
	sgatSpriteVertex[0].flY = fTop;
	sgatSpriteVertex[0].field_08 = 0.5f;
	sgatSpriteVertex[0].field_0c = 1.0f;
	sgatSpriteVertex[0].flU = flLeft;
	sgatSpriteVertex[0].flV = flTop;
	sgatSpriteVertex[1].flX = fLeft;
	sgatSpriteVertex[1].flY = fBottom;
	sgatSpriteVertex[1].field_08 = 0.5f;
	sgatSpriteVertex[1].field_0c = 1.0f;
	sgatSpriteVertex[1].flU = flLeft;
	sgatSpriteVertex[1].flV = flBottom;
	sgatSpriteVertex[2].flX = fRight;
	sgatSpriteVertex[2].flY = fBottom;
	sgatSpriteVertex[2].field_08 = 0.5f;
	sgatSpriteVertex[2].field_0c = 1.0f;
	sgatSpriteVertex[2].flU = flRight;
	sgatSpriteVertex[2].flV = flBottom;
	sgatSpriteVertex[3].flX = fRight;
	sgatSpriteVertex[3].flY = fTop;
	sgatSpriteVertex[3].field_08 = 0.5f;
	sgatSpriteVertex[3].field_0c = 1.0f;
	sgatSpriteVertex[3].flU = flRight;
	sgatSpriteVertex[3].flV = flTop;
	glpDevice->lpVtbl->field_70(glpDevice, 6, 452, sgatSpriteVertex, 4, 28);
}

BOOL __fastcall D3DDrawCelPalette(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	int nPalette)
{
	D3DSPRITEINFO tInfo;
	BYTE bAlpha;

	if (!D3DSpriteLoadInfo(&tInfo, ptCelContext))
		return FALSE;
	if (D3DSpriteOffScreen(&tInfo, nX, nY))
		return FALSE;
	D3DSpriteSetTexSize(&tInfo);
	if (!D3DSpriteLoadTexture(&tInfo, NULL, 0, 1))
		return TRUE;
	bAlpha = D3DSpriteSetAlpha(nMode);
	D3DSpriteDrawQuad(&tInfo, nX, nY, dwColor, nPalette, bAlpha, FALSE, 0);
	return TRUE;
}

void __fastcall D3DSpriteDrawRows(D2CELCONTEXT *ptCelContext, int nX, int nY, int nFromBottom, int nRows,
	int nMode)
{
	D3DSPRITEINFO tInfo;
	float fPerTexel;
	BYTE bAlpha;

	bAlpha = D3DSpriteSetAlpha(nMode);
	if (!D2CMPSpriteCacheGetContextCel(ptCelContext, 0, 1))
		return;
	D3DSpriteGetInfo(&tInfo, ptCelContext);
	D3DSpriteSetTexSize(&tInfo);
	if (!D3DSpriteLoadTexture(&tInfo, NULL, 0, 1))
		return;
	fPerTexel = 1.0f / tInfo.nTexHeight;
	if (nFromBottom)
	{
		nY -= nFromBottom;
		tInfo.flTopV = (float)(DWORD)(tInfo.nTexHeight - nFromBottom - nRows) * fPerTexel;
		tInfo.flBottomV = (float)(DWORD)(tInfo.nTexHeight - nFromBottom) * fPerTexel;
	}
	else
		tInfo.flTopV = (float)(DWORD)(tInfo.nTexHeight - nRows) * fPerTexel;
	tInfo.nTexHeight = nRows;
	D3DSpriteDrawQuad(&tInfo, nX, nY, 0xFFFFFFFF, 0, bAlpha, FALSE, 0);
}

void __fastcall D3DSpriteDrawShadow(D2CELCONTEXT *ptCelContext, int nX, int nY)
{
	D3DSPRITEINFO tInfo;
	D3DSPRITEINFO *ptInfo;
	int nLeft;
	int nTop;
	int nScreenX;
	int nScreenY;
	int nWidth;
	int nHalfHeight;
	BOOL fPerspective;
	DWORD dwColor;
	float flLeft;
	float flRight;
	float flTop;
	float flBottom;
	BYTE bAlpha;
	float fTop;
	float fBottom;

	bAlpha = 255;
	ptInfo = &tInfo;
	if (!D3DSpriteLoadInfo(ptInfo, ptCelContext))
		return;
	D3DSpriteSetTexSize(ptInfo);
	if (!D3DSpriteLoadTexture(ptInfo, NULL, 0, 4))
		return;
	D3DSetBlendMode(1);
	if (D3DGetDrawCtxFlag())
	{
		D3DSetAlphaMode(4);
		bAlpha = 192;
	}
	else
		D3DSetAlphaMode(1);
	D3DSetFilterMode(1);
	flLeft = ptInfo->flLeftU;
	flRight = ptInfo->flRightU;
	flTop = ptInfo->flTopV;
	flBottom = ptInfo->flBottomV;
	fPerspective = D3DGetPerspective();
	ptInfo->nYOffset >>= 1;
	nHalfHeight = ptInfo->nTexHeight >> 1;
	nLeft = ptInfo->nXOffset;
	nTop = ptInfo->nYOffset;
	nWidth = ptInfo->nTexWidth;
	nLeft += nTop;
	nTop -= nHalfHeight;
	if (fPerspective && gnPerspDepth != 256)
	{
		nLeft = gnPerspDepth * nLeft / 256;
		nTop = gnPerspDepth * nTop / 256;
		nWidth = gnPerspDepth * nWidth / 256;
		nHalfHeight = gnPerspDepth * nHalfHeight / 256;
	}
	nScreenX = nLeft + nX;
	nScreenY = nTop + nY;
	fTop = (float)nScreenY;
	fBottom = (float)(nScreenY + nHalfHeight);
	sgatSpriteVertex[0].flX = (float)(nScreenX - nHalfHeight);
	sgatSpriteVertex[0].flY = fTop;
	sgatSpriteVertex[0].field_08 = 0.5f;
	sgatSpriteVertex[0].field_0c = 1.0f;
	sgatSpriteVertex[0].flU = flLeft;
	sgatSpriteVertex[0].flV = flTop;
	sgatSpriteVertex[1].flX = (float)nScreenX;
	sgatSpriteVertex[1].flY = fBottom;
	sgatSpriteVertex[1].field_08 = 0.5f;
	sgatSpriteVertex[1].field_0c = 1.0f;
	sgatSpriteVertex[1].flU = flLeft;
	sgatSpriteVertex[1].flV = flBottom;
	sgatSpriteVertex[2].flX = (float)(nScreenX + nWidth);
	sgatSpriteVertex[2].flY = fBottom;
	sgatSpriteVertex[2].field_08 = 0.5f;
	sgatSpriteVertex[2].field_0c = 1.0f;
	sgatSpriteVertex[2].flU = flRight;
	sgatSpriteVertex[2].flV = flBottom;
	sgatSpriteVertex[3].flX = (float)(nScreenX - nHalfHeight + nWidth);
	sgatSpriteVertex[3].flY = fTop;
	sgatSpriteVertex[3].field_08 = 0.5f;
	sgatSpriteVertex[3].field_0c = 1.0f;
	sgatSpriteVertex[3].flU = flRight;
	sgatSpriteVertex[3].flV = flTop;
	dwColor = (DWORD)bAlpha << 24;
	sgatSpriteVertex[0].dwColor = dwColor;
	sgatSpriteVertex[1].dwColor = dwColor;
	sgatSpriteVertex[2].dwColor = dwColor;
	sgatSpriteVertex[3].dwColor = dwColor;
	glpDevice->lpVtbl->field_70(glpDevice, 6, 452, sgatSpriteVertex, 4, 28);
}

void __fastcall D3DSpriteDrawHighlight(D2CELCONTEXT *ptCelContext, int nX, int nY, BYTE bColor)
{
	D3DSPRITEINFO tInfo;

	D3DSetAlphaMode(4);
	if (!D2CMPSpriteCacheGetContextCel(ptCelContext, 0, 1))
		return;
	D3DSpriteGetInfo(&tInfo, ptCelContext);
	tInfo.nWidth += 2;
	tInfo.nHeight += 2;
	D3DSpriteSetTexSize(&tInfo);
	if (!D3DSpriteLoadTexture(&tInfo, NULL, bColor, 3))
		return;
	D3DSpriteDrawQuad(&tInfo, nX - 1, nY + 1, 0xFFFFFFFF, 0, 64, TRUE, 0);
}

void __fastcall D3DSpriteWarp(int nX, int nY)
{
}
