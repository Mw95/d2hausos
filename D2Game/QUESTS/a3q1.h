#ifndef D2RE_D2GAME_A3Q1_H
#define D2RE_D2GAME_A3Q1_H

#include "../D2game.h"

void __fastcall A3Q1ObjInit(OBJINIT *ptInit);
void __fastcall A3Q1ObjectMode(QUEST *pQuest, OBJINIT *ptInit);
int __fastcall A3Q1OperateObject(OBJOPERATE *pObjectParam);
void __fastcall A3Q1Init(QUEST *pQuest);
BYTE __fastcall A3Q2Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);

#endif
