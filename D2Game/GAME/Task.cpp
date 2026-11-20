#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\GAME\\Task.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "Linklist.h"
#include "Game.h"
#include "Task.h"

#define NUM_TASKQUEUES		32
#define TASK_INTERVAL		40
#define TASK_EMPTYGAMETIME	300000

struct TASKQUEUE
{
 	DWORD		nIndex;
 	int			nTasks;
 	LINK		tTasks;
 	LINK		tLink;
};
typedef struct TASKQUEUE TASKQUEUE;
typedef TASKQUEUE *TASKQUEUES[NUM_TASKQUEUES];

#define TASKOF(ptLink)			CONTAINING_RECORD(ptLink, TASK, tLink)
#define TASKQUEUEOF(ptLink)		CONTAINING_RECORD(ptLink, TASKQUEUE, tLink)

struct TASKLOCK : CRITICAL_SECTION
{
	TASKLOCK();
	~TASKLOCK();
	void Enter() { EnterCriticalSection(this); }
	void Leave() { LeaveCriticalSection(this); }
};

static BOOL sgfPerfCounter;
static DWORD sgdwQueues;
static DWORD sgdwNumTasks;
static LARGE_INTEGER sgtPerfFrequency;
static LINK sgtQueues;
static TASKLOCK sgtTasksLock;
static TASKLOCK sgatQueueCritSect[NUM_TASKQUEUES];
static TASKQUEUES sgaptQueue = { 0 };

static inline DWORD TaskGetTime(void)
{
	LARGE_INTEGER tCount;

	if (sgfPerfCounter)
	{
		QueryPerformanceCounter(&tCount);
		return (DWORD)(tCount.QuadPart * 1000 / sgtPerfFrequency.QuadPart);
	}
	return GetTickCount();
}

static inline void TaskQueueInsertTask(TASKQUEUE *ptQueue, TASK *ptTask, int nDelay)
{
	LINK *ptList;
	LINK *ptPrev;

	ptList = &ptQueue->tTasks;
	ptTask->dwTime += nDelay;
	for (ptPrev = ptList->ptPrev; ptPrev != ptList; ptPrev = ptPrev->ptPrev)
		if ((int)(TASKOF(ptPrev)->dwTime - ptTask->dwTime) <= 0)
			break;
	LinkListInsertAfter(ptPrev, &ptTask->tLink);
}

static inline void TaskQueueDropTask(DWORD nQueue)
{
	TASKQUEUE *ptQueue;
	TASKQUEUE *ptTailQueue;
	LINK *ptTaskQTailLink;
	LINK *ptTaskBalanceLink;

	sgtTasksLock.Enter();
	ptQueue = sgaptQueue[nQueue & (NUM_TASKQUEUES - 1)];
	if (ptQueue)
	{
		ptTaskQTailLink = LinkListGetTail(&sgtQueues);
#line 256
		D2ASSERT(ptTaskQTailLink);
		ptTailQueue = TASKQUEUEOF(ptTaskQTailLink);
		if (ptTailQueue->nTasks == ptQueue->nTasks + 1)
		{
			sgatQueueCritSect[ptTailQueue->nIndex].Enter();
			ptTaskBalanceLink = LinkListGetTail(&ptTailQueue->tTasks);
#line 265
			D2ASSERT(ptTaskBalanceLink);
			LinkListUnlink(ptTaskBalanceLink);
			sgatQueueCritSect[ptTailQueue->nIndex].Leave();
			sgatQueueCritSect[ptQueue->nIndex].Enter();
			TaskQueueInsertTask(ptQueue, TASKOF(ptTaskBalanceLink), 0);
			sgatQueueCritSect[ptQueue->nIndex].Leave();
			ptTailQueue->nTasks--;
			LinkListUnlink(ptTaskQTailLink);
			LinkListInsertHead(&sgtQueues, ptTaskQTailLink);
		}
		else
		{
			ptQueue->nTasks--;
			LinkListUnlink(&ptQueue->tLink);
			LinkListInsertHead(&sgtQueues, &ptQueue->tLink);
		}
	}
	sgdwNumTasks--;
	sgtTasksLock.Leave();
}

void __cdecl TaskInitTimer(void)
{
	sgfPerfCounter = QueryPerformanceFrequency(&sgtPerfFrequency);
}

void __cdecl TaskFreeQueues(void)
{
	int i;

	sgtTasksLock.Enter();
	for (i = 0; i < NUM_TASKQUEUES; i++)
	{
		sgatQueueCritSect[i].Enter();
		if (sgaptQueue[i])
		{
			LinkListUnlink(&sgaptQueue[i]->tLink);
#line 319
			D2FREEPOOL(NULL, sgaptQueue[i]);
			sgaptQueue[i] = NULL;
		}
		sgatQueueCritSect[i].Leave();
	}
	sgdwQueues = 0;
	sgdwNumTasks = 0;
	sgtTasksLock.Leave();
}

extern void TaskQueueTakeShare(TASKQUEUE *ptNew);
__inline void TaskQueueTakeShare(TASKQUEUE *ptNew)
{
	TASKQUEUE *ptQueue;
	LINK *ptList;
	LINK *ptQueueLink;
	LINK *ptLast;
	LINK *ptCut;
	LINK *ptMove;
	LINK *ptNextMove;
	LINK *ptPrev;
	DWORD nPerQueue;
	int nExtra;
	int nSkip;

	nPerQueue = sgdwNumTasks / sgdwQueues;
	nExtra = sgdwNumTasks % sgdwQueues;
	nExtra = (sgdwQueues - 1 < NUM_TASKQUEUES ? sgdwQueues - 1 : NUM_TASKQUEUES) - nExtra;
	ptList = &ptNew->tTasks;
	for (ptQueueLink = sgtQueues.ptNext; ptQueueLink != &sgtQueues;
		ptQueueLink = ptQueueLink->ptNext)
	{
		ptQueue = TASKQUEUEOF(ptQueueLink);
		nSkip = ptQueue->nTasks - !(nPerQueue + (--nExtra >= 0));
		sgatQueueCritSect[ptQueue->nIndex].Enter();
		ptLast = ptQueue->tTasks.ptPrev;
		ptCut = ptLast;
		while (nSkip-- && ptCut != &ptQueue->tTasks)
			ptCut = ptCut->ptNext;
		ptMove = ptCut->ptNext;
		ptCut->ptNext = &ptQueue->tTasks;
		ptQueue->tTasks.ptPrev = ptCut;
		sgatQueueCritSect[ptQueue->nIndex].Leave();
		if (ptMove == &ptQueue->tTasks)
			continue;
		ptLast->ptNext = NULL;
		for (ptPrev = ptList->ptPrev; ptPrev != ptList; ptPrev = ptPrev->ptPrev)
			if ((int)(TASKOF(ptMove)->dwTime - TASKOF(ptPrev)->dwTime) >= 0)
				break;
		for (;;)
		{
			if (ptPrev->ptNext == ptList)
			{
				ptMove->ptPrev = ptPrev;
				ptLast->ptNext = ptList;
				ptPrev->ptNext = ptMove;
				ptList->ptPrev = ptLast;
				break;
			}
			ptNextMove = ptMove->ptNext;
			LinkListInsertAfter(ptPrev, ptMove);
			if (!ptNextMove)
				break;
			ptPrev = ptMove;
			ptMove = ptNextMove;
			while (ptPrev->ptNext != ptList
				&& (int)(TASKOF(ptMove)->dwTime - TASKOF(ptPrev->ptNext)->dwTime) > 0)
				ptPrev = ptPrev->ptNext;
		}
	}
	LinkListInsertHead(&sgtQueues, &ptNew->tLink);
}

int __cdecl TaskCreateQueue(void)
{
	DWORD nIndex;
	TASKQUEUE *ptNew;

	sgtTasksLock.Enter();
	if (!sgtQueues.ptPrev)
		LinkListInit(&sgtQueues);
	nIndex = sgdwQueues & (NUM_TASKQUEUES - 1);
	sgdwQueues++;
	sgatQueueCritSect[nIndex].Enter();
	if (!sgaptQueue[nIndex])
	{
#line 348
		ptNew = (TASKQUEUE *)D2ALLOCPOOL(NULL, sizeof(TASKQUEUE));
		ptNew->nIndex = nIndex;
		ptNew->nTasks = 0;
		LinkListInit(&ptNew->tTasks);
		sgaptQueue[nIndex] = ptNew;
		TaskQueueTakeShare(ptNew);
	}
	sgatQueueCritSect[nIndex].Leave();
	sgtTasksLock.Leave();
	return nIndex;
}

BOOL __fastcall TaskQueueGame(TASK *ptTask, DWORD eType, DWORD dwGameId)
{
	BOOL fQueued;
	LINK *ptQueueLink;
	TASKQUEUE *ptQueue;

	fQueued = FALSE;
	ptTask->dwTime = TaskGetTime();
	ptTask->eType = eType;
	ptTask->dwGameId = dwGameId;
	sgtTasksLock.Enter();
	if (!sgtQueues.ptPrev)
		LinkListInit(&sgtQueues);
	else
	{
		ptQueueLink = LinkListGetHead(&sgtQueues);
		if (ptQueueLink)
		{
			ptQueue = TASKQUEUEOF(ptQueueLink);
			sgatQueueCritSect[ptQueue->nIndex].Enter();
			LinkListInsertHead(&ptQueue->tTasks, &ptTask->tLink);
			sgatQueueCritSect[ptQueue->nIndex].Leave();
			ptQueue->nTasks++;
			sgdwNumTasks++;
			LinkListUnlink(ptQueueLink);
			LinkListInsertTail(&sgtQueues, ptQueueLink);
			fQueued = TRUE;
		}
	}
	sgtTasksLock.Leave();
	return fQueued;
}

int __fastcall TaskGetDue(DWORD nQueueRaw, TASK **pptTask)
{
	int nWait;
	DWORD nQueue;
	TASKQUEUE *ptQueue;
	LINK *ptLink;
	TASK *ptTask;

	nQueue = nQueueRaw & (NUM_TASKQUEUES - 1);
	nWait = 100;
	*pptTask = NULL;
	sgatQueueCritSect[nQueue].Enter();
	ptQueue = sgaptQueue[nQueue];
	if (!ptQueue)
	{
		sgatQueueCritSect[nQueue].Leave();
		return nWait;
	}
	ptLink = LinkListGetHead(&ptQueue->tTasks);
	if (ptLink)
	{
		ptTask = TASKOF(ptLink);
		nWait = ptTask->dwTime - TaskGetTime();
		if (nWait <= 0)
		{
			LinkListUnlink(ptLink);
			*pptTask = ptTask;
		}
	}
	sgatQueueCritSect[ptQueue->nIndex].Leave();
	return nWait;
}

int __fastcall TaskGetWait(DWORD nQueueRaw)
{
	int nWait;
	DWORD nQueue;
	TASKQUEUE *ptQueue;
	LINK *ptLink;

	nQueue = nQueueRaw & (NUM_TASKQUEUES - 1);
	nWait = 100;
	sgatQueueCritSect[nQueue].Enter();
	ptQueue = sgaptQueue[nQueue];
	if (!ptQueue)
		sgatQueueCritSect[nQueue].Leave();
	else
	{
		ptLink = LinkListGetHead(&ptQueue->tTasks);
		if (ptLink)
			nWait = TASKOF(ptLink)->dwTime - TaskGetTime();
		sgatQueueCritSect[ptQueue->nIndex].Leave();
	}
	return nWait;
}

extern BOOL TaskGameIsExpired(GAME *ptGame);
__inline BOOL TaskGameIsExpired(GAME *ptGame)
{
	BOOL fExpired;
	DWORD dwNow;

	fExpired = FALSE;
	dwNow = GetTickCount();
	if (!ptGame->nNumClients)
	{
		if (!ptGame->dwEmptyTick)
			ptGame->dwEmptyTick = dwNow;
		if (dwNow - ptGame->dwEmptyTick > TASK_EMPTYGAMETIME
			|| (ptGame->nFrame > 1500 && ptGame->nFrame < 7500))
			fExpired = TRUE;
	}
	else
		ptGame->dwEmptyTick = 0;
	return fExpired;
}

extern TASKQUEUE *TaskQueueLock(DWORD nQueue);
__inline TASKQUEUE *TaskQueueLock(DWORD nQueue)
{
	DWORD nIndex;
	TASKQUEUE *ptQueue;

	nIndex = nQueue & (NUM_TASKQUEUES - 1);
	sgatQueueCritSect[nIndex].Enter();
	ptQueue = sgaptQueue[nIndex];
	if (!ptQueue)
		sgatQueueCritSect[nIndex].Leave();
	return ptQueue;
}

extern void TaskQueueRequeue(DWORD nQueue, TASK *ptTask, int nDelay);
__inline void TaskQueueRequeue(DWORD nQueue, TASK *ptTask, int nDelay)
{
	TASKQUEUE *ptQueue;

	ptQueue = TaskQueueLock(nQueue);
	if (!ptQueue)
		return;
	TaskQueueInsertTask(ptQueue, ptTask, nDelay);
	sgatQueueCritSect[ptQueue->nIndex].Leave();
}

static inline void TaskRunGameFrame(DWORD nQueue, TASK *ptTask)
{
	DWORD dwGameId;
	GAME *ptGame;

	dwGameId = ptTask->dwGameId;
	ptGame = GameLockByHandle(dwGameId);
	if (!ptGame)
		return;
	GameUpdate(ptGame);
	GameSendAllClientUpdates(ptGame, 0);
	if (!TaskGameIsExpired(ptGame))
	{
		GameUnlock(ptGame);
		TaskQueueRequeue(nQueue, ptTask, TASK_INTERVAL);
		return;
	}
	GameUnlock(ptGame);
	GameClose(dwGameId);
	TaskQueueDropTask(nQueue);
}

void __fastcall TaskProcessGame(DWORD nQueue, TASK *ptTask)
{
#line 452
	D2ASSERT(ptTask->eType == TASK_PROCESSGAME);
	TaskRunGameFrame(nQueue, ptTask);
}

TASKLOCK::TASKLOCK()
{
	InitializeCriticalSection(this);
}

TASKLOCK::~TASKLOCK()
{
	DeleteCriticalSection(this);
}
