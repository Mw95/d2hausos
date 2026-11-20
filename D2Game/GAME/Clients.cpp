#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\GAME\\Clients.cpp"
#include <stdio.h>
#include "../D2game.h"
#include "D2Macros.h"
#include "Arena.h"
#include "Clients.h"
#include "Level.h"
#include "SCmd.h"
#include "../INVENTORY/InvMode.h"
#include "../ITEMS/ItemMode.h"
#include "Game.h"
#include "../PLAYER/PlrMsg.h"
#include "../PLAYER/Player.h"
#include "../PLAYER/PlayerPets.h"
#include "../PLAYER/PlrSave.h"
#include "../PLAYER/PlrModes.h"
#include "../PLAYER/PlrTrade.h"
#include "../QUESTS/QUESTS.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/sunitproxy.h"
#include "../UNIT/SUNITNPC.h"
#include "../GUILDS/Guilds.h"
#include "FogImports.h"

static CLIENTBUCKETS sgptGlobalClient = { 0 };
static SAVEHEADER sgtSummaryHeaderGfx = { 0 };
static CRITICAL_SECTION sgtGlobalClientByNameLock = { 0 };
static SAVEHEADER sgtSummaryHeaderColors = { 0 };
static CLIENTBUCKETS sgptGlobalClientByName = { 0 };
static CRITICAL_SECTION sgtGlobalClientLock = { 0 };

struct CLIENTCOORD
{
	int		nX;
	int		nY;
};

void __fastcall ClientChangeRoom(CLIENT *ptClient, GAMEROOM *hRoomNew);
BYTE __fastcall ClientGetAct(CLIENT *ptClient);
void __fastcall ClientSetState(CLIENT *ptClient, DWORD dwState);

void __fastcall ClientsInitLists(void)
{
	int i;

	InitializeCriticalSection(&sgtGlobalClientLock);
	EnterCriticalSection(&sgtGlobalClientLock);
	for (i = 0; i < CLIENT_NUMBUCKETS; i++)
		sgptGlobalClient[i] = NULL;
	LeaveCriticalSection(&sgtGlobalClientLock);
	InitializeCriticalSection(&sgtGlobalClientByNameLock);
	EnterCriticalSection(&sgtGlobalClientByNameLock);
	for (i = 0; i < CLIENT_NUMBUCKETS; i++)
		sgptGlobalClientByName[i] = NULL;
	LeaveCriticalSection(&sgtGlobalClientByNameLock);
}

void __fastcall ClientsTermLists(void)
{
	DeleteCriticalSection(&sgtGlobalClientLock);
	DeleteCriticalSection(&sgtGlobalClientByNameLock);
}

CLIENT * __fastcall ClientsFindInGame(GAME *ptGame, DWORD dwClientId)
{
	CLIENT *ptClient;

#line 82
	D2ASSERT(ptGame);
	for (ptClient = ptGame->ptClientList; ptClient; ptClient = ptClient->ptNextInGame)
		if (ptClient->dwClientId == dwClientId)
			return ptClient;
	return NULL;
}

UNIT * __stdcall ClientGetPlayerUnit(CLIENT *ptClient, BOOL bNoCheck)
{
	UNIT *hUnit;

#line 100
	D2ASSERT(ptClient);
	if (ptClient->dwFlags & 1)
		return ptClient->hPlayerUnit;
	if (bNoCheck)
		return ptClient->hPlayerUnit;
	if (!ptClient->hPlayerUnit)
		return NULL;
	hUnit = SUnitFind(ptClient->ptGame, ptClient->dwUnitType, ptClient->dwUnitId);
	if (!hUnit)
		ptClient->hPlayerUnit = hUnit;
	return hUnit;
}

void __fastcall ClientSetPlayerUnit(CLIENT *ptClient, UNIT *hUnit)
{
#line 124
	D2ASSERT(ptClient);
	if (hUnit)
	{
		ptClient->dwUnitType = hUnit->eType;
		ptClient->dwUnitId = hUnit->dwId;
		ptClient->hPlayerUnit = hUnit;
	}
	else
		ptClient->hPlayerUnit = NULL;
}

void __fastcall ClientPlacePlayer(CLIENT *ptClient, UNIT *hUnit, GAME *ptGame, GAMEROOM *hRoom,
	int nX, int nY)
{
	BYTE bAct;
	int nRoomX;
	int nRoomY;
	DWORD dwLevelId;
	void *pArenaStart;
	GAMERECT atRect[2];
	void *pNode;
	DWORD dwKey;
	DWORD dwField;
	UNIT *hOther;
	CLIENTCOORD tCoord;
	int nTownX;
	int nTownY;

#line 146
	D2ASSERT(hUnit);
#line 147
	D2ASSERT(ptGame);
	bAct = ClientGetAct(ptClient);
	if (!hRoom)
	{
		pArenaStart = GuildsGetData(ptGame);
		if (pArenaStart)
			dwLevelId = D2CommonGuildsGetRankLevelId(pArenaStart);
		else
			dwLevelId = D2CommonDungeonActGetInitialLevelId(ptGame->ptAct[bAct]);
		hRoom = (GAMEROOM *)D2CommonDungeonGetActStartRoom(ptGame->ptAct[bAct], dwLevelId, 0, &nRoomX,
			&nRoomY, D2CommonUnitGetSizeX(hUnit));
#line 166
		D2ASSERT(hRoom);
		D2CommonDungeonRoomCopyRects(hRoom, atRect);
		SCmdSendRoom(ptClient, D2CommonDungeonRoomGetLevelId(hRoom), atRect[1].nX, atRect[1].nY);
	}
	else
	{
		nRoomX = nX;
		nRoomY = nY;
	}
	if (hRoom != ptClient->hRoom)
		ClientChangeRoom(ptClient, hRoom);
	ptClient->hRoom = hRoom;
	SUnitPlace(hUnit, nRoomX, nRoomY, ptGame, hRoom, 0);
	ptClient->dwFlags &= ~1;
	SCmdSendUnit(ptClient, S2C_UNIT_WARP, (BYTE)hUnit->eType, hUnit->dwId, nRoomX, nRoomY, 1);
	SCmdSendMonRegionSeed(ptClient, ptGame->dwMonRegionSeed);
	for (pNode = D2CommonINVENTORYGetTrackedListHead(hUnit->ptInventory); pNode; )
	{
		dwKey = D2CommonINVENTORYNodeGetTrackedKey(pNode);
		if (!D2CommonINVENTORYNodeGetTrackedFlag(pNode))
		{
			dwField = D2CommonINVENTORYNodeGetTrackedData(pNode);
			pNode = (void *)D2CommonINVENTORYNodeGetTrackedNext(pNode);
			D2CommonINVENTORYRemoveTrackedNode(hUnit->ptInventory, dwKey, 0);
			if (dwKey && (WORD)dwField)
				SUnitNpcRestoreHire(ptGame, hUnit, dwField, dwKey);
		}
		else
		{
			hOther = SUnitFind(ptGame, UNIT_PLAYER, dwKey);
			if (hOther)
			{
				dwLevelId = D2CommonDungeonActGetTownLevelId(bAct);
				nTownX = 0;
				nTownY = 0;
				hRoom = (GAMEROOM *)D2CommonDungeonGetActStartRoom(ptGame->ptAct[bAct], dwLevelId, 10,
					&nTownX, &nTownY, D2CommonUnitGetSizeX(hOther));
				tCoord.nX = nTownX;
				tCoord.nY = nTownY;
				PlayerPlaceDeadBody(ptGame, hRoom, dwKey, (COLPOINT *)&tCoord);
			}
			pNode = (void *)D2CommonINVENTORYNodeGetTrackedNext(pNode);
		}
	}
}

static __inline DWORD ClientsUnitType(UNIT *hUnit) { return hUnit->eType; }
int __fastcall ClientLoadCharacter(CLIENT *ptClient, GAME *ptGame, DWORD dwArg)
{
	UNIT *hUnit;
	int nError;
	int i;

	hUnit = NULL;
#line 232
	D2ASSERT(ptGame);
#line 235
	D2ASSERT(ptClient);
	ptClient->dwFlags |= 1;
	nError = PlrSaveLoadCharacter(ptGame, ptClient, ptClient->szName, dwArg, &hUnit);
	if (nError)
		return nError;
	if (ptClient->wFlags & 4)
	{
		if (ptClient->wFlags & 8)
			return 21;
		if (!(ArenaGetFlags(ptGame) & 2048))
			return 20;
	}
	else if (ArenaGetFlags(ptGame) & 2048)
		return 19;
	SCmdSendUnitTypeId(ptClient, S2C_SET_CONTROL_UNIT, ClientsUnitType(hUnit), hUnit->dwId);
	for (i = 0; i < 8; i++)
		if ((int)ptClient->abHotkeys[i][0] != -1)
			SCmdSendPair(ptClient, i, ptClient->abHotkeys[i][0], ptClient->abHotkeys[i][1]);
	SCmdSendDword(ptClient, S2C_CMD_H58, D2CommonUnitGetPlayerLevelBits(hUnit));
	if (hUnit->dwFlags & 1)
	{
		INVENTORYModeSendItems(ptGame, hUnit, ptClient, NULL);
		hUnit->dwFlags |= 2097152;
	}
	PlrMsgSendLifeMana(hUnit, hUnit);
	INVENTORYModeResetItems(ptGame, hUnit);
	return 0;
}

CLIENT * __fastcall ClientsNew(GAME *ptGame, DWORD dwClientId, BYTE bClass,
	const char *szName, const char *szAccount,
	DWORD dwTokenResult)
{
	CLIENT *ptClient;
	int i;
	int nKey;

#line 301
	D2ASSERT(ptGame);
	if (ptGame->nNumClients < 8)
	{
#line 311
		ptClient = (CLIENT *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(CLIENT));
		for (i = 0; i < (int)(sizeof(CLIENT) / sizeof(DWORD)); i++)
			((DWORD *)ptClient)[i] = 0;
		ptClient->dwClientId = dwClientId;
		ptClient->dwTick = GetTickCount() + 180000;
		ptClient->ptGame = ptGame;
		if (ArenaTestCharTemplate(ptGame))
		{
			ptClient->bCharTemplate = bClass;
			ptClient->bClass = (BYTE)D2CommonArenaGetCharTemplateClass(bClass, ArenaGetType(ptGame));
		}
		else
			ptClient->bClass = bClass;
		StormSStrCopy(ptClient->szName, szName, 2147483647);
		StormSStrCopy(ptClient->szAccount, szAccount, 2147483647);
		ptClient->dwTokenResult = dwTokenResult;
		ClientSetState(ptClient, 0);
		ptClient->dwQueueHead = 0;
		ptClient->dwQueueTail = 0;
		ptClient->ptFreeNodes = NULL;
		for (i = 0; i < 8; i++)
		{
			ptClient->abHotkeys[i][0] = 255;
			ptClient->abHotkeys[i][1] = 0;
		}
		ptClient->ptNextInGame = ptGame->ptClientList;
		ptGame->ptClientList = ptClient;
		nKey = (BYTE)dwClientId;
		EnterCriticalSection(&sgtGlobalClientLock);
		ptClient->ptSrvNext = sgptGlobalClient[nKey];
		sgptGlobalClient[nKey] = ptClient;
		LeaveCriticalSection(&sgtGlobalClientLock);
		nKey = (BYTE)FogStringPackHash(szName);
		EnterCriticalSection(&sgtGlobalClientByNameLock);
		ptClient->ptSrvByNameNext = sgptGlobalClientByName[nKey];
		sgptGlobalClientByName[nKey] = ptClient;
		LeaveCriticalSection(&sgtGlobalClientByNameLock);
		ptGame->nNumClients++;
		return ptClient;
	}
	return NULL;
}

void __fastcall ClientsInitGame(GAME *ptGame)
{
	ptGame->nNumClients = 0;
	ptGame->ptClientList = NULL;
}

extern void __fastcall ClientWriteLook(UNIT *hUnit, SAVEHEADER *ptHeader);
__inline void __fastcall ClientWriteLook(UNIT *hUnit, SAVEHEADER *ptHeader)
{
	D2CommonINVENTORYWriteSaveHeaderItems(hUnit, ptHeader);
}

void __fastcall ClientUpdateSummary(CLIENT *ptClient, CLIENTSUMMARY *ptSummary)
{
	UNIT *hUnit;
	BYTE *pbSrc;
	BYTE *pbOut;
	WORD wFlags;
	BYTE bEmblemIcon;
	BYTE bEmblemColor1;
	BYTE bEmblemColor2;
	int i;

	pbOut = ptSummary->abField00;
	FogD2QSSocketWriteSevenBitPair(pbOut, 0);
	pbOut += 2;
	hUnit = ClientGetPlayerUnit(ptClient, FALSE);
	if (!hUnit)
		return;
	for (i = 0; i < 4; i++)
		((DWORD *)sgtSummaryHeaderGfx.abComponentGfx)[i] = 0xFFFFFFFF;
	ClientWriteLook(hUnit, &sgtSummaryHeaderGfx);
	pbSrc = sgtSummaryHeaderGfx.abComponentGfx;
	if (pbSrc)
	{
		StormSMemCopy(pbOut, pbSrc, 16);
		pbOut[16] = (BYTE)hUnit->dwClassId + 1;
	}
	for (i = 0; i < 4; i++)
		((DWORD *)sgtSummaryHeaderColors.abComponentColors)[i] = 0xFFFFFFFF;
	ClientWriteLook(hUnit, &sgtSummaryHeaderColors);
	pbSrc = sgtSummaryHeaderColors.abComponentColors;
	if (pbSrc)
		StormSMemCopy((pbOut + 17), pbSrc, 16);
	pbOut[33] = (BYTE)D2CommonStatsUnitGetValue(hUnit, 12);
	wFlags = ptClient->wFlags;
	if ((hUnit->dwMode == 17 || hUnit->dwMode == 0) && (wFlags & 4))
		wFlags |= 8;
	else
		wFlags &= ~8;
	((void (__stdcall *)(BYTE *, WORD))FogOrdinal10064)(pbOut + 34, wFlags);
	((void (__stdcall *)(BYTE *, WORD))FogOrdinal10064)(pbOut + 36, ptClient->wGuildFlags);
	bEmblemIcon = ptClient->abGuildEmblem[0];
	bEmblemColor1 = ptClient->abGuildEmblem[1];
	bEmblemColor2 = ptClient->abGuildEmblem[2];
	if (!bEmblemIcon)
		bEmblemIcon = 255;
	if (!bEmblemColor1)
		bEmblemColor1 = 255;
	if (!bEmblemColor2)
		bEmblemColor2 = 255;
	pbOut[38] = bEmblemIcon;
	pbOut[39] = bEmblemColor1;
	pbOut[40] = bEmblemColor2;
	*(DWORD *)(pbOut + 41) = ptClient->dwGuildTag;
}

void __fastcall ClientsRemove(GAME *ptGame, DWORD dwClientId)
{
	char szName[16];
	int nKey;
	CLIENT *ptSrvClient;
	CLIENT *ptPrev;
	CLIENT *ptClient;
	UNIT *hUnit;
	GAMEROOM *hRoom;
	__int64 nExperience;
	WORD wClass;
	DWORD dwLevel;
	CLIENTNODE178 *ptNode;

#line 469
	D2ASSERT(ptGame);
	nKey = (BYTE)dwClientId;
	szName[0] = 0;
	EnterCriticalSection(&sgtGlobalClientLock);
	ptSrvClient = sgptGlobalClient[nKey];
	if (ptSrvClient)
	{
		if (ptSrvClient->dwClientId == dwClientId)
		{
			strcpy(szName, ptSrvClient->szName);
			sgptGlobalClient[nKey] = ptSrvClient->ptSrvNext;
		}
		else
		{
			while (ptSrvClient->ptSrvNext->dwClientId != dwClientId)
				ptSrvClient = ptSrvClient->ptSrvNext;
#line 487
			D2ASSERT(ptSrvClient->ptSrvNext);
			strcpy(szName, ptSrvClient->ptSrvNext->szName);
			ptSrvClient->ptSrvNext = ptSrvClient->ptSrvNext->ptSrvNext;
		}
	}
	LeaveCriticalSection(&sgtGlobalClientLock);
	ptClient = NULL;
	ptPrev = ptGame->ptClientList;
	if (ptPrev)
	{
		if (ptPrev->dwClientId == dwClientId)
		{
			ptClient = ptPrev;
			ptGame->ptClientList = ptClient->ptNextInGame;
		}
		else
		{
			for (; ptPrev->ptNextInGame->dwClientId != dwClientId;
				ptPrev = ptPrev->ptNextInGame)
				;
			ptClient = ptPrev->ptNextInGame;
			ptPrev->ptNextInGame = ptClient->ptNextInGame;
		}
	}
	nKey = (BYTE)FogStringPackHash(szName);
	EnterCriticalSection(&sgtGlobalClientByNameLock);
	ptSrvClient = sgptGlobalClientByName[nKey];
#line 513
	D2ASSERT(sgptGlobalClientByName[nKey]);
	if (!strcmp(ptSrvClient->szName, szName))
		sgptGlobalClientByName[nKey] = ptSrvClient->ptSrvByNameNext;
	else
	{
#line 522
		D2ASSERT(ptSrvClient->ptSrvByNameNext);
		while (strcmp(ptSrvClient->ptSrvByNameNext->szName, szName))
		{
			ptSrvClient = ptSrvClient->ptSrvByNameNext;
#line 526
			D2ASSERT(ptSrvClient->ptSrvByNameNext);
		}
		ptSrvClient->ptSrvByNameNext = ptSrvClient->ptSrvByNameNext->ptSrvByNameNext;
	}
	LeaveCriticalSection(&sgtGlobalClientByNameLock);
	if (!ptClient)
		return;
	GameSendRosterRemoveToAll(ptGame, ptClient);
	hUnit = ClientGetPlayerUnit(ptClient, FALSE);
	if (hUnit)
	{
		nExperience = D2CommonStatsUnitGetBaseValue(hUnit, 13);
		wClass = (WORD)hUnit->dwClassId;
		dwLevel = D2CommonStatsUnitGetBaseValue(hUnit, 12);
		PartyRemovePlayer(ptGame, hUnit);
		PlayerPetsFree(ptGame, hUnit);
		ClientUpdateSummary(ptClient, &ptClient->uSummary.tSummary);
		SUnitProxyPlayerLeft(ptGame, hUnit);
		QuestsPlayerDropped(ptGame, hUnit);
		PlrTradeLeave(ptGame, hUnit);
		hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
		if (hRoom)
		{
			LevelUnitNotifyNear(ptGame, hRoom, hUnit);
			D2CommonDungeonRoomAddNode(hRoom, hUnit->eType, hUnit->dwId);
		}
		SUnitFree(ptClient->ptGame, hUnit);
		if (ptEventCallbackTable)
		{
#line 580
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)ptEventCallbackTable->fpLeaveGame), "ptEventCallbackTable->fpLeaveGame");
			if (ptEventCallbackTable->fpLeaveGame)
				ptEventCallbackTable->fpLeaveGame(ptGame->wGameId, wClass, dwLevel, nExperience,
					ptClient->wFlags,
					ptClient->szName, ptClient->uSummary.abSummary);
		}
	}
	else if (ptEventCallbackTable)
	{
		ptClient->uSummary.abSummary[0] = 0;
#line 593
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)ptEventCallbackTable->fpLeaveGame), "ptEventCallbackTable->fpLeaveGame");
		if (ptEventCallbackTable->fpLeaveGame)
			ptEventCallbackTable->fpLeaveGame(ptGame->wGameId, 0, 0, 0, 0, ptClient->szName,
				ptClient->uSummary.abSummary);
	}
	if (ptClient->hRoom)
		LevelRemoveClientNear(ptClient->hRoom, ptClient);
	if (ptClient->pbBuffer)
	{
#line 604
		D2FREEPOOL(ptClient->ptGame->pMemPool, ptClient->pbBuffer);
	}
	if (ptEventCallbackTable)
	{
#line 609
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)ptEventCallbackTable->fpServerLogMessage), "ptEventCallbackTable->fpServerLogMessage");
		if (ptEventCallbackTable->fpServerLogMessage)
			ptEventCallbackTable->fpServerLogMessage(0, "[SERVER]  Disconnect: %s", ptClient->szName);
	}
	while (ptClient->ptFreeNodes)
	{
		ptNode = ptClient->ptFreeNodes;
		ptClient->ptFreeNodes = ptNode->ptNext;
#line 619
		D2FREEPOOL(NULL, ptNode);
	}
#line 622
	D2FREEPOOL(ptClient->ptGame->pMemPool, ptClient);
	ptGame->nNumClients--;
}

void __fastcall ClientsFreeAll(GAME *ptGame)
{
	CLIENT *ptClient;
	CLIENT *ptNext;

	for (ptNext = ptGame->ptClientList; ptNext; )
	{
		ptClient = ptNext;
		ptNext = ptClient->ptNextInGame;
#line 636
		D2FREEPOOL(ptGame->pMemPool, ptClient);
	}
}

void __fastcall ClientChangeRoom(CLIENT *ptClient, GAMEROOM *hRoomNew)
{
	GAMEROOM *hRoomOld;
	GAMEROOM **phOldNear;
	DWORD dwOldNear;
	GAMEROOM **phNewNear;
	DWORD dwNewNear;
	DWORD i;
	DWORD j;
	DWORD dwLevelId;
	DWORD dwBits;

#line 661
	D2ASSERT(ptClient);
#line 662
	D2ASSERT(ptClient->ptGame);
	hRoomOld = ptClient->hRoom;
	phNewNear = NULL;
	phOldNear = NULL;
	dwNewNear = 0;
	dwOldNear = 0;
#line 674
	D2ASSERT(hRoomOld != hRoomNew);
	D2CommonDungeonRoomSwitch(hRoomOld, hRoomNew);
	D2CommonDungeonRoomChangeLevel(hRoomOld, hRoomNew);
	if (hRoomNew)
		D2CommonDungeonRoomGetNear(hRoomNew, &phNewNear, &dwNewNear);
	if (hRoomOld)
		D2CommonDungeonRoomGetNear(hRoomOld, &phOldNear, &dwOldNear);
	for (i = 0; i < dwNewNear; i++)
	{
		for (j = 0; j < dwOldNear; j++)
			if (phNewNear[i] == phOldNear[j])
				break;
		if (j >= dwOldNear)
			LevelRoomEnterView(ptClient->ptGame, phNewNear[i], ptClient);
	}
	for (i = 0; i < dwOldNear; i++)
	{
		for (j = 0; j < dwNewNear; j++)
			if (phOldNear[i] == phNewNear[j])
				break;
		if (j >= dwNewNear)
		{
			LevelRoomLeaveView(ptClient->ptGame, phOldNear[i], ptClient);
			if (ptClient->hRoom == phOldNear[i])
				PlrModeSendUnit(ptClient->ptGame, ClientGetPlayerUnit(ptClient, FALSE), ptClient, FALSE);
		}
	}
	ptClient->hRoom = hRoomNew;
	if (hRoomNew && ptClient->hPlayerUnit)
	{
		dwLevelId = D2CommonDungeonRoomGetLevelId(hRoomNew);
		dwBits = D2CommonUnitGetPlayerLevelBits(ptClient->hPlayerUnit);
		dwBits |= D2CommonDungeonGetPortalLevelBit(dwLevelId);
		D2CommonUnitSetPlayerLevelBits(ptClient->hPlayerUnit, dwBits);
	}
}

struct CLIENTNEARENTRY
{
	UNIT		*hUnit;
	DWORD		dwDistance;
	DWORD		nNext;
};

static __inline int ClientsAbs(int n)
{
	if (n < 0)
		return -n;
	else
		return n;
}

void __fastcall ClientUpdateNearPlayers(CLIENT *ptClient, DWORD dwMaxNear)
{
	GAMEROOM **phRoom;
	DWORD dwNumRooms;
	UNIT *hUnit;
	UNIT *hNear;
	int nX;
	int nY;
	DWORD dwDistance;
	DWORD dwMaxDistance;
	DWORD dwCount;
	DWORD nPrev;
	DWORD nSlot;
	DWORD nBefore;
	DWORD nIndex;
	DWORD i;
	CLIENTNEARENTRY atEntry[55];

	D2CommonDungeonRoomGetNear(ptClient->hRoom, &phRoom, &dwNumRooms);
#line 844
	D2ASSERT(phRoom);
	hUnit = ClientGetPlayerUnit(ptClient, FALSE);
	if (!hUnit)
		return;
	nX = D2CommonUnitGetX(hUnit);
	nY = D2CommonUnitGetY(hUnit);
	dwMaxDistance = 0;
	ptClient->nNear = 0;
	atEntry[0].nNext = 0;
	atEntry[0].dwDistance = 0;
	for (i = 0; i < dwNumRooms; i++)
	{
		for (hNear = phRoom[i]->ptUnitFirst; hNear; hNear = hNear->ptRoomNext)
		{
			if (hNear->eType != UNIT_PLAYER)
				continue;
			dwDistance = ClientsAbs(nX - (int)D2CommonUnitGetX(hNear))
				+ ClientsAbs(nY - (int)D2CommonUnitGetY(hNear));
			dwCount = ptClient->nNear;
			if (dwCount >= dwMaxNear && dwDistance >= dwMaxDistance)
				continue;
			nPrev = 0;
			while (atEntry[nPrev].nNext)
			{
				if (dwDistance <= atEntry[atEntry[nPrev].nNext].dwDistance)
					break;
				nPrev = atEntry[nPrev].nNext;
			}
			if (dwCount == dwMaxNear)
			{
				nBefore = 0;
				while (atEntry[atEntry[nBefore].nNext].nNext)
					nBefore = atEntry[nBefore].nNext;
				nSlot = atEntry[nBefore].nNext;
				if (dwCount && dwDistance >= atEntry[nSlot].dwDistance)
					continue;
				if (dwCount == 1)
					dwMaxDistance = dwDistance;
				else
					dwMaxDistance = atEntry[nBefore].dwDistance;
				atEntry[nBefore].nNext = 0;
			}
			else
			{
				nSlot = dwCount;
				ptClient->nNear = dwCount + 1;
			}
			if (!nPrev && dwDistance < atEntry[0].dwDistance)
			{
				if (ptClient->nNear - 1)
				{
					atEntry[nSlot].nNext = atEntry[0].nNext;
					atEntry[nSlot].hUnit = atEntry[0].hUnit;
					atEntry[nSlot].dwDistance = atEntry[0].dwDistance;
				}
				atEntry[0].nNext = nSlot;
				nSlot = 0;
			}
			else
			{
				atEntry[nSlot].nNext = atEntry[nPrev].nNext;
				atEntry[nPrev].nNext = nSlot;
			}
			atEntry[nSlot].dwDistance = dwDistance;
			atEntry[nSlot].hUnit = hNear;
			if (dwDistance > dwMaxDistance)
				dwMaxDistance = dwDistance;
		}
	}
	nIndex = 0;
	ptClient->nNear = 0;
	do
	{
		hNear = atEntry[nIndex].hUnit;
		ptClient->atNear[ptClient->nNear].bType = (BYTE)hNear->eType;
		ptClient->atNear[ptClient->nNear].dwUnitId = hNear->dwId;
		ptClient->atNear[ptClient->nNear].wX = (WORD)D2CommonUnitGetX(hNear);
		ptClient->atNear[ptClient->nNear].wY = (WORD)D2CommonUnitGetY(hNear);
		ptClient->nNear++;
		nIndex = atEntry[nIndex].nNext;
	}
	while (atEntry[nIndex].nNext);
}

void __fastcall ClientUpdateRoom(GAME *ptGame, CLIENT *ptClient)
{
	GAMEROOM *hRoom;
	GAMEROOM *hPathRoom;
	UNIT *hUnit;
	DWORD dwLevelOld;
	DWORD dwLevelNew;

#line 880
	D2ASSERT(ptGame);
#line 881
	D2ASSERT(ptClient);
	hRoom = ptClient->hRoom;
	hUnit = ClientGetPlayerUnit(ptClient, FALSE);
	if (!hUnit)
		return;
	hPathRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	LevelSendNearRemovals(hRoom, ptClient);
	LevelSendNearUnits(ptGame, hRoom, ptClient);
	D2CommonStatsUnitCallChangedAll(hUnit, hUnit, PlrMsgStatChanged);
	if (hUnit->dwFlags & 2097152)
		ITEMSModeCheckEquipment(ptGame, hUnit, TRUE);
	ptClient->dwUpdateFrames++;
	if (hRoom != hPathRoom)
	{
		dwLevelOld = D2CommonDungeonRoomGetLevelId(hRoom);
		dwLevelNew = D2CommonDungeonRoomGetLevelId(hPathRoom);
		if (dwLevelOld != dwLevelNew)
		{
			QuestsLevelChange(ptGame, dwLevelOld, dwLevelNew, hUnit);
			SUnitProxyChangeLevel(ptGame, hUnit, dwLevelOld, dwLevelNew);
		}
		ClientChangeRoom(ptClient, hPathRoom);
	}
	ArenaSendTagsToClient(ptGame, ptClient);
}

BOOL __fastcall ClientsIsInGame(GAME *ptGame, DWORD dwClientId)
{
	CLIENT *ptClient;

	for (ptClient = ptGame->ptClientList; ptClient; ptClient = ptClient->ptNextInGame)
		if (ptClient->dwClientId == dwClientId)
			return TRUE;
	return FALSE;
}

void __fastcall ClientSetRoom(CLIENT *ptClient, GAMEROOM *hRoom)
{
#line 958
	D2ASSERT(ptClient);
	if (hRoom != ptClient->hRoom)
		ClientChangeRoom(ptClient, hRoom);
	ptClient->hRoom = hRoom;
}

UNIT * __fastcall ClientFindLifeManaUnit(GAME *ptGame, CLIENT *ptClient)
{
	UNIT *hUnit;

#line 984
	D2ASSERT(ptClient);
	if (!(ptClient->dwFlags & 2))
		return NULL;
	hUnit = SUnitFind(ptGame, ptClient->dwField150Type, ptClient->dwField150Id);
	if (hUnit && !ClientSeesUnit(hUnit, ptClient))
		hUnit = NULL;
	if (!hUnit)
		ptClient->dwFlags &= ~2;
	return hUnit;
}

void __fastcall ClientSetLifeManaUnit(CLIENT *ptClient, UNIT *hUnit)
{
#line 1005
	D2ASSERT(ptClient);
	if (!hUnit)
	{
		ptClient->dwFlags &= ~2;
		return;
	}
	ptClient->dwField150Type = hUnit->eType;
	ptClient->dwField150Id = hUnit->dwId;
	ptClient->dwFlags |= 2;
}

BYTE __fastcall ClientGetAct(CLIENT *ptClient)
{
#line 1022
	D2ASSERT(ptClient);
	return ptClient->bAct;
}

void __fastcall ClientSetAct(CLIENT *ptClient, BYTE bAct)
{
#line 1033
	D2ASSERT(ptClient);
#line 1034
	D2ASSERT(bAct < NUM_ACTS);
	ptClient->bAct = bAct;
}

BYTE __fastcall ClientGetSaveAct(CLIENT *ptClient)
{
#line 1046
	D2ASSERT(ptClient);
	return ptClient->bSaveAct;
}

extern BOOL __fastcall ClientIsNearRoom(CLIENT *ptClient, GAMEROOM *hRoom);
__inline BOOL __fastcall ClientIsNearRoom(CLIENT *ptClient, GAMEROOM *hRoom)
{
	GAMEROOM **phClientRooms;
	DWORD nRooms;
	DWORD i;

	D2CommonDungeonRoomGetNear(ptClient->hRoom, &phClientRooms, &nRooms);
	for (i = 0; i < nRooms; i++)
	{
		D2ASSERT(phClientRooms[i]);
		if (hRoom == phClientRooms[i])
			return TRUE;
	}
	return FALSE;
}

DWORD __fastcall ClientGetId(CLIENT *ptClient)
{
#line 1134
	D2ASSERT(ptClient);
	return ptClient->dwClientId;
}

DWORD __fastcall ClientGetClass(CLIENT *ptClient)
{
#line 1146
	D2ASSERT(ptClient);
	return ptClient->bClass;
}

void __fastcall ClientSetClass(CLIENT *ptClient, DWORD dwClass)
{
	ptClient->bClass = (BYTE)dwClass;
}

void __fastcall ClientSetFlags(CLIENT *ptClient, DWORD dwFlags)
{
	ptClient->wFlags = (WORD)dwFlags;
}

DWORD __fastcall ClientGetFlags(CLIENT *ptClient)
{
	return ptClient->wFlags;
}

void __fastcall ClientChangeFlags(CLIENT *ptClient, WORD wMask, BOOL bSet)
{
	if (bSet)
		ptClient->wFlags |= wMask;
	else
		ptClient->wFlags &= ~wMask;
}

DWORD __fastcall ClientTestFlags(CLIENT *ptClient, WORD wMask)
{
	return ptClient->wFlags & wMask;
}

void __fastcall ClientsRecordStep(int nHigh, int nLow)
{
	char szAddress[100];
	BYTE bOctet1, bOctet2, bOctet3;
	BYTE bOctet4;
	BYTE bRecorded;
	BYTE bStep;

	bStep = 1;
	bOctet4 = 34;
	if (StormSRegLoadString("Diablo II", "Aux Battle.net", 0, szAddress, sizeof(szAddress)))
	{
		sscanf(szAddress, "%d.%d.%d.%d", (int *)&bOctet1, (int *)&bOctet2, (int *)&bOctet3,
			(int *)&bOctet4);
		if (bOctet4 & 16)
			bRecorded = 5;
		else if (bOctet4 & 8)
			bRecorded = 4;
		else if (bOctet4 & 64)
			bRecorded = 3;
		else if (bOctet4 & 4)
			bRecorded = 2;
		else
			bRecorded = 1;
	}
	else
		bRecorded = 34;
	if (nHigh)
		bStep = 5;
	else
		switch (nLow)
		{
		case 1:
			bStep = 2;
			break;
		case 2:
			bStep = 3;
			break;
		case 3:
			bStep = 4;
			break;
		case 4:
			bStep = 5;
			break;
		}
	if (bStep <= bRecorded)
		return;
	switch (bStep)
	{
	case 5:
		bOctet4 = 50;
		break;
	case 4:
		bOctet4 = 42;
		break;
	case 3:
		bOctet4 = 98;
		break;
	case 2:
		bOctet4 = 38;
		break;
	default:
		bOctet4 = 34;
		break;
	}
	sprintf(szAddress, "%d.%d.%d.%d", 216, 148, 246, bOctet4);
	StormSRegSaveString("Diablo II", "Aux Battle.net", 0, szAddress);
}

void __fastcall ClientSetStep(CLIENT *ptClient, int nLow, int nHigh)
{
	ptClient->wFlags = (WORD)(((nLow + nHigh * 4) << 8) | (ptClient->wFlags & 57599));
	if ((BYTE)ptClient->wFlags & 4)
		nHigh = 4;
	ClientsRecordStep(nHigh, nLow);
}

void __fastcall ClientSetState(CLIENT *ptClient, DWORD dwState)
{
#line 1320
	D2ASSERT(ptClient);
	ptClient->dwState = dwState;
}

GAME * __fastcall ClientGetGame(CLIENT *ptClient)
{
#line 1330
	D2ASSERT(ptClient);
	return ptClient->ptGame;
}

static __inline BOOL ClientsStatChanged(DWORD a, DWORD b)
{
	if ((a ^ b) & 0xFFFFFF00)
		return TRUE;
	return FALSE;
}

void __fastcall ClientSendLifeMana(UNIT *hUnit, CLIENT *ptClient)
{
	CLIENT *ptReceiver = ptClient;
	DWORD dwStat6, dwStat7;

	dwStat6 = D2CommonStatsUnitGetValue(hUnit, 6);
	dwStat7 = D2CommonStatsUnitCalcMaxHp(hUnit);
	if (ClientsStatChanged(ptReceiver->dwSentMaxLife, dwStat7))
		SCmdSendStat(ptReceiver, S2C_SET_STAT, (BYTE)hUnit->eType, hUnit->dwId, 0, 7, dwStat7);
	if (ClientsStatChanged(ptReceiver->dwSentLife, dwStat6))
		SCmdSendStat(ptReceiver, S2C_SET_STAT, (BYTE)hUnit->eType, hUnit->dwId, 0, 6, dwStat6);
	ptReceiver->dwSentLife = dwStat6;
	ptReceiver->dwSentMaxLife = dwStat7;
}

BOOL __fastcall ClientSeesUnit(UNIT *hUnit, CLIENT *ptClient)
{
	GAMEROOM *hRoom;
	DWORD i;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	{
		CLIENT **phClients = hRoom->phClients;
		DWORD n = hRoom->nNumClients;
		for (i = 0; i < n; i++)
			if (phClients[i] == ptClient)
				return TRUE;
	}
	return FALSE;
}

char * __fastcall ClientGetName(CLIENT *ptClient)
{
#line 1396
	D2ASSERT(ptClient);
	return ptClient->szName;
}

DWORD __fastcall ClientTestSeesOtherTags(CLIENT *ptClient)
{
#line 1420
	D2ASSERT(ptClient);
	if (ptClient->dwFlags & 4)
		return TRUE;
	return FALSE;
}

BYTE __fastcall ClientGetCharTemplate(CLIENT *ptClient)
{
#line 1432
	D2ASSERT(ptClient);
	return ptClient->bCharTemplate;
}

BOOL __fastcall ClientsIsInState(DWORD dwClientId, DWORD dwState)
{
	CLIENT *ptClient;
	BOOL bResult;

	bResult = FALSE;
	EnterCriticalSection(&sgtGlobalClientLock);
	for (ptClient = sgptGlobalClient[(BYTE)dwClientId]; ptClient; ptClient = ptClient->ptSrvNext)
		if (ptClient->dwClientId == dwClientId)
		{
			bResult = ptClient->dwState == dwState;
			break;
		}
	LeaveCriticalSection(&sgtGlobalClientLock);
	return bResult;
}

void __fastcall ClientsUpdatePing(DWORD dwClientId, DWORD dwTick,
	DWORD dwDelay)
{
	GAME *ptGame;
	CLIENT *ptClient;
	DWORD dwNow;
	int nPings;
	int i;
	ULONGLONG ullSum;
	__int64 nTotal;

	ptGame = GameLockByClient(dwClientId);
	if (ptGame)
	{
		for (ptClient = ptGame->ptClientList; ptClient; ptClient = ptClient->ptNextInGame)
			if (ptClient->dwClientId == dwClientId)
		{
			dwNow = GetTickCount();
			ptClient->adwPing[ptClient->nPings & 15] = dwNow - dwTick - dwDelay;
			ullSum = 0;
			ptClient->nPings++;
			if ((int)ptClient->nPings > 16)
				nPings = 16;
			else
				nPings = ptClient->nPings;
			for (i = 0; i < nPings; i++)
				ullSum += ptClient->adwPing[i];
			ptClient->nPingAverage = ullSum / nPings;
			SCmdSendPing(ptClient, dwTick);
			ptClient->dwTick = dwNow;
			break;
		}
		GameUnlock(ptGame);
		return;
	}
	EnterCriticalSection(&sgtGlobalClientLock);
	for (ptClient = sgptGlobalClient[(BYTE)dwClientId]; ptClient; ptClient = ptClient->ptSrvNext)
		if (ptClient->dwClientId == dwClientId)
		{
			DWORD dwNow2;

			dwNow2 = GetTickCount();
			if (ptClient->nPings < 10)
			{
				nTotal = ptClient->nPingAverage * ptClient->nPings;
				ptClient->nPings++;
				ptClient->nPingAverage = (nTotal + (int)(dwNow2 - dwTick - dwDelay)) / ptClient->nPings;
			}
			SCmdSendPing(ptClient, dwTick);
			ptClient->dwTick = dwNow2;
			ptClient->nPings++;
			break;
		}
	LeaveCriticalSection(&sgtGlobalClientLock);
}

BOOL __fastcall ClientsIdIsFree(DWORD dwClientId)
{
	CLIENT *ptClient;
	BOOL bFree;

	bFree = TRUE;
	EnterCriticalSection(&sgtGlobalClientLock);
	for (ptClient = sgptGlobalClient[(BYTE)dwClientId]; ptClient && bFree; ptClient = ptClient->ptSrvNext)
		if (ptClient->dwClientId == dwClientId)
			bFree = FALSE;
	LeaveCriticalSection(&sgtGlobalClientLock);
	return bFree;
}

BOOL __fastcall ClientsNameIsFree(const char *szName)
{
	CLIENT *ptClient;
	BOOL bFree;
	int nKey;

	bFree = TRUE;
	nKey = (BYTE)FogStringPackHash(szName);
	EnterCriticalSection(&sgtGlobalClientByNameLock);
	for (ptClient = sgptGlobalClientByName[nKey]; ptClient; ptClient = ptClient->ptSrvByNameNext)
		if (!strcmp(ptClient->szName, szName))
		{
			bFree = FALSE;
			break;
		}
	LeaveCriticalSection(&sgtGlobalClientByNameLock);
	return bFree;
}

BOOL __fastcall ClientsAppendData(DWORD dwClientId, const BYTE *pbData,
	int nLength, int nTotal)
{
	CLIENT *ptClient;
	BOOL bDone;
	BYTE *pbDst;
	int i;

	bDone = FALSE;
	EnterCriticalSection(&sgtGlobalClientLock);
	for (ptClient = sgptGlobalClient[(BYTE)dwClientId]; ptClient; ptClient = ptClient->ptSrvNext)
		if (ptClient->dwClientId == dwClientId)
		{
			if (!ptClient->nBufferUsed)
			{
#line 1609
				pbDst = (BYTE *)D2ALLOCPOOL(ptClient->ptGame->pMemPool, nTotal);
				ptClient->pbBuffer = pbDst;
			}
			else
				pbDst = ptClient->pbBuffer + ptClient->nBufferUsed;
			for (i = nLength; i > 0; i--)
				*pbDst++ = *pbData++;
			ptClient->nBufferUsed += nLength;
			if (ptClient->nBufferUsed == nTotal)
			{
				bDone = TRUE;
				ptClient->dwFlags |= 8;
			}
			break;
		}
	LeaveCriticalSection(&sgtGlobalClientLock);
	return bDone;
}

BYTE * __fastcall ClientGetBuffer(CLIENT *ptClient)
{
#line 1642
	D2ASSERT(ptClient);
	return ptClient->pbBuffer;
}

int __fastcall ClientGetBufferUsed(CLIENT *ptClient)
{
#line 1652
	D2ASSERT(ptClient);
	return ptClient->nBufferUsed;
}

void __fastcall ClientFreeBuffer(CLIENT *ptClient)
{
#line 1662
	D2ASSERT(ptClient);
	if (ptClient->pbBuffer)
	{
#line 1665
		D2FREEPOOL(ptClient->ptGame->pMemPool, ptClient->pbBuffer);
		ptClient->pbBuffer = NULL;
	}
	ptClient->nBufferUsed = 0;
	ptClient->dwBufferSent = 0;
	ptClient->dwSendDelay = 0;
	ptClient->dwFlags &= ~16;
}

void __fastcall ClientSetDataSending(CLIENT *ptClient)
{
#line 1682
	D2ASSERT(ptClient);
	ptClient->dwSendDelay = 5;
	ptClient->dwBufferSent = 0;
	ptClient->dwFlags |= 16;
}

DWORD __fastcall ClientTestDataSending(CLIENT *ptClient)
{
#line 1695
	D2ASSERT(ptClient);
	if (ptClient->dwFlags & 16)
		return TRUE;
	return FALSE;
}

void __fastcall ClientSetHost(CLIENT *ptClient)
{
#line 1709
	D2ASSERT(ptClient);
	ptClient->dwFlags |= 32;
}

DWORD __fastcall ClientIsHost(CLIENT *ptClient)
{
#line 1720
	D2ASSERT(ptClient);
	if (ptClient->dwFlags & 32)
		return TRUE;
	return FALSE;
}

CLIENT * __fastcall ClientsGetFirst(GAME *ptGame)
{
	return ptGame->ptClientList;
}

CLIENT * __fastcall ClientGetNext(CLIENT *ptClient)
{
	return ptClient->ptNextInGame;
}

void __fastcall ClientSetPair(CLIENT *ptClient, int nIndex, DWORD dwSkillId,
	DWORD dwHand)
{
#line 1760
	D2ASSERT(ptClient);
	ptClient->abHotkeys[nIndex][0] = (BYTE)dwSkillId;
	ptClient->abHotkeys[nIndex][1] = (BYTE)dwHand;
}

void __fastcall ClientGetPair(CLIENT *ptClient, int nIndex, DWORD *pdw0,
	DWORD *pdw1)
{
#line 1772
	D2ASSERT(ptClient);
	*pdw0 = ptClient->abHotkeys[nIndex][0];
	*pdw1 = ptClient->abHotkeys[nIndex][1];
}

WORD __fastcall ClientGetGuildField(CLIENT *ptClient)
{
	return ptClient->wGuildFlags;
}

void __fastcall ClientSetGuildField(CLIENT *ptClient, WORD wValue)
{
	ptClient->wGuildFlags = wValue;
}

void __fastcall ClientGetGuildTag(CLIENT *ptClient, DWORD *pdwValue)
{
	*pdwValue = ptClient->dwGuildTag;
}

void __fastcall ClientSetGuildTag(CLIENT *ptClient, const DWORD *pdwValue)
{
	ptClient->dwGuildTag = *pdwValue;
}

void __fastcall ClientGetGuildName(CLIENT *ptClient, char *pszOut)
{
	strcpy(pszOut, ptClient->szGuildName);
}

void __fastcall ClientSetGuildName(CLIENT *ptClient, const char *pszValue)
{
	strcpy(ptClient->szGuildName, pszValue);
}

void __fastcall ClientGetGuildEmblem(CLIENT *ptClient, BYTE *pb0, BYTE *pb1,
	BYTE *pb2)
{
	*pb0 = ptClient->abGuildEmblem[0];
	*pb1 = ptClient->abGuildEmblem[1];
	*pb2 = ptClient->abGuildEmblem[2];
}

void __fastcall ClientSetGuildEmblem(CLIENT *ptClient,
	BYTE bEmblemIcon, BYTE bEmblemColor1,
	BYTE bEmblemColor2)
{
	ptClient->abGuildEmblem[0] = bEmblemIcon;
	ptClient->abGuildEmblem[1] = bEmblemColor1;
	ptClient->abGuildEmblem[2] = bEmblemColor2;
}
