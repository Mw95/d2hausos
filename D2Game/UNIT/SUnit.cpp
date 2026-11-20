#include <stdio.h>
#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\UNIT\\SUnit.cpp"
#include "D2Macros.h"
#include "../GAME/Arena.h"
#include "../GAME/Clients.h"
#include "../GAME/Event.h"
#include "../GAME/Game.h"
#include "../GAME/SCmd.h"
#include "../GAME/Targets.h"
#include "../INVENTORY/InvMode.h"
#include "../ITEMS/ItemMode.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/Monster.h"
#include "../MONSTER/MonsterAI.h"
#include "../MONSTER/MonsterMsg.h"
#include "../OBJECTS/ObjMode.h"
#include "../OBJECTS/Objects.h"
#include "../PLAYER/Player.h"
#include "../PLAYER/PlayerPets.h"
#include "../PLAYER/PlrModes.h"
#include "../QUESTS/QUESTS.h"
#include "../QUESTS/a2q6.h"
#include "Party.h"
#include "PlayerList.h"
#include "SUnit.h"
#include "SUnitDmg.h"
#include "SUnitMsg.h"
#include "sunitproxy.h"
#include "../AI/AiGeneral.h"
#include "FogImports.h"

#define UNIT_PLAYER			0
#define UNIT_OBJECT			2
#define UNIT_MISSILE		3
#define UNIT_ITEM			4
#define UNIT_ROOMTILE		5
#define NUM_UNITTYPE		6
#define NUM_UNITBUCKETS		128
#define TARGET_LISTNO_NULL	11
#define PARTY_NONE			65535
#define UNITFLAG_7			7
#define LEVEL_7F			73
#define LEVEL_100			100
#define OBJECT_PRESET_INVALID	410
#define NUM_ALIGNMENT		3
#define ALIGNMENT_BAD		2
#define ALIGNMENT_UNSET		4
#define MONSTERCLASS_HYDRA1	351
#define MONSTERCLASS_HYDRA2	352
#define MONSTERCLASS_HYDRA3	353
#define UNIT_MASK_PET		0x80000000
#define UnitTestFlag(hUnit, dwMask)	((hUnit)->dwFlags0 & (dwMask))
#define STATE_ALIGNMENT		105
#define STAT_ALIGNMENT		172
#define STATE_36			54
#define STATE_5C			92
#define UNITFLAG_OWNED		131072
#define EVENT_MODE_FRAME	0
#define EVENT_MODE_END		1
#define EVENT_TYPE_2		2
#define SPAWN_FLAGS			7177
#define UNITFLAG_TELEPORTED	2048
#define UNITFLAG_10000		65536
#define PLAYERLIST_HOSTILE	8
#define INVNODE_EQUIPPED	3

static DWORD sgdwSeedSalt;

static const int scpnTypeOffsetTable[NUM_UNITTYPE] =
	{ 668, 1180, 1692, 2716, 2204, -1 };

extern UNIT **SUnitGetListHead(GAME *ptGame, int eTOU, DWORD dwId);
static inline void SUnitListInsert(GAME *ptGame, UNIT *ptUnit);

static void SUnitDropInteract(GAME *ptGame, UNIT *ptUnit)
{
	UNIT *hOther;
	DWORD dwOtherType;
	DWORD dwOtherId;

	dwOtherId = ptUnit->field_84;
	dwOtherType = ptUnit->field_88;
	if (ptUnit->field_8c)
	{
		hOther = SUnitFind(ptGame, dwOtherType, dwOtherId);
		if (hOther)
		{
			switch (hOther->eType)
			{
			case UNIT_MONSTER:
				MonsterAIRemoveThreat(ptGame, ptUnit, ((MONSTERDATA *)hOther->uTypeData)->pptThreats);
				SUnitProxyRemovePlayer(ptGame, ptUnit, hOther);
				break;
			case UNIT_OBJECT:
				ObjectModeReset(ptGame, hOther);
				break;
			}
			ptUnit->field_84 = (DWORD)-1;
			ptUnit->field_88 = NUM_UNITTYPE;
			ptUnit->field_8c = 0;
		}
	}
}

static void SUnitLeaveRoom(GAME *ptGame, UNIT *ptUnit)
{
	switch (ptUnit->eType)
	{
	case UNIT_PLAYER:
		SUnitDropInteract(ptGame, ptUnit);
	case UNIT_MONSTER:
		D2CommonPathClearIfExists(ptUnit);
		break;
	case UNIT_OBJECT:
		D2CommonPathClearIfExists(ptUnit);
		break;
	case UNIT_MISSILE:
		D2CommonPathClearIfExists(ptUnit);
		break;
	}
	D2CommonUnitRoomRemoveUnit(ptUnit);
}

static void SUnitFreePath(GAME *ptGame, UNIT *ptUnit)
{
	if (ptUnit->ptPath)
		D2CommonPathFreePool(ptGame->pMemPool, ptUnit->ptPath);
}

void __fastcall SUnitFree(GAME *ptGame  ,
	UNIT *ptUnit  )
{
	UNIT **pptHead;
	UNIT *ptUnitCurr;
	UNIT *ptUnitPrev;

#line 55
	D2ASSERT(ptGame != 0);
#line 58
	D2ASSERT(ptUnit);
#line 59
	D2ASSERTMSG((int)ptUnit->eType < NUM_UNITTYPE, "ptUnit->eType < NUM_UNITTYPE");
	SUnitLeaveRoom(ptGame, ptUnit);
	pptHead = SUnitGetListHead(ptGame, ptUnit->eType, ptUnit->dwId);
	ptUnitPrev = NULL;
	for (ptUnitCurr = *pptHead; ptUnitCurr; ptUnitCurr = ptUnitCurr->ptListNext)
	{
		if (ptUnitCurr == ptUnit)
			break;
		ptUnitPrev = ptUnitCurr;
	}
	if (ptUnitCurr)
	{
		if (!ptUnitPrev)
			*pptHead = ptUnitCurr->ptListNext;
		else
			ptUnitPrev->ptListNext = ptUnitCurr->ptListNext;
	}
	if (ptUnit->field_94)
	{
		QuestsFreeUnitLinks(ptGame, ptUnit->field_94);
		ptUnit->field_94 = NULL;
	}
	switch (ptUnit->eType)
	{
	case UNIT_PLAYER:
		TargetsFreeList(ptGame, ptUnit);
		PlayerRemovePortal(ptGame, ptUnit);
		PartyRemovePlayer(ptGame, ptUnit);
		PlayerFreeUnit(ptGame, ptUnit);
		SUnitFreePath(ptGame, ptUnit);
		break;
	case UNIT_MONSTER:
		TargetsRemove(ptGame, ptUnit);
		SUnitFreePath(ptGame, ptUnit);
		MonsterUnitFree(ptGame, ptUnit);
		break;
	case UNIT_OBJECT:
		ObjectFreeUnit(ptGame, ptUnit);
		D2CommonUnitFreeAuxBlock(ptUnit);
#line 93
		D2FREEPOOL(ptGame->pMemPool, ptUnit->uTypeData);
		break;
	case UNIT_MISSILE:
		SUnitFreePath(ptGame, ptUnit);
		MissilesUnitFree(ptGame, ptUnit);
		break;
	case UNIT_ITEM:
		ITEMSUnitFree(ptGame, ptUnit);
		D2CommonUnitFreeAuxBlock(ptUnit);
		D2CommonITEMSFreeItemData(ptGame->pMemPool, ptUnit);
		break;
	case UNIT_ROOMTILE:
		D2CommonUnitFreeAuxBlock(ptUnit);
		break;
	}
#line 125
	D2FREEPOOL(ptGame->pMemPool, ptUnit);
}

UNIT *__fastcall SUnitGetNextInList(UNIT *ptUnit  )
{
#line 149
	D2ASSERT(ptUnit);
	return ptUnit->ptListNext;
}

void __fastcall SUnitUpdateClients(GAME *ptGame  , UNIT *ptUnit
, GAMEROOM *hPrevRoom)
{
	GAMEROOM *hRoom;
	GAMEROOM *hActRoom;
	GAMEROOM *hLastRoom;
	void *hPath;
	DWORD dwLevelId;
	BYTE bAct;
	CLIENT **phClientsCurr;
	CLIENT **phClientsPrev;
	DWORD nClientsCurr;
	DWORD nClientsPrev;
	DWORD i;
	DWORD j;

	if (!D2CommonUnitGetRoomChanged(ptUnit))
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(ptUnit);
	if (!hPrevRoom)
	{
		hPath = ptUnit->ptPath;
		hLastRoom = (GAMEROOM *)D2CommonPathGetPrevRoom(hPath);
		dwLevelId = D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(ptUnit));
		if (dwLevelId)
		{
			bAct = D2CommonGetActNum(dwLevelId);
			for (hActRoom = (GAMEROOM *)D2CommonDungeonActGetFirstRoom(ptGame->ptAct[bAct]); hActRoom;
				hActRoom = hActRoom->ptActNext)
			{
				if (hActRoom == hLastRoom)
				{
					hPrevRoom = hLastRoom;
					goto found;
				}
			}
		}
		D2CommonPathClearPrevRoom(hPath);
		hPrevRoom = NULL;
	}
found:
	if (hPrevRoom && hRoom == hPrevRoom)
		return;
	if (ptUnit->eType == UNIT_MONSTER)
		((MONSTERDATA *)ptUnit->uTypeData)->ptArea = NULL;
	phClientsCurr = NULL;
	nClientsCurr = 0;
	if (hRoom)
	{
		phClientsCurr = hRoom->phClients;
		nClientsCurr = hRoom->nNumClients;
	}
	phClientsPrev = NULL;
	nClientsPrev = 0;
	if (hPrevRoom)
	{
		phClientsPrev = hPrevRoom->phClients;
		nClientsPrev = hPrevRoom->nNumClients;
	}
	i = 0;
	j = 0;
	while (i < nClientsCurr && j < nClientsPrev)
	{
#line 186
		D2ASSERT(phClientsCurr);
#line 187
		D2ASSERT(phClientsPrev);
		if ((DWORD)phClientsCurr[i] < (DWORD)phClientsPrev[j])
		{
			if (ptUnit != ClientGetPlayerUnit(phClientsCurr[i], FALSE))
				SUnitMsgSendUnit(ptGame, ptUnit, phClientsCurr[i]);
			i++;
		}
		else if ((DWORD)phClientsCurr[i] > (DWORD)phClientsPrev[j])
		{
			if (ptUnit != ClientGetPlayerUnit(phClientsPrev[j], FALSE))
				SUnitMsgSendRemove(ptUnit, phClientsPrev[j]);
			j++;
		}
		else
		{
			i++;
			j++;
		}
	}
	while (i < nClientsCurr)
	{
#line 207
		D2ASSERT(phClientsCurr);
		if (ptUnit != ClientGetPlayerUnit(phClientsCurr[i], FALSE))
			SUnitMsgSendUnit(ptGame, ptUnit, phClientsCurr[i]);
		i++;
	}
	while (j < nClientsPrev)
	{
#line 213
		D2ASSERT(phClientsPrev);
		if (ptUnit != ClientGetPlayerUnit(phClientsPrev[j], FALSE))
			SUnitMsgSendRemove(ptUnit, phClientsPrev[j]);
		j++;
	}
	D2CommonPathSetLeftRoom(ptUnit->ptPath, FALSE);
}

void __fastcall SUnitFreeTiles(GAME *ptGame  )
{
	UNIT *ptNext;
	UNIT *ptUnit;

#line 225
	D2ASSERT(ptGame);
	for (ptNext = ptGame->ptRoomTileList; ptNext; )
	{
		ptUnit = ptNext;
		ptNext = ptUnit->ptListNext;
		SUnitFree(ptGame, ptUnit);
	}
	ptGame->ptRoomTileList = NULL;
}

void __fastcall SUnitWarp(GAME *ptGame  ,
	UNIT *hTarget  ,
	UNIT *hTile)
{
	LVLWARPTXT *ptWarpDef;
	UNIT *hDestTile;
	GAMEROOM *hRoom;
	GAMEROOM *hSpawnRoom;
	COLPOINT tPoint;
	CLIENT *ptClient;
	BOOL fRet;
	int nX;
	int nY;
	int nWalkX;
	int nWalkY;
	BYTE bLife;
	DWORD dwLevelId;

#line 253
	D2ASSERT(ptGame);
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hTile);
	hDestTile = (UNIT *)D2CommonDungeonRoomGetWarpUnit(hRoom, hTile->dwClassId,
		&ptWarpDef);

	if (!hDestTile)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hDestTile);
	tPoint.nX = D2CommonUnitGetX(hDestTile);
	tPoint.nY = D2CommonUnitGetY(hDestTile);
#line 271
	D2ASSERT(UnitGetType( hTarget ) == UNIT_PLAYER);
	hSpawnRoom = (GAMEROOM *)D2CommonCollisnFindSpawnPointNear(hRoom, &tPoint,
		D2CommonUnitGetSizeX(hTarget), 7177, 1);
	if (!hSpawnRoom)
		return;
	dwLevelId = D2CommonDungeonRoomGetLevelId(hSpawnRoom);
	if (dwLevelId == LEVEL_7F || dwLevelId == LEVEL_100)
	{
		if (A2Q6TestLevel(ptGame, hTarget, dwLevelId))
			return;
	}
	nX = tPoint.nX;
	nY = tPoint.nY;
	fRet = SUnitMoveToRoom(ptGame, hTarget, hRoom, nX, nY, 0, 0);
#line 292
	D2ASSERT(fRet);
	ptClient = SUnitGetClient(hTarget);
	nWalkX = ptWarpDef->ExitWalkX + nX;
	nWalkY = ptWarpDef->ExitWalkY + nY;
	PlrModeStartXY(ptGame, hTarget, NULL, 2, nWalkX, nWalkY, FALSE);
	bLife = (BYTE)(D2CommonStatsUnitGetValue(hTarget, 6) * 100 / D2CommonStatsUnitCalcMaxHp(hTarget));
	SCmdSendUnitModeAt(ptClient, S2C_UNIT_MODE_POS, (BYTE)hTarget->eType, hTarget->dwId, 1, (WORD)nWalkX,
		(WORD)nWalkY, 0,
		bLife);
}

void __fastcall SUnitInitSeed(UNIT *ptUnit  , D2SEED *ptSeed)
{
#line 329
	D2ASSERT(ptUnit);
	D2CommonSeedInit(&ptUnit->tSeed);
	if (ptSeed)
		ptUnit->dwInitSeed = D2CommonSeedNext(ptSeed);
	else
		ptUnit->dwInitSeed = D2CommonSeedInvent(sgdwSeedSalt++ * 2);
	D2CommonSeedSet(&ptUnit->tSeed, ptUnit->dwInitSeed);
}

extern void __fastcall SUnitSeedItem(UNIT *hItem);
__inline void __fastcall SUnitSeedItem(UNIT *hItem)
{
	D2CommonITEMSAdvanceItemSeed(hItem);
}

UNIT *__fastcall SUnitCreate(DWORD eType, DWORD dwClassId,
	int nX,
	int nY, GAME *ptGame  , GAMEROOM *hRoom,
	DWORD dwFlags, DWORD dwMode, DWORD dwUnitId)
{
	UNIT *ptUnit;
	OBJECTDATA *ptObjectData;
	D2SEED *ptSeed;
	DWORD dwSeed;
	DWORD dwItemSeed;
	int i;

#line 362
	D2ASSERT(ptGame != 0);
#line 365
	ptUnit = (UNIT *)D2ALLOCPOOL(ptGame->pMemPool, 268);
#line 366
	D2ASSERT(ptUnit);
	for (i = 0; i < 268 / (int)sizeof(DWORD); i++)
		((DWORD *)ptUnit)[i] = 0;
	ptUnit->dwClassId = dwClassId;
	ptUnit->eType = eType;
	ptUnit->dwMemPool = (DWORD)ptGame->pMemPool;
	ptUnit->field_a0 = ptGame;
	ptUnit->bAct = D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom));
	ptUnit->field_94 = NULL;
	if (eType != UNIT_PLAYER)
	{
		ptSeed = NULL;
		if (hRoom)
			ptSeed = &ROOM5(hRoom)->tSeed;
		SUnitInitSeed(ptUnit, ptSeed);
		if (eType == UNIT_ITEM)
		{
			D2CommonITEMSCreateItemData(ptGame->pMemPool, ptUnit);
			SUnitSeedItem(ptUnit);
			if (hRoom)
				dwSeed = D2CommonSeedNext(&ROOM5(hRoom)->tSeed);
			else
				dwSeed = D2CommonSeedInvent(sgdwSeedSalt++ * 2);
			D2CommonITEMSSetItemSeedValue(ptUnit, dwSeed);
			dwItemSeed = D2CommonITEMSGetItemSeedValue(ptUnit);
			D2CommonSeedSet(D2CommonITEMSGetItemSeed(ptUnit), dwItemSeed);
		}
	}
	ptUnit->dwFlags0 |= 16;
	ptUnit->nTargetListNo = TARGET_LISTNO_NULL;
	switch (eType)
	{
	case UNIT_PLAYER:
		PlayerInitUnit(ptGame, ptUnit, ptGame->field_60);
		if (dwMode != PLRMODE_DEAD)
			TargetsNewList(ptGame, ptUnit, 0);
		if (++ptGame->field_60 == (DWORD)-1)
			ptGame->field_60 = 0;
		break;
	case UNIT_MONSTER:
		ptUnit->dwMode = dwMode;
		if (dwFlags & 2)
			MonsterInitUnit(ptGame, hRoom, ptUnit, dwUnitId);
		else
		{
			MonsterInitUnit(ptGame, hRoom, ptUnit, ptGame->field_260);
			if (++ptGame->field_260 == (DWORD)-1)
				ptGame->field_260 = 0;
		}
		break;
	case UNIT_OBJECT:
#line 415
		ptObjectData = (OBJECTDATA *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(OBJECTDATA));
		ptUnit->uTypeData = ptObjectData;
		for (i = 0; i < (int)(sizeof(OBJECTDATA) / sizeof(DWORD)); i++)
			((DWORD *)ptObjectData)[i] = 0;
		((OBJECTDATA *)ptUnit->uTypeData)->ptTypeData = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(dwClassId);
		ptUnit->dwMode = dwMode;
		D2CommonUnitAllocAuxBlock(ptUnit);
		ObjectInitUnit(ptGame, ptUnit, ptGame->field_264, hRoom, nX, nY);
		if (++ptGame->field_264 == (DWORD)-1)
			ptGame->field_264 = 0;
		break;
	case UNIT_ITEM:
		ptUnit->dwMode = dwMode;
		D2CommonUnitAllocAuxBlock(ptUnit);
		ITEMSInitUnit(ptUnit, ptGame, ptGame->field_268, (DWORD)hRoom);
		ptGame->field_268++;
		break;
	case UNIT_MISSILE:
		ptUnit->dwMode = dwMode;
		MissilesUnitInit(ptGame, ptUnit, ptGame->field_26c);
		ptGame->field_26c++;
		break;
	case UNIT_ROOMTILE:
		D2CommonUnitAllocAuxBlock(ptUnit);
		ptUnit->dwId = ptGame->field_270;
		ptUnit->dwFlags0 |= 2;
		ptGame->field_270++;
		break;
	}
	if (dwFlags & 1)
		SUnitPlace(ptUnit, nX, nY, ptGame, hRoom, TRUE);
	return ptUnit;
}

__inline UNIT **SUnitGetListHead(GAME *ptGame  ,
	int eTOU  ,
	DWORD dwId)
{
	DWORD dwBucket;

	if (eTOU == UNIT_ROOMTILE)
		return &ptGame->ptRoomTileList;
	dwBucket = dwId & (NUM_UNITBUCKETS - 1);
#line 474
	D2ASSERT(scpnTypeOffsetTable[ eTOU ] != -1);
	return (UNIT **)((BYTE *)ptGame + scpnTypeOffsetTable[eTOU]) + dwBucket;
}

UNIT *__fastcall SUnitFind(GAME *ptGame  , DWORD eType,
	DWORD dwId)
{
	UNIT *ptUnit;

#line 487
	D2ASSERT(ptGame);
	for (ptUnit = *SUnitGetListHead(ptGame, eType, dwId); ptUnit; ptUnit = ptUnit->ptListNext)
	{
		if (ptUnit->dwId == dwId)
			return ptUnit;
	}
	return NULL;
}

UNIT *__fastcall SUnitGetOwner(GAME *ptGame  ,
	UNIT *ptUnit  )
{
#line 513
	D2ASSERT(ptUnit);
	if (!(ptUnit->dwFlags & 1024))
		return NULL;
	return SUnitFind(ptGame, ptUnit->dwLinkedUnitType, ptUnit->dwLinkedUnitId);
}

static inline void SUnitListInsert(GAME *ptGame  , UNIT *ptUnit
)
{
	UNIT **pptHead;
	UNIT *ptUnitCurr;
	UNIT *ptUnitPrev;

	pptHead = SUnitGetListHead(ptGame, ptUnit->eType, ptUnit->dwId);
	ptUnitPrev = NULL;
	for (ptUnitCurr = *pptHead; ptUnitCurr; ptUnitCurr = ptUnitCurr->ptListNext)
	{
		if (ptUnitCurr->dwId <= ptUnit->dwId)
			break;
		ptUnitPrev = ptUnitCurr;
	}
#line 549
	D2ASSERTMSG(!ptUnitCurr || (ptUnitCurr->dwId != ptUnit->dwId),
		"!ptUnitCurr || (ptUnitCurr->dwId != ptUnit->dwId)");
	ptUnit->ptListNext = ptUnitCurr;
	if (ptUnitPrev)
		ptUnitPrev->ptListNext = ptUnit;
	else
		*pptHead = ptUnit;
}

void __fastcall SUnitPlace(UNIT *ptUnit  , int nX,
	int nY,
	GAME *ptGame  , GAMEROOM *hRoom, BOOL bNoFlag)
{
	MISSOBJECTSTXT *ptObjTxt;
	GAMEROOM *hPathRoom;

#line 656
	D2ASSERT(ptGame);
#line 660
	D2ASSERT(ptUnit);
	ptUnit->bAct = D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom));
	switch (ptUnit->eType)
	{
	case UNIT_PLAYER:
		D2CommonPathCreate(ptGame->pMemPool, hRoom, nX, nY, ptUnit, 0);
		PlayerStartMode(ptGame, ptUnit, nX, nY);
		break;
	case UNIT_MONSTER:
		D2CommonPathCreate(ptGame->pMemPool, hRoom, nX, nY, ptUnit, 0);
		MonsterStartMode(ptGame, ptUnit, nX, nY, bNoFlag);
		hPathRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(ptUnit);
		if (hPathRoom->nNumClients)
			MonsterRestartAi(ptGame, ptUnit);
		else
			SUnitReset(ptGame, ptUnit);
		break;
	case UNIT_OBJECT:
		ptObjTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(ptUnit->dwClassId);
		D2CommonUnitInitPosition(0, ptUnit, hRoom, nX, nY);
		if (ptObjTxt->HasCollision[ptUnit->dwMode])
			D2CommonUnitSetCollision(ptUnit, hRoom, nX, nY);
		D2CommonAddUnitToRoom(ptUnit, hRoom);
		break;
	case UNIT_ITEM:
		if (ptUnit->dwMode == 3)
		{
			D2CommonUnitInitPosition(0, ptUnit, hRoom, nX, nY);
			D2CommonUnitSetCollision(ptUnit, hRoom, nX, nY);
			D2CommonAddUnitToRoom(ptUnit, hRoom);
		}
		break;
	case UNIT_MISSILE:
		D2CommonPathCreate(ptGame->pMemPool, hRoom, nX, nY, ptUnit, 0);
		break;
	case UNIT_ROOMTILE:
		D2CommonUnitInitPosition(0, ptUnit, hRoom, nX, nY);
		D2CommonUnitSetCollision(ptUnit, hRoom, nX, nY);
		D2CommonAddUnitToRoom(ptUnit, hRoom);
		break;
	default:
#line 713
		D2ASSERT(0);
	}
	SUnitListInsert(ptGame, ptUnit);
	if (D2CommonUnitGetPathRoom(ptUnit))
		D2CommonUnitRoomAddToChangedList(ptUnit);
	if (!bNoFlag)
		D2CommonUnitSetRoomChanged(ptUnit, 1);
}

void __fastcall SUnitReplace(GAME *ptGame  ,
	UNIT *hUnit  ,
	GAMEROOM *hRoom, int nX, int nY)
{
	MISSOBJECTSTXT *ptObjTxt;

#line 734
	D2ASSERT(ptGame);
#line 735
	D2ASSERT(hUnit);
	hUnit->bAct = D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom));
	hUnit->dwFlags &= ~256;
	switch (hUnit->eType)
	{
	case UNIT_PLAYER:
		D2CommonPathFreePool(ptGame->pMemPool, hUnit->ptPath);
		D2CommonPathCreate(ptGame->pMemPool, hRoom, nX, nY, hUnit, 0);
		PlayerStartMode(ptGame, hUnit, nX, nY);
		break;
	case UNIT_MONSTER:
		D2CommonPathFreePool(ptGame->pMemPool, hUnit->ptPath);
		D2CommonPathCreate(ptGame->pMemPool, hRoom, nX, nY, hUnit, 0);
		MonsterStartMode(ptGame, hUnit, nX, nY, 0);
		if (((GAMEROOM *)D2CommonUnitGetPathRoom(hUnit))->nNumClients)
			MonsterRestartAi(ptGame, hUnit);
		else
			SUnitReset(ptGame, hUnit);
		break;
	case UNIT_OBJECT:
		ptObjTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hUnit->dwClassId);
		D2CommonUnitInitPosition(0, hUnit, hRoom, nX, nY);
		if (ptObjTxt->HasCollision[hUnit->dwMode])
			D2CommonUnitSetCollision(hUnit, hRoom, nX, nY);
		D2CommonAddUnitToRoom(hUnit, hRoom);
		break;
	case UNIT_ITEM:
		if (hUnit->dwMode == 3)
		{
			D2CommonUnitInitPosition(0, hUnit, hRoom, nX, nY);
			D2CommonUnitSetCollision(hUnit, hRoom, nX, nY);
			D2CommonAddUnitToRoom(hUnit, hRoom);
		}
		break;
	case UNIT_MISSILE:
		D2CommonPathFreePool(ptGame->pMemPool, hUnit->ptPath);
		D2CommonPathCreate(ptGame->pMemPool, hRoom, nX, nY, hUnit, 0);
		break;
	case UNIT_ROOMTILE:
		D2CommonUnitInitPosition(0, hUnit, hRoom, nX, nY);
		D2CommonUnitSetCollision(hUnit, hRoom, nX, nY);
		D2CommonAddUnitToRoom(hUnit, hRoom);
		break;
	default:
#line 795
		D2FATAL("Illegal unit type");
	}
	if (D2CommonUnitGetPathRoom(hUnit))
		D2CommonUnitRoomAddToChangedList(hUnit);
	D2CommonUnitSetRoomChanged(hUnit, 1);
}

void __fastcall SUnitSetClient(UNIT *ptUnit  , CLIENT *ptClient)
{
#line 811
	D2ASSERT(ptUnit);
#line 812
	D2ASSERTMSG(UnitGetType(ptUnit) == UNIT_PLAYER, "UnitGetType(hUnit) == UNIT_PLAYER");
	((PLRINFO *)D2CommonUnitGetPlrInfo(ptUnit))->ptClient = ptClient;
}

CLIENT *__fastcall SUnitGetClient(UNIT *ptUnit  )
{
#line 823
	D2ASSERT(ptUnit);
#line 824
	D2ASSERTMSG(UnitGetType(ptUnit) == UNIT_PLAYER, "UnitGetType(hUnit) == UNIT_PLAYER");
	return ((PLRINFO *)D2CommonUnitGetPlrInfo(ptUnit))->ptClient;
}

void __fastcall SUnitReset(GAME *ptGame  ,
	UNIT *ptUnit  )
{
#line 844
	D2ASSERT(ptUnit);
	D2CommonStatsUnitClearFieldEc(ptUnit);
	ptUnit->dwFlags0 &= ~17;
	D2CommonUnitSetRoomChanged(ptUnit, 0);
	ptUnit->dwFlags0 &= ~33792;
	ptUnit->dwFlags &= 0xFEDEE7FF;
	D2CommonStatsUnitClearActiveStates(ptUnit);
	INVENTORYModeResetItems(ptGame, ptUnit);
	if (D2CommonStatsTestHasOverlay(ptUnit))
		D2CommonStatsUnitClearHasOverlay(ptUnit);
	if (D2CommonStatsTestHasOverlay(ptUnit))
		D2CommonStatsUnitClearHasOverlay(ptUnit);
	switch (ptUnit->eType)
	{
	case UNIT_PLAYER:
		D2CommonStatsUnitSetValue(ptUnit, 29, -1);
		ptUnit->dwFlags0 &= ~256;
		ArenaUnitClear(ptGame, ptUnit);
		break;
	case UNIT_OBJECT:
		ptUnit->dwFlags0 &= ~256;
		break;
	case UNIT_MONSTER:
		ptUnit->dwFlags0 &= ~2304;
		break;
	case UNIT_MISSILE:
		break;
	case UNIT_ITEM:
		ptUnit->dwFlags0 &= ~4096;
		D2CommonITEMSDataSetFlags(ptUnit, 32, 0);
		D2CommonITEMSDataSetFlags(ptUnit, 8192, 0);
		break;
	}
}

void __fastcall SUnitSetEvent(UNIT *ptUnit  , WORD wEvent,
	UNIT *hOther)
{
#line 900
	D2ASSERT(ptUnit);
	ptUnit->wEvent = wEvent;
	ptUnit->field_90 = hOther;
	D2CommonUnitRoomAddToChangedList(ptUnit);
	ptUnit->dwFlags0 |= 1024;
}

void __fastcall SUnitResetAnim(UNIT *ptUnit  )
{
#line 910
	D2ASSERT(ptUnit);
	ptUnit->bAnimEvent = 0;
	if (ptUnit->eType == UNIT_PLAYER)
	{
		if (ptUnit->dwMode == 18)
		{
			D2CommonUnitInitAnimState(ptUnit);
			return;
		}
		ptUnit->hAnimSequence = NULL;
	}
	if (ptUnit->eType == UNIT_MONSTER)
	{
		if (ptUnit->dwMode == 14)
		{
			D2CommonUnitInitAnimState(ptUnit);
			return;
		}
		ptUnit->hAnimSequence = NULL;
	}
	ptUnit->nAnimPos = 0;
	if (ptUnit->ptPath)
		D2CommonUnitUpdateAnimRate(ptUnit);
	else
		D2CommonUnitSetAnimRecord(ptUnit, ptUnit->eType, ptUnit->dwClassId, ptUnit->dwMode);
	ptUnit->nAnimLength = ptUnit->ptAnimRecord->dwFrames << 8;
}

UNIT *__fastcall SUnitSpawnPreset(GAME *ptGame  , GAMEROOM *hRoom,
	int eType, int nClass, int nX, int nY,
	DWORD dwMode, DWORD dwUnused)
{
	UNIT *hUnit;
	ITEMCREATE tCreate;
	int i;

	hUnit = NULL;
	switch (eType)
	{
	case UNIT_MONSTER:
		hUnit = MonsterSpawnClass(ptGame, hRoom, nClass, nX, nY, dwMode);
		if (hUnit)
			hUnit->dwFlags0 |= 50331648;
		return hUnit;
	case UNIT_OBJECT:
		if (nClass > OBJECT_PRESET_INVALID)
		{
			hUnit = ObjectPresetCreate(ptGame, hRoom, nClass, nX, nY, dwMode);
			if (hUnit)
				hUnit->dwFlags0 |= 50331648;
			return hUnit;
		}
		if (nClass == OBJECT_PRESET_INVALID)
		{
			FogErrorManagerLog(" could not place object due to invalid preset data\n");
			return NULL;
		}
	default:
		hUnit = SUnitCreate(eType, nClass, nX, nY, ptGame, hRoom, 1, dwMode, 0);
		if (eType == UNIT_ITEM)
		{
			if (!hUnit)
				return hUnit;
			for (i = 0; i < (int)(sizeof(ITEMCREATE) / sizeof(DWORD)); i++)
				((DWORD *)&tCreate)[i] = 0;
			tCreate.dwQuality = 2;
			tCreate.nLevel = D2CommonDungeonRoomGetLevelId(hRoom);
			ITEMSInitStats(ptGame, &hUnit, &tCreate);
		}
		break;
	}
	if (hUnit)
		hUnit->dwFlags0 |= 50331648;
	return hUnit;
}

void __fastcall SUnitSpawnRoomPresets(GAME *ptGame  ,
	GAMEROOM *hRoom)
{
	PRESETUNIT *ptFirst;
	PRESETUNIT *ptPreset;
	UNIT *hUnit;
	void *pPathCell;
	int i;

#line 1110
	D2ASSERT(ptGame);
	ptFirst = (PRESETUNIT *)D2CommonDungeonRoomGetPresetUnits(hRoom);
	for (ptPreset = ptFirst; ptPreset; ptPreset = ptPreset->ptNext)
	{
		if (ptPreset->nUnitType == UNIT_MONSTER || (ptPreset->dwFlags & 1))
			continue;
		{
		GAMERECT atRects[2] = { 0 };
		D2CommonDungeonRoomCopyRects(hRoom, atRects);
			int nX = ptPreset->nX + atRects[0].nX;
			int nY = ptPreset->nY + atRects[0].nY;
			hUnit = SUnitSpawnPreset(ptGame, hRoom, ptPreset->nUnitType, ptPreset->nClassId,
				nX, nY, ptPreset->nMode, 0);
			if (hUnit && ptPreset->nUnitType == UNIT_MONSTER)
			{
				void **ppPath = &ptPreset->ptPath;
				pPathCell = AiGetCell(hUnit);
				if (*ppPath)
					D2CommonPresetInfoDetachFirstDword(ppPath, pPathCell);
			}
		}
	}
	for (ptPreset = ptFirst; ptPreset; ptPreset = ptPreset->ptNext)
	{
		if (ptPreset->nUnitType != UNIT_MONSTER || (ptPreset->dwFlags & 1))
			continue;
		{
		GAMERECT atRects[2] = { 0 };
		D2CommonDungeonRoomCopyRects(hRoom, atRects);
			int nX = ptPreset->nX + atRects[0].nX;
			int nY = ptPreset->nY + atRects[0].nY;
			hUnit = SUnitSpawnPreset(ptGame, hRoom, ptPreset->nUnitType, ptPreset->nClassId,
				nX, nY, ptPreset->nMode, 0);
			if (hUnit && ptPreset->nUnitType == UNIT_MONSTER)
			{
				void **ppPath = &ptPreset->ptPath;
				pPathCell = AiGetCell(hUnit);
				if (*ppPath)
					D2CommonPresetInfoDetachFirstDword(ppPath, pPathCell);
			}
		}
	}
}

void __fastcall SUnitUpdateTarget(GAME *ptGame  ,
	UNIT *hUnit  )
{
	void *hPath;
	DWORD dwTargetType;
	DWORD dwTargetId;
	UNIT *hTarget;

#line 1143
	D2ASSERT(hUnit);
#line 1144
	D2ASSERT(ptGame);
	hPath = hUnit->ptPath;
#line 1147
	D2ASSERT(hPath);
	if (!D2CommonPathGetTarget(hPath))
		return;
	D2CommonPathGetTargetSnapshot(hPath, &dwTargetType, &dwTargetId);
	hTarget = SUnitFind(ptGame, dwTargetType, dwTargetId);
	if ((DWORD)hTarget != D2CommonPathGetTarget(hPath))
		D2CommonPathSetTarget(hPath, 0);
	if (hTarget && hTarget->eType == UNIT_ITEM && (hTarget->dwMode == 2 || hTarget->dwMode == 1))
		D2CommonPathSetTarget(hPath, 0);
}

UNIT *__stdcall SUnitGetTarget(GAME *ptGame  ,
	UNIT *hUnit  )
{
	void *ptPath;

	SUnitUpdateTarget(ptGame, hUnit);
	ptPath = hUnit->ptPath;
	return (UNIT *)D2CommonPathGetTarget(ptPath);
}

int __fastcall SUnitStepAnim(GAME *ptGame  ,
	UNIT *hUnit  )
{
	int nRet;

#line 1184
	D2ASSERT(ptGame);
	SUnitUpdateTarget(ptGame, hUnit);
	nRet = D2CommonStepUnitWalk(hUnit, 1024) ? 1 : 2;
	if (D2CommonUnitGetRoomChanged(hUnit))
		SUnitUpdateClients(ptGame, hUnit, NULL);
	return nRet;
}

void __fastcall SUnitSetMode(GAME *ptGame  ,
	UNIT *hUnit  ,
	DWORD dwMode)
{
	SUnitDmgFreeHeld(ptGame, hUnit);
	D2CommonUnitSetMode(hUnit, dwMode);
}

void __fastcall SUnitLinkPortals(UNIT *ptFirst  , UNIT *ptSecond
)
{
	GAMERECT atRects[2];
	GAMEROOM *hRoom;

#line 1221
	D2ASSERT(ptFirst);
#line 1223
	D2ASSERT(ptSecond);
	OBJDATA(ptFirst)->nLinkedX = D2CommonUnitGetX(ptSecond);
	OBJDATA(ptFirst)->nLinkedY = D2CommonUnitGetY(ptSecond);
	OBJDATA(ptSecond)->nLinkedX = D2CommonUnitGetX(ptFirst);
	OBJDATA(ptSecond)->nLinkedY = D2CommonUnitGetY(ptFirst);
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(ptSecond);
	if (hRoom)
	{
		D2CommonDungeonRoomCopyRects(hRoom, atRects);
		OBJDATA(ptFirst)->nTargetX = atRects[1].nX;
		OBJDATA(ptFirst)->nTargetY = atRects[1].nY;
	}
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(ptFirst);
	if (hRoom)
	{
		D2CommonDungeonRoomCopyRects(hRoom, atRects);
		OBJDATA(ptSecond)->nTargetX = atRects[1].nX;
		OBJDATA(ptSecond)->nTargetY = atRects[1].nY;
	}
}

UNIT *__fastcall SUnitGetPortalTarget(GAME *ptGame  , UNIT *ptPortal
)
{
	DWORD dwAct;
	int nX;
	int nY;
	BYTE bLevel;
	GAMEROOM *hRoom;

#line 1276
	D2ASSERT(ptPortal);
	bLevel = OBJDATA(ptPortal)->field_04;
	dwAct = D2CommonGetActNum(bLevel);
	nX = OBJDATA(ptPortal)->nTargetX;
	nY = OBJDATA(ptPortal)->nTargetY;
	if (!D2CommonDungeonActFindRoomByPoint(ptGame->ptAct[dwAct], nX, nY))
	{
		hRoom = (GAMEROOM *)D2CommonDungeonActGetLevelRoom(ptGame->ptAct[dwAct], nX, nY);
		GameRespawnRoom(ptGame, hRoom);
	}
	return SUnitFind(ptGame, ptPortal->dwLinkedUnitType, ptPortal->dwLinkedUnitId);
}

UNIT *__fastcall SUnitIterate(GAME *ptGame  , DWORD eType,
	void *pArg, PFNSUNITITERATE pfnIterate  )
{
	UNIT **pptList;
	UNIT *ptUnit;
	int i;

#line 1314
	D2ASSERT(ptGame);
#line 1315
	D2ASSERT(pfnIterate);
	if (eType == UNIT_PLAYER)
		pptList = ptGame->pUnitList[UNIT_PLAYER];
	else if (eType == UNIT_MONSTER)
		pptList = ptGame->pUnitList[UNIT_MONSTER];
	else
		return NULL;
#line 1335
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)pfnIterate), "pfnIterate");
	for (i = 0; i < NUM_UNITBUCKETS; pptList++, i++)
	{
		for (ptUnit = *pptList; ptUnit; ptUnit = SUnitGetNextInList(ptUnit))
		{
			if (!D2CommonStatsGetState(ptUnit, UNITFLAG_7) && pfnIterate(ptGame, ptUnit, pArg) == 1)
				return ptUnit;
		}
	}
	return NULL;
}

void __fastcall SUnitForEachPlayer(GAME *ptGame  ,
	PFNSUNITCALLBACK pfnIterate
, void *pArg)
{
	UNIT *ptUnit;
	UNIT **pptList;
	int i;

	pptList = ptGame->pUnitList[UNIT_PLAYER];
#line 1366
	D2ASSERT(ptGame && pfnIterate);
	for (i = 0; i < NUM_UNITBUCKETS; pptList++, i++)
	{
		for (ptUnit = *pptList; ptUnit; ptUnit = SUnitGetNextInList(ptUnit))
		{
			if (!D2CommonStatsGetState(ptUnit, UNITFLAG_7))
				pfnIterate(ptGame, ptUnit, pArg);
		}
	}
}

void __fastcall UnitSetEventList(UNIT *ptUnit  ,
	struct EVENT **phEvents)
{
#line 1390
	D2ASSERT(ptUnit);
	ptUnit->hUnitEvents = phEvents;
}

struct EVENT **__fastcall UnitGetEventList(UNIT *ptUnit  )
{
#line 1398
	D2ASSERT(ptUnit);
	return ptUnit->hUnitEvents;
}

void __fastcall SUnitClearModeEvents(GAME *ptGame  ,
	UNIT *hUnit  )
{
	EventRemoveByType(ptGame, hUnit, EVENT_MODE_FRAME);
	EventRemoveByType(ptGame, hUnit, EVENT_MODE_END);
}

void __fastcall SUnitScheduleModeEvents(GAME *ptGame  ,
	UNIT *hUnit  )
{
	struct UNITANIMREC *ptAnimRec;
	int nEventFrame;
	int nPos;
	int nPrevPos;
	int nNextPos;
	int nAnimRate;
	int nAnimLength;
	int nLastFrame;
	int nAnimFrame;
	int nEvent;

	nEventFrame = ptGame->nFrame;
	nPos = D2CommonUnitGetWClassModeValue(hUnit) << 8;
	nPrevPos = nPos;
	if (hUnit->hAnimSequence)
	{
		nAnimRate = hUnit->nSeqRate;
		nAnimLength = hUnit->nSeqLength;
	}
	else
	{
		nAnimRate = hUnit->nAnimRate;
		nAnimLength = hUnit->nAnimLength;
	}
#line 1433
	D2ASSERT(nAnimRate);
	ptAnimRec = hUnit->ptAnimRecord;
	nLastFrame = -1;
	nNextPos = nAnimRate + nPos;
	while (nNextPos < nAnimLength)
	{
		for (nAnimFrame = nPrevPos >> 8; nAnimFrame <= nPos >> 8; nAnimFrame++)
		{
			if (hUnit->hAnimSequence)
				D2CommonTokTblsGetFrameClass(hUnit->hAnimSequence, nAnimFrame << 8, &nEvent);
			else
				nEvent = ptAnimRec->abFrameEvents[nAnimFrame];
			if (nEvent && nLastFrame != nAnimFrame && nEvent > 0 && nEvent <= 4)
			{
				nLastFrame = nAnimFrame;
				if (nEventFrame == ptGame->nFrame)
					nEventFrame++;
				EventAdd(ptGame, hUnit, EVENT_MODE_FRAME, nEventFrame);
			}
		}
		nPrevPos = nPos;
		nPos += nAnimRate;
		nEventFrame++;
		nNextPos = nAnimRate + nPos;
	}
	if (nEventFrame == ptGame->nFrame)
		nEventFrame++;
	EventAdd(ptGame, hUnit, EVENT_MODE_END, nEventFrame);
}

void __fastcall SUnitSetStandingEvent(GAME *ptGame  ,
	UNIT *hUnit  )
{
	EventRemoveByType(ptGame, hUnit, EVENT_MODE_FRAME);
	EventAddPersistent(ptGame, hUnit, EVENT_MODE_FRAME);
}

void __fastcall SUnitGetInfo(UNIT *ptUnit  ,
	GAMEUNITINFO *ptInfo  )
{
	PLRINFO *ptPlrInfo;

#line 1499
	D2ASSERT(ptInfo);
#line 1500
	D2ASSERT(ptUnit);
	if (ptUnit->eType == UNIT_PLAYER && (ptPlrInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(ptUnit)) != NULL)
		strcpy(ptInfo->szDesc, ptPlrInfo->szName);
	else
		ptInfo->szDesc[0] = 0;
	ptInfo->eType = ptUnit->eType;
	ptInfo->dwClassId = ptUnit->dwClassId;
	ptInfo->dwUnitId = ptUnit->dwId;
	ptInfo->dwMode = ptUnit->dwMode;
	ptInfo->dwX = D2CommonUnitGetX(ptUnit);
	ptInfo->dwY = D2CommonUnitGetY(ptUnit);
	ptInfo->dwLinkedUnitType = ptUnit->dwLinkedUnitType;
	ptInfo->dwLinkedUnitId = ptUnit->dwLinkedUnitId;
}

UNIT *__fastcall SUnitGetInteractUnit(GAME *ptGame  ,
	UNIT *hUnit  )
{
	if (!hUnit->field_8c)
		return NULL;
	return SUnitFind(ptGame, hUnit->field_88, hUnit->field_84);
}

void __fastcall SUnitFreeInventory(GAME *ptGame  ,
	UNIT *hUnit  )
{
	void *hInventory;
	void *hNode;
	void *hNext;
	UNIT *hItem;
	UNIT *hRemovedItem;

#line 1543
	D2ASSERT(hUnit);
	hInventory = hUnit->ptInventory;
	if (!hInventory)
		return;
	for (hNode = D2CommonINVENTORYGetCategoryListHead(hInventory); hNode; hNode = hNext)
	{
		hNext = (void *)D2CommonINVENTORYNodeGetNext(hNode);
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(hNode);
#line 1553
		D2ASSERT(hItem);
		if (D2CommonINVENTORYNodeGetCategory(hNode) == INVNODE_EQUIPPED)
			ITEMSModeRemoveEffects(ptGame, hItem, hUnit, FALSE, TRUE);
		hRemovedItem = (UNIT *)D2CommonINVENTORYRemoveItem(hInventory, hItem);
#line 1564
		D2ASSERT(hRemovedItem);
		SUnitFree(ptGame, hItem);
	}
}

GAME *__fastcall SUnitGetGame(UNIT *hUnit  )
{
#line 1576
	D2ASSERT(hUnit);
	return hUnit->field_a0;
}

void __fastcall SUnitSetPetId(UNIT *hUnit  , DWORD dwValue)
{
#line 1585
	D2ASSERT(hUnit);
	((PLRINFO *)hUnit->uTypeData)->dwPetId = dwValue;
}

DWORD __fastcall SUnitGetPetId(UNIT *hUnit  )
{
#line 1595
	D2ASSERT(hUnit);
	return ((PLRINFO *)hUnit->uTypeData)->dwPetId;
}

void __fastcall SUnitSetObjectFlags(UNIT *hUnit  , BYTE bValue)
{
#line 1606
	D2ASSERT(hUnit);
	hUnit->field_98 = bValue;
}

BYTE __fastcall SUnitGetObjectFlags(UNIT *hUnit  )
{
#line 1615
	D2ASSERT(hUnit);
	return hUnit->field_98;
}

BOOL __stdcall SUnitGetInteract(UNIT *hUnit  , DWORD *pdwType,
	DWORD *pdwUnitId)
{
	*pdwUnitId = hUnit->field_84;
	*pdwType = hUnit->field_88;
	return hUnit->field_8c;
}

void __stdcall SUnitSetInteract(UNIT *hUnit  , DWORD dwType,
	DWORD dwUnitId)
{
	char szMessage[80];
	PLRINFO *ptPlrInfo;

	if (hUnit->field_8c)
	{
		if (hUnit->eType == UNIT_PLAYER
			&& (ptPlrInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit)) != NULL)
			sprintf(szMessage, "Player %s attempted to operate while busy\n", ptPlrInfo->szName);
		return;
	}
	hUnit->field_84 = dwUnitId;
	hUnit->field_88 = dwType;
	hUnit->field_8c = 1;
}

UNIT *__stdcall SUnitResetInteract(UNIT *hUnit  )
{
	hUnit->field_84 = (DWORD)-1;
	hUnit->field_88 = NUM_UNITTYPE;
	hUnit->field_8c = 0;
	return hUnit;
}

BOOL __fastcall SUnitIsDead(UNIT *hUnit  )
{
	DWORD dwMode;

#line 1709
	D2ASSERT(hUnit);
	dwMode = hUnit->dwMode;
	switch (hUnit->eType)
	{
	case UNIT_PLAYER:
		if (dwMode && dwMode != PLRMODE_DEAD)
			return FALSE;
		break;
	case UNIT_MONSTER:
		if (dwMode && dwMode != MONMODE_DEAD)
			return FALSE;
		break;
	}
	return TRUE;
}

BOOL __fastcall SUnitIsHostile(GAME *ptGame  , UNIT *hUnit
, UNIT *hOther)
{
	UNIT *hOwner;

	if (hUnit->eType == UNIT_MONSTER && (hOwner = AiGetOwner(hUnit)) != NULL)
		hUnit = hOwner;
	if (hOther->eType == UNIT_MONSTER && (hOwner = AiGetOwner(hOther)) != NULL)
		hOther = hOwner;
	if (hUnit == hOther)
		return FALSE;
	if (hUnit->eType == UNIT_PLAYER && hOther->eType == UNIT_PLAYER)
		return PlayerListTestFlag(hUnit, hOther, PLAYERLIST_HOSTILE);
	return !D2CommonStatsUnitsTestSameAlignment(hUnit, hOther);
}

BOOL __fastcall SUnitIsTargetPet(GAME *ptGame  , UNIT *hUnit
, UNIT *hOther, int nSkill)
{
	DWORD dwOwnerId;
	DWORD dwOwnerType;

	if (!hUnit || !hOther)
		return FALSE;
	if (hUnit->eType != UNIT_PLAYER && hOther->eType != UNIT_MONSTER)
		return FALSE;
	if (!((SKILLSTXT *)D2CommonDataTblsGetSkill(nSkill))->TargetPet)
		return FALSE;
	AiGetOwnerId(hOther, &dwOwnerId, &dwOwnerType);
	if (dwOwnerType != UNIT_PLAYER || dwOwnerId != hUnit->dwId)
		return FALSE;
	return TRUE;
}

BOOL __fastcall SUnitIsTargetAlly(GAME *ptGame  , UNIT *hUnit
, UNIT *hOther, int nSkill)
{
	DWORD dwType;
	DWORD dwUnitId;
	WORD wParty;

	if (!hUnit || !hOther || hUnit->eType != UNIT_PLAYER)
		return FALSE;
	dwType = hOther->eType;
	if (dwType != UNIT_PLAYER && dwType != UNIT_MONSTER)
		return FALSE;
	if (!((SKILLSTXT *)D2CommonDataTblsGetSkill(nSkill))->TargetAlly)
		return FALSE;
	dwUnitId = hOther->dwId;
	if (dwType == UNIT_MONSTER)
	{
		AiGetOwnerId(hOther, &dwUnitId, &dwType);
		if (dwUnitId == (DWORD)-1 || dwType != UNIT_PLAYER)
			return FALSE;
	}
	wParty = SUnitGetParty(hUnit);
	if (wParty == PARTY_NONE)
		return FALSE;
	hOther = SUnitFind(ptGame, dwType, dwUnitId);
	if (!hOther)
		return FALSE;
	return wParty == SUnitGetParty(hOther);
}

BOOL __fastcall SUnitIsAlly(GAME *ptGame  , UNIT *hUnit
, UNIT *hOther)
{
	UNIT *hOwner;
	DWORD wParty;
	DWORD wOtherParty;

	if (hUnit->eType == UNIT_MONSTER && (hOwner = AiGetOwner(hUnit)) != NULL)
		hUnit = hOwner;
	if (hOther->eType == UNIT_MONSTER && (hOwner = AiGetOwner(hOther)) != NULL)
		hOther = hOwner;
	if (hUnit == hOther)
		return TRUE;
	if (hUnit->eType == UNIT_PLAYER && hOther->eType == UNIT_PLAYER)
	{
		wParty = SUnitGetParty(hUnit);
		wOtherParty = SUnitGetParty(hOther);
		return wParty == wOtherParty && wOtherParty != PARTY_NONE;
	}
	return D2CommonStatsUnitsTestSameAlignment(hUnit, hOther);
}

void __fastcall SUnitSetAlignment(UNIT *hUnit  ,
	BYTE bAlignNew)
{
	void *hList;
	int eAlignOld;
	int eAlignNew  ;
	GAMEROOM *hRoom;

	eAlignNew = bAlignNew;
	eAlignOld = ALIGNMENT_UNSET;
	if (!hUnit)
		return;
#line 1979
	D2ASSERT(eAlignNew < NUM_ALIGNMENT);
#line 1982
	D2ASSERT(! ( (UnitGetClass (hUnit) != MONSTERCLASS_HYDRA1) &&
		(UnitGetClass (hUnit) != MONSTERCLASS_HYDRA2) &&
		(UnitGetClass (hUnit) != MONSTERCLASS_HYDRA3) &&
		(eAlignNew == ALIGNMENT_BAD) &&
		(UnitTestFlag (hUnit, (DWORD) UNIT_MASK_PET)) ));
	hList = (void *)D2CommonStatsFindListByState(hUnit, STATE_ALIGNMENT);
	if (!hList)
	{
		hList = (void *)D2CommonStatsAllocList(hUnit->dwMemPool, 0, 0, hUnit->eType, hUnit->dwId);
#line 1997
		D2WARN(hList);
		if (hList)
		{
			D2CommonStatsSetListState(hList, STATE_ALIGNMENT);
			D2CommonStatsUnitLinkList(hUnit, hList);
			D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_ALIGNMENT, TRUE);
		}
	}
	else
		eAlignOld = (int)D2CommonStatsListGetValueA(hList, STAT_ALIGNMENT);
	D2CommonStatsListSetValueB(hList, STAT_ALIGNMENT, eAlignNew);
	D2CommonStatsUnitSetActiveStateValue(hUnit, STATE_ALIGNMENT, TRUE);
	if (hUnit->eType != UNIT_MONSTER || !hUnit->field_a0)
		return;
	MonsterRegionModeChange((DWORD *)&hUnit->field_a0->pMonRegions, hUnit, eAlignOld, eAlignNew);
	if (eAlignOld == eAlignNew)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	if (!eAlignNew)
		D2CommonDungeonRoomIncPlayerUnits(hRoom);
	else if (!eAlignOld)
		D2CommonDungeonRoomDecPlayerUnits(hRoom);
}

void __fastcall SUnitSetStateEndLinked(UNIT *hUnit  , BOOL bSet)
{
	DAMAGE tDamage;
	GAME *ptGame;
	UNIT *hOwner;
	int i;

	D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_36, bSet);
	if (!bSet && D2CommonStatsGetState(hUnit, STATE_5C))
	{
		D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_5C, bSet);
		ptGame = SUnitGetGame(hUnit);
		if ((hUnit->dwFlags & UNITFLAG_OWNED) && (hOwner = SUnitFind(ptGame, hUnit->dwLastAttackerType,
			hUnit->dwLastAttackerId)) != NULL)
		{
			for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
				((DWORD *)&tDamage)[i] = 0;
			tDamage.wFlags |= 2;
			SUnitDmgReact(ptGame, hOwner, hUnit, &tDamage);
		}
		else
			SUnitDmgKill(ptGame, hUnit, NULL, 1);
	}
	if (hUnit->eType == UNIT_MONSTER)
		EventRemoveByType(SUnitGetGame(hUnit), hUnit, EVENT_TYPE_2);
}

BOOL __fastcall SUnitMoveToRoom(GAME *ptGame  , UNIT *hUnit
, GAMEROOM *hRoom, int nX, int nY,
	BOOL bExact, BOOL bTeleported)
{
	GAMEROOM *hUnitRoom;
	void *hPath;
	GAMECOORD tPoint;
	GAMERECT atRects[2];
	CLIENT *ptClient;

	hUnitRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	hPath = hUnit->ptPath;
#line 2052
	D2ASSERT(hPath);
	tPoint.dwX = nX;
	tPoint.dwY = nY;
	if (!hRoom)
	{
		hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(hRoom, tPoint.dwX, tPoint.dwY);
		if (!hRoom)
		{
			hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(hUnitRoom, tPoint.dwX, tPoint.dwY);
			if (!hRoom)
				return FALSE;
		}
	}
	if (!bExact)
		hRoom = (GAMEROOM *)D2CommonCollisnFindSpawnPointNear(hRoom, &tPoint,
			D2CommonUnitGetSizeX(hUnit), SPAWN_FLAGS, 0);
	if (!hRoom)
		return FALSE;
	if (!D2CommonStepTeleportUnit(hPath, hUnit, hRoom, tPoint.dwX, tPoint.dwY))
		return FALSE;
	if (hUnit->eType == UNIT_PLAYER)
	{
		ptClient = SUnitGetClient(hUnit);
		D2CommonDungeonRoomCopyRects(hRoom, atRects);
		SCmdSendRoom(ptClient,
			D2CommonDungeonRoomGetLevelId(hRoom), (WORD)atRects[1].nX, (WORD)atRects[1].nY);
		if (bTeleported)
		{
			D2CommonUnitRoomAddToChangedList(hUnit);
			hUnit->dwFlags |= UNITFLAG_TELEPORTED;
		}
		else
		{
			D2CommonUnitRoomAddToChangedList(hUnit);
			hUnit->dwFlags |= UNITFLAG_10000;
		}
		SUnitUpdateClients(ptGame, hUnit, NULL);
		PlayerPetsWarp(ptGame, hUnit, tPoint.dwX, tPoint.dwY);
		return TRUE;
	}
	if (bTeleported)
	{
		D2CommonUnitRoomAddToChangedList(hUnit);
		hUnit->dwFlags |= UNITFLAG_TELEPORTED;
	}
	else
	{
		D2CommonUnitRoomAddToChangedList(hUnit);
		hUnit->dwFlags |= UNITFLAG_10000;
	}
	SUnitUpdateClients(ptGame, hUnit, NULL);
	return TRUE;
}

WORD __fastcall SUnitGetParty(UNIT *hUnit  )
{
	if (hUnit && hUnit->field_a0 && hUnit->eType == UNIT_PLAYER)
		return PartyGetId(hUnit->field_a0, hUnit);
	return PARTY_NONE;
}
