#define D2_THIS_FILE "C:\\D2\\Source\\D2Glide\\Src\\glSprite.cpp"
#include "glCore.h"
#include "glCaches.h"
#include "StormImports.h"
#include "D2GlideImports.h"
#include "D2Macros.h"

#define DECOMPRESSION_BUFFER_SIZE	65536

struct D2CELCONTEXT
{
 	void		*hCel;
};

struct GLSPRITEINFO
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
 	int			nLongSideLog2;
 	int			nAspectLog2;
};

GLCACHE gatTextureCache[3] = { 0 };
static DWORD sgfSpriteTmuByCache = 0;
static DWORD PLACEHOLDER_UnreferencedData_1001fe34[64] = { 0 };
static GLVERTEX sgatSpriteVertex[4] = { 0 };
static DWORD sgnSpriteTmu = 0;
static BYTE *sgpbDecompressionBuffer = 0;
static BYTE *sgpbHighlightBuffer = 0;
static int sganSpriteAspectTall[4] = { 0, -1, -2, -3 };
static int sganSpriteAspectWide[4] = { 0, 1, 2, 3 };
static int sgaanSpriteTexScale[7][2] =
{
	{ 32, 256 }, { 64, 256 }, { 128, 256 }, { 256, 256 },
	{ 256, 128 }, { 256, 64 }, { 256, 32 },
};

static void __fastcall GlideSpriteCelFreed(GLTEXLOCATION *ptLocations);

void __fastcall GlideSpriteOpen(void)
{
#line 74
	D2ASSERT(!sgpbDecompressionBuffer);
#line 75
	D2ASSERT(!sgpbHighlightBuffer);
#line 76
	sgpbDecompressionBuffer = (BYTE *)D2ALLOC2(65536);
#line 77
	sgpbHighlightBuffer = (BYTE *)D2ALLOC2(65536);
	D2CMPCelSetHardwareFree(GlideSpriteCelFreed);
	sgfSpriteTmuByCache = FALSE;
	sgnSpriteTmu = gnTmuCount > 1;
	if (gnTmuCount > 1 && gnScreenWidth == 800 && gnScreenHeight == 600)
		sgfSpriteTmuByCache = TRUE;
}

void __fastcall GlideSpriteClose(void)
{
	if (sgpbDecompressionBuffer)
	{
#line 111
		D2FREE2(sgpbDecompressionBuffer);
		sgpbDecompressionBuffer = NULL;
	}
	if (sgpbHighlightBuffer)
	{
#line 117
		D2FREE2(sgpbHighlightBuffer);
		sgpbHighlightBuffer = NULL;
	}
}

static void __fastcall GlideSpriteCelFreed(GLTEXLOCATION *ptLocations)
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
		((GLTEXCACHEITEM *)gatTextureCache[nCache].tItems)[nItem].ptLocations = NULL;
	}
}

static inline void GlideSpriteGetInfo(GLSPRITEINFO *ptInfo, D2CELCONTEXT *ptCelContext)
{
#line 201
	D2ASSERT(ptCelContext->hCel);
	ptInfo->ptCelContext = ptCelContext;
	ptInfo->nWidth = D2CMPCelGetWidth(ptCelContext->hCel);
	ptInfo->nXOffset = D2CMPCelGetOffsetX(ptCelContext->hCel);
	ptInfo->nHeight = D2CMPCelGetHeight(ptCelContext->hCel);
	ptInfo->nYOffset = D2CMPCelGetOffsetY(ptCelContext->hCel);
}

static inline BOOL GlideSpriteLoadInfo(GLSPRITEINFO *ptInfo, D2CELCONTEXT *ptCelContext)
{
	if (!D2CMPSpriteCacheGetContextCel(ptCelContext, 0, 1))
		return FALSE;
	GlideSpriteGetInfo(ptInfo, ptCelContext);
	return TRUE;
}

static inline void GlideSpriteSetAlpha(int nMode)
{
	switch (nMode)
	{
	case 5:
	case 7:
		GlideSetAlphaMode(1);
		break;
	case 3:
		GlideSetAlphaMode(2);
		grConstantColorValue(255);
		grDitherMode(0);
		break;
	case 4:
		GlideSetAlphaMode(3);
		grConstantColorValue(255);
		grDitherMode(0);
		break;
	default:
		GlideSetAlphaMode(4);
		grDitherMode(0);
		if (nMode == 0)
			grConstantColorValue(64);
		else if (nMode == 1)
			grConstantColorValue(128);
		else if (nMode == 2)
			grConstantColorValue(192);
		break;
	}
}

static inline BOOL GlideSpriteOnScreen(const GLVERTEX *ptVertex)
{
	return ptVertex->flX >= 0.0f && ptVertex->flX < 640.0f && ptVertex->flY >= 0.0f
		&& ptVertex->flY < 433.0f;
}

static void __fastcall GlideSpriteSetTexSize(GLSPRITEINFO *ptInfo);
static BOOL __fastcall GlideSpriteLoadTexture(GLSPRITEINFO *ptInfo, const BYTE *pbPreMap, BYTE bColor, int nSlot);

BOOL __fastcall GlideDrawCelPerspective(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	int nBoundary, const BYTE *pbPreMap)
{
	GLSPRITEINFO tInfo;
	int i;
	float fLeftS;
	float fRightS;
	float fTopT;
	float fBottomT;
	int nScreenX;
	int nScreenY;
	int nDown;
	int nAcross;
	float fX;
	float fY;
	int nRight;
	int nUp;
	int nScaleS;
	int nScaleT;

	if (!GlideSpriteLoadInfo(&tInfo, ptCelContext))
		return FALSE;
	GlideSpriteSetTexSize(&tInfo);
	nScaleS = sgaanSpriteTexScale[tInfo.nAspectLog2 + 3][0];
	nScaleT = sgaanSpriteTexScale[tInfo.nAspectLog2 + 3][1];
	fLeftS = (float)nScaleS * tInfo.flLeftU;
	GL_FLOAT_STORE(fLeftS);
	fRightS = (float)nScaleS * tInfo.flRightU;
	GL_FLOAT_STORE(fRightS);
	fTopT = (float)nScaleT * tInfo.flTopV;
	GL_FLOAT_STORE(fTopT);
	fBottomT = (float)nScaleT * tInfo.flBottomV;
	GL_FLOAT_STORE(fBottomT);
	if (!GlideGetPerspective())
	{
		nAcross = tInfo.nWidth << 11;
		nDown = tInfo.nHeight << 12;
	}
	else
	{
		nAcross = tInfo.nTexWidth << 11;
		nDown = tInfo.nTexHeight << 12;
	}
	nRight = tInfo.nXOffset << 11;
	nUp = tInfo.nYOffset << 12;
	nX += nUp + nRight;
	nY += nUp - nRight;
	GlidePerspectiveProject(nX - nDown, nY - nDown, 0, &nScreenX, &nScreenY);
	fX = (float)nScreenX;
	fY = (float)nScreenY;
	sgatSpriteVertex[0].flX = fX;
	sgatSpriteVertex[0].flY = fY;
	sgatSpriteVertex[0].field_0c = 1.0f;
	sgatSpriteVertex[0].flS = fLeftS;
	sgatSpriteVertex[0].flT = fTopT;
	GlidePerspectiveProject(nX, nY, 0, &nScreenX, &nScreenY);
	fX = (float)nScreenX;
	fY = (float)nScreenY;
	sgatSpriteVertex[1].flX = fX;
	sgatSpriteVertex[1].flY = fY;
	sgatSpriteVertex[1].field_0c = 1.0f;
	sgatSpriteVertex[1].flS = fLeftS;
	sgatSpriteVertex[1].flT = fBottomT;
	GlidePerspectiveProject(nX + nAcross, nY - nAcross, 0, &nScreenX, &nScreenY);
	fX = (float)nScreenX;
	fY = (float)nScreenY;
	sgatSpriteVertex[2].flX = fX;
	sgatSpriteVertex[2].flY = fY;
	sgatSpriteVertex[2].field_0c = 1.0f;
	sgatSpriteVertex[2].flS = fRightS;
	sgatSpriteVertex[2].flT = fBottomT;
	GlidePerspectiveProject(nX + nAcross - nDown, nY - nDown - nAcross, 0, &nScreenX, &nScreenY);
	fX = (float)nScreenX;
	fY = (float)nScreenY;
	sgatSpriteVertex[3].flX = fX;
	sgatSpriteVertex[3].flY = fY;
	sgatSpriteVertex[3].field_0c = 1.0f;
	sgatSpriteVertex[3].flS = fRightS;
	sgatSpriteVertex[3].flT = fTopT;
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
	if (!GlideSpriteOnScreen(&sgatSpriteVertex[0]) && !GlideSpriteOnScreen(&sgatSpriteVertex[1])
		&& !GlideSpriteOnScreen(&sgatSpriteVertex[2]) && !GlideSpriteOnScreen(&sgatSpriteVertex[3]))
		return TRUE;
	if (!GlideSpriteLoadTexture(&tInfo, pbPreMap, 0, 1))
		return TRUE;
	GlideSpriteSetAlpha(nMode);
	GlideSetColorCombine(1);
	GlideSetFilterMode(1);
	GlideSetVertexLight(&sgatSpriteVertex[0], dwColor);
	sgatSpriteVertex[1].dwColor = sgatSpriteVertex[0].dwColor;
	sgatSpriteVertex[2].dwColor = sgatSpriteVertex[0].dwColor;
	sgatSpriteVertex[3].dwColor = sgatSpriteVertex[0].dwColor;
	grDrawVertexArrayContiguous(5, 4, sgatSpriteVertex, sizeof(GLVERTEX));
	grDitherMode(2);
	return TRUE;
}

static inline int GlideSpriteTexSide(int nSide)
{
	if (nSide > 128)
		return 256;
	if (nSide > 64)
		return 128;
	if (nSide > 32)
		return 64;
	if (nSide > 16)
		return 32;
	return nSide > 8 ? 16 : 8;
}

static void __fastcall GlideSpriteSetTexSize(GLSPRITEINFO *ptInfo)
{
	int nTexWidth;
	int nTexHeight;
	int nLonger;
	int nShift;
	int nLog2;

	nTexWidth = GlideSpriteTexSide(ptInfo->nWidth);
	ptInfo->nTexWidth = nTexWidth;
	nTexHeight = GlideSpriteTexSide(ptInfo->nHeight);
	ptInfo->nTexHeight = nTexHeight;
	nLonger = nTexWidth > nTexHeight ? nTexWidth : nTexHeight;
	if (nLonger == 256)
		nLog2 = 8;
	else if (nLonger == 128)
		nLog2 = 7;
	else if (nLonger == 64)
		nLog2 = 6;
	else if (nLonger == 32)
		nLog2 = 5;
	else
		nLog2 = (nLonger == 16) + 3;
	ptInfo->nLongSideLog2 = nLog2;
	if (nTexWidth == nTexHeight)
		ptInfo->nAspectLog2 = 0;
	else if (nTexWidth < nTexHeight)
	{
		for (nShift = 0; nShift < 3; nShift++)
		{
			nTexWidth += nTexWidth;
			if (nTexWidth > nTexHeight)
				break;
		}
		ptInfo->nTexWidth = nTexHeight >> nShift;
		ptInfo->nAspectLog2 = sganSpriteAspectTall[nShift];
	}
	else
	{
		for (nShift = 0; nShift < 3; nShift++)
		{
			nTexHeight += nTexHeight;
			if (nTexHeight > nTexWidth)
				break;
		}
		ptInfo->nTexHeight = nTexWidth >> nShift;
		ptInfo->nAspectLog2 = sganSpriteAspectWide[nShift];
	}
	if (!GlideGetPerspective())
	{
		ptInfo->flLeftU = 0.0f;
		ptInfo->flBottomV = 1.0f;
		ptInfo->flRightU = (float)ptInfo->nWidth / ptInfo->nTexWidth;
		ptInfo->flTopV = 1.0f - (float)ptInfo->nHeight / ptInfo->nTexHeight;
	}
	else
	{
		ptInfo->flLeftU = 0.0f;
		ptInfo->flTopV = 0.0f;
		ptInfo->flRightU = 1.0f;
		ptInfo->flBottomV = 1.0f;
	}
	ptInfo->nTexSize = ptInfo->nTexWidth * ptInfo->nTexHeight;
}

static BOOL __fastcall GlideSpriteLoadTexture(GLSPRITEINFO *ptInfo, const BYTE *pbPreMap, BYTE bColor, int nSlot)
{
	GLTEXLOCATION *ptLocations;
	GLCACHE *ptCache;
	GLTEXINFO tTexInfo;
	DWORD dwAddress;
	const BYTE *pbMap;
	const BYTE *pbFrom;
	BYTE *pbTo;
	int nAsked;
	int n;

#line 296
	D2ASSERT(ptInfo->ptCelContext->hCel);
	ptLocations = (GLTEXLOCATION *)D2CMPCelGetSlot(ptInfo->ptCelContext->hCel);
	nAsked = nSlot;
	if (nSlot == 4)
		nSlot = 1;
	tTexInfo.field_00 = tTexInfo.field_04 = ptInfo->nLongSideLog2;
	tTexInfo.field_0c = 5;
	tTexInfo.dwAspectLog2 = ptInfo->nAspectLog2;
	tTexInfo.pbPixels = sgpbDecompressionBuffer;
	if (!ptLocations[nSlot].nWhichCache)
	{
		if (ptInfo->nTexSize <= 4096)
			ptLocations[nSlot].nWhichCache = 3;
		else if (ptInfo->nTexSize <= 16384)
			ptLocations[nSlot].nWhichCache = 2;
		else if (ptInfo->nTexSize <= 65536)
			ptLocations[nSlot].nWhichCache = 1;
		else
		{
#line 334
			D2FATAL("Sprite had bad cache location index!");
		}
	}
	ptCache = &gatTextureCache[ptLocations[nSlot].nWhichCache - 1];
	if (sgfSpriteTmuByCache)
		sgnSpriteTmu = ptLocations[nSlot].nWhichCache == 1;
	if (ptLocations[nSlot].nIndex
		&& (((GLTEXCACHEITEM *)ptCache->tItems)[ptLocations[nSlot].nIndex - 1].pbPreMap == pbPreMap
			|| nAsked == 4))
	{
		GlideTextureCacheTouch(ptCache, ptLocations[nSlot].nIndex);
		dwAddress = ((GLTEXCACHEITEM *)ptCache->tItems)[ptLocations[nSlot].nIndex - 1].dwAddress;
	}
	else
	{
		if (!ptLocations[nSlot].nIndex)
			ptLocations[nSlot].nIndex = GlideTextureCacheAlloc(ptCache);
		((GLTEXCACHEITEM *)ptCache->tItems)[ptLocations[nSlot].nIndex - 1].ptLocations = ptLocations;
		((GLTEXCACHEITEM *)ptCache->tItems)[ptLocations[nSlot].nIndex - 1].nSlot = nSlot;
		((GLTEXCACHEITEM *)ptCache->tItems)[ptLocations[nSlot].nIndex - 1].pbPreMap = pbPreMap;
		dwAddress = ((GLTEXCACHEITEM *)ptCache->tItems)[ptLocations[nSlot].nIndex - 1].dwAddress;
#line 378
		D2ASSERT(sgpbDecompressionBuffer);
#line 379
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
			for (n = 0; n < ptInfo->nTexSize; n++)
			{
				if (*pbTo)
					*pbTo = pbMap[*pbTo];
				pbTo++;
			}
			break;
		case 3:
			D2CMPCelContextDraw(ptInfo->ptCelContext, 1 - ptInfo->nXOffset,
				ptInfo->nTexHeight - ptInfo->nYOffset - 2, 0, 0, sgpbDecompressionBuffer,
				ptInfo->nTexHeight, ptInfo->nTexWidth, 0, 0, 0);
#line 419
			D2ASSERT(sgpbHighlightBuffer);
#line 420
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
			tTexInfo.pbPixels = sgpbHighlightBuffer;
			break;
		}
		grTexDownloadMipMap(sgnSpriteTmu, dwAddress, 3, &tTexInfo);
	}
	grTexSource(sgnSpriteTmu, dwAddress, 3, &tTexInfo);
	GlideSetTexCombine(sgnSpriteTmu);
	return TRUE;
}

static void __fastcall GlideSpriteDrawQuad(GLSPRITEINFO *ptInfo, int nX, int nY, DWORD dwColor, int nPalette,
	BOOL fPalette, DWORD dwUnused);

static inline BOOL GlideSpriteOffScreen(const GLSPRITEINFO *ptInfo, int nX, int nY)
{
	nX += ptInfo->nXOffset;
	if (nX > gnScreenWidth)
		return TRUE;
	if (ptInfo->nWidth + nX < 0)
		return TRUE;
	nY += ptInfo->nYOffset;
	if (nY < 0)
		return TRUE;
	if (nY - ptInfo->nHeight < gnScreenHeight)
		return FALSE;
	return TRUE;
}

BOOL __fastcall GlideDrawCel(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	const BYTE *pbPreMap)
{
	GLSPRITEINFO tInfo;

	if (!GlideSpriteLoadInfo(&tInfo, ptCelContext))
		return FALSE;
	if (GlideSpriteOffScreen(&tInfo, nX, nY))
		return FALSE;
	GlideSpriteSetTexSize(&tInfo);
	if (nMode == 7)
	{
		if (!GlideSpriteLoadTexture(&tInfo, pbPreMap, 0, 2))
			return TRUE;
	}
	else if (!GlideSpriteLoadTexture(&tInfo, pbPreMap, 0, 1))
		return TRUE;
	GlideSpriteSetAlpha(nMode);
	GlideSpriteDrawQuad(&tInfo, nX, nY, dwColor, 0, FALSE, 0);
	return TRUE;
}

static void __fastcall GlideSpriteDrawQuad(GLSPRITEINFO *ptInfo, int nX, int nY, DWORD dwColor, int nPalette,
	BOOL fPalette, DWORD dwUnused)
{
	float fLeftS;
	float fRightS;
	float fTopT;
	float fBottomT;
	DWORD fPerspective;
	int nLeft;
	int nTop;
	int nWidth;
	int nHeight;
	int nAspect;
	int nScaleS;
	int nScaleT;
	int nBaseX;
	int nBaseY;
	float fLeft;
	float fRight;
	float fTop;
	float fBottom;

	GlideSetColorCombine(1);
	GlideSetFilterMode(1);
	if (!nPalette && !fPalette)
		GlideSetVertexLight(&sgatSpriteVertex[0], dwColor);
	else
	{
		sgatSpriteVertex[0].dwColor = ((((0xFFFFFF00 | gptEnvInfo->pbPalette[nPalette * 3]) << 8)
			| gptEnvInfo->pbPalette[nPalette * 3 + 1]) << 8) | gptEnvInfo->pbPalette[nPalette * 3 + 2];
	}
	sgatSpriteVertex[1].dwColor = sgatSpriteVertex[0].dwColor;
	sgatSpriteVertex[2].dwColor = sgatSpriteVertex[0].dwColor;
	sgatSpriteVertex[3].dwColor = sgatSpriteVertex[0].dwColor;
	nAspect = ptInfo->nAspectLog2 + 3;
	nScaleS = sgaanSpriteTexScale[nAspect][0];
	nScaleT = sgaanSpriteTexScale[nAspect][1];
	fLeftS = (float)nScaleS * ptInfo->flLeftU;
	GL_FLOAT_STORE(fLeftS);
	fRightS = (float)nScaleS * ptInfo->flRightU;
	GL_FLOAT_STORE(fRightS);
	fTopT = (float)nScaleT * ptInfo->flTopV;
	GL_FLOAT_STORE(fTopT);
	fBottomT = (float)nScaleT * ptInfo->flBottomV;
	GL_FLOAT_STORE(fBottomT);
	fPerspective = GlideGetPerspective();
	if (!fPerspective)
	{
		nWidth = ptInfo->nWidth;
		nHeight = ptInfo->nHeight;
	}
	else
	{
		nWidth = ptInfo->nTexWidth;
		nHeight = ptInfo->nTexHeight;
	}
	nLeft = ptInfo->nXOffset;
	nTop = ptInfo->nYOffset - nHeight;
	if (fPerspective && gnPerspDepth != 256)
	{
		nLeft = gnPerspDepth * nLeft / 256;
		nTop = gnPerspDepth * nTop / 256;
		nWidth = gnPerspDepth * nWidth / 256;
		nHeight = gnPerspDepth * nHeight / 256;
	}
	nBaseX = nLeft + nX;
	fLeft = (float)nBaseX;
	fRight = (float)(nBaseX + nWidth);
	nBaseY = nTop + nY;
	fTop = (float)nBaseY;
	fBottom = (float)(nBaseY + nHeight);
	sgatSpriteVertex[0].flX = fLeft;
	sgatSpriteVertex[0].flY = fTop;
	sgatSpriteVertex[0].field_0c = 1.0f;
	sgatSpriteVertex[0].flS = fLeftS;
	sgatSpriteVertex[0].flT = fTopT;
	sgatSpriteVertex[1].flX = fLeft;
	sgatSpriteVertex[1].flY = fBottom;
	sgatSpriteVertex[1].field_0c = 1.0f;
	sgatSpriteVertex[1].flS = fLeftS;
	sgatSpriteVertex[1].flT = fBottomT;
	sgatSpriteVertex[2].flX = fRight;
	sgatSpriteVertex[2].flY = fBottom;
	sgatSpriteVertex[2].field_0c = 1.0f;
	sgatSpriteVertex[2].flS = fRightS;
	sgatSpriteVertex[2].flT = fBottomT;
	sgatSpriteVertex[3].flX = fRight;
	sgatSpriteVertex[3].flY = fTop;
	sgatSpriteVertex[3].field_0c = 1.0f;
	sgatSpriteVertex[3].flS = fRightS;
	sgatSpriteVertex[3].flT = fTopT;
	grDrawVertexArrayContiguous(5, 4, sgatSpriteVertex, sizeof(GLVERTEX));
	grDitherMode(2);
}

BOOL __fastcall GlideDrawCelPalette(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	int nPalette)
{
	GLSPRITEINFO tInfo;

	if (!GlideSpriteLoadInfo(&tInfo, ptCelContext))
		return FALSE;
	if (GlideSpriteOffScreen(&tInfo, nX, nY))
		return FALSE;
	GlideSpriteSetTexSize(&tInfo);
	if (GlideSpriteLoadTexture(&tInfo, NULL, 0, 1))
	{
		GlideSpriteSetAlpha(nMode);
		GlideSpriteDrawQuad(&tInfo, nX, nY, dwColor, nPalette, FALSE, 0);
	}
	return TRUE;
}

void __fastcall GlideSpriteDrawRows(D2CELCONTEXT *ptCelContext, int nX, int nY, int nFromBottom, int nRows,
	int nMode)
{
	GLSPRITEINFO tInfo;
	float fPerTexel;

	GlideSpriteSetAlpha(nMode);
	if (!D2CMPSpriteCacheGetContextCel(ptCelContext, 0, 1))
		return;
	GlideSpriteGetInfo(&tInfo, ptCelContext);
	GlideSpriteSetTexSize(&tInfo);
	if (!GlideSpriteLoadTexture(&tInfo, NULL, 0, 1))
		return;
	fPerTexel = 1.0f / tInfo.nTexHeight;
	GL_FLOAT_STORE(fPerTexel);
	if (!GlideGetPerspective())
	{
		if (nFromBottom)
		{
			nY -= nFromBottom;
			tInfo.flTopV += (float)(DWORD)(tInfo.nHeight - nFromBottom - nRows) * fPerTexel;
			tInfo.flBottomV -= (float)(DWORD)nFromBottom * fPerTexel;
		}
		else
			tInfo.flTopV += (float)(DWORD)(tInfo.nHeight - nRows) * fPerTexel;
		tInfo.nHeight = nRows;
	}
	else
	{
		if (nFromBottom)
		{
			nY -= nFromBottom;
			tInfo.flTopV = (float)(DWORD)(tInfo.nTexHeight - nFromBottom - nRows) * fPerTexel;
			tInfo.flBottomV = (float)(DWORD)(tInfo.nTexHeight - nFromBottom) * fPerTexel;
		}
		else
			tInfo.flTopV = (float)(DWORD)(tInfo.nTexHeight - nRows) * fPerTexel;
		tInfo.nTexHeight = nRows;
	}
	GlideSpriteDrawQuad(&tInfo, nX, nY, 0xFFFFFFFF, 0, FALSE, 0);
}

void __fastcall GlideSpriteDrawShadow(D2CELCONTEXT *ptCelContext, int nX, int nY)
{
	GLSPRITEINFO tInfo;
	GLSPRITEINFO *ptInfo;
	float fLeftS;
	float fRightS;
	float fTopT;
	float fBottomT;
	DWORD fPerspective;
	int nLeft;
	int nTop;
	int nWidth;
	int nHalfHeight;
	int nBaseX;
	int nBaseY;
	int nScaleS;
	int nScaleT;
	float fTop;
	float fBottom;
	float fX;

	if (!D2CMPSpriteCacheGetContextCel(ptCelContext, 0, 1))
		return;
	ptInfo = &tInfo;
	GlideSpriteGetInfo(ptInfo, ptCelContext);
	GlideSpriteSetTexSize(ptInfo);
	if (!GlideSpriteLoadTexture(ptInfo, NULL, 0, 4))
		return;
	GlideSetColorCombine(1);
	if (GlideGetDrawCtxFlag())
	{
		GlideSetAlphaMode(4);
		grConstantColorValue(192);
	}
	else
		GlideSetAlphaMode(1);
	GlideSetFilterMode(1);
	nScaleS = sgaanSpriteTexScale[ptInfo->nAspectLog2 + 3][0];
	nScaleT = sgaanSpriteTexScale[ptInfo->nAspectLog2 + 3][1];
	fLeftS = (float)nScaleS * ptInfo->flLeftU;
	GL_FLOAT_STORE(fLeftS);
	fRightS = (float)nScaleS * ptInfo->flRightU;
	GL_FLOAT_STORE(fRightS);
	fTopT = (float)nScaleT * ptInfo->flTopV;
	GL_FLOAT_STORE(fTopT);
	fBottomT = (float)nScaleT * ptInfo->flBottomV;
	GL_FLOAT_STORE(fBottomT);
	fPerspective = GlideGetPerspective();
	if (!fPerspective)
	{
		nWidth = ptInfo->nWidth;
		nHalfHeight = ptInfo->nHeight;
	}
	else
	{
		nWidth = ptInfo->nTexWidth;
		nHalfHeight = ptInfo->nTexHeight;
	}
	ptInfo->nYOffset >>= 1;
	nHalfHeight >>= 1;
	nLeft = ptInfo->nXOffset + ptInfo->nYOffset;
	nTop = ptInfo->nYOffset - nHalfHeight;
	if (fPerspective && gnPerspDepth != 256)
	{
		nLeft = gnPerspDepth * nLeft / 256;
		nTop = gnPerspDepth * nTop / 256;
		nWidth = gnPerspDepth * nWidth / 256;
		nHalfHeight = gnPerspDepth * nHalfHeight / 256;
	}
	nBaseX = nX + nLeft;
	nBaseY = nTop + nY;
	fTop = (float)nBaseY;
	fBottom = (float)(nBaseY + nHalfHeight);
	fX = (float)(nBaseX - nHalfHeight);
	sgatSpriteVertex[0].flX = fX;
	sgatSpriteVertex[0].flY = fTop;
	sgatSpriteVertex[0].field_0c = 1.0f;
	sgatSpriteVertex[0].flS = fLeftS;
	sgatSpriteVertex[0].flT = fTopT;
	fX = (float)nBaseX;
	sgatSpriteVertex[1].flX = fX;
	sgatSpriteVertex[1].flY = fBottom;
	sgatSpriteVertex[1].field_0c = 1.0f;
	sgatSpriteVertex[1].flS = fLeftS;
	sgatSpriteVertex[1].flT = fBottomT;
	fX = (float)(nBaseX + nWidth);
	sgatSpriteVertex[2].flX = fX;
	sgatSpriteVertex[2].flY = fBottom;
	sgatSpriteVertex[2].field_0c = 1.0f;
	sgatSpriteVertex[2].flS = fRightS;
	sgatSpriteVertex[2].flT = fBottomT;
	fX = (float)(nBaseX - nHalfHeight + nWidth);
	sgatSpriteVertex[3].flX = fX;
	sgatSpriteVertex[3].flY = fTop;
	sgatSpriteVertex[3].field_0c = 1.0f;
	sgatSpriteVertex[3].flS = fRightS;
	sgatSpriteVertex[3].flT = fTopT;
	sgatSpriteVertex[0].dwColor = 0;
	sgatSpriteVertex[1].dwColor = 0;
	sgatSpriteVertex[2].dwColor = 0;
	sgatSpriteVertex[3].dwColor = 0;
	grDrawVertexArrayContiguous(5, 4, sgatSpriteVertex, sizeof(GLVERTEX));
}

void __fastcall GlideSpriteDrawHighlight(D2CELCONTEXT *ptCelContext, int nX, int nY, BYTE bColor)
{
	GLSPRITEINFO tInfo;

	GlideSpriteSetAlpha(0);
	if (!D2CMPSpriteCacheGetContextCel(ptCelContext, 0, 1))
		return;
	GlideSpriteGetInfo(&tInfo, ptCelContext);
	tInfo.nWidth += 2;
	tInfo.nHeight += 2;
	GlideSpriteSetTexSize(&tInfo);
	if (!GlideSpriteLoadTexture(&tInfo, NULL, bColor, 3))
		return;
	GlideSpriteDrawQuad(&tInfo, nX - 1, nY + 1, 0xFFFFFFFF, 0, TRUE, 0);
}

void __fastcall GlideSpriteWarp(int nX, int nY)
{
}
