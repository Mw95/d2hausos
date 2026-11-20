#ifndef D2RE_D2GAME_MONSTERSPAWN_H
#define D2RE_D2GAME_MONSTERSPAWN_H

#include "../D2game.h"

struct DRLGLOGICNODE;

void __fastcall MonsterUniqueMinionAddLife(UNIT *hMinion, UNIT *hLeader, DWORD dwUnused);
UNIT *__fastcall MonsterSpawn(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY, int nClass,
	DWORD dwMode, int nRadius,
		WORD wFlags);
UNIT *__fastcall MonsterSpawnMinion(GAME *ptGame, struct DRLGLOGICNODE *ptArea,
	UNIT *hLeader, int nClass, DWORD dwMode, int nRadius,
	WORD wFlags);
UNIT *__fastcall MonsterSpawnInArea(GAME *ptGame, GAMEROOM *hRoom,
	struct DRLGLOGICNODE *ptArea,
	int nX, int nY, int nClass, DWORD dwMode, int nRadius,
	WORD wFlags);
UNIT *__fastcall MonsterSpawnAt(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY, int nClass,
	DWORD dwMode, WORD wFlags);
UNIT *__fastcall MonsterSpawnWithArg(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY, int nClass,
	DWORD dwMode, DWORD dwUnitId, int nRadius,
	WORD wFlags);
UNIT *__fastcall MonsterSpawnFromPopulation(GAME *ptGame, UNIT *hUnit, int nIndex,
	DWORD dwMode, int nRadius,
		WORD wFlags);
BOOL __fastcall MonsterSpawnMinions(GAME *ptGame, UNIT *hLeader, int nClass,
	DWORD dwMode, int nRadius, int nCount,
	WORD wFlags);
BOOL __fastcall MonsterSpawnFormation(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY,
	UNIT *hOwner,
	int nClass, DWORD dwMode, const FORMATIONPOINT *ptFormation,
	WORD wFlags);
BOOL __fastcall MonsterSpawnFormationAt(GAME *ptGame, GAMEROOM *hRoom, DWORD dwUnused1,
	DWORD dwUnused2, UNIT *hUnit, int nClass,
		WORD wFlags);
BOOL __fastcall MonsterUniqueSpawnGroup(GAME *ptGame, UNIT *hLeader, int nClass,
	DWORD dwMode, int nCount, int nKind,
	WORD wFlags);

#endif
