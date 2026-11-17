#define D2_THIS_FILE "C:\\D2\\Source\\D2Glide\\Src\\glDebugFont.cpp"
#include "glCore.h"
#include "StormImports.h"
#include "D2GlideImports.h"
#include "D2Macros.h"

static DWORD sgdwFontInk = 16777215;
static GLTEXINFO sgtFontTexInfo = { 4, 4, (DWORD)-1, 5, NULL };
static int sganBlotX[8] = { 1, 2, 0, 1, 2, 3, 1, 2 };
static int sganBlotY[8] = { 0, 0, 1, 1, 1, 1, 2, 2 };
static DWORD sgadwGreyRamp[17] =
{
	0, 286331136, 572662272, 858993408, 1145324544, 1431655680, 1717986816, 2004317952,
	0x88888800, 0x99999900, 0xAAAAAA00, 0xBBBBBB00, 0xCCCCCC00, 0xDDDDDD00, 0xEEEEEE00, 0xFFFFFF00,
	0xFFFFFF00
};

static struct
{
	DWORD adwDebugPalette[1024];
	int anCharWidth[96];
	DWORD adwFontAddress[96];
} sgtFontBlock = { 0 };
static GLVERTEX sgatFontVertex[4] = { 0 };
static DWORD sgdwSlot50Arg2 = 0;
static DWORD sgdwSlot50Arg1 = 0;
static COLORREF sgdwFontPaper = 0;
static BYTE *sgpbCharMapBase = 0;
static DWORD sgdwFontTmu = 0;
static BYTE **sgppbLightGrid = 0;

DWORD __fastcall GlideDownloadDebugFont(DWORD dwBase, DWORD dwAlign)
{
	BYTE *pbBuffer;
	DWORD dwMask;
	DWORD dwStep;
	DWORD dwBytes;
	DWORD dwAddress;
	int i;

	dwStep = 128;
	dwMask = dwAlign - 1;
	if (dwMask & 128)
		dwStep = (dwMask | 128) + 1;
	dwBytes = dwStep * 96;
	if (dwMask & dwBase)
		dwBase = (dwMask | dwBase) + 1;
	dwAddress = dwBase;
#line 72
	pbBuffer = (BYTE *)D2ALLOC2(128);
	sgtFontTexInfo.pbPixels = pbBuffer;
	for (i = 0; i < 96; i++)
	{
		sgtFontBlock.adwFontAddress[i] = dwAddress;
		memcpy(pbBuffer, sgpbCharMapBase + i * 128, 32 * sizeof(DWORD));
		grTexDownloadMipMap(sgdwFontTmu, sgtFontBlock.adwFontAddress[i], 3, &sgtFontTexInfo);
		dwAddress += dwStep;
	}
#line 86
	D2FREE2(pbBuffer);
	sgatFontVertex[0].flX = 0.0f;
	sgatFontVertex[0].flY = 0.0f;
	sgatFontVertex[0].dwColor = 0xFFFFFFFF;
	sgatFontVertex[0].field_0c = 1.0f;
	sgatFontVertex[0].flS = 0.0f;
	sgatFontVertex[0].flT = 0.0f;
	sgatFontVertex[1].flX = 0.0f;
	sgatFontVertex[1].flY = 0.0f;
	sgatFontVertex[1].dwColor = 0xFFFFFFFF;
	sgatFontVertex[1].field_0c = 1.0f;
	sgatFontVertex[1].flS = 0.0f;
	sgatFontVertex[1].flT = 256.0f;
	sgatFontVertex[2].flX = 0.0f;
	sgatFontVertex[2].flY = 0.0f;
	sgatFontVertex[2].dwColor = 0xFFFFFFFF;
	sgatFontVertex[2].field_0c = 1.0f;
	sgatFontVertex[2].flS = 128.0f;
	sgatFontVertex[2].flT = 256.0f;
	sgatFontVertex[3].flX = 0.0f;
	sgatFontVertex[3].flY = 0.0f;
	sgatFontVertex[3].dwColor = 0xFFFFFFFF;
	sgatFontVertex[3].field_0c = 1.0f;
	sgatFontVertex[3].flS = 128.0f;
	sgatFontVertex[3].flT = 0.0f;
	return dwBytes;
}

void __fastcall GlideCreateDebugFont(void)
{
	HDC hDC;
	HFONT hFont;
	HBITMAP hBitmap;
	RECT rc;
	BYTE *pbOut;
	char szChar[1];
	int nChar;
	int x;
	int y;

	hDC = CreateDCA("DISPLAY", NULL, NULL, NULL);
	hFont = CreateFontA(10, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 1, "Courier");
	hBitmap = CreateBitmap(8, 16, 1, 8, NULL);
	SelectObject(hDC, hFont);
	SelectObject(hDC, hBitmap);
	SetTextColor(hDC, sgdwFontInk);
	SetBkColor(hDC, sgdwFontPaper);
	SetBkMode(hDC, 2);
	SetRect(&rc, 0, 0, 8, 16);
#line 126
	D2ASSERT(!sgpbCharMapBase);
#line 127
	sgpbCharMapBase = (BYTE *)D2ALLOC2(12288);
	for (nChar = 0; nChar < 96; nChar++)
	{
		pbOut = sgpbCharMapBase + nChar * 128;
		szChar[0] = (char)(nChar + 32);
		DrawTextA(hDC, szChar, 1, &rc, 0);
		for (y = 0; y < 16; y++)
			for (x = 0; x < 8; x++)
			{
				if (GetPixel(hDC, x, y) == sgdwFontInk)
					*pbOut++ = 255;
				else
					*pbOut++ = 0;

			}
	}
	GetCharWidthA(hDC, 32, 127, sgtFontBlock.anCharWidth);
	DeleteObject(hBitmap);
	DeleteObject(hFont);
	DeleteDC(hDC);
}

void __fastcall GlideFreeDebugFont(void)
{
	if (sgpbCharMapBase)
	{
#line 225
		D2FREE2(sgpbCharMapBase);
		sgpbCharMapBase = NULL;
	}
}

void __fastcall GlideDrawDebugText(int nX, int nY, const char *pszFormat, va_list vaArgs)
{
	char szText[512];
	int nLength;
	int nAdvance;
	int nRight;
	int nStartX;
	int i;
	char c;

	nLength = wvsprintfA(szText, pszFormat, vaArgs);
	nStartX = nX;
	GlideSetColorCombine(1);
	GlideSetAlphaMode(1);
	GlideSetFilterMode(1);
	GlideSetTexCombine(sgdwFontTmu);
	for (i = 0; i < nLength; i++)
	{
		c = szText[i];
		if (nX > 640)
			nAdvance = 0;
		else if (nX < 0)
			nAdvance = sgtFontBlock.anCharWidth[c];
		else if (c >= 32 && c <= 127)
		{
			c -= 32;
			nRight = nX + 8;
			sgatFontVertex[0].flX = (float)nX;
			sgatFontVertex[0].flY = (float)nY;
			sgatFontVertex[1].flX = (float)nX;
			sgatFontVertex[1].flY = (float)(nY + 16);
			sgatFontVertex[2].flX = (float)nRight;
			sgatFontVertex[2].flY = (float)(nY + 16);
			sgatFontVertex[3].flX = (float)nRight;
			sgatFontVertex[3].flY = (float)nY;
			grTexSource(sgdwFontTmu, sgtFontBlock.adwFontAddress[c], 3, &sgtFontTexInfo);
			grDrawVertexArrayContiguous(5, 4, sgatFontVertex, sizeof(GLVERTEX));
			nAdvance = sgtFontBlock.anCharWidth[c];
		}
		else
			nAdvance = sgtFontBlock.anCharWidth[0];
		nX += nAdvance;
		if (szText[i] == '\n')
		{
			nX = nStartX;
			nY += 12;
		}
	}
}

void __fastcall GlideDrawDebugPalette(void)
{
	GLVERTEX tPoint;
	int nRow;
	int nCol;
	int nX;
	int nY;
	int k;

	grColorCombine(1, 0, 1, 2, 0);
	grChromakeyMode(0);
	for (nRow = 0; nRow < 32; nRow++)
	{
		for (nCol = 0; nCol < 32; nCol++)
		{
			nX = (nCol << 1) + 400 - (nRow << 1);
			nY = (nRow + nCol + 50) << 1;
			grConstantColorValue(sgadwGreyRamp[sgtFontBlock.adwDebugPalette[nRow * 32 + nCol]]);
			for (k = 0; k < 8; k++)
			{
				tPoint.flX = (float)(sganBlotX[k] + nX + 150);
				tPoint.flY = (float)(sganBlotY[k] + nY);
				grDrawPoint(&tPoint);
			}
		}
	}
	grColorCombine(3, 1, 0, 1, 0);
	grChromakeyMode(1);
}

void __fastcall GlideDrawDebugLightGrid(void)
{
	GLVERTEX tCentre;
	GLVERTEX tPoint;
	const BYTE *pbEntry;
	DWORD dwLevel;
	DWORD dwColor;
	int nRow;
	int nCol;
	int nX;
	int nY;
	int k;

	grColorCombine(1, 0, 1, 2, 0);
	grChromakeyMode(0);
	for (nRow = 0; nRow < 48; nRow++)
	{
		for (nCol = 0; nCol < 48; nCol++)
		{
			nX = (nCol << 1) + 100 - (nRow << 1);
			nY = (nRow + nCol + 50) << 1;
			if (nRow == 24 && nCol == nRow)
			{
				grConstantColorValue(0);
				for (k = 0; k < 8; k++)
				{
					tCentre.flX = (float)(sganBlotX[k] + nX);
					tCentre.flY = (float)(sganBlotY[k] + nY);
					grDrawPoint(&tCentre);
				}
				continue;
			}
			else
			{
				pbEntry = sgppbLightGrid[nRow] + nCol * 8;
				dwLevel = pbEntry[4];
				dwColor = (pbEntry[5] * dwLevel) >> 8;
				dwColor = (dwColor << 8) + ((pbEntry[6] * dwLevel) >> 8);
				dwColor = (dwColor << 8) + ((pbEntry[7] * dwLevel) >> 8);
				grConstantColorValue(dwColor * 256);
			}
			for (k = 0; k < 8; k++)
			{
				tPoint.flX = (float)(sganBlotX[k] + nX);
				tPoint.flY = (float)(sganBlotY[k] + nY);
				grDrawPoint(&tPoint);
			}
		}
	}
	grColorCombine(3, 1, 0, 1, 0);
	grChromakeyMode(1);
}

void __fastcall GlideSetAndDrawDebugLightGrid(const DWORD *pdwPalette, const DWORD *pdwGrid, DWORD dwLightX,
	DWORD dwLightY)
{
	sgdwSlot50Arg1 = dwLightX;
	sgdwSlot50Arg2 = dwLightY;
	memcpy(sgtFontBlock.adwDebugPalette, pdwPalette, 1024 * sizeof(DWORD));
	memcpy(sgppbLightGrid[0], pdwGrid, 4608 * sizeof(DWORD));
	GlideDrawDebugLightGrid();
}

DWORD __fastcall GlideGetDrawCtxFlag(void)
{
	return gptDrawCtx->dwBlendShadow;
}
