#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\UNIT\\Party.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/SCmd.h"
#include "../PLAYER/PartyScreen.h"
#include "Friendly.h"
#include "Party.h"
#include "SUnit.h"

#define UNIT_PLAYER			0
#define PARTY_NONE			65535
#define STAT_LEVEL			12
#define STAT_GOLD			14

void __fastcall PartyAddLevel(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall PartyCountLiving(GAME *ptGame, UNIT *hUnit, void *pArg);

void __fastcall PartySystemCreate(GAME *ptGame  )
{
	PARTYSYSTEM *ptSystem;
	int i;

#line 23
	D2ASSERT(ptGame);
#line 24
	D2ASSERT(!ptGame->hParty);
#line 25
	ptSystem = (PARTYSYSTEM *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(PARTYSYSTEM));
#line 26
	D2ASSERT(ptSystem);
	for (i = 0; i < (int)(sizeof(PARTYSYSTEM) / sizeof(DWORD)); i++)
		((DWORD *)ptSystem)[i] = 0;
	ptSystem->wNextId = 0;
	ptGame->hParty = ptSystem;
}

void __fastcall PartySystemFree(GAME *ptGame  )
{
	PARTYSYSTEM *ptSystem;
	PARTY *ptParty;
	PARTY *ptNextParty;
	PARTYMEMBER *ptMember;
	PARTYMEMBER *ptNextMember;

#line 45
	D2ASSERT(ptGame);
	ptSystem = ptGame->hParty;
	if (!ptSystem)
		return;
	for (ptParty = ptSystem->pFirst; ptParty; ptParty = ptNextParty)
	{
		ptNextParty = ptParty->pNext;
		for (ptMember = ptParty->pMembers; ptMember; ptMember = ptNextMember)
		{
			ptNextMember = ptMember->pNext;
#line 62
			D2FREEPOOL(ptGame->pMemPool, ptMember);
		}
#line 66
		D2FREEPOOL(ptGame->pMemPool, ptParty);
	}
#line 70
	D2FREEPOOL(ptGame->pMemPool, ptSystem);
	ptGame->hParty = NULL;
}

WORD __fastcall PartyCreate(GAME *ptGame  )
{
	PARTYSYSTEM *ptSystem;
	PARTY *ptParty;
	int i;

	ptSystem = ptGame->hParty;
#line 85
	D2ASSERT(ptSystem);
#line 86
	ptParty = (PARTY *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(PARTY));
#line 87
	D2ASSERT(ptParty);
	for (i = 0; i < (int)(sizeof(PARTY) / sizeof(DWORD)); i++)
		((DWORD *)ptParty)[i] = 0;
	ptParty->wId = ptSystem->wNextId;
	ptSystem->wNextId++;
	if (ptSystem->wNextId == PARTY_NONE)
		ptSystem->wNextId = 0;
	ptParty->pNext = ptSystem->pFirst;
	ptSystem->pFirst = ptParty;
	return ptParty->wId;
}

static inline void PartyDelete(GAME *ptGame, PARTY *ptParty  )
{
	PARTYMEMBER *ptMember;
	PARTYMEMBER *ptNextMember;
	UNIT *hUnit;

#line 111
	D2ASSERT(ptParty);
	for (ptMember = ptParty->pMembers; ptMember; ptMember = ptNextMember)
	{
		hUnit = SUnitFind(ptGame, UNIT_PLAYER, ptMember->dwUnitId);
		if (!hUnit)
		{
#line 116
			D2WARN(hUnit);
		}
		else
		{
			PartyScreenReset(ptGame, hUnit);
			SCmdSendParty(hUnit, hUnit);
		}
		ptNextMember = ptMember->pNext;
#line 124
		D2FREEPOOL(ptGame->pMemPool, ptMember);
	}
#line 127
	D2FREEPOOL(ptGame->pMemPool, ptParty);
}

static inline void PartyRemove(GAME *ptGame, WORD wParty)
{
	PARTYSYSTEM *ptSystem;
	PARTY *ptParty;
	PARTY *ptPrev;

	ptSystem = ptGame->hParty;
	ptPrev = NULL;
	D2_OPAQUE(ptSystem);
#line 142
	D2ASSERT(ptSystem);
	for (ptParty = ptSystem->pFirst; ptParty; ptParty = ptParty->pNext)
	{
		if (ptParty->wId == wParty)
		{
			if (ptPrev)
				ptPrev->pNext = ptParty->pNext;
			else
				ptSystem->pFirst = ptParty->pNext;
			PartyDelete(ptGame, ptParty);
			return;
		}
		ptPrev = ptParty;
	}
}

extern PARTY *PartyFind(GAME *ptGame, WORD wParty);
__inline PARTY *PartyFind(GAME *ptGame, WORD wParty)
{
	PARTYSYSTEM *ptSystem;
	PARTY *ptParty;

	ptSystem = ptGame->hParty;
#line 172
	D2ASSERT(ptSystem);
	for (ptParty = ptSystem->pFirst; ptParty; ptParty = ptParty->pNext)
		if (ptParty->wId == wParty)
			return ptParty;
	return NULL;
}

BOOL __fastcall PartyAddPlayer(GAME *ptGame  , WORD wParty,
	UNIT *hPlayer  )
{
	PARTY *ptParty;
	PARTYMEMBER *ptNode;
	PARTYMEMBER *ptMember;
	UNIT *hUnit;
	DWORD dwUnitId;
	int i;

	ptParty = PartyFind(ptGame, wParty);
	dwUnitId = hPlayer->dwId;
	if (ptParty && UnitGetType (hPlayer) == UNIT_PLAYER) {
	for (ptMember = ptParty->pMembers; ptMember; ptMember = ptMember->pNext)
		if (ptMember->dwUnitId == dwUnitId)
			return TRUE;
#line 226
	ptNode = (PARTYMEMBER *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(PARTYMEMBER));
#line 227
	D2ASSERT(ptNode);
	for (i = 0; i < (int)(sizeof(PARTYMEMBER) / sizeof(DWORD)); i++)
		((DWORD *)ptNode)[i] = 0;
	ptNode->dwUnitId = dwUnitId;
	ptNode->pNext = ptParty->pMembers;
	ptParty->pMembers = ptNode;
	SCmdSendAllParty(ptGame, hPlayer);
	for (ptMember = ptNode->pNext; ptMember; ptMember = ptMember->pNext)
	{
		hUnit = SUnitFind(ptGame, UNIT_PLAYER, ptMember->dwUnitId);
		if (hUnit)
		{
			FriendlyClearHostile(ptGame, hPlayer, hUnit);
			FriendlyClearHostile(ptGame, hUnit, hPlayer);
		}
	}
	return TRUE;
	}
#line 216
	D2ASSERT(ptParty);
#line 217
	D2ASSERT(UnitGetType (hPlayer) == UNIT_PLAYER);
	return TRUE;
}

void __fastcall PartyRemovePlayer(GAME *ptGame  , UNIT *hPlayer
)
{
	PARTY *ptParty;
	PARTYMEMBER *ptMember;
	PARTYMEMBER *ptPrev;
	DWORD dwUnitId;
	WORD wParty;
	int nMembers;

	ptPrev = NULL;
	dwUnitId = hPlayer->dwId;
#line 266
	D2ASSERT(UnitGetType (hPlayer) == UNIT_PLAYER);
	wParty = PartyGetId(ptGame, hPlayer);
	if (wParty == PARTY_NONE)
		return;
	ptParty = PartyFind(ptGame, wParty);
#line 274
	D2ASSERT(ptParty);
	ptMember = ptParty->pMembers;
	if (ptMember)
	{
		while (1)
		{
			if (ptMember->dwUnitId == dwUnitId)
			{
				if (ptPrev)
					ptPrev->pNext = ptMember->pNext;
				else
					ptParty->pMembers = ptMember->pNext;
				PartyScreenReset(ptGame, hPlayer);
				SCmdSendParty(hPlayer, hPlayer);
#line 290
				D2FREEPOOL(ptGame->pMemPool, ptMember);
				break;
			}
			ptPrev = ptMember;
			ptMember = ptMember->pNext;
			if (!ptMember)
				break;
		}
	}
	ptParty = PartyFind(ptGame, wParty);
	nMembers = 0;
	if (ptParty)
		for (ptMember = ptParty->pMembers; ptMember; ptMember = ptMember->pNext)
			nMembers++;
	if (nMembers > 1)
		return;
	PartyRemove(ptGame, wParty);
}

int __fastcall PartyGetLevelSum(GAME *ptGame  , UNIT *hUnit
)
{
	int nLevels;

	nLevels = 0;
	if (SUnitGetParty(hUnit) == PARTY_NONE)
		return D2CommonStatsUnitGetValue(hUnit, STAT_LEVEL);
	PartyForEachInLevel(ptGame, hUnit, PartyAddLevel, &nLevels);
	return nLevels;
}

void __fastcall PartyAddLevel(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	if (SUnitIsDead(hUnit))
		return;
	*(int *)pArg += D2CommonStatsUnitGetValue(hUnit, STAT_LEVEL);
}

void __fastcall PartyCountLiving(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	if (!hUnit)
		return;
	if (SUnitIsDead(hUnit))
		return;
	(*(int *)pArg)++;
}

void __fastcall PartyForEach(GAME *ptGame  , WORD wParty,
	PFNPARTYCALLBACK pfnCallback, void *pArg)
{
	PARTY *ptParty;
	PARTYMEMBER *ptMember;
	UNIT *hPlayer;

	ptParty = PartyFind(ptGame, wParty);
	if (!ptParty || wParty == PARTY_NONE)
		return;
	for (ptMember = ptParty->pMembers; ptMember; ptMember = ptMember->pNext)
	{
		hPlayer = SUnitFind(ptGame, UNIT_PLAYER, ptMember->dwUnitId);
		pfnCallback(ptGame, hPlayer, pArg);
	}
}

void __fastcall PartyForEachInLevel(GAME *ptGame  , UNIT *hUnit
, PFNPARTYCALLBACK pfnCallback, void *pArg)
{
	PARTY *ptParty;
	PARTYMEMBER *ptMember;
	UNIT *hMember;
	DWORD hRoom;
	DWORD dwLevelId;
	WORD wParty;

	hRoom = D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
	{
		pfnCallback(ptGame, hUnit, pArg);
		return;
	}
	dwLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
	wParty = SUnitGetParty(hUnit);
	if (wParty == PARTY_NONE)
	{
		pfnCallback(ptGame, hUnit, pArg);
		return;
	}
	ptParty = PartyFind(ptGame, wParty);
	if (!ptParty)
		return;
	for (ptMember = ptParty->pMembers; ptMember; ptMember = ptMember->pNext)
	{
		hMember = SUnitFind(ptGame, UNIT_PLAYER, ptMember->dwUnitId);
		hRoom = D2CommonUnitGetPathRoom(hMember);
		if (hRoom && D2CommonDungeonRoomGetLevelId(hRoom) == dwLevelId)
			pfnCallback(ptGame, hMember, pArg);
	}
}

extern int PartyCountLivingInLevel(GAME *ptGame, UNIT *hUnit);
__inline int PartyCountLivingInLevel(GAME *ptGame, UNIT *hUnit)
{
	int nLiving;

	nLiving = 0;
	PartyForEachInLevel(ptGame, hUnit, PartyCountLiving, &nLiving);
	return nLiving;
}

static __inline void PartyCalcGold(UNIT *hUnit, int *pnGive, int *pnLeft, int nGold)
{
	int nMax;
	int nHeld;

	nMax = (int)D2CommonUnitCalcGoldLimit(hUnit);
	nHeld = D2CommonStatsUnitGetValue(hUnit, STAT_GOLD);
	*pnLeft = 0;
	*pnGive = nGold;
	if (nHeld + nGold > nMax)
	{
		*pnGive = nMax - nHeld;
		*pnLeft = nGold - *pnGive;
	}
}

int __fastcall PartyShareGold(GAME *ptGame  , UNIT *hUnit
, int nGold)
{
	PARTY *ptParty;
	PARTYMEMBER *ptMember;
	UNIT *hMember;
	DWORD hRoom;
	DWORD dwLevelId;
	int nLiving;
	int nShare;
	int nGive;
	int nLeft;

	nLiving = PartyCountLivingInLevel(ptGame, hUnit);
	if (nLiving <= 1)
	{
		PartyCalcGold(hUnit, &nGive, &nLeft, nGold);
		D2CommonStatsUnitAddValue(hUnit, STAT_GOLD, nGive);
		return nLeft;
	}
	ptParty = PartyFind(ptGame, SUnitGetParty(hUnit));
	if (!ptParty)
	{
		PartyCalcGold(hUnit, &nGive, &nLeft, nGold);
		D2CommonStatsUnitAddValue(hUnit, STAT_GOLD, nGive);
		return nLeft;
	}
	nShare = nGold / nLiving;
	dwLevelId = D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hUnit));
	for (ptMember = ptParty->pMembers; ptMember; ptMember = ptMember->pNext)
	{
		hMember = SUnitFind(ptGame, UNIT_PLAYER, ptMember->dwUnitId);
		if (!hMember || hMember == hUnit)
			continue;
		hRoom = D2CommonUnitGetPathRoom(hMember);
		if (!hRoom)
			continue;
		if (D2CommonDungeonRoomGetLevelId(hRoom) != dwLevelId)
			continue;
		if (hUnit->dwMode == PLRMODE_DEATH || hUnit->dwMode == PLRMODE_DEAD)
			continue;
		PartyCalcGold(hMember, &nGive, &nLeft, nShare);
		D2CommonStatsUnitAddValue(hMember, STAT_GOLD, nGive);
		nGold += nLeft - nShare;
	}
	PartyCalcGold(hUnit, &nGive, &nLeft, nGold);
	D2CommonStatsUnitAddValue(hUnit, STAT_GOLD, nGive);
	return nLeft;
}

void __fastcall PartySendLives(GAME *ptGame  , UNIT *hUnit
)
{
	PARTY *ptParty;
	PARTYMEMBER *ptMember;
	UNIT *hMember;
	CLIENT *ptClient;
	WORD wParty;

	ptClient = SUnitGetClient(hUnit);
	wParty = SUnitGetParty(hUnit);
	if (wParty == PARTY_NONE)
		return;
	ptParty = PartyFind(ptGame, wParty);
	if (!ptParty)
		return;
	for (ptMember = ptParty->pMembers; ptMember; ptMember = ptMember->pNext)
	{
		hMember = SUnitFind(ptGame, UNIT_PLAYER, ptMember->dwUnitId);
		if (hMember && hMember != hUnit)
			SCmdSendLife(ptClient, hMember);
	}
}

WORD __fastcall PartyGetId(GAME *ptGame  , UNIT *hPlayer
)
{
	PARTY *ptParty;
	PARTYMEMBER *ptMember;
	DWORD dwUnitId;

	dwUnitId = hPlayer->dwId;
	if (hPlayer->ptInventory)
		dwUnitId = D2CommonINVENTORYGetOwnerId(hPlayer->ptInventory);
#line 589
	D2ASSERT(UnitGetType (hPlayer) == UNIT_PLAYER);
#line 590
	D2ASSERT(ptGame);
	if (ptGame->hParty)
		for (ptParty = ptGame->hParty->pFirst; ptParty; ptParty = ptParty->pNext)
			for (ptMember = ptParty->pMembers; ptMember; ptMember = ptMember->pNext)
				if (ptMember->dwUnitId == dwUnitId)
					return ptParty->wId;
	return PARTY_NONE;
}
