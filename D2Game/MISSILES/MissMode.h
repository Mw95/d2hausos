#ifndef D2RE_D2GAME_MISSMODE_H
#define D2RE_D2GAME_MISSMODE_H

#include "../D2game.h"

#define NUM_MISSILE_MODES	9
extern MISSILEMODECHART gptMissileModeChart[NUM_MISSILE_MODES];

typedef int (__fastcall *PFNMISSILEMODE)(GAME *ptGame, UNIT *hMissile);
typedef int (__fastcall *PFNMISSILEHIT)(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
typedef void (__fastcall *PFNMISSILEDAMAGE)(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, DAMAGE *ptDamage);

void __fastcall MissModeApplyDamage(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, DAMAGE *ptDamage);
int __fastcall MissModeHitBlaze(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
void __fastcall MissModeDamageIceArrow(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, DAMAGE *ptDamage);
int __fastcall MissModeHitFireBall(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitPotion(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitFreezingArrow(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitHolyBolt(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitBattleCommand(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitBattleOrders(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
BOOL __fastcall MissModeCreateRingOfSixteen(GAME *ptGame, UNIT *hOwner,
	UNIT *hSpawn,
	int eMissile, int nType, int nLevel);
int __fastcall MissModeHitPlagueJavelin(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitRancidGasPotion(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitChokingGasPotion(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitStranglingGasPotion(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
BOOL __fastcall MissModeCreateRingOfEight(GAME *ptGame, UNIT *hOwner,
	UNIT *hSpawn,
	int eMissile, int nType, int nLevel);
int __fastcall MissModeHitPantherPot(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
void __fastcall MissModeDamageRogue(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, DAMAGE *ptDamage);
void __fastcall MissModeDamageWarcry(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, DAMAGE *ptDamage);
int __fastcall MissModeHitImmolationArrow(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
void __fastcall MissModeDamageIceBlast(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, DAMAGE *ptDamage);
void __fastcall MissModeDamageBlessedHammer(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, DAMAGE *ptDamage);
int __fastcall MissModeHitChainLightning(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitFistOfTheHeavens(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitGlacialSpike(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitMonGlacialSpike(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitMeteor(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitBattleCry(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeModeCloud(GAME *ptGame, UNIT *hMissile);
int __fastcall MissModeModePoisonJav(GAME *ptGame, UNIT *hMissile);
int __fastcall MissModeModePlagueJavelin(GAME *ptGame, UNIT *hMissile);
int __fastcall MissModeModeFire(GAME *ptGame, UNIT *hMissile);
void __fastcall MissModeDamageFire(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, DAMAGE *ptDamage);
int __fastcall MissModeModeFirewallMaker(GAME *ptGame, UNIT *hMissile);
int __fastcall MissModeModeBoneWallMaker(GAME *ptGame, UNIT *hMissile);
int __fastcall MissModeModeGrimWard(GAME *ptGame, UNIT *hMissile);
int __fastcall MissModeHitGrimWardScare(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeModeFrozenOrb(GAME *ptGame, UNIT *hMissile);
int __fastcall MissModeModeFrozenOrbNova(GAME *ptGame, UNIT *hMissile);
int __fastcall MissModeHitFrozenOrb(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitGrimWardStart(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeModeBatLightning(GAME *ptGame, UNIT *hMissile);
int __fastcall MissModeModeMonBlizCenter(GAME *ptGame, UNIT *hMissile);
UNIT *__fastcall MissModeCreateNearby(GAME *ptGame, UNIT *hMissile, int nRange,
	int nDelay, int eMissile, DWORD dwPattern);
int __fastcall MissModeModeBlizzardCenter(GAME *ptGame, UNIT *hMissile);
int __fastcall MissModeHitGuided(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitLightningFury(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
void __fastcall MissModeDamageGuidedArrow(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, DAMAGE *ptDamage);
int __fastcall MissModeModeGuided(GAME *ptGame, UNIT *hMissile);
int __fastcall MissModeHitSpiderGooLay(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
void __fastcall MissModeSpiderGooEnd(UNIT *hUnit, int nState);
int __fastcall MissModeHitSpiderGoo(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitHowl(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitShout(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitFirehead(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeModeDiabWallMaker(GAME *ptGame, UNIT *hMissile);
int __fastcall MissModeModeFingerMageSpider(GAME *ptGame, UNIT *hMissile);
int __fastcall MissModeHitFingerMageSpider(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeModeCairnStones(GAME *ptGame, UNIT *hMissile);
int __fastcall MissModeHitCairnStones(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitTowerChestSpawner(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeModeTowerChestSpawner(GAME *ptGame, UNIT *hMissile);
int __fastcall MissModeModeRadamentDeath(GAME *ptGame, UNIT *hMissile);
void __fastcall MissModeRadamentDeathFirst(GAME *ptGame, UNIT *hMissile, UNIT *hUnit,
	DWORD dwArg, DWORD dwUnused);
void __fastcall MissModeRadamentDeathLast(GAME *ptGame, UNIT *hMissile, UNIT *hUnit,
	DWORD dwArg, DWORD dwUnused);
int __fastcall MissModeHitHellMeteorDown(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
int __fastcall MissModeHitOrbMist(GAME *ptGame, UNIT *hMissile, UNIT *hTarget);
void __fastcall MissModeHitUnit(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, PFNMISSILEDAMAGE pfnDamage);
int __fastcall MissModeModeDefault(GAME *ptGame, UNIT *hMissile);
int __fastcall MissModeCollide(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, BOOL fExplode);
void __fastcall MissModeRun(GAME *ptGame, UNIT *hMissile, int eEventType);

#endif
