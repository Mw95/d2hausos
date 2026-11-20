#ifndef D2RE_D2GAME_PLRMSG_H
#define D2RE_D2GAME_PLRMSG_H

#include "../D2game.h"

void __fastcall PlrModeSendLife(UNIT *hUnit, CLIENT *ptClient);
void __fastcall PlrModeSendPosition(UNIT *hUnit, CLIENT *ptClient, BOOL fArg);
void __fastcall PlrMsgSendMode(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient);
void __fastcall PlrMsgStatChanged(UNIT *hUnit, int nStat, int nValue, UNIT *hPlayer);
void __fastcall PlrMsgSendLifeMana(UNIT *hUnit, UNIT *hPlayer);
void __fastcall PlrMsgSendStats(UNIT *hUnit, CLIENT *ptClient);
int __fastcall PlrMsgInteract(UNIT *hPlayer, DWORD dwUnitType, DWORD dwUnitId, BOOL bFlag, GAME *ptGame);
void __fastcall PlrMsgSendChatAll(GAME *ptGame, const char *szText, BYTE bColor);
void __fastcall PlrMsgSendTextAll(GAME *ptGame, char *szText, BYTE bColor);
void __fastcall PlrMsgSendSystemMsgAll(GAME *ptGame, GAMEPLAYERMSG *ptMsg);
void __fastcall PlrMsgWarpToLevel(GAME *ptGame, UNIT *hUnit, DWORD dwLevelId,
	DWORD dwArg);

void __fastcall PlrMsgStateEnd(UNIT *hUnit, DWORD dwState);
int __fastcall PlrMsgHandleCommand(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize);
void __fastcall PlrMsgSendKilled(GAME *ptGame, UNIT *hKiller, UNIT *hDead);
void __fastcall PlrMsgSetSaveEnabled(DWORD dwEnabled);
int __fastcall PlrMsgLoadItem(GAME *ptGame, UNIT *hUnit, struct ITEMSAVE *ptItemSave, UNIT **phParent,
	int *pnAttached);

#endif
