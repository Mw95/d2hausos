#ifndef D2RE_D2GAME_ARENA_H
#define D2RE_D2GAME_ARENA_H

#include "../D2game.h"

void __fastcall ArenaSendTagsToClient(GAME *ptGame, CLIENT *ptClient);
DWORD __fastcall ArenaGetFlags(GAME *ptGame);
DWORD __fastcall ArenaTestCharTemplate(GAME *ptGame);
DWORD __fastcall ArenaGetType(GAME *ptGame);

void __fastcall ArenaClearTagSendFlag(GAME *ptGame);
DWORD __fastcall ArenaTestNoActChange(GAME *ptGame);
DWORD __fastcall ArenaHasGuild(GAME *ptGame);
void __fastcall ArenaNew(GAME *ptGame, WORD wUnused, DWORD dwFlags,
	BYTE bType);
void __fastcall ArenaFree(GAME *ptGame);
void __fastcall ArenaSendTagsOnJoin(GAME *ptGame, CLIENT *ptClient);
DWORD __fastcall ArenaTestRequiredFlag(GAME *ptGame);
BOOL __fastcall ArenaStringIsTerminated(const char *psz,
	int nMax);
void __fastcall ArenaSrvMsgDispatch(SRVMSG *ptMsg, DWORD dwLength);
void __fastcall ArenaUnitFree(GAME *ptGame, UNIT *hUnit);
void __fastcall ArenaUnitClear(GAME *ptGame, UNIT *hUnit);

#endif
