#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\Ai\\AiThink.cpp"

#include <stdio.h>
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "../GAME/SCmd.h"
#include "../GAME/Targets.h"
#include "../MONSTER/Monster.h"
#include "../MONSTER/MonsterAI.h"
#include "../MONSTER/MonsterMode.h"
#include "../MONSTER/MonsterMsg.h"
#include "../MONSTER/MonsterUnique.h"
#include "../OBJECTS/ObjMode.h"
#include "../PLAYER/Player.h"
#include "../PLAYER/PlayerPets.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "../GUILDS/Guilds.h"
#include "../QUESTS/a1q4.h"
#include "../QUESTS/a1q5.h"
#include "../QUESTS/a2q1.h"
#include "../QUESTS/a2q4.h"
#include "../QUESTS/a2q5.h"
#include "../QUESTS/a2q6.h"
#include "../QUESTS/a3q3.h"
#include "../QUESTS/a3q4.h"
#include "../QUESTS/a3q7.h"
#include "../QUESTS/a4q1.h"
#include "AiGeneral.h"
#include "AiUtil.h"
#include "AiTatics.h"
#include "AiThink.h"
#include "../SKILLS/Skills.h"
#include "FogImports.h"
#define UNIT_PLAYER				0
#define UNIT_MONSTER			1
#define EVENT_RUN_AI			2
#define AIMOVE_ALL				7
#define AIMOVE_EVENTDELETE		4
#define AIMOVE_MARK				1
#define MONDATA(hUnit)	((MONSTERDATA *)(hUnit)->uTypeData)
#define UNIT_STATE_UNINTERRUPTABLE	54
#define StatsGetState(hUnit, nState) \
	D2CommonStatsGetStateRaw((DWORD)(hUnit), (DWORD)(nState))
#define UnitIsDead(hUnit)		SUnitIsDead(hUnit)
#define AIPARAM_KEEP			(-666)
#define AITHINK_ROWS			194
#define AITHINK_SPECIALS		14
static inline int AiThinkAbs(int nValue)
{
	if (nValue < 0)
		nValue = -nValue;
	return nValue;
}
#define ABS(nValue)				AiThinkAbs(nValue)

static int PLACEHOLDER_UnreferencedData_100bec48 = 0;
static BOOL gbAiThink91Toggle = 0;
static int gnAiThinkNpcWalks = 0;
static int PLACEHOLDER_UnreferencedData_100bec54 = 0;

void __fastcall AiThinkNone(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
}

void __fastcall AiThinkCircleTarget(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	int nDist;
	AIGENERAL *ptAi;

	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	if (!MonsterTestPrevModeGetHit(hUnit) && nDist >= 4)
	{
		ptAi = ptParms->ptAi;
		if (nDist > 10 && !ptAi->adwParam[0])
		{
			ptAi->adwParam[0] = 1;
			AiWalkAround(ptGame, hUnit, hTarget, 12);
			return;
		}
		if ((D2CommonSeedNext(&hUnit->tSeed) & 255) < 77)
		{
			AiStepAside(ptGame, hUnit, 4);
			return;
		}
		if ((D2CommonSeedNext(&hUnit->tSeed) & 255) < 26)
			ptAi->adwParam[0] = 0;
		AiStand(ptGame, hUnit, 10);
		return;
	}
	AiThinkSet(ptGame, hUnit, ptParms->ptAi, 0);
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkAttackOnce(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	BOOL bInReach;
	AIGENERAL *ptAi;

	hTarget = ptParms->hTarget;
	bInReach = ptParms->fTargetInReach;
	ptAi = ptParms->ptAi;
	if (ptAi->adwParam[0])
	{
		AiStand(ptGame, hUnit, 50);
		return;
	}
	if (bInReach)
	{
		ptAi->adwParam[0] = 1;
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkTheSmith(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	int nLife;

	hTarget = ptParms->hTarget;
	if (ptParms->fTargetInReach)
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	nLife = D2CommonUnitCalcLifePercent(hUnit);
	if (nLife < 0)
		nLife = 0;
	else if (nLife > 100)
		nLife = 100;
	AiSetVelocity(hUnit, 0, (100 - nLife) >> 1, 0);
	AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
}

void __fastcall AiThinkDoomKnight(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptSeed = &hUnit->tSeed;
	if (ptParms->fTargetInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < 90)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
	}
	else if (D2CommonSeedRand(ptSeed, 100) < 80)
	{
		AiChargeTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
		return;
	}
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkAbyssKnight(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	int nDist;
	BOOL bInReach;
	AIGENERAL *ptAi;
	D2SEED *ptSeed;
	BYTE bGfxIndex;

	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	bInReach = ptParms->fTargetInReach;
	ptSeed = &hUnit->tSeed;
	ptAi = ptParms->ptAi;
	if (D2CommonUnitCalcLifePercent(hUnit) < 40 && !D2CommonStatsGetState(hUnit, 14)
		&& D2CommonSeedRand(ptSeed, 100) < 70)
	{
		AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_MONBONEARMOR, NULL, 0, 0);
		return;
	}
	if (bInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < 90)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		AiStand(ptGame, hUnit, 10);
		return;
	}
	if (nDist < 6)
		ptAi->adwParam[0] = 1;
	if (!ptAi->adwParam[0])
	{
		bGfxIndex = hUnit->abComponentVariations[10];
#line 329
		D2ASSERT(bGfxIndex < 4);
		SkillsSetLevel(hUnit, SKILL_MISSILESKILL1, bGfxIndex + 323, 0);
		AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_MISSILESKILL1, hTarget, 0, 0);
		ptAi->adwParam[0] = 1;
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < 70)
	{
		AiSetVelocity(hUnit, 2, 0, (BYTE)(D2CommonSeedRand(ptSeed, 2) + 6));
		AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
	}
	else
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 3, FALSE);
	ptAi->adwParam[0] = 0;
}

void __fastcall AiThinkOblivionKnight(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	AIGENERAL *ptAi;
	int nDist;
	MONSTERSCALE tScale;
	int nGain;
	int nMaxLife;
	int nElapsed;
	AIFINDRECORD tFind;
	UNIT *hFound;
	UNIT *hNear;
	int nNearDist;
	DWORD dwNearDir;
	int nArg;
	BYTE bGfxIndex;
	D2SEED *ptSeed;

	ptAi = ptParms->ptAi;
	nDist = ptParms->nDist;
	hTarget = ptParms->hTarget;
	MonsterGetScale(&tScale, ptGame, D2CommonUnitGetPathRoom(hUnit), (DWORD)hUnit);
	if (ptAi->adwParam[1] && tScale.nLifePlayers > (int)ptAi->adwParam[1])
	{
		nGain = tScale.nLifePlayers - (int)ptAi->adwParam[1];
		if (nGain <= 0)
			nGain = 1;
		else if (nGain > 8)
			nGain = 8;
		nMaxLife = D2CommonStatsUnitCalcMaxHp(hUnit);
		D2CommonStatsUnitSetValue(hUnit, 6, (DWORD)D2CommonStatsUnitGetValue(hUnit, 6) * nGain);
		D2CommonStatsUnitSetValue(hUnit, 7, nMaxLife * nGain);
	}
	ptAi->adwParam[1] = tScale.nLifePlayers;
	nElapsed = ABS(ptGame->nFrame - (int)ptAi->adwParam[0]);
	tFind.tKin.hFound = NULL;
	tFind.tKin.hWeak = NULL;
	AiFind(ptGame, hUnit, &tFind, AiThinkFindKnight, 1);
	hFound = tFind.tKin.hFound;
	if (nDist < 6)
	{
		if (!D2CommonStatsGetState(hTarget, 60))
		{
			ptAi->adwParam[0] = ptGame->nFrame;
			AiUseMonCurseCast(ptGame, hUnit, MONMODE_SKILL2, hTarget, ptGame->bDifficultyLevel + 3, 4);
			return;
		}
		if (hFound && (DWORD)AiThinkDistance(hFound, hTarget) > (DWORD)nDist
			&& AiWalkToTarget(ptGame, hUnit, hFound, AIMOVE_EVENTDELETE))
			return;
		AiSetVelocity(hUnit, 2, 50, 0);
		if (AiWalkAway(ptGame, hUnit, hTarget, 10, TRUE))
			return;
		AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_MONBONESPIRIT, hTarget, 0, 0);
		return;
	}
	ptSeed = &hUnit->tSeed;
	hNear = AiFindTargetInSight(ptGame, hUnit, &nNearDist, &dwNearDir);
	if (hNear && nNearDist < 25)
	{
		if (nElapsed > 500 && D2CommonSeedRand(ptSeed, 100) < 50)
		{
			nArg = (int)D2CommonSeedRand(ptSeed, 6);
			ptAi->adwParam[0] = ptGame->nFrame;
			AiUseMonCurseCast(ptGame, hUnit, MONMODE_SKILL2, hNear, 3, nArg);
			return;
		}
		if (D2CommonSeedRand(ptSeed, 100) < 80)
		{
			if (D2CommonSeedRand(ptSeed, 100) < 70)
			{
				bGfxIndex = hUnit->abComponentVariations[10];
#line 510
				D2ASSERT(bGfxIndex < 4);
				SkillsSetLevel(hUnit, SKILL_MISSILESKILL1, bGfxIndex + 323, 0);
				AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_MISSILESKILL1, hNear, 0, 0);
				return;
			}
			AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_MONBONESPIRIT, hNear, 0, 0);
			return;
		}
	}
	if (nDist > 9 && D2CommonSeedRand(ptSeed, 100) < 30)
	{
		AiWalkAround(ptGame, hUnit, hTarget, 6);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < 70)
	{
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 3, FALSE);
		return;
	}
	AiStand(ptGame, hUnit, 10);
}

UNIT * __fastcall AiThinkFindKnight(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind)
{
	if (hCandidate->eType == UNIT_MONSTER
		&& (hCandidate->dwClassId == 310 || hCandidate->dwClassId == 311) && !SUnitIsDead(hCandidate)
		&& (hCandidate->dwFlags0 & 2)
		&& (DWORD)AiThinkDistance(hUnit, hCandidate) <= 30
		&& D2CommonStatsUnitsTestSameAlignment(hUnit,
			hCandidate) && D2CommonStatsUnitGetAlignment(hCandidate))
	{
		ptFind->tKin.hFound = hCandidate;
		if (D2CommonUnitCalcLifePercent(hCandidate) < 40)
			ptFind->tKin.hWeak = hCandidate;
		if (ptFind->tKin.hFound && ptFind->tKin.hWeak)
			return hUnit;
	}
	return NULL;
}

void __fastcall AiThinkMoonClan(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptSeed = &hUnit->tSeed;
	if (ptParms->fTargetInReach)
	{
		if ((D2CommonSeedNext(ptSeed) & 255) < 205)
		{
			if ((D2CommonSeedNext(ptSeed) & 255) < 128)
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			else
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
	}
	else if ((D2CommonSeedNext(ptSeed) & 255) < 192)
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
		return;
	}
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkBrute(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	MONSTATSTXT *ptMonStats;
	D2SEED *ptSeed;
	int nLife;

	hTarget = ptParms->hTarget;
	ptMonStats = ptParms->ptMonStats;
	ptSeed = &hUnit->tSeed;
	if (ptParms->fTargetInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
		{
			if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
			{
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
				return;
			}
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2])
		{
			AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
			return;
		}
		AiStand(ptGame, hUnit, 15);
		return;
	}
	nLife = D2CommonUnitCalcLifePercent(hUnit);
	if (nLife < 40)
		nLife = 40;
	else if (nLife > 100)
		nLife = 100;
	AiSetVelocity(hUnit, 0, 100 - nLife, 0);
	AiChargeTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
}

void __fastcall AiThinkThornedHulk(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	AIGENERAL *ptAi;
	MONSTATSTXT *ptMonStats;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptAi = ptParms->ptAi;
	ptMonStats = ptParms->ptMonStats;
	ptSeed = &hUnit->tSeed;
	if (ptParms->fTargetInReach)
	{
		if (ptAi->adwParam[0])
		{
			AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK2, SKILL_MONFRENZY, hTarget, 0, 0);
			ptAi->adwParam[0]--;
			return;
		}
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
		{
			if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[3])
			{
				ptAi->adwParam[0] = 5;
				AiStand(ptGame, hUnit, 1);
				return;
			}
			if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
			{
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
				return;
			}
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2])
		{
			AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
			return;
		}
		AiStand(ptGame, hUnit, 15);
		return;
	}
	ptAi->adwParam[0] = 0;
	AiChargeTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
}

void __fastcall AiThinkCarrionBird(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	int nDist;
	MONSTATSTXT *ptMonStats;
	BOOL bInReach;
	AIGENERAL *ptAi;
	int nParam;
	void *hRoom;
	int nX;
	int nY;
	int nUnitX;
	int nUnitY;
	int nRange;
	int nSpan;
	int nDraw;
	int nMin;
	BOOL bStay;
	AIFINDRECORD tFind;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptAi = ptParms->ptAi;
	nDist = ptParms->nDist;
	bInReach = ptParms->fTargetInReach;
	ptMonStats = ptParms->ptMonStats;
	ptSeed = &hUnit->tSeed;
	nParam = (int)ptAi->adwParam[0];
	if (!hTarget)
	{
		if (nParam < 1)
		{
			AiThinkIn(ptGame, hUnit, 12);
			return;
		}
		if (AiThinkEmerge(ptGame, hUnit))
		{
			AiMoveToDistance(ptGame, hUnit, hUnit, MONMODE_SKILL2, 2, 3);
			AiThinkIn(ptGame, hUnit, 12);
			ptParms->ptAi->adwParam[0] = (DWORD)-1;
			return;
		}
	}
	else
	{
		if (D2CommonUnitGetPathRoom(hTarget) != D2CommonUnitGetPathRoom(hUnit)
			&& (DWORD)AiThinkDistance(hUnit, hTarget) > 12)
		{
			if (nParam >= 1)
			{
				if (AiThinkEmerge(ptGame, hUnit))
				{
					AiMoveToDistance(ptGame, hUnit, hUnit, MONMODE_SKILL2, 2, 3);
					AiThinkIn(ptGame, hUnit, 12);
					ptParms->ptAi->adwParam[0] = (DWORD)-1;
					return;
				}
			}
			else
			{
				AiWalkToDistance(ptGame, hUnit, hTarget, 9, 0);
				return;
			}
		}
	}
	if (!nParam && !AiGetOwner(hUnit) && hTarget && (DWORD)AiThinkDistance(hUnit, hTarget) > 12
		&& D2CommonSeedRand(ptSeed, 100) < 60)
	{
		hUnit->dwFlags0 &= ~14;
		D2CommonCollisnClearPoint(D2CommonUnitGetPathRoom(hUnit), (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), 4096);
		D2CommonCollisnClearPoint(D2CommonUnitGetPathRoom(hUnit), (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), 256);
		D2CommonPathSetCollisionPatternByUnit(hUnit, 3);
		D2CommonPathSetCollisionTestMask(hUnit->ptPath, 0);
		AiThinkIn(ptGame, hUnit, 12);
		ptParms->ptAi->adwParam[0] = (D2CommonSeedNext(ptSeed) & 7) + 24;
		AiMoveToDistance(ptGame, hUnit, hTarget, MONMODE_SKILL1, 8, 8);
		AiThinkIn(ptGame, hUnit, 12);
		return;
	}
	tFind.tUnit.hUnit = NULL;
	tFind.tUnit.nDist = 11;
	AiFind(ptGame, hUnit, &tFind, AiThinkFindHurtUnit, 1);
	if (nParam > 1)
	{
		bStay = tFind.tUnit.hUnit || (nDist < 6 && D2CommonSeedRand(ptSeed, 100) < 15);
		if (bStay)
		{
			nParam = 1;
			ptParms->ptAi->adwParam[0] = 1;
		}
		else
		{
			hUnit->dwFlags0 &= ~14;
			D2CommonCollisnClearPoint(D2CommonUnitGetPathRoom(hUnit), (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), 4096);
			D2CommonCollisnClearPoint(D2CommonUnitGetPathRoom(hUnit), (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), 256);
			D2CommonPathSetCollisionPatternByUnit(hUnit, 3);
			D2CommonPathSetCollisionTestMask(hUnit->ptPath, 0);
			AiThinkIn(ptGame, hUnit, 12);
			nUnitX = (int)D2CommonUnitGetX(hUnit);
			nUnitY = (int)D2CommonUnitGetY(hUnit);
			if (ptParms->ptAi->adwParam[1] && ptParms->ptAi->adwParam[2]
				&& (DWORD)AiThinkPointDistance(hUnit, ptParms->ptAi->adwParam[1], ptParms->ptAi->adwParam[2]) > 1)
			{
				COLPOINT tPoint;

				tPoint.nX = (int)ptParms->ptAi->adwParam[1];
				tPoint.nY = (int)ptParms->ptAi->adwParam[2];
				if (hTarget && D2CommonCollisnFindSpawnPointNear(D2CommonUnitGetPathRoom(hTarget), &tPoint,
					(int)D2CommonUnitGetSizeX(hUnit), 65535, 1))
				{
					ptParms->ptAi->adwParam[1] = tPoint.nX;
					ptParms->ptAi->adwParam[2] = tPoint.nY;
				}
				else
				{

					if (D2CommonCollisnFindSpawnPointNear(D2CommonUnitGetPathRoom(hUnit), &tPoint, ((int)D2CommonUnitGetSizeX(hUnit)),
						65535, 1))
					{
						ptParms->ptAi->adwParam[1] = tPoint.nX;
						ptParms->ptAi->adwParam[2] = tPoint.nY;
					}
				}
				ptParms->ptAi->adwParam[0]--;
				AiSetVelocity(hUnit, 7, 0, 0);
				ptAi = ptParms->ptAi;
				AiSetModeToPoint(ptGame, hUnit, ptAi->adwParam[1], ptAi->adwParam[2], MONMODE_SKILL1);
				AiThinkIn(ptGame, hUnit, 12);
				return;
			}
			nRange = nParam + 8;
			nSpan = nRange << 1;
			if (nSpan)
				nDraw = (int)D2CommonSeedRand(ptSeed, nSpan);
			else
				nDraw = 0;
			nX = (int)D2CommonUnitGetX(hTarget) + nDraw - nRange;

			nY = (int)D2CommonUnitGetY(hTarget) + (nSpan ? (int)D2CommonSeedRand(ptSeed, nSpan) : 0) - nRange;
			if (nRange * 2 < 12)
				nMin = 12;
			else
			{
				nMin = nRange * 2;
				if (nMin > 36)
					nMin = 36;
			}
			while ((DWORD)AiThinkPointDistance(hUnit, nX, nY) < (DWORD)nMin)
			{
				if (nX < nUnitX)
					nX--;
				else if (nX > nUnitX)
					nX++;
				if (nY < nUnitY)
					nY--;
				else if (nY > nUnitY)
					nY++;
				if (nX == nUnitX && nY == nUnitY)
				{
					nX++;
					nY++;
				}
			}
			ptParms->ptAi->adwParam[1] = nX;
			ptParms->ptAi->adwParam[2] = nY;
			AiSetVelocity(hUnit, 1, 0, 0);
			AiSetModeToPoint(ptGame, hUnit, nX, nY, MONMODE_SKILL1);
			AiThinkIn(ptGame, hUnit, 12);
			ptParms->ptAi->adwParam[0]--;
			return;
		}
	}
	if (nParam >= 1)
	{
		if (AiThinkEmerge(ptGame, hUnit))
		{
			AiMoveToDistance(ptGame, hUnit, hTarget, MONMODE_SKILL2, 2, 3);
			AiThinkIn(ptGame, hUnit, 12);
			ptParms->ptAi->adwParam[0] = (DWORD)-1;
			return;
		}
		ptParms->ptAi->adwParam[0] = 8;
		AiThinkIn(ptGame, hUnit, 12);
	}
	if (bInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			AiThinkIn(ptGame, hUnit, 12);
			return;
		}
		AiThinkIn(ptGame, hUnit, 8);
		return;
	}
	if (nParam != -1 && D2CommonSeedRand(ptSeed, 100) >= ptMonStats->AIParam[4])
	{
		AiThinkIn(ptGame, hUnit, 8);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < 70)
		AiWalkToDistance(ptGame, hUnit, hTarget, 9, 0);
	else
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 6, FALSE);
	ptParms->ptAi->adwParam[0] = 0;
	AiThinkIn(ptGame, hUnit, 12);
}

BOOL __fastcall AiThinkEmerge(GAME *ptGame, UNIT *hUnit)
{
	BOOL bOut;
	hUnit->dwFlags0 |= 14;
	if (D2CommonPathGetCollisionPatternByUnit(hUnit) != 1) {
		if (!SUnitMoveToRoom(ptGame, hUnit, (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit), (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), FALSE, FALSE))
			bOut = FALSE;
		else {
			D2CommonCollisnSetPattern(D2CommonUnitGetPathRoom(hUnit), (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), 1, 256);
			D2CommonPathSetCollisionPatternByUnit(hUnit, 1);
			D2CommonPathSetCollisionTestMask(hUnit->ptPath, 7169);
			AiThinkIn(ptGame, hUnit, 12);
			bOut = TRUE;
		}
	} else
		bOut = FALSE;
	return bOut;
}

UNIT * __fastcall AiThinkFindHurtUnit(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind)
{
	double dLife;
	int nDist;

	if ((hCandidate->eType == UNIT_MONSTER || hCandidate->eType == UNIT_PLAYER) && hUnit != hCandidate)
	{
		dLife = D2CommonStatsUnitGetValue(hCandidate, 6);
		if (!(D2CommonStatsUnitCalcMaxHp(hCandidate) * 0.75 < dLife) && hCandidate->dwMode
			&& hCandidate->dwMode != MONMODE_DEAD)
		{
			nDist = AiThinkDistance(hUnit, hCandidate);
			if (nDist < ptFind->tUnit.nDist)
			{
				ptFind->tUnit.hUnit = hCandidate;
				ptFind->tUnit.nDist = nDist;
			}
		}
	}
	return NULL;
}

void __fastcall AiThinkDuneBeast(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	BOOL bInReach;
	int nDist;
	AIGENERAL *ptAi;
	MONSTATSTXT *ptMonStats;
	int nLife;
	int nVel;
	int nStat;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptSeed = &hUnit->tSeed;
	nDist = ptParms->nDist;
	bInReach = ptParms->fTargetInReach;
	ptAi = ptParms->ptAi;
	ptMonStats = ptParms->ptMonStats;
	nLife = D2CommonUnitCalcLifePercent(hUnit);
	if ((DWORD)ptMonStats->nVelocity > 0) nVel = (int)(ptMonStats->Run * 100 / (DWORD)ptMonStats->nVelocity); else nVel = 0;
	if (nVel < 100) nVel = 0;
	else
	{
		nVel -= 100;
		if (nVel > 120)
			nVel = 120;
	}
	if (ptAi->adwParam[0])
	{
		ptAi->adwParam[1] = 0;
		ptAi->adwParam[0]--;
		if (!ptAi->adwParam[0] || nLife > 75)
		{
			D2CommonStatsUnitSetValue(hUnit, 74,
				(DWORD)D2CommonStatsUnitGetValue(hUnit, 74) - ptAi->adwParam[2]);
			if (nLife > 75)
			{
				ptAi->adwParam[0] = 0;
				AiSetVelocity(hUnit, 13, nVel, 0);
				AiChargeTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
				return;
			}
		}
		if (bInReach && D2CommonSeedRand(ptSeed, 100) < 33)
		{
			if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			else
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
			return;
		}
		if (nDist >= 24 && !MonsterTestPrevModeGetHit(hUnit))
		{
			if (D2CommonSeedRand(ptSeed, 100) < 33)
				AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
			AiStand(ptGame, hUnit, 20);
			return;
		}
		AiSetVelocity(hUnit, 2, nVel, 0);
		if (AiWalkAway(ptGame, hUnit, hTarget, 15, TRUE))
			return;
		if (bInReach)
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		else
			AiStepAside(ptGame, hUnit, 5);
		return;
	}
	if (!bInReach)
	{
		AiChargeTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
		return;
	}
	if (MonsterTestPrevModeGetHit(hUnit))
	{
		if (nLife < 33 && D2CommonSeedRand(ptSeed, 100) < 50)
		{
			ptAi->adwParam[0] = D2CommonSeedRand(ptSeed, 5) + 1;
			nStat = D2CommonStatsUnitGetValue(hUnit, 74);
			if (nStat)
				ptAi->adwParam[2] = nStat >> 3;
			else
				ptAi->adwParam[2] = 0;
			D2CommonStatsUnitSetValue(hUnit, 74, ptAi->adwParam[2] + nStat);
			AiSetVelocity(hUnit, 2, nVel, 0);
			AiWalkAway(ptGame, hUnit, hTarget, 15, FALSE);
			return;
		}
		if (ptAi->adwParam[1] && D2CommonSeedRand(ptSeed, 100) < 20)
		{
			AiWalkToTargetFast(ptGame, hUnit, hTarget, 3, FALSE);
			ptAi->adwParam[1] = 0;
			return;
		}
	}
	if (ptAi->adwParam[1] && D2CommonSeedRand(ptSeed, 100) >= ptMonStats->AIParam[0])
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2])
		{
			AiWalkToTargetFast(ptGame, hUnit, hTarget, 3, FALSE);
			ptAi->adwParam[1] = 0;
		}
		AiStand(ptGame, hUnit, 15);
		return;
	}
	ptAi->adwParam[1] = 1;
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
	else
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
}

void __fastcall AiThinkFollowOwner(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{

	EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
	if (ptParms->ptAi->adwParam[0])
	{
		AIGENERAL *ptAi;
		UNIT *hOwner;
		UNIT *hFound;
		UNIT *hNear;
		int nOwnerDist;
		DWORD dwOwnerDir;
		int nNearDist;
		DWORD dwNearDir;

		hOwner = AiGetOwner(hUnit);
		if (!hOwner)
			AiStand(ptGame, hUnit, 10);
		else
		{
			D2SEED *ptSeed;

			ptSeed = &hUnit->tSeed;
			AiThinkDistance(hUnit, hOwner);
			ptAi = ptParms->ptAi;
			hFound = AiFindTarget(ptGame, hOwner, ptAi, &nOwnerDist, &dwOwnerDir);
			if (hFound && (hFound->dwFlags0 & 1073741824))
				hFound = NULL;
			hNear = AiFindTargetInSight(ptGame, hUnit, &nNearDist, &dwNearDir);
			if (!hNear || nNearDist > 15)
			{
				hNear = NULL;
				if (hFound && !D2CommonUnitTestLineBlocked(hUnit, hFound, 4))
				{
					nNearDist = AiThinkDistance(hUnit, hFound);
					if (nNearDist < 20)
						hNear = hFound;
				}
			}
			if (AiThinkMoveToOwner(ptGame, hUnit, hNear, hOwner, dwNearDir, ptParms))
				return;
			if (D2CommonSeedRand(ptSeed, 100) < 80)
			{
				SkillsSetLevel(hUnit, SKILL_HIREABLEMISSILE, ptAi->adwParam[1], 0);
				AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK1, SKILL_HIREABLEMISSILE, hNear, 0, 0);
				return;
			}
			if (D2CommonSeedRand(ptSeed, 100) < 75)
			{
				AiStand(ptGame, hUnit, 10);
				return;
			}
			AiWalkToTargetFast(ptGame, hUnit, hNear, 3, FALSE);
		}
	}
	else
	{
		AIGENERAL *ptAi;
		UNIT *hOwner;
		UNIT *hFound;
		UNIT *hNear;
		int nOwnerDist;
		DWORD dwOwnerDir;
		int nNearDist;
		DWORD dwNearDir;
		D2SEED *ptSeed;

		ptSeed = &hUnit->tSeed;
		hOwner = AiGetOwner(hUnit);
		if (!hOwner)
		{
			AiStand(ptGame, hUnit, 10);
			return;
		}
		AiThinkDistance(hUnit, hOwner);
		ptAi = ptParms->ptAi;
		hFound = AiFindTarget(ptGame, hOwner, ptAi, &nOwnerDist, &dwOwnerDir);
		if (hFound && (hFound->dwFlags0 & 1073741824))
			hFound = NULL;
		hNear = AiTryWithin(ptGame, hUnit, ptAi, &nNearDist, &dwNearDir, 24);
		if (!hNear || nNearDist > 6)
		{
			hNear = NULL;
			if (hFound)
			{
				nNearDist = AiThinkDistance(hUnit, hFound);
				if (nNearDist < 36)
					hNear = hFound;
			}
		}
		if (!AiThinkSees(hUnit, hNear))
			hNear = NULL;
		if (AiThinkMoveToOwner(ptGame, hUnit, hNear, hOwner, dwNearDir, ptParms))
			return;
		if (dwNearDir)
		{
			if (D2CommonSeedRand(ptSeed, 100) < 80)
			{
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hNear);
				return;
			}
			AiStand(ptGame, hUnit, 10);
			return;
		}
		AiSetVelocity(hUnit, 0, 0, 12);
		AiChargeTarget(ptGame, hUnit, hNear, AIMOVE_ALL);
	}
}

BOOL __fastcall AiThinkMoveToOwner(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, UNIT *hOwner,
	DWORD dwInReach, AIPARMS *ptParms)
{
	void *ptPath;
	int nOwnerDist;
	int eOwnerMode;
	int nRange;
	int nOwnerLogic;
	int nUnitLogic;
	int nPathX;
	int nPathY;
	int nTargetX;
	int nTargetY;
	int nCase;
	BYTE bDir;
	BYTE abOpposite[8];

	int nStep;
	int nX;
	int nY;
	void *hRoom;
	void *hNewRoom;

	ptPath = hOwner->ptPath;
	nOwnerDist = AiThinkDistance(hUnit, hOwner);
	eOwnerMode = (int)hOwner->dwMode;
	nRange = (PlayerPetsCount(hOwner) >> 1) + 6;
	if (nRange > 25)
		nRange = 25;
	if (nOwnerDist <= 1 && eOwnerMode == PLRMODE_NEUTRAL && !dwInReach)
	{
		AiSetVelocity(hUnit, 7, 0, 0);
		if (AiWalkAround(ptGame, hUnit, hOwner, (BYTE)nRange))
			return TRUE;
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
		if (AiWalkAway(ptGame, hUnit, hOwner, nRange, TRUE))
			return TRUE;
		AiSetVelocity(hUnit, 0, 0, 40);
		AiWalkToPoint(ptGame, hUnit, (int)D2CommonPathGetEndXRaw((DWORD)ptPath), (int)D2CommonPathGetEndYRaw((DWORD)ptPath));
		return TRUE;
	}
	if (hTarget && !D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hUnit)))
		return FALSE;
	nOwnerLogic = D2CommonDungeonGetLogicValue(D2CommonUnitGetPathRoom(hOwner), D2CommonUnitGetX(hOwner), D2CommonUnitGetY(hOwner));
	nUnitLogic = D2CommonDungeonGetLogicValue(D2CommonUnitGetPathRoom(hUnit), D2CommonUnitGetX(hUnit), D2CommonUnitGetY(hUnit));
	nPathX = (int)D2CommonPathGetTargetXRaw((DWORD)ptPath);
	nPathY = (int)D2CommonPathGetTargetYRaw((DWORD)ptPath);
	nTargetX = (int)D2CommonPathGetEndXRaw((DWORD)ptPath);
	nTargetY = (int)D2CommonPathGetEndYRaw((DWORD)ptPath);
	nCase = 2;
	if (eOwnerMode == PLRMODE_WALK || eOwnerMode == PLRMODE_RUN)
		nCase = 0;
	if (nPathX != nTargetX && nPathY != nTargetY)
		nCase = 0;
	if (nOwnerLogic != nUnitLogic)
		nCase = 1;
	if (nOwnerDist > nRange)
		nCase = 1;
	if (nOwnerDist > 100)
		nCase = 3;
	switch (nCase)
	{ int nDX; int nDY;
	case 0:
		bDir = (BYTE)D2CommonUnitSetPositionExtra(hOwner, nTargetX, nTargetY);
		bDir = D2CommonMonsterRegionLookupTableB(bDir);
		abOpposite[0] = 4;
		abOpposite[1] = 3;
		abOpposite[2] = 2;
		abOpposite[3] = 1;
		abOpposite[4] = 0;
		abOpposite[5] = 7;
		abOpposite[6] = 6;
		abOpposite[7] = 5;
		D2CommonMonsterRegionGetOffsetPair(abOpposite[bDir], &nDX, &nDY);
		nX = nTargetX + nDX * 8;
		nY = nTargetY + nDY * 8;
		AiSetVelocity(hUnit, 0, 0, 40);
		if (AiWalkToPoint(ptGame, hUnit, nX, nY))
			return TRUE;
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
		AiStepAside(ptGame, hUnit, 4);
		return TRUE;
	case 1:
		if (eOwnerMode == PLRMODE_WALK)
		{
			AiSetVelocity(hUnit, 0, 0, 100);
			AiWalkToPoint(ptGame, hUnit, nTargetX, nTargetY);
			return TRUE;
		}
		AiSetVelocity(hUnit, 0, 50, 100);
		if (AiWalkToTargetStep(ptGame, hUnit, hOwner, (BYTE)nRange))
			return TRUE;
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
		AiSetVelocity(hUnit, 15, 50, 100);
		if (AiWalkToTargetStep(ptGame, hUnit, hOwner, (BYTE)nRange))
			return TRUE;
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
		nStep = (int)((DWORD)AiThinkDistance(hUnit, hOwner) >> 2);
		if (nStep < 4)
			nStep = 4;
		AiSetVelocity(hUnit, 0, 15, 0);
		AiStepAside(ptGame, hUnit, (BYTE)nStep);
		return TRUE;
	case 2:
		if (D2CommonSeedRand(&hUnit->tSeed, 100) < 10)
		{
			if (AiStepAside(ptGame, hUnit, 4))
				return TRUE;
			EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
			AiSetVelocity(hUnit, 0, 0, 40);
			AiWalkToPoint(ptGame, hUnit, (int)D2CommonPathGetEndXRaw((DWORD)ptPath), (int)D2CommonPathGetEndYRaw((DWORD)ptPath));
			return TRUE;
		}
		AiStand(ptGame, hUnit, 15);
		return TRUE;
	case 3:
	{
		int nSpawnX;
		int nSpawnY;
		DRLGLOGICNODE *ptNode;

		hRoom = (void *)D2CommonUnitGetPathRoom(hOwner);
		ptNode = (DRLGLOGICNODE *)D2CommonDungeonRoomGetLogicNode(hRoom, D2CommonUnitGetX(hOwner), D2CommonUnitGetY(hOwner));
		if (!MonsterFindSpawnPoint(ptGame, (GAMEROOM *)hRoom, ptNode, 363, &nSpawnX, &nSpawnY, FALSE))
			return FALSE;
		hNewRoom = D2CommonDungeonFindRoomGame(hRoom, nSpawnX, nSpawnY);
		if (!hNewRoom || !SUnitMoveToRoom(ptGame, hUnit, (GAMEROOM *)hNewRoom, nSpawnX, nSpawnY, FALSE, FALSE))
			return FALSE;
		D2CommonUnitRoomAddToChangedList(hUnit);
		hUnit->dwFlags |= 65536;
		AiStand(ptGame, hUnit, 5);
		return TRUE;
	}
	}
	return FALSE;
}

void __fastcall AiThinkArach(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	int nDist;
	BOOL bInReach;
	MONSTATSTXT *ptMonStats;
	AIGENERAL *ptAi;
	int nLife;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptSeed = &hUnit->tSeed;
	nDist = ptParms->nDist;
	bInReach = ptParms->fTargetInReach;
	ptMonStats = ptParms->ptMonStats;
	ptAi = ptParms->ptAi;
	nLife = D2CommonUnitCalcLifePercent(hUnit);
	if (ptAi->adwParam[0] == 1)
	{
		ptAi->adwParam[1] = 0;
		if (nLife > ptMonStats->AIParam[3])
		{
			ptAi->adwParam[0] = 0;
			if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2])
			{
				ptAi->adwParam[0] = 2;
				AiChargeTarget(ptGame, hUnit, hTarget, 0);
				return;
			}
			AiWalkToTargetFast(ptGame, hUnit, hTarget, 6, FALSE);
			return;
		}
		if (bInReach
			&& D2CommonSeedRand(ptSeed, 100) < (DWORD)(ptMonStats->AIParam[0] - 25))
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		if (nDist >= 8 && !MonsterTestPrevModeGetHit(hUnit))
		{
			ptAi->adwParam[0] = 0;
			AiWalkToTargetFast(ptGame, hUnit, hTarget, 12, FALSE);
			return;
		}
		AiWalkAway(ptGame, hUnit, hTarget, 4, FALSE);
		return;
	}
	if (bInReach)
	{
		ptAi->adwParam[0] = 2;
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		if (nLife < ptMonStats->AIParam[4])
		{
			ptAi->adwParam[0] = 1;
			if (!D2CommonStatsGetState(hUnit, 22))
			{
				AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK2, SKILL_SPIDERLAY, NULL, 0, 0);
				return;
			}
			AiWalkAway(ptGame, hUnit, hTarget, 8, FALSE);
			return;
		}
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2])
		{
			AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
			return;
		}
		AiStand(ptGame, hUnit, 15);
		return;
	}
	if (!MonsterTestPrevModeGetHit(hUnit) && ptAi->adwParam[1] != 1)
	{
		ptAi->adwParam[1] = 0;
		if (ptAi->adwParam[2] || D2CommonSeedRand(ptSeed, 100) >= ptMonStats->AIParam[2])
		{
			if (D2CommonSeedRand(ptSeed, 100) < 20)
			{
				AiStepAside(ptGame, hUnit, 6);
				return;
			}
			AiStand(ptGame, hUnit, 15);
			return;
		}
	}
	ptAi->adwParam[1] = 1;
	AiChargeTarget(ptGame, hUnit, hTarget, 0);
}

void __fastcall AiThinkSwampDwellerOut(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	int nX;
	int nY;

	ptAi = ptParms->ptAi;
	if ((int)ptAi->adwParam[2] >= 2)
	{
		AiUseSkill(ptGame, hUnit, SKILL_SUBMERGE, ptParms->hTarget, 0, 0);
		ptAi->adwParam[2] = 1;
		hUnit->dwFlags0 &= ~14;
		D2CommonCollisnClearPoint(D2CommonUnitGetPathRoom(hUnit), (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), 4096);
		D2CommonCollisnClearPoint(D2CommonUnitGetPathRoom(hUnit), (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), 256);
		D2CommonPathSetCollisionPatternByUnit(hUnit, 3);
		D2CommonPathSetCollisionTestMask(hUnit->ptPath, 0);
		AiThinkIn(ptGame, hUnit, 12);
		return;
	}
	AiThinkSet(ptGame, hUnit, ptAi, ptAi->nThinkKind);
	AiThinkIn(ptGame, hUnit, 1);
}

void __fastcall AiThinkSwampDweller(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	UNIT *hTarget;
	BOOL bInReach;
	int nDist;
	MONSTATSTXT *ptMonStats;
	int nRoll;
	int nX;
	int nY;

	ptAi = ptParms->ptAi;
	hTarget = ptParms->hTarget;
	bInReach = ptParms->fTargetInReach;
	nDist = ptParms->nDist;
	if (!ptAi->adwParam[2])
	{
		if (nDist > 32)
		{
			AiUseSkill(ptGame, hUnit, SKILL_SUBMERGE, hTarget, 0, 0);
			AiThinkIn(ptGame, hUnit, 8);
			ptAi->adwParam[2] = 1;
			hUnit->dwFlags0 &= ~14;
			D2CommonCollisnClearPoint(D2CommonUnitGetPathRoom(hUnit), (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit), 4096);

			D2CommonCollisnClearPoint(D2CommonUnitGetPathRoom(hUnit), ((int)D2CommonUnitGetX(hUnit)), ((int)D2CommonUnitGetY(hUnit)), 256);
			D2CommonPathSetCollisionPatternByUnit(hUnit, 3);
			D2CommonPathSetCollisionTestMask(hUnit->ptPath, 0);
			AiThinkIn(ptGame, hUnit, 12);
			return;
		}
		if (AiThinkEmerge(ptGame, hUnit))
		{
			AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_EMERGE, hUnit, 0, 0);
			ptAi->adwParam[2] = 2;
		}
		else
			AiStand(ptGame, hUnit, 8);
	}
	else if (ptAi->adwParam[2] == 1)
	{
		if ((nDist < 9 && AiThinkEmerge(ptGame, hUnit))
			|| (nDist < 20 && (int)ptAi->adwParam[1] > 64 && AiThinkEmerge(ptGame, hUnit)))
		{
			AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_EMERGE, hUnit, 0, 0);
			ptAi->adwParam[2] = 2;
			return;
		}
		hUnit->dwFlags0 &= ~14;

		D2CommonCollisnClearPoint(D2CommonUnitGetPathRoom(hUnit), ((int)D2CommonUnitGetX(hUnit)), ((int)D2CommonUnitGetY(hUnit)), 4096);

		D2CommonCollisnClearPoint(D2CommonUnitGetPathRoom(hUnit), ((int)D2CommonUnitGetX(hUnit)), ((int)D2CommonUnitGetY(hUnit)), 256);
		D2CommonPathSetCollisionPatternByUnit(hUnit, 3);
		D2CommonPathSetCollisionTestMask(hUnit->ptPath, 0);
		AiThinkIn(ptGame, hUnit, 12);
		AiThinkIn(ptGame, hUnit, 24);
		ptAi->adwParam[2] = 1;
		ptAi->adwParam[1]++;
		return;
	}
	else
	{
		if (!hTarget)
		{
			AiThinkIn(ptGame, hUnit, 32);
			return;
		}
		ptMonStats = ptParms->ptMonStats;
		nRoll = (int)D2CommonSeedRand(&hUnit->tSeed, 100);
		if (bInReach)
		{
			if (nRoll < ptMonStats->AIParam[0])
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			else if (nRoll < ptMonStats->AIParam[0] + 20)
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
			else if (nRoll < ptMonStats->AIParam[0] + 25)
				AiWalkToTargetFast(ptGame, hUnit, hTarget, 3, FALSE);
			else if (nRoll < ptMonStats->AIParam[0] + 30)
				AiWalkToTarget(ptGame, hUnit, hTarget, 0);
			else
				AiThinkIn(ptGame, hUnit, 25);
			return;
		}
		if (nDist < 12 && nRoll < ptMonStats->AIParam[1])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
			return;
		}
		if (nRoll < 66)
		{
			AiWalkToTargetStep(ptGame, hUnit, hTarget, 4);
			return;
		}
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
	}
}

void __fastcall AiThinkGhoulLord(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	UNIT *hTarget;
	int nDist;
	MONSTATSTXT *ptMonStats;
	BOOL bInReach;
	int nVel;
	int nSkills;
	int fSkill1;
	int fSkill2;
	int fSkill4;
	UNIT *hNear;
	int nNearDist;
	DWORD dwNearDir;
	int nLife;
	int nMax;
	int nRoll;
	D2SEED *ptSeed;

	ptAi = ptParms->ptAi;
	hTarget = ptParms->hTarget;
	if (!hTarget)
	{
		ptAi->adwParam[2] = 0;
		AiStand(ptGame, hUnit, 10);
		return;
	}
	nDist = ptParms->nDist;
	ptMonStats = ptParms->ptMonStats;
	bInReach = ptParms->fTargetInReach;
	ptSeed = &hUnit->tSeed;
	if ((DWORD)ptMonStats->nVelocity > 0)
		nVel = (int)(ptMonStats->Run * 100 / (DWORD)ptMonStats->nVelocity);
	else
		nVel = 0;
	if (nVel < 100)
		nVel = 0;
	else
	{
		nVel -= 100;
		if (nVel > 120)
			nVel = 120;
	}

	nSkills = ptMonStats->AIParam[4];
	fSkill1 = nSkills & 1;
	fSkill2 = nSkills & 2;
	fSkill4 = nSkills & 4;
	hNear = AiFindTargetInSight(ptGame, hUnit, &nNearDist, &dwNearDir);
	if (MonsterTestPrevModeGetHit(hUnit))
	{
		ptAi->adwParam[2] = 1;
		if (nDist < 20 && nDist > (int)ptAi->adwParam[1])
			ptAi->adwParam[1] = nDist;
		if (bInReach)
		{
			if (D2CommonSeedRand(ptSeed, 100) <= 30 && fSkill1)
			{
				if (D2CommonSeedRand(ptSeed, 100) < 50)
				{
					AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_VAMPIREFIREBALL, hTarget, 0, 0);
				}
				else
				{
					SkillsSetLevel(hUnit, SKILL_MISSILESKILL1, 277, 0);
					AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_MISSILESKILL1, hTarget, 0, 0);
				}
			}
			else AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
	}
	nLife = D2CommonUnitCalcLifePercent(hUnit);
	if (ptAi->adwParam[0] == 1)
	{
		if (nLife > 75)
		{
			ptAi->adwParam[0] = 0;
			AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
			return;
		}
		if (nDist < 14 || nDist <= (int)ptAi->adwParam[1])
		{
			AiSetVelocity(hUnit, 0, nVel, 0);
			if (AiWalkAway(ptGame, hUnit, hTarget, 8, TRUE))
				return;
		}
		if (nDist < ptMonStats->AIParam[2]
			&& D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
		{
			if (!(D2CommonSeedRand(ptSeed, 100) <= 20 && fSkill2) && nNearDist <= 20 && fSkill1)
			{
				if (!hNear)
				{
					AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
					return;
				}
				if (D2CommonSeedRand(ptSeed, 100) < 50)
				{
					AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_VAMPIREFIREBALL, hNear, 0, 0);
					return;
				}
				SkillsSetLevel(hUnit, SKILL_MISSILESKILL1, 277, 0);
				AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_MISSILESKILL1, hNear, 0, 0);
				return;
			}
			if (fSkill2)
			{
				AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_VAMPIREFIREWALL, hTarget, 0, 0);
				return;
			}
		}
	}
	else
	{
		ptAi->adwParam[0] = 0;
		if (nLife < 33)
		{
			ptAi->adwParam[0] = 1;
			AiWalkAway(ptGame, hUnit, hTarget, 8, FALSE);
			return;
		}
		if (bInReach)
		{
			ptAi->adwParam[2] = 1;
			if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
			{
				if (!(D2CommonSeedRand(ptSeed, 100) <= 30 && fSkill1))
				{
					AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
					return;
				}
				if (hNear)
				{
					if (D2CommonSeedRand(ptSeed, 100) < 50)
					{
						AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_VAMPIREFIREBALL, hNear, 0, 0);
						return;
					}
					SkillsSetLevel(hUnit, SKILL_MISSILESKILL1, 277, 0);
					AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_MISSILESKILL1, hNear, 0, 0);
					return;
				}
			}
			if (D2CommonSeedRand(ptSeed, 100) < 33)
			{
				AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
				return;
			}
			AiStand(ptGame, hUnit, 15);
			return;
		}
		if (nDist < ptMonStats->AIParam[2])
		{
			ptAi->adwParam[2] = 1;
			if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
			{
				nMax = 0;
				if (fSkill1)
					nMax = 50;
				if (fSkill2)
					nMax += 25;
				if (fSkill4)
					nMax += 25;
				if (nMax)
					nRoll = (int)D2CommonSeedRand(ptSeed, nMax);
				else
					nRoll = 0;
				if (!D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hUnit))))
					nRoll = 0;
				if (nRoll < 50 && nNearDist <= 20 && fSkill1)
				{
					if (hNear)
					{
						if (D2CommonSeedRand(ptSeed, 100) < 50)
							AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_VAMPIREFIREBALL, hNear, 0, 0);
						else
						{
							SkillsSetLevel(hUnit, SKILL_MISSILESKILL1, 277, 0);
							AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_MISSILESKILL1, hNear, 0, 0);
						}
					}
					else AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
					return;
				}
				if (nRoll < 75 && fSkill2)
				{
					AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_VAMPIREFIREWALL, hTarget, 0, 0);
					return;
				}
				if (fSkill4)
				{
					AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_VAMPIREMETEOR, hTarget, 0, 0);
					return;
				}
				AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
				return;
			}
			if (nDist > 10)
			{
				AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
				return;
			}
			if (D2CommonSeedRand(ptSeed, 100) < 33)
			{
				AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
				return;
			}
			if (nDist < 5 && D2CommonSeedRand(ptSeed, 100) < 50)
			{
				AiWalkAway(ptGame, hUnit, hTarget, 8, FALSE);
				return;
			}
			AiStand(ptGame, hUnit, 10);
			return;
		}
		if (ptAi->adwParam[2])
		{
			AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
			return;
		}
	}
	AiStand(ptGame, hUnit, 15);
}

void __fastcall AiThinkZakarumite(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	UNIT *hLeader;
	DWORD dwOwnerType;
	MONSTATSTXT *ptMonStats;
	int nVel;
	PLRINFO *ptInfo;
	BOOL bInReach;
	AIGENERAL *ptAi;
	int nLife;
	int nX;
	int nY;
	MONSTERSCALE tScale;
	DWORD dwChance;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptSeed = &hUnit->tSeed;
	hLeader = hTarget;
	if (hTarget->eType == UNIT_MONSTER)
	{ DWORD dwOwnerId;
		AiGetOwnerId(hTarget, &dwOwnerId, &dwOwnerType);
		if (dwOwnerType == UNIT_PLAYER)
			hLeader = SUnitFind(ptGame, UNIT_PLAYER, dwOwnerId);
	}
	ptMonStats = ptParms->ptMonStats;
	if ((DWORD)ptMonStats->nVelocity > 0)
		nVel = (int)(ptMonStats->Run * 100 / (DWORD)ptMonStats->nVelocity);
	else
		nVel = 0;
	if (nVel < 100)
		nVel = 0;
	else
	{
		nVel -= 100;
		if (nVel > 120)
			nVel = 120;
	}

	if (hLeader->eType == UNIT_PLAYER)
	{
		ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hLeader);
		if (D2CommonStepTestGridBit(ptInfo->apQuestRecords[ptGame->bDifficultyLevel], 21, 0))
		{
			AiSetVelocity(hUnit, 0, nVel, 0);
			if (AiRunAway(ptGame, hUnit, hTarget, 8, TRUE))
				return;
			AiStepAside(ptGame, hUnit, 6);
			return;
		}
	}
	bInReach = ptParms->fTargetInReach;
	ptAi = ptParms->ptAi;
	nLife = D2CommonUnitCalcLifePercent(hUnit);
	if (MonsterTestPrevModeGetHit(hUnit))
	{
		if (!ptAi->adwParam[1] && nLife < ptMonStats->AIParam[2])
		{
			ptAi->adwParam[0] = 0;
			AiSetVelocity(hUnit, 0, nVel, 0);
			if (AiRunAway(ptGame, hUnit, hTarget, 8, TRUE))
				return;
			ptAi->adwParam[1] = 5;
		}
		if (D2CommonCollisnTestPoint(D2CommonUnitGetPathRoom(hUnit), ((int)D2CommonUnitGetX(hUnit)), ((int)D2CommonUnitGetY(hUnit)), 64))
		{
			if (bInReach)
				AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
			else
				AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
			return;
		}
	}
	if (ptAi->adwParam[1])
		ptAi->adwParam[1]--;
	if (bInReach)
	{
		if (ptAi->adwParam[0] && D2CommonSeedRand(ptSeed, 100) >= ptMonStats->AIParam[0])
		{
			if (D2CommonSeedRand(ptSeed, 100) < 80)
				AiStand(ptGame, hUnit, 10);
			else
				AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
			return;
		}
		ptAi->adwParam[0] = 1;
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
		else
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	MonsterGetScale(&tScale, ptGame, D2CommonUnitGetPathRoom(hUnit), (DWORD)hUnit);
	dwChance = (DWORD)(ptMonStats->AIParam[3] + tScale.nDifficulty * 10);
	ptAi->adwParam[0] = 0;
	if (D2CommonSeedRand(ptSeed, 100) < dwChance)
	{
		AiSetVelocity(hUnit, 0, nVel, 0);
		AiRunToTarget(ptGame, hUnit, hTarget);
		return;
	}
	AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
}

void __fastcall AiThinkSexton(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	BOOL bInReach;
	AIGENERAL *ptAi;
	MONSTATSTXT *ptMonStats;
	int nLife;
	int nX;
	int nY;
	int nTargetX;
	int nTargetY;
	int nShift;
	DWORD dwChance;
	AIFINDRECORD tFind;
	UNIT *hFound;

	hTarget = ptParms->hTarget;
	bInReach = ptParms->fTargetInReach;
	ptAi = ptParms->ptAi;
	ptMonStats = ptParms->ptMonStats;
	nLife = D2CommonUnitCalcLifePercent(hUnit);
	if ((MonsterTestPrevModeGetHit(hUnit) || bInReach) && nLife < 33 && !ptAi->adwParam[0])
	{
		ptAi->adwParam[0] = 1;
		nX = (int)D2CommonUnitGetX(hUnit);
		nY = (int)D2CommonUnitGetY(hUnit);
		nTargetX = (int)D2CommonUnitGetX(hTarget);
		nTargetY = (int)D2CommonUnitGetY(hTarget);
		if (bInReach)
			nShift = 2;
		else
			nShift = 0;
		nTargetX += (nTargetX - nX) << nShift;
		nTargetY += (nTargetY - nY) << nShift;
		if (MonsterCanUseSkill(ptGame, hUnit, 184, NULL, nTargetX, nTargetY))
		{
			AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_MONTELEPORT, NULL, nTargetX, nTargetY);
			return;
		}
	}
	ptAi->adwParam[0] = 0;
	dwChance = ptMonStats->AIParam[0];
	if (nLife > 0 && nLife < 100)
		dwChance += (100 - nLife) >> 3;
	if (bInReach)
	{
		if (D2CommonSeedRand(&hUnit->tSeed, 100) < dwChance)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
		return;
	}
	tFind.tHurt.hFound = NULL;
	tFind.tHurt.nFound = 0;
	AiFind(ptGame, hUnit, &tFind, AiThinkFindHurtZakarum, 1);
	hFound = tFind.tHurt.hFound;
	if (hFound && D2CommonSeedRand(&hUnit->tSeed, 100) < 25)
	{
		AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_ZAKARUMHEAL, hFound, 0, 0);
		return;
	}
	if (UnitLineClear(hUnit, hTarget))
	{
		if (D2CommonSeedRand(&hUnit->tSeed, 100) < dwChance)
		{
			if (!ptAi->adwParam[2]
				&& D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[1])
			{
				AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_MONBLIZZARD, hTarget, 0, 0);
				ptAi->adwParam[2] = 1;
				return;
			}
			ptAi->adwParam[2] = 0;
			if (D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[2])
			{
				SkillsSetLevel(hUnit, SKILL_MISSILESKILL1, 170, 0);
				AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_MISSILESKILL1, hTarget, 0, 0);
				return;
			}
			AiStand(ptGame, hUnit, 12);
			return;
		}
	}
	else if (D2CommonSeedRand(&hUnit->tSeed, 100) < dwChance
		&& D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[1])
	{
		AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_MONBLIZZARD, hTarget, 0, 0);
		return;
	}
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < 30)
	{
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
		return;
	}
	AiStand(ptGame, hUnit, 20);
}

UNIT * __fastcall AiThinkFindHurtZakarum(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind)
{
	DWORD dwLife;

	if (hUnit != hCandidate && hCandidate->eType == UNIT_MONSTER
		&& D2CommonStatsUnitsTestSameAlignment(hUnit, hCandidate) && D2CommonStatsUnitGetAlignment(hCandidate)
		&& hCandidate->dwClassId >= 235 && hCandidate->dwClassId <= 241
		&& hCandidate->dwMode && hCandidate->dwMode != MONMODE_DEAD
		&& D2CommonUnitCalcLifePercent(hCandidate) <= 60)
	{

		if ((DWORD)D2CommonStatsUnitCalcMaxHp(hCandidate) != D2CommonStatsUnitGetValue(hCandidate, 6)
			&& (DWORD)AiThinkDistance(hUnit, hCandidate) < 36)
		{
			ptFind->tHurt.hFound = hCandidate;
			ptFind->tHurt.nFound++;
		}
	}
	return NULL;
}

void __fastcall AiThinkCouncilMember(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	MONSTATSTXT *ptMonStats;
	BOOL bInReach;
	UNIT *hTarget;
	int nLife;
	int nRoll;
	int nX;
	int nY;
	AIFINDRECORD tFind;
	D2SEED *ptSeed;

	ptAi = ptParms->ptAi;
	ptMonStats = ptParms->ptMonStats;
	hTarget = ptParms->hTarget;
	bInReach = ptParms->fTargetInReach;
	nLife = D2CommonUnitCalcLifePercent(hUnit);
	MonsterTestPrevModeGetHit(hUnit);
	ptSeed = &hUnit->tSeed;
	ptMonStats->dwMissS1 = 343;
	if (ptAi->adwParam[0] == 2)
	{
		if (bInReach)
		{
			AiWalkAway(ptGame, hUnit, hTarget, 6, TRUE);
			return;
		}
		ptMonStats->dwMissS1 = 65535;
		nRoll = (int)D2CommonSeedRand(ptSeed, 100);
		if (nRoll < 40)
		{
			AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_HYDRA, hTarget, (int)D2CommonUnitGetX(hTarget) - 5, (int)D2CommonUnitGetY(hTarget) - 5);
			goto lRoam;
		}
		if (nRoll < 80)
		{
			tFind.tWeak.hFound = NULL;
			tFind.tWeak.nLife = 0;
			AiFind(ptGame, hUnit, &tFind, AiThinkFindWeakestFellow, 1);
			if (tFind.tWeak.hFound)
			{
				AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_ZAKARUMHEAL, tFind.tWeak.hFound, 0, 0);
				ptAi->adwParam[0] = 0;
				return;
			}
		}
		AiStand(ptGame, hUnit, 12);
		goto lRoam;
	}
	if (bInReach)
	{
		if (!ptAi->adwParam[0] && D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		if (ptAi->adwParam[0] == 1
			|| D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_SKILL1, hTarget);
			goto lRoam;
		}
	}
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1] && nLife < 99)
	{
		ptAi->adwParam[0] = 2;
		AiStand(ptGame, hUnit, 8);
		return;
	}
	if (nLife < ptMonStats->AIParam[3] && (int)ptAi->adwParam[1] < 5)
	{
		ptAi->adwParam[0] = 1;
		AiWalkAway(ptGame, hUnit, hTarget, 6, TRUE);
		AiThinkIn(ptGame, hUnit, 32);
		ptAi->adwParam[1]++;
		return;
	}
	if (bInReach)
	{
		ptAi->adwParam[0] = 1;
		AiStand(ptGame, hUnit, 12);
		return;
	}
	if (!ptAi->adwParam[0])
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
		return;
	}
	ptAi->adwParam[0] = 0;
	AiStepAside(ptGame, hUnit, 12);
	return;
lRoam:
	ptAi->adwParam[0] = 0;
}

UNIT * __fastcall AiThinkFindWeakestFellow(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind)
{
	int nLife;
	DWORD dwLife;

	if (hCandidate->eType != UNIT_MONSTER
		|| !D2CommonStatsUnitsTestSameAlignment(hUnit, hCandidate) || !D2CommonStatsUnitGetAlignment(hCandidate)
		|| !hCandidate->dwMode || hCandidate->dwMode == MONMODE_DEAD)
		return NULL;
	nLife = D2CommonUnitCalcLifePercent(hCandidate);
	if (nLife > 75)
		return NULL;
	if (ptFind->tWeak.nLife <= 0)
		ptFind->tWeak.nLife = 100;
	dwLife = D2CommonStatsUnitGetValue(hCandidate, 6);
	if ((DWORD)D2CommonStatsUnitCalcMaxHp(hCandidate) != dwLife
		&& (DWORD)AiThinkDistance(hUnit, hCandidate) < 30 && ptFind->tWeak.nLife > nLife)
	{
		ptFind->tWeak.nLife = nLife;
		ptFind->tWeak.hFound = hCandidate;
	}
	return NULL;
}

void __fastcall AiThinkRatManHitAndBackOff(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	UNIT *hTarget;
	int nDist;
	BOOL bInReach;
	int nTargetLife;
	AICOMMAND *ptCommand;
	UNIT *hGoal;
	D2SEED *ptSeed;

	ptAi = ptParms->ptAi;
	hTarget = ptParms->hTarget;
	if (!hTarget)
	{
		ptAi->adwParam[0] = (DWORD)ptParms->hTarget;
		AiStand(ptGame, hUnit, 10);
		return;
	}
	ptSeed = &hUnit->tSeed;
	nDist = ptParms->nDist;
	bInReach = ptParms->fTargetInReach;
	nTargetLife = D2CommonUnitCalcLifePercent(hTarget);
	ptCommand = AiGetCommand(hUnit);
	if (ptCommand)
	{
		if (ptCommand->nCommand == 1)
		{
			hGoal = SUnitFind(ptGame, ptCommand->dwArg2, ptCommand->dwArg1);
			if (hGoal)
			{
				ptAi->adwParam[0] = 0;
				ptAi->adwParam[1] = 0;
				AiSetVelocity(hUnit, 13, 50, 0);
				AiWalkToTarget(ptGame, hUnit, hGoal, 0);
				AiPopCommand(ptGame, hUnit);
				return;
			}
		}
		if (ptCommand->nCommand == 14)
		{
			hGoal = SUnitFind(ptGame, ptCommand->dwArg2, ptCommand->dwArg1);
			if (hGoal)
			{
				ptAi->adwParam[0] = 0;
				ptAi->adwParam[1] = 0;
				AiSetVelocity(hUnit, 13, 50, 0);
				AiWalkToTarget(ptGame, hUnit, hGoal, 0);
				AiPopCommand(ptGame, hUnit);
				return;
			}
		}
		AiPopCommand(ptGame, hUnit);
	}
	if (bInReach && ptAi->adwParam[0] != 2)
	{
		if (!ptAi->adwParam[0])
		{
			ptAi->adwParam[0] = 1;
			ptAi->adwParam[1] = 0;
		}
		ptAi->adwParam[1]++;
		if ((int)ptAi->adwParam[1] > 4 && nTargetLife > 33)
			ptAi->adwParam[0] = 2;
		else
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
	}
	switch (ptAi->adwParam[0])
	{
	case 0:
		AiSetVelocity(hUnit, 13, 50, 0);
		AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
		return;
	case 1:
		ptAi->adwParam[1]++;
		if ((int)ptAi->adwParam[1] > 4 && nTargetLife > 33)
			ptAi->adwParam[0] = 2;
	default:
		AiStand(ptGame, hUnit, 10);
		return;
	case 2:
		if (nDist > 12)
		{
			if (D2CommonSeedRand(ptSeed, 100) < 20)
			{
				AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
				return;
			}
		}
		else
		{
			AiSetVelocity(hUnit, 2, 50, 0);
			if (AiWalkAway(ptGame, hUnit, hTarget, 14, TRUE))
				return;
		}
		ptAi->adwParam[0] = 0;
		AiStand(ptGame, hUnit, 10);
		return;
	}
}

void __fastcall AiThinkRatManBackOffInSight(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	UNIT *hTarget;
	int nDist;
	BOOL bInReach;
	AICOMMAND *ptCommand;
	UNIT *hGoal;
	UNIT *hNear;
	int nNearDist;
	DWORD dwNearDir;

	ptAi = ptParms->ptAi;
	hTarget = ptParms->hTarget;
	if (!hTarget)
	{
		ptAi->adwParam[0] = 0;
		AiStand(ptGame, hUnit, 10);
		return;
	}
	nDist = ptParms->nDist;
	bInReach = ptParms->fTargetInReach;
	ptCommand = AiGetCommand(hUnit);
	if (ptCommand)
	{
		if (ptCommand->nCommand == 1)
		{
			hGoal = SUnitFind(ptGame, ptCommand->dwArg2, ptCommand->dwArg1);
			if (hGoal)
			{
				ptAi->adwParam[0] = 0;
				ptAi->adwParam[1] = 0;
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hGoal);
				AiPopCommand(ptGame, hUnit);
				return;
			}
		}
		if (ptCommand->nCommand == 14)
		{
			hGoal = SUnitFind(ptGame, ptCommand->dwArg2, ptCommand->dwArg1);
			if (hGoal)
			{
				ptAi->adwParam[0] = 0;
				ptAi->adwParam[1] = 0;
				AiSetVelocity(hUnit, 13, 50, 0);
				AiWalkToTarget(ptGame, hUnit, hGoal, 0);
				AiPopCommand(ptGame, hUnit);
				return;
			}
		}
		AiPopCommand(ptGame, hUnit);
	}
	if (!bInReach && nDist > 20)
	{
		AiSetVelocity(hUnit, 0, 50, 0);
		AiWalkAround(ptGame, hUnit, hTarget, 6);
		return;
	}
	hNear = AiFindTargetInSight(ptGame, hUnit, &nNearDist, &dwNearDir);
	if ((bInReach || nDist < 6) && D2CommonSeedRand(&hUnit->tSeed, 100) < 30)
		ptAi->adwParam[0] = 2;
	switch (ptAi->adwParam[0])
	{
	case 0:
		ptAi->adwParam[1]++;
		if ((int)ptAi->adwParam[1] > (int)D2CommonSeedRand(&hUnit->tSeed, 3) + 3)
		{
			ptAi->adwParam[0] = 1;
			ptAi->adwParam[1] = 0;
		}
		if (hNear)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hNear);
			return;
		}
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
		return;
	case 1:
		ptAi->adwParam[0] = 0;
		ptAi->adwParam[1] = 0;
		AiSetVelocity(hUnit, 0, 50, 0);
		if (AiWalkToTargetFast(ptGame, hUnit, hTarget, 6, TRUE))
			return;
		goto lStand;
	case 2:
		if (nDist > 12)
		{
			if (D2CommonSeedRand(&hUnit->tSeed, 100) < 20
				&& AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, TRUE))
				return;
			ptAi->adwParam[0] = 0;
			ptAi->adwParam[1] = 0;
			goto lStand;
		}
		AiSetVelocity(hUnit, 2, 50, 0);
		if (AiWalkAway(ptGame, hUnit, hTarget, 14, TRUE))
			return;
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hNear);
		ptAi->adwParam[0] = 0;
		ptAi->adwParam[1] = 0;
		return;
	default:
lStand:
		AiStand(ptGame, hUnit, 10);
		return;
	}
}

void __fastcall AiThinkRatManShaman(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	int nDist;
	MONSTATSTXT *ptMonStats;
	int nLevel;
	AIFINDRECORD tFind;
	UNIT *hDead;
	int nDead;

	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	ptMonStats = ptParms->ptMonStats;
	nLevel = (int)D2CommonSkillsCalcNodeLevel(hUnit, D2CommonSkillsFindByKey(hUnit, 178), 1);
	if (nLevel <= 0)
		nLevel = 1;
	if (nDist < nLevel && !D2CommonStatsGetState(hUnit, 12))
	{
		AICOMMAND tCommand;

		tCommand.nCommand = 1;
		tCommand.dwArg1 = hTarget->dwId;
		tCommand.dwArg2 = hTarget->eType;
		AiCommandMinions(ptGame, hUnit, &tCommand);
		AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK1, SKILL_FETISHINFERNO, hTarget, 0, 0);
		return;
	}
	if (D2CommonStatsGetState(hUnit, 12))
		D2CommonStatsUnitSetActiveStateBit(hUnit, 12, 0);
	tFind.tDead.hUnit = NULL;
	tFind.tDead.nCount = 0;
	tFind.tDead.nMax = 100;
	AiFind(ptGame, hUnit, &tFind, NULL, 9);
	hDead = tFind.tDead.hUnit;
	nDead = tFind.tDead.nCount;
	if (hDead
		&& D2CommonSeedRand(&hUnit->tSeed, 100)
			< (DWORD)(ptMonStats->AIParam[0] + nDead * 8)
		&& MonsterCanUseSkill(ptGame, hUnit, 181, hDead, 0, 0))
	{
		if (tFind.tDead.nDist > 15)
		{
			AiWalkAround(ptGame, hUnit, hDead, 10);
			return;
		}
		{
			AICOMMAND tCommand;

			tCommand.nCommand = 14;
			tCommand.dwArg1 = hDead->dwId;
			tCommand.dwArg2 = UNIT_MONSTER;
			AiCommandMinions(ptGame, hUnit, &tCommand);
			AiUseSkill(ptGame, hUnit, SKILL_RESURRECT, hDead, 0, 0);
			return;
		}
	}
	if (nDist > nLevel)
	{
		UNIT *hFound;

		tFind.nCount = 0;
		hFound = AiFind(ptGame, hUnit, &tFind, AiThinkCountFellows, 10);
		if (!tFind.nCount && D2CommonSeedRand(&hUnit->tSeed, 100) < 80)
		{
			AiWalkAround(ptGame, hUnit, hTarget, (BYTE)(nLevel - 1));
			return;
		}
		if (hFound && nDist < 20
			&& D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[1])
		{
			AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_FETISHAURA, hFound, 0, 0);
			return;
		}
	}
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < 50)
	{
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
		return;
	}
	AiStand(ptGame, hUnit, 10);
}

UNIT * __fastcall AiThinkCountFellows(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind)
{
	DWORD dwClass;

	dwClass = D2CommonMonsterUnitGetTypeLocalIndex(hCandidate);
	if ((dwClass == 141 || dwClass == 396) && (hCandidate->dwFlags0 & 2))
		ptFind->nCount++;
	if ((DWORD)AiThinkDistance(hUnit, hCandidate) > 26)
		return NULL;
	if (D2CommonStatsGetState(hCandidate, 25))
		return NULL;
	if (SUnitIsDead(hCandidate))
		return NULL;
	else
		return hCandidate;
}

void __fastcall AiThinkWaterWatcherLimb(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	UNIT *hTarget;
	int nDist;
	BOOL bInReach;
	MONSTATSTXT *ptMonStats;
	UNIT *hOwner;
	DWORD dwOwnerMode;

	ptAi = ptParms->ptAi;
	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	bInReach = ptParms->fTargetInReach;
	ptMonStats = ptParms->ptMonStats;
	hOwner = AiGetOwner(hUnit);
	if (!hOwner)
	{
		SUnitDmgKill(ptGame, hUnit, NULL, TRUE);
		return;
	}
	dwOwnerMode = hOwner->dwMode;
	if (dwOwnerMode == MONMODE_DEAD && D2CommonSeedRand(&hUnit->tSeed, 100) < 40)
	{
		SUnitDmgKill(ptGame, hUnit, SUnitGetTarget(ptGame, hOwner), TRUE);
		return;
	}
	if (!ptAi->adwParam[2])
	{
		AiUseSkill(ptGame, hUnit, SKILL_SUBMERGE, hTarget, 0, 0);
		AiThinkIn(ptGame, hUnit, 8);
		ptAi->adwParam[1] = ptMonStats->AIParam[2] + ptGame->nFrame;
		ptAi->adwParam[2] = 1;
		return;
	}
	if (ptAi->adwParam[2] != 1
		&& (nDist > 12
			|| (!bInReach && D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[1])
			|| (dwOwnerMode == 14 && D2CommonSeedRand(&hUnit->tSeed, 100) < 50))
		&& ptGame->nFrame > (int)ptAi->adwParam[1])
	{
		AiUseSkill(ptGame, hUnit, SKILL_SUBMERGE, hTarget, 0, 0);
		AiThinkIn(ptGame, hUnit, 8);
		ptAi->adwParam[1] = ptMonStats->AIParam[2] + ptGame->nFrame;
		ptAi->adwParam[2] = 1;
		return;
	}
	if (ptAi->adwParam[2] == 1)
	{
		if ((bInReach || nDist < 12) && ptGame->nFrame > (int)ptAi->adwParam[1]
			&& (dwOwnerMode != 14 || D2CommonSeedRand(&hUnit->tSeed, 100) < 5))
		{
			AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_EMERGE, hUnit, 0, 0);
			ptAi->adwParam[1] = ptMonStats->AIParam[3] + ptGame->nFrame;
			ptAi->adwParam[2] = 2;
			return;
		}
		if (ptAi->adwParam[2] == 1)
		{
			AiThinkIn(ptGame, hUnit, ptMonStats->AIParam[4]);
			return;
		}
	}
	if (bInReach && D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[0])
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	AiStand(ptGame, hUnit, ptMonStats->AIParam[4]);
}

void __fastcall AiThinkWaterWatcherHead(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	MONSTATSTXT *ptMonStats;
	UNIT *hTarget;
	int nDist;
	BOOL bInReach;
	UNIT *hNear;

	ptAi = ptParms->ptAi;
	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	bInReach = ptParms->fTargetInReach;
	ptMonStats = ptParms->ptMonStats;
	hNear = AiFindTargetInSight(ptGame, hUnit, &nDist, (DWORD *)&bInReach);
	if (!ptAi->adwParam[2])
	{
		AiUseSkill(ptGame, hUnit, SKILL_SUBMERGE, hTarget, 0, 0);
		AiThinkIn(ptGame, hUnit, 8);
		ptAi->adwParam[1] = ptMonStats->AIParam[2] + ptGame->nFrame;
		ptAi->adwParam[2] = 1;
		return;
	}
	if (ptAi->adwParam[2] != 1
		&& (nDist > 12
			|| (!bInReach && D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[1]))
		&& ptGame->nFrame > (int)ptAi->adwParam[1])
	{
		AiUseSkill(ptGame, hUnit, SKILL_SUBMERGE, hTarget, 0, 0);
		AiThinkIn(ptGame, hUnit, 20);
		ptAi->adwParam[1] = ptMonStats->AIParam[2] + ptGame->nFrame;
		ptAi->adwParam[2] = 1;
		return;
	}
	if (ptAi->adwParam[2] == 1)
	{
		if ((bInReach || (hTarget && D2CommonSeedRand(&hUnit->tSeed, 100) < 75))
			&& ptGame->nFrame > (int)ptAi->adwParam[1])
		{
			AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_EMERGE, hUnit, 0, 0);
			ptAi->adwParam[1] = ptMonStats->AIParam[3] + ptGame->nFrame;
			ptAi->adwParam[2] = 2;
			return;
		}
		if (ptAi->adwParam[2] == 1)
		{
			AiThinkIn(ptGame, hUnit, ptMonStats->AIParam[4]);
			return;
		}
	}
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[0])
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hNear);
		return;
	}
	AiStand(ptGame, hUnit, ptMonStats->AIParam[4]);
}

void __fastcall AiThinkPatrolInit(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hOwner;
	int nOwnerX;
	int nOwnerY;
	int nX;
	int nY;
	AIGENERAL *ptAi;

	hOwner = AiGetOwner(hUnit);
	if (!hOwner)
	{
#line 3405
		D2ASSERT(0);
	}
	nOwnerX = (int)D2CommonUnitGetX(hOwner);
	nOwnerY = (int)D2CommonUnitGetY(hOwner);
	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	ptAi = ptParms->ptAi;
	ptAi->adwParam[0] = nX - nOwnerX;
	ptAi->adwParam[1] = nY - nOwnerY;
	if (hOwner == hUnit)
		ptAi->adwParam[2] = D2CommonSeedRand(&hUnit->tSeed, 100);
}

void __fastcall AiThinkPatrol(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	AICOMMAND *ptCommand;
	BOOL bBroken;
	AICOMMAND tCommand;
	int nStep;
	int nX;
	int nY;
	int nAside;
	D2SEED *ptSeed;

	ptAi = ptParms->ptAi;
	ptCommand = AiGetCommand(hUnit);
	if (ptCommand && ptCommand->nCommand == 8)
	{
		AiRemoveFromOwner(hUnit);
		AiThinkSet(ptGame, hUnit, ptParms->ptAi, 0);
		AiStand(ptGame, hUnit, 10);
		return;
	}
	bBroken = FALSE;
	AiForEachMinion(hUnit, (DWORD)&bBroken, 0, (PFNAIMINIONFN)AiThinkPatrolOnMinion);
	if (!MonsterTestPrevModeGetHit(hUnit) && !bBroken)
	{
		if (AiGetOwner(hUnit) == hUnit)
		{
			ptSeed = &hUnit->tSeed;
			if (D2CommonSeedRand(ptSeed, 100) < 30)
			{
				AiStand(ptGame, hUnit, 10);
				return;
			}
			nStep = (int)ptAi->adwParam[2] + 1;
			if (nStep >= 100)
				nStep = 0;
			nX = (int)D2CommonUnitGetX(hUnit);
			nY = (int)D2CommonUnitGetY(hUnit);
			nAside = (int)D2CommonSeedRand(ptSeed, 10);
			if (nStep < 25)
			{
				if (D2CommonSeedNext(&hUnit->tSeed) & 1)
					nAside = -nAside;
				nX += nAside;
				nY -= 5;
			}
			else if (nStep < 50)
			{
				if (D2CommonSeedNext(&hUnit->tSeed) & 1)
					nAside = -nAside;
				nX += 5;
				nY += nAside;
			}
			if (nStep >= 50 && nStep < 75)
			{
				if (D2CommonSeedNext(&hUnit->tSeed) & 1)
					nAside = -nAside;
				nX += nAside;
				nY += 5;
			}
			if (nStep >= 75)
			{
				if (D2CommonSeedNext(&hUnit->tSeed) & 1)
					nAside = -nAside;
				nX -= 5;
				nY += nAside;
			}
			if (AiWalkToPoint(ptGame, hUnit, nX, nY))
			{
				tCommand.nCommand = 2;
				tCommand.dwArg1 = (DWORD)nX;
				tCommand.dwArg2 = (DWORD)nY;
				AiCommandMinions(ptGame, hUnit, &tCommand);
			}
			else
				nStep += 25;
			ptAi->adwParam[2] = (DWORD)nStep;
			return;
		}
		if (!ptCommand)
		{
			AiStand(ptGame, hUnit, 15);
			return;
		}
		nX = (int)(ptCommand->dwArg1 + ptAi->adwParam[0]);
		nY = (int)(ptCommand->dwArg2 + ptAi->adwParam[1]);
		AiPopCommand(ptGame, hUnit);
		AiWalkToPoint(ptGame, hUnit, nX, nY);
		return;
	}
	tCommand.nCommand = 8;
	AiCommandMinions(ptGame, hUnit, &tCommand);
	AiRemoveFromOwner(hUnit);
	AiThinkSet(ptGame, hUnit, ptParms->ptAi, 0);
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkPatrolOnMinion(UNIT *hMinion, BOOL *pbFlag, DWORD dwUnused)
{
	if (!hMinion || D2CommonMonsterUnitTestFieldC(hMinion))
		*pbFlag = TRUE;
}

void __fastcall AiThinkGriswold(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptSeed = &hUnit->tSeed;
	if (ptParms->fTargetInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < 80)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
	}
	else if (D2CommonSeedRand(ptSeed, 100) < 50)
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
		return;
	}
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkSkeleton(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	MONSTATSTXT *ptMonStats;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptSeed = &hUnit->tSeed;
	ptMonStats = ptParms->ptMonStats;
	if (ptParms->fTargetInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
		{
			if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[3])
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			else
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
			return;
		}
		AiStand(ptGame, hUnit, ptMonStats->AIParam[2]); return; }
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
		return;
	}
	AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
}

void __fastcall AiThinkSkeletonArcher(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	int nDist;
	DWORD dwNearDir;
	UNIT *hNear;
	MONSTATSTXT *ptMonStats;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptSeed = &hUnit->tSeed;
	nDist = ptParms->nDist;
	if (MonsterTestPrevModeGetHit(hUnit))
	{
		hNear = AiFindTargetInSight(ptGame, hUnit, &nDist, &dwNearDir);
		if (hNear)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hNear);
			return;
		}
	}
	ptMonStats = ptParms->ptMonStats;
	hNear = AiFindTargetInSight(ptGame, hUnit, &nDist, &dwNearDir);
	if (hNear && nDist < 20)
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hNear);
			return;
		}
		if (D2CommonSeedRand(ptSeed, 100) < 20)
		{
			AiWalkToTargetFast(ptGame, hUnit, hTarget, 3, FALSE);
			return;
		}
		AiStand(ptGame, hUnit, ptMonStats->AIParam[1]);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2])
	{
		AiWalkToDistance(ptGame, hUnit, hTarget, ptMonStats->AIParam[3], ptMonStats->AIParam[4]);
		return;
	}
	AiStand(ptGame, hUnit, 20);
}

void __fastcall AiThinkGhost(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	MONSTATSTXT *ptMonStats;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptSeed = &hUnit->tSeed;
	ptMonStats = ptParms->ptMonStats;
	if (ptParms->fTargetInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		AiStand(ptGame, hUnit, ptMonStats->AIParam[2]); return; }
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
	{
		AiWalkToDistance(ptGame, hUnit, hTarget, 12, 0);
		return;
	}
	AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
}

void __fastcall AiThinkFlyingScimitar(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	D2SEED *ptSeed;
	UNIT *hTarget;
	MONSTATSTXT *ptMonStats;

	ptSeed = &hUnit->tSeed;
	hTarget = ptParms->hTarget;
	ptMonStats = ptParms->ptMonStats;
	if (ptParms->fTargetInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[3])
		{
			AiWalkToTargetFast(ptGame, hUnit, hTarget, 2, FALSE);
			return;
		} AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
	}
	else
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
		{
			AiWalkToDistance(ptGame, hUnit, hTarget, 8, 1);
			return;
		}
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
		{
			AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
			return;
		} AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
	}

}

void __fastcall AiThinkHuntress(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	MONSTATSTXT *ptMonStats;
	AIFINDRECORD tFind;
	D2SEED *ptSeed;
	UNIT *hFound;

	hTarget = ptParms->hTarget;
	ptMonStats = ptParms->ptMonStats;
	ptSeed = &hUnit->tSeed;
	if (ptParms->fTargetInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		AiStand(ptGame, hUnit, ptMonStats->AIParam[3]);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
	{
		AiSetVelocity(hUnit, 0, 75, 0);
		AiChargeTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[4])
	{
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 3, FALSE);
		return;
	}
	tFind.tUnit.hUnit = NULL;
	tFind.tUnit.nDist = 2147483647;
	AiFind(ptGame, hUnit, &tFind, AiThinkKinFindFn, 1);
	hFound = tFind.tUnit.hUnit;
	if (hFound && tFind.tUnit.nDist > ptMonStats->AIParam[2])
	{
		AiSetVelocity(hUnit, 0, 75, 0);
		AiWalkToTarget(ptGame, hUnit, hFound, AIMOVE_ALL);
		return;
	}
	AiStand(ptGame, hUnit, ptMonStats->AIParam[3]);
}

UNIT * __fastcall AiThinkKinFindFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind)
{
	int nDist;

	if (hCandidate->eType == UNIT_MONSTER && hUnit != hCandidate && hUnit->dwClassId == hCandidate->dwClassId
		&& hCandidate->dwMode && hCandidate->dwMode != MONMODE_DEAD)
	{
		nDist = AiThinkDistance(hUnit, hCandidate);
		if (nDist < ptFind->tUnit.nDist)
		{
			ptFind->tUnit.hUnit = hCandidate;
			ptFind->tUnit.nDist = nDist;
		}
	}
	return NULL;
}

void __fastcall AiThinkSlinger(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	BOOL bInReach;
	MONSTATSTXT *ptMonStats;
	UNIT *hTarget;
	int nDist;
	DWORD dwNearDir;
	UNIT *hNear;
	AIFINDRECORD tFind;

	bInReach = ptParms->fTargetInReach;
	ptMonStats = ptParms->ptMonStats;
	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	hNear = AiFindTargetInSight(ptGame, hUnit, &nDist, &dwNearDir);
	if ((bInReach || nDist < 8)
		&& D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[3])
	{
		AiWalkAway(ptGame, hUnit, hTarget, 16, TRUE);
		AiThinkIn(ptGame, hUnit, ptMonStats->AIParam[4]);
		return;
	}
	if (nDist > 12 && D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[0])
	{
		AiWalkAround(ptGame, hUnit, hTarget, 4);
		return;
	}
	if (hNear && nDist < 30)
	{
		if (D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[1])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hNear);
			return;
		} AiStand(ptGame, hUnit, ptMonStats->AIParam[4]);
	}
	else
	{
		tFind.tUnit.hUnit = NULL;
		tFind.tUnit.nDist = 2147483647;
		AiFind(ptGame, hUnit, &tFind, AiThinkKinFindFn, 1);
		if (tFind.tUnit.hUnit && tFind.tUnit.nDist > ptMonStats->AIParam[2])
		{
			AiWalkToTarget(ptGame, hUnit, tFind.tUnit.hUnit, AIMOVE_ALL);
			return;
		} AiStand(ptGame, hUnit, ptMonStats->AIParam[4]);
	}

}

void __fastcall AiThinkSandMaggotOut(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AICOMMAND *ptCommand;
	AIGENERAL *ptAi;

	ptCommand = AiFindOrAddCommand(ptGame, hUnit, 14, FALSE);
	ptAi = ptParms->ptAi;
	if (ptCommand->dwArg4 == 1)
	{
		AiUseSkill(ptGame, hUnit, SKILL_MAGOTTUP, SUnitGetTarget(ptGame, hUnit), 0, 0);
		AiThinkWithin(ptGame, hUnit, 30);
		ptCommand->dwArg4 = 0;
		return;
	}
	AiThinkSet(ptGame, hUnit, ptAi, ptAi->nThinkKind);
	AiThinkWithin(ptGame, hUnit, 1);
}

void __fastcall AiThinkSandMaggot(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	AIGENERAL *ptAi;
	BOOL bInReach;
	MONSTATSTXT *ptMonStats;
	int nNearDist;
	DWORD dwNearDir;
	UNIT *hNear;
	D2SEED *ptSeed = &hUnit->tSeed;
	hTarget = ptParms->hTarget;
	ptAi = ptParms->ptAi;
	bInReach = ptParms->fTargetInReach;
	ptMonStats = ptParms->ptMonStats;
	hNear = AiFindTargetInSight(ptGame, hUnit, &nNearDist, &dwNearDir);
	if ((int)ptAi->adwParam[0] < 3 && !hTarget && (!hNear || nNearDist > 10)
		&& ptGame->nFrame > (int)ptAi->adwParam[1])
	{
		AiUseSkill(ptGame, hUnit, SKILL_MAGOTTDOWN, NULL, 0, 0);
		AiThinkWithin(ptGame, hUnit, 30);
		ptAi->adwParam[1] = ptMonStats->AIParam[4] + ptGame->nFrame;
		ptAi->adwParam[0] = 3;
		return;
	}
	if ((hTarget || (hNear && nNearDist < 16)) && ptAi->adwParam[0] == 3
		&& ptGame->nFrame > (int)ptAi->adwParam[1])
	{
		AiUseSkill(ptGame, hUnit, SKILL_MAGOTTUP, hTarget, 0, 0);
		AiThinkWithin(ptGame, hUnit, 25);
		ptAi->adwParam[1] = ptMonStats->AIParam[4] + ptGame->nFrame;
		ptAi->adwParam[0] = 1;
		return;
	}
	if (ptAi->adwParam[0] == 3)
	{
		AiThinkWithin(ptGame, hUnit, 20);
		return;
	}
	if ((DWORD)MonsterGetLifePercent(hUnit) < 25 && nNearDist < 7 && ptGame->nFrame > (int)ptAi->adwParam[1]
		&& D2CommonSeedRand(ptSeed, 100) < 20)
	{
		AiUseSkill(ptGame, hUnit, SKILL_MAGOTTDOWN, hTarget, 0, 0);
		AiThinkWithin(ptGame, hUnit, 30);
		ptAi->adwParam[1] = ptMonStats->AIParam[4] + ptGame->nFrame;
		ptAi->adwParam[0] = 3;
		return;
	}
	if (bInReach && D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[3])
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	if (hNear && nNearDist < 15
		&& D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hNear);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < 20)
	{
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 6, FALSE);
		return;
	}
	if ((int)ptAi->adwParam[2] < ptMonStats->AIParam[2]
		&& D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
	{
		if (ptAi->adwParam[0] == 2)
		{
			ptAi->adwParam[2]++;
			ptAi->adwParam[0] = 1;
			AiUseSkill(ptGame, hUnit, SKILL_MAGOTTLAY, hTarget, 0, 0);
			AiThinkWithin(ptGame, hUnit, 20);
			return;
		}
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 6, FALSE);
		ptAi->adwParam[0] = 2;
		return;
	}
	AiThinkWithin(ptGame, hUnit, 12);
}

void __fastcall AiThinkSandMaggotQueen(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	MONSTATSTXT *ptMonStats;

	int nX;
	int nY;
	int nClass;
	int nMode;
	GAMEROOM *hRoom;

	ptMonStats = ptParms->ptMonStats;

	if (ptParms->ptAi->adwParam[2])
	{
		AiStand(ptGame, hUnit, ptMonStats->AIParam[1]);
		ptParms->ptAi->adwParam[2] = 0;
		return;
	}
	if (!ptParms->ptAi->adwParam[1])
	{
		if ((int)ptParms->ptAi->adwParam[0] < ptMonStats->AIParam[0])
		{
			AiSetModeToPoint(ptGame, hUnit, D2CommonUnitGetX(hUnit), D2CommonUnitGetY(hUnit), MONMODE_SKILL1);

			AiThinkIn(ptGame, hUnit, ptMonStats->AIParam[1]);
			ptParms->ptAi->adwParam[1] = 1;
			return;
		}
	}
	if (ptParms->ptAi->adwParam[1])
	{
		D2CommonMonsterGetSpecialPosition(hUnit, &nClass, &nX, &nY, &nMode, 0);
		hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hUnit), nX, nY);
		if (hRoom && MonsterSpawn(ptGame, hRoom, nX, nY, nClass, (DWORD)nMode, 2, 66))
			ptParms->ptAi->adwParam[0]++;
		AiThinkIn(ptGame, hUnit, ptMonStats->AIParam[1]);
		ptParms->ptAi->adwParam[1] = 0;
		ptParms->ptAi->adwParam[2] = 1;
	}
}

void __fastcall AiThinkTombViper(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	int nDist;
	BOOL bInReach;
	MONSTATSTXT *ptMonStats;
	UNIT *hTarget;
	int nX;
	int nY;
	D2SEED *ptSeed;

	nDist = ptParms->nDist;
	ptSeed = &hUnit->tSeed;
	bInReach = ptParms->fTargetInReach;
	ptMonStats = ptParms->ptMonStats;
	hTarget = ptParms->hTarget;
	if (ptParms->ptAi->adwParam[0])
		D2CommonStatsUnitSetActiveStateBit(hUnit, 90, 0);
	if (bInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
		{
			if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			else
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
			return;
		}
		AiStand(ptGame, hUnit, ptMonStats->AIParam[4]);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2])
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
		return;
	}
	if (nDist < 8 && D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[3])
	{
		nY = (int)D2CommonUnitGetY(hTarget);

		if (MonsterCanUseSkill(ptGame, hUnit, 107, hTarget, ((int)D2CommonUnitGetX(hTarget)), nY))
		{
			AiUseSkill(ptGame, hUnit, SKILL_CHARGE, hTarget, 0, 0);
			D2CommonStatsUnitSetActiveStateBit(hUnit, 90, 1);
			ptParms->ptAi->adwParam[0] = 1;
			return;
		}
	}
	AiStand(ptGame, hUnit, ptMonStats->AIParam[4]);
}

void __fastcall AiThinkBalrog(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	BOOL bInReach;
	MONSTATSTXT *ptMonStats;

	hTarget = ptParms->hTarget;
	bInReach = ptParms->fTargetInReach;
	ptMonStats = ptParms->ptMonStats;
	if (D2CommonStatsGetState(hUnit, 12))
		D2CommonStatsUnitSetActiveStateBit(hUnit, 12, 0);
	if (bInReach)
	{
		if (D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[0])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_FETISHINFERNO, hTarget, 0, 0);
		return;
	}
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < 80)
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
		return;
	}
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkBlunderbore(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	MONSTATSTXT *ptMonStats;
	MONSTERSCALE tScale;
	DWORD dwChance;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptSeed = &hUnit->tSeed;
	ptMonStats = ptParms->ptMonStats;
	if (ptParms->fTargetInReach)
	{
		MonsterGetScale(&tScale, ptGame, D2CommonUnitGetPathRoom(hUnit), (DWORD)hUnit);
		dwChance = (DWORD)(ptMonStats->AIParam[0] + tScale.nDifficulty * 10);
		if (D2CommonSeedRand(ptSeed, 100) < dwChance)
		{
			if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[4])
				AiUseSkill(ptGame, hUnit, SKILL_SMITE, hTarget, 0, 0);
			else
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		AiStand(ptGame, hUnit, ptMonStats->AIParam[1]);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2])
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
		return;
	}
	AiStand(ptGame, hUnit, ptMonStats->AIParam[3]);
}

void __fastcall AiThinkDungSoldier(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	BOOL bInReach;
	int nDist;
	AICOMMAND *ptCommand;
	AICOMMAND tCommand;
	MONSTATSTXT *ptMonStats;
	AIGENERAL *ptAi;
	D2SEED *ptSeed;

	nDist = ptParms->nDist;
	hTarget = ptParms->hTarget;
	bInReach = ptParms->fTargetInReach;
	ptSeed = &hUnit->tSeed;
	ptCommand = AiGetCommand(hUnit);
	if (!ptCommand && nDist < 20 && AiGetOwner(hUnit) == hUnit
		&& D2CommonSeedRand(ptSeed, 100) < 20)
	{
		tCommand.nCommand = 1;
		AiCommandMinions(ptGame, hUnit, &tCommand);
		AiPushCommand(ptGame, hUnit, &tCommand);
		ptCommand = &tCommand;
	}
	if (ptCommand && ptCommand->nCommand == 1)
	{
		if (bInReach)
		{
			AiPopCommand(ptGame, hUnit);
			AiUseSkill(ptGame, hUnit, SKILL_JAB, hTarget, 0, 0);
			return;
		}
		AiSetVelocity(hUnit, 2, 100, 0);
		if (AiWalkToTarget(ptGame, hUnit, hTarget, 0))
			return;
		AiPopCommand(ptGame, hUnit);
		return;
	}
	if (bInReach)
	{
		ptMonStats = ptParms->ptMonStats;
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
		{
			if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[3])
			{
				AiUseSkill(ptGame, hUnit, SKILL_JAB, hTarget, 0, 0);
				return;
			}
			if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			else
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
			return;
		}
		AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
		return;
	}
	ptAi = ptParms->ptAi;
	if (!ptAi->adwParam[0])
	{
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 0, FALSE);
		ptAi->adwParam[0] = 1;
		return;
	}
	AiSetVelocity(hUnit, 2, 0, 4);
	AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
	if (D2CommonSeedRand(ptSeed, 100) > 10)
		ptAi->adwParam[0] = 0;
}

void __fastcall AiThinkSandMaggotYoung(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	MONSTATSTXT *ptMonStats;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptSeed = &hUnit->tSeed;
	ptMonStats = ptParms->ptMonStats;
	if (ptParms->fTargetInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		AiStand(ptGame, hUnit, ptMonStats->AIParam[1]);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2])
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_MARK);
		return;
	}
	AiStand(ptGame, hUnit, ptMonStats->AIParam[3]);
}

void __fastcall AiThinkSandMaggotEgg(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	MONSTATSTXT *ptMonStats;
	UNIT *hTarget;

	ptAi = ptParms->ptAi;
	hTarget = ptParms->hTarget;
	ptMonStats = ptParms->ptMonStats;
	AiFindOrAddCommand(ptGame, hUnit, 14, FALSE);
	if (ptAi->adwParam[0] != 1 && D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[1])
	{
		AiUseSkill(ptGame, hUnit, SKILL_MAGGOTEGG, hTarget, 0, 0);
		AiThinkIn(ptGame, hUnit, ptMonStats->AIParam[0]);
		ptAi->adwParam[0] = 1;
		return;
	}
	if (ptAi->adwParam[0] == 1)
		SUnitDmgKill(ptGame, hUnit, SUnitGetTarget(ptGame, hUnit), TRUE);
	AiStand(ptGame, hUnit, ptMonStats->AIParam[0]);
}

void __fastcall AiThinkAndariel(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptSeed = &hUnit->tSeed;
	if (ptParms->fTargetInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < 30)
		{
			AiUseSkill(ptGame, hUnit, SKILL_ANDRIALSPRAY, hTarget, 0, 0);
			return;
		}
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < 10)
	{
		AiStand(ptGame, hUnit, 5);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < 30)
	{
		if (D2CommonSeedRand(ptSeed, 100) < 50)
		{
			AiUseSkill(ptGame, hUnit, SKILL_ANDRIALSPRAY, hTarget, 0, 0);
			return;
		}
		AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK1, SKILL_ANDYPOISONBOLT, hTarget, 0, 0);
		return;
	}
	AiSetVelocity(hUnit, 1, 0, 0);
	AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
}

void __fastcall AiThinkSummoner(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	int nDist;
	AIGENERAL *ptAi;
	UNIT *hTarget;
	MONSTATSTXT *ptMonStats;
	BOOL bFirst;
	int nStat;
	int nX;
	int nY;

	nDist = ptParms->nDist;
	ptAi = ptParms->ptAi;
	hTarget = ptParms->hTarget;
	if (!ptAi->adwParam[0])
		A2Q5BossSeen(ptGame);
	ptAi->adwParam[0]++;
	ptMonStats = ptParms->ptMonStats;
	if (nDist < 5 && D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[4])
		AiWalkAway(ptGame, hUnit, hTarget, 6, FALSE);
	bFirst = TRUE;
	if (hTarget)
	{
		nStat = D2CommonStatsUnitGetValue(hTarget, 43);
		if (D2CommonStatsUnitGetValue(hTarget, 39) < nStat)
			bFirst = FALSE;
	}
	nX = (int)D2CommonUnitGetX(hTarget);
	nY = (int)D2CommonUnitGetY(hTarget);
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[0]
		|| (bFirst && D2CommonSeedRand(&hUnit->tSeed, 100) < 33))
	{
		gbAiThink91Toggle = !gbAiThink91Toggle;
		if (gbAiThink91Toggle && nDist < 4)
			AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_FROST_NOVA, hTarget, nX, nY);
		else
			AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_GLACIAL_SPIKE, hTarget, nX, nY);
		return;
	}
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[1]
		|| (!bFirst && D2CommonSeedRand(&hUnit->tSeed, 100) < 33))
	{
		gbAiThink91Toggle = !gbAiThink91Toggle;
		if (gbAiThink91Toggle)
			AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_FIRE_BALL, hTarget, nX, nY);
		else
			AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_FIRE_WALL, hTarget, nX, nY);
		return;
	}
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[2])
	{
		AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_WEAKEN, hTarget, nX, nY);
		return;
	}
	AiStepAside(ptGame, hUnit, 4);
}

void __fastcall AiThinkGloam(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget = ptParms->hTarget;
	BOOL bInReach = ptParms->fTargetInReach;
	MONSTATSTXT *ptMonStats = ptParms->ptMonStats;
	AIGENERAL *ptAi = ptParms->ptAi;
	D2SEED *ptSeed;
	int aaanGo6[7][4][2] = {
		{ { -10, -3 }, { 1, -5 }, { 6, 0 }, { 4, 10 } },
		{ { -9, -2 }, { -6, -11 }, { -2, -5 }, { 6, 1 } },
		{ { 0, 13 }, { -12, 0 }, { 7, 5 }, { -3, -11 } },
		{ { -15, -3 }, { -6, -3 }, { -4, -5 }, { -6, -13 } },
		{ { -12, -2 }, { -6, 2 }, { -5, 5 }, { 1, 11 } },
		{ { -13, -6 }, { -8, -11 }, { -7, 0 }, { 1, 8 } },
		{ { -5, -8 }, { -8, 1 }, { 1, -6 }, { 6, 9 } },
	};
	int aaanCast6[7][4][2] = {
		{ { 1, -5 }, { 6, 0 }, { 4, 10 }, { -10, -3 } },
		{ { 1, 8 }, { -9, -2 }, { -5, 4 }, { 1, 8 } },
		{ { -12, 0 }, { 7, 5 }, { -3, -11 }, { 11, -2 } },
		{ { 7, 9 }, { -15, -3 }, { -6, -13 }, { 9, 7 } },
		{ { -5, -7 }, { -5, -7 }, { 9, 0 }, { -12, -2 } },
		{ { -8, -11 }, { -2, -6 }, { -2, -6 }, { -13, -6 } },
		{ { -8, 1 }, { 1, -6 }, { 6, 9 }, { 0, 0 } },
	};
	int aanGo5[5][2] = {
		{ -5, -5 }, { 3, -5 }, { 6, 3 }, { 3, 6 }, { -5, 3 },
	};
	int aanCast5[5][2] = {
		{ 6, 3 }, { 3, 6 }, { -5, 3 }, { -5, -5 }, { 3, -5 },
	};
	ptSeed = &hUnit->tSeed;
	GAMEROOM *hRoom;
	int i;
	int j;
	int nEnd;
	DWORD dwStatList;
	UNITFINDDATA tFind;
	UNITFINDARGS tArgs;

	if (D2CommonSeedRand(ptSeed, 1000) <= 5 && (int)ptAi->adwParam[0] < 4)
		ptAi->adwParam[0] = 4;
	if (ptAi->adwParam[0] == 4)
	{
		int nX;
		int nY;

		hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
		nX = (int)D2CommonUnitGetX(hUnit);
		nY = (int)D2CommonUnitGetY(hUnit);
		for (i = 0; i < (int)(sizeof(UNITFINDARGS) / sizeof(DWORD)); i++)
			((DWORD *)&tArgs)[i] = 0;
		tArgs.nY = nY;
		tArgs.nX = nX;
		tArgs.dwFlags = 1411;
		tArgs.nDistance = 64;
		D2CommonUnitFindsInit(ptGame->pMemPool, &tFind, hRoom, nX, nY, 64,
			AiThinkTestRingUnit, &tArgs);
		D2CommonUnitFindsFindUnits(&tFind);
		if (tFind.nNumUnits == 4 && D2CommonSeedRand(ptSeed, 100) <= 20)
		{
			for (i = 0; i < 4; i++)
			{
				AiSetParam(tFind.phUnitsArray[i], 1, 6);
				AiSetParam(tFind.phUnitsArray[i], 2, 0);
				AiSetParam(tFind.phUnitsArray[i], 3, (DWORD)(i + 1));
				AiStand(ptGame, hUnit, 173 - ptGame->nFrame % 173);
				if (hTarget && hTarget->eType == UNIT_PLAYER && hUnit && hUnit->eType == UNIT_MONSTER
					&& !SUnitIsDead(hUnit) && !SUnitIsDead(hTarget))
				{
					nEnd = ptGame->nFrame + 108000;
					dwStatList = D2CommonStatsAllocList(ptGame->pMemPool, 2, nEnd, hTarget->eType,
						hTarget->dwId);
					if (dwStatList)
					{
						D2CommonStatsListSetValueB(dwStatList, 80, 100);
						D2CommonStatsSetListEndFrame(dwStatList, nEnd);
						EventAdd(ptGame, hTarget, 12, nEnd);
						D2CommonStatsUnitLinkList(hTarget, dwStatList);
					}
				}
			}
		}
		else if (tFind.nNumUnits >= 5)
		{
			for (j = 0; j < 5; j++)
			{
				AiSetParam(tFind.phUnitsArray[j], 1, 5);
				AiSetParam(tFind.phUnitsArray[j], 2, 0);
				AiSetParam(tFind.phUnitsArray[j], 3, (DWORD)(j + 1));
				AiStand(ptGame, hUnit, 173 - ptGame->nFrame % 173);
			}
		}
		else
		{
			ptAi->adwParam[0] = 2;
			AiStand(ptGame, hUnit, 8);
			return;
		}
	}
	if ((int)ptAi->adwParam[0] >= 6)
	{
		int nX;
		int nY;

		nX = (int)D2CommonUnitGetX(hTarget)
			+ aaanGo6[ptAi->adwParam[0] - 6][ptAi->adwParam[2] - 1][0];
		nY = (int)D2CommonUnitGetY(hTarget)
			+ aaanGo6[ptAi->adwParam[0] - 6][ptAi->adwParam[2] - 1][1];
		if ((DWORD)AiThinkPointDistance(hUnit, nX, nY) > 1)
		{
			if (D2CommonSeedRand(ptSeed, 100) < 10)
			{
				AiStepAside(ptGame, hUnit, 8);
				return;
			}
			AiSetModeToPointPathZero(ptGame, hUnit, nX, nY, 2);
			return;
		}
		if (ptGame->nFrame % 53)
		{
			AiStand(ptGame, hUnit, 53 - ptGame->nFrame % 53);
			return;
		}
		if ((int)ptAi->adwParam[1] < 3)
		{
			if (!aaanCast6[ptAi->adwParam[0] - 6][ptAi->adwParam[2] - 1][0]
				&& !aaanCast6[ptAi->adwParam[0] - 6][ptAi->adwParam[2] - 1][1])
			{
				ptAi->adwParam[1]++;
				AiStand(ptGame, hUnit, 53 - ptGame->nFrame % 53);
				return;
			}
			nX = (int)D2CommonUnitGetX(hTarget)
				+ aaanCast6[ptAi->adwParam[0] - 6][ptAi->adwParam[2] - 1][0];
			nY = (int)D2CommonUnitGetY(hTarget)
				+ aaanCast6[ptAi->adwParam[0] - 6][ptAi->adwParam[2] - 1][1];
			AiSetModeAtPoint(ptGame, hUnit, MONMODE_CAST, nX, nY);
			ptAi->adwParam[1]++;
			return;
		}
		ptAi->adwParam[0]++;
		if ((int)ptAi->adwParam[0] - 6 > 6)
			ptAi->adwParam[0] = 2;
		ptAi->adwParam[1] = 0;
		AiStand(ptGame, hUnit, 173 - ptGame->nFrame % 173);
		return;
	}
	if (ptAi->adwParam[0] == 5)
	{
		int nX;
		int nY;

		nX = (int)D2CommonUnitGetX(hTarget) + aanGo5[ptAi->adwParam[2] - 1][0];
		nY = (int)D2CommonUnitGetY(hTarget) + aanGo5[ptAi->adwParam[2] - 1][1];
		if ((DWORD)AiThinkPointDistance(hUnit, nX, nY) > 1)
		{
			if (D2CommonSeedRand(ptSeed, 100) < 10)
			{
				AiStepAside(ptGame, hUnit, 8);
				return;
			}
			AiSetModeToPointPathZero(ptGame, hUnit, nX, nY, 2);
			return;
		}
		if (ptGame->nFrame % 53)
		{
			AiStand(ptGame, hUnit, 53 - ptGame->nFrame % 53);
			return;
		}
		if ((int)ptAi->adwParam[1] < 3)
		{
			if (!aanCast5[ptAi->adwParam[2] - 1][0] && !aanCast5[ptAi->adwParam[2] - 1][1])
			{
				ptAi->adwParam[1]++;
				AiStand(ptGame, hUnit, 173 - ptGame->nFrame % 173);
			}
			else
			{
				nX = (int)D2CommonUnitGetX(hTarget) + aanCast5[ptAi->adwParam[2] - 1][0];
				nY = (int)D2CommonUnitGetY(hTarget) + aanCast5[ptAi->adwParam[2] - 1][1];
				AiSetModeAtPoint(ptGame, hUnit, MONMODE_CAST, nX, nY);
				ptAi->adwParam[1]++;
			}
			return;
		}
		ptAi->adwParam[0] = 2;
		ptAi->adwParam[1] = 0;
		AiStand(ptGame, hUnit, 173 - ptGame->nFrame % 173);
		return;
	}
	if (ptAi->adwParam[0] == 1)
	{
		if ((int)ptAi->adwParam[1] <= 0)
		{
			if (bInReach)
			{
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_SKILL1, NULL);
				ptAi->adwParam[0] = 3;
				return;
			}
			if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
			{
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_SKILL1, NULL);
				ptAi->adwParam[0] = 2;
				return;
			}
		}
		ptAi->adwParam[1]--;
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2])
		{
			ptAi->adwParam[0] = 1;
			AiWalkToTarget(ptGame, hUnit, hTarget, 0);
			return;
		}
		ptAi->adwParam[0] = 1;
		AiStepAside(ptGame, hUnit, 6);
		return;
	}
	if ((!bInReach && D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
		|| ptAi->adwParam[0] == 2)
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_CAST, hTarget);
		ptAi->adwParam[0] = 0;
		return;
	}
	if ((bInReach && D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
		|| ptAi->adwParam[0] == 3)
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		ptAi->adwParam[0] = 0;
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2])
	{
		ptAi->adwParam[0] = 1;
		AiWalkToTarget(ptGame, hUnit, hTarget, 0);
		ptAi->adwParam[1] = 3;
		return;
	}
	ptAi->adwParam[0] = 1;
	AiStepAside(ptGame, hUnit, 4);
	ptAi->adwParam[1] = 3;
}

BOOL __fastcall AiThinkTestRingUnit(UNIT *hUnit, DWORD dwArgs)
{
	if (hUnit->eType != UNIT_MONSTER)
		return FALSE;
	if (D2CommonMonsterUnitTestFieldC(hUnit))
		return FALSE;
	return D2CommonMonsterUnitGetTypeLocalIndex(hUnit) == 118;
}

void __fastcall AiThinkFoulCrow(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	UNIT *hTarget;
	BOOL bInReach;
	int nDist;
	D2SEED *ptSeed;

	ptAi = ptParms->ptAi;
	bInReach = ptParms->fTargetInReach;
	nDist = ptParms->nDist;
	ptSeed = &hUnit->tSeed;
	hTarget = ptParms->hTarget;
	if (ptAi->adwParam[0] == 1 && bInReach)
	{
		ptAi->adwParam[0] = 0;
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	ptAi->adwParam[0] = 0;
	if (bInReach)
	{
		AiSetVelocity(hUnit, 0, 50, 0);
		if (!AiWalkAway(ptGame, hUnit, hTarget, 4, TRUE))
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < 70)
	{
		if (nDist > 3)
		{
			if (D2CommonSeedRand(ptSeed, 100) > 10)
			{
				AiSetVelocity(hUnit, 0, -50, 0);
				AiStepAside(ptGame, hUnit, 4);
				return;
			}
			AiSetVelocity(hUnit, 0, 0, 0);
			AiStepAside(ptGame, hUnit, 3);
			return;
		}
	}
	else
	{
		AiSetVelocity(hUnit, 0, 100, nDist);
		ptAi->adwParam[0] = 1;
		AiWalkToTarget(ptGame, hUnit, hTarget, 0);
		return;
	}
	AiSetVelocity(hUnit, 0, 50, 0);
	if (!AiWalkAway(ptGame, hUnit, hTarget, 4, TRUE))
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
}

void __fastcall AiThinkAfflicted(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	BOOL bInReach;
	UNIT *hTarget;
	MONSTATSTXT *ptMonStats;
	int nDist;

	bInReach = ptParms->fTargetInReach;
	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	ptMonStats = ptParms->ptMonStats;
	if (!bInReach && MonsterTestPrevModeGetHit(hUnit))
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
		return;
	}
	if ((DWORD)MonsterGetLifePercent(hUnit) < ptMonStats->AIParam[0])
	{ DWORD dwNearDir;
		if (nDist < 3)
		{
			AiSetVelocity(hUnit, 0, 50, 0);
			if (!AiWalkAway(ptGame, hUnit, hTarget, 5, TRUE))
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
			return;
		}
		if (nDist > 15)
		{
			AiWalkToTargetStep(ptGame, hUnit, hTarget, 6);
			return;
		}
		if (AiFindTargetInSight(ptGame, hUnit, &nDist, &dwNearDir)
			&& D2CommonSeedRand(&hUnit->tSeed, 100) < 60)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
			return;
		}
		if (D2CommonSeedRand(&hUnit->tSeed, 100) < 60)
		{
			AiStand(ptGame, hUnit, 10);
			return;
		}
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 3, FALSE);
		return;
	}
	if (bInReach)
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
}

void __fastcall AiThinkDarkRanger(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	MONSTATSTXT *ptMonStats;
	UNIT *hTarget;
	int nDist;
	BOOL bInReach;
	DWORD dwNearDir;
	UNIT *hNear;
	MONSTERSCALE tScale;
	DWORD dwChance;

	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	bInReach = ptParms->fTargetInReach;
	ptMonStats = ptParms->ptMonStats;
	hNear = AiFindTargetInSight(ptGame, hUnit, &nDist, &dwNearDir);
	if (!hNear)
	{
		if (D2CommonSeedRand(&hUnit->tSeed, 100) < 50)
		{
			AiWalkToTargetFast(ptGame, hUnit, hTarget, 3, FALSE);
			return;
		}
		AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
		return;
	}
	if (!bInReach && MonsterTestPrevModeGetHit(hUnit))
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hNear);
		return;
	}
	MonsterGetScale(&tScale, ptGame, D2CommonUnitGetPathRoom(hUnit), (DWORD)hUnit);
	dwChance = (DWORD)(ptMonStats->AIParam[3] + tScale.nDifficulty * 10);
	if (nDist < 6 && D2CommonSeedRand(&hUnit->tSeed, 100) < dwChance)
	{
		AiSetVelocity(hUnit, 0, 100, 0);
		if (AiRunAway(ptGame, hUnit, hNear, 12, TRUE))
			return;
	}
	if (nDist > 10 && D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[0])
	{
		AiSetVelocity(hUnit, 0, 10, 0);
		AiWalkToTargetStep(ptGame, hUnit, hNear, 9);
		return;
	}
	if (nDist > 20 - tScale.nDifficulty * 3)
	{
		AiSetVelocity(hUnit, 0, 100, 0);
		AiRunToTargetStep(ptGame, hUnit, hNear, 9);
		return;
	}
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[1])
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hNear);
		return;
	}
	AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
}

void __fastcall AiThinkDarkHunter(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	BOOL bInReach;
	int nDist;
	MONSTERSCALE tScale;
	MONSTATSTXT *ptMonStats;
	DWORD dwChance;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	bInReach = ptParms->fTargetInReach;
	nDist = ptParms->nDist;
	MonsterGetScale(&tScale, ptGame, D2CommonUnitGetPathRoom(hUnit), (DWORD)hUnit);
	if (nDist > 20 - tScale.nDifficulty * 3)
	{
		AiSetVelocity(hUnit, 13, 100, 0);
		AiRunToTargetStep(ptGame, hUnit, hTarget, 3);
		return;
	}
	ptSeed = &hUnit->tSeed;
	ptMonStats = ptParms->ptMonStats;
	if (bInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
	{
		dwChance = (DWORD)(tScale.nDifficulty * 15);
		if (D2CommonSeedRand(ptSeed, 100) < dwChance)
		{
			AiSetVelocity(hUnit, 13, 100, 0);
			AiRunToTargetStep(ptGame, hUnit, hTarget, 3);
			return;
		}
		AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
		return;
	}
	AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
}

void __fastcall AiThinkGuardPlace(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	int nDist;
	BOOL bInReach;
	GAMEROOM *hRoom;
	MONSTATSTXT *ptMonStats;
	AICOMMAND *ptCommand;
	AICOMMAND tCommand;
	int nLogic;
	D2SEED *ptSeed;
	AIGENERAL *ptAi;
	PRESETFILELIST *ptPath;
	PRESETPATHNODE *ptNode;

	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	bInReach = ptParms->fTargetInReach;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	ptMonStats = ptParms->ptMonStats;
	ptSeed = &hUnit->tSeed;
	ptCommand = AiFindCommand(hUnit, 10, FALSE);
	if (!ptCommand)
	{
		tCommand.nCommand = 10;
		tCommand.dwArg1 = D2CommonUnitGetX(hUnit);
		tCommand.dwArg2 = D2CommonUnitGetY(hUnit);
		AiPushCommand(ptGame, hUnit, &tCommand);
		ptCommand = AiFindCommand(hUnit, 10, FALSE);
	}
	nLogic = D2CommonDungeonGetLogicValue(hRoom, ptCommand->dwArg1, ptCommand->dwArg2);
	if (nLogic != D2CommonDungeonGetLogicValue(hRoom, D2CommonUnitGetX(hUnit), D2CommonUnitGetY(hUnit)))
	{
		MonsterModeSetOverride(MONDATA(hUnit)->ptModeData, 1, 0, 0);
		if (AiRunToPoint(ptGame, hUnit, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2))
			return;
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
	}
	if (nLogic != D2CommonDungeonGetLogicValue(hRoom, D2CommonUnitGetX(hTarget), D2CommonUnitGetY(hTarget)))
	{
		if (D2CommonUnitGetX(hUnit) == ptCommand->dwArg1
			&& D2CommonUnitGetY(hUnit) == ptCommand->dwArg2)
		{
			if (nDist < 25)
			{
				ptAi = ptParms->ptAi;
				ptPath = (PRESETFILELIST *)ptAi->pCell;
				if (ptPath && (int)ptAi->adwParam[0] < ptPath->nNodes)
				{
					ptNode = &ptPath->pArray[ptAi->adwParam[0]];
					AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK1, SKILL_VAMPIREFIREWALL, NULL, ptNode->nX,
						ptNode->nY);
					ptAi->adwParam[0]++;
					ptAi->adwParam[1] = (DWORD)ptGame->nFrame;
					return;
				}
				if (ABS(ptGame->nFrame - (int)ptAi->adwParam[1]) > 700)
					ptAi->adwParam[0] = 0;
			}
			AiStand(ptGame, hUnit, 10);
			return;
		}
		MonsterModeSetOverride(MONDATA(hUnit)->ptModeData, 1, 0, 0);
		if (AiRunToPoint(ptGame, hUnit, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2))
			return;
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
	}
	ptAi = ptParms->ptAi;
	ptPath = (PRESETFILELIST *)ptAi->pCell;
	if (ptPath && (int)ptAi->adwParam[0] < ptPath->nNodes)
	{
		ptNode = &ptPath->pArray[ptAi->adwParam[0]];
		AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK1, SKILL_VAMPIREFIREWALL, NULL, ptNode->nX, ptNode->nY);
		ptAi->adwParam[0]++;
		ptAi->adwParam[1] = (DWORD)ptGame->nFrame;
		return;
	}
	if (ABS(ptGame->nFrame - (int)ptAi->adwParam[1]) > 700)
		ptAi->adwParam[0] = 0;
	if (bInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < (DWORD)(ptMonStats->AIParam[1] + 10))
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
	{
		AiSetVelocity(hUnit, 0, 100, 0);
		AiRunToTarget(ptGame, hUnit, hTarget);
		return;
	}
	AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
}

void __fastcall AiThinkDarkSpearwoman(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	int nDist;
	BOOL bInReach;
	MONSTATSTXT *ptMonStats;
	MONSTERSCALE tScale;
	DWORD dwChance;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	bInReach = ptParms->fTargetInReach;
	ptMonStats = ptParms->ptMonStats;
	if (nDist > 15)
	{
		D2CommonPathSetStopDistance(hUnit->ptPath, ptMonStats->bMeleeRng);
		AiSetVelocity(hUnit, 13, 100, 0);
		AiRunToTargetStep(ptGame, hUnit, hTarget, ptMonStats->bMeleeRng);
		return;
	}
	ptSeed = &hUnit->tSeed;
	if (bInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
	{
		MonsterGetScale(&tScale, ptGame, D2CommonUnitGetPathRoom(hUnit), (DWORD)hUnit);
		dwChance = (DWORD)(tScale.nDifficulty * 15);
		D2CommonPathSetStopDistance(hUnit->ptPath, ptMonStats->bMeleeRng);
		if (D2CommonSeedRand(ptSeed, 100) < dwChance)
		{
			AiSetVelocity(hUnit, 13, 100, 0);
			AiRunToTargetStep(ptGame, hUnit, hTarget, ptMonStats->bMeleeRng);
			return;
		}
		AiWalkToTargetStep(ptGame, hUnit, hTarget, 3);
		return;
	}
	AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
}

void __fastcall AiThinkFallen(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	DWORD dwMode;
	int nX;
	int nY;
	GAMEROOM **phNear;
	int nNear;
	DWORD i;
	DWORD *pdwRing;
	UNIT *hFound;
	int nDX;
	int nDY;
	int nBodyDist;
	int nDist;
	BOOL bInReach;
	UNIT *hTarget;
	MONSTATSTXT *ptMonStats;
	AICOMMAND *ptCommand;
	AICOMMAND tCommand;
	D2SEED *ptSeed;

	ptAi = ptParms->ptAi;
	dwMode = hUnit->dwMode;
	ptSeed = &hUnit->tSeed;
	if (!dwMode || dwMode == MONMODE_DEAD)
		return;
	EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	D2CommonDungeonRoomGetNear(D2CommonUnitGetPathRoom(hUnit), &phNear, &nNear);
	while (nNear)
	{
		pdwRing = (*phNear)->adwRing;
		for (i = 0; i < 4; pdwRing++, i++)
		{
			hFound = SUnitFind(ptGame, UNIT_MONSTER, *pdwRing);
			if (!hFound || hFound->dwMode)
				continue;
			nDX = (int)D2CommonUnitGetX(hFound) - nX;
			nDY = (int)D2CommonUnitGetY(hFound) - nY;
			nDX = ABS(nDX);
			nDY = ABS(nDY);
			if (nDX > nDY)
				nBodyDist = (nDY + nDX * 2) >> 1;
			else
				nBodyDist = (nDX + nDY * 2) >> 2;
			if ((DWORD)nBodyDist < 20)
			{
				ptAi->adwParam[0] = 1;
				AiPopCommand(ptGame, hUnit);
				AiSetVelocity(hUnit, 0, 50, 0);
				if (AiWalkAway(ptGame, hUnit, ptParms->hTarget, 12, TRUE))
				{
					if (!D2CommonSeedRand(ptSeed, 12))
						SUnitSetEvent(hUnit, 16, NULL);
					return;
				}
				nNear = 1;
				break;
			}
		}
		phNear++;
		nNear--;
	}
	if (dwMode != 1)
	{
		AiStand(ptGame, hUnit, 10);
		return;
	}
	nDist = ptParms->nDist;
	bInReach = ptParms->fTargetInReach;
	hTarget = ptParms->hTarget;
	ptMonStats = ptParms->ptMonStats;
	ptCommand = AiGetCommand(hUnit);
	if (!ptCommand)
	{
		if (!bInReach && MonsterTestPrevModeGetHit(hUnit))
		{
			AiWalkToTarget(ptGame, hUnit, hTarget, 0);
			return;
		}
		if (nDist < 15 && AiGetOwner(hUnit) == hUnit
			&& D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
		{
			tCommand.nCommand = 1;
			AiCommandMinions(ptGame, hUnit, &tCommand);
			AiPushCommand(ptGame, hUnit, &tCommand);
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_SKILL2, hTarget);
			return;
		}
		if (bInReach)
		{
			if (!ptAi->adwParam[0]
				&& D2CommonSeedRand(ptSeed, 100) >= ptMonStats->AIParam[3])
			{
				if (D2CommonSeedRand(ptSeed, 100) < 30)
				{
					AiSetModeOnTarget(ptGame, hUnit, MONMODE_SKILL2, hTarget);
					return;
				}
				AiStand(ptGame, hUnit, 10);
				return;
			}
			ptAi->adwParam[0] = 0;
			if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			else
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
			return;
		}
		if (nDist > ptMonStats->AIParam[2])
		{
			if (D2CommonSeedRand(ptSeed, 100) < 30)
			{
				AiStepAside(ptGame, hUnit, 3);
				return;
			}
			AiStand(ptGame, hUnit, 10);
			return;
		}
		AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
		return;
	}
	if (ptCommand->nCommand == 1)
	{
		if (bInReach)
		{
			if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[3])
			{
				if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
					AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
				else
					AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
				return;
			}
			AiStand(ptGame, hUnit, 5);
			return;
		}
		if (!AiWalkToTarget(ptGame, hUnit, hTarget, 0))
			AiPopCommand(ptGame, hUnit);
		return;
	}
	AiPopCommand(ptGame, hUnit);
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkHollowOne(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	int nDist;
	MONSTATSTXT *ptMonStats;
	AIFINDRECORD tFind;
	UNIT *hNear;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptSeed = &hUnit->tSeed;
	nDist = ptParms->nDist;
	ptMonStats = ptParms->ptMonStats;
	if (ptParms->fTargetInReach && D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	if (nDist < 5 && D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
		return;
	}
	tFind.tRaise.hDead = NULL;
	tFind.tRaise.nDead = 0;
	tFind.tRaise.hHurt = NULL;
	tFind.tRaise.nHurt = 0;
	tFind.tRaise.nFound = 0;
	AiFind(ptGame, hUnit, &tFind, AiThinkFindUndeadFellow, 1);
	{
		UNIT *hRaise;
		int nChance;

		hRaise = tFind.tRaise.hHurt;
		if (tFind.tRaise.nHurt)
		{
			nChance = ptMonStats->AIParam[2];
			if (D2CommonSeedRand(ptSeed, 100) < nChance)
			{
				D2CommonUnitSetTarget(hUnit, hRaise);
				AiUseSkill(ptGame, hUnit, SKILL_BESTOW, hRaise, 0, 0);
				return;
			}
		}
		hRaise = tFind.tRaise.hDead;
		if (tFind.tRaise.nDead)
		{
			nChance = ptMonStats->AIParam[1];
			if (D2CommonSeedRand(ptSeed, 100) < nChance && MonsterCanUseSkill(ptGame, hUnit, 181, hRaise, 0, 0))
			{
				D2CommonUnitSetTarget(hUnit, hRaise);
				AiUseSkill(ptGame, hUnit, SKILL_RESURRECT, hRaise, 0, 0);
				return;
			}
		}
	}
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[3])
	{
		DWORD dwNearDir;
		hNear = AiFindTargetInSight(ptGame, hUnit, &nDist, &dwNearDir);
		if (hNear)
		{
			AiUseSkill(ptGame, hUnit, SKILL_UNHOLYBOLT, hNear, 0, 0);
			return;
		}
	}
	if (!tFind.tRaise.nFound)
	{
		AiSetVelocity(hUnit, 0, 50, 0);
		AiWalkToTargetStep(ptGame, hUnit, hTarget, 3);
		return;
	}
	AiWalkToTargetFast(ptGame, hUnit, hTarget, 3, FALSE);
}

UNIT * __fastcall AiThinkFindUndeadFellow(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind)
{
	MONSTATSTXT *ptMonStats;
	DWORD dwRange;
	DWORD dwMode;
	int nLife;

	if (hCandidate->eType != UNIT_MONSTER || hUnit == hCandidate)
		return NULL;
	if (!D2CommonStatsUnitsTestSameAlignment(hUnit, hCandidate))
		return NULL;
	if (!D2CommonStatsUnitGetAlignment(hCandidate))
		return NULL;
	ptMonStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(hCandidate->dwClassId);
	if (hUnit->dwClassId == 229)
	{
		A2Q1MonsterThink(ptGame, hUnit);
		dwRange = 35;
		if (!ptMonStats->bLUndead && !ptMonStats->bHUndead)
			return NULL;
	}
	else
	{
		dwRange = 25;
		if (!ptMonStats->bLUndead)
			return NULL;
	}
	if (MonsterUniqueIsCounted(MONDATA(hCandidate)->ptUnique))
		return NULL;
	if ((DWORD)AiThinkDistance(hUnit, hCandidate) > dwRange)
		return NULL;
	if (!(hCandidate->dwFlags0 & 2))
		return NULL;
	if (D2CommonStatsUnitTestStateGroupC(hCandidate))
		return NULL;
	ptFind->tRaise.nFound++;
	dwMode = hCandidate->dwMode;
	if (dwMode == MONMODE_DEAD)
	{
		ptFind->tRaise.hDead = hCandidate;
		ptFind->tRaise.nDead++;
		return NULL;
	}
	nLife = D2CommonStatsUnitGetValue(hCandidate, 6);
	if (D2CommonStatsUnitCalcMaxHp(hCandidate) != nLife && dwMode)
	{
		ptFind->tRaise.hHurt = hCandidate;
		ptFind->tRaise.nHurt++;
	}
	return NULL;
}

void __fastcall AiThinkSandRaider(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	UNIT *hTarget;
	MONSTATSTXT *ptMonStats;
	int nDist;
	BOOL bInReach;
	AIFINDRECORD tFind;
	DWORD dwChance;

	ptAi = ptParms->ptAi;
	hTarget = ptParms->hTarget;
	ptMonStats = ptParms->ptMonStats;
	nDist = ptParms->nDist;
	bInReach = ptParms->fTargetInReach;
	ptMonStats = ptParms->ptMonStats;
	if (!ptAi->adwParam[0])
	{
		D2CommonStatsUnitSetActiveStateBit(hUnit, 90, 0);
		D2CommonStatsUnitSetActiveStateBit(hUnit, 91, 0);
		ptAi->adwParam[1] = 0;
	}
	ptAi->adwParam[0]++;
	if ((int)ptAi->adwParam[0] == ptMonStats->AIParam[4])
	{
		if (hUnit->dwClassId != 30 && hUnit->dwClassId != 32)
			D2CommonUnitAddAnimOverlay(hUnit, 46, 0);
		else
			D2CommonUnitAddAnimOverlay(hUnit, 150, 0);
	}
	else if ((int)ptAi->adwParam[0] > ptMonStats->AIParam[4])
	{
		if (hUnit->dwClassId != 30 && hUnit->dwClassId != 32)
			D2CommonStatsUnitSetActiveStateBit(hUnit, 91, 1);
		else
			D2CommonStatsUnitSetActiveStateBit(hUnit, 90, 1);
		ptAi->adwParam[1] = 1;
	}
	if ((int)ptAi->adwParam[2] < 10 && (DWORD)MonsterGetLifePercent(hUnit) < ptMonStats->AIParam[0])
	{
		tFind.tUnit.hUnit = NULL;
		tFind.tUnit.nDist = 2147483647;
		AiFind(ptGame, hUnit, &tFind, AiThinkFindNearFellow, 1);
		if (tFind.tUnit.hUnit)
		{
			AiWalkToTarget(ptGame, hUnit, tFind.tUnit.hUnit, 0);
			return;
		}
		ptAi->adwParam[2]++;
	}
	dwChance = ptMonStats->AIParam[1] - ptAi->adwParam[1] * 35;
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < dwChance && nDist > 3)
	{
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 0, FALSE);
		return;
	}
	if (bInReach)
	{
		if (ptAi->adwParam[1] == 1)
		{
			AiUseSkill(ptGame, hUnit, SKILL_MONSTER_START, hTarget, 0, 0);
			ptAi->adwParam[0] = 0;
			ptAi->adwParam[1] = 0;
			return;
		}
		if (D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[2])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
	}
	else if (D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[1])
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, 0);
		return;
	}
	if ((int)ptAi->adwParam[0] > ptMonStats->AIParam[4] + 6)
	{
		ptAi->adwParam[0] = 0;
		ptAi->adwParam[1] = 0; }
	AiStand(ptGame, hUnit, ptMonStats->AIParam[3]);
}

UNIT * __fastcall AiThinkFindNearFellow(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind)
{
	int nDist;

	if (hCandidate->eType == UNIT_MONSTER && hCandidate->dwMode && hCandidate->dwMode != MONMODE_DEAD)
	{
		nDist = AiThinkDistance(hUnit, hCandidate);
		if (nDist >= 3 && nDist < ptFind->tUnit.nDist)
		{
			ptFind->tUnit.hUnit = hCandidate;
			ptFind->tUnit.nDist = nDist;
		}
	}
	return NULL;
}

void __fastcall AiThinkQuillRat(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	int nDist;
	UNIT *hTarget;
	BOOL bInReach;
	AICOMMAND *ptCommand;
	MONSTATSTXT *ptMonStats;
	BYTE bRange;

	nDist = ptParms->nDist;
	hTarget = ptParms->hTarget;
	bInReach = ptParms->fTargetInReach;
	ptCommand = AiGetCommand(hUnit);
	if (ptCommand)
	{
		if (SUnitFind(ptGame, ptCommand->dwArg1, ptCommand->dwArg2))
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
			AiPopCommand(ptGame, hUnit);
			return;
		}
		AiPopCommand(ptGame, hUnit);
	}
	if (bInReach)
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	if (MonsterTestPrevModeGetHit(hUnit))
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
		return;
	}
	ptMonStats = ptParms->ptMonStats;
	if (nDist < ptMonStats->AIParam[0])
	{
		if (D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[1])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
			return;
		}
		if (AiWalkAway(ptGame, hUnit, hTarget, ptMonStats->AIParam[3], TRUE))
			return;
		if (nDist < 4)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
			return;
		}
	}
	bRange = 3;
	if (ptMonStats->AIParam[3] >= 3)
		bRange = ptMonStats->AIParam[3];
	AiStepAside(ptGame, hUnit, bRange);
}

void __fastcall AiThinkQuillBear(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	MONSTATSTXT *ptMonStats;
	BOOL bInReach;

	hTarget = ptParms->hTarget;
	bInReach = ptParms->fTargetInReach;
	ptMonStats = ptParms->ptMonStats;
	if (MonsterTestPrevModeGetHit(hUnit))
	{ AICOMMAND tCommand;
		tCommand.dwArg1 = hTarget->eType;
		tCommand.nCommand = 1;
		tCommand.dwArg2 = hTarget->dwId;
		AiCommandMinions(ptGame, hUnit, &tCommand);
		if (bInReach)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
		return;
	}
	if (bInReach)
	{
		if (D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[0])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
		return;
	}
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[1])
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, AIMOVE_ALL);
		return;
	}
	AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
}

void __fastcall AiThinkZombie(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	BOOL bInReach;
	int nDist;
	UNIT *hTarget;
	MONSTATSTXT *ptMonStats;
	MONSTERSCALE tScale;
	int nNear;
	DWORD dwChance;
	BOOL bRun;
	D2SEED *ptSeed;

	bInReach = ptParms->fTargetInReach;
	ptSeed = &hUnit->tSeed;
	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	ptMonStats = ptParms->ptMonStats;
	MonsterGetScale(&tScale, ptGame, D2CommonUnitGetPathRoom(hUnit), (DWORD)hUnit);
	if (bInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
		return;
	}
	nNear = ptMonStats->AIParam[0] + tScale.nDifficulty * 5;
	bRun = FALSE;
	dwChance = (DWORD)(ptMonStats->AIParam[1] + tScale.nDifficulty * 10);
	if (MonsterTestPrevModeGetHit(hUnit))
		bRun = TRUE;
	if (nDist < nNear && D2CommonSeedRand(ptSeed, 100) < dwChance)
		bRun = TRUE;
	if (D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hUnit)) == 17)
		bRun = TRUE;
	if ((BOOL)(hUnit->dwClassId == 8 || bRun))
	{
		AiSetVelocity(hUnit, 0, 100, 0);
		AiRunToTarget(ptGame, hUnit, hTarget);
		return;
	}
	AiStepAside(ptGame, hUnit, 3);
}

void __fastcall AiThinkFallenShaman(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	BOOL bInReach;
	int nDist;
	UNIT *hTarget;
	MONSTATSTXT *ptMonStats;
	AIFINDRECORD tFind;
	AICOMMAND tCommand;
	UNIT *hNear;
	DWORD dwNearDir;
	D2SEED *ptSeed;

	bInReach = ptParms->fTargetInReach;
	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	if (bInReach)
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	ptSeed = &hUnit->tSeed;
	ptMonStats = ptParms->ptMonStats;
	tFind.tDead.hUnit = NULL;
	tFind.tDead.nCount = 0;
	tFind.tDead.nMax = 24;
	AiFind(ptGame, hUnit, &tFind, NULL, 9);
	hNear = tFind.tDead.hUnit;
	if (hNear && tFind.tDead.nCount
		&& D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
	{
		tCommand.nCommand = 1;
		AiCommandMinions(ptGame, hUnit, &tCommand);
		if (MonsterCanUseSkill(ptGame, hUnit, 181, hNear, 0, 0))
		{
			AiUseSkill(ptGame, hUnit, SKILL_RESURRECT, hNear, 0, 0);
			return;
		}
		if (nDist < 15)
		{
			AiUseSkill(ptGame, hUnit, SKILL_SHAMANFIRE, hTarget, 0, 0);
			return;
		}
	}
	hNear = AiFindTargetInSight(ptGame, hUnit, &nDist, &dwNearDir);
	if (hNear && nDist < 15 && D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
	{
		tCommand.nCommand = 1;
		AiCommandMinions(ptGame, hUnit, &tCommand);
		AiUseSkill(ptGame, hUnit, SKILL_SHAMANFIRE, hNear, 0, 0);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2])
	{
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 3, FALSE);
		return;
	}
	AiStand(ptGame, hUnit, 10);
}

static BYTE gabAiThinkQuarter[64] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0,
};
static BYTE gabAiThinkQuarterDir[4] = {
	31, 49, 0, 17,
};

void __fastcall AiThinkGargoyleTrap(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	int nDist;
	MONSTATSTXT *ptMonStats;
	AIGENERAL *ptAi;
	int nTargetX;
	int nTargetY;
	int nX;
	int nY;
	int nDX, nDY;
	BYTE bDir;
	D2SEED *ptSeed;
	int nAbsX;
	int nPX, nPY;

	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	ptMonStats = ptParms->ptMonStats;
	ptAi = ptParms->ptAi;
	if (ptAi->adwParam[0])
	{
		ptAi->adwParam[0] = 0;
		AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
		return;
	}
	D2CommonUnitSetTarget(hUnit, hTarget);
	nTargetX = (int)D2CommonUnitGetX(hTarget);
	nTargetY = (int)D2CommonUnitGetY(hTarget);
	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	nDX = nTargetX;
	nTargetX -= nX;
	nDY = nTargetY;
	nAbsX = ABS(nTargetX);
	nTargetY -= nY;
	if (nAbsX < ABS(nTargetY))
	{
		nPX = nX;
		nPY = nDY;
	}
	else
	{
		nPX = nDX;
		nPY = nY;
	}
	ptSeed = &hUnit->tSeed;
	bDir = (BYTE)D2CommonUnitSetPositionExtra(hUnit, nPX, nPY);
	D2CommonPathSetFacingImmediate(hUnit->ptPath, gabAiThinkQuarterDir[gabAiThinkQuarter[bDir]]);
	if ((ABS(nTargetX) < 6 || (nTargetY < 0 && (nTargetY = -nTargetY), nTargetY < 6)) && nDist < ptMonStats->AIParam[0]
		&& D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
	{
		AiUseSkill(ptGame, hUnit, SKILL_GARGOYLETRAP, hTarget, 0, 0);
		ptAi->adwParam[0] = 1;
		return;
	}
	AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
}

void __fastcall AiThinkFireTower(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	AIGENERAL *ptAi;
	MONSTATSTXT *ptMonStats;
	int nTargetX;
	int nTargetY;
	int nX;
	int nY;
	BYTE bDir;
	BYTE bTurn;
	BYTE bNew;
	int nDiff;
	int nAheadX;
	int nAheadY;

	hTarget = ptParms->hTarget;
	ptMonStats = ptParms->ptMonStats;
	ptAi = ptParms->ptAi;
	if (ptAi->adwParam[0])
	{
		ptAi->adwParam[0] = 0;
		AiStand(ptGame, hUnit, ptMonStats->AIParam[4]);
		return;
	}
	if (!ptAi->adwParam[1])
	{
		ptAi->adwParam[1] = 1;
		AiUseSkillInMode(ptGame, hUnit, 14, SKILL_DESERTTURRET, NULL, 0, 0);
		ptAi->adwParam[2] = 0;
		AiStand(ptGame, hUnit, ptMonStats->AIParam[4]);
		return;
	}
	nTargetX = (int)D2CommonUnitGetX(hTarget);
	nTargetY = (int)D2CommonUnitGetY(hTarget);
	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	bDir = (BYTE)D2CommonUnitSetPositionExtra(hUnit, nTargetX, nTargetY);
	bDir = D2CommonMonsterRegionLookupTableB(bDir);
	bNew = (BYTE)ptAi->adwParam[2];
	bTurn = bNew;
	if (bDir != bNew)
	{
		nDiff = AiMoveAbs((int)bDir - (int)bNew);
		if (nDiff < 4)
		{
			if (bTurn < bDir)
				bTurn++;
			else
				bTurn--;
		}
		else if (bTurn == 7)
			goto lWrap;
		else if (bTurn == 0)
		{
			bNew = 7;
			goto lStore;
		}
		else if (bTurn < 4)
			bTurn--;
		else
			bTurn++;
		bNew = bTurn;
		if (bTurn == 8)
		{
lWrap:
			bNew = 0;
		}
		else if (bTurn == 255)
			bNew = 7;
		else if (bTurn > 7)
			bNew = 0;
lStore:
		ptAi->adwParam[2] = bNew;
	}
	{
		signed char acStepX[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
		signed char acStepY[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };

		nAheadX = nX + (acStepX[bNew] << 4);
		nAheadY = nY + (acStepY[bNew] << 4);
	}
	D2CommonUnitSetTarget(hUnit, hTarget);
	if (MonsterCanUseSkill(ptGame, hUnit, 203, hTarget, nAheadX, nAheadY))
	{
		nTargetY = (int)D2CommonUnitGetY(hTarget);
		if (MonsterCanUseSkill(ptGame, hUnit, 203, hTarget, (int)D2CommonUnitGetX(hTarget), nTargetY))
			AiUseSkillInMode(ptGame, hUnit, 14, SKILL_DESERTTURRET, NULL, nAheadX, nAheadY);
	}
	AiThinkIn(ptGame, hUnit, ptMonStats->AIParam[4]);
}

void __fastcall AiThinkLightningSpire(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	MONSTATSTXT *ptMonStats;
	UNIT *hTarget;

	hTarget = ptParms->hTarget;
	ptMonStats = ptParms->ptMonStats;
	ptAi = ptParms->ptAi;
	if (!ptAi->adwParam[0])
	{
		if (!ptAi->adwParam[1])
		{
			AiUseSkillInMode(ptGame, hUnit, 14, SKILL_ARCANETOWER, hTarget, 0, 0);
			AiThinkIn(ptGame, hUnit, ptMonStats->AIParam[4]);
			ptAi->adwParam[0] = 1;
			ptAi->adwParam[1] = 1;
			return;
		}
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		ptAi->adwParam[0] = 1;
		ptAi->adwParam[1] = 0;
		return;
	}
	ptAi->adwParam[0] = 0;
	AiStand(ptGame, hUnit, 150);
}

void __fastcall AiThinkAttackThenDie(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	MONSTATSTXT *ptMonStats;
	UNIT *hTarget;
	AIGENERAL *ptAi;
	int nDist;
	BOOL bInReach; D2SEED *ptSeed;

	ptSeed = &hUnit->tSeed; hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	bInReach = ptParms->fTargetInReach;
	ptMonStats = ptParms->ptMonStats;
	ptAi = ptParms->ptAi;
	if (ptAi->adwParam[0] == 2)
	{
		if (hTarget)
			SUnitDmgKill(ptGame, hUnit, hTarget, TRUE);
		else
			SUnitDmgKill(ptGame, hUnit, NULL, TRUE);
		return;
	}
	if (nDist < 16 && !ptAi->adwParam[0])
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_SKILL1, hTarget);
		ptAi->adwParam[0] = 1;
		return;
	}
	if (!bInReach && ptAi->adwParam[0] == 1 && D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, 0);
		return;
	}
	if (!bInReach && ptAi->adwParam[0] == 1)
	{
		AiStepAside(ptGame, hUnit, 8);
		return;
	}
	if (bInReach)
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		EventAdd(ptGame, hUnit, 0, ptGame->nFrame + 1);
		ptAi->adwParam[0] = 2;
		return;
	}
	AiStand(ptGame, hUnit, ptMonStats->AIParam[2]);
}

void __fastcall AiThinkDriedCorpse(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	BOOL bInReach;
	int nDist;
	UNIT *hTarget;
	MONSTATSTXT *ptMonStats;
	D2SEED *ptSeed;

	nDist = ptParms->nDist;
	bInReach = ptParms->fTargetInReach;
	hTarget = ptParms->hTarget;
	if (MonsterTestPrevModeGetHit(hUnit) && !bInReach)
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, 7);
		return;
	}
	ptMonStats = ptParms->ptMonStats;
	ptSeed = &hUnit->tSeed;
	if (nDist > ptMonStats->AIParam[0])
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
			AiStepAside(ptGame, hUnit, 3);
		else
			AiStand(ptGame, hUnit, 10);
		return;
	}
	if (bInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2])
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		else
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
	}
	else
		AiWalkToTarget(ptGame, hUnit, hTarget, 7);
}

void __fastcall AiThinkSandLeaper(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	int nDist;
	UNIT *hTarget;
	BOOL bInReach;
	MONSTATSTXT *ptMonStats;
	D2SEED *ptSeed;

	nDist = ptParms->nDist;
	hTarget = ptParms->hTarget;
	bInReach = ptParms->fTargetInReach;
	ptMonStats = ptParms->ptMonStats;
	ptSeed = &hUnit->tSeed;
	if (nDist < 5 && D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0]
		&& MonsterCanUseSkill(ptGame, hUnit, 132, hTarget, 0, 0))
	{
		AiUseSkill(ptGame, hUnit, SKILL_LEAP, hTarget, 0, 0);
		return;
	}
	if (bInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
			return;
		}
	}
	else if (nDist > 10)
	{
		AiSetVelocity(hUnit, 0, 75, 0);
		AiWalkAround(ptGame, hUnit, hTarget, 5);
		return;
	}
	else if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2])
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, 7);
		return;
	}
	else if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[3])
	{
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
		return;
	}
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkItchies(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptSeed = &hUnit->tSeed;
	if (ptParms->fTargetInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < 50)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
	}
	else if (D2CommonSeedRand(ptSeed, 100) < 60)
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, 7);
		return;
	}
	AiStand(ptGame, hUnit, 20);
}

void __fastcall AiThinkFoulCrowNestInit(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi = ptParms->ptAi;

	ptAi->adwParam[0] = (DWORD)ptGame->nFrame;
	ptAi->adwParam[1] = 0;
}

void __fastcall AiThinkFoulCrowNest(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	int nDist;
	AIGENERAL *ptAi;
	MONSTATSTXT *ptMonStats;
	int nFrame;
	int nX;
	int nY;
	GAMEROOM *hRoom;

	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	ptAi = ptParms->ptAi;
	ptMonStats = ptParms->ptMonStats;
	if (nDist <= 20)
	{
		if ((int)ptAi->adwParam[1] >= ptMonStats->AIParam[2])
		{
			hUnit->dwFlags0 |= 131072;
			AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
			return;
		}
		nFrame = ptGame->nFrame;
		if (ABS(nFrame - (int)ptAi->adwParam[0]) >= ptMonStats->AIParam[0])
		{
			ptAi->adwParam[0] = (DWORD)nFrame;

			if (MonsterHasRoom(206, ((GAMEROOM *)D2CommonUnitGetPathRoom(hUnit)), ((int)D2CommonUnitGetX(hUnit)), ((int)D2CommonUnitGetY(hUnit)), FALSE))
			{
				ptAi->adwParam[1]++;
				AiUseSkill(ptGame, hUnit, SKILL_NEST, hTarget, 0, 0);
				return;
			}
		}
	}
	AiStand(ptGame, hUnit, 20);
}

void __fastcall AiThinkDuriel(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	BOOL bInReach;
	int nRoll;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	bInReach = ptParms->fTargetInReach;
	ptSeed = &hUnit->tSeed;
	if (!D2CommonSkillsFindByKey(hUnit, 114))
	{
		SkillsSetLevel(hUnit, SKILL_HOLY_FREEZE, 5, 1);
		SkillsSetCurrent(hUnit, FALSE, SKILL_HOLY_FREEZE);
	}
	if (bInReach)
	{
		nRoll = (int)D2CommonSeedRand(ptSeed, 100);
		if (nRoll < 33)
		{
			if (D2CommonSeedRand(ptSeed, 100) < 50)
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			else
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
			return;
		}
		if (nRoll < 66)
		{
			AiUseSkill(ptGame, hUnit, SKILL_JAB, hTarget, 0, 0);
			return;
		}
		if (nRoll < 100)
		{
			AiUseSkill(ptGame, hUnit, SKILL_SMITE, hTarget, 0, 0);
			return;
		}
	}
	if (D2CommonSeedRand(ptSeed, 100) < 60)
	{
		AiUseSkill(ptGame, hUnit, SKILL_CHARGE, hTarget, 0, 0);
		return;
	}
	AiSetVelocity(hUnit, 13, 0, 0);
	AiWalkToTarget(ptGame, hUnit, hTarget, 7);
}

void __fastcall AiThinkBloodravenInit(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	ptParms->ptAi->adwParam[2] = 0;
}

void __fastcall AiThinkBloodraven(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	int nDist;
	BOOL bInReach;
	AICOMMAND *ptCommand;
	AICOMMAND tCommand;
	AIGENERAL *ptAi;
	int nPlaceDist;
	int nLimit;
	int nRange;
	int nOffX;
	int nOffY;
	int nX;
	int nY;
	UNIT *hFound;
	int nFoundDist;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	bInReach = ptParms->fTargetInReach;
	ptCommand = AiFindCommand(hUnit, 10, FALSE);
	if (!ptCommand)
	{
		tCommand.nCommand = 10;
		tCommand.dwArg1 = D2CommonUnitGetX(hUnit);
		tCommand.dwArg2 = D2CommonUnitGetY(hUnit);
		AiPushCommand(ptGame, hUnit, &tCommand);
		ptCommand = AiFindCommand(hUnit, 10, FALSE);
	}
	ptAi = ptParms->ptAi;
	nPlaceDist = 0;
	if (ptCommand)
	{
		nPlaceDist = AiThinkEdgeDistance(hTarget, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2);
		if (nDist > 45)
		{
			AiStand(ptGame, hUnit, 5);
			return;
		}
		if (nPlaceDist >= 50 || (DWORD)AiThinkEdgeDistance(hUnit, (int)ptCommand->dwArg1,
			(int)ptCommand->dwArg2) > 50)
		{
			ptAi->adwParam[2] = 1;
			AiSetVelocity(hUnit, 7, 100, 0);
			if (AiRunToPoint(ptGame, hUnit, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2))
				return;
			EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
		}
		if (ptAi->adwParam[2]
			&& (DWORD)AiThinkEdgeDistance(hUnit, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2) > 5)
		{
			AiSetVelocity(hUnit, 7, 100, 0);
			if (AiRunToPoint(ptGame, hUnit, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2))
				return;
			EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
		}
		ptAi->adwParam[2] = 0;
	}
	if (nDist > 20 && nPlaceDist < 50)
	{
		nDist >>= 1;
		if (nDist < 12)
			nDist = 12;
		AiSetVelocity(hUnit, 7, 100, 0);
		if (AiRunAround(ptGame, hUnit, hTarget, (BYTE)nDist))
			return;
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
	}
	ptSeed = &hUnit->tSeed;
	nLimit = ptGame->bDifficultyLevel * 2 + 8;
	ptAi->adwParam[0] += 3;
	if (!bInReach && (int)ptAi->adwParam[1] < nLimit
		&& D2CommonSeedRand(ptSeed, 100) < ptAi->adwParam[0])
	{
		nRange = (int)D2CommonSeedRand(ptSeed, 15) + 5;
		if (D2CommonSeedNext(ptSeed) & 1)
		{
			nOffX = nRange;
			if (nRange)
				nOffY = (int)D2CommonSeedRand(ptSeed, nRange);
			else
				nOffY = 0;
		}
		else
		{
			if (nRange)
				nOffX = (int)D2CommonSeedRand(ptSeed, nRange);
			else
				nOffX = 0;
			nOffY = nRange;
		}
		if (D2CommonSeedNext(ptSeed) & 1)
			nOffX = -nOffX;
		if (D2CommonSeedNext(ptSeed) & 1)
			nOffY = -nOffY;
		nY = (int)D2CommonUnitGetY(hTarget) + nOffY;

		AiUseSkill(ptGame, hUnit, SKILL_NEST, NULL, ((int)D2CommonUnitGetX(hTarget) + nOffX), nY);
		ptAi->adwParam[0] = 0;
		ptAi->adwParam[1]++;
		return;
	}
	if (nDist > 5)
	{ DWORD dwFoundDir;
		if (D2CommonSeedRand(ptSeed, 100) < 5 && nPlaceDist < 50)
		{
			AiSetVelocity(hUnit, 7, 100, 0);
			AiRunAround(ptGame, hUnit, hTarget, 12);
			return;
		}
		hFound = AiFindTargetInSight(ptGame, hUnit, &nFoundDist, &dwFoundDir);
		if (hFound && !MonsterTestPrevModeGetHit(hUnit) && D2CommonSeedRand(ptSeed, 100) < 80)
		{
			nLimit = (ptGame->bDifficultyLevel + 4) * 10;
			if (D2CommonSeedRand(ptSeed, 100) < (DWORD)nLimit)
				AiUseSkill(ptGame, hUnit, SKILL_QUICK_STRIKE, hFound, 0, 0);
			else
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hFound);
			return;
		}
		AiSetVelocity(hUnit, 0, 50, 0);
		if (AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, TRUE))
			return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < 30 && nDist < 12)
	{
		AiSetVelocity(hUnit, 7, 100, 0);
		if (AiRunAway(ptGame, hUnit, hTarget, (BYTE)(12 - nDist), FALSE))
			return;
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
	}
	AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
}

void __fastcall AiThinkMummyGenerator(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	AIGENERAL *ptAi;
	int nX;
	int nY;
	GAMEROOM *hRoom;

	hTarget = ptParms->hTarget;
	ptAi = ptParms->ptAi;
	if (ptParms->nDist > 25)
	{
		AiStand(ptGame, hUnit, 20);
		return;
	}
	if ((int)ptAi->adwParam[1] > 16)
	{
		hUnit->dwFlags0 |= 131072;
		AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
		return;
	}
	if (ABS(ptGame->nFrame - (int)ptAi->adwParam[0]) >= 125)
	{

		if (MonsterHasRoom(228, ((GAMEROOM *)D2CommonUnitGetPathRoom(hUnit)), ((int)D2CommonUnitGetX(hUnit)), ((int)D2CommonUnitGetY(hUnit)), FALSE))
		{
			ptAi->adwParam[0] = (DWORD)ptGame->nFrame;
			ptAi->adwParam[1]++;
			AiUseSkill(ptGame, hUnit, SKILL_NEST, hTarget, 0, 0);
			return;
		}
	}
	AiStand(ptGame, hUnit, (int)D2CommonSeedRand(&hUnit->tSeed, 10) + 20);
}

void __fastcall AiThinkSuckerNest(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	AIGENERAL *ptAi;
	MONSTATSTXT *ptMonStats;
	int nX;
	int nY;
	GAMEROOM *hRoom;

	hTarget = ptParms->hTarget;
	ptAi = ptParms->ptAi;
	ptMonStats = ptParms->ptMonStats;
	if (ptParms->nDist <= 25)
	{
		if ((int)ptAi->adwParam[1] > 16)
		{
			hUnit->dwFlags0 |= 131072;
			AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
			return;
		}
		if (ABS(ptGame->nFrame - (int)ptAi->adwParam[0]) >= ptMonStats->AIParam[0])
		{

			if (MonsterHasRoom(334, ((GAMEROOM *)D2CommonUnitGetPathRoom(hUnit)), ((int)D2CommonUnitGetX(hUnit)), (int)D2CommonUnitGetY(hUnit), FALSE))
			{
				ptAi->adwParam[0] = (DWORD)ptGame->nFrame;
				ptAi->adwParam[1]++;
				AiUseSkill(ptGame, hUnit, SKILL_NEST, hTarget, 0, 0);
				return;
			}
		}
		AiStand(ptGame, hUnit, 20); return; }
	AiStand(ptGame, hUnit, 20);
}

void __fastcall AiThinkDesertWingOut(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;

	ptAi = ptParms->ptAi;
	if (hUnit->dwMode == 11)
	{
		ptAi->adwParam[0] = 1;
		AiThinkIn(ptGame, hUnit, 1);
		return;
	}
	if (ptAi->adwParam[0] == 4)
	{
		ptAi->adwParam[0] = 1;
		AiSetModeOnTarget(ptGame, hUnit, 10, NULL);
		return;
	}
	AiThinkSet(ptGame, hUnit, ptAi, ptAi->nThinkKind);
	AiStand(ptGame, hUnit, 1);
}

void __fastcall AiThinkDesertWing(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	UNIT *hTarget;
	int nDist;
	BOOL bInReach;
	DWORD dwStep;
	D2SEED *ptSeed;

	ptAi = ptParms->ptAi;
	ptSeed = &hUnit->tSeed;
	nDist = ptParms->nDist;
	bInReach = ptParms->fTargetInReach;
	hTarget = ptParms->hTarget;
	dwStep = ptAi->adwParam[0];
	if (dwStep == 0 || dwStep == 4)
	{
		AiSetModeOnTarget(ptGame, hUnit, 10, NULL);
		AiThinkIn(ptGame, hUnit, 8);
		ptAi->adwParam[0] = 1;
		return;
	}
	if (dwStep == 1)
	{
		if (hTarget && nDist <= 10 && D2CommonSeedRand(ptSeed, 100) >= 80)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_SKILL2, NULL);
			ptAi->adwParam[0] = 2;
			return;
		}
		AiSetModeOnTarget(ptGame, hUnit, 11, NULL);
		AiThinkIn(ptGame, hUnit, 1);
		return;
	}
	if (dwStep == 2)
	{
		if ((int)ptAi->adwParam[1] < 20 && D2CommonUnitCalcLifePercent(hUnit) < 35
			&& D2CommonSeedRand(ptSeed, 100) < 33)
		{
			AiWalkAway(ptGame, hUnit, hTarget, 14, FALSE);
			ptAi->adwParam[0] = 4;
			ptAi->adwParam[1]++;
			return;
		}
		if (D2CommonSeedRand(ptSeed, 100) < 25)
		{
			AiStepAside(ptGame, hUnit, 6);
			return;
		}
		if (nDist < 8)
		{
			AiWalkToTarget(ptGame, hUnit, hTarget, 0);
			ptAi->adwParam[0] = 3;
			return;
		}
	}
	else if (dwStep == 3)
	{
		if (!bInReach)
		{
			AiWalkToTarget(ptGame, hUnit, hTarget, 0);
			return;
		}
		if (D2CommonSeedRand(ptSeed, 100) < 50)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
			return;
		}
		if (D2CommonUnitCalcLifePercent(hUnit) < 35
			&& D2CommonSeedRand(ptSeed, 100) < 33)
		{
			AiWalkAway(ptGame, hUnit, hTarget, 12, FALSE);
			ptAi->adwParam[0] = 2;
			return;
		}
		if (D2CommonSeedRand(ptSeed, 100) < 66)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
	}
	AiStand(ptGame, hUnit, 30);
}

void __fastcall AiThinkReturnedMage(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	int nNearDist;
	DWORD dwNearDir;
	UNIT *hNear;
	MONSTATSTXT *ptMonStats;
	int nDist;
	MONSTERSCALE tScale;
	int anChance[3];
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptSeed = &hUnit->tSeed;
	nNearDist = ptParms->nDist;
	hNear = AiFindTargetInSight(ptGame, hUnit, &nNearDist, &dwNearDir);
	ptMonStats = ptParms->ptMonStats;
	if (hNear)
	{
		nDist = AiThinkDistance(hUnit, hNear);
		if (nDist > 9 && D2CommonSeedRand(ptSeed, 100) < 30)
		{
			AiSetVelocity(hUnit, 0, 10, 0);
			AiWalkToTargetStep(ptGame, hUnit, hNear, 10);
			return;
		}
		anChance[0] = 0;
		anChance[1] = 30;
		anChance[2] = 50;
		MonsterGetScale(&tScale, ptGame, D2CommonUnitGetPathRoom(hUnit), (DWORD)hUnit);
		if (nDist <= 5
			&& D2CommonSeedRand(ptSeed, 100) < (DWORD)anChance[tScale.nDifficulty])
		{
			AiSetVelocity(hUnit, 0, 25, 0);
			if (AiWalkAway(ptGame, hUnit, hNear, 5, TRUE))
				return;
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		if (nDist < 25 && D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hNear);
			return;
		}
	}
	if (D2CommonSeedRand(ptSeed, 100) < 20)
	{
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
		return;
	}
	AiStand(ptGame, hUnit, 5);
}

BYTE __fastcall AiThinkNpcOnHoldPoint(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms, PRESETPATHNODE *ptNode)
{
	BYTE bWalked;
	BYTE bResult;
	AICOMMAND *ptCommand;

	bResult = FALSE;
	if (hUnit && ptNode && ptNode->nX && ptNode->nY)
	{
		bWalked = FALSE;
		if ((DWORD)AiThinkPointDistance(hUnit, ptNode->nX, ptNode->nY) > 0)
		{
			AiSetVelocity(hUnit, 7, 0, 0);
			AiWalkToPointZeroStop(ptGame, hUnit, ptNode->nX, ptNode->nY);
			ptCommand = AiFindOrAddCommand(ptGame, hUnit, 4, FALSE);
			if (ptCommand)
			{
				ptCommand->dwArg1 = (DWORD)ptNode->nX;
				ptCommand->dwArg2 = (DWORD)ptNode->nY;
				ptCommand->dwArg3 = 12;
				ptCommand->dwArg4 = 10;
			}
			bWalked = TRUE;
		}
		ptCommand = AiFindOrAddCommand(ptGame, hUnit, 4, FALSE);
		if (ptCommand)
		{
			ptCommand->dwArg1 = (DWORD)ptNode->nX;
			ptCommand->dwArg2 = (DWORD)ptNode->nY;
			ptCommand->dwArg3 = 20;
			ptCommand->dwArg4 = 10;
			bResult = bWalked;
			return bResult;
		}
		bResult = bWalked;
	}
	return bResult;
}

BYTE __fastcall AiThinkNpcOnWalkPoint(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms, PRESETPATHNODE *ptNode)
{
	AICOMMAND *ptCommand;
	BYTE bWalked;

	bWalked = FALSE;
	if (hUnit && ptNode && ptNode->nX && ptNode->nY
		&& (DWORD)AiThinkPointDistance(hUnit, ptNode->nX, ptNode->nY) > 0)
	{
		AiSetVelocity(hUnit, 7, 0, 0);
		AiWalkToPointZeroStop(ptGame, hUnit, ptNode->nX, ptNode->nY);
		ptCommand = AiFindOrAddCommand(ptGame, hUnit, 4, FALSE);
		if (ptCommand)
		{
			ptCommand->dwArg1 = (DWORD)ptNode->nX;
			ptCommand->dwArg2 = (DWORD)ptNode->nY;
			ptCommand->dwArg3 = 12;
			ptCommand->dwArg4 = 10;
		}
		bWalked = TRUE;
	}
	return bWalked;
}

BYTE __fastcall AiThinkNpcOnFirstSkillPoint(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms,
	PRESETPATHNODE *ptNode)
{
	BYTE bWalked;
	BYTE bResult;
	AICOMMAND *ptCommand;

	bResult = FALSE;
	if (hUnit && ptNode && ptNode->nX && ptNode->nY)
	{
		bWalked = FALSE;
		if ((DWORD)AiThinkPointDistance(hUnit, ptNode->nX, ptNode->nY) > 0)
		{
			AiSetVelocity(hUnit, 7, 0, 0);
			AiWalkToPointZeroStop(ptGame, hUnit, ptNode->nX, ptNode->nY);
			ptCommand = AiFindOrAddCommand(ptGame, hUnit, 4, FALSE);
			if (ptCommand)
			{
				ptCommand->dwArg1 = (DWORD)ptNode->nX;
				ptCommand->dwArg2 = (DWORD)ptNode->nY;
				ptCommand->dwArg3 = 12;
				ptCommand->dwArg4 = 10;
			}
			bWalked = TRUE;
		}
		ptCommand = AiFindOrAddCommand(ptGame, hUnit, 7, FALSE);
		if (ptCommand)
		{
			if (ptParms->ptMonStats->bModeS1)
				ptCommand->dwArg1 = MONMODE_SKILL1;
			else
			{
				FogErrorManagerLog("NPC: %d tried to skill1", hUnit->dwClassId);
				ptCommand->dwArg1 = 1;
			}
			ptCommand->dwArg2 = (DWORD)ptNode->nX;
			ptCommand->dwArg3 = (DWORD)ptNode->nY;
			ptCommand->dwArg4 = 4;
		}
		ptCommand = AiFindOrAddCommand(ptGame, hUnit, 4, FALSE);
		if (ptCommand)
		{
			ptCommand->dwArg1 = (DWORD)ptNode->nX;
			ptCommand->dwArg2 = (DWORD)ptNode->nY;
			ptCommand->dwArg3 = 12;
			ptCommand->dwArg4 = 10;
		}
		bResult = bWalked;
	}
	return bResult;
}

BYTE __fastcall AiThinkNpcOnSecondSkillPoint(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms,
	PRESETPATHNODE *ptNode)
{
	BYTE bWalked;
	BYTE bResult;
	AICOMMAND *ptCommand;

	bResult = FALSE;
	if (hUnit && ptNode && ptNode->nX && ptNode->nY)
	{
		bWalked = FALSE;
		if ((DWORD)AiThinkPointDistance(hUnit, ptNode->nX, ptNode->nY) > 0)
		{
			AiSetVelocity(hUnit, 7, 0, 0);
			AiWalkToPointZeroStop(ptGame, hUnit, ptNode->nX, ptNode->nY);
			ptCommand = AiFindOrAddCommand(ptGame, hUnit, 4, FALSE);
			if (ptCommand)
			{
				ptCommand->dwArg1 = (DWORD)ptNode->nX;
				ptCommand->dwArg2 = (DWORD)ptNode->nY;
				ptCommand->dwArg3 = 12;
				ptCommand->dwArg4 = 10;
			}
			bWalked = TRUE;
		}
		ptCommand = AiFindOrAddCommand(ptGame, hUnit, 7, FALSE);
		if (ptCommand)
		{
			if (ptParms->ptMonStats->bModeS2)
				ptCommand->dwArg1 = MONMODE_SKILL2;
			else
			{
				FogErrorManagerLog("NPC: %d tried to skill1", hUnit->dwClassId);
				ptCommand->dwArg1 = 1;
			}
			ptCommand->dwArg2 = (DWORD)ptNode->nX;
			ptCommand->dwArg3 = (DWORD)ptNode->nY;
			ptCommand->dwArg4 = 4;
		}
		ptCommand = AiFindOrAddCommand(ptGame, hUnit, 4, FALSE);
		if (ptCommand)
		{
			ptCommand->dwArg1 = (DWORD)ptNode->nX;
			ptCommand->dwArg2 = (DWORD)ptNode->nY;
			ptCommand->dwArg3 = 12;
			ptCommand->dwArg4 = 10;
		}
		bResult = bWalked;
	}
	return bResult;
}

void __fastcall AiThinkNpc(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AICOMMAND *ptCommand;
	DWORD dwBusy;
	DWORD dwIdle;
	GAMECOORD tCoord;

	ptCommand = AiFindOrAddCommand(ptGame, hUnit, 10, FALSE);
	if (ptCommand && !ptCommand->dwArg1 && !ptCommand->dwArg2)
	{
		ptCommand->dwArg1 = D2CommonUnitGetX(hUnit);
		ptCommand->dwArg2 = D2CommonUnitGetY(hUnit);
		AiStand(ptGame, hUnit, 20);
		return;
	}
	switch (hUnit->dwClassId)
	{
	case 265:
		if (A1Q4GetCainWalkSpot(ptGame, hUnit, &tCoord))
		{
			if ((DWORD)AiThinkPointDistance(hUnit, (int)tCoord.dwX, (int)tCoord.dwY) > 2)
				AiWalkToPoint(ptGame, hUnit, (int)tCoord.dwX, (int)tCoord.dwY);
			else
				A1Q4SetCainArrived(ptGame, hUnit);
		}
		break;
	case 201:
		if (!A2Q4NotWaiting(ptGame))
		{
			AiStand(ptGame, hUnit, 40);
			return;
		}
		A2Q4MonsterThink(ptGame, hUnit, &dwIdle, &dwBusy);
		if (dwBusy)
			AiStand(ptGame, hUnit, 20);
		if (!dwIdle)
			return;
		if (A2Q4TakeArrived(ptGame, hUnit))
		{
			ptCommand = AiFindOrAddCommand(ptGame, hUnit, 10, FALSE);
			if (ptCommand)
			{
				ptCommand->dwArg1 = D2CommonUnitGetX(hUnit) + 9;
				ptCommand->dwArg2 = D2CommonUnitGetY(hUnit);
				AiStand(ptGame, hUnit, 50);
				return;
			}
		}
		break;
	case 254:
		if (A3Q4TestFlag(ptGame))
		{
			AiSetModeAtPoint(ptGame, hUnit, MONMODE_SKILL1, 0, 0);
			A3Q4ClearFlag(ptGame);
			return;
		}
		break;
	case 255:
		if (A3Q3GetSpot(ptGame, &tCoord))
		{
			if ((DWORD)AiThinkPointDistance(hUnit, (int)tCoord.dwX, (int)tCoord.dwY) > 3)
			{
				AiWalkToPoint(ptGame, hUnit, (int)tCoord.dwX, (int)tCoord.dwY);
				return;
			}
			AiSetModeAtPoint(ptGame, hUnit, MONMODE_SKILL1, 0, 0);
			A3Q3OpenGate(ptGame);
			return;
		}
		break;
	}
	if (!AiThinkNpcPlayer(ptGame, hUnit, ptParms) && !AiThinkNpcCommand(ptGame, hUnit, ptParms)
		&& !AiThinkNpcPath(ptGame, hUnit, ptParms))
		AiStand(ptGame, hUnit, 8);
}

BYTE __fastcall AiThinkNpcPlayer(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	BOOL bClose;
	UNIT *hPlayer;
	int nDist;
	THREATNODE **pptThreats;
	BOOL bThreat;
	BOOL bHasThreats;
	AIGENERAL *ptAi;
	AICOMMAND *ptCommand;
	AICOMMAND *ptWalk;

	bClose = FALSE;
	hPlayer = AiFindNearPlayer(ptGame, hUnit, &bClose);
	nDist = AiThinkDistance(hUnit, hPlayer);
	pptThreats = MONDATA(hUnit)->pptThreats;
	bThreat = MonsterAIIsThreat(pptThreats, hPlayer);
	if (hPlayer->eType == UNIT_PLAYER && PlayerIsBusy(hPlayer) == TRUE)
		bThreat = TRUE;
	bHasThreats = MonsterAIHasThreats(pptThreats);
	ptAi = ptParms->ptAi;
	if ((BOOL)(bHasThreats || bThreat || (int)ptAi->adwParam[0] > 0))
	{
		if ((DWORD)AiThinkPointDistance(hUnit, (int)ptAi->adwParam[1], (int)ptAi->adwParam[2]) > 2
			&& (int)ptAi->adwParam[0] > 36)
			AiWalkToPoint(ptGame, hUnit, (int)ptAi->adwParam[1], (int)ptAi->adwParam[2]);
		else if ((int)ptAi->adwParam[0] > 0)
		{
			if (hUnit->ptPath)
				D2CommonPathClearSteps(hUnit->ptPath);
			AiStand(ptGame, hUnit, 8);
		}
		if ((int)ptAi->adwParam[0] < 0)
			ptAi->adwParam[0] = 0;
		else
			ptAi->adwParam[0]--;
		return TRUE;
	}
	if (!hPlayer || hPlayer == hUnit)
		return FALSE;
	if (nDist > 2 && nDist < 24)
	{
		ptCommand = AiFindCommand(hUnit, 10, FALSE);
		if (ptCommand)
		{
			if ((DWORD)AiThinkPointDistance(hUnit, (int)ptCommand->dwArg1,
				(int)ptCommand->dwArg2) > 16)
			{
				ptWalk = AiFindOrAddCommand(ptGame, hUnit, 4, FALSE);
				if (ptWalk)
				{
					ptWalk->dwArg1 = ptCommand->dwArg1;
					ptWalk->dwArg2 = ptCommand->dwArg2;
					ptWalk->dwArg3 = 12;
					ptWalk->dwArg4 = 10;
					AiStand(ptGame, hUnit, 10);
					return TRUE;
				}
				AiStand(ptGame, hUnit, 10);
				return TRUE;
			}
		}
		else
		{
			ptCommand = AiFindOrAddCommand(ptGame, hUnit, 10, FALSE);
			if (ptCommand && !ptCommand->dwArg1 && !ptCommand->dwArg2)
			{
				ptCommand->dwArg1 = D2CommonUnitGetX(hUnit);
				ptCommand->dwArg2 = D2CommonUnitGetY(hUnit);
				AiStand(ptGame, hUnit, 20);
				return TRUE;
			}
		}
		AiSetVelocity(hUnit, 1, 0, 0);
		if (nDist >= 5)
			AiWalkToDistance(ptGame, hUnit, hPlayer, 3, 2);
		else
			AiWalkToDistance(ptGame, hUnit, hPlayer, nDist - 2, 2);
		return TRUE;
	}
	D2CommonPathClearSteps(hUnit->ptPath);
	if (!ptAi->adwParam[1])
	{
		ptAi->adwParam[1] = 60;
		if (hPlayer->eType == UNIT_PLAYER)
		{
			SUnitSetEvent(hUnit, 17, hPlayer);
			AiStand(ptGame, hUnit, 20);
			return TRUE;
		}
		AiStand(ptGame, hUnit, 20);
		return TRUE;
	}
	if ((int)ptAi->adwParam[1] < 0)
		ptAi->adwParam[1] = 0;
	if ((int)ptAi->adwParam[1] > 0)
		ptAi->adwParam[1]--;
	AiStand(ptGame, hUnit, 20);
	return TRUE;
}

BYTE __fastcall AiThinkNpcCommand(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	BYTE bDone;
	AICOMMAND *ptCommand;
	int nDist;

	bDone = FALSE;
	ptCommand = AiFindCommand(hUnit, 4, FALSE);
	if (ptCommand && (int)ptCommand->dwArg3 > 0)
	{
		if (ptCommand->dwArg1 && ptCommand->dwArg2
			&& (DWORD)AiThinkPointDistance(hUnit, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2) > 3)
		{
			if (gnAiThinkNpcWalks % 4 == 0)
			{
				AiSetVelocity(hUnit, 5, 0, 0);
				AiWalkToPointZeroStop(ptGame, hUnit, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2);
				bDone = TRUE;
				gnAiThinkNpcWalks++;
			}
			else
			{
				AiSetVelocity(hUnit, 1, 0, 0);
				AiWalkToPointZeroStop(ptGame, hUnit, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2);
				bDone = TRUE;
				gnAiThinkNpcWalks++;
			}
			ptCommand->dwArg3--;
		}
		else
		{
			AiStand(ptGame, hUnit, (int)ptCommand->dwArg4);
			ptCommand->dwArg3--;
			bDone = TRUE;
		}
	}
	else
	{
	ptCommand = AiFindCommand(hUnit, 5, FALSE);
	if (ptCommand)
	{
		if ((int)ptCommand->dwArg1 > 0)
		{
			if ((int)ptCommand->dwArg1 % 2)
			{
				AiStepAside(ptGame, hUnit, (BYTE)ptCommand->dwArg2);
				bDone = TRUE;
			}
			else if ((int)ptCommand->dwArg3 > 0)
			{
				AiStand(ptGame, hUnit, (int)ptCommand->dwArg4);
				ptCommand->dwArg3--;
				bDone = TRUE;
			}
			ptCommand->dwArg1--;
		}
		else if ((int)ptCommand->dwArg3 > 0)
		{
			AiStand(ptGame, hUnit, (int)ptCommand->dwArg4);
			bDone = TRUE;
			ptCommand->dwArg3--;
		}
	}
	if (!bDone)
	{
	ptCommand = AiFindCommand(hUnit, 7, FALSE);
	if (ptCommand && ptCommand->dwArg1)
	{
	if (ptCommand->dwArg1 != MONMODE_SKILL1 && ptCommand->dwArg1 != MONMODE_SKILL2 && ptCommand->dwArg1 != 10
		&& ptCommand->dwArg1 != 11)
	{
		ptCommand->dwArg1 = 0;
		AiStand(ptGame, hUnit, 50);
		bDone = TRUE;
	}
	nDist = AiThinkPointDistance(hUnit, (int)ptCommand->dwArg2, (int)ptCommand->dwArg3);
	if (!(bDone))
	{
		if (nDist > 0)
		{
			if ((int)ptCommand->dwArg4 > 0)
			{
				AiSetVelocity(hUnit, 7, 0, 0);
				AiWalkToPointZeroStop(ptGame, hUnit, (int)ptCommand->dwArg2, (int)ptCommand->dwArg3);
				bDone = TRUE;
				ptCommand->dwArg4--;
				goto lDone;
			}
			if (nDist > 1)
				ptCommand->dwArg1 = 0;
		}
		if (hUnit->dwClassId == 154)
			D2CommonPathSetFacingSmoothed(hUnit->ptPath, 56);
		if (hUnit->dwClassId == 155)
			D2CommonPathSetFacingSmoothed(hUnit->ptPath, 52);
		if (hUnit->dwClassId == 178)
			D2CommonPathSetFacingSmoothed(hUnit->ptPath, 4);
		if (hUnit->dwClassId == 405)
		{
			if (ptCommand->dwArg1 == MONMODE_SKILL1)
				D2CommonPathSetFacingSmoothed(hUnit->ptPath, 52);
			if (ptCommand->dwArg1 == MONMODE_SKILL2)
				D2CommonPathSetFacingSmoothed(hUnit->ptPath, 48);
		}
		if (ptCommand->dwArg1)
		{
			if (hUnit->dwMode == ptCommand->dwArg1)
				AiStand(ptGame, hUnit, 50);
			else
			{
				AiSetModeOnTarget(ptGame, hUnit, (int)ptCommand->dwArg1, hUnit);
				ptCommand->dwArg1 = 0;
			}
			bDone = TRUE; }
		if (hUnit->dwClassId == 178 && D2CommonSeedRand(&hUnit->tSeed, 100) < 66)
			ptCommand->dwArg1 = MONMODE_SKILL1;
	}
	}
	}
	}
lDone:
	return bDone;
}

static PFNAINPCNODE sgapfnAiThinkNpcNode[7] = {
	NULL, AiThinkNpcOnWalkPoint, AiThinkNpcOnHoldPoint, AiThinkNpcOnWalkPoint, AiThinkNpcOnFirstSkillPoint,
		AiThinkNpcOnSecondSkillPoint, NULL,
};
static int PLACEHOLDER_UnreferencedData_100b93b0 = sizeof(sgapfnAiThinkNpcNode) / sizeof(sgapfnAiThinkNpcNode[0]);

static __inline DWORD AiThinkRandom(D2SEED *ptSeed, DWORD dwMax)
{
	if (dwMax)
		return D2CommonSeedRand(ptSeed, dwMax);
	else
		return 0;
}

BYTE __fastcall AiThinkNpcPath(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	BYTE bDone;
	PRESETFILELIST *ptPresetPath = NULL;
	PRESETPATHNODE *ptNode = NULL;
	PFNAINPCNODE pfnNode = NULL;

	bDone = FALSE;
	ptPresetPath = (PRESETFILELIST *)ptParms->ptAi->pCell;
	if (ptPresetPath)
	{
		if (D2CommonSeedRand(&hUnit->tSeed, 100) < 66)
		{
#line 7738
			D2ASSERT(ptPresetPath->nNodes);
			ptNode = &ptPresetPath->pArray[AiThinkRandom(&hUnit->tSeed, ptPresetPath->nNodes)];
			pfnNode = sgapfnAiThinkNpcNode[ptNode->nAction];
			if (pfnNode)
			{
				bDone = pfnNode(ptGame, hUnit, ptParms, ptNode);
			}
		}
	}
	return bDone;
}

void __fastcall AiThinkTyrael(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	BOOL bClose;
	UNIT *hPlayer;
	int nDist;
	THREATNODE **pptThreats;
	AIGENERAL *ptAi;
	DWORD dwCount;

	bClose = FALSE;
	hPlayer = AiFindNearPlayer(ptGame, hUnit, &bClose);
	if (hPlayer && hPlayer != hUnit)
	{
		nDist = AiThinkDistance(hUnit, hPlayer);
		pptThreats = MONDATA(hUnit)->pptThreats;
		if (hPlayer->eType == UNIT_PLAYER && PlayerIsBusy(hPlayer) == TRUE)
		{
			AiStand(ptGame, hUnit, 10);
			return;
		}
		if (MonsterAIHasThreats(pptThreats))
		{
			AiStand(ptGame, hUnit, 10);
			return;
		}
		ptAi = ptParms->ptAi;
		dwCount = ptAi->adwParam[1];
		if (nDist < 24)
		{
			if (!dwCount)
			{
				ptAi->adwParam[1] = 60;
				if (hPlayer->eType == UNIT_PLAYER)
					SUnitSetEvent(hUnit, 17, hPlayer);
			}
			else
				ptAi->adwParam[1] = dwCount - 1;
		}
		else
		{
			if ((int)dwCount <= 0)
				ptAi->adwParam[1] = 60;
			ptAi->adwParam[1]--;
		}
	}
	else if (hUnit->dwClassId == 251)
	{
		if (A2Q6MonsterTest(ptGame, hUnit) && !MonsterAIHasThreats(MONDATA(hUnit)->pptThreats))
		{
			AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
			AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
			A2Q6EventAll(ptGame);
			return;
		}
	}
	else if (hUnit->dwClassId == 406)
	{
		if (A4Q1NoPlayerNear(ptGame, hUnit) && !MonsterAIHasThreats(MONDATA(hUnit)->pptThreats))
		{
			AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
			AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
			A4Q1UnitEvents(ptGame);
			return;
		}
	}
	AiStand(ptGame, hUnit, 20);
}

void __fastcall AiThinkNpcCommandOrPath(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AICOMMAND *ptCommand;

	ptCommand = AiFindOrAddCommand(ptGame, hUnit, 10, FALSE);
	if (ptCommand && !ptCommand->dwArg1 && !ptCommand->dwArg2)
	{
		ptCommand->dwArg1 = D2CommonUnitGetX(hUnit);
		ptCommand->dwArg2 = D2CommonUnitGetY(hUnit);
		AiStand(ptGame, hUnit, 20);
		return;
	}
	if (!AiThinkNpcCommand(ptGame, hUnit, ptParms) && !AiThinkNpcPath(ptGame, hUnit, ptParms))
		AiStand(ptGame, hUnit, 12);
}

void __fastcall AiThinkHellMeteor(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	MONSTATSTXT *ptMonStats;
	int nOffX;
	int nOffY;
	int nRange;
	int nX;
	int nY;

	ptMonStats = ptParms->ptMonStats;
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[0])
	{
		if (ptMonStats->AIParam[2] * 2 != 0)
			nOffX = (int)D2CommonSeedRand(&hUnit->tSeed, ptMonStats->AIParam[2] * 2);
		else
			nOffX = 0;
		nRange = ptMonStats->AIParam[2];
		nX = (int)D2CommonUnitGetX(hUnit) + nOffX - nRange;
		if (nRange * 2 != 0)
			nOffY = (int)D2CommonSeedRand(&hUnit->tSeed, nRange * 2);
		else
			nOffY = 0;
		nY = (int)D2CommonUnitGetY(hUnit) + nOffY - ptMonStats->AIParam[2];
		AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK1, SKILL_HELLMETEOR, NULL, nX, nY);
		return;
	}
	AiStand(ptGame, hUnit, ptMonStats->AIParam[1]);
}

void __fastcall AiThinkKaelan(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AICOMMAND *ptCommand;

	BOOL bOwnSpot;

	UNIT *hPlayer;
	int nDist;

	ptCommand = AiFindOrAddCommand(ptGame, hUnit, 10, FALSE);
	if (ptCommand && !ptCommand->dwArg1 && !ptCommand->dwArg2)
	{
		ptCommand->dwArg1 = D2CommonUnitGetX(hUnit);
		ptCommand->dwArg2 = D2CommonUnitGetY(hUnit);
		AiStand(ptGame, hUnit, 20);
		return;
	}
	ptCommand = AiFindCommand(hUnit, 10, FALSE);
	if (!A2Q4ObjectIdle(ptGame))
	{ GAMECOORD tCoord;
		tCoord.dwX = ptCommand->dwArg1;
		tCoord.dwY = ptCommand->dwArg2;
		bOwnSpot = TRUE;
		if (A2Q4GetSpot(ptGame, &tCoord))
			bOwnSpot = FALSE;
		if ((int)ptCommand->dwArg4 < ptGame->nFrame && ptGame->nFrame - (int)ptCommand->dwArg4 < 200)
		{
			AiStand(ptGame, hUnit, 20);
			return;
		}
		if ((DWORD)AiThinkPointDistance(hUnit, (int)tCoord.dwX, (int)tCoord.dwY) > 1)
		{
			AiWalkToPoint(ptGame, hUnit, (int)tCoord.dwX, (int)tCoord.dwY);
			ptCommand->dwArg4 = (DWORD)ptGame->nFrame;
			return;
		}
		if ((BOOL)(bOwnSpot && AiThinkNpcPlayer(ptGame, hUnit, ptParms)) == FALSE)
		{
			AiStand(ptGame, hUnit, 20);
			return;
		}
		ptCommand->dwArg4 = (DWORD)ptGame->nFrame;
		return;
	}
	if (ptCommand)
	{ BOOL bClose;
		hPlayer = AiFindNearPlayer(ptGame, hUnit, &bClose);
		nDist = AiThinkDistance(hUnit, hPlayer);
		if (A2Q4ObjectOpen(ptGame)
			&& (DWORD)AiThinkPointDistance(hUnit, (int)ptCommand->dwArg1,
				(int)ptCommand->dwArg2 - 3) > 2)
		{
			AiWalkToPoint(ptGame, hUnit, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2 - 3);
			return;
		}
		if ((DWORD)AiThinkPointDistance(hUnit, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2 - 3) > 7)
		{
			AiWalkToPoint(ptGame, hUnit, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2 - 3);
			return;
		}
		if (!AiThinkNpcPlayer(ptGame, hUnit, ptParms) && hPlayer && hPlayer != hUnit)
		{
			if (hPlayer->eType == UNIT_PLAYER)
			{
				if (nDist < 12)
				{
					if (D2CommonSeedRand(&hUnit->tSeed, 1000) < 100)
						AiWalkAround(ptGame, hUnit, hPlayer, 2);
					AiStand(ptGame, hUnit, 200);
					return;
				}
				AiStand(ptGame, hUnit, 20);
			}
			if (D2CommonSeedRand(&hUnit->tSeed, 1000) < 50)
				AiStepAside(ptGame, hUnit, 3);
		}
	}
	AiStand(ptGame, hUnit, 120);
}

void __fastcall AiThinkDeckardCain(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AICOMMAND *ptCommand;
	GAMECOORD tCoord;
	int nX;
	int nY;

	if (AiThinkWalkToQuestSpotStart(ptGame, hUnit, ptParms))
		return;
	if (MonsterAIHasThreats(MONDATA(hUnit)->pptThreats))
		goto lStand;
	ptCommand = AiFindCommand(hUnit, 3, FALSE);
	if (!hUnit->dwMode || hUnit->dwMode == 12)
		return;
	if (ptCommand)
	{
	if ((int)ptCommand->dwArg3 >= 2)
	{
		ptCommand->dwArg3++;
		if (A1Q4GetObjectSpot(ptGame, hUnit, &tCoord))
		{
			if ((int)ptCommand->dwArg3 < 8
				&& (DWORD)AiThinkPointDistance(hUnit, (int)tCoord.dwX, (int)tCoord.dwY) > 0)
			{
				AiWalkToPoint(ptGame, hUnit, (int)tCoord.dwX, (int)tCoord.dwY);
				return;
			}
			A1Q4UnitArrived(ptGame, hUnit);
			D2CommonStatsUnitSetValue(hUnit, 6, 0);
			AiSetModeAtPoint(ptGame, hUnit, 0, (int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit));
			return;
		}
	}
	else
	{
		if (AiThinkPointDistance(hUnit, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2) > 1
			&& (int)ptCommand->dwArg4 <= 5)
		{
			ptCommand->dwArg4++;
			AiWalkToPoint(ptGame, hUnit, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2);
			return;
		}
		if (ptCommand->dwArg3 == 1)
		{
			A1Q4CreateObject(ptGame, hUnit);
			ptCommand->dwArg3 = 2;
		}
	}
	AiStand(ptGame, hUnit, 20);
	}
	else
	{
		AiThinkWalkToQuestSpotStart(ptGame, hUnit, ptParms);
lStand:
		AiStand(ptGame, hUnit, 40);
	}
}

BOOL __fastcall AiThinkWalkToQuestSpotStart(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AICOMMAND *ptCommand;
	BOOL bStarted;

	bStarted = FALSE;
	ptCommand = AiFindOrAddCommand(ptGame, hUnit, 3, FALSE);
	if (ptCommand && !ptCommand->dwArg1 && !ptCommand->dwArg2)
	{
		ptCommand->dwArg1 = D2CommonUnitGetX(hUnit) + 3;
		ptCommand->dwArg2 = D2CommonUnitGetY(hUnit) + 3;
		if (!A1Q4SetSpot(ptGame, hUnit))
		{
			A1Q4UnitArrived(ptGame, hUnit);
			D2CommonStatsUnitSetValue(hUnit, 6, 0);
			AiSetModeAtPoint(ptGame, hUnit, 0, D2CommonUnitGetX(hUnit), D2CommonUnitGetY(hUnit));
		}
		ptCommand->dwArg3 = 1;
		ptCommand->dwArg4 = 0;
		AiStand(ptGame, hUnit, 80);
		bStarted = TRUE;
	}
	return bStarted;
}

void __fastcall AiThinkResetThinkOut(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;

	AIPARMS *ptAiParms = ptParms;
	ptAi = ptParms->ptAi;
	if (D2CommonStatsGetState(hUnit, 12))
		D2CommonStatsUnitSetActiveStateBit(hUnit, 12, 0);
	AiThinkSet(ptGame, hUnit, ptAiParms->ptAi, ptAi->nThinkKind);
	AiStand(ptGame, hUnit, 1);
}
#line 8091

static __inline void AiThinkHirelingFallback(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms, int nClass)
{
	if (nClass == 271)
		AiThinkSet(ptGame, hUnit, ptParms->ptAi, 5);
	else
		AiThinkSet(ptGame, hUnit, ptParms->ptAi, 6);
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkHireling(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	int nClass;
	AIGENERAL *ptAi;
	UNIT *hOwner;
	int eMode;
	int nParam;
	DWORD dwFar;
	DWORD dwNear;
	DWORD dwDist;
	int eOwnerMode;
	void *hRoom;
	void *hNewRoom;
	DRLGLOGICNODE *ptArea;
	int nX;
	int nY;
	int nAside;
	D2SEED *ptSeed;
	UNIT *hTarget;
	DWORD dwSize;

	nClass = (int)hUnit->dwClassId;
	ptSeed = &hUnit->tSeed;
	hOwner = AiGetOwner(hUnit);
	if (!hOwner)
	{
		AiThinkHirelingFallback(ptGame, hUnit, ptParms, nClass);
		return;
	}
	if (hOwner->eType != UNIT_PLAYER)
	{
		AiThinkHirelingFallback(ptGame, hUnit, ptParms, nClass);
		return;
	}
	if (D2CommonStatsGetState(hUnit, 12))
		D2CommonStatsUnitSetActiveStateBit(hUnit, 12, 0);
	eMode = (int)hUnit->dwMode;
	if (eMode == 2 || eMode == 15)
		return;
	ptAi = ptParms->ptAi;
	AiGetCommand(hUnit);
	nParam = (int)ptAi->adwParam[0];
	if (nParam > 10 && nParam < 20)
	{
		dwNear = (DWORD)nParam;
		dwFar = (DWORD)(nParam * 2);
		nParam >>= 1;
	}
	else
	{
		dwFar = 20;
		dwNear = 10;
		nParam = 5;
	}
	dwDist = (DWORD)AiThinkDistance(hUnit, hOwner);
	eOwnerMode = (int)hOwner->dwMode;
	if (dwDist > 100)
	{
		hRoom = (void *)D2CommonUnitGetPathRoom(hOwner);
		ptArea = (DRLGLOGICNODE *)D2CommonDungeonRoomGetLogicNode(hRoom, D2CommonUnitGetX(hOwner),
			D2CommonUnitGetY(hOwner));
		if (MonsterFindSpawnPoint(ptGame, (GAMEROOM *)hRoom, ptArea, 363, &nX, &nY, FALSE))
		{
			hNewRoom = D2CommonDungeonFindRoomGame(hRoom, nX, nY);
			if (hNewRoom && SUnitMoveToRoom(ptGame, hUnit, (GAMEROOM *)hNewRoom, nX, nY, FALSE, FALSE))
			{
				D2CommonUnitRoomAddToChangedList(hUnit);
				hUnit->dwFlags |= 65536;
				AiStand(ptGame, hUnit, 5);
				return;
			}
		}
	}
	if (dwDist > dwFar)
	{
		AiSetVelocity(hUnit, 0, 60, 100);
		if (AiRunToTargetStep(ptGame, hUnit, hOwner, (BYTE)nParam))
			return;
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
		AiSetVelocity(hUnit, 15, 60, 100);
		AiRunToTargetStep(ptGame, hUnit, hOwner, (BYTE)nParam);
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
		nAside = (int)((DWORD)AiThinkDistance(hUnit, hOwner) >> 2);
		if (nAside < 4)
			nAside = 4;
		AiStepAside(ptGame, hUnit, (BYTE)nAside);
		return;
	}
	if (dwDist > dwNear)
	{
		if (eOwnerMode == PLRMODE_WALK || eOwnerMode == PLRMODE_TOWN_WALK)
		{
			AiSetVelocity(hUnit, 0, 0, 100);
			if (AiWalkToTargetStep(ptGame, hUnit, hOwner, (BYTE)nParam))
				return;
			EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
			AiSetVelocity(hUnit, 15, 0, 100);
			if (AiWalkToTargetStep(ptGame, hUnit, hOwner, (BYTE)nParam))
				return;
			EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
			nAside = (int)((DWORD)AiThinkDistance(hUnit, hOwner) >> 2);
			if (nAside < 4)
				nAside = 4;
			AiStepAside(ptGame, hUnit, (BYTE)nAside);
			return;
		}
		if (eOwnerMode == PLRMODE_RUN)
		{
			AiSetVelocity(hUnit, 0, 60, 100);
			if (AiRunToTargetStep(ptGame, hUnit, hOwner, (BYTE)nParam))
				return;
			EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
			AiSetVelocity(hUnit, 15, 60, 100);
			AiRunToTargetStep(ptGame, hUnit, hOwner, (BYTE)nParam);
			EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
			nAside = (int)((DWORD)AiThinkDistance(hUnit, hOwner) >> 2);
			if (nAside < 4)
				nAside = 4;
			AiStepAside(ptGame, hUnit, (BYTE)nAside);
			return;
		}
	}
	if (eMode != 1)
	{
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
		AiStand(ptGame, hUnit, 5);
		return;
	}
	dwSize = D2CommonPathGetCollisionPatternByUnit(hUnit);
	if (D2CommonCollisnTestPatternAnyRaw((DWORD)D2CommonUnitGetPathRoom(hUnit), D2CommonUnitGetX(hUnit),
			D2CommonUnitGetY(hUnit), dwSize, 64))
	{
		AiStepAside(ptGame, hUnit, 5);
		return;
	}
	if (!D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hUnit)))
	{ DWORD dwDir;
		hTarget = AiFindTargetInSight(ptGame, hUnit, (int *)&dwDist, &dwDir);
		if (hTarget && dwDist < 25)
		{
			AiThinkHirelingFight(ptGame, hUnit, nClass, hOwner, hTarget, ptSeed, ptParms);
			return;
		}
	}
	dwDist = (DWORD)AiThinkDistance(hUnit, hOwner);
	if (dwDist <= 1)
	{
		if (AiWalkAway(ptGame, hUnit, hOwner, (BYTE)(nParam - 1), TRUE))
			return;
		AiWalkAround(ptGame, hUnit, hOwner, (BYTE)(nParam - 1));
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < 5)
	{
		AiWalkAround(ptGame, hUnit, hOwner, (BYTE)dwNear);
		return;
	}
	AiStand(ptGame, hUnit, 5);
}

void __fastcall AiThinkHirelingFight(GAME *ptGame, UNIT *hUnit, int nClass, UNIT *hOwner,
	UNIT *hTarget, D2SEED *ptSeed, AIPARMS *ptParms)
{
	MONSTATSTXT *ptMonStats;
	AIGENERAL *ptAi;
	DWORD dwChance;
	PETDATA *ptPet;
	int nDist;
	BOOL bAct;

	ptMonStats = ptParms->ptMonStats;
	ptAi = ptParms->ptAi;
	if (nClass == 338)
	{
		dwChance = 98;
		if (ptAi->adwParam[1])
			dwChance = (DWORD)(2 / (int)ptAi->adwParam[1] + 98); nDist = AiThinkDistance(hUnit, hTarget);
	}
	else
	{
		dwChance = 80;
		if (ptAi->adwParam[1])
			dwChance = (DWORD)(20 / (int)ptAi->adwParam[1] + 80); nDist = AiThinkDistance(hUnit, hTarget);
	}

	ptPet = PlayerPetsGetData(hOwner, hUnit->dwId);
	if (!ptPet)
	{
		AiStand(ptGame, hUnit, 10);
		return;
	}
	bAct = FALSE;
	if (D2CommonSeedRand(ptSeed, 100) < dwChance)
		bAct = TRUE;
	if (!ptMonStats->AIParam[0])
	{
		if (nDist < 4 && D2CommonSeedRand(ptSeed, 100) < 50)
		{
			if (AiWalkAround(ptGame, hUnit, hOwner, 4))
				return;
			EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
			if (AiWalkAway(ptGame, hUnit, hTarget, 4, TRUE))
				return;
			goto skill;
		}
		else if (bAct)
		{
skill:
			AiThinkHirelingSkill(ptGame, hUnit, nClass, ptPet->dwSeed, hTarget, ptSeed);
			return;
		}
		else
		{
			AiStand(ptGame, hUnit, 10);
			return;
		}
	}
	if (nDist < 2 && D2CommonUnitTestPositionClear(hUnit, hTarget, 0))
	{
		if (bAct)
		{
			AiThinkHirelingSkill(ptGame, hUnit, nClass, ptPet->dwSeed, hTarget, ptSeed);
			return;
		}
		AiStand(ptGame, hUnit, 10);
		return;
	}
	D2CommonPathSetStopDistance(hUnit->ptPath, ptMonStats->bMeleeRng);
	AiWalkToTarget(ptGame, hUnit, hTarget, 1);
}

void __fastcall AiThinkHirelingSkill(GAME *ptGame, UNIT *hUnit, int nClass,
	DWORD dwPetSeed, UNIT *hTarget, D2SEED *ptSeed)
{
	MONKILLSRECORD tRecord;
	int nParam;
	int nDist;
	int nRange;

	tRecord.nKind = D2CommonMonsterKillsLogClassify(nClass, 0);
	D2CommonMonsterKillsLogRollRecord(dwPetSeed, &tRecord, ptGame->bDifficultyLevel);
	if (nClass == 271)
	{
		nParam = 120;
		if (tRecord.nHireSpecial == 1 && D2CommonSeedRand(ptSeed, 100) < 20)
			nParam = 121;
		if (tRecord.nHireSpecial == 2 && D2CommonSeedRand(ptSeed, 100) < 20)
			nParam = 122;
		SkillsSetLevel(hUnit, SKILL_HIREABLEMISSILE, (DWORD)nParam, 0);
		AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK1, SKILL_HIREABLEMISSILE, hTarget, 0, 0);
		return;
	}
	if (nClass == 338)
	{
		if (tRecord.nHireSpecial == 4 && D2CommonSeedRand(ptSeed, 100) < 30)
		{
			AiUseSkill(ptGame, hUnit, SKILL_JAB, hTarget, 0, 0);
			return;
		}
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	if (nClass == 359)
	{
		if (D2CommonUnitTestPositionClear(hUnit, hTarget, 0)
			&& D2CommonSeedRand(ptSeed, 100) < 50)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		if (tRecord.nHireSpecial == 6 || tRecord.nHireSpecial == 9)
		{
			if (D2CommonSeedRand(ptSeed, 100) < 67)
				AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_PRIMEBOLT, hTarget, 0, 0);
			else
				AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_PRIMELIGHTNING, hTarget, 0, 0);
			return;
		}
		if (tRecord.nHireSpecial == 7 || tRecord.nHireSpecial == 10)
		{
			if (D2CommonSeedRand(ptSeed, 100) >= 75 && !D2CommonStatsFindListByState(hUnit, 10))
				AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_MONFROZENARMOR, hTarget, 0, 0);
			else
				AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_PRIMESPIKE, hTarget, 0, 0);
			return;
		}
		if (tRecord.nHireSpecial == 8 || tRecord.nHireSpecial == 11)
		{
			nDist = AiThinkDistance(hUnit, hTarget);
			nRange = (int)D2CommonSkillsCalcNodeLevel(hUnit, D2CommonSkillsFindByKey(hUnit, 178), 1) + 3;
			if (nRange <= 0)
				nRange = 1;
			if (D2CommonSeedRand(ptSeed, 100) >= 50 && nDist < nRange)
				AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_FETISHINFERNO, hTarget, 0, 0);
			else
				AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_VAMPIREFIREBALL, hTarget, 0, 0);
			return;
		}
	}
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkRogueScout(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	int nDist;
	DWORD dwDir;
	UNIT *hTarget;
	D2SEED *ptSeed;

	if (hUnit->dwMode != 1)
	{
		AiStand(ptGame, hUnit, 5);
		return;
	}
	ptSeed = &hUnit->tSeed;
	if (!D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hUnit)))
	{
		hTarget = AiFindTargetInSight(ptGame, hUnit, &nDist, &dwDir);
		if (hTarget && nDist < 20)
		{
			if (D2CommonSeedRand(ptSeed, 100) < 30)
			{
				if (hUnit->dwClassId == 271)
				{
					SkillsSetLevel(hUnit, SKILL_HIREABLEMISSILE, 120, 0);
					AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK1, SKILL_HIREABLEMISSILE, hTarget, 0, 0);
					return;
				}
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
				return;
			}
			if (D2CommonSeedRand(ptSeed, 100) < 30)
			{
				AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, 0);
				return;
			}
			AiStand(ptGame, hUnit, 10);
			return;
		}
	}
	if (D2CommonSeedRand(ptSeed, 100) < 20)
	{
		AiStepAside(ptGame, hUnit, 5);
		return;
	}
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkFightOutOfTown(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{

	UNIT *hTarget;
	D2SEED *ptSeed;

	if (hUnit->dwMode != 1)
	{
		AiStand(ptGame, hUnit, 5);
		return;
	}
	ptSeed = &hUnit->tSeed;
	if (!D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hUnit)))
	{ int nDist; DWORD dwReach;
		hTarget = AiFindTarget(ptGame, hUnit, ptParms->ptAi, &nDist, &dwReach);
		if (hTarget)
		{
			if (dwReach)
			{
				if (D2CommonSeedRand(ptSeed, 100) < 80)
				{
					AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
					return;
				}
			}
			else if (D2CommonSeedRand(ptSeed, 100) < 30)
			{
				AiWalkToTarget(ptGame, hUnit, hTarget, 7);
				return;
			}
			AiStand(ptGame, hUnit, 10);
			return;
		}
	}
	if (D2CommonSeedRand(ptSeed, 100) < 20)
	{
		AiStepAside(ptGame, hUnit, 5);
		return;
	}
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkAttackInSight(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	int nDist;
	DWORD dwDir;
	UNIT *hTarget;

	nDist = ptParms->nDist;
	dwDir = (DWORD)ptParms->fTargetInReach;
	hTarget = AiFindTargetInSight(ptGame, hUnit, &nDist, &dwDir);
	if (hTarget && nDist < 25)
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	AiStand(ptGame, hUnit, 50);
}

void __fastcall AiThinkNavi(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	BOOL bClose;
	UNIT *hPlayer;
	AIGENERAL *ptAi;
	int nDist;
	DWORD dwDir;
	UNIT *hTarget;

	if (MonsterAIHasThreats(MONDATA(hUnit)->pptThreats))
	{
		AiStand(ptGame, hUnit, 10);
		return;
	}
	hPlayer = AiFindNearPlayer(ptGame, hUnit, &bClose);
	if (hPlayer && hPlayer->eType == UNIT_PLAYER && !PlayerIsBusy(hPlayer)
		&& D2CommonSeedRand(&hUnit->tSeed, 3) && bClose)
	{
		ptAi = ptParms->ptAi;
		if (!ptAi->adwParam[1])
		{
			ptAi->adwParam[1] = 60;
			if (hPlayer->eType == UNIT_PLAYER)
			{
				SUnitSetEvent(hUnit, 17, hPlayer);
				AiStand(ptGame, hUnit, 20);
				return;
			}
		}
		else
		{
			if ((int)ptAi->adwParam[1] < 0)
				ptAi->adwParam[1] = 0;
			if ((int)ptAi->adwParam[1] > 0)
				ptAi->adwParam[1]--;
		}
		AiStand(ptGame, hUnit, 20);
		return;
	}
	hTarget = AiFindTargetInSight(ptGame, hUnit, &nDist, &dwDir);
	if (hTarget && nDist < 25)
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	AiStand(ptGame, hUnit, 50);
}

void __fastcall AiThinkWait(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	if (D2CommonUnitGetPathRoom(hUnit))
		AiStand(ptGame, hUnit, 200);
}

void __fastcall AiThinkGroupMember(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	int nDist;
	AICOMMAND *ptCommand;
	BOOL bBroken;
	AICOMMAND tCommand;

	nDist = ptParms->nDist;
	ptCommand = AiGetCommand(hUnit);
	if (ptCommand && ptCommand->nCommand == 8)
	{
		AiRemoveFromOwner(hUnit);
		AiThinkSet(ptGame, hUnit, ptParms->ptAi, 0);
		AiStand(ptGame, hUnit, 10);
		return;
	}
	{
		bBroken = FALSE;
		AiForEachMinion(hUnit, (DWORD)&bBroken, 0, (PFNAIMINIONFN)AiThinkPatrolOnMinion);
		if (!MonsterTestPrevModeGetHit(hUnit) && !bBroken && nDist >= 20)
		{
			AiStand(ptGame, hUnit, 10);
			return;
		}
		tCommand.nCommand = 8;
		AiCommandMinions(ptGame, hUnit, &tCommand);
	}
	AiRemoveFromOwner(hUnit);
	AiThinkSet(ptGame, hUnit, ptParms->ptAi, 0);
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkAttackOrStepAside(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	D2SEED *ptSeed;
	ptSeed = &hUnit->tSeed;
	if (ptParms->fTargetInReach && !ptParms->ptMonStats->bInteract && ptParms->ptMonStats->bModeA1)
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, ptParms->hTarget);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < 20)
	{
		AiStepAside(ptGame, hUnit, 3);
		return;
	}
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkFleeInit(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	int nCmd;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return;
	switch (hTarget->eType)
	{
	case UNIT_PLAYER:
		nCmd = 1;
		break;
	case UNIT_MONSTER:
		nCmd = 2;
		break;
	case 3:
		nCmd = 4;
		break;
	default:
		return;
	}
	MonsterAISetTargetCmd(hUnit, nCmd, hTarget->dwId);
}

void __fastcall AiThinkFlee(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	AIGENERAL *ptAi;
	int nDist;
	BOOL bReach;
	int nLimit;
	MONSTATSTXT *ptMonStats;
	int nVel;

	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	bReach = ptParms->fTargetInReach;
	ptAi = ptParms->ptAi;
	if (!D2CommonStatsGetState(hUnit, 56))
	{
		AiThinkSet(ptGame, hUnit, ptParms->ptAi, 0);
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
		EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + 1);
		return;
	}
	nLimit = (int)ptAi->adwParam[0];
	if (!nLimit)
		nLimit = 30;
	if (nDist > nLimit)
	{
		AiStand(ptGame, hUnit, 10);
		return;
	}
	ptMonStats = ptParms->ptMonStats;
	if ((DWORD)ptMonStats->nVelocity > 0)
		nVel = (int)(ptMonStats->Run * 100 / (DWORD)ptMonStats->nVelocity);
	else
		nVel = 0;
	if (nVel < 100)
		nVel = 0;
	else
	{
		nVel -= 100;
		if (nVel > 120)
			nVel = 120;
	}

	if (!ptAi->adwParam[2])
	{
		ptAi->adwParam[2] = 1;
		MonsterAIClearTargetCmd(hUnit);
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
		AiSetVelocity(hUnit, 2, nVel, 0);
		if (ptMonStats->bModeRN)
			AiRunAway(ptGame, hUnit, hTarget, 30, FALSE);
		else
			AiWalkAway(ptGame, hUnit, hTarget, 30, FALSE);
		return;
	}
	if (bReach && !ptMonStats->bInteract && ptMonStats->bModeA1)
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	AiSetVelocity(hUnit, 2, nVel, 0);
	if (ptMonStats->bModeRN)
	{
		if (AiRunAway(ptGame, hUnit, hTarget, 30, TRUE))
			return;
	}
	else if (AiWalkAway(ptGame, hUnit, hTarget, 30, TRUE))
		return;
	AiStepAside(ptGame, hUnit, 6);
}

void __fastcall AiThinkChargeUnit(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	AIGENERAL *ptAi;
	DWORD dwReach;
	int nParam;
	UNIT *hOther;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget || D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hTarget)))
	{
		AiThinkSet(ptGame, hUnit, MONDATA(hUnit)->ptAi, 0);
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
		EventAdd(ptGame, hUnit, EVENT_RUN_AI, ptGame->nFrame + 1);
		return;
	}
	ptAi = ptParms->ptAi;
	if (!ptAi->adwParam[0])
	{
		ptAi->adwParam[0] = 1;
		AiChargeTarget(ptGame, hUnit, hTarget, 7);
		return;
	}
	dwReach = D2CommonUnitTestPositionClear(hUnit, hTarget, 0);
	if (dwReach)
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	hOther = AiIdleByParam(ptGame, hUnit, ptParms->ptAi, &nParam, &dwReach);
	if (hOther && dwReach)
	{
		ptAi->adwParam[0] = 0;
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hOther);
		return;
	}
	ptAi->adwParam[0] = 1;
	D2CommonUnitSetTarget(hUnit, hTarget);
	AiChargeTarget(ptGame, hUnit, hTarget, 7);
}

void __fastcall AiThinkSpawnBatchInit(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi = ptParms->ptAi;

	ptAi->adwParam[0] = (DWORD)ptGame->nFrame;
	ptAi->adwParam[1] = 10;
}

void __fastcall AiThinkSpawnBatch(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	int eMode;
	AIGENERAL *ptAi;
	MONSTATSTXT *ptMonStats;
	int nFrame;
	int nClass;
	int nX;
	int nY;
	int nMode;
	GAMEROOM *hRoom;
	UNIT *hSpawn;

	eMode = (int)hUnit->dwMode;
	if (ptParms->nDist > 10)
	{
		if (eMode == 1)
		{
			AiStand(ptGame, hUnit, 5);
			return;
		}
	}
	else if (eMode == 1)
	{
		AiSetModeAtPoint(ptGame, hUnit, 10, 0, 0);
		AiThinkIn(ptGame, hUnit, 20);
		return;
	}
	if (eMode == 10)
	{
		AiSetModeAtPoint(ptGame, hUnit, 11, 0, 0);
		AiThinkIn(ptGame, hUnit, 20);
		return;
	}
	if (eMode != 11)
	{
		AiThinkIn(ptGame, hUnit, 1);
		return;
	}
	ptMonStats = ptParms->ptMonStats;
	ptAi = ptParms->ptAi;
	if (!ptAi->adwParam[1])
	{
		AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
		return;
	}
	nFrame = ptGame->nFrame;
	if (ABS(nFrame - (int)ptAi->adwParam[0]) >= ptMonStats->AIParam[1])
	{
		ptAi->adwParam[0] = (DWORD)nFrame;
		D2CommonMonsterGetSpecialPosition(hUnit, &nClass, &nX, &nY, &nMode, 0);
		hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hUnit), nX, nY);
		if (hRoom)
		{
			hSpawn = MonsterSpawn(ptGame, hRoom, nX, nY, nClass, (DWORD)nMode, 2, 66);
			if (hSpawn)
			{
				ptAi->adwParam[1]--;
				hSpawn->dwFlags0 |= 67239936;
			}
		}
	}
	AiThinkIn(ptGame, hUnit, ptMonStats->AIParam[1]);
}

void __fastcall AiThinkGroper(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	MONSTATSTXT *ptMonStats;
	int nDist;
	BOOL bReach;
	AIGENERAL *ptAi;
	int nLife;
	BOOL bKind3;
	int nVel;

	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	bReach = ptParms->fTargetInReach;
	ptAi = ptParms->ptAi;
	ptMonStats = ptParms->ptMonStats;
	nLife = D2CommonUnitCalcLifePercent(hUnit);
	bKind3 = MonsterTestPrevModeGetHit(hUnit);
	if ((DWORD)ptMonStats->nVelocity > 0)
		nVel = (int)(ptMonStats->Run * 100 / (DWORD)ptMonStats->nVelocity);
	else
		nVel = 0;
	if (nVel < 100)
		nVel = 0;
	else
	{
		nVel -= 100;
		if (nVel > 120)
			nVel = 120;
	}

	if (bKind3)
	{
		ptAi->adwParam[0] = 0;
		if (bReach)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		if (nDist < 6)
		{
			AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_FINGERMAGESPIDER, hTarget, 0, 0);
			return;
		}
	}
	if (ptAi->adwParam[0] == 1)
	{
		if (nLife <= ptMonStats->AIParam[2] && D2CommonSeedRand(&hUnit->tSeed, 100) >= 25
			&& (int)ptAi->adwParam[1] <= 4)
		{
			if (nDist < 14)
			{
				AiSetVelocity(hUnit, 0, nVel, 0);
				AiWalkAway(ptGame, hUnit, hTarget, 14, FALSE);
				ptAi->adwParam[1]++;
				return;
			}
		}
		else
			ptAi->adwParam[0] = 0;
		AiStand(ptGame, hUnit, 15);
		return;
	}
	if (!AiGetOwner(hUnit) && nLife < ptMonStats->AIParam[3])
	{
		ptAi->adwParam[0] = 1;
		AiWalkAway(ptGame, hUnit, hTarget, 9, FALSE);
		return;
	}
	if (bReach)
	{
		if (D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[0])
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		else
			AiWalkToTargetFast(ptGame, hUnit, hTarget, 5, 0);
		return;
	}
	if (nDist < 6)
	{
		if (D2CommonSeedRand(&hUnit->tSeed, 100) < ptMonStats->AIParam[1])
		{
			AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_FINGERMAGESPIDER, hTarget, 0, 0);
			return;
		}
		if (D2CommonStatsGetState(hTarget, 84))
		{
			AiWalkToTarget(ptGame, hUnit, hTarget, 0);
			return;
		}
		AiStand(ptGame, hUnit, 12);
		return;
	}
	AiWalkToTarget(ptGame, hUnit, hTarget, 7);
}

void __fastcall AiThinkCorpulent(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	MONSTATSTXT *ptMonStats;
	BOOL bReach;
	UNIT *hTarget;
	DWORD hRoom;
	int nX;
	int nY;
	UNITFINDDATA tFind;
	UNITFINDARGS tArgs;
	int nPick;
	UNIT *hFound;
	int i;
	D2SEED *ptSeed = &hUnit->tSeed;
	ptAi = ptParms->ptAi;
	hTarget = ptParms->hTarget;
	bReach = ptParms->fTargetInReach;
	ptMonStats = ptParms->ptMonStats;
	D2CommonUnitCalcLifePercent(hUnit);
	MonsterTestPrevModeGetHit(hUnit);
	hRoom = D2CommonUnitGetPathRoom(hUnit);
	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	if (!ptAi->adwParam[0])
	{
		for (i = 0; i < (int)(sizeof(UNITFINDARGS) / sizeof(DWORD)); i++)
			((DWORD *)&tArgs)[i] = 0;
		tArgs.nY = nY;
		tArgs.nX = nX;
		tArgs.dwFlags = 1411;
		tArgs.nDistance = 2;
		D2CommonUnitFindsInit(ptGame->pMemPool, &tFind, hRoom, nX, nY, 2,
			AiThinkTestSoft, &tArgs);
		D2CommonUnitFindsFindUnits(&tFind);
		if (tFind.nNumUnits)
			nPick = (int)D2CommonSeedRand(ptSeed, tFind.nNumUnits);
		else
			nPick = 0;
		hFound = tFind.phUnitsArray[nPick];
		if (hFound && D2CommonSeedRand(ptSeed, 100) < 20)
		{
			AiWalkToTargetStep(ptGame, hUnit, hFound, 1);
			ptAi->adwParam[0] = 2;
			ptAi->adwParam[1] = hFound->dwId;
			return;
		}
	}
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[4]
		&& (!ptAi->adwParam[0] || ptAi->adwParam[0] == 1))
	{
		ptAi->adwParam[0] = 1;
		for (i = 0; i < (int)(sizeof(UNITFINDARGS) / sizeof(DWORD)); i++)
			((DWORD *)&tArgs)[i] = 0;
		tArgs.nX = nX;
		tArgs.nY = nY;
		tArgs.dwFlags = 1411;
		tArgs.nDistance = 24;
		D2CommonUnitFindsInit(ptGame->pMemPool, &tFind, hRoom, nX, nY, 24,
			AiThinkTestSoft, &tArgs);
		D2CommonUnitFindsFindUnits(&tFind);
		if (tFind.nNumUnits)
			nPick = (int)D2CommonSeedRand(ptSeed, tFind.nNumUnits);
		else
			nPick = 0;
		hFound = tFind.phUnitsArray[nPick];
		if (hFound)
		{
			AiWalkToTargetStep(ptGame, hUnit, hFound, 1);
			ptAi->adwParam[0] = 2;
			ptAi->adwParam[1] = hFound->dwId;
		}
		else
		{
			ptAi->adwParam[0] = 0;
			AiStepAside(ptGame, hUnit, 8);
		}
		D2CommonUnitFindsFreeArray(&tFind);
		return;
	}
	if (ptAi->adwParam[0] == 2)
	{
		hFound = SUnitFind(ptGame, UNIT_MONSTER, ptAi->adwParam[1]);
		if (hFound)
		{
			if ((DWORD)AiThinkDistance(hUnit, hFound) > 2)
			{
				if ((int)ptAi->adwParam[1] < 6)
				{
					AiWalkToTargetStep(ptGame, hUnit, hFound, 1);
					ptAi->adwParam[1]++;
					return;
				}
				ptAi->adwParam[1] = 0;
				ptAi->adwParam[0] = 0;
				AiStepAside(ptGame, hUnit, 8);
				return;
			}
			ptAi->adwParam[0] = 3;
			AiStand(ptGame, hUnit, 8);
			return;
		}
		ptAi->adwParam[0] = 0;
		AiStepAside(ptGame, hUnit, 8);
		return;
	}
	if (ptAi->adwParam[0] == 3)
	{
		hFound = SUnitFind(ptGame, UNIT_MONSTER, ptAi->adwParam[1]);
		if (hFound && hFound->dwMode == MONMODE_DEAD)
		{
			AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_REGURGITATOREAT, hFound, 0, 0);
			ptAi->adwParam[0] = 4;
			return;
		}
		ptAi->adwParam[0] = 0;
		AiStepAside(ptGame, hUnit, 8);
		return;
	}
	if (!ptAi->adwParam[0] && bReach
		&& D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		return;
	}
	if (ptAi->adwParam[0] == 4)
	{
		if (!bReach)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
			ptAi->adwParam[0] = 5;
			return;
		}
		AiWalkAway(ptGame, hUnit, hTarget, 8, TRUE);
		return;
	}
	if (ptAi->adwParam[0] == 5)
	{
		AiWalkAway(ptGame, hUnit, hTarget, 16, TRUE);
		ptAi->adwParam[0] = 0;
		return;
	}
	if (!bReach && D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2])
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, 0);
		return;
	}
	ptAi->adwParam[0] = 0;
	AiStand(ptGame, hUnit, 8);
}

BOOL __fastcall AiThinkTestSoft(UNIT *hUnit, DWORD dwArgs)
{
	if (hUnit->eType == UNIT_MONSTER && D2CommonMonsterUnitTestFieldC(hUnit)
		&& ((MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId))->bSoft)
		return TRUE;
	return FALSE;
}

void __fastcall AiThinkMephisto(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	int nDist;
	BOOL bReach;
	UNIT *hTarget;
	int nLife;
	int nHurt;
	AIGENERAL *ptAi;
	DWORD eTurn;
	int nDraw;
	BOOL bFlee;
	D2SEED *ptSeed;

	ptSeed = &hUnit->tSeed;
	nDist = ptParms->nDist;
	bReach = ptParms->fTargetInReach;
	hTarget = ptParms->hTarget;
	nLife = D2CommonUnitCalcLifePercent(hUnit);
	nHurt = (100 - nLife) / 5;
	if (nHurt < 0)
		nHurt = 0;
	ptAi = ptParms->ptAi;
	eTurn = ptAi->adwParam[2];
	if (bReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) > 10)
			eTurn = 3;
		else
		{
			eTurn = 2;
			ptAi->adwParam[0] = D2CommonSeedRand(ptSeed, 3) + 3;
		}
	}
	else if (nDist > 20)
		eTurn = 4;
	else if (!eTurn)
	{
		bFlee = nLife <= 20 && nDist < 5 && D2CommonSeedRand(ptSeed, 100) < 40;
		if (!bFlee && D2CommonSeedRand(ptSeed, 100) >= (DWORD)(nHurt + 50))
			eTurn = 5;
		else if (bFlee)
			eTurn = 1;
		else
		{
			eTurn = 2;
			ptAi->adwParam[0] = D2CommonSeedRand(ptSeed, 3) + 3;
		}
	}
	switch (eTurn)
	{
	case 0:
		eTurn = 4;
		AiStand(ptGame, hUnit, 5);
		break;
	case 1:
		eTurn = 0;
		AiSetVelocity(hUnit, 0, 50, 0);
		if (AiWalkAway(ptGame, hUnit, hTarget, 8, TRUE))
			break;
		if (bReach)
		{
			AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK2, SKILL_PRIMEPOISONNOVA, hTarget, 0, 0);
			break;
		}
	case 2:
		if (!--ptAi->adwParam[0])
			eTurn = 0;
		if (ptAi->adwParam[1])
		{
			ptAi->adwParam[1]++;
			nDraw = (int)D2CommonSeedRand(ptSeed, 100);
			if (nDraw < 33)
				AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK2, SKILL_PRIMEBOLT, hTarget, 0, 0);
			else if (nDraw < 66)
				AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK2, SKILL_PRIMELIGHTNING, hTarget, 0, 0);
			else
			{
				SkillsSetLevel(hUnit, SKILL_MISSILESKILL1, 276, 0);
				AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK2, SKILL_MISSILESKILL1, hTarget, 0, 0);
			}
			if (D2CommonSeedRand(ptSeed, 100) < (DWORD)(50 - nHurt))
			{
				ptAi->adwParam[2] = eTurn;
				ptAi->adwParam[1] = 0;
				return;
			}
			break;
		}
		ptAi->adwParam[1] = 2;
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 3, 0);
		break;
	case 3:
		eTurn = 0;
		if (D2CommonSeedRand(ptSeed, 100) < (DWORD)(nHurt + 80))
		{
			if (D2CommonSeedRand(ptSeed, 100) < (DWORD)(80 - nHurt))
			{
				AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
				break;
			}
			AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK2, SKILL_PRIMEPOISONNOVA, hTarget, 0, 0);
			break;
		}
		AiSetVelocity(hUnit, 0, 50, 0);
		AiWalkToTargetFast(ptGame, hUnit, hTarget, 3, 0);
		break;
	case 4:
		eTurn = 0;
		AiSetVelocity(hUnit, 0, 50, 0);
		AiWalkAround(ptGame, hUnit, hTarget, 6);
		break;
	default:
		eTurn = 0;
		if (D2CommonSeedRand(ptSeed, 100) < 65)
		{
			if (D2CommonSeedRand(ptSeed, 100) >= 65 && nDist <= 5)
			{
				AiSetVelocity(hUnit, 0, 0, 4);
				AiWalkToTarget(ptGame, hUnit, hTarget, 7);
			}
			else
				AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, 0);
		}
		else
			AiStand(ptGame, hUnit, 10);
		break;
	}
	ptAi->adwParam[2] = eTurn;
}

void __fastcall AiThinkDiabloOut(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{

	AIGENERAL *ptAi;
	AICOMMAND *ptCommand;
	DWORD dwY;
	DWORD dwX;
	AICOMMAND tCommand;

	ptAi = ptParms->ptAi;
	dwY = 0;
	dwX = 0;
	ptCommand = AiFindCommand(hUnit, 10, FALSE);
	if (ptCommand)
	{
		dwX = ptCommand->dwArg1;
		dwY = ptCommand->dwArg2;
	}
	if (D2CommonStatsGetState(hUnit, 12))
		D2CommonStatsUnitSetActiveStateBit(hUnit, 12, 0);
	AiThinkSet(ptGame, hUnit, ptParms->ptAi, ptAi->nThinkKind);
	if (!AiFindCommand(hUnit, 10, FALSE) && dwX && dwY)
	{
		tCommand.nCommand = 10;
		tCommand.dwArg1 = dwX;
		tCommand.dwArg2 = dwY;
		AiPushCommand(ptGame, hUnit, &tCommand); }
	AiStand(ptGame, hUnit, 1);
}

void __fastcall AiThinkDiablo(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AICOMMAND *ptCommand;
	AICOMMAND tCommand;
	TARGET *ptTargetNode;
	UNIT *hOther;
	UNIT *hBest;
	int nBest;
	int nCount;
	UNIT *hBest2;
	int nBest2;
	int nX;
	int nY;
	int nDX;
	int nDY;
	int nDist;
	int nScore;
	int i;
	BYTE bAct;
	DWORD ptPath;
	AIGENERAL *ptAi;
	int eCmd;
	MONSTERSCALE tScale;
	int nGain;
	int nMaxLife;
	BOOL bDone;
	int nWalkX;
	int nWalkY;
	int nCmdX;
	int nCmdY;
	int nPointDist;
	int nSignX;
	int nSignY;
	DWORD dwPortal;
	TARGET **pptList;

	ptCommand = AiFindCommand(hUnit, 10, FALSE);
	if (!ptCommand)
	{
		tCommand.nCommand = 10;
		tCommand.dwArg1 = D2CommonUnitGetX(hUnit);
		tCommand.dwArg2 = D2CommonUnitGetY(hUnit);
		AiPushCommand(ptGame, hUnit, &tCommand);
		ptCommand = AiFindCommand(hUnit, 10, FALSE);
	}
	D2CommonUnitGetPathRoom(hUnit);
	hBest = NULL;
	nCount = 0;
	nBest = 0;
	ptTargetNode = ptGame->aptTargetList[0];
	pptList = ptGame->aptTargetList;
	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	bAct = hUnit->bAct;
	for (i = 0; i < 8; ptTargetNode = *++pptList, i++)
	{
		if (!ptTargetNode)
			continue;
		hOther = ptTargetNode->hUnit;
#line 10376
		D2ASSERT(UnitGetType (ptTargetNode->hUnit) == UNIT_PLAYER);
		if (bAct != hOther->bAct)
			continue;
		nDX = (int)D2CommonUnitGetX(hOther) - nX;
		nDY = (int)D2CommonUnitGetY(hOther) - nY;
		nDX = ABS(nDX);
		nDY = ABS(nDY);
		if (nDX > nDY)
			nDist = (nDY + nDX * 2) >> 1;
		else
			nDist = (nDX + nDY * 2) >> 2;
		if ((DWORD)nDist >= 55)
			continue;
		if (hOther->dwMode == 17 || !hOther->dwMode)
			nScore = 0;
		else
			nScore = AiThinkCalcFoeScore(hUnit, hOther, ptCommand);
		for (;;)
		{
			if (nBest < nScore)
			{
				nBest = nScore;
				hBest = hOther;
			}
			nCount++;
			ptTargetNode = ptTargetNode->ptNext;
			if (!ptTargetNode)
				break;
			hOther = ptTargetNode->hUnit;
			nScore = AiThinkCalcFoeScore(hUnit, hOther, ptCommand);
		}
	}
	for (; ptTargetNode; ptTargetNode = ptTargetNode->ptNext)
	{
		hOther = ptTargetNode->hUnit;
		if (bAct != hOther->bAct)
			continue;
		nScore = AiThinkCalcFoeScore(hUnit, hOther, ptCommand);
		if (nBest < nScore)
		{
			nBest = nScore;
			hBest = hOther;
		}
		nCount++;
	}
	ptTargetNode = pptList[1];
	hBest2 = NULL;
	nBest2 = 0;
	for (; ptTargetNode; ptTargetNode = ptTargetNode->ptNext)
	{
		hOther = ptTargetNode->hUnit;
		if (bAct != hOther->bAct)
			continue;
		nScore = AiThinkCalcFoeScore(hUnit, hOther, ptCommand);
		if (nBest2 < nScore)
		{
			nBest2 = nScore;
			hBest2 = hOther;
		}
	}
	if (hBest2)
	{
		nDX = (int)D2CommonUnitGetX(hBest2) - nX;
		nDY = (int)D2CommonUnitGetY(hBest2) - nY;
		nDX = ABS(nDX);
		nDY = ABS(nDY);
		if (nDX > nDY)
			nDist = (nDY + nDX * 2) >> 1;
		else
			nDist = (nDX + nDY * 2) >> 2;
		if (nDist < 5 && !D2CommonUnitTestPositionClear(hUnit, hBest, 0))
		{
			ptPath = (DWORD)hUnit->ptPath;
			D2CommonPathSetTarget(ptPath, hBest);
			D2CommonPathSetType(ptPath, 2);
			D2CommonPathCompute(ptPath, hUnit);
			if (!D2CommonPathGetStepCount(ptPath))
			{
				hBest = hBest2;
				nBest = nBest2;
			}
		}
	}
	ptAi = ptParms->ptAi;
	eCmd = AiThinkRollCommand(ptAi, hUnit, hBest, nBest, nCount, ptCommand);
	MonsterGetScale(&tScale, ptGame, D2CommonUnitGetPathRoom(hUnit), (DWORD)hUnit);
	if (ptAi->adwParam[1] && tScale.nLifePlayers > (int)ptAi->adwParam[1])
	{
		nGain = tScale.nLifePlayers - (int)ptAi->adwParam[1];
		if (nGain <= 0)
			nGain = 1;
		else if (nGain > 8)
			nGain = 8;
		nMaxLife = D2CommonStatsUnitCalcMaxHp(hUnit);
		D2CommonStatsUnitSetValue(hUnit, 6, (DWORD)D2CommonStatsUnitGetValue(hUnit, 6) * nGain);
		D2CommonStatsUnitSetValue(hUnit, 7, nMaxLife * nGain);
	}
	bDone = TRUE;
	switch (eCmd)
	{
	case 1:
		AiSetVelocity(hUnit, 0, 20, 0);
		nWalkY = (int)D2CommonUnitGetY(hBest);
		AiWalkToPoint(ptGame, hUnit, (int)D2CommonUnitGetX(hBest), nWalkY);
		break;
	case 2:
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hBest);
		break;
	case 3:
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hBest);
		break;
	case 4:
		AiSetModeOnTarget(ptGame, hUnit, 11, hBest);
		break;
	case 5:
		if (D2CommonStatsGetState(hUnit, 12))
		{
			D2CommonStatsUnitSetActiveStateBit(hUnit, 12, 0);
			AiStand(ptGame, hUnit, 2);
			break;
		}
		bDone = FALSE;
		AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_DIABLIGHT, hBest, 0, 0);
		break;
	case 6:
		AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_DIABFIRE, hBest, 0, 0);
		break;
	case 7:
		AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL2, SKILL_DIABCOLD, hBest, 0, 0);
		break;
	case 8:
		AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_DIABWALL, hBest, 0, 0);
		break;
	case 13:
		AiUseSkillInMode(ptGame, hUnit, 10, SKILL_PRIMEFIREWALL, hBest, 0, 0);
		break;
	case 9:
		AiUseSkillInMode(ptGame, hUnit, 10, SKILL_DIABPRISON, hBest, 0, 0);
		break;
	case 10:
		AiUseSkill(ptGame, hUnit, SKILL_DIABRUN, hBest, 0, 0);
		break;
	case 12:
		AiWalkToTargetFast(ptGame, hUnit, hBest, 4, 0);
		break;
	case 14:
		AiSetVelocity(hUnit, 0, 50, 100);
		if (AiWalkToPoint(ptGame, hUnit, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2))
			break;
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
		nWalkX = (int)D2CommonUnitGetX(hUnit);
		nWalkY = (int)D2CommonUnitGetY(hUnit);
		nCmdX = (int)ptCommand->dwArg1;
		nCmdY = (int)ptCommand->dwArg2;
		nPointDist = AiThinkPointDistance(hUnit, nCmdX, nCmdY);
		nSignY = 0;
		nSignX = 0;
		if (nWalkX < nCmdX)
			nSignX = -1;
		if (nWalkX > nCmdX)
			nSignX = 1;
		if (nWalkY < nCmdY)
			nSignY = -1;
		if (nWalkY > nCmdY)
			nSignY = 1;
		AiSetVelocity(hUnit, 0, 50, 100);
		if (AiWalkToPoint(ptGame, hUnit, (nPointDist >> 1) * nSignX + nCmdX,
			(nPointDist >> 1) * nSignY + nCmdY))
			break;
		EventRemoveByType(ptGame, hUnit, EVENT_RUN_AI);
		AiUseSkillInMode(ptGame, hUnit, 10, SKILL_PRIMEFIREWALL, hBest, 0, 0);
		break;
	case 15:
		if (hBest->eType != UNIT_PLAYER)
		{
			AiStand(ptGame, hUnit, 3);
			break;
		}
		dwPortal = PlayerGetPortalId(hBest);
		SUnitFind(ptGame, 2, dwPortal);
		AiUseSkillInMode(ptGame, hUnit, 10, SKILL_DIABPRISON, NULL, (int)dwPortal, 2);
		break;
	case 16:
		AiSetVelocity(hUnit, 0, 20, 0);
		AiStepAside(ptGame, hUnit, 5);
		break;
	default:
		AiStand(ptGame, hUnit, 10);
		break;
	}
	ptAi->adwParam[1] = (DWORD)tScale.nLifePlayers;
	if (bDone)
		ptAi->adwParam[0] = 0;
	else
		ptAi->adwParam[0] = (DWORD)eCmd;
}

int __fastcall AiThinkCalcFoeScore(UNIT *hUnit, UNIT *hOther, AICOMMAND *ptCommand)
{
	BOOL bReach;
	int nPlace;
	int nStat24;
	int nStat27;
	int nStat29;
	int nStat25;
	int nStat2b;
	int nAttack;
	int nStat16;
	int nStat31;
	int nStat33;
	int nStat35;
	int nStat37;
	int nDefence;
	int nState;
	int nLife;
	int nMaxLife;
	int nLow;
	void *hSkill;
	int nLevelL;
	int nLevelR;
	BYTE bRankL;
	BYTE bRankR;
	int nSkills;
	int nLine;
	int nClass;
	int nScore;

	bReach = D2CommonUnitTestPositionClear(hUnit, hOther, 0);
	nPlace = 0;
	if (ptCommand
		&& AiThinkEdgeDistance(hUnit, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2) > 85
		&& AiThinkEdgeDistance(hOther, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2) < 85)
		nPlace = 100;
	nStat24 = D2CommonStatsUnitGetValue(hOther, 36);
	nStat27 = D2CommonStatsUnitGetValue(hOther, 39);
	nStat29 = D2CommonStatsUnitGetValue(hOther, 41);
	nStat25 = D2CommonStatsUnitGetValue(hOther, 37);
	if (!bReach)
		nStat2b = 0;
	else
		nStat2b = D2CommonStatsUnitGetValue(hOther, 43);
	nAttack = (nStat2b + (nStat24 + nStat25 * 2) * 4 + nStat29 + nStat27) / 15;
	nStat16 = D2CommonStatsUnitGetValue(hOther, 22);
	nStat31 = D2CommonStatsUnitGetValue(hOther, 49);
	nStat33 = D2CommonStatsUnitGetValue(hOther, 51);
	nStat35 = D2CommonStatsUnitGetValue(hOther, 53);
	nStat37 = D2CommonStatsUnitGetValue(hOther, 55);
	nDefence = ((D2CommonStatsUnitGetValue(hOther, 58) >> 8) + nStat37 + nStat35 + nStat33 + nStat31
		+ nStat16) / 2;
	if (D2CommonStatsGetState(hOther, 11))
		nState = 100;
	else
		nState = 0;
	nLife = D2CommonStatsUnitGetValue(hOther, 6);
	nMaxLife = D2CommonStatsUnitCalcMaxHp(hOther);
	if ((BOOL)(!nMaxLife || nLife * 100 / nMaxLife < 20))
		nLow = 100;
	else
		nLow = 0;
	nLevelL = nLevelR = 0;
	bRankL = bRankR = 0;
	hSkill = (void *)D2CommonUnitGetLeftSkill(hOther);
	if (hSkill)
	{
		nLevelL = (int)D2CommonSkillsCalcNodeLevel(hOther, hSkill, 1);
		bRankL = ((SKILLSTXT *)D2CommonDataTblsGetSkill(D2CommonSkillsGetNodeType(hSkill)))->AttackRank;
	}
	hSkill = (void *)D2CommonUnitGetRightSkill(hOther);
	if (hSkill)
	{
		nLevelR = (int)D2CommonSkillsCalcNodeLevel(hOther, hSkill, 1);
		bRankR = ((SKILLSTXT *)D2CommonDataTblsGetSkill(D2CommonSkillsGetNodeType(hSkill)))->AttackRank;
	}
	nSkills = (bRankR * nLevelR + bRankL * nLevelL) / 4;
	nLine = 0;
	if (bReach)
		nLine = 100;
	else if (UnitLineClear(hUnit, hOther))
		nLine = 75;
	if (D2CommonStatsUnitGetAlignment(hOther) == 1 && hOther->eType == UNIT_MONSTER)
	{
		nClass = (int)D2CommonMonsterUnitGetTypeLocalIndex(hOther);
		if (nClass >= 340 && nClass <= 343)
			return 0;
	}
	nScore = ((nPlace + nLine) * 5 + nAttack + (nSkills + nState * 2 + nDefence) * 2 + nLow * 3) / 22;
	if (!nScore)
		nScore = 1;
	return nScore;
}

int __fastcall AiThinkRollCommand(AIGENERAL *ptAi, UNIT *hUnit, UNIT *hBest,
	int nBest, int nCount, AICOMMAND *ptCommand)
{
	GAME *ptGame;
	UNIT *hPortal;
	BOOL bPortalNear;
	BOOL bSkill;
	void *hSkill;
	DWORD dwNodeType;
	BOOL bFar;
	BOOL bVeryFar;
	int nDist;
	BOOL bReach;
	BOOL bLine;
	int nStat27;
	int nStat29;
	D2SEED *ptSeed;
	BOOL bDone;
	int anWeight[17];
	int nLife;
	MONSTERSCALE tScale;
	int nTotal;
	int nPick;
	int *pnWeight;
	int i;

	if (ptAi->adwParam[0])
		return (int)ptAi->adwParam[0];
	if (!hBest)
	{
		if (D2CommonCollisnTestPatternAnyRaw(D2CommonUnitGetPathRoom(hUnit), D2CommonUnitGetX(hUnit),
			D2CommonUnitGetY(hUnit), 2, 64))
			return 16;
		if (D2CommonSeedRand(&hUnit->tSeed, 1000) < 1)
			return 4;
		else
			return 11;
	}
	ptGame = SUnitGetGame(hBest);
	if (!ptGame)
		ptGame = SUnitGetGame(hUnit);
	hPortal = NULL;
	bPortalNear = FALSE;
	if (hBest->eType == UNIT_PLAYER)
	{
		hPortal = SUnitFind(ptGame, 2, PlayerGetPortalId(hBest));
		if (hPortal && D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hPortal)) == 108
			&& (DWORD)D2CommonUnitUtilGetDistance(hPortal, ptCommand->dwArg1, ptCommand->dwArg2) < 85)
			bPortalNear = TRUE;
	}
	bSkill = FALSE;
	hSkill = (void *)D2CommonUnitGetLeftSkill(hBest);
	if (hSkill)
	{
		dwNodeType = D2CommonSkillsGetNodeType(hSkill);
		if (dwNodeType == 59 || dwNodeType == 56)
			bSkill = TRUE;
		if (dwNodeType == 51)
		{
			if ((int)D2CommonSkillsCalcNodeLevel(hBest, hSkill, 1) > 3)
				bSkill = TRUE;
		}
		else if (dwNodeType == 27 && (int)D2CommonSkillsCalcNodeLevel(hBest, hSkill, 1) > 7)
			bSkill = TRUE;
	}
	hSkill = (void *)D2CommonUnitGetRightSkill(hBest);
	if (hSkill)
	{
		dwNodeType = D2CommonSkillsGetNodeType(hSkill);
		if (dwNodeType == 59 || dwNodeType == 56)
			bSkill = TRUE;
		if (dwNodeType == 51)
		{
			if ((int)D2CommonSkillsCalcNodeLevel(hBest, hSkill, 1) > 3)
				bSkill = TRUE;
		}
		else if (dwNodeType == 27 && (int)D2CommonSkillsCalcNodeLevel(hBest, hSkill, 1) > 7)
			bSkill = TRUE;
	}
	bVeryFar = FALSE;
	bFar = FALSE;
	if (ptCommand)
	{
		nDist = AiThinkEdgeDistance(hBest, (int)ptCommand->dwArg1, (int)ptCommand->dwArg2);
		if (nDist > 85)
			bFar = TRUE;
		if (nDist > 105)
			bVeryFar = TRUE;
	}
	bReach = D2CommonUnitTestPositionClear(hUnit, hBest, 0);
	bLine = UnitLineClear(hUnit, hBest);
	nStat27 = D2CommonStatsUnitGetValue(hBest, 39);
	nStat29 = D2CommonStatsUnitGetValue(hBest, 41);
	ptSeed = &hUnit->tSeed;
	bDone = FALSE;
	if (bReach)
	{
		int anSet[17] = { 0, 0, 40, 70, 0, 40, 24, 40, 15, 0, 0, 0, 0, 0, 0, 0, 0 };

		memcpy(anWeight, anSet, sizeof(anWeight));
		nLife = D2CommonStatsUnitGetValue(hBest, 6);
		if (nLife * 100 / D2CommonStatsUnitCalcMaxHp(hBest) < 20)
			anWeight[2] = 50;
		if (D2CommonStatsGetState(hBest, 11))
		{
			anWeight[7] = 0;
			anWeight[8] = 0;
		}
		if (nStat27 > nStat29)
			anWeight[6] -= 10;
		if (nStat27 < nStat29)
			anWeight[6] += 10;
		if (!bLine)
		{
			anWeight[5] = 0;
			anWeight[6] = 0;
		}
		if (bPortalNear)
			anWeight[15] = 10;
		bDone = TRUE;
	}
	if (!bLine)
	{
		if (!bDone)
		{
			int anSet[17] = { 0, 0, 0, 0, 5, 0, 25, 0, 25, 40, 0, 0, 25, 0, 0, 0, 0 };

			memcpy(anWeight, anSet, sizeof(anWeight));
			if (nCount < 2)
			{
				anWeight[6] = 0;
				anWeight[1] = 25;
				anWeight[8] -= 5;
				anWeight[9] = 0;
			}
			if (bSkill && !bFar)
				anWeight[13] = 15;
			else if (bSkill || bFar)
			{
				anWeight[13] = 25;
				anWeight[1] = 0;
				anWeight[12] = 15;
				if (!anWeight[9])
					anWeight[9] = 20;
				if (nCount < 2)
					anWeight[9] -= 5;
			}
			if (bVeryFar)
				anWeight[14] = 60;
			if (bPortalNear)
				anWeight[15] = 20;
		}
	}
	else if (!bDone)
	{
		int anSet[17] = { 0, 0, 0, 0, 0, 25, 25, 0, 15, 20, 10, 0, 20, 0, 0, 0, 0 };

		memcpy(anWeight, anSet, sizeof(anWeight));
		if ((DWORD)AiThinkDistance(hUnit, hBest) > 25)
		{
			anWeight[10] = 20;
			anWeight[5] = 0;
			anWeight[8] -= 5;
		}
		if (nStat27 > nStat29)
		{
			anWeight[6] -= 10;
			anWeight[8] -= 10;
		}
		if (nStat27 < nStat29)
		{
			anWeight[6] += 10;
			anWeight[8] += 10;
		}
		if (nCount < 2)
			anWeight[6] -= 10;
		if (nCount > 3)
			anWeight[6] += 5;
		if ((DWORD)nBest > 60)
			anWeight[9] += 10;
		MonsterGetScale(&tScale, ptGame, D2CommonUnitGetPathRoom(hUnit), (DWORD)hUnit);
		if (nCount < 2 && tScale.nDifficulty < 2)
			anWeight[9] = 0;
		if (bSkill && !bFar)
		{
			anWeight[10] = 30;
			anWeight[13] = 15;
			anWeight[6] += 10;
		}
		else if (bSkill || bFar)
		{
			anWeight[1] = 0;
			anWeight[12] = 10;
			anWeight[10] = 0;
			anWeight[13] = 15;
			if (!anWeight[9])
				anWeight[9] = 10;
		}
		if (bVeryFar)
		{
			anWeight[9] = 20;
			anWeight[14] = 60;
		}
		if (bPortalNear)
			anWeight[15] = 15;
	}
	if (MonsterCanUseSkill(ptGame, hUnit, 199, hBest, 0, 0))
		anWeight[9] = 0;
	if (anWeight[15] && hPortal && !MonsterCanUseSkill(ptGame, hUnit, 199, hPortal, 0, 0))
		anWeight[15] = 0;
	pnWeight = anWeight;
	nTotal = 0;
	for (i = 0; i < 17; i++)
		nTotal += *pnWeight++;
	if (nTotal)
		nPick = (int)D2CommonSeedRand(ptSeed, nTotal);
	else
		nPick = 0;
	nTotal = 0;
	for (i = 0, pnWeight = anWeight; i < 17; pnWeight++, i++)
	{
		nTotal += *pnWeight;
		if (nPick < nTotal)
			return i;
	}
	return 11;
}

void __fastcall AiThinkStandUntilExpiredInit(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;

	ptAi = ptParms->ptAi;
	ptAi->adwParam[0] = D2CommonSkillsGetTypeParamTwo(SKILL_BONE_WALL) + ptGame->nFrame;
}

void __fastcall AiThinkStandUntilExpired(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	if ((int)ptParms->ptAi->adwParam[0] < ptGame->nFrame)
	{
		AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
		return;
	}
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkStandOrSkillMode(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < 20)
	{
		AiSetModeAtPoint(ptGame, hUnit, MONMODE_SKILL1, 0, 0);
		return;
	}
	AiStand(ptGame, hUnit, 30);
}

void __fastcall AiThinkSucker(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	AIGENERAL *ptAi;
	MONSTATSTXT *ptMonStats;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptAi = ptParms->ptAi;
	ptMonStats = ptParms->ptMonStats;
	ptSeed = &hUnit->tSeed;
	if (ptParms->fTargetInReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2] || !ptAi->adwParam[1])
		{
			ptAi->adwParam[1]++;
			if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[3])
			{
				AiUseSkill(ptGame, hUnit, SKILL_MOSQUITO, hTarget, 0, 0);
				return;
			}
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		if (D2CommonSeedRand(ptSeed, 100) > 20)
		{
			AiStand(ptGame, hUnit, 15);
			return;
		}
		ptAi->adwParam[0] = 2;
		ptAi->adwParam[1] = 0;
	}
	if (ptAi->adwParam[0] == 0)
	{
		AiSetVelocity(hUnit, 13, 100, 0);
		AiWalkToTarget(ptGame, hUnit, hTarget, 0);
		return;
	}
	if (ptAi->adwParam[0] == 1)
	{
		AiSetVelocity(hUnit, 0, 50, 0);
		AiStepAside(ptGame, hUnit, 4);
		if ((int)++ptAi->adwParam[1] > 5)
		{
			ptAi->adwParam[0] = 0;
			ptAi->adwParam[1] = 0;
		}
		return;
	}
	if (ptAi->adwParam[0] == 2)
	{
		AiSetVelocity(hUnit, 2, 100, 0);
		AiWalkAway(ptGame, hUnit, hTarget, 10, TRUE);
		ptAi->adwParam[0] = 1;
		ptAi->adwParam[1] = 0;
		return;
	}
	ptAi->adwParam[0] = 0;
	ptAi->adwParam[1] = 0;
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkHydra(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;

	if ((int)ptParms->ptAi->adwParam[0] < ptGame->nFrame)
	{
		AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
		return;
	}
	hTarget = ptParms->hTarget;
	if (hTarget && ptParms->nDist < 25 && D2CommonSeedRand(&hUnit->tSeed, 100) < 60)
	{
		SkillsSetLevel(hUnit, SKILL_HIREABLEMISSILE, 247, 0);
		AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK1, SKILL_HIREABLEMISSILE, hTarget, 0, 0);
		return;
	}
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkTrapFireBolt(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget = ptParms->hTarget;
	int nDist = ptParms->nDist;
	AIGENERAL *ptAi;
	if (!hTarget) {
		hUnit->dwFlags0 |= 131072;
		AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
		return;
	}
	ptAi = ptParms->ptAi;
	if (nDist < 20 && ptAi->adwParam[1] != 1) {
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		ptAi->adwParam[1]++;
		return;
	}
	hUnit->dwFlags0 |= 131072;
	AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
}

void __fastcall AiThinkTrapPoison(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	AIGENERAL *ptAi;
	int nDist;

	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	if (!hTarget)
	{
		hUnit->dwFlags0 |= 131072;
		AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
		return;
	}
	if (nDist < 20)
	{
		ptAi = ptParms->ptAi;
		if (ptAi->adwParam[1] == 1)
		{
			hUnit->dwFlags0 |= 131072;
			AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
			return;
		}
		AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK1, SKILL_PRIMEPOISONNOVA, hTarget, 0, 0);
		ptAi->adwParam[1]++;
		return;
	}
	hUnit->dwFlags0 |= 131072;
	AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
	AiStand(ptGame, hUnit, 40);
}

void __fastcall AiThinkTrapNova(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	AIGENERAL *ptAi;
	int nDist;

	hTarget = ptParms->hTarget;
	nDist = ptParms->nDist;
	if (!hTarget)
	{
		hUnit->dwFlags0 |= 131072;
		AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
		return;
	}
	if (nDist < 20)
	{
		ptAi = ptParms->ptAi;
		if (ptAi->adwParam[1] == 1)
		{
			hUnit->dwFlags0 |= 131072;
			AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
			return;
		}
		AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_NOVA, hTarget, 0, 0);
		ptAi->adwParam[1]++;
		return;
	}
	hUnit->dwFlags0 |= 131072;
	AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
	AiStand(ptGame, hUnit, 40);
}

void __fastcall AiThinkTrapAlignedX(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	GAMECOORD tPos;
	GAMECOORD tTargetPos;
	int nLevel;
	MONREGION *ptRegion;

	hTarget = ptParms->hTarget;
	if (ptParms->nDist >= 15 || ptParms->nDist <= 5)
	{
		AiStand(ptGame, hUnit, 40);
		return;
	}
	D2CommonUnitGetPosition(hUnit, &tPos);
	D2CommonUnitGetPosition(hTarget, &tTargetPos);
	if (ABS((int)tPos.dwX - (int)tTargetPos.dwX) > 2
		|| ptGame->nFrame - (int)ptParms->ptAi->adwParam[2] <= 0)
	{
		AiStand(ptGame, hUnit, 30);
		return;
	}
	ptParms->ptAi->adwParam[2] = ptGame->nFrame + 30;
	nLevel = (int)D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hUnit));
	ptRegion = (MONREGION *)D2CommonMonsterRegionArrayGetDword(&ptGame->pMonRegions, nLevel);
	if (ptRegion->nTrapRoll < 0)
	{
		if (nLevel < 40)
			ptRegion->nTrapRoll = 0;
		else
			ptRegion->nTrapRoll = D2CommonSeedBetween(&hUnit->tSeed, 0, 3);
	}
	switch (ptRegion->nTrapRoll)
	{
	case 1:
		ptParms->ptAi->adwParam[2] = ptGame->nFrame + 200;
		AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_POISONBALLTRAP, hTarget, 0, 0);
		return;
	}
	AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
}

void __fastcall AiThinkTrapAlignedY(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	GAMECOORD tPos;
	GAMECOORD tTargetPos;
	int nLevel;
	MONREGION *ptRegion;

	hTarget = ptParms->hTarget;
	if (ptParms->nDist >= 15 || ptParms->nDist <= 5)
	{
		AiStand(ptGame, hUnit, 40);
		return;
	}
	D2CommonUnitGetPosition(hUnit, &tPos);
	D2CommonUnitGetPosition(hTarget, &tTargetPos);
	if (ABS((int)tPos.dwY - (int)tTargetPos.dwY) > 2
		|| ptGame->nFrame - (int)ptParms->ptAi->adwParam[2] <= 0)
	{
		AiStand(ptGame, hUnit, 30);
		return;
	}
	ptParms->ptAi->adwParam[2] = ptGame->nFrame + 30;
	nLevel = (int)D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hUnit));
	ptRegion = (MONREGION *)D2CommonMonsterRegionArrayGetDword(&ptGame->pMonRegions, nLevel);
	if (ptRegion->nTrapRoll < 0)
	{
		if (nLevel < 40)
			ptRegion->nTrapRoll = 0;
		else
			ptRegion->nTrapRoll = D2CommonSeedBetween(&hUnit->tSeed, 0, 3);
	}
	switch (ptRegion->nTrapRoll)
	{
	case 1:
		ptParms->ptAi->adwParam[2] = ptGame->nFrame + 200;
		AiUseSkillInMode(ptGame, hUnit, MONMODE_SKILL1, SKILL_POISONBALLTRAP, hTarget, 0, 0);
		return;
	}
	AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
}

void __fastcall AiThinkPillar(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;

	hTarget = ptParms->hTarget;
	if (ptParms->fTargetInReach)
	{
		if (D2CommonSeedRand(&hUnit->tSeed, 100) < 70)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		AiStand(ptGame, hUnit, 20);
		return;
	}
	AiStand(ptGame, hUnit, 40);
}

void __fastcall AiThinkTrappedSoul(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	AIGENERAL *ptAi;
	int nDist;
	BOOL bReach;
	GAMECOORD tPos;
	GAMECOORD tTargetPos;

	hTarget = ptParms->hTarget;
	ptAi = ptParms->ptAi;
	nDist = ptParms->nDist;
	bReach = ptParms->fTargetInReach;
	hUnit->dwFlags0 |= 131072;
	D2CommonUnitGetPosition(hUnit, &tPos);
	if (!hTarget || nDist >= 5)
	{
		if (ptAi->adwParam[0])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_SKILL1, hTarget);
			return;
		}
		AiStand(ptGame, hUnit, 15);
		return;
	}
	if (!ptAi->adwParam[0])
	{
		ptAi->adwParam[0] = 1;
		ptAi->adwParam[1] = (DWORD)ptGame->nFrame;
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_SKILL2, hTarget);
		return;
	}
	if (!bReach || ptGame->nFrame <= (int)ptAi->adwParam[1])
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_SKILL1, hTarget);
		return;
	}
	D2CommonUnitGetPosition(hTarget, &tTargetPos);
	if (tPos.dwX >= tTargetPos.dwX && tPos.dwY <= tTargetPos.dwY)
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
		ptAi->adwParam[1] = (DWORD)(ptGame->nFrame + 35);
		return;
	}
	if (tPos.dwX <= tTargetPos.dwX && tPos.dwY >= tTargetPos.dwY)
	{
		AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK2, hTarget);
		ptAi->adwParam[1] = (DWORD)(ptGame->nFrame + 35);
		return;
	}
	AiSetModeOnTarget(ptGame, hUnit, MONMODE_SKILL1, hTarget);
	ptAi->adwParam[1] = (DWORD)(ptGame->nFrame + 5);
}

void __fastcall AiThinkIzual(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	AIGENERAL *ptAi;
	int nDist;
	BOOL bReach;
	MONSTATSTXT *ptMonStats;
	int nX;
	int nY;
	D2SEED *ptSeed;

	nDist = ptParms->nDist;
	hTarget = ptParms->hTarget;
	bReach = ptParms->fTargetInReach;
	ptSeed = &hUnit->tSeed;
	ptMonStats = ptParms->ptMonStats;
	ptAi = ptParms->ptAi;
	if (!ptAi->adwParam[0])
	{
		ptAi->adwParam[0] = 1;
		A4Q1MonsterSeen(ptGame);
	}
	if (bReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[0])
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		AiStand(ptGame, hUnit, 16);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[1])
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, 7);
		return;
	}
	nX = (int)D2CommonUnitGetX(hTarget);
	nY = (int)D2CommonUnitGetY(hTarget);
	if (D2CommonSeedRand(ptSeed, 100) < ptMonStats->AIParam[2] && nDist < 10)
	{
		AiUseSkillInMode(ptGame, hUnit, MONMODE_CAST, SKILL_FROST_NOVA, hTarget, nX, nY);
		AiThinkIn(ptGame, hUnit, 48);
		return;
	}
	if (nDist > 10)
	{
		AiWalkToDistance(ptGame, hUnit, hTarget, 6, 9);
		return;
	}
	AiStand(ptGame, hUnit, 16);
}

void __fastcall AiThinkSpawnWhenNearInit(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi = ptParms->ptAi;

	ptAi->adwParam[0] = (DWORD)ptGame->nFrame;
	ptAi->adwParam[1] = 4;
}

void __fastcall AiThinkSpawnWhenNear(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	GAMEROOM *hRoom;
	int nClass;
	int nX;
	int nY;

	if (ptParms->nDist > 5)
	{
		AiStand(ptGame, hUnit, 10);
		return;
	}
	ptAi = ptParms->ptAi;
	if (!ptAi->adwParam[1])
	{
		AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
		return;
	}
	if (ABS(ptGame->nFrame - (int)ptAi->adwParam[0]) >= 125)
	{
		hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
		nClass = (int)D2CommonMonsterResolvePopulationIndex(hRoom, 96);
		if (MonsterFindSpawnPoint(ptGame, hRoom, NULL, nClass, &nX, &nY, FALSE)
			&& MonsterSpawn(ptGame, hRoom, nX, nY, nClass, 1, 2, 66))
			ptAi->adwParam[1]--;
	}
	AiStand(ptGame, hUnit, 10);
}

void __fastcall AiThinkAttackOnceThenDie(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	int nX;
	int nY;

	ptAi = ptParms->ptAi;
	if (ptAi->adwParam[1] == 1)
	{
		hUnit->dwFlags0 |= 131072;
		AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
		return;
	}
	AiSetModeAtPoint(ptGame, hUnit, MONMODE_ATTACK1, (int)D2CommonUnitGetX(hUnit) - 10, (int)D2CommonUnitGetY(hUnit));
	ptAi->adwParam[1] = 1;
}

void __fastcall AiThinkYoungdiablo(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	AIGENERAL *ptAi;
	int nDist;
	GAMECOORD tSpot;

	nDist = ptParms->nDist;
	hTarget = ptParms->hTarget;
	ptAi = ptParms->ptAi;
	if (!A3Q7GetSpot(ptGame, hUnit, &tSpot))
	{
		AiStand(ptGame, hUnit, 10);
		return;
	}
	if (!ptAi->adwParam[0])
	{
		ptAi->adwParam[0] = 1;
		ptAi->adwParam[1] = 0;
		ptAi->adwParam[2] = 0;
	}
	if (!hTarget)
	{
		AiStand(ptGame, hUnit, 10);
		return;
	}
	if (nDist < 20)
	{
		if (ptAi->adwParam[0] == 1)
		{
			AiWalkToPoint(ptGame, hUnit, (int)tSpot.dwX, (int)tSpot.dwY);
			ptAi->adwParam[0] = 2;
			return;
		}
		if (ptAi->adwParam[0] == 2)
		{
			if ((DWORD)AiThinkPointDistance(hUnit, (int)tSpot.dwX, (int)tSpot.dwY) < 2)
			{
				hUnit->dwFlags0 |= 131072;
				AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
				A3Q7StartTimer(ptGame, hUnit);
				return;
			}
			if ((int)ptAi->adwParam[1] < 3)
			{
				ptAi->adwParam[1]++;
				AiSetVelocity(hUnit, 1, 0, 0);
				AiWalkToPoint(ptGame, hUnit, (int)tSpot.dwX, (int)tSpot.dwY);
				return;
			}
			hUnit->dwFlags0 |= 131072;
			AiSetModeAtPoint(ptGame, hUnit, 0, 0, 0);
			A3Q7StartTimer(ptGame, hUnit);
			return;
		}
	}
	AiStand(ptGame, hUnit, 40);
}

void __fastcall AiThinkFleshSpawner(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	AIGENERAL *ptAi;
	BOOL bReach;
	int nRunCount;
	int nRunIndex;
	AIFINDRECORD tFind;
	BYTE abRange[8];
	BYTE bDir;
	BYTE bRange;
	int nOffsetX;
	int nOffsetY;
	int nX;
	int nY;
	int i;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptSeed = &hUnit->tSeed;
	bReach = ptParms->fTargetInReach;
	ptAi = ptParms->ptAi;
	D2CommonMonsterTblsGetRun(hUnit->dwClassId, &nRunCount, &nRunIndex);
	tFind.tClass.dwClassId = (DWORD)(nRunIndex + 301);
	tFind.tClass.nMax = 25;
	tFind.tClass.nCount = 0;
	AiFind(ptGame, hUnit, &tFind, NULL, 12);
	if ((int)ptAi->adwParam[0] < 16 && tFind.tClass.nCount < 5
		&& D2CommonSeedRand(ptSeed, 100) < 35 && D2CommonStatsUnitGetAlignment(hUnit) == 2)
	{
		bDir = (BYTE)D2CommonUnitSetPositionExtra(hUnit, D2CommonUnitGetX(hTarget), D2CommonUnitGetY(hTarget));
		bDir = D2CommonMonsterRegionLookupTableB(bDir);
		abRange[0] = 6;
		abRange[1] = 4;
		abRange[2] = 2;
		abRange[3] = 2;
		abRange[4] = 2;
		abRange[5] = 0;
		abRange[6] = 6;
		abRange[7] = 6;
		for (i = 0; i < 8; i++)
		{
			if (bDir > 8)
				bDir = 0;

			D2CommonMonsterRegionGetOffsetPair(abRange[bDir], &nOffsetX, &nOffsetY);
			nX = (int)D2CommonUnitGetX(hUnit) + nOffsetX * 3;
			nY = (int)D2CommonUnitGetY(hUnit) + nOffsetY * 3;
			if (MonsterHasRoom(298, (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit), nX, nY, FALSE))
			{
				AiUseSkill(ptGame, hUnit, SKILL_NEST, NULL, nX, nY);
				ptAi->adwParam[0]++;
				return;
			}
			bDir++;
		}
	}
	if (bReach)
	{
		if (D2CommonSeedRand(ptSeed, 100) < 80)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
		AiStand(ptGame, hUnit, 10);
		return;
	}
	if (D2CommonSeedRand(ptSeed, 100) < 70)
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, 7);
		return;
	}
	AiWalkToTargetFast(ptGame, hUnit, hTarget, 4, FALSE);
}

void __fastcall AiThinkFleshBeast(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	AIGENERAL *ptAi;

	DWORD fReach, dwState;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	fReach = ptParms->fTargetInReach;
	ptAi = ptParms->ptAi;
	dwState = ptAi->adwParam[0];
	ptSeed = &hUnit->tSeed;
	if (!dwState)
	{
		ptAi->adwParam[0] = 1;
		AiStand(ptGame, hUnit, 5);
		return;
	}
	if (fReach)
	{
		if ((D2CommonSeedNext(ptSeed) & 255) < 205)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
	}
	else if ((D2CommonSeedNext(ptSeed) & 255) < 205)
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, 7);
		return;
	}
	AiStand(ptGame, hUnit, 10);
}

int __fastcall AiGetKind(AIRECORDHEAD *ptRecord)
{
	return ptRecord->field_00;
}

void __fastcall AiSetParams(AIGENERAL *ptAi, int nParam0, int nParam1,
	int nParam2)
{
	if (!ptAi)
		return;
	if (nParam0 != AIPARAM_KEEP)
		ptAi->adwParam[0] = (DWORD)nParam0;
	if (nParam1 != AIPARAM_KEEP)
		ptAi->adwParam[1] = (DWORD)nParam1;
	if (nParam2 != AIPARAM_KEEP)
		ptAi->adwParam[2] = (DWORD)nParam2;
}

BOOL __fastcall AiThinkKindApplies(UNIT *hUnit, MONSTATSTXT *ptMonStats, int nKind,
	BOOL bUnique)
{
	if (!nKind)
		return TRUE;
	if (nKind != 10 && nKind != 11 && nKind != 12)
		return TRUE;
	if (bUnique && MonsterUniqueIsSuperUnique(MONDATA(hUnit)->ptUnique))
		return FALSE;
	return (BOOL)D2CommonUnitTestMonsterCombinedFlag(hUnit->dwClassId);
}
struct AITHINKROW
{
 	int			nIdle;
 	PFNAITHINK	pfnThinkInit;
 	PFNAITHINK	pfnThink;
 	PFNAITHINK	pfnThinkOut;
};
typedef struct AITHINKROW AITHINKROW;

static AITHINKROW sgatAiThinks[AITHINK_ROWS + AITHINK_SPECIALS] =
{
	{ 0, NULL, NULL, NULL },
	{ 1, NULL, AiThinkSkeleton, NULL },
	{ 1, NULL, AiThinkZombie, NULL },
	{ 1, NULL, AiThinkAfflicted, NULL },
	{ 1, NULL, AiThinkFoulCrow, NULL },
	{ 1, NULL, AiThinkFallen, NULL },
	{ 1, NULL, AiThinkBrute, NULL },
	{ 1, NULL, AiThinkSandRaider, NULL },
	{ 1, NULL, NULL, NULL },
	{ 1, NULL, AiThinkGhost, NULL },
	{ 1, NULL, AiThinkDarkHunter, NULL },
	{ 1, NULL, AiThinkDuneBeast, NULL },
	{ 1, NULL, AiThinkMoonClan, NULL },
	{ 1, NULL, AiThinkFallenShaman, NULL },
	{ 1, NULL, AiThinkQuillRat, NULL },
	{ 4, NULL, AiThinkSandMaggot, AiThinkSandMaggotOut },
	{ 1, NULL, AiThinkTombViper, NULL },
	{ 1, NULL, AiThinkSandLeaper, NULL },
	{ 1, NULL, AiThinkHuntress, NULL },
	{ 1, NULL, AiThinkItchies, NULL },
	{ 1, NULL, AiThinkDungSoldier, NULL },
	{ 1, NULL, AiThinkDriedCorpse, NULL },
	{ 1, NULL, AiThinkHollowOne, NULL },
	{ 1, NULL, NULL, NULL },
	{ 1, NULL, AiThinkCarrionBird, NULL },
	{ 1, NULL, AiThinkSucker, NULL },
	{ 1, NULL, AiThinkGloam, NULL },
	{ 1, NULL, AiThinkArach, NULL },
	{ 1, NULL, AiThinkThornedHulk, NULL },
	{ 2, NULL, AiThinkGhoulLord, NULL },
	{ 1, NULL, AiThinkDesertWing, AiThinkDesertWingOut },
	{ 2, NULL, AiThinkRatManHitAndBackOff, NULL },
	{ 0, NULL, AiThinkDeckardCain, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNone, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 1, NULL, AiThinkHellMeteor, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 1, NULL, AiThinkAndariel, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 1, NULL, AiThinkDarkRanger, NULL },
	{ 1, NULL, AiThinkDarkSpearwoman, NULL },
	{ 1, NULL, AiThinkSkeletonArcher, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNone, NULL },
	{ 1, NULL, AiThinkSandMaggotYoung, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 1, NULL, AiThinkBlunderbore, NULL },
	{ 1, NULL, AiThinkSandMaggotEgg, NULL },
	{ 0, NULL, AiThinkNpcCommandOrPath, NULL },
	{ 0, NULL, AiThinkNpcCommandOrPath, NULL },
	{ 0, NULL, AiThinkNpcCommandOrPath, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNpcCommandOrPath, NULL },
	{ 0, NULL, AiThinkStandOrSkillMode, NULL },
	{ 0, NULL, AiThinkStandOrSkillMode, NULL },
	{ 1, AiThinkFoulCrowNestInit, AiThinkFoulCrowNest, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 1, NULL, AiThinkDuriel, NULL },
	{ 2, NULL, AiThinkRatManHitAndBackOff, NULL },
	{ 1, NULL, NULL, NULL },
	{ 1, NULL, NULL, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 1, AiThinkFoulCrowNestInit, AiThinkMummyGenerator, NULL },
	{ 1, NULL, AiThinkHollowOne, NULL },
	{ 1, NULL, AiThinkAttackThenDie, NULL },
	{ 1, NULL, AiThinkAttackThenDie, NULL },
	{ 1, NULL, AiThinkAttackThenDie, NULL },
	{ 1, NULL, AiThinkAttackThenDie, NULL },
	{ 1, NULL, AiThinkFlyingScimitar, NULL },
	{ 1, NULL, AiThinkZakarumite, NULL },
	{ 1, NULL, AiThinkSexton, NULL },
	{ 1, NULL, AiThinkMephisto, NULL },
	{ 0, NULL, AiThinkDiablo, AiThinkDiabloOut },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 5, NULL, AiThinkSwampDweller, AiThinkSwampDwellerOut },
	{ 1, NULL, AiThinkSummoner, NULL },
	{ 0, NULL, AiThinkTyrael, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 1, NULL, AiThinkIzual, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 1, NULL, AiThinkWaterWatcherLimb, NULL },
	{ 1, NULL, AiThinkWaterWatcherHead, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkNavi, NULL },
	{ 1, AiThinkBloodravenInit, AiThinkBloodraven, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkRogueScout, NULL },
	{ 0, NULL, AiThinkHireling, AiThinkResetThinkOut },
	{ 1, NULL, AiThinkAttackInSight, NULL },
	{ 1, NULL, AiThinkGargoyleTrap, NULL },
	{ 1, NULL, AiThinkReturnedMage, NULL },
	{ 1, NULL, AiThinkRatManShaman, AiThinkResetThinkOut },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkSandMaggotQueen, NULL },
	{ 0, NULL, AiThinkFollowOwner, NULL },
	{ 0, NULL, AiThinkFollowOwner, NULL },
	{ 0, NULL, AiThinkFollowOwner, NULL },
	{ 0, NULL, AiThinkFollowOwner, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkNpcCommandOrPath, NULL },
	{ 1, NULL, AiThinkDuneBeast, NULL },
	{ 0, NULL, AiThinkNpcCommandOrPath, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 1, NULL, AiThinkFleshSpawner, NULL },
	{ 1, NULL, AiThinkFleshBeast, NULL },
	{ 1, NULL, AiThinkGroper, NULL },
	{ 1, NULL, AiThinkCorpulent, NULL },
	{ 1, NULL, AiThinkDoomKnight, NULL },
	{ 1, NULL, AiThinkAbyssKnight, NULL },
	{ 1, NULL, AiThinkOblivionKnight, NULL },
	{ 1, NULL, AiThinkQuillBear, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 1, AiThinkSpawnBatchInit, AiThinkSpawnBatch, NULL },
	{ 2, NULL, AiThinkTrapFireBolt, NULL },
	{ 1, NULL, AiThinkTrapAlignedX, NULL },
	{ 1, NULL, AiThinkTrapAlignedY, NULL },
	{ 2, NULL, AiThinkTrapPoison, NULL },
	{ 2, NULL, AiThinkTrapFireBolt, NULL },
	{ 0, NULL, AiThinkKaelan, NULL },
	{ 1, AiThinkSpawnWhenNearInit, AiThinkSpawnWhenNear, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, AiThinkFoulCrowNestInit, AiThinkSuckerNest, NULL },
	{ 0, NULL, AiThinkHireling, AiThinkResetThinkOut },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, AiThinkStandUntilExpiredInit, AiThinkStandUntilExpired, NULL },
	{ 1, NULL, AiThinkCouncilMember, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 2, NULL, AiThinkHydra, NULL },
	{ 2, NULL, AiThinkHydra, NULL },
	{ 2, NULL, AiThinkHydra, NULL },
	{ 1, NULL, AiThinkPillar, NULL },
	{ 1, NULL, AiThinkAttackOnceThenDie, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkFollowOwner, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkHireling, AiThinkResetThinkOut },
	{ 1, NULL, AiThinkBalrog, NULL },
	{ 1, NULL, AiThinkSkeleton, NULL },
	{ 1, NULL, AiThinkReturnedMage, NULL },
	{ 1, NULL, AiThinkGriswold, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkTyrael, NULL },
	{ 1, NULL, AiThinkYoungdiablo, NULL },
	{ 1, NULL, AiThinkTrapNova, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 1, NULL, AiThinkLightningSpire, NULL },
	{ 1, NULL, AiThinkFireTower, NULL },
	{ 1, NULL, AiThinkSlinger, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 1, NULL, AiThinkReturnedMage, NULL },
	{ 1, NULL, AiThinkReturnedMage, NULL },
	{ 1, NULL, AiThinkReturnedMage, NULL },
	{ 1, NULL, AiThinkSkeleton, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 1, NULL, AiThinkSlinger, NULL },
	{ 2, NULL, AiThinkRatManBackOffInSight, NULL },
	{ 1, NULL, AiThinkAttackOnce, NULL },
	{ 1, NULL, AiThinkTheSmith, NULL },
	{ 1, NULL, AiThinkTrappedSoul, NULL },
	{ 1, NULL, AiThinkTrappedSoul, NULL },
	{ 0, NULL, AiThinkNpc, NULL },
	{ 0, NULL, AiThinkTyrael, NULL },
	{ 1, NULL, AiThinkRatManHitAndBackOff, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 1, NULL, AiThinkTheSmith, NULL },
	{ 0, NULL, NULL, NULL },
	{ 0, NULL, AiThinkWait, NULL },
	{ 1, NULL, AiThinkCircleTarget, NULL },
	{ 1, AiThinkPatrolInit, AiThinkPatrol, NULL },
	{ 0, NULL, AiThinkHireling, AiThinkResetThinkOut },
	{ 0, NULL, AiThinkRogueScout, NULL },
	{ 0, NULL, AiThinkFightOutOfTown, NULL },
	{ 0, NULL, AiThinkFollowOwner, NULL },
	{ 1, NULL, AiThinkAttackInSight, NULL },
	{ 1, NULL, AiThinkGroupMember, NULL },
	{ 1, NULL, AiThinkAttackOrStepAside, NULL },
	{ 1, AiThinkFleeInit, AiThinkFlee, NULL },
	{ 0, NULL, AiThinkChargeUnit, NULL },
	{ 1, NULL, AiThinkGuardPlace, NULL },
};

#define sgptAiThinkSpecials (&sgatAiThinks[AITHINK_ROWS])

void __fastcall AiThinkSet(GAME *ptGame, UNIT *hUnit, AIGENERAL *ptAi, int nKind)
{
	const AITHINKROW *ptAiThink;
	const AITHINKROW *ptCurrent;
	MONSTATSTXT *ptMonStats;
	DWORD dwClass;

	PFNAITHINK pfnFunction;
	int i;

#line 13731
	D2WARN(! StatsGetState (hUnit, UNIT_STATE_UNINTERRUPTABLE));
	if (!ptGame || !hUnit || !ptAi || nKind >= AITHINK_SPECIALS || hUnit->eType != UNIT_MONSTER)
		return;
	dwClass = hUnit->dwClassId;
	if ((int)dwClass >= 410)
		nKind = 1;
	if (ptAi->pfnAi)
	{
		int nThink = ptAi->nThinkKind;
		ptMonStats = MONDATA(hUnit)->ptMonStats;
		if (nThink && AiThinkKindApplies(hUnit, ptMonStats, nThink, FALSE))
			ptCurrent = &sgptAiThinkSpecials[nThink];
		else
			ptCurrent = &sgatAiThinks[ptMonStats->nBaseId];
		if (ptAi->pfnAi == ptCurrent->pfnThink && ptCurrent->pfnThinkOut)
		{
			ptAi->nThinkKind = nKind;
			ptAi->pfnAi = ptCurrent->pfnThinkOut;
			return;
		}
	}
	memset(ptAi->adwParam, 0, sizeof(ptAi->adwParam));
	AiClearCommands(ptGame, hUnit);
	ptMonStats = MONDATA(hUnit)->ptMonStats;
	if (nKind && AiThinkKindApplies(hUnit, ptMonStats, nKind, FALSE))
		ptAiThink = &sgptAiThinkSpecials[nKind];
	else
		ptAiThink = &sgatAiThinks[ptMonStats->nBaseId];
	if (!ptAiThink)
		return;
	if (ptAiThink->pfnThinkInit)
	{ AIPARMS tAiParms;
		for (i = 0; i < (int)(sizeof(tAiParms) / sizeof(DWORD)); i++)
			((DWORD *)&tAiParms)[i] = 0;
		tAiParms.ptAi = ptAi;
		tAiParms.ptMonStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(dwClass);
#line 13786
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)ptAiThink->pfnThinkInit), "ptAiThink->pfnThinkInit");
		ptAiThink->pfnThinkInit(ptGame, hUnit, &tAiParms);
	}
	pfnFunction = ptAiThink->pfnThink;
	if (!pfnFunction)
	{
		if (!nKind)
			pfnFunction = AiThinkDefault;
		else
			pfnFunction = AiThinkWait;
	}
	ptAi->nThinkKind = nKind;
	ptAi->pfnAi = pfnFunction;
#line 13803
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)pfnFunction), "pfnFunction");
}

void __fastcall AiThinkDefault(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	UNIT *hTarget;
	D2SEED *ptSeed;

	hTarget = ptParms->hTarget;
	ptSeed = &hUnit->tSeed;
	if (ptParms->fTargetInReach)
	{
		if ((D2CommonSeedNext(ptSeed) & 255) < 128)
		{
			AiSetModeOnTarget(ptGame, hUnit, MONMODE_ATTACK1, hTarget);
			return;
		}
	}
	else if ((D2CommonSeedNext(ptSeed) & 255) < 153)
	{
		AiWalkToTarget(ptGame, hUnit, hTarget, 0);
		return;
	}
	AiStand(ptGame, hUnit, 20);
}

static __inline DWORD AiThinkUnitClass(UNIT *hUnit)
{
	return hUnit->dwClassId;
}

static __inline DWORD AiThinkUnitId(UNIT *hUnit)
{
	return hUnit->dwId;
}

static void __fastcall AiThinkReact(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	MONSTATSTXT *ptMonStats;
	AIGENERAL *ptAi;
	DWORD dwUnique;
	int nX;
	int nY;
	BOOL bMelee;
	int nClass;
	GAMEROOM *hRoom;
	void *ptArea;
	int nSpawnX;
	int nSpawnY;
	DWORD dwRoom;
	int nWalk;
	MISSLEVELDATA *ptLevel;
	UNIT *hTarget;
	AIFINDRECORD tFind;
	int nLife;

	if (!ptParms->hTarget)
		return;
	ptMonStats = ptParms->ptMonStats;
	ptAi = ptParms->ptAi;
	dwUnique = MonsterUniqueIsCounted(MONDATA(hUnit)->ptUnique);
	if ((hUnit->dwClassId == 250 || dwUnique) && (DWORD)ptParms->nDist < 20
		&& ptParms->hTarget->eType == UNIT_PLAYER && !AiRecordTestFlags((AIRECORDHEAD *)ptAi, 16))
	{
		SUnitSetEvent(hUnit, 15, NULL);
		ptParms->wFlags |= 4;
		AiRecordSetFlags((AIRECORDHEAD *)ptAi, 16, TRUE);
		AiStand(ptGame, hUnit, 20);
		return;
	}
	if (AiRecordTestFlags((AIRECORDHEAD *)ptAi, AIFLAG_20))
	{
#line 14016
		D2ASSERT(UnitIsDead (hUnit) == FALSE);
		if (D2CommonSeedRand(&hUnit->tSeed, 100) < 40)
		{
			nX = (int)D2CommonUnitGetX(hUnit);
			nY = (int)D2CommonUnitGetY(hUnit);
			D2CommonUnitGetX(ptParms->hTarget);
			D2CommonUnitGetY(ptParms->hTarget);
			bMelee = ptMonStats->bIsMelee;
			nClass = (int)D2CommonMonsterUnitGetTypeLocalIndex(hUnit);
			if (nClass == 10 || nClass == 345)
				bMelee = TRUE;
			nLife = D2CommonUnitCalcLifePercent(hUnit);
			if (nLife < 30 || (!bMelee && (DWORD)ptParms->nDist < 5))
			{
				if (nLife < 30)
				{
					if (D2CommonSeedRand(&hUnit->tSeed, 100) < 30)
						D2CommonStatsUnitAddValue(hUnit, 6,
							D2CommonStatsUnitCalcMaxHp(hUnit) / 6);
				}
			hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
			ptArea = (void *)D2CommonDungeonRoomGetLogicNode(hRoom, nX, nY);
			if (MonsterFindSpawnPoint(ptGame, hRoom, (DRLGLOGICNODE *)ptArea, nClass, &nSpawnX, &nSpawnY,
				FALSE))
			{
				dwRoom = (DWORD)D2CommonDungeonFindRoomGame(hRoom, nSpawnX, nSpawnY);
				if (dwRoom && !D2CommonDungeonRoomIsTown(dwRoom))
				{
					AiUseSkillInMode(ptGame, hUnit, MONMODE_ATTACK1, SKILL_MONTELEPORT, NULL, nSpawnX,
						nSpawnY);
					ptParms->wFlags |= 4;
					return;
				}
			}
			}
		}
	}
walk:
	dwRoom = D2CommonUnitGetPathRoom(hUnit);
	if (!dwRoom || !ptMonStats->bIsMelee || !ptMonStats->bModeW)
		return;
	ptLevel = (MISSLEVELDATA *)D2CommonLvlTblsGetLevelData(D2CommonDungeonRoomGetLevelId(dwRoom));
	nWalk = ptLevel->MonSpcWalk;
	if (nWalk <= 0 || (DWORD)ptParms->nDist <= (DWORD)nWalk)
		return;
	hTarget = ptParms->hTarget;
	if (AiThinkSees(hUnit, hTarget))
		return;
	tFind.tExcl.hUnit = NULL;
	tFind.tExcl.nDist = 2147483647;
	tFind.tExcl.hExclude = hTarget;
	AiFind(ptGame, hUnit, &tFind, NULL, 11);
	if (!tFind.tExcl.hUnit)
	{
		ptParms->wFlags |= 4;
		AiStepAside(ptGame, hUnit, 4);
		return;
	}
	ptParms->hTarget = tFind.tExcl.hUnit;
	ptParms->nDist = tFind.tExcl.nDist;
}

static void __fastcall AiThinkPreempt(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	MONSTATSTXT *ptMonStats;
	AIFINDRECORD tFind;
	AIGENERAL *ptAi;
	UNIT *hRef;
	int nDist;

	if (StatsGetState(hUnit, 21))
	{
		ptParms->wFlags |= 7;
		AiStand(ptGame, hUnit, 3);
		return;
	}
	ptMonStats = MONDATA(hUnit)->ptMonStats;
	ptParms->ptMonStats = ptMonStats;
	if (ptMonStats->bOpenDoors && (D2CommonPathGetLineOfSightRaw((DWORD)hUnit->ptPath) & 2048))
	{
		tFind.tUnit.hUnit = NULL;
		tFind.tUnit.nDist = 2147483647;
		AiFind(ptGame, hUnit, &tFind, NULL, 8);
		if (tFind.tUnit.hUnit && tFind.tUnit.nDist < 3
			&& ((MISSOBJECTSTXT *)D2CommonObjTblsGetObject(AiThinkUnitClass(tFind.tUnit.hUnit)))->MonsterOK)
		{
			ObjectModeOperateInRange(ptGame, hUnit, 2, AiThinkUnitId(tFind.tUnit.hUnit));
			ptParms->wFlags |= 7;
			AiStand(ptGame, hUnit, 5);
			return;
		}
	}
	ptAi = ptParms->ptAi;
	hRef = SUnitFind(ptGame, ptAi->dwRefUnitType, ptAi->dwRefUnitId);
	if (hRef && ptMonStats->bModeW)
	{
		if (SUnitIsDead(hRef))
		{
			ptAi->dwRefUnitId = (DWORD)-1;
			ptAi->dwRefUnitType = 1;
		}
		else
		{
			nDist = AiThinkDistance(hUnit, hRef);
			if (nDist <= 1 && ptAi->dwRefUnitType == UNIT_PLAYER)
			{
				ptParms->wFlags |= 7;
				if (!AiWalkAway(ptGame, hUnit, hRef, 19, TRUE))
					AiWalkAround(ptGame, hUnit, hRef, 19);
			}
			else if (nDist > 20)
			{
				ptParms->wFlags |= 7;
				AiSetVelocity(hUnit, 7, 0, 40);
				AiWalkAround(ptGame, hUnit, hRef, 19);
			}
		}
	}
}

extern const AITHINKROW *__fastcall AiThinkRowFor(UNIT *hUnit, int nKind);
__inline const AITHINKROW *__fastcall AiThinkRowFor(UNIT *hUnit, int nKind)
{
	MONSTATSTXT *ptMonStats;
	const AITHINKROW *ptAiThink;

	ptMonStats = MONDATA(hUnit)->ptMonStats;
	if (AiThinkKindApplies(hUnit, ptMonStats, nKind, FALSE) && nKind)
		ptAiThink = &sgptAiThinkSpecials[nKind];
	else
		ptAiThink = &sgatAiThinks[ptMonStats->nBaseId];
	return ptAiThink;
}

static void __fastcall AiThinkLookForTarget(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms)
{
	AIGENERAL *ptAi;
	const AITHINKROW *ptAiThink;

	ptAi = ptParms->ptAi;
	ptAiThink = AiThinkRowFor(hUnit, ptAi->nThinkKind);
	switch ((DWORD)ptAiThink->nIdle)
	{
	case 0:
		break;
	case 1:
		ptParms->hTarget = AiIdleByParam(ptGame, hUnit, ptParms->ptAi, &ptParms->nDist,
			(DWORD *)&ptParms->fTargetInReach);
		if (!ptParms->hTarget)
			ptParms->wFlags |= 6;
		break;
	case 2:
		ptParms->hTarget = AiFindTarget(ptGame, hUnit, ptParms->ptAi, &ptParms->nDist,
			(DWORD *)&ptParms->fTargetInReach);
		break;
	case 4:
		ptParms->hTarget = AiIdle(ptGame, hUnit, ptParms->ptAi, &ptParms->nDist,
			(DWORD *)&ptParms->fTargetInReach);
		if (!ptParms->hTarget)
			ptParms->wFlags |= 6;
		break;
	case 5:
		ptParms->hTarget = AiIdle(ptGame, hUnit, ptParms->ptAi, &ptParms->nDist,
			(DWORD *)&ptParms->fTargetInReach);
		break;
	default:
		ptParms->wFlags |= 6;
		break;
	}
}

void __fastcall AiThinkProcess(GAME *ptGame, UNIT *hUnit)
{
	AIPARMS tAiParms;
	int i;

#line 14148
	D2ASSERT(ptGame);
#line 14149
	D2ASSERT(hUnit);
	for (i = 0; i < (int)(sizeof(tAiParms) / sizeof(DWORD)); i++)
		((DWORD *)&tAiParms)[i] = 0;
	tAiParms.ptAi = MONDATA(hUnit)->ptAi;
	AiThinkPreempt(ptGame, hUnit, &tAiParms);
	if (!(tAiParms.wFlags & 1))
		AiThinkLookForTarget(ptGame, hUnit, &tAiParms);
	if (!(tAiParms.wFlags & 2))
		AiThinkReact(ptGame, hUnit, &tAiParms);
	if (!(tAiParms.wFlags & 4))
	{
#line 14190
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)tAiParms.ptAi->pfnAi), "tAiParms.ptAi->pfnAi");
		tAiParms.ptAi->pfnAi(ptGame, hUnit, &tAiParms);
	}
}
