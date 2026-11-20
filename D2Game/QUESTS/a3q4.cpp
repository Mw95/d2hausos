#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a3q4.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../ITEMS/Items.h"
#include "QUESTS.h"
#include "a3q3.h"
#include "a3q4.h"
#include "a2q5.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"

#define A3Q4_CODE_BIRD		540291943
#define A3Q4_CODE_FIGURINE	540291946
#define A3Q4_CODE_POTION	544897400

static QUESTNPCTABLE sgtA3Q4Npcs[9] =
{
	{ { { 245, 527, { 0, 0 }, 1 }, { 252, 528, { 0, 0 }, 2 } }, 2 },
	{ { { 264, 529, { 0, 0 }, 1 } }, 1 },
	{ { { 245, 531, { 0, 0 }, 2 }, { 254, 534, { 0, 0 }, 1 }, { 297, 530, { 0, 0 }, 2 },
		{ 253, 532, { 0, 0 }, 2 }, { 252, 533, { 0, 0 }, 2 } }, 5 },
	{ { { 245, 531, { 0, 0 }, 1 }, { 253, 532, { 0, 0 }, 2 }, { 252, 533, { 0, 0 }, 2 } }, 3 },
	{ { { 254, 534, { 0, 0 }, 2 }, { 255, 535, { 0, 0 }, 2 }, { 253, 536, { 0, 0 }, 2 },
		{ 297, 537, { 0, 0 }, 2 } }, 4 },
	{ { { 254, 538, { 0, 0 }, 1 }, { 264, 539, { 0, 0 }, 2 }, { 245, 540, { 0, 0 }, 2 },
		{ 255, 541, { 0, 0 }, 2 } }, 4 },
	{ { { 254, 538, { 0, 0 }, 2 }, { 264, 539, { 0, 0 }, 2 }, { 245, 540, { 0, 0 }, 2 },
		{ 255, 541, { 0, 0 }, 2 }, { 297, 542, { 0, 0 }, 2 } }, 5 },
	{ { { 245, 527, { 0, 0 }, 2 }, { 252, 528, { 0, 0 }, 2 } }, 2 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

BYTE __fastcall A3Q4StatusFilter(QUEST *pQuest, UNIT *hPlayer, void *pGameRecord, void *pPlayerRecord,
	BYTE *pbStatus);
void __fastcall A3Q4FlagParty(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A3Q4Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q4ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q4NpcMessage(QUEST *pQuestInfo, QUESTPARAM *pqParam);
void __fastcall A3Q4Activate(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A3Q4SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A3Q4ItemAcquired(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A3Q4Sequence(QUEST *pQuestInfo);
void __fastcall A3Q4PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q4PlayerDropped(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q4PlayerJoined(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q4UnitKilled(QUEST *pQuest, QUESTPARAM *pqParam);

BYTE __fastcall A3Q4StatusFilter(QUEST *pQuest  , UNIT *hPlayer,
	void *pGameRecord, void *pPlayerRecord, BYTE *pbStatus)
{
	UNIT *hBird;
	UNIT *hFigurine;
	BYTE bStatus;

	if (!D2CommonStepTestGridBit(pPlayerRecord, 15, 0))
	{
		*pbStatus = 0;
		return TRUE;
	}
	if (D2CommonStepTestGridBit(pPlayerRecord, 20, 1))
	{
		*pbStatus = 5;
		return TRUE;
	}
	if (D2CommonStepTestGridBit(pPlayerRecord, 20, 0))
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 20, 13))
			*pbStatus = 13;
		else
			*pbStatus = 11;
		return TRUE;
	}
	*pbStatus = 0;
	hBird = ITEMSFindByType(hPlayer, A3Q4_CODE_BIRD);
	hFigurine = ITEMSFindByType(hPlayer, A3Q4_CODE_FIGURINE);
	if (hBird)
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 20, 4))
			*pbStatus = 4;
		else
			*pbStatus = 3;
		return TRUE;
	}
	if (hFigurine)
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 20, 2))
			*pbStatus = 2;
		else
			*pbStatus = 1;
		return TRUE;
	}
	if (pQuest->bActive)
	{
		bStatus = pQuest->bStatus;
		if (bStatus >= 6)
		{
			if (pQuest->ptGame->bGameType != 3)
				bStatus = 7;
			else
				bStatus = 6;
		}
		*pbStatus = bStatus;
		return TRUE;
	}
	return TRUE;
}

void __fastcall A3Q4FlagParty(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuest;
	void *pRecord;

	pQuest = QuestsFind(ptGame, 18);
	if (!pQuest)
		return;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 20, 0))
		return;
	if (D2CommonStepTestGridBit(pRecord, 20, 1))
		return;
	D2CommonQuestRecordSetBit(pRecord, 20, ((A3Q4RECORD *)pQuest->pQuestRecord)->dwRecordBit);
}

void __fastcall A3Q4Init(QUEST *pQuest  )
{
	A3Q4RECORD *pQuestRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->ptNpcTables = sgtA3Q4Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->bState = 0;
	pQuest->eFilter = 20;
	pQuest->pActiveFilter = A3Q4Active;
	pQuest->bDoneStateMin = 0;
	pQuest->pStatusFilter = A3Q4StatusFilter;
	pQuest->pSequence = A3Q4Sequence;
	pQuest->dwNextQuestId = 16;
	pQuest->pCallback[4] = A3Q4ItemAcquired;
	pQuest->pCallback[0] = A3Q4Talk;
	pQuest->pCallback[11] = A3Q4NpcMessage;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A3Q4ChangedLevel;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A3Q4PlayerStarted;
	pQuest->pCallback[10] = A2Q5PlayerDone;
	pQuest->pCallback[QUESTCALLBACK_PLAYERJOINEDGAME] = A3Q4PlayerJoined;
	pQuest->pCallback[QUESTCALLBACK_PLAYERDROPPEDWITHQUESTITEM] = A3Q4PlayerDropped;
	QuestsPlayersClear(&pQuest->tPlayers);
#line 821
	pQuestRecord = (A3Q4RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A3Q4RECORD));
	pQuest->pQuestRecord = pQuestRecord;
	for (i = 0; i < (int)(sizeof(A3Q4RECORD) / sizeof(DWORD)); i++)
		((DWORD *)pQuestRecord)[i] = 0;
	pQuestRecord->bFigurinePending = 1;
	pQuestRecord->bPickAllowed = 1;
}

void __fastcall A3Q4Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	int nNpcClass;
	DWORD dwBit0;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	dwBit0 = D2CommonStepTestGridBit(pRecord, 20, 0);
	nNpcClass = (int)pqParam->hEventUnit->dwClassId;
	if (dwBit0 == 1)
	{
		if (QuestsHasPlayer(pQuest, pqParam->hPlayer->dwId))
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 6);
		return;
	}
	if (nNpcClass == 254 && ((A3Q4RECORD *)pQuest->pQuestRecord)->bNpcSkillPending == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, 20, 1))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 5);
		return;
	}
	if (ITEMSFindByType(pqParam->hPlayer, A3Q4_CODE_BIRD))
	{
		if (nNpcClass == 245 && !D2CommonStepTestGridBit(pRecord, 20, 4))
		{
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 3);
			return;
		}
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 2);
		return;
	}
	if (!ITEMSFindByType(pqParam->hPlayer, A3Q4_CODE_FIGURINE))
		return;
	if (!D2CommonStepTestGridBit(pRecord, 20, 2))
	{
		if (nNpcClass == 245 || nNpcClass == 252)
		{
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 0);
			return;
		}
	}
	else if (nNpcClass == 245 || nNpcClass == 252)
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 7);
		return;
	}
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 1);
}

void __fastcall A3Q4ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	if (pqParam->field_14 == 75)
		QuestsPlayerLeft(pQuest, pqParam);
}

void __fastcall A3Q4NpcMessage(QUEST *pQuestInfo  , QUESTPARAM *pqParam
)
{
	A3Q4RECORD *pQuestRecord;
	void *pRecord;
	WORD wNpcClass;
	WORD wMessage;
	WORD wParty;
	GAME *ptGame;
	UNIT *hPlayer;

	wNpcClass = (WORD)pqParam->field_14;
	wMessage = (WORD)pqParam->field_18;
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 20, 0) == 1)
		return;
	pQuestRecord = (A3Q4RECORD *)pQuestInfo->pQuestRecord;
	if (wNpcClass == 245)
	{
		if (wMessage == 527)
		{
			D2CommonQuestRecordSetBit(pRecord, 20, 2);
			if (pQuestInfo->bState == 1)
			{
#line 468
				QuestsSetState(pQuestInfo, 2, D2_THIS_FILE, __LINE__);
				pQuestRecord->bStatus2Pending = 1;
				pQuestInfo->pCallback[2] = A3Q4Activate;
			}
			QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
			pQuestRecord->dwRecordBit = 2;
			hPlayer = pqParam->hPlayer;
			ptGame = pqParam->ptGame;
			wParty = SUnitGetParty(hPlayer);
			if (wParty != 65535)
				PartyForEach(ptGame, wParty, A3Q4FlagParty, NULL);
			return;
		}
		if (wMessage != 531)
			return;
		D2CommonQuestRecordSetBit(pRecord, 20, 4);
		pQuestRecord->bStatus4Pending = 1;
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		pQuestRecord->dwRecordBit = 4;
		hPlayer = pqParam->hPlayer;
		ptGame = pqParam->ptGame;
		wParty = SUnitGetParty(hPlayer);
		if (wParty != 65535)
			PartyForEach(ptGame, wParty, A3Q4FlagParty, NULL);
		return;
	}
	if (wNpcClass == 254)
	{
		if (wMessage == 534)
		{
			if (!ITEMSFindByType(pqParam->hPlayer, A3Q4_CODE_BIRD))
				return;
			if (D2CommonStepTestGridBit(pRecord, 20, 1))
				return;
			if (D2CommonStepTestGridBit(pRecord, 20, 0))
				return;
			QuestsRemoveItem(pqParam->ptGame, pqParam->hPlayer, A3Q4_CODE_BIRD);
			pQuestRecord->nItemCount--;
			if (pQuestInfo->bActive && pQuestInfo->bState != 4)
			{
#line 499
				QuestsSetState(pQuestInfo, 4, D2_THIS_FILE, __LINE__);
			}
			D2CommonQuestRecordSetBit(pRecord, 20, 1);
			pQuestRecord->bStatus5Pending = 1;
			pQuestInfo->pCallback[2] = A3Q4Activate;
			pQuestRecord->dwRecordBit = 1;
			hPlayer = pqParam->hPlayer;
			ptGame = pqParam->ptGame;
			wParty = SUnitGetParty(hPlayer);
			if (wParty != 65535)
				PartyForEach(ptGame, wParty, A3Q4FlagParty, NULL);
			return;
		}
		if (wMessage != 538)
			return;
		if (D2CommonStepTestGridBit(pRecord, 20, 0))
			return;
		if (D2CommonStepTestGridBit(pRecord, 20, 1))
		{
			D2CommonQuestRecordClearBit(pRecord, 20, 1);
			D2CommonQuestRecordSetBit(pRecord, 20, 0);
			D2CommonQuestRecordSetBit(pRecord, 20, 13);
			D2CommonQuestRecordClearMidBits(pRecord, pQuestInfo->eFilter);
			D2CommonQuestRecordSetBit(pRecord, 20, 5);
			if (pQuestInfo->bActive)
			{
#line 526
				QuestsSetState(pQuestInfo, 5, D2_THIS_FILE, __LINE__);
			}
			QuestsCreateItem(pqParam->ptGame, pqParam->hPlayer, A3Q4_CODE_POTION, 0, 2);
			QuestsPlayersAdd(&pQuestInfo->tPlayers, pqParam->hPlayer->dwId);
		}
		if (!D2CommonStepTestGridBit(pRecord, 20, 13))
			return;
		QuestsSetRecordBit(pqParam->ptGame, 20, 13);
#line 534
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		pQuestInfo->pSequence(pQuestInfo);
		return;
	}
	if (wNpcClass != 264)
		return;
	if (wMessage != 529)
		return;
	if (!ITEMSFindByType(pqParam->hPlayer, A3Q4_CODE_FIGURINE))
		return;
	pQuestInfo->pCallback[2] = A3Q4Activate;
	QuestsRemoveItem(pqParam->ptGame, pqParam->hPlayer, A3Q4_CODE_FIGURINE);
	pQuestRecord->nItemCount--;
	if (!QuestsCreateItem(pqParam->ptGame, pqParam->hPlayer, A3Q4_CODE_BIRD, 0, 2))
		return;
	pQuestRecord->nItemCount++;
#line 555
	QuestsSetState(pQuestInfo, 3, D2_THIS_FILE, __LINE__);
	pQuestRecord->bStatus3Pending = 1;
	pQuestInfo->pCallback[2] = A3Q4Activate;
}

void __fastcall A3Q4Activate(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q4RECORD *pQuestRecord;
	WORD wNpcClass;

	wNpcClass = (WORD)pqParam->hEventUnit->dwClassId;
	pQuestRecord = (A3Q4RECORD *)pQuest->pQuestRecord;
	if (wNpcClass == 254)
	{
		if (pQuestRecord->bStatus5Pending != 1)
			return;
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 5, NULL, A3Q4SendStatus, TRUE);
		pQuestRecord->bStatus5Pending = 0;
		pQuestRecord->bNpcSkillPending = 1;
		return;
	}
	if (wNpcClass == 245)
	{
		if (pQuestRecord->bStatus2Pending == 1)
		{
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 2, NULL, A3Q4SendStatus, TRUE);
			pQuestRecord->bStatus2Pending = 0;
		}
		if (pQuestRecord->bStatus4Pending != 1)
			return;
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 4, NULL, A3Q4SendStatus, TRUE);
		pQuestRecord->bStatus4Pending = 0;
		return;
	}
	if (wNpcClass != 264)
		return;
	if (pQuestRecord->bStatus3Pending != 1)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 3, NULL, A3Q4SendStatus, TRUE);
	pQuestRecord->bStatus3Pending = 0;
}

BOOL __fastcall A3Q4SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 20, 0) || D2CommonStepTestGridBit(pRecord, 20, 15))
		&& !D2CommonStepTestGridBit(pRecord, 20, 13)
		&& !D2CommonStepTestGridBit(pRecord, 20, 14))
		return FALSE;
	QuestsSendStatus(ptGame, hUnit, 18);
	return FALSE;
}

void __fastcall A3Q4ItemAcquired(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	DWORD dwCode;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 20, 0))
		return;
	if (D2CommonStepTestGridBit(pRecord, 20, 1))
		return;
#line 594
	QuestsDebugMessage(pQuest->ptGame, "something acquired", D2_THIS_FILE, __LINE__);
	dwCode = ((ITEMSTXT *)D2CommonItemTblsGetItemStats(pqParam->hEventUnit->dwClassId))->code;
	if (dwCode == A3Q4_CODE_BIRD)
	{
#line 600
		QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
		QuestsSetStatus(pQuest, 3, NULL, A3Q4SendStatus, TRUE);
		return;
	}
	if (dwCode != A3Q4_CODE_FIGURINE)
		return;
	QuestsSetStatus(pQuest, 1, NULL, A3Q4SendStatus, TRUE);
#line 611
	QuestsSetState(pQuest, 1, D2_THIS_FILE, __LINE__);
	if (D2CommonStepTestGridBit(pRecord, 20, 6))
		return;
	D2CommonQuestRecordSetBit(pRecord, 20, 6);
	SUnitSetEvent(pqParam->hPlayer, 70, pqParam->hPlayer);
}

BYTE __fastcall A3Q4Sequence(QUEST *pQuestInfo  )
{
	QUEST *pNextQuest;
	BYTE bNext;

	if (pQuestInfo->bState != 5 && pQuestInfo->bActive)
		return TRUE;
	pNextQuest = QuestsFind(pQuestInfo->ptGame, (int)pQuestInfo->dwNextQuestId);
	bNext = FALSE;
	if (pNextQuest)
	{
#line 633
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		bNext = pNextQuest->pSequence(pNextQuest);
	}
	pNextQuest = QuestsFind(pQuestInfo->ptGame, 17);
	if (pNextQuest) {
#line 639
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		return pNextQuest->pSequence(pNextQuest) || bNext; }
	return bNext;
}

void __fastcall A3Q4PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q4RECORD *pQuestRecord;
	void *pRecord;
	UNIT *hBird;
	UNIT *hFigurine;
	GAME *ptGame;
	UNIT *hPlayer;

	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	pQuestRecord = (A3Q4RECORD *)pQuest->pQuestRecord;
	if (D2CommonStepTestGridBit(pRecord, 20, 0) || D2CommonStepTestGridBit(pRecord, 20, 1))
		goto inactive;
	hBird = ITEMSFindByType(pqParam->hPlayer, A3Q4_CODE_BIRD);
	hFigurine = ITEMSFindByType(pqParam->hPlayer, A3Q4_CODE_FIGURINE);
	if (D2CommonStepTestGridBit(pRecord, 20, 1))
	{
		pQuest->bActive = 0;
		pQuestRecord->bPickAllowed = 0;
		pQuestRecord->bFigurinePending = 0;
		return;
	}
	if (hBird)
	{
		pQuestRecord->bPickAllowed = 0;
		pQuestRecord->bFigurinePending = 0;
		if (D2CommonStepTestGridBit(pRecord, 20, 4))
		{
			pQuest->bState = 3;
			pQuest->bStatus = 4;
			return;
		}
		pQuest->bState = 3;
		pQuest->bStatus = 3;
		return;
	}
	if (!hFigurine)
		return;
	pQuestRecord->bPickAllowed = 0;
	pQuestRecord->bFigurinePending = 0;
	if (D2CommonStepTestGridBit(pRecord, 20, 2))
	{
		pQuest->bState = 2;
		pQuest->bStatus = 2;
		return;
	}
	pQuest->bState = 1;
	pQuest->bStatus = 1;
	return;
inactive:
	pQuest->bActive = 0;
	pQuestRecord->bPickAllowed = 0;
	pQuestRecord->bFigurinePending = 0;
}

void __fastcall A3Q4PlayerDropped(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q4RECORD *pQuestRecord;

	pQuestRecord = (A3Q4RECORD *)pQuest->pQuestRecord;
	if (--pQuestRecord->nItemCount)
		return;
	if (!pQuest->bActive)
		return;
	if (!pQuestRecord->bFigurineDropped)
		return;
	if (pQuest->bState >= 4)
		return;
	pQuestRecord->bItemLost = 1;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 6, NULL, A3Q4SendStatus, TRUE);
}

void __fastcall A3Q4PlayerJoined(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q4RECORD *pQuestRecord;
	BYTE fBird;
	int nStatus;

	fBird = FALSE;
	pQuestRecord = (A3Q4RECORD *)pQuest->pQuestRecord;
	if (ITEMSFindByType(pqParam->hPlayer, A3Q4_CODE_FIGURINE))
		pQuestRecord->nItemCount++;
	if (ITEMSFindByType(pqParam->hPlayer, A3Q4_CODE_BIRD))
	{
		fBird = TRUE;
		pQuestRecord->nItemCount++;
	}
	if (pQuestRecord->nItemCount != 1)
		return;
	if (!pQuest->bActive)
		return;
	if (!pQuestRecord->bItemLost)
		return;
	pQuestRecord->bItemLost = 0;
	pQuest->bStatusFlags = 0;
	nStatus = 1;
	if (fBird)
		nStatus = 3;
	QuestsSetStatus(pQuest, nStatus, NULL, A3Q4SendStatus, TRUE);
}

BOOL __fastcall A3Q4MonsterAct(QUEST *pQuest  , UNIT *hMonster)
{
	A3Q4RECORD *pQuestRecord;
	int nClass = (int)hMonster->dwClassId;

	pQuestRecord = (A3Q4RECORD *)pQuest->pQuestRecord;
	if (!pQuest->bActive || !pQuestRecord->bFigurinePending || pQuestRecord->bPickAllowed != 1)
		return FALSE;
	if (nClass >= 141 && nClass <= 145)
		return FALSE;
	QuestsAttachUnit(pQuest->ptGame, hMonster, 18);
	pQuest->pCallback[8] = A3Q4UnitKilled;
	pQuestRecord->bPickAllowed = 0;
	pQuestRecord->bMonsterPicked = 1;
	pQuestRecord->dwPickedMonsterId = hMonster->dwId;
	return TRUE;
}

void __fastcall A3Q4UnitKilled(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q4RECORD *pQuestRecord;
	int nLevel;

#line 841
	QuestsDebugMessage(pQuest->ptGame, "killed boss for quest", D2_THIS_FILE, __LINE__);
	pQuestRecord = (A3Q4RECORD *)pQuest->pQuestRecord;
	if (!pQuest->bActive || !pQuestRecord->bMonsterPicked || !pQuestRecord->bFigurinePending)
		return;
	pqParam->hEventUnit->dwDropItemCode = A3Q4_CODE_FIGURINE;
	if (ITEMSDropFromUnit(pQuest->ptGame, pqParam->hEventUnit, 2, &nLevel, NULL, 44, FALSE))
	{
		pQuest->pCallback[8] = NULL;
		pQuestRecord->bFigurinePending = 0;
		pQuestRecord->bFigurineDropped = 1;
		pQuestRecord->nItemCount++;
		if (pQuest->bState < 1)
		{
#line 860
			QuestsSetState(pQuest, 1, D2_THIS_FILE, __LINE__);
		}
		if (!pQuest->bStatus)
			QuestsSetStatus(pQuest, 1, NULL, A3Q4SendStatus, TRUE);
	}
	else
		pQuestRecord->bPickAllowed = 1;
	pQuestRecord->bMonsterPicked = 0;
}

void __fastcall A3Q4UnitAct(QUEST *pQuest  , UNIT *hUnit
)
{
	A3Q4RECORD *pQuestRecord;

	pQuestRecord = (A3Q4RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bMonsterPicked != 1)
		return;
	if (pQuestRecord->dwPickedMonsterId != hUnit->dwId)
		return;
	pQuestRecord->bMonsterPicked = 0;
	pQuestRecord->bPickAllowed = 1;
}

BOOL __fastcall A3Q4TestFlag(GAME *ptGame  )
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptGame, 18);
	if (pQuest && ((A3Q4RECORD *)pQuest->pQuestRecord)->bNpcSkillPending)
		return TRUE;
	return FALSE;
}

void __fastcall A3Q4ClearFlag(GAME *ptGame  )
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptGame, 18);
	if (pQuest)
		((A3Q4RECORD *)pQuest->pQuestRecord)->bNpcSkillPending = 0;
}
