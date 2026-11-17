#define D2_THIS_FILE "C:\\D2\\Source\\D2Direct3D\\Src\\d3dDebugFont.cpp"
#include "d3dCaches.h"
#include "StormImports.h"
#include "D2Direct3DImports.h"
#include "D2Macros.h"

static DWORD sgdwFontInk = 16777215;
static int sganCharWidth[96] = { 0 };
static D3DTLVERTEX sgatFontVertex[4] = { 0 };
static COLORREF sgdwFontPaper = 0;
static D3DSURFACE *sgalpddsFont[96] = { 0 };
static D3DTEXTURE *sgalptexFont[96] = { 0 };

DWORD __fastcall D3DCreateDebugFont(DWORD dwBase)
{
	HDC hDC;
	HFONT hFont;
	HBITMAP hBitmap;
	RECT rc;
	D3DSURFDESC tDesc;
	D3DSURFACE *lpddsMemory;
	D3DTEXTURE *lptexMemory;
	HRESULT hResult;
	WORD *pwPixel;
	BOOL success;
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
	lpddsMemory = NULL;
	lptexMemory = NULL;
	D3DCreateMemoryTexture(&lpddsMemory, &lptexMemory, 8, 16);
	for (nChar = 32; nChar <= 127; nChar++)
	{
		szChar[0] = (char)nChar;
		DrawTextA(hDC, szChar, 1, &rc, 0);
		tDesc.dwSize = sizeof(tDesc);
		memset(&tDesc.dwFlags, 0, sizeof(tDesc) - sizeof(DWORD));
		hResult = lpddsMemory->lpVtbl->pfnLock(lpddsMemory, NULL, &tDesc, 33, NULL);
		if (hResult)
			D3DLogDDError("Failed to lock memory surface", hResult);
		pwPixel = (WORD *)tDesc.pbSurface;
		for (y = 0; y < 16; y++)
			for (x = 0; x < 8; x++)
			{
				if (GetPixel(hDC, x, y) == sgdwFontInk)
					*pwPixel++ = 65535;
				else
					*pwPixel++ = 0;

			}
		hResult = lpddsMemory->lpVtbl->pfnUnlock(lpddsMemory, NULL);
		if (hResult)
			D3DLogDDError("Failed to unlock memory surface", hResult);
		success = D3DCreateDeviceTexture(&sgalpddsFont[nChar - 32], &sgalptexFont[nChar - 32], 8, 16);
#line 192
		D2ASSERT(success);
		if (gtVideoConfig.dwFlags & gdwBitMasks[10])
			D3DCopySurface(sgalpddsFont[nChar - 32], lpddsMemory, 16);
		else
			sgalptexFont[nChar - 32]->lpVtbl->field_14(sgalptexFont[nChar - 32], lptexMemory);
	}
	lpddsMemory->lpVtbl->field_08(lpddsMemory);
	lptexMemory->lpVtbl->field_08(lptexMemory);
	GetCharWidthA(hDC, 32, 127, sganCharWidth);
	DeleteObject(hBitmap);
	DeleteObject(hFont);
	DeleteDC(hDC);
	sgatFontVertex[0].flX = 0.0f;
	sgatFontVertex[0].dwColor = 0xFFFFFFFF;
	sgatFontVertex[1].dwColor = 0xFFFFFFFF;
	sgatFontVertex[2].dwColor = 0xFFFFFFFF;
	sgatFontVertex[3].dwColor = 0xFFFFFFFF;
	sgatFontVertex[0].flY = 0.0f;
	sgatFontVertex[0].field_08 = 0.5f;
	sgatFontVertex[0].field_0c = 1.0f;
	sgatFontVertex[0].flU = 0.0f;
	sgatFontVertex[0].flV = 0.0f;
	sgatFontVertex[1].flX = 0.0f;
	sgatFontVertex[1].flY = 0.0f;
	sgatFontVertex[1].field_08 = 0.5f;
	sgatFontVertex[1].field_0c = 1.0f;
	sgatFontVertex[1].flU = 0.0f;
	sgatFontVertex[1].flV = 1.0f;
	sgatFontVertex[2].flX = 0.0f;
	sgatFontVertex[2].flY = 0.0f;
	sgatFontVertex[2].field_08 = 0.5f;
	sgatFontVertex[2].field_0c = 1.0f;
	sgatFontVertex[2].flU = 1.0f;
	sgatFontVertex[2].flV = 1.0f;
	sgatFontVertex[3].flX = 0.0f;
	sgatFontVertex[3].flY = 0.0f;
	sgatFontVertex[3].field_08 = 0.5f;
	sgatFontVertex[3].field_0c = 1.0f;
	sgatFontVertex[3].flU = 1.0f;
	sgatFontVertex[3].flV = 0.0f;
	return dwBase + 12288;
}

void WINAPI D3DFreeDebugFont(void)
{
	int i;

	for (i = 0; i < 96; i++)
	{
		if (sgalpddsFont[i])
		{
			sgalpddsFont[i]->lpVtbl->field_08(sgalpddsFont[i]);
			sgalpddsFont[i] = NULL;
		}
		if (sgalptexFont[i])
		{
			sgalptexFont[i]->lpVtbl->field_08(sgalptexFont[i]);
			sgalptexFont[i] = NULL;
		}
	}
}

void __fastcall D3DSetDebugFontPalette(void *ptPalette)
{
	HRESULT hResult;
	int i;

	if (!ptPalette)
		return;
	for (i = 0; i < 96; i++)
	{
		if (!sgalpddsFont[i])
			continue;
		hResult = sgalpddsFont[i]->lpVtbl->pfnSetPalette(sgalpddsFont[i], ptPalette);
		if (hResult)
		{
			D3DLogDDError("SetPalette (debug font surface)", hResult);
			return;
		}
	}
}

void __fastcall D3DDrawDebugText(int nX, int nY, const char *pszFormat, va_list vaArgs)
{
	char szBuffer[512];
	int nLength;
	int nLineX;
	int nRight;
	int nAdvance;
	char cChar;
	int i;

	nLength = wvsprintfA(szBuffer, pszFormat, vaArgs);
	nLineX = nX;
	D3DSetBlendMode(1);
	D3DSetAlphaMode(1);
	D3DSetFilterMode(1);
	for (i = 0; i < nLength; i++)
	{
		cChar = szBuffer[i];
		if (nX > 640)
			nAdvance = 0;
		else if (nX < 0)
			nAdvance = sganCharWidth[cChar];
		else if (cChar >= 32 && cChar <= 127)
		{
			cChar -= 32;
			nRight = nX + 8;
			sgatFontVertex[0].flX = (float)nX;
			sgatFontVertex[0].flY = (float)nY;
			sgatFontVertex[1].flX = (float)nX;
			sgatFontVertex[1].flY = (float)(nY + 16);
			sgatFontVertex[2].flX = (float)nRight;
			sgatFontVertex[2].flY = (float)(nY + 16);
			sgatFontVertex[3].flX = (float)nRight;
			sgatFontVertex[3].flY = (float)nY;
			glpDevice->lpVtbl->field_98(glpDevice, 0, sgalptexFont[cChar]);
			glpDevice->lpVtbl->field_70(glpDevice, 6, 452, sgatFontVertex, 4, 28);
			nAdvance = sganCharWidth[cChar];
		}
		else
			nAdvance = sganCharWidth[0];
		nX += nAdvance;
		if (szBuffer[i] == '\n')
		{
			nX = nLineX;
			nY += 12;
		}
	}
}

void WINAPI D3DNothingDebugLightGrid(DWORD dwValue1, DWORD dwValue2)
{
}

DWORD WINAPI D3DGetDrawCtxFlag(void)
{
	return gptDrawCtx->dwBlendShadow;
}
