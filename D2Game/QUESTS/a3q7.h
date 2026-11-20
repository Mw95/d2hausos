#ifndef D2RE_D2GAME_A3Q7_H
#define D2RE_D2GAME_A3Q7_H

#include "../D2game.h"

void __fastcall A3Q7SetQuestStatus(GAME *ptGame, BYTE bRestore);
void __fastcall A3Q7ObjectsDone(GAME *ptGame, UNIT *hObject);
BOOL __fastcall A3Q7GetSpot(GAME *ptGame, UNIT *hUnit, GAMECOORD *ptCoord);
void __fastcall A3Q7InitDarkWandererStart(OBJINIT *ptInit);
void __fastcall A3Q7Init(QUEST *pQuest);
void __fastcall A3Q7StartTimer(GAME *ptGame, UNIT *hUnit);
void __fastcall A4Q0Init(QUEST *pQuest);

#endif
