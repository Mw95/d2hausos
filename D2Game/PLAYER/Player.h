#ifndef D2RE_D2GAME_PLAYER_H
#define D2RE_D2GAME_PLAYER_H

#include "../D2game.h"

void __fastcall PlayerInitUnit(GAME *ptGame, UNIT *hUnit, DWORD dwUnitId);
void __fastcall PlayerFreeUnit(GAME *ptGame, UNIT *hPlayer);
void __fastcall PlayerFreeAll(GAME *ptGame);
void __fastcall PlayerClearTarget(GAME *ptGame, UNIT *hUnit, UNIT *hTarget);
void __fastcall PlayerStartMode(GAME *ptGame, UNIT *hUnit, DWORD dwX,
	DWORD dwY);
void __fastcall PlayerAddStartItem(GAME *ptGame, UNIT *hPlayer,
	INITITEM *ptInitItem  );
void __fastcall PlayerAddStartItems(UNIT *hPlayer, GAME *ptGame, BYTE bType);
void __fastcall PlayerSendItems(UNIT *hPlayer, CLIENT *ptClient);
BOOL __fastcall PlayerIsBusy(UNIT *hUnit);
void __fastcall PlayerClearBusy(UNIT *hUnit);
void __fastcall PlayerInteractEnd(GAME *ptGame, UNIT *hUnit);
void __fastcall PlayerInteractStart(GAME *ptGame, UNIT *hUnit);
void __fastcall PlayerPlaceDeadBody(GAME *ptGame, GAMEROOM *hRoom, DWORD dwUnitId, COLPOINT *ptCoord);
void __fastcall PlayerSetPortalId(UNIT *hPlayer, DWORD dwPortalId);
DWORD __fastcall PlayerGetPortalId(UNIT *hPlayer);
void __fastcall PlayerRemovePortal(GAME *ptGame, UNIT *hPlayer);
BOOL __fastcall PlayerDropGold(GAME *ptGame, UNIT *hPlayer, DWORD dwUnitId, DWORD dwAmount);
int __fastcall PlayerInteract(GAME *ptGame, UNIT *hPlayer, DWORD dwArg);
BOOL __fastcall PlayerSetClientPair(GAME *ptGame, UNIT *hPlayer, int nIndex,
	DWORD dwSkillId, DWORD dwHand);
int __fastcall PlayerCountAlive(GAME *ptGame);
void __fastcall PlayerDeathPenalty(GAME *ptGame, UNIT *hPlayer);

#endif
