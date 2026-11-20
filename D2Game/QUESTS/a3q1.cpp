#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a3q1.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/MonsterUnique.h"
#include "QUESTS.h"
#include "a3q1.h"
#include "a2q5.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"

#define A3Q1_CODE_BOOK		543318626
#define A3Q2_CODE_FLAIL1	540108401
#define A3Q2_CODE_FLAIL2	540173937
#define A3Q2_CODE_EYE		544826737
#define A3Q2_CODE_HEART		544368753
#define A3Q2_CODE_BRAIN		544367217

static QUESTNPCTABLE sgtA3Q1Npcs[6] =
{
	{ { { 254, 549, { 0, 0 }, 1 } }, 1 },
	{ { { 254, 550, { 0, 0 }, 2 }, { 255, 551, { 0, 0 }, 2 }, { 264, 552, { 0, 0 }, 2 },
		{ 252, 553, { 0, 0 }, 2 }, { 253, 554, { 0, 0 }, 2 }, { 245, 555, { 0, 0 }, 2 },
		{ 297, 556, { 0, 0 }, 2 } }, 7 },
	{ { { 254, 557, { 0, 0 }, 2 }, { 255, 558, { 0, 0 }, 2 }, { 264, 559, { 0, 0 }, 2 },
		{ 252, 560, { 0, 0 }, 2 }, { 253, 561, { 0, 0 }, 2 }, { 245, 562, { 0, 0 }, 2 },
		{ 297, 563, { 0, 0 }, 2 } }, 7 },
	{ { { 254, 564, { 0, 0 }, 1 }, { 255, 565, { 0, 0 }, 2 }, { 264, 566, { 0, 0 }, 2 },
		{ 252, 567, { 0, 0 }, 2 }, { 253, 568, { 0, 0 }, 2 }, { 245, 569, { 0, 0 }, 2 },
		{ 297, 570, { 0, 0 }, 2 } }, 7 },
	{ { { 245, 569, { 0, 0 }, 2 }, { 297, 570, { 0, 0 }, 2 } }, 2 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static int sgnA3Q1NpcTable[6] = { -1, 0, 1, 2, 3, 4 };

BYTE __fastcall A3Q1Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
BOOL __fastcall A3Q1SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A3Q1Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q1NpcMessage(QUEST *pQuestInfo, QUESTPARAM *pqParam);
BOOL __fastcall A3Q1FlagStarted(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A3Q1CreditInAct(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A3Q1Reward(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A3Q1FlagSeen(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A3Q1Activate(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q1ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q1ItemAcquired(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q1ItemDropped(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q1PlayerDropped(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q1PlayerJoined(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q1PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A3Q1Sequence(QUEST *pQuestInfo);
BYTE __fastcall A3Q1StatusFilter(QUEST *pQuest, UNIT *hPlayer, void *pGameRecord, void *pPlayerRecord,
	BYTE *pbStatus);
void __fastcall A3Q1PartyHasBook(GAME *ptGame, UNIT *hUnit, void *pArg);

void __fastcall A3Q1ObjInit(OBJINIT *ptInit  )
{
	QUEST *pQuest;
	A3Q0RECORD *pRecord;
	UNIT *hNpc;

	pQuest = QuestsFind(ptInit->ptGame, 14);
	if (!pQuest)
		return;
	pRecord = (A3Q0RECORD *)pQuest->pQuestRecord;
	pRecord->bSpotSet = 1;
	pRecord->nSpotX = ptInit->nX;
	pRecord->nSpotY = ptInit->nY;
	if (!QuestsGetRecordBit(ptInit->ptGame, 16, 13))
		return;
	if (pRecord->bSpotHratliSpawned)
		return;
	if (pRecord->bHratliSpawned && SUnitFind(ptInit->ptGame, 1, pRecord->dwHratliId))
	{
		pRecord->bHratliFound = 1;
		return;
	}
	hNpc = MonsterSpawn(ptInit->ptGame, ptInit->hRoom, ptInit->nX, ptInit->nY, 253, 1, -1, 0);
	if (!hNpc)
		return;
	pRecord->dwHratliId = hNpc->dwId;
	pRecord->bSpotHratliSpawned = 1;
	hNpc->dwFlags0 |= 50331648;
}

BYTE __fastcall A3Q1Active(QUEST *pQuest  , int nNpcClass, UNIT *hPlayer,
	void *pPlayerRecord)
{
	if (nNpcClass == 254)
	{
		if (pQuest->bState == 1 && !D2CommonStepTestGridBit(pPlayerRecord, 17, 0)
			&& !D2CommonStepTestGridBit(pPlayerRecord, 17, 15))
			return TRUE;
		if (ITEMSFindByType(hPlayer, A3Q1_CODE_BOOK))
			return TRUE;
	}
	return FALSE;
}

void __fastcall A3Q1ObjectMode(QUEST *pQuest  , OBJINIT *ptInit
)
{
	if (!pQuest)
		return;
	if (!pQuest->bActive)
		D2CommonUnitSetMode(ptInit->hObject, 2);
	else
		D2CommonUnitSetMode(ptInit->hObject, ((A3Q1RECORD *)pQuest->pQuestRecord)->dwObjectMode);
}

int __fastcall A3Q1OperateObject(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuest;
	A3Q1RECORD *pRecord;
	int nLevel;

	pQuest = QuestsFind(pObjectParam->ptGame, 15);
	if (!pQuest)
		return 0;
	if (!pQuest->bActive)
		return 0;
	if (pObjectParam->hObject->dwMode)
		return 0;
	pObjectParam->hObject->dwDropItemCode = A3Q1_CODE_BOOK;
	if (!ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 2, &nLevel, NULL, 44, FALSE))
		return 0;
	D2CommonUnitSetMode(pObjectParam->hObject, 2);
	pRecord = (A3Q1RECORD *)pQuest->pQuestRecord;
	pRecord->bBookInGame = 1;
	pRecord->bObjectOperated = 1;
	pRecord->nBookCount++;
	pRecord->dwObjectMode = 2;
	pRecord->dwObjectId = pObjectParam->hObject->dwId;
	pQuest->pCallback[0] = A3Q1Talk;
	pQuest->pCallback[11] = A3Q1NpcMessage;
	if (pQuest->bState != 3)
	{
#line 688
		QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
	}
	if (pQuest->bStatus != 1)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 1, NULL, A3Q1SendStatus, FALSE);
	}
	return 0;
}

BOOL __fastcall A3Q1SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 17, 0) || D2CommonStepTestGridBit(pRecord, 17, 15))
		&& !D2CommonStepTestGridBit(pRecord, 17, 13)
		&& !D2CommonStepTestGridBit(pRecord, 17, 14))
		return FALSE;
	QuestsSendStatus(ptGame, hUnit, 15);
	return FALSE;
}

void __fastcall A3Q1Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	DWORD dwDone;
	DWORD dwBit13;
	int nNpcClass;
	int nTable;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	dwDone = D2CommonStepTestGridBit(pRecord, 17, 0);
	dwBit13 = D2CommonStepTestGridBit(pRecord, 17, 13);
	nNpcClass = pqParam->hEventUnit->dwClassId;
	if (dwDone == 1 && !dwBit13)
		return;
	if (ITEMSFindByType(pqParam->hPlayer, A3Q1_CODE_BOOK))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 3);
		return;
	}
	if (dwDone == 1)
	{
		if (QuestsHasPlayer(pQuest, pqParam->hPlayer->dwId))
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 4);
		return;
	}
	if (!pQuest->bActive)
		return;
	if (pQuest->bState >= 4)
		return;
	nTable = sgnA3Q1NpcTable[pQuest->bState];
	if (nTable == -1)
		return;
	if ((DWORD)nTable >= 6)
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, nTable);
}

void __fastcall A3Q1NpcMessage(QUEST *pQuestInfo  , QUESTPARAM *pqParam
)
{
	A3Q1RECORD *pQuestRecord;
	void *pRecord;
	WORD wNpcClass;
	WORD wMessage;
	GAME *ptGame;
	UNIT *hPlayer;

	wNpcClass = (WORD)pqParam->field_14;
	wMessage = (WORD)pqParam->field_18;
	pQuestRecord = (A3Q1RECORD *)pQuestInfo->pQuestRecord;
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 17, 0) == 1)
		return;
	if (wNpcClass != 254)
		return;
	if (wMessage == 549)
	{
#line 411
		QuestsSetState(pQuestInfo, 2, D2_THIS_FILE, __LINE__);
		pQuestInfo->bStatusFlags = 0;
		QuestsSetStatus(pQuestInfo, 1, NULL, A3Q1SendStatus, TRUE);
		SUnitIterate(pQuestInfo->ptGame, 0, NULL, A3Q1FlagStarted);
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		return;
	}
	if (wMessage != 564)
		return;
	QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
	if (D2CommonStepTestGridBit(pRecord, 17, 0))
		return;
	if (ITEMSFindByType(pqParam->hPlayer, A3Q1_CODE_BOOK))
	{
		QuestsRemoveItem(pqParam->ptGame, pqParam->hPlayer, A3Q1_CODE_BOOK);
		pQuestRecord->bBookHandedIn = 1;
		pQuestRecord->dwHandedInById = pqParam->hPlayer->dwId;
		pQuestInfo->pCallback[2] = A3Q1Activate;
	}
	if (pQuestRecord->bRewardPending == 1)
	{
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A3Q1CreditInAct);
		pQuestRecord->bRewardPending = 0;
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A3Q1Reward);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A3Q1FlagSeen);
	}
	pQuestInfo->bStatusFlags = 0;
	QuestsSetStatus(pQuestInfo, 13, NULL, A3Q1SendStatus, FALSE);
	QuestsPlayersAdd(&pQuestInfo->tPlayers, pqParam->hPlayer->dwId);
	if (!D2CommonStepTestGridBit(pRecord, 17, 13))
		return;
	if (!pQuestInfo->bActive)
		return;
	QuestsSetRecordBit(pqParam->ptGame, 17, 13);
	if (pQuestInfo->bState == 5)
		return;
#line 451
	QuestsSetState(pQuestInfo, 5, D2_THIS_FILE, __LINE__);
#line 452
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
	pQuestInfo->pSequence(pQuestInfo);
}

BOOL __fastcall A3Q1FlagStarted(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	QUEST *pQuest;
	int dwState;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 17, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 17, 1) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 15);
	if (!pQuest)
		return FALSE;
	dwState = pQuest->bState;
	switch (dwState)
	{
	case 2:
		D2CommonQuestRecordSetBit(pRecord, 17, 2);
		break;
	case 3:
		D2CommonQuestRecordSetBit(pRecord, 17, 2);
		break;
	}
	return FALSE;
}

BOOL __fastcall A3Q1CreditInAct(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 17, 0))
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 17, 1))
		return FALSE;
	if (D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom)) != 2)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 17, 13);
	D2CommonQuestRecordSetBit(pRecord, 17, 0);
	D2CommonQuestRecordSetBit(pRecord, 17, 1);
	QuestsSendPlayerRecord(ptGame, hUnit);
	return FALSE;
}

BOOL __fastcall A3Q1Reward(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 17, 1) == 1)
	{
		D2CommonStatsUnitAddValue(hUnit, 4, 5);
		QuestsSendFlag(hUnit, 15);
		D2CommonQuestRecordClearBit(pRecord, 17, 1);
	}
	return FALSE;
}

BOOL __fastcall A3Q1FlagSeen(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 17, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 17, 1) == 1)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 17, 14);
	return FALSE;
}

void __fastcall A3Q1Activate(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q1RECORD *pQuestRecord;

	if ((WORD)pqParam->hEventUnit->dwClassId != 254)
		return;
	pQuestRecord = (A3Q1RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bBookHandedIn != 1)
		return;
	if (pqParam->hPlayer->dwId != pQuestRecord->dwHandedInById)
		return;
	SUnitSetEvent(pqParam->hPlayer, 65, pqParam->hPlayer);
	pQuestRecord->bBookHandedIn = 0;
}

void __fastcall A3Q1Init(QUEST *pQuest  )
{
	A3Q1RECORD *pQuestRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->bState = 0;
	pQuest->ptNpcTables = sgtA3Q1Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->eFilter = 17;
	pQuest->bDoneStateMin = 4;
	pQuest->pStatusFilter = A3Q1StatusFilter;
	pQuest->pActiveFilter = A3Q1Active;
	pQuest->pSequence = A3Q1Sequence;
	pQuest->dwNextQuestId = 19;
#line 919
	pQuestRecord = (A3Q1RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A3Q1RECORD));
	pQuest->pQuestRecord = pQuestRecord;
	pQuest->pCallback[4] = A3Q1ItemAcquired;
	pQuest->pCallback[5] = A3Q1ItemDropped;
	pQuest->pCallback[0] = A3Q1Talk;
	pQuest->pCallback[11] = A3Q1NpcMessage;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A3Q1ChangedLevel;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A3Q1PlayerStarted;
	pQuest->pCallback[QUESTCALLBACK_PLAYERJOINEDGAME] = A3Q1PlayerJoined;
	pQuest->pCallback[10] = A2Q5PlayerDone;
	pQuest->pCallback[QUESTCALLBACK_PLAYERDROPPEDWITHQUESTITEM] = A3Q1PlayerDropped;
	for (i = 0; i < (int)(sizeof(A3Q1RECORD) / sizeof(DWORD)); i++)
		((DWORD *)pQuestRecord)[i] = 0;
	pQuestRecord->bObjectOperated = 0;
	pQuestRecord->bBookInGame = 0;
	pQuestRecord->bRewardPending = 1;
	QuestsPlayersClear(&pQuestRecord->tPlayers);
}

void __fastcall A3Q1ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	UNIT *hPlayer;
	GAME *ptGame;
	DWORD dwLevelNew;
	DWORD dwLevelOld;

	dwLevelNew = pqParam->field_14;
	dwLevelOld = pqParam->field_18;
	if (dwLevelOld == 79 && pQuest->bActive && pQuest->bState < 1)
		pQuest->bState = 1;
	if (dwLevelNew != 75)
		return;
	QuestsPlayerLeft(pQuest, pqParam);
	if (pQuest->bState != 2)
		return;
	ptGame = pQuest->ptGame;
	hPlayer = pqParam->hPlayer;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0) == 1)
		return;
	if (pQuest->bStatus != 1)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 1, NULL, A3Q1SendStatus, TRUE);
	}
#line 358
	QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
	SUnitIterate(pQuest->ptGame, 0, NULL, A3Q1FlagStarted);
}

void __fastcall A3Q1ItemAcquired(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q1RECORD *pQuestRecord;

	pQuestRecord = (A3Q1RECORD *)pQuest->pQuestRecord;
	if (!pQuest->bActive)
		return;
#line 603
	QuestsDebugMessage(pQuest->ptGame, "tome acquired", D2_THIS_FILE, __LINE__);
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 2, NULL, A3Q1SendStatus, TRUE);
#line 614
	QuestsSetState(pQuest, 4, D2_THIS_FILE, __LINE__);
	QuestsPlayersAdd(&pQuestRecord->tPlayers, pqParam->hPlayer->dwId);
}

void __fastcall A3Q1ItemDropped(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	QuestsPlayersRemove(&((A3Q1RECORD *)pQuest->pQuestRecord)->tPlayers, pqParam->hPlayer->dwId);
	if (!pQuest->bActive)
		return;
#line 635
	QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 1, NULL, A3Q1SendStatus, TRUE);
}

void __fastcall A3Q1PlayerDropped(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q1RECORD *pQuestRecord;

	pQuestRecord = (A3Q1RECORD *)pQuest->pQuestRecord;
	if (--pQuestRecord->nBookCount)
		return;
	if (!pQuest->bActive)
		return;
	if (!pQuestRecord->bBookInGame)
		return;
	if (pQuest->bState > 3)
		return;
	pQuestRecord->bBookLost = 1;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 8, NULL, A3Q1SendStatus, TRUE);
}

void __fastcall A3Q1PlayerJoined(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q1RECORD *pQuestRecord;

	pQuestRecord = (A3Q1RECORD *)pQuest->pQuestRecord;
	if (!ITEMSFindByType(pqParam->hPlayer, A3Q1_CODE_BOOK))
		return;
	if (++pQuestRecord->nBookCount != 1)
		return;
	if (!pQuest->bActive)
		return;
	if (!pQuestRecord->bBookLost)
		return;
	pQuestRecord->bBookLost = 0;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 2, NULL, A3Q1SendStatus, TRUE);
}

void __fastcall A3Q1PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q1RECORD *pQuestRecord;
	void *pRecord;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 17, 0) == 1)
	{
		QuestsSetRecordBit(pQuest->ptGame, 17, 13);
		return;
	}
	if (ITEMSFindByType(pqParam->hPlayer, A3Q1_CODE_BOOK))
	{
		pQuest->bState = 4;
		pQuest->bStatus = 2;
		pQuestRecord = (A3Q1RECORD *)pQuest->pQuestRecord;
		pQuestRecord->dwObjectMode = 2;
		pQuestRecord->bBookInGame = 1;
		pQuestRecord->nBookCount++;
		QuestsPlayersAdd(&pQuestRecord->tPlayers, pqParam->hPlayer->dwId);
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 17, 3) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 17, 2) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 2;
	}
}

BYTE __fastcall A3Q1Sequence(QUEST *pQuestInfo  )
{
	QUEST *pNextQuest;

	if (pQuestInfo->bState != 5 && pQuestInfo->bActive)
		return TRUE;
	pNextQuest = QuestsFind(pQuestInfo->ptGame, (int)pQuestInfo->dwNextQuestId);
	if (pNextQuest)
	{
#line 797
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		return pNextQuest->pSequence(pNextQuest);
	}
	return FALSE;
}

BYTE __fastcall A3Q1StatusFilter(QUEST *pQuest  , UNIT *hPlayer,
	void *pGameRecord, void *pPlayerRecord, BYTE *pbStatus)
{
	A3Q1RECORD *pQuestRecord;
	WORD wParty;

	if (!D2CommonStepTestGridBit(pPlayerRecord, 15, 0))
	{
		*pbStatus = 0;
		return TRUE;
	}
	if (D2CommonStepTestGridBit(pPlayerRecord, 17, 0))
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 17, 13))
			*pbStatus = 13;
		else
			*pbStatus = 11;
		return TRUE;
	}
	*pbStatus = 0;
	if (ITEMSFindByType(hPlayer, A3Q1_CODE_BOOK))
	{
		*pbStatus = 2;
		return TRUE;
	}
	pQuestRecord = (A3Q1RECORD *)pQuest->pQuestRecord;
	wParty = SUnitGetParty(hPlayer);
	if (wParty != 65535)
	{
		pQuestRecord->bPartyHasBook = 0;
		PartyForEach(pQuest->ptGame, wParty, A3Q1PartyHasBook, NULL);
		if (pQuestRecord->bPartyHasBook)
		{
			*pbStatus = 2;
			return TRUE;
		}
	}
	if (pQuest->bActive)
	{
		if (pQuest->bState <= 3)
		{
			*pbStatus = 1;
			if (pQuestRecord->bBookInGame == 1 && !pQuestRecord->nBookCount)
			{
				if (pQuest->ptGame->bGameType != 3)
					*pbStatus = 9;
				else
					*pbStatus = 8;
				return TRUE;
			}
		}
		else
		{
			*pbStatus = 12;
		}
	}
	return TRUE;
}

void __fastcall A3Q1PartyHasBook(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	if (ITEMSFindByType(hUnit, A3Q1_CODE_BOOK))
		((A3Q1RECORD *)QuestsFind(ptGame, 15)->pQuestRecord)->bPartyHasBook = 1;
}

static __inline BYTE A3Q1FlailFree(QUEST *pQuest19)
{
	if (!pQuest19)
		return TRUE;
	return !((A3Q5RECORD *)pQuest19->pQuestRecord)->bFlailPlaced;
}

BYTE __fastcall A3Q2Active(QUEST *pQuest  , int nNpcClass, UNIT *hPlayer,
	void *pPlayerRecord)
{
	QUEST *pQuest19;
	int nHeld;

	if (nNpcClass != 245)
		return FALSE;
	if (pQuest->bState >= 1 && !D2CommonStepTestGridBit(pPlayerRecord, 18, 2))
		return TRUE;
	nHeld = 0;
	if (ITEMSFindByType(hPlayer, A3Q2_CODE_FLAIL1))
	{
		nHeld = 1;
		if (!D2CommonStepTestGridBit(pPlayerRecord, 18, 5))
			return TRUE;
	}
	if (ITEMSFindByType(hPlayer, A3Q2_CODE_EYE))
	{
		nHeld++;
		if (!D2CommonStepTestGridBit(pPlayerRecord, 18, 3))
			return TRUE;
	}
	if (ITEMSFindByType(hPlayer, A3Q2_CODE_HEART))
	{
		nHeld++;
		if (!D2CommonStepTestGridBit(pPlayerRecord, 18, 6))
			return TRUE;
	}
	if (ITEMSFindByType(hPlayer, A3Q2_CODE_BRAIN))
	{
		nHeld++;
		if (!D2CommonStepTestGridBit(pPlayerRecord, 18, 4))
			return TRUE;
	}
	if (ITEMSFindByType(hPlayer, A3Q2_CODE_FLAIL2) && !D2CommonStepTestGridBit(pPlayerRecord, 18, 7))
	{
		pQuest19 = QuestsFind(pQuest->ptGame, 19);
		if (A3Q1FlailFree(pQuest19))
			return TRUE;
	}
	if (nHeld == 4 && !D2CommonStepTestGridBit(pPlayerRecord, 18, 5))
		return TRUE;
	return FALSE;
}
