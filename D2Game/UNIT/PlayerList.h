#ifndef D2RE_D2GAME_PLAYERLIST_H
#define D2RE_D2GAME_PLAYERLIST_H

#include "../D2game.h"

void __fastcall PlayerListFree(GAME *ptGame, UNIT *hPlayer);
void __fastcall PlayerListRemoveUnit(GAME *ptGame, UNIT *hPlayer, void *pArg);
void __fastcall PlayerListAdd(GAME *ptGame, UNIT *hPlayer, UNIT *hUnit);
void __fastcall PlayerListSetFlag(GAME *ptGame, UNIT *hPlayer, UNIT *hUnit, DWORD dwFlag, BOOL bSet);
DWORD __fastcall PlayerListTestFlag(UNIT *hPlayer, UNIT *hUnit, DWORD dwMask);
WORD __fastcall PlayerListGetFlags(UNIT *hPlayer, UNIT *hUnit);
void __fastcall PlayerListSetState(UNIT *hPlayer, UNIT *hUnit, DWORD dwState);
WORD __fastcall PlayerListGetState(UNIT *hPlayer, UNIT *hUnit);
void __fastcall PlayerListSendState(UNIT *hPlayer, UNIT *hUnit);
void __fastcall PlayerListSendFlags(GAME *ptGame, UNIT *hPlayer, UNIT *hUnit);
void __fastcall PlayerListAddPlayer(GAME *ptGame, UNIT *hPlayer);
void __fastcall PlayerListJoin(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall PlayerListRefresh(GAME *ptGame, UNIT *hPlayer);

#endif
