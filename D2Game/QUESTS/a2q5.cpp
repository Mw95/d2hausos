#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a2q5.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "QUESTS.h"
#include "a2q5.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"

struct A2Q5RECORD
{
 	BYTE		bBossKilled;
 	BYTE		bBossSeen;
 	BYTE		unobserved_02[2];
 	GAMEROOM	*hBossRoom;
 	BYTE		bTimerPending;
 	BYTE		bEventsSent;
 	BYTE		unobserved_0a[2];
};
typedef struct A2Q5RECORD A2Q5RECORD;

static QUESTNPCTABLE sgtA2Q5Npcs[4] =
{
	{ { { 176, 414, { 0, 0 }, 2 }, { 175, 413, { 0, 0 }, 2 }, { 198, 408, { 0, 0 }, 2 },
		{ 199, 417, { 0, 0 }, 2 }, { 177, 410, { 0, 0 }, 2 }, { 202, 411, { 0, 0 }, 2 },
		{ 244, 418, { 0, 0 }, 2 }, { 210, 412, { 0, 0 }, 2 }, { 201, 409, { 0, 0 }, 2 },
		{ 200, 415, { 0, 0 }, 2 }, { 178, 416, { 0, 0 }, 2 } }, 11 },
	{ { { 176, 427, { 0, 0 }, 1 }, { 175, 424, { 0, 0 }, 1 }, { 198, 419, { 0, 0 }, 1 },
		{ 199, 423, { 0, 0 }, 1 }, { 177, 422, { 0, 0 }, 1 }, { 202, 426, { 0, 0 }, 1 },
		{ 244, 429, { 0, 0 }, 1 }, { 210, 425, { 0, 0 }, 1 }, { 201, 421, { 0, 0 }, 1 },
		{ 200, 420, { 0, 0 }, 1 }, { 178, 428, { 0, 0 }, 1 } }, 11 },
	{ { { 176, 427, { 0, 0 }, 2 }, { 175, 424, { 0, 0 }, 2 }, { 198, 419, { 0, 0 }, 2 },
		{ 199, 423, { 0, 0 }, 2 }, { 177, 422, { 0, 0 }, 2 }, { 202, 426, { 0, 0 }, 2 },
		{ 244, 429, { 0, 0 }, 2 }, { 210, 425, { 0, 0 }, 2 }, { 201, 421, { 0, 0 }, 2 },
		{ 200, 420, { 0, 0 }, 2 }, { 178, 428, { 0, 0 }, 2 } }, 11 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static int sgnA2Q5NpcTable[4] = { -1, 0, 1, 2 };

BYTE __fastcall A2Q5Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall A2Q5PartyCredit(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A2Q5Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q5NpcMessage(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A2Q5SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A2Q5UnitKilled(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A2Q5KilledNear(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A2Q5Timer(GAME *ptGame, QUEST *pQuest);
BOOL __fastcall A2Q5KilledEvent(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q5KilledParty(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q5KilledFlag(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A2Q5ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q5PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A2Q5UpdatePlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q6Event(GAME *ptGame, UNIT *hUnit, void *pArg);

void __fastcall A2Q4LevelObjects(GAME *ptGame  , DWORD dwLevelId)
{
	QUEST *pQuest;
	A2Q4RECORD *pQuestRecord;

	pQuest = QuestsFind(ptGame, 11);
	if (!pQuest)
		return;
	pQuestRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
	if (dwLevelId == 74)
	{
		if (!pQuestRecord->wBasementPortalMode)
		{
			pQuestRecord->wBasementPortalMode = 1;
			pQuestRecord->wArcanePortalMode = 2;
		}
	}
	else if (dwLevelId == 54)
	{
		if (!pQuestRecord->wArcanePortalMode)
		{
			pQuestRecord->wArcanePortalMode = 1;
			pQuestRecord->wBasementPortalMode = 2;
		}
	}
}

BYTE __fastcall A2Q5Active(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer, void *pPlayerRecord)
{
	if ((nNpcClass == 176 || nNpcClass == 175 || nNpcClass == 199 || nNpcClass == 177 || nNpcClass == 202
		|| nNpcClass == 244 || nNpcClass == 210 || nNpcClass == 201 || nNpcClass == 200
		|| nNpcClass == 178)
		&& D2CommonStepTestGridBit(pPlayerRecord, 13, 1) == 1)
		return TRUE;
	return FALSE;
}

void __fastcall A2Q5PartyCredit(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	DWORD dwLevel;

	if (!QuestsFind(ptGame, 12))
		return;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 13, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, 13, 1) == 1)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	dwLevel = D2CommonDungeonRoomGetLevelId(hRoom);
	if (!dwLevel)
		return;
	if (D2CommonGetActNum(dwLevel) != 1)
		return;
	D2CommonQuestRecordSetBit(pRecord, 13, 13);
	D2CommonQuestRecordSetBit(pRecord, 13, 1);
}

void __fastcall A2Q5Init(QUEST *pQuest  )
{
	A2Q5RECORD *pRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->ptNpcTables = sgtA2Q5Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->bState = 0;
#line 528
	pRecord = (A2Q5RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A2Q5RECORD));
	pQuest->pQuestRecord = pRecord;
	pQuest->eFilter = 13;
	pQuest->pStatusFilter = NULL;
	pQuest->bDoneStateMin = 2;
	pQuest->pActiveFilter = A2Q5Active;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A2Q5ChangedLevel;
	pQuest->pCallback[11] = A2Q5NpcMessage;
	pQuest->pCallback[10] = A2Q5PlayerDone;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A2Q5PlayerStarted;
	pQuest->pCallback[8] = A2Q5UnitKilled;
	pQuest->pCallback[0] = A2Q5Talk;
	pRecord->bBossKilled = 0;
	pRecord->bBossSeen = 0;
	pRecord->bTimerPending = 0;
	pRecord->hBossRoom = NULL;
}

void __fastcall A2Q5Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	void *pRecord;
	int nNpcClass;
	DWORD dwDone;
	DWORD dwBit13;
	int nTable;

	ptGame = pqParam->ptGame;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	nNpcClass = (int)pqParam->hEventUnit->dwClassId;
	if (D2CommonStepTestGridBit(pRecord, 13, 1))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 1);
		return;
	}
	if (QuestsHasPlayer(pQuest, pqParam->hPlayer->dwId))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 2);
		return;
	}
	if (!pQuest->bActive)
		return;
	if (!pQuest->bState)
		return;
	dwDone = D2CommonStepTestGridBit(pRecord, 13, 0);
	dwBit13 = D2CommonStepTestGridBit(pRecord, 13, 13);
	if (dwDone == 1 && !dwBit13)
		return;
	if (pQuest->bState >= 2 && !dwBit13)
		return;
	nTable = sgnA2Q5NpcTable[pQuest->bState];
	if (nTable == -1)
		return;
	if ((DWORD)nTable >= 4)
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, nTable);
}

void __fastcall A2Q5NpcMessage(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	WORD wMessage;
	GAME *ptGame;
	void *pRecord;

	wMessage = (WORD)pqParam->field_18;
	if (wMessage != 427 && wMessage != 424 && wMessage != 419 && wMessage != 423 && wMessage != 422
		&& wMessage != 426 && wMessage != 429 && wMessage != 425 && wMessage != 421
		&& wMessage != 420
		&& wMessage != 428)
		return;
	ptGame = pqParam->ptGame;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 13, 1))
	{
		if (D2CommonStepTestGridBit(pRecord, 13, 13))
		{
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 13, NULL, A2Q5SendStatus, TRUE);
			QuestsSetRecordBit(pQuest->ptGame, 13, 13);
#line 244
			QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
		}
		QuestsPlayersAdd(&pQuest->tPlayers, pqParam->hPlayer->dwId);
		D2CommonQuestRecordSetBit(pRecord, 13, 0);
		D2CommonQuestRecordClearBit(pRecord, 13, 1);
	}
	QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
}

BOOL __fastcall A2Q5SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 13, 0) || D2CommonStepTestGridBit(pRecord, 13, 15))
		&& !D2CommonStepTestGridBit(pRecord, 13, 13) && !D2CommonStepTestGridBit(pRecord, 13, 14))
		return FALSE;
	QuestsSendStatus(ptGame, hUnit, 12);
	return FALSE;
}

void __fastcall A2Q5PlayerDone(QUEST *pQuest, QUESTPARAM *pqParam)
{
	QuestsPlayerDone(pQuest, pqParam);
}

void __fastcall A2Q5UnitKilled(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A2Q5RECORD *pQuestRecord;

	if (pQuest->bActive)
	{
#line 444
		QuestsDebugMessage(pQuest->ptGame, "killed summoner for quest", D2_THIS_FILE, __LINE__);
#line 445
		QuestsSetState(pQuest, 2, D2_THIS_FILE, __LINE__);
		pQuestRecord = (A2Q5RECORD *)pQuest->pQuestRecord;
		if (!pQuestRecord->bTimerPending)
		{
			pQuestRecord->bTimerPending = 1;
			QuestsAddTimer(pQuest, A2Q5Timer, 3);
		}
		pQuestRecord->bEventsSent = 0;
		pQuestRecord->bBossKilled = 1;
		pQuestRecord->hBossRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(pqParam->hEventUnit);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A2Q5KilledNear);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A2Q5KilledParty);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A2Q5KilledFlag);
	}
	QuestsSetAll(pQuest->ptGame, 7);
}

BOOL __fastcall A2Q5KilledNear(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	QUEST *pQuestInfo;
	A2Q5RECORD *pQuestRecord;
	BOOL fNear;
	GAMEROOM **phNear;
	DWORD dwNear;
	DWORD i;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	pQuestInfo = QuestsFind(ptGame, 12);
#line 269
	D2ASSERT(pQuestInfo != NULL);
	pQuestRecord = (A2Q5RECORD *)pQuestInfo->pQuestRecord;
	fNear = FALSE;
	if (hRoom == pQuestRecord->hBossRoom)
		fNear = TRUE;
	else
	{
		D2CommonDungeonRoomGetNear(D2CommonUnitGetPathRoom(hUnit), &phNear, &dwNear);
		for (i = 0; i < dwNear; i++)
		{
			if (phNear[i] == pQuestRecord->hBossRoom)
			{
				fNear = TRUE;
				break;
			}
		}
	}
	if (D2CommonStepTestGridBit(pRecord, 13, 0))
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 13, 1))
		return FALSE;
	if (!fNear)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 13, 13);
	D2CommonQuestRecordSetBit(pRecord, 13, 1);
	return FALSE;
}

BYTE __fastcall A2Q5Timer(GAME *ptGame  , QUEST *pQuest
)
{
	A2Q5RECORD *pQuestRecord;

	pQuestRecord = (A2Q5RECORD *)pQuest->pQuestRecord;
	if (!pQuestRecord->bEventsSent)
	{
		pQuestRecord->bEventsSent = 1;
		SUnitIterate(ptGame, 0, NULL, A2Q5KilledEvent);
		return FALSE;
	}
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 4, NULL, A2Q5SendStatus, TRUE);
	pQuestRecord->bTimerPending = 0;
	return TRUE;
}

BOOL __fastcall A2Q5KilledEvent(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	GAMEROOM *hRoom;

	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecordX, 13, 13))
		return FALSE;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	if (D2CommonDungeonRoomGetLevelId(hRoom) != 74)
		return FALSE;
	SUnitSetEvent(hUnit, 49, hUnit);
	return FALSE;
}

BOOL __fastcall A2Q5KilledParty(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	WORD wParty;

	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecordX, 13, 13))
	{
		wParty = SUnitGetParty(hUnit);
		if (wParty != 65535)
			PartyForEach(ptGame, wParty, A2Q5PartyCredit, NULL);
	}
	return FALSE;
}

BOOL __fastcall A2Q5KilledFlag(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 13, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 13, 1) == 1)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 13, 14);
	QuestsSendFlagInAct(hUnit, 12, 0);
	return FALSE;
}

void __fastcall A2Q5ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	if (pQuest->bActive && (int)pqParam->field_18 >= 40 && pqParam->field_14 == 40)
		QuestsPlayerLeft(pQuest, pqParam);
}

void __fastcall A2Q5PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	void *pRecord;

	ptGame = pqParam->ptGame;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0))
		return;
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 15))
		return;
	if (D2CommonStepTestGridBit(pRecord, 13, 2) != 1)
		return;
	pQuest->bStatus = 2;
	pQuest->bState = 1;
}

void __fastcall A2Q5BossSeen(GAME *ptGame  )
{
	QUEST *pQuest;
	A2Q5RECORD *pQuestRecord;

	pQuest = QuestsFind(ptGame, 12);
	if (!pQuest)
		return;
	if (pQuest->bActive != 1)
		return;
	pQuestRecord = (A2Q5RECORD *)pQuest->pQuestRecord;
	if (!pQuestRecord->bBossSeen)
		pQuestRecord->bBossSeen = 1;
	if (!pQuest->bState)
	{
#line 566
		QuestsSetState(pQuest, 1, D2_THIS_FILE, __LINE__);
	}
	if (pQuest->bStatus >= 2)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 2, NULL, A2Q5SendStatus, TRUE);
	SUnitIterate(pQuest->ptGame, 0, NULL, A2Q5UpdatePlayer);
}

BOOL __fastcall A2Q5UpdatePlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	QUEST *pQuest;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 13, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 13, 1) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 12);
	if (pQuest && pQuest->bState == 1)
		D2CommonQuestRecordSetBit(pRecord, 13, 2);
	return FALSE;
}

void __fastcall A2Q5UnitAttached(GAME *ptGame  , QUEST *pQuest
, UNIT *hUnit  )
{
}

UNIT * __fastcall A2Q6EventAll(GAME *ptGame  )
{
	return SUnitIterate(ptGame, 0, NULL, A2Q6Event);
}

BOOL __fastcall A2Q6Event(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecordX, 14, 13))
		SUnitSetEvent(hUnit, 52, hUnit);
	return FALSE;
}

void __fastcall A2Q6PartyCredit(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;

	if (!QuestsFind(ptGame, 13))
		return;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 14, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, 14, 3) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, 14, 4) == 1)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	if (D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom)) != 1)
		return;
	D2CommonQuestRecordSetBit(pRecord, 14, 13);
	D2CommonQuestRecordSetBit(pRecord, 14, 3);
	ClientSetStep(SUnitGetClient(hUnit), 2, ptGame->bDifficultyLevel);
}

BYTE __fastcall A2Q6StatusFilter(QUEST *pQuest  , UNIT *hPlayer,
	void *pGameRecord, void *pPlayerRecord, BYTE *pbStatus)
{
	A2Q6RECORD *pQuestRecord;

	if (!D2CommonStepTestGridBit(pPlayerRecord, 7, 0))
	{
		*pbStatus = 0;
		return TRUE;
	}
	pQuestRecord = (A2Q6RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bForcedStatus)
	{
		*pbStatus = (BYTE)pQuestRecord->nForcedStatus;
		return TRUE;
	}
	*pbStatus = 0;
	if (D2CommonStepTestGridBit(pPlayerRecord, 14, 0))
		return TRUE;
	if (D2CommonStepTestGridBit(pPlayerRecord, 14, 3) == 1)
	{
		*pbStatus = 5;
		return TRUE;
	}
	if (D2CommonStepTestGridBit(pPlayerRecord, 14, 4) == 1)
	{
		*pbStatus = 6;
		return TRUE;
	}
	if (!pQuest->bActive)
		return TRUE;
	if (D2CommonStepTestGridBit(pPlayerRecord, 14, 5) == 1)
	{
		*pbStatus = pQuest->bStatus;
		return TRUE;
	}
	if (pQuest->bState < 3)
	{
		*pbStatus = pQuest->bStatus;
		return TRUE;
	}
	*pbStatus = 12;
	return TRUE;
}
