#define D2_THIS_FILE "C:\\D2\\Source\\D2Glide\\Src\\glVertex.cpp"
#include "glCore.h"
#include "StormImports.h"
#include "D2GlideImports.h"
#include "D2Macros.h"

BYTE gbVertexGreen = 0;
BYTE gbVertexRed = 0;
BYTE gbVertexBlue = 0;
BYTE *gpbLightTable = 0;

void __fastcall GlideVertexOpen(void)
{
#line 23
	gpbLightTable = (BYTE *)D2ALLOC2(65536);
}

void __fastcall GlideVertexClose(void)
{
	if (gpbLightTable)
	{
#line 28
		D2FREE2(gpbLightTable);
	}
}

void __fastcall GlideBuildLightTable(void)
{
	int a;
	int b;

	for (b = 0; b < 256; b++)
		for (a = 0; a < 256; a++)
			gpbLightTable[(b << 8) + a] = (BYTE)(a * b / 255);
}

void __fastcall GlideSetVertexColor(int bRed, int bGreen, int bBlue)
{
	gbVertexRed = (BYTE)bRed;
	gbVertexGreen = (BYTE)bGreen;
	gbVertexBlue = (BYTE)bBlue;
}

void __fastcall GlideSetVertexLightRGB(GLVERTEX *ptVertex, BYTE bLight)
{
	const BYTE *pbRow;

	pbRow = gpbLightTable + ((DWORD)bLight << 8);
	ptVertex->dwColor = ((((0xFFFFFF00 | pbRow[gbVertexRed]) << 8) | pbRow[gbVertexGreen]) << 8)
		| pbRow[gbVertexBlue];
}

void __fastcall GlideSetVertexLight(GLVERTEX *ptVertex, DWORD dwColor)
{
	const BYTE *pbRow;

	pbRow = gpbLightTable + ((dwColor & 255) << 8);
	ptVertex->dwColor = ((((0xFFFFFF00 | pbRow[(BYTE)(dwColor >> 8)]) << 8) | pbRow[(BYTE)(dwColor >> 16)])
		<< 8) | pbRow[(BYTE)(dwColor >> 24)];
}
