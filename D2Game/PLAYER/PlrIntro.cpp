#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\Player\\PlrIntro.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "FogImports.h"

struct PLRINTRONPC
{
	int		nClass;
	int		nBit;
};
static PLRINTRONPC sgatPlrIntroNpcs[29] =
{
	{ 410, 0 }, { 147, 1 }, { 148, 2 }, { 150, 3 }, { 155, 4 }, { 154, 5 },
	{ 265, 6 }, { 175, 7 }, { 176, 8 }, { 177, 9 }, { 178, 10 }, { 202, 11 },
	{ 200, 12 }, { 210, 13 }, { 201, 14 }, { 198, 15 }, { 199, 16 }, { 244, 17 },
	{ 245, 18 }, { 246, 19 }, { 251, 20 }, { 252, 21 }, { 253, 22 }, { 254, 23 },
	{ 255, 24 }, { 256, 25 }, { 257, 26 }, { 264, 27 }, { 297, 28 },
};
static int sgnPlrIntroNpcs = 29;

extern int PlrIntroGetBit(int nClass);
__inline int PlrIntroGetBit(int nClass)
{
	int i;

	for (i = 0; i < sgnPlrIntroNpcs; i++)
		if (sgatPlrIntroNpcs[i].nClass == nClass)
			return sgatPlrIntroNpcs[i].nBit;
	return 0;
}

void __fastcall PlrIntroSetMet(UNIT *hUnit, GAME *ptGame, int nClass)
{
	PLRINTRO *ptIntro;
	ptIntro = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->ptIntro[ptGame->bDifficultyLevel];
	FogBitManipSetBit(ptIntro->ptBits0->pbData, PlrIntroGetBit(nClass));
}

DWORD __fastcall PlrIntroTestMet(UNIT *hUnit, GAME *ptGame, int nClass)
{
	PLRINTRO *ptIntro;
	ptIntro = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->ptIntro[ptGame->bDifficultyLevel];
	return FogBitManipTestBit(ptIntro->ptBits0->pbData, PlrIntroGetBit(nClass));
}

void __fastcall PlrIntroSetIntroduced(PLRINTRO *ptIntro, int nClass)
{
	FogBitManipSetBit(ptIntro->ptBits1->pbData, PlrIntroGetBit(nClass));
}

DWORD __fastcall PlrIntroTestIntroduced(PLRINTRO *ptIntro, int nClass)
{
	return FogBitManipTestBit(ptIntro->ptBits1->pbData, PlrIntroGetBit(nClass));
}

void __fastcall PlrIntroClearIntroduced(GAME *ptGame, UNIT *hUnit, int nClass)
{
	PLRINTRO *ptIntro;
	ptIntro = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->ptIntro[ptGame->bDifficultyLevel];
	FogBitManipClearBit(ptIntro->ptBits1->pbData, PlrIntroGetBit(nClass));
}

void __fastcall PlrIntroGetMetBits(PLRINTRO *ptIntro, DWORD *pdwBits)
{
	*(__int64 *)pdwBits = *(__int64 *)ptIntro->ptBits0->pbData;
}

void __fastcall PlrIntroGetIntroducedBits(PLRINTRO *ptIntro, DWORD *pdwBits)
{
	*(__int64 *)pdwBits = *(__int64 *)ptIntro->ptBits1->pbData;
}

void __fastcall PlrIntroSetMetBits(PLRINTRO *ptIntro, const DWORD *pdwBits)
{
	*(__int64 *)ptIntro->ptBits0->pbData = *(__int64 *)pdwBits;
}

void __fastcall PlrIntroSetIntroducedBits(PLRINTRO *ptIntro,
	const DWORD *pdwBits)
{
	*(__int64 *)ptIntro->ptBits1->pbData = *(__int64 *)pdwBits;
}

PLRINTRO * __fastcall PlrIntroNew(GAME *ptGame)
{
	PLRINTRO *ptIntro;
	BITBUFFER *ptBits;
	DWORD *pdwData;

#line 186
	ptIntro = (PLRINTRO *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(PLRINTRO));
#line 187
	ptBits = (BITBUFFER *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(BITBUFFER));
#line 188
	pdwData = (DWORD *)D2ALLOCPOOL(ptGame->pMemPool, 8);
	memset(pdwData, 0, 8);
	FogBitManipInitBuffer(ptBits, pdwData, 8);
	ptIntro->ptBits0 = ptBits;
#line 193
	ptBits = (BITBUFFER *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(BITBUFFER));
#line 194
	pdwData = (DWORD *)D2ALLOCPOOL(ptGame->pMemPool, 8);
	memset(pdwData, 0, 8);
	FogBitManipInitBuffer(ptBits, pdwData, 8);
	ptIntro->ptBits1 = ptBits;
	return ptIntro;
}

void __fastcall PlrIntroFree(GAME *ptGame, PLRINTRO *ptIntro)
{
	if (ptIntro->ptBits0)
	{
#line 209
		D2FREEPOOL(ptGame->pMemPool, ptIntro->ptBits0->pbData);
#line 210
		D2FREEPOOL(ptGame->pMemPool, ptIntro->ptBits0);
		ptIntro->ptBits0 = NULL;
	}
	if (ptIntro->ptBits1)
	{
#line 215
		D2FREEPOOL(ptGame->pMemPool, ptIntro->ptBits1->pbData);
#line 216
		D2FREEPOOL(ptGame->pMemPool, ptIntro->ptBits1);
		ptIntro->ptBits1 = NULL;
	}
#line 219
	D2FREEPOOL(ptGame->pMemPool, ptIntro);
}

BOOL __fastcall PlrCanBeInterrupted(UNIT *hUnit, BOOL bCheckSkill,
	BOOL bCheckChance)
{
	void *hSkill;
	SKILLSTXT *ptSkillsTxt;
	int nChance;

	if (D2CommonStatsGetState(hUnit, 54))
		return FALSE;
	if (!hUnit->dwMode || hUnit->dwMode == 17)
		return FALSE;
	if (!bCheckSkill)
		return TRUE;
	if (bCheckChance)
	{
		hSkill = (void *)D2CommonUnitGetSkillThirdCached(hUnit);
		if (!hSkill)
			return TRUE;
		ptSkillsTxt = (SKILLSTXT *)D2CommonDataTblsGetSkill(D2CommonSkillsGetNodeType(hSkill));
		if (!ptSkillsTxt->Interrupt)
			return FALSE;
		if (D2CommonStatsGetState(hUnit, 42))
		{
			DWORD hStateList = D2CommonStatsFindListByState(hUnit, 42);
			nChance = D2CommonStatsListGetValueA(hStateList, 164);
			if ((int)D2CommonSeedRand(&hUnit->tSeed, 100) < nChance)
				return FALSE;
		}
	}
	return TRUE;
}
