#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a4q3.cpp"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/MonsterMode.h"
#include "../MONSTER/MonsterMsg.h"
#include "../UNIT/Party.h"
#include "QUESTS.h"
#include "a1q2.h"
#include "a4q2.h"
#include "a4q3.h"
#include "../UNIT/SUnit.h"

#define UNIT_MONSTER		1
#define UNIT_OBJECT			2
#define A4Q3_LEVEL			108
#define A4Q3_LEVEL_ENTERED	103
#define A4Q3_NPC			246
#define A4Q3_OBJECT			131
#define A4Q3_CODE_HFH		543712872
#define A4Q3_CODE_MSS		544437101
#define PARTY_NONE			65535

static QUESTNPCTABLE sgtA4Q3Npcs[5] =
{
	{ { { 246, 678, { 0, 0 }, 1 } }, 1 },
	{ { { 246, 679, { 0, 0 }, 1 } }, 1 },
	{ { { 246, 680, { 0, 0 }, 1 } }, 1 },
	{ { { 246, 680, { 0, 0 }, 2 } }, 1 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

int __fastcall A4Q3OperateObject(OBJOPERATE *pObjectParam);
void __fastcall A4Q3CreditPartyMember(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A4Q3SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A4Q3FlagInAct(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A4Q3Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall A4Q3ItemAcquired(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A4Q3Sequence(QUEST *pQuestThis);
void __fastcall A4Q3ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A4Q3FlagPlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A4Q3NpcMessage(QUEST *pQuestInfo, QUESTPARAM *pqParam);
void __fastcall A4Q3Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A4Q3Activate(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A4Q3PlayerDropped(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A4Q3PlayerJoined(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A4Q3PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A4Q3UnitKilled(QUEST *pQuest, QUESTPARAM *pqParam);

void __fastcall A4Q3ReleaseMonsters(GAME *ptGame  ,
	A4Q2RECORD *pQuestRecord)
{
	MONMODECHANGE tReq;
	GAMEROOM *hRoom;
	UNIT *hUnit;
	UNIT *hNext;

	if (pQuestRecord->bMonstersReleased)
		return;
	pQuestRecord->bMonstersReleased = 1;
	QuestsSetAll(ptGame, 12);
	for (hRoom = (GAMEROOM *)D2CommonDungeonActGetFirstRoom(
		ptGame->ptAct[3]); hRoom; hRoom = hRoom->ptActNext)
	{
		if (D2CommonDungeonRoomGetLevelId(hRoom) != A4Q3_LEVEL)
			continue;
		for (hUnit = hRoom->ptUnitFirst; hUnit; hUnit = hNext)
		{
			hNext = hUnit->ptRoomNext;
			if (hUnit->eType != UNIT_MONSTER)
				continue;
			if (hUnit->dwClassId == 243)
				continue;
			if (SUnitIsDead(hUnit))
				continue;
			if (D2CommonStatsUnitGetAlignment(hUnit) != 2)
				continue;
			MonsterModeInit(hUnit, 0, &tReq);
			MonsterModeSet(ptGame, &tReq, TRUE);
		}
	}
}

void __fastcall A4Q3InitObject(OBJINIT *ptInit  )
{
	QUEST *pQuest;
	A4Q2RECORD *pQuestRecord;
	int i;

	pQuest = QuestsFind(ptInit->ptGame, 23);
	if (!pQuest)
		return;
	pQuestRecord = (A4Q2RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bDiabloSpawned)
		return;
	pQuestRecord->bSpawnObjectKnown = 1;
	pQuestRecord->dwSpawnObjectId = ptInit->hObject->dwId;
	for (i = 0; i < 5; i++)
		if (!pQuestRecord->abSealsOperated[i])
			return;
	if (pQuestRecord->nOtherKills != 3)
		return;
	A4Q3ReleaseMonsters(ptInit->ptGame, pQuestRecord);
	if (pQuestRecord->bTimerPending)
		return;
	pQuestRecord->field_18 = 0;
	pQuestRecord->bSpawnPhase = 1;
	pQuestRecord->bEndPhase = 0;
	pQuestRecord->bTimerPending = 1;
	QuestsAddTimer(pQuest, A4Q2Timer, 1);
}

int __fastcall A4Q3OperateSealFirst(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuest;
	A4Q2RECORD *pQuestRecord;
	GAMEROOM *hRoom;
	GAMEROOM *hSpotRoom;
	GAMECOORD *ptSpot;

	if (pObjectParam->hObject->dwMode)
		return 0;
	pQuest = QuestsFind(pObjectParam->ptGame, 23);
	if (!pQuest)
		return 0;
	pQuestRecord = (A4Q2RECORD *)pQuest->pQuestRecord;
	ptSpot = &pQuestRecord->tFirstSpawnSpot;
	D2CommonUnitGetPosition(pObjectParam->hObject, ptSpot);
	pQuestRecord->tFirstSpawnSpot.dwX -= 12;
	pQuestRecord->tFirstSpawnSpot.dwY -= 52;
	hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(pObjectParam->hObject),
		ptSpot->dwX, ptSpot->dwY);
	if (!hRoom)
		return 0;
	hSpotRoom = NULL;
	QuestsFindSpot(hRoom, ptSpot, 3, 7953, &hSpotRoom);
	if (!hSpotRoom)
		return 0;
	if (SUnitCreate(UNIT_OBJECT, A4Q3_OBJECT, (int)ptSpot->dwX,
			(int)ptSpot->dwY, pObjectParam->ptGame, hSpotRoom, 1, 0, 0))
		A4Q3OperateObject(pObjectParam);
	return 0;
}

int __fastcall A4Q3OperateSealSecond(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuest;
	A4Q2RECORD *pQuestRecord;
	GAMEROOM *hRoom;
	GAMEROOM *hSpotRoom;
	GAMECOORD *ptSpot;

	if (pObjectParam->hObject->dwMode)
		return 0;
	pQuest = QuestsFind(pObjectParam->ptGame, 23);
	if (!pQuest)
		return 0;
	pQuestRecord = (A4Q2RECORD *)pQuest->pQuestRecord;
	ptSpot = &pQuestRecord->tSecondSpawnSpot;
	D2CommonUnitGetPosition(pObjectParam->hObject, ptSpot);
	pQuestRecord->tSecondSpawnSpot.dwX -= 39;
	pQuestRecord->tSecondSpawnSpot.dwY += 33;
	hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(pObjectParam->hObject),
		ptSpot->dwX, ptSpot->dwY);
	if (!hRoom)
		return 0;
	hSpotRoom = NULL;
	QuestsFindSpot(hRoom, ptSpot, 3, 7953, &hSpotRoom);
	if (!hSpotRoom)
		return 0;
	if (SUnitCreate(UNIT_OBJECT, A4Q3_OBJECT, (int)ptSpot->dwX,
			(int)ptSpot->dwY, pObjectParam->ptGame, hSpotRoom, 1, 0, 0))
		A4Q3OperateObject(pObjectParam);
	return 0;
}

int __fastcall A4Q3OperateSealThird(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuest;
	A4Q2RECORD *pQuestRecord;
	GAMEROOM *hRoom;
	GAMEROOM *hSpotRoom;
	GAMECOORD *ptSpot;

	if (pObjectParam->hObject->dwMode)
		return 0;
	pQuest = QuestsFind(pObjectParam->ptGame, 23);
	if (!pQuest)
		return 0;
	pQuestRecord = (A4Q2RECORD *)pQuest->pQuestRecord;
	ptSpot = &pQuestRecord->tThirdSpawnSpot;
	D2CommonUnitGetPosition(pObjectParam->hObject, ptSpot);
	pQuestRecord->tThirdSpawnSpot.dwX += 32;
	pQuestRecord->tThirdSpawnSpot.dwY += 16;
	hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(pObjectParam->hObject),
		ptSpot->dwX, ptSpot->dwY);
	if (!hRoom)
		return 0;
	hSpotRoom = NULL;
	QuestsFindSpot(hRoom, ptSpot, 3, 7953, &hSpotRoom);
	if (!hSpotRoom)
		return 0;
	if (SUnitCreate(UNIT_OBJECT, A4Q3_OBJECT, (int)ptSpot->dwX,
			(int)ptSpot->dwY, pObjectParam->ptGame, hSpotRoom, 1, 0, 0))
		A4Q3OperateObject(pObjectParam);
	return 0;
}

int __fastcall A4Q3OperateObject(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuest;
	A4Q2RECORD *pQuestRecord;
	MISSOBJECTSTXT *ptObjectTxt;
	int i;

	if (pObjectParam->hObject->dwMode)
		return 0;
	D2CommonUnitSetMode(pObjectParam->hObject, 1);
	ptObjectTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
	EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
		pObjectParam->ptGame->nFrame + (int)((DWORD)ptObjectTxt->FrameCnt[1] >> 7));
	pQuest = QuestsFind(pObjectParam->ptGame, 23);
	if (!pQuest)
		return 0;
	pQuestRecord = (A4Q2RECORD *)pQuest->pQuestRecord;
	switch (pObjectParam->hObject->dwClassId)
	{
	case 392:
		pQuestRecord->abSealsOperated[0] = 1;
		break;
	case 393:
		pQuestRecord->abSealsOperated[1] = 1;
		break;
	case 394:
		pQuestRecord->abSealsOperated[2] = 1;
		break;
	case 395:
		pQuestRecord->abSealsOperated[3] = 1;
		break;
	case 396:
		pQuestRecord->abSealsOperated[4] = 1;
		break;
	}
	for (i = 0; i < 5; i++)
		if (!pQuestRecord->abSealsOperated[i])
			return 0;
	if (pQuestRecord->nOtherKills != 3)
		return 0;
	A4Q3ReleaseMonsters(pObjectParam->ptGame, pQuestRecord);
	if (pQuestRecord->bTimerPending)
		return 0;
	pQuestRecord->field_18 = 0;
	pQuestRecord->bSpawnPhase = 1;
	pQuestRecord->bEndPhase = 0;
	pQuestRecord->bTimerPending = 1;
	QuestsAddTimer(pQuest, A4Q2Timer, 1);
	return 0;
}

static __inline void A4Q3SpawnI(GAME *ptGame, UNIT *hObject, GAMECOORD *ptSpot, int nClass)
{
	GAMEROOM *hRoom;

	hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hObject), ptSpot->dwX, ptSpot->dwY);
	if (hRoom && MonsterSpawnClass(ptGame, hRoom, nClass, (int)ptSpot->dwX, (int)ptSpot->dwY, 1))
		return;
	EventAdd(ptGame, hObject, 7, ptGame->nFrame + 10);
}

void __fastcall A4Q3ObjectEvent(GAME *ptGame  , UNIT *hObject
)
{
	QUEST *pQuest;
	A4Q2RECORD *pQuestRecord;
	GAMECOORD tCoord;

	D2CommonUnitGetPosition(hObject, &tCoord);
	pQuest = QuestsFind(ptGame, 23);
	if (!pQuest)
		return;
	pQuestRecord = (A4Q2RECORD *)pQuest->pQuestRecord;
	if (tCoord.dwX == pQuestRecord->tFirstSpawnSpot.dwX && tCoord.dwY == pQuestRecord->tFirstSpawnSpot.dwY)
		A4Q3SpawnI(ptGame, hObject, &pQuestRecord->tFirstSpawnSpot, 474);
	else if (tCoord.dwX == pQuestRecord->tSecondSpawnSpot.dwX && tCoord.dwY == pQuestRecord->tSecondSpawnSpot.dwY)
		A4Q3SpawnI(ptGame, hObject, &pQuestRecord->tSecondSpawnSpot, 475);
	else if (tCoord.dwX == pQuestRecord->tThirdSpawnSpot.dwX && tCoord.dwY == pQuestRecord->tThirdSpawnSpot.dwY)
		A4Q3SpawnI(ptGame, hObject, &pQuestRecord->tThirdSpawnSpot, 476);
}
BOOL __fastcall A4Q3MonsterKilled(GAME *ptGame  )
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptGame, 23);
	if (pQuest)
	{
		if (!((A4Q2RECORD *)pQuest->pQuestRecord)->bDiabloKilled)
			return FALSE;
	}
	return TRUE;
}

void __fastcall A4Q3InitObjectMode(OBJINIT *ptInit  )
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptInit->ptGame, 24);
	if (pQuest && pQuest->bActive)
	{
		D2CommonUnitSetMode(ptInit->hObject, ((A4Q3RECORD *)pQuest->pQuestRecord)->wHellforgeMode);
		return;
	}
	D2CommonUnitSetMode(ptInit->hObject, 3);
}

void __fastcall A4Q3CreditPartyMember(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuest;
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 27, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, 27, 1) == 1)
		return;
	QuestsRemoveItem(ptGame, hUnit, A4Q3_CODE_HFH);
	D2CommonQuestRecordSetBit(pRecord, 27, 13);
	D2CommonQuestRecordSetBit(pRecord, 27, 1);
	D2CommonQuestRecordClearMidBits(pRecord, 27);
	pQuest = QuestsFind(ptGame, 24);
	if (pQuest)
		((A4Q3RECORD *)pQuest->pQuestRecord)->nGemsPerDrop++;
}

int __fastcall A4Q3OperateHellforge(OBJOPERATE *pObjectParam  )
{
	GAME *ptGame;
	QUEST *pQuest;
	A4Q3RECORD *pQuestRecord;
	MISSOBJECTSTXT *ptObjectTxt;
	UNIT *hPlayer;
	UNIT *hItem;
	void *pRecord;
	WORD wParty;

	pQuest = QuestsFind(pObjectParam->ptGame, 24);
	if (!pQuest)
		return 0;
	pQuestRecord = (A4Q3RECORD *)pQuest->pQuestRecord;
	ptObjectTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->hObject->dwClassId);
	switch (pObjectParam->hObject->dwMode)
	{
	case 0:
		if (ITEMSFindByType(pObjectParam->hPlayer, A4Q3_CODE_MSS))
		{
			D2CommonUnitSetMode(pObjectParam->hObject, 1);
			EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
				pObjectParam->ptGame->nFrame + (int)((DWORD)ptObjectTxt->FrameCnt[1] >> 8));
			pQuestRecord->wHellforgeMode = 2;
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 2, NULL, A4Q3SendStatus, TRUE);
			QuestsRemoveItem(pObjectParam->ptGame, pObjectParam->hPlayer, A4Q3_CODE_MSS);
			return 0;
		}
		SUnitSetEvent(pObjectParam->hPlayer, 18, pObjectParam->hPlayer);
		if (pQuest->bActive && !pQuest->bState)
		{
#line 201
			QuestsSetState(pQuest, 1, D2_THIS_FILE, __LINE__);
		}
		return 0;
	case 2:
		hItem = ITEMSFindByType(pObjectParam->hPlayer, A4Q3_CODE_HFH);
		hPlayer = pObjectParam->hPlayer;
		if (!hItem)
			goto lbEvent;
		if (!hPlayer->ptInventory)
		{
			SUnitSetEvent(hPlayer, 18, hPlayer);
			return 0;
		}
		hItem = (UNIT *)D2CommonINVENTORYFindEquippedMatch(hPlayer->ptInventory);
		if (!hItem)
		{
			SUnitSetEvent(pObjectParam->hPlayer, 18, pObjectParam->hPlayer);
			return 0;
		}
		if (((ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId))->code != A4Q3_CODE_HFH)
		{
			hPlayer = pObjectParam->hPlayer;
lbEvent:
			SUnitSetEvent(hPlayer, 18, hPlayer);
			return 0;
		}
		if (++pQuestRecord->nOperations <= 2)
			return 0;
		D2CommonUnitSetMode(pObjectParam->hObject, 3);
		pQuestRecord->wHellforgeMode = 3;
		QuestsRemoveItem(pObjectParam->ptGame, pObjectParam->hPlayer, A4Q3_CODE_HFH);
		ptGame = pObjectParam->ptGame;
		hPlayer = pObjectParam->hPlayer;
		pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
		D2CommonQuestRecordSetBit(pRecord, 27, 13);
		D2CommonQuestRecordSetBit(pRecord, 27, 1);
		D2CommonQuestRecordClearMidBits(pRecord, pQuest->eFilter);
		pQuestRecord->bFinished = 1;
		pQuestRecord->bGemsPending = 1;
		pQuestRecord->nDropsLeft = 4;
		pQuestRecord->nGemsPerDrop = 1;
		wParty = SUnitGetParty(pObjectParam->hPlayer);
		if (wParty != PARTY_NONE)
			PartyForEach(pObjectParam->ptGame, wParty, A4Q3CreditPartyMember, NULL);
#line 253
		QuestsSetState(pQuest, 4, D2_THIS_FILE, __LINE__);
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 13, NULL, A4Q3SendStatus, TRUE);
		EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 7,
			pObjectParam->ptGame->nFrame + (int)((DWORD)ptObjectTxt->FrameCnt[3] >> 8));
		SUnitIterate(pObjectParam->ptGame, 0, pObjectParam->hPlayer, A4Q3FlagInAct);
		QuestsSetAll(pObjectParam->ptGame, 14);
		return 0;
	}
	return 0;
}

BOOL __fastcall A4Q3SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 27, 0) || D2CommonStepTestGridBit(pRecord, 27, 15))
		&& !D2CommonStepTestGridBit(pRecord, 27, 13)
		&& !D2CommonStepTestGridBit(pRecord, 27, 14))
		return FALSE;
	QuestsSendStatus(ptGame, hUnit, 24);
	return FALSE;
}

BOOL __fastcall A4Q3FlagInAct(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 27, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 27, 1) == 1)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 27, 14);
	QuestsSendFlagInAct(hUnit, 24, 0);
	return FALSE;
}

BYTE __fastcall A4Q3Active(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer, void *pPlayerRecord)
{
	if (nNpcClass == A4Q3_NPC && !D2CommonStepTestGridBit(pPlayerRecord, 27, 0)
		&& !D2CommonStepTestGridBit(pPlayerRecord, 27, 1) && pQuest->bActive && pQuest->bState == 1)
		return TRUE;
	return FALSE;
}

void __fastcall A4Q3Init(QUEST *pQuest  )
{
	A4Q3RECORD *pQuestRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->pQuestRecord = NULL;
	pQuest->bStatus = 0;
	pQuest->bState = 0;
	pQuest->pStatusFilter = NULL;
	pQuest->ptNpcTables = sgtA4Q3Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->pSequence = A4Q3Sequence;
	pQuest->dwNextQuestId = 23;
	pQuest->eFilter = 27;
	pQuest->bDoneStateMin = 4;
	pQuest->pActiveFilter = A4Q3Active;
	pQuest->pCallback[10] = A1Q2PlayerLeft;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A4Q3ChangedLevel;
	pQuest->pCallback[0] = A4Q3Talk;
	pQuest->pCallback[2] = A4Q3Activate;
	pQuest->pCallback[11] = A4Q3NpcMessage;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A4Q3PlayerStarted;
	pQuest->pCallback[QUESTCALLBACK_PLAYERJOINEDGAME] = A4Q3PlayerJoined;
	pQuest->pCallback[QUESTCALLBACK_PLAYERDROPPEDWITHQUESTITEM] = A4Q3PlayerDropped;
	pQuest->pCallback[8] = A4Q3UnitKilled;
	pQuest->pCallback[4] = A4Q3ItemAcquired;
#line 763
	pQuestRecord = (A4Q3RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A4Q3RECORD));
	pQuest->pQuestRecord = pQuestRecord;
	for (i = 0; i < (int)(sizeof(A4Q3RECORD) / sizeof(DWORD)); i++)
		((DWORD *)pQuestRecord)[i] = 0;
}

void __fastcall A4Q3ItemAcquired(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	UNIT *hPlayer;
	GAME *ptGame;
	ptGame = pQuest->ptGame;
	hPlayer = pqParam->hPlayer;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 27, 0))
		return;
	if (D2CommonStepTestGridBit(pRecord, 27, 13))
		return;
	if (D2CommonStepTestGridBit(pRecord, 27, 1))
		return;
	if (!pQuest->bActive)
		return;
	if (pQuest->bStatus == 13)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 3, NULL, A4Q3SendStatus, TRUE);
	if (pQuest->bState)
		return;
#line 114
	QuestsSetState(pQuest, 1, D2_THIS_FILE, __LINE__);
}

BYTE __fastcall A4Q3Sequence(QUEST *pQuestThis  )
{
	QUEST *pQuest;

	if (!pQuestThis->bState && pQuestThis->bActive == 1)
	{
#line 301
		QuestsSetState(pQuestThis, 1, D2_THIS_FILE, __LINE__);
		return TRUE;
	}
	if (pQuestThis->bState != 5 && pQuestThis->bActive)
		return TRUE;
	pQuest = QuestsFind(pQuestThis->ptGame, (int)pQuestThis->dwNextQuestId);
	if (pQuest)
	{
#line 309
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuest->pSequence), "pQuest->pSequence");
		return pQuest->pSequence(pQuest);
	}
	return FALSE;
}

void __fastcall A4Q3ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	GAME *ptGame;
	UNIT *hPlayer;

	if (pqParam->field_14 != A4Q3_LEVEL_ENTERED)
		return;
	QuestsPlayerLeft(pQuest, pqParam);
	if (pQuest->bState != 2)
		return;
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 1) == 1)
		return;
#line 393
	QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
	SUnitIterate(pQuest->ptGame, 0, NULL, A4Q3FlagPlayer);
}

BOOL __fastcall A4Q3FlagPlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuest;
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 27, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 27, 1) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 24);
	if (!pQuest)
		return FALSE;
	if (pQuest->bState == 2)
	{
		D2CommonQuestRecordSetBit(pRecord, 27, 2);
		return FALSE;
	}
	if (pQuest->bState != 3)
		return FALSE;
	if (pQuest->bStatus == 1)
	{
		D2CommonQuestRecordSetBit(pRecord, 27, 3);
		return FALSE;
	}
	D2CommonQuestRecordSetBit(pRecord, 27, 4);
	return FALSE;
}

void __fastcall A4Q3NpcMessage(QUEST *pQuestInfo  , QUESTPARAM *pqParam
)
{
	A4Q3RECORD *pQuestRecord;
	void *pRecord;
	WORD wNpcClass;
	WORD wMessage;
	GAME *ptGame;
	UNIT *hPlayer;

	wMessage = (WORD)pqParam->field_18;
	wNpcClass = (WORD)pqParam->field_14;
	ptGame = pqParam->ptGame;
	hPlayer = pqParam->hPlayer;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (wNpcClass != A4Q3_NPC)
		return;
	pQuestRecord = (A4Q3RECORD *)pQuestInfo->pQuestRecord;
	if (wMessage == 678)
	{
		pQuestRecord->bCainMessageDone = 1;
#line 422
		QuestsSetState(pQuestInfo, 2, D2_THIS_FILE, __LINE__);
		SUnitIterate(pQuestInfo->ptGame, 0, NULL, A4Q3FlagPlayer);
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		return;
	}
	if (wMessage == 679)
	{
		if (!QuestsCreateItem(pqParam->ptGame, pqParam->hPlayer, A4Q3_CODE_MSS, 0, 2))
			return;
		pQuestRecord->bCainMessageDone = 1;
		if (pQuestInfo->bState < 2)
		{
#line 435
			QuestsSetState(pQuestInfo, 2, D2_THIS_FILE, __LINE__);
		}
		SUnitIterate(pQuestInfo->ptGame, 0, NULL, A4Q3FlagPlayer);
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		return;
	}
	if (wMessage != 680)
		return;
	if (!D2CommonStepTestGridBit(pRecord, pQuestInfo->eFilter, 1))
		return;
	if (D2CommonStepTestGridBit(pRecord, pQuestInfo->eFilter, 13) && pQuestInfo->bState != 5)
	{
		pQuestInfo->bStatusFlags = 0;
		QuestsSetStatus(pQuestInfo, 13, NULL, A4Q3SendStatus, FALSE);
#line 450
		QuestsSetState(pQuestInfo, 5, D2_THIS_FILE, __LINE__);
#line 451
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		pQuestInfo->pSequence(pQuestInfo);
	}
	D2CommonQuestRecordSetBit(pRecord, pQuestInfo->eFilter, 0);
	D2CommonQuestRecordClearBit(pRecord, pQuestInfo->eFilter, 1);
	QuestsPlayersAdd(&pQuestInfo->tPlayers, pqParam->hPlayer->dwId);
	QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
}

void __fastcall A4Q3Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	DWORD dwBit0;
	DWORD dwBit13;
	int nNpcClass;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	nNpcClass = (int)pqParam->hEventUnit->dwClassId;
	dwBit0 = D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0);
	dwBit13 = D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 13);
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 1))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 2);
		return;
	}
	if (QuestsHasPlayer(pQuest, pqParam->hPlayer->dwId) == 1)
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 3);
		return;
	}
	if (!pQuest->bState)
		return;
	if (dwBit0 == 1)
		return;
	if (pQuest->bState >= 4 && !dwBit13)
		return;
	if (D2CommonStepTestGridBit(pRecord, 27, 0))
		return;
	if (D2CommonStepTestGridBit(pRecord, 27, 1))
		return;
	if (!pQuest->bActive)
		return;
	if (pQuest->bState == 1)
	{
		if (ITEMSFindByType(pqParam->hPlayer, A4Q3_CODE_MSS))
		{
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 0);
			return;
		}
	}
	else if (ITEMSFindByType(pqParam->hPlayer, A4Q3_CODE_MSS))
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 1);
}

void __fastcall A4Q3Activate(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A4Q3RECORD *pQuestRecord;

	if ((WORD)pqParam->hEventUnit->dwClassId != A4Q3_NPC)
		return;
	pQuestRecord = (A4Q3RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bCainMessageDone != 1)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 1, NULL, A4Q3SendStatus, TRUE);
	pQuestRecord->bCainMessageDone = 0;
	pQuest->pCallback[2] = NULL;
	SUnitIterate(pQuest->ptGame, 0, NULL, A4Q3FlagPlayer);
}

void __fastcall A4Q3PlayerDropped(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A4Q3RECORD *pQuestRecord;

	pQuestRecord = (A4Q3RECORD *)pQuest->pQuestRecord;
	if (D2CommonITEMSGetTypeCode(pqParam->hEventUnit) == A4Q3_CODE_HFH)
		pQuestRecord->nHammers--;
}

void __fastcall A4Q3PlayerJoined(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A4Q3RECORD *pQuestRecord;

	pQuestRecord = (A4Q3RECORD *)pQuest->pQuestRecord;
	if (ITEMSFindByType(pqParam->hPlayer, A4Q3_CODE_HFH))
		pQuestRecord->nHammers++;
}

void __fastcall A4Q3PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A4Q3RECORD *pQuestRecord;
	GAME *ptGame = pqParam->ptGame;
	void *pRecord;

	UNIT *f61 = pqParam->hPlayer;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(f61))
		->apQuestRecords[ptGame->bDifficultyLevel];
	pQuestRecord = (A4Q3RECORD *)pQuest->pQuestRecord;
	if (ITEMSFindByType(pqParam->hPlayer, A4Q3_CODE_HFH))
		pQuestRecord->nHammers++;
	if (!(D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0)))
	{
		if (!(D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 15)))
		{
			if (D2CommonStepTestGridBit(pRecord, 27, 4) == 1)
			{
				pQuest->bStatus = 2;
				pQuest->bState = 3;
				return;
			}
			if (D2CommonStepTestGridBit(pRecord, 27, 3) == 1)
			{
				pQuest->bStatus = 1;
				pQuest->bState = 3;
				return;
			}
			if (D2CommonStepTestGridBit(pRecord, 27, 2) == 1) {
				pQuest->bStatus = 1;
				pQuest->bState = 2; }
		} }
}

void __fastcall A4Q3UnitKilled(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A4Q3RECORD *pQuestRecord;
	int nLevel;

	pQuestRecord = (A4Q3RECORD *)pQuest->pQuestRecord;
	if (!pQuest->bActive)
		return;
	pqParam->hEventUnit->dwDropItemCode = A4Q3_CODE_HFH;
	if (ITEMSDropFromUnit(pqParam->ptGame, pqParam->hEventUnit, 7, &nLevel, NULL, 44, FALSE))
		pQuestRecord->nHammers++;
}
