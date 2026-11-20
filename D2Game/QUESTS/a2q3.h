#ifndef D2RE_D2GAME_A2Q3_H
#define D2RE_D2GAME_A2Q3_H

#include "../D2game.h"

void __fastcall A2Q2PlayerJoined(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q2PlayerDropped(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A2Q3NoStaff(GAME *ptGame, UNIT *hUnit);
int __fastcall A2Q2OperateScroll(OBJOPERATE *pObjectParam);
int __fastcall A2Q2OperateShaft(OBJOPERATE *pObjectParam);
int __fastcall A2Q2OperateBox(OBJOPERATE *pObjectParam);
void __fastcall A2Q3Nothing(GAME *ptGame, UNIT *hUnit);
void __fastcall A2Q2BoxState(GAME *ptGame, int *pnCount, BYTE *pbFlag);
void __fastcall A2Q3ObjInit(QUEST *pQuest, OBJINIT *ptInit);
void __fastcall A2Q3Init(QUEST *pQuest);
int __fastcall A2Q3OperateObject(OBJOPERATE *pObjectParam);
void __fastcall A2Q3ActLoaded(GAME *ptGame, BYTE bAct);

#endif
