#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\SKILLS\\SkillBar.cpp"
#include "D2Macros.h"
#include "../AI/AiGeneral.h"
#include "../AI/AiThink.h"
#include "../GAME/Event.h"
#include "../GUILDS/Guilds.h"
#include "../MISSILES/Missiles.h"
#include "../MISSILES/MissMode.h"
#include "../MONSTER/MonsterEval.h"
#include "../MONSTER/MonsterMode.h"
#include "../PLAYER/PlayerPets.h"
#include "../PLAYER/PlrModes.h"
#include "../PLAYER/PlrMsg.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "SkillAma.h"
#include "SkillBar.h"
#include "Skills.h"

#define SKILL_BARBARIAN_START	126
#define NUM_BARBARIAN_SKILLS	30
#define STATE_TAUNT				27
#define STATE_SHOUT				26
#define STATE_CORPSEUSED		104
#define STATE_LEAPING			18
#define STATE_FRENZY			94
#define STATE_BATTLEORDERS		32
#define STATE_BATTLECOMMAND		51
#define STATE_BERSERK			95
#define STATE_CONCENTRATE		15

static DWORD sgadwPotionCodes[4][3] =
{
	{ 540176488, 540176493, 544437874 },
	{ 540242024, 540242029, 544437874 },
	{ 540307560, 540307565, 543979122 },
	{ 540307560, 540307565, 543979122 },
};
static int sgnPotionCodes = sizeof(sgadwPotionCodes) / sizeof(DWORD);

static SKILLCHART sgptSkillChart[NUM_BARBARIAN_SKILLS] =
{
	{ SkillsBashStart, SkillsUseHit, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ NULL, SkillsHowl, SKILLCHART_NOMISSILE },
	{ SkillsFindPotionStart, SkillsFindPotion, SKILLCHART_NOMISSILE },
	{ SkillsLeapStart, SkillsLeap, SKILLCHART_NOMISSILE },
	{ SkillsDoubleSwingStart, SkillsDoubleSwing, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ NULL, SkillsTaunt, SKILLCHART_NOMISSILE },
	{ NULL, SkillsShout, SKILLCHART_NOMISSILE },
	{ SkillsStunStart, SkillsUseHit, SKILLCHART_NOMISSILE },
	{ NULL, SkillsDoubleThrow, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ SkillsFindItemStart, SkillsFindItem, SKILLCHART_NOMISSILE },
	{ SkillsLeapAttackStart, SkillsLeapAttack, SKILLCHART_NOMISSILE },
	{ SkillsConcentrateStart, SkillsConcentrate, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ NULL, SkillsBattleCry, SKILLCHART_NOMISSILE },
	{ SkillsFrenzyStart, SkillsFrenzy, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ NULL, SkillsBattleOrders, SKILLCHART_NOMISSILE },
	{ SkillsGrimWardStart, SkillsGrimWard, SKILLCHART_NOMISSILE },
	{ SkillsWhirlwindStart, SkillsWhirlwind, SKILLCHART_NOMISSILE },
	{ SkillsBerserkStart, SkillsUseHit, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ NULL, SkillsWarCry, SKILLCHART_NOMISSILE },
	{ NULL, SkillsBattleCommand, SKILLCHART_NOMISSILE },
};

BOOL __fastcall SkillsBashStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	DAMAGE *ptHeld;
	UNIT *hTarget;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget,
		D2CommonSkillsCalcToHitByLevel(SKILL_BARBARIAN_START, nLevel));
	if (tDamage.wFlags & 1)
	{
		tDamage.wFlags |= 8;
		tDamage.dwDamagePercent = D2CommonSkillsCalcParamOneByLevel(SKILL_BARBARIAN_START, nLevel);
		tDamage.dwHitClass = 112;
	}
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	ptHeld = SUnitDmgFindHeld(hUnit, hTarget);
	if (ptHeld)
		ptHeld->nPhysicalDamage += D2CommonSkillsCalcParamThreeByLevel(SKILL_BARBARIAN_START, nLevel) << 8;
	return TRUE;
}

BOOL __fastcall SkillsDoubleSwingStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget,
		D2CommonSkillsCalcToHitByLevel(SKILL_DOUBLE_SWING, nLevel));
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	return TRUE;
}

BOOL __fastcall SkillsDoubleSwing(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	UNIT *hNext;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if ((hUnit->nAnimPos & ~255) == 2304)
	{
		SUnitDmgDeliver(ptGame, hUnit, hTarget);
		hNext = SkillsBarNextTarget(ptGame, hUnit);
		if (!hNext)
			return FALSE;
		D2CommonUnitSetTarget(hUnit, hNext);
		for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
			((DWORD *)&tDamage)[i] = 0;
		tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hNext, D2CommonSkillsCalcToHitByLevel(SKILL_DOUBLE_SWING,
			nLevel));
		SUnitDmgHold(ptGame, hUnit, hNext, &tDamage);
		return TRUE;
	}
	hUnit->dwFlags0 |= 64;
	SUnitDmgDeliver(ptGame, hUnit, hTarget);
	return TRUE;
}

UNIT * __fastcall SkillsBarNextTarget(GAME *ptGame, UNIT *hUnit)
{
	UNITFINDARGS tArgs;
	UNITFINDDATA tFind;
	UNIT *hKept;
	UNIT *hLowest;
	UNIT *hHighest;
	UNIT *hFound;
	DWORD hRoom;
	DWORD dwLowest;
	DWORD dwHighest;
	DWORD dwId;
	int nX;
	int nY;
	int i;

	hKept = SUnitGetTarget(ptGame, hUnit);
	hLowest = NULL;
	hHighest = NULL;
	hRoom = D2CommonUnitGetPathRoom(hUnit);
	dwLowest = 0xFFFFFFFF;
	dwHighest = 0;
	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	for (i = 0; i < (int)(sizeof(UNITFINDARGS) / sizeof(DWORD)); i++)
		((DWORD *)&tArgs)[i] = 0;
	tArgs.dwFlags = 1411;
	tArgs.hExcluded = hUnit;
	tArgs.nX = nX;
	tArgs.nY = nY;
	tArgs.nDistance = 3;
	tArgs.pFieldPair = (void *)D2CommonFieldTblsAllocPair();
	D2CommonFieldTblsSetPair(tArgs.pFieldPair, nX, nY);
	D2CommonUnitFindsInit(ptGame->pMemPool, &tFind, hRoom, nX, nY, 3, 0, &tArgs);
	D2CommonUnitFindsFindUnits(&tFind);
	for (i = 0; i < tFind.nNumUnits; i++)
	{
		if (!SUnitIsHostile(ptGame, hUnit, tFind.phUnitsArray[i]))
			continue;
		dwId = tFind.phUnitsArray[i]->dwId;
		if (dwId < dwLowest)
		{
			dwLowest = dwId;
			hLowest = tFind.phUnitsArray[i];
		}
		if (dwId > dwHighest)
		{
			dwHighest = dwId;
			hHighest = tFind.phUnitsArray[i];
		}
		hKept = tFind.phUnitsArray[0];
	}
	hFound = hLowest;
	if (hKept == hFound)
		hFound = hHighest;
	if (!hFound)
		hFound = SUnitGetTarget(ptGame, hUnit);
	D2CommonUnitFindsFreeArray(&tFind);
	D2CommonFieldTblsFreePair(tArgs.pFieldPair);
	return hFound;
}

BOOL __fastcall SkillsHowl(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nValue;

	nValue = (int)D2CommonSkillsGetTypeParamOne(SKILL_HOWL) * (nLevel - 1);
	nValue += D2CommonMissileTblsGetVel(MISSILE_HOWL);
	SkillsNova(ptGame, hUnit, hUnit, MISSILE_HOWL, nLevel, nValue);
	return TRUE;
}

BOOL __fastcall SkillsFindPotionStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	return D2CommonSkillsUnitTestGuardD(SUnitGetTarget(ptGame, hUnit));
}

static BYTE SkillsBarGetAct(UNIT *hUnit)
{
	DWORD hRoom;

	hRoom = D2CommonUnitGetPathRoom(hUnit);
#line 220
	D2ASSERT(hRoom);
	return D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom));
}

BOOL __fastcall SkillsFindPotion(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MONSTATSTXT *ptStats;
	UNIT *hCorpse;
	DWORD dwCode;
	BYTE bAct;
	int nChance;
	int nRoll;
	int nKind;
	int nHealing;

	hCorpse = SUnitGetTarget(ptGame, hUnit);
	if (!(!D2CommonSkillsUnitTestGuardD(hCorpse)))
	{
		nChance = D2CommonSkillsCalcCurveParamOne(nLevel, SKILL_FIND_POTION);
		if ((int)D2CommonSeedRand(&hUnit->tSeed, 100) < nChance)
		{
			ptStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(hCorpse->dwClassId);
#line 254
			D2ASSERT(ptStats->bSoft);
			bAct = SkillsBarGetAct(hUnit);
			nKind = 0;
			nRoll = (int)D2CommonSeedRand(&hUnit->tSeed, 100);
			nHealing = (int)D2CommonSkillsGetTypeParamThree(SKILL_FIND_POTION);
			if (nRoll < nHealing)
				nKind = 1;
			else if (nRoll < (int)D2CommonSkillsGetTypeParamFour(SKILL_FIND_POTION) + nHealing)
				nKind = 2;
			if ((DWORD)bAct >= (DWORD)sgnPotionCodes)
				dwCode = sgadwPotionCodes[3][nKind];
			else
				dwCode = sgadwPotionCodes[bAct][nKind];
#line 257
			D2ASSERT(dwCode);
			SkillsDropItem(ptGame, hCorpse, dwCode, 2); }
		D2CommonStatsUnitSetActiveStateBit(hCorpse, STATE_CORPSEUSED, 1);
		return TRUE; }
	return FALSE;
}

BOOL __fastcall SkillsTaunt(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;
	DWORD hList;
	DWORD dwOwnerId;
	DWORD dwOwnerType;
	BOOL bFind;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	bFind = TRUE;
	if (hTarget && hTarget->eType == UNIT_MONSTER && !D2CommonStatsGetState(hTarget, STATE_TAUNT)
		&& SkillsCanBeAffected(hTarget, 12) && !SUnitIsDead(hTarget))
	{
		AiGetOwnerId(hTarget, &dwOwnerId, &dwOwnerType);
		if ((dwOwnerId == 0xFFFFFFFF || dwOwnerType) && SUnitIsHostile(ptGame, hUnit, hTarget))
			bFind = FALSE;
	}
	if (bFind)
		hTarget = SkillsFindNearest(ptGame, hUnit, 20, SkillsTauntTest);
	if (!hTarget)
		return FALSE;
	hList = D2CommonStatsFindListByMask(hTarget, 32);
	if (hList)
	{
		D2CommonStatsUnitUnlinkList(hTarget, hList);
		D2CommonStatsListFree(hList);
	}
	D2CommonStatsUnitSetActiveStateBit(hTarget, STATE_TAUNT, 1);
	hList = D2CommonStatsAllocList(ptGame->pMemPool, 32, 0xFFFFFFFF, hUnit->eType, hUnit->dwId);
	if (hList)
	{
		D2CommonStatsSetListState(hList, STATE_TAUNT);
		D2CommonStatsUnitLinkList(hTarget, hList);
		D2CommonStatsListSetValueA(hList, 19, D2CommonSkillsCalcParamOneByLevel(SKILL_TAUNT, nLevel));
		D2CommonStatsListSetValueA(hList, 25, D2CommonSkillsCalcParamThreeByLevel(SKILL_TAUNT, nLevel));
	}
	AiThinkSet(ptGame, hTarget, ((MONSTERDATA *)hTarget->uTypeData)->ptAi, 12);
	AiSetParams(((MONSTERDATA *)hTarget->uTypeData)->ptAi, -666, -666, -666);
	AiSetUnitRef(hTarget, NULL);
	EventRemoveByType(ptGame, hTarget, 2);
	EventAdd(ptGame, hTarget, 2, ptGame->nFrame + 1);
	D2CommonUnitSetTarget(hTarget, hUnit);
	return TRUE;
}

BOOL __fastcall SkillsTauntTest(GAME *ptGame, UNIT *hUnit)
{
	DWORD dwOwnerId;
	DWORD dwOwnerType;

	if (hUnit->eType != UNIT_MONSTER)
		return FALSE;
	if (D2CommonStatsGetState(hUnit, STATE_TAUNT))
		return FALSE;
	if (!SkillsCanBeAffected(hUnit, 12))
		return FALSE;
	if (SUnitIsDead(hUnit))
		return FALSE;
	AiGetOwnerId(hUnit, &dwOwnerId, &dwOwnerType);
	if (dwOwnerId != 0xFFFFFFFF && !dwOwnerType)
		return FALSE;
	return TRUE;
}

void __fastcall SkillsShoutApply(GAME *ptGame, UNIT *hTarget, UNIT *hOwner,
	int nLevel)
{
	DWORD hList;
	int nEndFrame;

	nEndFrame = D2CommonSkillsCalcParamThreeByLevel(SKILL_SHOUT, nLevel) + ptGame->nFrame;
	hList = D2CommonStatsFindListByState(hTarget, STATE_SHOUT);
	if (!hList)
	{
		hList = D2CommonStatsAllocList(ptGame->pMemPool, 2, nEndFrame, hTarget->eType,
			hTarget->dwId);
		if (!hList)
			return;
		D2CommonStatsSetListState(hList, STATE_SHOUT);
		D2CommonStatsSetListRemoveCallback(hList, PlrMsgStateEnd);
		D2CommonStatsUnitLinkList(hTarget, hList);
		D2CommonStatsUnitSetActiveStateBit(hTarget, STATE_SHOUT, 1);
	}
	D2CommonStatsUnitSetActiveStateValue(hTarget, STATE_SHOUT, 1);
	D2CommonStatsListSetValueB(hList, 171, D2CommonSkillsCalcParamOneByLevel(SKILL_SHOUT, nLevel));
	D2CommonStatsSetListEndFrame(hList, nEndFrame);
	EventAdd(ptGame, hTarget, 12, nEndFrame);
}

BOOL __fastcall SkillsShout(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILESTXT *ptMissile;
	int nValue;

	ptMissile = (MISSILESTXT *)D2CommonMissileTblsGetMissile(MISSILE_SHOUT);
	nValue = D2CommonMissileTblsGetVel(MISSILE_SHOUT) + ptMissile->Param1 * (nLevel - 1);
	SkillsNova(ptGame, hUnit, hUnit, MISSILE_SHOUT, nLevel, nValue);
	SkillsShoutApply(ptGame, hUnit, hUnit, nLevel);
	return TRUE;
}

BOOL __fastcall SkillsStunStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget, D2CommonSkillsCalcToHitByLevel(SKILL_STUN, nLevel));
	if (tDamage.wFlags & 1)
	{
		tDamage.nStunLength = D2CommonSkillsCalcParamOneByLevel(SKILL_STUN, nLevel);
		tDamage.dwHitClass = 96;
		SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	}
	return TRUE;
}

BOOL __fastcall SkillsDoubleThrow(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hItem;
	UNIT *hMissile;
	int nMissile;

	hItem = (UNIT *)D2CommonUnitGetActiveWeapon(hUnit);
	if (!hItem)
		return FALSE;
	nMissile = (int)D2CommonITEMSGetTypeMissileType(hItem);
	if (nMissile == 65535)
		return FALSE;
	if (D2CommonITEMSGetType(hItem) == 38)
		hMissile = SkillsThrowMissile(ptGame, nMissile, hUnit, nLevel, 1);
	else
		hMissile = SkillsCreateMissile(ptGame, nMissile, hUnit, nLevel, 0, 0, 0, 0, 1);
	if (hMissile)
	{
		SkillsThrowDamage(hUnit, hMissile, hItem);
		D2CommonStatsUnitAddValue(hMissile, 19, D2CommonSkillsCalcToHitByLevel(SKILL_DOUBLE_THROW, nLevel));
	}
	return TRUE;
}

BOOL __fastcall SkillsFindItemStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget && D2CommonSkillsUnitTestGuardA(hTarget))
		return TRUE;
	return FALSE;
}

BOOL __fastcall SkillsFindItem(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLSTXT *ptSkill;
	UNIT *hCorpse;
	int nQuality;
	int nChance;
	int nRoll;
	int nLow;
	int nHigh;

	ptSkill = (SKILLSTXT *)D2CommonDataTblsGetSkill(SKILL_FIND_ITEM);
	nQuality = 1;
	hCorpse = SUnitGetTarget(ptGame, hUnit);
	if (!hCorpse || !D2CommonSkillsUnitTestGuardA(hCorpse))
		return FALSE;
	nChance = D2CommonSkillsCalcCurveParamOne(nLevel, SKILL_FIND_ITEM);
	if ((int)D2CommonSeedRand(&hUnit->tSeed, 100) < nChance)
	{
		nRoll = (int)D2CommonSeedRand(&hUnit->tSeed, 100);
		nLow = 0;
		nHigh = ptSkill->Param[0];
		if (nRoll >= nLow && nRoll < nHigh)
			nQuality = 1;
		nLow += ptSkill->Param[0];
		nHigh += ptSkill->Param[1];
		if (nRoll >= nLow && nRoll < nHigh)
			nQuality = 2;
		nLow += ptSkill->Param[1];
		nHigh += ptSkill->Param[2];
		if (nRoll >= nLow && nRoll < nHigh)
			nQuality = 3;
		if (nRoll >= ptSkill->Param[2] + nLow && nRoll < ptSkill->Param[3] + nHigh)
			nQuality = 4;
		MonsterModeDropTreasureItem(ptGame, hCorpse, nQuality);
	}
	D2CommonStatsUnitSetActiveStateBit(hCorpse, STATE_CORPSEUSED, 1);
	return TRUE;
}

BOOL __fastcall SkillsLeapStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	GAMECOORD tCoord;
	UNIT *hTarget;
	DWORD hSkill;
	DWORD hRoom;
	BOOL bMonster;
	int nX;
	int nY;
	int nUnitX;
	int nUnitY;
	int nRange;
	int nDist;
	int nSkillId;
	int nOffY;
	int nTries;
	int nTx;
	int nTy;
	BOOL bFound;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	bMonster = hUnit->eType == UNIT_MONSTER;
#line 721
	D2ASSERT(hSkill);
	if (bMonster)
	{
		hTarget = SUnitGetTarget(ptGame, hUnit);
		if (!hTarget)
		{
			if (!SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY))
			{
				D2CommonUnitSetSkillThirdCached(hUnit, 0);
				return FALSE;
			}
			D2CommonSkillsSetNodeScratchF(hSkill, 0xFFFFFFFF);
		}
		else
		{
			D2CommonSkillsSetNodeFlags(hSkill, 8);
			MonsterEvalSetDamage(hUnit, 4);
			MonsterEvalAttack(hUnit, ptGame);
			SUnitDmgDeliver(ptGame, hUnit, hTarget);
			D2CommonSkillsSetNodeScratchE(hSkill, hTarget->eType);
			D2CommonSkillsSetNodeScratchF(hSkill, hTarget->dwId);
			if (SUnitIsDead(hUnit))
				return FALSE;
			nX = (int)D2CommonUnitGetX(hTarget);
			nY = (int)D2CommonUnitGetY(hTarget);
			nX = 2 * nX - (int)D2CommonUnitGetX(hUnit);
			nY = 2 * nY - (int)D2CommonUnitGetY(hUnit);
		}
		tCoord.dwX = nX;
		tCoord.dwY = nY;
		if (!D2CommonCollisnFindSpawnPointNear(D2CommonUnitGetPathRoom(hUnit), &tCoord, D2CommonUnitGetSizeX(hUnit),
			7169, 0))
		{
			D2CommonUnitSetSkillThirdCached(hUnit, 0);
			return FALSE;
		}
		nX = tCoord.dwX;
		nY = tCoord.dwY;
	}
	else
	{
		if (!SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY))
			return FALSE;
		nSkillId = (int)D2CommonSkillsGetNodeType(hSkill);
		nRange = D2CommonSkillsCalcCurveParamOne(nLevel, nSkillId);
		nDist = D2CommonUnitUtilGetDistance(hUnit, nX, nY);
		if (!nDist)
			nDist = 1;
		nUnitX = (int)D2CommonUnitGetX(hUnit);
		nUnitY = (int)D2CommonUnitGetY(hUnit);
		if (nDist > nRange)
		{
			nX = (nX - nUnitX) * nRange / nDist + nUnitX;
			nY = (nY - nUnitY) * nRange / nDist + nUnitY;
		}
		nTries = 3;
		bFound = TRUE;
		nLevel = nY - nUnitY;
		if (nLevel > 2)
			nLevel = 2;
		else if (nLevel < -2)
			nLevel = -2;
		nOffY = nUnitX - nX;
		if (nOffY > 2)
			nOffY = 2;
		else if (nOffY < -2)
			nOffY = -2;
		tCoord.dwX = nX;
		tCoord.dwY = nY;
		for (;;)
		{
			hRoom = (DWORD)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hUnit), tCoord.dwX,
				tCoord.dwY);
			hRoom = (DWORD)D2CommonCollisnFindSpawnPointNear(hRoom, &tCoord, D2CommonUnitGetSizeX(hUnit), 7177, 1);
			nTx = tCoord.dwX;
			nTy = tCoord.dwY;
			if (hRoom
				&& !D2CommonCollisnTestPatternAnyRaw(hRoom, nTx, nTy,
					D2CommonPathGetCollisionPatternByUnit(hUnit), 7177)
				&& D2CommonSkillsAddFieldA(nUnitX, nUnitY, nTx, nTy, hRoom, 2052)
				&& D2CommonUnitUtilGetDistance(hUnit, nTx, nTy) <= nRange)
				break;
			nTries--;
			if (nTries == 2)
			{
				tCoord.dwX = nX + nLevel;
				tCoord.dwY = nY + nOffY;
			}
			else if (nTries == 1)
			{
				tCoord.dwX = nX - nLevel;
				tCoord.dwY = nY - nOffY;
			}
			else if (!nTries)
			{
				bFound = FALSE;
				break;
			}
		}
		if (!bFound)
			return FALSE;
		nX = nTx;
		nY = nTy;
	}

	hRoom = (DWORD)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hUnit), nX, nY);
	D2CommonCollisnSetPattern(hRoom, nX, nY, D2CommonPathGetCollisionPatternByUnit(hUnit), 128);
	SUnitSetStateEndLinked(hUnit, TRUE);
	D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_LEAPING, 1);
	D2CommonSkillsSetNodeScratchC(hSkill, nX);
	D2CommonSkillsSetNodeScratchD(hSkill, nY);
	if (bMonster)
	{
		hTarget = SUnitGetTarget(ptGame, hUnit);
		if (hTarget)
		{
			D2CommonSkillsSetNodeScratchE(hSkill, hTarget->eType);
			D2CommonSkillsSetNodeScratchF(hSkill, hTarget->dwId);
		}
	}
	D2CommonSkillsSetNodeFlags(hSkill, 128);
	return TRUE;
}

BOOL __fastcall SkillsLeapBegin(GAME *ptGame, UNIT *hUnit, DWORD hSkill)
{
	void *ptPath;
	DWORD dwSpeed;
	BOOL bMonster;
	int nX;
	int nY;

	ptPath = hUnit->ptPath;
	nX = (int)D2CommonSkillsGetNodeScratchC(hSkill);
	nY = (int)D2CommonSkillsGetNodeScratchD(hSkill);
	bMonster = hUnit->eType == UNIT_MONSTER;
	D2CommonSkillsSetNodeFlags(hSkill, 4353);
	D2CommonPathSetCollisionTestMask(ptPath, 0);
	D2CommonPathSetCollisionMask(ptPath, 0);
	D2CommonPathSetTargetCoord(ptPath, nX, nY);
	D2CommonPathSetType(ptPath, 9);
	if (bMonster)
		dwSpeed = ((MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId))->Run << 8;
	else
		dwSpeed = (DWORD)((CHARSTATSTXT *)D2CommonDataTblsGetCharStats(hUnit->dwClassId))->RunVelocity << 8;
	D2CommonPathSetVelocity(ptPath, dwSpeed);
	D2CommonPathCompute(ptPath, hUnit);
	return TRUE;
}

BOOL __fastcall SkillsLeapStep(GAME *ptGame, UNIT *hUnit, DWORD hSkill)
{
	UNIT *hTarget;
	void *ptPath;
	DWORD hRoom;
	DWORD hNode;
	BOOL bMonster;
	int nClass;
	int nX;
	int nY;

	ptPath = hUnit->ptPath;
	hRoom = D2CommonUnitGetPathRoom(hUnit);
	hNode = hSkill;
	nX = (int)D2CommonSkillsGetNodeScratchC(hNode);
	nY = (int)D2CommonSkillsGetNodeScratchD(hNode);
	bMonster = hUnit->eType == UNIT_MONSTER;
	if (bMonster)
		nClass = (int)D2CommonMonsterUnitGetTypeLocalIndex(hUnit);
	else
		nClass = 410;
	if ((int)D2CommonUnitGetX(hUnit) == nX && (int)D2CommonUnitGetY(hUnit) == nY)
	{
		if (bMonster)
		{
			D2CommonCollisnClearPattern(hRoom, nX, nY, D2CommonPathGetCollisionPatternByUnit(hUnit), 256);
			D2CommonPathSetCollisionMask(ptPath, 256);
			D2CommonPathSetCollisionTestMask(ptPath, 7169);
			D2CommonPathSetType(ptPath, 2);
		}
		else
		{
			D2CommonCollisnClearPattern(hRoom, nX, nY, D2CommonPathGetCollisionPatternByUnit(hUnit), 128);
			D2CommonPathSetCollisionMask(ptPath, 128);
			D2CommonPathSetCollisionTestMask(ptPath, 7177);
			D2CommonPathSetType(ptPath, 7);
		}
		if (bMonster)
		{
			if (nClass == 78)
			{
				ptPath = hUnit->ptPath;
				hTarget = SUnitFind(ptGame, D2CommonSkillsGetNodeScratchE(hSkill), D2CommonSkillsGetNodeScratchF(hSkill));
				if (hTarget)
				{
					D2CommonSkillsSetNodeFlags(hSkill, 1);
					D2CommonUnitSetAnimSequencePos(hUnit, 12);
					hUnit->nAnimLength = (hUnit->nAnimLength & ~255) + 256;
					D2CommonStepSnapToSquare(ptPath);
					D2CommonPathSetDistMax(ptPath, 5);
					D2CommonPathSetStopDistance(ptPath, 1);
					D2CommonPathSetTarget(ptPath, 0);
					nX = 3 * nX - 2 * (int)D2CommonUnitGetX(hTarget);
					nY = 3 * nY - 2 * (int)D2CommonUnitGetY(hTarget);
					hRoom = (DWORD)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hUnit), nX, nY);
					if (!hRoom || !D2CommonDungeonRoomIsTown(hRoom))
					{
						D2CommonPathSetTargetCoord(ptPath, nX, nY);
						D2CommonPathSetType(ptPath, 8);
						D2CommonPathSetSquareCountdown(ptPath, 5);
						D2CommonPathCompute(ptPath, hUnit);
					}
				}
			}
		}
		else
		{
			D2CommonSkillsSetNodeFlags(hSkill, 512);
			hUnit->nAnimLength = (hUnit->nAnimLength & ~255) - 256;
		}
		SUnitSetStateEndLinked(hUnit, FALSE);
		D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_LEAPING, 0);
		return TRUE;
	}
	if (bMonster)
	{
		int nSeq;

		nSeq = 0;
		if (nClass == 78)
			nSeq = 8;
		D2CommonUnitSetAnimSequencePos(hUnit, nSeq);
		hUnit->nAnimLength = (hUnit->nAnimLength & ~255) + 256;
		return FALSE;
	}
	D2CommonUnitSetAnimSequencePos(hUnit, 10);
	hUnit->nAnimLength = (hUnit->nAnimLength & ~255) + 256;
	return FALSE;
}

BOOL __fastcall SkillsLeap(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreate;
	DAMAGE tDamage;
	UNIT *hMissile;
	DWORD hSkill;
	DWORD dwFlags;
	BOOL bMonster;
	int nRadius;
	int nX;
	int nY;
	int i;

	hSkill = D2CommonSkillsFindByKey(hUnit, SKILL_LEAP);
	dwFlags = D2CommonSkillsGetNodeFlags(hSkill);
	bMonster = hUnit->eType == UNIT_MONSTER;
#line 955
	D2ASSERT(hSkill);
	EventRemoveByType(ptGame, hUnit, 1);
	if (dwFlags & 256)
	{
		if (!SkillsLeapStep(ptGame, hUnit, hSkill) || bMonster)
			return TRUE;
		for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.hSource = hUnit;
		tCreate.hSpawnLocation = hUnit;
		tCreate.nLevel = 1;
		tCreate.eMissile = MISSILE_LEAPKNOCKBACK;
		hMissile = MissilesCreate(ptGame, &tCreate);
		if (!hMissile)
			return TRUE;
		for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
			((DWORD *)&tDamage)[i] = 0;
		tDamage.wFlags = 9;

		SUnitDmgSplash(ptGame, hMissile, ((int)D2CommonUnitGetX(hUnit)), ((int)D2CommonUnitGetY(hUnit)), D2CommonSkillsCalcParamThreeByLevel(SKILL_LEAP, nLevel), &tDamage, FALSE, FALSE, NULL, 0);
		MissModeCollide(ptGame, hMissile, NULL, FALSE);
		return TRUE;
	}
	if (dwFlags & 128)
		return SkillsLeapBegin(ptGame, hUnit, hSkill);
	if (!bMonster)
		D2CommonSkillsSetNodeFlags(hSkill, 0);
	return TRUE;
}

BOOL __fastcall SkillsLeapAttackStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;
	DWORD hSkill;
	DWORD dwPattern;
	int nX;
	int nY;

	hSkill = D2CommonSkillsFindByKey(hUnit, SKILL_LEAP_ATTACK);
	dwPattern = D2CommonPathGetCollisionPatternByUnit(hUnit);
#line 1039
	D2ASSERT(hSkill);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!D2CommonSkillsGetFieldPastTarget(hUnit, hTarget, &nX, &nY))
	{
		if (!hTarget || !D2CommonUnitTestPositionClear(hUnit, hTarget, 0))
			return FALSE;
		SUnitSetMode(ptGame, hUnit, 1);
		D2CommonUnitSetSkillThirdCached(hUnit, 0);
		DWORD hBasic = D2CommonSkillsFindByKey(hUnit, 0);
		PlrModeStartID(ptGame, hUnit, (void *)hBasic, 7, hTarget->eType,
			hTarget->dwId, TRUE);
		return TRUE;
	}
	DWORD hRoom = D2CommonUnitGetPathRoom(hUnit);
	D2CommonCollisnSetPattern(hRoom, nX, nY, dwPattern, 128);
	SUnitSetStateEndLinked(hUnit, TRUE);
	D2CommonSkillsSetNodeScratchC(hSkill, nX);
	D2CommonSkillsSetNodeScratchD(hSkill, nY);
	if (hTarget)
	{
		D2CommonSkillsSetNodeScratchE(hSkill, hTarget->eType);
		D2CommonSkillsSetNodeScratchF(hSkill, hTarget->dwId);
	}
	else
		D2CommonSkillsSetNodeScratchE(hSkill, 6);
	D2CommonSkillsSetNodeFlags(hSkill, 4224);
	D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_LEAPING, 1);
	if (hTarget && D2CommonStatsGetState(hTarget, 54))
		SkillsLeapHit(ptGame, hUnit, hTarget);
	return TRUE;
}

extern BOOL SkillsLeapAttackHit(GAME *ptGame, UNIT *hUnit, int nLevel);
__inline BOOL SkillsLeapAttackHit(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	DWORD hSkill;
	DWORD hList;
	int i;

	hSkill = D2CommonSkillsFindByKey(hUnit, SKILL_LEAP_ATTACK);
#line 1103
	D2ASSERT(hSkill);
	hTarget = SkillsLeapAttackTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = 9;
	tDamage.dwDamagePercent = D2CommonSkillsCalcParamThreeByLevel(SKILL_LEAP_ATTACK, nLevel);
	tDamage.dwHitClass = D2CommonUnitGetStateByte(hUnit);
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	SUnitDmgApply(ptGame, hUnit, hTarget, TRUE, &tDamage);
	SUnitDmgReact(ptGame, hUnit, hTarget, &tDamage);
	D2CommonUnitAddAnimOverlay(hTarget, 147, 0);
	hList = D2CommonStatsFindListByState(hTarget, 21);
	if (hList)
	{
		D2CommonStatsUnitUnlinkList(hTarget, hList);
		D2CommonStatsListFree(hList);
	}
	return TRUE;
}

BOOL __fastcall SkillsLeapAttack(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DWORD hSkill;
	DWORD dwFlags;

	hSkill = D2CommonSkillsFindByKey(hUnit, SKILL_LEAP_ATTACK);
	dwFlags = D2CommonSkillsGetNodeFlags(hSkill);
#line 1146
	D2ASSERT(hSkill);
	if (dwFlags & 256)
	{
		if (SkillsLeapStep(ptGame, hUnit, hSkill) && !SkillsLeapAttackTarget(ptGame, hUnit))
			hUnit->nAnimLength = 1024;
		return TRUE;
	}
	if (dwFlags & 128)
		return SkillsLeapBegin(ptGame, hUnit, hSkill);
	if (dwFlags & 512)
		return SkillsLeapAttackHit(ptGame, hUnit, nLevel);
	D2CommonSkillsSetNodeFlags(hSkill, 4096);
	return TRUE;
}

UNIT * __fastcall SkillsLeapAttackTarget(GAME *ptGame, UNIT *hUnit)
{
	UNIT *hTarget;
	DWORD hSkill;
	DWORD eType;
	DWORD dwId;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget && !D2CommonUnitTestPositionClear(hUnit, hTarget, 0))
		hTarget = NULL;
	hSkill = D2CommonSkillsFindByKey(hUnit, SKILL_LEAP_ATTACK);
	if (!hTarget)
	{
		eType = D2CommonSkillsGetNodeScratchE(hSkill);
		dwId = D2CommonSkillsGetNodeScratchF(hSkill);
		if (eType != 6)
			hTarget = SUnitFind(ptGame, eType, dwId);
		if (!hTarget)
			hTarget = SkillsBarNextTarget(ptGame, hUnit);
	}
	if (hTarget)
	{
		D2CommonSkillsSetNodeScratchE(hSkill, hTarget->eType);
		D2CommonSkillsSetNodeScratchF(hSkill, hTarget->dwId);
	}
	else
		D2CommonSkillsSetNodeScratchE(hSkill, 6);
	return hTarget;
}

BOOL __fastcall SkillsBattleCry(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsNova(ptGame, hUnit, hUnit, MISSILE_BATTLECRY, nLevel, 0);
	return TRUE;
}

BOOL __fastcall SkillsFrenzyStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	DWORD hSkill;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	if (!hTarget)
		return FALSE;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget, D2CommonSkillsCalcToHitByLevel(SKILL_FRENZY,
		nLevel));
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	if (hSkill)
	{
		if (tDamage.wFlags & 1)
		{
			D2CommonSkillsSetNodeScratchC(hSkill, 1);
			return TRUE;
		}
		D2CommonSkillsSetNodeScratchC(hSkill, 0);
	}
	return TRUE;
}

BOOL __fastcall SkillsFrenzy(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	UNIT *hNext;
	DWORD hSkill;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if ((hUnit->nAnimPos & ~255) <= 2304)
	{
	if (hTarget)
		SUnitDmgDeliver(ptGame, hUnit, hTarget);
	SkillsFrenzyState(ptGame, hUnit, nLevel);
	hNext = SkillsBarNextTarget(ptGame, hUnit);
	if (!hNext)
		return FALSE;
	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hNext, D2CommonSkillsCalcToHitByLevel(SKILL_FRENZY, nLevel));
	SUnitDmgHold(ptGame, hUnit, hNext, &tDamage);
	if (hSkill)
	{
		if (tDamage.wFlags & 1)
		{
			D2CommonSkillsSetNodeScratchC(hSkill, 1);
			return TRUE;
		}
		D2CommonSkillsSetNodeScratchC(hSkill, 0);
	}
	return TRUE;
	}
	hUnit->dwFlags0 |= 64;
	if (hTarget)
		SUnitDmgDeliver(ptGame, hUnit, hTarget);
	SkillsFrenzyState(ptGame, hUnit, nLevel);
	return TRUE;
}

void __fastcall SkillsFrenzyState(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DWORD hSkill;
	DWORD hList;
	int nEndFrame;
	int nCount;
	int nValue;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	if (hSkill && !D2CommonSkillsGetNodeScratchC(hSkill))
		return;
	hList = D2CommonStatsFindListByState(hUnit, STATE_FRENZY);
	nEndFrame = D2CommonSkillsCalcParamOneByLevel(SKILL_FRENZY, nLevel) + ptGame->nFrame;
	if (!hList)
	{
		hList = D2CommonStatsAllocList(ptGame->pMemPool, 2, nEndFrame, hUnit->eType, hUnit->dwId);
		if (!hList)
			return;
		D2CommonStatsSetListState(hList, STATE_FRENZY);
		D2CommonStatsSetListRemoveCallback(hList, SkillsFrenzyStateEnd);
		D2CommonStatsListSetValueB(hList, 169, 0);
		D2CommonStatsUnitLinkList(hUnit, hList);
		D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_FRENZY, 1);
	}
	D2CommonStatsSetListEndFrame(hList, nEndFrame);
	EventAdd(ptGame, hUnit, 12, nEndFrame);
	D2CommonStatsUnitSetActiveStateValue(hUnit, STATE_FRENZY, 1);
	nCount = (int)D2CommonStatsListGetValueA(hList, 169) + 1;
	D2CommonStatsListSetValueB(hList, 169, nCount);
	nValue = D2CommonSkillsCalcCurveParamThree(nCount, SKILL_FRENZY);
	D2CommonStatsListSetValueB(hList, 67, nValue);
	D2CommonStatsListSetValueB(hList, 68, nValue);
	D2CommonStatsListSetValueB(hList, 69, nValue);
	D2CommonUnitUpdateAnimRate(hUnit);
}

void __fastcall SkillsFrenzyStateEnd(UNIT *hUnit, DWORD dwState)
{
	D2CommonStatsUnitSetActiveStateBit(hUnit, dwState, 0);
	D2CommonUnitUpdateAnimRate(hUnit);
}

void __fastcall SkillsBattleCommandApply(GAME *ptGame, UNIT *hTarget, UNIT *hOwner,
	int nLevel)
{
	DWORD hList;
	int nEndFrame;

	if (!hTarget || hTarget->eType != UNIT_PLAYER || !hOwner || !SUnitIsAlly(ptGame, hOwner, hTarget))
		return;
	nEndFrame = D2CommonSkillsCalcParamOneByLevel(SKILL_BATTLE_COMMAND, nLevel) + ptGame->nFrame;
	hList = D2CommonStatsFindListByState(hTarget, STATE_BATTLECOMMAND);
	if (!hList)
	{
		hList = D2CommonStatsAllocList(ptGame->pMemPool, 2, nEndFrame, hTarget->eType,
			hTarget->dwId);
		if (hList)
		{
			D2CommonStatsSetListState(hList, STATE_BATTLECOMMAND);
			D2CommonStatsSetListRemoveCallback(hList, SkillsBattleCommandEnd);
			D2CommonStatsUnitLinkList(hTarget, hList);
			D2CommonStatsUnitSetActiveStateBit(hTarget, STATE_BATTLECOMMAND, 1);
		}
	}
	D2CommonStatsSetListEndFrame(hList, nEndFrame);
	EventAdd(ptGame, hTarget, 12, nEndFrame);
	if (hTarget->eType == UNIT_PLAYER)
		PlayerPetsUpdateMax(hTarget);
	D2CommonSkillsRefreshPassives(hTarget);
}

void __fastcall SkillsBattleCommandEnd(UNIT *hUnit, DWORD dwState)
{
	D2CommonStatsUnitSetActiveStateBit(hUnit, dwState, 0);
	if (hUnit->eType == UNIT_PLAYER)
		PlayerPetsUpdateMax(hUnit);
	D2CommonSkillsRefreshPassives(hUnit);
}

void __fastcall SkillsBattleOrdersApply(GAME *ptGame, UNIT *hTarget, UNIT *hOwner,
	int nLevel)
{
	SKILLSTATE tState;
	void *hList;
	int nValue;

	if (!hTarget || !hOwner || !SUnitIsAlly(ptGame, hOwner, hTarget))
		return;
	nValue = D2CommonSkillsCalcCurveParamThree(nLevel, SKILL_BATTLE_ORDERS);
	memset(&tState, 0, sizeof(tState));
	tState.hSource = hOwner;
	tState.hTarget = hTarget;
	tState.nSkill = SKILL_BATTLE_ORDERS;
	tState.nLevel = nLevel;
	tState.nDuration = D2CommonSkillsCalcParamOneByLevel(SKILL_BATTLE_ORDERS, nLevel);
	tState.dwStatId = 77;
	tState.nStatValue = nValue;
	tState.dwState = STATE_BATTLEORDERS;
	tState.pfnRemove = (DWORD)SkillsBattleOrdersEnd;
	hList = SkillsSetState(&tState);
	if (hList)
	{
		D2CommonStatsListSetValueB(hList, 76, nValue);
		D2CommonStatsListSetValueB(hList, 162, nValue);
	}
}

void __fastcall SkillsBattleOrdersEnd(UNIT *hUnit, DWORD dwState)
{
	int nMana;
	int nMaxMana;
	int nLife;
	int nMaxLife;
	int nStamina;
	int nMaxStamina;

	D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_BATTLEORDERS, 0);
	nMana = D2CommonStatsUnitGetValue(hUnit, 8);
	nMaxMana = D2CommonStatsUnitCalcMaxMana(hUnit);
	nLife = D2CommonStatsUnitGetValue(hUnit, 6);
	nMaxLife = D2CommonStatsUnitCalcMaxHp(hUnit);
	nStamina = D2CommonStatsUnitGetValue(hUnit, 10);
	nMaxStamina = D2CommonStatsUnitCalcMaxStamina(hUnit);
	if (nMana > nMaxMana)
		D2CommonStatsUnitSetValue(hUnit, 8, nMaxMana);
	if (nLife > nMaxLife)
		D2CommonStatsUnitSetValue(hUnit, 6, nMaxLife);
	if (nStamina > nMaxStamina)
		D2CommonStatsUnitSetValue(hUnit, 10, nMaxStamina);
}

BOOL __fastcall SkillsBattleOrders(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsNova(ptGame, hUnit, hUnit, MISSILE_BATTLEORDERS, nLevel, 0);
	SkillsBattleOrdersApply(ptGame, hUnit, hUnit, nLevel);
	return TRUE;
}

BOOL __fastcall SkillsGrimWardStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget && D2CommonSkillsUnitTestGuardE(hTarget))
		return TRUE;
	return FALSE;
}

BOOL __fastcall SkillsGrimWard(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreate;
	GAMECOORD tCoord;
	UNIT *hCorpse;
	DWORD hRoom;
	int i;

	hCorpse = SUnitGetTarget(ptGame, hUnit);
	if (!hCorpse || !D2CommonSkillsUnitTestGuardE(hCorpse))
		return FALSE;
	tCoord.dwX = D2CommonUnitGetX(hCorpse);
	tCoord.dwY = D2CommonUnitGetY(hCorpse);
	hRoom = (DWORD)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hCorpse), tCoord.dwX, tCoord.dwY);
	(void)D2CommonCollisnFindSpawnPointNear(hRoom, &tCoord, 2, 4096, 1);
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = 1;
	tCreate.hSource = hUnit;
	tCreate.nLevel = nLevel;
	switch (((MONSTATSTXT *)D2CommonMonsterTblsGetStats(hCorpse->dwClassId))->OverlayHeight)
	{
	case 2:
		tCreate.eMissile = MISSILE_GRIMWARDMEDIUMSTART;
		break;
	case 3:
		tCreate.eMissile = MISSILE_GRIMWARDLARGESTART;
		break;
	default:
		tCreate.eMissile = MISSILE_GRIMWARDSMALLSTART;
		break;
	}
	tCreate.nX = tCoord.dwX;
	tCreate.nY = tCoord.dwY;
	MissilesCreate(ptGame, &tCreate);
	D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hCorpse), hCorpse->eType, hCorpse->dwId);
	SUnitFree(ptGame, hCorpse);
	return TRUE;
}

BOOL __fastcall SkillsWhirlwindStart(GAME *ptGame, UNIT *hPlayer  ,
	int nLevel)
{
	CHARSTATSTXT *ptStats;
	PATHPOINT *ptPoints;
	UNIT *hTarget;
	void *ptPath;
	DWORD hSkill;
	DWORD dwPattern;
	int nLength;
	int nX;
	int nY;

	hSkill = D2CommonSkillsFindByKey(hPlayer, SKILL_WHIRLWIND);
	ptPath = hPlayer->ptPath;
#line 1540
	D2ASSERT(UnitGetType (hPlayer) == UNIT_PLAYER);
#line 1541
	D2ASSERT(hSkill);
	if (!SkillsGetTargetSpot(ptGame, hPlayer, &nX, &nY))
		return FALSE;
	hTarget = SUnitGetTarget(ptGame, hPlayer);
	if (hTarget && D2CommonUnitTestPositionClear(hPlayer, hTarget, 0))
		return SkillsWhirlAt(ptGame, hPlayer, hTarget);
	D2CommonPathSetCollisionTestMask(ptPath, 3073);
	D2CommonPathSetType(ptPath, 7);
	if (!D2CommonPathCompute(ptPath, hPlayer))
	{
		D2CommonPathSetCollisionTestMask(ptPath, 7177);
		return FALSE;
	}
	ptStats = (CHARSTATSTXT *)D2CommonDataTblsGetCharStats(hPlayer->dwClassId);
#line 1565
	D2ASSERT(ptStats);
	D2CommonPathSetVelocity(ptPath, (DWORD)ptStats->WalkVelocity << 8);
	D2CommonPathSetCollisionTestMask(ptPath, 1025);
	nLength = (int)D2CommonPathGetSteps(ptPath, &ptPoints);
#line 1574
	D2ASSERT(nLength);
	nX = ptPoints[nLength - 1].wX;
	nY = ptPoints[nLength - 1].wY;

	D2CommonCollisnSetPattern(D2CommonUnitGetPathRoom(hPlayer), nX, nY, D2CommonPathGetCollisionPatternByUnit(hPlayer), 128);
	SUnitSetStateEndLinked(hPlayer, TRUE);
	D2CommonStatsUnitSetActiveStateBit(hPlayer, STATE_LEAPING, 1);
	D2CommonSkillsSetNodeFlags(hSkill, 1);
	D2CommonSkillsSetNodeScratchC(hSkill, nX);
	D2CommonSkillsSetNodeScratchD(hSkill, nY);
	D2CommonSkillsSetNodeScratchE(hSkill, 0xFFFFFFFF);
	return TRUE;
}

BOOL __fastcall SkillsWhirlwind(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	void *ptPath;
	DWORD hSkill;
	DWORD dwFlags;
	DWORD dwPattern;
	int nX;
	int nY;
	int nHits;
	int nHit;
	int i;

	hSkill = D2CommonSkillsFindByKey(hUnit, SKILL_WHIRLWIND);
	ptPath = hUnit->ptPath;
#line 1609
	D2ASSERT(hSkill);
	nX = (int)D2CommonSkillsGetNodeScratchC(hSkill);
	nY = (int)D2CommonSkillsGetNodeScratchD(hSkill);
	dwFlags = D2CommonSkillsGetNodeFlags(hSkill);
	if ((dwFlags & 3) == 3)
	{

		D2CommonCollisnClearPattern(D2CommonUnitGetPathRoom(hUnit), nX, nY, D2CommonPathGetCollisionPatternByUnit(hUnit), 128);
		D2CommonPathSetCollisionMask(ptPath, 128);
		D2CommonPathSetCollisionTestMask(ptPath, 7177);
		D2CommonSkillsSetNodeFlags(hSkill, 0);
		SUnitSetStateEndLinked(hUnit, FALSE);
		D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_LEAPING, 0);
	}
	else if (dwFlags & 1)
	{
		D2CommonUnitSetAnimSequencePos(hUnit, 3);
		hUnit->nAnimLength = 1280;
		nHits = (int)D2CommonSkillsGetTypeParamThree(SKILL_WHIRLWIND);
		for (nHit = 0; nHit < nHits; nHit++)
		{
			hTarget = SkillsFindNext(ptGame, hUnit, D2CommonSkillsGetNodeScratchE(hSkill), 5, 0);
			if (!hTarget)
				break;
			D2CommonSkillsSetNodeScratchE(hSkill, hTarget->dwId);
			for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
				((DWORD *)&tDamage)[i] = 0;
			tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget,
				D2CommonSkillsCalcToHitByLevel(SKILL_WHIRLWIND, nLevel));
			tDamage.dwDamagePercent = D2CommonSkillsCalcParamOneByLevel(SKILL_WHIRLWIND, nLevel);
			SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
			SUnitDmgDeliver(ptGame, hUnit, hTarget);
			if (SUnitIsDead(hUnit))
				break;
		}
	}
	return TRUE;
}

BOOL __fastcall SkillsBerserkStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	DWORD hList;
	int nToHit;
	int nDamage;
	int nMin;
	int nMax;
	int nEndFrame;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	nToHit = D2CommonSkillsGetDifficultyTableEntry(nLevel, SKILL_BERSERK, 1) << 1;
	nDamage = D2CommonSkillsGetDifficultyTableEntry(nLevel, SKILL_BERSERK, 0) << 1;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget, nToHit);
	tDamage.dwDamagePercent = nDamage;
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	if (tDamage.wFlags & 1)
	{
		tDamage.nMagicDamage += tDamage.nPhysicalDamage;
		tDamage.nPhysicalDamage = 0;
	}
	nMin = (int)D2CommonSkillsGetTypeParamThree(SKILL_BERSERK);
	nMax = (int)D2CommonSkillsGetTypeParamFour(SKILL_BERSERK);
	nEndFrame = ptGame->nFrame + nMax - D2CommonSkillsCalcCurveValue(nLevel, 0, nMax - nMin);
	hList = D2CommonStatsFindListByState(hUnit, STATE_BERSERK);
	if (!hList)
	{
		hList = D2CommonStatsAllocList(ptGame->pMemPool, 2, nEndFrame, hUnit->eType, hUnit->dwId);
		if (!hList)
			return TRUE;
		D2CommonStatsSetListState(hList, STATE_BERSERK);
		D2CommonStatsSetListRemoveCallback(hList, SkillsStateRemove);
		D2CommonStatsUnitLinkList(hUnit, hList);
		D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_BERSERK, 1);
	}
	D2CommonStatsSetListEndFrame(hList, nEndFrame);
	EventAdd(ptGame, hUnit, 12, nEndFrame);
	D2CommonStatsListSetValueB(hList, 31, -D2CommonUnitCalcArmorClassWithBonus(hUnit));
	D2CommonStatsListSetValueB(hList, 36, D2CommonSkillsGetTypeParamFive(SKILL_BERSERK));
	return TRUE;
}

BOOL __fastcall SkillsWarCry(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsNova(ptGame, hUnit, hUnit, MISSILE_WARCRY, nLevel, 0);
	return TRUE;
}

BOOL __fastcall SkillsBattleCommand(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsNova(ptGame, hUnit, hUnit, MISSILE_BATTLECOMMAND, nLevel, 0);
	SkillsBattleCommandApply(ptGame, hUnit, hUnit, nLevel);
	return TRUE;
}

BOOL __fastcall SkillsConcentrateStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	DWORD hList;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget,
		D2CommonSkillsCalcToHitByLevel(SKILL_CONCENTRATE, nLevel));
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	if (!D2CommonStatsFindListByState(hUnit, STATE_CONCENTRATE))
	{
		hList = D2CommonStatsAllocList(ptGame->pMemPool, 4, 0, hUnit->eType, hUnit->dwId);
		if (hList)
		{
			D2CommonStatsUnitLinkList(hUnit, hList);
			D2CommonStatsSetListState(hList, STATE_CONCENTRATE);
			D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_CONCENTRATE, 1);
			D2CommonStatsSetListRemoveCallback(hList, SkillsConcentrateEnd);
			D2CommonStatsListSetValueB(hList, 171, D2CommonSkillsCalcParamOneByLevel(SKILL_CONCENTRATE,
				nLevel));
			SUnitSetStateEndLinked(hUnit, TRUE);
		}
	}
	return TRUE;
}

void __fastcall SkillsConcentrateEnd(UNIT *hUnit, DWORD dwState)
{
	SUnitSetStateEndLinked(hUnit, FALSE);
}

BOOL __fastcall SkillsConcentrate(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE *ptHeld;
	UNIT *hTarget;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	ptHeld = SUnitDmgFindHeld(hUnit, hTarget);
	if (!ptHeld)
		return TRUE;
	if (ptHeld->wFlags & 1)
		D2CommonUnitAddAnimOverlay(hTarget, 54, 0);
	return SkillsUseHit(ptGame, hUnit, nLevel);
}

BOOL __fastcall SkillsBarStart(GAME *ptGame, UNIT *hUnit, int nSkillId  ,
	int nLevel)
{
#line 1888
	D2ASSERT(nSkillId >= SKILL_BARBARIAN_START);
#line 1889
	D2ASSERT(nSkillId - SKILL_BARBARIAN_START < NUM_BARBARIAN_SKILLS);
	if (!SkillsHasMana(hUnit))
		return FALSE;
	if (sgptSkillChart[nSkillId - SKILL_BARBARIAN_START].pfnStart) {
#line 1896
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgptSkillChart[nSkillId - SKILL_BARBARIAN_START].pfnStart),
			"sgptSkillChart[nSkillId - SKILL_BARBARIAN_START].pfnStart");
		if (sgptSkillChart[nSkillId - SKILL_BARBARIAN_START].pfnStart(ptGame, hUnit, nLevel))
			SkillsUseMana(hUnit);
		else
			return FALSE;
	}
	return TRUE;
}
#line 1904

BOOL __fastcall SkillsBarUse(GAME *ptGame, UNIT *hUnit, int nSkillId,
	int nLevel)
{
	MISSILESTXT *ptMissilesTxt;
	int nBarSkillId;

	nBarSkillId = nSkillId - SKILL_BARBARIAN_START;
#line 1918
	D2ASSERT(nBarSkillId >= 0);
#line 1919
	D2ASSERT(nBarSkillId < NUM_BARBARIAN_SKILLS);
	if (sgptSkillChart[nBarSkillId].pfnUse)
	{
#line 1922
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgptSkillChart[nBarSkillId].pfnUse),
			"sgptSkillChart[nBarSkillId].pfnUse");
		if (sgptSkillChart[nBarSkillId].pfnUse(ptGame, hUnit, nLevel)) {
			if (!sgptSkillChart[nBarSkillId].pfnStart)
				SkillsUseMana(hUnit);
		} else
			return FALSE;
	}
	if (sgptSkillChart[nBarSkillId].nMissile != SKILLCHART_NOMISSILE)
	{
		ptMissilesTxt = (MISSILESTXT *)D2CommonMissileTblsGetMissile(sgptSkillChart[nBarSkillId].nMissile);
		hUnit->dwFlags0 |= 64;
		SkillsCreateMissile(ptGame, sgptSkillChart[nBarSkillId].nMissile, hUnit, nLevel, 0, 0, 0, 0,
			ptMissilesTxt->Qty);
		if (!sgptSkillChart[nBarSkillId].pfnStart)
			SkillsUseMana(hUnit);
	}
	return TRUE;
}
