#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\MONSTER\\Monster.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "../AI/AiThink.h"
#include "Monster.h"
#include "MonsterAI.h"
#include "MonsterMode.h"
#include "../PLAYER/Player.h"
#include "../QUESTS/QUESTS.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "../UNIT/sunitproxy.h"
#include "../SKILLS/Skills.h"

#define UNIT_MONSTER		1
#define EVENT_RUN_AI		2
#define NUM_DIFFICULTY_LEVELS	3

struct MONITEMSTXT
{
 	BYTE		unobserved_00[42];
 	BYTE		type;
};

static DWORD sgadwNoShieldCodes[1] =
{
	543712116,
};
static DWORD sgnNoShieldCodes = 1;

MONSTERREGIONREF *__fastcall MonsterAllocRegionRef(GAME *ptGame)
{
	MONSTERREGIONREF *ptRef;
	int i;

#line 33
	ptRef = (MONSTERREGIONREF *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(MONSTERREGIONREF));
	for (i = 0; i < 3; i++)
		((DWORD *)ptRef)[i] = 0;
	return ptRef;
}

void __fastcall MonsterFreeRegionRef(GAME *ptGame, MONSTERREGIONREF *ptRef)
{
#line 41
	D2FREEPOOL(ptGame->pMemPool, ptRef);
}

void __fastcall MonsterRegionRefSetPrevMode(MONSTERREGIONREF *ptRef, DWORD dwValue)
{
	ptRef->dwPrevMode = dwValue;
}

void __fastcall MonsterRegionRefSetLevel(MONSTERREGIONREF *ptRef, DWORD dwLevelId)
{
	ptRef->dwLevelId = dwLevelId;
}

static void MonsterSetupStats(GAME *ptGame, GAMEROOM *hRoom, MONPOPSLOT *ptSlot, UNIT *hUnit)
{
	int eClass;
	BYTE bComponent;
	MONSTERSCALE tScale;
	int nDifficulty;
	MONSTATSTXT *ptStats;
	int nExp;
	int nMin;
	int nRange;
	int nRoll;
	int nLife;
	int nRegen;
	void *hList;
	int i;

	eClass = hUnit->dwClassId;
	MonsterInitComponents(ptSlot, hUnit);
	if (eClass == 312)
	{
		bComponent = (BYTE)D2CommonSeedRand(&hUnit->tSeed, 4);
		hUnit->abComponentVariations[10] = bComponent;
		hUnit->abComponentVariations[11] = bComponent;
	}
	else if (eClass == 311)
		hUnit->abComponentVariations[10] = (BYTE)D2CommonSeedRand(&hUnit->tSeed, 4);
	MonsterGetScale(&tScale, ptGame, (DWORD)hRoom, (DWORD)hUnit);
	ptStats = ((MONSTERDATA *)hUnit->uTypeData)->ptMonStats;
	nDifficulty = ptGame->bDifficultyLevel;
#line 142
	D2ASSERT(ptGame->bDifficultyLevel < NUM_DIFFICULTY_LEVELS);
	D2CommonStatsUnitAllocStats(hUnit, 0);
	D2CommonStatsUnitSetValue(hUnit, 12, ptStats->Level[nDifficulty]);
	D2CommonStatsUnitSetValue(hUnit, 31, ptStats->AC[nDifficulty]);
	D2CommonStatsUnitSetValue(hUnit, 36, ptStats->DamageResist[nDifficulty]);
	D2CommonStatsUnitSetValue(hUnit, 37, ptStats->MagicResist[nDifficulty]);
	D2CommonStatsUnitSetValue(hUnit, 39, ptStats->FireResist[nDifficulty]);
	D2CommonStatsUnitSetValue(hUnit, 41, ptStats->LightResist[nDifficulty]);
	D2CommonStatsUnitSetValue(hUnit, 43, ptStats->ColdResist[nDifficulty]);
	D2CommonStatsUnitSetValue(hUnit, 45, ptStats->PoisonResist[nDifficulty]);
	D2CommonStatsUnitSetValue(hUnit, 20, ptStats->bToBlock);
	D2CommonStatsUnitSetValue(hUnit, 68, 100);
	D2CommonStatsUnitSetValue(hUnit, 67, 75);
	D2CommonStatsUnitSetValue(hUnit, 69, 100);
	nExp = ptStats->Exp[nDifficulty];
	D2CommonStatsUnitSetValue(hUnit, 13, nExp + (tScale.nExpPlayers - 1) * nExp / 3 * 2);
	nMin = ptStats->anMinHP[nDifficulty] << 8;
	nRange = (ptStats->MaxHP[nDifficulty] << 8) - nMin + 1;
	if (nRange)
		nRoll = (int)D2CommonSeedRand(&hUnit->tSeed, nRange);
	else
		nRoll = 0;
	nLife = (nMin + nRoll) * tScale.nLifePlayers;
	D2CommonStatsUnitSetValue(hUnit, 7, nLife);
	D2CommonStatsUnitSetValue(hUnit, 6, nLife);
	nRegen = ptStats->nDamageRegen * nLife >> 12;
	if (!nRegen)
		nRegen = 1;
	D2CommonStatsUnitSetValue(hUnit, 74, nRegen);
	hList = (void *)D2CommonStatsAllocList(ptGame->pMemPool, 1, 0, 1, hUnit->dwId);
	D2CommonStatsUnitLinkList(hUnit, hList);
	if (ptStats->bInteract)
		hUnit->ptInventory = SUnitProxyGetInventory(ptGame, eClass);
	switch (eClass) { case 291: case 357:
		hUnit->ptInventory = (void *)D2CommonINVENTORYCreate(ptGame->pMemPool, hUnit->dwId, 1, eClass,
			0); break; }
	UNIT8(hUnit)->ptSkills = (void *)D2CommonSkillsAllocNode(ptGame->pMemPool);
	for (i = 0; i < 5; i++)
	{
		if (ptStats->Skill1[i])
			SkillsSetLevel(hUnit, ptStats->Skill1[i], tScale.nSkillBonus + ptStats->Skill1Lvl[i], 1);
	}
	if (eClass == 243)
	{
		SkillsSetLevel(hUnit, SKILL_DIABPRISON, 1, 1);
		SkillsSetLevel(hUnit, SKILL_PRIMEFIREWALL, tScale.nSkillBonus + 8, 1);
	}
	D2CommonMonsterUnitApplyChartEffects(hUnit);
	if (ptStats->bIsAtt)
		hUnit->dwFlags0 |= 4;
	if (ptStats->bPetIgnore)
		hUnit->dwFlags0 |= 1073741824;
}

void __fastcall MonsterInitUnit(GAME *ptGame, GAMEROOM *hRoom, UNIT *hUnit, DWORD dwUnitId)
{
	MONSTERDATA *ptData;
	MONPOPSLOT *ptSlot;

	hUnit->dwId = dwUnitId;
	hUnit->dwFlags0 |= 10;
	ptData = MonsterAICreate(ptGame);
	hUnit->uTypeData = ptData;
	ptData->ptMonStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId);
	ptSlot = (MONPOPSLOT *)D2CommonMonsterRegionFindOrAddSlot(&ptGame->pMonRegions, hRoom, hUnit);
	MonsterSetupStats(ptGame, hRoom, ptSlot, hUnit);
	EventUnitInit(ptGame, hUnit);
	AiThinkSet(ptGame, hUnit, ((MONSTERDATA *)hUnit->uTypeData)->ptAi, 0);
	((MONSTERREGIONREF *)ptData->pRegionRef)->dwLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
	if (hUnit->dwMode && hUnit->dwMode != 12)
		QuestsAttachLevelQuest(ptGame, hUnit, hRoom, 1);
	else
		SUnitSetMode(ptGame, hUnit, hUnit->dwMode);
	SUnitResetAnim(hUnit);
}

void __fastcall MonsterStartMode(GAME *ptGame, UNIT *hUnit, DWORD dwX, DWORD dwY, DWORD dwUnused)
{
	int eMode;
	MONMODECHANGE tChange;

	eMode = hUnit->dwMode;
	D2CommonPathSetVelocity(hUnit->ptPath,
		((MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId))->nVelocity << 8);
	D2CommonUnitUpdateAnimRate(hUnit);
	MonsterModeInit(hUnit, eMode, &tChange);
	tChange.nX = dwX;
	tChange.nY = dwY;
	MonsterModeSet(ptGame, &tChange, 1);
}

void __fastcall GameFreeMonsterUnits(GAME *ptGame)
{
	UNIT **phBucket;
	UNIT *hUnit;
	UNIT *hNext;
	int i;

	phBucket = ptGame->pUnitList[UNIT_MONSTER];
	for (i = 128; i; i--)
	{
		for (hUnit = *phBucket; hUnit; hUnit = hNext)
		{
			hNext = SUnitGetNextInList(hUnit);
			*phBucket = hNext;
			SUnitFree(ptGame, hUnit);
		}
		phBucket++;
	}
}

void __fastcall MonsterUnitFree(GAME *ptGame, UNIT *hUnit)
{
	MONSTATSTXT *ptMonStats;

	SUnitDmgFreeHeld(ptGame, hUnit);
	ptMonStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId);
	if (!ptMonStats->bInteract)
	{
		SUnitFreeInventory(ptGame, hUnit);
		D2CommonINVENTORYDestroy(hUnit->ptInventory);
		hUnit->ptInventory = NULL;
	}
	switch (hUnit->dwClassId)
	{ case 291: case 357:
		SUnitFreeInventory(ptGame, hUnit);
		D2CommonINVENTORYDestroy(hUnit->ptInventory);
		hUnit->ptInventory = NULL; break;
	}
	if (UNIT5(hUnit)->ptChatNode)
		D2CommonChatNodeFree(ptGame->pMemPool, UNIT5(hUnit)->ptChatNode);
	D2CommonStatsUnitFreeStats(hUnit);
	D2CommonSkillsFreeList(hUnit);
	if (hUnit->uTypeData)
		MonsterAIDestroy(ptGame, (MONSTERDATA *)hUnit->uTypeData);
	EventUnitFree(ptGame, hUnit);
}

void __fastcall MonsterClearTarget(GAME *ptGame, UNIT *hUnit, UNIT *hTarget)
{
	if (hTarget == SUnitGetTarget(ptGame, hUnit))
	{
		D2CommonUnitSetTarget(hUnit, 0);
		D2CommonPathSetTarget(hUnit->ptPath, 0);
	}
}

void __fastcall MonsterRestartAi(GAME *ptGame, UNIT *hUnit)
{
	DWORD dwClass;

	EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
	if (hUnit->dwMode == 1)
	{
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
		EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + 2);
	}
	else if (hUnit->dwMode == 12 || hUnit->dwMode == 0)
		return;
	dwClass = D2CommonMonsterUnitGetTypeLocalIndex(hUnit);
	if (dwClass == 110 || dwClass == 136 || dwClass == 118 || dwClass == 247)
	{
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
		EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + 2);
	}
}

void __fastcall MonsterRemoveEvents(GAME *ptGame, UNIT *hUnit)
{
	EventRemoveByType(ptGame, hUnit, 2);
	EventRemoveByType(ptGame, hUnit, 3);
}

void __fastcall MonsterGetScale(MONSTERSCALE *ptScale, GAME *ptGame, DWORD dwUnused1, DWORD dwUnused2)
{
	int nCount;

	if (!ptScale)
		return;
	nCount = PlayerCountAlive(ptGame);
	if (!nCount)
		nCount = 1;
	ptScale->nLifePlayers = nCount;
	ptScale->nExpPlayers = nCount;
	ptScale->nSkillBonus =
		((MONDIFFICULTYTXT *)D2CommonDataTblsGetDifficultyLevel(ptGame->bDifficultyLevel))->MonsterSkillBonus;
#line 429
	D2ASSERT(ptGame->bDifficultyLevel < NUM_DIFFICULTY_LEVELS);
	ptScale->nDifficulty = ptGame->bDifficultyLevel;
}

void __fastcall MonsterInitComponents(MONPOPSLOT *ptSlot, UNIT *hUnit)
{
	D2SEED *ptSeed;
	BYTE *pbVariation;
	BYTE *pb;
	BYTE bComponent;
	int nCount;
	int nRoll;
	DWORD dwCount;
	int k;
	DWORD dwK;
	int i;

#line 445
	D2ASSERT(hUnit);
#line 446
	D2ASSERT(UnitGetType (hUnit) == UNIT_MONSTER);
	pbVariation = ptSlot->abVariations;
	ptSeed = &hUnit->tSeed;
	if (ptSlot)
	{
		for (bComponent = 0; bComponent < 16; bComponent++)
		{
			pb = pbVariation;
			nCount = 0;
			for (k = 3; k; k--, pb += 2)
				if (bComponent == *pb)
					nCount++;
			if (nCount)
				nRoll = (int)D2CommonSeedRand(ptSeed, nCount);
			else
				nRoll = 0;
			for (pb = pbVariation, dwK = 0; dwK < 3; pb += 2, dwK++)
			{
				if (!nRoll && bComponent == *pb)
				{
					hUnit->abComponentVariations[bComponent] = pb[1];
					break;
				}
				if (bComponent == *pb)
					nRoll--;
			}
		}
		return;
	}
	for (k = 16, i = 0, pb = hUnit->abComponentVariations; k; k--, i++, pb++)
	{
		dwCount = D2CommonMonsterRandGetComponentTableByteRaw((DWORD)hUnit, i);
		*pb = dwCount ? D2CommonSeedRand(ptSeed, dwCount) : 0;
	}
}

BOOL __fastcall MonsterHasComponents(UNIT *hUnit)
{
	int i;

	for (i = 0; i < 16; i++)
	{
		if (hUnit->abComponentVariations[(BYTE)i])
			break;
	}
	if (i >= 16)
		return FALSE;
	return TRUE;
}

BOOL __fastcall MonsterIsShieldEquipped(UNIT *hMonster)
{
	UNIT * hMonsterCopy = hMonster;
	BYTE dwIndex;
	DWORD dwCode;
	DWORD i;
	struct MONITEMSTXT *ptItem;

	if (hMonster->dwClassId == 243)
		return TRUE;
	if (hMonster->dwClassId == 310)
		return TRUE;
	if (hMonster->dwClassId == 359)
		return FALSE;
	dwIndex = hMonster->abComponentVariations[7];
	dwCode = D2CommonMonsterRandGetComponentCode(hMonster, 7, dwIndex);
	if (dwCode)
	{
		for (i = 0; i < sgnNoShieldCodes; i++) {
			if (dwCode == sgadwNoShieldCodes[i])
				return FALSE; }
		ptItem = (struct MONITEMSTXT *)D2CommonItemTblsFindItemByCode(dwCode, &dwIndex);
		if (ptItem && 2 == ptItem->type)
			return TRUE;
		return FALSE; }
	return FALSE;
}

void __fastcall MonsterChangeClass(GAME *ptGame, UNIT *hUnit, DWORD dwClassId, DWORD dwMode)
{
	GAMEROOM *hRoom;
	DWORD dwUnitId;

#line 549
	D2ASSERT(hUnit);
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	dwUnitId = hUnit->dwId;
	MonsterUnitFree(ptGame, hUnit);
	hUnit->dwClassId = dwClassId;
	MonsterInitUnit(ptGame, hRoom, hUnit, dwUnitId);
	D2CommonUnitSetMode(hUnit, dwMode);
}
