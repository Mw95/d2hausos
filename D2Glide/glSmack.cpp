#define D2_THIS_FILE "C:\\D2\\Source\\D2Glide\\Src\\glSmack.cpp"
#include "glCore.h"
#include "StormImports.h"
#include "D2GlideImports.h"
#include "D2Macros.h"

struct SMACK
{
 	DWORD		unobserved_000;
 	int			dwWidth;
 	int			dwHeight;
 	DWORD		dwNumFrames;
 	BYTE		unobserved_010[884 - 16];
 	DWORD		dwFrameNum;
};

struct GLSMACKVIDEO
{
 	SMACK		*ptSmack;
 	int			nX;
 	int			nY;
};
static GLTEXINFO sgtSmackTexInfoA = { 8, 8, 2, 10, NULL };
static GLTEXINFO sgtSmackTexInfoB = { 8, 8, 2, 10, NULL };

static BYTE *sgpbSmackHalfA = 0;
static BYTE *sgpbSmackHalfB = 0;
static BYTE *sglpSmackBigBuffer = 0;
static GLVERTEX sgatSmackVertex[4] = { 0 };
static DWORD sgdwSmackAddressA = 0;
static DWORD sgdwSmackAddressB = 0;
static BOOL sgfSmackActive = 0;
static DWORD sgdwSmackTmu = 0;

void __fastcall GlideSmackStartup(void)
{
	sgatSmackVertex[0].dwColor = 0xFFFFFFFF;
	sgatSmackVertex[1].dwColor = 0xFFFFFFFF;
	sgatSmackVertex[2].dwColor = 0xFFFFFFFF;
	sgatSmackVertex[3].dwColor = 0xFFFFFFFF;
#line 75
	sglpSmackBigBuffer = (BYTE *)D2ALLOC2(65536);
#line 77
	sgpbSmackHalfA = (BYTE *)D2ALLOC2(32768);
#line 78
	sgpbSmackHalfB = (BYTE *)D2ALLOC2(32768);
}

DWORD __fastcall GlideSmackCreateBuffer(DWORD dwBase, DWORD dwAlign)
{
	DWORD dwMask;
	DWORD dwStep;

	dwStep = 32768;
	dwMask = dwAlign - 1;
	if (dwMask & 32768)
		dwStep = (dwMask | 32768) + 1;
	if (dwMask & dwBase)
		dwBase = (dwMask | dwBase) + 1;
	sgdwSmackAddressA = dwBase;
	sgdwSmackAddressB = dwStep + dwBase;
	sgtSmackTexInfoA.pbPixels = sgpbSmackHalfA;
	sgtSmackTexInfoB.pbPixels = sgpbSmackHalfB;
	return dwStep * 2;
}

void __fastcall GlideSmackShutdown(void)
{
	if (sglpSmackBigBuffer)
	{
#line 116
		D2FREE2(sglpSmackBigBuffer);
		sglpSmackBigBuffer = NULL;
	}
	if (sgpbSmackHalfA)
	{
#line 122
		D2FREE2(sgpbSmackHalfA);
		sgpbSmackHalfA = NULL;
	}
	if (sgpbSmackHalfB)
	{
#line 127
		D2FREE2(sgpbSmackHalfB);
		sgpbSmackHalfB = NULL;
	}
}

void __fastcall GlideSmackFreeBuffer(void)
{
}

void __fastcall GlideSmackOpen(const char *pszFileName, GLSMACKVIDEO *ptVideo)
{
	DWORD *pdwClear;
	int i;

	if (sgfSmackActive)
	{
		ptVideo->ptSmack = NULL;
		return;
	}
#line 164
	D2ASSERT(sglpSmackBigBuffer);
	pdwClear = (DWORD *)sglpSmackBigBuffer;
	for (i = 0; i < 16384; i++)
		pdwClear[i] = 0;
	pdwClear = (DWORD *)sgpbSmackHalfA;
	for (i = 0; i < 8192; i++)
		pdwClear[i] = 0;
	pdwClear = (DWORD *)sgpbSmackHalfB;
	for (i = 0; i < 8192; i++)
		pdwClear[i] = 0;
	if (!gfSuspended)
	{
		grTexDownloadMipMap(sgdwSmackTmu, sgdwSmackAddressA, 3, &sgtSmackTexInfoA);
		grTexDownloadMipMap(sgdwSmackTmu, sgdwSmackAddressB, 3, &sgtSmackTexInfoB);
	}
	if (ptVideo->ptSmack)
		return;
	ptVideo->ptSmack = (SMACK *)SmackOpen((DWORD)pszFileName, 1040384, (DWORD)-1);
	if (!ptVideo->ptSmack)
	{
		sgfSmackActive = FALSE;
		return;
	}
	SmackToBuffer((DWORD)ptVideo->ptSmack, 0, 0, 1024, 128, (DWORD)sglpSmackBigBuffer, 0xC0000000);
	sgfSmackActive = TRUE;
}

void __fastcall GlideSmackUpdate(GLSMACKVIDEO *ptVideo)
{
	SMACK *ptSmack;
	const DWORD *pdwFrom;
	DWORD *pdwToA;
	DWORD *pdwToB;
	int nTop;
	int nWidth;
	int nVideoWidth;
	int nHeight;
	int nX;
	int nRight;
	int nY;
	int nRows;
	int nHalfBytes = 512;

	ptSmack = ptVideo->ptSmack;
	if (!ptSmack)
		return;
	if (!SmackWait((DWORD)ptSmack))
	{
		if (!ghWnd)
			return;
		SmackDoFrame((DWORD)ptSmack);
		pdwFrom = (const DWORD *)sglpSmackBigBuffer;
		pdwToA = (DWORD *)sgpbSmackHalfA;
		pdwToB = (DWORD *)sgpbSmackHalfB;
		for (nRows = 64; nRows; nRows--)
		{
			memcpy(pdwToA, pdwFrom, nHalfBytes);
			pdwToA += 128;
			memcpy(pdwToB, pdwFrom + 128, nHalfBytes);
			pdwFrom += 256;
			pdwToB += 128;
		}
		if (ptSmack->dwFrameNum == ptSmack->dwNumFrames)
		{
			if (ptVideo->ptSmack)
			{
				SmackClose((DWORD)ptVideo->ptSmack);
				ptVideo->ptSmack = NULL;
			}
			sgfSmackActive = FALSE;
		}
		else
			SmackNextFrame((DWORD)ptSmack);
		if (gfSuspended)
			return;
		grTexDownloadMipMap(sgdwSmackTmu, sgdwSmackAddressA, 3, &sgtSmackTexInfoA);
		grTexDownloadMipMap(sgdwSmackTmu, sgdwSmackAddressB, 3, &sgtSmackTexInfoB);
	}
	if (gfSuspended)
		return;
	ptSmack = ptVideo->ptSmack;
	GlideSetColorCombine(1);
	GlideSetAlphaMode(1);
	GlideSetFilterMode(1);
	GlideSetTexCombine(sgdwSmackTmu);
	grChromakeyMode(0);
	nTop = ptVideo->nY - ptSmack->dwHeight;
	nY = ptVideo->nY;
	nHeight = ptSmack->dwHeight;
	grTexSource(sgdwSmackTmu, sgdwSmackAddressA, 3, &sgtSmackTexInfoA);
	nX = ptVideo->nX;
	nRight = nX + 256;
	nVideoWidth = ptSmack->dwWidth;
	nWidth = 256;
	if (nVideoWidth <= 256)
		nWidth = nVideoWidth;
	sgatSmackVertex[0].flX = (float)nX;
	sgatSmackVertex[0].flY = (float)nTop;
	sgatSmackVertex[0].field_0c = 1.0f;
	sgatSmackVertex[0].flS = 0.0f;
	sgatSmackVertex[0].flT = 0.0f;
	sgatSmackVertex[1].flX = (float)nX;
	sgatSmackVertex[1].flY = (float)nY;
	sgatSmackVertex[1].field_0c = 1.0f;
	sgatSmackVertex[1].flS = 0.0f;
	sgatSmackVertex[1].flT = (float)nHeight;
	sgatSmackVertex[2].flX = (float)nRight;
	sgatSmackVertex[2].flY = (float)nY;
	sgatSmackVertex[2].field_0c = 1.0f;
	sgatSmackVertex[2].flS = (float)nWidth;
	sgatSmackVertex[2].flT = (float)nHeight;
	sgatSmackVertex[3].flX = (float)nRight;
	sgatSmackVertex[3].flY = (float)nTop;
	sgatSmackVertex[3].field_0c = 1.0f;
	sgatSmackVertex[3].flS = (float)nWidth;
	sgatSmackVertex[3].flT = 0.0f;
	grDrawVertexArrayContiguous(5, 4, sgatSmackVertex, sizeof(GLVERTEX));
	grTexSource(sgdwSmackTmu, sgdwSmackAddressB, 3, &sgtSmackTexInfoB);
	nX += 256;
	nRight = ptSmack->dwWidth + nX - 256;
	nWidth = ptSmack->dwWidth - 256;
	sgatSmackVertex[0].flX = (float)nX;
	sgatSmackVertex[0].flY = (float)nTop;
	sgatSmackVertex[0].field_0c = 1.0f;
	sgatSmackVertex[0].flS = 0.0f;
	sgatSmackVertex[0].flT = 0.0f;
	sgatSmackVertex[1].flX = (float)nX;
	sgatSmackVertex[1].flY = (float)nY;
	sgatSmackVertex[1].field_0c = 1.0f;
	sgatSmackVertex[1].flS = 0.0f;
	sgatSmackVertex[1].flT = (float)nHeight;
	sgatSmackVertex[2].flX = (float)nRight;
	sgatSmackVertex[2].flY = (float)nY;
	sgatSmackVertex[2].field_0c = 1.0f;
	sgatSmackVertex[2].flS = (float)nWidth;
	sgatSmackVertex[2].flT = (float)nHeight;
	sgatSmackVertex[3].flX = (float)nRight;
	sgatSmackVertex[3].flY = (float)nTop;
	sgatSmackVertex[3].field_0c = 1.0f;
	sgatSmackVertex[3].flS = (float)nWidth;
	sgatSmackVertex[3].flT = 0.0f;
	grDrawVertexArrayContiguous(5, 4, sgatSmackVertex, sizeof(GLVERTEX));
	grChromakeyMode(1);
}

void __fastcall GlideSmackClose(GLSMACKVIDEO *ptVideo)
{
	if (ptVideo->ptSmack)
	{
		SmackClose((DWORD)ptVideo->ptSmack);
		ptVideo->ptSmack = NULL;
	}
	sgfSmackActive = FALSE;
}
