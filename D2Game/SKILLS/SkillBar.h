#ifndef D2RE_D2GAME_SKILLBAR_H
#define D2RE_D2GAME_SKILLBAR_H

#include "../D2game.h"

BOOL __fastcall SkillsBashStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsDoubleSwingStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsDoubleSwing(GAME *ptGame, UNIT *hUnit, int nLevel);
UNIT * __fastcall SkillsBarNextTarget(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall SkillsHowl(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsFindPotionStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsFindPotion(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsTaunt(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsTauntTest(GAME *ptGame, UNIT *hUnit);
void __fastcall SkillsShoutApply(GAME *ptGame, UNIT *hTarget, UNIT *hOwner, int nLevel);
BOOL __fastcall SkillsShout(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsStunStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsDoubleThrow(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsFindItemStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsFindItem(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsLeapStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsLeapBegin(GAME *ptGame, UNIT *hUnit, DWORD hSkill);
BOOL __fastcall SkillsLeapStep(GAME *ptGame, UNIT *hUnit, DWORD hSkill);
BOOL __fastcall SkillsLeap(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsLeapAttackStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsLeapAttack(GAME *ptGame, UNIT *hUnit, int nLevel);
UNIT * __fastcall SkillsLeapAttackTarget(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall SkillsBattleCry(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsFrenzyStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsFrenzy(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsFrenzyState(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsFrenzyStateEnd(UNIT *hUnit, DWORD dwState);
void __fastcall SkillsBattleCommandApply(GAME *ptGame, UNIT *hTarget, UNIT *hOwner, int nLevel);
void __fastcall SkillsBattleCommandEnd(UNIT *hUnit, DWORD dwState);
void __fastcall SkillsBattleOrdersApply(GAME *ptGame, UNIT *hTarget, UNIT *hOwner, int nLevel);
void __fastcall SkillsBattleOrdersEnd(UNIT *hUnit, DWORD dwState);
BOOL __fastcall SkillsBattleOrders(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsGrimWardStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsGrimWard(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsWhirlwindStart(GAME *ptGame, UNIT *hPlayer, int nLevel);
BOOL __fastcall SkillsWhirlwind(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsBerserkStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsWarCry(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsBattleCommand(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsConcentrateStart(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsConcentrateEnd(UNIT *hUnit, DWORD dwState);
BOOL __fastcall SkillsConcentrate(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsBarStart(GAME *ptGame, UNIT *hUnit, int nSkillId, int nLevel);
BOOL __fastcall SkillsBarUse(GAME *ptGame, UNIT *hUnit, int nSkillId, int nLevel);

#endif
