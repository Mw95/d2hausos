#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a4q1.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../MONSTER/MonsterUnique.h"
#include "QUESTS.h"
#include "a1q2.h"
#include "a4q1.h"
#include "a2q8.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"

#define A4Q1_NPC			367
#define A4Q1_MONSTER		406

static QUESTNPCTABLE sgtA4Q0Npcs[2] =
{
	{ { { 367, 664, { 0, 0 }, 1 } }, 1 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static QUESTNPCTABLE sgtA4Q1Npcs[6] =
{
	{ { { 367, 670, { 0, 0 }, 1 } }, 1 },
	{ { { 367, 671, { 0, 0 }, 2 }, { 246, 672, { 0, 0 }, 2 } }, 2 },
	{ { { 367, 673, { 0, 0 }, 2 }, { 246, 674, { 0, 0 }, 2 } }, 2 },
	{ { { 367, 676, { 0, 0 }, 1 }, { 246, 677, { 0, 0 }, 2 }, { 406, 675, { 0, 0 }, 1 } }, 3 },
	{ { { 367, 676, { 0, 0 }, 2 }, { 246, 677, { 0, 0 }, 2 } }, 2 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static int sgnA4Q1NpcTable[6] = { -1, 0, 1, 2, 3, 4 };

BOOL __fastcall A4Q1FlagPlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A4Q1SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A4Q1CreditPartyMember(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A4Q1Sequence(QUEST *pQuestThis);
void __fastcall A4Q1NpcMessage(QUEST *pQuestInfo, QUESTPARAM *pqParam);
void __fastcall A4Q1Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A4Q1UnitKilled(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A4Q1FlagSeen(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A4Q1FlagParty(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A4Q1Timer(GAME *ptGame, QUEST *pQuest);
BOOL __fastcall A4Q1CreditNear(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A4Q1ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A4Q1Activate(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A4Q1PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A4Q1NearPlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A4Q1UnitEvent(GAME *ptGame, UNIT *hUnit, void *pArg);

void __fastcall A4Q0Init(QUEST *pQuest  )
{
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->pCallback[0] = A4Q0Talk;
	pQuest->pCallback[11] = A4Q0NpcMessage;
	pQuest->ptNpcTables = sgtA4Q0Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->bState = 0;
	pQuest->bStatus = 0;
	pQuest->pQuestRecord = NULL;
	pQuest->eFilter = 24;
	pQuest->pStatusFilter = A2Q8StatusFilter;
	pQuest->pActiveFilter = A4Q0Active;
}

void __fastcall A4Q0NpcMessage(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	UNIT *hPlayer;
	GAME *ptGame;
	void *pRecord;
	if ((WORD)pqParam->field_14 != A4Q1_NPC)
		return;
	if ((WORD)pqParam->field_18 != 664)
		return;
	ptGame = pqParam->ptGame;
	hPlayer = pqParam->hPlayer;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	D2CommonQuestRecordSetBit(pRecord, 24, 0);
}

void __fastcall A4Q0Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecordX;
	GAME *ptGame;
	UNIT *hPlayer;
	if (pqParam->hEventUnit->dwClassId != A4Q1_NPC)
		return;
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecordX, 24, 0))
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, (int)pqParam->hEventUnit->dwClassId, 0);
}

BYTE __fastcall A4Q0Active(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer, void *pPlayerRecord)
{
	if (nNpcClass == A4Q1_NPC && !D2CommonStepTestGridBit(pPlayerRecord, 24, 0))
		return TRUE;
	return FALSE;
}

BYTE __fastcall A4Q1Active(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer, void *pPlayerRecord)
{
	if (D2CommonStepTestGridBit(pPlayerRecord, 25, 0))
		return FALSE;
	if (nNpcClass == A4Q1_NPC)
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 25, 1) == 1)
			return TRUE;
		if (pQuest->bState == 1)
			return TRUE;

	}
	if (nNpcClass == A4Q1_MONSTER && !D2CommonStepTestGridBit(pPlayerRecord, 25, 5))
		return TRUE;
	return FALSE;
}

void __fastcall A4Q1CreditPartyMember(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	DWORD dwLevelId;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 25, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, 25, 1))
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	dwLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
	if (!dwLevelId)
		return;
	if (D2CommonGetActNum(dwLevelId) != 3)
		return;
	D2CommonQuestRecordSetBit(pRecord, 25, 13);
	D2CommonQuestRecordSetBit(pRecord, 25, 1);
	D2CommonQuestRecordClearMidBits(pRecord, 25);
}

void __fastcall A4Q1Init(QUEST *pQuest  )
{
	A4Q1RECORD *pQuestRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->ptNpcTables = sgtA4Q1Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->bState = 1;
	pQuest->pCallback[10] = A1Q2PlayerLeft;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A4Q1ChangedLevel;
	pQuest->pCallback[8] = A4Q1UnitKilled;
	pQuest->pCallback[0] = A4Q1Talk;
	pQuest->pCallback[2] = A4Q1Activate;
	pQuest->pCallback[11] = A4Q1NpcMessage;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A4Q1PlayerStarted;
	pQuest->eFilter = 25;
	pQuest->bDoneStateMin = 4;
	pQuest->pStatusFilter = NULL;
	pQuest->pActiveFilter = A4Q1Active;
#line 662
	pQuestRecord = (A4Q1RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A4Q1RECORD));
	pQuest->pQuestRecord = pQuestRecord;
	pQuest->pSequence = A4Q1Sequence;
	pQuest->dwNextQuestId = 24;
	for (i = 0; i < (int)(sizeof(A4Q1RECORD) / sizeof(DWORD)); i++)
		((DWORD *)pQuestRecord)[i] = 0;
}

BYTE __fastcall A4Q1Sequence(QUEST *pQuestThis  )
{
	QUEST *pQuest;

	if (pQuestThis->bState != 5 && pQuestThis->bActive)
		return TRUE;
	pQuest = QuestsFind(pQuestThis->ptGame, (int)pQuestThis->dwNextQuestId);
	if (pQuest)
	{
#line 161
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuest->pSequence), "pQuest->pSequence");
		return pQuest->pSequence(pQuest);
	}
	return FALSE;
}

void __fastcall A4Q1NpcMessage(QUEST *pQuestInfo  , QUESTPARAM *pqParam
)
{
	A4Q1RECORD *pQuestRecord;
	void *pRecord;
	WORD wNpcClass;
	WORD wMessage;
	GAME *ptGame;
	UNIT *hPlayer;

	wNpcClass = (WORD)pqParam->field_14;
	wMessage = (WORD)pqParam->field_18;
	pQuestRecord = (A4Q1RECORD *)pQuestInfo->pQuestRecord;
	hPlayer = pqParam->hPlayer;
	ptGame = pQuestInfo->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (wNpcClass == A4Q1_NPC)
	{
		if (wMessage == 670)
		{
			pQuestRecord->bTyraelMessageDone = 1;
#line 211
			QuestsSetState(pQuestInfo, 2, D2_THIS_FILE, __LINE__);
			SUnitIterate(pQuestInfo->ptGame, 0, NULL, A4Q1FlagPlayer);
			QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
			return;
		}
		if (wMessage != 676)
			return;
		if (!D2CommonStepTestGridBit(pRecord, pQuestInfo->eFilter, 1))
			return;
		if (D2CommonStepTestGridBit(pRecord, pQuestInfo->eFilter, 13))
		{
			if (pQuestInfo->bState != 5)
			{
#line 222
				QuestsSetState(pQuestInfo, 5, D2_THIS_FILE, __LINE__);
#line 223
				D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
				pQuestInfo->pSequence(pQuestInfo);
				pQuestInfo->bStatusFlags = 0;
				QuestsSetStatus(pQuestInfo, 13, NULL, A4Q1SendStatus, FALSE);
			}
			pQuestInfo->pCallback[2] = NULL;
			QuestsSetRecordBit(pqParam->ptGame, pQuestInfo->eFilter, 13);
		}
		D2CommonQuestRecordSetBit(pRecord, pQuestInfo->eFilter, 0);
		D2CommonQuestRecordClearBit(pRecord, pQuestInfo->eFilter, 1);
		D2CommonQuestRecordClearMidBits(pRecord, pQuestInfo->eFilter);
		D2CommonStatsUnitAddValue(pqParam->hPlayer, 5, 2);
		QuestsSendFlag(pqParam->hPlayer, 22);
		QuestsPlayersAdd(&pQuestInfo->tPlayers, pqParam->hPlayer->dwId);
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		return;
	}
	if (wNpcClass != A4Q1_MONSTER)
		return;
	if (wMessage != 675)
		return;
	pQuestRecord->bIzualMessageDone = 1;
	pQuestRecord->field_0f = 1;
	D2CommonQuestRecordSetBit(pRecord, pQuestInfo->eFilter, 5);
	if (pQuestInfo->bStatus == 4)
		return;
	pQuestInfo->bStatusFlags = 0;
	QuestsSetStatus(pQuestInfo, 4, NULL, A4Q1SendStatus, TRUE);
}

BOOL __fastcall A4Q1FlagPlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuest;
	void *pRecord;
	A4Q1RECORD *pQRec;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 25, 0) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 22);
	if (!pQuest)
		return FALSE;
	pQRec = (A4Q1RECORD *)pQuest->pQuestRecord;
	if (pQuest->bState == 2)
	{
		D2CommonQuestRecordSetBit(pRecord, 25, 2);
		return FALSE;
	}
	if (pQuest->bState != 3)
		return FALSE;
	if (pQRec->field_0c)
	{
		D2CommonQuestRecordSetBit(pRecord, 25, 4);
		return FALSE;
	}
	D2CommonQuestRecordSetBit(pRecord, 25, 3);
	return FALSE;
}

BOOL __fastcall A4Q1SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 25, 0) || D2CommonStepTestGridBit(pRecord, 25, 15))
		&& !D2CommonStepTestGridBit(pRecord, 25, 13)
		&& !D2CommonStepTestGridBit(pRecord, 25, 14))
		return FALSE;
	QuestsSendStatus(ptGame, hUnit, 22);
	return FALSE;
}

void __fastcall A4Q1Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	DWORD dwBit0;
	DWORD dwBit13;
	int nNpcClass;
	int nTable;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	dwBit0 = D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0);
	dwBit13 = D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 13);
	nNpcClass = (int)pqParam->hEventUnit->dwClassId;
	if (nNpcClass == A4Q1_MONSTER && !D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 5))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 3);
		return;
	}
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
	if (dwBit0 == 1)
		return;
	if (pQuest->bState >= 4 && !dwBit13)
		return;
	if (!pQuest->bActive)
		return;
	nTable = sgnA4Q1NpcTable[pQuest->bState];
	if (nTable == -1)
		return;
	if ((DWORD)nTable >= 6)
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, nTable);
}

void __fastcall A4Q1UnitKilled(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A4Q1RECORD *pQuestRecord;
	void *pRecord;
	GAME *ptGame;
	UNIT *hPlayer;

	pQuestRecord = (A4Q1RECORD *)pQuest->pQuestRecord;
#line 501
	QuestsDebugMessage(pQuest->ptGame, "Finished quest", D2_THIS_FILE, __LINE__);
	pQuest->pCallback[2] = NULL;
#line 503
	QuestsSetState(pQuest, 4, D2_THIS_FILE, __LINE__);
	pQuest->pCallback[8] = NULL;
	pQuestRecord->hMonster = pqParam->hEventUnit;
	if (pQuest->bActive)
	{
		if (pqParam->hPlayer)
		{
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
			pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
			if (!D2CommonStepTestGridBit(pRecord, 25, 0)
				&& !D2CommonStepTestGridBit(pRecord, 25, 1))
			{
				D2CommonQuestRecordSetBit(pRecord, 25, 13);
				D2CommonQuestRecordSetBit(pRecord, 25, 1);
				D2CommonQuestRecordClearMidBits(pRecord, pQuest->eFilter);
			}
		}
		pQuestRecord->bSpawnPending = 1;
		D2CommonUnitGetPosition(pqParam->hEventUnit, &pQuestRecord->tDeathSpot);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A4Q1CreditNear);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A4Q1FlagParty);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A4Q1FlagSeen);
	}
	if (pQuestRecord->bTimerPending)
		return;
	pQuestRecord->bTimerPending = 1;
	QuestsAddTimer(pQuest, A4Q1Timer, 3);
}

BOOL __fastcall A4Q1FlagSeen(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 25, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 25, 1) == 1)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 25, 14);
	QuestsSendFlagInAct(hUnit, 22, 0);
	return FALSE;
}

BOOL __fastcall A4Q1FlagParty(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	WORD wParty;

	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecordX, 25, 13))
		return FALSE;
	wParty = SUnitGetParty(hUnit);
	if (wParty != 65535)
		PartyForEach(ptGame, wParty, A4Q1CreditPartyMember, NULL);
	return FALSE;
}

BYTE __fastcall A4Q1Timer(GAME *ptGame  , QUEST *pQuest
)
{
	A4Q1RECORD *pQuestRecord;
	GAMEROOM *hRoom;

	pQuestRecord = (A4Q1RECORD *)pQuest->pQuestRecord;
	if (pQuest->bState == 4 && pQuest->bStatus != 4 && pQuest->bStatus != 13)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 3, NULL, A4Q1SendStatus, TRUE);
	}
	if (pQuestRecord->bSpawnPending)
	{
		pQuestRecord->bSpawnPending = 0;
		hRoom = (GAMEROOM *)D2CommonDungeonActFindRoomByPoint(ptGame->ptAct[3], pQuestRecord->tDeathSpot.dwX,
			pQuestRecord->tDeathSpot.dwY);
		if (hRoom)
			MonsterSpawn(ptGame, hRoom, (int)pQuestRecord->tDeathSpot.dwX, (int)pQuestRecord->tDeathSpot.dwY,
				A4Q1_MONSTER, 8, -1, 0);
	}
	pQuestRecord->bTimerPending = 0;
	return TRUE;
}

BOOL __fastcall A4Q1CreditNear(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuestInfo;
	GAMEROOM *hRoom;
	GAMEROOM *hMonsterRoom;
	GAMEROOM **phNear;
	DWORD dwNear;
	DWORD i;
	BOOL fNear;
	void *pRecord;
	A4Q1RECORD *pRec;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	pQuestInfo = QuestsFind(ptGame, 22);
#line 444
	D2ASSERT(pQuestInfo != NULL);
	pRec = (A4Q1RECORD *)pQuestInfo->pQuestRecord;
	hMonsterRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(pRec->hMonster);
	if (hRoom)
	{
		fNear = FALSE;
		if (hRoom == hMonsterRoom)
			fNear = TRUE;
		else
		{
			D2CommonDungeonRoomGetNear(D2CommonUnitGetPathRoom(hUnit), &phNear, &dwNear);
			for (i = 0; i < dwNear; i++)
			{
				if (phNear[i] == hMonsterRoom)
				{
					fNear = TRUE;
					break;
				}
			}
		}
		pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
		if (!D2CommonStepTestGridBit(pRecord, 25, 0) && !D2CommonStepTestGridBit(pRecord, 25, 1) && fNear)
		{
			D2CommonQuestRecordSetBit(pRecord, 25, 13);
			D2CommonQuestRecordSetBit(pRecord, 25, 1);
		}
	}
	return FALSE;
}

void __fastcall A4Q1ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecordX;
	GAME *ptGame;
	UNIT *hPlayer;
	if (pqParam->field_14 != 103)
		return;
	QuestsPlayerLeft(pQuest, pqParam);
	if (pQuest->bState != 2)
		return;
	ptGame = pqParam->ptGame;
	hPlayer = pqParam->hPlayer;
	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecordX, pQuest->eFilter, 0) == 1)
		return;
#line 561
	QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
	SUnitIterate(pQuest->ptGame, 0, NULL, A4Q1FlagPlayer);
	if (pQuest->bStatus)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 1, NULL, A4Q1SendStatus, FALSE);
}

void __fastcall A4Q1Activate(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A4Q1RECORD *pQuestRecord;

	if ((WORD)pqParam->hEventUnit->dwClassId != A4Q1_NPC)
		return;
	pQuestRecord = (A4Q1RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bTyraelMessageDone != 1)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 1, NULL, A4Q1SendStatus, TRUE);
	pQuestRecord->bTyraelMessageDone = 0;
	pQuest->pCallback[2] = NULL;
}

void __fastcall A4Q1PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
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
	if (D2CommonStepTestGridBit(pRecord, 25, 4) == 1)
	{
		((A4Q1RECORD *)pQuest->pQuestRecord)->field_0c = 1;
		pQuest->bStatus = 2;
		pQuest->bState = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 25, 3) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 25, 2) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 2;
	}
}

void __fastcall A4Q1MonsterSeen(GAME *ptGame  )
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptGame, 22);
	if (!pQuest)
		return;
	if (!pQuest->bActive)
		return;
	if (pQuest->bStatus >= 2)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 2, NULL, A4Q1SendStatus, TRUE);
}

BOOL __fastcall A4Q1NoPlayerNear(GAME *ptGame  , UNIT *hUnit
)
{
	QUEST *pQuest;
	A4Q1RECORD *pQuestRecord;

	pQuest = QuestsFind(ptGame, 22);
	if (pQuest) {
		pQuestRecord = (A4Q1RECORD *)pQuest->pQuestRecord;
		if (!pQuestRecord->bIzualMessageDone)
			return FALSE;
		pQuestRecord->bPlayerNear = 0;
		pQuestRecord->hMonster = hUnit;
		SUnitForEachPlayer(ptGame, A4Q1NearPlayer, pQuestRecord);
		return pQuestRecord->bPlayerNear == 0; }
	return FALSE;
}

void __fastcall A4Q1NearPlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	if (D2CommonUnitGetProximityMetric(hUnit, ((A4Q1RECORD *)pArg)->hMonster) < 5)
		((A4Q1RECORD *)pArg)->bPlayerNear = 1;
}

UNIT * __fastcall A4Q1UnitEvents(GAME *ptGame  )
{
	return SUnitIterate(ptGame, 0, NULL, A4Q1UnitEvent);
}

BOOL __fastcall A4Q1UnitEvent(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecordX, 25, 13))
		SUnitSetEvent(hUnit, 72, hUnit);
	return FALSE;
}
