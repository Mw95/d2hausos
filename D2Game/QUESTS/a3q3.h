#ifndef D2RE_D2GAME_A3Q3_H
#define D2RE_D2GAME_A3Q3_H

#include "../D2game.h"

void __fastcall A3Q2ObjectOperated(QUEST *pQuest, UNIT *hLever);
void __fastcall A3Q2InitSewerStairs(OBJINIT *ptInit);
void __fastcall A3Q2InitSewerLever(OBJINIT *ptInit);
void __fastcall A3Q2FlailMade(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall A3Q2NoFlail(GAME *ptGame, UNIT *hUnit);
int __fastcall A3Q2OperateHeartChest(OBJOPERATE *pObjectParam);
int __fastcall A3Q2OperateEyeChest(OBJOPERATE *pObjectParam);
int __fastcall A3Q2OperateBrainChest(OBJOPERATE *pObjectParam);
void __fastcall A3Q3Init(QUEST *pQuest);
void __fastcall A3Q3InitGidbinnDecoy(OBJINIT *ptInit);
int __fastcall A3Q3OperateObject(OBJOPERATE *pObjectParam);
BOOL __fastcall A3Q3GetSpot(GAME *ptGame, GAMECOORD *ptCoord);
void __fastcall A3Q3OpenGate(GAME *ptGame);
void __fastcall A3Q3InitGidbinnAltar(OBJINIT *ptInit);
BYTE __fastcall A3Q4Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);

#endif
