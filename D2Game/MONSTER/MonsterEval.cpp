#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\MONSTER\\MonsterEval.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "MonsterEval.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "../SKILLS/Skills.h"

#define NUM_DIFFICULTY_LEVELS	3
#define UNIT_MONSTER			1

static FORMATIONPOINT sgatFormation0[5] =
{
	{ 4, 0, 0 }, { 1, 0, 0 }, { 0, 1, 0 }, { 2, 1, 0 }, { 1, 2, 0 },
};
static const FORMATIONPOINT *sgaptFormations[1] =
{
	sgatFormation0,
};

FORMATIONPOINT * __fastcall MonsterPickFormation(GAMEROOM *hRoom)
{
	D2SEED *ptSeed;
	if (hRoom)
		ptSeed = &ROOM5(hRoom)->tSeed;
	else
		ptSeed = 0;
	return (FORMATIONPOINT *)sgaptFormations[D2CommonSeedRand(ptSeed, 1)];
}

int __fastcall MonsterRegionGetKind(MONREGION *ptRegion, GAMEROOM *hRoom)
{
	BYTE bCount;
	BYTE bMax2;
	int nRooms;
	int nPop;

	bMax2 = ptRegion->bMonUMax;
	bCount = (BYTE)ptRegion->nUniques;
	if (bCount < ptRegion->bMonUMin && (nRooms = ptRegion->nRooms) != 0)
	{
		nPop = ptRegion->dwRoomsPopulated;
		if ((int)D2CommonSeedRand(hRoom ? (DWORD)&ROOM5(hRoom)->tSeed : 0, 100)
			< (int)(nPop * 100) / nRooms)
			return 0;
	}
	if (bCount < bMax2)
	{
		if ((int)D2CommonSeedRand(hRoom ? (DWORD)&ROOM5(hRoom)->tSeed : 0, 100) <= 5)
			return 0;
	}
	return ((int)D2CommonSeedRand(hRoom ? (DWORD)&ROOM5(hRoom)->tSeed : 0, 100) > 35) + 1;
}

BOOL __fastcall MonsterEvalIsAlive(UNIT *hUnit)
{
	if (hUnit->eType != UNIT_MONSTER)
		return FALSE;
	return D2CommonStatsUnitGetValue(hUnit, 6) > 0;
}

void __fastcall MonsterEvalKilled(UNIT *hKiller  , UNIT *hUnit, GAME *ptGame)
{
	int nStat13;

#line 31
	D2ASSERT(hKiller);
	if ((hUnit->dwFlags0 & 67108864) == 67108864)
		return;
	nStat13 = D2CommonStatsUnitGetBaseValue(hUnit, 13);
	DWORD nBase = D2CommonStatsUnitGetBaseValue(hUnit, 12);
	SUnitDmgAwardExp(ptGame, hKiller, hUnit, nBase, nStat13);
}

void __fastcall MonsterEvalSetDamage(UNIT *hUnit, int eMode)
{
	MONSTATSTXT *ptStats;
	int anBonus[4];
	DWORD dwStatList;
	GAME *ptGame;
	int nDifficulty;
	int nMinDamage;
	int nMaxDamage;
	int nToHit;
	int nElMin;
	int nElMax;
	int nElDur;
	BYTE bElType;

	ptStats = ((MONSTERDATA *)(hUnit->uTypeData))->ptMonStats;
	memset(anBonus, 0, sizeof(anBonus));
	SkillsGetDamage(hUnit, eMode, anBonus);
	dwStatList = D2CommonStatsFindListByMask(hUnit, 1);
	D2CommonStatsListReset(dwStatList);
	ptGame = SUnitGetGame(hUnit);
	nDifficulty = ptGame->bDifficultyLevel;
#line 81
	D2ASSERT(ptGame->bDifficultyLevel < NUM_DIFFICULTY_LEVELS);
	switch (eMode)
	{
	case 4:
		nMinDamage = ptStats->pnAttack1MinDamage[nDifficulty] + anBonus[0];
		nMaxDamage = ptStats->pnAttack1MaxDamage[nDifficulty] + anBonus[1];
		nToHit = ptStats->A1ToHit[nDifficulty] + anBonus[2];
		goto setdamage;
	case 5:
		nMinDamage = ptStats->A2MinD[nDifficulty] + anBonus[0];
		nMaxDamage = ptStats->A2MaxD[nDifficulty] + anBonus[1];
		nToHit = ptStats->A2ToHit[nDifficulty] + anBonus[2];
		goto setdamage;
	case 8:
		nMinDamage = ptStats->S1MinD[nDifficulty] + anBonus[0];
		nMaxDamage = ptStats->S1MaxD[nDifficulty] + anBonus[1];
		nToHit = ptStats->S1ToHit[nDifficulty] + anBonus[2];
	setdamage:
		D2CommonStatsListSetValueB(dwStatList, 21, nMinDamage);
		D2CommonStatsListSetValueB(dwStatList, 22, nMaxDamage);
		D2CommonStatsListSetValueB(dwStatList, 19, nToHit);
		break;
	}
	D2CommonStatsListSetValueB(dwStatList, 20, anBonus[3]);
	if (ptStats->ElMode <= 0 || eMode != ptStats->ElMode)
		return;
	if (ptStats->ElPct < 100 && D2CommonSeedRand(&hUnit->tSeed, 100) >= ptStats->ElPct)
		return;
	nElMin = ptStats->ElMinD;
	nElMax = ptStats->ElMaxD;
	nElDur = ptStats->ElDur;
	bElType = ptStats->ElType;
	if (bElType == 10)
		bElType = (BYTE)(D2CommonSeedRand(&hUnit->tSeed, 5) + 1);
	switch (bElType)
	{
	case 1:
		D2CommonStatsListSetValueB(dwStatList, 48, nElMin);
		D2CommonStatsListSetValueB(dwStatList, 49, nElMax);
		break;
	case 2:
		D2CommonStatsListSetValueB(dwStatList, 50, nElMin);
		D2CommonStatsListSetValueB(dwStatList, 51, nElMax);
		break;
	case 3:
		D2CommonStatsListSetValueB(dwStatList, 52, nElMin);
		D2CommonStatsListSetValueB(dwStatList, 53, nElMax);
		break;
	case 4:
		D2CommonStatsListSetValueB(dwStatList, 54, nElMin);
		D2CommonStatsListSetValueB(dwStatList, 55, nElMax);
		D2CommonStatsListSetValueB(dwStatList, 56, nElDur);
		break;
	case 5:
		D2CommonStatsListSetValueB(dwStatList, 57, nElMin);
		D2CommonStatsListSetValueB(dwStatList, 58, nElMax);
		D2CommonStatsListSetValueB(dwStatList, 59, nElDur * 5);
		break;
	case 6:
		D2CommonStatsListSetValueB(dwStatList, 60, nElMin);
		D2CommonStatsListSetValueB(dwStatList, 61, nElMax);
		break;
	case 7:
		D2CommonStatsListSetValueB(dwStatList, 62, nElMin);
		D2CommonStatsListSetValueB(dwStatList, 63, nElMax);
		break;
	case 8:
		D2CommonStatsListSetValueB(dwStatList, 64, nElMin);
		D2CommonStatsListSetValueB(dwStatList, 65, nElMax);
		break;
	case 9:
		D2CommonStatsListSetValueB(dwStatList, 66, nElDur);
		break;
	}
}

WORD __fastcall MonsterEvalHit(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg,
	DWORD dwUnused)
{
	if (!dwUnused)
		return SUnitDmgHit(ptGame, hUnit, hTarget, (int)dwArg);
	return SUnitDmgHit(ptGame, hUnit, hTarget, (int)dwArg);
}

void __fastcall MonsterEvalAttack(UNIT *hUnit, GAME *ptGame)
{
	UNIT *hTarget;
	DAMAGE tDamage;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = (WORD)SUnitDmgHit(ptGame, hUnit, hTarget, 0);
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
}

void __fastcall MonsterEvalDamageFlags(GAME *ptGame, UNIT *hAttacker, UNIT *hUnit,
	DAMAGE *ptDamage)
{
	if (!(ptDamage->wFlags & 4))
		return;
	if (D2CommonMonsterUnitGetTypeLocalIndex(hUnit) != 78)
		return;
	if ((ptDamage->wFlags & 4) && !D2CommonStatsGetState(hUnit, 1))
		ptDamage->wFlags |= 8;
}
