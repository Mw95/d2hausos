#define D2_THIS_FILE "C:\\D2\\Source\\D2Glide\\Src\\glFloor.cpp"
#include "glCore.h"
#include "D2GlideImports.h"

struct GLTILE;

struct GLTILEPOINT
{
 	int			nLightRow;
 	BYTE		bRed;
 	BYTE		bGreen;
 	BYTE		bBlue;
 	BYTE		unobserved_07;
 	int			nX;
 	int			nY;
};

#define GL_GRID_SCALE_T	128.0f
#define GL_GRID_STEP_T	0.0078125f
#define GL_GRID_SCALE_S	256.0f
#define GL_GRID_STEP_S	0.00390625f
#define GL_GRID_CELL	0.0625f

static GLVERTEX sgatTileGrid[36] = { 0 };
static GLVERTEX *sgaptTileStrips[5][12] = { 0 };
static GLVERTEX *sgaptTileCorners[4] =
{
	&sgatTileGrid[0], &sgatTileGrid[5], &sgatTileGrid[35], &sgatTileGrid[30]
};

void __fastcall GlideFloorGridInit(void)
{
	GLVERTEX *ptVertex;
	int nP;
	int nQ;
	int a;
	int b;
	int r;
	int c;

	ptVertex = sgatTileGrid;
	for (b = 0; b < 6; b++)
	{
		for (a = 0; a < 6; a++)
		{
			nP = nQ = 0;
			if (!a)
				nQ = nP = 1;
			else if (a == 5)
				nQ = nP = -1;
			if (!b)
			{
				nQ--;
				nP++;
			}
			else if (b == 5)
			{
				nQ++;
				nP--;
			}
			ptVertex->field_0c = 1.0f;
			ptVertex->flS = (float)(int)(((float)(a - b) * GL_GRID_CELL
				+ (float)(nQ + 80) * GL_GRID_STEP_S) * GL_GRID_SCALE_S);
			ptVertex->flT = (float)(int)(((float)(a + b) * GL_GRID_CELL + (float)nP * GL_GRID_STEP_T)
				* GL_GRID_SCALE_T);
			ptVertex++;
		}
	}
	for (r = 0; r < 5; r++)
		for (c = 0; c < 6; c++)
		{
			sgaptTileStrips[r][c * 2] = &sgatTileGrid[r * 6 + c];
			sgaptTileStrips[r][c * 2 + 1] = &sgatTileGrid[(r + 1) * 6 + c];
		}
}

static inline DWORD GlideTilePointColor(const GLTILEPOINT *ptPoint)
{
	const BYTE *pbRow;

	pbRow = gpbLightTable + (ptPoint->nLightRow << 8);
	return ((((0xFFFFFF00 | pbRow[ptPoint->bRed]) << 8) | pbRow[ptPoint->bGreen]) << 8)
		| pbRow[ptPoint->bBlue];
}

void __fastcall GlideDrawTile(GLTILE *pTile, const GLTILEPOINT *ptPoints, DWORD dwX, DWORD dwY,
	DWORD dwTileX, DWORD dwTileY, BYTE bAlpha, DWORD dwBoundaryMode, int nFilter)
{
	int i;

	if (!GlideBindTile(pTile))
		return;
	GlideSetColorCombine(1);
	if (bAlpha == 255)
		GlideSetAlphaMode(1);
	else
	{
		GlideSetAlphaMode(4);
		grConstantColorValue(bAlpha);
	}
	if (nFilter == 1) GlideSetFilterMode(2); else GlideSetFilterMode(1);
	if (gptDrawCtx->dwFlatLighting)
	{
		sgatTileGrid[0].dwColor = GlideTilePointColor(&ptPoints[0]);
		sgatTileGrid[0].flX = (float)ptPoints[0].nX;
		sgatTileGrid[0].flY = (float)ptPoints[0].nY;
		sgatTileGrid[5].dwColor = GlideTilePointColor(&ptPoints[5]);
		sgatTileGrid[5].flX = (float)ptPoints[5].nX;
		sgatTileGrid[5].flY = (float)ptPoints[5].nY;
		sgatTileGrid[35].dwColor = GlideTilePointColor(&ptPoints[35]);
		sgatTileGrid[35].flX = (float)ptPoints[35].nX;
		sgatTileGrid[35].flY = (float)ptPoints[35].nY;
		sgatTileGrid[30].dwColor = GlideTilePointColor(&ptPoints[30]);
		sgatTileGrid[30].flX = (float)ptPoints[30].nX;
		sgatTileGrid[30].flY = (float)ptPoints[30].nY;
		grDrawVertexArray(5, 4, sgaptTileCorners);
		return;
	}
	{
		GLVERTEX *ptV = sgatTileGrid;
		const GLTILEPOINT *ptP = ptPoints;

		for (i = 36; i > 0; i--)
		{
			ptV->dwColor = GlideTilePointColor(ptP);
			ptV->flX = (float)ptP->nX;
			ptV->flY = (float)ptP->nY;
			ptV++;
			ptP++;
		}
	}
	for (i = 0; i < 5; i++)
		grDrawVertexArray(4, 12, sgaptTileStrips[i]);
}
