#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a2q0.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../PLAYER/PlrIntro.h"
#include "QUESTS.h"
#include "a1q4.h"
#include "a2q0.h"
#include "a2q1.h"
#include "a2q8.h"
#include "a3q0.h"

static QUESTNPCTABLE sgtA2IntroNpcs[3] =
{
	{ { { 199, 203, { 0, 0 }, 0 }, { 198, 190, { 0, 0 }, 0 }, { 176, 221, { 0, 0 }, 0 },
		{ 200, 230, { 0, 0 }, 0 }, { 210, 241, { 0, 0 }, 0 }, { 178, 263, { 0, 0 }, 0 },
		{ 175, 215, { 0, 0 }, 0 }, { 202, 274, { 0, 0 }, 0 }, { 177, 285, { 0, 0 }, 0 } }, 9 },
	{ { { 199, 204, { 0, 0 }, 0 }, { 198, 190, { 0, 0 }, 0 }, { 176, 221, { 0, 0 }, 0 },
		{ 200, 231, { 0, 0 }, 0 }, { 210, 242, { 0, 0 }, 0 }, { 178, 264, { 0, 0 }, 0 },
		{ 175, 215, { 0, 0 }, 0 }, { 202, 274, { 0, 0 }, 0 }, { 177, 286, { 0, 0 }, 0 } }, 9 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static QUESTNPCTABLE sgtA2Q0Npcs[3] =
{
	{ { { 201, 253, { 0, 0 }, 0 } }, 1 },
	{ { { 244, 125, { 0, 0 }, 0 } }, 1 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

void __fastcall A2Q0Met(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q0Talk(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A2Q0Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);

void __fastcall A1Q6IntroInit(QUEST *pQuest  )
{
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->pCallback[0] = A2Q0IntroTalk;
	pQuest->pCallback[11] = A2Q0IntroMet;
	pQuest->ptNpcTables = sgtA2IntroNpcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->bState = 0;
	pQuest->bStatus = 0;
	pQuest->pQuestRecord = NULL;
	pQuest->eFilter = 41;
	pQuest->pStatusFilter = A3Q0IntroStatusFilter;
	pQuest->pActiveFilter = A2Q0IntroActive;
}

void __fastcall A2Q0IntroMet(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	int nNpcClass;
	WORD wMessage;

	nNpcClass = pqParam->field_14;
	wMessage = (WORD)pqParam->field_18;
	switch (nNpcClass)
	{
	case 199:
		if (wMessage == 204 || wMessage == 203)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	case 200:
		if (wMessage == 231 || wMessage == 230)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	case 210:
		if (wMessage == 242 || wMessage == 241)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	case 178:
		if (wMessage == 264 || wMessage == 263)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	case 177:
		if (wMessage == 286 || wMessage == 285)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	case 198:
		if (wMessage == 190)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	case 175:
		if (wMessage == 215)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	case 202:
		if (wMessage == 274)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	}
}

void __fastcall A2Q0IntroTalk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	UNIT *hPlayer;
	int nNpcClass;
	int nPlayerClass;
	int nFavoured;

	hPlayer = pqParam->hPlayer;
	nFavoured = 5;
	nNpcClass = pqParam->hEventUnit->dwClassId;
	nPlayerClass = hPlayer->dwClassId;
	switch (nNpcClass)
	{
	case 199:
		nFavoured = 2;
		break;
	case 200:
		nFavoured = 4;
		break;
	case 210:
		nFavoured = 0;
		break;
	case 178:
		nFavoured = 3;
		break;
	case 177:
		nFavoured = 1;
		break;
	case 175:
	case 198:
	case 202:
		break;
	default:
		return;
	}
	if (PlrIntroTestMet(hPlayer, pqParam->ptGame, nNpcClass))
		return;
	if (nPlayerClass == nFavoured)
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 1);
	else
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 0);
}

BYTE __fastcall A2Q0IntroActive(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer  , void *pPlayerRecord)
{
	return FALSE;
}

void __fastcall A2Q0Init(QUEST *pQuest  )
{
	QUESTPLAYERS *pPlayers;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->bState = 0;
	pQuest->bStatus = 0;
	pQuest->pQuestRecord = NULL;
	pQuest->pCallback[11] = A2Q0Met;
	pQuest->pCallback[0] = A2Q0Talk;
	pQuest->pCallback[10] = A2Q0PlayerLeft;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A2Q0PlayerStarted;
	pQuest->ptNpcTables = sgtA2Q0Npcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->eFilter = 8;
	pQuest->pStatusFilter = A2Q8StatusFilter;
	pQuest->pActiveFilter = A2Q0Active;
#line 172
	pPlayers = (QUESTPLAYERS *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(QUESTPLAYERS));
	pQuest->pQuestRecord = pPlayers;
	QuestsPlayersClear(pPlayers);
}

void __fastcall A2Q0Met(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	void *pRecord;
	WORD wNpcClass;
	WORD wMessage;

	wNpcClass = (WORD)pqParam->field_14;
	wMessage = (WORD)pqParam->field_18;
	if (wNpcClass == 201)
	{
		if (wMessage != 253)
			return;
		ptGame = pqParam->ptGame;
		pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
			->apQuestRecords[ptGame->bDifficultyLevel];
		D2CommonQuestRecordSetBit(pRecord, 8, 0);
		QuestsSetRecordBit(pqParam->ptGame, 8, 13);
		return;
	}
	if (wNpcClass == 244 && wMessage == 125)
		QuestsPlayersAdd((QUESTPLAYERS *)pQuest->pQuestRecord, pqParam->hPlayer->dwId);
}

void __fastcall A2Q0Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	int nNpcClass;
	void *pRecord;
	QUESTPLAYERS *pPlayers;
	UNIT *hPlayer;

	ptGame = pqParam->ptGame;
	nNpcClass = pqParam->hEventUnit->dwClassId;
	hPlayer = pqParam->hPlayer;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (nNpcClass == 201)
	{
		if (!D2CommonStepTestGridBit(pRecord, 8, 0))
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 0);
		return;
	}
	if (nNpcClass != 244)
		return;
	pPlayers = (QUESTPLAYERS *)pQuest->pQuestRecord;
	if (D2CommonStepTestGridBit(pRecord, 4, 14) != 1)
		return;
	if (QuestsPlayersFind(pPlayers, pqParam->hPlayer->dwId))
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, 244, 1);
	A1Q4RemovePlayer(pQuest->ptGame, pqParam->hPlayer);
}

BYTE __fastcall A2Q0Active(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer  , void *pPlayerRecord)
{
	if (nNpcClass == 201 && !D2CommonStepTestGridBit(pPlayerRecord, 8, 0))
		return TRUE;
	return FALSE;
}
