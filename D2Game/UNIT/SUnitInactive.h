#ifndef D2RE_D2GAME_SUNITINACTIVE_H
#define D2RE_D2GAME_SUNITINACTIVE_H

#include "../D2game.h"

struct INACTIVENODE;
struct INACTIVEITEM;
struct INACTIVEMONSTER;

void __fastcall SUnitInactiveRestoreRoom(GAME *ptGame, GAMEROOM *hRoom);
void __fastcall SUnitInactiveFreeAll(GAME *ptGame);
void __fastcall SUnitInactiveRemoveUnit(GAME *ptGame, UNIT *hUnit);
void __fastcall SUnitInactiveRemoveClass(GAME *ptGame, WORD wType, WORD wClassId, BYTE bAct);

UNIT *__fastcall SUnitInactiveRestoreItem(GAME *ptGame, GAMEROOM *hRoom, struct INACTIVEITEM *ptRecord);
void __fastcall SUnitInactiveSave(GAME *ptGame, UNIT *hUnit);
void __fastcall SUnitInactiveSaveItem(GAME *ptGame, UNIT *hItem, struct INACTIVENODE *ptNode,
	struct INACTIVEITEM **pptOut);
struct INACTIVENODE *__fastcall SUnitInactiveFindNode(GAME *ptGame, GAMEROOM *hRoom, BOOL fCreate);
void __fastcall SUnitInactiveRestoreAi(GAME *ptGame, UNIT *hUnit, struct INACTIVEMONSTER *ptRecord);

#endif
