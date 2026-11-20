#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a3q5.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../AI/AiTatics.h"
#include "../GAME/Event.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/MonsterUnique.h"
#include "../PLAYER/PlrTrade.h"
#include "QUESTS.h"
#include "a3q5.h"
#include "a2q5.h"
#include "../UNIT/Party.h"
#include "QuestsFX.h"
#include "../UNIT/SUnit.h"

#define A3Q5_CODE_FLAIL1	540108401
#define A3Q5_CODE_FLAIL2	540173937
#define A3Q5_CODE_EYE		544826737
#define A3Q5_CODE_HEART		544368753
#define A3Q5_CODE_BRAIN		544367217
#define A3Q5_CODE_BOX		544763746

#define A3Q5_MONSTER		366

static QUESTNPCTABLE sgtA3Q5Npcs[8] =
{
	{ { { 255, 594, { 0, 0 }, 1 } }, 1 },
	{ { { 254, 595, { 0, 0 }, 2 }, { 255, 597, { 0, 0 }, 2 }, { 264, 599, { 0, 0 }, 2 },
		{ 252, 601, { 0, 0 }, 2 }, { 253, 603, { 0, 0 }, 2 }, { 245, 605, { 0, 0 }, 2 },
		{ 297, 607, { 0, 0 }, 2 } }, 7 },
	{ { { 254, 596, { 0, 0 }, 2 }, { 255, 598, { 0, 0 }, 2 }, { 264, 600, { 0, 0 }, 2 },
		{ 252, 602, { 0, 0 }, 2 }, { 253, 604, { 0, 0 }, 2 }, { 245, 606, { 0, 0 }, 2 },
		{ 297, 608, { 0, 0 }, 2 } }, 7 },
	{ { { 254, 609, { 0, 0 }, 2 }, { 255, 611, { 0, 0 }, 2 }, { 264, 612, { 0, 0 }, 2 },
		{ 252, 614, { 0, 0 }, 2 }, { 253, 616, { 0, 0 }, 2 }, { 245, 618, { 0, 0 }, 2 },
		{ 297, 619, { 0, 0 }, 2 } }, 7 },
	{ { { 254, 610, { 0, 0 }, 2 }, { 264, 613, { 0, 0 }, 2 }, { 252, 615, { 0, 0 }, 2 },
		{ 253, 617, { 0, 0 }, 2 }, { 297, 620, { 0, 0 }, 2 } }, 5 },
	{ { { 254, 621, { 0, 0 }, 2 }, { 255, 622, { 0, 0 }, 2 }, { 264, 623, { 0, 0 }, 2 },
		{ 252, 624, { 0, 0 }, 2 }, { 253, 625, { 0, 0 }, 2 }, { 245, 626, { 0, 0 }, 1 },
		{ 297, 627, { 0, 0 }, 2 } }, 7 },
	{ { { 254, 621, { 0, 0 }, 2 }, { 255, 622, { 0, 0 }, 2 }, { 264, 623, { 0, 0 }, 2 },
		{ 252, 624, { 0, 0 }, 2 }, { 253, 625, { 0, 0 }, 2 }, { 245, 626, { 0, 0 }, 2 },
		{ 297, 627, { 0, 0 }, 2 } }, 7 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static int sgnA3Q5NpcTable[8] = { -1, 0, 1, 2, 3, 4, 5, 6 };

BOOL __fastcall A3Q5FlagPlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A3Q5SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A3Q5FlagPartyMember(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A3Q5FinishPartyMember(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A3Q5Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q5ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q5Activate(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q5NpcMessage(QUEST *pQuestInfo, QUESTPARAM *pqParam);
BYTE __fastcall A3Q5Sequence(QUEST *pQuestInfo);
void __fastcall A3Q5PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q5UnitKilled(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A3Q5CreditInAct(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A3Q5FlagParty(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A3Q5FlagSeen(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A3Q5UnitEvent(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A3Q5CountItems(GAME *ptGame, UNIT *hUnit, void *pArg);

BYTE __fastcall A3Q5Active(QUEST *pQuest  , int nNpcClass, UNIT *hPlayer,
	void *pPlayerRecord)
{
	if (D2CommonStepTestGridBit(pPlayerRecord, 21, 0))
		return FALSE;
	if (nNpcClass == 255)
	{
		if (pQuest->bState == 1)
			return TRUE;

	}
	if (nNpcClass == 245 && D2CommonStepTestGridBit(pPlayerRecord, 21, 4)
		&& !((A3Q5RECORD *)pQuest->pQuestRecord)->bFlailPlaced)
		return TRUE;
	return FALSE;
}

void __fastcall A3Q5AddMonster(GAME *ptGame  , UNIT *hMonster)
{
	QUEST *pQuest;
	A3Q5RECORD *pQuestRecord;
	int i;
	DWORD dwId;

	pQuest = QuestsFind(ptGame, 19);
	if (!pQuest || !pQuest->bActive)
		return;
	pQuestRecord = (A3Q5RECORD *)pQuest->pQuestRecord;
	pQuestRecord->bMonsterAdded = 1;
	if (pQuestRecord->nMonsterCount >= 6)
		return;
	dwId = hMonster->dwId;
	for (i = 0; i < pQuestRecord->nMonsterCount; i++)
	{
		if (pQuestRecord->adwMonsterIds[i] == dwId)
			return;
	}
	pQuestRecord->adwMonsterIds[pQuestRecord->nMonsterCount] = dwId;
	pQuestRecord->nMonsterCount++;
	pQuestRecord->nMonstersLeft = pQuestRecord->nMonsterCount;
	if (!pQuest->bActive)
		return;
	if (pQuest->bState <= 1)
	{
		if (pQuest->bStatus != 1)
		{
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 1, NULL, A3Q5SendStatus, TRUE);
		}
	}
	else if (pQuest->bStatus != 3)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 3, NULL, A3Q5SendStatus, TRUE);
	}
	if (QuestsGetRecordBit(pQuest->ptGame, 17, 13) == 1)
	{
#line 601
		QuestsSetState(pQuest, 4, D2_THIS_FILE, __LINE__);
	}
	else
	{
#line 605
		QuestsSetState(pQuest, 5, D2_THIS_FILE, __LINE__);
	}
	SUnitIterate(pQuest->ptGame, 0, NULL, A3Q5FlagPlayer);
}

BOOL __fastcall A3Q5FlagPlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuest;
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 21, 0) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 19);
	if (!pQuest)
		return FALSE;
	if (pQuest->bState >= 2 && pQuest->bState <= 3)
		D2CommonQuestRecordSetBit(pRecord, 21, 2);
	if (((A3Q5RECORD *)pQuest->pQuestRecord)->bMonsterAdded)
		D2CommonQuestRecordSetBit(pRecord, 21, 3);
	return FALSE;
}

BOOL __fastcall A3Q5SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 21, 0) || D2CommonStepTestGridBit(pRecord, 21, 15))
		&& !D2CommonStepTestGridBit(pRecord, 21, 13)
		&& !D2CommonStepTestGridBit(pRecord, 21, 14))
		return FALSE;
	QuestsSendStatus(ptGame, hUnit, 19);
	return FALSE;
}

void __fastcall A3Q5FlagPartyMember(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	DWORD dwLevelId;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 21, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, 21, 4) == 1)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	dwLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
	if (!dwLevelId)
		return;
	if (D2CommonGetActNum(dwLevelId) != 2)
		return;
	if (D2CommonStepTestGridBit(pRecord, 18, 0))
		D2CommonQuestRecordSetBit(pRecord, 21, 0);
	else
		D2CommonQuestRecordSetBit(pRecord, 21, 4);
	D2CommonQuestRecordSetBit(pRecord, 21, 13);
}

void __fastcall A3Q5FinishPartyMember(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 18, 0) == 1)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	if (D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom)) != 2)
		return;
	D2CommonQuestRecordSetBit(pRecord, 18, 0);
	D2CommonQuestRecordSetBit(pRecord, 18, 13);
	if (!PlrTradeIsTrading(ptGame, hUnit))
	{
		QuestsRemoveItem(ptGame, hUnit, A3Q5_CODE_EYE);
		QuestsRemoveItem(ptGame, hUnit, A3Q5_CODE_HEART);
		QuestsRemoveItem(ptGame, hUnit, A3Q5_CODE_BRAIN);
		QuestsRemoveItem(ptGame, hUnit, A3Q5_CODE_FLAIL1);
		QuestsRemoveItem(ptGame, hUnit, A3Q5_CODE_FLAIL2);
	}
	if (D2CommonStepTestGridBit(pRecord, 21, 4) && !D2CommonStepTestGridBit(pRecord, 21, 0))
		D2CommonQuestRecordSetBit(pRecord, 21, 0);
}

int __fastcall A3Q5OperateObject(OBJOPERATE *pObjectParam  )
{
	void *pRecordX;
	GAME *ptGame;
	UNIT *hPlayer;
	QUEST *pQuestInfo;
	A3Q5RECORD *pQuestRecord;
	UNIT *hItem;
	UNIT *hMonster;
	MISSOBJECTSTXT *ptObjectTxt;
	WORD wParty;

	if (pObjectParam->hObject->dwMode)
		return 0;
	hItem = (UNIT *)D2CommonINVENTORYFindEquippedMatch(pObjectParam->hPlayer->ptInventory);
	if (!hItem || D2CommonITEMSGetTypeCode(hItem) != A3Q5_CODE_FLAIL2)
	{
		SUnitSetEvent(pObjectParam->hPlayer, 18, pObjectParam->hPlayer);
		return 0;
	}
	pQuestInfo = QuestsFind(pObjectParam->ptGame, 19);
	pQuestRecord = (A3Q5RECORD *)pQuestInfo->pQuestRecord;
	if (++pQuestRecord->nFlailOperations < 2)
		return 0;
	hPlayer = pObjectParam->hPlayer;
	ptGame = pObjectParam->ptGame;
	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	D2CommonQuestRecordSetBit(pRecordX, 18, 0);
	hPlayer = pObjectParam->hPlayer;
	ptGame = pObjectParam->ptGame;
	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	D2CommonQuestRecordSetBit(pRecordX, 18, 13);
	QuestsRemoveItem(pObjectParam->ptGame, pObjectParam->hPlayer, A3Q5_CODE_FLAIL2);
	hPlayer = pObjectParam->hPlayer;
	ptGame = pObjectParam->ptGame;
	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecordX, 21, 4))
	{
		hPlayer = pObjectParam->hPlayer;
		ptGame = pObjectParam->ptGame;
		pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
		if (!D2CommonStepTestGridBit(pRecordX, 21, 0))
		{
			hPlayer = pObjectParam->hPlayer;
			ptGame = pObjectParam->ptGame;
			pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
			D2CommonQuestRecordSetBit(pRecordX, 21, 0);
		}
	}
	hMonster = SUnitFind(pObjectParam->ptGame, 1, pQuestRecord->dwOrbId);
	if (hMonster)
	{
		AiSetModeAtPoint(pObjectParam->ptGame, hMonster, 0, 0, 0);
		QuestsFXOpenStairs(pObjectParam->ptGame, hMonster);
	}
	D2CommonUnitSetMode(pObjectParam->hObject, 1);
	ptObjectTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->hObject->dwClassId);
	EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
		(int)(((DWORD)ptObjectTxt->FrameCnt[1] >> 8) + pObjectParam->ptGame->nFrame));
	if (pQuestInfo)
	{
		pQuestRecord->bFlailPlaced = 1;
		QuestsSetAll(pQuestInfo->ptGame, 10);
#line 816
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		pQuestInfo->pSequence(pQuestInfo);
	}
	wParty = SUnitGetParty(pObjectParam->hPlayer);
	if (wParty != 65535)
		PartyForEach(pObjectParam->ptGame, wParty, A3Q5FinishPartyMember, NULL);
	return 0;
}

void __fastcall A3Q5InitStairs(OBJINIT *ptInit  )
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptInit->ptGame, 19);
	if (((A3Q5RECORD *)pQuest->pQuestRecord)->bFlailPlaced)
		D2CommonUnitSetMode(ptInit->hObject, 2);
}

void __fastcall A3Q5InitCompellingOrb(OBJINIT *ptInit  )
{
	QUEST *pQuest;
	A3Q5RECORD *pQuestRecord;
	UNIT *hMonster;

	pQuest = QuestsFind(ptInit->ptGame, 19);
	pQuestRecord = (A3Q5RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bFlailPlaced)
		D2CommonUnitSetMode(ptInit->hObject, 2);
	if (pQuestRecord->bOrbSpawned)
		return;
	hMonster = MonsterSpawnAt(ptInit->ptGame, ptInit->hRoom, ptInit->nX, ptInit->nY, A3Q5_MONSTER, 1, 0);
	hMonster->dwFlags0 |= 131072;
	if (!hMonster)
		return;
	pQuestRecord->dwOrbId = hMonster->dwId;
	pQuestRecord->bOrbSpawned = 1;
}

void __fastcall A3Q5Init(QUEST *pQuest  )
{
	A3Q5RECORD *pQuestRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->bState = 0;
	pQuest->pStatusFilter = NULL;
	pQuest->ptNpcTables = sgtA3Q5Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->pCallback[8] = A3Q5UnitKilled;
	pQuest->pCallback[0] = A3Q5Talk;
	pQuest->pCallback[11] = A3Q5NpcMessage;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A3Q5ChangedLevel;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A3Q5PlayerStarted;
	pQuest->pCallback[10] = A2Q5PlayerDone;
	pQuest->pSequence = A3Q5Sequence;
	pQuest->dwNextQuestId = 20;
	pQuest->eFilter = 21;
	pQuest->pActiveFilter = A3Q5Active;
	pQuest->bDoneStateMin = 6;
#line 1025
	pQuestRecord = (A3Q5RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A3Q5RECORD));
	pQuest->pQuestRecord = pQuestRecord;
	for (i = 0; i < (int)(sizeof(A3Q5RECORD) / sizeof(DWORD)); i++)
		((DWORD *)pQuestRecord)[i] = 0;
	memset(pQuest->pQuestRecord, 0, sizeof(A3Q5RECORD));
}

void __fastcall A3Q5Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	DWORD dwBit0;
	DWORD dwBit13;
	int nNpcClass;
	int nTable;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	dwBit0 = D2CommonStepTestGridBit(pRecord, 21, 0);
	dwBit13 = D2CommonStepTestGridBit(pRecord, 21, 13);
	nNpcClass = (int)pqParam->hEventUnit->dwClassId;
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 4))
	{
		if (((A3Q5RECORD *)pQuest->pQuestRecord)->bFlailPlaced)
			return;
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 5);
		return;
	}
	if (dwBit0 == 1 && !QuestsHasPlayer(pQuest, pqParam->hPlayer->dwId))
		return;
	if (pQuest->bState >= 6)
	{
		if (dwBit13 == 1)
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 6);
		return;
	}
	if (!pQuest->bActive)
		return;
	nTable = sgnA3Q5NpcTable[pQuest->bState];
	if (nTable == -1)
		return;
	if ((DWORD)nTable >= 8)
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, nTable);
}

void __fastcall A3Q5ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	GAME *ptGame;
	DWORD dwLevelNew;
	DWORD dwLevelOld;
	UNIT *hPlayer;

	dwLevelNew = pqParam->field_14;
	dwLevelOld = pqParam->field_18;
	if (dwLevelOld == 82 && pQuest->bActive && !pQuest->bState)
	{
		pQuest->bState = 1;
		pQuest->pCallback[2] = A3Q5Activate;
	}
	if (dwLevelNew != 75)
		return;
	QuestsPlayerLeft(pQuest, pqParam);
	if (pQuest->bState <= 1 || pQuest->bState > 3)
		return;
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 4) == 1)
		return;
	if (pQuest->bStatus < 3)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 2, NULL, A3Q5SendStatus, FALSE);
	}
	if (QuestsGetRecordBit(pqParam->ptGame, 17, 13) == 1)
	{
#line 364
		QuestsSetState(pQuest, 4, D2_THIS_FILE, __LINE__);
	}
	else
	{
#line 368
		QuestsSetState(pQuest, 5, D2_THIS_FILE, __LINE__);
	}
	SUnitIterate(pQuest->ptGame, 0, NULL, A3Q5FlagPlayer);
}

void __fastcall A3Q5Activate(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q5RECORD *pQuestRecord;

	if ((WORD)pqParam->hEventUnit->dwClassId != 255)
		return;
	pQuestRecord = (A3Q5RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bStatus2Pending != 1)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 2, NULL, A3Q5SendStatus, TRUE);
	pQuestRecord->bStatus2Pending = 0;
	pQuest->pCallback[2] = NULL;
	SUnitIterate(pQuest->ptGame, 0, NULL, A3Q5FlagPlayer);
}

void __fastcall A3Q5NpcMessage(QUEST *pQuestInfo  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	WORD wNpcClass;
	WORD wMessage;
	GAME *ptGame;
	UNIT *hPlayer;

	wMessage = (WORD)pqParam->field_18;
	wNpcClass = (WORD)pqParam->field_14;
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 21, 0) == 1)
		return;
	if (wNpcClass == 255)
	{
		if (wMessage != 594)
			return;
		if (QuestsGetRecordBit(pqParam->ptGame, 17, 13) == 1)
		{
#line 401
			QuestsSetState(pQuestInfo, 2, D2_THIS_FILE, __LINE__);
		}
		else
		{
#line 405
			QuestsSetState(pQuestInfo, 3, D2_THIS_FILE, __LINE__);
		}
		((A3Q5RECORD *)pQuestInfo->pQuestRecord)->bStatus2Pending = 1;
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		return;
	}
	if (wNpcClass != 245)
		return;
	if (wMessage != 626)
		return;
	if (!D2CommonStepTestGridBit(pRecord, 21, 4))
		return;
	if (D2CommonStepTestGridBit(pRecord, 18, 0))
	{
		QuestsRemoveItem(pqParam->ptGame, pqParam->hPlayer, A3Q5_CODE_EYE);
		QuestsRemoveItem(pqParam->ptGame, pqParam->hPlayer, A3Q5_CODE_HEART);
		QuestsRemoveItem(pqParam->ptGame, pqParam->hPlayer, A3Q5_CODE_BRAIN);
		QuestsRemoveItem(pqParam->ptGame, pqParam->hPlayer, A3Q5_CODE_FLAIL1);
		QuestsRemoveItem(pqParam->ptGame, pqParam->hPlayer, A3Q5_CODE_FLAIL2);
	}
	if (D2CommonStepTestGridBit(pRecord, 21, 13) && pQuestInfo->bState != 7)
	{
		QuestsSetRecordBit(pqParam->ptGame, 21, 13);
		pQuestInfo->bStatusFlags = 0;
		QuestsSetStatus(pQuestInfo, 13, NULL, A3Q5SendStatus, FALSE);
#line 431
		QuestsSetState(pQuestInfo, 7, D2_THIS_FILE, __LINE__);
	}
#line 434
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
	pQuestInfo->pSequence(pQuestInfo);
	D2CommonQuestRecordSetBit(pRecord, 21, 0);
	D2CommonQuestRecordClearBit(pRecord, 21, 4);
	QuestsPlayersAdd(&pQuestInfo->tPlayers, pqParam->hPlayer->dwId);
	QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
}

BYTE __fastcall A3Q5Sequence(QUEST *pQuestInfo  )
{
	QUEST *pNextQuest;

	if (pQuestInfo->bState != 7 && pQuestInfo->bActive)
		return TRUE;
	pNextQuest = QuestsFind(pQuestInfo->ptGame, (int)pQuestInfo->dwNextQuestId);
	if (pNextQuest)
	{
#line 470
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		return pNextQuest->pSequence(pNextQuest);
	}
	return FALSE;
}

void __fastcall A3Q5PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q5RECORD *pQuestRecord;
	void *pRecord;
	GAME *ptGame = pqParam->ptGame;

	pQuestRecord = (A3Q5RECORD *)pQuest->pQuestRecord;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 18, 0))
		pQuestRecord->bFlailPlaced = 1;
	if (D2CommonStepTestGridBit(pRecord, 21, 0) == 1
		|| D2CommonStepTestGridBit(pRecord, 21, 4) == 1)
	{
		QuestsSetRecordBit(pQuest->ptGame, 21, 13);
		pQuest->bActive = 0;
		return;
	}
	pQuestRecord->dwStarterDone = D2CommonStepTestGridBit(pRecord, 17, 0);
	if (D2CommonStepTestGridBit(pRecord, 21, 2))
	{
		if (D2CommonStepTestGridBit(pRecord, 21, 3))
		{
			if (pQuestRecord->dwStarterDone)
			{
				pQuest->bState = 4;
				pQuest->bStatus = 3;
				return;
			}
			pQuest->bState = 5;
			pQuest->bStatus = 3;
		}
		else
		{
			if (pQuestRecord->dwStarterDone)
			{
				pQuest->bState = 2;
				pQuest->bStatus = 2;
				return;
			}
			pQuest->bState = 3;
			pQuest->bStatus = 2;
			return;
		}
	}
	else if (D2CommonStepTestGridBit(pRecord, 21, 3))
	{
		if (pQuestRecord->dwStarterDone)
		{
			pQuest->bState = 4;
			pQuest->bStatus = 3;
			return;
		}
		pQuest->bState = 5;
		pQuest->bStatus = 3;
		return;
	}
}

void __fastcall A3Q5UnitKilled(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q5RECORD *pQuestRecord;
	QUEST *pQuest16;
	A3Q2RECORD *pQuest16Record;
	int nDropped;
	int nLevel;
	int i;

#line 878
	QuestsDebugMessage(pQuest->ptGame, "killed boss for quest", D2_THIS_FILE, __LINE__);
	pQuestRecord = (A3Q5RECORD *)pQuest->pQuestRecord;
	if (pqParam->hEventUnit->dwClassId == A3Q5_MONSTER)
	{
		pqParam->hEventUnit->dwFlags0 |= 131072;
		return;
	}
	if (!pQuestRecord->bFlail1Dropped || !pQuestRecord->bCubeDropped)
	{
		pQuestRecord->nFlail1DropCount = 0;
		pQuestRecord->nCubeDropCount = 0;
		SUnitIterate(pQuest->ptGame, 0, NULL, A3Q5CountItems);
	}
	if (!pQuestRecord->bFlail1Dropped)
	{
		nDropped = 0;
		pqParam->hEventUnit->dwDropItemCode = A3Q5_CODE_FLAIL1;
		for (i = 0; i < pQuestRecord->nFlail1DropCount; i++)
		{
			if (ITEMSDropFromUnit(pQuest->ptGame, pqParam->hEventUnit, 7, &nLevel, NULL, 44, FALSE))
				nDropped++;
		}
		if (nDropped)
		{
			pQuestRecord->bFlail1Dropped = 1;
			pQuest16 = QuestsFind(pQuest->ptGame, 16);
			if (pQuest16)
			{
				pQuest16Record = (A3Q2RECORD *)pQuest16->pQuestRecord;
				pQuest16Record->bFlail1Dropped = 1;
				pQuest16Record->nFlail1Count += nDropped;
			}
		}
	}
	else if (!pQuestRecord->bCubeDropped)
	{
		pQuestRecord->bCubeDropped = 1;
		pqParam->hEventUnit->dwDropItemCode = A3Q5_CODE_BOX;
		for (i = 0; i < pQuestRecord->nCubeDropCount; i++)
			ITEMSDropFromUnit(pQuest->ptGame, pqParam->hEventUnit, 2, &nLevel, NULL, 44, FALSE);
	}
	if (!pQuest->bActive)
		return;
	if (pQuestRecord->nMonstersLeft <= 0)
		return;
	if (--pQuestRecord->nMonstersLeft)
		return;
	pQuest->bStatusFlags = 0;
	if (pQuestRecord->bFlailPlaced)
	{
#line 936
		QuestsSetState(pQuest, 7, D2_THIS_FILE, __LINE__);
		QuestsSetStatus(pQuest, 13, NULL, A3Q5SendStatus, TRUE);
	}
	else
	{
#line 941
		QuestsSetState(pQuest, 6, D2_THIS_FILE, __LINE__);
		QuestsSetStatus(pQuest, 4, NULL, A3Q5SendStatus, TRUE);
	}
	SUnitIterate(pQuest->ptGame, 0, NULL, A3Q5CreditInAct);
	SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A3Q5FlagParty);
	SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A3Q5FlagSeen);
	SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A3Q5UnitEvent);
	QuestsSetRecordBit(pQuest->ptGame, 21, 13);
}

BOOL __fastcall A3Q5CreditInAct(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 21, 0))
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 21, 4))
		return FALSE;
	if (D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom)) != 2)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 18, 0))
		D2CommonQuestRecordSetBit(pRecord, 21, 0);
	else
		D2CommonQuestRecordSetBit(pRecord, 21, 4);
	D2CommonQuestRecordSetBit(pRecord, 21, 13);
	return FALSE;
}

BOOL __fastcall A3Q5FlagParty(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	WORD wParty;

	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecordX, 21, 13))
		return FALSE;
	wParty = SUnitGetParty(hUnit);
	if (wParty != 65535)
		PartyForEach(ptGame, wParty, A3Q5FlagPartyMember, NULL);
	return FALSE;
}

BOOL __fastcall A3Q5FlagSeen(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 21, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 21, 4) == 1)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 21, 14);
	QuestsSendFlagInAct(hUnit, 19, 0);
	return FALSE;
}

BOOL __fastcall A3Q5UnitEvent(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecordX, 21, 13))
		SUnitSetEvent(hUnit, 62, hUnit);
	return FALSE;
}

BOOL __fastcall A3Q5CountItems(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuest;
	A3Q5RECORD *pQuestRecord;
	GAMEROOM *hRoom;
	DWORD dwLevelId;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	dwLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
	if (!dwLevelId)
		return FALSE;
	if (D2CommonGetActNum(dwLevelId) != 2)
		return FALSE;
	pQuest = QuestsFind(ptGame, 19);
	if (!pQuest)
		return FALSE;
	pQuestRecord = (A3Q5RECORD *)pQuest->pQuestRecord;
	if (!ITEMSFindByType(hUnit, A3Q5_CODE_FLAIL1))
		pQuestRecord->nFlail1DropCount++;
	if (!ITEMSFindByType(hUnit, A3Q5_CODE_BOX))
		pQuestRecord->nCubeDropCount++;
	return FALSE;
}

BOOL __fastcall A3Q5TestLevel(GAME *ptGame  , UNIT *hUnit
)
{
	QUEST *pQuest;
	GAMEROOM *hRoom;

	pQuest = QuestsFind(ptGame, 19);
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (hRoom && D2CommonDungeonRoomGetLevelId(hRoom) == 101)
		return FALSE;
	if (pQuest)
		return !((A3Q5RECORD *)pQuest->pQuestRecord)->bFlailPlaced;
	return FALSE;
}

BYTE __fastcall A3Q6Active(QUEST *pQuest  , int nNpcClass, UNIT *hPlayer,
	void *pPlayerRecord)
{
	if (!D2CommonStepTestGridBit(pPlayerRecord, 22, 0) && nNpcClass == 255 && pQuest->bState == 1)
		return TRUE;
	if (D2CommonStepTestGridBit(pPlayerRecord, 22, 11))
		return TRUE;
	return FALSE;
}
