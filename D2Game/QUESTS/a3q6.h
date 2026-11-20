#ifndef D2RE_D2GAME_A3Q6_H
#define D2RE_D2GAME_A3Q6_H

#include "../D2game.h"

void __fastcall A3Q6ObjectOperated(QUEST *pQuest, UNIT *hObject);
void __fastcall A3Q6InitMephistoBridge(OBJINIT *ptInit);
void __fastcall A3Q6InitHellgate(OBJINIT *ptInit);
void __fastcall A3Q6Init(QUEST *pQuest);
BOOL __fastcall A3Q6SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A3Q6InitNatalyaStart(OBJINIT *ptInit);

#endif
