#ifndef D2RE_D2GAME_FRIENDLY_H
#define D2RE_D2GAME_FRIENDLY_H

#include "../D2game.h"

BOOL __fastcall FriendlyClearHostile(GAME *ptGame, UNIT *hPlayer, UNIT *hUnit);
void __fastcall FriendlySetHostile(GAME *ptGame, UNIT *hPlayer, UNIT *hUnit);

#endif
