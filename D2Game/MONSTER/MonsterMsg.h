#ifndef D2RE_D2GAME_MONSTERMSG_H
#define D2RE_D2GAME_MONSTERMSG_H

#include "../D2game.h"

struct DRLGLOGICNODE;
struct FORMATIONPOINT;
struct MONSPAWNCTX;

void __fastcall MonsterSendMode(GAME *ptGame, UNIT *hMonster, CLIENT *hClient);
BOOL __fastcall MonsterFindSpawnPoint(GAME *ptGame, GAMEROOM *hRoom,
	struct DRLGLOGICNODE *ptArea, int nClass, int *pnX,
	int *pnY, BOOL bAwayFromWarps);
UNIT * __fastcall MonsterSpawnClass(GAME *ptGame, GAMEROOM *hRoom, int nClass,
	int nX, int nY, DWORD dwArg);
void __fastcall MonsterPopulateRoom(GAME *ptGame, GAMEROOM *hRoom);
BOOL __fastcall MonsterFindFormationPoint(GAME *ptGame, GAMEROOM *hRoom,
	struct DRLGLOGICNODE *ptArea, int nClass,
	struct FORMATIONPOINT *ptFormation, int *pnX,
	int *pnY);
void __fastcall MonsterSpawnCritter(GAME *ptGame, GAMEROOM *hRoom);
void __fastcall MonsterRegionsInit(GAME *ptGame);
void __fastcall MonsterRegionAdd(DWORD *padwRegions, GAMEROOM *hRoom, UNIT *hUnit,
	BOOL bFlag);
void __fastcall MonsterRegionModeChange(DWORD *padwRegions, UNIT *hUnit,
	int nModeOld, int nModeNew);
void __fastcall MonsterRegionAddAligned(DWORD *padwRegions, UNIT *hUnit);
void __fastcall MonsterRegionRemoveAligned(DWORD *padwRegions, UNIT *hUnit);
void __fastcall MonsterRegionRemove(DWORD *padwRegions, int nLevel,
	int nLevelOld, int nMode, BOOL fDead,
	BOOL fNoCount, UNIT *hUnit);
BOOL __fastcall MonsterHasRoom(int nClass, GAMEROOM *hRoom, int nX,
	int nY, BOOL bArg);
BOOL __fastcall MonsterCanUseSkill(GAME *ptGame, UNIT *hUnit, int nKind,
	UNIT *hTarget, int nX, int nY);
int __fastcall MonsterGetSpawnMode(UNIT *hUnit, int nSkill, DWORD dwUnused);
UNIT * __fastcall MonsterSpawnNear(GAME *ptGame, UNIT *hUnit, int nClass,
	DWORD dwMode, int nRadius, WORD wFlags);
UNIT * __fastcall MonsterSpawnAround(struct MONSPAWNCTX *ptCtx);

#endif
