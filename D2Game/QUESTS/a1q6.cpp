#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a1q6.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "../ITEMS/Items.h"
#include "QUESTS.h"
#include "a1q5.h"
#include "a1q6.h"
#include "a2q0.h"
#include "a2q2.h"
#include "a2q8.h"
#include "a3q0.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"
#include "../SKILLS/Skills.h"

static int sgnA1Q6NpcTable[7] = { -1, 0, 1, 2, 3, 4, 0 };

static QUESTNPCTABLE sgtA1Q6Npcs[6] =
{
	{ { { 265, 166, { 0, 0 }, 1 } }, 1 },
	{ { { 148, 168, { 0, 0 }, 2 }, { 150, 172, { 0, 0 }, 2 }, { 154, 169, { 0, 0 }, 2 },
		{ 265, 167, { 0, 0 }, 2 }, { 147, 170, { 0, 0 }, 2 }, { 155, 171, { 0, 0 }, 2 } }, 6 },
	{ { { 150, 178, { 0, 0 }, 2 }, { 155, 177, { 0, 0 }, 2 }, { 147, 175, { 0, 0 }, 2 },
		{ 265, 173, { 0, 0 }, 2 }, { 154, 176, { 0, 0 }, 2 }, { 148, 174, { 0, 0 }, 2 } }, 6 },
	{ { { 150, 181, { 0, 0 }, 0 }, { 265, 184, { 0, 0 }, 0 }, { 154, 180, { 0, 0 }, 2 },
		{ 147, 182, { 0, 0 }, 2 }, { 155, 183, { 0, 0 }, 0 }, { 148, 179, { 0, 0 }, 0 } }, 6 },
	{ { { 150, 181, { 0, 0 }, 2 }, { 265, 184, { 0, 0 }, 2 }, { 154, 180, { 0, 0 }, 2 },
		{ 147, 182, { 0, 0 }, 2 }, { 155, 183, { 0, 0 }, 2 }, { 148, 179, { 0, 0 }, 2 } }, 6 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static DWORD sgadwA1Q6Drops1[7] =
{
	544629607, 544367463, 543318887, 544826215, 543646567, 544695143, 543386483,
};
static DWORD sgadwA1Q6Drops2[7] =
{
	544633703, 544371559, 543322983, 544830311, 543650663, 544699239, 544566131,
};

static QUESTNPCTABLE sgtA1Q6Npc10aNpcs[6] =
{
	{ { { 266, 59, { 0, 0 }, 1 } }, 1 },
	{ { { 266, 60, { 0, 0 }, 1 } }, 1 },
	{ { { 266, 61, { 0, 0 }, 1 } }, 1 },
	{ { { 266, 62, { 0, 0 }, 1 } }, 1 },
	{ { { 266, 63, { 0, 0 }, 1 } }, 1 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

BYTE __fastcall A1Q6Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall A1Q6NpcActivated(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A1Q6SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A1Q6NpcMessage(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A1Q6UpdatePlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A1Q6Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q6ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q6UnitKilled(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A1Q6SetEvent(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A1Q6PlayerLeft(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A1Q6CreditInLevel(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A1Q6FlagPlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A1Q6CreditParty(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A1Q6Timer(GAME *ptGame, QUEST *pQuest);
BOOL __fastcall A1Q6OpenPortal(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A1Q6Sequence(QUEST *pQuest);
BYTE __fastcall A1Q6StartTimer(GAME *ptGame, QUEST *pQuest);
void __fastcall A1Q6PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q6NaviTalk(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A1Q6NaviActive(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);

BYTE __fastcall A1Q6Active(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer, void *pPlayerRecord)
{
	DWORD dwUnitId;
	A1Q6RECORD *pQuestRecord;

	dwUnitId = hPlayer->dwId;
	pQuestRecord = (A1Q6RECORD *)pQuest->pQuestRecord;
	if (nNpcClass == 265)
	{
		if (QuestsPlayersFind(&pQuestRecord->tPlayers1, dwUnitId))
			return TRUE;
		if (D2CommonStepTestGridBit(pPlayerRecord, 6, 0) == 1)
			return FALSE;
		if (pQuest->bState != 1)
			return FALSE;
		if (!D2CommonStepTestGridBit(pPlayerRecord, 6, 1))
			return TRUE;
	}
	if (nNpcClass == 155)
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 6, 0) == 1)
			return FALSE;
		if (D2CommonStepTestGridBit(pPlayerRecord, 6, 1) == 1) return TRUE; return FALSE;
	}
	if (nNpcClass == 148)
		{ if (QuestsPlayersFind(&pQuestRecord->tPlayers2, dwUnitId) != 0) return TRUE; return FALSE; }
	if (nNpcClass == 150)
		{ if (QuestsPlayersFind(&pQuestRecord->tPlayers3, dwUnitId) != 0) return TRUE; return FALSE; }
	return FALSE;
}

void __fastcall A1Q6Init(QUEST *pQuest  )
{
	A1Q6RECORD *pQuestRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A1Q6ChangedLevel;
	pQuest->pCallback[8] = A1Q6UnitKilled;
	pQuest->pCallback[0] = A1Q6Talk;
	pQuest->pCallback[11] = A1Q6NpcMessage;
	pQuest->pCallback[2] = A1Q6NpcActivated;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A1Q6PlayerStarted;
	pQuest->pSequence = A1Q6Sequence;
	pQuest->eFilter = 6;
	pQuest->ptNpcTables = sgtA1Q6Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->bStatus = 0;
	pQuest->bState = 0;
#line 837
	pQuestRecord = (A1Q6RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A1Q6RECORD));
	pQuest->bDoneStateMin = 4;
	pQuest->pQuestRecord = pQuestRecord;
	pQuest->pStatusFilter = NULL;
	pQuest->dwNextQuestId = 30;
	pQuest->pActiveFilter = A1Q6Active;
	pQuestRecord->dwBossId = 0;
	pQuestRecord->field_190 = 0;
	pQuestRecord->wTimerRuns = 0;
	pQuestRecord->bBossKilled = 0;
	pQuestRecord->bStatus1Pending = 0;
	QuestsPlayersClear(&pQuestRecord->tPlayers1);
	QuestsPlayersClear(&pQuestRecord->tPlayers2);
	QuestsPlayersClear(&pQuestRecord->tPlayers3);
}

void __fastcall A1Q6NpcActivated(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q6RECORD *pQuestRecord;

	if ((WORD)pqParam->hEventUnit->dwClassId != 265)
		return;
	pQuestRecord = (A1Q6RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bStatus1Pending != 1)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 1, NULL, A1Q6SendStatus, TRUE);
	pQuestRecord->bStatus1Pending = 0;
	pQuest->pCallback[2] = NULL;
}

BOOL __fastcall A1Q6SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg  )
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 6, 0) || D2CommonStepTestGridBit(pRecord, 6, 15))
	{
		if (!D2CommonStepTestGridBit(pRecord, 6, 13) && !D2CommonStepTestGridBit(pRecord, 6, 14))
			return FALSE;
	}
	QuestsSendStatus(ptGame, hUnit, 6);
	return FALSE;
}

void __fastcall A1Q6NpcMessage(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	WORD wNpcClass;
	WORD wMessage;
	A1Q6RECORD *pQuestRecord;
	DWORD dwUnitId;
	void *pRecord;
	GAME *ptGame;
	UNIT *hPlayer;

	wNpcClass = (WORD)pqParam->field_14;
	wMessage = (WORD)pqParam->field_18;
	pQuestRecord = (A1Q6RECORD *)pQuest->pQuestRecord;
	dwUnitId = pqParam->hPlayer->dwId;
	if (wNpcClass == 265)
	{
		if (wMessage == 166)
		{
#line 192
			QuestsSetState(pQuest, 2, D2_THIS_FILE, __LINE__);
			pQuestRecord->bStatus1Pending = 1;
			SUnitIterate(pQuest->ptGame, 0, NULL, A1Q6UpdatePlayer);
			QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		}
		else if (wMessage == 184)
			QuestsPlayersRemove(&pQuestRecord->tPlayers1, dwUnitId);
		return;
	}
	if (wNpcClass == 155)
	{
		if (wMessage != 183)
			return;
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
		pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
		if (!D2CommonStepTestGridBit(pRecord, 6, 1))
			return;
		if (D2CommonStepTestGridBit(pRecord, 6, 13))
		{
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 13, NULL, A1Q6SendStatus, FALSE);
#line 214
			QuestsSetState(pQuest, 5, D2_THIS_FILE, __LINE__);
			QuestsSetRecordBit(pQuest->ptGame, 6, 13);
		}
		D2CommonQuestRecordClearBit(pRecord, 6, 1);
		D2CommonQuestRecordSetBit(pRecord, 6, 0);
		QuestsPlayersAdd(&pQuest->tPlayers, pqParam->hPlayer->dwId);
		QuestsSendPlayerRecord(pQuest->ptGame, pqParam->hPlayer);
		return;
	}
	if (wNpcClass == 148)
	{
		if (wMessage == 179)
			QuestsPlayersRemove(&pQuestRecord->tPlayers2, dwUnitId);
		return;
	}
	if (wNpcClass == 150 && wMessage == 181)
		QuestsPlayersRemove(&pQuestRecord->tPlayers3, dwUnitId);
}

BOOL __fastcall A1Q6UpdatePlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg  )
{
	void *pRecord;
	QUEST *pQuest;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 6, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 6, 1) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 6);
	if (!pQuest)
		return FALSE;
	if (pQuest->bState == 2)
	{
		D2CommonQuestRecordSetBit(pRecord, 6, 2);
		return FALSE;
	}
	if (pQuest->bState == 3)
	{
		if (pQuest->bStatus == 1)
		{
			D2CommonQuestRecordSetBit(pRecord, 6, 3);
			return FALSE;
		}
		D2CommonQuestRecordSetBit(pRecord, 6, 4);
	}
	return FALSE;
}

void __fastcall A1Q6Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	DWORD fFinished;
	DWORD fBit13;
	DWORD fStarted;
	int nNpcClass;
	DWORD dwUnitId;
	A1Q6RECORD *pQuestRecord;
	BYTE bState;
	DWORD dwTable;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	fFinished = D2CommonStepTestGridBit(pRecord, 6, 0);
	fBit13 = D2CommonStepTestGridBit(pRecord, 6, 13);
	fStarted = D2CommonStepTestGridBit(pRecord, 6, 1);
	pQuestRecord = (A1Q6RECORD *)pQuest->pQuestRecord;
	nNpcClass = pqParam->hEventUnit->dwClassId;
	dwUnitId = pqParam->hPlayer->dwId;
	switch (nNpcClass)
	{
	case 148:
		if (QuestsPlayersFind(&pQuestRecord->tPlayers2, dwUnitId))
		{
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, 148, 3);
			return;
		}
		break;
	case 150:
		if (QuestsPlayersFind(&pQuestRecord->tPlayers3, dwUnitId))
		{
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, 150, 3);
			return;
		}
		break;
	case 265:
		if (QuestsPlayersFind(&pQuestRecord->tPlayers1, dwUnitId))
		{
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, 265, 3);
			return;
		}
		break;
	}
	if (fStarted)
	{
		if (nNpcClass != 265 && nNpcClass != 148 && nNpcClass != 150)
		{
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 3);
			return;
		}
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 4);
		return;
	}
	if (QuestsHasPlayer(pQuest, pqParam->hPlayer->dwId) == 1)
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 4);
		return;
	}
	bState = pQuest->bState;
	if (pQuest->bState == 1 && nNpcClass == 265 && !fFinished)
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 0);
		return;
	}
	if (!pQuest->bState)
		return;
	if (fFinished == 1 && !fBit13)
		return;
	if (pQuest->bState >= 4 && !fBit13)
		return;
	if (fFinished == 1 && fBit13 == 1)
		return;
	dwTable = (DWORD)sgnA1Q6NpcTable[pQuest->bState];
	if (dwTable == (DWORD)-1)
		return;
	if (dwTable >= 6)
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, (int)dwTable);
}

void __fastcall A1Q6ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	int nLevelId;
	DWORD fLeft;
	BYTE fChanged;
	void *pRecord;
	GAME *ptGame;
	UNIT *hPlayer;

	nLevelId = (int)pqParam->field_18;
	fLeft = pqParam->field_14;
	if (nLevelId >= 34 && nLevelId <= 37 && pQuest->bActive)
	{
		fChanged = FALSE;
		if (pQuest->bState < 3)
		{
#line 358
			QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
			fChanged = TRUE;
		}
		if (nLevelId == 37)
		{
			if (pQuest->bStatus < 2)
			{
				pQuest->bStatusFlags = 0;
				QuestsSetStatus(pQuest, 2, NULL, A1Q6SendStatus, TRUE);
				fChanged = TRUE;
			}
		}
		else if (pQuest->bStatus != 1)
		{
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 1, NULL, A1Q6SendStatus, FALSE);
			fChanged = TRUE;
		}
		if (fChanged)
			SUnitIterate(pQuest->ptGame, 0, NULL, A1Q6UpdatePlayer);
#line 384
		QuestsDebugMessage(pqParam->ptGame, "started quest by jumping to level", D2_THIS_FILE, __LINE__);
		return;
	}
	if (pQuest->bState == 4 && nLevelId == 40)
	{
#line 391
		QuestsSetState(pQuest, 5, D2_THIS_FILE, __LINE__);
		return;
	}
	if (fLeft != 1)
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
#line 405
	QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
	SUnitIterate(pQuest->ptGame, 0, NULL, A1Q6UpdatePlayer);
}

void __fastcall A1Q6UnitKilled(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q6RECORD *pQuestRecord;
	int nLevel;
	DWORD dwDraw;
	int i;
	GAME *ptGame;
	void *pRecord;

	pQuestRecord = (A1Q6RECORD *)pQuest->pQuestRecord;
	pQuest->pCallback[2] = NULL;
	if (pQuest->bActive)
	{
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A1Q6CreditInLevel);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A1Q6CreditParty);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A1Q6FlagPlayer);
		if (pqParam->hPlayer)
		{
			ptGame = pQuest->ptGame;
			pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
			if (!D2CommonStepTestGridBit(pRecord, 6, 0))
			{
				for (i = 2; i; i--)
				{
					dwDraw = D2CommonSeedRand(QuestsGetSeed(pQuest->ptGame), 7);
					pqParam->hEventUnit->dwDropItemCode = sgadwA1Q6Drops1[dwDraw];
					ITEMSDropFromUnit(pQuest->ptGame, pqParam->hEventUnit, 2, &nLevel, NULL, 44, FALSE);
				}
				dwDraw = D2CommonSeedRand(QuestsGetSeed(pQuest->ptGame), 7);
				pqParam->hEventUnit->dwDropItemCode = sgadwA1Q6Drops2[dwDraw];
				ITEMSDropFromUnit(pQuest->ptGame, pqParam->hEventUnit, 2, &nLevel, NULL, 44, FALSE);
				pqParam->hEventUnit->dwDropItemCode = 0;
			}
		}
	}
	pQuestRecord->dwBossId = pqParam->hEventUnit->dwId;
	pQuestRecord->wTimerRuns = 1;
	if (pQuest->bActive)
	{
		QuestsAddTimer(pQuest, A1Q6Timer, 1);
		SUnitIterate(pQuest->ptGame, 0, NULL, A1Q6SetEvent);
	}
	pQuestRecord->bBossKilled = 1;
#line 688
	QuestsSetState(pQuest, 4, D2_THIS_FILE, __LINE__);
	pQuest->pCallback[10] = A1Q6PlayerLeft;
	pQuest->pCallback[8] = NULL;
}

BOOL __fastcall A1Q6SetEvent(GAME *ptGame  , UNIT *hUnit
, void *pArg  )
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 6, 1) && D2CommonStepTestGridBit(pRecord, 6, 13))
		SUnitSetEvent(hUnit, 31, hUnit);
	return FALSE;
}

void __fastcall A1Q6PlayerLeft(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q6RECORD *pQuestRecord;
	DWORD dwUnitId;

	QuestsPlayerDone(pQuest, pqParam);
	pQuestRecord = (A1Q6RECORD *)pQuest->pQuestRecord;
	dwUnitId = pqParam->hPlayer->dwId;
	QuestsPlayersRemove(&pQuestRecord->tPlayers1, dwUnitId);
	QuestsPlayersRemove(&pQuestRecord->tPlayers2, dwUnitId);
	QuestsPlayersRemove(&pQuestRecord->tPlayers3, dwUnitId);
}

BOOL __fastcall A1Q6CreditInLevel(GAME *ptGame  , UNIT *hUnit
, void *pArg  )
{
	void *pRecord;
	GAMEROOM *hRoom;
	A1Q6RECORD *pQuestRecord;
	DWORD dwUnitId;
	QUEST *pQuest;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (D2CommonStepTestGridBit(pRecord, 6, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 6, 15) == 1)
		return FALSE;
	if (!hRoom)
		return FALSE;
	if (D2CommonDungeonRoomGetLevelId(hRoom) != 37)
		return FALSE;
	pQuest = QuestsFind(ptGame, 6);
	pQuestRecord = (A1Q6RECORD *)pQuest->pQuestRecord;
	dwUnitId = hUnit->dwId;
	QuestsPlayersAdd(&pQuestRecord->tPlayers1, dwUnitId);
	QuestsPlayersAdd(&pQuestRecord->tPlayers2, dwUnitId);
	QuestsPlayersAdd(&pQuestRecord->tPlayers3, dwUnitId);
	D2CommonQuestRecordSetBit(pRecord, 6, 13);
	D2CommonQuestRecordSetBit(pRecord, 6, 1);
	ClientSetStep(SUnitGetClient(hUnit), 1, ptGame->bDifficultyLevel);
	return FALSE;
}

BOOL __fastcall A1Q6FlagPlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg  )
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 6, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 6, 1) == 1)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 6, 14);
	QuestsSendFlagInAct(hUnit, 6, 0);
	return FALSE;
}

BOOL __fastcall A1Q6CreditParty(GAME *ptGame  , UNIT *hUnit
, void *pArg  )
{
	void *pRecordX;
	WORD wParty;

	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecordX, 6, 13))
		return FALSE;
	wParty = SUnitGetParty(hUnit);
	if (wParty != 65535)
		PartyForEach(ptGame, wParty, A1Q6PartyJoined, NULL);
	return FALSE;
}

BYTE __fastcall A1Q6Timer(GAME *ptGame  , QUEST *pQuest
)
{
	A1Q6RECORD *pQuestRecord;

	pQuestRecord = (A1Q6RECORD *)pQuest->pQuestRecord;
	pQuestRecord->wTimerRuns++;
	if (pQuestRecord->wTimerRuns == 10)
	{
		SUnitIterate(ptGame, 0, SUnitFind(ptGame, 1, pQuestRecord->dwBossId), A1Q6OpenPortal);
		return FALSE;
	}
	if (!(pQuestRecord->wTimerRuns != 12))
	{
		if (pQuest->bStatus != 3 && pQuest->bStatus != 13)
		{
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 3, NULL, A1Q6SendStatus, TRUE);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL __fastcall A1Q6OpenPortal(GAME *ptGame  , UNIT *hUnit
, void *pArg  )
{
	GAMEROOM *hRoom;
	GAMECOORD tCoord;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (hRoom)
	{
		if (D2CommonDungeonRoomGetLevelId(hRoom) == 37)
		{
			D2CommonUnitGetPosition(hUnit, &tCoord);
			SkillsOpenPortal(ptGame, hUnit, hRoom, (int)tCoord.dwX, (int)tCoord.dwY, LEVEL_ACT_1_TOWN, NULL, 59,
				FALSE);
			return TRUE;
		}
	}
	return FALSE;
}

BYTE __fastcall A1Q6Sequence(QUEST *pQuest  )
{
	if (!pQuest->bState && pQuest->bActive == 1)
		QuestsAddTimer(pQuest, A1Q6StartTimer, 20);
	return TRUE;
}

BYTE __fastcall A1Q6StartTimer(GAME *ptGame  , QUEST *pQuest
)
{
	if (!pQuest->bState)
	{
#line 605
		QuestsSetState(pQuest, 1, D2_THIS_FILE, __LINE__);
	}
	return TRUE;
}

void __fastcall A1Q6PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
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
	if (D2CommonStepTestGridBit(pRecord, 6, 4) == 1)
	{
		pQuest->bStatus = 2;
		pQuest->bState = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 6, 3) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 6, 2) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 2;
	}
}

void __fastcall A1Q6NaviInit(QUEST *pQuest  )
{
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->pCallback[0] = A1Q6NaviTalk;
	pQuest->pCallback[8] = A2Q2UnitKilled;
	pQuest->ptNpcTables = sgtA1Q6Npc10aNpcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->bState = 0;
	pQuest->bStatus = 0;
	pQuest->eFilter = 29;
	pQuest->pStatusFilter = A2Q8StatusFilter;
	pQuest->pActiveFilter = A1Q6NaviActive;
}

void __fastcall A1Q6NaviTalk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	QUEST *pQuest1;
	void *pRecord;
	UNIT *hPlayer;

	DWORD dwClass = pqParam->hEventUnit->dwClassId;

	if (dwClass != 266)
		return;
	pQuest1 = QuestsFind(pQuest->ptGame, 1);
	if (!QuestsGetRecordBit(pqParam->ptGame, 1, 13) && pQuest1->bActive)
	{
		GAME *ptGame = pqParam->ptGame;

		hPlayer = pqParam->hPlayer;
		pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))
			->apQuestRecords[ptGame->bDifficultyLevel];
		if (!D2CommonStepTestGridBit(pRecord, 1, 0) && !D2CommonStepTestGridBit(pRecord, 1, 13)
			&& !D2CommonStepTestGridBit(pRecord, 1, 1))
		{
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, dwClass,
				(int)D2CommonSeedRand(&pqParam->hPlayer->tSeed, 2));
			return;
		}
	}
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, dwClass,
		(int)D2CommonSeedRand(&pqParam->hPlayer->tSeed, 3) + 2);
}

BYTE __fastcall A1Q6NaviActive(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer, void *pPlayerRecord  )
{
	void *pRecord;
	GAME *ptGame;

	if (nNpcClass == 266 && QuestsFind(pQuest->ptGame, 1)->bActive && !QuestsGetRecordBit(pQuest->ptGame, 1, 13))
	{
		ptGame = pQuest->ptGame;
		pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
		if (!D2CommonStepTestGridBit(pRecord, 1, 0) && !D2CommonStepTestGridBit(pRecord, 1, 13)
			&& !D2CommonStepTestGridBit(pRecord, 1, 1))
			return TRUE;
	}
	return FALSE;
}
