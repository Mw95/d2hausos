#include "d3dCaches.h"
#include "D2Direct3DImports.h"

BYTE gbVertexGreen = 0;
BYTE gabLightTable[256][256] = { 0 };
BYTE gbVertexRed = 0;
BYTE gbVertexBlue = 0;

void WINAPI D3DBuildLightTable(void)
{
	int a;
	int b;

	for (a = 0; a < 256; a++)
		for (b = 0; b < 256; b++)
			gabLightTable[a][b] = (BYTE)(a * b / 255);
}

void __fastcall D3DSetVertexColor(int bRed, int bGreen, int bBlue)
{
	gbVertexRed = (BYTE)bRed;
	gbVertexGreen = (BYTE)bGreen;
	gbVertexBlue = (BYTE)bBlue;
}

void __fastcall D3DSetVertexLightRGB(D3DTLVERTEX *ptVertex, BYTE bLight, BYTE bAlpha)
{
	const BYTE *pbRow;

	pbRow = gabLightTable[bLight];
	ptVertex->dwColor = (((((DWORD)bAlpha << 8) | pbRow[gbVertexRed]) << 8) | pbRow[gbVertexGreen]) << 8
		| pbRow[gbVertexBlue];
	ptVertex->field_14 = 0;
}

void __fastcall D3DSetVertexLight(D3DTLVERTEX *ptVertex, DWORD dwColor, BYTE bAlpha)
{
	const BYTE *pbRow;

	pbRow = gabLightTable[(BYTE)dwColor];
	ptVertex->dwColor = (((((DWORD)bAlpha << 8) | pbRow[(BYTE)(dwColor >> 8)]) << 8)
		| pbRow[(BYTE)(dwColor >> 16)]) << 8 | pbRow[dwColor >> 24];
	ptVertex->field_14 = 0;
}
