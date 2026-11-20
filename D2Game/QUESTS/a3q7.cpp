#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a3q7.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../MONSTER/MonsterUnique.h"
#include "QUESTS.h"
#include "a2q0.h"
#include "a2q8.h"
#include "a3q6.h"
#include "a3q7.h"
#include "a4q1.h"
#include "../UNIT/SUnit.h"

#define A3Q7_MONSTER		368
#define A3Q7_OBJECT			131

static int sgnA3Q7Spots[8][2] =
{
	{ -3, -3 }, { -3, 0 }, { -3, 3 }, { 0, -3 }, { 0, 3 }, { 3, -3 }, { 3, 0 }, { -3, 3 },
};

void __fastcall A3Q7PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A3Q7Timer(GAME *ptGame, QUEST *pQuest);
BOOL __fastcall A3Q7FlagInAct(GAME *ptGame, UNIT *hUnit, void *pArg);

void __fastcall A3Q7SetQuestStatus(GAME *ptGame  , BYTE bRestore
)
{
	QUEST *pQuest = NULL;
	A3Q6RECORD *pQuestRecord;
	DWORD dwStatus;
	BYTE fIterate;

	pQuest = QuestsFind(ptGame, 20);
	if (!pQuest)
		return;
	if (!pQuest->bActive)
		return;
	if (pQuest->bState >= 6)
		return;
	pQuestRecord = (A3Q6RECORD *)pQuest->pQuestRecord;
	if (bRestore)
		dwStatus = pQuestRecord->dwSavedStatus;
	else
	{
		pQuestRecord->dwSavedStatus = pQuest->bStatus;
		if (pQuest->ptGame->bGameType != 3)
			dwStatus = 7;
		else
			dwStatus = 6;
	}
	fIterate = FALSE;
	if (pQuest->bState > 0)
		fIterate = TRUE;
	QuestsSetStatus(pQuest, dwStatus, NULL, A3Q6SendStatus, fIterate);
}

void __fastcall A3Q7ObjectsDone(GAME *ptGame  , UNIT *hObject
)
{
	QUEST *pQuest = NULL;
	A3Q6RECORD *pQuestRecord;
	UNIT *hFound;

	pQuest = QuestsFind(ptGame, 20);
	if (!pQuest)
		return;
	pQuestRecord = (A3Q6RECORD *)pQuest->pQuestRecord;
	pQuestRecord->dwHellgateMode = 2;
	pQuestRecord->dwBridgeMode = 2;
	if (pQuestRecord->bHellgatePresent)
	{
		hFound = SUnitFind(ptGame, 2, pQuestRecord->dwHellgateId);
		if (hFound)
			D2CommonUnitSetMode(hFound, 1);
	}
	if (!pQuestRecord->bBridgePresent)
		return;
	hFound = SUnitFind(ptGame, 2, pQuestRecord->dwBridgeId);
	if (hFound)
		D2CommonUnitSetMode(hFound, 2);
}

BOOL __fastcall A3Q7GetSpot(GAME *ptGame  , UNIT *hUnit
, GAMECOORD *ptCoord)
{
	QUEST *pQuest = NULL;
	A3Q7RECORD *pQuestRecord;
	BOOL bBlocked;

	pQuest = QuestsFind(ptGame, 28);
	if (pQuest)
	{
		pQuestRecord = (A3Q7RECORD *)pQuest->pQuestRecord;
		if (!pQuestRecord->bSpotSet)
			return FALSE;
		*ptCoord = *(GAMECOORD *)&pQuestRecord->dwSpotX;
		bBlocked = FALSE;
		if (!pQuestRecord->bSpotMoved)
		{
			ptCoord->dwY -= 20;
			pQuestRecord->bSpotMoved = 1;
			bBlocked = D2CommonUnitTestLineBlockedToPoint(hUnit, ptCoord->dwX, ptCoord->dwY, 7169);
			if (!bBlocked)
				*(GAMECOORD *)&pQuestRecord->dwSpotX = *ptCoord;
			else
			{
				ptCoord->dwY += 9;
				bBlocked = D2CommonUnitTestLineBlockedToPoint(hUnit, ptCoord->dwX, ptCoord->dwY, 7169);
				if (!bBlocked)
					*(GAMECOORD *)&pQuestRecord->dwSpotX = *ptCoord;
				else
				{
					ptCoord->dwX += 2;
					bBlocked = D2CommonUnitTestLineBlockedToPoint(hUnit, ptCoord->dwX, ptCoord->dwY, 7169);
					if (!bBlocked)
					{
						*(GAMECOORD *)&pQuestRecord->dwSpotX = *ptCoord;
						return TRUE;
					}
					ptCoord->dwX -= 4;
					bBlocked = D2CommonUnitTestLineBlockedToPoint(hUnit, ptCoord->dwX, ptCoord->dwY, 7169);
					if (!bBlocked)
					{
						*(GAMECOORD *)&pQuestRecord->dwSpotX = *ptCoord;
						return TRUE;
					}
					*ptCoord = *(GAMECOORD *)&pQuestRecord->dwSpotX;
					ptCoord->dwY -= 8;
					bBlocked = D2CommonUnitTestLineBlockedToPoint(hUnit, ptCoord->dwX, ptCoord->dwY, 7169);
					if (!bBlocked)
						*(GAMECOORD *)&pQuestRecord->dwSpotX = *ptCoord;
				}
			}
		}
		if (!bBlocked)
			return TRUE;
		ptCoord->dwY += 5;
		*(GAMECOORD *)&pQuestRecord->dwSpotX = *ptCoord;
		return TRUE;
	}
	return FALSE;
}

void __fastcall A3Q7InitDarkWandererStart(OBJINIT *ptInit  )
{
	QUEST *pQuest = NULL;
	A3Q7RECORD *pQuestRecord;
	GAMEROOM *hRoom;

	pQuest = QuestsFind(ptInit->ptGame, 28);
	if (!pQuest)
		return;
	pQuestRecord = (A3Q7RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bDarkWandererPending)
	{
		pQuestRecord->dwSpotX = ptInit->nX + 7;
		pQuestRecord->dwSpotY = ptInit->nY;
		hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(ptInit->hRoom, pQuestRecord->dwSpotX,
			pQuestRecord->dwSpotY);
		if (MonsterSpawn(ptInit->ptGame, hRoom, (int)pQuestRecord->dwSpotX, (int)pQuestRecord->dwSpotY,
			A3Q7_MONSTER, 1, -1, 0))
			pQuestRecord->bDarkWandererPending = 0;
	}
	pQuestRecord->bSpotSet = 1;
}

void __fastcall A3Q7Init(QUEST *pQuest  )
{
	A3Q7RECORD *pQuestRecord;
	int i;

	memset(pQuest->pCallback, 0, (NUM_QUESTCALLBACKS) * sizeof(pQuest->pCallback[0]));
	pQuest->ptNpcTables = NULL;
	pQuest->bState = 0;
	pQuest->bStatus = 0;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A3Q7PlayerStarted;
	pQuest->bCallbacksEnabled = 1;
#line 185
	pQuestRecord = (A3Q7RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A3Q7RECORD));
	pQuest->pQuestRecord = pQuestRecord;
	for (i = 0; i < (int)(sizeof(A3Q7RECORD) / sizeof(DWORD)); i++)
		((DWORD *)pQuestRecord)[i] = 0;
	pQuest->eFilter = 16;
	pQuest->pStatusFilter = A2Q8StatusFilter;
	pQuest->pActiveFilter = A2Q0IntroActive;
	((A3Q7RECORD *)pQuest->pQuestRecord)->bDarkWandererPending = 1;
}

void __fastcall A3Q7PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	void *pRecordX;
	GAME *ptGame;
	DWORD dwDone;
	UNIT *hPlayer;

	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecordX = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	dwDone = D2CommonStepTestGridBit(pRecordX, 32, 0);
	if (dwDone != 1)
		return;
	QuestsSetRecordBit(pQuest->ptGame, 32, 13);
	((A3Q7RECORD *)pQuest->pQuestRecord)->bDarkWandererPending = 0;
}

void __fastcall A3Q7StartTimer(GAME *ptGame  , UNIT *hUnit
)
{
	QUEST *pQuest = NULL;
	A3Q7RECORD *pQuestRecord;

	pQuest = QuestsFind(ptGame, 28);
	if (!pQuest)
		return;
	pQuestRecord = (A3Q7RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bTimerDone)
		return;
	if (pQuestRecord->bTimerPending)
		return;
	pQuestRecord->bTimerPending = 1;
	pQuestRecord->dwMonsterId = hUnit->dwId;
	QuestsAddTimer(pQuest, A3Q7Timer, 2);
}

BYTE __fastcall A3Q7Timer(GAME *ptGame  , QUEST *pQuest
)
{
	D2SEED *ptSeed;
	A3Q7RECORD *pQuestRecord;
	UNIT *hUnit;
	GAMEROOM *hRoom;
	GAMEROOM *hSpotRoom;
	GAMECOORD tCoord;
	GAMECOORD tSpot;
	DWORD dwRow;

	pQuestRecord = (A3Q7RECORD *)pQuest->pQuestRecord;
	if (!pQuestRecord->bTimerDone)
	{
		SUnitIterate(ptGame, 0, NULL, A3Q7FlagInAct);
		pQuestRecord->bTimerDone = 1;
		pQuestRecord->bTimerPending = 0;
		hUnit = SUnitFind(ptGame, 1, pQuestRecord->dwMonsterId);
		if (hUnit)
		{
			ptSeed = QuestsGetSeed(ptGame);
			dwRow = D2CommonSeedRand(ptSeed, 2);
			hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
			D2CommonUnitGetPosition(hUnit, &tCoord);
			for (; dwRow < 8; dwRow++)
			{
				tSpot = tCoord;
				tSpot.dwX += sgnA3Q7Spots[dwRow][0];
				tSpot.dwY += sgnA3Q7Spots[dwRow][1];
				QuestsFindSpot(hRoom, &tSpot, 3, 7953, &hSpotRoom);
				if (hSpotRoom)
					SUnitCreate(2, A3Q7_OBJECT, (int)tSpot.dwX, (int)tSpot.dwY, ptGame, hSpotRoom, 1, 0, 0);
			}
		}
	}
	return TRUE;
}

BOOL __fastcall A3Q7FlagInAct(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	DWORD dwLevelId;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 32, 0))
		return FALSE;
	if (!D2CommonUnitGetPathRoom(hUnit))
		return FALSE;
	dwLevelId = D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hUnit));
	if (!dwLevelId)
		return FALSE;
	if (D2CommonGetActNum(dwLevelId) != 2)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 32, 0);
	return FALSE;
}
