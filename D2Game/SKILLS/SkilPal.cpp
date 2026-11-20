#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\SKILLS\\SkilPal.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../AI/AiGeneral.h"
#include "../GAME/Event.h"
#include "../GAME/Targets.h"
#include "../MISSILES/Missiles.h"
#include "../MISSILES/MissMode.h"
#include "../MONSTER/MonsterEval.h"
#include "../MONSTER/MonsterMode.h"
#include "../UNIT/PlayerList.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "SkillAma.h"
#include "Skills.h"
#include "SkilPal.h"

#define SKILL_PALADIN_START		96
#define NUM_PALADIN_SKILLS		30
#define MONSTER_49				73
#define MONSTER_D3				211
#define MONSTER_123				291
#define STATE_RESISTFIRE		3
#define STATE_RESISTCOLD		4
#define STATE_RESISTLIGHTNING	5
#define STATE_SALVATION			8
#define STATE_CHARGE			18
#define STATE_HOLYFIRE			35
#define STATE_THORNS			36
#define STATE_DEFIANCE			37
#define STATE_HOLYFREEZE		43
#define STATE_HOLYFROZEN		44
#define STATE_CLEANSING			45
#define STATE_HOLYSHOCK			46
#define STATE_SANCTUARY			47
#define STATE_REDEMPTION		50
#define STATE_CONVERSION		53
#define STATE_36				54
#define STATE_REDEEMED			99
#define STATE_6B				107
#define STAT_THORNS				131
#define AURA_SIGHTFLAGS			2053
#define EVENT_STATEEND			12
#define MISSFLAG_POSITION		1
#define MISSFLAG_TARGETABS		32
#define UNIT_NONE				6
#define STATE_CONVICTION		28
#define STATE_CONVICTED			29
#define STATE_MIGHT				33
#define STATE_PRAYER			34
#define STATE_BLESSEDAIM		40
#define STATE_VIGOR				41
#define STATE_CONCENTRATION		42
#define STATE_MEDITATION		48
#define STATE_FANATICISM		49
#define STATE_55				85
#define STATE_HOLYSHIELD		101
#define AURA_FRAMES				200
#define AURA_FINDFLAGS			8195
#define UNIT_PLAYER				0
#define UNIT_MONSTER			1

static __inline void *SkillsAuraSetState(UNIT *hSource, UNIT *hTarget, int nSkill, int nLevel, DWORD dwStatId,
	int nStatValue, DWORD dwState, DWORD pfnRemove)
{
	SKILLSTATE tData;

	memset(&tData, 0, sizeof(tData));
	tData.hSource = hSource;
	tData.hTarget = hTarget;
	tData.nSkill = nSkill;
	tData.nLevel = nLevel;
	tData.nDuration = AURA_FRAMES;
	tData.dwStatId = dwStatId;
	tData.nStatValue = nStatValue;
	tData.dwState = dwState;
	tData.pfnRemove = pfnRemove;
	return SkillsSetState(&tData);
}

static __inline void SkillsDamageAuraPulse(GAME *ptGame, UNIT *hUnit, int nLevel, int nSkill, DWORD dwState,
	DWORD dwFindFlags, PFNAREAAFFECT pfnAffect)
{
	SkillsAuraSetState(hUnit, hUnit, nSkill, nLevel, 0xFFFFFFFF, 0, dwState, 0);
	if (!D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hUnit)))
		SkillsAreaEffectAt(ptGame, dwFindFlags, (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hUnit,
			nSkill, nLevel, pfnAffect);
}

BOOL __fastcall SkillsSacrificeStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	DWORD hSkill;
	int nPercent;
	int i;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	D2CommonSkillsSetNodeFlags(hSkill, 8);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget, D2CommonSkillsCalcToHitByLevel(SKILL_PALADIN_START,
		nLevel));
	if (tDamage.wFlags & 1)
	{
		nPercent = D2CommonSkillsCalcParamOneByLevel(SKILL_PALADIN_START, nLevel);
		tDamage.nPhysicalDamage = SUnitDmgRollDamage(hUnit, D2CommonStatsUnitGetValue(hUnit, 21) << 8,
			D2CommonStatsUnitGetValue(hUnit, 22) << 8, nPercent, 0);
		tDamage.dwFlags0 = 1;
	}
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	return TRUE;
}

BOOL __fastcall SkillsSacrifice(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	DAMAGE *ptHeld;
	UNIT *hTarget;
	int nDamage;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	ptHeld = SUnitDmgFindHeld(hUnit, hTarget);
	if (!ptHeld)
		return TRUE;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.dwFlags0 |= 4096;
	nDamage = (int)D2CommonSkillsGetTypeParamThree(SKILL_PALADIN_START) * ptHeld->nPhysicalDamage / 100;
	tDamage.nPhysicalDamage = nDamage;
	tDamage.nTotalDamage = nDamage;
	SUnitDmgApply(ptGame, hUnit, hUnit, FALSE, &tDamage);
	SUnitDmgReact(ptGame, hUnit, hUnit, &tDamage);
	SUnitDmgDeliver(ptGame, hUnit, hTarget);
	return TRUE;
}

BOOL __fastcall SkillsSmiteStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	ITEMSTXT *ptItemsTxt;
	UNIT *hTarget;
	UNIT *hShield;
	DWORD eType;
	DWORD dwSkill;
	DWORD ptHolyShield;
	WORD wFlags;
	int nHolyShield;
	int nMin;
	int nMax;
	int nPercent;
	int i;

	eType = hUnit->eType;
	dwSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	if (!D2CommonUnitTestPositionClear(hUnit, hTarget, 0))
		return FALSE;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	if (eType == UNIT_PLAYER)
	{
		if (!D2CommonINVENTORYFindShieldInHands(hUnit->ptInventory, &hShield))
			return FALSE;
		ptItemsTxt = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hShield->dwClassId);
		nMin = ptItemsTxt->mindam;
		nMax = ptItemsTxt->maxdam;
		ptHolyShield = D2CommonSkillsFindByKey(hUnit, SKILL_HOLY_SHIELD);
		if (ptHolyShield && D2CommonStatsGetState(hUnit, STATE_HOLYSHIELD))
		{
			nHolyShield = (int)D2CommonSkillsCalcNodeLevel(hUnit, ptHolyShield, 1);
			nMin += D2CommonSkillsCalcValueA(SKILL_HOLY_SHIELD, hUnit, nHolyShield);
			nMax += D2CommonSkillsCalcValueB(SKILL_HOLY_SHIELD, hUnit, nHolyShield);
		}

		tDamage.nPhysicalDamage = SUnitDmgRollDamage(hUnit, nMin << 8, nMax << 8, D2CommonSkillsCalcParamThreeByLevel(SKILL_SMITE, nLevel), 0);
		tDamage.nStunLength = D2CommonSkillsCalcParamOneByLevel(SKILL_SMITE, nLevel);
	}
	else
	{
		tDamage.nPhysicalDamage = (((int)D2CommonSeedRand(&hUnit->tSeed, 4) + 1) * nLevel) << 8;
		tDamage.nStunLength = 25;
	}
	wFlags = SUnitDmgHit(ptGame, hUnit, hTarget, 0) | 9;
	tDamage.dwFlags0 = 2;
	tDamage.wFlags = wFlags;
	tDamage.dwHitClass = 101;
	D2CommonSkillsSetNodeFlags(dwSkill, 8);
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	return TRUE;
}

BOOL __fastcall SkillsPrayer(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLEFFECT tEffect;
	int nHealed;
	int nCost;
	int nMana;
	int nMaxLife;
	int nLife;
	int i;

	nHealed = 0;
	nCost = D2CommonSkillsCalcManaCost(SKILL_PRAYER, nLevel);
	nMana = D2CommonStatsUnitGetValue(hUnit, 8);
	SkillsAuraSetState(hUnit, hUnit, SKILL_PRAYER, nLevel, 0xFFFFFFFF, 0, STATE_PRAYER, (DWORD)SkillsPrayerEnd);
	if (nCost > nMana)
	{
		D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_55, 0);
		return FALSE;
	}
	if (SUnitIsAlly(ptGame, hUnit, hUnit))
	{
		nMaxLife = D2CommonStatsUnitCalcMaxHp(hUnit);
		nLife = D2CommonStatsUnitGetValue(hUnit, 6);
		if (nLife < nMaxLife)
		{
			nLife += D2CommonSkillsCalcParamThreeByLevel(SKILL_PRAYER, nLevel) << 8;
			if (nLife > nMaxLife)
				nLife = nMaxLife;
			D2CommonStatsUnitSetValue(hUnit, 6, nLife);
			nHealed++;
		}
	}
	for (i = 0; i < (int)(sizeof(SKILLEFFECT) / sizeof(DWORD)); i++)
		((DWORD *)&tEffect)[i] = 0;
	tEffect.dwFlags = AURA_FINDFLAGS;
	tEffect.nX = (int)D2CommonUnitGetX(hUnit);
	tEffect.nY = (int)D2CommonUnitGetY(hUnit);
	tEffect.hUnit = hUnit;
	tEffect.nSkill = SKILL_PRAYER;
	tEffect.nLevel = nLevel;
	tEffect.pfnAreaAffect = SkillsPrayerAffect;
	tEffect.dwAffectArg = (DWORD)&nHealed;
	SkillsAreaEffect(ptGame, &tEffect);
	if (nHealed)
	{
		D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_55, 1);
		SkillsSpendMana(hUnit, nCost);
		return TRUE;
	}
	D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_55, 0);
	return TRUE;
}

void __fastcall SkillsPrayerEnd(UNIT *hUnit, DWORD dwState)
{
	D2CommonStatsUnitSetActiveStateBit(hUnit, dwState, 0);
	D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_55, 0);
}

void __fastcall SkillsPrayerAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	int nMaxLife;
	int nLife;

	if (!SUnitIsAlly(ptGame, hUnit, hTarget))
		return;
	nMaxLife = D2CommonStatsUnitCalcMaxHp(hTarget);
	nLife = D2CommonStatsUnitGetValue(hTarget, 6);
	if (nLife < nMaxLife)
	{
		nLife += D2CommonSkillsCalcParamThreeByLevel(SKILL_PRAYER, nLevel) << 8;
		if (nLife > nMaxLife)
			nLife = nMaxLife;
		D2CommonStatsUnitSetValue(hTarget, 6, nLife);
		(*(int *)dwArg)++;
	}
	if (hUnit == hTarget)
		return;
	SkillsAuraSetState(hUnit, hTarget, SKILL_PRAYER, nLevel, 0xFFFFFFFF, 0, STATE_PRAYER, 0);
}

void __fastcall SkillsVigorAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	DWORD hList;

	if (!SUnitIsAlly(ptGame, hUnit, hTarget))
		return;
	hList = D2CommonStatsFindListByState(hTarget, STATE_VIGOR);
	if (hList)
	{
		D2CommonStatsListSetValueB(hList, 28, D2CommonSkillsCalcParamThreeByLevel(SKILL_VIGOR, nLevel));
		D2CommonStatsListSetValueB(hList, 162, D2CommonSkillsCalcParamThreeByLevel(SKILL_VIGOR, nLevel));
		D2CommonStatsListSetValueB(hList, 67, D2CommonSkillsCalcCurveParamFive(nLevel, SKILL_VIGOR));
		D2CommonStatsSetListEndFrame(hList, ptGame->nFrame + AURA_FRAMES);
	}
	else
	{
		hList = (DWORD)SkillsAuraSetState(hUnit, hTarget, SKILL_VIGOR, nLevel, 28,
			D2CommonSkillsCalcParamThreeByLevel(SKILL_VIGOR, nLevel), STATE_VIGOR, (DWORD)SkillsVigorEnd);
		if (hList)
		{
			D2CommonStatsListSetValueB(hList, 162, D2CommonSkillsCalcParamThreeByLevel(SKILL_VIGOR, nLevel));
			D2CommonStatsListSetValueB(hList, 67, D2CommonSkillsCalcCurveParamFive(nLevel, SKILL_VIGOR));
		}
	}
	D2CommonUnitUpdateAnimRate(hTarget);
}

void __fastcall SkillsVigorEnd(UNIT *hUnit, DWORD dwState)
{
	int nStamina;
	int nMaxStamina;

	D2CommonStatsUnitSetActiveStateBit(hUnit, dwState, 0);
	D2CommonUnitUpdateAnimRate(hUnit);
	nStamina = D2CommonStatsUnitGetValue(hUnit, 10);
	nMaxStamina = D2CommonStatsUnitCalcMaxStamina(hUnit);
	if (nStamina > nMaxStamina)
		D2CommonStatsUnitSetValue(hUnit, 10, nMaxStamina);
}

BOOL __fastcall SkillsVigor(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;

	SkillsAreaEffectAt(ptGame, AURA_FINDFLAGS, (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hUnit, SKILL_VIGOR, nLevel, SkillsVigorAffect);
	SkillsVigorAffect(ptGame, hUnit, hUnit, nLevel, 0);
	return TRUE;
}

void __fastcall SkillsMeditationAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	if (!SUnitIsAlly(ptGame, hUnit, hTarget))
		return;
	SkillsAuraSetState(hUnit, hTarget, SKILL_MEDITATION, nLevel, 27,
		D2CommonSkillsCalcParamThreeByLevel(SKILL_MEDITATION, nLevel), STATE_MEDITATION, 0);
}

BOOL __fastcall SkillsMeditation(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;

	SkillsAreaEffectAt(ptGame, AURA_FINDFLAGS, (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hUnit, SKILL_MEDITATION, nLevel,
		SkillsMeditationAffect);
	SkillsMeditationAffect(ptGame, hUnit, hUnit, nLevel, 0);
	return TRUE;
}

void __fastcall SkillsMightAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	if (!SUnitIsAlly(ptGame, hUnit, hTarget))
		return;
	SkillsAuraSetState(hUnit, hTarget, SKILL_MIGHT, nLevel, 25,
		D2CommonSkillsCalcParamThreeByLevel(SKILL_MIGHT, nLevel), STATE_MIGHT, 0);
}

BOOL __fastcall SkillsMight(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;

	SkillsMightAffect(ptGame, hUnit, hUnit, nLevel, 0);
	SkillsAreaEffectAt(ptGame, AURA_FINDFLAGS, (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hUnit, SKILL_MIGHT, nLevel, SkillsMightAffect);
	return TRUE;
}

void __fastcall SkillsBlessedAimAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	if (!SUnitIsAlly(ptGame, hUnit, hTarget))
		return;
	SkillsAuraSetState(hUnit, hTarget, SKILL_BLESSED_AIM, nLevel, 119,
		D2CommonSkillsCalcParamThreeByLevel(SKILL_BLESSED_AIM, nLevel), STATE_BLESSEDAIM, 0);
}

BOOL __fastcall SkillsBlessedAim(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;

	SkillsAreaEffectAt(ptGame, AURA_FINDFLAGS, (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hUnit, SKILL_BLESSED_AIM, nLevel,
		SkillsBlessedAimAffect);
	SkillsBlessedAimAffect(ptGame, hUnit, hUnit, nLevel, 0);
	return TRUE;
}

void __fastcall SkillsConcentrationAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	void *hList;

	if (!SUnitIsAlly(ptGame, hUnit, hTarget))
		return;
	hList = SkillsAuraSetState(hUnit, hTarget, SKILL_CONCENTRATION, nLevel, 25,
		D2CommonSkillsCalcParamThreeByLevel(SKILL_CONCENTRATION, nLevel), STATE_CONCENTRATION, 0);
	if (hList)
		D2CommonStatsListSetValueB(hList, 164, D2CommonSkillsGetTypeParamFive(SKILL_CONCENTRATION));
}

BOOL __fastcall SkillsConcentration(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;

	SkillsAreaEffectAt(ptGame, AURA_FINDFLAGS, (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hUnit, SKILL_CONCENTRATION, nLevel,
		SkillsConcentrationAffect);
	SkillsConcentrationAffect(ptGame, hUnit, hUnit, nLevel, 0);
	return TRUE;
}

void __fastcall SkillsFanaticismAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	void *hList;
	int nPercent;

	if (!SUnitIsAlly(ptGame, hUnit, hTarget))
		return;
	nPercent = D2CommonSkillsCalcCurveParamThree(nLevel, SKILL_FANATICISM);
	hList = SkillsAuraSetState(hUnit, hTarget, SKILL_FANATICISM, nLevel,
		68, D2CommonStatsUnitGetBaseValue(hTarget, 68) * nPercent / 100, STATE_FANATICISM, 0);
	if (hList)
		D2CommonStatsListSetValueB(hList, 119, D2CommonSkillsCalcToHitByLevel(SKILL_FANATICISM, nLevel));
}

BOOL __fastcall SkillsFanaticism(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;

	SkillsAreaEffectAt(ptGame, AURA_FINDFLAGS, (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hUnit, SKILL_FANATICISM, nLevel,
		SkillsFanaticismAffect);
	SkillsFanaticismAffect(ptGame, hUnit, hUnit, nLevel, 0);
	return TRUE;
}

BOOL __fastcall SkillsConviction(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;

	SkillsAreaEffectAt(ptGame, AURA_FINDFLAGS, (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hUnit, SKILL_CONVICTION, nLevel,
		SkillsConvictionAffect);
	SkillsAuraSetState(hUnit, hUnit, SKILL_CONVICTION, nLevel, 0xFFFFFFFF, 0, STATE_CONVICTION, 0);
	return TRUE;
}

void __fastcall SkillsConvictionAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	DWORD hList;
	int nPercent;
	int nDefence;
	int nValue;

	if (!SUnitIsHostile(ptGame, hUnit, hTarget))
		return;
	nPercent = D2CommonSkillsCalcCurveParamThree(nLevel, SKILL_CONVICTION);
	nDefence = D2CommonSkillsCalcCurveParamFive(nLevel, SKILL_CONVICTION);
	hList = (DWORD)SkillsAuraSetState(hUnit, hTarget, SKILL_CONVICTION, nLevel, 167, nLevel, STATE_CONVICTED, 0);
	if (!hList)
		return;
	D2CommonStatsListSetValueB(hList, 171, -nDefence);
	switch (hTarget->eType)
	{
	case UNIT_PLAYER:
		nValue = D2CommonStatsUnitGetValue(hTarget, 40);
		D2CommonStatsListSetValueB(hList, 40, nValue * nPercent / -100);
		break;
	case UNIT_MONSTER:
		nValue = D2CommonStatsUnitGetValue(hTarget, 39);
		D2CommonStatsListSetValueB(hList, 39, nValue * nPercent / -100);
		break;
	}
	switch (hTarget->eType)
	{
	case UNIT_PLAYER:
		nValue = D2CommonStatsUnitGetValue(hTarget, 42);
		D2CommonStatsListSetValueB(hList, 42, nValue * nPercent / -100);
		break;
	case UNIT_MONSTER:
		nValue = D2CommonStatsUnitGetValue(hTarget, 41);
		D2CommonStatsListSetValueB(hList, 41, nValue * nPercent / -100);
		break;
	}
	switch (hTarget->eType)
	{
	case UNIT_PLAYER:
		nValue = D2CommonStatsUnitGetValue(hTarget, 44);
		D2CommonStatsListSetValueB(hList, 44, nValue * nPercent / -100);
		break;
	case UNIT_MONSTER:
		nValue = D2CommonStatsUnitGetValue(hTarget, 43);
		D2CommonStatsListSetValueB(hList, 43, nValue * nPercent / -100);
		break;
	}
}

void __fastcall SkillsResistFireAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	if (!SUnitIsAlly(ptGame, hUnit, hTarget))
		return;
	SkillsAuraSetState(hUnit, hTarget, SKILL_RESIST_FIRE, nLevel, 39,
		D2CommonSkillsCalcCurveParamThree(nLevel, SKILL_RESIST_FIRE), STATE_RESISTFIRE, 0);
}

BOOL __fastcall SkillsResistFire(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;

	SkillsAreaEffectAt(ptGame, AURA_FINDFLAGS, (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hUnit, SKILL_RESIST_FIRE, nLevel,
		SkillsResistFireAffect);
	SkillsResistFireAffect(ptGame, hUnit, hUnit, nLevel, 0);
	return TRUE;
}

void __fastcall SkillsResistColdAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	if (!SUnitIsAlly(ptGame, hUnit, hTarget))
		return;
	SkillsAuraSetState(hUnit, hTarget, SKILL_RESIST_COLD, nLevel, 43,
		D2CommonSkillsCalcCurveParamThree(nLevel, SKILL_RESIST_COLD), STATE_RESISTCOLD, 0);
}

BOOL __fastcall SkillsResistCold(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;

	SkillsAreaEffectAt(ptGame, AURA_FINDFLAGS, (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hUnit, SKILL_RESIST_COLD, nLevel,
		SkillsResistColdAffect);
	SkillsResistColdAffect(ptGame, hUnit, hUnit, nLevel, 0);
	return TRUE;
}

void __fastcall SkillsResistLightningAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	if (!SUnitIsAlly(ptGame, hUnit, hTarget))
		return;
	SkillsAuraSetState(hUnit, hTarget, SKILL_RESIST_LIGHTNING, nLevel, 41,
		D2CommonSkillsCalcCurveParamThree(nLevel, SKILL_RESIST_LIGHTNING), STATE_RESISTLIGHTNING, 0);
}

BOOL __fastcall SkillsResistLightning(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;

	SkillsAreaEffectAt(ptGame, AURA_FINDFLAGS, (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hUnit, SKILL_RESIST_LIGHTNING, nLevel,
		SkillsResistLightningAffect);
	SkillsResistLightningAffect(ptGame, hUnit, hUnit, nLevel, 0);
	return TRUE;
}

void __fastcall SkillsSalvationAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	if (!SUnitIsAlly(ptGame, hUnit, hTarget))
		return;
	SkillsAuraSetState(hUnit, hTarget, SKILL_SALVATION, nLevel, 41,
		D2CommonSkillsCalcCurveParamThree(nLevel, SKILL_SALVATION), STATE_SALVATION, 0);
}

BOOL __fastcall SkillsSalvation(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;

	SkillsAreaEffectAt(ptGame, AURA_FINDFLAGS, (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hUnit, SKILL_SALVATION, nLevel, SkillsSalvationAffect);
	SkillsSalvationAffect(ptGame, hUnit, hUnit, nLevel, 0);
	return TRUE;
}

BOOL __fastcall SkillsHolyFire(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsDamageAuraPulse(ptGame, hUnit, nLevel, SKILL_HOLY_FIRE, STATE_HOLYFIRE, AURA_FINDFLAGS, SkillsHolyFireAffect);
	return TRUE;
}

void __fastcall SkillsHolyFireAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	DAMAGE tDamage;
	int nX;
	int nY;
	int i;

	if (!(hTarget->dwFlags0 & 4))
		return;
	if (!SUnitIsHostile(ptGame, hUnit, hTarget))
		return;
	if (!D2CommonSkillsAddFieldB((int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hTarget, AURA_SIGHTFLAGS))
		return;
	D2CommonUnitAddAnimOverlay(hTarget, 81, 0);
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	SkillsElemDamage(hUnit, &tDamage, SKILL_HOLY_FIRE, nLevel);
	tDamage.dwHitClass = 45;
	tDamage.wFlags = 16385;
	SUnitDmgApply(ptGame, hUnit, hTarget, TRUE, &tDamage);
	if (!D2CommonStatsUnitGetValue(hTarget, 6))
		tDamage.wFlags |= 2;
	SUnitDmgReact(ptGame, hUnit, hTarget, &tDamage);
}

BOOL __fastcall SkillsHolyFreeze(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsDamageAuraPulse(ptGame, hUnit, nLevel, SKILL_HOLY_FREEZE, STATE_HOLYFREEZE, AURA_FINDFLAGS, SkillsHolyFreezeAffect);
	return TRUE;
}

void __fastcall SkillsHolyFreezeAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	DWORD hList;
	int nSlow;
	int nX;
	int nY;

	if (!(hTarget->dwFlags0 & 4))
		return;
	if (!SUnitIsHostile(ptGame, hUnit, hTarget))
		return;
	if (!D2CommonSkillsAddFieldB((int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hTarget, AURA_SIGHTFLAGS))
		return;
	nSlow = -D2CommonSkillsCalcCurveParamThree(nLevel, SKILL_HOLY_FREEZE);
	hList = (DWORD)SkillsAuraSetState(hUnit, hTarget, SKILL_HOLY_FREEZE, nLevel,
		67, nSlow, STATE_HOLYFROZEN, (DWORD)SkillsHolyFreezeEnd);
	if (hList)
	{
		D2CommonStatsListSetValueA(hList, 68, nSlow);
		D2CommonStatsListSetValueA(hList, 69, nSlow);
	}
	D2CommonUnitUpdateAnimRate(hTarget);
	if (D2CommonSeedRand(&hTarget->tSeed, 100) < 20)
		D2CommonStatsUnitSetActiveStateBit(hTarget, STATE_6B, 1);
	else
		D2CommonStatsUnitSetActiveStateBit(hTarget, STATE_6B, 0);
}

void __fastcall SkillsHolyFreezeEnd(UNIT *hUnit, DWORD dwState)
{
	if (SUnitIsDead(hUnit) && D2CommonStatsUnitTestStateKept(hUnit, dwState))
		return;
	D2CommonStatsUnitSetActiveStateBit(hUnit, dwState, 0);
	if (!SUnitIsDead(hUnit))
		D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_6B, 0);
	D2CommonUnitUpdateAnimRate(hUnit);
}

BOOL __fastcall SkillsHolyShock(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsDamageAuraPulse(ptGame, hUnit, nLevel, SKILL_HOLY_SHOCK, STATE_HOLYSHOCK, AURA_FINDFLAGS, SkillsHolyShockAffect);
	return TRUE;
}

void __fastcall SkillsHolyShockAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	DAMAGE tDamage;
	int nX;
	int nY;
	int i;

	if (!(hTarget->dwFlags0 & 4))
		return;
	if (!SUnitIsHostile(ptGame, hUnit, hTarget))
		return;
	if (!D2CommonSkillsAddFieldB((int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hTarget, AURA_SIGHTFLAGS))
		return;
	D2CommonUnitAddAnimOverlay(hTarget, 110, 0);
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	SkillsElemDamage(hUnit, &tDamage, SKILL_HOLY_SHOCK, nLevel);
	tDamage.dwHitClass = 77;
	tDamage.wFlags = 16385;
	SUnitDmgApply(ptGame, hUnit, hTarget, TRUE, &tDamage);
	if (!D2CommonStatsUnitGetValue(hTarget, 6))
		tDamage.wFlags |= 2;
	SUnitDmgReact(ptGame, hUnit, hTarget, &tDamage);
}

BOOL __fastcall SkillsThorns(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;

	SkillsAreaEffectAt(ptGame, AURA_FINDFLAGS, (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hUnit, SKILL_THORNS, nLevel, SkillsThornsAffect);
	SkillsThornsAffect(ptGame, hUnit, hUnit, nLevel, 0);
	return TRUE;
}

void __fastcall SkillsThornsAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{

	if (!SUnitIsAlly(ptGame, hUnit, hTarget))
		return;
	SkillsAuraSetState(hUnit, hTarget, SKILL_THORNS, nLevel, STAT_THORNS, nLevel, STATE_THORNS, 0);
}

BOOL __fastcall SkillsSanctuary(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsDamageAuraPulse(ptGame, hUnit, nLevel, SKILL_SANCTUARY, STATE_SANCTUARY, 8194, SkillsSanctuaryAffect);
	return TRUE;
}

void __fastcall SkillsSanctuaryAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	DAMAGE tDamage;
	MONSTATSTXT *ptStats;
	UNIT *hOwner;
	int i;

	if (hTarget->eType != UNIT_MONSTER)
		return;
	ptStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(hTarget->dwClassId);
	if (!ptStats->bLUndead && !ptStats->bHUndead)
		return;
	hOwner = AiGetOwner(hTarget);
	if (hOwner && hOwner->eType == UNIT_PLAYER && !PlayerListTestFlag(hUnit, hOwner, 8))
		return;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = 9;
	tDamage.dwFlags0 = 2;
	tDamage.dwHitClass = 157;
	SkillsElemDamage(hUnit, &tDamage, SKILL_SANCTUARY, nLevel);
	SUnitDmgApply(ptGame, hUnit, hTarget, TRUE, &tDamage);
	SUnitDmgReact(ptGame, hUnit, hTarget, &tDamage);
}

void __fastcall SkillsRedemptionAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	int nChance;
	int nAdd;
	int nMax;
	int nNow;

	if (!hTarget)
		return;
	if (!D2CommonSkillsUnitTestGuardC(hTarget))
		return;
	nChance = D2CommonSkillsCalcCurveParamThree(nLevel, SKILL_REDEMPTION);
	if (!dwArg && (int)D2CommonSeedRand(&hUnit->tSeed, 100) >= nChance)
		return;
	if (hUnit && hUnit->eType == UNIT_PLAYER)
	{
		nAdd = D2CommonSkillsCalcParamFiveByLevel(SKILL_REDEMPTION, nLevel) << 8;
		nMax = D2CommonStatsUnitCalcMaxHp(hUnit);
		nNow = D2CommonStatsUnitGetValue(hUnit, 6);
		if (nNow + nAdd > nMax)
			nAdd = nMax - nNow;
		D2CommonStatsUnitAddValue(hUnit, 6, nAdd);
		nAdd = D2CommonSkillsCalcParamFiveByLevel(SKILL_REDEMPTION, nLevel) << 8;
		nMax = D2CommonStatsUnitCalcMaxMana(hUnit);
		nNow = D2CommonStatsUnitGetValue(hUnit, 8);
		if (nNow + nAdd > nMax)
			nAdd = nMax - nNow;
		D2CommonStatsUnitAddValue(hUnit, 8, nAdd);
	}
	D2CommonStatsUnitSetActiveStateBit(hTarget, STATE_REDEEMED, 1);
	hTarget->dwFlags0 &= ~6;
}

BOOL __fastcall SkillsRedemption(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;

	SkillsAreaEffectAt(ptGame, 12290, (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hUnit, SKILL_REDEMPTION, nLevel,
		(PFNAREAAFFECT)MissModeRadamentDeathFirst);
	SkillsAuraSetState(hUnit, hUnit, SKILL_REDEMPTION, nLevel, 0xFFFFFFFF, 0, STATE_REDEMPTION, 0);
	return TRUE;
}

BOOL __fastcall SkillsBlessedHammer(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreate;
	UNIT *hMissile;
	void *ptPath;
	int nX;
	int nY;
	int i;

	hUnit->dwFlags0 |= 64;
	if (!SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY))
		return FALSE;
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.nDX = nX;
	tCreate.dwFlags = MISSFLAG_TARGETABS;
	tCreate.hSource = hUnit;
	tCreate.hSpawnLocation = hUnit;
	tCreate.nDY = nY;
	tCreate.nLevel = nLevel;
	tCreate.eMissile = MISSILE_BLESSEDHAMMER;
	hMissile = MissilesCreate(ptGame, &tCreate);
#line 1374
	D2ASSERT(hMissile);
	ptPath = hMissile->ptPath;
	D2CommonPathSetType(ptPath, 14);
	D2CommonPathCompute(ptPath, hMissile);
	return TRUE;
}

void __fastcall SkillsDefianceAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	if (!SUnitIsAlly(ptGame, hUnit, hTarget))
		return;
	SkillsAuraSetState(hUnit, hTarget, SKILL_DEFIANCE, nLevel, 171,
		D2CommonSkillsCalcParamThreeByLevel(SKILL_DEFIANCE, nLevel), STATE_DEFIANCE, 0);
}

BOOL __fastcall SkillsDefiance(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;

	SkillsAreaEffectAt(ptGame, AURA_FINDFLAGS, (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hUnit, SKILL_DEFIANCE, nLevel, SkillsDefianceAffect);
	SkillsDefianceAffect(ptGame, hUnit, hUnit, nLevel, 0);
	return TRUE;
}

BOOL __fastcall SkillsCleansing(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;

	SkillsAreaEffectAt(ptGame, AURA_FINDFLAGS, (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), hUnit, SKILL_CLEANSING, nLevel, SkillsCleansingAffect);
	SkillsCleansingAffect(ptGame, hUnit, hUnit, nLevel, 0);
	return TRUE;
}

void __fastcall SkillsCleansingAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel, DWORD dwArg)
{
	DWORD hList;
	int nPercent;
	int nFrame;
	int nEnd;

	if (!SUnitIsAlly(ptGame, hUnit, hTarget))
		return;
	SkillsAuraSetState(hUnit, hTarget, SKILL_CLEANSING, nLevel, 31, 0, STATE_CLEANSING, 0);
	nPercent = 100 - D2CommonSkillsCalcCurveParamThree(nLevel, SKILL_CLEANSING);
	hList = D2CommonStatsFindListByState(hTarget, 2);
	if (hList)
	{
		nFrame = ptGame->nFrame;
		nEnd = ((int)D2CommonStatsGetListEndFrame(hList) - nFrame) * nPercent / 100 + nFrame;
		D2CommonStatsSetListEndFrame(hList, nEnd);
		EventAdd(ptGame, hTarget, EVENT_STATEEND, nEnd);
	}
	hList = D2CommonStatsFindListByMask(hTarget, 32);
	if (!hList)
		return;
	if (!D2CommonStatsTestStateSharedListBase(D2CommonStatsGetListState(hList)))
		return;
	nFrame = ptGame->nFrame;
	nEnd = ((int)D2CommonStatsGetListEndFrame(hList) - nFrame) * nPercent / 100 + nFrame;
	D2CommonStatsSetListEndFrame(hList, nEnd);
	EventAdd(ptGame, hTarget, EVENT_STATEEND, nEnd);
}

BOOL __fastcall SkillsChargeStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MONMODECHANGE tReq;
	CHARSTATSTXT *ptPlayerStats;
	MONSTATSTXT *ptStats;
	UNIT *hTarget;
	void *ptPath;
	DWORD hSkill;
	DWORD dwClassId;
	BOOL bAimed;
	int nVelocity;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	ptPath = hUnit->ptPath;
#line 1521
	D2ASSERT(hSkill);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	bAimed = FALSE;
	if (hUnit->eType == UNIT_PLAYER)
	{
		if (hTarget && D2CommonUnitTestPositionClear(hUnit, hTarget, 0))
			return SkillsWhirlAt(ptGame, hUnit, hTarget);
	}
	else if (hTarget && D2CommonUnitTestPositionClear(hUnit, hTarget, 0))
	{
		SUnitSetMode(ptGame, hUnit, 1);
		D2CommonUnitSetSkillThirdCached(hUnit, 0);
		MonsterModeInit(hUnit, 5, &tReq);
		tReq.hTarget = hTarget;
		return MonsterModeSet(ptGame, &tReq, TRUE);
	}
	if (hTarget)
	{
		if (!D2CommonStatsGetState(hTarget, STATE_36))
		{
			D2CommonSkillsSetNodeScratchC(hSkill, hTarget->eType);
			D2CommonSkillsSetNodeScratchD(hSkill, hTarget->dwId);
			SkillsLeapHit(ptGame, hUnit, hTarget);
			bAimed = TRUE;
		}
	}
	if (!bAimed)
	{
		D2CommonSkillsSetNodeScratchC(hSkill, UNIT_NONE);
		D2CommonSkillsSetNodeScratchD(hSkill, 0);
	}
	D2CommonSkillsSetNodeScratchF(hSkill, 7);
	if (hUnit->eType == UNIT_PLAYER)
	{
		ptPlayerStats = (CHARSTATSTXT *)D2CommonDataTblsGetCharStats(hUnit->dwClassId);
#line 1565
		D2ASSERT(ptPlayerStats);
		nVelocity = ptPlayerStats->RunVelocity << 8;
	}
	else
	{
		dwClassId = hUnit->dwClassId;
		ptStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(dwClassId);
#line 1574
		D2ASSERT(ptStats);
		nVelocity = (int)(ptStats->Run << 8);
		if (dwClassId == MONSTER_D3)
			D2CommonSkillsSetNodeScratchF(hSkill, 15);
		D2CommonPathClearKeptTarget(ptPath);
		D2CommonPathSetDistMax(ptPath, 20);
	}
	nVelocity = D2CommonStatsUnitGetValue(hUnit, 67) * nVelocity / 100;
	nVelocity = (int)D2CommonSkillsGetTypeParamOne(SKILL_CHARGE) * nVelocity / 100;
	D2CommonPathSetVelocity(ptPath, nVelocity);
	D2CommonPathCompute(ptPath, hUnit);
	D2CommonSkillsSetNodeFlags(hSkill, 1);
	D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_CHARGE, 1);
	return TRUE;
}

BOOL __fastcall SkillsCharge(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	SKILLFINDTARGET tFind;
	UNIT *hTarget;
	UNIT *hFound;
	DWORD hSkill;
	DWORD dwSkillFlags;
	DWORD dwClass;
	DWORD eTargetType;
	DWORD dwTargetId;
	int nHitFrame;
	int nLength;
	int i;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	dwSkillFlags = D2CommonSkillsGetNodeFlags(hSkill);
	hTarget = NULL;
	nHitFrame = (int)D2CommonSkillsGetNodeScratchF(hSkill);
	nLength = 15;
	if (hUnit->eType == UNIT_MONSTER)
	{
		dwClass = D2CommonMonsterUnitGetTypeLocalIndex(hUnit);
		if (dwClass != MONSTER_49)
		{
			if (dwClass == MONSTER_D3)
			{
				nLength = 31;
				nHitFrame = 14;
			}
		}
		else
		{
			nLength = 14;
			nHitFrame = 7;
		}
	}
	eTargetType = D2CommonSkillsGetNodeScratchC(hSkill);
	dwTargetId = D2CommonSkillsGetNodeScratchD(hSkill);
	if (eTargetType != UNIT_NONE)
		hTarget = SUnitFind(ptGame, eTargetType, dwTargetId);
	if (dwSkillFlags & 1)
	{
		if (hTarget && D2CommonUnitTestPositionClear(hUnit, hTarget, 0))
		{
			D2CommonSkillsSetNodeFlags(hSkill, 0);
			D2CommonUnitSetAnimSequencePos(hUnit, nHitFrame + 1);
			hUnit->nAnimLength = nLength << 8;
			D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_CHARGE, 0);
			return TRUE;
		}
		if (dwSkillFlags & 2)
		{
			D2CommonSkillsSetNodeFlags(hSkill, 0);
			D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_CHARGE, 0);
			for (i = 0; i < (int)(sizeof(SKILLFINDTARGET) / sizeof(DWORD)); i++)
				((DWORD *)&tFind)[i] = 0;
			tFind.dwFlags = 3;
			tFind.hUnit = hUnit;
			tFind.dwUnitId = dwTargetId;
			tFind.nX = (int)D2CommonUnitGetX(hUnit);
			tFind.nY = (int)D2CommonUnitGetY(hUnit);
			tFind.nRange = 3;
			hFound = SkillsFindTarget(ptGame, &tFind);
			if (hFound && D2CommonUnitTestPositionClear(hUnit, hFound, 0))
			{
				D2CommonSkillsSetNodeScratchC(hSkill, hFound->eType);
				D2CommonSkillsSetNodeScratchD(hSkill, hFound->dwId);
				return TRUE;
			}
			hUnit->nAnimLength = 256;
			return FALSE;
		}
		if ((hUnit->nSeqPos >> 8) >= nHitFrame)
		{
			D2CommonUnitSetAnimSequencePos(hUnit, 0);
			hUnit->nAnimLength = nLength << 8;
			D2CommonSkillsSetNodeFlags(hSkill, 1);
		}
		hUnit->dwFlags0 &= ~64;
		return TRUE;
	}
	D2CommonSkillsSetNodeFlags(hSkill, 0);
	D2CommonSkillsSetNodeScratchC(hSkill, 0);
	D2CommonSkillsSetNodeScratchD(hSkill, 0);
	D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_CHARGE, 0);
	hUnit->nAnimLength = (nLength - (hUnit->nSeqPos >> 8)) << 8;
	hUnit->dwFlags0 |= 64;
	if (!hTarget)
	{
		for (i = 0; i < (int)(sizeof(SKILLFINDTARGET) / sizeof(DWORD)); i++)
			((DWORD *)&tFind)[i] = 0;
		tFind.hUnit = hUnit;
		tFind.dwFlags = 3;
		tFind.dwUnitId = 0xFFFFFFFF;
		tFind.nX = (int)D2CommonUnitGetX(hUnit);
		tFind.nY = (int)D2CommonUnitGetY(hUnit);
		tFind.nRange = 3;
		hTarget = SkillsFindTarget(ptGame, &tFind);
		if (!hTarget)
			goto lbDone;
	}
	if (!D2CommonUnitTestPositionClear(hUnit, hTarget, 0))
		goto lbDone;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = 9;
	tDamage.dwDamagePercent = D2CommonSkillsCalcParamThreeByLevel(SKILL_CHARGE, nLevel);
	if (hUnit->eType == UNIT_MONSTER && hUnit->dwClassId == MONSTER_D3)
		MonsterEvalSetDamage(hUnit, 5);
	tDamage.dwHitClass = 112;
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	SUnitDmgDeliver(ptGame, hUnit, hTarget);
	D2CommonUnitAddAnimOverlay(hTarget, 147, 0);
lbDone:
	return TRUE;
}

BOOL __fastcall SkillsVengeanceStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	DWORD hSkill;
	int nToHit;
	int nPercent;
	int nMin;
	int nMax;
	int nElement;
	int nWhich;
	int i;

	hSkill = D2CommonSkillsFindByKey(hUnit, SKILL_VENGEANCE);
	D2CommonSkillsSetNodeFlags(hSkill, 8);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	nToHit = D2CommonSkillsCalcToHitByLevel(SKILL_VENGEANCE, nLevel);
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget, nToHit);
	if (tDamage.wFlags & 1)
	{

		tDamage.nPhysicalDamage = SUnitDmgRollDamage(hUnit, D2CommonStatsUnitGetValue(hUnit, 21) << 8, D2CommonStatsUnitGetValue(hUnit, 22) << 8, 0, 0);
		tDamage.dwFlags0 = 1;
		nPercent = D2CommonSkillsCalcParamOneByLevel(SKILL_VENGEANCE, nLevel);
		nMin = D2CommonStatsUnitGetValue(hUnit, 21);
		nMax = D2CommonStatsUnitGetValue(hUnit, 22);
		if (nMax - nMin > 0)
			nMin += (int)D2CommonSeedRand(&hUnit->tSeed, nMax - nMin);
		nElement = (nMin * nPercent / 100) << 8;
		tDamage.nFireDamage = nElement;
		tDamage.nColdDamage = nElement;
		tDamage.nLightningDamage = nElement;
		tDamage.nColdLength = D2CommonSkillsCalcELenByLevel(SKILL_VENGEANCE, nLevel);
		nWhich = (int)D2CommonSkillsGetNodeScratchC(hSkill);
		switch (nWhich)
		{
		case 0:
			tDamage.dwHitClass = 32;
			break;
		case 1:
			tDamage.dwHitClass = 48;
			break;
		case 2:
			tDamage.dwHitClass = 64;
			break;
		}
		D2CommonSkillsSetNodeScratchC(hSkill, (nWhich + 1) % 3);
	}
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	return TRUE;
}

void __fastcall SkillsConversionEnd(UNIT *hUnit, DWORD dwState)
{
	SUnitSetAlignment(hUnit, 2);
	D2CommonStatsUnitSetActiveStateBit(hUnit, dwState, 0);
	TargetsRemove(SUnitGetGame(hUnit), hUnit);
}

BOOL __fastcall SkillsConversionStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget, 0);
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	return TRUE;
}

BOOL __fastcall SkillsConversion(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;
	UNIT *hOwner;
	DWORD hList;
	BOOL bCanConvert;
	int nChance;
	int nEnd;

	hUnit->dwFlags0 |= 64;
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	if (!SUnitDmgFindHeld(hUnit, hTarget))
	{
		SUnitDmgDeliver(ptGame, hUnit, hTarget);
		return FALSE;
	}
	bCanConvert = TRUE;
	if (hTarget->eType != UNIT_MONSTER)
		bCanConvert = FALSE;
	else if (D2CommonStatsUnitGetAlignment(hTarget) != 2)
		bCanConvert = FALSE;
	else if (!SkillsCanBeAffected(hTarget, 11))
		bCanConvert = FALSE;
	nChance = D2CommonSkillsCalcCurveParamThree(nLevel, SKILL_CONVERSION);
	if ((int)D2CommonSeedRand(&hUnit->tSeed, 100) > nChance || !bCanConvert)
	{
		SUnitDmgDeliver(ptGame, hUnit, hTarget);
		return FALSE;
	}
	nEnd = D2CommonSkillsCalcParamOneByLevel(SKILL_CONVERSION, nLevel) + ptGame->nFrame;
	hList = D2CommonStatsFindListByState(hTarget, STATE_CONVERSION);
	if (!hList)
	{
		hList = D2CommonStatsAllocList(ptGame->pMemPool, 2050, nEnd, hUnit->eType,
			hUnit->dwId);
		if (!hList)
			return FALSE;
		D2CommonStatsSetListState(hList, STATE_CONVERSION);
		D2CommonStatsSetListRemoveCallback(hList, SkillsConversionEnd);
		D2CommonStatsUnitLinkList(hTarget, hList);
		D2CommonStatsUnitSetActiveStateBit(hTarget, STATE_CONVERSION, 1);
	}
	D2CommonStatsSetListEndFrame(hList, nEnd);
	EventAdd(ptGame, hTarget, EVENT_STATEEND, nEnd);
	hOwner = AiGetOwner(hTarget);
	if (hOwner && hOwner->eType == UNIT_MONSTER)
	{
		if (hOwner == hTarget)
			AiFreeMinions(hTarget);
		else
			AiRemoveFromOwner(hTarget);
	}
	SUnitSetAlignment(hTarget, 0);
	TargetsAddToList(ptGame, hTarget, 0, hUnit->nTargetListNo);
	D2CommonUnitSetTarget(hUnit, 0);
	return TRUE;
}

BOOL __fastcall SkillsHolyShieldStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	if (!hUnit->ptInventory)
		return FALSE;
	return D2CommonINVENTORYFindShieldInHands(hUnit->ptInventory, 0) != 0;
}

BOOL __fastcall SkillsHolyShield(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLSTATE tData;
	int i;

	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tData)[i] = 0;
	tData.hSource = hUnit;
	tData.hTarget = hUnit;
	tData.nSkill = SKILL_HOLY_SHIELD;
	tData.nLevel = nLevel;
	tData.nDuration = D2CommonSkillsCalcParamOneByLevel(SKILL_HOLY_SHIELD, nLevel);
	tData.dwStatId = 20;
	tData.nStatValue = D2CommonSkillsCalcCurveParamFive(nLevel, SKILL_HOLY_SHIELD);
	tData.dwState = STATE_HOLYSHIELD;
	tData.pfnRemove = 0;
	SkillsSetState(&tData);
	return TRUE;
}

BOOL __fastcall SkillsZealStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;
	DWORD hSkill;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hSkill)
		return FALSE;
	if (!hTarget)
	{
		hTarget = SkillsFindNext(ptGame, hUnit, 0xFFFFFFFF, 4, 4);
		if (!hTarget)
		{
			D2CommonSkillsSetNodeScratchC(hSkill, 0);
			return FALSE;
		}
	}
	D2CommonSkillsSetNodeScratchC(hSkill, D2CommonSkillsCalcParamFiveByLevel(SKILL_ZEAL, nLevel));
	D2CommonSkillsSetNodeScratchD(hSkill, hTarget->eType);
	D2CommonSkillsSetNodeScratchE(hSkill, hTarget->dwId);
	return TRUE;
}

BOOL __fastcall SkillsZeal(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	DWORD hSkill;
	DWORD dwTargetId;
	DWORD eTargetType;
	int nBlows;
	int nToHit;
	int i;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	nBlows = (int)D2CommonSkillsGetNodeScratchC(hSkill);
	hTarget = SUnitFind(ptGame, D2CommonSkillsGetNodeScratchD(hSkill), D2CommonSkillsGetNodeScratchE(hSkill));
	if (!hTarget)
	{
		hTarget = SkillsFindNext(ptGame, hUnit, 0xFFFFFFFF, 4, 4);
		if (!hTarget)
			return FALSE;
	}
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	nToHit = D2CommonSkillsCalcToHitByLevel(SKILL_ZEAL, nLevel);
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget, nToHit);
	tDamage.dwDamagePercent = D2CommonSkillsCalcParamThreeByLevel(SKILL_ZEAL, nLevel);
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	SUnitDmgDeliver(ptGame, hUnit, hTarget);
	nBlows--;
	if (nBlows <= 0)
		return FALSE;
	D2CommonSkillsSetNodeScratchC(hSkill, nBlows);
	hTarget = SkillsFindNext(ptGame, hUnit, hTarget->dwId, 4, 4);
	if (!hTarget)
		return FALSE;
	D2CommonSkillsSetNodeScratchD(hSkill, hTarget->eType);
	D2CommonSkillsSetNodeScratchE(hSkill, hTarget->dwId);
	SkillsSlowAnim(hUnit, (int)D2CommonSkillsGetTypeParamTwo(D2CommonSkillsGetNodeType(hSkill)));
	return FALSE;
}

BOOL __fastcall SkillsFistOfTheHeavens(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreate;
	UNIT *hTarget;
	UNIT *hMissile;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	D2CommonUnitAddAnimOverlay(hTarget, 158, 0);
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = MISSFLAG_POSITION;
	tCreate.hSource = hUnit;
	tCreate.nX = (int)D2CommonUnitGetX(hTarget);
	tCreate.nY = (int)D2CommonUnitGetY(hTarget);
	tCreate.nLevel = nLevel;
	tCreate.eMissile = MISSILE_FISTOFTHEHEAVENSDELAY;
	hMissile = MissilesCreate(ptGame, &tCreate);
	if (hMissile)
	{
		D2CommonMissileSetScratchFirst(hMissile, hTarget->eType);
		D2CommonMissileSetScratchSecond(hMissile, hTarget->dwId);
	}
	return TRUE;
}

static SKILLCHART sgptSkillChart[NUM_PALADIN_SKILLS] =
{
	{ SkillsSacrificeStart, SkillsSacrifice, SKILLCHART_NOMISSILE },
	{ SkillsSmiteStart, SkillsUseHit, SKILLCHART_NOMISSILE },
	{ NULL, SkillsMight, SKILLCHART_NOMISSILE },
	{ NULL, SkillsPrayer, SKILLCHART_NOMISSILE },
	{ NULL, SkillsResistFire, SKILLCHART_NOMISSILE },
	{ NULL, NULL, MISSILE_HOLYBOLT },
	{ NULL, SkillsHolyFire, SKILLCHART_NOMISSILE },
	{ NULL, SkillsThorns, SKILLCHART_NOMISSILE },
	{ NULL, SkillsDefiance, SKILLCHART_NOMISSILE },
	{ NULL, SkillsResistCold, SKILLCHART_NOMISSILE },
	{ SkillsZealStart, SkillsZeal, SKILLCHART_NOMISSILE },
	{ SkillsChargeStart, SkillsCharge, SKILLCHART_NOMISSILE },
	{ NULL, SkillsBlessedAim, SKILLCHART_NOMISSILE },
	{ NULL, SkillsCleansing, SKILLCHART_NOMISSILE },
	{ NULL, SkillsResistLightning, SKILLCHART_NOMISSILE },
	{ SkillsVengeanceStart, SkillsUseHit, SKILLCHART_NOMISSILE },
	{ NULL, SkillsBlessedHammer, SKILLCHART_NOMISSILE },
	{ NULL, SkillsConcentration, SKILLCHART_NOMISSILE },
	{ NULL, SkillsHolyFreeze, SKILLCHART_NOMISSILE },
	{ NULL, SkillsVigor, SKILLCHART_NOMISSILE },
	{ SkillsConversionStart, SkillsConversion, SKILLCHART_NOMISSILE },
	{ SkillsHolyShieldStart, SkillsHolyShield, SKILLCHART_NOMISSILE },
	{ NULL, SkillsHolyShock, SKILLCHART_NOMISSILE },
	{ NULL, SkillsSanctuary, SKILLCHART_NOMISSILE },
	{ NULL, SkillsMeditation, SKILLCHART_NOMISSILE },
	{ NULL, SkillsFistOfTheHeavens, SKILLCHART_NOMISSILE },
	{ NULL, SkillsFanaticism, SKILLCHART_NOMISSILE },
	{ NULL, SkillsConviction, SKILLCHART_NOMISSILE },
	{ NULL, SkillsRedemption, SKILLCHART_NOMISSILE },
	{ NULL, SkillsSalvation, SKILLCHART_NOMISSILE },
};

BOOL __fastcall SkillsPalStart(GAME *ptGame, UNIT *hUnit, int nSkillId  ,
	int nLevel)
{
#line 2221
	D2ASSERT(nSkillId >= SKILL_PALADIN_START);
#line 2222
	D2ASSERT(nSkillId - SKILL_PALADIN_START < NUM_PALADIN_SKILLS);
	if (!SkillsHasMana(hUnit))
		return FALSE;
	if (sgptSkillChart[nSkillId - SKILL_PALADIN_START].pfnStart) {
#line 2229
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgptSkillChart[nSkillId - SKILL_PALADIN_START].pfnStart), "sgptSkillChart[ nSkillId - SKILL_PALADIN_START ].pfnStart");
		if (sgptSkillChart[nSkillId - SKILL_PALADIN_START].pfnStart(ptGame, hUnit, nLevel))
			SkillsUseMana(hUnit);
		else
			return FALSE;
	}
	return TRUE;
}
#line 2235

BOOL __fastcall SkillsPalUse(GAME *ptGame, UNIT *hUnit, int nSkillId,
	int nLevel)
{
	SKILLSTXT *ptSkillsTxt;
	MISSILESTXT *ptMissilesTxt;
	int nPalSkillId;

	nPalSkillId = nSkillId - SKILL_PALADIN_START;
#line 2249
	D2ASSERT(nPalSkillId >= 0);
#line 2250
	D2ASSERT(nPalSkillId < NUM_PALADIN_SKILLS);
	ptSkillsTxt = (SKILLSTXT *)D2CommonDataTblsGetSkill(nSkillId);
	if (sgptSkillChart[nPalSkillId].pfnUse)
	{
#line 2255
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgptSkillChart[nPalSkillId].pfnUse), "sgptSkillChart[nPalSkillId].pfnUse");
		if (sgptSkillChart[nPalSkillId].pfnUse(ptGame, hUnit, nLevel)) {
			if (!sgptSkillChart[nPalSkillId].pfnStart && !ptSkillsTxt->Periodic)
				SkillsUseMana(hUnit);
		} else
			return FALSE;
	}
	if (sgptSkillChart[nPalSkillId].nMissile != SKILLCHART_NOMISSILE)
	{
		ptMissilesTxt = (MISSILESTXT *)D2CommonMissileTblsGetMissile(sgptSkillChart[nPalSkillId].nMissile);
		hUnit->dwFlags0 |= 64;
		SkillsCreateMissile(ptGame, sgptSkillChart[nPalSkillId].nMissile, hUnit, nLevel, 0, 0, 0, 0,
			ptMissilesTxt->Qty);
		if (!sgptSkillChart[nPalSkillId].pfnStart)
			SkillsUseMana(hUnit);
	}
	return TRUE;
}

WORD __fastcall SkillsThornsHit(GAME *ptGame, UNIT *hAttacker, UNIT *hTarget,
	DAMAGE *ptDamage)
{
	DAMAGE tDamage;
	DWORD dwLevel;
	int nPercent;
	int i;

	dwLevel = D2CommonStatsGetTotalValue(hTarget, STAT_THORNS);
	if (!dwLevel)
		return 0;
	if (hTarget->eType == UNIT_MONSTER && hTarget->dwClassId == MONSTER_123)
	{
		if (hAttacker->eType != UNIT_PLAYER)
			nPercent = D2CommonSkillsCalcParamOneByLevel(SKILL_IRONGOLEM, dwLevel);
		else
			nPercent = D2CommonSkillsCalcParamThreeByLevel(SKILL_IRONGOLEM, dwLevel);
	}
	else
	{
		if (hAttacker->eType == UNIT_PLAYER)
			nPercent = D2CommonSkillsCalcParamFiveByLevel(SKILL_THORNS, dwLevel);
		else
			nPercent = D2CommonSkillsCalcParamThreeByLevel(SKILL_THORNS, dwLevel);
	}
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.dwHitClass = 141;
	tDamage.wFlags = 16385;
	tDamage.nPhysicalDamage = ptDamage->nPhysicalDamage * nPercent / 100;
	SUnitDmgApply(ptGame, hTarget, hAttacker, TRUE, &tDamage);
	SUnitDmgReact(ptGame, hTarget, hAttacker, &tDamage);
	return tDamage.wFlags;
}
