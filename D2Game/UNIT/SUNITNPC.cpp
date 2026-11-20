#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\UNIT\\SUNITNPC.CPP"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "../GAME/SCmd.h"
#include "../ITEMS/ItemMode.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/MonsterAI.h"
#include "../MONSTER/MonsterMsg.h"
#include "../MONSTER/MonsterUnique.h"
#include "../PLAYER/PlayerPets.h"
#include "../PLAYER/PlrMsg.h"
#include "../QUESTS/QUESTS.h"
#include "../QUESTS/a1q3.h"
#include "SUnit.h"
#include "sunitproxy.h"
#include "SUNITNPC.h"
#include "../SKILLS/Skills.h"
#include "FogImports.h"

#define UNIT_MONSTER			1
#define UNIT_ITEM				4

#define STAT_LIFE				6
#define STAT_MANA				8
#define STAT_STAMINA			10
#define STAT_LEVEL				12
#define STAT_GOLD				14
#define STAT_GOLDBANK			15
#define STAT_QUANTITY			70
#define STAT_DURABILITY			72

#define NPC_STATES				138
#define NPC_MAXFAILED			32
#define NPC_GAMBLEITEMS			12
#define NPC_GAMBLETRIES			100

#define SCMD_NPCRESULT			35
#define SCMD_UNITEVENT			37
#define SCMD_HIRE				71
#define SCMD_HIRELIST			72
#define SCMD_HIREGIVEN			73
#define SCMD_NPCREWARD			81

#define NPCRESULT_BOUGHT		0
#define NPCRESULT_SOLD			1
#define NPCRESULT_REPAIRED		2
#define NPCRESULT_REPAIREDALL	3
#define NPCRESULT_HIRED			5
#define NPCRESULT_HEALED		6
#define NPCRESULT_NOITEM		7
#define NPCRESULT_REFUSED		9
#define NPCRESULT_NOROOM		10
#define NPCRESULT_NOSALE		11
#define NPCRESULT_NOGOLD		12
#define NPCRESULT_NOHEAL		14
#define NPCRESULT_NOHIRE		15

#define NPCCLASS_HIRE1			150
#define NPCCLASS_HIRE2			198
#define NPCCLASS_HIRE3			252
#define HIRECLASS_1				271
#define HIRECLASS_2				338
#define HIRECLASS_3				359
#define HIREID_1_FIRST			3411
#define HIREID_1_LAST			3451
#define HIREID_2_FIRST			1019
#define HIREID_2_LAST			1039
#define HIREID_3_FIRST			1040
#define HIREID_3_LAST			1059
#define HIRE_OFFERS				10
#define HIRE_OFFERROWS			20

#define ITEMTYPE_7				7
#define ITEMTYPE_12				18
#define ITEMTYPE_16				22

#pragma pack(push, 1)
struct NPCEVENTPACKET
{
 	BYTE		bCmd;
 	BYTE		bType;
 	DWORD		dwUnitId;
 	WORD		wEvent;
};
struct NPCHIREPACKET
{
 	BYTE		bCmd;
 	WORD		wId;
 	DWORD		dwSeed;
};
struct NPCHIREGIVENPACKET
{
 	BYTE		bCmd;
 	WORD		wKind;
 	WORD		wId;
 	BYTE		unobserved_05[10];
};
struct NPCREWARDPACKET
{
 	BYTE		bCmd;
 	DWORD		dwNpcId;
 	BYTE		bResult;
 	BYTE		unobserved_06;
};
#pragma pack(pop)
typedef struct NPCEVENTPACKET NPCEVENTPACKET;
typedef struct NPCHIREPACKET NPCHIREPACKET;
typedef struct NPCHIREGIVENPACKET NPCHIREGIVENPACKET;
typedef struct NPCREWARDPACKET NPCREWARDPACKET;

struct NPCGAMBLELIST
{
 	int			nCount;
 	DWORD		*pdwRows;
};
typedef struct NPCGAMBLELIST NPCGAMBLELIST;

static DWORD sgdwItemRin;
DWORD gdwNpcHealerId;

#define NPCSENDRESULT(hPlayer, bResult, dwId, bField01) \
	SCmdSendNpcResult(SUnitGetClient(hPlayer), SCMD_NPCRESULT, (bResult), \
		(DWORD)D2CommonStatsUnitGetValue(hPlayer, STAT_GOLD), (dwId), (bField01)); \

#define NPC_ACTS				4

void __fastcall SUnitNpcSendUnitEvent(UNIT *hUnit, CLIENT *ptClient)
{
	UNIT *hOther;
	NPCEVENTPACKET tPacket;

	hOther = hUnit->field_90;
	if (hOther && ClientGetPlayerUnit(ptClient, FALSE) != hOther)
		return;
	tPacket.bCmd = SCMD_UNITEVENT;
	tPacket.dwUnitId = hUnit->dwId;
	tPacket.bType = (BYTE)hUnit->eType;
	tPacket.wEvent = hUnit->wEvent;
	SCmdSendUnitSound(ptClient, (const struct SCMDCOPY8 *)&tPacket);
}

extern void SUnitNpcTakeGambleI(UNIT *hPlayer, UNIT *hNpc, UNIT *hItem, GAME *ptGame);
__inline void SUnitNpcTakeGambleI(UNIT *hPlayer, UNIT *hNpc, UNIT *hItem, GAME *ptGame)
{
	void *hInventory;
	int nResult;

	if (!hItem || !hNpc)
		return;
	hInventory = SUnitProxyGetPlayerInventory(ptGame, hPlayer, hNpc);
	if (hInventory)
	{
		ITEMSModeTakeToCursor(ptGame, hNpc, hItem->dwId, &nResult, 0, FALSE, hInventory, FALSE);
		hItem->dwFlags |= 16;
		D2CommonUnitSetChanged(hNpc, 1);
		(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
		return;
	}
#line 303
	FogErrorManagerLog("Error at line %d of file %s", __LINE__, D2_THIS_FILE);
}

void __fastcall SUnitNpcReleaseItem(UNIT *hNPC  , UNIT *hItem
	 , GAME *ptGame  , BOOL fToNpc)
{
	int nResult;

#line 312
	D2ASSERT(ptGame);
#line 313
	D2ASSERT(hNPC);
#line 314
	D2ASSERT(hItem);
	SUnitProxyBuyItem(ptGame, hNPC, hItem);
	ITEMSModeTakeToCursor(ptGame, hNPC, hItem->dwId, &nResult, 0, FALSE, NULL, FALSE);
	if (fToNpc)
	{
		hItem->dwFlags |= 16;
		D2CommonUnitSetChanged(hNPC, 1);
		(void)D2CommonINVENTORYAddDeltaNode(hNPC->ptInventory, hItem);
		return;
	}
	ITEMSFree(ptGame, hItem);
}

static __inline void SUnitNpcSendHireI(CLIENT *ptClient, WORD wId, DWORD dwSeed)
{
	NPCHIREPACKET tPacket;

	tPacket.wId = wId;
	tPacket.dwSeed = dwSeed;
	tPacket.bCmd = SCMD_HIRE;
	SCmdSendHire(ptClient, (const struct SCMDCOPY7 *)&tPacket);
}

void __fastcall SUnitNpcSendHires(GAME *ptGame, CLIENT *ptClient, UNIT *hNpc, BOOL bFirst)
{
	NPCPROXY *pInv;
	DWORD *pdwHire;
	DWORD dwClassId;
	int nIndex;
	int i;

	(void)ptGame;
	(void)bFirst;
	if (hNpc->dwClassId != NPCCLASS_HIRE1 && hNpc->dwClassId != NPCCLASS_HIRE2
		&& hNpc->dwClassId != NPCCLASS_HIRE3)
		return;
	SCmdSendCmd(ptClient, SCMD_HIRELIST);
	pInv = SUnitProxyFindNpcOf(ClientGetGame(ptClient), hNpc, &nIndex);
	if (!pInv)
		return;
	dwClassId = hNpc->dwClassId;
	if (!pInv->ItemList.bHiresDrawn)
		SUnitNpcInitHires(ClientGetGame(ptClient), dwClassId, pInv);
	pdwHire = &pInv->ptHires->dwHired;
	for (i = NPCHIRE_COUNT; i; i--, pdwHire += 4)
	{
		if (pdwHire[1] && !pdwHire[0])
		{
			SUnitNpcSendHireI(ptClient, *(WORD *)(pdwHire - 2), pdwHire[-1]);
		}
	}
}

void __fastcall SUnitNpcInitHires(GAME *ptGame, DWORD dwClassId, NPCPROXY *pInv
	 )
{
	MONPETDESC *ptHires;
	D2SEED *ptSeed;
	int nFirst;
	int nLast;
	int nId;
	int nOffer;
	int nStart;
	int i;

	if (pInv->ItemList.bHiresDrawn)
		return;
	pInv->ItemList.bHiresDrawn = 1;
	if (pInv->ptHires)
		return;
#line 60
	ptHires = (MONPETDESC *)D2ALLOCPOOL(ptGame->pMemPool, NPCHIRE_COUNT * sizeof(MONPETDESC));
	pInv->ptHires = ptHires;
	for (i = 0; i < (int)(NPCHIRE_COUNT * sizeof(MONPETDESC) / sizeof(DWORD)); i++)
		((DWORD *)ptHires)[i] = 0;
	ptSeed = SUnitProxyGetSeed(ptGame);
	ptHires = pInv->ptHires;
	if (dwClassId == NPCCLASS_HIRE2)
	{
		nFirst = HIREID_2_FIRST;
		nLast = HIREID_2_LAST;
	}
	else if (dwClassId == NPCCLASS_HIRE3)
	{
		nFirst = HIREID_3_FIRST;
		nLast = HIREID_3_LAST;
	}
	else
	{
		nFirst = HIREID_1_FIRST;
		nLast = HIREID_1_LAST;
	}
	for (nId = nFirst; nId <= nLast; nId++)
	{
		i = nId - nFirst;
		if (i >= NPCHIRE_COUNT)
			break;
		ptHires[i].wHireId = (WORD)(i + nFirst);
		ptHires[i].dwSeed = D2CommonSeedNext(ptSeed);
		ptHires[i].dwHired = 0;
		ptHires[i].dwOffered = 0;
	}
	for (nOffer = 0; nOffer < HIRE_OFFERS; nOffer++)
	{
		if (dwClassId == NPCCLASS_HIRE3 || dwClassId == NPCCLASS_HIRE2)
			i = (int)D2CommonSeedRand(ptSeed, HIRE_OFFERROWS);
		else
			i = (int)D2CommonSeedRand(ptSeed, NPCHIRE_COUNT);
		nStart = i;
		for (;;)
		{
			if (!ptHires[i].dwOffered && !ptHires[i].dwHired)
			{
				ptHires[i].dwOffered = 1;
				break;
			}
			i++;
			if (i >= NPCHIRE_COUNT)
				i = 0;
			if (i == nStart)
				return;
		}
	}
}

static __inline int SUnitNpcStockLevelI(GAME *ptGame, UNIT *hPlayer, UNIT *hNpc)
{
	NPCPROXY *pInv;
	int nPlrLevel;
	int nLevel;
	int nIndex;

	nPlrLevel = D2CommonStatsUnitGetValue(hPlayer, STAT_LEVEL);
	pInv = SUnitProxyFindNpcOf(ptGame, hNpc, &nIndex);
	if (pInv)
	{
		int anLevelCap[NPC_ACTS] = { 12, 20, 28, 36 };

		if (pInv->ItemList.bAct < NPC_ACTS)
		{
			if (anLevelCap[pInv->ItemList.bAct] < nPlrLevel)
				nLevel = anLevelCap[pInv->ItemList.bAct];
			else
				nLevel = nPlrLevel;
		}
		else
			nLevel = nPlrLevel;
	}
	else
		nLevel = nPlrLevel;
	return nLevel;
}

void __fastcall SUnitNpcFillStock(GAME *ptGame, UNIT *hPlayer, UNIT *hNpc, NPCSTATE *pList)
{
	NPCPROXY *pNPCNode;
	NPCPROXY *pInv;
	D2SEED *ptSeed;
	int nLevel;
	int nPlrLevel;
	int nFailed;
	int nIndex;
	int i;
	DWORD dwCount;
	DWORD j;
	DWORD dwRoll;
	int nQuality;
	DWORD dwClassId;
	BOOL fMagic;

	ptSeed = SUnitProxyGetSeed(ptGame);
	pList->dwTick = GetTickCount();
	nFailed = 0;
	pNPCNode = SUnitProxyFindNpcOf(ptGame, hNpc, &nIndex);
	if (pNPCNode)
	{
		nLevel = SUnitNpcStockLevelI(ptGame, hPlayer, hNpc);
		for (i = 0; i < pNPCNode->ItemList.nItemStats; i++)
		{
			dwCount = D2CommonSeedBetween(ptSeed, pNPCNode->ItemList.pItemStats[i].Min,
				pNPCNode->ItemList.pItemStats[i].Max + 1);
			for (j = 0; j < dwCount; j++)
			{
				nQuality = 2;
				dwClassId = pNPCNode->ItemList.pItemStats[i].dwClassId;
				fMagic = FALSE;
				if (D2CommonITEMSTestTypeBitfieldFirstBit(dwClassId) == 1)
					fMagic = TRUE;
				dwRoll = D2CommonSeedRand(ptSeed, 100);
				if (nLevel < 5)
				{
					if (dwRoll > 90)
						nQuality = 1;
				}
				else if (nLevel < 10)
				{
					if (dwRoll > 85)
						nQuality = 3;
				}
				else if (dwRoll >= 75)
					nQuality = 3;
				if (!SUnitNpcCreateStockItem(hNpc, pNPCNode->ItemList.pItemStats[i].dwClassId, ptGame, TRUE,
					nQuality,
					nLevel))
					nFailed++;
				if (nFailed > NPC_MAXFAILED)
					return;
			}
			if (D2CommonITEMSTestTypeBitfieldFirstBit(pList->pItemStats[i].dwClassId) == 1
				&& pList->pItemStats[i].MagicLvl <= nLevel)
			{
				NPCITEMSTAT *ptStat = &pNPCNode->ItemList.pItemStats[i];

				dwCount = D2CommonSeedBetween(ptSeed, ptStat->MagicMin, ptStat->MagicMax + 1);
				for (; dwCount > 0; dwCount--)
				{
					if (!SUnitNpcCreateStockItem(hNpc, pNPCNode->ItemList.pItemStats[i].dwClassId, ptGame, TRUE,
						4,
						nLevel))
						nFailed++;
				}
			}
		}
		return;
	}
#line 716
	D2ASSERT(pNPCNode != NULL);
}

UNIT *__fastcall SUnitNpcCreateStockItem(UNIT *hNPC  , DWORD dwRow,
	GAME *ptGame  , BOOL fAdd, int nQuality,
	int nLevel)
{
	ITEMSTXT *ptItemsTxt;
	const char *pszName;
	UNIT *hItem;
	BOOL fOk;
	BYTE bKind;
	int nDurability;
	int i;
	int j;

#line 192
	D2ASSERT(ptGame);
#line 193
	D2ASSERT(hNPC);
	if (ptGame->bDifficultyLevel > 0)
	{
		ptItemsTxt = (ITEMSTXT *)D2CommonItemTblsGetItemStats(dwRow);
		if (ptGame->bDifficultyLevel == 1)
			dwRow += ptItemsTxt->NightmareUpgrade;
		else
			dwRow += ptItemsTxt->HellUpgrade;
	}
	hItem = NULL;
	for (i = 0; i < 2; i++)
	{
		for (j = 0; ; )
		{
			fOk = TRUE;
			hItem = ITEMSCreate(hNPC, dwRow, ptGame, 4, nQuality, 0, nLevel, FALSE, 0, 0);
			if (D2CommonITEMSDataGetModType(hItem) == 1
				&& !stricmp(pszName = (const char *)D2CommonItemTblsGetLowQualityItem(
					D2CommonITEMSDataGetUniqueOrSetRow( hItem)), "Cracked"))
			{
				ITEMSFree(ptGame, hItem);
				hItem = NULL;
				fOk = FALSE;
			}
			j++;
			if (fOk)
				break;
			if (j >= 3)
				return NULL;
		}
		if (dwRow != hItem->dwClassId)
		{
			ITEMSFree(ptGame, hItem);
			nQuality = 2;
			hItem = NULL;
		}
		else
			i = 2;
	}
	if (hItem)
	{
		bKind = SUnitProxyGetItemKind(hItem);
		if (bKind == 255)
		{
			ITEMSFree(ptGame, hItem);
			return NULL;
		}
		((void (__stdcall *)(UNIT *, BYTE))D2CommonITEMSDataSetPageRaw)(hItem, bKind);
		if (D2CommonITEMSDataTestFlags(hItem, 256))
			ITEMSModeRepairItem(ptGame, NULL, hItem);
		else
		{
			nDurability = D2CommonStatsUnitCalcMaxDurability(hItem);
			if (nDurability > 0)
				D2CommonStatsUnitSetValue(hItem, STAT_DURABILITY, nDurability);
		}

		if (!ITEMSModePlaceCursorItem(ptGame, hNPC, hItem->dwId, (int)D2CommonUnitGetX(hItem), ((int)D2CommonUnitGetY(hItem)),
			TRUE,
			TRUE, NULL))
		{
			SUnitProxyGiveItem(hNPC, ptGame, hItem, 0);
			return NULL;
		}
		SUnitProxySellItem(ptGame, hNPC, hItem, 0);
		D2CommonITEMSDataSetFlags(hItem, 16, 1);
		if (fAdd)
		{
			hItem->dwFlags |= 4;
			if (ITEMSModeHasContents(hItem))
				D2CommonITEMSDataSetFlags(hItem, 1, 1);
			D2CommonUnitSetChanged(hNPC, 1);
			(void)D2CommonINVENTORYAddDeltaNode(hNPC->ptInventory, hItem); }
	}
	return hItem;
}

void __fastcall SUnitNpcSell(GAME *ptGame  , UNIT *hPlayer, DWORD dwNpcId,
	DWORD dwItemId, WORD wItemMode, DWORD dwUnused4)
{
	UNIT *hInteract;
	UNIT *hNpc;
	UNIT *hItem;
	UNIT *hCopy;
	NPCVISIT *ptVisit;
	void *ptSkill;
	BOOL fSellable;
	DWORD dwPrice;
	int nCharges;
	int nIndex;
	int nType;
	BYTE bKind;
	DWORD bDifficulty;
	CLIENT *ptClient;
	void *ptQuest;
	const char *pszLowQuality;
	NPCPROXY *ptProxy;

	(void)dwUnused4;
#line 1276
	D2ASSERT(ptGame);
	hInteract = SUnitGetInteractUnit(ptGame, hPlayer);
	hNpc = SUnitFind(ptGame, UNIT_MONSTER, dwNpcId);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hNpc || hInteract != hNpc)
		goto refused;
	if (!ITEMSIsInInventory(hPlayer, hItem, NULL))
	{
		NPCSENDRESULT(hPlayer, NPCRESULT_NOSALE, (DWORD)-1, 0);
		return;
	}
	if (!hItem || hItem->dwMode != wItemMode || !D2CommonITEMSTestSellable(hItem))
		goto refused;
	bDifficulty = ptGame->bDifficultyLevel;
	ptQuest = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[bDifficulty];
	dwPrice = D2CommonITEMSGetPrice(hItem, bDifficulty, ptQuest, hNpc->dwClassId, 1);
	fSellable = TRUE;
	if (D2CommonITEMSDataGetModType(hItem) == 1)
	{
		pszLowQuality = (const char *)D2CommonItemTblsGetLowQualityItem(D2CommonITEMSDataGetUniqueOrSetRow(
			hItem));
		if (!stricmp(pszLowQuality, "Cracked"))
			fSellable = FALSE;
	}
	if (D2CommonITEMSDataTestFlags(hItem, 256))
		fSellable = FALSE;
	if (D2CommonITEMSGetType(hItem) == ITEMTYPE_7)
		fSellable = FALSE;
	if (ITEMSModeHasContents(hItem))
		fSellable = FALSE;
	ptProxy = SUnitProxyFindNpcOf(ptGame, hNpc, &nIndex);
	ptVisit = SUnitProxyAddVisit(ptGame, ptProxy, hPlayer);
	if (!ptVisit->bOwnStock && fSellable)
	{
		hCopy = SUnitNpcCopyItem(ptGame, hItem, hNpc);
		D2CommonUnitSetMode(hCopy, 4);
		if (hCopy)
		{
			bKind = SUnitProxyGetItemKind(hCopy);
			if (bKind == 255)
				ITEMSFree(ptGame, hCopy);
			else
			{
				D2CommonITEMSDataSetPage(hCopy, bKind);
				if (!ITEMSModePlaceCursorItem(ptGame, hNpc, hCopy->dwId,
					(int)D2CommonUnitGetX(hCopy),
					(int)D2CommonUnitGetY(hCopy), TRUE, TRUE, NULL))
					ITEMSFree(ptGame, hCopy);
				else
				{
					SUnitProxySellItem(ptGame, hNpc, hCopy, 1);
					D2CommonITEMSDataSetFlags(hCopy, 16, 1);
					hCopy->dwFlags |= 4;
					if (ITEMSModeHasContents(hCopy))
						D2CommonITEMSDataSetFlags(hCopy, 1, 1);
					D2CommonUnitSetChanged(hNpc, 1);
					(void)D2CommonINVENTORYAddDeltaNode(hNpc->ptInventory, hCopy);
				}
			}
		}
	}
	fSellable = TRUE;
	if (wItemMode == 0)
	{
		DWORD dwFlags;

		nType = (int)D2CommonITEMSGetType(hItem);
		if (nType == ITEMTYPE_16)
		{
			DWORD dwSkill;

			dwSkill = ((BOOKSTXT *)D2CommonItemTblsGetBook(D2CommonITEMSDataGetMagicSuffixAt(hItem,
				0)))->skillscrolloffset;
			ptSkill = (void *)D2CommonSkillsFindByKey(hPlayer, dwSkill);
			if (ptSkill)
			{
				nCharges = (int)D2CommonSkillsGetNodeCount(ptSkill) - 1;
				if (nCharges <= 0)
				{
					if ((void *)D2CommonUnitGetRightSkill(hPlayer) == ptSkill)
						SkillsSetCurrent(hPlayer, FALSE, SKILL_ATTACK);
					nCharges = 0;
				}
				D2CommonSkillsSetNodeCount(ptSkill, nCharges);
				SCmdSendSkillState(SUnitGetClient(hPlayer), (BYTE)hPlayer->eType, hPlayer->dwId,
					(WORD)dwSkill,
					(BYTE)nCharges);
			}
		}
		else if (nType == ITEMTYPE_12)
		{
			int Count;
			DWORD dwSkill;

			Count = D2CommonStatsUnitGetValue(hItem, STAT_QUANTITY);
#line 1411
			D2ASSERT(Count >= 0);
			if (Count)
			{
				dwSkill = ((BOOKSTXT *)D2CommonItemTblsGetBook(D2CommonITEMSDataGetMagicSuffixAt(hItem,
					0)))->skillbookoffset;
				ptSkill = (void *)D2CommonSkillsFindByKey(hPlayer, dwSkill);
				if (ptSkill)
				{
					nCharges = (int)D2CommonSkillsGetNodeCount(ptSkill) - Count;
					if (nCharges <= 0)
					{
						if ((void *)D2CommonUnitGetRightSkill(hPlayer) == ptSkill)
							SkillsSetCurrent(hPlayer, FALSE, SKILL_ATTACK);
						nCharges = 0;
					}
					D2CommonSkillsSetNodeCount(ptSkill, nCharges);
					SCmdSendSkillState(SUnitGetClient(hPlayer), (BYTE)hPlayer->eType, hPlayer->dwId,
						(WORD)dwSkill, (BYTE)nCharges);
				}
			}
		}
		ptClient = SUnitGetClient(hPlayer);
		dwFlags = 0;
		D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
		D2CommonITEMSDataSetPreviousPage(hItem, D2CommonITEMSDataGetPageRaw((DWORD)hItem));
		SCmdSendItemFromGrid(ptClient, hPlayer, hItem, dwFlags);
		ITEMSModeRemoveGridItem(ptGame, hPlayer, hItem, FALSE);
	}
	else if (wItemMode == 4)
	{
		if (!ITEMSModeFreeCursorItem(ptGame, hPlayer, hItem))
			fSellable = FALSE;
	}
	else
	{
		int nLoc;

		ITEMSModeUnequipToCursor(ptGame, hPlayer, D2CommonITEMSGetCategoryRaw((DWORD)hItem), TRUE, &nLoc);
	}
	if (!fSellable)
	{
refused:
		NPCSENDRESULT(hPlayer, NPCRESULT_REFUSED, (DWORD)-1, 0);
		return;
	}
	ITEMSAddGold(ptGame, hPlayer, dwPrice);
	NPCSENDRESULT(hPlayer, NPCRESULT_SOLD, dwItemId, 3);
}

void __fastcall SUnitNpcRestoreHire(GAME *ptGame, UNIT *hPlayer, DWORD dwHire,
	DWORD dwSeed)
{
	MONPETDESC *ptHire;
	COLPOINT tCoord;
	GAMEROOM *hRoom;
	UNIT *hHire;
	DWORD dwNpcClass;
	int nHireClass;
	int nFirst;
	int nAct;
	int nIndex;
	MONPETDESC tDesc;
	NPCPROXY *ptProxy;

	nAct = D2CommonMonsterKillsLogClassify(0, dwHire);
	if (nAct < 0 || hPlayer->bAct != nAct)
		return;
	switch (nAct)
	{
	case 0:
		nHireClass = HIRECLASS_1;
		dwNpcClass = NPCCLASS_HIRE1;
		nFirst = HIREID_1_FIRST;
		break;
	case 1:
		nHireClass = HIRECLASS_2;
		dwNpcClass = NPCCLASS_HIRE2;
		nFirst = HIREID_2_FIRST;
		break;
	default:
		nHireClass = HIRECLASS_3;
		dwNpcClass = NPCCLASS_HIRE3;
		nFirst = HIREID_3_FIRST;
		break;
	}
	ptProxy = SUnitProxyFindNpc(ptGame, dwNpcClass, &nIndex);
	ptHire = ptProxy->ptHires;
	if (ptHire)
	{
		ptHire += (WORD)dwHire - nFirst;
		if (!ptHire->dwOffered)
			ptHire->dwHired = 1;
	}
	hHire = MonsterSpawnNear(ptGame, hPlayer, nHireClass, 1, 4, 0);
	if (!hHire)
	{
		tCoord.nX = (int)D2CommonUnitGetX(hPlayer);
		tCoord.nY = (int)D2CommonUnitGetY(hPlayer);
		D2CommonCollisnFindFreeArea(D2CommonUnitGetPathRoom(hPlayer), &tCoord, 1, 7169, &hRoom);
		if (hRoom)
		{
			hHire = MonsterSpawn(ptGame, hRoom, tCoord.nX, tCoord.nY, nHireClass, 1, -1, 0);
			if (!hHire)
				return;
		}
		tDesc.dwSeed = dwSeed;
		tDesc.wHireId = (WORD)dwHire;
		MonsterAIPetInit(ptGame, hPlayer, hHire, &tDesc);
		return;
	}
	tDesc.dwSeed = dwSeed;
	tDesc.wHireId = (WORD)dwHire;
	MonsterAIPetInit(ptGame, hPlayer, hHire, &tDesc);
}

UNIT *__fastcall SUnitNpcCopyItem(GAME *ptGame, UNIT *hItem, UNIT *hNpc)
{
	void *pInventory;
	ITEMCREATE tCreate;
	UNIT *hNewItem;
	UNIT *hNewItemItem;
	void *hNode;
	int nResult;
	UNIT *hKey;

	ITEMSFillCreate(ptGame, &tCreate, hItem, 1);
	hNewItem = ITEMSRecreate(ptGame, &tCreate);
#line 1744
	D2ASSERT(hNewItem);
	if (!ITEMSModeHasContents(hItem))
		return hNewItem;
	pInventory = hItem->ptInventory;
	for (hNode = D2CommonINVENTORYGetCategoryListHead(pInventory); hNode;
		hNode = (void *)D2CommonINVENTORYNodeGetNext(hNode))
	{
		hKey = (UNIT *)D2CommonINVENTORYNodeGetKeyA(hNode);
		ITEMSFillCreate(ptGame, &tCreate, hKey, 1);
		hNewItemItem = ITEMSRecreate(ptGame, &tCreate);
#line 1753
		D2ASSERT(hNewItemItem);
		D2CommonUnitSetMode(hNewItemItem, 4);
		if (!ITEMSModeSocketGem(ptGame, hNpc, hNewItemItem->dwId, hNewItem->dwId, &nResult, FALSE,
			TRUE,
			FALSE))
		{
#line 1758
			D2ASSERT(0);
		}
	}
	return hNewItem;
}

void __fastcall SUnitNpcBuy(GAME *ptGame  , UNIT *hPlayer, DWORD dwNpcId,
	DWORD dwItemId, DWORD dwUnused3, DWORD dwItemMode,
	DWORD dwUnused5)
{
	UNIT *hInteract;
	UNIT *hNpc;

#line 2029
	D2ASSERT(ptGame);
	hInteract = SUnitGetInteractUnit(ptGame, hPlayer);
	hNpc = SUnitFind(ptGame, UNIT_MONSTER, dwNpcId);
	if (hNpc && hInteract == hNpc)
	{
		SUnitNpcBuyItem(ptGame, hPlayer, hNpc, dwItemId, dwUnused3, dwItemMode, dwUnused5);
		return;
	}
	NPCSENDRESULT(hPlayer, NPCRESULT_REFUSED, (DWORD)-1, 0);
}

void __fastcall SUnitNpcBuyItem(GAME *ptGame  , UNIT *hPlayer, UNIT *hNpc,
	DWORD dwItemId, DWORD dwUnused3, DWORD dwItemMode,
	DWORD dwUnused5)
{
	UNIT *hItem;
	UNIT *hNew;
	UNIT *hTome;
	UNIT *hWeapon;
	void *hInventory;
	void *hOwnInventory;
	int nPrice;
	int nGold;
	int nGoldBank;
	int nGoldNow;
	DWORD dwClassId;
	DWORD dwWeaponType;
	int nResult;
	DWORD bDifficulty;
	DWORD dwMode;
	void *pRecord;
	BYTE fEquip;

	(void)dwUnused3;
	(void)dwUnused5;
#line 1775
	D2ASSERT(ptGame);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (hItem)
	{
		dwMode = dwItemMode & 65535;
		dwItemMode = dwMode;
		if (dwMode == 0)
		{
			if (ITEMSIsInInventory(hNpc, hItem, NULL))
				goto found;
		}
		else if (dwMode == 2)
		{
			hOwnInventory = SUnitProxyGetPlayerInventory(ptGame, hPlayer, hNpc);
			if (hOwnInventory)
			{
				if (ITEMSIsInInventory(hNpc, hItem, hOwnInventory) == 1)
					goto found;
				NPCSENDRESULT(hPlayer, NPCRESULT_NOITEM, dwItemId, 0);
				return;
			}
			NPCSENDRESULT(hPlayer, NPCRESULT_NOITEM, dwItemId, 0);
			return;
		}
		else
			goto found;
	}
	NPCSENDRESULT(hPlayer, NPCRESULT_NOITEM, dwItemId, 0);
	return;
found:
	bDifficulty = ptGame->bDifficultyLevel;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[bDifficulty];
	nPrice = (int)D2CommonITEMSGetPrice(hItem, bDifficulty, pRecord, hNpc->dwClassId, dwItemMode);
	nGold = D2CommonStatsUnitGetValue(hPlayer, STAT_GOLD);
	nGoldBank = D2CommonStatsUnitGetValue(hPlayer, STAT_GOLDBANK);
	if (nPrice > nGold + nGoldBank)
	{
		NPCSENDRESULT(hPlayer, NPCRESULT_NOGOLD, (DWORD)-1, 0);
		return;
	}
	hInventory = hPlayer->ptInventory;
	if (D2CommonINVENTORYGetCursorItem(hInventory))
	{
		NPCSENDRESULT(hPlayer, NPCRESULT_NOITEM, (DWORD)-1, 0);
		return;
	}
	if (ITEMSModeFindTomeFor(ptGame, hPlayer, hItem, TRUE))
	{
		nGoldNow = D2CommonStatsUnitGetValue(hPlayer, STAT_GOLD);
		if (D2CommonStatsUnitGetValue(hPlayer, STAT_GOLDBANK) + nGoldNow >= nPrice)
		{
			if (nPrice <= nGoldNow)
				D2CommonStatsUnitAddValue(hPlayer, STAT_GOLD, -nPrice);
			else
			{
				D2CommonStatsUnitAddValue(hPlayer, STAT_GOLD, -nGoldNow);
				D2CommonStatsUnitAddValue(hPlayer, STAT_GOLDBANK, nGoldNow - nPrice);
			}
		}
		else
		{
			NPCSENDRESULT(hPlayer, NPCRESULT_NOGOLD, (DWORD)-1, 0);
			return;
		}
		hTome = ITEMSModeFindTomeFor(ptGame, hPlayer, hItem, FALSE);
		SUnitNpcReplaceStock(ptGame, hNpc, hItem, hPlayer, dwItemMode);
		NPCSENDRESULT(hPlayer, NPCRESULT_BOUGHT, hTome->dwId, 5);
		return;
	}
	hNew = SUnitNpcCopyItem(ptGame, hItem, hNpc);
	if (!hNew)
	{
		NPCSENDRESULT(hPlayer, NPCRESULT_REFUSED, (DWORD)-1, 0);
		return;
	}
	nGoldNow = D2CommonStatsUnitGetValue(hPlayer, STAT_GOLD);
	if (D2CommonStatsUnitGetValue(hPlayer, STAT_GOLDBANK) + nGoldNow >= nPrice)
	{
		if (nPrice <= nGoldNow)
			D2CommonStatsUnitAddValue(hPlayer, STAT_GOLD, -nPrice);
		else
		{
			D2CommonStatsUnitAddValue(hPlayer, STAT_GOLD, -nGoldNow);
			D2CommonStatsUnitAddValue(hPlayer, STAT_GOLDBANK, nGoldNow - nPrice);
		}
	}
	else
	{
		NPCSENDRESULT(hPlayer, NPCRESULT_NOGOLD, (DWORD)-1, 0);
		return;
	}
	((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->dwBoughtItemId = hNew->dwId;
	D2CommonUnitSetMode(hNew, 4);
	dwClassId = hNew->dwClassId;
	if (D2CommonITEMSTestAutobeltOrFits(hInventory, hNew)
		&& ITEMSModePutInBelt(ptGame, hPlayer, hNew->dwId, 0, TRUE, &nResult))
		goto bought;
	fEquip = TRUE;
	hWeapon = (UNIT *)D2CommonINVENTORYFindEquippedMatch(hInventory);
	dwWeaponType = D2CommonUnitGetItemFlagsWord(hPlayer);
	if (dwClassId == gdwItemCqv && hWeapon && dwWeaponType != 7)
		fEquip = FALSE;
	if (dwClassId == gdwItemAqv && hWeapon && dwWeaponType != 1)
		fEquip = FALSE;
	if (hWeapon && D2CommonITEMSIsWeapon(hNew))
		fEquip = FALSE;
	if (!hWeapon && (dwClassId == gdwItemCqv || dwClassId == gdwItemAqv))
		fEquip = FALSE;
	if (fEquip && ITEMSModeEquipFromGround(ptGame, hPlayer, hNew))
		goto bought;
	D2CommonITEMSDataSetPage(hNew, 0);
	if (!ITEMSModePlaceCursorItem(ptGame, hPlayer, hNew->dwId, 0, 0, TRUE, TRUE, NULL))
	{
		if (nGold - D2CommonStatsUnitGetValue(hPlayer, STAT_GOLD))
			D2CommonStatsUnitAddValue(hPlayer, STAT_GOLD, nGold - D2CommonStatsUnitGetValue(hPlayer,
				STAT_GOLD));
		if (nGoldBank - D2CommonStatsUnitGetValue(hPlayer, STAT_GOLDBANK))
			D2CommonStatsUnitAddValue(hPlayer, STAT_GOLDBANK,
				nGoldBank - D2CommonStatsUnitGetValue(hPlayer, STAT_GOLDBANK));
		NPCSENDRESULT(hPlayer, NPCRESULT_NOROOM, (DWORD)-1, 0);
		return;
	}

bought:
	SUnitNpcReplaceStock(ptGame, hNpc, hItem, hPlayer, dwItemMode);
	D2CommonITEMSDataSetFlags(hNew, 2, 1);
	NPCSENDRESULT(hPlayer, NPCRESULT_BOUGHT, hNew->dwId, 4);
}

void __fastcall SUnitNpcReplaceStock(GAME *ptGame, UNIT *hNpc, UNIT *hItem, UNIT *hPlayer, DWORD dwItemMode
)
{
	NPCPROXY *pInv;
	NPCPLAYERINV *ptOwn;
	void *hNode;
	UNIT *hFound;
	void *hNext;
	DWORD dwPlayerId;
	int nIndex;
	int nLevel;

	pInv = SUnitProxyFindNpcOf(ptGame, hNpc, &nIndex);
	if (pInv->dwGambler && dwItemMode == 2)
	{
		ptOwn = pInv->ptPlayerInvs;
		dwPlayerId = hPlayer->dwId;
		for (; ptOwn; ptOwn = ptOwn->ptNext)
		{
			if (ptOwn->dwUnitId != dwPlayerId)
				continue;
			if (!ptOwn->ptInventory)
				break;
			for (hNode = D2CommonINVENTORYGetCategoryListHead(ptOwn->ptInventory); hNode; hNode = hNext)
			{
				hFound = (UNIT *)D2CommonINVENTORYNodeGetKeyA(hNode);
				hNext = (void *)D2CommonINVENTORYNodeGetNext(hNode);
				if (hFound == hItem)
				{
					SUnitNpcTakeGambleI(hPlayer, hNpc, hItem, ptGame);
					return;
				}
			}
			break;
		}
	}
	nLevel = SUnitNpcStockLevelI(ptGame, hPlayer, hNpc);
	SUnitProxyOrder(hNpc, ptGame, nLevel, 0, D2CommonITEMSDataGetModType(hItem));
	SUnitNpcReleaseItem(hNpc, hItem, ptGame, TRUE);
}

static void SUnitNpcHireOffer(GAME *ptGame, UNIT *hPlayer, UNIT *hNpc, WORD wHire)
{
	MONKILLSRECORD tRecord;
	NPCPROXY *pInv;
	MONPETDESC *ptHire;
	UNIT *hHire;
	DWORD dwHireClass;
	DWORD dwClassId;
	int nLevel;
	int nIndex;
	int nFirst;
	int nLast;
	int nPrice;
	int nGold;
	BOOL fFirst;
	void *pQuest;
	BOOL fPaid;
	int nThreats;
	THREATNODE **pptThreats;

	nLevel = SUnitNpcStockLevelI(ptGame, hPlayer, hNpc);
	D2CommonUnitGetPlrInfo(hPlayer);
	if (hNpc->dwClassId == NPCCLASS_HIRE1 && nLevel < 8)
	{
		pQuest = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
		if (!D2CommonStepTestGridBit(pQuest, 2, 0))
		{
			NPCSENDRESULT(hPlayer, NPCRESULT_NOSALE, (DWORD)-1, 0);
			return;
		}
	}
	pInv = SUnitProxyFindNpcOf(ptGame, hNpc, &nIndex);
	if (!pInv)
		goto refused;
	dwClassId = hNpc->dwClassId;
	if (dwClassId == NPCCLASS_HIRE2)
	{
		nFirst = HIREID_2_FIRST;
		nLast = HIREID_2_LAST;
		dwHireClass = HIRECLASS_2;
	}
	else if (dwClassId == NPCCLASS_HIRE3)
	{
		nFirst = HIREID_3_FIRST;
		nLast = HIREID_3_LAST;
		dwHireClass = HIRECLASS_3;
	}
	else
	{
		nFirst = HIREID_1_FIRST;
		nLast = HIREID_1_LAST;
		dwHireClass = HIRECLASS_1;
	}
	if ((int)wHire < nFirst || (int)wHire > nLast)
		goto refused;
	ptHire = &pInv->ptHires[wHire - nFirst];
	if (ptHire->wHireId != wHire || ptHire->dwHired)
		goto refused;
	tRecord.nKind = D2CommonMonsterKillsLogClassify(0, ptHire->wHireId);
	D2CommonMonsterKillsLogRollRecord(ptHire->dwSeed, &tRecord, ptGame->bDifficultyLevel);
	nPrice = tRecord.nCost;
	nGold = D2CommonStatsUnitGetValue(hPlayer, STAT_GOLD);
	fPaid = TRUE;
	if (D2CommonStatsUnitGetValue(hPlayer, STAT_GOLDBANK) + nGold >= nPrice)
	{
		if (nPrice <= nGold)
			D2CommonStatsUnitAddValue(hPlayer, STAT_GOLD, -nPrice);
		else
		{
			D2CommonStatsUnitAddValue(hPlayer, STAT_GOLD, -nGold);
			D2CommonStatsUnitAddValue(hPlayer, STAT_GOLDBANK, nGold - nPrice);
		}
	}
	else
	{
		NPCSENDRESULT(hPlayer, NPCRESULT_NOGOLD, (DWORD)-1, 0);
		fPaid = FALSE;
	}
	if (!fPaid)
		return;
	hHire = MonsterSpawnNear(ptGame, hNpc, dwHireClass, 1, 4, 0);
	if (!hHire)
	{
		hHire = MonsterSpawnNear(ptGame, hPlayer, dwHireClass, 1, 4, 0);
		if (!hHire)
		{
			NPCSENDRESULT(hPlayer, NPCRESULT_NOHIRE, (DWORD)-1, 0);
			return;
		}
	}
	ptHire->dwHired = 1;
	MonsterAIPetInit(ptGame, hPlayer, hHire, ptHire);

	pptThreats = ((MONSTERDATA *)hNpc->uTypeData)->pptThreats;
	nThreats = MonsterAICountThreats(pptThreats);
	SUnitNpcSendHires(ptGame, SUnitGetClient(hPlayer), hNpc, (nThreats <= 1));
	NPCSENDRESULT(hPlayer, NPCRESULT_HIRED, hHire->dwId, 0);
	SUnitNpcRenewHires(ptGame, dwClassId, pInv);
	return;

refused:
	NPCSENDRESULT(hPlayer, NPCRESULT_REFUSED, (DWORD)-1, 0);
}

void __fastcall SUnitNpcHire(GAME *ptGame, UNIT *hPlayer, DWORD dwNpcId, WORD wHire)
{
	UNIT *hInteract;
	UNIT *hNpc;

	hInteract = SUnitGetInteractUnit(ptGame, hPlayer);
	hNpc = SUnitFind(ptGame, UNIT_MONSTER, dwNpcId);
	if (hNpc && hInteract == hNpc)
	{
		SUnitNpcHireOffer(ptGame, hPlayer, hNpc, wHire);
		return;
	}
	NPCSENDRESULT(hPlayer, NPCRESULT_REFUSED, (DWORD)-1, 0);
}

void __fastcall SUnitNpcRenewHires(GAME *ptGame, DWORD dwClassId, NPCPROXY *pInv
	 )
{
	MONPETDESC *ptHire;
	int nLeft;
	int i;

	nLeft = 0;
	ptHire = pInv->ptHires;
	for (i = NPCHIRE_COUNT; i; i--, ptHire++)
	{
		if (ptHire->dwOffered && !ptHire->dwHired)
			nLeft++;
	}
	if (nLeft)
		return;
#line 1512
	D2FREEPOOL(ptGame->pMemPool, pInv->ptHires);
	pInv->ptHires = NULL;
	pInv->ItemList.bHiresDrawn = 0;
	SUnitNpcInitHires(ptGame, dwClassId, pInv);
}

void __fastcall SUnitNpcRepairItem(GAME *ptGame  , UNIT *hPlayer, DWORD dwNpcId,
	DWORD dwItemId, WORD wItemMode, DWORD dwUnused4)
{
	UNIT *hInteract;
	UNIT *hNpc;
	UNIT *hItem;
	DWORD dwPerPoint;
	int nDurability;
	int nPrice;
	int nGold;
	int nGoldNow;
	int nPoints;
	int nNew;
	DWORD dwDifficulty;
	BOOL fFull;
	void *pQuest;
	BOOL fShort;

	(void)wItemMode;
	(void)dwUnused4;
#line 2069
	D2ASSERT(ptGame);
	hInteract = SUnitGetInteractUnit(ptGame, hPlayer);
	hNpc = SUnitFind(ptGame, UNIT_MONSTER, dwNpcId);
	if (!hNpc || hInteract != hNpc)
		goto refused;
	if (hNpc->dwClassId != 257 && hNpc->dwClassId != 154 && hNpc->dwClassId != 178
		&& hNpc->dwClassId != 253)
		goto refused;
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem || !ITEMSIsInInventory(hPlayer, hItem, NULL))
		goto refused;
	dwItemId = D2CommonStatsUnitCalcMaxDurability(hItem);
	if (!dwItemId)
		goto refused;
	dwDifficulty = ptGame->bDifficultyLevel;
	pQuest = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[dwDifficulty];
	nPrice = (int)D2CommonITEMSGetPrice(hItem, dwDifficulty, pQuest, hNpc->dwClassId, 3);
	nDurability = D2CommonStatsUnitGetValue(hItem, STAT_DURABILITY);
	nGold = D2CommonStatsUnitGetValue(hPlayer, STAT_GOLD);
	fFull = TRUE;
	if (D2CommonStatsUnitGetValue(hPlayer, STAT_GOLDBANK) + nGold >= nPrice)
	{
		if (nPrice <= nGold)
			D2CommonStatsUnitAddValue(hPlayer, STAT_GOLD, -nPrice);
		else
		{
			D2CommonStatsUnitAddValue(hPlayer, STAT_GOLD, -nGold);
			D2CommonStatsUnitAddValue(hPlayer, STAT_GOLDBANK, nGold - nPrice);
		}
	}
	else
	{
		fShort = TRUE;
		nGold = D2CommonStatsUnitGetValue(hPlayer, STAT_GOLD);
		dwPerPoint = ((DWORD)nPrice << 10) / (dwItemId - nDurability);
		if (nGold <= 0 || (int)dwPerPoint >= nGold << 10 || (int)dwPerPoint <= 0)
			goto nogold;
		nPoints = (nGold << 10) / (int)dwPerPoint;
		if (D2CommonITEMSDataTestFlags(hItem, 256))
			goto nogold;
		nGoldNow = D2CommonStatsUnitGetValue(hPlayer, STAT_GOLD);
		if (D2CommonStatsUnitGetValue(hPlayer, STAT_GOLDBANK) + nGoldNow < nGold)
			goto nogold;
		if (nGold <= nGoldNow)
			D2CommonStatsUnitAddValue(hPlayer, STAT_GOLD, -nGold);
		else
		{
			D2CommonStatsUnitAddValue(hPlayer, STAT_GOLD, -nGoldNow);
			D2CommonStatsUnitAddValue(hPlayer, STAT_GOLDBANK, nGoldNow - nGold);
		}
		nNew = (int)dwItemId;
		if (nNew >= nPoints + nDurability)
			nNew = nPoints + nDurability;
		D2CommonStatsUnitSetValue(hItem, STAT_DURABILITY, nNew);
		fShort = FALSE;
nogold:
		if (fShort)
		{
			NPCSENDRESULT(hPlayer, NPCRESULT_NOGOLD, (DWORD)-1, 0);
			return;
		}
		fFull = FALSE;
	}
	if (fFull)
	{
		if (D2CommonITEMSDataTestFlags(hItem, 256))
		{
			ITEMSModeRepairItem(ptGame, hPlayer, hItem);
			goto repaired;
		}
		nNew = D2CommonStatsUnitCalcMaxDurability(hItem);
		if (nNew <= 0)
			goto repaired;
		D2CommonStatsUnitSetValue(hItem, STAT_DURABILITY, nNew);
		if (!hPlayer)
			goto repaired;
	}
	SCmdSendItemStat(SUnitGetClient(hPlayer), hItem, 1, STAT_DURABILITY, nNew);
repaired:
	NPCSENDRESULT(hPlayer, NPCRESULT_REPAIRED, (DWORD)-1, 1);
	return;

refused:
	NPCSENDRESULT(hPlayer, NPCRESULT_REFUSED, (DWORD)-1, 0);
}

static void * SUnitNpcInventory(UNIT *h)
{
	return h->ptInventory;
}

void __fastcall SUnitNpcRepairAll(GAME *ptGame, UNIT *hPlayer, DWORD dwNpcId)
{
	UNIT *hInteract;
	UNIT *hNpc;
	UNIT *hItem;
	void *hNode;
	void *hNext;
	DWORD dwLoc;
	int nCost;
	int nGold;
	void *pRec;

	hInteract = SUnitGetInteractUnit(ptGame, hPlayer);
	hNpc = SUnitFind(ptGame, UNIT_MONSTER, dwNpcId);
	if (!hNpc || hInteract != hNpc)
		goto refused;
	nCost = (int)D2CommonITEMSCountUnidentified(hPlayer);
	if (!nCost)
		goto refused;
	pRec = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRec, 4, 0) != 1)
	{
		pRec = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
		if (D2CommonStepTestGridBit(pRec, 4, 1) != 1)
		{
			nCost *= 100;
			nGold = D2CommonStatsUnitGetValue(hPlayer, STAT_GOLD);
			if (D2CommonStatsUnitGetValue(hPlayer, STAT_GOLDBANK) + nGold >= nCost)
			{
				if (nCost <= nGold)
					D2CommonStatsUnitAddValue(hPlayer, STAT_GOLD, -nCost);
				else
				{
					D2CommonStatsUnitAddValue(hPlayer, STAT_GOLD, -nGold);
					D2CommonStatsUnitAddValue(hPlayer, STAT_GOLDBANK, nGold - nCost);
				}
			}
			else
			{
				NPCSENDRESULT(hPlayer, NPCRESULT_NOGOLD, (DWORD)-1, 0);
				return;
			}
		}
	}
	for (hNode = D2CommonINVENTORYGetCategoryListHead(SUnitNpcInventory(hPlayer)); hNode; hNode = hNext)
	{
		hNext = (void *)D2CommonINVENTORYNodeGetNext(hNode);
		dwLoc = D2CommonINVENTORYNodeGetCategory(hNode);
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(hNode);
		if ((dwLoc == 1 && (!D2CommonITEMSDataGetPage(hItem) || D2CommonITEMSDataGetPage(hItem) == 3)) || dwLoc == 3)
		{
			if (!D2CommonITEMSDataTestFlags(hItem, 16))
				ITEMSModeMarkHandItem(hPlayer, (UNIT *)D2CommonINVENTORYNodeGetKeyA(hNode));
		}
	}
	NPCSENDRESULT(hPlayer, NPCRESULT_REPAIREDALL, (DWORD)-1, 0);
	return;

refused:
	NPCSENDRESULT(hPlayer, NPCRESULT_REFUSED, (DWORD)-1, 0);
}

static void SUnitNpcRefuseReward(GAME *ptGame, UNIT *hPlayer, CLIENT *ptClient, NPCREWARDPACKET *ptPacket,
	DWORD dwItemId)
{
	ptPacket->bResult = 7;
	SCmdSendUseItemState(ptClient, (const struct SCMDCOPY7 *)ptPacket);
	ITEMSModeDropCursorItem(ptGame, hPlayer, dwItemId);
}

void __fastcall SUnitNpcInteract(GAME *ptGame, UNIT *hPlayer, DWORD dwAction, DWORD dwNpcId,
	DWORD dwItemId)
{
	ITEMCREATE tCreate;
	ITEMCREATE tCreateBack;
	COLPOINT tCoord;
	COLPOINT tSpawn;
	NPCREWARDPACKET tPacket;
	GAMEROOM *hRoom;
	CLIENT *ptClient;
	UNIT *hNpc;
	UNIT *hItem;
	UNIT *hReward;
	DWORD dwClassId;
	DWORD dwWaypoint;
	BOOL fFirst;
	int nLevel;
	void *pRecord;

	hNpc = SUnitFind(ptGame, UNIT_MONSTER, dwNpcId);
	if (!hNpc || !hPlayer)
		return;
	dwClassId = hNpc->dwClassId;
	fFirst = FALSE;
	if (MonsterAICountThreats(((MONSTERDATA *)hNpc->uTypeData)->pptThreats) == 1)
		fFirst = TRUE;
	if (dwAction == 1)
	{
		MonsterAIAckThreat(hNpc, hPlayer);
		SUnitNpcOpenTrade(ptGame, hPlayer, hNpc, fFirst, FALSE);
		return;
	}
	if (dwAction == 2)
	{
		MonsterAIAckThreat(hNpc, hPlayer);
		SUnitNpcOpenTrade(ptGame, hPlayer, hNpc, fFirst, TRUE);
		return;
	}
	if (dwAction == 3)
	{

		SUnitNpcSendHires(ptGame, SUnitGetClient(hPlayer), hNpc, (MonsterAICountThreats(((MONSTERDATA *)hNpc->uTypeData)->pptThreats) <= 1));
		return;
	}
	if (dwClassId == 154)
	{
		tPacket.bCmd = SCMD_NPCREWARD;
		tPacket.dwNpcId = dwNpcId;
		ptClient = SUnitGetClient(hPlayer);
		hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
		pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
		if (!D2CommonStepTestGridBit(pRecord, 3, 1))
		{
			SUnitNpcRefuseReward(ptGame, hPlayer, ptClient, &tPacket, dwItemId);
			return;
		}
		if (!hItem)
			return;
		if (!D2CommonITEMSTestModifiable(hItem))
		{
			SUnitNpcRefuseReward(ptGame, hPlayer, ptClient, &tPacket, dwItemId);
			return;
		}
		ITEMSFillCreate(ptGame, &tCreate, hItem, 0);
		ITEMSFillCreate(ptGame, &tCreateBack, hItem, 1);
		if (!ITEMSModeFreeCursorItem(ptGame, hPlayer, hItem))
		{
			SUnitNpcRefuseReward(ptGame, hPlayer, ptClient, &tPacket, dwItemId);
			return;
		}
		tCreate.dwQuality = 6;
		nLevel = ITEMSGetUnitLevel(hPlayer, LEVEL_ID_NONE);
		if (nLevel > 5)
			nLevel += 4;
		tCreate.nLevel = nLevel;
		hReward = ITEMSCreateFromRecord(ptGame, &tCreate, FALSE);
		if (!hReward)
		{
			tPacket.bResult = 7;
			SCmdSendUseItemState(ptClient, (const struct SCMDCOPY7 *)&tPacket);
			hItem = ITEMSRecreate(ptGame, &tCreateBack);
			D2CommonUnitSetMode(hItem, 4);
			ITEMSModeDropCursorItem(ptGame, hPlayer, hItem->dwId);
			return;
		}
		if (D2CommonITEMSDataTestFlags(hReward, 256))
			ITEMSModeRepairItem(ptGame, NULL, hReward);
		else
		{
			nLevel = D2CommonStatsUnitCalcMaxDurability(hReward);
			if (nLevel > 0)
				D2CommonStatsUnitSetValue(hReward, STAT_DURABILITY, nLevel);
		}
		D2CommonITEMSDataSetFlags(hReward, 16, 1);
		D2CommonITEMSDataSetPage(hReward, 0);
		if (!ITEMSModePlaceCursorItem(ptGame, hPlayer, hReward->dwId, 0, 0, TRUE, TRUE, NULL))
		{
			D2CommonUnitSetMode(hReward, 3);
			D2CommonUnitGetPosition(hPlayer, &tCoord);
			hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hPlayer);
			hRoom = ITEMSFindDropSpot(hRoom, &tCoord, &tSpawn, 1);
			if (hRoom)
			{
				D2CommonUnitInitPosition(0, hReward, hRoom, tSpawn.nX, tSpawn.nY);
				D2CommonUnitSetCollision(hReward, hRoom, tSpawn.nX, tSpawn.nY);
				D2CommonAddUnitToRoom(hReward, hRoom);
			}
			else
			{
				nLevel = D2CommonStatsUnitGetValue(hPlayer, STAT_LEVEL) * 1000;
				D2CommonStatsUnitAddValue(hPlayer, STAT_GOLD, nLevel);
			}
		}
		A1Q3Deactivate(ptGame, hPlayer);
		tPacket.bResult = 6;
		SCmdSendUseItemState(ptClient, (const struct SCMDCOPY7 *)&tPacket);
		return;
	}
	if (dwClassId == 155)
	{
		PlrMsgWarpToLevel(ptGame, hPlayer, LEVEL_ACT_2_TOWN, 0);
		QuestsChangedLevel(ptGame, hPlayer, (QUESTLEVELMSG *)hNpc, 40, 1);
		if (D2CommonWaypointGetLevelIndex(LEVEL_ACT_2_TOWN, &dwWaypoint))
		{
			WORD *pwWaypoints = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apwWaypoints[ptGame->bDifficultyLevel];

			D2CommonWaypointSetField(pwWaypoints, dwWaypoint);
		}
		return;
	}
	if (dwClassId == 175)
	{
		PlrMsgWarpToLevel(ptGame, hPlayer, LEVEL_ACT_1_TOWN, 5);
		return;
	}
	if (dwClassId == 210)
	{
		if (dwAction)
			return;
		PlrMsgWarpToLevel(ptGame, hPlayer, LEVEL_ACT_3_TOWN, 0);
		QuestsChangedLevel(ptGame, hPlayer, (QUESTLEVELMSG *)hNpc, 75, 40);
		if (D2CommonWaypointGetLevelIndex(LEVEL_ACT_3_TOWN, &dwWaypoint))
		{
			WORD *pwWaypoints = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apwWaypoints[ptGame->bDifficultyLevel];

			D2CommonWaypointSetField(pwWaypoints, dwWaypoint);
		}
		return;
	}
	if (dwClassId == 264 && !dwAction)
		PlrMsgWarpToLevel(ptGame, hPlayer, LEVEL_ACT_2_TOWN, 5);
}

void __fastcall SUnitNpcOpenTrade(GAME *ptGame, UNIT *hPlayer, UNIT *hNPC  ,
	BOOL fFirst, BOOL fOwn)
{
	NPCPROXY *pInv;
	NPCPROXY *pNpc;
	NPCSTATE *ptState;
	NPCPLAYERINV *ptOwn;
	void *hNode;
	void *hInventory;
	UNIT *hItem;
	DWORD dwClassId;
	BOOL fFound;
	DWORD dwPlayerId;
	int nIndex;
	int nIndex2;

#line 746
	D2ASSERT(hNPC);
	pInv = SUnitProxyFindNpcOf(ptGame, hNPC, &nIndex);
	pInv->dwTradeOpened = 1;
	pInv->dwUnitId = hNPC->dwId;
	dwClassId = hNPC->dwClassId;
	pNpc = SUnitProxyFindNpcOf(ptGame, hNPC, &nIndex2);
	if (pNpc)
	{
		if (fOwn)
		{
			if (pNpc->dwGambler)
			{
				SUnitProxyAddVisit(ptGame, pNpc, hPlayer)->bOwnStock = 1;
				ptOwn = pNpc->ptPlayerInvs;
				dwPlayerId = hPlayer->dwId;
				fFound = FALSE;
				for (; ptOwn; ptOwn = ptOwn->ptNext)
				{
					if (ptOwn->dwUnitId == dwPlayerId)
						fFound = TRUE;
				}
				if (!fFound)
					SUnitNpcFillGamble(ptGame, hNPC, hPlayer, pNpc);
			}
		}
		else
		{
			ptState = &pNpc->ItemList;
			if (ptState && ptState->bVendor)
			{
				SUnitProxyAddVisit(ptGame, pNpc, hPlayer)->bOwnStock = 0;
				if (!ptState->bStockFilled)
				{
					SUnitNpcFillStock(ptGame, hPlayer, hNPC, ptState);
					ptState->bStockFilled = 1;
				}
				if (!ptState->bHiresDrawn)
				{
					if (dwClassId == NPCCLASS_HIRE1 || dwClassId == NPCCLASS_HIRE2
						|| dwClassId == NPCCLASS_HIRE3)
						SUnitNpcInitHires(ptGame, dwClassId, pNpc);
					ptState->bHiresDrawn = 1;
				}
			}
		}
	}
	if (hNPC->dwClassId == NPCCLASS_HIRE1 || hNPC->dwClassId == NPCCLASS_HIRE2)
		return;
	if (fFirst && !fOwn && pInv)
	{
		ptState = &pInv->ItemList;
		if (ptState->bRenewPending)
		{
			ptState->bRenewPending = 0;
			SUnitProxyResetNpc(ptGame, pInv, hNPC);
			SUnitNpcFillStock(ptGame, hPlayer, hNPC, ptState);
			ptState->bStockFilled = 1;
		}
	}

	D2CommonUnitSetChanged(hNPC, 1);
	if (!fOwn)
	{
		hNode = D2CommonINVENTORYGetCategoryListHead(SUnitNpcInventory(hNPC));
		while (hNode)
		{
			hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(hNode);
			hNode = (void *)D2CommonINVENTORYNodeGetNext(hNode);
			hItem->dwFlags |= 4;
			if (ITEMSModeHasContents(hItem))
				D2CommonITEMSDataSetFlags(hItem, 1, 1);
			(void)D2CommonINVENTORYAddDeltaNode(hNPC->ptInventory, hItem);
		}
	}
	else
	{
		hInventory = SUnitProxyGetPlayerInventory(ptGame, hPlayer, hNPC);
		if (hInventory)
		{
			hNode = D2CommonINVENTORYGetCategoryListHead(hInventory);
			while (hNode)
			{
				hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(hNode);
				hNode = (void *)D2CommonINVENTORYNodeGetNext(hNode);
				hItem->dwFlags |= 4;
				if (ITEMSModeHasContents(hItem))
					D2CommonITEMSDataSetFlags(hItem, 1, 1);
				(void)D2CommonINVENTORYAddDeltaNode(hNPC->ptInventory, hItem);
			}
		}
	}
}

void __fastcall SUnitNpcMarkBought(GAME *ptGame, UNIT *hPlayer, DWORD dwItemId)
{
	UNIT *hItem;
	PLRINFO *pInfo;

	pInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
	if (pInfo->dwBoughtItemId == dwItemId)
	{
		hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
		if (hItem)
			ITEMSModeMarkHandItem(hPlayer, hItem);
	}
}

void __fastcall SUnitNpcFillGamble(GAME *ptGame, UNIT *hNpc, UNIT *hPlayer, NPCPROXY *pInv
	 )
{
	NPCGAMBLELIST *ptGamble;
	NPCPLAYERINV *ptOwn;
	D2SEED *ptSeed;
	UNIT *hItem;
	DWORD dwRow;
	int nTries;
	int nItems;
	int nLevel;
	int nItemLevel;
	int nDraws;
	int nQuality;
	int nRoll;
	int nDurability;
	int nY;

	if (!sgdwItemRin)
		D2CommonItemTblsFindItemByCode(544106866, &sgdwItemRin);
	ptGamble = (NPCGAMBLELIST *)D2CommonItemTblsGetGambleItems();
	if (!ptGamble)
		return;
	nItems = 0;
	nTries = 0;
#line 2497
	ptOwn = (NPCPLAYERINV *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(NPCPLAYERINV));
	ptOwn->ptInventory = (void *)D2CommonINVENTORYCreate(ptGame->pMemPool, 0, 6, 0, 0);
	ptOwn->dwUnitId = hPlayer->dwId;
	ptOwn->ptNext = pInv->ptPlayerInvs;
	pInv->ptPlayerInvs = ptOwn;
	nLevel = D2CommonStatsUnitGetValue(hPlayer, STAT_LEVEL);
	ptSeed = SUnitProxyGetSeed(ptGame);
	while (nItems < NPC_GAMBLEITEMS)
	{
		if (nTries >= NPC_GAMBLETRIES)
			return;
		nRoll = (int)D2CommonSeedRand(ptSeed, 10) + nLevel - 5;
		if (nRoll < 5)
			nItemLevel = 5;
		else
			nItemLevel = nRoll;
		nDraws = 0;
		do
		{
			dwRow = ptGamble->pdwRows[ptGamble->nCount ? D2CommonSeedRand(ptSeed,
				ptGamble->nCount) : 0];
			nDraws++;
			if (((ITEMSTXT *)D2CommonItemTblsGetItemStats(dwRow))->level <= nItemLevel)
				break;
			if (nDraws > 6)
			{
				dwRow = sgdwItemRin;
				break;
			}
		} while (TRUE);
		nQuality = 4;
		nRoll = (int)D2CommonSeedRand(ptSeed, 10000);
		if (nRoll > 8500 && nRoll < 9200)
			nQuality = 6;
		else if (nRoll >= 9200 && nRoll < 9700)
			nQuality = 5;
		else if (nRoll >= 9700)
			nQuality = 7;
		hItem = ITEMSCreate(hNpc, dwRow, ptGame, 4, nQuality, 0, nItemLevel, FALSE, 0, 0);
		if (hItem)
		{
			nItems++;
			D2CommonITEMSDataSetPage(hItem, 0);
			if (D2CommonITEMSDataTestFlags(hItem, 256))
				ITEMSModeRepairItem(ptGame, NULL, hItem);
			else
			{
				nDurability = D2CommonStatsUnitCalcMaxDurability(hItem);
				if (nDurability > 0)
					D2CommonStatsUnitSetValue(hItem, STAT_DURABILITY, nDurability);
			}
			D2CommonITEMSDataSetFlags(hItem, 16, 0);
			if (!ITEMSModePlaceCursorItem(ptGame, hNpc, hItem->dwId,
				(int)D2CommonUnitGetX(hItem), (int)D2CommonUnitGetY(hItem),
				TRUE, FALSE, ptOwn->ptInventory))
			{
				ITEMSFree(ptGame, hItem);
				return;
			}
		}
		nTries++;
	}
}

void __fastcall SUnitNpcTellThreat(GAME *ptGame, UNIT *hUnit, UNIT *hNpc)
{
	gdwNpcHealerId = hNpc->dwId;
	if (hNpc->dwClassId == 148 || hNpc->dwClassId == 178 || hNpc->dwClassId == 405
		|| hNpc->dwClassId == 255)
		SUnitNpcHeal(ptGame, hUnit, hNpc);
}

void __fastcall SUnitNpcHeal(GAME *ptGame, UNIT *hPlayer, UNIT *hNpc)
{
	void *ptState;
	DWORD dwMax;
	BOOL fChanged;
	BOOL fCured;
	int i;

	if (D2CommonStatsGetState(hPlayer, 31))
		D2CommonStatsUnitSetActiveStateBit(hPlayer, 31, 0);
	if (SUnitGetInteractUnit(ptGame, hPlayer) != hNpc)
		return;
	fChanged = FALSE;
	dwMax = D2CommonStatsUnitCalcMaxHp(hPlayer);
	if ((DWORD)D2CommonStatsUnitGetValue(hPlayer, STAT_LIFE) < dwMax)
	{
		D2CommonStatsUnitSetValue(hPlayer, STAT_LIFE, dwMax);
		PlrMsgStatChanged(hPlayer, STAT_LIFE, (int)dwMax, hPlayer);
		fChanged = TRUE;
	}
	dwMax = D2CommonStatsUnitCalcMaxMana(hPlayer);
	if ((DWORD)D2CommonStatsUnitGetValue(hPlayer, STAT_MANA) < dwMax)
	{
		D2CommonStatsUnitSetValue(hPlayer, STAT_MANA, dwMax);
		PlrMsgStatChanged(hPlayer, STAT_MANA, (int)dwMax, hPlayer);
		fChanged = TRUE;
	}
	dwMax = D2CommonStatsUnitCalcMaxStamina(hPlayer);
	if ((DWORD)D2CommonStatsUnitGetValue(hPlayer, STAT_STAMINA) < dwMax)
	{
		D2CommonStatsUnitSetValue(hPlayer, STAT_STAMINA, dwMax);
		PlrMsgStatChanged(hPlayer, STAT_STAMINA, (int)dwMax, hPlayer);
		fChanged = TRUE;
	}
	ptState = (void *)D2CommonStatsFindListByState(hPlayer, 2);
	if (ptState)
	{
		D2CommonStatsUnitUnlinkList(hPlayer, ptState);
		D2CommonStatsListFree(ptState);
		fChanged = TRUE;
	}
	ptState = (void *)D2CommonStatsFindListByState(hPlayer, 1);
	if (ptState)
	{
		D2CommonStatsUnitUnlinkList(hPlayer, ptState);
		D2CommonStatsListFree(ptState);
		fChanged = TRUE;
	}
	fCured = FALSE;
	for (i = 0; i < NPC_STATES; i++)
	{
		if (D2CommonStatsGetState(hPlayer, i) && D2CommonStatsTestStateSharedListBase(i))
		{
			ptState = (void *)D2CommonStatsFindListByState(hPlayer, i);
			if (ptState)
			{
				D2CommonStatsUnitUnlinkList(hPlayer, ptState);
				D2CommonStatsListFree(ptState);
				fCured = TRUE;
			}
		}
	}
	if (fCured)
		fChanged = TRUE;
	PlayerPetsForEach(ptGame, hPlayer, SUnitNpcHealPet, &fChanged);
	if (fChanged == TRUE)
		SUnitSetEvent(hNpc, 10, NULL);
}

void __fastcall SUnitNpcHealPet(GAME *ptGame, UNIT *hPlayer, UNIT *hPet, void *pArg)
{
	void *ptState;
	int nLife;
	int nMax;
	BOOL fCured;
	int i;

	(void)ptGame;
	(void)hPlayer;
	nLife = D2CommonStatsUnitGetValue(hPet, STAT_LIFE);
	nMax = D2CommonStatsUnitCalcMaxHp(hPet);
	if (nLife < nMax)
	{
		D2CommonStatsUnitSetValue(hPet, STAT_LIFE, nMax);
		*(BYTE *)pArg = 1;
	}
	fCured = FALSE;
	for (i = 0; i < NPC_STATES; i++)
	{
		if (D2CommonStatsGetState(hPet, i) && D2CommonStatsTestStateSharedListBase(i))
		{
			ptState = (void *)D2CommonStatsFindListByState(hPet, i);
			if (ptState)
			{
				D2CommonStatsUnitUnlinkList(hPet, ptState);
				D2CommonStatsListFree(ptState);
				fCured = TRUE;
			}
		}
	}
	if (fCured)
		*(BYTE *)pArg = 1;
}

void __fastcall SUnitNpcBuyHeal(GAME *ptGame, UNIT *hPlayer, DWORD dwNpcId)
{
	UNIT *hNpc;
	UNIT *hInteract;
	BOOL fHurt;
	DWORD dwMax;
	int nPrice;
	int nGold;
	BOOL fPaid;

	hNpc = SUnitFind(ptGame, UNIT_MONSTER, dwNpcId);
	hInteract = SUnitGetInteractUnit(ptGame, hPlayer);
	if (!hNpc || hInteract != hNpc)
		return;
	fHurt = FALSE;
	dwMax = D2CommonStatsUnitCalcMaxHp(hPlayer);
	if ((DWORD)D2CommonStatsUnitGetValue(hPlayer, STAT_LIFE) != dwMax)
		fHurt = TRUE;
	nPrice = (int)D2CommonUnitGetCombinedStatDelta(hPlayer);
	if (!nPrice)
		nPrice = 1;
	dwMax = D2CommonStatsUnitCalcMaxMana(hPlayer);
	if ((DWORD)D2CommonStatsUnitGetValue(hPlayer, STAT_MANA) != dwMax)
		fHurt = TRUE;
	dwMax = D2CommonStatsUnitCalcMaxStamina(hPlayer);
	if ((DWORD)D2CommonStatsUnitGetValue(hPlayer, STAT_STAMINA) == dwMax && !fHurt)
	{
		NPCSENDRESULT(hPlayer, NPCRESULT_NOHEAL, (DWORD)-1, 0);
		return;
	}
	if (D2CommonStatsUnitGetValue(hPlayer, STAT_GOLDBANK) + (DWORD)D2CommonStatsUnitGetValue(hPlayer, STAT_GOLD) >= (DWORD)nPrice)
	{
		nGold = D2CommonStatsUnitGetValue(hPlayer, STAT_GOLD);
		fPaid = TRUE;
		if (D2CommonStatsUnitGetValue(hPlayer, STAT_GOLDBANK) + nGold >= nPrice)
		{
			if (nPrice <= nGold)
				D2CommonStatsUnitAddValue(hPlayer, STAT_GOLD, -nPrice);
			else
			{
				D2CommonStatsUnitAddValue(hPlayer, STAT_GOLD, -nGold);
				D2CommonStatsUnitAddValue(hPlayer, STAT_GOLDBANK, nGold - nPrice);
			}
		}
		else
		{
			NPCSENDRESULT(hPlayer, NPCRESULT_REFUSED, (DWORD)-1, 0);
			fPaid = FALSE;
		}
		if (fPaid)
		{
			SUnitNpcHeal(ptGame, hPlayer, hNpc);
			NPCSENDRESULT(hPlayer, NPCRESULT_HEALED, (DWORD)-1, 0);
		}
	}
	else
		NPCSENDRESULT(hPlayer, NPCRESULT_NOGOLD, (DWORD)-1, 0);
}

void __fastcall SUnitNpcLeave(GAME *ptGame, UNIT *hUnit, UNIT *hNpc)
{
	SUnitResetInteract(hUnit);
	MonsterAIRemoveThreat(ptGame, hUnit, ((MONSTERDATA *)hNpc->uTypeData)->pptThreats);
}

void __fastcall SUnitNpcGiveHire(GAME *ptGame, UNIT *hUnit, DWORD dwNpcClass)
{
	NPCHIREGIVENPACKET tPacket;
	NPCPROXY *pInv;
	MONPETDESC *ptHires;
	UNIT *hHire;
	int nFirst;
	int nLast;
	int nHireClass;
	int nId;
	int nIndex;
	int i;

	pInv = SUnitProxyFindNpc(ptGame, dwNpcClass, &nIndex);
	if (!pInv)
		return;
	ptHires = pInv->ptHires;
	if (!ptHires)
		return;
	if (dwNpcClass == NPCCLASS_HIRE2)
	{
		nLast = HIREID_2_LAST;
		nFirst = HIREID_2_FIRST;
		nHireClass = HIRECLASS_2;
	}
	else if (dwNpcClass == NPCCLASS_HIRE3)
	{
		nLast = HIREID_3_LAST;
		nFirst = HIREID_3_FIRST;
		nHireClass = HIRECLASS_3;
	}
	else
	{
		nLast = HIREID_1_LAST;
		nFirst = HIREID_1_FIRST;
		nHireClass = HIRECLASS_1;
	}
	for (nId = nFirst; nId <= nLast; nId++)
	{
		i = nId - nFirst;
		if (i >= NPCHIRE_COUNT)
			break;
		if (ptHires[i].dwHired == 1 || ptHires[i].dwOffered != 1)
			continue;
		tPacket.bCmd = SCMD_HIREGIVEN;
		tPacket.wKind = 2;
		tPacket.wId = ptHires[i].wHireId;
		ptHires[i].dwHired = 1;
		SCmdSendQuestCommand(SUnitGetClient(hUnit), (const struct SCMDCOPY15 *)&tPacket);
		hHire = MonsterSpawnNear(ptGame, hUnit, nHireClass, 1, 4, 0);
		if (!hHire)
		{
			hHire = MonsterSpawnNear(ptGame, hUnit, nHireClass, 1, 6, 0);
			if (!hHire)
			{
				hHire = MonsterSpawnNear(ptGame, hUnit, nHireClass, 1, 12, 0);
				if (!hHire)
					return;
			}
		}
		MonsterAIPetInit(ptGame, hUnit, hHire, &ptHires[i]);
		break;
	}
	SUnitNpcRenewHires(ptGame, dwNpcClass, pInv);
}

BOOL __fastcall SUnitNpcCanTakeItem(GAME *ptGame, UNIT *hPlayer, UNIT *hNpc, UNIT *hItem, BOOL fOwn
)
{
	NPCPROXY *pInv;
	NPCVISIT *ptVisit;
	NPCPLAYERINV *ptOwn;
	void *hNode;
	DWORD dwUnitId;
	int nIndex;

	pInv = SUnitProxyFindNpcOf(ptGame, hNpc, &nIndex);
	if (pInv)
	{
		ptVisit = SUnitProxyAddVisit(ptGame, pInv, hPlayer);
		dwUnitId = hPlayer->dwId;
		if (ptVisit && ptVisit->bOwnStock)
		{
			if (fOwn)
				return TRUE;
			for (ptOwn = SUnitProxyFindNpcOf(ptGame, hNpc, (int *)&fOwn)->ptPlayerInvs; ptOwn; ptOwn = ptOwn->ptNext)
			{
				if (ptOwn->dwUnitId == dwUnitId)
				{
					for (hNode = D2CommonINVENTORYGetCategoryListHead(ptOwn->ptInventory); hNode;
						hNode = (void *)D2CommonINVENTORYNodeGetNext(hNode))
					{
						if ((UNIT *)D2CommonINVENTORYNodeGetKeyA(hNode) == hItem)
							return TRUE;
					}
					return FALSE;
				}
			}
		}
		else if (D2CommonITEMSDataTestFlags(hItem, 16))
			return TRUE;
		return FALSE;
	}
	return TRUE;
}
