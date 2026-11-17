#define D2_THIS_FILE "C:\\D2\\Source\\D2DDraw\\Src\\ddEngine.cpp"
#include "ddCore.h"
#include "D2Macros.h"

void WINAPI DdNothingDebugLightGrid(DWORD dwValue1, DWORD dwValue2)
{
}

DWORD WINAPI DdGetDrawCtxFlag(void)
{
	return gptDrawCtx->dwBlendShadow;
}

void __fastcall DdFillSpan(int nX1, int nY1, int nX2, int nY2, BYTE bValue, int nEnvIndex)
{
	BYTE *pbTable;
	DWORD dwKind;
	BYTE *pbRow;
	BYTE *pbPixel;
	DWORD dwRows;
	DWORD x;
	int nMaxX;
	int nMaxY;
	int nLeft;
	int nRight;
	int nTop;
	int nBottom;

	pbTable = DdGetEnvValue(nEnvIndex, &dwKind);
	nMaxX = gnBufferWidth - 1;
	nX1 = nX1 > 0 ? nX1 : 0;
	nLeft = nX1 < nMaxX ? nX1 : nMaxX;
	nX2 = nX2 > 0 ? nX2 : 0;
	nRight = nX2 < nMaxX ? nX2 : nMaxX;
	if (nLeft == nRight)
		return;
	nMaxY = gnBufferHeight - 1;
	nTop = nY1 > 0 ? nY1 : 0;
	if (nTop >= nMaxY)
		nTop = nMaxY;
	nBottom = nY2 > 0 ? nY2 : 0;
	if (nBottom < nMaxY)
		nMaxY = nBottom;
	nBottom = nMaxY;
	if (nTop == nBottom)
		return;
#line 57
	D2ASSERTMSG(nBottom > nTop, "nY2 > nY1");
	pbRow = gpbSurface + gnBufferWidth * nTop + nLeft;
	dwRows = nBottom - nTop;
	switch (dwKind)
	{
	case 1:
		while (dwRows--)
		{
			pbPixel = pbRow;
			x = nRight - nLeft;
			while (x--)
			{
				*pbPixel = pbTable[*pbPixel];
				pbPixel++;
			}
			pbRow += gnBufferWidth;
		}
		break;
	case 2:
		while (dwRows--)
		{
			pbPixel = pbRow;
			x = nRight - nLeft;
			while (x--)
			{
				*pbPixel = pbTable[(*pbPixel << 8) + bValue];
				pbPixel++;
			}
			pbRow += gnBufferWidth;
		}
		break;
	default:
		while (dwRows--)
		{
			memset(pbRow, bValue, nRight - nLeft);
			pbRow += gnBufferWidth;
		}
		break;
	}
}

void __fastcall DdFillSpanSelect(int nX1, int nY1, int nX2, int nY2, BYTE bValue, BYTE bSelector)
{
	int nEnvIndex;

	if (bSelector >= 192)
		nEnvIndex = 5;
	else if (bSelector >= 128)
		nEnvIndex = 2;
	else if (bSelector >= 64)
		nEnvIndex = 1;
	else if (bSelector <= 0)
		return;
	else
		nEnvIndex = 0;
	DdFillSpan(nX1, nY1, nX2, nY2, bValue, nEnvIndex);
}

void __fastcall DdDrawLine(int nX1, int nY1, int nX2, int nY2, BYTE bColor, DWORD dwUnused)
{
	int nDeltaX;
	int nDeltaY, nY;
	int nStepX;
	int nStepY;
	int nError;
	int i;

	nDeltaX = nX2 - nX1;
	nDeltaY = nY2 - nY1; nY = nY1;
	if (nDeltaX < 0)
	{
		nStepX = -1;
		nDeltaX = -nDeltaX;
	}
	else
		nStepX = 1;
	if (nDeltaY < 0)
	{
		nStepY = -1;
		nDeltaY = -nDeltaY;
	}
	else
		nStepY = 1;
	if (nX1 >= 0 && nX1 < gnBufferWidth && nY1 >= 0 && nY1 < gnBufferHeight)
		gpbSurface[gnBufferWidth * nY1 + nX1] = bColor;
	nError = 0;
	if (nDeltaX < nDeltaY)
	{
		for (i = 0; i < nDeltaY; i++)
		{
			nError += nDeltaX;
			nY += nStepY;
			if (nError > nDeltaY)
			{
				nX1 += nStepX;
				nError -= nDeltaY;
			}
			if (nX1 >= 0 && nX1 < gnBufferWidth && nY >= 0 && nY < gnBufferHeight)
				gpbSurface[gnBufferWidth * nY + nX1] = bColor;
		}
	}
	else
	{
		for (i = 0; i < nDeltaX; i++)
		{
			nError += nDeltaY;
			nX1 += nStepX;
			if (nError > nDeltaX)
			{
				nY += nStepY;
				nError -= nDeltaX;
			}
			if (nX1 >= 0 && nX1 < gnBufferWidth && nY >= 0 && nY < gnBufferHeight)
				gpbSurface[gnBufferWidth * nY + nX1] = bColor;
		}
	}
}

static void __fastcall DdDrawSegment(const POINT *ptFrom, const POINT *ptTo, BYTE bColor);

void __fastcall DdDrawDiamond(const RECT *prc, BYTE bColor)
{
	POINT tTop;
	POINT tRight;
	POINT tBottom;
	POINT tLeft;
	int nMidX;
	int nMidY;
	int nWidth;
	int nHeight;

#line 301
	D2ASSERT(prc->left <= prc->right);
#line 302
	D2ASSERT(prc->top <= prc->bottom);
	nWidth = prc->right - prc->left;
	nHeight = prc->bottom - prc->top;
	if (nWidth == 0 || nHeight == 0)
		return;
	nMidX = ((DWORD)nWidth >> 1) + prc->left;
	nMidY = ((DWORD)nHeight >> 1) + prc->top;
	tRight.y = nMidY;
	tLeft.x = prc->left;
	tLeft.y = nMidY;
	tTop.x = nMidX;
	tTop.y = prc->top;
	tRight.x = prc->right;
	tBottom.x = nMidX;
	tBottom.y = prc->bottom;
	DdDrawSegment(&tTop, &tRight, bColor);
	DdDrawSegment(&tRight, &tBottom, bColor);
	DdDrawSegment(&tBottom, &tLeft, bColor);
	DdDrawSegment(&tLeft, &tTop, bColor);
}

static void __fastcall DdDrawSegment(const POINT *ptFrom, const POINT *ptTo, BYTE bColor)
{
	int nDeltaX;
	int nDeltaY;
	int nLengthSq;
	int nSteps;
	int nStepX;
	int nStepY;
	int nAccX;
	int nAccY;
	int nX;
	int nX0;
	int nY;
	int nY0;
	int nMove;
	int i;
	BYTE *pbPixel;

	nX0 = ptFrom->x;
	nY0 = ptFrom->y;
	nDeltaX = ptTo->x - nX0;
	nDeltaY = ptTo->y - nY0;
	if (!(nDeltaX || nDeltaY))
		return;
	nLengthSq = nDeltaY * nDeltaY + nDeltaX * nDeltaX;
	nSteps = 0;
	if (nLengthSq > 0)
	{
		do
		{
			nSteps++;
		} while (nSteps * nSteps < nLengthSq);
	}
	nStepX = (nDeltaX << 16) / nSteps;
	nStepY = (nDeltaY << 16) / nSteps;
	nAccX = 0;
	nAccY = 0;
	i = 0;
	nY = nY0;
	nX = nX0;
	while (nY < 0 || nY >= gnBufferHeight || nX < 0 || nX >= gnBufferWidth)
	{
		nAccX += nStepX;
		if (nAccX >= 65536 || nAccX <= -65536)
		{
			nMove = nAccX / 65536;
			nX += nMove;
			nAccX += nMove * -65536;
		}
		nAccY += nStepY;
		if (nAccY >= 65536 || nAccY <= -65536)
		{
			nMove = nAccY / 65536;
			nY += nMove;
			nAccY += nMove * -65536;
		}
		if (i >= nSteps)
			return;
		i++;
	}
	pbPixel = gpbSurface + nX + gnBufferWidth * nY;
	*pbPixel = bColor;
	i++;
	if (i >= nSteps)
		return;
	while (1)
	{
		nAccX += nStepX;
		if (nAccX >= 65536 || nAccX <= -65536)
		{
			nMove = nAccX / 65536;
			nX += nMove;
			pbPixel += nMove;
			nAccX += nMove * -65536;
		}
		if (nX < 0)
			return;
		if (nX >= gnBufferWidth)
			return;
		nAccY += nStepY;
		if (nAccY >= 65536 || nAccY <= -65536)
		{
			nMove = nAccY / 65536;
			nY += nMove;
			pbPixel += gnBufferWidth * nMove;
			nAccY += nMove * -65536;
		}
		if (nY < 0)
			return;
		if (nY >= gnBufferHeight)
			return;
		*pbPixel = bColor;
		i++;
		if (i >= nSteps)
			return;
	}
}

void __fastcall DdDrawFrame(const RECT *prc, BYTE bColor)
{
	POINT tTopRight;
	POINT tBottomRight;
	POINT tTopLeft;
	POINT tBottomLeft;

#line 329
	D2ASSERT(prc->left <= prc->right);
#line 330
	D2ASSERT(prc->top <= prc->bottom);
	if (prc->right - prc->left == 0 || prc->bottom - prc->top == 0)
		return;
	tTopRight.x = prc->right;
	tBottomRight.x = prc->right;
	tBottomRight.y = prc->bottom;
	tBottomLeft.y = prc->bottom;
	tTopLeft.x = prc->left;
	tTopLeft.y = prc->top;
	tTopRight.y = prc->top;
	tBottomLeft.x = prc->left;
	DdDrawSegment(&tTopLeft, &tTopRight, bColor);
	DdDrawSegment(&tTopRight, &tBottomRight, bColor);
	DdDrawSegment(&tBottomRight, &tBottomLeft, bColor);
	DdDrawSegment(&tBottomLeft, &tTopLeft, bColor);
}

void __fastcall DdFillRect(RECT *prc, BYTE bColor)
{
	BYTE *pbRow;
	DWORD dwWidth;
	int nRows;
	DWORD x;
	int y;

#line 357
	D2ASSERT(prc->left <= prc->right);
#line 358
	D2ASSERT(prc->top <= prc->bottom);
	if (prc->right < 0 || prc->left >= gnBufferWidth || prc->bottom < 0 || prc->top >= gnBufferHeight)
		return;
	if (prc->left < 0)
		prc->left = 0;
	if (prc->right >= gnBufferWidth)
		prc->right = gnBufferWidth - 1;
	if (prc->top < 0)
		prc->top = 0;
	if (prc->bottom >= gnBufferHeight)
		prc->bottom = gnBufferHeight - 1;
	dwWidth = prc->right - prc->left;
	nRows = prc->bottom - prc->top;
	if (dwWidth == 0 || nRows == 0)
		return;
	pbRow = gpbSurface + gnBufferWidth * prc->top + prc->left;
	for (y = nRows; y; y--)
	{
		for (x = 0; x < dwWidth; x++)
			pbRow[x] = bColor;
		pbRow += gnBufferWidth;
	}
}

void __fastcall DdFillSquare(const POINT *ptCenter, BYTE bSize, BYTE bColor)
{
	RECT rc;

	if (!bSize)
		return;
	bSize--;
	SetRect(&rc, ptCenter->x - bSize, ptCenter->y - bSize, ptCenter->x + bSize + 1, ptCenter->y + bSize + 1);
	DdFillRect(&rc, bColor);
}

void __fastcall DdClearRender(BOOL fAbovePanel)
{
	DDBLTFX tFx;
	HRESULT hResult;

	tFx.dwSize = sizeof(tFx);
	memset(&tFx.dwDDFX, 0, sizeof(tFx) - sizeof(DWORD));
	tFx.dwFillColor = 0;
	if (!sglpddsRender)
		return;
	hResult = sglpddsRender->Blt(NULL, NULL, NULL, 1024, &tFx);
	if (!hResult)
		return;
	if (hResult == (HRESULT)0x887601AE)
	{
		if (fAbovePanel)
			memset(gpbSurface, 0, gnBufferWidth * (gnBufferHeight - 47));
		else
			memset(gpbSurface, 0, gnBufferWidth * gnBufferHeight);
		return;
	}
	DdLogError("Blit (screen clear) failed!", hResult);
}
