#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a2q2.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../ITEMS/Items.h"
#include "QUESTS.h"
#include "a2q1.h"
#include "a2q2.h"
#include "a2q5.h"
#include "a2q3.h"
#include "a2q4.h"

#define A2Q2_CODE_STAFF		544502632
#define A2Q2_CODE_BOX		544763746
#define A2Q2_CODE_SCROLL	540111476
#define A2Q2_CODE_AMULET	544237942
#define A2Q2_CODE_SHAFT		543585133

static QUESTNPCTABLE sgtA2Q2Npcs[11] =
{
	{ { { 244, 335, { 0, 0 }, 0 } }, 1 },
	{ { { 244, 336, { 0, 0 }, 0 } }, 1 },
	{ { { 244, 337, { 0, 0 }, 0 } }, 1 },
	{ { { 244, 338, { 0, 0 }, 0 } }, 1 },
	{ { { 244, 339, { 0, 0 }, 0 } }, 1 },
	{ { { 244, 339, { 0, 0 }, 2 } }, 1 },
	{ { { 244, 335, { 0, 0 }, 2 } }, 1 },
	{ { { 244, 336, { 0, 0 }, 2 } }, 1 },
	{ { { 244, 337, { 0, 0 }, 2 } }, 1 },
	{ { { 244, 338, { 0, 0 }, 2 } }, 1 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

BOOL __fastcall A2Q4SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A2Q4UpdatePlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A2Q2NpcMessage(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q2Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q2ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q2ItemAcquired(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A2Q2NeedsStatus(GAME *ptGame, UNIT *hUnit);
void __fastcall A2Q2ItemDropped(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q2PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);

#undef D2_THIS_FILE
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a2q4.cpp"
void __fastcall A2Q4StaffMade(GAME *ptGame  , UNIT *hUnit
)
{
	QUEST *pQuest;
	A2Q2RECORD *pStaffRecord;
	void *pRecord;
	A2Q4RECORD *pQuestRecord;
	UNIT *hObject;

	pQuest = QuestsFind(ptGame, 9);
	if (pQuest)
	{
		pStaffRecord = (A2Q2RECORD *)pQuest->pQuestRecord;
		pStaffRecord->bStaffMade = 1;
		pStaffRecord->dwStaffMakerId = hUnit->dwId;
		pStaffRecord->nShaftCount--;
		pStaffRecord->nAmuletCount--;
		pStaffRecord->nStaffCount++;
	}
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	QuestsSendPlayerRecord(ptGame, hUnit);
	D2CommonQuestRecordSetBit(pRecord, 10, 11);
	pQuest = QuestsFind(ptGame, 11);
	if (!pQuest)
		return;
	pQuestRecord = (A2Q4RECORD *)pQuest->pQuestRecord;
	if (pQuest->bActive)
	{
		if (!pQuest->bState)
		{
#line 965
			QuestsSetState(pQuest, 1, D2_THIS_FILE, __LINE__);
		}
		if (pQuest->bStatus)
			return;
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 1, NULL, A2Q4SendStatus, TRUE);
		return;
	}
	pQuestRecord->bBlockerMode2Set = 1;
	if (!pQuestRecord->wBlockerMode)
		pQuestRecord->bBlockerMode0To2 = 1;
	pQuestRecord->wBlockerMode = 2;
	SUnitIterate(pQuest->ptGame, 0, NULL, A2Q4UpdatePlayer);
	if (pQuestRecord->bBlockerCreated != 1)
		return;
	hObject = SUnitFind(pQuest->ptGame, 2, pQuestRecord->dwBlockerId);
	if (!hObject)
		return;
	D2CommonUnitSetMode(hObject, 2);
	D2CommonUnitClearCollision(hObject);
}

BYTE __fastcall A2Q2StatusFilter(QUEST *pQuest  , UNIT *hPlayer,
	void *pGameRecord, void *pPlayerRecord, BYTE *pbStatus)
{
	UNIT *hScroll;
	UNIT *hShaft;
	UNIT *hBox;
	UNIT *hAmulet;
	UNIT *hStaff;
	DWORD dwBit3;
	A2Q2RECORD *pQuestRecord;
	QUEST *pQuest10;

	if (!D2CommonStepTestGridBit(pPlayerRecord, 7, 0))
	{
		*pbStatus = 0;
		return TRUE;
	}
	if (!D2CommonStepTestGridBit(pPlayerRecord, 10, 0)
		&& !D2CommonStepTestGridBit(pPlayerRecord, 10, 1))
	{
	*pbStatus = 0;
	hScroll = ITEMSFindByType(hPlayer, A2Q2_CODE_SCROLL);
	hShaft = ITEMSFindByType(hPlayer, A2Q2_CODE_SHAFT);
	hBox = ITEMSFindByType(hPlayer, A2Q2_CODE_BOX);
	hAmulet = ITEMSFindByType(hPlayer, A2Q2_CODE_AMULET);
	hStaff = ITEMSFindByType(hPlayer, A2Q2_CODE_STAFF);
	dwBit3 = D2CommonStepTestGridBit(pPlayerRecord, 10, 3);
	if (hShaft && hBox && hAmulet)
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 10, 6))
		{
			*pbStatus = 3;
			return TRUE;
		}
		if (dwBit3)
			*pbStatus = 2;
		else
			*pbStatus = 4;
		return TRUE;
	}
	if (hStaff)
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 10, 10))
			*pbStatus = 5;
		else
			*pbStatus = 6;
		return TRUE;
	}
	if (hScroll && !D2CommonStepTestGridBit(pPlayerRecord, 10, 3))
	{
		*pbStatus = 1;
		return TRUE;
	}
	pQuestRecord = (A2Q2RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bCubeDropped && !pQuestRecord->nCubeCount
		|| !pQuestRecord->nAmuletCount && (pQuest10 = QuestsFind(pQuest->ptGame, 10)) != NULL
			&& ((A2Q3RECORD *)pQuest10->pQuestRecord)->bObjectOperated
		|| pQuestRecord->bShaftDropped && !pQuestRecord->nShaftCount)
		*pbStatus = 9;
	if (dwBit3)
	{
		*pbStatus = 2;
		return TRUE;
	}
	if (hAmulet || hShaft || hBox)
		*pbStatus = 4;
	}
	else
	{
		if (!D2CommonStepTestGridBit(pPlayerRecord, 14, 0))
		{
			if (D2CommonStepTestGridBit(pPlayerRecord, 10, 10))
				*pbStatus = 5;
			else
				*pbStatus = 6;
			return TRUE;
		}
		if (D2CommonStepTestGridBit(pPlayerRecord, 10, 13))
			*pbStatus = 13;
		else
			*pbStatus = 11;
	}
	return TRUE;
}

#undef D2_THIS_FILE
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a2q2.cpp"

void __fastcall A2Q2Init(QUEST *pQuest  )
{
	A2Q2RECORD *pRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->ptNpcTables = sgtA2Q2Npcs;
	pQuest->bCallbacksEnabled = 1;
#line 1005
	pRecord = (A2Q2RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A2Q2RECORD));
	pQuest->pQuestRecord = pRecord;
	pQuest->eFilter = 10;
	pQuest->pStatusFilter = A2Q2StatusFilter;
	pQuest->bDoneStateMin = 0;
	pQuest->pActiveFilter = A2Q2Active;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A2Q2ChangedLevel;
	pQuest->pCallback[0] = A2Q2Talk;
	pQuest->pCallback[11] = A2Q2NpcMessage;
	pQuest->pCallback[10] = A2Q5PlayerDone;
	pQuest->pCallback[4] = A2Q2ItemAcquired;
	pQuest->pCallback[5] = A2Q2ItemDropped;
	pQuest->pCallback[8] = A2Q2UnitKilled;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A2Q2PlayerStarted;
	pQuest->pCallback[QUESTCALLBACK_PLAYERJOINEDGAME] = A2Q2PlayerJoined;
	pQuest->pCallback[QUESTCALLBACK_PLAYERDROPPEDWITHQUESTITEM] = A2Q2PlayerDropped;
	for (i = 0; i < (int)(sizeof(A2Q2RECORD) / sizeof(DWORD)); i++)
		((DWORD *)pRecord)[i] = 0;
	pQuest->bStatus = 13;
}

void __fastcall A2Q2NpcMessage(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	WORD wNpcClass;
	WORD wMessage;
	GAME *ptGame;
	void *pRecord;

	UNIT *hPlayerX;
	wNpcClass = (WORD)pqParam->field_14;
	wMessage = (WORD)pqParam->field_18;
	ptGame = pqParam->ptGame;
	hPlayerX = pqParam->hPlayer;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayerX))->apQuestRecords[ptGame->bDifficultyLevel];
	if (wNpcClass != 244)
		return;
	switch (wMessage)
	{
	case 335:
		QuestsRemoveItem(pqParam->ptGame, pqParam->hPlayer, A2Q2_CODE_SCROLL);
		D2CommonQuestRecordSetBit(pRecord, 10, 3);
		break;
	case 336:
		pQuest->bStatusFlags = 0;
		D2CommonQuestRecordSetBit(pRecord, 10, 4);
		D2CommonQuestRecordSetBit(pRecord, 10, 3);
		break;
	case 337:
		pQuest->bStatusFlags = 0;
		D2CommonQuestRecordSetBit(pRecord, 10, 5);
		D2CommonQuestRecordSetBit(pRecord, 10, 3);
		break;
	case 338:
		pQuest->bStatusFlags = 0;
		D2CommonQuestRecordSetBit(pRecord, 10, 6);
		D2CommonQuestRecordSetBit(pRecord, 10, 3);
		break;
	case 339:
		D2CommonQuestRecordClearBit(pRecord, 10, 1);
		D2CommonQuestRecordSetBit(pRecord, 10, 3);
		QuestsPlayersAdd(&pQuest->tPlayers, pqParam->hPlayer->dwId);
		D2CommonQuestRecordSetBit(pRecord, 10, 10);
		D2CommonQuestRecordSetBit(pRecord, 10, 4);
		D2CommonQuestRecordSetBit(pRecord, 10, 6);
		D2CommonQuestRecordSetBit(pRecord, 10, 5);
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		break;
	}
}

void __fastcall A2Q2Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	void *pRecord;
	DWORD dwBit13;
	DWORD dwDone;
	BYTE bItem;
	DWORD dwClass;

	ptGame = pqParam->ptGame;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	dwBit13 = D2CommonStepTestGridBit(pRecord, 10, 13);
	dwDone = D2CommonStepTestGridBit(pRecord, 10, 0);
	dwClass = pqParam->hEventUnit->dwClassId;
	if (dwClass != 244)
		return;
	if (D2CommonStepTestGridBit(pRecord, 10, 1))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, dwClass, 4);
		return;
	}
	if (A2Q2ItemState(pQuest, pqParam->hPlayer, &bItem) && bItem != 255)
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, dwClass, bItem);
		return;
	}
	if (QuestsHasPlayer(pQuest, pqParam->hPlayer->dwId))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, dwClass, 5);
		return;
	}
	if (dwDone == 1 && (!dwBit13 || dwBit13 == 1))
		return;
	if (bItem > 9 && D2CommonStepTestGridBit(pRecord, 10, 3))
		bItem = 6;
	if (bItem > 9)
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, dwClass, bItem);
}

void __fastcall A2Q2ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	if (pqParam->field_14 == 40)
		QuestsPlayerLeft(pQuest, pqParam);
}

void __fastcall A2Q2UnitKilled(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
}

static __inline void *A2Q2RecordI(UNIT *hPlayer, GAME *ptGame)
{
	return ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
}

static void A2Q2PieceAcquired(QUEST *pQuest, QUESTPARAM *pqParam, DWORD dwBit3)
{
	GAME *ptQuestGame;
	UNIT *hPlayer;

	if (!dwBit3)
	{
		pQuest->bStatus = 6;
		hPlayer = pqParam->hPlayer;
		ptQuestGame = pQuest->ptGame;
		if (A2Q2NeedsStatus(ptQuestGame, hPlayer))
			QuestsSendStatus(ptQuestGame, hPlayer, 9);
	}
	else
	{
		pQuest->bStatus = 2;
		hPlayer = pqParam->hPlayer;
		ptQuestGame = pQuest->ptGame;
		if (A2Q2NeedsStatus(ptQuestGame, hPlayer))
			QuestsSendStatus(ptQuestGame, hPlayer, 9);
	}
}

void __fastcall A2Q2ItemAcquired(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	ITEMSTXT *ptItemStats;
	GAME *ptGame;
	DWORD dwBit3;
	GAME *ptQuestGame;
	UNIT *hPlayer;

	ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(pqParam->hEventUnit->dwClassId);
	ptGame = pqParam->ptGame;
	dwBit3 = D2CommonStepTestGridBit(A2Q2RecordI(pqParam->hPlayer, ptGame), 10, 3);
	pQuest->bStatusFlags = 0;
	if (pQuest->bAct
		== D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(pqParam->hPlayer))))
		A2Q2NeedsStatus(pqParam->ptGame, pqParam->hPlayer);
	if (ptItemStats->code == A2Q2_CODE_SCROLL && !dwBit3)
	{
		pQuest->bStatus = 1;
		hPlayer = pqParam->hPlayer;
		ptQuestGame = pQuest->ptGame;
		if (A2Q2NeedsStatus(ptQuestGame, hPlayer))
			QuestsSendStatus(ptQuestGame, hPlayer, 9);
	}
	else if (ptItemStats->code == A2Q2_CODE_AMULET)
	{
		if (dwBit3)
		{
			pQuest->bStatus = 2;
			hPlayer = pqParam->hPlayer;
			ptQuestGame = pQuest->ptGame;
			if (A2Q2NeedsStatus(ptQuestGame, hPlayer))
				QuestsSendStatus(ptQuestGame, hPlayer, 9);
		}
		else
		{
			pQuest->bStatus = 6;
			hPlayer = pqParam->hPlayer;
			ptQuestGame = pQuest->ptGame;
			if (A2Q2NeedsStatus(ptQuestGame, hPlayer))
				QuestsSendStatus(ptQuestGame, hPlayer, 9);
		}
	}
	else if (ptItemStats->code == A2Q2_CODE_BOX)
		A2Q2PieceAcquired(pQuest, pqParam, dwBit3);
	else if (ptItemStats->code == A2Q2_CODE_SHAFT)
		A2Q2PieceAcquired(pQuest, pqParam, dwBit3);
}

BOOL __fastcall A2Q2NeedsStatus(GAME *ptGame  , UNIT *hUnit
)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 10, 0) || D2CommonStepTestGridBit(pRecord, 10, 15))
		&& !D2CommonStepTestGridBit(pRecord, 10, 13) && !D2CommonStepTestGridBit(pRecord, 10, 14))
		return FALSE;
	return TRUE;
}

void __fastcall A2Q2ItemDropped(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	ITEMSTXT *ptItemStats;
	GAME *ptGame;
	void *pRecord;
	UNIT *hPlayer;

	ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(pqParam->hEventUnit->dwClassId);
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (ptItemStats->code == A2Q2_CODE_AMULET && D2CommonStepTestGridBit(pRecord, 10, 4))
		D2CommonQuestRecordClearBit(pRecord, 10, 4);
	if (ptItemStats->code == A2Q2_CODE_BOX && D2CommonStepTestGridBit(pRecord, 10, 6))
		D2CommonQuestRecordClearBit(pRecord, 10, 6);
	if (ptItemStats->code == A2Q2_CODE_SHAFT && D2CommonStepTestGridBit(pRecord, 10, 5))
		D2CommonQuestRecordClearBit(pRecord, 10, 5);
}

void __fastcall A2Q2PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	void *pRecord;
	UNIT *hShaft;
	UNIT *hBox;
	UNIT *hAmulet;
	UNIT *hStaff;
	A2Q2RECORD *pQuestRecord;
	QUEST *pQuest13;

	ptGame = pqParam->ptGame;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	hShaft = ITEMSFindByType(pqParam->hPlayer, A2Q2_CODE_SHAFT);
	hBox = ITEMSFindByType(pqParam->hPlayer, A2Q2_CODE_BOX);
	hAmulet = ITEMSFindByType(pqParam->hPlayer, A2Q2_CODE_AMULET);
	hStaff = ITEMSFindByType(pqParam->hPlayer, A2Q2_CODE_STAFF);
	pQuestRecord = (A2Q2RECORD *)pQuest->pQuestRecord;
	if (hShaft)
		pQuestRecord->nShaftCount++;
	if (hAmulet)
		pQuestRecord->nAmuletCount++;
	if (hBox)
		pQuestRecord->nCubeCount++;
	if (hStaff)
		pQuestRecord->nStaffCount++;
	if (D2CommonStepTestGridBit(pRecord, 10, 0))
		return;
	pQuest13 = QuestsFind(pqParam->ptGame, 13);
	if (!pQuest13)
		return;
	if (!pQuest13->bActive)
		return;
	if (((A2Q6RECORD *)pQuest13->pQuestRecord)->bObjectsFound)
		return;
	if (ITEMSFindByType(pqParam->hPlayer, A2Q2_CODE_STAFF))
		return;
	if (hShaft && hBox && hAmulet)
		return;
	if (!hShaft)
		D2CommonQuestRecordClearBit(pRecord, 10, 5);
	if (!hBox)
		D2CommonQuestRecordClearBit(pRecord, 10, 6);
	if (!hAmulet)
		D2CommonQuestRecordClearBit(pRecord, 10, 4);
}
