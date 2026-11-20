#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\Ai\\AiTatics.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "../MONSTER/MonsterMode.h"
#include "../GUILDS/Guilds.h"
#include "AiTatics.h"
#include "../SKILLS/Skills.h"
#include <stdio.h>
#include "../GAME/SCmd.h"
#include "../UNIT/SUnit.h"
#include "AiThink.h"
#include "AiUtil.h"

#define AIMOVE_EVENTDELETE		4
#define AIMOVE_MARK				1
#define AIMOVE_STAND			2
#define EVENT_RUN_AI			2
#define UNIT_MONSTER			1
#define EVENT_CHAT				6
#define AIMSG_SAY				32

#pragma pack(push, 1)
struct AISAYMSG
{
 	BYTE		bCmd;
 	BYTE		bUnitType;
 	DWORD		dwUnitId;
 	BYTE		field_06;
 	BYTE		unobserved_07;
 	BYTE		field_08;
 	BYTE		unobserved_09;
 	WORD		wString;
 	BYTE		unobserved_0c[40 - 12];
};
#pragma pack(pop)
typedef struct AISAYMSG AISAYMSG;

BOOL __fastcall AiSetModeOnTarget(GAME *ptGame, UNIT *hUnit, int eMode, UNIT *hTarget)
{
	MONMODECHANGE tReq;

	MonsterModeInit(hUnit, eMode, &tReq);
	tReq.hTarget = hTarget;
	return MonsterModeSet(ptGame, &tReq, TRUE);
}

BOOL __fastcall AiSetModeAtPoint(GAME *ptGame, UNIT *hUnit, int eMode, int nX,
	int nY)
{
	MONMODECHANGE tReq;

	MonsterModeInit(hUnit, eMode, &tReq);
	tReq.nX = nX;
	tReq.nY = nY;
	return MonsterModeSet(ptGame, &tReq, TRUE);
}

BOOL __fastcall AiUseSkill(GAME *ptGame, UNIT *hUnit, int nSkill, UNIT *hTarget,
	int nX, int nY)
{
	MONMODECHANGE tReq;

	MonsterModeInit(hUnit, MONMODE_SEQUENCE, &tReq);
	D2CommonUnitSetSkillThirdCached(hUnit, D2CommonSkillsFindByKey(hUnit, nSkill));
	D2CommonPathSetStopDistance(hUnit->ptPath, 1);
	tReq.hTarget = hTarget;
	tReq.nX = nX;
	tReq.nY = nY;
	return MonsterModeSet(ptGame, &tReq, FALSE);
}

BOOL __fastcall AiUseSkillInMode(GAME *ptGame, UNIT *hUnit, int eMode,
	int nSkill, UNIT *hTarget, int nX, int nY)
{
	MONMODECHANGE tReq;
	MONMODECHANGE tStand;

	MonsterModeInit(hUnit, eMode, &tReq);
	D2CommonUnitSetSkillThirdCached(hUnit, D2CommonSkillsFindByKey(hUnit, nSkill));
	hUnit->dwFlags0 |= 64;
	D2CommonPathSetStopDistance(hUnit->ptPath, 1);
	tReq.hTarget = hTarget;
	tReq.nX = nX;
	tReq.nY = nY;
	if (MonsterModeSet(ptGame, &tReq, FALSE))
		return TRUE;
	if (hUnit->dwMode != MONMODE_NEUTRAL)
	{
		MonsterModeInit(hUnit, MONMODE_NEUTRAL, &tStand);
		tStand.hTarget = hUnit;
		MonsterModeSet(ptGame, &tStand, TRUE);
	}
	EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
	EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + 10);
	return FALSE;
}

struct EVENT * __fastcall AiStand(GAME *ptGame, UNIT *hUnit, int nDelay)
{
	MONMODECHANGE tReq;

	if (!nDelay)
		nDelay = 1;
	if (hUnit->dwMode != MONMODE_NEUTRAL)
	{
		MonsterModeInit(hUnit, MONMODE_NEUTRAL, &tReq);
		tReq.hTarget = hUnit;
		MonsterModeSet(ptGame, &tReq, TRUE);
	}
	EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
	return EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + nDelay);
}

struct EVENT * __fastcall AiThinkIn(GAME *ptGame, UNIT *hUnit, int nDelay)
{
	if (!nDelay)
		nDelay = 1;
	EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
	return EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + nDelay);
}

DWORD __fastcall AiThinkWithin(GAME *ptGame, UNIT *hUnit, int nDelay)
{
	int nFrame;

	if (!nDelay)
		nDelay = 1;
	nFrame = EventGetFirstFrame(ptGame, hUnit, EVENT_RUN_AI);
	if (nFrame && nFrame != ptGame->nFrame && (DWORD)nFrame < (DWORD)(ptGame->nFrame + nDelay))
		return (DWORD)nFrame;
	EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
	return (DWORD)EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + nDelay);
}

void __fastcall AiSetVelocity(UNIT *hUnit, int nMode, int nVel  ,
	BYTE bArg)
{
#line 255
	D2ASSERT((nVel >= -120) && (nVel <= 120));
	if (nMode == 1)
		nMode = 7;
	MonsterModeSetOverride((MONMODEDATA *)((MONSTERDATA *)hUnit->uTypeData)->ptModeData, nMode, nVel,
		bArg);
}

BOOL __fastcall AiWalkToTarget(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	DWORD fFlags  )
{
	return AiMove(ptGame, hUnit, hTarget, MONMODE_WALK, 0, 0, 1, fFlags);
}

BOOL __fastcall AiMove(GAME *ptGame  , UNIT *hUnit, UNIT *hTarget,
	int eMode, int nX, int nY, BYTE bDwStep,
	WORD fFlags  )
{
	MONMODECHANGE tReq;
	MONMODECHANGE tStand;

#line 265
	D2ASSERT(ptGame);
	if (eMode == MONMODE_RUN && D2CommonStatsGetState(hUnit, 60))
	{
		MonsterModeClearOverride(hUnit);
		eMode = MONMODE_WALK;
	}
	D2CommonPathSetStopDistance(hUnit->ptPath, bDwStep);
	MonsterModeInit(hUnit, eMode, &tReq);
	if (hTarget)
		tReq.hTarget = hTarget;
	else
	{
		tReq.nX = nX;
		tReq.nY = nY;
	}
	eMode = MonsterModeSet(ptGame, &tReq, TRUE);
	if (!eMode)
	{
		if (fFlags & AIMOVE_EVENTDELETE)
			EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
		if (fFlags & AIMOVE_MARK)
			AiRecordSetFlags((AIRECORDHEAD *)((MONSTERDATA *)hUnit->uTypeData)->ptAi, AIFLAG_40, TRUE);
		if (fFlags & AIMOVE_STAND)
		{
#line 304
			D2ASSERT(fFlags & AIMOVE_EVENTDELETE);
			if (D2CommonSeedRand(&hUnit->tSeed, 100) < 70)
				AiStepAside(ptGame, hUnit, 4);
			else
			{
				if (hUnit->dwMode != MONMODE_NEUTRAL)
				{
					MonsterModeInit(hUnit, MONMODE_NEUTRAL, &tStand);
					tStand.hTarget = hUnit;
					MonsterModeSet(ptGame, &tStand, TRUE);
				}
				EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
				EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + 10);
			}
		}
	}
	return eMode;
}

BOOL __fastcall AiRunToTarget(GAME *ptGame, UNIT *hUnit, UNIT *hTarget)
{
	return AiMove(ptGame, hUnit, hTarget, MONMODE_RUN, 0, 0, 1, 0);
}

BOOL __fastcall AiChargeTarget(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	DWORD fFlags  )
{
	AiSetVelocity(hUnit, 13, 0, 0);
	return AiMove(ptGame, hUnit, hTarget, MONMODE_WALK, 0, 0, 1, fFlags);
}

BOOL __fastcall AiWalkToPoint(GAME *ptGame, UNIT *hUnit, int nX, int nY)
{
	return AiMove(ptGame, hUnit, NULL, MONMODE_WALK, nX, nY, 1, 0);
}

BOOL __fastcall AiRunToPoint(GAME *ptGame, UNIT *hUnit, int nX, int nY)
{
	return AiMove(ptGame, hUnit, NULL, MONMODE_RUN, nX, nY, 1, 0);
}

BOOL __fastcall AiWalkToPointZeroStop(GAME *ptGame, UNIT *hUnit, int nX,
	int nY)
{
	return AiMove(ptGame, hUnit, NULL, MONMODE_WALK, nX, nY, 0, 0);
}

BOOL __fastcall AiWalkToTargetStep(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, BYTE bStep)
{
	if (!bStep)
		bStep = 1;
	return AiMove(ptGame, hUnit, hTarget, MONMODE_WALK, 0, 0, bStep, 0);
}

BOOL __fastcall AiRunToTargetStep(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, BYTE bStep)
{
	if (!bStep)
		bStep = 1;
	return AiMove(ptGame, hUnit, hTarget, MONMODE_RUN, 0, 0, bStep, 0);
}

static __inline DWORD AiMoveFlags(BOOL bDelete) { if (bDelete) return AIMOVE_EVENTDELETE; return 0; }
BOOL __fastcall AiWalkAway(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, BYTE dwDist,
	BOOL bDelete)
{
	DWORD dwX;
	DWORD dwY;
	DWORD dwTargetX;
	DWORD dwTargetY;
	int nDX;
	int nDY;
	int nDist;

	dwX = D2CommonUnitGetX(hUnit);
	dwY = D2CommonUnitGetY(hUnit);
	dwTargetX = D2CommonUnitGetX(hTarget);
	dwTargetY = D2CommonUnitGetY(hTarget);
	nDY = 0;
	nDX = 0;
	if (dwTargetX > dwX)
		nDX = -1;
	if (dwTargetX < dwX)
		nDX = 1;
	if (dwTargetY > dwY)
		nDY = -1;
	if (dwTargetY < dwY)
		nDY = 1;
	nDist = (BYTE)dwDist;
	dwX += nDist * nDX;
	dwY += nDist * nDY;
	if ((BYTE)dwDist > 5)
		AiSetVelocity(hUnit, 0, 0, dwDist);
	return AiMove(ptGame, hUnit, NULL, MONMODE_WALK, (int)dwX, (int)dwY, 1, AiMoveFlags(bDelete));
}

BOOL __fastcall AiRunAway(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, BYTE dwDist,
	BOOL bDelete)
{
	DWORD dwX;
	DWORD dwY;
	DWORD dwTargetX;
	DWORD dwTargetY;
	int nDX;
	int nDY;
	int nDist;

	dwX = D2CommonUnitGetX(hUnit);
	dwY = D2CommonUnitGetY(hUnit);
	dwTargetX = D2CommonUnitGetX(hTarget);
	dwTargetY = D2CommonUnitGetY(hTarget);
	nDY = 0;
	nDX = 0;
	if (dwTargetX > dwX)
		nDX = -1;
	if (dwTargetX < dwX)
		nDX = 1;
	if (dwTargetY > dwY)
		nDY = -1;
	if (dwTargetY < dwY)
		nDY = 1;
	nDist = (BYTE)dwDist;
	dwX += nDist * nDX;
	dwY += nDist * nDY;
	if ((BYTE)dwDist > 5)
		AiSetVelocity(hUnit, 0, 0, dwDist);
	return AiMove(ptGame, hUnit, NULL, MONMODE_RUN, (int)dwX, (int)dwY, 1, AiMoveFlags(bDelete));
}

BOOL __fastcall AiStepAside(GAME *ptGame, UNIT *hUnit, BYTE bRange)
{
	D2SEED *ptSeed;
	int nX;
	int nY;
	int nDX;
	int nDY;
	int nRange;

	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	ptSeed = &hUnit->tSeed;
	if (D2CommonSeedNext(ptSeed) & 1)
	{
		nDX = bRange;
		nRange = bRange;
		if (nRange)
			nDY = (int)D2CommonSeedRand(ptSeed, nRange);
		else
			nDY = 0;
	}
	else
	{
		nRange = bRange;
		nDY = nRange;
		if (nRange)
			nDX = (int)D2CommonSeedRand(ptSeed, nRange);
		else
			nDX = 0;
	}
	if (D2CommonSeedNext(ptSeed) & 1)
		nDX = -nDX;
	if (D2CommonSeedNext(ptSeed) & 1)
		nDY = -nDY;
	return AiMove(ptGame, hUnit, NULL, MONMODE_WALK, nX + nDX, nY + nDY, 1, 0);
}

BOOL __fastcall AiWalkAround(GAME *ptGame, UNIT *hUnit, UNIT *hOther,
	BYTE bRange)
{
	D2SEED *ptSeed;
	int nX;
	int nY;
	int nDX;
	int nDY;
	int nRange;

	nX = (int)D2CommonUnitGetX(hOther);
	nY = (int)D2CommonUnitGetY(hOther);
	ptSeed = &hUnit->tSeed;
	if (D2CommonSeedNext(ptSeed) & 1)
	{
		nDX = bRange;
		nRange = bRange;
		if (nRange)
			nDY = (int)D2CommonSeedRand(ptSeed, nRange);
		else
			nDY = 0;
	}
	else
	{
		nRange = bRange;
		nDY = nRange;
		if (nRange)
			nDX = (int)D2CommonSeedRand(ptSeed, nRange);
		else
			nDX = 0;
	}
	if (D2CommonSeedNext(ptSeed) & 1)
		nDX = -nDX;
	if (D2CommonSeedNext(ptSeed) & 1)
		nDY = -nDY;
	return AiMove(ptGame, hUnit, NULL, MONMODE_WALK, nX + nDX, nY + nDY, 1, 0);
}

BOOL __fastcall AiRunAround(GAME *ptGame, UNIT *hUnit, UNIT *hOther,
	BYTE bRange)
{
	D2SEED *ptSeed;
	int nX;
	int nY;
	int nDX;
	int nDY;
	int nRange;

	nX = (int)D2CommonUnitGetX(hOther);
	nY = (int)D2CommonUnitGetY(hOther);
	ptSeed = &hUnit->tSeed;
	if (D2CommonSeedNext(ptSeed) & 1)
	{
		nDX = bRange;
		nRange = bRange;
		if (nRange)
			nDY = (int)D2CommonSeedRand(ptSeed, nRange);
		else
			nDY = 0;
	}
	else
	{
		nRange = bRange;
		nDY = nRange;
		if (nRange)
			nDX = (int)D2CommonSeedRand(ptSeed, nRange);
		else
			nDX = 0;
	}
	if (D2CommonSeedNext(ptSeed) & 1)
		nDX = -nDX;
	if (D2CommonSeedNext(ptSeed) & 1)
		nDY = -nDY;
	return AiMove(ptGame, hUnit, NULL, MONMODE_RUN, nX + nDX, nY + nDY, 1, 0);
}

BOOL __fastcall AiWalkToTargetFast(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, BYTE bArg,
	BOOL bDelete)
{
	int nMode;

	nMode = 6;
	if ((D2CommonSeedNext(&hUnit->tSeed) & 255) < 128)
		nMode = 5;
	AiSetVelocity(hUnit, nMode, 0, bArg);
	return AiMove(ptGame, hUnit, hTarget, MONMODE_WALK, 0, 0, 1, AiMoveFlags(bDelete));
}

void __fastcall AiUseMonCurseCast(GAME *ptGame, UNIT *hUnit, int eMode, UNIT *hTarget,
	int nCurseLevel, int nCurse)
{
	MONMODECHANGE tReq;
	MONMODECHANGE tStand;

	if (!ptGame || !hUnit)
		return;
	if (!hTarget)
		return;
	if (nCurseLevel <= 0)
		nCurseLevel = 1;
	if (nCurse > 6)
		return;
	D2CommonUnitSetTarget(hUnit, hTarget);
	SkillsSetLevel(hUnit, SKILL_MONCURSECAST, (DWORD)(WORD)nCurse << 16 | (WORD)nCurseLevel, 0);
	MonsterModeInit(hUnit, eMode, &tReq);
	D2CommonUnitSetSkillThirdCached(hUnit, D2CommonSkillsFindByKey(hUnit, 212));
	hUnit->dwFlags0 |= 64;
	D2CommonPathSetStopDistance(hUnit->ptPath, 1);
	tReq.hTarget = hTarget;
	tReq.nX = 0;
	tReq.nY = 0;
	if (MonsterModeSet(ptGame, &tReq, FALSE))
		return;
	if (hUnit->dwMode != MONMODE_NEUTRAL)
	{
		MonsterModeInit(hUnit, MONMODE_NEUTRAL, &tStand);
		tStand.hTarget = hUnit;
		MonsterModeSet(ptGame, &tStand, TRUE);
	}
	EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
	EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + 10);
}

void __fastcall AiThinkSay(GAME *ptGame, UNIT *hUnit, UNIT *hPlayer, WORD wString,
	BOOL bToClient)
{
	char szText[12];
	AISAYMSG tMsg;
	CLIENT *ptClient;
	void *ptNode;

	if (wString <= 0 || wString > 65535)
		return;
	if (bToClient)
	{
		ptClient = SUnitGetClient(hPlayer);
		tMsg.bCmd = AIMSG_SAY;
		tMsg.dwUnitId = hUnit->dwId;
		tMsg.bUnitType = UNIT_MONSTER;
		tMsg.field_06 = 1;
		tMsg.field_08 = 3;
		tMsg.wString = wString;
		SCmdSendUnitMessage(ptClient, (const SCMDSYSTEMMSG *)&tMsg);
		return;
	}
	sprintf(szText, "%d", wString);
	if (hUnit->ptChatNode)
		D2CommonChatNodeFree(ptGame->pMemPool, hUnit->ptChatNode);
	ptNode = (void *)D2CommonChatNodeNew(ptGame->pMemPool, szText, ptGame->nFrame);
	hUnit->ptChatNode = ptNode;
	D2CommonUnitRoomAddToChangedList(hUnit);
	hUnit->dwFlags0 |= 256;
	EventAdd(ptGame, hUnit, EVENT_CHAT, (int)D2CommonChatNodeGetEndFrame(ptNode));
}

BOOL __fastcall AiSetModeToPoint(GAME *ptGame, UNIT *hUnit, int nX, int nY,
	int eMode)
{
	MONMODECHANGE tReq;

	MonsterModeInit(hUnit, eMode, &tReq);
	D2CommonPathSetStopDistance(hUnit->ptPath, 1);
	tReq.nX = nX;
	tReq.nY = nY;
	return MonsterModeSet(ptGame, &tReq, TRUE);
}
BOOL __fastcall AiSetModeToPointPathZero(GAME *ptGame, UNIT *hUnit, int nX,
	int nY,
	int eMode)
{
	MONMODECHANGE tReq;

	MonsterModeInit(hUnit, eMode, &tReq);
	D2CommonPathSetStopDistance(hUnit->ptPath, 0);
	tReq.nX = nX;
	tReq.nY = nY;
	return MonsterModeSet(ptGame, &tReq, TRUE);
}

BOOL __fastcall AiMoveToDistance(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int eMode,
	int nMax, int nDist)
{
	MONMODECHANGE tReq;
	int nSign;
	int nStep;
	int nTargetX;
	int nTargetY;
	int nX;
	int nY;
	int nDX;
	int nDY;
	int nSum;
	int nStepX;
	int nStepY;
	int nSignX;
	int nSignY;

	nSign = 1;
	nStep = AiThinkDistance(hUnit, hTarget);
	if (nStep < nDist)
		nSign = -1;
	nStep = AiMoveAbs(nStep - nDist);
	if (nStep > nMax)
		nStep = nMax;
	nTargetX = (int)D2CommonUnitGetX(hTarget);
	nTargetY = (int)D2CommonUnitGetY(hTarget);
	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	nStepX = 0;
	nStepY = 0;
	nDX = AiMoveAbs(nTargetX - nX);
	nDY = AiMoveAbs(nTargetY - nY);
	nSum = nDX + nDY;
	if (nSum < nStep)
		nSum = nStep;
	if (nSum > 0)
	{
		nStepX = nDX * nStep / nSum;
		nStepY = nDY * nStep / nSum;
		while (nStepX + nStepY < nStep)
		{
			nStepX++;
			nStepY++;
		}
	}
	nSignX = 0;
	nSignY = 0;
	if (nTargetX > nX)
		nSignX = 1;
	if (nTargetX < nX)
		nSignX = -1;
	if (nTargetY > nY)
		nSignY = 1;
	if (nTargetY < nY)
		nSignY = -1;
	MonsterModeInit(hUnit, eMode, &tReq);
	D2CommonPathSetStopDistance(hUnit->ptPath, 1);
	tReq.nX = nSignX * nStepX * nSign + nX;
	tReq.nY = nSignY * nStepY * nSign + nY;
	return MonsterModeSet(ptGame, &tReq, TRUE);
}

BOOL __fastcall AiWalkToDistance(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nMax,
	int nDist)
{
	return AiMoveToDistance(ptGame, hUnit, hTarget, 2, nMax, nDist);
}

extern BYTE __fastcall AiGetTargetFacing(UNIT *hUnit);
__inline BYTE __fastcall AiGetTargetFacing(UNIT *hUnit)
{
	return D2CommonPathGetTargetFacing(hUnit->ptPath);
}
