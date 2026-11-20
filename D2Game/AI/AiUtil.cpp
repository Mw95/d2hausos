#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\Ai\\AiUtil.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "../GAME/Targets.h"
#include "../MONSTER/MonsterAI.h"
#include "../UNIT/SUnit.h"
#include "AiGeneral.h"
#include "AiUtil.h"
#include "AiThink.h"
#include "../GUILDS/Guilds.h"

#define UNIT_OBJECT				2
#define UNIT_MISSILE			3
#define MONDATA(hUnit)	((MONSTERDATA *)(hUnit)->uTypeData)
#define AIFIND_NONE				2147483647
#define AIFIND_KINDS			13
#define AITARGET_PLAYER_LISTS	8
#define AITARGET_LIST_FURTHER	8
#define AITARGET_LIST_LAST		9
#define AITARGET_RANGE			55
#define AITARGET_NEAR			35
#define ALIGN_HOSTILE			0
#define ALIGN_NEUTRAL			1
#define ALIGN_FRIENDLY			2

struct AIFINDMINION
{
 	PFNAIFIND	pfnFind;
 	GAME		*ptGame;
 	UNIT		*hUnit;
 	AIFINDRECORD	*pArg;
};
typedef struct AIFINDMINION AIFINDMINION;

static __inline int AiUtilHalfSpan(int nDx, int nDy)
{
	if (nDx > nDy)
		return (nDy + nDx * 2) / 2;
	return (nDx + nDy * 2) / 2;
}

static __inline int AiUtilSpan(int nDx, int nDy, UNIT *hSizeUnit)
{
	int nSize;

	if (nDx < 0)
		nDx = -nDx;
	if (nDy < 0)
		nDy = -nDy;
	nSize = (int)D2CommonUnitGetSizeX(hSizeUnit);
	nDx -= nSize;
	nDy -= nSize;
	if (nDx < 0)
		nDx = 0;
	if (nDy < 0)
		nDy = 0;
	return AiUtilHalfSpan(nDx, nDy);
}

static __inline DWORD AiUtilTargetSpan(UNIT *hTarget, int nX, int nY)
{
	int nDx;
	int nDy;

	nDx = (int)D2CommonUnitGetX(hTarget) - nX;
	nDy = (int)D2CommonUnitGetY(hTarget) - nY;
	if (nDx < 0)
		nDx = -nDx;
	if (nDy < 0)
		nDy = -nDy;
	if (nDx > nDy)
		return (DWORD)((nDy + nDx * 2) >> 1);
	return (DWORD)((nDx + nDy * 2) >> 2);
}

int __fastcall AiThinkDistance(UNIT *hUnit, UNIT *hTarget)
{
	int nDx;
	int nDy;
	int nSize;

	nDx = (int)D2CommonUnitGetX(hUnit);
	nDx -= (int)D2CommonUnitGetX(hTarget);
	nDy = (int)D2CommonUnitGetY(hUnit);
	nDy -= (int)D2CommonUnitGetY(hTarget);
	if (nDx < 0)
		nDx = -nDx;
	if (nDy < 0)
		nDy = -nDy;
	nSize = (int)D2CommonUnitGetSizeX(hUnit);
	nDx -= nSize;
	nDy -= nSize;
	if (nDx < 0)
		nDx = 0;
	if (nDy < 0)
		nDy = 0;
	return AiUtilHalfSpan(nDx, nDy);
}
int __fastcall AiThinkEdgeDistance(UNIT *hUnit, int nX, int nY)
{
	int nDx;
	int nDy;
	int nSize;

	nDx = (int)D2CommonUnitGetX(hUnit) - nX;
	if (nDx < 0)
		nDx = -nDx;
	nDy = (int)D2CommonUnitGetY(hUnit) - nY;
	if (nDy < 0)
		nDy = -nDy;
	nSize = (int)((D2CommonUnitGetSizeX(hUnit) >> 1) + 1);
	nDx -= nSize;
	nDy -= nSize;
	if (nDx < 0)
		nDx = 0;
	if (nDy < 0)
		nDy = 0;
	return AiUtilHalfSpan(nDx, nDy);
}
int __fastcall AiThinkPointDistance(UNIT *hUnit, int nX, int nY)
{
	void *ptPath = hUnit->ptPath;
	int nDx;
	int nDy;

	nDx = (int)D2CommonPathGetGameX(ptPath) - nX;
	if (nDx < 0)
		nDx = -nDx;
	nDy = (int)D2CommonPathGetGameY(ptPath) - nY;
	if (nDy < 0)
		nDy = -nDy;
	return AiUtilHalfSpan(nDx, nDy);
}

BOOL __fastcall AiThinkSees(UNIT *hUnit, UNIT *hTarget)
{
	int nDist;
	int nStep;
	int nTargetX;
	int nTargetY;
	int nDx;
	int nDy;

	if (!hUnit || !hTarget)
		return FALSE;
	{
	BYTE abStep[25] = { 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };
	nDist = AiThinkDistance(hUnit, hTarget);
	if (nDist >= 25)
		nStep = 3;
	else
		nStep = abStep[nDist];
	nTargetX = (int)D2CommonUnitGetX(hTarget);
	nTargetY = (int)D2CommonUnitGetY(hTarget);
	nDx = (int)D2CommonUnitGetX(hUnit) - nTargetX;
	nDy = (int)D2CommonUnitGetY(hUnit) - nTargetY;
	if (nDx < 0)
		nDx = -nStep;
	if (nDx > 0)
		nDx = nStep;
	if (nDy < 0)
		nDy = -nStep;
	if (nDy > 0)
		nDy = nStep;
	if (!D2CommonUnitTestLineBlockedToPoint(hUnit, nTargetX, nTargetY, 7169))
		return TRUE;
	if (!D2CommonUnitTestLineBlockedToPoint(hUnit, nTargetX - nDy, nTargetY + nDx, 7169))
		return TRUE;
	return !D2CommonUnitTestLineBlockedToPoint(hUnit, nTargetX + nDy, nTargetY - nDx, 7169);
	}
}

UNIT * __fastcall AiFindAllyFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind)
{
	int nX;
	int nY;
	int nDx;
	int nDy;
	int nDist;

	if (hCandidate->eType != UNIT_MONSTER || hUnit == hCandidate)
		return NULL;
	if ((hUnit->eType == UNIT_MONSTER || hUnit->eType == UNIT_PLAYER)
		&& !D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hCandidate))
		&& (hCandidate->dwFlags0 & 4)
		&& !SUnitIsDead(hCandidate)
		&& !SUnitIsDead(hUnit)
		&& SUnitIsHostile(ptGame, hUnit, hCandidate))
		return NULL;
	nX = (int)D2CommonUnitGetX(hUnit);
	nDx = nX - (int)D2CommonUnitGetX(hCandidate);
	nY = (int)D2CommonUnitGetY(hUnit);
	nDy = nY - (int)D2CommonUnitGetY(hCandidate);
	nDist = AiUtilSpan(nDx, nDy, hUnit);
	if (nDist < ptFind->tNearest.nMax)
	{
		ptFind->tNearest.nCount++;
		if (nDist < ptFind->tNearest.nDist)
		{
			ptFind->tNearest.nDist = nDist;
			ptFind->tNearest.hUnit = hCandidate;
		}
	}
	return NULL;
}

UNIT * __fastcall AiFindDyingFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind)
{
	int nX;
	int nY;
	int nDx;
	int nDy;
	int nDist;

	if (hCandidate->eType != UNIT_MONSTER)
		return NULL;
	if (hCandidate->dwMode != MONMODE_DEATH)
		return NULL;
	nX = (int)D2CommonUnitGetX(hUnit);
	nDx = (int)D2CommonUnitGetX(hCandidate) - nX;
	nY = (int)D2CommonUnitGetY(hUnit);
	nDy = (int)D2CommonUnitGetY(hCandidate) - nY;
	nDist = AiUtilSpan(nDx, nDy, hCandidate);
	if ((DWORD)nDist < ptFind->dwMax)
		return hCandidate;
	return NULL;
}

UNIT * __fastcall AiFindNearFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind)
{
	int nDx;
	int nDy;
	int nSize;
	int nDist;
	AIGENERAL *ptAi;
	DWORD dwLogic;

	if ((hCandidate->eType == UNIT_MONSTER || hCandidate->eType == UNIT_PLAYER)
		&& (hCandidate->dwFlags0 & 4)
		&& !SUnitIsDead(hCandidate)
		&& SUnitIsHostile(ptGame, hUnit, hCandidate))
	{
		nDx = (int)D2CommonUnitGetX(hCandidate) - (int)D2CommonUnitGetX(hUnit);
		nDy = (int)D2CommonUnitGetY(hCandidate) - (int)D2CommonUnitGetY(hUnit);
		if (nDx < 0)
			nDx = -nDx;
		if (nDy < 0)
			nDy = -nDy;
		nSize = (int)D2CommonUnitGetSizeX(hCandidate);
		nDx -= nSize;
		nDy -= nSize;
		if (nDx < 0)
			nDx = 0;
		if (nDy < 0)
			nDy = 0;
		if (nDx > nDy)
			nDist = (nDy + nDx * 2) / 2;
		else
			nDist = (nDx + nDy * 2) / 2;
		if (nDist > ptFind->tNear.nMax)
			return NULL;
		if (ptFind->tNear.bCheck && D2CommonUnitTestLineBlocked(hCandidate, hUnit, 4))
			return NULL;
		if (D2CommonStatsUnitGetAlignment(hCandidate) != ALIGN_NEUTRAL)
		{
			if (nDist < ptFind->tNear.nDist)
			{
				ptFind->tNear.hUnit = hCandidate;
				ptFind->tNear.nDist = nDist;
			}
		}
		else if (nDist < ptFind->tNear.nDist2)
		{
			ptFind->tNear.hUnit2 = hCandidate;
			ptFind->tNear.nDist2 = nDist;
		}
		return NULL;
	}
	if (hCandidate->eType == UNIT_MONSTER && hUnit->eType != UNIT_PLAYER
		&& D2CommonStatsUnitGetAlignment(hUnit) == ALIGN_FRIENDLY && ptFind->tNear.bCheck
		&& !D2CommonMonsterUnitTestFieldC(hCandidate))
	{
		ptAi = MONDATA(hCandidate)->ptAi;
		if (ptAi->wFlags & AIFLAG_08)
		{
			dwLogic = D2CommonDungeonGetLogicValue(D2CommonUnitGetPathRoom(hCandidate),
				(int)D2CommonUnitGetX(hCandidate), (int)D2CommonUnitGetY(hCandidate));
			if (ptFind->tNear.nLogic == dwLogic)
			{
				ptAi->wFlags |= AIFLAG_08;
				ptFind->tNear.bCheck = FALSE;
			}
		}
	}
	return NULL;
}

UNIT * __fastcall AiFindHostileFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind)
{
	int nX;
	int nY;
	int nDx;
	int nDy;
	int nDist;
	DWORD eCandType = hCandidate->eType;

	if ((hUnit->eType == UNIT_MONSTER || hUnit->eType == UNIT_PLAYER)
		&& (eCandType == UNIT_MONSTER || eCandType == UNIT_PLAYER)
		&& !D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hCandidate))
		&& (hCandidate->dwFlags0 & 4)
		&& !SUnitIsDead(hCandidate)
		&& !SUnitIsDead(hUnit)
		&& SUnitIsHostile(ptGame, hUnit, hCandidate)
		&& !D2CommonUnitTestLineBlocked(hCandidate, hUnit, 4))
	{
		nX = (int)D2CommonUnitGetX(hUnit);
		nDx = (int)D2CommonUnitGetX(hCandidate) - nX;
		nY = (int)D2CommonUnitGetY(hUnit);
		nDy = (int)D2CommonUnitGetY(hCandidate) - nY;
		nDist = AiUtilSpan(nDx, nDy, hCandidate);
		if (D2CommonStatsUnitGetAlignment(hCandidate) != ALIGN_NEUTRAL)
		{
			if (nDist < ptFind->tPair.nDist)
			{
				ptFind->tPair.nDist = nDist;
				ptFind->tPair.hUnit = hCandidate;
			}
		}
		else if (nDist < ptFind->tPair.nDist2)
		{
			ptFind->tPair.hUnit2 = hCandidate;
			ptFind->tPair.nDist2 = nDist;
		}
	}
	return NULL;
}

UNIT * __fastcall AiFindHostileButFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind)
{
	DWORD eCandType;
	BOOL bHostile;
	int nDx;
	int nDy;
	int nSize;
	int nDist;

	eCandType = hCandidate->eType;
	if (hUnit->eType != UNIT_MONSTER && hUnit->eType != UNIT_PLAYER)
		bHostile = FALSE;
	else if (eCandType != UNIT_MONSTER && eCandType != UNIT_PLAYER)
		bHostile = FALSE;
	else if (D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hCandidate)))
		bHostile = FALSE;
	else if (!(hCandidate->dwFlags0 & 4))
		bHostile = FALSE;
	else if (SUnitIsDead(hCandidate))
		bHostile = FALSE;
	else if (SUnitIsDead(hUnit))
		bHostile = FALSE;
	else
		bHostile = SUnitIsHostile(ptGame, hUnit, hCandidate);
	if (hCandidate != ptFind->tExcl.hExclude && bHostile
		&& !D2CommonUnitTestLineBlocked(hCandidate, hUnit, 4))
	{
		nDx = (int)D2CommonUnitGetX(hCandidate) - (int)D2CommonUnitGetX(hUnit);
		nDy = (int)D2CommonUnitGetY(hCandidate) - (int)D2CommonUnitGetY(hUnit);
		if (nDx < 0)
			nDx = -nDx;
		if (nDy < 0)
			nDy = -nDy;
		nSize = (int)D2CommonUnitGetSizeX(hCandidate);
		nDx -= nSize;
		nDy -= nSize;
		if (nDx < 0)
			nDx = 0;
		if (nDy < 0)
			nDy = 0;
		if (nDx > nDy)
			nDist = (nDy + nDx * 2) / 2;
		else
			nDist = (nDx + nDy * 2) / 2;
		if (D2CommonStatsUnitGetAlignment(hCandidate) != ALIGN_NEUTRAL && nDist < ptFind->tExcl.nDist)
		{
			ptFind->tExcl.hUnit = hCandidate;
			ptFind->tExcl.nDist = nDist;
		}
	}
	return NULL;
}

UNIT * __fastcall AiFindObjectFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind)
{
	int nX;
	int nY;
	int nDx;
	int nDy;
	int nDist;

	if (hCandidate->eType == UNIT_OBJECT && D2CommonUnitIsDoorRaw((DWORD)hCandidate) && !hCandidate->dwMode)
	{
		nX = (int)D2CommonUnitGetX(hUnit);
		nDx = (int)D2CommonUnitGetX(hCandidate) - nX;
		nY = (int)D2CommonUnitGetY(hUnit);
		nDy = (int)D2CommonUnitGetY(hCandidate) - nY;
		nDist = AiUtilSpan(nDx, nDy, hCandidate);
		if (nDist < ptFind->tUnit.nDist)
		{
			ptFind->tUnit.hUnit = hCandidate;
			ptFind->tUnit.nDist = nDist;
		}
	}
	return NULL;
}

UNIT * __fastcall AiFindDeadMinionFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind)
{
	int nX;
	int nY;
	int nDx;
	int nDy;
	int nDist;

	nX = (int)D2CommonUnitGetX(hUnit);
	nDx = nX - (int)D2CommonUnitGetX(hCandidate);
	nY = (int)D2CommonUnitGetY(hUnit);
	nDy = nY - (int)D2CommonUnitGetY(hCandidate);
	nDist = AiUtilSpan(nDx, nDy, hUnit);
	if (nDist <= ptFind->tDead.nMax && hCandidate->dwMode == MONMODE_DEAD && AiGetOwner(hCandidate) == hUnit
		&& (hCandidate->dwFlags0 & 2) && !D2CommonStatsUnitTestStateGroupC(hCandidate))
	{
		ptFind->tDead.hUnit = hCandidate;
		ptFind->tDead.nCount++;
		ptFind->tDead.nDist = nDist;
	}
	return NULL;
}

UNIT * __fastcall AiFindHostileSeenFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind)
{
	BOOL bHostile;
	int nDx;
	int nDy;
	int nSize;
	int nDist;
	DWORD eCandType = hCandidate->eType;

	if (eCandType != UNIT_MONSTER && eCandType != UNIT_PLAYER)
		bHostile = FALSE;
	else if (!(hCandidate->dwFlags0 & 4))
		bHostile = FALSE;
	else if (SUnitIsDead(hCandidate))
		bHostile = FALSE;
	else
		bHostile = SUnitIsHostile(ptGame, hUnit, hCandidate);
	if (hCandidate != ptFind->tExcl.hExclude && bHostile
		&& D2CommonStatsUnitGetAlignment(hCandidate) != ALIGN_NEUTRAL && AiThinkSees(hUnit, hCandidate))
	{
		nDx = (int)D2CommonUnitGetX(hCandidate) - (int)D2CommonUnitGetX(hUnit);
		nDy = (int)D2CommonUnitGetY(hCandidate) - (int)D2CommonUnitGetY(hUnit);
		if (nDx < 0)
			nDx = -nDx;
		if (nDy < 0)
			nDy = -nDy;
		nSize = (int)D2CommonUnitGetSizeX(hCandidate);
		nDx -= nSize;
		nDy -= nSize;
		if (nDx < 0)
			nDx = 0;
		if (nDy < 0)
			nDy = 0;
		if (nDx > nDy)
			nDist = (nDy + nDx * 2) / 2;
		else
			nDist = (nDx + nDy * 2) / 2;
		if (nDist < ptFind->tExcl.nDist)
		{
			ptFind->tExcl.hUnit = hCandidate;
			ptFind->tExcl.nDist = nDist;
		}
	}
	return NULL;
}

UNIT * __fastcall AiCountClassFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind)
{
	int nX;
	int nY;
	int nDx;
	int nDy;
	int nDist;

	if (hCandidate->eType == UNIT_MONSTER && hCandidate->dwClassId == ptFind->tClass.dwClassId)
	{
		nX = (int)D2CommonUnitGetX(hUnit);
		nDx = nX - (int)D2CommonUnitGetX(hCandidate);
		nY = (int)D2CommonUnitGetY(hUnit);
		nDy = nY - (int)D2CommonUnitGetY(hCandidate);
		nDist = AiUtilSpan(nDx, nDy, hUnit);
		if (nDist <= ptFind->tClass.nMax && !D2CommonMonsterUnitTestFieldC(hCandidate))
			ptFind->tClass.nCount++;
	}
	return NULL;
}

static const AIFINDKIND sgatAiFindKinds[AIFIND_KINDS] =
{
	{ 0, NULL },
	{ 0, NULL },
	{ 1, NULL },
	{ 0, AiFindAllyFn },
	{ 0, AiFindDyingFn },
	{ 2, AiFindNearFn },
	{ 0, AiFindHostileFn },
	{ 0, AiFindHostileButFn },
	{ 0, AiFindObjectFn },
	{ 3, AiFindDeadMinionFn },
	{ 3, NULL },
	{ 2, AiFindHostileSeenFn },
	{ 0, AiCountClassFn },
};

extern UNIT * AiFindInRooms(GAME *ptGame, UNIT *hUnit, AIFINDRECORD *pArg, PFNAIFIND pfnFind);
__inline UNIT * AiFindInRooms(GAME *ptGame, UNIT *hUnit, AIFINDRECORD *pArg, PFNAIFIND pfnFind)
{
	GAMEROOM **pptRooms;
	DWORD dwRooms;
	DWORD i;
	UNIT *hCandidate;
	UNIT *hFound;

#line 728
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)pfnFind), "pfnFind");
	D2CommonDungeonRoomGetNear(D2CommonUnitGetPathRoom(hUnit), &pptRooms, &dwRooms);
	for (i = 0; i < dwRooms; i++)
	{
		for (hCandidate = pptRooms[i]->ptUnitFirst; hCandidate; hCandidate = hCandidate->ptRoomNext)
		{
			hFound = pfnFind(ptGame, hUnit, hCandidate, pArg);
			if (hFound)
				return hFound;
		}
	}
	return NULL;
}

static __inline UNIT * AiFindPlayersInView(GAME *ptGame, UNIT *hUnit, AIFINDRECORD *pArg, PFNAIFIND pfnFind)
{
	GAMEROOM **pptRooms;
	DWORD dwRooms;
	DWORD i;
	DWORD dwClients;
	struct CLIENT **phClients;
	UNIT *hPlayer;
	UNIT *hFound;

#line 755
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)pfnFind), "pfnFind");
	D2CommonDungeonRoomGetNear(D2CommonUnitGetPathRoom(hUnit), &pptRooms, &dwRooms);
	for (i = 0; i < dwRooms; i++)
	{
		dwClients = (*pptRooms)->nNumClients;
		phClients = (*pptRooms)->phClients;
		for (; dwClients; dwClients--)
		{
			hPlayer = ClientGetPlayerUnit(*phClients, FALSE);
			if (hPlayer)
			{
				hFound = pfnFind(ptGame, hUnit, hPlayer, pArg);
				if (hFound)
					return hFound;
			}
			phClients++;
		}
		pptRooms++;
	}
	return NULL;
}

static __inline UNIT * AiFindInViewedRooms(GAME *ptGame, UNIT *hUnit, AIFINDRECORD *pArg, PFNAIFIND pfnFind)
{
	GAMEROOM **pptRooms;
	DWORD dwRooms;
	DWORD i;
	UNIT *hCandidate;
	UNIT *hFound;

#line 793
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)pfnFind), "pfnFind");
	if (hUnit->eType != UNIT_MONSTER && hUnit->eType != UNIT_PLAYER)
		return NULL;
	if (SUnitIsDead(hUnit))
		return NULL;
	D2CommonDungeonRoomGetNear(D2CommonUnitGetPathRoom(hUnit), &pptRooms, &dwRooms);
	for (i = 0; i < dwRooms; pptRooms++, i++)
	{
		if (!(*pptRooms)->nNumClients || D2CommonDungeonRoomIsTown(*pptRooms))
			continue;
		for (hCandidate = (*pptRooms)->ptUnitFirst; hCandidate; hCandidate = hCandidate->ptRoomNext)
		{
			hFound = pfnFind(ptGame, hUnit, hCandidate, pArg);
			if (hFound)
				return hFound;
		}
	}
	return NULL;
}

static __inline UNIT * AiFindMinions(GAME *ptGame, UNIT *hUnit, AIFINDRECORD *pArg, PFNAIFIND pfnFind)
{
	AIFINDMINION tFind;
	UNIT *hOwner;
	UNIT *hFound;

#line 852
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)pfnFind), "pfnFind");
	hOwner = AiGetOwner(hUnit);
	if (!hOwner || hOwner->eType != UNIT_MONSTER)
		return NULL;
	tFind.ptGame = ptGame;
	tFind.pArg = pArg;
	hFound = NULL;
	tFind.pfnFind = pfnFind;
	tFind.hUnit = hUnit;
	AiForEachMinion(hOwner, (DWORD)&tFind, (DWORD)&hFound, AiFindMinionFn);
	return hFound;
}

UNIT * __fastcall AiFind(GAME *ptGame, UNIT *hUnit, AIFINDRECORD *pArg, PFNAIFIND pfnFind, int nKind)
{
	int nSearch;

	if (nKind != 1 && nKind != 10 && nKind != 2)
		pfnFind = sgatAiFindKinds[nKind].pfnFind;
	nSearch = sgatAiFindKinds[nKind].nSearch;
#line 936
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)pfnFind), "pfnFind");
	if (nSearch == 1)
		return AiFindPlayersInView(ptGame, hUnit, pArg, pfnFind);
	if (nSearch == 2)
		return AiFindInViewedRooms(ptGame, hUnit, pArg, pfnFind);
	if (nSearch == 3)
		return AiFindMinions(ptGame, hUnit, pArg, pfnFind);
	return AiFindInRooms(ptGame, hUnit, pArg, pfnFind);
}

void __fastcall AiFindMinionFn(UNIT *hMinion, DWORD dwFind, DWORD dwResult)
{
	AIFINDMINION *ptFind;
	UNIT **phResult;
	UNIT *hFound;
	GAME *ptGame;
	UNIT *hUnit;
	PFNAIFIND pfnFind;

	ptFind = (AIFINDMINION *)dwFind;
	phResult = (UNIT **)dwResult;
	hFound = *phResult;
	if (hFound)
		return;
	ptGame = ptFind->ptGame;
	hUnit = ptFind->hUnit;
	pfnFind = ptFind->pfnFind;
	hFound = pfnFind(ptGame, hUnit, hMinion, ptFind->pArg);
	if (hFound)
		*phResult = hFound;
}

UNIT * __fastcall AiFindTarget(GAME *ptGame, UNIT *hUnit, AIGENERAL *ptAi, int *pnDist,
	DWORD *pdwDir)
{
	AIFINDRECORD tFind;
	TARGET **pptList;
	TARGET *ptTargetNode;
	DRLGLOGICNODE *ptArea;
	UNIT *hTarget;
	UNIT *hBest;
	UNIT *hLast;
	DWORD dwRoom;
	DWORD dwDist;
	DWORD dwBest;
	DWORD dwLast;
	DWORD dwNearest;
	BOOL bAreaFlag;
	BOOL bCheck;
	BOOL bForce;
	BYTE bAct;
	int nLogic;
	int nX;
	int nY;
	int i;

	dwRoom = D2CommonUnitGetPathRoom(hUnit);
	bAreaFlag = FALSE;
	ptArea = NULL;
	bForce = AiRecordTestFlags((AIRECORDHEAD *)ptAi, AIFLAG_40);
	if (bForce)
		bCheck = TRUE;
	else if (D2CommonDungeonRoomGetKindFlag(dwRoom))
		bCheck = FALSE;
	else
	{
		bCheck = !AiRecordTestFlags((AIRECORDHEAD *)ptAi, AIFLAG_08);
		if (hUnit->eType == UNIT_MONSTER)
		{
			ptArea = (DRLGLOGICNODE *)MONDATA(hUnit)->ptArea;
			if (bCheck && ptArea)
			{
				bAreaFlag = ptArea->fSkipTargetCheck;
				bCheck = !bAreaFlag;
			}
		}
	}
	if (bForce)
		AiRecordSetFlags((AIRECORDHEAD *)ptAi, AIFLAG_40, FALSE);
	dwNearest = AIFIND_NONE;
	if (!AiFindCommandTarget(ptGame, hUnit, bCheck, FALSE, &hBest, (int *)&dwBest))
	{
		if (D2CommonStatsUnitGetAlignment(hUnit) == ALIGN_FRIENDLY)
		{
			pptList = ptGame->aptTargetList;
			ptTargetNode = *pptList;
			hBest = NULL;
			dwBest = AITARGET_NEAR;
			nX = (int)D2CommonUnitGetX(hUnit);
			nY = (int)D2CommonUnitGetY(hUnit);
			bAct = hUnit->bAct;
			for (i = 0; i < AITARGET_PLAYER_LISTS; i++)
			{
				if (ptTargetNode)
				{
					hTarget = ptTargetNode->hUnit;
#line 1018
					D2ASSERT(UnitGetType (ptTargetNode->hUnit) == UNIT_PLAYER);
					if (bAct == hTarget->bAct)
					{
						dwDist = AiUtilTargetSpan(hTarget, nX, nY);
						if (dwDist < dwNearest)
							dwNearest = dwDist;
						if (dwDist < AITARGET_RANGE)
						{
							if (hTarget->dwMode == PLRMODE_DEAD || hTarget->dwMode == PLRMODE_DEATH)
								dwDist = AIFIND_NONE;
							for (;;)
							{
								if (dwDist < dwBest
									&& (!bCheck || !D2CommonUnitTestLineBlocked(hUnit, hTarget, 4)))
								{
									dwBest = dwDist;
									hBest = hTarget;
								}
								ptTargetNode = ptTargetNode->ptNext;
								if (!ptTargetNode)
									break;
								hTarget = ptTargetNode->hUnit;
								dwDist = AiUtilTargetSpan(hTarget, nX, nY);
							}
						}
					}
				}
				ptTargetNode = *++pptList;
			}
			while (ptTargetNode)
			{
				hTarget = ptTargetNode->hUnit;
				if (bAct == hTarget->bAct)
				{
					dwDist = AiUtilTargetSpan(hTarget, nX, nY);
					if (dwDist < dwBest && (!bCheck || !D2CommonUnitTestLineBlocked(hUnit, hTarget, 4)))
					{
						dwBest = dwDist;
						hBest = hTarget;
					}
				}
				ptTargetNode = ptTargetNode->ptNext;
			}
			ptTargetNode = pptList[1];
			hLast = NULL;
			dwLast = AIFIND_NONE;
			while (ptTargetNode)
			{
				hTarget = ptTargetNode->hUnit;
				if (bAct == hTarget->bAct)
				{
					dwDist = AiUtilTargetSpan(hTarget, nX, nY);
					if (dwDist < dwLast && (!bCheck || !D2CommonUnitTestLineBlocked(hUnit, hTarget, 4)))
					{
						dwLast = dwDist;
						hLast = hTarget;
					}
				}
				ptTargetNode = ptTargetNode->ptNext;
			}
			if (hLast && AiPreferTarget(hUnit, &hBest, (int *)&dwBest, hLast, (int)dwLast))
			{
				hBest = hLast;
				dwBest = dwLast;
			}
		}
		else
		{
			nLogic = D2CommonDungeonGetLogicValue(dwRoom, (int)D2CommonUnitGetX(hUnit),
				(int)D2CommonUnitGetY(hUnit));
			tFind.tNear.hUnit = NULL;
			tFind.tNear.nDist = AIFIND_NONE;
			tFind.tNear.bCheck = bCheck;
			tFind.tNear.nMax = AITARGET_NEAR;
			tFind.tNear.nLogic = nLogic;
			tFind.tNear.hUnit2 = NULL;
			tFind.tNear.nDist2 = AIFIND_NONE;
			AiFind(ptGame, hUnit, &tFind, NULL, 5);
			if (tFind.tNear.hUnit2 && AiPreferTarget(hUnit, &tFind.tNear.hUnit, &tFind.tNear.nDist, tFind.tNear.hUnit2, tFind.tNear.nDist2))
			{
				hBest = tFind.tNear.hUnit2;
				dwBest = (DWORD)tFind.tNear.nDist2;
			}
			else
			{
				hBest = tFind.tNear.hUnit;
				dwBest = (DWORD)tFind.tNear.nDist;
			}
		}
	}
	if (!hBest)
	{
		*pdwDir = 0;
		*pnDist = (int)dwNearest;
		return NULL;
	}
	if (D2CommonStatsUnitGetAlignment(hUnit) != ALIGN_HOSTILE)
	{
		AiRecordSetFlags((AIRECORDHEAD *)ptAi, AIFLAG_08, TRUE);
		if (ptArea)
			ptArea->fSkipTargetCheck = !bAreaFlag;
	}
	*pdwDir = D2CommonUnitTestPositionClear(hUnit, hBest, 0);
	*pnDist = (int)dwBest;
	return hBest;
}

BOOL __fastcall AiPreferTarget(UNIT *hUnit, UNIT **phBest, int *pnDist, UNIT *hOther,
	int nOtherDist)
{
	AIFINDRECORD tFind;
	UNIT *hBest;
	void *ptPath;
	DWORD dwType;
	DWORD dwTarget;
	DWORD dwX;
	DWORD dwY;

	if (!hOther || hUnit->eType == UNIT_PLAYER)
		return FALSE;
	hBest = *phBest;
	if (!hBest)
		return TRUE;
	if (nOtherDist > 5)
		return FALSE;
	ptPath = hUnit->ptPath;
	dwType = D2CommonPathGetType(ptPath);
	dwTarget = D2CommonPathGetTarget(ptPath);
	dwX = D2CommonPathGetTargetXRaw((DWORD)ptPath);
	dwY = D2CommonPathGetTargetYRaw((DWORD)ptPath);
	D2CommonPathSetTarget(ptPath, hBest);
	D2CommonPathSetType(ptPath, 2);
	D2CommonPathCompute(ptPath, hUnit);
	D2CommonPathSetTarget(ptPath, dwTarget);
	D2CommonPathSetTargetCoord(ptPath, dwX, dwY);
	D2CommonPathSetType(ptPath, dwType);
	if (D2CommonPathGetStepCount(ptPath))
		return FALSE;
	tFind.tExcl.hUnit = NULL;
	tFind.tExcl.nDist = AIFIND_NONE;
	tFind.tExcl.hExclude = hBest;
	AiFind(SUnitGetGame(hUnit), hUnit, &tFind, NULL, 7);
	if (!tFind.tExcl.hUnit || tFind.tExcl.nDist >= 20)
		return TRUE;
	*phBest = tFind.tExcl.hUnit;
	*pnDist = tFind.tExcl.nDist;
	return FALSE;
}

BOOL __fastcall AiFindCommandTarget(GAME *ptGame, UNIT *hUnit, BOOL bCheck,
	BOOL bSight, UNIT **phTarget, int *pnDist)
{
	UNIT *hTarget;
	DWORD eType;
	DWORD dwId;
	DWORD dwAlign;
	int nCmd;
	int nDist;
	DWORD dwMode;
	int nLogic;
	AIFINDRECORD tFind;

	if (hUnit->eType != UNIT_MONSTER)
		return FALSE;
	nCmd = MonsterAIGetTargetCmd(hUnit);
	if (!nCmd)
		return FALSE;
	switch (nCmd)
	{
	case 1:
		dwId = MonsterAIGetTargetCmdArg(hUnit);
		hTarget = SUnitFind(ptGame, UNIT_PLAYER, dwId);
		if (!hTarget)
			break;
		nDist = AiThinkDistance(hTarget, hUnit);
		if (bSight && D2CommonUnitTestLineBlocked(hTarget, hUnit, 4))
			hTarget = NULL;
		break;
	case 2:
		dwId = MonsterAIGetTargetCmdArg(hUnit);
		hTarget = SUnitFind(ptGame, UNIT_MONSTER, dwId);
		if (!hTarget)
			break;
		nDist = AiThinkDistance(hTarget, hUnit);
		if (bSight && D2CommonUnitTestLineBlocked(hTarget, hUnit, 4))
			hTarget = NULL;
		break;
	case 4:
		dwId = MonsterAIGetTargetCmdArg(hUnit);
		hTarget = SUnitFind(ptGame, UNIT_MISSILE, dwId);
		if (!hTarget)
			break;
		nDist = AiThinkDistance(hTarget, hUnit);
		if (bSight && D2CommonUnitTestLineBlocked(hTarget, hUnit, 4))
			hTarget = NULL;
		break;
	case 3:
		dwAlign = D2CommonStatsUnitGetAlignment(hUnit);
		if (D2CommonSeedNext(&hUnit->tSeed) & 1)
		{
			if (dwAlign == ALIGN_FRIENDLY)
				SUnitSetAlignment(hUnit, ALIGN_HOSTILE);
			else if (dwAlign == ALIGN_HOSTILE)
				SUnitSetAlignment(hUnit, ALIGN_FRIENDLY);
		}
		tFind.tNear.hUnit = NULL;
		tFind.tNear.nDist = AIFIND_NONE;
		if (!bSight)
		{
			nLogic = D2CommonDungeonGetLogicValue(D2CommonUnitGetPathRoom(hUnit),
				(int)D2CommonUnitGetX(hUnit), (int)D2CommonUnitGetY(hUnit));
			tFind.tNear.nMax = AITARGET_NEAR;
			tFind.tNear.bCheck = bCheck;
			tFind.tNear.nLogic = nLogic;
			tFind.tNear.hUnit2 = NULL;
			tFind.tNear.nDist2 = AIFIND_NONE;
			AiFind(ptGame, hUnit, &tFind, NULL, 5);
		}
		else
		{
			tFind.tPair.hUnit2 = NULL;
			tFind.tPair.nDist2 = AIFIND_NONE;
			AiFind(ptGame, hUnit, &tFind, NULL, 6);
		}
		hTarget = tFind.tNear.hUnit;
		nDist = tFind.tNear.nDist;
		SUnitSetAlignment(hUnit, (BYTE)dwAlign);
		break;
	default:
		hTarget = NULL;
		break;
	}
	if (hTarget)
	{
		dwMode = hTarget->dwMode;
		switch (hTarget->eType)
		{
		case UNIT_MONSTER:
			if (dwMode == MONMODE_DEATH)
				break;
			if (dwMode == MONMODE_DEAD)
			{
				MonsterAIClearTargetCmd(hUnit);
				return FALSE;
			}
			goto lFound;
		case UNIT_PLAYER:
			if (dwMode == PLRMODE_DEATH || dwMode == PLRMODE_DEAD)
				break;
		case UNIT_MISSILE:
		lFound:
			*phTarget = hTarget;
			*pnDist = nDist;
			return TRUE;
		}
	}
	MonsterAIClearTargetCmd(hUnit);
	return FALSE;
}

UNIT * __fastcall AiFindTargetInSight(GAME *ptGame, UNIT *hUnit, int *pnDist,
	DWORD *pdwDir)
{
	AIFINDRECORD tFind;
	UNIT *hBest;
	int nDist;

	if (!AiFindCommandTarget(ptGame, hUnit, FALSE, TRUE, &hBest, &nDist))
	{
		tFind.tPair.hUnit = NULL;
		tFind.tPair.nDist = AIFIND_NONE;
		tFind.tPair.hUnit2 = NULL;
		tFind.tPair.nDist2 = AIFIND_NONE;
		AiFind(ptGame, hUnit, &tFind, NULL, 6);
		if (AiPreferTarget(hUnit, &tFind.tPair.hUnit, &tFind.tPair.nDist, tFind.tPair.hUnit2, tFind.tPair.nDist2))
		{
			hBest = tFind.tPair.hUnit2;
			nDist = tFind.tPair.nDist2;
		}
		else
		{
			hBest = tFind.tPair.hUnit;
			nDist = tFind.tPair.nDist;
		}
	}
	if (!hBest)
	{
		*pdwDir = 0;
		*pnDist = AIFIND_NONE;
		return NULL;
	}
	*pdwDir = D2CommonUnitTestPositionClear(hUnit, hBest, 0);
	*pnDist = nDist;
	return hBest;
}

BOOL __fastcall AiRecordTestFlags(AIRECORDHEAD *ptRecord, WORD wMask)
{
	if ((WORD)(ptRecord->wFlags & wMask))
		return TRUE;
	return FALSE;
}

void __fastcall AiRecordSetFlags(AIRECORDHEAD *ptRecord, WORD wMask,
	BOOL bSet)
{
	if (bSet)
		ptRecord->wFlags |= wMask;
	else
		ptRecord->wFlags &= ~wMask;
}
