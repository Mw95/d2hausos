#include "../D2game.h"
#include "../AI/AiThink.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a2q4.cpp"
#include "D2Macros.h"
#include "../AI/AiTatics.h"
#include "../GAME/Event.h"
#include "../GAME/Game.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/MonsterAI.h"
#include "../MONSTER/MonsterUnique.h"
#include "QUESTS.h"
#include "a2q4.h"
#include "a2q5.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"
#include "../SKILLS/Skills.h"

#include "../AI/AiUtil.h"

#define A2Q2_CODE_STAFF		544502632
#define A2Q2_CODE_BOX		544763746
#define A2Q2_CODE_SCROLL	540111476
#define A2Q2_CODE_AMULET	544237942
#define A2Q2_CODE_SHAFT		543585133

static QUESTNPCTABLE sgtA2Q4Npcs[12] =
{
	{ { { 177, 373, { 0, 0 }, 1 } }, 1 },
	{ { { 177, 373, { 0, 0 }, 2 }, { 201, 377, { 0, 0 }, 1 } }, 2 },
	{ { { 176, 380, { 0, 0 }, 2 }, { 175, 381, { 0, 0 }, 2 }, { 198, 375, { 0, 0 }, 2 },
		{ 199, 376, { 0, 0 }, 2 }, { 177, 383, { 0, 0 }, 2 }, { 202, 382, { 0, 0 }, 2 },
		{ 244, 378, { 0, 0 }, 2 }, { 201, 377, { 0, 0 }, 2 }, { 210, 384, { 0, 0 }, 2 },
		{ 200, 379, { 0, 0 }, 2 }, { 178, 374, { 0, 0 }, 2 } }, 11 },
	{ { { 176, 393, { 0, 0 }, 2 }, { 175, 394, { 0, 0 }, 2 }, { 198, 387, { 0, 0 }, 2 },
		{ 199, 385, { 0, 0 }, 2 }, { 177, 388, { 0, 0 }, 2 }, { 201, 386, { 0, 0 }, 2 },
		{ 202, 389, { 0, 0 }, 2 }, { 244, 395, { 0, 0 }, 2 }, { 210, 392, { 0, 0 }, 2 },
		{ 200, 391, { 0, 0 }, 2 }, { 178, 390, { 0, 0 }, 2 } }, 11 },
	{ { { 176, 406, { 0, 0 }, 1 }, { 175, 403, { 0, 0 }, 1 }, { 198, 397, { 0, 0 }, 1 },
		{ 199, 400, { 0, 0 }, 1 }, { 201, 398, { 0, 0 }, 1 }, { 177, 399, { 0, 0 }, 1 },
		{ 202, 405, { 0, 0 }, 1 }, { 244, 407, { 0, 0 }, 1 }, { 210, 402, { 0, 0 }, 1 },
		{ 200, 401, { 0, 0 }, 1 }, { 178, 404, { 0, 0 }, 1 } }, 11 },
	{ { { 175, 403, { 0, 0 }, 2 }, { 198, 397, { 0, 0 }, 2 }, { 199, 400, { 0, 0 }, 2 },
		{ 201, 398, { 0, 0 }, 2 }, { 177, 399, { 0, 0 }, 2 }, { 202, 405, { 0, 0 }, 2 },
		{ 244, 407, { 0, 0 }, 2 }, { 210, 402, { 0, 0 }, 2 }, { 200, 401, { 0, 0 }, 2 },
		{ 178, 404, { 0, 0 }, 2 } }, 10 },
	{ { { 331, 185, { 0, 0 }, 1 } }, 1 },
	{ { { 331, 186, { 0, 0 }, 1 } }, 1 },
	{ { { 331, 187, { 0, 0 }, 1 } }, 1 },
	{ { { 331, 188, { 0, 0 }, 1 } }, 1 },
	{ { { 331, 189, { 0, 0 }, 1 } }, 1 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static int sgnA2Q4NpcTable[8] = { -1, 0, 1, 2, 3, 4, 0, 0 };

BOOL __fastcall A2Q4SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q4UpdatePlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A2Q4PartyCredit(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A2Q4Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall A2Q4Activate(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q4NpcMessage(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q4Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q4ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q4SpawnMonster(QUEST *pQuest, UNIT *hObject, GAMEROOM *hRoom, GAMECOORD *ptCoord);
BYTE __fastcall A2Q4Sequence(QUEST *pQuestInfo);
void __fastcall A2Q4PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A2Q4FindNear(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q4FlagSeen(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q4CreditInLevel(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q4ReturnFalse(GAME *ptGame, UNIT *hUnit, void *pArg);

BYTE __fastcall A2Q4Active(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer, void *pPlayerRecord)
{
	A2Q4RECORD *pQuestRecord;

	if (D2CommonStepTestGridBit(pPlayerRecord, 12, 0) == 1)
		return FALSE;
	if (nNpcClass == 331)
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 12, 1) == 1)
			return FALSE;
		pQuestRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
		switch (pQuestRecord->wBlockerMode)
		{
		case 0:
			if (!D2CommonStepTestGridBit(pPlayerRecord, 12, 7))
				return TRUE;
			break;
		case 2:
			if (pQuestRecord->bBlockerMode0To2 && !D2CommonStepTestGridBit(pPlayerRecord, 12, 8))
				return TRUE;
			break;
		}
		return FALSE;
	}
	if (nNpcClass == 177)
	{
		if (pQuest->bState == 1 && !D2CommonStepTestGridBit(pPlayerRecord, 12, 1)
			&& !D2CommonStepTestGridBit(pPlayerRecord, 12, 0))
			return TRUE;
	}
	else if (nNpcClass == 201)
	{
		if (pQuest->bState == 2 && !D2CommonStepTestGridBit(pPlayerRecord, 12, 1)
			&& !D2CommonStepTestGridBit(pPlayerRecord, 12, 0))
			return TRUE;
	}
	return FALSE;
}

BOOL __fastcall A2Q4SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 12, 0) || D2CommonStepTestGridBit(pRecord, 12, 15))
		&& !D2CommonStepTestGridBit(pRecord, 12, 13) && !D2CommonStepTestGridBit(pRecord, 12, 14))
		return FALSE;
	QuestsSendStatus(ptGame, hUnit, 11);
	return FALSE;
}

BOOL __fastcall A2Q4UpdatePlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	QUEST *pQuest;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 12, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 12, 1) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 11);
	if (!pQuest)
		return FALSE;
	if (pQuest->bState == 2)
	{
		D2CommonQuestRecordSetBit(pRecord, 12, 2);
		return FALSE;
	}
	if (pQuest->bState == 3)
	{
		D2CommonQuestRecordSetBit(pRecord, 12, 3);
		return FALSE;
	}
	if (pQuest->bState == 4)
	{
		if (pQuest->bStatus == 2 || pQuest->bStatus == 3)
			D2CommonQuestRecordSetBit(pRecord, 12, 4);
		else if (pQuest->bStatus == 4)
			D2CommonQuestRecordSetBit(pRecord, 12, 5);
	}
	return FALSE;
}

BOOL __fastcall A2Q4ObjectOpen(GAME *ptGame  )
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptGame, 11);
	if (pQuest && ((A2Q4RECORD *)pQuest->pQuestRecord)->wBlockerMode == 2)
		return TRUE;
	return FALSE;
}

void __fastcall A2Q4PartyCredit(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	DWORD dwLevel;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 12, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, 12, 1) == 1)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	dwLevel = D2CommonDungeonRoomGetLevelId(hRoom);
	if (!dwLevel)
		return;
	if (D2CommonGetActNum(dwLevel) != 1)
		return;
	D2CommonQuestRecordSetBit(pRecord, 12, 13);
	D2CommonQuestRecordSetBit(pRecord, 12, 1);
	D2CommonQuestRecordSetBit(pRecord, 12, 0);
	D2CommonQuestRecordClearMidBits(pRecord, 12);
	D2CommonQuestRecordSetBit(pRecord, 12, 8);
	D2CommonQuestRecordSetBit(pRecord, 12, 7);
}

void __fastcall A2Q4Init(QUEST *pQuest  )
{
	A2Q4RECORD *pRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->ptNpcTables = sgtA2Q4Npcs;
	pQuest->pCallback[0] = A2Q4Talk;
	pQuest->pCallback[11] = A2Q4NpcMessage;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A2Q4ChangedLevel;
	pQuest->pCallback[10] = A2Q5PlayerDone;
	pQuest->pCallback[2] = A2Q4Activate;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A2Q4PlayerStarted;
	pQuest->bCallbacksEnabled = 1;
	pQuest->bState = 0;
#line 995
	pQuest->pQuestRecord = D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A2Q4RECORD));
	pQuest->eFilter = 12;
	pQuest->pStatusFilter = NULL;
	pQuest->bDoneStateMin = 5;
	pQuest->pActiveFilter = A2Q4Active;
	pQuest->pSequence = A2Q4Sequence;
	pQuest->dwNextQuestId = 13;
	QuestsPlayersClear(&pQuest->tPlayers);
	pRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
	memset(pRecord, 0, sizeof(A2Q4RECORD));
	pRecord->field_00 = 0;
	pRecord->field_01 = 0;
	pRecord->field_02 = 0;
	pRecord->field_03 = 0;
	pRecord->bStatus2Pending = 0;
}

void __fastcall A2Q4Activate(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A2Q4RECORD *pRecord;

	if ((WORD)pqParam->hEventUnit->dwClassId != 177)
		return;
	pRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
	if (pRecord->bStatus2Pending != 1)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 2, NULL, A2Q4SendStatus, TRUE);
	pRecord->bStatus2Pending = 0;
	pQuest->pCallback[2] = NULL;
	SUnitIterate(pQuest->ptGame, 0, NULL, A2Q4UpdatePlayer);
}

void __fastcall A2Q4NpcMessage(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	WORD wNpcClass;
	WORD wMessage;
	A2Q4RECORD *pQuestRecord;
	GAME *ptGame;
	void *pRecord;
	GAMECOORD tCoord;
	GAMEROOM *hRoom;
	UNIT *hObject;
	UNIT *hPlayer;

	wNpcClass = (WORD)pqParam->field_14;
	wMessage = (WORD)pqParam->field_18;
	if (!pQuest)
		return;
	pQuestRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
	ptGame = pqParam->ptGame;
	hPlayer = pqParam->hPlayer;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (wNpcClass == 331)
	{
		switch (wMessage)
		{
		case 186:
			D2CommonQuestRecordSetBit(pRecord, 12, 7);
			break;
		case 187:
		case 188:
		case 189:
			D2CommonQuestRecordSetBit(pRecord, 12, 8);
			break;
		}
		return;
	}
	if (wMessage == 406 || wMessage == 403 || wMessage == 397 || wMessage == 400 || wMessage == 398
		|| wMessage == 399 || wMessage == 405 || wMessage == 407 || wMessage == 402
		|| wMessage == 401
		|| wMessage == 404)
	{
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		if (D2CommonStepTestGridBit(pRecord, 12, 1))
		{
			D2CommonQuestRecordClearBit(pRecord, 12, 1);
			QuestsPlayersAdd(&pQuest->tPlayers, pqParam->hPlayer->dwId);
		}
	}
	if (wMessage == 396)
	{
		if (pQuest->bActive && pQuest->bStatus < 5)
		{
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 5, NULL, A2Q4SendStatus, FALSE);
		}
		if (!pQuestRecord->bPortalToLevel46Opened)
		{
			hRoom = NULL;
			D2CommonUnitGetPosition(pqParam->hPlayer, &tCoord);
			QuestsFindSpot((GAMEROOM *)D2CommonUnitGetPathRoom(pqParam->hPlayer), &tCoord, 2, 40465,
				&hRoom);
			if (hRoom && SkillsOpenPortal(pQuest->ptGame, pqParam->hPlayer, hRoom, (int)tCoord.dwX,
				(int)tCoord.dwY,
				LEVEL_ACT_2_VALLEY_OF_THE_KINGS, 0, 60, 0))
			{
				pQuestRecord->bPortalToLevel46Opened = 1;
				QuestsSetRecordBit(pqParam->ptGame, pQuest->eFilter, 13);
			}
		}
	}
	if (wNpcClass == 177)
	{
		if (wMessage != 373)
			return;
#line 479
		QuestsSetState(pQuest, 2, D2_THIS_FILE, __LINE__);
		pQuestRecord->bStatus2Pending = 1;
		pQuestRecord->bBlockerMode2Set = 1;
		if (!pQuestRecord->wBlockerMode)
			pQuestRecord->bBlockerMode0To2 = 1;
		pQuestRecord->wBlockerMode = 2;
		SUnitIterate(pQuest->ptGame, 0, NULL, A2Q4UpdatePlayer);
		if (pQuestRecord->bBlockerCreated == 1)
		{
			hObject = SUnitFind(pQuest->ptGame, 2, pQuestRecord->dwBlockerId);
			if (hObject)
			{
				D2CommonUnitSetMode(hObject, 2);
				D2CommonUnitClearCollision(hObject);
			}
		}
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		return;
	}
	if (wNpcClass == 201 && wMessage == 377)
	{
#line 487
		QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 3, NULL, A2Q4SendStatus, TRUE);
		SUnitIterate(pQuest->ptGame, 0, NULL, A2Q4UpdatePlayer);
	}
}

void __fastcall A2Q4Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	void *pRecord;
	int nNpcClass;
	D2SEED *ptSeed;
	int nTable;

	ptGame = pqParam->ptGame;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	nNpcClass = (int)pqParam->hEventUnit->dwClassId;
	if (nNpcClass == 331)
	{
		ptSeed = QuestsGetSeed(pQuest->ptGame);
		if (((A2Q4RECORD *)pQuest->pQuestRecord)->bBlockerMode2Set)
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass,
				(int)D2CommonSeedRand(ptSeed, 3) + 8);
		else
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, 331, 7);
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 12, 1))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 4);
		return;
	}
	if (QuestsHasPlayer(pQuest, pqParam->hPlayer->dwId))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 5);
		return;
	}
	if (!pQuest->bState)
		return;
	if (!pQuest->bActive)
		return;
	if (D2CommonStepTestGridBit(pRecord, 12, 0) == 1)
		return;
	nTable = sgnA2Q4NpcTable[pQuest->bState];
	if (nTable == -1)
		return;
	if ((DWORD)nTable >= 12)
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, nTable);
}

static __inline void *A2Q4RecordI(UNIT *hPlayer, GAME *ptGame)
{
	return ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
}

void __fastcall A2Q4ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	BYTE fState;
	A2Q4RECORD *pQuestRecord;
	UNIT *hMonster;
	UNIT *hObject;
	GAMECOORD tCoord;
	void *pRecord;
	GAME *ptGame;
	UNIT *hPlayer;
	int nLevel;
	int nOldLevel;

	nLevel = pqParam->field_18;
	nOldLevel = pqParam->field_14;
	if (nLevel == 74)
	{
		fState = FALSE;
		if (pQuest->bState < 4)
		{
			fState = TRUE;
#line 669
			QuestsSetState(pQuest, 4, D2_THIS_FILE, __LINE__);
		}
		if (pQuest->bStatus < 4)
		{
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 4, NULL, A2Q4SendStatus, TRUE);
		}
		else if (!fState)
			return;
		SUnitIterate(pQuest->ptGame, 0, NULL, A2Q4UpdatePlayer);
		return;
	}
	if (nLevel == 50)
	{
		D2CommonQuestRecordSetBit(A2Q4RecordI(pqParam->hPlayer, pQuest->ptGame), 12, 8);
		D2CommonQuestRecordSetBit(A2Q4RecordI(pqParam->hPlayer, pQuest->ptGame), 12, 7);
	}
	if (nOldLevel != 40)
		return;
	pQuestRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bFirstJerhynOut == 1)
	{
		hMonster = SUnitFind(pQuest->ptGame, 1, pQuestRecord->dwFirstJerhynId);
		if (!hMonster)
			pQuestRecord->bFirstJerhynOut = 0;
		else if (MonsterAIHasThreats(((MONSTERDATA *)hMonster->uTypeData)->pptThreats))
			MonsterAIClearThreats(pQuest->ptGame, hMonster, NULL);
		else
		{
			GameUnitGone(hMonster, pQuest->ptGame);
			pQuestRecord->bFirstJerhynOut = 0;
		}
	}
	if (!pQuestRecord->bJerhynSpawned && pQuestRecord->bBlockerCreated)
	{
		hObject = SUnitFind(pQuest->ptGame, 2, pQuestRecord->dwBlockerId);
		if (hObject)
		{
			D2CommonUnitGetPosition(hObject, &tCoord);
			A2Q4SpawnMonster(pQuest, hObject, (GAMEROOM *)D2CommonUnitGetPathRoom(hObject), &tCoord);
		}
	}
	QuestsPlayerLeft(pQuest, pqParam);
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 12, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, 12, 1) == 1)
		return;
	if (pQuest->bState != 3)
		return;
#line 740
	QuestsSetState(pQuest, 4, D2_THIS_FILE, __LINE__);
	SUnitIterate(pQuest->ptGame, 0, NULL, A2Q4UpdatePlayer);
}

void __fastcall A2Q4SpawnMonster(QUEST *pQuest  , UNIT *hObject
, GAMEROOM *hRoom, GAMECOORD *ptCoord)
{
	A2Q4RECORD *pQuestRecord;
	UNIT *hMonster;
	QUEST *pQuest13;
	GAMEROOM *hSpotRoom;

	pQuestRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bFirstJerhynOut == 1)
	{
		hMonster = SUnitFind(pQuest->ptGame, 1, pQuestRecord->dwFirstJerhynId);
		if (hMonster)
		{
			if (MonsterAIHasThreats(((MONSTERDATA *)hMonster->uTypeData)->pptThreats))
			{
				MonsterAIClearThreats(pQuest->ptGame, hMonster, NULL);
				return;
			}
			GameUnitGone(hMonster, pQuest->ptGame);
			pQuestRecord->bFirstJerhynOut = 0;
		}
	}
	if (pQuestRecord->bJerhynSpawned == 1)
		return;
	pQuest13 = QuestsFind(pQuest->ptGame, 13);
	if ((BOOL)(pQuest13 && pQuest13->bActive && pQuest13->bState < 2) == FALSE)
		ptCoord->dwX += 15;
	else
		ptCoord->dwX -= 10;
	ptCoord->dwY -= 3;
	hSpotRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(hRoom, ptCoord->dwX, ptCoord->dwY);
	QuestsFindSpot(hRoom, ptCoord, 3, 256, &hSpotRoom);
	hMonster = MonsterSpawn(pQuest->ptGame, hSpotRoom, (int)ptCoord->dwX, (int)ptCoord->dwY, 201, 1, -1, 0);
	if (!hMonster)
	{
		hMonster = MonsterSpawn(pQuest->ptGame, hSpotRoom, (int)ptCoord->dwX, (int)ptCoord->dwY, 201, 1, 2,
			0);
		if (!hMonster)
			return;
	}
	hMonster->dwFlags0 |= 50331648;
	pQuestRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
	pQuestRecord->bJerhynSpawned = 1;
	if (pQuestRecord->bJerhynSpawnSpotSet)
		return;
	pQuestRecord->bJerhynSpawnSpotSet = 1;
	pQuestRecord->nJerhynSpawnX = (int)ptCoord->dwX;
	pQuestRecord->nJerhynSpawnY = (int)ptCoord->dwY;
}

static __inline void A2Q4NextSequence(QUEST *pQuestInfo)
{
	QUEST *pNextQuest;

	pNextQuest = QuestsFind(pQuestInfo->ptGame, (int)pQuestInfo->dwNextQuestId);
	if (!pNextQuest)
		return;
#line 852
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
	pNextQuest->pSequence(pNextQuest);
}

BYTE __fastcall A2Q4Sequence(QUEST *pQuestInfo  )
{
	if (!pQuestInfo->bState && pQuestInfo->bActive == 1)
	{
#line 864
		QuestsSetState(pQuestInfo, 1, D2_THIS_FILE, __LINE__);
		A2Q4NextSequence(pQuestInfo);
	}
	return TRUE;
}

void __fastcall A2Q4PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	void *pRecord;
	A2Q4RECORD *pQuestRecord;

	ptGame = pqParam->ptGame;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	pQuestRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
	if (D2CommonStepTestGridBit(pRecord, 11, 0))
	{
		pQuestRecord->bBlockerMode2Set = 1;
		pQuestRecord->wBlockerMode = 2;
	}
	if (D2CommonStepTestGridBit(pRecord, 12, 0) == 1
		|| D2CommonStepTestGridBit(pRecord, 12, 1) == 1)
	{
		QuestsSetRecordBit(pQuest->ptGame, 12, 13);
		pQuestRecord->bBlockerMode2Set = 1;
		pQuestRecord->wBlockerMode = 2;
		return;
	}
	if (ITEMSFindByType(pqParam->hPlayer, A2Q2_CODE_STAFF))
	{
		pQuest->bStatus = 1;
		pQuest->bState = 1;
	}
	if (D2CommonStepTestGridBit(pRecord, 12, 5) == 1)
	{
		pQuest->bStatus = 4;
		pQuest->bState = 4;
		pQuestRecord->bBlockerMode2Set = 1;
		pQuestRecord->wBlockerMode = 2;
	}
	else if (D2CommonStepTestGridBit(pRecord, 12, 4) == 1)
	{
		pQuest->bStatus = 3;
		pQuest->bState = 4;
		pQuestRecord->bBlockerMode2Set = 1;
		pQuestRecord->wBlockerMode = 2;
	}
	else if (D2CommonStepTestGridBit(pRecord, 12, 3) == 1)
	{
		pQuest->bStatus = 3;
		pQuest->bState = 3;
		pQuestRecord->bBlockerMode2Set = 1;
		pQuestRecord->wBlockerMode = 2;
	}
	else if (D2CommonStepTestGridBit(pRecord, 12, 2) == 1)
	{
		pQuest->bStatus = 2;
		pQuest->bState = 2;
		pQuestRecord->bBlockerMode2Set = 1;
		pQuestRecord->wBlockerMode = 2;
	}
	if (D2CommonStepTestGridBit(pRecord, 10, 0) == 1)
	{
		pQuestRecord->bBlockerMode2Set = 1;
		pQuestRecord->wBlockerMode = 2;
	}
}

void __fastcall A2Q4ObjectSpawned(GAME *ptGame  , GAMEROOM *hRoom,
	int nX, int nY)
{
}

void __fastcall A2Q4ObjInitMonster(QUEST *pQuest  , OBJINIT *ptInit
)
{
	A2Q4RECORD *pQuestRecord;
	QUEST *pQuest13;
	GAMECOORD tCoord;
	GAMEROOM *hRoom;
	UNIT *hMonster;

	pQuestRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bJerhynSpawned)
		return;
	if (QuestsGetRecordBit(pQuest->ptGame, 8, 13))
		return;
	if (QuestsGetRecordBit(pQuest->ptGame, 12, 13))
		return;
	pQuest13 = QuestsFind(pQuest->ptGame, 13);
	if (!pQuest13)
		return;
	if (!pQuest13->bActive)
		return;
	if (pQuest13->bState >= 2)
		return;
	hRoom = ptInit->hRoom;
	tCoord.dwX = (DWORD)ptInit->nX;
	tCoord.dwY = (DWORD)ptInit->nY;
	QuestsFindSpot(ptInit->hRoom, &tCoord, 2, 256, &hRoom);
	hMonster = MonsterSpawn(pQuest->ptGame, hRoom, (int)tCoord.dwX, (int)tCoord.dwY, 201, 1, -1, 0);
	if (!hMonster)
		return;
	pQuestRecord->bFirstJerhynOut = 1;
	pQuestRecord->dwFirstJerhynId = hMonster->dwId;
}

void __fastcall A2Q4ObjInitNpc(QUEST *pQuest  , OBJINIT *ptInit
)
{
	A2Q4RECORD *pQuestRecord;
	GAMECOORD tCoord;
	QUEST *pQuest13;
	GAMEROOM *hRoom;

	pQuestRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
	if (QuestsGetRecordBit(pQuest->ptGame, 14, 13) != 1)
	{
		hRoom = ptInit->hRoom;
		pQuestRecord->nKaelanX = ptInit->nX + 1;
		pQuestRecord->nKaelanY = ptInit->nY;
		MonsterSpawnAt(pQuest->ptGame, hRoom, pQuestRecord->nKaelanX, pQuestRecord->nKaelanY, 331, 1, 0);
	}
	EventAdd(ptInit->ptGame, ptInit->hObject, 7, ptInit->ptGame->nFrame + 1);
	tCoord.dwX = (DWORD)ptInit->nX;
	tCoord.dwY = (DWORD)ptInit->nY;
	if (pQuestRecord->bJerhynSpawned)
		return;
	if (!QuestsGetRecordBit(pQuest->ptGame, 8, 13) && !QuestsGetRecordBit(pQuest->ptGame, 9, 13))
	{
		pQuest13 = QuestsFind(pQuest->ptGame, 13);
		if (pQuest13 && pQuest13->bActive && pQuest13->bState < 2)
			return;
	}
	A2Q4SpawnMonster(pQuest, ptInit->hObject, ptInit->hRoom, &tCoord);
}

static __inline BOOL A2Q4TombsNotStarted(GAME *ptGame)
{
	QUEST *pQuest13;

	pQuest13 = QuestsFind(ptGame, 13);
	if (pQuest13 && pQuest13->bActive && pQuest13->bState < 2)
		return FALSE;
	return TRUE;
}

static __inline BOOL A2Q4TombsAtState1(GAME *ptGame)
{
	QUEST *pQuest13;

	pQuest13 = QuestsFind(ptGame, 13);
	if (pQuest13 && pQuest13->bActive && pQuest13->bState == 1)
		return TRUE;
	return FALSE;
}

BOOL __fastcall A2Q4TakeArrived(GAME *ptGame  , UNIT *hUnit)
{
	QUEST *pQuest;
	A2Q4RECORD *pQuestRecord;

	pQuest = QuestsFind(ptGame, 11);
	if (!pQuest)
		return FALSE;
	pQuestRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bJerhynArrived != 1 || pQuestRecord->bArrivalTaken)
		return FALSE;
	pQuestRecord->bArrivalTaken = 1;
	return TRUE;
}

void __fastcall A2Q4OperateObject(QUEST *pQuest  , UNIT *hObject
)
{
	A2Q4RECORD *pQuestRecord;
	GAMECOORD tCoord;
	UNIT *hCreated;

	pQuestRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
	D2CommonUnitGetPosition(hObject, &tCoord);
	if (pQuestRecord->bBlockerMode2Set)
		return;
	if (pQuestRecord->bBlockerCreated)
		return;
	pQuestRecord->wBlockerMode = 0;
	pQuestRecord->nBlockerX = (int)tCoord.dwX - 2;
	pQuestRecord->nBlockerY = (int)tCoord.dwY - 1;
	hCreated = SUnitCreate(2, 318, pQuestRecord->nBlockerX, pQuestRecord->nBlockerY, pQuest->ptGame,
		(GAMEROOM *)D2CommonUnitGetPathRoom(hObject), 1, 0, 0);
	if (!hCreated)
	{
		pQuestRecord->nBlockerY = (int)tCoord.dwY;
		hCreated = SUnitCreate(2, 318, pQuestRecord->nBlockerX, pQuestRecord->nBlockerY, pQuest->ptGame,
			(GAMEROOM *)D2CommonUnitGetPathRoom(hObject), 1, 0, 0);
		if (!hCreated)
			return;
	}
	hCreated->dwFlags0 |= 50331648;
	pQuestRecord->bBlockerCreated = 1;
	pQuestRecord->dwBlockerId = hCreated->dwId;
}

void __fastcall A2Q4ObjInitCreated(OBJINIT *ptInit  )
{
	QUEST *pQuest;
	A2Q4RECORD *pQuestRecord;

	pQuest = QuestsFind(ptInit->ptGame, 11);
	if (!pQuest)
		return;
	pQuestRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
	D2CommonUnitSetMode(ptInit->hObject, pQuestRecord->wBlockerMode);
	pQuestRecord->dwBlockerId = ptInit->hObject->dwId;
	if (pQuestRecord->wBlockerMode == 2)
		D2CommonUnitClearCollision(ptInit->hObject);
}

BOOL __fastcall A2Q4NotWaiting(GAME *ptGame  )
{
	QUEST *pQuest;
	QUEST *pQuest8;
	QUEST *pQuest13;
	BYTE bNotWaiting;

	pQuest = QuestsFind(ptGame, 11);
	if (pQuest && ((A2Q4RECORD *)pQuest->pQuestRecord)->bJerhynSpawned
		&& (pQuest8 = QuestsFind(pQuest->ptGame, 8)) != NULL && pQuest8->bActive && pQuest8->bState != 5
		&& (pQuest13 = QuestsFind(pQuest->ptGame, 13)) != NULL && pQuest13->bActive && pQuest13->bState < 4
		&& pQuest->bActive == 1 && pQuest->bState < 2)
		bNotWaiting = FALSE;
	else
		bNotWaiting = TRUE;
	return bNotWaiting == TRUE;
}

BOOL __fastcall A2Q4ObjectIdle(GAME *ptGame  )
{
	QUEST *pQuest;
	A2Q4RECORD *pQuestRecord;

	pQuest = QuestsFind(ptGame, 11);
	if (!(!pQuest))
	{
		pQuestRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
		if (pQuestRecord->field_18 || pQuestRecord->field_19)
			return FALSE;
		return pQuestRecord->wBlockerMode == 2;
	}
	return TRUE;
}

BOOL __fastcall A2Q4GetSpot(GAME *ptGame  , GAMECOORD *ptCoord)
{
	QUEST *pQuest;
	A2Q4RECORD *pQuestRecord;

	pQuest = QuestsFind(ptGame, 11);
	if (!pQuest)
		return FALSE;
	pQuestRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->field_18)
	{
		ptCoord->dwX = (DWORD)pQuestRecord->nKaelanX;
		ptCoord->dwY = (DWORD)pQuestRecord->nKaelanY;
		return TRUE;
	}
	if (pQuestRecord->field_19)
	{
		*ptCoord = *(GAMECOORD *)&pQuestRecord->nKaelanX;

		ptCoord->dwY -= 4;
		return TRUE;
	}
	return FALSE;
}

void __fastcall A2Q4MonsterThink(GAME *ptGame  , UNIT *hUnit
, DWORD *pdwIdle, DWORD *pdwBusy)
{
	QUEST *pQuest;
	A2Q4RECORD *pQuestRecord;
	UNIT *hObject;
	GAMECOORD tCoord;
	GAMEROOM *hRoom;

	pQuest = QuestsFind(ptGame, 11);
	if (!pQuest || !pQuest->bActive)
	{
		*pdwIdle = 1;
		*pdwBusy = 0;
		return;
	}
	pQuestRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
	if (!pQuestRecord->bJerhynSpawned)
	{
		*pdwIdle = 1;
		*pdwBusy = 0;
		return;
	}
	if (QuestsGetRecordBit(ptGame, 12, 13) || QuestsGetRecordBit(ptGame, 14, 13)
		|| A2Q4TombsNotStarted(ptGame))
	{
		*pdwIdle = 1;
		*pdwBusy = 0;
		return;
	}
	if (pQuestRecord->bJerhynPlaced)
	{
		*pdwIdle = 1;
		*pdwBusy = 0;
		return;
	}
	if (A2Q4TombsAtState1(ptGame))
		pQuestRecord->bQuest13State1Seen = 1;
	if (pQuestRecord->bQuest13State1Seen == 1)
	{
		if (!pQuestRecord->bJerhynWalking || pQuestRecord->bJerhynArrived)
		{
			pQuestRecord->bPlayerNear = 0;
			pQuestRecord->dwNearPlayerId = 0;
			SUnitIterate(pQuest->ptGame, 0, NULL, A2Q4FindNear);
			if (pQuestRecord->bPlayerNear)
			{
				AiWalkToPoint(ptGame, hUnit, pQuestRecord->nBlockerX, pQuestRecord->nBlockerY);
				pQuestRecord->bJerhynWalking = 1;
				pQuestRecord->bQuest13State1Seen = 0;
				*pdwIdle = 0;
				*pdwBusy = 0;
				return;
			}
			*pdwIdle = 0;
			*pdwBusy = 1;
			return;
		}
		*pdwIdle = 0;
		*pdwBusy = 1;
		if (pQuestRecord->bBlockerCreated != 1)
			return;
		if ((DWORD)AiThinkPointDistance(hUnit, pQuestRecord->nBlockerX, pQuestRecord->nBlockerY) <= 2)
		{
			hObject = SUnitFind(ptGame, 2, pQuestRecord->dwBlockerId);
			if (!hObject)
			{
				*pdwIdle = 0;
				*pdwBusy = 1;
				return;
			}
			D2CommonUnitGetPosition(hObject, &tCoord);
			tCoord.dwX += 4;
			hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hObject);
			if (!(D2CommonCollisnFindSpawnPointNear(hRoom, &tCoord, 1, 7169, 0)
				|| D2CommonCollisnFindSpawnPointNear(hRoom, &tCoord, 2, 7169, 0)
				|| D2CommonCollisnFindSpawnPointNear(hRoom, &tCoord, 3, 7169, 0)))
			{
				*pdwIdle = 0;
				*pdwBusy = 1;
				return;
			}
			if (!SUnitMoveToRoom(ptGame, hUnit, hRoom, (int)tCoord.dwX, (int)tCoord.dwY, 0, 0))
			{
				*pdwIdle = 0;
				*pdwBusy = 1;
				return;
			}
			pQuestRecord->bJerhynArrived = 1;
			pQuestRecord->bJerhynPlaced = 1;
			return;
		}
		pQuestRecord->field_19 = 1;
		pQuestRecord->field_18 = 0;
		AiWalkToPoint(ptGame, hUnit, pQuestRecord->nBlockerX, pQuestRecord->nBlockerY);
		*pdwBusy = 0;
		return;
	}
	*pdwIdle = 1;
	*pdwBusy = 0;
}

BOOL __fastcall A2Q4FindNear(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	A2Q4RECORD *pQuestRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 14, 0) == 1)
		return FALSE;
	if (!(D2CommonStepTestGridBit(pRecord, 14, 1) == 1)) {
		pQuestRecord = (A2Q4RECORD *)QuestsFind(ptGame, 11)->pQuestRecord;
		if ((DWORD)AiThinkPointDistance(hUnit, pQuestRecord->nBlockerX, pQuestRecord->nBlockerY) > 30)
			return FALSE;
		pQuestRecord->bPlayerNear = 1;
		pQuestRecord->dwNearPlayerId = hUnit->dwId;
		return TRUE; }
	return FALSE;
}

int __fastcall A2Q4OperateTome(OBJOPERATE *pObjectParam  )
{
	MISSOBJECTSTXT *ptObjectTxt;
	QUEST *pQuest;

	if (!pObjectParam->hObject->dwMode)
	{
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		ptObjectTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
		EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
			pObjectParam->ptGame->nFrame + ((DWORD)ptObjectTxt->FrameCnt[1] >> 8));
	}
	pQuest = QuestsFind(pObjectParam->ptGame, 11);
	if (!pQuest)
		return 0;
	QuestsSendUnitMessage(pObjectParam->hPlayer, pObjectParam->hObject, 396);
	if (pQuest->bActive && pQuest->bState != 5)
	{
#line 1518
		QuestsSetState(pQuest, 5, D2_THIS_FILE, __LINE__);
		SUnitIterate(pQuest->ptGame, 0, NULL, A2Q4CreditInLevel);
		SUnitIterate(pQuest->ptGame, 0, NULL, A2Q4FlagSeen);
		SUnitIterate(pQuest->ptGame, 0, NULL, A2Q4ReturnFalse);
	}
	return 0;
}

BOOL __fastcall A2Q4FlagSeen(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 12, 0) != 1
		&& D2CommonStepTestGridBit(pRecord, 12, 1) != 1)
		D2CommonQuestRecordSetBit(pRecord, 12, 14);
	return FALSE;
}

BOOL __fastcall A2Q4CreditInLevel(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	WORD wParty;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 12, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 12, 1) == 1)
		return FALSE;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	if (D2CommonDungeonRoomGetLevelId(hRoom) != 74)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 12, 13);
	D2CommonQuestRecordSetBit(pRecord, 12, 1);
	D2CommonQuestRecordSetBit(pRecord, 12, 0);
	D2CommonQuestRecordClearMidBits(pRecord, 12);
	D2CommonQuestRecordSetBit(pRecord, 12, 8);
	D2CommonQuestRecordSetBit(pRecord, 12, 7);
	wParty = SUnitGetParty(hUnit);
	if (wParty != 65535)
		PartyForEach(ptGame, wParty, A2Q4PartyCredit, NULL);
	return FALSE;
}

BOOL __fastcall A2Q4ReturnFalse(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	D2CommonStepTestGridBit(pRecordX, 12, 13);
	return FALSE;
}

void __fastcall A2Q4ObjInitLevel(OBJINIT *ptInit  )
{
	QUEST *pQuest;
	DWORD dwLevel;
	A2Q4RECORD *pQuestRecord;
	MISSOBJECTSTXT *ptObjectTxt;

	pQuest = QuestsFind(ptInit->ptGame, 11);
	if (!pQuest)
		return;
	dwLevel = D2CommonDungeonRoomGetLevelId(ptInit->hRoom);
	pQuestRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
	if (dwLevel == 74)
	{
		D2CommonUnitSetMode(ptInit->hObject, pQuestRecord->wArcanePortalMode);
		if (pQuestRecord->wArcanePortalMode != 1)
			return;
		pQuestRecord->wArcanePortalMode = 2;
		ptObjectTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(ptInit->hObject->dwClassId);
		EventAdd(ptInit->ptGame, ptInit->hObject, 1,
			ptInit->ptGame->nFrame + ((DWORD)ptObjectTxt->FrameCnt[1] >> 8) + 1);
		return;
	}
	else if (dwLevel == 54)
	{
		D2CommonUnitSetMode(ptInit->hObject, pQuestRecord->wBasementPortalMode);
		if (pQuestRecord->wBasementPortalMode != 1)
			return;
		pQuestRecord->wBasementPortalMode = 2;
		ptObjectTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(ptInit->hObject->dwClassId);
	}
	else
		return;
	EventAdd(ptInit->ptGame, ptInit->hObject, 1,
		ptInit->ptGame->nFrame + ((DWORD)ptObjectTxt->FrameCnt[1] >> 8) + 1);
}
