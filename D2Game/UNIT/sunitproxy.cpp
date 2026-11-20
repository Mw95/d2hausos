#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\UNIT\\sunitproxy.cpp"
#include "D2Macros.h"
#include "../INVENTORY/InvMode.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/MonsterAI.h"
#include "../QUESTS/QUESTS.h"
#include "../QUESTS/a1q4.h"
#include "SUnit.h"
#include "sunitproxy.h"

#define SUNITPROXY_NUMMONSTERS	410
#define SUNITPROXY_NUMVENDORS	14
#define SUNITPROXY_RENEWTICKS	240000
#define SUNITPROXY_ITEMKIND_SOLD	3
#define SUNITPROXY_ITEMKIND_2		2
#define MONSTER_MAGIC			4
#define LEVEL_ACT1_TOWN			1
#define LEVEL_ACT2_TOWN			40
#define LEVEL_ACT3_TOWN			75
#define LEVEL_ACT4_TOWN			103
#define GAMETYPE_3				3
#define NPCINV_MODE				6

struct NPCTABLEROW
{
	DWORD		dwClassId;
	BYTE		bVendor;
	BYTE		bAct;
	BYTE		field_06;
	BYTE		field_07;
};
typedef struct NPCTABLEROW NPCTABLEROW;

struct NPCLEVELCOUNT
{
	DWORD		dwLevelId;
	int			nCount;
};
typedef struct NPCLEVELCOUNT NPCLEVELCOUNT;

typedef NPCITEMLIST NPCVENDORLISTS[SUNITPROXY_NUMVENDORS];
typedef BYTE NPCVENDORBUILT[SUNITPROXY_NUMVENDORS];

static NPCVENDORBUILT sgabVendorBuilt = { 0 };
static NPCVENDORLISTS sgatVendorLists = { 0 };
static DWORD sgdwItemTsc = 0;
static DWORD sgdwItemIsc = 0;
DWORD gdwItemCqv = 0;
DWORD gdwItemAqv = 0;

static NPCTABLEROW sgatNpcTable[] =
{
	{ 148, 1, 0, 1, 0 }, { 146, 0, 0, 0, 0 }, { 154, 1, 0, 0, 0 }, { 150, 0, 0, 0, 0 },
	{ 147, 1, 0, 0, 0 }, { 155, 0, 0, 0, 0 }, { 266, 0, 0, 0, 0 }, { 175, 0, 1, 0, 0 },
	{ 176, 0, 1, 0, 0 }, { 177, 1, 1, 1, 0 }, { 178, 1, 1, 0, 0 }, { 202, 1, 1, 0, 0 },
	{ 200, 0, 1, 0, 0 }, { 210, 0, 1, 0, 0 }, { 201, 0, 1, 0, 0 }, { 198, 0, 1, 0, 0 },
	{ 199, 1, 1, 0, 0 }, { 377, 0, 1, 0, 0 }, { 378, 0, 1, 0, 0 }, { 265, 0, 0, 0, 0 },
	{ 244, 0, 1, 0, 0 }, { 245, 0, 2, 0, 0 }, { 251, 0, 2, 0, 0 }, { 252, 1, 2, 0, 0 },
	{ 253, 1, 2, 0, 0 }, { 254, 1, 2, 0, 0 }, { 255, 1, 2, 1, 0 }, { 264, 0, 2, 0, 0 },
	{ 246, 0, 3, 0, 0 }, { 406, 0, 3, 0, 0 }, { 367, 0, 3, 0, 0 }, { 408, 0, 3, 0, 0 },
	{ 257, 1, 3, 1, 0 }, { 405, 1, 3, 1, 0 }, { 297, 0, 2, 0, 0 }, { 331, 0, 1, 0, 0 },
};
static int sgnNpcTable = sizeof(sgatNpcTable) / sizeof(sgatNpcTable[0]);

static BYTE sgabPageEntries[] =
{
	255, 255, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0,
	0, 3, 3, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 0, 3, 3, 3, 3, 1, 1,
};
static int sgNumPageEntries = sizeof(sgabPageEntries);

static void SUnitProxyFreeItems(GAME *ptGame, void *hInventory)
{
	void *hNode;
	void *hNext;
	UNIT *hItem;

	for (hNode = D2CommonINVENTORYGetCategoryListHead(hInventory); hNode; hNode = hNext)
	{
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(hNode);
		hNext = (void *)D2CommonINVENTORYNodeGetNext(hNode);
#line 417
		D2ASSERT(hItem);
		D2CommonINVENTORYNodeGetCategory(hNode);
		SUnitFree(ptGame, hItem);
	}
}

NPCPROXY *__fastcall SUnitProxyFindNpc(GAME *ptGame, DWORD mc  , int *pnIndex
)
{
	NPCPROXY *ptNpcs;

	ptNpcs = ptGame->ptProxy->ptNpcs;
	for (*pnIndex = 0; *pnIndex < MAX_NPC; (*pnIndex)++)
	{
		if (ptNpcs[*pnIndex].dwClassId == mc)
			return &ptNpcs[*pnIndex];
	}
#line 104
	D2ASSERT(mc != 0);
	return NULL;
}

NPCPROXY *__fastcall SUnitProxyFindNpcOf(GAME *ptGame, UNIT *hNpc, int *pnIndex
)
{
	return SUnitProxyFindNpc(ptGame, hNpc->dwClassId, pnIndex);
}

void __fastcall SUnitProxyGiveItem(UNIT *hNpc, GAME *ptGame  ,
	UNIT *hItem, DWORD dwArg)
{
	NPCORDER *ptOrder;
	NPCPROXY *pInv;
	int nIndex;

#line 123
	ptOrder = (NPCORDER *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(NPCORDER));
	pInv = SUnitProxyFindNpcOf(ptGame, hNpc, &nIndex);
	if (pInv)
	{
		ptOrder->ptNext = pInv->ptOrders;
		pInv->ptOrders = ptOrder;
		ptOrder->dwHoldsItem = 1;
		ptOrder->field_00 = (DWORD)hItem;
		ptOrder->field_08 = dwArg;
	}
	else
		ITEMSFree(ptGame, hItem);
}

extern void SUnitProxyCopyItemList(GAME *ptGame, NPCITEMLIST *ptDest, NPCITEMLIST *ptSrc
, ITEMSTATSDATATBLDESC *ptDesc);
__inline void SUnitProxyCopyItemList(GAME *ptGame, NPCITEMLIST *ptDest, NPCITEMLIST *ptSrc
, ITEMSTATSDATATBLDESC *ptDesc)
{
	ptDest->nItemStats = ptSrc->nItemStats;
#line 158
	ptDest->pItemStats = (NPCITEMSTAT *)D2ALLOCPOOL(ptGame->pMemPool, ptDesc->nSize * sizeof(NPCITEMSTAT));
	memcpy(ptDest->pItemStats, ptSrc->pItemStats, ptDesc->nSize * sizeof(NPCITEMSTAT));
}

#define SUNITPROXY_STOCK(n) ((NPCITEMLIST *)&ptNpcs[n].ItemList.pItemStats)

void __fastcall SUnitProxyAlloc(GAME *ptGame  )
{
	SUNITPROXY *ptProxy;
	NPCPROXY *ptNpcs;
	NPCPROXY *pInv;
	ITEMSTATSDATATBLDESC *ptDesc;
	DWORD *pdw;
	int nIndex;
	int mc;
	BOOL bFound;
	int i;

#line 173
	ptNpcs = (NPCPROXY *)D2ALLOCPOOL(ptGame->pMemPool, MAX_NPC * sizeof(NPCPROXY));
	pdw = (DWORD *)ptNpcs;
	for (i = 0; i < MAX_NPC * (int)sizeof(NPCPROXY) / 4; i++)
		pdw[i] = 0;
#line 175
	ptProxy = (SUNITPROXY *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(SUNITPROXY));
	ptProxy->ptNpcs = NULL;
	ptProxy->nNpcs = 0;
	ptProxy->field_10 = 0;
	D2CommonSeedInit(&ptProxy->tSeed);
	D2CommonSeedSet(&ptProxy->tSeed, D2CommonSeedInvent(0));
	if (!sgdwItemTsc)
		D2CommonItemTblsFindItemByCode(543388532, &sgdwItemTsc);
	if (!sgdwItemIsc)
		D2CommonItemTblsFindItemByCode(543388521, &sgdwItemIsc);
	if (!gdwItemCqv)
		D2CommonItemTblsFindItemByCode(544633187, &gdwItemCqv);
	if (!gdwItemAqv)
		D2CommonItemTblsFindItemByCode(544633185, &gdwItemAqv);
	ptDesc = (ITEMSTATSDATATBLDESC *)D2CommonItemTblsGetItemStatsDesc();
	nIndex = 0;
	for (mc = 0; mc < SUNITPROXY_NUMMONSTERS; mc++)
	{
		if (!((MONSTATSTXT *)D2CommonMonsterTblsGetStats(mc))->bInteract)
			continue;
		ptNpcs[nIndex].dwClassId = (DWORD)mc;
		ptNpcs[nIndex].dwTradeOpened = 0;
		ptProxy->field_10++;
		ptNpcs[nIndex].ptOrders = NULL;
		ptNpcs[nIndex].ItemList.bStockFilled = 0;
		ptNpcs[nIndex].ItemList.bHiresDrawn = 0;
		ptNpcs[nIndex].ItemList.dwTick = 0;
		ptNpcs[nIndex].ItemList.bVendor = 0;
		ptNpcs[nIndex].ptInventory = (void *)D2CommonINVENTORYCreate(ptGame->pMemPool, 0, NPCINV_MODE, 0, 0);
		ptNpcs[nIndex].ptSoldItems = NULL;
		ptNpcs[nIndex].ptVisits = NULL;
		ptNpcs[nIndex].ptPlayerInvs = NULL;
		ptNpcs[nIndex].dwGambler = 0;
#line 229
		D2ASSERT(nIndex < MAX_NPC);
		ptProxy->nNpcs++;
		bFound = FALSE;
		for (i = 0; i < sgnNpcTable && !bFound; )
		{
			if (sgatNpcTable[i].dwClassId == (DWORD)mc)
				bFound = TRUE;
			else
				i++;
		}
		if (bFound == TRUE)
		{
			ptNpcs[nIndex].ItemList.bAct = sgatNpcTable[i].bAct;
			ptNpcs[nIndex].ItemList.bVendor = sgatNpcTable[i].bVendor;
			ptNpcs[nIndex].ItemList.field_04 = 0;
			ptNpcs[nIndex].ItemList.field_05 = 0;
			ptNpcs[nIndex].ItemList.field_06 = sgatNpcTable[i].field_06;
			ptNpcs[nIndex].ItemList.bRenewPending = 0;
			if (ptNpcs[nIndex].ItemList.bVendor == 1)
			{
				switch ((int)ptNpcs[nIndex].dwClassId)
				{
				case 154:
					SUnitProxyCopyItemList(ptGame, SUNITPROXY_STOCK(nIndex), &sgatVendorLists[2], ptDesc);
					ptNpcs[nIndex].ItemList.field_04 = 1;
					ptNpcs[nIndex].ItemList.field_05 = 1;
					break;
				case 178:
					SUnitProxyCopyItemList(ptGame, SUNITPROXY_STOCK(nIndex), &sgatVendorLists[3], ptDesc);
					ptNpcs[nIndex].ItemList.field_04 = 1;
					ptNpcs[nIndex].ItemList.field_05 = 1;
					break;
				case 148:
					SUnitProxyCopyItemList(ptGame, SUNITPROXY_STOCK(nIndex), &sgatVendorLists[0], ptDesc);
					break;
				case 177:
					SUnitProxyCopyItemList(ptGame, SUNITPROXY_STOCK(nIndex), &sgatVendorLists[5], ptDesc);
					break;
				case 147:
					SUnitProxyCopyItemList(ptGame, SUNITPROXY_STOCK(nIndex), &sgatVendorLists[1], ptDesc);
					ptNpcs[nIndex].ItemList.field_04 = 1;
					ptNpcs[nIndex].ItemList.field_05 = 1;
					ptNpcs[nIndex].dwGambler = 1;
					break;
				case 199:
					SUnitProxyCopyItemList(ptGame, SUNITPROXY_STOCK(nIndex), &sgatVendorLists[9], ptDesc);
					ptNpcs[nIndex].dwGambler = 1;
					break;
				case 202:
					SUnitProxyCopyItemList(ptGame, SUNITPROXY_STOCK(nIndex), &sgatVendorLists[4], ptDesc);
					break;
				case 257:
					SUnitProxyCopyItemList(ptGame, SUNITPROXY_STOCK(nIndex), &sgatVendorLists[12], ptDesc);
					ptNpcs[nIndex].ItemList.field_04 = 1;
					ptNpcs[nIndex].ItemList.field_05 = 1;
					break;
				case 405:
					SUnitProxyCopyItemList(ptGame, SUNITPROXY_STOCK(nIndex), &sgatVendorLists[13], ptDesc);
					ptNpcs[nIndex].ItemList.field_04 = 1;
					ptNpcs[nIndex].ItemList.field_05 = 1;
					ptNpcs[nIndex].dwGambler = 1;
					break;
				case 254:
					SUnitProxyCopyItemList(ptGame, SUNITPROXY_STOCK(nIndex), &sgatVendorLists[7], ptDesc);
					ptNpcs[nIndex].dwGambler = 1;
					break;
				case 252:
					SUnitProxyCopyItemList(ptGame, SUNITPROXY_STOCK(nIndex), &sgatVendorLists[10], ptDesc);
					ptNpcs[nIndex].ItemList.field_04 = 1;
					ptNpcs[nIndex].ItemList.field_05 = 1;
					break;
				case 253:
					SUnitProxyCopyItemList(ptGame, SUNITPROXY_STOCK(nIndex), &sgatVendorLists[6], ptDesc);
					ptNpcs[nIndex].ItemList.field_04 = 1;
					ptNpcs[nIndex].ItemList.field_05 = 1;
					break;
				case 255:
					SUnitProxyCopyItemList(ptGame, SUNITPROXY_STOCK(nIndex), &sgatVendorLists[8], ptDesc);
					break;
				}
			}
			else
				ptNpcs[nIndex].ItemList.pItemStats = NULL;
		}
		nIndex++;
	}
	ptProxy->ptNpcs = ptNpcs;
	ptGame->ptProxy = ptProxy;
}

void __fastcall SUnitProxyFree(GAME *ptGame  )
{
	SUNITPROXY *ptProxy;
	NPCPROXY *pInv;
	NPCVISIT *ptVisit;
	NPCVISIT *ptNext;
	int i;

	ptProxy = ptGame->ptProxy;
	pInv = ptProxy->ptNpcs;
	for (i = 0; i < ptProxy->nNpcs; i++)
	{
		if (pInv[i].ptVisits)
		{
			for (ptVisit = pInv[i].ptVisits; ptVisit; ptVisit = ptNext)
			{
				ptNext = ptVisit->ptNext;
#line 504
				D2FREEPOOL(ptGame->pMemPool, ptVisit);
			}
			pInv[i].ptVisits = NULL;
		}
		SUnitProxyFreeNpc(ptGame, &pInv[i]);
		if (pInv[i].ItemList.pItemStats)
		{
#line 513
			D2FREEPOOL(ptGame->pMemPool, pInv[i].ItemList.pItemStats);
		}
		pInv[i].ItemList.pItemStats = NULL;
		if (pInv[i].ptHires)
		{
#line 524
			D2FREEPOOL(ptGame->pMemPool, pInv[i].ptHires);
			pInv[i].ptHires = NULL;
		}
		pInv[i].ptOrders = NULL;
	}
#line 535
	D2FREEPOOL(ptGame->pMemPool, ptProxy->ptNpcs);
#line 536
	D2FREEPOOL(ptGame->pMemPool, ptProxy);
	ptGame->ptProxy = NULL;
}

void __fastcall SUnitProxyFreeNpc(GAME *ptGame  , NPCPROXY *pInv
)
{
	NPCPLAYERINV *ptPlayerInv;
	NPCPLAYERINV *ptPlayerNext;
	NPCSOLD *ptSold;
	NPCSOLD *ptSoldNext;
	NPCORDER *ptOrder;
	NPCORDER *ptOrderNext;

	if (pInv->dwGambler)
	{
		for (ptPlayerInv = pInv->ptPlayerInvs; ptPlayerInv; ptPlayerInv = ptPlayerNext)
		{
			ptPlayerNext = ptPlayerInv->ptNext;
			if (ptPlayerInv->ptInventory)
			{
				SUnitProxyFreeItems(ptGame, ptPlayerInv->ptInventory);
				D2CommonINVENTORYDestroy(ptPlayerInv->ptInventory);
				ptPlayerInv->ptInventory = NULL;
			}
#line 448
			D2FREEPOOL(ptGame->pMemPool, ptPlayerInv);
		}
		pInv->ptPlayerInvs = NULL;
	}
	if (pInv->ptInventory)
		SUnitProxyFreeItems(ptGame, pInv->ptInventory);
	D2CommonINVENTORYDestroy(pInv->ptInventory);
	pInv->ptInventory = NULL;
	for (ptSoldNext = pInv->ptSoldItems; ptSoldNext; )
	{
		ptSold = ptSoldNext;
		ptSoldNext = ptSold->ptNext;
#line 463
		D2FREEPOOL(ptGame->pMemPool, ptSold);
	}
	pInv->ptSoldItems = NULL;
	for (ptOrder = pInv->ptOrders; ptOrder; ptOrder = ptOrderNext)
	{
		ptOrderNext = ptOrder->ptNext;
		if (ptOrder->dwHoldsItem == 1 && ptOrder->field_00)
			ITEMSFree(ptGame, (UNIT *)ptOrder->field_00);
#line 478
		D2FREEPOOL(ptGame->pMemPool, ptOrder);
	}
	pInv->ptOrders = NULL;
}

extern void SUnitProxyQueueOrder(GAME *ptGame, NPCPROXY *pInv, DWORD dwValue, DWORD dwArg8,
	DWORD dwArg4);
__inline void SUnitProxyQueueOrder(GAME *ptGame, NPCPROXY *pInv, DWORD dwValue, DWORD dwArg8,
	DWORD dwArg4)
{
	NPCORDER *ptOrder;

#line 568
	ptOrder = (NPCORDER *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(NPCORDER));
	ptOrder->ptNext = pInv->ptOrders;
	ptOrder->dwHoldsItem = 0;
	ptOrder->field_00 = dwValue;
	ptOrder->field_08 = dwArg8;
	ptOrder->field_04 = dwArg4;
	pInv->ptOrders = ptOrder;
}

void __fastcall SUnitProxyOrder(UNIT *hNpc, GAME *ptGame  , DWORD
	dwLevel, DWORD dwArg8, DWORD dwArg4)
{
	NPCPROXY *pInv;
	NPCSTATE *ptState;
	DWORD dwTick;
	int nIndex;

	dwTick = GetTickCount();
	pInv = SUnitProxyFindNpcOf(ptGame, hNpc, &nIndex);
	if (!pInv)
		return;
	ptState = &pInv->ItemList;
	if (!ptState || ptState->bVendor != 1 || ptState->bStockFilled != 1 || pInv->dwTradeOpened != 1)
		return;
	SUnitProxyQueueOrder(ptGame, pInv, dwLevel, dwArg8, dwArg4);
	ptState->dwTick = dwTick;
}

void __fastcall SUnitProxyChangeLevel(GAME *ptGame  , UNIT *hUnit,
	DWORD dwLevelOld, DWORD dwLevelNew)
{
	NPCLEVELCOUNT tCount;

	tCount.dwLevelId = dwLevelOld;
	tCount.nCount = 0;
	switch (dwLevelOld)
	{
	case LEVEL_ACT1_TOWN:
		SUnitForEachPlayer(ptGame, SUnitProxyCountInLevel, &tCount);
		SUnitProxyRenew(ptGame, hUnit, 0, tCount.nCount == 0);
		break;
	case LEVEL_ACT2_TOWN:
		SUnitForEachPlayer(ptGame, SUnitProxyCountInLevel, &tCount);
		SUnitProxyRenew(ptGame, hUnit, 1, tCount.nCount == 0);
		break;
	case LEVEL_ACT3_TOWN:
		SUnitForEachPlayer(ptGame, SUnitProxyCountInLevel, &tCount);
		SUnitProxyRenew(ptGame, hUnit, 2, tCount.nCount == 0);
		break;
	case LEVEL_ACT4_TOWN:
		SUnitForEachPlayer(ptGame, SUnitProxyCountInLevel, &tCount);
		SUnitProxyRenew(ptGame, hUnit, 3, tCount.nCount == 0);
		break;
	}
	switch (dwLevelNew)
	{
	case LEVEL_ACT1_TOWN:
		QuestsSendIntroduced(ptGame, hUnit, 0);
		break;
	case LEVEL_ACT2_TOWN:
		QuestsSendIntroduced(ptGame, hUnit, 1);
		A1Q4EnteredLevel(ptGame, hUnit);
		break;
	case LEVEL_ACT3_TOWN:
		QuestsSendIntroduced(ptGame, hUnit, 2);
		break;
	}
}

static __inline void *SUnitProxyResetI(GAME *ptGame, NPCPROXY *pInv)
{
	SUnitProxyFreeNpc(ptGame, pInv);
	pInv->ItemList.bStockFilled = 0;
	pInv->abSoldCounts[0] = 0;
	pInv->abSoldCounts[1] = 0;
	pInv->abSoldCounts[2] = 0;
	pInv->abSoldCounts[3] = 0;
	pInv->ItemList.dwTick = 0;
	pInv->ptInventory = (void *)D2CommonINVENTORYCreate(ptGame->pMemPool, 0, NPCINV_MODE, 0, 0);
	return pInv->ptInventory;
}

void __fastcall SUnitProxyRenew(GAME *ptGame  , UNIT *hUnit,
	BYTE bAct, BOOL bEmpty)
{
	SUNITPROXY *ptProxy;
	NPCPROXY *pInv;
	NPCSTATE *ptState;
	NPCPLAYERINV *ptPlayerInv;
	NPCPLAYERINV *ptPrev;
	UNIT *hNpc;
	DWORD dwTick;
	DWORD dwUnitId;
	int i;

	ptProxy = ptGame->ptProxy;
	pInv = ptProxy->ptNpcs;
	i = 0;
	dwTick = GetTickCount();
	dwUnitId = hUnit->dwId;
	for (; i < ptProxy->nNpcs; i++)
	{
		ptState = &pInv[i].ItemList;
		if (!ptState || pInv[i].ItemList.bAct != bAct || pInv[i].ItemList.bVendor != 1
			|| pInv[i].dwTradeOpened != 1)
			continue;
		if (pInv[i].dwGambler)
		{
			ptPrev = NULL;
			for (ptPlayerInv = pInv[i].ptPlayerInvs; ptPlayerInv;
				ptPrev = ptPlayerInv, ptPlayerInv = ptPlayerInv->ptNext)
			{
				if (ptPlayerInv->dwUnitId == dwUnitId)
				{
					if (ptPrev)
						ptPrev->ptNext = ptPlayerInv->ptNext;
					else
						pInv[i].ptPlayerInvs = ptPlayerInv->ptNext;
					if (ptPlayerInv->ptInventory)
					{
						SUnitProxyFreeItems(ptGame, ptPlayerInv->ptInventory);
						D2CommonINVENTORYDestroy(ptPlayerInv->ptInventory);
						ptPlayerInv->ptInventory = NULL;
					}
#line 624
					D2FREEPOOL(ptGame->pMemPool, ptPlayerInv);
					break;
				}
			}
		}
		if (pInv[i].ItemList.bStockFilled != 1)
			continue;
		if (bEmpty)
		{
			hNpc = SUnitFind(ptGame, UNIT_MONSTER, pInv[i].dwUnitId);
			if (hNpc && hNpc->dwClassId == pInv[i].dwClassId)
			{
				if (!MonsterAICountThreats(((MONSTERDATA *)hNpc->uTypeData)->pptThreats))
				{
					hNpc->ptInventory = SUnitProxyResetI(ptGame, &pInv[i]);
				}
				else
					pInv[i].ItemList.bRenewPending = 1;
			}
			else
			{
				SUnitProxyResetI(ptGame, &pInv[i]);
			}
		}
		else if (dwTick > pInv[i].ItemList.dwTick + SUNITPROXY_RENEWTICKS)
		{
			pInv[i].ItemList.bRenewPending = 1;
			pInv[i].ItemList.dwTick = dwTick;
		}
	}
}

void __fastcall SUnitProxyCountInLevel(GAME *ptGame, UNIT *hUnit, void *pArg)
{
	void *hRoom;

	hRoom = (void *)D2CommonUnitGetPathRoom(hUnit);
	if (hRoom && ((NPCLEVELCOUNT *)pArg)->dwLevelId == D2CommonDungeonRoomGetLevelId(hRoom))
		((NPCLEVELCOUNT *)pArg)->nCount++;
}

void __fastcall SUnitProxyPlayerLeft(GAME *ptGame  , UNIT *hUnit)
{
	NPCLEVELCOUNT tCount;
	void *hRoom;
	BYTE bAct;

	if (ptGame->bGameType == GAMETYPE_3)
		return;
	hRoom = (void *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	tCount.dwLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
	tCount.nCount = 0;
	switch (tCount.dwLevelId)
	{
	case LEVEL_ACT1_TOWN:
		bAct = 0;
		break;
	case LEVEL_ACT2_TOWN:
		bAct = 1;
		break;
	case LEVEL_ACT3_TOWN:
		bAct = 2;
		break;
	case LEVEL_ACT4_TOWN:
		bAct = 3;
		break;
	default:
		return;
	}
	SUnitForEachPlayer(ptGame, SUnitProxyCountInLevel, &tCount);
	SUnitProxyRenew(ptGame, hUnit, bAct, --tCount.nCount == 0);
}

void __fastcall SUnitProxyLevelUp(GAME *ptGame  , UNIT *hUnit, int
	nLevel)
{
	SUNITPROXY *ptProxy;
	NPCPROXY *pInv;
	NPCSTATE *ptState;
	int i;

	i = 0;
	ptProxy = ptGame->ptProxy;
	pInv = ptProxy->ptNpcs;
	GetTickCount();
	for (; i < ptProxy->nNpcs; i++)
	{
		ptState = &pInv[i].ItemList;
		if (ptState->bVendor == 1 && ptState->bStockFilled == 1 && pInv[i].dwTradeOpened == 1)
		{
			SUnitProxyQueueOrder(ptGame, &pInv[i], (DWORD)nLevel, 1, 4);
		}
	}
}

BYTE __fastcall SUnitProxyGetItemKind(UNIT *hItem  )
{
	int Offset;
	BYTE bKind;

	Offset = (int)D2CommonITEMSGetType(hItem);
#line 1099
	D2ASSERT(Offset < sgNumPageEntries);
	bKind = sgabPageEntries[Offset];
	if (bKind != SUNITPROXY_ITEMKIND_SOLD && D2CommonITEMSTestModTypeMagicOrAbove(hItem))
		return SUNITPROXY_ITEMKIND_2;
	return bKind;
}

void __fastcall SUnitProxySellItem(GAME *ptGame  , UNIT *hNpc, UNIT
	*hItem  , DWORD dwArg)
{
	NPCPROXY *pInv;
	NPCSOLD *ptSold;
	int nIndex;

	pInv = SUnitProxyFindNpcOf(ptGame, hNpc, &nIndex);
	SUnitProxyCountStock(pInv, hNpc, hItem, ptGame, 1);
	pInv->abSoldCounts[SUnitProxyGetItemKind(hItem)]++;
#line 1133
	ptSold = (NPCSOLD *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(NPCSOLD));
	ptSold->dwClassId = hItem->dwClassId;
	ptSold->field_04 = D2CommonITEMSGetTypeLevel(hItem);
	ptSold->field_08 = dwArg;
	ptSold->dwUnitId = hItem->dwId;
	ptSold->dwTick = GetTickCount();
	if (!(pInv->ptSoldItems))
	{
		pInv->ptSoldItems = ptSold;
		ptSold->ptNext = NULL;
	}
	else
	{
		ptSold->ptNext = pInv->ptSoldItems;
		pInv->ptSoldItems = ptSold;
	}
}

void __fastcall SUnitProxyCountStock(NPCPROXY *pInv  , UNIT *hNpc,
	UNIT *hItem  , GAME *ptGame  , int nDelta
)
{
	DWORD dwClassId;
	int nLow;
	int nHigh;
	int useidx;
	int nTries;
	BYTE bOffset;

	dwClassId = hItem->dwClassId;
	nLow = 0;
	nHigh = pInv->ItemList.nItemStats;
	bOffset = D2CommonITEMSDataGetModType(hItem) != MONSTER_MAGIC;
	nTries = 0;
	while (nHigh >= nLow)
	{
		useidx = (nHigh + nLow) / 2;
		if (dwClassId == pInv->ItemList.pItemStats[useidx].dwClassId)
		{
			if (nDelta == -1)
			{
#line 1005
				D2ASSERT(pInv->ItemList.pItemStats[useidx].nInvCount[bOffset] > 0);
			}
			pInv->ItemList.pItemStats[useidx].nInvCount[bOffset] += nDelta;
			return;
		}
		if (dwClassId < pInv->ItemList.pItemStats[useidx].dwClassId)
			nHigh = useidx - 1;
		else
			nLow = useidx + 1;
		if (++nTries > pInv->ItemList.nItemStats)
			return;
	}
}

void __fastcall SUnitProxyBuyItem(GAME *ptGame  , UNIT *hNpc, UNIT
	*hItem  )
{
	NPCPROXY *pInv;
	NPCSOLD *ptSold;
	NPCSOLD *ptPrev;
	NPCSOLD *ptHead;
	DWORD dwUnitId;
	int nIndex;

	pInv = SUnitProxyFindNpcOf(ptGame, hNpc, &nIndex);
	SUnitProxyCountStock(pInv, hNpc, hItem, ptGame, -1);
	pInv->abSoldCounts[SUnitProxyGetItemKind(hItem)]--;
	if (!pInv->ptSoldItems)
		return;
	dwUnitId = hItem->dwId;
	ptHead = pInv->ptSoldItems;
	ptPrev = NULL;
	for (ptSold = ptHead; ptSold; ptPrev = ptSold, ptSold = ptSold->ptNext)
	{
		if (ptSold->dwUnitId == dwUnitId)
		{
			if (ptSold == ptHead)
				pInv->ptSoldItems = ptSold->ptNext;
			else if (ptPrev)
				ptPrev->ptNext = ptSold->ptNext;
			else
			{
#line 1202
				D2ASSERT(0);
			}
#line 1206
			D2FREEPOOL(ptGame->pMemPool, ptSold);
			return;
		}
	}
}

void *__fastcall SUnitProxyGetInventory(GAME *ptGame  , DWORD dwClassId
)
{
	int nIndex;

	return SUnitProxyFindNpc(ptGame, dwClassId, &nIndex)->ptInventory;
}

D2SEED *__fastcall SUnitProxyGetSeed(GAME *ptGame  )
{
	return &ptGame->ptProxy->tSeed;
}

void __fastcall SUnitProxyRemovePlayer(GAME *ptGame  , UNIT *hPlayer
, UNIT *hNpc)
{
	NPCPROXY *pInv;
	NPCPLAYERINV *ptPlayerInv;
	NPCPLAYERINV *ptPrevInv;
	NPCVISIT *ptVisit;
	NPCVISIT *ptPrevVisit;
	DWORD dwUnitId;
	int nIndex;

	pInv = SUnitProxyFindNpcOf(ptGame, hNpc, &nIndex);
	if (!pInv)
		return;
	dwUnitId = hPlayer->dwId;
	if (pInv->dwGambler)
	{
		ptPrevInv = NULL;
		for (ptPlayerInv = pInv->ptPlayerInvs; ptPlayerInv;
			ptPrevInv = ptPlayerInv, ptPlayerInv = ptPlayerInv->ptNext)
		{
			if (ptPlayerInv->dwUnitId == dwUnitId)
			{
				if (ptPrevInv)
					ptPrevInv->ptNext = ptPlayerInv->ptNext;
				else
					pInv->ptPlayerInvs = ptPlayerInv->ptNext;
				if (ptPlayerInv->ptInventory)
				{
					SUnitProxyFreeItems(ptGame, ptPlayerInv->ptInventory);
					D2CommonINVENTORYDestroy(ptPlayerInv->ptInventory);
					ptPlayerInv->ptInventory = NULL;
				}
#line 1268
				D2FREEPOOL(ptGame->pMemPool, ptPlayerInv);
				break;
			}
		}
	}
	if (!pInv->ptVisits)
		return;
	ptPrevVisit = NULL;
	for (ptVisit = pInv->ptVisits; ptVisit; ptPrevVisit = ptVisit, ptVisit = ptVisit->ptNext)
	{
		if (ptVisit->dwUnitId == dwUnitId)
		{
			if (ptPrevVisit)
				ptPrevVisit->ptNext = ptVisit->ptNext;
			else
				pInv->ptVisits = ptVisit->ptNext;
#line 1292
			D2FREEPOOL(ptGame->pMemPool, ptVisit);
			return;
		}
	}
}

void __fastcall SUnitProxySendItems(GAME *ptGame  , UNIT *hNpc,
	UNIT *hPlayer, CLIENT *ptClient)
{
	NPCPROXY *pInv;
	NPCPLAYERINV *ptPlayerInv;
	DWORD dwUnitId;
	int nIndex;

	INVENTORYModeSendItems(ptGame, hNpc, ptClient, NULL);
	pInv = SUnitProxyFindNpcOf(ptGame, hNpc, &nIndex);
	if (!pInv || !pInv->dwGambler)
		return;
	dwUnitId = hPlayer->dwId;
	ptPlayerInv = pInv->ptPlayerInvs;
	for (; ptPlayerInv; ptPlayerInv = ptPlayerInv->ptNext)
	{
		if (ptPlayerInv->dwUnitId == dwUnitId)
		{
			if (ptPlayerInv->ptInventory)
				INVENTORYModeSendItems(ptGame, hNpc, ptClient, ptPlayerInv->ptInventory);
			return;
		}
	}
}

void *__fastcall SUnitProxyGetPlayerInventory(GAME *ptGame  , UNIT *hPlayer
, UNIT *hNpc)
{
	NPCPROXY *pInv;
	NPCPLAYERINV *ptPlayerInv;
	DWORD dwUnitId;
	int nIndex;

	pInv = SUnitProxyFindNpcOf(ptGame, hNpc, &nIndex);
	if (!pInv || !pInv->dwGambler)
		return NULL;
	ptPlayerInv = pInv->ptPlayerInvs;
	dwUnitId = hPlayer->dwId;
	for (; ptPlayerInv; ptPlayerInv = ptPlayerInv->ptNext)
	{
		if (ptPlayerInv->dwUnitId == dwUnitId)
			return ptPlayerInv->ptInventory;
	}
	return NULL;
}

NPCVISIT *__fastcall SUnitProxyAddVisit(GAME *ptGame  , NPCPROXY *pInv
, UNIT *hPlayer)
{
	NPCVISIT *ptVisit;
	DWORD dwUnitId;

	dwUnitId = hPlayer->dwId;
	for (ptVisit = pInv->ptVisits; ptVisit; ptVisit = ptVisit->ptNext)
	{
		if (ptVisit->dwUnitId == dwUnitId)
			return ptVisit;
	}
#line 1362
	ptVisit = (NPCVISIT *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(NPCVISIT));
	ptVisit->dwUnitId = dwUnitId;
	ptVisit->ptNext = pInv->ptVisits;
	pInv->ptVisits = ptVisit;
	return ptVisit;
}

void __fastcall SUnitProxyResetNpc(GAME *ptGame  , NPCPROXY *pInv
, UNIT *hNpc)
{
	SUnitProxyFreeNpc(ptGame, pInv);
	pInv->ItemList.bStockFilled = 0;
	pInv->abSoldCounts[0] = 0;
	pInv->abSoldCounts[1] = 0;
	pInv->abSoldCounts[2] = 0;
	pInv->abSoldCounts[3] = 0;
	pInv->ItemList.dwTick = 0;
	pInv->ptInventory = (void *)D2CommonINVENTORYCreate(ptGame->pMemPool, 0, NPCINV_MODE, 0, 0);
	if (hNpc)
		hNpc->ptInventory = pInv->ptInventory;
}

void __fastcall SUnitProxyInit(void)
{
	int mc;

	for (mc = 0; mc < SUNITPROXY_NUMMONSTERS; mc++)
	{
		if (!((MONSTATSTXT *)D2CommonMonsterTblsGetStats(mc))->bInteract)
			continue;
		switch (mc)
		{
		case 154:	SUnitProxyBuildVendor(2);	break;
		case 178:	SUnitProxyBuildVendor(3);	break;
		case 148:	SUnitProxyBuildVendor(0);	break;
		case 177:	SUnitProxyBuildVendor(5);	break;
		case 147:	SUnitProxyBuildVendor(1);	break;
		case 199:	SUnitProxyBuildVendor(9);	break;
		case 202:	SUnitProxyBuildVendor(4);	break;
		case 257:	SUnitProxyBuildVendor(12);	break;
		case 405:	SUnitProxyBuildVendor(13);	break;
		case 254:	SUnitProxyBuildVendor(7);	break;
		case 252:	SUnitProxyBuildVendor(10);	break;
		case 253:	SUnitProxyBuildVendor(6);	break;
		case 255:	SUnitProxyBuildVendor(8);	break;
		}
	}
}

void __fastcall SUnitProxyBuildVendor(int nVendor)
{
	ITEMSTATSDATATBLDESC *ptDesc;
	ITEMSTXT *ptRow;
	NPCITEMSTAT *ptStat;
	int nCount;
	int i;

	ptDesc = (ITEMSTATSDATATBLDESC *)D2CommonItemTblsGetItemStatsDesc();
	if (sgabVendorBuilt[nVendor])
		return;
	sgabVendorBuilt[nVendor] = 1;
#line 1398
	sgatVendorLists[nVendor].pItemStats = (NPCITEMSTAT *)D2ALLOCPOOL(NULL,
		ptDesc->nSize * sizeof(NPCITEMSTAT));
	memset(sgatVendorLists[nVendor].pItemStats, 0, ptDesc->nSize * sizeof(NPCITEMSTAT));
	nCount = 0;
	sgatVendorLists[nVendor].nItemStats = 0;
	for (i = 0; i < ptDesc->nSize; i++)
	{
		ptRow = &ptDesc->ptItemStats[i];
		if (!ptRow->spawnable || !ptRow->Max[nVendor])
			continue;
		sgatVendorLists[nVendor].pItemStats[nCount].Min = ptDesc->ptItemStats[i].Min[nVendor];
		sgatVendorLists[nVendor].pItemStats[nCount].Max = ptDesc->ptItemStats[i].Max[nVendor];
		sgatVendorLists[nVendor].pItemStats[nCount].MagicMin = ptDesc->ptItemStats[i].MagicMin[nVendor];
		sgatVendorLists[nVendor].pItemStats[nCount].MagicMax = ptDesc->ptItemStats[i].MagicMax[nVendor];
		sgatVendorLists[nVendor].pItemStats[nCount].MagicLvl = ptDesc->ptItemStats[i].MagicLvl[nVendor];
		sgatVendorLists[nVendor].pItemStats[nCount].dwClassId = (DWORD)i;
		nCount++;
	}
	sgatVendorLists[nVendor].nItemStats = nCount;
}

void __fastcall SUnitProxyFreeVendors(void)
{
	int i;

	for (i = 0; i < SUNITPROXY_NUMVENDORS; i++)
	{
		if (sgabVendorBuilt[i] == 1)
		{
			sgabVendorBuilt[i] = 0;
			if (sgatVendorLists[i].pItemStats)
			{
#line 1505
				D2FREEPOOL(NULL, sgatVendorLists[i].pItemStats);
				sgatVendorLists[i].pItemStats = NULL;
			}
		}
	}
}
