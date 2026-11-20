#ifndef D2RE_D2GAME_SUNITMSG_H
#define D2RE_D2GAME_SUNITMSG_H

#include "../D2game.h"

void __fastcall SUnitMsgSendUnit(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient);
void __fastcall SUnitMsgSendStates(UNIT *hUnit, CLIENT *ptClient, BOOL bAll);
void __fastcall SUnitMsgSendOverlay(UNIT *hUnit, CLIENT *ptClient);
void __fastcall SUnitMsgSendRemove(UNIT *hUnit, CLIENT *ptClient);
void __fastcall SUnitMsgSendChat(UNIT *hUnit, CLIENT *ptClient);

#endif
