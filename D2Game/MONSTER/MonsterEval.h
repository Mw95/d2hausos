#ifndef D2RE_D2GAME_MONSTEREVAL_H
#define D2RE_D2GAME_MONSTEREVAL_H

#include "../D2game.h"

FORMATIONPOINT * __fastcall MonsterPickFormation(GAMEROOM *hRoom);
int __fastcall MonsterRegionGetKind(MONREGION *ptRegion, GAMEROOM *hRoom);
BOOL __fastcall MonsterEvalIsAlive(UNIT *hUnit);
void __fastcall MonsterEvalKilled(UNIT *hKiller, UNIT *hUnit, GAME *ptGame);
void __fastcall MonsterEvalSetDamage(UNIT *hUnit, int eMode);
WORD __fastcall MonsterEvalHit(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg,
	DWORD dwUnused);
void __fastcall MonsterEvalAttack(UNIT *hUnit, GAME *ptGame);
void __fastcall MonsterEvalDamageFlags(GAME *ptGame, UNIT *hAttacker, UNIT *hUnit,
	DAMAGE *ptDamage);

#endif
