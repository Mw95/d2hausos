#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a2q6.cpp"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "../GAME/Event.h"
#include "../GAME/Game.h"
#include "../GAME/SCmd.h"
#include "../ITEMS/Items.h"
#include "../PLAYER/Player.h"
#include "../PLAYER/PlrTrade.h"
#include "QUESTS.h"
#include "a2q5.h"
#include "a2q6.h"
#include "a3q5.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"
#include "../SKILLS/Skills.h"

#define UNIT_OBJECT			2
#define UNIT_ROOMTILE		5

#define A2Q2_CODE_STAFF		544502632
#define A2Q2_CODE_AMULET	544237942
#define A2Q2_CODE_SHAFT		543585133

#pragma pack(push, 1)
struct A2Q6LEVELCMD
{
 	BYTE		bCmd;
 	WORD		wQuest;
 	WORD		wOrder;
 	BYTE		unobserved_05[10];
};
#pragma pack(pop)
typedef struct A2Q6LEVELCMD A2Q6LEVELCMD;

static QUESTNPCTABLE sgtA2Q6Npcs[8] =
{
	{ { { 201, 430, { 0, 0 }, 1 } }, 1 },
	{ { { 176, 434, { 0, 0 }, 2 }, { 175, 433, { 0, 0 }, 2 }, { 198, 441, { 0, 0 }, 2 },
		{ 199, 432, { 0, 0 }, 2 }, { 201, 431, { 0, 0 }, 2 }, { 177, 439, { 0, 0 }, 2 },
		{ 202, 438, { 0, 0 }, 2 }, { 244, 440, { 0, 0 }, 2 }, { 210, 436, { 0, 0 }, 2 },
		{ 200, 435, { 0, 0 }, 2 }, { 178, 437, { 0, 0 }, 2 } }, 11 },
	{ { { 251, 302, { 0, 0 }, 1 } }, 1 },
	{ { { 176, 445, { 0, 0 }, 2 }, { 175, 446, { 0, 0 }, 2 }, { 198, 451, { 0, 0 }, 2 },
		{ 199, 443, { 0, 0 }, 2 }, { 177, 449, { 0, 0 }, 2 }, { 201, 442, { 0, 0 }, 1 },
		{ 251, 302, { 0, 0 }, 1 }, { 202, 444, { 0, 0 }, 2 }, { 244, 452, { 0, 0 }, 2 },
		{ 210, 450, { 0, 0 }, 2 }, { 200, 448, { 0, 0 }, 2 }, { 178, 447, { 0, 0 }, 2 } }, 12 },
	{ { { 176, 445, { 0, 0 }, 2 }, { 175, 446, { 0, 0 }, 2 }, { 198, 451, { 0, 0 }, 2 },
		{ 199, 443, { 0, 0 }, 2 }, { 177, 449, { 0, 0 }, 2 }, { 201, 442, { 0, 0 }, 2 },
		{ 251, 302, { 0, 0 }, 1 }, { 202, 444, { 0, 0 }, 2 }, { 244, 452, { 0, 0 }, 2 },
		{ 200, 448, { 0, 0 }, 2 }, { 178, 447, { 0, 0 }, 2 } }, 11 },
	{ { { 210, 450, { 0, 0 }, 1 } }, 1 },
	{ { { 176, 445, { 0, 0 }, 1 }, { 175, 446, { 0, 0 }, 1 }, { 177, 449, { 0, 0 }, 1 },
		{ 202, 444, { 0, 0 }, 1 }, { 244, 452, { 0, 0 }, 1 }, { 178, 447, { 0, 0 }, 1 } }, 6 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static int sgnA2Q6NpcTable[6] = { -1, 0, 1, 2, 3, 4 };

static DWORD sgadwA2Q6Classes[7] =
{
	313, 312, 308, 310, 311, 309, 307,
};

void __fastcall A2Q6PartyFlag(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A2Q6Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall A2Q6Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q6Activate(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A2Q6SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q6UpdatePlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A2Q6NpcMessage(QUEST *pQuestInfo, QUESTPARAM *pqParam);
BOOL __fastcall A2Q6CreditInLevel(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q6CreditParty(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q6FlagPlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A2Q6UnitKilled(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A2Q6Timer(GAME *ptGame, QUEST *pQuest);
BOOL __fastcall A2Q6KilledInLevel(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A2Q6ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A2Q6Sequence(QUEST *pQuestInfo);
void __fastcall A2Q6PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);
DWORD __fastcall A2Q6NextClass(QUEST *pQuest);
BYTE __fastcall A2Q6ObjectTimer(GAME *ptGame, QUEST *pQuest);
void __fastcall A2Q6PartyDone(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A2Q6NearPlayer(GAME *ptGame, UNIT *hUnit, void *pArg);

void __fastcall A2Q6PartyFlag(GAME *ptGame  , UNIT *hUnit
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
	if (D2CommonStepTestGridBit(pRecord, 14, 5) == 1)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	if (D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom)) != 1)
		return;
	D2CommonQuestRecordSetBit(pRecord, 14, 5);
}

BYTE __fastcall A2Q6Active(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer, void *pPlayerRecord)
{
	A2Q6RECORD *pQuestRecord;

	if (nNpcClass == 201)
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 14, 0) == 1)
			return FALSE;
		if (pQuest->bState == 1 && !D2CommonStepTestGridBit(pPlayerRecord, 14, 3)
			&& !D2CommonStepTestGridBit(pPlayerRecord, 14, 4))
			return TRUE;
		if (D2CommonStepTestGridBit(pPlayerRecord, 14, 3) == 1)
			return TRUE;
	}
	else if (nNpcClass == 210)
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 14, 4) == 1)
			return TRUE;
	}
	else if (nNpcClass == 251)
	{
		pQuestRecord = (A2Q6RECORD *)pQuest->pQuestRecord;
		if (pQuest->bActive == 1 && pQuestRecord->bDurielKilled == 1 && !pQuestRecord->bTownPortalOpened)
			return TRUE;
	}
	else if (D2CommonStepTestGridBit(pPlayerRecord, 14, 13))
	{
		if (nNpcClass == 176)
		{
			if (!D2CommonStepTestGridBit(pPlayerRecord, 14, 6))
				return TRUE;
		}
		else if (nNpcClass == 175)
		{
			if (!D2CommonStepTestGridBit(pPlayerRecord, 14, 7))
				return TRUE;
		}
		else if (nNpcClass == 177)
		{
			if (!D2CommonStepTestGridBit(pPlayerRecord, 14, 8))
				return TRUE;
		}
		else if (nNpcClass == 202)
		{
			if (!D2CommonStepTestGridBit(pPlayerRecord, 14, 9))
				return TRUE;
		}
		else if (nNpcClass == 244)
		{
			if (!D2CommonStepTestGridBit(pPlayerRecord, 14, 10))
				return TRUE;
		}
		else if (nNpcClass == 178)
		{
			if (!D2CommonStepTestGridBit(pPlayerRecord, 14, 11))
				return TRUE;
		}
	}
	return FALSE;
}

void __fastcall A2Q6Init(QUEST *pQuest  )
{
	A2Q6RECORD *pQuestRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A2Q6ChangedLevel;
	pQuest->pCallback[0] = A2Q6Talk;
	pQuest->pCallback[11] = A2Q6NpcMessage;
	pQuest->pCallback[10] = A2Q5PlayerDone;
	pQuest->pCallback[2] = A2Q6Activate;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A2Q6PlayerStarted;
	pQuest->pCallback[8] = A2Q6UnitKilled;
	pQuest->ptNpcTables = sgtA2Q6Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->bState = 0;
#line 1066
	pQuestRecord = (A2Q6RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A2Q6RECORD));
	pQuest->pQuestRecord = pQuestRecord;
	pQuest->eFilter = 14;
	pQuest->pStatusFilter = A2Q6StatusFilter;
	pQuest->bDoneStateMin = 4;
	pQuest->pActiveFilter = A2Q6Active;
	pQuest->pSequence = A2Q6Sequence;
	for (i = 0; i < (int)(sizeof(A2Q6RECORD) / sizeof(DWORD)); i++)
		((DWORD *)pQuestRecord)[i] = 0;
	pQuestRecord->bDurielKilled = 0;
	pQuestRecord->field_02 = 0;
	pQuestRecord->bPortalTimerPending = 0;
	pQuestRecord->dwTyraelDoorMode = 0;
	pQuestRecord->field_1c = 0;
	pQuestRecord->dwDrawnLevelId = 0;
}

void __fastcall A2Q6Talk(QUEST *pQuest  , QUESTPARAM *pqParam
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
	if (nNpcClass == 251)
	{
		if (((A2Q6RECORD *)pQuest->pQuestRecord)->dwTyraelDoorMode == 2)
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 2);
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 14, 13))
	{
		if (nNpcClass == 176)
		{
			if (!D2CommonStepTestGridBit(pRecord, 14, 6))
			{
				QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 6);
				return;
			}
		}
		else if (nNpcClass == 175)
		{
			if (!D2CommonStepTestGridBit(pRecord, 14, 7))
			{
				QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 6);
				return;
			}
		}
		else if (nNpcClass == 177)
		{
			if (!D2CommonStepTestGridBit(pRecord, 14, 8))
			{
				QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 6);
				return;
			}
		}
		else if (nNpcClass == 202)
		{
			if (!D2CommonStepTestGridBit(pRecord, 14, 9))
			{
				QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 6);
				return;
			}
		}
		else if (nNpcClass == 244)
		{
			if (!D2CommonStepTestGridBit(pRecord, 14, 10))
			{
				QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 6);
				return;
			}
		}
		else if (nNpcClass == 178)
		{
			if (!D2CommonStepTestGridBit(pRecord, 14, 11))
			{
				QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 6);
				return;
			}
		}
	}
	if (D2CommonStepTestGridBit(pRecord, 14, 3))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 3);
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 14, 4))
	{
		if (nNpcClass == 210)
		{
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 5);
			return;
		}
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 4);
		return;
	}
	if (QuestsHasPlayer(pQuest, pqParam->hPlayer->dwId))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 4);
		return;
	}
	if (!pQuest->bState)
		return;
	dwDone = D2CommonStepTestGridBit(pRecord, 14, 0);
	dwBit13 = D2CommonStepTestGridBit(pRecord, 14, 13);
	if (dwDone == 1 && !dwBit13)
		return;
	if (pQuest->bState >= 4 && !dwBit13)
		return;
	nTable = sgnA2Q6NpcTable[pQuest->bState];
	if (sgnA2Q6NpcTable[pQuest->bState] == 1 && nNpcClass == 177)
	{
		if (QuestsGetRecordBit(pQuest->ptGame, 12, 13) == (DWORD)nTable)
			return;
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, nTable);
		return;
	}
	if (nTable == -1)
		return;
	if ((DWORD)nTable >= 8)
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, nTable);
}

void __fastcall A2Q6Activate(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A2Q6RECORD *pQuestRecord;
	WORD wNpcClass;

	wNpcClass = (WORD)pqParam->hEventUnit->dwClassId;
	pQuestRecord = (A2Q6RECORD *)pQuest->pQuestRecord;
	if (wNpcClass == 251)
	{
		if (pQuestRecord->bStatus4Pending != 1)
			return;
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 4, NULL, A2Q6SendStatus, TRUE);
		pQuestRecord->bStatus4Pending = 0;
		pQuest->pCallback[2] = NULL;
		return;
	}
	if (wNpcClass != 201)
		return;
	if (pQuestRecord->bStatus1Pending == 1)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 1, NULL, A2Q6SendStatus, TRUE);
		pQuestRecord->bStatus1Pending = 0;
	}
	else if (pQuestRecord->bStatus6Pending == 1)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 6, NULL, A2Q6SendStatus, TRUE);
		pQuestRecord->bStatus6Pending = 0;
	}
	else
		return;
	pQuest->pCallback[2] = NULL;
	SUnitIterate(pQuest->ptGame, 0, NULL, A2Q6UpdatePlayer);
}

BOOL __fastcall A2Q6SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 14, 0) || D2CommonStepTestGridBit(pRecord, 14, 15))
		&& !D2CommonStepTestGridBit(pRecord, 14, 13) && !D2CommonStepTestGridBit(pRecord, 14, 14))
		return FALSE;
	QuestsSendStatus(ptGame, hUnit, 13);
	return FALSE;
}

BOOL __fastcall A2Q6UpdatePlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	QUEST *pQuest;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 14, 0) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 13);
	if (pQuest && pQuest->bState >= 2)
		D2CommonQuestRecordSetBit(pRecord, 14, 2);
	return FALSE;
}

void __fastcall A2Q6NpcMessage(QUEST *pQuestInfo  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	void *pRecord;
	A2Q6RECORD *pQuestRecord;
	WORD wNpcClass;
	WORD wMessage;
	int nX;
	int nY;
	GAMEROOM *hRoom;
	QUEST *pNextQuest;

	ptGame = pqParam->ptGame;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	pQuestRecord = (A2Q6RECORD *)pQuestInfo->pQuestRecord;
	wNpcClass = (WORD)pqParam->field_14;
	wMessage = (WORD)pqParam->field_18;
	if (wNpcClass == 251)
	{
		if (pQuestInfo->bActive && wMessage == 302 && !pQuestRecord->bTownPortalOpened)
		{
			pQuestRecord->bPortalSpotWanted = 1;
			if (SkillsOpenPortal(pQuestInfo->ptGame, pqParam->hPlayer, ((GAMEROOM *)D2CommonUnitGetPathRoom(pqParam->hPlayer)), ((int)D2CommonUnitGetX(pqParam->hPlayer)), ((int)D2CommonUnitGetY(pqParam->hPlayer)), LEVEL_ACT_2_TOWN, 0,
				59, 0))
			{
#line 576
				QuestsSetState(pQuestInfo, 4, D2_THIS_FILE, __LINE__);
				SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A2Q6CreditInLevel);
				SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A2Q6CreditParty);
				SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A2Q6FlagPlayer);
				pQuestRecord->bTownPortalOpened = 1;
				pQuestRecord->bStatus4Pending = 1;
				pQuestInfo->pCallback[2] = A2Q6Activate;
			}
			pQuestRecord->bPortalSpotWanted = 0;
		}
	}
	else if (wNpcClass == 201)
	{
		if (wMessage == 430)
		{
			QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
#line 595
			QuestsSetState(pQuestInfo, 2, D2_THIS_FILE, __LINE__);
			pNextQuest = QuestsFind(pQuestInfo->ptGame, 10);
			pQuestRecord->bStatus1Pending = 1;
			pQuestInfo->pCallback[2] = A2Q6Activate;
			if (!pNextQuest)
				return;
#line 601
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
			pNextQuest->pSequence(pNextQuest);
			return;
		}
		if (wMessage != 442)
			return;
		if (!D2CommonStepTestGridBit(pRecord, 14, 3))
			return;
		if (D2CommonStepTestGridBit(pRecord, 14, 13))
		{
#line 619
			QuestsSetState(pQuestInfo, 5, D2_THIS_FILE, __LINE__);
		}
		pQuestRecord->bStatus6Pending = 1;
		pQuestInfo->pCallback[2] = A2Q6Activate;
		D2CommonQuestRecordSetBit(pRecord, 14, 4);
		D2CommonQuestRecordClearBit(pRecord, 14, 3);
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		return;
	}
	else if (wNpcClass == 210)
	{
		if (wMessage != 450)
			return;
		if (!D2CommonStepTestGridBit(pRecord, 14, 4))
			return;
		if (D2CommonStepTestGridBit(pRecord, 10, 0))
		{
			QuestsRemoveItem(pqParam->ptGame, pqParam->hPlayer, A2Q2_CODE_STAFF);
			QuestsRemoveItem(pqParam->ptGame, pqParam->hPlayer, A2Q2_CODE_AMULET);
			QuestsRemoveItem(pqParam->ptGame, pqParam->hPlayer, A2Q2_CODE_SHAFT);
		}
		if (D2CommonStepTestGridBit(pRecord, 14, 13))
		{
			QuestsSetRecordBit(pQuestInfo->ptGame, 14, 13);
#line 643
			QuestsSetState(pQuestInfo, 5, D2_THIS_FILE, __LINE__);
		}
		pQuestInfo->bStatusFlags = 0;
		QuestsSetStatus(pQuestInfo, 13, NULL, A2Q6SendStatus, TRUE);
		D2CommonQuestRecordSetBit(pRecord, 14, 0);
		QuestsSendPlayerRecord(pQuestInfo->ptGame, pqParam->hPlayer);
		D2CommonQuestRecordClearBit(pRecord, 14, 4);
		QuestsPlayersAdd(&pQuestInfo->tPlayers, pqParam->hPlayer->dwId);
		return;
	}
	switch (wMessage)
	{
	case 445:
		D2CommonQuestRecordSetBit(pRecord, 14, 6);
		break;
	case 446:
		D2CommonQuestRecordSetBit(pRecord, 14, 7);
		break;
	case 449:
		D2CommonQuestRecordSetBit(pRecord, 14, 8);
		break;
	case 444:
		D2CommonQuestRecordSetBit(pRecord, 14, 9);
		break;
	case 452:
		D2CommonQuestRecordSetBit(pRecord, 14, 10);
		break;
	case 447:
		D2CommonQuestRecordSetBit(pRecord, 14, 11);
		break;
	}
}

BOOL __fastcall A2Q6CreditInLevel(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	QUEST *pQuestInfo;
	BYTE bInLevel;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	pQuestInfo = QuestsFind(ptGame, 13);
#line 400
	D2ASSERT(pQuestInfo != NULL);
	if (!pQuestInfo)
		return FALSE;
	bInLevel = FALSE;
	if (D2CommonDungeonRoomGetLevelId(hRoom) == 73)
		bInLevel = TRUE;
	if (D2CommonStepTestGridBit(pRecord, 14, 13))
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 14, 3))
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 14, 4))
		return FALSE;
	if (!bInLevel)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 14, 13);
	D2CommonQuestRecordSetBit(pRecord, 14, 3);
	ClientSetStep(SUnitGetClient(hUnit), 2, ptGame->bDifficultyLevel);
	return FALSE;
}
#line 417

BOOL __fastcall A2Q6CreditParty(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	WORD wParty;

	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecordX, 14, 13))
	{
		wParty = SUnitGetParty(hUnit);
		if (wParty != 65535)
			PartyForEach(ptGame, wParty, A2Q6PartyCredit, NULL);
	}
	return FALSE;
}

BOOL __fastcall A2Q6FlagPlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 14, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 14, 3) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 14, 4) == 1)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 14, 14);
	QuestsSendFlagInAct(hUnit, 13, 0);
	return FALSE;
}

void __fastcall A2Q6UnitKilled(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A2Q6RECORD *pQuestRecord;
	UNIT *hObject;
	MISSOBJECTSTXT *ptObjectTxt;
	GAME *ptGame;
	void *pRecord;

#line 772
	QuestsDebugMessage(pQuest->ptGame, "killed duriel for quest", D2_THIS_FILE, __LINE__);
	if (pQuest->bActive)
	{
#line 775
		QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
		QuestsAddTimer(pQuest, A2Q6Timer, 8);
		if (pqParam->hPlayer)
		{
			ptGame = pQuest->ptGame;
			pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
			if (!D2CommonStepTestGridBit(pRecord, 14, 0))
				QuestsDropItem(pQuest, pqParam, 61);
		}
	}
	pQuestRecord = (A2Q6RECORD *)pQuest->pQuestRecord;
	pQuest->pCallback[2] = NULL;
	pQuest->pCallback[8] = NULL;
	pQuestRecord->bDurielKilled = 1;
	pQuestRecord->hDurielRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(pqParam->hEventUnit);
	QuestsSetAll(pQuest->ptGame, 8);
	SUnitIterate(pQuest->ptGame, 0, NULL, A2Q6KilledInLevel);
	if (pQuestRecord->bTyraelDoorPresent)
	{
		hObject = SUnitFind(pQuest->ptGame, UNIT_OBJECT, pQuestRecord->dwTyraelDoorId);
		if (hObject)
		{
			D2CommonUnitSetMode(hObject, 1);
			ptObjectTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hObject->dwClassId);
			EventAdd(pQuest->ptGame, hObject, 1,
				(int)(((DWORD)ptObjectTxt->FrameCnt[1] >> 8) + pQuest->ptGame->nFrame));
		}
	}
	pQuestRecord->dwTyraelDoorMode = 2;
}

BYTE __fastcall A2Q6Timer(GAME *ptGame  , QUEST *pQuest
)
{
	A2Q6RECORD *pQuestRecord;

	pQuestRecord = (A2Q6RECORD *)pQuest->pQuestRecord;
	if (pQuest->bStatus != 5 && pQuest->bStatus != 4 && pQuest->bStatus != 3)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 3, NULL, A2Q6SendStatus, TRUE);
	}
	pQuestRecord->field_00 = 0;
	return TRUE;
}

BOOL __fastcall A2Q6KilledInLevel(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	WORD wParty;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	if (D2CommonDungeonRoomGetLevelId(hRoom) != 73)
		return FALSE;
	if ((D2CommonStepTestGridBit(pRecord, 14, 0) || D2CommonStepTestGridBit(pRecord, 14, 3))
		&& D2CommonStepTestGridBit(pRecord, 14, 4) && D2CommonStepTestGridBit(pRecord, 14, 5))
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 14, 5);
	wParty = SUnitGetParty(hUnit);
	if (wParty != 65535)
		PartyForEach(ptGame, wParty, A2Q6PartyFlag, NULL);
	return FALSE;
}

void __fastcall A2Q6ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	int nLevel;
	A2Q6RECORD *pQuestRecord;
	DWORD dwLevel;

	nLevel = (int)pqParam->field_18;
	if (nLevel < 40 || nLevel >= 75)
		return;
	if (pqParam->field_14 == 40)
		QuestsPlayerLeft(pQuest, pqParam);
	if (!pQuest->bActive)
		return;
	pQuestRecord = (A2Q6RECORD *)pQuest->pQuestRecord;
	if (!pQuestRecord->bDurielKilled)
	{
		if (!pQuestRecord->dwDrawnLevelId)
			pQuestRecord->dwDrawnLevelId = D2CommonDungeonActGetDrawnLevel(pqParam->ptGame->ptAct[1]);
		dwLevel = pQuestRecord->dwDrawnLevelId;
		if (!dwLevel)
			return;
		if ((DWORD)nLevel == dwLevel)
		{
			if (pQuest->bState == 2)
				return;
#line 870
			QuestsSetState(pQuest, 2, D2_THIS_FILE, __LINE__);
			if (pQuest->bStatus <= 1)
			{
				pQuest->bStatusFlags = 0;
				QuestsSetStatus(pQuest, 2, NULL, A2Q6SendStatus, FALSE);
			}
			A2Q6UpdatePlayer(pqParam->ptGame, pqParam->hPlayer, NULL);
			return;
		}
		if (nLevel == 46)
		{
			if (!pQuest->bState)
			{
#line 886
				QuestsSetState(pQuest, 2, D2_THIS_FILE, __LINE__);
				A2Q6UpdatePlayer(pqParam->ptGame, pqParam->hPlayer, NULL);
			}
			if (!pQuest->bStatus)
			{
				pQuest->bStatusFlags = 0;
				QuestsSetStatus(pQuest, 1, NULL, A2Q6SendStatus, TRUE);
			}
			return;
		}
	}
	if (nLevel != 73)
		return;
	if (pQuest->bState > 1)
		return;
#line 901
	QuestsSetState(pQuest, 2, D2_THIS_FILE, __LINE__);
	if (pQuest->bStatus > 1)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 2, NULL, A2Q6SendStatus, FALSE);
	A2Q6UpdatePlayer(pqParam->ptGame, pqParam->hPlayer, NULL);
}

BYTE __fastcall A2Q6Sequence(QUEST *pQuestInfo  )
{
	QUEST *pNextQuest;

	if (!pQuestInfo->bState && pQuestInfo->bActive == 1)
	{
#line 990
		QuestsSetState(pQuestInfo, 1, D2_THIS_FILE, __LINE__);
	}
	pNextQuest = QuestsFind(pQuestInfo->ptGame, 10);
	if (!(!pNextQuest))
	{
#line 995
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		return pNextQuest->pSequence(pNextQuest);
	}
	return TRUE;
}

void __fastcall A2Q6PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 10, 0))
	{
		((A2Q6RECORD *)pQuest->pQuestRecord)->bQuest10Finished = 1;
		((A2Q6RECORD *)pQuest->pQuestRecord)->bOpenPending = 1;
	}
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0)
		|| D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 15))
	{
		((A2Q6RECORD *)pQuest->pQuestRecord)->bFinishedPlayerSeen = 1;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 14, 2) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 2;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 14, 3) == 1)
	{
		pQuest->bStatus = 5;
		pQuest->bState = 5;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 14, 4) == 1)
	{
		pQuest->bStatus = 6;
		pQuest->bState = 5;
	}
}

void __fastcall A2Q6SendLevel(GAME *ptGame  , UNIT *hUnit
)
{
	QUEST *pQuest;
	CLIENT *ptClient;
	DWORD dwLevel;
	A2Q6LEVELCMD Cmd;

	pQuest = QuestsFind(ptGame, 13);
	Cmd.bCmd = 73;
	Cmd.wQuest = 13;
	ptClient = SUnitGetClient(hUnit);
	dwLevel = D2CommonDungeonActGetDrawnLevel(ptGame->ptAct[1]);
	if (!dwLevel)
		Cmd.wOrder = 0;
	else
	{
		Cmd.wOrder = (WORD)(dwLevel - 66);
		((A2Q6RECORD *)pQuest->pQuestRecord)->dwDrawnLevelId = dwLevel;
	}
	SCmdSendQuestCommand(ptClient, (const struct SCMDCOPY15 *)&Cmd);
}

DWORD __fastcall A2Q6NextClass(QUEST *pQuest  )
{
	DWORD dwLevel;
	A2Q6RECORD *pQuestRecord;
	int nSkip;
	int i;
	int j;
	int n;

	dwLevel = D2CommonDungeonActGetDrawnLevel(pQuest->ptGame->ptAct[1]);
	if (dwLevel)
	{
		pQuestRecord = (A2Q6RECORD *)pQuest->pQuestRecord;
		nSkip = (int)(dwLevel - 66);
		if (!pQuestRecord->bClassesFilled)
		{
			j = 0;
			pQuestRecord->bClassesFilled = 1;
			i = 0;
			for (n = 0; n < 7; n++)
			{
				if (i == nSkip)
					i++;
				else if (i < 7 && j < 6)
					pQuestRecord->adwClasses[j++] = sgadwA2Q6Classes[i++];
			}
		}
		if (pQuestRecord->wClassIndex == 6)
			pQuestRecord->wClassIndex = 0;
		return pQuestRecord->adwClasses[pQuestRecord->wClassIndex++];
	}
	return 307;
}

DWORD __fastcall A2Q6GetClass(GAME *ptGame  )
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptGame, 13);
	if (pQuest) {
		return A2Q6NextClass(pQuest); }
	return 307;
}

void __fastcall A2Q6OperateTiles(OBJOPERATE *pObjectParam  )
{
	UNIT *hUnit;

	for (hUnit = ((GAMEROOM *)D2CommonUnitGetPathRoom(pObjectParam->hObject))->ptUnitFirst; hUnit;
		hUnit = hUnit->ptRoomNext)
	{
		if (hUnit->eType == UNIT_ROOMTILE)
			SUnitWarp(pObjectParam->ptGame, pObjectParam->hPlayer, hUnit);
	}
}

void __fastcall A2Q6InitRecordedObject(OBJINIT *ptInit)
{
	QUEST *pQuest;
	A2Q6RECORD *pQuestRecord;

	pQuest = QuestsFind(ptInit->ptGame, 13);
	if (pQuest)
	{
		pQuestRecord = (A2Q6RECORD *)pQuest->pQuestRecord;
		pQuestRecord->field_06 = 1;
		pQuestRecord->field_28 = ptInit->hObject->dwId;
		if (!pQuest->bActive)
		{
			D2CommonUnitSetMode(ptInit->hObject, 2);
			return;
		}
		if (pQuestRecord->bObjectsFound == 1)
		{
			D2CommonUnitSetMode(ptInit->hObject, 2);
			return;
		}
		if (pQuestRecord->bOpenPending == 1)
			D2CommonUnitSetMode(ptInit->hObject, 2);
		else
			D2CommonUnitSetMode(ptInit->hObject, 0);
	}
	else
		D2CommonUnitSetMode(ptInit->hObject, 2);
}

void __fastcall A2Q6InitTyraelDoor(OBJINIT *ptInit)
{
	QUEST *pQuest;
	A2Q6RECORD *pQuestRecord;

	pQuest = QuestsFind(ptInit->ptGame, 13);
	if (!pQuest)
		return;
	pQuestRecord = (A2Q6RECORD *)pQuest->pQuestRecord;
	pQuestRecord->bTyraelDoorPresent = 1;
	pQuestRecord->dwTyraelDoorId = ptInit->hObject->dwId;
	if (!pQuest->bActive)
		D2CommonUnitSetMode(ptInit->hObject, 2);
	else
		D2CommonUnitSetMode(ptInit->hObject, pQuestRecord->dwTyraelDoorMode);
}

BOOL __fastcall A2Q6TestLevel(GAME *ptGame  , UNIT *hUnit
, DWORD dwLevelId)
{
	QUEST *pQuest;

	switch (dwLevelId)
	{
	case 100:
		return A3Q5TestLevel(ptGame, hUnit);
	case 73:
		pQuest = QuestsFind(ptGame, 13);
		if (pQuest && pQuest->bActive)
			return !((A2Q6RECORD *)pQuest->pQuestRecord)->bObjectsFound;
		return FALSE;
	}
	return FALSE;
}

void __fastcall A2Q6InitOrifice(OBJINIT *ptInit)
{
	QUEST *pQuest;
	A2Q6RECORD *pQuestRecord;

	pQuest = QuestsFind(ptInit->ptGame, 13);
	if (!pQuest)
		return;
	pQuestRecord = (A2Q6RECORD *)pQuest->pQuestRecord;
	pQuestRecord->bOrificePresent = 1;
	pQuestRecord->dwOrificeId = ptInit->hObject->dwId;
	if (pQuestRecord->bQuest10Finished && !pQuestRecord->bPortalTimerPending && !pQuestRecord->bObjectsFound)
	{
		QuestsAddTimer(pQuest, A2Q6ObjectTimer, 1);
		pQuestRecord->bPortalTimerPending = 1;
		D2CommonUnitSetMode(ptInit->hObject, 2);
		return;
	}
	if (pQuestRecord->bObjectsFound)
		D2CommonUnitSetMode(ptInit->hObject, 2);
}

BYTE __fastcall A2Q6ObjectTimer(GAME *ptGame  , QUEST *pQuest
)
{
	int nCount;
	A2Q6RECORD *pQuestRecord;
	UNIT *hObject;
	UNIT *hCreated;
	MISSOBJECTSTXT *ptObjectTxt;
	int nX;
	int nY;
	GAMEROOM *hRoom;
	GAMEROOM *hObjectRoom;

	nCount = 0;
	pQuestRecord = (A2Q6RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bOpenPending == 1)
	{
		if (pQuestRecord->field_06)
		{
			hObject = SUnitFind(ptGame, UNIT_OBJECT, pQuestRecord->field_28);
			if (hObject)
			{
				nCount = 1;
				if (!hObject->dwMode)
				{
					if (pQuestRecord->bQuest10Finished)
						D2CommonUnitSetMode(hObject, 2);
					else
					{
						D2CommonUnitSetMode(hObject, 1);
						ptObjectTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hObject->dwClassId);
						EventAdd(pQuest->ptGame, hObject, 1,
							(int)(((DWORD)ptObjectTxt->FrameCnt[1] >> 8) + pQuest->ptGame->nFrame));
					}
				}
			}
		}
		if (pQuestRecord->bOrificePresent)
		{
			hObject = SUnitFind(ptGame, UNIT_OBJECT, pQuestRecord->dwOrificeId);
			if (hObject)
			{
				nX = (int)D2CommonUnitGetX(hObject) - 13;
				nY = (int)D2CommonUnitGetY(hObject) + 3;

				hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(((GAMEROOM *)D2CommonUnitGetPathRoom(hObject)), nX, nY);
				if (hRoom)
				{
					hCreated = SUnitCreate(UNIT_OBJECT, 100, nX, nY, ptGame, hRoom, 1, 1, 0);
					if (hCreated)
					{
						nCount++;
						if (pQuestRecord->bQuest10Finished)
							D2CommonUnitSetMode(hCreated, 2);
						else
						{
							D2CommonUnitSetMode(hCreated, 1);
							D2CommonDungeonRoomSetHold(D2CommonUnitGetPathRoom(hObject), 1);
							D2CommonDungeonRoomSetHold(hRoom, 1);
						}
					}
				}
			}
		}
		if (!nCount)
			return FALSE;
		pQuestRecord->bObjectsFound = 1;
		pQuestRecord->bOpenPending = 0;
	}
	pQuestRecord->bPortalTimerPending = 0;
	return TRUE;
}

void __fastcall A2Q6PartyDone(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 10, 0) == 1)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	if (D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom)) != 1)
		return;
	D2CommonQuestRecordSetBit(pRecord, 10, 0);
	D2CommonQuestRecordSetBit(pRecord, 10, 13);
	if (PlrTradeIsTrading(ptGame, hUnit))
		return;
	QuestsRemoveItem(ptGame, hUnit, A2Q2_CODE_STAFF);
	QuestsRemoveItem(ptGame, hUnit, A2Q2_CODE_AMULET);
	QuestsRemoveItem(ptGame, hUnit, A2Q2_CODE_SHAFT);
}

int __fastcall A2Q6OperateObject(OBJOPERATE *pObjectParam  )
{
	UNIT *hTrade;
	CLIENT *ptClient;
	BYTE abPacket[7];

	if (!QuestsFind(pObjectParam->ptGame, 13))
		return 0;
	hTrade = SUnitGetInteractUnit(pObjectParam->ptGame, pObjectParam->hPlayer);
	ptClient = SUnitGetClient(pObjectParam->hPlayer);
	switch (pObjectParam->hObject->dwMode)
	{
	case 0:
		if (PlayerIsBusy(pObjectParam->hPlayer) == 1)
			return 1;
		if (!ITEMSFindByType(pObjectParam->hPlayer, A2Q2_CODE_STAFF))
		{
			SUnitSetEvent(pObjectParam->hPlayer, 18, pObjectParam->hPlayer);
			return 1;
		}
		SUnitSetInteract(pObjectParam->hPlayer, 2, pObjectParam->hObject->dwId);
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		abPacket[0] = 81;
		*(DWORD *)&abPacket[1] = pObjectParam->hObject->dwId;
		abPacket[5] = 0;
		SCmdSendUseItemState(ptClient, (const struct SCMDCOPY7 *)abPacket);
		return 0;
	case 1:
		if (hTrade != pObjectParam->hObject)
			return 0;
		SUnitResetInteract(pObjectParam->hPlayer);
		D2CommonUnitSetMode(pObjectParam->hObject, 2);
		return 0;
	}
	return 0;
}

void __fastcall A2Q6ObjectUsed(GAME *ptGame  , UNIT *hPlayer,
	UNIT *hObject)
{
	WORD wParty;
	MISSILESTXT *ptMissile;
	int nInterval;
	QUEST *pQuest;
	A2Q6RECORD *pQuestRecord;
	GAMEROOM *hRoomObject;
	GAMEROOM *hRoomPortal;
	int nX;
	int nY;
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	D2CommonQuestRecordSetBit(pRecord, 10, 0);
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	D2CommonQuestRecordSetBit(pRecord, 10, 13);
	QuestsRemoveItem(ptGame, hPlayer, A2Q2_CODE_STAFF);
	QuestsRemoveItem(ptGame, hPlayer, A2Q2_CODE_AMULET);
	QuestsRemoveItem(ptGame, hPlayer, A2Q2_CODE_SHAFT);
	wParty = SUnitGetParty(hPlayer);
	if (wParty != 65535)
		PartyForEach(ptGame, wParty, A2Q6PartyDone, NULL);
	QuestsSetAll(ptGame, 3);
	ptMissile = (MISSILESTXT *)D2CommonMissileTblsGetMissile(MISSILE_HORADRICSTAFF);
	nInterval = (ptMissile->Range - 75) / 20;
	pQuest = QuestsFind(ptGame, 13);
	pQuestRecord = (A2Q6RECORD *)pQuest->pQuestRecord;
	pQuestRecord->bOpenPending = 1;
	pQuestRecord->bStaffPlaced = 1;
	if (!pQuestRecord->bPortalTimerPending)
	{
		QuestsAddTimer(pQuest, A2Q6ObjectTimer, (DWORD)nInterval);
		pQuestRecord->bPortalTimerPending = 1;
	}
	D2CommonUnitSetMode(hObject, 1);
	hRoomObject = (GAMEROOM *)D2CommonUnitGetPathRoom(hObject);
#line 1662
	D2ASSERT(hRoomObject);
	nY = (int)D2CommonUnitGetY(hObject) + 3;

	hRoomPortal = (GAMEROOM *)D2CommonDungeonFindRoomGame(hRoomObject, ((int)D2CommonUnitGetX(hObject) - 13), nY);
#line 1666
	D2ASSERT(hRoomPortal);
	D2CommonDungeonRoomSetHold(hRoomObject, 0);
	D2CommonDungeonRoomSetHold(hRoomPortal, 0);
}

BOOL __fastcall A2Q6MonsterTest(GAME *ptGame  , UNIT *hUnit
)
{
	QUEST *pQuest;
	A2Q6RECORD *pQuestRecord;

	pQuest = QuestsFind(ptGame, 13);
	if (pQuest) {
		pQuestRecord = (A2Q6RECORD *)pQuest->pQuestRecord;
		if (pQuestRecord->bFinishedPlayerSeen)
			return TRUE;
		if (!pQuestRecord->bTownPortalOpened)
			return FALSE;
		pQuestRecord->bPlayerNear = 0;
		pQuestRecord->hTyrael = hUnit;
		SUnitForEachPlayer(ptGame, A2Q6NearPlayer, pQuestRecord);
		return !pQuestRecord->bPlayerNear; }
	return FALSE;
}

void __fastcall A2Q6NearPlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	if (D2CommonUnitGetProximityMetric(hUnit, ((A2Q6RECORD *)pArg)->hTyrael) < 12)
		((A2Q6RECORD *)pArg)->bPlayerNear = 1;
}

BOOL __fastcall A2Q6PortalSpot(GAME *ptGame  , GAMECOORD *ptCoord,
	GAMEROOM **phRoom)
{
	QUEST *pQuest;
	GAMEROOM *hRoom;
	DWORD dwX;
	DWORD dwY;
	DWORD dwRoom;

	pQuest = QuestsFind(ptGame, 13);
	if (pQuest) {
		if (((A2Q6RECORD *)pQuest->pQuestRecord)->bPortalSpotWanted != 1)
			return FALSE;
		hRoom = (GAMEROOM *)D2CommonDungeonGetActStartRoom(ptGame->ptAct[1], LEVEL_ACT_2_TOWN, 12, &dwX, &dwY, 3);
		if (!hRoom)
			return FALSE;
		GameRespawnRoom(ptGame, hRoom);
		dwRoom = (DWORD)D2CommonCollisnFindSpawnPointStep(hRoom, ptCoord, 3, 40465, 7);
		if (!dwRoom)
			return FALSE;
		ptCoord->dwX = dwX;
		ptCoord->dwY = dwY;
		*phRoom = (GAMEROOM *)dwRoom;
		return TRUE; }
	return FALSE;
}
