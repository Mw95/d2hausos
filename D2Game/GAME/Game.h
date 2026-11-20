#ifndef D2RE_D2GAME_GAME_H
#define D2RE_D2GAME_GAME_H

#include "../D2game.h"

DWORD __fastcall GameFindById(WORD wGameId);
BOOL __fastcall GameCheckCreatePacket(DWORD dwClientId, SRVPACKET *ptPacket);
void __fastcall GameReceivePing(DWORD dwClientId, DWORD dwTick, DWORD dwDelay);
void __fastcall GameCreateFromPacket(DWORD dwClientId, char *szGameName, BYTE bGameType, BYTE bClass,
	char *szPlrName, WORD wArena, DWORD dwFlags, BYTE bArenaType, BYTE bUnused1, BYTE bUnused2,
	BYTE bDifficulty);
void __fastcall GameSendRosterRemoveToAll(GAME *ptGame, CLIENT *ptClient);
BOOL __fastcall GameValidateJoinAct(DWORD dwClientId);
void __stdcall GameDropClient(DWORD dwClientId);
void __fastcall GameRemoveClient(GAME *ptGame, DWORD dwClientId);
void __fastcall GameOnJoinAct(DWORD dwClientId);
BOOL __fastcall GameJoin(DWORD dwClientId, WORD wGameId, BYTE bClass, BYTE *pbData, DWORD dwToken,
	char *szName, DWORD *pdwResult);
void __fastcall GameJoinDone(DWORD dwClientId, WORD wGameId, BYTE bClass, BYTE *pbData, char *szName,
	DWORD dwResult);
BOOL __fastcall GameValidateEndGame(DWORD dwClientId);
void __fastcall GameOnDisconnect(DWORD dwClientId, DWORD bReason);
void __fastcall GameOnEndGame(DWORD dwClientId, BOOL bNotify);
BOOL __fastcall GameValidateDisconnect(DWORD dwClientId);
GAME * __fastcall GameLockByClient(DWORD dwClientId);
void __fastcall GameUnlock(GAME *ptGame);
GAME * __fastcall GameLockByHandle(DWORD dwGame);
BOOL __fastcall GameReturnTrue(DWORD dwClientId);
void __fastcall GameSendGameList(DWORD dwClientId);
BOOL __fastcall GameReceiveDataChunk(DWORD dwClientId, SRVPACKET *ptPacket);

void __fastcall GameUpdate(GAME *ptGame);
void __fastcall GameSendAllClientUpdates(GAME *ptGame, DWORD dwArg);
void __fastcall GameClose(DWORD dwGame);
void __fastcall GameForEachClient(GAME *ptsGame, void (__fastcall *pfn)(CLIENT *, void *), void *pArg);
void __fastcall GameUnitGone(UNIT *hUnit, GAME *ptsGame);
void __fastcall GameRespawnRoom(GAME *ptGame, GAMEROOM *hRoom);
void __fastcall GameFlushClients(GAME *ptGame);
void __fastcall GameDropHost(GAME *ptsGame, CLIENT *ptClient);
#define GAME_MAXGAMES		1024
typedef DWORD GAMEHANDLES[GAME_MAXGAMES];
struct GAMEHANDLEBLOCK
{
 	GAMEHANDLES	dwHandles;
 	int			nFrameMs;
 	GAME		*ptFirstGame;
};
typedef struct GAMEHANDLEBLOCK GAMEHANDLEBLOCK;
extern EVENTCALLBACKTABLE *ptEventCallbackTable;
GAME * __fastcall GameLockFirst(void);
void __fastcall GameUnlockFirst(void);

#endif
