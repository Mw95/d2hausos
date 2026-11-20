#ifndef D2RE_D2GAME_PLRTRADE_H
#define D2RE_D2GAME_PLRTRADE_H

#include "../D2game.h"

void __fastcall PlrTradeCommand(GAME *ptGame, UNIT *hUnit, WORD wCmd,
	DWORD dwArg);
void __fastcall PlrTradeTransmute(GAME *ptGame, UNIT *hUnit);
void __fastcall PlrTradeCancel(GAME *ptGame, UNIT *hUnit, UNIT *hOther,
	PLRINFO *ptPlayerInfo1, PLRINFO *ptPlayerInfo2);
void __fastcall PlrTradeRestoreItems(GAME *ptGame, UNIT *hUnit, BOOL fUnused);
void __fastcall PlrTradeSendUnitGone(GAME *ptGame, UNIT *hPlayer, void *pArg);
void __fastcall PlrTradeCommit(GAME *ptGame, UNIT *hUnit, UNIT *hOther,
	PLRINFO *ptPlayerInfo1, PLRINFO *ptPlayerInfo2);
void __fastcall PlrTradeReset(GAME *ptGame, UNIT *hUnit, UNIT *hOther,
	PLRINFO *ptPlayerInfo1, PLRINFO *ptPlayerInfo2);
BYTE __fastcall PlrTradeMoveItems(GAME *ptGame, UNIT *hFrom, UNIT *hTo);
void __fastcall PlrTradeLeave(GAME *ptGame, UNIT *hUnit);
void __fastcall PlrTradeInteractEnd(GAME *ptGame, UNIT *hUnit);
void __fastcall PlrTradeEndInteract(GAME *ptGame, UNIT *hUnit);
void __fastcall PlrTradeAddItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
void __fastcall PlrTradeRemoveItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
int __fastcall PlrTradeCopyItems(UNIT *hUnit, BYTE *pbBuffer, int nSize);
int __fastcall PlrTradeCanAddItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
BOOL __fastcall PlrTradeIsFree(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall PlrTradeIsTrading(GAME *ptGame, UNIT *hUnit);
void __fastcall PlrTradeUpdateAll(GAME *ptGame);
void __fastcall PlrTradeUpdate(GAME *ptGame, UNIT *hUnit, void *pArg);

#endif
