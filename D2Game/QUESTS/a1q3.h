#ifndef D2RE_D2GAME_A1Q3_H
#define D2RE_D2GAME_A1Q3_H

#include "../D2game.h"

void __fastcall A1Q3PartyCredit(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A1Q3Deactivate(GAME *ptGame, UNIT *hUnit);
int __fastcall A1Q3OperateObject(OBJOPERATE *pObjectParam);
void __fastcall A1Q3Init(QUEST *pQuest);
void __fastcall A1Q3ObjInit(QUEST *pQuest, OBJINIT *ptInit);

#endif
