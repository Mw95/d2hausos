#ifndef D2RE_D2GAME_A2Q4_H
#define D2RE_D2GAME_A2Q4_H

#include "../D2game.h"

void __fastcall A2Q4StaffMade(GAME *ptGame, UNIT *hUnit);
BYTE __fastcall A2Q2StatusFilter(QUEST *pQuest, UNIT *hPlayer, void *pGameRecord, void *pPlayerRecord,
	BYTE *pbStatus);
BOOL __fastcall A2Q4ObjectIdle(GAME *ptGame);
void __fastcall A2Q4Init(QUEST *pQuest);
void __fastcall A2Q4ObjInitMonster(QUEST *pQuest, OBJINIT *ptInit);
void __fastcall A2Q4ObjInitNpc(QUEST *pQuest, OBJINIT *ptInit);
BOOL __fastcall A2Q4TakeArrived(GAME *ptGame, UNIT *hUnit);
void __fastcall A2Q4OperateObject(QUEST *pQuest, UNIT *hObject);
void __fastcall A2Q4ObjInitCreated(OBJINIT *ptInit);
BOOL __fastcall A2Q4NotWaiting(GAME *ptGame);
BOOL __fastcall A2Q4ObjectOpen(GAME *ptGame);
BOOL __fastcall A2Q4GetSpot(GAME *ptGame, GAMECOORD *ptCoord);
void __fastcall A2Q4MonsterThink(GAME *ptGame, UNIT *hUnit, DWORD *pdwIdle, DWORD *pdwBusy);
int __fastcall A2Q4OperateTome(OBJOPERATE *pObjectParam);
void __fastcall A2Q4ObjInitLevel(OBJINIT *ptInit);

#endif
