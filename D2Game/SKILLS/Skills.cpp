#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\SKILLS\\Skills.cpp"

#include "../D2game.h"
#include "D2Macros.h"
#include "../AI/AiGeneral.h"
#include "../AI/AiThink.h"
#include "../GAME/Event.h"
#include "../GAME/Game.h"
#include "../GAME/SCmd.h"
#include "../GUILDS/Guilds.h"
#include "../ITEMS/ItemMode.h"
#include "../ITEMS/Items.h"
#include "../MISSILES/Missiles.h"
#include "../MONSTER/MonsterMode.h"
#include "../MONSTER/MonsterUnique.h"
#include "../PLAYER/PlayerPets.h"
#include "../PLAYER/PlrModes.h"
#include "../QUESTS/QUESTS.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "SkillAma.h"
#include "SkillBar.h"
#include "SkillItem.h"
#include "SkillMonst.h"
#include "SkillNec.h"
#include "SkilPal.h"
#include "Skills.h"
#include "SkillSor.h"

enum { NUM_COMMON_SKILLS = 6 };
enum { NUM_SKILL_CLASSES = 7 };
#define SKILLPAGE_LIGHTNING		2
#define STAT_NEWSKILLS			5
#define STAT_MANA				8
#define STAT_TOHIT				19
#define STAT_MINDAMAGE			21
#define STAT_MAXDAMAGE			22
#define STAT_FIRERESIST			39
#define STAT_LIGHTRESIST		41
#define STAT_COLDRESIST			43
#define STAT_43					67
#define STAT_QUANTITY			70
#define STAT_7D					125
#define STAT_ITEMAURA_BLESSEDAIM	151
#define STAT_ITEMAURA_DEFIANCE	152
#define STAT_9D					157
#define STATE_LIGHTNINGMASTERY	72
#define STATE_THUNDERSTORM		38
#define EVENT_PERIODIC			8
#define EVENT_AURA				9
#define EVENT_STATEEND			12
#define BODYLOC_RARM			4
#define BODYLOC_LARM			5
#define ITEMTYPE_THROWING		38
#define ITEMTYPE_1B				27
#define ITEMTYPE_23				35
#define MISSFLAG_POSITION		1
#define MISSFLAG_TARGETABS		32
#define MISSFLAG_ARRIVE			1024
#define MISSFLAG_STAT13			4096
#define MISSFLAG_TARGETREL		2
#define MISSFLAG_VELOCITY		4
#define LEVEL_27				39
#define STATE_55				85
#define STATE_STUNNED			21
#define UNIT_STATE_UNINTERRUPTABLE	54
#define EVENT_SKILLCHANGE		23
#define UNIT_OBJECT				2
#define NUM_NOVA_MISSILES		64
#define MAX_SKILL_LEVEL			20
#define DrlgGetActNum(nLevelId)	D2CommonGetActNum(nLevelId)
#define StatsGetState(hUnit, nState) \
	D2CommonStatsGetStateRaw((DWORD)(hUnit), (DWORD)(nState))

typedef BOOL (__fastcall *PFNSKILLCLASSFN)(GAME *ptGame, UNIT *hUnit, int nSkillId, int nLevel);

typedef void (__fastcall *PFNSKILLPASSON)(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);

struct SKILLRANGE
{
 	int			nStart;
 	int			nCount;
};
typedef struct SKILLRANGE SKILLRANGE;

static const SKILLRANGE sgatSkillRanges[NUM_SKILL_CLASSES] =
{
	{ 6, 30 },
	{ 36, 30 },
	{ 66, 30 },
	{ 96, 30 },
	{ 126, 30 },
	{ 156, 61 },
	{ 217, 4 },
};

extern const int PLACEHOLDER_UnreferencedData_100a6a48[8] = { -1, 1, 1, -1, -1, -1, 1, 1 };
extern const int PLACEHOLDER_UnreferencedData_100a6a68[14] = { 18, 20, 17, 20, 15, 19, 18, 8, 2, 11, 4, 13, 6, 9 };
extern const int PLACEHOLDER_UnreferencedData_100a6aa0[16] = { 20, -20, 0, 0, 14, -14, -14, 14, 0, 0, 20, -20, 14, 14, -14, -14 };

static PFNSKILLFN sgppfnCommonSkillChart[NUM_COMMON_SKILLS][2] =
{
	{ SkillsAttackStart, SkillsAttack },
	{ SkillsKickStart, SkillsUseHit },
	{ NULL, SkillsThrow },
	{ SkillsUnsummonStart, SkillsUnsummon },
	{ NULL, SkillsLeftHandThrow },
	{ SkillsAttackStart, SkillsLeftHandSwing },
};

static PFNSKILLCLASSFN sgpfnSkillPassOn[NUM_SKILL_CLASSES][2] =
{
	{ SkillsAmaStart, SkillsAmaUse },
	{ SkillsSorStart, SkillsSorUse },
	{ SkillsNecStart, SkillsNecUse },
	{ SkillsPalStart, SkillsPalUse },
	{ SkillsBarStart, SkillsBarUse },
	{ SkillsMonstStart, SkillsMonstUse },
	{ SkillsItemStart, SkillsItemUse },
};

struct SKILLSTATEROW
{
 	int			nSkill;
 	int			nState;
 	PFNSKILLPASSON	pfnPassOn;
};
typedef struct SKILLSTATEROW SKILLSTATEROW;

static SKILLSTATEROW sgtSkillStates[] =
{
	{ 98, 33, SkillsMightAffect },
	{ 99, 34, NULL },
	{ 100, 3, SkillsResistFireAffect },
	{ 102, 35, NULL },
	{ 103, 36, NULL },
	{ 104, 37, SkillsDefianceAffect },
	{ 105, 4, SkillsResistColdAffect },
	{ 108, 40, SkillsBlessedAimAffect },
	{ 115, 41, SkillsVigorAffect },
	{ 110, 5, SkillsResistLightningAffect },
	{ 113, 42, SkillsConcentrationAffect },
	{ 114, 43, NULL },
	{ 109, 45, NULL },
	{ 118, 46, NULL },
	{ 119, 47, NULL },
	{ 120, 48, SkillsMeditationAffect },
	{ 122, 49, NULL },
	{ 124, 50, NULL },
	{ 123, 28, NULL },
	{ 125, 8, SkillsSalvationAffect },
};
static int sgnSkillStates = sizeof(sgtSkillStates) / sizeof(sgtSkillStates[0]);

UNIT * __fastcall SkillsFindNearest(GAME *ptGame, UNIT *hUnit, int nRange, PFNSKILLUNITTEST pfnTest)
{
	UNITFINDARGS tArgs;
	UNITFINDDATA tFind;
	DWORD hRoom;
	UNIT *hNearest;
	int nX;
	int nY;
	int nBest;
	int nDX;
	int nDY;
	int nDistance;
	int i;

	hRoom = D2CommonUnitGetPathRoom(hUnit);
	hNearest = NULL;
	nBest = nRange * nRange * 4;
	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	for (i = 0; i < (int)(sizeof(UNITFINDARGS) / sizeof(DWORD)); i++)
		((DWORD *)&tArgs)[i] = 0;
	tArgs.dwFlags = 1923;
	tArgs.hExcluded = hUnit;
	tArgs.nX = nX;
	tArgs.nY = nY;
	tArgs.nDistance = nRange;
	tArgs.pFieldPair = (void *)D2CommonFieldTblsAllocPair();
	D2CommonFieldTblsSetPair(tArgs.pFieldPair, nX, nY);
	D2CommonUnitFindsInit(ptGame->pMemPool, &tFind, hRoom, nX, nY, nRange, 0, &tArgs);
	D2CommonUnitFindsFindUnits(&tFind);
	for (i = 0; i < tFind.nNumUnits; i++)
	{
		if (!pfnTest(ptGame, tFind.phUnitsArray[i]))
			continue;
		if (!SUnitIsHostile(ptGame, hUnit, tFind.phUnitsArray[i]))
			continue;
		nDX = (int)D2CommonUnitGetX(tFind.phUnitsArray[i]) - nX;
		nDY = (int)D2CommonUnitGetY(tFind.phUnitsArray[i]) - nY;
		nDistance = nDY * nDY + nDX * nDX;
		if (nDistance < nBest)
		{
			nBest = nDistance;
			hNearest = tFind.phUnitsArray[i];
		}
	}
	D2CommonUnitFindsFreeArray(&tFind);
	D2CommonFieldTblsFreePair(tArgs.pFieldPair);
	return hNearest;
}

BOOL __fastcall SkillsUseMana(UNIT *hUnit)
{
	SKILLSTXT *ptData;
	DWORD hSkill;
	int nLevel;
	int nMana;
	int nBonus;
	int nSkillId;

	if (hUnit->eType != UNIT_PLAYER)
		return TRUE;
	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	if (!hSkill)
		return FALSE;
	ptData = (SKILLSTXT *)D2CommonSkillsGetNodeTypeRecord(hSkill);
	nLevel = (int)D2CommonSkillsCalcNodeLevel(hUnit, hSkill, 1) - 1;
	if (!ptData->mana && !ptData->lvlmana)
		return FALSE;
#line 98
	D2ASSERT(ptData);
	if (nLevel < 0)
		nLevel = 0;
	nMana = ptData->lvlmana * nLevel + ptData->mana;
	if (StatsGetState(hUnit, STATE_LIGHTNINGMASTERY) && ptData->skillpage == SKILLPAGE_LIGHTNING)
	{
		nBonus = D2CommonSkillsCalcCurveParamOne(D2CommonSkillsCalcNodeLevel(hUnit,
			D2CommonSkillsFindByKey(hUnit, SKILL_LIGHTNING_MASTERY), 1), SKILL_LIGHTNING_MASTERY);
		nMana += nBonus * nMana / -100;
	}
	nMana <<= ptData->manashift;
	nSkillId = (int)D2CommonSkillsGetNodeType(hSkill);
	if (nSkillId == SKILL_AMAZON_START || nSkillId == SKILL_INFERNO)
	{
		if (nMana < 0)
			nMana = 0;
	}
	else if (nMana < 256)
		nMana = 256;
	if (D2CommonStatsUnitGetValue(hUnit, STAT_MANA) >= nMana)
	{
		D2CommonStatsUnitAddValue(hUnit, STAT_MANA, -nMana);
		return TRUE;
	}
	return FALSE;
}

BOOL __fastcall SkillsSpendMana(UNIT *hUnit, int nMana)
{
	if (hUnit->eType != UNIT_PLAYER)
		return TRUE;
	if (D2CommonStatsUnitGetValue(hUnit, STAT_MANA) >= nMana)
	{
		D2CommonStatsUnitAddValue(hUnit, STAT_MANA, -nMana);
		return TRUE;
	}
	return FALSE;
}

BOOL __fastcall SkillsHasMana(UNIT *hUnit)
{
	SKILLSTXT *ptData;
	DWORD hSkill;
	int nLevel;
	int nMana;

	if (hUnit->eType != UNIT_PLAYER)
		return TRUE;
	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	if (!hSkill)
		return FALSE;
	ptData = (SKILLSTXT *)D2CommonSkillsGetNodeTypeRecord(hSkill);
	nLevel = (int)D2CommonSkillsCalcNodeLevel(hUnit, hSkill, 1) - 1;
#line 173
	D2ASSERT(ptData);
	if (nLevel < 0)
		nLevel = 0;
	nMana = (ptData->lvlmana * nLevel + ptData->mana) << ptData->manashift;
	return D2CommonStatsUnitGetValue(hUnit, STAT_MANA) >= nMana;
}

UNITFINDDATA * __fastcall SkillsFindUnits(GAME *ptGame, UNIT *hUnit, int nRange, int nX, int nY)
{
	UNITFINDARGS tArgs;
	UNITFINDDATA *ptFind;
	DWORD hRoom;
	int i;

#line 191
	ptFind = (UNITFINDDATA *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(UNITFINDDATA));
	for (i = 0; i < (int)(sizeof(UNITFINDARGS) / sizeof(DWORD)); i++)
		((DWORD *)&tArgs)[i] = 0;
	tArgs.dwFlags = 1411;
	tArgs.hExcluded = hUnit;
	tArgs.nX = nX;
	tArgs.nY = nY;
	tArgs.nDistance = nRange;
	hRoom = D2CommonUnitGetPathRoom(hUnit);
	D2CommonUnitFindsInit(ptGame->pMemPool, ptFind, hRoom, nX, nY, nRange, 0, &tArgs);
	D2CommonUnitFindsFindUnits(ptFind);
	return ptFind;
}

void __fastcall SkillsFreeUnits(GAME *ptGame, UNITFINDDATA *ptFind)
{
	D2CommonUnitFindsFreeArray(ptFind);
#line 217
	D2FREEPOOL(ptGame->pMemPool, ptFind);
}

BOOL __fastcall SkillsUseHit(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;

	hUnit->dwFlags0 |= 64;
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	SUnitDmgDeliver(ptGame, hUnit, hTarget);
	return TRUE;
}

void __fastcall SkillsStateRemove(UNIT *hUnit, DWORD dwState)
{
	if (SUnitIsDead(hUnit) && D2CommonStatsUnitTestStateKept(hUnit, dwState))
		return;
	D2CommonStatsUnitSetActiveStateBit(hUnit, dwState, 0);
}

void * __fastcall SkillsSetState(SKILLSTATE *ptData)
{
	MONSTATSTXT *ptMonStats;
	GAME *ptGame;
	DWORD hList;
	DWORD dwFlags;
	BOOL bFound;
	BOOL bStat;
	int nOld;
	int nEnd;
	int nValue;
	DWORD dwState;

	dwState = ptData->dwState;
	dwFlags = 0;
	if (D2CommonStatsTestStateSharedList(dwState))
		dwFlags = 32;
	if (D2CommonStatsTestStateOwned(dwState))
		dwFlags |= 2048;
	ptGame = SUnitGetGame(ptData->hTarget);
	if (ptData->hTarget->eType == UNIT_MONSTER)
	{
		ptMonStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(ptData->hTarget->dwClassId);
		if (ptMonStats)
		{
			if (ptMonStats->bIsNPC)
				return NULL;
			if (!ptMonStats->bIsAtt)
				return NULL;
		}
	}
	D2CommonUnitRoomAddToChangedList(ptData->hTarget);
	if (dwFlags)
		hList = D2CommonStatsFindListByMask(ptData->hTarget, dwFlags);
	else
		hList = D2CommonStatsFindListByState(ptData->hTarget, ptData->dwState);
	bFound = FALSE;
	bStat = D2CommonStatsTestStateStoresStat(ptData->dwState);
	nOld = 0;
	if (hList)
	{
		if (D2CommonStatsGetListState(hList) == ptData->dwState)
		{
			bFound = TRUE;
			if (bStat)
				nOld = (int)D2CommonStatsListGetValueA(hList, ptData->dwStatId);
		}
		D2CommonStatsUnitUnlinkList(ptData->hTarget, hList);
		D2CommonStatsListFree(hList);
	}
	D2CommonStatsUnitSetActiveStateBit(ptData->hTarget, ptData->dwState, 1);
	if (bFound && (!bStat || ptData->nStatValue == nOld))
		D2CommonStatsUnitSetActiveStateValue(ptData->hTarget, ptData->dwState, 0);
	dwFlags |= 2;
	nEnd = ptGame->nFrame + ptData->nDuration;
	hList = D2CommonStatsAllocList(ptData->hTarget->dwMemPool, dwFlags, nEnd, ptData->hSource->eType,
		ptData->hSource->dwId);
	EventAdd(ptGame, ptData->hTarget, EVENT_STATEEND, nEnd);
	D2CommonStatsSetListState(hList, ptData->dwState);
	if (bStat)
	{
		nValue = ptData->nStatValue;
		switch (ptData->nSkill)
		{
		case SKILL_SALVATION:
			D2CommonStatsListSetValueB(hList, STAT_FIRERESIST, nValue);
			D2CommonStatsListSetValueB(hList, STAT_COLDRESIST, nValue);
			D2CommonStatsListSetValueB(hList, STAT_LIGHTRESIST, nValue);
			break;
		case SKILL_FETISHAURA:
			D2CommonStatsListSetValueB(hList, STAT_43, nValue);
			break;
		default:
			D2CommonStatsListSetValueB(hList, ptData->dwStatId, nValue);
			break;
		}
	}
	D2CommonStatsUnitLinkList(ptData->hTarget, hList);
	if (!ptData->pfnRemove)
	{
		D2CommonStatsSetListRemoveCallback(hList, SkillsStateRemove);
		return (void *)hList;
	}
#line 338
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)ptData->pfnRemove), "ptData->pfnRemove");
	D2CommonStatsSetListRemoveCallback(hList, ptData->pfnRemove);
	return (void *)hList;
}

void * __fastcall SkillsSetSkillState(UNIT *hSource, UNIT *hTarget, int nSkill, int nLevel, int nStat,
	int nState, PFNSTATEREMOVE pfnRemove)
{
	SKILLSTATE tData;
	SKILLSTXT *ptSkillsTxt;
	int i;

	ptSkillsTxt = (SKILLSTXT *)D2CommonDataTblsGetSkill(nSkill);
	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tData)[i] = 0;
	tData.hSource = hSource;
	tData.hTarget = hTarget;
	tData.nSkill = nSkill;
	tData.nLevel = nLevel;
	tData.nDuration = ptSkillsTxt->Param[1] * nLevel;
	tData.dwStatId = (DWORD)nStat;
	tData.nStatValue = ptSkillsTxt->Param[2] * (nLevel - 1) + ptSkillsTxt->Param[0];
	tData.dwState = (DWORD)nState;
	tData.pfnRemove = (DWORD)pfnRemove;
	return SkillsSetState(&tData);
}

int __fastcall SkillsUseQuantity(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	int nQuantity;
	int nType;
	int nResult;

	if (hItem)
	{
		nQuantity = D2CommonStatsUnitGetValue(hItem, STAT_QUANTITY) - 1;
		if (nQuantity < 0)
		{
			nQuantity = 0;
		}
		D2CommonStatsUnitSetValue(hItem, STAT_QUANTITY, nQuantity);
		nType = (int)D2CommonITEMSDataGetModType(hItem);
		if ((nType == 4 || nType == 6 || nType == 7) && nQuantity <= 0)
		{
			D2CommonStatsUnitAddValue(hItem, STAT_QUANTITY, 1);
			SkillsSetCurrent(hUnit, TRUE, (int)D2CommonUnitGetPlayerLeftSkill(hUnit));
		}
		SCmdSendItemStat(SUnitGetClient(hUnit), hItem, 1, STAT_QUANTITY, (DWORD)nQuantity);
		nResult = nQuantity + 1;
	}
	else
	{
		nResult = 0;
	}
	return nResult;
}

UNIT * __fastcall SkillsCreateMissile(GAME *ptGame, int nMissile, UNIT *hUnit, int nLevel, int nOffsetX,
	int nOffsetY, int nTargetX, int nTargetY, BOOL bUseQuantity)
{
	MISSILECREATE tCreateMissile;
	UNIT *hTarget;
	void *ptPath;
	int nValue;
	int i;

	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreateMissile)[i] = 0;
	tCreateMissile.dwFlags = MISSFLAG_TARGETABS | MISSFLAG_POSITION;
	tCreateMissile.hSource = hUnit;
	tCreateMissile.nX = (int)D2CommonUnitGetX(hUnit) + nOffsetX;
	tCreateMissile.nY = (int)D2CommonUnitGetY(hUnit) + nOffsetY;
	tCreateMissile.eMissile = nMissile;
	tCreateMissile.nLevel = nLevel;
	if (nTargetX && nTargetY)
	{
		tCreateMissile.nDX = nTargetX;
		tCreateMissile.nDY = nTargetY;
	}
	else
	{
		hTarget = SUnitGetTarget(ptGame, hUnit);
		if (hTarget)
		{
			tCreateMissile.nDX = (int)D2CommonUnitGetX(hTarget);
			tCreateMissile.nDY = (int)D2CommonUnitGetY(hTarget);
		}
		else
		{
			ptPath = hUnit->ptPath;
			tCreateMissile.nDX = (int)D2CommonPathGetTargetXRaw((DWORD)ptPath);
			tCreateMissile.nDY = (int)D2CommonPathGetTargetYRaw((DWORD)ptPath);
		}
		if (!tCreateMissile.nDX || !tCreateMissile.nDY)
			return NULL;
	}
	D2_OPAQUE(tCreateMissile.nDX);
	D2_OPAQUE(tCreateMissile.nDY);
#line 429
	D2ASSERT(tCreateMissile.nDX && tCreateMissile.nDY);
	if (hUnit->eType == UNIT_MONSTER)
	{
		nValue = D2CommonStatsUnitGetValue(hUnit, STAT_TOHIT);
		if (nValue)
		{
			tCreateMissile.nToHit = nValue;
			tCreateMissile.dwFlags |= MISSFLAG_STAT13;
		}
	}
	if (hUnit && bUseQuantity && hUnit->eType == UNIT_PLAYER && SkillsUseThrowQuantity(ptGame, hUnit) <= 0)
		return NULL;
	return MissilesCreate(ptGame, &tCreateMissile);
}

UNIT * __fastcall SkillsCreateMissileAt(GAME *ptGame, UNIT *hUnit, int nLevel, int nMissile, int nX, int nY)
{
	MISSILECREATE tCreate;
	UNIT *hTarget;
	int i;

	void		* cX_ptPath;
	if (!nX && !nY)
	{
		hTarget = SUnitGetTarget(ptGame, hUnit);
		if (hTarget)
		{
			nX = (int)D2CommonUnitGetX(hTarget);
			nY = (int)D2CommonUnitGetY(hTarget);
		}
		else
		{
			cX_ptPath = hUnit->ptPath;
			nX = (int)D2CommonPathGetTargetXRaw((DWORD)cX_ptPath);
			nY = (int)D2CommonPathGetTargetYRaw((DWORD)cX_ptPath);
		}
		if (!nX && !nY)
			return NULL;
	}
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = MISSFLAG_POSITION;
	tCreate.nY = nY;
	tCreate.hSource = hUnit;
	tCreate.nX = nX;
	tCreate.nLevel = nLevel;
	tCreate.eMissile = nMissile;
	return MissilesCreate(ptGame, &tCreate);
}

UNIT * __fastcall SkillsThrowMissile(GAME *ptGame, int nMissile, UNIT *hUnit, int nLevel, BOOL bUseQuantity)
{
	MISSILECREATE tCreate;
	UNIT *hTarget;
	int i;

	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = MISSFLAG_ARRIVE | MISSFLAG_TARGETABS;
	tCreate.hSource = hUnit;
	tCreate.hSpawnLocation = hUnit;
	tCreate.nLevel = nLevel;
	tCreate.eMissile = nMissile;
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget)
	{
		tCreate.nDX = (int)D2CommonUnitGetX(hTarget);
		tCreate.nDY = (int)D2CommonUnitGetY(hTarget);
	}
	else
	{
		tCreate.nDX = (int)D2CommonPathGetTargetXRaw((DWORD)hUnit->ptPath);
		tCreate.nDY = (int)D2CommonPathGetTargetYRaw((DWORD)hUnit->ptPath);
	}
	if (hUnit && bUseQuantity && hUnit->eType == UNIT_PLAYER && SkillsUseThrowQuantity(ptGame, hUnit) <= 0)
		return NULL;
	return MissilesCreate(ptGame, &tCreate);
}

int __fastcall SkillsUseThrowQuantity(GAME *ptGame, UNIT *hUnit)
{
	DWORD hWeapon;
	DWORD pInventory;
	DWORD hItem;
	BOOL bSameHand;
	int nType;

	if (hUnit->eType != UNIT_PLAYER)
		return 0;
	hWeapon = D2CommonUnitGetActiveWeapon(hUnit);
	bSameHand = TRUE;
	nType = (int)D2CommonITEMSGetType(hWeapon);
	if (nType == ITEMTYPE_1B || nType == ITEMTYPE_23)
		bSameHand = FALSE;
	pInventory = (DWORD)hUnit->ptInventory;
	hItem = D2CommonINVENTORYGetLocSlot(pInventory, BODYLOC_RARM);
	if (hItem && (!bSameHand || hItem == hWeapon)
		&& (D2CommonITEMSGetTypeStackable(hItem) || D2CommonITEMSGetTypeThrowable(hItem)
			|| D2CommonStatsGetTotalValue(hItem, STAT_7D)))
		return SkillsUseQuantity(ptGame, hUnit, (UNIT *)hItem);
	hItem = D2CommonINVENTORYGetLocSlot(pInventory, BODYLOC_LARM);
	if (hItem && (!bSameHand || hItem == hWeapon)
		&& (D2CommonITEMSGetTypeStackable(hItem) || D2CommonITEMSGetTypeThrowable(hItem)
			|| D2CommonStatsGetTotalValue(hItem, STAT_7D)))
		return SkillsUseQuantity(ptGame, hUnit, (UNIT *)hItem);
	return 0;
}

BOOL __fastcall SkillsHasThrowQuantity(UNIT *hUnit)
{
	DWORD hWeapon;
	DWORD pInventory;
	DWORD hItem;
	DWORD hOther;
	BOOL bSameHand;
	int nType;

	if (hUnit->eType != UNIT_PLAYER)
		return FALSE;
	hWeapon = D2CommonUnitGetActiveWeapon(hUnit);
	bSameHand = TRUE;
	nType = (int)D2CommonITEMSGetType(hWeapon);
	if (nType == ITEMTYPE_1B || nType == ITEMTYPE_23)
	{
		bSameHand = FALSE;
		if (!D2CommonSkillsGetNodeType(D2CommonUnitGetSkillThirdCached(hUnit))
			&& D2CommonStatsGetTotalValue(hWeapon, STAT_9D))
			return TRUE;
	}
	pInventory = (DWORD)hUnit->ptInventory;
	hItem = D2CommonINVENTORYGetLocSlot(pInventory, BODYLOC_RARM);
	if (hItem && (!bSameHand || hItem == hWeapon)
		&& (D2CommonITEMSGetTypeStackable(hItem) || D2CommonITEMSGetTypeThrowable(hItem)
			|| D2CommonStatsGetTotalValue(hItem, STAT_7D)))
		return D2CommonITEMSValidateQuantity(hItem);
	hOther = D2CommonINVENTORYGetLocSlot(pInventory, BODYLOC_LARM);
	if (hOther && (!bSameHand || hItem == hWeapon)
		&& (D2CommonITEMSGetTypeStackable(hOther) || D2CommonITEMSGetTypeThrowable(hOther)
			|| D2CommonStatsGetTotalValue(hOther, STAT_7D)))
		return D2CommonITEMSValidateQuantity(hOther);
	return FALSE;
}

BOOL __fastcall SkillsAttackStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	DWORD hWeapon;
	int i;

	hUnit->nAnimPos = (int)(D2CommonUnitGetWClassModeValue(hUnit) << 8);
	if (D2CommonCOMPOSITIsRangedAttack(hUnit))
	{
		if (!SkillsHasThrowQuantity(hUnit)) { PlrModeWearWeapons(ptGame, hUnit); PlrModeBreakItems(ptGame, hUnit); return FALSE; }
		return TRUE;

	}
	hWeapon = D2CommonUnitGetActiveWeapon(hUnit);
	if (hWeapon && D2CommonITEMSGetType(hWeapon) == ITEMTYPE_THROWING)
		return FALSE;
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget, 0);
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	return TRUE;
}

BOOL __fastcall SkillsAttack(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;
	int nMissile;

	hUnit->dwFlags0 |= 64;
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (D2CommonCOMPOSITIsRangedAttack(hUnit))
	{
		nMissile = (int)D2CommonSkillsGetRangedAttackCost(hUnit);
		switch (nMissile)
		{
		case MISSILE_ARROW:
		case MISSILE_BOLT:
		case MISSILE_EXPLODINGARROW:
			SkillsCreateMissile(ptGame, nMissile, hUnit, nLevel, 0, 0, 0, 0, TRUE);
			return TRUE;
		case MISSILE_MAGICARROW:
			SkillsCreateMissile(ptGame, MISSILE_MAGICARROW, hUnit, nLevel, 0, 0, 0, 0, FALSE);
			return TRUE;
		}
	}
	else if (hTarget)
		SUnitDmgDeliver(ptGame, hUnit, hTarget);
	return TRUE;
}

void __fastcall SkillsThrowDamage(UNIT *hUnit, UNIT *hMissile, UNIT *hItem)
{
	int nMin;
	int nMax;
	int nToHit;
	int nValue;

	if (!hMissile)
		return;
	nMin = D2CommonStatsUnitGetValue(hMissile, STAT_MINDAMAGE);
	nMax = D2CommonStatsUnitGetValue(hMissile, STAT_MAXDAMAGE);
	nToHit = D2CommonStatsUnitGetValue(hMissile, STAT_TOHIT);
	nValue = D2CommonSkillsGetItemSkillValue(hUnit, 0, hItem, 0, 0);
	nMin += nValue * nMin / 100;
	nMax += nValue * nMax / 100;
	nValue = D2CommonSkillsGetItemSkillValue(hUnit, 0, hItem, 0, 1);
	D2CommonStatsUnitSetValue(hMissile, STAT_MINDAMAGE, nMin);
	D2CommonStatsUnitSetValue(hMissile, STAT_MAXDAMAGE, nMax);
	D2CommonStatsUnitSetValue(hMissile, STAT_TOHIT, nValue + nToHit);
}

BOOL __fastcall SkillsThrow(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DWORD pInventory;
	DWORD hItem;
	int nBodyLoc;
	int nFound;
	int nMissile;
	UNIT *hMissile;

	pInventory = (DWORD)hUnit->ptInventory;
	hItem = 0;
	if (D2CommonINVENTORYGetWeapon(pInventory, &hItem, &nBodyLoc, &nFound))
	{
		if (!nFound)
		{
			if (nBodyLoc == BODYLOC_LARM)
				hItem = D2CommonINVENTORYGetLocSlot(pInventory, BODYLOC_RARM);
			else
				hItem = D2CommonINVENTORYGetLocSlot(pInventory, BODYLOC_LARM);
		}
		if (!D2CommonITEMSGetTypeThrowable(hItem) && !D2CommonStatsGetTotalValue(hItem, STAT_7D))
			return FALSE;
	}
	if (!hItem)
		return FALSE;
	hUnit->dwFlags0 |= 64;
	nMissile = (int)D2CommonITEMSGetTypeMissileType(hItem);
	if (D2CommonITEMSGetType(hItem) == ITEMTYPE_THROWING)
		hMissile = SkillsThrowMissile(ptGame, nMissile, hUnit, nLevel, TRUE);
	else
		hMissile = SkillsCreateMissile(ptGame, nMissile, hUnit, nLevel, 0, 0, 0, 0, TRUE);
	SkillsThrowDamage(hUnit, hMissile, (UNIT *)hItem);
	return TRUE;
}

BOOL __fastcall SkillsKickStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	SKILLSTXT *ptSkillsTxt;
	UNIT *hTarget;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	ptSkillsTxt = (SKILLSTXT *)D2CommonDataTblsGetSkill(SKILL_KICK);
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = 9;
	tDamage.dwFlags0 = 2;
	tDamage.nPhysicalDamage = ptSkillsTxt->MinDam << ptSkillsTxt->HitShift;
	tDamage.dwHitClass = 1;
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	return TRUE;
}

BOOL __fastcall SkillsUnsummonStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;
	DWORD hSkill;
	int nType;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget || hTarget->eType != UNIT_MONSTER || AiGetOwner(hTarget) != hUnit)
		return FALSE;
	nType = PlayerPetsGetType(hUnit, hTarget->dwId);
	if (nType < 3 || nType > 6)
		return FALSE;
	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	if (!hSkill)
		return FALSE;
	D2CommonSkillsSetNodeScratchC(hSkill, hTarget->dwId);
	return TRUE;
}

BOOL __fastcall SkillsUnsummon(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DWORD hSkill;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	if (!hSkill)
		return FALSE;
	PlayerPetsRemove(ptGame, hUnit, D2CommonSkillsGetNodeScratchC(hSkill), TRUE);
	return TRUE;
}

BOOL __fastcall SkillsLeftHandSwing(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;

	hUnit->dwFlags0 |= 64;
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (D2CommonCOMPOSITIsRangedAttack(hUnit))
	{
		SkillsCreateMissile(ptGame, MISSILE_ARROW, hUnit, nLevel, 0, 0, 0, 0, TRUE);
		return TRUE;
	}
	if (hTarget)
		SUnitDmgDeliver(ptGame, hUnit, hTarget);
	return TRUE;
}

BOOL __fastcall SkillsLeftHandThrow(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DWORD pInventory;
	DWORD hItem;
	int nBodyLoc;
	int nFound;
	int nMissile;
	UNIT *hMissile;

	pInventory = (DWORD)hUnit->ptInventory;
	hItem = 0;
	if (D2CommonINVENTORYGetWeapon(pInventory, &hItem, &nBodyLoc, &nFound))
	{
		if (nFound)
		{
			if (nBodyLoc == BODYLOC_LARM)
				hItem = D2CommonINVENTORYGetLocSlot(pInventory, BODYLOC_RARM);
			else
				hItem = D2CommonINVENTORYGetLocSlot(pInventory, BODYLOC_LARM);
		}
		if (!D2CommonITEMSGetTypeThrowable(hItem) && !D2CommonStatsGetTotalValue(hItem, STAT_7D))
			return FALSE;
	}
	if (!hItem)
		return FALSE;
	nMissile = (int)D2CommonITEMSGetTypeMissileType(hItem);
	if (D2CommonITEMSGetType(hItem) == ITEMTYPE_THROWING)
		hMissile = SkillsThrowMissile(ptGame, nMissile, hUnit, nLevel, TRUE);
	else
		hMissile = SkillsCreateMissile(ptGame, nMissile, hUnit, nLevel, 0, 0, 0, 0, TRUE);
	SkillsThrowDamage(hUnit, hMissile, (UNIT *)hItem);
	return TRUE;
}

void __fastcall SkillsTransmogrify(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	hUnit->dwFlags &= ~64;
	if (hItem && ITEMSModeTransmogrify(ptGame, hUnit, hItem))
		SUnitSetEvent(hUnit, 4, hUnit);
	SCmdSendSpellIcon(SUnitGetClient(hUnit), NULL, 255, 1, 0, 65535);
}

BOOL __fastcall SkillsIsClassSkill(UNIT *hUnit, int nSkillId)
{
	DWORD dwClass = hUnit->dwClassId;

	if (nSkillId < sgatSkillRanges[dwClass].nStart)
		return FALSE;
	return nSkillId < sgatSkillRanges[dwClass].nCount + sgatSkillRanges[dwClass].nStart;
}

BOOL __fastcall SkillsStart(GAME *ptGame, UNIT *hUnit)
{
	SKILLSTXT *pctSkillData;
	DWORD hSkill;
	UNIT *hTarget;
	int nSkillId;
	int nLevel;
	BOOL bResult;
	BOOL fOk = TRUE;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	if (!hSkill)
		return FALSE;
	nSkillId = (int)D2CommonSkillsGetNodeType(hSkill);
	pctSkillData = (SKILLSTXT *)D2CommonDataTblsGetSkill(nSkillId);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget)
	{
		if (pctSkillData->TargetableOnly && !SUnitIsHostile(ptGame, hUnit, hTarget)
				&& !SUnitIsTargetPet(ptGame, hUnit, hTarget, nSkillId)
			&& !SUnitIsTargetAlly(ptGame, hUnit, hTarget, nSkillId))
			fOk = FALSE;
		else if (!(hTarget->dwFlags0 & 2))
			D2CommonUnitSetTarget(hUnit, 0);
		else if (hTarget->eType == UNIT_MONSTER && hTarget->dwMode == MONMODE_DEAD)
		{
			if (!pctSkillData->SelectDead)
				D2CommonUnitSetTarget(hUnit, 0);
		}
		else if (pctSkillData->SelectDead)
			D2CommonUnitSetTarget(hUnit, 0);
	}
	if (!fOk)
	{
		if (hUnit->eType == UNIT_PLAYER)
		{
lTown:
			PlrModeStartXY(ptGame, hUnit, NULL, PLRMODE_TOWN_NEUTRAL, 0, 0, TRUE);
		}
lFalse:
		return FALSE;
	}
	nLevel = (int)D2CommonSkillsCalcNodeLevel(hUnit, hSkill, 1);
	pctSkillData = (SKILLSTXT *)D2CommonDataTblsGetSkill(nSkillId);
#line 1176
	D2ASSERT(pctSkillData);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!(hTarget && !pctSkillData->TargetAlly && SUnitIsTargetAlly(ptGame, hUnit, hTarget, nSkillId)))
	{
		if (!pctSkillData->InTown && D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hUnit)))
		{
			D2CommonUnitSetSkillThirdCached(hUnit, 0);
			switch (hUnit->eType)
			{
			case UNIT_PLAYER:
				goto lTown;
			case UNIT_MONSTER:
				return FALSE;
			}
			goto lFalse; }
		bResult = SkillsDispatch(ptGame, hUnit, nSkillId, nLevel, 0);
		if (hUnit->eType == UNIT_PLAYER && !bResult)
			PlrModeStartXY(ptGame, hUnit, NULL, PLRMODE_TOWN_NEUTRAL, 0, 0, TRUE);
		return bResult; }
	return FALSE;
}

extern BOOL sVerifyPlayerCanDoSkill(UNIT *hUnit, int eSkillId, SKILLSTXT *pctSkillData);
__inline BOOL sVerifyPlayerCanDoSkill(UNIT *hUnit, int eSkillId, SKILLSTXT *pctSkillData)
{
	DWORD dwClass = hUnit->dwClassId;

	return eSkillId >= sgatSkillRanges[dwClass].nStart && eSkillId < sgatSkillRanges[dwClass].nCount + sgatSkillRanges[dwClass].nStart;
}

BOOL __fastcall SkillsDispatch(GAME *ptGame, UNIT *hUnit, int eSkillId, int nLevel, int nTableColumn)
{
	void		* cX_ptPath;
	SKILLSTXT *pctSkillData;
	UNIT *hTarget;
	DWORD dwMask;
	int nX;
	int nY;
	int i;

	pctSkillData = (SKILLSTXT *)D2CommonDataTblsGetSkill(eSkillId);
	if (!nTableColumn && pctSkillData->LineOfSight)
	{
		hTarget = SUnitGetTarget(ptGame, hUnit);
		if (hTarget)
		{
			nX = (int)D2CommonUnitGetX(hTarget);
			nY = (int)D2CommonUnitGetY(hTarget);
		}
		else
		{
			cX_ptPath = hUnit->ptPath;
			nX = (int)D2CommonPathGetTargetXRaw((DWORD)cX_ptPath);
			nY = (int)D2CommonPathGetTargetYRaw((DWORD)cX_ptPath);
		}
		if (nX && nY)
		{
			switch (pctSkillData->LineOfSight)
			{
			case 1:
				dwMask = 4;
				break;
			case 2:
				dwMask = 7177;
				break;
			case 3:
				dwMask = 384;
				break;
			case 4:
				dwMask = 2052;
				break;
			case 5:
				dwMask = 2053;
				break;
			default:
#line 1082
				D2ASSERTMSG(FALSE, "Bad Line of Sight Check(Server)\nTell Ted Bisson");
				break;
			}
			if (!D2CommonSkillsAddFieldB(nX, nY, hUnit, dwMask))
				return FALSE;
		}
	}
	if (eSkillId < NUM_COMMON_SKILLS)
	{
		if (sgppfnCommonSkillChart[eSkillId][nTableColumn])
		{
#line 1093
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgppfnCommonSkillChart[eSkillId][nTableColumn]), "sgppfnCommonSkillChart[eSkillId][nTableColumn]");
		return sgppfnCommonSkillChart[eSkillId][nTableColumn](ptGame, hUnit, nLevel);
		}
		return TRUE;
	}
	if (pctSkillData->ClassReq && hUnit->eType == UNIT_PLAYER)
	{
#line 1099
		D2ASSERTMSG(sVerifyPlayerCanDoSkill(hUnit, eSkillId, pctSkillData), "sVerifyPlayerCanDoSkill( hUnit, eSkillId, pctSkillData )");
	}
	for (i = 0; i < NUM_SKILL_CLASSES; i++)
	{
		if (eSkillId < sgatSkillRanges[i].nStart + sgatSkillRanges[i].nCount
			&& eSkillId >= sgatSkillRanges[i].nStart)
			break;
	}
#line 1106
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgpfnSkillPassOn[i][nTableColumn]), "sgpfnSkillPassOn[i][nTableColumn]");
	return sgpfnSkillPassOn[i][nTableColumn](ptGame, hUnit, eSkillId, nLevel);
}

void __fastcall SkillsUse(GAME *ptGame, UNIT *hUnit)
{
	DWORD hSkill;
	int nSkillId;
	int nLevel;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	if (!hSkill)
		return;
	nSkillId = (int)D2CommonSkillsGetNodeType(hSkill);
	nLevel = (int)D2CommonSkillsCalcNodeLevel(hUnit, hSkill, 1);
	SkillsDispatch(ptGame, hUnit, nSkillId, nLevel, 1);
	if (!((BYTE)D2CommonSkillsGetNodeFlags(hSkill) & 64))
		D2CommonSkillsResetNode(hSkill);
}

void __fastcall SkillsEventPeriodic(GAME *ptGame, UNIT *hUnit)
{
	DWORD hSkill;
	int nSkillId;

	if (StatsGetState(hUnit, STATE_THUNDERSTORM))
	{
		SkillsDispatch(ptGame, hUnit, SKILL_THUNDER_STORM, (int)D2CommonSkillsCalcNodeLevel(hUnit,
			D2CommonSkillsFindByKey(hUnit, SKILL_THUNDER_STORM), 1), 1);
		return;
	}
	hSkill = D2CommonUnitGetRightSkill(hUnit);
#line 1265
	D2ASSERT(hSkill);
	nSkillId = (int)D2CommonSkillsGetNodeType(hSkill);
	if (((SKILLSTXT *)D2CommonDataTblsGetSkill(nSkillId))->Periodic && !SUnitIsDead(hUnit))
	{
		EventAdd(ptGame, hUnit, EVENT_PERIODIC, ptGame->nFrame + 75);
		SkillsDispatch(ptGame, hUnit, nSkillId, (int)D2CommonSkillsCalcNodeLevel(hUnit, hSkill, 1), 1);
		return;
	}
	EventRemoveByType(ptGame, hUnit, EVENT_PERIODIC);
}

void __fastcall SkillsEventAura(GAME *ptGame, UNIT *hUnit)
{
	int nLevel;
	int nSkillId;

	nLevel = (int)D2CommonStatsGetTotalValue(hUnit, STAT_ITEMAURA_DEFIANCE);
	if (nLevel)
		nSkillId = SKILL_DEFIANCE;
	else
	{
		nLevel = (int)D2CommonStatsGetTotalValue(hUnit, STAT_ITEMAURA_BLESSEDAIM);
		if (!nLevel)
			return;
		nSkillId = SKILL_BLESSED_AIM;
	}
	if (hUnit->dwMode != 0 && hUnit->dwMode != 17)
	{
		EventAdd(ptGame, hUnit, EVENT_AURA, ptGame->nFrame + 199);
		SkillsDispatch(ptGame, hUnit, nSkillId, nLevel, 1);
		return;
	}
	EventRemoveByType(ptGame, hUnit, EVENT_AURA);
}

BOOL __fastcall SkillsAdd(UNIT *hUnit, int nSkill)
{
	DWORD hSkill;
	DWORD dwState;
	DWORD nLevel;

	if (D2CommonStatsUnitGetBaseValue(hUnit, STAT_NEWSKILLS))
	{
		D2CommonStatsUnitAddValue(hUnit, STAT_NEWSKILLS, -1);
		hSkill = D2CommonSkillsAddSkill(hUnit, nSkill);
		if (hUnit->eType == UNIT_PLAYER)
			PlayerPetsUpdateMax(hUnit);
		nLevel = D2CommonSkillsCalcNodeLevel(hUnit, hSkill, 0);
		SCmdSendSkill(hUnit, (WORD)nSkill, (BYTE)nLevel, 0);
		dwState = D2CommonSkillsLookupKeyAFromKeyB(nSkill);
		if (dwState)
			D2CommonStatsUnitSetActiveStateBit(hUnit, dwState, 1);
		D2CommonSkillsUpdatePassive(hUnit, nSkill);
		return TRUE;
	}
	return FALSE;
}

extern void __fastcall SkillsSelect(UNIT *hUnit, int nSkill);
__inline void __fastcall SkillsSelect(UNIT *hUnit, int nSkill)
{
	DWORD hOldSkill;
	DWORD hSkill;
	DWORD dwState;

	hOldSkill = D2CommonOrdinal10854((DWORD)hUnit);
	if (D2CommonStatsUnitGetBaseValue(hUnit, 12) < (int)D2CommonOrdinal10881((DWORD)nSkill))
		return;
	hSkill = D2CommonSkillsFindByKey(hUnit, nSkill);
	if (!hSkill || hSkill == hOldSkill)
		return;
	D2CommonOrdinal10853((DWORD)hUnit, hSkill);
	if (!hOldSkill)
		return;
	dwState = D2CommonSkillsLookupKeyAFromKeyB(D2CommonSkillsGetNodeType(hOldSkill));
	if (dwState && !D2CommonStatsTestStatePassesOn(dwState))
		D2CommonStatsUnitSetActiveStateBit(hUnit, dwState, 0);
}

void __fastcall SkillsSetCurrent(UNIT *hUnit, BOOL fLeft, int nSkill)
{
	GAME *ptGame;
	SKILLSTXT *ptSkillsTxt;
	DWORD hOldSkill;
	DWORD hState;
	int nOldSkill;
	int i;

	if (nSkill == SKILL_LEFT_HAND_SWING)
		return;
	if (!fLeft)
		D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_55, 0);
	if (!D2CommonSkillsFindByKey(hUnit, nSkill))
		return;
	hOldSkill = 0;
	if (fLeft)
		D2CommonSkillsCacheLeftSkillRaw((DWORD)hUnit, (DWORD)nSkill);
	else
	{
		hOldSkill = D2CommonUnitGetRightSkill(hUnit);
		D2CommonSkillsCacheRightSkillRaw((DWORD)hUnit, (DWORD)nSkill);
	}
	if (hUnit->eType == UNIT_PLAYER)
		SCmdSendCurrentSkill(SUnitGetClient(hUnit), (BYTE)hUnit->eType, hUnit->dwId, (BYTE)fLeft,
			(WORD)nSkill);
	if (fLeft)
		return;
	if (hOldSkill)
	{
		nOldSkill = (int)D2CommonSkillsGetNodeType(hOldSkill);
		for (i = 0; i < sgnSkillStates; i++)
		{
			if (sgtSkillStates[i].nSkill != nOldSkill)
				continue;
			hState = D2CommonStatsFindListByState(hUnit, sgtSkillStates[i].nState);
			if (hState)
			{
				D2CommonStatsUnitUnlinkList(hUnit, hState);
				D2CommonStatsListFree(hState);
			}
			else
				D2CommonStatsUnitSetActiveStateBit(hUnit, sgtSkillStates[i].nState, 0);
		}
	}
	ptSkillsTxt = (SKILLSTXT *)D2CommonDataTblsGetSkill(nSkill);
	if (!ptSkillsTxt->Periodic)
		return;
	ptGame = SUnitGetGame(hUnit);
	EventRemoveByType(ptGame, hUnit, EVENT_PERIODIC);
	EventAdd(ptGame, hUnit, EVENT_PERIODIC, ptGame->nFrame + 75);
	for (i = 0; i < sgnSkillStates; i++)
	{
		if (sgtSkillStates[i].nSkill != nSkill)
			continue;
			if (D2CommonStatsTestStatePassesOn(sgtSkillStates[i].nState)) {

		if (sgtSkillStates[i].pfnPassOn)
		{
#line 1394
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgtSkillStates[i].pfnPassOn), "sgtSkillStates[i].pfnPassOn");
			sgtSkillStates[i].pfnPassOn(ptGame, hUnit, hUnit, (int)D2CommonSkillsCalcNodeLevel(hUnit,
				D2CommonSkillsFindByKey(hUnit, nSkill), 1), 0);
		}
		else
			SkillsDispatch(ptGame, hUnit, nSkill, (int)D2CommonSkillsCalcNodeLevel(hUnit,
				D2CommonSkillsFindByKey(hUnit, nSkill), 1), 1);
		} else D2CommonStatsUnitSetActiveStateBit(hUnit, sgtSkillStates[i].nState, 1); }
}

UNIT * __fastcall SkillsOpenPortalEnd(GAME *ptGame, UNIT *hPortal, int nLevelId, int nLevelFrom)
{
	GAMECOORD tCoord;
	GAMEROOM *hStart;
	GAMEROOM *hRoom;
	UNIT *hOther;
	int nX;
	int nY;
	int eLevelIdLocal;
	BYTE bAct;
	BYTE bFlags;

	bAct = D2CommonGetActNum(nLevelId);
	hRoom = (GAMEROOM *)D2CommonDungeonGetActStartRoom(ptGame->ptAct[bAct], nLevelId, 11, &nX,
		&nY, 3);
	hStart = hRoom;
	GameRespawnRoom(ptGame, hRoom);
	if (!hRoom)
	{
		D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hPortal), 2, hPortal->dwId);
		SUnitFree(ptGame, hPortal);
		return NULL;
	}
	tCoord.dwX = (DWORD)nX;
	tCoord.dwY = (DWORD)nY;
	if (QuestsPortalSpot(ptGame, &tCoord, nLevelFrom, &hRoom))
		hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(hRoom, tCoord.dwX, tCoord.dwY);
	if (!hRoom)
	{
		hRoom = hStart;
		tCoord.dwX = (DWORD)nX;
		tCoord.dwY = (DWORD)nY;
	}
	hRoom = (GAMEROOM *)D2CommonCollisnFindSpawnPointStep(hRoom, &tCoord, 3, 40465, 5);
	if (!hRoom)
	{
		hRoom = (GAMEROOM *)D2CommonDungeonGetActStartRoom(ptGame->ptAct[bAct], nLevelId, 11, &nX,
			&nY, 3);
		if (!hRoom)
			hRoom = hStart;
	}
	nX = (int)tCoord.dwX;
	nY = (int)tCoord.dwY;
	hOther = SUnitCreate(UNIT_OBJECT, hPortal->dwClassId, nX, nY, ptGame, hRoom, 1, 2, 0);
	if (!hOther)
	{
		D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hPortal), 2, hPortal->dwId);
		SUnitFree(ptGame, hPortal);
		return NULL;
	}
	eLevelIdLocal = (int)D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hPortal));
#line 1571
	D2ASSERT(eLevelIdLocal <= 255);
	((OBJECTDATA *)hOther->uTypeData)->field_04 = (BYTE)eLevelIdLocal;
	bFlags = (BYTE)(D2CommonUnitGetObjectFlagsRaw((DWORD)hOther) | 3);
	D2CommonUnitSetObjectFlags(hOther, bFlags);
	D2CommonUnitSetMode(hOther, 2);
	SUnitLinkPortals(hPortal, hOther);
	D2CommonUnitSetLinkedUnit(hPortal, hOther);
	D2CommonUnitSetLinkedUnit(hOther, hPortal);
	return hOther;
}

BOOL __fastcall SkillsOpenPortal(GAME *ptGame, UNIT *hOwner, GAMEROOM *hRoom, int nX, int nY, int nLevelId,
	UNIT **phPortal, int nClassId, BOOL fExact)
{
	COLPOINT tPoint;
	UNIT *hPortal;
	UNIT *hOther;
	int eLevelIdLocal;
	BYTE bAct;
	BYTE bFlags;

#line 1594
	D2ASSERT(hRoom);
	if (phPortal)
		*phPortal = NULL;
	if (hOwner && D2CommonDungeonRoomIsTown(hRoom)
		&& (nLevelId != LEVEL_27 || nClassId != OBJECT_PERMANENT_TOWN_PORTAL))
	{
		SUnitSetEvent(hOwner, EVENT_SKILLCHANGE, hOwner);
		return FALSE;
	}
	bAct = D2CommonGetActNum(nLevelId);
	eLevelIdLocal = (int)D2CommonDungeonRoomGetLevelId(hRoom);
#line 1613
	D2ASSERT(bAct == DrlgGetActNum ( eLevelIdLocal ));
	tPoint.nX = nX;
	tPoint.nY = nY;
	if (!fExact)
	{
		if (!D2CommonCollisnFindSpawnPointField(hRoom, &tPoint, &tPoint, 3, 7681, 3073, 0))
			return FALSE;
	}
	nX = tPoint.nX;
	nY = tPoint.nY;
	hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(hRoom, nX, nY);
	if (!hRoom)
		return FALSE;
	hPortal = SUnitCreate(UNIT_OBJECT, (DWORD)nClassId, nX, nY, ptGame, hRoom, 1, 1, 0);
	if (phPortal)
		*phPortal = hPortal;
	D2CommonUnitSetMode(hPortal, 1);
	hOther = SkillsOpenPortalEnd(ptGame, hPortal, nLevelId, eLevelIdLocal);
	if (!hOther)
		return FALSE;
	bFlags = (BYTE)(D2CommonUnitGetObjectFlagsRaw((DWORD)hOther) | 3);
	D2CommonUnitSetObjectFlags(hOther, bFlags);
	D2CommonDungeonRoomSetHold(hRoom, 0);
	D2CommonDungeonRoomSetHold(D2CommonUnitGetPathRoom(hOther), 0);
	return TRUE;
}

BOOL __fastcall SkillsGetTargetSpot(GAME *ptGame, UNIT *hUnit, int *pnX, int *pnY)
{
	UNIT *hTarget;
	void *ptPath;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget)
	{
		*pnX = (int)D2CommonUnitGetX(hTarget);
		*pnY = (int)D2CommonUnitGetY(hTarget);
	}
	else
	{
		ptPath = hUnit->ptPath;
		*pnX = (int)D2CommonPathGetTargetXRaw((DWORD)ptPath);
		*pnY = (int)D2CommonPathGetTargetYRaw((DWORD)ptPath);
	}
	if (*pnX && *pnY)
		return TRUE;
	return FALSE;
}

void __fastcall SkillsStepLengthen(int *pnX, int *pnY)
{
	int nSquare;

	nSquare = *pnY * *pnY + *pnX * *pnX;
	if (nSquare < 4)
	{
		*pnX = *pnX * 4;
		*pnY = *pnY << 2;
		nSquare = *pnY * *pnY + *pnX * *pnX;
	}
	if (nSquare < 16)
	{
		*pnX = *pnX * 2;
		*pnY = *pnY << 1;
	}
}

void __fastcall SkillsStepTurn(int *pnX, int *pnY)
{
	int nX;
	int nY;

	nY = *pnY;
	nX = -*pnX;
	while (nX * nX + nY * nY > 3)
	{
		nY = nY / 2;
		nX = nX / 2;
	}
	*pnX = nY;
	*pnY = nX;
}

void __fastcall SkillsNova(GAME *ptGame, UNIT *hUnit, UNIT *hCentre, int nMissile, int nLevel, int nVelocity)
{
	int anDX[NUM_NOVA_MISSILES] =
	{
		30, 29, 29, 28, 27, 26, 24, 23, 21, 19, 16, 14, 11, 8, 5, 2,
		0, -2, -5, -8, -11, -14, -16, -19, -21, -23, -24, -26, -27, -28, -29, -29,
		-30, -29, -29, -28, -27, -26, -24, -23, -21, -19, -16, -14, -11, -8, -5, -2,
		0, 2, 5, 8, 11, 14, 16, 19, 21, 23, 24, 26, 27, 28, 29, 29,
	};
	int anDY[NUM_NOVA_MISSILES] =
	{
		0, 2, 5, 8, 11, 14, 16, 19, 21, 23, 24, 26, 27, 28, 29, 29,
		30, 29, 29, 28, 27, 26, 24, 23, 21, 19, 16, 14, 11, 8, 5, 2,
		0, -2, -5, -8, -11, -14, -16, -19, -21, -23, -24, -26, -27, -28, -29, -29,
		-30, -29, -29, -28, -27, -26, -24, -23, -21, -19, -16, -14, -11, -8, -5, -2,
	};
	MISSILECREATE tCreate;
	int i;

	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = MISSFLAG_POSITION | MISSFLAG_TARGETREL;
	tCreate.nX = (int)D2CommonUnitGetX(hCentre);
	tCreate.nY = (int)D2CommonUnitGetY(hCentre);
	tCreate.hSource = hUnit;
	tCreate.eMissile = nMissile;
	tCreate.nLevel = nLevel;
	if (nVelocity)
	{
		tCreate.dwFlags |= MISSFLAG_VELOCITY;
		tCreate.nVelocity = nVelocity;
	}
	for (i = 0; i < NUM_NOVA_MISSILES; i++)
	{
		tCreate.nDX = anDX[i];
		tCreate.nDY = anDY[i];
		MissilesCreate(ptGame, &tCreate);
	}
}

typedef void (__fastcall *PFNSTATEDO)(GAME *ptGame, UNIT *hUnit);
struct ACTIVESTATEROW
{
 	int			nState;
 	PFNSTATEDO	pfnDo;
};
typedef struct ACTIVESTATEROW ACTIVESTATEROW;

static const ACTIVESTATEROW scptActiveStateChart[] =
{
	{ 0, NULL },
};

void __fastcall SkillsActiveStates(GAME *ptGame, UNIT *hUnit)
{
	int i;

	if (!D2CommonStatsUnitTestHasAction(hUnit))
		return;
	for (i = 0; scptActiveStateChart[i].nState; i++)
	{
		if (!StatsGetState(hUnit, scptActiveStateChart[i].nState))
			continue;
#line 1922
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)scptActiveStateChart[i].pfnDo), "scptActiveStateChart[i].pfnDo");
		scptActiveStateChart[i].pfnDo(ptGame, hUnit);
	}
}

static __inline void SkillsRemovePet(GAME *ptGame, UNIT *hUnit)
{
	UNIT *hOld;

#line 1936
	D2ASSERT(hUnit);
	hOld = SUnitFind(ptGame, UNIT_MONSTER, SUnitGetPetId(hUnit));
	if (hOld)
	{
		if (hOld->dwFlags & 256)
			SUnitFree(ptGame, hOld);
		else
		{
			hOld->dwFlags0 |= 67108864;
			SUnitDmgKill(ptGame, hOld, hUnit, TRUE);
		}
	}
	SUnitSetPetId(hUnit, (DWORD)-1);
}

UNIT * __fastcall SkillsSummon(GAME *ptGame, SKILLSUMMON *ptSummon)
{
	GAMEROOM *hRoom;
	UNIT *hOwner;
	DWORD hPathRoom;
	UNIT *hTarget;
	UNIT *hPet;
	void *ptPath;
	int nX;
	int nY;

	if (ptSummon->dwFlags & 1)
	{
		nX = ptSummon->nX;
		nY = ptSummon->nY;
	}
	else
	{
		hOwner = ptSummon->hOwner;
		hTarget = SUnitGetTarget(ptGame, hOwner);
		if (hTarget)
		{
			nX = (int)D2CommonUnitGetX(hTarget);
			nY = (int)D2CommonUnitGetY(hTarget);
		}
		else
		{
			ptPath = hOwner->ptPath;
			nX = (int)D2CommonPathGetTargetXRaw((DWORD)ptPath);
			nY = (int)D2CommonPathGetTargetYRaw((DWORD)ptPath);
		}
	}
	hPathRoom = D2CommonUnitGetPathRoom(ptSummon->hOwner);
	hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(hPathRoom, nX, nY);
	if (!hRoom)
		return NULL;
	hPet = MonsterSpawn(ptGame, hRoom, nX, nY, (int)ptSummon->dwClassId, ptSummon->dwMode, -1, 66);
	if (!hPet)
	{
		if (ptSummon->dwFlags & 4)
			goto lbFail;
		hPet = MonsterSpawn(ptGame, hRoom, nX, nY, (int)ptSummon->dwClassId, ptSummon->dwMode, 4, 66);
		if (!hPet)
		{
lbFail:
			return NULL;
		}
	}
	if (!(ptSummon->dwFlags & 8))
		hPet->dwFlags0 |= 0x80000000;
	if (ptSummon->dwFlags & 2)
	{
		SkillsRemovePet(ptGame, ptSummon->hOwner);
		SUnitSetPetId(ptSummon->hOwner, hPet->dwId);
	}
	if (ptSummon->dwPetType)
		PlayerPetsAdd(ptGame, ptSummon->hOwner, hPet, (int)ptSummon->dwPetType);
	hPet->dwFlags0 |= 131072;
	AiSetOwner(ptGame, hPet, ptSummon->hOwner->dwId, ptSummon->hOwner->eType, FALSE, FALSE);
#line 2036
	D2ASSERT(!StatsGetState (hPet, UNIT_STATE_UNINTERRUPTABLE));
	AiThinkSet(ptGame, hPet, ((MONSTERDATA *)hPet->uTypeData)->ptAi, (int)ptSummon->dwAiKind);
	MonsterRestartAi(ptGame, hPet);
	return hPet;
}
#line 2041

UNIT * __fastcall SkillsDropItem(GAME *ptGame, UNIT *hUnit, DWORD dwCode, DWORD dwQuality)
{
	ITEMCREATE tCreate;
	COLPOINT tCoord;
	COLPOINT tSpawn;
	GAMEROOM *hRoom;
	int nIndex;
	int i;

	nIndex = D2CommonItemTblsFindItemIndexByCode(dwCode);
	if (!(nIndex == -1)) {
		D2CommonUnitGetPosition(hUnit, &tCoord);
		hRoom = ITEMSFindDropSpot((GAMEROOM *)D2CommonUnitGetPathRoom(hUnit), &tCoord, &tSpawn, 1);
		if (!hRoom)
			return NULL;
		for (i = 0; i < (int)(sizeof(ITEMCREATE) / sizeof(DWORD)); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.hSpawner = hUnit;
		tCreate.ptSeed = NULL;
		tCreate.ptGame = ptGame;
		tCreate.dwClassId = (DWORD)nIndex;
		tCreate.dwMode = 3;
		tCreate.nX = tSpawn.nX;
		tCreate.nY = tSpawn.nY;
		tCreate.hRoom = hRoom;
		tCreate.dwQuality = dwQuality;
		tCreate.wCreateFlags = 1;
		tCreate.nLevel = ITEMSGetUnitLevel(hUnit, LEVEL_ID_NONE);
		return ITEMSCreateFromRecord(ptGame, &tCreate, FALSE); }
	return NULL;
}

void __fastcall SkillsCurseArea(GAME *ptGame, DWORD dwFlags, UNIT *hUnit, int nLevel, int nSkill,
	PFNSKILLCURSE pfnCurse)
{
	UNITFINDARGS tArgs;
	UNITFINDDATA tFind;
	GAMEROOM *hRoom;
	UNIT *hTarget;
	void *ptPath;
	int nX;
	int nY;
	int nRange;
	int i;

#line 2152
	D2ASSERT(pfnCurse);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget)
	{
		nX = (int)D2CommonUnitGetX(hTarget);
		nY = (int)D2CommonUnitGetY(hTarget);
	}
	else
	{
		ptPath = hUnit->ptPath;
		nX = (int)D2CommonPathGetTargetXRaw((DWORD)ptPath);
		nY = (int)D2CommonPathGetTargetYRaw((DWORD)ptPath);
	}
	if (!nX || !nY)
		return;
	nRange = D2CommonSkillsCalcParamOneByLevel(nSkill, nLevel);
	for (i = 0; i < (int)(sizeof(UNITFINDARGS) / sizeof(DWORD)); i++)
		((DWORD *)&tArgs)[i] = 0;
	tArgs.dwFlags = dwFlags;
	tArgs.hExcluded = hUnit;
	tArgs.nX = nX;
	tArgs.nY = nY;
	tArgs.nDistance = nRange;
	hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hUnit), nX, nY);
	if (!hRoom)
		return;
	D2CommonUnitFindsInit(ptGame->pMemPool, &tFind, hRoom, nX, nY,
		nRange, 0, &tArgs);
	D2CommonUnitFindsFindUnits(&tFind);
#line 2180
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)pfnCurse), "pfnCurse");
	for (i = 0; i < tFind.nNumUnits; i++)
		pfnCurse(ptGame, hUnit, tFind.phUnitsArray[i], nLevel);
	D2CommonUnitFindsFreeArray(&tFind);
}

void __fastcall SkillsAreaEffect(GAME *ptGame, SKILLEFFECT *ptEffect)
{
	UNITFINDARGS tArgs;
	UNITFINDDATA tFind;
	GAMEROOM *hRoom;
	int nRange;
	int i;

#line 2250
	D2ASSERT(ptEffect);
#line 2251
	D2ASSERT(ptEffect->pfnAreaAffect);
	nRange = D2CommonSkillsCalcParamOneByLevel(ptEffect->nSkill, ptEffect->nLevel);
	for (i = 0; i < (int)(sizeof(UNITFINDARGS) / sizeof(DWORD)); i++)
		((DWORD *)&tArgs)[i] = 0;
	tArgs.dwFlags = ptEffect->dwFlags;
	tArgs.hExcluded = ptEffect->hUnit;
	tArgs.nX = ptEffect->nX;
	tArgs.nY = ptEffect->nY;
	tArgs.nDistance = nRange;
	hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(ptEffect->hUnit),
		ptEffect->nX, ptEffect->nY);
	if (!hRoom)
		return;
	D2CommonUnitFindsInit(ptGame->pMemPool, &tFind, hRoom, ptEffect->nX,
		ptEffect->nY, nRange, 0, &tArgs);
	tFind.dwFlags = ptEffect->dwFlags;
	D2CommonUnitFindsFindUnits(&tFind);
#line 2280
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)ptEffect->pfnAreaAffect), "ptEffect->pfnAreaAffect");
	for (i = 0; i < tFind.nNumUnits; i++)
		ptEffect->pfnAreaAffect(ptGame, ptEffect->hUnit, tFind.phUnitsArray[i], ptEffect->nLevel,
			ptEffect->dwAffectArg);
	D2CommonUnitFindsFreeArray(&tFind);
}

void __fastcall SkillsAreaEffectAt(GAME *ptGame, DWORD dwFlags, int nX, int nY, UNIT *hUnit, int nSkill,
	int nLevel, PFNAREAAFFECT pfnAreaAffect)
{
	SKILLEFFECT tEffect;
	int i;

	for (i = 0; i < (int)(sizeof(SKILLEFFECT) / sizeof(DWORD)); i++)
		((DWORD *)&tEffect)[i] = 0;
	tEffect.nX = nX;
	tEffect.nY = nY;
	tEffect.dwFlags = dwFlags;
	tEffect.hUnit = hUnit;
	tEffect.nSkill = nSkill;
	tEffect.nLevel = nLevel;
	tEffect.pfnAreaAffect = pfnAreaAffect;
	SkillsAreaEffect(ptGame, &tEffect);
}

void __fastcall SkillsSetLevel(UNIT *hUnit, int nSkill, int nLevel, DWORD dwArg)
{
	if (hUnit->eType != UNIT_MONSTER)
	{
#line 2341
		D2ASSERTMSG(nLevel <= MAX_SKILL_LEVEL, "Attempt to set skill level beyond max level!\nPlease tell Ted Bisson!");
	}
	D2CommonSkillsSetSkillLevel(hUnit, nSkill, nLevel, dwArg);
	if (hUnit->eType == UNIT_PLAYER)
		PlayerPetsUpdateMax(hUnit);
}

extern void __fastcall SkillsUnlearn(UNIT *hUnit, int nSkill);
__inline void __fastcall SkillsUnlearn(UNIT *hUnit, int nSkill)
{
	D2CommonOrdinal10850((DWORD)hUnit, (DWORD)nSkill);
	D2CommonSkillsUpdatePassive(hUnit, nSkill);
	if (hUnit->eType == UNIT_PLAYER)
		PlayerPetsUpdateMax(hUnit);
}

void __fastcall SkillsRestorePassives(UNIT *hUnit)
{
	void *hSkillInfo;
	DWORD hSkill;
	int nSkillId;
	DWORD dwState;

	hSkillInfo = hUnit->field_c8;
#line 2379
	D2WARN(hSkillInfo);
	for (hSkill = D2CommonSkillsHeadGetFirst(hSkillInfo); hSkill; hSkill = D2CommonSkillsNodeGetNext(hSkill))
	{
		nSkillId = (int)D2CommonSkillsGetNodeType(hSkill);
		dwState = D2CommonSkillsLookupKeyAFromKeyB(nSkillId);
		D2CommonStatsUnitSetActiveStateBit(hUnit, dwState, 1);
		D2CommonSkillsUpdatePassive(hUnit, nSkillId);
	}
}

void __fastcall SkillsGetDamage(UNIT *hUnit, int eMode, int *pnDamage)
{
	MONSTATSTXT *ptMonStats;
	SKILLSTXT *ptSkill;
	UNIT *hOwner;
	int nLevel;
	int nValue;

	switch (hUnit->dwClassId)
	{
	case MONSTER_FIREGOLEM:
	case MONSTER_NECROSKELETON:
		break;
	default:
		return;
	}
	hOwner = AiGetOwner(hUnit);
	if (!hOwner)
		return;
	switch (hUnit->dwClassId)
	{
	case MONSTER_FIREGOLEM:
		nLevel = (int)D2CommonSkillsSumNodeBonus(hOwner, SKILL_FIREGOLEM);
		ptMonStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId);
		nValue = D2CommonSkillsCalcParamThreeByLevel(SKILL_FIREGOLEM, nLevel);
		pnDamage[0] = ptMonStats->pnAttack1MinDamage[0] * nValue / 100;
		pnDamage[1] = ptMonStats->pnAttack1MaxDamage[0] * nValue / 100;
		break;
	case MONSTER_NECROSKELETON:
		nLevel = (int)D2CommonSkillsSumNodeBonus(hOwner, SKILL_SKELETON_MASTERY);
		if (!nLevel)
			break;
		ptSkill = (SKILLSTXT *)D2CommonDataTblsGetSkill(SKILL_SKELETON_MASTERY);
		pnDamage[0] = ptSkill->Param[1] * nLevel;
		pnDamage[1] = ptSkill->Param[1] * nLevel;
		break;
	}
}

void __fastcall SkillsElemDamage(UNIT *hUnit, DAMAGE *ptDamage, int nSkill, int nLevel)
{
	SKILLSTXT *ptSkill;
	int nLength;
	int nBonus;
	int nMin;
	int nMax;
	int nDamage;

	ptSkill = (SKILLSTXT *)D2CommonDataTblsGetSkill(nSkill);
	nLevel--;
	nLength = ptSkill->ELevLen * nLevel + ptSkill->ELen;
	nBonus = ptSkill->ELev * nLevel;
	nMin = (ptSkill->EMin + nBonus) << ptSkill->HitShift;
	nMax = (ptSkill->EMax + nBonus) << ptSkill->HitShift;
	if (nMax - nMin)
		nDamage = (int)D2CommonSeedRand(&hUnit->tSeed, nMax - nMin);
	else
		nDamage = 0;
	nDamage += nMin;
	switch (ptSkill->EType)
	{
	case 1:
		ptDamage->nFireDamage = nDamage;
		break;
	case 2:
		ptDamage->nLightningDamage = nDamage;
		break;
	case 4:
		ptDamage->nColdDamage = nDamage;
		ptDamage->nColdLength = nLength;
		break;
	case 3:
		ptDamage->nMagicDamage = nDamage;
		break;
	case 5:
		ptDamage->nPoisonDamage = nDamage;
		ptDamage->nPoisonLength = nLength;
		break;
	}
}

void __fastcall SkillsFindTargets(GAME *ptGame, SKILLFINDTARGET *ptTarget, UNITFINDDATA *ptFind)
{
	UNITFINDARGS tArgs;
	GAMEROOM *hRoom;
	DWORD dwFlags;
	int i;

#line 2558
	D2ASSERT(ptTarget);
	for (i = 0; i < (int)(sizeof(UNITFINDDATA) / sizeof(DWORD)); i++)
		((DWORD *)ptFind)[i] = 0;
	dwFlags = 8832;
	if (ptTarget->dwFlags & 1)
		dwFlags |= 1;
	if (ptTarget->dwFlags & 2)
		dwFlags |= 2;
	for (i = 0; i < (int)(sizeof(UNITFINDARGS) / sizeof(DWORD)); i++)
		((DWORD *)&tArgs)[i] = 0;
	tArgs.hExcluded = ptTarget->hUnit;
	tArgs.dwFlags = dwFlags;
	tArgs.nX = ptTarget->nX;
	tArgs.nY = ptTarget->nY;
	tArgs.nDistance = ptTarget->nRange;
	tArgs.pFieldPair = (void *)D2CommonFieldTblsAllocPair();
	D2CommonFieldTblsSetPair(tArgs.pFieldPair, ptTarget->nX, ptTarget->nY);
	hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(ptTarget->hUnit),
		ptTarget->nX, ptTarget->nY);
	if (!hRoom)
	{
		D2CommonFieldTblsFreePair(tArgs.pFieldPair);
		ptFind->nNumUnits = 0;
		return;
	}
	D2CommonUnitFindsInit(ptGame->pMemPool, ptFind, hRoom, ptTarget->nX,
		ptTarget->nY, ptTarget->nRange, 0, &tArgs);
	ptFind->dwFlags = dwFlags;
	D2CommonUnitFindsFindUnits(ptFind);
	D2CommonFieldTblsFreePair(tArgs.pFieldPair);
}

extern int SkillsCountFound(GAME *ptGame, SKILLFINDTARGET *ptTarget);
__inline int SkillsCountFound(GAME *ptGame, SKILLFINDTARGET *ptTarget)
{
	UNITFINDDATA tFind;
	int nCount;

	D2_OPAQUE(ptTarget);
#line 2608
	D2ASSERT(ptTarget);
	SkillsFindTargets(ptGame, ptTarget, &tFind);
	nCount = tFind.nNumUnits;
	D2CommonUnitFindsFreeArray(&tFind);
	return nCount;
}

UNIT * __fastcall SkillsFindTarget(GAME *ptGame, SKILLFINDTARGET *ptTarget)
{
	UNITFINDDATA tFind;
	UNIT *hNext;
	UNIT *hFirst;
	UNIT *hOther;
	DWORD dwNextId;
	DWORD dwFirstId;
	DWORD dwId;
	int nX;
	int nY;
	int i;

	hNext = NULL;
	hFirst = NULL;
#line 2654
	D2ASSERT(ptTarget);
	SkillsFindTargets(ptGame, ptTarget, &tFind);
	dwFirstId = ptTarget->dwUnitId;
	dwNextId = (DWORD)-1;
	for (i = 0; i < tFind.nNumUnits; i++)
	{
		hOther = tFind.phUnitsArray[i];
		dwId = hOther->dwId;
		if (!SUnitIsHostile(ptGame, ptTarget->hUnit, hOther))
			continue;
		if ((ptTarget->dwFlags & 4) && !D2CommonUnitTestPositionClear(ptTarget->hUnit, hOther, 0))
			continue;
		if (ptTarget->dwFlags & 8)
		{

			if (!D2CommonSkillsAddFieldB(((int)D2CommonUnitGetX(ptTarget->hUnit)), ((int)D2CommonUnitGetY(ptTarget->hUnit)), hOther, ptTarget->dwCollisionMask))
				continue;
		}
		if (dwId > ptTarget->dwUnitId && dwId < dwNextId)
		{
			dwNextId = dwId;
			hNext = tFind.phUnitsArray[i];
		}
		if (dwId <= dwFirstId)
		{
			dwFirstId = dwId;
			hFirst = tFind.phUnitsArray[i];
		}
	}
	D2CommonUnitFindsFreeArray(&tFind);
	if (dwNextId == ptTarget->dwUnitId || dwNextId == (DWORD)-1)
		hNext = hFirst;
	return hNext;
}

void __fastcall SkillsLeapHit(GAME *ptGame, UNIT *hUnit, UNIT *hTarget)
{
	MONMODECHANGE tReq;
	DWORD hState;
	int nEnd;

	if (StatsGetState(hTarget, UNIT_STATE_UNINTERRUPTABLE))
		return;
	if (hTarget->eType != UNIT_MONSTER)
		return;
	if (MonsterUniqueIsCounted(((MONSTERDATA *)hTarget->uTypeData)->ptUnique))
		return;
	switch (hTarget->dwClassId)
	{
	case MONSTER_ANDARIEL:
	case MONSTER_DURIEL:
	case MONSTER_MEPHISTO:
	case MONSTER_DIABLO:
		return;
	}
	hState = D2CommonStatsFindListByState(hTarget, STATE_STUNNED);
	if (!hState)
	{
		nEnd = ptGame->nFrame + 100;
		hState = D2CommonStatsAllocList(ptGame->pMemPool, 2, nEnd, hTarget->eType,
			hTarget->dwId);
		if (hState)
		{
			EventAdd(ptGame, hTarget, EVENT_STATEEND, nEnd);
			D2CommonStatsSetListState(hState, STATE_STUNNED);
			D2CommonStatsSetListRemoveCallback(hState, SkillsStateRemove);
			D2CommonStatsUnitLinkList(hTarget, hState);
			D2CommonStatsUnitSetActiveStateBit(hTarget, STATE_STUNNED, 1);
		}
	}
	else
	{
		nEnd = ptGame->nFrame + 100;
		D2CommonStatsSetListEndFrame(hState, nEnd);
		EventAdd(ptGame, hTarget, EVENT_STATEEND, nEnd);
	}
	MonsterModeInit(hTarget, 1, &tReq);
	MonsterModeSet(ptGame, &tReq, TRUE);
	hTarget->dwFlags0 &= ~1;
}

UNIT * __fastcall SkillsFindNext(GAME *ptGame, UNIT *hUnit, DWORD dwUnitId, int nRange, DWORD dwFlags)
{
	SKILLFINDTARGET tTarget;
	int i;

	for (i = 0; i < (int)(sizeof(SKILLFINDTARGET) / sizeof(DWORD)); i++)
		((DWORD *)&tTarget)[i] = 0;
	tTarget.dwFlags = dwFlags | 3;
	tTarget.hUnit = hUnit;
	tTarget.dwUnitId = dwUnitId;
	tTarget.nX = (int)D2CommonUnitGetX(hUnit);
	tTarget.nY = (int)D2CommonUnitGetY(hUnit);
	tTarget.nRange = nRange;
	if ((BYTE)dwFlags & 8)
		tTarget.dwCollisionMask = 2053;
	return SkillsFindTarget(ptGame, &tTarget);
}

int __fastcall SkillsCountTargets(GAME *ptGame, UNIT *hUnit, int nRange)
{
	SKILLFINDTARGET tTarget;
	int i;

	for (i = 0; i < (int)(sizeof(SKILLFINDTARGET) / sizeof(DWORD)); i++)
		((DWORD *)&tTarget)[i] = 0;
	tTarget.dwFlags = 3;
	tTarget.hUnit = hUnit;
	tTarget.nX = (int)D2CommonUnitGetX(hUnit);
	tTarget.nY = (int)D2CommonUnitGetY(hUnit);
	tTarget.nRange = nRange;
	return SkillsCountFound(ptGame, &tTarget);
}

void __fastcall SkillsSlowAnim(UNIT *hUnit, int nPercent)
{
	hUnit->nAnimPos = (100 - nPercent) * (hUnit->nAnimPos >> 8) / 100 << 8;
}

BOOL __fastcall SkillsWhirlAt(GAME *ptGame, UNIT *hPlayer, UNIT *hTarget)
{
	DWORD hSkill;

#line 2820
	D2ASSERT(hPlayer);
#line 2821
	D2ASSERT(hTarget);
#line 2822
	D2ASSERT(ptGame);
	SUnitSetMode(ptGame, hPlayer, 1);
	D2CommonUnitSetSkillThirdCached(hPlayer, 0);
	hSkill = D2CommonSkillsFindByKey(hPlayer, 0);
	PlrModeStartID(ptGame, hPlayer, (void *)hSkill, 7, hTarget->eType, hTarget->dwId, TRUE);
	return TRUE;
}

BOOL __fastcall SkillsCanBeAffected(UNIT *hUnit, int nKind)
{
	if (MonsterCanBeAffected(hUnit, nKind))
		return TRUE;
	else
		return FALSE;
}

extern int __fastcall SkillsGetManaPoints(UNIT *hUnit, int nSkill);
__inline int __fastcall SkillsGetManaPoints(UNIT *hUnit, int nSkill)
{
	DWORD hSkill;

	hSkill = D2CommonSkillsFindByKey(hUnit, nSkill);
	if (!hSkill)
		return 0;
	return (int)D2CommonOrdinal10919((DWORD)nSkill, D2CommonSkillsCalcNodeLevel(hUnit, hSkill, 1));
}

extern void * __fastcall SkillsGetString(WORD wStringId);
__inline void * __fastcall SkillsGetString(WORD wStringId)
{
	return D2LangStrGetStringByIndex(wStringId);
}
