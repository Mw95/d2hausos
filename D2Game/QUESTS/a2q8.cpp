#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a2q8.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../PLAYER/PlrIntro.h"
#include "QUESTS.h"
#include "a2q8.h"
#include "a3q0.h"

static QUESTNPCTABLE sgtA2Q8Npcs[2] =
{
	{ { { 378, 303, { 0, 0 }, 1 } }, 1 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

void __fastcall A2Q8NpcMessage(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q8Talk(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A2Q8Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);

BYTE __fastcall A2Q8StatusFilter(QUEST *pQuest  , UNIT *hPlayer, void *pGameRecord,
	void *pPlayerRecord, BYTE *pbStatus)
{
	return FALSE;
}

BYTE __fastcall A2Q7Active(QUEST *pQuest  , int nNpcClass, UNIT *hPlayer,
	void *pPlayerRecord)
{
	void *pRecord;
	GAME *ptGame;

	if (nNpcClass != 377)
		return FALSE;
	ptGame = pQuest->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecord, 30, 0) && !D2CommonStepTestGridBit(pRecord, 30, 13))
		return TRUE;
	if (D2CommonStepTestGridBit(pRecord, 9, 13) == 1 && !D2CommonStepTestGridBit(pRecord, 30, 0))
		return TRUE;
	return FALSE;
}

void __fastcall A2Q8Init(QUEST *pQuest  )
{
	A2Q8RECORD *pRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->bState = 0;
	pQuest->bStatus = 0;
	pQuest->pCallback[11] = A2Q8NpcMessage;
	pQuest->pCallback[0] = A2Q8Talk;
	pQuest->ptNpcTables = sgtA2Q8Npcs;
	pQuest->bCallbacksEnabled = 1;
#line 150
	pRecord = (A2Q8RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A2Q8RECORD));
	pQuest->pQuestRecord = pRecord;
	pRecord->field_00 = 0;
	pRecord->field_01 = 0;
	pQuest->eFilter = 31;
	pQuest->pStatusFilter = A2Q8StatusFilter;
	pQuest->pActiveFilter = A2Q8Active;
}

void __fastcall A2Q8NpcMessage(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	UNIT *hPlayer;
	void *pRecord;
	if ((WORD)pqParam->field_14 != 378 || (WORD)pqParam->field_18 != 303)
		return;
	ptGame = pqParam->ptGame;
	hPlayer = pqParam->hPlayer;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	D2CommonQuestRecordSetBit(pRecord, 31, 0);
}

void __fastcall A2Q8Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	UNIT *hPlayer;
	int nNpcClass;
	void *pRecord;
	QUEST *pQuest13;
	QUEST *pQuest10;

	nNpcClass = pqParam->hEventUnit->dwClassId;
	hPlayer = pqParam->hPlayer;
	ptGame = pQuest->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (nNpcClass != 378)
		return;
	pQuest13 = QuestsFind(pQuest->ptGame, 13);
	if (!pQuest13 || !pQuest13->bActive || pQuest13->bState >= 2)
		return;
	pQuest10 = QuestsFind(pQuest->ptGame, 10);
	if (pQuest10 && pQuest10->bActive && pQuest10->bState < 4)
		return;
	if (D2CommonStepTestGridBit(pRecord, 14, 1))
		return;
	if (D2CommonStepTestGridBit(pRecord, 14, 0))
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, 378, 0);
}

BYTE __fastcall A2Q8Active(QUEST *pQuest  , int nNpcClass, UNIT *hPlayer,
	void *pPlayerRecord)
{
	void *pRecord;
	QUEST *pQuest13;
	GAME *ptGame;

	if (nNpcClass != 378)
		return FALSE;
	ptGame = pQuest->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	pQuest13 = QuestsFind(pQuest->ptGame, 13);
	if (!pQuest13 || !pQuest13->bActive || pQuest13->bState >= 2)
		return FALSE;
	if (!QuestsGetRecordBit(pQuest->ptGame, 11, 13))
		return FALSE;
	if (!D2CommonStepTestGridBit(pRecord, 14, 1))
	{
		if (!D2CommonStepTestGridBit(pRecord, 14, 0))
			return TRUE;
	}
	return FALSE;
}
