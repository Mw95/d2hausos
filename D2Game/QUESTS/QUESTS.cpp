#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\QUESTS.CPP"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "../GAME/Event.h"
#include "../GAME/SCmd.h"
#include "../ITEMS/Items.h"
#include "../ITEMS/ItemMode.h"
#include "../MONSTER/MonsterMsg.h"
#include "../MONSTER/MonsterUnique.h"
#include "../PLAYER/PlrIntro.h"
#include "../PLAYER/PlrMsg.h"
#include "QUESTS.h"
#include "a1q1.h"
#include "a1q2.h"
#include "a1q3.h"
#include "a1q4.h"
#include "a1q5.h"
#include "a1q6.h"
#include "a2q0.h"
#include "a2q1.h"
#include "a2q2.h"
#include "a2q3.h"
#include "a2q4.h"
#include "a2q5.h"
#include "a2q6.h"
#include "a2q7.h"
#include "a2q8.h"
#include "a3q0.h"
#include "a3q1.h"
#include "a3q2.h"
#include "a3q3.h"
#include "a3q4.h"
#include "a3q5.h"
#include "a3q6.h"
#include "a3q7.h"
#include "a4q1.h"
#include "a4q2.h"
#include "QuestsFX.h"
#include "a4q3.h"
#include "../UNIT/SUnit.h"
#include "../AI/AiGeneral.h"

struct QUESTINITROW
{
 	PFNQUESTINIT	pQuestInit;
 	BYTE		bAct;
 	BYTE		unobserved_05[3];
 	DWORD		field_08;
 	BYTE		bNoReset;
 	BYTE		unobserved_0d[3];
 	int			nQuestId;
 	int			nRecordWord;
};
typedef struct QUESTINITROW QUESTINITROW;

struct QUESTINTROROW
{
 	PFNQUESTINIT	pQuestInit;
 	BYTE		bAct;
 	BYTE		unobserved_05[3];
};
typedef struct QUESTINTROROW QUESTINTROROW;

#pragma pack(push, 1)
struct QUESTSTATUSMSG
{
	BYTE		bCmd;
	BYTE		abStatus[MAX_QUEST_STATUS];
};
struct QUESTINFOMSG
{
	BYTE		bCmd;
	WORD		field_01;
	WORD		field_03;
	WORD		field_05;
	DWORD		field_07;
	DWORD		field_0b;
};
struct QUESTSTATEMSG
{
	BYTE		bCmd;
	BYTE		bQuest;
	BYTE		field_02;
	BYTE		field_03;
	BYTE		field_04;
};
struct QUESTLISTMSG
{
	BYTE		bCmd;
	BYTE		abQuests[30];
};
struct QUESTRECORDMSG
{
	BYTE		bCmd;
	BYTE		abRecord[96];
};
struct QUESTFLAGMSG
{
	BYTE		bCmd;
	BYTE		bValue;
};
struct QUESTNPCMSG
{
	BYTE		bCmd;
	BYTE		field_01;
	DWORD		dwUnitId;
};
struct QUESTINTROMSG
{
	BYTE		bCmd;
	BYTE		bAct;
	WORD		awNpcs[12];
};
struct QUESTSYSMSG
{
	BYTE		bCmd;
	BYTE		field_01;
	DWORD		field_02;
	BYTE		field_06;
	BYTE		unobserved_07;
	BYTE		field_08;
	BYTE		unobserved_09;
	WORD		field_0a;
	BYTE		unobserved_0c[40 - 12];
};
#pragma pack(pop)
typedef struct QUESTSTATUSMSG QUESTSTATUSMSG;
typedef struct QUESTINFOMSG QUESTINFOMSG;
typedef struct QUESTSTATEMSG QUESTSTATEMSG;
typedef struct QUESTLISTMSG QUESTLISTMSG;
typedef struct QUESTRECORDMSG QUESTRECORDMSG;
typedef struct QUESTFLAGMSG QUESTFLAGMSG;
typedef struct QUESTNPCMSG QUESTNPCMSG;
typedef struct QUESTINTROMSG QUESTINTROMSG;
typedef struct QUESTSYSMSG QUESTSYSMSG;

#define QUEST_CODE_RING			544106866
#define QUEST_CODE_BKD			543452002
#define QUEST_CODE_TRS			544436852

static BYTE sgbQuestGate = 1;

static QUESTINITROW sgQuestInit[30] =
{
	{ QuestsInitWarriv,      0, { 0, 0, 0 }, 100, 1, { 0, 0, 0 }, 0, 0 },
	{ A1Q1Init,         0, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 1, 1 },
	{ A1Q2Init,         0, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 2, 2 },
	{ A1Q3Init,         0, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 3, 3 },
	{ A1Q4Init,         0, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 4, 4 },
	{ A1Q5Init,         0, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 5, 5 },
	{ A1Q6Init,         0, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 6, 6 },
	{ A2Q0Init,         1, { 0, 0, 0 }, 100, 1, { 0, 0, 0 }, 7, 8 },
	{ A2Q1Init,         1, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 8, 9 },
	{ A2Q2Init,         1, { 0, 0, 0 }, 100, 1, { 0, 0, 0 }, 9, 10 },
	{ A2Q3Init,         1, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 10, 11 },
	{ A2Q4Init,         1, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 11, 12 },
	{ A2Q5Init,         1, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 12, 13 },
	{ A2Q6Init,         1, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 13, 14 },
	{ A2Q7Init,         1, { 0, 0, 0 }, 100, 1, { 0, 0, 0 }, 26, 30 },
	{ A2Q8Init,         1, { 0, 0, 0 }, 100, 1, { 0, 0, 0 }, 27, 31 },
	{ A3Q0Init,         2, { 0, 0, 0 }, 100, 1, { 0, 0, 0 }, 14, 16 },
	{ A3Q1Init,         2, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 15, 17 },
	{ A3Q2Init,         2, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 16, 18 },
	{ A3Q3Init,         2, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 17, 19 },
	{ A3Q4Init,         2, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 18, 20 },
	{ A3Q5Init,         2, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 19, 21 },
	{ A3Q6Init,         2, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 20, 22 },
	{ A3Q7Init,         2, { 0, 0, 0 }, 100, 0, { 0, 0, 0 }, 28, 32 },
	{ A4Q0Init,         3, { 0, 0, 0 }, 100, 1, { 0, 0, 0 }, 21, 24 },
	{ A4Q1Init,         3, { 0, 0, 0 }, 0, 0, { 0, 0, 0 }, 22, 25 },
	{ A4Q2Init,         3, { 0, 0, 0 }, 0, 0, { 0, 0, 0 }, 23, 26 },
	{ A4Q3Init,         3, { 0, 0, 0 }, 0, 0, { 0, 0, 0 }, 24, 27 },
	{ A1Q6NaviInit,   0, { 0, 0, 0 }, 100, 1, { 0, 0, 0 }, 25, 29 },
	{ QuestsFXInit,     3, { 0, 0, 0 }, 100, 1, { 0, 0, 0 }, 29, 33 },
};
static int sgnQuestInit = 30;
static QUESTINTROROW sgQuestIntro[3] =
{
	{ QuestsIntroInit,  0, { 0, 0, 0 } },
	{ A1Q6IntroInit,    1, { 0, 0, 0 } },
	{ A2Q8IntroInit,    2, { 0, 0, 0 } },
};
static int sgnQuestIntro = 3;
static int sgnQuestNpcsAct2[6] = { 148, 154, 150, 147, 155, 265 };
static int sgnQuestNpcsAct3[11] =
	{ 175, 176, 177, 178, 202, 200, 210, 201, 198, 199, 244 };
static int sgnQuestNpcsAct4[7] = { 245, 252, 253, 254, 255, 264, 297 };
static DWORD sgdwQuestDebug;

void __fastcall QuestsFreeUnitLinks(GAME *ptGame  , QUESTLINK *pHead)
{
	QUESTLINK *pNext;

	while (pHead)
	{
		pNext = pHead->pNext;
#line 69
		D2FREEPOOL(ptGame->pMemPool, pHead);
		pHead = pNext;
	}
}

static __inline BOOL QuestsLinkUnit(GAME *ptGame, UNIT *hUnit  , QUEST *pQuest)
{
	QUESTLINK *pHead;

	pHead = hUnit->field_94;
#line 81
	D2ASSERT(hUnit);
	for (; pHead && pHead->pQuest; pHead = pHead->pNext)
	{
		if (pHead->pQuest == pQuest)
			return FALSE;
	}
#line 93
	pHead = (QUESTLINK *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(QUESTLINK));
	pHead->pQuest = pQuest;
	pHead->pNext = hUnit->field_94;
	hUnit->field_94 = pHead;
	return TRUE;
}

QUEST * __fastcall QuestsFind(GAME *ptGame  , int nId  )
{
	QUESTHEAD *pQuestHead;
	QUEST *pQuest;

	pQuestHead = ptGame->pQuestHead;
#line 108
	D2ASSERT(nId >= 0);
	if (!pQuestHead)
		return NULL;
	for (pQuest = pQuestHead->pQuestList; pQuest; pQuest = pQuest->pNext)
	{
		if (pQuest->Id == nId)
			return pQuest;
	}
	return NULL;
}

void __fastcall QuestsAttachLevelQuest(GAME *ptGame  , UNIT *hUnit
, GAMEROOM *hRoom, BOOL fAttach)
{
	int nLevelId;
	MISSLEVELDATA *ptLevel;
	BYTE bQuest;

	nLevelId = (int)D2CommonDungeonRoomGetLevelId(hRoom);
	if (!nLevelId)
		return;
	ptLevel = (MISSLEVELDATA *)D2CommonLvlTblsGetLevelData(nLevelId);
	bQuest = ptLevel->Quest;
	if (!bQuest)
		return;
	if (fAttach)
	{
		if (!QuestsAttachUnit(ptGame, hUnit, ptLevel->Quest) && sgdwQuestDebug)
			PlrMsgSendChatAll(ptGame, "Error attaching to unit", 4);
	}
	else
	{
		if (QuestsAttachUnit(ptGame, hUnit, ptLevel->Quest) == TRUE && sgdwQuestDebug)
			PlrMsgSendChatAll(ptGame, "Error attaching to unit", 4);
	}
}

BOOL __fastcall QuestsAttachUnit(GAME *ptGame  , UNIT *hUnit
, int nQuest  )
{
	QUEST *pQuest;
	DWORD dwType;

#line 212
	D2ASSERT(nQuest >= 0);
	pQuest = QuestsFind(ptGame, nQuest);
#line 239
	if (pQuest) {
		dwType = hUnit->eType;
		if (nQuest == 4)
		{
			if (dwType == 2 && hUnit->dwClassId == 61)
				A1Q4UnitAttached(ptGame, pQuest, hUnit);
		}
		else if (nQuest == 8)
		{
			void __fastcall A2Q5UnitAttached(GAME *ptGame, QUEST *pQuest, UNIT *hUnit);

			if (dwType == UNIT_MONSTER && hUnit->dwClassId == 229)
				A2Q5UnitAttached(ptGame, pQuest, hUnit);
		}
		else if (nQuest == 12)
		{
			void __fastcall A2Q5UnitAttached(GAME *ptGame, QUEST *pQuest, UNIT *hUnit);

			if (dwType == UNIT_MONSTER && hUnit->dwClassId == 250)
				A2Q5UnitAttached(ptGame, pQuest, hUnit);
		}
		return QuestsLinkUnit(ptGame, hUnit, pQuest); }
#line 239
	D2ASSERT(pQuest);
}
#line 257

void __fastcall QuestsNpcTable(QUEST *pQuest  , void *pArg,
	int nNpcClass, int nIndex)
{
	const QUESTNPCTABLE *ptTable;
	const struct QUESTNPCENTRY *ptEntry;
	DWORD dwFlag;
	int i;

	ptTable = pQuest->ptNpcTables;
	for (i = 0; i < ptTable[nIndex].nCount; i++)
	{
		ptEntry = &ptTable[nIndex].tEntry[i];
		if (ptEntry->nNpcClass == nNpcClass)
		{
			dwFlag = ptEntry->dwFlag;
			if (dwFlag == 1)
				dwFlag = 0;
			D2CommonTextListInsert(pArg, ptEntry->wArg, dwFlag);
		}
	}
}

void __fastcall QuestsInit(GAME *ptGame  )
{
	QUESTHEAD *pQuestHead;
	QUEST *pQuest;
	QUEST *pList;
	DWORD dwSeed;
	int Index;
	DWORD i;
	int nId;
	BYTE bAct;

	pList = NULL;
	pQuest = NULL;
	for (Index = 0; Index < sgnQuestInit; Index++)
	{
#line 373
		pQuest = (QUEST *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(QUEST));
		bAct = sgQuestInit[Index].bAct;
		nId = sgQuestInit[Index].nQuestId;
		for (i = 0; i < sizeof(QUEST) / sizeof(DWORD); i++)
			((DWORD *)pQuest)[i] = 0;
		pQuest->Id = nId;
		pQuest->ptGame = ptGame;
		pQuest->bCallbacksEnabled = 0;
		pQuest->bStatus = 0;
		pQuest->bActive = 1;
		pQuest->bAct = bAct;
		pQuest->tPlayers.wCount = 0;
		pQuest->pNext = pList;
		pList = pQuest;
		if (sgQuestInit[Index].pQuestInit)
		{
#line 379
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgQuestInit[Index].pQuestInit), "sgQuestInit[Index].pQuestInit");
			sgQuestInit[Index].pQuestInit(pQuest);
		}
	}
	for (Index = 0; Index < sgnQuestIntro; Index++)
	{
#line 385
		pQuest = (QUEST *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(QUEST));
		for (i = 0; i < sizeof(QUEST) / sizeof(DWORD); i++)
			((DWORD *)pQuest)[i] = 0;
		pQuest->Id = Index + 30;
		pQuest->ptGame = ptGame;
		pQuest->bCallbacksEnabled = 1;
		pQuest->bStatus = 0;
		pQuest->bActive = 0;
		pQuest->bAct = sgQuestIntro[Index].bAct;
		pQuest->pNext = pList;
		pList = pQuest;
		if (sgQuestIntro[Index].pQuestInit)
		{
#line 397
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgQuestIntro[Index].pQuestInit), "sgQuestIntro[Index].pQuestInit");
			sgQuestIntro[Index].pQuestInit(pQuest);
		}
	}
#line 402
	pQuestHead = (QUESTHEAD *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(QUESTHEAD));
	pQuestHead->pQuestList = pQuest;
	pQuestHead->bPickedSet = FALSE;
	pQuestHead->pTimers = NULL;
	pQuestHead->dwTick = 0;
	D2CommonSeedInit(&pQuestHead->tSeed);
	dwSeed = D2CommonSeedInvent(0);
	D2CommonSeedSet(&pQuestHead->tSeed, dwSeed);
	pQuestHead->bExecuting = FALSE;
	pQuestHead->pQuestRecord = (void *)D2CommonQuestRecordNew(ptGame->pMemPool);
	ptGame->pQuestHead = pQuestHead;
}

void __fastcall QuestsFree(GAME *ptGame  )
{
	QUESTHEAD *pQuestHead;
	QUEST *pQuest;
	QUEST *pNext;
	QUESTTIMER *pTimer;
	QUESTTIMER *pTimerNext;

	pQuestHead = ptGame->pQuestHead;
	if (!pQuestHead)
		return;
	pQuest = pQuestHead->pQuestList;
	while (pQuest)
	{
		pNext = pQuest->pNext;
		if (pQuest->pQuestRecord)
		{
#line 447
			D2FREEPOOL(ptGame->pMemPool, pQuest->pQuestRecord);
		}
#line 449
		D2FREEPOOL(ptGame->pMemPool, pQuest);
		pQuest = pNext;
	}
	pTimer = pQuestHead->pTimers;
	while (pTimer)
	{
		pTimerNext = pTimer->pNext;
#line 456
		D2FREEPOOL(ptGame->pMemPool, pTimer);
		pTimer = pTimerNext;
	}
	D2CommonQuestRecordFree(ptGame->pMemPool, pQuestHead->pQuestRecord);
#line 460
	D2FREEPOOL(ptGame->pMemPool, pQuestHead);
}

D2SEED * __fastcall QuestsGetSeed(GAME *ptGame  )
{
	QUESTHEAD *pQuestHead;

	pQuestHead = ptGame->pQuestHead;
	if (pQuestHead)
		return &pQuestHead->tSeed;
#line 475
	D2ASSERT(pQuestHead != NULL);
	return &pQuestHead->tSeed;
}

void __fastcall QuestsUnitKilled(GAME *ptGame  , UNIT *hUnit
, UNIT *hKiller)
{
	QUESTPARAM tParam;
	struct MONSTERUNIQUE *ptUnique;
	CLIENT *ptClient;
	DWORD dwOwnerId;
	DWORD dwOwnerType;
	BYTE fForce;

	if (!hUnit->field_94)
		return;
	if (ptGame->bGameType == 3 && !hKiller)
	{
		ptClient = ClientsGetFirst(ptGame);
		if (ptClient)
			hKiller = ClientGetPlayerUnit(ptClient, FALSE);
	}
	tParam.ptGame = ptGame;
	tParam.eCallback = 8;
	tParam.hEventUnit = hUnit;
	if (hKiller && hKiller->eType == UNIT_PLAYER)
		tParam.hPlayer = hKiller;
	else
	{
		tParam.hPlayer = NULL;
		if (hKiller && hKiller->eType == UNIT_MONSTER)
		{
			AiGetOwnerId(hKiller, &dwOwnerId, &dwOwnerType);
			if (dwOwnerType == UNIT_PLAYER)
				tParam.hPlayer = SUnitFind(ptGame, UNIT_PLAYER, dwOwnerId);
		}
	}
	fForce = FALSE;
	ptUnique = ((MONSTERDATA *)hUnit->uTypeData)->ptUnique;
	if (MonsterUniqueIsSuperUnique(ptUnique))
	{
		switch (MonsterUniqueGetId(ptUnique))
		{
		case 65519:
		case 65520:
		case 65522:
		case 65529:
		case 65530:
		case 65531:
		case 65532:
			fForce = TRUE;
			break;
		}
	}
	if (hUnit->dwClassId >= 242 && (hUnit->dwClassId <= 243 || hUnit->dwClassId == 391))
		fForce = TRUE;
	QuestsUnitCallback(&tParam, fForce);
}

void __fastcall QuestsUnitCallback(QUESTPARAM *pqParam  ,
	BYTE fForce)
{
	UNIT *hUnit = pqParam->hEventUnit;

	for (QUESTLINK *pHead = hUnit->field_94; pHead; pHead = pHead->pNext)
	{
		if (pHead->pQuest->pCallback[pqParam->eCallback])
		{
			if (pHead->pQuest->bCallbacksEnabled == 1 || fForce)
			{
#line 542
				D2ASSERTMSG(!IsBadCodePtr((FARPROC)pHead->pQuest->pCallback[pqParam->eCallback]), "pHead->pQuest->pCallback[pqParam->eCallback]");
				pHead->pQuest->pCallback[pqParam->eCallback](pHead->pQuest, pqParam);
			}
		}
	}
}

void __fastcall QuestsLevelChange(GAME *ptGame  , DWORD dwLevelOld,
	DWORD dwLevelNew, UNIT *hUnit  )
{
	QUESTPARAM tParam;

	tParam.ptGame = ptGame;
	tParam.eCallback = 3;
	tParam.field_18 = dwLevelNew;
	tParam.field_14 = dwLevelOld;
	tParam.hPlayer = hUnit;
	QuestsCallback(&tParam, TRUE, FALSE);
}

void __fastcall QuestsCallback(QUESTPARAM *pqParam  , BYTE fForce,
	BYTE fSameAct)
{
	QUEST *pQuest;
	GAMEROOM *hRoom;
	BYTE bAct;

	if (!pqParam->ptGame->pQuestHead)
		return;
	pQuest = pqParam->ptGame->pQuestHead->pQuestList;
	bAct = 255;
	if (fSameAct == TRUE)
	{
		hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(pqParam->hPlayer);
		if (hRoom)
			bAct = D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom));
	}
	for (; pQuest; pQuest = pQuest->pNext)
	{
		if (pQuest->pCallback[pqParam->eCallback] && (fForce || pQuest->bCallbacksEnabled == 1)
			&& (bAct == 255 || bAct == pQuest->bAct))
		{
#line 518
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuest->pCallback[pqParam->eCallback]), "pQuest->pCallback[pqParam->eCallback]");
			pQuest->pCallback[pqParam->eCallback](pQuest, pqParam);
		}
	}
}

void __fastcall QuestsPlayerDropped(GAME *ptGame  , UNIT *hUnit
)
{
	void *pInventory;
	QUESTPARAM tParam;
	QUEST *pQuestInfo;
	ITEMSTXT *ptItemStats;
	UNIT *hItem;
	void *pNode;
	void *pNext;

	tParam.hPlayer = hUnit;
	tParam.ptGame = ptGame;
	tParam.eCallback = QUESTCALLBACK_PLAYERDROPPEDWITHQUESTITEM;
	pInventory = hUnit->ptInventory;
	pNode = D2CommonINVENTORYGetCategoryListHead(pInventory);
	if (pNode)
	do
	{
		pNext = (void *)D2CommonINVENTORYNodeGetNext(pNode);
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(pNode);
		ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
		if (ptItemStats->quest)
		{
			pQuestInfo = QuestsFind(ptGame, ptItemStats->quest - 1);
			tParam.hEventUnit = hItem;
			if (pQuestInfo && pQuestInfo->pCallback[QUESTCALLBACK_PLAYERDROPPEDWITHQUESTITEM])
			{
#line 693
				D2ASSERTMSG( !IsBadCodePtr((FARPROC)pQuestInfo->pCallback[QUESTCALLBACK_PLAYERDROPPEDWITHQUESTITEM]), "pQuestInfo->pCallback[QUESTCALLBACK_PLAYERDROPPEDWITHQUESTITEM]");
				pQuestInfo->pCallback[QUESTCALLBACK_PLAYERDROPPEDWITHQUESTITEM](pQuestInfo, &tParam);
			}
		}
		pNode = pNext;
	} while (pNode);
	tParam.eCallback = 10;
	QuestsCallback(&tParam, TRUE, FALSE);
}

void __fastcall QuestsNpcMessage(CLIENT *ptClient  , UNIT *hUnit
, UNIT *hNpc, DWORD dwMessage)
{
	QUESTPARAM tParam;

	tParam.ptGame = ClientGetGame(ptClient);
	tParam.eCallback = 0;
	tParam.hEventUnit = hNpc;
	tParam.field_14 = dwMessage;
	tParam.hPlayer = hUnit;
	QuestsCallback(&tParam, TRUE, TRUE);
}

void __fastcall QuestsNpcActivate(GAME *ptGame  , UNIT *hUnit
, UNIT *hNpc)
{
	QUESTPARAM tParam;

	tParam.ptGame = ptGame;
	tParam.hPlayer = hUnit;
	tParam.hEventUnit = hNpc;
	tParam.eCallback = 2;
	QuestsCallback(&tParam, TRUE, TRUE);
}

void __fastcall QuestsItemAcquired(GAME *ptGame  , UNIT *hUnit
, UNIT *hItem  )
{
	QUESTPARAM tParam;

	tParam.ptGame = ptGame;
	tParam.hPlayer = hUnit;
	tParam.eCallback = 4;
	tParam.hEventUnit = hItem;
	QuestsUnitCallback(&tParam, FALSE);
}

void __fastcall QuestsItemDropped(GAME *ptGame  , UNIT *hUnit
, UNIT *hItem  )
{
	QUESTPARAM tParam;

	tParam.ptGame = ptGame;
	tParam.hPlayer = hUnit;
	tParam.eCallback = 5;
	tParam.hEventUnit = hItem;
	QuestsUnitCallback(&tParam, FALSE);
}

void __fastcall QuestsUpdateTimers(GAME *ptGame  )
{
	QUESTHEAD *pQuestHead;
	QUESTTIMER *pTimer;
	QUESTTIMER *pPrev;
	QUESTTIMER *pNext;

	pQuestHead = ptGame->pQuestHead;
	if (!pQuestHead)
		return;
	if (++pQuestHead->dwTick == 0xFFFFFFFF)
	{
		for (pTimer = pQuestHead->pTimers; pTimer; pTimer = pTimer->pNext)
			pTimer->dwDue = 0xFFFFFFFF - pTimer->dwDue;
	}
	pQuestHead->bExecuting = TRUE;
	pPrev = NULL;
	for (pTimer = pQuestHead->pTimers; pTimer; pTimer = pNext)
	{
		pNext = pTimer->pNext;
		if (pTimer->dwDue < pQuestHead->dwTick)
		{
#line 964
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)pTimer->pCallback), "pTimer->pCallback");
			if (pTimer->pCallback(ptGame, pTimer->pQuest) == 1)
			{
				if (!pPrev)
				{
					pQuestHead->pTimers = pTimer->pNext;
#line 971
					D2FREEPOOL(ptGame->pMemPool, pTimer);
					pTimer = NULL;
				}
				else
				{
					pPrev->pNext = pTimer->pNext;
#line 978
					D2FREEPOOL(ptGame->pMemPool, pTimer);
					pTimer = pPrev;
				}
			}
			else
				pTimer->dwDue = pTimer->dwInterval + pQuestHead->dwTick;
		}
		pPrev = pTimer;
	}
	pQuestHead->bExecuting = FALSE;
}

void __fastcall QuestsAddTimer(QUEST *pQuest  ,
	PFNQUESTTIMER pfnTimer, DWORD dwInterval)
{
	QUESTHEAD *pQuestHead;
	QUESTTIMER *pTimer;
	QUESTTIMER *pNext;

	pQuestHead = pQuest->ptGame->pQuestHead;
	if (!pQuestHead)
		return;
#line 1003
	D2ASSERT(!pQuestHead->bExecuting);
	pNext = pQuestHead->pTimers;
#line 1006
	pTimer = (QUESTTIMER *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(QUESTTIMER));
	pTimer->pNext = pNext;
	pTimer->pQuest = pQuest;
	pTimer->pCallback = pfnTimer;
	pTimer->dwDue = pQuestHead->dwTick + dwInterval;
	pTimer->dwInterval = dwInterval;
	pQuestHead->pTimers = pTimer;
}

void __fastcall QuestsGetStatus(QUEST *pQuest  , BYTE *pbStatus,
	void *pRecord)
{
	DWORD dwBit14;

	dwBit14 = D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 14);
	if (pQuest->bState >= pQuest->bDoneStateMin)
	{
		if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 13) == 1)
		{
#line 1065
			D2ASSERT(pQuest->eFilter < MAX_QUEST_STATUS);
			*pbStatus = pQuest->bStatus;
			return;
		}
		if (pQuest->Id == 4 && dwBit14)
		{
			if (pQuest->bState == 6)
				*pbStatus = 12;
			else
				*pbStatus = pQuest->bStatus;
			return;
		}
		if (pQuest->Id == 10)
		{
			if (pQuest->bStatus == 4)
				*pbStatus = 4;
			else
				*pbStatus = 12;
			return;
		}
		goto set12;
	}
	if (dwBit14)
	{
set12:
		*pbStatus = 12;
		return;
	}
	if (pQuest->Id == 4 && pQuest->bStatus == 6
		&& !D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 13))
	{
		*pbStatus = 12;
		return;
	}
	*pbStatus = pQuest->bStatus;
}

void __fastcall QuestsSendAllStatus(GAME *ptGame  , UNIT *hPlayer
)
{
	QUESTHEAD *pQuestHead;
	QUEST *pQuest;
	CLIENT *ptClient;
	void *pRecord;
	QUESTSTATUSMSG tStatus;
	QUESTINFOMSG tInfo;
	BYTE fSend;
	BYTE bStatus;
	int i;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	SCmdSendQuests(SUnitGetClient(hPlayer), S2C_NPC_DIALOG, 6, 0, pRecord, 0);
	ptClient = SUnitGetClient(hPlayer);
	tStatus.bCmd = 75;
	for (i = 0; i < (int)(sizeof(tStatus.abStatus) / sizeof(DWORD)); i++)
		((DWORD *)tStatus.abStatus)[i] = 0;
	pQuestHead = ptGame->pQuestHead;
	if (!pQuestHead)
		return;
	pQuest = pQuestHead->pQuestList;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (pQuest)
	{
		while (1)
		{
#line 1159
			D2ASSERT(pQuest->Id < MAX_QUEST_STATUS);
			if (pQuest->bStatus)
			{
				if (!pQuest->pStatusFilter)
					QuestsGetStatus(pQuest, &tStatus.abStatus[pQuest->eFilter], pRecord);
				else
				{
#line 1183
					D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuest->pStatusFilter), "pQuest->pStatusFilter");
					if (pQuest->pStatusFilter(pQuest, hPlayer, pQuestHead->pQuestRecord, pRecord, &bStatus) == 1)
						tStatus.abStatus[pQuest->eFilter] = bStatus;
				}
			}
			pQuest = pQuest->pNext;
			if (!pQuest)
				break;
		}
	}
	memset(&tInfo, 0, sizeof(tInfo));
	fSend = FALSE;
	if (tStatus.abStatus[1])
	{
		pQuest = QuestsFind(ptGame, 1);
		if (pQuest)
		{
			fSend = TRUE;
			tInfo.field_03 = (WORD)A1Q1GetRemaining(pQuest);
		}
	}
	if ((D2CommonStepTestGridBit(pRecord, 12, 0) || D2CommonStepTestGridBit(pRecord, 12, 13))
		&& ptGame->ptAct[1])
	{
		tInfo.field_05 = (WORD)((DWORD)D2CommonDungeonActGetDrawnLevel(ptGame->ptAct[1]) - 66);
		fSend = TRUE;
	}
	if (fSend)
	{
		tInfo.bCmd = 73;
		tInfo.field_01 = 1;
		SCmdSendQuestCommand(ptClient, (const struct SCMDCOPY15 *)&tInfo);
	}
	SCmdSendQuestLog(ptClient, &tStatus);
}

void __fastcall QuestsDebugMessage(GAME *ptGame  ,
	const char *szText, const char *szFile, int nLine)
{
	if (sgdwQuestDebug)
		PlrMsgSendChatAll(ptGame, szText, 4);
}

void __fastcall QuestsRemoveItem(GAME *ptGame  , UNIT *hUnit
, DWORD dwType)
{
	UNIT *hItem;
	void *ptInventory;
	CLIENT *ptClient;

	hItem = ITEMSFindByType(hUnit, dwType);
	if (!hItem)
		return;
	ptInventory = hUnit->ptInventory;
	ptClient = SUnitGetClient(hUnit);
	dwType = 0;
	D2CommonITEMSSetOrClearFlags(&dwType, 32, 1);
	switch (hItem->dwMode)
	{
	case 0:
		D2CommonITEMSDataSetPreviousPage(hItem, D2CommonITEMSDataGetPageRaw((DWORD)hItem));
		SCmdSendItemFromGrid(ptClient, hUnit, hItem, dwType);
		ITEMSModeRemoveGridItem(ptGame, hUnit, hItem, FALSE);
		break;
	case 1:
	{
		int nResult;
		DWORD dwSlot;

		dwSlot = D2CommonINVENTORYFindSlotIndex(ptInventory, hItem);
		ITEMSModeUnequipToCursor(ptGame, hUnit, dwSlot, TRUE, &nResult);
		break;
	}
	case 4:
		ITEMSModeFreeCursorItem(ptGame, hUnit, hItem);
		D2CommonINVENTORYSetCursorItem(ptInventory, 0);
		break;
	}
}

void __fastcall QuestsSendStatus(GAME *ptGame  , UNIT *hPlayer
, BYTE bQuest)
{
	QUEST *pQuestInfo;
	CLIENT *ptClient;
	GAMEROOM *hRoom;
	void *pRecord;
	QUESTSTATEMSG tMsg;
	int nLevelId;
	BYTE bFlags;

	pQuestInfo = QuestsFind(ptGame, bQuest);
	if (!pQuestInfo)
		return;
	ptClient = SUnitGetClient(hPlayer);
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hPlayer);
	if (!hRoom)
		return;
	nLevelId = (int)D2CommonDungeonRoomGetLevelId(hRoom);
	if (!nLevelId)
		return;
	if (pQuestInfo->bAct != D2CommonGetActNum(nLevelId))
		return;
	bFlags = pQuestInfo->bStatusFlags;
	tMsg.bCmd = 86;
	tMsg.field_02 = bFlags;
	tMsg.bQuest = bQuest;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (!pQuestInfo->pStatusFilter)
		QuestsGetStatus(pQuestInfo, &tMsg.field_03, pRecord);
	else
	{ BYTE bStatus;
		bStatus = 0;
#line 1371
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pStatusFilter), "pQuestInfo->pStatusFilter");
		if (pQuestInfo->pStatusFilter(pQuestInfo, hPlayer, ptGame->pQuestHead->pQuestRecord, pRecord,
			&bStatus) == 1)
			tMsg.field_03 = bStatus;
	}
	if (pQuestInfo->eFilter == 1)
		tMsg.field_04 = (BYTE)A1Q1GetRemaining(pQuestInfo);
	SCmdSendQuestStatus(ptClient, &tMsg);
}

void __fastcall QuestsSetStatus(QUEST *pQuest  , DWORD bStatus,
	void *pArg, PFNSUNITITERATE pfnIterate  ,
	BYTE fIterate)
{
	pQuest->bStatus = bStatus;
	if (fIterate == 1)
	{
		if (pfnIterate)
			SUnitIterate(pQuest->ptGame, UNIT_PLAYER, pArg, pfnIterate);
		else
		{
#line 1401
			D2ASSERT(pfnIterate != NULL);
		}
	}
}

void __fastcall QuestsSetState(QUEST *pQuest  , int nState,
	const char *szFile, int nLine)
{
	char szText[128];

	pQuest->bState = (BYTE)nState;
	wsprintfA(szText, "changing state to %d\n", nState);
	if (sgdwQuestDebug)
		PlrMsgSendChatAll(pQuest->ptGame, szText, 4);
}

void __fastcall QuestsMonsterMessage(GAME *ptGame  , UNIT *hUnit
, DWORD dwMonsterId, WORD wMessage)
{
	QUESTPARAM tParam;
	UNIT *hMonster;

	tParam.ptGame = ptGame;
	tParam.eCallback = 11;
	tParam.hPlayer = hUnit;
	tParam.field_14 = 0;
	if (dwMonsterId == 0xFFFFFFFF)
		tParam.hEventUnit = NULL;
	else
	{
		hMonster = SUnitFind(ptGame, UNIT_MONSTER, dwMonsterId);
		tParam.hEventUnit = hMonster;
		if (hMonster)
			tParam.field_14 = hMonster->dwClassId;
	}
	tParam.field_18 = wMessage;
	QuestsCallback(&tParam, TRUE, TRUE);
}

static __inline void QuestsResetQuest(GAME *ptGame, int nId)
{
	QUEST *pQuestInfo;

	pQuestInfo = QuestsFind(ptGame, nId);
#line 1460
	D2ASSERT(pQuestInfo != NULL);
	pQuestInfo->bActive = 0;
	pQuestInfo->bCallbacksEnabled = 0;
	pQuestInfo->bStatus = 0;
}

void __fastcall QuestsPlayerStarted(GAME *ptGame  , UNIT *hUnit
, BOOL fNoReset)
{
	QUESTHEAD *pQuestHead;
	QUEST *pQuest;
	QUEST *pQuestInfo;
	QUESTLISTMSG tList;

	CLIENT *ptClient;
	void *pRecord;
	int anActs[NUM_ACTS];
	int Index;

	pQuestHead = ptGame->pQuestHead;
	if (!pQuestHead->bPickedSet)
	{
		pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
		memset(anActs, 0, sizeof(anActs));
		pQuestHead->bPickedSet = TRUE;
		if (!fNoReset)
		{
			if (sgbQuestGate)
			{
				for (Index = 0; Index < sgnQuestInit; Index++)
				{
					QUESTINITROW *pInit = &sgQuestInit[Index];
					if (pInit->bAct < NUM_ACTS)
					{
						if (pInit->bNoReset)
							continue;
						if (!D2CommonStepTestGridBit(pRecord, pInit->nRecordWord, 0)
							&& !D2CommonStepTestGridBit(pRecord, pInit->nRecordWord, 15))
							continue;
						QuestsResetQuest(ptGame, pInit->nQuestId);
						anActs[pInit->bAct]++;
						D2CommonQuestRecordSetBit(pQuestHead->pQuestRecord, pInit->nRecordWord, 15);
						continue;
					}
#line 1528
					D2ASSERT(sgQuestInit[Index].bAct < NUM_ACTS);
				}
			}
			for (pQuest = pQuestHead->pQuestList; pQuest; pQuest = pQuest->pNext)
			{
				QUESTPARAM tParam;

				if (!pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME])
					continue;
#line 1537
				D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME]), "pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME]");
				tParam.ptGame = ptGame;
				tParam.hPlayer = hUnit;
				tParam.eCallback = QUESTCALLBACK_PLAYERSTARTEDGAME;
				pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME](pQuest, &tParam);
			}
		}
		pQuestInfo = QuestsFind(ptGame, 1);
#line 1550
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		pQuestInfo->pSequence(pQuestInfo);
		pQuestInfo = QuestsFind(ptGame, 8);
#line 1553
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		pQuestInfo->pSequence(pQuestInfo);
		pQuestInfo = QuestsFind(ptGame, 18);
#line 1556
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		pQuestInfo->pSequence(pQuestInfo);
		pQuestInfo = QuestsFind(ptGame, 22);
#line 1559
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		pQuestInfo->pSequence(pQuestInfo);
	}
	else
	{
		for (pQuest = pQuestHead->pQuestList; pQuest; pQuest = pQuest->pNext)
		{
			QUESTPARAM tParam;

			if (!pQuest->pCallback[QUESTCALLBACK_PLAYERJOINEDGAME])
				continue;
#line 1570
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuest->pCallback[QUESTCALLBACK_PLAYERJOINEDGAME]), "pQuest->pCallback[QUESTCALLBACK_PLAYERJOINEDGAME]");
			tParam.hPlayer = hUnit;
			tParam.ptGame = ptGame;
			tParam.eCallback = QUESTCALLBACK_PLAYERJOINEDGAME;
			pQuest->pCallback[QUESTCALLBACK_PLAYERJOINEDGAME](pQuest, &tParam);
		}
	}
	tList.bCmd = S2C_QUEST_BLOCK;
	for (Index = 0; Index < sgnQuestInit; Index++)
	{
		pQuestInfo = QuestsFind(ptGame, sgQuestInit[Index].nQuestId);
#line 1592
		D2ASSERT(pQuestInfo != NULL);
		tList.abQuests[Index] = pQuestInfo->bActive;
	}
	ptClient = SUnitGetClient(hUnit);
	SCmdSendQuestList(ptClient, &tList);
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	SCmdSendQuests(SUnitGetClient(hUnit), S2C_NPC_DIALOG, 6, 0, pRecord, 0);
	QuestsSendRecord(ptGame, ptClient);
	pQuestInfo = QuestsFind(ptGame, 1);
	if (!pQuestInfo)
		return;
	if (A1Q1IsDone(pQuestInfo))
	{ QUESTFLAGMSG tFlag;
		tFlag.bCmd = 124;
		tFlag.bValue = 0;
		SCmdSendQuestTrigger(ptClient, &tFlag);
	}
}

DWORD __stdcall QuestsGetPickedSet(GAME *ptGame  )
{
	QUESTHEAD *pQuestHead;

#line 1843
	D2ASSERT(ptGame != NULL);
	pQuestHead = ptGame->pQuestHead;
	if (!pQuestHead)
		return 0;
	return pQuestHead->bPickedSet;
}

BOOL __stdcall QuestsTestQuest(GAME *ptGame  ,
	int nQuest  )
{
	QUESTHEAD *pQuestHead;
	QUEST *pQuest;

#line 1859
	D2ASSERT(ptGame != NULL);
	pQuestHead = ptGame->pQuestHead;
#line 1861
	D2ASSERT(pQuestHead->bPickedSet);
	for (pQuest = pQuestHead->pQuestList; pQuest; pQuest = pQuest->pNext)
	{
		if (pQuest->Id == nQuest)
			return pQuest->bActive == 1;
	}
	return TRUE;
}

void __fastcall QuestsSendRecord(GAME *ptGame  , CLIENT *ptClient
)
{
	QUESTHEAD *pQuestHead;
	QUESTRECORDMSG tMsg;

	pQuestHead = ptGame->pQuestHead;
#line 1885
	D2ASSERT(pQuestHead->bPickedSet);
	tMsg.bCmd = S2C_NPCMENU_BLOB;
	D2CommonQuestRecordCopyOut(pQuestHead->pQuestRecord, tMsg.abRecord, 96, 0);
	SCmdSendQuestRecord(ptClient, &tMsg);
}

BOOL __fastcall QuestsNpcActive(GAME *ptGame  , UNIT *hPlayer
, UNIT *hNPC  )
{
	QUESTHEAD *pQuestHead;
	QUEST *pQuest;
	MONSTATSTXT *ptMonsterStats;
	QUESTNPCMSG tMsg;
	void *pRecord;
	int nNpcClass;
	BYTE bAct;

#line 1900
	D2ASSERT(UnitGetType(hPlayer) == (WORD)UNIT_PLAYER);
#line 1901
	D2ASSERT(UnitGetType(hNPC) == (WORD)UNIT_MONSTER);
	nNpcClass = hNPC->dwClassId;
	ptMonsterStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(nNpcClass);
#line 1906
	D2ASSERT(ptMonsterStats->bInteract != FALSE);
	pQuestHead = ptGame->pQuestHead;
#line 1910
	D2ASSERT(pQuestHead->bPickedSet);
	bAct = ClientGetAct(SUnitGetClient(hPlayer));
	for (pQuest = pQuestHead->pQuestList; pQuest; pQuest = pQuest->pNext)
	{
		if (bAct != pQuest->bAct)
			continue;
		if (pQuest->pActiveFilter)
		{
#line 1924
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuest->pActiveFilter), "pQuest->pActiveFilter");
			pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
			if (pQuest->pActiveFilter(pQuest, nNpcClass, hPlayer, pRecord) == 1)
			{
				tMsg.bCmd = S2C_UNIT_PING;
				tMsg.field_01 = 1;
				tMsg.dwUnitId = hNPC->dwId;
				SCmdSendNpcActive(SUnitGetClient(hPlayer), (const struct SCMDCOPY6 *)&tMsg);
				return TRUE;
			}
		}
		else
		{
#line 1938
			D2ASSERT(pQuest->pActiveFilter != NULL);
		}
	}
	return FALSE;
}

void __fastcall QuestsSetRecordBit(GAME *ptGame  , int nQuest,
	int nBit)
{
	QUESTHEAD *pQuestHead;

	pQuestHead = ptGame->pQuestHead;
#line 1951
	D2ASSERT(pQuestHead->bPickedSet);
	D2CommonQuestRecordSetBit(pQuestHead->pQuestRecord, nQuest, nBit);
}

DWORD __fastcall QuestsGetRecordBit(GAME *ptGame  , int nQuest,
	int nBit)
{
	QUESTHEAD *pQuestHead;

	pQuestHead = ptGame->pQuestHead;
#line 1963
	D2ASSERT(pQuestHead->bPickedSet);
	return D2CommonStepTestGridBit(pQuestHead->pQuestRecord, nQuest, nBit);
}

void __fastcall QuestsObjectSpawned(GAME *ptGame  ,
	GAMEROOM *hRoom, int nX, int nY,
	int nClass)
{
	if (nClass == 331)
	{
		void __fastcall A2Q4ObjectSpawned(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY);

		A2Q4ObjectSpawned(ptGame, hRoom, nX, nY);
		return;
	}
	if (nClass == 201)
	{
		void __fastcall A2Q4ObjectSpawned(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY);

		A2Q4ObjectSpawned(ptGame, hRoom, nX, nY);
	}
}

void __fastcall QuestsUseItem(GAME *ptGame  ,
	UNIT *hUnit  , DWORD dwItemId)
{
	UNIT *hItem;
	ITEMSTXT *ptItemStats;

	hItem = SUnitFind(ptGame, 4, dwItemId);
	if (!hItem)
		return;
	if (!ITEMSIsInInventory(hUnit, hItem, NULL))
		return;
	ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
	if (!ptItemStats)
		return;
	if (ptItemStats->code == QUEST_CODE_BKD)
		A1Q4SendOrder(ptGame, hUnit);
	else if (ptItemStats->code == QUEST_CODE_TRS)
		A2Q6SendLevel(ptGame, hUnit);
}

void __fastcall QuestsInitJerhynMonster(OBJINIT *ptInit  )
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptInit->ptGame, 11);
	if (pQuest)
		A2Q4ObjInitMonster(pQuest, ptInit);
}

void __fastcall QuestsInitJerhynNpc(OBJINIT *ptInit  )
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptInit->ptGame, 11);
	if (pQuest)
		A2Q4ObjInitNpc(pQuest, ptInit);
}

void __fastcall QuestsInitTaintedSunAltar(OBJINIT *ptInit  )
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptInit->ptGame, 10);
	if (pQuest)
	{
		A2Q3ObjInit(pQuest, ptInit);
		return;
	}
	if (ptInit->hObject->dwMode != 2)
		D2CommonUnitSetMode(ptInit->hObject, 2);
}

void __fastcall QuestsInitMalus(OBJINIT *ptInit  )
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptInit->ptGame, 3);
	if (pQuest)
	{
		A1Q3ObjInit(pQuest, ptInit);
		return;
	}
	if (ptInit->hObject->dwMode != 2)
		D2CommonUnitSetMode(ptInit->hObject, 2);
}

void __fastcall QuestsInitGibbet(OBJINIT *ptInit  )
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptInit->ptGame, 4);
	if (pQuest)
	{
		A1Q4CairnObjInit(pQuest, ptInit);
		return;
	}
	if (ptInit->hObject->dwMode != 2)
		D2CommonUnitSetMode(ptInit->hObject, 2);
}

void __cdecl QuestsClearGate(void)
{
	sgbQuestGate = 0;
}

void __fastcall QuestsObjectOperate(GAME *ptGame  , UNIT *hObject
)
{
	QUEST *pQuest;
	GAMEROOM *hRoom;
	int nLevelId;

	switch ((int)hObject->dwClassId)
	{
	case 131:
		hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hObject);
		if (!hRoom)
			break;
		nLevelId = (int)D2CommonDungeonRoomGetLevelId(hRoom);
		switch (nLevelId)
		{
		case 76:
			MonsterSpawnNear(ptGame, hObject, 301, 1, -1, 0);
			break;
		case 108:
			A4Q3ObjectEvent(ptGame, hObject);
			break;
		}
		break;
	case 26:
		A1Q4ObjectEvent(ptGame, hObject);
		break;
	case 122:
		pQuest = QuestsFind(ptGame, 11);
		if (pQuest)
			A2Q4OperateObject(pQuest, hObject);
		break;
	case 189:
		pQuest = QuestsFind(ptGame, 4);
		if (pQuest)
			A1Q4OperateEvent(pQuest, hObject);
		break;
	case 268:
		pQuest = QuestsFind(ptGame, 4);
		if (pQuest)
			A1Q4OperateDrop(pQuest, hObject);
		break;
	case 367:
		pQuest = QuestsFind(ptGame, 16);
		if (pQuest)
			A3Q2ObjectOperated(pQuest, hObject);
		break;
	case 341:
		pQuest = QuestsFind(ptGame, 20);
		if (pQuest)
			A3Q6ObjectOperated(pQuest, hObject);
		break;
	case 376:
		pQuest = QuestsFind(ptGame, 24);
		if (pQuest)
			QuestsFXDropGems(pQuest, hObject);
		break;
	case 371:
		pQuest = QuestsFind(ptGame, 5);
		if (pQuest)
			A1Q5ObjectEvent(pQuest, hObject);
		break;
	}
}

void __fastcall QuestsInitLamTome(OBJINIT *ptInit  )
{
	QUEST *pQuestInfo;

	pQuestInfo = QuestsFind(ptInit->ptGame, 15);
	if (pQuestInfo)
	{
		A3Q1ObjectMode(pQuestInfo, ptInit);
		return;
	}
#line 2250
	D2ASSERT(pQuestInfo);
	A3Q1ObjectMode(pQuestInfo, ptInit);
}

void __fastcall QuestsOfferFigurineMonster(GAME *ptGame  ,
	UNIT *hMonster)
{
	QUEST *pQuest;
	MONSTATSTXT *ptMonsterStats;

	if (D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hMonster))) != 2)
		return;
	pQuest = QuestsFind(ptGame, 18);
	if (!pQuest)
		return;
	if (pQuest->bActive != 1)
		return;
	if (hMonster->dwClassId == 407)
		return;
	ptMonsterStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(hMonster->dwClassId);
	if (ptMonsterStats->bFlying)
		return;
	A3Q4MonsterAct(pQuest, hMonster);
}

void __fastcall QuestsReleasePickedMonster(GAME *ptGame  ,
	UNIT *hUnit  )
{
	QUESTLINK *pHead;
	QUEST *pQuest;

	pHead = hUnit->field_94;
	if (!pHead)
		return;
	if (D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hUnit))) != 2)
		return;
	pQuest = pHead->pQuest;
	if (pQuest->eFilter != 20)
		return;
	if (pQuest->bCallbacksEnabled != 1)
		return;
	A3Q4UnitAct(pQuest, hUnit);
}

UNIT * __fastcall QuestsCreateItem(GAME *ptGame  , UNIT *hUnit
, DWORD dwCode, int nLevel, DWORD dwQuality)
{
	UNIT *hItem;
	GAMEROOM *hRoom;
	GAMECOORD tCoord;
	DWORD dwClassId;
	int nUnitLevel;

	if (D2CommonItemTblsFindItemByCode(dwCode, &dwClassId))
	{
		nUnitLevel = ITEMSGetUnitLevel(hUnit, LEVEL_ID_NONE);
		if (nLevel)
			nUnitLevel = nLevel;
		hItem = ITEMSCreate(hUnit, dwClassId, ptGame, 4, dwQuality, 0, nUnitLevel, FALSE, 0, 0);
		if (hItem)
		{
			D2CommonITEMSDataSetPage(hItem, 0);
			if (!ITEMSModePlaceCursorItem(ptGame, hUnit, hItem->dwId, 0, 0, TRUE, TRUE, NULL))
			{
				D2CommonUnitGetPosition(hUnit, &tCoord);
				QuestsFindSpot((GAMEROOM *)D2CommonUnitGetPathRoom(hUnit), &tCoord, 1, 7681, &hRoom);
				if (hRoom)
				{
					D2CommonUnitSetMode(hItem, 3);
					D2CommonUnitInitPosition(0, hItem, hRoom, tCoord.dwX, tCoord.dwY);
					D2CommonUnitSetCollision(hItem, hRoom, tCoord.dwX, tCoord.dwY);
					D2CommonAddUnitToRoom(hItem, hRoom);
					hItem->dwFlags0 |= 4098;
					D2CommonUnitSetMode(hItem, 3);
				hItem->dwFlags0 |= 33554432;
				}
			}
			else if (!D2CommonITEMSDataTestFlags(hItem, 16))
				ITEMSModeMarkHandItem(hUnit, hItem);
			return hItem;
		}
	}
	return NULL;
}

void __fastcall QuestsSendIntroduced(GAME *ptGame  , UNIT *hUnit
, BYTE bAct)
{
	PLRINTRO *ptIntro;
	QUESTINTROMSG tMsg;
	const int *pnNpc;
	WORD *pwNpc;
	BOOL fAny;
	int i;

	ptIntro = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->ptIntro[ptGame->bDifficultyLevel];
	switch (bAct)
	{
	case 0:
		fAny = FALSE;
		for (i = 0; i < (int)(sizeof(tMsg.awNpcs) / (sizeof(DWORD))); i++)
			((DWORD *)tMsg.awNpcs)[i] = 26870170;
		pwNpc = tMsg.awNpcs;
		for (pnNpc = sgnQuestNpcsAct2; (int)pnNpc < (int)&sgnQuestNpcsAct2[arrsize(sgnQuestNpcsAct2)]; pnNpc++)
		{
			if (PlrIntroTestIntroduced(ptIntro, *pnNpc))
			{
				fAny = TRUE;
				*pwNpc++ = (WORD)*pnNpc;
			}
		}
		break;
	case 1:
		fAny = FALSE;
		for (i = 0; i < (int)(sizeof(tMsg.awNpcs) / (sizeof(DWORD))); i++)
			((DWORD *)tMsg.awNpcs)[i] = 26870170;
		pwNpc = tMsg.awNpcs;
		for (pnNpc = sgnQuestNpcsAct3; (int)pnNpc < (int)&sgnQuestNpcsAct3[arrsize(sgnQuestNpcsAct3)]; pnNpc++)
		{
			if (PlrIntroTestIntroduced(ptIntro, *pnNpc))
			{
				fAny = TRUE;
				*pwNpc++ = (WORD)*pnNpc;
			}
		}
		break;
	case 2:
		fAny = FALSE;
		for (i = 0; i < (int)(sizeof(tMsg.awNpcs) / (sizeof(DWORD))); i++)
			((DWORD *)tMsg.awNpcs)[i] = 26870170;
		pwNpc = tMsg.awNpcs;
		for (pnNpc = sgnQuestNpcsAct4; (int)pnNpc < (int)&sgnQuestNpcsAct4[arrsize(sgnQuestNpcsAct4)]; pnNpc++)
		{
			if (PlrIntroTestIntroduced(ptIntro, *pnNpc))
			{
				fAny = TRUE;
				*pwNpc++ = (WORD)*pnNpc;
			}
		}
		break;
	default:
		return;
	}
	if (fAny)
	{
		tMsg.bCmd = 132;
		tMsg.bAct = bAct;
		SCmdSendNpcIntroduced(SUnitGetClient(hUnit), (const struct SCMDCOPY26 *)&tMsg);
	}
}

void __fastcall QuestsChangedLevel(GAME *ptGame  , UNIT *hUnit
, QUESTLEVELMSG *ptMsg, DWORD dwLevelNew,
	DWORD dwLevelOld)
{

	QUEST *pQuestInfo;
	PLRINFO *ptInfo;
	PLRINTRO *ptIntro;
	CLIENT *ptClient;
	void *pRecord;
	int i;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (ptMsg->eType != 1)
		return;
	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
	switch (ptMsg->dwClassId)
	{
	case 155:
		if (!D2CommonStepTestGridBit(pRecord, 7, 0))
		{
			D2CommonQuestRecordSetBit(pRecord, 7, 0);
			D2CommonQuestRecordSetBit(pRecord, 7, 13);
			QuestsSendPlayerRecord(ptGame, hUnit);
			ptClient = SUnitGetClient(hUnit);
			if (ptInfo->bBusy != 1)
			{
				ptInfo->bBusy = 1;
				SCmdSendVideo(ptClient, 2);
			}
			ptIntro = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->ptIntro[ptGame->bDifficultyLevel];
			for (i = 0; i < (int)arrsize(sgnQuestNpcsAct2); i++)
				PlrIntroSetIntroduced(ptIntro, sgnQuestNpcsAct2[i]);
		}
		if (D2CommonStepTestGridBit(pRecord, 6, 13) == 1
			&& D2CommonStepTestGridBit(pRecord, 6, 0) == 1)
		{
			pQuestInfo = QuestsFind(ptGame, 6);
			if (pQuestInfo && pQuestInfo->bActive)
			{ QUESTPARAM tParam;
				tParam.ptGame = ptGame;
				tParam.eCallback = QUESTCALLBACK_CHANGEDLEVEL;
				tParam.field_14 = 1;
				tParam.field_18 = 40;
				tParam.hPlayer = hUnit;
				if (pQuestInfo->pCallback[QUESTCALLBACK_CHANGEDLEVEL])
				{
#line 2525
					D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pCallback[QUESTCALLBACK_CHANGEDLEVEL]), "pQuestInfo->pCallback[QUESTCALLBACK_CHANGEDLEVEL]");
					pQuestInfo->pCallback[QUESTCALLBACK_CHANGEDLEVEL](pQuestInfo, &tParam);
				}
			}
		}
		A1Q4EnteredLevel(ptGame, hUnit);
		break;
	case 210:
		if (!D2CommonStepTestGridBit(pRecord, 15, 0))
		{
			D2CommonQuestRecordSetBit(pRecord, 15, 0);
			D2CommonQuestRecordSetBit(pRecord, 15, 13);
			ptIntro = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->ptIntro[ptGame->bDifficultyLevel];
			for (i = 0; i < (int)arrsize(sgnQuestNpcsAct3); i++)
				PlrIntroSetIntroduced(ptIntro, sgnQuestNpcsAct3[i]);
			QuestsSendPlayerRecord(ptGame, hUnit);
			ptClient = SUnitGetClient(hUnit);
			if (ptInfo->bBusy != 1)
			{
				ptInfo->bBusy = 1;
				SCmdSendVideo(ptClient, 3);
			}
		}
		break;
	}
}

void __fastcall QuestsObjectLevel(GAME *ptGame  , UNIT *hPlayer
, DWORD dwLevelId, UNIT *hObject  )
{
	PLRINFO *ptInfo;
	PLRINTRO *ptIntro;
	CLIENT *ptClient;
	void *pRecord;
	int i;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (dwLevelId == 102)
	{
		if (D2CommonStepTestGridBit(pRecord, 23, 0))
			return;
		D2CommonQuestRecordSetBit(pRecord, 23, 0);
		D2CommonQuestRecordSetBit(pRecord, 23, 13);
		ptIntro = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->ptIntro[ptGame->bDifficultyLevel];
		for (i = 0; i < (int)arrsize(sgnQuestNpcsAct4); i++)
			PlrIntroSetIntroduced(ptIntro, sgnQuestNpcsAct4[i]);
		QuestsSendPlayerRecord(ptGame, hPlayer);
		ptClient = SUnitGetClient(hPlayer);
		if (ptInfo->bBusy != 1)
		{
			ptInfo->bBusy = 1;
			SCmdSendVideo(ptClient, 4);
		}
	}
	else
		A3Q7ObjectsDone(ptGame, hObject);
}

void __fastcall QuestsPlayersAdd(QUESTPLAYERS *pPlayers, DWORD dwUnitId)
{
	WORD nCount;
	int i;

	nCount = pPlayers->wCount;
	if (nCount >= 32)
		return;
	for (i = 0; i < nCount; i++)
	{
		if (pPlayers->adwUnitIds[i] == dwUnitId)
			return;
	}
	pPlayers->adwUnitIds[nCount] = dwUnitId;
	pPlayers->wCount++;
}

BOOL __fastcall QuestsPlayersRemove(QUESTPLAYERS *pPlayers, DWORD dwUnitId)
{
	int nCount;
	int i;

	nCount = pPlayers->wCount;
	for (i = 0; i < nCount; i++)
	{
		if (pPlayers->adwUnitIds[i] == dwUnitId)
		{
			if (i != nCount - 1)
				pPlayers->adwUnitIds[i] = pPlayers->adwUnitIds[nCount - 1];
			pPlayers->wCount--;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL __fastcall QuestsPlayersFind(QUESTPLAYERS *pPlayers, DWORD dwUnitId)
{
	int nCount;
	int i;

	nCount = pPlayers->wCount;
	for (i = 0; i < nCount; i++)
	{
		if (pPlayers->adwUnitIds[i] == dwUnitId)
			return TRUE;
	}
	return FALSE;
}

BOOL __fastcall QuestsHasPlayer(QUEST *pQuest  , DWORD dwUnitId)
{
	int nCount;
	int i;

	nCount = pQuest->tPlayers.wCount;
	for (i = 0; i < nCount; i++)
	{
		if (pQuest->tPlayers.adwUnitIds[i] == dwUnitId)
			return TRUE;
	}
	return FALSE;
}

void __fastcall QuestsPlayersClear(QUESTPLAYERS *pPlayers)
{
	pPlayers->wCount = 0;
}

void __fastcall QuestsPlayerLeft(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	if (pQuest->tPlayers.wCount > 0)
		QuestsPlayersRemove(&pQuest->tPlayers, pqParam->hPlayer->dwId);
}

void __fastcall QuestsFindSpot(GAMEROOM *hRoom, GAMECOORD *ptCoord,
	int nSize, DWORD dwFlags, GAMEROOM **phRoom)
{
	GAMEROOM *hRow;
	GAMEROOM *hCell;
	GAMERECT atRect[2];
	GAMECOORD tStart;
	int nMax;
	int nRow;
	int nCol;
	int nY;
	int nX;

	tStart = *ptCoord;
	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	for (nMax = 1; nMax < 100; nMax++)
	{
		for (nRow = nMax; nRow > -nMax; nRow -= 2)
		{
			nY = tStart.dwY + nRow;
			ptCoord->dwY = nY;
			hRow = hRoom;
			if (nY < atRect[0].nY || nY >= atRect[0].nY + atRect[0].nHeight)
				hRow = (GAMEROOM *)D2CommonDungeonFindRoomGame(hRoom, ptCoord->dwX, nY);
			if (!hRow)
				continue;
			for (nCol = -nMax; nCol < nMax; nCol += 2)
			{
				nX = tStart.dwX + nCol;
				hCell = hRow;
				D2CommonDungeonRoomCopyRects(hRow, atRect);
				ptCoord->dwX = nX;
				if (nX < atRect[0].nX || nX >= atRect[0].nX + atRect[0].nWidth)
					hCell = (GAMEROOM *)D2CommonDungeonFindRoomGame(hRow, nX, ptCoord->dwY);
				if (!hCell)
					continue;
				if (D2CommonCollisnTestArea(hCell, ptCoord->dwX, ptCoord->dwY, nSize * 2 + 1,
					nSize * 2 + 1, dwFlags))
					continue;
				if (ptCoord->dwX >= (DWORD)atRect[0].nX
					&& ptCoord->dwX < (DWORD)(atRect[0].nX + atRect[0].nWidth)
					&& ptCoord->dwY >= (DWORD)atRect[0].nY
					&& ptCoord->dwY < (DWORD)(atRect[0].nY + atRect[0].nHeight))
				{
					*phRoom = hCell;
					return;
				}
			}
		}
	}
	*phRoom = NULL;
}

void __fastcall QuestsPlayerDone(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	GAME *ptGame;
	void *pRecord;

	ptGame = pQuest->ptGame;
	pRecord
		= ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 1) != 1)
		return;
	if (!D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0))
		return;
	if (pQuest->tPlayers.wCount > 0)
		QuestsPlayersRemove(&pQuest->tPlayers, pqParam->hPlayer->dwId);
}

void __fastcall QuestsSendPlayerRecord(GAME *ptGame  , UNIT *hUnit
)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	SCmdSendQuests(SUnitGetClient(hUnit), S2C_NPC_DIALOG, 6, 0, pRecord, 0);
}

void __fastcall QuestsPlayersSetStatus(GAME *ptGame  , QUESTPLAYERS *pPlayers
, int nQuest, DWORD dwEvent)
{
	UNIT *hPlayer;
	void *pRecord;
	int i;

	for (i = 0; i < pPlayers->wCount; i++)
	{
		hPlayer = SUnitFind(ptGame, UNIT_PLAYER, pPlayers->adwUnitIds[i]);
		if (!hPlayer)
			continue;
		pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
		if (D2CommonStepTestGridBit(pRecord, nQuest, 0) == 1)
			continue;
		if (D2CommonStepTestGridBit(pRecord, nQuest, 1) == 1)
			continue;
		D2CommonQuestRecordSetBit(pRecord, nQuest, 13);
		D2CommonQuestRecordSetBit(pRecord, nQuest, 1);
		if (dwEvent)
			SUnitSetEvent(hPlayer, dwEvent, hPlayer);
	}
}

void __fastcall QuestsSendUnitMessage(UNIT *hPlayer  , UNIT *hUnit
, WORD wMessage)
{
	CLIENT *ptClient;
	QUESTSYSMSG tMsg;

	ptClient = SUnitGetClient(hPlayer);
	tMsg.field_02 = hUnit->dwId;
	tMsg.field_0a = wMessage;
	tMsg.bCmd = 32;
	tMsg.field_01 = 2;
	tMsg.field_08 = 0;
	tMsg.field_06 = 1;
	SCmdSendUnitMessage(ptClient, (const SCMDSYSTEMMSG *)&tMsg);
}

void __fastcall QuestsSetAll(GAME *ptGame  , BYTE bValue)
{
	ptGame->pQuestHead->bTrigger = bValue;
	SUnitIterate(ptGame, UNIT_PLAYER, NULL, QuestsSendAllCb);
}

BOOL __fastcall QuestsSendAllCb(GAME *ptGame  ,
	UNIT *hUnit  , void *pArg)
{
	QUESTFLAGMSG tFlag;

	tFlag.bCmd = 124;
	tFlag.bValue = ptGame->pQuestHead->bTrigger;
	QuestsSendPlayerRecord(ptGame, hUnit);
	SCmdSendQuestTrigger(SUnitGetClient(hUnit), &tFlag);
	return FALSE;
}

void __fastcall QuestsTextMessage(GAME *ptGame  , UNIT *hUnit
, UNIT *hNpc)
{
	QUESTPARAM tParam;
	QUESTSYSMSG tMsg;
	CLIENT *ptClient;
	void *pText;

	pText = (void *)D2CommonTextListNew(ptGame->pMemPool);
	ptClient = SUnitGetClient(hUnit);
	tParam.ptGame = ClientGetGame(ptClient);
	tParam.hEventUnit = hNpc;
	tParam.eCallback = 0;
	tParam.hPlayer = hUnit;
	tParam.field_14 = (DWORD)pText;
	QuestsCallback(&tParam, TRUE, TRUE);
	tMsg.bCmd = 32;
	tMsg.field_02 = hNpc->dwId;
	tMsg.field_01 = (BYTE)hNpc->eType;
	D2CommonTextListPack(pText, &tMsg.field_06);
	SCmdSendUnitMessage(ptClient, (const SCMDSYSTEMMSG *)&tMsg);
	D2CommonTextListFree(pText);
	QuestsSendRecord(ptGame, ptClient);
}

BOOL __fastcall QuestsPortalSpot(GAME *ptGame  ,
	GAMECOORD *ptCoord, int nType, GAMEROOM **phRoom)
{
	if (nType == 73)
		return A2Q6PortalSpot(ptGame, ptCoord, phRoom);
	return FALSE;
}

BOOL __fastcall QuestsOperateObject(OBJOPERATE *pObjectParam  )
{
	MISSOBJECTSTXT *ptObjectTxt;
	GAMECOORD tCoord;

	if (!pObjectParam->hObject->dwMode)
	{
		D2CommonUnitGetPosition(pObjectParam->hObject, &tCoord);
		ptObjectTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
		if (ptObjectTxt->Mode[1])
		{
			D2CommonUnitSetMode(pObjectParam->hObject, 1);
			EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
				(int)(((DWORD)ptObjectTxt->FrameCnt[1] >> 8) + pObjectParam->ptGame->nFrame));
		}
		else
			D2CommonUnitSetMode(pObjectParam->hObject, 2);
		pObjectParam->hObject->dwFlags0 &= ~2u;
		return TRUE;
	}
	return FALSE;
}

void __fastcall QuestsSendFlag(UNIT *hUnit  , DWORD bQuest)
{
	QUESTSTATEMSG tMsg;

	tMsg.bCmd = 86;
	tMsg.field_02 = 2;
	tMsg.bQuest = bQuest;
	SCmdSendQuestStatus(SUnitGetClient(hUnit), &tMsg);
}

void __fastcall QuestsSendFlagInAct(UNIT *hUnit  , DWORD bQuest,
	DWORD bAct)
{
	QUESTSTATEMSG tMsg;
	GAMEROOM *hRoom;
	int nLevelId;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (hRoom)
	{
		nLevelId = (int)D2CommonDungeonRoomGetLevelId(hRoom);
		if (!nLevelId)
			return;
		if (D2CommonGetActNum(nLevelId) < (BYTE)bAct)
			return;
	}
	tMsg.bCmd = 86;
	tMsg.field_02 = 0;
	tMsg.bQuest = bQuest;
	SCmdSendQuestStatus(SUnitGetClient(hUnit), &tMsg);
}

void __fastcall QuestsDropItem(QUEST *pQuest  , QUESTPARAM *pqParam
, int nTreasureClass)
{
	void *ptTreasureClass;
	UNIT *hItem;
	DWORD dwCode;
	DWORD dwQuality;
	DWORD dwRoll;
	int nLevel;
	int nTries;

	dwQuality = 5;
	dwRoll = D2CommonSeedRand(QuestsGetSeed(pQuest->ptGame), 100);
	if (dwRoll < 60)
		dwQuality = 6;
	else if (dwRoll < 90)
		dwQuality = 7;
	ptTreasureClass = (void *)D2CommonMonsterTblsGetTreasureClass(nTreasureClass);
	if (!ptTreasureClass)
		return;
	dwCode = ITEMSModeDrawCode(pQuest->ptGame, (struct ITEMCODELIST *)ptTreasureClass, pqParam->hEventUnit,
		TRUE);
	if (!dwCode)
		dwCode = QUEST_CODE_RING;
	pqParam->hEventUnit->dwDropItemCode = dwCode;
	nTries = 0;
	nLevel = 0;
	while (nTries < 5)
	{
		hItem = ITEMSDropFromUnit(pQuest->ptGame, pqParam->hEventUnit, dwQuality, &nLevel, NULL, 44, FALSE);
		if (hItem)
		{
			nTries++;
			if (D2CommonITEMSDataGetModType(hItem) == dwQuality)
			{
				pqParam->hEventUnit->dwDropItemCode = 0;
				return;
			}
			dwCode = ITEMSModeDrawCode(pQuest->ptGame, (struct ITEMCODELIST *)ptTreasureClass,
				pqParam->hEventUnit, TRUE);
			if (!dwCode)
				dwCode = QUEST_CODE_RING;
			pqParam->hEventUnit->dwDropItemCode = dwCode;
		}
	}
	pqParam->hEventUnit->dwDropItemCode = 0;
}
