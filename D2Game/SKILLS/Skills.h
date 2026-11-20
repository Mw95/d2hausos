#ifndef D2RE_D2GAME_SKILLS_H
#define D2RE_D2GAME_SKILLS_H

#include "../D2game.h"

typedef BOOL (__fastcall *PFNSKILLUNITTEST)(GAME *ptGame, UNIT *hUnit);
typedef void (__fastcall *PFNSTATEREMOVE)(UNIT *hUnit, DWORD dwState);
typedef void (__fastcall *PFNSKILLCURSE)(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel);

UNIT * __fastcall SkillsFindNearest(GAME *ptGame, UNIT *hUnit, int nRange, PFNSKILLUNITTEST pfnTest);
BOOL __fastcall SkillsUseMana(UNIT *hUnit);
BOOL __fastcall SkillsSpendMana(UNIT *hUnit, int nMana);
BOOL __fastcall SkillsHasMana(UNIT *hUnit);
UNITFINDDATA * __fastcall SkillsFindUnits(GAME *ptGame, UNIT *hUnit, int nRange, int nX, int nY);
void __fastcall SkillsFreeUnits(GAME *ptGame, UNITFINDDATA *ptFind);
BOOL __fastcall SkillsUseHit(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsStateRemove(UNIT *hUnit, DWORD dwState);
void * __fastcall SkillsSetState(SKILLSTATE *ptData);
void * __fastcall SkillsSetSkillState(UNIT *hSource, UNIT *hTarget, int nSkill, int nLevel, int nStat,
	int nState, PFNSTATEREMOVE pfnRemove);
int __fastcall SkillsUseQuantity(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
UNIT * __fastcall SkillsCreateMissile(GAME *ptGame, int nMissile, UNIT *hUnit, int nLevel, int nOffsetX,
	int nOffsetY, int nTargetX, int nTargetY, BOOL bUseQuantity);
UNIT * __fastcall SkillsCreateMissileAt(GAME *ptGame, UNIT *hUnit, int nLevel, int nMissile, int nX, int nY);
UNIT * __fastcall SkillsThrowMissile(GAME *ptGame, int nMissile, UNIT *hUnit, int nLevel, BOOL bUseQuantity);
int __fastcall SkillsUseThrowQuantity(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall SkillsHasThrowQuantity(UNIT *hUnit);
BOOL __fastcall SkillsAttackStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsAttack(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsThrowDamage(UNIT *hUnit, UNIT *hMissile, UNIT *hItem);
BOOL __fastcall SkillsThrow(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsKickStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsUnsummonStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsUnsummon(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsLeftHandSwing(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsLeftHandThrow(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsTransmogrify(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
BOOL __fastcall SkillsIsClassSkill(UNIT *hUnit, int nSkillId);
BOOL __fastcall SkillsStart(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall SkillsDispatch(GAME *ptGame, UNIT *hUnit, int eSkillId, int nLevel, int nTableColumn);
void __fastcall SkillsUse(GAME *ptGame, UNIT *hUnit);
void __fastcall SkillsEventPeriodic(GAME *ptGame, UNIT *hUnit);
void __fastcall SkillsEventAura(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall SkillsAdd(UNIT *hUnit, int nSkill);

void __fastcall SkillsSetCurrent(UNIT *hUnit, BOOL fLeft, int nSkill);
UNIT * __fastcall SkillsOpenPortalEnd(GAME *ptGame, UNIT *hPortal, int nLevelId, int nLevelFrom);
BOOL __fastcall SkillsOpenPortal(GAME *ptGame, UNIT *hOwner, GAMEROOM *hRoom, int nX, int nY, int nLevelId,
	UNIT **phPortal, int nClassId, BOOL fExact);
BOOL __fastcall SkillsGetTargetSpot(GAME *ptGame, UNIT *hUnit, int *pnX, int *pnY);
void __fastcall SkillsStepLengthen(int *pnX, int *pnY);
void __fastcall SkillsStepTurn(int *pnX, int *pnY);
void __fastcall SkillsNova(GAME *ptGame, UNIT *hUnit, UNIT *hCentre, int nMissile, int nLevel, int nVelocity);
void __fastcall SkillsActiveStates(GAME *ptGame, UNIT *hUnit);
UNIT * __fastcall SkillsSummon(GAME *ptGame, SKILLSUMMON *ptSummon);
UNIT * __fastcall SkillsDropItem(GAME *ptGame, UNIT *hUnit, DWORD dwCode, DWORD dwQuality);
void __fastcall SkillsCurseArea(GAME *ptGame, DWORD dwFlags, UNIT *hUnit, int nLevel, int nSkill,
	PFNSKILLCURSE pfnCurse);
void __fastcall SkillsAreaEffect(GAME *ptGame, SKILLEFFECT *ptEffect);
void __fastcall SkillsAreaEffectAt(GAME *ptGame, DWORD dwFlags, int nX, int nY, UNIT *hUnit, int nSkill,
	int nLevel, PFNAREAAFFECT pfnAreaAffect);
void __fastcall SkillsSetLevel(UNIT *hUnit, int nSkill, int nLevel, DWORD dwArg);
void __fastcall SkillsRestorePassives(UNIT *hUnit);
void __fastcall SkillsGetDamage(UNIT *hUnit, int eMode, int *pnDamage);
void __fastcall SkillsElemDamage(UNIT *hUnit, DAMAGE *ptDamage, int nSkill, int nLevel);
void __fastcall SkillsFindTargets(GAME *ptGame, SKILLFINDTARGET *ptTarget, UNITFINDDATA *ptFind);
UNIT * __fastcall SkillsFindTarget(GAME *ptGame, SKILLFINDTARGET *ptTarget);
void __fastcall SkillsLeapHit(GAME *ptGame, UNIT *hUnit, UNIT *hTarget);
UNIT * __fastcall SkillsFindNext(GAME *ptGame, UNIT *hUnit, DWORD dwUnitId, int nRange, DWORD dwFlags);
int __fastcall SkillsCountTargets(GAME *ptGame, UNIT *hUnit, int nRange);
void __fastcall SkillsSlowAnim(UNIT *hUnit, int nPercent);
BOOL __fastcall SkillsWhirlAt(GAME *ptGame, UNIT *hPlayer, UNIT *hTarget);
BOOL __fastcall SkillsCanBeAffected(UNIT *hUnit, int nKind);

#endif
