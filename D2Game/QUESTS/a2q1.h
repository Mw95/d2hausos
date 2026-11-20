#ifndef D2RE_D2GAME_A2Q1_H
#define D2RE_D2GAME_A2Q1_H

#include "../D2game.h"

void __fastcall A2Q0PlayerLeft(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q0PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q1Init(QUEST *pQuest);
void __fastcall A2Q1MonsterThink(GAME *ptGame, UNIT *hUnit);
BYTE __fastcall A2Q2Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
BYTE __fastcall A2Q2ItemState(QUEST *pQuest, UNIT *hUnit, BYTE *pbItem);

#endif
