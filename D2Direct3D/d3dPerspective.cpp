#define D2_THIS_FILE "C:\\D2\\Source\\D2Direct3D\\Src\\d3dPerspective.cpp"
#include "d3dCaches.h"
#include "StormImports.h"
#include "D2Direct3DImports.h"
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
static float sgafPerspSinA[ANGLE_TABLE_SIZE];
static float sgafPerspCosA[ANGLE_TABLE_SIZE];
static float sgafPerspSinB[ANGLE_TABLE_SIZE];
static float sgafPerspCosB[ANGLE_TABLE_SIZE];
static int sgnPerspOriginX;
static int sgnPerspOriginY;
int gnPerspDepth = 0;

static inline void D3DPerspFloatToInt(float fValue, int *pnValue)
{
	float fSum;

	if (!(!(fValue >= 0.0f))) {
		fSum = fValue + 12582912.0f;
		*pnValue = *(int *)&fSum & 4194303; }
	else
		*pnValue = (int)fValue;
}

void WINAPI D3DPerspectiveInit(void)
{
	int i;
	float nOffset;

	for (i = 0; i < ANGLE_TABLE_SIZE; i++)
	{
		nOffset = (float)(i - ANGLE_TABLE_SIZE / 2);
		sgafPerspSinA[i] = gfPerspSinA * nOffset;
		sgafPerspCosA[i] = gfPerspCosA * nOffset;
		sgafPerspSinB[i] = gfPerspSinB * nOffset;
		sgafPerspCosB[i] = gfPerspCosB * nOffset;
	}
	gnPerspDepth = 256;
}

DWORD WINAPI D3DGetPerspective(void)
{
	if (gptDrawCtx->dwPerspectiveAllowed)
		return gptDrawCtx->dwPerspective;
	return 0;
}

void __fastcall D3DSetPerspectiveOrigin(int nX, int nY)
{
	sgnPerspOriginX = nX;
	sgnPerspOriginY = nY;
}

void __fastcall D3DPerspectiveProject(int nX, int nY, int nZ, int *pnX, int *pnY)
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
#line 114
	D2ASSERT(nX >= 0 && nX < ANGLE_TABLE_SIZE);
#line 115
	D2ASSERT(nY >= 0 && nY < ANGLE_TABLE_SIZE);
	fAcross = sgafPerspCosB[nX];
	fAcross += sgafPerspSinB[nY];
	D3D_FLOAT_STORE(fAcross);
	fDown = sgafPerspSinB[nX] - sgafPerspCosB[nY];
	D3D_FLOAT_STORE(fDown);
	if (!nZ)
	{
		fHeight = -gfPerspCosA * fDown;
		fDepth = gfPerspSinA * fDown;
	}
	else
	{
		nZ = ANGLE_TABLE_SIZE / 2 - (int)(nZ * -1.2f);
#line 129
		D2ASSERT(nZ >= 0 && nZ < ANGLE_TABLE_SIZE);
		fHeight = -gfPerspCosA * fDown;
		fHeight += sgafPerspSinA[nZ];
		fDepth = gfPerspSinA * fDown + sgafPerspCosA[nZ];
	}
	fScale = 3000.0f / (fDepth - 3000.0f);
	fScreenX = fScale * fAcross + 320.0f;
	D3D_FLOAT_STORE(fScreenX);
	fScreenY = fScale * fHeight + 240.0f - 32.0f;
	D3DPerspFloatToInt(fScreenX, pnX);
	D3DPerspFloatToInt(fScreenY, pnY);
}

void __fastcall D3DPerspectiveProjectDepth(int nX, int nY, int nZ, int *pnX, int *pnY, BOOL fFar)
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
#line 156
	D2ASSERT(nX >= 0 && nX < ANGLE_TABLE_SIZE);
#line 157
	D2ASSERT(nY >= 0 && nY < ANGLE_TABLE_SIZE);
	fDown = sgafPerspSinB[nX] - sgafPerspCosB[nY];
	fHeight = -(gfPerspCosA * fDown);
	D3D_FLOAT_STORE(fHeight);
	fDepth = fDown * gfPerspSinA;
	fScale = 3000.0f / (fDepth - 3000.0f);
	fScreenX = (sgafPerspSinB[nY] + sgafPerspCosB[nX]) * fScale + 320.0f;
	D3D_FLOAT_STORE(fScreenX);
	fOffset = fScale * fHeight;
	fScreenY = fScale * fHeight + 240.0f - 16.0f;
	D3DPerspFloatToInt(fScreenX, pnX);
	D3DPerspFloatToInt(fScreenY, pnY);
	fDelta = fOffset - 3000.0f / (fDepth + gfPerspCosA256 - 3000.0f) * (gfPerspSinA256 + fHeight);
	if (fFar)
		fDelta *= 1.2f;
	else
		fDelta *= 1.1f;
	D3DPerspFloatToInt(fDelta, &gnPerspDepth);
}

void WINAPI D3DResetPerspectiveDepth(void)
{
	gnPerspDepth = 256;
}
