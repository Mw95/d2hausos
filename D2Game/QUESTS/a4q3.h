#ifndef D2RE_D2GAME_A4Q3_H
#define D2RE_D2GAME_A4Q3_H

#include "../D2game.h"

void __fastcall A4Q3ReleaseMonsters(GAME *ptGame, A4Q2RECORD *pQuestRecord);
void __fastcall A4Q3InitObject(OBJINIT *ptInit);
int __fastcall A4Q3OperateSealFirst(OBJOPERATE *pObjectParam);
int __fastcall A4Q3OperateSealSecond(OBJOPERATE *pObjectParam);
int __fastcall A4Q3OperateSealThird(OBJOPERATE *pObjectParam);
int __fastcall A4Q3OperateObject(OBJOPERATE *pObjectParam);
void __fastcall A4Q3ObjectEvent(GAME *ptGame, UNIT *hObject);
BOOL __fastcall A4Q3MonsterKilled(GAME *ptGame);
void __fastcall A4Q3InitObjectMode(OBJINIT *ptInit);
int __fastcall A4Q3OperateHellforge(OBJOPERATE *pObjectParam);
void __fastcall A4Q3Init(QUEST *pQuest);

#endif
