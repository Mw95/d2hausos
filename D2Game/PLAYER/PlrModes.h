#ifndef D2RE_D2GAME_PLRMODES_H
#define D2RE_D2GAME_PLRMODES_H

#include "../D2game.h"

typedef void (__fastcall *PFNPLRMODESTARTXY)(GAME *ptGame, UNIT *hPlayer, int nMode, int nX, int nY);
typedef void (__fastcall *PFNPLRMODESTARTID)(GAME *ptGame, UNIT *hPlayer, int nMode, UNIT *hTarget);
typedef int (__fastcall *PFNPLRMODEDO)(GAME *ptGame, UNIT *hPlayer);
typedef void (__fastcall *PFNPLRMODEEVENT)(GAME *ptGame, UNIT *hPlayer);

void __fastcall PlrModeStartNeutral(GAME *ptGame, UNIT *hPlayer, int nMode, int nX,
	int nY);
int __fastcall PlrModeUpdateNone(GAME *ptGame, UNIT *hPlayer);
void __fastcall PlrModeStartMoveToUnit(GAME *ptGame, UNIT *hPlayer, int nMode, UNIT *hTarget);
void __fastcall PlrModeStartMove(GAME *ptGame, UNIT *hPlayer, int nMode);
void __fastcall PlrModeStartMoveTo(GAME *ptGame, UNIT *hPlayer, int nMode, int nX, int nY);
int __fastcall PlrModeUpdateMove(GAME *ptGame, UNIT *hPlayer);
UNIT * __fastcall PlrModeCreateCorpse(GAME *ptGame, UNIT *hPlayer, int nX, int nY, GAMEROOM *hRoom);
#ifndef D2_PLRMODES_CPP
void __fastcall PlrModeRemoveBodyItem(GAME *ptGame, int nLoc, UNIT *hItem, UNIT *hPlayer);
#endif
void __fastcall PlrModePickupCorpse(GAME *ptGame, UNIT *hPlayer, UNIT *hDeadBody);
void __fastcall PlrModeStartDeath(GAME *ptGame, UNIT *hPlayer, int nMode, UNIT *hTarget);
void __fastcall PlrModeStartDead(GAME *ptGame, UNIT *hPlayer, int nMode, int nX, int nY);
void __fastcall PlrModeStartBlock(GAME *ptGame, UNIT *hPlayer, int nMode, int nX, int nY);
int __fastcall PlrModeUpdateAnim(GAME *ptGame, UNIT *hPlayer);
void __fastcall PlrModeStartGetHit(GAME *ptGame, UNIT *hPlayer, int nMode, int nX, int nY);
void __fastcall PlrModeStartSkillXY(GAME *ptGame, UNIT *hPlayer, int nMode, int nX, int nY);
void __fastcall PlrModeStartSkillID(GAME *ptGame, UNIT *hPlayer, int nMode, UNIT *hTarget);
void __fastcall PlrModeWearWeapons(GAME *ptGame, UNIT *hUnit);
void __fastcall PlrModeWearItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
void __fastcall PlrModeBreakItems(GAME *ptGame, UNIT *hUnit);
int __fastcall PlrModeUpdateSkill(GAME *ptGame, UNIT *hPlayer);
void __fastcall PlrModeEventRegenerate(GAME *ptGame, UNIT *hPlayer);
void __fastcall PlrModeEventEndMode(GAME *ptGame, UNIT *hPlayer);
void __fastcall PlrModeEventUpdateMode(GAME *ptGame, UNIT *hPlayer);
void __fastcall PlrModeSendUnit(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient, BOOL fNoItems);
void __fastcall PlrModeStartXY(GAME *ptGame, UNIT *hPlayer, void *ptSkill, int nMode, int nX, int nY,
	BOOL fForce);
BOOL __fastcall PlrModeCanStart(UNIT *hPlayer, int nMode);
void __fastcall PlrModeStartID(GAME *ptGame, UNIT *hPlayer, void *ptSkill, int nMode, DWORD dwType,
	DWORD dwId, BOOL fForce);
void __fastcall PlrModeEventChat(GAME *ptGame, UNIT *hPlayer);
void __fastcall PlrModeEventPeriodic(GAME *ptGame, UNIT *hPlayer);
void __fastcall PlrModeEvent(GAME *ptGame, UNIT *hPlayer, int eEventType);

#endif
