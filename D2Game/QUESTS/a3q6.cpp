#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a3q6.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "../GAME/Event.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/MonsterUnique.h"
#include "QUESTS.h"
#include "a2q3.h"
#include "a3q5.h"
#include "a3q6.h"
#include "a2q5.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"

#define A3Q6_CODE_MSS		544437101

#define A3Q6_MONSTER		297

static QUESTNPCTABLE sgtA3Q6Npcs[8] =
{
	{ { { 255, 628, { 0, 0 }, 1 } }, 1 },
	{ { { 254, 629, { 0, 0 }, 2 }, { 255, 631, { 0, 0 }, 2 }, { 264, 633, { 0, 0 }, 2 },
		{ 252, 635, { 0, 0 }, 2 }, { 253, 637, { 0, 0 }, 2 }, { 245, 639, { 0, 0 }, 2 },
		{ 297, 641, { 0, 0 }, 2 } }, 7 },
	{ { { 254, 630, { 0, 0 }, 2 }, { 255, 632, { 0, 0 }, 2 }, { 264, 634, { 0, 0 }, 2 },
		{ 253, 638, { 0, 0 }, 2 }, { 245, 640, { 0, 0 }, 2 }, { 297, 642, { 0, 0 }, 2 } }, 6 },
	{ { { 254, 643, { 0, 0 }, 2 }, { 255, 645, { 0, 0 }, 2 }, { 264, 647, { 0, 0 }, 2 },
		{ 252, 649, { 0, 0 }, 2 }, { 253, 651, { 0, 0 }, 2 }, { 245, 653, { 0, 0 }, 2 },
		{ 297, 655, { 0, 0 }, 2 } }, 7 },
	{ { { 254, 644, { 0, 0 }, 2 }, { 255, 646, { 0, 0 }, 2 }, { 264, 648, { 0, 0 }, 2 },
		{ 252, 650, { 0, 0 }, 2 }, { 253, 652, { 0, 0 }, 2 }, { 245, 654, { 0, 0 }, 2 },
		{ 297, 656, { 0, 0 }, 2 } }, 7 },
	{ { { 254, 657, { 0, 0 }, 1 }, { 255, 658, { 0, 0 }, 1 }, { 264, 659, { 0, 0 }, 1 },
		{ 252, 660, { 0, 0 }, 1 }, { 253, 661, { 0, 0 }, 1 }, { 245, 662, { 0, 0 }, 1 },
		{ 297, 663, { 0, 0 }, 1 } }, 7 },
	{ { { 254, 657, { 0, 0 }, 2 }, { 255, 658, { 0, 0 }, 2 }, { 264, 659, { 0, 0 }, 2 },
		{ 252, 660, { 0, 0 }, 2 }, { 253, 661, { 0, 0 }, 2 }, { 245, 662, { 0, 0 }, 2 },
		{ 297, 663, { 0, 0 }, 2 } }, 7 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static int sgnA3Q6NpcTable[8] = { -1, 0, 1, 2, 3, 4, 5, 6 };

BOOL __fastcall A3Q6FlagPlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A3Q6SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A3Q6Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q6ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q6Activate(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q6NpcMessage(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q6UnitKilled(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A3Q6CreditInAct(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A3Q6Timer(GAME *ptGame, QUEST *pQuest);
BOOL __fastcall A3Q6FlagParty(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A3Q6FlagSeen(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A3Q6UnitEvent(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A3Q6Sequence(QUEST *pQuest);
void __fastcall A3Q6PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);

void __fastcall A3Q6CreditPartyMember(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuest;
	void *pRecord;
	GAMEROOM *hRoom;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 22, 0) == 1)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	if (D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom)) != 2)
		return;
	D2CommonQuestRecordSetBit(pRecord, 22, 13);
	D2CommonQuestRecordSetBit(pRecord, 22, 0);
	D2CommonQuestRecordSetBit(pRecord, 22, 11);
	ClientSetStep(SUnitGetClient(hUnit), 3, ptGame->bDifficultyLevel);
	pQuest = QuestsFind(ptGame, 20);
	if (pQuest)
		((A3Q6RECORD *)pQuest->pQuestRecord)->nCreditedPlayers++;
}

void __fastcall A3Q6ObjectOperated(QUEST *pQuest  , UNIT *hObject
)
{
	A3Q6RECORD *pQuestRecord;
	UNIT *hUnit;
	DWORD dwMode;

	pQuestRecord = (A3Q6RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->dwHellgateMode == 2)
	{
		dwMode = hObject->dwMode;
		if (dwMode == 2)
			return;
		for (hUnit = ((GAMEROOM *)D2CommonUnitGetPathRoom(hObject))->ptUnitFirst; hUnit;
			hUnit = hUnit->ptRoomNext)
		{
			if (hUnit->eType == 0 && D2CommonUnitCalcEdgeDistance(hUnit, hObject) < 18)
			{
				if (dwMode == 0)
				{
					pQuestRecord->dwBridgeMode = 2;
					D2CommonUnitSetMode(hObject, 1);
				}
				else if (dwMode == 1)
				{
					pQuestRecord->dwBridgeMode = 2;
					D2CommonUnitSetMode(hObject, 2);
					D2CommonUnitClearCollision(hObject);
				}
				break;
			}
		}
	}
	EventAdd(pQuest->ptGame, hObject, 7, pQuest->ptGame->nFrame + 24);
}

void __fastcall A3Q6InitMephistoBridge(OBJINIT *ptInit  )
{
	QUEST *pQuest;
	A3Q6RECORD *pQuestRecord;

	pQuest = QuestsFind(ptInit->ptGame, 20);
	if (!pQuest)
		return;
	pQuestRecord = (A3Q6RECORD *)pQuest->pQuestRecord;
	pQuestRecord->bBridgePresent = 1;
	pQuestRecord->dwBridgeId = ptInit->hObject->dwId;
	D2CommonUnitSetMode(ptInit->hObject, pQuestRecord->dwBridgeMode);
	if (pQuestRecord->dwBridgeMode == 2)
		return;
	EventAdd(ptInit->ptGame, ptInit->hObject, 7, ptInit->ptGame->nFrame + 20);
}

void __fastcall A3Q6InitHellgate(OBJINIT *ptInit  )
{
	QUEST *pQuest;
	A3Q6RECORD *pQuestRecord;

	pQuest = QuestsFind(ptInit->ptGame, 20);
	if (D2CommonDungeonRoomGetLevelId(ptInit->hRoom) == 104)
	{
		D2CommonUnitSetMode(ptInit->hObject, 2);
		return;
	}
	if (!pQuest)
		return;
	pQuestRecord = (A3Q6RECORD *)pQuest->pQuestRecord;
	pQuestRecord->bHellgatePresent = 1;
	pQuestRecord->dwHellgateId = ptInit->hObject->dwId;
	D2CommonUnitSetMode(ptInit->hObject, pQuestRecord->dwHellgateMode);
}

void __fastcall A3Q6Init(QUEST *pQuest  )
{
	A3Q6RECORD *pQuestRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->bState = 0;
	pQuest->pStatusFilter = NULL;
	pQuest->ptNpcTables = sgtA3Q6Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->pCallback[8] = A3Q6UnitKilled;
	pQuest->pCallback[0] = A3Q6Talk;
	pQuest->pCallback[11] = A3Q6NpcMessage;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A3Q6ChangedLevel;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A3Q6PlayerStarted;
	pQuest->pCallback[10] = A2Q5PlayerDone;
	pQuest->pSequence = A3Q6Sequence;
	pQuest->eFilter = 22;
	pQuest->bDoneStateMin = 6;
	pQuest->pActiveFilter = A3Q6Active;
#line 888
	pQuestRecord = (A3Q6RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A3Q6RECORD));
	pQuest->pQuestRecord = pQuestRecord;
	for (i = 0; i < (int)(sizeof(A3Q6RECORD) / sizeof(DWORD)); i++)
		((DWORD *)pQuestRecord)[i] = 0;
}

void __fastcall A3Q6Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	UNIT *hPlayerX;
	GAME *ptGame = pqParam->ptGame;
	void *pRecord;
	DWORD dwBit0;
	DWORD dwBit13;
	int nNpcClass;
	int nTable;
	hPlayerX = pqParam->hPlayer;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayerX))
		->apQuestRecords[ptGame->bDifficultyLevel];
	dwBit0 = D2CommonStepTestGridBit(pRecord, 22, 0);
	dwBit13 = D2CommonStepTestGridBit(pRecord, 22, 13);
	nNpcClass = (int)pqParam->hEventUnit->dwClassId;
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 11))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 5);
		return;
	}
	if (dwBit0 == 1)
	{
		if (QuestsHasPlayer(pQuest, pqParam->hPlayer->dwId) == 1)
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 6);
		return;
	}
	if (pQuest->bState >= 6)
	{
		if (dwBit13 == 1)
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 6);
		return;
	}
	nTable = sgnA3Q6NpcTable[pQuest->bState];
	if (nTable == -1)
		return;
	if ((DWORD)nTable >= 8)
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, nTable);
}

void __fastcall A3Q6ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	DWORD dwLevelNew;
	BOOL fStatus;
	GAME *ptGame;
	UNIT *hPlayer;

	dwLevelNew = pqParam->field_18;
	if (pqParam->field_14 == 75)
	{
		QuestsPlayerLeft(pQuest, pqParam);
		if (pQuest->bState > 1 && pQuest->bState <= 3)
		{
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
			pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
			if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0) == 1)
				return;
			if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 11) == 1)
				return;
			if (pQuest->bStatus < 3)
			{
				pQuest->bStatusFlags = 0;
				QuestsSetStatus(pQuest, 2, NULL, A3Q6SendStatus, FALSE);
			}
			if (QuestsGetRecordBit(pqParam->ptGame, 17, 13) == 1)
			{
#line 362
				QuestsSetState(pQuest, 4, D2_THIS_FILE, __LINE__);
			}
			else
			{
#line 366
				QuestsSetState(pQuest, 5, D2_THIS_FILE, __LINE__);
			}
			SUnitIterate(pQuest->ptGame, 0, NULL, A3Q6FlagPlayer);
		}
	}
	if ((int)dwLevelNew >= 98 && pQuest->bActive && !pQuest->bState)
	{
		pQuest->bState = 1;
		pQuest->pCallback[2] = A3Q6Activate;
	}
	if (dwLevelNew == 100)
	{
		fStatus = FALSE;
		if (pQuest->bStatus == 2)
		{
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 3, NULL, A3Q6SendStatus, TRUE);
			fStatus = TRUE;
		}
		else if (!pQuest->bStatus)
		{
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 3, NULL, A3Q6SendStatus, TRUE);
			fStatus = TRUE;
		}
		if (pQuest->bState == 1)
		{
			if (QuestsGetRecordBit(pqParam->ptGame, 17, 13) == 1)
			{
#line 398
				QuestsSetState(pQuest, 4, D2_THIS_FILE, __LINE__);
			}
			else
			{
#line 402
				QuestsSetState(pQuest, 5, D2_THIS_FILE, __LINE__);
			}
		}
		else if (!fStatus)
			return;
		SUnitIterate(pQuest->ptGame, 0, NULL, A3Q6FlagPlayer);
		return;
	}
	if (dwLevelNew != 102)
		return;
	if (pQuest->bStatus != 4)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 4, NULL, A3Q6SendStatus, TRUE);
	}
	if (pQuest->bState != 4 && pQuest->bState != 5)
	{
		if (QuestsGetRecordBit(pqParam->ptGame, 17, 13) == 1)
		{
#line 424
			QuestsSetState(pQuest, 4, D2_THIS_FILE, __LINE__);
		}
		else
		{
#line 428
			QuestsSetState(pQuest, 5, D2_THIS_FILE, __LINE__);
		}
	}
	SUnitIterate(pQuest->ptGame, 0, NULL, A3Q6FlagPlayer);
}

BOOL __fastcall A3Q6FlagPlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuest;
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 22, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 22, 11) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 20);
	if (!pQuest)
		return FALSE;
	switch (pQuest->bState)
	{
	case 2:
		D2CommonQuestRecordSetBit(pRecord, 22, 2);
		return FALSE;
	case 3:
		D2CommonQuestRecordSetBit(pRecord, 22, 3);
		return FALSE;
	case 4:
		if (pQuest->bStatus == 2)
		{
			D2CommonQuestRecordSetBit(pRecord, 22, 4);
			return FALSE;
		}
		if (pQuest->bStatus == 3)
		{
			D2CommonQuestRecordSetBit(pRecord, 22, 5);
			return FALSE;
		}
		if (pQuest->bStatus == 4)
			D2CommonQuestRecordSetBit(pRecord, 22, 6);
		return FALSE;
	case 5:
		if (pQuest->bStatus == 2)
		{
			D2CommonQuestRecordSetBit(pRecord, 22, 7);
			return FALSE;
		}
		if (pQuest->bStatus == 3)
		{
			D2CommonQuestRecordSetBit(pRecord, 22, 8);
			return FALSE;
		}
		if (pQuest->bStatus == 4)
			D2CommonQuestRecordSetBit(pRecord, 22, 9);
		return FALSE;
	}
	return FALSE;
}

BOOL __fastcall A3Q6SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 22, 0) || D2CommonStepTestGridBit(pRecord, 22, 15))
		&& !D2CommonStepTestGridBit(pRecord, 22, 13)
		&& !D2CommonStepTestGridBit(pRecord, 22, 14))
		return FALSE;
	QuestsSendStatus(ptGame, hUnit, 20);
	return FALSE;
}

void __fastcall A3Q6Activate(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q6RECORD *pQuestRecord;

	if ((WORD)pqParam->hEventUnit->dwClassId != 255)
		return;
	pQuestRecord = (A3Q6RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bStatus2Pending != 1)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 2, NULL, A3Q6SendStatus, TRUE);
	pQuestRecord->bStatus2Pending = 0;
	pQuest->pCallback[2] = NULL;
}

void __fastcall A3Q6NpcMessage(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	WORD wNpcClass;
	WORD wMessage;
	GAME *ptGame;
	UNIT *hPlayer;

	wNpcClass = (WORD)pqParam->field_14;
	wMessage = (WORD)pqParam->field_18;
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecord, 22, 11) && D2CommonStepTestGridBit(pRecord, 22, 0))
		return;
	if (wNpcClass == 255 && wMessage == 628)
	{
		if (QuestsGetRecordBit(pqParam->ptGame, 17, 13) == 1)
		{
#line 461
			QuestsSetState(pQuest, 2, D2_THIS_FILE, __LINE__);
		}
		else
		{
#line 465
			QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
		}
		((A3Q6RECORD *)pQuest->pQuestRecord)->bStatus2Pending = 1;
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		return;
	}
	if (wMessage != 658 && wMessage != 657 && wMessage != 659 && wMessage != 660 && wMessage != 661
		&& wMessage != 662 && wMessage != 663)
		return;
	if (D2CommonStepTestGridBit(pRecord, 22, 11))
	{
		if (D2CommonStepTestGridBit(pRecord, 22, 13))
		{
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 13, NULL, A3Q6SendStatus, FALSE);
#line 485
			QuestsSetState(pQuest, 7, D2_THIS_FILE, __LINE__);
		}
		D2CommonQuestRecordClearBit(pRecord, 22, 11);
		QuestsPlayersAdd(&pQuest->tPlayers, pqParam->hPlayer->dwId);
	}
	QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
}

void __fastcall A3Q6UnitKilled(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q6RECORD *pQuestRecord;
	UNIT *hObject;
	MISSOBJECTSTXT *ptObjectTxt;
	int nLevel;
	int i;
	GAME *ptGame;
	void *pRecord;

#line 617
	QuestsDebugMessage(pQuest->ptGame, "killed boss for quest", D2_THIS_FILE, __LINE__);
#line 618
	QuestsSetState(pQuest, 6, D2_THIS_FILE, __LINE__);
	pQuestRecord = (A3Q6RECORD *)pQuest->pQuestRecord;
	pQuest->pCallback[10] = A2Q5PlayerDone;
	if (pQuest->bActive)
	{
		if (pqParam->hPlayer)
		{
			ptGame = pQuest->ptGame;
			pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
			if (!D2CommonStepTestGridBit(pRecord, 22, 0))
				QuestsDropItem(pQuest, pqParam, 62);
		}
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A3Q6CreditInAct);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A3Q6FlagParty);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A3Q6FlagSeen);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A3Q6UnitEvent);
		if (!pQuestRecord->bTimerPending)
		{
			QuestsAddTimer(pQuest, A3Q6Timer, 12);
			pQuestRecord->bTimerPending = 1;
		}
	}
	QuestsSetRecordBit(pqParam->ptGame, 22, 13);
	if (pQuestRecord->bHellgatePresent)
	{
		hObject = SUnitFind(pQuest->ptGame, 2, pQuestRecord->dwHellgateId);
		if (hObject)
		{
			D2CommonUnitSetMode(hObject, 1);
			ptObjectTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hObject->dwClassId);
			EventAdd(pQuest->ptGame, hObject, 1,
				(int)(((DWORD)ptObjectTxt->FrameCnt[1] >> 8) + pQuest->ptGame->nFrame));
		}
	}
	pQuestRecord->dwHellgateMode = 2;
	pqParam->hEventUnit->dwDropItemCode = A3Q6_CODE_MSS;
	for (i = 0; i < pQuestRecord->nCreditedPlayers; i++)
	{
		if (ITEMSDropFromUnit(pqParam->ptGame, pqParam->hEventUnit, 2, &nLevel, NULL, 44, FALSE))
		{
			pQuestRecord->bMssDropped = 1;
			pQuestRecord->nMssDropped++;
		}
	}
	A2Q3Nothing(pQuest->ptGame, (UNIT *)pQuestRecord->nMssDropped);
	QuestsSetAll(pQuest->ptGame, 11);
}

BOOL __fastcall A3Q6CreditInAct(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuest;
	void *pRecord;
	GAMEROOM *hRoom;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 22, 0))
		return FALSE;
	if (D2CommonDungeonRoomGetLevelId(hRoom) != 102)
		return FALSE;
	pQuest = QuestsFind(ptGame, 20);
	if (D2CommonStepTestGridBit(pRecord, 22, 11))
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 22, 13);
	D2CommonQuestRecordSetBit(pRecord, 22, 0);
	D2CommonQuestRecordSetBit(pRecord, 22, 11);
	ClientSetStep(SUnitGetClient(hUnit), 3, ptGame->bDifficultyLevel);
	((A3Q6RECORD *)pQuest->pQuestRecord)->nCreditedPlayers++;
	return FALSE;
}

BYTE __fastcall A3Q6Timer(GAME *ptGame  , QUEST *pQuest
)
{
	A3Q6RECORD *pQuestRecord;

	pQuestRecord = (A3Q6RECORD *)pQuest->pQuestRecord;
	if (pQuest->bStatus != 4)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 4, NULL, A3Q6SendStatus, TRUE);
	}
	pQuestRecord->bTimerPending = 0;
	return TRUE;
}

BOOL __fastcall A3Q6FlagParty(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	WORD wParty;

	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecordX, 22, 13))
		return FALSE;
	wParty = SUnitGetParty(hUnit);
	if (wParty != 65535)
		PartyForEach(ptGame, wParty, A3Q6CreditPartyMember, NULL);
	return FALSE;
}

BOOL __fastcall A3Q6FlagSeen(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 22, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 22, 11) == 1)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 22, 14);
	QuestsSendFlagInAct(hUnit, 20, 0);
	return FALSE;
}

BOOL __fastcall A3Q6UnitEvent(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecordX, 22, 13))
		SUnitSetEvent(hUnit, 64, hUnit);
	return FALSE;
}

BYTE __fastcall A3Q6Sequence(QUEST *pQuest  )
{
	if (!(pQuest->bState)) {
		if (!pQuest->bActive)
			return TRUE;
		pQuest->bState = 1;
		pQuest->pCallback[2] = A3Q6Activate;
		return TRUE; }
	return TRUE;
}

void __fastcall A3Q6PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 22, 0) == 1
		|| D2CommonStepTestGridBit(pRecord, 22, 11) == 1
		|| D2CommonStepTestGridBit(pRecord, 23, 0) == 1)
	{
		QuestsSetRecordBit(pQuest->ptGame, 22, 13);
		((A3Q6RECORD *)pQuest->pQuestRecord)->dwHellgateMode = 2;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 22, 9) == 1)
	{
		pQuest->bState = 5;
		pQuest->bStatus = 4;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 22, 8) == 1)
	{
		pQuest->bState = 5;
		pQuest->bStatus = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 22, 7) == 1)
	{
		pQuest->bState = 5;
		pQuest->bStatus = 2;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 22, 6) == 1)
	{
		pQuest->bState = 4;
		pQuest->bStatus = 4;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 22, 5) == 1)
	{
		pQuest->bState = 4;
		pQuest->bStatus = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 22, 4) == 1)
	{
		pQuest->bState = 4;
		pQuest->bStatus = 2;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 22, 3) == 1)
	{
		pQuest->bState = 2;
		pQuest->bStatus = 2;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 22, 2) == 1)
	{
		pQuest->bState = 3;
		pQuest->bStatus = 2;
	}
}

void __fastcall A3Q6InitNatalyaStart(OBJINIT *ptInit  )
{
	A3Q6RECORD *pQuestRecord;
	UNIT *hMonster;

	if (QuestsGetRecordBit(ptInit->ptGame, 22, 13))
		return;
	pQuestRecord = (A3Q6RECORD *)QuestsFind(ptInit->ptGame, 20)->pQuestRecord;
	if (pQuestRecord->bNatalyaSpawned)
	{
		hMonster = SUnitFind(ptInit->ptGame, 1, pQuestRecord->dwNatalyaId);
		if (hMonster && hMonster->dwClassId == A3Q6_MONSTER)
			return;
	}
	hMonster = MonsterSpawn(ptInit->ptGame, ptInit->hRoom, ptInit->nX, ptInit->nY, A3Q6_MONSTER, 1, -1, 0);
	if (!hMonster)
	{
		hMonster = MonsterSpawn(ptInit->ptGame, ptInit->hRoom, ptInit->nX, ptInit->nY, A3Q6_MONSTER, 1, 3, 0);
		if (!hMonster)
		{
			pQuestRecord->bNatalyaSpawned = 0;
			return;
		}
	}
	pQuestRecord->bNatalyaSpawned = 1;
	pQuestRecord->dwNatalyaId = hMonster->dwId;
}
