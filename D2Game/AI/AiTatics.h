#ifndef D2RE_D2GAME_AITATICS_H
#define D2RE_D2GAME_AITATICS_H

#include "../D2game.h"

struct EVENT;

BOOL __fastcall AiSetModeOnTarget(GAME *ptGame, UNIT *hUnit, int eMode, UNIT *hTarget);
BOOL __fastcall AiSetModeAtPoint(GAME *ptGame, UNIT *hUnit, int eMode, int nX,
	int nY);
BOOL __fastcall AiUseSkill(GAME *ptGame, UNIT *hUnit, int nSkill, UNIT *hTarget,
	int nX, int nY);
BOOL __fastcall AiUseSkillInMode(GAME *ptGame, UNIT *hUnit, int eMode,
	int nSkill, UNIT *hTarget, int nX, int nY);
struct EVENT * __fastcall AiStand(GAME *ptGame, UNIT *hUnit, int nDelay);
struct EVENT * __fastcall AiThinkIn(GAME *ptGame, UNIT *hUnit, int nDelay);
DWORD __fastcall AiThinkWithin(GAME *ptGame, UNIT *hUnit, int nDelay);
void __fastcall AiSetVelocity(UNIT *hUnit, int nMode, int nVel, BYTE bArg);
BOOL __fastcall AiWalkToTarget(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD fFlags);
BOOL __fastcall AiMove(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int eMode,
	int nX, int nY, BYTE dwStep, WORD fFlags);
BOOL __fastcall AiRunToTarget(GAME *ptGame, UNIT *hUnit, UNIT *hTarget);
BOOL __fastcall AiChargeTarget(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD fFlags);
BOOL __fastcall AiWalkToPoint(GAME *ptGame, UNIT *hUnit, int nX, int nY);
BOOL __fastcall AiRunToPoint(GAME *ptGame, UNIT *hUnit, int nX, int nY);
BOOL __fastcall AiWalkToPointZeroStop(GAME *ptGame, UNIT *hUnit, int nX,
	int nY);
BOOL __fastcall AiWalkToTargetStep(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, BYTE bStep);
BOOL __fastcall AiRunToTargetStep(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, BYTE bStep);
BOOL __fastcall AiWalkAway(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, BYTE dwDist,
	BOOL bDelete);
BOOL __fastcall AiRunAway(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, BYTE dwDist,
	BOOL bDelete);
BOOL __fastcall AiStepAside(GAME *ptGame, UNIT *hUnit, BYTE bRange);
BOOL __fastcall AiWalkAround(GAME *ptGame, UNIT *hUnit, UNIT *hOther,
	BYTE bRange);
BOOL __fastcall AiRunAround(GAME *ptGame, UNIT *hUnit, UNIT *hOther,
	BYTE bRange);
BOOL __fastcall AiWalkToTargetFast(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, BYTE bArg,
	BOOL bDelete);
void __fastcall AiUseMonCurseCast(GAME *ptGame, UNIT *hUnit, int eMode, UNIT *hTarget,
	int nCurseLevel, int nCurse);
void __fastcall AiThinkSay(GAME *ptGame, UNIT *hUnit, UNIT *hPlayer, WORD wString,
	BOOL bToClient);
BOOL __fastcall AiSetModeToPoint(GAME *ptGame, UNIT *hUnit, int nX, int nY,
	int eMode);
BOOL __fastcall AiSetModeToPointPathZero(GAME *ptGame, UNIT *hUnit, int nX,
	int nY,
	int eMode);
BOOL __fastcall AiMoveToDistance(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int eMode,
	int nMax, int nDist);
BOOL __fastcall AiWalkToDistance(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nMax,
	int nDist);
static inline int AiMoveAbs(int nValue)
{
	if (nValue < 0)
		return -nValue;
	else
		return nValue;
}

#endif
