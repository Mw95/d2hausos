#ifndef D2RE_D2GAME_A2Q8_H
#define D2RE_D2GAME_A2Q8_H

#include "../D2game.h"

BYTE __fastcall A2Q8StatusFilter(QUEST *pQuest, UNIT *hPlayer, void *pGameRecord, void *pPlayerRecord,
	BYTE *pbStatus);
BYTE __fastcall A2Q7Active(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall A2Q8Init(QUEST *pQuest);
void __fastcall A2Q8IntroInit(QUEST *pQuest);

#endif
