#ifndef D2RE_D2GAME_A4Q1_H
#define D2RE_D2GAME_A4Q1_H

#include "../D2game.h"

void __fastcall A4Q0NpcMessage(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A4Q0Talk(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A4Q0Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall A4Q1Init(QUEST *pQuest);
void __fastcall A4Q1MonsterSeen(GAME *ptGame);
BOOL __fastcall A4Q1NoPlayerNear(GAME *ptGame, UNIT *hUnit);
UNIT * __fastcall A4Q1UnitEvents(GAME *ptGame);

#endif
