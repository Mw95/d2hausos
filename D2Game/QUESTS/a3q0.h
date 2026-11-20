#ifndef D2RE_D2GAME_A3Q0_H
#define D2RE_D2GAME_A3Q0_H

#include "../D2game.h"

void __fastcall A3Q0IntroTalk(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A3Q0IntroStatusFilter(QUEST *pQuest, UNIT *hPlayer, void *pGameRecord, void *pPlayerRecord,
	BYTE *pbStatus);
BYTE __fastcall A3Q0IntroActive(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall A3Q0Init(QUEST *pQuest);
void __fastcall A3Q0ObjInit(OBJINIT *ptInit);

#endif
