#ifndef D2RE_D2GAME_SKILLMONST_H
#define D2RE_D2GAME_SKILLMONST_H

#include "../D2game.h"

BOOL __fastcall SkillsShamanFire(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsUnHolyBolt(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsFireHitStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsFireHit(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMaggotEggStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMaggotEgg(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMagottUpStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMagottDownStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMagottDown(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMagottLay(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsAndrialSprayStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsAndrialSpray(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsAndyPoisonBolt(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsPoisonBallTrap(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsCursedBallTrapRight(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsCursedBallTrapLeft(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsUseLevelAsMissile(GAME *ptGame, UNIT *hUnit, int nMissile);
BOOL __fastcall SkillsJumpStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsJump(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsSwarmMoveStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsSwarmMove(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsNestStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsNest(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsQuickStrikeStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsQuickStrike(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsVampireFireball(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsVampireFirewall(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsVampireMeteor(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsGargoyleTrap(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsDesertTurret(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsArcaneTower(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsSpiderLayStart(GAME *ptGame, UNIT *hUnit, int nLevel);
UNIT * __fastcall SkillsSpiderLayWalk(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall SkillsFetishAura(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsZakarumHealStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsSubmergeStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsSubmerge(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsEmergeStart(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsMonstRevive(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall SkillsResurrectStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsBestowStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsPrimeBolt(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsPrimeLightning(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsPrimeBlaze(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMonTeleport(GAME *ptGame, UNIT *hUnit, int nLevel);

BOOL __fastcall SkillsPrimeSpike(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsPrimeIceNova(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsPrimePoisonball(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsPrimePoisonNova(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsFetishInfernoStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsFetishInferno(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsDiabCold(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsFingerMageSpider(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsRegurgitatorEat(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsDiabWall(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsDiabWallModify(UNIT *hMissile, DWORD dwIndex);
BOOL __fastcall SkillsDiabRunStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsDiabRun(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsDiabPrisonStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMonBlizzard(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMosquitoStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMosquito(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMonFrozenArmor(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMonBoneArmor(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMonBoneSpirit(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsMonstAmplifyDamage(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);
void __fastcall SkillsMonstWeaken(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);
void __fastcall SkillsMonstIronMaiden(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);
void __fastcall SkillsMonstLifeTap(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);
void __fastcall SkillsMonstDecrepify(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);
void __fastcall SkillsMonstLowerResist(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);
BOOL __fastcall SkillsMonCurseCast(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsHellMeteorStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMonFrenzyStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMonFrenzy(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMonstStart(GAME *ptGame, UNIT *hUnit, int nSkillId, int nLevel);
BOOL __fastcall SkillsMonstUse(GAME *ptGame, UNIT *hUnit, int nSkillId, int nLevel);

#endif
