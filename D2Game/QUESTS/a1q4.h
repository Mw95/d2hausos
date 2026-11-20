#ifndef D2RE_D2GAME_A1Q4_H
#define D2RE_D2GAME_A1Q4_H

#include "../D2game.h"

struct A1Q4RECORD
{
 	WORD		awOrder[5];
 	BYTE		unobserved_0a[2];
 	WORD		wNumStonesTouched;
 	BYTE		unobserved_0e[2];
 	DWORD		adwStones[5];
 	BYTE		unobserved_24[4];
 	DWORD		dwInvisibleObjectId;
 	WORD		wNoScrollTouches;
 	BYTE		unobserved_2e[2];
 	DWORD		dwTreeId;
 	DWORD		dwGibbetId;
 	DWORD		dwScrollItemId;
 	DWORD		dwGibbetPlayerId;
 	DWORD		dwAlphaStoneId;
 	BYTE		bStonePortalTimerPending;
 	BYTE		bStonePortalOpened;
 	BYTE		bCainTimerPending;
 	BYTE		bTreePresent;
 	BYTE		bGibbetPresent;
 	BYTE		bInvisibleObjectPresent;
 	BYTE		bOrderDrawn;
 	BYTE		bScrollItemIdSet;
 	BYTE		field_4c;
 	BYTE		bScrollDeciphered;
 	BYTE		bStonesDone;
 	BYTE		bQuestDone;
 	BYTE		bTownCainMade;
 	BYTE		bTownCainWanted;
 	BYTE		bStatus1Pending;
 	BYTE		unobserved_53;
 	DWORD		dwGibbetMode;
 	DWORD		dwTreeMode;
 	BYTE		abStones[5];
 	BYTE		bCainFreed;
 	BYTE		bCainNotCreated;
 	BYTE		bCreditedPlayerSeen;
 	BYTE		bStatus3Pending;
 	BYTE		unobserved_65;
 	BYTE		bTownPortalOpened;
 	BYTE		bCainPortalSpotSet;
 	DWORD		dwTownCainId;
 	DWORD		dwCainStartId;
 	BYTE		bCainStartPresent;
 	BYTE		unobserved_71[3];
 	UNIT		*hTristramPlayer;
 	BYTE		bScrollObtained;
 	BYTE		unobserved_79[3];
 	int			nScrollsHeld;
 	int			nTownPortalTicks;
 	int			nCainStartX;
 	int			nCainStartY;
 	int			nWirtDropsLeft;
 	BYTE		bWirtDropsDrawn;
 	BYTE		field_91;
 	BYTE		field_92;
 	BYTE		bCainActivated;
 	BYTE		unobserved_94;
 	BYTE		bCainPortalMade;
 	BYTE		bTownCainPortalMade;
 	BYTE		unobserved_97;
 	DWORD		dwCainPortalId;
 	GAMECOORD	tCainPortalSpot;
 	DWORD		dwTownCainPortalId;
 	GAMECOORD	tCainWalkSpot;
 	QUESTPLAYERS	tPlayers1;
 	QUESTPLAYERS	tPlayers2;
};
typedef struct A1Q4RECORD A1Q4RECORD;

void __fastcall A1Q4SetDone(QUEST *pQuest, BYTE bTimer, BYTE bNotify);
BOOL __fastcall A1Q4FreeCain(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A1Q4CainTimer(GAME *ptGame, QUEST *pQuest);
void __fastcall A1Q4UnitAttached(GAME *ptGame, QUEST *pQuest, UNIT *hUnit);
void __fastcall A1Q4ObjectEvent(GAME *ptGame, UNIT *hObject);
int __fastcall A1Q4OperateObject(OBJOPERATE *pObjectParam);
void __fastcall A1Q4StoneInit(OBJINIT *ptInit);
int __fastcall A1Q4OperateStone(OBJOPERATE *pObjectParam);
int __fastcall A1Q4OperateTree(OBJOPERATE *pObjectParam);
void __fastcall A1Q4SendOrder(GAME *ptGame, UNIT *hUnit);
void __fastcall A1Q4Init(QUEST *pQuest);
void __fastcall A1Q4SpawnNpc(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY);
void __fastcall A1Q4TreeObjInit(OBJINIT *ptInit);
void __fastcall A1Q4AttachObjInit(OBJINIT *ptInit);
void __fastcall A1Q4CairnObjInit(QUEST *pQuest, OBJINIT *ptInit);
void __fastcall A1Q4RemovePlayer(GAME *ptGame, UNIT *hUnit);
void __fastcall A1Q4EnteredLevel(GAME *ptGame, UNIT *hUnit);
void __fastcall A1Q4NpcObjInit(OBJINIT *ptInit);
BOOL __fastcall A1Q4CastAtSpot(GAME *ptGame, UNIT *hUnit);
void __fastcall A1Q4EventObjInit(OBJINIT *ptInit);
void __fastcall A1Q4OperateEvent(QUEST *pQuest, UNIT *hObject);
BOOL __fastcall A1Q4GetCainWalkSpot(GAME *ptGame, UNIT *hUnit, GAMECOORD *ptCoord);
void __fastcall A1Q4CreateObject(GAME *ptGame, UNIT *hUnit);

#endif
