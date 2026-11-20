#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\PLAYER\\PlayerPets.cpp"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "../GAME/SCmd.h"
#include "PlayerPets.h"
#include "../UNIT/PlayerList.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"

#define UNIT_PLAYER				0
#define UNIT_MONSTER			1
#define EPETTYPE_GOLEM			3
#define EPETTYPE_HIRELING		7
#define PETFLAG_NODEATH			256
#define PETFLAG0_KILLED			67108864
#define PETFLAG0_31				0x80000000
#define MONSTER_CLAYGOLEM		289
#define MONSTER_BLOODGOLEM		290
#define MONSTER_IRONGOLEM		291
#define MONSTER_FIREGOLEM		292
#define SKILL_VALKYRIE			32
#define SKILL_CLAY_GOLEM			75
#define SKILL_BLOODGOLEM		85
#define SKILL_IRONGOLEM			90
#define SKILL_FIREGOLEM			94
#define PET_WARP_MASK			7937

struct PETMSG
{
 	DWORD		dwPetId;
 	DWORD		dwOwnerId;
 	WORD		wClass;
 	BYTE		unobserved_0a[2];
 	int			eType;
 	PETDATA		tData;
};
typedef struct PETMSG PETMSG;

struct PETTESTARG
{
 	BOOL		bFound;
 	UNIT		*hPlayer;
};
typedef struct PETTESTARG PETTESTARG;

struct PETSKILL
{
	int		nSkill;
	int		eType;
	int		nBonus;
};
typedef struct PETSKILL PETSKILL;

static PETSKILL sgatPetSkills[3] =
{
	{ 70, 4, 0 }, { 80, 5, 0 }, { 95, 6, 0 },
};
static int sgnPetSkills = 3;

void __fastcall PlayerPetsTestAnyPlayerEach(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall PlayerPetsSendRemoveEach(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall PlayerPetsClearSet(GAME *ptGame, UNIT *hPlayer, PETSET *ptSet  ,
	BOOL bRemove);
void __fastcall PlayerPetsSetRemove(GAME *ptGame, PETSET *ptSet  , DWORD dwUnitId,
	BOOL bKill);
BOOL __fastcall PlayerPetsSetAdd(GAME *ptGame, UNIT *hPlayer, PETSET *ptSet  ,
	UNIT *hPet  , PETDATA *ptData);
void __fastcall PlayerPetsSendAddEach(GAME *ptGame, UNIT *hUnit, void *pArg);

static inline void PlayerPetsKillUnit(GAME *ptGame, DWORD dwUnitId)
{
	UNIT *hPet;
	PETMSG tMsg;

	hPet = SUnitFind(ptGame, UNIT_MONSTER, dwUnitId);
	if (!hPet)
		return;
	if (hPet->dwFlags & PETFLAG_NODEATH)
	{
#line 84
		D2WARN(!hPet);
		SUnitFree(ptGame, hPet);
	}
	else
	{
		hPet->dwFlags0 |= PETFLAG0_KILLED;
		SUnitDmgKill(ptGame, hPet, NULL, 0);
	}
	memset(&tMsg, 0, sizeof(tMsg));
	tMsg.dwPetId = dwUnitId;
	SUnitForEachPlayer(ptGame, PlayerPetsSendRemoveEach, &tMsg);
}

static inline PETSET *PlayerPetsGetSet(PLAYERPETS *ptPets, int eType  )
{
#line 116
	D2ASSERT(eType >= EPETTYPE_NONE && eType < NUM_PET_TYPES);
	return &ptPets->atSets[eType];
}

BOOL __fastcall PlayerPetsTestAnyPlayer(GAME *ptGame, UNIT *hPlayer)
{
	PETTESTARG tArg;

	if (!ptGame || !hPlayer || hPlayer->eType != UNIT_PLAYER)
		return FALSE;
	memset(&tArg, 0, sizeof(tArg));
	tArg.hPlayer = hPlayer;
	tArg.bFound = FALSE;
	SUnitForEachPlayer(ptGame, PlayerPetsTestAnyPlayerEach, &tArg);
	return tArg.bFound;
}

void __fastcall PlayerPetsTestAnyPlayerEach(GAME *ptGame, UNIT *hUnit, void *pArg)
{
	PETTESTARG *ptArg = (PETTESTARG *)pArg;

	if (PlayerListTestFlag(ptArg->hPlayer, hUnit, 8))
		ptArg->bFound = TRUE;
}

void __fastcall PlayerPetsNew(GAME *ptGame, UNIT *hPlayer)
{
	PLAYERPETS *ptPets;
	PLRINFO *ptInfo;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
#line 127
	D2ASSERT(ptInfo);
	if (ptInfo->ptPets)
		PlayerPetsFree(ptGame, hPlayer);
#line 135
	ptPets = (PLAYERPETS *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(PLAYERPETS));
	memset(ptPets, 0, sizeof(PLAYERPETS));
	ptInfo->ptPets = ptPets;
	PlayerPetsSetMax(ptGame, hPlayer, 1, 1);
	PlayerPetsSetMax(ptGame, hPlayer, EPETTYPE_HIRELING, 1);
	switch (hPlayer->dwClassId)
	{
	case 0:
		PlayerPetsSetMax(ptGame, hPlayer, 2, 1);
		PlayerPetsSetMax(ptGame, hPlayer, 8, 1);
		break;
	case 2:
		PlayerPetsSetMax(ptGame, hPlayer, EPETTYPE_GOLEM, 1);
		break;
	}
}

void __fastcall PlayerPetsClearSet(GAME *ptGame, UNIT *hPlayer, PETSET *ptSet  ,
	BOOL bRemove)
{
	PETNODE *ptNode;
	PETNODE *ptNext;
	UNIT *hPet;
	PETMSG tMsg;

	for (ptNode = ptSet->ptHead; ptNode; ptNode = ptNext)
	{
		ptNext = ptNode->ptNext;
		if (!bRemove)
			PlayerPetsKillUnit(ptGame, ptNode->dwUnitId);
		else
		{
			hPet = SUnitFind(ptGame, UNIT_MONSTER, ptNode->dwUnitId);
			if (hPet)
			{
				memset(&tMsg, 0, sizeof(tMsg));
				tMsg.dwPetId = ptNode->dwUnitId;
				SUnitForEachPlayer(ptGame, PlayerPetsSendRemoveEach, &tMsg);
				D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hPet), 1, hPet->dwId);
				SUnitFree(ptGame, hPet);
			}
		}
#line 188
		D2FREEPOOL(ptGame->pMemPool, ptNode);
	}
	memset(ptSet, 0, sizeof(*ptSet));
}

void __fastcall PlayerPetsSendRemoveEach(GAME *ptGame, UNIT *hUnit, void *pArg)
{
	PETMSG *ptMsg = (PETMSG *)pArg;

	SCmdSendPet(SUnitGetClient(hUnit), 0, ptMsg->eType, ptMsg->dwPetId, ptMsg->dwOwnerId,
		ptMsg->wClass);
}

void __fastcall PlayerPetsClearAll(GAME *ptGame, UNIT *hPlayer)
{
	PLRINFO *ptInfo;
	PLAYERPETS *ptPets;
	int eType;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
#line 206
	D2ASSERT(ptInfo);
	ptPets = ptInfo->ptPets;
	if (!ptPets)
		return;
	for (eType = 1; eType < NUM_PET_TYPES; eType++)
		PlayerPetsClearSet(ptGame, hPlayer, PlayerPetsGetSet(ptPets, eType), eType == EPETTYPE_HIRELING);
	PlayerPetsUpdateMax(hPlayer);
}

void __fastcall PlayerPetsFree(GAME *ptGame, UNIT *hPlayer)
{
	PLRINFO *ptInfo;
	PLAYERPETS *ptPets;
	int eType;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
#line 231
	D2ASSERT(ptInfo);
	ptPets = ptInfo->ptPets;
	if (!ptPets)
		return;
	for (eType = 1; eType < NUM_PET_TYPES; eType++)
		PlayerPetsClearSet(ptGame, hPlayer, PlayerPetsGetSet(ptPets, eType), eType == EPETTYPE_HIRELING);
#line 243
	D2FREEPOOL(ptGame->pMemPool, ptPets);
	ptInfo->ptPets = NULL;
}

extern DWORD PlayerPetsGetFirstId(UNIT *hPlayer, int eType  );
__inline DWORD PlayerPetsGetFirstId(UNIT *hPlayer, int eType  )
{
	PLRINFO *ptInfo;
	PETSET *ptSet;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
#line 258
	D2ASSERT(ptInfo);
	if (!ptInfo->ptPets)
		return (DWORD)-1;
	ptSet = PlayerPetsGetSet(ptInfo->ptPets, eType);
	if (!ptSet)
		return (DWORD)-1;
	if (!ptSet->ptHead)
		return (DWORD)-1;
	return ptSet->ptHead->dwUnitId;
}

static inline void PlayerPetsCheckSkill(GAME *ptGame, UNIT *hPlayer,
	PETSET *ptSet  , int eType  )
{
	UNIT *hPet;
	int nSkill;

	switch (eType)
	{
	case 2:
	case 8:
		if (!ptSet->ptHead)
			return;
		if (!SUnitFind(ptGame, UNIT_MONSTER, ptSet->ptHead->dwUnitId))
			return;
		if (!D2CommonSkillsCalcNodeLevel(hPlayer, D2CommonSkillsFindByKey(hPlayer, SKILL_VALKYRIE), 1))
			PlayerPetsRemove(ptGame, hPlayer, ptSet->ptHead->dwUnitId, TRUE);
		return;
	case EPETTYPE_GOLEM:
		if (!ptSet->ptHead)
			return;
		hPet = SUnitFind(ptGame, UNIT_MONSTER, ptSet->ptHead->dwUnitId);
		if (!hPet)
			return;
		switch (hPet->dwClassId)
		{
		case MONSTER_CLAYGOLEM:
			nSkill = SKILL_CLAY_GOLEM;
			break;
		case MONSTER_BLOODGOLEM:
			nSkill = SKILL_BLOODGOLEM;
			break;
		case MONSTER_IRONGOLEM:
			nSkill = SKILL_IRONGOLEM;
			break;
		case MONSTER_FIREGOLEM:
			nSkill = SKILL_FIREGOLEM;
			break;
		default:
#line 320
			D2ASSERTMSG(FALSE, "Non-Golem unit in Golem pet list!\nTell Ted Bisson!");
		}
		break;
	default:
		return;
	}
	if (!D2CommonSkillsCalcNodeLevel(hPlayer, D2CommonSkillsFindByKey(hPlayer, nSkill), 1))
		PlayerPetsRemove(ptGame, hPlayer, ptSet->ptHead->dwUnitId, TRUE);
}

void __fastcall PlayerPetsSetMax(GAME *ptGame, UNIT *hPlayer, int eType  ,
	int nMax  )
{
	PLRINFO *ptInfo;
	PLAYERPETS *ptPets;
	PETSET *ptSet;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
#line 338
	D2ASSERT(ptInfo);
	if (eType == 1 && nMax != 1)
		return;
	ptPets = ptInfo->ptPets;
	if (!ptPets || ptPets->fLocked)
		return;
	ptSet = PlayerPetsGetSet(ptPets, eType);
	if (!ptSet)
		return;
	ptSet->nMax = nMax;
	while (ptSet->nCurrent > nMax && ptSet->nCurrent > 0)
		PlayerPetsRemove(ptGame, hPlayer, (WORD)PlayerPetsGetFirstId(hPlayer, eType), TRUE);
	PlayerPetsCheckSkill(ptGame, hPlayer, ptSet, eType);
}

int __fastcall PlayerPetsGetMax(UNIT *hPlayer, int eType  )
{
	PLRINFO *ptInfo;
	PETSET *ptSet;
	int nMax = 0;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
	if (ptInfo && ptInfo->ptPets)
	{
		ptSet = PlayerPetsGetSet(ptInfo->ptPets, eType);
		if (ptSet)
			nMax = ptSet->nMax;
	}
	return nMax;
}

void __fastcall PlayerPetsRemove(GAME *ptGame, UNIT *hPlayer, DWORD dwUnitId, BOOL bKill)
{
	PLRINFO *ptInfo;
	PLAYERPETS *ptPets;
	PETSET *ptSet;
	int eType;
	PETMSG tMsg;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
#line 448
	D2ASSERT(ptInfo);
	ptPets = ptInfo->ptPets;
	if (!ptPets)
		return;
	eType = PlayerPetsGetType(hPlayer, dwUnitId);
	if (eType == EPETTYPE_NONE && bKill)
		PlayerPetsKillUnit(ptGame, dwUnitId);
	ptSet = PlayerPetsGetSet(ptPets, eType);
	if (!ptSet)
		return;
	PlayerPetsSetRemove(ptGame, ptSet, dwUnitId, bKill);
	memset(&tMsg, 0, sizeof(tMsg));
	tMsg.dwPetId = dwUnitId;
	SUnitForEachPlayer(ptGame, PlayerPetsSendRemoveEach, &tMsg);
}

void __fastcall PlayerPetsSetRemove(GAME *ptGame, PETSET *ptSet  , DWORD dwUnitId,
	BOOL bKill)
{
	PETNODE *ptNode;
	PETNODE *ptPrev = NULL;
	UNIT *hPet;

	for (ptNode = ptSet->ptHead; ptNode; ptNode = ptNode->ptNext)
	{
		if (ptNode->dwUnitId == dwUnitId)
		{
			if (ptPrev)
				ptPrev->ptNext = ptNode->ptNext;
			else
				ptSet->ptHead = ptNode->ptNext;
			hPet = SUnitFind(ptGame, UNIT_MONSTER, ptNode->dwUnitId);
			if (hPet)
			{
				if (bKill)
					PlayerPetsKillUnit(ptGame, ptNode->dwUnitId);
				else
					hPet->dwFlags0 &= ~PETFLAG0_31;
			}
#line 424
			D2FREEPOOL(ptGame->pMemPool, ptNode);
			ptSet->nCurrent--;
#line 426
			D2ASSERTMSG(ptSet->nCurrent >= 0, "Too many pets removed from list!\nTell Ted Bisson");
			return;
		}
		ptPrev = ptNode;
	}
}

void __fastcall PlayerPetsAdd(GAME *ptGame, UNIT *hPlayer, UNIT *hPet  ,
	int eType  )
{
	PLRINFO *ptInfo;
	PETSET *ptSet;
	PETDATA tData;
	PETMSG tMsg;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
#line 572
	D2ASSERT(ptInfo);
	if (!ptInfo->ptPets)
		return;
	ptSet = PlayerPetsGetSet(ptInfo->ptPets, eType);
	if (!ptSet)
		return;
	tData.dwSeed = 0;
	tData.dwHireId = 0;
	if (!PlayerPetsSetAdd(ptGame, hPlayer, ptSet, hPet, &tData))
		return;
	memset(&tMsg, 0, sizeof(tMsg));
	tMsg.dwPetId = hPet->dwId;
	tMsg.eType = eType;
	tMsg.wClass = (WORD)hPet->dwClassId;
	tMsg.dwOwnerId = hPlayer->dwId;
	SUnitForEachPlayer(ptGame, PlayerPetsSendAddEach, &tMsg);
}

BOOL __fastcall PlayerPetsSetAdd(GAME *ptGame, UNIT *hPlayer, PETSET *ptSet  ,
	UNIT *hPet  , PETDATA *ptData)
{
	PETNODE *ptLast;
	PETNODE *ptNode;

	if (!ptSet->nMax)
	{
		PlayerPetsUpdateMax(hPlayer);
		if (!ptSet->nMax)
		{
			PlayerPetsKillUnit(ptGame, hPet->dwId);
			return FALSE;
		}
	}
	if (ptSet->nCurrent == ptSet->nMax)
	{
		PlayerPetsSetRemove(ptGame, ptSet, ptSet->ptHead->dwUnitId, TRUE);
#line 504
		D2ASSERT(ptSet->nCurrent != ptSet->nMax);
	}
	ptLast = ptSet->ptHead;
	if (ptLast)
		while (ptLast->ptNext)
			ptLast = ptLast->ptNext;
#line 516
	ptNode = (PETNODE *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(PETNODE));
	memset(ptNode, 0, sizeof(PETNODE));
	ptNode->dwUnitId = hPet->dwId;
	ptNode->tData = *ptData;
	if (ptLast)
		ptLast->ptNext = ptNode;
	else
		ptSet->ptHead = ptNode;
	ptSet->nCurrent++;
#line 529
	D2ASSERT(ptSet->nCurrent <= ptSet->nMax);
	return TRUE;
}

void __fastcall PlayerPetsSendAddEach(GAME *ptGame, UNIT *hUnit, void *pArg)
{
	PETMSG *ptMsg = (PETMSG *)pArg;

	if (!ptMsg->tData.dwSeed && !ptMsg->tData.dwHireId)
		SCmdSendPet(SUnitGetClient(hUnit), 1, ptMsg->eType, ptMsg->dwPetId, ptMsg->dwOwnerId,
			ptMsg->wClass);
	else
		SCmdSendHireling(SUnitGetClient(hUnit), 1, ptMsg->eType, ptMsg->dwPetId, ptMsg->dwOwnerId,
			ptMsg->wClass, ptMsg->tData.dwSeed, ptMsg->tData.dwHireId);
}

void __fastcall PlayerPetsAddData(GAME *ptGame, UNIT *hPlayer, UNIT *hPet  ,
	int eType  , PETDATA *ptData)
{
	PLRINFO *ptInfo;
	PETSET *ptSet;
	PETDATA tData;
	PETMSG tMsg;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
#line 608
	D2ASSERT(ptInfo);
	if (!ptInfo->ptPets)
		return;
	ptSet = PlayerPetsGetSet(ptInfo->ptPets, eType);
	if (!ptSet)
		return;
	tData = *ptData;
	if (!PlayerPetsSetAdd(ptGame, hPlayer, ptSet, hPet, &tData))
		return;
	memset(&tMsg, 0, sizeof(tMsg));
	tMsg.dwPetId = hPet->dwId;
	tMsg.dwOwnerId = hPlayer->dwId;
	tMsg.wClass = (WORD)hPet->dwClassId;
	tMsg.tData = *ptData;
	tMsg.eType = eType;
	SUnitForEachPlayer(ptGame, PlayerPetsSendAddEach, &tMsg);
}

int __fastcall PlayerPetsGetType(UNIT *hPlayer, DWORD dwUnitId)
{
	PLRINFO *ptInfo;
	PETSET *ptSet;
	PETNODE *ptNode;
	int eType;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
#line 646
	D2ASSERT(ptInfo);
	if (!ptInfo->ptPets)
		return EPETTYPE_NONE;
	for (eType = 1; eType < NUM_PET_TYPES; eType++)
	{
		ptSet = PlayerPetsGetSet(ptInfo->ptPets, eType);
		if (!ptSet)
			continue;
		for (ptNode = ptSet->ptHead; ptNode; ptNode = ptNode->ptNext)
			if (ptNode->dwUnitId == dwUnitId)
				return eType;
	}
	return EPETTYPE_NONE;
}

PETDATA * __fastcall PlayerPetsGetData(UNIT *hPlayer, DWORD dwUnitId)
{
	PLRINFO *ptInfo;
	PETSET *ptSet;
	PETNODE *ptNode;
	int eType;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
#line 682
	D2ASSERT(ptInfo);
	if (!ptInfo->ptPets)
		return NULL;
	for (eType = 1; eType < NUM_PET_TYPES; eType++)
	{
		ptSet = PlayerPetsGetSet(ptInfo->ptPets, eType);
		if (!ptSet)
			continue;
		for (ptNode = ptSet->ptHead; ptNode; ptNode = ptNode->ptNext)
			if (ptNode->dwUnitId == dwUnitId)
				return &ptNode->tData;
	}
	return NULL;
}

void __fastcall PlayerPetsUpdateMax(UNIT *hPlayer  )
{
	GAME *ptGame;
	PLRINFO *ptInfo;
	PLAYERPETS *ptPets;
	int i;

	ptGame = SUnitGetGame(hPlayer);
	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
#line 734
	D2ASSERT(UnitGetType (hPlayer) == UNIT_PLAYER);
	ptPets = ptInfo->ptPets;
	if (!ptPets || ptPets->fLocked)
		return;
	for (i = 0; i < sgnPetSkills; i++)
		PlayerPetsSetMax(ptGame, hPlayer, sgatPetSkills[i].eType, (int)D2CommonSkillsCalcNodeLevel(hPlayer,
			D2CommonSkillsFindByKey(hPlayer, sgatPetSkills[i].nSkill), 1) + sgatPetSkills[i].nBonus);
	PlayerPetsSetMax(ptGame, hPlayer, EPETTYPE_HIRELING, 1);
	PlayerPetsSetMax(ptGame, hPlayer, 1, 1);
	switch (hPlayer->dwClassId)
	{
	case 0:
		PlayerPetsSetMax(ptGame, hPlayer, 2, 1);
		PlayerPetsSetMax(ptGame, hPlayer, 8, 1);
		break;
	case 2:
		PlayerPetsSetMax(ptGame, hPlayer, EPETTYPE_GOLEM, 1);
		break;
	}
}

void __fastcall PlayerPetsWarp(GAME *ptGame, UNIT *hPlayer, int nX, int nY)
{
	PLAYERPETS *ptPets;
	GAMEROOM *hRoom;
	GAMEROOM *hNewRoom;
	PETSET *ptSet;
	PETNODE *ptNode;
	PETNODE *ptNext;
	UNIT *hPet;
	COLPOINT tCoord;
	int eType;

	ptPets = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->ptPets;
	if (!ptPets)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hPlayer);
	for (eType = 1; eType < NUM_PET_TYPES; eType++)
	{
		ptSet = PlayerPetsGetSet(ptPets, eType);
		if (!ptSet)
			continue;
		if (eType > EPETTYPE_NONE && eType <= EPETTYPE_HIRELING)
		{
			for (ptNode = ptSet->ptHead; ptNode; ptNode = ptNode->ptNext)
			{
				hPet = SUnitFind(ptGame, UNIT_MONSTER, ptNode->dwUnitId);
				if (!hPet)
					continue;
				tCoord.nX = nX;
				tCoord.nY = nY;
				hNewRoom = (GAMEROOM *)D2CommonCollisnFindSpawnPointNear(hRoom, &tCoord,
					D2CommonUnitGetSizeX(hPet), PET_WARP_MASK, 0);
				if (!hNewRoom)
					continue;
				SUnitMoveToRoom(ptGame, hPet, hNewRoom, tCoord.nX, tCoord.nY, 0, 0);
				D2CommonUnitSetRoomChanged(hPet, 1);
				SUnitUpdateClients(ptGame, hPet, NULL);
				MonsterRestartAi(ptGame, hPet);
			}
		}
		else
		{
			for (ptNode = ptSet->ptHead; ptNode; ptNode = ptNext)
			{
				ptNext = ptNode->ptNext;
				PlayerPetsKillUnit(ptGame, ptNode->dwUnitId);
#line 793
				D2FREEPOOL(ptGame->pMemPool, ptNode);
			}
			ptSet->ptHead = NULL;
			ptSet->nCurrent = 0;
		}
	}
}

void __fastcall PlayerPetsSendLife(GAME *ptGame, UNIT *hPlayer)
{
	PLAYERPETS *ptPets;
	CLIENT *ptClient;
	PETSET *ptSet;
	PETNODE *ptNode;
	UNIT *hPet;
	int eType;

	ptPets = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->ptPets;
	if (!ptPets)
		return;
	ptClient = SUnitGetClient(hPlayer);
	for (eType = 1; eType < NUM_PET_TYPES; eType++)
	{
		ptSet = PlayerPetsGetSet(ptPets, eType);
		if (!ptSet || eType < 2 || (eType > 3 && eType != EPETTYPE_HIRELING))
			continue;
		for (ptNode = ptSet->ptHead; ptNode; ptNode = ptNode->ptNext)
		{
			hPet = SUnitFind(ptGame, UNIT_MONSTER, ptNode->dwUnitId);
			if (hPet)
				SCmdSendLife(ptClient, hPet);
		}
	}
}

void __fastcall PlayerPetsForEach(GAME *ptGame, UNIT *hPlayer, PFNPETCALLBACK pfnCallBack
, void *pArg)
{
	PLRINFO *ptInfo;
	PLAYERPETS *ptPets;
	PETSET *ptSet;
	PETNODE *ptNode;
	UNIT *hPet;
	int eType;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
#line 958
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)pfnCallBack), "pfnCallBack");
	ptPets = ptInfo->ptPets;
	if (!ptPets)
		return;
	for (eType = 1; eType < NUM_PET_TYPES; eType++)
	{
		ptSet = PlayerPetsGetSet(ptPets, eType);
		if (!ptSet)
			continue;
		for (ptNode = ptSet->ptHead; ptNode; ptNode = ptNode->ptNext)
		{
			hPet = SUnitFind(ptGame, UNIT_MONSTER, ptNode->dwUnitId);
			if (hPet)
				pfnCallBack(ptGame, hPlayer, hPet, pArg);
		}
	}
}

void __fastcall PlayerPetsLock(UNIT *hPlayer)
{
	PLAYERPETS *ptPets;

	ptPets = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->ptPets;
	if (ptPets)
		ptPets->fLocked = TRUE;
}

void __fastcall PlayerPetsUnlock(UNIT *hPlayer)
{
	PLAYERPETS *ptPets;

	ptPets = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->ptPets;
	if (ptPets)
	{
		ptPets->fLocked = FALSE;
		PlayerPetsUpdateMax(hPlayer);
	}
}

UNIT * __fastcall PlayerPetsFind(GAME *ptGame, UNIT *hPlayer, int ePetType  )
{
	PLRINFO *ptInfo;
	PETSET *ptSet;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
	if (!ptInfo)
		return NULL;
#line 1039
	D2ASSERT(ePetType < NUM_PET_TYPES);
	if (!ptInfo->ptPets)
		return NULL;
	ptSet = &ptInfo->ptPets->atSets[ePetType];
	if (!ptSet)
		return NULL;
	if (!ptSet->ptHead)
		return NULL;
	return SUnitFind(ptGame, UNIT_MONSTER, ptSet->ptHead->dwUnitId);
}

int __fastcall PlayerPetsCount(UNIT *hPlayer)
{
	PLRINFO *ptInfo;
	PLAYERPETS *ptPets;
	int nCount;
	int i;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
	nCount = 0;
	if (!ptInfo)
		return nCount;
	ptPets = ptInfo->ptPets;
	if (!ptPets)
		return nCount;
	for (i = 0; i < NUM_PET_TYPES; i++)
		nCount += ptPets->atSets[i].nCurrent;
	return nCount;
}

void __fastcall PlayerPetsSendAll(CLIENT *ptClient, UNIT *hPlayer)
{
	PLRINFO *ptInfo;
	GAME *ptGame;
	PLAYERPETS *ptPets;
	PETSET *ptSet;
	PETNODE *ptNode;
	UNIT *hPet;
	DWORD dwOwnerId;
	int eType;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
	ptGame = SUnitGetGame(hPlayer);
	dwOwnerId = hPlayer->dwId;
	if (!ptInfo || !ptGame)
		return;
	ptPets = ptInfo->ptPets;
	if (!ptPets)
		return;
	for (eType = 0; eType < NUM_PET_TYPES; eType++)
	{
		ptSet = &ptPets->atSets[eType];
		if (!ptSet)
			continue;
		for (ptNode = ptSet->ptHead; ptNode; ptNode = ptNode->ptNext)
		{
			hPet = SUnitFind(ptGame, UNIT_MONSTER, ptNode->dwUnitId);
			if (!hPet)
				continue;
			if (eType != EPETTYPE_HIRELING)
				SCmdSendPet(ptClient, 1, eType, ptNode->dwUnitId, dwOwnerId, (WORD)hPet->dwClassId);
			else
				SCmdSendHireling(ptClient, 1, EPETTYPE_HIRELING, ptNode->dwUnitId, dwOwnerId,
					(WORD)hPet->dwClassId, ptNode->tData.dwSeed, ptNode->tData.dwHireId);
		}
	}
}
