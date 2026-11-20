#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\GAME\\Arena.cpp"
#include "../D2game.h"
#include "../StormNew.h"
#include "D2Macros.h"
#include "../D2gameImports.h"
#include "Arena.h"
#include "Game.h"
#include "Clients.h"
#include "SCmd.h"
#include "D2NetImports.h"

void __stdcall GameKickClient(DWORD dwClientId);

static __inline ARENA *ArenaGetChecked(GAME *ptGame)
{
	ARENA *ptArena;

	ptArena = ptGame->ptArena;
#line 37
	D2ASSERT(ptArena);
	return ptArena;
}

void __fastcall ArenaNew(GAME *ptGame, WORD wUnused, DWORD dwFlags,
	BYTE bType)
{
	ARENA *ptArena;
	int i;

#line 48
	ptArena = (ARENA *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(ARENA));
	for (i = 0; i < (int)(sizeof(ARENA) / sizeof(DWORD)); i++)
		((DWORD *)ptArena)[i] = 0;
	ptGame->ptArena = ptArena;
	ptArena->dwFlags = dwFlags & 96711;
	ptArena->bType = bType;
}

void __fastcall ArenaFree(GAME *ptGame)
{
	if (ptGame->ptArena)
	{
#line 68
		D2FREEPOOL(ptGame->pMemPool, ptGame->ptArena);
		ptGame->ptArena = NULL;
	}
}

void __fastcall ArenaUnitNew(GAME *ptGame, UNIT *hUnit)
{
	ARENAUNIT *ptArenaUnit;
	PLRINFO *ptPlayerInfo;
	int i;

#line 75
	ptArenaUnit = (ARENAUNIT *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(ARENAUNIT));
	for (i = 0; i < (int)(sizeof(ARENAUNIT) / sizeof(DWORD)); i++)
		((DWORD *)ptArenaUnit)[i] = 0;
	ptPlayerInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
#line 78
	D2ASSERT(ptPlayerInfo);
	ptPlayerInfo->ptArenaUnit = ptArenaUnit;
}

void __fastcall ArenaUnitFree(GAME *ptGame, UNIT *hUnit)
{
	PLRINFO *ptPlayerInfo;

	ptPlayerInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
#line 86
	D2ASSERT(ptPlayerInfo);
	if (ptPlayerInfo->ptArenaUnit)
	{
#line 91
		D2FREEPOOL(ptGame->pMemPool, ptPlayerInfo->ptArenaUnit);
		ptPlayerInfo->ptArenaUnit = NULL;
	}
}

void __fastcall ArenaUnitClear(GAME *ptGame, UNIT *hUnit)
{
	ARENAUNIT *ptArenaUnit;

#line 201
	D2ASSERT(UnitGetType ( hUnit ) == UNIT_PLAYER);
	ptArenaUnit = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->ptArenaUnit;
#line 205
	D2ASSERT(ptArenaUnit);
	ptArenaUnit->field_04 = 0;
}

void __fastcall ArenaClearTagSendFlag(GAME *ptGame)
{
	if (ptGame->ptArena)
		ptGame->ptArena->dwFlags &= ~1024;
}

extern DWORD ArenaUnitGetTagGate(UNIT *hUnit);
__inline DWORD ArenaUnitGetTagGate(UNIT *hUnit)
{
	ARENAUNIT *ptArenaUnit;

	ptArenaUnit = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->ptArenaUnit;
#line 223
	D2ASSERT(ptArenaUnit);
	return ptArenaUnit->field_04;
}

extern WORD ArenaUnitGetTag(UNIT *hUnit);
__inline WORD ArenaUnitGetTag(UNIT *hUnit)
{
	ARENAUNIT *ptArenaUnit;

	ptArenaUnit = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->ptArenaUnit;
#line 233
	D2ASSERT(ptArenaUnit);
	return ptArenaUnit->wSortKey;
}

void __fastcall ArenaSendTagsToClient(GAME *ptGame, CLIENT *ptClient)
{
	UNIT *hUnit;
	UNIT *hOwnUnit;
	CLIENT *ptOther;

	if (!ptGame->ptArena || !(ptGame->ptArena->dwFlags & 1024))
		return;
	hUnit = ClientGetPlayerUnit(ptClient, FALSE);
	if (hUnit && ArenaUnitGetTagGate(hUnit))
		SCmdSendArenaTag(ptClient, hUnit->dwId, ArenaUnitGetTag(hUnit));
	if (!ClientTestSeesOtherTags(ptClient))
		return;
	hOwnUnit = ClientGetPlayerUnit(ptClient, FALSE);
	for (ptOther = ptGame->ptClientList; ptOther; ptOther = ptOther->ptNextInGame)
	{
		hUnit = ClientGetPlayerUnit(ptOther, FALSE);
		if (ptOther->dwState == 4 && hUnit && hUnit != hOwnUnit && ArenaUnitGetTagGate(hUnit))
			SCmdSendArenaTag(ptClient, hUnit->dwId, ArenaUnitGetTag(hUnit));
	}
}

void __fastcall ArenaSendTagsOnJoin(GAME *ptGame, CLIENT *ptClient)
{
	UNIT *hUnit;
	CLIENT *ptOther;

	ClientGetPlayerUnit(ptClient, FALSE);
	for (ptOther = ptGame->ptClientList; ptOther; ptOther = ptOther->ptNextInGame)
	{
		hUnit = ClientGetPlayerUnit(ptOther, FALSE);
		if (ptOther->dwState == 4 && hUnit)
			SCmdSendArenaTag(ptClient, hUnit->dwId, ArenaUnitGetTag(hUnit));
	}
	hUnit = ClientGetPlayerUnit(ptClient, TRUE);
	if (ptClient->dwState != 4 && hUnit)
		SCmdSendArenaTag(ptClient, hUnit->dwId, ArenaUnitGetTag(hUnit));
}

DWORD __fastcall ArenaTestRequiredFlag(GAME *ptGame)
{
	ARENA *ptArena;

	ptArena = ArenaGetChecked(ptGame);
	if (ptArena->dwFlags & 4)
		return TRUE;
	return FALSE;
}

DWORD __fastcall ArenaTestNoActChange(GAME *ptGame)
{
	ARENA *ptArena;

	ptArena = ArenaGetChecked(ptGame);
	if (ptArena->dwFlags & 2)
		return TRUE;
	return FALSE;
}

DWORD __fastcall ArenaHasGuild(GAME *ptGame)
{
	ARENA *ptArena;

	ptArena = ArenaGetChecked(ptGame);
	return (ptArena->dwFlags >> 16) & 1;
}

DWORD __fastcall ArenaGetFlags(GAME *ptGame)
{
	ARENA *ptArena;

	ptArena = ptGame->ptArena;
#line 317
	D2ASSERT(ptArena);
	return ptArena->dwFlags;
}

BOOL __fastcall ArenaUnitAlwaysFalse(GAME *ptGame, UNIT *hUnit)
{
	return FALSE;
}

DWORD __fastcall ArenaTestCharTemplate(GAME *ptGame)
{
	ARENA *ptArena;

	ptArena = ArenaGetChecked(ptGame);
	return (ptArena->dwFlags >> 8) & 1;
}

DWORD __fastcall ArenaGetType(GAME *ptGame)
{
	ARENA *ptArena;

	ptArena = ptGame->ptArena;
#line 374
	D2ASSERT(ptArena);
	return ptArena->bType;
}

BOOL __fastcall ArenaStringIsTerminated(const char *psz, int nMax)
{
	int i;

	for (i = 0; i < nMax; i++)
		if (!*psz++)
			return TRUE;
	return FALSE;
}

void __fastcall ArenaSrvMsgDispatch(SRVMSG *ptMsg, DWORD dwLength)
{
	DWORD dwClientId;
	GAME *ptGame;
	DWORD dwGame;
	DWORD dwError;
	DWORD dwFull;
	DWORD dwResult;
	char szName[16];

	dwClientId = ptMsg->dwClientId;
	ptMsg = (SRVMSG *)&ptMsg->uPacket;
	switch (*(BYTE *)ptMsg)
	{
	case 0:
	{
		SRVPACKET *ptPacket = (SRVPACKET *)ptMsg;
		if (!GameCheckCreatePacket(dwClientId, ptPacket))
			break;
		GameCreateFromPacket(dwClientId, ptPacket->tCreate.szGameName, ptPacket->tCreate.bGameType,
			ptPacket->tCreate.bClass, ptPacket->tCreate.szPlrName, ptPacket->tCreate.wArena,
			ptPacket->tCreate.dwFlags, ptPacket->tCreate.bArenaType, ptPacket->tCreate.bField2b,
			ptPacket->tCreate.bField2c, ptPacket->tCreate.bDifficulty);
		return;
	}
	case 6:
	{
		SRVPACKET *ptPacket = (SRVPACKET *)ptMsg;
		GameReceivePing(dwClientId, ptPacket->t6.dwTick, ptPacket->t6.dwDelay);
		return;
	}
	case 1:
	{
		SRVPACKET *ptPacket = (SRVPACKET *)ptMsg;
		if (!ptEventCallbackTable)
		{
			DWORD dwField08 = ptPacket->tJoin.dwField08;
			DWORD dwGameId = ptPacket->tJoin.wGameId;

			dwFull = 0;
			if (dwGameId != 1)
				dwError = 6;
			else if (!(dwGame = GameFindById(dwGameId)))
				dwError = 6;
			else if (!(ptGame = GameLockByHandle(dwGame)))
				dwError = 6;
			else if (dwField08)
				dwError = 16;
			else
			{
				if (ptGame->nNumClients >= 8)
					dwFull = 15;
				GameUnlock(ptGame);
				dwError = dwFull;
			}
			if (dwError)
			{
				SCmdSendJoinError(dwClientId, dwError);
				D2NetServerDisconnect(dwClientId);
			}
		}
		szName[0] = 0;
		if (!GameJoin(dwClientId, ptPacket->tJoin.wGameId, ptPacket->tJoin.bClass, ptPacket->tJoin.abData,
			ptPacket->tJoin.dwToken, szName, &dwResult))
			break;
		GameJoinDone(dwClientId, ptPacket->tJoin.wGameId, ptPacket->tJoin.bClass, ptPacket->tJoin.abData,
			szName, dwResult);
		return;
	}
	case 2:
		if (GameValidateEndGame(dwClientId))
			GameOnEndGame(dwClientId, 1);
		return;
	case 3:
		if (!GameReturnTrue(dwClientId))
			break;
		GameSendGameList(dwClientId);
		return;
	case 4:
		if (!GameValidateJoinAct(dwClientId))
			break;
		GameOnJoinAct(dwClientId);
		return;
	case 5:
	{
		SRVPACKET *ptPacket = (SRVPACKET *)ptMsg;
		if (!GameReturnTrue(dwClientId))
			break;
		GameReceiveDataChunk(dwClientId, ptPacket);
		return;
	}
	case 7:
		if (GameValidateDisconnect(dwClientId))
		{
			GameOnDisconnect(dwClientId, 0);
			return;
		}
		break;
	case 9:
		D2gameGameKickClient(dwClientId);
		return;
	default:
		return;
	}
	GameDropClient(dwClientId);
}
