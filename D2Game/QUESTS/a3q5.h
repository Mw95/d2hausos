#ifndef D2RE_D2GAME_A3Q5_H
#define D2RE_D2GAME_A3Q5_H

#include "../D2game.h"

BYTE __fastcall A3Q5Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall A3Q5AddMonster(GAME *ptGame, UNIT *hMonster);
int __fastcall A3Q5OperateObject(OBJOPERATE *pObjectParam);
void __fastcall A3Q5InitStairs(OBJINIT *ptInit);
void __fastcall A3Q5InitCompellingOrb(OBJINIT *ptInit);
void __fastcall A3Q5Init(QUEST *pQuest);
BOOL __fastcall A3Q5TestLevel(GAME *ptGame, UNIT *hUnit);
BYTE __fastcall A3Q6Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);

#endif
