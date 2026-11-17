#define D2_THIS_FILE "C:\\D2\\Source\\D2Glide\\Src\\glWall.cpp"
#include "glCore.h"
#include "glCaches.h"
#include "StormImports.h"
#include "D2GlideImports.h"
#include "D2Macros.h"

#define NUMBER_OF_SUPPORTED_TILE_TYPES	10

static GLVERTEX sgatWallGrid[15] = { 0 };
static GLVERTEX sgatWallQuad[4] = { 0 };
static BOOL sgfWallCornersReady = 0;
static GLVERTEX *sgaptWallStrip0[10] =
{
	&sgatWallGrid[0], &sgatWallGrid[1], &sgatWallGrid[3], &sgatWallGrid[4], &sgatWallGrid[6],
	&sgatWallGrid[7], &sgatWallGrid[9], &sgatWallGrid[10], &sgatWallGrid[12], &sgatWallGrid[13]
};
static GLVERTEX *sgaptWallStrip1[10] =
{
	&sgatWallGrid[1], &sgatWallGrid[2], &sgatWallGrid[4], &sgatWallGrid[5], &sgatWallGrid[7],
	&sgatWallGrid[8], &sgatWallGrid[10], &sgatWallGrid[11], &sgatWallGrid[13], &sgatWallGrid[14]
};
static int sgaanWallCorner[NUMBER_OF_SUPPORTED_TILE_TYPES][6][3] =
{
	{ { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
	{ { 0, 5, -40 }, { 0, 3, -56 }, { 0, 1, -72 }, { 0, -1, -88 }, { 0, -3, -104 }, { 0, -5, -120 } },
	{ { -5, 0, -120 }, { -3, 0, -104 }, { -1, 0, -88 }, { 1, 0, -72 }, { 3, 0, -56 }, { 5, 0, -40 } },
	{ { 0, 5, -40 }, { 0, 3, -56 }, { 0, 1, -72 }, { 1, 0, -72 }, { 3, 0, -56 }, { 5, 0, -40 } },
	{ { -5, 0, -120 }, { -3, 0, -104 }, { -1, 0, -88 }, { 0, -1, -88 }, { 0, -3, -104 }, { 0, -5, -120 } },
	{ { 0, 5, -40 }, { 0, 3, -56 }, { 0, 1, -72 }, { 1, 0, -72 }, { 3, 0, -56 }, { 5, 0, -40 } },
	{ { 5, 10, 40 }, { 5, 8, 24 }, { 5, 6, 8 }, { 5, 4, -8 }, { 5, 2, -24 }, { 5, 0, -40 } },
	{ { 0, 5, -40 }, { 2, 5, -24 }, { 4, 5, -8 }, { 6, 5, 8 }, { 8, 5, 24 }, { 10, 5, 40 } },
	{ { 0, 5, -40 }, { 2, 5, -24 }, { 4, 5, -8 }, { 5, 4, -8 }, { 5, 2, -24 }, { 5, 0, -40 } },
	{ { 5, 10, 40 }, { 5, 8, 24 }, { 5, 6, 8 }, { 6, 5, 8 }, { 8, 5, 24 }, { 10, 5, 40 } },
};

void __fastcall GlideWallStartup(void)
{
	int nType;
	int nColumn;
	int i;
	int j;
	int nX;
	int nY;
	int *pnCorner;
	float flT;

	sgatWallQuad[0].flX = 0.0f;
	sgatWallQuad[0].flY = 0.0f;
	sgatWallQuad[0].field_0c = 1.0f;
	sgatWallQuad[0].flS = 0.0f;
	sgatWallQuad[0].flT = 0.0f;
	sgatWallQuad[1].flX = 0.0f;
	sgatWallQuad[1].flY = 0.0f;
	sgatWallQuad[1].field_0c = 1.0f;
	sgatWallQuad[1].flS = 0.0f;
	sgatWallQuad[1].flT = 256.0f;
	sgatWallQuad[2].flX = 0.0f;
	sgatWallQuad[2].flY = 0.0f;
	sgatWallQuad[2].field_0c = 1.0f;
	sgatWallQuad[2].flS = 256.0f;
	sgatWallQuad[2].flT = 256.0f;
	sgatWallQuad[3].flX = 0.0f;
	sgatWallQuad[3].flY = 0.0f;
	sgatWallQuad[3].field_0c = 1.0f;
	sgatWallQuad[3].flS = 256.0f;
	sgatWallQuad[3].flT = 0.0f;
	for (i = 0; i < 5; i++)
	{
		flT = (float)(i << 6);
		for (j = 0; j < 3; j++)
		{
			sgatWallGrid[i * 3 + j].field_0c = 1.0f;
			sgatWallGrid[i * 3 + j].flS = (float)(j << 7);
			sgatWallGrid[i * 3 + j].flT = flT;
		}
	}
	if (sgfWallCornersReady)
		return;
	for (nType = 0; nType < NUMBER_OF_SUPPORTED_TILE_TYPES; nType++)
	{
		for (nColumn = 0; nColumn < 6; nColumn++)
		{
			pnCorner = sgaanWallCorner[nType][nColumn];
			nX = pnCorner[0];
			nY = pnCorner[1];
			pnCorner[0] = nX << 16;
			pnCorner[1] = nY << 16;
		}
	}
	sgfWallCornersReady = TRUE;
}

static inline void GlideWallPlaceQuad(int nX, int nY, int nRight, int nTop)
{
	sgatWallQuad[0].flX = (float)nX;
	sgatWallQuad[0].flY = (float)nTop;
	sgatWallQuad[1].flX = (float)nX;
	sgatWallQuad[1].flY = (float)nY;
	sgatWallQuad[2].flX = (float)nRight;
	sgatWallQuad[2].flY = (float)nY;
	sgatWallQuad[3].flX = (float)nRight;
	sgatWallQuad[3].flY = (float)nTop;
}

static inline BOOL GlideWallOnScreen(const GLVERTEX *ptVertex, float fLeft, int nRight)
{
	const float fTop = 0.0f;
	const float fBottom = 433.0f;

	return ptVertex->flX >= fLeft && ptVertex->flX < (float)nRight
		&& ptVertex->flY >= fTop && ptVertex->flY < fBottom;
}

BOOL __fastcall GlideWallDrawFlat(GLTILE *pTile, int nX, int nY, BYTE bLight, int nBoundary)
{
	BOOL fDrawn;
	int nLeft;
	int nRight;
	int nPartX;
	int nPartY;
	int nPartRight;
	int nPartTop;
	int i;

	fDrawn = FALSE;
	nLeft = -32;
	nRight = 640;
	if (nBoundary == -160)
		nRight = 320;
	else if (nBoundary == 160)
		nLeft = 288;
	nY += 32;
	for (i = 0; i < pTile->nCompCount; i++)
	{
		nPartX = pTile->ptCompArray[i].sOffsetX + nX;
		nPartY = pTile->ptCompArray[i].sOffsetY + nY;
		if (nPartX < nLeft || nPartX >= nRight || nPartY < 0 || nPartY >= 465)
			continue;
		fDrawn = TRUE;
		nPartRight = nPartX + 32;
		nPartTop = nPartY - 32;
		GlideBindTileComp(&pTile->ptCompArray[i]);
		GlideWallPlaceQuad(nPartX, nPartY, nPartRight, nPartTop);
		GlideSetVertexLightRGB(&sgatWallQuad[0], bLight);
		sgatWallQuad[1].dwColor = sgatWallQuad[0].dwColor;
		sgatWallQuad[2].dwColor = sgatWallQuad[0].dwColor;
		sgatWallQuad[3].dwColor = sgatWallQuad[0].dwColor;
		grDrawVertexArrayContiguous(5, 4, sgatWallQuad, sizeof(GLVERTEX));
	}
	return fDrawn;
}

BOOL __fastcall GlideWallDrawFlatPerspective(GLTILE *pTile, int nX, int nY, BYTE bLight, int nBoundary)
{
	BOOL fDrawn;
	float fLeft;
	float fShift;
	int nLeft;
	int nRight;
	int nShift;
	int nColumn;
	int nRow;
	int nWorldX;
	int nWorldY;
	int nPointX;
	int nPointY;
	int nScreenX;
	int nScreenY;
	int nLine;
	int i;
	int j;

	nLeft = 0;
	nRight = 640;
	nShift = 0;
	if (nBoundary == -160)
		nShift = -160;
	else if (nBoundary == 160)
		nShift = 160;
	fDrawn = FALSE;
	if (nBoundary == -160 || nBoundary == 160)
	{
		nRight = 480;
		nLeft = 160;
	}
	nX <<= 13;
	nY <<= 13;
	for (i = 0; i < pTile->nCompCount; i++)
	{
		nColumn = pTile->ptCompArray[i].sOffsetX >> 5;
		nRow = (pTile->ptCompArray[i].sOffsetY + 96) >> 5;
		nWorldY = nRow;
		nWorldX = ((nColumn + nWorldY * 2) << 16) - 229376;
		nWorldY = ((nWorldY + nRow - nColumn) << 16) + 98304;
		GlidePerspectiveProject(nWorldX + nX, nWorldY + nY, 0, &nScreenX, &nScreenY);
		sgatWallGrid[0].flX = (float)nScreenX;
		sgatWallGrid[0].flY = (float)nScreenY;
		GlidePerspectiveProject(nWorldX + nX + 131072, nWorldY + nY + 131072, 0, &nScreenX, &nScreenY);
		sgatWallGrid[12].flX = (float)nScreenX;
		sgatWallGrid[12].flY = (float)nScreenY;
		GlidePerspectiveProject(nWorldX + nX + 196608, nWorldY + nY + 65536, 0, &nScreenX, &nScreenY);
		sgatWallGrid[14].flX = (float)nScreenX;
		sgatWallGrid[14].flY = (float)nScreenY;
		GlidePerspectiveProject(nWorldX + nX + 65536, nWorldY + nY - 65536, 0, &nScreenX, &nScreenY);
		sgatWallGrid[2].flX = (float)nScreenX;
		sgatWallGrid[2].flY = (float)nScreenY;
		fLeft = (float)nLeft;
		if (!GlideWallOnScreen(&sgatWallGrid[0], fLeft, nRight) && !GlideWallOnScreen(&sgatWallGrid[12],
			fLeft, nRight)
			&& !GlideWallOnScreen(&sgatWallGrid[14], fLeft, nRight) && !GlideWallOnScreen(&sgatWallGrid[2],
				fLeft,
			nRight))
			continue;
		GlideBindTileComp(&pTile->ptCompArray[i]);
		fShift = (float)nShift;
		for (nLine = 0; nLine < 5; nLine++)
		{
			nPointX = nWorldX + nX + nLine * 32768;
			nPointY = nWorldY + nY + nLine * 32768;
			for (j = 0; j < 3; nPointX += 32768, j++, nPointY -= 32768)
			{
				GlidePerspectiveProject(nPointX, nPointY, 0, &nScreenX, &nScreenY);
				sgatWallGrid[nLine * 3 + j].flX = (float)nScreenX;
				sgatWallGrid[nLine * 3 + j].flY = (float)nScreenY;
				sgatWallGrid[nLine * 3 + j].flX = fShift + sgatWallGrid[nLine * 3 + j].flX;
				GlideSetVertexLightRGB(&sgatWallGrid[nLine * 3 + j], bLight);
			}
		}
		grDrawVertexArray(4, 10, sgaptWallStrip0);
		grDrawVertexArray(4, 10, sgaptWallStrip1);
		fDrawn = TRUE;
	}
	return fDrawn;
}

BOOL __fastcall GlideDrawWallLight(GLTILE *pTile, int nX, int nY, BYTE bLight, int nBoundary)
{
	BOOL fDrawn;

	fDrawn = FALSE;
	if (!(!D2CMPTileCacheLoadTile(pTile, 1)))
	{
		GlideSetColorCombine(1);
		if (GlideGetDrawCtxFlag())
		{
			GlideSetAlphaMode(4);
			grConstantColorValue(192);
		} else
			GlideSetAlphaMode(1);
		GlideSetFilterMode(1);
		if (GlideGetPerspective())
			fDrawn = GlideWallDrawFlatPerspective(pTile, nX, nY, bLight, nBoundary);
		else
			fDrawn = GlideWallDrawFlat(pTile, nX, nY, bLight, nBoundary); }
	return fDrawn;
}

static inline BOOL GlideWallDrawLit(GLTILE *pTile, int nX, int nY, const DWORD *pdwLight, int nBoundary)
{
	BOOL fDrawn;
	DWORD dwLeft;
	DWORD dwRight;
	int nLeft;
	int nRight;
	int nPartX;
	int nPartY;
	int nPartRight;
	int nPartTop;
	int i;

	nLeft = -32;
	nRight = 640;
	fDrawn = FALSE;
	if (nBoundary == -160)
		nRight -= nRight / 2;
	else if (nBoundary == 160)
		nLeft += nRight / 2;
	nY += 32;
	for (i = 0; i < pTile->nCompCount; i++)
	{
		nPartX = pTile->ptCompArray[i].sOffsetX + nX;
		nPartY = pTile->ptCompArray[i].sOffsetY + nY;
		if (nPartX < nLeft || nPartX >= nRight || nPartY < 0 || nPartY >= 465)
			continue;
		dwLeft = pdwLight[pTile->ptCompArray[i].sOffsetX >> 5];
		dwRight = pdwLight[(pTile->ptCompArray[i].sOffsetX >> 5) + 1];
		nPartRight = nPartX + 32;
		nPartTop = nPartY - 32;
		fDrawn = TRUE;
		GlideBindTileComp(&pTile->ptCompArray[i]);
		GlideWallPlaceQuad(nPartX, nPartY, nPartRight, nPartTop);
		GlideSetVertexLight(&sgatWallQuad[0], dwLeft);
		GlideSetVertexLight(&sgatWallQuad[1], dwLeft);
		GlideSetVertexLight(&sgatWallQuad[2], dwRight);
		GlideSetVertexLight(&sgatWallQuad[3], dwRight);
		grDrawVertexArrayContiguous(5, 4, sgatWallQuad, sizeof(GLVERTEX));
	}
	return fDrawn;
}

static BOOL __fastcall GlideWallDrawLitPerspective(GLTILE *pTile, int nX, int nY, const DWORD *pdwLight,
	int nBoundary);

BOOL __fastcall GlideDrawWallLit(GLTILE *pTile, int nX, int nY, const DWORD *pdwLight, int nBoundary,
	BYTE bAlpha)
{
	BOOL fDrawn;

	fDrawn = FALSE;
	if (D2CMPTileCacheLoadTile(pTile, 1))
	{
		GlideSetColorCombine(1);
		if (bAlpha == 255)
			GlideSetAlphaMode(1);
		else
		{
			GlideSetAlphaMode(4);
			grConstantColorValue(bAlpha);
		}
		GlideSetFilterMode(1);
		if (GlideGetPerspective())
			fDrawn = GlideWallDrawLitPerspective(pTile, nX, nY, pdwLight, nBoundary);
		else
			fDrawn = GlideWallDrawLit(pTile, nX, nY, pdwLight, nBoundary);
	}
	return fDrawn;
}

extern int PLACEHOLDER_SymbolCounterPad00; extern int PLACEHOLDER_SymbolCounterPad01; extern int PLACEHOLDER_SymbolCounterPad02; extern int PLACEHOLDER_SymbolCounterPad03; extern int PLACEHOLDER_SymbolCounterPad04; extern int PLACEHOLDER_SymbolCounterPad05; extern int PLACEHOLDER_SymbolCounterPad06; extern int PLACEHOLDER_SymbolCounterPad07;
extern int PLACEHOLDER_SymbolCounterPad08; extern int PLACEHOLDER_SymbolCounterPad09; extern int PLACEHOLDER_SymbolCounterPad10; extern int PLACEHOLDER_SymbolCounterPad11; extern int PLACEHOLDER_SymbolCounterPad12; extern int PLACEHOLDER_SymbolCounterPad13; extern int PLACEHOLDER_SymbolCounterPad14; extern int PLACEHOLDER_SymbolCounterPad15;
extern int PLACEHOLDER_SymbolCounterPad16; extern int PLACEHOLDER_SymbolCounterPad17; extern int PLACEHOLDER_SymbolCounterPad18; extern int PLACEHOLDER_SymbolCounterPad19; extern int PLACEHOLDER_SymbolCounterPad20; extern int PLACEHOLDER_SymbolCounterPad21; extern int PLACEHOLDER_SymbolCounterPad22; extern int PLACEHOLDER_SymbolCounterPad23;

static BOOL __fastcall GlideWallDrawLitPerspective(GLTILE *pTile, int nX, int nY, const DWORD *pdwLight,
	int nBoundary)
{
	DWORD adwColor[3];
	DWORD dwLeft;
	DWORD dwRight;
	BOOL fDrawn;
	float fLeft;
	float fShift;
	int nType;
	int nOffsetX;
	int nLeft;
	int nRight;
	int nShift;
	int nPartX;
	int nPartY;
	const int *pnLeft;
	const int *pnRight;
	int nLeftX;
	int nLeftY;
	int nLeftZ;
	int nRightX;
	int nRightY;
	int nRightZ;
	int nMidX;
	int nMidY;
	int nMidZ;
	int nScreenX;
	int nScreenY;
	int nStep;
	int nDrop;
	int i;
	int j;
	int nNext;

	nLeft = 0;
	nRight = 640;
	nShift = 0;
	if (nBoundary == -160)
		nShift = -160;
	else if (nBoundary == 160)
		nShift = 160;
	fDrawn = FALSE;
	if (nBoundary == -160 || nBoundary == 160)
	{
		nRight -= 160;
		nLeft += 160;
	}
	nX <<= 13;
	nY <<= 13;
	nType = pTile->nType;
#line 445
	D2ASSERT(nType >= 0 && nType <NUMBER_OF_SUPPORTED_TILE_TYPES);
	for (i = 0; i < pTile->nCompCount; i++)
	{
		nPartX = pTile->ptCompArray[i].sOffsetX;
		nOffsetX = nPartX >> 5;
		nPartY = pTile->ptCompArray[i].sOffsetY;
#line 453
		D2ASSERT(nOffsetX >= 0 && nOffsetX < 5);
		nNext = nOffsetX + 1;
		pnLeft = sgaanWallCorner[nType][nOffsetX];
		pnRight = sgaanWallCorner[nType][nNext];
		nLeftX = pnLeft[0] + nX;
		nLeftY = pnLeft[1] + nY;
		nLeftZ = pnLeft[2] - nPartY;
		nRightX = pnRight[0] + nX;
		nRightY = pnRight[1] + nY;
		nRightZ = pnRight[2] - nPartY;
		GlidePerspectiveProject(nLeftX, nLeftY, nLeftZ, &nScreenX, &nScreenY);
		sgatWallGrid[0].flX = (float)nScreenX;
		sgatWallGrid[0].flY = (float)nScreenY;
		GlidePerspectiveProject(nLeftX, nLeftY, nLeftZ - 32, &nScreenX, &nScreenY);
		sgatWallGrid[12].flX = (float)nScreenX;
		sgatWallGrid[12].flY = (float)nScreenY;
		GlidePerspectiveProject(nRightX, nRightY, nRightZ - 32, &nScreenX, &nScreenY);
		sgatWallGrid[14].flX = (float)nScreenX;
		sgatWallGrid[14].flY = (float)nScreenY;
		GlidePerspectiveProject(nRightX, nRightY, nRightZ, &nScreenX, &nScreenY);
		sgatWallGrid[2].flX = (float)nScreenX;
		sgatWallGrid[2].flY = (float)nScreenY;
		fLeft = (float)nLeft;
		if (!GlideWallOnScreen(&sgatWallGrid[0], fLeft, nRight) && !GlideWallOnScreen(&sgatWallGrid[12],
			fLeft, nRight)
			&& !GlideWallOnScreen(&sgatWallGrid[14], fLeft, nRight) && !GlideWallOnScreen(&sgatWallGrid[2],
				fLeft,
			nRight))
			continue;
		nMidY = (nRightY + nLeftY) >> 1;
		nMidZ = (nLeftZ + nRightZ) >> 1;
		nMidX = (nRightX + nLeftX) >> 1;
		dwLeft = adwColor[0] = pdwLight[nOffsetX];
		dwRight = adwColor[2] = pdwLight[nOffsetX + 1];
		adwColor[1] = (BYTE)(((BYTE)(dwRight >> 24) + (BYTE)(dwLeft >> 24)) >> 1);
		adwColor[1] = (adwColor[1] << 8) + (BYTE)(((BYTE)(dwRight >> 16) + (BYTE)(dwLeft >> 16)) >> 1);
		adwColor[1] = (adwColor[1] << 8) + (BYTE)(((BYTE)(dwRight >> 8) + (BYTE)(dwLeft >> 8)) >> 1);
		adwColor[1] = (adwColor[1] << 8) + (BYTE)(((BYTE)dwRight + (BYTE)dwLeft) >> 1);
		GlideBindTileComp(&pTile->ptCompArray[i]);
		fShift = (float)nShift;
		for (nStep = 0; nStep < 5; nStep++)
		{
			nDrop = nStep << 3;
			if (nStep != 0 && nStep != 4)
			{
				GlidePerspectiveProject(nLeftX, nLeftY, nLeftZ - nDrop, &nScreenX, &nScreenY);
				sgatWallGrid[nStep * 3].flX = (float)nScreenX;
				sgatWallGrid[nStep * 3].flY = (float)nScreenY;
				GlidePerspectiveProject(nRightX, nRightY, nRightZ - nDrop, &nScreenX, &nScreenY);
				sgatWallGrid[nStep * 3 + 2].flX = (float)nScreenX;
				sgatWallGrid[nStep * 3 + 2].flY = (float)nScreenY;
			}
			GlidePerspectiveProject(nMidX, nMidY, nMidZ - nDrop, &nScreenX, &nScreenY);
			sgatWallGrid[nStep * 3 + 1].flX = (float)nScreenX;
			sgatWallGrid[nStep * 3 + 1].flY = (float)nScreenY;
			for (j = 0; j < 3; j++)
			{
				sgatWallGrid[nStep * 3 + j].flX = fShift + sgatWallGrid[nStep * 3 + j].flX;
				GlideSetVertexLight(&sgatWallGrid[nStep * 3 + j], adwColor[j]);
			}
		}
		grDrawVertexArray(4, 10, sgaptWallStrip0);
		grDrawVertexArray(4, 10, sgaptWallStrip1);
		fDrawn = TRUE;
	}
	return fDrawn;
}

BOOL __fastcall GlideDrawWallOpaque(GLTILE *pTile, int nX, int nY, const DWORD *pdwLight, int nBoundary)
{
	BOOL fDrawn;

	fDrawn = FALSE;
	if (D2CMPTileCacheLoadTile(pTile, 1))
	{
		GlideSetColorCombine(1);
		GlideSetAlphaMode(1);
		GlideSetFilterMode(1);
		if (GlideGetPerspective())
			fDrawn = GlideWallDrawLitPerspective(pTile, nX, nY, pdwLight, nBoundary);
		else
			fDrawn = GlideWallDrawLit(pTile, nX, nY, pdwLight, nBoundary);
	}
	return fDrawn;
}
