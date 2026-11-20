#ifndef D2RE_D2GAME_SUNIT_H
#define D2RE_D2GAME_SUNIT_H

#include "../D2game.h"

typedef void (__fastcall *PFNSUNITCALLBACK)(GAME *ptGame, UNIT *hUnit, void *pArg);

void __fastcall SUnitFree(GAME *ptGame, UNIT *ptUnit);
UNIT *__fastcall SUnitGetNextInList(UNIT *ptUnit);
void __fastcall SUnitUpdateClients(GAME *ptGame, UNIT *ptUnit, GAMEROOM *hPrevRoom);
void __fastcall SUnitFreeTiles(GAME *ptGame);
void __fastcall SUnitWarp(GAME *ptGame, UNIT *hTarget, UNIT *hTile);
void __fastcall SUnitInitSeed(UNIT *ptUnit, D2SEED *ptSeed);
UNIT *__fastcall SUnitCreate(DWORD eType, DWORD dwClassId, int nX, int nY, GAME *ptGame, GAMEROOM *hRoom,
	DWORD dwFlags, DWORD dwMode, DWORD dwUnitId);
UNIT *__fastcall SUnitFind(GAME *ptGame, DWORD eType, DWORD dwId);
UNIT *__fastcall SUnitGetOwner(GAME *ptGame, UNIT *ptUnit);
void __fastcall SUnitPlace(UNIT *ptUnit, int nX, int nY, GAME *ptGame, GAMEROOM *hRoom, BOOL bNoFlag);
void __fastcall SUnitReplace(GAME *ptGame, UNIT *hUnit, GAMEROOM *hRoom, int nX, int nY);
void __fastcall SUnitSetClient(UNIT *ptUnit, CLIENT *ptClient);
CLIENT *__fastcall SUnitGetClient(UNIT *ptUnit);
void __fastcall SUnitReset(GAME *ptGame, UNIT *ptUnit);
void __fastcall SUnitSetEvent(UNIT *ptUnit, WORD wEvent, UNIT *hOther);
void __fastcall SUnitResetAnim(UNIT *ptUnit);
UNIT *__fastcall SUnitSpawnPreset(GAME *ptGame, GAMEROOM *hRoom, int eType, int nClass, int nX, int nY,
	DWORD dwMode, DWORD dwUnused);
void __fastcall SUnitSpawnRoomPresets(GAME *ptGame, GAMEROOM *hRoom);
void __fastcall SUnitUpdateTarget(GAME *ptGame, UNIT *hUnit);
UNIT *__stdcall SUnitGetTarget(GAME *ptGame, UNIT *hUnit);
int __fastcall SUnitStepAnim(GAME *ptGame, UNIT *hUnit);
void __fastcall SUnitSetMode(GAME *ptGame, UNIT *hUnit, DWORD dwMode);
void __fastcall SUnitLinkPortals(UNIT *ptFirst, UNIT *ptSecond);
UNIT *__fastcall SUnitGetPortalTarget(GAME *ptGame, UNIT *ptPortal);
UNIT *__fastcall SUnitIterate(GAME *ptGame, DWORD eType, void *pArg, PFNSUNITITERATE pfnIterate);
void __fastcall SUnitForEachPlayer(GAME *ptGame, PFNSUNITCALLBACK pfnIterate, void *pArg);
void __fastcall UnitSetEventList(UNIT *ptUnit, struct EVENT **phEvents);
struct EVENT **__fastcall UnitGetEventList(UNIT *ptUnit);
void __fastcall SUnitClearModeEvents(GAME *ptGame, UNIT *hUnit);
void __fastcall SUnitScheduleModeEvents(GAME *ptGame, UNIT *hUnit);
void __fastcall SUnitSetStandingEvent(GAME *ptGame, UNIT *hUnit);
void __fastcall SUnitGetInfo(UNIT *ptUnit, GAMEUNITINFO *ptInfo);
UNIT *__fastcall SUnitGetInteractUnit(GAME *ptGame, UNIT *hUnit);
void __fastcall SUnitFreeInventory(GAME *ptGame, UNIT *hUnit);
GAME *__fastcall SUnitGetGame(UNIT *hUnit);
void __fastcall SUnitSetPetId(UNIT *hUnit, DWORD dwValue);
DWORD __fastcall SUnitGetPetId(UNIT *hUnit);
void __fastcall SUnitSetObjectFlags(UNIT *hUnit, BYTE bValue);
BYTE __fastcall SUnitGetObjectFlags(UNIT *hUnit);
BOOL __stdcall SUnitGetInteract(UNIT *hUnit, DWORD *pdwType, DWORD *pdwUnitId);
void __stdcall SUnitSetInteract(UNIT *hUnit, DWORD dwType, DWORD dwUnitId);
UNIT *__stdcall SUnitResetInteract(UNIT *hUnit);
BOOL __fastcall SUnitIsDead(UNIT *hUnit);
BOOL __fastcall SUnitIsHostile(GAME *ptGame, UNIT *hUnit, UNIT *hOther);
BOOL __fastcall SUnitIsTargetPet(GAME *ptGame, UNIT *hUnit, UNIT *hOther, int nSkill);
BOOL __fastcall SUnitIsTargetAlly(GAME *ptGame, UNIT *hUnit, UNIT *hOther, int nSkill);
BOOL __fastcall SUnitIsAlly(GAME *ptGame, UNIT *hUnit, UNIT *hOther);
void __fastcall SUnitSetAlignment(UNIT *hUnit, BYTE eAlignNew);
void __fastcall SUnitSetStateEndLinked(UNIT *hUnit, BOOL bSet);
BOOL __fastcall SUnitMoveToRoom(GAME *ptGame, UNIT *hUnit, GAMEROOM *hRoom, int nX, int nY, BOOL bExact,
	BOOL bTeleported);
WORD __fastcall SUnitGetParty(UNIT *hUnit);

#endif
