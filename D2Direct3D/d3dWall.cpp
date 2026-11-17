#define D2_THIS_FILE "C:\\D2\\Source\\D2Direct3D\\Src\\d3dWall.cpp"
#include "d3dCaches.h"
#include "StormImports.h"
#include "D2Direct3DImports.h"
#include "D2Macros.h"

#define NUMBER_OF_SUPPORTED_TILE_TYPES	10

static D3DTLVERTEX sgatWallGrid[15] = { 0 };
static D3DTLVERTEX sgatWallQuad[4] = { 0 };
static WORD sgawWallStrip0[10] = { 0, 1, 3, 4, 6, 7, 9, 10, 12, 13 };
static WORD sgawWallStrip1[10] = { 1, 2, 4, 5, 7, 8, 10, 11, 13, 14 };
struct D3DWALLCORNER
{
 	int			nX;
 	int			nY;
 	int			nHeight;
};
static D3DWALLCORNER sgatWallCorner[NUMBER_OF_SUPPORTED_TILE_TYPES][6] =
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

void WINAPI D3DWallStartup(void)
{
	D3DWALLCORNER *ptCorner;
	int nType;
	int nColumn;
	int i;
	int j;

	sgatWallQuad[0].flX = 0.0f;
	sgatWallQuad[0].flY = 0.0f;
	sgatWallQuad[0].field_08 = 0.5f;
	sgatWallQuad[0].field_0c = 1.0f;
	sgatWallQuad[0].flU = 0.0f;
	sgatWallQuad[0].flV = 0.0f;
	sgatWallQuad[1].flX = 0.0f;
	sgatWallQuad[1].flY = 0.0f;
	sgatWallQuad[1].field_08 = 0.5f;
	sgatWallQuad[1].field_0c = 1.0f;
	sgatWallQuad[1].flU = 0.0f;
	sgatWallQuad[1].flV = 1.0f;
	sgatWallQuad[2].flX = 0.0f;
	sgatWallQuad[2].flY = 0.0f;
	sgatWallQuad[2].field_08 = 0.5f;
	sgatWallQuad[2].field_0c = 1.0f;
	sgatWallQuad[2].flU = 1.0f;
	sgatWallQuad[2].flV = 1.0f;
	sgatWallQuad[3].flX = 0.0f;
	sgatWallQuad[3].flY = 0.0f;
	sgatWallQuad[3].field_08 = 0.5f;
	sgatWallQuad[3].field_0c = 1.0f;
	sgatWallQuad[3].flU = 1.0f;
	sgatWallQuad[3].flV = 0.0f;
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 3; j++)
		{
			sgatWallGrid[i * 3 + j].field_08 = 0.5f;
			sgatWallGrid[i * 3 + j].field_0c = 1.0f;
			sgatWallGrid[i * 3 + j].flU = (j * 0.984375f) * 0.5f + 0.015625f;
			sgatWallGrid[i * 3 + j].flV = (i * 0.984375f) * 0.25f + 0.015625f;
		}
	}
	for (nType = 0; nType < NUMBER_OF_SUPPORTED_TILE_TYPES; nType++)
	{
		for (nColumn = 0; nColumn < 6; nColumn++)
		{
			ptCorner = &sgatWallCorner[nType][nColumn];
			ptCorner->nX <<= 16;
			ptCorner->nY <<= 16;
		}
	}
}

static inline void D3DWallPlaceQuad(int nLeft, int nTop, int nRight, int nBottom)
{
	sgatWallQuad[0].flX = (float)nLeft;
	sgatWallQuad[0].flY = (float)nTop;
	sgatWallQuad[1].flX = (float)nLeft;
	sgatWallQuad[1].flY = (float)nBottom;
	sgatWallQuad[2].flX = (float)nRight;
	sgatWallQuad[2].flY = (float)nBottom;
	sgatWallQuad[3].flX = (float)nRight;
	sgatWallQuad[3].flY = (float)nTop;
}

static inline BOOL D3DWallOnScreen(const D3DTLVERTEX *ptVertex, float fLeft, int nRight)
{
	return ptVertex->flX >= fLeft && nRight > ptVertex->flX && ptVertex->flY >= 0.0f
		&& ptVertex->flY < 433.0f;
}

static inline float D3DVertexX(const D3DTLVERTEX *ptVertex)
{
	return ptVertex->flX;
}

static inline float D3DVertexY(const D3DTLVERTEX *ptVertex)
{
	return ptVertex->flY;
}

static inline BOOL D3DWallCornerOnScreen(const D3DTLVERTEX *ptVertex, float fLeft, int nRight)
{
	return D3DVertexX(ptVertex) >= fLeft && D3DVertexX(ptVertex) < nRight && D3DVertexY(ptVertex) >= 0.0f
		&& D3DVertexY(ptVertex) < 433.0f;
}

BOOL __fastcall D3DWallDrawFlat(D3DTILE *pTile, int nX, int nY, BYTE bLight, BYTE bAlpha, int nBoundary)
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
		D3DBindTileComp(&pTile->ptCompArray[i]);
		D3DWallPlaceQuad(nPartX, nPartTop, nPartRight, nPartY);
		D3DSetVertexLightRGB(&sgatWallQuad[0], bLight, bAlpha);
		sgatWallQuad[1].dwColor = sgatWallQuad[0].dwColor;
		sgatWallQuad[2].dwColor = sgatWallQuad[0].dwColor;
		sgatWallQuad[3].dwColor = sgatWallQuad[0].dwColor;
		glpDevice->lpVtbl->field_70(glpDevice, 6, 452, sgatWallQuad, 4, 28);
	}
	return fDrawn;
}

BOOL __fastcall D3DWallDrawFlatPerspective(D3DTILE *pTile, int nX, int nY, BYTE bLight, BYTE bAlpha,
	int nBoundary)
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
		D3DPerspectiveProject(nWorldX + nX, nWorldY + nY, 0, &nScreenX, &nScreenY);
		sgatWallGrid[0].flX = (float)nScreenX;
		sgatWallGrid[0].flY = (float)nScreenY;
		D3DPerspectiveProject(nWorldX + nX + 131072, nWorldY + nY + 131072, 0, &nScreenX, &nScreenY);
		sgatWallGrid[12].flX = (float)nScreenX;
		sgatWallGrid[12].flY = (float)nScreenY;
		D3DPerspectiveProject(nWorldX + nX + 196608, nWorldY + nY + 65536, 0, &nScreenX, &nScreenY);
		sgatWallGrid[14].flX = (float)nScreenX;
		sgatWallGrid[14].flY = (float)nScreenY;
		D3DPerspectiveProject(nWorldX + nX + 65536, nWorldY + nY - 65536, 0, &nScreenX, &nScreenY);
		sgatWallGrid[2].flX = (float)nScreenX;
		sgatWallGrid[2].flY = (float)nScreenY;
		fLeft = (float)nLeft;
		if (!D3DWallOnScreen(&sgatWallGrid[0], fLeft, nRight) && !D3DWallOnScreen(&sgatWallGrid[12], fLeft,
			nRight)
			&& !D3DWallOnScreen(&sgatWallGrid[14], fLeft, nRight) && !D3DWallOnScreen(&sgatWallGrid[2], fLeft,
			nRight))
			continue;
		D3DBindTileComp(&pTile->ptCompArray[i]);
		fShift = (float)nShift;
		for (nLine = 0; nLine < 5; nLine++)
		{
			nPointX = nWorldX + nX + nLine * 32768;
			nPointY = nWorldY + nY + nLine * 32768;
			for (j = 0; j < 3; j++, nPointX += 32768, nPointY -= 32768)
			{
				D3DPerspectiveProject(nPointX, nPointY, 0, &nScreenX, &nScreenY);
				sgatWallGrid[nLine * 3 + j].flY = (float)nScreenY;
				sgatWallGrid[nLine * 3 + j].flX = (float)nScreenX + fShift;
				D3DSetVertexLightRGB(&sgatWallGrid[nLine * 3 + j], bLight, bAlpha);
			}
		}
		glpDevice->lpVtbl->field_74(glpDevice, 5, 452, sgatWallGrid, 15, sgawWallStrip0, 10, 28);
		glpDevice->lpVtbl->field_74(glpDevice, 5, 452, sgatWallGrid, 15, sgawWallStrip1, 10, 28);
		fDrawn = TRUE;
	}
	return fDrawn;
}

BOOL __fastcall D3DDrawWallLight(D3DTILE *pTile, int nX, int nY, BYTE bLight, int nBoundary)
{
	BYTE bAlpha;
	BOOL fDrawn;

	fDrawn = FALSE;
	if (!(!D2CMPTileCacheLoadTile(pTile, 1)))
	{
		bAlpha = 255;
		D3DSetBlendMode(1);
		if (D3DGetDrawCtxFlag())
		{
			D3DSetAlphaMode(4);
			bAlpha = 192;
		} else
			D3DSetAlphaMode(1);
		D3DSetFilterMode(1);
		if (D3DGetPerspective())
			fDrawn = D3DWallDrawFlatPerspective(pTile, nX, nY, bLight, bAlpha, nBoundary);
		else
			fDrawn = D3DWallDrawFlat(pTile, nX, nY, bLight, bAlpha, nBoundary); }
	return fDrawn;
}

static inline BOOL D3DWallDrawLit(D3DTILE *pTile, int nX, int nY, const DWORD *pdwLight, BYTE bAlpha,
	int nBoundary)
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
		D3DBindTileComp(&pTile->ptCompArray[i]);
		D3DWallPlaceQuad(nPartX, nPartTop, nPartRight, nPartY);
		D3DSetVertexLight(&sgatWallQuad[0], dwLeft, bAlpha);
		D3DSetVertexLight(&sgatWallQuad[1], dwLeft, bAlpha);
		D3DSetVertexLight(&sgatWallQuad[2], dwRight, bAlpha);
		D3DSetVertexLight(&sgatWallQuad[3], dwRight, bAlpha);
		glpDevice->lpVtbl->field_70(glpDevice, 6, 452, sgatWallQuad, 4, 28);
	}
	return fDrawn;
}

static BOOL __fastcall D3DWallDrawLitPerspective(D3DTILE *pTile, int nX, int nY, const DWORD *pdwLight, BYTE bAlpha,
	int nBoundary);

BOOL __fastcall D3DDrawWallLit(D3DTILE *pTile, int nX, int nY, const DWORD *pdwLight, int nBoundary,
	BYTE bAlpha)
{
	BOOL fDrawn;

	fDrawn = FALSE;
	if (D2CMPTileCacheLoadTile(pTile, 1))
	{
		D3DSetBlendMode(1);
		if (bAlpha == 255) D3DSetAlphaMode(1); else D3DSetAlphaMode(4);
		D3DSetFilterMode(1);
		if (D3DGetPerspective())
			fDrawn = D3DWallDrawLitPerspective(pTile, nX, nY, pdwLight, bAlpha, nBoundary);
		else
			fDrawn = D3DWallDrawLit(pTile, nX, nY, pdwLight, bAlpha, nBoundary);
	}
	return fDrawn;
}

extern int PLACEHOLDER_SymbolCounterPad00; extern int PLACEHOLDER_SymbolCounterPad01; extern int PLACEHOLDER_SymbolCounterPad02; extern int PLACEHOLDER_SymbolCounterPad03; extern int PLACEHOLDER_SymbolCounterPad04; extern int PLACEHOLDER_SymbolCounterPad05; extern int PLACEHOLDER_SymbolCounterPad06; extern int PLACEHOLDER_SymbolCounterPad07;
extern int PLACEHOLDER_SymbolCounterPad08; extern int PLACEHOLDER_SymbolCounterPad09; extern int PLACEHOLDER_SymbolCounterPad10; extern int PLACEHOLDER_SymbolCounterPad11; extern int PLACEHOLDER_SymbolCounterPad12; extern int PLACEHOLDER_SymbolCounterPad13; extern int PLACEHOLDER_SymbolCounterPad14; extern int PLACEHOLDER_SymbolCounterPad15;
extern int PLACEHOLDER_SymbolCounterPad16; extern int PLACEHOLDER_SymbolCounterPad17; extern int PLACEHOLDER_SymbolCounterPad18; extern int PLACEHOLDER_SymbolCounterPad19; extern int PLACEHOLDER_SymbolCounterPad20; extern int PLACEHOLDER_SymbolCounterPad21; extern int PLACEHOLDER_SymbolCounterPad22; extern int PLACEHOLDER_SymbolCounterPad23;

static BOOL __fastcall D3DWallDrawLitPerspective(D3DTILE *pTile, int nX, int nY, const DWORD *pdwLight, BYTE bAlpha,
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
	int nPartY;
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
#line 446
	D2ASSERT(nType >= 0 && nType <NUMBER_OF_SUPPORTED_TILE_TYPES);
	for (i = 0; i < pTile->nCompCount; i++)
	{
		nOffsetX = pTile->ptCompArray[i].sOffsetX >> 5;
		nPartY = pTile->ptCompArray[i].sOffsetY;
#line 454
		D2ASSERT(nOffsetX >= 0 && nOffsetX < 5);
		nLeftX = sgatWallCorner[nType][nOffsetX].nX + nX;
		nLeftY = sgatWallCorner[nType][nOffsetX].nY + nY;
		nLeftZ = sgatWallCorner[nType][nOffsetX].nHeight - nPartY;
		nRightX = sgatWallCorner[nType][nOffsetX + 1].nX + nX;
		nRightY = sgatWallCorner[nType][nOffsetX + 1].nY + nY;
		nRightZ = sgatWallCorner[nType][nOffsetX + 1].nHeight - nPartY;
		D3DPerspectiveProject(nLeftX, nLeftY, nLeftZ, &nScreenX, &nScreenY);
		sgatWallGrid[0].flX = (float)nScreenX;
		sgatWallGrid[0].flY = (float)nScreenY;
		D3DPerspectiveProject(nLeftX, nLeftY, nLeftZ - 32, &nScreenX, &nScreenY);
		sgatWallGrid[12].flX = (float)nScreenX;
		sgatWallGrid[12].flY = (float)nScreenY;
		D3DPerspectiveProject(nRightX, nRightY, nRightZ - 32, &nScreenX, &nScreenY);
		sgatWallGrid[14].flX = (float)nScreenX;
		sgatWallGrid[14].flY = (float)nScreenY;
		D3DPerspectiveProject(nRightX, nRightY, nRightZ, &nScreenX, &nScreenY);
		sgatWallGrid[2].flX = (float)nScreenX;
		sgatWallGrid[2].flY = (float)nScreenY;
		fLeft = (float)nLeft;
		if (!D3DWallCornerOnScreen(&sgatWallGrid[0], fLeft, nRight)
			&& !D3DWallCornerOnScreen(&sgatWallGrid[12], fLeft, nRight)
			&& !D3DWallCornerOnScreen(&sgatWallGrid[14], fLeft, nRight)
			&& !D3DWallCornerOnScreen(&sgatWallGrid[2], fLeft, nRight))
			continue;
		nMidY = (nRightY + nLeftY) >> 1;
		nMidZ = (nLeftZ + nRightZ) >> 1;
		nMidX = (nRightX + nLeftX) >> 1;
		adwColor[0] = pdwLight[nOffsetX];
		adwColor[2] = pdwLight[nOffsetX + 1];
		dwLeft = adwColor[0];
		dwRight = adwColor[2];
		adwColor[1] = (BYTE)(((BYTE)(dwRight >> 24) + (BYTE)(dwLeft >> 24)) >> 1);
		adwColor[1] = (adwColor[1] << 8) + (BYTE)(((BYTE)(dwRight >> 16) + (BYTE)(dwLeft >> 16)) >> 1);
		adwColor[1] = (adwColor[1] << 8) + (BYTE)(((BYTE)(dwRight >> 8) + (BYTE)(dwLeft >> 8)) >> 1);
		adwColor[1] = (adwColor[1] << 8) + (BYTE)(((BYTE)dwRight + (BYTE)dwLeft) >> 1);
		D3DBindTileComp(&pTile->ptCompArray[i]);
		fShift = (float)nShift;
		for (nStep = 0; nStep < 5; nStep++)
		{
			nDrop = nStep << 3;
			if (nStep != 0 && nStep != 4)
			{
				D3DPerspectiveProject(nLeftX, nLeftY, nLeftZ - nDrop, &nScreenX, &nScreenY);
				sgatWallGrid[nStep * 3].flX = (float)nScreenX;
				sgatWallGrid[nStep * 3].flY = (float)nScreenY;
				D3DPerspectiveProject(nRightX, nRightY, nRightZ - nDrop, &nScreenX, &nScreenY);
				sgatWallGrid[nStep * 3 + 2].flX = (float)nScreenX;
				sgatWallGrid[nStep * 3 + 2].flY = (float)nScreenY;
			}
			D3DPerspectiveProject(nMidX, nMidY, nMidZ - nDrop, &nScreenX, &nScreenY);
			sgatWallGrid[nStep * 3 + 1].flX = (float)nScreenX;
			sgatWallGrid[nStep * 3 + 1].flY = (float)nScreenY;
			for (j = 0; j < 3; j++)
			{
				sgatWallGrid[nStep * 3 + j].flX = fShift + sgatWallGrid[nStep * 3 + j].flX;
				D3DSetVertexLight(&sgatWallGrid[nStep * 3 + j], adwColor[j], bAlpha);
			}
		}
		glpDevice->lpVtbl->field_74(glpDevice, 5, 452, sgatWallGrid, 15, sgawWallStrip0, 10, 28);
		glpDevice->lpVtbl->field_74(glpDevice, 5, 452, sgatWallGrid, 15, sgawWallStrip1, 10, 28);
		fDrawn = TRUE;
	}
	return fDrawn;
}

BOOL __fastcall D3DDrawWallOpaque(D3DTILE *pTile, int nX, int nY, const DWORD *pdwLight, int nBoundary)
{
	BOOL fDrawn;

	fDrawn = FALSE;
	if (D2CMPTileCacheLoadTile(pTile, 1))
	{
		D3DSetBlendMode(1);
		D3DSetAlphaMode(1);
		D3DSetFilterMode(1);
		if (D3DGetPerspective())
			fDrawn = D3DWallDrawLitPerspective(pTile, nX, nY, pdwLight, 255, nBoundary);
		else
			fDrawn = D3DWallDrawLit(pTile, nX, nY, pdwLight, 255, nBoundary);
	}
	return fDrawn;
}
