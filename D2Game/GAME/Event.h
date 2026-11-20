#ifndef D2RE_D2GAME_EVENT_H
#define D2RE_D2GAME_EVENT_H

#include "../D2game.h"

struct EVENT;

void __fastcall ClientQueueAppend(CLIENT *ptClient, CLIENTNODE178 *ptNode);
void __fastcall ClientQueuePushFront(CLIENT *ptClient, CLIENTNODE178 *ptNode);
CLIENTNODE178 * __fastcall ClientQueuePop(CLIENT *ptClient);
CLIENTNODE178 * __fastcall ClientGetQueueTail(CLIENT *ptClient);
CLIENTNODE178 * __fastcall ClientGetQueueHead(CLIENT *ptClient);
void __fastcall ClientGetAccount(CLIENT *ptClient, char *szOut);
void __fastcall ClientGetTokenResult(CLIENT *ptClient, DWORD *pdwOut);
void __fastcall EventRemoveByType(GAME *ptGame, UNIT *hUnit, int eEventType);
#ifndef D2_EVENT_CPP
void __fastcall EventRemove(GAME *ptGame, struct EVENT *ptEvent);
#endif
struct EVENT * __fastcall EventAdd(GAME *ptGame, UNIT *hUnit, int eEventType, int nEventFrame);
#ifndef D2_EVENT_CPP
struct EVENT * __fastcall EventNew(GAME *ptGame, UNIT *hUnit, int eEventType, int nEventFrame);
#endif
struct EVENT * __fastcall EventAddPersistent(GAME *ptGame, UNIT *hUnit, int eEventType);
void __fastcall EventUnitInit(GAME *ptGame, UNIT *hUnit);
void __fastcall EventUnitFree(GAME *ptGame, UNIT *hUnit);
void __fastcall EventListCreate(GAME *ptGame);
void __fastcall EventListFree(GAME *ptGame);
void __fastcall EventListRun(GAME *ptGame);
int __fastcall EventGetFirstFrame(GAME *ptGame, UNIT *hUnit, int eEventType);
void __cdecl GameLog(int nLevel, const char *pszFormat, ...);
void __fastcall ITEMSUnitFree(GAME *ptGame, UNIT *hUnit);
void __fastcall GameFreeMissileUnits(GAME *ptGame);
struct EVENT * __fastcall MissilesUnitInit(GAME *ptGame, UNIT *hUnit, DWORD dwUnitId);
void __fastcall MissilesUnitFree(GAME *ptGame, UNIT *hUnit);
void __fastcall GameFreeMonsterUnits(GAME *ptGame);
void __fastcall MonsterUnitFree(GAME *ptGame, UNIT *hUnit);
void __fastcall MonsterClearTarget(GAME *ptGame, UNIT *hUnit, UNIT *hTarget);
void __fastcall MonsterRestartAi(GAME *ptGame, UNIT *hUnit);

#endif
