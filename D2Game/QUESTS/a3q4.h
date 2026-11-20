#ifndef D2RE_D2GAME_A3Q4_H
#define D2RE_D2GAME_A3Q4_H

#include "../D2game.h"

void __fastcall A3Q4Init(QUEST *pQuest);
BOOL __fastcall A3Q4MonsterAct(QUEST *pQuest, UNIT *hMonster);
void __fastcall A3Q4UnitAct(QUEST *pQuest, UNIT *hUnit);
BOOL __fastcall A3Q4TestFlag(GAME *ptGame);
void __fastcall A3Q4ClearFlag(GAME *ptGame);

#endif
