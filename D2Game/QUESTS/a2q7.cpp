#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a2q7.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "QUESTS.h"
#include "a2q2.h"
#include "a2q7.h"
#include "a2q8.h"

static QUESTNPCTABLE sgtA2Q7Npcs[6] =
{
	{ { { 377, 59, { 0, 0 }, 1 } }, 1 },
	{ { { 377, 60, { 0, 0 }, 1 } }, 1 },
	{ { { 377, 61, { 0, 0 }, 1 } }, 1 },
	{ { { 377, 62, { 0, 0 }, 1 } }, 1 },
	{ { { 377, 63, { 0, 0 }, 1 } }, 1 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

void __fastcall A2Q7NpcMessage(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q7NpcActivated(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q7Talk(QUEST *pQuest, QUESTPARAM *pqParam);

void __fastcall A2Q7Init(QUEST *pQuest  )
{
	A2Q7RECORD *pQuestRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->pCallback[0] = A2Q7Talk;
	pQuest->bState = 0;
	pQuest->bStatus = 0;
	pQuest->pCallback[11] = A2Q7NpcMessage;
	pQuest->pCallback[8] = A2Q2UnitKilled;
	pQuest->ptNpcTables = sgtA2Q7Npcs;
	pQuest->bCallbacksEnabled = 1;
#line 194
	pQuestRecord = (A2Q7RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A2Q7RECORD));
	pQuest->pQuestRecord = pQuestRecord;
	pQuestRecord->field_00 = 0;
	pQuest->eFilter = 30;
	pQuest->pStatusFilter = A2Q8StatusFilter;
	pQuest->pActiveFilter = A2Q7Active;
}

void __fastcall A2Q7NpcMessage(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	WORD wMessage;
	void *pRecord;
	UNIT *hPlayer;
	GAME *ptGame;

	if ((WORD)pqParam->field_14 != 377)
		return;
	hPlayer = pqParam->hPlayer;
	wMessage = (WORD)pqParam->field_18;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (wMessage != 59 && wMessage != 60)
	{
		D2CommonQuestRecordSetBit(pRecord, 30, 0);
		return;
	}
	D2CommonQuestRecordSetBit(pRecord, 30, 13);
	pQuest->pCallback[2] = A2Q7NpcActivated;
}

void __fastcall A2Q7NpcActivated(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A2Q7RECORD *pQuestRecord;

	if ((WORD)pqParam->hEventUnit->dwClassId != 377)
		return;
	pQuestRecord = (A2Q7RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->field_00 != 1)
		return;
	pQuestRecord->field_00 = 0;
	pQuest->pCallback[2] = NULL;
}

void __fastcall A2Q7Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecord;
	QUEST *pQuest8;
	GAME *ptGame;
	DWORD nClass;
	UNIT *hPlayer;

	nClass = pqParam->hEventUnit->dwClassId;
	if (nClass != 377)
		return;
	if (nClass != 377)
		return;
	ptGame = pqParam->ptGame;
	hPlayer = pqParam->hPlayer;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecord, 30, 0) && !D2CommonStepTestGridBit(pRecord, 9, 0)
		&& !D2CommonStepTestGridBit(pRecord, 9, 13) && !D2CommonStepTestGridBit(pRecord, 9, 1)
		&& !QuestsGetRecordBit(pqParam->ptGame, 9, 13))
	{
		pQuest8 = QuestsFind(pQuest->ptGame, 8);
		if ((!pQuest8 || pQuest8->bActive) && !D2CommonStepTestGridBit(pRecord, 30, 13))
		{
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, 377,
				(int)D2CommonSeedRand(&pqParam->hPlayer->tSeed, 2));
			return;
		}
	}
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, 377,
		(int)D2CommonSeedRand(&pqParam->hPlayer->tSeed, 3) + 2);
}
