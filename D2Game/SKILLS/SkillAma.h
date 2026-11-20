#ifndef D2RE_D2GAME_SKILLAMA_H
#define D2RE_D2GAME_SKILLAMA_H

#include "../D2game.h"

typedef BOOL (__fastcall *PFNSKILLFN)(GAME *ptGame, UNIT *hUnit, int nLevel);

#define SKILLCHART_NOMISSILE	65535

struct SKILLCHART
{
 	PFNSKILLFN	pfnStart;
 	PFNSKILLFN	pfnUse;
 	int			nMissile;
};
typedef struct SKILLCHART SKILLCHART;

BOOL __fastcall SkillsInnerSight(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsJabStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsJabHit(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMultipleShotStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsMultipleShot(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsPowerStrikeStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsSlowMissiles(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsImpaleStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsGuidedArrowStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsGuidedArrow(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsChargedStrikeStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsChargedStrike(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsStrafeStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsStrafe(GAME *ptGame, UNIT *hUnit, int nLevel);
void __fastcall SkillsStrafeModify(UNIT *hMissile, DWORD dwPercent);
BOOL __fastcall SkillsFendStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsFend(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsLightningStrikeStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsLightningStrike(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsDopplezon(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsValkyrie(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsAmaMissileStart(GAME *ptGame, UNIT *hUnit, int nLevel);
BOOL __fastcall SkillsAmaStart(GAME *ptGame, UNIT *hUnit, int nSkillId, int nLevel);
BOOL __fastcall SkillsAmaUse(GAME *ptGame, UNIT *hUnit, int nSkillId, int nLevel);

#endif
