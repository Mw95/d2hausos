#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\UNIT\\SUnitInactive.cpp"
#include "../D2game.h"
#include "../AI/AiThink.h"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "../GAME/Targets.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/MonsterMsg.h"
#include "../MONSTER/MonsterUnique.h"
#include "../OBJECTS/Objects.h"
#include "../PLAYER/PlayerPets.h"
#include "../QUESTS/QUESTS.h"
#include "../QUESTS/a4q2.h"
#include "SUnit.h"
#include "../AI/AiGeneral.h"
#include "../GUILDS/Guilds.h"
#include "SUnitInactive.h"

#define UNIT_OBJECT				2
#define UNIT_ITEM				4
#define UNIT_ROOMTILE			5

#define PMODE_DEAD				17

#define TARGET_LISTNO_MAX_REGULAR	8
#define TARGET_LISTNO_NULL		11
#define TARGET_LISTNO_9			9
#define UnitGetTargetListNo(hUnit)	((hUnit)->nTargetListNo)

#define ALIGNMENT_0				0
#define ALIGNMENT_NEUTRAL		1
#define ALIGNMENT_BAD			2

#define MAXIMUM_ATTACHED_ITEMS	4
#define NUM_UNIQUEMODS			9

#define STAT_MAXLIFE			7
#define STAT_LIFE				6
#define STAT_EXPERIENCE			13
#define STAT_GOLD				14
#define STAT_QUANTITY			70
#define STAT_DURABILITY			72
#define STAT_MAXDURABILITY		73

#define ITEMTYPE_GOLD			4
#define ITEMTYPE_EAR			7
#define ITEMFLAG_SOCKETED		2048

#define IM_UNIQUE				1
#define IM_FIELD0C				2
#define IM_DEAD					4
#define IM_AIFLAG1				8
#define IM_AIFLAG2				16
#define IM_MINION				32
#define IM_NOCOUNT				64
#define IM_ALIGN1				128
#define IM_ALIGN0				256
#define IM_ALIGN2				512
#define IM_INLIST				1024

#define UNITFLAG0_10			16
#define UNITFLAG0_0200			512
#define UNITFLAG0_KEEP			33554432
#define UNITFLAG0_RESPAWNED		50331648
#define UNIT_MASK_PET			0x80000000
#define UNITFLAG_HELD			256
#define UNITFLAG_200			512

#define UNITFLAG_7				7
#define EVENT_1					1
#define EVENT_REGENERATE		2
#define EVENT_5					5

#define LEVEL_6C				108
#define MONCLASS_F3				243
#define MONCLASS_10B			267
#define MONCLASS_137			311
#define MONCLASS_138			312
#define MONCLASS_16B			363
#define MONCLASS_16C			364
#define OBJCLASS_3B				59
#define OBJCLASS_3C				60
#define OBJCLASS_64				100
#define UNIQUE_FIRSTSUPER		65493
#define SUPERUNIQUE_6			6
#define AISTATE_A				10

#define SPAWNFLAGS				98
#define SPAWNFLAGS_NOCOUNT		106
#define SPAWNFLAG_FLYING		128
#define SPAWN_RADIUS_NONE		(-1)
#define SPAWN_RADIUS			5
#define FREEAREA_PATTERN		7169

#define FRAMES_LIFEBACK			7500
#define FRAMES_ITEMKEPT			22500
#define FRAMES_GOODITEMKEPT		90000
#define ITEMQUALITY_4			4
#define ITEMQUALITY_7			7

struct INACTIVEITEM
{
 	BYTE		bEarLevel;
 	char		szEarName[20 - 1];
 	int			nLevel;
 	DWORD		dwClassId;
 	DWORD		dwMode;
 	DWORD		dwQuality;
 	int			nQuantity;
 	int			nDurability;
 	int			nMaxDurability;
 	int			nX;
 	int			nY;
 	DWORD		dwFileIndex;
 	DWORD		dwOwnerId;
 	DWORD		dwFlags;
 	DWORD		dwSeed;
 	DWORD		dwItemSeed;
 	int			nFrame;
 	BYTE		bQuest;
 	BYTE		unobserved_51[3];
 	struct INACTIVEITEM	*aptSocketed[MAXIMUM_ATTACHED_ITEMS];
 	struct INACTIVEITEM	*ptNext;
};
typedef struct INACTIVEITEM INACTIVEITEM;

struct INACTIVEMONSTER
{
 	int			nX;
 	int			nY;
 	DWORD		dwClassId;
 	DWORD		dwUnitId;
 	DWORD		dwUnitFlags0;
 	DWORD		dwUnitFlags;
 	DWORD		dwFlags;
 	DWORD		dwOwnerId;
 	AIMINION	*pAiList;
 	DWORD		dwAiCell;
 	int			nAiKind;

 	DWORD		dwRegionLevel;
 	WORD		wUniqueId;
 	BYTE		unobserved_32[2];
 	int			aeMods[NUM_UNIQUEMODS];
 	int			nExperience;
 	int			nMaxLife;
 	int			nLife;
 	DWORD		field_64;
 	DWORD		dwAiArg2;
 	int			nFrame;
 	struct INACTIVEMONSTER	*ptNext;
};
typedef struct INACTIVEMONSTER INACTIVEMONSTER;

struct INACTIVEUNIT
{
 	int			nX;
 	int			nY;
 	DWORD		eType;
 	DWORD		dwClassId;
 	DWORD		nMode;
 	int			nFrame;
 	DWORD		dwUnitFlags0;
 	DWORD		dwUnitFlags;
 	DWORD		field_20;
 	DWORD		field_24;
 	BYTE		bField28;
 	BYTE		unobserved_29[3];
 	DWORD		dwDropItemCode;
 	struct INACTIVEUNIT	*ptNext;
};
typedef struct INACTIVEUNIT INACTIVEUNIT;

struct INACTIVENODE
{
 	int			nX;
 	int			nY;
 	INACTIVEITEM	*ptItems;
 	INACTIVEMONSTER	*ptMonsters;
 	INACTIVEUNIT	*ptUnits;
 	struct INACTIVENODE	*ptNext;
};
typedef struct INACTIVENODE INACTIVENODE;

#define MONDATA(hUnit)	((MONSTERDATA *)(hUnit)->uTypeData)

static void SUnitInactiveFreeMonster(GAME *ptGame, INACTIVEMONSTER *ptRecord)
{
	if (ptRecord->dwAiCell)
		D2CommonPresetFileListFree(ptGame->pMemPool, ptRecord->dwAiCell);
#line 149
	D2FREEPOOL(ptGame->pMemPool, ptRecord);
}

static void SUnitInactiveTakeNode(GAME *ptGame, GAMEROOM *hRoom, INACTIVEITEM **ppItems,
	INACTIVEUNIT **ppUnits)
{
	GAMERECT atRect[2];
	INACTIVENODE *ptNode;
	INACTIVENODE *ptPrev;
	DWORD dwAct;

	*ppUnits = NULL;
	*ppItems = NULL;
	dwAct = D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom));
	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	ptPrev = NULL;
	for (ptNode = ptGame->ptInactive[dwAct]; ptNode; ptNode = ptNode->ptNext)
	{
		if (atRect[0].nX > ptNode->nX)
			goto done;
		if (atRect[0].nX == ptNode->nX && atRect[0].nY == ptNode->nY)
			break;
		ptPrev = ptNode;
	}
	if (ptNode)
	{
		if (ptPrev)
			ptPrev->ptNext = ptNode->ptNext;
		else
			ptGame->ptInactive[dwAct] = ptNode->ptNext;
		*ppUnits = ptNode->ptUnits;
		*ppItems = ptNode->ptItems;
#line 190
		D2FREEPOOL(ptGame->pMemPool, ptNode);
	}
done:
	;
}

static int ObjChargesRegained(int nFrames, int nFrameDelta)
{
	int nCount;

	nCount = nFrames / nFrameDelta;
	if (nCount >= 1)
		nCount = 1;
	return nCount;
}

static int SUnitInactiveAlignment(DWORD dwFlags)
{
	int eAlign;

	eAlign = ALIGNMENT_BAD;
	if (dwFlags & IM_ALIGN0)
		eAlign = ALIGNMENT_0;
	if (dwFlags & IM_ALIGN1)
		eAlign = ALIGNMENT_NEUTRAL;
	return eAlign;
}

static void ObjSetCharges(UNIT *hUnit, BYTE bCharges)
{
	D2ASSERT(hUnit);
	OBJDATA(hUnit)->field_04 = bCharges;
}

static void SUnitInactiveRestoreUnit(GAME *ptGame, GAMEROOM *hRoom,
	INACTIVEUNIT *ptInactiveUnitPrev  )
{
	UNIT *hUnit;
	MISSOBJECTSTXT *ptObj;
	DWORD eType;
	int nMode;
	int nEventFrame;
	int nFrameDelta;
	int nCount;
	int nAt;
	BYTE bMax;
	BYTE bCharges;
	BYTE bCur;

	hUnit = NULL;
	eType = ptInactiveUnitPrev->eType;
	nMode = ptInactiveUnitPrev->nMode;
	if (ptInactiveUnitPrev->dwUnitFlags & UNITFLAG_HELD)
	{
		hUnit = SUnitFind(ptGame, eType, ptInactiveUnitPrev->field_20);
		if (hUnit)
		{
			SUnitReplace(ptGame, hUnit, hRoom, ptInactiveUnitPrev->nX, ptInactiveUnitPrev->nY);
			hUnit->dwFlags0 |= UNITFLAG0_10;
			switch (eType)
			{
			case UNIT_PLAYER:
#line 331
				D2ASSERT(ptInactiveUnitPrev->nMode == PMODE_DEAD);
				D2CommonUnitSetMode(hUnit, PMODE_DEAD);
				break;
			case UNIT_OBJECT:
				ObjSetCharges(hUnit, ptInactiveUnitPrev->bField28);
				break;
			}
		}
#line 337
		D2FREEPOOL(ptGame->pMemPool, ptInactiveUnitPrev);
		return;
	}
	if (eType != UNIT_MONSTER)
		hUnit = SUnitSpawnPreset(ptGame, hRoom, (int)eType, (int)ptInactiveUnitPrev->dwClassId,
			ptInactiveUnitPrev->nX, ptInactiveUnitPrev->nY, nMode,
			ptInactiveUnitPrev->dwUnitFlags0);
	switch (eType)
	{
	case UNIT_OBJECT:
#line 357
		D2ASSERT(hUnit);
		ptObj = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hUnit->dwClassId);
		if (D2CommonUnitTestObjectSubclassBitZero(hUnit) && ptInactiveUnitPrev->nFrame)
		{
			nEventFrame = (int)((DWORD)(WORD)ptInactiveUnitPrev->field_20 << 16
				| (WORD)ptInactiveUnitPrev->field_24);
			if (nEventFrame > 0)
			{
				if (nEventFrame <= ptGame->nFrame)
					nEventFrame = ptGame->nFrame + 1;
				EventAdd(ptGame, hUnit, EVENT_5, nEventFrame);
			}
			ObjSetCharges(hUnit, ptInactiveUnitPrev->bField28);
			D2CommonUnitSetObjectShrine(hUnit, D2CommonObjTblsGetShrine(ptInactiveUnitPrev->bField28));
		}
		else if (ptObj->SubClass & 32)
		{
			bMax = ObjectRegenerateMax(ptObj);
			if (ptInactiveUnitPrev->bField28 < bMax)
			{
				nFrameDelta = ptObj->Param[0];
#line 384
				D2ASSERT(nFrameDelta);
				if (ptInactiveUnitPrev->nFrame < ptGame->nFrame + nFrameDelta)
				{
					nCount = ObjChargesRegained(ptGame->nFrame - ptInactiveUnitPrev->nFrame, nFrameDelta);
					bCur = ptInactiveUnitPrev->bField28;
					bCharges = (BYTE)nCount + bCur;
					if (bCharges > bMax)
						bCharges = bMax;
					hUnit->dwMode = (DWORD)((int)ObjectRegenerateModes() - (int)bCharges / ptObj->Param[2]);
					if (hUnit->dwMode > 2)
					{
						hUnit->dwMode = 0;
						ObjSetCharges(hUnit, bMax);
						break;
					}
				}
				else
					bCharges = ptInactiveUnitPrev->bField28;
				ObjSetCharges(hUnit, bCharges);
				if (bCharges < bMax)
				{
					nCount = (int)bMax - (int)bCharges;
					for (nAt = 0; nAt < nCount; nAt++)
						EventAdd(ptGame, hUnit, EVENT_REGENERATE, ptGame->nFrame + (nAt + 1) * nFrameDelta);
				}
			}
		}
		else
		{
			ObjSetCharges(hUnit, ptInactiveUnitPrev->bField28);
			hUnit->field_98 = (BYTE)ptInactiveUnitPrev->field_24;
			if (ptInactiveUnitPrev->dwDropItemCode)
				hUnit->dwDropItemCode = ptInactiveUnitPrev->dwDropItemCode;
		}
		break;
	}
#line 440
	D2FREEPOOL(ptGame->pMemPool, ptInactiveUnitPrev);
}

static void SUnitInactiveSaveUnit(GAME *ptGame, UNIT *hUnit)
{
	GAMERECT atRect[2];
	GAMEROOM *hRoom;
	INACTIVENODE *ptNode;
	INACTIVENODE *ptPrev;
	INACTIVENODE *ptNew;
	INACTIVEUNIT *ptRecord;
	MISSOBJECTSTXT *ptObj;
	DWORD eType;
	DWORD bAct;
	int i;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	bAct = D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom));
	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	ptNode = ptGame->ptInactive[bAct];
	ptPrev = NULL;
	for (; ptNode; ptNode = ptNode->ptNext)
	{
		if (atRect[0].nX == ptNode->nX && atRect[0].nY == ptNode->nY)
			break;
		if (atRect[0].nX > ptNode->nX)
			break;
		ptPrev = ptNode;
	}
	if (!ptNode || atRect[0].nX > ptNode->nX)
	{
#line 563
		ptNew = (INACTIVENODE *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(INACTIVENODE));
		for (i = 0; i < (int)(sizeof(INACTIVENODE) / sizeof(DWORD)); i++)
			((DWORD *)ptNew)[i] = 0;
		ptNew->nX = atRect[0].nX;
		ptNew->nY = atRect[0].nY;
		ptNew->ptNext = ptNode;
		if (ptPrev)
			ptPrev->ptNext = ptNew;
		else
			ptGame->ptInactive[bAct] = ptNew;
		ptNode = ptNew;
	}
	eType = hUnit->eType;
	if (eType == UNIT_ITEM)
	{
		SUnitInactiveSaveItem(ptGame, hUnit, ptNode, NULL);
		return;
	}
#line 585
	ptRecord = (INACTIVEUNIT *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(INACTIVEUNIT));
	for (i = 0; i < (int)(sizeof(INACTIVEUNIT) / sizeof(DWORD)); i++)
		((DWORD *)ptRecord)[i] = 0;
	ptRecord->nX = (int)D2CommonUnitGetX(hUnit);
	ptRecord->nY = (int)D2CommonUnitGetY(hUnit);
	ptRecord->eType = eType;
	ptRecord->dwClassId = hUnit->dwClassId;
	switch (eType)
	{
	case UNIT_PLAYER:
		ptRecord->field_20 = hUnit->dwId;
		ptRecord->nMode = hUnit->dwMode;
		break;
	case UNIT_OBJECT:
	{
		ptObj = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hUnit->dwClassId);
		if (hUnit->dwMode == OBJMODE_OPERATING && !ptObj->CycleAnim[OBJMODE_OPERATING]
			&& ptObj->Mode[OBJMODE_OPENED])
			hUnit->dwMode = OBJMODE_OPENED;
		ptRecord->nMode = hUnit->dwMode;
		if (D2CommonUnitTestObjectSubclassBitZero(hUnit))
		{
			ptRecord->field_20 = (DWORD)EventGetFirstFrame(ptGame, hUnit, EVENT_5) >> 16;
			ptRecord->field_24 = (DWORD)EventGetFirstFrame(ptGame, hUnit, EVENT_5) & 65535;
		}
		else if (hUnit->dwClassId == OBJCLASS_3B || hUnit->dwClassId == OBJCLASS_3C
			|| hUnit->dwClassId == OBJCLASS_64)
			ptRecord->field_20 = hUnit->dwId;
		else
			ptRecord->field_24 = hUnit->field_98;
		ptRecord->dwDropItemCode = hUnit->dwDropItemCode;
		ptRecord->bField28 = OBJDATA(hUnit)->field_04;
		break;
	}
	default:
		ptRecord->nMode = hUnit->dwMode;
		break;
	}
	ptRecord->nFrame = ptGame->nFrame;
	ptRecord->dwUnitFlags0 = hUnit->dwFlags0;
	ptRecord->dwUnitFlags = hUnit->dwFlags;
	ptRecord->ptNext = ptNode->ptUnits;
	ptNode->ptUnits = ptRecord;
}

static void SUnitInactiveRestoreItems(GAME *ptGame, GAMEROOM *hRoom, INACTIVEITEM *ptNextItem)
{
	INACTIVEITEM *ptItem;

	while ((ptItem = ptNextItem) != NULL)
	{
		ptNextItem = ptItem->ptNext;
		if (!ptItem->bQuest)
		{
			if (ptItem->dwQuality == ITEMQUALITY_4 || ptItem->dwQuality == ITEMQUALITY_4 + 1
				|| ptItem->dwQuality == ITEMQUALITY_4 + 2 || ptItem->dwQuality == ITEMQUALITY_7)
			{
				if ((DWORD)(ptGame->nFrame - ptItem->nFrame) >= FRAMES_GOODITEMKEPT)
					continue;
			}
			else if ((DWORD)(ptGame->nFrame - ptItem->nFrame) >= FRAMES_ITEMKEPT)
				continue;
		}
		SUnitInactiveRestoreItem(ptGame, hRoom, ptItem);
	}
}

void __fastcall SUnitInactiveRestoreRoom(GAME *ptGame, GAMEROOM *hRoom)
{
	INACTIVENODE *ptNode;
	INACTIVEMONSTER *ptInactiveUnitPrev;
	INACTIVEMONSTER *ptNextMonster;
	INACTIVEITEM *ptNextItem;
	INACTIVEUNIT *ptUnits;
	INACTIVEUNIT *ptNextUnit;
	UNIT *hUnit;
	COLPOINT tPoint;
	BOOL fReleased;
	DWORD dwFlags;
	DWORD dwClassId;
	DWORD dwMode;
	WORD wSpawnFlags;
	int nX;
	int nY;
	int nSpawnX;
	int nSpawnY;
	int nKind;
	int eOldAlign;
	int nList;
	int nRegionLevel;
	int nLevelId;
	MONSTERDATA *ptMonData;
	int *peMods;
	int i;

	ptNode = SUnitInactiveFindNode(ptGame, hRoom, FALSE);
	if (ptNode)
	{
		ptNextMonster = ptNode->ptMonsters;
		fReleased = FALSE;
		if (D2CommonDungeonRoomGetLevelId(hRoom) == LEVEL_6C && A4Q2MonstersReleased(ptGame))
			fReleased = TRUE;
		while (ptNextMonster)
		{
			ptInactiveUnitPrev = ptNextMonster;
			ptNextMonster = ptInactiveUnitPrev->ptNext;
			dwMode = MONMODE_NEUTRAL;
			nX = ptInactiveUnitPrev->nX;
			nY = ptInactiveUnitPrev->nY;
			dwClassId = ptInactiveUnitPrev->dwClassId;
			dwFlags = ptInactiveUnitPrev->dwFlags;
			if (dwFlags & IM_DEAD)
				dwMode = MONMODE_DEAD;
			else if (fReleased && dwClassId != MONCLASS_F3)
			{
				SUnitInactiveFreeMonster(ptGame, ptInactiveUnitPrev);
				continue;
			}
			hUnit = NULL;
			if (ptInactiveUnitPrev->dwUnitFlags & UNITFLAG_HELD)
			{
				hUnit = SUnitFind(ptGame, UNIT_MONSTER, ptInactiveUnitPrev->dwUnitId);
				if (hUnit)
					SUnitReplace(ptGame, hUnit, hRoom, nX, nY);
				AiFreeMinionList(ptGame, ptInactiveUnitPrev->pAiList);
				SUnitInactiveFreeMonster(ptGame, ptInactiveUnitPrev);
				continue;
			}
			wSpawnFlags = SPAWNFLAGS;
			if (dwFlags & IM_NOCOUNT)
				wSpawnFlags = SPAWNFLAGS_NOCOUNT;
			if (((MONSTATSTXT *)D2CommonMonsterTblsGetStats(dwClassId))->bFlying)
				wSpawnFlags |= SPAWNFLAG_FLYING;
			nKind = 0;
			if (ptInactiveUnitPrev->dwFlags & IM_UNIQUE)
				nKind = 1;
			if (ptInactiveUnitPrev->dwFlags & IM_MINION)
				nKind = 2;
			switch (nKind)
			{
			case 0:
				hUnit = MonsterSpawnWithArg(ptGame, hRoom, nX, nY, (int)dwClassId, dwMode,
					ptInactiveUnitPrev->dwUnitId, SPAWN_RADIUS_NONE, wSpawnFlags);
				if (hUnit)
					break;
				hUnit = MonsterSpawnWithArg(ptGame, hRoom, nX, nY, (int)dwClassId, dwMode,
					ptInactiveUnitPrev->dwUnitId, SPAWN_RADIUS, wSpawnFlags);
				if (hUnit)
					break;
				if (MonsterFindSpawnPoint(ptGame, hRoom, NULL, (int)dwClassId, &nSpawnX, &nSpawnY, FALSE))
				{
					hUnit = MonsterSpawnWithArg(ptGame, hRoom, nSpawnX, nSpawnY, (int)dwClassId, dwMode,
						ptInactiveUnitPrev->dwUnitId, SPAWN_RADIUS_NONE, wSpawnFlags);
					if (hUnit)
						break;
				}
				{
					GAMEROOM *hFreeRoom;

					tPoint.nX = nX;
					tPoint.nY = nY;
					D2CommonCollisnFindFreeArea(hRoom, &tPoint, 1, FREEAREA_PATTERN, &hFreeRoom);
					if (hFreeRoom)
						hUnit = MonsterSpawnWithArg(ptGame, hFreeRoom, tPoint.nX, tPoint.nY, (int)dwClassId,
							dwMode, ptInactiveUnitPrev->dwUnitId, SPAWN_RADIUS_NONE, wSpawnFlags);
				}
				break;
			case 1:
#line 1497
				D2WARN(! (ptInactiveUnitPrev->dwFlags & IM_NOCOUNT));
				hUnit = MonsterUniqueRecreate(ptGame, hRoom, nX, nY, (int)dwClassId,
					(int)ptInactiveUnitPrev->dwUnitId, ptInactiveUnitPrev->wUniqueId,
					ptInactiveUnitPrev->dwFlags & IM_FIELD0C, ptInactiveUnitPrev->aeMods);
				break;
			case 2:
#line 1501
				D2WARN(! (ptInactiveUnitPrev->dwFlags & IM_NOCOUNT));
				hUnit = MonsterUniqueSpawnWithMods(ptGame, hRoom, nX, nY, (int)dwClassId,
					(int)ptInactiveUnitPrev->dwUnitId, ptInactiveUnitPrev->aeMods);
				break;
			}
			dwFlags = ptInactiveUnitPrev->dwFlags;
			eOldAlign = SUnitInactiveAlignment(dwFlags);
			if (hUnit)
			{
				AiRestoreOwner(ptGame, hUnit, ptInactiveUnitPrev->dwOwnerId, 1, dwFlags & IM_AIFLAG1,
					dwFlags & IM_AIFLAG2, ptInactiveUnitPrev->pAiList);
				D2CommonStatsUnitSetValue(hUnit, STAT_EXPERIENCE, ptInactiveUnitPrev->nExperience);
				if ((DWORD)(ptGame->nFrame - ptInactiveUnitPrev->nFrame) >= FRAMES_LIFEBACK)
				{
					D2CommonStatsUnitSetValue(hUnit, STAT_MAXLIFE, ptInactiveUnitPrev->nMaxLife);
					D2CommonStatsUnitSetValue(hUnit, STAT_LIFE, ptInactiveUnitPrev->nMaxLife);
				}
				else
				{
					D2CommonStatsUnitSetValue(hUnit, STAT_MAXLIFE, ptInactiveUnitPrev->nMaxLife);
					D2CommonStatsUnitSetValue(hUnit, STAT_LIFE, ptInactiveUnitPrev->nLife);
				}
				AiThinkSet(ptGame, hUnit, MONDATA(hUnit)->ptAi, (int)ptInactiveUnitPrev->nAiKind);

				SUnitInactiveRestoreAi(ptGame, hUnit, ptInactiveUnitPrev);
				if (ptInactiveUnitPrev->dwFlags & IM_INLIST)
				{
					if (eOldAlign == ALIGNMENT_0)
					{ nList = TARGET_LISTNO_MAX_REGULAR; TargetsAddToFurtherList(ptGame, hUnit, 0, nList); }
					else
					{
#line 1538
						D2ASSERT(eOldAlign == ALIGNMENT_NEUTRAL);
						nList = TARGET_LISTNO_9; TargetsAddToFurtherList(ptGame, hUnit, 0, nList);
					}

				}
				ptMonData = MONDATA(hUnit);
				peMods = MonsterUniqueGetMods(ptMonData->ptUnique);
				for (i = 0; i < NUM_UNIQUEMODS; i++)
				{
					if (peMods[i] != ptInactiveUnitPrev->aeMods[i])
						MonsterUniqueAddMod(ptGame, hUnit, ptInactiveUnitPrev->aeMods[i], FALSE);
				}
				SUnitSetAlignment(hUnit, (BYTE)eOldAlign);
				if (ptInactiveUnitPrev->dwUnitFlags & UNITFLAG_200)
					hUnit->dwFlags |= UNITFLAG_200;
				hUnit->dwFlags0 |= UNITFLAG0_RESPAWNED;
			}
			else
				AiFreeMinionList(ptGame, ptInactiveUnitPrev->pAiList);
			nRegionLevel = (int)ptInactiveUnitPrev->dwRegionLevel;
			nLevelId = (int)D2CommonDungeonRoomGetLevelId(hRoom);
			MonsterRegionRemove((DWORD *)&ptGame->pMonRegions, nRegionLevel, nLevelId, eOldAlign,
				dwMode == MONMODE_DEAD, (BOOL)(ptInactiveUnitPrev->dwFlags & IM_NOCOUNT), hUnit);
			SUnitInactiveFreeMonster(ptGame, ptInactiveUnitPrev);
		}
	}
	SUnitInactiveTakeNode(ptGame, hRoom, &ptNextItem, &ptNextUnit);
	SUnitInactiveRestoreItems(ptGame, hRoom, ptNextItem);
	while (ptNextUnit)
	{
		ptUnits = ptNextUnit;
		ptNextUnit = ptUnits->ptNext;
		SUnitInactiveRestoreUnit(ptGame, hRoom, ptUnits);
	}
}

UNIT *__fastcall SUnitInactiveRestoreItem(GAME *ptGame, GAMEROOM *hRoom, INACTIVEITEM *ptRecord)
{
	ITEMCREATE tCreate;
	UNIT *hItem;
	void *ptInventory;
	UNIT *hSocket;
	int i;

	for (i = 0; i < (int)(sizeof(ITEMCREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.nLevel = ptRecord->nLevel;
	tCreate.dwClassId = ptRecord->dwClassId;
	tCreate.dwMode = ptRecord->dwMode;
	tCreate.nX = ptRecord->nX;
	tCreate.nY = ptRecord->nY;
	tCreate.dwQuality = ptRecord->dwQuality;
	tCreate.nQuantity = ptRecord->nQuantity;
	tCreate.nDurability = ptRecord->nDurability;
	tCreate.nMaxDurability = ptRecord->nMaxDurability;
	tCreate.dwFileIndex = ptRecord->dwFileIndex;
	tCreate.dwSeed = ptRecord->dwSeed;
	tCreate.dwItemSeed = ptRecord->dwItemSeed;
	tCreate.ptGame = ptGame;
	tCreate.hRoom = hRoom;
	tCreate.wCreateFlags = 1;
	tCreate.dwFlags = ptRecord->dwFlags;
	hItem = ITEMSRecreate(ptGame, &tCreate);
#line 244
	D2ASSERT(hItem);
	D2CommonITEMSDataSetUniqueOrSetRow(hItem, ptRecord->dwFileIndex);
	D2CommonITEMSDataSetOwnerId(hItem, ptRecord->dwOwnerId);
	if (D2CommonITEMSGetType(hItem) == ITEMTYPE_EAR)
	{
		D2CommonITEMSSetNameBuffer(hItem, ptRecord->szEarName);
		D2CommonITEMSDataSetEarLevel(hItem, ptRecord->bEarLevel);
	}
	ptInventory = hItem->ptInventory;
	for (i = 0; i < MAXIMUM_ATTACHED_ITEMS; i++)
	{
		if (ptRecord->aptSocketed[i])
		{
			hSocket = SUnitInactiveRestoreItem(ptGame, hRoom, ptRecord->aptSocketed[i]);
			D2CommonINVENTORYAddItem(ptInventory, hSocket, 1);
		}
	}
#line 266
	D2FREEPOOL(ptGame->pMemPool, ptRecord);
	return hItem;
}

void __fastcall SUnitInactiveFreeAll(GAME *ptGame)
{
	INACTIVENODE **pptList;
	INACTIVENODE *ptNode;
	INACTIVENODE *ptNextNode;
	INACTIVEUNIT *ptUnit;
	INACTIVEUNIT *ptNextUnit;
	INACTIVEITEM *ptItem;
	INACTIVEITEM *ptNextItem;
	INACTIVEMONSTER *ptMonster;
	INACTIVEMONSTER *ptNextMonster;
	int nAct;

	pptList = ptGame->ptInactive;
	for (nAct = NUM_ACTS; nAct; nAct--, pptList++)
	{
		for (ptNextNode = *pptList; ptNextNode; )
		{
			ptNode = ptNextNode;
			ptNextNode = ptNode->ptNext;
			for (ptNextUnit = ptNode->ptUnits; ptNextUnit; )
			{
				ptUnit = ptNextUnit;
				ptNextUnit = ptUnit->ptNext;
#line 671
				D2FREEPOOL(ptGame->pMemPool, ptUnit);
			}
			for (ptItem = ptNode->ptItems; ptItem; ptItem = ptNextItem)
			{
				ptNextItem = ptItem->ptNext;
#line 681
				D2FREEPOOL(ptGame->pMemPool, ptItem);
			}
			for (ptNextMonster = ptNode->ptMonsters; ptNextMonster; )
			{
				ptMonster = ptNextMonster;
				ptNextMonster = ptMonster->ptNext;
				AiFreeMinionList(ptGame, ptMonster->pAiList);
				SUnitInactiveFreeMonster(ptGame, ptMonster);
			}
#line 688
			D2FREEPOOL(ptGame->pMemPool, ptNode);
		}
	}
}

void __fastcall SUnitInactiveRemoveUnit(GAME *ptGame, UNIT *hUnit)
{
	GAMEROOM *hRoom;
	UNIT *hOwner;
	struct MONSTERUNIQUE *ptUnique;
	MISSOBJECTSTXT *ptObj;
	DWORD eType, dwClassId, dwMode;
	BOOL fKeep, fDead;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	eType = hUnit->eType;
	dwClassId = hUnit->dwClassId;
	fKeep = FALSE;
	if (eType == UNIT_MONSTER)
	{
		hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
		dwMode = hUnit->dwMode;
		fKeep = FALSE;
		fDead = FALSE;
		if (SUnitIsDead(hUnit))
			fDead = TRUE;
		if (D2CommonDrlgGetLevelSaveMonsters(D2CommonDungeonRoomGetLevelId(hRoom)))
			fKeep = TRUE;
		if (hUnit->dwFlags0 & UNITFLAG0_KEEP)
			fKeep = TRUE;
		if (dwMode == MONMODE_DEAD)
		{
			if (hRoom)
				hRoom = (GAMEROOM *)&ROOM5(hRoom)->tSeed;
			else
				hRoom = NULL;
			if (D2CommonSeedRand(hRoom, 3))
				fKeep = FALSE;
		}
		if (fDead)
		{
			if (D2CommonStatsUnitTestStateGroupC(hUnit))
				fKeep = FALSE;
			if (!D2CommonStatsUnitGetAlignment(hUnit))
				fKeep = FALSE;
		}
		if (hUnit->nTargetListNo < TARGET_LISTNO_MAX_REGULAR)
			fKeep = FALSE;
		if (fDead && (hUnit->dwClassId == MONCLASS_16B || hUnit->dwClassId == MONCLASS_16C))
			fKeep = FALSE;
		ptUnique = MONDATA(hUnit)->ptUnique;
		if (MonsterUniqueIsCounted(ptUnique) && fDead)
			fKeep = FALSE;
		if (MonsterUniqueIsMinion(ptUnique) && fDead)
			fKeep = FALSE;
		if (hUnit->dwFlags0 & UNIT_MASK_PET)
		{
			hOwner = AiGetOwner(hUnit);
			if (hOwner && hOwner->eType == UNIT_PLAYER)
				PlayerPetsRemove(ptGame, hOwner, hUnit->dwId, TRUE);
			fKeep = FALSE;
		}
		if (hUnit->dwFlags0 & UNITFLAG0_0200)
			fKeep = FALSE;
		if (((MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId))->bRestore && fKeep)
			SUnitInactiveSave(ptGame, hUnit);
		SUnitFree(ptGame, hUnit);
		return;
	}
	if (D2CommonDrlgGetLevelSaveMonsters(D2CommonDungeonRoomGetLevelId(hRoom))
		|| (hUnit->dwFlags0 & UNITFLAG0_KEEP))
		fKeep = TRUE;
	switch (eType)
	{
	case UNIT_OBJECT:
		if (dwClassId == OBJCLASS_3B || dwClassId == OBJCLASS_3C)
		{
			EventRemoveByType(ptGame, hUnit, EVENT_1);
			goto held;
		}
		ptObj = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(dwClassId);
		if (!ptObj->Restore || (SUnitGetObjectFlags(hUnit) & 2))
			fKeep = FALSE;
		if (ptObj->RestoreVirgins && hUnit->dwMode)
			fKeep = FALSE;
		EventRemoveByType(ptGame, hUnit, EVENT_1);
		break;
	case UNIT_PLAYER:
		if (D2CommonStatsGetState(hUnit, UNITFLAG_7))
		{
			EventRemoveByType(ptGame, hUnit, EVENT_1);
			goto held;
		}
		break;
	case UNIT_ITEM:
	case UNIT_ROOMTILE:
		fKeep = TRUE;
		break;
	default:
		fKeep = FALSE;
		break;
	}
	if (fKeep)
		SUnitInactiveSave(ptGame, hUnit);
	if (eType != UNIT_ITEM)
		SUnitFree(ptGame, hUnit);
	return;

held:
	hUnit->dwFlags |= UNITFLAG_HELD;
	SUnitInactiveSave(ptGame, hUnit);
	D2CommonUnitRoomLeave(hUnit);
}

void __fastcall SUnitInactiveSave(GAME *ptGame, UNIT *hUnit  )
{
	GAMEROOM *hRoom;
	INACTIVEMONSTER *ptRecord;
	INACTIVENODE *ptNode;
	MONSTERDATA *ptMonData;
	struct MONSTERUNIQUE *ptUnique;
	MONSTATSTXT *ptStats;
	AICOMMAND *ptState;
	void **ppCell;
	DWORD dwClassId;
	DWORD dwOwnerId;
	DWORD dwKind;
	BOOL fFlag1;
	BOOL fFlag2;
	AIMINION *ptList;
	int nListNo;
	int nAlign;
	int *peMods;
	int i;

	if (hUnit->eType == UNIT_MONSTER)
	{
		hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
#line 1218
		ptRecord = (INACTIVEMONSTER *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(INACTIVEMONSTER));
		for (i = 0; i < (int)(sizeof(INACTIVEMONSTER) / sizeof(DWORD)); i++)
			((DWORD *)ptRecord)[i] = 0;
		ptRecord->dwOwnerId = (DWORD)-1;
		ptRecord->nX = (int)D2CommonUnitGetX(hUnit);
		ptRecord->nY = (int)D2CommonUnitGetY(hUnit);
		ptRecord->dwClassId = hUnit->dwClassId;
		ptRecord->nFrame = ptGame->nFrame;
		ptRecord->nAiKind = AiGetKind((AIRECORDHEAD *)MONDATA(hUnit)->ptAi);
		ptMonData = MONDATA(hUnit);
		ptUnique = ptMonData->ptUnique;
		ptRecord->wUniqueId = MonsterUniqueGetId(ptUnique);
		dwClassId = hUnit->dwClassId;
		ptStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(dwClassId);
		if (ptStats->bInteract || ptStats->bIsNPC || ptStats->bInTown)
		{
			ptState = AiFindOrAddCommand(ptGame, hUnit, AISTATE_A, FALSE);
			if (ptState && ptState->dwArg1)
			{
				ptRecord->field_64 = ptState->dwArg1;
				ptRecord->dwAiArg2 = ptState->dwArg2;
			}
			ppCell = AiGetCell(hUnit);
			if (*ppCell)
				D2CommonPresetInfoDetachFirstDword(ppCell, &ptRecord->dwAiCell);
		}
		if (dwClassId == MONCLASS_10B || (int)ptRecord->wUniqueId - UNIQUE_FIRSTSUPER == SUPERUNIQUE_6
			|| dwClassId == MONCLASS_F3)
		{
			ptState = AiFindOrAddCommand(ptGame, hUnit, AISTATE_A, FALSE);
			if (ptState)
			{
				ptRecord->field_64 = ptState->dwArg1;
				ptRecord->dwAiArg2 = ptState->dwArg2;
			}
		}
		if (dwClassId == MONCLASS_138)
			ptRecord->field_64 = hUnit->abComponentVariations[10];
		else if (dwClassId == MONCLASS_137)
			ptRecord->field_64 = hUnit->abComponentVariations[10];
		if (MonsterUniqueIsChampion(ptUnique))
			ptRecord->dwFlags |= IM_FIELD0C;
		if (MonsterUniqueIsUnique(ptUnique))
		{
			ptRecord->dwFlags |= IM_UNIQUE;
			QuestsReleasePickedMonster(ptGame, hUnit);
		}
		if (MonsterUniqueIsMinion(ptUnique))
			ptRecord->dwFlags |= IM_MINION;
		if (AiRecordTestFlags((AIRECORDHEAD *)ptMonData->pRegionRef, AIFLAG_02))
			ptRecord->dwFlags |= IM_NOCOUNT;
		nAlign = D2CommonStatsUnitGetAlignment(hUnit);
		if (nAlign == 0)
			ptRecord->dwFlags |= IM_ALIGN0;
		else if (nAlign == 2)
			ptRecord->dwFlags |= IM_ALIGN2;
		else if (nAlign == 1)
			ptRecord->dwFlags |= IM_ALIGN1;
		nListNo = UnitGetTargetListNo(hUnit);
#line 1271
		D2ASSERT(UnitGetTargetListNo (hUnit) >= TARGET_LISTNO_MAX_REGULAR);
		if (nListNo != TARGET_LISTNO_NULL)
			ptRecord->dwFlags |= IM_INLIST;
		ptRecord->nExperience = D2CommonStatsUnitGetValue(hUnit, STAT_EXPERIENCE);
		ptRecord->nMaxLife = D2CommonStatsUnitCalcMaxHp(hUnit);
		ptRecord->nLife = D2CommonStatsUnitGetValue(hUnit, STAT_LIFE);
		ptRecord->dwRegionLevel = MonsterRegionRefGetLevel(ptMonData->pRegionRef);
		peMods = MonsterUniqueGetMods(ptUnique);
		for (i = 0; i < NUM_UNIQUEMODS; i++)
			ptRecord->aeMods[i] = peMods[i];
		if (hUnit->dwMode == MONMODE_DEAD || hUnit->dwMode == MONMODE_DEATH)
			ptRecord->dwFlags |= IM_DEAD;
		AiTakeOwner(hUnit, &dwOwnerId, &dwKind, &fFlag1, &fFlag2, &ptList);
		if (dwOwnerId != (DWORD)-1 && dwKind == 1)
		{
			ptRecord->dwOwnerId = dwOwnerId;
			ptRecord->pAiList = ptList;
			if (fFlag1)
				ptRecord->dwFlags |= IM_AIFLAG1;
			if (fFlag2)
				ptRecord->dwFlags |= IM_AIFLAG2;
		}
		ptRecord->dwUnitId = hUnit->dwId;
		ptRecord->dwUnitFlags0 = hUnit->dwFlags0;
		ptRecord->dwUnitFlags = hUnit->dwFlags;
		ptNode = SUnitInactiveFindNode(ptGame, hRoom, TRUE);
		ptRecord->ptNext = ptNode->ptMonsters;
		ptNode->ptMonsters = ptRecord;
	}
	else
		SUnitInactiveSaveUnit(ptGame, hUnit);
}

void __fastcall SUnitInactiveSaveItem(GAME *ptGame, UNIT *hItem, INACTIVENODE *ptNode, INACTIVEITEM **pptOut)
{
	INACTIVEITEM *ptRecord;
	INACTIVEITEM **pptSlot;
	void *ptInventory;
	void *ptInvNode;
	void *ptNextNode;
	UNIT *hGem;
	int nQuantity;
	int nGemIndex;
	int i;

#line 448
	ptRecord = (INACTIVEITEM *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(INACTIVEITEM));
	for (i = 0; i < (int)(sizeof(INACTIVEITEM) / sizeof(DWORD)); i++)
		((DWORD *)ptRecord)[i] = 0;
	if (D2CommonITEMSGetType(hItem) == ITEMTYPE_EAR)
	{
		strcpy(ptRecord->szEarName, D2CommonITEMSGetNameBuffer(hItem));
		ptRecord->bEarLevel = D2CommonITEMSDataGetEarLevel(hItem);
	}
	ptRecord->nLevel = (int)D2CommonITEMSGetLevelId(hItem);
	ptRecord->dwClassId = hItem->dwClassId;
	ptRecord->dwMode = hItem->dwMode;
	ptRecord->dwQuality = D2CommonITEMSDataGetModType(hItem);
	if (D2CommonITEMSGetType(hItem) == ITEMTYPE_GOLD)
		ptRecord->nQuantity = D2CommonStatsUnitGetValue(hItem, STAT_GOLD);
	else
	{
		nQuantity = D2CommonStatsUnitGetValue(hItem, STAT_QUANTITY);
#line 465
		D2ASSERT(nQuantity >= 0);
		ptRecord->nQuantity = nQuantity;
	}
	ptRecord->nDurability = D2CommonStatsUnitGetValue(hItem, STAT_DURABILITY);
	ptRecord->nMaxDurability = D2CommonStatsUnitGetBaseValue(hItem, STAT_MAXDURABILITY);
	ptRecord->nX = (int)D2CommonUnitGetX(hItem);
	ptRecord->nY = (int)D2CommonUnitGetY(hItem);
	ptRecord->dwFileIndex = D2CommonITEMSDataGetUniqueOrSetRow(hItem);
	ptRecord->dwOwnerId = D2CommonITEMSDataGetOwnerId(hItem);
	ptRecord->dwSeed = hItem->dwInitSeed;
	ptRecord->dwItemSeed = D2CommonITEMSGetItemSeedValue(hItem);
	ptRecord->dwFlags = D2CommonITEMSDataGetFlags(hItem);
	ptRecord->nFrame = ptGame->nFrame;
	ptRecord->bQuest = ((ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId))->quest;
	for (i = 0; i < MAXIMUM_ATTACHED_ITEMS; i++)
		ptRecord->aptSocketed[i] = NULL;
	if (D2CommonITEMSGetTypeHasInv(hItem) && D2CommonITEMSGetTypeGemSockets(hItem) > 0
		&& D2CommonITEMSDataTestFlags(hItem, ITEMFLAG_SOCKETED))
	{
		ptInventory = hItem->ptInventory;
		nGemIndex = 0;
		ptInvNode = D2CommonINVENTORYGetCategoryListHead(ptInventory);
		if (ptInvNode)
		{
			pptSlot = ptRecord->aptSocketed;
			while (ptInvNode)
			{
				ptNextNode = (void *)D2CommonINVENTORYNodeGetNext(ptInvNode);
				hGem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptInvNode);
				D2CommonINVENTORYRemoveItem(ptInventory, hGem);
				nGemIndex++;
				SUnitInactiveSaveItem(ptGame, hGem, ptNode, pptSlot++);
#line 500
				D2ASSERT(nGemIndex<= MAXIMUM_ATTACHED_ITEMS);
				ptInvNode = ptNextNode;
			}
		}
	}
	if (pptOut)
		*pptOut = ptRecord;
	else
	{
		ptRecord->ptNext = ptNode->ptItems;
		ptNode->ptItems = ptRecord;
	}
	SUnitFree(ptGame, hItem);
}

void __fastcall SUnitInactiveRemoveClass(GAME *ptGame, WORD wType, WORD wClassId, BYTE bAct)
{
	INACTIVENODE *ptNode;
	INACTIVEMONSTER *ptMonster;
	INACTIVEMONSTER *ptPrevMonster;
	INACTIVEUNIT *ptUnit;
	INACTIVEUNIT *ptPrevUnit;

	if (wType == UNIT_MONSTER)
	{
		for (ptNode = ptGame->ptInactive[bAct]; ptNode; ptNode = ptNode->ptNext)
		{
			ptPrevMonster = NULL;
			for (ptMonster = ptNode->ptMonsters; ptMonster; ptMonster = ptMonster->ptNext)
			{
				if (ptMonster->dwClassId == wClassId)
				{
					if (ptMonster == ptNode->ptMonsters)
						ptNode->ptMonsters = ptMonster->ptNext;
					else
						ptPrevMonster->ptNext = ptMonster->ptNext;
					AiFreeMinionList(ptGame, ptMonster->pAiList);
					SUnitInactiveFreeMonster(ptGame, ptMonster);
					return;
				}
				ptPrevMonster = ptMonster;
			}
		}
		return;
	}
	for (ptNode = ptGame->ptInactive[bAct]; ptNode; ptNode = ptNode->ptNext)
	{
		ptPrevUnit = NULL;
		for (ptUnit = ptNode->ptUnits; ptUnit; ptUnit = ptUnit->ptNext)
		{
			if (ptUnit->eType == wType && ptUnit->dwClassId == wClassId)
			{
				if (ptUnit == ptNode->ptUnits)
					ptNode->ptUnits = ptUnit->ptNext;
				else
					ptPrevUnit->ptNext = ptUnit->ptNext;
#line 809
				D2FREEPOOL(ptGame->pMemPool, ptUnit);
				return;
			}
			ptPrevUnit = ptUnit;
		}
	}
}

INACTIVENODE *__fastcall SUnitInactiveFindNode(GAME *ptGame, GAMEROOM *hRoom, BOOL fCreate)
{
	GAMERECT atRect[2];
	INACTIVENODE *ptNode;
	INACTIVENODE *ptPrev;
	INACTIVENODE *ptNew;
	BOOL fFound;
	DWORD dwAct;
	int i;

	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	fFound = FALSE;
	dwAct = D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom));
	ptPrev = NULL;
	for (ptNode = ptGame->ptInactive[dwAct]; ptNode; ptNode = ptNode->ptNext)
	{
		if (atRect[0].nX == ptNode->nX && atRect[0].nY == ptNode->nY)
		{
			fFound = TRUE;
			break;
		}
		if (atRect[0].nX > ptNode->nX)
			break;
		ptPrev = ptNode;
	}
	if (!fCreate)
	{
		if (fFound)
			return ptNode;
		else
			return NULL;
	}
	if (!(fFound))
	{
#line 1055
		ptNew = (INACTIVENODE *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(INACTIVENODE));
		for (i = 0; i < (int)(sizeof(INACTIVENODE) / sizeof(DWORD)); i++)
			((DWORD *)ptNew)[i] = 0;
		ptNew->nX = atRect[0].nX;
		ptNew->nY = atRect[0].nY;
		ptNew->ptNext = ptNode;
		if (ptPrev)
			ptPrev->ptNext = ptNew; else
			ptGame->ptInactive[dwAct] = ptNew;
		ptNode = ptNew;
	}
	return ptNode;
}

void __fastcall SUnitInactiveRestoreAi(GAME *ptGame, UNIT *hUnit, INACTIVEMONSTER *ptRecord)
{
	MONSTATSTXT *ptStats;
	AICOMMAND *ptState;
	void **ppCell;
	DWORD *pdwCell;
	BYTE bVar;
	DWORD dwClassId;

	dwClassId = hUnit->dwClassId;
	ptStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(dwClassId);
	if (ptStats->bInteract || ptStats->bIsNPC || ptStats->bInTown)
	{
		ptState = AiFindOrAddCommand(ptGame, hUnit, AISTATE_A, FALSE);
		if (ptState)
		{
			ptState->dwArg1 = ptRecord->field_64;
			ptState->dwArg2 = ptRecord->dwAiArg2;
			ptState->dwArg3 = 0;
		}
		pdwCell = &ptRecord->dwAiCell;
		ppCell = AiGetCell(hUnit);
		if (*pdwCell)
			D2CommonPresetInfoDetachFirstDword(pdwCell, ppCell);
	}
	if (dwClassId == MONCLASS_10B || (int)ptRecord->wUniqueId - UNIQUE_FIRSTSUPER == SUPERUNIQUE_6
		|| dwClassId == MONCLASS_F3)
	{
		if (ptRecord->field_64 || ptRecord->dwAiArg2)
		{
			ptState = AiFindOrAddCommand(ptGame, hUnit, AISTATE_A, FALSE);
			if (ptState)
			{
				ptState->dwArg1 = ptRecord->field_64;
				ptState->dwArg2 = ptRecord->dwAiArg2;
			}
		}
	}
	if (dwClassId == MONCLASS_138)
	{
		bVar = (BYTE)ptRecord->field_64;
		hUnit->abComponentVariations[10] = bVar;
		hUnit->abComponentVariations[11] = bVar;
	}
	else if (dwClassId == MONCLASS_137)
		hUnit->abComponentVariations[10] = (BYTE)ptRecord->field_64;
}
