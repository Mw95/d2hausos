#define D2_THIS_FILE "C:\\D2\\Source\\D2Direct3D\\Src\\d3dEngine.cpp"
#include "d3dCaches.h"
#include "StormImports.h"
#include "D2Direct3DImports.h"
#include "D2Macros.h"

static inline int D3DClampTo(int n, int nMax)
{
	n = n <= 0 ? 0 : n;
	if (n < nMax)
		return n;
	return nMax;
}

void __fastcall D3DFillSpan(int nX1, int nY1, int nX2, int nY2, BYTE bColor, int nMode)
{
	D3DTLVERTEX atVertex[4];
	DWORD dwColor;

	nX1 = D3DClampTo(nX1, gnScreenWidth - 1);
	nX2 = D3DClampTo(nX2, gnScreenWidth - 1);
	if (nX1 == nX2)
		return;
	nY1 = D3DClampTo(nY1, gnScreenHeight - 1);
	nY2 = D3DClampTo(nY2, gnScreenHeight - 1);
	if (nY2 == nY1)
		return;
#line 43
	D2ASSERT(nY2 > nY1);
	D3DSetBlendMode(2);
	D3DSetAlphaMode(4);
	D3DSetFilterMode(1);
	atVertex[0].flX = (float)nX1;
	atVertex[0].flY = (float)nY1;
	atVertex[0].field_08 = 0.5f;
	atVertex[0].field_0c = 1.0f;
	atVertex[0].flU = 0.0f;
	atVertex[0].flV = 0.0f;
	atVertex[1].flX = (float)nX1;
	atVertex[1].flY = (float)nY2;
	atVertex[1].field_08 = 0.5f;
	atVertex[1].field_0c = 1.0f;
	atVertex[1].flU = 0.0f;
	atVertex[1].flV = 0.0f;
	atVertex[2].flX = (float)nX2;
	atVertex[2].flY = (float)nY2;
	atVertex[2].field_08 = 0.5f;
	atVertex[2].field_0c = 1.0f;
	atVertex[2].flU = 0.0f;
	atVertex[2].flV = 0.0f;
	atVertex[3].flX = (float)nX2;
	atVertex[3].flY = (float)nY1;
	atVertex[3].field_08 = 0.5f;
	atVertex[3].field_0c = 1.0f;
	atVertex[3].flU = 0.0f;
	atVertex[3].flV = 0.0f;
	dwColor = D3DGetPaletteColor(bColor) & 16777215;
	switch (nMode)
	{
	case 0:
		dwColor |= 1073741824;
		break;

	case 1:
		dwColor |= 0x80000000;
		break;

	case 2:
		dwColor |= 0xC0000000;
		break;

	default:
		dwColor |= 0xFF000000;
		break;
	}
	atVertex[0].dwColor = dwColor;
	atVertex[1].dwColor = dwColor;
	atVertex[2].dwColor = dwColor;
	atVertex[3].dwColor = dwColor;
	glpDevice->lpVtbl->field_70(glpDevice, 6, 452, atVertex, 4, 28);
}

void __fastcall D3DFillSpanAlpha(int nX1, int nY1, int nX2, int nY2, BYTE bColor, BYTE bAlpha)
{
	D3DTLVERTEX atVertex[4];
	DWORD dwColor;

	nX1 = D3DClampTo(nX1, gnScreenWidth - 1);
	nX2 = D3DClampTo(nX2, gnScreenWidth - 1);
	if (nX1 == nX2)
		return;
	nY1 = D3DClampTo(nY1, gnScreenHeight - 1);
	nY2 = D3DClampTo(nY2, gnScreenHeight - 1);
	if (nY2 == nY1)
		return;
#line 104
	D2ASSERT(nY2 > nY1);
	D3DSetBlendMode(2);
	D3DSetAlphaMode(4);
	D3DSetFilterMode(1);
	atVertex[0].flX = (float)nX1;
	atVertex[0].flY = (float)nY1;
	atVertex[0].field_08 = 0.5f;
	atVertex[0].field_0c = 1.0f;
	atVertex[0].flU = 0.0f;
	atVertex[0].flV = 0.0f;
	atVertex[1].flX = (float)nX1;
	atVertex[1].flY = (float)nY2;
	atVertex[1].field_08 = 0.5f;
	atVertex[1].field_0c = 1.0f;
	atVertex[1].flU = 0.0f;
	atVertex[1].flV = 0.0f;
	atVertex[2].flX = (float)nX2;
	atVertex[2].flY = (float)nY2;
	atVertex[2].field_08 = 0.5f;
	atVertex[2].field_0c = 1.0f;
	atVertex[2].flU = 0.0f;
	atVertex[2].flV = 0.0f;
	atVertex[3].flX = (float)nX2;
	atVertex[3].flY = (float)nY1;
	atVertex[3].field_08 = 0.5f;
	atVertex[3].field_0c = 1.0f;
	atVertex[3].flU = 0.0f;
	atVertex[3].flV = 0.0f;
	dwColor = (D3DGetPaletteColor(bColor) & 16777215) | ((DWORD)bAlpha << 24);
	atVertex[0].dwColor = dwColor;
	atVertex[1].dwColor = dwColor;
	atVertex[2].dwColor = dwColor;
	atVertex[3].dwColor = dwColor;
	glpDevice->lpVtbl->field_70(glpDevice, 6, 452, atVertex, 4, 28);
}

void __fastcall D3DDrawLine(int nX1, int nY1, int nX2, int nY2, BYTE bBColor, BYTE bAlpha)
{
	D3DTLVERTEX atVertex[2];
	DWORD dwColor;

	if (nX2 < 0 && nX1 < 0)
		return;
	if (nX2 >= gnScreenWidth && nX1 >= gnScreenWidth)
		return;
	if (nY2 < 0 && nY1 < 0)
		return;
	if (nY2 >= gnScreenHeight && nY1 >= gnScreenHeight)
		return;
	dwColor = D3DGetPaletteColor(bBColor);
	D3DSetBlendMode(2);
	if (bAlpha == 255)
		D3DSetAlphaMode(1);
	else
	{
		D3DSetAlphaMode(4);
		dwColor = ((DWORD)bAlpha << 24) | (dwColor & 16777215);
	}
	D3DSetFilterMode(1);
	if (nX1 == nX2 && nY1 == nY2)
	{
		atVertex[0].flX = (float)nX1;
		atVertex[0].field_08 = 0.5f;
		atVertex[0].field_0c = 1.0f;
		atVertex[0].flU = 0.0f;
		atVertex[0].flY = (float)nY1;
		atVertex[0].flV = 0.0f;
		atVertex[0].dwColor = dwColor;
		glpDevice->lpVtbl->field_70(glpDevice, 1, 452, atVertex, 1, 28);
		return;
	}
	atVertex[0].flX = (float)nX1;
	atVertex[0].field_08 = 0.5f;
	atVertex[0].field_0c = 1.0f;
	atVertex[0].flU = 0.0f;
	atVertex[0].flY = (float)nY1;
	atVertex[0].flV = 0.0f;
	atVertex[1].field_08 = 0.5f;
	atVertex[1].field_0c = 1.0f;
	atVertex[1].flU = 0.0f;
	atVertex[1].flX = (float)nX2;
	atVertex[1].flV = 0.0f;
	atVertex[0].dwColor = dwColor;
	atVertex[1].dwColor = dwColor;
	atVertex[1].flY = (float)nY2;
	glpDevice->lpVtbl->field_70(glpDevice, 2, 452, atVertex, 2, 28);
}

void __fastcall D3DDrawDiamond(const RECT *prc, BYTE bColor)
{
	int nWidth;
	int nHeight;
	int nCenterX;
	int nCenterY;
	int nRight;
	int nBottom;
	int nLeft;
	int nTop;

#line 197
	D2ASSERT(prc->left <= prc->right);
#line 198
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
	D3DDrawLine(nCenterX, nTop, nRight, nCenterY, bColor, 255);
	D3DDrawLine(nRight, nCenterY, nCenterX, nBottom, bColor, 255);
	D3DDrawLine(nCenterX, nBottom, nLeft, nCenterY, bColor, 255);
	D3DDrawLine(nLeft, nCenterY, nCenterX, nTop, bColor, 255);
}

void __fastcall D3DDrawFrame(const RECT *prc, BYTE bColor)
{
	int nLeft;
	int nRight;
	int nY1;
	int nY2;

#line 235
	D2ASSERT(prc->left <= prc->right);
#line 236
	D2ASSERT(prc->top <= prc->bottom);
	nLeft = D3DClampTo(prc->left, gnScreenWidth - 1);
	nRight = D3DClampTo(prc->right, gnScreenWidth - 1);
	if (nLeft == nRight)
		return;
	nY1 = D3DClampTo(prc->top, gnScreenHeight - 1);
	nY2 = D3DClampTo(prc->bottom, gnScreenHeight - 1);
	if (nY2 == nY1)
		return;
#line 252
	D2ASSERT(nY2 > nY1);
	D3DDrawLine(nLeft, nY1, nRight, nY1, bColor, 255);
	D3DDrawLine(nRight, nY1, nRight, nY2, bColor, 255);
	D3DDrawLine(nRight, nY2, nLeft, nY2, bColor, 255);
	D3DDrawLine(nLeft, nY2, nLeft, nY1, bColor, 255);
}

void __fastcall D3DFillRect(const RECT *prc, BYTE bColor)
{
#line 268
	D2ASSERT(prc->left <= prc->right);
#line 269
	D2ASSERT(prc->top <= prc->bottom);
	D3DFillSpan(prc->left, prc->top, prc->right, prc->bottom, bColor, 5);
}

void __fastcall D3DFillSquare(const POINT *ptCenter, BYTE bSize, BYTE bColor)
{
	RECT rc;

	if (!bSize)
		return;
	bSize--;
	SetRect(&rc, ptCenter->x - bSize, ptCenter->y - bSize, ptCenter->x + bSize + 1, ptCenter->y + bSize + 1);
	D3DFillSpan(rc.left, rc.top, rc.right, rc.bottom, bColor, 5);
}

void __fastcall D3DEngineClear(BOOL fAbovePanel)
{
	RECT rc;

	if (gtVideoConfig.dwFlags & gdwBitMasks[10])
	{
		D3DClearRender();
		return;
	}
	rc.left = 0;
	rc.top = 0;
	rc.right = gnScreenWidth;
	rc.bottom = gnScreenHeight;
	glpViewport->lpVtbl->field_50(glpViewport, 1, &rc, 1, 0xFF000000, 0, 0);
}
