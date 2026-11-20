#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\GAME\\Targets.cpp"
#include <string.h>
#include "../D2game.h"
#include "D2Macros.h"
#include "Targets.h"

#define TARGET_LISTNO_MAX_REGULAR	8
#define TARGET_LISTNO_TOTAL			10
#define TARGET_LISTNO_NULL			11
#define UNIT_MONSTER				1

#define UnitGetTargetListNo(hUnit)	((hUnit)->nTargetListNo)

void __fastcall TargetsNewList(GAME *ptGame, UNIT *hUnit, DWORD dwField04)
{
	int nListNo;
	TARGET *ptTarget;
	TARGET **pptList;

#line 17
	D2ASSERT(ptGame);
#line 18
	D2ASSERT(hUnit);
#line 19
	D2ASSERT(UnitGetType (hUnit) == UNIT_PLAYER);
#line 20
	D2ASSERT(UnitGetTargetListNo (hUnit) == TARGET_LISTNO_NULL);
	pptList = ptGame->aptTargetList;
	for (nListNo = 0; nListNo < TARGET_LISTNO_MAX_REGULAR; pptList++, nListNo++)
		if (!*pptList)
			break;
#line 32
	D2ASSERT(ptGame->nNumClients <= TARGET_LISTNO_MAX_REGULAR);
#line 33
	D2ASSERT(nListNo < TARGET_LISTNO_MAX_REGULAR);
#line 37
	ptTarget = (TARGET *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(TARGET));
	memset(ptTarget, 0, sizeof(TARGET));
	D2_OPAQUE(ptTarget);
#line 41
	D2ASSERT(ptTarget);
	ptTarget->hUnit = hUnit;
	ptTarget->field_04 = dwField04;
	*pptList = ptTarget;
	hUnit->nTargetListNo = nListNo;
}

void __fastcall TargetsAddToList(GAME *ptGame, UNIT *hUnit, DWORD dwField04, int nListNo)
{
	TARGET *ptTargetHead;
	TARGET *ptTargetNew;

#line 58
	D2ASSERT(ptGame);
#line 59
	D2ASSERT(hUnit);
#line 60
	D2ASSERT(nListNo < TARGET_LISTNO_MAX_REGULAR);
#line 61
	D2ASSERT((UnitGetType (hUnit) == UNIT_PLAYER) || (UnitGetType (hUnit) == UNIT_MONSTER));
#line 62
	D2ASSERT(UnitGetTargetListNo (hUnit) == TARGET_LISTNO_NULL);
	ptTargetHead = ptGame->aptTargetList[nListNo];
#line 69
	D2ASSERT(ptTargetHead);
#line 70
	D2ASSERT(ptTargetHead->hUnit);
#line 74
	ptTargetNew = (TARGET *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(TARGET));
	memset(ptTargetNew, 0, sizeof(TARGET));
	D2_OPAQUE(ptTargetNew);
#line 78
	D2ASSERT(ptTargetNew);
	ptTargetNew->hUnit = hUnit;
	ptTargetNew->field_04 = dwField04;
	ptTargetNew->ptNext = ptTargetHead->ptNext;
	ptTargetNew->ptPrev = ptTargetHead;
	if (ptTargetHead->ptNext)
		ptTargetHead->ptNext->ptPrev = ptTargetNew;
	ptTargetHead->ptNext = ptTargetNew;
	hUnit->nTargetListNo = nListNo;
}

void __fastcall TargetsAddToFurtherList(GAME *ptGame, UNIT *hUnit, DWORD dwField04,
	int nListNo)
{
	TARGET *ptFirst;
	TARGET *ptTargetNew;

#line 98
	D2ASSERT(ptGame);
#line 99
	D2ASSERT(UnitGetTargetListNo (hUnit) == TARGET_LISTNO_NULL);
#line 100
	D2ASSERT((UnitGetType (hUnit) == UNIT_PLAYER) || (UnitGetType (hUnit) == UNIT_MONSTER));
#line 101
	D2ASSERT((nListNo >= TARGET_LISTNO_MAX_REGULAR) && (nListNo < TARGET_LISTNO_TOTAL));
	ptFirst = ptGame->aptTargetList[nListNo];
#line 111
	ptTargetNew = (TARGET *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(TARGET));
	memset(ptTargetNew, 0, sizeof(TARGET));
	D2_OPAQUE(ptTargetNew);
#line 115
	D2ASSERT(ptTargetNew);
	ptTargetNew->hUnit = hUnit;
	ptTargetNew->field_04 = dwField04;
	ptGame->aptTargetList[nListNo] = ptTargetNew;
	if (ptFirst)
	{
		ptTargetNew->ptNext = ptFirst;
		ptFirst->ptPrev = ptTargetNew;
	}
	hUnit->nTargetListNo = nListNo;
}

void __fastcall TargetsFreeList(GAME *ptGame, UNIT *hUnit)
{
	int nListNo;
	TARGET *ptTargetHead;
	TARGET *ptTarget;
	TARGET *ptNext;
	UNIT *hTarget;

#line 138
	D2ASSERT(ptGame);
#line 139
	D2ASSERT(hUnit);
#line 140
	D2ASSERT((UnitGetType (hUnit) == UNIT_PLAYER) || (UnitGetType (hUnit) == UNIT_MONSTER));
	nListNo = UnitGetTargetListNo(hUnit);
	if (nListNo == TARGET_LISTNO_NULL)
		return;
	ptTargetHead = ptGame->aptTargetList[nListNo];
#line 154
	D2ASSERT(ptTargetHead->hUnit);
#line 155
	D2ASSERT((nListNo > TARGET_LISTNO_MAX_REGULAR) || (UnitGetType (ptTargetHead->hUnit) == UNIT_PLAYER));
	for (ptTarget = ptTargetHead; ptTarget; ptTarget = ptNext)
	{
		hTarget = ptTarget->hUnit;
		ptNext = ptTarget->ptNext;
#line 164
		D2ASSERT(hTarget);
		hTarget->nTargetListNo = TARGET_LISTNO_NULL;
#line 167
		D2FREEPOOL(ptGame->pMemPool, ptTarget);
	}
	ptGame->aptTargetList[nListNo] = NULL;
}

void __fastcall TargetsRemove(GAME *ptGame, UNIT *hUnit)
{
	int nListNo;
	TARGET *ptTarget;
	TARGET *ptFirst;

#line 179
	D2ASSERT(ptGame);
#line 180
	D2ASSERT(hUnit);
#line 181
	D2ASSERT((UnitGetType (hUnit) == UNIT_PLAYER) || (UnitGetType (hUnit) == UNIT_MONSTER));
	nListNo = UnitGetTargetListNo(hUnit);
	if (nListNo == TARGET_LISTNO_NULL)
		return;
	ptTarget = ptGame->aptTargetList[nListNo];
#line 193
	D2ASSERT(ptTarget);
	if (nListNo < TARGET_LISTNO_MAX_REGULAR)
	{
		for (ptTarget = ptTarget->ptNext; ptTarget; ptTarget = ptTarget->ptNext)
		{
			if (ptTarget->hUnit == hUnit)
			{
				ptTarget->ptPrev->ptNext = ptTarget->ptNext;
				if (ptTarget->ptNext)
					ptTarget->ptNext->ptPrev = ptTarget->ptPrev;
				break;
			}
		}
	}
	else
	{
		ptFirst = ptTarget;
		for (; ptTarget; ptTarget = ptTarget->ptNext)
		{
			if (ptTarget->hUnit == hUnit)
			{
				if (ptTarget == ptFirst)
				{
					ptGame->aptTargetList[nListNo] = ptTarget->ptNext;
					if (ptTarget->ptNext)
						ptTarget->ptNext->ptPrev = NULL;
				}
				else
				{
					ptTarget->ptPrev->ptNext = ptTarget->ptNext;
					if (ptTarget->ptNext)
						ptTarget->ptNext->ptPrev = ptTarget->ptPrev;
				}
				break;
			}
		}
	}
#line 240
	D2ASSERT(ptTarget);
#line 243
	D2FREEPOOL(ptGame->pMemPool, ptTarget);
	hUnit->nTargetListNo = TARGET_LISTNO_NULL;
}
