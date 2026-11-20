#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a1q2.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "QUESTS.h"
#include "a1q1.h"
#include "a1q2.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUNITNPC.h"

struct A1Q2RECORD
{
 	BYTE		field_00;
 	BYTE		field_01;
 	BYTE		bBossKilled;
 	BYTE		bStatus1Pending;
 	DWORD		field_04;
 	DWORD		dwBossId;
};
typedef struct A1Q2RECORD A1Q2RECORD;

static QUESTNPCTABLE sgtA1Q2Npcs[6] =
{
	{ { { 150, 81, { 0, 0 }, 1 } }, 1 },
	{ { { 150, 82, { 0, 0 }, 2 }, { 155, 86, { 0, 0 }, 2 }, { 154, 83, { 0, 0 }, 2 },
		{ 148, 85, { 0, 0 }, 2 }, { 147, 84, { 0, 0 }, 2 } }, 5 },
	{ { { 150, 87, { 0, 0 }, 2 }, { 155, 91, { 0, 0 }, 2 }, { 154, 89, { 0, 0 }, 2 },
		{ 148, 88, { 0, 0 }, 2 }, { 147, 90, { 0, 0 }, 2 } }, 5 },
	{ { { 150, 92, { 0, 0 }, 0 }, { 155, 96, { 0, 0 }, 2 }, { 154, 94, { 0, 0 }, 2 },
		{ 148, 93, { 0, 0 }, 2 }, { 147, 95, { 0, 0 }, 2 } }, 5 },
	{ { { 155, 96, { 0, 0 }, 2 }, { 150, 92, { 0, 0 }, 2 }, { 148, 93, { 0, 0 }, 2 },
		{ 147, 95, { 0, 0 }, 2 } }, 4 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static int sgnA1Q2NpcTable[6] = { -1, 0, 1, 2, 3, 4 };

BYTE __fastcall A1Q2Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall A1Q2Activate(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A1Q2SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A1Q2UpdatePlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A1Q2NpcMessage(QUEST *pQuestInfo, QUESTPARAM *pqParam);
void __fastcall A1Q2Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q2PlayerLeft(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q2UnitKilled(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A1Q2Timer(GAME *ptGame, QUEST *pQuest);
BOOL __fastcall A1Q2KilledNear(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A1Q2KilledFlag(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A1Q2KilledEvent(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A1Q2KilledParty(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A1Q2ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A1Q2Sequence(QUEST *pQuest);
void __fastcall A1Q2PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);

BYTE __fastcall A1Q2Active(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer  , void *pPlayerRecord)
{
	if (!(nNpcClass != 150)) {
		if (D2CommonStepTestGridBit(pPlayerRecord, pQuest->eFilter, 0) == 1)
			return FALSE;
		if (pQuest->bState == 1 && !D2CommonStepTestGridBit(pPlayerRecord, pQuest->eFilter, 1))
			return TRUE;
		if (D2CommonStepTestGridBit(pPlayerRecord, pQuest->eFilter, 1) == 1)
			return TRUE; }
	return FALSE;
}

void __fastcall A1Q2Init(QUEST *pQuest  )
{
	A1Q2RECORD *pRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A1Q2ChangedLevel;
	pQuest->pCallback[8] = A1Q2UnitKilled;
	pQuest->pCallback[0] = A1Q2Talk;
	pQuest->pCallback[11] = A1Q2NpcMessage;
	pQuest->pCallback[10] = A1Q2PlayerLeft;
	pQuest->pCallback[2] = A1Q2Activate;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A1Q2PlayerStarted;
	pQuest->pSequence = A1Q2Sequence;
	pQuest->ptNpcTables = sgtA1Q2Npcs;
	pQuest->eFilter = 2;
	pQuest->bCallbacksEnabled = 1;
#line 637
	pRecord = (A1Q2RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A1Q2RECORD));
	pQuest->pQuestRecord = pRecord;
	pQuest->pStatusFilter = NULL;
	pQuest->bDoneStateMin = 4;
	pQuest->dwNextQuestId = 4;
	pQuest->pActiveFilter = A1Q2Active;
	pRecord->field_01 = 0;
	pRecord->field_00 = 0;
	pRecord->bStatus1Pending = 0;
	pRecord->bBossKilled = 0;
	pRecord->dwBossId = 0;
	pQuest->bState = 0;
}

void __fastcall A1Q2Activate(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q2RECORD *pRecord;

	if ((WORD)pqParam->hEventUnit->dwClassId != 150)
		return;
	pRecord = (A1Q2RECORD *)pQuest->pQuestRecord;
	if (pRecord->bStatus1Pending != 1)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 1, NULL, A1Q2SendStatus, TRUE);
	pRecord->bStatus1Pending = 0;
	pQuest->pCallback[2] = NULL;
	SUnitIterate(pQuest->ptGame, 0, NULL, A1Q2UpdatePlayer);
}

BOOL __fastcall A1Q2SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 2, 0) || D2CommonStepTestGridBit(pRecord, 2, 15))
		&& !D2CommonStepTestGridBit(pRecord, 2, 13) && !D2CommonStepTestGridBit(pRecord, 2, 14))
		return FALSE;
	QuestsSendStatus(ptGame, hUnit, 2);
	return FALSE;
}

BOOL __fastcall A1Q2UpdatePlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	QUEST *pQuest;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 2, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 2, 1) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 2);
	if (!pQuest)
		return FALSE;
	if (pQuest->bState == 2)
	{
		D2CommonQuestRecordSetBit(pRecord, 2, 2);
		return FALSE;
	}
	if (pQuest->bState == 3)
	{
		if (pQuest->bStatus == 1)
		{
			D2CommonQuestRecordSetBit(pRecord, 2, 3);
			return FALSE;
		}
		D2CommonQuestRecordSetBit(pRecord, 2, 4);
	}
	return FALSE;
}

void __fastcall A1Q2NpcMessage(QUEST *pQuestInfo  , QUESTPARAM *pqParam
)
{
	WORD wMessage;
	WORD wNpcClass;
	void *pRecord;
	GAME *ptGame;
	UNIT *hPlayer;

	wMessage = (WORD)pqParam->field_18;
	wNpcClass = (WORD)pqParam->field_14;
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (wNpcClass != 150)
		return;
	if (wMessage == 81)
	{
		((A1Q2RECORD *)pQuestInfo->pQuestRecord)->bStatus1Pending = 1;
#line 192
		QuestsSetState(pQuestInfo, 2, D2_THIS_FILE, __LINE__);
		SUnitIterate(pQuestInfo->ptGame, 0, NULL, A1Q2UpdatePlayer);
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		return;
	}
	if (wMessage != 92)
		return;
	if (!D2CommonStepTestGridBit(pRecord, pQuestInfo->eFilter, 1))
		return;
	if (D2CommonStepTestGridBit(pRecord, pQuestInfo->eFilter, 13) && pQuestInfo->bState != 5)
	{
		pQuestInfo->bStatusFlags = 0;
		QuestsSetStatus(pQuestInfo, 13, NULL, A1Q2SendStatus, FALSE);
#line 205
		QuestsSetState(pQuestInfo, 5, D2_THIS_FILE, __LINE__);
#line 206
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		pQuestInfo->pSequence(pQuestInfo);
	}
	D2CommonQuestRecordSetBit(pRecord, pQuestInfo->eFilter, 0);
	D2CommonQuestRecordClearBit(pRecord, pQuestInfo->eFilter, 1);
	QuestsSendPlayerRecord(pQuestInfo->ptGame, pqParam->hPlayer);
	QuestsPlayersAdd(&pQuestInfo->tPlayers, pqParam->hPlayer->dwId);
	SUnitNpcGiveHire(pQuestInfo->ptGame, pqParam->hPlayer, 150);
	QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
}

void __fastcall A1Q2Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	DWORD dwDone;
	int nNpcClass;
	int nTable;
	GAME *ptGame;
	UNIT *hPlayer;

	nNpcClass = (int)pqParam->hEventUnit->dwClassId;
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	dwDone = D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0);
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 1))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 3);
		return;
	}
	if (QuestsHasPlayer(pQuest, pqParam->hPlayer->dwId) == 1)
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 4);
		return;
	}
	if (pQuest->bState && dwDone != 1 && pQuest->bState < 4)
	{
		nTable = sgnA1Q2NpcTable[pQuest->bState];
		if (nTable != -1 && (DWORD)nTable < 6)
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, nTable);
	}
}

void __fastcall A1Q2PlayerLeft(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	QuestsPlayersRemove(&pQuest->tPlayers, pqParam->hPlayer->dwId);
}

void __fastcall A1Q2UnitKilled(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q2RECORD *pRecord;

	if (!pQuest->bActive)
		return;
#line 445
	QuestsDebugMessage(pQuest->ptGame, "killed boss for quest", D2_THIS_FILE, __LINE__);
#line 446
	QuestsSetState(pQuest, 4, D2_THIS_FILE, __LINE__);
	pRecord = (A1Q2RECORD *)pQuest->pQuestRecord;
	pRecord->field_01 = 1;
	pRecord->bBossKilled = 1;
	pRecord->dwBossId = pqParam->hEventUnit->dwId;
	SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A1Q2KilledNear);
	SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A1Q2KilledParty);
	SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A1Q2KilledFlag);
	SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A1Q2KilledEvent);
	QuestsAddTimer(pQuest, A1Q2Timer, 15);
	pQuest->pCallback[2] = NULL;
	pRecord->field_00 = 1;
	pRecord->field_04 = 1;
	QuestsSetRecordBit(pqParam->ptGame, pQuest->eFilter, 13);
}

BYTE __fastcall A1Q2Timer(GAME *ptGame  , QUEST *pQuest
)
{
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 3, NULL, A1Q2SendStatus, TRUE);
	return TRUE;
}

BOOL __fastcall A1Q2KilledNear(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	GAMEROOM *hRoom;
	GAMEROOM *hBossRoom;
	GAMEROOM **phNear;
	DWORD dwNear;
	DWORD i;
	BOOL fNear;
	QUEST *pQuestInfo;
	A1Q2RECORD *pQuestRecord;
	void *pRecord;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	pQuestInfo = QuestsFind(ptGame, 2);
#line 307
	D2ASSERT(pQuestInfo != NULL);
	pQuestRecord = (A1Q2RECORD *)pQuestInfo->pQuestRecord;
	if (!pQuestRecord->bBossKilled)
		return FALSE;
	hBossRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(SUnitFind(ptGame, 1, pQuestRecord->dwBossId));
	if (hRoom)
	{
		fNear = FALSE;
		if (hRoom != hBossRoom)
		{
			D2CommonDungeonRoomGetNear(D2CommonUnitGetPathRoom(hUnit), &phNear, &dwNear);
			for (i = 0; i < dwNear; i++)
			{
				if (phNear[i] == hBossRoom)
				{
					fNear = TRUE;
					break;
				}
			}
		}
		else
			fNear = TRUE;
		pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];

		if (!D2CommonStepTestGridBit(pRecord, 2, 0) && !D2CommonStepTestGridBit(pRecord, 2, 1) && fNear)
		{
			D2CommonQuestRecordSetBit(pRecord, 2, 13);
			D2CommonQuestRecordSetBit(pRecord, 2, 1);
		}
	}
	return FALSE;
}

BOOL __fastcall A1Q2KilledFlag(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 2, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 2, 1) == 1)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 2, 14);
	QuestsSendFlagInAct(hUnit, 2, 0);
	return FALSE;
}

BOOL __fastcall A1Q2KilledEvent(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecordX, 2, 13) == 1)
		SUnitSetEvent(hUnit, 32, hUnit);
	return FALSE;
}

BOOL __fastcall A1Q2KilledParty(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	WORD wParty;

	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecordX, 2, 13))
	{
		wParty = SUnitGetParty(hUnit);
		if (wParty != 65535)
			PartyForEach(ptGame, wParty, A1Q2PartyCredit, NULL);
	}
	return FALSE;
}

void __fastcall A1Q2ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	BYTE bUpdate;
	UNIT *hPlayer;
	GAME *ptGame;
	DWORD dwEvent;
	DWORD dwArea;

	dwEvent = pqParam->field_14;
	dwArea = pqParam->field_18;
	if (dwArea == 17 && pQuest->bActive == 1)
	{
		bUpdate = FALSE;
		if (pQuest->bState < 3)
		{
#line 487
			QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
			pQuest->bStatusFlags = 0;
			bUpdate = TRUE;
		}
		if (pQuest->bStatus == 1 || pQuest->bStatus == 0)
		{
			QuestsSetStatus(pQuest, 2, NULL, A1Q2SendStatus, TRUE);
			bUpdate = TRUE;
		}
		if (bUpdate)
			SUnitIterate(pQuest->ptGame, 0, NULL, A1Q2UpdatePlayer);
		if (bUpdate)
			return;
	}
	if (dwEvent != 1)
		return;
	QuestsPlayerLeft(pQuest, pqParam);
	if (pQuest->bState != 2)
		return;
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 1) == 1)
		return;
#line 517
	QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
	SUnitIterate(pQuest->ptGame, 0, NULL, A1Q2UpdatePlayer);
}

BYTE __fastcall A1Q2Sequence(QUEST *pQuestInfo  )
{
	QUEST *pNextQuest;

	if (!pQuestInfo->bState && pQuestInfo->bActive == 1)
	{
#line 558
		QuestsSetState(pQuestInfo, 1, D2_THIS_FILE, __LINE__);
		return TRUE;
	}
	if (pQuestInfo->bState != 5 && pQuestInfo->bActive)
		return TRUE;
	pNextQuest = QuestsFind(pQuestInfo->ptGame, (int)pQuestInfo->dwNextQuestId);
	if (pNextQuest)
	{
#line 569
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		return pNextQuest->pSequence(pNextQuest);
	}
	return FALSE;
}

void __fastcall A1Q2PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0))
		return;
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 15))
		return;
	if (D2CommonStepTestGridBit(pRecord, 2, 4) == 1)
	{
		pQuest->bStatus = 2;
		pQuest->bState = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 2, 3) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 2, 2) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 2;
	}
}
