#ifndef D2RE_D2GAME_PARTYSCREEN_H
#define D2RE_D2GAME_PARTYSCREEN_H

#include "../D2game.h"

void __fastcall PartyScreenOnCancel(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg);
void __fastcall PartyScreenOnInvite(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg);
void __fastcall PartyScreenOnLeave(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg);
void __fastcall PartyScreenOnHostile(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg);
void __fastcall PartyScreenReset(GAME *ptGame, UNIT *hUnit);
void __fastcall PartyScreenCommand(GAME *ptGame, UNIT *hUnit, int eCommand, DWORD dwUnitId, DWORD dwArg);

#endif
