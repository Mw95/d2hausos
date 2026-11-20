#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\UNIT\\SUnitDmg.cpp"
#include "D2Macros.h"
#include "../DEBUG/Debug.h"
#include "../GAME/Event.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/Monster.h"
#include "../MONSTER/MonsterEval.h"
#include "../MONSTER/MonsterMode.h"
#include "../MONSTER/MonsterUnique.h"
#include "../MISSILES/MissMode.h"
#include "../PLAYER/PlayerPets.h"
#include "../PLAYER/PlrModes.h"
#include "../QUESTS/QUESTS.h"
#include "../SKILLS/SkillItem.h"
#include "../SKILLS/SkillNec.h"
#include "../SKILLS/SkilPal.h"
#include "SUnit.h"
#include "SUnitDmg.h"
#include "Party.h"
#include "../AI/AiGeneral.h"
#include "../SKILLS/Skills.h"
#include "../SKILLS/SkillSor.h"

#define UNIT_PLAYER			0
#define UNIT_OBJECT			2
#define UNIT_ITEM			4
#define PARTY_NONE			65535
#define STATS_NONE			-1
#define STATS_STRENGTH		0
#define STATS_LEVEL			12
#define STATS_EXPERIENCE	13
#define STATS_TOHIT			19
#define STATS_TOBLOCK		20
#define STATS_MINDAMAGE		21
#define STATS_MAXDAMAGE		22
#define STATS_LASTEXP		29
#define STATS_ARMORCLASS	31
#define STATS_ARMORCLASS_VS_MISSILE	32
#define STATS_ARMORCLASS_VS_HTH		33
#define STATS_FIREMINDAM	48
#define STATS_FIREMAXDAM	49
#define STATS_LIGHTMINDAM	50
#define STATS_LIGHTMAXDAM	51
#define STATS_MAGICMINDAM	52
#define STATS_MAGICMAXDAM	53
#define STATS_COLDMINDAM	54
#define STATS_COLDMAXDAM	55
#define STATS_COLDLENGTH	56
#define STATS_POISONMINDAM	57
#define STATS_POISONMAXDAM	58
#define STATS_POISONLENGTH	59
#define STATS_LIFEDRAINMINDAM	60
#define STATS_LIFEDRAINMAXDAM	61
#define STATS_MANADRAINMINDAM	62
#define STATS_MANADRAINMAXDAM	63
#define STATS_STAMDRAINMINDAM	64
#define STATS_STAMDRAINMAXDAM	65
#define STATS_STUNLENGTH	66
#define STATS_DEXTERITY		2
#define STATS_HITPOINTS		6
#define STATS_MANA			8
#define STATS_STAMINA		10
#define STATS_DAMAGEPERCENT	25
#define STATS_NORMAL_DAMAGE_REDUCTION	34
#define STATS_MAGIC_DAMAGE_REDUCTION	35
#define STATS_DAMAGERESIST	36
#define STATS_COLDRESIST	43
#define STATS_HPREGEN		74
#define STATS_ITEM_NORMALDAMAGE			111
#define STATS_ITEM_IGNORETARGETAC		115
#define STATS_ITEM_FRACTIONALTARGETAC	116
#define STATS_ITEM_PREVENTHEAL			117
#define STATS_ITEM_HALFFREEZEDURATION	118
#define STATS_ITEM_TOHIT_PERCENT		119
#define STATS_ITEM_DAMAGETARGETAC		120
#define STATS_ITEM_DEMONDAMAGE_PERCENT	121
#define STATS_ITEM_UNDEADDAMAGE_PERCENT	122
#define STATS_ITEM_DEMON_TOHIT			123
#define STATS_ITEM_UNDEAD_TOHIT			124
#define STATS_ITEM_DEADLYSTRIKE			141
#define STATS_ITEM_ABSORBFIRE_PERCENT	142
#define STATS_ITEM_ABSORBFIRE			143
#define STATS_ITEM_ABSORBLIGHT_PERCENT	144
#define STATS_ITEM_ABSORBLIGHT			145
#define STATS_ITEM_ABSORBMAGIC_PERCENT	146
#define STATS_ITEM_ABSORBMAGIC			147
#define STATS_ITEM_ABSORBCOLD_PERCENT	148
#define STATS_ITEM_ABSORBCOLD			149
#define STATS_ITEM_CANNOTBEFROZEN		153
#define UNIT_STATE_UNINTERRUPTABLE	54
#define STATE_1				1
#define STATE_2				2
#define STATE_A				10
#define STATE_B				11
#define STATE_E				14
#define STATE_10			16
#define STATE_15			21
#define STATE_1E			30
#define STATE_24			36
#define STATE_37			55
#define STATE_3A			58
#define STATE_40			64
#define STATE_41			65
#define STATE_42			66
#define STATE_44			68
#define STATE_49			73
#define STATE_58			88
#define STATE_5C			92
#define STATE_6B			107
#define STATE_85			133
#define STATE_89			137
#define STAT_4A				74
#define STAT_43				67
#define STAT_44				68
#define STAT_45				69
#define OVERLAY_94			148
#define OVERLAY_97			151
#define OVERLAY_98			152
#define EVENT_C				12
#define EVENT_RUN_AI		2
#define EVENT_3				3
#define SKILL_41			65
#define SKILL_D				13
#define SKILL_12			18
#define SKILL_9				9
#define SKILL_1D			29
#define MONSTER_122			290
#define MONSTER_9C			156
#define MONSTER_D3			211
#define MONSTER_F2			242
#define MONSTER_F3			243
#define MONSTER_LAST		410
#define MONTYPE_57			87
#define MONREGION_13		19
#define UNIT_MASK_PET		0x80000000
#define UNITFLAG0_8000		32768
#define DAMAGE0_NOROLL		1
#define DAMAGE0_HELD		2
#define DAMAGE0_ANY			4096
#define DAMAGE_HIT			1
#define DAMAGE_KILL			2
#define DAMAGE_GETHIT		4
#define DAMAGE_KNOCKBACK	8
#define DAMAGE_BLOCK		16
#define DAMAGE_40			64
#define DAMAGE_SKILLD		128
#define DAMAGE_SKILL12		256
#define DAMAGE_SKILL1D		512
#define DAMAGE_400			1024
#define DAMAGE_800			2048
#define DAMAGE_DOUBLED		8192
#define DAMAGE_NOINTERRUPT	16384
#define DAMAGE_DODGED		896
#define SKILLHIT_STOP		2

void __fastcall PlrEvalLevelUp(UNIT *hPlayer, GAME *ptGame);

static BYTE sgbDamagePick;

struct DAMAGERESISTCHART
{
 	int			nOffset;
 	int			nResistStat;
 	int			nMaxResistStat;
 	int			nReductionIndex;
 	int			nMin;
 	BOOL		bMonsterOnly;
 	char		szName[8];
 	DWORD		field_20;
};
typedef struct DAMAGERESISTCHART DAMAGERESISTCHART;

static const DAMAGERESISTCHART sgatResistChart[12] =
{
	{ 8, 36, -1,   1, 0, 0, "Dam",  1 },
	{ 16, 39, 40, 2, 0, 0, "Fire", 1 },
	{ 20, 41, 42, 2, 0, 0, "Ligt", 1 },
	{ 28, 43, 44, 2, 0, 0, "Cold", 1 },
	{ 24, 37, 38, 2, 0, 0, "Magc", 1 },
	{ 32, 45, 46, 0, 0, 0, "Pois", 0 },
	{ 48, -1,   -1,   2, 0, 1, "Life", 1 },
	{ 52, -1,   -1,   2, 0, 1, "Mana", 1 },
	{ 56, -1,   -1,   2, 0, 1, "Stam", 1 },
	{ 40, 43, 44, 0, 0, 0, "CLen", 0 },
	{ 44, 43, 44, 0, 0, 0, "FLen", 0 },
	{ 36, 110, -1,   0, 0, 0, "PLen", 0 },
};

struct DURBODY
{
 	int			nLoc;
 	int			nChance;
};
typedef struct DURBODY DURBODY;

static DURBODY sgatDurBodyTbl[7] =
{
	{ 1, 3 }, { 3, 2 }, { 4, 2 }, { 5, 2 }, { 8, 4 }, { 9, 4 }, { 10, 3 },
};
static DWORD snDurBodyTblSize = 7;
static int sganNoWearSkills[2] = { 97, 1 };
static DWORD sgnNoWearSkills = 2;

struct DAMAGEPICK
{
 	int			nDamage;
 	DWORD		dwFlag;
};
typedef struct DAMAGEPICK DAMAGEPICK;

struct PARTYEXP
{
 	DWORD		dwLevelId;
 	UNIT		*hPlayer;
 	int			nExp;
 	int			nLevelSum;
 	int			nMonLevel;
};
typedef struct PARTYEXP PARTYEXP;

BOOL __fastcall SUnitDmgSetFlag(DAMAGE *ptDamage, DWORD dwFlag)
{
	if (ptDamage->dwHitClass & 240)
		return FALSE;
	ptDamage->dwHitClass |= dwFlag;
	return TRUE;
}

static int SUnitDmgRollAdd(UNIT *hUnit, int nMin, int nMax,
	int nPercent, int nAdd)
{
	int nTemp;
	int nRange;
	int nRoll;
	int nDamage;

	if (!nMax)
		return nAdd;
	if (nMax < nMin)
	{
		nTemp = nMin;
		nMin = nMax;
		nMax = nTemp;
	}
	nRange = nMax - nMin;
	if (nRange)
		nRoll = (int)D2CommonSeedRand(&hUnit->tSeed, nRange);
	else
		nRoll = 0;
	nDamage = (nRoll + nMin) * (nPercent + 100) / 100 + nAdd;
	if (nDamage < 0)
		nDamage = 0;
	return nDamage;
}

int __fastcall SUnitDmgRollDamage(UNIT *hUnit, int nMin,
	int nMax, int nPercent, int nAdd)
{
	UNIT *hWeapon;
	int nDamage;

	if (nMin < 1)
		nMin = 256;
	if (nMax <= nMin)
		nMax = nMin + 256;
	nPercent += (DWORD)D2CommonStatsUnitGetValue(hUnit, STATS_DAMAGEPERCENT);
	hWeapon = (UNIT *)D2CommonUnitGetActiveWeapon(hUnit);
	if (hWeapon)
	{
		if (D2CommonITEMSGetTypeStrBonus(hWeapon))
			nPercent += (DWORD)D2CommonStatsUnitGetValue(hUnit, STATS_STRENGTH);
		if (D2CommonITEMSGetTypeDexBonus(hWeapon))
			nPercent += (DWORD)D2CommonStatsUnitGetValue(hUnit, STATS_DEXTERITY);
	}
	nDamage = SUnitDmgRollAdd(hUnit, nMin, nMax, nPercent, nAdd);
	return (int)(D2CommonStatsGetTotalValue(hUnit, STATS_ITEM_NORMALDAMAGE) << 8) + nDamage;
}

static int SUnitDmgRollStat(UNIT *hUnit, int nAdd, int nMaxStat,
	int nMinStat)
{
	int nMin;
	int nMax;

	nMax = (int)((DWORD)D2CommonStatsUnitGetValue(hUnit, nMaxStat) << 8);
	nMin = (int)((DWORD)D2CommonStatsUnitGetValue(hUnit, nMinStat) << 8);
	return SUnitDmgRollAdd(hUnit, nMin, nMax, 0, nAdd);
}

static __inline int SUnitDmgColdResist(UNIT *hAttacker, int nResist)
{
	int nPercent;
	DWORD eType = hAttacker->eType;

	if (nResist > 0 && (eType == UNIT_PLAYER || eType == UNIT_MONSTER)
		&& D2CommonStatsGetState(hAttacker, STATE_49))
	{
		nPercent = D2CommonSkillsCalcCurveParamOne(D2CommonSkillsCalcNodeLevel(hAttacker,
			D2CommonSkillsFindByKey(hAttacker, SKILL_41), 1), SKILL_41);
		nResist -= nPercent * nResist / 100;
	}
	return nResist;
}

static __inline void SUnitDmgAbsorb(UNIT *hTarget, DAMAGE *ptDamage, int *pnDamage,
	int nPercentStat, int nFlatStat)
{
	int nPercent;
	int nAbsorb;

	nPercent = D2CommonStatsGetTotalValue(hTarget, nPercentStat);
	if (nPercent)
	{
		nAbsorb = *pnDamage * nPercent / 100;
		ptDamage->nAbsorbLife += nAbsorb;
		*pnDamage -= nAbsorb;
	}
	nAbsorb = D2CommonStatsGetTotalValue(hTarget, nFlatStat);
	if (nAbsorb)
	{
		nAbsorb <<= 8;
		if (nAbsorb >= *pnDamage)
			nAbsorb = *pnDamage;
		ptDamage->nAbsorbLife += nAbsorb;
		*pnDamage -= nAbsorb;
	}
}

void __fastcall SUnitDmgResist(UNIT *hAttacker, UNIT *hTarget,
	DAMAGE *ptDamage)
{
	const DAMAGERESISTCHART *pctChart;
	int nReductionSize;
	int anReduction[3];
	int nNormalReduction;
	int nMagicReduction;
	BOOL bMonster;
	int *pnValue;
	int nReduce;
	int nResist;
	int nCap;
	int nOld;
	int nNew;
	int nPrev;
	int i;

	nNormalReduction = (DWORD)D2CommonStatsUnitGetValue(hTarget, STATS_NORMAL_DAMAGE_REDUCTION) << 8;
	nMagicReduction = (DWORD)D2CommonStatsUnitGetValue(hTarget, STATS_MAGIC_DAMAGE_REDUCTION) << 8;
	if (D2CommonStatsGetTotalValue(hTarget, STATS_ITEM_CANNOTBEFROZEN))
	{
		ptDamage->nColdLength = 0;
		ptDamage->nFreezeLength = 0;
	}
	else if (D2CommonStatsGetTotalValue(hTarget, STATS_ITEM_HALFFREEZEDURATION))
	{
		ptDamage->nColdLength /= 2;
		ptDamage->nFreezeLength /= 2;
	}
	if (D2CommonStatsGetState(hTarget, STATE_85))
		ptDamage->nPoisonLength = 0;

	nReductionSize = 3;
	anReduction[0] = 0;
	anReduction[1] = nNormalReduction;
	anReduction[2] = nMagicReduction;
	bMonster = hAttacker->eType == UNIT_MONSTER;
	for (i = 0, pctChart = sgatResistChart; i < 12; i++, pctChart++)
	{
		if (!bMonster && pctChart->bMonsterOnly)
			continue;
		{
		int nResistStat;
		int nMaxResistStat;

		pnValue = (int *)((BYTE *)ptDamage + pctChart->nOffset);
		if (!*pnValue)
			continue;
#line 847
		D2ASSERT(pctChart->nReductionIndex < nReductionSize && pctChart->nReductionIndex >= 0);
		nReduce = anReduction[pctChart->nReductionIndex];
		nMaxResistStat = pctChart->nMaxResistStat;
		nResistStat = pctChart->nResistStat;
		if (nResistStat == STATS_NONE)
			nResist = 0;
		else
		{
			nResist = D2CommonStatsUnitGetValue(hTarget, nResistStat);
			if (nResistStat == STATS_COLDRESIST)
				nResist = SUnitDmgColdResist(hAttacker, nResist);
		}
		if (hTarget->eType == UNIT_PLAYER && nResistStat != STATS_DAMAGERESIST)
			nResist += ((MONDIFFICULTYTXT *)D2CommonDataTblsGetDifficultyLevel(
				SUnitGetGame(hTarget)->bDifficultyLevel))->ResistPenalty;
		if (!nResist)
			nCap = 0;
		else
		{
			if (nMaxResistStat == STATS_NONE)
				nMaxResistStat = 75;
			else
				nMaxResistStat = D2CommonStatsUnitGetValue(hTarget, nMaxResistStat);
			nMaxResistStat += 75;
			nCap = nMaxResistStat;
			if (nMaxResistStat >= 95)
				nCap = 95;
			if (hTarget->eType == UNIT_MONSTER && D2CommonMonsterUnitGetTypeLocalIndex(hTarget) == MONTYPE_57
				&& nResistStat == STATS_DAMAGERESIST)
				nCap = 99;
			if (nResist < nCap)
				nCap = nResist;
		}
		nOld = *pnValue;
		nNew = *pnValue;
		if (nNew)
		{
			do
			{
				nNew = *pnValue;
				if (nCap)
					nNew = (100 - nCap) * nNew / 100;
				if (nNew > 0)
				{
					int nMin = pctChart->nMin;

					nNew -= nReduce;
					if (nNew < nMin)
						nNew = nMin;
				}
			} while (0);
		}
		DebugResist(SUnitGetGame(hAttacker), hAttacker, hTarget, nOld, nCap, nReduce, nNew, pctChart->szName,
			pctChart->field_20);
		*pnValue = nNew;
		}
	}

	nPrev = ptDamage->nFireDamage;
	SUnitDmgAbsorb(hTarget, ptDamage, &ptDamage->nFireDamage, STATS_ITEM_ABSORBFIRE_PERCENT,
		STATS_ITEM_ABSORBFIRE);
	DebugUnitValue(SUnitGetGame(hAttacker), hAttacker, hTarget, nPrev - ptDamage->nFireDamage, (DWORD)"fire");
	nPrev = ptDamage->nLightningDamage;
	SUnitDmgAbsorb(hTarget, ptDamage, &ptDamage->nLightningDamage, STATS_ITEM_ABSORBLIGHT_PERCENT,
		STATS_ITEM_ABSORBLIGHT);
	DebugUnitValue(SUnitGetGame(hAttacker), hAttacker, hTarget, nPrev - ptDamage->nLightningDamage, (DWORD)"ltng");
	nPrev = ptDamage->nColdDamage;
	SUnitDmgAbsorb(hTarget, ptDamage, &ptDamage->nColdDamage, STATS_ITEM_ABSORBCOLD_PERCENT,
		STATS_ITEM_ABSORBCOLD);
	DebugUnitValue(SUnitGetGame(hAttacker), hAttacker, hTarget, nPrev - ptDamage->nColdDamage, (DWORD)"cold");
	nPrev = ptDamage->nMagicDamage;
	SUnitDmgAbsorb(hTarget, ptDamage, &ptDamage->nMagicDamage, STATS_ITEM_ABSORBMAGIC_PERCENT,
		STATS_ITEM_ABSORBMAGIC);
	DebugUnitValue(SUnitGetGame(hAttacker), hAttacker, hTarget, nPrev - ptDamage->nMagicDamage, (DWORD)"magic");

	if (D2CommonStatsGetState(hTarget, STATE_E))
		SkillsBoneArmorAbsorb(hTarget, ptDamage);
	ptDamage->nTotalDamage = ptDamage->nPhysicalDamage + ptDamage->nFireDamage + ptDamage->nPoisonDamage
		+ ptDamage->nColdDamage
		+ ptDamage->nMagicDamage + ptDamage->nLightningDamage;
	if (hAttacker->eType == UNIT_MONSTER)
		ptDamage->nTotalDamage += ptDamage->nLifeDrain;
	if (hAttacker && hTarget && hAttacker != hTarget)
	{
		if (hTarget->eType == UNIT_PLAYER
			&& (hAttacker->eType == UNIT_PLAYER || (hAttacker->dwFlags0 & UNIT_MASK_PET)))
		{
			ptDamage->nTotalDamage /= 2;
			ptDamage->nPoisonDamage /= 2;
		}
		if (hAttacker->eType == UNIT_MONSTER && (hTarget->dwFlags0 & UNIT_MASK_PET))
		{
			switch (hAttacker->dwClassId)
			{
			case MONSTER_9C:
			case MONSTER_D3:
			case MONSTER_F2:
			case MONSTER_F3:
				ptDamage->nTotalDamage *= 10;
				ptDamage->nPoisonDamage *= 10;
				break;
			}
		}
	}
	if (D2CommonStatsGetState(hTarget, STATE_1E))
		SkillsEnergyShieldAbsorb(hAttacker, hTarget, ptDamage);
}
void __fastcall SUnitDmgApply(GAME *ptGame, UNIT *hAttacker,
	UNIT *hTarget  , BOOL bResist,
	DAMAGE *ptDamage)
{
	MONDIFFICULTYTXT *ptDifficultyLevel;
	void *ptStatList;
	GAME *ptUnitGame;
	DWORD eTargetType;
	DWORD eAttackerType;
	int nLifeBefore;
	int nMax;
	int nValue;
	int nDamage;
	int nLength;
	int nFreeze;
	int nEnd;
	int nAmount;

	if (!SUnitIsHostile(ptGame, hAttacker, hTarget) && !(ptDamage->dwFlags0 & DAMAGE0_ANY))
	{
		ptDamage->wFlags &= ~(DAMAGE_HIT | DAMAGE_KILL);
		return;
	}
	eAttackerType = hAttacker->eType;
	eTargetType = hTarget->eType;
	if (eTargetType == UNIT_MONSTER
		&& !((MONSTATSTXT *)D2CommonMonsterTblsGetStats(hTarget->dwClassId))->bKillable)
		return;
	if (D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hTarget)))
	{
		DAMAGE *ptTownDamage = ptDamage;
		ptTownDamage->wFlags &= ~DAMAGE_KILL;
		return;
	}
	if (SUnitIsDead(hTarget))
		return;
	nLifeBefore = D2CommonStatsUnitGetValue(hTarget, STATS_HITPOINTS);
	D2CommonUnitSetSecondLinkedUnit(hTarget, hAttacker);
	if (eAttackerType == UNIT_MONSTER && (ptDamage->wFlags & DAMAGE_HIT))
		MonsterUniqueOnHit(ptGame, hAttacker);
	if (bResist)
		SUnitDmgResist(hAttacker, hTarget, ptDamage);

	nAmount = ptDamage->nAbsorbLife;
	if (nAmount > 0)
	{
		nMax = D2CommonStatsUnitCalcMaxHp(hTarget);
		nValue = (DWORD)D2CommonStatsUnitGetValue(hTarget, STATS_HITPOINTS) + nAmount;
		if (nValue >= nMax)
			nValue = nMax;
		D2CommonStatsUnitSetValue(hTarget, STATS_HITPOINTS, nValue);
	}
	nDamage = ptDamage->nTotalDamage;
	if (hTarget->eType == UNIT_MONSTER && hTarget->dwClassId == MONSTER_122)
		nDamage = SkillsBloodGolemShare(hTarget, nDamage);
	if (nDamage > 0)
	{
		nValue = (DWORD)D2CommonStatsUnitGetValue(hTarget, STATS_HITPOINTS) - nDamage;
		if (nValue < 256)
			nValue = 0;
		D2CommonStatsUnitSetValue(hTarget, STATS_HITPOINTS, nValue);
	}
	nAmount = ptDamage->nManaDrain;
	if (nAmount > 0)
	{
		nValue = (DWORD)D2CommonStatsUnitGetValue(hTarget, STATS_MANA) - nAmount;
		if (nValue < 256)
			nValue = 0;
		D2CommonStatsUnitSetValue(hTarget, STATS_MANA, nValue);
	}
	nAmount = ptDamage->nStaminaDrain;
	if (nAmount > 0)
	{
		nValue = (DWORD)D2CommonStatsUnitGetValue(hTarget, STATS_STAMINA) - nAmount;
		if (nValue < 0 || nValue < 256)
			nValue = 0;
		D2CommonStatsUnitSetValue(hTarget, STATS_STAMINA, nValue);
	}

	if (ptDamage->nStunLength > 0)
	{
		nLength = ptDamage->nStunLength;
		if (hTarget->eType == UNIT_MONSTER)
		{
			if (MonsterUniqueIsCounted(((MONSTERDATA *)hTarget->uTypeData)->ptUnique)
				&& !(D2CommonSeedNext(&hAttacker->tSeed) & 1))
				goto chill;
			switch (hTarget->dwClassId)
			{
			case MONSTER_9C:
			case MONSTER_D3:
			case MONSTER_F2:
			case MONSTER_F3:
				goto chill;
			}
		}
		if (nLength > 0)
		{
			void *ptStunList;
			if (nLength > 250)
				nLength = 250;
			D2CommonUnitAddAnimOverlay(hTarget, OVERLAY_94, nLength);
			ptStunList = (void *)D2CommonStatsFindListByState(hTarget, STATE_15);
			ptUnitGame = SUnitGetGame(hAttacker);
			nEnd = ptUnitGame->nFrame + nLength;
			if (!(ptStunList))
			{
				ptStunList = (void *)D2CommonStatsAllocList(ptUnitGame->pMemPool, 2, nEnd,
					hAttacker->eType, hAttacker->dwId);
				EventAdd(ptUnitGame, hTarget, EVENT_C, nEnd);
				D2CommonStatsSetListState(ptStunList, STATE_15);
				D2CommonStatsSetListRemoveCallback(ptStunList, SkillsStateRemove);
				D2CommonStatsUnitLinkList(hTarget, ptStunList);
				D2CommonStatsUnitSetActiveStateBit(hTarget, STATE_15, 1);
			}
			else
			{
				D2CommonStatsSetListEndFrameSecond(ptStunList, nEnd);
				EventAdd(ptUnitGame, hTarget, EVENT_C, nEnd);
			}
		}
	}
chill:
	if (ptDamage->nColdLength > 0)
		SUnitDmgChill(hAttacker, hTarget, ptDamage->nColdLength);

	nEnd = ptDamage->nFreezeLength;
	if (nEnd > 0)
	{
		nFreeze = nEnd;
		switch (hTarget->eType)
		{
		case UNIT_PLAYER:
			SUnitDmgChill(hAttacker, hTarget, nFreeze);
			goto poison;
		case UNIT_MONSTER:
			if (D2CommonStatsGetState(hTarget, UNIT_STATE_UNINTERRUPTABLE))
				goto poison;
			if (D2CommonMonsterRegionUnitIsBossRaw(D2CommonMonsterTblsGetStats(hTarget->dwClassId),
				(DWORD)hTarget) || MonsterUniqueIsCounted(((MONSTERDATA *)hTarget->uTypeData)->ptUnique))
			{
				SUnitDmgChill(hAttacker, hTarget, nFreeze);
				goto poison;
			}
			break;
		}
		ptStatList = (void *)D2CommonStatsFindListByState(hTarget, STATE_1);
		ptUnitGame = SUnitGetGame(hAttacker);
		if (hTarget->eType == UNIT_MONSTER)
		{
			ptDifficultyLevel = (MONDIFFICULTYTXT *)D2CommonDataTblsGetDifficultyLevel(
				ptUnitGame->bDifficultyLevel);
#line 462
			D2ASSERT(ptDifficultyLevel->nMonsterFreezeDivisor);
			nFreeze /= ptDifficultyLevel->nMonsterFreezeDivisor;
			nEnd = nFreeze;
		}
		nEnd += ptUnitGame->nFrame;
		if (!ptStatList)
		{
			D2CommonStatsUnitSetActiveStateBit(hTarget, STATE_1, 1);
			ptStatList = (void *)D2CommonStatsAllocList(ptUnitGame->pMemPool, 2, nEnd, hAttacker->eType,
				hAttacker->dwId);
			D2CommonStatsSetListState(ptStatList, STATE_1);
			D2CommonStatsSetListRemoveCallback(ptStatList, SkillsStateRemove);
			D2CommonStatsUnitLinkList(hTarget, ptStatList);
		}
		else
		{
			nValue = D2CommonStatsGetGlobalListEndFrame(ptStatList);
			if (nValue < nEnd)
				D2CommonStatsSetListEndFrameSecond(ptStatList, nEnd);
			else
				nEnd = nValue;
		}
		EventAdd(ptUnitGame, hTarget, EVENT_C, nEnd);
#line 493
		D2WARN(UnitGetType (hTarget) == UNIT_MONSTER);
		EventRemoveByType(ptUnitGame, hTarget, EVENT_RUN_AI);
		EventAdd(ptUnitGame, hTarget, EVENT_RUN_AI, nFreeze + ptUnitGame->nFrame + 1);
	}
poison:
	if (ptDamage->nPoisonLength > 0)
		SUnitDmgPoison(hAttacker, hTarget, ptDamage->nPoisonDamage, ptDamage->nPoisonLength);

	if (D2CommonStatsUnitGetValue(hTarget, STATS_HITPOINTS) > 0)
	{
		ptDamage->wFlags &= ~DAMAGE_KILL;
		if (ptDamage->nTotalDamage && eTargetType == UNIT_MONSTER)
		{
			EventRemoveByType(ptGame, hTarget, EVENT_3);
			EventAddPersistent(ptGame, hTarget, EVENT_3);
		}
	}
	else
		ptDamage->wFlags |= DAMAGE_KILL;
	SUnitDmgLeech(hAttacker, hTarget, nLifeBefore, ptDamage);
	if (eAttackerType == UNIT_OBJECT)
		return;
	if (ptDamage->wFlags & DAMAGE_HIT)
		SkillsItemApplyPassiveMods(ptGame, hAttacker, hTarget, 3, ptDamage, 0, !bResist);
	if (ptDamage->wFlags & DAMAGE_KILL)
		SkillsItemApplyPassiveMods(ptGame, hAttacker, hTarget, 5, NULL, 0, !bResist);
}

void __fastcall SUnitDmgPoison(UNIT *hAttacker, UNIT *hTarget,
	int nDamage, int nLength)
{
	void *ptStatList;
	GAME *ptGame;
	int nEnd;

	if (!nLength)
		return;
	ptStatList = (void *)D2CommonStatsFindListByState(hTarget, STATE_2);
	ptGame = SUnitGetGame(hAttacker);
	if (hTarget->eType == UNIT_MONSTER)
	{
		EventRemoveByType(ptGame, hTarget, EVENT_3);
		EventAddPersistent(ptGame, hTarget, EVENT_3);
	}
	nEnd = ptGame->nFrame + nLength;
	if (!ptStatList)
	{
		D2CommonStatsUnitSetActiveStateBit(hTarget, STATE_2, 1);
		ptStatList = (void *)D2CommonStatsAllocList(ptGame->pMemPool, 2, nEnd, hAttacker->eType,
			hAttacker->dwId);
		EventAdd(ptGame, hTarget, EVENT_C, nEnd);
		D2CommonStatsListSetValueA(ptStatList, STAT_4A, -nDamage);
		D2CommonStatsSetListState(ptStatList, STATE_2);
		D2CommonStatsSetListRemoveCallback(ptStatList, SkillsStateRemove);
		D2CommonStatsUnitLinkList(hTarget, ptStatList);
		return;
	}
	if (-(int)D2CommonStatsListGetValueA(ptStatList, STAT_4A) > nDamage)
		return;
	D2CommonStatsSetListEndFrame(ptStatList, nEnd);
	EventAdd(ptGame, hTarget, EVENT_C, nEnd);
	D2CommonStatsListSetValueB(ptStatList, STAT_4A, -nDamage);
}

void __fastcall SUnitDmgChill(UNIT *hAttacker, UNIT *hTarget,
	int nLength)
{
	MONDIFFICULTYTXT *ptDifficultyLevel;
	void *ptStatList;
	GAME *ptGame;
	int nEnd;

	ptStatList = (void *)D2CommonStatsFindListByState(hTarget, STATE_B);
	ptGame = SUnitGetGame(hAttacker);
	if (hTarget->eType == UNIT_MONSTER)
	{
		ptDifficultyLevel = (MONDIFFICULTYTXT *)D2CommonDataTblsGetDifficultyLevel(ptGame->bDifficultyLevel);
#line 388
		D2ASSERT(ptDifficultyLevel->nMonsterColdDivisor);
		nLength /= ptDifficultyLevel->nMonsterColdDivisor;
	}
	nEnd = ptGame->nFrame + nLength;
	if (!ptStatList)
	{
		D2CommonStatsUnitSetActiveStateBit(hTarget, STATE_B, 1);
		ptStatList = (void *)D2CommonStatsAllocList(ptGame->pMemPool, 2, nEnd, hAttacker->eType,
			hAttacker->dwId);
		EventAdd(ptGame, hTarget, EVENT_C, nEnd);
		D2CommonStatsSetListState(ptStatList, STATE_B);
		D2CommonStatsSetListRemoveCallback(ptStatList, SkillsHolyFreezeEnd);
		D2CommonStatsUnitLinkList(hTarget, ptStatList);
		D2CommonStatsListSetValueA(ptStatList, STAT_43, -50);
		D2CommonStatsListSetValueA(ptStatList, STAT_44, -50);
		D2CommonStatsListSetValueA(ptStatList, STAT_45, -50);
		D2CommonUnitUpdateAnimRate(hTarget);
	}
	else if ((int)D2CommonStatsGetGlobalListEndFrame(ptStatList) < nEnd)
	{
		D2CommonStatsSetListEndFrameSecond(ptStatList, nEnd);
		EventAdd(ptGame, hTarget, EVENT_C, nEnd);
	}
	if (D2CommonSeedRand(&hTarget->tSeed, 100) < 20)
		D2CommonStatsUnitSetActiveStateBit(hTarget, STATE_6B, 1);
	else
		D2CommonStatsUnitSetActiveStateBit(hTarget, STATE_6B, 0);
}

void __fastcall SUnitDmgLeech(UNIT *hAttacker, UNIT *hTarget,
	int nLife, DAMAGE *ptDamage)
{
	int nMana;
	int nStamina;
	int nLeech;
	int nMax;
	int nValue;

	switch (hAttacker->eType)
	{
	case UNIT_PLAYER:
		if (ptDamage->nPhysicalDamage < nLife)
			nLife = ptDamage->nPhysicalDamage;
		if (nLife <= 0)
			return;
		if (ptDamage->nManaDrain)
		{
			nLeech = ptDamage->nManaDrain * nLife / 100;
			if (nLeech > 0)
			{
				nMax = D2CommonStatsUnitCalcMaxMana(hAttacker);
				nValue = (DWORD)D2CommonStatsUnitGetValue(hAttacker, STATS_MANA) + nLeech;
				if (nValue >= nMax)
					nValue = nMax;
				D2CommonStatsUnitSetValue(hAttacker, STATS_MANA, nValue);
			}
			D2CommonUnitAddAnimOverlay(hAttacker, OVERLAY_98, 0);
		}
		if (ptDamage->nLifeDrain)
		{
			nLeech = ptDamage->nLifeDrain * nLife / 100;
			if (nLeech > 0)
			{
				nMax = D2CommonStatsUnitCalcMaxHp(hAttacker);
				nValue = (DWORD)D2CommonStatsUnitGetValue(hAttacker, STATS_HITPOINTS) + nLeech;
				if (nValue >= nMax)
					nValue = nMax;
				D2CommonStatsUnitSetValue(hAttacker, STATS_HITPOINTS, nValue);
			}
			D2CommonUnitAddAnimOverlay(hAttacker, OVERLAY_97, 0);
		}
		break;
	case UNIT_MONSTER:
		if (hAttacker->dwClassId == MONSTER_122)
			SkillsBloodGolemLife(hAttacker, hTarget, nLife);
		nMana = D2CommonStatsUnitGetValue(hTarget, STATS_MANA);
		nStamina = D2CommonStatsUnitGetValue(hTarget, STATS_STAMINA);
		nLeech = 0;
		nLeech += ptDamage->nLifeDrain < nLife ? ptDamage->nLifeDrain : nLife;
		nLeech += ptDamage->nManaDrain < nMana ? ptDamage->nManaDrain : nMana;
		nLeech += ptDamage->nStaminaDrain < nStamina ? ptDamage->nStaminaDrain : nStamina;
		if (!nLeech)
			return;
		nMax = D2CommonStatsUnitCalcMaxHp(hAttacker);
		nMax -= (DWORD)D2CommonStatsUnitGetValue(hAttacker, STATS_HITPOINTS);
		if (nLeech >= nMax)
			nLeech = nMax;
		if (nLeech <= 0)
			return;
		D2CommonStatsUnitAddValue(hAttacker, STATS_HITPOINTS, nLeech);
		D2CommonUnitAddAnimOverlay(hAttacker, OVERLAY_97, 0);
		break;
	}
}

void __fastcall SUnitDmgFreeHeld(GAME *ptGame, UNIT *hUnit)
{
	DAMAGEHOLD *ptHold;
	DAMAGEHOLD *ptNext;
	DAMAGEHOLD *ptPrev;
	DAMAGEHOLD *ptNode;
	DAMAGEHOLD *ptNodePrev;
	UNIT *hTarget;

	ptHold = hUnit->ptDamageHold;
	if (!ptHold)
		return;
	ptPrev = NULL;
	do
	{
		ptNext = ptHold->ptNext;
		if (ptHold->eType == hUnit->eType && ptHold->dwUnitId == hUnit->dwId)
		{
			hTarget = SUnitFind(ptHold->ptGame, ptHold->eTargetType, ptHold->dwTargetId);
			if (hTarget)
			{
				ptNodePrev = NULL;
				for (ptNode = hTarget->ptDamageHold; ptNode; ptNode = ptNode->ptTargetNext)
				{
					if (ptNode->eTargetType == hTarget->eType && ptNode->dwTargetId == hTarget->dwId
						&& ptNode->eType == hUnit->eType && ptNode->dwUnitId == hUnit->dwId)
					{
						if (!ptNodePrev)
							hTarget->ptDamageHold = ptNode->ptTargetNext;
						else
							ptNodePrev->ptTargetNext = ptNode->ptTargetNext;
					}
					else
						ptNodePrev = ptNode;
				}
			}
			if (!ptPrev)
				hUnit->ptDamageHold = ptHold->ptNext;
			else
				ptPrev->ptNext = ptHold->ptNext;
#line 1124
			D2FREEPOOL(ptGame->pMemPool, ptHold);
		}
		else
			ptPrev = ptHold;
		ptHold = ptNext;
	} while (ptHold);
}

void __fastcall SUnitDmgFreeHold(GAME *ptGame, UNIT *hUnit,
	UNIT *hTarget)
{
	DAMAGEHOLD *ptHold;
	DAMAGEHOLD *ptNext;
	DAMAGEHOLD *ptPrev;
	DAMAGEHOLD *ptNode;
	DAMAGEHOLD *ptNodePrev;

	ptHold = hUnit->ptDamageHold;
	if (!ptHold)
		return;
	ptPrev = NULL;
	do
	{
		ptNext = ptHold->ptNext;
		if (ptHold->eType == hUnit->eType && ptHold->dwUnitId == hUnit->dwId
			&& ptHold->eTargetType == hTarget->eType && ptHold->dwTargetId == hTarget->dwId)
		{
			ptNodePrev = NULL;
			for (ptNode = hTarget->ptDamageHold; ptNode; ptNode = ptNode->ptTargetNext)
			{
				if (ptNode->eTargetType == hTarget->eType && ptNode->dwTargetId == hTarget->dwId
					&& ptNode->eType == hUnit->eType && ptNode->dwUnitId == hUnit->dwId)
				{
					if (!ptNodePrev)
						hTarget->ptDamageHold = ptNode->ptTargetNext;
					else
						ptNodePrev->ptTargetNext = ptNode->ptTargetNext;
				}
				else
					ptNodePrev = ptNode;
			}
			if (!ptPrev)
				hUnit->ptDamageHold = ptHold->ptNext;
			else
				ptPrev->ptNext = ptHold->ptNext;
#line 1168
			D2FREEPOOL(ptGame->pMemPool, ptHold);
		}
		else
			ptPrev = ptHold;
		ptHold = ptNext;
	} while (ptHold);
}

extern BOOL __fastcall SUnitDmgTestStateKept(UNIT *hUnit, int nShift);
__inline BOOL __fastcall SUnitDmgTestStateKept(UNIT *hUnit, int nShift)
{
	return D2CommonStatsUnitTestStateKept(hUnit, nShift);
}

void __fastcall SUnitDmgKill(GAME *ptGame, UNIT *hUnit,
	UNIT *hKiller, BOOL bRemovePet)
{
	MONMODECHANGE tModeChange;
	UNIT *hOwner;
	DWORD eKillerType;
	DWORD dwX;
	DWORD dwY;
	DWORD bDirection;

	switch (hUnit->eType)
	{
	case UNIT_PLAYER:
		if (!hUnit->dwMode || hUnit->dwMode == PLRMODE_DEAD)
			return;
		break;
	case UNIT_MONSTER:
		if (!hUnit->dwMode || hUnit->dwMode == MONMODE_DEAD)
			return;
		hOwner = AiGetOwner(hUnit);
		if (bRemovePet && hOwner && hOwner->eType == UNIT_PLAYER)
			PlayerPetsRemove(ptGame, hOwner, hUnit->dwId, FALSE);
		if (!((MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId))->bKillable)
			return;
		break;
	default:
		return;
	}
	if (hKiller)
		bDirection = ((BYTE (__stdcall *)(DWORD, DWORD, DWORD))D2CommonOrdinal10413)((DWORD)hUnit, D2CommonUnitGetX(hKiller), D2CommonUnitGetY(hKiller));
	else
		bDirection = ((BYTE (__stdcall *)(DWORD))D2CommonOrdinal10156)((DWORD)hUnit->ptPath);
	if (hKiller)
	{
		eKillerType = hKiller->eType;
		MonsterEvalKilled(hKiller, hUnit, ptGame);
		if ((int)hUnit->dwClassId < MONSTER_LAST && eKillerType == UNIT_PLAYER)
		{
			D2CommonMonsterKillsLogIncrement(hKiller, hUnit->dwClassId);
			hOwner = AiGetOwner(hUnit);
			if (hOwner && hOwner->eType == UNIT_PLAYER)
				PlayerPetsRemove(ptGame, hOwner, hUnit->dwId, FALSE);
		}
	}
	if (hUnit->eType != UNIT_MONSTER)
		return;
	MonsterModeInit(hUnit, MONMODE_DEATH, &tModeChange);
	tModeChange.bDirection = bDirection;
	tModeChange.hTarget = hKiller;
	MonsterModeSet(ptGame, &tModeChange, TRUE);
	if (!(hUnit->dwFlags0 & UNIT_MASK_PET))
		QuestsUnitKilled(ptGame, hUnit, hKiller);
}

void __fastcall SUnitDmgReact(GAME *ptGame, UNIT *hAttacker,
	UNIT *hEnemy  , DAMAGE *ptDamage)
{
	DAMAGEPICK atPick[8];
	MONMODECHANGE tModeChange;
	void *hSkill;
	DWORD dwFlag;
	WORD wFlags;
	int nStart;
	int nDamage;
	int i;

	if (ptDamage->wFlags & DAMAGE_HIT)
	{
		if (!ptDamage->dwFixedHitClass && !(ptDamage->dwHitClass & 240))
		{
			atPick[0].nDamage = ptDamage->nColdDamage;
			atPick[0].dwFlag = 48;
			atPick[1].nDamage = ptDamage->nFireDamage;
			atPick[1].dwFlag = 32;
			atPick[2].nDamage = ptDamage->nLightningDamage;
			atPick[2].dwFlag = 64;
			atPick[3].nDamage = ptDamage->nPoisonDamage;
			atPick[3].dwFlag = 80;
			atPick[4].nDamage = ptDamage->nColdDamage;
			atPick[4].dwFlag = 48;
			atPick[5].nDamage = ptDamage->nFireDamage;
			atPick[5].dwFlag = 32;
			atPick[6].nDamage = ptDamage->nLightningDamage;
			atPick[6].dwFlag = 64;
			atPick[7].nDamage = ptDamage->nPoisonDamage;
			atPick[7].dwFlag = 80;
			nStart = sgbDamagePick % 4;
			sgbDamagePick++;
			for (i = 0; i < 4; i++)
			{
				if (atPick[nStart + i].nDamage > 0)
				{
					dwFlag = atPick[nStart + i].dwFlag;
					goto lPicked;
				}
			}
			dwFlag = (ptDamage->wFlags >> 9) & 16;
lPicked:
			ptDamage->dwHitClass |= dwFlag;
		}
		hEnemy->dwHitClass = ptDamage->dwHitClass;
	}
	if (D2CommonStatsGetState(hEnemy, UNIT_STATE_UNINTERRUPTABLE))
	{
		if (ptDamage->wFlags & DAMAGE_KILL)
			D2CommonStatsUnitSetActiveStateBit(hEnemy, STATE_5C, 1);
		return;
	}
	switch (hEnemy->eType)
	{
	case UNIT_MONSTER:
		if (!hEnemy->dwMode || hEnemy->dwMode == MONMODE_DEAD)
			return;
		if ((ptDamage->wFlags & DAMAGE_KNOCKBACK)
			&& !((MONSTATSTXT *)D2CommonMonsterTblsGetStats(hEnemy->dwClassId))->bModeKB)
			ptDamage->wFlags = (ptDamage->wFlags & ~DAMAGE_KNOCKBACK) | DAMAGE_GETHIT;
		MonsterEvalDamageFlags(ptGame, hAttacker, hEnemy, ptDamage);
		wFlags = ptDamage->wFlags;
		if (wFlags & DAMAGE_KILL)
		{
			if (hEnemy->dwFlags0 & UNIT_MASK_PET)
			{
				UNIT *hOwner = AiGetOwner(hEnemy);

				if (hOwner && hOwner->eType == UNIT_PLAYER)
					PlayerPetsRemove(ptGame, hOwner, hEnemy->dwId, FALSE);
			}
			SUnitDmgKill(ptGame, hEnemy, hAttacker, TRUE);
			return;
		}
		if (wFlags & DAMAGE_KNOCKBACK)
		{
			MonsterModeInit(hEnemy, MONMODE_KNOCKBACK, &tModeChange);
			tModeChange.hTarget = hAttacker;
			MonsterModeSet(ptGame, &tModeChange, TRUE);
			return;
		}
		if (wFlags & DAMAGE_BLOCK)
		{
#line 1417
			D2ASSERT(MonsterIsShieldEquipped (hEnemy));
			nDamage = ptDamage->nPhysicalDamage;
			if (!D2CommonStatsGetState(hEnemy, STATE_1) && !D2CommonStatsGetState(hEnemy, STATE_2)
				&& nDamage > D2CommonStatsUnitCalcMaxHp(hEnemy) / 12
				&& (hEnemy->eType != UNIT_MONSTER
					|| ((MONSTATSTXT *)D2CommonMonsterTblsGetStats(hEnemy->dwClassId))->bModeGH)
				&& !(ptDamage->wFlags & DAMAGE_NOINTERRUPT))
			{
				MonsterModeInit(hEnemy, MONMODE_BLOCK, &tModeChange);
				MonsterModeSet(ptGame, &tModeChange, TRUE);
				return;
			}
			MonsterRegionRefSetPrevMode((MONSTERREGIONREF *)((MONSTERDATA *)hEnemy->uTypeData)->pRegionRef,
				MONREGION_13);
			return;
		}
		if (wFlags & DAMAGE_GETHIT)
		{
			if (!D2CommonStatsFindListByState(hEnemy, STATE_15))
			{
				nDamage = ptDamage->nPhysicalDamage;
				if (D2CommonStatsGetState(hEnemy, STATE_1) || D2CommonStatsGetState(hEnemy, STATE_2)
					|| nDamage <= D2CommonStatsUnitCalcMaxHp(hEnemy) / 12
					|| (hEnemy->eType == UNIT_MONSTER
						&& !((MONSTATSTXT *)D2CommonMonsterTblsGetStats(hEnemy->dwClassId))->bModeGH))
				{
					D2CommonUnitRoomAddToChangedList(hEnemy);
					hEnemy->dwFlags0 |= UNITFLAG0_8000;
					MonsterRegionRefSetPrevMode((MONSTERREGIONREF *)((MONSTERDATA *)hEnemy->uTypeData)->pRegionRef,
						MONREGION_13);
					MonsterUniqueOnGetHit(ptGame, hEnemy);
					break;
				}
			}
			MonsterModeInit(hEnemy, MONMODE_GET_HIT, &tModeChange);
			MonsterModeSet(ptGame, &tModeChange, TRUE);
			MonsterUniqueOnGetHit(ptGame, hEnemy);
			return;
		}
		if (wFlags & DAMAGE_NOINTERRUPT)
		{
			D2CommonUnitRoomAddToChangedList(hEnemy);
			hEnemy->dwFlags0 |= UNITFLAG0_8000;
			MonsterRegionRefSetPrevMode((MONSTERREGIONREF *)((MONSTERDATA *)hEnemy->uTypeData)->pRegionRef,
				MONREGION_13);
			MonsterUniqueOnGetHit(ptGame, hEnemy);
			return;
		}
		return;

	case UNIT_PLAYER:
		wFlags = ptDamage->wFlags;
		if (wFlags & DAMAGE_SKILLD)
		{
			hSkill = (void *)D2CommonSkillsFindByKey(hEnemy, SKILL_D);
#line 1450
			D2WARN(hSkill);
			D2CommonSkillsSetNodeFlags(hSkill, D2CommonSkillsGetNodeFlags(hSkill) | 4);
			PlrModeStartID(ptGame, hEnemy, (void *)D2CommonSkillsFindByKey(hEnemy, SKILL_D),
				PLRMODE_SKILL1, hAttacker->eType, hAttacker->dwId, FALSE);
			break;
		}
		if (wFlags & DAMAGE_SKILL12)
		{
			hSkill = (void *)D2CommonSkillsFindByKey(hEnemy, SKILL_12);
#line 1458
			D2WARN(hSkill);
			D2CommonSkillsSetNodeFlags(hSkill, D2CommonSkillsGetNodeFlags(hSkill) | 4);
			PlrModeStartID(ptGame, hEnemy, (void *)D2CommonSkillsFindByKey(hEnemy, SKILL_12),
				PLRMODE_SKILL1, hAttacker->eType, hAttacker->dwId, FALSE);
			return;
		}
		if (wFlags & DAMAGE_SKILL1D)
		{
			hSkill = (void *)D2CommonSkillsFindByKey(hEnemy, SKILL_1D);
#line 1466
			D2WARN(hSkill);
			D2CommonSkillsSetNodeFlags(hSkill, D2CommonSkillsGetNodeFlags(hSkill) | 4);
			PlrModeStartID(ptGame, hEnemy, (void *)D2CommonSkillsFindByKey(hEnemy, SKILL_1D),
				PLRMODE_SKILL1, hAttacker->eType, hAttacker->dwId, FALSE);
			return;
		}
		if (wFlags & DAMAGE_KILL)
		{
			if (!hEnemy->dwMode || hEnemy->dwMode == PLRMODE_DEAD)
				return;
			PlrModeStartID(ptGame, hEnemy, NULL, 0, hAttacker->eType, hAttacker->dwId, FALSE);
			return;
		}
		if (wFlags & DAMAGE_KNOCKBACK)
		{
			PlrModeStartID(ptGame, hEnemy, NULL, PLRMODE_KNOCK_BACK, hAttacker->eType, hAttacker->dwId,
				FALSE);
			return;
		}
		if (wFlags & DAMAGE_BLOCK)
		{
			nDamage = ptDamage->nPhysicalDamage;
			if (D2CommonStatsGetState(hEnemy, STATE_1) || D2CommonStatsGetState(hEnemy, STATE_2)
				|| nDamage <= D2CommonStatsUnitCalcMaxHp(hEnemy) / 12)
				return;
			if (hEnemy->eType == UNIT_MONSTER
				&& !((MONSTATSTXT *)D2CommonMonsterTblsGetStats(hEnemy->dwClassId))->bModeGH)
				return;
			if (ptDamage->wFlags & DAMAGE_NOINTERRUPT)
				return;
			PlrModeStartXY(ptGame, hEnemy, NULL, PLRMODE_BLOCK, 0, 0, FALSE);
			return;
		}
		if (wFlags & DAMAGE_GETHIT)
		{
			if (!D2CommonStatsFindListByState(hEnemy, STATE_15))
			{
				nDamage = ptDamage->nPhysicalDamage;
				if (D2CommonStatsGetState(hEnemy, STATE_1) || D2CommonStatsGetState(hEnemy, STATE_2)
					|| nDamage <= D2CommonStatsUnitCalcMaxHp(hEnemy) / 12)
					goto interrupt_player;
				if (hEnemy->eType == UNIT_MONSTER
					&& !((MONSTATSTXT *)D2CommonMonsterTblsGetStats(hEnemy->dwClassId))->bModeGH)
					goto interrupt_player;
			}
			PlrModeStartXY(ptGame, hEnemy, NULL, PLRMODE_GET_HIT, ptDamage->nTotalDamage, 0, FALSE);
			return;
		}
		if (!(wFlags & DAMAGE_NOINTERRUPT))
			return;
interrupt_player:
		D2CommonUnitRoomAddToChangedList(hEnemy);
		hEnemy->dwFlags0 |= UNITFLAG0_8000;
		return;
	}
}

typedef DWORD (__fastcall *SUNITDMGHITFN)(GAME *, UNIT *, UNIT *, DAMAGE *);
typedef DWORD (__fastcall *SUNITDMGSTATEFN)(GAME *, UNIT *, UNIT *);

void __fastcall SUnitDmgDeliver(GAME *ptGame, UNIT *hAttacker,
	UNIT *hTarget)
{
	DAMAGE tDamage;
	UNIT *ahItems[7];
	DAMAGEHOLD *ptHold;
	void *ptInventory;
	void *hSkill;
	UNIT *hItem;
	DWORD dwSkill;
	DWORD dwResult;
	BYTE bCount;
	int nAdjust;
	DWORD i;

	if (!hAttacker || !hTarget)
		return;
	for (ptHold = hAttacker->ptDamageHold; ptHold; ptHold = ptHold->ptNext)
	{
		if (ptHold->eType == hAttacker->eType && ptHold->dwUnitId == hAttacker->dwId
			&& ptHold->eTargetType == hTarget->eType && ptHold->dwTargetId == hTarget->dwId)
			break;
	}
	if (!ptHold)
		return;
	tDamage = ptHold->tDamage;
	if (D2CommonUnitTestModeGroup(hTarget))
		nAdjust = -1;
	else
		nAdjust = 0;
	if (!D2CommonUnitTestPositionClear(hAttacker, hTarget, nAdjust))
		goto free_hold;
	if (tDamage.wFlags & DAMAGE_HIT)
	{
		if (hAttacker->eType != UNIT_OBJECT)
		{
			SkillsItemApplyPassiveMods(ptGame, hAttacker, hTarget, 2, &tDamage, 0, TRUE);
			switch (hAttacker->eType)
			{
			case UNIT_PLAYER:
				if (!hAttacker->dwMode)
					return;
				break;
			case UNIT_MONSTER:
				if (!hAttacker->dwMode)
					return;
				break;
			}
		}
		SUnitDmgApply(ptGame, hAttacker, hTarget, FALSE, &tDamage);
		if (tDamage.nOverlayId > 0)
			D2CommonUnitAddAnimOverlay(hTarget, tDamage.nOverlayId, 0);
		if (D2CommonStatsGetState(hTarget, STATE_A) && tDamage.nTotalDamage)
			SkillsFrozenArmorHit(ptGame, hTarget, hAttacker);
		if (!tDamage.dwFixedHitClass && !(tDamage.dwHitClass & 15))
			tDamage.dwHitClass |= D2CommonUnitGetStateByte(hAttacker);
		if (hAttacker->eType == UNIT_PLAYER)
		{
			hSkill = (void *)D2CommonUnitGetSkillThirdCached(hAttacker);
			if (hSkill)
			{
				dwSkill = D2CommonSkillsGetNodeType(hSkill);
				for (i = 0; i < sgnNoWearSkills; i++)
				{
					if (dwSkill == (DWORD)sganNoWearSkills[i])
						goto states;
				}
			}
			ptInventory = hAttacker->ptInventory;
			if (ptInventory)
			{
				hItem = (UNIT *)D2CommonINVENTORYGetLocSlot(ptInventory, 5);
				if (hItem && D2CommonITEMSIsWeapon(hItem))
					ITEMSWear(ptGame, hAttacker, hItem);
				hItem = (UNIT *)D2CommonINVENTORYGetLocSlot(ptInventory, 4);
				if (hItem && D2CommonITEMSIsWeapon(hItem))
					ITEMSWear(ptGame, hAttacker, hItem);
			}
		}
		if (hTarget->eType == UNIT_PLAYER && (ptInventory = hTarget->ptInventory) != NULL)
		{
#line 1589
			D2ASSERT(snDurBodyTblSize == 7);
			for (i = 0; i < 7; i++)
				ahItems[i] = NULL;
			bCount = 0;
			for (i = 0; i < snDurBodyTblSize; i++)
			{
				hItem = (UNIT *)D2CommonINVENTORYGetLocSlot(ptInventory, sgatDurBodyTbl[i].nLoc);
				if (hItem && D2CommonITEMSIsArmor(hItem))
				{
					ahItems[i] = hItem;
					bCount++;
				}
			}
			if (bCount > 0)
			{
				do
				{
					if (snDurBodyTblSize)
						i = D2CommonSeedRand(&hTarget->tSeed, snDurBodyTblSize);
					else
						i = 0;
				} while (!ahItems[i] || (sgatDurBodyTbl[i].nChance
					&& D2CommonSeedRand(&hTarget->tSeed, sgatDurBodyTbl[i].nChance)));
				ITEMSWear(ptGame, hTarget, ahItems[i]);
			}
		}
	}
states:
	dwResult = 0;
	if (D2CommonStatsGetState(hTarget, STATE_58))
		dwResult = ((SUNITDMGSTATEFN)SkillsShiverArmorHit)(ptGame, hTarget, hAttacker);
	if (tDamage.wFlags & DAMAGE_HIT)
	{
		if (hAttacker->eType != UNIT_OBJECT)
		{
			switch (hAttacker->eType)
			{
			case UNIT_PLAYER:
				if (!hAttacker->dwMode)
					return;
				break;
			case UNIT_MONSTER:
				if (!hAttacker->dwMode)
					return;
				break;
			}
		}
		if (tDamage.wFlags & DAMAGE_40)
			dwResult |= ((SUNITDMGHITFN)SkillsThornsHit)(ptGame, hAttacker, hTarget, &tDamage);
		if (dwResult & SKILLHIT_STOP)
			return;
		if (tDamage.wFlags & DAMAGE_400)
			dwResult |= ((SUNITDMGHITFN)SkillsIronMaidenHit)(ptGame, hAttacker, hTarget, &tDamage);
		if (dwResult & SKILLHIT_STOP)
			return;
		if (tDamage.wFlags & DAMAGE_800)
			dwResult |= ((SUNITDMGHITFN)SkillsLifeTapHit)(ptGame, hAttacker, hTarget, &tDamage);
		if (dwResult & SKILLHIT_STOP)
			return;
	}
	SUnitDmgReact(ptGame, hAttacker, hTarget, &tDamage);
free_hold:
	SUnitDmgFreeHold(ptGame, hAttacker, hTarget);
}

DAMAGE *__fastcall SUnitDmgFindHeld(UNIT *hUnit, UNIT *hTarget)
{
	DAMAGEHOLD *ptHold;

	for (ptHold = hUnit->ptDamageHold; ptHold; ptHold = ptHold->ptNext)
	{
		if (ptHold->eType == hUnit->eType && ptHold->dwUnitId == hUnit->dwId
			&& ptHold->eTargetType == hTarget->eType && ptHold->dwTargetId == hTarget->dwId)
			return &ptHold->tDamage;
	}
	return NULL;
}

WORD __fastcall SUnitDmgGetHeldFlags(UNIT *hUnit, UNIT *hTarget)
{
	DAMAGEHOLD *ptHold;

	if (hTarget)
	{
		for (ptHold = hUnit->ptDamageHold; ptHold; ptHold = ptHold->ptNext)
		{
			if (ptHold->eType == hUnit->eType && ptHold->dwUnitId == hUnit->dwId
				&& ptHold->eTargetType == hTarget->eType && ptHold->dwTargetId == hTarget->dwId)
				return (BYTE)ptHold->tDamage.wFlags;
		}
	}
	return 0;
}

BOOL __stdcall SUnitDmgRollRange(UNIT *hUnit, int *pnFirst,
	int *pnSecond)
{
	*pnFirst = *pnFirst ? (int)D2CommonSeedRand(&hUnit->tSeed, *pnFirst) : 0;
	*pnSecond = *pnSecond ? (int)D2CommonSeedRand(&hUnit->tSeed, *pnSecond) : 0;
	return *pnFirst >= *pnSecond;
}

BOOL __fastcall SUnitDmgRollHit(UNIT *hAttacker, UNIT *hTarget,
	int nToHit, BOOL bMissile)
{
	CHARSTATSTXT *ptStats;
	SKILLSTATE tState;
	UNIT *hWeapon;
	int nChance;
	int nWeapon;
	int nTargetLevel;
	int nLevel;
	int nDefense;
	int nRating;
	int nPercent;
	int nBonus;
	int nRoll;
	int nHit;
	int i;

	nChance = 100;
	nWeapon = 0;
	if (!hTarget || !hAttacker)
		return FALSE;
	nTargetLevel = D2CommonStatsUnitGetValue(hTarget, STATS_LEVEL);
	nDefense = D2CommonUnitCalcArmorClassWithBonus(hTarget);
	if (bMissile)
		nDefense += D2CommonStatsUnitGetValue(hTarget, STATS_ARMORCLASS_VS_MISSILE);
	else
		nDefense += D2CommonStatsUnitGetValue(hTarget, STATS_ARMORCLASS_VS_HTH);
	nLevel = D2CommonStatsUnitGetValue(hAttacker, STATS_LEVEL);
	nPercent = 0;
	if (hAttacker->eType == UNIT_PLAYER)
	{
		nRating = (int)D2CommonUnitGetAttackerIndexedValue(hAttacker);
		if (D2CommonStatsGetTotalValue(hAttacker, STATS_ITEM_IGNORETARGETAC))
			nDefense = 0;
		nPercent = (int)D2CommonStatsGetTotalValue(hAttacker, STATS_ITEM_FRACTIONALTARGETAC);
		if (nPercent)
			nDefense = nPercent * nDefense / 100;
		nBonus = (int)D2CommonStatsGetTotalValue(hAttacker, STATS_ITEM_DEMON_TOHIT);
		if (nBonus && D2CommonMonsterRegionUnitIsDemonRaw((DWORD)hTarget))
			nRating += nBonus;
		nBonus = (int)D2CommonStatsGetTotalValue(hAttacker, STATS_ITEM_UNDEAD_TOHIT);
		if (nBonus && D2CommonMonsterRegionUnitIsUndead(hTarget))
			nRating += nBonus;
		if (!bMissile)
		{
			hWeapon = (UNIT *)D2CommonUnitGetActiveWeapon(hAttacker);
			if (hWeapon)
				nWeapon = D2CommonSkillsGetItemSkillValueAny(hAttacker, 0, hWeapon, 0, 0, 1);
		}
		nPercent = (int)D2CommonStatsGetTotalValue(hAttacker, STATS_ITEM_TOHIT_PERCENT) + nWeapon + nToHit;
	}
	else
	{
		nRating = nToHit + D2CommonStatsUnitGetValue(hAttacker, STATS_TOHIT) * 4;
		nPercent = (int)D2CommonStatsGetTotalValue(hAttacker, STATS_ITEM_TOHIT_PERCENT);
	}
	nHit = nRating + nRating * nPercent / 100;
	if (hAttacker->eType == UNIT_PLAYER)
	{
		ptStats = (CHARSTATSTXT *)D2CommonDataTblsGetCharStats(hAttacker->dwClassId);
#line 1858
		D2ASSERT(ptStats);
		nHit = ptStats->ToHitFactor * nHit / 100;
	}
	if (nDefense < 0)
	{
		nHit -= nDefense;
		nDefense = 0;
	}
	if (nHit < 0)
	{
		nDefense -= nHit;
		nHit = 0;
	}
	if (nDefense < 0)
		nDefense = 0;
	if (nHit || nDefense)
		nChance = nHit * 100 / (nHit + nDefense);
	nChance = nLevel * nChance * 2 / (nLevel + nTargetLevel);
	if (nChance < 5)
		nChance = 5;
	else if (nChance > 95)
		nChance = 95;
	nRoll = (int)D2CommonSeedRand(&hAttacker->tSeed, 100);
	DebugUnitValue(SUnitGetGame(hAttacker), hAttacker, hTarget, nChance, (DWORD)nRoll);
	if (nRoll >= nChance)
		return FALSE;
	if (hAttacker->eType == UNIT_PLAYER && hTarget->eType == UNIT_MONSTER
		&& D2CommonStatsGetTotalValue(hAttacker, STATS_ITEM_PREVENTHEAL))
	{
		for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
			((DWORD *)&tState)[i] = 0;
		tState.hSource = hAttacker;
		tState.hTarget = hTarget;
		tState.nSkill = 0;
		tState.nLevel = 1;
		tState.nDuration = 25000;
		tState.dwStatId = 31;
		tState.nStatValue = 0;
		tState.dwState = 52;
		SkillsSetState(&tState);
	}
	return TRUE;
}

WORD __fastcall SUnitDmgHit(GAME *ptGame, UNIT *hAttacker  ,
	UNIT *hTarget  , int nToHit)
{
	DWORD dwFlags;
	DWORD dwAvoid;

	dwFlags = 0;
#line 1960
	D2ASSERT(hTarget);
#line 1962
	D2ASSERT(UnitGetType (hAttacker) == UNIT_PLAYER || UnitGetType (hAttacker) == UNIT_MONSTER);
#line 1964
	D2ASSERT(UnitGetType (hTarget) == UNIT_PLAYER || UnitGetType (hTarget) == UNIT_MONSTER);
	if (!SUnitIsHostile(ptGame, hAttacker, hTarget))
		return 0;
	switch (hTarget->eType)
	{
	case UNIT_PLAYER:
		if (hTarget->dwMode == PLRMODE_RUN)
			dwFlags = DAMAGE_HIT;
		break;
	case UNIT_MONSTER:
		if (!MonsterEvalIsAlive(hTarget))
			return 0;
		break;
	}
	if (!D2CommonUnitTestPositionClear(hAttacker, hTarget, 0))
		return 0;
	if (!(dwFlags & DAMAGE_HIT))
	{
		if (SUnitDmgRollHit(hAttacker, hTarget, nToHit, FALSE))
			dwFlags |= DAMAGE_HIT;
	}
	if (dwFlags & DAMAGE_HIT)
	{
		dwAvoid = SUnitDmgBlock(hAttacker, hTarget, FALSE, TRUE);
		if (dwAvoid & 2)
			dwFlags |= DAMAGE_SKILL12;
		if (dwAvoid & 4)
			dwFlags |= DAMAGE_SKILLD;
		if (dwAvoid & 1)
			dwFlags |= DAMAGE_BLOCK;
		if (dwAvoid)
			dwFlags &= 65534;
	}
	if (D2CommonStatsGetState(hTarget, STATE_24))
		dwFlags |= DAMAGE_40;
	if (D2CommonStatsGetState(hAttacker, STATE_37))
		dwFlags |= DAMAGE_400;
	if (D2CommonStatsGetState(hTarget, STATE_3A))
		dwFlags |= DAMAGE_800;
	if (dwFlags & DAMAGE_BLOCK)
		dwFlags &= 65534;
	if ((dwFlags & DAMAGE_HIT) && !D2CommonStatsGetState(hTarget, UNIT_STATE_UNINTERRUPTABLE))
		dwFlags |= DAMAGE_GETHIT;
	return (WORD)dwFlags;
}

void __fastcall SUnitDmgHold(GAME *ptGame, UNIT *hUnit,
	UNIT *ptDestUnit  , DAMAGE *ptDamage)
{
	DAMAGEHOLD *ptHold;
	DWORD eType;
	int nDamage;

	eType = hUnit->eType;
	if (!(ptDamage->wFlags & DAMAGE_DODGED) && (ptDamage->wFlags & DAMAGE_HIT))
	{
		if (!(ptDamage->dwFlags0 & DAMAGE0_HELD))
			SUnitDmgRoll(hUnit, ptDestUnit, ptDamage);
		SUnitDmgResist(hUnit, ptDestUnit, ptDamage);
		nDamage = ptDamage->nPoisonDamage + ptDamage->nColdDamage + ptDamage->nMagicDamage
			+ ptDamage->nLightningDamage
			+ ptDamage->nFireDamage + ptDamage->nPhysicalDamage;
		if (eType == UNIT_MONSTER)
			nDamage += ptDamage->nLifeDrain;
		if ((int)(nDamage & 0xFFFFFF00) > (int)((DWORD)D2CommonStatsUnitGetValue(ptDestUnit, STATS_HITPOINTS)
			& 0xFFFFFF00))
			ptDamage->wFlags |= DAMAGE_KILL;
	}
#line 1186
	D2ASSERT((ptDestUnit->eType == UNIT_MONSTER) || (ptDestUnit->eType == UNIT_PLAYER));
#line 1189
	ptHold = (DAMAGEHOLD *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(DAMAGEHOLD));
	ptHold->ptGame = ptGame;
	ptHold->eType = hUnit->eType;
	ptHold->dwUnitId = hUnit->dwId;
	ptHold->eTargetType = ptDestUnit->eType;
	ptHold->dwTargetId = ptDestUnit->dwId;
	ptHold->tDamage = *ptDamage;
	ptHold->ptNext = hUnit->ptDamageHold;
	hUnit->ptDamageHold = ptHold;
	ptHold->ptTargetNext = NULL;
}

void __fastcall SUnitDmgRoll(UNIT *hAttacker, UNIT *hTarget,
	DAMAGE *ptDamage)
{
	MONSTATSTXT *ptMonStats;
	GAME *ptGame;
	UNIT *hWeapon;
	UNIT *hItem;
	DWORD eType;
	DWORD eTargetType;
	int nMin;
	int nMax;
	int nChance;
	int nValue;

	D2CommonUnitApplyPrimaryWeaponSwap(hAttacker);
	eType = hAttacker->eType;
	eTargetType = hTarget->eType;
	ptGame = SUnitGetGame(hAttacker);
	if (!(ptDamage->dwFlags0 & DAMAGE0_NOROLL))
	{
		ITEMSTXT *ptItemStats;
		void *ptInventory;
		WORD wClass;

		nMin = D2CommonStatsUnitGetValue(hAttacker, STATS_MINDAMAGE);
		nMax = D2CommonStatsUnitGetValue(hAttacker, STATS_MAXDAMAGE);
		ptInventory = hAttacker->ptInventory;
		hWeapon = (UNIT *)D2CommonUnitGetActiveWeapon(hAttacker);
		if (hWeapon)
		{
			ptDamage->dwWeaponId = hWeapon->dwId;
			if (D2CommonINVENTORYTestCanEquipSlot(hAttacker, ptInventory) == 2)
			{
				ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hWeapon->dwClassId);
#line 631
				D2ASSERT(ptItemStats);
				nMin += ptItemStats->bTwoHandMinDam - ptItemStats->mindam;
				nMax += ptItemStats->bTwoHandMaxDam - ptItemStats->maxdam;
			}
		}
		ptDamage->nPhysicalDamage = SUnitDmgRollDamage(hAttacker, nMin << 8, nMax << 8,
			(int)ptDamage->dwDamagePercent,
			ptDamage->nPhysicalDamage);
		if (D2CommonStatsGetState(hAttacker, STATE_40) && hWeapon)
		{
			wClass = D2CommonITEMSGetWClassIndex(hWeapon);
			if (wClass == 3 || wClass == 6)
			{
				nChance = D2CommonSkillsCalcCurveParamOne(D2CommonSkillsCalcNodeLevel(hAttacker,
					D2CommonSkillsFindByKey(hAttacker, SKILL_9), 1), SKILL_9);
				if ((int)D2CommonSeedRand(&hAttacker->tSeed, 100) < nChance)
				{
					ptDamage->nPhysicalDamage *= 2;
					ptDamage->wFlags |= DAMAGE_DOUBLED;
				}
			}
		}
		nChance = (int)D2CommonStatsGetTotalValue(hAttacker, STATS_ITEM_DEADLYSTRIKE);
		if (nChance && (int)D2CommonSeedRand(&hAttacker->tSeed, 100) < nChance)
		{
			ptDamage->nPhysicalDamage *= 2;
			ptDamage->wFlags |= DAMAGE_DOUBLED;
		}
	}
	ptDamage->nFireDamage += SUnitDmgRollStat(hAttacker, ptDamage->nFireDamage, STATS_FIREMAXDAM,
		STATS_FIREMINDAM);
	ptDamage->nLightningDamage += SUnitDmgRollStat(hAttacker, ptDamage->nLightningDamage, STATS_LIGHTMAXDAM,
		STATS_LIGHTMINDAM);
	ptDamage->nColdDamage += SUnitDmgRollStat(hAttacker, ptDamage->nColdDamage, STATS_COLDMAXDAM,
		STATS_COLDMINDAM);
	ptDamage->nMagicDamage += SUnitDmgRollStat(hAttacker, ptDamage->nMagicDamage, STATS_MAGICMAXDAM,
		STATS_MAGICMINDAM);
	if (eType == UNIT_MONSTER)
	{
		ptDamage->nLifeDrain += SUnitDmgRollStat(hAttacker, ptDamage->nLifeDrain, STATS_LIFEDRAINMAXDAM,
			STATS_LIFEDRAINMINDAM);
		ptDamage->nManaDrain += SUnitDmgRollStat(hAttacker, ptDamage->nManaDrain, STATS_MANADRAINMAXDAM,
			STATS_MANADRAINMINDAM);
		ptDamage->nStaminaDrain += SUnitDmgRollStat(hAttacker, ptDamage->nStaminaDrain, STATS_STAMDRAINMAXDAM,
			STATS_STAMDRAINMINDAM);
	}
	else if (eType == UNIT_PLAYER)
	{
		ptDamage->nLifeDrain += D2CommonStatsUnitGetValue(hAttacker, STATS_LIFEDRAINMINDAM);
		ptDamage->nManaDrain += D2CommonStatsUnitGetValue(hAttacker, STATS_MANADRAINMINDAM);
	}
	nMin = D2CommonStatsUnitGetValue(hAttacker, STATS_POISONMINDAM);
	nMax = D2CommonStatsUnitGetValue(hAttacker, STATS_POISONMAXDAM);
	ptDamage->nPoisonDamage += SUnitDmgRollAdd(hAttacker, nMin, nMax, 0, ptDamage->nPoisonDamage);
	if (ptDamage->nPoisonDamage)
		ptDamage->nPoisonLength += D2CommonStatsUnitGetValue(hAttacker, STATS_POISONLENGTH);
	if (ptDamage->nColdDamage)
		ptDamage->nColdLength += D2CommonStatsUnitGetValue(hAttacker, STATS_COLDLENGTH);
	if (!ptDamage->nStunLength)
		ptDamage->nStunLength += D2CommonStatsUnitGetValue(hAttacker, STATS_STUNLENGTH);
	SkillsFireMasteryDamage(hAttacker, ptDamage);
	if (D2CommonStatsGetState(hAttacker, STATE_10))
	{
		int nMin;
		int nMax;

		nMin = 0;
		nMax = 0;
		D2CommonSkillsAddListRange(hAttacker, &nMin, &nMax, 0);
		nMin <<= 8;
		nMax <<= 8;
		ptDamage->nFireDamage += SUnitDmgRollAdd(hAttacker, nMin, nMax, 0, 0);
	}
	SUnitGetGame(hAttacker);
	if (hTarget->eType == UNIT_MONSTER)
	{
		ptMonStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(hTarget->dwClassId);
		if ((ptMonStats->bLUndead || ptMonStats->bHUndead) && hAttacker->eType == UNIT_PLAYER)
		{
			hWeapon = (UNIT *)D2CommonUnitGetActiveWeapon(hAttacker);
			if (hWeapon && !D2CommonITEMSGetWeaponGroupEnum(hWeapon))
				ptDamage->nPhysicalDamage += ptDamage->nPhysicalDamage / 2;
		}
	}
	if (eType == UNIT_PLAYER && eTargetType == UNIT_MONSTER)
	{
		nValue = (int)D2CommonStatsGetTotalValue(hAttacker, STATS_ITEM_DAMAGETARGETAC);
		if (nValue)
		{
			int nArmor;

			nArmor = nValue + D2CommonStatsUnitGetValue(hTarget, STATS_ARMORCLASS);
			if (nArmor < 0)
				nArmor = 0;
			D2CommonStatsUnitSetValue(hTarget, STATS_ARMORCLASS, nArmor);
		}
		nValue = (int)D2CommonStatsGetTotalValue(hAttacker, STATS_ITEM_DEMONDAMAGE_PERCENT);
		if (nValue && D2CommonMonsterRegionUnitIsDemonRaw((DWORD)hTarget))
			ptDamage->nPhysicalDamage = ptDamage->nPhysicalDamage * nValue / 100;
		nValue = (int)D2CommonStatsGetTotalValue(hAttacker, STATS_ITEM_UNDEADDAMAGE_PERCENT);
		if (nValue && D2CommonMonsterRegionUnitIsUndead(hTarget))
			ptDamage->nPhysicalDamage += ptDamage->nPhysicalDamage * nValue / 100;
	}
	if (eType == UNIT_PLAYER)
	{
		if (ptDamage->dwWeaponId)
		{
			hItem = SUnitFind(ptGame, UNIT_ITEM, ptDamage->dwWeaponId);
			if (hItem)
			{
				int nCrush;

				nCrush = 0;
				nValue = D2CommonSkillsGetItemSkillValueAny(hAttacker, 0, hItem, &nCrush, 0, 0);
				if (nValue)
				{
					ptDamage->nPhysicalDamage += ptDamage->nPhysicalDamage * nValue / 100;
					nChance = nCrush * 2;
					if ((int)D2CommonSeedRand(&hAttacker->tSeed, 100) < nChance)
					{
						ptDamage->nPhysicalDamage *= 2;
						ptDamage->wFlags |= DAMAGE_DOUBLED;
					}
				}
			}
		}
		ptDamage->wFlags |= (WORD)SkillsItemApplyPassiveMods(ptGame, hAttacker, hTarget, 1, ptDamage, 0,
			TRUE);
	}
	D2CommonUnitApplyEquipStatSwap(hAttacker);
	MonsterModeCriticalHit(hAttacker, hTarget, ptDamage);
}

static void SUnitDmgAddExp(GAME *ptGame, UNIT *hPlayer,
	int nExp, int nLevel, int nMonLevel)
{
	int nTotal;

	SUnitDmgScaleExp(hPlayer, &nExp, nLevel, nMonLevel);
	D2CommonStatsUnitSetValue(hPlayer, STATS_LASTEXP, nExp);
	if (D2CommonStatsGetState(hPlayer, STATE_89))
		nExp += nExp >> 1;
	nTotal = D2CommonStatsUnitGetBaseValue(hPlayer, STATS_EXPERIENCE) + nExp;
	D2CommonStatsUnitSetValue(hPlayer, STATS_EXPERIENCE, nTotal);
	if (nLevel != D2CommonDataTblsGetLevelFromExperience(hPlayer->dwClassId, nTotal))
		PlrEvalLevelUp(hPlayer, ptGame);
}

static void SUnitDmgAwardPartyExp(GAME *ptGame, UNIT *hPlayer,
	int nMonLevel, int nExp)
{
	PARTYEXP tShare;
	WORD wParty;
	int nLevelSum;
	int nLevel;
	int nBase;

	nBase = nExp;
	wParty = SUnitGetParty(hPlayer);
	nLevelSum = PartyGetLevelSum(ptGame, hPlayer);
	if (!nLevelSum)
		return;
	memset(&tShare, 0, sizeof(tShare));
	tShare.hPlayer = hPlayer;
	tShare.nExp = nBase;
	tShare.nMonLevel = nMonLevel;
	tShare.dwLevelId = D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hPlayer));
	tShare.nLevelSum = nLevelSum;
	nLevel = D2CommonStatsUnitGetValue(hPlayer, STATS_LEVEL);
	if (wParty != PARTY_NONE && nLevelSum != nLevel)
	{
		tShare.nExp /= nLevelSum;
		if (!tShare.nExp)
			tShare.nExp = 1;
		PartyForEach(ptGame, wParty, SUnitDmgShareExp, &tShare);
		nBase = nLevel * tShare.nExp;
		nBase += nBase / 10;
	}
	SUnitDmgAddExp(ptGame, hPlayer, nBase, nLevel, nMonLevel);
}

void __fastcall SUnitDmgAwardExp(GAME *ptGame, UNIT *hKiller,
	UNIT *hUnit, int nMonLevel, int nExp)
{
	UNIT *hPlayer;
	void *hList;
	DWORD eType;
	DWORD dwUnitId;
	int nLevel;
	int nBase;

	hPlayer = hKiller;
	if (hKiller->eType != UNIT_PLAYER && hKiller->eType != UNIT_MONSTER)
		return;
	switch (hKiller->eType)
	{
	case UNIT_PLAYER:
		if (hKiller->dwMode == PLRMODE_DEATH || hKiller->dwMode == PLRMODE_DEAD)
			return;
		break;
	case UNIT_MONSTER:
		if (hKiller->dwMode == MONMODE_DEATH || hKiller->dwMode == MONMODE_DEAD)
			return;
		break;
	}
	if (hKiller->eType == UNIT_MONSTER)
	{
		hPlayer = AiGetOwner(hKiller);
		hList = NULL;
		if (D2CommonStatsUnitTestOwnedState(hKiller))
			hList = (void *)D2CommonStatsFindListByMask(hKiller, 2048);
		if (hUnit && D2CommonStatsUnitTestOwnedState(hUnit))
			hList = (void *)D2CommonStatsFindListByMask(hUnit, 2048);
		if (hList)
		{
			eType = D2CommonStatsGetListSourceType(hList);
			dwUnitId = D2CommonStatsGetListSourceId(hList);
			hPlayer = SUnitFind(ptGame, eType, dwUnitId);
		}
		if (!hPlayer || hPlayer->eType != UNIT_PLAYER)
			return;
		if (hKiller->dwMode == PLRMODE_DEATH || hKiller->dwMode == PLRMODE_DEAD)
			return;
	}
	if (SUnitGetParty(hPlayer) != PARTY_NONE)
		SUnitDmgAwardPartyExp(ptGame, hPlayer, nMonLevel, nExp);
	else
	{
		nLevel = D2CommonStatsUnitGetValue(hPlayer, STATS_LEVEL);
		nBase = nExp;
		SUnitDmgAddExp(ptGame, hPlayer, nBase, nLevel, nMonLevel);
	}

}

static __inline void __fastcall SUnitDmgScaleExp(UNIT *hPlayer, int *pnExp,
	int nLevel, int nMonLevel)
{
	int nPercent;

	if (nLevel >= D2CommonDataTblsGetMaxLevel(hPlayer->dwClassId))
	{
		*pnExp = 0;
		return;
	}
	if (nLevel >= 25 && nLevel < nMonLevel)
		*pnExp = *pnExp * nLevel / nMonLevel;
	else if (nMonLevel >= nLevel + 5)
	{
		switch (nMonLevel - nLevel - 5)
		{
		case 0:
			nPercent = 100;
			break;
		case 1:
			nPercent = 88;
			break;
		case 2:
			nPercent = 68;
			break;
		case 3:
			nPercent = 36;
			break;
		case 4:
			nPercent = 15;
			break;
		default:
			nPercent = 5;
			break;
		}
		*pnExp = *pnExp * nPercent / 100;
	}
	else if (nMonLevel <= nLevel - 5)
	{
		switch (nLevel - nMonLevel - 5)
		{
		case 0:
			nPercent = 100;
			break;
		case 1:
			nPercent = 81;
			break;
		case 2:
			nPercent = 62;
			break;
		case 3:
			nPercent = 43;
			break;
		case 4:
			nPercent = 24;
			break;
		default:
			nPercent = 5;
			break;
		}
		*pnExp = *pnExp * nPercent / 100;
	}
	if (!*pnExp)
		*pnExp = 1;
}

void __fastcall SUnitDmgShareExp(GAME *ptGame, UNIT *hPlayer,
	void *pArg)
{
	PARTYEXP *ptShare;
	int nMonLevel;
	int nLevel;
	int nExp;

	ptShare = (PARTYEXP *)pArg;
	nMonLevel = ptShare->nMonLevel;
	if (SUnitIsDead(hPlayer))
		return;
	if (hPlayer == ptShare->hPlayer)
		return;
	if (D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hPlayer)) != ptShare->dwLevelId)
		return;
	if (hPlayer->dwMode == PLRMODE_DEATH || hPlayer->dwMode == PLRMODE_DEAD)
		return;
	nLevel = D2CommonStatsUnitGetValue(hPlayer, STATS_LEVEL);
	nExp = ptShare->nExp * nLevel;
	SUnitDmgAddExp(ptGame, hPlayer, nExp, nLevel, nMonLevel);
}

DWORD __fastcall SUnitDmgBlock(UNIT *hAttacker, UNIT *hTarget,
	BOOL bMissile, BOOL bBlock)
{
	int nBlock;
	int nRoll;

	nBlock = 0;
	switch (hTarget->eType)
	{
	case UNIT_PLAYER:
		if (D2CommonINVENTORYFindShieldInHands(hTarget->ptInventory, 0))
		{
			nBlock = D2CommonStatsUnitGetValue(hTarget, STATS_TOBLOCK);
			nBlock += ((CHARSTATSTXT *)D2CommonDataTblsGetCharStats(hTarget->dwClassId))->BlockFactor;
		}
		break;
	case UNIT_MONSTER:
		if (MonsterIsShieldEquipped(hTarget))
			nBlock = D2CommonStatsUnitGetValue(hTarget, STATS_TOBLOCK);
		break;
	}
	if (nBlock && bBlock)
	{
		if (nBlock >= 75)
			nBlock = 75;
		nRoll = (int)D2CommonSeedRand(&hTarget->tSeed, 100);
		DebugRoll(SUnitGetGame(hAttacker), hAttacker, hTarget, nBlock, nRoll, "blocks");
		if (nRoll < nBlock)
			return 1;
		return SUnitDmgEvade(hAttacker, hTarget, bMissile);
	}
	return SUnitDmgEvade(hAttacker, hTarget, bMissile);
}

DWORD __fastcall SUnitDmgEvade(UNIT *hAttacker, UNIT *hTarget,
	BOOL bMissile)
{
	DWORD dwResult;
	int nChance;
	int nRoll;

	dwResult = 0;
	if (hTarget->eType != UNIT_PLAYER)
		return 0;
	if (hTarget->dwMode == PLRMODE_RUN || hTarget->dwMode == PLRMODE_WALK)
	{
		if (D2CommonStatsGetState(hTarget, STATE_44))
		{
			nChance = D2CommonSkillsCalcCurveParamOne(D2CommonSkillsCalcNodeLevel(hTarget,
				D2CommonSkillsFindByKey(hTarget, SKILL_1D), 1), SKILL_1D);
			nRoll = (int)D2CommonSeedRand(&hTarget->tSeed, 100);
			DebugRoll(SUnitGetGame(hAttacker), hAttacker, hTarget, nChance, nRoll, "evades");
			if (nRoll < nChance)
				dwResult = 8;
		}
	}
	else if (!bMissile)
	{
		if (D2CommonStatsGetState(hTarget, STATE_41))
		{
			nChance = D2CommonSkillsCalcCurveParamOne(D2CommonSkillsCalcNodeLevel(hTarget,
				D2CommonSkillsFindByKey(hTarget, SKILL_D), 1), SKILL_D);
			nRoll = (int)D2CommonSeedRand(&hTarget->tSeed, 100);
			DebugRoll(SUnitGetGame(hAttacker), hAttacker, hTarget, nChance, nRoll, "dodges");
			if (nRoll < nChance)
				dwResult = 4;
		}
	}
	else
	{
		if (D2CommonStatsGetState(hTarget, STATE_42))
		{
			nChance = D2CommonSkillsCalcCurveParamOne(D2CommonSkillsCalcNodeLevel(hTarget,
				D2CommonSkillsFindByKey(hTarget, SKILL_12), 1), SKILL_12);
			nRoll = (int)D2CommonSeedRand(&hTarget->tSeed, 100);
			DebugRoll(SUnitGetGame(hAttacker), hAttacker, hTarget, nChance, nRoll, "avoids");
			if (nRoll < nChance)
				dwResult = 2;
		}
	}
	return dwResult;
}

BOOL __fastcall SUnitDmgSplash(GAME *ptGame, UNIT *hSource,
	int nX, int nY, int nRadius,
	DAMAGE *ptDamage, BOOL bOwner, BOOL bOwnerOwner,
	PFNSPLASHTEST pfnCallBack  , DWORD dwFlags)
{
	UNITFINDDATA tFind;
	UNITFINDARGS tArgs;
	DAMAGE tDamage;
	void *hRoom;
	UNIT *hOwner;
	UNIT *hOwnerOwner;
	UNIT *hUnit;
	BOOL bResult;
	int i;

	bResult = FALSE;
	hRoom = (void *)D2CommonUnitGetPathRoom(hSource);
	hRoom = D2CommonDungeonFindRoomGame(hRoom, nX, nY);
	if (!dwFlags)
		dwFlags = 1411;
	for (i = 0; i < (int)(sizeof(UNITFINDARGS) / sizeof(DWORD)); i++)
		((DWORD *)&tArgs)[i] = 0;
	tArgs.dwFlags = dwFlags;
	tArgs.nDistance = nRadius;
	tArgs.nX = nX;
	tArgs.nY = nY;
	D2CommonUnitFindsInit(ptGame->pMemPool, &tFind, hRoom, nX, nY, nRadius, 0,
		&tArgs);
	D2CommonUnitFindsFindUnits(&tFind);
	hOwner = SUnitGetOwner(ptGame, hSource);
	if (hOwner)
		hOwnerOwner = SUnitGetOwner(ptGame, hOwner);
	else
		hOwnerOwner = NULL;
	if (!hOwner)
		return FALSE;
	for (i = 0; i < tFind.nNumUnits; i++)
	{
		hUnit = tFind.phUnitsArray[i];
		if (hOwnerOwner == hUnit && !bOwnerOwner)
			continue;
		if (hOwner == hUnit && !bOwner)
			continue;
		if (!SUnitIsHostile(ptGame, hOwner, hUnit))
			continue;
		if (!D2CommonSkillsAddFieldB(nX, nY, tFind.phUnitsArray[i], 2053))
			continue;
		tDamage = *ptDamage;
		if (pfnCallBack)
		{
#line 2508
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)pfnCallBack), "pfnCallBack");
			if (pfnCallBack(ptGame, hSource, tFind.phUnitsArray[i]))
			{
				MissModeApplyDamage(ptGame, hSource, tFind.phUnitsArray[i], &tDamage);
				bResult = TRUE;
			}
		}
		else
		{
			MissModeApplyDamage(ptGame, hSource, tFind.phUnitsArray[i], &tDamage);
			bResult = TRUE;
		}
	}
	D2CommonUnitFindsFreeArray(&tFind);
	return bResult;
}
