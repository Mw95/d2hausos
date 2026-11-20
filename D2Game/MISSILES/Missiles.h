#ifndef D2RE_D2GAME_MISSILES_H
#define D2RE_D2GAME_MISSILES_H

#include "../D2game.h"

UNIT *__fastcall MissilesCreate(GAME *ptGame, MISSILECREATE *ptCreate);
void __fastcall MissilesSendToClient(CLIENT *ptClient, GAME *ptGame, UNIT *hMissile,
	BOOL fTarget);
BOOL __fastcall MissilesCollideMonster(UNIT *hUnit);
BOOL __fastcall MissilesCollidePlayer(UNIT *hUnit);
BOOL __fastcall MissilesCollideUnit(UNIT *hUnit);
BOOL __fastcall MissilesCollideMissile(UNIT *hUnit);
void __fastcall MissilesCreateInRadius(GAME *ptGame, UNIT *hUnit, int nRadius,
	int eMissile);

#endif
