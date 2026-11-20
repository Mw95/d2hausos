#ifndef D2RE_D2GAME_OBJECTS_H
#define D2RE_D2GAME_OBJECTS_H

#include "../D2game.h"

BOOL __fastcall ObjectThemeNone(GAME *ptGame, GAMEROOM *hRoom, GAMERECT *ptRect);
void __fastcall ObjectPopulateRoom(GAME *ptGame, GAMEROOM *hRoom);
void __fastcall ObjectFreeAll(GAME *ptGame);
BYTE __fastcall ObjectRegenerateMax(MISSOBJECTSTXT *ptTypeData);

UNIT *__fastcall ObjectPresetCreate(GAME *ptGame, GAMEROOM *hRoom, int nClass, int nX, int nY,
	DWORD dwArg);
void __fastcall ObjectInitUnit(GAME *ptGame, UNIT *hObject, DWORD dwUnitId, GAMEROOM *hRoom,
	int nX, int nY);
void __fastcall ObjectFreeUnit(GAME *ptGame, UNIT *hObject);
BYTE __fastcall ObjectRegenerateModes(void);
int __fastcall ObjectEvalGetLinkValue(UNIT *hObject);
void __fastcall ObjectEvalDamageEnemy(GAME *ptGame, UNIT *hObject, UNIT *hEnemy, int nType);
void __fastcall ObjectEvalUpdate(GAME *ptGame, UNIT *hObject);

#endif
