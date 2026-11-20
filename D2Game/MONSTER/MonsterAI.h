#ifndef D2RE_D2GAME_MONSTERAI_H
#define D2RE_D2GAME_MONSTERAI_H

#include "../D2game.h"

struct MONPETDESC;

struct MONKILLSRECORD
{
 	int			nKind;
 	DWORD		dwComponentTR;
 	DWORD		dwComponentHD;
 	DWORD		dwComponentLH;
 	DWORD		dwComponentSH;
 	DWORD		dwComponentRH;
 	int			nLife;
 	int			nDefense;
 	int			nLevel;
 	int			nDamageMin;
 	int			nDamageMax;
 	int			nHireSpecial;
 	int			nSkillLevel;
 	int			nCost;
 	WORD		wFlags;
 	BYTE		unobserved_3a[2];
};
typedef struct MONKILLSRECORD MONKILLSRECORD;

typedef void (__fastcall *PFNTHREATCALLBACK)(UNIT *hUnit);

MONSTERDATA * __fastcall MonsterAICreate(GAME *ptGame);
void __fastcall MonsterAIDestroy(GAME *ptGame, MONSTERDATA *ptMonsterData);
HPTRACK * __fastcall MonsterGetHpTrack(MONSTERDATA *ptMonsterData);
void __fastcall MonsterAIPetInit(GAME *ptGame, UNIT *hPlayer, UNIT *hPet,
	struct MONPETDESC *ptDesc);
void __fastcall MonsterAIPetLevelUp(GAME *ptGame, UNIT *hPlayer);
int __fastcall MonsterAICountThreats(THREATNODE **pptThreats);
BOOL __fastcall MonsterAIHasThreats(THREATNODE **pptThreats);
BOOL __fastcall MonsterAIIsThreat(THREATNODE **pptThreats, UNIT *hUnit);
void __fastcall MonsterAIRemoveThreat(GAME *ptGame, UNIT *hUnit, THREATNODE **pptThreats);
void __fastcall MonsterAITellThreat(GAME *ptGame, UNIT *hUnit, UNIT *hMonster,
	THREATNODE **pptThreats);
void __fastcall MonsterAIAckThreat(UNIT *hMonster, UNIT *hUnit);
BOOL __fastcall MonsterAIThreatAcked(UNIT *hMonster, UNIT *hUnit);
void __fastcall MonsterAIDropThreat(GAME *ptGame, UNIT *hUnit, UNIT *hMonster,
	THREATNODE **pptThreats);
void __fastcall MonsterAIAddThreat(GAME *ptGame, UNIT *hUnit, UNIT *hMonster,
	THREATNODE **pptThreats, DWORD dwArg);
BOOL __fastcall MonsterAISetTargetCmd(UNIT *hMonster, int eTargetCmd, DWORD dwArg);
int __fastcall MonsterAIGetTargetCmd(UNIT *hMonster);
DWORD __fastcall MonsterAIGetTargetCmdArg(UNIT *hMonster);
void __fastcall MonsterAIClearTargetCmd(UNIT *hMonster);
BOOL __fastcall MonsterAIThreatTold(UNIT *hMonster, UNIT *hUnit);
void __fastcall MonsterAIClearThreats(GAME *ptGame, UNIT *hMonster,
	PFNTHREATCALLBACK pfnCallback);
int __fastcall MonsterRegionPickClass(MONREGION *ptRegion, GAMEROOM *hRoom, MONSTATSTXT **pptStats,
	DWORD nPercent, BOOL bUnique);

#endif
