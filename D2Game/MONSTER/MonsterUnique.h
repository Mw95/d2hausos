#ifndef D2RE_D2GAME_MONSTERUNIQUE_H
#define D2RE_D2GAME_MONSTERUNIQUE_H

#include "../D2game.h"

struct DRLGLOGICNODE;
struct MONSTERUNIQUE;

#include "MonsterSpawn.h"
struct MONSTERUNIQUE *__fastcall MonsterUniqueAlloc(GAME *ptGame);
void __fastcall MonsterUniqueFree(GAME *ptGame, struct MONSTERUNIQUE *ptUnique);
int *__fastcall MonsterUniqueGetMods(struct MONSTERUNIQUE *ptUnique);
WORD __fastcall MonsterUniqueGetId(struct MONSTERUNIQUE *ptUnique);
DWORD __fastcall MonsterUniqueIsChampion(struct MONSTERUNIQUE *ptUnique);
DWORD __fastcall MonsterUniqueIsSuperUnique(struct MONSTERUNIQUE *ptUnique);
DWORD __fastcall MonsterUniqueIsCounted(struct MONSTERUNIQUE *ptUnique);
DWORD __fastcall MonsterUniqueIsUnique(struct MONSTERUNIQUE *ptUnique);
DWORD __fastcall MonsterUniqueIsMinion(struct MONSTERUNIQUE *ptUnique);

#define MonsterRegionRefGetLevel(ptRef) MonsterUniqueIsUnique((struct MONSTERUNIQUE *)(ptRef))
#define MonsterModeDataGetVelocity(ptMode) MonsterUniqueIsCounted((struct MONSTERUNIQUE *)(ptMode))
BOOL __fastcall MonsterUniqueHasMods(struct MONSTERUNIQUE *ptUnique);
DWORD __fastcall MonsterUniqueGetBurstFlag(struct MONSTERUNIQUE *ptUnique);
void __fastcall MonsterUniqueModInitId(UNIT *hUnit, int eMod,
	BOOL fCount);
void __fastcall MonsterUniqueModInitLife(UNIT *hUnit, int eMod,
	BOOL fCount);
void __fastcall MonsterUniqueModInitLevel(UNIT *hUnit, int eMod,
	BOOL fCount);
void __fastcall MonsterUniqueModInitChampion(UNIT *hUnit, int eMod,
	BOOL fCount);
void __fastcall MonsterUniqueModInitResist(UNIT *hUnit, int eMod,
	BOOL fCount);
void __fastcall MonsterUniqueModInitSkill(UNIT *hUnit, int eMod,
	BOOL fCount);
void __fastcall MonsterUniqueModInitAura(UNIT *hUnit, int eMod,
	BOOL fCount);
BOOL __fastcall MonsterUniqueTypeSetupStrong(UNIT *hUnit, int *peMod);
BOOL __fastcall MonsterUniqueTypeSetupFast(UNIT *hUnit, int *peMod);
BOOL __fastcall MonsterUniqueTypeSetupCursed(UNIT *hUnit, int *peMod);
BOOL __fastcall MonsterUniqueTypeSetupResistant(UNIT *hUnit, int *peMod);
BOOL __fastcall MonsterUniqueTypeSetupFire(UNIT *hUnit, int *peMod);
BOOL __fastcall MonsterUniqueTypeSetupLightning(UNIT *hUnit, int *peMod);
BOOL __fastcall MonsterUniqueTypeSetupCold(UNIT *hUnit, int *peMod);
BOOL __fastcall MonsterUniqueTypeSetupManaBurn(UNIT *hUnit, int *peMod);
BOOL __fastcall MonsterUniqueTypeSetupSpectral(UNIT *hUnit, int *peMod);
BOOL __fastcall MonsterUniqueTypeSetupTeleport(UNIT *hUnit, int *peMod);
BOOL __fastcall MonsterUniqueTypeSetupStoneSkin(UNIT *hUnit, int *peMod);
BOOL __fastcall MonsterUniqueTypeSetupShots(UNIT *hUnit, int *peMod);
BOOL __fastcall MonsterUniqueTypeSetupAura(UNIT *hUnit, int *peMod);
void __fastcall MonsterUniqueStrongOnMode(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueAddDamage(GAME *ptGame, UNIT *hUnit, int nMinStat,
	int nMaxStat, int nLengthStat,
		int nMultiply);
void __fastcall MonsterUniqueStrongOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueFastOnStart(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueCursedOnHit(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueCursedOnTarget(GAME *ptGame, UNIT *hUnit, UNIT *hTarget,
	int nLevel);
void __fastcall MonsterUniqueFireOnMode(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueFireOnStart(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueFireOnEvent(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueFireOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueExplodeOnEvent(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueChampionOnMode(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueChampionOnStart(GAME *ptGame, UNIT *hUnit, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueChampionOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueLightningOnMode(GAME *ptGame, UNIT *hUnit, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueLightningOnEvent(GAME *ptGame, UNIT *hUnit, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueLightningOnGetHit(GAME *ptGame, UNIT *hUnit, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueLightningOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueColdOnMode(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueColdOnEvent(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueColdOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniquePoisonCloudOnEvent(GAME *ptGame, UNIT *hUnit, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueGroupDeathOnStart(GAME *ptGame, UNIT *hUnit, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueGroupDeathOnMinion(UNIT *hMinion, GAME *ptGame, DWORD dwArg);
void __fastcall MonsterUniqueHirelingOnMode(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueHirelingOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueBugLightningOnStart(GAME *ptGame, UNIT *hUnit, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueBugLightningOnEvent(GAME *ptGame, UNIT *hUnit, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueManaBurnOnMode(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueManaBurnOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueSpectralOnMode(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueSpectralOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueThiefOnHit(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueQuestOnStart(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueQueenOnMode(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueQueenOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueShotsOnMissile(GAME *ptGame, UNIT *hMissile, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueLightningOnStart(GAME *ptGame, UNIT *hUnit, int eMod,
	DWORD dwArg);
void __fastcall MonsterUniqueModQueueEvent(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueKillOnEvent(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueModInitStats(GAME *ptGame, UNIT *hUnit, int eMod, DWORD dwArg);
void __fastcall MonsterUniqueOnMode(GAME *ptGame, UNIT *hUnit);
void __fastcall MonsterUniqueCallMode(GAME *ptGame, UNIT *hUnit, UNIT *hMissile, int eCall);
void __fastcall MonsterUniqueOnStart(GAME *ptGame, UNIT *hUnit);
void __fastcall MonsterUniqueOnEvent(GAME *ptGame, UNIT *hUnit);
void __fastcall MonsterUniqueOnHit(GAME *ptGame, UNIT *hUnit);
void __fastcall MonsterUniqueOnGetHit(GAME *ptGame, UNIT *hUnit);
void __fastcall MonsterUniqueMissileMods(GAME *ptGame, UNIT *hSource, UNIT *hMissile);
UNIT *__fastcall MonsterUniqueSpawnBoss(GAME *ptGame, GAMEROOM *hRoom,
	struct DRLGLOGICNODE *ptArea, int nClass, BOOL fChampion);
UNIT *__fastcall MonsterUniqueSpawn(GAME *ptGame, GAMEROOM *hRoom,
	struct DRLGLOGICNODE *ptArea, int nX, int nY, int nGroup, int nClass,
	BOOL bAwayFromWarps);
void __fastcall MonsterUniqueSpawnMinions(GAME *ptGame, GAMEROOM *hRoom,
	struct DRLGLOGICNODE *ptArea, UNIT *hBoss, BOOL fMinions,
	int nMin, int nMax);
void __fastcall MonsterUniqueModInitCallback(UNIT *hUnit, int eMod, BOOL fCount);
void __fastcall MonsterUniqueAddRandomMods(UNIT *hMonster, BOOL fChampion);
UNIT *__fastcall MonsterUniqueSpawnSuper(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY,
	int nKind, int nSuper);
UNIT *__fastcall MonsterUniqueRecreate(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY, int nClass,
	int nGroup, WORD nUniqueId, DWORD dwChampion,
	const int *peMods);
UNIT *__fastcall MonsterUniqueSpawnWithMods(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY, int nClass,
	int nGroup, const int *peMods);
void __fastcall MonsterUniqueAddMod(GAME *ptGame, UNIT *hMonster, int eMod, BOOL fCount);
void __fastcall MonsterUniqueMakeChampion(GAME *ptGame, UNIT *hMonster);
void __fastcall MonsterUniqueMakeUnique(GAME *ptGame, UNIT *hMonster);
UNIT *__fastcall MonsterUniqueCreateShrine(GAME *ptGame, GAMEROOM *hRoom, int nClass, int nX, int nY,
	DWORD dwArg);
UNIT *__fastcall MonsterUniqueCreateActObject(GAME *ptGame, GAMEROOM *hRoom, int nClass, int nX, int nY,
	DWORD dwUnused);

#endif
