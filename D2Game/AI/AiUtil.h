#ifndef D2RE_D2GAME_AIUTIL_H
#define D2RE_D2GAME_AIUTIL_H

#include "../D2game.h"
#include "AiGeneral.h"

typedef UNIT * (__fastcall *PFNAIFIND)(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, union AIFINDRECORD *ptFind);

struct AIFINDKIND
{
 	int			nSearch;
 	PFNAIFIND	pfnFind;
};
typedef struct AIFINDKIND AIFINDKIND;

struct AIFINDPAIR
{
 	UNIT		*hUnit;
 	int			nDist;
 	UNIT		*hUnit2;
 	int			nDist2;
};
typedef struct AIFINDPAIR AIFINDPAIR;

struct AIFINDEXCL
{
 	UNIT		*hUnit;
 	int			nDist;
 	UNIT		*hExclude;
};
typedef struct AIFINDEXCL AIFINDEXCL;

struct AIFINDNEAR
{
 	UNIT		*hUnit;
 	int			nDist;
 	BOOL		bCheck;
 	int			nMax;
 	DWORD		nLogic;
 	UNIT		*hUnit2;
 	int			nDist2;
};
typedef struct AIFINDNEAR AIFINDNEAR;

struct AIFINDDEAD
{
 	UNIT		*hUnit;
 	int			nCount;
 	int			nDist;
 	int			nMax;
};
typedef struct AIFINDDEAD AIFINDDEAD;

struct AIFINDCLASS
{
 	DWORD		dwClassId;
 	int			nMax;
 	int			nCount;
};
typedef struct AIFINDCLASS AIFINDCLASS;

struct AIFINDNEAREST
{
 	UNIT		*hUnit;
 	int			nDist;
 	int			nCount;
 	int			nMax;
};
typedef struct AIFINDNEAREST AIFINDNEAREST;

struct AIFINDKIN
{
 	UNIT		*hFound;
 	UNIT		*hWeak;
};
typedef struct AIFINDKIN AIFINDKIN;

struct AIFINDHURT
{
 	UNIT		*hFound;
 	int			nFound;
};
typedef struct AIFINDHURT AIFINDHURT;

struct AIFINDWEAK
{
 	UNIT		*hFound;
 	int			nLife;
};
typedef struct AIFINDWEAK AIFINDWEAK;

struct AIFINDRAISE
{
 	UNIT		*hDead;
 	int			nDead;
 	UNIT		*hHurt;
 	int			nHurt;
 	int			nFound;
};
typedef struct AIFINDRAISE AIFINDRAISE;

union AIFINDRECORD
{
	AIFINDUNIT		tUnit;
	AIFINDNEAREST	tNearest;
	DWORD			dwMax;
	AIFINDNEAR		tNear;
	AIFINDPAIR		tPair;
	AIFINDEXCL		tExcl;
	AIFINDDEAD		tDead;
	int				nCount;
	AIFINDCLASS		tClass;
	AIFINDKIN		tKin;
	AIFINDHURT		tHurt;
	AIFINDWEAK		tWeak;
	AIFINDRAISE		tRaise;
};
typedef union AIFINDRECORD AIFINDRECORD;

int __fastcall AiThinkDistance(UNIT *hUnit, UNIT *hTarget);
int __fastcall AiThinkEdgeDistance(UNIT *hUnit, int nX, int nY);
int __fastcall AiThinkPointDistance(UNIT *hUnit, int nX, int nY);
BOOL __fastcall AiThinkSees(UNIT *hUnit, UNIT *hTarget);
UNIT * __fastcall AiFindAllyFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind);
UNIT * __fastcall AiFindDyingFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind);
UNIT * __fastcall AiFindNearFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind);
UNIT * __fastcall AiFindHostileFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind);
UNIT * __fastcall AiFindHostileButFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind);
UNIT * __fastcall AiFindObjectFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind);
UNIT * __fastcall AiFindDeadMinionFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind);
UNIT * __fastcall AiFindHostileSeenFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind);
UNIT * __fastcall AiCountClassFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, AIFINDRECORD *ptFind);
UNIT * __fastcall AiFind(GAME *ptGame, UNIT *hUnit, AIFINDRECORD *pArg, PFNAIFIND pfnFind, int nKind);
void __fastcall AiFindMinionFn(UNIT *hMinion, DWORD dwFind, DWORD dwResult);
UNIT * __fastcall AiFindTarget(GAME *ptGame, UNIT *hUnit, AIGENERAL *ptAi, int *pnDist, DWORD *pdwDir);
BOOL __fastcall AiPreferTarget(UNIT *hUnit, UNIT **phBest, int *pnDist, UNIT *hOther, int nOtherDist);
BOOL __fastcall AiFindCommandTarget(GAME *ptGame, UNIT *hUnit, BOOL bCheck, BOOL bSight, UNIT **phTarget,
	int *pnDist);
UNIT * __fastcall AiFindTargetInSight(GAME *ptGame, UNIT *hUnit, int *pnDist, DWORD *pdwDir);

#endif
