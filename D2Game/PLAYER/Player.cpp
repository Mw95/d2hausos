#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\PLAYER\\Player.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Arena.h"
#include "../GAME/Clients.h"
#include "../GAME/Event.h"
#include "../GAME/SCmd.h"
#include "../ITEMS/ItemMode.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/MonsterAI.h"
#include "../OBJECTS/ObjMode.h"
#include "Player.h"
#include "PlayerPets.h"
#include "PlrIntro.h"
#include "PlrModes.h"
#include "PlrTrade.h"
#include "../UNIT/Party.h"
#include "../UNIT/PlayerList.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "../UNIT/SUNITNPC.h"

#define UNIT_PLAYER				0
#define UNIT_MONSTER			1
#define UNIT_OBJECT				2
#define UNIT_MISSILE			3
#define UNIT_ITEM				4
#define ITEMTYPE_GOLD			4
#define ITEMSGetType(hItem)		D2CommonITEMSGetTypeRaw((DWORD)(hItem))

#define STAT_LEVEL				12
#define STAT_EXPERIENCE			13
#define STAT_GOLD				14
#define STAT_GOLDBANK			15
#define STAT_QUANTITY			70
#define STAT_DURABILITY			72
#define STAT_GOLDLOST			175
#define OBJECT_PORTAL			59
#define PLAYER_MAX_GOLD_DROP	2000000000
#define ITEMCODE_WAND			543452791
#define ITEMCODE_SHORTSTAFF		544502643
#define ITEMCODE_BOX			544763746
#define PLAYER_NUM_ACTS			4
#define PLAYER_NUM_CLASSES		5
#define PLAYER_NUM_INITITEMS	10

void __fastcall ArenaUnitNew(GAME *ptGame, UNIT *hUnit);
PLRINTRO * __fastcall PlrIntroNew(GAME *ptGame);

void __fastcall PlayerCountAliveEach(GAME *ptGame, UNIT *hPlayer, void *pArg);
UNIT * __fastcall PlayerCreateStartItem(GAME *ptGame, UNIT *hPlayer, DWORD dwCode, DWORD dwClass);
void __fastcall PlayerAddActItems(UNIT *hPlayer, GAME *ptGame, BYTE bAct);

static DWORD sgdwWandSeed = 0xFFFFFFFF, sgdwWandItemSeed = 0xFFFFFFFF;
static DWORD sgdwStaffSeed = 0xFFFFFFFF, sgdwStaffItemSeed = 0xFFFFFFFF;

static INITITEM sgatPlayerActItems[PLAYER_NUM_ACTS][PLAYER_NUM_CLASSES][PLAYER_NUM_INITITEMS] =
{
	{
		{
			{ 544633185, 5, 1 }, { 544694883, 4, 1 }, { 544631911, 0, 1 }, { 543974259, 3, 1 },
			{ 544042088, 1, 1 }, { 544498285, 9, 1 }, { 543973997, 8, 1 }, { 543975277, 10, 1 },
			{ 543389026, 0, 1 }, { 544633185, 0, 1 },
		},
		{
			{ 544106595, 3, 1 }, { 544238451, 1, 1 }, { 543973996, 8, 1 }, { 543975276, 10, 1 },
			{ 544498284, 9, 1 }, { 544502643, 4, 1 }, { 0 }, { 0 },
			{ 0 }, { 0 },
		},
		{
			{ 544106595, 3, 1 }, { 544238451, 1, 1 }, { 543973996, 8, 1 }, { 543975276, 10, 1 },
			{ 544498284, 9, 1 }, { 543454067, 4, 1 }, { 543389026, 5, 1 }, { 0 },
			{ 0 }, { 0 },
		},
		{
			{ 544367219, 5, 1 }, { 544500075, 4, 1 }, { 543649394, 3, 1 }, { 543973997, 8, 1 },
			{ 544498285, 9, 1 }, { 543975276, 10, 1 }, { 543388532, 0, 2 }, { 543388521, 0, 2 },
			{ 544042088, 1, 1 }, { 0 },
		},
		{
			{ 543649394, 3, 1 }, { 544238451, 1, 1 }, { 543973996, 8, 1 }, { 543975276, 10, 1 },
			{ 544498284, 9, 1 }, { 543520865, 4, 1 }, { 543520865, 5, 1 }, { 0 },
			{ 0 }, { 0 },
		},
	},
	{
		{
			{ 544633185, 5, 1 }, { 543318643, 4, 1 }, { 543781492, 0, 1 }, { 543977587, 3, 1 },
			{ 543975526, 1, 1 }, { 544498285, 9, 1 }, { 543974004, 8, 1 }, { 543975284, 10, 1 },
			{ 543650412, 0, 1 }, { 544633185, 0, 1 },
		},
		{
			{ 543649394, 3, 1 }, { 544042088, 1, 1 }, { 543974004, 8, 1 }, { 543975277, 10, 1 },
			{ 544498285, 9, 1 }, { 544502626, 4, 1 }, { 0 }, { 0 },
			{ 0 }, { 0 },
		},
		{
			{ 543649394, 3, 1 }, { 544042088, 1, 1 }, { 543974004, 8, 1 }, { 543975277, 10, 1 },
			{ 544498285, 9, 1 }, { 543781483, 4, 1 }, { 543650412, 5, 1 }, { 543324269, 0, 1 },
			{ 544633187, 0, 1 }, { 544633187, 0, 1 },
		},
		{
			{ 543454050, 5, 1 }, { 544500075, 4, 1 }, { 543977587, 3, 1 }, { 543974004, 8, 1 },
			{ 544498292, 9, 1 }, { 543975284, 10, 1 }, { 543388532, 0, 2 }, { 543388521, 0, 2 },
			{ 544042088, 1, 1 }, { 0 },
		},
		{
			{ 544500848, 3, 1 }, { 543975526, 1, 1 }, { 543974004, 8, 1 }, { 543975284, 10, 1 },
			{ 544498292, 9, 1 }, { 543780973, 4, 1 }, { 544760114, 5, 1 }, { 0 },
			{ 0 }, { 0 },
		},
	},
	{
		{
			{ 544633185, 5, 1 }, { 544694883, 4, 1 }, { 544631911, 0, 1 }, { 543974259, 3, 1 },
			{ 544042088, 1, 1 }, { 544498285, 9, 1 }, { 543973997, 8, 1 }, { 543975277, 10, 1 },
			{ 543389026, 0, 1 }, { 544633185, 0, 1 },
		},
		{
			{ 543649394, 3, 1 }, { 544238451, 1, 1 }, { 543973996, 8, 1 }, { 543975276, 10, 1 },
			{ 544498284, 9, 1 }, { 544502643, 4, 1 }, { 0 }, { 0 },
			{ 0 }, { 0 },
		},
		{
			{ 543253868, 3, 1 }, { 544238451, 1, 1 }, { 543973996, 8, 1 }, { 543975276, 10, 1 },
			{ 544498284, 9, 1 }, { 543454067, 4, 1 }, { 543389026, 5, 1 }, { 0 },
			{ 0 }, { 0 },
		},
		{
			{ 543454060, 5, 1 }, { 544500075, 4, 1 }, { 543649394, 3, 1 }, { 543973997, 8, 1 },
			{ 544498285, 9, 1 }, { 543975276, 10, 1 }, { 543388532, 0, 2 }, { 543388521, 0, 2 },
			{ 544042088, 1, 1 }, { 0 },
		},
		{
			{ 544568435, 3, 1 }, { 544238451, 1, 1 }, { 543973996, 8, 1 }, { 543975276, 10, 1 },
			{ 544498284, 9, 1 }, { 543454067, 4, 1 }, { 544760168, 5, 1 }, { 0 },
			{ 0 }, { 0 },
		},
	},
	{
		{ { 0 } },
	},
};

void __fastcall PlayerInitUnit(GAME *ptGame, UNIT *hUnit  , DWORD dwUnitId)
{
	void *hInventory;
	PLRINFO *ptPlrInfo;
	int i;

#line 70
	D2ASSERT(hUnit);
	hUnit->dwId = dwUnitId;
	hUnit->dwFlags0 |= 14;
	hInventory = (void *)D2CommonINVENTORYCreate(ptGame->pMemPool, dwUnitId, hUnit->eType,
		hUnit->dwClassId, 0);
#line 77
	D2ASSERT(hInventory);
	hUnit->ptInventory = hInventory;
	hUnit->field_c8 = (void *)D2CommonSkillsAllocNode(ptGame->pMemPool);
	D2CommonSkillsRebuildCaches(hUnit);
	D2CommonStatsUnitAllocStats(hUnit, 0);
	D2CommonUnitCreatePlrInfo(hUnit);
	ptPlrInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
	if (ptPlrInfo)
	{
		for (i = 0; i < 3; i++)
			ptPlrInfo->ptIntro[i] = PlrIntroNew(ptGame);
		ptPlrInfo->dwBoughtItemId = 0;
	}
	SUnitSetClient(hUnit, 0);
	EventUnitInit(ptGame, hUnit);
	ArenaUnitNew(ptGame, hUnit);
	PlayerPetsNew(ptGame, hUnit);
	SUnitSetAlignment(hUnit, 0);
}

void __fastcall PlayerFreeUnit(GAME *ptGame, UNIT *hPlayer)
{
	void *ptNode;
	UNIT *hUnit;
	GAMEROOM *hRoom;
	PLRINFO *ptPlrInfo;
	HPTRACK *ptTrack;
	HPTRACK *ptNext;
	PLRINTRO **pptIntro;
	int i;

	PlayerPetsFree(ptGame, hPlayer);
	PartyRemovePlayer(ptGame, hPlayer);
	PlayerListFree(ptGame, hPlayer);
	SUnitDmgFreeHeld(ptGame, hPlayer);
	for (ptNode = D2CommonINVENTORYGetTrackedListHead(hPlayer->ptInventory); ptNode;
		ptNode = (void *)D2CommonINVENTORYNodeGetTrackedNext(ptNode))
	{
		hUnit = SUnitFind(ptGame, UNIT_PLAYER, D2CommonINVENTORYNodeGetTrackedKey(ptNode));
		if (hUnit)
		{
			hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
			if (hRoom)
				D2CommonDungeonRoomAddNode(hRoom, hUnit->eType, hUnit->dwId);
			SUnitFree(ptGame, hUnit);
		}
	}
	SUnitFreeInventory(ptGame, hPlayer);
	D2CommonINVENTORYDestroy(hPlayer->ptInventory);
	hPlayer->ptInventory = NULL;
	D2CommonStatsUnitFreeStats(hPlayer);
	D2CommonSkillsFreeList(hPlayer);
	ArenaUnitFree(ptGame, hPlayer);
	ptPlrInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
	if (ptPlrInfo)
	{
		for (ptTrack = ptPlrInfo->ptHpTrack; ptTrack; ptTrack = ptNext)
		{
			ptNext = ptTrack->ptNext;
#line 42
			D2FREEPOOL(ptGame->pMemPool, ptTrack);
		}
		ptPlrInfo->ptHpTrack = NULL;
		pptIntro = ptPlrInfo->ptIntro;
		for (i = 3; i; i--, pptIntro++)
		{
			PlrIntroFree(ptGame, *pptIntro);
			*pptIntro = NULL;
		}
	}
	D2CommonUnitFreePlrInfo(ptGame->pMemPool, hPlayer);
	if (UNIT5(hPlayer)->ptChatNode)
		D2CommonChatNodeFree(ptGame->pMemPool, UNIT5(hPlayer)->ptChatNode);
	EventUnitFree(ptGame, hPlayer);
}

void __fastcall PlayerFreeAll(GAME *ptGame)
{
	UNIT *hUnit;
	UNIT *hNext;
	int i;

	for (i = 0; i < 128; i++)
	{
		for (hUnit = ptGame->pUnitList[UNIT_PLAYER][i]; hUnit; )
		{
			hNext = hUnit;
			hUnit = SUnitGetNextInList(hNext);
			SUnitFree(ptGame, hNext);
		}
	}
}

void __fastcall PlayerClearTarget(GAME *ptGame, UNIT *hUnit, UNIT *hTarget)
{
	if (hTarget == SUnitGetTarget(ptGame, hUnit))
		D2CommonUnitSetTarget(hUnit, 0);
}

void __fastcall PlayerStartMode(GAME *ptGame, UNIT *hUnit, DWORD dwX, DWORD dwY)
{
	PlrModeStartXY(ptGame, hUnit, NULL, 1, (int)dwX, (int)dwY, TRUE);
	EventAddPersistent(ptGame, hUnit, 3);
	EventAddPersistent(ptGame, hUnit, 4);
	EventAdd(ptGame, hUnit, 11, ptGame->nFrame + 250);
}

static inline BOOL PlayerItemToInventory(GAME *ptGame, UNIT *hPlayer  ,
	UNIT *hItem  )
{
#line 184
	D2ASSERT(hItem);
#line 185
	D2ASSERT(hPlayer);
	D2CommonITEMSDataSetPage(hItem, 0);
	return ITEMSModePlaceCursorItem(ptGame, hPlayer, hItem->dwId, D2CommonUnitGetX(hItem),
		D2CommonUnitGetY(hItem), TRUE, TRUE, NULL);
}

static inline BOOL PlayerEquipNewItem(GAME *ptGame, UNIT *hPlayer  ,
	UNIT *hItem  )
{
	int nResult;

#line 202
	D2ASSERT(hItem);
#line 203
	D2ASSERT(hPlayer);
	return ITEMSModeEquipCursorItem(ptGame, hPlayer, hItem->dwId,
		D2CommonITEMSGetCategoryRaw((DWORD)hItem), TRUE, &nResult);
}

static inline BOOL PlayerItemToBelt(GAME *ptGame, UNIT *hPlayer  ,
	UNIT *hItem  )
{
	int nResult;

#line 220
	D2ASSERT(hItem);
#line 221
	D2ASSERT(hPlayer);
	return ITEMSModePutInBelt(ptGame, hPlayer, hItem->dwId, D2CommonUnitGetX(hItem), TRUE,
		&nResult);
}

static inline void PlayerItemFill(UNIT *hItem)
{
	if (D2CommonITEMSGetType(hItem) == 5)
		D2CommonStatsUnitSetValue(hItem, STAT_QUANTITY, 250);
	else
		D2CommonStatsUnitSetValue(hItem, STAT_DURABILITY, D2CommonStatsUnitCalcMaxDurability(hItem));
}

void __fastcall PlayerAddStartItem(GAME *ptGame, UNIT *hPlayer,
	INITITEM *ptInitItem  )
{
	UNIT *hItem;
	DWORD dwClass;
	int i;

#line 678
	D2ASSERT(ptInitItem);
	if (!D2CommonItemTblsFindItemByCode(ptInitItem->dwCode, &dwClass))
		return;
	for (i = 0; i < ptInitItem->bCount; i++)
	{
		hItem = PlayerCreateStartItem(ptGame, hPlayer, ptInitItem->dwCode, dwClass);
		D2CommonITEMSDataSetFlags(hItem, 131072, 1);
		D2CommonITEMSSetCategory(hItem, ptInitItem->bLoc);
		if (D2CommonITEMSGetTypeBelt(hItem))
		{
			if (!PlayerItemToBelt(ptGame, hPlayer, hItem) && !PlayerItemToInventory(ptGame, hPlayer, hItem))
			{
#line 690
				D2ASSERT(0);
			}
		}
		else if (ptInitItem->bLoc)
		{
			if (!PlayerEquipNewItem(ptGame, hPlayer, hItem) && !PlayerItemToInventory(ptGame, hPlayer, hItem))
			{
#line 696
				D2ASSERT(0);
			}
			PlayerItemFill(hItem);
		}
		else
		{
			PlayerItemToInventory(ptGame, hPlayer, hItem);
			PlayerItemFill(hItem);
			if (D2CommonITEMSGetType(hItem) == 5)
				D2CommonStatsUnitSetValue(hItem, STAT_QUANTITY, 100);
		}
	}
}

static __inline UNIT *PlayerSeededStartItem(GAME *ptGame, UNIT *hPlayer, DWORD dwClass, int nLevel,
	DWORD *pdwItemSeed, DWORD *pdwSeed, int nWantedTab)
{
	UNIT *hNewItem;

	while (*pdwSeed != 0xFFFFFFFF || *pdwItemSeed != 0xFFFFFFFF)
	{
		int nTab;
		int nSkillLevel;
		int nSkill;
		int nStat108;
		int nStat109;

		hNewItem = ITEMSCreate(hPlayer, dwClass, ptGame, 4, 2, 1, nLevel, TRUE, *pdwSeed, *pdwItemSeed);
		nSkill = D2CommonStatsGetTotalValue(hNewItem, 107);
		nStat108 = D2CommonStatsGetTotalValue(hNewItem, 108);
		nStat109 = D2CommonStatsGetTotalValue(hNewItem, 109);
		D2CommonLoggingUnpackDword(nSkill, &nTab, &nSkillLevel);
		if (nTab == nWantedTab && nSkillLevel == 1 && !nStat108 && !nStat109)
			return hNewItem;
		*pdwSeed = 0xFFFFFFFF;
		*pdwItemSeed = 0xFFFFFFFF;
	}
	for (;;)
	{
		int nTab;
		int nSkillLevel;
		int nSkill;
		int nStat108;
		int nStat109;

		hNewItem = ITEMSCreate(hPlayer, dwClass, ptGame, 4, 2, 1, nLevel, FALSE, 0, 0);
		nSkill = D2CommonStatsGetTotalValue(hNewItem, 107);
		nStat108 = D2CommonStatsGetTotalValue(hNewItem, 108);
		nStat109 = D2CommonStatsGetTotalValue(hNewItem, 109);
		D2CommonLoggingUnpackDword(nSkill, &nTab, &nSkillLevel);
		if (nTab == nWantedTab && nSkillLevel == 1 && !nStat108 && !nStat109)
			break;
		ITEMSFree(ptGame, hNewItem);
	}
	*pdwSeed = hNewItem->dwInitSeed;
	*pdwItemSeed = D2CommonITEMSGetItemSeedValue(hNewItem);
	return hNewItem;
}

UNIT * __fastcall PlayerCreateStartItem(GAME *ptGame, UNIT *hPlayer, DWORD dwCode, DWORD dwClass)
{
	UNIT *hNewItem;
	int nLevel;

	nLevel = ITEMSGetUnitLevel(hPlayer, LEVEL_ID_NONE);
	if (hPlayer->dwClassId == 2 && dwCode == ITEMCODE_WAND)
		hNewItem = PlayerSeededStartItem(ptGame, hPlayer, dwClass, nLevel, &sgdwWandItemSeed, &sgdwWandSeed, 70);
	else if (hPlayer->dwClassId == 1 && dwCode == ITEMCODE_SHORTSTAFF)
		hNewItem = PlayerSeededStartItem(ptGame, hPlayer, dwClass, nLevel, &sgdwStaffItemSeed, &sgdwStaffSeed, 36);
	else
		hNewItem = ITEMSCreate(hPlayer, dwClass, ptGame, 4, 2, 1, nLevel, FALSE, 0, 0);
#line 323
	D2ASSERT(hNewItem);
	return hNewItem;
}

void __fastcall PlayerAddStartItems(UNIT *hPlayer, GAME *ptGame, BYTE bType)
{
	CHARSTATSTXT *ptPlrStats;
	int i;

	switch (bType)
	{
	case 0:
		ptPlrStats = (CHARSTATSTXT *)D2CommonDataTblsGetCharStats(hPlayer->dwClassId);
#line 732
		D2ASSERT(ptPlrStats);
		for (i = 0; i < PLAYER_NUM_INITITEMS; i++)
			PlayerAddStartItem(ptGame, hPlayer, &ptPlrStats->atItems[i]);
		break;
	case 1:
		PlayerAddActItems(hPlayer, ptGame, bType);
		D2CommonStatsUnitSetValue(hPlayer, STAT_GOLD, 500);
		break;
	case 2:
		PlayerAddActItems(hPlayer, ptGame, bType);
		D2CommonStatsUnitSetValue(hPlayer, STAT_GOLD, 1000);
		break;
	case 3:
		PlayerAddActItems(hPlayer, ptGame, bType);
		D2CommonStatsUnitSetValue(hPlayer, STAT_GOLD, 5000);
		break;
	}
}

void __fastcall PlayerAddActItems(UNIT *hPlayer, GAME *ptGame, BYTE bAct)
{
	const INITITEM *ptInitItem;
	UNIT *hItem;
	DWORD dwClass;
	int nClass;
	int i, j;

	nClass = hPlayer->dwClassId;
#line 626
	D2WARN(bAct > 0);
	ptInitItem = sgatPlayerActItems[bAct - 1][nClass];
	for (i = 0; i < PLAYER_NUM_INITITEMS; i++, ptInitItem++)
	{
		D2_OPAQUE(ptInitItem);
#line 630
		D2ASSERT(ptInitItem);
		if (!D2CommonItemTblsFindItemByCode(ptInitItem->dwCode, &dwClass))
			continue;
		for (j = 0; j < ptInitItem->bCount; j++)
		{
			hItem = PlayerCreateStartItem(ptGame, hPlayer, ptInitItem->dwCode, dwClass);
			D2CommonITEMSDataSetFlags(hItem, 131072, 1);
			D2CommonITEMSSetCategory(hItem, ptInitItem->bLoc);
			if (D2CommonITEMSGetTypeBelt(hItem) && ptInitItem->bLoc == 8)
			{
				if (!PlayerItemToBelt(ptGame, hPlayer, hItem)
					&& !PlayerItemToInventory(ptGame, hPlayer, hItem))
				{
#line 644
					D2ASSERT(0);
				}
			}
			else if (ptInitItem->bLoc)
			{
				if (!PlayerEquipNewItem(ptGame, hPlayer, hItem)
					&& !PlayerItemToInventory(ptGame, hPlayer, hItem))
				{
#line 650
					D2ASSERT(0);
				}
				PlayerItemFill(hItem);
			}
			else
			{
				PlayerItemToInventory(ptGame, hPlayer, hItem);
				PlayerItemFill(hItem);
			}
		}
	}
}

void __fastcall PlayerSendItems(UNIT *hPlayer, CLIENT *ptClient)
{
	void *hInventory;
	void *ptNode;
	UNIT *hItem;

	hInventory = hPlayer->ptInventory;
#line 780
	D2ASSERT(hInventory);
	for (ptNode = D2CommonINVENTORYGetCategoryListHead(hInventory); ptNode;
		ptNode = (void *)D2CommonINVENTORYNodeGetNext(ptNode))
	{
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
		if (D2CommonINVENTORYNodeGetCategory(ptNode) == 3)
			SCmdSendItemEquip(ptClient, hPlayer, hItem, 0);
		else if (hPlayer == ClientGetPlayerUnit(ptClient,
			FALSE) && D2CommonINVENTORYNodeGetCategory(ptNode) == 1)
			SCmdSendItemToGrid(ptClient, hPlayer, hItem, 0);
		else if (hPlayer == ClientGetPlayerUnit(ptClient,
			FALSE) && D2CommonINVENTORYNodeGetCategory(ptNode) == 2)
			SCmdSendItemToBelt(ptClient, hPlayer, hItem, 0);
	}
}

BOOL __fastcall PlayerIsBusy(UNIT *hUnit)
{
	void *pInventory;
	DWORD dwType;
	DWORD dwUnitId;

	if (SUnitGetInteract(hUnit, &dwType, &dwUnitId))
		return TRUE;
	pInventory = hUnit->ptInventory;
	if (D2CommonINVENTORYGetCursorItem(pInventory))
		return TRUE;
	return ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->bBusy;
}

void __fastcall PlayerClearBusy(UNIT *hUnit)
{
	PLRINFO *ptPlayerInfo;

	ptPlayerInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
#line 829
	D2ASSERT(ptPlayerInfo->bBusy != FALSE);
	ptPlayerInfo->bBusy = FALSE;
}

void __fastcall PlayerInteractEnd(GAME *ptGame, UNIT *hUnit)
{
	ITEMSTXT *ptItemsTxt;
	UNIT *hTarget;
	DWORD dwType;
	DWORD dwUnitId;

	if (!SUnitGetInteract(hUnit, &dwType, &dwUnitId))
		return;
	hTarget = SUnitFind(ptGame, dwType, dwUnitId);
	if (!hTarget)
		return;
	switch (dwType)
	{
	case UNIT_MONSTER:
		SUnitNpcLeave(ptGame, hUnit, hTarget);
		break;
	case UNIT_OBJECT:
		ObjectModeReleasePlayer(ptGame, hUnit, hTarget);
		break;
	case UNIT_PLAYER:
		PlrTradeInteractEnd(ptGame, hUnit);
		break;
	case UNIT_ITEM:
		ptItemsTxt = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hTarget->dwClassId);
		if (ptItemsTxt->code == ITEMCODE_BOX)
			PlrTradeEndInteract(ptGame, hUnit);
		break;
	}
}

void __fastcall PlayerInteractStart(GAME *ptGame, UNIT *hUnit)
{
	ITEMSTXT *ptItemsTxt;
	UNIT *hTarget;
	DWORD dwType;
	DWORD dwUnitId;

	if (SUnitGetInteract(hUnit, &dwType, &dwUnitId))
	{
		hTarget = SUnitFind(ptGame, dwType, dwUnitId);
		if (hTarget)
		{
			switch (dwType)
			{
			case UNIT_MONSTER:
				SUnitNpcLeave(ptGame, hUnit, hTarget);
				SCmdSendInteractionStop(SUnitGetClient(hUnit), 91, 1, hTarget->dwId, 0);
				return;
			case UNIT_OBJECT:
				ObjectModeReleasePlayer(ptGame, hUnit, hTarget);
				SCmdSendInteractionStop(SUnitGetClient(hUnit), 91, 2, hTarget->dwId, 0);
				return;
			case UNIT_PLAYER:
				PlrTradeCommand(ptGame, hUnit, 6, 0);
				return;
			case UNIT_ITEM:
				ptItemsTxt = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hTarget->dwClassId);
				if (ptItemsTxt->code != ITEMCODE_BOX)
					return;
				PlrTradeEndInteract(ptGame, hUnit);
				SCmdSendInteractionStop(SUnitGetClient(hUnit), 91, 4, hTarget->dwId, 0);
				return;
			}
		}
	}
	SCmdSendInteractionStop(SUnitGetClient(hUnit), 91, 6, 0, 0);
}

void __fastcall PlayerPlaceDeadBody(GAME *ptGame, GAMEROOM *hRoom  ,
	DWORD dwUnitId, COLPOINT *ptCoord)
{
	UNIT *hDeadBody;
	void *ptPath;
	int nItem;

#line 913
	D2ASSERT(hRoom);
	(void)D2CommonCollisnFindSpawnPointStep(hRoom, ptCoord, 2, 39961, 4);
	hDeadBody = SUnitFind(ptGame, UNIT_PLAYER, dwUnitId);
	ptPath = hDeadBody->ptPath;
	D2_OPAQUE(hDeadBody);
#line 920
	D2ASSERT(hDeadBody);
	D2CommonStepPlaceUnit(ptPath, hDeadBody, hRoom, ptCoord->nX, ptCoord->nY);
	hDeadBody->dwFlags0 |= 2;
	D2CommonUnitSetMode(hDeadBody, 17);
	D2CommonStatsUnitSetActiveStateBit(hDeadBody, 7, 1);
	hDeadBody->dwFlags0 |= 16;
	EventRemoveByType(ptGame, hDeadBody, 3);
	EventRemoveByType(ptGame, hDeadBody, 4);
	EventRemoveByType(ptGame, hDeadBody, 6);
	nItem = D2CommonINVENTORYGetOwnerId(hDeadBody->ptInventory);
	if (nItem != -1)
		SCmdSendAllCorpse(ptGame, nItem, dwUnitId, 1);
}

void __fastcall PlayerSetPortalId(UNIT *hPlayer  , DWORD dwPortalId)
{
	PLRINFO *ptPlayerInfo;

#line 942
	D2ASSERT(hPlayer);
	ptPlayerInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
#line 945
	D2ASSERT(ptPlayerInfo);
	ptPlayerInfo->dwPortalId = dwPortalId;
}

DWORD __fastcall PlayerGetPortalId(UNIT *hPlayer  )
{
	PLRINFO *ptPlayerInfo;

#line 955
	D2ASSERT(hPlayer);
	ptPlayerInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
#line 958
	D2ASSERT(ptPlayerInfo);
	return ptPlayerInfo->dwPortalId;
}

void __fastcall PlayerRemovePortal(GAME *ptGame, UNIT *hPlayer  )
{
	PLRINFO *ptPlayerInfo;
	UNIT *hPortal;
	UNIT *hOther;
	GAMEROOM *hRoom;

#line 968
	D2ASSERT(hPlayer);
	ptPlayerInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
#line 971
	D2ASSERT(ptPlayerInfo);
	hPortal = SUnitFind(ptGame, UNIT_OBJECT, ptPlayerInfo->dwPortalId);
	if (!hPortal || hPortal->dwClassId != OBJECT_PORTAL)
		return;
	hOther = SUnitGetPortalTarget(ptGame, hPortal);
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hPortal);
	if (hRoom)
		D2CommonDungeonRoomAddNode(hRoom, UNIT_OBJECT, hPortal->dwId);
	SUnitFree(ptGame, hPortal);
	D2CommonDungeonRoomSetHold(hRoom, 1);
	if (!hOther)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hOther);
	if (hRoom)
		D2CommonDungeonRoomAddNode(hRoom, UNIT_OBJECT, hOther->dwId);
	SUnitFree(ptGame, hOther);
	D2CommonDungeonRoomSetHold(hRoom, 1);
}

BOOL __fastcall PlayerDropGold(GAME *ptGame, UNIT *hPlayer, DWORD dwUnitId, DWORD dwAmount)
{
	COLPOINT tCoord;
	UNIT *hGold;
	DWORD dwLeft;
	DWORD dwPile;
	DWORD dwDropped;

	if (SUnitFind(ptGame, UNIT_PLAYER, dwUnitId) != hPlayer)
		return FALSE;
	if (dwAmount > (DWORD)D2CommonStatsUnitGetValue(hPlayer, STAT_GOLD))
		return FALSE;
	if (dwAmount <= 0)
		return TRUE;
	dwLeft = dwAmount;
	D2CommonUnitGetPosition(hPlayer, &tCoord);
	dwDropped = 0;
	while (dwAmount > dwDropped)
	{
		if (dwLeft > PLAYER_MAX_GOLD_DROP)
			dwPile = PLAYER_MAX_GOLD_DROP;
		else
			dwPile = dwLeft;
		dwDropped += dwPile;
		dwLeft -= dwPile;
		hGold = ITEMSDropOtherItem(ptGame, (GAMEROOM *)D2CommonUnitGetPathRoom(hPlayer), &tCoord, 4, TRUE,
			hPlayer);
#line 1036
		D2ASSERT(hGold);
#line 1037
		D2ASSERT(ITEMSGetType(hGold) == ITEMTYPE_GOLD);
		D2CommonStatsUnitSetValue(hGold, STAT_GOLD, dwPile);
		if (hPlayer && hPlayer->eType == UNIT_PLAYER)
			D2CommonUnitSetLinkedUnit(hGold, hPlayer);
	}
	D2CommonStatsUnitSetValue(hPlayer, STAT_GOLD,
		(DWORD)D2CommonStatsUnitGetValue(hPlayer, STAT_GOLD) - dwAmount);
	return TRUE;
}

int __fastcall PlayerInteract(GAME *ptGame, UNIT *hPlayer, DWORD dwArg)
{
	UNIT *hTarget;
	DWORD dwType;
	DWORD dwUnitId;

	if (SUnitGetInteract(hPlayer, &dwType, &dwUnitId))
	{
		hTarget = SUnitFind(ptGame, dwType, dwUnitId);
		if (!hTarget)
		{
			SUnitResetInteract(hPlayer);
			return 0;
		}
		switch (dwType)
		{
		case UNIT_PLAYER:
			return PlrTradeCanAddItem(ptGame, hPlayer, (UNIT *)dwArg);
		case UNIT_MONSTER:
			return MonsterAIThreatTold(hTarget, hPlayer);
		}
	}
	else if (((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->bBusy)
	{
		return 0;
	}
	return PlrTradeCanAddItem(ptGame, hPlayer, (UNIT *)dwArg);
}

BOOL __fastcall PlayerSetClientPair(GAME *ptGame, UNIT *hPlayer, int nIndex, DWORD dwSkillId, DWORD dwHand)
{
	CLIENT *ptClient;

	ptClient = SUnitGetClient(hPlayer);
	if (!ptClient)
		return FALSE;
	ClientSetPair(ptClient, nIndex, dwSkillId, dwHand);
	return TRUE;
}

int __fastcall PlayerCountAlive(GAME *ptGame)
{
	int nCount;

	nCount = 0;
	SUnitForEachPlayer(ptGame, PlayerCountAliveEach, &nCount);
	return nCount;
}

void __fastcall PlayerCountAliveEach(GAME *ptGame, UNIT *hPlayer, void *pArg)
{
	if (!SUnitIsDead(hPlayer))
		(*(int *)pArg)++;
}

void __fastcall PlayerDeathPenalty(GAME *ptGame, UNIT *hPlayer)
{
	MONDIFFICULTYTXT *ptDifficulty;
	int nLevel;
	int nGold;
	int nGoldBank;
	int nTotal;
	int nPercent;
	int nLoss;
	int nKept;
	DWORD dwClass;
	DWORD dwLevelExp;
	DWORD dwSpan;
	DWORD dwExp;
	DWORD dwExpLoss;

	nLevel = D2CommonStatsUnitGetValue(hPlayer, STAT_LEVEL);
	nGold = D2CommonStatsUnitGetValue(hPlayer, STAT_GOLD);
	nGoldBank = D2CommonStatsUnitGetValue(hPlayer, STAT_GOLDBANK);
	nTotal = nGoldBank + nGold;
	if (nLevel < 20)
		nPercent = nLevel;
	else
		nPercent = 20;
	nKept = nLevel * 500;
	nLoss = nPercent * nTotal / 100;
	if (ptGame->bGameType == 3)
	{
		if (nTotal - nLoss < nKept)
		{
			nLoss = nTotal - nKept;
			if (nLoss < 0)
				nLoss = 0;
		}
		if (nLoss > nGold)
			nLoss = nGold;
	}
	D2CommonStatsUnitSetValue(hPlayer, STAT_GOLDLOST, nLoss);
	if (nLoss <= nGold)
		PlayerDropGold(ptGame, hPlayer, hPlayer->dwId, nGold - nLoss);
	else if (ptGame->bGameType != 3)
		D2CommonStatsUnitSetValue(hPlayer, STAT_GOLDBANK, nGoldBank - nLoss + nGold);
	D2CommonStatsUnitSetValue(hPlayer, STAT_GOLD, 0);

	ptDifficulty = (MONDIFFICULTYTXT *)D2CommonDataTblsGetDifficultyLevel(ptGame->bDifficultyLevel);
	dwClass = hPlayer->dwClassId;
	nLevel = D2CommonStatsUnitGetValue(hPlayer, STAT_LEVEL);
	if (nLevel <= 1)
		return;
	dwLevelExp = D2CommonDataTblsGetExperience(dwClass, nLevel - 1);
	dwSpan = D2CommonDataTblsGetExperience(dwClass, nLevel) - dwLevelExp;
	dwExp = D2CommonStatsUnitGetValue(hPlayer, STAT_EXPERIENCE);
	dwExpLoss = (DWORD)(ptDifficulty->DeathExpPenalty * dwSpan) / 100;
	if (!dwExpLoss)
		return;
	dwExp -= dwExpLoss;
	if (dwExp <= dwLevelExp)
		dwExp = dwLevelExp + 1;
	D2CommonStatsUnitSetValue(hPlayer, STAT_EXPERIENCE, dwExp);
}
