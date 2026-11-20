#ifndef D2RE_D2GAME_A1Q5_H
#define D2RE_D2GAME_A1Q5_H

#include "../D2game.h"

struct A1Q5RECORD
{
 	DWORD		adwReportedIds[12];
 	WORD		wReportedCount;
 	BYTE		unobserved_32[2];
 	DWORD		adwCreditedIds[12];
 	WORD		wCreditedCount;
 	BYTE		unobserved_66[2];
 	DWORD		adwObjectIds[8];
 	WORD		wObjectCount;
 	BYTE		unobserved_8a[2];
 	QUESTPLAYERS	tPlayers;
 	GAMECOORD	tKillCoord;
 	BYTE		bFinished;
 	BYTE		bMonsterSpawned;
 	BYTE		bSequencePending;
 	BYTE		bObjectOperated;
 	BYTE		field_11c;
 	BYTE		unobserved_11d[3];
};
typedef struct A1Q5RECORD A1Q5RECORD;

struct A1Q6RECORD
{
 	QUESTPLAYERS	tPlayers1;
 	QUESTPLAYERS	tPlayers2;
 	QUESTPLAYERS	tPlayers3;
 	DWORD		dwBossId;
 	WORD		field_190;
 	WORD		wTimerRuns;
 	BYTE		bBossKilled;
 	BYTE		bStatus1Pending;
 	BYTE		unobserved_196[2];
};
typedef struct A1Q6RECORD A1Q6RECORD;

BOOL __fastcall A1Q4GetObjectSpot(GAME *ptGame, UNIT *hUnit, GAMECOORD *ptCoord);
BOOL __fastcall A1Q4SetSpot(GAME *ptGame, UNIT *hUnit);
void __fastcall A1Q4UnitArrived(GAME *ptGame, UNIT *hUnit);
void __fastcall A1Q4SetCainArrived(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall A1Q4TestCainActivated(GAME *ptGame);
void __fastcall A1Q4OperateDrop(QUEST *pQuest, UNIT *hObject);
int __fastcall A1Q5OperateObject(OBJOPERATE *pObjectParam);
void __fastcall A1Q5ObjectEvent(QUEST *pQuest, UNIT *hObject);
void __fastcall A1Q5Init(QUEST *pQuest);
void __fastcall A1Q5ObjInit(OBJINIT *ptInit);
void __fastcall A1Q5AddObject(OBJINIT *ptInit);
void __fastcall A1Q6PartyJoined(GAME *ptGame, UNIT *hUnit, void *pArg);

#endif
