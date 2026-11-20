#ifndef D2RE_D2GAME_A2Q6_H
#define D2RE_D2GAME_A2Q6_H

#include "../D2game.h"

void __fastcall A2Q6Init(QUEST *pQuest);
void __fastcall A2Q6SendLevel(GAME *ptGame, UNIT *hUnit);
DWORD __fastcall A2Q6GetClass(GAME *ptGame);
void __fastcall A2Q6OperateTiles(OBJOPERATE *pObjectParam);
void __fastcall A2Q6InitRecordedObject(OBJINIT *ptInit);
void __fastcall A2Q6InitTyraelDoor(OBJINIT *ptInit);
BOOL __fastcall A2Q6TestLevel(GAME *ptGame, UNIT *hUnit, DWORD dwLevelId);
void __fastcall A2Q6InitOrifice(OBJINIT *ptInit);
int __fastcall A2Q6OperateObject(OBJOPERATE *pObjectParam);
void __fastcall A2Q6ObjectUsed(GAME *ptGame, UNIT *hPlayer, UNIT *hObject);
BOOL __fastcall A2Q6MonsterTest(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall A2Q6PortalSpot(GAME *ptGame, GAMECOORD *ptCoord, GAMEROOM **phRoom);

#endif
