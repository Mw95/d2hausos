#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\MONSTER\\MonsterUnique.cpp"

#include <string.h>
#include "../D2game.h"
#include "D2Macros.h"
#include "MonsterUnique.h"
#include "MonsterMode.h"
#include "MonsterMsg.h"
#include "../GAME/Event.h"
#include "../ITEMS/Items.h"
#include "../ITEMS/ItemMode.h"
#include "../MISSILES/Missiles.h"
#include "../OBJECTS/objrgn.h"
#include "../PLAYER/PlayerPets.h"
#include "../QUESTS/QUESTS.h"
#include "../QUESTS/a3q5.h"
#include "../QUESTS/QuestsFX.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "../AI/AiGeneral.h"
#include "../AI/AiThink.h"
#include "../GUILDS/Guilds.h"
#include "../SKILLS/Skills.h"
#include "../SKILLS/SkillSor.h"
#include "FogImports.h"

#define UNIT_MONSTER		1
#define UNIT_OBJECT			2
#define UNIT_MISSILE		3

#define MODE_MISSILEFIRE	5
#define UNIQUE_MAX_MODS		9
#define NUM_DIFFICULTY_LEVELS	3
#define NUM_UNIQUE_MODS		32
#define NUM_UNIQUE_MODTYPES	14
#define NUM_MODE_CALLS		6
#define gdwBitMasks			FoggdwBitMasks

struct MONSTERUNIQUE
{
 	WORD		wUniqueId;
 	BYTE		unobserved_02[2];
 	DWORD		dwUnique;
 	DWORD		dwSuperUnique;
 	DWORD		dwChampion;
 	DWORD		dwRegionCounted;
 	DWORD		dwMinion;
 	int			nBurstFrame;
 	WORD		wFlags;
 	BYTE		unobserved_1e[2];
 	int			aeMods[UNIQUE_MAX_MODS];
};

#define MONSTERDATA7(hUnit)	((MONSTERDATA *)(hUnit)->uTypeData)

struct MONREGIOND2GAME7
{
 	BYTE		unobserved_000[296];
 	int			nUniques;
};

struct SUPERUNIQUED2GAME7
{
 	BYTE		unobserved_00[180];
 	DWORD		dwClass;
 	int			aeMods[3];
 	int			nMinGrp;
 	int			nMaxGrp;
};

typedef void (__fastcall *PFNUNIQUEMODMODE)(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
typedef void (__fastcall *PFNUNIQUEMODINIT)(UNIT *hUnit, int eMod, BOOL fCount);
typedef BOOL (__fastcall *PFNUNIQUEMODTYPESETUP)(UNIT *hUnit, int *peMod);

struct UNIQUESHRINE
{
	DWORD		dwClass;
	int			nMin;
	int			nMax;
};
static const int sganUniqueDamageStats[5][3] =
{
	{ 48, 49, -1 }, { 50, 51, -1 }, { 52, 53, -1 }, { 54, 55, 56 }, { 57, 58, 59 },
};
static const struct UNIQUESHRINE sgatUniqueShrines[6] =
{
	{ 136, 2, 6 }, { 136, 7, 7 }, { 136, 8, 11 }, { 136, 12, 12 }, { 136, 1, 5 }, { 136, 14, 14 },
};
static DWORD sgadwUniqueModInherit[NUM_UNIQUE_MODS] =
{
	1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0,
};
struct UNIQUEMODSKILL
{
	int			nLevel;
	int			nShift;
	int			nSkill;
};
static struct UNIQUEMODSKILL sgatUniqueMod30Skills[8] =
{
	{ 0, 2, 98 }, { 0, 1, 102 }, { 0, 0, 108 }, { 0, 1, 114 }, { 0, 1, 118 }, { 0, 2, 123 }, { 0, 2,
		122 },
	{ 999, 2, 103 },
};

static inline DWORD MonsterUniqueRandom(D2SEED *ptSeed, DWORD dwMax)
{
	if (dwMax)
		return D2CommonSeedRand(ptSeed, dwMax);
	else
		return 0;
}

struct MONSTERUNIQUE *__fastcall MonsterUniqueAlloc(GAME *ptGame)
{
	struct MONSTERUNIQUE *ptUnique;
	DWORD i;

#line 79
	ptUnique = (struct MONSTERUNIQUE *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(struct MONSTERUNIQUE));
	for (i = 0; i < sizeof(struct MONSTERUNIQUE) / sizeof(DWORD); i++)
		((DWORD *)ptUnique)[i] = 0;
	return ptUnique;
}

void __fastcall MonsterUniqueFree(GAME *ptGame, struct MONSTERUNIQUE *ptUnique)
{
	if (ptUnique)
	{
#line 88
		D2FREEPOOL(ptGame->pMemPool, ptUnique);
	}
}

int *__fastcall MonsterUniqueGetMods(struct MONSTERUNIQUE *ptUnique)
{
	return ptUnique->aeMods;
}

WORD __fastcall MonsterUniqueGetId(struct MONSTERUNIQUE *ptUnique)
{
	return ptUnique->wUniqueId;
}

DWORD __fastcall MonsterUniqueIsChampion(struct MONSTERUNIQUE *ptUnique)
{
	return ptUnique->dwChampion;
}

DWORD __fastcall MonsterUniqueIsSuperUnique(struct MONSTERUNIQUE *ptUnique)
{
	return ptUnique->dwSuperUnique;
}

DWORD __fastcall MonsterUniqueIsCounted(struct MONSTERUNIQUE *ptUnique)
{
	return ptUnique->dwRegionCounted;
}

DWORD __fastcall MonsterUniqueIsUnique(struct MONSTERUNIQUE *ptUnique)
{
	return ptUnique->dwUnique;
}

DWORD __fastcall MonsterUniqueIsMinion(struct MONSTERUNIQUE *ptUnique)
{
	return ptUnique->dwMinion;
}

BOOL __fastcall MonsterUniqueHasMods(struct MONSTERUNIQUE *ptUnique)
{
	return ptUnique->aeMods[0] != 0;
}

DWORD __fastcall MonsterUniqueGetBurstFlag(struct MONSTERUNIQUE *ptUnique)
{
	return ptUnique->wFlags & 2;
}

void __fastcall MonsterUniqueModInitId(UNIT *hUnit, int eMod, BOOL fCount)
{
	GAMERECT atRects[2];
	GAME *ptGame;

	if (!fCount)
		return;
	ptGame = SUnitGetGame(hUnit);
	DWORD tNest = (DWORD)D2CommonUnitGetPathRoom(hUnit);
	D2CommonDungeonRoomCopyRects(tNest, atRects);
	MONSTERDATA7(hUnit)->ptUnique->wUniqueId =
		(WORD)((ptGame->nFrame + atRects[0].nX + atRects[0].nY) % 65493);
}

void __fastcall MonsterUniqueModInitLife(UNIT *hUnit, int eMod, BOOL fCount)
{
	int nLife;

	nLife = D2CommonStatsUnitGetValue(hUnit, 6);
	if (!fCount)
		nLife <<= 1;
	else
	{
		if (MONSTERDATA7(hUnit)->ptUnique->dwChampion)
			nLife <<= 2;
		else
			nLife <<= 3;
		D2CommonStatsUnitSetValue(hUnit, 74, 0);
	}
	D2CommonStatsUnitSetValue(hUnit, 7, nLife);
	D2CommonStatsUnitSetValue(hUnit, 6, nLife);
}

void __fastcall MonsterUniqueModInitLevel(UNIT *hUnit, int eMod, BOOL fCount)
{
	if (!fCount)
		return;
	D2CommonStatsUnitSetValue(hUnit, 12, (DWORD)D2CommonStatsUnitGetValue(hUnit, 12) + 3);
	D2CommonStatsUnitSetValue(hUnit, 13, (DWORD)D2CommonStatsUnitGetBaseValue(hUnit, 13) * 5);
}

void __fastcall MonsterUniqueModInitChampion(UNIT *hUnit, int eMod, BOOL fCount)
{
	DWORD dwExperience;

	if (fCount)
	{
		D2CommonStatsUnitSetValue(hUnit, 12, (DWORD)D2CommonStatsUnitGetValue(hUnit, 12) - 1);
		dwExperience = D2CommonStatsUnitGetBaseValue(hUnit, 13);
		D2CommonStatsUnitSetValue(hUnit, 13, dwExperience - dwExperience / 5 * 2);
	}
}

void __fastcall MonsterUniqueModInitResist(UNIT *hUnit, int eMod, BOOL fCount)
{
	int nFire, nLight, nCold;

	if (!fCount)
		return;
	if (eMod == 8)
	{
		D2CommonStatsUnitSetValue(hUnit, 39, 75);
		D2CommonStatsUnitSetValue(hUnit, 41, 75);
		D2CommonStatsUnitSetValue(hUnit, 43, 75);
	}
	else if (eMod == 9)
		D2CommonStatsUnitSetValue(hUnit, 39, 75);
	else if (eMod == 18)
		D2CommonStatsUnitSetValue(hUnit, 43, 75);
	else if (eMod == 17)
		D2CommonStatsUnitSetValue(hUnit, 41, 75);
	else if (eMod == 23)
		D2CommonStatsUnitSetValue(hUnit, 45, 75);
	else if (eMod == 25)
		D2CommonStatsUnitSetValue(hUnit, 37, (DWORD)D2CommonStatsUnitGetValue(hUnit, 37) + 20);
	else if (eMod == 27)
	{
		nFire = D2CommonStatsUnitGetValue(hUnit, 39);
		nLight = D2CommonStatsUnitGetValue(hUnit, 41);
		nCold = D2CommonStatsUnitGetValue(hUnit, 43) + 20;
		D2CommonStatsUnitSetValue(hUnit, 39, nFire + 20);
		D2CommonStatsUnitSetValue(hUnit, 41, nLight + 20);
		D2CommonStatsUnitSetValue(hUnit, 43, nCold);
	}
	else if (eMod == 28)
	{
		nFire = (DWORD)D2CommonStatsUnitGetValue(hUnit, 36) + 50;
		nLight = (DWORD)D2CommonStatsUnitGetValue(hUnit, 31) * 2;
		D2CommonStatsUnitSetValue(hUnit, 36, nFire);
		D2CommonStatsUnitSetValue(hUnit, 31, nLight);
	}
}

void __fastcall MonsterUniqueModInitSkill(UNIT *hUnit, int eMod, BOOL fCount)
{
	if (!fCount)
		return;
	SkillsSetLevel(hUnit, SKILL_MONTELEPORT, 1, 1);
	D2CommonSkillsSetNodeMode(D2CommonSkillsFindByKey(hUnit, 184), 4);
	AiSetMonTeleportFlag(hUnit, TRUE);
}

void __fastcall MonsterUniqueModInitAura(UNIT *hUnit, int eMod, BOOL fCount)
{
	D2SEED tSeed;
	WORD dwUniqueId;
	int nLevel, nCount, nIndex, i;

	if (!fCount)
		return;
	nLevel = D2CommonStatsUnitGetValue(hUnit, 12);
	if (!nLevel)
		nLevel = 1;
	nCount = 0;
	for (i = 0; i < 8; i++)
		if (sgatUniqueMod30Skills[i].nLevel <= nLevel)
			nCount++;
	if (!nCount)
		nCount = 1;
	dwUniqueId = MONSTERDATA7(hUnit)->ptUnique->wUniqueId;
	D2CommonSeedInit(&tSeed);
	D2CommonSeedSet(&tSeed, dwUniqueId);
	nIndex = (int)D2CommonSeedRand(&tSeed, nCount);
	if ((int)dwUniqueId - 65493 == 37)
		nIndex = 5;
	SkillsSetLevel(hUnit, sgatUniqueMod30Skills[nIndex].nSkill,
		nLevel >> sgatUniqueMod30Skills[nIndex].nShift, 1);
	SkillsSetCurrent(hUnit, FALSE, sgatUniqueMod30Skills[nIndex].nSkill);
}

BOOL __fastcall MonsterUniqueTypeSetupStrong(UNIT *hUnit, int *peMod)
{
	*peMod = 5;
	return TRUE;
}

BOOL __fastcall MonsterUniqueTypeSetupFast(UNIT *hUnit, int *peMod)
{
	*peMod = 6;
	return TRUE;
}

BOOL __fastcall MonsterUniqueTypeSetupCursed(UNIT *hUnit, int *peMod)
{
	*peMod = 7;
	return TRUE;
}

BOOL __fastcall MonsterUniqueTypeSetupResistant(UNIT *hUnit, int *peMod)
{
	if (!D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hUnit))))
		return FALSE;
	*peMod = 8;
	return TRUE;
}

BOOL __fastcall MonsterUniqueTypeSetupFire(UNIT *hUnit, int *peMod)
{
	*peMod = 9;
	return TRUE;
}

BOOL __fastcall MonsterUniqueTypeSetupLightning(UNIT *hUnit, int *peMod)
{
	if (D2CommonMonsterUnitGetTypeLocalIndex(hUnit) == 78)
		return FALSE;
	*peMod = 17;
	return TRUE;
}

BOOL __fastcall MonsterUniqueTypeSetupCold(UNIT *hUnit, int *peMod)
{
	*peMod = 18;
	return TRUE;
}

BOOL __fastcall MonsterUniqueTypeSetupManaBurn(UNIT *hUnit, int *peMod)
{
	*peMod = 25;
	return TRUE;
}

BOOL __fastcall MonsterUniqueTypeSetupSpectral(UNIT *hUnit, int *peMod)
{
	*peMod = 27;
	return TRUE;
}

BOOL __fastcall MonsterUniqueTypeSetupTeleport(UNIT *hUnit, int *peMod)
{
	if (!((MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId))->bModeA1)
		return FALSE;
	*peMod = 26;
	return TRUE;
}

BOOL __fastcall MonsterUniqueTypeSetupStoneSkin(UNIT *hUnit, int *peMod)
{
	*peMod = 28;
	return TRUE;
}

BOOL __fastcall MonsterUniqueTypeSetupShots(UNIT *hUnit, int *peMod)
{
	DWORD dwClass;
	int nType;

	dwClass = hUnit->dwClassId;
	nType = (int)D2CommonMonsterUnitGetTypeLocalIndex(hUnit);
	if (dwClass == 395)
		return FALSE;
	if (nType == 131)
		return FALSE;
	if (nType == 110)
		return FALSE;
	if (((MONSTATSTXT *)D2CommonMonsterTblsGetStats(dwClass))->bIsMelee)
		return FALSE;
	*peMod = 29;
	return TRUE;
}

BOOL __fastcall MonsterUniqueTypeSetupAura(UNIT *hUnit, int *peMod)
{
	*peMod = 30;
	return TRUE;
}

void __fastcall MonsterUniqueStrongOnMode(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	if (dwArg)
		MonsterUniqueAddDamage(ptGame, hUnit, 21, 22, -1, 1);
}

void __fastcall MonsterUniqueAddDamage(GAME *ptGame, UNIT *hUnit, int nMinStat,
	int nMaxStat, int nLengthStat, int nMultiply)
{
	void *ptStatList;
	int nMin, nMax, nToHit, nNewMin, nNewMax;

	nMin = D2CommonStatsUnitGetValue(hUnit, 21);
	nMax = D2CommonStatsUnitGetValue(hUnit, 22);
	nToHit = D2CommonStatsUnitGetValue(hUnit, 19);
	if (nMax <= 0)
		return;
	if (nMax < 1)
		nMax = 1;
	if (nMinStat == 57)
	{
		nNewMin = (DWORD)D2CommonStatsUnitGetValue(hUnit, nMinStat) + (nMin >> 4);
		nNewMax = D2CommonStatsUnitGetValue(hUnit, nMaxStat);
		nMax >>= 4;
		nNewMax += nMax;
	}
	else
	{
		nNewMin = (DWORD)D2CommonStatsUnitGetValue(hUnit, nMinStat) + nMin * nMultiply;
		nNewMax = D2CommonStatsUnitGetValue(hUnit, nMaxStat);
		nMax *= nMultiply;
		nNewMax += nMax;
	}
	ptStatList = NULL;
	nToHit *= 2;
	if (UnitGetType(hUnit) == UNIT_MONSTER)
	{
		ptStatList = (void *)D2CommonStatsFindListByMask(hUnit, 1);
		D2CommonStatsListSetValueB(ptStatList, nMinStat, nNewMin);
		D2CommonStatsListSetValueB(ptStatList, nMaxStat, nNewMax);
		D2CommonStatsListSetValueB(ptStatList, 19, nToHit);
	}
	else
	{
		D2CommonStatsUnitSetValue(hUnit, nMinStat, nNewMin);
		D2CommonStatsUnitSetValue(hUnit, nMaxStat, nNewMax);
		D2CommonStatsUnitSetValue(hUnit, 19, nToHit);
	}
	if (nLengthStat == -1)
		return;
	nNewMin = (DWORD)D2CommonStatsUnitGetValue(hUnit, nLengthStat) + 40;
	if (UnitGetType(hUnit) == UNIT_MONSTER)
		D2CommonStatsListSetValueB(ptStatList, nLengthStat, nNewMin);
	else
		D2CommonStatsUnitSetValue(hUnit, nLengthStat, nNewMin);
}

void __fastcall MonsterUniqueStrongOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg)
{
	if (dwArg && UnitGetType(hMissile) == UNIT_MISSILE)
		MonsterUniqueAddDamage(ptGame, hMissile, 21, 22, -1, 1);
}

void __fastcall MonsterUniqueFastOnStart(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	MONSTATSTXT *ptStats;
	void *ptStatList;
	int nVelocity;

	nVelocity = 0;
	ptStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId);
	if (ptStats->nVelocity)
	{
		nVelocity = (int)(2048 / (DWORD)ptStats->nVelocity) - 128;
		if (nVelocity < 0)
			nVelocity = 10;
		else if (nVelocity > 60)
			nVelocity = 60;
	}
	D2CommonStatsUnitFreeStateLists(hUnit);
	ptStatList = (void *)D2CommonStatsAllocList(ptGame->pMemPool, 4, 0, 1, hUnit->dwId);
	D2CommonStatsListSetValueA(ptStatList, 67, nVelocity);
	D2CommonStatsUnitLinkList(hUnit, ptStatList);
	if (dwArg)
	{
		ptStatList = (void *)D2CommonStatsAllocList(ptGame->pMemPool, 4, 0, 1, hUnit->dwId);
		D2CommonStatsListSetValueA(ptStatList, 68, 100);
		D2CommonStatsUnitLinkList(hUnit, ptStatList);
	}
	D2CommonUnitUpdateAnimRate(hUnit);
}

void __fastcall MonsterUniqueCursedOnHit(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	int nLevel;

	if (!dwArg)
		return;
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < 50)
		return;
	nLevel = (int)D2CommonStatsUnitGetValue(hUnit, 12) >> 2;
	if (!nLevel)
		nLevel = 1;
	SkillsCurseArea(ptGame, 3, hUnit, nLevel, SKILL_NECROMANCER_START,
		MonsterUniqueCursedOnTarget);
}

void __fastcall MonsterUniqueCursedOnTarget(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel)
{
	SKILLSTATE tHit;
	DWORD i;

	if (!SUnitIsHostile(ptGame, hUnit, hTarget))
		return;
	for (i = 0; i < sizeof(tHit) / sizeof(DWORD); i++)
		((DWORD *)&tHit)[i] = 0;
	tHit.hSource = hUnit;
	tHit.hTarget = hTarget;
	tHit.nSkill = 66;
	tHit.nLevel = nLevel;
	tHit.nDuration = D2CommonSkillsCalcParamThreeByLevel(SKILL_NECROMANCER_START, nLevel);
	tHit.dwStatId = 36;
	tHit.nStatValue = -(int)D2CommonSkillsGetTypeParamFive(SKILL_NECROMANCER_START);
	tHit.dwState = 9;
	tHit.pfnRemove = 0;
	SkillsSetState(&tHit);
}

void __fastcall MonsterUniqueFireOnMode(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	if (dwArg)
		MonsterUniqueAddDamage(ptGame, hUnit, 48, 49, -1, 1);
}

void __fastcall MonsterUniqueFireOnStart(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	if (dwArg && !hUnit->dwMode)
		EventAdd(ptGame, hUnit, 7, ptGame->nFrame + 4);
}

void __fastcall MonsterUniqueFireOnEvent(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	DAMAGE tDamage;
	UNIT *hMissile;
	DWORD i;
	int nX, nY, nLife, nMin, nDamage;

	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	hMissile = SkillsCreateMissile(ptGame, MISSILE_MONSTERCORPSEEXPLODE, hUnit, 1, 0, 0, nX,
		nY, 1);
	if (!hMissile)
		return;
	nLife = (MONSTERDATA7(hUnit)->ptMonStats->MaxHP[0] << 11) >> 1;
	nMin = nLife * 60 / 100;
	nDamage = (int)MonsterUniqueRandom(&hUnit->tSeed, nLife - nMin) + nMin;
	for (i = 0; i < sizeof(tDamage) / sizeof(DWORD); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.nPhysicalDamage = nDamage >> 3;
	tDamage.nFireDamage = nDamage >> 3;
	SUnitDmgSplash(ptGame, hMissile, nX, nY, ptGame->bDifficultyLevel + 4, &tDamage, FALSE, FALSE, NULL,
		1409);
}

void __fastcall MonsterUniqueFireOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg)
{
	if (dwArg && UnitGetType(hMissile) == UNIT_MISSILE)
		MonsterUniqueAddDamage(ptGame, hMissile, 48, 49, -1, 1);
}

void __fastcall MonsterUniqueExplodeOnEvent(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	DAMAGE tDamage;
	UNIT *hMissile;
	DWORD i;
	int nX, nY;

	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	hMissile = SkillsCreateMissile(ptGame, MISSILE_MONSTERCORPSEEXPLODE, hUnit, 1, 0, 0, nX,
		nY, 1);
	if (!hMissile)
		return;
	for (i = 0; i < sizeof(tDamage) / sizeof(DWORD); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.nPhysicalDamage = 25600;
	tDamage.nFireDamage = 25600;
	SUnitDmgSplash(ptGame, hMissile, nX, nY, 6, &tDamage, FALSE, FALSE, NULL, 1411);
}

void __fastcall MonsterUniqueChampionOnMode(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	void *ptStatList;
	int nMin, nMax, nToHit;

	if (!dwArg)
		return;
	nMin = D2CommonStatsUnitGetValue(hUnit, 21);
	nMax = D2CommonStatsUnitGetValue(hUnit, 22);
	nToHit = D2CommonStatsUnitGetValue(hUnit, 19);
	if (nMax <= 0)
		return;
	if (nMax < 1)
		nMax = 1;
	nToHit *= 2;
	nMax *= 2;
	if (UnitGetType(hUnit) == UNIT_MONSTER)
	{
		ptStatList = (void *)D2CommonStatsFindListByMask(hUnit, 1);
		D2CommonStatsListSetValueB(ptStatList, 21, nMin);
		D2CommonStatsListSetValueB(ptStatList, 22, nMax);
		D2CommonStatsListSetValueB(ptStatList, 19, nToHit);
	}
	else
	{
		D2CommonStatsUnitSetValue(hUnit, 21, nMin);
		D2CommonStatsUnitSetValue(hUnit, 22, nMax);
		D2CommonStatsUnitSetValue(hUnit, 19, nToHit);
	}
}

void __fastcall MonsterUniqueChampionOnStart(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	void *ptStatList;

	if (!dwArg)
		return;
	D2CommonStatsUnitFreeStateLists(hUnit);
	ptStatList = (void *)D2CommonStatsAllocList(ptGame->pMemPool, 4, 0, 1, hUnit->dwId);
	D2CommonStatsListSetValueA(ptStatList, 67, 20);
	D2CommonStatsUnitLinkList(hUnit, ptStatList);
	D2CommonUnitUpdateAnimRate(hUnit);
}

void __fastcall MonsterUniqueChampionOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg)
{
	if (dwArg && UnitGetType(hMissile) == UNIT_MISSILE)
		MonsterUniqueChampionOnMode(ptGame, hMissile, eMod, dwArg);
}

void __fastcall MonsterUniqueLightningOnMode(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	if (dwArg)
		MonsterUniqueAddDamage(ptGame, hUnit, 50, 51, -1, 1);
}

void __fastcall MonsterUniqueLightningOnEvent(GAME *ptGame, UNIT *hUnit, int eMod,
	DWORD dwArg)
{
	struct MONSTERUNIQUE *ptUnique;
	MISSILECREATE tCreate;
	int anDX[4], anDY[4];
	int nFrames, nLevel, nX, nY, i, j;
	DWORD k;
	int nNow;

	if (!dwArg)
		return;
	ptUnique = MONSTERDATA7(hUnit)->ptUnique;
	nNow = ptGame->nFrame;
	nFrames = nNow - ptUnique->nBurstFrame;
	if (nFrames < 0)
		nFrames = -nFrames;
	if (nFrames < 10)
	{
		ptUnique->wFlags &= ~2;
		return;
	}
	ptUnique->wFlags |= 2;
	ptUnique->nBurstFrame = nNow;
	nLevel = (int)D2CommonStatsUnitGetValue(hUnit, 12) >> 1;
	if (!nLevel)
		nLevel = 1;
	for (k = 0; k < sizeof(tCreate) / sizeof(DWORD); k++)
		((DWORD *)&tCreate)[k] = 0;
	tCreate.dwFlags = 33;
	tCreate.hSource = hUnit;
	tCreate.nX = (int)D2CommonUnitGetX(hUnit);
	tCreate.nY = (int)D2CommonUnitGetY(hUnit);
	tCreate.nLevel = nLevel;
	tCreate.eMissile = 195;
	tCreate.pfnModify = SkillsChargedBoltModify;
	anDX[0] = 0;
	anDX[1] = 1;
	anDX[2] = 0;
	anDX[3] = -1;
	anDY[0] = -1;
	anDY[1] = 0;
	anDY[2] = 1;
	anDY[3] = 0;
	for (i = 0; i < 4; i++)
	{
		nX = (int)D2CommonUnitGetX(hUnit) + anDX[i];
		nY = (int)D2CommonUnitGetY(hUnit) + anDY[i];
		for (j = 0; j < 2; j++)
		{
			tCreate.dwModifyArg = j;
			tCreate.nDX = nX;
			tCreate.nDY = nY;
			MissilesCreate(ptGame, &tCreate);
		}
	}
}

void __fastcall MonsterUniqueLightningOnGetHit(GAME *ptGame, UNIT *hUnit, int eMod,
	DWORD dwArg)
{
	if (dwArg && hUnit->dwMode != 3)
		MonsterUniqueLightningOnEvent(ptGame, hUnit, eMod, dwArg);
}

void __fastcall MonsterUniqueLightningOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg)
{
	if (dwArg && UnitGetType(hMissile) == UNIT_MISSILE)
		MonsterUniqueAddDamage(ptGame, hMissile, 50, 51, -1, 1);
}

void __fastcall MonsterUniqueColdOnMode(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	if (dwArg)
		MonsterUniqueAddDamage(ptGame, hUnit, 54, 55, 56, 1);
}

void __fastcall MonsterUniqueColdOnEvent(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	DWORD dwVelocity;

	if (!dwArg)
		return;
	dwVelocity = D2CommonMissileTblsGetVel(MISSILE_FROSTNOVA);
	SkillsNova(ptGame, hUnit, hUnit, MISSILE_COLDUNIQUE, (int)D2CommonStatsUnitGetValue(hUnit, 12) >> 1,
		dwVelocity);
}

void __fastcall MonsterUniqueColdOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg)
{
	if (dwArg && UnitGetType(hMissile) == UNIT_MISSILE)
		MonsterUniqueAddDamage(ptGame, hMissile, 54, 55, -1, 1);
}

void __fastcall MonsterUniquePoisonCloudOnEvent(GAME *ptGame, UNIT *hUnit, int eMod,
	DWORD dwArg)
{
	int nX, nY, nLevel;

	SkillsCreateMissile(ptGame, MISSILE_CORPSEPOISONCLOUD, hUnit, D2CommonStatsUnitGetValue(hUnit, 12), 0, 0, ((int)D2CommonUnitGetX(hUnit)), ((int)D2CommonUnitGetY(hUnit)), 1);
}

void __fastcall MonsterUniqueGroupDeathOnStart(GAME *ptGame, UNIT *hUnit, int eMod,
	DWORD dwArg)
{
	MONMODECHANGE tArgs;
	UNIT *hOwner;

	if (hUnit->dwMode)
		return;
	hOwner = AiGetOwner(hUnit);
	if (!hOwner)
		return;
	AiForEachMinion(hUnit, (DWORD)ptGame, 0, (PFNAIMINIONFN)MonsterUniqueGroupDeathOnMinion);
	AiSetOwner(ptGame, hOwner, (DWORD)-1, 1, 0, 0);
	if (hOwner == hUnit)
		return;
	MonsterModeInit(hOwner, 0, &tArgs);
	MonsterModeSet(ptGame, &tArgs, 1);
}

void __fastcall MonsterUniqueGroupDeathOnMinion(UNIT *hMinion, GAME *ptGame, DWORD dwArg)
{
	MONMODECHANGE tArgs;

	AiRemoveFromOwner(hMinion);
	AiSetOwner(ptGame, hMinion, (DWORD)-1, 1, 0, 0);
	if (!hMinion->dwMode)
		return;
	MonsterModeInit(hMinion, 0, &tArgs);
	MonsterModeSet(ptGame, &tArgs, 1);
}

void __fastcall MonsterUniqueHirelingOnMode(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	MONSTATSTXT *ptStats;
	void *ptStatList;
	int nMin, nMax;

	nMin = D2CommonStatsUnitGetValue(hUnit, 21);
	nMax = D2CommonStatsUnitGetValue(hUnit, 22);
	if (nMax <= 0)
		return;
#line 1403
	D2ASSERT(UnitGetClass (hUnit) != MONSTERCLASS_ROGUEHIREABLE);
	ptStats = MONSTERDATA7(hUnit)->ptMonStats;
#line 1408
	D2ASSERT(ptStats->pnAttack1MinDamage[0] == 1);
#line 1409
	D2ASSERT(ptStats->pnAttack1MaxDamage[0] == 1);
	nMin = nMin + (DWORD)D2CommonStatsUnitGetValue(hUnit, 23) - 1;
	nMax = nMax + (DWORD)D2CommonStatsUnitGetValue(hUnit, 24) - 1;
	if (nMin < 0)
		nMin = 0;
	if (nMax < 0)
		nMax = 0;
	ptStatList = (void *)D2CommonStatsFindListByMask(hUnit, 1);
	D2CommonStatsListSetValueB(ptStatList, 21, nMin);
	D2CommonStatsListSetValueB(ptStatList, 22, nMax);
}

void __fastcall MonsterUniqueHirelingOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg)
{
	MONSTATSTXT *ptMonStats;
	MISSILESTXT *ptMisStats;
	UNIT *hOwner;
	int nMin, nMax;

	if (UnitGetType(hMissile) != UNIT_MISSILE)
		return;
	nMin = D2CommonStatsUnitGetValue(hMissile, 21);
	nMax = D2CommonStatsUnitGetValue(hMissile, 22);
	if (nMax <= 0)
		return;
	hOwner = SUnitGetOwner(ptGame, hMissile);
	if (!hOwner)
		return;
	if (UnitGetClass(hOwner) != MONSTERCLASS_ROGUEHIREABLE)
		return;
	ptMonStats = MONSTERDATA7(hOwner)->ptMonStats;
#line 1444
	D2ASSERT(ptMonStats->pnAttack1MinDamage[0] == 0);
#line 1445
	D2ASSERT(ptMonStats->pnAttack1MaxDamage[0] == 0);
	ptMisStats = (MISSILESTXT *)D2CommonMissileTblsGetMissile(D2CommonMissileGetClassId(hMissile));
#line 1448
	D2ASSERTMSG(ptMisStats->nMinDamage == 1, "ptMisStats->nMinDamage == 1");
#line 1449
	D2ASSERTMSG(ptMisStats->nMaxDamage == 1, "ptMisStats->nMaxDamage == 1");
	nMin = nMin + ((DWORD)D2CommonStatsUnitGetValue(hOwner, 23) << 8) - 256;
	nMax = ((DWORD)D2CommonStatsUnitGetValue(hOwner, 24) << 8) + nMax - 256;
	if (nMin < 0)
		nMin = 0;
	if (nMax < 0)
		nMax = 0;
	D2CommonStatsUnitSetValue(hMissile, 21, nMin);
	D2CommonStatsUnitSetValue(hMissile, 22, nMax);
}

void __fastcall MonsterUniqueBugLightningOnStart(GAME *ptGame, UNIT *hUnit, int eMod,
	DWORD dwArg)
{
	if (hUnit->dwMode == 3 || !hUnit->dwMode)
		EventAdd(ptGame, hUnit, 7, ptGame->nFrame + 2);
}

void __fastcall MonsterUniqueBugLightningOnEvent(GAME *ptGame, UNIT *hUnit, int eMod,
	DWORD dwArg)
{
	MISSILECREATE tCreate;
	int anDX[4], anDY[4];
	int nX, nY, i, j;
	DWORD k;

	for (k = 0; k < sizeof(tCreate) / sizeof(DWORD); k++)
		((DWORD *)&tCreate)[k] = 0;
	tCreate.dwFlags = 33;
	tCreate.hSource = hUnit;
	tCreate.nX = (int)D2CommonUnitGetX(hUnit);
	tCreate.nY = (int)D2CommonUnitGetY(hUnit);
	tCreate.nLevel = D2CommonStatsUnitGetValue(hUnit, 12);
	tCreate.eMissile = 225;
	tCreate.pfnModify = SkillsChargedBoltModify;
	anDX[0] = 0;
	anDX[1] = 1;
	anDX[2] = 0;
	anDX[3] = -1;
	anDY[0] = -1;
	anDY[1] = 0;
	anDY[2] = 1;
	anDY[3] = 0;
	for (i = 0; i < 4; i++)
	{
		nX = (int)D2CommonUnitGetX(hUnit) + anDX[i];
		nY = (int)D2CommonUnitGetY(hUnit) + anDY[i];
		for (j = 0; j < 2; j++)
		{
			tCreate.dwModifyArg = j;
			tCreate.nDX = nX;
			tCreate.nDY = nY;
			MissilesCreate(ptGame, &tCreate);
		}
	}
}

void __fastcall MonsterUniqueManaBurnOnMode(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	if (dwArg)
		MonsterUniqueAddDamage(ptGame, hUnit, 62, 63, -1, 4);
}

void __fastcall MonsterUniqueManaBurnOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg)
{
	if (dwArg && UnitGetType(hMissile) == UNIT_MISSILE)
		MonsterUniqueAddDamage(ptGame, hMissile, 62, 63, -1, 4);
}

void __fastcall MonsterUniqueSpectralOnMode(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	const int *pnStats;
	DWORD nRow;

	if (!dwArg)
		return;
	nRow = D2CommonSeedRand(&hUnit->tSeed, 5);
	MonsterUniqueAddDamage(ptGame, hUnit, sganUniqueDamageStats[nRow][0], sganUniqueDamageStats[nRow][1],
		sganUniqueDamageStats[nRow][2], 1);
}

void __fastcall MonsterUniqueSpectralOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg)
{
	DWORD nRow;

	if (!dwArg || UnitGetType(hMissile) != UNIT_MISSILE)
		return;
	nRow = D2CommonSeedRand(&hMissile->tSeed, 5);
	MonsterUniqueAddDamage(ptGame, hMissile, sganUniqueDamageStats[nRow][0], sganUniqueDamageStats[nRow][1],
		sganUniqueDamageStats[nRow][2], 1);
}

void __fastcall MonsterUniqueThiefOnHit(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	COLPOINT tCoord, tSpawn;
	GAMEROOM *hRoom;
	UNIT *hTarget, *hItem, *hCopy;
	int nSlot, i;
	void *pInv;

	if (!dwArg)
		return;
	if (D2CommonSeedRand(&hUnit->tSeed, 100) < 30)
		return;
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget || UnitGetType(hTarget) != UNIT_PLAYER)
		return;
	pInv = hTarget->ptInventory;
	for (i = 0; i < 16; i++)
	{
		hItem = (UNIT *)D2CommonINVENTORYGetBeltSlot(pInv, i);
		if (hItem)
			break;
	}
	if (!hItem)
		return;
	hCopy = ITEMSDuplicate(ptGame, hItem, hTarget);
	D2CommonUnitGetPosition(hTarget, &tCoord);
	hRoom = ITEMSFindDropSpot((GAMEROOM *)D2CommonUnitGetPathRoom(hTarget), &tCoord, &tSpawn, 1);
	if (!hRoom)
	{
#line 1606
		D2ASSERT(0);
	}
	D2CommonUnitInitPosition(0, hCopy, hRoom, tSpawn.nX, tSpawn.nY);
	D2CommonUnitSetCollision(hCopy, hRoom, tSpawn.nX, tSpawn.nY);
	D2CommonAddUnitToRoom(hCopy, hRoom);
	hCopy->dwFlags0 |= 2;
	D2CommonUnitSetMode(hCopy, 3);
	D2CommonITEMSDataSetFlags(hCopy, 8192, 1);
	hCopy->dwFlags0 |= 33554432;
	QuestsItemDropped(ptGame, hTarget, hCopy);
	nSlot = (int)D2CommonUnitGetX(hItem);
	ITEMSModeUseUpBeltItem(ptGame, hTarget, hItem);
	ITEMSModeBeltShiftDown(ptGame, hTarget, nSlot);
	hTarget->dwFlags |= 16777216;
}

void __fastcall MonsterUniqueQuestOnStart(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	switch (hUnit->dwClassId)
	{
	case 156:
		if (!hUnit->dwMode)
			QuestsFXOnAndarielDeath(ptGame, hUnit);
		break;
	case 267:
		if (!hUnit->dwMode)
			QuestsFXOnBloodravenDeath(ptGame, hUnit);
		break;
	case 229:
		if (!hUnit->dwMode)
			QuestsFXOnRadamentDeath(ptGame, hUnit);
		break;
	case 242:
		if (!hUnit->dwMode)
			QuestsFXOnMephistoDeath(ptGame, hUnit);
		break;
	case 256:
		if (!hUnit->dwMode)
			QuestsFXOnIzualDeath(ptGame, hUnit);
		break;
	}
}

void __fastcall MonsterUniqueQueenOnMode(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	MISSILECREATE tCreate;
	int anDX[4], anDY[4];
	int nX, nY, i, j;
	DWORD k;

	for (k = 0; k < sizeof(tCreate) / sizeof(DWORD); k++)
		((DWORD *)&tCreate)[k] = 0;
	tCreate.dwFlags = 33;
	tCreate.hSource = hUnit;
	tCreate.nX = (int)D2CommonUnitGetX(hUnit);
	tCreate.nY = (int)D2CommonUnitGetY(hUnit);
	tCreate.nLevel = D2CommonStatsUnitGetValue(hUnit, 12);
	tCreate.eMissile = 321;
	tCreate.pfnModify = SkillsChargedBoltModify;
	anDX[0] = 0;
	anDX[1] = 1;
	anDX[2] = 0;
	anDX[3] = -1;
	anDY[0] = -1;
	anDY[1] = 0;
	anDY[2] = 1;
	anDY[3] = 0;
	for (i = 0; i < 4; i++)
	{
		nX = (int)D2CommonUnitGetX(hUnit) + anDX[i];
		nY = (int)D2CommonUnitGetY(hUnit) + anDY[i];
		for (j = 0; j < 2; j++)
		{
			tCreate.dwModifyArg = j;
			tCreate.nDX = nX;
			tCreate.nDY = nY;
			MissilesCreate(ptGame, &tCreate);
		}
	}
}

void __fastcall MonsterUniqueQueenOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg)
{
	if (UnitGetType(hMissile) == UNIT_MISSILE)
		MonsterUniqueAddDamage(ptGame, hMissile, 57, 58, 59, 1);
}

void __fastcall MonsterUniqueShotsOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg)
{
	struct MONSTERUNIQUE *ptUnique;
	D2SEED tSeed;
	UNIT *hOwner, *hTarget;
	void *ptPath;
	int nMissile, nTargetX, nTargetY, nDX, nDY, nCount;

	if (!dwArg)
		return;
	if (UnitGetType(hMissile) != UNIT_MISSILE)
		return;
	nMissile = hMissile->dwClassId;
	hOwner = SUnitGetOwner(ptGame, hMissile);
	if (!hOwner)
		return;
	ptUnique = MONSTERDATA7(hOwner)->ptUnique;
	if (ptUnique->wFlags & 1)
		return;
	ptPath = hMissile->ptPath;
	hTarget = SUnitGetTarget(ptGame, hOwner);
	if (!hTarget)
	{
		nTargetX = (int)D2CommonPathGetTargetXRaw((DWORD)ptPath);
		nTargetY = (int)D2CommonPathGetTargetYRaw((DWORD)ptPath);
	}
	else
	{
		nTargetX = (int)D2CommonUnitGetX(hTarget);
		nTargetY = (int)D2CommonUnitGetY(hTarget);
	}
	D2CommonSeedInit(&tSeed);
	D2CommonSeedSet(&tSeed, 1397049683);
	tSeed.dwHigh = hOwner->dwId;
	nDX = (int)D2CommonUnitGetX(hOwner) - nTargetX;
	nDY = (int)D2CommonUnitGetY(hOwner) - nTargetY;
	if (nDX < 0)
		nDX = -1;
	else if (nDX > 0)
		nDX = 1;
	if (nDY < 0)
		nDY = -1;
	else if (nDY > 0)
		nDY = 1;
	if (nMissile >= 63 && nMissile <= 66)
	{
		nDY = 0;
		nDX = 0;
	}
	nCount = (int)D2CommonSeedRand(&tSeed, 2) + 1;
	ptUnique->wFlags |= 1;
	for (; nCount > 0; nCount--)
	{
		SkillsCreateMissile(ptGame, nMissile, hOwner, D2CommonMissileGetLevel(hMissile), 0, 0,
			nTargetX - nDY, nTargetY + nDX, 0);
		SkillsCreateMissile(ptGame, nMissile, hOwner, D2CommonMissileGetLevel(hMissile), 0, 0,
			nTargetX + nDY, nTargetY - nDX, 0);
		nDX += nDX;
		nDY += nDY;
	}
	ptUnique->wFlags &= ~1;
}

void __fastcall MonsterUniqueLightningOnStart(GAME *ptGame, UNIT *hUnit, int eMod,
	DWORD dwArg)
{
	if (dwArg && hUnit->dwMode == 3)
		EventAdd(ptGame, hUnit, 7, ptGame->nFrame + 2);
}

static PFNUNIQUEMODINIT sgpfnUniqueModInit[NUM_UNIQUE_MODS] =
{
	NULL, MonsterUniqueModInitId, MonsterUniqueModInitLife, NULL, MonsterUniqueModInitLevel, NULL, NULL, NULL,
	MonsterUniqueModInitResist, MonsterUniqueModInitResist, NULL, NULL, (PFNUNIQUEMODINIT)AiThinkNone, NULL,
		NULL, NULL,
	MonsterUniqueModInitChampion, MonsterUniqueModInitResist, MonsterUniqueModInitResist, NULL, NULL, NULL,
		NULL,
	MonsterUniqueModInitResist, NULL, MonsterUniqueModInitResist, MonsterUniqueModInitSkill,
	MonsterUniqueModInitResist, MonsterUniqueModInitResist, NULL, MonsterUniqueModInitAura, NULL,
};

static PFNUNIQUEMODTYPESETUP spfnUniqueModTypeSetup[NUM_UNIQUE_MODTYPES] =
{
	NULL, MonsterUniqueTypeSetupStrong, MonsterUniqueTypeSetupFast, MonsterUniqueTypeSetupCursed,
		MonsterUniqueTypeSetupResistant,
	MonsterUniqueTypeSetupFire, MonsterUniqueTypeSetupLightning, MonsterUniqueTypeSetupCold,
		MonsterUniqueTypeSetupManaBurn,
	MonsterUniqueTypeSetupSpectral, MonsterUniqueTypeSetupTeleport, MonsterUniqueTypeSetupStoneSkin,
		MonsterUniqueTypeSetupShots,
	MonsterUniqueTypeSetupAura,
};
static PFNUNIQUEMODMODE spfnUniqueModMode[NUM_UNIQUE_MODS][NUM_MODE_CALLS] =
{
	{ NULL, NULL, NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL, NULL, NULL },
	{ MonsterUniqueStrongOnMode, NULL, NULL, NULL, NULL, MonsterUniqueStrongOnMissile },
	{ NULL, MonsterUniqueFastOnStart, NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, MonsterUniqueCursedOnHit, NULL, NULL },
	{ NULL, NULL, NULL, NULL, NULL, NULL },
	{ MonsterUniqueFireOnMode, MonsterUniqueFireOnStart, MonsterUniqueFireOnEvent, NULL, NULL,
		MonsterUniqueFireOnMissile },
	{ NULL, MonsterUniqueModQueueEvent, MonsterUniquePoisonCloudOnEvent, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL, NULL, NULL },
	{ MonsterUniqueModInitStats, NULL, NULL, NULL, NULL, NULL },
	{ NULL, MonsterUniqueGroupDeathOnStart, NULL, NULL, NULL, NULL },
	{ MonsterUniqueChampionOnMode, MonsterUniqueChampionOnStart, NULL, NULL, NULL,
		MonsterUniqueChampionOnMissile },
	{ MonsterUniqueLightningOnMode, MonsterUniqueLightningOnStart, MonsterUniqueLightningOnEvent, NULL,
		MonsterUniqueLightningOnGetHit,
		MonsterUniqueLightningOnMissile },
	{ MonsterUniqueColdOnMode, MonsterUniqueModQueueEvent, MonsterUniqueColdOnEvent, NULL, NULL,
		MonsterUniqueColdOnMissile },
	{ MonsterUniqueHirelingOnMode, NULL, NULL, NULL, NULL, MonsterUniqueHirelingOnMissile },
	{ NULL, MonsterUniqueBugLightningOnStart, MonsterUniqueBugLightningOnEvent, NULL, NULL, NULL },
	{ NULL, NULL, MonsterUniqueKillOnEvent, NULL, NULL, NULL },
	{ NULL, MonsterUniqueQuestOnStart, NULL, NULL, NULL, NULL },
	{ MonsterUniqueQueenOnMode, NULL, NULL, NULL, NULL, MonsterUniqueQueenOnMissile },
	{ NULL, NULL, NULL, MonsterUniqueThiefOnHit, NULL, NULL },
	{ MonsterUniqueManaBurnOnMode, NULL, NULL, NULL, NULL, MonsterUniqueManaBurnOnMissile },
	{ NULL, NULL, NULL, NULL, NULL, NULL },
	{ MonsterUniqueSpectralOnMode, NULL, NULL, NULL, NULL, MonsterUniqueSpectralOnMissile },
	{ NULL, NULL, NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL, NULL, MonsterUniqueShotsOnMissile },
	{ NULL, NULL, NULL, NULL, NULL, NULL },
	{ NULL, MonsterUniqueModQueueEvent, MonsterUniqueExplodeOnEvent, NULL, NULL, NULL },
};
static int sganUniqueSuperKind0[2] = { 43, 43 };
static int sganUniqueSuperKind1[2] = { 43, 43 };
static int sganUniqueModUsedIndex[NUM_UNIQUE_MODS] =
{
	0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0, 6, 7, 0, 0, 0, 0, 0, 0, 8, 10, 9, 11, 12, 13, 0,
};

void __fastcall MonsterUniqueModQueueEvent(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	if ((dwArg || eMod != 18) && !hUnit->dwMode)
		EventAdd(ptGame, hUnit, 7, ptGame->nFrame + 4);
}

void __fastcall MonsterUniqueKillOnEvent(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	MONMODECHANGE tArgs;
	UNIT *hOwner;

	if (SUnitIsDead(hUnit))
		return;
	if (D2CommonStatsGetState(hUnit, 54))
	{
		EventAdd(ptGame, hUnit, 7, ptGame->nFrame + 3);
		return;
	}
	if (D2CommonStatsGetState(hUnit, 96))
	{
		hOwner = AiGetOwner(hUnit);
		if (hOwner && UnitGetType(hOwner) == UNIT_PLAYER)
		{
			PlayerPetsRemove(ptGame, hOwner, hUnit->dwId, 1);
			return;
		}
	}
	MonsterModeInit(hUnit, 0, &tArgs);
	MonsterModeSet(ptGame, &tArgs, 1);
}

void __fastcall MonsterUniqueModInitStats(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg)
{
	ITEMLEVELDATA *ptLevelData;
	void *ptStatList;
	int eLevelId, nLevel, nValue;

	nLevel = 2;
	eLevelId = D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hUnit));
	if (eLevelId)
	{
		ptLevelData = (ITEMLEVELDATA *)D2CommonLvlTblsGetLevelData(eLevelId);
		if (!ptLevelData->abLevel[ptGame->bDifficultyLevel])
			nLevel = 1;
		else
			nLevel = ptLevelData->abLevel[ptGame->bDifficultyLevel];
	}
	nValue = nLevel + 50;
	if (nValue >= 90)
		nValue = 90;
	D2CommonStatsUnitSetValue(hUnit, 12, nLevel);
	D2CommonStatsUnitSetValue(hUnit, 19, nValue);
#line 1887
	D2ASSERT(UnitGetType (hUnit) == UNIT_MONSTER);
	ptStatList = (void *)D2CommonStatsFindListByMask(hUnit, 1);
	switch (hUnit->dwClassId)
	{
	case 326:
		D2CommonStatsListSetValueB(ptStatList, 21, 0);
		D2CommonStatsListSetValueB(ptStatList, 22, 0);
		D2CommonStatsListSetValueB(ptStatList, 48, nLevel >> 1);
		D2CommonStatsListSetValueB(ptStatList, 49, (nLevel * 3) >> 1);
		return;
	case 327:
	case 328:
		if (nLevel < (nLevel >> 1) + 1)
			nValue = nLevel;
		else
			nValue = (nLevel >> 1) + 1;
		D2CommonStatsListSetValueB(ptStatList, 21, nValue);
		D2CommonStatsListSetValueB(ptStatList, 22, nLevel);
		return;
	case 329:
		D2CommonStatsListSetValueB(ptStatList, 21, 0);
		D2CommonStatsListSetValueB(ptStatList, 22, 0);
		D2CommonStatsListSetValueB(ptStatList, 57, nLevel);
		nLevel += nLevel;
		D2CommonStatsListSetValueB(ptStatList, 58, nLevel);
		D2CommonStatsListSetValueB(ptStatList, 59, nLevel);
		return;
	case 330:
	case 369:
		D2CommonStatsListSetValueB(ptStatList, 21, 0);
		D2CommonStatsListSetValueB(ptStatList, 22, 0);
		D2CommonStatsListSetValueB(ptStatList, 50, nLevel >> 1);
		D2CommonStatsListSetValueB(ptStatList, 51, (nLevel * 3) >> 1);
		return;
	}
}

void __fastcall MonsterUniqueOnMode(GAME *ptGame, UNIT *hUnit)
{
	MonsterUniqueCallMode(ptGame, hUnit, NULL, 0);
}

void __fastcall MonsterUniqueCallMode(GAME *ptGame, UNIT *hUnit, UNIT *hMissile, int eCall)
{
	struct MONSTERUNIQUE *ptUnique;
	int *peMod;
	int eMod;
	int i;

	ptUnique = MONSTERDATA7(hUnit)->ptUnique;
	if (!ptUnique->aeMods[0])
		return;
	if (eCall != MODE_MISSILEFIRE)
	{
		for (i = 0, peMod = ptUnique->aeMods; i < UNIQUE_MAX_MODS; i++, peMod++)
		{
			eMod = *peMod;
			if (spfnUniqueModMode[eMod][eCall])
			{
#line 2089
				D2ASSERTMSG(!IsBadCodePtr((FARPROC)spfnUniqueModMode[eMod][eCall]), "spfnUniqueModMode[eMod][eCall]");
				spfnUniqueModMode[eMod][eCall](ptGame, hUnit, eMod, ptUnique->dwRegionCounted);
			}
		}
		return;
	}
	for (i = 0, peMod = ptUnique->aeMods; i < UNIQUE_MAX_MODS; i++, peMod++)
	{
		eMod = *peMod;
		if (spfnUniqueModMode[eMod][MODE_MISSILEFIRE])
		{
#line 2102
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)spfnUniqueModMode[eMod][MODE_MISSILEFIRE]), "spfnUniqueModMode[eMod][MODE_MISSILEFIRE]");
			spfnUniqueModMode[eMod][MODE_MISSILEFIRE](ptGame, hMissile, eMod, ptUnique->dwRegionCounted);
		}
	}
}

void __fastcall MonsterUniqueOnStart(GAME *ptGame, UNIT *hUnit)
{
	MonsterUniqueCallMode(ptGame, hUnit, NULL, 1);
}

void __fastcall MonsterUniqueOnEvent(GAME *ptGame, UNIT *hUnit)
{
	MonsterUniqueCallMode(ptGame, hUnit, NULL, 2);
}

void __fastcall MonsterUniqueOnHit(GAME *ptGame, UNIT *hUnit)
{
	MonsterUniqueCallMode(ptGame, hUnit, NULL, 3);
}

void __fastcall MonsterUniqueOnGetHit(GAME *ptGame, UNIT *hUnit)
{
	MonsterUniqueCallMode(ptGame, hUnit, NULL, 4);
}

void __fastcall MonsterUniqueMissileMods(GAME *ptGame, UNIT *hSource, UNIT *hMissile)
{
	if (hSource && hMissile && UnitGetType(hSource) == UNIT_MONSTER)
		MonsterUniqueCallMode(ptGame, hSource, hMissile, MODE_MISSILEFIRE);
}

UNIT *__fastcall MonsterUniqueSpawnBoss(GAME *ptGame, GAMEROOM *hRoom,
	struct DRLGLOGICNODE *ptArea, int nClass, BOOL fChampion)
{
	UNIT *hMonster;

	hMonster = MonsterUniqueSpawn(ptGame, hRoom, ptArea, 0, 0, -1, nClass, 1);
	if (!hMonster)
		return NULL;
	MonsterUniqueAddRandomMods(hMonster, fChampion);
	MonsterUniqueSpawnMinions(ptGame, hRoom, ptArea, hMonster, TRUE, 3, 6);
	return hMonster;
}

UNIT *__fastcall MonsterUniqueSpawn(GAME *ptGame, GAMEROOM *hRoom,
	struct DRLGLOGICNODE *ptArea, int nX, int nY, int nGroup, int nClass,
	BOOL bAwayFromWarps)
{
	struct MONSTERUNIQUE *ptUnique;
	struct MONREGIOND2GAME7 *ptRegion;
	COLPOINT tPoint;
	GAMEROOM *hFreeRoom;
	UNIT *hMonster;

	if (!nX && !nY && !MonsterFindSpawnPoint(ptGame, hRoom, ptArea, nClass, &nX, &nY, bAwayFromWarps))
		return NULL;
	if (ptArea)
		hMonster = MonsterSpawnInArea(ptGame, hRoom, ptArea, nX, nY, nClass, 1, -1, 64);
	else if (nGroup == -1)
		hMonster = MonsterSpawn(ptGame, hRoom, nX, nY, nClass, 1, nGroup, 64);
	else
	{
		hMonster = MonsterSpawnWithArg(ptGame, hRoom, nX, nY, nClass, 1, nGroup, -1, 98);
		if (!hMonster)
		{
			hMonster = MonsterSpawnWithArg(ptGame, hRoom, nX, nY, nClass, 1, nGroup, 5, 98);
			if (!hMonster)
			{
				if (MonsterFindSpawnPoint(ptGame, hRoom, NULL, nClass, &nX, &nY, 0))
					hMonster = MonsterSpawnWithArg(ptGame, hRoom, nX, nY, nClass, 1, nGroup, -1,
						98);
				if (!hMonster)
				{
					tPoint.nX = nX;
					tPoint.nY = nY;
					D2CommonCollisnFindFreeArea(hRoom, &tPoint, 1, 7169, &hFreeRoom);
					if (hFreeRoom)
						hMonster = MonsterSpawnWithArg(ptGame, hFreeRoom, tPoint.nX, tPoint.nY, nClass, 1,
							nGroup, -1, 98);
				}
			}
		}
	}
	if (!hMonster)
		return NULL;
	ptUnique = MONSTERDATA7(hMonster)->ptUnique;
	ptRegion = (struct MONREGIOND2GAME7 *)D2CommonMonsterRegionArrayGetDword(&ptGame->pMonRegions,
		MonsterRegionRefGetLevel(MONSTERDATA7(hMonster)->pRegionRef));
	if (ptRegion && !ptUnique->dwRegionCounted)
		ptRegion->nUniques++;
	ptUnique->dwRegionCounted = 1;
	ptUnique->dwUnique = 1;
	QuestsOfferFigurineMonster(ptGame, hMonster);
	AiSetOwner(ptGame, hMonster, hMonster->dwId, 1, 1, 0);
	return hMonster;
}

void __fastcall MonsterUniqueSpawnMinions(GAME *ptGame, GAMEROOM *hRoom,
	struct DRLGLOGICNODE *ptArea, UNIT *hBoss, BOOL fMinions,
	int nMin, int nMax)
{
	struct MONSTERUNIQUE *ptUnique, *ptMinionUnique;
	int aeBaseMods[4];
	int *peBossMods, *peMods, *peDst, *peScan;
	UNIT *hMinion;
	int nCount, nClass, nLeft, eMod, i, j;

	if (fMinions)
	{
		nCount = MonsterUniqueRandom(&hBoss->tSeed, nMax - nMin + 1) + nMin;
		ptUnique = MONSTERDATA7(hBoss)->ptUnique;
		peBossMods = ptUnique->aeMods;
		for (i = UNIQUE_MAX_MODS, peMods = ptUnique->aeMods; i; i--, peMods++)
			if (*peMods == 16)
				nCount = 0;
		nClass = hBoss->dwClassId;
		switch (nClass)
		{
		case 229:
			nClass = 3;
			break;
		case 365:
			nClass = 6;
			break;
		}
		if (nClass >= 410)
			nCount = 0;
		for (nLeft = 0; nLeft < nCount; nLeft++)
		{
			if (ptArea)
				hMinion = MonsterSpawnMinion(ptGame, ptArea, hBoss, nClass, 1, 3, 64);
			else
				hMinion = MonsterSpawnNear(ptGame, hBoss, nClass, 1, 3, 64);
			if (!hMinion)
				continue;
			ptMinionUnique = MONSTERDATA7(hMinion)->ptUnique;
			peMods = peBossMods;
			peDst = ptMinionUnique->aeMods;
			for (i = 0, peScan = peDst; i < UNIQUE_MAX_MODS; peScan++, i++)
				if (!*peScan)
					break;
			j = i;
			peDst += i;
			for (; j < UNIQUE_MAX_MODS; peMods++, j++)
			{
				if (!*peMods)
					break;
				if (sgadwUniqueModInherit[*peMods])
					*peDst++ = *peMods;
			}
			AiSetOwner(ptGame, hMinion, hBoss->dwId, 1, 0, 0);
			AiAddMinion(ptGame, hBoss, hMinion);
			AiSetUnitRef(hMinion, hBoss);
			ptMinionUnique->dwMinion = 1;
		}
	}
	ptUnique = MONSTERDATA7(hBoss)->ptUnique;
	aeBaseMods[0] = 1;
	aeBaseMods[1] = 2;
	aeBaseMods[2] = 3;
	aeBaseMods[3] = 4;
	for (i = 0; i < 4; i++)
	{
		eMod = aeBaseMods[i];
		if (sgpfnUniqueModInit[eMod])
		{
			sgpfnUniqueModInit[eMod](hBoss, eMod, TRUE);
#line 678
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgpfnUniqueModInit[eMod]), "sgpfnUniqueModInit[eMod]");
		}
		AiForEachMinion(hBoss, (DWORD)eMod, 0, (PFNAIMINIONFN)MonsterUniqueModInitCallback);
	}
	for (i = 0, peMods = ptUnique->aeMods; i < UNIQUE_MAX_MODS; i++, peMods++)
	{
		eMod = *peMods;
		if (sgpfnUniqueModInit[eMod])
		{
			sgpfnUniqueModInit[eMod](hBoss, eMod, TRUE);
#line 689
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgpfnUniqueModInit[eMod]), "sgpfnUniqueModInit[eMod]");
		}
		AiForEachMinion(hBoss, (DWORD)eMod, 0, (PFNAIMINIONFN)MonsterUniqueModInitCallback);
	}
}

void __fastcall MonsterUniqueModInitCallback(UNIT *hUnit, int eMod, BOOL fCount)
{
	if (sgpfnUniqueModInit[eMod])
	{
#line 653
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgpfnUniqueModInit[eMod]), "sgpfnUniqueModInit[eMod]");
		sgpfnUniqueModInit[eMod](hUnit, eMod, fCount);
	}
}

void __fastcall MonsterUniqueAddRandomMods(UNIT *hMonster, BOOL fChampion)
{
	struct MONSTERUNIQUE *ptUnique;
	GAME *ptGame;
	BYTE abUsed[NUM_UNIQUE_MODTYPES];
	int *peSlot;
	D2SEED *ptSeed;
	int i, nAdd, nMod, nStart;

	nAdd = 0;
	ptSeed = &hMonster->tSeed;
	ptUnique = MONSTERDATA7(hMonster)->ptUnique;
	for (i = 0, peSlot = ptUnique->aeMods; i < UNIQUE_MAX_MODS; peSlot++, i++)
		if (!*peSlot)
			break;
	if (i >= UNIQUE_MAX_MODS)
		return;
	peSlot = &ptUnique->aeMods[i];
	if (fChampion && D2CommonSeedRand(ptSeed, 100) < 20)
	{
		ptUnique->dwChampion = 1;
		*peSlot = 16;
		return;
	}
	memset(abUsed, 0, sizeof(abUsed));
	nAdd = D2CommonSeedRand(ptSeed, 1) + 1;
	if (i + nAdd >= UNIQUE_MAX_MODS)
		nAdd = UNIQUE_MAX_MODS - i;
	ptGame = SUnitGetGame(hMonster);
#line 913
	D2ASSERT(ptGame);
#line 914
	D2ASSERT(ptGame->bDifficultyLevel < NUM_DIFFICULTY_LEVELS);
	nAdd += ptGame->bDifficultyLevel;
	if (!nAdd)
		return;
	do
	{
		nMod = D2CommonSeedRand(ptSeed, 13) + 1;
		nStart = nMod;
		while (abUsed[nMod])
		{
			nMod++;
			if (nMod >= NUM_UNIQUE_MODTYPES)
				nMod = 1;
			if (nMod == nStart)
				return;
		}
		abUsed[nMod] = 1;
		if (!nMod)
			return;
#line 925
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)spfnUniqueModTypeSetup[nMod]), "spfnUniqueModTypeSetup[nMod]");
		if (spfnUniqueModTypeSetup[nMod](hMonster, peSlot))
		{
			nAdd--;
			peSlot++;
		}
	} while (nAdd);
}

extern void MonsterUniqueGiveMod(GAME *ptGame, UNIT *hMonster, int eMod, BOOL fCount);

UNIT *__fastcall MonsterUniqueSpawnSuper(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY,
	int nKind, int nSuper)
{
	struct MONSTERUNIQUE *ptUnique;
	struct SUPERUNIQUED2GAME7 *ptSuper;
	const int *pnTable;
	D2SEED *ptSeed;
	UNIT *hMonster;
	BYTE abUsed[NUM_UNIQUE_MODTYPES];
	int *peSlot;
	int nIndex, nStart, nMod, nMinGrp, nMaxGrp, i, nUniqueId, nMonX, nMonY;
	int nK;
	BOOL fMod30;
	AIGENERAL *ptAi;

	if (hRoom)
		ptSeed = &ROOM7(hRoom)->tSeed;
	else
		ptSeed = NULL;
	switch (nKind)
	{
	case 0:
		nIndex = D2CommonSeedRand(ptSeed, 2);
		nSuper = sganUniqueSuperKind0[nIndex];
		pnTable = sganUniqueSuperKind0;
		goto lFind;
	case 1:
		nIndex = D2CommonSeedRand(ptSeed, 2);
		nSuper = sganUniqueSuperKind1[nIndex];
		pnTable = sganUniqueSuperKind1;
	lFind:
		nStart = nIndex;
		while (ptGame->abSuperUniques[nSuper >> 3] & gdwBitMasks[nSuper & 7])
		{
			nIndex++;
			if (nIndex >= 2)
				nIndex = 0;
			if (nIndex == nStart)
				return NULL;
			nSuper = pnTable[nIndex];
		}
		break;
	case 3:
		if (nSuper >= 42)
			return NULL;
		if (ptGame->abSuperUniques[nSuper >> 3] & gdwBitMasks[nSuper & 7])
			return NULL;
		break;
	default:
		return NULL;
	}
	ptSuper = (struct SUPERUNIQUED2GAME7 *)D2CommonMonsterTblsGetSuperUnique(nSuper);
	if (nSuper == 15 || nSuper == 6)
	{
		nMonY = nY;
		nMonX = nX;
	}
	else
	{
		nMonX = 0;
		nMonY = 0;
	}
	hMonster = MonsterUniqueSpawn(ptGame, hRoom, NULL, nMonX, nMonY, -1, ptSuper->dwClass, 0);
	if (!hMonster)
		return NULL;
	ptGame->abSuperUniques[nSuper >> 3] |= (BYTE)gdwBitMasks[nSuper & 7];
	ptUnique = MONSTERDATA7(hMonster)->ptUnique;
	memset(abUsed, 0, sizeof(abUsed));
	fMod30 = FALSE;
	ptUnique->dwSuperUnique = 1;
	for (peSlot = ptUnique->aeMods, nK = 0; nK < UNIQUE_MAX_MODS; peSlot++, nK++)
		if (!*peSlot)
			break;
#line 2313
	D2ASSERT(nK < (UNIQUE_MAX_MODS - 3));
	peSlot = &ptUnique->aeMods[nK];
	for (i = 0; i < 3; i++)
	{
		nMod = ptSuper->aeMods[i];
		if (!nMod)
			break;
		if (nMod == 30)
			fMod30 = TRUE;
		*peSlot++ = nMod;
		abUsed[sganUniqueModUsedIndex[nMod]] = 1;
	}
#line 2326
	D2ASSERT(ptGame->bDifficultyLevel < NUM_DIFFICULTY_LEVELS);
#line 2327
	D2ASSERT(nK < (UNIQUE_MAX_MODS - (3 + ptGame->bDifficultyLevel)));
	i = 0;
	if (i < ptGame->bDifficultyLevel) while (1)
	{
		nMod = D2CommonSeedRand(&hMonster->tSeed, 13) + 1;
		nStart = nMod;
		while (abUsed[nMod])
		{
			nMod++;
			if (nMod >= NUM_UNIQUE_MODTYPES)
				nMod = 1;
			if (nMod == nStart)
				goto mods_done;
		}
		abUsed[nMod] = 1;
		if (!nMod)
			break;
#line 2332
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)spfnUniqueModTypeSetup[nMod]), "spfnUniqueModTypeSetup[nMod]");
		if (spfnUniqueModTypeSetup[nMod](hMonster, peSlot))
			peSlot++;
		i++;
		if (i >= ptGame->bDifficultyLevel)
			break;
	}
mods_done:
	nMinGrp = ptSuper->nMinGrp;
	nMaxGrp = ptSuper->nMaxGrp;
	if (nMinGrp && nMaxGrp)
	{
		nMinGrp += ptGame->bDifficultyLevel;
		nMaxGrp += ptGame->bDifficultyLevel;
	}
	MonsterUniqueSpawnMinions(ptGame, hRoom, NULL, hMonster, TRUE, nMinGrp, nMaxGrp);
	nUniqueId = nSuper - 43;
	ptUnique->wUniqueId = (WORD)nUniqueId;
	if (fMod30)
		MonsterUniqueModInitAura(hMonster, 30, 1);
	ptAi = MONSTERDATA7(hMonster)->ptAi;
	switch (nSuper)
	{
	case 6:
		QuestsAttachUnit(ptGame, hMonster, 5);
		AiThinkSet(ptGame, hMonster, ptAi, 13);
		break;
	case 26:
	case 27:
	case 29:
		QuestsAttachUnit(ptGame, hMonster, 19);
		A3Q5AddMonster(ptGame, hMonster);
		break;
	case 36:
	case 37:
	case 38:
		QuestsAttachUnit(ptGame, hMonster, 23);
		break;
	case 39:
		QuestsAttachUnit(ptGame, hMonster, 4);
		break;
	case 10:
		for (i = D2CommonSeedRand(&hMonster->tSeed, 5) + 2; i; i--)
			MonsterSpawnNear(ptGame, hMonster, 4, 1, 4, 64);
		MonsterSpawnNear(ptGame, hMonster, 276, 1, 4, 64);
		MonsterSpawnNear(ptGame, hMonster, 381, 1, 4, 64);
		MonsterSpawnNear(ptGame, hMonster, 385, 1, 4, 64);
		MonsterSpawnNear(ptGame, hMonster, 389, 1, 4, 64);
		break;
	}
	MonsterUniqueGiveMod(ptGame, hMonster, 22, TRUE);
	return hMonster;
}

UNIT *__fastcall MonsterUniqueRecreate(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY, int nClass,
	int nGroup, WORD nUniqueId, DWORD dwChampion,
	const int *peMods)
{
	struct MONSTERUNIQUE *ptUnique;
	UNIT *hMonster;
	int nSuper, i;
	int *peDst;
	BOOL fMod30;

	hMonster = MonsterUniqueSpawn(ptGame, hRoom, NULL, nX, nY, nGroup, nClass, 1);
	if (!hMonster)
		return NULL;
	ptUnique = MONSTERDATA7(hMonster)->ptUnique;
	fMod30 = FALSE;
	ptUnique->dwChampion = dwChampion;
	for (i = UNIQUE_MAX_MODS, peDst = ptUnique->aeMods; i; i--, peMods++, peDst++)
	{
		*peDst = *peMods;
		if (*peMods == 30)
			fMod30 = TRUE;
	}
	MonsterUniqueSpawnMinions(ptGame, hRoom, NULL, hMonster, FALSE, 0, 0);
	ptUnique->wUniqueId = (WORD)nUniqueId;
	if (fMod30)
		MonsterUniqueModInitAura(hMonster, 30, 1);
	nSuper = (nUniqueId & 65535) - 65493;
	if (nSuper >= 0 && nSuper < 42)
		ptUnique->dwSuperUnique = 1;
	switch (nSuper)
	{
	case 6:
		QuestsAttachUnit(ptGame, hMonster, 5);
		break;
	case 26:
	case 27:
	case 29:
		QuestsAttachUnit(ptGame, hMonster, 19);
		break;
	case 39:
		QuestsAttachUnit(ptGame, hMonster, 4);
		break;
	case 36:
	case 37:
	case 38:
		QuestsAttachUnit(ptGame, hMonster, 23);
		break;
	}
	return hMonster;
}

UNIT *__fastcall MonsterUniqueSpawnWithMods(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY, int nClass,
	int nGroup, const int *peMods)
{
	struct MONSTERUNIQUE *ptUnique;
	int aeBaseMods[4];
	int *peDst;
	UNIT *hMonster;
	int eMod;
	int i;

	hMonster = MonsterSpawnWithArg(ptGame, hRoom, nX, nY, nClass, 1, nGroup, -1, 98);
	if (!hMonster)
	{
		hMonster = MonsterSpawnWithArg(ptGame, hRoom, nX, nY, nClass, 1, nGroup, 4, 98);
		if (!hMonster)
		{
			if (MonsterFindSpawnPoint(ptGame, hRoom, NULL, nClass, &nX, &nY, 1))
				hMonster = MonsterSpawnWithArg(ptGame, hRoom, nX, nY, nClass, 1, nGroup, -1, 98);
			if (!hMonster)
				return NULL;
		}
	}
	ptUnique = MONSTERDATA7(hMonster)->ptUnique;
	for (i = UNIQUE_MAX_MODS, peDst = ptUnique->aeMods; i; i--)
		*peDst++ = *peMods++;
	aeBaseMods[1] = 2;
	ptUnique->dwMinion = 1;
	aeBaseMods[0] = 1;
	aeBaseMods[2] = 3;
	aeBaseMods[3] = 4;
	for (i = 0; i < 4; i++)
	{
		eMod = aeBaseMods[i];
		if (sgpfnUniqueModInit[eMod])
		{
#line 2495
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgpfnUniqueModInit[eMod]), "sgpfnUniqueModInit[eMod]");
			sgpfnUniqueModInit[eMod](hMonster, eMod, FALSE);
		}
	}
	for (i = 0, peDst = ptUnique->aeMods; i < UNIQUE_MAX_MODS; i++, peDst++)
	{
		eMod = *peDst;
		if (sgpfnUniqueModInit[eMod])
		{
#line 2505
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgpfnUniqueModInit[eMod]), "sgpfnUniqueModInit[eMod]");
			sgpfnUniqueModInit[eMod](hMonster, eMod, FALSE);
		}
	}
	return hMonster;
}

__inline void MonsterUniqueGiveMod(GAME *ptGame, UNIT *hMonster, int eMod, BOOL fCount)
{
	struct MONSTERUNIQUE *ptUnique;
	struct MONREGIOND2GAME7 *ptRegion;
	int i;
	int *peMod;

	ptUnique = MONSTERDATA7(hMonster)->ptUnique;
	if (fCount)
	{
		ptRegion = (struct MONREGIOND2GAME7 *)D2CommonMonsterRegionArrayGetDword(&ptGame->pMonRegions,
			MonsterRegionRefGetLevel(MONSTERDATA7(hMonster)->pRegionRef));
		if (ptRegion && !ptUnique->dwRegionCounted)
			ptRegion->nUniques++;
		ptUnique->dwRegionCounted = 1;
	}
	peMod = ptUnique->aeMods;
	for (i = 0; i < UNIQUE_MAX_MODS; peMod++, i++)
		if (!*peMod)
			break;
	ptUnique->aeMods[i] = eMod;
	if (sgpfnUniqueModInit[eMod])
	{
#line 2526
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgpfnUniqueModInit[eMod]), "sgpfnUniqueModInit[eMod]");
		sgpfnUniqueModInit[eMod](hMonster, eMod, fCount);
	}
}

void __fastcall MonsterUniqueAddMod(GAME *ptGame, UNIT *hMonster, int eMod, BOOL fCount)
{
	MonsterUniqueGiveMod(ptGame, hMonster, eMod, fCount);
}

void __fastcall MonsterUniqueMakeChampion(GAME *ptGame, UNIT *hMonster)
{
	struct MONSTERUNIQUE *ptUnique;
	struct MONREGIOND2GAME7 *ptRegion;
	int i;
	int *peMod;

	ptUnique = MONSTERDATA7(hMonster)->ptUnique;
	if (ptUnique->dwChampion)
		return;
	ptRegion = (struct MONREGIOND2GAME7 *)D2CommonMonsterRegionArrayGetDword(&ptGame->pMonRegions,
		MonsterRegionRefGetLevel(MONSTERDATA7(hMonster)->pRegionRef));
	if (ptRegion && !ptUnique->dwRegionCounted)
		ptRegion->nUniques++;
	ptUnique->dwRegionCounted = 1;
	ptUnique->dwChampion = 1;
	ptUnique->dwUnique = 1;
	peMod = ptUnique->aeMods;
	for (i = 0; i < UNIQUE_MAX_MODS; peMod++, i++)
		if (!*peMod)
			break;
	ptUnique->aeMods[i] = 16;
	MonsterUniqueSpawnMinions(ptGame, (GAMEROOM *)D2CommonUnitGetPathRoom(hMonster), NULL, hMonster, TRUE,
		0, 0);
}

void __fastcall MonsterUniqueMakeUnique(GAME *ptGame, UNIT *hMonster)
{
	MONSTERDATA *ptMonsterData;
	struct MONSTERUNIQUE *ptUnique;
	struct MONREGIOND2GAME7 *ptRegion;

	ptMonsterData = MONSTERDATA7(hMonster);
	ptUnique = ptMonsterData->ptUnique;
	ptUnique->dwUnique = 1;
	ptRegion = (struct MONREGIOND2GAME7 *)D2CommonMonsterRegionArrayGetDword(&ptGame->pMonRegions,
		MonsterRegionRefGetLevel(MONSTERDATA7(hMonster)->pRegionRef));
	if (ptRegion && !ptUnique->dwRegionCounted)
		ptRegion->nUniques++;
	ptUnique->dwRegionCounted = 1;
	MonsterUniqueAddRandomMods(hMonster, TRUE);
	MonsterUniqueSpawnMinions(ptGame, (GAMEROOM *)D2CommonUnitGetPathRoom(hMonster), NULL, hMonster,
		FALSE, 0, 0);
	hMonster->dwFlags0 |= 2048;
	AiRecordSetFlags((AIRECORDHEAD *)ptMonsterData->pRegionRef, AIFLAG_01, TRUE);
}

UNIT *__fastcall MonsterUniqueCreateShrine(GAME *ptGame, GAMEROOM *hRoom, int nClass, int nX, int nY,
	DWORD dwArg)
{
	const struct UNIQUESHRINE *ptShrine;
	UNIT *hObject;
	int nShrine;

	if (nClass < 411 || nClass > 416)
		return NULL;
	ptShrine = &sgatUniqueShrines[nClass - 411];
	hObject = SUnitCreate(UNIT_OBJECT, ptShrine->dwClass, nX, nY, ptGame, hRoom, 1, dwArg, 0);
	nShrine = MonsterUniqueRandom(&hObject->tSeed, ptShrine->nMax - ptShrine->nMin) + ptShrine->nMin;
	if (nShrine == 5 || nShrine == 4)
		nShrine = 2;
	OBJDATA(hObject)->field_04 = (BYTE)nShrine;
	D2CommonUnitSetObjectShrine(hObject, D2CommonObjTblsGetShrine(nShrine));
	return hObject;
}

UNIT *__fastcall MonsterUniqueCreateActObject(GAME *ptGame, GAMEROOM *hRoom, int nClass, int nX, int nY,
	DWORD dwUnused)
{
	D2SEED *ptSeed;
	DWORD dwClass;
	int eLevelId;
	BYTE bAct;

	bAct = D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom));
	ptSeed = ObjectRgnGetSeed(ptGame, hRoom);
	eLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
	switch (bAct)
	{
	case 1:
		if (eLevelId == 74)
		{
			DWORD adwChoice[4];

			adwChoice[0] = 387;
			adwChoice[1] = 389;
			adwChoice[2] = 390;
			adwChoice[3] = 391;
			dwClass = adwChoice[D2CommonSeedRand(ptSeed, 4)];
		}
		else
		{
			DWORD adwChoice[2];

			adwChoice[0] = 87;
			adwChoice[1] = 88;
			dwClass = adwChoice[D2CommonSeedRand(ptSeed, 2)];
		}
		break;
	case 2:
		if (eLevelId == 83)
		{
			DWORD adwChoice[4];

			adwChoice[0] = 329;
			adwChoice[1] = 330;
			adwChoice[2] = 331;
			adwChoice[3] = 332;
			dwClass = adwChoice[D2CommonSeedRand(ptSeed, 4)];
		}
		else
		{
			DWORD adwChoice[2];

			adwChoice[0] = 181;
			adwChoice[1] = 183;
			dwClass = adwChoice[D2CommonSeedRand(ptSeed, 2)];
		}
		break;
	default:
	{
		DWORD adwChoice13[13];

		adwChoice13[0] = 5;
		adwChoice13[1] = 6;
		adwChoice13[2] = 139;
		adwChoice13[3] = 140;
		adwChoice13[4] = 141;
		adwChoice13[5] = 144;
		adwChoice13[6] = 176;
		adwChoice13[7] = 177;
		adwChoice13[8] = 198;
		adwChoice13[9] = 240;
		adwChoice13[10] = 241;
		adwChoice13[11] = 242;
		adwChoice13[12] = 243;
		dwClass = adwChoice13[D2CommonSeedRand(ptSeed, 13)];
		break;
	}
	}
	if (nClass == 371)
		dwClass = 371;
	return SUnitCreate(UNIT_OBJECT, dwClass, nX, nY, ptGame, hRoom, 1, 0, 0);
}
