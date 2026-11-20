#ifndef D2RE_D2GAME_A2Q5_H
#define D2RE_D2GAME_A2Q5_H

#include "../D2game.h"
#include "QUESTS.h"

void __fastcall A2Q4LevelObjects(GAME *ptGame, DWORD dwLevelId);
void __fastcall A2Q5PlayerDone(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A2Q5Init(QUEST *pQuest);
void __fastcall A2Q5BossSeen(GAME *ptGame  );
UNIT * __fastcall A2Q6EventAll(GAME *ptGame);
void __fastcall A2Q6PartyCredit(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A2Q6StatusFilter(QUEST *pQuest, UNIT *hPlayer, void *pGameRecord, void *pPlayerRecord,
	BYTE *pbStatus);

#endif
