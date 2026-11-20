#ifndef D2RE_D2GAME_CLIENTS_H
#define D2RE_D2GAME_CLIENTS_H

#include "../D2game.h"

CLIENT * __fastcall ClientsFindInGame(GAME *ptGame, DWORD dwClientId);
UNIT * __stdcall ClientGetPlayerUnit(CLIENT *ptClient, BOOL bNoCheck);
DWORD __fastcall ClientTestSeesOtherTags(CLIENT *ptClient);

void __fastcall ClientSetRoom(CLIENT *ptClient, GAMEROOM *hRoom);
BYTE __fastcall ClientGetAct(CLIENT *ptClient);
void __fastcall ClientSetAct(CLIENT *ptClient, BYTE bAct);
void __fastcall ClientSetState(CLIENT *ptClient, DWORD dwState);
GAME * __fastcall ClientGetGame(CLIENT *ptClient);
WORD __fastcall ClientGetGuildField(CLIENT *ptClient);
void __fastcall ClientGetGuildTag(CLIENT *ptClient, DWORD *pdwValue);
void __fastcall ClientGetGuildName(CLIENT *ptClient, char *pszOut);
void __fastcall ClientsInitLists(void);
void __fastcall ClientsTermLists(void);
void __fastcall ClientPlacePlayer(CLIENT *ptClient, UNIT *hUnit, GAME *ptGame, GAMEROOM *hRoom,
	int nX, int nY);
int __fastcall ClientLoadCharacter(CLIENT *ptClient, GAME *ptGame,
	DWORD dwArg);
CLIENT * __fastcall ClientsNew(GAME *ptGame, DWORD dwClientId, BYTE bClass,
	const char *szName, const char *szAccount,
	DWORD dwTokenResult);
void __fastcall ClientsInitGame(GAME *ptGame);
void __fastcall ClientsRemove(GAME *ptGame, DWORD dwClientId);
void __fastcall ClientsFreeAll(GAME *ptGame);
void __fastcall ClientUpdateNearPlayers(CLIENT *ptClient, DWORD dwMaxNear);
void __fastcall ClientUpdateRoom(GAME *ptGame, CLIENT *ptClient);
BOOL __fastcall ClientsIsInGame(GAME *ptGame, DWORD dwClientId);
UNIT * __fastcall ClientFindLifeManaUnit(GAME *ptGame, CLIENT *ptClient);
DWORD __fastcall ClientGetId(CLIENT *ptClient);
DWORD __fastcall ClientGetFlags(CLIENT *ptClient);
void __fastcall ClientSendLifeMana(UNIT *hUnit, CLIENT *ptClient);
char * __fastcall ClientGetName(CLIENT *ptClient);
BOOL __fastcall ClientsIsInState(DWORD dwClientId,
	DWORD dwState);
void __fastcall ClientsUpdatePing(DWORD dwClientId, DWORD dwTick,
	DWORD dwDelay);
BOOL __fastcall ClientsIdIsFree(DWORD dwClientId);
BOOL __fastcall ClientsNameIsFree(const char *szName);
BOOL __fastcall ClientsAppendData(DWORD dwClientId, const BYTE *pbData,
	int nLength, int nTotal);
void __fastcall ClientFreeBuffer(CLIENT *ptClient);
DWORD __fastcall ClientTestDataSending(CLIENT *ptClient);
void __fastcall ClientSetHost(CLIENT *ptClient);
DWORD __fastcall ClientIsHost(CLIENT *ptClient);
CLIENT * __fastcall ClientsGetFirst(GAME *ptGame);
DWORD __fastcall ClientTestFlags(CLIENT *ptClient, WORD wMask);

BOOL __fastcall ClientSeesUnit(UNIT *hUnit, CLIENT *ptClient);

void __fastcall ClientSetPair(CLIENT *ptClient, int nIndex, DWORD dwSkillId,
	DWORD dwHand);

void __fastcall ClientSetStep(CLIENT *ptClient, int nLow, int nHigh);

#endif
