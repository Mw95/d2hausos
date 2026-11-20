#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\SKILLS\\SkillMonst.cpp"

#include "../D2game.h"
#include "D2Macros.h"
#include "../AI/AiGeneral.h"
#include "../GAME/Event.h"
#include "../MISSILES/Missiles.h"
#include "../MONSTER/MonsterEval.h"
#include "../MONSTER/MonsterMode.h"
#include "../MONSTER/MonsterMsg.h"
#include "../MONSTER/MonsterUnique.h"
#include "../OBJECTS/Objects.h"
#include "../QUESTS/QUESTS.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "SkilPal.h"
#include "SkillAma.h"
#include "SkillBar.h"
#include "SkillMonst.h"
#include "SkillNec.h"
#include "Skills.h"
#include "SkillCurse.h"
#include "SkillSor.h"

#define SKILL_MONSTER_START		156
#define NUM_MONSTER_SKILLS		61
#define MONSTER_PRISONFL		340
#define NUM_MONSTER_CURSES		6

typedef void (__fastcall *PFNSKILLMONSTCURSE)(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);

BOOL __fastcall SkillsShamanFire(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nRunCount;
	int nRunIndex;

	hUnit->dwFlags0 |= 64;
	D2CommonMonsterTblsGetRun(hUnit->dwClassId, &nRunCount, &nRunIndex);
	SkillsCreateMissile(ptGame, nRunIndex + MISSILE_SHAFIRE1, hUnit, nLevel, 0, 0, 0, 0, 0);
	return TRUE;
}

BOOL __fastcall SkillsUnHolyBolt(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nRunCount;
	int nRunIndex;

	hUnit->dwFlags0 |= 64;
	D2CommonMonsterTblsGetRun(hUnit->dwClassId, &nRunCount, &nRunIndex);
	SkillsCreateMissile(ptGame, nRunIndex + MISSILE_UNHOLYBOLT1, hUnit, nLevel, 0, 0, 0, 0, 0);
	return TRUE;
}

BOOL __fastcall SkillsFireHitStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	int nDamage;
	int i;
	DWORD hSkill;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	D2CommonSkillsSetNodeFlags(hSkill, 8);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	if (hUnit->eType == UNIT_PLAYER)
		tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget, 100);
	else
		tDamage.wFlags = MonsterEvalHit(ptGame, hUnit, hTarget, (DWORD)(nLevel * 5 + 15), 0);
	if (tDamage.wFlags & 1)
	{
		nDamage = (int)((D2CommonSeedRand(&hUnit->tSeed, 3) + nLevel) << 8);
		tDamage.nFireDamage = nDamage;
		if (hUnit->dwClassId == 30 || hUnit->dwClassId == 32)
		{
			tDamage.nColdDamage = nDamage;
			tDamage.nColdLength = D2CommonSkillsCalcParamThreeByLevel(SKILL_MONSTER_START, nLevel);
			tDamage.nFireDamage = 0;
		}
	}
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	return TRUE;
}

BOOL __fastcall SkillsFireHit(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget)
	{
		SUnitDmgDeliver(ptGame, hUnit, hTarget);
		return TRUE;
	}
	return FALSE;
}

BOOL __fastcall SkillsMaggotEggStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nRunIndex;
	int nRunCount;
	DWORD hSkill;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	D2CommonMonsterTblsGetRun(hUnit->dwClassId, &nRunCount, &nRunIndex);
	D2CommonSkillsSetNodeScratchD(hSkill, 0);
	D2CommonSkillsSetNodeScratchE(hSkill, nLevel);
	D2CommonSkillsSetNodeScratchF(hSkill, nRunIndex + 180);
	hUnit->dwFlags0 &= ~14;
	D2CommonPathReplaceCollisionMask(hUnit, TRUE);
	return TRUE;
}

BOOL __fastcall SkillsMaggotEgg(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DWORD dwFrame;
	DWORD dwCount;
	DWORD hSkill;
	int nSpawned;
	int nRadius;
	int nTries;

	D2CommonUnitGetAnimFrameAndCount(hUnit, &dwFrame, &dwCount);
	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	nSpawned = 0;
	hUnit->nSeqRate = 0;
	if (MonsterSpawnNear(ptGame, hUnit, (int)D2CommonSkillsGetNodeScratchF(hSkill), 8, -1, 0))
		nSpawned = 1;
	nRadius = 1;
	for (nTries = 0; nSpawned < nLevel && nRadius < 5 && nSpawned < 12 && nTries < 24; nTries++)
	{
		if (MonsterSpawnNear(ptGame, hUnit, (int)D2CommonSkillsGetNodeScratchF(hSkill), 8, nRadius, 0))
			nSpawned++;
		else
			nRadius++;
	}
	SUnitDmgKill(ptGame, hUnit, SUnitGetTarget(ptGame, hUnit), TRUE);
	return TRUE;
}

BOOL __fastcall SkillsMagottUpStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	GAMEROOM *hRoom;
	int nX;
	int nY;
	BOOL fRet;

	hUnit->dwFlags0 |= 14;

	fRet = SUnitMoveToRoom(ptGame, hUnit, ((GAMEROOM *)D2CommonUnitGetPathRoom(hUnit)), ((int)D2CommonUnitGetX(hUnit)), ((int)D2CommonUnitGetY(hUnit)), FALSE, FALSE);
#line 192
	D2WARN(fRet);

	D2CommonCollisnSetPattern(((GAMEROOM *)D2CommonUnitGetPathRoom(hUnit)), ((int)D2CommonUnitGetX(hUnit)), ((int)D2CommonUnitGetY(hUnit)), 1, 4096);
	return TRUE;
}

BOOL __fastcall SkillsMagottDownStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	GAMEROOM *hRoom;
	int nX;
	int nY;

	hUnit->dwFlags0 &= ~14;

	D2CommonCollisnClearPattern(((GAMEROOM *)D2CommonUnitGetPathRoom(hUnit)), ((int)D2CommonUnitGetX(hUnit)), ((int)D2CommonUnitGetY(hUnit)), 3, 4096);
	return TRUE;
}

BOOL __fastcall SkillsMagottDown(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DWORD dwFrame;
	DWORD dwCount;
	int nLife;
	int nMaxLife;

	D2CommonUnitGetAnimFrameAndCount(hUnit, &dwFrame, &dwCount);
	if ((int)dwFrame <= 0)
		hUnit->nSeqRate = 0;
	nLife = D2CommonStatsUnitGetValue(hUnit, 6);
	nMaxLife = D2CommonStatsUnitCalcMaxHp(hUnit);
	if (nLife < nMaxLife * 3 / 4)
		D2CommonStatsUnitAddValue(hUnit, 6, nMaxLife / 10);
	hUnit->dwFlags0 |= 64;
	return TRUE;
}

BOOL __fastcall SkillsMagottLay(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	BYTE abSteps[8];
	GAMEROOM *hRoom;
	UNIT *hTarget;
	int nOffsetX;
	int nOffsetY;
	int nRunIndex;
	int nRunCount;
	int nX;
	int nY;
	BYTE bDir;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	bDir = (BYTE)D2CommonUnitSetPositionExtra(hUnit, (int)D2CommonUnitGetX(hTarget), (int)D2CommonUnitGetY(hTarget));
	abSteps[0] = 10;
	abSteps[1] = 8;
	abSteps[2] = 22;
	abSteps[3] = 20;
	abSteps[4] = 18;
	abSteps[5] = 16;
	abSteps[6] = 14;
	abSteps[7] = 12;
	bDir = abSteps[D2CommonMonsterRegionLookupTableB(bDir)];
	D2CommonMonsterRegionGetOffsetPair(bDir, &nOffsetX, &nOffsetY);
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	nX = (int)D2CommonUnitGetX(hUnit) + nOffsetX;
	nY = (int)D2CommonUnitGetY(hUnit) + nOffsetY;
	D2CommonMonsterTblsGetRun(hUnit->dwClassId, &nRunCount, &nRunIndex);
	MonsterSpawn(ptGame, hRoom, nX, nY, nRunIndex + MONSTER_SANDMAGGOTEGG, 8, -1, 0);
	hUnit->bAnimEvent = 0;
	return TRUE;
}

BOOL __fastcall SkillsAndrialSprayStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;
	DWORD hSkill;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	D2CommonSkillsSetNodeScratchC(hSkill, D2CommonUnitGetX(hTarget));
	D2CommonSkillsSetNodeScratchD(hSkill, D2CommonUnitGetY(hTarget));
	return TRUE;
}

BOOL __fastcall SkillsAndrialSpray(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreate;
	DWORD hSkill;
	DWORD dwSpotX;
	DWORD dwSpotY;
	DWORD dwFrame;
	DWORD dwCount;
	int nOffsetX;
	int nOffsetY;
	int nX;
	int nY;
	int i;
	BYTE bDir;
	BYTE bStep;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	dwSpotX = D2CommonSkillsGetNodeScratchC(hSkill);
	dwSpotY = D2CommonSkillsGetNodeScratchD(hSkill);
	BYTE aabSpray[8][9] = {
		{ 27, 14, 15, 3, 99, 7, 21, 22, 31 },
		{ 26, 12, 13, 2, 99, 6, 19, 20, 30 },
		{ 25, 10, 11, 1, 99, 5, 17, 18, 29 },
		{ 24, 8, 9, 0, 99, 4, 15, 16, 28 },
		{ 31, 22, 23, 7, 99, 3, 13, 14, 27 },
		{ 30, 20, 7, 6, 99, 2, 1, 12, 26 },
		{ 29, 18, 19, 5, 99, 1, 9, 10, 25 },
		{ 28, 16, 17, 4, 99, 0, 23, 8, 24 } };
	bDir = (BYTE)D2CommonUnitSetPositionExtra(hUnit, dwSpotX, dwSpotY);
	bDir = D2CommonMonsterRegionLookupTableB(bDir);
	BYTE abSteps[8] = { 29, 28, 27, 26, 25, 24, 31, 30 };
	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	D2CommonMonsterRegionGetOffsetPair(abSteps[bDir], &nOffsetX, &nOffsetY);
	nX += nOffsetX;
	nY += nOffsetY;
	D2CommonUnitGetAnimFrameAndCount(hUnit, &dwFrame, &dwCount);
	bStep = (&aabSpray[bDir][0])[dwFrame - 4];
	if (bStep != 99)
	{
		D2CommonMonsterRegionGetOffsetPair(bStep, &nOffsetX, &nOffsetY);
		nX += nOffsetX;
		nY += nOffsetY;
	}
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = 32;
	tCreate.nDX = nX;
	tCreate.nDY = nY;
	tCreate.hSource = hUnit;
	tCreate.hSpawnLocation = hUnit;
	tCreate.nLevel = 1;
	tCreate.eMissile = MISSILE_ANDRIAL;
	MissilesCreate(ptGame, &tCreate);
	return TRUE;
}

BOOL __fastcall SkillsAndyPoisonBolt(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	hUnit->dwFlags0 |= 64;
	SkillsCreateMissile(ptGame, MISSILE_ANDYPOISONBOLT, hUnit, nLevel, 0, 0, 0, 0, 0);
	return TRUE;
}

BOOL __fastcall SkillsPoisonBallTrap(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	hUnit->dwFlags0 |= 64;
	SkillsCreateMissile(ptGame, MISSILE_TRAP_POISON_BALL_LEFT, hUnit, nLevel, 0, 0, 0, 0, 0);
	return TRUE;
}

BOOL __fastcall SkillsCursedBallTrapRight(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	hUnit->dwFlags0 |= 64;
	SkillsCreateMissile(ptGame, MISSILE_TRAP_CURSED_SKULL_RIGHT, hUnit, nLevel, 0, 0, 0, 0, 0);
	return TRUE;
}

BOOL __fastcall SkillsCursedBallTrapLeft(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	hUnit->dwFlags0 |= 64;
	SkillsCreateMissile(ptGame, MISSILE_TRAP_CURSED_SKULL_LEFT, hUnit, nLevel, 0, 0, 0, 0, 0);
	return TRUE;
}

BOOL __fastcall SkillsUseLevelAsMissile(GAME *ptGame, UNIT *hUnit, int nMissile)
{
	UNIT *hOwner;
	int nLevel;

	nLevel = 1;
	hUnit->dwFlags0 |= 64;
	switch (nMissile) {
	case 208: case 209: case 210: case 211:
		hOwner = AiGetOwner(hUnit);
		if (hOwner)
			nLevel = (int)D2CommonSkillsCalcNodeLevel(hOwner,
				D2CommonSkillsFindByKey(hOwner, SKILL_SKELETON_MASTERY), 1) + 1;
		break;
	case 247:
		hOwner = AiGetOwner(hUnit);
		if (hOwner)
			nLevel = (int)D2CommonSkillsCalcNodeLevel(hOwner,
				D2CommonSkillsFindByKey(hOwner, SKILL_HYDRA), nLevel);
		break;
	default:
		nLevel = D2CommonStatsUnitGetValue(hUnit, 12); }

	SkillsCreateMissile(ptGame, nMissile, hUnit, nLevel, 0, 0, 0, 0, 0);
	return TRUE;
}

BOOL __fastcall SkillsJumpStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	GAMEROOM *hRoom;
	UNIT *hTarget;
	DWORD hSkill;
	int nX;
	int nY;
	int i;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
#line 405
	D2ASSERT(hSkill);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
	{
		nX = (int)D2CommonPathGetTargetXRaw((DWORD)hUnit->ptPath);
		nY = (int)D2CommonPathGetTargetYRaw((DWORD)hUnit->ptPath);
		D2CommonSkillsSetNodeScratchF(hSkill, -1);
	}
	else
	{
		nX = (int)D2CommonUnitGetX(hTarget);
		nY = (int)D2CommonUnitGetY(hTarget);
		nX = nX * 2 - (int)D2CommonUnitGetX(hUnit);
		nY = nY * 2 - (int)D2CommonUnitGetY(hUnit);
		D2CommonSkillsSetNodeFlags(hSkill, 8);
		for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
			((DWORD *)&tDamage)[i] = 0;
		tDamage.wFlags = MonsterEvalHit(ptGame, hUnit, hTarget, (DWORD)(nLevel * 5 + 15), 0);
		if (tDamage.wFlags & 1)
			tDamage.nPhysicalDamage = (int)((D2CommonSeedRand(&hUnit->tSeed, 3) + nLevel) << 8);
		SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
		D2CommonSkillsSetNodeScratchE(hSkill, hTarget->eType);
		D2CommonSkillsSetNodeScratchF(hSkill, hTarget->dwId);
	}
	hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hUnit), nX, nY);
	D2CommonCollisnSetPattern(hRoom, nX, nY, D2CommonPathGetCollisionPatternByUnit(hUnit), 256);
	SUnitSetStateEndLinked(hUnit, TRUE);
	D2CommonSkillsSetNodeScratchC(hSkill, nX);
	D2CommonSkillsSetNodeScratchD(hSkill, nY);
	D2CommonSkillsSetNodeFlags(hSkill, 128);
	return TRUE;
}

BOOL __fastcall SkillsJump(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MONSTATSTXT *ptMonStats;
	UNIT *hTarget;
	void *ptPath;
	DWORD hSkill;
	DWORD dwSkillFlags;
	DWORD dwId;
	int nPos;
	int nClass;
	int nX;
	int nY;
	int nPathX;

	hSkill = D2CommonSkillsFindByKey(hUnit, SKILL_JUMP);
	dwSkillFlags = D2CommonSkillsGetNodeFlags(hSkill);
#line 476
	D2ASSERT(hSkill);
	EventRemoveByType(ptGame, hUnit, 1);
	if (hUnit->bAnimEvent == 2)
	{
		hUnit->bAnimEvent = 0;
		dwId = D2CommonSkillsGetNodeScratchF(hSkill);
		hTarget = SUnitFind(ptGame, D2CommonSkillsGetNodeScratchE(hSkill), dwId);
		if (hTarget)
			SUnitDmgDeliver(ptGame, hUnit, hTarget);
		return TRUE;
	}
	nClass = (int)D2CommonMonsterUnitGetTypeLocalIndex(hUnit);
	nX = (int)D2CommonSkillsGetNodeScratchC(hSkill);
	nY = (int)D2CommonSkillsGetNodeScratchD(hSkill);
	if (dwSkillFlags & 256)
	{
		if ((int)D2CommonUnitGetX(hUnit) != nX || (int)D2CommonUnitGetY(hUnit) != nY)
		{
			nPos = 0;
			if (nClass == MONSTER_SANDLEAPER)
				nPos = 8;
			D2CommonUnitSetAnimSequencePos(hUnit, nPos);
			hUnit->nAnimLength = (hUnit->nAnimLength & ~255) + 256;
			return TRUE;
		}
		SUnitSetStateEndLinked(hUnit, FALSE);
		ptPath = hUnit->ptPath;
		D2CommonCollisnClearPattern(D2CommonUnitGetPathRoom(hUnit), nX, nY, D2CommonPathGetCollisionPatternByUnit(hUnit), 256);
		D2CommonPathSetCollisionMask(ptPath, 256);
		D2CommonPathSetCollisionTestMask(ptPath, 7169);
		D2CommonPathSetType(ptPath, 101);
		D2CommonSkillsSetNodeFlags(hSkill, 0);
		if (nClass != MONSTER_SANDLEAPER)
			return TRUE;
		dwId = D2CommonSkillsGetNodeScratchF(hSkill);
		hTarget = SUnitFind(ptGame, D2CommonSkillsGetNodeScratchE(hSkill), dwId);
		if (!hTarget)
			return TRUE;
		D2CommonSkillsSetNodeFlags(hSkill, 1);
		D2CommonUnitSetAnimSequencePos(hUnit, 12);
		hUnit->nAnimLength = (hUnit->nAnimLength & ~255) + 256;
		D2CommonStepSnapToSquare(ptPath);
		D2CommonPathSetDistMax(ptPath, 5);
		D2CommonPathSetStopDistance(ptPath, 1);
		D2CommonPathSetTarget(ptPath, 0);
		nPathX = nX * 3 - (int)(D2CommonUnitGetX(hTarget) << 1);
		nY = nY * 3 - (int)(D2CommonUnitGetY(hTarget) * 2);
		D2CommonPathSetTargetCoord(ptPath, nPathX, nY);
		D2CommonPathSetType(ptPath, 8);
		D2CommonPathSetSquareCountdown(ptPath, 5);
		D2CommonPathCompute(ptPath, hUnit);
		return TRUE;
	}
	if (dwSkillFlags & 128)
	{
		D2CommonSkillsSetNodeFlags(hSkill, 257);
		ptPath = hUnit->ptPath;
		D2CommonPathSetCollisionTestMask(ptPath, 0);
		D2CommonPathSetCollisionMask(ptPath, 0);
		D2CommonPathSetTargetCoord(ptPath, nX, nY);
		D2CommonPathSetType(ptPath, 9);
		ptMonStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId);
		D2CommonPathSetVelocity(ptPath, ptMonStats->Run << 8);
		D2CommonPathCompute(ptPath, hUnit);
		return TRUE;
	}
	D2CommonSkillsSetNodeFlags(hSkill, 0);
	return TRUE;
}

BOOL __fastcall SkillsSwarmMoveStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	void *ptPath;
	DWORD hSkill;

	if (!SUnitGetTarget(ptGame, hUnit))
		return FALSE;
	ptPath = hUnit->ptPath;
	D2CommonPathSetDistMax(ptPath, 5);
	D2CommonPathSetType(ptPath, 2);
	D2CommonPathCompute(ptPath, hUnit);
	if (!D2CommonPathGetStepCount(ptPath))
	{
		D2CommonPathSetType(ptPath, 1);
		D2CommonPathCompute(ptPath, hUnit);
		if (!D2CommonPathGetStepCount(ptPath))
			return FALSE;
	}
	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	D2CommonSkillsSetNodeFlags(hSkill, 1);
	return TRUE;
}

BOOL __fastcall SkillsSwarmMove(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DWORD hSkill;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	if (D2CommonSkillsGetNodeFlags(hSkill) & 2)
	{
		D2CommonSkillsSetNodeFlags(hSkill, 0);
		D2CommonUnitSetAnimSequencePos(hUnit, 19);
		return TRUE;
	}
	D2CommonUnitSetAnimSequencePos(hUnit, 11);
	return TRUE;
}

BOOL __fastcall SkillsNestStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DWORD hSkill;
	int nClass;
	int nX;
	int nY;
	int nMode;
	void *pRoom;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	D2CommonMonsterGetSpecialPosition(hUnit, &nClass, &nX, &nY, &nMode, 0);
	D2CommonSkillsSetNodeScratchC(hSkill, nClass);
	D2CommonSkillsSetNodeScratchD(hSkill, nX);
	D2CommonSkillsSetNodeScratchE(hSkill, nY);
	D2CommonSkillsSetNodeScratchF(hSkill, nMode);
	pRoom = D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hUnit), nX, nY);
	D2CommonCollisnSetPattern(pRoom, nX, nY, 1, 256);
	SUnitSetStateEndLinked(hUnit, TRUE);
	EventRemoveByType(ptGame, hUnit, 1);
	return TRUE;
}

BOOL __fastcall SkillsNest(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	GAMEROOM *hRoom;
	UNIT *hMonster;
	void *ptCollRoom;
	DWORD hSkill;
	int nClass;
	int nX;
	int nY;
	int nMode;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	EventRemoveByType(ptGame, hUnit, 1);
	nClass = (int)D2CommonSkillsGetNodeScratchC(hSkill);
	nX = (int)D2CommonSkillsGetNodeScratchD(hSkill);
	nY = (int)D2CommonSkillsGetNodeScratchE(hSkill);
	nMode = (int)D2CommonSkillsGetNodeScratchF(hSkill);
	hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hUnit), nX, nY);
	SUnitSetStateEndLinked(hUnit, FALSE);
	if (hRoom)
	{
		ptCollRoom = D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hUnit), nX, nY);
		D2CommonCollisnClearPattern(ptCollRoom, nX, nY, 1, 256);
		hMonster = MonsterSpawn(ptGame, hRoom, nX, nY, nClass, (DWORD)nMode, -1, 0);
		if (hMonster)
			hMonster->dwFlags0 |= 67239936;
	}
	return TRUE;
}

BOOL __fastcall SkillsQuickStrikeStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MonsterEvalAttack(hUnit, ptGame);
	return TRUE;
}

BOOL __fastcall SkillsQuickStrike(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget)
		SUnitDmgDeliver(ptGame, hUnit, hTarget);
	MonsterModeUseSkill(ptGame, hUnit, 1);
	return TRUE;
}

BOOL __fastcall SkillsVampireFireball(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	hUnit->dwFlags0 |= 64;
	SkillsCreateMissile(ptGame, MISSILE_VAMPIREFIREBALL, hUnit, nLevel, 0, 0, 0, 0, 0);
	return TRUE;
}

BOOL __fastcall SkillsVampireFirewall(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreate;
	DWORD hRoom;
	int nX;
	int nY;
	int nDX;
	int nDY;
	int i;

	if (!SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY))
		return FALSE;
	hRoom = D2CommonUnitGetPathRoom(hUnit);
	if (!(D2CommonDungeonRoomIsTown(D2CommonDungeonFindRoomGame(hRoom, nX, nY)))) {
		for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.dwFlags = 33;
		tCreate.hSource = hUnit;
		tCreate.nX = nX;
		tCreate.nY = nY;
		tCreate.nLevel = nLevel;
		tCreate.eMissile = MISSILE_VAMPIREFIREWALLMAKER;
		nDX = (int)D2CommonUnitGetX(hUnit) - nX;
		nDY = (int)D2CommonUnitGetY(hUnit) - nY;
		tCreate.nDX = nX - nDY;
		tCreate.nDY = nY + nDX;
		MissilesCreate(ptGame, &tCreate);
		tCreate.nDX = nX + nDY;
		tCreate.nDY = nY - nDX;
		MissilesCreate(ptGame, &tCreate);
		tCreate.dwFlags = 1;
		if (hUnit->dwClassId == MONSTER_DARKSTALKER)
			tCreate.eMissile = MISSILE_COUNTESSFIREWALL;
		else
			tCreate.eMissile = MISSILE_VAMPIREFIREWALL;
		tCreate.nDX = 0;
		tCreate.nDY = 0;
		MissilesCreate(ptGame, &tCreate);
	}
	return TRUE;
}

BOOL __fastcall SkillsVampireMeteor(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsCreateMissileAt(ptGame, hUnit, (DWORD)nLevel, MISSILE_VAMPIREMETEORCENTER, 0, 0);
	return TRUE;
}

static __inline int GargoyleTrapAbs(int n)
{
	if (n < 0)
		return -n;
	else
		return n;
}

BOOL __fastcall SkillsGargoyleTrap(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreate;
	UNIT *hTarget;
	int nTargetX;
	int nTargetY;
	int nUnitX;
	int nUnitY;
	int nDistX;
	int nDistY;
	int nX;
	int nY;
	int i;

	hUnit->dwFlags0 |= 64;
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget)
	{
		nTargetX = (int)D2CommonUnitGetX(hTarget);
		nTargetY = (int)D2CommonUnitGetY(hTarget);
		nUnitX = (int)D2CommonUnitGetX(hUnit);
		nUnitY = (int)D2CommonUnitGetY(hUnit);
		nX = nTargetX;
		nY = nTargetY;
		nDistX = GargoyleTrapAbs(nTargetX - nUnitX);
		nDistY = nTargetY - nUnitY;
		if (nDistY < 0)
			nDistY = -nDistY;
		if (nDistX < nDistY)
		{
			nX = nUnitX;
			for (i = 0; i < 4; i++)
			{
				if (nX > nTargetX)
					nX--;
				else if (nX < nTargetX)
					nX++;
			}
		}
		else
		{
			nY = nUnitY;
			for (i = 0; i < 4; i++)
			{
				if (nY > nTargetY)
					nY--;
				else if (nY < nTargetY)
					nY++;
			} }
		for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.hSource = hUnit;
		tCreate.dwFlags = 3;
		tCreate.nDX = nX - nUnitX;
		tCreate.eMissile = MISSILE_SHAFIRE3;
		tCreate.nDY = nY - nUnitY;
		tCreate.nX = tCreate.nDX / 6 + nUnitX - 1;
		tCreate.nY = tCreate.nDY / 6 + nUnitY - 1;
		tCreate.nLevel = nLevel;
		MissilesCreate(ptGame, &tCreate);
		return TRUE; }
	return TRUE;
}

static int sganTurretOffsets[3] = { 0, 1, 2 };

BOOL __fastcall SkillsDesertTurret(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreate;
	int nSpotX;
	int nSpotY;
	int nUnitX;
	int nUnitY;
	int nDX;
	int nDY;
	int i;
	int *pnOffset;

	hUnit->dwFlags0 |= 64;
	if (!SkillsGetTargetSpot(ptGame, hUnit, &nSpotX, &nSpotY))
		return FALSE;
	nUnitX = (int)D2CommonUnitGetX(hUnit);
	nUnitY = (int)D2CommonUnitGetY(hUnit);
	nDX = nSpotX - nUnitX;
	nDY = nSpotY - nUnitY;
	if (nDX < 0 && nDY < 0)
		nDX = 0;
	else if (nDX == 0 && nDY < 0)
		nDX = -nDY;
	else if (nDX > 0 && nDY < 0)
		nDY = 0;
	else if (nDX > 0 && nDY == 0)
		nDY = nDX;
	else if (nDX > 0 && nDY > 0)
		nDX = 0;
	else if (nDX == 0 && nDY > 0)
		nDX = -nDY;
	else if (nDX < 0 && nDY > 0)
		nDY = 0;
	else if (nDX < 0 && nDY == 0)
		nDY = nDX;
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = 3;
	for (pnOffset = sganTurretOffsets; (int)pnOffset < (int)&sganTurretOffsets[3]; pnOffset++)
	{
		if (nDY)
			nDX += *pnOffset;
		else
		{
			nDY = *pnOffset;
			nDX += nDY;
		}
		tCreate.hSource = hUnit;
		tCreate.nDX = nDX;
		tCreate.nDY = nDY;
		tCreate.nLevel = nLevel;
		tCreate.eMissile = MISSILE_DESERTFIREBALL;
		tCreate.nX = nDX / 8 + nUnitX;
		tCreate.nY = nDX / 8 + nUnitY;
		MissilesCreate(ptGame, &tCreate);
	}
	return TRUE;
}

BOOL __fastcall SkillsArcaneTower(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreate;
	UNIT *hTarget;
	int nUnitX;
	int nUnitY;
	int nDX;
	int nDY;
	int nTargetX;
	int nTargetY;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	hUnit->dwFlags0 |= 64;
	if (!hTarget)
		return FALSE;
	nTargetX = (int)D2CommonUnitGetX(hTarget);
	nTargetY = (int)D2CommonUnitGetY(hTarget);
	nUnitX = (int)D2CommonUnitGetX(hUnit);
	nUnitY = (int)D2CommonUnitGetY(hUnit);
	nDX = nTargetX - nUnitX;
	nDY = nTargetY - nUnitY;
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.nDX = nDX;
	tCreate.nDY = nDY;
	tCreate.nLevel = nLevel;
	tCreate.dwFlags = 3;
	tCreate.hSource = hUnit;
	tCreate.nX = nUnitX - nDX / 8;
	tCreate.nY = nUnitY - nDY / 8;
	tCreate.eMissile = MISSILE_NOVA;
	MissilesCreate(ptGame, &tCreate);
	return TRUE;
}

BOOL __fastcall SkillsSpiderLayStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsSetSkillState(hUnit, hUnit, SKILL_SPIDERLAY, nLevel, (DWORD)-1, 22, 0);
	return TRUE;
}

UNIT * __fastcall SkillsSpiderLayWalk(GAME *ptGame, UNIT *hUnit)
{
	BYTE abSteps[8];
	MISSILECREATE tCreate;
	void *ptPath;
	int nOffsetX;
	int nOffsetY;
	int nLevel;
	int nX;
	int nY;
	int i;
	BYTE bStep;
	UNIT *hMissile;
	void *ptRoom;

	hMissile = NULL;
	ptPath = hUnit->ptPath;
	if (ptPath && D2CommonStepTestSquareCrossed(ptPath))
	{
		abSteps[0] = 10;
		abSteps[1] = 8;
		abSteps[2] = 22;
		abSteps[3] = 20;
		abSteps[4] = 18;
		abSteps[5] = 16;
		abSteps[6] = 14;
		abSteps[7] = 12;
		bStep = abSteps[D2CommonMonsterRegionLookupTableB(D2CommonPathGetTargetFacingRaw((DWORD)ptPath))];
		D2CommonMonsterRegionGetOffsetPair(bStep, &nOffsetX, &nOffsetY);
		nLevel = (int)D2CommonSkillsCalcNodeLevel(hUnit, D2CommonSkillsFindByKey(hUnit, SKILL_SPIDERLAY), 1);
		nX = (int)D2CommonUnitGetX(hUnit);
		nY = (int)D2CommonUnitGetY(hUnit);
		nX += nOffsetX;
		nY += nOffsetY;
		ptRoom = (void *)D2CommonUnitGetPathRoom(hUnit);
		(void)D2CommonDungeonFindRoomGame(ptRoom, nX, nY);
		for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.dwFlags = 33;
		tCreate.hSource = hUnit;
		tCreate.nDX = nX + nOffsetX * 2;
		tCreate.nDY = nY + nOffsetY * 2;
		tCreate.nLevel = nLevel;
		tCreate.nX = nX;
		tCreate.nY = nY;
		tCreate.nVelocity = 3;
		tCreate.eMissile = MISSILE_SPIDERGOOLAY;
		hMissile = MissilesCreate(ptGame, &tCreate);
	}
	return hMissile;
}

BOOL __fastcall SkillsFetishAura(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNITFINDDATA *ptFind;
	SKILLSTXT *ptSkillsTxt;
	UNIT *hOther;
	int nX;
	int nY;
	int i;
	int nRadius;

	if (!SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY))
		return FALSE;
	ptSkillsTxt = (SKILLSTXT *)D2CommonDataTblsGetSkill(SKILL_FETISHAURA);
	nRadius = ptSkillsTxt->Param[3] + nLevel * 2;
	ptFind = SkillsFindUnits(ptGame, hUnit, nRadius, nX, nY);
	for (i = 0; i < ptFind->nNumUnits; i++)
	{
		hOther = ptFind->phUnitsArray[i];
		if (SUnitIsHostile(ptGame, hUnit, hOther))
			continue;
		if (hOther->dwClassId > MONSTER_STYGIANDOLL_400)
			continue;
		if (hOther->dwClassId < MONSTER_RATMAN_396
			&& (hOther->dwClassId < MONSTER_RATMAN || hOther->dwClassId > MONSTER_STYGIANDOLL))
			continue;
		if (SkillsMonstValidTarget(ptGame, hUnit, hOther))
			SkillsSetSkillState(hUnit, hOther, SKILL_FETISHAURA, nLevel, 68, 25, 0);
	}
	SkillsFreeUnits(ptGame, ptFind);
	return TRUE;
}

BOOL __fastcall SkillsZakarumHealStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;
	int nMaxLife;
	int nBase;
	int nRange;
	int nHeal;
	int nLife;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	hUnit->dwFlags0 |= 64;
	nMaxLife = D2CommonStatsUnitCalcMaxHp(hTarget);
	nBase = nLevel * 5;
	nRange = nBase + 15;
	if (nRange >= 100)
		nRange = 100;
	nHeal = nRange - nBase;
	if (nHeal)
		nHeal = (int)D2CommonSeedRand(&hUnit->tSeed, nHeal);
	else
		nHeal = 0;
	nHeal = (nHeal + nBase) * nMaxLife / 100;
	nLife = D2CommonStatsUnitGetValue(hTarget, 6);
	if (nLife + nHeal > nMaxLife)
		nHeal = nMaxLife - nLife;
	D2CommonStatsUnitAddValue(hTarget, 6, nHeal);
	return TRUE;
}

BOOL __fastcall SkillsSubmergeStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	hUnit->dwFlags0 &= ~14;
	return TRUE;
}

BOOL __fastcall SkillsSubmerge(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DWORD dwFrame;
	DWORD dwCount;

	D2CommonUnitGetAnimFrameAndCount(hUnit, &dwFrame, &dwCount);
	if ((int)dwFrame <= 0)
		hUnit->nSeqRate = 0;
	hUnit->bAnimEvent = 0;
	return TRUE;
}

BOOL __fastcall SkillsEmergeStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	hUnit->dwFlags0 |= 14;
	return TRUE;
}

void __fastcall SkillsMonstRevive(GAME *ptGame, UNIT *hUnit)
{
	GAMEROOM *hRoom;
	int nLife;
	DWORD *padwRegions;

#line 1158
	D2ASSERT(UnitGetType (hUnit) == UNIT_MONSTER);
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	nLife = D2CommonStatsUnitGetBaseValue(hUnit, 7);
	D2CommonStatsUnitSetValue(hUnit, 6, nLife);
	hUnit->dwFlags0 |= 14;
	hUnit->dwFlags0 |= 131072;
	hUnit->dwFlags0 |= 67108864;
	MonsterRestartAi(ptGame, hUnit);
	padwRegions = (DWORD *)&ptGame->pMonRegions;
	MonsterRegionRemoveAligned(padwRegions, hUnit);
	QuestsAttachLevelQuest(ptGame, hUnit, hRoom, FALSE);
	D2CommonCollisnSetPattern(hRoom, D2CommonUnitGetX(hUnit), D2CommonUnitGetY(hUnit), 1, 256);
}

BOOL __fastcall SkillsResurrectStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MONMODECHANGE tReq;
	GAMEROOM *hRoom;
	UNIT *hTarget;
	int eMode;
	int nX;
	int nY;
	DWORD nPattern;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	nX = (int)D2CommonUnitGetX(hTarget);
	nY = (int)D2CommonUnitGetY(hTarget);
	hUnit->dwFlags0 |= 64;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hTarget);
	nPattern = D2CommonPathGetCollisionPatternByUnit(hTarget);
	if (D2CommonCollisnTestPatternAnyRaw((DWORD)hRoom, (DWORD)nX, (DWORD)nY, nPattern, 7169))
		return FALSE;
	D2CommonUnitSetTarget(hUnit, hTarget);
	D2CommonPathSetTarget(hUnit->ptPath, hTarget);
	SkillsMonstRevive(ptGame, hTarget);
	eMode = MonsterGetSpawnMode(hTarget, SKILL_RESURRECT, 0);
	MonsterModeInit(hTarget, eMode, &tReq);
	MonsterGetSpawnMode(hTarget, SKILL_RESURRECT, 0);
	MonsterModeSet(ptGame, &tReq, eMode != 14);
	D2CommonUnitAddAnimOverlay(hTarget, 27, 0);
	return TRUE;
}

BOOL __fastcall SkillsBestowStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;
	int nMaxLife;
	int nLife;
	int nBase;
	int nRange;
	int nHeal;

	D2CommonDataTblsGetSkill(SKILL_BESTOW);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	hUnit->dwFlags0 |= 64;
	nMaxLife = D2CommonStatsUnitCalcMaxHp(hTarget);
	nLife = D2CommonStatsUnitGetValue(hTarget, 6);
	nBase = nLevel * 5;
	nRange = nBase + 15;
	if (nRange > 100)
		nRange = 100;
	nHeal = nRange - nBase;
	if (nHeal)
		nHeal = (int)D2CommonSeedRand(&hUnit->tSeed, nHeal);
	else
		nHeal = 0;
	nHeal = (nHeal + nBase) * nMaxLife / 100;
	if (nHeal + nLife > nMaxLife)
		nHeal = nMaxLife - nLife;
	D2CommonStatsUnitAddValue(hTarget, 6, nHeal);
	D2CommonUnitAddAnimOverlay(hTarget, 27, 0);
	return TRUE;
}

BOOL __fastcall SkillsPrimeBolt(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreateMissile;
	int i;
	int j;

	hUnit->dwFlags0 |= 64;
	for (i = 0; i < nLevel + 2; i++)
	{
		for (j = 0; j < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); j++)
			((DWORD *)&tCreateMissile)[j] = 0;
		tCreateMissile.dwFlags = 33;
		tCreateMissile.hSource = hUnit;
		tCreateMissile.nX = (int)D2CommonUnitGetX(hUnit);
		tCreateMissile.nY = (int)D2CommonUnitGetY(hUnit);
		tCreateMissile.nLevel = nLevel;
		tCreateMissile.eMissile = MISSILE_CHARGEDSTRIKEBOLT;
		tCreateMissile.pfnModify = SkillsChargedBoltModify;
		tCreateMissile.dwModifyArg = (DWORD)i;
		if (SkillsGetTargetSpot(ptGame, hUnit, &tCreateMissile.nDX, &tCreateMissile.nDY))
		{
#line 1299
			D2ASSERT(tCreateMissile.nDX && tCreateMissile.nDY);
			MissilesCreate(ptGame, &tCreateMissile);
		}
	}
	return TRUE;
}

BOOL __fastcall SkillsPrimeLightning(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	hUnit->dwFlags0 |= 64;
	SkillsCreateMissile(ptGame, MISSILE_MONSTERLIGHT, hUnit, nLevel, 0, 0, 0, 0, 0);
	return TRUE;
}

BOOL __fastcall SkillsPrimeBlaze(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsSetSkillState(hUnit, hUnit, SKILL_BLAZE, nLevel, (DWORD)-1, 13, 0);
	return TRUE;
}

BOOL __fastcall SkillsMonTeleport(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	GAMEROOM *hRoom;
	DWORD hPathRoom;
	int nX;
	int nY;

	nX = (int)D2CommonPathGetTargetXRaw((DWORD)hUnit->ptPath);
	nY = (int)D2CommonPathGetTargetYRaw((DWORD)hUnit->ptPath);
	hPathRoom = D2CommonUnitGetPathRoom(hUnit);
	hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(hPathRoom, nX, nY);
	if (!SUnitMoveToRoom(ptGame, hUnit, hRoom, nX, nY, FALSE, FALSE))
		return FALSE;
	D2CommonUnitRoomAddToChangedList(hUnit);
	hUnit->dwFlags |= 65536;
	return TRUE;
}

BOOL __fastcall SkillsPrimeSpike(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	hUnit->dwFlags0 |= 64;
	SkillsCreateMissile(ptGame, MISSILE_MONGLACIALSPIKE, hUnit, nLevel, 0, 0, 0, 0, 0);
	return TRUE;
}

BOOL __fastcall SkillsPrimeIceNova(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nMissile;

	if (hUnit->dwClassId == MONSTER_DIABLO)
		nMissile = MISSILE_DIABFIRE;
	else
		nMissile = MISSILE_FROSTNOVA;
	hUnit->dwFlags0 |= 64;
	SkillsNova(ptGame, hUnit, hUnit, nMissile, nLevel, D2CommonMissileTblsGetVel(nMissile));
	return TRUE;
}

BOOL __fastcall SkillsPrimePoisonball(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;
	int nOffsetX;
	int nOffsetY;
	int i;

	hUnit->dwFlags0 |= 64;
	SkillsCreateMissile(ptGame, MISSILE_POISONBALL, hUnit, nLevel, 0, 0, 0, 0, 0);
	if (!SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY))
		return FALSE;
	nOffsetY = 5;
	nOffsetX = 5;
	for (i = 3; i; i--)
	{
		if (D2CommonSeedNext(&hUnit->tSeed) & 1)
			nOffsetX = -nOffsetX;
		if (D2CommonSeedNext(&hUnit->tSeed) & 1)
			nOffsetY = -nOffsetY;
		SkillsCreateMissile(ptGame, MISSILE_POISONBALL, hUnit, nLevel, 0, 0, nX + nOffsetX, nY + nOffsetY, 0);
	}
	return TRUE;
}

BOOL __fastcall SkillsPrimePoisonNova(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreateMissile;
	MISSILESTXT *ptMissile;
	int i;

	hUnit->dwFlags0 |= 64;
	ptMissile = (MISSILESTXT *)D2CommonMissileTblsGetMissile(MISSILE_PRIMEPOISONCLOUD);
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreateMissile)[i] = 0;
	tCreateMissile.dwFlags = 31;
	tCreateMissile.hSource = hUnit;
	tCreateMissile.hSpawnLocation = hUnit;
	tCreateMissile.eMissile = MISSILE_PRIMEPOISONCLOUD;
	tCreateMissile.nX = (int)D2CommonUnitGetX(hUnit);
	tCreateMissile.nY = (int)D2CommonUnitGetY(hUnit);
	tCreateMissile.nLevel = nLevel;
	tCreateMissile.nSubLoops = nLevel - 1;
	tCreateMissile.nVelocity = ptMissile->Param1 << 6;
	{
	int anRingX[16] = { 0, 1, 2, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1 };
	int anRingY[16] = { 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1, 0, 1, 2, 2 };
	for (i = 0; i < 16; i += 2)
	{
		tCreateMissile.nDX = anRingX[i];
		tCreateMissile.nDY = anRingY[i];
		MissilesCreate(ptGame, &tCreateMissile);
	}
	tCreateMissile.nVelocity = ptMissile->Param2 << 6;
	for (i = 1; i < 16; i += 2)
	{
		tCreateMissile.nDX = anRingX[i];
		tCreateMissile.nDY = anRingY[i];
		MissilesCreate(ptGame, &tCreateMissile);
	}
	}
	return TRUE;
}

static __inline int SkillsMonstInfernoDelay(int nBaseClass)
{
	switch (nBaseClass)
	{
	case MONSTER_DIABLO:
		return 6;
	case MONSTER_RATMANSHAMAN:
		return 14;
	case MONSTER_IRON_WOLF:
		return 7;
	default:
		return 1;
	}
}

BOOL __fastcall SkillsFetishInfernoStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nDelay;
	DWORD hSkill;

	nDelay = 15;
	if (hUnit->dwClassId == MONSTER_DIABLO)
		nDelay = 40;
	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	D2CommonSkillsSetNodeScratchC(hSkill, ptGame->nFrame + nDelay);
	if (!D2CommonStatsGetState(hUnit, 12))
		D2CommonStatsUnitSetActiveStateBit(hUnit, 12, 1);
	return TRUE;
}

BOOL __fastcall SkillsFetishInferno(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreateMissile;
	MISSILESTXT *ptMissile;
	MISSILESTXT *ptMissileB;
	UNIT *hMissile;
	UNIT *hOwner;
	int nBaseClass;
	int nMissile;
	int nRange;
	int i;
	int nDelay;
	BOOL fResult;

	nBaseClass = (int)D2CommonMonsterUnitGetTypeLocalIndex(hUnit);
	if (nBaseClass == MONSTER_DIABLO)
		nMissile = MISSILE_DIABLIGHT;
	else
		nMissile = MISSILE_FETISHINFERNO1;
	ptMissile = (MISSILESTXT *)D2CommonMissileTblsGetMissile(nMissile);
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreateMissile)[i] = 0;
	tCreateMissile.dwFlags = 37;
	tCreateMissile.hSource = hUnit;
	tCreateMissile.nX = (int)D2CommonUnitGetX(hUnit);
	tCreateMissile.nY = (int)D2CommonUnitGetY(hUnit);
	tCreateMissile.nLevel = nLevel;
	tCreateMissile.eMissile = nMissile;
	tCreateMissile.nVelocity = ptMissile->Vel;
	if (!SkillsGetTargetSpot(ptGame, hUnit, &tCreateMissile.nDX, &tCreateMissile.nDY))
	{
		nDelay = SkillsMonstInfernoDelay(nBaseClass);
		EventRemoveByType(ptGame, hUnit, 0);
		EventAdd(ptGame, hUnit, 1, ptGame->nFrame + nDelay);
		fResult = FALSE;
	}
	else
	{
		hMissile = MissilesCreate(ptGame, &tCreateMissile);
		if (!hMissile)
		{
			nDelay = SkillsMonstInfernoDelay(nBaseClass);
			EventRemoveByType(ptGame, hUnit, 0);
			EventAdd(ptGame, hUnit, 1, ptGame->nFrame + nDelay);
			fResult = FALSE;
		}
		else
		{
			hOwner = SUnitGetOwner(SUnitGetGame(hMissile), hMissile);
			ptMissileB = (MISSILESTXT *)D2CommonMissileTblsGetMissile(hMissile->dwClassId);
			nRange = ptMissileB->Param2 + nLevel - 1;
			if (hOwner->dwClassId == MONSTER_IRON_WOLF)
				nRange += 3;
			D2CommonPathSetDistMax(hMissile->ptPath, nRange);
			D2CommonUnitSetMissileAnimRate(hMissile, ((int)ptMissileB->AnimLen << 8) / nRange);
			D2CommonMissileSetRange(hMissile, nRange);
			D2CommonMissileSetRangeLeft(hMissile, nRange);
			hUnit->nAnimPos = 2816;
			if (ptGame->nFrame < (int)D2CommonSkillsGetNodeScratchC(D2CommonUnitGetSkillThirdCached(hUnit))
				&& D2CommonStatsGetState(hUnit, 12))
			{
				EventRemoveByType(ptGame, hUnit, 1);
				EventAdd(ptGame, hUnit, 0, ptGame->nFrame + 2);
			}
			else
			{
				nDelay = SkillsMonstInfernoDelay(nBaseClass);
				EventRemoveByType(ptGame, hUnit, 0);
				EventAdd(ptGame, hUnit, 1, ptGame->nFrame + nDelay);
			}
			fResult = TRUE;
		}
	}
	return fResult;
}

BOOL __fastcall SkillsDiabCold(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	SKILLSTXT *ptSkill;
	UNIT *hTarget;
	int nMin;
	int nRange;
	int i;

	hUnit->dwFlags0 |= 64;
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return TRUE;
	ptSkill = (SKILLSTXT *)D2CommonDataTblsGetSkill(SKILL_DIABCOLD);
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	nMin = D2CommonSkillsCalcEMinByLevel(SKILL_DIABCOLD, nLevel);
	nRange = D2CommonSkillsCalcEMaxByLevel(SKILL_DIABCOLD, nLevel) - nMin;
	tDamage.nColdDamage = (nRange ? (int)D2CommonSeedRand(&hUnit->tSeed, nRange) : 0) + nMin;
	tDamage.nColdDamage <<= ptSkill->HitShift;
	tDamage.nMagicDamage = tDamage.nColdDamage * 2;
	tDamage.nFreezeLength = ptSkill->Param[1];
	tDamage.wFlags |= 1;
	SUnitDmgApply(ptGame, hUnit, hTarget, TRUE, &tDamage);
	if (!D2CommonStatsUnitGetValue(hTarget, 6))
		tDamage.wFlags |= 2;
	SUnitDmgReact(ptGame, hUnit, hTarget, &tDamage);
	return TRUE;
}

BOOL __fastcall SkillsFingerMageSpider(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreateMissile;
	UNIT *hTarget;
	int nX;
	int nY;
	int i;

	hUnit->dwFlags0 |= 64;
	D2CommonMissileTblsGetMissile(MISSILE_FINGERMAGESPIDER);
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreateMissile)[i] = 0;
	tCreateMissile.dwFlags = 3;
	tCreateMissile.hSource = hUnit;
	tCreateMissile.hSpawnLocation = hUnit;
	tCreateMissile.eMissile = MISSILE_FINGERMAGESPIDER;
	tCreateMissile.nX = (int)D2CommonUnitGetX(hUnit);
	tCreateMissile.nY = (int)D2CommonUnitGetY(hUnit);
	tCreateMissile.nLevel = nLevel;
	tCreateMissile.nSubLoops = nLevel - 1;
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget)
	{
		D2CommonPathTurnToSquare(hUnit->ptPath, (int)D2CommonUnitGetX(hTarget), (int)D2CommonUnitGetY(hTarget), 0);
		tCreateMissile.nDX = (int)D2CommonUnitGetX(hTarget) - (int)D2CommonUnitGetX(hUnit);
		tCreateMissile.nDY = (int)D2CommonUnitGetY(hTarget) - (int)D2CommonUnitGetY(hUnit);
	}
	else
	{
		nX = (int)D2CommonUnitGetX(hUnit);
		tCreateMissile.nDX = (int)D2CommonPathGetTargetXRaw((DWORD)hUnit->ptPath) - nX;
		nY = (int)D2CommonUnitGetY(hUnit);
		tCreateMissile.nDY = (int)D2CommonPathGetTargetYRaw((DWORD)hUnit->ptPath) - nY;
	}
	MissilesCreate(ptGame, &tCreateMissile);
	return TRUE;
}

BOOL __fastcall SkillsRegurgitatorEat(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget || hTarget->eType != UNIT_MONSTER || hTarget->dwMode != 12)
		return FALSE;
	D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hTarget), 1, hTarget->dwId);
	SUnitFree(ptGame, hTarget);
	return TRUE;
}

BOOL __fastcall SkillsDiabWall(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreateMissile;
	int i;

	hUnit->dwFlags0 |= 64;
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreateMissile)[i] = 0;
	tCreateMissile.dwFlags = 33;
	tCreateMissile.hSource = hUnit;
	tCreateMissile.nX = (int)D2CommonUnitGetX(hUnit);
	tCreateMissile.nY = (int)D2CommonUnitGetY(hUnit);
	tCreateMissile.nLevel = nLevel;
	tCreateMissile.eMissile = MISSILE_DIABWALLMAKER;
	tCreateMissile.pfnModify = SkillsDiabWallModify;
	if (SkillsGetTargetSpot(ptGame, hUnit, &tCreateMissile.nDX, &tCreateMissile.nDY))
	{
		for (i = 0; i < nLevel; i++)
		{
			tCreateMissile.dwModifyArg = (DWORD)i;
			MissilesCreate(ptGame, &tCreateMissile);
		}
	}
	return TRUE;
}

void __fastcall SkillsDiabWallModify(UNIT *hMissile, DWORD dwIndex)
{
	void *ptPath;
	int nRange;

	if (!hMissile)
		return;
	nRange = (int)D2CommonMissileGetRange(hMissile);
	if (nRange >= 100)
	{
		nRange = 99;
		D2CommonMissileSetRange(hMissile, nRange);
	}
	ptPath = hMissile->ptPath;
	D2CommonSeedSet(&hMissile->tSeed, D2CommonPathGetTargetXRaw((DWORD)ptPath) + dwIndex);
	if (D2CommonSeedRand(&hMissile->tSeed, 100) < 20)
		return;
	D2CommonPathSetType(ptPath, 10);
	D2CommonPathSetDistMax(ptPath, nRange);
	D2CommonPathCompute(ptPath, hMissile);
}

BOOL __fastcall SkillsDiabRunStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;
	DWORD hSkill;
	int nY;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	D2CommonSkillsSetNodeFlags(hSkill, 0);
	D2CommonSkillsSetNodeScratchC(hSkill, hTarget->dwId);
	D2CommonSkillsSetNodeScratchD(hSkill, hTarget->eType);
	D2CommonPathSetTarget(hUnit->ptPath, 0);
	nY = (int)D2CommonUnitGetY(hTarget);
	D2CommonPathSetTargetCoord(hUnit->ptPath, D2CommonUnitGetX(hTarget), nY);
	return TRUE;
}

BOOL __fastcall SkillsDiabRun(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	DWORD hSkill;
	void *ptPath;
	int nFrame;
	int nCount;
	int nMin;
	int nRange;
	int nDamage;
	DWORD dwId;
	int i;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	if (D2CommonSkillsGetNodeFlags(hSkill) & 2)
	{
		D2CommonSkillsSetNodeFlags(hSkill, 0);
		hUnit->nAnimLength = 2048;
		D2CommonUnitSetAnimSequencePos(hUnit, 14);
		return TRUE;
	}
	D2CommonUnitGetAnimFrameAndCount(hUnit, &nFrame, &nCount);
	if (nFrame == 5)
	{
		ptPath = hUnit->ptPath;
		D2CommonPathSetVelocity(ptPath,
			(int)((DWORD)D2CommonStatsUnitGetValue(hUnit, 67) * 5 << 10) / 100);
		D2CommonPathSetType(ptPath, 1);
		D2CommonPathCompute(ptPath, hUnit);
		D2CommonSkillsSetNodeFlags(hSkill, 1);
		return TRUE;
	}
	if (nFrame == 13)
	{
		hUnit->nAnimLength = 4096;
		D2CommonUnitSetAnimSequencePos(hUnit, 6);
		return TRUE;
	}
	if (hUnit->bAnimEvent != 1)
		return TRUE;
	dwId = D2CommonSkillsGetNodeScratchC(hSkill);
	hTarget = SUnitFind(ptGame, D2CommonSkillsGetNodeScratchD(hSkill), dwId);
	if (!hTarget || !D2CommonUnitTestPositionClear(hUnit, hTarget, 0))
		return TRUE;
	hUnit->bAnimEvent = 0;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	nMin = D2CommonSkillsCalcValueA(SKILL_DIABRUN, hUnit, nLevel);
	nRange = D2CommonSkillsCalcValueB(SKILL_DIABRUN, hUnit, nLevel) - nMin;
	if (nRange)
		nDamage = (int)D2CommonSeedRand(&hUnit->tSeed, nRange);
	else
		nDamage = 0;
	tDamage.nPhysicalDamage = (nDamage + nMin) << 8;
	tDamage.wFlags = 5;
	SUnitDmgApply(ptGame, hUnit, hTarget, TRUE, &tDamage);
	if (!D2CommonStatsUnitGetValue(hTarget, 6))
		tDamage.wFlags |= 2;
	SUnitDmgReact(ptGame, hUnit, hTarget, &tDamage);
	return TRUE;
}

BOOL __fastcall SkillsDiabPrisonStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;
	void *ptPath;
	DWORD dwId;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
	{
		ptPath = hUnit->ptPath;
		if (!ptPath)
			return FALSE;
		dwId = D2CommonPathGetTargetXRaw((DWORD)ptPath);
		hTarget = SUnitFind(ptGame, D2CommonPathGetTargetYRaw((DWORD)ptPath), dwId);
		if (!hTarget)
			return FALSE;
	}
	MonsterSpawnFormationAt(ptGame, (GAMEROOM *)D2CommonUnitGetPathRoom(hTarget), 0, 0, hTarget,
		MONSTER_PRISONFL, 0);
	return TRUE;
}

BOOL __fastcall SkillsMonBlizzard(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsCreateMissileAt(ptGame, hUnit, (DWORD)nLevel, MISSILE_MONBLIZCENTER, 0, 0);
	return TRUE;
}

BOOL __fastcall SkillsMosquitoStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	D2SEED tSeed;
	MONSTATSTXT *ptMonStats;
	UNIT *hTarget;
	DWORD hSkill;
	int nRange;
	int nCount;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	ptMonStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId);
	D2CommonSeedInit(&tSeed);
	D2CommonSeedSet(&tSeed, 1397049683);
	tSeed.dwHigh = hUnit->dwId;
	nRange = (int)ptMonStats->AIParam[1] - (int)ptMonStats->AIParam[0];
	nCount = (nRange ? (int)D2CommonSeedRand(&tSeed, nRange) : 0) + ptMonStats->AIParam[0];
	if (nCount < 1)
		nCount = 1;
	D2CommonSkillsSetNodeScratchC(hSkill, nCount);
	D2CommonSkillsSetNodeScratchD(hSkill, hTarget->dwId);
	D2CommonSkillsSetNodeScratchE(hSkill, hTarget->eType);
	return TRUE;
}

BOOL __fastcall SkillsMosquito(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	D2SEED *ptSeed;
	UNIT *hTarget;
	DWORD hSkill;
	int nCount;
	int nMin;
	int nMax;
	int nLength;
	int nLife;
	int nMaxLife;
	int nDamage;
	DWORD dwId;
	int i;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	nCount = (int)D2CommonSkillsGetNodeScratchC(hSkill);
	dwId = D2CommonSkillsGetNodeScratchD(hSkill);
	hTarget = SUnitFind(ptGame, D2CommonSkillsGetNodeScratchE(hSkill), dwId);
	if (!hTarget || !D2CommonUnitTestPositionClear(hUnit, hTarget, 0))
		return TRUE;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	ptSeed = &hUnit->tSeed;
	nMin = D2CommonSkillsCalcValueA(SKILL_MOSQUITO, hUnit, nLevel);
	nMax = D2CommonSkillsCalcValueB(SKILL_MOSQUITO, hUnit, nLevel);
	D2CommonSkillsCalcEMinByLevel(SKILL_MOSQUITO, nLevel);
	D2CommonSkillsCalcEMaxByLevel(SKILL_MOSQUITO, nLevel);
	nLength = D2CommonSkillsCalcELenByLevel(SKILL_MOSQUITO, nLevel);
	if (nMax - nMin)
		nDamage = (int)D2CommonSeedRand(ptSeed, nMax - nMin);
	else
		nDamage = 0;
	tDamage.nPhysicalDamage = (nDamage + nMin) << 8;
	if (nMax - nMin)
		nDamage = (int)D2CommonSeedRand(ptSeed, nMax - nMin);
	else
		nDamage = 0;
	tDamage.nPoisonDamage = (nDamage + nMin) << 1;
	tDamage.nPoisonLength = nLength;
	if (nMax - nMin)
		nDamage = (int)D2CommonSeedRand(ptSeed, nMax - nMin);
	else
		nDamage = 0;
	tDamage.nManaDrain = (nDamage + nMin) << 8;
	if (nMax - nMin)
		nDamage = (int)D2CommonSeedRand(ptSeed, nMax - nMin);
	else
		nDamage = 0;
	tDamage.nStaminaDrain = (nDamage + nMin) << 8;
	tDamage.wFlags = 1;
	nLife = D2CommonStatsUnitGetValue(hUnit, 6);
	nMaxLife = D2CommonStatsUnitCalcMaxHp(hUnit);
	nLife += tDamage.nPhysicalDamage >> 8;
	if (nLife > nMaxLife)
		nLife = nMaxLife;
	D2CommonStatsUnitSetValue(hUnit, 6, nLife);
	SUnitDmgApply(ptGame, hUnit, hTarget, TRUE, &tDamage);
	if (!D2CommonStatsUnitGetValue(hTarget, 6))
		tDamage.wFlags |= 2;
	SUnitDmgReact(ptGame, hUnit, hTarget, &tDamage);
	nCount--;
	if (nCount)
	{
		D2CommonUnitSetAnimSequencePos(hUnit, 12);
		hUnit->nAnimLength = (hUnit->nAnimLength & ~255) + 256;
		D2CommonSkillsSetNodeScratchC(hSkill, nCount);
	}
	return TRUE;
}

BOOL __fastcall SkillsMonFrozenArmor(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLSTATE tState;
	DWORD hList;
	int i;

	hList = D2CommonStatsFindListByState(hUnit, 10);
	if (hList)
	{
		D2CommonStatsUnitUnlinkList(hUnit, hList);
		D2CommonStatsListFree(hList);
	}
	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tState)[i] = 0;
	tState.hSource = hUnit;
	tState.hTarget = hUnit;
	tState.nSkill = SKILL_MONFROZENARMOR;
	tState.nLevel = nLevel;
	tState.nDuration = D2CommonSkillsCalcParamThreeByLevel(SKILL_MONFROZENARMOR, nLevel);
	tState.dwStatId = 171;
	tState.nStatValue = D2CommonSkillsCalcParamOneByLevel(SKILL_MONFROZENARMOR, nLevel);
	tState.dwState = 10;
	return (BOOL)(DWORD)SkillsSetState(&tState);
}

BOOL __fastcall SkillsMonBoneArmor(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DWORD hList;
	int nArmor;

	nArmor = D2CommonSkillsCalcParamOneByLevel(SKILL_MONBONEARMOR, nLevel);
	nArmor <<= 8;
	hUnit->dwFlags0 |= 64;
	D2CommonUnitRoomAddToChangedList(hUnit);
	hList = D2CommonStatsFindListByState(hUnit, 14);
	if (!hList)
	{
		D2CommonStatsUnitSetActiveStateBit(hUnit, 14, 1);
		hList = D2CommonStatsAllocList(ptGame->pMemPool, 0, 0, hUnit->eType, hUnit->dwId);
		D2CommonStatsSetListState(hList, 14);
		D2CommonStatsListSetValueB(hList, 132, nArmor);
		D2CommonStatsListSetValueB(hList, 133, nArmor);
		D2CommonStatsUnitLinkList(hUnit, hList);
		return TRUE;
	}
	D2CommonStatsUnitSetActiveStateValue(hUnit, 14, 1);
	D2CommonStatsListSetValueB(hList, 132, nArmor);
	D2CommonStatsListSetValueB(hList, 133, nArmor);
	return TRUE;
}

BOOL __fastcall SkillsMonBoneSpirit(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreateMissile;
	UNIT *hTarget;
	UNIT *hMissile;
	int nX;
	int nY;
	int nDX;
	int nDY;
	int i;
	int *pnOffset;

	hUnit->dwFlags0 |= 64;
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY))
		return FALSE;
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreateMissile)[i] = 0;
	tCreateMissile.dwFlags = 32;
	tCreateMissile.hSource = hUnit;
	tCreateMissile.hSpawnLocation = hUnit;
	tCreateMissile.hTarget = hTarget;
	tCreateMissile.nDX = nX;
	tCreateMissile.nDY = nY;
	tCreateMissile.nLevel = nLevel;
	tCreateMissile.eMissile = MISSILE_MONBONESPIRIT;
	if (!hTarget)
		tCreateMissile.dwFlags = 1056;
	hMissile = MissilesCreate(ptGame, &tCreateMissile);
	if (hMissile)
	{
		if (hTarget)
			D2CommonMissileSetScratchFirst(hMissile, 1);
		else
		{
			D2CommonMissileSetScratchFirst(hMissile, 2);
			nDX = nX - (int)D2CommonUnitGetX(hUnit);
			nDY = nY - (int)D2CommonUnitGetY(hUnit);
			D2CommonMissileSetScratchSecond(hMissile, ((DWORD)(int)(short)nDY << 16) + (DWORD)(int)(short)nDX);
		}
	}
	return TRUE;
}

void __fastcall SkillsMonstAmplifyDamage(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel)
{
	SKILLSTATE tState;
	int i;

	if (!SkillsMonstValidTarget(ptGame, hUnit, hTarget))
		return;
	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tState)[i] = 0;
	tState.hSource = hUnit;
	tState.hTarget = hTarget;
	tState.nSkill = SKILL_NECROMANCER_START;
	tState.nLevel = nLevel;
	tState.nDuration = D2CommonSkillsCalcParamThreeByLevel(SKILL_NECROMANCER_START, nLevel);
	tState.dwStatId = 36;
	tState.nStatValue = -(int)D2CommonSkillsGetTypeParamFive(SKILL_NECROMANCER_START);
	tState.dwState = 9;
	tState.pfnRemove = 0;
	SkillsSetState(&tState);
}

void __fastcall SkillsMonstWeaken(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel)
{
	SKILLSTATE tState;
	int i;

	if (!SkillsMonstValidTarget(ptGame, hUnit, hTarget))
		return;
	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tState)[i] = 0;
	tState.hSource = hUnit;
	tState.hTarget = hTarget;
	tState.nSkill = SKILL_WEAKEN;
	tState.nLevel = nLevel;
	tState.nDuration = D2CommonSkillsCalcParamThreeByLevel(SKILL_WEAKEN, nLevel);
	tState.dwStatId = 25;
	tState.nStatValue = -(int)D2CommonSkillsGetTypeParamFive(SKILL_WEAKEN);
	tState.dwState = 19;
	tState.pfnRemove = 0;
	SkillsSetState(&tState);
}

void __fastcall SkillsMonstIronMaiden(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel)
{

	if (!SkillsMonstValidTarget(ptGame, hUnit, hTarget))
		return;
	SkillsSetCurseState(hUnit, hTarget, SKILL_IRON_MAIDEN, nLevel, 129, nLevel, 55);
}

void __fastcall SkillsMonstLifeTap(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel)
{

	if (!SkillsMonstValidTarget(ptGame, hUnit, hTarget))
		return;
	SkillsSetCurseState(hUnit, hTarget, SKILL_LIFE_TAP, nLevel, 130, nLevel, 58);
}

void __fastcall SkillsMonstDecrepify(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel)
{
	DWORD hList;
	DWORD dwValue;

	if (!SkillsMonstValidTarget(ptGame, hUnit, hTarget))
		return;
	hList = (DWORD)SkillsSetCurseState(hUnit, hTarget, SKILL_DECREPIFY, nLevel, 170, nLevel, 60);
	if (!hList)
		return;
	dwValue = D2CommonSkillsGetTypeParamFive(SKILL_DECREPIFY);
	D2CommonStatsListSetValueB(hList, 67, dwValue);
	D2CommonStatsListSetValueB(hList, 36, dwValue);
	D2CommonStatsListSetValueB(hList, 25, dwValue);
}

void __fastcall SkillsMonstLowerResist(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel)
{
	DWORD hList;
	int nResist;

	if (!SkillsMonstValidTarget(ptGame, hUnit, hTarget))
		return;
	nResist = -D2CommonSkillsCalcCurveParamFive(nLevel, SKILL_LOWER_RESIST);
	hList = (DWORD)SkillsSetCurseState(hUnit, hTarget, SKILL_LOWER_RESIST, nLevel, 39, nResist, 61);
	if (!hList)
		return;
	D2CommonStatsListSetValueA(hList, 41, nResist);
	D2CommonStatsListSetValueA(hList, 43, nResist);
	D2CommonStatsListSetValueA(hList, 45, nResist);
}

struct MONSTERCURSE
{
 	int			nSkill;
 	int			nRange;
 	PFNSKILLMONSTCURSE	pfnCurse;
};
typedef struct MONSTERCURSE MONSTERCURSE;

static MONSTERCURSE sgatMonsterCurses[NUM_MONSTER_CURSES] =
{
	{ SKILL_NECROMANCER_START, 3, SkillsMonstAmplifyDamage },
	{ SKILL_WEAKEN, 3, SkillsMonstWeaken },
	{ SKILL_IRON_MAIDEN, 3, SkillsMonstIronMaiden },
	{ SKILL_LIFE_TAP, 3, SkillsMonstLifeTap },
	{ SKILL_DECREPIFY, 3, SkillsMonstDecrepify },
	{ SKILL_LOWER_RESIST, 3, SkillsMonstLowerResist },
};

BOOL __fastcall SkillsMonCurseCast(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nCurseLevel;
	int nCurse;

	nCurse = (int)((DWORD)nLevel >> 16);
	nCurseLevel = nLevel & 65535;
	if (!ptGame || !hUnit)
		return FALSE;
	if (nCurseLevel <= 0)
		nCurseLevel = 1;
	if (nCurse > NUM_MONSTER_CURSES)
		return FALSE;
	SkillsCurseArea(ptGame, sgatMonsterCurses[nCurse].nRange, hUnit, nCurseLevel,
		sgatMonsterCurses[nCurse].nSkill, sgatMonsterCurses[nCurse].pfnCurse);
	return TRUE;
}

BOOL __fastcall SkillsHellMeteorStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsCreateMissileAt(ptGame, hUnit, (DWORD)nLevel, MISSILE_HELLMETEORDOWN, 0, 0);
	return TRUE;
}

BOOL __fastcall SkillsMonFrenzyStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	DWORD hSkill;
	int i;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.wFlags = SUnitDmgHit(ptGame, hUnit, hTarget,
		D2CommonSkillsCalcToHitByLevel(SKILL_MONFRENZY, nLevel));
	SUnitDmgHold(ptGame, hUnit, hTarget, &tDamage);
	if (hSkill)
	{
		if (tDamage.wFlags & 1)
		{
			D2CommonSkillsSetNodeScratchC(hSkill, 1);
			return TRUE;
		}
		D2CommonSkillsSetNodeScratchC(hSkill, 0);
	}
	return TRUE;
}

BOOL __fastcall SkillsMonFrenzy(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;
	DWORD hSkill;
	DWORD hList;
	int nLength;
	int nEndFrame;
	int nCount;
	int nValue;

	hSkill = D2CommonUnitGetSkillThirdCached(hUnit);
	hTarget = SUnitGetTarget(ptGame, hUnit);
	nLength = D2CommonSkillsCalcParamOneByLevel(SKILL_MONFRENZY, nLevel);
	if (!hTarget)
		return FALSE;
	SUnitDmgDeliver(ptGame, hUnit, hTarget);
	hUnit->dwFlags0 |= 64;
	if (hSkill && !D2CommonSkillsGetNodeScratchC(hSkill))
		return FALSE;
	hList = D2CommonStatsFindListByState(hUnit, 103);
	nEndFrame = ptGame->nFrame + nLength;
	if (!hList)
	{
		hList = D2CommonStatsAllocList(ptGame->pMemPool, 2, nEndFrame, hUnit->eType,
			hUnit->dwId);
		if (!hList)
			return FALSE;
		D2CommonStatsSetListState(hList, 103);
		D2CommonStatsSetListRemoveCallback(hList, SkillsFrenzyStateEnd);
		D2CommonStatsListSetValueB(hList, 169, 0);
		D2CommonStatsUnitLinkList(hUnit, hList);
		D2CommonStatsUnitSetActiveStateBit(hUnit, 103, 1);
	}
	D2CommonStatsSetListEndFrame(hList, nEndFrame);
	EventAdd(ptGame, hUnit, 12, nEndFrame);
	D2CommonStatsUnitSetActiveStateValue(hUnit, 103, 1);
	nCount = (int)D2CommonStatsListGetValueA(hList, 169) + 1;
	D2CommonStatsListSetValueB(hList, 169, nCount);
	nValue = D2CommonSkillsCalcCurveParamThree(nCount, SKILL_MONFRENZY);
	D2CommonStatsListSetValueB(hList, 67, nValue);
	D2CommonStatsListSetValueB(hList, 68, nValue);
	D2CommonStatsListSetValueB(hList, 69, nValue);
	D2CommonUnitUpdateAnimRate(hUnit);
	return TRUE;
}

struct MONSTERSKILLCHART
{
 	PFNSKILLFN	pfnStart;
 	PFNSKILLFN	pfnUse;
};
typedef struct MONSTERSKILLCHART MONSTERSKILLCHART;

#define Objects31480	((PFNSKILLFN)ObjectThemeNone)

static MONSTERSKILLCHART sgptSkillChart[NUM_MONSTER_SKILLS] =
{
	{ SkillsFireHitStart, SkillsFireHit },
	{ NULL, SkillsUnHolyBolt },
	{ NULL, NULL },
	{ SkillsMaggotEggStart, SkillsMaggotEgg },
	{ NULL, SkillsShamanFire },
	{ SkillsMagottUpStart, Objects31480 },
	{ SkillsMagottDownStart, SkillsMagottDown },
	{ NULL, SkillsMagottLay },
	{ SkillsAndrialSprayStart, SkillsAndrialSpray },
	{ SkillsJumpStart, SkillsJump },
	{ SkillsSwarmMoveStart, SkillsSwarmMove },
	{ SkillsNestStart, SkillsNest },
	{ SkillsQuickStrikeStart, SkillsQuickStrike },
	{ NULL, SkillsVampireFireball },
	{ NULL, SkillsVampireFirewall },
	{ NULL, SkillsVampireMeteor },
	{ NULL, SkillsGargoyleTrap },
	{ SkillsSpiderLayStart, NULL },
	{ NULL, NULL },
	{ NULL, NULL },
	{ SkillsSubmergeStart, SkillsSubmerge },
	{ NULL, SkillsFetishAura },
	{ SkillsFetishInfernoStart, SkillsFetishInferno },
	{ SkillsZakarumHealStart, NULL },
	{ SkillsEmergeStart, NULL },
	{ SkillsResurrectStart, NULL },
	{ SkillsBestowStart, NULL },
	{ NULL, SkillsUseLevelAsMissile },
	{ NULL, SkillsMonTeleport },
	{ NULL, SkillsPrimeLightning },
	{ NULL, SkillsPrimeBolt },
	{ NULL, SkillsPrimeBlaze },
	{ NULL, SkillsVampireFirewall },
	{ NULL, SkillsPrimeSpike },
	{ NULL, SkillsPrimeIceNova },
	{ NULL, SkillsPrimePoisonball },
	{ NULL, SkillsPrimePoisonNova },
	{ SkillsFetishInfernoStart, SkillsFetishInferno },
	{ NULL, SkillsDiabCold },
	{ NULL, SkillsPrimeIceNova },
	{ NULL, SkillsFingerMageSpider },
	{ NULL, SkillsDiabWall },
	{ SkillsDiabRunStart, SkillsDiabRun },
	{ SkillsDiabPrisonStart, NULL },
	{ NULL, SkillsPoisonBallTrap },
	{ NULL, SkillsAndyPoisonBolt },
	{ NULL, SkillsUseLevelAsMissile },
	{ NULL, SkillsDesertTurret },
	{ NULL, SkillsArcaneTower },
	{ NULL, SkillsMonBlizzard },
	{ SkillsMosquitoStart, SkillsMosquito },
	{ NULL, SkillsCursedBallTrapRight },
	{ NULL, SkillsCursedBallTrapLeft },
	{ NULL, SkillsMonFrozenArmor },
	{ NULL, SkillsMonBoneArmor },
	{ NULL, SkillsMonBoneSpirit },
	{ NULL, SkillsMonCurseCast },
	{ SkillsHellMeteorStart, NULL },
	{ NULL, SkillsRegurgitatorEat },
	{ SkillsMonFrenzyStart, SkillsMonFrenzy },
	{ NULL, NULL },
};

BOOL __fastcall SkillsMonstStart(GAME *ptGame, UNIT *hUnit, int nSkillId  ,
	int nLevel)
{
#line 2438
	D2ASSERT(nSkillId >= SKILL_MONSTER_START);
#line 2439
	D2ASSERT(nSkillId - SKILL_MONSTER_START < NUM_MONSTER_SKILLS);
	if (!SkillsHasMana(hUnit))
		return FALSE;
	if (sgptSkillChart[nSkillId - SKILL_MONSTER_START].pfnStart) {
#line 2444
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgptSkillChart[nSkillId - SKILL_MONSTER_START].pfnStart), "sgptSkillChart [nSkillId - SKILL_MONSTER_START].pfnStart");
		if (sgptSkillChart[nSkillId - SKILL_MONSTER_START].pfnStart(ptGame, hUnit, nLevel)) {
			SkillsUseMana(hUnit);
			return TRUE; }
		return FALSE; }
	return TRUE;
}

BOOL __fastcall SkillsMonstUse(GAME *ptGame, UNIT *hUnit, int nSkillId  ,
	int nLevel)
{
#line 2458
	D2ASSERT(nSkillId >= SKILL_MONSTER_START);
#line 2459
	D2ASSERT(nSkillId - SKILL_MONSTER_START < NUM_MONSTER_SKILLS);
	if (sgptSkillChart[nSkillId - SKILL_MONSTER_START].pfnUse) {
#line 2462
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgptSkillChart[nSkillId - SKILL_MONSTER_START].pfnUse), "sgptSkillChart [nSkillId - SKILL_MONSTER_START].pfnUse");
		if (sgptSkillChart[nSkillId - SKILL_MONSTER_START].pfnUse(ptGame, hUnit, nLevel)) {
			if (!sgptSkillChart[nSkillId - SKILL_MONSTER_START].pfnStart)
				SkillsUseMana(hUnit);
			return TRUE; }
		return FALSE; }
	return TRUE;
}
