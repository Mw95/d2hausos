#ifndef D2RE_D2GAME_SKILLSOR_H
#define D2RE_D2GAME_SKILLSOR_H

#include "../D2game.h"

BOOL __fastcall SkillsInferno(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsSorInfernoStart(GAME *ptGame, UNIT *hUnit, int nLevel, int nMissile);
BOOL __fastcall SkillsInfernoStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsFrozenArmor(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsFrozenArmorHit(GAME *ptGame, UNIT *hUnit, UNIT *hTarget);
BOOL __fastcall SkillsShiverArmor(GAME *ptGame, UNIT *hUnit, int nLevel);
WORD __fastcall SkillsShiverArmorHit(GAME *ptGame, UNIT *hUnit, UNIT *hTarget);
BOOL __fastcall SkillsTeleport(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMeteor(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsThunderStormStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsThunderStorm(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsEnergyShield(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsBlizzard(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsChillingArmor(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsHydraStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsHydra(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsFrozenOrb(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsSorNova(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsChargedBoltModify(UNIT *hMissile, DWORD dwArg);
BOOL __fastcall SkillsChargedBolt(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsBlaze(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsBlazeStep(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall SkillsFireWall(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsEnchant(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsChainLightning(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsStaticField(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsTelekinesisStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsTelekinesis(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsFrostNova(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsEnergyShieldAbsorb(UNIT *hAttacker, UNIT *hUnit, DAMAGE *ptDamage);
void __fastcall SkillsFireMasteryDamage(UNIT *hUnit, DAMAGE *ptDamage);
BOOL __fastcall SkillsSorStart(GAME *ptGame, UNIT *hUnit, int nSkillId, int nLevel);
BOOL __fastcall SkillsSorUse(GAME *ptGame, UNIT *hUnit, int nSkillId, int nLevel);
void __fastcall SkillsChillingArmorHit(GAME *ptGame, UNIT *hMissile, UNIT *hUnit);

#endif
