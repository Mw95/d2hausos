#ifndef D2RE_D2GAME_LEVEL_H
#define D2RE_D2GAME_LEVEL_H

#include "../D2game.h"

void __fastcall LevelSendNearUnits(GAME *ptGame, GAMEROOM *hRoomCenter, CLIENT *ptClient);
void __fastcall LevelSendNearRemovals(GAMEROOM *hRoomCenter, CLIENT *ptClient);
void __fastcall LevelFreeRoomNodes(GAME *ptGame);
void __fastcall LevelRoomEnterView(GAME *ptGame, GAMEROOM *hRoom, CLIENT *ptClient);
void __fastcall LevelRoomLeaveView(GAME *ptGame, GAMEROOM *hRoom, CLIENT *ptClient);
void __fastcall LevelUnitNotifyNear(GAME *ptGame, GAMEROOM *hRoomCenter, UNIT *hUnit);
void __fastcall LevelRemoveClientNear(GAMEROOM *hRoomCenter, CLIENT *ptClient);
void __fastcall LevelSendActInit(GAME *ptGame, CLIENT *ptClient);
void __fastcall LevelChangeAct(GAME *ptGame, CLIENT *ptClient, DWORD dwLevelId,
	DWORD dwArg);
void __fastcall LevelWarpUnit(GAME *ptGame, UNIT *hUnit, DWORD dwLevelId,
	DWORD dwArg);
void __fastcall LevelLoadAct(GAME *ptGame, BYTE bAct);
void __fastcall LevelFreeGameUnits(GAME *ptGame);
void __fastcall LevelUpdateChangedUnits(GAME *ptGame);
DWORD __fastcall LevelSendSysPlayer(DWORD dwClientId, WORD wGameId,
	WORD wNumPlayers, const char *szName);

#endif
