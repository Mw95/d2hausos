#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a2q3.cpp"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "../GAME/SCmd.h"
#include "../ITEMS/Items.h"
#include "QUESTS.h"
#include "a2q3.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"

#define A2Q2_CODE_STAFF		544502632
#define A2Q2_CODE_BOX		544763746
#define A2Q2_CODE_SCROLL	540111476
#define A2Q2_CODE_AMULET	544237942
#define A2Q2_CODE_SHAFT		543585133

#pragma pack(push, 1)
struct A2Q3CYCLECMD
{
 	BYTE		bCmd;
 	BYTE		bQuest;
 	BYTE		bStatusFlags;
 	BYTE		unobserved_03[2];
};
#pragma pack(pop)
typedef struct A2Q3CYCLECMD A2Q3CYCLECMD;

static QUESTNPCTABLE sgtA2Q3Npcs[6] =
{
	{ { { 176, 344, { 0, 0 }, 2 }, { 175, 343, { 0, 0 }, 2 }, { 198, 341, { 0, 0 }, 2 },
		{ 199, 342, { 0, 0 }, 2 }, { 177, 348, { 0, 0 }, 1 }, { 202, 347, { 0, 0 }, 2 },
		{ 244, 350, { 0, 0 }, 2 }, { 210, 349, { 0, 0 }, 2 }, { 200, 345, { 0, 0 }, 2 },
		{ 201, 340, { 0, 0 }, 2 }, { 178, 346, { 0, 0 }, 2 } }, 11 },
	{ { { 176, 344, { 0, 0 }, 2 }, { 175, 343, { 0, 0 }, 2 }, { 198, 341, { 0, 0 }, 2 },
		{ 199, 342, { 0, 0 }, 2 }, { 177, 348, { 0, 0 }, 2 }, { 202, 347, { 0, 0 }, 2 },
		{ 244, 350, { 0, 0 }, 2 }, { 210, 349, { 0, 0 }, 2 }, { 200, 345, { 0, 0 }, 2 },
		{ 201, 340, { 0, 0 }, 2 }, { 178, 346, { 0, 0 }, 2 } }, 11 },
	{ { { 176, 361, { 0, 0 }, 2 }, { 175, 353, { 0, 0 }, 2 }, { 198, 352, { 0, 0 }, 2 },
		{ 199, 359, { 0, 0 }, 2 }, { 177, 358, { 0, 0 }, 2 }, { 202, 357, { 0, 0 }, 2 },
		{ 244, 360, { 0, 0 }, 2 }, { 210, 355, { 0, 0 }, 2 }, { 201, 351, { 0, 0 }, 2 },
		{ 200, 354, { 0, 0 }, 2 }, { 178, 356, { 0, 0 }, 2 } }, 11 },
	{ { { 176, 368, { 0, 0 }, 1 }, { 175, 366, { 0, 0 }, 1 }, { 198, 363, { 0, 0 }, 1 },
		{ 199, 364, { 0, 0 }, 1 }, { 177, 371, { 0, 0 }, 1 }, { 202, 370, { 0, 0 }, 1 },
		{ 244, 372, { 0, 0 }, 1 }, { 210, 367, { 0, 0 }, 1 }, { 200, 365, { 0, 0 }, 1 },
		{ 201, 362, { 0, 0 }, 1 } }, 10 },
	{ { { 176, 368, { 0, 0 }, 2 }, { 175, 366, { 0, 0 }, 2 }, { 198, 363, { 0, 0 }, 2 },
		{ 201, 362, { 0, 0 }, 2 }, { 199, 364, { 0, 0 }, 2 }, { 177, 371, { 0, 0 }, 2 },
		{ 202, 370, { 0, 0 }, 2 }, { 210, 367, { 0, 0 }, 2 }, { 200, 365, { 0, 0 }, 2 },
		{ 178, 369, { 0, 0 }, 2 } }, 10 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static int sgnA2Q3NpcTable[6] = { -1, 0, 1, 2, 3, 0 };

BOOL __fastcall A2Q2CountScroll(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q2CountShaft(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q2CountBox(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A2Q3PartyCredit(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A2Q3Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
BOOL __fastcall A2Q3SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q3SendAltCycle(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A2Q3NpcMessage(QUEST *pQuestInfo, QUESTPARAM *pqParam);
void __fastcall A2Q3PlayerDone(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q3Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q3ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A2Q3Timer(GAME *ptGame, QUEST *pQuest);
BYTE __fastcall A2Q3Sequence(QUEST *pQuestInfo);
BOOL __fastcall A2Q3UpdatePlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A2Q3PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A2Q3FlagPlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q3CreditParty(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q3SendUsualCycle(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q3CreditInLevel(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A2Q3DoneTimer(GAME *ptGame, QUEST *pQuest);
BOOL __fastcall A2Q3PlayerEvent(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q3CountAmulet(GAME *ptGame, UNIT *hUnit, void *pArg);

void __fastcall A2Q2PlayerJoined(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A2Q2RECORD *pRecord;
	QUEST *pQuest10;
	QUEST *pQuest13;

	pRecord = (A2Q2RECORD *)pQuest->pQuestRecord;
	if (ITEMSFindByType(pqParam->hPlayer, A2Q2_CODE_SHAFT))
		pRecord->nShaftCount++;
	if (ITEMSFindByType(pqParam->hPlayer, A2Q2_CODE_AMULET))
		pRecord->nAmuletCount++;
	if (ITEMSFindByType(pqParam->hPlayer, A2Q2_CODE_BOX))
		pRecord->nCubeCount++;
	if (ITEMSFindByType(pqParam->hPlayer, A2Q2_CODE_STAFF))
		pRecord->nStaffCount++;
	if (!pRecord->bMissingReported)
		return;
	if (pRecord->nStaffCount <= 0)
	{
		if (!pRecord->nCubeCount && pRecord->bCubeDropped)
			return;
		if (!pRecord->nAmuletCount)
		{
			pQuest10 = QuestsFind(pQuest->ptGame, 10);
			if (pQuest10 && ((A2Q3RECORD *)pQuest10->pQuestRecord)->bObjectOperated)
				return;
		}
		if (!pRecord->nShaftCount && pRecord->bShaftDropped)
			return;
	}
	pQuest13 = QuestsFind(pQuest->ptGame, 13);
	if (!pQuest13 || !pQuest13->bActive)
		return;
	if (((A2Q6RECORD *)pQuest13->pQuestRecord)->bStaffPlaced != 1)
		((A2Q6RECORD *)pQuest13->pQuestRecord)->bForcedStatus = 0;
}

void __fastcall A2Q2PlayerDropped(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A2Q2RECORD *pRecord;
	QUEST *pQuest10;
	QUEST *pQuest13;
	A2Q6RECORD *pRecord13;
	BOOL fMissing;

	pRecord = (A2Q2RECORD *)pQuest->pQuestRecord;
	switch (D2CommonITEMSGetTypeCode(pqParam->hEventUnit))
	{
	case A2Q2_CODE_AMULET:
		pRecord->nAmuletCount--;
		break;
	case A2Q2_CODE_SHAFT:
		pRecord->nShaftCount--;
		break;
	case A2Q2_CODE_STAFF:
		pRecord->nStaffCount--;
		break;
	case A2Q2_CODE_BOX:
		pRecord->nCubeCount--;
		break;
	}
	if (pRecord->bMissingReported || pRecord->nStaffCount)
		return;
	fMissing = FALSE;
	if (pRecord->bCubeDropped && !pRecord->nCubeCount)
		fMissing = TRUE;
	else
	{
		if (!pRecord->nAmuletCount)
		{
			pQuest10 = QuestsFind(pQuest->ptGame, 10);
			if (pQuest10 && ((A2Q3RECORD *)pQuest10->pQuestRecord)->bObjectOperated)
				fMissing = TRUE;
		}
		if (!fMissing && pRecord->bShaftDropped && !pRecord->nShaftCount)
			fMissing = TRUE;
	}
	if (!fMissing)
		return;
	pQuest13 = QuestsFind(pQuest->ptGame, 13);
	if (pQuest13 && pQuest13->bActive)
	{
		pRecord13 = (A2Q6RECORD *)pQuest13->pQuestRecord;
		if (pRecord13->bStaffPlaced != 1)
		{
			if (pQuest13->ptGame->bGameType == 3)
			{
				pRecord13->bForcedStatus = 1;
				pRecord13->nForcedStatus = 8;
			}
			else
			{
				pRecord13->bForcedStatus = 1;
				pRecord13->nForcedStatus = 9;
			}
		}
	}
	pRecord->bMissingReported = 1;
}

BOOL __fastcall A2Q3NoStaff(GAME *ptGame  , UNIT *hUnit
)
{
	return !ITEMSFindByType(hUnit, A2Q2_CODE_STAFF);
}

int __fastcall A2Q2OperateScroll(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuest;
	A2Q2RECORD *pRecord;
	int nLevel;
	int i;

	if (QuestsOperateObject(pObjectParam))
	{
		pObjectParam->hObject->dwDropItemCode = A2Q2_CODE_SCROLL;
		pQuest = QuestsFind(pObjectParam->ptGame, 9);
		if (pQuest)
		{
			pRecord = (A2Q2RECORD *)pQuest->pQuestRecord;
			pRecord->nDropCount = 0;
			SUnitIterate(pObjectParam->ptGame, 0, pObjectParam->hPlayer, A2Q2CountScroll);
			for (i = 0; i < pRecord->nDropCount; i++)
				ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 7, &nLevel, NULL, 44, TRUE);
		}
	}
	return 0;
}

BOOL __fastcall A2Q2CountScroll(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	QUEST *pQuest;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 10, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 10, 3))
		return FALSE;
	pQuest = QuestsFind(ptGame, 9);
	if (pQuest)
		((A2Q2RECORD *)pQuest->pQuestRecord)->nDropCount++;
	return FALSE;
}

int __fastcall A2Q2OperateShaft(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuest;
	A2Q2RECORD *pRecord;
	int nLevel;
	int i;

	if (!(!QuestsOperateObject(pObjectParam)))
	{
		pObjectParam->hObject->dwDropItemCode = A2Q2_CODE_SHAFT;
		pQuest = QuestsFind(pObjectParam->ptGame, 9);
		if (pQuest)
		{
			pRecord = (A2Q2RECORD *)pQuest->pQuestRecord;
			pRecord->nDropCount = 0;
			SUnitIterate(pObjectParam->ptGame, 0, pObjectParam->hPlayer, A2Q2CountShaft);
			for (i = 0; i < pRecord->nDropCount; i++)
			{
				if (ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 7, &nLevel, NULL, 44, TRUE))
				{
					pRecord->bShaftDropped = 1;
					pRecord->nShaftCount++;
				}
			}
		}
	}
	return 0;
}

BOOL __fastcall A2Q2CountShaft(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuest;

	if (ITEMSFindByType(hUnit, A2Q2_CODE_SHAFT))
		return FALSE;
	if (ITEMSFindByType(hUnit, A2Q2_CODE_STAFF))
		return FALSE;
	pQuest = QuestsFind(ptGame, 9);
	if (pQuest)
		((A2Q2RECORD *)pQuest->pQuestRecord)->nDropCount++;
	return FALSE;
}

int __fastcall A2Q2OperateBox(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuest;
	A2Q2RECORD *pRecord;
	int nLevel;
	int i;

	if (!(!QuestsOperateObject(pObjectParam)))
	{
		pObjectParam->hObject->dwDropItemCode = A2Q2_CODE_BOX;
		pQuest = QuestsFind(pObjectParam->ptGame, 9);
		if (pQuest)
		{
			pRecord = (A2Q2RECORD *)pQuest->pQuestRecord;
			pRecord->nDropCount = 0;
			SUnitIterate(pObjectParam->ptGame, 0, pObjectParam->hPlayer, A2Q2CountBox);
			for (i = 0; i < pRecord->nDropCount; i++)
			{
				if (ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 2, &nLevel, NULL, 44, TRUE))
				{
					pRecord->bCubeDropped = 1;
					pRecord->nCubeCount++;
				}
			}
		}
	}
	return 0;
}

BOOL __fastcall A2Q2CountBox(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuest;

	if (ITEMSFindByType(hUnit, A2Q2_CODE_BOX))
		return FALSE;
	pQuest = QuestsFind(ptGame, 9);
	if (pQuest)
		((A2Q2RECORD *)pQuest->pQuestRecord)->nDropCount++;
	return FALSE;
}

void __fastcall A2Q3Nothing(GAME *ptGame  , UNIT *hUnit
)
{
}

void __fastcall A2Q2BoxState(GAME *ptGame  , int *pnCount, BYTE *pbFlag)
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptGame, 9);
	if (pQuest)
	{
		*pnCount = ((A2Q2RECORD *)pQuest->pQuestRecord)->nCubeCount;
		*pbFlag = ((A2Q2RECORD *)pQuest->pQuestRecord)->bCubeDropped;
	}
	*pnCount = 0;
	*pbFlag = 1;
}

void __fastcall A2Q3PartyCredit(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	DWORD dwLevelId;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 11, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, 11, 1) == 1)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	dwLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
	if (!dwLevelId)
		return;
	if (D2CommonGetActNum(dwLevelId) != 1)
		return;
	D2CommonQuestRecordSetBit(pRecord, 11, 13);
	D2CommonQuestRecordSetBit(pRecord, 11, 1);
}

BYTE __fastcall A2Q3Active(QUEST *pQuest  , int nNpcClass, UNIT *hPlayer,
	void *pPlayerRecord)
{
	if (D2CommonStepTestGridBit(pPlayerRecord, pQuest->eFilter, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pPlayerRecord, pQuest->eFilter, 1) == 1)
	{
		if (nNpcClass == 176 || nNpcClass == 175 || nNpcClass == 198 || nNpcClass == 199
			|| nNpcClass == 177 || nNpcClass == 202 || nNpcClass == 244 || nNpcClass == 210
			|| nNpcClass == 200 || nNpcClass == 201)
			return TRUE;
	}
	if (nNpcClass == 177 && pQuest->bState == 1)
		return TRUE;
	return FALSE;
}

static __inline BOOL A2Q3StartCycle(QUEST *pQuest)
{
	GAME *ptGame;

	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 1, NULL, A2Q3SendStatus, TRUE);
	if (!pQuest->bState)
	{
#line 528
		QuestsSetState(pQuest, 1, D2_THIS_FILE, __LINE__);
	}
	ptGame = pQuest->ptGame;
	if (ptGame->ptAct[1])
	{
		D2CommonEnvSetAltCycle(ptGame->ptAct[1]);
		SUnitIterate(ptGame, 0, NULL, A2Q3SendAltCycle);
		return TRUE;
	}
	((A2Q3RECORD *)pQuest->pQuestRecord)->bAltCyclePending = 1;
	return FALSE;
}

void __fastcall A2Q3ObjInit(QUEST *pQuest  , OBJINIT *ptInit
)
{
	A2Q3RECORD *pRecord;

	pRecord = (A2Q3RECORD *)pQuest->pQuestRecord;
	pRecord->field_05 = 1;
	pRecord->dwObjectId = ptInit->hObject->dwId;
	if (pQuest->bActive == 1)
	{
		if (pQuest->bState <= 1)
		{
			if (!A2Q3StartCycle(pQuest))
				pRecord->bAltCyclePending = 1;
			else
				pRecord->bAltCycleOn = 1;
			pQuest->bState = 3;
			pQuest->bStatusFlags = 0;
		}
		if (!pQuest->bStatus)
		{
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 2, NULL, A2Q3SendStatus, TRUE);
		}
	}
	D2CommonUnitSetMode(ptInit->hObject, pRecord->dwObjectMode);
}

BOOL __fastcall A2Q3SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 11, 0) || D2CommonStepTestGridBit(pRecord, 11, 15))
		&& !D2CommonStepTestGridBit(pRecord, 11, 13) && !D2CommonStepTestGridBit(pRecord, 11, 14))
		return FALSE;
	QuestsSendStatus(ptGame, hUnit, 10);
	return FALSE;
}

BOOL __fastcall A2Q3SendAltCycle(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	CLIENT *ptClient;
	BYTE bAct;
	A2Q3CYCLECMD tCmd;
	SCMDENVPACKET tEnv;

	ptClient = SUnitGetClient(hUnit);
	bAct = ClientGetAct(ptClient);
	if (bAct == 1)
	{
		tEnv.bCmd = S2C_ROOM_OP;
		tEnv.dwCycle = 5;
		tEnv.dwAngle = 0;
		tEnv.bAlt = bAct;
		SCmdSendEnv(ptClient, &tEnv);
		tCmd.bCmd = 86;
		tCmd.bQuest = 10;
		tCmd.bStatusFlags = 16;
		SCmdSendQuestStatus(ptClient, &tCmd);
	}
	return FALSE;
}

void __fastcall A2Q3Init(QUEST *pQuest  )
{
	A2Q3RECORD *pRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->bState = 0;
	pQuest->pCallback[0] = A2Q3Talk;
	pQuest->pCallback[11] = A2Q3NpcMessage;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A2Q3ChangedLevel;
	pQuest->pCallback[10] = A2Q3PlayerDone;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A2Q3PlayerStarted;
	pQuest->ptNpcTables = sgtA2Q3Npcs;
	pQuest->bCallbacksEnabled = 1;
#line 801
	pRecord = (A2Q3RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A2Q3RECORD));
	pQuest->eFilter = 11;
	pQuest->dwNextQuestId = 11;
	pQuest->pQuestRecord = pRecord;
	pQuest->pStatusFilter = NULL;
	pQuest->bDoneStateMin = 4;
	pQuest->pActiveFilter = A2Q3Active;
	pQuest->pSequence = A2Q3Sequence;
	for (i = 0; i < (int)(sizeof(A2Q3RECORD) / sizeof(DWORD)); i++)
		((DWORD *)pRecord)[i] = 0;
	pRecord->dwObjectMode = 0;
	QuestsPlayersClear(&pRecord->tPlayers);
}

void __fastcall A2Q3NpcMessage(QUEST *pQuestInfo  , QUESTPARAM *pqParam
)
{
	int nMessage;
	void *pRecord;
	GAME *ptGame;

	nMessage = (WORD)pqParam->field_18;
	if (nMessage != 348 && (nMessage <= 361 || nMessage > 372))
		return;
	if (nMessage == 348)
	{
		if (!pQuestInfo->bActive)
			return;
		if (pqParam->field_14 != 177)
			return;
		if (pQuestInfo->bStatus == 1)
		{
			pQuestInfo->bStatusFlags = 0;
			QuestsSetStatus(pQuestInfo, 2, NULL, A2Q3SendStatus, FALSE);
		}
		if (pQuestInfo->bState != 1)
			return;
#line 205
		QuestsSetState(pQuestInfo, 2, D2_THIS_FILE, __LINE__);
		SUnitIterate(pQuestInfo->ptGame, 0, NULL, A2Q3UpdatePlayer);
		return;
	}
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (pQuestInfo->bState != 5 && pQuestInfo->bActive == 1
		&& D2CommonStepTestGridBit(pRecord, pQuestInfo->eFilter, 13))
	{
		pQuestInfo->bStatusFlags = 0;
		QuestsSetStatus(pQuestInfo, 13, NULL, A2Q3SendStatus, FALSE);
#line 220
		QuestsSetState(pQuestInfo, 5, D2_THIS_FILE, __LINE__);
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		pQuestInfo->pSequence(pQuestInfo);
	}
	QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
	if (!D2CommonStepTestGridBit(pRecord, pQuestInfo->eFilter, 1))
		return;
	D2CommonQuestRecordSetBit(pRecord, 11, 0);
	D2CommonQuestRecordClearBit(pRecord, 11, 1);
	QuestsPlayersAdd(&pQuestInfo->tPlayers, pqParam->hPlayer->dwId);
	if (!pQuestInfo->bActive)
		QuestsSetRecordBit(pQuestInfo->ptGame, 11, 13);
}

void __fastcall A2Q3PlayerDone(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	QuestsPlayerDone(pQuest, pqParam);
	QuestsPlayersRemove(&((A2Q3RECORD *)pQuest->pQuestRecord)->tPlayers, pqParam->hPlayer->dwId);
}

void __fastcall A2Q3Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	void *pRecord;
	int nNpcClass;
	int nTable;

	ptGame = pqParam->ptGame;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	nNpcClass = pqParam->hEventUnit->dwClassId;
	if (D2CommonStepTestGridBit(pRecord, 11, 1))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 3);
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 11, 14))
		return;
	if (QuestsHasPlayer(pQuest, pqParam->hPlayer->dwId) == 1)
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 4);
		return;
	}
	if (!pQuest->bState || !pQuest->bActive)
		return;
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0) == 1)
		return;
	if (pQuest->bState >= 4)
		return;
	nTable = sgnA2Q3NpcTable[pQuest->bState];
	if (nTable == -1 || (unsigned int)nTable >= 6)
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, nTable);
}
void __fastcall A2Q3ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	DWORD dwLevelNew;
	DWORD dwLevelOld;
	A2Q3RECORD *pRecord;
	void *pSeed;

	dwLevelNew = pqParam->field_18;
	dwLevelOld = pqParam->field_14;
	pRecord = (A2Q3RECORD *)pQuest->pQuestRecord;
	if ((dwLevelNew == 44 || dwLevelNew == 45) && !pQuest->bState && pQuest->bActive == 1
		&& !pRecord->bTimerPending)
	{
		pSeed = QuestsGetSeed(pQuest->ptGame);
		QuestsAddTimer(pQuest, A2Q3Timer,
			D2CommonSeedBetween(pSeed, 15, 17));
		pRecord->bTimerPending = 1;
	}
	if (dwLevelNew == 40 && pRecord->bAltCyclePending == 1)
	{
		if (A2Q3StartCycle(pQuest))
		{
			SUnitIterate(pQuest->ptGame, 0, NULL, A2Q3UpdatePlayer);
			pRecord->bAltCyclePending = 0;
		}
	}
	if (dwLevelOld != 40)
		return;
	QuestsPlayerLeft(pQuest, pqParam);
	if (pQuest->bState == 2)
	{
#line 623
		QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
	}
}

BYTE __fastcall A2Q3Timer(GAME *ptGame  , QUEST *pQuest
)
{
	A2Q3RECORD *pRecord;

	pRecord = (A2Q3RECORD *)pQuest->pQuestRecord;
	pRecord->bTimerPending = 0;
	if (!pRecord->bAltCycleOn)
	{
		if (!A2Q3StartCycle(pQuest))
			pRecord->bAltCyclePending = 1;
		else
		{
			pRecord->bAltCycleOn = 1;
			SUnitIterate(pQuest->ptGame, 0, NULL, A2Q3UpdatePlayer);
		}
	}
	return TRUE;
}

BYTE __fastcall A2Q3Sequence(QUEST *pQuestInfo  )
{
	QUEST *pNextQuest;

	if (pQuestInfo->bState != 5 && pQuestInfo->bActive)
		return TRUE;
	pNextQuest = QuestsFind(pQuestInfo->ptGame, (int)pQuestInfo->dwNextQuestId);
	if (pNextQuest)
	{
#line 676
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		return pNextQuest->pSequence(pNextQuest);
	}
	return FALSE;
}

BOOL __fastcall A2Q3UpdatePlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	QUEST *pQuest;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 11, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 11, 1) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 10);
	if (!pQuest)
		return FALSE;
	if (pQuest->bState >= 1)
		D2CommonQuestRecordSetBit(pRecord, 11, 2);
	if (pQuest->bState == 2)
	{
		D2CommonQuestRecordSetBit(pRecord, 11, 3);
		return FALSE;
	}
	if (pQuest->bState == 3)
		D2CommonQuestRecordSetBit(pRecord, 11, 4);
	return FALSE;
}

void __fastcall A2Q3PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	void *pRecord;
	UNIT *hPlayer;
	A2Q3RECORD *pQuestRecord;
	A2Q3CYCLECMD tCmd;

	ptGame = pqParam->ptGame;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 11, 0) == 1
		|| D2CommonStepTestGridBit(pRecord, 11, 1) == 1)
	{
		QuestsSetRecordBit(pQuest->ptGame, 11, 13);
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 11, 2) != 1)
		return;
	hPlayer = pqParam->hPlayer;
	pQuestRecord = (A2Q3RECORD *)pQuest->pQuestRecord;
	if (!A2Q3StartCycle(pQuest))
	{ A2Q3CYCLECMD tCmd;
		tCmd.bCmd = 86;
		tCmd.bQuest = 10;
		pQuestRecord->bAltCyclePending = 1;
		tCmd.bStatusFlags = 1;
		SCmdSendQuestStatus(SUnitGetClient(hPlayer), &tCmd);
	}
	else
		pQuestRecord->bAltCycleOn = 1;
	if (D2CommonStepTestGridBit(pRecord, 11, 4))
	{
		pQuest->bStatus = 2;
		pQuest->bState = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 11, 3))
	{
		pQuest->bStatus = 2;
		pQuest->bState = 2;
		return;
	}
	pQuest->bStatus = 1;
	pQuest->bState = 1;
}
int __fastcall A2Q3OperateObject(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuest;
	A2Q3RECORD *pQuestRecord;
	GAME *ptGame;
	UNIT *hPlayer;
	void *pRecord;
	QUEST *pQuest9;
	int nLevel;
	int nDropped;
	int i;

	pQuest = QuestsFind(pObjectParam->ptGame, 10);
	if (!pQuest)
		return 0;
	if (pObjectParam->hObject->dwMode)
		return 0;
	pQuestRecord = (A2Q3RECORD *)pQuest->pQuestRecord;
	if (!pQuest->bActive)
	{
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		pQuestRecord->dwObjectMode = 2;
		pQuestRecord->nDropCount = 0;
		pQuestRecord->bObjectOperated = 1;
		SUnitIterate(pObjectParam->ptGame, 0, pObjectParam->hPlayer, A2Q3CountAmulet);
		nLevel = (int)D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(pObjectParam->hObject));
		nDropped = 0;
		pObjectParam->hObject->dwDropItemCode = A2Q2_CODE_AMULET;
		for (i = 0; i < pQuestRecord->nDropCount; i++)
		{
			if (ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 7, &nLevel, NULL, 44, FALSE))
				nDropped++;
		}
		pQuest9 = QuestsFind(pQuest->ptGame, 9);
		if (pQuest9)
			((A2Q2RECORD *)pQuest9->pQuestRecord)->nAmuletCount += nDropped;
		return 0;
	}
	pQuestRecord->hObjectRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(pObjectParam->hObject);
	pQuestRecord->dwObjectLevelId = D2CommonDungeonRoomGetLevelId(pQuestRecord->hObjectRoom);
	D2CommonUnitSetMode(pObjectParam->hObject, 1);
	pQuestRecord->dwObjectMode = 2;
	pQuestRecord->field_05 = 1;
	pQuestRecord->dwObjectId = pObjectParam->hObject->dwId;
	pQuestRecord->bObjectOperated = 1;
	if (pQuest->bActive)
	{
		ptGame = pQuest->ptGame;
		if (ptGame->ptAct[1])
		{
			((A2Q3RECORD *)pQuest->pQuestRecord)->bAltCycleOn = 0;
			D2CommonEnvSetUsualCycle(ptGame->ptAct[1]);
			SUnitIterate(ptGame, 0, NULL, A2Q3SendUsualCycle);
		}
#line 968
		QuestsDebugMessage(pQuest->ptGame, "Finished quest", D2_THIS_FILE, __LINE__);
		QuestsSetState(pQuest, 4, D2_THIS_FILE, __LINE__);
		QuestsSetAll(pQuest->ptGame, 6);
	}
	hPlayer = pObjectParam->hPlayer;
	ptGame = pObjectParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecord, 11, 0)
		|| !ITEMSFindByType(pObjectParam->hPlayer, A2Q2_CODE_AMULET))
	{
		pQuestRecord->nDropCount = 0;
		SUnitIterate(pObjectParam->ptGame, 0, pObjectParam->hPlayer, A2Q3CountAmulet);
		if (pQuest->bActive)
		{
			if (!D2CommonStepTestGridBit(pRecord, 11, 0))
			{
				D2CommonQuestRecordSetBit(pRecord, 11, 1);
				D2CommonQuestRecordSetBit(pRecord, 11, 13);
				D2CommonQuestRecordClearBit(pRecord, 11, 14);
			}
			SUnitIterate(pObjectParam->ptGame, 0, pObjectParam->hObject, A2Q3CreditInLevel);
			SUnitIterate(pObjectParam->ptGame, 0, pObjectParam->hObject, A2Q3CreditParty);
			SUnitIterate(pObjectParam->ptGame, 0, pObjectParam->hObject, A2Q3PlayerEvent);
			QuestsSetRecordBit(pObjectParam->ptGame, pQuest->eFilter, 13);
		}
		nLevel = (int)D2CommonDungeonRoomGetLevelId(pQuestRecord->hObjectRoom);
		nDropped = 0;
		pObjectParam->hObject->dwDropItemCode = A2Q2_CODE_AMULET;
		for (i = 0; i < pQuestRecord->nDropCount; i++)
		{
			if (ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 7, &nLevel, NULL, 44, FALSE))
				nDropped++;
		}
		pQuest9 = QuestsFind(pQuest->ptGame, 9);
		if (pQuest9)
			((A2Q2RECORD *)pQuest9->pQuestRecord)->nAmuletCount += nDropped;
	}
	SUnitIterate(pQuest->ptGame, 0, NULL, A2Q3FlagPlayer);
	QuestsAddTimer(pQuest, A2Q3DoneTimer, 10);
	return 0;
}

BOOL __fastcall A2Q3FlagPlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 11, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 11, 1) == 1)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 11, 14);
	QuestsSendFlagInAct(hUnit, 10, 0);
	return FALSE;
}

BOOL __fastcall A2Q3CreditParty(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	WORD wParty;

	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecordX, 11, 13))
	{
		wParty = SUnitGetParty(hUnit);
		if (wParty != 65535)
			PartyForEach(ptGame, wParty, A2Q3PartyCredit, NULL);
	}
	return FALSE;
}

BOOL __fastcall A2Q3SendUsualCycle(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	CLIENT *ptClient;
	SCMDENVPACKET tEnv;

	ptClient = SUnitGetClient(hUnit);
	if (ClientGetAct(ptClient) == 1)
	{
		tEnv.bCmd = S2C_ROOM_OP;
		tEnv.dwCycle = 2;
		tEnv.dwAngle = 0;
		tEnv.bAlt = 0;
		SCmdSendEnv(ptClient, &tEnv);
	}
	return FALSE;
}

BOOL __fastcall A2Q3CreditInLevel(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	QUEST *pQuest;
	A2Q3RECORD *pQuestRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	pQuest = QuestsFind(ptGame, 10);
	if (!hRoom || !pQuest)
		return FALSE;
	pQuestRecord = (A2Q3RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->dwObjectLevelId != D2CommonDungeonRoomGetLevelId(hRoom))
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 11, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 11, 15) == 1)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 11, 13);
	D2CommonQuestRecordSetBit(pRecord, 11, 1);
	return FALSE;
}

BYTE __fastcall A2Q3DoneTimer(GAME *ptGame  , QUEST *pQuest
)
{
	A2Q3RECORD *pRecord;

	pRecord = (A2Q3RECORD *)pQuest->pQuestRecord;
	if (pQuest->bState == 4)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 3, NULL, A2Q3SendStatus, TRUE);
	}
	pRecord->field_06 = 0;
	return TRUE;
}

BOOL __fastcall A2Q3PlayerEvent(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecordX, 11, 13))
		SUnitSetEvent(hUnit, 50, hUnit);
	return FALSE;
}

BOOL __fastcall A2Q3CountAmulet(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuest;
	void *pRecord;

	if (ITEMSFindByType(hUnit, A2Q2_CODE_AMULET))
		return FALSE;
	if (ITEMSFindByType(hUnit, A2Q2_CODE_STAFF))
		return FALSE;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 10, 0))
		return FALSE;
	pQuest = QuestsFind(ptGame, 10);
	if (pQuest)
		((A2Q3RECORD *)pQuest->pQuestRecord)->nDropCount++;
	return FALSE;
}

void __fastcall A2Q3ActLoaded(GAME *ptGame  , BYTE bAct)
{
	QUEST *pQuest;
	A2Q3RECORD *pRecord;

	if (bAct != 1)
		return;
	pQuest = QuestsFind(ptGame, 10);
	if (!pQuest)
		return;
	pRecord = (A2Q3RECORD *)pQuest->pQuestRecord;
	if (pRecord->bAltCyclePending != 1)
		return;
	D2CommonEnvSetAltCycle(ptGame->ptAct[1]);
	pRecord->bAltCyclePending = 0;
	pRecord->bAltCycleOn = 1;
}
