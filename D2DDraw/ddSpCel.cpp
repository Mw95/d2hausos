#define D2_THIS_FILE "C:\\D2\\Source\\D2DDraw\\Src\\ddSpCel.cpp"
#include "ddCore.h"
#include "D2DDrawImports.h"
#include "D2Macros.h"
#include "ddCel.h"

static DWORD sgadwEnvKind[8] = { 2, 2, 2, 1, 1, 0, 1, 0 };

int gnDdClipRight = 0;
int gnDdClipLeft = 0;

BYTE * __fastcall DdGetEnvValue(int nIndex, DWORD *pdwKind)
{
	*pdwKind = sgadwEnvKind[nIndex];
	switch (nIndex)
	{
	case 0:
		return gptEnvInfo->pbBlendMode0;
	case 1:
		return gptEnvInfo->pbBlendMode1;
	case 2:
		return gptEnvInfo->pbBlendMode2;
	case 3:
		return gptEnvInfo->pbBlendMode3;
	case 4:
		return gptEnvInfo->pbBlendMode4;
	case 6:
		return gptEnvInfo->pbBlendMode6;
	}
	return NULL;
}

void __fastcall DdSmackInit(BOOL fWide)
{
	if (!fWide)
	{
		gnDdClipLeft = 0;
		gnDdClipRight = 640;
	}
	else
	{
		gnDdClipLeft = 0;
		gnDdClipRight = 800;
	}
}

BOOL __fastcall DdDrawCelSkip(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	DWORD dwUnused, const BYTE *pbPreMap)
{
	return DdDrawCel(ptCelContext, nX, nY, dwColor, nMode, pbPreMap);
}

BOOL __fastcall DdDrawCel(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	const BYTE *pbPreMap)
{
	DWORD dwKind;
	int nColor;
	const BYTE *pbColorMap;
	const BYTE *pbBlend;

	if (!D2CMPSpriteCacheGetContextCel(ptCelContext, 0, 1))
		return FALSE;
	if (!DdCelIsVisible(ptCelContext->hCel, nX, nY, FALSE))
		return FALSE;
	nColor = dwColor & 255;
	pbColorMap = NULL;
	pbBlend = DdGetEnvValue(nMode, &dwKind);
	if (nColor != 255)
		pbColorMap = gptEnvInfo->apbColorMaps[nColor >> 3];
	if (nMode == 7)
		pbColorMap = gptEnvInfo->pbBrightColorMap;
	D2CMPCelContextDraw(ptCelContext, nX, nY, gnDdClipLeft, gnDdClipRight, gpbSurface,
		gnBufferHeight, gnBufferWidth, pbColorMap, pbBlend, pbPreMap);
	return TRUE;
}

BOOL __fastcall DdDrawCelPreMap(D2CELCONTEXT *ptCelContext, int nX, int nY, DWORD dwColor, int nMode,
	int nPreMap)
{
	DWORD dwKind;
	int nColor;
	const BYTE *pbColorMap;
	const BYTE *pbPreMap;
	const BYTE *pbBlend;

	if (!D2CMPSpriteCacheGetContextCel(ptCelContext, 0, 1))
		return FALSE;
	if (!DdCelIsVisible(ptCelContext->hCel, nX, nY, FALSE))
		return FALSE;
	pbColorMap = NULL;
	pbPreMap = NULL;
	pbBlend = DdGetEnvValue(nMode, &dwKind);
	nColor = dwColor & 255;
	if (nColor != 255)
		pbColorMap = gptEnvInfo->apbColorMaps[nColor >> 3];
	if (nMode == 7)
		pbColorMap = gptEnvInfo->pbBrightColorMap;
	if (nPreMap)
		pbPreMap = gptEnvInfo->apbPreMaps[nPreMap];
	D2CMPCelContextDraw(ptCelContext, nX, nY, gnDdClipLeft, gnDdClipRight, gpbSurface,
		gnBufferHeight, gnBufferWidth, pbColorMap, pbBlend, pbPreMap);
	return TRUE;
}
