#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\SKILLS\\SkillItem.cpp"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "../GAME/SCmd.h"
#include "../GUILDS/Guilds.h"
#include "../ITEMS/ItemMode.h"
#include "../MONSTER/MonsterUnique.h"
#include "../PLAYER/Player.h"
#include "../PLAYER/PlayerPets.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "SkillItem.h"
#include "SkillNec.h"
#include "Skills.h"

#define NUM_SPELLFUNCOFFSETS	23
#define SKILL_ITEM_START		217
#define NUM_ITEM_SKILLS			4
#define UNIT_OBJECT				2
#define UNIT_ITEM				4
#define ITEMTYPE_TOME			18
#define ITEMTYPE_SCROLL			22
#define ITEMFLAG_INUSE			4
#define STAT_VITALITY			3
#define STAT_ENERGY				1
#define STAT_HITPOINTS			6
#define STAT_MANA				8
#define STAT_STAMINA			10
#define STAT_LEVEL				12
#define STAT_MANARECOVERY		26
#define STAT_QUANTITY			70
#define STAT_VALUE				71
#define STAT_HPREGEN			74
#define STAT_ATTACKERTAKESDAMAGE	78
#define STAT_DAMAGETOMANA		114
#define STAT_ATTACKERTAKESLIGHTDAMAGE	128
#define STAT_OPENWOUNDS			135
#define STAT_CRUSHINGBLOW		136
#define STAT_KICKDAMAGE			137
#define STAT_MANAAFTERKILL		138
#define STAT_HEALAFTERDEMONKILL	139
#define STAT_SLOW				150
#define STAT_REANIMATE			155
#define STATLIST_ITEMS			4096
#define PET_SKELETON			4

void __fastcall SCmdSendSkill(UNIT *hUnit, WORD wSkill, BYTE bSkillLevel, BYTE bRemove);
void __fastcall SCmdSendTradeEvent(CLIENT *ptClient,
	BYTE bValue);
void __fastcall ObjectEvalSetLinkValue(UNIT *hObject, int nValue);

struct ITEMSPELL
{
 	DWORD			dwUseInit;
 	PFNITEMSPELL	pfnSpellInit;
 	PFNITEMSPELL	pfnSpell;
 	DWORD			dwParam;
};
typedef struct ITEMSPELL ITEMSPELL;

struct PASSIVEMOD
{
 	int				nStat;
 	int				nOwner;
 	int				nEvent;
 	PFNPASSIVEMOD	pFunc;
 	int				nParam;
 	DWORD			fFlag;
};
typedef struct PASSIVEMOD PASSIVEMOD;

struct ITEMSKILL
{
 	int				nSkillId;
 	PFNITEMSKILL	pfnSkill;
};
typedef struct ITEMSKILL ITEMSKILL;

static ITEMSPELL sgtItemSpellTbl[] =
{
	{ 1, SkillsItemOnItemStart, SkillsItemOnItem, 0 },
	{ 0, NULL, SkillsItemAddStat, 0 },
	{ 0, NULL, SkillsItemNoSpell, 0 },
	{ 0, NULL, SkillsItemPortal, 0 },
	{ 0, NULL, SkillsItemStamina, 0 },
	{ 0, NULL, SkillsItemStamina, 0 },
	{ 0, NULL, SkillsItemCureSingleState, 0 },
	{ 0, NULL, SkillsItemCureSingleState, 0 },
	{ 0, NULL, SkillsItemRestoreLifeMana, 0 },
	{ 0, NULL, SkillsItemRestoreFull, 0 },
	{ 0, NULL, SkillsItemCureStates, 0 },
	{ 0, NULL, SkillsItemCureStates, 0 },
	{ 0, NULL, SkillsItemInteract, 0 },
	{ 0, NULL, SkillsItemHealPotion, 0 },
	{ 0, NULL, SkillsItemHealPotion, 0 },
	{ 0, NULL, SkillsItemHealPotion, 0 },
	{ 0, NULL, SkillsItemHealPotion, 0 },
	{ 0, NULL, SkillsItemHealPotion, 0 },
	{ 0, NULL, SkillsItemManaPotion, 0 },
	{ 0, NULL, SkillsItemManaPotion, 0 },
	{ 0, NULL, SkillsItemManaPotion, 0 },
	{ 0, NULL, SkillsItemManaPotion, 0 },
	{ 0, NULL, SkillsItemManaPotion, 0 },
};
static DWORD sgnItemSpellTblSize = sizeof(sgtItemSpellTbl) / sizeof(sgtItemSpellTbl[0]);
static const char *sgpszClassAll = "all";

static PASSIVEMOD sgtApplyPassiveModTbl[] =
{
	{ STAT_ATTACKERTAKESDAMAGE, 2, 3, SkillsItemAttackerTakesDamage, 5, 1 },
	{ 81, 1, 1, SkillsItemKnockback, 5, 0 },
	{ 83, 2, 4, SkillsItemAddClassSkills, 0, 0 },
	{ 84, 2, 4, SkillsItemAddClassSkills, 3, 0 },
	{ 85, 2, 4, SkillsItemAddClassSkills, 2, 0 },
	{ 86, 2, 4, SkillsItemAddClassSkills, 1, 0 },
	{ 87, 2, 4, SkillsItemAddClassSkills, 4, 0 },
	{ 107, 2, 4, SkillsItemAddSingleSkill, 5, 0 },
	{ 108, 2, 4, SkillsItemAddSingleSkill, 5, 0 },
	{ 109, 2, 4, SkillsItemAddSingleSkill, 5, 0 },
	{ 112, 1, 3, SkillsItemHowl, 5, 1 },
	{ 113, 1, 3, SkillsItemStupidity, 5, 1 },
	{ STAT_DAMAGETOMANA, 2, 2, SkillsItemDamageToMana, 5, 0 },
	{ 126, 2, 4, SkillsItemAddFireSkills, 5, 0 },
	{ STAT_ATTACKERTAKESLIGHTDAMAGE, 2, 3, SkillsItemAttackerTakesLightDmg, 5, 1 },
	{ 134, 1, 3, SkillsItemFreeze, 5, 1 },
	{ STAT_OPENWOUNDS, 1, 1, SkillsItemOpenWounds, 5, 1 },
	{ STAT_CRUSHINGBLOW, 1, 3, SkillsItemCrushingBlow, 5, 1 },
	{ STAT_KICKDAMAGE, 1, 2, SkillsItemKickDamage, 5, 1 },
	{ STAT_MANAAFTERKILL, 1, 5, SkillsItemManaAfterKill, 5, 0 },
	{ STAT_HEALAFTERDEMONKILL, 1, 5, SkillsItemHealAfterDemonKill, 5, 0 },
	{ STAT_SLOW, 1, 3, SkillsItemSlow, 5, 1 },
	{ 151, 1, 4, SkillsItemFlagOne, 5, 0 },
	{ 152, 1, 4, SkillsItemFlagOne, 5, 0 },
	{ STAT_REANIMATE, 1, 5, SkillsItemReanimate, 5, 0 },
};
static int sgnApplyPassiveModTblSize = sizeof(sgtApplyPassiveModTbl) / sizeof(sgtApplyPassiveModTbl[0]);

static PASSIVEMOD sgtRemovePassiveModTbl[] =
{
	{ 83, 2, 4, SkillsItemRemoveClassSkills, 0, 0 },
	{ 84, 2, 4, SkillsItemRemoveClassSkills, 3, 0 },
	{ 85, 2, 4, SkillsItemRemoveClassSkills, 2, 0 },
	{ 86, 2, 4, SkillsItemRemoveClassSkills, 1, 0 },
	{ 87, 2, 4, SkillsItemRemoveClassSkills, 4, 0 },
	{ 107, 2, 4, SkillsItemRemoveSingleSkill, 5, 0 },
	{ 108, 2, 4, SkillsItemRemoveSingleSkill, 5, 0 },
	{ 109, 2, 4, SkillsItemRemoveSingleSkill, 5, 0 },
	{ 126, 2, 4, SkillsItemRemoveFireSkills, 5, 0 },
};
static DWORD sgnRemovePassiveModTblSize =
	sizeof(sgtRemovePassiveModTbl) / sizeof(sgtRemovePassiveModTbl[0]);

static ITEMSKILL sgtItemSkillTbl[] =
{
	{ 217, SkillsItemUseBook },
	{ 218, SkillsItemUseBook },
	{ 219, SkillsItemUseBook },
	{ 220, SkillsItemUseBook },
};
static DWORD sgnItemSkillTblSize = sizeof(sgtItemSkillTbl) / sizeof(sgtItemSkillTbl[0]);

BOOL __fastcall SkillsItemOnItemStart(GAME *ptGame, UNIT *hUnit, UNIT *hItem,
	UNIT *hTarget, DWORD dwX, DWORD dwY,
	int nSkill, DWORD dwParam)
{
	BYTE bIcon;

	D2CommonITEMSDataSetFlags(hItem, ITEMFLAG_INUSE, TRUE);

	SCmdSendSpellIcon(SUnitGetClient(hUnit), hItem, D2CommonITEMSGetSpellIcon(hItem), 0, 1, (WORD)nSkill);
	return TRUE;
}

BOOL __fastcall SkillsItemRestoreLifeMana(GAME *ptGame, UNIT *hUnit, UNIT *hItem,
	UNIT *hTarget, DWORD dwX, DWORD dwY,
	int nSkill, DWORD dwParam)
{
	int nMax;
	int nDelta;
	int nAdd;

	nMax = D2CommonStatsUnitCalcMaxHp(hUnit);
	nDelta = nMax * 35 / 100;
#line 247
	D2ASSERT(nDelta);
	nAdd = SkillsBloodGolemHeal(ptGame, hUnit, nDelta);
	if (D2CommonStatsUnitGetValue(hUnit, STAT_HITPOINTS) + nAdd > nMax)
		nAdd = nMax - D2CommonStatsUnitGetValue(hUnit, STAT_HITPOINTS);
	D2CommonStatsUnitAddValue(hUnit, STAT_HITPOINTS, nAdd);
	nMax = D2CommonStatsUnitCalcMaxMana(hUnit);
	nAdd = nMax * 35 / 100;
	if (D2CommonStatsUnitGetValue(hUnit, STAT_MANA) + nAdd > nMax)
		nAdd = nMax - D2CommonStatsUnitGetValue(hUnit, STAT_MANA);
	D2CommonStatsUnitAddValue(hUnit, STAT_MANA, nAdd);
	return TRUE;
}

BOOL __fastcall SkillsItemRestoreFull(GAME *ptGame, UNIT *hUnit, UNIT *hItem,
	UNIT *hTarget, DWORD dwX, DWORD dwY,
	int nSkill, DWORD dwParam)
{
	int nLife;
	int nMax;
	int nMana;

	nLife = D2CommonStatsUnitGetValue(hUnit, STAT_HITPOINTS);
	nMax = D2CommonStatsUnitCalcMaxHp(hUnit) - nLife;
	SkillsBloodGolemHeal(ptGame, hUnit, 0);
	D2CommonStatsUnitAddValue(hUnit, STAT_HITPOINTS, nMax);
	nMana = D2CommonStatsUnitGetValue(hUnit, STAT_MANA);
	D2CommonStatsUnitAddValue(hUnit, STAT_MANA, D2CommonStatsUnitCalcMaxMana(hUnit) - nMana);
	return TRUE;
}

BOOL __fastcall SkillsItemOnItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem,
	UNIT *hTarget, DWORD dwX, DWORD dwY, int nSkill,
	DWORD dwParam)
{
	void *hInventory;
	DWORD hNode;

	hInventory = hUnit->ptInventory;
#line 125
	D2ASSERT(hInventory);
	for (hNode = (DWORD)D2CommonINVENTORYGetCategoryListHead(
		hInventory); hNode; hNode = D2CommonINVENTORYNodeGetNext(hNode))
	{
		if ((UNIT *)D2CommonINVENTORYNodeGetKeyA(hNode) == hTarget)
		{
			if (D2CommonITEMSDataTestFlags(hTarget, 16))
				return FALSE;
			ITEMSModeMarkHandItem(hUnit, hTarget);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL __fastcall SkillsItemInteract(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget,
	DWORD dwX, DWORD dwY, int nSkill,
	DWORD dwParam)
{
	UNIT *hOther;

	if (PlayerIsBusy(hUnit))
	{
		hOther = SUnitGetInteractUnit(ptGame, hUnit);
		if (!hOther || hOther->eType != UNIT_OBJECT || hOther->dwClassId != OBJECT_BANK)
			return FALSE;
		SUnitResetInteract(hUnit);
		ITEMSModeResetBookSkills(ptGame, hUnit);
		SCmdSendTradeEvent(SUnitGetClient(hUnit), 15);
	}
	SUnitSetInteract(hUnit, UNIT_ITEM, hItem->dwId);
	SCmdSendTradeEvent(SUnitGetClient(hUnit), 19);
	ITEMSModeResetBookSkills(ptGame, hUnit);
	return FALSE;
}

BOOL __fastcall SkillsItemAddStat(GAME *ptGame, UNIT *hUnit, UNIT *hItem,
	UNIT *hTarget, DWORD dwX, DWORD dwY,
	int nSkill, DWORD dwParam)
{
	int nStat;

	nStat = (int)D2CommonITEMSDataGetUniqueOrSetRow(hItem);
	D2CommonStatsUnitAddValue(hUnit, nStat, D2CommonStatsUnitGetValue(hItem, STAT_VALUE));
	return TRUE;
}

BOOL __fastcall SkillsItemNoSpell(GAME *ptGame, UNIT *hUnit,
	UNIT *hItem, UNIT *hTarget, DWORD dwX,
	DWORD dwY, int nSkill, DWORD dwParam)
{
	return TRUE;
}

BOOL __fastcall SkillsItemPortal(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget,
	DWORD dwX, DWORD dwY, int nSkill,
	DWORD dwParam)
{
	DWORD hRoom;
	DWORD dwTown;
	UNIT *hPortal;
	UNIT *hOther;
	DWORD dwMade;
	BYTE bAct;
	DWORD dwLevel;

	hRoom = D2CommonUnitGetPathRoom(hUnit);
	dwLevel = D2CommonDungeonRoomGetLevelId(hRoom);
	bAct = (BYTE)D2CommonGetActNum(dwLevel);
	dwTown = D2CommonDungeonActGetTownLevelId(bAct);
	if (D2CommonDungeonRoomIsTown(hRoom))
	{
		SUnitSetEvent(hUnit, 23, hUnit);
		return FALSE;
	}
	PlayerRemovePortal(ptGame, hUnit);
	hPortal = NULL;
	dwMade = SkillsOpenPortal(ptGame, hUnit, (GAMEROOM *)hRoom, (int)D2CommonUnitGetX(hUnit),
		(int)D2CommonUnitGetY(hUnit), (int)dwTown, &hPortal, OBJECT_PORTAL, 0);
	SUnitSetEvent(hUnit, 7, NULL);
	if (dwMade)
	{
		PlayerSetPortalId(hUnit, hPortal->dwId);
		if (hPortal)
		{
			ObjectEvalSetLinkValue(hPortal, (int)hUnit->dwId);
			hOther = SUnitGetPortalTarget(ptGame, hPortal);
			if (hOther)
				ObjectEvalSetLinkValue(hOther, (int)hUnit->dwId);
		}
	}
	if (hItem)
		SCmdSendUnitTypeId(SUnitGetClient(hUnit), 111, (BYTE)hItem->eType, hItem->dwId);
	return (BOOL)dwMade;
}

BOOL __fastcall SkillsItemStamina(GAME *ptGame, UNIT *hUnit, UNIT *hItem,
	UNIT *hTarget, DWORD dwX, DWORD dwY,
	int nSkill, DWORD dwParam)
{
	int nStamina;

	nStamina = D2CommonStatsUnitGetValue(hUnit, STAT_STAMINA);
	D2CommonStatsUnitAddValue(hUnit, STAT_STAMINA,
		D2CommonStatsUnitCalcMaxStamina(hUnit) - nStamina);
	return TRUE;
}

BOOL __fastcall SkillsItemCureSingleState(GAME *ptGame, UNIT *hUnit,
	UNIT *hItem,
	UNIT *hTarget, DWORD dwX, DWORD dwY,
	int nSkill, DWORD dwParam)
{
	DWORD hList;

	hList = D2CommonStatsFindListByState(hUnit, 2);
	if (hList)
	{
		D2CommonStatsUnitUnlinkList(hUnit, hList);
		D2CommonStatsListFree(hList);
	}
	if (D2CommonStatsGetState(hUnit, 2))
		D2CommonStatsUnitSetActiveStateBit(hUnit, 2, FALSE);
	return TRUE;
}

BOOL __fastcall SkillsItemCureStates(GAME *ptGame, UNIT *hUnit, UNIT *hItem,
	UNIT *hTarget, DWORD dwX, DWORD dwY,
	int nSkill, DWORD dwParam)
{
	DWORD hList;

	hList = D2CommonStatsFindListByState(hUnit, 1);
	if (hList)
		D2CommonStatsUnitUnlinkList(hUnit, hList);
	if (D2CommonStatsGetState(hUnit, 1))
		D2CommonStatsUnitSetActiveStateBit(hUnit, 1, FALSE);
	hList = D2CommonStatsFindListByState(hUnit, 11);
	if (hList)
	{
		D2CommonStatsUnitUnlinkList(hUnit, hList);
		D2CommonStatsListFree(hList);
	}
	if (D2CommonStatsGetState(hUnit, 11))
		D2CommonStatsUnitSetActiveStateBit(hUnit, 11, FALSE);
	return TRUE;
}

BOOL __fastcall SkillsItemHealPotion(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget,
	DWORD dwX, DWORD dwY, int nSkill,
	DWORD dwParam)
{
	ITEMSTXT *ptItemStats;
	DWORD dwPoints;
	DWORD dwRoll;
	DWORD dwVitality;
	DWORD hList;
	DWORD dwSum;
	int nEnd;
	int nFrames;

	ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
#line 599
	D2ASSERT(ptItemStats);
	dwPoints = 0;
	switch (ptItemStats->code)
	{
	case 540110952:
		dwPoints = 20;
		break;
	case 540176488:
		dwPoints = 40;
		break;
	case 540242024:
		dwPoints = 60;
		break;
	case 540307560:
		dwPoints = 80;
		break;
	case 540373096:
		dwPoints = 100;
		break;
	}
	dwPoints = D2CommonITEMSScaleValueByPlayerClass(hUnit, dwPoints);
	dwVitality = D2CommonStatsUnitGetValue(hUnit, STAT_VITALITY);
	if (dwVitality)
		dwRoll = D2CommonSeedRand(&hUnit->tSeed, dwVitality);
	else
		dwRoll = 0;
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < dwRoll >> 1)
		dwPoints <<= 1;
	dwPoints <<= 8;
	hList = D2CommonStatsFindListByState(hUnit, 100);
	if (!hList)
	{
		nEnd = ptGame->nFrame + 128;
		D2CommonStatsUnitSetActiveStateBit(hUnit, 100, TRUE);
		hList = D2CommonStatsAllocList(ptGame->pMemPool, 2, nEnd, hUnit->eType, hUnit->dwId);
		EventAdd(ptGame, hUnit, 12, nEnd);
		D2CommonStatsListSetValueA(hList, STAT_HPREGEN, (int)dwPoints >> 7);
		D2CommonStatsSetListState(hList, 100);
		D2CommonStatsSetListRemoveCallback(hList, SkillsStateRemove);
		D2CommonStatsUnitLinkList(hUnit, hList);
	}
	else
	{
		nFrames = (int)D2CommonStatsGetListEndFrame(hList) - ptGame->nFrame;
		dwSum = D2CommonStatsListGetValueA(hList, STAT_HPREGEN) * nFrames + dwPoints + 127;
		nFrames += 128;
		nEnd = nFrames + ptGame->nFrame;
		D2CommonStatsSetListEndFrame(hList, nEnd);
		EventAdd(ptGame, hUnit, 12, nEnd);
		D2CommonStatsListSetValueB(hList, STAT_HPREGEN, dwSum / (DWORD)nFrames);
	}
	return TRUE;
}

BOOL __fastcall SkillsItemManaPotion(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget,
	DWORD dwX, DWORD dwY, int nSkill,
	DWORD dwParam)
{
	ITEMSTXT *ptItemStats;
	DWORD dwPoints;
	DWORD dwRoll;
	DWORD dwEnergy;
	DWORD hList;
	DWORD dwSum;
	int nEnd;
	int nFrames;

	ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
#line 700
	D2ASSERT(ptItemStats);
	dwPoints = 0;
	switch (ptItemStats->code)
	{
	case 540110957:
		dwPoints = 20;
		break;
	case 540176493:
		dwPoints = 40;
		break;
	case 540242029:
		dwPoints = 60;
		break;
	case 540307565:
		dwPoints = 80;
		break;
	case 540373101:
		dwPoints = 100;
		break;
	}
	dwPoints = D2CommonITEMSScaleValueByPlayerClassAlt(hUnit, dwPoints);
	dwEnergy = D2CommonStatsUnitGetValue(hUnit, STAT_ENERGY);
	if (dwEnergy)
		dwRoll = D2CommonSeedRand(&hUnit->tSeed, dwEnergy);
	else
		dwRoll = 0;
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < dwRoll >> 1)
		dwPoints <<= 1;
	dwPoints <<= 8;
	hList = D2CommonStatsFindListByState(hUnit, 106);
	if (!hList)
	{
		nEnd = ptGame->nFrame + 128;
		D2CommonStatsUnitSetActiveStateBit(hUnit, 106, TRUE);
		hList = D2CommonStatsAllocList(ptGame->pMemPool, 2, nEnd, hUnit->eType, hUnit->dwId);
		EventAdd(ptGame, hUnit, 12, nEnd);
		D2CommonStatsListSetValueA(hList, STAT_MANARECOVERY, (int)dwPoints >> 7);
		D2CommonStatsSetListState(hList, 106);
		D2CommonStatsSetListRemoveCallback(hList, SkillsStateRemove);
		D2CommonStatsUnitLinkList(hUnit, hList);
	}
	else
	{
		nFrames = (int)D2CommonStatsGetListEndFrame(hList) - ptGame->nFrame;
		dwSum = D2CommonStatsListGetValueA(hList, STAT_MANARECOVERY) * nFrames + dwPoints + 127;
		nFrames += 128;
		nEnd = nFrames + ptGame->nFrame;
		D2CommonStatsSetListEndFrame(hList, nEnd);
		EventAdd(ptGame, hUnit, 12, nEnd);
		D2CommonStatsListSetValueB(hList, STAT_MANARECOVERY, dwSum / (DWORD)nFrames);
	}
	return TRUE;
}

static __inline void SkillsItemResetInUse(UNIT *hUnit, UNIT *hItem)
{
	void *hInventory;
	DWORD hNode;
	UNIT *hOther;

#line 142
	D2ASSERT(hUnit);
	hInventory = hUnit->ptInventory;
#line 145
	D2ASSERT(hInventory);
	for (hNode = (DWORD)D2CommonINVENTORYGetCategoryListHead(
		hInventory); hNode; hNode = D2CommonINVENTORYNodeGetNext(hNode))
	{
		hOther = (UNIT *)D2CommonINVENTORYNodeGetKeyA(hNode);
		if (D2CommonITEMSDataTestFlags(hOther, ITEMFLAG_INUSE))
		{
			D2CommonITEMSDataSetFlags(hOther, ITEMFLAG_INUSE, FALSE);
			SCmdSendSpellIcon(SUnitGetClient(hUnit), hOther, 255, 1, 0, 65535);
		}
	}
	SCmdSendUnitTypeId(SUnitGetClient(hUnit), 111, (BYTE)hItem->eType, hItem->dwId);
}

BOOL __fastcall SkillsItemUseSpell(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget, DWORD dwX,
	DWORD dwY)
{
	BOOKSTXT *ptBookMods;
	ITEMSTXT *ptItemStats;
	int nSpellOffset;
	int nSkill;
	BOOL bResult;

#line 789
	D2ASSERT(sgnItemSpellTblSize <= NUM_SPELLFUNCOFFSETS);
	if (D2CommonITEMSGetType(hItem) == ITEMTYPE_TOME)
	{
		ptBookMods = (BOOKSTXT *)D2CommonItemTblsGetBook(D2CommonITEMSDataGetMagicSuffixAt(hItem, 0));
#line 797
		D2ASSERT(ptBookMods);
		nSpellOffset = ptBookMods->spelloffset;
		nSkill = ptBookMods->skillbookoffset;
	}
	else if (D2CommonITEMSGetType(hItem) == ITEMTYPE_SCROLL)
	{
		ptBookMods = (BOOKSTXT *)D2CommonItemTblsGetBook(D2CommonITEMSDataGetMagicSuffixAt(hItem, 0));
#line 802
		D2ASSERT(ptBookMods);
		nSpellOffset = ptBookMods->spelloffset;
		nSkill = ptBookMods->skillscrolloffset;
	}
	else
	{
		ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
#line 807
		D2ASSERT(ptItemStats);
		nSpellOffset = ptItemStats->spelloffset;
		nSkill = -1;
	}
	if (nSpellOffset != -1)
	{
		if (sgtItemSpellTbl[nSpellOffset].dwUseInit && !D2CommonITEMSDataTestFlags(hItem, ITEMFLAG_INUSE))
		{
#line 813
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgtItemSpellTbl[nSpellOffset].pfnSpellInit),
				"sgtItemSpellTbl[nSpellOffset].pfnSpellInit");
			if (!sgtItemSpellTbl[nSpellOffset].pfnSpellInit(ptGame, hUnit, hItem, NULL, dwX, dwY, nSkill,
				sgtItemSpellTbl[nSpellOffset].dwParam))
				SkillsItemResetInUse(hUnit, hItem);
			return FALSE;
		}
		D2CommonITEMSDataSetFlags(hItem, ITEMFLAG_INUSE, TRUE);
#line 820
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgtItemSpellTbl[nSpellOffset].pfnSpell),
			"sgtItemSpellTbl[nSpellOffset].pfnSpell");
		bResult = sgtItemSpellTbl[nSpellOffset].pfnSpell(ptGame, hUnit, hItem, hTarget, dwX, dwY, nSkill,
			sgtItemSpellTbl[nSpellOffset].dwParam);
		if (!bResult)
			SkillsItemResetInUse(hUnit, hItem);
		return bResult;
	}
	return FALSE;
}

WORD __fastcall SkillsItemAttackerTakesDamage(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nRow, DAMAGE *ptDamage, DWORD dwUnused4)
{
	DAMAGE tDamage;
	int i;

	if (!(hUnit->dwFlags0 & 4))
		return 0;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.nPhysicalDamage = (int)(D2CommonStatsGetTotalValue(hTarget, STAT_ATTACKERTAKESDAMAGE) << 8);
	SUnitDmgApply(ptGame, hTarget, hUnit, TRUE, &tDamage);
	D2CommonUnitAddAnimOverlay(hUnit, 21, 0);
	if (!(D2CommonStatsUnitGetValue(hUnit, STAT_HITPOINTS) <= 0))
		return 0;
	tDamage.wFlags |= 3;
	SUnitDmgReact(ptGame, hTarget, hUnit, &tDamage);
	return 0;
}

WORD __fastcall SkillsItemAttackerTakesLightDmg(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nRow, DAMAGE *ptDamage, DWORD dwUnused4)
{
	DAMAGE tDamage;
	int i;

	if (!(hUnit->dwFlags0 & 4))
		return 0;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.nPhysicalDamage = (int)(D2CommonStatsGetTotalValue(hTarget, STAT_ATTACKERTAKESLIGHTDAMAGE) << 8);
	SUnitDmgApply(ptGame, hTarget, hUnit, TRUE, &tDamage);
	D2CommonUnitAddAnimOverlay(hUnit, 1, 0);
	if (!(D2CommonStatsUnitGetValue(hUnit, STAT_HITPOINTS) <= 0))
		return 0;
	tDamage.wFlags |= 3;
	SUnitDmgReact(ptGame, hTarget, hUnit, &tDamage);
	return 0;
}

WORD __fastcall SkillsItemKnockback(GAME *ptGame, UNIT *hUnit,
	UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4)
{
	return 8;
}

WORD __fastcall SkillsItemAddClassSkills(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow,
	DAMAGE *ptDamage, DWORD dwUnused4)
{
	DWORD hList;
	DWORD dwLevels;
	int nStat;

	if (hUnit->eType == UNIT_PLAYER && hUnit->dwClassId == (DWORD)sgtApplyPassiveModTbl[nRow].nParam)
	{
		nStat = sgtApplyPassiveModTbl[nRow].nStat;
		if (hUnit == hTarget)
		{
			hList = D2CommonStatsFindListByMask(hUnit, STATLIST_ITEMS);
			if (hList)
				dwLevels = D2CommonStatsListGetValueA(hList, nStat);
			else
				dwLevels = 0;
		}
		else
			dwLevels = D2CommonStatsGetTotalValue(hTarget, nStat);
		if (dwLevels)
			SkillsItemAddSkillLevels(hUnit, (int)dwLevels, FALSE, 0);
	}
	return 0;
}

void __fastcall SkillsItemAddSkillLevels(UNIT *hUnit, int nLevels, BOOL fByType, BYTE bEType)
{
	void *hSkillInfo;
	SKILLSTXT *ptSkill;
	DWORD hSkill;
	DWORD hNext;
	int nSkillId;
	BYTE *pbClass;
	BOOL fAdd;

	hSkillInfo = hUnit->field_c8;
#line 165
	D2ASSERT(hSkillInfo);
	for (hSkill = D2CommonSkillsHeadGetFirst(hSkillInfo); hSkill; hSkill = hNext)
	{
		hNext = D2CommonSkillsNodeGetNext(hSkill);
		nSkillId = (int)D2CommonSkillsGetNodeType(hSkill);
		pbClass = D2CommonSkillsGetTypeCharClass(nSkillId);
		fAdd = TRUE;
		if ((int)D2CommonSkillsCalcNodeLevel(hUnit, hSkill, 1) <= 0)
			fAdd = FALSE;
		if (*(const DWORD *)sgpszClassAll == *(DWORD *)pbClass)
			fAdd = FALSE;
		if (nSkillId < 6)
			fAdd = FALSE;
		if (!D2CommonSkillsTestIdFlag(nSkillId))
			fAdd = FALSE;
		ptSkill = (SKILLSTXT *)D2CommonDataTblsGetSkill(nSkillId);
#line 188
		D2ASSERT(ptSkill);
		if ((!fByType || ptSkill->EType == bEType) && fAdd)
		{
			D2CommonSkillsAdjustBonusByKey(hUnit, nSkillId, nLevels);
			D2CommonSkillsCalcNodeLevel(hUnit, hSkill, 0);
			SCmdSendSkill(hUnit, (WORD)nSkillId, (BYTE)D2CommonSkillsCalcNodeLevel(hUnit, hSkill, 0), 1);
		}
	}
}

WORD __fastcall SkillsItemAddSingleSkill(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow,
	DAMAGE *ptDamage, DWORD dwUnused4)
{
	DWORD hList;
	DWORD dwValue;
	DWORD dwSkillId;
	DWORD hSkill;
	DWORD dwOld;
	DWORD dwClass;
	int nLevels;
	int nStat;

	nStat = sgtApplyPassiveModTbl[nRow].nStat;
	if (hUnit->eType != UNIT_PLAYER)
		return 0;
	if (hUnit == hTarget)
	{
		hList = D2CommonStatsFindListByMask(hUnit, STATLIST_ITEMS);
		if (hList)
			dwValue = D2CommonStatsListGetValueA(hList, nStat);
		else
			dwValue = 0;
	}
	else
		dwValue = D2CommonStatsGetTotalValue(hTarget, nStat);
	if (!dwValue)
		return 0;
	dwClass = hUnit->dwClassId;
	switch (D2CommonITEMSGetType(hTarget))
	{
	case 24:
		if (dwClass != 3)
			goto fail;
		break;
	case 25:
		if (dwClass != 2)
			goto fail;
		break;
	case 26:
		if (dwClass != 1)
			goto fail;
		break;
	}
	nStat = sgtApplyPassiveModTbl[nRow].nStat;
	if (hUnit == hTarget)
	{
		hList = D2CommonStatsFindListByMask(hUnit, STATLIST_ITEMS);
		if (hList)
			dwValue = D2CommonStatsListGetValueA(hList, nStat);
		else
			dwValue = 0;
	}
	else
		dwValue = D2CommonStatsGetTotalValue(hTarget, nStat);
	D2CommonLoggingUnpackDword(dwValue, &dwSkillId, &nLevels);
	hSkill = D2CommonSkillsFindByKey(hUnit, dwSkillId);
	dwOld = 0;
	if (hSkill)
		dwOld = D2CommonSkillsCalcNodeLevel(hUnit, hSkill, 0);
	D2CommonSkillsAdjustBonusByKey(hUnit, dwSkillId, nLevels);
	SCmdSendSkill(hUnit, (WORD)dwSkillId, (BYTE)dwOld, 1);
fail:
	return 0;
}

WORD __fastcall SkillsItemHowl(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow,
	DAMAGE *ptDamage, DWORD dwUnused4)
{
	struct MONSTERUNIQUE *ptUnique;

	if (hTarget->eType != UNIT_MONSTER)
		return 0;
	if (hTarget && hUnit)
	{
		ptUnique = ((MONSTERDATA *)hTarget->uTypeData)->ptUnique;
		if (!MonsterUniqueIsChampion(ptUnique) && !MonsterUniqueIsCounted(ptUnique))
			MonsterScare(ptGame, hUnit, hTarget, 20, 20);
	}
	return 0;
}

WORD __fastcall SkillsItemStupidity(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow,
	DAMAGE *ptDamage, DWORD dwUnused4)
{
	if (hTarget && hUnit)
		SkillsDimVisionCurse(ptGame, hUnit, hTarget, 1);
	return 0;
}

WORD __fastcall SkillsItemSlow(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nRow, DAMAGE *ptDamage, DWORD dwUnused4)
{
	SKILLSTATE tState;
	int nSlow;
	int i;

	nSlow = (int)D2CommonStatsGetTotalValue(hUnit, STAT_SLOW);
	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tState)[i] = 0;
	tState.hSource = hUnit;
	tState.hTarget = hTarget;
	tState.nSkill = 0;
	tState.nLevel = 1;
	tState.nDuration = 50;
	tState.dwStatId = 67;
	tState.nStatValue = -nSlow;
	tState.dwState = 44;
	SkillsSetState(&tState);
	return 0;
}

WORD __fastcall SkillsItemFlagOne(GAME *ptGame, UNIT *hUnit,
	UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4)
{
	return 1;
}

WORD __fastcall SkillsItemReanimate(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow,
	DAMAGE *ptDamage, DWORD dwUnused4)
{
	struct MONSTERUNIQUE *ptUnique;
	int nChance;
	int nLevel;

	ptUnique = ((MONSTERDATA *)hTarget->uTypeData)->ptUnique;
	if (MonsterUniqueIsChampion(ptUnique) || MonsterUniqueIsCounted(ptUnique))
		return 0;
	nChance = (int)D2CommonStatsGetTotalValue(hUnit, STAT_REANIMATE);
	if ((int)D2CommonSeedRand(&hUnit->tSeed, 100) >= nChance)
		return 0;
	nLevel = D2CommonStatsUnitGetValue(hUnit, STAT_LEVEL);
	if (!PlayerPetsGetMax(hUnit, PET_SKELETON))
		PlayerPetsSetMax(ptGame, hUnit, PET_SKELETON, 1);
	SkillsNecUse(ptGame, hUnit, SKILL_RAISESKELETON, nLevel);
	return 0;
}

WORD __fastcall SkillsItemDamageToMana(GAME *ptGame, UNIT *hUnit,
	UNIT *hTarget, int nRow, DAMAGE *ptDamage, DWORD dwUnused4)
{
	int nPercent;
	int nMax;
	int nMana;

	if (!hTarget || hTarget->eType != UNIT_PLAYER)
		return 0;
	nPercent = (int)D2CommonStatsGetTotalValue(hTarget, STAT_DAMAGETOMANA);
	if (!nPercent)
		return 0;
	nMax = D2CommonStatsUnitCalcMaxMana(hTarget);
	nMana = ptDamage->nTotalDamage * nPercent / 100;
	if (D2CommonStatsUnitGetValue(hTarget, STAT_MANA) + nMana > nMax)
		nMana = nMax - D2CommonStatsUnitGetValue(hTarget, STAT_MANA);
	D2CommonStatsUnitAddValue(hTarget, STAT_MANA, nMana);
	return 0;
}

WORD __fastcall SkillsItemAddFireSkills(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow,
	DAMAGE *ptDamage, DWORD dwUnused4)
{
	DWORD hList;
	DWORD dwLevels;
	int nStat;

	if (hUnit->eType == UNIT_PLAYER)
	{
		nStat = sgtApplyPassiveModTbl[nRow].nStat;
		if (hUnit == hTarget)
		{
			hList = D2CommonStatsFindListByMask(hUnit, STATLIST_ITEMS);
			if (hList)
				dwLevels = D2CommonStatsListGetValueA(hList, nStat);
			else
				dwLevels = 0;
		} else
			dwLevels = D2CommonStatsGetTotalValue(hTarget, nStat);
		if (dwLevels)
			SkillsItemAddSkillLevels(hUnit, (int)dwLevels, TRUE, 1);
	}
	return 0;
}

WORD __fastcall SkillsItemFreeze(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow,
	DAMAGE *ptDamage, DWORD dwUnused4)
{
	DAMAGE tDamage;
	int nLength;
	int i;

	if (!hTarget || !hUnit)
		return 0;
	nLength = D2CommonStatsUnitGetValue(hUnit, STAT_LEVEL) * 2 + 100;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.nFreezeLength = nLength;
	SUnitDmgApply(ptGame, hUnit, hTarget, TRUE, &tDamage);
	return 0;
}

WORD __fastcall SkillsItemOpenWounds(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nRow, DAMAGE *ptDamage, DWORD dwUnused4)
{
	struct MONSTERUNIQUE *ptUnique;
	SKILLSTATE tState;
	DWORD dwChance;
	int nLength;
	int i;

	if (!hTarget || hUnit->eType != UNIT_PLAYER || hTarget->eType != UNIT_MONSTER)
		return 0;
	ptUnique = ((MONSTERDATA *)hTarget->uTypeData)->ptUnique;
	if (MonsterUniqueIsChampion(ptUnique) || MonsterUniqueIsCounted(ptUnique))
		return 0;
	dwChance = D2CommonStatsGetTotalValue(hUnit, STAT_OPENWOUNDS);
	if (D2CommonSeedRand(&hUnit->tSeed, 100) >= dwChance)
		return 0;
	nLength = D2CommonStatsUnitGetValue(hUnit, STAT_LEVEL) * 2 + 100;
	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tState)[i] = 0;
	tState.hSource = hUnit;
	tState.hTarget = hTarget;
	tState.nSkill = 0;
	tState.nLevel = 1;
	tState.nDuration = nLength;
	tState.dwStatId = 74;
	tState.nStatValue = -20;
	tState.dwState = 62;
	SkillsSetState(&tState);
	return 0;
}

WORD __fastcall SkillsItemCrushingBlow(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nRow, DAMAGE *ptDamage, DWORD dwUnused4)
{
	struct MONSTERUNIQUE *ptUnique;
	DWORD dwChance;
	int nLife;
	WORD wResult;

	wResult = 0;
	if (hUnit->eType == UNIT_PLAYER && hTarget->eType == UNIT_MONSTER)
	{
		dwChance = D2CommonStatsGetTotalValue(hUnit, STAT_CRUSHINGBLOW);
		if (D2CommonSeedRand(&hUnit->tSeed, 100) < dwChance)
		{
			ptUnique = ((MONSTERDATA *)hTarget->uTypeData)->ptUnique;
			if (!MonsterUniqueIsChampion(ptUnique) && !MonsterUniqueIsCounted(ptUnique))
			{
				nLife = D2CommonStatsUnitGetValue(hTarget, STAT_HITPOINTS) >> 1;
				if (nLife <= 0)
					wResult = 2;
				D2CommonStatsUnitSetValue(hTarget, STAT_HITPOINTS, nLife);
			}
		}
	}
	return wResult;
}

WORD __fastcall SkillsItemKickDamage(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nRow, DAMAGE *ptDamage, DWORD dwUnused4)
{
	DWORD dwKick;

	dwKick = D2CommonStatsGetTotalValue(hUnit, STAT_KICKDAMAGE);
	if (hUnit->eType != UNIT_PLAYER || !D2CommonUnitGetSkillThirdCached(hUnit))
		return 0;
#line 1301
	D2ASSERT(ptDamage);
	ptDamage->nPhysicalDamage += (int)(dwKick << 8);
	return 0;
}

WORD __fastcall SkillsItemManaAfterKill(GAME *ptGame, UNIT *hUnit,
	UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4)
{
	int nMana;
	int nMax;
	int nCurrent;

	if (hUnit->eType != UNIT_PLAYER)
		return 0;
	nMana = (int)(D2CommonStatsGetTotalValue(hUnit, STAT_MANAAFTERKILL) << 8);
	nMax = D2CommonStatsUnitCalcMaxMana(hUnit);
	nCurrent = D2CommonStatsUnitGetValue(hUnit, STAT_MANA);
	if (nCurrent + nMana > nMax)
		nMana = nMax - nCurrent;
	D2CommonStatsUnitAddValue(hUnit, STAT_MANA, nMana);
	if (nMana)
		D2CommonUnitAddAnimOverlay(hUnit, 152, 0);
	return 0;
}

WORD __fastcall SkillsItemHealAfterDemonKill(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nRow, DAMAGE *ptDamage, DWORD dwUnused4)
{
	int nLife;
	int nMax;
	int nHp;

	if (hUnit->eType != UNIT_PLAYER || !D2CommonMonsterRegionUnitIsDemonRaw((DWORD)hTarget))
		return 0;
	nLife = (int)(D2CommonStatsGetTotalValue(hUnit, STAT_HEALAFTERDEMONKILL) << 8);
	nMax = D2CommonStatsUnitCalcMaxHp(hUnit);
	nHp = nLife + D2CommonStatsUnitGetValue(hUnit, STAT_HITPOINTS);
	if (nMax < nHp)
		nHp = nMax;
	D2CommonStatsUnitSetValue(hUnit, STAT_HITPOINTS, nHp);
	return 0;
}

WORD __fastcall SkillsItemApplyPassiveMods(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nEvent,
	DAMAGE *ptDamage, DWORD dwUnused4, BOOL fArg5)
{
	WORD wResult;
	int i;

	if (nEvent == 3)
		D2CommonUnitApplyPrimaryWeaponSwap(hUnit);
	wResult = 0;
	for (i = 0; i < sgnApplyPassiveModTblSize; i++)
	{
		if (sgtApplyPassiveModTbl[i].nEvent != nEvent)
			continue;
		if (sgtApplyPassiveModTbl[i].fFlag && !fArg5)
			continue;
		if ((sgtApplyPassiveModTbl[i].nOwner == 2
				&& (int)D2CommonStatsGetTotalValue(hTarget, sgtApplyPassiveModTbl[i].nStat) > 0)
			|| (sgtApplyPassiveModTbl[i].nOwner == 1
				&& (int)D2CommonStatsGetTotalValue(hUnit, sgtApplyPassiveModTbl[i].nStat) > 0))
		{
#line 1376
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgtApplyPassiveModTbl[i].pFunc),
				"sgtApplyPassiveModTbl[i].pFunc");
			wResult |= sgtApplyPassiveModTbl[i].pFunc(ptGame, hUnit, hTarget, i, ptDamage, dwUnused4);
		}
	}
	if (nEvent == 3)
		D2CommonUnitApplyEquipStatSwap(hUnit);
	if (hUnit && hUnit->eType == UNIT_PLAYER)
		D2CommonSkillsRefreshPassives(hUnit);
	if (hTarget && hTarget->eType == UNIT_PLAYER)
		D2CommonSkillsRefreshPassives(hTarget);
	return wResult;
}

WORD __fastcall SkillsItemRemoveClassSkills(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nRow, DAMAGE *ptDamage, DWORD dwUnused4)
{
	DWORD hList;
	DWORD dwLevels;
	int nStat;

	if (hUnit->eType == UNIT_PLAYER && hUnit->dwClassId == (DWORD)sgtRemovePassiveModTbl[nRow].nParam)
	{
		nStat = sgtRemovePassiveModTbl[nRow].nStat;
		if (hUnit == hTarget)
		{
			hList = D2CommonStatsFindListByMask(hUnit, STATLIST_ITEMS);
			if (hList)
				dwLevels = D2CommonStatsListGetValueA(hList, nStat);
			else
				dwLevels = 0;
		}
		else
			dwLevels = D2CommonStatsGetTotalValue(hTarget, nStat);
		if (dwLevels)
			SkillsItemAddSkillLevels(hUnit, -(int)dwLevels, FALSE, 0);
	}
	return 0;
}

WORD __fastcall SkillsItemRemoveSingleSkill(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nRow, DAMAGE *ptDamage, DWORD dwUnused4)
{
	DWORD hList;
	DWORD dwValue;
	DWORD dwSkillId;
	DWORD hSkill;
	DWORD dwClass;
	DWORD dwLevel;
	int nLevels;
	int nStat;

	nStat = sgtRemovePassiveModTbl[nRow].nStat;
	if (hUnit->eType != UNIT_PLAYER)
		return 0;
	if (hUnit == hTarget)
	{
		hList = D2CommonStatsFindListByMask(hUnit, STATLIST_ITEMS);
		if (hList)
			dwValue = D2CommonStatsListGetValueA(hList, nStat);
		else
			dwValue = 0;
	}
	else
		dwValue = D2CommonStatsGetTotalValue(hTarget, nStat);
	if (!dwValue)
		return 0;
	dwClass = hUnit->dwClassId;
	switch (D2CommonITEMSGetType(hTarget))
	{
	case 24:
		if (dwClass != 3)
			goto fail;
		break;
	case 25:
		if (dwClass != 2)
			goto fail;
		break;
	case 26:
		if (dwClass != 1)
			goto fail;
		break;
	}
	nStat = sgtRemovePassiveModTbl[nRow].nStat;
	if (hUnit == hTarget)
	{
		hList = D2CommonStatsFindListByMask(hUnit, STATLIST_ITEMS);
		if (hList)
			dwValue = D2CommonStatsListGetValueA(hList, nStat);
		else
			dwValue = 0;
	}
	else
		dwValue = D2CommonStatsGetTotalValue(hTarget, nStat);
	D2CommonLoggingUnpackDword(dwValue, &dwSkillId, &nLevels);
	hSkill = D2CommonSkillsFindByKey(hUnit, dwSkillId);
	if (!hSkill)
		return 0;
	D2CommonSkillsAdjustBonusByKey(hUnit, dwSkillId, -nLevels);
	if (!D2CommonSkillsCalcNodeLevel(hUnit, hSkill, 1))
	{
		if (D2CommonUnitGetRightSkill(hUnit) == hSkill)
			SkillsSetCurrent(hUnit, FALSE, SKILL_ATTACK);
		if (D2CommonUnitGetLeftSkill(hUnit) == hSkill)
			SkillsSetCurrent(hUnit, TRUE, SKILL_ATTACK);
	}
	dwLevel = D2CommonSkillsCalcNodeLevel(hUnit, hSkill, 0);
	SCmdSendSkill(hUnit, (WORD)dwSkillId, (BYTE)dwLevel, 1);
fail:
	return 0;
}

WORD __fastcall SkillsItemRemoveFireSkills(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow,
	DAMAGE *ptDamage, DWORD dwUnused4)
{
	DWORD hList;
	DWORD dwLevels;
	int nStat;

	if (hUnit->eType == UNIT_PLAYER)
	{
		nStat = sgtRemovePassiveModTbl[nRow].nStat;
		if (hUnit == hTarget)
		{
			hList = D2CommonStatsFindListByMask(hUnit, STATLIST_ITEMS);
			if (hList)
				dwLevels = D2CommonStatsListGetValueA(hList, nStat);
			else
				dwLevels = 0;
		} else
			dwLevels = D2CommonStatsGetTotalValue(hTarget, nStat);
		if (dwLevels)
			SkillsItemAddSkillLevels(hUnit, -(int)dwLevels, TRUE, 1);
	}
	return 0;
}

void __fastcall SkillsItemRemovePassiveMods(GAME *ptGame, UNIT *hUnit, UNIT *hItem, int nEvent)
{
	DWORD i;

	for (i = 0; i < sgnRemovePassiveModTblSize; i++)
	{
		if (sgtRemovePassiveModTbl[i].nEvent != nEvent)
			continue;
		if (sgtRemovePassiveModTbl[i].nOwner == 1)
		{
			if ((int)D2CommonStatsGetTotalValue(hUnit, sgtRemovePassiveModTbl[i].nStat) > 0)
			{
#line 1515
				D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgtRemovePassiveModTbl[i].pFunc),
					"sgtRemovePassiveModTbl[i].pFunc");
				sgtRemovePassiveModTbl[i].pFunc(ptGame, hUnit, hItem, (int)i, NULL, 0);
			}
		}
		else if (sgtRemovePassiveModTbl[i].nOwner == 2)
		{
			if ((int)D2CommonStatsGetTotalValue(hItem, sgtRemovePassiveModTbl[i].nStat) > 0)
			{
#line 1520
				D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgtRemovePassiveModTbl[i].pFunc),
					"sgtRemovePassiveModTbl[i].pFunc");
				sgtRemovePassiveModTbl[i].pFunc(ptGame, hUnit, hItem, (int)i, NULL, 0);
			}
		}
	}
}

BOOL __fastcall SkillsItemUseBook(GAME *ptGame, UNIT *hUnit, int nSkillId)
{
	void *hInventory;
	BOOKSTXT *pSuffix;
	DWORD hNode;
	UNIT *hItem;

	int nX;
	int nY;

	hInventory = hUnit->ptInventory;
#line 1549
	D2ASSERT(hInventory);
	for (hNode = (DWORD)D2CommonINVENTORYGetCategoryListHead(
		hInventory); hNode; hNode = D2CommonINVENTORYNodeGetNext(hNode))
	{
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(hNode);
		if (D2CommonITEMSGetType(hItem) == ITEMTYPE_TOME)
		{
			pSuffix = (BOOKSTXT *)D2CommonItemTblsGetBook(D2CommonITEMSDataGetMagicSuffixAt(hItem, 0));
#line 1557
			D2ASSERT(pSuffix);
			if (pSuffix->skillbookoffset != nSkillId)
				continue;
			if (D2CommonStatsUnitGetValue(hItem, STAT_QUANTITY) <= 0)
				continue;
		}
		else if (D2CommonITEMSGetType(hItem) == ITEMTYPE_SCROLL)
		{
			pSuffix = (BOOKSTXT *)D2CommonItemTblsGetBook(D2CommonITEMSDataGetMagicSuffixAt(hItem, 0));
#line 1564
			D2ASSERT(pSuffix);
			if (pSuffix->skillscrolloffset != nSkillId)
				continue;
		}
		else
			continue;
		switch (D2CommonINVENTORYNodeGetCategory(hNode))
		{ int nResult;
		case 1:
			ITEMSModeUseItem(ptGame, hUnit, hItem->dwId, (DWORD)((int)D2CommonUnitGetX(hItem)), (DWORD)((int)D2CommonUnitGetY(hItem)), &nResult);
			return TRUE;
		case 2:

			ITEMSModeUseBeltItem(ptGame, hUnit, hItem->dwId, (DWORD)((int)D2CommonUnitGetX(hItem)), (DWORD)((int)D2CommonUnitGetY(hItem)), &nResult);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL __fastcall SkillsItemStart(GAME *ptGame, UNIT *hUnit,
	int nSkillId  , int nLevel)
{
#line 1594
	D2ASSERT(nSkillId >= SKILL_ITEM_START);
#line 1595
	D2ASSERT(nSkillId - SKILL_ITEM_START < NUM_ITEM_SKILLS);
	return TRUE;
}

BOOL __fastcall SkillsItemUse(GAME *ptGame, UNIT *hUnit, int nSkillId  ,
	int nLevel)
{
	DWORD i;

#line 1605
	D2ASSERT(nSkillId >= SKILL_ITEM_START);
#line 1606
	D2ASSERT(nSkillId - SKILL_ITEM_START < NUM_ITEM_SKILLS);
	for (i = 0; i < sgnItemSkillTblSize; i++)
	{
		if (sgtItemSkillTbl[i].nSkillId == nSkillId)
		{
#line 1610
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgtItemSkillTbl[i].pfnSkill), "sgtItemSkillTbl[i].pfnSkill");
			sgtItemSkillTbl[i].pfnSkill(ptGame, hUnit, nSkillId);
			return TRUE;
		}
	}
	return FALSE;
}
