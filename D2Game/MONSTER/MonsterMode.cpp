#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\MONSTER\\MonsterMode.cpp"
#include <stddef.h>
#include "../D2game.h"
#include "../AI/AiThink.h"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "../GAME/Event.h"
#include "../GAME/SCmd.h"
#include "../GAME/Targets.h"
#include "../ITEMS/ItemMode.h"
#include "../ITEMS/Items.h"
#include "Monster.h"
#include "MonsterAI.h"
#include "MonsterEval.h"
#include "MonsterMsg.h"
#include "MonsterUnique.h"
#include "MonsterMode.h"
#include "../PLAYER/Player.h"
#include "../PLAYER/PlrModes.h"
#include "../QUESTS/QUESTS.h"
#include "../QUESTS/a2q2.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "../UNIT/SUnitMsg.h"
#include "../UNIT/sunitproxy.h"
#include "../UNIT/SUNITNPC.h"
#include "../AI/AiGeneral.h"
#include "../GUILDS/Guilds.h"
#include "../SKILLS/SkillMonst.h"
#include "../SKILLS/Skills.h"
#include "../SKILLS/SkillSor.h"
#include "FogImports.h"

#define UNIT_MONSTER			1
#define NUM_MONSTERMODES		16
#define NUM_DIFFICULTY_LEVELS	3
#define MMODE_DEATH				0
#define MMODE_ATTACK2			5
#define MONSTERCLASS_IRONGOLEM	291
#define MONSTERCLASS_UNDEADHORROR3	312
#define UNIT_STATE_UNINTERRUPTABLE	54
#define EVENT_RUN_AI			2
#define MAX_CHOOSE_LIST			512
#define ITEM_QUALITY_RUNE		544106866
#define ITEMCODE_NONE			544766072
#define StatsGetState(hUnit, nState) \
	D2CommonStatsGetStateRaw((DWORD)(hUnit), (DWORD)(nState))
#define MonsterUtilTestModeMove(hUnit, eMode) \
	D2CommonMonsterUtilTestModeMove(ptGame, hUnit, eMode)
#define UnitGetClass(hUnit)		((hUnit)->dwClassId)

typedef BOOL (__fastcall *PFNMONMODESTART)(GAME *ptGame, MONMODECHANGE *ptReq);
typedef int (__fastcall *PFNMONMODEDO)(GAME *ptGame, UNIT *hUnit);
typedef void (__fastcall *PFNMONMODEEND)(GAME *ptGame, UNIT *hUnit);
struct MONMODEENTRY
{
 	PFNMONMODESTART	pfnStart;
 	PFNMONMODEDO	pfnDo;
 	PFNMONMODEEND	pfnEnd;
 	BOOL		fFrameEvents;
};
typedef struct MONMODEENTRY MONMODEENTRY;

struct MONMODECHART
{
 	BOOL		bAlways;
 	BOOL		bNever;
 	DWORD		dwColumn;
};
typedef struct MONMODECHART MONMODECHART;

struct MONBOSSDROP
{
 	int			nClass;
 	int			nTreasureClass;
 	int			nQuality;
 	int			nQuest;
 	int			nQuestFlag;
 	int			nQualityQuest;
};
typedef struct MONBOSSDROP MONBOSSDROP;

struct MONITEMPERCENTTXT
{
 	BYTE		HeartPercent;
 	BYTE		BodyPartPercent;
 	BYTE		TreasureClassPercent;
 	BYTE		ComponentPercent;
};
typedef struct MONITEMPERCENTTXT MONITEMPERCENTTXT;

struct MONCHOOSEITEM
{
 	int			nIndex;
 	DWORD		dwCode;
};
typedef struct MONCHOOSEITEM MONCHOOSEITEM;

typedef void (__fastcall *PFNGAMEUNIT)(GAME *ptGame, UNIT *hUnit);

static const MONMODECHART sgatMonModeChart[NUM_MONSTERMODES] =
{
	{ FALSE, TRUE, 0 },
	{ FALSE, TRUE, 0 },
	{ TRUE, FALSE, 0 },
	{ FALSE, TRUE, 0 },
	{ FALSE, FALSE, offsetof(MONSTATSTXT, A1Move) },
	{ FALSE, FALSE, offsetof(MONSTATSTXT, A2Move) },
	{ FALSE, TRUE, 0 },
	{ FALSE, FALSE, offsetof(MONSTATSTXT, bCMove) },
	{ FALSE, FALSE, offsetof(MONSTATSTXT, S1Move) },
	{ FALSE, FALSE, offsetof(MONSTATSTXT, S2Move) },
	{ FALSE, FALSE, offsetof(MONSTATSTXT, S3Move) },
	{ FALSE, FALSE, offsetof(MONSTATSTXT, S4Move) },
	{ FALSE, TRUE, 0 },
	{ TRUE, FALSE, 0 },
	{ TRUE, FALSE, 0 },
	{ TRUE, FALSE, 0 },
};

static int sganChampionTC[12] =
{
	36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
};
static int PLACEHOLDER_UnreferencedData_100aae80 = sizeof(sganChampionTC) / sizeof(sganChampionTC[0]);

static MONBOSSDROP sgatBossDrops[7] =
{
	{ 156, 58, 4, 6, 6, 4 },
	{ 267, 59, 3, 2, 2, 4 },
	{ 211, 61, 4, 13, 14, 4 },
	{ 250, 60, 4, 12, 13, 4 },
	{ 242, 62, 4, 20, 22, 4 },
	{ 256, 63, 4, 22, 25, 4 },
	{ 243, 64, 4, 23, 26, 4 },
};
static DWORD sgdwBossDrops = sizeof(sgatBossDrops) / sizeof(DWORD);

static int sganRareTC[5] =
{
	65, 66, 67, 68, 69,
};
static int PLACEHOLDER_UnreferencedData_100aaf48 = sizeof(sganRareTC) / sizeof(sganRareTC[0]);

static DWORD sgadwNoComponent[4] =
{
	544500076, 543450477, 544831080, 543712116,
};
static DWORD sgdwNoComponent = sizeof(sgadwNoComponent) / sizeof(DWORD);

static BYTE sgabEndToNeutral[NUM_MONSTERMODES] =
{
	0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
};

static BOOL sgbMonModesOff;

void __fastcall MonsterModeCriticalHit(UNIT *hUnit, UNIT *hTarget,
	DAMAGE *ptDamage)
{
	if (hUnit->eType != UNIT_MONSTER)
		return;
	if ((int)D2CommonSeedRand(&hUnit->tSeed, 100) >= 5)
		return;
	ptDamage->nPhysicalDamage <<= 1;
	ptDamage->nFireDamage <<= 1;
	ptDamage->nLightningDamage <<= 1;
	ptDamage->nMagicDamage <<= 1;
	ptDamage->nColdDamage <<= 1;
	ptDamage->nPoisonDamage <<= 1;
	if (hTarget && !SUnitDmgSetFlag(ptDamage, 16))
		D2CommonUnitAddAnimOverlay(hTarget, UNIT_STATE_UNINTERRUPTABLE, 0);
}

void __fastcall MonsterModeClearOverride(UNIT *hUnit)
{
	MONMODEDATA *ptModeData;

	ptModeData = ((MONSTERDATA *)(hUnit->uTypeData))->ptModeData;
	ptModeData->dwPathTypeNext = 0;
	ptModeData->nVelocityNext = 0;
	ptModeData->bDistMaxNext = 0;
}

void __fastcall MonsterModeSetOverride(MONMODEDATA *ptModeData,
	DWORD dwPathType, DWORD dwVelocity, BYTE bDistMax)
{
	if (dwPathType)
		ptModeData->dwPathTypeNext = dwPathType;
	if (dwVelocity)
		ptModeData->nVelocityNext = dwVelocity;
	if (bDistMax)
	{
		if (bDistMax >= 100)
			bDistMax = 99;
		ptModeData->bDistMaxNext = bDistMax;
	}
}

MONMODEDATA * __fastcall MonsterModeCreate(GAME *ptGame)
{
	MONMODEDATA *ptModeData;
	int i;

#line 310
	ptModeData = (MONMODEDATA *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(MONMODEDATA));
	for (i = 0; i < (int)(sizeof(MONMODEDATA) / sizeof(DWORD)); i++)
		((DWORD *)ptModeData)[i] = 0;
	return ptModeData;
}

void __fastcall MonsterModeFree(GAME *ptGame, MONMODEDATA *ptModeData)
{
#line 318
	D2FREEPOOL(ptGame->pMemPool, ptModeData);
}

static inline void MonsterModeDropRare(GAME *ptGame, UNIT *hUnit, BOOL bChampion,
	MONMODECHANGE *ptReq, BOOL *pbDropped)
{
	GAMEROOM *hRoom;
	BYTE bAct;
	void *ptMonsterStats;
	void *ptTreasureClass;
	DWORD dwCode;
	int nQuality;
	int nTreasureClass;
	UNIT *hItem;
	void (__fastcall *pfnAdjust)(MONMODECHANGE *, int *, DWORD);
	void (__fastcall *pfnGoldFind)(UNIT *, MONMODECHANGE *);

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
#line 748
	D2ASSERT(hRoom);
	bAct = D2CommonDrlgGetActNum(0, D2CommonDungeonRoomGetLevelId(hRoom));
	ptMonsterStats = (void *)D2CommonMonsterTblsGetStats(hUnit->dwClassId);
#line 752
	D2ASSERT(ptMonsterStats);
	if (ptGame->bDifficultyLevel)
		nTreasureClass = 69;
	else if (bAct < 4)
		nTreasureClass = sganRareTC[bAct];
	else
		nTreasureClass = sganRareTC[4];
	ptTreasureClass = (void *)D2CommonMonsterTblsGetTreasureClass(nTreasureClass);
#line 763
	D2ASSERT(ptTreasureClass);
	dwCode = ITEMSModeDrawCode(ptGame, (struct ITEMCODELIST *)ptTreasureClass, hUnit, bChampion);
#line 767
	D2ASSERT(dwCode);
	nQuality = 0;
	if (bChampion)
	{
		if (bAct > 2 && !(D2CommonSeedNext(&hUnit->tSeed) & 7))
			nQuality = 6;
		else
			nQuality = 4;
	}
	if (!dwCode)
		return;
	if (!bChampion)
	{
		ITEMSTXT *ptItemsTxt;
		DWORD dwIndex;

		ptItemsTxt = (ITEMSTXT *)D2CommonItemTblsFindItemByCode(dwCode, &dwIndex);
		if (D2CommonStatsUnitGetValue(hUnit, 12) + 3 < ptItemsTxt->level)
			return;
	}
	pfnAdjust = MonsterModeAdjustQuality;
	pfnAdjust(ptReq, &nQuality, dwCode);
	hItem = MonsterModeCreateItem(ptGame, hUnit, dwCode, nQuality);
	if (!hItem)
		return;
	pfnGoldFind = MonsterModeGoldFind;
	pfnGoldFind(hItem, ptReq);
	if (!bChampion)
		return;
	if (!D2CommonITEMSTestModTypeMagicOrAbove(hItem))
		MonsterModeDropMagic(ptGame, hUnit, ptReq, nQuality);
	*pbDropped = TRUE;
}

extern BOOL MonsterModeIsNoComponent(DWORD dwCode);
__inline BOOL MonsterModeIsNoComponent(DWORD dwCode)
{
	DWORD j;

	for (j = 0; j < sgdwNoComponent; j++)
	{
		if (dwCode == sgadwNoComponent[j])
			return TRUE;
	}
	return FALSE;
}

static inline void MonsterModeDropComponents(GAME *ptGame, UNIT *hUnit, MONMODECHANGE *ptReq,
	BOOL *pbDropped)
{
	GAMEROOM *hRoom;
	MONSTATSTXT *ptMonsterStats;
	MONITEMPERCENTTXT *ptPercent;
	DWORD nPlayers;
	DWORD dwCode;
	DWORD dwChance;
	DWORD i;
	BYTE bComponent;
	int nQuality;
	UNIT *hItem;
	void (__fastcall *pfnGoldFind)(UNIT *, MONMODECHANGE *);

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
#line 830
	D2ASSERT(hRoom);
	nPlayers = (DWORD)PlayerCountAlive(ptGame);
	ptMonsterStats = ((MONSTATSTXT *)(D2CommonMonsterTblsGetStats(hUnit->dwClassId)));
#line 834
	D2ASSERT(ptMonsterStats);
	ptPercent = (MONITEMPERCENTTXT *)D2CommonMonsterTblsGetMonItemPercent(0);
	if (ptMonsterStats->bSpawnComponents != 1 || ptPercent->ComponentPercent <= 0)
		return;
	for (i = 0; i < 16; i++)
	{
		if (*pbDropped)
			return;
		bComponent = hUnit->abComponentVariations[(BYTE)i];
		dwCode = D2CommonMonsterRandGetComponentCode(hUnit, i, bComponent);
		if (!dwCode)
			continue;
		if (MonsterModeIsNoComponent(dwCode))
			continue;
		dwChance = ptPercent->ComponentPercent * nPlayers;
		if (dwChance > 90)
			dwChance = 90;
		nQuality = 0;
		if (D2CommonSeedRand(&hUnit->tSeed, 100) >= dwChance)
			continue;
		dwCode = MonsterModeChooseItem(hUnit, dwCode);
		if (!dwCode)
			continue;
		MonsterModeAdjustQuality(ptReq, &nQuality, dwCode);
		hItem = MonsterModeCreateItem(ptGame, hUnit, dwCode, nQuality);
		if (!hItem)
			continue;
		pfnGoldFind = MonsterModeGoldFind;
		pfnGoldFind(hItem, ptReq);
		if (D2CommonITEMSTestModTypeMagicOrAbove(hItem))
			*pbDropped = TRUE;
	}
}

static __inline DWORD MonsterModeGetBossIndex(int nClass)
{
	DWORD i;

	for (i = 0; i < sgdwBossDrops; i++)
	{
		if (nClass == sgatBossDrops[i].nClass)
			return i;
	}
#line 896
	D2FATAL("Boss is not in table!");
	return 0;
}

static inline void MonsterModeDropUnique(GAME *ptGame, UNIT *hUnit, MONMODECHANGE *ptReq)
{
	struct MONSTERUNIQUE *ptUnique;
	DWORD dwExtra;
	DWORD dwCode;

	ptUnique = ((MONSTERDATA *)(hUnit->uTypeData))->ptUnique;
	dwExtra = 0;
	if (ptUnique && (D2CommonMonsterRegionUnitIsBossRaw(0, (DWORD)hUnit) || MonsterUniqueIsChampion(ptUnique)
		|| MonsterUniqueIsCounted(ptUnique)))
		dwExtra = 1;
	dwCode = ITEMSGetHealingPotion(hUnit);
#line 930
	D2ASSERT(dwCode);
	for (; dwExtra > 0; dwExtra--)
		MonsterModeCreateItem(ptGame, hUnit, dwCode, 2);
	if (D2CommonMonsterRegionUnitIsBossRaw(0, (DWORD)hUnit))
		MonsterModeDropBoss(ptGame, hUnit, ptReq, (int)MonsterModeGetBossIndex(hUnit->dwClassId));
}

void __fastcall MonsterModeDropItems(GAME *ptGame, MONMODECHANGE *ptReq, DWORD dwUnused)
{
	UNIT *hUnit;
	GAMEROOM *hRoom;
	MONSTATSTXT *ptMonsterStats;
	MONITEMPERCENTTXT *ptPercent;
	struct MONSTERUNIQUE *ptUnique;
	DWORD dwCode;
	int nPlayers;
	BOOL bChampion;
	BOOL bDropped;

	int nChance;
	DWORD dwChance;
	UNIT *hItem;

	hUnit = ptReq->hUnit;
#line 944
	D2ASSERT(hUnit);
	if (hUnit->dwFlags0 & 131072)
		return;
	if (D2CommonCollisnTestPoint(D2CommonUnitGetPathRoom(hUnit), D2CommonUnitGetX(hUnit),
		D2CommonUnitGetY(hUnit), 2049))
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
#line 956
	D2ASSERT(hRoom);
	nPlayers = PlayerCountAlive(ptGame);
	bDropped = FALSE;
	ptMonsterStats = ((MONSTATSTXT *)(D2CommonMonsterTblsGetStats(hUnit->dwClassId)));
#line 963
	D2ASSERT(ptMonsterStats);
	bChampion = FALSE;
	ptUnique = ((MONSTERDATA *)(hUnit->uTypeData))->ptUnique;
	if (ptUnique)
	{
		if (((MONSTATSTXT *)(D2CommonMonsterTblsGetStats(hUnit->dwClassId)))->bIsNPC
			|| MonsterUniqueIsCounted(ptUnique))
			bChampion = TRUE;
		if (MonsterUniqueIsChampion(ptUnique))
		{
			if (D2CommonSeedRand(&hUnit->tSeed, 100) < 40)
				bChampion = TRUE;
			else
			{
				struct ITEMCODELIST *ptTreasureClass;

				bChampion = FALSE;
				ptTreasureClass = (struct ITEMCODELIST *)D2CommonMonsterTblsGetTreasureClass(1);
				dwCode = ITEMSModeDrawCode(ptGame, ptTreasureClass, hUnit, FALSE);
				if (dwCode)
				{ int nQuality;
					nQuality = 0;
					MonsterModeAdjustQuality(ptReq, &nQuality, dwCode);
					hItem = MonsterModeCreateItem(ptGame, hUnit, dwCode, nQuality);
					if (hItem)
					{
						D2CommonStatsUnitSetValue(hItem, 14, (DWORD)D2CommonStatsUnitGetValue(hItem,
							14) * 5);
						MonsterModeGoldFind(hItem, ptReq);
					}
				}
			}
		}
	}
	if (ptMonsterStats->bSpawnComponents == 1)
		ptPercent = (MONITEMPERCENTTXT *)D2CommonMonsterTblsGetMonItemPercent(0);
	else
		ptPercent = (MONITEMPERCENTTXT *)D2CommonMonsterTblsGetMonItemPercent(1);
#line 1006
	D2ASSERT(ptPercent);
	MonsterModeDropComponents(ptGame, hUnit, ptReq, &bDropped);
	if (!bDropped)
	{
		nChance = ptPercent->TreasureClassPercent * nPlayers + ptMonsterStats->bSpawnPctBonus;
		if (nChance >= 90)
			nChance = 90;
		if (bChampion || D2CommonSeedRand(&hUnit->tSeed, 100) < (DWORD)nChance)
		{
			if (D2CommonSeedRand(&hUnit->tSeed, 100) < 5)
				MonsterModeDropRare(ptGame, hUnit, bChampion, ptReq, &bDropped);
			else
				MonsterModeDropTreasure(ptGame, hUnit, bChampion, ptReq, &bDropped);
		}
	}
	dwChance = ptPercent->HeartPercent * (DWORD)nPlayers;
	if (dwChance > 90)
		dwChance = 90;
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < dwChance
		&& ptMonsterStats->Heart != ITEMCODE_NONE)
		MonsterModeCreateItem(ptGame, hUnit, ptMonsterStats->Heart, 2);
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < dwChance
		&& ptMonsterStats->BodyPart != ITEMCODE_NONE)
		MonsterModeCreateItem(ptGame, hUnit, ptMonsterStats->BodyPart, 2);
	MonsterModeDropUnique(ptGame, hUnit, ptReq);
}

UNIT * __fastcall MonsterModeCreateItem(GAME *ptGame, UNIT *hUnit, DWORD dwCode, int nQuality)
{
	int nClass;
	COLPOINT tFrom;
	COLPOINT tTo;
	GAMEROOM *hRoom;
	ITEMCREATE tCreate;
	int i;

	nClass = D2CommonItemTblsFindItemIndexByCode(dwCode);
	if (!(nClass == -1)) {
		D2CommonUnitGetPosition(hUnit, &tFrom);
		hRoom = ITEMSFindDropSpot((GAMEROOM *)D2CommonUnitGetPathRoom(hUnit), &tFrom, &tTo, 1);
		if (!hRoom)
			return NULL;
		for (i = 0; i < (int)(sizeof(ITEMCREATE) / sizeof(DWORD)); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.hSpawner = hUnit;
		tCreate.ptGame = ptGame;
		tCreate.dwClassId = nClass;
		tCreate.dwMode = 3;
		tCreate.hRoom = hRoom;
		tCreate.ptSeed = NULL;
		tCreate.nX = tTo.nX;
		tCreate.nY = tTo.nY;
		tCreate.dwQuality = nQuality;
		tCreate.wCreateFlags = 1;
		tCreate.nLevel = ITEMSGetUnitLevel(hUnit, LEVEL_ID_NONE);
		return ITEMSCreateFromRecord(ptGame, &tCreate, FALSE); }
	return NULL;
}

void __fastcall MonsterModeAdjustQuality(MONMODECHANGE *ptReq, int *pnQuality, DWORD dwCode)
{
	int nClass;
	int nMagicFind;

	nClass = D2CommonItemTblsFindItemIndexByCode(dwCode);
	if (!ptReq->hTarget)
		return;
	if (*pnQuality == 4 || *pnQuality == 6 || *pnQuality == 7 || *pnQuality == 5)
		return;
	nMagicFind = (int)D2CommonStatsGetTotalValue(ptReq->hTarget, 80);
	if (!nMagicFind)
		return;
	ITEMSRollMagicQuality(ptReq->hUnit, D2CommonStatsUnitGetValue(ptReq->hUnit, 12), nMagicFind,
		nClass,
		(DWORD *)pnQuality);
}

void __fastcall MonsterModeGoldFind(UNIT *hItem, MONMODECHANGE *ptReq)
{
	int nPercent;
	int nGold;

	if (!hItem || D2CommonITEMSGetType(hItem) != 4)
		return;
	nPercent = 100;
	if (ptReq->hTarget)
		nPercent = (int)D2CommonStatsGetTotalValue(ptReq->hTarget, 79) + 100;
	nGold = D2CommonStatsUnitGetValue(hItem, 14);
	D2CommonStatsUnitSetValue(hItem, 14, nGold * nPercent / 100);
}

static inline void MonsterModeChooseAdd(MONCHOOSEITEM *ptList,
	int *pnCount  , int nIndex, DWORD dwCode)
{
#line 476
	D2ASSERT(*pnCount < MAX_CHOOSE_LIST);
	ptList[*pnCount].dwCode = dwCode;
	ptList[*pnCount].nIndex = nIndex;
	(*pnCount)++;
}

static inline DWORD MonsterModeChooseDraw(UNIT *hUnit, MONCHOOSEITEM *ptList,
	int nCount  )
{
	int nFindIndex;

	nFindIndex = (int)D2CommonSeedRand(&hUnit->tSeed, nCount);
#line 492
	D2ASSERT(nFindIndex < nCount);
	return ptList[nFindIndex].dwCode;
}

DWORD __fastcall MonsterModeChooseItem(UNIT *hUnit, DWORD dwCode)
{
	DWORD dwIndex;
	ITEMSTXT *ptItemsTxt;
	DWORD dwLevel;
	DWORD dwType;
	ITEMSTATSDATATBLDESC *ptDataTblDesc;
	ITEMSTXT *ptFirst;
	ITEMSTXT *ptStat;
	int nFirst;
	int nRecords;
	int nCount;
	int i;
	MONCHOOSEITEM atList[MAX_CHOOSE_LIST];

	ptItemsTxt = (ITEMSTXT *)D2CommonItemTblsFindItemByCode(dwCode, &dwIndex);
	if (ptItemsTxt)
	{
		dwLevel = (DWORD)ITEMSGetUnitLevel(hUnit, LEVEL_ID_NONE);
		if (ptItemsTxt->level <= dwLevel + 2)
			return dwCode;
		dwType = D2CommonITEMSGetTypeNoCheck(dwIndex);
		ptDataTblDesc = (ITEMSTATSDATATBLDESC *)D2CommonItemTblsGetItemStatsDesc();
#line 515
		D2ASSERT(ptDataTblDesc);
		nRecords = 0;
		nFirst = 0;
		ptFirst = NULL;
		if (D2CommonITEMSIsArmorNoCheck(dwIndex))
		{
			ptFirst = ptDataTblDesc->ptArmorStart;
			nFirst = ((BYTE *)ptDataTblDesc->ptArmorStart - (BYTE *)ptDataTblDesc->ptItemStats) >> 9;
			ptFirst = ptDataTblDesc->ptArmorStart;
			nRecords = ((BYTE *)ptDataTblDesc->ptMiscStart - (BYTE *)ptDataTblDesc->ptArmorStart) >> 9;
		}
		else if (D2CommonITEMSIsWeaponNoCheck(dwIndex))
		{
			ptFirst = ptDataTblDesc->ptWeaponsStart;
			nFirst = ((BYTE *)ptDataTblDesc->ptWeaponsStart - (BYTE *)ptDataTblDesc->ptItemStats) >> 9;
			ptFirst = ptDataTblDesc->ptWeaponsStart;
			nRecords = ((BYTE *)ptDataTblDesc->ptArmorStart - (BYTE *)ptDataTblDesc->ptWeaponsStart) >> 9;
		}
		else if (D2CommonITEMSIsMiscellaneousNoCheck(dwIndex))
		{
			nFirst = ((BYTE *)ptDataTblDesc->ptMiscStart - (BYTE *)ptDataTblDesc->ptItemStats) >> 9;
			nRecords = ((ptDataTblDesc->nSize << 9) - ((BYTE *)ptDataTblDesc->ptMiscStart - (BYTE *)ptDataTblDesc->ptItemStats)) >> 9;
			ptFirst = ptDataTblDesc->ptMiscStart;
		}
		nCount = 0;
		for (i = 0; i < nRecords; i++)
		{
			ptStat = ptFirst + i;
			D2_OPAQUE(ptStat);
#line 536
			D2ASSERT(ptStat);
			if (ptStat->type == (BYTE)dwType && ptStat->level <= dwLevel && ptStat->spawnable && !ptStat->quest)
				MonsterModeChooseAdd(atList, &nCount, nFirst + i, ptStat->code); }
		if (nCount <= 0)
			return 0;
		return MonsterModeChooseDraw(hUnit, atList, nCount); }
	return 0;
}

void __fastcall MonsterModeDropMagic(GAME *ptGame, UNIT *hUnit, MONMODECHANGE *ptReq,
	int nQuality)
{
	GAMEROOM *hRoom;
	BYTE bAct;
	void *ptMonsterStats;
	void *ptTreasureClass;
	DWORD dwCode;
	DWORD dwTries;
	BOOL bDone;
	int nIdx;
	UNIT *hItem;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
#line 560
	D2ASSERT(hRoom);
	bAct = D2CommonDrlgGetActNum(0, D2CommonDungeonRoomGetLevelId(hRoom));
	ptMonsterStats = (void *)D2CommonMonsterTblsGetStats(hUnit->dwClassId);
#line 564
	D2ASSERT(ptMonsterStats);
	dwTries = 0;
	bDone = FALSE;
	while (!bDone)
	{
		nIdx = (int)D2CommonSeedRand(&hUnit->tSeed, 3);
		ptTreasureClass = (void *)D2CommonMonsterTblsGetTreasureClass(sganChampionTC[bAct * 3 + nIdx]);
#line 572
		D2ASSERT(ptTreasureClass);
		dwCode = ITEMSModeDrawCode(ptGame, (struct ITEMCODELIST *)ptTreasureClass, hUnit, FALSE);
#line 577
		D2ASSERT(dwCode);
		MonsterModeAdjustQuality(ptReq, &nQuality, dwCode);
		hItem = MonsterModeCreateItem(ptGame, hUnit, dwCode, nQuality);
		if (hItem)
		{
			MonsterModeGoldFind(hItem, ptReq);
			if (D2CommonITEMSTestModTypeMagicOrAbove(hItem))
				bDone = TRUE;
			else
				ITEMSFree(ptGame, hItem);
		}
		dwTries++;
		if (dwTries > 250)
		{
#line 592
			D2WARN("We tried 250 times to create a magic item!\nPlease tell Jon Morin");
			return;
		}
	}
}

void __fastcall MonsterModeDropBoss(GAME *ptGame, UNIT *hUnit, MONMODECHANGE *ptReq,
	int nBoss)
{
	BOOL bQuest;
	DWORD dwTries;
	BOOL bDone;
	void *ptTreasureClass;
	DWORD dwCode;
	int nQuality;
	UNIT *hPlayer;
	UNIT *hTarget;
	DWORD dwOwnerType;
	DWORD dwOwnerId;
	void *ptQuest;
	UNIT *hItem;

	bQuest = QuestsTestQuest(ptGame, sgatBossDrops[nBoss].nQuest);
	dwTries = 0;
	bDone = FALSE;
	while (!bDone)
	{
		ptTreasureClass = (void *)D2CommonMonsterTblsGetTreasureClass(sgatBossDrops[nBoss].nTreasureClass);
#line 609
		D2ASSERT(ptTreasureClass);
		dwCode = ITEMSModeDrawCode(ptGame, (struct ITEMCODELIST *)ptTreasureClass, hUnit, TRUE);
#line 613
		D2ASSERT(dwCode);
		nQuality = sgatBossDrops[nBoss].nQuality;
		hPlayer = NULL;
		hTarget = ptReq->hTarget;
		if (hTarget)
		{
			if (hTarget->eType == UNIT_PLAYER)
				hPlayer = hTarget;
			else if (ptReq->hTarget && ptReq->hTarget->eType == UNIT_MONSTER)
			{
				AiGetOwnerId(hTarget, &dwOwnerId, &dwOwnerType);
				if (dwOwnerType == UNIT_PLAYER)
					hPlayer = SUnitFind(ptGame, UNIT_PLAYER, dwOwnerId);
			}
		}
		ptQuest = NULL;
		if (hPlayer)
			ptQuest = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
		if (bQuest && ptQuest)
		{
			if (!D2CommonStepTestGridBit(ptQuest, sgatBossDrops[nBoss].nQuestFlag, 0)
				&& !D2CommonStepTestGridBit(ptQuest, sgatBossDrops[nBoss].nQuestFlag, 1))
				nQuality = sgatBossDrops[nBoss].nQualityQuest;
		}
		MonsterModeAdjustQuality(ptReq, &nQuality, dwCode);
		hItem = MonsterModeCreateItem(ptGame, hUnit, dwCode, nQuality);
		if (hItem)
		{
			MonsterModeGoldFind(hItem, ptReq);
			if ((int)D2CommonITEMSDataGetModType(hItem) == nQuality)
				bDone = TRUE;
			else
				ITEMSFree(ptGame, hItem);
		}
		dwTries++;
		if (dwTries > 250)
		{
#line 661
			D2WARN("We tried 250 times to create a magic item!\nPlease tell Jon Morin");
			return;
		}
	}
}

void __fastcall MonsterModeDropTreasure(GAME *ptGame, UNIT *hUnit, BOOL bChampion,
	MONMODECHANGE *ptReq, BOOL *pbDropped)
{
	GAMEROOM *hRoom;
	BYTE bAct;
	MONSTATSTXT *ptMonsterStats;
	int nDifficulty;
	int nCount;
	int nIndex;
	int nTreasureClass;
	void *ptTreasureClass;
	DWORD dwCode;
	int nQuality;
	UNIT *hItem;
	DWORD dwTmp;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
#line 672
	D2ASSERT(hRoom);
	bAct = D2CommonDrlgGetActNum(0, D2CommonDungeonRoomGetLevelId(hRoom));
	ptMonsterStats = ((MONSTATSTXT *)(D2CommonMonsterTblsGetStats(hUnit->dwClassId)));
#line 676
	D2ASSERT(ptMonsterStats);
	nDifficulty = ptGame->bDifficultyLevel;
#line 679
	D2ASSERT(ptGame->bDifficultyLevel < NUM_DIFFICULTY_LEVELS);
	for (nCount = 0; nCount < 4; nCount++)
	{
		if (!ptMonsterStats->TreasureClass[nDifficulty][nCount])
			break;
	}
	if (!nCount)
		return;
	nIndex = (int)D2CommonSeedRand(&hUnit->tSeed, nCount);
	nTreasureClass = ptMonsterStats->TreasureClass[nDifficulty][nIndex];
	if (bChampion)
	{
		nIndex = (int)D2CommonSeedRand(&hUnit->tSeed, 3);
		nTreasureClass = sganChampionTC[bAct * 3 + nIndex];
	}
	ptTreasureClass = (void *)D2CommonMonsterTblsGetTreasureClass(nTreasureClass);
#line 695
	D2ASSERT(ptTreasureClass);
	dwTmp = ITEMSModeDrawCode(ptGame, (struct ITEMCODELIST *)ptTreasureClass, hUnit, bChampion);
	if (!dwTmp)
	{
#line 702
		FogErrorManagerLog("ITEMSGetItemInTreasureClass() call at line %d of file %s failed.\n", __LINE__,
			D2_THIS_FILE);
		dwTmp = ITEM_QUALITY_RUNE;
	}
	dwCode = dwTmp;
	nQuality = 0;
	if (bChampion)
	{
		if (bAct > 2 && !(D2CommonSeedNext(&hUnit->tSeed) & 7))
			nQuality = 6;
		else
			nQuality = 4;
	}
	else
		dwCode = MonsterModeChooseItem(hUnit, dwCode);
	if (!dwCode)
		return;
	MonsterModeAdjustQuality(ptReq, &nQuality, dwCode);
	hItem = MonsterModeCreateItem(ptGame, hUnit, dwCode, nQuality);
	if (!hItem)
		return;
	MonsterModeGoldFind(hItem, ptReq);
	if (!bChampion)
		return;
	if (!D2CommonITEMSTestModTypeMagicOrAbove(hItem))
		MonsterModeDropMagic(ptGame, hUnit, ptReq, nQuality);
	*pbDropped = TRUE;
}

void __fastcall MonsterModeRegen(GAME *ptGame, UNIT *hUnit)
{
	int nRegen;
	int nLife;
	int nMaxLife;
	DWORD dwState;
	DWORD dwUnitId;
	UNIT *hKiller;
	int anStates[2];
	int i;

	if (StatsGetState(hUnit, 52))
		return;
	nRegen = D2CommonStatsUnitGetValue(hUnit, 74);
	if (D2CommonStatsUnitTestRegenBaseOnly(hUnit))
		nRegen -= D2CommonStatsUnitGetBaseValue(hUnit, 74);
	if (!nRegen)
		return;
	nLife = D2CommonStatsUnitGetValue(hUnit, 6);
	nMaxLife = D2CommonStatsUnitCalcMaxHp(hUnit);
	nLife += nRegen;
	if (nLife > nMaxLife)
	{
		EventRemoveByType(ptGame, hUnit, 3);
		nLife = nMaxLife;
	}
	if (nLife < 1)
		nLife = 0;
	D2CommonStatsUnitSetValue(hUnit, 6, nLife);
	if (nLife)
		return;
	if (!hUnit->dwMode || hUnit->dwMode == 12)
		return;
	anStates[0] = 2;
	anStates[1] = 62;
	dwState = 0;
	for (i = 0; i < 2 && !dwState; i++)
		dwState = D2CommonStatsFindListByState(hUnit, anStates[i]);
	UNIT5(hUnit)->dwHitClass = 0;
	hKiller = NULL;
	if (dwState)
	{
		dwUnitId = D2CommonStatsGetListSourceId(dwState);
		hKiller = SUnitFind(ptGame, D2CommonStatsGetListSourceType(dwState), dwUnitId);
	}
	if (StatsGetState(hUnit, UNIT_STATE_UNINTERRUPTABLE))
	{
		D2CommonStatsUnitSetActiveStateBit(hUnit, 92, 1);
		return;
	}
	SUnitDmgKill(ptGame, hUnit, hKiller, 1);
}

static __inline void MonsterModeFireQuills(GAME *ptGame, UNIT *hUnit, int nSkill)
{
	void *ptPath;
	UNIT *hTarget;
	int nTargetX;
	int nTargetY;
	int nStepX;
	int nStepY;
	int nCount;
	MONSTATSTXT *ptMonsterStats;
	D2SEED tSeed;

	ptPath = hUnit->ptPath;
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
	{
		nTargetX = (int)D2CommonPathGetTargetXRaw((DWORD)ptPath);
		nTargetY = (int)D2CommonPathGetTargetYRaw((DWORD)ptPath);
	}
	else
	{
		nTargetX = (int)D2CommonUnitGetX(hTarget);
		nTargetY = (int)D2CommonUnitGetY(hTarget);
	}
	nStepX = 5;
	nStepY = 5;
	D2CommonPathSetTarget(ptPath, 0);
	D2CommonSeedInit(&tSeed);
	D2CommonSeedSet(&tSeed, 1397049683);
	tSeed.dwHigh = hUnit->dwId;
	ptMonsterStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId);
	for (nCount = ptMonsterStats->AIParam[2]; nCount > 0; nCount--)
	{
		if (D2CommonSeedNext(&tSeed) & 1)
			nStepX = -nStepX;
		if (D2CommonSeedNext(&tSeed) & 1)
			nStepY = -nStepY;
		D2CommonPathSetTargetCoord(ptPath, nTargetX + nStepX, nTargetY + nStepY);
		SkillsCreateMissile(ptGame, nSkill, hUnit, 1, 0, 0, 0, 0, 0);
	}
	D2CommonPathSetTarget(ptPath, hTarget);
}

void __fastcall MonsterModeUseSkill(GAME *ptGame, UNIT *hUnit, int nSkillArg)
{
	int nSkill;
	int nLevel;

	if (!hUnit)
		return;
	nSkill = (int)D2CommonMonsterRegionGetMissileField(hUnit, nSkillArg);
	if (nSkill == 65535)
		return;
	nLevel = ((MONDIFFICULTYTXT *)D2CommonDataTblsGetDifficultyLevel(
		ptGame->bDifficultyLevel))->MonsterSkillBonus + 1;
	if (hUnit->dwFlags0 & 512)
		nLevel = D2CommonStatsUnitGetValue(hUnit, 12);
	if (!SkillsCreateMissile(ptGame, nSkill, hUnit, nLevel, 0, 0, 0, 0, 1))
		return;
	if (D2CommonMonsterUnitGetTypeLocalIndex(hUnit) != 63)
		return;
	MonsterModeFireQuills(ptGame, hUnit, nSkill);
}

static void MonsterModeSetAtTarget(GAME *ptGame, UNIT *hUnit, int eMode)
{
	MONMODECHANGE tReq;

	MonsterModeInit(hUnit, eMode, &tReq);
	tReq.hTarget = SUnitGetTarget(ptGame, hUnit);
	MonsterModeSet(ptGame, &tReq, TRUE);
}

void __fastcall MonsterModeEndAttack(GAME *ptGame, UNIT *hUnit)
{
	int nMode;
	int nIndex;

	nMode = hUnit->dwMode;
	nIndex = (int)D2CommonMonsterUnitGetTypeLocalIndex(hUnit);
	if (((MONSTATSTXT *)(D2CommonMonsterTblsGetStats(nIndex)))->SplEndGeneric)
	{
		if (nIndex == 110)
		{
			if (nMode == 8)
			{
				MonsterModeProcessEvent(ptGame, hUnit, EVENT_RUN_AI);
				return;
			}
		}
		else if (nIndex == 247)
		{
			if (nMode == 14)
			{
				MonsterModeProcessEvent(ptGame, hUnit, EVENT_RUN_AI);
				return;
			}
		}
		else if (nIndex == 136)
		{
			if (nMode == 10 || nMode == 11)
			{
				MonsterModeProcessEvent(ptGame, hUnit, EVENT_RUN_AI);
				return;
			}
		}
		else if (nIndex == 230 || nIndex == 231)
		{
			MonsterModeProcessEvent(ptGame, hUnit, EVENT_RUN_AI);
			return;
		}
		else if (nIndex == 118)
		{
			if (nMode == 2)
			{
				MonsterModeProcessEvent(ptGame, hUnit, EVENT_RUN_AI);
				return;
			}
		}
		else if (nIndex == 403)
		{
			MonsterModeProcessEvent(ptGame, hUnit, EVENT_RUN_AI);
			return;
		}
	}
	if (sgabEndToNeutral[nMode])
	{
		D2CommonUnitSetMode(hUnit, 1);
		MonsterModeProcessEvent(ptGame, hUnit, EVENT_RUN_AI);
		return;
	}
#line 1379
	D2ASSERT(! StatsGetState (hUnit, UNIT_STATE_UNINTERRUPTABLE));
	if (StatsGetState(hUnit, UNIT_STATE_UNINTERRUPTABLE))
	{
		SUnitSetStateEndLinked(hUnit, 0);
		if (SUnitIsDead(hUnit))
			return;
	}
	MonsterModeSetAtTarget(ptGame, hUnit, 1);
}

void __fastcall MonsterModeEndSpecialA(GAME *ptGame, UNIT *hUnit)
{
	MonsterModeSetAtTarget(ptGame, hUnit, 8);
	MonsterModeProcessEvent(ptGame, hUnit, EVENT_RUN_AI);
}

void __fastcall MonsterModeEndSpecialB(GAME *ptGame, UNIT *hUnit)
{
	MonsterModeSetAtTarget(ptGame, hUnit, 8);
	MonsterModeProcessEvent(ptGame, hUnit, EVENT_RUN_AI);
}

int __fastcall MonsterModeUpdateWait(GAME *ptGame, UNIT *hUnit)
{
	EventRemoveByType(ptGame, hUnit, 0);
	return 1;
}

BOOL __fastcall MonsterModeStartDeath(GAME *ptGame, MONMODECHANGE *ptReq)
{
	UNIT *hUnit;
	int nIndex;
	int nX;
	int nY;
	DWORD dwSource;
	int nLife;
	int nBase;
	int nRand;
	DAMAGE tDamage;
	int i;

	hUnit = ptReq->hUnit;
	SUnitSetMode(ptGame, hUnit, 0);
	MonsterModeKill(ptGame, hUnit, ptReq->bDirection);
	MonsterModeDropItems(ptGame, ptReq, 0);
	MonsterRegionAddAligned((DWORD *)&ptGame->pMonRegions, hUnit);
	D2CommonDungeonRoomRingBufferPush(D2CommonUnitGetPathRoom(hUnit), hUnit->dwId);
	if (!((MONSTATSTXT *)(D2CommonMonsterTblsGetStats(hUnit->dwClassId)))->bDeathDamage)
		return TRUE;
	nIndex = (int)D2CommonMonsterUnitGetTypeLocalIndex(hUnit);
	if (nIndex == 212)
	{
		nX = (int)D2CommonUnitGetX(hUnit);
		nY = (int)D2CommonUnitGetY(hUnit);
		dwSource = (DWORD)SkillsCreateMissile(ptGame, MISSILE_MONSTERCORPSEEXPLODE, hUnit, 1,
			0, 0, nX, nY, 1);
		if (!dwSource)
			return TRUE;
		nLife = D2CommonStatsUnitGetBaseValue(hUnit, 7);
		nBase = nLife * 75 / 100;
		if (nLife - nBase)
			nRand = (int)D2CommonSeedRand(&hUnit->tSeed, nLife - nBase);
		else
			nRand = 0;
		for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
			((DWORD *)&tDamage)[i] = 0;
		tDamage.nPhysicalDamage = (nRand + nBase) >> 3;
		SUnitDmgSplash(ptGame, (UNIT *)dwSource, nX, nY, 4, &tDamage, FALSE, FALSE, NULL, 1409);
		return TRUE;
	}
	if (nIndex == 392 || nIndex == 393)
		MonsterModeDeathBlast(ptGame, hUnit);
	return TRUE;
}

void __fastcall MonsterModeKill(GAME *ptGame, UNIT *hUnit, BYTE bDirection)
{
	MONSTATSTXT *ptMonsterStats;
	void *ptPath;

	if (UNIT5(hUnit)->ptChatNode)
	{
		D2CommonChatNodeFree(ptGame->pMemPool, UNIT5(hUnit)->ptChatNode);
		UNIT5(hUnit)->ptChatNode = NULL;
		D2CommonUnitRoomAddToChangedList(hUnit);
		hUnit->dwFlags0 |= 256;
	}
	AiPassLeadership(hUnit);
	ptMonsterStats = ((MONSTATSTXT *)(D2CommonMonsterTblsGetStats(hUnit->dwClassId)));
	TargetsRemove(ptGame, hUnit);
	hUnit->dwFlags0 &= 0xFFFF7FF3;
	D2CommonStatsUnitFreeLists(hUnit);
	D2CommonStatsUnitCommitPendingFlags(hUnit, D2CommonMonsterRegionUnitIsBossRaw((DWORD)ptMonsterStats,
		(DWORD)hUnit));
	if (D2CommonStatsUnitTestStateGroupB(hUnit))
		hUnit->dwFlags0 &= ~2;
	ptPath = hUnit->ptPath;
	D2CommonMonsterUnitGetTypeLocalIndex(hUnit);
	if (!ptMonsterStats->bDeadCollision)
		D2CommonPathReplaceCollisionMask(hUnit, 1);
	((void (__stdcall *)(DWORD, BYTE))D2CommonPathSetFacingImmediateRaw)((DWORD)ptPath, bDirection);
	MonsterRemoveEvents(ptGame, hUnit);
}

void __fastcall MonsterModeDeathBlast(GAME *ptGame, UNIT *hUnit)
{
	UNIT *hOther;
	DAMAGE tDamage;
	int i;

	for (hOther = ((GAMEROOM *)D2CommonUnitGetPathRoom(hUnit))->ptUnitFirst; hOther;
		hOther = hOther->ptRoomNext)
	{
		if (hOther->eType != UNIT_PLAYER || hOther->dwMode == 17)
			continue;
		if (D2CommonUnitGetProximityMetric(hUnit, hOther) > 2)
			continue;
		if (D2CommonUnitTestLineBlockedIgnoring(hOther, hUnit, 7169))
			continue;
		for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
			((DWORD *)&tDamage)[i] = 0;
		tDamage.nPhysicalDamage = D2CommonStatsUnitGetValue(hOther, 6) >> 5;
		tDamage.wFlags = 1;
		if (!StatsGetState(hOther, UNIT_STATE_UNINTERRUPTABLE))
			tDamage.wFlags |= 4;
		SUnitDmgResist(hUnit, hOther, &tDamage);
		SUnitDmgApply(ptGame, hUnit, hOther, 1, &tDamage);
		SUnitDmgReact(ptGame, hUnit, hOther, &tDamage);
	}
}

void __fastcall MonsterModeEndDeath(GAME *ptGame, UNIT *hUnit)
{
	int nIndex;

#line 1534
	D2ASSERT(hUnit);
	SUnitSetMode(ptGame, hUnit, 12);
	nIndex = (int)D2CommonMonsterUnitGetTypeLocalIndex(hUnit);
	if (((MONSTATSTXT *)(D2CommonMonsterTblsGetStats(nIndex)))->SplEndDeath && nIndex == 278)
	{
		MonsterChangeClass(ptGame, hUnit, hUnit->dwClassId - 137, 1);
		MonsterRestartAi(ptGame, hUnit);
	}
}

int __fastcall MonsterModeUpdateDeath(GAME *ptGame, UNIT *hUnit)
{
#line 1572
	D2WARN(MonsterUtilTestModeMove (hUnit, MMODE_DEATH));
	SUnitStepAnim(ptGame, hUnit);
	D2CommonUnitStepAnimState(hUnit);
	if (D2CommonUnitTestAnimComplete(hUnit))
		SUnitSetMode(ptGame, hUnit, 12);
	return 1;
}

BOOL __fastcall MonsterModeStartDead(GAME *ptGame, MONMODECHANGE *ptReq)
{
	UNIT *hUnit;

	hUnit = ptReq->hUnit;
	if (hUnit->dwMode)
		MonsterModeKill(ptGame, hUnit, ptReq->bDirection);
	SUnitSetMode(ptGame, hUnit, 12);
	return TRUE;
}

BOOL __fastcall MonsterModeStartNeutral(GAME *ptGame, MONMODECHANGE *ptReq)
{
	UNIT *hUnit;

	hUnit = ptReq->hUnit;
	SUnitSetMode(ptGame, hUnit, 1);
	if (StatsGetState(hUnit, 21))
	{
		EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + 45);
		return TRUE;
	}
	EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + 15);
	return TRUE;
}

BOOL __fastcall MonsterModeStartSkillThree(GAME *ptGame, MONMODECHANGE *ptReq)
{
	SUnitSetMode(ptGame, ptReq->hUnit, 10);
	return TRUE;
}

int __fastcall MonsterModeUpdateSkillThree(GAME *ptGame, UNIT *hUnit)
{
	SUnitStepAnim(ptGame, hUnit);
	D2CommonUnitStepAnimState(hUnit);
	if (D2CommonUnitTestAnimComplete(hUnit))
		SUnitSetMode(ptGame, hUnit, 11);
	return 1;
}

BOOL __fastcall MonsterModeStartSkillFour(GAME *ptGame, MONMODECHANGE *ptReq)
{
	UNIT *hUnit;

	hUnit = ptReq->hUnit;
	SUnitSetMode(ptGame, hUnit, 11);
	EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + 15);
	return TRUE;
}

BOOL __fastcall MonsterModeStartWalk(GAME *ptGame, MONMODECHANGE *ptReq)
{
	UNIT *hUnit;
	BOOL bPath;

	hUnit = ptReq->hUnit;
	bPath = D2CommonPathGetStepCount(hUnit->ptPath);
	if (!bPath)
		return bPath;
	SUnitSetMode(ptGame, hUnit, 2);
	return TRUE;
}

int __fastcall MonsterModeUpdateWalk(GAME *ptGame, UNIT *hUnit)
{
	if (StatsGetState(hUnit, 13))
		SkillsBlazeStep(ptGame, hUnit);
	if (StatsGetState(hUnit, 22))
		SkillsSpiderLayWalk(ptGame, hUnit);
	if (SUnitStepAnim(ptGame, hUnit) == 2)
	{
		MonsterModeEndAttack(ptGame, hUnit);
		return 2;
	}
	return 1;
}

BOOL __fastcall MonsterModeStartRun(GAME *ptGame, MONMODECHANGE *ptReq)
{
	UNIT *hUnit;
	BOOL bPath;

	hUnit = ptReq->hUnit;
	bPath = D2CommonPathGetStepCount(hUnit->ptPath);
	if (!bPath)
		return bPath;
	SUnitSetMode(ptGame, hUnit, 15);
	return TRUE;
}

int __fastcall MonsterModeUpdateRun(GAME *ptGame, UNIT *hUnit)
{
	if (SUnitStepAnim(ptGame, hUnit) == 2)
	{
		MonsterModeEndAttack(ptGame, hUnit);
		return 2;
	}
	return 1;
}

BOOL __fastcall MonsterModeStartGetHit(GAME *ptGame, MONMODECHANGE *ptReq)
{
	UNIT *hUnit;

	hUnit = ptReq->hUnit;
	if (!hUnit->dwMode || hUnit->dwMode == 12)
		return TRUE;
	if (!((MONSTATSTXT *)(D2CommonMonsterTblsGetStats(hUnit->dwClassId)))->bModeGH)
		return FALSE;
	SUnitSetMode(ptGame, hUnit, 3);
	return TRUE;
}

extern DWORD MonsterModeMoves(UNIT *hUnit, int eMode);
__inline DWORD MonsterModeMoves(UNIT *hUnit, int eMode)
{
	const MONMODECHART *ptChart;

	ptChart = &sgatMonModeChart[eMode];
	if (ptChart->bAlways)
		return 1;
	if (ptChart->bNever)
		return 0;
	BYTE *pbStats = (BYTE *)D2CommonMonsterTblsGetStats(hUnit->dwClassId);
	return pbStats[ptChart->dwColumn];
}

BOOL __fastcall MonsterModeStartAttack(GAME *ptGame, MONMODECHANGE *ptReq)
{
	UNIT *hUnit;
	void *ptPath;

	hUnit = ptReq->hUnit;
	if (MonsterModeMoves(hUnit, ptReq->eMode))
	{
		ptPath = hUnit->ptPath;
		if (!D2CommonPathGetStepCount(ptPath))
		{
			if (D2CommonMonsterUnitGetTypeLocalIndex(hUnit) == 110)
			{
				D2CommonPathCompute(ptPath, hUnit);
				SUnitSetMode(ptGame, hUnit, ptReq->eMode);
				return TRUE;
			}
			return FALSE;
		}
		D2CommonPathCompute(ptPath, hUnit);
	}
	SUnitSetMode(ptGame, hUnit, ptReq->eMode);
	if (D2CommonUnitGetSkillThirdCached(hUnit))
	{
		SkillsStart(ptGame, hUnit);
		return TRUE;
	}
	MonsterEvalAttack(hUnit, ptGame);
	return TRUE;
}

int __fastcall MonsterModeUpdateAttack(GAME *ptGame, UNIT *hUnit)
{
	DWORD dwSkill;
	DWORD dwSkillFlags;
	DWORD dwMoves;
	UNIT *hTarget;
	BOOL fEvent;
	BOOL fUse;

	dwSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	if (dwSkill)
	{
		dwSkillFlags = D2CommonSkillsGetNodeFlags(dwSkill);
		fUse = TRUE;
		if (dwSkillFlags & 1)
		{
			SUnitUpdateTarget(ptGame, hUnit);
			if (SUnitStepAnim(ptGame, hUnit) == 2)
			{
				D2CommonSkillsSetNodeFlags(dwSkill, dwSkillFlags | 2);
				SkillsUse(ptGame, hUnit);
				if (hUnit->dwMode == 14)
					fUse = (hUnit->dwFlags0 >> 6) & 1;
				else if (hUnit->bAnimEvent == 1)
					fUse = TRUE;
				else
					fUse = FALSE;
			}
		}
		if (fUse)
			SkillsUse(ptGame, hUnit);
		D2CommonUnitStepAnimState(hUnit);
		return 1;
	}
	dwMoves = MonsterModeMoves(hUnit, hUnit->dwMode);
	if (dwMoves)
	{
		SUnitStepAnim(ptGame, hUnit);
		D2CommonUnitStepAnimState(hUnit);
		if (D2CommonUnitTestAnimComplete(hUnit))
			return 2;
		if (hUnit->dwMode == 14)
			fEvent = (hUnit->dwFlags0 >> 6) & 1;
		else if (hUnit->bAnimEvent == 1)
			fEvent = TRUE;
		else
			fEvent = FALSE;
		if (!fEvent)
			return 1;
	}
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget)
		SUnitDmgDeliver(ptGame, hUnit, hTarget);
	MonsterModeUseSkill(ptGame, hUnit, dwMoves);
	return 1;
}

BOOL __fastcall MonsterModeStartBlock(GAME *ptGame, MONMODECHANGE *ptReq)
{
	SUnitSetMode(ptGame, ptReq->hUnit, 6);
	return TRUE;
}

BOOL __fastcall MonsterModeStartKnockback(GAME *ptGame, MONMODECHANGE *ptReq)
{
	UNIT *hUnit;
	MONSTATSTXT *ptMonsterStats;
	void *ptPath;

	hUnit = ptReq->hUnit;
	ptMonsterStats = ((MONSTATSTXT *)(D2CommonMonsterTblsGetStats(hUnit->dwClassId)));
	if (!ptMonsterStats->bModeGH || !ptMonsterStats->bModeKB)
		return FALSE;
	if (!hUnit->dwMode)
		return TRUE;
	ptPath = hUnit->ptPath;
	D2CommonPathSetType(ptPath, 8);
	if (D2CommonMonsterUnitGetTypeLocalIndex(hUnit) != 78)
		D2CommonPathSetSquareCountdown(ptPath, 5);
	else
		D2CommonPathSetSquareCountdown(ptPath, 10);
	D2CommonPathCompute(ptPath, hUnit);
	SUnitSetMode(ptGame, hUnit, 13);
	return TRUE;
}

void __fastcall MonsterModeEndKnockback(GAME *ptGame, UNIT *hUnit)
{
	MONMODECHANGE tReq;
	MONSTATSTXT *ptMonsterStats;
	DWORD nFrame;

	D2CommonPathRestoreType(hUnit->ptPath);
	ptMonsterStats = ((MONSTATSTXT *)(D2CommonMonsterTblsGetStats(hUnit->dwClassId)));
	if (!ptMonsterStats->bModeGH)
	{
		EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + 1);
		return;
	}
	if (D2CommonMonsterUnitGetTypeLocalIndex(hUnit) != 78)
	{
		MonsterModeInit(hUnit, 3, &tReq);
		UNIT5(hUnit)->dwHitClass = 160;
		MonsterModeSet(ptGame, &tReq, TRUE);
		return;
	}
	if (StatsGetState(hUnit, 21))
		nFrame = ptGame->nFrame + 45;
	else
		nFrame = ptGame->nFrame + 15;
	EventAdd(ptGame, hUnit, EVENT_RUN_AI, nFrame);
}

int __fastcall MonsterModeUpdateKnockback(GAME *ptGame, UNIT *hUnit)
{
	SUnitStepAnim(ptGame, hUnit);
	D2CommonUnitStepAnimState(hUnit);
	if (D2CommonUnitTestAnimComplete(hUnit))
	{
		MonsterModeEndKnockback(ptGame, hUnit);
		return 2;
	}
	return 1;
}

BOOL __fastcall MonsterModeStartSequence(GAME *ptGame, MONMODECHANGE *ptReq)
{
	UNIT *hUnit;

	hUnit = ptReq->hUnit;
	SUnitSetMode(ptGame, hUnit, 14);
	hUnit->dwFlags0 &= ~64;
	return SkillsStart(ptGame, hUnit);
}

int __fastcall MonsterModeUpdateSequence(GAME *ptGame, UNIT *hUnit)
{
	DWORD dwSkill;
	DWORD dwSkillFlags;
	BOOL bEnd;
	int nKind;

	if (D2CommonUnitTestAnimComplete(hUnit))
	{
		MonsterModeEndAttack(ptGame, hUnit);
		return 2;
	}
	dwSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	dwSkillFlags = D2CommonSkillsGetNodeFlags(dwSkill);
	bEnd = TRUE;
	if (dwSkillFlags & 1)
	{
		SUnitUpdateTarget(ptGame, hUnit);
		if (SUnitStepAnim(ptGame, hUnit) == 2)
		{
			D2CommonSkillsSetNodeFlags(dwSkill, dwSkillFlags | 2);
			SkillsUse(ptGame, hUnit);
			bEnd = FALSE;
		}
	}
	nKind = hUnit->bAnimEvent;
	if (nKind == 4 || (bEnd && !(hUnit->dwFlags0 & 64) && (nKind == 1 || nKind == 2)))
		SkillsUse(ptGame, hUnit);
	D2CommonUnitStepAnimState(hUnit);
	return 1;
}

static const MONMODEENTRY sgatMonModeTable[NUM_MONSTERMODES + 4] =
{
	{ MonsterModeStartDeath, MonsterModeUpdateDeath, MonsterModeEndDeath, TRUE },
	{ MonsterModeStartNeutral, MonsterModeUpdateWait, NULL, FALSE },
	{ MonsterModeStartWalk, MonsterModeUpdateWalk, NULL, TRUE },
	{ MonsterModeStartGetHit, MonsterModeUpdateWait, MonsterModeEndAttack, TRUE },
	{ MonsterModeStartAttack, MonsterModeUpdateAttack, MonsterModeEndAttack, TRUE },
	{ MonsterModeStartAttack, MonsterModeUpdateAttack, MonsterModeEndAttack, TRUE },
	{ MonsterModeStartBlock, MonsterModeUpdateWait, MonsterModeEndAttack, TRUE },
	{ MonsterModeStartAttack, MonsterModeUpdateAttack, MonsterModeEndAttack, TRUE },
	{ MonsterModeStartAttack, MonsterModeUpdateAttack, MonsterModeEndAttack, TRUE },
	{ MonsterModeStartAttack, MonsterModeUpdateAttack, MonsterModeEndAttack, TRUE },
	{ MonsterModeStartSkillThree, MonsterModeUpdateSkillThree, (PFNGAMEUNIT)A2Q2UnitKilled, FALSE },
	{ MonsterModeStartSkillFour, MonsterModeUpdateWait, NULL, FALSE },
	{ MonsterModeStartDead, MonsterModeUpdateWait, NULL, FALSE },
	{ MonsterModeStartKnockback, MonsterModeUpdateKnockback, MonsterModeEndKnockback, TRUE },
	{ MonsterModeStartSequence, MonsterModeUpdateSequence, NULL, TRUE },
	{ MonsterModeStartRun, MonsterModeUpdateRun, NULL, TRUE },
	{ MonsterModeStartAttack, MonsterModeUpdateAttack, MonsterModeEndAttack, TRUE },
	{ MonsterModeStartAttack, MonsterModeUpdateAttack, MonsterModeEndAttack, TRUE },
	{ MonsterModeStartAttack, MonsterModeUpdateAttack, MonsterModeEndSpecialA, TRUE },
	{ MonsterModeStartAttack, MonsterModeUpdateAttack, MonsterModeEndSpecialB, TRUE },
};

const MONMODEENTRY * __fastcall MonsterModeGetEntry(UNIT *hUnit, int eMode)
{
	int nClass;
	MONSTATSTXT *ptMonsterStats;

	nClass = hUnit->dwClassId;
	ptMonsterStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(nClass);
	if (!ptMonsterStats->SplGetModeChart)
		return &sgatMonModeTable[eMode];
	if (nClass == 243)
	{
		if (eMode == 11)
			return &sgatMonModeTable[16];
		if (eMode == 10)
			return &sgatMonModeTable[17];
	}
	else if (nClass == 403)
	{
		if (eMode == 4)
			return &sgatMonModeTable[18];
		if (eMode == 5)
			return &sgatMonModeTable[18];
		if (eMode == 9)
			return &sgatMonModeTable[19];
		if (eMode == 8)
			return &sgatMonModeTable[19];
	}
	return &sgatMonModeTable[eMode];
}

void __fastcall MonsterModeUpdate(GAME *ptGame, UNIT *hUnit)
{
	const MONMODEENTRY *cptModeEntry;

	if (sgbMonModesOff)
		return;
	cptModeEntry = MonsterModeGetEntry(hUnit, hUnit->dwMode);
#line 2092
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)cptModeEntry->pfnDo), "cptModeEntry->pfnDo");
	cptModeEntry->pfnDo(ptGame, hUnit);
}

void __fastcall MonsterModeEnd(GAME *ptGame, UNIT *hUnit)
{
	const MONMODEENTRY *cptModeEntry;

	if (sgbMonModesOff)
		return;
	cptModeEntry = MonsterModeGetEntry(hUnit, hUnit->dwMode);
#line 2108
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)cptModeEntry->pfnEnd), "cptModeEntry->pfnEnd");
	cptModeEntry->pfnEnd(ptGame, hUnit);
}

static __inline void MonsterModeCountPath(DWORD dwPathType, DWORD dwVelocity, BYTE bDistMax, GAME *ptGame,
	UNIT *hMonster, MONMODEDATA *ptModeData)
{
	void *ptPathNow;
	int nIndex;
	int nStat;

	if (ptModeData->dwPathTypeNext)
		dwPathType = ptModeData->dwPathTypeNext;
	if (ptModeData->nVelocityNext)
		dwVelocity = ptModeData->nVelocityNext;
	if (ptModeData->bDistMaxNext)
		bDistMax = ptModeData->bDistMaxNext;
	ptModeData->dwPathTypeNext = 0;
	ptModeData->nVelocityNext = 0;
	ptModeData->bDistMaxNext = 0;
	ptPathNow = hMonster->ptPath;
	nStat = -1;
	if (dwPathType == 100)
	{
		dwVelocity = 0;
		nIndex = 0;
	}
	else
	{
		if (!bDistMax)
			bDistMax = 5;
		if (dwPathType == 101)
			dwPathType = 13;
		nIndex = MonsterModeSetPath(hMonster, dwPathType, ptModeData, bDistMax);
		if (nIndex != 1 && D2CommonPathGetTarget(ptPathNow))
			nStat = 10;
	}
	ptModeData->field_14 = nStat;
	ptModeData->nVelocity = dwVelocity;
	ptGame->adwStats[nIndex]++;
	if (nIndex)
		ptGame->adwStats[16]++;
}

static __inline void MonsterModeApplyVelocity(MONMODEDATA *ptModeData, UNIT *hMonster)
{
	DWORD dwVelocity;
	DWORD dwStatList;

	dwVelocity = ptModeData->nVelocity;
	if (!dwVelocity)
		return;
	dwStatList = D2CommonStatsAllocList(hMonster->dwMemPool, 4, 0, 1, hMonster->dwId);
	D2CommonStatsListSetValueA(dwStatList, 67, dwVelocity);
	D2CommonStatsUnitLinkList(hMonster, dwStatList);
	D2CommonUnitUpdateAnimRate(hMonster);
}

static __inline void MonsterModeNotePrev(int nAnimMode, UNIT *hMonster)
{
	void *ptRecord;
	int nRecordMode;

	ptRecord = ((MONSTERDATA *)(hMonster->uTypeData))->pRegionRef;
	nRecordMode = AiGetKind((AIRECORDHEAD *)ptRecord);
	if (nRecordMode >= 16)
		nAnimMode = nRecordMode - 16;
	else if (nRecordMode == 13 && nAnimMode == 3)
		nAnimMode = nRecordMode;
	MonsterRegionRefSetPrevMode((MONSTERREGIONREF *)ptRecord, nAnimMode);
}

BOOL __stdcall MonsterModeSet(GAME *ptGame, MONMODECHANGE *ptReq, BOOL bArg)
{
	UNIT *hMonster;
	int eMode;
	void *ptPath;
	MONMODEDATA *ptModeData;
	int nAnimMode;
	const MONMODEENTRY *cptModeEntry;
	BOOL bStarted;
	UNIT *hUnit;
	int nNewMode;
	DWORD dwStatList;

	hMonster = ptReq->hUnit;
	ptPath = hMonster->ptPath;
	eMode = ptReq->eMode;
#line 2124
	D2ASSERT(! StatsGetState (hMonster, UNIT_STATE_UNINTERRUPTABLE));
	if (StatsGetState(hMonster, UNIT_STATE_UNINTERRUPTABLE))
	{
		SUnitSetStateEndLinked(hMonster, 0);
		if (SUnitIsDead(hMonster))
			return TRUE;
	}
#line 2131
	D2ASSERT(! ((UnitGetClass (hMonster) == MONSTERCLASS_UNDEADHORROR3) && (eMode == MMODE_ATTACK2)));
#line 2132
	D2ASSERT(! ((UnitGetClass (hMonster) == MONSTERCLASS_IRONGOLEM) && (eMode == MMODE_ATTACK2)));
	if (!SUnitIsDead(hMonster))
		D2CommonPathSetCollisionByType(hMonster);
	nAnimMode = hMonster->dwMode;
	if (nAnimMode != 1)
		MonsterModeNotePrev(nAnimMode, hMonster);
	ptModeData = NULL;
	if (eMode != 3)
	{
		if (ptReq->hTarget)
			D2CommonPathSetTarget(ptPath, ptReq->hTarget);
		else
			D2CommonPathSetTargetCoord(ptPath, ptReq->nX, ptReq->nY);
		D2CommonPathSetRecalcBudget(ptPath, 20);
		ptModeData = ((MONSTERDATA *)(hMonster->uTypeData))->ptModeData;
		MonsterModeCountPath(ptReq->bPathType, ptReq->nVelocity, ptReq->bDistMax, ptGame, hMonster, ptModeData);
		MonsterEvalSetDamage(hMonster, eMode);
		MonsterUniqueOnMode(ptGame, hMonster);
	}
	cptModeEntry = MonsterModeGetEntry(hMonster, eMode);
#line 2162
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)cptModeEntry->pfnStart), "cptModeEntry->pfnStart");
	bStarted = cptModeEntry->pfnStart(ptGame, ptReq);
	if (!bStarted)
	{
		D2CommonUnitSetSkillThirdCached(hMonster, 0);
		if (eMode && eMode != 12 && SUnitIsDead(hMonster))
			return TRUE;
		hUnit = ptReq->hUnit;
		SUnitSetMode(ptGame, hUnit, 1);
		if (StatsGetState(hUnit, 21))
			EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + 45);
		else
			EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + 15);
	}
	hMonster->dwFlags0 |= 524288;
	SUnitResetAnim(hMonster);
	if (ptModeData)
		MonsterModeApplyVelocity(ptModeData, hMonster);
	MonsterUniqueOnStart(ptGame, hMonster);
	nNewMode = hMonster->dwMode;
	if (bArg && eMode == nNewMode)
		D2CommonUnitSetSkillThirdCached(hMonster, 0);
	SUnitClearModeEvents(ptGame, hMonster);
	if (MonsterModeGetEntry(hMonster, nNewMode)->fFrameEvents)
	{
		if (MonsterModeMoves(hMonster, nNewMode))
			SUnitSetStandingEvent(ptGame, hMonster);
		else
			SUnitScheduleModeEvents(ptGame, hMonster);
	}
	if (!(nNewMode == 3))
		return bStarted;
	D2CommonPathSetType(ptPath, 0);
	D2CommonPathSetDistMax(ptPath, 0);

	return bStarted;
}

int __fastcall MonsterModeSetPath(UNIT *hUnit, int nPathType,
	MONMODEDATA *ptModeData, BYTE bDistMax)
{
	void *ptPath;
	DWORD dwPath;
	DWORD dwTargetX;
	DWORD dwTargetY;

	ptPath = hUnit->ptPath;
	((void (__stdcall *)(DWORD, BYTE))D2CommonPathSetDistMaxRaw)((DWORD)ptPath, bDistMax);
	dwPath = D2CommonPathGetTarget(ptPath);
	dwTargetX = D2CommonPathGetTargetXRaw((DWORD)ptPath);
	dwTargetY = D2CommonPathGetTargetYRaw((DWORD)ptPath);
	if (dwPath != ptModeData->dwTargetUnit || dwTargetX != ptModeData->dwTargetX
		|| dwTargetY != ptModeData->dwTargetY)
	{
		ptModeData->dwTargetUnit = dwPath;
		ptModeData->dwTargetX = dwTargetX;
		ptModeData->dwTargetY = dwTargetY;
		ptModeData->field_00 = 0;
		ptModeData->field_01 = 0;
	}
	D2CommonPathSetType(ptPath, nPathType);
	D2CommonPathCompute(ptPath, hUnit);
	if (D2CommonPathGetStepCount(ptPath))
	{
		D2CommonUnitRoomAddToChangedList(hUnit);
		hUnit->dwFlags0 |= 1;
		ptModeData->field_00 = 0;
		ptModeData->field_01 = 0;
		return nPathType;
	}
	switch (nPathType)
	{
	case 2:
	case 7:
	case 9:
	case 13:
		D2CommonUnitRoomAddToChangedList(hUnit);
		hUnit->dwFlags0 |= 1;
		D2CommonPathSetType(ptPath, 15);
		D2CommonPathCompute(ptPath, hUnit);
		return 15;
	}
	return nPathType;
}

void __stdcall MonsterModeInit(UNIT *hUnit, int eMode  , MONMODECHANGE *ptReq)
{
	DWORD bPathType;
	int i;

#line 2212
	D2ASSERT(eMode < NUM_MONSTERMODES);
	D2CommonUnitSetSkillThirdCached(hUnit, 0);
	for (i = 0; i < (int)(sizeof(MONMODECHANGE) / sizeof(DWORD)); i++)
		((DWORD *)ptReq)[i] = 0;
	ptReq->eMode = eMode;
	ptReq->hUnit = hUnit;
	D2CommonMonsterTblsGetStats(hUnit->dwClassId);
	bPathType = 100;
	if (MonsterModeMoves(hUnit, eMode))
		bPathType = 101;
	ptReq->bPathType = bPathType;
}

void __fastcall MonsterModeEventClearTarget(GAME *ptGame, UNIT *hUnit)
{
	MonsterAIClearTargetCmd(hUnit);
}

static const PFNMONMODEEND scpfnProcessFunctions[13] =
{
	MonsterModeUpdate, MonsterModeEnd, AiThinkProcess, MonsterModeRegen, (PFNGAMEUNIT)A2Q2UnitKilled,
	SkillsActiveStates, PlrModeEventChat, MonsterUniqueOnEvent, SkillsEventPeriodic, NULL,
	MonsterModeEventClearTarget, NULL, ITEMSModeEventExpire,
};

void __fastcall MonsterModeProcessEvent(GAME *ptGame, UNIT *hUnit, int eEvent  )
{
	if (eEvent < 3 || (eEvent > 5 && eEvent != 12))
	{
		if (StatsGetState(hUnit, 1) && !SUnitIsDead(hUnit))
			return;
	}
#line 2326
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)scpfnProcessFunctions[eEvent]), "scpfnProcessFunctions[eEvent]");
	scpfnProcessFunctions[eEvent](ptGame, hUnit);
}

UNIT * __fastcall MonsterModeDropTreasureItem(GAME *ptGame, UNIT *hUnit, int nQuality)
{
	MONSTATSTXT *ptMonsterStats;
	int nDifficulty;
	int nCount;
	int i;
	int nIdx;
	void *ptTreasureClass;
	DWORD dwCode;

	ptMonsterStats = ((MONSTATSTXT *)(D2CommonMonsterTblsGetStats(hUnit->dwClassId)));
	nDifficulty = ptGame->bDifficultyLevel;
#line 2342
	D2ASSERT(ptGame->bDifficultyLevel < NUM_DIFFICULTY_LEVELS);
	nCount = 0;
	for (i = 0; i < 4; i++)
	{
		if (ptMonsterStats->TreasureClass[nDifficulty][i])
			nCount++;
	}
	if (!nCount)
		goto lbNull;
	nIdx = (int)D2CommonSeedRand(&hUnit->tSeed, nCount);
	nCount = ptMonsterStats->TreasureClass[nDifficulty][nIdx];
	ptTreasureClass = (void *)D2CommonMonsterTblsGetTreasureClass(nCount);
	if (!ptTreasureClass)
	{
	lbNull:
		return NULL;
	}
	dwCode = MonsterModeChooseItem(hUnit,
		ITEMSModeDrawCode(ptGame, (struct ITEMCODELIST *)ptTreasureClass, hUnit, FALSE));
	if (!dwCode)
	{
		ptTreasureClass = (void *)D2CommonMonsterTblsGetTreasureClass(1);
		dwCode = ITEMSModeDrawCode(ptGame, (struct ITEMCODELIST *)ptTreasureClass, hUnit, FALSE);
		if (!dwCode)
			return NULL;
	}
	return MonsterModeCreateItem(ptGame, hUnit, dwCode, nQuality);
}

extern DWORD __fastcall MonsterModeGetChatEndFrame(void *hNode);
__inline DWORD __fastcall MonsterModeGetChatEndFrame(void *hNode)
{
	return D2CommonChatNodeGetEndFrame(hNode);
}

void __fastcall MonsterModeUpdateClient(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient)
{
	UNIT *hPlayer;
	BYTE bField;
	int nLife;
	int nMaxLife;
	BYTE bLife;
	void *ptRecord;
	struct MONSTERUNIQUE *ptUnique;
	int *pnRecord;
	WORD wRecord;
	DWORD dwPacked;
	WORD wField;
	DWORD dwFlag;
	MONSTERDATA *ptData;

	if (hUnit->dwFlags & 65536)
	{
		SCmdSendUnit(ptClient, S2C_UNIT_WARP, (BYTE)hUnit->eType, hUnit->dwId, (WORD)D2CommonUnitGetX(hUnit),
			(WORD)D2CommonUnitGetY(hUnit), 1);
		SUnitUpdateClients(ptGame, hUnit, NULL);
	}
	if (hUnit->dwFlags0 & 1)
		MonsterSendMode(ptGame, hUnit, ptClient);
	hUnit->dwFlags0 &= ~524288;
	if (hUnit->dwFlags0 & 256)
		SUnitMsgSendChat(hUnit, ptClient);
	if (hUnit->dwFlags & 1)
	{
		hPlayer = ClientGetPlayerUnit(ptClient, FALSE);
		if (hPlayer && MonsterAIThreatAcked(hUnit, hPlayer))
			SUnitProxySendItems(ptGame, hUnit, hPlayer, ptClient);
	}
	if (hUnit->dwFlags0 & 1024)
		SUnitNpcSendUnitEvent(hUnit, ptClient);
	if (hUnit->dwFlags0 & 32768)
	{
		bField = (BYTE)UNIT5(hUnit)->dwHitClass;
		nLife = D2CommonStatsUnitGetValue(hUnit, 6);
		nMaxLife = D2CommonStatsUnitCalcMaxHp(hUnit);
		if (nMaxLife)
		{
			bLife = (BYTE)((nLife << 7) / nMaxLife);
			if (bLife > 1)
				bLife--;
		}
		else
			bLife = 0;
		if (MonsterUniqueGetBurstFlag(((MONSTERDATA *)(hUnit->uTypeData))->ptUnique))
			bLife |= 128;
		SCmdSendUnitMode(ptClient, 12, 1, hUnit->dwId, 19, bField, bLife);
	}
	if (D2CommonStatsUnitTestAnyActiveState(hUnit))
		SUnitMsgSendStates(hUnit, ptClient, 0);
	if (D2CommonStatsTestHasOverlay(hUnit))
		SUnitMsgSendOverlay(hUnit, ptClient);
	if (hUnit->dwFlags0 & 2048)
	{
		ptData = (MONSTERDATA *)hUnit->uTypeData;
		ptRecord = ptData->pRegionRef;
		if (AiRecordTestFlags((AIRECORDHEAD *)ptRecord, AIFLAG_01))
		{
			ptUnique = ptData->ptUnique;
			pnRecord = MonsterUniqueGetMods(ptUnique);
			wRecord = MonsterUniqueGetId(ptUnique);
			dwPacked = (BYTE)pnRecord[0] | ((BYTE)pnRecord[1] << 8);
			wField = (BYTE)pnRecord[2];
			dwFlag = MonsterUniqueIsChampion(ptUnique);
			AiRecordSetFlags((AIRECORDHEAD *)ptRecord, AIFLAG_01, FALSE);
			SCmdSendMonsterMods(ptClient, hUnit->dwId, 1, wRecord, (WORD)dwPacked, wField, (WORD)dwFlag);
		}
	}
}
