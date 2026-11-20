#include <windows.h>
#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\PLAYER\\PlrModes.cpp"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "../GAME/Event.h"
#include "../GAME/Game.h"
#include "../GAME/SCmd.h"
#include "../INVENTORY/InvMode.h"
#include "../ITEMS/ItemMode.h"
#include "../ITEMS/Items.h"
#include "Player.h"
#include "PlayerPets.h"
#define D2_PLRMODES_CPP
#include "PlrModes.h"
#include "PlrMsg.h"
#include "PlrSave.h"
#include "../QUESTS/QUESTS.h"
#include "../QUESTS/a2q2.h"
#include "../UNIT/Party.h"
#include "../UNIT/PlayerList.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitMsg.h"
#include "../UNIT/SUNITNPC.h"
#include "../AI/AiGeneral.h"
#include "../SKILLS/Skills.h"
#include "../SKILLS/SkillSor.h"

#define UNIT_PLAYER			0
#define ITEM_MODE_INVBODY	1
#define ITEM_MODE_HAND		4
#define ITEMTYPE_GOLD		4
#define UnitGetType(hUnit)	((hUnit)->eType)
#define UnitGetMode(hUnit)	((hUnit)->dwMode)
#define UnitGetId(hUnit)	((hUnit)->dwId)
#define ITEMSGetType(hItem)	D2CommonITEMSGetTypeRaw((DWORD)(hItem))

BOOL __fastcall PlrCanBeInterrupted(UNIT *hUnit, BOOL bCheckSkill,
	BOOL bCheckChance);
void __fastcall ClientSetLifeManaUnit(CLIENT *ptClient, UNIT *hUnit);
void __fastcall ClientChangeFlags(CLIENT *ptClient, WORD wMask, BOOL bSet);
void __fastcall SCmdSendAllCorpse(GAME *ptGame, DWORD dwUnitId,
	DWORD dwBodyId,
	BYTE bAdd);
void __fastcall SCmdSendUnitModeAt(CLIENT *ptClient, BYTE bCmd, BYTE bTOU, DWORD dwUnitId,
	BYTE bMode, WORD wX, WORD wY,
	BYTE bHitClass, BYTE bLife);

typedef void (__fastcall *PFNGAMEUNIT)(GAME *ptGame, UNIT *hUnit);

struct PLRMODESTARTCHART
{
	PFNPLRMODESTARTXY	pfnStartXY;
	PFNPLRMODESTARTID	pfnStartID;
};
static const PLRMODESTARTCHART sgtStartChart[20] =
{
	{ NULL,                     PlrModeStartDeath },
	{ PlrModeStartNeutral,      NULL },
	{ PlrModeStartMoveTo,       PlrModeStartMoveToUnit },
	{ PlrModeStartMoveTo,       PlrModeStartMoveToUnit },
	{ PlrModeStartGetHit,        NULL },
	{ PlrModeStartNeutral,      NULL },
	{ PlrModeStartMoveTo,       PlrModeStartMoveToUnit },
	{ PlrModeStartSkillXY,      PlrModeStartSkillID },
	{ PlrModeStartSkillXY,      PlrModeStartSkillID },
	{ PlrModeStartBlock,        NULL },
	{ PlrModeStartSkillXY,      PlrModeStartSkillID },
	{ PlrModeStartSkillXY,      PlrModeStartSkillID },
	{ PlrModeStartSkillXY,      PlrModeStartSkillID },
	{ PlrModeStartSkillXY,      PlrModeStartSkillID },
	{ PlrModeStartSkillXY,      PlrModeStartSkillID },
	{ PlrModeStartSkillXY,      PlrModeStartSkillID },
	{ PlrModeStartSkillXY,      PlrModeStartSkillID },
	{ PlrModeStartDead,         NULL },
	{ PlrModeStartSkillXY,      PlrModeStartSkillID },
	{ PlrModeStartMoveTo,       PlrModeStartMoveToUnit },
};

static const PFNPLRMODEEVENT scpfnEventFunctions[13] =
{
	PlrModeEventUpdateMode,
	PlrModeEventEndMode,
	NULL,
	PlrModeEventRegenerate,
	(PFNGAMEUNIT)A2Q2UnitKilled,
	SkillsActiveStates,
	PlrModeEventChat,
	NULL,
	SkillsEventPeriodic,
	SkillsEventAura,
	NULL,
	PlrModeEventPeriodic,
	ITEMSModeEventExpire,
};

static int sgnPlrModeWearTypes[5] = { 5, 6, 33, 42, 43 };
static DWORD sgdwPlrModeWearTypes = 5;
static int sgnPlrModeReloadTypes[4] = { 38, 33, 42, 43 };
static DWORD sgdwPlrModeReloadTypes = 4;
static DWORD sgdwPlrModeWearModes[3] = { 7, 8, 11 };
static int sgnPlrModeWearModes = 3;
static PFNPLRMODEDO sgpfnDoMode[40] =
{
	PlrModeUpdateNone,      PlrModeUpdateNone,      PlrModeUpdateMove,      PlrModeUpdateMove,
	PlrModeUpdateAnim,      PlrModeUpdateNone,      PlrModeUpdateMove,      PlrModeUpdateSkill,
	PlrModeUpdateSkill,     PlrModeUpdateAnim,      PlrModeUpdateSkill,     PlrModeUpdateSkill,
	PlrModeUpdateSkill,    PlrModeUpdateSkill,     PlrModeUpdateSkill,     PlrModeUpdateSkill,
	PlrModeUpdateSkill,    PlrModeUpdateNone,      PlrModeUpdateSkill,     PlrModeUpdateMove,
	NULL,                               NULL,
	(PFNPLRMODEDO)PlrModeStartMoveToUnit,  (PFNPLRMODEDO)PlrModeStartMoveToUnit,
	NULL,                               NULL,
	(PFNPLRMODEDO)PlrModeStartMoveToUnit,  (PFNPLRMODEDO)PlrModeStartSkillID,
	(PFNPLRMODEDO)PlrModeStartSkillID,     NULL,
	(PFNPLRMODEDO)PlrModeStartSkillID,     (PFNPLRMODEDO)PlrModeStartSkillID,
	(PFNPLRMODEDO)PlrModeStartSkillID,     (PFNPLRMODEDO)PlrModeStartSkillID,
	(PFNPLRMODEDO)PlrModeStartSkillID,     (PFNPLRMODEDO)PlrModeStartSkillID,
	(PFNPLRMODEDO)PlrModeStartSkillID,     NULL,
	(PFNPLRMODEDO)PlrModeStartSkillID,     (PFNPLRMODEDO)PlrModeStartMoveToUnit,
};

void __fastcall PlrModeStartNeutral(GAME *ptGame, UNIT *hPlayer, int nMode, int nX,
	int nY)
{
	GAMEROOM *hRoom;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hPlayer);
	if (hRoom)
	{
		if (D2CommonDungeonRoomIsTown(hRoom))
			nMode = 5;
		else
			nMode = 1;
	}
	SUnitClearModeEvents(ptGame, hPlayer);
	SUnitSetMode(ptGame, hPlayer, nMode);
	D2CommonUnitSetSkillThirdCached(hPlayer, 0);
}

int __fastcall PlrModeUpdateNone(GAME *ptGame, UNIT *hPlayer)
{
	return 1;
}

void __fastcall PlrModeStartMoveToUnit(GAME *ptGame, UNIT *hPlayer, int nMode, UNIT *hTarget)
{
	void *ptPath;

#line 256
	D2ASSERT(ptGame);
	ptPath = hPlayer->ptPath;
	if (nMode == 19 && UnitGetMode(hPlayer) == 19)
		return;
	D2CommonPathSetTarget(ptPath, hTarget);
	PlrModeStartMove(ptGame, hPlayer, nMode);
}

void __fastcall PlrModeStartMove(GAME *ptGame, UNIT *hPlayer, int nMode)
{
	void *ptPath;

#line 206
	D2ASSERT(ptGame);
	ptPath = hPlayer->ptPath;
	D2CommonPathRestoreType(ptPath);
	switch (nMode)
	{
	case 3:
		if (D2CommonStatsUnitGetValue(hPlayer, 10))
			break;
	case 2:
	case 6:
		if (D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hPlayer)))
			nMode = 6;
		else
			nMode = 2;
		break;
	case 19:
		D2CommonPathSetType(ptPath, 8);
		D2CommonPathSetSquareCountdown(ptPath, 5);
		break;
	}
	D2CommonPathCompute(ptPath, hPlayer);
	D2CommonUnitSetSkillThirdCached(hPlayer, 0);
	if (!D2CommonPathGetStepCount(ptPath))
	{
		PlrModeStartNeutral(ptGame, hPlayer, 1, 0, 0);
		return;
	}
	SUnitSetMode(ptGame, hPlayer, nMode);
	SUnitClearModeEvents(ptGame, hPlayer);
	SUnitSetStandingEvent(ptGame, hPlayer);
}

void __fastcall PlrModeStartMoveTo(GAME *ptGame, UNIT *hPlayer, int nMode, int nX, int nY)
{
#line 270
	D2ASSERT(ptGame);
	D2CommonPathSetTargetCoord(hPlayer->ptPath, nX, nY);
	PlrModeStartMove(ptGame, hPlayer, nMode);
}

int __fastcall PlrModeUpdateMove(GAME *ptGame, UNIT *hPlayer)
{
	void *pInventory;
	CHARSTATSTXT *ptCharStats;
	UNIT *hArmor;
	ITEMSTXT *ptItemsTxt;
	int nDrain;
	int nBonus;
	int nStep;
	int nSkill;
	void *ptSkill;
	DWORD dwMode;

	SUnitUpdateTarget(ptGame, hPlayer);
	if (D2CommonStatsGetState(hPlayer, 13))
		SkillsBlazeStep(ptGame, hPlayer);
	if (UnitGetMode(hPlayer) == 3)
	{
#line 293
		D2ASSERT(hPlayer);
		if (!D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hPlayer)))
		{
			ptCharStats = (CHARSTATSTXT *)D2CommonDataTblsGetCharStats(hPlayer->dwClassId);
			nDrain = ptCharStats->RunDrain * 2;
			pInventory = hPlayer->ptInventory;
			hArmor = (UNIT *)D2CommonINVENTORYGetLocSlot(pInventory, 3);
			if (hArmor)
			{
				ptItemsTxt = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hArmor->dwClassId);
				nDrain *= ptItemsTxt->speed / 10 + 1;
			}
			nBonus = (int)D2CommonStatsGetTotalValue(hPlayer, 154);
			if (nBonus)
				nDrain = nBonus * nDrain / 100;
			D2CommonStatsUnitAddValue(hPlayer, 10, -nDrain);
			if (D2CommonStatsUnitGetValue(hPlayer, 10) < 0)
			{
				D2CommonStatsUnitSetValue(hPlayer, 10, 0);
				PlrModeStartMove(ptGame, hPlayer, 2);
			}
		}
	}
	nStep = SUnitStepAnim(ptGame, hPlayer);
	if (nStep == 2)
	{
		if (hPlayer->dwKeptCmdPending)
		{
			nSkill = hPlayer->nKeptCmd;
			if (nSkill >= 0)
			{
				ptSkill = (void *)D2CommonSkillsFindByKey(hPlayer, nSkill);
				dwMode = D2CommonSkillsGetNodeModeIfAny(hPlayer, ptSkill);
				PlrModeStartID(ptGame, hPlayer, ptSkill, (int)dwMode, hPlayer->dwKeptTargetType,
					hPlayer->dwKeptTargetId,
					FALSE);
				hPlayer->dwKeptCmdPending = 0;
			}
			else
			{
				PlrModeStartNeutral(ptGame, hPlayer, 1, 0, 0);
				PlrMsgInteract(hPlayer, hPlayer->dwKeptTargetType, hPlayer->dwKeptTargetId, nSkill == -2,
					ptGame);
				hPlayer->dwKeptCmdPending = 0;
			}
		}
		else
			PlrModeStartNeutral(ptGame, hPlayer, 1, 0, 0);
	}
	return nStep;
}

static __inline void PlrModeDropGold(GAME *ptGame, UNIT *hUnit, UNIT *hPlayer, int nGold)
{
	COLPOINT tCoord;
	UNIT *hGold;

	D2CommonUnitGetPosition(hUnit, &tCoord);
	hGold = ITEMSDropOtherItem(ptGame, (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit), &tCoord, ITEMTYPE_GOLD,
		TRUE, hPlayer);
#line 406
	D2ASSERT(hGold);
#line 407
	D2ASSERT(ITEMSGetType(hGold) == ITEMTYPE_GOLD);
	D2CommonStatsUnitSetValue(hGold, 14, nGold);
	D2CommonITEMSDataSetOwnerId(hGold, hPlayer->dwId);
}

static void __fastcall PlrModeRemoveBodyItem(GAME *ptGame, int nLoc, UNIT *hItem, UNIT *hPlayer)
{
	void *hInventory;
	BOOL fSetComplete;
	UNIT *hRemove;

	hInventory = hPlayer->ptInventory;
	fSetComplete = D2CommonITEMSTestSetComplete(hPlayer, hItem);
	D2CommonINVENTORYOfferRemoveDelta(hInventory, hItem);
	hRemove = (UNIT *)D2CommonINVENTORYRemoveItem(hInventory, hItem);
#line 424
	D2ASSERT(hRemove);
#line 425
	D2ASSERT(hItem == hRemove);
	D2CommonINVENTORYClearLocSlot(hInventory, nLoc);
	if (D2CommonITEMSGetType(hItem) == 19)
		ITEMSModeDropBeltOverflow(ptGame, hPlayer, NULL);
	hItem->dwFlags0 &= ~2;
	ITEMSModeRemoveEffects(ptGame, hItem, hPlayer, FALSE, TRUE);
	D2CommonITEMSDataSetFlags(hItem, 32, 1);
	D2CommonITEMSDataSetCmdFlags(hItem, 1, 1);
	D2CommonITEMSDataSetCmdFlags(hItem, 16, 1);
	(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
	if (fSetComplete)
		ITEMSModeRemoveSetBonus(hPlayer, hItem);
	D2CommonUnitSetChanged(hPlayer, 1);
}

UNIT * __fastcall PlrModeCreateCorpse(GAME *ptGame, UNIT *hPlayer, int nX, int nY, GAMEROOM *hRoom)
{
	void *hInventory;
	void *hBodyInventory;
	void *hCursorInventory;
	void *ptNode;
	void *ptNext;
	DWORD dwCorpseId;
	int nLoc;
	UNIT *hItem;
	UNIT *hCopy;
	UNIT *hDeadBody;
	GAMEROOM *hDropRoom;
	CLIENT *ptClient;
	int nGold;
	int nRecord;

	hInventory = hPlayer->ptInventory;
	for (ptNode = D2CommonINVENTORYGetTrackedListHead(hInventory); ptNode; ptNode = ptNext)
	{
		ptNext = (void *)D2CommonINVENTORYNodeGetTrackedNext(ptNode);
		dwCorpseId = D2CommonINVENTORYNodeGetTrackedKey(ptNode);
		if (!SUnitFind(ptGame, UNIT_PLAYER, dwCorpseId))
			D2CommonINVENTORYRemoveTrackedNode(hInventory, dwCorpseId, 1);
	}
	hInventory = hPlayer->ptInventory;
	if ((DWORD)D2CommonINVENTORYGetTrackedListCount(hInventory) > 0)
	{
		void *hDropInventory;
		int nDropLoc;

		hDropInventory = hPlayer->ptInventory;
		for (nDropLoc = -1; nDropLoc < 11; nDropLoc++)
		{
			COLPOINT tCoord;
			COLPOINT tSpawn;
			if (nDropLoc == -1)
				hItem = (UNIT *)D2CommonINVENTORYGetCursorItem(hDropInventory);
			else
				hItem = (UNIT *)D2CommonINVENTORYGetLocSlot(hDropInventory, nDropLoc);
			if (!hItem)
				continue;
			if (nDropLoc == -1)
			{
#line 487
				D2ASSERT(UnitGetMode(hItem) == ITEM_MODE_HAND);
				hCursorInventory = hPlayer->ptInventory;
				ptClient = SUnitGetClient(hPlayer);
				SCmdSendCursorCleared(ptClient, hPlayer);
				D2CommonINVENTORYSetCursorItem(hCursorInventory, 0);
				D2CommonITEMSDataSetCmdFlags(hItem, 1, 1);
				(void)D2CommonINVENTORYAddDeltaNode(hCursorInventory, hItem);
				D2CommonUnitSetChanged(hPlayer, 1);
			}
			else
			{
#line 493
				D2ASSERT(UnitGetMode(hItem) == ITEM_MODE_INVBODY);
				PlrModeRemoveBodyItem(ptGame, nDropLoc, hItem, hPlayer);
			}
			hCopy = ITEMSDuplicate(ptGame, hItem, hPlayer);
			D2CommonUnitGetPosition(hPlayer, &tCoord);
			hDropRoom = ITEMSFindDropSpot((GAMEROOM *)D2CommonUnitGetPathRoom(hPlayer), &tCoord, &tSpawn,
				1);
			if (!hDropRoom)
			{
#line 508
				D2ASSERT(0);
			}
			D2CommonUnitInitPosition(0, hCopy, hDropRoom, tSpawn.nX, tSpawn.nY);
			D2CommonUnitSetCollision(hCopy, hDropRoom, tSpawn.nX, tSpawn.nY);
			D2CommonAddUnitToRoom(hCopy, hDropRoom);
			hCopy->dwFlags0 |= 2;
			D2CommonUnitSetMode(hCopy, 3);
			D2CommonITEMSDataSetFlags(hCopy, 8192, 1);
			hCopy->dwFlags0 |= 33554432;
			QuestsItemDropped(ptGame, hPlayer, hCopy);
		}
		nGold = D2CommonStatsUnitGetValue(hPlayer, 14);
		D2CommonStatsUnitSetValue(hPlayer, 14, 0);
		PlayerDropGold(ptGame, hPlayer, hPlayer->dwId, nGold);
		return NULL;
	}

	hDeadBody = SUnitCreate(UNIT_PLAYER, (int)hPlayer->dwClassId, nX, nY, ptGame, hRoom, 1, 17, 0);
#line 568
	D2ASSERT(hDeadBody);
#line 569
	D2ASSERT(hRoom);
	ptClient = SUnitGetClient(hPlayer);
	hDeadBody->dwFlags0 &= ~4;
	SUnitSetClient(hDeadBody, ptClient);
	D2CommonUnitSetPlayerName(hDeadBody, ClientGetName(ptClient));
	SUnitInitSeed(hDeadBody, (D2SEED *)&ptGame->ptAct[NUM_ACTS]);
	D2CommonUnitSetMode(hDeadBody, 17);
	D2CommonStatsUnitSetActiveStateBit(hDeadBody, 7, 1);
	hDeadBody->dwFlags |= 4096;
	hDeadBody->dwFlags0 |= 2;
	D2CommonINVENTORYAppendTrackedNode(hInventory, hDeadBody->dwId, 0, 1);
	hBodyInventory = hDeadBody->ptInventory;
#line 584
	D2ASSERT(UnitGetId(hDeadBody) != UnitGetId(hPlayer));
	D2CommonINVENTORYSetOwnerId(hBodyInventory, hPlayer->dwId);
	for (nLoc = -1; nLoc < 11; nLoc++)
	{
		COLPOINT tCoord;
		COLPOINT tSpawn;
		if (nLoc == -1)
			hItem = (UNIT *)D2CommonINVENTORYGetCursorItem(hInventory);
		else
			hItem = (UNIT *)D2CommonINVENTORYGetLocSlot(hInventory, nLoc);
		if (!hItem)
			continue;
		if (nLoc == -1)
		{
#line 604
			D2ASSERT(UnitGetMode(hItem) == ITEM_MODE_HAND);
			hCursorInventory = hPlayer->ptInventory;
			ptClient = SUnitGetClient(hPlayer);
			SCmdSendCursorCleared(ptClient, hPlayer);
			D2CommonINVENTORYSetCursorItem(hCursorInventory, 0);
			D2CommonITEMSDataSetCmdFlags(hItem, 1, 1);
			(void)D2CommonINVENTORYAddDeltaNode(hCursorInventory, hItem);
			D2CommonUnitSetChanged(hPlayer, 1);
		}
		else
		{
#line 610
			D2ASSERT(UnitGetMode(hItem) == ITEM_MODE_INVBODY);
			PlrModeRemoveBodyItem(ptGame, nLoc, hItem, hPlayer);
		}
		D2CommonITEMSDataSetPageRaw((DWORD)hItem, 0);
		hCopy = ITEMSDuplicate(ptGame, hItem, hPlayer);
		nRecord = D2CommonUnitGetInvRecordIndexRaw((DWORD)hDeadBody, 0);
		if (D2CommonINVENTORYPlaceItemRaw((DWORD)hBodyInventory, (DWORD)hCopy, nRecord, 0, 0))
		{
			if (!D2CommonINVENTORYAddItem(hBodyInventory, hCopy, 1))
			{
#line 633
				D2ASSERT(0);
			}
			hCopy->dwFlags0 &= ~2;
			D2CommonUnitSetMode(hCopy, 0);
			hCopy->dwFlags0 &= ~33554432;
			D2CommonITEMSDataSetCmdFlags(hCopy, 2, 1);
			(void)D2CommonINVENTORYAddDeltaNode(hBodyInventory, hCopy);
			D2CommonUnitSetChanged(hDeadBody, 1);
			QuestsItemAcquired(ptGame, hDeadBody, hCopy);
			continue;
		}
		D2CommonUnitGetPosition(hPlayer, &tCoord);
		hDropRoom = ITEMSFindDropSpot((GAMEROOM *)D2CommonUnitGetPathRoom(hPlayer), &tCoord, &tSpawn, 1);
		if (!hDropRoom)
		{
#line 648
			D2WARN("lost item");
		}
		else
		{
			D2CommonUnitInitPosition(0, hCopy, hDropRoom, tSpawn.nX, tSpawn.nY);
			D2CommonUnitSetCollision(hCopy, hDropRoom, tSpawn.nX, tSpawn.nY);
			D2CommonAddUnitToRoom(hCopy, hDropRoom);
			hCopy->dwFlags0 |= 2;
		}
		hCopy->dwFlags0 |= 4096;
		D2CommonUnitSetMode(hCopy, 3);
		QuestsItemDropped(ptGame, hPlayer, hCopy);
	}
	nGold = D2CommonStatsUnitGetValue(hPlayer, 14);
	D2CommonStatsUnitSetValue(hPlayer, 14, 0);
	if (nGold > 0)
		PlrModeDropGold(ptGame, hDeadBody, hPlayer, nGold);
	SCmdSendAllCorpse(ptGame, hPlayer->dwId, hDeadBody->dwId, 1);
	return hDeadBody;
}

void __fastcall PlrModePickupCorpse(GAME *ptGame, UNIT *hPlayer, UNIT *hDeadBody)
{
	void *pInventory;
	DWORD dwOwnerId;
	UNIT *hOwner;

	if (!D2CommonStatsGetState(hDeadBody, 7))
		return;
#line 678
	D2ASSERT(hPlayer);
#line 679
	D2ASSERT(hDeadBody);
#line 680
	D2ASSERT(UnitGetType(hPlayer) == UNIT_PLAYER);
	pInventory = hDeadBody->ptInventory;
	dwOwnerId = D2CommonINVENTORYGetOwnerId(pInventory);
	if (hPlayer->dwId != dwOwnerId)
	{
		hOwner = SUnitFind(ptGame, UNIT_PLAYER, dwOwnerId);
		if (!hOwner)
			return;
		if (!PlayerListTestFlag(hOwner, hPlayer, 1))
			return;
	}
	if (ITEMSTransferDeadBodyItemsToPlayer(ptGame, hPlayer, hDeadBody))
	{
		D2CommonINVENTORYRemoveTrackedNode(hPlayer->ptInventory, hDeadBody->dwId, 1);
		D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hDeadBody), hDeadBody->eType, hDeadBody->dwId);
		SCmdSendAllCorpse(ptGame, hPlayer->dwId, hDeadBody->dwId, 0);
		D2CommonUnitClearCollision(hDeadBody);
		SUnitFree(ptGame, hDeadBody);
		SUnitSetEvent(hPlayer, 74, hPlayer);
		return;
	}
	SUnitSetEvent(hPlayer, 22, hPlayer);
}

void __fastcall PlrModeStartDeath(GAME *ptGame, UNIT *hPlayer, int nMode, UNIT *hTarget)
{

	UNIT *hOwner;

	PlayerDeathPenalty(ptGame, hPlayer);
	SUnitSetMode(ptGame, hPlayer, 0);
	CLIENT *t31 = SUnitGetClient(hPlayer);
	ClientChangeFlags(t31, 8, TRUE);
	SUnitResetAnim(hPlayer);
	D2CommonStatsUnitFreeLists(hPlayer);
	D2CommonStatsUnitCommitPendingFlags(hPlayer, 0);
	CLIENT *t22 = SUnitGetClient(hPlayer);
	ClientSetLifeManaUnit(t22, NULL);
	D2CommonPathReplaceCollisionMask(hPlayer, 1);
	EventRemoveByType(ptGame, hPlayer, 8);
	SUnitClearModeEvents(ptGame, hPlayer);
	SUnitClearModeEvents(ptGame, hPlayer);
	SUnitScheduleModeEvents(ptGame, hPlayer);
	if (!D2CommonStatsGetState(hPlayer, 7))
		hPlayer->dwFlags0 &= ~2;
	if (hPlayer && hTarget && hPlayer->eType == UNIT_PLAYER && hPlayer != hTarget) {
		if (hTarget->eType == UNIT_PLAYER)
			ITEMSDropEar(ptGame, hPlayer);
		else if (hTarget->eType == 1) {
			hOwner = AiGetOwner(hTarget);
			if (hOwner && hOwner->eType == UNIT_PLAYER && hOwner != hPlayer)
				ITEMSDropEar(ptGame, hPlayer);
		} }
	if (hTarget)
		PlrMsgSendKilled(ptGame, hTarget, hPlayer);
	PlayerInteractEnd(ptGame, hPlayer);
}

static __inline void __fastcall PlrModeStartDead(GAME *ptGame, UNIT *hPlayer, int nMode, int nX, int nY)
{
	CLIENT *ptClient;
	GAMEROOM *hRoom;
	int nUnitY;

	PlrModeCreateCorpse(ptGame, hPlayer, (int)D2CommonUnitGetX(hPlayer), (int)D2CommonUnitGetY(hPlayer), (GAMEROOM *)D2CommonUnitGetPathRoom(hPlayer));
	PlayerPetsClearAll(ptGame, hPlayer);
	ptClient = SUnitGetClient(hPlayer);
	ClientChangeFlags(ptClient, 8, TRUE);
	if (!ptEventCallbackTable || ClientTestFlags(ptClient, 4))
		PlrSaveCharacter(ptGame, hPlayer, ClientGetName(SUnitGetClient(hPlayer)));
	SUnitSetMode(ptGame, hPlayer, 17);
	SUnitResetAnim(hPlayer);
	D2CommonStatsUnitFreeLists(hPlayer);
	D2CommonStatsUnitCommitPendingFlags(hPlayer, 0);
	ptClient = SUnitGetClient(hPlayer);
	ClientSetLifeManaUnit(ptClient, NULL);
	D2CommonPathReplaceCollisionMask(hPlayer, 1);
	EventRemoveByType(ptGame, hPlayer, 8);
	SUnitClearModeEvents(ptGame, hPlayer);
	SUnitClearModeEvents(ptGame, hPlayer);
}

void __fastcall PlrModeStartBlock(GAME *ptGame, UNIT *hPlayer, int nMode, int nX, int nY)
{
#line 838
	D2ASSERT(hPlayer);
	SUnitSetMode(ptGame, hPlayer, nMode);
	SUnitResetAnim(hPlayer);
	D2CommonUnitSetSkillThirdCached(hPlayer, 0);
	SUnitClearModeEvents(ptGame, hPlayer);
	SUnitScheduleModeEvents(ptGame, hPlayer);
}

int __fastcall PlrModeUpdateAnim(GAME *ptGame, UNIT *hPlayer)
{
	D2CommonUnitStepAnimState(hPlayer);
	if (D2CommonUnitTestAnimComplete(hPlayer))
		return 2;
	else
		return 1;
}

void __fastcall PlrModeStartGetHit(GAME *ptGame, UNIT *hPlayer, int nMode, int nX, int nY)
{
#line 865
	D2ASSERT(hPlayer);
	SUnitSetMode(ptGame, hPlayer, nMode);
	SUnitResetAnim(hPlayer);
	D2CommonUnitSetSkillThirdCached(hPlayer, 0);
	SUnitClearModeEvents(ptGame, hPlayer);
	SUnitScheduleModeEvents(ptGame, hPlayer);
}

void __fastcall PlrModeStartSkillXY(GAME *ptGame, UNIT *hPlayer, int nMode, int nX, int nY)
{
	D2CommonPathSetTargetCoord(hPlayer->ptPath, nX, nY);
	SUnitSetMode(ptGame, hPlayer, nMode);
	D2CommonUnitSetTarget(hPlayer, 0);
	SUnitResetAnim(hPlayer);
	SUnitClearModeEvents(ptGame, hPlayer);
	SUnitSetStandingEvent(ptGame, hPlayer);
	hPlayer->dwFlags0 &= ~64;
	SkillsStart(ptGame, hPlayer);
}

void __fastcall PlrModeStartSkillID(GAME *ptGame, UNIT *hPlayer, int nMode, UNIT *hTarget)
{
	SUnitSetMode(ptGame, hPlayer, nMode);
#line 943
	D2ASSERT(hTarget);
	D2CommonUnitSetTarget(hPlayer, hTarget);
	SUnitResetAnim(hPlayer);
	SUnitClearModeEvents(ptGame, hPlayer);
	SUnitSetStandingEvent(ptGame, hPlayer);
	hPlayer->dwFlags0 &= ~64;
	SkillsStart(ptGame, hPlayer);
}

void __fastcall PlrModeWearWeapons(GAME *ptGame, UNIT *hUnit)
{
	void *hInventory;
	UNIT *hItem;

	if (hUnit->eType != UNIT_PLAYER)
		return;
	hInventory = hUnit->ptInventory;
	hItem = (UNIT *)D2CommonINVENTORYGetLocSlot(hInventory, 4);
	if (hItem)
		PlrModeWearItem(ptGame, hUnit, hItem);
	hItem = (UNIT *)D2CommonINVENTORYGetLocSlot(hInventory, 5);
	if (hItem)
		PlrModeWearItem(ptGame, hUnit, hItem);
}

void __fastcall PlrModeWearItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	DWORD dwType;
	BYTE bLoc;
	BOOL fWear;
	BOOL fReload;
	DWORD i;
	UNIT *hSame;
	DWORD dwLastId;
	UNIT *hLast;
	void *hInventory;
	int nResult;

	if (!D2CommonITEMSGetTypeStackable(hItem) && !D2CommonITEMSGetTypeThrowable(hItem)
		&& !D2CommonStatsGetTotalValue(hItem, 125))
		return;
	if (D2CommonStatsUnitGetValue(hItem, 70) > 0)
		return;
	if (D2CommonStatsGetTotalValue(hItem, 125))
	{
		D2CommonStatsUnitSetValue(hItem, 70, 0);
		return;
	}
	fWear = FALSE;
	dwType = D2CommonITEMSGetType(hItem);
	bLoc = D2CommonITEMSGetCategory(hItem);
	for (i = 0; i < sgdwPlrModeWearTypes; i++)
		if (dwType == (DWORD)sgnPlrModeWearTypes[i])
		{
			fWear = TRUE;
			break;
		}
	fReload = FALSE;
	for (i = 0; i < sgdwPlrModeReloadTypes; i++)
		if (dwType == (DWORD)sgnPlrModeReloadTypes[i])
		{
			fReload = TRUE;
			break;
		}
	D2CommonUnitSetMode(hUnit, 1);
	ITEMSModeUnequipToCursor(ptGame, hUnit, D2CommonITEMSGetCategoryRaw((DWORD)hItem), TRUE, &nResult);
	D2CommonITEMSDataSetCmdFlags(hItem, 1, 1);
	if (fWear)
	{
		hSame = (UNIT *)D2CommonINVENTORYFindGridItemByType(hUnit->ptInventory, dwType, 0);
		if (hSame && hItem->dwClassId == hSame->dwClassId
			&& ITEMSModeEquipFromGrid(ptGame, hUnit, hSame->dwId, bLoc))
			return;
	}
	if (!fReload)
		return;
	{
		int nLoc;
		nLoc = bLoc;
		dwLastId = D2CommonUnitGetPlayerWeaponId(hUnit);
		if (!dwLastId)
			return;
		hLast = SUnitFind(ptGame, 4, dwLastId);
		hInventory = hUnit->ptInventory;
		if (hLast && hInventory && !D2CommonITEMSDataGetPage(hLast)
			&& D2CommonINVENTORYTestHasItem(hInventory, hLast) && ITEMSModeFindBodyLoc(hUnit, hLast, &nLoc))
			ITEMSModeEquipFromGrid(ptGame, hUnit, dwLastId, bLoc);
	}
	D2CommonUnitSetPlayerWeaponId(hUnit, 0);
}

void __fastcall PlrModeBreakItems(GAME *ptGame, UNIT *hUnit)
{
	void *hInventory;
	void *ptNode;
	UNIT *hItem;
	BOOL fModeSet;

	if (hUnit->eType != UNIT_PLAYER)
		return;
	hInventory = hUnit->ptInventory;
	ptNode = D2CommonINVENTORYGetCategoryListHead(hInventory);
	fModeSet = FALSE;
	while (ptNode)
	{
		if (D2CommonINVENTORYNodeGetCategory(ptNode) == 3)
		{
			hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
			if (D2CommonITEMSIsWeapon(hItem) && D2CommonITEMSTestTypeHasDurability(hItem)
				&& D2CommonStatsUnitGetValue(hItem, 72) <= 0
				&& !D2CommonITEMSDataTestFlags(hItem, 256))
			{
				if (!fModeSet)
				{
					D2CommonUnitSetMode(hUnit, 1);
					fModeSet = TRUE;
				}
				ITEMSModeBreakItem(ptGame, hUnit, hItem);
				ptNode = D2CommonINVENTORYGetCategoryListHead(hInventory);
				continue;
			}
		}
		ptNode = (void *)D2CommonINVENTORYNodeGetNext(ptNode);
	}
}

int __fastcall PlrModeUpdateSkill(GAME *ptGame, UNIT *hPlayer)
{
	void *ptSkill;
	DWORD dwFlags;
	BOOL fRun;
	BOOL fWorn;
	DWORD dwPathKind;
	SKILLSTXT *ptSkillsTxt;
	int i;

	ptSkill = (void *)D2CommonUnitGetSkillThirdCached(hPlayer);
	dwFlags = D2CommonSkillsGetNodeFlags(ptSkill);
	fRun = TRUE;
	if (dwFlags & 1)
	{
		SUnitUpdateTarget(ptGame, hPlayer);
		if (SUnitStepAnim(ptGame, hPlayer) == 2)
		{
			dwFlags |= 2;
			D2CommonSkillsSetNodeFlags(ptSkill, dwFlags);
			SkillsUse(ptGame, hPlayer);
			fRun = FALSE;
		}
	}
	dwPathKind = hPlayer->bAnimEvent;
	if (fRun && !(hPlayer->dwFlags0 & 64) && (dwPathKind == 1 || dwPathKind == 2))
		SkillsUse(ptGame, hPlayer);
	D2CommonUnitStepAnimState(hPlayer);
	if (!(!D2CommonUnitTestAnimComplete(hPlayer)))
	{
		SUnitClearModeEvents(ptGame, hPlayer);
		fWorn = FALSE;
		for (i = 0; i < sgnPlrModeWearModes; i++)
			if (UnitGetMode(hPlayer) == sgdwPlrModeWearModes[i])
			{
				PlrModeWearWeapons(ptGame, hPlayer);
				PlrModeBreakItems(ptGame, hPlayer);
				fWorn = TRUE;
				break;
			}
		if (ptSkill && !fWorn)
		{
			ptSkillsTxt = (SKILLSTXT *)D2CommonDataTblsGetSkill(D2CommonSkillsGetNodeType(ptSkill));
			if (ptSkillsTxt && ptSkillsTxt->Durability)
			{
				PlrModeWearWeapons(ptGame, hPlayer);
				PlrModeBreakItems(ptGame, hPlayer);
			}
		}
		return 2;
	}
	return 1;
}

void __fastcall PlrModeEventRegenerate(GAME *ptGame, UNIT *hPlayer)
{
	int nRegen;
	int nLife;
	int nMax;
	void *ptList;
	int nStamina;
	int nShift;
	int nAdd;
	int nBonus;
	int nMana;
	int nRate;
	int nDivisor;
	CHARSTATSTXT *ptPlayerInitStats;

	if (SUnitIsDead(hPlayer))
		return;
	nRegen = D2CommonStatsUnitGetValue(hPlayer, 74);
	if (nRegen)
	{
		nLife = D2CommonStatsUnitGetValue(hPlayer, 6);
		nMax = D2CommonStatsUnitCalcMaxHp(hPlayer);
		nLife += nRegen;
		if (nLife > nMax)
		{
			nLife = nMax;
			ptList = (void *)D2CommonStatsFindListByState(hPlayer, 100);
			if (ptList)
			{
				D2CommonStatsUnitUnlinkList(hPlayer, ptList);
				D2CommonStatsListFree(ptList);
			}
		}
		if (nLife < 256)
			nLife = 256;
		D2CommonStatsUnitSetValue(hPlayer, 6, nLife);
	}
	if (!D2CommonStatsGetState(hPlayer, 31))
	{
		nStamina = D2CommonStatsUnitGetValue(hPlayer, 10);
		nShift = 8;
		switch (UnitGetMode(hPlayer))
		{
		case 6:
			nShift = 9;
			goto regenerate;
		case 2:
			nShift = 9;
			if (!(nStamina & 0xFFFFFF00))
				break;
			goto regenerate;
		case 1:
		case 5:
		regenerate:
			nMax = D2CommonStatsUnitCalcMaxStamina(hPlayer);
			if (nStamina < nMax)
			{
				nAdd = nMax >> nShift;
				nBonus = D2CommonStatsUnitGetValue(hPlayer, 28);
				if (nBonus)
					nAdd += nBonus * nAdd / 100;
				nStamina += nAdd;
				if (nStamina > nMax)
					nStamina = nMax;
				D2CommonStatsUnitSetValue(hPlayer, 10, nStamina);
			}
			break;
		}
	}
	nMana = D2CommonStatsUnitGetValue(hPlayer, 8);
	nMax = D2CommonStatsUnitCalcMaxMana(hPlayer);
	nAdd = 0;
	if (!D2CommonStatsGetState(hPlayer, 31) && !D2CommonStatsGetState(hPlayer, 85))
	{
		ptPlayerInitStats = (CHARSTATSTXT *)D2CommonDataTblsGetCharStats(hPlayer->dwClassId);
#line 1333
		D2ASSERT(ptPlayerInitStats);
		nDivisor = ptPlayerInitStats->ManaRegen * 25;
		if (!nDivisor)
			nDivisor = 7500;
		if (nMax / nDivisor < 1)
			nRate = 1;
		else
			nRate = nMax / nDivisor;
		nBonus = D2CommonStatsUnitGetValue(hPlayer, 27);
		if (hPlayer->dwClassId == 1 && D2CommonStatsGetState(hPlayer, 70))
		{
			D2CommonDataTblsGetSkill(SKILL_WARMTH);
			nBonus += D2CommonSkillsCalcParamOneByLevel(SKILL_WARMTH, D2CommonSkillsCalcNodeLevel(hPlayer,
				D2CommonSkillsFindByKey(hPlayer, 37), 1));
		}
		nAdd = (nBonus + 100) * nRate / 100;
	}
	nAdd += D2CommonStatsUnitGetValue(hPlayer, 26);
	if (nMax <= nMana && nAdd > 0)
	{
		ptList = (void *)D2CommonStatsFindListByState(hPlayer, 106);
		if (ptList)
		{
			D2CommonStatsUnitUnlinkList(hPlayer, ptList);
			D2CommonStatsListFree(ptList);
		}
	}
	if (nAdd > nMax - nMana)
		nAdd = nMax - nMana;
	if (nAdd < -nMana)
		nAdd = -nMana;
	D2CommonStatsUnitAddValue(hPlayer, 8, nAdd);
}

void __fastcall PlrModeEventEndMode(GAME *ptGame, UNIT *hPlayer)
{
	switch (UnitGetMode(hPlayer))
	{
	case 0:
		PlrModeStartDead(ptGame, hPlayer, 17, 0, 0);
		return;
	case 19:
		PlrModeStartXY(ptGame, hPlayer, NULL, 4, -1, 0, FALSE);
		return;
	}
	PlrModeStartNeutral(ptGame, hPlayer, 1, 0, 0);
}

void __fastcall PlrModeEventUpdateMode(GAME *ptGame, UNIT *hPlayer)
{
	DWORD dwMode;

#line 1425
	D2ASSERT(ptGame);
	dwMode = UnitGetMode(hPlayer);
#line 1431
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgpfnDoMode[dwMode]), "sgpfnDoMode[UnitGetMode(hPlayer)]");
	if (sgpfnDoMode[dwMode](ptGame, hPlayer) == 2)
		PlrModeEventEndMode(ptGame, hPlayer);
}

void __fastcall PlrModeSendUnit(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient, BOOL fNoItems)
{
	UNIT *hPlayer;

	hPlayer = ClientGetPlayerUnit(ptClient, FALSE);
	if (hUnit->dwFlags & 65536)
		PlrModeSendPosition(hUnit, ptClient, TRUE);
	else if ((hUnit->dwFlags & 2048) && hUnit != hPlayer)
		PlrModeSendPosition(hUnit, ptClient, FALSE);
	if (!fNoItems && (hUnit->dwFlags & 1))
		INVENTORYModeSendItems(ptGame, hUnit, ptClient, NULL);
	if (hUnit->dwFlags0 & 1)
		PlrMsgSendMode(ptGame, hUnit, ptClient);
	if (hUnit->dwFlags0 & 1024)
		SUnitNpcSendUnitEvent(hUnit, ptClient);
	if (hUnit->dwFlags0 & 256)
		SUnitMsgSendChat(hUnit, ptClient);
	if (hUnit->dwFlags0 & 32768)
		PlrModeSendLife(hUnit, ptClient);
	if (D2CommonStatsUnitTestAnyActiveState(hUnit))
		SUnitMsgSendStates(hUnit, ptClient, 0);
	if (D2CommonStatsTestHasOverlay(hUnit))
		SUnitMsgSendOverlay(hUnit, ptClient);
	D2CommonStatsUnitCallChanged(hUnit, hPlayer, 67, PlrMsgStatChanged);
	D2CommonStatsUnitCallChanged(hUnit, hPlayer, 68, PlrMsgStatChanged);
	D2CommonStatsUnitCallChanged(hUnit, hPlayer, 12, PlrMsgStatChanged);
}

void __fastcall PlrModeStartXY(GAME *ptGame, UNIT *hPlayer, void *ptSkill, int nMode, int nX, int nY,
	BOOL fForce)
{
	const PLRMODESTARTCHART *cptStartChart;
	BOOL fCanStart;

#line 1518
	D2ASSERT(ptGame);
	fCanStart = PlrModeCanStart(hPlayer, nMode);
	if (!fCanStart)
		return;
	if (!fForce && !PlrCanBeInterrupted(hPlayer, nMode, TRUE))
		return;
	cptStartChart = &sgtStartChart[nMode];
#line 1526
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)cptStartChart->pfnStartXY), "cptStartChart->pfnStartXY");
	if (ptSkill)
		D2CommonUnitSetSkillThirdCached(hPlayer, ptSkill);
	cptStartChart->pfnStartXY(ptGame, hPlayer, nMode, nX, nY);
}

BOOL __fastcall PlrModeCanStart(UNIT *hPlayer, int nMode)
{
	void *ptSkill;

	if (hPlayer->ptInventory && D2CommonINVENTORYGetCursorItem(hPlayer->ptInventory) && nMode != 0)
	{
		if (nMode != 17)
			return FALSE;
	}
	switch (nMode)
	{
	case 0:
	case 1:
	case 5:
	case 17:
		return TRUE;
	}
	switch (UnitGetMode(hPlayer))
	{
	case 0:
	case 4:
	case 9:
	case 12:
	case 17:
		return FALSE;
	case 7:
	case 8:
	case 10:
	case 11:
		if ((hPlayer->nAnimPos >> 8) + 3 < (hPlayer->nAnimLength >> 8) && nMode != 4 && nMode != 9)
			return FALSE;
		return TRUE;
	case 13:
		if (!hPlayer->dwClassId)
			return FALSE;
		break;
	case 18:
		ptSkill = (void *)D2CommonUnitGetSkillThirdCached(hPlayer);
		return ptSkill && D2CommonSkillsGetNodeType(ptSkill) == 41;
	}
	return TRUE;
}

void __fastcall PlrModeStartID(GAME *ptGame, UNIT *hPlayer, void *ptSkill, int nMode, DWORD dwType,
	DWORD dwId, BOOL fForce)
{
	UNIT *hTarget;
	const PLRMODESTARTCHART *cptStartChart;

#line 1561
	D2ASSERT(ptGame);
	hTarget = SUnitFind(ptGame, dwType, dwId);
	if (!hTarget)
	{
#line 1566
		D2CommonLoggingWrite(1, 2, 4, 4, D2_THIS_FILE, __LINE__, "StartModeID: Bad Target");
		return;
	}
	if (!PlrModeCanStart(hPlayer, nMode))
		return;
	if (!fForce && !PlrCanBeInterrupted(hPlayer, nMode, TRUE))
		return;
	hPlayer->nKeptCmd = 0;
	hPlayer->dwKeptCmdPending = 0;
	cptStartChart = &sgtStartChart[nMode];
#line 1583
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)cptStartChart->pfnStartID), "cptStartChart->pfnStartID");
	if (ptSkill)
		D2CommonUnitSetSkillThirdCached(hPlayer, ptSkill);
	cptStartChart->pfnStartID(ptGame, hPlayer, nMode, hTarget);
}

void __fastcall PlrModeEventChat(GAME *ptGame, UNIT *hPlayer)
{
	void *ptChatNode;
	DWORD dwFrame;

	ptChatNode = hPlayer->ptChatNode;
	if (!ptChatNode)
		return;
	dwFrame = D2CommonChatNodeGetEndFrame(ptChatNode);
	if (dwFrame <= (DWORD)ptGame->nFrame)
	{
		D2CommonChatNodeFree(ptGame->pMemPool, ptChatNode);
		hPlayer->ptChatNode = NULL;
		D2CommonUnitRoomAddToChangedList(hPlayer);
		hPlayer->dwFlags0 |= 256;
		return;
	}
	EventAdd(ptGame, hPlayer, 6, (int)dwFrame);
}

void __fastcall PlrModeEventPeriodic(GAME *ptGame, UNIT *hPlayer)
{
#line 1623
	D2ASSERT(ptGame);
#line 1624
	D2ASSERT(hPlayer);
	PartySendLives(ptGame, hPlayer);
	PlayerPetsSendLife(ptGame, hPlayer);
	EventAdd(ptGame, hPlayer, 11, ptGame->nFrame + 30);
}

void __fastcall PlrModeEvent(GAME *ptGame, UNIT *hPlayer, int eEventType)
{
#line 1660
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)scpfnEventFunctions[eEventType]), "scpfnEventFunctions[eEventType]");
	scpfnEventFunctions[eEventType](ptGame, hPlayer);
}
