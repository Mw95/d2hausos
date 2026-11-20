#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a3q0.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../MONSTER/MonsterUnique.h"
#include "../PLAYER/PlrIntro.h"
#include "QUESTS.h"
#include "a2q8.h"
#include "a3q0.h"
#include "../UNIT/SUnit.h"

static QUESTNPCTABLE sgtA3IntroNpcs[3] =
{
	{ { { 297, 453, { 0, 0 }, 0 }, { 245, 458, { 0, 0 }, 0 }, { 264, 478, { 0, 0 }, 0 },
		{ 252, 490, { 0, 0 }, 0 }, { 254, 501, { 0, 0 }, 0 }, { 255, 514, { 0, 0 }, 0 } }, 6 },
	{ { { 297, 453, { 0, 0 }, 0 }, { 264, 479, { 0, 0 }, 0 }, { 252, 491, { 0, 0 }, 0 },
		{ 254, 502, { 0, 0 }, 0 }, { 255, 515, { 0, 0 }, 0 } }, 5 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

static QUESTNPCTABLE sgtA3Q0Npcs[3] =
{
	{ { { 253, 465, { 0, 0 }, 1 } }, 1 },
	{ { { 253, 466, { 0, 0 }, 1 } }, 1 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

void __fastcall A2Q8IntroMet(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q0NpcMessage(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A3Q0Talk(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A3Q0Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall A3Q0PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);

void __fastcall A2Q8IntroInit(QUEST *pQuest  )
{
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->pCallback[0] = A3Q0IntroTalk;
	pQuest->pCallback[11] = A2Q8IntroMet;
	pQuest->ptNpcTables = sgtA3IntroNpcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->bState = 0;
	pQuest->bStatus = 0;
	pQuest->pQuestRecord = NULL;
	pQuest->eFilter = 41;
	pQuest->pStatusFilter = A3Q0IntroStatusFilter;
	pQuest->pActiveFilter = A3Q0IntroActive;
}

void __fastcall A2Q8IntroMet(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	int nNpcClass;
	WORD wMessage;

	nNpcClass = pqParam->field_14;
	wMessage = (WORD)pqParam->field_18;
	switch (nNpcClass)
	{
	case 254:
		if (wMessage == 502 || wMessage == 501)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	case 264:
		if (wMessage == 479 || wMessage == 478)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	case 252:
		if (wMessage == 491 || wMessage == 490)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	case 255:
		if (wMessage == 515 || wMessage == 514)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	case 297:
		if (wMessage == 453)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	case 245:
		if (wMessage == 458)
			PlrIntroSetMet(pqParam->hPlayer, pqParam->ptGame, nNpcClass);
		break;
	}
}

void __fastcall A3Q0IntroTalk(QUEST *pQuest  , QUESTPARAM *pqParam
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
	case 254:
		nFavoured = 2;
		break;
	case 264:
		nFavoured = 4;
		break;
	case 252:
		nFavoured = 0;
		break;
	case 255:
		nFavoured = 3;
		break;
	case 245:
	case 297:
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

BYTE __fastcall A3Q0IntroStatusFilter(QUEST *pQuest  , UNIT *hPlayer,
	void *pGameRecord, void *pPlayerRecord, BYTE *pbStatus)
{
	*pbStatus = 0;
	return FALSE;
}

BYTE __fastcall A3Q0IntroActive(QUEST *pQuest  , int nNpcClass, UNIT *hPlayer,
	void *pPlayerRecord)
{
	if (nNpcClass == 245 && !PlrIntroTestMet(hPlayer, pQuest->ptGame, nNpcClass))
		return TRUE;
	return FALSE;
}

void __fastcall A3Q0Init(QUEST *pQuest  )
{
	A3Q0RECORD *pRecord;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->bState = 0;
	pQuest->bStatus = 0;
	pQuest->pCallback[11] = A3Q0NpcMessage;
	pQuest->pCallback[0] = A3Q0Talk;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A3Q0PlayerStarted;
	pQuest->ptNpcTables = sgtA3Q0Npcs;
	pQuest->bCallbacksEnabled = 1;
#line 145
	pRecord = (A3Q0RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A3Q0RECORD));
	pQuest->pQuestRecord = pRecord;
	for (i = 0; i < (int)(sizeof(A3Q0RECORD) / sizeof(DWORD)); i++)
		((DWORD *)pRecord)[i] = 0;
	pQuest->eFilter = 16;
	pQuest->pStatusFilter = A2Q8StatusFilter;
	pQuest->pActiveFilter = A3Q0Active;
}

void __fastcall A3Q0NpcMessage(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	WORD wMessage;
	void *pRecord;
	UNIT *hPlayer;
	GAME *ptGame;
	wMessage = (WORD)pqParam->field_18;
	if ((WORD)pqParam->field_14 != 253)
		return;
	hPlayer = pqParam->hPlayer;
	ptGame = pQuest->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (wMessage != 465 && wMessage != 466)
		return;
	D2CommonQuestRecordSetBit(pRecord, 16, 0);
	QuestsSetRecordBit(pQuest->ptGame, 16, 13);
}

void __fastcall A3Q0Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	UNIT *hPlayer;
	void *pRecord;
	if (pqParam->hEventUnit->dwClassId != 253)
		return;
	ptGame = pqParam->ptGame;
	hPlayer = pqParam->hPlayer;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 16, 0))
		return;
	if (pqParam->hPlayer->dwClassId == 1)
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, 253, 1);
	else
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, 253, 0);
}

BYTE __fastcall A3Q0Active(QUEST *pQuest  , int nNpcClass, UNIT *hPlayer,
	void *pPlayerRecord)
{
	if (nNpcClass == 253 && !D2CommonStepTestGridBit(pPlayerRecord, 16, 0))
		return TRUE;
	return FALSE;
}

void __fastcall A3Q0PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame = pqParam->ptGame;
	void *pRecord;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 16, 0) == 1)
		QuestsSetRecordBit(pQuest->ptGame, 16, 13);
}

void __fastcall A3Q0ObjInit(OBJINIT *ptInit  )
{
	QUEST *pQuest;
	A3Q0RECORD *pRecord;
	UNIT *hNpc;

	pQuest = QuestsFind(ptInit->ptGame, 14);
	if (!pQuest)
		return;
	if (QuestsGetRecordBit(ptInit->ptGame, 16, 13))
		return;
	pRecord = (A3Q0RECORD *)pQuest->pQuestRecord;
	if (pRecord->bHratliSpawned && SUnitFind(ptInit->ptGame, 1, pRecord->dwHratliId))
		return;
	hNpc = MonsterSpawn(ptInit->ptGame, ptInit->hRoom, ptInit->nX, ptInit->nY, 253, 1, -1, 0);
	if (!hNpc)
		return;
	pRecord->dwHratliId = hNpc->dwId;
	pRecord->bHratliSpawned = 1;
}
