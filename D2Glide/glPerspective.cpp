#define D2_THIS_FILE "C:\\D2\\Source\\D2Glide\\Src\\glPerspective.cpp"
#include "glCore.h"
#include "StormImports.h"
#include "D2GlideImports.h"
#include "D2Macros.h"
#include <math.h>

#define ANGLE_TABLE_SIZE	4096
#define PERSP_ANGLE_A		1.0471975803375244
#define PERSP_ANGLE_B		2.35619455575943

float gfPerspSinA = (float)sin(PERSP_ANGLE_A);
float gfPerspCosA = (float)cos(PERSP_ANGLE_A);
float gfPerspSinB = (float)sin(PERSP_ANGLE_B);
float gfPerspCosB = (float)cos(PERSP_ANGLE_B);
float gfPerspSinA256 = gfPerspSinA * 256.0f;
float gfPerspCosA256 = gfPerspCosA * 256.0f;
int gnPerspDepth = 0;
static float *sgpfPerspSinA = 0;
static float *sgpfPerspCosA = 0;
static float *sgpfPerspSinB = 0;
static float *sgpfPerspCosB = 0;
static int sgnPerspOriginX;
static int sgnPerspOriginY;

void __fastcall GlidePerspectiveOpen(void)
{
#line 62
	sgpfPerspSinA = (float *)D2ALLOC2(ANGLE_TABLE_SIZE * sizeof(float));
#line 63
	sgpfPerspCosA = (float *)D2ALLOC2(ANGLE_TABLE_SIZE * sizeof(float));
#line 64
	sgpfPerspSinB = (float *)D2ALLOC2(ANGLE_TABLE_SIZE * sizeof(float));
#line 65
	sgpfPerspCosB = (float *)D2ALLOC2(ANGLE_TABLE_SIZE * sizeof(float));
}

void __fastcall GlidePerspectiveClose(void)
{
	if (sgpfPerspSinA)
	{
#line 70
		D2FREE2(sgpfPerspSinA);
	}
	if (sgpfPerspCosA)
	{
#line 71
		D2FREE2(sgpfPerspCosA);
	}
	if (sgpfPerspSinB)
	{
#line 72
		D2FREE2(sgpfPerspSinB);
	}
	if (sgpfPerspCosB)
	{
#line 73
		D2FREE2(sgpfPerspCosB);
	}
}

static inline void GlidePerspFloatToInt(float fValue, int *pnValue)
{
	float fSum;

	if (!(!(fValue >= 0.0f))) {
		fSum = fValue + 12582912.0f;
		*pnValue = *(int *)&fSum & 4194303; }
	else
		*pnValue = (int)fValue;
}

void __fastcall GlidePerspectiveInit(void)
{
	int i;
	float nOffset;

	for (i = 0; i < ANGLE_TABLE_SIZE; i++)
	{
		nOffset = (float)(i - ANGLE_TABLE_SIZE / 2);
		sgpfPerspSinA[i] = gfPerspSinA * nOffset;
		sgpfPerspCosA[i] = gfPerspCosA * nOffset;
		sgpfPerspSinB[i] = gfPerspSinB * nOffset;
		sgpfPerspCosB[i] = gfPerspCosB * nOffset;
	}
	gnPerspDepth = 256;
}

DWORD __fastcall GlideGetPerspective(void)
{
	if (gptDrawCtx->dwPerspectiveAllowed)
		return gptDrawCtx->dwPerspective;
	return 0;
}

void __fastcall GlideSetPerspectiveOrigin(int nX, int nY)
{
	sgnPerspOriginX = nX;
	sgnPerspOriginY = nY;
}

void __fastcall GlidePerspectiveProject(int nX, int nY, int nZ, int *pnX, int *pnY)
{
	float fAcross;
	float fDown;
	float fScreenX;
	float fScale;
	float fScreenY;
	float fDepth;
	float fHeight;

	nX = ((nX - sgnPerspOriginX) * 24 >> 16) + ANGLE_TABLE_SIZE / 2;
	nY = ((nY - sgnPerspOriginY) * 24 >> 16) + ANGLE_TABLE_SIZE / 2;
#line 134
	D2ASSERT(nX >= 0 && nX < ANGLE_TABLE_SIZE);
#line 135
	D2ASSERT(nY >= 0 && nY < ANGLE_TABLE_SIZE);
	fAcross = sgpfPerspCosB[nX] + sgpfPerspSinB[nY];
	GL_FLOAT_STORE(fAcross);
	fDown = sgpfPerspSinB[nX] - sgpfPerspCosB[nY];
	GL_FLOAT_STORE(fDown);
	if (!nZ)
	{
		fHeight = -gfPerspCosA * fDown;
		fDepth = gfPerspSinA * fDown;
	}
	else
	{
		nZ = ANGLE_TABLE_SIZE / 2 - (int)(nZ * -1.2f);
#line 149
		D2ASSERT(nZ >= 0 && nZ < ANGLE_TABLE_SIZE);
		fHeight = -gfPerspCosA * fDown;
		fHeight += sgpfPerspSinA[nZ];
		fDepth = gfPerspSinA * fDown + sgpfPerspCosA[nZ];
	}
	fScale = 3000.0f / (fDepth - 3000.0f);
	fScreenX = fScale * fAcross + 320.0f;
	GL_FLOAT_STORE(fScreenX);
	fScreenY = fScale * fHeight + 240.0f - 32.0f;
	GlidePerspFloatToInt(fScreenX, pnX);
	GlidePerspFloatToInt(fScreenY, pnY);
}

void __fastcall GlidePerspectiveProjectDepth(int nX, int nY, int nZ, int *pnX, int *pnY, BOOL fFar)
{
	float fDown;
	float fHeight;
	float fScreenX;
	float fOffset;
	float fDepth;
	float fScale;
	float fScreenY;
	float fDelta;

	nX = ((nX - sgnPerspOriginX) * 24 >> 16) + ANGLE_TABLE_SIZE / 2;
	nY = ((nY - sgnPerspOriginY) * 24 >> 16) + ANGLE_TABLE_SIZE / 2;
#line 176
	D2ASSERT(nX >= 0 && nX < ANGLE_TABLE_SIZE);
#line 177
	D2ASSERT(nY >= 0 && nY < ANGLE_TABLE_SIZE);
	fDown = sgpfPerspSinB[nX] - sgpfPerspCosB[nY];
	fHeight = -(gfPerspCosA * fDown);
	GL_FLOAT_STORE(fHeight);
	fDepth = fDown * gfPerspSinA;
	fScale = 3000.0f / (fDepth - 3000.0f);
	fScreenX = (sgpfPerspSinB[nY] + sgpfPerspCosB[nX]) * fScale + 320.0f;
	GL_FLOAT_STORE(fScreenX);
	fOffset = fScale * fHeight;
	fScreenY = fScale * fHeight + 240.0f - 16.0f;
	GlidePerspFloatToInt(fScreenX, pnX);
	GlidePerspFloatToInt(fScreenY, pnY);
	fDelta = fOffset - 3000.0f / (fDepth + gfPerspCosA256 - 3000.0f) * (gfPerspSinA256 + fHeight);
	if (fFar)
		fDelta *= 1.2f;
	else
		fDelta *= 1.1f;
	GlidePerspFloatToInt(fDelta, &gnPerspDepth);
}

void __cdecl GlideResetPerspectiveDepth(void)
{
	gnPerspDepth = 256;
}
