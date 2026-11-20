#ifndef D2RE_D2GAME_QUESTSFX_H
#define D2RE_D2GAME_QUESTSFX_H

#include "../D2game.h"

void __fastcall QuestsFXDropGems(QUEST *pQuest, UNIT *hObject);
void __fastcall QuestsFXInit(QUEST *pQuest);
void __fastcall QuestsFXTalk(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall QuestsFXActive(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall QuestsFXOnBloodravenDeath(GAME *ptGame, UNIT *hUnit);
void __fastcall QuestsFXAffectNear(GAME *ptGame, UNIT *hUnit, int nRange, int nDelayMin, int nDelayMax,
	BOOL fUndead);
void __fastcall QuestsFXOnMephistoDeath(GAME *ptGame, UNIT *hUnit);
void __fastcall QuestsFXOnAndarielDeath(GAME *ptGame, UNIT *hUnit);
void __fastcall QuestsFXOnRadamentDeath(GAME *ptGame, UNIT *hUnit);
void __fastcall QuestsFXOpenStairs(GAME *ptGame, UNIT *hUnit);
void __fastcall QuestsFXOnIzualDeath(GAME *ptGame, UNIT *hUnit);

#endif
