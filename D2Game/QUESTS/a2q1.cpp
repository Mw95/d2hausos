#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a2q1.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../ITEMS/Items.h"
#include "QUESTS.h"
#include "a2q1.h"
#include "a2q5.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"

struct A2Q1RECORD
{
 	BYTE		bBossKilled;
 	BYTE		unobserved_01[3];
 	GAMEROOM	*hBossRoom;
 	BYTE		bStatus1Pending;
 	BYTE		bStatus2Sent;
 	BYTE		bTimerPending;
 	BYTE		unobserved_0b;
 	int			nDropCount;
 	BYTE		bStarterCredited;
 	BYTE		unobserved_11[3];
};
typedef struct A2Q1RECORD A2Q1RECORD;

#define A2Q1_CODE_BOOK		544437089
#define A2Q2_CODE_STAFF		544502632
#define A2Q2_CODE_BOX		544763746
#define A2Q2_CODE_SCROLL	540111476
#define A2Q2_CODE_AMULET	544237942
#define A2Q2_CODE_SHAFT		543585133

static QUESTNPCTABLE sgtA2Q1Npcs[9] =
{
	{ { { 176, 304, { 0, 0 }, 0 } }, 1 },
	{ { { 176, 310, { 0, 0 }, 2 }, { 175, 307, { 0, 0 }, 2 }, { 198, 305, { 0, 0 }, 2 },
		{ 199, 306, { 0, 0 }, 2 }, { 177, 312, { 0, 0 }, 2 }, { 202, 313, { 0, 0 }, 2 },
		{ 244, 314, { 0, 0 }, 2 }, { 210, 311, { 0, 0 }, 2 }, { 200, 308, { 0, 0 }, 2 },
		{ 178, 309, { 0, 0 }, 2 } }, 10 },
	{ { { 176, 317, { 0, 0 }, 2 }, { 175, 315, { 0, 0 }, 2 }, { 198, 318, { 0, 0 }, 2 },
		{ 199, 320, { 0, 0 }, 2 }, { 177, 322, { 0, 0 }, 2 }, { 202, 321, { 0, 0 }, 2 },
		{ 244, 324, { 0, 0 }, 2 }, { 210, 316, { 0, 0 }, 2 }, { 200, 319, { 0, 0 }, 2 },
		{ 178, 323, { 0, 0 }, 2 } }, 10 },
	{ { { 176, 334, { 0, 0 }, 1 }, { 198, 325, { 0, 0 }, 2 }, { 199, 330, { 0, 0 }, 2 },
		{ 177, 326, { 0, 0 }, 2 }, { 202, 327, { 0, 0 }, 2 }, { 244, 333, { 0, 0 }, 2 },
		{ 210, 328, { 0, 0 }, 2 }, { 200, 329, { 0, 0 }, 2 }, { 178, 332, { 0, 0 }, 2 } }, 9 },
	{ { { 175, 331, { 0, 0 }, 2 }, { 176, 334, { 0, 0 }, 2 }, { 198, 325, { 0, 0 }, 2 },
		{ 199, 330, { 0, 0 }, 2 }, { 177, 326, { 0, 0 }, 2 }, { 244, 333, { 0, 0 }, 2 },
		{ 210, 328, { 0, 0 }, 2 }, { 200, 329, { 0, 0 }, 2 }, { 178, 332, { 0, 0 }, 2 } }, 9 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static int sgnA2Q1NpcTable[7] = { -1, 0, 1, 2, 3, 4, 0 };

BYTE __fastcall A2Q1Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall A2Q1PartyCredit(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A2Q1Activate(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A2Q1SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A2Q1NpcMessage(QUEST *pQuestInfo, QUESTPARAM *pqParam);
BOOL __fastcall A2Q1UpdatePlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A2Q1Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q1UnitKilled(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A2Q1KilledFlag(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q1KilledNear(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A2Q1Timer(GAME *ptGame, QUEST *pQuest);
BOOL __fastcall A2Q1CountBooks(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A2Q1ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A2Q1Sequence(QUEST *pQuestInfo);
void __fastcall A2Q1PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);

void __fastcall A2Q0PlayerLeft(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	QuestsPlayersRemove((QUESTPLAYERS *)pQuest->pQuestRecord, pqParam->hPlayer->dwId);
}

void __fastcall A2Q0PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	UNIT *hPlayer;
	void *pRecord;

	ptGame = pqParam->ptGame;
	hPlayer = pqParam->hPlayer;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 8, 0) == 1)
		QuestsSetRecordBit(pQuest->ptGame, 8, 13);
}

BYTE __fastcall A2Q1Active(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer  , void *pPlayerRecord)
{
	if (nNpcClass == 176)
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, pQuest->eFilter, 0) == 1)
			return FALSE;
		if (pQuest->bActive && pQuest->bState == 1
			&& !D2CommonStepTestGridBit(pPlayerRecord, pQuest->eFilter, 15))
			return TRUE;
		if (D2CommonStepTestGridBit(pPlayerRecord, pQuest->eFilter, 1) == 1)
			return TRUE;
	}
	return FALSE;
}

void __fastcall A2Q1PartyCredit(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	DWORD dwLevel;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 9, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, 9, 1) == 1)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	dwLevel = D2CommonDungeonRoomGetLevelId(hRoom);
	if (!dwLevel)
		return;
	if (D2CommonGetActNum(dwLevel) != 1)
		return;
	D2CommonQuestRecordSetBit(pRecord, 9, 13);
	D2CommonQuestRecordSetBit(pRecord, 9, 1);
	D2CommonQuestRecordSetBit(pRecord, 9, 5);
}

void __fastcall A2Q1Init(QUEST *pQuest  )
{
	A2Q1RECORD *pRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A2Q1ChangedLevel;
	pQuest->bCallbacksEnabled = 1;
	pQuest->bState = 1;
	pQuest->pCallback[8] = A2Q1UnitKilled;
	pQuest->pCallback[0] = A2Q1Talk;
	pQuest->pCallback[11] = A2Q1NpcMessage;
	pQuest->pCallback[10] = A2Q5PlayerDone;
	pQuest->pCallback[2] = A2Q1Activate;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A2Q1PlayerStarted;
	pQuest->dwNextQuestId = 13;
	pQuest->pSequence = A2Q1Sequence;
	pQuest->ptNpcTables = sgtA2Q1Npcs;
#line 666
	pRecord = (A2Q1RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A2Q1RECORD));
	pQuest->pQuestRecord = pRecord;
	pQuest->eFilter = 9;
	pQuest->pStatusFilter = NULL;
	pQuest->bDoneStateMin = 4;
	pQuest->pActiveFilter = A2Q1Active;
	for (i = 0; i < (int)(sizeof(A2Q1RECORD) / sizeof(DWORD)); i++)
		((DWORD *)pRecord)[i] = 0;
	pRecord->hBossRoom = NULL;
	pRecord->bBossKilled = 0;
}

void __fastcall A2Q1Activate(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A2Q1RECORD *pRecord;

	if ((WORD)pqParam->hEventUnit->dwClassId != 176)
		return;
	pRecord = (A2Q1RECORD *)pQuest->pQuestRecord;
	if (pRecord->bStatus1Pending != 1)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 1, NULL, A2Q1SendStatus, TRUE);
	pRecord->bStatus1Pending = 0;
	pQuest->pCallback[2] = NULL;
}

BOOL __fastcall A2Q1SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 9, 0) || D2CommonStepTestGridBit(pRecord, 9, 15))
		&& !D2CommonStepTestGridBit(pRecord, 9, 13) && !D2CommonStepTestGridBit(pRecord, 9, 14))
		return FALSE;
	QuestsSendStatus(ptGame, hUnit, 8);
	return FALSE;
}

void __fastcall A2Q1NpcMessage(QUEST *pQuestInfo  , QUESTPARAM *pqParam
)
{
	WORD wMessage;
	WORD wNpcClass;
	GAME *ptGame;
	void *pRecord;
	A2Q1RECORD *pQuestRecord;
	QUEST *pNextQuest;

	UNIT *hPlayerX;
	wMessage = (WORD)pqParam->field_18;
	wNpcClass = (WORD)pqParam->field_14;
	ptGame = pqParam->ptGame;
	hPlayerX = pqParam->hPlayer;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayerX))->apQuestRecords[ptGame->bDifficultyLevel];
	if (wNpcClass != 176)
		return;
	pQuestRecord = (A2Q1RECORD *)pQuestInfo->pQuestRecord;
	if (wMessage == 304)
	{
		pQuestRecord->bStatus1Pending = 1;
#line 239
		QuestsSetState(pQuestInfo, 2, D2_THIS_FILE, __LINE__);
		SUnitIterate(pQuestInfo->ptGame, 0, NULL, A2Q1UpdatePlayer);
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		return;
	}
	if (wMessage != 334)
		return;
	if (!D2CommonStepTestGridBit(pRecord, pQuestInfo->eFilter, 1))
		return;
	if (D2CommonStepTestGridBit(pRecord, pQuestInfo->eFilter, 13))
	{
		if (pQuestInfo->bState != 5)
		{
			pQuestInfo->bStatusFlags = 0;
			QuestsSetStatus(pQuestInfo, 13, NULL, A2Q1SendStatus, TRUE);
			pQuestInfo->pCallback[2] = NULL;
#line 253
			QuestsSetState(pQuestInfo, 5, D2_THIS_FILE, __LINE__);
#line 254
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
			pQuestInfo->pSequence(pQuestInfo);
		}
		if (!pQuestInfo->bActive)
			QuestsSetRecordBit(pQuestInfo->ptGame, 9, 13);
	}
	else if (pQuestRecord->bStarterCredited)
	{
		pNextQuest = QuestsFind(pQuestInfo->ptGame, (int)pQuestInfo->dwNextQuestId);
		if (pNextQuest)
		{
#line 269
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
			pNextQuest->pSequence(pNextQuest);
		}
	}
	D2CommonQuestRecordSetBit(pRecord, pQuestInfo->eFilter, 0);
	D2CommonQuestRecordClearBit(pRecord, pQuestInfo->eFilter, 1);
	QuestsPlayersAdd(&pQuestInfo->tPlayers, pqParam->hPlayer->dwId);
	QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
}

BOOL __fastcall A2Q1UpdatePlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	QUEST *pQuest;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 9, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 9, 1) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 8);
	if (!pQuest)
		return FALSE;
	if (pQuest->bState == 2)
	{
		D2CommonQuestRecordSetBit(pRecord, 9, 2);
		return FALSE;
	}
	if (pQuest->bState != 3)
		return FALSE;
	if (pQuest->bStatus == 1)
	{
		D2CommonQuestRecordSetBit(pRecord, 9, 3);
		return FALSE;
	}
	D2CommonQuestRecordSetBit(pRecord, 9, 4);
	return FALSE;
}

void __fastcall A2Q1Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	int nNpcClass;
	void *pRecord;
	DWORD dwBit13;
	DWORD dwDone;
	int nTable;

	UNIT *hPlayerX;
	ptGame = pqParam->ptGame;
	nNpcClass = pqParam->hEventUnit->dwClassId;
	hPlayerX = pqParam->hPlayer;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayerX))->apQuestRecords[ptGame->bDifficultyLevel];
	dwBit13 = D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 13);
	dwDone = D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0);
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 1))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 3);
		return;
	}
	if (QuestsHasPlayer(pQuest, pqParam->hPlayer->dwId))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 4);
		return;
	}
	if (dwDone == 1)
		return;
	if (!pQuest->bState)
		return;
	if (pQuest->bState >= 4 && !dwBit13)
		return;
	nTable = sgnA2Q1NpcTable[pQuest->bState];
	if (nTable == -1)
		return;
	if ((DWORD)nTable >= 9)
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, nTable);
}

void __fastcall A2Q1UnitKilled(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A2Q1RECORD *pRecord;
	int nLevel;
	int i;

	if (!pQuest->bActive)
		return;
#line 491
	QuestsDebugMessage(pQuest->ptGame, "killed radament for quest", D2_THIS_FILE, __LINE__);
#line 492
	QuestsSetState(pQuest, 4, D2_THIS_FILE, __LINE__);
	QuestsSetRecordBit(pqParam->ptGame, pQuest->eFilter, 13);
	pRecord = (A2Q1RECORD *)pQuest->pQuestRecord;
	pQuest->pCallback[2] = NULL;
	pRecord->bBossKilled = 1;
	pRecord->hBossRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(pqParam->hEventUnit);
	SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A2Q1KilledNear);
	SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A2Q1KilledFlag);
	pRecord->nDropCount = 0;
	SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A2Q1CountBooks);
	if (!pRecord->bTimerPending)
	{
		pRecord->bTimerPending = 1;
		QuestsAddTimer(pQuest, A2Q1Timer, 12);
	}
	if (!pRecord->nDropCount)
		return;
	pqParam->hEventUnit->dwDropItemCode = A2Q1_CODE_BOOK;
	for (i = 0; i < pRecord->nDropCount; i++)
		ITEMSDropFromUnit(pQuest->ptGame, pqParam->hEventUnit, 2, &nLevel, NULL, 44, FALSE);
}

BOOL __fastcall A2Q1KilledFlag(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 9, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 9, 1) == 1)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 9, 14);
	QuestsSendFlagInAct(hUnit, 8, 0);
	return FALSE;
}

BOOL __fastcall A2Q1KilledNear(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	GAMEROOM *hRoom;
	GAMEROOM **phNear;
	DWORD dwNear;
	DWORD i;
	BOOL fNear;
	QUEST *pQuestInfo;
	A2Q1RECORD *pQuestRecord;
	void *pRecord;
	WORD wParty;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	pQuestInfo = QuestsFind(ptGame, 8);
#line 389
	D2ASSERT(pQuestInfo != NULL);
	pQuestRecord = (A2Q1RECORD *)pQuestInfo->pQuestRecord;
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
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, pQuestInfo->eFilter, 0))
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, pQuestInfo->eFilter, 1))
		return FALSE;
	if (!fNear)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, pQuestInfo->eFilter, 13);
	D2CommonQuestRecordSetBit(pRecord, pQuestInfo->eFilter, 1);
	D2CommonQuestRecordSetBit(pRecord, 9, 5);
	wParty = SUnitGetParty(hUnit);
	if (wParty != 65535)
		PartyForEach(ptGame, wParty, A2Q1PartyCredit, NULL);
	return FALSE;
}

BYTE __fastcall A2Q1Timer(GAME *ptGame  , QUEST *pQuest
)
{
	A2Q1RECORD *pRecord;

	pRecord = (A2Q1RECORD *)pQuest->pQuestRecord;
	if (pQuest->bState == 4)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 3, NULL, A2Q1SendStatus, TRUE);
	}
	pRecord->bTimerPending = 0;
	return TRUE;
}

BOOL __fastcall A2Q1CountBooks(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	QUEST *pQuest;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 9, 5))
	{
		pQuest = QuestsFind(ptGame, 8);
		if (pQuest && !ITEMSFindByType(hUnit, A2Q1_CODE_BOOK))
			((A2Q1RECORD *)pQuest->pQuestRecord)->nDropCount++;
	}
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 9, 13))
		SUnitSetEvent(hUnit, 48, hUnit);
	return FALSE;
}

void __fastcall A2Q1ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	void *pRecord;
	UNIT *hPlayer;

	if (pqParam->field_14 != 40)
		return;
	QuestsPlayerLeft(pQuest, pqParam);
	if (pQuest->bState != 2)
		return;
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 1) == 1)
		return;
#line 547
	QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
	SUnitIterate(pQuest->ptGame, 0, NULL, A2Q1UpdatePlayer);
}

BYTE __fastcall A2Q1Sequence(QUEST *pQuestInfo  )
{
	QUEST *pNextQuest;

	if (pQuestInfo->bState != 5 && pQuestInfo->bActive)
		return TRUE;
	pNextQuest = QuestsFind(pQuestInfo->ptGame, (int)pQuestInfo->dwNextQuestId);
	if (pNextQuest)
	{
#line 575
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		return pNextQuest->pSequence(pNextQuest);
	}
	return FALSE;
}

void __fastcall A2Q1PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	void *pRecord;

	ptGame = pqParam->ptGame;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 9, 0) == 1)
	{
		if (D2CommonStepTestGridBit(pRecord, 9, 5) == 1
			&& !ITEMSFindByType(pqParam->hPlayer, A2Q1_CODE_BOOK))
		{
			D2CommonQuestRecordClearBit(pRecord, 9, 0);
			D2CommonQuestRecordClearBit(pRecord, 9, 1);
			D2CommonQuestRecordClearBit(pRecord, 9, 15);
			D2CommonQuestRecordClearBit(pRecord, 9, 5);
		}
		QuestsSetRecordBit(pQuest->ptGame, 9, 13);
		pQuest->bActive = 0;
		pQuest->bState = 0;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 9, 15) == 1)
	{
		if (D2CommonStepTestGridBit(pRecord, 9, 1) == 1)
			((A2Q1RECORD *)pQuest->pQuestRecord)->bStarterCredited = 1;
		pQuest->bState = 0;
		pQuest->bActive = 0;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 9, 4) == 1)
	{
		pQuest->bStatus = 2;
		pQuest->bState = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 9, 3) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 9, 2) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 2;
	}
}

void __fastcall A2Q1MonsterThink(GAME *ptGame  , UNIT *hUnit
)
{
	QUEST *pQuest;
	A2Q1RECORD *pRecord;
	BYTE fState;
	BYTE fFirst;

	pQuest = QuestsFind(ptGame, 8);
	if (!pQuest || pQuest->bActive != 1)
		return;
	if (pQuest->bState >= 3 && pQuest->bStatus >= 2)
		return;
	if (D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hUnit)) != 49)
		return;
	fState = FALSE;
	pQuest->pCallback[2] = NULL;
	if (pQuest->bState < 3)
	{
#line 703
		fState = TRUE;
		QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
	}
	if (pQuest->bStatus < 2)
	{
		pRecord = (A2Q1RECORD *)pQuest->pQuestRecord;
		pQuest->bStatusFlags = 0;
		fFirst = FALSE;
		if (!pRecord->bStatus2Sent)
		{
			pRecord->bStatus2Sent = 1;
			fFirst = TRUE;
		}
		QuestsSetStatus(pQuest, 2, NULL, A2Q1SendStatus, fFirst);
	}
	else if (!fState)
		return;
	SUnitIterate(pQuest->ptGame, 0, NULL, A2Q1UpdatePlayer);
}

BYTE __fastcall A2Q2Active(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer  , void *pPlayerRecord)
{
	BYTE bItem;

	if (nNpcClass == 244)
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 10, 1) == 1)
			return TRUE;
		if (A2Q2ItemState(pQuest, hPlayer, &bItem))
			return TRUE;
	}
	return FALSE;
}

BYTE __fastcall A2Q2ItemState(QUEST *pQuest  , UNIT *hUnit
, BYTE *pbItem)
{
	void *pRecord;
	GAME *ptGame;

	*pbItem = 255;
	ptGame = pQuest->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (ITEMSFindByType(hUnit, A2Q2_CODE_STAFF))
	{
		if (!D2CommonStepTestGridBit(pRecord, 10, 10))
		{
			*pbItem = 4;
			return TRUE;
		}
		*pbItem = 5;
		return FALSE;
	}
	if (ITEMSFindByType(hUnit, A2Q2_CODE_BOX))
	{
		if (!D2CommonStepTestGridBit(pRecord, 10, 6))
		{
			*pbItem = 3;
			return TRUE;
		}
		*pbItem = 9;
	}
	if (ITEMSFindByType(hUnit, A2Q2_CODE_SCROLL))
	{
		if (!D2CommonStepTestGridBit(pRecord, 10, 3))
		{
			*pbItem = 0;
			return TRUE;
		}
		*pbItem = 6;
	}
	if (ITEMSFindByType(hUnit, A2Q2_CODE_AMULET))
	{
		if (!D2CommonStepTestGridBit(pRecord, 10, 4))
		{
			*pbItem = 1;
			return TRUE;
		}
		*pbItem = 7;
	}
	if (ITEMSFindByType(hUnit, A2Q2_CODE_SHAFT))
	{
		if (!D2CommonStepTestGridBit(pRecord, 10, 5))
		{
			*pbItem = 2;
			return TRUE;
		}
		*pbItem = 8;
	}
	return FALSE;
}
