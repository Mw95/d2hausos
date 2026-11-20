#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\QuestsFX.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "../ITEMS/Items.h"
#include "../MISSILES/Missiles.h"
#include "../MONSTER/MonsterUnique.h"
#include "QUESTS.h"
#include "a2q8.h"
#include "QuestsFX.h"
#include "../SKILLS/Skills.h"

#define UNIT_MONSTER		1
#define QUESTSFX_NPC		408
#define QUESTSFX_STAIRS		386

static DWORD sgadwQuestsFXGems4[7] =
{
	544632935, 544370791, 543322215, 544829543, 543649895, 544698471, 544893811,
};
static DWORD sgadwQuestsFXGems2[7] =
{
	544635495, 544369767, 543321191, 544828519, 543648871, 544697447, 543976307,
};
static DWORD sgadwQuestsFXGems1[7] =
{
	544633703, 544371559, 543322983, 544830311, 543650663, 544699239, 544566131,
};

static QUESTNPCTABLE sgtQuestsFXNpcs[3] =
{
	{ { { 408, 668, { 0, 0 }, 1 } }, 1 },
	{ { { 408, 669, { 0, 0 }, 1 } }, 1 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

void __fastcall QuestsFXDropGems(QUEST *pQuest  , UNIT *hObject)
{
	A4Q3RECORD *pQuestRecord;
	int nLevel;
	int nDropped;
	int i;

	pQuestRecord = (A4Q3RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bFinished)
		D2CommonUnitSetMode(hObject, 4);
	if (pQuestRecord->bGemsPending)
	{ int nLevel;
		nLevel = 50;
		nDropped = 0;
		for (i = 0; i < pQuestRecord->nGemsPerDrop; i++)
		{
			switch (pQuestRecord->nDropsLeft)
			{
			case 4:
				hObject->dwDropItemCode = sgadwQuestsFXGems4[D2CommonSeedRand(QuestsGetSeed(pQuest->ptGame),
					7)];
				break;
			case 2:
			case 3:
				hObject->dwDropItemCode = sgadwQuestsFXGems2[D2CommonSeedRand(QuestsGetSeed(pQuest->ptGame),
					7)];
				break;
			case 1:
				hObject->dwDropItemCode = sgadwQuestsFXGems1[D2CommonSeedRand(QuestsGetSeed(pQuest->ptGame),
					7)];
				break;
			default:
				return;
			}
			if (ITEMSDropFromUnit(pQuest->ptGame, hObject, 2, &nLevel, NULL, 44, FALSE))
				nDropped++;
		}
		if (!nDropped)
			return;
		pQuestRecord->nDropsLeft--;
		if (pQuestRecord->nDropsLeft <= 0)
		{
			pQuestRecord->bGemsPending = 0;
			return;
		}
		EventAdd(pQuest->ptGame, hObject, 7, pQuest->ptGame->nFrame + 20); }
}

void __fastcall QuestsFXInit(QUEST *pQuest  )
{
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->pCallback[0] = QuestsFXTalk;
	pQuest->ptNpcTables = sgtQuestsFXNpcs;
	pQuest->bCallbacksEnabled = 1;
	pQuest->bState = 0;
	pQuest->bStatus = 0;
	pQuest->pQuestRecord = NULL;
	pQuest->eFilter = 33;
	pQuest->pStatusFilter = A2Q8StatusFilter;
	pQuest->pActiveFilter = QuestsFXActive;
}

void __fastcall QuestsFXTalk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	QUEST *pQuestFound;
	void *pRecord;
	DWORD nClass;
	GAME *ptGame;
	UNIT *hPlayer;

	nClass = pqParam->hEventUnit->dwClassId;
	if (nClass != QUESTSFX_NPC)
		return;
	if (nClass != QUESTSFX_NPC)
		return;
	ptGame = pqParam->ptGame;
	hPlayer = pqParam->hPlayer;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecord, 27, 0) && !D2CommonStepTestGridBit(pRecord, 27, 13))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, QUESTSFX_NPC, 0);
		return;
	}
	pQuestFound = QuestsFind(pQuest->ptGame, 23);
	if (pQuestFound && ((A4Q2RECORD *)pQuestFound->pQuestRecord)->bDiabloKilled)
		return;
	QuestsNpcTable(pQuest, (void *)pqParam->field_14, QUESTSFX_NPC, 1);
}

BYTE __fastcall QuestsFXActive(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer, void *pPlayerRecord)
{
	void *pRecord;
	GAME *ptGame;
	ptGame = pQuest->ptGame;

	if (nNpcClass != QUESTSFX_NPC)
		return FALSE;
	GAME * ptGameCopy = ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGameCopy->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecord, 27, 0) && !D2CommonStepTestGridBit(pRecord, 27, 13))
		return TRUE;
	if (D2CommonStepTestGridBit(pRecord, 26, 13) == 1
		&& !D2CommonStepTestGridBit(pRecord, 26, 0))
		return TRUE;
	return FALSE;
}
#line 188

void __fastcall QuestsFXOnBloodravenDeath(GAME *ptGame  ,
	UNIT *hUnit)
{
	QuestsFXAffectNear(ptGame, hUnit, 35, 25, 125, TRUE);
}

void __fastcall QuestsFXAffectNear(GAME *ptGame  , UNIT *hUnit,
	int nRange, int nDelayMin  , int nDelayMax
, BOOL fUndead)
{
	UNITFINDDATA tFind;
	UNITFINDARGS tArgs;
	MONSTATSTXT *ptMonStats;
	DWORD hRoom;
	BOOL fAffected;
	int nX;
	int nY;
	int nDelay;
	int i;

	hRoom = D2CommonUnitGetPathRoom(hUnit);
	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
#line 32
	D2ASSERT(nDelayMax);
#line 33
	D2ASSERT(nDelayMax > nDelayMin);
	for (i = 0; i < (int)(sizeof(UNITFINDARGS) / sizeof(DWORD)); i++)
		((DWORD *)&tArgs)[i] = 0;
	tArgs.hExcluded = hUnit;
	tArgs.nY = nY;
	tArgs.nDistance = nRange;
	tArgs.dwFlags = 1411;
	tArgs.nX = nX;
	D2CommonUnitFindsInit(ptGame->pMemPool, &tFind, hRoom, nX, nY, nRange, 0, &tArgs);
	D2CommonUnitFindsFindUnits(&tFind);
	for (i = 0; i < tFind.nNumUnits; i++)
	{
		if (tFind.phUnitsArray[i]->eType != UNIT_MONSTER)
			continue;
		ptMonStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(tFind.phUnitsArray[i]->dwClassId);
		fAffected = FALSE;
		if (D2CommonStatsUnitGetAlignment(tFind.phUnitsArray[i]) == 2)
			fAffected = TRUE;
		if (fUndead && !ptMonStats->bLUndead && !ptMonStats->bHUndead)
			continue;
		if (!fAffected)
			continue;
		if (nDelayMax - nDelayMin)
			nDelay = (int)D2CommonSeedRand(&hUnit->tSeed, nDelayMax - nDelayMin);
		else
			nDelay = 0;
		EventAdd(ptGame, tFind.phUnitsArray[i], 7, ptGame->nFrame + nDelay + nDelayMin);
		MonsterUniqueAddMod(ptGame, tFind.phUnitsArray[i], 21, FALSE);
	}
	D2CommonUnitFindsFreeArray(&tFind);
}

void __fastcall QuestsFXOnMephistoDeath(GAME *ptGame  ,
	UNIT *hUnit)
{
	MISSILECREATE tCreate;
	int i;

	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.hSource = hUnit;
	tCreate.hSpawnLocation = hUnit;
	tCreate.dwFlags = 32768;
	tCreate.eMissile = 299;
	tCreate.nLevel = 1;
	tCreate.nRange = 100;
	MissilesCreate(ptGame, &tCreate);
}

void __fastcall QuestsFXOnAndarielDeath(GAME *ptGame  ,
	UNIT *hUnit)
{
	QuestsFXAffectNear(ptGame, hUnit, 35, 1, 51, FALSE);
}

void __fastcall QuestsFXOnRadamentDeath(GAME *ptGame  ,
	UNIT *hUnit)
{
	int nDelayMax;
	int nX;
	int nY;

	nDelayMax = ((MISSILESTXT *)D2CommonMissileTblsGetMissile(MISSILE_RADAMENTDEATH))->Range - 100;
	if (nDelayMax < 100)
		nDelayMax = 100;
	QuestsFXAffectNear(ptGame, hUnit, 35, 40, nDelayMax, FALSE);
	nY = (int)D2CommonUnitGetY(hUnit);

	SkillsCreateMissileAt(ptGame, hUnit, 1, MISSILE_RADAMENTDEATH, ((int)D2CommonUnitGetX(hUnit)), nY);
}

void __fastcall QuestsFXOpenStairs(GAME *ptGame  , UNIT *hUnit)
{
	MISSILECREATE tCreate;
	MISSOBJECTSTXT *ptObjectTxt;
	UNIT *hStairs;
	UNIT *hMissile;
	int i;
	DWORD hRoom;

	hStairs = (UNIT *)D2CommonUnitFindsTestUnitNearRoom(D2CommonUnitGetPathRoom(hUnit), 2, QUESTSFX_STAIRS);
#line 136
	D2ASSERT(hStairs);
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = 1056;
	tCreate.eMissile = 368;
	tCreate.hSource = hUnit;
	tCreate.hSpawnLocation = hUnit;
	tCreate.nDX = (int)D2CommonUnitGetX(hStairs);
	tCreate.nDY = (int)D2CommonUnitGetY(hStairs);
	tCreate.nLevel = 1;
	hMissile = MissilesCreate(ptGame, &tCreate);
	if (hMissile)
	{
		D2CommonMissileSetScratchFirst(hMissile, hStairs->dwId);
		hRoom = D2CommonUnitGetPathRoom(hStairs);
		D2CommonDungeonRoomSetHold(hRoom, 0);
		return;
	}
	if (hStairs->dwMode)
		return;
	D2CommonUnitSetMode(hStairs, 1);
	ptObjectTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(QUESTSFX_STAIRS);
	EventAdd(ptGame, hStairs, 1, ptGame->nFrame + (int)((DWORD)ptObjectTxt->FrameCnt[1] >> 8));
}

void __fastcall QuestsFXOnIzualDeath(GAME *ptGame  , UNIT *hUnit)
{
	QuestsFXAffectNear(ptGame, hUnit, 105, 1, 2, FALSE);
}
