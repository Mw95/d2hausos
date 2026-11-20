#ifndef D2RE_D2GAME_A1Q1_H
#define D2RE_D2GAME_A1Q1_H

#include "../D2game.h"

void __fastcall A1Q1WarrivNpcMessage(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q1WarrivTalk(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A1Q1WarrivActive(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
DWORD __fastcall A1Q1GetRemaining(QUEST *pQuest);
void __fastcall A1Q1Init(QUEST *pQuest);
BYTE __fastcall A1Q1IsDone(QUEST *pQuest);
void __fastcall A1Q2PartyCredit(GAME *ptGame, UNIT *hUnit, void *pArg);

#endif
