#ifndef D2RE_D2GAME_SUNITDMG_H
#define D2RE_D2GAME_SUNITDMG_H

#include "../D2game.h"

BOOL __fastcall SUnitDmgSetFlag(DAMAGE *ptDamage, DWORD dwFlag);
int __fastcall SUnitDmgRollDamage(UNIT *hUnit, int nMin, int nMax, int nPercent, int nAdd);
void __fastcall SUnitDmgResist(UNIT *hAttacker, UNIT *hTarget, DAMAGE *ptDamage);
void __fastcall SUnitDmgApply(GAME *ptGame, UNIT *hAttacker, UNIT *hTarget, BOOL bResist, DAMAGE *ptDamage);
void __fastcall SUnitDmgPoison(UNIT *hAttacker, UNIT *hTarget, int nDamage, int nLength);
void __fastcall SUnitDmgChill(UNIT *hAttacker, UNIT *hTarget, int nLength);
void __fastcall SUnitDmgLeech(UNIT *hAttacker, UNIT *hTarget, int nLife, DAMAGE *ptDamage);
void __fastcall SUnitDmgFreeHeld(GAME *ptGame, UNIT *hUnit);
void __fastcall SUnitDmgFreeHold(GAME *ptGame, UNIT *hUnit, UNIT *hTarget);
void __fastcall SUnitDmgKill(GAME *ptGame, UNIT *hUnit, UNIT *hKiller, BOOL bRemovePet);
void __fastcall SUnitDmgReact(GAME *ptGame, UNIT *hAttacker, UNIT *hEnemy, DAMAGE *ptDamage);
void __fastcall SUnitDmgDeliver(GAME *ptGame, UNIT *hAttacker, UNIT *hTarget);
DAMAGE *__fastcall SUnitDmgFindHeld(UNIT *hUnit, UNIT *hTarget);
WORD __fastcall SUnitDmgGetHeldFlags(UNIT *hUnit, UNIT *hTarget);
BOOL __stdcall SUnitDmgRollRange(UNIT *hUnit, int *pnFirst, int *pnSecond);
BOOL __fastcall SUnitDmgRollHit(UNIT *hAttacker, UNIT *hTarget, int nToHit, BOOL bMissile);
WORD __fastcall SUnitDmgHit(GAME *ptGame, UNIT *hAttacker, UNIT *hTarget, int nToHit);
void __fastcall SUnitDmgHold(GAME *ptGame, UNIT *hUnit, UNIT *ptDestUnit, DAMAGE *ptDamage);
void __fastcall SUnitDmgRoll(UNIT *hAttacker, UNIT *hTarget, DAMAGE *ptDamage);
void __fastcall SUnitDmgAwardExp(GAME *ptGame, UNIT *hKiller, UNIT *hUnit, int nMonLevel, int nExp);
void __fastcall SUnitDmgScaleExp(UNIT *hPlayer, int *pnExp, int nLevel, int nMonLevel);
void __fastcall SUnitDmgShareExp(GAME *ptGame, UNIT *hPlayer, void *pArg);
DWORD __fastcall SUnitDmgBlock(UNIT *hAttacker, UNIT *hTarget, BOOL bMissile, BOOL bBlock);
DWORD __fastcall SUnitDmgEvade(UNIT *hAttacker, UNIT *hTarget, BOOL bMissile);
typedef BOOL (__fastcall *PFNSPLASHTEST)(GAME *ptGame, UNIT *hSource, UNIT *hUnit);
BOOL __fastcall SUnitDmgSplash(GAME *ptGame, UNIT *hSource, int nX, int nY, int nRadius, DAMAGE *ptDamage,
	BOOL bOwner, BOOL bOwnerOwner, PFNSPLASHTEST pfnCallBack, DWORD dwFlags);

#endif
