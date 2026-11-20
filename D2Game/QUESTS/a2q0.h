#ifndef D2RE_D2GAME_A2Q0_H
#define D2RE_D2GAME_A2Q0_H

#include "../D2game.h"

void __fastcall A2Q0IntroMet(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q0IntroTalk(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A2Q0IntroActive(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall A2Q0Init(QUEST *pQuest);

#endif
