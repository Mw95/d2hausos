#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\PLAYER\\PlrEval.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Game.h"
#include "../GAME/SCmd.h"
#include "../MONSTER/MonsterAI.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "../UNIT/sunitproxy.h"

#define STATS_STRENGTH			0
#define STATS_ENERGY			1
#define STATS_DEXTERITY			2
#define STATS_VITALITY			3
#define STATS_STATPTS			4
#define STATS_NEWSKILLS			5
#define STATS_HITPOINTS			6
#define STATS_MAXHP				7
#define STATS_MANA				8
#define STATS_MAXMANA			9
#define STATS_STAMINA			10
#define STATS_MAXSTAMINA		11
#define STATS_LEVEL				12
#define STATS_EXPERIENCE		13
#define STATS_TOHIT				19
#define STATS_TOBLOCK			20
#define STATS_NEXTEXP			30
#define STATS_VELOCITYPERCENT	67
#define STATS_ATTACKRATE		68
#define STATS_OTHER_ANIMRATE	69

static const int sganActLevel[NUM_ACTS] = { 1, 15, 20, 26 };

extern const int PLACEHOLDER_UnreferencedData_100a652c[5][2] = { { 30, 6 }, { 30, 36 }, { 30, 66 }, { 30, 96 }, { 30, 126 } };

typedef void (__fastcall *PFNEVENTLEVELUP)(DWORD wGameId, DWORD dwArg, WORD wClass, int nLevel);

typedef void (__fastcall *PFNGAMEUNITCALLBACK)(GAME *ptGame, UNIT *hUnit, void *pArg);

void __fastcall PlrEvalLevelUpEach(GAME *ptGame, UNIT *hUnit, void *pArg);

void __fastcall PlrEvalInit(GAME *ptGame, UNIT *hPlayer, BYTE bAct  )
{
	CHARSTATSTXT *ptCharStats;
	DWORD dwClass;
	int nLife;
	int nMana;
	int nStamina;
	int nLevels;
	int nExp;
	int nLevel;

	dwClass = hPlayer->dwClassId;
	ptCharStats = (CHARSTATSTXT *)D2CommonDataTblsGetCharStats(dwClass);
	D2CommonStatsUnitSetValue(hPlayer, STATS_STRENGTH, ptCharStats->str);
	D2CommonStatsUnitSetValue(hPlayer, STATS_ENERGY, ptCharStats->Int);
	D2CommonStatsUnitSetValue(hPlayer, STATS_DEXTERITY, ptCharStats->dex);
	D2CommonStatsUnitSetValue(hPlayer, STATS_VITALITY, ptCharStats->vit);
	D2CommonStatsUnitSetValue(hPlayer, STATS_TOHIT, 0);
	D2CommonStatsUnitSetValue(hPlayer, STATS_TOBLOCK, 0);
	nLife = (ptCharStats->hpadd + ptCharStats->vit) << 8;
	D2CommonStatsUnitSetValue(hPlayer, STATS_HITPOINTS, nLife);
	D2CommonStatsUnitSetValue(hPlayer, STATS_MAXHP, nLife);
	nMana = ptCharStats->Int << 8;
	D2CommonStatsUnitSetValue(hPlayer, STATS_MANA, nMana);
	D2CommonStatsUnitSetValue(hPlayer, STATS_MAXMANA, nMana);
	nStamina = (((ptCharStats->vit + ptCharStats->str) >> 1) << 8) + (64 << 8);
	D2CommonStatsUnitSetValue(hPlayer, STATS_STAMINA, nStamina);
	D2CommonStatsUnitSetValue(hPlayer, STATS_MAXSTAMINA, nStamina);
	D2CommonStatsUnitSetValue(hPlayer, STATS_LEVEL, 1);
	D2CommonStatsUnitSetValue(hPlayer, STATS_NEXTEXP,
		D2CommonDataTblsGetExperience(dwClass, 1));
	D2CommonStatsUnitSetValue(hPlayer, STATS_ATTACKRATE, 100);
	D2CommonStatsUnitSetValue(hPlayer, STATS_VELOCITYPERCENT, 75);
	D2CommonStatsUnitSetValue(hPlayer, STATS_OTHER_ANIMRATE, 100);
	if (bAct <= 0)
		return;
#line 88
	D2ASSERT(bAct < NUM_ACTS);
	nLevel = sganActLevel[bAct];
	if (nLevel <= 1)
		return;
	nLevels = nLevel - 1;
	do
	{
		nExp = D2CommonStatsUnitGetBaseValue(hPlayer, STATS_NEXTEXP);
		nExp -= D2CommonStatsUnitGetBaseValue(hPlayer, STATS_EXPERIENCE);

		SUnitDmgAwardExp(ptGame, hPlayer, NULL, D2CommonStatsUnitGetBaseValue(hPlayer, STATS_LEVEL), nExp);
	} while (--nLevels);
}

extern BOOL __fastcall PlrEvalCanLearnSkill(UNIT *hPlayer, void *ptType, int nSkillId);
__inline BOOL __fastcall PlrEvalCanLearnSkill(UNIT *hPlayer, void *ptType, int nSkillId)
{
	if (!D2CommonSkillsTestPrerequisites(hPlayer, ptType, nSkillId))
		return FALSE;
	return D2CommonSkillsTestStatRequirements(hPlayer, ptType, nSkillId);
}

void __fastcall PlrEvalLevelUp(UNIT *hPlayer, GAME *ptGame)
{
	CHARSTATSTXT *ptCharStats;
	EVENTCALLBACKTABLE *ptTable;
	void **ppfnLevelUp;
	DWORD dwClass;
	int nOldLevel;
	int nNewLevel;
	int nLevelDiff;
	int nLife;
	int nStamina;
	int nMana;
	int nHitpoints;
	int nCurMana;
	int nCurStamina;

	dwClass = hPlayer->dwClassId;
	nOldLevel = D2CommonStatsUnitGetBaseValue(hPlayer, STATS_LEVEL);
	nNewLevel = D2CommonDataTblsGetLevelFromExperience(dwClass, D2CommonStatsUnitGetValue(hPlayer,
		STATS_EXPERIENCE));
	D2CommonStatsUnitSetValue(hPlayer, STATS_LEVEL, nNewLevel);
	D2CommonStatsUnitSetValue(hPlayer, STATS_NEXTEXP, D2CommonDataTblsGetExperience(dwClass, nNewLevel));
	nLevelDiff = nNewLevel - nOldLevel;
#line 195
	D2ASSERT(nLevelDiff > 0);
	ptCharStats = (CHARSTATSTXT *)D2CommonDataTblsGetCharStats(dwClass);
	nLife = D2CommonStatsUnitGetBaseValue(hPlayer, STATS_MAXHP) + (ptCharStats->LifePerLevel << 6);
	nStamina = D2CommonStatsUnitGetBaseValue(hPlayer, STATS_MAXSTAMINA)
		+ (ptCharStats->StaminaPerLevel << 6);
	nMana = D2CommonStatsUnitGetBaseValue(hPlayer, STATS_MAXMANA) + (ptCharStats->ManaPerLevel << 6);
	D2CommonStatsUnitSetValue(hPlayer, STATS_MAXHP, nLife);
	D2CommonStatsUnitSetValue(hPlayer, STATS_MAXSTAMINA, nStamina);
	D2CommonStatsUnitSetValue(hPlayer, STATS_MAXMANA, nMana);
	nHitpoints = D2CommonStatsUnitCalcMaxHp(hPlayer);
	nCurMana = D2CommonStatsUnitCalcMaxMana(hPlayer);
	nCurStamina = D2CommonStatsUnitCalcMaxStamina(hPlayer);
	D2CommonStatsUnitSetValue(hPlayer, STATS_HITPOINTS, nHitpoints);
	D2CommonStatsUnitSetValue(hPlayer, STATS_MANA, nCurMana);
	D2CommonStatsUnitSetValue(hPlayer, STATS_STAMINA, nCurStamina);
	D2CommonStatsUnitAddValue(hPlayer, STATS_STATPTS, nLevelDiff * 5);
	D2CommonStatsUnitAddValue(hPlayer, STATS_NEWSKILLS, nLevelDiff);
	SUnitProxyLevelUp(ptGame, hPlayer, nNewLevel);
	MonsterAIPetLevelUp(ptGame, hPlayer);
	SUnitSetEvent(hPlayer, 2, hPlayer);
	SUnitForEachPlayer(ptGame, PlrEvalLevelUpEach, hPlayer);
	if (ptEventCallbackTable && ptEventCallbackTable->pfnLevelUp)
		((PFNEVENTLEVELUP)ptEventCallbackTable->pfnLevelUp)(ptGame->wGameId, (DWORD)D2CommonUnitGetPlayerName(hPlayer),
			(WORD)hPlayer->dwClassId, nNewLevel);
}

void __fastcall PlrEvalLevelUpEach(GAME *ptGame, UNIT *hUnit, void *pArg)
{
	SCmdSendPlayerInfo(hUnit, (UNIT *)pArg);
}

BOOL __fastcall PlrEvalAddStatPoint(UNIT *hPlayer, int nStat)
{
	CHARSTATSTXT *ptCharStats;

	if (!D2CommonStatsUnitGetValue(hPlayer, STATS_STATPTS))
		return TRUE;
	switch (nStat)
	{
	case STATS_STRENGTH:
		if (D2CommonStatsUnitGetValue(hPlayer, STATS_STATPTS))
		{
			D2CommonStatsUnitAddValue(hPlayer, STATS_STATPTS, -1);
			D2CommonStatsUnitAddValue(hPlayer, STATS_STRENGTH, 1);
		}
		break;
	case STATS_ENERGY:
		if (D2CommonStatsUnitGetValue(hPlayer, STATS_STATPTS))
		{
			ptCharStats = (CHARSTATSTXT *)D2CommonDataTblsGetCharStats(hPlayer->dwClassId);
			D2CommonStatsUnitAddValue(hPlayer, STATS_STATPTS, -1);
			D2CommonStatsUnitAddValue(hPlayer, STATS_ENERGY, 1);
			D2CommonStatsUnitAddValue(hPlayer, STATS_MANA, ptCharStats->ManaPerMagic << 6);
			D2CommonStatsUnitAddValue(hPlayer, STATS_MAXMANA, ptCharStats->ManaPerMagic << 6);
		}
		break;
	case STATS_DEXTERITY:
		if (D2CommonStatsUnitGetValue(hPlayer, STATS_STATPTS))
		{
			D2CommonStatsUnitAddValue(hPlayer, STATS_STATPTS, -1);
			D2CommonStatsUnitAddValue(hPlayer, STATS_DEXTERITY, 1);
		}
		break;
	case STATS_VITALITY:
		if (D2CommonStatsUnitGetValue(hPlayer, STATS_STATPTS))
		{
			ptCharStats = (CHARSTATSTXT *)D2CommonDataTblsGetCharStats(hPlayer->dwClassId);
			D2CommonStatsUnitAddValue(hPlayer, STATS_STATPTS, -1);
			D2CommonStatsUnitAddValue(hPlayer, STATS_VITALITY, 1);
			D2CommonStatsUnitAddValue(hPlayer, STATS_MAXHP, ptCharStats->LifePerVitality << 6);
			D2CommonStatsUnitAddValue(hPlayer, STATS_HITPOINTS, ptCharStats->LifePerVitality << 6);
			D2CommonStatsUnitAddValue(hPlayer, STATS_MAXSTAMINA, ptCharStats->StaminaPerVitality << 6);
			D2CommonStatsUnitAddValue(hPlayer, STATS_STAMINA, ptCharStats->StaminaPerVitality << 6);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
