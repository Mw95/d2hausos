#ifndef D2RE_D2GAME_DEBUG_H
#define D2RE_D2GAME_DEBUG_H

#include "../D2game.h"

UNIT *__stdcall DebugFindUnit(DWORD eType, DWORD dwId);
struct ACT *__stdcall DebugGetAct(int nAct);
int __cdecl DebugCountMonsters(void);
GAMEROOM *__stdcall DebugFindRoom(BYTE bAct, int nX, int nY);
void __fastcall DebugUnitValue(GAME *ptGame, UNIT *hAttacker, UNIT *hTarget, int nValue, DWORD dwArg);

#endif
