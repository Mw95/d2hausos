#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\ITEMS\\Items.cpp"
#include <string.h>
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "../GAME/SCmd.h"
#include "../GAME/Event.h"
#include "../INVENTORY/InvMode.h"
#include "ItemMode.h"
#include "Items.h"
#include "../QUESTS/QUESTS.h"
#include "../UNIT/SUnit.h"

#define MAX_CHOOSE_LIST		512
#define BASE_GOLD_RAND		65
#define NUM_ITEMSETS		18
#define LEVEL_ID_NONE		0
#define UNIT_MONSTER		1
#define UNIT_MISSILE		3
#define UNIT_ITEM			4

#define ITEMTYPE_ANY		44
#define ITEMCODE_GOLD		543452263
#define ITEMCODE_EAR		544366949

#define ITEMQUAL_LOW		1
#define ITEMQUAL_NORMAL		2
#define ITEMQUAL_HIQUALITY	3
#define ITEMQUAL_MAGIC		4
#define ITEMQUAL_SET		5
#define ITEMQUAL_RARE		6
#define ITEMQUAL_UNIQUE		7

struct ITEMCHOOSENODE
{
 	int			nIndex;
 	void		*ptStat;
};
typedef struct ITEMCHOOSENODE ITEMCHOOSENODE;

static int sgnItemsForcedPrefix = -1;
static int sgnItemsForcedSuffix = -1;
static BYTE sgbGoldIndexGuardB = 0;
static int sgnGoldIndexB = 0;
static BYTE sgbGoldIndexGuardA = 0;
static int sgnGoldIndexA = 0;
static int sgnItemsGoldIndex = 0;

static DWORD sgadwItemsMagicTypes[] = { 10, 12 };
static DWORD sgnItemsMagicTypes = arrsize(sgadwItemsMagicTypes);
static DWORD sgadwItemsNormalTypes[] = { 20, 18, 38, 22, 40, 9, 41, 7 };
static DWORD sgnItemsNormalTypes = arrsize(sgadwItemsNormalTypes);
static DWORD sgadwItemsQualities[] = { ITEMQUAL_UNIQUE, ITEMQUAL_RARE, ITEMQUAL_SET, ITEMQUAL_MAGIC,
	ITEMQUAL_HIQUALITY, ITEMQUAL_NORMAL };
static DWORD sgnItemsQualities = arrsize(sgadwItemsQualities);
static DWORD sgadwItemsHealingPotions[] = { 540110952, 540176488, 540242024, 540307560 };
static DWORD sgnItemsHealingPotions = arrsize(sgadwItemsHealingPotions);
static DWORD sgadwItemsManaPotions[] = { 540110957, 540176493, 540242029, 540307565 };
static DWORD sgnItemsManaPotions = arrsize(sgadwItemsManaPotions);
static const int sganItemsMagicQualities[4] = { ITEMQUAL_UNIQUE, ITEMQUAL_RARE, ITEMQUAL_SET,
	ITEMQUAL_MAGIC };

static inline DWORD ITEMSRandom(D2SEED *ptSeed, DWORD dwMax)
{
	if (dwMax)
		return D2CommonSeedRand(ptSeed, dwMax);
	else
		return 0;
}

static inline void ITEMSChooseListAdd(ITEMCHOOSENODE *ptChooseList, int *pnCount, int nIndex,
	void *ptStat)
{
#line 112
	D2ASSERT(*pnCount < MAX_CHOOSE_LIST);
	ITEMCHOOSENODE *ptNode = &ptChooseList[*pnCount];
	int nNext = *pnCount + 1;
	ptNode->ptStat = ptStat;
	ptNode->nIndex = nIndex;
	*pnCount = nNext;
}

static inline ITEMCHOOSENODE *ITEMSChooseListAt(ITEMCHOOSENODE *ptChooseList, int nCount, int nFindIndex)
{
#line 128
	D2ASSERT(nFindIndex < nCount);
	return &ptChooseList[nFindIndex];
}

static inline ITEMCHOOSENODE *ITEMSChooseListPick(D2SEED *ptSeed, ITEMCHOOSENODE *ptChooseList, int nCount)
{
	return ITEMSChooseListAt(ptChooseList, nCount, ITEMSRandom(ptSeed, nCount));
}

void __fastcall ITEMSInitUnit(UNIT *hItem, GAME *ptGame, DWORD dwUnitId,
	DWORD dwUnused)
{
	ITEMSTXT *ptItemStats;
	void *hInventory;

	hItem->dwId = dwUnitId;
	hItem->dwFlags0 = (hItem->dwFlags0 & ~8) | 2;
	D2CommonITEMSDataSetFlags(hItem, 16, 1);
	D2CommonStatsUnitAllocStats(hItem, 0);
	D2CommonStatsUnitLinkList(hItem, D2CommonStatsAllocList(ptGame->pMemPool, 64, 0, 4,
		hItem->dwId));
	if (D2CommonITEMSGetTypeHasInv(hItem))
	{
		hInventory = (void *)D2CommonINVENTORYCreate(ptGame->pMemPool, hItem->dwId, hItem->eType,
			hItem->dwClassId, 1);
#line 150
		D2ASSERT(hInventory);
		hItem->ptInventory = hInventory;
	}
	ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
	if (ptItemStats->quest)
		QuestsAttachUnit(ptGame, hItem, ptItemStats->quest - 1);
	EventUnitInit(ptGame, hItem);
}

GAMEROOM *__fastcall ITEMSFindDropSpot(GAMEROOM *hRoom, COLPOINT *ptCoord,
	COLPOINT *ptSpawn, int nSize)
{
	int nX, nY;

	nX = ptCoord->nX + 2;
	nY = ptCoord->nY + 3;
	if (D2CommonDungeonFindRoomGame(hRoom, nX, nY))
	{
		ptSpawn->nX = nX;
		ptSpawn->nY = nY;
	}
	else
	{
		ptSpawn->nX = ptCoord->nX;
		ptSpawn->nY = ptCoord->nY;
	}
	return (GAMEROOM *)D2CommonCollisnFindSpawnPointField(hRoom, ptSpawn, ptCoord, nSize, 7681,
		2049, 1);
}

int __fastcall ITEMSChooseArmor(D2SEED *ptSeed, int nLevelId, int nType,
	BOOL fNoRarity)
{
	ITEMCHOOSENODE atChooseList[MAX_CHOOSE_LIST];
	ITEMCHOOSENODE *ptChooseList;
	ITEMCHOOSENODE *ptChooseNode;
	ITEMSTATSDATATBLDESC *ptDesc;
	ITEMSTXT *ptItemStats;
	ITEMSTXT *ptStat;
	int nBase, nNum, nCount, i;

	ptDesc = (ITEMSTATSDATATBLDESC *)D2CommonItemTblsGetItemStatsDesc();
	ptItemStats = ptDesc->ptArmorStart;
	nBase = ptItemStats - ptDesc->ptItemStats;
	nNum = ptDesc->ptMiscStart - ptItemStats;
	nCount = 0;
#line 232
	D2ASSERT(ptItemStats);
	ptChooseList = atChooseList;
	for (i = 0; i < nNum; i++)
	{
		ptStat = &ptItemStats[i];
		D2_OPAQUE(ptStat);
#line 235
		D2ASSERT(ptStat);
		if (ITEMSCanSpawn(ptSeed, ptStat, nLevelId, nType, fNoRarity))
			ITEMSChooseListAdd(atChooseList, &nCount, nBase + i, ptStat);
	}
	D2_OPAQUE(ptChooseList);
#line 243
	D2ASSERT(ptChooseList);
	ptChooseNode = ITEMSChooseListPick(ptSeed, ptChooseList, nCount);
	D2_OPAQUE(ptChooseNode);
#line 246
	D2ASSERT(ptChooseNode);
	return ptChooseNode->nIndex;
}

BOOL __fastcall ITEMSCanSpawn(D2SEED *ptSeed, ITEMSTXT *ptStat, int nLevelId, int nType,
	BOOL fNoRarity)
{
	BYTE bAct;
	int nChance;
	BOOL fSpawn;

	if (nLevelId <= 0)
		nLevelId = 1;
	if (ptStat->completed != 1 || ptStat->spawnable != 1 || ptStat->quest || ptStat->level > nLevelId)
		fSpawn = FALSE;
	else
	{
		if (!fNoRarity)
		{
			bAct = D2CommonGetActNum(nLevelId);
			nChance = ptStat->rarity - bAct;
			if (nChance > 0 && D2CommonSeedRand(ptSeed, nChance))
			{
				fSpawn = FALSE;
				goto done;
			}
		}
		if (nType != ITEMTYPE_ANY && ptStat->type != nType)
			fSpawn = FALSE;
		else
			fSpawn = TRUE;
	}
done:
	return fSpawn;
}

int __fastcall ITEMSChooseWeapon(D2SEED *ptSeed, int nLevelId, int nType,
	BOOL fNoRarity)
{
	ITEMCHOOSENODE atChooseList[MAX_CHOOSE_LIST];
	ITEMCHOOSENODE *ptChooseList;
	ITEMCHOOSENODE *ptChooseNode;
	ITEMSTATSDATATBLDESC *ptDesc;
	ITEMSTXT *ptItemStats;
	ITEMSTXT *ptStat;
	int nBase, nNum, nCount, i;

	ptDesc = (ITEMSTATSDATATBLDESC *)D2CommonItemTblsGetItemStatsDesc();
	ptItemStats = ptDesc->ptWeaponsStart;
	nBase = ptItemStats - ptDesc->ptItemStats;
	nNum = ptDesc->ptArmorStart - ptItemStats;
	nCount = 0;
#line 266
	D2ASSERT(ptItemStats);
	ptChooseList = atChooseList;
	for (i = 0; i < nNum; i++)
	{
		ptStat = &ptItemStats[i];
		D2_OPAQUE(ptStat);
#line 269
		D2ASSERT(ptStat);
		if (ITEMSCanSpawn(ptSeed, ptStat, nLevelId, nType, fNoRarity))
			ITEMSChooseListAdd(atChooseList, &nCount, nBase + i, ptStat);
	}
	D2_OPAQUE(ptChooseList);
#line 277
	D2ASSERT(ptChooseList);
	ptChooseNode = ITEMSChooseListPick(ptSeed, ptChooseList, nCount);
	D2_OPAQUE(ptChooseNode);
#line 280
	D2ASSERT(ptChooseNode);
	return ptChooseNode->nIndex;
}

int __fastcall ITEMSChooseOtherItem(D2SEED *ptSeed, int nLevelId, int nType,
	BOOL fNoRarity, BOOL fType28)
{
	ITEMCHOOSENODE atChooseList[MAX_CHOOSE_LIST];
	ITEMCHOOSENODE *ptChooseList;
	ITEMCHOOSENODE *ptChooseNode;
	ITEMSTATSDATATBLDESC *ptDesc;
	ITEMSTXT *ptItemStats;
	ITEMSTXT *ptStat;
	int nBase, nNum, nCount, i;
	int &snGoldIndex = sgnGoldIndexA;

	if (!(sgbGoldIndexGuardA & 1))
	{
		sgbGoldIndexGuardA |= 1;
		snGoldIndex = D2CommonItemTblsFindItemIndexByCode(ITEMCODE_GOLD);
	}
#line 297
	D2ASSERT(snGoldIndex != -1);
	ptDesc = (ITEMSTATSDATATBLDESC *)D2CommonItemTblsGetItemStatsDesc();
	ptItemStats = ptDesc->ptMiscStart;
	nBase = ptItemStats - ptDesc->ptItemStats;
	nNum = &ptDesc->ptItemStats[ptDesc->nSize] - ptItemStats;
	nCount = 0;
#line 305
	D2ASSERT(ptItemStats);
	ptChooseList = atChooseList;
	for (i = 0, ptStat = ptItemStats; i < nNum; i++, ptStat++)
	{
		D2_OPAQUE(ptStat);
#line 308
		D2ASSERT(ptStat);
		if ((fType28 || ptStat->type != 40) && ITEMSCanSpawn(ptSeed, ptStat, nLevelId, nType, fNoRarity))
			ITEMSChooseListAdd(atChooseList, &nCount, nBase + i, ptStat);
	}
	D2_OPAQUE(ptChooseList);
#line 318
	D2ASSERT(ptChooseList);
	ptChooseNode = ITEMSChooseListPick(ptSeed, ptChooseList, nCount);
	D2_OPAQUE(ptChooseNode);
#line 321
	D2ASSERT(ptChooseNode);
	return ptChooseNode->nIndex;
}

extern void * __fastcall ITEMSGetUniqueList(GAME *ptGame, int nClass);
__inline void * __fastcall ITEMSGetUniqueList(GAME *ptGame, int nClass)
{
	D2ASSERT(ptGame);
	D2ASSERT(nClass < MAX_UNIQUE_ITEMS);
	return ptGame->aptUniqueItems[nClass];
}

BOOL __fastcall ITEMSRemoveUnique(GAME *ptGame, UNIT *hItem)
{
	ITEMUNIQUENODE *ptNode, *ptPrev, *ptHead, *ptNext;
	int nClass;

#line 739
	D2ASSERT(ptGame);
#line 740
	D2ASSERT(hItem);
	if (D2CommonITEMSDataGetModType(hItem) != ITEMQUAL_UNIQUE)
		return FALSE;
	nClass = D2CommonITEMSDataGetUniqueOrSetRow(hItem);
	ptHead = ptGame->aptUniqueItems[nClass];
	if (!ptHead)
		return FALSE;
	ptPrev = NULL;
	for (ptNode = ptHead; ptNode; ptNode = ptNode->ptNext)
	{
		if (ptNode->hItem == hItem)
		{
			ptNext = ptNode->ptNext;
#line 756
			D2FREEPOOL(ptGame->pMemPool, ptNode);
			if (!ptPrev)
				ptHead = ptNext;
			else
				ptPrev->ptNext = ptNext;
			ptGame->aptUniqueItems[nClass] = ptHead;
			return TRUE;
		}
		ptPrev = ptNode;
	}
#line 783
	D2ASSERT(0);
	return FALSE;
}

void __fastcall ITEMSRollMagicQuality(UNIT *hUnit, int nLevel, int nDivisor,
	DWORD dwClassId, DWORD *pdwQuality)
{
	GAME *ptGame;
	ITEMRATIOTXT *ptRatio;
	const int *pnQuality;
	int nChance, nQuality;

	if (nDivisor <= 0)
		return;
	ptGame = SUnitGetGame(hUnit);
#line 1406
	D2ASSERT(ptGame);
	ptRatio = (ITEMRATIOTXT *)((DWORD (__stdcall *)(DWORD, BYTE))D2CommonItemTblsGetItemRatioRaw)(dwClassId, ptGame->bDifficultyLevel);
#line 1408
	D2ASSERT(ptRatio);
	nChance = 0;
	nQuality = 0;
	for (pnQuality = sganItemsMagicQualities; (int)pnQuality < (int)&sganItemsMagicQualities[4]; pnQuality++)
	{
		switch (*pnQuality)
		{
		case ITEMQUAL_UNIQUE:
			nChance = ptRatio->Unique - nLevel;
			nQuality = ITEMQUAL_UNIQUE;
			break;
		case ITEMQUAL_RARE:
			nChance = ptRatio->Rare - nLevel;
			nQuality = ITEMQUAL_RARE;
			break;
		case ITEMQUAL_SET:
			nChance = ptRatio->Set - nLevel;
			nQuality = ITEMQUAL_SET;
			break;
		case ITEMQUAL_MAGIC:
			nChance = ptRatio->Magic - nLevel / ptRatio->MagicDivisor;
			nQuality = ITEMQUAL_MAGIC;
			break;
		}
		nChance = nChance * 100 / nDivisor;
		if (nChance <= 0)
			nChance = 1;
		if (!(nChance ? D2CommonSeedRand(&hUnit->tSeed, nChance) : 0))
		{
			*pdwQuality = nQuality;
			return;
		}
	}
}

static inline BOOL ITEMSMakeLowQuality(UNIT *hItem)
{
	LOWQUALITYITEMMODSDATATBLDESC *ptDataTblDesc;
	ITEMSTXT *ptItemStats;
	void *ptStatList;
	DWORD dwMaxDurability, dwDurability;
	int nValue;

	ptDataTblDesc = (LOWQUALITYITEMMODSDATATBLDESC *)D2CommonItemTblsGetLowQualityItemDesc();
#line 1336
	D2ASSERT(ptDataTblDesc);
	D2CommonITEMSDataSetUniqueOrSetRow(hItem, D2CommonSeedRand(D2CommonITEMSGetItemSeed(hItem),
		ptDataTblDesc->nSize));
	if (D2CommonITEMSTestTypeHasDurability(hItem))
	{
		ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
#line 1343
		D2ASSERT(ptItemStats);
		dwMaxDurability = ptItemStats->durability / 3;
		dwDurability = ITEMSRandom(&hItem->tSeed, dwMaxDurability / 2);
		dwDurability += dwMaxDurability / 2;
		if (!dwMaxDurability)
		{
			dwMaxDurability = 1;
			dwDurability = dwMaxDurability;
		}
		D2CommonStatsUnitSetValue(hItem, 72, dwDurability);
		D2CommonStatsUnitSetValue(hItem, 73, dwMaxDurability);
	}
	ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
#line 1357
	D2ASSERT(ptItemStats);
	ptStatList = (void *)D2CommonStatsFindListByMask(hItem, 64);
	if (D2CommonITEMSIsWeapon(hItem))
	{
		D2CommonStatsUnitSetValue(hItem, 22,
			ITEMSPercent(D2CommonStatsUnitGetBaseValue(hItem, 22), 75, 1));
		D2CommonStatsUnitSetValue(hItem, 21,
			ITEMSPercent(D2CommonStatsUnitGetBaseValue(hItem, 21), 75, 0));
		if (D2CommonITEMSGetTypeThrowable(hItem))
		{
			D2CommonStatsListSetValueB(ptStatList, 159,
				ITEMSPercent(D2CommonStatsGetTotalValue(hItem, 159), 75, 0));
			D2CommonStatsListSetValueB(ptStatList, 160,
				ITEMSPercent(D2CommonStatsGetTotalValue(hItem, 160), 75, 0));
		}
		return TRUE;
	}
	if (D2CommonITEMSIsArmor(hItem))
	{
		nValue = D2CommonStatsUnitGetBaseValue(hItem, 31) * 75 / 100;
		if (nValue <= 0)
			nValue = 1;
		D2CommonStatsUnitSetValue(hItem, 31, nValue);
		return TRUE;
	}
	return FALSE;
}

static inline BOOL ITEMSMakeUnique(GAME *ptGame, UNIT *hItem, ITEMCREATE *ptCreate)
{
	UNIQUEITEMMODSDATATBLDESC *ptDataTblDesc;
	ITEMSTXT *ptItemStats;
	UNIQUEITEMSTXT *ptUniqueMods;
	int nDurability, nMaxDurability;
	DWORD i;

	ptDataTblDesc = (UNIQUEITEMMODSDATATBLDESC *)D2CommonItemTblsGetUniqueItemDesc();
#line 1000
	D2ASSERT(ptDataTblDesc);
	ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
#line 1003
	D2ASSERT(ptItemStats);
	if (D2CommonITEMSTestTypeHasDurability(hItem))
	{
		nDurability = D2CommonStatsUnitGetValue(hItem, 72);
		nMaxDurability = D2CommonStatsUnitGetBaseValue(hItem, 73);
		nDurability = min(nDurability * 5, 255);
		D2CommonStatsUnitSetValue(hItem, 72, nDurability);
		nMaxDurability = min(nMaxDurability * 5, 255);
		D2CommonStatsUnitSetValue(hItem, 73, nMaxDurability);
	}
	if (ptCreate->dwRecreate)
	{
		ptUniqueMods = (UNIQUEITEMSTXT *)D2CommonItemTblsGetUniqueItem(ptCreate->dwFileIndex);
#line 1020
		D2ASSERTMSG(ptItemStats->code == ptUniqueMods->code, "Unique Item being created did not have nModClass set!");
		D2CommonITEMSDataSetUniqueOrSetRow(hItem, ptCreate->dwFileIndex);
		if (ITEMSAddUniqueToGame(ptGame, hItem, ptCreate->dwRecreate))
		{
			D2CommonITEMSDataSetFlags(hItem, 16, 0);
			D2CommonITEMSModsApply(3, hItem, ptUniqueMods, 0, 0);
			return TRUE;
		}
#line 1030
		D2FATAL("Could not insert a recreated unique item into game unique item list!");
	}
	for (i = 0; i < ptDataTblDesc->nSize; i++)
	{
		ptUniqueMods = (UNIQUEITEMSTXT *)D2CommonItemTblsGetUniqueItem(i);
#line 1038
		D2ASSERT(ptUniqueMods);
		if (ptItemStats->code == ptUniqueMods->code
			&& (!ITEMSIsUniqueInGame(ptGame, i) || ptItemStats->quest))
		{
			D2CommonITEMSDataSetUniqueOrSetRow(hItem, i);
			if (ITEMSAddUniqueToGame(ptGame, hItem, ptCreate->dwRecreate))
			{
				D2CommonITEMSDataSetFlags(hItem, 16, 0);
				D2CommonITEMSModsApply(3, hItem, ptUniqueMods, 0, 0);
				return TRUE;
			}
			D2CommonITEMSDataSetUniqueOrSetRow(hItem, -1);
		}
	}
	return FALSE;
}

BOOL __fastcall ITEMSApplyQuality(GAME *ptGame, UNIT **phItem, ITEMCREATE *ptCreate)
{
	DWORD dwSeed;
	DWORD i;
	DWORD dwQuality;

	D2CommonITEMSDataSetMagicPrefix(*phItem, -1);
	D2CommonITEMSDataSetMagicSuffix(*phItem, -1);
	for (i = 0; i < 3; i++)
	{
		D2CommonITEMSDataSetMagicPrefixAt(*phItem, -1, i);
		D2CommonITEMSDataSetMagicSuffixAt(*phItem, -1, i);
	}
	D2CommonITEMSDataSetRarePrefix(*phItem, -1);
	D2CommonITEMSDataSetRareSuffix(*phItem, -1);
	dwQuality = ITEMSRollQuality(*phItem, ptCreate);
	if (ptCreate->dwQuality)
	{
		dwQuality = ptCreate->dwQuality;
	}
	if (!D2CommonITEMSTestKeepsRolledQuality(*phItem))
	{
		D2CommonITEMSDataSetModType(*phItem, ITEMQUAL_NORMAL);
	}
	else
	{
		D2CommonITEMSDataSetModType(*phItem, dwQuality);
	}
	for (i = 0; i < sgnItemsMagicTypes; i++)
	{
		if (D2CommonITEMSGetType(*phItem) == sgadwItemsMagicTypes[i]
			&& !D2CommonITEMSTestModTypeMagicOrAbove(*phItem))
		{
			D2CommonITEMSDataSetModType(*phItem, ITEMQUAL_MAGIC);
			break;
		}
	}
	for (i = 0; i < sgnItemsNormalTypes; i++)
	{
		if (D2CommonITEMSGetType(*phItem) == sgadwItemsNormalTypes[i])
		{
			D2CommonITEMSDataSetModType(*phItem, ITEMQUAL_NORMAL);
			break;
		}
	}
	switch (D2CommonITEMSDataGetModType(*phItem))
	{
	case ITEMQUAL_LOW:
		dwSeed = D2CommonSeedGetLow(D2CommonITEMSGetItemSeed(*phItem));
		if (ITEMSMakeLowQuality(*phItem))
		{
			break;
		}
		ITEMSMakeNormalSeeded(ptGame, ptCreate, phItem, dwSeed);
		break;
	case ITEMQUAL_NORMAL:
		ITEMSMakeNormal(ptGame, *phItem, ptCreate);
		break;
	case ITEMQUAL_HIQUALITY:
		D2CommonITEMSDataSetUniqueOrSetRow(*phItem, -1);
		dwSeed = D2CommonSeedGetLow(D2CommonITEMSGetItemSeed(*phItem));
		if (ITEMSMakeHiQuality(ptGame, *phItem, ptCreate))
		{
			break;
		}
		ITEMSMakeNormalSeeded(ptGame, ptCreate, phItem, dwSeed);
		break;
	case ITEMQUAL_RARE:
		D2CommonITEMSDataSetRarePrefix(*phItem, -1);
		D2CommonITEMSDataSetRareSuffix(*phItem, -1);
		dwSeed = D2CommonSeedGetLow(D2CommonITEMSGetItemSeed(*phItem));
		if (ITEMSMakeRare(*phItem, ptCreate))
		{
			break;
		}
		if (ITEMSTryMagic(ptGame, ptCreate, phItem, &dwSeed))
		{
			break;
		}
		if (ITEMSTryHiQuality(ptGame, ptCreate, phItem, &dwSeed))
		{
			break;
		}
		ITEMSMakeNormalSeeded(ptGame, ptCreate, phItem, dwSeed);
		break;
	case ITEMQUAL_MAGIC:
		dwSeed = D2CommonSeedGetLow(D2CommonITEMSGetItemSeed(*phItem));
		if (ITEMSMakeMagic(phItem, ptCreate))
		{
			break;
		}
		if (ITEMSTryHiQuality(ptGame, ptCreate, phItem, &dwSeed))
		{
			break;
		}
		ITEMSMakeNormalSeeded(ptGame, ptCreate, phItem, dwSeed);
		break;
	case ITEMQUAL_UNIQUE:
		D2CommonITEMSDataSetUniqueOrSetRow(*phItem, -1);
		dwSeed = D2CommonSeedGetLow(D2CommonITEMSGetItemSeed(*phItem));
		if (ITEMSMakeUnique(ptGame, *phItem, ptCreate))
		{
			break;
		}
		if (ITEMSTryRare(ptGame, ptCreate, phItem, &dwSeed))
		{
			break;
		}
		if (ITEMSTryMagic(ptGame, ptCreate, phItem, &dwSeed))
		{
			break;
		}
		if (ITEMSTryHiQuality(ptGame, ptCreate, phItem, &dwSeed))
		{
			break;
		}
		ITEMSMakeNormalSeeded(ptGame, ptCreate, phItem, dwSeed);
		break;
	case ITEMQUAL_SET:
		D2CommonITEMSDataSetUniqueOrSetRow(*phItem, -1);
		dwSeed = D2CommonSeedGetLow(D2CommonITEMSGetItemSeed(*phItem));
		if (ITEMSMakeSet(ptGame, *phItem))
		{
			break;
		}
		if (ITEMSTryMagic(ptGame, ptCreate, phItem, &dwSeed))
		{
			break;
		}
		if (ITEMSTryHiQuality(ptGame, ptCreate, phItem, &dwSeed))
		{
			break;
		}
		ITEMSMakeNormalSeeded(ptGame, ptCreate, phItem, dwSeed);
		return TRUE;
	default:
#line 1746
		D2ASSERT(0);
		return FALSE;
	}
	return TRUE;
}

BOOL __fastcall ITEMSIsUniqueInGame(GAME *ptGame, int nClass)
{
#line 655
	D2ASSERT(ptGame);
#line 656
	D2ASSERT(nClass < MAX_UNIQUE_ITEMS);
	return ptGame->aptUniqueItems[nClass] != NULL;
}

BOOL __fastcall ITEMSAddUniqueToGame(GAME *ptGame, UNIT *hItem, BOOL fRecreate)
{
	ITEMSTXT *ptItemStats;
	ITEMUNIQUENODE *ptNewNode;
	ITEMUNIQUENODE *ptHead;
	int nModClass;

#line 687
	D2ASSERT(ptGame);
	ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
	if (D2CommonITEMSDataGetModType(hItem) == ITEMQUAL_UNIQUE
		&& ITEMSIsUniqueInGame(ptGame, D2CommonITEMSDataGetUniqueOrSetRow(hItem)) && !fRecreate
		&& !ptItemStats->quest)
		return FALSE;
	nModClass = D2CommonITEMSDataGetUniqueOrSetRow(hItem);
#line 695
	D2ASSERT(nModClass < MAX_UNIQUE_ITEMS);
	ptHead = ptGame->aptUniqueItems[nModClass];
#line 697
	ptNewNode = (ITEMUNIQUENODE *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(ITEMUNIQUENODE));
#line 698
	D2ASSERT(ptNewNode);
	memset(ptNewNode, 0, sizeof(ITEMUNIQUENODE));
	ptNewNode->hItem = hItem;
	ptNewNode->ptNext = ptHead;
	ptGame->aptUniqueItems[nModClass] = ptNewNode;
	return TRUE;
}

static inline int ITEMSChooseRarePrefix(UNIT *hItem)
{
	ITEMCHOOSENODE atChooseList[MAX_CHOOSE_LIST];
	ITEMCHOOSENODE *ptChooseNode;
	RAREITEMMODSDATATBLDESC *ptDesc;
	RAREAFFIXTXT *ptPrefixMods;
	RAREAFFIXTXT *ptMod;
	int nBase, nNum, nCount, i;
	int nFindIndex;
	D2SEED *ptSeed;

	ptDesc = (RAREITEMMODSDATATBLDESC *)D2CommonItemTblsGetRareAffixDesc();
	ptPrefixMods = ptDesc->ptPrefixes;
	nBase = ptPrefixMods - ptDesc->ptItemMods;
	nNum = &ptDesc->ptItemMods[ptDesc->nSize] - ptPrefixMods;
	nCount = 0;
#line 628
	D2ASSERT(ptPrefixMods);
	for (i = 0; i < nNum; i++)
	{
		ptMod = &ptPrefixMods[i];
#line 631
		D2ASSERT(ptMod);
		if (D2CommonITEMSModsTestRareFits(hItem, ptMod))
			ITEMSChooseListAdd(atChooseList, &nCount, nBase + i, ptMod);
	}
	if (nCount <= 0)
		return -1;
	ptSeed = (D2SEED *)D2CommonITEMSGetItemSeed(hItem);
	nFindIndex = D2CommonSeedRand(ptSeed, nCount);
	ptChooseNode = ITEMSChooseListAt(atChooseList, nCount, nFindIndex);
	D2_OPAQUE(ptChooseNode);
#line 644
	D2ASSERT(ptChooseNode);
	return ptChooseNode->nIndex;
}

static inline int ITEMSChooseRareSuffix(UNIT *hItem)
{
	ITEMCHOOSENODE atChooseList[MAX_CHOOSE_LIST];
	ITEMCHOOSENODE *ptChooseNode;
	RAREITEMMODSDATATBLDESC *ptDesc;
	RAREAFFIXTXT *ptSuffixMods;
	RAREAFFIXTXT *ptMod;
	int nBase, nNum, nCount, i;
	int nFindIndex;
	D2SEED *ptSeed;

	ptDesc = (RAREITEMMODSDATATBLDESC *)D2CommonItemTblsGetRareAffixDesc();
	ptSuffixMods = ptDesc->ptSuffixes;
	nBase = ptSuffixMods - ptDesc->ptItemMods;
	nNum = ptDesc->ptPrefixes - ptSuffixMods;
	nCount = 0;
#line 590
	D2ASSERT(ptSuffixMods);
	for (i = 0; i < nNum; i++)
	{
		ptMod = &ptSuffixMods[i];
#line 593
		D2ASSERT(ptMod);
		if (D2CommonITEMSModsTestRareFits(hItem, ptMod))
			ITEMSChooseListAdd(atChooseList, &nCount, nBase + i, ptMod);
	}
	if (nCount <= 0)
		return -1;
	ptSeed = (D2SEED *)D2CommonITEMSGetItemSeed(hItem);
	nFindIndex = D2CommonSeedRand(ptSeed, nCount);
	ptChooseNode = ITEMSChooseListAt(atChooseList, nCount, nFindIndex);
	D2_OPAQUE(ptChooseNode);
#line 606
	D2ASSERT(ptChooseNode);
	return ptChooseNode->nIndex;
}

BOOL __fastcall ITEMSMakeRare(UNIT *hItem, ITEMCREATE *ptCreate)
{
	D2SEED *ptSeed;
	DWORD dwSide;
	int nPrefix, nSuffix, nPrefixes, nSuffixes, nMods, nMod, nOld, j;

	nPrefix = ITEMSChooseRarePrefix(hItem);
	nSuffix = ITEMSChooseRareSuffix(hItem);
	if (nPrefix == -1 || nSuffix == -1)
		return FALSE;
	D2CommonITEMSDataSetRarePrefix(hItem, nPrefix);
	D2CommonITEMSDataSetRareSuffix(hItem, nSuffix);
	nPrefixes = 0;
	nSuffixes = 0;
	ptSeed = (D2SEED *)D2CommonITEMSGetItemSeed(hItem);
	for (nMods = D2CommonSeedRand(ptSeed, 3) + 4; nMods; nMods--)
	{
		dwSide = D2CommonSeedNext(ptSeed) & 1;
		if (nPrefixes != 3 && (nSuffixes == 3 || !dwSide))
		{
			int nTries;
			BOOL fSame;

			nTries = 0;
			for (;;)
			{
				nMod = ITEMSChooseMagicPrefix(hItem, TRUE, FALSE);
				fSame = FALSE;
				for (j = 0; j < 3; j++)
				{
					nOld = D2CommonITEMSDataGetMagicPrefixAt(hItem, j);
					if (nOld != -1)
					{
						MAGICAFFIXTXT *ptOld, *ptNew;

						ptOld = (MAGICAFFIXTXT *)D2CommonItemTblsGetMagicAffix(nOld);
						ptNew = (MAGICAFFIXTXT *)D2CommonItemTblsGetMagicAffix(nMod);
						if (nMod == nOld || ptNew->group == ptOld->group)
							fSame = TRUE;
					}
				}
				if (fSame)
				{
					if (nTries++ > 250)
					{
						D2CommonITEMSDataSetMagicPrefixAt(hItem, -1, nPrefixes);
						break;
					}
				}
				else
				{
					D2CommonITEMSDataSetMagicPrefixAt(hItem, nMod, nPrefixes);
					nPrefixes++;
					break;
				}
			}
		}
		else
		{
			int nTries;
			BOOL fSame;

			nTries = 0;
			for (;;)
			{
				nMod = ITEMSChooseMagicSuffix(hItem, TRUE, FALSE);
				fSame = FALSE;
				for (j = 0; j < 3; j++)
				{
					nOld = D2CommonITEMSDataGetMagicSuffixAt(hItem, j);
					if (nOld != -1)
					{
						MAGICAFFIXTXT *ptOld, *ptNew;

						ptOld = (MAGICAFFIXTXT *)D2CommonItemTblsGetMagicAffix(nOld);
						ptNew = (MAGICAFFIXTXT *)D2CommonItemTblsGetMagicAffix(nMod);
						if (nMod == nOld || ptNew->group == ptOld->group)
							fSame = TRUE;
					}
				}
				if (fSame)
				{
					if (nTries++ > 250)
					{
						D2CommonITEMSDataSetMagicSuffixAt(hItem, -1, nSuffixes);
						break;
					}
				}
				else
				{
					D2CommonITEMSDataSetMagicSuffixAt(hItem, nMod, nSuffixes);
					nSuffixes++;
					break;
				}
			}
		}
	}
	D2CommonITEMSDataSetFlags(hItem, 16, 0);
	for (j = 0; j < 3; j++)
	{
		nOld = D2CommonITEMSDataGetMagicPrefixAt(hItem, j);
		if (nOld != -1)
		{
			MAGICAFFIXTXT *ptOld;

			ptOld = (MAGICAFFIXTXT *)D2CommonItemTblsGetMagicAffix(nOld);
			D2CommonITEMSModsApply(0, hItem, ptOld, 0, 0);
		}
		nOld = D2CommonITEMSDataGetMagicSuffixAt(hItem, j);
		if (nOld != -1)
		{
			MAGICAFFIXTXT *ptOld;

			ptOld = (MAGICAFFIXTXT *)D2CommonItemTblsGetMagicAffix(nOld);
			D2CommonITEMSModsApply(0, hItem, ptOld, 0, 0);
		}
	}
	D2CommonITEMSAddRandomSkillStatsByType(hItem, ptCreate->nLevel);
	return TRUE;
}

int __fastcall ITEMSChooseMagicSuffix(UNIT *hItem, BOOL fForce, BOOL fApply)
{
	ITEMCHOOSENODE atChooseList[MAX_CHOOSE_LIST];
	ITEMCHOOSENODE *ptChooseNode;
	MAGICITEMMODSDATATBLDESC *ptDesc;
	MAGICAFFIXTXT *ptSuffixMods;
	MAGICAFFIXTXT *ptMod;
	int nBase, nNum, nCount, nLevel, nIndex, i;
	BOOL fFound;

	ptDesc = (MAGICITEMMODSDATATBLDESC *)D2CommonItemTblsGetMagicAffixDesc();
	nBase = ptDesc->ptSuffixes - ptDesc->ptItemMods;
	nCount = 0;
	nNum = ptDesc->ptPrefixes - ptDesc->ptSuffixes;
	nIndex = -1;
	if ((D2CommonSeedNext((DWORD)D2CommonITEMSGetItemSeed(hItem)) & 1) || fForce)
	{
		nLevel = D2CommonITEMSGetLevelId(hItem);
		ptSuffixMods = ptDesc->ptSuffixes;
#line 456
		D2ASSERT(ptSuffixMods);
		for (i = 0, ptMod = ptSuffixMods; i < nNum; i++, ptMod++)
		{
			D2_OPAQUE(ptMod);
#line 459
			D2ASSERT(ptMod);
			if (ptMod->level <= nLevel + 2 && ptMod->spawnable
				&& D2CommonITEMSModsTestMagicFits(hItem, ptMod))
				ITEMSChooseListAdd(atChooseList, &nCount, nBase + i, ptMod);
		}
		if (nCount <= 0)
			return -1;
		{
			D2SEED *ptSeed = (D2SEED *)D2CommonITEMSGetItemSeed(hItem);
			int nFindIndex = D2CommonSeedRand(ptSeed, nCount);

			ptChooseNode = ITEMSChooseListAt(atChooseList, nCount, nFindIndex);
		}
		D2_OPAQUE(ptChooseNode);
#line 476
		D2ASSERT(ptChooseNode);
		nIndex = ptChooseNode->nIndex;
		if (sgnItemsForcedSuffix != -1)
		{
			ITEMCHOOSENODE *ptNode;

			fFound = FALSE;
			for (i = 0; i < nCount; i++)
			{
				ptNode = &atChooseList[i];
				if (ptNode->nIndex == nBase + sgnItemsForcedSuffix)
				{
					fFound = TRUE;
					break;
				}
			}
#line 495
			D2ASSERT(fFound);
			ptChooseNode = ptNode;
			nIndex = ptChooseNode->nIndex;
			D2CommonITEMSDataSetMagicSuffix(hItem, nIndex);
		}
		if (fApply)
			D2CommonITEMSModsApply(0, hItem, ptChooseNode->ptStat, 0, 0);
	}
	return nIndex;
}

int __fastcall ITEMSChooseMagicPrefix(UNIT *hItem, BOOL fForce, BOOL fApply)
{
	ITEMCHOOSENODE atChooseList[MAX_CHOOSE_LIST];
	ITEMCHOOSENODE *ptChooseNode;
	MAGICITEMMODSDATATBLDESC *ptDesc;
	MAGICAFFIXTXT *ptPrefixMods;
	MAGICAFFIXTXT *ptMod;
	int nBase, nNum, nCount, nLevel, nIndex, i;
	BOOL fFound;
	D2SEED *ptCoinSeed;

	ptDesc = (MAGICITEMMODSDATATBLDESC *)D2CommonItemTblsGetMagicAffixDesc();
	nBase = ptDesc->ptPrefixes - ptDesc->ptItemMods;
	nNum = &ptDesc->ptItemMods[ptDesc->nSize] - ptDesc->ptPrefixes;
	nIndex = -1;
	nCount = 0;
	ptCoinSeed = (D2SEED *)D2CommonITEMSGetItemSeed(hItem);
	if (D2CommonSeedRand(ptCoinSeed, 2) || fForce)
	{
		nLevel = D2CommonITEMSGetLevelId(hItem);
		ptPrefixMods = ptDesc->ptPrefixes;
#line 525
		D2ASSERT(ptPrefixMods);
		for (i = 0; i < nNum; i++)
		{
			ptMod = &ptPrefixMods[i];
			D2_OPAQUE(ptMod);
#line 528
			D2ASSERT(ptMod);
			if (ptMod->level <= nLevel + 2 && ptMod->spawnable
				&& D2CommonITEMSModsTestMagicFits(hItem, ptMod))
				ITEMSChooseListAdd(atChooseList, &nCount, nBase + i, ptMod);
		}
		if (nCount <= 0)
			return -1;
		{
			D2SEED *ptSeed = (D2SEED *)D2CommonITEMSGetItemSeed(hItem);
			int nFindIndex = D2CommonSeedRand(ptSeed, nCount);

			ptChooseNode = ITEMSChooseListAt(atChooseList, nCount, nFindIndex);
		}
		D2_OPAQUE(ptChooseNode);
#line 545
		D2ASSERT(ptChooseNode);
		nIndex = ptChooseNode->nIndex;
		if (sgnItemsForcedPrefix != -1)
		{
			ITEMCHOOSENODE *ptNode;

			fFound = FALSE;
			for (i = 0; i < nCount; i++)
			{
				ptNode = &atChooseList[i];
				if (ptNode->nIndex == nBase + sgnItemsForcedPrefix)
				{
					fFound = TRUE;
					break;
				}
			}
#line 563
			D2ASSERT(fFound);
			ptChooseNode = ptNode;
			nIndex = ptChooseNode->nIndex;
			D2CommonITEMSDataSetMagicPrefix(hItem, nIndex);
		}
		if (fApply)
			D2CommonITEMSModsApply(0, hItem, ptChooseNode->ptStat, 0, 0);
	}
	return nIndex;
}

BOOL __fastcall ITEMSMakeMagic(UNIT **phItem, ITEMCREATE *ptCreate)
{
	BOOL fNeedSuffix;

	fNeedSuffix = FALSE;
	if (sgnItemsForcedPrefix != -1)
		D2CommonITEMSDataSetMagicPrefixAt(*phItem, ITEMSChooseMagicPrefix(*phItem, TRUE, TRUE), 0);
	else if (sgnItemsForcedSuffix == -1)
	{
		D2CommonITEMSDataSetMagicPrefixAt(*phItem, ITEMSChooseMagicPrefix(*phItem, FALSE, TRUE), 0);
		if (D2CommonITEMSDataGetMagicPrefixAt(*phItem, 0) == (DWORD)-1)
			fNeedSuffix = TRUE;
	}
	if (sgnItemsForcedSuffix != -1)
		D2CommonITEMSDataSetMagicSuffixAt(*phItem, ITEMSChooseMagicSuffix(*phItem, TRUE, TRUE), 0);
	else if (sgnItemsForcedPrefix == -1)
		D2CommonITEMSDataSetMagicSuffixAt(*phItem, ITEMSChooseMagicSuffix(*phItem, fNeedSuffix, TRUE), 0);
	if (D2CommonITEMSDataGetMagicPrefixAt(*phItem, 0) == (DWORD)-1
		&& D2CommonITEMSDataGetMagicSuffixAt(*phItem, 0) == (DWORD)-1)
		return FALSE;
	D2CommonITEMSDataSetFlags(*phItem, 16, 0);
	D2CommonITEMSAddRandomSkillStatsByType(*phItem, ptCreate->nLevel);
	return TRUE;
}

BOOL __fastcall ITEMSMakeSet(GAME *ptGame, UNIT *hItem)
{
	SETITEMMODSDATATBLDESC *ptDataTblDesc;
	ITEMSTXT *ptItemStats;
	SETITEMSTXT *ptSetMods;
	SETITEMINFO *ptSetInfo;
	DWORD dwSeed, dwStart, j;
	int nDurability, nDur2, nMaxDurability, nSet, nItem, nIndex, i;

	ptDataTblDesc = (SETITEMMODSDATATBLDESC *)D2CommonItemTblsGetSetItemDesc();
#line 1077
	D2ASSERT(ptDataTblDesc);
	ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
#line 1080
	D2ASSERT(ptItemStats);
#line 1082
	D2ASSERT(ptDataTblDesc->nSize <= NUM_ITEMSETS);
	dwSeed = D2CommonSeedGetLow(D2CommonITEMSGetItemSeed(hItem));
	dwStart = D2CommonSeedRand(D2CommonITEMSGetItemSeed(hItem), ptDataTblDesc->nSize);

	nDurability = D2CommonStatsUnitGetValue(hItem, 72);
	nMaxDurability = D2CommonStatsUnitGetBaseValue(hItem, 73);
	nDur2 = nDurability * 2;
	if (nDur2 >= 255)
		nDur2 = 255;
	D2CommonStatsUnitSetValue(hItem, 72, nDur2);
	nDur2 = nMaxDurability * 2;
	if (nDur2 >= 255)
		nDur2 = 255;
	D2CommonStatsUnitSetValue(hItem, 73, nDur2);
	nSet = -1;
	nItem = -1;
	for (i = 0; i < (int)ptDataTblDesc->nSize; i++)
	{
		nIndex = (i + dwStart) % ptDataTblDesc->nSize;
		ptSetMods = (SETITEMSTXT *)D2CommonItemTblsGetSetItem(nIndex);
#line 1108
		D2ASSERT(ptSetMods);
		for (j = 0, ptSetInfo = ptSetMods->atItems; j < 6; j++, ptSetInfo++)
		{
			D2_OPAQUE(ptSetInfo);
#line 1112
			D2ASSERT(ptSetInfo);
			if (ptItemStats->code == ptSetInfo->code)
			{
				nItem = j;
				nSet = nIndex;
				break;
			}
		}
	}
	if (nItem != -1)
	{
		D2CommonITEMSDataSetUniqueOrSetRow(hItem, nSet);
		D2CommonITEMSDataSetFlags(hItem, 16, 0);
		ptSetMods = (SETITEMSTXT *)D2CommonItemTblsGetSetItem(nSet);
		D2CommonITEMSModsApply(4, hItem, ptSetMods, nItem, 0);
		return TRUE;
	}
	D2CommonSeedSet(D2CommonITEMSGetItemSeed(hItem), dwSeed);
	return FALSE;
}

static inline BOOL ITEMSAnyUntried(BYTE *abTried, int nCount)
{
	int j;

	for (j = 0; j < nCount; j++)
		if (!abTried[j])
			return TRUE;
	return FALSE;
}

BOOL __fastcall ITEMSMakeHiQuality(GAME *ptGame, UNIT *hItem, ITEMCREATE *ptCreate)
{
	HIQUALITYITEMMODSDATATBLDESC *ptDataTblDesc;
	ITEMSTXT *ptItemStats;
	struct QUALITYITEMSTXT *ptHiqualityMods;
	BYTE abTried[10];
	BYTE *pbTried;
	int nCount, nIndex, j;

	ptDataTblDesc = (HIQUALITYITEMMODSDATATBLDESC *)D2CommonItemTblsGetQualityItemDesc();
#line 1146
	D2ASSERT(ptDataTblDesc);
	ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
#line 1149
	D2ASSERT(ptItemStats);
	nCount = ptDataTblDesc->nSize;
	memset(abTried, 0, sizeof(abTried));
	if (D2CommonITEMSGetTypeThrowable(hItem) || ptItemStats->nodurability)
		nCount = 4;
	for (;;)
	{
		nIndex = ITEMSRandom((D2SEED *)D2CommonITEMSGetItemSeed(hItem), nCount);
		pbTried = &abTried[nIndex];
		if (*pbTried)
			continue;
		ptHiqualityMods = (struct QUALITYITEMSTXT *)D2CommonItemTblsGetQualityItem(nIndex);
#line 1167
		D2ASSERT(ptHiqualityMods);
		if (D2CommonITEMSModsTestQualityFits(hItem, ptHiqualityMods))
			break;
		*pbTried = 1;
		if (!ITEMSAnyUntried(abTried, nCount))
			return FALSE;

	}
	D2CommonITEMSDataSetUniqueOrSetRow(hItem, nIndex);
	D2CommonITEMSModsApply(1, hItem, ptHiqualityMods, 0, 0);
	D2CommonITEMSAddRandomSkillStatsByType(hItem, ptCreate->nLevel);
	return TRUE;
}

void __fastcall ITEMSMakeNormal(GAME *ptGame, UNIT *hItem, ITEMCREATE *ptCreate)
{
	DWORD dwFileIdx;

	switch (D2CommonITEMSGetType(hItem))
	{
	case 40:
		dwFileIdx = ptCreate->dwFileIndex;
		if (ptCreate->hSpawner)
			dwFileIdx = ptCreate->hSpawner->dwClassId;
		D2CommonITEMSDataSetUniqueOrSetRow(hItem, dwFileIdx);
		return;
	case 7:
		dwFileIdx = ptCreate->dwFileIndex;
		if (ptCreate->hSpawner)
			dwFileIdx = ptCreate->hSpawner->dwClassId;
		D2CommonITEMSDataSetUniqueOrSetRow(hItem, dwFileIdx);
		D2CommonITEMSDataSetFlags(hItem, 65536, 1);
		return;
	case 22:
		D2CommonITEMSDataSetMagicSuffixAt(hItem, ITEMSFindBook(hItem, TRUE), 0);
		return;
	case 18:
		D2CommonITEMSDataSetMagicSuffixAt(hItem, ITEMSFindBook(hItem, FALSE), 0);
		return;
	}
	D2CommonITEMSAddRandomSkillStatsByType(hItem, ptCreate->nLevel);
	if (D2CommonITEMSGetTypeHasInv(hItem) && !D2CommonITEMSGetTypeStackable(hItem)
		&& (D2CommonITEMSIsWeapon(hItem) || D2CommonITEMSGetType(hItem) == 2
			|| D2CommonITEMSGetType(hItem) == 37)
		&& D2CommonSeedRand(D2CommonITEMSGetItemSeed(hItem), 100) < 33 && !ptCreate->dwNoSockets)
		D2CommonITEMSDataSetFlags(hItem, 2048, 1);
}

int __fastcall ITEMSFindBook(UNIT *hItem, BOOL fScroll)
{
	ITEMSTXT *ptItemStats;
	BOOKITEMMODSDATATBLDESC *ptBookModsDataTblDesc;
	BOOKSTXT *ptBookMods;
	BOOKSTXT *ptMod;
	int nIndex;
	DWORD i;

	ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
#line 798
	D2ASSERT(ptItemStats);
	ptBookModsDataTblDesc = (BOOKITEMMODSDATATBLDESC *)D2CommonItemTblsGetBookDesc();
#line 800
	D2ASSERT(ptBookModsDataTblDesc);
	ptBookMods = ptBookModsDataTblDesc->ptItemMods;
	nIndex = -1;
#line 803
	D2ASSERT(ptBookMods);
	for (i = 0, ptMod = ptBookMods; i < ptBookModsDataTblDesc->nSize; i++, ptMod++)
	{
		D2_OPAQUE(ptMod);
#line 806
		D2ASSERT(ptMod);
		if (fScroll)
		{
			if (ptItemStats->code == ptMod->scrollspellcode)
			{
				nIndex = i;
				break;
			}
		}
		else
		{
			if (ptItemStats->code == ptMod->bookspellcode)
			{
				nIndex = i;
				break;
			}
		}
	}
	return nIndex;
}

int __fastcall ITEMSPercent(int nValue, int nPercent,
	int nMin)
{
	int nResult;

	nResult = nValue * nPercent / 100;
	if (nResult < nMin)
		return nMin;
	return nResult;
}

int __fastcall ITEMSRollQuality(UNIT *hItem, ITEMCREATE *ptCreate)
{
	GAME *ptGame;
	ITEMRATIOTXT *ptRatio;
	int nChance, nQuality;
	DWORD i;

	ptGame = SUnitGetGame(hItem);
#line 1461
	D2ASSERT(ptGame);
	ptRatio = (ITEMRATIOTXT *)((DWORD (__stdcall *)(DWORD, BYTE))D2CommonItemTblsGetItemRatioRaw)(hItem->dwClassId, ptGame->bDifficultyLevel);
#line 1463
	D2ASSERT(ptRatio);
	nChance = 0;
	nQuality = ITEMQUAL_LOW;
	for (i = 0; i < sgnItemsQualities; i++)
	{
		switch (sgadwItemsQualities[i])
		{
		case ITEMQUAL_UNIQUE:
			nChance = ptRatio->Unique - ptCreate->nLevel;
			nQuality = ITEMQUAL_UNIQUE;
			break;
		case ITEMQUAL_RARE:
			nChance = ptRatio->Rare - ptCreate->nLevel;
			nQuality = ITEMQUAL_RARE;
			break;
		case ITEMQUAL_SET:
			nChance = ptRatio->Set - ptCreate->nLevel;
			nQuality = ITEMQUAL_SET;
			break;
		case ITEMQUAL_MAGIC:
			nChance = ptRatio->Magic - ptCreate->nLevel / ptRatio->MagicDivisor;
			nQuality = ITEMQUAL_MAGIC;
			break;
		case ITEMQUAL_HIQUALITY:
			nChance = ptRatio->HiQuality - ptCreate->nLevel / ptRatio->HiQualityDivisor;
			nQuality = ITEMQUAL_HIQUALITY;
			break;
		case ITEMQUAL_NORMAL:
			nChance = ptRatio->Normal - ptCreate->nLevel / ptRatio->NormalDivisor;
			nQuality = ITEMQUAL_NORMAL;
			break;
		}
		if (nChance <= 0)
			nChance = 1;
		if (!ITEMSRandom((D2SEED *)D2CommonITEMSGetItemSeed(hItem), nChance))
			return nQuality;
	}
	if (ptCreate->dwHiQualityDefault)
		return ITEMQUAL_HIQUALITY;
	else
		return ITEMQUAL_LOW;
}

void __fastcall ITEMSClearMods(UNIT **phItem)
{
	DWORD i;

	D2CommonITEMSDataSetMagicPrefix(*phItem, -1);
	D2CommonITEMSDataSetMagicSuffix(*phItem, -1);
	for (i = 0; i < 3; i++)
	{
		D2CommonITEMSDataSetMagicPrefixAt(*phItem, -1, i);
		D2CommonITEMSDataSetMagicSuffixAt(*phItem, -1, i);
	}
	D2CommonITEMSDataSetRarePrefix(*phItem, -1);
	D2CommonITEMSDataSetRareSuffix(*phItem, -1);
	D2CommonITEMSDataSetUniqueOrSetRow(*phItem, -1);
}

void __fastcall ITEMSMakeNormalSeeded(GAME *ptGame, ITEMCREATE *ptCreate, UNIT **phItem,
	DWORD dwSeed)
{
	ITEMSClearMods(phItem);
	D2CommonITEMSSetItemSeedValue(*phItem, dwSeed);
	D2CommonSeedSet(D2CommonITEMSGetItemSeed(*phItem), dwSeed);
	ITEMSRollQuality(*phItem, ptCreate);
	D2CommonITEMSDataSetModType(*phItem, ITEMQUAL_NORMAL);
	ptCreate->dwQuality = ITEMQUAL_NORMAL;
	ITEMSMakeNormal(ptGame, *phItem, ptCreate);
}

BOOL __fastcall ITEMSTryHiQuality(GAME *ptGame, ITEMCREATE *ptCreate, UNIT **phItem,
	DWORD *pdwSeed)
{
	ITEMSClearMods(phItem);
	D2CommonITEMSSetItemSeedValue(*phItem, *pdwSeed);
	D2CommonSeedSet(D2CommonITEMSGetItemSeed(*phItem), *pdwSeed);
	ITEMSRollQuality(*phItem, ptCreate);
	D2CommonITEMSDataSetModType(*phItem, ITEMQUAL_HIQUALITY);
	ptCreate->dwQuality = ITEMQUAL_HIQUALITY;
	*pdwSeed = D2CommonSeedGetLow(D2CommonITEMSGetItemSeed(*phItem));
	return ITEMSMakeHiQuality(ptGame, *phItem, ptCreate);
}

BOOL __fastcall ITEMSTryMagic(GAME *ptGame, ITEMCREATE *ptCreate, UNIT **phItem,
	DWORD *pdwSeed)
{
	ITEMSClearMods(phItem);
	D2CommonITEMSSetItemSeedValue(*phItem, *pdwSeed);
	D2CommonSeedSet(D2CommonITEMSGetItemSeed(*phItem), *pdwSeed);
	ITEMSRollQuality(*phItem, ptCreate);
	D2CommonITEMSDataSetModType(*phItem, ITEMQUAL_MAGIC);
	ptCreate->dwQuality = ITEMQUAL_MAGIC;
	*pdwSeed = D2CommonSeedGetLow(D2CommonITEMSGetItemSeed(*phItem));
	return ITEMSMakeMagic(phItem, ptCreate);
}

BOOL __fastcall ITEMSTryRare(GAME *ptGame, ITEMCREATE *ptCreate, UNIT **phItem,
	DWORD *pdwSeed)
{
	ITEMSClearMods(phItem);
	D2CommonITEMSSetItemSeedValue(*phItem, *pdwSeed);
	D2CommonSeedSet(D2CommonITEMSGetItemSeed(*phItem), *pdwSeed);
	ITEMSRollQuality(*phItem, ptCreate);
	D2CommonITEMSDataSetModType(*phItem, ITEMQUAL_RARE);
	ptCreate->dwQuality = ITEMQUAL_RARE;
	*pdwSeed = D2CommonSeedGetLow(D2CommonITEMSGetItemSeed(*phItem));
	return ITEMSMakeRare(*phItem, ptCreate);
}

static inline void ITEMSInitArmorClass(UNIT *hItem)
{
	ITEMSTXT *ptItemStats;
	DWORD nACValue, nMaxAC;
	int nMinAC;

	ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
#line 409
	D2ASSERT(ptItemStats);
	nMinAC = ptItemStats->minac;
	nMaxAC = ptItemStats->maxac;
	nACValue = ITEMSRandom(&hItem->tSeed, nMaxAC - nMinAC) + nMinAC;
#line 414
	D2ASSERT(nACValue <= nMaxAC);
	D2CommonStatsUnitSetValue(hItem, 31, nACValue);
}

static inline void ITEMSInitWeaponDamage(UNIT *hItem)
{
	ITEMSTXT *ptItemStats;
	void *ptStatList;

	ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
#line 427
	D2ASSERT(ptItemStats);
	D2CommonStatsUnitSetValue(hItem, 22, ptItemStats->maxdam);
	D2CommonStatsUnitSetValue(hItem, 21, ptItemStats->mindam);
	if (ptItemStats->maxmisdam)
	{
		ptStatList = (void *)D2CommonStatsFindListByMask(hItem, 64);
		D2CommonStatsListSetValueA(ptStatList, 159, ptItemStats->minmisdam);
		D2CommonStatsListSetValueA(ptStatList, 160, ptItemStats->maxmisdam);
	}
}

BOOL __fastcall ITEMSInitStats(GAME *ptGame, UNIT **phItem, ITEMCREATE *ptCreate)
{
	ITEMSTXT *ptItemStats;
	UNIT *hItem;
	DWORD dwQuality, dwGold;
	int nGold, nDurability, nMaxDurability, nQuantity, nMin, nMax;
	int nMinStack;
	int nMaxStack;

	dwQuality = D2CommonITEMSDataGetModType(*phItem);
	if (D2CommonITEMSGetType(*phItem) == 4)
	{
		nGold = ptCreate->nLevel * 2 / 3;
		if (nGold <= 0)
			nGold = 1;
		dwGold = (D2CommonSeedRand(&(*phItem)->tSeed, nGold * 60) + nGold * 15) / 8;
		if (!dwGold)
			dwGold = 1;
		D2CommonStatsUnitSetValue(*phItem, 14, dwGold);
	}
	else if (D2CommonITEMSGetType(*phItem) == 5 || D2CommonITEMSGetType(*phItem) == 6)
	{
		nMinStack = D2CommonITEMSGetTypeMinStack(*phItem);
#line 1802
		D2ASSERT(nMinStack >= 0);
		nMaxStack = D2CommonITEMSGetTypeMaxStack(*phItem);
#line 1804
		D2ASSERT(nMaxStack >= 0);
		nQuantity = ITEMSRandom(&(*phItem)->tSeed, nMaxStack - nMinStack) + nMinStack;
#line 1806
		D2ASSERT(nQuantity >= 0);
		D2CommonStatsUnitSetValue(*phItem, 70, nQuantity);
	}
	else
	{
		ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats((*phItem)->dwClassId);
#line 1810
		D2ASSERT(ptItemStats);
		if (D2CommonITEMSIsArmor(*phItem))
		{
			D2CommonStatsUnitSetValue(*phItem, 20, ptItemStats->block);
			D2CommonStatsUnitSetValue(*phItem, 67, -ptItemStats->speed);
			nMaxDurability = ptItemStats->durability;
			nDurability = (DWORD)nMaxDurability / 2;
			nDurability += ITEMSRandom(&(*phItem)->tSeed, nDurability);
			if (nDurability >= 255)
				nDurability = 255;
			D2CommonStatsUnitSetValue(*phItem, 72, nDurability);
			if (nMaxDurability >= 255)
				nMaxDurability = 255;
			D2CommonStatsUnitSetValue(*phItem, 73, nMaxDurability);
			ITEMSInitArmorClass(*phItem);
		}
		else if (D2CommonITEMSIsWeapon(*phItem))
		{
			if (D2CommonITEMSGetTypeStackable(*phItem))
			{
				nMin = D2CommonITEMSGetTypeMinStack(*phItem);
				nQuantity = ITEMSRandom(&(*phItem)->tSeed, D2CommonITEMSGetTypeMaxStack(*phItem) - nMin) + nMin;
				if (dwQuality == ITEMQUAL_MAGIC || dwQuality == ITEMQUAL_RARE || dwQuality == ITEMQUAL_UNIQUE
					|| D2CommonITEMSGetType(*phItem) == 38)
					nQuantity = 1;
				D2CommonStatsUnitSetValue(*phItem, 70, nQuantity);
			}
			nMaxDurability = ptItemStats->durability;
			nDurability = (DWORD)nMaxDurability / 2;
			nDurability += ITEMSRandom(&(*phItem)->tSeed, nDurability);
			if (nDurability >= 255)
				nDurability = 255;
			D2CommonStatsUnitSetValue(*phItem, 72, nDurability);
			if (nMaxDurability >= 255)
				nMaxDurability = 255;
			D2CommonStatsUnitSetValue(*phItem, 73, nMaxDurability);
			ITEMSInitWeaponDamage(*phItem);
			D2CommonStatsUnitSetValue(*phItem, 68, -ptItemStats->speed);
		}
		else
		{
			if (D2CommonITEMSGetTypeStackable(*phItem))
			{
				nMin = D2CommonITEMSGetTypeMinStack(*phItem);
				nMax = D2CommonITEMSGetTypeMaxStack(*phItem);
				if (D2CommonITEMSGetType(*phItem) == 18)
				{
					nQuantity = D2CommonSeedRand(&(*phItem)->tSeed, 5) + 1;
					nQuantity += D2CommonGetActNum(ptCreate->nLevel);
				}
				else
					nQuantity = ITEMSRandom(&(*phItem)->tSeed, nMax - nMin) + nMin;
				if (dwQuality == ITEMQUAL_MAGIC || dwQuality == ITEMQUAL_RARE || dwQuality == ITEMQUAL_UNIQUE)
					nQuantity = 1;
				D2CommonStatsUnitSetValue(*phItem, 70, nQuantity);
			}
			if (D2CommonITEMSGetType(*phItem) == 11)
				D2CommonITEMSRollElixirKind(*phItem);
		}
	}
	hItem = *phItem;
	if (D2CommonITEMSGetType(hItem) == 10)
		D2CommonITEMSDataSetGfxIndex(hItem, D2CommonSeedRand(&hItem->tSeed, 5));
	if (D2CommonITEMSGetType(hItem) == 12)
		D2CommonITEMSDataSetGfxIndex(hItem, D2CommonSeedRand(&hItem->tSeed, 3));
	return ITEMSApplyQuality(ptGame, phItem, ptCreate);
}

typedef UNIT *(__fastcall *PFNSUNITCREATEW)(DWORD, DWORD, int, int, GAME *, GAMEROOM *, WORD, DWORD, DWORD);
#define SUnitCreateW ((PFNSUNITCREATEW)SUnitCreate)

UNIT *__fastcall ITEMSCreateFromRecord(GAME *ptGame, ITEMCREATE *ptCreate, BOOL fSeeds)
{
	UNIT *hNewItem;
	PLRINFO *ptPlayerInfo;
	CLIENT *ptClient;

	hNewItem = SUnitCreateW(UNIT_ITEM, ptCreate->dwClassId, ptCreate->nX, ptCreate->nY, ptCreate->ptGame,
		ptCreate->hRoom, ptCreate->wCreateFlags, ptCreate->dwMode, 0);
#line 1897
	D2ASSERT(hNewItem);
	if (fSeeds)
	{
		hNewItem->dwInitSeed = ptCreate->dwSeed;
		D2CommonSeedSet(&hNewItem->tSeed, ptCreate->dwSeed);
		D2CommonITEMSSetItemSeedValue(hNewItem, ptCreate->dwItemSeed);
		D2CommonSeedSet(D2CommonITEMSGetItemSeed(hNewItem), ptCreate->dwItemSeed);
	}
	if (ptCreate->nLevel <= 0)
		ptCreate->nLevel = 1;
	D2CommonITEMSSetLevelId(hNewItem, ptCreate->nLevel);
	D2CommonITEMSDataSetFlags(hNewItem, 8192, 1);
	D2CommonITEMSDataSetPage(hNewItem, 255);
	ITEMSInitStats(ptGame, &hNewItem, ptCreate);
	if (D2CommonITEMSGetType(hNewItem) == 7)
	{
#line 1920
		D2ASSERT(ptCreate->hSpawner);
#line 1921
		D2ASSERT(UnitGetType(ptCreate->hSpawner) == UNIT_PLAYER);
		ptPlayerInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(ptCreate->hSpawner);
#line 1923
		D2ASSERT(ptPlayerInfo);
		D2CommonITEMSSetNameBuffer(hNewItem, ptPlayerInfo);
		D2CommonITEMSDataSetEarLevel(hNewItem, D2CommonStatsUnitGetValue(ptCreate->hSpawner, 12));
		ptClient = SUnitGetClient(ptCreate->hSpawner);
		D2CommonITEMSDataSetFlags(hNewItem, 32768, ClientTestFlags(ptClient, 4));
	}
	sgnItemsForcedPrefix = -1;
	sgnItemsForcedSuffix = -1;
	return hNewItem;
}

UNIT *__fastcall ITEMSRecreate(GAME *ptGame, ITEMCREATE *ptCreate)
{
	UNIT *hItem;

	hItem = SUnitCreateW(UNIT_ITEM, ptCreate->dwClassId, ptCreate->nX, ptCreate->nY, ptCreate->ptGame,
		ptCreate->hRoom, ptCreate->wCreateFlags, ptCreate->dwMode, 0);
#line 1953
	D2ASSERT(hItem);
	ptCreate->dwRecreate = 1;
	hItem->dwInitSeed = ptCreate->dwSeed;
	D2CommonSeedSet(&hItem->tSeed, ptCreate->dwSeed);
	D2CommonITEMSSetItemSeedValue(hItem, ptCreate->dwItemSeed);
	D2CommonSeedSet(D2CommonITEMSGetItemSeed(hItem), ptCreate->dwItemSeed);
	D2CommonITEMSSetLevelId(hItem, ptCreate->nLevel);
	D2CommonITEMSDataSetPage(hItem, 255);
	ITEMSInitStats(ptGame, &hItem, ptCreate);
	D2CommonITEMSDataSetFlags(hItem, 16, D2CommonITEMSMaskFlags(ptCreate->dwFlags, 16));
	D2CommonITEMSDataSetFlags(hItem, 4096, D2CommonITEMSMaskFlags(ptCreate->dwFlags, 4096));
	D2CommonITEMSDataSetFlags(hItem, 2048, D2CommonITEMSMaskFlags(ptCreate->dwFlags, 2048));
	D2CommonITEMSDataSetFlags(hItem, 256, D2CommonITEMSMaskFlags(ptCreate->dwFlags, 256));
	D2CommonITEMSDataSetFlags(hItem, 131072, D2CommonITEMSMaskFlags(ptCreate->dwFlags, 131072));
	if (D2CommonITEMSGetType(hItem) == 4)
		D2CommonStatsUnitSetValue(hItem, 14, ptCreate->nQuantity);
	else
		D2CommonStatsUnitSetValue(hItem, 70, ptCreate->nQuantity);
	D2CommonStatsUnitSetValue(hItem, 72, ptCreate->nDurability);
	D2CommonStatsUnitSetValue(hItem, 73, ptCreate->nMaxDurability);
	if (D2CommonITEMSGetType(hItem) == 7)
	{
		D2CommonITEMSSetNameBuffer(hItem, ptCreate->szName);
		D2CommonITEMSDataSetEarLevel(hItem, ptCreate->bEarLevel);
		D2CommonITEMSDataSetFlags(hItem, 32768, D2CommonITEMSMaskFlags(ptCreate->dwFlags, 32768));
	}
	sgnItemsForcedPrefix = -1;
	sgnItemsForcedSuffix = -1;
	return hItem;
}

UNIT *__fastcall ITEMSDropRandom(GAME *ptGame, GAMEROOM *hRoom, COLPOINT *ptCoord,
	ITEMCREATE *ptCreateOut)
{
	ITEMCREATE tCreate;
	ITEMLEVELDATA *ptLevelData;
	COLPOINT tCoord, tSpawn;
	D2SEED *ptSeed;
	GAMEROOM *hSpawnRoom;
	UNIT *hItem;
	int eLevelId;
	int nLevel, nItemIndex;
	DWORD i;

	eLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
#line 2003
	D2ASSERT(eLevelId != LEVEL_ID_NONE);
	ptLevelData = (ITEMLEVELDATA *)D2CommonLvlTblsGetLevelData(eLevelId);
	nLevel = ptLevelData->abLevel[ptGame->bDifficultyLevel];
	if (nLevel > 1)
		nLevel--;
	if (hRoom)
		ptSeed = &ROOM7(hRoom)->tSeed;
	else
		ptSeed = NULL;
	nItemIndex = ITEMSChooseItem(ptSeed, nLevel, ITEMTYPE_ANY, FALSE, FALSE);
	tCoord = *ptCoord;
	hSpawnRoom = ITEMSFindDropSpot(hRoom, &tCoord, &tSpawn, 1);
	if (hSpawnRoom)
	{
		for (i = 0; i < sizeof(ITEMCREATE) / sizeof(DWORD); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.hSpawner = NULL;
		tCreate.ptSeed = ptSeed;
		tCreate.ptGame = ptGame;
		tCreate.dwMode = 3;
		tCreate.dwClassId = nItemIndex;
		tCreate.nX = tSpawn.nX;
		tCreate.nY = tSpawn.nY;
		tCreate.hRoom = hSpawnRoom;
		tCreate.wCreateFlags = 1;
		tCreate.dwQuality = 0;
		tCreate.nLevel = nLevel;
		hItem = ITEMSCreateFromRecord(ptGame, &tCreate, FALSE);
		if (ptCreateOut)
			*ptCreateOut = tCreate;
		return hItem;
	}
	return NULL;
}

int __fastcall ITEMSChooseItem(D2SEED *ptSeed, int nLevelId, int nType,
	BOOL fNoRarity, BOOL fType28)
{
	int nItemIndex;
	int nGoldChance, nArmorChance, nWeaponChance, nWeaponLimit;
	int &snGoldIndex = sgnGoldIndexB;

	if (!(sgbGoldIndexGuardB & 1))
	{
		sgbGoldIndexGuardB |= 1;
		snGoldIndex = D2CommonItemTblsFindItemIndexByCode(ITEMCODE_GOLD);
	}
	nItemIndex = snGoldIndex;
#line 332
	D2ASSERT(snGoldIndex != -1);
#line 335
	D2ASSERT(nItemIndex != 0);
#line 336
	D2ASSERT(nLevelId <= BASE_GOLD_RAND);
	nGoldChance = BASE_GOLD_RAND - nLevelId;
	nWeaponChance = (nLevelId >> 1) + 10;
	if (nLevelId % 2)
		nWeaponChance++;
	nArmorChance = (nLevelId >> 1) + nGoldChance + 5;
	nWeaponLimit = nArmorChance + nWeaponChance;
	nWeaponChance = D2CommonSeedRand(ptSeed, 100);
	if (nWeaponChance < nGoldChance)
		nItemIndex = snGoldIndex;
	else if (nWeaponChance < nArmorChance)
		nItemIndex = ITEMSChooseArmor(ptSeed, nLevelId, nType, fNoRarity);
	else if (nWeaponChance < nWeaponLimit)
		nItemIndex = ITEMSChooseWeapon(ptSeed, nLevelId, nType, fNoRarity);
	else if (nWeaponChance < 100)
		nItemIndex = ITEMSChooseOtherItem(ptSeed, nLevelId, nType, fNoRarity, fType28);
	else
	{
#line 394
		D2ASSERT(0);
		nItemIndex = 0;
	}
	return nItemIndex;
}

UNIT *__fastcall ITEMSDropGold(GAME *ptGame, GAMEROOM *hRoom, COLPOINT *ptCoord)
{
	ITEMCREATE tCreate;
	ITEMLEVELDATA *ptLevelData;
	COLPOINT tCoord, tSpawn;
	D2SEED *ptSeed;
	GAMEROOM *hSpawnRoom;
	UNIT *hItem;
	int eLevelId;
	int nLevel;
	DWORD i;

	eLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
#line 2044
	D2ASSERT(eLevelId != LEVEL_ID_NONE);
	ptLevelData = (ITEMLEVELDATA *)D2CommonLvlTblsGetLevelData(eLevelId);
	nLevel = ptLevelData->abLevel[ptGame->bDifficultyLevel];
	if (nLevel > 1)
		nLevel--;
	if (hRoom)
		ptSeed = &ROOM7(hRoom)->tSeed;
	else
		ptSeed = NULL;
	if (!sgnItemsGoldIndex)
		D2CommonItemTblsFindItemByCode(ITEMCODE_GOLD, &sgnItemsGoldIndex);
	tCoord = *ptCoord;
	hSpawnRoom = ITEMSFindDropSpot(hRoom, &tCoord, &tSpawn, 1);
	if (hSpawnRoom)
	{
		for (i = 0; i < sizeof(ITEMCREATE) / sizeof(DWORD); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.hSpawner = NULL;
		tCreate.ptSeed = ptSeed;
		tCreate.ptGame = ptGame;
		tCreate.dwClassId = sgnItemsGoldIndex;
		tCreate.dwMode = 3;
		tCreate.nX = tSpawn.nX;
		tCreate.nY = tSpawn.nY;
		tCreate.hRoom = hSpawnRoom;
		tCreate.wCreateFlags = 1;
		tCreate.dwQuality = 0;
		tCreate.nLevel = nLevel;
		hItem = ITEMSCreateFromRecord(ptGame, &tCreate, FALSE);
		if (hItem)
			D2CommonITEMSDataSetFlags(hItem, 8192, 0);
		return hItem;
	}
	return NULL;
}

UNIT *__fastcall ITEMSDropArmor(GAME *ptGame, GAMEROOM *hRoom, COLPOINT *ptCoord,
	int nType, BOOL fNoRarity, UNIT *hSpawner)
{
	ITEMCREATE tCreate;
	ITEMLEVELDATA *ptLevelData;
	COLPOINT tCoord, tSpawn;
	D2SEED *ptSeed;
	GAMEROOM *hSpawnRoom;
	int eLevelId;
	int nLevel, nItemIndex;
	DWORD i;

	eLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
#line 2092
	D2ASSERT(eLevelId != LEVEL_ID_NONE);
	ptLevelData = (ITEMLEVELDATA *)D2CommonLvlTblsGetLevelData(eLevelId);
	nLevel = ptLevelData->abLevel[ptGame->bDifficultyLevel];
	if (nLevel > 1)
		nLevel--;
	if (hRoom)
		ptSeed = &ROOM7(hRoom)->tSeed;
	else
		ptSeed = NULL;
	nItemIndex = ITEMSChooseArmor(ptSeed, nLevel, nType, fNoRarity);
	tCoord = *ptCoord;
	hSpawnRoom = ITEMSFindDropSpot(hRoom, &tCoord, &tSpawn, 1);
	if (hSpawnRoom)
	{
		for (i = 0; i < sizeof(ITEMCREATE) / sizeof(DWORD); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.hSpawner = hSpawner;
		tCreate.ptGame = ptGame;
		tCreate.ptSeed = ptSeed;
		tCreate.dwClassId = nItemIndex;
		tCreate.nX = tSpawn.nX;
		tCreate.dwMode = 3;
		tCreate.nY = tSpawn.nY;
		tCreate.hRoom = hSpawnRoom;
		tCreate.wCreateFlags = 1;
		tCreate.dwQuality = 0;
		tCreate.nLevel = nLevel;
		tCreate.dwHiQualityDefault = 1;
		return ITEMSCreateFromRecord(ptGame, &tCreate, FALSE);
	}
	return NULL;
}

UNIT *__fastcall ITEMSDropWeapon(GAME *ptGame, GAMEROOM *hRoom, COLPOINT *ptCoord,
	int nType, BOOL fNoRarity, UNIT *hSpawner)
{
	ITEMCREATE tCreate;
	ITEMLEVELDATA *ptLevelData;
	COLPOINT tCoord, tSpawn;
	D2SEED *ptSeed;
	GAMEROOM *hSpawnRoom;
	int eLevelId;
	int nLevel, nItemIndex;
	DWORD i;

	eLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
#line 2131
	D2ASSERT(eLevelId != LEVEL_ID_NONE);
	ptLevelData = (ITEMLEVELDATA *)D2CommonLvlTblsGetLevelData(eLevelId);
	nLevel = ptLevelData->abLevel[ptGame->bDifficultyLevel];
	if (nLevel > 1)
		nLevel--;
	if (hRoom)
		ptSeed = &ROOM7(hRoom)->tSeed;
	else
		ptSeed = NULL;
	do
		nItemIndex = ITEMSChooseWeapon(ptSeed, nLevel, nType, fNoRarity);
	while (!D2CommonITEMSTestTypeBitfieldSecondBit(nItemIndex) || nItemIndex == -1);
	tCoord = *ptCoord;
	hSpawnRoom = ITEMSFindDropSpot(hRoom, &tCoord, &tSpawn, 1);
	if (hSpawnRoom)
	{
		for (i = 0; i < sizeof(ITEMCREATE) / sizeof(DWORD); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.hSpawner = hSpawner;
		tCreate.ptSeed = ptSeed;
		tCreate.nX = tSpawn.nX;
		tCreate.ptGame = ptGame;
		tCreate.dwClassId = nItemIndex;
		tCreate.dwMode = 3;
		tCreate.nY = tSpawn.nY;
		tCreate.hRoom = hSpawnRoom;
		tCreate.wCreateFlags = 1;
		tCreate.dwQuality = 0;
		tCreate.nLevel = nLevel;
		tCreate.dwHiQualityDefault = 1;
		return ITEMSCreateFromRecord(ptGame, &tCreate, FALSE);
	}
	return NULL;
}

UNIT *__fastcall ITEMSDropOtherItem(GAME *ptGame, GAMEROOM *hRoom, COLPOINT *ptCoord,
	int nType, BOOL fNoRarity, UNIT *hSpawner)
{
	ITEMCREATE tCreate;
	ITEMLEVELDATA *ptLevelData;
	COLPOINT tCoord, tSpawn;
	D2SEED *ptSeed;
	GAMEROOM *hSpawnRoom;
	int eLevelId;
	int nLevel, nItemIndex;
	BOOL fMonster;
	DWORD i;

	eLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
#line 2178
	D2ASSERT(eLevelId != LEVEL_ID_NONE);
	ptLevelData = (ITEMLEVELDATA *)D2CommonLvlTblsGetLevelData(eLevelId);
	if (D2CommonDungeonRoomIsTown(hRoom))
		nLevel = 1;
	else
	{
		nLevel = ptLevelData->abLevel[ptGame->bDifficultyLevel];
		if (nLevel > 1)
			nLevel--;
	}
	fMonster = FALSE;
	if (hRoom)
		ptSeed = &ROOM7(hRoom)->tSeed;
	else
		ptSeed = NULL;
	if (hSpawner && UnitGetType(hSpawner) == UNIT_MONSTER)
		fMonster = TRUE;
	nItemIndex = ITEMSChooseOtherItem(ptSeed, nLevel, nType, fNoRarity, fMonster);
	tCoord = *ptCoord;
	hSpawnRoom = ITEMSFindDropSpot(hRoom, &tCoord, &tSpawn, 1);
	if (hSpawnRoom)
	{
		for (i = 0; i < sizeof(ITEMCREATE) / sizeof(DWORD); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.hSpawner = hSpawner;
		tCreate.ptSeed = ptSeed;
		tCreate.ptGame = ptGame;
		tCreate.dwClassId = nItemIndex;
		tCreate.nX = tSpawn.nX;
		tCreate.dwMode = 3;
		tCreate.nY = tSpawn.nY;
		tCreate.hRoom = hSpawnRoom;
		tCreate.wCreateFlags = 1;
		tCreate.dwQuality = 0;
		tCreate.nLevel = nLevel;
		return ITEMSCreateFromRecord(ptGame, &tCreate, FALSE);
	}
	return NULL;
}

UNIT *__fastcall ITEMSDropFromUnit(GAME *ptGame, UNIT *hUnit, DWORD dwQuality,
	int *pnLevel, ITEMCREATE *ptCreateOut, int nType,
	BOOL fNoRarity)
{
	ITEMCREATE tCreate;
	ITEMSTXT *ptItemStats;
	COLPOINT tPos, tSpawn;
	GAMEROOM *hRoom, *hSpawnRoom;
	UNIT *hItem;
	BOOL fMonster;
	int nItemIndex;
	int nTries;
	D2SEED *ptSeed;
	DWORD i;

#line 2225
	D2ASSERT(ptGame);
	ptSeed = &hUnit->tSeed;
	fMonster = FALSE;
	if (hUnit && UnitGetType(hUnit) == UNIT_MONSTER)
		fMonster = TRUE;
	*pnLevel = ITEMSGetUnitLevel(hUnit, LEVEL_ID_NONE);
	if (!UNIT7(hUnit)->dwDropItemCode)
	{
		nItemIndex = ITEMSChooseItem(ptSeed, *pnLevel, nType, fNoRarity, fMonster);
		if (dwQuality == ITEMQUAL_MAGIC)
		{
			nTries = 0;
			ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(nItemIndex);
			while (!ptItemStats || !(ptItemStats->bitfield1 & 1))
			{
				if (nTries > 10)
					nItemIndex = ITEMSChooseWeapon(&hUnit->tSeed, *pnLevel, nType, fNoRarity);
				else
					nItemIndex = ITEMSChooseItem(&hUnit->tSeed, *pnLevel, nType, fNoRarity, fMonster);
				ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(nItemIndex);
				nTries++;
			}
		}
	}
	else
	{
		nItemIndex = D2CommonItemTblsFindItemIndexByCode(UNIT7(hUnit)->dwDropItemCode);
#line 2256
		D2ASSERT(nItemIndex != -1);
	}
	D2CommonUnitGetPosition(hUnit, &tPos);
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	hSpawnRoom = ITEMSFindDropSpot(hRoom, &tPos, &tSpawn, 1);
	if (hSpawnRoom)
	{
		for (i = 0; i < sizeof(ITEMCREATE) / sizeof(DWORD); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.ptSeed = NULL;
		tCreate.nX = tSpawn.nX;
		tCreate.hSpawner = hUnit;
		tCreate.ptGame = ptGame;
		tCreate.dwClassId = nItemIndex;
		tCreate.nY = tSpawn.nY;
		tCreate.hRoom = hSpawnRoom;
		tCreate.dwMode = 3;
		tCreate.dwQuality = dwQuality;
		tCreate.nLevel = *pnLevel;
		tCreate.wCreateFlags = 1;
		hItem = ITEMSCreateFromRecord(ptGame, &tCreate, FALSE);
		if (ptCreateOut)
			*ptCreateOut = tCreate;
		return hItem;
	}
	return NULL;
}

UNIT *__fastcall ITEMSCreate(UNIT *hSpawner, DWORD dwClassId, GAME *ptGame, DWORD dwMode,
	DWORD dwQuality, DWORD dwNoSockets, int nLevel,
	BOOL fSeeds,
	DWORD dwSeed, DWORD dwItemSeed)
{
	ITEMCREATE tCreate;
	UNIT *hItem;
	DWORD i;
	if (nLevel <= 0)
		nLevel = 1;
	for (i = 0; i < sizeof(ITEMCREATE) / sizeof(DWORD); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwClassId = dwClassId;
	tCreate.dwMode = dwMode;
	tCreate.dwQuality = dwQuality;
	tCreate.hSpawner = hSpawner;
	tCreate.ptSeed = NULL;
	tCreate.ptGame = ptGame;
	tCreate.nX = 0;
	tCreate.nY = 0;
	tCreate.hRoom = NULL;
	tCreate.wCreateFlags = 1;
	tCreate.dwNoSockets = dwNoSockets;
	tCreate.dwSeed = dwSeed;
	tCreate.dwItemSeed = dwItemSeed;
	if (nLevel == -1)
		tCreate.nLevel = ITEMSGetUnitLevel(hSpawner, LEVEL_ID_NONE);
	else
		tCreate.nLevel = nLevel;
	if (tCreate.nLevel <= 0)
		tCreate.nLevel = 1;
	hItem = ITEMSCreateFromRecord(ptGame, &tCreate, fSeeds);
	D2CommonITEMSDataSetFlags(hItem, 16, 1);
	return hItem;
}

void __fastcall ITEMSUnitFree(GAME *ptGame, UNIT *hUnit)
{
	if (D2CommonITEMSGetTypeHasInv(hUnit))
	{
		SUnitFreeInventory(ptGame, hUnit);
		D2CommonINVENTORYDestroy(hUnit->ptInventory);
		hUnit->ptInventory = NULL;
	}
	ITEMSRemoveUnique(ptGame, hUnit);
	D2CommonStatsUnitFreeStats(hUnit);
	EventUnitFree(ptGame, hUnit);
}

void __fastcall GameFreeMissileUnits(GAME *ptGame)
{
	UNIT *hUnit;
	int i;

	for (i = 0; i < 128; i++)
	{
		hUnit = ptGame->pUnitList[UNIT_MISSILE][i];
		while (hUnit)
		{
			if (hUnit->dwMode != 6)
			{
				SUnitFree(ptGame, hUnit);
				hUnit = ptGame->pUnitList[UNIT_MISSILE][i];
			}
			else
				hUnit = SUnitGetNextInList(hUnit);
		}
	}
}

void __fastcall ITEMSFree(GAME *ptGame, UNIT *hItem)
{
	SUnitFree(ptGame, hItem);
}

BOOL __fastcall ITEMSIsInInventory(UNIT *hUnit, UNIT *hItem, void *hOtherInventory)
{
	void *hInventory;
	void *pNode;

	hInventory = hUnit->ptInventory;
#line 2396
	D2ASSERT(hInventory);
	if (hOtherInventory)
		hInventory = hOtherInventory;
	if (hItem == (UNIT *)D2CommonINVENTORYGetCursorItem(hInventory))
		return TRUE;
	for (pNode = D2CommonINVENTORYGetCategoryListHead(hInventory); pNode;
		pNode = (void *)D2CommonINVENTORYNodeGetNext(pNode))
	{
		if (hItem == (UNIT *)D2CommonINVENTORYNodeGetKeyA(pNode))
			return TRUE;
	}
	return FALSE;
}

UNIT *__fastcall ITEMSFindByType(UNIT *hUnit, DWORD dwType)
{
	void *hInventory, *pNode;
	UNIT *hItem;

	hInventory = hUnit->ptInventory;
	if (!hInventory)
		return NULL;
	hItem = (UNIT *)D2CommonINVENTORYGetCursorItem(hInventory);
	if (hItem && dwType == D2CommonITEMSGetTypeCode(hItem))
		return hItem;
	for (pNode = D2CommonINVENTORYGetCategoryListHead(hInventory); pNode;
		pNode = (void *)D2CommonINVENTORYNodeGetNext(pNode))
	{
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(pNode);
		if (dwType == D2CommonITEMSGetTypeCode(hItem) && (BYTE)D2CommonITEMSDataGetPageRaw((DWORD)hItem) != 1)
			return hItem;
	}
	return NULL;
}

void __fastcall ITEMSSetForcedPrefix(int nPrefix)
{
	MAGICITEMMODSDATATBLDESC *ptDesc;
	int nBase;

	if (nPrefix == -1)
		return;
	ptDesc = (MAGICITEMMODSDATATBLDESC *)D2CommonItemTblsGetMagicAffixDesc();
	nBase = ptDesc->ptPrefixes - ptDesc->ptItemMods;
	if ((int)((DWORD)nBase + (DWORD)nPrefix) >= nBase
		&& nPrefix <= &ptDesc->ptItemMods[ptDesc->nSize] - ptDesc->ptPrefixes + nBase)
	{
		sgnItemsForcedPrefix = nPrefix;
		return;
	}
#line 2577
	D2ASSERT(0);
}

void __fastcall ITEMSSetForcedSuffix(int nSuffix)
{
	MAGICITEMMODSDATATBLDESC *ptDesc;
	int nBase, nNum;

	if (nSuffix == -1)
		return;
	ptDesc = (MAGICITEMMODSDATATBLDESC *)D2CommonItemTblsGetMagicAffixDesc();
	nBase = ptDesc->ptSuffixes - ptDesc->ptItemMods;
	nNum = ptDesc->ptPrefixes - ptDesc->ptSuffixes;
	if ((int)((DWORD)nBase + (DWORD)nSuffix) >= nBase
		&& nSuffix <= nNum + nBase)
		sgnItemsForcedSuffix = nSuffix;
	else
	{
#line 2597
		D2ASSERT(0);
	}
}

int __fastcall ITEMSGetUnitLevel(UNIT *hUnit, int eLevelId)
{
	ITEMLEVELDATA *ptLvlData;
	GAME *ptGame;
	int nLevel;

	if (UnitGetType(hUnit) == UNIT_MONSTER)
		return D2CommonStatsUnitGetValue(hUnit, 12);
	if (UnitGetType(hUnit) == UNIT_PLAYER)
	{
		nLevel = D2CommonStatsUnitGetBaseValue(hUnit, 12);
		if (nLevel > 0)
			return nLevel;
		return 1;
	}
	if (hUnit)
		eLevelId = D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hUnit));
	else
	{
#line 2675
		D2ASSERT(eLevelId != LEVEL_ID_NONE);
	}
	ptLvlData = (ITEMLEVELDATA *)D2CommonLvlTblsGetLevelData(eLevelId);
	ptGame = SUnitGetGame(hUnit);
#line 2680
	D2ASSERT(ptLvlData);
	nLevel = ptLvlData->abLevel[ptGame->bDifficultyLevel];
	if (nLevel > 0)
		return nLevel;
	return 1;
}

void __fastcall ITEMSFreeUniqueLists(GAME *ptGame)
{
	ITEMUNIQUENODE **pptHead, *ptNode, *ptNext;
	int i;

#line 2694
	D2ASSERT(ptGame);
	for (i = 0, pptHead = ptGame->aptUniqueItems; i < MAX_UNIQUE_ITEMS; i++, pptHead++)
	{
		for (ptNode = *pptHead; ptNode; ptNode = ptNext)
		{
			ptNext = ptNode->ptNext;
#line 2700
			D2FREEPOOL(ptGame->pMemPool, ptNode);
		}
		*pptHead = NULL;
	}
}

void __fastcall ITEMSWear(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	BOOL fArmor, fWeapon;
	DWORD dwChance;
	int nDurability;

	fArmor = D2CommonITEMSIsArmor(hItem);
	fWeapon = D2CommonITEMSIsWeapon(hItem);
	if (!fArmor && !fWeapon)
		return;
	if (!D2CommonITEMSTestTypeHasDurability(hItem))
		return;
	dwChance = 5;
	if (fArmor)
		dwChance = 10;
	if (D2CommonSeedRand(&hUnit->tSeed, 100) >= dwChance)
		return;
	nDurability = (DWORD)D2CommonStatsUnitGetValue(hItem, 72) - 1;
	if (nDurability > 0)
	{
		D2CommonStatsUnitSetValue(hItem, 72, nDurability);
		SCmdSendItemStat(SUnitGetClient(hUnit), hItem, 1, 72, nDurability);
		return;
	}
	if (!fWeapon && !D2CommonITEMSDataTestFlags(hItem, 256))
	{
		ITEMSModeBreakItem(ptGame, hUnit, hItem);
		return;
	}
	D2CommonStatsUnitSetValue(hItem, 72, 0);
	SCmdSendItemStat(SUnitGetClient(hUnit), hItem, 1, 72, 0);
}

void __fastcall ITEMSFillCreate(GAME *ptGame, ITEMCREATE *ptCreate, UNIT *hItem,
	DWORD dwRecreate)
{
	int nQuantity;
	DWORD i;

	for (i = 0; i < sizeof(ITEMCREATE) / sizeof(DWORD); i++)
		((DWORD *)ptCreate)[i] = 0;
	ptCreate->ptGame = ptGame;
	ptCreate->nLevel = D2CommonITEMSGetLevelId(hItem);
	if (ptCreate->nLevel <= 0)
		ptCreate->nLevel = 1;
	ptCreate->dwClassId = hItem->dwClassId;
	ptCreate->dwMode = hItem->dwMode;
	ptCreate->dwQuality = D2CommonITEMSDataGetModType(hItem);
	ptCreate->nX = D2CommonUnitGetX(hItem);
	ptCreate->nY = D2CommonUnitGetY(hItem);
	if (D2CommonITEMSGetType(hItem) == 4)
		ptCreate->nQuantity = D2CommonStatsUnitGetValue(hItem, 14);
	else if (D2CommonITEMSGetTypeStackable(hItem))
	{
		nQuantity = D2CommonStatsUnitGetValue(hItem, 70);
#line 2761
		D2ASSERT(nQuantity >= 0);
		ptCreate->nQuantity = nQuantity;
	}
	ptCreate->wCreateFlags = 1;
	ptCreate->ptSeed = &hItem->tSeed;
	ptCreate->dwSeed = hItem->dwInitSeed;
	ptCreate->dwItemSeed = D2CommonITEMSGetItemSeedValue(hItem);
	ptCreate->dwFileIndex = D2CommonITEMSDataGetUniqueOrSetRow(hItem);
	ptCreate->dwFlags = D2CommonITEMSDataGetFlags(hItem);
	ptCreate->nDurability = D2CommonStatsUnitGetValue(hItem, 72);
	ptCreate->nMaxDurability = D2CommonStatsUnitGetBaseValue(hItem, 73);
	ptCreate->dwFileIndex = D2CommonITEMSDataGetUniqueOrSetRow(hItem);
	ptCreate->dwRecreate = dwRecreate;
	if (D2CommonITEMSGetType(hItem) == 7)
	{
		strcpy(ptCreate->szName, D2CommonITEMSGetNameBuffer(hItem));
		ptCreate->bEarLevel = D2CommonITEMSDataGetEarLevel(hItem);
	}
}

void __fastcall ITEMSDropGoldPiles(GAME *ptGame, UNIT *hUnit, DWORD dwGold)
{
	COLPOINT tPos, tSpawn;
	GAMEROOM *hRoom;
	UNIT *hGold;
	DWORD dwAll, dwTotal, dwPile;
	int nGoldIndex;

	if (!D2CommonItemTblsFindItemByCode(ITEMCODE_GOLD, &nGoldIndex))
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	dwTotal = 0;
	dwAll = dwGold;
	while (dwTotal < dwAll)
	{
		if (dwGold > 2000000000)
			dwPile = 2000000000;
		else
			dwPile = dwGold;
		dwTotal += dwPile;
		dwGold -= dwPile;
		D2CommonUnitGetPosition(hUnit, &tPos);
		hRoom = ITEMSFindDropSpot(hRoom, &tPos, &tSpawn, 1);
		if (hRoom)
		{
			hGold = ITEMSCreate(hUnit, nGoldIndex, ptGame, 4, ITEMQUAL_NORMAL, 1, 1, FALSE, 0,
				0);
			if (hGold)
			{
				D2CommonStatsUnitSetValue(hGold, 14, dwPile);
				D2CommonUnitSetMode(hGold, 3);
				D2CommonUnitInitPosition(0, hGold, hRoom, tSpawn.nX, tSpawn.nY);
				D2CommonUnitSetCollision(hGold, hRoom, tSpawn.nX, tSpawn.nY);
				D2CommonAddUnitToRoom(hGold, hRoom);
				hGold->dwFlags0 |= 4098;
				D2CommonUnitSetMode(hGold, 3);
				hGold->dwFlags0 |= 33554432;
			}
		}
	}
}

void __fastcall ITEMSAddGold(GAME *ptGame, UNIT *hUnit, DWORD dwAmount)
{
	DWORD dwMax, dwGold;

	dwMax = D2CommonUnitCalcGoldLimit(hUnit);
	dwGold = D2CommonStatsUnitGetValue(hUnit, 14);
	if (dwGold == dwMax)
	{
		ITEMSDropGoldPiles(ptGame, hUnit, dwAmount);
		return;
	}
	if (dwGold + dwAmount > dwMax)
	{
		D2CommonStatsUnitAddValue(hUnit, 14, dwMax - dwGold);
		ITEMSDropGoldPiles(ptGame, hUnit, dwGold - dwMax + dwAmount);
		return;
	}
	D2CommonStatsUnitAddValue(hUnit, 14, dwAmount);
}

void __fastcall ITEMSDropEar(GAME *ptGame, UNIT *hUnit)
{
	COLPOINT tPos, tSpawn;
	GAMEROOM *hRoom;
	UNIT *hEar;
	void *ptItem;
	int nEarIndex;

	ptItem = (void *)D2CommonItemTblsFindItemByCode(ITEMCODE_EAR, &nEarIndex);
#line 2866
	D2ASSERT(ptItem);
	D2CommonUnitGetPosition(hUnit, &tPos);
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	hRoom = ITEMSFindDropSpot(hRoom, &tPos, &tSpawn, 1);
	if (!hRoom)
		return;
	hEar = ITEMSCreate(hUnit, nEarIndex, ptGame, 4, ITEMQUAL_NORMAL, 1, 1, FALSE, 0, 0);
	if (!hEar)
		return;
	D2CommonUnitSetMode(hEar, 3);
	D2CommonUnitInitPosition(0, hEar, hRoom, tSpawn.nX, tSpawn.nY);
	D2CommonUnitSetCollision(hEar, hRoom, tSpawn.nX, tSpawn.nY);
	D2CommonAddUnitToRoom(hEar, hRoom);
	hEar->dwFlags0 |= 33558530;
}

DWORD __fastcall ITEMSGetHealingPotion(UNIT *hUnit)
{
	GAMEROOM *hRoom;
	GAME *ptGame;
	BYTE bAct;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
#line 2901
	D2ASSERT(hRoom);
	bAct = D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom));
	ptGame = SUnitGetGame(hUnit);
	if (ptGame && ptGame->bDifficultyLevel > 0)
		bAct += 2;
	if (!(bAct < sgnItemsHealingPotions)) {
		return sgadwItemsHealingPotions[arrsize(sgadwItemsHealingPotions) - 1]; }
	return sgadwItemsHealingPotions[bAct];
}

DWORD __fastcall ITEMSGetManaPotion(UNIT *hUnit)
{
	GAMEROOM *hRoom;
	GAME *ptGame;
	BYTE bAct;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
#line 2932
	D2ASSERT(hRoom);
	bAct = D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom));
	ptGame = SUnitGetGame(hUnit);
	if (ptGame && ptGame->bDifficultyLevel > 0)
		bAct += 2;
	if (!(bAct < sgnItemsManaPotions)) {
		return sgadwItemsManaPotions[arrsize(sgadwItemsManaPotions) - 1]; }
	return sgadwItemsManaPotions[bAct];
}

UNIT *__fastcall ITEMSDuplicate(GAME *ptGame, UNIT *hItem, UNIT *hOwner)
{
	void *pInventory;
	ITEMCREATE tCreate;
	UNIT *hNewItem;
	UNIT *hNewItemItem;
	UNIT *hInItem;
	void *pNode;
	int nResult;

	ITEMSFillCreate(ptGame, &tCreate, hItem, 1);
	hNewItem = ITEMSRecreate(ptGame, &tCreate);
#line 2958
	D2ASSERT(hNewItem);
	D2CommonITEMSDataSetPage(hNewItem, D2CommonITEMSDataGetPageRaw((DWORD)hItem));
	if (ITEMSModeHasContents(hItem))
	{
		pInventory = hItem->ptInventory;
		for (pNode = D2CommonINVENTORYGetCategoryListHead(pInventory); pNode;
			pNode = (void *)D2CommonINVENTORYNodeGetNext(pNode))
		{
			hInItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(pNode);
			ITEMSFillCreate(ptGame, &tCreate, hInItem, 1);
			hNewItemItem = ITEMSRecreate(ptGame, &tCreate);
#line 2972
			D2ASSERT(hNewItemItem);
			D2CommonITEMSDataSetPage(hNewItemItem, D2CommonITEMSDataGetPageRaw((DWORD)hInItem));
			INVENTORYModeClearItemFlags(ptGame, hNewItemItem);
			D2CommonITEMSDataSetCmdFlags(hNewItemItem, 1, 0);
			D2CommonUnitSetMode(hNewItemItem, 4);
			if (!ITEMSModeSocketGem(ptGame, hOwner, hNewItemItem->dwId, hNewItem->dwId, &nResult,
				FALSE, TRUE, FALSE))
			{
#line 2984
				D2ASSERT(0);
			}
		}
	}
	INVENTORYModeClearItemFlags(ptGame, hNewItem);
	D2CommonITEMSDataSetCmdFlags(hNewItem, 1, 0);
	return hNewItem;
}

void __fastcall ITEMSFreeAll(GAME *ptGame)
{
	UNIT *hUnit, *hNext;
	int i;

	for (i = 0; i < 128; i++)
	{
		for (hNext = ptGame->pUnitList[UNIT_ITEM][i]; hNext; )
		{
			hUnit = hNext;
			hNext = SUnitGetNextInList(hUnit);
			SUnitFree(ptGame, hUnit);
		}
	}
}
