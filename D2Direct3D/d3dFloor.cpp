#include "d3dCaches.h"
#include "D2Direct3DImports.h"

struct D3DTILEPOINT
{
 	int			nLightRow;
 	BYTE		bRed;
 	BYTE		bGreen;
 	BYTE		bBlue;
 	BYTE		unobserved_07;
 	int			nX;
 	int			nY;
};

static WORD sgawTileTriangles[150] = { 0 };
static WORD sgawTileStrips[5][12] = { 0 };
static D3DVERTEXBUFFER *sglpTileVertices = 0;
static DWORD PLACEHOLDER_UnreferencedData_10027914 = 0;
static WORD sgawTileCorners[4] = { 0, 5, 35, 30 };

void WINAPI D3DEngineOpen(void)
{
	D3DVBDESC tDesc = {0};
	D3DTLVERTEX *ptVertex;
	HRESULT hResult;
	float fScaleU;
	float fScaleV;
	float fU, fV;
	int nEdgeU;
	int nEdgeV;
	int i;
	int j;
	int k;
	int nIndex;

	tDesc.field_04 = 0;
	fScaleU = 160.0f / gnFloorCells * 0.1f;
	D3D_FLOAT_STORE(fScaleU);
	tDesc.dwSize = sizeof(tDesc);
	tDesc.field_08 = 452;
	tDesc.dwNumVertices = 36;
	fScaleV = 80.0f / gnFloorRows * 0.1f;
	D3D_FLOAT_STORE(fScaleV);
	hResult = glpD3D->lpVtbl->field_24(glpD3D, &tDesc, &sglpTileVertices, 4, NULL);
	if (hResult)
	{
		D3DLogError("CreateVertexBuffer failed!", hResult);
		return;
	}
	hResult = sglpTileVertices->lpVtbl->field_0c(sglpTileVertices, 33, &ptVertex, NULL);
	if (hResult)
	{
		D3DLogDDError("Vertex buffer lock failed!", hResult);
		return;
	}
	for (j = 0; j < 6; j++)
		for (i = 0; i < 6; i++)
		{
			nEdgeU = 0;
			nEdgeV = 0;
			if (i == 0)
			{
				nEdgeU = 1;
				nEdgeV = 1;
			}
			else if (i == 5)
			{
				nEdgeU = -1;
				nEdgeV = -1;
			}
			if (j == 0)
			{
				nEdgeU--;
				nEdgeV++;
			}
			else if (j == 5)
			{
				nEdgeU++;
				nEdgeV--;
			}
			fU = (float)(nEdgeU + 80) / gnFloorCells + (float)(i - j) * fScaleU;
			fV = (float)nEdgeV / gnFloorRows + (float)(i + j) * fScaleV;
			ptVertex->field_08 = 0.5f;
			ptVertex->field_0c = 1.0f;
			ptVertex->flU = fU; ptVertex->flV = fV;
			ptVertex++;
		}
	hResult = sglpTileVertices->lpVtbl->field_10(sglpTileVertices);
	if (hResult)
	{
		D3DLogDDError("Vertex buffer unlock failed!", hResult);
		return;
	}
	for (j = 0; j < 5; j++)
		for (i = 0; i < 6; i++)
		{
			sgawTileStrips[j][i * 2] = (WORD)(i + j * 6);
			sgawTileStrips[j][i * 2 + 1] = (WORD)(i + (j + 1) * 6);
		}
	nIndex = 0;
	for (j = 0; j < 5; j++)
		for (k = 0; k < 10; k++)
		{
			sgawTileTriangles[nIndex++] = sgawTileStrips[j][k];
			sgawTileTriangles[nIndex++] = sgawTileStrips[j][k + 1];
			sgawTileTriangles[nIndex++] = sgawTileStrips[j][k + 2];
		}
}

void WINAPI D3DEngineClose(void)
{
	if (sglpTileVertices)
	{
		sglpTileVertices->lpVtbl->field_08(sglpTileVertices);
		sglpTileVertices = NULL;
	}
}

void __fastcall D3DDrawTile(D3DTILE *pTile, const D3DTILEPOINT *ptPoints, DWORD dwX, DWORD dwY,
	DWORD dwTileX, DWORD dwTileY, BYTE bAlpha, DWORD dwBoundaryMode, int nFilter)
{
	D3DTLVERTEX *ptVertices;
	const BYTE *pbRow;
	HRESULT hResult;
	int i;

	if (!D3DBindTile(pTile))
		return;
	D3DSetBlendMode(1);
	if (bAlpha == 255) D3DSetAlphaMode(1); else D3DSetAlphaMode(4);
	if (nFilter == 1) D3DSetFilterMode(2); else D3DSetFilterMode(1);
	hResult = sglpTileVertices->lpVtbl->field_0c(sglpTileVertices, 33, &ptVertices, NULL);
	if (hResult)
	{
		D3DLogDDError("Vertex buffer lock failed!", hResult);
		return;
	}
	if (gptDrawCtx->dwFlatLighting)
	{
		pbRow = gabLightTable[ptPoints[0].nLightRow];
		ptVertices[0].dwColor = (((((DWORD)bAlpha << 8) | pbRow[ptPoints[0].bRed]) << 8
			| pbRow[ptPoints[0].bGreen]) << 8) | pbRow[ptPoints[0].bBlue];
		ptVertices[0].field_14 = 0;
		ptVertices[0].flX = (float)ptPoints[0].nX;
		ptVertices[0].flY = (float)ptPoints[0].nY;
		pbRow = gabLightTable[ptPoints[5].nLightRow];
		ptVertices[5].dwColor = (((((DWORD)bAlpha << 8) | pbRow[ptPoints[5].bRed]) << 8
			| pbRow[ptPoints[5].bGreen]) << 8) | pbRow[ptPoints[5].bBlue];
		ptVertices[5].field_14 = 0;
		ptVertices[5].flX = (float)ptPoints[5].nX;
		ptVertices[5].flY = (float)ptPoints[5].nY;
		pbRow = gabLightTable[ptPoints[35].nLightRow];
		ptVertices[35].dwColor = (((((DWORD)bAlpha << 8) | pbRow[ptPoints[35].bRed]) << 8
			| pbRow[ptPoints[35].bGreen]) << 8) | pbRow[ptPoints[35].bBlue];
		ptVertices[35].field_14 = 0;
		ptVertices[35].flX = (float)ptPoints[35].nX;
		ptVertices[35].flY = (float)ptPoints[35].nY;
		pbRow = gabLightTable[ptPoints[30].nLightRow];
		ptVertices[30].dwColor = (((((DWORD)bAlpha << 8) | pbRow[ptPoints[30].bRed]) << 8
			| pbRow[ptPoints[30].bGreen]) << 8) | pbRow[ptPoints[30].bBlue];
		ptVertices[30].field_14 = 0;
		ptVertices[30].flX = (float)ptPoints[30].nX;
		ptVertices[30].flY = (float)ptPoints[30].nY;
		hResult = sglpTileVertices->lpVtbl->field_10(sglpTileVertices);
		if (hResult)
		{
			D3DLogDDError("Vertex buffer unlock failed!", hResult);
			return;
		}
		glpDevice->lpVtbl->field_8c(glpDevice, 6, sglpTileVertices, sgawTileCorners, 4, 28);
		return;
	}
	for (i = 0; i < 36; i++)
	{
		pbRow = gabLightTable[ptPoints[i].nLightRow];
		ptVertices->dwColor = (((((DWORD)bAlpha << 8) | pbRow[ptPoints[i].bRed]) << 8
			| pbRow[ptPoints[i].bGreen]) << 8) | pbRow[ptPoints[i].bBlue];
		ptVertices->field_14 = 0;
		ptVertices->flX = (float)ptPoints[i].nX;
		ptVertices->flY = (float)ptPoints[i].nY;
		ptVertices++;
	}
	hResult = sglpTileVertices->lpVtbl->field_10(sglpTileVertices);
	if (hResult)
	{
		D3DLogDDError("Vertex buffer unlock failed!", hResult);
		return;
	}
	glpDevice->lpVtbl->field_8c(glpDevice, 4, sglpTileVertices, sgawTileTriangles, 150, 28);
}
