#ifndef D2RE_D2GAME_PARTY_H
#define D2RE_D2GAME_PARTY_H

#include "../D2game.h"

typedef void (__fastcall *PFNPARTYCALLBACK)(GAME *ptGame, UNIT *hUnit, void *pArg);

void __fastcall PartySystemCreate(GAME *ptGame);
void __fastcall PartySystemFree(GAME *ptGame);
WORD __fastcall PartyCreate(GAME *ptGame);
BOOL __fastcall PartyAddPlayer(GAME *ptGame, WORD wParty, UNIT *hPlayer);
void __fastcall PartyRemovePlayer(GAME *ptGame, UNIT *hPlayer);
int __fastcall PartyGetLevelSum(GAME *ptGame, UNIT *hUnit);
void __fastcall PartyForEach(GAME *ptGame, WORD wParty, PFNPARTYCALLBACK pfnCallback, void *pArg);
void __fastcall PartyForEachInLevel(GAME *ptGame, UNIT *hUnit, PFNPARTYCALLBACK pfnCallback, void *pArg);
int __fastcall PartyShareGold(GAME *ptGame, UNIT *hUnit, int nGold);
void __fastcall PartySendLives(GAME *ptGame, UNIT *hUnit);
WORD __fastcall PartyGetId(GAME *ptGame, UNIT *hPlayer);

#endif
