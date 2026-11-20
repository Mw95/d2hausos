#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a3q2.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "../GAME/SCmd.h"
#include "../ITEMS/Items.h"
#include "QUESTS.h"
#include "a2q2.h"
#include "a2q3.h"
#include "a2q6.h"
#include "a3q1.h"
#include "a3q2.h"
#include "a3q7.h"
#include "../UNIT/SUnit.h"

#define A3Q2_CODE_FLAIL1	540108401
#define A3Q2_CODE_FLAIL2	540173937
#define A3Q2_CODE_EYE		544826737
#define A3Q2_CODE_HEART		544368753
#define A3Q2_CODE_BRAIN		544367217

#pragma pack(push, 1)
struct A3Q2PARTCMD
{
 	BYTE		bCmd;
 	BYTE		bQuest;
 	BYTE		bStatusFlags;
 	BYTE		bStatus;
 	BYTE		unobserved_04;
};
#pragma pack(pop)
typedef struct A3Q2PARTCMD A3Q2PARTCMD;

static QUESTNPCTABLE sgtA3Q2Npcs[13] =
{
	{ { { 245, 543, { 0, 0 }, 1 } }, 1 },
	{ { { 245, 545, { 0, 0 }, 1 } }, 1 },
	{ { { 245, 544, { 0, 0 }, 1 } }, 1 },
	{ { { 245, 546, { 0, 0 }, 1 } }, 1 },
	{ { { 245, 547, { 0, 0 }, 1 } }, 1 },
	{ { { 245, 548, { 0, 0 }, 1 } }, 1 },
	{ { { 245, 543, { 0, 0 }, 2 } }, 1 },
	{ { { 245, 545, { 0, 0 }, 2 } }, 1 },
	{ { { 245, 544, { 0, 0 }, 2 } }, 1 },
	{ { { 245, 546, { 0, 0 }, 2 } }, 1 },
	{ { { 245, 547, { 0, 0 }, 2 } }, 1 },
	{ { { 245, 548, { 0, 0 }, 2 } }, 1 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

void __fastcall A3Q2Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q2ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A3Q2StatusFilter(QUEST *pQuest, UNIT *hPlayer, void *pGameRecord, void *pPlayerRecord,
	BYTE *pbStatus);
void __fastcall A3Q2NpcMessage(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q2Activate(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A3Q2SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A3Q2Sequence(QUEST *pQuestInfo);
void __fastcall A3Q2ItemAcquired(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q2PlayerJoined(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q2PlayerDropped(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q2PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);

void __fastcall A3Q2Init(QUEST *pQuest  )
{
	A3Q2RECORD *pQuestRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->ptNpcTables = sgtA3Q2Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->bStatus = 1;
	pQuest->bState = 0;
	pQuest->eFilter = 18;
	pQuest->bDoneStateMin = 0;
	pQuest->pStatusFilter = A3Q2StatusFilter;
	pQuest->pActiveFilter = A3Q2Active;
	pQuest->pSequence = A3Q2Sequence;
	pQuest->dwNextQuestId = 15;
	pQuest->pCallback[0] = A3Q2Talk;
	pQuest->pCallback[11] = A3Q2NpcMessage;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A3Q2ChangedLevel;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A3Q2PlayerStarted;
	pQuest->pCallback[10] = A2Q2UnitKilled;
	pQuest->pCallback[4] = A3Q2ItemAcquired;
	pQuest->pCallback[QUESTCALLBACK_PLAYERJOINEDGAME] = A3Q2PlayerJoined;
	pQuest->pCallback[QUESTCALLBACK_PLAYERDROPPEDWITHQUESTITEM] = A3Q2PlayerDropped;
#line 910
	pQuestRecord = (A3Q2RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A3Q2RECORD));
	pQuest->pQuestRecord = pQuestRecord;
	for (i = 0; i < (int)(sizeof(A3Q2RECORD) / sizeof(DWORD)); i++)
		((DWORD *)pQuestRecord)[i] = 0;
	pQuestRecord->dwStairsMode = 0;
	QuestsPlayersClear(&pQuestRecord->tPlayers);
}

static __inline BYTE A3Q2FlailFree(QUEST *pQuest19)
{
	if (!pQuest19)
		return TRUE;
	return !((A3Q5RECORD *)pQuest19->pQuestRecord)->bFlailPlaced;
}

void __fastcall A3Q2Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	QUEST *pQuest19;
	int nNpcClass;
	int nHeld;
	BOOL fFlail1;
	BOOL fFlail2;
	BOOL fEye;
	BOOL fHeart;
	BOOL fBrain;
	GAME *ptGame = pqParam->ptGame;
	PLRINFO *ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer);

	pRecord = ptInfo->apQuestRecords[ptGame->bDifficultyLevel];
	nNpcClass = pqParam->hEventUnit->dwClassId;
	if (nNpcClass != 245)
		return;
	if (pQuest->bState >= 1 && !D2CommonStepTestGridBit(pRecord, 18, 2))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 0);
		return;
	}
	nHeld = 0;
	fFlail2 = FALSE;
	fFlail1 = FALSE;
	fEye = FALSE;
	fHeart = FALSE;
	fBrain = FALSE;
	if (ITEMSFindByType(pqParam->hPlayer, A3Q2_CODE_FLAIL2))
	{
		fFlail2 = TRUE;
		if (!D2CommonStepTestGridBit(pRecord, 18, 7))
		{
			pQuest19 = QuestsFind(pQuest->ptGame, 19);
			if (A3Q2FlailFree(pQuest19))
			{
				QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 5);
				return;
			}
		}
	}
	if (ITEMSFindByType(pqParam->hPlayer, A3Q2_CODE_FLAIL1))
	{
		nHeld = 1;
		fFlail1 = TRUE;
		if (!D2CommonStepTestGridBit(pRecord, 18, 5))
		{
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 4);
			return;
		}
	}
	if (ITEMSFindByType(pqParam->hPlayer, A3Q2_CODE_EYE))
	{
		nHeld++;
		fEye = TRUE;
		if (!D2CommonStepTestGridBit(pRecord, 18, 3))
		{
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 1);
			return;
		}
	}
	if (ITEMSFindByType(pqParam->hPlayer, A3Q2_CODE_HEART))
	{
		nHeld++;
		fHeart = TRUE;
		if (!D2CommonStepTestGridBit(pRecord, 18, 6))
		{
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 2);
			return;
		}
	}
	if (ITEMSFindByType(pqParam->hPlayer, A3Q2_CODE_BRAIN))
	{
		nHeld++;
		fBrain = TRUE;
		if (!D2CommonStepTestGridBit(pRecord, 18, 4))
		{
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 3);
			return;
		}
	}
	if (nHeld > 0)
	{
		if (fFlail1)
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 10);
		else if (fHeart)
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 8);
		else if (fEye)
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 7);
		else if (fBrain)
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 9);
		else
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 6);
	}
	else if (fFlail2)
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 11);
	else if (D2CommonStepTestGridBit(pRecord, 18, 2))
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 6);
}

void __fastcall A3Q2ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	if (pqParam->field_18 != 77)
		return;
	if (!pQuest->bActive)
		return;
	if (pQuest->bState >= 3)
		return;
#line 335
	QuestsSetState(pQuest, 1, D2_THIS_FILE, __LINE__);
}

BYTE __fastcall A3Q2StatusFilter(QUEST *pQuest  , UNIT *hPlayer,
	void *pGameRecord, void *pPlayerRecord, BYTE *pbStatus)
{
	DWORD dwBit2;
	int nHeld;
	BOOL fFlail1;
	BOOL fEye;
	BOOL fHeart;
	BOOL fBrain;

	*pbStatus = 0;
	if (!D2CommonStepTestGridBit(pPlayerRecord, 15, 0))
		return TRUE;
	dwBit2 = D2CommonStepTestGridBit(pPlayerRecord, 18, 2);
	if (dwBit2 || (pQuest->bActive && pQuest->bState >= 2))
		*pbStatus = 1;
	nHeld = 0;
	if (ITEMSFindByType(hPlayer, A3Q2_CODE_FLAIL2))
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 18, 7))
			*pbStatus = 6;
		else
			*pbStatus = 5;
		return TRUE;
	}
	fFlail1 = FALSE;
	fEye = FALSE;
	fHeart = FALSE;
	fBrain = FALSE;
	if (ITEMSFindByType(hPlayer, A3Q2_CODE_FLAIL1))
	{
		nHeld = 1;
		fFlail1 = TRUE;
	}
	if (ITEMSFindByType(hPlayer, A3Q2_CODE_EYE))
	{
		nHeld++;
		fEye = TRUE;
	}
	if (ITEMSFindByType(hPlayer, A3Q2_CODE_HEART))
	{
		nHeld++;
		fHeart = TRUE;
	}
	if (ITEMSFindByType(hPlayer, A3Q2_CODE_BRAIN))
	{
		nHeld++;
		fBrain = TRUE;
	}
	if (nHeld == 4)
	{
		if (D2CommonStepTestGridBit(pPlayerRecord, 18, 5))
			*pbStatus = 5;
		else
			*pbStatus = 7;
		return TRUE;
	}
	if (nHeld)
	{
		if (!fEye)
			*pbStatus = 1;
		else if (!fBrain)
			*pbStatus = 2;
		else if (!fHeart)
			*pbStatus = 4;
		else if (fFlail1)
			*pbStatus = 7;
		else
			*pbStatus = 3;
		return TRUE;
	}
	if (dwBit2)
	{
		*pbStatus = 1;
		return TRUE;
	}
	*pbStatus = pQuest->bState > 1;
	return TRUE;
}

void __fastcall A3Q2NpcMessage(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	WORD wNpcClass;
	WORD wMessage;
	GAME *ptGame;
	UNIT *hPlayer;

	hPlayer = pqParam->hPlayer;
	wNpcClass = (WORD)pqParam->field_14;
	wMessage = (WORD)pqParam->field_18;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))

		->apQuestRecords[ptGame->bDifficultyLevel];
	if (wNpcClass != 245)
		return;
	switch (wMessage)
	{
	case 543:
#line 497
		QuestsSetState(pQuest, 2, D2_THIS_FILE, __LINE__);
		D2CommonQuestRecordSetBit(pRecord, 18, 2);
		((A3Q2RECORD *)pQuest->pQuestRecord)->bStatus1Pending = 1;
		pQuest->pCallback[2] = A3Q2Activate;
		break;
	case 545:
		D2CommonQuestRecordSetBit(pRecord, 18, 3);
		break;
	case 544:
		D2CommonQuestRecordSetBit(pRecord, 18, 6);
		break;
	case 546:
		D2CommonQuestRecordSetBit(pRecord, 18, 4);
		break;
	case 547:
		D2CommonQuestRecordSetBit(pRecord, 18, 5);
		break;
	case 548:
		D2CommonQuestRecordSetBit(pRecord, 18, 7);
		break;
	}
}

void __fastcall A3Q2Activate(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	if ((WORD)pqParam->hEventUnit->dwClassId != 245)
		return;
	if (((A3Q2RECORD *)pQuest->pQuestRecord)->bStatus1Pending != 1)
		return;
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 1, NULL, A3Q2SendStatus, TRUE);
	pQuest->pCallback[2] = NULL;
}

BOOL __fastcall A3Q2SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecord, 18, 0) && !D2CommonStepTestGridBit(pRecord, 18, 15))
		QuestsSendStatus(ptGame, hUnit, 16);
	return FALSE;
}

BYTE __fastcall A3Q2Sequence(QUEST *pQuestInfo  )
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
#line 532
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		return pNextQuest->pSequence(pNextQuest);
	}
	return FALSE;
}

static __inline void A3Q2SendPart(QUEST *pQuest, QUESTPARAM *pqParam, BYTE bStatus)
{
	A3Q2PARTCMD tCmd;

	BYTE bFlags;

	bFlags = pQuest->bStatusFlags;
	tCmd.bCmd = 86;
	tCmd.bStatusFlags = bFlags;
	tCmd.bQuest = 16;
	tCmd.bStatus = bStatus;
	SCmdSendQuestStatus(SUnitGetClient(pqParam->hPlayer), &tCmd);
}

void __fastcall A3Q2ItemAcquired(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	DWORD dwCode;
	BYTE fNoBit5;
	BYTE fNoBit7;
	BYTE fFlail1;
	BYTE fEye;
	BYTE fBrain;
	BYTE fHeart;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecord, 15, 0))
		return;
	if (D2CommonStepTestGridBit(pRecord, 18, 0))
		return;
	dwCode = D2CommonITEMSGetTypeCode(pqParam->hEventUnit);
	fNoBit5 = FALSE;
	fNoBit7 = FALSE;
	D2CommonStepTestGridBit(pRecord, 18, 2);
	D2CommonStepTestGridBit(pRecord, 18, 3);
	D2CommonStepTestGridBit(pRecord, 18, 4);
	if (!D2CommonStepTestGridBit(pRecord, 18, 5))
		fNoBit5 = TRUE;
	D2CommonStepTestGridBit(pRecord, 18, 6);
	if (!D2CommonStepTestGridBit(pRecord, 18, 7))
		fNoBit7 = TRUE;
	fEye = FALSE;
	fHeart = FALSE;
	fBrain = FALSE;
	fFlail1 = FALSE;
	if (ITEMSFindByType(pqParam->hPlayer, A3Q2_CODE_FLAIL1))
		fFlail1 = TRUE;
	ITEMSFindByType(pqParam->hPlayer, A3Q2_CODE_FLAIL2);
	if (ITEMSFindByType(pqParam->hPlayer, A3Q2_CODE_EYE))
		fEye = TRUE;
	if (ITEMSFindByType(pqParam->hPlayer, A3Q2_CODE_BRAIN))
		fBrain = TRUE;
	if (ITEMSFindByType(pqParam->hPlayer, A3Q2_CODE_HEART))
		fHeart = TRUE;
	if (dwCode == A3Q2_CODE_FLAIL2)
	{
		if (pQuest->bStatus)
			pQuest->bStatus = 7;
		if (D2CommonStepTestGridBit(pRecord, 15, 0))
			return;
		if (fNoBit7)
			A3Q2SendPart(pQuest, pqParam, 5);
		else
			A3Q2SendPart(pQuest, pqParam, 6);
		return;
	}
	if (dwCode == A3Q2_CODE_FLAIL1)
	{
		if (!fEye)
		{
			A3Q2SendPart(pQuest, pqParam, 1);
			return;
		}
		if (!fBrain)
		{
			A3Q2SendPart(pQuest, pqParam, 2);
			return;
		}
		if (!fHeart)
		{
			A3Q2SendPart(pQuest, pqParam, 4);
			return;
		}
		if (!fNoBit5)
			A3Q2SendPart(pQuest, pqParam, 7);
		else
			A3Q2SendPart(pQuest, pqParam, 5);
		return;
	}
	if (dwCode == A3Q2_CODE_EYE)
	{
		if (!fBrain)
		{
			A3Q2SendPart(pQuest, pqParam, 2);
			return;
		}
		if (!fFlail1)
		{
			A3Q2SendPart(pQuest, pqParam, 3);
			return;
		}
		if (!fHeart)
		{
			A3Q2SendPart(pQuest, pqParam, 4);
			return;
		}
		A3Q2SendPart(pQuest, pqParam, 7);
		return;
	}
	if (dwCode == A3Q2_CODE_HEART)
	{
		if (!fBrain)
		{
			A3Q2SendPart(pQuest, pqParam, 2);
			return;
		}
		if (!fFlail1)
		{
			A3Q2SendPart(pQuest, pqParam, 3);
			return;
		}
		if (!fEye)
		{
			A3Q2SendPart(pQuest, pqParam, 1);
			return;
		}
		A3Q2SendPart(pQuest, pqParam, 7);
		return;
	}
	if (dwCode == A3Q2_CODE_BRAIN)
	{
		if (!fHeart)
		{
			A3Q2SendPart(pQuest, pqParam, 4);
			return;
		}
		if (!fFlail1)
		{
			A3Q2SendPart(pQuest, pqParam, 3);
			return;
		}
		if (!fEye)
		{
			A3Q2SendPart(pQuest, pqParam, 1);
			return;
		}
		A3Q2SendPart(pQuest, pqParam, 7);
	}
}

void __fastcall A3Q2PlayerJoined(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q2RECORD *pQuestRecord;
	A3Q5RECORD *pQuest19Record;
	QUEST *pQuest19;
	int nCount;
	BYTE bFlag;
	GAME *ptGame;

	pQuestRecord = (A3Q2RECORD *)pQuest->pQuestRecord;
	if (ITEMSFindByType(pqParam->hPlayer, A3Q2_CODE_EYE))
		pQuestRecord->nEyeCount++;
	if (ITEMSFindByType(pqParam->hPlayer, A3Q2_CODE_HEART))
		pQuestRecord->nHeartCount++;
	if (ITEMSFindByType(pqParam->hPlayer, A3Q2_CODE_BRAIN))
		pQuestRecord->nBrainCount++;
	if (ITEMSFindByType(pqParam->hPlayer, A3Q2_CODE_FLAIL1))
		pQuestRecord->nFlail1Count++;
	if (ITEMSFindByType(pqParam->hPlayer, A3Q2_CODE_FLAIL2))
		pQuestRecord->nFlail2Count++;
	if (!pQuestRecord->bPartsMissing)
		return;
	A2Q2BoxState(pQuest->ptGame, &nCount, &bFlag);
	if (pQuestRecord->nFlail2Count != 1)
	{
		if (pQuestRecord->nEyeCount <= 0)
			return;
		if (pQuestRecord->nHeartCount <= 0)
			return;
		if (pQuestRecord->nBrainCount <= 0)
			return;
		if (pQuestRecord->nFlail1Count <= 0)
			return;
		if (nCount <= 0)
			return;
	}
	pQuestRecord->bPartsMissing = 0;
	ptGame = pQuest->ptGame;
	pQuest19 = QuestsFind(ptGame, 19);
	if (!pQuest19)
		return;
	pQuest19Record = (A3Q5RECORD *)pQuest19->pQuestRecord;
	if (pQuest19Record->bFlailPlaced)
		return;
	if (pQuest19Record->bPartsMissingTold != 1)
		return;
	pQuest19Record->bPartsMissingTold = 0;
	A3Q7SetQuestStatus(ptGame, TRUE);
}

void __fastcall A3Q2PlayerDropped(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q2RECORD *pQuestRecord;
	QUEST *pQuest19;
	int nCount;
	BYTE bFlag;
	GAME *ptGame;

	pQuestRecord = (A3Q2RECORD *)pQuest->pQuestRecord;
	switch (D2CommonITEMSGetTypeCode(pqParam->hEventUnit))
	{
	case A3Q2_CODE_FLAIL2:
		pQuestRecord->nFlail2Count--;
		break;
	case A3Q2_CODE_FLAIL1:
		pQuestRecord->nFlail1Count--;
		break;
	case A3Q2_CODE_BRAIN:
		pQuestRecord->nBrainCount--;
		break;
	case A3Q2_CODE_EYE:
		pQuestRecord->nEyeCount--;
		break;
	case A3Q2_CODE_HEART:
		pQuestRecord->nHeartCount--;
		break;
	}
	A2Q2BoxState(pQuest->ptGame, &nCount, &bFlag);
	if (!pQuestRecord->nFlail2Count || (bFlag && nCount < 1)
		|| (pQuestRecord->nEyeCount < 1 && pQuestRecord->bEyeDropped)
		|| (pQuestRecord->nHeartCount < 1 && pQuestRecord->bHeartDropped)
		|| (pQuestRecord->nBrainCount < 1 && pQuestRecord->bBrainDropped)
		|| (pQuestRecord->nFlail1Count < 1 && pQuestRecord->bFlail1Dropped))
	{
		pQuestRecord->bPartsMissing = 1;
		ptGame = pQuest->ptGame;
		pQuest19 = QuestsFind(ptGame, 19);
		if (pQuest19 && !((A3Q5RECORD *)pQuest19->pQuestRecord)->bFlailPlaced)
		{
			((A3Q5RECORD *)pQuest19->pQuestRecord)->bPartsMissingTold = 1;
			A3Q7SetQuestStatus(ptGame, FALSE);
		}
	}
}

void __fastcall A3Q2PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A3Q2RECORD *pQuestRecord;
	void *pRecord;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 18, 0))
		return;
	if (!D2CommonStepTestGridBit(pRecord, 18, 7))
	{
		if (D2CommonStepTestGridBit(pRecord, 18, 2))
		{
			pQuest->bState = 2;
			pQuest->bStatus = 7;
		}
		A3Q2PlayerJoined(pQuest, pqParam);
		pQuestRecord = (A3Q2RECORD *)pQuest->pQuestRecord;
		if (!(pQuestRecord->nFlail1Count + pQuestRecord->nHeartCount + pQuestRecord->nBrainCount
			+ pQuestRecord->nEyeCount))
			return;
	}
	pQuest->bState = 2;
	pQuest->bStatus = 7;
}

int __fastcall A3Q2OperateTiles(OBJOPERATE *pObjectParam  )
{
	if (pObjectParam->hObject->dwMode == 2)
		A2Q6OperateTiles(pObjectParam);
	return 0;
}

int __fastcall A3Q2OperateObject(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuest;
	A3Q2RECORD *pQuestRecord;
	MISSOBJECTSTXT *ptObjectTxt;

	if (pObjectParam->hObject->dwMode)
		return 0;
	pQuest = QuestsFind(pObjectParam->ptGame, 16);
	if (!pQuest)
		return 0;
	pQuestRecord = (A3Q2RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bStairsPresent != 1)
		return 0;
	if (!SUnitFind(pObjectParam->ptGame, 2, pQuestRecord->dwStairsId))
		return 0;
	D2CommonUnitSetMode(pObjectParam->hObject, 1);
	ptObjectTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
	EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
		pObjectParam->ptGame->nFrame + ((DWORD)ptObjectTxt->FrameCnt[1] >> 8));
	pQuestRecord->dwStairsMode = 2;
	EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 7, pObjectParam->ptGame->nFrame + 30);
	QuestsSetAll(pQuest->ptGame, 9);
	return 0;
}
