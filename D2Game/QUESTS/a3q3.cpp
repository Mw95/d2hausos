#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a3q3.cpp"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/MonsterUnique.h"
#include "../OBJECTS/ObjMode.h"
#include "QUESTS.h"
#include "a3q3.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUNITNPC.h"
#include "../SKILLS/Skills.h"

#define A3Q2_CODE_FLAIL2	540173937
#define A3Q2_CODE_EYE		544826737
#define A3Q2_CODE_HEART		544368753
#define A3Q2_CODE_BRAIN		544367217
#define A3Q3_CODE_GOLD		543452263
#define A3Q3_CODE_ITEM		540226407
#define A3Q3_CODE_RING		544106866
#define A3Q4_CODE_BIRD		540291943
#define A3Q4_CODE_FIGURINE	540291946

static QUESTNPCTABLE sgtA3Q3Npcs[8] =
{
	{ { { 253, 571, { 0, 0 }, 1 } }, 1 },
	{ { { 254, 572, { 0, 0 }, 2 }, { 255, 573, { 0, 0 }, 2 }, { 264, 574, { 0, 0 }, 2 },
		{ 252, 575, { 0, 0 }, 2 }, { 253, 576, { 0, 0 }, 2 }, { 245, 577, { 0, 0 }, 2 },
		{ 297, 578, { 0, 0 }, 2 } }, 7 },
	{ { { 254, 579, { 0, 0 }, 2 }, { 255, 580, { 0, 0 }, 2 }, { 264, 581, { 0, 0 }, 2 },
		{ 252, 582, { 0, 0 }, 2 }, { 253, 583, { 0, 0 }, 2 }, { 245, 584, { 0, 0 }, 2 },
		{ 297, 585, { 0, 0 }, 2 } }, 7 },
	{ { { 254, 586, { 0, 0 }, 2 }, { 255, 587, { 0, 0 }, 1 }, { 264, 588, { 0, 0 }, 2 },
		{ 253, 590, { 0, 0 }, 2 }, { 245, 591, { 0, 0 }, 2 }, { 297, 592, { 0, 0 }, 2 } }, 6 },
	{ { { 264, 588, { 0, 0 }, 2 }, { 254, 586, { 0, 0 }, 2 }, { 245, 591, { 0, 0 }, 2 },
		{ 297, 592, { 0, 0 }, 2 } }, 4 },
	{ { { 264, 588, { 0, 0 }, 2 }, { 252, 589, { 0, 0 }, 1 }, { 254, 586, { 0, 0 }, 2 },
		{ 245, 591, { 0, 0 }, 2 }, { 297, 592, { 0, 0 }, 2 } }, 5 },
	{ { { 255, 593, { 0, 0 }, 1 } }, 1 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static int sgnA3Q3NpcTable[6] = { -1, 0, 1, 2, 3, 4 };

BOOL __fastcall A3Q2CountHeart(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A3Q2CountEye(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A3Q2CountBrain(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A3Q3StatusFilter(QUEST *pQuest, UNIT *hPlayer, void *pGameRecord, void *pPlayerRecord,
	BYTE *pbStatus);
BYTE __fastcall A3Q3Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall A3Q3PartyFlag(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A3Q3Activate(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A3Q3FlagPlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A3Q3SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A3Q3Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q3ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q3NpcMessage(QUEST *pQuestInfo, QUESTPARAM *pqParam);
BOOL __fastcall A3Q3FlagGiven(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A3Q3ItemAcquired(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A3Q3Sequence(QUEST *pQuestInfo);
void __fastcall A3Q3PlayerJoined(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q3PlayerDropped(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q3PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A3Q3Timer(GAME *ptGame, QUEST *pQuest);
void __fastcall A3Q3UnitKilled(QUEST *pQuest, QUESTPARAM *pqParam);

void __fastcall A3Q2ObjectOperated(QUEST *pQuest  , UNIT *hLever)
{
	A3Q2RECORD *pQuestRecord;
	UNIT *hObject;
	MISSOBJECTSTXT *ptObjectTxt;

	pQuestRecord = (A3Q2RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bStairsPresent != 1)
		return;
	hObject = SUnitFind(pQuest->ptGame, 2, pQuestRecord->dwStairsId);
	if (!hObject)
	{
		pQuestRecord->dwStairsMode = 2;
		return;
	}
	D2CommonUnitSetMode(hObject, pQuestRecord->dwStairsMode);
	if (pQuestRecord->dwStairsMode == 2)
		return;
	pQuestRecord->dwStairsMode = 2;
	ptObjectTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hObject->dwClassId);
	EventAdd(pQuest->ptGame, hObject, 1,
		(int)(((DWORD)ptObjectTxt->FrameCnt[1] >> 8) + pQuest->ptGame->nFrame));
}

void __fastcall A3Q2InitSewerStairs(OBJINIT *ptInit  )
{
	QUEST *pQuest;
	A3Q2RECORD *pQuestRecord;

	pQuest = QuestsFind(ptInit->ptGame, 16);
	if (!pQuest)
		return;
	pQuestRecord = (A3Q2RECORD *)pQuest->pQuestRecord;
	pQuestRecord->bStairsPresent = 1;
	pQuestRecord->dwStairsId = ptInit->hObject->dwId;
	if (!pQuest->bActive)
	{
		D2CommonUnitSetMode(ptInit->hObject, 2);
		return;
	}
	D2CommonUnitSetMode(ptInit->hObject, pQuestRecord->dwStairsMode);
}

void __fastcall A3Q2InitSewerLever(OBJINIT *ptInit  )
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptInit->ptGame, 16);
	if (pQuest && !pQuest->bActive)
		D2CommonUnitSetMode(ptInit->hObject, 2);
}

void __fastcall A3Q2FlailMade(GAME *ptGame  , UNIT *hUnit
)
{
	QUEST *pQuest;
	A3Q2RECORD *pQuestRecord;
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecord, 18, 0))
		D2CommonStepTestGridBit(pRecord, 18, 1);
	pQuest = QuestsFind(ptGame, 16);
	if (!pQuest)
		return;
	pQuestRecord = (A3Q2RECORD *)pQuest->pQuestRecord;
	pQuestRecord->nEyeCount--;
	pQuestRecord->nHeartCount--;
	pQuestRecord->nBrainCount--;
	pQuestRecord->nFlail1Count--;
	pQuestRecord->nFlail2Count++;
}

static __inline void A3Q2CountWithout(GAME *ptGame, UNIT *hUnit, DWORD dwCode)
{
	QUEST *pQuest;

	if (ITEMSFindByType(hUnit, dwCode))
		return;
	if (ITEMSFindByType(hUnit, A3Q2_CODE_FLAIL2))
		return;
	pQuest = QuestsFind(ptGame, 16);
	if (pQuest)
		((A3Q2RECORD *)pQuest->pQuestRecord)->nDropCount++;
}

BOOL __fastcall A3Q2NoFlail(GAME *ptGame  , UNIT *hUnit
)
{
	return ITEMSFindByType(hUnit, A3Q2_CODE_FLAIL2) == NULL;
}

int __fastcall A3Q2OperateHeartChest(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuest;
	A3Q2RECORD *pQuestRecord;
	int nLevel;
	int nGold;
	int i;

	if (!(!QuestsOperateObject(pObjectParam)))
	{
		nGold = (int)D2CommonSeedRand(QuestsGetSeed(pObjectParam->ptGame), 5) + 5;
		for (i = 0; i < nGold; i++)
			SkillsDropItem(pObjectParam->ptGame, pObjectParam->hObject, A3Q3_CODE_GOLD, 2);
		pObjectParam->hObject->dwDropItemCode = A3Q2_CODE_HEART;
		pQuest = QuestsFind(pObjectParam->ptGame, 16);
		if (pQuest)
		{
			pQuestRecord = (A3Q2RECORD *)pQuest->pQuestRecord;
			pQuestRecord->nDropCount = 0;
			SUnitIterate(pObjectParam->ptGame, 0, pObjectParam->hPlayer, A3Q2CountHeart);
			for (i = 0; i < pQuestRecord->nDropCount; i++)
			{
				if (ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 2, &nLevel, NULL, 44, TRUE))
				{
					pQuestRecord->bHeartDropped = 1;
					pQuestRecord->nHeartCount++;
				}
			}
		}
		ObjectDropOne(pObjectParam, 4);
		return 0;
	}
	return 0;
}

BOOL __fastcall A3Q2CountHeart(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	A3Q2CountWithout(ptGame, hUnit, A3Q2_CODE_HEART);
	return FALSE;
}

int __fastcall A3Q2OperateEyeChest(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuest;
	A3Q2RECORD *pQuestRecord;
	int nLevel;
	int nGold;
	int i;

	if (!(!QuestsOperateObject(pObjectParam)))
	{
		nGold = (int)D2CommonSeedRand(QuestsGetSeed(pObjectParam->ptGame), 5) + 5;
		for (i = 0; i < nGold; i++)
			SkillsDropItem(pObjectParam->ptGame, pObjectParam->hObject, A3Q3_CODE_GOLD, 2);
		pObjectParam->hObject->dwDropItemCode = A3Q2_CODE_EYE;
		pQuest = QuestsFind(pObjectParam->ptGame, 16);
		if (pQuest)
		{
			pQuestRecord = (A3Q2RECORD *)pQuest->pQuestRecord;
			pQuestRecord->nDropCount = 0;
			SUnitIterate(pObjectParam->ptGame, 0, pObjectParam->hPlayer, A3Q2CountEye);
			for (i = 0; i < pQuestRecord->nDropCount; i++)
			{
				if (ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 2, &nLevel, NULL, 44, TRUE))
				{
					pQuestRecord->bEyeDropped = 1;
					pQuestRecord->nEyeCount++;
				}
			}
		}
		ObjectDropOne(pObjectParam, 4);
		return 0;
	}
	return 0;
}

BOOL __fastcall A3Q2CountEye(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	A3Q2CountWithout(ptGame, hUnit, A3Q2_CODE_EYE);
	return FALSE;
}

int __fastcall A3Q2OperateBrainChest(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuest;
	A3Q2RECORD *pQuestRecord;
	int nLevel;
	int nGold;
	int i;

	if (!(!QuestsOperateObject(pObjectParam)))
	{
		nGold = (int)D2CommonSeedRand(QuestsGetSeed(pObjectParam->ptGame), 5) + 5;
		for (i = 0; i < nGold; i++)
			SkillsDropItem(pObjectParam->ptGame, pObjectParam->hObject, A3Q3_CODE_GOLD, 2);
		pObjectParam->hObject->dwDropItemCode = A3Q2_CODE_BRAIN;
		pQuest = QuestsFind(pObjectParam->ptGame, 16);
		if (pQuest)
		{
			pQuestRecord = (A3Q2RECORD *)pQuest->pQuestRecord;
			pQuestRecord->nDropCount = 0;
			SUnitIterate(pObjectParam->ptGame, 0, pObjectParam->hPlayer, A3Q2CountBrain);
			for (i = 0; i < pQuestRecord->nDropCount; i++)
			{
				if (ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 2, &nLevel, NULL, 44, TRUE))
				{
					pQuestRecord->bBrainDropped = 1;
					pQuestRecord->nBrainCount++;
				}
			}
		}
		ObjectDropOne(pObjectParam, 4);
		return 0;
	}
	return 0;
}

BOOL __fastcall A3Q2CountBrain(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	A3Q2CountWithout(ptGame, hUnit, A3Q2_CODE_BRAIN);
	return FALSE;
}

BYTE __fastcall A3Q3StatusFilter(QUEST *pQuest  , UNIT *hPlayer,
	void *pGameRecord, void *pPlayerRecord, BYTE *pbStatus)
{
	A3Q3RECORD *pQuestRecord;

	if (!D2CommonStepTestGridBit(pPlayerRecord, 15, 0))
	{
		*pbStatus = 0;
		return TRUE;
	}
	if (D2CommonStepTestGridBit(pPlayerRecord, 19, 0))
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 19, 13))
			*pbStatus = 13;
		else
			*pbStatus = 11;
		return TRUE;
	}
	pQuestRecord = (A3Q3RECORD *)pQuest->pQuestRecord;
	*pbStatus = 0;
	if (ITEMSFindByType(hPlayer, A3Q3_CODE_ITEM))
	{
		*pbStatus = 4;
		return TRUE;
	}
	if (pQuestRecord->bItemLost)
	{
		if (pQuest->ptGame->bGameType == 3)
			*pbStatus = 8;
		else
			*pbStatus = 7;
		return TRUE;
	}
	if (D2CommonStepTestGridBit(pPlayerRecord, 19, 6))
	{
		if (!D2CommonStepTestGridBit(pPlayerRecord, 19, 8))
			*pbStatus = 6;
		if (!D2CommonStepTestGridBit(pPlayerRecord, 19, 7))
			*pbStatus = 5;
		return TRUE;
	}
	if (D2CommonStepTestGridBit(pPlayerRecord, 19, 4))
	{
		*pbStatus = 3;
		return TRUE;
	}
	if (D2CommonStepTestGridBit(pPlayerRecord, 19, 3))
	{
		*pbStatus = 2;
		return TRUE;
	}
	if (D2CommonStepTestGridBit(pPlayerRecord, 19, 2))
	{
		*pbStatus = 1;
		return TRUE;
	}
	return TRUE;
}

BYTE __fastcall A3Q3Active(QUEST *pQuest  , int nNpcClass, UNIT *hPlayer,
	void *pPlayerRecord)
{
	if (D2CommonStepTestGridBit(pPlayerRecord, 19, 0))
		return FALSE;
	if (nNpcClass == 253)
	{
		if (ITEMSFindByType(hPlayer, A3Q3_CODE_ITEM))
			return FALSE;
		if (pQuest->bState == 1) return TRUE;
	}
	if (nNpcClass == 255)
	{
		if (ITEMSFindByType(hPlayer, A3Q3_CODE_ITEM))
			return TRUE;
		if (!D2CommonStepTestGridBit(pPlayerRecord, 19, 6))
			return FALSE;
		if (!D2CommonStepTestGridBit(pPlayerRecord, 19, 8)) return TRUE;
	}
	if (nNpcClass == 252)
	{
		if (!D2CommonStepTestGridBit(pPlayerRecord, 19, 6))
			return FALSE;
		if (!D2CommonStepTestGridBit(pPlayerRecord, 19, 7)) return TRUE;
	}
	return FALSE;
}

void __fastcall A3Q3PartyFlag(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;

	if (!QuestsFind(ptGame, 17))
		return;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 19, 0))
		return;
	if (D2CommonStepTestGridBit(pRecord, 19, 6))
		return;
	if (D2CommonStepTestGridBit(pRecord, 19, 8))
		return;
	if (D2CommonStepTestGridBit(pRecord, 19, 7))
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	if (D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom)) != 2)
		return;
	D2CommonQuestRecordSetBit(pRecord, 19, 6);
}

void __fastcall A3Q3Init(QUEST *pQuest  )
{
	A3Q3RECORD *pQuestRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->bState = 0;
	pQuest->ptNpcTables = sgtA3Q3Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->eFilter = 19;
	pQuest->pActiveFilter = A3Q3Active;
	pQuest->bDoneStateMin = 4;
	pQuest->pStatusFilter = A3Q3StatusFilter;
	pQuest->pSequence = A3Q3Sequence;
	pQuest->dwNextQuestId = 15;
	pQuest->pCallback[4] = A3Q3ItemAcquired;
	pQuest->pCallback[0] = A3Q3Talk;
	pQuest->pCallback[11] = A3Q3NpcMessage;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A3Q3ChangedLevel;
	pQuest->pCallback[2] = A3Q3Activate;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A3Q3PlayerStarted;
	pQuest->pCallback[QUESTCALLBACK_PLAYERJOINEDGAME] = A3Q3PlayerJoined;
	pQuest->pCallback[QUESTCALLBACK_PLAYERDROPPEDWITHQUESTITEM] = A3Q3PlayerDropped;
#line 883
	pQuestRecord = (A3Q3RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A3Q3RECORD));
	pQuest->pQuestRecord = pQuestRecord;
	for (i = 0; i < (int)(sizeof(A3Q3RECORD) / sizeof(DWORD)); i++)
		((DWORD *)pQuestRecord)[i] = 0;
	pQuestRecord->dwAltarMode = 0;
}

void __fastcall A3Q3Activate(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q3RECORD *pQuestRecord;
	WORD wNpcClass;

	wNpcClass = (WORD)pqParam->hEventUnit->dwClassId;
	pQuestRecord = (A3Q3RECORD *)pQuest->pQuestRecord;
	if (wNpcClass == 253)
	{
		if (pQuestRecord->bStatus2Pending != 1)
			return;
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 2, NULL, A3Q3SendStatus, TRUE);
		pQuestRecord->bStatus2Pending = 0;
		SUnitIterate(pQuest->ptGame, 0, NULL, A3Q3FlagPlayer);
		return;
	}
	if (wNpcClass == 255 && pQuestRecord->bItemHandedIn)
	{
		pQuestRecord->bItemHandedIn = 0;
		pQuestRecord->bOpenGatePending = 1;
	}
}

BOOL __fastcall A3Q3FlagPlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	QUEST *pQuest;
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 19, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 19, 6) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 17);
	if (!pQuest)
		return FALSE;
	if (ITEMSFindByType(hUnit, A3Q3_CODE_ITEM))
	{
		D2CommonQuestRecordSetBit(pRecord, 19, 5);
		return FALSE;
	}
	if (((A3Q3RECORD *)pQuest->pQuestRecord)->bItemDropped)
	{
		D2CommonQuestRecordSetBit(pRecord, 19, 4);
		if (D2CommonStepTestGridBit(pRecord, 19, 3))
			return FALSE;
		D2CommonQuestRecordSetBit(pRecord, 19, 2);
		return FALSE;
	}
	if (pQuest->bState == 2)
		D2CommonQuestRecordSetBit(pRecord, 19, 3);
	return FALSE;
}

BOOL __fastcall A3Q3SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 19, 0) || D2CommonStepTestGridBit(pRecord, 19, 15))
		&& !D2CommonStepTestGridBit(pRecord, 19, 13)
		&& !D2CommonStepTestGridBit(pRecord, 19, 14))
		return FALSE;
	QuestsSendStatus(ptGame, hUnit, 17);
	return FALSE;
}

void __fastcall A3Q3Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	int nNpcClass;
	int nTable;
	BOOL fNo8;
	BOOL fNo7;
	DWORD dwBit0;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	dwBit0 = D2CommonStepTestGridBit(pRecord, 19, 0);
	nNpcClass = (int)pqParam->hEventUnit->dwClassId;
	if (dwBit0 == 1)
	{
		if (QuestsHasPlayer(pQuest, pqParam->hPlayer->dwId))
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 4);
		return;
	}
	if (ITEMSFindByType(pqParam->hPlayer, A3Q3_CODE_ITEM))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 3);
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 19, 6))
	{
		fNo8 = !D2CommonStepTestGridBit(pRecord, 19, 8);
		fNo7 = !D2CommonStepTestGridBit(pRecord, 19, 7);
		if (nNpcClass == 252)
		{
			if (fNo7)
			{
				QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 5);
				return;
			}
		}
		else if (nNpcClass == 255 && fNo8)
		{
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 6);
			return;
		}
		else if (fNo7)
		{
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 5);
			return;
		}
		if (fNo8)
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 6);
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 19, 5))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 4);
		return;
	}
	if (pQuest->bState >= 4)
		return;
	nTable = sgnA3Q3NpcTable[pQuest->bState];
	if (nTable == -1)
		return;
	if ((DWORD)nTable >= 8)
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, nTable);
}

void __fastcall A3Q3ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	GAME *ptGame;
	DWORD dwLevelNew;
	DWORD dwLevelOld;
	UNIT *hPlayer;

	dwLevelNew = pqParam->field_14;
	dwLevelOld = pqParam->field_18;
	if (dwLevelOld == 78 && pQuest->bActive && !pQuest->bState)
		pQuest->bState = 1;
	if (dwLevelNew != 75)
		return;
	QuestsPlayerLeft(pQuest, pqParam);
	if (pQuest->bState == 2)
	{
		hPlayer = pqParam->hPlayer;
		ptGame = pqParam->ptGame;
		pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))
			->apQuestRecords[ptGame->bDifficultyLevel];
		if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0) == 1)
			return;
		if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 15) == 1)
			return;
#line 465
		QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
		SUnitIterate(pQuest->ptGame, 0, NULL, A3Q3FlagPlayer);
		return;
	}
	if (pQuest->bState == 5)
		pQuest->bCallbacksEnabled = 0;
}

void __fastcall A3Q3NpcMessage(QUEST *pQuestInfo  , QUESTPARAM *pqParam
)
{
	A3Q3RECORD *pQuestRecord;
	void *pRecord;
	WORD wNpcClass;
	WORD wMessage;
	WORD wParty;
	GAME *ptGame;
	UNIT *hPlayer;

	wMessage = (WORD)pqParam->field_18;
	wNpcClass = (WORD)pqParam->field_14;
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 19, 0) == 1)
		return;
	if (wNpcClass == 255)
	{
		if (wMessage == 587)
		{
			if (ITEMSFindByType(pqParam->hPlayer, A3Q3_CODE_ITEM)
				&& D2CommonStepTestGridBit(pRecord, 19, 5)
				&& !D2CommonStepTestGridBit(pRecord, 19, 8))
			{
				pQuestRecord = (A3Q3RECORD *)pQuestInfo->pQuestRecord;
				pQuestRecord->bItemHandedIn = 1;
				QuestsRemoveItem(pqParam->ptGame, pqParam->hPlayer, A3Q3_CODE_ITEM);
				((A3Q3RECORD *)pQuestInfo->pQuestRecord)->nItemCount--;
				D2CommonQuestRecordSetBit(pRecord, 19, 6);
				wParty = SUnitGetParty(pqParam->hPlayer);
				if (wParty != 65535)
					PartyForEach(pQuestInfo->ptGame, wParty, A3Q3PartyFlag, NULL);
				SUnitIterate(pQuestInfo->ptGame, 0, NULL, A3Q3FlagGiven);
			}
			if (pQuestInfo->bActive)
				QuestsSetRecordBit(pqParam->ptGame, 19, 13);
			pQuestInfo->bStatusFlags = 0;
			QuestsSetStatus(pQuestInfo, 13, NULL, A3Q3SendStatus, FALSE);
			if (!pQuestInfo->bActive)
				return;
			if (pQuestInfo->bState == 5)
				return;
#line 599
			QuestsSetState(pQuestInfo, 5, D2_THIS_FILE, __LINE__);
#line 600
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
			pQuestInfo->pSequence(pQuestInfo);
			return;
		}
		if (wMessage != 593)
			return;
		if (!D2CommonStepTestGridBit(pRecord, 19, 6))
			return;
		if (D2CommonStepTestGridBit(pRecord, 19, 8))
			return;
		D2CommonQuestRecordSetBit(pRecord, 19, 8);
		QuestsCreateItem(pqParam->ptGame, pqParam->hPlayer, A3Q3_CODE_RING, 21, 6);
		if (!D2CommonStepTestGridBit(pRecord, 19, 7))
			return;
		D2CommonQuestRecordSetBit(pRecord, 19, 13);
		D2CommonQuestRecordSetBit(pRecord, 19, 0);
		QuestsPlayersAdd(&pQuestInfo->tPlayers, pqParam->hPlayer->dwId);
		return;
	}
	if (wNpcClass == 253)
	{
		if (wMessage != 571)
			return;
#line 626
		QuestsSetState(pQuestInfo, 2, D2_THIS_FILE, __LINE__);
		((A3Q3RECORD *)pQuestInfo->pQuestRecord)->bStatus2Pending = 1;
		return;
	}
	if (wNpcClass != 252)
		return;
	if (wMessage != 589)
		return;
	if (D2CommonStepTestGridBit(pRecord, 19, 7))
		return;
	D2CommonQuestRecordSetBit(pRecord, 19, 7);
	SUnitNpcGiveHire(pQuestInfo->ptGame, pqParam->hPlayer, 252);
	if (!D2CommonStepTestGridBit(pRecord, 19, 8))
		return;
	D2CommonQuestRecordSetBit(pRecord, 19, 13);
	D2CommonQuestRecordSetBit(pRecord, 19, 0);
	QuestsPlayersAdd(&pQuestInfo->tPlayers, pqParam->hPlayer->dwId);
}

BOOL __fastcall A3Q3FlagGiven(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecord, 19, 0) && !D2CommonStepTestGridBit(pRecord, 19, 6)
		&& !D2CommonStepTestGridBit(pRecord, 19, 8) && !D2CommonStepTestGridBit(pRecord, 19, 7))
		D2CommonQuestRecordSetBit(pRecord, 19, 14);
	return FALSE;
}

void __fastcall A3Q3ItemAcquired(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	ITEMSTXT *ptItemStats;
	void *pRecord;
	GAME *ptGame;
	UNIT *hPlayer;

#line 660
	QuestsDebugMessage(pQuest->ptGame, "ginbinn acquired", D2_THIS_FILE, __LINE__);
	ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(pqParam->hEventUnit->dwClassId);
	if (pQuest->bActive && ptItemStats->code == A3Q3_CODE_ITEM)
	{
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 4, NULL, A3Q3SendStatus, TRUE);
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
		pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
		if (!D2CommonStepTestGridBit(pRecord, 19, 9))
		{
			D2CommonQuestRecordSetBit(pRecord, 19, 9);
			SUnitSetEvent(pqParam->hPlayer, 63, pqParam->hPlayer);
		}
#line 677
		QuestsSetState(pQuest, 4, D2_THIS_FILE, __LINE__);
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 3, NULL, A3Q3SendStatus, TRUE);
	}
	SUnitIterate(pQuest->ptGame, 0, NULL, A3Q3FlagPlayer);
}

BYTE __fastcall A3Q3Sequence(QUEST *pQuestInfo  )
{
	QUEST *pNextQuest;

	if (pQuestInfo->bState != 5 && pQuestInfo->bActive)
	{
		if (!pQuestInfo->bState)
			pQuestInfo->bState = 1;
		return TRUE;
	}
	pNextQuest = QuestsFind(pQuestInfo->ptGame, (int)pQuestInfo->dwNextQuestId);
	if (pNextQuest)
	{
#line 699
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		return pNextQuest->pSequence(pNextQuest);
	}
	return FALSE;
}

void __fastcall A3Q3PlayerJoined(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q3RECORD *pQuestRecord;

	pQuestRecord = (A3Q3RECORD *)pQuest->pQuestRecord;
	if (ITEMSFindByType(pqParam->hPlayer, A3Q3_CODE_ITEM))
		pQuestRecord->nItemCount++;
	if (pQuestRecord->bItemLost && pQuestRecord->nItemCount == 1)
		pQuestRecord->bItemLost = 0;
}

void __fastcall A3Q3PlayerDropped(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q3RECORD *pQuestRecord;

	pQuestRecord = (A3Q3RECORD *)pQuest->pQuestRecord;
	if (D2CommonITEMSGetTypeCode(pqParam->hEventUnit) != A3Q3_CODE_ITEM)
		return;
	if (--pQuestRecord->nItemCount)
		return;
	if (!pQuestRecord->bItemDropped)
		return;
	if (!pQuest->bActive)
		return;
	if (pQuest->bState >= 5)
		return;
	pQuestRecord->bItemLost = 1;
}

void __fastcall A3Q3PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q3RECORD *pQuestRecord;
	void *pRecord;
	UNIT *hItem;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	hItem = ITEMSFindByType(pqParam->hPlayer, A3Q3_CODE_ITEM);
	pQuestRecord = (A3Q3RECORD *)pQuest->pQuestRecord;
	if (hItem)
		pQuestRecord->nItemCount++;
	if (!D2CommonStepTestGridBit(pRecord, 19, 0))
	{
		if (hItem)
		{
			pQuest->bStatus = 4;
			pQuest->bState = 4;
			SUnitIterate(pQuest->ptGame, 0, NULL, A3Q3FlagPlayer);
			return;
		}
		if (D2CommonStepTestGridBit(pRecord, 19, 6) == 1)
		{
			pQuestRecord->dwAltarMode = 2;
			pQuest->bState = 5;
			if (!D2CommonStepTestGridBit(pRecord, 19, 7))
			{
				pQuest->bStatus = 5;
				return;
			}
			if (!D2CommonStepTestGridBit(pRecord, 19, 8))
			{
				pQuest->bStatus = 6;
				return;
			}
			D2CommonQuestRecordSetBit(pRecord, 19, 0);
			pQuest->bActive = 0;
			return;
		}
		if (D2CommonStepTestGridBit(pRecord, 19, 5) == 1)
		{
			pQuest->bState = 3;
			pQuest->bStatus = 2;
		}
		if (D2CommonStepTestGridBit(pRecord, 19, 4) == 1
			|| D2CommonStepTestGridBit(pRecord, 19, 3) == 1)
		{
			pQuest->bState = 2;
			pQuest->bStatus = 2;
			return;
		}
		if (D2CommonStepTestGridBit(pRecord, 19, 2) == 1)
		{
			pQuest->bState = 3;
			pQuest->bStatus = 1;
		}
	}
	else
		pQuestRecord->dwAltarMode = 2;
}

void __fastcall A3Q3InitGidbinnDecoy(OBJINIT *ptInit  )
{
	QUEST *pQuest;
	A3Q3RECORD *pQuestRecord;
	UNIT *hBoss;
	GAME *ptGame;
	GAMEROOM *hRoom;

	pQuest = QuestsFind(ptInit->ptGame, 17);
	if (pQuest && pQuest->bActive)
	{
		pQuestRecord = (A3Q3RECORD *)pQuest->pQuestRecord;
		pQuestRecord->bDecoyPresent = 1;
		pQuestRecord->nDecoyX = ptInit->nX;
		pQuestRecord->nDecoyY = ptInit->nY;
		if (!pQuestRecord->bObjectOperated)
			return;
		if (pQuestRecord->bBossSpawned)
			return;
		hRoom = ptInit->hRoom;
		ptGame = ptInit->ptGame;
		pQuestRecord->bSpawningBoss = 1;
		hBoss = MonsterUniqueSpawnBoss(ptGame, hRoom, NULL, 407, TRUE);
		if (hBoss)
		{
			pQuestRecord->bObjectOperated = 0;
			pQuestRecord->bBossSpawned = 1;
			pQuestRecord->dwBossId = hBoss->dwId;
		}
		pQuestRecord->bSpawningBoss = 0;
		return;
	}
	if (pQuest)
	{
		D2CommonUnitSetMode(ptInit->hObject, 2);
		return;
	}
	D2CommonUnitSetMode(ptInit->hObject, 2);
}

int __fastcall A3Q3OperateObject(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuest;
	A3Q3RECORD *pQuestRecord;
	MISSOBJECTSTXT *ptObjectTxt;

	if (pObjectParam->hObject->dwMode)
		return 0;
	pQuest = QuestsFind(pObjectParam->ptGame, 17);
	if (pQuest && pQuest->bActive)
	{
		if (!pQuest->bState)
			pQuest->bState = 1;
		pQuestRecord = (A3Q3RECORD *)pQuest->pQuestRecord;
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		ptObjectTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
		EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
			(int)(((DWORD)ptObjectTxt->FrameCnt[1] >> 8) + pObjectParam->ptGame->nFrame));
		pQuestRecord->bObjectOperated = 1;
		if (!pQuestRecord->bTimerPending)
		{
			pQuestRecord->bTimerPending = 1;
			QuestsAddTimer(pQuest, A3Q3Timer, 7);
		}
	}
	return 0;
}

BYTE __fastcall A3Q3Timer(GAME *ptGame  , QUEST *pQuest
)
{
	A3Q3RECORD *pQuestRecord;
	GAMEROOM *hRoom;
	GAMEROOM **phNear;
	DWORD dwNear;
	DWORD i;
	UNIT *hUnit;
	UNIT *hBoss;

	pQuestRecord = (A3Q3RECORD *)pQuest->pQuestRecord;
	pQuestRecord->bTimerPending = 0;
	if (pQuestRecord->bDecoyPresent != 1 || !pQuestRecord->bObjectOperated || pQuestRecord->bBossSpawned
		|| !ptGame->ptAct[2])
		return TRUE;
	hRoom = (GAMEROOM *)D2CommonDungeonActFindRoomByPoint(ptGame->ptAct[2], pQuestRecord->nDecoyX,
		pQuestRecord->nDecoyY);
	if (!hRoom)
		return TRUE;
	D2CommonDungeonRoomGetNear(hRoom, &phNear, &dwNear);
	for (i = 0; i < dwNear; i++)
	{
		for (hUnit = phNear[i]->ptUnitFirst; hUnit; hUnit = hUnit->ptRoomNext)
		{
			if (hUnit->eType == 0)
			{
				pQuestRecord->bSpawningBoss = 1;
				hBoss = MonsterUniqueSpawnBoss(ptGame, hRoom, NULL, 407, TRUE);
				if (hBoss)
				{
					pQuestRecord->bObjectOperated = 0;
					pQuestRecord->bBossSpawned = 1;
					pQuestRecord->dwBossId = hBoss->dwId;
				}
				pQuestRecord->bSpawningBoss = 0;
				pQuest->pCallback[8] = A3Q3UnitKilled;
				return TRUE;
			}
		}
	}
	return TRUE;
}

void __fastcall A3Q3UnitKilled(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q3RECORD *pQuestRecord;
	struct MONSTERUNIQUE *ptUnique;
	int nLevel;

#line 946
	QuestsDebugMessage(pQuest->ptGame, "killed boss for quest", D2_THIS_FILE, __LINE__);
	pQuestRecord = (A3Q3RECORD *)pQuest->pQuestRecord;
	if (!pQuestRecord->bBossSpawned)
		return;
	if (!pQuest->bActive)
		return;
	ptUnique = ((MONSTERDATA *)pqParam->hEventUnit->uTypeData)->ptUnique;
	if (!MonsterUniqueIsSuperUnique(ptUnique) && !D2CommonMonsterRegionUnitIsBossRaw(0,
		(DWORD)pqParam->hEventUnit) && !MonsterUniqueIsChampion(ptUnique)
			&& !MonsterUniqueIsCounted(ptUnique))
		return;
	pqParam->hEventUnit->dwDropItemCode = A3Q3_CODE_ITEM;
	if (ITEMSDropFromUnit(pQuest->ptGame, pqParam->hEventUnit, 2, &nLevel, NULL, 44, FALSE))
	{
		pQuestRecord->bItemDropped = 1;
		SUnitIterate(pQuest->ptGame, 0, NULL, A3Q3FlagPlayer);
		pQuest->pCallback[8] = NULL;
		return;
	}
	pQuestRecord->bBossSpawned = 0;
}

BOOL __fastcall A3Q3GetSpot(GAME *ptGame  , GAMECOORD *ptCoord)
{
	QUEST *pQuest;
	A3Q3RECORD *pQuestRecord;

	pQuest = QuestsFind(ptGame, 17);
	if (pQuest) {
		pQuestRecord = (A3Q3RECORD *)pQuest->pQuestRecord;
		if (!pQuestRecord->bOpenGatePending)
			return FALSE;
		ptCoord->dwX = (DWORD)pQuestRecord->nAltarX;
		ptCoord->dwY = (DWORD)pQuestRecord->nAltarY;
		return TRUE; }
	return FALSE;
}

void __fastcall A3Q3OpenGate(GAME *ptGame  )
{
	QUEST *pQuest;
	A3Q3RECORD *pQuestRecord;
	UNIT *hObject;
	MISSOBJECTSTXT *ptObjectTxt;

	pQuest = QuestsFind(ptGame, 17);
	if (!pQuest)
		return;
	pQuestRecord = (A3Q3RECORD *)pQuest->pQuestRecord;
	pQuestRecord->bOpenGatePending = 0;
	hObject = SUnitFind(ptGame, 2, pQuestRecord->dwAltarId);
	if (!hObject)
		return;
	D2CommonUnitSetMode(hObject, 1);
	ptObjectTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hObject->dwClassId);
	EventAdd(ptGame, hObject, 1, (int)(((DWORD)ptObjectTxt->FrameCnt[1] >> 8) + ptGame->nFrame + 1));
	pQuestRecord->dwAltarMode = 2;
}

void __fastcall A3Q3InitGidbinnAltar(OBJINIT *ptInit  )
{
	QUEST *pQuest;
	A3Q3RECORD *pQuestRecord;

	pQuest = QuestsFind(ptInit->ptGame, 17);
	if (!pQuest)
		return;
	pQuestRecord = (A3Q3RECORD *)pQuest->pQuestRecord;
	pQuestRecord->dwAltarId = ptInit->hObject->dwId;
	pQuestRecord->nAltarX = ptInit->nX;
	pQuestRecord->nAltarY = ptInit->nY;
	D2CommonUnitSetMode(ptInit->hObject, pQuestRecord->dwAltarMode);
}

BYTE __fastcall A3Q4Active(QUEST *pQuest  , int nNpcClass, UNIT *hPlayer,
	void *pPlayerRecord)
{
	if (D2CommonStepTestGridBit(pPlayerRecord, 20, 0))
		return FALSE;
	if (D2CommonStepTestGridBit(pPlayerRecord, 20, 1))
		return FALSE;
	if (nNpcClass == 254)
	{
		if (ITEMSFindByType(hPlayer, A3Q4_CODE_BIRD))
			return TRUE;
		if (D2CommonStepTestGridBit(pPlayerRecord, 20, 1))
			return TRUE;
	}
	if (nNpcClass == 264)
	{
		if (ITEMSFindByType(hPlayer, A3Q4_CODE_FIGURINE))
			return TRUE;
	}
	if (nNpcClass == 245)
	{
		if (ITEMSFindByType(hPlayer, A3Q4_CODE_FIGURINE)
			&& !D2CommonStepTestGridBit(pPlayerRecord, 20, 2))
			return TRUE;
		if (ITEMSFindByType(hPlayer, A3Q4_CODE_BIRD)
			&& !D2CommonStepTestGridBit(pPlayerRecord, 20, 4))
			return TRUE;
	}
	return FALSE;
}
