#define D2_THIS_FILE "C:\\D2\\Source\\D2DDraw\\Src\\ddDebugFont.cpp"
#include "ddCore.h"
#include "D2Macros.h"

struct DDDEBUGFONT
{
	int			anCharWidth[96];
	int			anGlyphOffset[96];
};

static COLORREF sgdwTextColor = 16777215;
static DDDEBUGFONT sgtDebugFont = { 0 };
static COLORREF sgdwBkColor = 0;
BYTE *gpbDebugFont = NULL;

void __fastcall DdCreateDebugFont(void)
{
	HDC hDC;
	HFONT hFont;
	HBITMAP hBitmap;
	RECT rc;
	BYTE *pbGlyph;
	int *pnOffset;
	int nOffset;
	int nChar;
	char chText;
	int x;
	int y;

	hDC = CreateDCA("DISPLAY", NULL, NULL, NULL);
	hFont = CreateFontA(10, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 1, "Courier");
	hBitmap = CreateBitmap(8, 16, 1, 8, NULL);
	SelectObject(hDC, hFont);
	SelectObject(hDC, hBitmap);
	SetTextColor(hDC, sgdwTextColor);
	SetBkColor(hDC, sgdwBkColor);
	SetBkMode(hDC, OPAQUE);
	SetRect(&rc, 0, 0, 8, 16);
#line 58
	gpbDebugFont = (BYTE *)D2ALLOC2(12288);
	pbGlyph = gpbDebugFont;
	nOffset = 0;
	nChar = 32;
	for (pnOffset = sgtDebugFont.anGlyphOffset; (int)pnOffset <= (int)&sgtDebugFont.anGlyphOffset[95]; pnOffset++)
	{
		chText = (char)nChar;
		DrawTextA(hDC, &chText, 1, &rc, 0);
		for (y = 0; y < 16; y++)
		{
			for (x = 0; x < 8; x++)
			{
				if (GetPixel(hDC, x, y) == sgdwTextColor)
					*pbGlyph++ = 255;
				else
					*pbGlyph++ = 0;

			}
		}
		*pnOffset = nOffset;
		nOffset += 128;
		nChar++;
	}
	GetCharWidthA(hDC, 32, 127, sgtDebugFont.anCharWidth);
	DeleteObject(hBitmap);
	DeleteObject(hFont);
	DeleteDC(hDC);
}

void WINAPI DdFreeDebugFont(void)
{
	if (gpbDebugFont)
	{
#line 93
		D2FREE2(gpbDebugFont);
		gpbDebugFont = NULL;
	}
}

void __fastcall DdDrawDebugText(int nX, int nY, const char *pszFormat, va_list vaArgs)
{
	char szText[512];
	int nLength;
	int nAdvance;
	int nStartX;
	int i;
	int nRow;
	int nCol;
	char c;
	BYTE *pbDest;
	const BYTE *pbGlyph;

	nLength = wvsprintfA(szText, pszFormat, vaArgs);
	nStartX = nX;
	for (i = 0; i < nLength; i++)
	{
		c = szText[i];
		if (nX > 640)
			nAdvance = 0;
		else if (nX < 0)
			nAdvance = sgtDebugFont.anCharWidth[c];
		else if (c >= 32 && c <= 127)
		{
			c -= 32;
			pbGlyph = gpbDebugFont + sgtDebugFont.anGlyphOffset[c];
			pbDest = gpbSurface + gnBufferWidth * nY + nX;
			for (nRow = 16; nRow; nRow--)
			{
				for (nCol = 8; nCol; nCol--)
				{
					if (*pbGlyph++)
						*pbDest = 255;
					pbDest++;
				}
				pbDest += gnBufferWidth - 8;
			}
			nAdvance = sgtDebugFont.anCharWidth[c];
		}
		else
			nAdvance = sgtDebugFont.anCharWidth[0];
		nX += nAdvance;
		if (szText[i] == '\n')
		{
			nX = nStartX;
			nY += 12;
		}
	}
}
