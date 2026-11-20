#ifndef D2RE_D2GAME_MONSTERMODE_H
#define D2RE_D2GAME_MONSTERMODE_H

#include "../D2game.h"

struct MONMODEENTRY;

void __fastcall MonsterModeCriticalHit(UNIT *hUnit, UNIT *hTarget,
	DAMAGE *ptDamage);
void __fastcall MonsterModeClearOverride(UNIT *hUnit);
void __fastcall MonsterModeSetOverride(MONMODEDATA *ptModeData,
	DWORD dwPathType, DWORD dwVelocity, BYTE bDistMax);
MONMODEDATA * __fastcall MonsterModeCreate(GAME *ptGame);
void __fastcall MonsterModeFree(GAME *ptGame, MONMODEDATA *ptModeData);
void __fastcall MonsterModeDropItems(GAME *ptGame, MONMODECHANGE *ptReq, DWORD dwUnused);
UNIT * __fastcall MonsterModeCreateItem(GAME *ptGame, UNIT *hUnit, DWORD dwCode,
	int nQuality);
void __fastcall MonsterModeAdjustQuality(MONMODECHANGE *ptReq, int *pnQuality, DWORD dwCode);
void __fastcall MonsterModeGoldFind(UNIT *hItem, MONMODECHANGE *ptReq);
DWORD __fastcall MonsterModeChooseItem(UNIT *hUnit, DWORD dwCode);
void __fastcall MonsterModeDropMagic(GAME *ptGame, UNIT *hUnit, MONMODECHANGE *ptReq,
	int nQuality);
void __fastcall MonsterModeDropBoss(GAME *ptGame, UNIT *hUnit, MONMODECHANGE *ptReq,
	int nBoss);
void __fastcall MonsterModeDropTreasure(GAME *ptGame, UNIT *hUnit, BOOL bChampion,
	MONMODECHANGE *ptReq, BOOL *pbDropped);
void __fastcall MonsterModeRegen(GAME *ptGame, UNIT *hUnit);
void __fastcall MonsterModeUseSkill(GAME *ptGame, UNIT *hUnit, int nSkillArg);
void __fastcall MonsterModeEndAttack(GAME *ptGame, UNIT *hUnit);
void __fastcall MonsterModeEndSpecialA(GAME *ptGame, UNIT *hUnit);
void __fastcall MonsterModeEndSpecialB(GAME *ptGame, UNIT *hUnit);
int __fastcall MonsterModeUpdateWait(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall MonsterModeStartDeath(GAME *ptGame, MONMODECHANGE *ptReq);
void __fastcall MonsterModeKill(GAME *ptGame, UNIT *hUnit, BYTE bDirection);
void __fastcall MonsterModeDeathBlast(GAME *ptGame, UNIT *hUnit);
void __fastcall MonsterModeEndDeath(GAME *ptGame, UNIT *hUnit);
int __fastcall MonsterModeUpdateDeath(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall MonsterModeStartDead(GAME *ptGame, MONMODECHANGE *ptReq);
BOOL __fastcall MonsterModeStartNeutral(GAME *ptGame, MONMODECHANGE *ptReq);
BOOL __fastcall MonsterModeStartSkillThree(GAME *ptGame, MONMODECHANGE *ptReq);
int __fastcall MonsterModeUpdateSkillThree(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall MonsterModeStartSkillFour(GAME *ptGame, MONMODECHANGE *ptReq);
BOOL __fastcall MonsterModeStartWalk(GAME *ptGame, MONMODECHANGE *ptReq);
int __fastcall MonsterModeUpdateWalk(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall MonsterModeStartRun(GAME *ptGame, MONMODECHANGE *ptReq);
int __fastcall MonsterModeUpdateRun(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall MonsterModeStartGetHit(GAME *ptGame, MONMODECHANGE *ptReq);
BOOL __fastcall MonsterModeStartAttack(GAME *ptGame, MONMODECHANGE *ptReq);
int __fastcall MonsterModeUpdateAttack(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall MonsterModeStartBlock(GAME *ptGame, MONMODECHANGE *ptReq);
BOOL __fastcall MonsterModeStartKnockback(GAME *ptGame, MONMODECHANGE *ptReq);
void __fastcall MonsterModeEndKnockback(GAME *ptGame, UNIT *hUnit);
int __fastcall MonsterModeUpdateKnockback(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall MonsterModeStartSequence(GAME *ptGame, MONMODECHANGE *ptReq);
int __fastcall MonsterModeUpdateSequence(GAME *ptGame, UNIT *hUnit);
const struct MONMODEENTRY * __fastcall MonsterModeGetEntry(UNIT *hUnit, int eMode);
void __fastcall MonsterModeUpdate(GAME *ptGame, UNIT *hUnit);
void __fastcall MonsterModeEnd(GAME *ptGame, UNIT *hUnit);
BOOL __stdcall MonsterModeSet(GAME *ptGame, MONMODECHANGE *ptReq, BOOL bArg);
int __fastcall MonsterModeSetPath(UNIT *hUnit, int nPathType,
	MONMODEDATA *ptModeData, BYTE bDistMax);
void __stdcall MonsterModeInit(UNIT *hUnit, int eMode, MONMODECHANGE *ptReq);
void __fastcall MonsterModeEventClearTarget(GAME *ptGame, UNIT *hUnit);
void __fastcall MonsterModeProcessEvent(GAME *ptGame, UNIT *hUnit, int eEvent);
UNIT * __fastcall MonsterModeDropTreasureItem(GAME *ptGame, UNIT *hUnit, int nQuality);
void __fastcall MonsterModeUpdateClient(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient);

#endif
