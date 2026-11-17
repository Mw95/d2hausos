#define D2_THIS_FILE "C:\\D2\\Source\\D2Direct3D\\Src\\d3dSmack.cpp"
#include "d3dCaches.h"
#include "StormImports.h"
#include "D2Direct3DImports.h"
#include "D2Macros.h"

struct SMACK
{
 	DWORD		unobserved_000;
 	DWORD		dwWidth;
 	DWORD		dwHeight;
 	DWORD		dwNumFrames;
 	BYTE		unobserved_010[884 - 16];
 	DWORD		dwFrameNum;
};

struct D3DSMACKVIDEO
{
 	SMACK		*ptSmack;
 	int			nX;
 	int			nY;
};

static BYTE *sglpSmackBigBuffer = 0;
static D3DTLVERTEX sgatSmackVertex[4] = { 0 };
static DWORD sgfSmackActive = 0;
static D3DSURFACE *sgalpddsSmackMemory[2] = { 0 };
static D3DTEXTURE *sgalptexSmackMemory[2] = { 0 };
static D3DSURFACE *sgalpddsSmackDevice[2] = { 0 };
static D3DTEXTURE *sgalptexSmackDevice[2] = { 0 };

static void __fastcall D3DSmackUpload(void);

void WINAPI D3DSmackStartup(void)
{
	sgatSmackVertex[0].dwColor = 0xFFFFFFFF;
	sgatSmackVertex[1].dwColor = 0xFFFFFFFF;
	sgatSmackVertex[2].dwColor = 0xFFFFFFFF;
	sgatSmackVertex[3].dwColor = 0xFFFFFFFF;
#line 57
	sglpSmackBigBuffer = (BYTE *)D2ALLOC2(65536);
	memset(sglpSmackBigBuffer, 0, 65536);
}

DWORD __fastcall D3DSmackCreateBuffer(DWORD dwBase)
{
	BOOL fSuccess;
	int i;

	for (i = 0; i < 2; i++)
	{
		D3DCreateMemoryTexture(&sgalpddsSmackMemory[i], &sgalptexSmackMemory[i], 256, 64);
		fSuccess = D3DCreateDeviceTexture(&sgalpddsSmackDevice[i], &sgalptexSmackDevice[i], 256, 64);
#line 76
		D2ASSERT(fSuccess);
	}
	return dwBase + 65536;
}

void WINAPI D3DSmackShutdown(void)
{
	if (sglpSmackBigBuffer)
	{
#line 91
		D2FREE2(sglpSmackBigBuffer);
		sglpSmackBigBuffer = NULL;
	}
}

void WINAPI D3DSmackFreeBuffer(void)
{
	int i;

	for (i = 0; i < 2; i++)
	{
		if (sgalpddsSmackMemory[i])
		{
			sgalpddsSmackMemory[i]->lpVtbl->field_08(sgalpddsSmackMemory[i]);
			sgalpddsSmackMemory[i] = NULL;
		}
		if (sgalptexSmackMemory[i])
		{
			sgalptexSmackMemory[i]->lpVtbl->field_08(sgalptexSmackMemory[i]);
			sgalptexSmackMemory[i] = NULL;
		}
		if (sgalpddsSmackDevice[i])
		{
			sgalpddsSmackDevice[i]->lpVtbl->field_08(sgalpddsSmackDevice[i]);
			sgalpddsSmackDevice[i] = NULL;
		}
		if (sgalptexSmackDevice[i])
		{
			sgalptexSmackDevice[i]->lpVtbl->field_08(sgalptexSmackDevice[i]);
			sgalptexSmackDevice[i] = NULL;
		}
	}
}

static inline void D3DSmackLoadTextures(void)
{
	if (gtVideoConfig.dwFlags & gdwBitMasks[10])
	{
		D3DCopySurface(sgalpddsSmackDevice[0], sgalpddsSmackMemory[0], 64);
		D3DCopySurface(sgalpddsSmackDevice[1], sgalpddsSmackMemory[1], 64);
	}
	else
	{
		sgalptexSmackDevice[0]->lpVtbl->field_14(sgalptexSmackDevice[0], sgalptexSmackMemory[0]);
		sgalptexSmackDevice[1]->lpVtbl->field_14(sgalptexSmackDevice[1], sgalptexSmackMemory[1]);
	}
}

void __fastcall D3DSmackOpen(const char *pszFileName, D3DSMACKVIDEO *ptVideo)
{
	SMACK *ptSmack;

	if (sgfSmackActive)
	{
		ptVideo->ptSmack = NULL;
		return;
	}
	if (!gfSuspended)
	{
#line 150
		D2ASSERT(sglpSmackBigBuffer);
		memset(sglpSmackBigBuffer, 0, 65536);
		D3DSmackUpload();
		D3DSmackLoadTextures();
	}
	if (ptVideo->ptSmack)
		return;
	ptSmack = (SMACK *)SmackOpen((DWORD)pszFileName, 1040384, (DWORD)-1);
	ptVideo->ptSmack = ptSmack;
	if (!ptSmack)
	{
		sgfSmackActive = FALSE;
		return;
	}
	SmackToBuffer((DWORD)ptSmack, 0, 0, 1024, 128, (DWORD)sglpSmackBigBuffer, 0x80000000);
	sgfSmackActive = TRUE;
}

void __fastcall D3DSmackUpdate(D3DSMACKVIDEO *ptVideo)
{
	SMACK *ptSmack;
	int nLeft;
	int nRight;
	int nTop;
	int nBottom;
	float flU;

	ptSmack = ptVideo->ptSmack;
	if (!ptSmack)
		return;
	if (!SmackWait((DWORD)ptSmack))
	{
		if (!ghWnd)
			return;
		SmackDoFrame((DWORD)ptSmack);
		D3DSmackUpload();
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
		D3DSmackLoadTextures();
	}
	if (gfSuspended)
		return;
	ptSmack = ptVideo->ptSmack;
	nTop = ptVideo->nY - ptSmack->dwHeight;
	nBottom = ptVideo->nY;
	D3DSetBlendMode(1);
	D3DSetAlphaMode(1);
	D3DSetFilterMode(1);
	nLeft = ptVideo->nX;
	nRight = nLeft + 256;
	sgatSmackVertex[0].flX = (float)nLeft;
	sgatSmackVertex[0].flY = (float)nTop;
	sgatSmackVertex[0].field_08 = 0.5f;
	sgatSmackVertex[0].field_0c = 1.0f;
	sgatSmackVertex[0].flU = 0.0f;
	sgatSmackVertex[0].flV = 0.0f;
	sgatSmackVertex[1].flX = (float)nLeft;
	sgatSmackVertex[1].flY = (float)nBottom;
	sgatSmackVertex[1].field_08 = 0.5f;
	sgatSmackVertex[1].field_0c = 1.0f;
	sgatSmackVertex[1].flU = 0.0f;
	sgatSmackVertex[1].flV = 1.0f;
	sgatSmackVertex[2].flX = (float)nRight;
	sgatSmackVertex[2].flY = (float)nBottom;
	sgatSmackVertex[2].field_08 = 0.5f;
	sgatSmackVertex[2].field_0c = 1.0f;
	sgatSmackVertex[2].flU = 1.0f;
	sgatSmackVertex[2].flV = 1.0f;
	sgatSmackVertex[3].flX = (float)nRight;
	sgatSmackVertex[3].flY = (float)nTop;
	sgatSmackVertex[3].field_08 = 0.5f;
	sgatSmackVertex[3].field_0c = 1.0f;
	sgatSmackVertex[3].flU = 1.0f;
	sgatSmackVertex[3].flV = 0.0f;
	glpDevice->lpVtbl->field_98(glpDevice, 0, sgalptexSmackDevice[0]);
	glpDevice->lpVtbl->field_70(glpDevice, 6, 452, sgatSmackVertex, 4, 28);
	nLeft += 256;
	nRight = ptSmack->dwWidth + nLeft - 256;
	flU = (float)(ptSmack->dwWidth - 256) / 256.0f;
	sgatSmackVertex[0].flX = (float)nLeft;
	sgatSmackVertex[0].flY = (float)nTop;
	sgatSmackVertex[0].field_08 = 0.5f;
	sgatSmackVertex[0].field_0c = 1.0f;
	sgatSmackVertex[0].flU = 0.0f;
	sgatSmackVertex[0].flV = 0.0f;
	sgatSmackVertex[1].flX = (float)nLeft;
	sgatSmackVertex[1].flY = (float)nBottom;
	sgatSmackVertex[1].field_08 = 0.5f;
	sgatSmackVertex[1].field_0c = 1.0f;
	sgatSmackVertex[1].flU = 0.0f;
	sgatSmackVertex[1].flV = 1.0f;
	sgatSmackVertex[2].flX = (float)nRight;
	sgatSmackVertex[2].flY = (float)nBottom;
	sgatSmackVertex[2].field_08 = 0.5f;
	sgatSmackVertex[2].field_0c = 1.0f;
	sgatSmackVertex[2].flU = flU;
	sgatSmackVertex[2].flV = 1.0f;
	sgatSmackVertex[3].flX = (float)nRight;
	sgatSmackVertex[3].flY = (float)nTop;
	sgatSmackVertex[3].field_08 = 0.5f;
	sgatSmackVertex[3].field_0c = 1.0f;
	sgatSmackVertex[3].flU = flU;
	sgatSmackVertex[3].flV = 0.0f;
	glpDevice->lpVtbl->field_98(glpDevice, 0, sgalptexSmackDevice[1]);
	glpDevice->lpVtbl->field_70(glpDevice, 6, 452, sgatSmackVertex, 4, 28);
}

void __fastcall D3DSmackClose(D3DSMACKVIDEO *ptVideo)
{
	if (ptVideo->ptSmack)
	{
		SmackClose((DWORD)ptVideo->ptSmack);
		ptVideo->ptSmack = NULL;
	}
	sgfSmackActive = FALSE;
}

static void __fastcall D3DSmackUpload(void)
{
	D3DSURFDESC tDesc;
	HRESULT hResult;
	const WORD *pwFrom;
	const WORD *pwRow;
	WORD *pwTo;
	BYTE *pbTo;
	int nPitch;
	int nRows;
	int x;

	tDesc.dwSize = sizeof(tDesc);
	memset(&tDesc.dwFlags, 0, sizeof(tDesc) - 4);
	hResult = sgalpddsSmackMemory[0]->lpVtbl->pfnLock(sgalpddsSmackMemory[0], NULL, &tDesc, 33, NULL);
	if (hResult)
		D3DLogDDError("Failed to lock memory surface", hResult);
	nPitch = tDesc.nPitch;
	pbTo = tDesc.pbSurface;
	pwFrom = (const WORD *)sglpSmackBigBuffer;
	for (nRows = 64; nRows; nRows--)
	{
		pwRow = pwFrom;
		pwTo = (WORD *)pbTo;
		for (x = 0; x < 256; x++)
			*pwTo++ = *pwRow++ | 32768;
		pwFrom += 512;
		pbTo += nPitch;
	}
	hResult = sgalpddsSmackMemory[0]->lpVtbl->pfnUnlock(sgalpddsSmackMemory[0], NULL);
	if (hResult)
		D3DLogDDError("Failed to unlock memory surface", hResult);
	hResult = sgalpddsSmackMemory[1]->lpVtbl->pfnLock(sgalpddsSmackMemory[1], NULL, &tDesc, 33, NULL);
	if (hResult)
		D3DLogDDError("Failed to lock memory surface", hResult);
	nPitch = tDesc.nPitch;
	pbTo = tDesc.pbSurface;
	pwFrom = (const WORD *)(sglpSmackBigBuffer + 512);
	for (nRows = 64; nRows; nRows--)
	{
		pwRow = pwFrom;
		pwTo = (WORD *)pbTo;
		for (x = 0; x < 256; x++)
			*pwTo++ = *pwRow++ | 32768;
		pwFrom += 512;
		pbTo += nPitch;
	}
	hResult = sgalpddsSmackMemory[1]->lpVtbl->pfnUnlock(sgalpddsSmackMemory[1], NULL);
	if (hResult)
		D3DLogDDError("Failed to unlock memory surface", hResult);
}
