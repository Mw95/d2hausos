#ifndef D2RE_D2GAME_AIGENERAL_H
#define D2RE_D2GAME_AIGENERAL_H

#include "../D2game.h"

struct AIFINDUNIT
{
 	UNIT		*hUnit;
 	int			nDist;
};
typedef struct AIFINDUNIT AIFINDUNIT;

void __fastcall DebugRoll(GAME *ptGame, UNIT *hAttacker, UNIT *hTarget, int nChance, int nRoll,
	const char *pszName);
void __fastcall DebugResist(GAME *ptGame, UNIT *hAttacker, UNIT *hTarget, int nOld, int nResist, int nReduce,
	int nNew, const char *pszName, DWORD dwArg);
AIGENERAL * __fastcall AiGeneralNew(GAME *ptGame);
void __fastcall AiSetParam(UNIT *hUnit, int nIndex, DWORD dwValue);
void __fastcall AiGeneralFree(GAME *ptGame, AIGENERAL *ptAi);
AICOMMAND * __fastcall AiAddCommand(GAME *ptGame, UNIT *hUnit);
void __fastcall AiPopCommand(GAME *ptGame, UNIT *hUnit);
void __fastcall AiClearCommands(GAME *ptGame, UNIT *hUnit);
AICOMMAND * __fastcall AiGetCommand(UNIT *hUnit);
AICOMMAND * __fastcall AiFindCommand(UNIT *hUnit, int nCommand, BOOL bMakeCurrent);
void __fastcall AiCommandMinions(GAME *ptGame, UNIT *hUnit, AICOMMAND *ptCommand);
void __fastcall AiPushCommand(GAME *ptGame, UNIT *hUnit, AICOMMAND *ptCommand);
AICOMMAND * __fastcall AiFindOrAddCommand(GAME *ptGame, UNIT *hUnit, int nCommand, BOOL bMakeCurrent);
void ** __stdcall AiGetCell(UNIT *hUnit);
void __fastcall AiSetOwner(GAME *ptGame, UNIT *hUnit, DWORD dwOwnerId, DWORD dwOwnerType, BOOL bFlag01,
	BOOL bFlag02);
void __fastcall AiGetOwnerId(UNIT *hUnit, DWORD *pdwOwnerId, DWORD *pdwOwnerType);
UNIT * __fastcall AiGetOwner(UNIT *hUnit);
void __fastcall AiAddMinion(GAME *ptGame, UNIT *hUnit, UNIT *hMinion);
void __fastcall AiRemoveFromOwner(UNIT *hUnit);
void __fastcall AiFreeMinions(UNIT *hUnit);
void __fastcall AiFreeMinionList(GAME *ptGame, AIMINION *ptMinion);
void __fastcall AiForEachMinion(UNIT *hUnit, DWORD dwArg1, DWORD dwArg2, PFNAIMINIONFN pfnParty);
void __fastcall AiTakeOwner(UNIT *hUnit, DWORD *pdwOwnerId, DWORD *pdwOwnerType, BOOL *pbFlag01,
	BOOL *pbFlag02, AIMINION **pptMinions);
void __fastcall AiRestoreOwner(GAME *ptGame, UNIT *hUnit, DWORD dwOwnerId, DWORD dwOwnerType, BOOL bFlag01,
	BOOL bFlag02, AIMINION *ptMinions);
void __fastcall AiPassLeadership(UNIT *hUnit);
UNIT * __fastcall AiIdleByParam(GAME *ptGame, UNIT *hUnit, AIGENERAL *ptAi, int *pnParam, DWORD *pdwDir);
UNIT * __fastcall AiIdle(GAME *ptGame, UNIT *hUnit, AIGENERAL *ptAi, int *pnParam, DWORD *pdwDir);
UNIT * __fastcall AiTryWithin(GAME *ptGame, UNIT *hUnit, AIGENERAL *ptAi, int *pnParam, DWORD *pdwDir,
	int nMax);
UNIT * __fastcall AiFindNearPlayer(GAME *ptGame, UNIT *hUnit, BOOL *pbClose);
union AIFINDRECORD;
UNIT * __fastcall AiFindNearPlayerFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate,
	union AIFINDRECORD *ptFind);

#endif
