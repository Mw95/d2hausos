#ifndef D2RE_D2GAME_MONSTER_H
#define D2RE_D2GAME_MONSTER_H

#include "../D2game.h"

MONSTERREGIONREF *__fastcall MonsterAllocRegionRef(GAME *ptGame);
void __fastcall MonsterFreeRegionRef(GAME *ptGame, MONSTERREGIONREF *ptRef);
void __fastcall MonsterRegionRefSetPrevMode(MONSTERREGIONREF *ptRef,
	DWORD dwValue);
void __fastcall MonsterRegionRefSetLevel(MONSTERREGIONREF *ptRef,
	DWORD dwLevelId);
void __fastcall MonsterInitUnit(GAME *ptGame, GAMEROOM *hRoom, UNIT *hUnit, DWORD dwUnitId);
void __fastcall MonsterStartMode(GAME *ptGame, UNIT *hUnit, DWORD dwX,
	DWORD dwY, DWORD dwUnused);
void __fastcall MonsterRemoveEvents(GAME *ptGame, UNIT *hUnit);
void __fastcall MonsterGetScale(MONSTERSCALE *ptScale, GAME *ptGame,
	DWORD dwUnused1, DWORD dwUnused2);
void __fastcall MonsterInitComponents(MONPOPSLOT *ptSlot, UNIT *hUnit);
BOOL __fastcall MonsterHasComponents(UNIT *hUnit);
BOOL __fastcall MonsterIsShieldEquipped(UNIT *hMonster);
void __fastcall MonsterChangeClass(GAME *ptGame, UNIT *hUnit, DWORD dwClassId,
	DWORD dwMode);

#endif
