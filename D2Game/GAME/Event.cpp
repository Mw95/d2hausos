#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\GAME\\Event.cpp"
#include "D2Macros.h"
#include <stdio.h>
#define D2_EVENT_CPP
#include "Event.h"
#include "Game.h"
#include "../ITEMS/ItemMode.h"
#include "../ITEMS/Items.h"
#include "../MISSILES/MissMode.h"
#include "../MONSTER/MonsterAI.h"
#include "../MONSTER/MonsterMode.h"
#include "../OBJECTS/ObjMode.h"
#include "../PLAYER/PlrModes.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "FogImports.h"

#define NUM_EVENTS				13
#define EVENT_LIST_NONE			(-1)
#define EVENT_UNREGISTERED_FLAG	2
#define EVENT_RUN_AI			2
#define UNIT_STATE_UNINTERRUPTABLE	54
#define UNIT_MONSTER			1
#define UNIT_OBJECT				2
#define UNIT_MISSILE			3
#define UNIT_ITEM				4
#define StatsGetState(hUnit, nState) \
	D2CommonStatsGetStateRaw((DWORD)(hUnit), (DWORD)(nState))

#define EVENT_FLAG_EXPIRE		1
#define EVENT_FLAG_PERSISTENT	4
#define EVENT_FRAMES			64
#define EVENT_LISTS				5
#define EVENTS_PER_HEADER		600
#define EVENT_LIST_PLAYER		0
#define EVENT_LIST_MONSTER		1
#define EVENT_LIST_MISSILE		2
#define EVENT_LIST_OBJECT		3
#define EVENT_LIST_ITEM			4

struct EVENT
{
 	int			eEventType;
 	DWORD		dwFlags;
 	int			nFrame;
 	UNIT		*hUnit;
 	DWORD		dwUnitId;
 	int			eUnitType;
 	struct EVENT	*ptNext;
 	struct EVENT	*ptPrev;
 	struct EVENT	*ptUnitNext;
 	struct EVENT	*ptUnitPrev;
};
typedef struct EVENT EVENT;

struct EVENTHEADER
{
 	EVENT		atEvents[EVENTS_PER_HEADER];
 	EVENT		*ptFree;
 	struct EVENTHEADER	*ptNext;
};
typedef struct EVENTHEADER EVENTHEADER;

struct GAMEEVENTLIST
{
 	int			nSlot;
 	EVENT		*aptFrame[EVENT_LISTS][EVENT_FRAMES];
 	EVENT		*aptPersistent[EVENT_LISTS];
 	EVENT		*ptCurrent;
 	EVENTHEADER	tHeader;
};
typedef struct GAMEEVENTLIST GAMEEVENTLIST;

static const int sgnEventListIndex[6] =
{
	EVENT_LIST_PLAYER, EVENT_LIST_MONSTER, EVENT_LIST_OBJECT, EVENT_LIST_MISSILE, EVENT_LIST_ITEM,
	EVENT_LIST_NONE,
};

void __fastcall ClientQueueAppend(CLIENT *ptClient, CLIENTNODE178 *ptNode)
{
	CLIENTNODE178 *ptTail;

	ptNode->ptNext = NULL;
	ptTail = (CLIENTNODE178 *)ptClient->dwQueueTail;
	if (!ptTail)
	{
		ptClient->dwQueueHead = (DWORD)ptNode;
		ptClient->dwQueueTail = (DWORD)ptNode;
		return;
	}
	ptTail->ptNext = ptNode;
	ptClient->dwQueueTail = (DWORD)ptNode;
}

void __fastcall ClientQueuePushFront(CLIENT *ptClient, CLIENTNODE178 *ptNode)
{
	ptNode->ptNext = NULL;
	if (!ptClient->dwQueueHead)
	{
		ptNode->ptNext = (CLIENTNODE178 *)ptClient->dwQueueHead;
		ptClient->dwQueueHead = (DWORD)ptNode;
		return;
	}
	ptClient->dwQueueHead = (DWORD)ptNode;
	ptClient->dwQueueTail = (DWORD)ptNode;
}

CLIENTNODE178 * __fastcall ClientQueuePop(CLIENT *ptClient)
{
	CLIENTNODE178 *ptNode;

	ptNode = (CLIENTNODE178 *)ptClient->dwQueueHead;
	if ((CLIENTNODE178 *)ptClient->dwQueueTail == ptNode)
	{
		ptClient->dwQueueTail = 0;
		ptClient->dwQueueHead = 0;
	}
	if (ptNode)
		ptClient->dwQueueHead = (DWORD)ptNode->ptNext;
	return ptNode;
}

CLIENTNODE178 * __fastcall ClientGetQueueTail(CLIENT *ptClient)
{
	return (CLIENTNODE178 *)ptClient->dwQueueTail;
}

CLIENTNODE178 * __fastcall ClientGetQueueHead(CLIENT *ptClient)
{
	return (CLIENTNODE178 *)ptClient->dwQueueHead;
}

void __fastcall ClientGetAccount(CLIENT *ptClient, char *szOut)
{
	StormSStrCopy(szOut, ptClient->szAccount, 16);
}

void __fastcall ClientGetTokenResult(CLIENT *ptClient, DWORD *pdwOut)
{
	*pdwOut = ptClient->dwTokenResult;
}

static inline void EventHeaderInit(EVENTHEADER *ptHeaderNew  )
{
	EVENT *ptEvent;
	int i;

	D2_OPAQUE(ptHeaderNew);
#line 84
	D2ASSERT(ptHeaderNew);
	for (i = 0; i < (int)(sizeof(EVENTHEADER) / sizeof(DWORD)); i++)
		((DWORD *)ptHeaderNew)[i] = 0;
	ptEvent = ptHeaderNew->atEvents;
	for (i = 0; i < EVENTS_PER_HEADER - 1; i++)
	{
		ptEvent->ptNext = ptEvent + 1;
		ptEvent++;
	}
	ptEvent->ptNext = NULL;
	ptHeaderNew->ptFree = ptHeaderNew->atEvents;
}

static inline EVENT *EventListAlloc(GAME *ptGame, GAMEEVENTLIST *ptGameEventList  )
{
	EVENTHEADER *ptHeader;
	EVENT *ptEvent;
	int i;

	D2_OPAQUE(ptGameEventList);
#line 99
	D2ASSERT(ptGameEventList);
	ptHeader = &ptGameEventList->tHeader;
	while (ptHeader && !ptHeader->ptFree)
		ptHeader = ptHeader->ptNext;
	if (!ptHeader)
	{
#line 107
		ptHeader = (EVENTHEADER *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(EVENTHEADER));
		EventHeaderInit(ptHeader);
		ptHeader->ptNext = ptGameEventList->tHeader.ptNext;
		ptGameEventList->tHeader.ptNext = ptHeader;
	}
	ptEvent = ptHeader->ptFree;
	ptHeader->ptFree = ptEvent->ptNext;
	for (i = 0; i < (int)(sizeof(EVENT) / sizeof(DWORD)); i++)
		((DWORD *)ptEvent)[i] = 0;
	return ptEvent;
}

static inline void EventListRelease(GAMEEVENTLIST *ptGameEventList, EVENT *ptEvent)
{
	EVENTHEADER *ptHeader;

#line 124
	D2ASSERT(ptEvent);
	D2_OPAQUE(ptGameEventList);
#line 125
	D2ASSERT(ptGameEventList);
	ptHeader = &ptGameEventList->tHeader;
	D2_OPAQUE(ptHeader);
#line 129
	D2ASSERT(ptHeader);
	ptEvent->ptNext = ptHeader->ptFree;
	ptEvent->ptPrev = NULL;
	ptHeader->ptFree = ptEvent;
	ptEvent->dwFlags = EVENT_UNREGISTERED_FLAG;
}

static inline GAMEEVENTLIST *EventGetList(GAME *ptGame)
{
#line 139
	D2ASSERT(ptGame);
#line 140
	D2ASSERT(ptGame->hEvents);
	return (GAMEEVENTLIST *)ptGame->hEvents;
}

static EVENT * __fastcall EventNew(GAME *ptGame, UNIT *hUnit, int eEventType, int nEventFrame)
{
	GAMEEVENTLIST *ptGameEventList;
	EVENT **hUnitEvents;
	EVENT *ptEvent;

#line 165
	D2ASSERT(eEventType < NUM_EVENTS);
	ptGameEventList = EventGetList(ptGame);
	hUnitEvents = UnitGetEventList(hUnit);
#line 170
	D2ASSERT(hUnitEvents);
	ptEvent = EventListAlloc(ptGame, ptGameEventList);
	ptEvent->hUnit = hUnit;
	ptEvent->eEventType = eEventType;
	ptEvent->ptUnitNext = *hUnitEvents;
	if (*hUnitEvents)
		(*hUnitEvents)->ptUnitPrev = ptEvent;
	ptEvent->ptUnitPrev = NULL;
	*hUnitEvents = ptEvent;
	ptEvent->nFrame = nEventFrame;
	ptEvent->eUnitType = hUnit->eType;
	ptEvent->dwUnitId = hUnit->dwId;
	ptEvent->dwFlags &= ~EVENT_UNREGISTERED_FLAG;
	return ptEvent;
}

static inline EVENT **EventGetSlot(GAME *ptGame, int eUnitType, int nEventFrame)
{
	GAMEEVENTLIST *ptGameEventList;
	BOOL bPersistent;
	int nSlot;
	int nUnitTypeIndex;

	ptGameEventList = EventGetList(ptGame);
	bPersistent = nEventFrame == -1;
	nSlot = 0;
	if (!bPersistent)
	{
#line 201
		D2ASSERT(nEventFrame >= ptGame->nFrame);
		nSlot = nEventFrame % EVENT_FRAMES;
	}
	nUnitTypeIndex = sgnEventListIndex[eUnitType];
#line 206
	D2ASSERT(nUnitTypeIndex != EVENT_LIST_NONE);
	if (bPersistent)
		return &ptGameEventList->aptPersistent[nUnitTypeIndex];
	return &ptGameEventList->aptFrame[nUnitTypeIndex][nSlot];
}

static void __fastcall EventRemove(GAME *ptGame, EVENT *ptEvent)
{
	GAMEEVENTLIST *ptGameEventList;
	EVENT **pptList;
	int eUnitType;
	EVENT **hUnitEvents;

	ptGameEventList = EventGetList(ptGame);
	if (ptEvent == ptGameEventList->ptCurrent)
		ptGameEventList->ptCurrent = ptEvent->ptNext;
	if (ptEvent->ptPrev)
		ptEvent->ptPrev->ptNext = ptEvent->ptNext;
	else
	{
		eUnitType = ptEvent->eUnitType;
		pptList = EventGetSlot(ptGame, eUnitType, ptEvent->nFrame);
		*pptList = ptEvent->ptNext;
		if (*pptList)
			(*pptList)->ptPrev = NULL;
	}
	if (ptEvent->ptNext)
		ptEvent->ptNext->ptPrev = ptEvent->ptPrev;
	ptEvent->ptPrev = NULL;
	ptEvent->ptNext = NULL;
	if (ptEvent->hUnit)
	{
		hUnitEvents = UnitGetEventList(ptEvent->hUnit);
#line 245
		D2ASSERT(hUnitEvents);
		if (ptEvent->ptUnitNext)
			ptEvent->ptUnitNext->ptUnitPrev = ptEvent->ptUnitPrev;
		if (ptEvent->ptUnitPrev)
			ptEvent->ptUnitPrev->ptUnitNext = ptEvent->ptUnitNext;
		else
			*hUnitEvents = ptEvent->ptUnitNext;
	}
	EventListRelease(ptGameEventList, ptEvent);
}

void __fastcall EventRemoveByType(GAME *ptGame, UNIT *hUnit, int eEventType)
{
	EVENT **hUnitEvents;
	EVENT *ptEvent;
	EVENT *ptNext;

	hUnitEvents = UnitGetEventList(hUnit);
	if (!hUnitEvents)
		return;
	for (ptEvent = *hUnitEvents; ptEvent; ptEvent = ptNext)
	{
		ptNext = ptEvent->ptUnitNext;
#line 271
		D2ASSERT(ptEvent->hUnit == hUnit);
		if (ptEvent->eEventType == eEventType)
			EventRemove(ptGame, ptEvent);
	}
}

EVENT * __fastcall EventAdd(GAME *ptGame, UNIT *hUnit, int eEventType, int nEventFrame)
{
	EVENT *ptEvent;
	EVENT **pptList;
	EVENT *ptHead;

	if (nEventFrame <= ptGame->nFrame)
		nEventFrame = ptGame->nFrame + 1;
#line 302
	D2WARN(! ((UnitGetType (hUnit) == UNIT_MONSTER) &&(eEventType == EVENT_RUN_AI)
		&& StatsGetState (hUnit, UNIT_STATE_UNINTERRUPTABLE)));
	ptEvent = EventNew(ptGame, hUnit, eEventType, nEventFrame);
	pptList = EventGetSlot(ptGame, hUnit->eType, nEventFrame);
	ptHead = *pptList;
	ptEvent->ptNext = ptHead;
	ptEvent->ptPrev = NULL;
	if (ptHead)
		ptHead->ptPrev = ptEvent;
	*pptList = ptEvent;
	return ptEvent;
}

EVENT * __fastcall EventAddPersistent(GAME *ptGame, UNIT *hUnit, int eEventType)
{
	EVENT *ptEvent;
	EVENT **pptList;
	EVENT *ptHead;

	ptEvent = EventNew(ptGame, hUnit, eEventType, -1);
	pptList = EventGetSlot(ptGame, hUnit->eType, -1);
	ptEvent->dwFlags |= EVENT_FLAG_PERSISTENT;
	ptHead = *pptList;
	ptEvent->ptNext = ptHead;
	ptEvent->ptPrev = NULL;
	if (ptHead)
		ptHead->ptPrev = ptEvent;
	*pptList = ptEvent;
	return ptEvent;
}

void __fastcall EventUnitInit(GAME *ptGame, UNIT *hUnit)
{
	EVENT **hUnitEvents;

#line 337
	D2ASSERT(! UnitGetEventList ( hUnit ));
#line 339
	hUnitEvents = (EVENT **)D2ALLOCPOOL(ptGame->pMemPool, sizeof(EVENT *));
	*hUnitEvents = NULL;
	UnitSetEventList(hUnit, hUnitEvents);
}

void __fastcall EventUnitFree(GAME *ptGame, UNIT *hUnit)
{
	EVENT **ptUnitEvents;
	EVENT *ptEvent;
	EVENT *ptNext;

	ptUnitEvents = UnitGetEventList(hUnit);
#line 350
	D2ASSERT(ptUnitEvents);
	for (ptEvent = *ptUnitEvents; ptEvent; ptEvent = ptNext)
	{
		ptNext = ptEvent->ptUnitNext;
		EventRemove(ptGame, ptEvent);
		ptEvent->hUnit = NULL;
	}
#line 361
	D2FREEPOOL(ptGame->pMemPool, ptUnitEvents);
	UnitSetEventList(hUnit, NULL);
}

static void __fastcall EventRunPlayers(GAME *ptGame, GAMEEVENTLIST *ptGameEventList, EVENT *ptFirst,
	BOOL bFrame)
{
	EVENT *ptEvent;

	ptGameEventList->ptCurrent = NULL;
	if (bFrame)
	{
		for (ptEvent = ptFirst; ptEvent; ptEvent = ptGameEventList->ptCurrent)
		{
			ptGameEventList->ptCurrent = ptEvent->ptNext;
			if (ptEvent->nFrame != ptGame->nFrame)
				continue;
			ptEvent->dwFlags |= EVENT_FLAG_EXPIRE;
#line 388
			D2ASSERT(!(ptEvent->dwFlags & EVENT_UNREGISTERED_FLAG));
			PlrModeEvent(ptGame, ptEvent->hUnit, ptEvent->eEventType);
			if (ptEvent->dwFlags & EVENT_FLAG_EXPIRE)
				EventRemove(ptGame, ptEvent);
		}
		return;
	}
	for (ptEvent = ptFirst; ptEvent; ptEvent = ptGameEventList->ptCurrent)
	{
		ptGameEventList->ptCurrent = ptEvent->ptNext;
#line 406
		D2ASSERT(!(ptEvent->dwFlags & EVENT_UNREGISTERED_FLAG));
		PlrModeEvent(ptGame, ptEvent->hUnit, ptEvent->eEventType);
	}
}

static void __fastcall EventRunMonsters(GAME *ptGame, GAMEEVENTLIST *ptGameEventList, EVENT *ptFirst,
	BOOL bFrame)
{
	EVENT *ptEvent;

	ptGameEventList->ptCurrent = NULL;
	if (bFrame)
	{
		for (ptEvent = ptFirst; ptEvent; ptEvent = ptGameEventList->ptCurrent)
		{
			ptGameEventList->ptCurrent = ptEvent->ptNext;
			if (ptEvent->nFrame != ptGame->nFrame)
				continue;
			ptEvent->dwFlags |= EVENT_FLAG_EXPIRE;
#line 431
			D2ASSERT(!(ptEvent->dwFlags & EVENT_UNREGISTERED_FLAG));
			MonsterModeProcessEvent(ptGame, ptEvent->hUnit, ptEvent->eEventType);
			if (ptEvent->dwFlags & EVENT_FLAG_EXPIRE)
				EventRemove(ptGame, ptEvent);
		}
		return;
	}
	for (ptEvent = ptFirst; ptEvent; ptEvent = ptGameEventList->ptCurrent)
	{
		ptGameEventList->ptCurrent = ptEvent->ptNext;
#line 449
		D2ASSERT(!(ptEvent->dwFlags & EVENT_UNREGISTERED_FLAG));
		MonsterModeProcessEvent(ptGame, ptEvent->hUnit, ptEvent->eEventType);
	}
}

static void __fastcall EventRunObjects(GAME *ptGame, GAMEEVENTLIST *ptGameEventList, EVENT *ptFirst,
	BOOL bFrame)
{
	EVENT *ptEvent;

	ptGameEventList->ptCurrent = NULL;
	if (bFrame)
	{
		for (ptEvent = ptFirst; ptEvent; ptEvent = ptGameEventList->ptCurrent)
		{
			ptGameEventList->ptCurrent = ptEvent->ptNext;
			if (ptEvent->nFrame != ptGame->nFrame)
				continue;
			ptEvent->dwFlags |= EVENT_FLAG_EXPIRE;
#line 474
			D2ASSERT(!(ptEvent->dwFlags & EVENT_UNREGISTERED_FLAG));
			ObjectModeEvent(ptGame, ptEvent->hUnit, ptEvent->eEventType);
			if (ptEvent->dwFlags & EVENT_FLAG_EXPIRE)
				EventRemove(ptGame, ptEvent);
		}
		return;
	}
	for (ptEvent = ptFirst; ptEvent; ptEvent = ptGameEventList->ptCurrent)
	{
		ptGameEventList->ptCurrent = ptEvent->ptNext;
#line 492
		D2ASSERT(!(ptEvent->dwFlags & EVENT_UNREGISTERED_FLAG));
		ObjectModeEvent(ptGame, ptEvent->hUnit, ptEvent->eEventType);
	}
}

static void __fastcall EventRunMissiles(GAME *ptGame, GAMEEVENTLIST *ptGameEventList, EVENT *ptFirst,
	BOOL bFrame)
{
	EVENT *ptEvent;

	ptGameEventList->ptCurrent = NULL;
	if (bFrame)
	{
		for (ptEvent = ptFirst; ptEvent; ptEvent = ptGameEventList->ptCurrent)
		{
			ptGameEventList->ptCurrent = ptEvent->ptNext;
			if (ptEvent->nFrame != ptGame->nFrame)
				continue;
			ptEvent->dwFlags |= EVENT_FLAG_EXPIRE;
#line 517
			D2ASSERT(!(ptEvent->dwFlags & EVENT_UNREGISTERED_FLAG));
			MissModeRun(ptGame, ptEvent->hUnit, ptEvent->eEventType);
			if (ptEvent->dwFlags & EVENT_FLAG_EXPIRE)
				EventRemove(ptGame, ptEvent);
		}
		return;
	}
	for (ptEvent = ptFirst; ptEvent; ptEvent = ptGameEventList->ptCurrent)
	{
		ptGameEventList->ptCurrent = ptEvent->ptNext;
#line 535
		D2ASSERT(!(ptEvent->dwFlags & EVENT_UNREGISTERED_FLAG));
		MissModeRun(ptGame, ptEvent->hUnit, ptEvent->eEventType);
	}
}

static void __fastcall EventRunItems(GAME *ptGame, GAMEEVENTLIST *ptGameEventList, EVENT *ptFirst,
	BOOL bFrame)
{
	EVENT *ptEvent;

	ptGameEventList->ptCurrent = NULL;
	if (bFrame)
	{
		for (ptEvent = ptFirst; ptEvent; ptEvent = ptGameEventList->ptCurrent)
		{
			ptGameEventList->ptCurrent = ptEvent->ptNext;
			if (ptEvent->nFrame != ptGame->nFrame)
				continue;
			ptEvent->dwFlags |= EVENT_FLAG_EXPIRE;
#line 560
			D2ASSERT(!(ptEvent->dwFlags & EVENT_UNREGISTERED_FLAG));
			ITEMSModeRunEvent(ptGame, ptEvent->hUnit, ptEvent->eEventType);
			if (ptEvent->dwFlags & EVENT_FLAG_EXPIRE)
				EventRemove(ptGame, ptEvent);
		}
		return;
	}
	for (ptEvent = ptFirst; ptEvent; ptEvent = ptGameEventList->ptCurrent)
	{
		ptGameEventList->ptCurrent = ptEvent->ptNext;
#line 578
		D2ASSERT(!(ptEvent->dwFlags & EVENT_UNREGISTERED_FLAG));
		ITEMSModeRunEvent(ptGame, ptEvent->hUnit, ptEvent->eEventType);
	}
}

static void __fastcall EventListRunType(GAME *ptGame, int eUnitType)
{
	GAMEEVENTLIST *ptGameEventList;

	ptGameEventList = EventGetList(ptGame);
	switch (eUnitType)
	{
	case UNIT_PLAYER:
		EventRunPlayers(ptGame, ptGameEventList, ptGameEventList->aptPersistent[EVENT_LIST_PLAYER], FALSE);
		EventRunPlayers(ptGame, ptGameEventList,
			ptGameEventList->aptFrame[EVENT_LIST_PLAYER][ptGameEventList->nSlot], TRUE);
		return;
	case UNIT_MONSTER:
		EventRunMonsters(ptGame, ptGameEventList, ptGameEventList->aptPersistent[EVENT_LIST_MONSTER], FALSE);
		EventRunMonsters(ptGame, ptGameEventList,
			ptGameEventList->aptFrame[EVENT_LIST_MONSTER][ptGameEventList->nSlot], TRUE);
		return;
	case UNIT_OBJECT:
		EventRunObjects(ptGame, ptGameEventList, ptGameEventList->aptPersistent[EVENT_LIST_OBJECT], FALSE);
		EventRunObjects(ptGame, ptGameEventList,
			ptGameEventList->aptFrame[EVENT_LIST_OBJECT][ptGameEventList->nSlot], TRUE);
		return;
	case UNIT_MISSILE:
		EventRunMissiles(ptGame, ptGameEventList, ptGameEventList->aptPersistent[EVENT_LIST_MISSILE], FALSE);
		EventRunMissiles(ptGame, ptGameEventList,
			ptGameEventList->aptFrame[EVENT_LIST_MISSILE][ptGameEventList->nSlot], TRUE);
		return;
	case UNIT_ITEM:
		EventRunItems(ptGame, ptGameEventList, ptGameEventList->aptPersistent[EVENT_LIST_ITEM], FALSE);
		EventRunItems(ptGame, ptGameEventList,
			ptGameEventList->aptFrame[EVENT_LIST_ITEM][ptGameEventList->nSlot], TRUE);
		return;
	}
#line 622
	D2FATAL("Got unhandled unit type!");
}

void __fastcall EventListCreate(GAME *ptGame)
{
	GAMEEVENTLIST *ptGameEventList;
	int i;

#line 631
	ptGameEventList = (GAMEEVENTLIST *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(GAMEEVENTLIST));
	for (i = 0; i < (int)(sizeof(GAMEEVENTLIST) / sizeof(DWORD)); i++)
		((DWORD *)ptGameEventList)[i] = 0;
	EventHeaderInit(&ptGameEventList->tHeader);
#line 634
	D2ASSERT(! ptGame->hEvents);
	ptGame->hEvents = ptGameEventList;
}

void __fastcall EventListFree(GAME *ptGame)
{
	GAMEEVENTLIST *ptGameEventList;
	EVENTHEADER *ptHeader;
	EVENTHEADER *ptNext;

	ptGameEventList = EventGetList(ptGame);
	for (ptHeader = ptGameEventList->tHeader.ptNext; ptHeader; ptHeader = ptNext)
	{
		ptNext = ptHeader->ptNext;
#line 647
		D2FREEPOOL(ptGame->pMemPool, ptHeader);
	}
#line 650
	D2FREEPOOL(ptGame->pMemPool, ptGameEventList);
	ptGame->hEvents = NULL;
}

void __fastcall EventListRun(GAME *ptGame)
{
	GAMEEVENTLIST *ptGameEventList;

	ptGameEventList = EventGetList(ptGame);
	ptGameEventList->nSlot = ptGame->nFrame % EVENT_FRAMES;
	EventListRunType(ptGame, UNIT_MISSILE);
	EventListRunType(ptGame, UNIT_PLAYER);
	EventListRunType(ptGame, UNIT_MONSTER);
	EventListRunType(ptGame, UNIT_OBJECT);
	EventListRunType(ptGame, UNIT_ITEM);
}

int __fastcall EventGetFirstFrame(GAME *ptGame, UNIT *hUnit, int eEventType)
{
	EVENT **hUnitEvents;
	EVENT *ptEvent;
	EVENT *ptNext;
	int nFrame;

	nFrame = 0;
	hUnitEvents = UnitGetEventList(hUnit);
	if (hUnitEvents)
	{
		for (ptEvent = *hUnitEvents; ptEvent; ptEvent = ptNext)
		{
			ptNext = ptEvent->ptUnitNext;
#line 689
			D2ASSERT(ptEvent->hUnit == hUnit);
			if (ptEvent->eEventType == eEventType && ptEvent->nFrame > 0 && (ptEvent->nFrame < nFrame || !nFrame))
				nFrame = ptEvent->nFrame;
		}
	}
	return nFrame;
}

void __cdecl GameLog(int nLevel, const char *pszFormat, ...)
{
	char szMessage[4096];
	va_list pArgs;

	va_start(pArgs, pszFormat);
	vsprintf(szMessage, pszFormat, pArgs);
	va_end(pArgs);
	if (ptEventCallbackTable && ptEventCallbackTable->fpServerLogMessage)
	{
		ptEventCallbackTable->fpServerLogMessage(nLevel, szMessage);
		return;
	}
	if (nLevel)
		FogErrorManagerLog(szMessage);
}
