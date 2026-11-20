#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\ITEMS\\ItemMode.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "../GAME/SCmd.h"
#include "../INVENTORY/InvMode.h"
#include "ItemMode.h"
#include "Items.h"
#include "../PLAYER/Player.h"
#include "../PLAYER/PlayerPets.h"
#include "../PLAYER/PlrTrade.h"
#include "../QUESTS/QUESTS.h"
#include "../QUESTS/a2q2.h"
#include "../SKILLS/SkillItem.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitMsg.h"
#include "../SKILLS/Skills.h"

#define UNIT_ITEM			4
#define ITEMTYPE_GOLD		4
#define ITEMTYPE_GEM		20
#define ITEM_MODE_INVBELT	2
#define STAT_GOLD			14
#define STAT_QUANTITY		70
#define NUM_EVENTS			13

typedef struct BELTSTXT BELTSTXT;

struct ITEMQUEST
{
 	int			nQuest;
 	BYTE		bItemQuest;
 	BYTE		unobserved_05[3];
};
static ITEMQUEST sgatItemQuests[4] =
{
	{ 3, 4 }, { 18, 17 }, { 19, 18 }, { 27, 25 },
};
static DWORD sgadwStackTypes[6] =
{
	5, 6, 42, 43, 38, 41,
};
static DWORD sgnStackTypes = 6;
static DWORD sgadwRefreshCategories[4] =
{
	1, 3, 4, 5,
};
static DWORD sgnRefreshCategories = 4;

struct ITEMHANDINFO
{
 	BOOL		field_00;
 	BOOL		field_04;
 	BOOL		field_08;
 	BOOL		field_0c;
 	BOOL		field_10;
 	BOOL		field_14;
 	BOOL		field_18;
 	BOOL		field_1c;
 	BOOL		field_20;
 	BOOL		field_24;
};

static void *ITEMSModeGetItemInventory(UNIT *hItem)
{
	return hItem->ptInventory;
}

extern void * __fastcall ITEMSModeGetInventory(UNIT *hItem);
__inline void * __fastcall ITEMSModeGetInventory(UNIT *hItem)
{
	D2ASSERT(hItem);
	return hItem->ptInventory;
}

static int __fastcall ITEMSModeGetBookSkill(UNIT *hItem)
{
	DWORD nType;
	BOOKSTXT *pSuffix;

#line 116
	D2ASSERT(hItem);
	nType = D2CommonITEMSGetType(hItem);
	if (nType == 22 || nType == 18)
	{
		pSuffix = (BOOKSTXT *)D2CommonItemTblsGetBook(D2CommonITEMSDataGetMagicSuffixAt(hItem, 0));
#line 122
		D2ASSERT(pSuffix);
		D2_OPAQUE(nType);
		if (nType == 22)
			return pSuffix->skillscrolloffset;
		if (nType == 18)
			return pSuffix->skillbookoffset;
#line 128
		D2ASSERT(0);
	}
	return -1;
}

BOOL __fastcall ITEMSModeUpdateBookSkill(UNIT *hUnit, UNIT *hItem, BOOL fAdd)
{
	int nSkill;
	int nQuantity;
	int nCount;
	void *hSkill;
	BOOL bOk = TRUE;

	nSkill = ITEMSModeGetBookSkill(hItem);
	if (!(nSkill == -1))
	{
		nQuantity = D2CommonStatsUnitGetValue(hItem, STAT_QUANTITY);
		if (!nQuantity)
			nQuantity = 1;
		hSkill = (void *)D2CommonSkillsFindByKey(hUnit, nSkill);
		if (fAdd)
		{
			if (!hSkill)
			{
				nCount = nQuantity;
				D2CommonStatsUnitAddValue(hUnit, 5, nQuantity);
				SkillsAdd(hUnit, nSkill);
				hSkill = (void *)D2CommonSkillsFindByKey(hUnit, nSkill);
#line 172
				D2ASSERT(hSkill);
			}
			else
				nCount = D2CommonSkillsGetNodeCount(hSkill) + nQuantity;
		}
		else
		{
			if (hSkill) {
				nCount = D2CommonSkillsGetNodeCount(hSkill) - nQuantity;
				if (nCount < 0)
					nCount = 0;
			} else {
#line 183
				D2ASSERT(0);
				bOk = FALSE; } }
		if (bOk) {
		D2CommonSkillsSetNodeCount(hSkill, nCount);
		SCmdSendSkillState(SUnitGetClient(hUnit), (BYTE)hUnit->eType, hUnit->dwId, (WORD)nSkill,
			(BYTE)nCount);
		return TRUE; } }
	return FALSE;
}

static inline void ITEMSModeRemoveItemSkill(UNIT *hUnit, UNIT *hItem)
{
#line 388
	D2ASSERT(hUnit);
#line 389
	D2ASSERT(hItem);
	if (hUnit->eType == UNIT_PLAYER)
		ITEMSModeUpdateBookSkill(hUnit, hItem, FALSE);
}

void __fastcall ITEMSModeRemoveEffects(GAME *ptGame, UNIT *hItem, UNIT *hUnit, BOOL fForce,
	BOOL fClamp)
{
	UNIT *hOther;

	DWORD eUnitType;
	if (!hUnit)
		return;
	hOther = NULL;
	eUnitType = hUnit->eType;
	if (eUnitType == UNIT_PLAYER)
		hOther = (UNIT *)D2CommonINVENTORYFindEquippedMatch(hUnit->ptInventory);
	if (!D2CommonITEMSDataTestFlags(hItem, 256) || fForce)
	{
		D2CommonStatsUnitRemoveDye(hUnit, hItem);
		SkillsItemRemovePassiveMods(ptGame, hUnit, hItem, 4);
	}
	if (eUnitType == UNIT_PLAYER && hUnit->dwClassId == 4 && hOther && D2CommonITEMSIsWeapon(hItem)
		&& hOther != hItem)
	{
		if ((!D2CommonITEMSDataTestFlags(hOther, 256) || fForce) && D2CommonStatsTestModifying(hOther))
		{
			D2CommonStatsUnitRemoveDye(hUnit, hOther);
			D2CommonStatsUnitApplyDye(hUnit, hOther, 1);
		}
	}
	ITEMSModeRemoveItemSkill(hUnit, hItem);
	if (fClamp)
		D2CommonStatsUnitClampToMax(hUnit);
}

static __inline void *ITEMSModeQuestRecord(GAME *ptGame, UNIT *hUnit)
{
	return ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
}

BOOL __fastcall ITEMSModeCanTakeQuestItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	GAMEITEMSTXT *ptItemStats;
	GAMEITEMSTXT *ptInvStats;
	void *ptNode;
	void *ptNext;
	UNIT *hInvItem;
	DWORD dwCode;
	int nQuest;
	int i;
	void *pInventory;

	ptItemStats = (GAMEITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
	if (!ptItemStats->quest)
		return TRUE;
	dwCode = ptItemStats->code;
	if (dwCode == 544437089)
	{
		if (!D2CommonStepTestGridBit(ITEMSModeQuestRecord(ptGame, hUnit), 9, 5))
			return FALSE;
	}
	else if (dwCode == 540291946)
	{
		if (D2CommonStepTestGridBit(ITEMSModeQuestRecord(ptGame, hUnit), 20, 0))
			return FALSE;
	}
	else if (dwCode == 544897400)
	{
		if (!D2CommonStepTestGridBit(ITEMSModeQuestRecord(ptGame, hUnit), 20, 5))
			return FALSE;
	}
	nQuest = ptItemStats->quest - 1;
	if (nQuest == 9)
	{
		if (D2CommonStepTestGridBit(ITEMSModeQuestRecord(ptGame, hUnit), 10, 0) && ptItemStats->code
				!= 544763746)
			return FALSE;
	}
	else if (nQuest == 4)
	{
		if (D2CommonStepTestGridBit(ITEMSModeQuestRecord(ptGame, hUnit), 4, 0) && ptItemStats->code
				!= 543647084)
			return FALSE;
	}
	else
	{
		for (i = 0; i < 4; i++)
			if (ptItemStats->quest == sgatItemQuests[i].bItemQuest
				&& D2CommonStepTestGridBit(ITEMSModeQuestRecord(ptGame, hUnit), sgatItemQuests[i].nQuest, 0))
				return FALSE;
	}
	pInventory = hUnit->ptInventory;
	for (ptNode = D2CommonINVENTORYGetCategoryListHead(pInventory); ptNode; ptNode = ptNext)
	{
		hInvItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
		ptNext = (void *)D2CommonINVENTORYNodeGetNext(ptNode);
		ptInvStats = (GAMEITEMSTXT *)D2CommonItemTblsGetItemStats(hInvItem->dwClassId);
		if (!ptInvStats->quest || D2CommonITEMSDataGetPage(hInvItem) == 1
			|| ptInvStats->quest != ptItemStats->quest)
			continue;
		if (ptItemStats->code == ptInvStats->code
			|| (ptItemStats->code == 540291946 && ptInvStats->code == 540291943)
			|| (ptItemStats->code == 540291943 && ptInvStats->code == 540291946)
			|| (ptItemStats->code == 544435042 && ptInvStats->code == 543452002)
			|| (ptItemStats->code == 543452002 && ptInvStats->code == 544435042)
			|| (ptItemStats->code == 540226404 && ptInvStats->code == 540226407)
			|| (ptItemStats->code == 540226407 && ptInvStats->code == 540226404)
			|| (ptItemStats->code == 544502632 && ptInvStats->code == 543585133)
			|| (ptItemStats->code == 543585133 && ptInvStats->code == 544502632)
			|| (ptItemStats->code == 544502632 && ptInvStats->code == 544237942)
			|| (ptItemStats->code == 544237942 && ptInvStats->code == 544502632)
			|| (ptItemStats->code == 540173937 && ptInvStats->code == 540108401)
			|| (ptItemStats->code == 540108401 && ptInvStats->code == 540173937)
			|| (ptItemStats->code == 540173937 && ptInvStats->code == 544368753)
			|| (ptItemStats->code == 544368753 && ptInvStats->code == 540173937)
			|| (ptItemStats->code == 540173937 && ptInvStats->code == 544826737)
			|| (ptItemStats->code == 544826737 && ptInvStats->code == 540173937)
			|| (ptItemStats->code == 540173937 && ptInvStats->code == 544367217)
			|| (ptItemStats->code == 544367217 && ptInvStats->code == 540173937))
			return FALSE;
	}
	return TRUE;
}
void __fastcall ITEMSModePickupToCursor(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	int *pnResult)
{
	void *hInventory;
	UNIT *hItem;

#line 691
	D2ASSERT(ptGame);
#line 692
	D2ASSERT(hUnit);
	*pnResult = 0;
	hInventory = hUnit->ptInventory;
#line 697
	D2ASSERT(hInventory);
	if (D2CommonINVENTORYGetCursorItem(hInventory))
		return;
	if (PlayerIsBusy(hUnit))
		return;
	ITEMSModeClearMarkedItems(hUnit);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem)
		return;
	if (hItem->dwMode != 3)
	{
		*pnResult = 1;
		return;
	}
	if (!ITEMSModeCanTakeQuestItem(ptGame, hUnit, hItem))
	{
		D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hItem), hItem->eType, hItem->dwId);
		D2CommonITEMSDataSetPage(hItem, 255);
		hItem->dwFlags0 |= 4096;
		D2CommonUnitSetMode(hItem, 3);
		SUnitSetEvent(hUnit, 18, hUnit);
		return;
	}
	D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hItem), hItem->eType, hItem->dwId);
	if (D2CommonITEMSGetType(hItem) == ITEMTYPE_GOLD)
		ITEMSModePickupGold(ptGame, hUnit, hItem);
	else
	{
		D2CommonUnitClearCollision(hItem);
		D2CommonUnitRoomRemoveUnit(hItem);
		hItem->dwFlags0 &= ~2;
		D2CommonINVENTORYSetCursorItem(hInventory, hItem);
		D2CommonUnitSetMode(hItem, 4);
		D2CommonITEMSDataSetCmdFlags(hItem, 64, 1);
		D2CommonITEMSDataSetPage(hItem, 255);
		D2CommonUnitSetChanged(hUnit, 1);
		hItem->dwFlags0 &= ~33554432;
		(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
		QuestsItemAcquired(ptGame, hUnit, hItem);
	}
	SUnitSetEvent(hUnit, 1, hUnit);
}

void __fastcall ITEMSModePickupGold(GAME *ptGame, UNIT *hUnit, UNIT *hGold)
{
	int nMaxGold;
	int nGold;
	int nAmount;
	int nTake;
	int nLeft;
	UNIT *hOwner;

	nMaxGold = D2CommonUnitCalcGoldLimit(hUnit);
	nGold = D2CommonStatsUnitGetValue(hUnit, STAT_GOLD);
	nAmount = D2CommonStatsUnitGetValue(hGold, STAT_GOLD);
	nLeft = 0;
	nTake = nAmount;
	if (nAmount + nGold > nMaxGold)
	{
		nTake = nMaxGold - nGold;
		nLeft = nAmount - nTake;
	}
	hOwner = SUnitGetOwner(ptGame, hGold);
	if (hOwner && hOwner->eType == UNIT_PLAYER)
		D2CommonStatsUnitAddValue(hUnit, STAT_GOLD, nTake);
	else if (SUnitGetParty(hUnit) != 65535)
		nLeft = PartyShareGold(ptGame, hUnit, nAmount);
	else
		D2CommonStatsUnitAddValue(hUnit, STAT_GOLD, nTake);
	D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hGold), hGold->eType, hGold->dwId);
	D2CommonUnitClearCollision(hGold);
	D2CommonUnitRoomRemoveUnit(hGold);
	hGold->dwFlags0 &= ~2;
	ITEMSFree(ptGame, hGold);
	if (nLeft > 0)
	{
		ITEMSDropGoldPiles(ptGame, hUnit, nLeft);
		SUnitSetEvent(hUnit, 22, hUnit);
	}
}

BOOL __fastcall ITEMSModeFindBodyLoc(UNIT *hUnit, UNIT *hItem, int *pnLoc)
{
	void *hInventory;
	UNIT *hRight;
	UNIT *hLeft;
	UNIT *hFirst;
	UNIT *hSecond;
	DWORD nType;
	BYTE bLoc1;
	BYTE bLoc2;
	ITEMHANDINFO tOther;
	ITEMHANDINFO tItem;

#line 1064
	D2ASSERT(hUnit);
#line 1065
	D2ASSERT(hItem);
	{
		if (!D2CommonITEMSTestFitsInventorySpace(hItem, hUnit, 0, 0, 0) || !D2CommonITEMSTestHasBodyLoc(hItem)
			|| !D2CommonITEMSDataTestFlags(hItem, 16) || !D2CommonITEMSTestEffectsActive(hItem)
			|| D2CommonITEMSGetType(hItem) == 38)
			return FALSE;
		hInventory = hUnit->ptInventory;
		hRight = (UNIT *)D2CommonINVENTORYGetLocSlot(hInventory, 4);
		hLeft = (UNIT *)D2CommonINVENTORYGetLocSlot(hInventory, 5);
		nType = D2CommonITEMSGetType(hItem);
		if (nType == 6)
		{
			if (!(hRight && D2CommonITEMSGetType(hRight) == 35)
				&& !(hLeft && D2CommonITEMSGetType(hLeft) == 35))
				return FALSE;
		}
		else if (nType == 5)
		{
			if (!(hRight && D2CommonITEMSGetType(hRight) == 27)
				&& !(hLeft && D2CommonITEMSGetType(hLeft) == 27))
				return FALSE;
		}
		D2CommonITEMSGetTypeBodyLocs(hItem, &bLoc1, &bLoc2);
		if (bLoc1 == bLoc2)
		{
			if (D2CommonINVENTORYGetLocSlot(hInventory, bLoc1))
				return FALSE;
			*pnLoc = bLoc1;
			return TRUE;
		}
		ITEMSModeGetHandInfo(hUnit, hItem, &tItem);
		hFirst = (UNIT *)D2CommonINVENTORYGetLocSlot(hInventory, bLoc1);
		hSecond = (UNIT *)D2CommonINVENTORYGetLocSlot(hInventory, bLoc2);
		if (hFirst && hSecond)
			return FALSE;
		if (!hFirst && !hSecond)
		{
			*pnLoc = bLoc1;
			return TRUE;
		}
		if (hFirst)
		{
			ITEMSModeGetHandInfo(hUnit, hFirst, &tOther);
			if (!ITEMSModeCanHoldTogether(&tItem, &tOther))
				return FALSE;
			*pnLoc = bLoc2;
			return TRUE;
		}
		ITEMSModeGetHandInfo(hUnit, hSecond, &tOther);
		if (!ITEMSModeCanHoldTogether(&tItem, &tOther))
			return FALSE;
		*pnLoc = bLoc1;
		return TRUE; }
}
#line 1118

void __fastcall ITEMSModeGetHandInfo(UNIT *hUnit, UNIT *hItem, ITEMHANDINFO *ptInfo)
{
	int i;

#line 976
	D2ASSERT(hItem);
#line 977
	D2ASSERT(hUnit);
	for (i = 0; i < (int)(sizeof(ITEMHANDINFO) / sizeof(DWORD)); i++)
		((DWORD *)ptInfo)[i] = 0;
	if (hUnit->eType == UNIT_PLAYER && hUnit->dwClassId == 4)
		ptInfo->field_1c = TRUE;
	if (D2CommonITEMSGetType(hItem) == 27)
		ptInfo->field_00 = TRUE;
	if (D2CommonITEMSGetType(hItem) == 35)
		ptInfo->field_04 = TRUE;
	if (D2CommonITEMSGetType(hItem) == 5)
		ptInfo->field_08 = TRUE;
	if (D2CommonITEMSGetType(hItem) == 6)
		ptInfo->field_0c = TRUE;
	if (D2CommonITEMSGetType(hItem) == 2)
		ptInfo->field_10 = TRUE;
	if (D2CommonITEMSGetTypeTwoHanded(hItem))
		ptInfo->field_18 = TRUE;
	if (D2CommonITEMSIsWeapon(hItem))
		ptInfo->field_14 = TRUE;
	if (D2CommonITEMSGetTypeThrowable(hItem) || D2CommonStatsGetTotalValue(hItem, 125))
		ptInfo->field_20 = TRUE;
	if (D2CommonITEMSGetType(hItem) == 10)
		ptInfo->field_24 = TRUE;
}

BOOL __fastcall ITEMSModeCanHoldTogether(ITEMHANDINFO *ptA, ITEMHANDINFO *ptB)
{
	if ((ptA->field_00 && ptB->field_08) || (ptA->field_08 && ptB->field_00))
		return TRUE;
	if ((ptA->field_04 && ptB->field_0c) || (ptA->field_0c && ptB->field_04))
		return TRUE;
	if ((ptA->field_14 && !ptA->field_18 && ptB->field_10) || (ptB->field_14 && !ptB->field_18
		&& ptA->field_10))
		return TRUE;
	if (ptA->field_14 && !ptA->field_18 && ptB->field_14 && !ptB->field_18 && (ptA->field_1c
		|| ptB->field_1c))
		return TRUE;
	if (ptA->field_24 && ptB->field_24)
		return TRUE;
	return FALSE;
}

BOOL __fastcall ITEMSModePickupToInventory(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	int *pnResult)
{
	void *hInventory;
	UNIT *hItem;
	GAMEROOM *hRoom;

#line 1210
	D2ASSERT(ptGame);
#line 1211
	D2ASSERT(hUnit);
	*pnResult = 0;
	hInventory = hUnit->ptInventory;
#line 1216
	D2ASSERT(hInventory);
	if (D2CommonINVENTORYGetCursorItem(hInventory))
		return FALSE;
	if (PlayerIsBusy(hUnit))
		return FALSE;
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (hItem->dwMode != 3)
	{
		*pnResult = 1;
		return FALSE;
	}
	ITEMSModeClearMarkedItems(hUnit);
	if (!ITEMSModeCanTakeQuestItem(ptGame, hUnit, hItem))
	{
		D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hItem), hItem->eType, hItem->dwId);
		D2CommonITEMSDataSetPage(hItem, 255);
		hItem->dwFlags0 |= 4096;
		D2CommonUnitSetMode(hItem, 3);
		SUnitSetEvent(hUnit, 18, hUnit);
		return FALSE;
	}
	SUnitSetEvent(hUnit, 1, hUnit);
	if (D2CommonITEMSGetType(hItem) == ITEMTYPE_GOLD)
	{
		ITEMSModePickupGold(ptGame, hUnit, hItem);
		return TRUE;
	}
	if (ITEMSModeStackOntoInventory(ptGame, hUnit, hItem))
		return FALSE;
	if (ITEMSModeFindBodyLoc(hUnit, hItem, (int *)&dwItemId))
	{
		if (D2CommonINVENTORYTestBodyLocPlace(hUnit, dwItemId, hItem, 0) != 1)
		{
			*pnResult = 1;
			return FALSE;
		}
		hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hItem);
		if (hRoom)
		{
			D2CommonDungeonRoomAddNode(hRoom, hItem->eType, hItem->dwId);
			D2CommonUnitClearCollision(hItem);
			D2CommonUnitRoomRemoveUnit(hItem);
		}
		if (!ITEMSModeEquipFromGround(ptGame, hUnit, hItem))
			return FALSE;
		QuestsItemAcquired(ptGame, hUnit, hItem);
		return TRUE;
	}
	if (D2CommonITEMSGetTypeBelt(hItem) && D2CommonITEMSTestAutobeltOrFits(hInventory, hItem)
		&& D2CommonINVENTORYAddBeltItem(hInventory, hItem))
	{
		if (D2CommonUnitGetPathRoom(hItem))
		{
			D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hItem), hItem->eType, hItem->dwId);
			D2CommonUnitClearCollision(hItem);
			D2CommonUnitRoomRemoveUnit(hItem);
		}
		if (D2CommonINVENTORYAddItem(hInventory, hItem, 2))
		{
			D2CommonINVENTORYSetCursorItem(hInventory, 0);
			if (hUnit->eType == UNIT_PLAYER)
				ITEMSModeUpdateBookSkill(hUnit, hItem, TRUE);
			hItem->dwFlags0 &= ~2;
			D2CommonUnitSetMode(hItem, ITEM_MODE_INVBELT);
			D2CommonITEMSDataSetCmdFlags(hItem, 8192, 1);
			hItem->dwFlags0 &= ~33554432;
			(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
			D2CommonUnitSetChanged(hUnit, 1);
			return TRUE;
		}
#line 1305
		D2ASSERT(0);
	}
	if (ITEMSModePlaceInGrid(ptGame, hUnit, hItem, TRUE, 0))
		return TRUE;
	D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hItem), hItem->eType, hItem->dwId);
	D2CommonITEMSDataSetPage(hItem, 255);
	hItem->dwFlags0 |= 4096;
	D2CommonUnitSetMode(hItem, 3);
	SUnitSetEvent(hUnit, 22, hUnit);
	return FALSE;
}

static inline BOOL ITEMSModeStackTome(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	void *hInventory;
	UNIT *hDest;
	int nSrcValue;
	int nDstValue;
	int nDstMaxValue;
	int nQuantity;
	GAMEROOM *hRoom;

	hInventory = hUnit->ptInventory;
	hDest = (UNIT *)D2CommonINVENTORYFindGridBook(hInventory, hItem, 0);
	if (!hDest)
		return FALSE;
	nSrcValue = D2CommonStatsUnitGetValue(hItem, STAT_QUANTITY);
#line 894
	D2ASSERT(nSrcValue >= 0);
	nDstValue = D2CommonStatsUnitGetValue(hDest, STAT_QUANTITY);
#line 896
	D2ASSERT(nDstValue >= 0);
	nDstMaxValue = D2CommonITEMSGetTypeMaxStack(hDest);
#line 898
	D2ASSERT(nDstMaxValue >= 0);
	if (nDstValue + nSrcValue > nDstMaxValue)
	{
		D2CommonStatsUnitSetValue(hDest, STAT_QUANTITY, nDstMaxValue);
		SCmdSendItemStat(SUnitGetClient(hUnit), hDest, 1, STAT_QUANTITY, nDstMaxValue);
		nQuantity = nDstValue - nDstMaxValue + nSrcValue;
#line 906
		D2ASSERT(nQuantity >= 0);
		D2CommonStatsUnitSetValue(hItem, STAT_QUANTITY, nQuantity);
		SCmdSendItemStat(SUnitGetClient(hUnit), hItem, 1, STAT_QUANTITY, nQuantity);
		ITEMSModeAddBookCharges(hUnit, hDest, nDstMaxValue - nDstValue);
		return TRUE;
	}
	nQuantity = nDstValue + nSrcValue;
#line 917
	D2ASSERT(nQuantity >= 0);
	D2CommonStatsUnitSetValue(hDest, STAT_QUANTITY, nQuantity);
	SCmdSendItemStat(SUnitGetClient(hUnit), hDest, 1, STAT_QUANTITY, nQuantity);
	ITEMSModeAddBookCharges(hUnit, hDest, nSrcValue);
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hItem);
	if (hRoom)
	{
		D2CommonDungeonRoomAddNode(hRoom, hItem->eType, hItem->dwId);
		D2CommonUnitClearCollision(hItem);
		D2CommonUnitRoomRemoveUnit(hItem);
	}
	hItem->dwFlags0 &= ~2;
	ITEMSFree(ptGame, hItem);
	D2CommonINVENTORYSetCursorItem(hInventory, 0);
	return TRUE;
}

BOOL __fastcall ITEMSModeStackOntoInventory(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	DWORD nType;
	UNIT *hDest;
	DWORD i;

	nType = D2CommonITEMSGetType(hItem);
#line 946
	D2ASSERT(ptGame);
	if (nType == 22)
	{ int nResult;
		hDest = (UNIT *)D2CommonINVENTORYFindGridBook(hUnit->ptInventory, hItem, 0);
		if (!hDest)
			return FALSE;
		return ITEMSModeScrollToTome(ptGame, hUnit, hItem->dwId, hDest->dwId, &nResult);
	}
	if (nType == 18)
		return ITEMSModeStackTome(ptGame, hUnit, hItem);
	if (D2CommonITEMSGetTypeStackable(hItem))
		for (i = 0; i < sgnStackTypes; i++)
			if (nType == sgadwStackTypes[i])
				return ITEMSModeStackItem(ptGame, hUnit, hItem);
	return FALSE;
}

void __fastcall ITEMSModeAddBookCharges(UNIT *hUnit, UNIT *hItem, int nCharges)
{
	int nSkill;
	void *hSkill;
	int nCount;

#line 139
	D2ASSERT(hUnit);
#line 140
	D2ASSERT(hItem);
	nSkill = ITEMSModeGetBookSkill(hItem);
	if (nSkill == -1)
		return;
	hSkill = (void *)D2CommonSkillsFindByKey(hUnit, nSkill);
	if (!hSkill)
		return;
	nCount = D2CommonSkillsGetNodeCount(hSkill);
	nCount += nCharges;
	D2CommonSkillsSetNodeCount(hSkill, nCount);
	SCmdSendSkillState(SUnitGetClient(hUnit), (BYTE)hUnit->eType, hUnit->dwId, (WORD)nSkill,
		(BYTE)nCount);
}

BOOL __fastcall ITEMSModeStackItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	void *hInventory;
	UNIT *hDest;
	int nTries;
	DWORD nType;
	DWORD i;
	int nSrcValue;
	int nDstValue;
	int nMaxStack;
	int nQuantity;
	GAMEROOM *hRoom;
	BOOL fStacked;

	hInventory = hUnit->ptInventory;
	hDest = NULL;
	nTries = 0;
	fStacked = FALSE;
	while (D2CommonStatsUnitGetValue(hItem, STAT_QUANTITY) > 0)
	{
		nTries++;
		nType = D2CommonITEMSGetType(hItem);
		for (i = 0; i < sgnStackTypes; i++)
		{
			if (nType != sgadwStackTypes[i])
				continue;
			if (nType == 5 || nType == 6)
			{
				hDest = (UNIT *)D2CommonINVENTORYFindEquippedStack(hInventory, sgadwStackTypes[i], hDest);
				if (!hDest)
					hDest = (UNIT *)D2CommonINVENTORYFindGridStack(hInventory, sgadwStackTypes[i],
						hDest);
			}
			else
				hDest = (UNIT *)D2CommonINVENTORYFindGridStack(hInventory, sgadwStackTypes[i], hDest);
			break;
		}
		if (!hDest)
			return FALSE;
		if (!D2CommonITEMSTestCanStack(hItem, hDest))
			continue;
		nSrcValue = D2CommonStatsUnitGetValue(hItem, STAT_QUANTITY);
		nDstValue = D2CommonStatsUnitGetValue(hDest, STAT_QUANTITY);
		if (nDstValue + nSrcValue > (int)D2CommonITEMSGetTypeMaxStack(hDest))
		{
			nMaxStack = D2CommonITEMSGetTypeMaxStack(hDest);
#line 809
			D2ASSERT(nMaxStack >= 0);
			D2CommonStatsUnitSetValue(hDest, STAT_QUANTITY, nMaxStack);
			SCmdSendItemStat(SUnitGetClient(hUnit), hDest, 1, STAT_QUANTITY, nMaxStack);
			nQuantity = nDstValue - nMaxStack + nSrcValue;
#line 814
			D2ASSERT(nQuantity >= 0);
			D2CommonStatsUnitSetValue(hItem, STAT_QUANTITY, nQuantity);
			if (D2CommonITEMSGetType(hItem) == 18 && D2CommonITEMSGetType(hDest) == 18)
			{
				nMaxStack = D2CommonITEMSGetTypeMaxStack(hDest);
				ITEMSModeAddBookCharges(hUnit, hDest, nMaxStack - nDstValue);
			}
			continue;
		}
		nQuantity = nDstValue + nSrcValue;
#line 825
		D2ASSERT(nQuantity >= 0);
		D2CommonStatsUnitSetValue(hDest, STAT_QUANTITY, nQuantity);
		D2CommonStatsUnitSetValue(hItem, STAT_QUANTITY, 0);
		SCmdSendItemStat(SUnitGetClient(hUnit), hDest, 1, STAT_QUANTITY, nQuantity);
		if (D2CommonITEMSGetType(hItem) == 18 && D2CommonITEMSGetType(hDest) == 18)
			ITEMSModeAddBookCharges(hUnit, hDest, nSrcValue);
		hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hItem);
		if (hRoom)
		{
			D2CommonDungeonRoomAddNode(hRoom, hItem->eType, hItem->dwId);
			D2CommonUnitClearCollision(hItem);
			D2CommonUnitRoomRemoveUnit(hItem);
		}
		hItem->dwFlags0 &= ~2;
		ITEMSFree(ptGame, hItem);
		D2CommonINVENTORYSetCursorItem(hInventory, 0);
		fStacked = TRUE;
		break;
	}
	if (!fStacked)
	{
		if (!nTries)
			return FALSE;
#line 854
		D2ASSERT(0);
	}
	return fStacked;
}

extern void ITEMSModeAddItemSkill(UNIT *hUnit, UNIT *hItem);
__inline void ITEMSModeAddItemSkill(UNIT *hUnit, UNIT *hItem)
{
#line 200
	D2ASSERT(hUnit);
#line 201
	D2ASSERT(hItem);
	if (hUnit->eType == UNIT_PLAYER)
		ITEMSModeUpdateBookSkill(hUnit, hItem, TRUE);
}

BOOL __fastcall ITEMSModePlaceInGrid(GAME *ptGame, UNIT *hUnit, UNIT *hItem, BOOL fFromGround,
	DWORD dwPage)
{
	void *hInventory;
	GAMEROOM *hRoom;

#line 1166
	D2ASSERT(ptGame);
	hInventory = hUnit->ptInventory;
#line 1169
	D2ASSERT(hInventory);
	if (D2CommonINVENTORYPlaceItem(hInventory, hItem, D2CommonUnitGetInvRecordIndex(hUnit, dwPage), 0,
		0))
	{
	if (fFromGround)
	{
		hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hItem);
		if (hRoom)
		{
			D2CommonDungeonRoomAddNode(hRoom, hItem->eType, hItem->dwId);
			D2CommonUnitClearCollision(hItem);
			D2CommonUnitRoomRemoveUnit(hItem);
		}
	}
	if (D2CommonINVENTORYAddItem(hInventory, hItem, 1))
	{
		D2CommonINVENTORYSetCursorItem(hInventory, 0);
		ITEMSModeAddItemSkill(hUnit, hItem);
		hItem->dwFlags0 &= ~2;
		D2CommonUnitSetMode(hItem, 0);
		D2CommonITEMSDataSetCmdFlags(hItem, 128, 1);
		(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
		D2CommonUnitSetChanged(hUnit, 1);
		hItem->dwFlags0 &= ~33554432;
		D2CommonITEMSDataSetPage(hItem, dwPage);
		QuestsItemAcquired(ptGame, hUnit, hItem);
		return TRUE;
	}
#line 1199
	D2ASSERT(0);
	}
	return FALSE;
}

static inline void ITEMSModeDropCubeContents(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	GAMEITEMSTXT *ptItemStats;
	void *hInventory;
	void *ptNode;
	void *ptNext;
	UNIT *hInvItem;
	UNIT *hRemove;
	DWORD dwFlags;
	BYTE bPage;
	DWORD dwRecord;
	GAMEROOM *hRoom;
	COLPOINT tFrom;
	COLPOINT tTo;

	ptItemStats = (GAMEITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
#line 1323
	D2ASSERT(ptItemStats);
	if (ptItemStats->code != 544763746)
		return;
	hInventory = hUnit->ptInventory;
	ptNode = D2CommonINVENTORYGetCategoryListHead(hInventory);
	while (ptNode)
	{
		hInvItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
		ptNext = (void *)D2CommonINVENTORYNodeGetNext(ptNode);
		if (D2CommonITEMSDataGetPage(hInvItem) == 3)
		{
			dwFlags = 0;
			D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
			D2CommonITEMSDataSetPreviousPage(hItem, 3);
			SCmdSendItemFromGrid(SUnitGetClient(hUnit), hUnit, hInvItem, dwFlags);
			hRemove = (UNIT *)D2CommonINVENTORYRemoveItem(hInventory, hInvItem);
#line 1338
			D2ASSERT(hRemove);
#line 1339
			D2ASSERT(hInvItem == hRemove);
			bPage = D2CommonITEMSDataGetPage(hInvItem);
			dwRecord = D2CommonUnitGetInvRecordIndex(hUnit, bPage);
			D2CommonINVENTORYRemoveItemAt(hInventory, D2CommonUnitGetX(hInvItem), D2CommonUnitGetY(hInvItem), dwRecord, 0, bPage);
			D2CommonUnitSetMode(hRemove, 4);
			D2CommonITEMSDataSetPage(hInvItem, 0);
			if (!ITEMSModePlaceCursorItem(ptGame, hUnit, hInvItem->dwId, 0, 0, TRUE, TRUE, NULL))
			{
				D2CommonITEMSDataSetPage(hInvItem, 255);
				D2CommonUnitGetPosition(hUnit, &tFrom);
				hRoom = ITEMSFindDropSpot((GAMEROOM *)D2CommonUnitGetPathRoom(hUnit), &tFrom, &tTo, 1);
				if (!hRoom)
				{
#line 1357
					D2WARN("lost item, oh well");
					continue;
				}
				D2CommonUnitInitPosition(0, hInvItem, hRoom, tTo.nX, tTo.nY);
				D2CommonUnitSetCollision(hInvItem, hRoom, tTo.nX, tTo.nY);
				D2CommonAddUnitToRoom(hInvItem, hRoom);
				hInvItem->dwFlags0 |= 4098;
				D2CommonUnitSetMode(hInvItem, 3);
				QuestsItemDropped(ptGame, hUnit, hInvItem);
			}
		}
		ptNode = ptNext;
	}
}

void __fastcall ITEMSModeDropCursorItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId)
{
	UNIT *hItem;
	GAMEROOM *hRoom;
	COLPOINT tFrom;
	COLPOINT tTo;

#line 1379
	D2ASSERT(ptGame);
#line 1380
	D2ASSERT(hUnit);
	ITEMSModeClearMarkedItems(hUnit);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem || hItem->dwMode != 4)
		return;
	D2CommonUnitGetPosition(hUnit, &tFrom);
	hRoom = ITEMSFindDropSpot((GAMEROOM *)D2CommonUnitGetPathRoom(hUnit), &tFrom, &tTo, 1);
	if (!hRoom)
	{
#line 1403
		D2WARN("lost item");
		return;
	}
	D2CommonUnitInitPosition(0, hItem, hRoom, tTo.nX, tTo.nY);
	D2CommonUnitSetCollision(hItem, hRoom, tTo.nX, tTo.nY);
	D2CommonAddUnitToRoom(hItem, hRoom);
	hItem->dwFlags0 |= 2;
	D2CommonINVENTORYSetCursorItem(hUnit->ptInventory, 0);
	if (D2CommonITEMSGetTypeHasInv(hItem) && (DWORD)D2CommonINVENTORYGetCategoryListCount(ITEMSModeGetItemInventory(hItem)) > 0)
		D2CommonITEMSDataSetFlags(hItem, 1, 1);
	if (D2CommonITEMSDataTestFlags(hItem, 16384))
		D2CommonITEMSDataSetFlags(hItem, 16384, 0);
	hItem->dwFlags0 |= 4096;
	D2CommonUnitSetMode(hItem, 3);
	hItem->dwFlags0 |= 33554432;
	D2CommonITEMSDataSetPage(hItem, 255);
	QuestsItemDropped(ptGame, hUnit, hItem);
	ITEMSModeDropCubeContents(ptGame, hUnit, hItem);
}

BOOL __fastcall ITEMSModePlaceCursorItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	int nX, int nY, BOOL fAnywhere,
		BOOL fSend,
	void *hOtherInventory)
{
	UNIT *hItem;
	BYTE bPage;
	void *hInventory;
	DWORD dwRecord;
	BOOL fClearCursor;
	BOOL fTrade;

#line 1432
	D2ASSERT(ptGame);
#line 1433
	D2ASSERT(hUnit);
	ITEMSModeClearMarkedItems(hUnit);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem || hItem->dwMode != 4)
		return FALSE;
	bPage = D2CommonITEMSDataGetPage(hItem);
	hInventory = hUnit->ptInventory;
	if (hOtherInventory)
		hInventory = hOtherInventory;
	fTrade = FALSE;
	fClearCursor = TRUE;
	dwRecord = D2CommonUnitGetInvRecordIndex(hUnit, bPage);
	if (hUnit->eType == UNIT_PLAYER)
	{
		switch (bPage)
		{
		case 1:
			fClearCursor = FALSE;
			break;
		case 2:
			fTrade = TRUE;
			break;
		}
	}
	if (fAnywhere)
	{
		if (!D2CommonINVENTORYPlaceItem(hInventory, hItem, dwRecord, 0, bPage))
			goto fail;
	}
	else
	{
		if (nX < 0)
			nX = 0;
		if (nY < 0)
			nY = 0;
		if (!D2CommonINVENTORYPlaceItemAt(hInventory, hItem, nX, nY, dwRecord, 0, bPage))
			return FALSE;
	}
	if (!D2CommonINVENTORYAddItem(hInventory, hItem, 1))
		goto fail;
	if (D2CommonITEMSDataGetPage(hItem) != 4 && hUnit->eType == UNIT_PLAYER)
		ITEMSModeUpdateBookSkill(hUnit, hItem, TRUE);
	hItem->dwFlags0 &= ~2;
	if (fClearCursor)
		D2CommonINVENTORYSetCursorItem(hInventory, 0);
	D2CommonUnitSetMode(hItem, 0);
	if (fSend)
	{
		D2CommonITEMSDataSetCmdFlags(hItem, 2, 1);
		if (D2CommonITEMSGetTypeHasInv(hItem) && (DWORD)D2CommonINVENTORYGetCategoryListCount(ITEMSModeGetItemInventory(hItem)) > 0)
			D2CommonITEMSDataSetFlags(hItem, 1, 1);
		if (D2CommonITEMSDataTestFlags(hItem, 16384))
			D2CommonITEMSDataSetFlags(hItem, 16384, 0);
		D2CommonUnitSetChanged(hUnit, 1);
		(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
	}
	if (fTrade == TRUE)
		PlrTradeAddItem(ptGame, hUnit, hItem);
	return TRUE;
	fail: return FALSE;
}

BOOL __fastcall ITEMSModeTakeToCursor(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	int *pnResult, int nSend, BOOL fFlag20,
	void *hOtherInventory, BOOL fForce)
{
	void *hInventory;
	UNIT *hItem;
	UNIT *hRemove;
	BYTE bPage;
	DWORD dwRecord;
	DWORD dwX;
	DWORD dwY;
	BOOL fClearCursor;
	BOOL fTrade;

#line 1548
	D2ASSERT(ptGame);
#line 1549
	D2ASSERT(hUnit);
	*pnResult = 0;
	hInventory = hUnit->ptInventory;
	if (hOtherInventory)
		hInventory = hOtherInventory;
#line 1558
	D2ASSERT(hInventory);
	if (D2CommonINVENTORYGetCursorItem(hInventory) && !fForce)
		return FALSE;
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (hUnit->eType == UNIT_PLAYER && !PlayerIsBusy(hUnit) && D2CommonITEMSDataGetPage(hItem))
	{
#line 1578
		D2ASSERT(0);
	}
	if (hItem->dwMode != 0)
	{
		*pnResult = 1;
		return FALSE;
	}
	ITEMSModeClearMarkedItems(hUnit);
	hItem->dwFlags0 &= ~2;
	hRemove = (UNIT *)D2CommonINVENTORYRemoveItem(hInventory, hItem);
#line 1592
	D2ASSERT(hRemove);
#line 1593
	D2ASSERT(hItem == hRemove);
	if (hUnit->eType == UNIT_PLAYER)
		ITEMSModeUpdateBookSkill(hUnit, hItem, FALSE);
	bPage = D2CommonITEMSDataGetPage(hItem);
	dwRecord = D2CommonUnitGetInvRecordIndex(hUnit, bPage);
	fClearCursor = TRUE;
	fTrade = FALSE;
	if (hUnit->eType == UNIT_PLAYER)
	{
		switch (bPage)
		{
		case 1:
			fClearCursor = FALSE;
			break;
		case 2:
			fTrade = TRUE;
			break;
		}
	}
	else
		fClearCursor = FALSE;

	D2CommonINVENTORYRemoveItemAt(hInventory, D2CommonUnitGetX(hItem), D2CommonUnitGetY(hItem), dwRecord, 0, bPage);
	D2CommonITEMSDataSetPreviousPage(hItem, bPage);
	D2CommonITEMSDataSetPage(hItem, 255);
	if (fClearCursor)
		D2CommonINVENTORYSetCursorItem(hInventory, hItem);
	D2CommonUnitSetMode(hItem, 4);
	if (nSend == 1)
	{
		if (fFlag20)
			D2CommonITEMSDataSetFlags(hItem, 32, 1);
		if (fTrade == TRUE)
			PlrTradeRemoveItem(ptGame, hUnit, hItem);
		D2CommonITEMSDataSetCmdFlags(hItem, 4, 1);
		if (D2CommonITEMSGetTypeHasInv(hItem) && (DWORD)D2CommonINVENTORYGetCategoryListCount(ITEMSModeGetItemInventory(hItem)) > 0)
			D2CommonITEMSDataSetFlags(hItem, 1, 1);
		if (D2CommonITEMSDataTestFlags(hItem, 16384))
			D2CommonITEMSDataSetFlags(hItem, 16384, 0);
		(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
		D2CommonUnitSetChanged(hUnit, 1);
	}
	return TRUE;
}

void __fastcall ITEMSModeRemoveSetBonus(UNIT *hUnit, UNIT *hItem)
{
	void *ptList;
	GAME *ptGame;

	if (D2CommonITEMSTestSetComplete(hUnit, hItem))
		return;
	ptList = (void *)D2CommonStatsFindListByMask(hUnit, 4096);
	if (!ptList)
		return;
	ptGame = SUnitGetGame(hUnit);
	SkillsItemRemovePassiveMods(ptGame, hUnit, hUnit, 4);
	D2CommonStatsUnitSetActiveStateBit(hUnit, 97, 0);
	D2CommonStatsUnitUnlinkList(hUnit, ptList);
	D2CommonStatsListFree(ptList);
}

void __fastcall ITEMSModeCheckEquipment(GAME *ptGame, UNIT *hUnit, BOOL fSend)
{
	int nChanges;
	void *hInventory;
	void *ptNode;
	UNIT *hItem;
	DWORD fUsable;
	DWORD i;
	BOOL fRestart;

	if (hUnit->eType != UNIT_PLAYER)
		return;
	nChanges = 0;
	hInventory = hUnit->ptInventory;
	do
	{
		fRestart = FALSE;
		for (ptNode = D2CommonINVENTORYGetCategoryListHead(hInventory); ptNode;
			ptNode = (void *)D2CommonINVENTORYNodeGetNext(ptNode))
		{
			if (D2CommonINVENTORYNodeGetCategory(ptNode) == 3)
			{
				hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
				fUsable = D2CommonITEMSTestFitsInventorySpace(hItem, hUnit, 0, 0, 0);
				if (!D2CommonITEMSDataTestFlags(hItem, 16384) && !D2CommonITEMSDataTestFlags(hItem, 256))
				{
					if (!fUsable)
					{
						D2CommonITEMSDataSetFlags(hItem, 16384, 1);
						D2CommonINVENTORYOfferRemoveDelta(hUnit->ptInventory, hItem);
						if (D2CommonUnitGetAnimMode(hItem) == 1)
							ITEMSModeRemoveEffects(ptGame, hItem, hUnit, TRUE, TRUE);
						nChanges++;
						fRestart = TRUE;
						break;
					}
				}
				else if (D2CommonITEMSDataTestFlags(hItem, 16384) && fUsable)
				{
					D2CommonITEMSDataSetFlags(hItem, 16384, 0);
					D2CommonINVENTORYOfferAddDelta(hUnit->ptInventory, hItem);
					if (D2CommonUnitGetAnimMode(hItem) == 1)
						ITEMSModeApplyEffects(ptGame, hItem, hUnit, TRUE);
					nChanges--;
				}
			}
		}
	} while (fRestart);
	if (fSend)
		SCmdSendEquipmentChecked(SUnitGetClient(hUnit), hUnit);
	if (nChanges)
	{
		void *hItemInventory = hUnit->ptInventory;
		void *ptItemNode;

		for (ptItemNode = D2CommonINVENTORYGetCategoryListHead(hItemInventory); ptItemNode;
			ptItemNode = (void *)D2CommonINVENTORYNodeGetNext(ptItemNode))
		{
			hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptItemNode);
			if (!hItem)
				continue;
			for (i = 0; i < sgnRefreshCategories; i++)
			{
				if (sgadwRefreshCategories[i] == D2CommonITEMSGetCategory(hItem))
				{
					if (D2CommonINVENTORYNodeGetCategory(ptItemNode) == 3
						&& !D2CommonINVENTORYTestHasDeltaKey(hItemInventory, hItem->dwId))
					{
						D2CommonITEMSDataSetFlags(hItem, 262144, 1);
						D2CommonITEMSDataSetFlags(hItem, 1, 1);
						(void)D2CommonINVENTORYAddDeltaNode(hItemInventory, hItem);
						D2CommonUnitSetChanged(hUnit, 1);
					}
					break;
				}
			}
		}
	}
}

void __fastcall ITEMSModeApplyEffects(GAME *ptGame, UNIT *hItem, UNIT *hUnit, BOOL fForce)
{
	GAMEITEMSTXT *ptItemStats;
	void *ptGemMods;
	DWORD nType;
	DWORD nMods;
	UNIT *hOther;
	BOOL fDyeMode;
	DWORD eUnitType;

	if (!hUnit)
		return;
	if (D2CommonITEMSGetType(hItem) == ITEMTYPE_GEM)
	{
#line 219
		D2ASSERT(UnitGetType(hUnit) == UNIT_ITEM);
		ptItemStats = (GAMEITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
#line 221
		D2ASSERT(ptItemStats);
		ptGemMods = (void *)D2CommonItemTblsGetGem(ptItemStats->gemoffset);
#line 223
		D2ASSERT(ptGemMods);
		nType = D2CommonITEMSGetType(hUnit);
		if (D2CommonITEMSIsWeapon(hUnit))
			nMods = 0;
		else if (nType == 37)
			nMods = 1;
		else if (nType == 2)
			nMods = 2;
		else
		{
#line 234
			D2ASSERT(0);
		}
		D2CommonITEMSModsApply(2, hItem, ptGemMods, nMods, 0);
	}
	ITEMSModeAddItemSkill(hUnit, hItem);
	eUnitType = hUnit->eType;
	hOther = NULL;
	if (eUnitType == UNIT_PLAYER)
		hOther = (UNIT *)D2CommonINVENTORYFindEquippedMatch(ITEMSModeGetItemInventory(hUnit));
	fDyeMode = TRUE;
	if (eUnitType == UNIT_PLAYER && hUnit->dwClassId == 4)
	{
		if (hItem != hOther && D2CommonITEMSIsWeapon(hItem))
			fDyeMode = FALSE;
		if (D2CommonITEMSDataTestFlags(hItem, 256) && !fForce)
		{
			D2CommonUnitUpdateAnimRate(hUnit);
			return;
		}
	}
	else if (D2CommonITEMSDataTestFlags(hItem, 256) && !fForce)
		goto lDone;
	D2CommonStatsUnitApplyDye(hUnit, hItem, fDyeMode);
	SkillsItemApplyPassiveMods(ptGame, hUnit, hItem, 4, NULL, 0, FALSE);
lDone:
	D2CommonUnitUpdateAnimRate(hUnit);
}

BOOL __fastcall ITEMSModeEquipCursorItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	BYTE bLoc, BOOL fNoFitTest, int *pnResult)
{
	UNIT *hItem;
	int nPlace;
	void *hInventory;
	UNIT *hOther;

#line 1851
	D2ASSERT(ptGame);
#line 1852
	D2ASSERT(hUnit);
	*pnResult = 0;
	ITEMSModeClearMarkedItems(hUnit);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (hItem->dwMode != 4)
	{
		*pnResult = 1;
		return FALSE;
	}
	nPlace = D2CommonINVENTORYTestBodyLocPlace(hUnit, bLoc, hItem, fNoFitTest);
	if (nPlace != 1 && nPlace != 2)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (!fNoFitTest && !D2CommonITEMSTestFitsInventorySpace(hItem, hUnit, 0, 0, 0))
	{
		*pnResult = 1;
		return FALSE;
	}
	hInventory = hUnit->ptInventory;
	hOther = (UNIT *)D2CommonINVENTORYFindEquippedMatch(hInventory);
	if (hOther)
		D2CommonUnitSetPlayerWeaponId(hUnit, hOther);
	if (!D2CommonINVENTORYSetLocSlot(hInventory, hItem, bLoc))
		return TRUE;
	if (D2CommonINVENTORYAddItem(hInventory, hItem, 3))
	{
		D2CommonITEMSSetCategory(hItem, bLoc);
		D2CommonINVENTORYOfferAddDelta(hInventory, hItem);
		D2CommonINVENTORYSetCursorItem(hInventory, 0);
		ITEMSModeApplyEffects(ptGame, hItem, hUnit, FALSE);
		ITEMSModeRefreshItems(ptGame, hUnit, 0);
		hItem->dwFlags0 &= ~2;
		D2CommonUnitSetMode(hItem, 1);
		D2CommonITEMSDataSetPage(hItem, 255);
		D2CommonITEMSDataSetCmdFlags(hItem, 8, 1);
		D2CommonITEMSDataSetFlags(hItem, 1, 1);
		if (D2CommonITEMSDataTestFlags(hItem, 16384))
			D2CommonITEMSDataSetFlags(hItem, 16384, 0);
		(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
		D2CommonUnitSetChanged(hUnit, 1);
		ITEMSModeUpdateWeaponSkills(hUnit);
		ITEMSModeApplySetBonus(ptGame, hUnit, hItem);
		ITEMSModeCheckEquipment(ptGame, hUnit, FALSE);
		return TRUE;
	}
#line 1927
	D2ASSERT(0);
	return TRUE;
}

void __fastcall ITEMSModeUpdateWeaponSkills(UNIT *hUnit)
{
	void *hInventory;
	UNIT *hWeapon;
	UNIT *hOther;
	UNIT *hLeftHand;
	void *hSkillRight;
	void *hSkillLeft;
	BOOL fLeftSet;

	if (hUnit->eType != UNIT_PLAYER)
		return;
	hInventory = hUnit->ptInventory;
	hWeapon = (UNIT *)D2CommonINVENTORYFindEquippedMatch(hInventory);
	hOther = (UNIT *)D2CommonINVENTORYGetLocSlot(hInventory, 4);
	hLeftHand = (UNIT *)D2CommonINVENTORYGetLocSlot(hInventory, 5);
	if (hWeapon == hOther)
		hOther = hLeftHand;
	if (hOther && !D2CommonITEMSIsWeapon(hOther))
		hOther = NULL;
	hSkillRight = (void *)D2CommonUnitGetRightSkill(hUnit);
	hSkillLeft = (void *)D2CommonUnitGetLeftSkill(hUnit);
	if (!hWeapon)
		return;
	fLeftSet = FALSE;
	if (hOther && D2CommonITEMSGetType(hOther) == 38)
	{
		if (D2CommonSkillsGetNodeType(hSkillRight) != 4 && D2CommonSkillsGetNodeType(hSkillRight) != 140)
		{
			D2CommonUnitSyncPlayerRightSkill(hUnit);
			SkillsSetCurrent(hUnit, FALSE, SKILL_LEFT_HAND_THROW);
			hSkillRight = (void *)D2CommonUnitGetRightSkill(hUnit);
		}
		fLeftSet = TRUE;
	}
	if (D2CommonITEMSGetType(hWeapon) == 38)
	{
		if (D2CommonSkillsGetNodeType(hSkillLeft) != 2)
		{
			D2CommonUnitSyncPlayerLeftSkill(hUnit);
			SkillsSetCurrent(hUnit, TRUE, SKILL_THROW);
			D2CommonUnitGetLeftSkill(hUnit);
		}
	}
	else
		ITEMSModeCheckSkill(hUnit, TRUE, hSkillLeft);
	if (!fLeftSet)
		ITEMSModeCheckSkill(hUnit, FALSE, hSkillRight);
}

void __fastcall ITEMSModeCheckSkill(UNIT *hUnit, BOOL fLeft, void *hSkillCurr)
{
	int nSkill;
	void *hSkill;

#line 312
	D2ASSERT(hUnit);
#line 313
	D2ASSERT(hSkillCurr);
	if (fLeft)
		nSkill = D2CommonUnitGetPlayerLeftSkill(hUnit);
	else
		nSkill = D2CommonUnitGetPlayerRightSkill(hUnit);
	hSkill = (void *)D2CommonSkillsFindByKey(hUnit, nSkill);
	if (hSkill && hSkill != hSkillCurr)
	{
		switch ((DWORD)D2CommonSkillsTestUsable(hUnit, hSkill))
		{
		case 2:
		case 7:
			break;
		default:
			SkillsSetCurrent(hUnit, fLeft, nSkill);
			return;
		}
	}
	switch ((DWORD)D2CommonSkillsTestUsable(hUnit, hSkillCurr))
	{
	case 2:
	case 7:
		SkillsSetCurrent(hUnit, fLeft, SKILL_ATTACK);
		break;
	}
}

void __fastcall ITEMSModeApplySetBonus(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	void *ptList;
	void *ptSetMods;

	if (!D2CommonITEMSTestSetComplete(hUnit, hItem))
		return;
	if (D2CommonStatsFindListByState(hUnit, 97))
		return;
	ptList = (void *)D2CommonStatsAllocList(ptGame->pMemPool, 4096, 0, hUnit->eType, hUnit->dwId);
	if (ptList)
	{
		D2CommonStatsSetListState(ptList, 97);
		D2CommonStatsUnitLinkList(hUnit, ptList);
		D2CommonStatsUnitSetActiveStateBit(hUnit, 97, 1);
		D2CommonStatsListAddValue(ptList, 71, hItem->dwId);
		ptSetMods = (void *)D2CommonItemTblsGetSetItem(D2CommonITEMSDataGetUniqueOrSetRow(hItem));
#line 1670
		D2ASSERT(ptSetMods);
		D2CommonITEMSModsApplySetProperties(hUnit, hItem, ptSetMods);
		SkillsItemApplyPassiveMods(ptGame, hUnit, hUnit, 4, NULL, 0, FALSE);
	}
	SUnitSetEvent(hUnit, 10, hUnit);
}

BOOL __fastcall ITEMSModeSwapHands(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	BYTE bLoc, BOOL fNoFitTest, int *pnResult)
{
	UNIT *hItem;
	BYTE bOther;
	void *hInventory;
	UNIT *hOpposite;
	UNIT *hRemove;
	UNIT *hWeapon;
	DWORD dwLocIndex;
	BOOL fSetComplete;

#line 1940
	D2ASSERT(ptGame);
#line 1941
	D2ASSERT(hUnit);
	*pnResult = 0;
	ITEMSModeClearMarkedItems(hUnit);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (hItem->dwMode != 4)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (bLoc != 5 && bLoc != 4)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (bLoc == 5)
		bOther = 4;
	else
		bOther = 5;
	hInventory = hUnit->ptInventory;
	hOpposite = (UNIT *)D2CommonINVENTORYGetLocSlot(hInventory, bOther);
	if (!hOpposite)
	{
		*pnResult = 1;
		return FALSE;
	}
	D2CommonINVENTORYGetEquipField(hUnit, hInventory, &hOpposite, bOther);
	if (!hOpposite)
	{
		*pnResult = 1;
		return FALSE;
	}
	dwLocIndex = bLoc;
	if (D2CommonINVENTORYTestBodyLocPlace(hUnit, dwLocIndex, hItem, fNoFitTest) != 2)
	{
		*pnResult = 1;
		return FALSE;
	}
	ITEMSModeRemoveEffects(ptGame, hOpposite, hUnit, FALSE, TRUE);
	if (!fNoFitTest && !D2CommonITEMSTestFitsInventorySpace(hItem, hUnit, 0, 0, 0))
	{
		ITEMSModeApplyEffects(ptGame, hOpposite, hUnit, FALSE);
		SUnitSetEvent(hUnit, 19, NULL);
		return FALSE;
	}
	hWeapon = (UNIT *)D2CommonINVENTORYFindEquippedMatch(hInventory);
	if (hWeapon)
		D2CommonUnitSetPlayerWeaponId(hUnit, hWeapon);
	fSetComplete = D2CommonITEMSTestSetComplete(hUnit, hOpposite);
	D2CommonINVENTORYOfferRemoveDelta(hInventory, hOpposite);
	hRemove = (UNIT *)D2CommonINVENTORYRemoveItem(hInventory, hOpposite);
#line 2007
	D2ASSERT(hRemove);
#line 2008
	D2ASSERT(hOpposite == hRemove);
	D2CommonINVENTORYClearLocSlot(hInventory, bOther);
	if (D2CommonITEMSGetType(hRemove) == 19)
		ITEMSModeDropBeltOverflow(ptGame, hUnit, NULL);
	D2CommonINVENTORYSetCursorItem(hInventory, hOpposite);
	hOpposite->dwFlags0 &= ~2;
	D2CommonUnitSetMode(hOpposite, 4);
	if (fSetComplete)
		ITEMSModeRemoveSetBonus(hUnit, hOpposite);
	if (D2CommonINVENTORYSetLocSlot(hInventory, hItem, dwLocIndex))
	{
		if (D2CommonINVENTORYAddItem(hInventory, hItem, 3))
		{
			D2CommonITEMSSetCategory(hItem, bLoc);
			D2CommonINVENTORYOfferAddDelta(hInventory, hItem);
			ITEMSModeApplyEffects(ptGame, hItem, hUnit, FALSE);
			ITEMSModeRefreshItems(ptGame, hUnit, 0);
			hItem->dwFlags0 &= ~2;
			D2CommonUnitSetMode(hItem, 1);
			D2CommonITEMSDataSetPage(hItem, 255);
			D2CommonITEMSDataSetCmdFlags(hItem, 65536, 1);
			D2CommonITEMSDataSetFlags(hItem, 1, 1);
			if (D2CommonITEMSDataTestFlags(hItem, 16384))
				D2CommonITEMSDataSetFlags(hItem, 16384, 0);
			(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
			ITEMSModeUpdateWeaponSkills(hUnit);
			ITEMSModeApplySetBonus(ptGame, hUnit, hItem);
			ITEMSModeCheckEquipment(ptGame, hUnit, FALSE);
		}
		else
		{
#line 2060
			D2ASSERT(0);
		}
	}
	D2CommonUnitSetChanged(hUnit, 1);
	return TRUE;
}

void __fastcall ITEMSModeDropBeltOverflow(GAME *ptGame, UNIT *hUnit, UNIT *hBelt)
{
	void *hInventory;
	int i;
	UNIT *hItem;
	UNIT *hRemove;
	DWORD dwFlags;
	GAMEROOM *hRoom;
	COLPOINT tTo;
	COLPOINT tFrom;
	BELTSTXT tBelt;

	hInventory = hUnit->ptInventory;
#line 2077
	D2ASSERT(hInventory);
	if (hBelt)
		D2CommonBeltTblsGetBelt(D2CommonUnitGetItemCodeValue(hBelt), &tBelt);
	else
		D2CommonBeltTblsGetBelt(2, &tBelt);
	for (i = 0; i < 16; i++)
	{
		hItem = (UNIT *)D2CommonINVENTORYGetBeltSlot(hInventory, i);
		if (i < tBelt.numboxes || !hItem)
			continue;
		dwFlags = 0;
		D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
		SCmdSendItemFromBelt(SUnitGetClient(hUnit), hUnit, hItem, dwFlags);
		hRemove = (UNIT *)D2CommonINVENTORYRemoveItem(hInventory, hItem);
#line 2094
		D2ASSERT(hRemove);
#line 2095
		D2ASSERT(hItem == hRemove);
		D2CommonINVENTORYClearBeltSlot(hInventory, D2CommonUnitGetX(hItem));
		D2CommonUnitSetMode(hRemove, 4);
		ITEMSModeRemoveItemSkill(hUnit, hItem);
		D2CommonITEMSDataSetPage(hItem, 0);
		if (ITEMSModePlaceCursorItem(ptGame, hUnit, hItem->dwId, 0, 0, TRUE, TRUE, NULL))
			continue;
		D2CommonUnitGetPosition(hUnit, &tFrom);
		hRoom = ITEMSFindDropSpot((GAMEROOM *)D2CommonUnitGetPathRoom(hUnit), &tFrom, &tTo, 1);
		if (!hRoom)
		{
#line 2111
			D2WARN("lost item, oh well");
			continue;
		}
		D2CommonUnitInitPosition(0, hItem, hRoom, tTo.nX, tTo.nY);
		D2CommonUnitSetCollision(hItem, hRoom, tTo.nX, tTo.nY);
		D2CommonAddUnitToRoom(hItem, hRoom);
		hItem->dwFlags0 |= 4098;
		D2CommonUnitSetMode(hItem, 3);
		D2CommonITEMSDataSetPage(hItem, 255);
		QuestsItemDropped(ptGame, hUnit, hItem);
	}
}

BOOL __fastcall ITEMSModeUnequipToCursor(GAME *ptGame, UNIT *hUnit, DWORD dwLoc,
	BOOL fFlag20, int *pnResult)
{
	void *hInventory;
	UNIT *hItem;
	UNIT *hRemove;
	int nPlace;
	BOOL fSetComplete;

#line 2136
	D2ASSERT(ptGame);
#line 2137
	D2ASSERT(hUnit);
	*pnResult = 0;
	hInventory = hUnit->ptInventory;
#line 2142
	D2ASSERT(hInventory);
	if (D2CommonINVENTORYGetCursorItem(hInventory))
		return FALSE;
	dwLoc &= 255;
	hItem = (UNIT *)D2CommonINVENTORYGetLocSlot(hInventory, dwLoc);
	if (!hItem)
	{
		*pnResult = 0;
		return FALSE;
	}
	ITEMSModeClearMarkedItems(hUnit);
	nPlace = D2CommonINVENTORYTestBodyLocPlace(hUnit, dwLoc, 0, 0);
	if (nPlace != 3 && nPlace != 4)
	{
		*pnResult = 1;
		return FALSE;
	}
	D2CommonINVENTORYGetEquipField(hUnit, hInventory, &hItem, dwLoc);
	if (!hItem)
	{
		*pnResult = 1;
		return FALSE;
	}
	fSetComplete = D2CommonITEMSTestSetComplete(hUnit, hItem);
	D2CommonINVENTORYOfferRemoveDelta(hInventory, hItem);
	hRemove = (UNIT *)D2CommonINVENTORYRemoveItem(hInventory, hItem);
#line 2179
	D2ASSERT(hRemove);
#line 2180
	D2ASSERT(hItem == hRemove);
	D2CommonINVENTORYClearLocSlot(hInventory, dwLoc);
	if (D2CommonITEMSGetType(hRemove) == 19)
		ITEMSModeDropBeltOverflow(ptGame, hUnit, NULL);
	if (fFlag20)
		D2CommonITEMSDataSetFlags(hItem, 32, 1);
	else
		D2CommonINVENTORYSetCursorItem(hInventory, hItem);
	ITEMSModeRemoveEffects(ptGame, hItem, hUnit, FALSE, TRUE);
	ITEMSModeRefreshItems(ptGame, hUnit, 0);
	hItem->dwFlags0 &= ~2;
	D2CommonUnitSetMode(hItem, 4);
	D2CommonITEMSDataSetCmdFlags(hItem, 16, 1);
	if (ITEMSModeHasContents(hItem))
		D2CommonITEMSDataSetFlags(hItem, 1, 1);
	if (D2CommonITEMSDataTestFlags(hItem, 16384))
		D2CommonITEMSDataSetFlags(hItem, 16384, 0);
	(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
	D2CommonUnitSetChanged(hUnit, 1);
	if (fSetComplete)
		ITEMSModeRemoveSetBonus(hUnit, hItem);
	ITEMSModeUpdateWeaponSkills(hUnit);
	ITEMSModeCheckEquipment(ptGame, hUnit, FALSE);
	return TRUE;
}

BOOL __fastcall ITEMSModeSwapBodyItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	BYTE bLoc, BOOL fNoFitTest, int *pnResult)
{
	UNIT *hItem;
	BYTE dwLocIndex;
	void *hInventory;
	UNIT *hWeapon;
	UNIT *hBodyItem;
	UNIT *hRemove;
	BOOL fSetComplete;

#line 2229
	D2ASSERT(ptGame);
#line 2230
	D2ASSERT(hUnit);
	*pnResult = 0;
	ITEMSModeClearMarkedItems(hUnit);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (hItem->dwMode != 4)
	{
		*pnResult = 1;
		return FALSE;
	}
	dwLocIndex = bLoc;
	if (D2CommonINVENTORYTestBodyLocPlace(hUnit, dwLocIndex, hItem, fNoFitTest) != 5)
	{
		*pnResult = 1;
		return FALSE;
	}
	hInventory = hUnit->ptInventory;
	hWeapon = (UNIT *)D2CommonINVENTORYFindEquippedMatch(hInventory);
	if (hWeapon)
		D2CommonUnitSetPlayerWeaponId(hUnit, hWeapon);
	hBodyItem = (UNIT *)D2CommonINVENTORYGetLocSlot(hInventory, dwLocIndex);
	D2CommonINVENTORYGetEquipField(hUnit, hInventory, &hBodyItem, dwLocIndex);
	if (hBodyItem->dwMode != 1)
	{
		*pnResult = 1;
		return FALSE;
	}
	ITEMSModeRemoveEffects(ptGame, hBodyItem, hUnit, FALSE, TRUE);
	if (!fNoFitTest && !D2CommonITEMSTestFitsInventorySpace(hItem, hUnit, 0, 0, 0))
	{
		ITEMSModeApplyEffects(ptGame, hBodyItem, hUnit, FALSE);
		SUnitSetEvent(hUnit, 19, NULL);
		return FALSE;
	}
	if (D2CommonITEMSGetType(hItem) == 19)
		ITEMSModeDropBeltOverflow(ptGame, hUnit, hItem);
	fSetComplete = D2CommonITEMSTestSetComplete(hUnit, hBodyItem);
	D2CommonINVENTORYOfferRemoveDelta(hInventory, hBodyItem);
	hRemove = (UNIT *)D2CommonINVENTORYRemoveItem(hInventory, hBodyItem);
#line 2287
	D2ASSERT(hRemove);
#line 2288
	D2ASSERT(hBodyItem == hRemove);
	D2CommonINVENTORYClearLocSlot(hInventory, dwLocIndex);
	D2CommonINVENTORYSetCursorItem(hInventory, hBodyItem);
	hBodyItem->dwFlags0 &= ~2;
	D2CommonUnitSetMode(hBodyItem, 4);
	D2CommonITEMSDataSetFlags(hBodyItem, 128, 1);
	D2CommonITEMSDataSetCmdFlags(hBodyItem, 32, 1);
	D2CommonITEMSDataSetFlags(hBodyItem, 1, 1);
	if (D2CommonITEMSDataTestFlags(hBodyItem, 16384))
		D2CommonITEMSDataSetFlags(hBodyItem, 16384, 0);
	(void)D2CommonINVENTORYAddDeltaNode(hInventory, hBodyItem);
	if (fSetComplete)
		ITEMSModeRemoveSetBonus(hUnit, hBodyItem);
	if (D2CommonINVENTORYSetLocSlot(hInventory, hItem, dwLocIndex))
	{
		if (D2CommonINVENTORYAddItem(hInventory, hItem, 3))
		{
			D2CommonITEMSSetCategory(hItem, bLoc);
			D2CommonINVENTORYOfferAddDelta(hInventory, hItem);
			ITEMSModeApplyEffects(ptGame, hItem, hUnit, FALSE);
			ITEMSModeRefreshItems(ptGame, hUnit, 0);
			hItem->dwFlags0 &= ~2;
			D2CommonUnitSetMode(hItem, 1);
			D2CommonITEMSDataSetPage(hItem, 255);
			D2CommonITEMSDataSetFlags(hItem, 64, 1);
			D2CommonITEMSDataSetFlags(hItem, 1, 1);
			D2CommonITEMSDataSetCmdFlags(hItem, 32, 1);
			D2CommonITEMSDataSetFlags(hItem, 1, 1);
			if (D2CommonITEMSDataTestFlags(hItem, 16384))
				D2CommonITEMSDataSetFlags(hItem, 16384, 0);
			(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
			ITEMSModeUpdateWeaponSkills(hUnit);
			ITEMSModeApplySetBonus(ptGame, hUnit, hItem);
			D2CommonUnitSetChanged(hUnit, 1);
			ITEMSModeCheckEquipment(ptGame, hUnit, FALSE);
			return TRUE;
		}
#line 2343
		D2ASSERT(0);
	}
#line 2348
	D2ASSERT(0);
	*pnResult = 1;
	return FALSE;
}

BOOL __fastcall ITEMSModeEquipTwoHands(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	BYTE bLoc, int *pnResult)
{
	UNIT *hItem;
	DWORD dwLocIndex;
	DWORD dwRecord;
	void *hInventory;
	UNIT *hOpposite;
	DWORD dwOther;
	UNIT *hRemove;
	UNIT *hSelected;
	BOOL fSetComplete;

#line 2365
	D2ASSERT(ptGame);
#line 2366
	D2ASSERT(hUnit);
	*pnResult = 0;
	ITEMSModeClearMarkedItems(hUnit);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (hItem->dwMode != 4)
	{
		*pnResult = 1;
		return FALSE;
	}
	dwLocIndex = bLoc;
	if (D2CommonINVENTORYTestBodyLocPlace(hUnit, dwLocIndex, hItem, 0) != 7)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (!D2CommonITEMSTestFitsInventorySpace(hItem, hUnit, 0, 0, 0))
	{
		*pnResult = 1;
		return FALSE;
	}
	dwRecord = D2CommonUnitGetInvRecordIndex(hUnit, 0);
	hInventory = hUnit->ptInventory;
	if (bLoc == 5)
	{
		hOpposite = (UNIT *)D2CommonINVENTORYGetLocSlot(hInventory, 4);
		dwOther = 4;
	}
	else
	{
		hOpposite = (UNIT *)D2CommonINVENTORYGetLocSlot(hInventory, 5);
		dwOther = 5;
	}
	if (!D2CommonINVENTORYTestSlotFits(hInventory, hItem, dwOther, dwRecord))
	{
		*pnResult = 1;
		return FALSE;
	}
#line 2410
	D2ASSERT(hOpposite);
	if (hOpposite->dwMode != 1)
	{
		*pnResult = 1;
		return FALSE;
	}
	fSetComplete = D2CommonITEMSTestSetComplete(hUnit, hOpposite);
	D2CommonINVENTORYOfferRemoveDelta(hInventory, hOpposite);
	hRemove = (UNIT *)D2CommonINVENTORYRemoveItem(hInventory, hOpposite);
#line 2419
	D2ASSERT(hRemove);
#line 2420
	D2ASSERT(hOpposite == hRemove);
	D2CommonINVENTORYClearLocSlot(hInventory, dwOther);
	ITEMSModeRemoveEffects(ptGame, hRemove, hUnit, FALSE, TRUE);
	if (fSetComplete)
		ITEMSModeRemoveSetBonus(hUnit, hOpposite);
	if (D2CommonINVENTORYPlaceItem(hInventory, hRemove, dwRecord, 0, 0))
	{
		if (!D2CommonINVENTORYAddItem(hInventory, hRemove, 1))
		{
			*pnResult = 1;
			return FALSE;
		}
		D2CommonITEMSDataSetPage(hRemove, 0);
		hRemove->dwFlags0 &= ~2;
		D2CommonINVENTORYSetCursorItem(hInventory, 0);
		D2CommonUnitSetMode(hRemove, 0);
		if (D2CommonITEMSGetTypeHasInv(hRemove)
			&& (DWORD)D2CommonINVENTORYGetCategoryListCount(ITEMSModeGetItemInventory(hRemove)) > 0)
			D2CommonITEMSDataSetFlags(hRemove, 1, 1);
		if (D2CommonITEMSDataTestFlags(hRemove, 16384))
			D2CommonITEMSDataSetFlags(hRemove, 16384, 0);
		D2CommonITEMSDataSetCmdFlags(hRemove, 16384, 1);
		D2CommonITEMSDataSetFlags(hRemove, 1, 1);
		(void)D2CommonINVENTORYAddDeltaNode(hInventory, hRemove);
	}
	hSelected = (UNIT *)D2CommonINVENTORYGetLocSlot(hInventory, dwLocIndex);
#line 2451
	D2ASSERT(hSelected);
	if (hSelected->dwMode != 1)
	{
		*pnResult = 1;
		return FALSE;
	}
	ITEMSModeRemoveEffects(ptGame, hSelected, hUnit, FALSE, TRUE);
	if (!D2CommonITEMSTestFitsInventorySpace(hItem, hUnit, 0, 0, 0))
	{
		ITEMSModeApplyEffects(ptGame, hSelected, hUnit, FALSE);
		D2CommonINVENTORYSetCursorItem(hInventory, hItem);
		D2CommonUnitSetMode(hItem, 4);
		D2CommonITEMSDataSetCmdFlags(hItem, ITEM_CMDFLAG_SETHANDITEM, 1);
		(void)D2CommonINVENTORYAddDeltaNode(hUnit->ptInventory, hItem);
		D2CommonUnitSetChanged(hUnit, 1);
		SUnitSetEvent(hUnit, 19, NULL);
		D2CommonUnitSetChanged(hUnit, 1);
		return FALSE;
	}
	fSetComplete = D2CommonITEMSTestSetComplete(hUnit, hSelected);
	D2CommonINVENTORYOfferRemoveDelta(hInventory, hSelected);
	hRemove = (UNIT *)D2CommonINVENTORYRemoveItem(hInventory, hSelected);
#line 2473
	D2ASSERT(hRemove);
#line 2474
	D2ASSERT(hSelected == hRemove);
	D2CommonINVENTORYClearLocSlot(hInventory, dwLocIndex);
	D2CommonINVENTORYSetCursorItem(hInventory, hSelected);
	hSelected->dwFlags0 &= ~2;
	D2CommonUnitSetMode(hSelected, 4);
	D2CommonITEMSDataSetCmdFlags(hSelected, 16, 1);
	D2CommonITEMSDataSetFlags(hSelected, 1, 1);
	if (D2CommonITEMSDataTestFlags(hSelected, 16384))
		D2CommonITEMSDataSetFlags(hSelected, 16384, 0);
	(void)D2CommonINVENTORYAddDeltaNode(hInventory, hSelected);
	if (fSetComplete)
		ITEMSModeRemoveSetBonus(hUnit, hSelected);
	if (D2CommonINVENTORYSetLocSlot(hInventory, hItem, dwLocIndex))
	{
		if (D2CommonINVENTORYAddItem(hInventory, hItem, 3))
		{
			D2CommonITEMSSetCategory(hItem, bLoc);
			D2CommonINVENTORYOfferAddDelta(hInventory, hItem);
			ITEMSModeApplyEffects(ptGame, hItem, hUnit, FALSE);
			ITEMSModeRefreshItems(ptGame, hUnit, 0);
			hItem->dwFlags0 &= ~2;
			D2CommonUnitSetMode(hItem, 1);
			D2CommonITEMSDataSetPage(hItem, 255);
			D2CommonITEMSDataSetFlags(hItem, 8, 1);
			D2CommonITEMSDataSetCmdFlags(hItem, 8, 1);
			D2CommonITEMSDataSetFlags(hItem, 1, 1);
			if (D2CommonITEMSGetTypeHasInv(hItem)
				&& (DWORD)D2CommonINVENTORYGetCategoryListCount(ITEMSModeGetItemInventory(hItem)) > 0)
				D2CommonITEMSDataSetFlags(hItem, 1, 1);
			if (D2CommonITEMSDataTestFlags(hItem, 16384))
				D2CommonITEMSDataSetFlags(hItem, 16384, 0);
			(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
			ITEMSModeUpdateWeaponSkills(hUnit);
			ITEMSModeApplySetBonus(ptGame, hUnit, hItem);
			D2CommonUnitSetChanged(hUnit, 1);
			ITEMSModeCheckEquipment(ptGame, hUnit, FALSE);
			return TRUE;
		}
#line 2527
		D2ASSERT(0);
	}
#line 2532
	D2ASSERT(0);
	*pnResult = 1;
	return FALSE;
}

static BYTE sgfCubeClassFound;
static int sgnCubeClass;

BOOL __fastcall ITEMSModeSwapGridItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwGridItemId, int nX, int nY,
	int *pnResult)
{
	UNIT *hItem;
	UNIT *hGridItem;
	UNIT *hRemove;
	BYTE bPage;
	void *hInventory;

	DWORD dwX;
	DWORD dwY;

#line 2549
	D2ASSERT(ptGame);
#line 2550
	D2ASSERT(hUnit);
	*pnResult = 0;
	ITEMSModeClearMarkedItems(hUnit);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem || hItem->dwMode != 4)
		return FALSE;
	hGridItem = SUnitFind(ptGame, UNIT_ITEM, dwGridItemId);
	bPage = D2CommonITEMSDataGetPage(hGridItem);
	if (bPage == 3)
	{
		if (!sgfCubeClassFound)
		{
			if (!D2CommonItemTblsFindItemByCode(544763746, &sgnCubeClass))
				return FALSE;
			sgfCubeClassFound = TRUE;
		}
		if (hItem->dwClassId == (DWORD)sgnCubeClass)
			return FALSE;
	}
	if (!hGridItem || hGridItem->dwMode != 0)
		return FALSE;
	hInventory = hUnit->ptInventory;
	if (bPage == 2)
		PlrTradeRemoveItem(ptGame, hUnit, hGridItem);
	hRemove = (UNIT *)D2CommonINVENTORYRemoveItem(hInventory, hGridItem);
#line 2599
	D2ASSERT(hRemove);
#line 2600
	D2ASSERT(hGridItem == hRemove);

	D2CommonINVENTORYRemoveItemAt(hInventory, D2CommonUnitGetX(hGridItem), D2CommonUnitGetY(hGridItem), D2CommonUnitGetInvRecordIndex(hUnit, bPage), 0, bPage);
	D2CommonITEMSDataSetPage(hGridItem, 255);
	D2CommonINVENTORYSetCursorItem(hInventory, hGridItem);
	if (hUnit->eType == UNIT_PLAYER)
		ITEMSModeUpdateBookSkill(hUnit, hGridItem, FALSE);
	D2CommonITEMSDataSetSwapPage(hGridItem, bPage);
	hGridItem->dwFlags0 &= ~2;
	D2CommonUnitSetMode(hGridItem, 4);
	D2CommonITEMSDataSetCmdFlags(hGridItem, 262144, 1);
	if (D2CommonITEMSGetTypeHasInv(hGridItem)
		&& (DWORD)D2CommonINVENTORYGetCategoryListCount(ITEMSModeGetItemInventory(hGridItem)) > 0)
		D2CommonITEMSDataSetFlags(hGridItem, 1, 1);
	if (D2CommonITEMSDataTestFlags(hGridItem, 16384))
		D2CommonITEMSDataSetFlags(hGridItem, 16384, 0);
	(void)D2CommonINVENTORYAddDeltaNode(hInventory, hGridItem);
	if (D2CommonINVENTORYPlaceItemAt(hInventory, hItem, nX, nY, D2CommonUnitGetInvRecordIndex(hUnit,
		bPage), 0,
		bPage))
	{
		D2CommonITEMSDataSetSwapPage(hItem, 0);
		D2CommonITEMSDataSetPage(hItem, bPage);
		D2CommonUnitSetX(hItem, nX);
		D2CommonUnitSetY(hItem, nY);
		if (D2CommonINVENTORYAddItem(hInventory, hItem, 1))
		{
			hItem->dwFlags0 &= ~2;
			if (hUnit->eType == UNIT_PLAYER)
				ITEMSModeUpdateBookSkill(hUnit, hItem, TRUE);
			D2CommonUnitSetMode(hItem, 0);
			if (D2CommonITEMSGetTypeHasInv(hItem)
				&& (DWORD)D2CommonINVENTORYGetCategoryListCount(ITEMSModeGetItemInventory(hItem)) > 0)
				D2CommonITEMSDataSetFlags(hItem, 1, 1);
			if (D2CommonITEMSDataTestFlags(hItem, 16384))
				D2CommonITEMSDataSetFlags(hItem, 16384, 0);
			D2CommonITEMSDataSetCmdFlags(hItem, 262144, 1);
			(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
			if (bPage == 2)
				PlrTradeAddItem(ptGame, hUnit, hItem);
			D2CommonUnitSetChanged(hUnit, 1);
			return TRUE;
		}
#line 2667
		D2ASSERT(0);
	}
#line 2672
	D2ASSERT(0);
	return FALSE;
}

void __fastcall ITEMSModeRemoveGridItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem, BOOL fSend)
{
	void *hInventory;
	UNIT *hRemove;
	BYTE bPage;
	DWORD dwRecord;
	DWORD dwX;
	DWORD dwY;

#line 2687
	D2ASSERT(ptGame);
#line 2688
	D2ASSERT(hUnit);
	hItem->dwFlags0 &= ~2;
	hInventory = hUnit->ptInventory;
	hRemove = (UNIT *)D2CommonINVENTORYRemoveItem(hInventory, hItem);
#line 2694
	D2ASSERT(hRemove);
#line 2695
	D2ASSERT(hItem == hRemove);
	bPage = D2CommonITEMSDataGetPage(hItem);
	dwRecord = ((int (__stdcall *)(UNIT *, BYTE))D2CommonUnitGetInvRecordIndexRaw)(hUnit, bPage);

	((void (__stdcall *)(void *, DWORD, DWORD, DWORD, DWORD, BYTE))D2CommonINVENTORYRemoveItemAtRaw)(hInventory, D2CommonUnitGetX(hItem), D2CommonUnitGetY(hItem), dwRecord, 0, bPage);
	D2CommonITEMSDataSetPage(hItem, 255);
	if (fSend)
		SUnitMsgSendRemove(hItem, SUnitGetClient(hUnit));
	ITEMSFree(ptGame, hItem);
}

void __fastcall ITEMSModeUseUpBeltItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	CLIENT *ptClient;
	DWORD dwFlags;

	ptClient = SUnitGetClient(hUnit);
	dwFlags = 0;
	D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
	SCmdSendItemFromBelt(ptClient, hUnit, hItem, dwFlags);
	ITEMSModeFreeBeltItem(ptGame, hUnit, hItem);
}

extern int ITEMSModeGetTomeSkill(UNIT *hItem);
__inline int ITEMSModeGetTomeSkill(UNIT *hItem)
{
	BOOKSTXT *pSuffix;

	if (D2CommonITEMSGetType(hItem) == 18)
	{
		pSuffix = (BOOKSTXT *)D2CommonItemTblsGetBook(D2CommonITEMSDataGetMagicSuffixAt(hItem, 0));
#line 2739
		D2ASSERT(pSuffix);
		return pSuffix->skillbookoffset;
	}
	if (D2CommonITEMSGetType(hItem) == 22)
	{
		pSuffix = (BOOKSTXT *)D2CommonItemTblsGetBook(D2CommonITEMSDataGetMagicSuffixAt(hItem, 0));
#line 2743
		D2ASSERT(pSuffix);
		return pSuffix->skillscrolloffset;
	}
	return -1;
}

static __inline BOOL ITEMSModeIsEmptyTome(UNIT *hItem)
{
	return D2CommonITEMSGetType(hItem) == 18 && D2CommonStatsUnitGetValue(hItem, STAT_QUANTITY) <= 0;
}

BOOL __fastcall ITEMSModeUseItemOnItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwTargetId, int *pnResult)
{
	UNIT *hItem;
	UNIT *hTarget;
	DWORD dwTargetMode;
	int nQuantity;
	int nSkill;
	CLIENT *ptClient;

#line 2779
	D2ASSERT(ptGame);
#line 2780
	D2ASSERT(hUnit);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	hTarget = SUnitFind(ptGame, UNIT_ITEM, dwTargetId);
	*pnResult = 0;
	if (!hTarget)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (!hItem || hItem == hTarget)
	{
		ITEMSModeClearMarkedItems(hUnit);
		return FALSE;
	}
	if (hItem->eType != UNIT_ITEM)
	{
		ITEMSModeClearMarkedItems(hUnit);
		return FALSE;
	}
	if (D2CommonINVENTORYGetCursorItem(ITEMSModeGetItemInventory(hUnit)))
		return FALSE;
	dwTargetMode = hTarget->dwMode;
	if (hItem->dwMode != 0 && hItem->dwMode != 1)
	{
		if (dwTargetMode == ITEM_MODE_INVBELT)
		{
			ITEMSModeClearMarkedItems(hUnit);
			ITEMSModeUseUpBeltItem(ptGame, hUnit, hTarget);
		}
		else if (dwTargetMode == 0)
		{
			if (D2CommonITEMSGetType(hTarget) == 18)
			{
				nQuantity = (DWORD)D2CommonStatsUnitGetValue(hTarget, STAT_QUANTITY) - 1;
#line 2814
				D2ASSERT(nQuantity >= 0);
				D2CommonStatsUnitSetValue(hTarget, STAT_QUANTITY, nQuantity);
				SCmdSendItemStat(SUnitGetClient(hUnit), hTarget, 1, STAT_QUANTITY, nQuantity);
			}
		}
		else
		{
#line 2819
			D2ASSERT(0);
		}
		return FALSE;
	}
	if (ITEMSModeIsEmptyTome(hTarget))
	{
		ITEMSModeClearMarkedItems(hUnit);
		SCmdSendUnitTypeId(SUnitGetClient(hUnit), 111, (BYTE)hTarget->eType, hTarget->dwId);
		return FALSE;
	}
	if (SkillsItemUseSpell(ptGame, hUnit, hTarget, hItem, 0, 0))
	{
		if (dwTargetMode == ITEM_MODE_INVBELT)
		{
			nSkill = ITEMSModeGetTomeSkill(hTarget);
			if (nSkill != -1)
			{
				ITEMSModeUseBookCharge(hUnit, nSkill);
				ITEMSModeUseUpBeltItem(ptGame, hUnit, hTarget);
			}
			else
				SCmdSendUnitTypeId(SUnitGetClient(hUnit), 111, (BYTE)hTarget->eType, hTarget->dwId);
			ITEMSModeClearMarkedItems(hUnit);
		}
		else if (dwTargetMode == 0)
		{
			DWORD dwFlags;
			BOOL fUsedUp;

			fUsedUp = TRUE;
			if (D2CommonITEMSGetType(hTarget) == 18)
			{
				nSkill = ITEMSModeGetTomeSkill(hTarget);
				if (nSkill != -1 && D2CommonStatsUnitGetValue(hTarget, STAT_QUANTITY) > 0)
				{
					D2CommonStatsUnitAddValue(hTarget, STAT_QUANTITY, -1);
					nQuantity = D2CommonStatsUnitGetValue(hTarget, STAT_QUANTITY);
					SCmdSendItemStat(SUnitGetClient(hUnit), hTarget, 1, STAT_QUANTITY, nQuantity);
					ITEMSModeUseBookCharge(hUnit, nSkill);
					fUsedUp = FALSE;
				}
			}
			else
			{
				nSkill = ITEMSModeGetTomeSkill(hTarget);
				if (nSkill != -1)
					ITEMSModeUseBookCharge(hUnit, nSkill);
			}
			if (fUsedUp)
			{
				ptClient = SUnitGetClient(hUnit);
				dwFlags = 0;
				D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
				SCmdSendItemFromGrid(ptClient, hUnit, hTarget, dwFlags);
				ITEMSModeRemoveGridItem(ptGame, hUnit, hTarget, FALSE);
			}
			else
				SCmdSendUnitTypeId(SUnitGetClient(hUnit), 111, (BYTE)hTarget->eType, hTarget->dwId);
			ITEMSModeClearMarkedItems(hUnit);
		}
		else
		{
#line 2870
			D2ASSERT(0);
		}
	}
	return TRUE;
}

void __fastcall ITEMSModeClearMarkedItems(UNIT *hUnit)
{
	void *hInventory;
	void *ptNode;
	UNIT *hItem;

#line 96
	D2ASSERT(hUnit);
	hInventory = hUnit->ptInventory;
#line 99
	D2ASSERT(hInventory);
	for (ptNode = D2CommonINVENTORYGetCategoryListHead(hInventory); ptNode;
		ptNode = (void *)D2CommonINVENTORYNodeGetNext(ptNode))
	{
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
		if (!D2CommonITEMSDataTestFlags(hItem, 4))
			continue;
		D2CommonITEMSDataSetFlags(hItem, 4, 0);
		SCmdSendSpellIcon(SUnitGetClient(hUnit), hItem, 255, 1, 0, 65535);
	}
}

void __fastcall ITEMSModeUseBookCharge(UNIT *hUnit, int nSkill)
{
	void *hSkill;
	int nCount;
	void *hRightSkill;
	hSkill = (void *)D2CommonSkillsFindByKey(hUnit, nSkill);
	if (hSkill)
	{
	nCount = D2CommonSkillsGetNodeCount(hSkill);
	nCount--;
	if (nCount <= 0)
	{
		hRightSkill = (void *)D2CommonUnitGetRightSkill(hUnit);
		if (hRightSkill == hSkill)
			SkillsSetCurrent(hUnit, FALSE, SKILL_ATTACK);
		nCount = 0;
	}
	D2CommonSkillsSetNodeCount(hSkill, nCount);
	SCmdSendSkillState(SUnitGetClient(hUnit), (BYTE)hUnit->eType, hUnit->dwId, (WORD)nSkill,
		(BYTE)nCount); }
	else
#line 2768
		D2ASSERT(0);
}

static inline void ITEMSModeSpendSkillCharge(UNIT *hUnit, int nSkill)
{
	void *hSkill;
	int nCount;

	hSkill = (void *)D2CommonSkillsFindByKey(hUnit, nSkill);
	if (!hSkill)
		return;
	nCount = D2CommonSkillsGetNodeCount(hSkill) - 1;
	if (nCount <= 0)
	{
		if ((void *)D2CommonUnitGetRightSkill(hUnit) == hSkill)
			SkillsSetCurrent(hUnit, FALSE, SKILL_ATTACK);
		nCount = 0;
	}
	D2CommonSkillsSetNodeCount(hSkill, nCount);
	SCmdSendSkillState(SUnitGetClient(hUnit), (BYTE)hUnit->eType, hUnit->dwId, (WORD)nSkill,
		(BYTE)nCount);
}

BOOL __fastcall ITEMSModeUseItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwX, DWORD dwY, int *pnResult)
{
	void *pInventory;
	UNIT *hItem;
	int nSkill;
	GAMEITEMSTXT *ptItemStats;
	void *ptQuest;

#line 2883
	D2ASSERT(ptGame);
#line 2884
	D2ASSERT(hUnit);
	*pnResult = 0;
	ITEMSModeClearMarkedItems(hUnit);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem)
	{
		*pnResult = 1;
		return FALSE;
	}
	pInventory = hUnit->ptInventory;
	if (D2CommonINVENTORYGetCursorItem(pInventory))
		return FALSE;
	if (hItem->dwMode != 0 || !D2CommonITEMSGetTypeUseable(hItem))
	{
		*pnResult = 1;
		return FALSE;
	}
	if (ITEMSModeIsEmptyTome(hItem))
		return FALSE;
	if (PlrTradeIsTrading(ptGame, hUnit))
		return FALSE;
	if (SkillsItemUseSpell(ptGame, hUnit, hItem, hItem, dwX, dwY))
	{
		if (D2CommonITEMSGetType(hItem) == 18)
		{
			nSkill = ITEMSModeGetTomeSkill(hItem);
			if (nSkill != -1 && D2CommonStatsUnitGetValue(hItem, STAT_QUANTITY) > 0)
			{
				D2CommonStatsUnitAddValue(hItem, STAT_QUANTITY, -1);
				SCmdSendItemStat(SUnitGetClient(hUnit), hItem, 1, STAT_QUANTITY,
					D2CommonStatsUnitGetValue(hItem, STAT_QUANTITY));
				SCmdSendUnitTypeId(SUnitGetClient(hUnit), 111, (BYTE)hItem->eType, hItem->dwId);
				ITEMSModeUseBookCharge(hUnit, nSkill);
			}
			return TRUE;
		}
		nSkill = ITEMSModeGetTomeSkill(hItem);
		if (nSkill != -1)
			ITEMSModeSpendSkillCharge(hUnit, nSkill);
	}
	else
	{
		ptItemStats = (GAMEITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
		ptQuest = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
		if (ptItemStats->code == 544437089)
		{
			if (D2CommonStepTestGridBit(ptQuest, 9, 5))
			{
				D2CommonQuestRecordClearBit(ptQuest, 9, 5);
				D2CommonStatsUnitAddValue(hUnit, 5, 1);
				QuestsSendFlag(hUnit, 8);
			}
			else
				SUnitSetEvent(hUnit, 9, hUnit);
			{
				CLIENT *ptClient;
				DWORD dwFlags;

				ITEMSModeClearMarkedItems(hUnit);
				ptClient = SUnitGetClient(hUnit);
				dwFlags = 0;
				D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
				SCmdSendItemFromGrid(ptClient, hUnit, hItem, dwFlags);
				ITEMSModeRemoveGridItem(ptGame, hUnit, hItem, FALSE);
				return TRUE;
			}
		}
		else if (ptItemStats->code == 544897400)
		{
			if (D2CommonStepTestGridBit(ptQuest, 20, 5))
			{
				D2CommonQuestRecordClearBit(ptQuest, 20, 5);
				D2CommonStatsUnitAddValue(hUnit, 7, 5120);
				D2CommonStatsUnitAddValue(hUnit, 6, 5120);
				QuestsSendFlag(hUnit, 18);
			}
			else
				SUnitSetEvent(hUnit, 9, hUnit);
			{
				CLIENT *ptClient;
				DWORD dwFlags;

				ITEMSModeClearMarkedItems(hUnit);
				ptClient = SUnitGetClient(hUnit);
				dwFlags = 0;
				D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
				SCmdSendItemFromGrid(ptClient, hUnit, hItem, dwFlags);
				ITEMSModeRemoveGridItem(ptGame, hUnit, hItem, FALSE);
				return TRUE;
			}
		}
		return FALSE; }
	{
		CLIENT *ptClient;
		DWORD dwFlags;

		ITEMSModeClearMarkedItems(hUnit);
		ptClient = SUnitGetClient(hUnit);
		dwFlags = 0;
		D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
		SCmdSendItemFromGrid(ptClient, hUnit, hItem, dwFlags);
		ITEMSModeRemoveGridItem(ptGame, hUnit, hItem, FALSE);
		return TRUE;
	}
}

extern BOOL ITEMSModeStackOnto(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget);
__inline BOOL ITEMSModeStackOnto(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget)
{
	void *hInventory;
	int nSrcValue;
	int nDstValue;
	int nMaxStack;
	int nQuantity;

	hInventory = hUnit->ptInventory;
	nSrcValue = D2CommonStatsUnitGetValue(hItem, STAT_QUANTITY);
#line 3002
	D2ASSERT(nSrcValue >= 0);
	nDstValue = D2CommonStatsUnitGetValue(hTarget, STAT_QUANTITY);
#line 3004
	D2ASSERT(nDstValue >= 0);
	nMaxStack = D2CommonITEMSGetTypeMaxStack(hTarget);
#line 3006
	D2ASSERT(nMaxStack >= 0);
	if (nDstValue + nSrcValue > nMaxStack)
	{
		D2CommonStatsUnitSetValue(hTarget, STAT_QUANTITY, nMaxStack);
		SCmdSendItemStat(SUnitGetClient(hUnit), hTarget, 1, STAT_QUANTITY, nMaxStack);
		nQuantity = nDstValue - nMaxStack + nSrcValue;
#line 3015
		D2ASSERT(nQuantity >= 0);
		D2CommonStatsUnitSetValue(hItem, STAT_QUANTITY, nQuantity);
		SCmdSendItemStat(SUnitGetClient(hUnit), hItem, 1, STAT_QUANTITY, nQuantity);
		if (D2CommonITEMSGetType(hItem) == 18 && D2CommonITEMSGetType(hTarget) == 18)
			ITEMSModeAddBookCharges(hUnit, hTarget, D2CommonITEMSGetTypeMaxStack(hTarget) - nDstValue);
		return TRUE;
	}
	nQuantity = nDstValue + nSrcValue;
#line 3029
	D2ASSERT(nQuantity >= 0);
	D2CommonStatsUnitSetValue(hTarget, STAT_QUANTITY, nQuantity);
	SCmdSendItemStat(SUnitGetClient(hUnit), hTarget, 1, STAT_QUANTITY, nQuantity);
	if (D2CommonITEMSGetType(hItem) == 18 && D2CommonITEMSGetType(hTarget) == 18)
		ITEMSModeAddBookCharges(hUnit, hTarget, nSrcValue);
	D2CommonINVENTORYSetCursorItem(hInventory, 0);
	SCmdSendCursorCleared(SUnitGetClient(hUnit), hUnit);
	ITEMSFree(ptGame, hItem);
	return FALSE;
}

BOOL __fastcall ITEMSModeStackCursorItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwTargetId, int *pnResult)
{
	UNIT *hItem;
	UNIT *hTarget;

#line 3057
	D2ASSERT(ptGame);
#line 3058
	D2ASSERT(hUnit);
	*pnResult = 0;
	ITEMSModeClearMarkedItems(hUnit);
	if (dwItemId == dwTargetId)
	{
		*pnResult = 1;
		return FALSE;
	}
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem)
	{
		*pnResult = 1;
		return FALSE;
	}
	hTarget = SUnitFind(ptGame, UNIT_ITEM, dwTargetId);
	if (!hTarget)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (!D2CommonITEMSTestCanStack(hItem, hTarget))
		return FALSE;
	if (hItem->dwClassId == hTarget->dwClassId)
	{
		if (D2CommonITEMSTestCanStack(hItem, hTarget) && ITEMSModeStackOnto(ptGame, hUnit, hItem, hTarget))
			D2CommonITEMSDataSetFlags(hTarget, 8, 1);
		D2CommonITEMSDataSetCmdFlags(hTarget, 256, 1);
		(void)D2CommonINVENTORYAddDeltaNode(hUnit->ptInventory, hTarget);
		D2CommonUnitSetChanged(hUnit, 1);
	}
	return TRUE;
}

BOOL __fastcall ITEMSModeNone(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwResult)
{
	return FALSE;
}

BOOL __fastcall ITEMSModePutInBelt(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwSlot, BOOL fAnywhere, int *pnResult)
{
	UNIT *hItem;
	void *hInventory;

#line 3172
	D2ASSERT(ptGame);
#line 3173
	D2ASSERT(hUnit);
	*pnResult = 0;
	if (hUnit->dwFlags & 16777216)
		return FALSE;
	ITEMSModeClearMarkedItems(hUnit);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (hItem->dwMode != 4)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (!D2CommonITEMSGetTypeBelt(hItem))
		goto fail;
	hInventory = hUnit->ptInventory;
	if (fAnywhere)
	{
		if (!D2CommonINVENTORYAddBeltItem(hInventory, hItem))
			goto fail;
	}
	else
	{
		BYTE bInvWidth;
		BYTE bInvHeight;
		D2CommonITEMSGetTypeInvWidthHeight(hItem, &bInvWidth, &bInvHeight);
		if (!D2CommonINVENTORYPlaceBeltItem(hInventory, hItem, dwSlot))
			goto fail;
		D2CommonITEMSDataSetPage(hItem, 255);
	}
	if (D2CommonINVENTORYAddItem(hInventory, hItem, 2))
	{
		D2CommonINVENTORYSetCursorItem(hInventory, 0);
		if (hUnit->eType == UNIT_PLAYER)
			ITEMSModeUpdateBookSkill(hUnit, hItem, TRUE);
		hItem->dwFlags0 &= ~2;
		D2CommonUnitSetMode(hItem, ITEM_MODE_INVBELT);
		D2CommonITEMSDataSetCmdFlags(hItem, 1024, 1);
		D2CommonUnitSetChanged(hUnit, 1);
		(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
		return TRUE;
	}
#line 3228
	D2ASSERT(0);
fail:
	return FALSE;
}

BOOL __fastcall ITEMSModeTakeFromBelt(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	int *pnResult)
{
	void *hInventory;
	UNIT *hItem;
	UNIT *hRemove;

#line 3241
	D2ASSERT(ptGame);
#line 3242
	D2ASSERT(hUnit);
	*pnResult = 0;
	if (hUnit->dwFlags & 16777216)
		return FALSE;
	hInventory = hUnit->ptInventory;
#line 3253
	D2ASSERT(hInventory);
	if (D2CommonINVENTORYGetCursorItem(hInventory))
		return FALSE;
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (hItem->dwMode != ITEM_MODE_INVBELT)
	{
		*pnResult = 1;
		return FALSE;
	}
	ITEMSModeClearMarkedItems(hUnit);
	hRemove = (UNIT *)D2CommonINVENTORYRemoveItem(hInventory, hItem);
#line 3276
	D2ASSERT(hRemove);
#line 3277
	D2ASSERT(hItem == hRemove);
	D2CommonINVENTORYClearBeltSlot(hInventory, D2CommonUnitGetX(hItem));
	D2CommonINVENTORYSetCursorItem(hInventory, hItem);
	if (hUnit->eType == UNIT_PLAYER)
		ITEMSModeUpdateBookSkill(hUnit, hItem, FALSE);
	hItem->dwFlags0 &= ~2;
	D2CommonUnitSetMode(hItem, 4);
	D2CommonITEMSDataSetCmdFlags(hItem, 2048, 1);
	D2CommonUnitSetChanged(hUnit, 1);
	(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
	return TRUE;
}

BOOL __fastcall ITEMSModeSwapBeltItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwBeltItemId, int *pnResult)
{
	UNIT *hItem;
	UNIT *hBeltItem;
	UNIT *hRemove;
	void *hInventory;

#line 3299
	D2ASSERT(ptGame);
#line 3300
	D2ASSERT(hUnit);
	*pnResult = 0;
	if (hUnit->dwFlags & 16777216)
		return FALSE;
	ITEMSModeClearMarkedItems(hUnit);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem || hItem->dwMode != 4 || !D2CommonITEMSGetTypeBelt(hItem))
		return FALSE;
	hBeltItem = SUnitFind(ptGame, UNIT_ITEM, dwBeltItemId);
	if (!hBeltItem)
		return FALSE;
	if (hBeltItem->dwMode != ITEM_MODE_INVBELT)
	{
		*pnResult = 1;
		return FALSE;
	}
	hInventory = hUnit->ptInventory;
	hRemove = (UNIT *)D2CommonINVENTORYRemoveItem(hInventory, hBeltItem);
#line 3331
	D2ASSERT(hRemove);
#line 3332
	D2ASSERT(hBeltItem == hRemove);
	D2CommonINVENTORYClearBeltSlot(hInventory, D2CommonUnitGetX(hBeltItem));
	D2CommonINVENTORYSetCursorItem(hInventory, hBeltItem);
	if (hUnit->eType == UNIT_PLAYER)
		ITEMSModeUpdateBookSkill(hUnit, hBeltItem, FALSE);
	hBeltItem->dwFlags0 &= ~2;
	D2CommonUnitSetMode(hBeltItem, 4);
	D2CommonITEMSDataSetCmdFlags(hBeltItem, 4096, 1);
	(void)D2CommonINVENTORYAddDeltaNode(hInventory, hBeltItem);
	if (D2CommonINVENTORYPlaceBeltItem(hInventory, hItem, D2CommonUnitGetX(hBeltItem)))
	{
		D2CommonUnitSetX(hItem, D2CommonUnitGetX(hBeltItem));
		D2CommonUnitSetY(hItem, 0);
		if (D2CommonINVENTORYAddItem(hInventory, hItem, 2))
		{
			if (hUnit->eType == UNIT_PLAYER)
				ITEMSModeUpdateBookSkill(hUnit, hItem, TRUE);
			hItem->dwFlags0 &= ~2;
			D2CommonUnitSetMode(hItem, ITEM_MODE_INVBELT);
			D2CommonITEMSDataSetCmdFlags(hItem, 4096, 1);
			(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
			D2CommonUnitSetChanged(hUnit, 1);
			return TRUE;
		}
#line 3359
		D2ASSERT(0);
	}
#line 3363
	D2ASSERT(0);
	return FALSE;
}

void __fastcall ITEMSModeFreeBeltItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	void *hInventory;
	UNIT *hRemove;

	hItem->dwFlags0 &= ~2;
	hInventory = hUnit->ptInventory;
	hRemove = (UNIT *)D2CommonINVENTORYRemoveItem(hInventory, hItem);
#line 3380
	D2ASSERT(hRemove);
#line 3381
	D2ASSERT(hItem == hRemove);
	D2CommonINVENTORYClearBeltSlot(hInventory, D2CommonUnitGetX(hItem));
	ITEMSFree(ptGame, hItem);
}

void __fastcall ITEMSModeBeltShiftDown(GAME *ptGame, UNIT *hUnit, DWORD dwSlot)
{
	void *hInventory;
	DWORD dwRow;
	DWORD dwIndex;
	DWORD dwGap;
	DWORD dwToRow;
	UNIT *hItem;

	if (dwSlot >= 4)
		return;
	hInventory = hUnit->ptInventory;
	dwGap = 1;
	for (dwRow = 1, dwIndex = dwSlot + 4; dwRow < 4; dwRow++, dwIndex += 4)
	{
		hItem = (UNIT *)D2CommonINVENTORYGetBeltSlot(hInventory, dwIndex);
		dwToRow = hItem ? dwRow - dwGap : 0;
		if (hItem)
		{
			if (!D2CommonINVENTORYPlaceBeltItem(hInventory, hItem, dwSlot + dwToRow * 4))
			{
#line 3404
				D2ASSERT(0);
			}
			D2CommonITEMSDataSetFlags(hItem, 1024, 1);
			D2CommonITEMSDataSetFlags(hItem, 1, 1);
			if (D2CommonITEMSDataTestFlags(hItem, 16384))
				D2CommonITEMSDataSetFlags(hItem, 16384, 0);
			D2CommonUnitSetChanged(hUnit, 1);
			(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
		}
		else
			dwGap++;
		D2CommonINVENTORYClearBeltSlot(hInventory, dwIndex);
	}
}

BOOL __fastcall ITEMSModeUseBeltItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwX, DWORD dwY, int *pnResult)
{
	void *pInventory;
	UNIT *hItem;
	int nSkill;
	DWORD dwSlot;

#line 3430
	D2ASSERT(ptGame);
#line 3431
	D2ASSERT(hUnit);
	*pnResult = 0;
	if (hUnit->dwFlags & 16777216)
		return FALSE;
	ITEMSModeClearMarkedItems(hUnit);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem)
	{
		*pnResult = 1;
		return FALSE;
	}
	pInventory = hUnit->ptInventory;
	if (D2CommonINVENTORYGetCursorItem(pInventory))
		return FALSE;
	if (hItem->dwMode != ITEM_MODE_INVBELT || !D2CommonITEMSGetTypeUseable(hItem))
	{
		*pnResult = 1;
		return FALSE;
	}
	if (PlrTradeIsTrading(ptGame, hUnit))
		return FALSE;
	if (!SkillsItemUseSpell(ptGame, hUnit, hItem, hItem, dwX, dwY))
		return FALSE;
	nSkill = ITEMSModeGetTomeSkill(hItem);
	if (nSkill != -1)
		ITEMSModeSpendSkillCharge(hUnit, nSkill);
	ITEMSModeClearMarkedItems(hUnit);
	dwSlot = D2CommonUnitGetX(hItem);
	ITEMSModeUseUpBeltItem(ptGame, hUnit, hItem);
	ITEMSModeBeltShiftDown(ptGame, hUnit, dwSlot);
	return TRUE;
}

void __fastcall ITEMSModeMarkHandItem(UNIT *hUnit, UNIT *hItem)
{
	void *hInventory;

	hInventory = hUnit->ptInventory;
	if (D2CommonITEMSDataTestFlags(hItem, 16) || !hInventory)
		return;
#line 3501
	D2ASSERT(!ITEMSDataTestCmdFlags(hItem, ITEM_CMDFLAG_SETHANDITEM));
	D2CommonITEMSDataSetFlags(hItem, 16, 1);
	D2CommonITEMSDataSetFlags(hItem, 1, 1);
	if (D2CommonITEMSDataTestFlags(hItem, 16384))
		D2CommonITEMSDataSetFlags(hItem, 16384, 0);
	(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
	D2CommonUnitSetChanged(hUnit, 1);
	SUnitSetEvent(hUnit, 6, hUnit);
}

BOOL __fastcall ITEMSModeFreeCursorItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	void *hInventory;
	CLIENT *ptClient;
	DWORD dwFlags;

	hInventory = hUnit->ptInventory;
	if (!hItem || (UNIT *)D2CommonINVENTORYGetCursorItem(hInventory) != hItem)
		return FALSE;
	ptClient = SUnitGetClient(hUnit);
	dwFlags = 0;
	D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
	SCmdSendCursorCleared(ptClient, hUnit);
	D2CommonINVENTORYSetCursorItem(hInventory, 0);
	ITEMSFree(ptGame, hItem);
	return TRUE;
}

BOOL __fastcall ITEMSModeSocketGem(GAME *ptGame, UNIT *hUnit, DWORD dwGemId,
	DWORD dwItemId, int *pnResult, BOOL fSend,
	BOOL fClearCursor, BOOL fCheckMode)
{
	UNIT *hGem;
	UNIT *hItem;
	DWORD nType;
	void *hItemInventory;

#line 3550
	D2ASSERT(ptGame);
	*pnResult = 0;
	if (hUnit)
		ITEMSModeClearMarkedItems(hUnit);
	hGem = SUnitFind(ptGame, UNIT_ITEM, dwGemId);
	if (!hGem)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (hGem->dwMode == 4)
	{
		hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
		if (!hItem)
		{
			*pnResult = 1;
			return FALSE;
		}
		if (fCheckMode && hItem->dwMode != 0 && hItem->dwMode != 1)
			return FALSE;
		if (hGem->eType != UNIT_ITEM || hItem->eType != UNIT_ITEM)
			return FALSE;
		if (D2CommonITEMSGetType(hGem) != ITEMTYPE_GEM)
			return FALSE;
		if (!D2CommonITEMSDataTestFlags(hItem, 2048))
			return FALSE;
		nType = D2CommonITEMSGetType(hItem);
		if (!D2CommonITEMSIsWeapon(hItem) && nType != 37 && nType != 2)
			return FALSE;
		hItemInventory = hItem->ptInventory;
		if (D2CommonINVENTORYGetCategoryListCount(hItemInventory) >= D2CommonITEMSGetTypeGemSockets(hItem))
			return FALSE;
		if (D2CommonINVENTORYAddItem(hItemInventory, hGem, 1))
		{
			hGem->dwFlags0 &= ~2;
			if (fClearCursor && hUnit)
				D2CommonINVENTORYSetCursorItem(hUnit->ptInventory, 0);
			ITEMSModeApplyEffects(ptGame, hGem, hItem, FALSE);
			D2CommonStatsUnitRebuildExport(hItem);
			D2CommonUnitSetMode(hGem, 6);
			if (fSend)
			{
				D2CommonITEMSDataSetFlags(hItem, 1, 1);
				if (D2CommonITEMSDataTestFlags(hItem, 16384))
					D2CommonITEMSDataSetFlags(hItem, 16384, 0);
				if (hUnit)
				{
					D2CommonUnitSetChanged(hUnit, 1);
					(void)D2CommonINVENTORYAddDeltaNode(hUnit->ptInventory, hItem);
				}
			}
			return TRUE;
		}
#line 3628
		D2ASSERT(0);
	}
	*pnResult = 1;
	return FALSE;
}

BOOL __fastcall ITEMSModeScrollToTome(GAME *ptGame, UNIT *hUnit, DWORD dwScrollId,
	DWORD dwTomeId, int *pnResult)
{
	UNIT *hScroll;
	UNIT *hTome;
	void *hInventory;
	void *hCursorInventory;
	DWORD dwTomeBook;
	int nCharges;
	CLIENT *ptClient;

	GAMEROOM *hRoom;

#line 3638
	D2ASSERT(ptGame);
#line 3639
	D2ASSERT(hUnit);
	ITEMSModeClearMarkedItems(hUnit);
	*pnResult = 0;
	hScroll = SUnitFind(ptGame, UNIT_ITEM, dwScrollId);
	if (!hScroll || (hScroll->dwMode != 3 && hScroll->dwMode != 4)
		|| D2CommonITEMSGetType(hScroll) != 22)
	{
		*pnResult = 1;
		return FALSE;
	}
	hTome = SUnitFind(ptGame, UNIT_ITEM, dwTomeId);
	if (!hTome || hTome->dwMode != 0 || D2CommonITEMSGetType(hTome) != 18)
	{
		*pnResult = 1;
		return FALSE;
	}
	hInventory = hUnit->ptInventory;
#line 3669
	D2ASSERT(hInventory);

	if (D2CommonITEMSDataGetMagicSuffixAt(hScroll, 0) == D2CommonITEMSDataGetMagicSuffixAt(hTome, 0))
	{
		nCharges = D2CommonStatsUnitGetValue(hTome, STAT_QUANTITY);
		if (nCharges >= (int)D2CommonITEMSGetTypeMaxStack(hTome))
			return FALSE;
		hCursorInventory = hUnit->ptInventory;
		if ((UNIT *)D2CommonINVENTORYGetCursorItem(hCursorInventory) == hScroll)
		{ DWORD dwFlags;
			ptClient = SUnitGetClient(hUnit);
			dwFlags = 0;
			D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
			SCmdSendCursorCleared(ptClient, hUnit);
			D2CommonINVENTORYSetCursorItem(hCursorInventory, 0);
			ITEMSFree(ptGame, hScroll);
		}
		else
		{
			hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hScroll);
			if (hRoom)
			{
				D2CommonDungeonRoomAddNode(hRoom, hScroll->eType, hScroll->dwId);
				D2CommonUnitClearCollision(hScroll);
				D2CommonUnitRoomRemoveUnit(hScroll);
			}
			hScroll->dwFlags0 &= ~2;
			ITEMSFree(ptGame, hScroll);
			D2CommonINVENTORYSetCursorItem(hInventory, 0);
		}
		D2CommonStatsUnitAddValue(hTome, STAT_QUANTITY, 1);

		SCmdSendItemStat(SUnitGetClient(hUnit), hTome, 1, STAT_QUANTITY, D2CommonStatsUnitGetValue(hTome, STAT_QUANTITY));
		ITEMSModeAddBookCharges(hUnit, hTome, 1);
		return TRUE;
	}
#line 3692
	D2ASSERT(0);
	*pnResult = 1;
	return FALSE;
}

BOOL __fastcall ITEMSModePutInCube(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwCubeId, int *pnResult)
{
	UNIT *hItem;
	UNIT *hCube;

#line 3702
	D2ASSERT(ptGame);
#line 3703
	D2ASSERT(hUnit);
	ITEMSModeClearMarkedItems(hUnit);
	*pnResult = 0;
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (hItem->dwMode != 3 && hItem->dwMode != 4)
	{
		*pnResult = 1;
		return FALSE;
	}
	hCube = SUnitFind(ptGame, UNIT_ITEM, dwCubeId);
	if (!hCube)
	{
		*pnResult = 1;
		return FALSE;
	}
	if (hCube->dwMode != 0)
	{
		*pnResult = 1;
		return FALSE;
	}
	D2CommonITEMSDataSetPage(hItem, 3);
	ITEMSModePlaceCursorItem(ptGame, hUnit, hItem->dwId, 0, 0, TRUE, TRUE, NULL);
	return TRUE;
}

BOOL __fastcall ITEMSModeEquipFromGrid(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	BYTE bLoc)
{
	UNIT *hGridItem;
	UNIT *hRemove;
	void *hInventory;
	DWORD dwRecord;
	DWORD dwX;
	DWORD dwY;
	DWORD dwLocIndex;

#line 3747
	D2ASSERT(ptGame);
#line 3748
	D2ASSERT(hUnit);
	ITEMSModeClearMarkedItems(hUnit);
	hGridItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hGridItem || hGridItem->dwMode != 0)
		return FALSE;
	if (!D2CommonITEMSTestFitsInventorySpace(hGridItem, hUnit, 0, 0, 0))
		return FALSE;
	hInventory = hUnit->ptInventory;
	hRemove = (UNIT *)D2CommonINVENTORYRemoveItem(hInventory, hGridItem);
#line 3765
	D2ASSERT(hRemove);
#line 3766
	D2ASSERT(hGridItem == hRemove);
	dwRecord = D2CommonUnitGetInvRecordIndex(hUnit, 0);

	D2CommonINVENTORYRemoveItemAt(hInventory, D2CommonUnitGetX(hGridItem), D2CommonUnitGetY(hGridItem), dwRecord, 0, 0);
	D2CommonITEMSDataSetPage(hGridItem, 255);
	dwLocIndex = bLoc;
	if (D2CommonINVENTORYSetLocSlot(hInventory, hGridItem, dwLocIndex))
	{
		if (D2CommonINVENTORYAddItem(hInventory, hGridItem, 3))
		{
			D2CommonITEMSSetCategory(hGridItem, bLoc);
			D2CommonINVENTORYOfferAddDelta(hInventory, hGridItem);
			D2CommonINVENTORYSetCursorItem(hInventory, 0);
			ITEMSModeApplyEffects(ptGame, hGridItem, hUnit, FALSE);
			ITEMSModeRefreshItems(ptGame, hUnit, 0);
			hGridItem->dwFlags0 &= ~2;
			D2CommonUnitSetMode(hGridItem, 1);
			D2CommonITEMSDataSetPage(hGridItem, 255);
			D2CommonITEMSDataSetCmdFlags(hGridItem, 32768, 1);
			if (D2CommonITEMSGetTypeHasInv(hGridItem)
				&& (DWORD)D2CommonINVENTORYGetCategoryListCount(ITEMSModeGetItemInventory(hGridItem)) > 0)
				D2CommonITEMSDataSetFlags(hGridItem, 1, 1);
			if (D2CommonITEMSDataTestFlags(hGridItem, 16384))
				D2CommonITEMSDataSetFlags(hGridItem, 16384, 0);
			(void)D2CommonINVENTORYAddDeltaNode(hInventory, hGridItem);
			ITEMSModeUpdateWeaponSkills(hUnit);
			D2CommonUnitSetChanged(hUnit, 1);
			ITEMSModeCheckEquipment(ptGame, hUnit, FALSE);
			return TRUE;
		}
		D2CommonINVENTORYClearLocSlot(hInventory, dwLocIndex);
#line 3799
		D2ASSERT(0);
	}
#line 3802
	D2ASSERT(0);
	return FALSE;
}

void __fastcall ITEMSModeEventExpire(GAME *ptGame, UNIT *hUnit)
{
	D2CommonStatsUnitExpireLists(hUnit, ptGame->nFrame);
}

typedef void (__fastcall *PFNITEMEVENT)(GAME *ptGame, UNIT *hUnit);
static const PFNITEMEVENT scpfnEventFunctions[NUM_EVENTS] =
{
	NULL, NULL, NULL, NULL, (PFNITEMEVENT)A2Q2UnitKilled, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	ITEMSModeEventExpire,
};

void __fastcall ITEMSModeRunEvent(GAME *ptGame, UNIT *hUnit, int eEventType)
{
#line 3844
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)scpfnEventFunctions[eEventType]), "scpfnEventFunctions[eEventType]");
	scpfnEventFunctions[eEventType](ptGame, hUnit);
}

BOOL __fastcall ITEMSModeUseKey(GAME *ptGame, UNIT *hUnit)
{
	void *ptNode;
	UNIT *hItem;
	BYTE bPage;
	int nQuantity;
	CLIENT *ptClient;
	DWORD dwFlags;

#line 3853
	D2ASSERT(ptGame);
#line 3854
	D2ASSERT(hUnit);
	{
		ITEMSModeClearMarkedItems(hUnit);
		ptNode = D2CommonINVENTORYGetCategoryListHead(ITEMSModeGetItemInventory(hUnit));
		if (ptNode) while (1)
		{
			hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
			if (D2CommonITEMSGetType(hItem) == 41)
			{
				bPage = D2CommonITEMSDataGetPage(hItem);
				if (!bPage || D2CommonITEMSDataGetPage(hItem) == 3)
				{
					if (D2CommonStatsUnitGetValue(hItem, STAT_QUANTITY) - 1 > 0)
					{
						D2CommonStatsUnitAddValue(hItem, STAT_QUANTITY, -1);
						nQuantity = D2CommonStatsUnitGetValue(hItem, STAT_QUANTITY);
						SCmdSendItemStat(SUnitGetClient(hUnit), hItem, 1, STAT_QUANTITY, nQuantity);
						return TRUE;
					}
					ptClient = SUnitGetClient(hUnit);
					dwFlags = 0;
					D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
					SCmdSendItemFromGrid(ptClient, hUnit, hItem, dwFlags);
					ITEMSModeRemoveGridItem(ptGame, hUnit, hItem, FALSE);
					return TRUE;
				}
			}
			ptNode = (void *)D2CommonINVENTORYNodeGetNext(ptNode);
			if (!ptNode)
				break;
		}
		return FALSE; }
}
#line 3881

struct ITEMMODEDIFFICULTY
{
 	BYTE		unobserved_00[8];
 	int			UberCodeOddsNormal;
 	int			UberCodeOddsGood;
 	BYTE		unobserved_10[32 - 16];
};

BOOL __fastcall ITEMSModeTransmogrify(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	GAMEITEMSTXT *ptOldItemStats;
	GAMEITEMSTXT *ptNewItemStats;
	int nNewClass;
	int i;
	ITEMCREATE tCreate;
	CLIENT *ptClient;
	UNIT *hNewItem;
	DWORD dwRange;
	DWORD dwRand;

#line 3889
	D2ASSERT(ptGame);
#line 3890
	D2ASSERT(hUnit);
#line 3891
	D2ASSERT(hItem);
#line 3892
	D2ASSERT(UnitGetType(hItem) == UNIT_ITEM);
	ptOldItemStats = (GAMEITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
#line 3898
	D2ASSERT(ptOldItemStats);
	if (ptOldItemStats->Transmogrify)
	{
		ptNewItemStats = (GAMEITEMSTXT *)D2CommonItemTblsFindItemByCode(ptOldItemStats->TMogType, &nNewClass);
#line 3903
		D2ASSERT(ptNewItemStats);
		if (hItem->dwMode == 0)
		{
			{
			int nClassId = nNewClass;
			for (i = 0; i < (int)(sizeof(ITEMCREATE) / sizeof(DWORD)); i++)
				((DWORD *)&tCreate)[i] = 0;
			tCreate.ptGame = ptGame;
			tCreate.nLevel = D2CommonITEMSGetLevelId(hItem);
			tCreate.dwMode = 4;
			tCreate.dwClassId = nClassId;
			tCreate.nX = D2CommonUnitGetX(hItem);
			tCreate.nY = D2CommonUnitGetY(hItem);
			tCreate.ptSeed = &hItem->tSeed;
			tCreate.wCreateFlags = 1;
			tCreate.dwQuality = 2;
			tCreate.dwSeed = hItem->dwInitSeed;
			tCreate.dwItemSeed = D2CommonITEMSGetItemSeedValue(hItem);
			tCreate.dwFileIndex = 2;
			}
			{
			DWORD dwFlags;
			ptClient = SUnitGetClient(hUnit);
			dwFlags = 0;
			D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
			SCmdSendItemFromGrid(ptClient, hUnit, hItem, dwFlags);
			}
			ITEMSModeRemoveGridItem(ptGame, hUnit, hItem, FALSE);
			hNewItem = ITEMSCreateFromRecord(ptGame, &tCreate, 0);
#line 3909
			D2ASSERT(hNewItem);
			if (ptOldItemStats->TMogMin > 0 && ptOldItemStats->TMogMax > 0)
			{
				dwRange = ptOldItemStats->TMogMax - ptOldItemStats->TMogMin;
				if (dwRange)
					dwRand = D2CommonSeedRand(&hUnit->tSeed, dwRange);
				else
					dwRand = 0;
				D2CommonStatsUnitSetValue(hNewItem, STAT_QUANTITY, ptOldItemStats->TMogMin + dwRand);
			}
			else
				D2CommonStatsUnitSetValue(hNewItem, STAT_QUANTITY, 0);
			D2CommonITEMSDataSetPage(hNewItem, 0);
			if (!ITEMSModePlaceCursorItem(ptGame, hUnit, hNewItem->dwId, tCreate.nX, tCreate.nY, FALSE, TRUE,
				NULL)
				&& !ITEMSModePlaceCursorItem(ptGame, hUnit, hNewItem->dwId, 0, 0, TRUE, TRUE, NULL))
				ITEMSModeDropCursorItem(ptGame, hUnit, hNewItem->dwId);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL __fastcall ITEMSModeEquipFromGround(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	void *hInventory;
	int nLoc;

	hInventory = hUnit->ptInventory;
	if (!(!ITEMSModeFindBodyLoc(hUnit, hItem, &nLoc)))
	{
		if (D2CommonINVENTORYSetLocSlot(hInventory, hItem, nLoc))
		{
			if (D2CommonINVENTORYAddItem(hInventory, hItem, 3))
			{
				D2CommonITEMSSetCategory(hItem, nLoc);
				D2CommonINVENTORYOfferAddDelta(hInventory, hItem);
				D2CommonINVENTORYSetCursorItem(hInventory, 0);
				ITEMSModeApplyEffects(ptGame, hItem, hUnit, FALSE);
				ITEMSModeRefreshItems(ptGame, hUnit, 0);
				hItem->dwFlags0 &= ~2;
				D2CommonUnitSetMode(hItem, 1);
				D2CommonITEMSDataSetCmdFlags(hItem, 512, 1);
				hItem->dwFlags0 &= ~33554432;
				(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
				D2CommonUnitSetChanged(hUnit, 1);
				D2CommonITEMSDataSetPage(hItem, 255);
				ITEMSModeUpdateWeaponSkills(hUnit);
				ITEMSModeApplySetBonus(ptGame, hUnit, hItem);
				return TRUE;
			}
			D2CommonINVENTORYClearLocSlot(hInventory, nLoc);
#line 3972
			D2ASSERT(0);
		}
#line 3975
		D2ASSERT(0);
		return FALSE;
	}
	return FALSE;
}

BOOL __fastcall ITEMSTransferDeadBodyItemsToPlayer(GAME *ptGame, UNIT *hPlayer, UNIT *hDeadBody)
{
	void *hBodyInventory;
	void *hInventory;
	BOOL fAll;
	BOOL fMoved;
	void *ptNode;
	void *ptNext;
	UNIT *hItem;
	UNIT *hRemove;
	GAMEROOM *hRoom;
	COLPOINT tFrom;
	COLPOINT tTo;

#line 3987
	D2ASSERT(ptGame);
#line 3988
	D2ASSERT(hDeadBody);
#line 3989
	D2ASSERT(hPlayer);
	fAll = TRUE;
	hBodyInventory = hDeadBody->ptInventory;
	hInventory = hPlayer->ptInventory;
	for (ptNode = D2CommonINVENTORYGetCategoryListHead(hBodyInventory); ptNode; ptNode = ptNext)
	{
		ptNext = (void *)D2CommonINVENTORYNodeGetNext(ptNode);
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
		D2CommonITEMSDataSetFlags(hItem, 1, 1);
		fMoved = TRUE;
		if (!ITEMSModeCanTakeQuestItem(ptGame, hPlayer, hItem))
		{
			D2CommonUnitGetPosition(hPlayer, &tFrom);
			hRoom = ITEMSFindDropSpot((GAMEROOM *)D2CommonUnitGetPathRoom(hPlayer), &tFrom, &tTo, 1);
			if (!hRoom)
			{
#line 4018
				D2WARN("lost item");
			}
			else
			{
				D2CommonUnitInitPosition(0, hItem, hRoom, tTo.nX, tTo.nY);
				D2CommonUnitSetCollision(hItem, hRoom, tTo.nX, tTo.nY);
				D2CommonAddUnitToRoom(hItem, hRoom);
				hItem->dwFlags0 |= 2;
			}
			D2CommonITEMSDataSetPage(hItem, 255);
			hItem->dwFlags0 |= 4096;
			D2CommonUnitSetMode(hItem, 3);
			QuestsItemDropped(ptGame, hPlayer, hItem);
		}
		else if (!ITEMSModeEquipFromGround(ptGame, hPlayer, hItem)
			&& !ITEMSModeStackOntoInventory(ptGame, hPlayer, hItem))
		{
			if (D2CommonITEMSGetTypeBelt(hItem) && D2CommonINVENTORYAddBeltItem(hInventory, hItem))
			{
				if (D2CommonINVENTORYAddItem(hInventory, hItem, 2))
				{
					D2CommonINVENTORYSetCursorItem(hInventory, 0);
					ITEMSModeAddItemSkill(hPlayer, hItem);
					hItem->dwFlags0 &= ~2;
					D2CommonUnitSetMode(hItem, ITEM_MODE_INVBELT);
					hItem->dwFlags0 &= ~33554432;
					D2CommonITEMSDataSetCmdFlags(hItem, 8192, 1);
					D2CommonITEMSDataSetPage(hItem, 255);
					(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
					D2CommonUnitSetChanged(hPlayer, 1);
				}
				else
				{
#line 4062
					D2FATAL("ITEMSTransferDeadBodyItemsToPlayer => INVENTORYAddItem() failed!");
				}
			}
			else if (!ITEMSModePlaceInGrid(ptGame, hPlayer, hItem, FALSE, 0))
			{
				fAll = FALSE;
				fMoved = FALSE;
			}
		}
		if (fMoved)
		{
			hRemove = (UNIT *)D2CommonINVENTORYRemoveItem(hBodyInventory, hItem);
#line 4088
			D2ASSERT(hRemove);
#line 4089
			D2ASSERT(hItem == hRemove);
		}
		else
		{
			D2CommonITEMSDataSetFlags(hItem, 1, 0);
		}
	}
	return fAll;
}

void __fastcall ITEMSModeRefreshItems(GAME *ptGame, UNIT *hUnit, BOOL fSend)
{
	int nSkillRight;
	int nSkillLeft;
	void *hInventory;
	void *hSkill;
	void *ptNode;
	void *ptNext;
	UNIT *hItem;

	nSkillRight = -1;
	nSkillLeft = -1;
	hInventory = hUnit->ptInventory;
	if (hUnit->eType == UNIT_PLAYER)
	{
		PlayerPetsLock(hUnit);
		hSkill = (void *)D2CommonUnitGetLeftSkill(hUnit);
		if (hSkill)
			nSkillRight = D2CommonSkillsGetNodeType(hSkill);
		hSkill = (void *)D2CommonUnitGetRightSkill(hUnit);
		if (hSkill)
			nSkillLeft = D2CommonSkillsGetNodeType(hSkill);
	}
	for (ptNode = D2CommonINVENTORYGetCategoryListHead(hInventory); ptNode; ptNode = ptNext)
	{
		ptNext = (void *)D2CommonINVENTORYNodeGetNext(ptNode);
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
		if (D2CommonINVENTORYNodeGetCategory(ptNode) == 3 && D2CommonITEMSTestEffectsActive(hItem))
		{
#line 4139
			D2ASSERT(hItem);
			ITEMSModeRemoveEffects(ptGame, hItem, hUnit, FALSE, FALSE);
		}
	}
	for (ptNode = D2CommonINVENTORYGetCategoryListHead(hInventory); ptNode; ptNode = ptNext)
	{
		ptNext = (void *)D2CommonINVENTORYNodeGetNext(ptNode);
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
		if (D2CommonINVENTORYNodeGetCategory(ptNode) == 3 && D2CommonITEMSTestEffectsActive(hItem))
		{
#line 4158
			D2ASSERT(hItem);
			ITEMSModeApplyEffects(ptGame, hItem, hUnit, FALSE);
		}
	}
	D2CommonStatsUnitClampToMax(hUnit);
	if (fSend)
		SCmdSendItemsRefreshed(SUnitGetClient(hUnit), hUnit);
	if (hUnit->eType == UNIT_PLAYER)
	{
		PlayerPetsUnlock(hUnit);
		if (nSkillRight != -1)
			SkillsSetCurrent(hUnit, TRUE, nSkillRight);
		if (nSkillLeft != -1)
			SkillsSetCurrent(hUnit, FALSE, nSkillLeft);
	}
}

BOOL __fastcall ITEMSModeHasContents(UNIT *hItem)
{
	if (D2CommonITEMSGetTypeHasInv(hItem) && (DWORD)D2CommonINVENTORYGetCategoryListCount(ITEMSModeGetItemInventory(hItem)) > 0)
		return TRUE;
	return FALSE;
}

BOOL __fastcall ITEMSModeCopySockets(GAME *ptGame, UNIT *hUnit, UNIT *hSource,
	UNIT *hDest, BOOL fSend)
{
	void *pInventory;
	void *ptNode;
	UNIT *hGem;
	UNIT *hNewItem;
	int nResult;
	ITEMCREATE tCreate;

	if (!D2CommonITEMSGetTypeHasInv(hSource))
		return TRUE;
	pInventory = hSource->ptInventory;
	for (ptNode = D2CommonINVENTORYGetCategoryListHead(pInventory); ptNode;
		ptNode = (void *)D2CommonINVENTORYNodeGetNext(ptNode))
	{
		hGem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
		ITEMSFillCreate(ptGame, &tCreate, hGem, 0);
		hNewItem = ITEMSRecreate(ptGame, &tCreate);
		D2CommonUnitSetMode(hNewItem, 4);
		D2CommonUnitSetX(hNewItem, D2CommonUnitGetX(hGem));
		D2CommonUnitSetY(hNewItem, D2CommonUnitGetY(hGem));
		if (!ITEMSModeSocketGem(ptGame, hUnit, hNewItem->dwId, hDest->dwId, &nResult, fSend, FALSE,
			TRUE))
			return FALSE;
	}
	return TRUE;
}

static __inline int ITEMSModeAtLeastOneI(int n)
{
	if (n < 1)
		return 1;
	else
		return n;
}

UNIT * __fastcall ITEMSModeFindTomeFor(GAME *ptGame, UNIT *hUnit, UNIT *hScroll,
	BOOL fTestOnly)
{
	void *ptNode;
	void *ptNext;
	UNIT *hTome;
	int i;
	int nCount;
	int nCharges;

	if (D2CommonITEMSGetType(hScroll) != 22)
		return NULL;
	for (ptNode = D2CommonINVENTORYGetCategoryListHead(hUnit->ptInventory); ptNode; ptNode = ptNext)
	{
		ptNext = (void *)D2CommonINVENTORYNodeGetNext(ptNode);
		hTome = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
		if (D2CommonITEMSGetType(hTome) != 18 || D2CommonITEMSDataGetPage(hTome))
			continue;
		for (i = 0; i < 3; i++)
		{
			if (D2CommonITEMSDataGetMagicPrefixAt(hTome, i) != D2CommonITEMSDataGetMagicPrefixAt(hScroll, i))
				goto lNext;
			if (D2CommonITEMSDataGetMagicSuffixAt(hTome, i) != D2CommonITEMSDataGetMagicSuffixAt(hScroll, i))
				goto lNext;
		}
		nCount = ITEMSModeAtLeastOneI(D2CommonStatsUnitGetValue(hScroll, STAT_QUANTITY));
		nCharges = D2CommonStatsUnitGetValue(hTome, STAT_QUANTITY);
		if (nCharges + nCount > (int)D2CommonITEMSGetTypeMaxStack(hTome))
			continue;
		if (fTestOnly)
			return hTome;
		D2CommonStatsUnitSetValue(hTome, STAT_QUANTITY, nCharges + nCount);
		SCmdSendItemStat(SUnitGetClient(hUnit), hTome, 1, STAT_QUANTITY, nCharges + nCount);
		ITEMSModeAddBookCharges(hUnit, hTome, nCount);
		return hTome;
lNext:
		;
	}
	return NULL;
}

void __fastcall ITEMSModeBreakItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	void *hInventory;

	hInventory = hUnit->ptInventory;
	D2CommonITEMSDataSetFlags(hItem, 256, 1);
	D2CommonINVENTORYOfferRemoveDelta(hUnit->ptInventory, hItem);
	if (D2CommonUnitGetAnimMode(hItem) == 1)
		ITEMSModeRemoveEffects(ptGame, hItem, hUnit, TRUE, TRUE);
	ITEMSModeCheckEquipment(ptGame, hUnit, FALSE);
	(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
	D2CommonUnitSetChanged(hUnit, 1);
	D2CommonStatsUnitSetValue(hItem, 72, 0);
	SCmdSendItemStat(SUnitGetClient(hUnit), hItem, 1, 72, 0);
	SUnitSetEvent(hUnit, 9, hUnit);
}

void __fastcall ITEMSModeRepairItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	void *hInventory;
	int nDurability;

	hInventory = NULL;
	if (hUnit)
		hInventory = hUnit->ptInventory;
	D2CommonITEMSDataSetFlags(hItem, 512, 1);
	D2CommonITEMSDataSetFlags(hItem, 256, 0);
	if (hInventory)
	{
		D2CommonINVENTORYOfferAddDelta(hUnit->ptInventory, hItem);
		if (D2CommonUnitGetAnimMode(hItem) == 1)
			ITEMSModeApplyEffects(ptGame, hItem, hUnit, TRUE);
		ITEMSModeCheckEquipment(ptGame, hUnit, FALSE);
		(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
		D2CommonUnitSetChanged(hUnit, 1);
	}
	nDurability = D2CommonStatsUnitCalcMaxDurability(hItem);
	if (nDurability > 0)
	{
		D2CommonStatsUnitSetValue(hItem, 72, nDurability);
		if (hUnit)
			SCmdSendItemStat(SUnitGetClient(hUnit), hItem, 1, 72, nDurability);
	}
	if (hUnit)
		ITEMSModeUpdateWeaponSkills(hUnit);
}

void __fastcall ITEMSModeResetBookSkills(GAME *ptGame, UNIT *hUnit)
{
	int nBooks;
	void *ptNode;
	void *ptNext;
	UNIT *hItem;
	int nSkill;
	void *hSkill;

	for (ptNode = D2CommonINVENTORYGetCategoryListHead(hUnit->ptInventory), nBooks = 0; ptNode; ptNode = ptNext)
	{
		ptNext = (void *)D2CommonINVENTORYNodeGetNext(ptNode);
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
		if (hItem->dwMode != 0)
			continue;
		nSkill = ITEMSModeGetBookSkill(hItem);
		if (nSkill != -1)
		{
			D2CommonStatsUnitGetValue(hItem, STAT_QUANTITY);
			hSkill = (void *)D2CommonSkillsFindByKey(hUnit, nSkill);
			if (hSkill)
				D2CommonSkillsGetNodeCount(hSkill);
			D2CommonSkillsSetNodeCount(hSkill, 0);
			SCmdSendSkillState(SUnitGetClient(hUnit), (BYTE)hUnit->eType, hUnit->dwId, (WORD)nSkill, 0);
		}
		if (!D2CommonITEMSDataGetPage(hItem))
			nBooks++;
	}
	if (!nBooks)
		return;
	for (ptNode = D2CommonINVENTORYGetCategoryListHead(hUnit->ptInventory); ptNode; ptNode = ptNext)
	{
		ptNext = (void *)D2CommonINVENTORYNodeGetNext(ptNode);
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
		if (hItem->dwMode != 0 || D2CommonITEMSDataGetPage(hItem))
			continue;
		ITEMSModeUpdateBookSkill(hUnit, hItem, TRUE);
		if (!--nBooks)
			return;
	}
}

BOOL __fastcall ITEMSModeSetHandItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
#line 4430
	D2ASSERT(hUnit);
#line 4431
	D2ASSERT(hItem);
	D2CommonUnitSetMode(hItem, 4);
	D2CommonITEMSDataSetCmdFlags(hItem, ITEM_CMDFLAG_SETHANDITEM, 1);
	(void)D2CommonINVENTORYAddDeltaNode(hUnit->ptInventory, hItem);
	D2CommonUnitSetChanged(hUnit, 1);
	return TRUE;
}

struct ITEMCODELIST
{
 	BYTE		bCount;
 	BYTE		unobserved_01[3];
 	DWORD		adwCode[1];
};

DWORD __fastcall ITEMSModeDrawCode(GAME *ptGame, ITEMCODELIST *ptList, UNIT *hUnit,
	BOOL fGood)
{
	DWORD dwIndex;
	DWORD dwCode;
	int nClass;
	GAMEITEMSTXT *ptItemStats;
	ITEMMODEDIFFICULTY *ptDifficulty;
	int nOdds;
	D2SEED *ptSeed;
	int nCount;

	ptSeed = &hUnit->tSeed;
	nCount = ptList->bCount;
	if (nCount)
		dwIndex = D2CommonSeedRand(ptSeed, nCount);
	else
		dwIndex = 0;
	dwCode = ptList->adwCode[dwIndex];
	ptItemStats = (GAMEITEMSTXT *)D2CommonItemTblsFindItemByCode(dwCode, &nClass);
	if (!ptItemStats || ptItemStats->ubercode == 538976304 || !ptItemStats->ubercode)
		return dwCode;
	ptDifficulty = (ITEMMODEDIFFICULTY *)D2CommonDataTblsGetDifficultyLevel(ptGame->bDifficultyLevel);
	if (fGood)
		nOdds = ptDifficulty->UberCodeOddsGood;
	else
		nOdds = ptDifficulty->UberCodeOddsNormal;
	if ((int)D2CommonSeedRand(ptSeed, 100) < nOdds)
		dwCode = ptItemStats->ubercode;
	return dwCode;
}

void __fastcall ITEMSModeUnequipAll(GAME *ptGame, UNIT *hUnit)
{
	void *hInventory;
	void *ptNode;
	UNIT *hInvItem;
	UNIT *hRemove;
	BOOL fSetComplete;

	if (!hUnit)
		return;
	hInventory = hUnit->ptInventory;
	if (!hInventory)
		return;
	for (ptNode = D2CommonINVENTORYGetCategoryListHead(hInventory); ptNode;
		ptNode = (void *)D2CommonINVENTORYNodeGetNext(ptNode))
	{
		if (D2CommonINVENTORYNodeGetCategory(ptNode) != 3)
			continue;
		hInvItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
#line 4502
		D2ASSERT(hInvItem);
		fSetComplete = D2CommonITEMSTestSetComplete(hUnit, hInvItem);
		D2CommonINVENTORYOfferRemoveDelta(hInventory, hInvItem);
		hRemove = (UNIT *)D2CommonINVENTORYRemoveItem(hInventory, hInvItem);
#line 4506
		D2ASSERT(hRemove);
#line 4507
		D2ASSERT(hInvItem == hRemove);
		D2CommonINVENTORYClearLocSlot(hInventory, D2CommonINVENTORYFindSlotIndex(hInventory, hInvItem));
		ITEMSModeRemoveEffects(ptGame, hInvItem, hUnit, FALSE, TRUE);
		ITEMSModeRefreshItems(ptGame, hUnit, 0);
		if (fSetComplete)
			ITEMSModeRemoveSetBonus(hUnit, hInvItem);
		ITEMSModeCheckEquipment(ptGame, hUnit, FALSE);
	}
	D2CommonINVENTORYSoftReset(hInventory);
}
