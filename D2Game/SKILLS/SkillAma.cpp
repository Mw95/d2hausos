#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\SKILLS\\SkillAma.cpp"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "../GAME/SCmd.h"
#include "../GAME/Targets.h"
#include "../ITEMS/ItemMode.h"
#include "../ITEMS/Items.h"
#include "../MISSILES/Missiles.h"
#include "../MONSTER/MonsterEval.h"
#include "../MONSTER/MonsterUnique.h"
#include "../PLAYER/PlrModes.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "SkillAma.h"
#include "Skills.h"
#include "SkillSor.h"

#define SKILL_AMAZON_START		6
#define NUM_AMAZON_SKILLS		30
#define STAT_MINDAMAGE			21
#define STAT_MAXDAMAGE			22
#define STAT_QUANTITY			70
#define STAT_DURABILITY			72
#define MON_DOPPLEZON			356
#define MON_VALKYRIE			357
#define EVENT_RUN_AI			2
#define EVENT_7					7

struct SKILLINITITEM
{
 	DWORD		dwCode;
 	BYTE		bLocation;
 	BYTE		field_05;
 	BYTE		unobserved_06[2];
};
typedef struct SKILLINITITEM SKILLINITITEM;

static SKILLINITITEM sgatValkyrieItems[2] =
{
	{ 544370803, 4, 1, { 0, 0 } },
	{ 543978854, 3, 1, { 0, 0 } },
};

static SKILLCHART sgptSkillChart[NUM_AMAZON_SKILLS] =
{
	{ NULL, NULL, 27 },
	{ SkillsAmaMissileStart, NULL, 12 },
	{ NULL, SkillsInnerSight, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ SkillsJabStart, SkillsJabHit, SKILLCHART_NOMISSILE },
	{ SkillsAmaMissileStart, NULL, 40 },
	{ SkillsMultipleShotStart, SkillsMultipleShot, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ SkillsPowerStrikeStart, SkillsUseHit, SKILLCHART_NOMISSILE },
	{ SkillsAmaMissileStart, NULL, 38 },
	{ SkillsAmaMissileStart, NULL, 41 },
	{ NULL, SkillsSlowMissiles, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ SkillsImpaleStart, SkillsUseHit, SKILLCHART_NOMISSILE },
	{ SkillsAmaMissileStart, NULL, 205 },
	{ SkillsAmaMissileStart, NULL, 28 },
	{ SkillsGuidedArrowStart, SkillsGuidedArrow, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ SkillsChargedStrikeStart, SkillsChargedStrike, SKILLCHART_NOMISSILE },
	{ SkillsAmaMissileStart, NULL, 43 },
	{ SkillsStrafeStart, SkillsStrafe, SKILLCHART_NOMISSILE },
	{ SkillsAmaMissileStart, NULL, 85 },
	{ NULL, SkillsDopplezon, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ SkillsFendStart, SkillsFend, SKILLCHART_NOMISSILE },
	{ SkillsAmaMissileStart, NULL, 87 },
	{ NULL, SkillsValkyrie, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ SkillsLightningStrikeStart, SkillsLightningStrike, SKILLCHART_NOMISSILE },
	{ SkillsAmaMissileStart, NULL, 206 },
};

BOOL __fastcall SkillsInnerSight(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNITFINDARGS tArgs;
	UNITFINDDATA tFind;
	SKILLSTATE tState;
	DWORD hRoom;
	int nX;
	int nY;
	int nRadius;
	int nLength;
	int nValue;
	int i;
	int k;

	hRoom = D2CommonUnitGetPathRoom(hUnit);
	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	nRadius = D2CommonSkillsCalcParamFiveByLevel(SKILL_INNER_SIGHT, nLevel);
	for (i = 0; i < (int)(sizeof(UNITFINDARGS) / sizeof(DWORD)); i++)
		((DWORD *)&tArgs)[i] = 0;
	tArgs.nDistance = nRadius;
	tArgs.nY = nY;
	tArgs.dwFlags = 1411;
	tArgs.hExcluded = hUnit;
	tArgs.nX = nX;
	D2CommonUnitFindsInit(ptGame->pMemPool, &tFind, hRoom, nX, nY, nRadius, 0, &tArgs);
	D2CommonUnitFindsFindUnits(&tFind);
	nLength = D2CommonSkillsCalcParamThreeByLevel(SKILL_INNER_SIGHT, nLevel);
	nValue = D2CommonSkillsCalcCurveParamOne(nLevel, SKILL_INNER_SIGHT);
	for (i = 0; i < tFind.nNumUnits; i++)
	{
		if (!SUnitIsHostile(ptGame, hUnit, tFind.phUnitsArray[i]))
			continue;
		for (k = 0; k < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); k++)
			((DWORD *)&tState)[k] = 0;
		tState.hSource = hUnit;
		tState.hTarget = tFind.phUnitsArray[i];
		tState.nSkill = SKILL_INNER_SIGHT;
		tState.nLevel = nLevel;
		tState.nDuration = nLength;
		tState.dwStatId = 31;
		tState.nStatValue = -nValue;
		tState.dwState = 17;
		SkillsSetState(&tState);
	}
	D2CommonUnitFindsFreeArray(&tFind);
	return TRUE;
}

BOOL __fastcall SkillsJabStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	DWORD eType;
	int i;

	DWORD hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	D2CommonSkillsSetNodeFlags(hSkill, 8);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	eType = hUnit->eType;
	if (eType == UNIT_PLAYER)
		tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget, D2CommonSkillsCalcToHitByLevel(SKILL_JAB,
			nLevel));
	else
		tDamage.wFlags = MonsterEvalHit(ptGame, hUnit, hTarget, nLevel * 5 + 15, 0);
	if (tDamage.wFlags & 1)
	{
		if (eType == UNIT_PLAYER)
			tDamage.dwDamagePercent = D2CommonSkillsCalcParamThreeByLevel(SKILL_JAB, nLevel);
		else
			tDamage.nPhysicalDamage = ((int)D2CommonSeedRand(&hUnit->tSeed, 3) + nLevel) << 8;
	}
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	return TRUE;
}

BOOL __fastcall SkillsJabHit(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	DWORD eType;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	SUnitDmgDeliver(ptGame, hUnit, hTarget);
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	eType = hUnit->eType;
	if (eType == UNIT_PLAYER)
		tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget, D2CommonSkillsCalcToHitByLevel(SKILL_JAB,
			nLevel));
	else
		tDamage.wFlags = MonsterEvalHit(ptGame, hUnit, hTarget, nLevel * 5 + 15, 0);
	if (tDamage.wFlags & 1)
	{
		if (eType == UNIT_PLAYER)
			tDamage.dwDamagePercent = D2CommonSkillsCalcParamThreeByLevel(SKILL_JAB, nLevel);
		else
			tDamage.nPhysicalDamage = ((int)D2CommonSeedRand(&hUnit->tSeed, 3) + nLevel) << 8;
	}
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	return TRUE;
}

BOOL __fastcall SkillsMultipleShotStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	if (!D2CommonCOMPOSITIsRangedAttack(hUnit))
		return FALSE;
	if (!(SkillsHasThrowQuantity(hUnit))) {
		PlrModeWearWeapons(ptGame, hUnit);
		PlrModeBreakItems(ptGame, hUnit);
		return FALSE; }
	return TRUE;
}

BOOL __fastcall SkillsMultipleShot(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreate;
	int nCount;
	int nX;
	int nY;
	int nDx;
	int nDy;
	int nMissile;
	int i;

	hUnit->dwFlags0 |= 64;
	nCount = D2CommonSkillsCalcParamOneByLevel(SKILL_MULTIPLE_SHOT, nLevel);
	if (!SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY))
		return TRUE;
	nDx = nX - (int)D2CommonUnitGetX(hUnit);
	nDy = nY - (int)D2CommonUnitGetY(hUnit);
	SkillsStepLengthen(&nDx, &nDy);
	SkillsStepTurn(&nDx, &nDy);
	nX += -(nDx * nCount / 2);
	nY += -(nDy * nCount / 2);
	nMissile = (D2CommonUnitGetItemFlagsWord(hUnit) != 1) + 213;
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = 2080;
	tCreate.hSource = hUnit;
	tCreate.hSpawnLocation = hUnit;
	tCreate.nLevel = nLevel;
	tCreate.eMissile = nMissile;
	tCreate.nActivate = (int)D2CommonSkillsGetTypeParamThree(SKILL_MULTIPLE_SHOT);
	for (i = 0; i < nCount; i++)
	{
		tCreate.nDX = nX;
		tCreate.nDY = nY;
		MissilesCreate(ptGame, &tCreate);
		nX += nDx;
		nY += nDy;
	}
	if (nMissile != 27)
		SkillsUseThrowQuantity(ptGame, hUnit);
	return TRUE;
}

BOOL __fastcall SkillsPowerStrikeStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	DWORD dwSkill;
	int i;

	dwSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	D2CommonSkillsSetNodeFlags(dwSkill, 8);
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget, 0);
	if (tDamage.wFlags & 1)
		SkillsElemDamage(hUnit, &tDamage, SKILL_POWER_STRIKE, nLevel);
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	return TRUE;
}

BOOL __fastcall SkillsSlowMissiles(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNITFINDARGS tArgs;
	UNITFINDDATA tFind;
	SKILLSTATE tState;
	DWORD hRoom;
	int nX;
	int nY;
	int nRadius;
	int nLength;
	int nValue;
	int i;
	int k;

	hRoom = D2CommonUnitGetPathRoom(hUnit);
	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	nRadius = D2CommonSkillsCalcParamFiveByLevel(SKILL_SLOW_MISSILES, nLevel);
	for (i = 0; i < (int)(sizeof(UNITFINDARGS) / sizeof(DWORD)); i++)
		((DWORD *)&tArgs)[i] = 0;
	tArgs.nDistance = nRadius;
	tArgs.nY = nY;
	tArgs.dwFlags = 1411;
	tArgs.hExcluded = hUnit;
	tArgs.nX = nX;
	D2CommonUnitFindsInit(ptGame->pMemPool, &tFind, hRoom, nX, nY, nRadius, 0, &tArgs);
	D2CommonUnitFindsFindUnits(&tFind);
	nLength = D2CommonSkillsCalcParamThreeByLevel(SKILL_SLOW_MISSILES, nLevel);
	nValue = D2CommonSkillsCalcParamOneByLevel(SKILL_SLOW_MISSILES, nLevel);
	for (i = 0; i < tFind.nNumUnits; i++)
	{
		if (!SUnitIsHostile(ptGame, hUnit, tFind.phUnitsArray[i]))
			continue;
		if (D2CommonMonsterUnitTestBoss(tFind.phUnitsArray[i]))
			continue;
		for (k = 0; k < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); k++)
			((DWORD *)&tState)[k] = 0;
		tState.hSource = hUnit;
		tState.hTarget = tFind.phUnitsArray[i];
		tState.nSkill = SKILL_SLOW_MISSILES;
		tState.nLevel = nLevel;
		tState.nDuration = nLength;
		tState.dwStatId = 161;
		tState.nStatValue = nValue;
		tState.dwState = 87;
		SkillsSetState(&tState);
	}
	D2CommonUnitFindsFreeArray(&tFind);
	return TRUE;
}

BOOL __fastcall SkillsImpaleStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	ITEMSTXT *ptItemsTxt;
	UNIT *hTarget;
	UNIT *hWeapon;
	DWORD dwSkill;
	int nPercent;
	int nMin;
	int nMax;
	int nLoss;
	int nChance;
	int nLeft;
	int nQty;
	int i;

	dwSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget || !SUnitIsHostile(ptGame, hUnit, hTarget))
		return FALSE;
	D2CommonSkillsSetNodeFlags(dwSkill, 8);
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget, D2CommonSkillsCalcToHitByLevel(SKILL_IMPALE,
		nLevel));
	if (tDamage.wFlags & 1)
	{
		tDamage.nPhysicalDamage = SUnitDmgRollDamage(hUnit, (D2CommonStatsUnitGetValue(hUnit, STAT_MINDAMAGE) << 8), (D2CommonStatsUnitGetValue(hUnit, STAT_MAXDAMAGE) << 8), D2CommonSkillsCalcParamOneByLevel(SKILL_IMPALE, nLevel), 0);
		tDamage.dwFlags0 = 1;
		hWeapon = (UNIT *)D2CommonUnitGetActiveWeapon(hUnit);
		if (hWeapon)
		{
			nLoss = (int)D2CommonSkillsGetTypeParamFour(SKILL_IMPALE);
			nChance = (int)D2CommonSkillsGetTypeParamThree(SKILL_IMPALE);
			if (D2CommonITEMSIsWeapon(hWeapon))
			{
				ptItemsTxt = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hWeapon->dwClassId);
				if (ptItemsTxt)
				{
					if (!ptItemsTxt->nodurability)
					{
						if ((int)D2CommonSeedRand(&hUnit->tSeed, 100) < nChance)
						{
							nLeft = D2CommonStatsUnitGetValue(hWeapon, STAT_DURABILITY) - nLoss;
#line 364
							D2ASSERTMSG(nLoss > 0, "Durability loss for Impale should never be less "
								"than zero!\nTell Ted Bisson!");
							if (nLeft < 0 || nLeft == 0)
								ITEMSModeBreakItem(ptGame, hUnit, hWeapon);
							else
							{
								D2CommonStatsUnitSetValue(hWeapon, STAT_DURABILITY, nLeft);
								SCmdSendItemStat(SUnitGetClient(hUnit), hWeapon, 1, STAT_DURABILITY, nLeft);
							}
						}
					}
					else
					{
						nQty = D2CommonStatsUnitGetValue(hWeapon, STAT_QUANTITY);
#line 379
						D2ASSERT(nQty >= 0);
						if (nQty && (int)D2CommonSeedRand(&hUnit->tSeed, 100) < nChance)
							SkillsUseThrowQuantity(ptGame, hUnit);
					}
				}
			}
		}
	}
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	return TRUE;
}

BOOL __fastcall SkillsGuidedArrowStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	return SkillsUseThrowQuantity(ptGame, hUnit) > 0;
}

BOOL __fastcall SkillsGuidedArrow(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreate;
	UNIT *hTarget;
	UNIT *hMissile;
	int nX;
	int nY;
	int nDx;
	int nDy;
	int i;

	hUnit->dwFlags0 |= 64;
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY))
		return FALSE;
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = 32;
	tCreate.hSource = hUnit;
	tCreate.hSpawnLocation = hUnit;
	tCreate.hTarget = hTarget;
	tCreate.nDX = nX;
	tCreate.nDY = nY;
	tCreate.eMissile = 86;
	if (!hTarget)
		tCreate.dwFlags = 1056;
	hMissile = MissilesCreate(ptGame, &tCreate);
	if (hMissile)
	{
		if (hTarget)
			D2CommonMissileSetScratchFirst(hMissile, 1);
		else
		{
			D2CommonMissileSetScratchFirst(hMissile, 2);
			nDx = nX - (int)D2CommonUnitGetX(hUnit);
			nDy = nY - (int)D2CommonUnitGetY(hUnit);
			D2CommonMissileSetScratchSecond(hMissile, ((int)(short)nDy << 16) + (short)nDx);
		}
	}
	return TRUE;
}

BOOL __fastcall SkillsChargedStrikeStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	DWORD dwSkill;
	int i;

	dwSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	D2CommonSkillsSetNodeFlags(dwSkill, 8);
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget, 0);
	if (tDamage.wFlags & 1)
		SkillsElemDamage(hUnit, &tDamage, SKILL_CHARGED_STRIKE, nLevel);
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	return TRUE;
}

BOOL __fastcall SkillsChargedStrike(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreate;
	UNIT *hTarget;
	int nCount;
	int nX;
	int nY;
	int nTargetX;
	int nTargetY;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	hUnit->dwFlags0 |= 64;
	if (!hTarget)
		return FALSE;
	SUnitDmgDeliver(ptGame, hUnit, hTarget);
	nCount = D2CommonSkillsCalcParamOneByLevel(SKILL_CHARGED_STRIKE, nLevel);
	nX = (int)D2CommonUnitGetX(hTarget);
	nY = (int)D2CommonUnitGetY(hTarget);
	nTargetX = nX * 2 - (int)D2CommonUnitGetX(hUnit);
	nTargetY = nY * 2 - (int)D2CommonUnitGetY(hUnit);
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.hSource = hUnit;
	tCreate.nDY = nTargetY;
	tCreate.nX = nX;
	tCreate.dwFlags = 33;
	tCreate.nY = nY;
	tCreate.nDX = nTargetX;
	tCreate.nLevel = nLevel;
	tCreate.eMissile = 215;
	tCreate.pfnModify = SkillsChargedBoltModify;
	for (i = 0; i < nCount; i++)
	{
		tCreate.dwModifyArg = (DWORD)i;
		MissilesCreate(ptGame, &tCreate);
	}
	return TRUE;
}

BOOL __fastcall SkillsStrafeStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;
	DWORD dwSkill;
	DWORD dwSkillId;
	int nCount;
	int nMax;

	dwSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!D2CommonCOMPOSITIsRangedAttack(hUnit))
		goto lbFalse;
	if (!SkillsHasThrowQuantity(hUnit))
	{
		PlrModeWearWeapons(ptGame, hUnit);
		PlrModeBreakItems(ptGame, hUnit);
	lbFalse:
		return FALSE;
	}
	if (!dwSkill)
		return FALSE;
	dwSkillId = D2CommonSkillsGetNodeType(dwSkill);
	if (!hTarget)
		hTarget = SkillsFindNext(ptGame, hUnit, (DWORD)-1, D2CommonSkillsGetTypeParamFive(dwSkillId), 0);
	D2CommonUnitGetItemFlagsWord(hUnit);
	if (SkillsUseThrowQuantity(ptGame, hUnit) <= 0)
		return FALSE;
	nCount = SkillsCountTargets(ptGame, hUnit, D2CommonSkillsGetTypeParamFive(dwSkillId));
	nMax = D2CommonSkillsCalcParamThreeByLevel(dwSkillId, nLevel);
	if (nCount >= nMax)
		nCount = nMax;
	if (!nCount)
		nCount = 1;
	D2CommonSkillsSetNodeScratchC(dwSkill, nCount);
	if (hTarget)
	{
		D2CommonSkillsSetNodeScratchD(dwSkill, hTarget->eType);
		D2CommonSkillsSetNodeScratchE(dwSkill, hTarget->dwId);
		return TRUE;
	}
	D2CommonSkillsSetNodeScratchD(dwSkill, 1);
	D2CommonSkillsSetNodeScratchE(dwSkill, -1);
	return TRUE;
}

BOOL __fastcall SkillsStrafe(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreate;
	UNIT *hTarget;
	UNIT *hNext;
	DWORD dwSkill;
	DWORD dwSkillId;
	DWORD dwId;
	int nLeft;
	int nTargetX;
	int nTargetY;
	int nMissile;
	int nPercent;
	int i;

	dwSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	if (!dwSkill)
		return FALSE;
	dwSkillId = D2CommonSkillsGetNodeType(dwSkill);
	nLeft = (int)D2CommonSkillsGetNodeScratchC(dwSkill);
	dwId = D2CommonSkillsGetNodeScratchE(dwSkill);
	hTarget = SUnitFind(ptGame, D2CommonSkillsGetNodeScratchD(dwSkill), dwId);
	if (!hTarget)
		hTarget = SkillsFindNext(ptGame, hUnit, (DWORD)-1, D2CommonSkillsGetTypeParamFive(dwSkillId), 0);
	if (hTarget)
		D2CommonUnitSetTarget(hUnit, hTarget);
	nTargetX = 0;
	nTargetY = 0;
	SkillsGetTargetSpot(ptGame, hUnit, &nTargetX, &nTargetY);
	nMissile = (D2CommonUnitGetItemFlagsWord(hUnit) != 1) + 213;
	nPercent = D2CommonSkillsCalcParamOneByLevel(dwSkillId, nLevel) + 100;
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = 33;
	tCreate.hSource = hUnit;
	tCreate.nX = (int)D2CommonUnitGetX(hUnit);
	tCreate.nY = (int)D2CommonUnitGetY(hUnit);
	tCreate.eMissile = nMissile;
	tCreate.pfnModify = SkillsStrafeModify;
	tCreate.nLevel = nLevel;
	tCreate.nDX = nTargetX;
	tCreate.nDY = nTargetY;
	tCreate.dwModifyArg = (DWORD)nPercent;
	MissilesCreate(ptGame, &tCreate);
	nLeft--;
	if (!nLeft)
		return FALSE;
	D2CommonSkillsSetNodeScratchC(dwSkill, nLeft);
	if (hTarget)
		hNext = SkillsFindNext(ptGame, hUnit, hTarget->dwId, D2CommonSkillsGetTypeParamFive(dwSkillId),
			0);
	else
		hNext = SkillsFindNext(ptGame, hUnit, (DWORD)-1, D2CommonSkillsGetTypeParamFive(dwSkillId), 0);
	if (hNext)
	{
		D2CommonSkillsSetNodeScratchD(dwSkill, hNext->eType);
		D2CommonSkillsSetNodeScratchE(dwSkill, hNext->dwId);
		SkillsSlowAnim(hUnit, (int)D2CommonSkillsGetTypeParamSix(dwSkillId));
	}
	return FALSE;
}

void __fastcall SkillsStrafeModify(UNIT *hMissile, DWORD dwPercent)
{
	D2CommonStatsUnitSetValue(hMissile, STAT_MINDAMAGE,
		(DWORD)D2CommonStatsUnitGetValue(hMissile, STAT_MINDAMAGE) * dwPercent / 100);
	D2CommonStatsUnitSetValue(hMissile, STAT_MAXDAMAGE,
		(DWORD)D2CommonStatsUnitGetValue(hMissile, STAT_MAXDAMAGE) * dwPercent / 100);
}

BOOL __fastcall SkillsFendStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;
	DWORD dwSkill;

	dwSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!dwSkill)
		return FALSE;
	if (!hTarget)
	{
		hTarget = SkillsFindNext(ptGame, hUnit, (DWORD)-1, 4, 4);
		if (!hTarget)
		{
			D2CommonSkillsSetNodeScratchC(dwSkill, 0);
			return FALSE;
		}
	}
	DWORD nTargets = SkillsCountTargets(ptGame, hUnit, 4);
	D2CommonSkillsSetNodeScratchC(dwSkill, nTargets);
	D2CommonSkillsSetNodeScratchD(dwSkill, hTarget->eType);
	D2CommonSkillsSetNodeScratchE(dwSkill, hTarget->dwId);
	return TRUE;
}

BOOL __fastcall SkillsFend(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	UNIT *hNext;
	DWORD dwSkill;
	DWORD dwId;
	int nLeft;
	int i;

	dwSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	dwId = D2CommonSkillsGetNodeScratchE(dwSkill);
	hTarget = SUnitFind(ptGame, D2CommonSkillsGetNodeScratchD(dwSkill), dwId);
	if (!hTarget)
	{
		hTarget = SkillsFindNext(ptGame, hUnit, D2CommonSkillsGetNodeScratchE(dwSkill), 4, 4);
		if (!hTarget)
			return FALSE;
	}
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget, D2CommonSkillsCalcToHitByLevel(SKILL_FEND, nLevel));
	tDamage.dwDamagePercent = D2CommonSkillsCalcParamThreeByLevel(SKILL_FEND, nLevel);
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	SUnitDmgDeliver(ptGame, hUnit, hTarget);
	nLeft = (int)D2CommonSkillsGetNodeScratchC(dwSkill);
	if (nLeft > 0)
	{
		D2CommonSkillsSetNodeScratchC(dwSkill, nLeft - 1);
		hNext = SkillsFindNext(ptGame, hUnit, hTarget->dwId, 4, 4);
		if (hNext)
		{
			D2CommonSkillsSetNodeScratchD(dwSkill, hNext->eType);
			D2CommonSkillsSetNodeScratchE(dwSkill, hNext->dwId);
			SkillsSlowAnim(hUnit, (int)D2CommonSkillsGetTypeParamTwo(D2CommonSkillsGetNodeType(dwSkill)));
		}
	}
	return FALSE;
}

BOOL __fastcall SkillsLightningStrikeStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	SKILLSTXT *ptSkillsTxt;
	UNIT *hTarget;
	int nMin;
	int nRange;
	int nRoll;
	int i;

	ptSkillsTxt = (SKILLSTXT *)D2CommonDataTblsGetSkill(SKILL_LIGHTNING_STRIKE);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget, 0);
	nMin = D2CommonSkillsCalcEMinByLevel(SKILL_LIGHTNING_STRIKE, nLevel);
	nRange = D2CommonSkillsCalcEMaxByLevel(SKILL_LIGHTNING_STRIKE, nLevel) - nMin;
	if (nRange)
		nRoll = (int)D2CommonSeedRand(&hUnit->tSeed, nRange);
	else
		nRoll = 0;
	tDamage.nLightningDamage = nRoll + nMin;
	tDamage.nLightningDamage <<= ptSkillsTxt->HitShift;
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	return TRUE;
}

BOOL __fastcall SkillsLightningStrike(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLFINDTARGET tFind;
	MISSILECREATE tCreate;
	UNIT *hTarget;
	UNIT *hNext;
	UNIT *hMissile;
	DWORD dwValue;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	SUnitDmgDeliver(ptGame, hUnit, hTarget);
	for (i = 0; i < (int)(sizeof(SKILLFINDTARGET) / sizeof(DWORD)); i++)
		((DWORD *)&tFind)[i] = 0;
	tFind.dwFlags = 3;
	tFind.hUnit = hUnit;
	tFind.dwUnitId = hTarget->dwId;
	tFind.nX = (int)D2CommonUnitGetX(hTarget);
	tFind.nY = (int)D2CommonUnitGetY(hTarget);
	tFind.nRange = (int)D2CommonSkillsGetTypeParamOne(SKILL_LIGHTNING_STRIKE);
	hNext = SkillsFindTarget(ptGame, &tFind);
	if (!hNext)
		return TRUE;
	dwValue = D2CommonSkillsCalcParamThreeByLevel(SKILL_LIGHTNING_STRIKE, nLevel);
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = 32;
	tCreate.hSource = hUnit;
	tCreate.hSpawnLocation = hTarget;
	tCreate.nLevel = nLevel;
	tCreate.eMissile = 232;
	tCreate.nDX = (int)D2CommonUnitGetX(hNext);
	tCreate.nDY = (int)D2CommonUnitGetY(hNext);
	hMissile = MissilesCreate(ptGame, &tCreate);
	if (hMissile)
		D2CommonMissileSetScratchFirst(hMissile, dwValue);
	return TRUE;
}

BOOL __fastcall SkillsDopplezon(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLSUMMON tSummon;
	UNIT *hPet;
	DWORD dwLife;
	int nLength;
	int i;

	hUnit->dwFlags0 |= 64;
	for (i = 0; i < (int)(sizeof(SKILLSUMMON) / sizeof(DWORD)); i++)
		((DWORD *)&tSummon)[i] = 0;
	tSummon.hOwner = hUnit;
	tSummon.dwClassId = MON_DOPPLEZON;
	tSummon.dwAiKind = 0;
	tSummon.dwMode = 1;
	tSummon.dwPetType = 8;
	hPet = SkillsSummon(ptGame, &tSummon);
	if (!hPet)
		return FALSE;
	D2CommonUnitSetLinkedUnit(hPet, hUnit);
	D2CommonStatsUnitSetValue(hPet, 74, 0);
	dwLife = D2CommonStatsUnitCalcMaxHp(hUnit);
	D2CommonStatsUnitSetValue(hPet, 6, dwLife);
	D2CommonStatsUnitSetValue(hPet, 7, dwLife);
	D2CommonStatsUnitSetActiveStateBit(hPet, 63, 1);
	nLength = D2CommonSkillsCalcParamOneByLevel(SKILL_DOPPLEZON, nLevel);
	EventAdd(ptGame, hPet, EVENT_7, ptGame->nFrame + nLength);
	MonsterUniqueAddMod(ptGame, hPet, 21, FALSE);
	D2CommonUnitAddAnimOverlay(hPet, 171, 0);
	TargetsAddToList(ptGame, hPet, 0, hUnit->nTargetListNo);
	return TRUE;
}

static __inline void *SkillsValkyrieFindItem(const SKILLINITITEM *ptInitItem, DWORD *pdwClass)
{
	if (!ptInitItem)
		return NULL;
	return (void *)D2CommonItemTblsFindItemByCode(ptInitItem->dwCode, pdwClass);
}

BOOL __fastcall SkillsValkyrie(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLSUMMON tSummon;
	const SKILLINITITEM *ptInitItem;
	UNIT *hPet;
	UNIT *hItem;
	DWORD dwStats;
	DWORD dwClass;
	int nResult;
	int nCount;
	int nLife;
	BOOL fRet;
	int i;

	hUnit->dwFlags0 |= 64;
	for (i = 0; i < (int)(sizeof(SKILLSUMMON) / sizeof(DWORD)); i++)
		((DWORD *)&tSummon)[i] = 0;
	tSummon.hOwner = hUnit;
	tSummon.dwClassId = MON_VALKYRIE;
	tSummon.dwAiKind = 7;
	tSummon.dwMode = 1;
	tSummon.dwPetType = 2;
	hPet = SkillsSummon(ptGame, &tSummon);
	if (!hPet)
		return FALSE;
	if (nLevel > 1)
	{
		nCount = nLevel - 1;
		dwStats = D2CommonStatsAllocList(ptGame->pMemPool, 0, 0, hPet->eType, hPet->dwId);
		if (dwStats)
		{
			D2CommonStatsUnitLinkList(hPet, dwStats);
			D2CommonStatsListSetValueA(dwStats, 171,
				(int)D2CommonSkillsGetTypeParamThree(SKILL_VALKYRIE) * nCount);
			D2CommonStatsListSetValueA(dwStats, 25,
				(int)D2CommonSkillsGetTypeParamTwo(SKILL_VALKYRIE) * nCount);
			D2CommonStatsListSetValueA(dwStats, 119,
				(int)D2CommonSkillsGetTypeParamFour(SKILL_VALKYRIE) * nCount);
		}
		nLife = D2CommonStatsUnitCalcMaxHp(hPet);
		nLife += (int)D2CommonSkillsGetTypeParamOne(SKILL_VALKYRIE) * nLife * nCount / 100;
		D2CommonStatsUnitSetValue(hPet, 7, nLife);
		D2CommonStatsUnitSetValue(hPet, 6, nLife);
	}
	for (i = 0; i < 2; i++)
	{
		ptInitItem = &sgatValkyrieItems[i];
#line 996
		D2ASSERT(ptInitItem);
		if (!SkillsValkyrieFindItem(ptInitItem, &dwClass))
			continue;
		hItem = ITEMSCreate(hPet, dwClass, ptGame, 4, 4, 1, D2CommonSkillsCalcParamFiveByLevel(SKILL_VALKYRIE,
			nLevel), FALSE, 0, 0);
		D2CommonITEMSDataSetFlags(hItem, 4096, 1);
		D2CommonITEMSSetCategory(hItem, ptInitItem->bLocation);
		if (!ptInitItem->bLocation)
			continue;
		fRet = ITEMSModeEquipCursorItem(ptGame, hPet, hItem->dwId,
			D2CommonITEMSGetCategoryRaw((DWORD)hItem), TRUE, &nResult);
#line 1012
		D2ASSERT(fRet);
		D2CommonStatsUnitSetValue(hItem, STAT_DURABILITY, D2CommonStatsUnitCalcMaxDurability(hItem));
	}
	EventRemoveByType(ptGame, hPet, EVENT_RUN_AI);
	EventAdd(ptGame, hPet, EVENT_RUN_AI, ptGame->nFrame + 20);
	D2CommonStatsUnitSetActiveStateBit(hPet, 93, 1);
	D2CommonUnitSetLinkedUnit(hPet, hUnit);
	TargetsAddToList(ptGame, hPet, 0, hUnit->nTargetListNo);
	return TRUE;
}

BOOL __fastcall SkillsAmaMissileStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{

	if (!(SkillsHasThrowQuantity(hUnit))) {
		GAME *ptUnusedGame = ptGame;
		PlrModeWearWeapons(ptGame, hUnit);
		PlrModeBreakItems(ptGame, hUnit);
		return FALSE; }
	return TRUE;
}
#line 1035

BOOL __fastcall SkillsAmaStart(GAME *ptGame, UNIT *hUnit, int nSkillId  ,
	int nLevel)
{
#line 1188
	D2ASSERT(nSkillId >= SKILL_AMAZON_START);
#line 1189
	D2ASSERT(nSkillId - SKILL_AMAZON_START < NUM_AMAZON_SKILLS);
	if (!SkillsHasMana(hUnit))
		return FALSE;
	if (sgptSkillChart[nSkillId - SKILL_AMAZON_START].pfnStart) {
#line 1195
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgptSkillChart[nSkillId - SKILL_AMAZON_START].pfnStart),
			"sgptSkillChart[nSkillId - SKILL_AMAZON_START].pfnStart");
		if (sgptSkillChart[nSkillId - SKILL_AMAZON_START].pfnStart(ptGame, hUnit, nLevel))
			SkillsUseMana(hUnit);
		else
			return FALSE;
	}
	return TRUE;
}
#line 1203

BOOL __fastcall SkillsAmaUse(GAME *ptGame, UNIT *hUnit, int nSkillId,
	int nLevel)
{
	MISSILESTXT *ptMissilesTxt;
	int nAmaSkillId;

	nAmaSkillId = nSkillId - SKILL_AMAZON_START;
#line 1217
	D2ASSERT(nAmaSkillId >= 0);
#line 1218
	D2ASSERT(nAmaSkillId < NUM_AMAZON_SKILLS);
	if (sgptSkillChart[nAmaSkillId].pfnUse)
	{
#line 1221
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgptSkillChart[nAmaSkillId].pfnUse),
			"sgptSkillChart[nAmaSkillId].pfnUse");
		if (sgptSkillChart[nAmaSkillId].pfnUse(ptGame, hUnit, nLevel)) {
			if (!sgptSkillChart[nAmaSkillId].pfnStart)
				SkillsUseMana(hUnit);
		} else
			return FALSE;
	}
	if (sgptSkillChart[nAmaSkillId].nMissile != SKILLCHART_NOMISSILE)
	{
		ptMissilesTxt = (MISSILESTXT *)D2CommonMissileTblsGetMissile(sgptSkillChart[nAmaSkillId].nMissile);
		hUnit->dwFlags0 |= 64;
		SkillsCreateMissile(ptGame, sgptSkillChart[nAmaSkillId].nMissile, hUnit, nLevel, 0, 0, 0, 0,
			ptMissilesTxt->Qty);
		if (!sgptSkillChart[nAmaSkillId].pfnStart)
			SkillsUseMana(hUnit);
	}
	return TRUE;
}
