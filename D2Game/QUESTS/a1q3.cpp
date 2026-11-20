#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a1q3.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../ITEMS/Items.h"
#include "../OBJECTS/ObjMode.h"
#include "QUESTS.h"
#include "a1q3.h"
#include "a1q4.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitInactive.h"

#define A1Q3_CODE_ITEM		544040040

struct A1Q3RECORD
{
 	BYTE		field_00;
 	BYTE		bObjectPresent;
 	BYTE		bStatus1Pending;
 	BYTE		bStatus13Pending;
 	DWORD		dwObjectId;
 	BYTE		field_08;
 	BYTE		unobserved_09[7];
 	BYTE		field_10;
 	BYTE		unobserved_11;
 	WORD		field_12;
 	QUESTPLAYERS	tPlayers;
 	DWORD		dwObjectMode;
 	int			nItemCount;
 	BYTE		bPartyHasItem;
 	BYTE		bStarterHasItem;
 	BYTE		unobserved_a2[2];
};
typedef struct A1Q3RECORD A1Q3RECORD;

static QUESTNPCTABLE sgtA1Q3Npcs[6] =
{
	{ { { 154, 146, { 0, 0 }, 1 } }, 1 },
	{ { { 148, 148, { 0, 0 }, 2 }, { 150, 149, { 0, 0 }, 2 }, { 265, 147, { 0, 0 }, 2 },
		{ 154, 150, { 0, 0 }, 2 }, { 147, 151, { 0, 0 }, 2 }, { 155, 153, { 0, 0 }, 2 } }, 6 },
	{ { { 150, 156, { 0, 0 }, 2 }, { 155, 159, { 0, 0 }, 2 }, { 154, 157, { 0, 0 }, 2 },
		{ 265, 154, { 0, 0 }, 2 }, { 148, 155, { 0, 0 }, 2 }, { 147, 158, { 0, 0 }, 2 } }, 6 },
	{ { { 150, 162, { 0, 0 }, 2 }, { 155, 165, { 0, 0 }, 2 }, { 154, 163, { 0, 0 }, 0 },
		{ 265, 160, { 0, 0 }, 2 }, { 148, 161, { 0, 0 }, 2 }, { 147, 164, { 0, 0 }, 2 } }, 6 },
	{ { { 150, 162, { 0, 0 }, 2 }, { 155, 165, { 0, 0 }, 2 }, { 265, 160, { 0, 0 }, 2 },
		{ 148, 161, { 0, 0 }, 2 }, { 147, 164, { 0, 0 }, 2 } }, 5 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static int sgnA1Q3NpcTable[6] = { -1, 0, 1, 2, 3, 4 };

BOOL __fastcall A1Q3SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A1Q3UpdatePlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A1Q3Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall A1Q3PartyHasItem(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A1Q3StatusFilter(QUEST *pQuest, UNIT *hPlayer, void *pGameRecord, void *pPlayerRecord,
	BYTE *pbStatus);
void __fastcall A1Q3Activate(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q3NpcMessage(QUEST *pQuestInfo, QUESTPARAM *pqParam);
void __fastcall A1Q3Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q3ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q3ItemAcquired(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q3PlayerJoined(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q3PlayerDropped(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q3PlayerLeft(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q3ItemCountDown(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A1Q3Sequence(QUEST *pQuest);
void __fastcall A1Q3PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);

void __fastcall A1Q3PartyCredit(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 3, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, 3, 1) == 1)
		return;
	if (D2CommonStatsUnitGetBaseValue(hUnit, 12) < 8)
		return;
	D2CommonQuestRecordSetBit(pRecord, 3, 13);
	D2CommonQuestRecordSetBit(pRecord, 3, 1);
}

void __fastcall A1Q3Deactivate(GAME *ptGame  , UNIT *hUnit
)
{
	void *pRecord;
	QUEST *pQuestInfo;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	D2CommonQuestRecordSetBit(pRecord, 3, 0);
	D2CommonQuestRecordClearBit(pRecord, 3, 1);
	if (D2CommonStepTestGridBit(pRecord, 3, 15))
		return;
	pQuestInfo = QuestsFind(ptGame, 3);
#line 352
	D2ASSERT(pQuestInfo != NULL);
	pQuestInfo->bCallbacksEnabled = 0;
#line 356
	QuestsDebugMessage(ptGame, "deactivated tools of trade", D2_THIS_FILE, __LINE__);
}

int __fastcall A1Q3OperateObject(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuest;
	A1Q3RECORD *pQuestRecord;
	void *pRecord;
	int nLevel;
	GAME *ptGame;
	UNIT *hPlayer;
	UNIT *hObject;
	DWORD dwMode;

	pQuest = QuestsFind(pObjectParam->ptGame, 3);
	if (!pQuest)
		return 0;
	hObject = pObjectParam->hObject;
	dwMode = hObject->dwMode;
	if (!pQuest->bActive)
	{
		D2CommonUnitSetMode(pObjectParam->hObject, 2);
		SUnitSetEvent(pObjectParam->hPlayer, 18, pObjectParam->hPlayer);
		return 0;
	}
	if (dwMode)
		return 0;
	hPlayer = pObjectParam->hPlayer;
	ptGame = pObjectParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 3, 0) == 1)
		return 0;
	if (D2CommonStepTestGridBit(pRecord, 3, 1) == 1)
		return 0;
	if (D2CommonStatsUnitGetBaseValue(pObjectParam->hPlayer, 12) < 8)
	{
		SUnitSetEvent(pObjectParam->hPlayer, 18, pObjectParam->hPlayer);
		return 0;
	}
	pObjectParam->hObject->dwDropItemCode = A1Q3_CODE_ITEM;
	if (!ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 2, &nLevel, NULL, 44, FALSE))
		return 0;
	D2CommonUnitSetMode(pObjectParam->hObject, 2);
	pQuestRecord = (A1Q3RECORD *)pQuest->pQuestRecord;
	pQuestRecord->bObjectPresent = 1;
	pQuestRecord->dwObjectMode = 2;
	pQuestRecord->dwObjectId = pObjectParam->hObject->dwId;
	pQuestRecord->nItemCount++;
	if (pQuest->bState != 4)
	{
#line 744
		QuestsSetState(pQuest, 4, D2_THIS_FILE, __LINE__);
		SUnitIterate(pQuest->ptGame, 0, NULL, A1Q3UpdatePlayer);
	}
	if (pQuest->bStatus != 1)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 1, NULL, A1Q3SendStatus, FALSE);
	}
	return 0;
}

BOOL __fastcall A1Q3SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 3, 0) || D2CommonStepTestGridBit(pRecord, 3, 15))
		&& !D2CommonStepTestGridBit(pRecord, 3, 13) && !D2CommonStepTestGridBit(pRecord, 3, 14))
		return FALSE;
	QuestsSendStatus(ptGame, hUnit, 3);
	return FALSE;
}

BOOL __fastcall A1Q3UpdatePlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	QUEST *pQuest;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 3, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 3, 1) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 3);
	if (!pQuest)
		return FALSE;
	if (pQuest->bState == 2)
	{
		D2CommonQuestRecordSetBit(pRecord, 3, 2);
		return FALSE;
	}
	if (pQuest->bState == 3 || pQuest->bState == 4)
		D2CommonQuestRecordSetBit(pRecord, 3, 3);
	return FALSE;
}

BYTE __fastcall A1Q3Active(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer  , void *pPlayerRecord)
{
	if (nNpcClass == 154)
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 3, 0) == 1)
			return FALSE;
		if (pQuest->bState == 1 && !D2CommonStepTestGridBit(pPlayerRecord, 3, 1))
			return TRUE;
		if (D2CommonStatsUnitGetBaseValue(hPlayer, 12) >= 8)
		{
			if (ITEMSFindByType(hPlayer, A1Q3_CODE_ITEM))
				return TRUE;
		}
	}
	return FALSE;
}

void __fastcall A1Q3PartyHasItem(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuest;

	if (!ITEMSFindByType(hUnit, A1Q3_CODE_ITEM))
		return;
	pQuest = QuestsFind(ptGame, 3);
	if (!pQuest)
		return;
	((A1Q3RECORD *)pQuest->pQuestRecord)->bPartyHasItem = 1;
}

BYTE __fastcall A1Q3StatusFilter(QUEST *pQuest  , UNIT *hPlayer
, void *pGameRecord, void *pPlayerRecord,
	BYTE *pbStatus)
{
	A1Q3RECORD *pQuestRecord;
	WORD wParty;
	BOOL fCarried;

	*pbStatus = 0;
	if (D2CommonStepTestGridBit(pPlayerRecord, 3, 1))
	{
		*pbStatus = 10;
		return TRUE;
	}
	fCarried = FALSE;
	if (ITEMSFindByType(hPlayer, A1Q3_CODE_ITEM))
		fCarried = TRUE;
	else
	{
		pQuestRecord = (A1Q3RECORD *)pQuest->pQuestRecord;
		wParty = SUnitGetParty(hPlayer);
		if (wParty != 65535)
		{
			pQuestRecord->bPartyHasItem = 0;
			PartyForEach(pQuest->ptGame, wParty, A1Q3PartyHasItem, NULL);
			if (pQuestRecord->bPartyHasItem)
				fCarried = TRUE;
		}
	}
	if (fCarried)
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 3, 0))
			return TRUE;
		*pbStatus = 2;
		return TRUE;
	}
	if (!pQuest->bActive)
		return TRUE;
	if (D2CommonStepTestGridBit(pPlayerRecord, 3, 13))
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 3, 1))
			*pbStatus = 10;
		else
			*pbStatus = 13;
		return TRUE;
	}
	if (D2CommonStepTestGridBit(pPlayerRecord, 3, 14))
	{
		*pbStatus = 12;
		return TRUE;
	}
	if (pQuest->bState < 5)
	{
		*pbStatus = pQuest->bStatus;
		return TRUE;
	}
	if (QuestsGetRecordBit(pQuest->ptGame, 3, 13))
	{
		if (D2CommonStatsUnitGetBaseValue(hPlayer, 12) < 8)
			*pbStatus = 4;
		else
			*pbStatus = 12;
	}
	return TRUE;
}

void __fastcall A1Q3Init(QUEST *pQuest  )
{
	A1Q3RECORD *pRecord;
	DWORD *pdw;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A1Q3ChangedLevel;
	pQuest->pCallback[10] = A1Q3PlayerLeft;
	pQuest->pCallback[QUESTCALLBACK_PLAYERDROPPEDWITHQUESTITEM] = A1Q3PlayerDropped;
	pQuest->pCallback[QUESTCALLBACK_PLAYERJOINEDGAME] = A1Q3PlayerJoined;
	pQuest->pCallback[4] = A1Q3ItemAcquired;
	pQuest->pCallback[6] = A1Q3ItemCountDown;
	pQuest->pCallback[0] = A1Q3Talk;
	pQuest->pCallback[11] = A1Q3NpcMessage;
	pQuest->pCallback[2] = A1Q3Activate;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A1Q3PlayerStarted;
	pQuest->pSequence = A1Q3Sequence;
	pQuest->ptNpcTables = sgtA1Q3Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->eFilter = 3;
#line 980
	pRecord = (A1Q3RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A1Q3RECORD));
	pQuest->pQuestRecord = pRecord;
	pQuest->bState = 0;
	pQuest->pStatusFilter = A1Q3StatusFilter;
	pQuest->bDoneStateMin = 5;
	pQuest->pActiveFilter = A1Q3Active;
	pQuest->dwNextQuestId = 6;
	pdw = (DWORD *)pRecord;
	for (i = 0; i < (int)(sizeof(A1Q3RECORD) / sizeof(DWORD)); i++)
		pdw[i] = 0;
	pRecord->field_00 = 0;
	pRecord->dwObjectId = 0;
	pRecord->field_10 = 0;
	pRecord->field_12 = 0;
	pRecord->bObjectPresent = 0;
	pRecord->field_08 = 0;
	pRecord->bStatus1Pending = 0;
	pRecord->bStatus13Pending = 0;
	pRecord->dwObjectMode = 0;
	QuestsPlayersClear(&pRecord->tPlayers);
}

void __fastcall A1Q3Activate(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q3RECORD *pQuestRecord;

	if ((WORD)pqParam->hEventUnit->dwClassId != 154)
		return;
	pQuestRecord = (A1Q3RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bStatus1Pending == 1)
	{
		SUnitIterate(pQuest->ptGame, 0, NULL, A1Q3UpdatePlayer);
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 1, NULL, A1Q3SendStatus, TRUE);
		pQuestRecord->bStatus1Pending = 0;
		return;
	}
	if (pQuestRecord->bStatus13Pending == 1)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 13, NULL, A1Q3SendStatus, TRUE);
		pQuestRecord->bStatus13Pending = 0;
	}
}

void __fastcall A1Q3NpcMessage(QUEST *pQuestInfo  , QUESTPARAM *pqParam
)
{
	A1Q3RECORD *pQuestRecord;
	WORD wMessage;
	WORD wNpcClass;
	UNIT *hItem;
	void *pRecord;
	WORD wParty;
	QUEST *pNextQuest;
	GAME *ptGame;
	UNIT *hPlayer;

	wMessage = (WORD)pqParam->field_18;
	wNpcClass = (WORD)pqParam->field_14;
	hItem = ITEMSFindByType(pqParam->hPlayer, A1Q3_CODE_ITEM);
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (wNpcClass != 154)
		return;
	pQuestRecord = (A1Q3RECORD *)pQuestInfo->pQuestRecord;
	if (wMessage == 163)
	{
		if (D2CommonStepTestGridBit(pRecord, 3, 0))
			return;
		pQuestRecord = (A1Q3RECORD *)pQuestInfo->pQuestRecord;
		QuestsPlayersAdd(&pQuestRecord->tPlayers, pqParam->hPlayer->dwId);
		if (hItem)
		{
			D2CommonQuestRecordSetBit(pRecord, 3, 13);
			D2CommonQuestRecordSetBit(pRecord, 3, 1);
			QuestsSendPlayerRecord(pQuestInfo->ptGame, pqParam->hPlayer);
			QuestsRemoveItem(pqParam->ptGame, pqParam->hPlayer, A1Q3_CODE_ITEM);
			pQuestRecord->nItemCount--;
			wParty = SUnitGetParty(pqParam->hPlayer);
			if (wParty != 65535)
				PartyForEach(pqParam->ptGame, wParty, A1Q3PartyCredit, NULL);
			if (pQuestInfo->bActive)
			{
				if (pQuestInfo->bState == 4)
				{
#line 247
					QuestsSetState(pQuestInfo, 5, D2_THIS_FILE, __LINE__);
					pQuestRecord->bStatus13Pending = 1;
					QuestsSetRecordBit(pQuestInfo->ptGame, 3, 13);
#line 250
					D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
					pQuestInfo->pSequence(pQuestInfo);
				}
				else if (pQuestRecord->bStarterHasItem)
				{
					pNextQuest = QuestsFind(pQuestInfo->ptGame, (int)pQuestInfo->dwNextQuestId);
					if (pNextQuest)
					{
#line 258
						D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
						pNextQuest->pSequence(pNextQuest);
					}
				}
			}
		}
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		return;
	}
	if (wMessage == 146)
	{
#line 268
		QuestsSetState(pQuestInfo, 2, D2_THIS_FILE, __LINE__);
		pQuestRecord->bStatus1Pending = 1;
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
	}
}

void __fastcall A1Q3Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	DWORD dwCredited;
	DWORD dwDone;
	int nNpcClass;
	int nTable;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	dwCredited = D2CommonStepTestGridBit(pRecord, 3, 13);
	dwDone = D2CommonStepTestGridBit(pRecord, 3, 0);
	nNpcClass = (int)pqParam->hEventUnit->dwClassId;
	if (dwDone == 1 && !dwCredited)
		return;
	if (ITEMSFindByType(pqParam->hPlayer, A1Q3_CODE_ITEM))
	{
		if (D2CommonStatsUnitGetBaseValue(pqParam->hPlayer, 12) < 8)
			return;
		if (D2CommonStepTestGridBit(pRecord, 3, 0))
			return;
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 3);
		return;
	}
	if (dwDone == 1)
		return;
	if (!pQuest->bState || pQuest->bState == 4)
		return;
	nTable = sgnA1Q3NpcTable[pQuest->bState];
	if (nTable != -1 && (DWORD)nTable < 6)
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, nTable);
}

void __fastcall A1Q3ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	DWORD dwOne;
	GAME *ptGame;
	UNIT *hPlayer;

	dwOne = pqParam->field_14;
	if (!pQuest->bActive)
	{
		pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = NULL;
		return;
	}
	if (dwOne != 1)
		return;
	if (pQuest->bState != 2)
		return;
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 1) == 1)
		return;
	if (pQuest->bStatus != 1)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 1, NULL, A1Q3SendStatus, TRUE);
	}
#line 393
	QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
	SUnitIterate(pQuest->ptGame, 0, NULL, A1Q3UpdatePlayer);
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = NULL;
}

void __fastcall A1Q3ItemAcquired(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecord, 3, 6))
	{
		D2CommonQuestRecordSetBit(pRecord, 3, 6);
		SUnitSetEvent(pqParam->hPlayer, 34, pqParam->hPlayer);
	}
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 2, NULL, A1Q3SendStatus, TRUE);
}

void __fastcall A1Q3PlayerJoined(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q3RECORD *pQuestRecord;

	if (!ITEMSFindByType(pqParam->hPlayer, A1Q3_CODE_ITEM))
		return;
	if (!pQuest->bActive)
		return;
	if (pQuest->bState == 5)
		return;
	pQuestRecord = (A1Q3RECORD *)pQuest->pQuestRecord;
	if (++pQuestRecord->nItemCount != 1)
		return;
	if (!pQuestRecord->bObjectPresent)
		return;
	if (pQuestRecord->dwObjectMode == 2)
		pQuest->bStatus = 2;
}

void __fastcall A1Q3PlayerDropped(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q3RECORD *pQuestRecord;

	if (!pQuest->bActive)
		return;
	if (pQuest->bState == 5)
		return;
	pQuestRecord = (A1Q3RECORD *)pQuest->pQuestRecord;
	if (--pQuestRecord->nItemCount)
		return;
	if (!pQuestRecord->bObjectPresent)
		return;
	if (pQuestRecord->dwObjectMode != 2)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 3, NULL, A1Q3SendStatus, TRUE);
}

void __fastcall A1Q3PlayerLeft(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	QuestsPlayersRemove(&((A1Q3RECORD *)pQuest->pQuestRecord)->tPlayers, pqParam->hPlayer->dwId);
}

void __fastcall A1Q3ItemCountDown(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q3RECORD *pQuestRecord;
	UNIT *hObject;

	if (!pQuest->bActive)
		return;
	pQuestRecord = (A1Q3RECORD *)pQuest->pQuestRecord;
	if (--pQuestRecord->nItemCount)
		return;
	if (pQuestRecord->dwObjectMode != 2)
		return;
	if (!pQuest->bActive)
		return;
	pQuestRecord = (A1Q3RECORD *)pQuest->pQuestRecord;
	pQuestRecord->dwObjectMode = 0;
	if (!pQuestRecord->bObjectPresent)
		return;
#line 456
	QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
	pQuest->bStatusFlags = 1;
	QuestsSetStatus(pQuest, 1, NULL, A1Q3SendStatus, TRUE);
	pQuestRecord->field_08 = 0;
	if (pQuestRecord->bObjectPresent != 1)
		return;
	hObject = SUnitFind(pQuest->ptGame, 2, pQuestRecord->dwObjectId);
	if (hObject && hObject->dwClassId == 108)
	{
		D2CommonUnitSetMode(hObject, 0);
		return;
	}
	pQuestRecord->bObjectPresent = 0;
}

BYTE __fastcall A1Q3Sequence(QUEST *pQuestInfo  )
{
	QUEST *pNextQuest;

	if (!pQuestInfo->bState && pQuestInfo->bActive == 1)
	{
#line 896
		QuestsSetState(pQuestInfo, 1, D2_THIS_FILE, __LINE__);
		return TRUE;
	}
	if (pQuestInfo->bState != 5 && pQuestInfo->bActive)
		return TRUE;
	pNextQuest = QuestsFind(pQuestInfo->ptGame, (int)pQuestInfo->dwNextQuestId);
	if (pNextQuest)
	{
#line 904
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		return pNextQuest->pSequence(pNextQuest);
	}
	return FALSE;
}

void __fastcall A1Q3PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q3RECORD *pQuestRecord;
	void *pRecord;
	GAME *ptGame;
	UNIT *hPlayer;

	if (ITEMSFindByType(pqParam->hPlayer, A1Q3_CODE_ITEM))
	{
		pQuestRecord = (A1Q3RECORD *)pQuest->pQuestRecord;
		pQuestRecord->bStarterHasItem = 1;
		pQuestRecord->nItemCount++;
	}
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0))
		return;
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 15))
		return;
	if (D2CommonStepTestGridBit(pRecord, 3, 2) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 2;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 3, 3) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 3;
	}
}

void __fastcall A1Q3ObjInit(QUEST *pQuest  , OBJINIT *ptInit
)
{
	A1Q3RECORD *pQuestRecord;

	if (!pQuest)
		return;
	pQuestRecord = (A1Q3RECORD *)pQuest->pQuestRecord;
	pQuestRecord->bObjectPresent = 1;
	pQuestRecord->dwObjectId = ptInit->hObject->dwId;
	if (!pQuest->bActive)
		pQuestRecord->dwObjectMode = 2;
	D2CommonUnitSetMode(ptInit->hObject, pQuestRecord->dwObjectMode);
}

void __fastcall A1Q4SetDone(QUEST *pQuest  , BYTE bTimer,
	BYTE bNotify)
{
	A1Q4RECORD *pQuestRecord;
	UNIT *hObject;

	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bGibbetPresent == 1)
	{
		hObject = SUnitFind(pQuest->ptGame, 2, pQuestRecord->dwGibbetId);
		if (hObject && hObject->dwClassId == 26)
		{
			if (hObject->dwMode == 3)
				goto objects;
			D2CommonUnitSetMode(hObject, 3);
		}
	}
	pQuestRecord->dwGibbetMode = 3;
objects:
	if (pQuestRecord->bTreePresent == 1)
	{
		hObject = SUnitFind(pQuest->ptGame, 2, pQuestRecord->dwTreeId);
		if (hObject)
			D2CommonUnitSetMode(hObject, 1);
	}
	if (bTimer)
	{
		if (!pQuestRecord->bCainTimerPending)
		{
			pQuestRecord->bCainTimerPending = 1;
			QuestsAddTimer(pQuest, A1Q4CainTimer, 1);
		}
	}
	else
	{
		if (!bNotify)
			goto done;
		SUnitIterate(pQuest->ptGame, 1, NULL, A1Q4FreeCain);
	}
	if (bNotify)
	{
		if (!pQuestRecord->bCainFreed)
			SUnitInactiveRemoveClass(pQuest->ptGame, 1, 146, 0);
		if (!pQuestRecord->bTownCainMade)
			pQuestRecord->bTownCainWanted = 1;
	}
done:
	pQuestRecord->bQuestDone = 1;
}
