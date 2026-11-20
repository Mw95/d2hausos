#include "../D2game.h"
#include "AiThink.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\Ai\\AiGeneral.cpp"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "../MONSTER/MonsterMode.h"
#include "../QUESTS/QUESTS.h"
#include "../UNIT/SUnit.h"
#include "../GUILDS/Guilds.h"
#include "AiTatics.h"
#include "AiGeneral.h"
#include "AiUtil.h"

#define MONDATA(hUnit)	((MONSTERDATA *)(hUnit)->uTypeData)
#define EVENT_RUN_AI			2

void __fastcall DebugRoll(GAME *ptGame, UNIT *hAttacker, UNIT *hTarget,
	int nChance, int nRoll, const char *pszName)
{
}

void __fastcall DebugResist(GAME *ptGame, UNIT *hAttacker, UNIT *hTarget,
	int nOld, int nResist, int nReduce,
	int nNew,
	const char *pszName, DWORD dwArg)
{
}

AIGENERAL * __fastcall AiGeneralNew(GAME *ptGame)
{
	AIGENERAL *ptAi;
	int i;

#line 27
	ptAi = (AIGENERAL *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(AIGENERAL));
	for (i = 0; i < (int)(sizeof(AIGENERAL) / sizeof(DWORD)); i++)
		((DWORD *)ptAi)[i] = 0;
	ptAi->dwOwnerId = (DWORD)-1;
	ptAi->dwOwnerType = UNIT_MONSTER;
	ptAi->dwRefUnitType = 1;
	ptAi->dwRefUnitId = (DWORD)-1;
	return ptAi;
}

void __fastcall AiSetParam(UNIT *hUnit, int nIndex, DWORD dwValue)
{
	AIGENERAL *ptAi;

	ptAi = MONDATA(hUnit)->ptAi;
	if (!ptAi)
		return;
	switch (nIndex)
	{
	case 1:
		ptAi->adwParam[0] = dwValue;
		break;
	case 2:
		ptAi->adwParam[1] = dwValue;
		break;
	case 3:
		ptAi->adwParam[2] = dwValue;
		break;
	}
}

extern void AiFreeCommands(GAME *ptGame, AIGENERAL *ptAi);

void __fastcall AiGeneralFree(GAME *ptGame, AIGENERAL *ptAi)
{
	if (ptAi->pCell)
		D2CommonPresetFileListFree(ptGame->pMemPool, ptAi->pCell);
	AiFreeMinionList(ptGame, ptAi->ptMinions);
	AiFreeCommands(ptGame, ptAi);
#line 99
	D2FREEPOOL(ptGame->pMemPool, ptAi);
}

AICOMMAND * __fastcall AiAddCommand(GAME *ptGame, UNIT *hUnit)
{
	AICOMMAND *ptNode;
	AIGENERAL *ptAi;
	int i;

#line 114
	ptNode = (AICOMMAND *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(AICOMMAND));
	for (i = 0; i < (int)(sizeof(AICOMMAND) / sizeof(DWORD)); i++)
		((DWORD *)ptNode)[i] = 0;
	ptAi = MONDATA(hUnit)->ptAi;
	if (!ptAi->ptCommandLast)
	{
		ptAi->ptCommandLast = ptNode;
		ptAi->ptCommand = ptNode;
		ptNode->ptNext = ptNode;
		ptNode->ptPrev = ptNode;
		return ptNode;
	}
	if (ptAi->ptCommandLast == ptAi->ptCommand)
		ptAi->ptCommandLast = ptNode;
	ptNode->ptNext = ptAi->ptCommand;
	ptNode->ptPrev = ptAi->ptCommand->ptPrev;
	ptAi->ptCommand->ptPrev = ptNode;
	ptNode->ptPrev->ptNext = ptNode;
	return ptNode;
}

void __fastcall AiPopCommand(GAME *ptGame, UNIT *hUnit)
{
	AIGENERAL *ptAi;
	AICOMMAND *ptNode;

	ptAi = MONDATA(hUnit)->ptAi;
	ptNode = ptAi->ptCommand;
	if (!ptNode)
		return;
	if (ptNode != ptNode->ptNext)
	{
		if (ptAi->ptCommandLast == ptNode)
			ptAi->ptCommandLast = ptNode->ptNext;
		ptAi->ptCommand = ptNode->ptNext;
		ptNode->ptPrev->ptNext = ptNode->ptNext;
		ptNode->ptNext->ptPrev = ptNode->ptPrev;
	}
	else
	{
		ptAi->ptCommandLast = NULL;
		ptAi->ptCommand = NULL;
	}
#line 156
	D2FREEPOOL(ptGame->pMemPool, ptNode);
}

void __fastcall AiClearCommands(GAME *ptGame, UNIT *hUnit)
{
	AIGENERAL *ptAi;

	ptAi = MONDATA(hUnit)->ptAi;
	while (ptAi->ptCommandLast)
		AiPopCommand(ptGame, hUnit);
}

AICOMMAND * __fastcall AiGetCommand(UNIT *hUnit)
{
	return MONDATA(hUnit)->ptAi->ptCommand;
}

AICOMMAND * __fastcall AiFindCommand(UNIT *hUnit, int nCommand,
	BOOL bMakeCurrent)
{
	AIGENERAL *ptAi;
	AICOMMAND *ptNode;

	ptAi = MONDATA(hUnit)->ptAi;
	if (!ptAi->ptCommand)
		return NULL;
	for (ptNode = ptAi->ptCommand->ptNext; ptNode->nCommand != nCommand && ptNode != ptAi->ptCommand;
		ptNode = ptNode->ptNext)
		;
	if (ptNode->nCommand != nCommand)
		return NULL;
	if (bMakeCurrent)
		ptAi->ptCommand = ptNode;
	return ptNode;
}

void __fastcall AiCommandMinions(GAME *ptGame, UNIT *hUnit, AICOMMAND *ptCommand)
{
	AIGENERAL *ptAi;
	AIMINION *ptMinion;
	UNIT *hOwner;
	UNIT *hMinion;

	if (hUnit->eType != UNIT_MONSTER)
		return;
	ptAi = MONDATA(hUnit)->ptAi;
	if (!ptAi->ptGame)
		return;
	hOwner = SUnitFind(ptAi->ptGame, ptAi->dwOwnerType, ptAi->dwOwnerId);
	if (!hOwner || hOwner->eType != UNIT_MONSTER)
		return;
	ptAi = MONDATA(hOwner)->ptAi;
#line 264
	D2ASSERT(ptAi);
	for (ptMinion = ptAi->ptMinions; ptMinion; ptMinion = ptMinion->ptNext)
	{
		hMinion = SUnitFind(ptAi->ptGame, UNIT_MONSTER, ptMinion->dwUnitId);
		if (hMinion)
			AiPushCommand(ptGame, hMinion, ptCommand);
	}
}

__inline void AiFreeCommands(GAME *ptGame, AIGENERAL *ptAi)
{
	AICOMMAND *ptNode;

	while (ptAi->ptCommandLast)
	{
		ptNode = ptAi->ptCommand;
		if (!ptNode)
			break;
		if (ptNode != ptNode->ptNext)
		{
			if (ptAi->ptCommandLast == ptNode)
				ptAi->ptCommandLast = ptNode->ptNext;
			ptAi->ptCommand = ptNode->ptNext;
			ptNode->ptPrev->ptNext = ptNode->ptNext;
			ptNode->ptNext->ptPrev = ptNode->ptPrev;
		}
		else
		{
			ptAi->ptCommandLast = NULL;
			ptAi->ptCommand = NULL;
		}
#line 179
		D2FREEPOOL(ptGame->pMemPool, ptNode);
	}
}

void __fastcall AiPushCommand(GAME *ptGame, UNIT *hUnit, AICOMMAND *ptCommand)
{
	AICOMMAND *ptNode;
	AIGENERAL *ptAi;

	ptNode = AiAddCommand(ptGame, hUnit);
	memcpy(&ptNode->nCommand, &ptCommand->nCommand, 20);
	ptAi = MONDATA(hUnit)->ptAi;
	if (ptNode)
		ptAi->ptCommand = ptNode;
	else
	{
		AICOMMAND * volatile ptDummy = ptAi->ptCommand;
	}
}

AICOMMAND * __fastcall AiFindOrAddCommand(GAME *ptGame, UNIT *hUnit, int nCommand,
	BOOL bMakeCurrent)
{
	AICOMMAND tCommand;

	if (!AiFindCommand(hUnit, nCommand, bMakeCurrent))
	{
		tCommand.nCommand = nCommand;
		tCommand.dwArg1 = 0;
		tCommand.dwArg2 = 0;
		tCommand.dwArg3 = 0;
		tCommand.dwArg4 = 0;
		AiPushCommand(ptGame, hUnit, &tCommand);
	}
	return AiFindCommand(hUnit, nCommand, bMakeCurrent);
}

void ** __stdcall AiGetCell(UNIT *hUnit)
{
	return &MONDATA(hUnit)->ptAi->pCell;
}

void __fastcall AiSetOwner(GAME *ptGame, UNIT *hUnit, DWORD dwOwnerId,
	DWORD dwOwnerType, BOOL bFlag01, BOOL bFlag02)
{
	AIGENERAL *ptAi;

	if (hUnit->eType != UNIT_MONSTER)
		return;
	ptAi = MONDATA(hUnit)->ptAi;
	if (bFlag02)
		AiRecordSetFlags((AIRECORDHEAD *)ptAi, AIFLAG_02, TRUE);
	if (bFlag01)
		AiRecordSetFlags((AIRECORDHEAD *)ptAi, AIFLAG_01, TRUE);
	ptAi->dwOwnerId = dwOwnerId;
	ptAi->dwOwnerType = dwOwnerType;
	ptAi->ptGame = ptGame;
}

void __fastcall AiGetOwnerId(UNIT *hUnit, DWORD *pdwOwnerId,
	DWORD *pdwOwnerType)
{
	AIGENERAL *ptAi;

	if (hUnit->eType != UNIT_MONSTER)
	{
		*pdwOwnerId = (DWORD)-1;
		return;
	}
	ptAi = MONDATA(hUnit)->ptAi;
	if (!ptAi->ptGame)
	{
		*pdwOwnerId = (DWORD)-1;
		return;
	}
	*pdwOwnerId = ptAi->dwOwnerId;
	*pdwOwnerType = ptAi->dwOwnerType;
}

UNIT * __fastcall AiGetOwner(UNIT *hUnit)
{
	AIGENERAL *ptAi;

	if (hUnit->eType != UNIT_MONSTER)
		return NULL;
	ptAi = MONDATA(hUnit)->ptAi;
	if (!ptAi->ptGame)
		return NULL;
	return SUnitFind(ptAi->ptGame, ptAi->dwOwnerType, ptAi->dwOwnerId);
}

void __fastcall AiAddMinion(GAME *ptGame, UNIT *hUnit, UNIT *hMinion)
{
	AIGENERAL *ptAi;
	AIMINION *ptNode;

	if (hUnit->eType != UNIT_MONSTER)
		return;
	ptAi = MONDATA(hUnit)->ptAi;
#line 392
	ptNode = (AIMINION *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(AIMINION));
	memset(ptNode, 0, sizeof(AIMINION));
	ptNode->dwUnitId = hMinion->dwId;
	if (ptAi->ptMinions)
		ptNode->ptNext = ptAi->ptMinions;
	ptAi->ptMinions = ptNode;
}

void __fastcall AiRemoveFromOwner(UNIT *hUnit)
{
	AIGENERAL *ptAi;
	AIGENERAL *ptOwnerAi;
	AIMINION *ptNode;
	AIMINION *ptPrev;
	GAME *ptGame;
	UNIT *hOwner;
	UNIT *hMinion;
	DWORD dwUnitId;

	if (hUnit->eType != UNIT_MONSTER)
		return;
	ptAi = MONDATA(hUnit)->ptAi;
	if (!ptAi->ptGame)
		return;
	hOwner = SUnitFind(ptAi->ptGame, ptAi->dwOwnerType, ptAi->dwOwnerId);
	if (!hOwner || hOwner->eType != UNIT_MONSTER)
		return;
	ptOwnerAi = MONDATA(hOwner)->ptAi;
	ptPrev = NULL;
	ptNode = ptOwnerAi->ptMinions;
	dwUnitId = hUnit->dwId;
	ptGame = ptOwnerAi->ptGame;
	for (; ptNode; ptPrev = ptNode, ptNode = ptNode->ptNext)
	{
		if (ptNode->dwUnitId == dwUnitId)
		{
			hMinion = SUnitFind(ptGame, UNIT_MONSTER, ptNode->dwUnitId);
			if (hMinion)
				AiSetOwner(ptGame, hMinion, (DWORD)-1, UNIT_MONSTER, FALSE, FALSE);
			if (ptPrev)
				ptPrev->ptNext = ptNode->ptNext;
			else
				ptOwnerAi->ptMinions = ptNode->ptNext;
#line 428
			D2FREEPOOL(ptGame->pMemPool, ptNode);
			return;
		}
	}
}

void __fastcall AiFreeMinions(UNIT *hUnit)
{
	AIGENERAL *ptAi;
	AIGENERAL *ptOwnerAi;
	AIMINION *ptNode;
	AIMINION *ptNext;
	GAME *ptGame;
	UNIT *hOwner;
	UNIT *hMinion;

	if (hUnit->eType != UNIT_MONSTER)
		return;
	ptAi = MONDATA(hUnit)->ptAi;
	if (!ptAi->ptGame)
		return;
	hOwner = SUnitFind(ptAi->ptGame, ptAi->dwOwnerType, ptAi->dwOwnerId);
	if (!hOwner || hOwner != hUnit || hOwner->eType != UNIT_MONSTER)
		return;
	ptOwnerAi = MONDATA(hOwner)->ptAi;
	ptNode = ptOwnerAi->ptMinions;
	ptGame = ptOwnerAi->ptGame;
	while (ptNode)
	{
		ptNext = ptNode->ptNext;
		hMinion = SUnitFind(ptGame, UNIT_MONSTER, ptNode->dwUnitId);
		if (hMinion)
			AiSetOwner(ptGame, hMinion, (DWORD)-1, UNIT_MONSTER, FALSE, FALSE);
#line 460
		D2FREEPOOL(ptGame->pMemPool, ptNode);
		ptNode = ptNext;
	}
	ptOwnerAi->ptMinions = NULL;
}

void __fastcall AiFreeMinionList(GAME *ptGame, AIMINION *ptMinion)
{
	AIMINION *ptNext;

	while (ptMinion)
	{
		ptNext = ptMinion->ptNext;
#line 475
		D2FREEPOOL(ptGame->pMemPool, ptMinion);
		ptMinion = ptNext;
	}
}

void __fastcall AiForEachMinion(UNIT *hUnit, DWORD dwArg1, DWORD dwArg2,
	PFNAIMINIONFN pfnParty  )
{
	AIGENERAL *ptAi;
	AIMINION *ptNode;
	AIMINION *ptNext;
	GAME *ptGame;
	UNIT *hOwner;
	UNIT *hMinion;

#line 483
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)pfnParty), "pfnParty");
	if (hUnit->eType != UNIT_MONSTER)
		return;
	ptAi = MONDATA(hUnit)->ptAi;
	if (!ptAi->ptGame)
		return;
	hOwner = SUnitFind(ptAi->ptGame, ptAi->dwOwnerType, ptAi->dwOwnerId);
	if (!hOwner || hOwner->eType != UNIT_MONSTER)
		return;
	ptAi = MONDATA(hOwner)->ptAi;
	ptGame = ptAi->ptGame;
	for (ptNode = ptAi->ptMinions; ptNode; ptNode = ptNext)
	{
		ptNext = ptNode->ptNext;
		hMinion = SUnitFind(ptGame, UNIT_MONSTER, ptNode->dwUnitId);
		if (hMinion)
			pfnParty(hMinion, dwArg1, dwArg2);
	}
}

void __fastcall AiTakeOwner(UNIT *hUnit, DWORD *pdwOwnerId,
	DWORD *pdwOwnerType, BOOL *pbFlag01, BOOL *pbFlag02,
	AIMINION **pptMinions)
{
	AIGENERAL *ptAi;

	if (hUnit->eType != UNIT_MONSTER)
	{
		*pdwOwnerId = (DWORD)-1;
		return;
	}
	ptAi = MONDATA(hUnit)->ptAi;
	if (ptAi->dwOwnerType != UNIT_MONSTER)
	{
		*pdwOwnerId = (DWORD)-1;
		return;
	}
	*pdwOwnerId = ptAi->dwOwnerId;
	*pdwOwnerType = ptAi->dwOwnerType;
	*pbFlag01 = AiRecordTestFlags((AIRECORDHEAD *)ptAi, AIFLAG_01);
	*pbFlag02 = AiRecordTestFlags((AIRECORDHEAD *)ptAi, AIFLAG_02);
	*pptMinions = ptAi->ptMinions;
	ptAi->dwOwnerId = (DWORD)-1;
	ptAi->ptMinions = NULL;
}

void __fastcall AiRestoreOwner(GAME *ptGame, UNIT *hUnit, DWORD dwOwnerId,
	DWORD dwOwnerType, BOOL bFlag01, BOOL bFlag02,
	AIMINION *ptMinions)
{
	AIGENERAL *ptAi;

	if (!hUnit || hUnit->eType != UNIT_MONSTER)
		return;
	ptAi = MONDATA(hUnit)->ptAi;
	AiFreeMinionList(ptGame, ptAi->ptMinions);
	AiSetOwner(ptGame, hUnit, dwOwnerId, dwOwnerType, bFlag01, bFlag02);
	ptAi->ptMinions = ptMinions;
}

extern void AiPromoteFirstMinion(UNIT *hUnit, AIGENERAL *ptAi);
__inline void AiPromoteFirstMinion(UNIT *hUnit, AIGENERAL *ptAi)
{
	AIMINION *ptFirst;
	AIMINION *ptNode;
	GAME *ptGame;
	UNIT *hLeader;
	UNIT *hMinion;

	ptFirst = ptAi->ptMinions;
	if (!ptFirst)
		return;
	ptGame = ptAi->ptGame;
	for (ptNode = ptFirst; ptNode; ptNode = ptNode->ptNext)
	{
		hLeader = SUnitFind(ptGame, UNIT_MONSTER, ptNode->dwUnitId);
		if (hLeader)
			break;
	}
	if (!ptNode)
		return;
	AiSetOwner(ptGame, hLeader, ptNode->dwUnitId, UNIT_MONSTER, TRUE, TRUE);
	AiSetOwner(ptGame, hUnit, ptNode->dwUnitId, UNIT_MONSTER, FALSE, FALSE);
	AiAddMinion(ptGame, hLeader, hUnit);
	do
	{
		hMinion = SUnitFind(ptGame, UNIT_MONSTER, ptFirst->dwUnitId);
		if (hMinion && hMinion != hLeader)
		{
			AiSetOwner(ptGame, hMinion, ptNode->dwUnitId, UNIT_MONSTER, FALSE, FALSE);
			AiAddMinion(ptGame, hLeader, hMinion);
		}
		ptFirst = ptFirst->ptNext;
	} while (ptFirst);
}

void __fastcall AiPassLeadership(UNIT *hUnit)
{
	AIGENERAL *ptAi;
	AIMINION *ptNode;
	GAME *ptGame;
	UNIT *hMinion;

	ptAi = MONDATA(hUnit)->ptAi;
	if (!AiRecordTestFlags((AIRECORDHEAD *)ptAi, AIFLAG_01))
		return;
	if (AiRecordTestFlags((AIRECORDHEAD *)ptAi, AIFLAG_02))
	{
		AiPromoteFirstMinion(hUnit, ptAi);
		return;
	}
	ptNode = ptAi->ptMinions;
	ptGame = ptAi->ptGame;
	for (; ptNode; ptNode = ptNode->ptNext)
	{
		hMinion = SUnitFind(ptGame, UNIT_MONSTER, ptNode->dwUnitId);
		if (hMinion)
			AiSetOwner(ptGame, hMinion, (DWORD)-1, UNIT_MONSTER, FALSE, FALSE);
	}
}

static __inline int AiIdleDelayI(int nParam)
{
	if (nParam >= 35)
		nParam = 25;
	else if (nParam < 25)
		nParam = 10;
	else
		nParam -= 10;
	return nParam;
}

UNIT * __fastcall AiIdleByParam(GAME *ptGame, UNIT *hUnit, AIGENERAL *ptAi, int *pnParam,
	DWORD *pdwDir)
{
	MONMODECHANGE tReq;
	UNIT *hResult;
	int nDelay;
	DWORD dwSize;

	hResult = AiFindTarget(ptGame, hUnit, ptAi, pnParam, pdwDir);
	if (hResult)
		return hResult;
	if (MonsterTestPrevModeGetHit(hUnit) && MONDATA(hUnit)->ptMonStats->bModeW)
	{
		AiStepAside(ptGame, hUnit, 5);
		return NULL;
	}
	dwSize = D2CommonPathGetCollisionPatternByUnit(hUnit);
	if (D2CommonCollisnTestPatternAnyRaw(D2CommonUnitGetPathRoom(hUnit), D2CommonUnitGetX(hUnit),
			D2CommonUnitGetY(hUnit), dwSize, 64)
		&& MONDATA(hUnit)->ptMonStats->bModeW)
	{
		AiStepAside(ptGame, hUnit, 5);
		return NULL;
	}
	nDelay = AiIdleDelayI(*pnParam);
	if (!nDelay)
		nDelay = 1;
	if (hUnit->dwMode != MONMODE_NEUTRAL)
	{
		MonsterModeInit(hUnit, MONMODE_NEUTRAL, &tReq);
		tReq.hTarget = hUnit;
		MonsterModeSet(ptGame, &tReq, TRUE);
	}
	EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
	EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + nDelay);
	return NULL;
}

UNIT * __fastcall AiIdle(GAME *ptGame, UNIT *hUnit, AIGENERAL *ptAi, int *pnParam,
	DWORD *pdwDir)
{
	UNIT *hResult;
	DWORD dwSize;
	DWORD dwY;
	DWORD dwX;
	DWORD dwRoom;

	hResult = AiFindTarget(ptGame, hUnit, ptAi, pnParam, pdwDir);
	if (hResult)
		return hResult;
	dwSize = D2CommonPathGetCollisionPatternByUnit(hUnit);

	if (D2CommonCollisnTestPatternAnyRaw(D2CommonUnitGetPathRoom(hUnit), D2CommonUnitGetX(hUnit), D2CommonUnitGetY(hUnit), dwSize, 64)
		&& ((MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId))->bModeW)
	{
		AiStepAside(ptGame, hUnit, 5);
		return NULL;
	}
	EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
	EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + 20);
	return NULL;
}

UNIT * __fastcall AiTryWithin(GAME *ptGame, UNIT *hUnit, AIGENERAL *ptAi, int *pnParam,
	DWORD *pdwDir, int nMax)
{
	UNIT *hResult;

	hResult = AiFindTarget(ptGame, hUnit, ptAi, pnParam, pdwDir);
	if (!hResult || *pnParam > nMax)
		return NULL;
	return hResult;
}

UNIT * __fastcall AiFindNearPlayer(GAME *ptGame, UNIT *hUnit, BOOL *pbClose)
{
	AIFINDRECORD tFind;

	if (!D2CommonUnitGetPathRoom(hUnit))
		return NULL;
	tFind.tUnit.hUnit = NULL;
	tFind.tUnit.nDist = 2147483647;
	AiFind(ptGame, hUnit, &tFind, AiFindNearPlayerFn, 2);
	if (!tFind.tUnit.hUnit)
		return hUnit;
	if (tFind.tUnit.nDist < 4)
		*pbClose = TRUE;
	return tFind.tUnit.hUnit;
}

UNIT * __fastcall AiFindNearPlayerFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate,
	AIFINDRECORD *ptFind)
{
	int nDist;

	if (hCandidate->eType != UNIT_PLAYER)
		return NULL;
	nDist = AiThinkDistance(hUnit, hCandidate);
	if (nDist > 15)
		return NULL;
	if (!((MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId))->bInteract)
	{
		ptFind->tUnit.hUnit = hCandidate;
		ptFind->tUnit.nDist = nDist;
		return hCandidate;
	}
	if (!QuestsNpcActive(ptGame, hCandidate, hUnit) || nDist >= ptFind->tUnit.nDist)
		return NULL;
	ptFind->tUnit.hUnit = hCandidate;
	ptFind->tUnit.nDist = nDist;
	return hCandidate;
}
