#ifndef D2RE_D2GAME_OBJRGN_H
#define D2RE_D2GAME_OBJRGN_H

#include "../D2game.h"

DWORD __fastcall ObjectRgnCreate(GAME *ptGame);
void __fastcall ObjectRgnFree(GAME *ptGame);
OBJRGNLEVEL * __fastcall ObjectRgnGetLevel(GAME *ptGame, int nLevelId);
D2SEED * __fastcall ObjectRgnGetSeed(GAME *ptGame, GAMEROOM *hRoom);
SHRINEPOP * __fastcall ObjectRgnGetShrinePop(GAME *ptGame);
void __fastcall ObjectRgnAddNode(GAME *ptGame, UNIT *hPlayer, GAMEROOM *hRoom);
void __fastcall ObjectRgnInitObject(OBJINIT *ptInit);
BYTE __fastcall ObjectRgnSpotsFull(GAME *ptGame, int nLevelId);
BYTE __fastcall ObjectRgnLevelPopulated(GAME *ptGame, int nLevelId);
BYTE __fastcall ObjectRgnTestOrientedSpotsFull(GAME *ptGame, int nLevelId);
void __fastcall ObjectRgnCount(GAME *ptGame, int nLevelId);
BYTE __fastcall ObjectRgnSpotIsFar(GAME *ptGame, int nLevelId, int nX, int nY);
void __fastcall ObjectRgnAddSpot(GAME *ptGame, int nLevelId, int nX, int nY);
BYTE __fastcall ObjectRgnIsOrientedSpotFar(GAME *ptGame, int nLevelId, int nX, int nY);
void __fastcall ObjectRgnAddOrientedSpot(GAME *ptGame, int nLevelId, int nX, int nY);
int __fastcall ObjectRgnGetMonClass(OBJOPERATE *pObjectParam);
void __fastcall PartyScreenOnLoot(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg);
void __fastcall PartyScreenOnHear(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg);
void __fastcall PartyScreenOnSquelch(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg);
void __fastcall PartyScreenOnAccept(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg);

#endif
