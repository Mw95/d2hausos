#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\UNIT\\Friendly.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "Friendly.h"
#include "PlayerList.h"

#define UNIT_PLAYER			0

BOOL __fastcall FriendlyClearHostile(GAME *ptGame  , UNIT *hPlayer
, UNIT *hUnit)
{
#line 20
	D2ASSERT(UnitGetType (hPlayer) == UNIT_PLAYER);
	PlayerListAdd(ptGame, hPlayer, hUnit);
	PlayerListSetFlag(ptGame, hPlayer, hUnit, 8, 0);
	return TRUE;
}

void __fastcall FriendlySetHostile(GAME *ptGame  , UNIT *hPlayer
, UNIT *hUnit)
{
#line 35
	D2ASSERT(UnitGetType (hPlayer) == UNIT_PLAYER);
	PlayerListAdd(ptGame, hPlayer, hUnit);
	PlayerListSetFlag(ptGame, hPlayer, hUnit, 8, 1);
}
