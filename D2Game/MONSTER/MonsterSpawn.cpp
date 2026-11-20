#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\MONSTER\\MonsterSpawn.cpp"

#include "../D2game.h"
#include "D2Macros.h"
#include "D2CMPImports.h"
#include "../GAME/Clients.h"
#include "../GAME/SCmd.h"
#include "../GAME/Targets.h"
#include "Monster.h"
#include "MonsterAI.h"
#include "MonsterEval.h"
#include "MonsterMsg.h"
#include "MonsterUnique.h"
#include "../QUESTS/QUESTS.h"
#include "../QUESTS/a4q2.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "../AI/AiGeneral.h"
#include "../GUILDS/Guilds.h"
#include "MonsterSpawn.h"
#include "FogImports.h"

struct DRLGTILE
{
 	BYTE		unobserved_00[8];
 	int			nX;
 	int			nY;
 	BYTE		unobserved_10[24 - 16];
 	void		*pTile;
 	BYTE		unobserved_1c[48 - 28];
};
typedef struct DRLGTILE DRLGTILE;

struct LEVELWARPS
{
 	int			anX[9];
 	int			anY[9];
 	int			nCount;
};
typedef struct LEVELWARPS LEVELWARPS;

struct LEVELSTXT
{
 	BYTE		unobserved_00[2];
 	BYTE		bAct;
 	BYTE		unobserved_03[8 - 3];
 	int			nWarpDist;
 	BYTE		unobserved_0c[16 - 12];
 	int			nMonDen;
 	BYTE		unobserved_14[22 - 20];
 	BYTE		bMonWndr;
};
typedef struct LEVELSTXT LEVELSTXT;

struct UNIQUEOFFSET
{
	int			nX;
	int			nY;
};
static const struct UNIQUEOFFSET sgatUniqueGroupOffsets[2][6] =
{
	{ { -1, -4 }, { 1, 4 }, { 1, -3 }, { -1, 3 }, { 0, 2 }, { 0, -2 } },
	{ { -3, -1 }, { 3, 1 }, { 2, -1 }, { -2, 1 }, { 1, 0 }, { -1, 0 } },
};

static const POINT sgatSpawnOffsets[4] =
{
	{ 0, -3 }, { 3, 0 }, { 0, 3 }, { -3, 0 },
};

static int sgnCritterClass[1] =
{
	270,
};
struct CRITTERRUN
{
 	BYTE		bFirst;
 	BYTE		bCount;
};
static struct CRITTERRUN sgatCritterRun[NUM_ACTS] =
{
	{ 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
};

static FORMATIONPOINT sgatUniqueFormation154[5] =
{
	{ 4, 0, 1 }, { 1, 1, 1 }, { 1, -1, 3 }, { -1, -1, 3 }, { -1, 1, 3 },
};

BOOL __fastcall MonsterFindSpawnPoint(GAME *ptGame, GAMEROOM *hRoom, DRLGLOGICNODE *ptArea,
	int nClass, int *pnX, int *pnY,
	BOOL bAwayFromWarps)
{
	RECT atRect[2];
	D2SEED *ptSeed;
	LEVELWARPS *ptWarps;
	BOOL bArea;
	int nLogic;
	int nX0;
	int nY0;
	int nWidth;
	int nHeight;
	int nTry;
	int nX;
	int nY;
	int nLevelId;
	int nWarpDist;
	LEVELSTXT *ptLevel;
	int nDX;
	int nDY;
	int nWarpX;
	int nWarpY;
	int i;
	BYTE bAct;

	if (!ptArea)
	{
		D2CommonDungeonRoomCopyRects(hRoom, atRect);
		nX0 = atRect[0].left + 1;
		nY0 = atRect[0].top + 1;
		nWidth = atRect[0].right - 1;
		nHeight = atRect[0].bottom - 1;
		nLogic = 0;
		bArea = FALSE;
	}
	else
	{
		CopyRect(&atRect[0], &ptArea->tRect);
		D2CommonEnvTileToSubtile(&atRect[0].left, &atRect[0].top);
		D2CommonEnvTileToSubtile(&atRect[0].right, &atRect[0].bottom);
		bArea = TRUE;
		nX0 = atRect[0].left + 1;
		nY0 = atRect[0].top + 1;
		nWidth = atRect[0].right - nX0;
		nHeight = atRect[0].bottom - nY0;
		nLogic = ptArea->nLogic;
	}
	if (hRoom)
		ptSeed = &ROOM5(hRoom)->tSeed;
	else
		ptSeed = NULL;
	for (nTry = 0; nTry < 20; nTry++)
	{
		nX = (int)D2CommonSeedRand(ptSeed, nWidth) + nX0;
		nY = (int)D2CommonSeedRand(ptSeed, nHeight) + nY0;
		if (bAwayFromWarps && hRoom)
		{
			nLevelId = (int)D2CommonDungeonRoomGetLevelId(hRoom);
			if (nLevelId)
			{
				ptWarps = (LEVELWARPS *)D2CommonDungeonRoomGetLevelWarpX(hRoom);
				if (ptWarps)
				{
					ptLevel = (LEVELSTXT *)D2CommonLvlTblsGetLevelData(nLevelId);
					nWarpDist = ptLevel->nWarpDist;
					for (i = 0; i < ptWarps->nCount; i++)
					{
						nDX = nX - ptWarps->anX[i];
						nDY = nY - ptWarps->anY[i];
						if (nDY * nDY + nDX * nDX < nWarpDist)
							goto lNext;
					}

					(void)D2CommonDungeonGetWarpRoom(ptGame->ptAct[D2CommonDrlgGetActNum(0, nLevelId)], nLevelId, 11, &nWarpX,
						&nWarpY);
					if (nWarpX > 0 && nWarpY > 0)
					{
						D2CommonEnvTileToSubtile(&nWarpX, &nWarpY);
						nDX = nX - nWarpX;
						nDY = nY - nWarpY;
						if (nDY * nDY + nDX * nDX < nWarpDist)
							continue;
					}
				}
			}
		}
		if (bArea)
		{
			if (D2CommonDungeonGetLogicValue(hRoom, nX, nY) != nLogic)
				continue;
			if (MonsterSpawnInArea(ptGame, hRoom, ptArea, nX, nY, nClass, 1, -1, 1))
				break;
		}
		else if (MonsterSpawn(ptGame, hRoom, nX, nY, nClass, 1, -1, 1))
			break;
	lNext:;
	}
	if (nTry >= 20)
		return FALSE;
	*pnX = nX;
	*pnY = nY;
	return TRUE;
}

UNIT * __fastcall MonsterSpawnClass(GAME *ptGame, GAMEROOM *hRoom, int nClass,
	int nX, int nY, DWORD dwArg)
{
	UNIT *hMonster;

	hMonster = NULL;
	if (nClass >= 438 && nClass <= 479)
		return MonsterUniqueSpawnSuper(ptGame, hRoom, nX, nY, 3, nClass - 438);
	switch (nClass)
	{
	case 420:
	case 421:
	{
		DWORD dwFlags;
		int nLevelId;
		int nOffset;

		nLevelId = (int)D2CommonDungeonRoomGetLevelId(hRoom);
		dwFlags = 0;
		if (nClass == 420)
			dwFlags = 4;
		nOffset = 0;
		switch (nLevelId)
		{
		case 76:
		case 77:
		case 78:
			nOffset = 1;
			break;
		case 92:
		case 93:
			nOffset = 2;
			break;
		case 86:
		case 87:
		case 88:
		case 89:
		case 90:
		case 91:
			nOffset = 0;
			break;
		}
		hMonster = MonsterSpawn(ptGame, hRoom, nX, nY, 261 + nOffset, dwArg, -1, dwFlags);
		break;
	}
	case 415:
		hMonster = MonsterSpawn(ptGame, hRoom, nX, nY, 267, dwArg, -1, 0);
		break;
	case 412:
	{
		MONSTATSTXT *ptStats;
		MONREGION *ptRegion;
		int nPicked;

		ptRegion = (MONREGION *)D2CommonMonsterRegionArrayGetDword(&ptGame->pMonRegions,
			D2CommonDungeonRoomGetLevelIdChecked(hRoom));
		nPicked = MonsterRegionPickClass(ptRegion, hRoom, &ptStats, 0, 1);
		if (!ptStats)
			return NULL;
		MonsterUniqueSpawnBoss(ptGame, hRoom, NULL, nPicked, 0);
		break;
	}
	case 413:
	{
		MONSTATSTXT *ptStats;
		MONREGION *ptRegion;
		UNIT *hMinion;
		int nPicked;
		int nCount;

		ptRegion = (MONREGION *)D2CommonMonsterRegionArrayGetDword(&ptGame->pMonRegions,
			D2CommonDungeonRoomGetLevelIdChecked(hRoom));
		nPicked = MonsterRegionPickClass(ptRegion, hRoom, &ptStats, 0, 1);
		if (!ptStats)
			return NULL;
		hMonster = MonsterSpawn(ptGame, hRoom, nX, nY, nPicked, dwArg, -1, 0);
		if (!hMonster)
			break;
		MonsterUniqueMakeChampion(ptGame, hMonster);
		if (!MonsterUniqueIsChampion(((MONSTERDATA *)hMonster->uTypeData)->ptUnique))
			break;
		for (nCount = (int)D2CommonSeedRand(&hMonster->tSeed, 3) + 1; nCount > 0; nCount--)
		{
			hMinion = MonsterSpawnMinion(ptGame, NULL, hMonster, nPicked, 1, 4, 0);
			if (hMinion)
				MonsterUniqueMakeChampion(ptGame, hMinion);
		}
		return hMonster;
	}
	case 414:
		hMonster = MonsterSpawn(ptGame, hRoom, nX, nY, 266, dwArg, -1, 0);
		break;
	case 418:
		hMonster = MonsterSpawn(ptGame, hRoom, nX, nY, 284, dwArg, -1, 8);
		break;
	case 427:
	case 428:
	case 432:
	case 433:
	{
		int nLevelId;

		if (nClass == 427)
			nClass = 19;
		else if (nClass == 428)
			nClass = 58;
		else if (nClass == 432)
			nClass = 141;
		else if (nClass == 433)
			nClass = 278;
		nClass = (int)D2CommonMonsterResolvePopulationIndex(hRoom, nClass);
		nLevelId = (int)D2CommonDungeonRoomGetLevelId(hRoom);
		{
		int nRunCount;
		int nRunIndex;

		D2CommonMonsterTblsGetRun(nClass, &nRunCount, &nRunIndex);
		switch (nClass - nRunIndex)
		{
		case 19:
			if (nLevelId == 6)
				nClass = 20;
			else if (nLevelId == 7 || nLevelId == 12 || nLevelId == 16)
				nClass = 21;
			break;
		case 58:
			if (nLevelId == 6)
				nClass = 59;
			else if (nLevelId == 7)
				nClass = 59;
			else if (nLevelId == 12)
				nClass = 60;
			else if (nLevelId == 16)
				nClass = 60;
			break;
		}
		}
		{
		MONSTATSTXT *ptStats;
		DWORD dwFlags;

		ptStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(nClass);
		dwFlags = 0;
		if (ptStats->bNeverCount)
			dwFlags = 8;
		hMonster = MonsterSpawn(ptGame, hRoom, nX, nY, nClass, dwArg, -1, dwFlags);
		if (!hMonster)
			hMonster = MonsterSpawn(ptGame, hRoom, nX, nY, nClass, dwArg, 4, dwFlags);
		}
		break;
	}
	default:
	{
		MONSTATSTXT *ptStats;
		DWORD dwFlags;

		if (nClass >= 410)
			break;
		ptStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(nClass);
		if (ptStats->bIsCritter)
			return NULL;
		dwFlags = 0;
		if (ptStats->bNeverCount)
			dwFlags = 8;
		hMonster = MonsterSpawn(ptGame, hRoom, nX, nY, nClass, dwArg, -1, dwFlags);
		if (hMonster)
			break;
		switch (nClass)
		{
		case 229:
		case 284:
		case 285:
		case 286:
		case 287:
		case 288:
		case 392:
		case 393:
			break;
		default:
			hMonster = MonsterSpawn(ptGame, hRoom, nX, nY, nClass, dwArg, 4, dwFlags);
		}
		break;
	}
	}
	return hMonster;
}

static __inline void MonsterPopulateMinions(GAME *ptGame, DRLGLOGICNODE *ptArea, UNIT *hLeader, int nClass,
	BYTE bMinGrp, BYTE bMaxGrp)
{
	int nMinions;
	int nRange;

	bMinGrp--;
	bMaxGrp--;
	nRange = bMaxGrp - bMinGrp + 1;
	nMinions = bMinGrp + (nRange ? (int)D2CommonSeedRand(&hLeader->tSeed, nRange) : 0);
	for (; nMinions > 0; nMinions--)
		MonsterSpawnMinion(ptGame, ptArea, hLeader, nClass, 1, 3, 0);
}

static __inline UNIT *MonsterPopulateGroup(GAME *ptGame, GAMEROOM *hRoom, DRLGLOGICNODE *ptArea, int nClass,
	MONSTATSTXT **pptStats)
{
	UNIT *hLeader;
	int nRunCount;
	int nRunIndex;
	int nX;
	int nY;
	BYTE bMinGrp;
	BYTE bMaxGrp;

	D2CommonMonsterTblsGetRun(nClass, &nRunCount, &nRunIndex);
	if (nClass - nRunIndex == 19 || nClass - nRunIndex == 91)
		return NULL;
	bMinGrp = (*pptStats)->bMinGrp;
	bMaxGrp = (*pptStats)->bMaxGrp;
	if (!bMinGrp && !bMaxGrp)
		return NULL;
	if (bMinGrp && bMaxGrp < bMinGrp)
		return NULL;
	if (!MonsterFindSpawnPoint(ptGame, hRoom, ptArea, nClass, &nX, &nY, TRUE))
		return NULL;
	hLeader = MonsterSpawnInArea(ptGame, hRoom, ptArea, nX, nY, nClass, 1, -1, 0);
	if (!hLeader)
		return NULL;
	MonsterPopulateMinions(ptGame, ptArea, hLeader, nClass, bMinGrp, bMaxGrp);
	return hLeader;
}

extern void MonsterPopulateAreas(GAME *ptGame, GAMEROOM *hRoom);
__inline void MonsterPopulateAreas(GAME *ptGame, GAMEROOM *hRoom)
{
	MONREGION *ptRegion;
	MONSTATSTXT *ptStats;
	DRLGLOGICNODE *ptArea;
	FORMATIONPOINT *ptFormation;
	D2SEED *ptSeed;
	UNIT *hLeader;
	UNIT *hMinion;
	RECT tRect;
	BOOL bSpawned;
	int nTries;
	int nPicked;
	int nKind;
	int nMinions;

	ptArea = (DRLGLOGICNODE *)D2CommonDungeonRoomGetLogicList(hRoom);
	ptRegion = (MONREGION *)D2CommonMonsterRegionArrayGetDword(&ptGame->pMonRegions,
		D2CommonDungeonRoomGetLevelIdChecked(hRoom));
	if ((int)ptRegion->dwMonDen > 100000)
		ptRegion->dwMonDen = 100000;
	bSpawned = FALSE;
	if (!ptArea)
		return;
	for (; ptArea; ptArea = ptArea->ptNext)
	{
		if (!ptArea->nLogic || ptArea->field_20)
			continue;
		CopyRect(&tRect, &ptArea->tRect);
		if (!tRect.left && !tRect.right)
			continue;
		D2CommonEnvTileToSubtile(&tRect.left, &tRect.top);
		D2CommonEnvTileToSubtile(&tRect.right, &tRect.bottom);
		nTries = (tRect.bottom - tRect.top) / 3 * ((tRect.right - tRect.left) / 3);
		if (hRoom)
			ptSeed = &ROOM5(hRoom)->tSeed;
		else
			ptSeed = NULL;
		while (nTries > 0)
		{
			if (D2CommonSeedRand(ptSeed, 100000) > ptRegion->dwMonDen)
			{
				nTries--;
				continue;
			}
			nPicked = MonsterRegionPickClass(ptRegion, hRoom, &ptStats, 20, 0);
			if (!ptStats)
				return;
			nKind = MonsterRegionGetKind(ptRegion, hRoom);
			if (nKind == 1)
				nKind = 2;
			switch (nKind)
			{
			case 0:
				nPicked = MonsterRegionPickClass(ptRegion, hRoom, &ptStats, 0, 1);
				hLeader = MonsterUniqueSpawnBoss(ptGame, hRoom, ptArea, nPicked, 1);
				if (hLeader)
				{
					if (MonsterUniqueIsChampion(((MONSTERDATA *)hLeader->uTypeData)->ptUnique))
					{
						for (nMinions = (int)D2CommonSeedRand(&hLeader->tSeed, 3) + 1; nMinions > 0; nMinions--)
						{
							hMinion = MonsterSpawnMinion(ptGame, ptArea, hLeader, nPicked, 1, 4, 0);
							if (hMinion)
								MonsterUniqueMakeChampion(ptGame, hMinion);
						}
					}
					goto lSpawned;
				}
				break;
			case 1:
			{
				int nX;
				int nY;

				ptFormation = MonsterPickFormation(hRoom);
				if (!MonsterFindFormationPoint(ptGame, hRoom, NULL, nPicked, ptFormation, &nX, &nY))
					goto lNextTry;
				hLeader = (UNIT *)MonsterSpawnFormation(ptGame, hRoom, nX, nY, NULL, nPicked, 1, ptFormation, 0);
				break;
			}
			default:
				hLeader = MonsterPopulateGroup(ptGame, hRoom, ptArea, nPicked, &ptStats);
				break;
			}
			if (!hLeader)
				goto lNextTry;
lSpawned:
			bSpawned = TRUE;
lNextTry:
			nTries--;
		}
	}
	if (bSpawned)
		ptRegion->dwRoomsWithMonsters++;
}

void __fastcall MonsterPopulateRoom(GAME *ptGame, GAMEROOM *hRoom)
{
	MONREGION *ptRegion;
	int nLevelId;

	ptRegion = (MONREGION *)D2CommonMonsterRegionArrayGetDword(&ptGame->pMonRegions,
		D2CommonDungeonRoomGetLevelId(hRoom));
	if (!ptRegion)
		return;
	ptRegion->dwRoomsPopulated++;
	nLevelId = (int)D2CommonDungeonRoomGetLevelIdChecked(hRoom);
	if (!nLevelId)
		return;
	if (ptRegion->nRooms < 0)
		ptRegion->nRooms = D2CommonDungeonActCountLevelRooms(ptGame->ptAct[ptRegion->bAct], nLevelId);
	if (!ptRegion->dwMonDen || !ptRegion->nRooms)
		return;
	if (D2CommonDungeonRoomGetLevelId(hRoom) == 108 && A4Q2MonstersReleased(ptGame))
		return;
	MonsterPopulateAreas(ptGame, hRoom);
}

BOOL __fastcall MonsterFindFormationPoint(GAME *ptGame, GAMEROOM *hRoom,
	DRLGLOGICNODE *ptArea, int nClass,
	FORMATIONPOINT *ptFormation, int *pnX, int *pnY)
{
	RECT atRect[2];
	D2SEED *ptSeed;
	FORMATIONPOINT *ptPoint;
	BOOL bArea;
	int nLogic;
	int nX0;
	int nY0;
	int nWidth;
	int nHeight;
	int nCount;
	int nTry;
	int nX;
	int nY;
	int i;

	if (!ptArea)
	{
		D2CommonDungeonRoomCopyRects(hRoom, atRect);
		nX0 = atRect[0].left + 1;
		nY0 = atRect[0].top + 1;
		nWidth = atRect[0].right - 1;
		nHeight = atRect[0].bottom - 1;
		nLogic = 0;
		bArea = FALSE;
	}
	else
	{
		CopyRect(&atRect[0], &ptArea->tRect);
		D2CommonEnvTileToSubtile(&atRect[0].left, &atRect[0].top);
		D2CommonEnvTileToSubtile(&atRect[0].right, &atRect[0].bottom);
		bArea = TRUE;
		nX0 = atRect[0].left + 1;
		nY0 = atRect[0].top + 1;
		nWidth = atRect[0].right - nX0;
		nHeight = atRect[0].bottom - nY0;
		nLogic = ptArea->nLogic;
	}
	if (hRoom)
		ptSeed = &ROOM5(hRoom)->tSeed;
	else
		ptSeed = NULL;
	nCount = ptFormation->nX;
	ptFormation++;
	for (nTry = 0; nTry < 20; nTry++)
	{
		nX = (int)D2CommonSeedRand(ptSeed, nWidth) + nX0;
		nY = (int)D2CommonSeedRand(ptSeed, nHeight) + nY0;
		for (ptPoint = ptFormation, i = 0; i < nCount; ptPoint++, i++)
		{
			if (bArea)
			{
				if (D2CommonDungeonGetLogicValue(hRoom, ptPoint->nX + nX, ptPoint->nY)
					!= nLogic)
					break;
				if (!MonsterSpawnInArea(ptGame, hRoom, ptArea, ptPoint->nX + nX, ptPoint->nY + nY, nClass, 1,
					-1, 1))
					break;
			}
			else if (!MonsterSpawn(ptGame, hRoom, ptPoint->nX + nX, ptPoint->nY + nY, nClass, 1, -1, 1))
				break;
		}
		if (i >= nCount)
			break;
	}
	if (nTry >= 20)
		return FALSE;
	*pnX = nX;
	*pnY = nY;
	return TRUE;
}

void __fastcall MonsterSpawnCritter(GAME *ptGame, GAMEROOM *hRoom)
{
	LEVELSTXT *ptLevel;
	MONREGION *ptRegion;
	UNIT *hMonster;
	int nClass;
	int nLevelId;
	int nCount;
	int nFirst;

	if (D2CommonSeedNext(hRoom ? &ROOM5(hRoom)->tSeed : NULL) & 32767)
		return;
	if (hRoom->nNumClients)
		return;
	nLevelId = (int)D2CommonDungeonRoomGetLevelId(hRoom);
	ptLevel = (LEVELSTXT *)D2CommonLvlTblsGetLevelData(nLevelId);
	if (!ptLevel->bMonWndr || !ptLevel->nMonDen)
		return;
	if ((int)D2CommonSeedRand(hRoom ? &ROOM5(hRoom)->tSeed : NULL, 100) >= 3)
		return;
	ptRegion = (MONREGION *)D2CommonMonsterRegionArrayGetDword(&ptGame->pMonRegions, nLevelId);
	if (ptRegion->nCritters >= 3)
		return;
	{
	BYTE bAct = ptLevel->bAct;
	if (bAct >= NUM_ACTS)
		return;
	nFirst = sgatCritterRun[bAct].bFirst; nCount = sgatCritterRun[bAct].bCount; }
	if (!nCount) return;
	nClass = sgnCritterClass[D2CommonSeedRand(hRoom ? &ROOM5(hRoom)->tSeed : NULL, nCount) + nFirst];
	{ int nX; int nY;
	if (!MonsterFindSpawnPoint(ptGame, hRoom, NULL, nClass, &nX, &nY, FALSE))
		return;
	hMonster = MonsterSpawn(ptGame, hRoom, nX, nY, nClass, 1, -1, 0); }
	if (!hMonster)
		return;
	TargetsAddToFurtherList(ptGame, hMonster, 0, 8);
	ptRegion->nCritters++;
}

void __fastcall MonsterRegionsInit(GAME *ptGame)
{
	D2SEED tSeed;
	DWORD dwSeed;

	D2CommonSeedInit(&tSeed);
	dwSeed = D2CommonSeedInvent(0);
	D2CommonSeedSet(&tSeed, dwSeed);
	ptGame->dwMonRegionSeed = dwSeed;
	D2CommonMonsterRegionCreatePopulation(ptGame->pMemPool, &ptGame->pMonRegions, &tSeed, dwSeed);
}

void __fastcall MonsterRegionAdd(DWORD *padwRegions, GAMEROOM *hRoom, UNIT *hUnit,
	BOOL bFlag)
{
	MONREGION *ptRegion;
	void *pRecord;

	pRecord = ((MONSTERDATA *)hUnit->uTypeData)->pRegionRef;
	ptRegion = (MONREGION *)D2CommonMonsterRegionArrayGetDword(padwRegions,
		D2CommonDungeonRoomGetLevelId(hRoom));
	if (bFlag)
	{
		AiRecordSetFlags((AIRECORDHEAD *)pRecord, AIFLAG_02, TRUE);
		return;
	}
	ptRegion->nMonsters++;
}

void __fastcall MonsterRegionModeChange(DWORD *padwRegions, UNIT *hUnit,
	int nModeOld, int nModeNew)
{
	MONREGION *ptRegion;
	void *pRecord;

	if (hUnit->eType != 1)
		return;
	pRecord = ((MONSTERDATA *)hUnit->uTypeData)->pRegionRef;
	if (AiRecordTestFlags((AIRECORDHEAD *)pRecord, AIFLAG_02))
		return;
	ptRegion = (MONREGION *)D2CommonMonsterRegionArrayGetDword(padwRegions,
		MonsterRegionRefGetLevel(pRecord));
	if (nModeOld == nModeNew)
		return;
	if (nModeOld == 2 && (nModeNew == 1 || nModeNew == 0))
		ptRegion->nMonsters--;
	if (nModeNew == 2 && nModeOld != 4)
		ptRegion->nMonsters++;
}

void __fastcall MonsterRegionAddAligned(DWORD *padwRegions, UNIT *hUnit)
{
	MONREGION *ptRegion;
	void *pRecord;

	pRecord = ((MONSTERDATA *)hUnit->uTypeData)->pRegionRef;
	if (AiRecordTestFlags((AIRECORDHEAD *)pRecord, AIFLAG_02))
		return;
	ptRegion = (MONREGION *)D2CommonMonsterRegionArrayGetDword(padwRegions,
		MonsterRegionRefGetLevel(pRecord));
	if (D2CommonStatsUnitGetAlignment(hUnit) == 2)
		ptRegion->field_130++;
}
void __fastcall MonsterRegionRemoveAligned(DWORD *padwRegions, UNIT *hUnit)
{
	MONREGION *ptRegion;
	void *pRecord;

	pRecord = ((MONSTERDATA *)hUnit->uTypeData)->pRegionRef;
	if (AiRecordTestFlags((AIRECORDHEAD *)pRecord, AIFLAG_02))
		return;
	ptRegion = (MONREGION *)D2CommonMonsterRegionArrayGetDword(padwRegions,
		MonsterRegionRefGetLevel(pRecord));
	if (D2CommonStatsUnitGetAlignment(hUnit) == 2)
		ptRegion->field_130--;
}

void __fastcall MonsterRegionRemove(DWORD *padwRegions, int nLevel,
	int nLevelOld, int nMode, BOOL fDead,
	BOOL fNoCount, UNIT *hUnit)
{
	MONREGION *ptRegion;
	MONREGION *ptRegionOld;
	MONSTERDATA *ptData;

	ptRegion = (MONREGION *)D2CommonMonsterRegionArrayGetDword(padwRegions, nLevel);
	ptRegionOld = (MONREGION *)D2CommonMonsterRegionArrayGetDword(padwRegions, nLevelOld);
	if (hUnit)
	{
		ptData = (MONSTERDATA *)hUnit->uTypeData;
		if (ptRegionOld != ptRegion)
			MonsterRegionRefSetLevel((MONSTERREGIONREF *)ptData->pRegionRef, (DWORD)nLevel);
		if (fNoCount)
			return;
		if (D2CommonStatsUnitGetAlignment(hUnit) != 2)
			return;
		ptRegionOld->nMonsters--;
		if (MonsterUniqueIsUnique(ptData->ptUnique))
			ptRegionOld->nUniques--;
		return;
	}
	if (!fNoCount && !fDead && nMode == 2)
		ptRegion->nMonsters--;
}

BOOL __fastcall MonsterHasRoom(int nClass, GAMEROOM *hRoom, int nX,
	int nY, BOOL bArg)
{
	GAMEROOM *hRoomAt;
	int nRunCount;
	int nRunIndex;
	int nPX, nPY;

	D2CommonMonsterTblsGetRun(nClass, &nRunCount, &nRunIndex);
	nClass -= nRunIndex;
	if (nClass == 206)
	{
		hRoomAt = (GAMEROOM *)D2CommonDungeonFindRoomGame(hRoom, nX, nY + 3);
		if (!hRoomAt)
			return FALSE;
		if (D2CommonCollisnTestSize(hRoomAt, nX, nY + 3, 2, 7169))
			return FALSE;
	}
	if (nClass == 228)
	{
		hRoomAt = (GAMEROOM *)D2CommonDungeonFindRoomGame(hRoom, nX, nY + 2);
		if (!hRoomAt)
			return FALSE;
		if (D2CommonCollisnTestSize(hRoomAt, nX, nY + 2, 2, 7169))
			return FALSE;
	}
	if (nClass == 334)
	{
		nPX = nX - 2;
		nPY = nY - 2;
		hRoomAt = (GAMEROOM *)D2CommonDungeonFindRoomGame(hRoom, nPX, nPY);
		if (!hRoomAt)
			return FALSE;
		if (D2CommonCollisnTestSize(hRoomAt, nPX, nPY, 2, 448))
			return FALSE;
	}
	if (nClass == 298 && !bArg)
	{
		hRoomAt = (GAMEROOM *)D2CommonDungeonFindRoomGame(hRoom, nX, nY);
		if (!hRoomAt)
			return FALSE;
		if (D2CommonCollisnTestSize(hRoomAt, nX, nY, 2, 7169))
			return FALSE;
	}
	return TRUE;
}

BOOL __fastcall MonsterCanUseSkill(GAME *ptGame, UNIT *hUnit, int nKind,
	UNIT *hTarget, int nX, int nY)
{
	GAMEROOM *hRoomAt;
	POINT tPoint;
	DWORD dwPath;
	int nUnitX;
	int nUnitY;
	int nTargetX;
	int nTargetY;

	D2CommonMonsterUnitGetTypeLocalIndex(hUnit);
	if (nKind == 167)
		return FALSE;
	if (nKind == 181)
	{
		if (!hTarget)
			return FALSE;
		if (!(hTarget->dwFlags0 & 2))
			return FALSE;
		dwPath = D2CommonPathGetCollisionPatternByUnit(hTarget);
		return !D2CommonCollisnTestPatternAnyRaw((DWORD)D2CommonUnitGetPathRoom(hTarget), (DWORD)D2CommonUnitGetX(hTarget),
			(DWORD)D2CommonUnitGetY(hTarget), dwPath, 7169);
	}
	if (nKind == 164)
		return hTarget != NULL;
	if (nKind == 132)
	{
		int nFromX, nFromY, nDX;

		if (!hTarget)
			return FALSE;
		if (D2CommonMonsterUnitTestFieldC(hUnit))
			return FALSE;
		nFromX = (int)D2CommonUnitGetX(hUnit);
		nFromY = (int)D2CommonUnitGetY(hUnit);
		nTargetX = (int)D2CommonUnitGetX(hTarget);
		nDX = nTargetX - nFromX;
		nTargetX += nDX;
		nTargetY = 2 * (int)D2CommonUnitGetY(hTarget) - nFromY;
		tPoint.x = nTargetX;
		tPoint.y = nTargetY;
		hRoomAt = (GAMEROOM *)D2CommonCollisnFindSpawnPointNear(D2CommonUnitGetPathRoom(hUnit), &tPoint,
			D2CommonUnitGetSizeX(hUnit), 7169, 0);
		if (!hRoomAt)
			return FALSE;
		if (D2CommonDungeonRoomIsTown(hRoomAt))
			return FALSE;
		if (D2CommonCollisnTestPatternAnyRaw((DWORD)hRoomAt, (DWORD)nTargetX, (DWORD)nTargetY,
			D2CommonPathGetCollisionPatternByUnit(hUnit), 7169)) return FALSE;
		return D2CommonSkillsAddFieldA(nFromX, nFromY, nTargetX, nTargetY,
			hRoomAt, 3073) != 0;
	}
	if (nKind == 199)
		return MonsterSpawnFormationAt(ptGame, (GAMEROOM *)D2CommonUnitGetPathRoom(hTarget), 0, 0,
			hTarget,
			340, 1);
	if (nKind == 184)
	{
		if (!hUnit)
			return FALSE;
		hRoomAt = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hUnit), nX, nY);
		if (!hRoomAt)
			return FALSE;
		if (D2CommonDungeonRoomIsTown(hRoomAt))
			return FALSE;
		if (D2CommonCollisnTestPatternAnyRaw((DWORD)hRoomAt, (DWORD)nX, (DWORD)nY,
			D2CommonPathGetCollisionPatternByUnit(hUnit), 7169))
			return FALSE;
		nUnitX = (int)D2CommonUnitGetX(hUnit);
		nUnitY = (int)D2CommonUnitGetY(hUnit);
		return D2CommonSkillsAddFieldA(nUnitX, nUnitY, nX, nY, hRoomAt,
			3073) != 0;
	}
	if (nKind == 107)
	{
		if (!hUnit)
			return FALSE;
		hRoomAt = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hUnit), nX, nY);
		if (!hRoomAt)
			return FALSE;
		if (D2CommonDungeonRoomIsTown(hRoomAt))
			return FALSE;
		nUnitX = (int)D2CommonUnitGetX(hUnit);
		nUnitY = (int)D2CommonUnitGetY(hUnit);
		return D2CommonSkillsAddFieldA(nUnitX, nUnitY, nX, nY, hRoomAt,
			3073) != 0;
	}
	if (nKind == 203)
	{
		if (!hUnit)
			return FALSE;
		hRoomAt = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hUnit), nX, nY);
		if (!hRoomAt)
			return FALSE;
		if (D2CommonDungeonRoomIsTown(hRoomAt))
			return FALSE;
		nUnitX = (int)D2CommonUnitGetX(hUnit);
		nUnitY = (int)D2CommonUnitGetY(hUnit);
		return D2CommonSkillsAddFieldA(nUnitX, nUnitY, nX, nY, hRoomAt,
			2053) != 0;
	}
	return TRUE;
}

int __fastcall MonsterGetSpawnMode(UNIT *hUnit, int nSkill, DWORD dwUnused)
{
	void *pSkill;
	DWORD dwClass;

	dwClass = D2CommonMonsterUnitGetTypeLocalIndex(hUnit);
	if (nSkill == 181)
	{
		if (dwClass == 19)
			return 8;
		if (dwClass == 0 || dwClass == 274 || dwClass == 379 || dwClass == 383 || dwClass == 387)
		{
			pSkill = (void *)D2CommonSkillsFindByKey(hUnit, 158);
			D2CommonUnitSetSkillThirdCached(hUnit, pSkill);
			return 14;
		}
		if (dwClass == 96)
			return 8;
		if (dwClass != 141 && dwClass != 396)
			return 1;
		pSkill = (void *)D2CommonSkillsFindByKey(hUnit, 158);
		D2CommonUnitSetSkillThirdCached(hUnit, pSkill);
		return 14;
	}
	return 1;
}

static __inline UNIT *MonsterPlaceNear(GAME *ptGame, UNIT *hUnit, int nClass,
	DWORD dwMode, int nRadius, WORD wFlags)
{
	MONSPAWNCTX tCtx;

	tCtx.dwUnitId = 0;
	tCtx.nClass = nClass;
	tCtx.dwMode = dwMode;
	tCtx.hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	tCtx.nRadius = nRadius;
	tCtx.ptGame = ptGame;
	tCtx.ptArea = NULL;
	tCtx.wFlags = wFlags;
	tCtx.nX = (int)D2CommonUnitGetX(hUnit);
	tCtx.nY = (int)D2CommonUnitGetY(hUnit);
	return MonsterSpawnAround(&tCtx);
}

UNIT * __fastcall MonsterSpawnNear(GAME *ptGame, UNIT *hUnit, int nClass,
	DWORD dwMode, int nRadius, WORD wFlags)
{
	return MonsterPlaceNear(ptGame, hUnit, nClass, dwMode, nRadius, wFlags);
}

static __inline int MonRandI(D2SEED *ptSeed, int n)
{
	if (n)
		return (int)D2CommonSeedRand(ptSeed, n);
	return 0;
}

UNIT * __fastcall MonsterSpawnAround(MONSPAWNCTX *ptCtx)
{
	RECT tRect;
	MONSTATSTXT *ptStats;
	MONSTATSTXT *ptRunStats;
	DRLGTILE *ptTiles;
	DRLGTILE *ptTile;
	int i;
	POINT tPoint;
	GAMEROOM *hRoom;
	D2SEED *ptSeed;
	UNIT *hMonster;
	UNIT *hMinion;
	GAME *ptGame;
	void *ptArea;
	DWORD dwPattern;
	DWORD dwTile;
	DWORD dwStart;
	DWORD dwNeverCount;
	BOOL bArea;
	BOOL bInside;
	int nLogic;
	int nRadius;
	int nRing;
	int nRings;
	int nSide;
	int nSteps;
	int nX;
	int nY;
	int nDX;
	int nDY;
	int nOffX;
	int nOffY;
	int nMinX;
	int nMinY;
	int nMaxX;
	int nMaxY;
	int nFoundX;
	int nFoundY;
	int nCreateFlags;
	int nCount;
	int nDiff;
	int nClass;
	int nMinionClass;
	WORD wFlags;

	hRoom = ptCtx->hRoom;
	wFlags = ptCtx->wFlags;
	ptStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(ptCtx->nClass);
	switch (ptStats->bSpawnCol)
	{
	case 0:
		dwPattern = 7169;
		break;
	case 1:
		dwPattern = 448;
		break;
	case 2:
		dwPattern = 7953;
		break;
	default:
		dwPattern = 7169;
		break;
	}
	if (!ptCtx->ptArea)
	{
		GAMERECT atRoomRects[2];

		D2CommonDungeonRoomCopyRects(hRoom, atRoomRects);
		tRect.bottom = atRoomRects[0].nY + atRoomRects[0].nHeight;
		tRect.left = atRoomRects[0].nX;
		tRect.right = atRoomRects[0].nX + atRoomRects[0].nWidth;
		tRect.top = atRoomRects[0].nY;
		nLogic = 0;
		bArea = FALSE;
	}
	else
	{
		CopyRect(&tRect, &ptCtx->ptArea->tRect);
		D2CommonEnvTileToSubtile(&tRect.left, &tRect.top);
		D2CommonEnvTileToSubtile(&tRect.right, &tRect.bottom);
		bArea = TRUE;
		nLogic = ptCtx->ptArea->nLogic;
	}
	if (hRoom)
		ptSeed = &ROOM5(hRoom)->tSeed;
	else
		ptSeed = NULL;
	nRadius = ptCtx->nRadius;
	if (nRadius < 0)
		nRing = 0;
	else
		nRing = 3;
	if (!nRing)
		nRings = 0;
	else
		nRings = nRadius * 3;
	nFoundY = 0;
	nFoundX = 0;
	if (ptStats->bSpawnCol == 1 && !(ptCtx->nClass >= 258 && ptCtx->nClass <= 263)
		&& ptCtx->nClass != 153)
	{
		DWORD dwTiles;

		ptTiles = (DRLGTILE *)D2CommonDungeonRoomGetSecondTileArray(hRoom, &dwTiles);
		if (ptTiles && dwTiles)
		{
			GAMERECT atRoomRects[2];
			DWORD dwFirst;

			dwFirst = D2CommonSeedRand(hRoom ? &ROOM5(hRoom)->tSeed : NULL, dwTiles);
			if (!dwFirst)
				dwFirst = 1;
			D2CommonDungeonRoomCopyRects(hRoom, atRoomRects);
			dwTile = dwFirst;
			dwStart = dwFirst - 1;
			while (dwTile != dwStart)
			{
				ptTile = &ptTiles[dwTile];
				if (ptTile->pTile && D2CMPTileGetFlags(ptTile->pTile) & 2)
				{
					nX = (ptTile->nX + atRoomRects[1].nX) * 5 + 3;
					nY = (ptTile->nY + atRoomRects[1].nY) * 5 + 3;
					if (!D2CommonCollisnTestPoint(hRoom, nX, nY, 256))
					{
						for (i = 0; i < 4; i++)
						{
							if (!D2CommonCollisnTestSize(hRoom, nX + sgatSpawnOffsets[i].x,
								nY + sgatSpawnOffsets[i].y, 2, 7177))
							{
								nFoundX = nX;
								nFoundY = nY;
								goto lFound;
							}
						}
					}
				}
				dwTile = (dwTile + 1) % dwTiles;
			}
		}
lFound:
		hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(hRoom, nFoundX, nFoundY);
	}
	else
	{
		while (nRing <= nRings)
		{
			nX = ptCtx->nX;
			nY = ptCtx->nY;
			nSide = nRing * 2;
			if (D2CommonSeedNext(ptSeed) & 1)
			{
				nOffX = nRing;
				nOffY = MonRandI(ptSeed, nSide >> 1);
				nDX = 0;
				nDY = 1;
			}
			else
			{
				nOffX = MonRandI(ptSeed, nSide >> 1);
				nOffY = nRing;
				nDX = 1;
				nDY = 0;
			}
			if (D2CommonSeedNext(ptSeed) & 1)
				nOffX = -nOffX;
			if (D2CommonSeedNext(ptSeed) & 1)
				nOffY = -nOffY;
			nX += nOffX;
			nY += nOffY;
			nMinX = ptCtx->nX - nRing;
			nMaxX = nMinX + nSide;
			nMaxY = ptCtx->nY - nRing + nSide;
			nMinY = ptCtx->nY - nRing;
			nSteps = nSide * 4;
			if (!nSteps)
				nSteps = 1;
			for (; nSteps > 0; nSteps--)
			{
				if (nX == nMinX && nY == nMinY)
				{
					nDX = 1;
					nDY = 0;
				}
				if (nX == nMaxX)
				{
					if (nY == nMinY)
					{
						nDX = 0;
						nDY = 1;
					}
					if (nY == nMaxY)
					{
						nDX = -1;
						nDY = 0;
					}
				}
				if (nX == nMinX)
				{
					if (nY == nMaxY)
					{
						nDX = 0;
						nDY = -1;
					}
					if (nY == nMinY && nX == nMaxX && nY == nMaxY)
					{
						nDX = 0;
						nDY = 0;
					}
				}
				nX += nDX;
				nY += nDY;
				tPoint.x = nX;
				tPoint.y = nY;
				bInside = PtInRect(&tRect, tPoint);
				if (bInside && bArea && D2CommonDungeonGetLogicValue(hRoom, nX, nY) != nLogic)
					bInside = FALSE;
				if (!bInside)
					continue;
				if (!MonsterHasRoom(ptCtx->nClass, hRoom, nX, nY, TRUE))
					continue;
				if (D2CommonCollisnTestSize(hRoom, nX, nY, ptStats->dwSizeX,
					dwPattern) && !((BYTE)ptCtx->wFlags & 128))
					continue;
				nFoundX = nX;
				nFoundY = nY;
				nRing = nRings;
				break;
			}
			nRing += 3;
		}
	}
	if (!nFoundX || !nFoundY)
		return NULL;
	if ((BYTE)wFlags & 1)
		return (UNIT *)1;
	nCreateFlags = 1;
	if ((BYTE)wFlags & 32)
		nCreateFlags = 3;
	hMonster = SUnitCreate(1, ptCtx->nClass, nFoundX, nFoundY, ptCtx->ptGame, hRoom, nCreateFlags,
		ptCtx->dwMode,
		ptCtx->dwUnitId);
	if (hMonster)
	{
		dwNeverCount = wFlags & 8;
		if (ptStats->bNeverCount)
			dwNeverCount = 1;
		MonsterRegionAdd((DWORD *)&ptCtx->ptGame->pMonRegions, hRoom, hMonster, dwNeverCount);
		ptArea = ptCtx->ptArea;
		if (!ptArea)
			ptArea = (void *)D2CommonDungeonRoomGetLogicNode(hRoom, nFoundX, nFoundY);
		((MONSTERDATA *)hMonster->uTypeData)->ptArea = ptArea;
		switch (ptStats->bAlign)
		{
		case 1:
			SUnitSetAlignment(hMonster, 0);
			hMonster->dwFlags0 |= 131072;
			break;
		case 2:
			SUnitSetAlignment(hMonster, 1);
			break;
		default:
			SUnitSetAlignment(hMonster, 2);
			break;
		}
		if (!((BYTE)wFlags & 2))
		{
			int nRunCount;
			int nRunIndex;

			nClass = ptCtx->nClass;
			D2CommonMonsterTblsGetRun(nClass, &nRunCount, &nRunIndex);
			ptGame = ptCtx->ptGame;
			switch (nClass - nRunIndex)
			{
			case 96:
				MonsterUniqueAddMod(ptGame, hMonster, 10, 0);
				break;
			case 91:
				MonsterUniqueAddMod(ptGame, hMonster, 20, 0);
				break;
			case 24:
				MonsterUniqueAddMod(ptGame, hMonster, 13, 0);
				break;
			case 211:
				MonsterUniqueAddMod(ptGame, hMonster, 11, 0);
				MonsterUniqueAddMod(ptGame, hMonster, 22, 1);
				break;
			case 340:
			case 341:
			case 342:
			case 343:
				MonsterUniqueAddMod(ptGame, hMonster, 15, 0);
				break;
			case 326:
			case 327:
			case 328:
			case 329:
			case 330:
			case 354:
				MonsterUniqueAddMod(ptGame, hMonster, 14, 0);
				break;
			}
		}
		{
			int nRunCount;
			int nRunIndex;

			D2CommonMonsterTblsGetRun(ptCtx->nClass, &nRunCount, &nRunIndex);
			ptGame = ptCtx->ptGame;
			hRoom = ptCtx->hRoom;
			switch (ptCtx->nClass - nRunIndex)
			{
			case 211:
				QuestsAttachUnit(ptGame, hMonster, 13);
				QuestsAttachUnit(ptGame, hMonster, 9);
				break;
			case 175:
				QuestsObjectSpawned(ptGame, hRoom, (int)D2CommonUnitGetX(hMonster),
					(int)D2CommonUnitGetY(hMonster), 175);
				break;
			case 250:
				QuestsAttachUnit(ptGame, hMonster, 12);
				break;
			case 243:
				MonsterUniqueAddMod(ptGame, hMonster, 22, 1);
				QuestsAttachUnit(ptGame, hMonster, 23);
				break;
			case 366:
				QuestsAttachUnit(ptGame, hMonster, 19);
				hMonster->dwFlags0 |= 131072;
				MonsterUniqueAddMod(ptGame, hMonster, 22, 1);
				break;
			case 340:
			case 341:
			case 342:
			case 343:
				hMonster->dwFlags0 |= 131072;
				break;
			case 242:
				QuestsAttachUnit(ptGame, hMonster, 20);
				MonsterUniqueAddMod(ptGame, hMonster, 22, 1);
				break;
			case 156:
				MonsterUniqueAddMod(ptGame, hMonster, 22, 1);
				QuestsAttachUnit(ptGame, hMonster, 6);
				break;
			case 409:
				QuestsAttachUnit(ptGame, hMonster, 24);
				break;
			case 256:
				MonsterUniqueAddMod(ptGame, hMonster, 22, 1);
				QuestsAttachUnit(ptGame, hMonster, 22);
				break;
			case 407:
				QuestsAttachUnit(ptGame, hMonster, 17);
				break;
			case 229:
				QuestsAttachUnit(ptGame, hMonster, 8);
				break;
			case 267:
				MonsterUniqueAddMod(ptGame, hMonster, 12, 1);
				MonsterUniqueAddMod(ptGame, hMonster, 22, 1);
				QuestsAttachUnit(ptGame, hMonster, 2);
				break;
			case 284:
				MonsterUniqueAddMod(ptGame, hMonster, 23, 1);
				MonsterUniqueAddMod(ptGame, hMonster, 22, 1);
				break;
			case 402:
				MonsterUniqueAddMod(ptGame, hMonster, 22, 1);
				break;
			case 292:
				MonsterUniqueAddMod(ptGame, hMonster, 31, 0);
				break;
			}
		}
		if (!((BYTE)wFlags & 64))
		{
			int nRunCount;
			int nFirst;

			nClass = ptCtx->nClass;
			D2CommonMonsterTblsGetRun(nClass, &nRunCount, &nFirst);
			ptGame = ptCtx->ptGame;
			wFlags = ptCtx->wFlags;
			switch (ptCtx->nClass - nFirst)
			{
			case 58:
				AiSetOwner(ptGame, hMonster, hMonster->dwId, 1, 1, 0);
				nCount = (int)D2CommonSeedRand(&hMonster->tSeed, 5) + 2;
				nMinionClass = nFirst + 19;
				D2CommonUnitGetPathRoom(hMonster);
				{
					DWORD dwLeaderId;

					dwLeaderId = hMonster->dwId;
					for (; nCount > 0; nCount--)
					{
						hMinion = MonsterSpawnNear(ptGame, hMonster, nMinionClass, 1, 4, 64);
						if (hMinion)
						{
							AiSetOwner(ptGame, hMinion, dwLeaderId, 1, 0, 0);
							AiAddMinion(ptGame, hMonster, hMinion);
						}
					}
				}
				break;
			case 19:
				AiSetOwner(ptGame, hMonster, hMonster->dwId, 1, 1, 1);
				ptRunStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(nClass);
				nDiff = ptRunStats->bMaxGrp - ptRunStats->bMinGrp;
				if (nDiff)
					nDiff = (int)D2CommonSeedRand(&hMonster->tSeed, nDiff);
				else
					nDiff = 0;
				nCount = ptRunStats->bMinGrp + nDiff;
				nMinionClass = nFirst + 19;
				D2CommonUnitGetPathRoom(hMonster);
				{
					DWORD dwLeaderId;

					dwLeaderId = hMonster->dwId;
					for (; nCount > 0; nCount--)
					{
						hMinion = MonsterSpawnNear(ptGame, hMonster, nMinionClass, 1, 4, 64);
						if (hMinion)
						{
							AiSetOwner(ptGame, hMinion, dwLeaderId, 1, 0, 0);
							AiAddMinion(ptGame, hMonster, hMinion);
						}
					}
				}
				break;
			case 141:
				AiSetOwner(ptGame, hMonster, hMonster->dwId, 1, 1, 1);
				ptRunStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(nClass);
				nDiff = ptRunStats->bMaxGrp - ptRunStats->bMinGrp;
				if (nDiff)
					nDiff = (int)D2CommonSeedRand(&hMonster->tSeed, nDiff);
				else
					nDiff = 0;
				nCount = ptRunStats->bMinGrp + nDiff;
				nMinionClass = nFirst + 141;
				D2CommonUnitGetPathRoom(hMonster);
				{
					DWORD dwLeaderId;

					dwLeaderId = hMonster->dwId;
					for (; nCount > 0; nCount--)
					{
						hMinion = MonsterSpawnNear(ptGame, hMonster, nMinionClass, 1, 4, 64);
						if (hMinion)
						{
							AiSetOwner(ptGame, hMinion, dwLeaderId, 1, 0, 0);
							AiAddMinion(ptGame, hMonster, hMinion);
						}
					}
				}
				break;
			case 101:
			{
				int anClass[4];

				anClass[0] = 1;
				anClass[1] = 2;
				anClass[2] = 3;
				anClass[3] = 4;
				nCount = (int)D2CommonSeedRand(&hMonster->tSeed, 7) + 2;
				for (; nCount; nCount--)
					MonsterPlaceNear(ptGame, hMonster, anClass[nFirst], 1, 4, 64);
				break;
			}
			case 238:
				nCount = (int)D2CommonSeedRand(&hMonster->tSeed, 3) + 2;
				for (; nCount; nCount--)
					MonsterSpawnNear(ptGame, hMonster, nFirst + 235, 1, 4, 64);
				break;
			case 91:
				AiSetOwner(ptGame, hMonster, hMonster->dwId, 1, 1, 1);
				ptRunStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(nClass);
				nDiff = ptRunStats->bMaxGrp - ptRunStats->bMinGrp;
				if (nDiff)
					nDiff = (int)D2CommonSeedRand(&hMonster->tSeed, nDiff);
				else
					nDiff = 0;
				nCount = ptRunStats->bMinGrp + nDiff;
				MonsterSpawnMinions(ptGame, hMonster, nClass, 1, 4, nCount, 64);
				break;
			case 261:
				AiSetOwner(ptGame, hMonster, hMonster->dwId, 1, 1, 0);
				nCount = (int)D2CommonSeedRand(&hMonster->tSeed, 3) + 4;
				MonsterUniqueSpawnGroup(ptGame, hMonster, nFirst + 258, 1, nCount, !(wFlags & 4), 64);
				break;
			case 396:
				AiSetOwner(ptGame, hMonster, hMonster->dwId, 1, 1, 1);
				ptRunStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(nClass);
				nDiff = ptRunStats->bMaxGrp - ptRunStats->bMinGrp;
				if (nDiff)
					nDiff = (int)D2CommonSeedRand(&hMonster->tSeed, nDiff);
				else
					nDiff = 0;
				nCount = ptRunStats->bMinGrp + nDiff;
				MonsterSpawnMinions(ptGame, hMonster, nFirst + 396, 1, 4, nCount, 64);
				break;
			case 278:
			case 407:
				AiSetOwner(ptGame, hMonster, hMonster->dwId, 1, 1, 1);
				nCount = ((int)(D2CommonSeedNext(&hMonster->tSeed) & 3) + 2) >> 1;
				MonsterSpawnMinions(ptGame, hMonster, nFirst + 141, 1, 4, nCount, 64);
				MonsterSpawnMinions(ptGame, hMonster, nFirst + 396, 1, 4, nCount, 64);
				break;
			case 267:
				nCount = (int)D2CommonSeedRand(&hMonster->tSeed, 5) + 5;
				for (; nCount; nCount--)
					MonsterSpawnNear(ptGame, hMonster, 6, 1, 4, 64);
				break;
			case 313:
				AiSetOwner(ptGame, hMonster, hMonster->dwId, 1, 1, 0);
				nCount = (int)D2CommonSeedRand(&hMonster->tSeed, 7);
				MonsterSpawnMinions(ptGame, hMonster, nFirst + 63, 1, 4, nCount + 3, 64);
				AiForEachMinion(hMonster, (DWORD)hMonster, 0, (PFNAIMINIONFN)MonsterUniqueMinionAddLife);
				break;
			case 312:
				nCount = (int)D2CommonSeedRand(&hMonster->tSeed, 3) + 2;
				for (; nCount; nCount--)
					MonsterSpawnNear(ptGame, hMonster, 310, 1, 4, 64);
				break;
			}
		}
	}
	return hMonster;
}

void __fastcall MonsterUniqueMinionAddLife(UNIT *hMinion, UNIT *hLeader, DWORD dwUnused)
{
	int nLife;

	nLife = D2CommonStatsUnitGetValue(hMinion, 6);
	nLife += (DWORD)D2CommonStatsUnitGetValue(hLeader, 12);
	D2CommonStatsUnitSetValue(hMinion, 7, nLife);
	D2CommonStatsUnitSetValue(hMinion, 6, nLife);
}

UNIT *__fastcall MonsterSpawn(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY, int nClass,
	DWORD dwMode, int nRadius, WORD wFlags)
{
	MONSPAWNCTX tCtx;

	tCtx.dwUnitId = 0;
	tCtx.nClass = nClass;
	tCtx.dwMode = dwMode;
	tCtx.hRoom = hRoom;
	tCtx.nRadius = nRadius;
	tCtx.ptGame = ptGame;
	tCtx.ptArea = NULL;
	tCtx.wFlags = wFlags;
	tCtx.nX = nX;
	tCtx.nY = nY;
	return MonsterSpawnAround(&tCtx);
}

UNIT *__fastcall MonsterSpawnMinion(GAME *ptGame, struct DRLGLOGICNODE *ptArea,
	UNIT *hLeader, int nClass, DWORD dwMode, int nRadius,
	WORD wFlags)
{
	MONSPAWNCTX tCtx;

	tCtx.dwUnitId = 0;
	tCtx.nClass = nClass;
	tCtx.dwMode = dwMode;
	tCtx.hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hLeader);
	tCtx.ptGame = ptGame;
	tCtx.ptArea = ptArea;
	tCtx.nRadius = nRadius;
	tCtx.wFlags = wFlags;
	tCtx.nX = (int)D2CommonUnitGetX(hLeader);
	tCtx.nY = (int)D2CommonUnitGetY(hLeader);
	return MonsterSpawnAround(&tCtx);
}

UNIT *__fastcall MonsterSpawnInArea(GAME *ptGame, GAMEROOM *hRoom,
	struct DRLGLOGICNODE *ptArea,
	int nX, int nY, int nClass, DWORD dwMode, int nRadius,
	WORD wFlags)
{
	MONSPAWNCTX tCtx;

	tCtx.nClass = nClass;
	tCtx.dwUnitId = 0;
	tCtx.dwMode = dwMode;
	tCtx.hRoom = hRoom;
	tCtx.nRadius = nRadius;
	tCtx.ptArea = ptArea;
	tCtx.nX = nX;
	tCtx.ptGame = ptGame;
	tCtx.nY = nY;
	tCtx.wFlags = wFlags;
	return MonsterSpawnAround(&tCtx);
}

static __inline UNIT *MonsterPlaceAt(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY, int nClass,
	DWORD dwMode, WORD wFlags)
{
	MONSPAWNCTX tCtx;

	tCtx.dwUnitId = 0;
	tCtx.nClass = nClass;
	tCtx.dwMode = dwMode;
	tCtx.hRoom = hRoom;
	tCtx.nRadius = -1;
	tCtx.ptGame = ptGame;
	tCtx.ptArea = NULL;
	tCtx.wFlags = wFlags;
	tCtx.nX = nX;
	tCtx.nY = nY;
	return MonsterSpawnAround(&tCtx);
}

UNIT *__fastcall MonsterSpawnAt(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY, int nClass,
	DWORD dwMode, WORD wFlags)
{
	return MonsterPlaceAt(ptGame, hRoom, nX, nY, nClass, dwMode, wFlags);
}

UNIT *__fastcall MonsterSpawnWithArg(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY, int nClass,
	DWORD dwMode, DWORD dwUnitId, int nRadius,
	WORD wFlags)
{
	MONSPAWNCTX tCtx;

	tCtx.dwUnitId = dwUnitId;
	tCtx.ptArea = NULL;
	tCtx.nClass = nClass;
	tCtx.dwMode = dwMode;
	tCtx.hRoom = hRoom;
	tCtx.nRadius = nRadius;
	tCtx.wFlags = wFlags;
	tCtx.ptGame = ptGame;
	tCtx.nX = nX;
	tCtx.nY = nY;
	return MonsterSpawnAround(&tCtx);
}

UNIT *__fastcall MonsterSpawnFromPopulation(GAME *ptGame, UNIT *hUnit, int nIndex,
	DWORD dwMode, int nRadius, WORD wFlags)
{
	MONSPAWNCTX tCtx;
	GAMEROOM *hRoom;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!D2CommonDungeonRoomGetLevelIdChecked(hRoom))
		return NULL;
	tCtx.dwUnitId = 0;
	tCtx.nClass = (int)D2CommonMonsterResolvePopulationIndex(hRoom, nIndex);
	tCtx.dwMode = dwMode;
	tCtx.hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	tCtx.ptGame = ptGame;
	tCtx.ptArea = NULL;
	tCtx.nRadius = nRadius;
	tCtx.wFlags = wFlags;
	tCtx.nX = (int)D2CommonUnitGetX(hUnit);
	tCtx.nY = (int)D2CommonUnitGetY(hUnit);
	return MonsterSpawnAround(&tCtx);
}

BOOL __fastcall MonsterSpawnMinions(GAME *ptGame, UNIT *hLeader, int nClass,
	DWORD dwMode, int nRadius, int nCount,
	WORD wFlags)
{
	UNIT *hMinion;
	DWORD dwLeaderId;
	BOOL bSpawned;
	int i;

	bSpawned = FALSE;
	if (!hLeader)
		return FALSE;
	D2CommonUnitGetPathRoom(hLeader);
	dwLeaderId = hLeader->dwId;
	for (i = nCount; i > 0; i--)
	{
		hMinion = MonsterPlaceNear(ptGame, hLeader, nClass, dwMode, nRadius, wFlags);
		if (!hMinion)
			continue;
		bSpawned = TRUE;
		AiSetOwner(ptGame, hMinion, dwLeaderId, 1, 0, 0);
		AiAddMinion(ptGame, hLeader, hMinion);
	}
	return bSpawned;
}

BOOL __fastcall MonsterSpawnFormation(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY,
	UNIT *hOwner,
	int nClass, DWORD dwMode, const FORMATIONPOINT *ptFormation,
	WORD wFlags)
{
	UNIT *hLeader, *hMonster;
	GAMEROOM *hLeaderRoom;
	DWORD dwLeaderId;
	BOOL bSpawned, bTest, bStepClass;
	int nCount, nKind, nPointX, nPointY, i;

	bSpawned = FALSE;
	hLeader = NULL;
	bTest = FALSE;
	bStepClass = FALSE;
	if (ptFormation->field_08 & 1)
		bStepClass = TRUE;
	if (wFlags & 1)
		bTest = TRUE;
	nCount = ptFormation->nX;
	for (i = 0; i < nCount; i++)
	{
		ptFormation++;
		if (bTest)
			nKind = 4;
		else
			nKind = ptFormation->field_08;
		switch (nKind)
		{
		case 1:
			hMonster = MonsterPlaceAt(ptGame, hRoom, ptFormation->nX + nX, ptFormation->nY + nY, nClass, dwMode, 0);
			if (hMonster)
				AiSetOwner(ptGame, hMonster, hMonster->dwId, 1, 1, 0);
			hLeader = hMonster;
			if (hMonster)
				bSpawned = TRUE;
			break;
		case 2:
			hMonster = MonsterPlaceAt(ptGame, hRoom, ptFormation->nX + nX, ptFormation->nY + nY, nClass, dwMode, 0);
			if (hMonster)
				AiSetOwner(ptGame, hMonster, hMonster->dwId, 1, 1, 1);
			hLeader = hMonster;
			if (hMonster)
				bSpawned = TRUE;
			break;
		case 5:
			if (!hOwner)
				break;
			hMonster = MonsterPlaceAt(ptGame, hRoom, ptFormation->nX + nX, ptFormation->nY + nY, nClass, dwMode, 0);
			if (!hMonster)
				break;
			AiSetOwner(ptGame, hMonster, hOwner->dwId, hOwner->eType, 0, 0);
			bSpawned = TRUE;
			break;
		case 3:
			if (!hLeader)
				break;
			nPointY = ptFormation->nY + nY;
			nPointX = ptFormation->nX + nX;
			hLeaderRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hLeader);
			dwLeaderId = hLeader->dwId;
			hMonster = MonsterPlaceAt(ptGame, hLeaderRoom, nPointX, nPointY, nClass, dwMode, 0);
			if (!hMonster)
				break;
			AiSetOwner(ptGame, hMonster, dwLeaderId, 1, 0, 0);
			AiAddMinion(ptGame, hLeader, hMonster);
			bSpawned = TRUE;
			break;
		case 0:
			if (MonsterPlaceAt(ptGame, hRoom, ptFormation->nX + nX, ptFormation->nY + nY, nClass, dwMode, wFlags))
				bSpawned = TRUE;
			break;
		case 4:
			bSpawned = TRUE;
			if (!MonsterPlaceAt(ptGame, hRoom, ptFormation->nX + nX, ptFormation->nY + nY, nClass, dwMode, wFlags))
				return FALSE;
			break;
		}
		if (bStepClass)
			nClass++;
	}
	return bSpawned;
}

BOOL __fastcall MonsterSpawnFormationAt(GAME *ptGame, GAMEROOM *hRoom, DWORD dwUnused1,
	DWORD dwUnused2, UNIT *hUnit, int nClass, WORD wFlags)
{
	int nX, nY;
	BOOL bRet;

	bRet = FALSE;
	if (nClass == 340)
	{
		if (!hUnit)
			return bRet;
		nX = (int)D2CommonUnitGetX(hUnit);
		nY = (int)D2CommonUnitGetY(hUnit);
		bRet = MonsterSpawnFormation(ptGame, hRoom, nX, nY, NULL, 340, 8, sgatUniqueFormation154, wFlags);
	}
	return bRet;
}

BOOL __fastcall MonsterUniqueSpawnGroup(GAME *ptGame, UNIT *hLeader, int nClass,
	DWORD dwMode, int nCount, int nKind,
	WORD wFlags)
{
	GAMEROOM *hRoom;
	int nSpotX, nSpotY;
	UNIT *hMinion;
	DWORD dwLeaderId;
	BOOL bSpawned;
	int nX, nY, nIndex, i;

	if (!hLeader)
		return FALSE;
	if (nKind < 0 || nKind > 2)
		nKind = 0;
	bSpawned = FALSE;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hLeader);
	dwLeaderId = hLeader->dwId;
	nX = (int)D2CommonUnitGetX(hLeader);
	nY = (int)D2CommonUnitGetY(hLeader);
	nIndex = (int)D2CommonSeedRand(&hLeader->tSeed, 6);
	for (i = nCount; i > 0; i--)
	{
		nSpotX = sgatUniqueGroupOffsets[nKind][nIndex].nX + nX;
		nSpotY = sgatUniqueGroupOffsets[nKind][nIndex].nY + nY;
		nIndex = (nIndex + 5) % 6;
		hMinion = MonsterPlaceAt(ptGame, hRoom, nSpotX, nSpotY, nClass, dwMode, wFlags);
		if (!hMinion)
			continue;
		bSpawned = TRUE;
		AiSetOwner(ptGame, hMinion, dwLeaderId, 1, 0, 0);
		AiAddMinion(ptGame, hLeader, hMinion);
	}
	return bSpawned;
}
