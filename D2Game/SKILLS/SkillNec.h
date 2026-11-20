#ifndef D2RE_D2GAME_SKILLNEC_H
#define D2RE_D2GAME_SKILLNEC_H

#include "../D2game.h"

BOOL __fastcall SkillsMonstValidTarget(GAME *ptGame, UNIT *hUnit, UNIT *hTarget);
BOOL __fastcall SkillsTeeth(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsPoisonNova(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsCorpseExplosionStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsCorpseExplosion(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsBoneArmor(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsBoneArmorAbsorb(UNIT *hUnit, DAMAGE *ptDamage);
BOOL __fastcall SkillsClayGolem(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsNecGolem(GAME *ptGame, UNIT *hUnit, int nLevel, int nSkillId);
BOOL __fastcall SkillsBloodGolem(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsIronGolemStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsIronGolem(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsFireGolem(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsReviveStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsRevive(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsRaiseStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsRaise(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsNecSkeletonLook(UNIT *hOwner, UNIT *hPet, int nMastery, BOOL bShield, BOOL bMage);
BOOL __fastcall SkillsAmplifyDamage(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsAmplifyDamageCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);
void __fastcall SkillsDimVisionEnd(UNIT *hUnit, DWORD dwState);
void __fastcall SkillsDimVisionCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);
BOOL __fastcall SkillsDimVision(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsWeaken(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsWeakenCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);
BOOL __fastcall SkillsIronMaiden(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsIronMaidenCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);
BOOL __fastcall SkillsTerror(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsTerrorCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);
BOOL __fastcall SkillsAttractStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsAttract(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsAttractCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);
void __fastcall SkillsAttractEnd(UNIT *hUnit, DWORD dwState);
BOOL __fastcall SkillsLifeTap(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsLifeTapCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);
BOOL __fastcall SkillsConfuse(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsConfuseCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);
BOOL __fastcall SkillsDecrepify(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsDecrepifyCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);
BOOL __fastcall SkillsLowerResist(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsLowerResistCurse(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);
BOOL __fastcall SkillsPoisonDaggerStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsPoisonExplosion(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsBoneSpirit(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsBoneWall(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsBonePrisonStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsBonePrison(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsNecStart(GAME *ptGame, UNIT *hUnit, int nSkillId, int nLevel);
BOOL __fastcall SkillsNecUse(GAME *ptGame, UNIT *hUnit, int nSkillId, int nLevel);
WORD __fastcall SkillsIronMaidenHit(GAME *ptGame, UNIT *hAttacker, UNIT *hTarget, DAMAGE *ptDamage);
WORD __fastcall SkillsLifeTapHit(GAME *ptGame, UNIT *hAttacker, UNIT *hTarget, DAMAGE *ptDamage);
void __fastcall SkillsBloodGolemLife(UNIT *hGolem, UNIT *hTarget, int nDamage);
int __fastcall SkillsBloodGolemShare(UNIT *hUnit, int nDamage);
int __fastcall SkillsBloodGolemHeal(GAME *ptGame, UNIT *hUnit, int nValue);

#endif
