#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a1q1.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "QUESTS.h"
#include "a1q1.h"
#include "a2q8.h"
#include "a3q0.h"
#include "../PLAYER/PlrIntro.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"

struct A1Q1RECORD
{
 	QUESTPLAYERS	tPlayers;
 	BYTE		bFinished;
 	BYTE		bCaveEntered;
 	BYTE		bStatus1Pending;
 	BYTE		bTimerPending;
 	int			nMonstersLeft;
};
typedef struct A1Q1RECORD A1Q1RECORD;

static QUESTNPCTABLE sgtQuestIntroNpcs[3] =
{
	{ { { 148, 11, { 0, 0 }, 0 }, { 147, 45, { 0, 0 }, 0 }, { 154, 36, { 0, 0 }, 0 },
		{ 150, 24, { 0, 0 }, 0 } }, 4 },
	{ { { 148, 12, { 0, 0 }, 0 }, { 147, 46, { 0, 0 }, 0 }, { 154, 37, { 0, 0 }, 0 },
		{ 150, 25, { 0, 0 }, 0 } }, 4 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static QUESTNPCTABLE sgtQuest0Npcs[3] =
{
	{ { { 155, 0, { 0, 0 }, 0 } }, 1 },
	{ { { 155, 1, { 0, 0 }, 0 } }, 1 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static QUESTNPCTABLE sgtA1Q1Npcs[8] =
{
	{ { { 148, 64, { 0, 0 }, 0 } }, 1 },
	{ { { 148, 65, { 0, 0 }, 2 }, { 155, 70, { 0, 0 }, 2 }, { 147, 69, { 0, 0 }, 2 },
		{ 150, 66, { 0, 0 }, 2 }, { 154, 67, { 0, 0 }, 2 } }, 5 },
	{ { { 150, 72, { 0, 0 }, 2 }, { 155, 75, { 0, 0 }, 2 }, { 154, 73, { 0, 0 }, 2 },
		{ 148, 71, { 0, 0 }, 2 }, { 147, 74, { 0, 0 }, 2 } }, 5 },
	{ { { 150, 77, { 0, 0 }, 2 }, { 155, 80, { 0, 0 }, 2 }, { 154, 78, { 0, 0 }, 2 },
		{ 148, 76, { 0, 0 }, 0 }, { 147, 79, { 0, 0 }, 2 } }, 5 },
	{ { { 155, 80, { 0, 0 }, 2 }, { 154, 78, { 0, 0 }, 2 }, { 147, 79, { 0, 0 }, 2 } }, 3 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static int sgnA1Q1NpcTable[6] = { -1, 0, 1, 2, 3, 4 };

void __fastcall A1Q1PartyCredit(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A1Q1Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall A1Q1Activate(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A1Q1SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A1Q1NpcMessage(QUEST *pQuestInfo, QUESTPARAM *pqParam);
BOOL __fastcall A1Q1UpdatePlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A1Q1Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q1PlayerLeft(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q1UnitKilled(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A1Q1KilledFlag(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A1Q1KilledEvent(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A1Q1KilledParty(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A1Q1Timer(GAME *ptGame, QUEST *pQuest);
void __fastcall A1Q1ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A1Q1Sequence(QUEST *pQuest);
void __fastcall A1Q1PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);

void __fastcall QuestsIntroInit(QUEST *pQuest  )
{
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->pCallback[0] = QuestsIntroTalk;
	pQuest->pCallback[11] = QuestsIntroMet;
	pQuest->ptNpcTables = sgtQuestIntroNpcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->bState = 0;
	pQuest->bStatus = 0;
	pQuest->pQuestRecord = NULL;
	pQuest->eFilter = 41;
	pQuest->pStatusFilter = A3Q0IntroStatusFilter;
	pQuest->pActiveFilter = QuestsIntroActive;
}

void __fastcall QuestsIntroMet(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	int nNpcClass;
	WORD wMessage;

	nNpcClass = pqParam->field_14;
	wMessage = (WORD)pqParam->field_18;
	switch (nNpcClass)
	{
	case 148:
		if (wMessage == 11 || wMessage == 12)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	case 147:
		if (wMessage == 45 || wMessage == 46)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	case 154:
		if (wMessage == 36 || wMessage == 37)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	case 150:
		if (wMessage == 24 || wMessage == 25)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	}
}

void __fastcall QuestsIntroTalk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	int nNpcClass;
	int nPlayerClass;
	int nFavoured;

	nNpcClass = pqParam->hEventUnit->dwClassId;
	nPlayerClass = pqParam->hPlayer->dwClassId;
	switch (nNpcClass)
	{
	case 150:
		nFavoured = 0;
		break;
	case 147:
		nFavoured = 2;
		break;
	case 154:
		nFavoured = 4;
		break;
	case 148:
		nFavoured = 1;
		break;
	default:
		return;
	}
	if (PlrIntroTestMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass))
		return;
	if (nPlayerClass == nFavoured)
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 1);
	else
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 0);
}

BYTE __fastcall QuestsIntroActive(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer  , void *pPlayerRecord)
{
	void *pRecord;
	GAME *ptGame;

	ptGame = pQuest->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 1, 0) != 1 && nNpcClass == 148
		&& !PlrIntroTestMet(hPlayer, pQuest->ptGame, 148))
		return TRUE;
	return FALSE;
}

void __fastcall QuestsInitWarriv(QUEST *pQuest  )
{
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->pCallback[0] = A1Q1WarrivTalk;
	pQuest->pCallback[11] = A1Q1WarrivNpcMessage;
	pQuest->ptNpcTables = sgtQuest0Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->bState = 0;
	pQuest->bStatus = 0;
	pQuest->pQuestRecord = NULL;
	pQuest->eFilter = 0;
	pQuest->pStatusFilter = A2Q8StatusFilter;
	pQuest->pActiveFilter = A1Q1WarrivActive;
}

void __fastcall A1Q1WarrivNpcMessage(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	WORD wMessage;
	void *pRecord;
	GAME *ptGame;
	UNIT *hPlayer;

	wMessage = (WORD)pqParam->field_18;
	if ((WORD)pqParam->field_14 != 155)
		return;
	if (wMessage != 0 && wMessage != 1)
		return;
	hPlayer = pqParam->hPlayer;
	ptGame = pQuest->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	D2CommonQuestRecordSetBit(pRecord, 0, 0);
}

void __fastcall A1Q1WarrivTalk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	GAME *ptGame;
	UNIT *hPlayer;

	if (pqParam->hEventUnit->dwClassId != 155)
		return;
	hPlayer = pqParam->hPlayer;
	ptGame = pQuest->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 0, 0))
		return;
	if (pqParam->hPlayer->dwClassId == 3)
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, 155, 1);
	else
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, 155, 0);
}

BYTE __fastcall A1Q1WarrivActive(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer  , void *pPlayerRecord)
{
	if (nNpcClass == 155 && !D2CommonStepTestGridBit(pPlayerRecord, 0, 0))
		return TRUE;
	return FALSE;
}

void __fastcall A1Q1PartyCredit(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	int nLevelId;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 1, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, 1, 1) == 1)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	nLevelId = (int)D2CommonDungeonRoomGetLevelId(hRoom);
	if (!nLevelId)
		return;
	if (D2CommonGetActNum(nLevelId))
		return;
	D2CommonQuestRecordSetBit(pRecord, 1, 13);
	D2CommonQuestRecordSetBit(pRecord, 1, 1);
}

DWORD __fastcall A1Q1GetRemaining(QUEST *pQuest  )
{
	return (DWORD)((A1Q1RECORD *)pQuest->pQuestRecord)->nMonstersLeft;
}

BYTE __fastcall A1Q1Active(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer  , void *pPlayerRecord)
{
	if (nNpcClass == 148 && D2CommonStepTestGridBit(pPlayerRecord, pQuest->eFilter, 0) != 1)
	{
		if (pQuest->bActive == 1 && pQuest->bState == 1
			&& !D2CommonStepTestGridBit(pPlayerRecord, pQuest->eFilter, 1))
			return TRUE;
		if (D2CommonStepTestGridBit(pPlayerRecord, pQuest->eFilter, 1) == 1)
			return TRUE;
	}
	return FALSE;
}

void __fastcall A1Q1Init(QUEST *pQuest  )
{
	A1Q1RECORD *pRecord;
	DWORD *pdw;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->pCallback[10] = A1Q1PlayerLeft;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A1Q1ChangedLevel;
	pQuest->pCallback[8] = A1Q1UnitKilled;
	pQuest->pCallback[0] = A1Q1Talk;
	pQuest->pCallback[2] = A1Q1Activate;
	pQuest->pCallback[11] = A1Q1NpcMessage;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A1Q1PlayerStarted;
	pQuest->ptNpcTables = sgtA1Q1Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->bState = 1;
#line 690
	pRecord = (A1Q1RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A1Q1RECORD));
	pQuest->pQuestRecord = pRecord;
	pQuest->eFilter = 1;
	pQuest->pStatusFilter = NULL;
	pQuest->bDoneStateMin = 4;
	pQuest->pActiveFilter = A1Q1Active;
	pQuest->pSequence = A1Q1Sequence;
	pQuest->dwNextQuestId = 2;
	pdw = (DWORD *)pRecord;
	for (i = 0; i < (int)(sizeof(A1Q1RECORD) / sizeof(DWORD)); i++)
		pdw[i] = 0;
	QuestsPlayersClear(&pRecord->tPlayers);
}

void __fastcall A1Q1Activate(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q1RECORD *pRecord;

	if ((WORD)pqParam->hEventUnit->dwClassId != 148)
		return;
	pRecord = (A1Q1RECORD *)pQuest->pQuestRecord;
	if (pRecord->bStatus1Pending != 1)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 1, NULL, A1Q1SendStatus, TRUE);
	pRecord->bStatus1Pending = 0;
	pQuest->pCallback[2] = NULL;
}

BOOL __fastcall A1Q1SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 1, 0) || D2CommonStepTestGridBit(pRecord, 1, 15))
		&& !D2CommonStepTestGridBit(pRecord, 1, 13) && !D2CommonStepTestGridBit(pRecord, 1, 14))
		return FALSE;
	QuestsSendStatus(ptGame, hUnit, 1);
	return FALSE;
}

void __fastcall A1Q1NpcMessage(QUEST *pQuestInfo  , QUESTPARAM *pqParam
)
{
	A1Q1RECORD *pQuestRecord;
	WORD wMessage;
	void *pRecord;
	GAME *ptGame;
	UNIT *hPlayer;

	pQuestRecord = (A1Q1RECORD *)pQuestInfo->pQuestRecord;
	wMessage = (WORD)pqParam->field_18;
	if ((WORD)pqParam->field_14 != 148)
		return;
	hPlayer = pqParam->hPlayer;
	ptGame = pQuestInfo->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (wMessage == 64)
	{
		pQuestRecord->bStatus1Pending = 1;
#line 197
		QuestsSetState(pQuestInfo, 2, D2_THIS_FILE, __LINE__);
		SUnitIterate(pQuestInfo->ptGame, 0, NULL, A1Q1UpdatePlayer);
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		return;
	}
	if (wMessage != 76)
		return;
	if (D2CommonStepTestGridBit(pRecord, pQuestInfo->eFilter, 1) != 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, pQuestInfo->eFilter, 13))
	{
		if (pQuestInfo->bState != 5)
		{
#line 209
			QuestsSetState(pQuestInfo, 5, D2_THIS_FILE, __LINE__);
#line 210
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
			pQuestInfo->pSequence(pQuestInfo);
			pQuestInfo->bStatusFlags = 0;
			QuestsSetStatus(pQuestInfo, 13, NULL, A1Q1SendStatus, FALSE);
		}
		pQuestInfo->pCallback[2] = NULL;
	}
	D2CommonQuestRecordSetBit(pRecord, pQuestInfo->eFilter, 0);
	D2CommonQuestRecordClearBit(pRecord, pQuestInfo->eFilter, 1);
	D2CommonQuestRecordClearMidBits(pRecord, pQuestInfo->eFilter);
	D2CommonStatsUnitAddValue(pqParam->hPlayer, 5, 1);
	QuestsPlayersAdd(&pQuestInfo->tPlayers, pqParam->hPlayer->dwId);
	QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
}

BOOL __fastcall A1Q1UpdatePlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	QUEST *pQuest;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 1, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 1, 1) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 1);
	if (!pQuest)
		return FALSE;
	if (pQuest->bState == 2)
	{
		D2CommonQuestRecordSetBit(pRecord, 1, 2);
		return FALSE;
	}
	if (pQuest->bState == 3)
	{
		if (pQuest->bStatus == 1)
		{
			D2CommonQuestRecordSetBit(pRecord, 1, 3);
			return FALSE;
		}
		D2CommonQuestRecordSetBit(pRecord, 1, 4);
	}
	return FALSE;
}

void __fastcall A1Q1Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	DWORD dwDone;
	DWORD dwCredited;
	int nNpcClass;
	int nTable;
	GAME *ptGame;
	ptGame = pQuest->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	dwDone = D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0);
	dwCredited = D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 13);
	nNpcClass = (int)pqParam->hEventUnit->dwClassId;
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
	if (dwDone != 1 && (pQuest->bState < 4 || dwCredited) && pQuest->bActive)
	{
		nTable = sgnA1Q1NpcTable[pQuest->bState];
		if (nTable != -1 && (DWORD)nTable < 8)
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, nTable);
	}
}

void __fastcall A1Q1PlayerLeft(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	QuestsPlayersRemove(&pQuest->tPlayers, pqParam->hPlayer->dwId);
	QuestsPlayersRemove(&((A1Q1RECORD *)pQuest->pQuestRecord)->tPlayers, pqParam->hPlayer->dwId);
}

void __fastcall A1Q1UnitKilled(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q1RECORD *pRecord;
	MONREGION *ptRegionStats;
	DWORD dwUnitId;
	int nRooms;

	if (!pQuest->bActive)
		return;
	pRecord = (A1Q1RECORD *)pQuest->pQuestRecord;
	ptRegionStats = (MONREGION *)D2CommonMonsterRegionArrayGetDword(&pqParam->ptGame->pMonRegions, 8);
#line 423
	D2ASSERT(ptRegionStats != NULL);
	pRecord->nMonstersLeft = ptRegionStats->nMonsters - ptRegionStats->field_130;
	if (pqParam->hPlayer)
	{
		dwUnitId = pqParam->hPlayer->dwId;
		if (!QuestsPlayersFind(&pRecord->tPlayers, dwUnitId))
			QuestsPlayersAdd(&pRecord->tPlayers, dwUnitId);
	}
	nRooms = D2CommonDungeonActCountLevelRooms(pqParam->ptGame->ptAct[D2CommonGetActNum(LEVEL_VIS_MAX)],
		LEVEL_VIS_MAX);
	if (nRooms <= (int)ptRegionStats->dwRoomsPopulated && ptRegionStats->field_130
		== ptRegionStats->nMonsters)
	{
#line 442
		QuestsDebugMessage(pQuest->ptGame, "Finished quest", D2_THIS_FILE, __LINE__);
		pRecord->bFinished = 1;
		pQuest->pCallback[2] = NULL;
#line 445
		QuestsSetState(pQuest, 4, D2_THIS_FILE, __LINE__);
		pQuest->pCallback[8] = NULL;
		QuestsSetRecordBit(pqParam->ptGame, pQuest->eFilter, 13);
		QuestsPlayersSetStatus(pqParam->ptGame, &pRecord->tPlayers, pQuest->eFilter, 0);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A1Q1KilledParty);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A1Q1KilledFlag);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A1Q1KilledEvent);
		QuestsSetAll(pQuest->ptGame, 0);
		if (!pRecord->bTimerPending)
		{
			pRecord->bTimerPending = 1;
			QuestsAddTimer(pQuest, A1Q1Timer, 8);
		}
		return;
	}
	if (nRooms <= (int)ptRegionStats->dwRoomsPopulated && pRecord->nMonstersLeft <= 5)
	{
		pQuest->bStatusFlags = 32;
		QuestsSetStatus(pQuest, 4, NULL, A1Q1SendStatus, TRUE);
		pQuest->pCallback[2] = NULL;
		return;
	}
	if (pQuest->bStatus == 4 && pRecord->nMonstersLeft > 5)
	{
		pQuest->bStatusFlags = 32;
		QuestsSetStatus(pQuest, 4, NULL, A1Q1SendStatus, TRUE);
	}
}

BOOL __fastcall A1Q1KilledFlag(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 1, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 1, 1) == 1)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 1, 14);
	QuestsSendFlagInAct(hUnit, 1, 0);
	QuestsSendPlayerRecord(ptGame, hUnit);
	return FALSE;
}

BOOL __fastcall A1Q1KilledEvent(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecordX, 1, 13))
		SUnitSetEvent(hUnit, 33, hUnit);
	return FALSE;
}

BOOL __fastcall A1Q1KilledParty(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	WORD wParty;

	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecordX, 1, 13))
	{
		wParty = SUnitGetParty(hUnit);
		if (wParty != 65535)
			PartyForEach(ptGame, wParty, A1Q1PartyCredit, NULL);
	}
	return FALSE;
}

BYTE __fastcall A1Q1Timer(GAME *ptGame  , QUEST *pQuest
)
{
	A1Q1RECORD *pRecord;

	pRecord = (A1Q1RECORD *)pQuest->pQuestRecord;
	if (pQuest->bState == 4)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 5, NULL, A1Q1SendStatus, TRUE);
	}
	pRecord->bTimerPending = 0;
	return TRUE;
}

void __fastcall A1Q1ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q1RECORD *pQuestRecord;
	void *pRecord;
	BYTE bUpdate;
	GAME *ptGame;
	UNIT *hPlayer;
	DWORD dwOne;
	DWORD dwArea;

	pQuestRecord = (A1Q1RECORD *)pQuest->pQuestRecord;
	dwOne = pqParam->field_14;
	dwArea = pqParam->field_18;
	if (dwArea == 8)
	{
		if (!pQuest->bActive)
			return;
		bUpdate = FALSE;
		if (pQuest->bState == 1 || pQuest->bState == 2)
		{
#line 540
			QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
			bUpdate = TRUE;
		}
		pQuestRecord->bCaveEntered = 1;
		if (pQuest->bStatus < 2)
		{
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 2, NULL, A1Q1SendStatus, TRUE);
			pQuest->pCallback[2] = NULL;
		}
		else if (!bUpdate)
			return;
		SUnitIterate(pQuest->ptGame, 0, NULL, A1Q1UpdatePlayer);
		return;
	}
	if (dwOne != 1)
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
#line 568
	QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
	SUnitIterate(pQuest->ptGame, 0, NULL, A1Q1UpdatePlayer);
	if (pQuest->bStatus == 1)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 1, NULL, A1Q1SendStatus, TRUE);
	pQuest->pCallback[2] = NULL;
}

BYTE __fastcall A1Q1Sequence(QUEST *pQuestInfo  )
{
	QUEST *pNextQuest;

	if (pQuestInfo->bState != 5 && pQuestInfo->bActive)
		return TRUE;
	pNextQuest = QuestsFind(pQuestInfo->ptGame, (int)pQuestInfo->dwNextQuestId);
	if (pNextQuest)
	{
#line 618
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		return pNextQuest->pSequence(pNextQuest);
	}
	return FALSE;
}

void __fastcall A1Q1PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
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
	if (D2CommonStepTestGridBit(pRecord, 1, 4) == 1)
	{
		pQuest->bStatus = 2;
		pQuest->bState = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 1, 3) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 1, 2) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 2;
	}
}

BYTE __fastcall A1Q1IsDone(QUEST *pQuest  )
{
	if (pQuest->bActive && pQuest->bState >= 4)
		return TRUE;
	return FALSE;
}

void __fastcall A1Q2PartyCredit(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	int nLevelId;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 2, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, 2, 1) == 1)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	nLevelId = (int)D2CommonDungeonRoomGetLevelId(hRoom);
	if (!nLevelId)
		return;
	if (D2CommonGetActNum(nLevelId))
		return;
	D2CommonQuestRecordSetBit(pRecord, 2, 13);
	D2CommonQuestRecordSetBit(pRecord, 2, 1);
}
