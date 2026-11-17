#define D2_THIS_FILE "C:\\D2\\Source\\D2Glide\\Src\\glEngine.cpp"
#include "glCore.h"
#include "StormImports.h"
#include "D2GlideImports.h"
#include "D2Macros.h"

struct GLTILE;

void __fastcall GlideDrawLine(int nX1, int nY1, int nX2, int nY2, BYTE bColor, BYTE bAlpha);
void __fastcall GlideFillSpan(int nX1, int nY1, int nX2, int nY2, BYTE bColor, int nMode);

static inline int GlideClampTo(int n, int nMax)
{
	n = n <= 0 ? 0 : n;
	if (n < nMax)
		return n;
	return nMax;
}

void __fastcall GlideFillSpan(int nX1, int nY1, int nX2, int nY2, BYTE bColor, int nMode)
{
	GLVERTEX atVertex[4];
	DWORD dwColor;

	nX1 = GlideClampTo(nX1, gnScreenWidth - 1);
	nX2 = GlideClampTo(nX2, gnScreenWidth - 1);
	if (nX1 == nX2)
		return;
	nY1 = GlideClampTo(nY1, gnScreenHeight - 1);
	nY2 = GlideClampTo(nY2, gnScreenHeight - 1);
	if (nY2 == nY1)
		return;
#line 41
	D2ASSERT(nY2 > nY1);
	GlideSetColorCombine(2);
	GlideSetAlphaMode(4);
	GlideSetFilterMode(1);
	grChromakeyMode(0);
	atVertex[0].flX = (float)nX1;
	atVertex[0].flY = (float)nY1;
	atVertex[0].field_0c = 1.0f;
	atVertex[0].flS = 0.0f;
	atVertex[0].flT = 0.0f;
	atVertex[1].flX = (float)nX1;
	atVertex[1].flY = (float)nY2;
	atVertex[1].field_0c = 1.0f;
	atVertex[1].flS = 0.0f;
	atVertex[1].flT = 0.0f;
	atVertex[2].flX = (float)nX2;
	atVertex[2].flY = (float)nY2;
	atVertex[2].field_0c = 1.0f;
	atVertex[2].flS = 0.0f;
	atVertex[2].flT = 0.0f;
	atVertex[3].flX = (float)nX2;
	atVertex[3].flY = (float)nY1;
	atVertex[3].field_0c = 1.0f;
	atVertex[3].flS = 0.0f;
	atVertex[3].flT = 0.0f;
	dwColor = GlidePaletteColor(bColor) & 0xFFFFFF00;
	switch (nMode)
	{
	case 0:
		dwColor += 64;
		break;

	case 1:
		dwColor += 128;
		break;

	case 2:
		dwColor += 192;
		break;

	default:
		dwColor += 255;
		break;
	}
	grConstantColorValue(dwColor);
	grDrawVertexArrayContiguous(5, 4, atVertex, sizeof(GLVERTEX));
	grChromakeyMode(1);
}

void __fastcall GlideFillSpanAlpha(int nX1, int nY1, int nX2, int nY2, BYTE bColor, BYTE bAlpha)
{
	GLVERTEX atVertex[4];

	nX1 = GlideClampTo(nX1, gnScreenWidth - 1);
	nX2 = GlideClampTo(nX2, gnScreenWidth - 1);
	if (nX1 == nX2)
		return;
	nY1 = GlideClampTo(nY1, gnScreenHeight - 1);
	nY2 = GlideClampTo(nY2, gnScreenHeight - 1);
	if (nY2 == nY1)
		return;
#line 98
	D2ASSERT(nY2 > nY1);
	GlideSetColorCombine(2);
	GlideSetAlphaMode(4);
	GlideSetFilterMode(1);
	grChromakeyMode(0);
	atVertex[0].flX = (float)nX1;
	atVertex[0].flY = (float)nY1;
	atVertex[0].field_0c = 1.0f;
	atVertex[0].flS = 0.0f;
	atVertex[0].flT = 0.0f;
	atVertex[1].flX = (float)nX1;
	atVertex[1].flY = (float)nY2;
	atVertex[1].field_0c = 1.0f;
	atVertex[1].flS = 0.0f;
	atVertex[1].flT = 0.0f;
	atVertex[2].flX = (float)nX2;
	atVertex[2].flY = (float)nY2;
	atVertex[2].field_0c = 1.0f;
	atVertex[2].flS = 0.0f;
	atVertex[2].flT = 0.0f;
	atVertex[3].flX = (float)nX2;
	atVertex[3].flY = (float)nY1;
	atVertex[3].field_0c = 1.0f;
	atVertex[3].flS = 0.0f;
	atVertex[3].flT = 0.0f;
	grConstantColorValue((GlidePaletteColor(bColor) & 0xFFFFFF00) | bAlpha);
	grDrawVertexArrayContiguous(5, 4, atVertex, sizeof(GLVERTEX));
	grChromakeyMode(1);
}

void __fastcall GlideDrawLine(int nX1, int nY1, int nX2, int nY2, BYTE bColor, BYTE bAlpha)
{
	GLVERTEX atVertex[2];
	DWORD dwColor;

	if (nX2 < 0 && nX1 < 0)
		return;
	if (nX2 >= gnScreenWidth && nX1 >= gnScreenWidth)
		return;
	if (nY2 < 0 && nY1 < 0)
		return;
	if (nY2 >= gnScreenHeight && nY1 >= gnScreenHeight)
		return;
	dwColor = GlidePaletteColor(bColor);
	GlideSetColorCombine(2);
	if (bAlpha == 255)
		GlideSetAlphaMode(1);
	else
	{
		GlideSetAlphaMode(4);
		dwColor = (dwColor & 0xFFFFFF00) | bAlpha;
	}
	GlideSetFilterMode(1);
	grChromakeyMode(0);
	grConstantColorValue(dwColor);
	if (nX1 == nX2 && nY1 == nY2)
	{
		atVertex[0].flX = (float)nX1;
		atVertex[0].flY = (float)nY1;
		atVertex[0].field_0c = 1.0f;
		atVertex[0].flS = 0.0f;
		atVertex[0].flT = 0.0f;
		grDrawPoint(&atVertex[0]);
		grChromakeyMode(1);
		return;
	}
	atVertex[0].flX = (float)nX1;
	atVertex[0].flY = (float)nY1;
	atVertex[0].field_0c = 1.0f;
	atVertex[0].flS = 0.0f;
	atVertex[0].flT = 0.0f;
	atVertex[1].flX = (float)nX2;
	atVertex[1].flY = (float)nY2;
	atVertex[1].field_0c = 1.0f;
	atVertex[1].flS = 0.0f;
	atVertex[1].flT = 0.0f;
	grDrawLine(&atVertex[0], &atVertex[1]);
	grChromakeyMode(1);
}

void __fastcall GlideDrawDiamond(const RECT *prc, BYTE bColor)
{
	int nWidth;
	int nHeight;
	int nCenterX;
	int nCenterY;
	int nRight;
	int nBottom;
	int nLeft;
	int nTop;

#line 188
	D2ASSERT(prc->left <= prc->right);
#line 189
	D2ASSERT(prc->top <= prc->bottom);
	nLeft = prc->left;
	nTop = prc->top;
	nWidth = prc->right - nLeft;
	nHeight = prc->bottom - nTop;
	if (!nWidth || !nHeight)
		return;
	nCenterX = ((DWORD)nWidth >> 1) + nLeft;
	nCenterY = ((DWORD)nHeight >> 1) + nTop;
	nRight = nWidth + nLeft;
	nBottom = nHeight + nTop;
	if (nRight < 0 || nLeft > gnScreenWidth || nBottom < 0 || nTop > gnScreenHeight)
		return;
	GlideDrawLine(nCenterX, nTop, nRight, nCenterY, bColor, 255);
	GlideDrawLine(nRight, nCenterY, nCenterX, nBottom, bColor, 255);
	GlideDrawLine(nCenterX, nBottom, nLeft, nCenterY, bColor, 255);
	GlideDrawLine(nLeft, nCenterY, nCenterX, nTop, bColor, 255);
}

void __fastcall GlideDrawFrame(const RECT *prc, BYTE bColor)
{
	int nLeft;
	int nRight;
	int nY1;
	int nY2;

#line 226
	D2ASSERT(prc->left <= prc->right);
#line 227
	D2ASSERT(prc->top <= prc->bottom);
	nLeft = GlideClampTo(prc->left, gnScreenWidth - 1);
	nRight = GlideClampTo(prc->right, gnScreenWidth - 1);
	if (nLeft == nRight)
		return;
	nY1 = GlideClampTo(prc->top, gnScreenHeight - 1);
	nY2 = GlideClampTo(prc->bottom, gnScreenHeight - 1);
	if (nY2 == nY1)
		return;
#line 243
	D2ASSERT(nY2 > nY1);
	GlideDrawLine(nLeft, nY1, nRight, nY1, bColor, 255);
	GlideDrawLine(nRight, nY1, nRight, nY2, bColor, 255);
	GlideDrawLine(nRight, nY2, nLeft, nY2, bColor, 255);
	GlideDrawLine(nLeft, nY2, nLeft, nY1, bColor, 255);
}

void __fastcall GlideFillRect(const RECT *prc, BYTE bColor)
{
#line 259
	D2ASSERT(prc->left <= prc->right);
#line 260
	D2ASSERT(prc->top <= prc->bottom);
	GlideFillSpan(prc->left, prc->top, prc->right, prc->bottom, bColor, 5);
}

void __fastcall GlideFillSquare(const POINT *ptCenter, BYTE bSize, BYTE bColor)
{
	RECT rc;

	if (!bSize)
		return;
	bSize--;
	SetRect(&rc, ptCenter->x - bSize, ptCenter->y - bSize, ptCenter->x + bSize + 1, ptCenter->y + bSize + 1);
	GlideFillSpan(rc.left, rc.top, rc.right, rc.bottom, bColor, 5);
}

void __fastcall GlideEngineClear(BOOL fAbovePanel)
{
	grBufferClear(0, 0, 0);
}
