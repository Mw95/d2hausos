#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a4q2.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "../GAME/Game.h"
#include "../GAME/Level.h"
#include "../GAME/SCmd.h"
#include "../MONSTER/MonsterUnique.h"
#include "../PLAYER/Player.h"
#include "../UNIT/Party.h"
#include "QUESTS.h"
#include "a4q2.h"
#include "a4q3.h"
#include "../UNIT/SUnit.h"

#define A4Q2_NPC			367
#define A4Q2_NPC2			246
#define A4Q2_MONSTER		243
#define UNIT_PLAYER			0
#define UNIT_OBJECT			2
#define PARTY_NONE			65535

#pragma pack(push, 1)
struct A4Q2CMD56
{
 	BYTE		bCmd;
 	BYTE		bQuest;
 	BYTE		bStatusFlags;
 	BYTE		unobserved_03[2];
};
struct A4Q2CMD49
{
 	BYTE		bCmd;
 	WORD		wQuest;
 	BYTE		unobserved_03[12];
};
#pragma pack(pop)
typedef struct A4Q2CMD56 A4Q2CMD56;
typedef struct A4Q2CMD49 A4Q2CMD49;

static QUESTNPCTABLE sgtA4Q2Npcs[5] =
{
	{ { { 367, 681, { 0, 0 }, 1 } }, 1 },
	{ { { 367, 683, { 0, 0 }, 2 }, { 246, 682, { 0, 0 }, 2 } }, 2 },
	{ { { 367, 684, { 0, 0 }, 1 }, { 246, 685, { 0, 0 }, 1 } }, 2 },
	{ { { 367, 684, { 0, 0 }, 2 }, { 246, 685, { 0, 0 }, 2 } }, 2 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static int sgnA4Q2NpcTable[5] = { -1, 0, 1, -1, -1 };

void __fastcall A4Q2CreditPartyMember(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A4Q2Sequence(QUEST *pQuestThis);
void __fastcall A4Q2NpcMessage(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A4Q2FlagPlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A4Q2Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A4Q2ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A4Q2SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A4Q2Activate(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A4Q2PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A4Q2UnitKilled(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A4Q2FlagInAct(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A4Q2SendFinished(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A4Q2Timer(GAME *ptGame, QUEST *pQuest);
BOOL __fastcall A4Q2WarpPlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A4Q2CreditParty(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A4Q2CreditNear(GAME *ptGame, UNIT *hUnit, void *pArg);

BYTE __fastcall A4Q2Active(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer, void *pPlayerRecord)
{
	if (nNpcClass == A4Q2_NPC)
	{
		if (!D2CommonStepTestGridBit(pPlayerRecord, 26, 0)
			&& !D2CommonStepTestGridBit(pPlayerRecord, 26, 15) && pQuest->bState == 1)
			return TRUE;
		if (D2CommonStepTestGridBit(pPlayerRecord, 26, 7))
			return TRUE;
	}
	if (nNpcClass == A4Q2_NPC2 && D2CommonStepTestGridBit(pPlayerRecord, 26, 6))
		return TRUE;
	return FALSE;
}

void __fastcall A4Q2CreditPartyMember(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuest;
	void *pRecord;
	DWORD hRoom;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 26, 0) == 1)
		return;
	hRoom = D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	if (D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom)) != 3)
		return;
	D2CommonQuestRecordSetBit(pRecord, 26, 13);
	D2CommonQuestRecordSetBit(pRecord, 26, 0);
	D2CommonQuestRecordClearMidBits(pRecord, 26);
	D2CommonQuestRecordSetBit(pRecord, 26, 6);
	D2CommonQuestRecordSetBit(pRecord, 26, 7);
	ClientSetStep(SUnitGetClient(hUnit), 4, ptGame->bDifficultyLevel);
	pQuest = QuestsFind(ptGame, 23);
	if (pQuest)
		((A4Q2RECORD *)pQuest->pQuestRecord)->nPlayersCredited++;
}

BOOL __fastcall A4Q2MonstersReleased(GAME *ptGame  )
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptGame, 23);
	if (pQuest) {
		return ((A4Q2RECORD *)pQuest->pQuestRecord)->bMonstersReleased; }
	return FALSE;
}

void __fastcall A4Q2Init(QUEST *pQuest  )
{
	A4Q2RECORD *pQuestRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->bState = 0;
	pQuest->pStatusFilter = NULL;
	pQuest->pCallback[8] = A4Q2UnitKilled;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A4Q2ChangedLevel;
	pQuest->pCallback[0] = A4Q2Talk;
	pQuest->pCallback[2] = A4Q2Activate;
	pQuest->pCallback[11] = A4Q2NpcMessage;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A4Q2PlayerStarted;
	pQuest->ptNpcTables = sgtA4Q2Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->eFilter = 26;
	pQuest->bDoneStateMin = 4;
	pQuest->pActiveFilter = A4Q2Active;
#line 861
	pQuestRecord = (A4Q2RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A4Q2RECORD));
	pQuest->pQuestRecord = pQuestRecord;
	pQuest->pSequence = A4Q2Sequence;
	for (i = 0; i < (int)(sizeof(A4Q2RECORD) / sizeof(DWORD)); i++)
		((DWORD *)pQuestRecord)[i] = 0;
}

BYTE __fastcall A4Q2Sequence(QUEST *pQuestThis  )
{
	if (!pQuestThis->bState && pQuestThis->bActive == 1)
	{
#line 115
		QuestsSetState(pQuestThis, 1, D2_THIS_FILE, __LINE__);
	}
	return TRUE;
}

void __fastcall A4Q2NpcMessage(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	UNIT *hPlayer;
	WORD wNpcClass;
	WORD wMessage;
	void *pRecord;

	wNpcClass = (WORD)pqParam->field_14;
	wMessage = (WORD)pqParam->field_18;
	if (wNpcClass == A4Q2_NPC)
	{
		if (wMessage == 681)
		{
			((A4Q2RECORD *)pQuest->pQuestRecord)->bTyraelMessageDone = 1;
#line 173
			QuestsSetState(pQuest, 2, D2_THIS_FILE, __LINE__);
			SUnitIterate(pQuest->ptGame, 0, NULL, A4Q2FlagPlayer);
			QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
			return;
		}
		if (wMessage != 684)
			return;
		hPlayer = pqParam->hPlayer;
		ptGame = pQuest->ptGame;
		pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
		D2CommonQuestRecordClearBit(pRecord, 26, 7);
		return;
	}
	if (wNpcClass != A4Q2_NPC2)
		return;
	if (wMessage != 685)
		return;
	hPlayer = pqParam->hPlayer;
	ptGame = pQuest->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	D2CommonQuestRecordClearBit(pRecord, 26, 6);
}

BOOL __fastcall A4Q2FlagPlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuest;
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 26, 0) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 23);
	if (!pQuest)
		return FALSE;
	if (pQuest->bState == 2)
	{
		D2CommonQuestRecordSetBit(pRecord, 26, 2);
		return FALSE;
	}
	if (pQuest->bState != 3)
		return FALSE;
	if (pQuest->bStatus == 1)
	{
		D2CommonQuestRecordSetBit(pRecord, 26, 3);
		return FALSE;
	}
	D2CommonQuestRecordSetBit(pRecord, 26, 4);
	return FALSE;
}

void __fastcall A4Q2Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	DWORD dwBit0;
	DWORD dwBit13;
	DWORD dwBit7;
	DWORD dwBit6;
	int nNpcClass;
	int nTable;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	dwBit0 = D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0);
	dwBit13 = D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 13);
	nNpcClass = (int)pqParam->hEventUnit->dwClassId;
	dwBit7 = D2CommonStepTestGridBit(pRecord, 26, 7);
	dwBit6 = D2CommonStepTestGridBit(pRecord, 26, 6);
	if (dwBit7 && nNpcClass == A4Q2_NPC)
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 2);
		return;
	}
	if (dwBit6 && nNpcClass == A4Q2_NPC2)
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 2);
		return;
	}
	if (dwBit7)
	{
		if (nNpcClass == A4Q2_NPC2)
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 3);
		return;
	}
	if (dwBit6)
	{
		if (nNpcClass == A4Q2_NPC)
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 3);
		return;
	}
	if (dwBit0 == 1)
		return;
	if (pQuest->bState >= 4 && !dwBit13)
		return;
	if (!pQuest->bActive)
		return;
	if (!pQuest->bState)
		return;
	nTable = sgnA4Q2NpcTable[pQuest->bState];
	if (nTable == -1)
		return;
	if ((DWORD)nTable >= 5)
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, nTable);
}

void __fastcall A4Q2ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	DWORD dwLevelNew;
	DWORD dwLevelOld;
	GAME *ptGame;
	void *pRecord;

	dwLevelNew = pqParam->field_14;
	dwLevelOld = pqParam->field_18;
	if (dwLevelNew == 103 && pQuest->bState == 2)
	{
		ptGame = pqParam->ptGame;
		pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
		if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0) == 1)
			return;
		if (pQuest->bState < 3)
		{
#line 552
			QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
		}
		if (pQuest->bStatus < 1)
		{
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 1, NULL, A4Q2SendStatus, FALSE);
			pQuest->pCallback[2] = NULL;
		}
		SUnitIterate(pQuest->ptGame, 0, NULL, A4Q2FlagPlayer);
	}
	if (dwLevelOld != 108)
		return;
	if (!pQuest->bActive)
		return;
	if (pQuest->bState < 3)
	{
#line 569
		QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
	}
	if (pQuest->bStatus < 1)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 1, NULL, A4Q2SendStatus, TRUE);
		pQuest->pCallback[2] = NULL;
	}
	SUnitIterate(pQuest->ptGame, 0, NULL, A4Q2FlagPlayer);
}

BOOL __fastcall A4Q2SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 26, 0) || D2CommonStepTestGridBit(pRecord, 26, 15))
		&& !D2CommonStepTestGridBit(pRecord, 26, 13)
		&& !D2CommonStepTestGridBit(pRecord, 26, 14))
		return FALSE;
	QuestsSendStatus(ptGame, hUnit, 23);
	return FALSE;
}

void __fastcall A4Q2Activate(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A4Q2RECORD *pQuestRecord;

	if ((WORD)pqParam->hEventUnit->dwClassId != A4Q2_NPC)
		return;
	pQuestRecord = (A4Q2RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bTyraelMessageDone != 1)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 1, NULL, A4Q2SendStatus, TRUE);
	pQuestRecord->bTyraelMessageDone = 0;
	pQuest->pCallback[2] = NULL;
}

void __fastcall A4Q2PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
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
	if (D2CommonStepTestGridBit(pRecord, 26, 4) == 1)
	{
		pQuest->bStatus = 2;
		pQuest->bState = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 26, 3) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 26, 2) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 2;
	}
}

static __inline void *A4Q2PlayerRecordI(GAME *ptGame, UNIT *hPlayer)
{
	return ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
}

void __fastcall A4Q2UnitKilled(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A4Q2RECORD *pQuestRecord;
	DWORD hRoom;
	BYTE fDrop;
	int i;

	pQuestRecord = (A4Q2RECORD *)pQuest->pQuestRecord;
	if (pqParam->hEventUnit->dwClassId != A4Q2_MONSTER)
	{
		pQuestRecord->nOtherKills++;
		if (pQuestRecord->nOtherKills != 3)
			return;
		for (i = 0; i < 5; i++)
			if (!pQuestRecord->abSealsOperated[i])
				return;
		A4Q3ReleaseMonsters(pQuest->ptGame, pQuestRecord);
		if (pQuestRecord->bTimerPending)
			return;
		pQuestRecord->field_18 = 0;
		pQuestRecord->bSpawnPhase = 1;
		pQuestRecord->bEndPhase = 0;
		pQuestRecord->bTimerPending = 1;
		QuestsAddTimer(pQuest, A4Q2Timer, 1);
		return;
	}
	QuestsSetAll(pQuest->ptGame, 13);
	pQuestRecord->bDiabloKilled = 1;
	fDrop = FALSE;
	hRoom = D2CommonUnitGetPathRoom(pqParam->hEventUnit);
	if (!hRoom)
		return;
	pQuestRecord->hDiabloDeathRoom = (GAMEROOM *)hRoom;
	if (pqParam->hPlayer)
	{
		if (!((A4Q2RECORD *)pQuest->pQuestRecord)->bTimerPending)
		{
			((A4Q2RECORD *)pQuest->pQuestRecord)->bSpawnPhase = 0;
			((A4Q2RECORD *)pQuest->pQuestRecord)->bEndPhase = 1;
			((A4Q2RECORD *)pQuest->pQuestRecord)->bDropHostPending = 1;
			((A4Q2RECORD *)pQuest->pQuestRecord)->bWarpPending = 1;
			((A4Q2RECORD *)pQuest->pQuestRecord)->bClientsFlushed = 0;
			((A4Q2RECORD *)pQuest->pQuestRecord)->bTimerPending = 1;
			((A4Q2RECORD *)pQuest->pQuestRecord)->field_18 = GetTickCount();
			QuestsAddTimer(pQuest, A4Q2Timer, 1);
		}
		if (!pQuest->bActive)
			return;
		if (!D2CommonStepTestGridBit(A4Q2PlayerRecordI(pqParam->ptGame, pqParam->hPlayer), 26, 0))
			fDrop = TRUE;
	}
	if (!pQuest->bActive)
		return;
	pQuestRecord->nPlayersCredited = 0;
	QuestsSetStatus(pQuest, 13, NULL, A4Q2SendStatus, TRUE);
	SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A4Q2CreditNear);
	SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A4Q2CreditParty);
	SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A4Q2FlagInAct);
	SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A4Q2SendFinished);
	if (!fDrop)
		return;
	for (i = 0; i < pQuestRecord->nPlayersCredited; i++)
		QuestsDropItem(pQuest, pqParam, 64);
}

BOOL __fastcall A4Q2FlagInAct(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 26, 0) == 1)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 26, 14);
	QuestsSendFlagInAct(hUnit, 23, 0);
	return FALSE;
}

BOOL __fastcall A4Q2SendFinished(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	A4Q2CMD56 tCmd;

	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecordX, 26, 13))
		return FALSE;
	CLIENT *t07 = SUnitGetClient(hUnit);
	tCmd.bCmd = 86;
	tCmd.bQuest = 23;
	tCmd.bStatusFlags = 2;
	SCmdSendQuestStatus(t07, &tCmd);
	return FALSE;
}
#line 773

BYTE __fastcall A4Q2Timer(GAME *ptGame  , QUEST *pQuest
)
{
	A4Q2RECORD *pQuestRecord;
	UNIT *hObject;
	UNIT *hMonster;
	GAMEROOM *hRoom;
	int nX;
	int nY;

	pQuestRecord = (A4Q2RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bSpawnPhase)
	{
		pQuestRecord->field_18++;
		if (pQuestRecord->field_18 < 10)
			return FALSE;
		if (!pQuestRecord->bSpawnObjectKnown)
			goto lFalse;
		hObject = SUnitFind(pQuest->ptGame, UNIT_OBJECT, pQuestRecord->dwSpawnObjectId);
		if (!hObject)
			goto lFalse;
		nY = (int)D2CommonUnitGetY(hObject);
		nX = (int)D2CommonUnitGetX(hObject);
		hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hObject);
		ptGame = pQuest->ptGame;
		hMonster = MonsterSpawn(ptGame, hRoom, nX, nY, A4Q2_MONSTER, 1, -1, 0);
		if (!hMonster)
			hMonster = MonsterSpawn(ptGame, hRoom, nX, nY, A4Q2_MONSTER, 1, 5, 0);
		if (!hMonster)
			hMonster = MonsterSpawn(ptGame, hRoom, nX, nY, A4Q2_MONSTER, 1, 10, 0);
		if (!hMonster)
			goto lFalse;
		hMonster->dwFlags0 |= 50331648;
		pQuestRecord->bDiabloSpawned = 1;
		if (!hMonster)
			goto lFalse;
		pQuestRecord->bSpawnPhase = 0;
		pQuestRecord->bTimerPending = 0;
	}
	else if (pQuestRecord->bEndPhase)
	{
		if (pQuestRecord->bDropHostPending && pQuestRecord->field_18 + 95000 < GetTickCount())
		{
			pQuestRecord->bTimerPending = 0;
			pQuestRecord->bEndPhase = 0;
			pQuestRecord->bDropHostPending = 0;
			GameDropHost(ptGame, NULL);
			return TRUE;
		}
		if (pQuestRecord->field_18 + 90000 < GetTickCount())
		{
			if (pQuestRecord->bWarpPending)
			{
				SUnitIterate(ptGame, 0, NULL, A4Q2WarpPlayer);
				pQuestRecord->bWarpPending = 0;
			}
			return FALSE;
		}
		if (pQuestRecord->field_18 + 75000 < GetTickCount() && !pQuestRecord->bClientsFlushed)
		{
			pQuestRecord->bClientsFlushed = 1;
			GameFlushClients(ptGame);
		}
	lFalse:
		return FALSE;
	}
	return TRUE;
}

BOOL __fastcall A4Q2WarpPlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	A4Q2CMD56 tCmd56;
	A4Q2CMD49 tCmd49;

	PlayerInteractStart(ptGame, hUnit);
	((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->bBusy = TRUE;
	((A4Q2RECORD *)QuestsFind(ptGame, 23)->pQuestRecord)->hLastWarpedPlayer = hUnit;
	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecordX, 26, 13))
	{
		LevelWarpUnit(ptGame, hUnit, LEVEL_ACT_4_TOWN, 0);
		CLIENT *t07 = SUnitGetClient(hUnit);
		tCmd56.bCmd = 86;
		tCmd56.bQuest = 23;
		tCmd56.bStatusFlags = 1;
		SCmdSendQuestStatus(t07, &tCmd56);
		return FALSE; }
	tCmd49.bCmd = 73;
	tCmd49.wQuest = 23;
	SCmdSendQuestCommand(SUnitGetClient(hUnit), (const struct SCMDCOPY15 *)&tCmd49);
	return FALSE;
}

BOOL __fastcall A4Q2CreditParty(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	WORD wParty;

	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecordX, 26, 13))
		return FALSE;
	wParty = SUnitGetParty(hUnit);
	if (wParty == PARTY_NONE)
		return FALSE;
	PartyForEach(ptGame, wParty, A4Q2CreditPartyMember, NULL);
	return FALSE;
}

BOOL __fastcall A4Q2CreditNear(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuest;
	A4Q2RECORD *pQuestRecord;
	GAMEROOM **phNear;
	void *pRecord;
	DWORD hRoom;
	DWORD dwNear;
	DWORD i;
	BOOL fNear;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 26, 0) == 1)
		return FALSE;
	hRoom = D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	pQuest = QuestsFind(ptGame, 23);
	if (!pQuest)
		return FALSE;
	pQuestRecord = (A4Q2RECORD *)pQuest->pQuestRecord;
	fNear = FALSE;
	if ((GAMEROOM *)hRoom == pQuestRecord->hDiabloDeathRoom)
		fNear = TRUE;
	else
	{
		D2CommonDungeonRoomGetNear(hRoom, &phNear, &dwNear);
		for (i = 0; i < dwNear; i++)
		{
			if (phNear[i] == pQuestRecord->hDiabloDeathRoom)
			{
				fNear = TRUE;
				break;
			}
		}
	}
	if (D2CommonStepTestGridBit(pRecord, 26, 0))
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 26, 1))
		return FALSE;
	if (!fNear)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 26, 13);
	D2CommonQuestRecordSetBit(pRecord, 26, 0);
	D2CommonQuestRecordClearMidBits(pRecord, 26);
	D2CommonQuestRecordSetBit(pRecord, 26, 6);
	D2CommonQuestRecordSetBit(pRecord, 26, 7);
	ClientSetStep(SUnitGetClient(hUnit), 4, ptGame->bDifficultyLevel);
	pQuest = QuestsFind(ptGame, 23);
	if (pQuest)
		((A4Q2RECORD *)pQuest->pQuestRecord)->nPlayersCredited++;
	return FALSE;
}
