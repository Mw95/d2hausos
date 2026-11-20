#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\UNIT\\PlayerList.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/SCmd.h"
#include "Party.h"
#include "SUnit.h"
#include "PlayerList.h"

#define UNIT_PLAYER			0
#define PARTY_NONE			65535
#define UNITFLAG_7			7

static inline PLAYERLIST *PlayerListGet(UNIT *hPlayer  ,
	UNIT *hUnit)
{
	PLRINFO *ptInfo;
	PLAYERLIST *ptList;
	DWORD dwId;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
	dwId = hUnit->dwId;
	if (hPlayer == hUnit)
		return NULL;
	for (ptList = ptInfo->ptPlayerList; ptList; ptList = ptList->pNext)
		if (ptList->dwUnitId == dwId)
			return ptList;
	return NULL;
}

void __fastcall PlayerListFree(GAME *ptGame  ,
	UNIT *hPlayer  )
{
	PLRINFO *ptInfo;
	PLAYERLIST *ptList;
	PLAYERLIST *ptNext;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
	SUnitForEachPlayer(ptGame, PlayerListRemoveUnit, hPlayer);
	for (ptList = ptInfo->ptPlayerList; ptList; ptList = ptNext)
	{
		ptNext = ptList->pNext;
#line 77
		D2FREEPOOL(ptGame->pMemPool, ptList);
	}
	ptInfo->ptPlayerList = NULL;
}

void __fastcall PlayerListRemoveUnit(GAME *ptGame  , UNIT *hPlayer
, void *pArg)
{
	PLRINFO *ptInfo;
	PLAYERLIST *ptList;
	PLAYERLIST *ptPrev;
	DWORD dwUnitId;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
	ptPrev = NULL;
	dwUnitId = ((UNIT *)pArg)->dwId;
	if (hPlayer == (UNIT *)pArg)
		return;
	for (ptList = ptInfo->ptPlayerList; ptList; ptList = ptList->pNext)
	{
		if (ptList->dwUnitId == dwUnitId)
		{
			if (ptPrev)
				ptPrev->pNext = ptList->pNext;
			else
				ptInfo->ptPlayerList = ptList->pNext;
#line 149
			D2FREEPOOL(ptGame->pMemPool, ptList);
			return;
		}
		ptPrev = ptList;
	}
}

void __fastcall PlayerListAdd(GAME *ptGame  ,
	UNIT *hPlayer  ,
	UNIT *hUnit)
{
	PLRINFO *ptInfo;
	PLAYERLIST *ptList;
	int i;

	ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
	if (hPlayer == hUnit)
		return;
	if (D2CommonStatsGetState(hUnit, UNITFLAG_7))
		return;
	if (D2CommonStatsGetState(hPlayer, UNITFLAG_7))
		return;
	if (PlayerListGet(hPlayer, hUnit))
		return;
#line 108
	ptList = (PLAYERLIST *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(PLAYERLIST));
	for (i = 0; i < (int)(sizeof(PLAYERLIST) / sizeof(DWORD)); i++)
		((DWORD *)ptList)[i] = 0;
	ptList->dwUnitId = hUnit->dwId;
	if (SUnitGetParty(hUnit) == PARTY_NONE)
		ptList->dwState = 0;
	else
		ptList->dwState = 1;
	ptList->pNext = ptInfo->ptPlayerList;
	ptInfo->ptPlayerList = ptList;
}

void __fastcall PlayerListSetFlag(GAME *ptGame  , UNIT *hPlayer
, UNIT *hUnit, DWORD dwFlag, BOOL bSet)
{
	PLAYERLIST *ptList;

	ptList = PlayerListGet(hPlayer, hUnit);
	if (hPlayer == hUnit)
		return;
	if (ptList)
	{
		if (bSet)
			ptList->dwFlags |= dwFlag;
		else
			ptList->dwFlags &= ~dwFlag;
	}
	SCmdSendAllRelation(ptGame, hPlayer->dwId, hUnit->dwId, (WORD)ptList->dwFlags);
}

DWORD __fastcall PlayerListTestFlag(UNIT *hPlayer  , UNIT *hUnit,
	DWORD dwMask)
{
	PLAYERLIST *ptList;

	ptList = PlayerListGet(hPlayer, hUnit);
	if (hPlayer != hUnit && ptList)
		return ptList->dwFlags & dwMask;
	return 0;
}

WORD __fastcall PlayerListGetFlags(UNIT *hPlayer  , UNIT *hUnit)
{
	PLAYERLIST *ptList;

	ptList = PlayerListGet(hPlayer, hUnit);
	if (hPlayer != hUnit && ptList)
		return (WORD)ptList->dwFlags;
	return 0;
}

void __fastcall PlayerListSetState(UNIT *hPlayer  , UNIT *hUnit,
	DWORD dwState)
{
	PLAYERLIST *ptList;

	ptList = PlayerListGet(hPlayer, hUnit);
	if (hPlayer != hUnit && ptList)
		ptList->dwState = dwState;
}

WORD __fastcall PlayerListGetState(UNIT *hPlayer  , UNIT *hUnit)
{
	PLAYERLIST *ptList;

	ptList = PlayerListGet(hPlayer, hUnit);
	if (hPlayer == hUnit)
		return 1;
	if (ptList)
		return (WORD)ptList->dwState;
	return 0;
}

void __fastcall PlayerListSendState(UNIT *hPlayer  , UNIT *hUnit)
{
	PLAYERLIST *ptList;

	ptList = PlayerListGet(hPlayer, hUnit);
	if (hPlayer->dwId == hUnit->dwId)
		return;
#line 263
	D2ASSERT(ptList);
	SCmdSendRosterState(SUnitGetClient(hPlayer), hUnit->dwId, (BYTE)ptList->dwState);
}

void __fastcall PlayerListSendFlags(GAME *ptGame  , UNIT *hPlayer
, UNIT *hUnit)
{
	PLAYERLIST *ptList;

	ptList = PlayerListGet(hPlayer, hUnit);
	if (ptList)
		SCmdSendAllRelation(ptGame, hPlayer->dwId, hUnit->dwId, (WORD)ptList->dwFlags);
}

void __fastcall PlayerListAddPlayer(GAME *ptGame  , UNIT *hPlayer
)
{
#line 331
	D2ASSERT(hPlayer);
	SUnitForEachPlayer(ptGame, PlayerListJoin, hPlayer);
}

void __fastcall PlayerListJoin(GAME *ptGame  , UNIT *hUnit,
	void *pArg)
{
	UNIT *hPlayer;
	PLAYERLIST *ptList;
	DWORD dwUnitId;

	hPlayer = (UNIT *)pArg;
	dwUnitId = hUnit->dwId;
	PlayerListAdd(ptGame, hUnit, hPlayer);
	PlayerListAdd(ptGame, hPlayer, hUnit);
	SCmdSendParty(hPlayer, hUnit);
	PlayerListSetState(hUnit, hPlayer, 0);
	if (SUnitGetParty(hUnit) == PARTY_NONE)
		PlayerListSetState(hPlayer, hUnit, 0);
	else
		PlayerListSetState(hPlayer, hUnit, 1);
	PlayerListSendState(hPlayer, hUnit);
	PlayerListSendState(hUnit, hPlayer);
	for (ptList = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->ptPlayerList; ptList;
		ptList = ptList->pNext)
		SCmdSendRelation(hPlayer, dwUnitId, ptList->dwUnitId, (WORD)ptList->dwFlags);
}

void __fastcall PlayerListRefresh(GAME *ptGame  , UNIT *hPlayer
)
{
	PLAYERLIST *ptList;
	UNIT *hUnit;

	for (ptList = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->ptPlayerList; ptList;
		ptList = ptList->pNext)
	{
		if (ptList->dwState != 2)
			continue;
		hUnit = SUnitFind(ptGame, UNIT_PLAYER, ptList->dwUnitId);
		if (!hUnit)
			continue;
		ptList->dwState = PartyGetId(ptGame, hUnit) != PARTY_NONE;
		PlayerListSendState(hPlayer, hUnit);
	}
}
