#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a1q5.cpp"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "../GAME/Event.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/MonsterUnique.h"
#include "../OBJECTS/ObjMode.h"
#include "QUESTS.h"
#include "a1q4.h"
#include "a1q5.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"
#include "../SKILLS/Skills.h"

#define A1Q5_CODE_GOLD		543452263

static QUESTNPCTABLE sgtA1Q5Npcs[5] =
{
	{ { { 265, 131, { 0, 0 }, 2 }, { 154, 129, { 0, 0 }, 2 }, { 148, 130, { 0, 0 }, 2 },
		{ 155, 132, { 0, 0 }, 2 }, { 150, 133, { 0, 0 }, 2 }, { 147, 128, { 0, 0 }, 2 } }, 6 },
	{ { { 150, 134, { 0, 0 }, 2 }, { 155, 136, { 0, 0 }, 2 }, { 154, 137, { 0, 0 }, 2 },
		{ 148, 138, { 0, 0 }, 2 }, { 265, 135, { 0, 0 }, 2 }, { 147, 139, { 0, 0 }, 2 } }, 6 },
	{ { { 150, 140, { 0, 0 }, 0 }, { 155, 141, { 0, 0 }, 0 }, { 265, 145, { 0, 0 }, 0 },
		{ 154, 144, { 0, 0 }, 0 }, { 148, 143, { 0, 0 }, 0 }, { 147, 142, { 0, 0 }, 0 } }, 6 },
	{ { { 150, 140, { 0, 0 }, 2 }, { 155, 141, { 0, 0 }, 2 }, { 265, 145, { 0, 0 }, 2 },
		{ 154, 144, { 0, 0 }, 2 }, { 148, 143, { 0, 0 }, 2 }, { 147, 142, { 0, 0 }, 2 } }, 6 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static int sgnA1Q5NpcTable[6] = { -1, -1, 0, 1, 2, 3 };

BOOL __fastcall A1Q4GetObjectSpot(GAME *ptGame, UNIT *hUnit, GAMECOORD *ptCoord);
BOOL __fastcall A1Q4SetSpot(GAME *ptGame, UNIT *hUnit);
void __fastcall A1Q4UnitArrived(GAME *ptGame, UNIT *hUnit);
void __fastcall A1Q4SetCainArrived(GAME *ptGame, UNIT *hUnit);
int __fastcall A1Q5OperateObject(OBJOPERATE *pObjectParam);
BOOL __fastcall A1Q5SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A1Q5UpdatePlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A1Q5Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall A1Q5PartyCredit(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A1Q5SpawnMonster(QUEST *pQuest, A1Q5RECORD *pQuestRecord);
void __fastcall A1Q5NpcMessage(QUEST *pQuestInfo, QUESTPARAM *pqParam);
BOOL __fastcall A1Q5ListRemove(A1Q5RECORD *pQuestRecord, DWORD dwUnitId, BOOL fSecond);
void __fastcall A1Q5PlayerLeft(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q5Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q5ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A1Q5Sequence(QUEST *pQuestInfo);
void __fastcall A1Q5UnitKilled(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A1Q5CreditInLevel(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A1Q5CreditFinish(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A1Q5FlagPlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A1Q5CreditParty(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A1Q5Timer(GAME *ptGame, QUEST *pQuest);
void __fastcall A1Q5PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q6PartyJoined(GAME *ptGame, UNIT *hUnit, void *pArg);

BOOL __fastcall A1Q4GetObjectSpot(GAME *ptGame  , UNIT *hUnit
, GAMECOORD *ptCoord)
{
	QUEST *pQuest;
	A1Q4RECORD *pQuestRecord;

	pQuest = QuestsFind(ptGame, 4);
	if (!pQuest)
		return FALSE;
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	if (!pQuestRecord->bCainPortalMade)
	{
		D2CommonUnitGetPosition(hUnit, ptCoord);
		return TRUE;
	}
	ptCoord->dwX = pQuestRecord->tCainPortalSpot.dwX;
	ptCoord->dwY = pQuestRecord->tCainPortalSpot.dwY;
	return TRUE;
}

BOOL __fastcall A1Q4SetSpot(GAME *ptGame  , UNIT *hUnit
)
{
	QUEST *pQuest;
	A1Q4RECORD *pQuestRecord;

	pQuest = QuestsFind(ptGame, 4);
	if (!pQuest)
		return FALSE;
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bCainPortalSpotSet)
		return FALSE;
	D2CommonUnitGetPosition(hUnit, &pQuestRecord->tCainPortalSpot);
	pQuestRecord->bCainPortalSpotSet = 1;
	return TRUE;
}

static __inline void A1Q4MakeCainPortal(GAME *ptGame, A1Q4RECORD *pQuestRecord)
{
	UNIT *hMonster;
	UNIT *hObject;
	GAMEROOM *hRoom;
	GAMECOORD tCoord;

	if (pQuestRecord->bTownCainMade != 1)
		return;
	hMonster = SUnitFind(ptGame, 1, pQuestRecord->dwTownCainId);
	if (!hMonster)
		return;
	D2CommonUnitGetPosition(hMonster, &tCoord);
	hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hMonster), tCoord.dwX,
		tCoord.dwY);
	if (!hRoom)
		return;
	hObject = SUnitCreate(2, 189, (int)tCoord.dwX, (int)tCoord.dwY, ptGame, hRoom, 1, 1, 0);
	if (hObject)
	{
		pQuestRecord->bTownCainPortalMade = 1;
		pQuestRecord->dwTownCainPortalId = hObject->dwId;
	}
}

void __fastcall A1Q4UnitArrived(GAME *ptGame  , UNIT *hUnit
)
{
	QUEST *pQuest;
	A1Q4RECORD *pQuestRecord;
	UNIT *hObject;
	OBJINIT tInit;

	pQuest = QuestsFind(ptGame, 4);
	if (!pQuest)
		return;
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	pQuestRecord->field_91 = 1;
	pQuestRecord->bTownCainWanted = 1;
	if (pQuestRecord->bCainStartPresent != 1)
		return;
	hObject = SUnitFind(ptGame, 2, pQuestRecord->dwCainStartId);
	if (!hObject)
		return;
	tInit.ptGame = ptGame;
	tInit.hObject = hObject;
	tInit.hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hObject);
	tInit.nX = (int)D2CommonUnitGetX(hObject);
	tInit.nY = (int)D2CommonUnitGetY(hObject);
	A1Q4NpcObjInit(&tInit);
	A1Q4MakeCainPortal(ptGame, pQuestRecord);
}

void __fastcall A1Q4SetCainArrived(GAME *ptGame  , UNIT *hUnit
)
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptGame, 4);
	if (pQuest)
		((A1Q4RECORD *)pQuest->pQuestRecord)->field_92 = 1;
}

BOOL __fastcall A1Q4TestCainActivated(GAME *ptGame  )
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptGame, 4);
	if (!pQuest)
		return FALSE;
	return ((A1Q4RECORD *)pQuest->pQuestRecord)->bCainActivated != 0;
}

void __fastcall A1Q4OperateDrop(QUEST *pQuest  , UNIT *hObject
)
{
	A1Q4RECORD *pQuestRecord;
	int nLevel;

	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	if (!pQuestRecord->bWirtDropsDrawn)
	{
		pQuestRecord->bWirtDropsDrawn = 1;
		pQuestRecord->nWirtDropsLeft = D2CommonSeedBetween(QuestsGetSeed(pQuest->ptGame), 10, 20);
		hObject->dwDropItemCode = A1Q5_CODE_GOLD;
	}
	if (!pQuestRecord->nWirtDropsLeft)
		return;
	nLevel = 0;
	if (!ITEMSDropFromUnit(pQuest->ptGame, hObject, 2, &nLevel, NULL, 44, FALSE))
		return;
	if (--pQuestRecord->nWirtDropsLeft > 0)
		EventAdd(pQuest->ptGame, hObject, 7, pQuest->ptGame->nFrame + 10);
}

int __fastcall A1Q5OperateObject(OBJOPERATE *pObjectParam  )
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
	pQuest = QuestsFind(pObjectParam->ptGame, 5);
	if (pQuest && pQuest->bActive)
	{
		QuestsSendUnitMessage(pObjectParam->hPlayer, pObjectParam->hObject, 127);
		if (pQuest->bState <= 1)
		{
#line 514
			QuestsSetState(pQuest, 2, D2_THIS_FILE, __LINE__);
			if (pQuest->bStatus < 1)
				((A1Q5RECORD *)pQuest->pQuestRecord)->bObjectOperated = 1;
		}
	}
	return 0;
}

BOOL __fastcall A1Q5SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecord, 5, 0) || D2CommonStepTestGridBit(pRecord, 5, 13)
		|| D2CommonStepTestGridBit(pRecord, 5, 14))
		QuestsSendStatus(ptGame, hUnit, 5);
	return FALSE;
}

BOOL __fastcall A1Q5UpdatePlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	QUEST *pQuest;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 5, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 5, 1) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 5);
	if (!pQuest)
		return FALSE;
	if (pQuest->bState == 2)
	{
		D2CommonQuestRecordSetBit(pRecord, 5, 2);
		return FALSE;
	}
	if (pQuest->bState != 3)
		return FALSE;
	switch (pQuest->bStatus)
	{
	case 1:
		D2CommonQuestRecordSetBit(pRecord, 5, 3);
		break;
	case 2:
		D2CommonQuestRecordSetBit(pRecord, 5, 4);
		break;
	case 3:
		D2CommonQuestRecordSetBit(pRecord, 5, 5);
		break;
	case 4:
		D2CommonQuestRecordSetBit(pRecord, 5, 6);
		break;
	}
	return FALSE;
}

BYTE __fastcall A1Q5Active(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer  , void *pPlayerRecord)
{
	A1Q5RECORD *pQuestRecord;
	int i;
	DWORD dwUnitId;

	dwUnitId = hPlayer->dwId;
	pQuestRecord = (A1Q5RECORD *)pQuest->pQuestRecord;
	for (i = 0; i < (int)pQuestRecord->wCreditedCount; i++)
	{
		if (pQuestRecord->adwCreditedIds[i] == dwUnitId)
		{
			if (nNpcClass == 155 || nNpcClass == 147)
				return FALSE;
			return TRUE;
		}
	}
	return FALSE;
}

void __fastcall A1Q5PartyCredit(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	DWORD dwLevelId;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 5, 0) == 1)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	dwLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
	if (!dwLevelId)
		return;
	if (D2CommonGetActNum(dwLevelId))
		return;
	D2CommonQuestRecordSetBit(pRecord, 5, 13);
	D2CommonQuestRecordSetBit(pRecord, 5, 0);
}

void __fastcall A1Q5ObjectEvent(QUEST *pQuest  , UNIT *hObject
)
{
	A1Q5RECORD *pQuestRecord;

	A1Q5SpawnMonster(pQuest, (A1Q5RECORD *)pQuest->pQuestRecord);
	pQuestRecord = (A1Q5RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bFinished && !pQuestRecord->bMonsterSpawned)
		EventAdd(pQuest->ptGame, hObject, 7, pQuest->ptGame->nFrame + 10);
}

void __fastcall A1Q5SpawnMonster(QUEST *pQuest  , A1Q5RECORD *pQuestRecord
)
{
	UNIT *hMonster;
	UNIT *hObject;
	UNIT *hMissile;
	GAMEROOM *hRoom;
	GAMECOORD tCoord;
	int nX;
	int nY;
	int i;

	if (!pQuestRecord->bFinished)
		return;
	if (pQuestRecord->bMonsterSpawned)
		return;
	hMonster = NULL;
	for (i = 0; i < (int)pQuestRecord->wObjectCount; i++)
	{
		hObject = SUnitFind(pQuest->ptGame, 2, pQuestRecord->adwObjectIds[i]);
		if (!hObject)
			continue;
		tCoord.dwX = pQuestRecord->tKillCoord.dwX;
		tCoord.dwY = pQuestRecord->tKillCoord.dwY;
		if (!hMonster)
		{
			hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hObject), tCoord.dwX,
				tCoord.dwY);
			if (hRoom)
				hMonster = MonsterSpawn(pQuest->ptGame, hRoom, (int)tCoord.dwX, (int)tCoord.dwY, 326, 12,
					-1, 8);
			if (!hMonster)
			{
				D2CommonUnitGetPosition(hObject, &tCoord);
				tCoord.dwX += 5;
				tCoord.dwY += 5;
				hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hObject), tCoord.dwX,
					tCoord.dwY);
				hMonster = MonsterSpawn(pQuest->ptGame, hRoom, (int)tCoord.dwX, (int)tCoord.dwY, 326, 12,
					-1, 8);
				if (!hMonster)
					continue;
			}
		}
		pQuestRecord->bMonsterSpawned = 1;
		hMissile = SkillsCreateMissileAt(pQuest->ptGame, hMonster, 1,
			MISSILE_TOWERCHESTSPAWNER, (int)D2CommonUnitGetX(hObject), (int)D2CommonUnitGetY(hObject));
		if (hMissile)
		{
			D2CommonMissileSetScratchFirst(hMissile, pQuestRecord->adwObjectIds[i]);
			D2CommonMissileSetScratchSecond(hMissile, 0);
			hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hMissile);
			D2CommonDungeonRoomSetHold(hRoom, 0);
		}
	}
}

void __fastcall A1Q5Init(QUEST *pQuest  )
{
	A1Q5RECORD *pQuestRecord;
	DWORD *pdw;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->bStatus = 0;
	pQuest->bState = 0;
	pQuest->pStatusFilter = NULL;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A1Q5ChangedLevel;
	pQuest->pCallback[11] = A1Q5NpcMessage;
	pQuest->pCallback[0] = A1Q5Talk;
	pQuest->pCallback[10] = A1Q5PlayerLeft;
	pQuest->pCallback[8] = A1Q5UnitKilled;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A1Q5PlayerStarted;
	pQuest->eFilter = 5;
	pQuest->ptNpcTables = sgtA1Q5Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->bDoneStateMin = 4;
	pQuest->pActiveFilter = A1Q5Active;
	pQuest->pSequence = A1Q5Sequence;
#line 1039
	pQuestRecord = (A1Q5RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A1Q5RECORD));
	pQuest->pQuestRecord = pQuestRecord;
	pQuest->dwNextQuestId = 3;
	pdw = (DWORD *)pQuestRecord;
	for (i = 0; i < (int)(sizeof(A1Q5RECORD) / sizeof(DWORD)); i++)
		pdw[i] = 0;
	QuestsPlayersClear(&pQuestRecord->tPlayers);
}

void __fastcall A1Q5NpcMessage(QUEST *pQuestInfo  , QUESTPARAM *pqParam
)
{
	A1Q5RECORD *pQuestRecord;
	WORD wMessage;
	WORD wNpcClass;
	BYTE bChanged;
	void *pRecord;
	DWORD dwUnitId;
	int i;
	GAME *ptGame;
	UNIT *hPlayer;

	wNpcClass = (WORD)pqParam->field_14;
	wMessage = (WORD)pqParam->field_18;
	pQuestRecord = (A1Q5RECORD *)pQuestInfo->pQuestRecord;
	if (wMessage == 127 && pQuestInfo->bActive)
	{
		bChanged = FALSE;
		if (pQuestRecord->bObjectOperated == 1)
		{
			if (pQuestInfo->bStatus < 1)
			{
				bChanged = TRUE;
				pQuestInfo->bStatusFlags = 0;
				QuestsSetStatus(pQuestInfo, 1, NULL, A1Q5SendStatus, TRUE);
			}
			if (pQuestInfo->bStatus == 3)
			{
				bChanged = TRUE;
				pQuestInfo->bStatusFlags = 0;
				QuestsSetStatus(pQuestInfo, 2, NULL, A1Q5SendStatus, TRUE);
				if (pQuestInfo->bState < 3)
				{
#line 297
					QuestsSetState(pQuestInfo, 3, D2_THIS_FILE, __LINE__);
				}
			}
		}
		if (pQuestInfo->bState < 2)
		{
#line 304
			QuestsSetState(pQuestInfo, 2, D2_THIS_FILE, __LINE__);
			SUnitIterate(pQuestInfo->ptGame, 0, NULL, A1Q5UpdatePlayer);
		}
		else if (bChanged)
			SUnitIterate(pQuestInfo->ptGame, 0, NULL, A1Q5UpdatePlayer);
	}
	if (wNpcClass != 154 && wNpcClass != 150 && wNpcClass != 265 && wNpcClass != 155 && wNpcClass != 148
		&& wNpcClass != 147)
		return;
	if (wMessage != 144 && wMessage != 140 && wMessage != 145 && wMessage != 141 && wMessage != 143
		&& wMessage != 142)
		return;
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 5, 13) && pQuestRecord->bSequencePending)
	{
		pQuestRecord->bSequencePending = 0;
#line 332
		QuestsSetState(pQuestInfo, 5, D2_THIS_FILE, __LINE__);
#line 333
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		pQuestInfo->pSequence(pQuestInfo);
	}
	dwUnitId = pqParam->hPlayer->dwId;
	for (i = 0; i < (int)pQuestRecord->wCreditedCount; i++)
	{
		if (pQuestRecord->adwCreditedIds[i] == dwUnitId)
		{
			if (A1Q5ListRemove(pQuestRecord, dwUnitId, TRUE) && pQuestRecord->wReportedCount < 12)
			{
				pQuestRecord->adwReportedIds[pQuestRecord->wReportedCount] = dwUnitId;
				pQuestRecord->wReportedCount++;
			}
			return;
		}
	}
}

BOOL __fastcall A1Q5ListRemove(A1Q5RECORD *pQuestRecord, DWORD dwUnitId,
	BOOL fSecond)
{
	int nCount;
	int i;

	if (fSecond)
	{
		nCount = (int)pQuestRecord->wCreditedCount;
		for (i = 0; i < nCount; i++)
		{
			if (pQuestRecord->adwCreditedIds[i] == dwUnitId)
			{
				if (i != nCount - 1)
					pQuestRecord->adwCreditedIds[i] = pQuestRecord->adwCreditedIds[nCount - 1];
				pQuestRecord->wCreditedCount--;
				return TRUE;
			}
		}
		return FALSE;
	}
	nCount = (int)pQuestRecord->wReportedCount;
	for (i = 0; i < nCount; i++)
	{
		if (pQuestRecord->adwReportedIds[i] == dwUnitId)
		{
			if (i != nCount - 1)
				pQuestRecord->adwReportedIds[i] = pQuestRecord->adwReportedIds[nCount - 1];
			pQuestRecord->wReportedCount--;
			return TRUE;
		}
	}
	return FALSE;
}

void __fastcall A1Q5PlayerLeft(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q5RECORD *pQuestRecord;
	DWORD dwUnitId;

	dwUnitId = pqParam->hPlayer->dwId;
	pQuestRecord = (A1Q5RECORD *)pQuest->pQuestRecord;
	QuestsPlayersRemove(&pQuestRecord->tPlayers, dwUnitId);
	A1Q5ListRemove(pQuestRecord, dwUnitId, TRUE);
	A1Q5ListRemove(pQuestRecord, dwUnitId, FALSE);
}

void __fastcall A1Q5Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q5RECORD *pQuestRecord;
	void *pRecord;
	DWORD dwDone;
	DWORD dwCredited;
	int nNpcClass;
	DWORD dwUnitId;
	int nTable;
	int i;
	GAME *ptGame;

	if (!pQuest->bActive)
		return;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	dwDone = D2CommonStepTestGridBit(pRecord, 5, 0);
	dwCredited = D2CommonStepTestGridBit(pRecord, 5, 13);
	if (dwDone == 1 && !dwCredited)
		return;
	pQuestRecord = (A1Q5RECORD *)pQuest->pQuestRecord;
	nNpcClass = (int)pqParam->hEventUnit->dwClassId;
	dwUnitId = pqParam->hPlayer->dwId;
	pQuestRecord = (A1Q5RECORD *)pQuest->pQuestRecord;
	if (pQuest->bState >= 4)
	{
		for (i = 0; i < (int)pQuestRecord->wCreditedCount; i++)
			if (pQuestRecord->adwCreditedIds[i] == dwUnitId)
				goto lbListed;
		for (i = 0; i < (int)pQuestRecord->wReportedCount; i++)
			if (pQuestRecord->adwReportedIds[i] == dwUnitId)
				goto lbListed;
		return;
	}
lbListed:
	for (i = 0; i < (int)pQuestRecord->wCreditedCount; i++)
	{
		if (pQuestRecord->adwCreditedIds[i] == dwUnitId)
		{
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 2);
			return;
		}
	}
	if (dwDone == 1 && dwCredited == 1)
	{
		for (i = 0; i < (int)pQuestRecord->wReportedCount; i++)
		{
			if (pQuestRecord->adwReportedIds[i] == dwUnitId)
			{
				QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 3);
				return;
			}
		}
		return; }
	nTable = sgnA1Q5NpcTable[pQuest->bState];
	if (nTable != -1 && (DWORD)nTable < 5)
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, nTable);
}

void __fastcall A1Q5ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q5RECORD *pQuestRecord;
	int nLevelNew;
	int nLevelOld;
	UNIT *hPlayer;
	GAME *ptGame;
	void *pRecord;

	nLevelNew = (int)pqParam->field_18;
	nLevelOld = (int)pqParam->field_14;
	if (pQuest->bActive)
	{
		if (nLevelNew == 20)
		{
			if (!pQuest->bState)
			{
#line 615
				QuestsSetState(pQuest, 2, D2_THIS_FILE, __LINE__);
				pQuest->bStatusFlags = 0;
				QuestsSetStatus(pQuest, 3, NULL, A1Q5SendStatus, TRUE);
				SUnitIterate(pQuest->ptGame, 0, NULL, A1Q5UpdatePlayer);
				return;
			}
			if (pQuest->bState == 3 && pQuest->bStatus == 1)
			{
				pQuest->bStatusFlags = 0;
				QuestsSetStatus(pQuest, 4, NULL, A1Q5SendStatus, TRUE);
				SUnitIterate(pQuest->ptGame, 0, NULL, A1Q5UpdatePlayer);
			}
			return;
		}
		if (nLevelNew == 25)
		{
			if (pQuest->bState >= 4)
				return;
			if (pQuest->bStatus == 2)
				return;
			if (pQuest->bState != 3)
			{
#line 634
				QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
			}
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 2, NULL, A1Q5SendStatus, TRUE);
			SUnitIterate(pQuest->ptGame, 0, NULL, A1Q5UpdatePlayer);
			return;
		}
	}
	if (nLevelOld != 1)
		return;
	if (pQuest->bState == 2)
	{
		hPlayer = pqParam->hPlayer;
		ptGame = pqParam->ptGame;
		pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
		if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0) == 1)
			return;
#line 653
		QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
		return;
	}
	if (pQuest->bState != 5 || !pQuest->bActive)
		return;
	pQuestRecord = (A1Q5RECORD *)pQuest->pQuestRecord;
	if (A1Q5ListRemove(pQuestRecord, pqParam->hPlayer->dwId, FALSE) && !pQuestRecord->wReportedCount
		&& !pQuestRecord->wCreditedCount)
		pQuest->bCallbacksEnabled = 0;
}

BYTE __fastcall A1Q5Sequence(QUEST *pQuestInfo  )
{
	QUEST *pNextQuest;

	if (pQuestInfo->bState < 2 && pQuestInfo->bActive == 1)
		return TRUE;
	if (pQuestInfo->bState != 5 && pQuestInfo->bActive)
		return TRUE;
	pNextQuest = QuestsFind(pQuestInfo->ptGame, (int)pQuestInfo->dwNextQuestId);
	if (pNextQuest)
	{
#line 736
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		return pNextQuest->pSequence(pNextQuest);
	}
	return FALSE;
}

static __inline void A1Q5CreditKeptI(GAME *ptGame, int eFilter, QUESTPLAYERS *pPlayers)
{
	UNIT *hPlayer;
	void *pRecord;
	int i;

	for (i = 0; i < (int)pPlayers->wCount; i++)
	{
		hPlayer = SUnitFind(ptGame, 0, pPlayers->adwUnitIds[i]);
		if (!hPlayer)
			continue;
		pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
		if (D2CommonStepTestGridBit(pRecord, eFilter, 0) == 1)
			continue;
		D2CommonQuestRecordSetBit(pRecord, eFilter, 13);
		D2CommonQuestRecordSetBit(pRecord, eFilter, 0);
		SUnitSetEvent(hPlayer, 35, hPlayer);
	}
}

void __fastcall A1Q5UnitKilled(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q5RECORD *pQuestRecord;

	pQuestRecord = (A1Q5RECORD *)pQuest->pQuestRecord;
	D2CommonUnitGetPosition(pqParam->hEventUnit, &pQuestRecord->tKillCoord);
	if (pQuest->bActive)
	{
#line 945
		QuestsDebugMessage(pQuest->ptGame, "Finished quest", D2_THIS_FILE, __LINE__);
		pQuest->pCallback[2] = NULL;
#line 947
		QuestsSetState(pQuest, 5, D2_THIS_FILE, __LINE__);
		pQuestRecord->bSequencePending = 1;
		SUnitIterate(pQuest->ptGame, 0, NULL, A1Q5CreditFinish);
		QuestsSetRecordBit(pQuest->ptGame, 5, 13);
		if (!((A1Q5RECORD *)pQuest->pQuestRecord)->wCreditedCount)
			pQuest->bCallbacksEnabled = 0;
		else
			pQuest->pCallback[10] = A1Q5PlayerLeft;
		pQuestRecord->bFinished = 1;
		pQuest->pCallback[8] = NULL;
		QuestsSetRecordBit(pqParam->ptGame, pQuest->eFilter, 13);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A1Q5CreditInLevel);
		A1Q5CreditKeptI(pqParam->ptGame, pQuest->eFilter, &pQuestRecord->tPlayers);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A1Q5CreditParty);
		SUnitIterate(pqParam->ptGame, 0, pqParam->hEventUnit, A1Q5FlagPlayer);
		QuestsAddTimer(pQuest, A1Q5Timer, 7);
	}
	A1Q5SpawnMonster(pQuest, pQuestRecord);
	if (!pQuestRecord->bMonsterSpawned)
		EventAdd(pQuest->ptGame, pqParam->hEventUnit, 7, pQuest->ptGame->nFrame + 10);
}

BOOL __fastcall A1Q5CreditInLevel(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	QUEST *pQuest;
	DWORD dwUnitId;
	A1Q5RECORD *pQuestRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 5, 0))
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 5, 1))
		return FALSE;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	if (D2CommonDungeonRoomGetLevelId(hRoom) != 25)
		return FALSE;
	pQuest = QuestsFind(ptGame, 5);
	if (!pQuest)
		return FALSE;
	pQuestRecord = (A1Q5RECORD *)pQuest->pQuestRecord;
	dwUnitId = hUnit->dwId;
	if (!QuestsPlayersFind(&pQuestRecord->tPlayers, dwUnitId))
		QuestsPlayersAdd(&pQuestRecord->tPlayers, dwUnitId);
	D2CommonQuestRecordSetBit(pRecord, 5, 0);
	D2CommonQuestRecordSetBit(pRecord, 5, 13);
	return FALSE;
}

BOOL __fastcall A1Q5CreditFinish(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	QUEST *pQuest;
	A1Q5RECORD *pQuestRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 5, 0) == 1)
		return FALSE;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	if (D2CommonDungeonRoomGetLevelId(hRoom) == 25)
	{
	D2CommonQuestRecordSetBit(pRecord, 5, 13);
	D2CommonQuestRecordSetBit(pRecord, 5, 0);
	D2CommonQuestRecordClearBit(pRecord, 5, 1);
	SUnitSetEvent(hUnit, 35, hUnit);
	pQuest = QuestsFind(ptGame, 5);
	if (!pQuest)
		return FALSE;
	pQuestRecord = (A1Q5RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->wCreditedCount < 12)
	{
		pQuestRecord->adwCreditedIds[pQuestRecord->wCreditedCount] = hUnit->dwId;
		pQuestRecord->wCreditedCount++;
	}
	return FALSE;
	}
	D2CommonQuestRecordSetBit(pRecord, 5, 14);
	return FALSE;
}

BOOL __fastcall A1Q5FlagPlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 5, 0) != 1)
	{
		D2CommonQuestRecordSetBit(pRecord, 5, 14);
		QuestsSendFlagInAct(hUnit, 5, 0);
	}
	return FALSE;
}

BOOL __fastcall A1Q5CreditParty(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecordX;
	WORD wParty;

	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecordX, 5, 13))
	{
		wParty = SUnitGetParty(hUnit);
		if (wParty != 65535)
			PartyForEach(ptGame, wParty, A1Q5PartyCredit, NULL);
	}
	return FALSE;
}

BYTE __fastcall A1Q5Timer(GAME *ptGame  , QUEST *pQuest
)
{
	A1Q5RECORD *pQuestRecord;

	pQuestRecord = (A1Q5RECORD *)pQuest->pQuestRecord;
	if (pQuest->bState == 5)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 13, NULL, A1Q5SendStatus, TRUE);
	}
	pQuestRecord->field_11c = 0;
	return TRUE;
}

void __fastcall A1Q5PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
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
	if (D2CommonStepTestGridBit(pRecord, 5, 4) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 5, 6) == 1)
	{
		pQuest->bStatus = 4;
		pQuest->bState = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 5, 5) == 1)
	{
		pQuest->bStatus = 3;
		pQuest->bState = 2;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 5, 3) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 3;
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 5, 2) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 2;
	}
}

void __fastcall A1Q5ObjInit(OBJINIT *ptInit  )
{
	QUEST *pQuestInfo;

	pQuestInfo = QuestsFind(ptInit->ptGame, 5);
#line 1059
	if (pQuestInfo) {
		if (!pQuestInfo->bActive)
			D2CommonUnitSetMode(ptInit->hObject, 3); return; }
#line 1059
	D2ASSERT(pQuestInfo);
}
#line 1063

void __fastcall A1Q5AddObject(OBJINIT *ptInit  )
{
	QUEST *pQuest;
	A1Q5RECORD *pQuestRecord;
	DWORD dwUnitId;
	int i;

	pQuest = QuestsFind(ptInit->ptGame, 5);
	if (!pQuest)
		return;
	pQuestRecord = (A1Q5RECORD *)pQuest->pQuestRecord;
	dwUnitId = ptInit->hObject->dwId;
	for (i = 0; i < (int)pQuestRecord->wObjectCount; i++)
		if (pQuestRecord->adwObjectIds[i] == dwUnitId)
			return;
	if (pQuestRecord->wObjectCount < 8)
	{
		pQuestRecord->adwObjectIds[pQuestRecord->wObjectCount] = dwUnitId;
		pQuestRecord->wObjectCount++;
	}
	A1Q5SpawnMonster(pQuest, pQuestRecord);
	if (pQuestRecord->bFinished && !pQuestRecord->bMonsterSpawned)
		EventAdd(pQuest->ptGame, ptInit->hObject, 7, pQuest->ptGame->nFrame + 10);
}

void __fastcall A1Q6PartyJoined(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	DWORD dwLevelId;
	A1Q6RECORD *pQuestRecord;
	DWORD dwUnitId;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 6, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, 6, 1) == 1)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	dwLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
	if (!dwLevelId)
		return;
	if (D2CommonGetActNum(dwLevelId))
		return;
	pQuestRecord = (A1Q6RECORD *)QuestsFind(ptGame, 6)->pQuestRecord;
	dwUnitId = hUnit->dwId;
	QuestsPlayersAdd(&pQuestRecord->tPlayers1, dwUnitId);
	QuestsPlayersAdd(&pQuestRecord->tPlayers2, dwUnitId);
	QuestsPlayersAdd(&pQuestRecord->tPlayers3, dwUnitId);
	D2CommonQuestRecordSetBit(pRecord, 6, 13);
	D2CommonQuestRecordSetBit(pRecord, 6, 1);
	ClientSetStep(SUnitGetClient(hUnit), 1, ptGame->bDifficultyLevel);
}
