#ifndef D2RE_D2GAME_SKILPAL_H
#define D2RE_D2GAME_SKILPAL_H

#include "../D2game.h"

BOOL __fastcall SkillsSacrificeStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsSacrifice(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsSmiteStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsPrayer(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsPrayerEnd(UNIT *hUnit, DWORD dwState);
void __fastcall SkillsPrayerAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
void __fastcall SkillsVigorAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
void __fastcall SkillsVigorEnd(UNIT *hUnit, DWORD dwState);
BOOL __fastcall SkillsVigor(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsMeditationAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
BOOL __fastcall SkillsMeditation(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsMightAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
BOOL __fastcall SkillsMight(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsBlessedAimAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
BOOL __fastcall SkillsBlessedAim(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsConcentrationAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
BOOL __fastcall SkillsConcentration(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsFanaticismAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
BOOL __fastcall SkillsFanaticism(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsConviction(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsConvictionAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
void __fastcall SkillsResistFireAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
BOOL __fastcall SkillsResistFire(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsResistColdAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
BOOL __fastcall SkillsResistCold(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsResistLightningAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel,
	DWORD dwArg);
BOOL __fastcall SkillsResistLightning(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsSalvationAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
BOOL __fastcall SkillsSalvation(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsHolyFire(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsHolyFireAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
BOOL __fastcall SkillsHolyFreeze(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsHolyFreezeAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
void __fastcall SkillsHolyFreezeEnd(UNIT *hUnit, DWORD dwState);
BOOL __fastcall SkillsHolyShock(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsHolyShockAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
BOOL __fastcall SkillsThorns(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsThornsAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
BOOL __fastcall SkillsSanctuary(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsSanctuaryAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
void __fastcall SkillsRedemptionAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
BOOL __fastcall SkillsRedemption(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsBlessedHammer(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsDefianceAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
BOOL __fastcall SkillsDefiance(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsCleansing(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsCleansingAffect(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
BOOL __fastcall SkillsChargeStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsCharge(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsVengeanceStart(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsConversionEnd(UNIT *hUnit, DWORD dwState);
BOOL __fastcall SkillsConversionStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsConversion(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsHolyShieldStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsHolyShield(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsZealStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsZeal(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsFistOfTheHeavens(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsPalStart(GAME *ptGame, UNIT *hUnit, int nSkillId, int nLevel);
BOOL __fastcall SkillsPalUse(GAME *ptGame, UNIT *hUnit, int nSkillId, int nLevel);
WORD __fastcall SkillsThornsHit(GAME *ptGame, UNIT *hAttacker, UNIT *hTarget, DAMAGE *ptDamage);

#endif
