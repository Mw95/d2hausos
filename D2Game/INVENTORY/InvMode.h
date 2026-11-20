#ifndef D2RE_D2GAME_INVMODE_H
#define D2RE_D2GAME_INVMODE_H

#include "../D2game.h"

void __fastcall INVENTORYModeSendItems(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient,
	void *hOtherInventory);
BOOL __fastcall INVENTORYModeSendItem(UNIT *hUnit, UNIT *hItem, UNIT *hUnused,
	CLIENT *ptClient);
void __fastcall INVENTORYModeClearItemFlags(GAME *ptGame, UNIT *hItem);
BOOL __fastcall INVENTORYModeHasItems(GAME *ptGame, UNIT *hUnit);
void __fastcall INVENTORYModeResetItems(GAME *ptGame, UNIT *hUnit);
void __fastcall INVENTORYModeSendGroundItem(UNIT *hItem, CLIENT *ptClient);
void __fastcall INVENTORYModeSendChangedItem(UNIT *hItem, CLIENT *ptClient);

#endif
