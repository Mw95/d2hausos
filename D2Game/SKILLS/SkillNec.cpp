#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\SKILLS\\SkillNec.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../AI/AiGeneral.h"
#include "../AI/AiThink.h"
#include "../GAME/Event.h"
#include "../GAME/SCmd.h"
#include "../GAME/Targets.h"
#include "../GUILDS/Guilds.h"
#include "../ITEMS/ItemMode.h"
#include "../MISSILES/Missiles.h"
#include "../MISSILES/MissMode.h"
#include "../MONSTER/MonsterAI.h"
#include "../MONSTER/MonsterMode.h"
#include "../MONSTER/MonsterUnique.h"
#include "../PLAYER/PlayerPets.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "SkillAma.h"
#include "SkillMonst.h"
#include "SkillNec.h"
#include "Skills.h"
#include "SkillCurse.h"

#define SKILL_NECROMANCER_START	66
#define NUM_NECROMANCER_SKILLS	30
#define STATE_BONEARMOR			14
#define STATE_IRONGOLEM			36
#define STATE_IRONMAIDEN		55
#define STATE_LIFETAP			58
#define UNIT_ITEM				4
#define UNIT_STATE_UNINTERRUPTABLE	54
#define StatsGetState(hUnit, nState) \
	D2CommonStatsGetStateRaw((DWORD)(hUnit), (DWORD)(nState))
#define STATE_REVIVE			96
#define NUM_SKELETON_LOOKS		11

static inline int SkillsNecCurseLength(GAME *ptGame, int nLength)
{
	MONDIFFICULTYTXT *ptDifficultyLevel;
	BYTE bDifficulty;

	bDifficulty = ptGame->bDifficultyLevel;
	ptDifficultyLevel = (MONDIFFICULTYTXT *)D2CommonDataTblsGetDifficultyLevel(bDifficulty);
#line 24
	D2ASSERT(ptDifficultyLevel->nMonsterAICurseDivisor);
	return nLength / ptDifficultyLevel->nMonsterAICurseDivisor;
}

BOOL __fastcall SkillsMonstValidTarget(GAME *ptGame, UNIT *hUnit, UNIT *hTarget)
{
	DWORD dwFlags;

	if (!hTarget)
		return FALSE;
	if (hTarget->eType == UNIT_MONSTER
		&& !((MONSTATSTXT *)D2CommonMonsterTblsGetStats(hTarget->dwClassId))->bModeW)
		return FALSE;
	dwFlags = hTarget->dwFlags0;
	if (!(dwFlags & 4))
		return FALSE;
	if (!(dwFlags & 8))
		return FALSE;
	if (!(dwFlags & 2))
		return FALSE;
	if (!SUnitIsHostile(ptGame, hUnit, hTarget))
		return FALSE;
	if (SUnitIsDead(hTarget))
		return FALSE;
	return TRUE;
}

BOOL __fastcall SkillsTeeth(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nCount;
	int nX;
	int nY;
	int nDX;
	int nDY;

	nCount = D2CommonSkillsCalcParamOneByLevel(SKILL_TEETH, nLevel);
	hUnit->dwFlags0 |= 64;
	if (!SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY))
		return FALSE;
	nDX = nX - (int)D2CommonUnitGetX(hUnit);
	nDY = nY - (int)D2CommonUnitGetY(hUnit);
	SkillsStepLengthen(&nDX, &nDY);
	SkillsStepTurn(&nDX, &nDY);
	nX += -(nDX * nCount / 2);
	nY += -(nDY * nCount / 2);
	for (; nCount > 0; nCount--)
	{
		SkillsCreateMissile(ptGame, MISSILE_TEETH, hUnit, nLevel, 0, 0, nX, nY, 0);
		nX += nDX;
		nY += nDY;
	}
	return TRUE;
}

BOOL __fastcall SkillsPoisonNova(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsNova(ptGame, hUnit, hUnit, MISSILE_POISONNOVA, nLevel,
		D2CommonMissileTblsGetVel(MISSILE_POISONNOVA));
	return TRUE;
}

BOOL __fastcall SkillsCorpseExplosionStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget && D2CommonSkillsUnitTestGuardB(hTarget))
		return TRUE;
	return FALSE;
}

BOOL __fastcall SkillsCorpseExplosion(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	SKILLSTXT *ptSkill;
	UNIT *hTarget;
	UNIT *hMissile;
	int nMaxLife;
	int nMin;
	int nRange;
	int nDamage;
	int nX;
	int nY;
	int nRadius;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget || !D2CommonSkillsUnitTestGuardB(hTarget))
		return FALSE;
	ptSkill = (SKILLSTXT *)D2CommonDataTblsGetSkill(SKILL_CORPSE_EXPLOSION);
	nMaxLife = D2CommonStatsUnitGetBaseValue(hTarget, 7);
	nMin = ptSkill->Param[0] * nMaxLife / 100;
	nRange = ptSkill->Param[1] * nMaxLife / 100 - nMin;
	nDamage = (nRange ? (int)D2CommonSeedRand(&hTarget->tSeed, nRange) : 0) + nMin;
	nX = (int)D2CommonUnitGetX(hTarget);
	nY = (int)D2CommonUnitGetY(hTarget);
	nRadius = D2CommonSkillsCalcParamThreeByLevel(SKILL_CORPSE_EXPLOSION, nLevel);
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.nPhysicalDamage = nDamage / 2;
	tDamage.nFireDamage = nDamage / 2;
	hMissile = SkillsCreateMissileAt(ptGame, hUnit, (DWORD)nLevel, MISSILE_CORPSEEXPLOSION, nX, nY);
#line 168
	D2ASSERT(hMissile);
	SUnitDmgSplash(ptGame, hMissile, nX, nY, nRadius, &tDamage, FALSE, FALSE, NULL, 0);
	MissModeCollide(ptGame, hMissile, NULL, FALSE);
	D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hTarget), 1, hTarget->dwId);
	SUnitFree(ptGame, hTarget);
	return TRUE;
}

BOOL __fastcall SkillsBoneArmor(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DWORD hList;
	int nAbsorb;

	nAbsorb = D2CommonSkillsCalcParamOneByLevel(SKILL_BONE_ARMOR, nLevel) << 8;
	hUnit->dwFlags0 |= 64;
	D2CommonUnitRoomAddToChangedList(hUnit);
	hList = D2CommonStatsFindListByState(hUnit, STATE_BONEARMOR);
	if (!hList)
	{
		D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_BONEARMOR, 1);
		hList = D2CommonStatsAllocList(ptGame->pMemPool, 0, 0, hUnit->eType, hUnit->dwId);
		D2CommonStatsSetListState(hList, STATE_BONEARMOR);
		D2CommonStatsListSetValueB(hList, 132, nAbsorb);
		D2CommonStatsListSetValueB(hList, 133, nAbsorb);
		D2CommonStatsUnitLinkList(hUnit, hList);
		return TRUE;
	}
	D2CommonStatsUnitSetActiveStateValue(hUnit, STATE_BONEARMOR, 1);
	D2CommonStatsListSetValueB(hList, 132, nAbsorb);
	D2CommonStatsListSetValueB(hList, 133, nAbsorb);
	return TRUE;
}

void __fastcall SkillsBoneArmorAbsorb(UNIT *hUnit, DAMAGE *ptDamage)
{
	DWORD hList;
	int nAbsorb;

	hList = D2CommonStatsFindListByState(hUnit, STATE_BONEARMOR);
#line 236
	D2WARN(hList);
	nAbsorb = (int)D2CommonStatsListGetValueA(hList, 132);
	if (nAbsorb)
	{
		if (nAbsorb <= ptDamage->nPhysicalDamage)
		{
			ptDamage->nPhysicalDamage -= nAbsorb;
			nAbsorb = 0;
		}
		else
		{
			nAbsorb -= ptDamage->nPhysicalDamage;
			ptDamage->nPhysicalDamage = 0;
		}
		D2CommonStatsListSetValueB(hList, 132, nAbsorb);
		if (nAbsorb)
		{
			D2CommonStatsUnitSetActiveStateValue(hUnit, STATE_BONEARMOR, 1);
			return;
		}
	}
	D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_BONEARMOR, 0);
	D2CommonStatsUnitUnlinkList(hUnit, hList);
	D2CommonStatsListFree(hList);
}

BOOL __fastcall SkillsClayGolem(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	return SkillsNecGolem(ptGame, hUnit, nLevel, SKILL_CLAY_GOLEM);
}

BOOL __fastcall SkillsNecGolem(GAME *ptGame, UNIT *hUnit, int nLevel,
	int nSkillId)
{
	SKILLSUMMON tSummon;
	UNIT *hGolem;
	UNIT *hTarget;
	UNIT *hItem;
	DWORD dwClass;
	DWORD hList;
	DWORD hSkill;
	BYTE bLoc;

	BOOL fRet;
	int nResult;
	int nPercent;
	int nLife;
	int nValue;
	int nSkillLevel;
	int i;

	switch (nSkillId)
	{
	case SKILL_CLAY_GOLEM:
		dwClass = MONSTER_CLAYGOLEM;
		break;
	case SKILL_BLOODGOLEM:
		dwClass = MONSTER_BLOODGOLEM;
		break;
	case SKILL_IRONGOLEM:
		dwClass = MONSTER_IRONGOLEM;
		break;
	case SKILL_FIREGOLEM:
		dwClass = MONSTER_FIREGOLEM;
		break;
	default:
		return FALSE;
	}
	for (i = 0; i < (int)(sizeof(SKILLSUMMON) / sizeof(DWORD)); i++)
		((DWORD *)&tSummon)[i] = 0;
	tSummon.dwFlags = 0;
	tSummon.hOwner = hUnit;
	tSummon.dwClassId = dwClass;
	tSummon.dwAiKind = 7;
	tSummon.dwMode = 8;
	tSummon.dwPetType = 3;
	if (nSkillId == SKILL_IRONGOLEM)
	{
		tSummon.dwFlags = 1;
		hTarget = SUnitGetTarget(ptGame, hUnit);
		if (hTarget)
		{
			tSummon.nX = (int)D2CommonUnitGetX(hTarget);
			tSummon.nY = (int)D2CommonUnitGetY(hTarget);
		}
		else
		{
			tSummon.nX = (int)D2CommonUnitGetX(hUnit);
			tSummon.nY = (int)D2CommonUnitGetY(hUnit);
		}
	}
	hGolem = SkillsSummon(ptGame, &tSummon);
	if (!hGolem)
		return FALSE;
	D2CommonStatsUnitSetValue(hGolem, 12, D2CommonStatsUnitGetValue(hUnit, 12));
	switch (nSkillId)
	{
	case SKILL_CLAY_GOLEM:
		nSkillLevel = nLevel - 1;
		nValue = (int)D2CommonSkillsGetTypeParamOne(SKILL_CLAY_GOLEM) * nSkillLevel + 100;
		nLife = D2CommonStatsUnitCalcMaxHp(hGolem) * nValue / 100;
		D2CommonStatsUnitSetValue(hGolem, 7, nLife);
		D2CommonStatsUnitSetValue(hGolem, 6, nLife);
		D2CommonStatsUnitSetValue(hGolem, 25,
			D2CommonSkillsGetTypeParamTwo(SKILL_CLAY_GOLEM) * nSkillLevel);
		D2CommonStatsUnitSetValue(hGolem, 74, 0);
		break;
	case SKILL_BLOODGOLEM:
		D2CommonStatsUnitSetValue(hGolem, 25,
			D2CommonSkillsGetTypeParamFour(SKILL_BLOODGOLEM) * (nLevel - 1));
		break;
	case SKILL_IRONGOLEM:
		hItem = SUnitGetTarget(ptGame, hUnit);
		if (hItem && hItem->eType == UNIT_ITEM && D2CommonITEMSTestTypeBitfieldSecondBit(hItem->dwClassId))
		{ BYTE bLoc2;
			D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hItem), hItem->eType, hItem->dwId);
			D2CommonUnitClearCollision(hItem);
			D2CommonUnitRoomRemoveUnit(hItem);
			D2CommonITEMSGetTypeBodyLocs(hItem, &bLoc, &bLoc2);
			D2CommonUnitSetMode(hItem, 4);
			fRet = ITEMSModeEquipCursorItem(ptGame, hGolem, hItem->dwId, bLoc, TRUE, &nResult);
#line 417
			D2ASSERT(fRet);
		}
		if (nLevel > 1)
		{
			D2CommonStatsUnitSetActiveStateBit(hGolem, STATE_IRONGOLEM, 1);
			hList = D2CommonStatsAllocList(ptGame->pMemPool, 0, 0, hGolem->eType, hGolem->dwId);
			D2CommonStatsSetListState(hList, STATE_IRONGOLEM);
			D2CommonStatsListSetValueB(hList, 131, nLevel - 1);
			D2CommonStatsUnitLinkList(hGolem, hList);
		}
		break;
	case SKILL_FIREGOLEM:
		hList = D2CommonStatsAllocList(ptGame->pMemPool, 0, 0, hGolem->eType, hGolem->dwId);
		if (hList)
		{
			D2CommonStatsListSetValueA(hList, 39, 100 - (DWORD)D2CommonSkillsCalcCurveParamOne(nLevel,
				SKILL_FIREGOLEM));
			D2CommonStatsListSetValueA(hList, 142, 100);
			D2CommonStatsUnitLinkList(hGolem, hList);
		}
		nValue = D2CommonSkillsCalcParamFiveByLevel(SKILL_FIREGOLEM, nLevel);
		if (nValue > 20)
			nValue = 20;
		SkillsSetLevel(hGolem, SKILL_HOLY_FIRE, (DWORD)nValue, 1);
		SkillsSetCurrent(hGolem, FALSE, SKILL_HOLY_FIRE);
		D2CommonUnitAddAnimOverlay(hGolem, 164, 0);
		break;
	}
	nSkillLevel = (int)D2CommonSkillsCalcNodeLevel(hUnit, D2CommonSkillsFindByKey(hUnit, SKILL_SUMMON_RESIST), 1);
	if (nSkillLevel)
	{
		nValue = D2CommonSkillsCalcCurveParamOne(nSkillLevel, SKILL_SUMMON_RESIST);
		hList = D2CommonStatsAllocList(hUnit->dwMemPool, 0, 0, hGolem->eType, hGolem->dwId);
		if (hList)
		{
			D2CommonStatsUnitLinkList(hGolem, hList);
			if (hGolem->dwClassId != MONSTER_FIREGOLEM)
				D2CommonStatsListSetValueB(hList, 39, nValue);
			D2CommonStatsListSetValueB(hList, 41, nValue);
			D2CommonStatsListSetValueB(hList, 43, nValue);
			D2CommonStatsListSetValueB(hList, 45, nValue);
		}
	}
	hSkill = D2CommonSkillsFindByKey(hUnit, SKILL_GOLEM_MASTERY);
	if (hSkill)
	{
		nSkillLevel = (int)D2CommonSkillsCalcNodeLevel(hUnit, hSkill, 1);
		if (nSkillLevel)
		{
			nLife = D2CommonStatsUnitCalcMaxHp(hGolem);
			nLife += (int)D2CommonSkillsGetTypeParamOne(SKILL_GOLEM_MASTERY) * nLife * nSkillLevel / 100;
			D2CommonStatsUnitSetValue(hGolem, 7, nLife);
			D2CommonStatsUnitSetValue(hGolem, 6, nLife);
			nValue = D2CommonSkillsCalcCurveParamThree(nSkillLevel, SKILL_GOLEM_MASTERY);
			hList = D2CommonStatsAllocList(hUnit->dwMemPool, 0, 0, UNIT_MONSTER, hGolem->dwId);
			if (hList)
			{
				D2CommonStatsUnitLinkList(hGolem, hList);
				D2CommonStatsListSetValueB(hList, 67, nValue);
			}
		}
	}
	SCmdSendLife(SUnitGetClient(hUnit), hGolem);
	TargetsAddToList(ptGame, hGolem, 0, hUnit->nTargetListNo);
	return TRUE;
}

BOOL __fastcall SkillsBloodGolem(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	return SkillsNecGolem(ptGame, hUnit, nLevel, SKILL_BLOODGOLEM);
}

BOOL __fastcall SkillsIronGolemStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	if (hTarget->eType != UNIT_ITEM)
		return FALSE;
	return D2CommonITEMSTestTypeBitfieldSecondBit(hTarget->dwClassId) != 0;
}

BOOL __fastcall SkillsIronGolem(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	return SkillsNecGolem(ptGame, hUnit, nLevel, SKILL_IRONGOLEM);
}

BOOL __fastcall SkillsFireGolem(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	return SkillsNecGolem(ptGame, hUnit, nLevel, SKILL_FIREGOLEM);
}

BOOL __fastcall SkillsReviveStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget && hTarget->eType == UNIT_MONSTER
		&& D2CommonMonsterUnitGetTypeLocalIndex(hTarget) != MONSTER_SANDLEAPER
		&& (!hTarget->dwMode || hTarget->dwMode == 12)
		&& D2CommonSkillsUnitTestGuardF(hTarget, 0)
		&& SkillsCanBeAffected(hTarget, 7))
		return TRUE;
	return FALSE;
}

BOOL __fastcall SkillsRevive(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MONMODECHANGE tReq;
	COLPOINT tPoint;
	UNIT *hTarget;
	UNIT *hOwner;
	GAMEROOM *hRoom;
	DWORD hList;
	BOOL fRet;
	int nLife;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if ((BOOL)(!hTarget || hTarget->eType != UNIT_MONSTER
		|| D2CommonMonsterUnitGetTypeLocalIndex(hTarget) == MONSTER_SANDLEAPER
		|| (hTarget->dwMode && hTarget->dwMode != 12)
		|| !D2CommonSkillsUnitTestGuardF(hTarget, 0)
		|| !SkillsCanBeAffected(hTarget, 7)))
		return FALSE;
	D2CommonCollisnClearPattern(D2CommonUnitGetPathRoom(hTarget), (int)D2CommonUnitGetX(hTarget),
		(int)D2CommonUnitGetY(hTarget), D2CommonPathGetCollisionPatternByUnit(hTarget), 32768);
	hTarget->dwFlags0 |= 67239950;
	MonsterRestartAi(ptGame, hTarget);
	for (i = 0; i < (int)(sizeof(MONMODECHANGE) / sizeof(DWORD)); i++)
		((DWORD *)&tReq)[i] = 0;
	MonsterModeInit(hTarget, 1, &tReq);
	MonsterModeSet(ptGame, &tReq, TRUE);
	tPoint.nX = (int)D2CommonUnitGetX(hTarget);
	tPoint.nY = (int)D2CommonUnitGetY(hTarget);
	hRoom = (GAMEROOM *)D2CommonCollisnFindSpawnPointNear(D2CommonUnitGetPathRoom(hTarget), &tPoint,
		D2CommonUnitGetSizeX(hTarget), 7169, 1);
	hOwner = AiGetOwner(hTarget);
	if (hOwner && hOwner->eType == UNIT_MONSTER)
	{
		if (hOwner == hTarget)
			AiFreeMinions(hTarget);
		else
			AiRemoveFromOwner(hTarget);
	}
	hList = D2CommonStatsAllocList(ptGame->pMemPool, 0, 0, hTarget->eType, hTarget->dwId);
	if (hList)
	{
		D2CommonStatsListSetValueB(hList, 67, D2CommonSkillsGetTypeParamFive(SKILL_REVIVE));
		D2CommonStatsUnitLinkList(hTarget, hList);
	}
	EventRemoveByType(ptGame, hTarget, 8);
	if (D2CommonUnitGetRightSkill(hTarget))
		D2CommonSkillsCacheRightSkillRaw((DWORD)hTarget, 0);
	fRet = SUnitMoveToRoom(ptGame, hTarget, hRoom, tPoint.nX, tPoint.nY, FALSE, FALSE);
#line 734
	D2ASSERT(fRet);
	nLife = D2CommonStatsUnitGetBaseValue(hTarget, 7);
	nLife += D2CommonSkillsCalcParamOneByLevel(SKILL_REVIVE, nLevel) * nLife / 100;
	nLife += ((int)D2CommonSkillsGetTypeParamOne(SKILL_SKELETON_MASTERY) * nLevel) << 8;
	D2CommonStatsUnitSetValue(hTarget, 6, nLife);
	D2CommonStatsUnitSetValue(hTarget, 7, nLife);
	D2CommonStatsUnitSetValue(hTarget, 25,
		D2CommonSkillsGetTypeParamOne(SKILL_SKELETON_MASTERY) * nLevel + 100);
	AiSetOwner(ptGame, hTarget, hUnit->dwId, hUnit->eType, FALSE, FALSE);
	AiSetUnitRef(hTarget, hUnit);
	EventRemoveByType(ptGame, hTarget, 2);
	EventAdd(ptGame, hTarget, 2, ptGame->nFrame + 15);
	SUnitSetAlignment(hTarget, 0);
	hTarget->dwFlags0 |= 0x80000000;
	D2CommonStatsUnitSetActiveStateBit(hTarget, STATE_REVIVE, 1);
	MonsterUniqueAddMod(ptGame, hTarget, 21, FALSE);
	EventAdd(ptGame, hTarget, 7, D2CommonSkillsCalcParamThreeByLevel(SKILL_REVIVE, nLevel) + ptGame->nFrame);
	PlayerPetsAdd(ptGame, hUnit, hTarget, 6);
	TargetsAddToList(ptGame, hTarget, 0, hUnit->nTargetListNo);
	return TRUE;
}
#line 755

BOOL __fastcall SkillsRaiseStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	return D2CommonSkillsUnitTestGuardF(SUnitGetTarget(ptGame, hUnit), 0);
}

BOOL __fastcall SkillsRaise(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLSUMMON tSummon;
	UNIT *hTarget;
	UNIT *hPet;
	DWORD hSkill;
	DWORD dwClass;
	DWORD hList;
	BOOL bShield;
	int nKind;
	int nX;
	int nY;
	int nMastery;
	int nLife;
	int nChance;
	int nSkillLevel;
	int nValue;
	int i;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!D2CommonSkillsUnitTestGuardF(hTarget, 0))
		return FALSE;
	switch (D2CommonSkillsGetNodeType(hSkill))
	{
	case SKILL_RAISESKELETON:
		dwClass = MONSTER_NECROSKELETON;
		nKind = 4;
		break;
	case SKILL_RAISESKELETONMAGE:
		dwClass = MONSTER_NECROMAGE;
		nKind = 5;
		break;
	default:
		return FALSE;
	}
	nX = (int)D2CommonUnitGetX(hTarget);
	nY = (int)D2CommonUnitGetY(hTarget);
	D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hTarget), 1, hTarget->dwId);
	SUnitFree(ptGame, hTarget);
	for (i = 0; i < (int)(sizeof(SKILLSUMMON) / sizeof(DWORD)); i++)
		((DWORD *)&tSummon)[i] = 0;
	tSummon.nY = nY;
	tSummon.dwFlags = 1;
	tSummon.hOwner = hUnit;
	tSummon.dwClassId = dwClass;
	tSummon.dwPetType = (DWORD)nKind;
	tSummon.dwAiKind = 7;
	tSummon.dwMode = 8;
	tSummon.nX = nX;
	hPet = SkillsSummon(ptGame, &tSummon);
	if (hPet)
	{
		switch (dwClass)
		{
		case MONSTER_NECROSKELETON:
			hSkill = D2CommonSkillsFindByKey(hUnit, SKILL_SKELETON_MASTERY);
			bShield = FALSE;
			nMastery = 0;
			if (hSkill)
				nMastery = (int)D2CommonSkillsCalcNodeLevel(hUnit, hSkill, 1);
			D2CommonStatsUnitSetValue(hPet, 12, D2CommonStatsUnitGetValue(hUnit, 12));
			D2CommonStatsUnitSetValue(hPet, 74, 0);
			D2CommonStatsUnitSetValue(hPet, 67, 200);
			nLife = D2CommonStatsUnitGetValue(hPet, 6);
			nLife += ((int)D2CommonSkillsGetTypeParamOne(SKILL_SKELETON_MASTERY) * nMastery) << 8;
			D2CommonStatsUnitSetValue(hPet, 6, nLife);
			D2CommonStatsUnitSetValue(hPet, 7, nLife);
			if (nLevel > 2)
			{
				nChance = (int)D2CommonSkillsGetTypeParamOne(SKILL_RAISESKELETON);
				if ((int)D2CommonSeedRand(&hUnit->tSeed, 100) < nChance)
					bShield = TRUE;
			}
			SkillsNecSkeletonLook(hUnit, hPet, nMastery, bShield, FALSE);
			break;
		case MONSTER_NECROMAGE:
			hSkill = D2CommonSkillsFindByKey(hUnit, SKILL_SKELETON_MASTERY);
			nMastery = 0;
			if (hSkill)
				nMastery = (int)D2CommonSkillsCalcNodeLevel(hUnit, hSkill, 1);
			D2CommonStatsUnitSetValue(hPet, 12, D2CommonStatsUnitGetValue(hUnit, 12));
			D2CommonStatsUnitSetValue(hPet, 74, 0);
			SkillsNecSkeletonLook(hUnit, hPet, nMastery, FALSE, TRUE);
			break;
		}
		nSkillLevel = (int)D2CommonSkillsCalcNodeLevel(hUnit, D2CommonSkillsFindByKey(hUnit, SKILL_SUMMON_RESIST), 1);
		if (nSkillLevel)
		{
			nValue = D2CommonSkillsCalcCurveParamOne(nSkillLevel, SKILL_SUMMON_RESIST);
			hList = D2CommonStatsAllocList(hUnit->dwMemPool, 0, 0, hPet->eType, hPet->dwId);
			if (hList)
			{
				D2CommonStatsUnitLinkList(hPet, hList);
				if (hPet->dwClassId != MONSTER_FIREGOLEM)
					D2CommonStatsListSetValueB(hList, 39, nValue);
				D2CommonStatsListSetValueB(hList, 41, nValue);
				D2CommonStatsListSetValueB(hList, 43, nValue);
				D2CommonStatsListSetValueB(hList, 45, nValue);
			} }
		TargetsAddToList(ptGame, hPet, 0, hUnit->nTargetListNo);
		return TRUE; }
	return FALSE;
}

static BYTE sgabSkeletonLooks[NUM_SKELETON_LOOKS][9] =
{
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 1, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 1, 0, 0, 0, 0, 0, 1, 1 },
	{ 1, 1, 0, 0, 0, 0, 1, 1, 2 },
	{ 1, 1, 0, 0, 0, 0, 1, 1, 2 },
	{ 1, 1, 1, 0, 0, 1, 1, 1, 3 },
	{ 2, 2, 1, 0, 0, 1, 1, 1, 3 },
	{ 2, 2, 1, 0, 0, 1, 1, 2, 4 },
	{ 2, 3, 1, 0, 0, 1, 2, 2, 4 },
	{ 2, 3, 1, 0, 0, 1, 2, 2, 4 },
};

void __fastcall SkillsNecSkeletonLook(UNIT *hOwner, UNIT *hPet, int nMastery,
	BOOL bShield, BOOL bMage)
{
	int nRand;

	nRand = (int)(D2CommonSeedNext(&hOwner->tSeed) & 3);
	if (nMastery > 10)
		nMastery = 10;
	hPet->abComponentVariations[0] = sgabSkeletonLooks[nMastery][0];
	hPet->abComponentVariations[1] = sgabSkeletonLooks[nMastery][2];
	hPet->abComponentVariations[8] = sgabSkeletonLooks[nMastery][3];
	hPet->abComponentVariations[9] = sgabSkeletonLooks[nMastery][4];
	hPet->abComponentVariations[2] = sgabSkeletonLooks[nMastery][5];
	hPet->abComponentVariations[3] = sgabSkeletonLooks[nMastery][6];
	hPet->abComponentVariations[4] = sgabSkeletonLooks[nMastery][7];
	if (bShield)
		hPet->abComponentVariations[7] = sgabSkeletonLooks[nMastery][1];
	if (bMage)
	{
		hPet->abComponentVariations[11] = (BYTE)nRand;
		hPet->abComponentVariations[12] = (BYTE)nRand;
		AiSetParams(((MONSTERDATA *)hPet->uTypeData)->ptAi, 1, nRand + 208, -666);
		return;
	}
	hPet->abComponentVariations[5] = sgabSkeletonLooks[nMastery][8];
}

BOOL __fastcall SkillsAmplifyDamage(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsCurseArea(ptGame, 3, hUnit, nLevel, SKILL_NECROMANCER_START, SkillsAmplifyDamageCurse);
	return TRUE;
}

void __fastcall SkillsAmplifyDamageCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel)
{
	SKILLSTATE tState;
	int i;

	if (!SkillsMonstValidTarget(ptGame, hUnit, hTarget))
		return;
	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tState)[i] = 0;
	tState.hSource = hUnit;
	tState.hTarget = hTarget;
	tState.nSkill = SKILL_NECROMANCER_START;
	tState.nLevel = nLevel;
	tState.nDuration = D2CommonSkillsCalcParamThreeByLevel(SKILL_NECROMANCER_START, nLevel);
	tState.dwStatId = 36;
	tState.nStatValue = -(int)D2CommonSkillsGetTypeParamFive(SKILL_NECROMANCER_START);
	tState.dwState = 9;
	tState.pfnRemove = 0;
	SkillsSetState(&tState);
}

void __fastcall SkillsDimVisionEnd(UNIT *hUnit, DWORD dwState)
{
	GAME *ptGame;

	ptGame = SUnitGetGame(hUnit);
#line 1106
	D2WARN(!StatsGetState (hUnit, UNIT_STATE_UNINTERRUPTABLE));
	AiThinkSet(ptGame, hUnit, ((MONSTERDATA *)hUnit->uTypeData)->ptAi, 0);
	SkillsStateRemove(hUnit, dwState);
}

void __fastcall SkillsDimVisionCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel)
{
	SKILLSTATE tState;
	int nLength;
	int i;

	if (hTarget->eType != UNIT_MONSTER)
		return;
	if (D2CommonStatsUnitGetAlignment(hTarget) == 1)
		return;
	if (!SUnitIsHostile(ptGame, hUnit, hTarget))
		return;
	if (SUnitIsDead(hTarget))
		return;
	if (!SkillsCanBeAffected(hTarget, 10))
		return;
	nLength = SkillsNecCurseLength(ptGame, D2CommonSkillsCalcParamThreeByLevel(SKILL_DIM_VISION, nLevel));
	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tState)[i] = 0;
	tState.hSource = hUnit;
	tState.hTarget = hTarget;
	tState.nSkill = SKILL_DIM_VISION;
	tState.nLevel = nLevel;
	tState.nDuration = nLength;
	tState.dwStatId = 0xFFFFFFFF;
	tState.nStatValue = 0;
	tState.dwState = 23;
	tState.pfnRemove = (DWORD)SkillsDimVisionEnd;
	SkillsSetState(&tState);
	AiThinkSet(ptGame, hTarget, ((MONSTERDATA *)hTarget->uTypeData)->ptAi, 10);
}

BOOL __fastcall SkillsDimVision(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsCurseArea(ptGame, 2, hUnit, nLevel, SKILL_DIM_VISION, SkillsDimVisionCurse);
	return TRUE;
}

BOOL __fastcall SkillsWeaken(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsCurseArea(ptGame, 3, hUnit, nLevel, SKILL_WEAKEN, SkillsWeakenCurse);
	return TRUE;
}

void __fastcall SkillsWeakenCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel)
{
	SKILLSTATE tState;
	int i;

	if (!SkillsMonstValidTarget(ptGame, hUnit, hTarget))
		return;
	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tState)[i] = 0;
	tState.hSource = hUnit;
	tState.hTarget = hTarget;
	tState.nSkill = SKILL_WEAKEN;
	tState.nLevel = nLevel;
	tState.nDuration = D2CommonSkillsCalcParamThreeByLevel(SKILL_WEAKEN, nLevel);
	tState.dwStatId = 25;
	tState.nStatValue = -(int)D2CommonSkillsGetTypeParamFive(SKILL_WEAKEN);
	tState.dwState = 19;
	tState.pfnRemove = 0;
	SkillsSetState(&tState);
}

BOOL __fastcall SkillsIronMaiden(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsCurseArea(ptGame, 3, hUnit, nLevel, SKILL_IRON_MAIDEN, SkillsIronMaidenCurse);
	return TRUE;
}

void __fastcall SkillsIronMaidenCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel)
{

	if (!SkillsMonstValidTarget(ptGame, hUnit, hTarget))
		return;
	SkillsSetCurseState(hUnit, hTarget, SKILL_IRON_MAIDEN, nLevel, 129, nLevel, STATE_IRONMAIDEN);
}

BOOL __fastcall SkillsTerror(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsCurseArea(ptGame, 3, hUnit, nLevel, SKILL_TERROR, SkillsTerrorCurse);
	return TRUE;
}

void __fastcall SkillsTerrorCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel)
{
	int nLength;
	int nValue;

	if (!SkillsMonstValidTarget(ptGame, hUnit, hTarget))
		return;
	if (hTarget->eType != UNIT_MONSTER)
		return;
	MonsterScare(ptGame, hUnit, hTarget, (DWORD)D2CommonSkillsCalcParamFiveByLevel(SKILL_TERROR, nLevel),
		D2CommonSkillsCalcParamThreeByLevel(SKILL_TERROR, nLevel));
}

BOOL __fastcall SkillsAttractStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;
	DWORD hSkill;

	hSkill = D2CommonSkillsFindByKey(hUnit, SKILL_ATTRACT);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if ((BOOL)(!hTarget || D2CommonStatsUnitGetAlignment(hTarget) != 2 || !SkillsCanBeAffected(hTarget, 15)
		|| SUnitIsDead(hTarget) || !SUnitIsHostile(ptGame, hUnit, hTarget)))
		return FALSE;
	D2CommonSkillsSetNodeScratchC(hSkill, hTarget->eType);
	D2CommonSkillsSetNodeScratchD(hSkill, hTarget->dwId);
	return TRUE;
}

BOOL __fastcall SkillsAttract(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLSTATE tState;
	UNIT *hTarget;
	UNIT *hOwner;
	DWORD hSkill;
	DWORD dwId;
	int nLength;
	int i;

	hSkill = D2CommonSkillsFindByKey(hUnit, SKILL_ATTRACT);
	dwId = D2CommonSkillsGetNodeScratchD(hSkill) & 65535;
	hTarget = SUnitFind(ptGame, D2CommonSkillsGetNodeScratchC(hSkill), dwId);
	if ((BOOL)(!hTarget || D2CommonStatsUnitGetAlignment(hTarget) != 2 || !SkillsCanBeAffected(hTarget, 15)
		|| SUnitIsDead(hTarget) || !SUnitIsHostile(ptGame, hUnit, hTarget)))
		return FALSE;
	SUnitSetAlignment(hTarget, 1);
	TargetsAddToFurtherList(ptGame, hTarget, 0, 9);
	hOwner = AiGetOwner(hTarget);
	if (hOwner && hOwner->eType == UNIT_MONSTER)
	{
		if (hOwner == hTarget)
			AiFreeMinions(hTarget);
		else
			AiRemoveFromOwner(hTarget);
	}
	SkillsCurseArea(ptGame, 2, hUnit, nLevel, SKILL_ATTRACT, SkillsAttractCurse);
	nLength = SkillsNecCurseLength(ptGame, D2CommonSkillsCalcParamThreeByLevel(SKILL_ATTRACT, nLevel));
	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tState)[i] = 0;
	tState.hSource = hUnit;
	tState.hTarget = hTarget;
	tState.nSkill = SKILL_ATTRACT;
	tState.nLevel = nLevel;
	tState.nDuration = nLength;
	tState.dwStatId = 0xFFFFFFFF;
	tState.dwState = 57;
	tState.pfnRemove = (DWORD)SkillsAttractEnd;
	SkillsSetState(&tState);
	return TRUE;
}

void __fastcall SkillsAttractCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel)
{
	DWORD hSkill;
	DWORD dwType;
	DWORD dwId;
	int nLength;

	hSkill = D2CommonSkillsFindByKey(hUnit, SKILL_ATTRACT);
	if (!hTarget || D2CommonStatsUnitGetAlignment(hTarget) != 2 || !SkillsCanBeAffected(hTarget, 15)
		|| SUnitIsDead(hTarget) || !SUnitIsHostile(ptGame, hUnit, hTarget))
		return;
	dwType = D2CommonSkillsGetNodeScratchC(hSkill);
	dwId = D2CommonSkillsGetNodeScratchD(hSkill);
	if (hTarget->eType == dwType && hTarget->dwId == dwId)
		return;
	nLength = SkillsNecCurseLength(ptGame, D2CommonSkillsCalcParamThreeByLevel(SKILL_ATTRACT, nLevel));
	if (!dwType) MonsterAISetTargetCmd(hTarget, 1, dwId); else MonsterAISetTargetCmd(hTarget, 2, dwId);
	EventAdd(ptGame, hTarget, 10, ptGame->nFrame + nLength);
}

void __fastcall SkillsAttractEnd(UNIT *hUnit, DWORD dwState)
{
	SUnitSetAlignment(hUnit, 2);
	D2CommonStatsUnitSetActiveStateBit(hUnit, dwState, 0);
	TargetsRemove(SUnitGetGame(hUnit), hUnit);
}

BOOL __fastcall SkillsLifeTap(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsCurseArea(ptGame, 3, hUnit, nLevel, SKILL_LIFE_TAP, SkillsLifeTapCurse);
	return TRUE;
}

void __fastcall SkillsLifeTapCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel)
{

	if (!SkillsMonstValidTarget(ptGame, hUnit, hTarget))
		return;
	SkillsSetCurseState(hUnit, hTarget, SKILL_LIFE_TAP, nLevel, 130, nLevel, STATE_LIFETAP);
}

BOOL __fastcall SkillsConfuse(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsCurseArea(ptGame, 2, hUnit, nLevel, SKILL_CONFUSE, SkillsConfuseCurse);
	return TRUE;
}

void __fastcall SkillsConfuseCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel)
{
	SKILLSTATE tState;
	void *hList;
	int nLength;
	int i;

	if (hTarget->eType != UNIT_MONSTER)
		return;
	if (D2CommonStatsUnitGetAlignment(hTarget) == 1)
		return;
	if (D2CommonStatsUnitGetAlignment(hTarget) != 2)
		return;
	if (!SUnitIsHostile(ptGame, hUnit, hTarget))
		return;
	if (SUnitIsDead(hTarget))
		return;
	if (!SkillsCanBeAffected(hTarget, 11))
		return;
	nLength = SkillsNecCurseLength(ptGame, D2CommonSkillsCalcParamThreeByLevel(SKILL_CONFUSE, nLevel));
	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tState)[i] = 0;
	tState.hSource = hUnit;
	tState.hTarget = hTarget;
	tState.nSkill = SKILL_CONFUSE;
	tState.nLevel = nLevel;
	tState.nDuration = nLength;
	tState.dwStatId = 0xFFFFFFFF;
	tState.dwState = 59;
	tState.pfnRemove = 0;
	hList = SkillsSetState(&tState);
	if (hList)
	{
		SUnitSetAlignment(hTarget, 1);
		D2CommonStatsSetListRemoveCallback(hList, SkillsAttractEnd);
		TargetsAddToFurtherList(ptGame, hTarget, 0, 9);
	}
	MonsterAISetTargetCmd(hTarget, 3, 0);
	EventAdd(ptGame, hTarget, 10, ptGame->nFrame + nLength);
}

BOOL __fastcall SkillsDecrepify(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsCurseArea(ptGame, 3, hUnit, nLevel, SKILL_DECREPIFY, SkillsDecrepifyCurse);
	return TRUE;
}

void __fastcall SkillsDecrepifyCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel)
{
	DWORD hList;
	DWORD dwValue;

	if (!SkillsMonstValidTarget(ptGame, hUnit, hTarget))
		return;
	hList = (DWORD)SkillsSetCurseState(hUnit, hTarget, SKILL_DECREPIFY, nLevel, 170, nLevel, 60);
	if (!hList)
		return;
	dwValue = D2CommonSkillsGetTypeParamFive(SKILL_DECREPIFY);
	D2CommonStatsListSetValueB(hList, 67, dwValue);
	D2CommonStatsListSetValueB(hList, 36, dwValue);
	D2CommonStatsListSetValueB(hList, 25, dwValue);
}

BOOL __fastcall SkillsLowerResist(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsCurseArea(ptGame, 3, hUnit, nLevel, SKILL_LOWER_RESIST, SkillsLowerResistCurse);
	return TRUE;
}

void __fastcall SkillsLowerResistCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel)
{
	DWORD hList;
	int nResist;

	if (!SkillsMonstValidTarget(ptGame, hUnit, hTarget))
		return;
	nResist = -D2CommonSkillsCalcCurveParamFive(nLevel, SKILL_LOWER_RESIST);
	hList = (DWORD)SkillsSetCurseState(hUnit, hTarget, SKILL_LOWER_RESIST, nLevel, 39, nResist, 61);
	if (!hList)
		return;
	D2CommonStatsListSetValueA(hList, 41, nResist);
	D2CommonStatsListSetValueA(hList, 43, nResist);
	D2CommonStatsListSetValueA(hList, 45, nResist);
	D2CommonStatsListSetValueA(hList, 37, nResist);
}

BOOL __fastcall SkillsPoisonDaggerStart(GAME *ptGame, UNIT *hUnit, int nLevel)
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
		D2CommonSkillsCalcToHitByLevel(SKILL_POISON_DAGGER, nLevel));
	if (tDamage.wFlags & 1)
		SkillsElemDamage(hUnit, &tDamage, SKILL_POISON_DAGGER, nLevel);
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	return tDamage.wFlags & 1;
}

BOOL __fastcall SkillsPoisonExplosion(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget || hTarget->eType != UNIT_MONSTER || hTarget->dwMode != 12)
		return FALSE;
	MissModeCreateRingOfSixteen(ptGame, hUnit, hTarget, MISSILE_POISONEXPLOSIONCLOUD, 0, nLevel);
	D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hTarget), 1, hTarget->dwId);
	SUnitFree(ptGame, hTarget);
	return TRUE;
}

BOOL __fastcall SkillsBoneSpirit(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreateMissile;
	UNIT *hTarget;
	UNIT *hMissile;
	int nX;
	int nY;
	int nDX;
	int nDY;
	int i;

	hUnit->dwFlags0 |= 64;
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY))
		return FALSE;
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreateMissile)[i] = 0;
	tCreateMissile.dwFlags = 32;
	tCreateMissile.hSource = hUnit;
	tCreateMissile.hSpawnLocation = hUnit;
	tCreateMissile.hTarget = hTarget;
	tCreateMissile.nDX = nX;
	tCreateMissile.nDY = nY;
	tCreateMissile.eMissile = MISSILE_BONESPIRIT;
	tCreateMissile.nLevel = nLevel;
	if (!hTarget)
		tCreateMissile.dwFlags = 1056;
	hMissile = MissilesCreate(ptGame, &tCreateMissile);
	if (hMissile)
	{
		if (hTarget)
			D2CommonMissileSetScratchFirst(hMissile, 1);
		else
		{
			D2CommonMissileSetScratchFirst(hMissile, 2);
			nDX = nX - (int)D2CommonUnitGetX(hUnit);
			nDY = nY - (int)D2CommonUnitGetY(hUnit);
			D2CommonMissileSetScratchSecond(hMissile, ((DWORD)(int)(short)nDY << 16) + (DWORD)(int)(short)nDX);
		}
	}
	return TRUE;
}

BOOL __fastcall SkillsBoneWall(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreateMissile;
	SKILLSUMMON tSummon;
	UNIT *hTarget;
	UNIT *hWall;
	UNIT *hMissile;
	int nX;
	int nY;
	int nLife;
	int nHalf;
	int nDX;
	int nDY;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget)
	{
		nX = (int)D2CommonUnitGetX(hTarget);
		nY = (int)D2CommonUnitGetY(hTarget);
	}
	else
	{
		nX = (int)D2CommonPathGetTargetXRaw((DWORD)hUnit->ptPath);
		nY = (int)D2CommonPathGetTargetYRaw((DWORD)hUnit->ptPath);
	}
	if (D2CommonDungeonRoomIsTown(D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hUnit), nX, nY)))
		return FALSE;
	for (i = 0; i < (int)(sizeof(SKILLSUMMON) / sizeof(DWORD)); i++)
		((DWORD *)&tSummon)[i] = 0;
	tSummon.dwFlags = 9;
	tSummon.hOwner = hUnit;
	tSummon.dwClassId = MONSTER_BONEWALL;
	tSummon.dwAiKind = 0;
	tSummon.nY = nY;
	tSummon.dwMode = 8;
	tSummon.nX = nX;
	tSummon.dwPetType = 0;
	hWall = SkillsSummon(ptGame, &tSummon);
	if (!hWall)
		return FALSE;
	AiSetOwner(ptGame, hWall, hWall->dwId, UNIT_MONSTER, FALSE, TRUE);
	MonsterUniqueAddMod(ptGame, hWall, 15, FALSE);
	nLife = D2CommonStatsUnitCalcMaxHp(hWall);
	nLife += (int)D2CommonSkillsGetTypeParamOne(SKILL_BONE_WALL) * nLife * nLevel / 100;
	D2CommonStatsUnitSetValue(hWall, 6, nLife);
	D2CommonStatsUnitSetValue(hWall, 7, nLife);
	TargetsAddToFurtherList(ptGame, hWall, 0, 9);
	nX = (int)D2CommonUnitGetX(hWall);
	nY = (int)D2CommonUnitGetY(hWall);
	if (D2CommonDungeonRoomIsTown(D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hUnit), nX, nY)))
		return FALSE;
	nHalf = D2CommonSkillsCalcParamThreeByLevel(SKILL_BONE_WALL, nLevel) / 2;
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreateMissile)[i] = 0;
	tCreateMissile.dwFlags = 33;
	tCreateMissile.hSource = hUnit;
	tCreateMissile.nX = nX;
	tCreateMissile.nY = nY;
	tCreateMissile.nLevel = nLevel;
	tCreateMissile.eMissile = MISSILE_BONEWALLMAKER;
	nDX = (int)D2CommonUnitGetX(hUnit) - nX;
	nDY = (int)D2CommonUnitGetY(hUnit) - nY;
	tCreateMissile.nDX = nX - nDY;
	tCreateMissile.nDY = nDX + nY;
	hMissile = MissilesCreate(ptGame, &tCreateMissile);
	if (hMissile)
	{
		D2CommonMissileSetScratchFirst(hMissile, hWall->dwId);
		D2CommonMissileSetScratchSecond(hMissile, nHalf);
	}
	tCreateMissile.nDX = nDY + nX;
	tCreateMissile.nDY = nY - nDX;
	hMissile = MissilesCreate(ptGame, &tCreateMissile);
	if (hMissile)
	{
		D2CommonMissileSetScratchFirst(hMissile, hWall->dwId);
		D2CommonMissileSetScratchSecond(hMissile, nHalf);
	}
	return TRUE;
}

BOOL __fastcall SkillsBonePrisonStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	return D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hTarget)) == 0;
}

BOOL __fastcall SkillsBonePrison(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLSUMMON tSummon;
	UNIT *hFirst = NULL;
	int anDX[10] = { -1, 1, 3, 3, 2, 0, -2, -4, -4, -3 };
	int anDY[10] = { 4, 3, 2, -1, -4, -4, -3, -2, 1, 3 };
	UNIT *hTarget;
	UNIT *hWall;
	DWORD dwSpot;
	int nX;
	int nY;
	int nLife;
	int i;
	int k;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	nX = (int)D2CommonUnitGetX(hTarget);
	nY = (int)D2CommonUnitGetY(hTarget);
	for (i = 0; i < 10; i++)
	{
		for (k = 0; k < (int)(sizeof(SKILLSUMMON) / sizeof(DWORD)); k++)
			((DWORD *)&tSummon)[k] = 0;
		tSummon.dwAiKind = 0;
		tSummon.dwPetType = 0;
		tSummon.hOwner = hUnit;
		tSummon.dwFlags = 9;
		tSummon.dwClassId = MONSTER_BONEWALL;
		tSummon.dwMode = 8;
		tSummon.nX = anDX[i] + nX;
		tSummon.nY = anDY[i] + nY;
		hWall = SkillsSummon(ptGame, &tSummon);
		if (!hWall)
			continue;
		MonsterUniqueAddMod(ptGame, hWall, 15, FALSE);
		nLife = D2CommonStatsUnitCalcMaxHp(hWall);
		nLife += (int)D2CommonSkillsGetTypeParamOne(SKILL_BONE_PRISON) * nLife * nLevel / 100;
		D2CommonStatsUnitSetValue(hWall, 6, nLife);
		D2CommonStatsUnitSetValue(hWall, 7, nLife);
		TargetsAddToFurtherList(ptGame, hWall, 0, 9);
		if (hFirst)
		{
			AiSetOwner(ptGame, hWall, hFirst->dwId, hFirst->eType, FALSE, FALSE);
			AiAddMinion(ptGame, hFirst, hWall);
		}
		else
		{
			hFirst = hWall;
			AiSetOwner(ptGame, hWall, hWall->dwId, UNIT_MONSTER, FALSE, TRUE);
		}
		dwSpot = D2CommonStepGetSquareDirection((int)D2CommonUnitGetX(hWall), (int)D2CommonUnitGetY(hWall),
			nX, nY);
		D2CommonPathSetFacingImmediate(hWall->ptPath, dwSpot);
	}
	return TRUE;
}

static SKILLCHART sgptSkillChart[NUM_NECROMANCER_SKILLS] =
{
	{ NULL, SkillsAmplifyDamage, SKILLCHART_NOMISSILE },
	{ NULL, SkillsTeeth, SKILLCHART_NOMISSILE },
	{ NULL, SkillsBoneArmor, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ SkillsRaiseStart, SkillsRaise, SKILLCHART_NOMISSILE },
	{ NULL, SkillsDimVision, SKILLCHART_NOMISSILE },
	{ NULL, SkillsWeaken, SKILLCHART_NOMISSILE },
	{ SkillsPoisonDaggerStart, SkillsUseHit, SKILLCHART_NOMISSILE },
	{ SkillsCorpseExplosionStart, SkillsCorpseExplosion, SKILLCHART_NOMISSILE },
	{ NULL, SkillsClayGolem, SKILLCHART_NOMISSILE },
	{ NULL, SkillsIronMaiden, SKILLCHART_NOMISSILE },
	{ NULL, SkillsTerror, SKILLCHART_NOMISSILE },
	{ NULL, SkillsBoneWall, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ SkillsRaiseStart, SkillsRaise, SKILLCHART_NOMISSILE },
	{ NULL, SkillsConfuse, SKILLCHART_NOMISSILE },
	{ NULL, SkillsLifeTap, SKILLCHART_NOMISSILE },
	{ SkillsCorpseExplosionStart, SkillsPoisonExplosion, SKILLCHART_NOMISSILE },
	{ NULL, NULL, MISSILE_BONESPEAR },
	{ NULL, SkillsBloodGolem, SKILLCHART_NOMISSILE },
	{ SkillsAttractStart, SkillsAttract, SKILLCHART_NOMISSILE },
	{ NULL, SkillsDecrepify, SKILLCHART_NOMISSILE },
	{ SkillsBonePrisonStart, SkillsBonePrison, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ SkillsIronGolemStart, SkillsIronGolem, SKILLCHART_NOMISSILE },
	{ NULL, SkillsLowerResist, SKILLCHART_NOMISSILE },
	{ NULL, SkillsPoisonNova, SKILLCHART_NOMISSILE },
	{ NULL, SkillsBoneSpirit, SKILLCHART_NOMISSILE },
	{ NULL, SkillsFireGolem, SKILLCHART_NOMISSILE },
	{ SkillsReviveStart, SkillsRevive, SKILLCHART_NOMISSILE },
};

BOOL __fastcall SkillsNecStart(GAME *ptGame, UNIT *hUnit, int nSkillId  ,
	int nLevel)
{
#line 2157
	D2ASSERT(nSkillId >= SKILL_NECROMANCER_START);
#line 2158
	D2ASSERT(nSkillId - SKILL_NECROMANCER_START < NUM_NECROMANCER_SKILLS);
	if (!SkillsHasMana(hUnit))
		return FALSE;
	if (sgptSkillChart[nSkillId - SKILL_NECROMANCER_START].pfnStart) {
#line 2165
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgptSkillChart[nSkillId - SKILL_NECROMANCER_START].pfnStart), "sgptSkillChart[nSkillId - SKILL_NECROMANCER_START].pfnStart");
		if (sgptSkillChart[nSkillId - SKILL_NECROMANCER_START].pfnStart(ptGame, hUnit, nLevel))
			SkillsUseMana(hUnit);
		else
			return FALSE;
	}
	return TRUE;

}
#line 2171

BOOL __fastcall SkillsNecUse(GAME *ptGame, UNIT *hUnit, int nSkillId,
	int nLevel)
{
	MISSILESTXT *ptMissilesTxt;
	int nNecSkillId;

	nNecSkillId = nSkillId - SKILL_NECROMANCER_START;
#line 2186
	D2ASSERT(nNecSkillId >= 0);
#line 2187
	D2ASSERT(nNecSkillId < NUM_NECROMANCER_SKILLS);
	if (sgptSkillChart[nNecSkillId].pfnUse)
	{
#line 2191
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgptSkillChart[nNecSkillId].pfnUse), "sgptSkillChart[nNecSkillId].pfnUse");
		if (sgptSkillChart[nNecSkillId].pfnUse(ptGame, hUnit, nLevel))
		{
			if (!sgptSkillChart[nNecSkillId].pfnStart)
				SkillsUseMana(hUnit);
		}
		else
			return FALSE;
	}
	if (sgptSkillChart[nNecSkillId].nMissile != SKILLCHART_NOMISSILE)
	{
		ptMissilesTxt = (MISSILESTXT *)D2CommonMissileTblsGetMissile(sgptSkillChart[nNecSkillId].nMissile);
		hUnit->dwFlags0 |= 64;
		SkillsCreateMissile(ptGame, sgptSkillChart[nNecSkillId].nMissile, hUnit, nLevel, 0, 0, 0, 0,
			ptMissilesTxt->Qty);
		if (!sgptSkillChart[nNecSkillId].pfnStart)
			SkillsUseMana(hUnit);
	}
	return TRUE;
}

WORD __fastcall SkillsIronMaidenHit(GAME *ptGame, UNIT *hAttacker, UNIT *hTarget, DAMAGE *ptDamage)
{
	DAMAGE tDamage;
	DWORD hList;
	int nSkillLevel;
	int nPercent;
	int i;

	hList = D2CommonStatsFindListByState(hAttacker, STATE_IRONMAIDEN);
	if (!hList)
		return 0;
	nSkillLevel = (int)D2CommonStatsListGetValueA(hList, 129);
#line 2237
	D2WARN(nSkillLevel);
	nPercent = D2CommonSkillsCalcParamFiveByLevel(SKILL_IRON_MAIDEN, nSkillLevel);
	if (hAttacker->eType == UNIT_PLAYER)
		nPercent /= 10;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = 16385;
	tDamage.dwHitClass = 141;
	tDamage.nPhysicalDamage = ptDamage->nPhysicalDamage * nPercent / 100;
	SUnitDmgApply(ptGame, hTarget, hAttacker, TRUE, &tDamage);
	SUnitDmgReact(ptGame, hTarget, hAttacker, &tDamage);
	return tDamage.wFlags;
}

WORD __fastcall SkillsLifeTapHit(GAME *ptGame, UNIT *hAttacker, UNIT *hTarget, DAMAGE *ptDamage)
{
	DWORD hList;
	int nSkillLevel;
	int nHeal;
	int nLife;
	int nMax;

	hList = D2CommonStatsFindListByState(hTarget, STATE_LIFETAP);
	if (!hList)
		return 0;
	nSkillLevel = (int)D2CommonStatsListGetValueA(hList, 130);
#line 2279
	D2WARN(nSkillLevel);
	nHeal = D2CommonSkillsCalcParamFiveByLevel(SKILL_LIFE_TAP, nSkillLevel);
	nHeal = nHeal * ptDamage->nPhysicalDamage / 100;
	nLife = D2CommonStatsUnitGetValue(hAttacker, 6);
	nMax = D2CommonStatsUnitCalcMaxHp(hAttacker);
	if (nLife + nHeal > nMax)
		nHeal = nMax - nLife;
	D2CommonStatsUnitAddValue(hAttacker, 6, nHeal);
	D2CommonUnitAddAnimOverlay(hAttacker, 151, 0);
	return 0;
}

static inline int SkillsNecAddLife(UNIT *hUnit, int nAmount)
{
	int nLife;
	int nMax;

	if (!nAmount)
		return 0;
	nLife = D2CommonStatsUnitGetValue(hUnit, 6);
	nMax = D2CommonStatsUnitCalcMaxHp(hUnit);
	nLife += nAmount;
	if (nLife > nMax)
	{
		nAmount += nMax - nLife;
		nLife = nMax;
	}
	D2CommonStatsUnitSetValue(hUnit, 6, nLife);
	return nAmount;
}

void __fastcall SkillsBloodGolemLife(UNIT *hGolem, UNIT *hTarget, int nDamage)
{
	UNIT *hOwner;
	int nShare;
	int nPart;
	int nTaken;

	hOwner = AiGetOwner(hGolem);
	if (!hOwner)
		return;
	if (D2CommonStatsUnitGetAlignment(hTarget) == 1)
		return;
	nShare = D2CommonSkillsCalcCurveParamOne(D2CommonSkillsCalcNodeLevel(hOwner,
		D2CommonSkillsFindByKey(hOwner, SKILL_BLOODGOLEM), 1), SKILL_BLOODGOLEM) * nDamage / 100;
	if (!nShare)
		return;
	nPart = SkillsNecAddLife(hOwner, (int)D2CommonSkillsGetTypeParamThree(SKILL_BLOODGOLEM) * nShare / 100);
	nShare -= nPart;
	nTaken = SkillsNecAddLife(hGolem, nShare);
	nShare -= nTaken;
	SkillsNecAddLife(hOwner, nShare);
	D2CommonUnitAddAnimOverlay(hGolem, 151, 0);
	if (nTaken)
		D2CommonUnitAddAnimOverlay(hOwner, 151, 0);
}

int __fastcall SkillsBloodGolemShare(UNIT *hUnit, int nDamage)
{
	UNIT *hOwner;
	int nPart;
	int nLife;

	hOwner = AiGetOwner(hUnit);
	if (!hOwner || !hOwner->dwMode || hOwner->dwMode == 17)
		return nDamage;
	nPart = (int)D2CommonSkillsGetTypeParamFive(SKILL_BLOODGOLEM) * nDamage / 100;
	nLife = D2CommonStatsUnitGetValue(hOwner, 6);
	if (nLife < 256)
		return nDamage;
	if (nLife - nPart < 256)
		nPart = nLife - 256;
	D2CommonStatsUnitSetValue(hOwner, 6, nLife - nPart);
	return nDamage - nPart;
}

int __fastcall SkillsBloodGolemHeal(GAME *ptGame, UNIT *hUnit, int nValue)
{
	UNIT *hGolem;
	int nPart;
	int nLife;
	int nMax;

	hGolem = PlayerPetsFind(ptGame, hUnit, 3);
	if (!hGolem || hGolem->dwClassId != MONSTER_BLOODGOLEM)
		return nValue;
	if (nValue)
	{
		nPart = (int)D2CommonSkillsGetTypeParamSix(SKILL_BLOODGOLEM) * nValue / 100;
		if (!nPart)
			nPart = 0;
		else
		{
			nLife = D2CommonStatsUnitGetValue(hGolem, 6);
			nMax = D2CommonStatsUnitCalcMaxHp(hGolem);
			nLife += nPart;
			if (nLife > nMax)
			{
				nPart += nMax - nLife;
				nLife = nMax;
			}
			D2CommonStatsUnitSetValue(hGolem, 6, nLife);
		}
		nValue -= nPart;
	}
	else
		D2CommonStatsUnitSetValue(hGolem, 6, D2CommonStatsUnitCalcMaxHp(hGolem));
	return nValue;
}
