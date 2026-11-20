#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\GAME\\Game.cpp"
#include <stdio.h>
#include "../D2game.h"
#include "../StormTableMembers.h"
#include "D2Macros.h"
#include "Arena.h"
#include "CCmd.h"
#include "Clients.h"
#include "Event.h"
#include "Game.h"
#include "Level.h"
#include "SCmd.h"
#include "Task.h"
#include "../MONSTER/MonsterMsg.h"
#include "../ITEMS/Items.h"
#include "../OBJECTS/Objects.h"
#include "../OBJECTS/objrgn.h"
#include "../PLAYER/PlrSave.h"
#include "../PLAYER/PlrMsg.h"
#include "../QUESTS/QUESTS.h"
#include "../UNIT/Party.h"
#include "../UNIT/PlayerList.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitInactive.h"
#include "../UNIT/sunitproxy.h"
#include "../GUILDS/Guilds.h"
#include "D2NetImports.h"
#include "FogImports.h"

#define GAME_MAXNAMETRIES	100
#define GAME_NUMCLASSES		5
#define GAME_UNITTYPES		5
#define GAME_UNITBUCKETS	128
#define GAME_HEARTBEAT		45000
#define GAME_MAXRESPAWNS	2000
#define MAX_UPDATES			55u

static GAMEHANDLES sgdwGameHandles;
static GAME *sgptFirstGame;
static BOOL sgbHeartbeat = TRUE;
static WORD sgwNextGameId = 1;
static DWORD sgdwGameSeed = 0xFFFFFFFF;
static BOOL sgbFixedRate = TRUE;
static DWORD sgdwMeasuredFps = 25;
static int sgnFps = 25;
static CRITICAL_SECTION sgtGameListLock = { 0 };
static GAMEDATATBL *sgptGameDataTbl = NULL;
EVENTCALLBACKTABLE *ptEventCallbackTable = NULL;
static DWORD sgdwStartAct = 0;
static BOOL sgbSendUpdates = FALSE;
static DWORD sgdwFpsTick = 0;
static DWORD sgdwLastFrameTime = 0;
static DWORD sgdwLastSendTime = 0;
static char sgszEmpty = 0;

typedef void (__fastcall *PFNGETDESCRIPTION)(char *szOut, DWORD dwClassId);

extern WORD GameNextId(void);
static BOOL GameNameInUse(const char *szName);
static void __fastcall GameSetUniqueName(GAME *ptGame, char *szName, DWORD dwUnused);
void __stdcall GameDropClient(DWORD dwClientId);
void __fastcall GameReportPlayers(GAME *ptGame);
void __fastcall GameLeaveClient(CLIENT *ptClient, DWORD bReason);
void __fastcall GameSendClientData(CLIENT *ptClient);
void __fastcall GameOnEndGame(DWORD dwClientId, BOOL bNotify);
extern void GameCountFrame(GAME *ptGame);
void __fastcall GameCheckHeartbeats(GAME *ptGame, BOOL *pbDropped);
static void __fastcall GameRespawnRooms(GAME *ptGame);
extern void __fastcall GameUpdateActRooms(GAME *ptGame);
static void __fastcall GameFreeIdleLevels(GAME *ptGame);
static void __fastcall GameUpdateClients(GAME *ptGame);
void __fastcall GameReleaseRooms(GAME *ptGame);
void __fastcall GameSendClientUpdates(GAME *ptGame, CLIENT *ptClient, BOOL bUpdate, DWORD dwUnused);
void __fastcall GameClose(DWORD dwGame);
void __fastcall GameFree(DWORD dwGame, GAME *ptGame);
void __fastcall GameForEachClient(GAME *ptsGame, void (__fastcall *pfn)(CLIENT *, void *), void *pArg);
void __fastcall GameDescribePlayer(char *szOut, DWORD dwClassId);
void __fastcall GameDescribeMonster(char *szOut, DWORD dwClassId);
void __fastcall GameDescribeObject(char *szOut, DWORD dwClassId);
void __fastcall GameDescribeItem(char *szOut, DWORD dwClassId);
void __fastcall GameDescribeMissile(char *szOut, DWORD dwClassId);
void __fastcall GameClientUnitGone(CLIENT *ptClient, UNIT *hUnit);
void __fastcall GameFindHostClient(CLIENT *ptClient, CLIENT **pptFound);
void __fastcall GameFindFirstClient(CLIENT *ptClient, CLIENT **pptFound);

static PFNGETDESCRIPTION sgpfnGetDescription[GAME_UNITTYPES] =
{
	GameDescribePlayer,
	GameDescribeMonster,
	GameDescribeObject,
	GameDescribeItem,
	GameDescribeMissile,
};

BOOL __cdecl GameServerInit(void)
{
	int i;

	StormSRegLoadValue("Diablo II", "PlayerPos", 0, &sgbSendUpdates);
	for (i = 0; i < GAME_MAXGAMES; i++)
		sgdwGameHandles[i] = 0;
	InitializeCriticalSection(&sgtGameListLock);
	ClientsInitLists();
	D2NetServerSetUnusedHook(GameDropClient);
	SUnitProxyInit();
	return TRUE;
}

BOOL __cdecl GameListFree(void)
{
	ClientsTermLists();
	DeleteCriticalSection(&sgtGameListLock);
	SUnitProxyFreeVendors();
	sgwNextGameId = 1;
	sgptGameDataTbl = NULL;
	sgdwStartAct = 0;
	return TRUE;
}

DWORD __fastcall GameFindById(WORD wGameId)
{
	GAME *ptGame;
	DWORD dwGame;
	int i;

	for (i = 0; i < GAME_MAXGAMES; i++)
	{
		dwGame = sgdwGameHandles[i];
		if (!dwGame)
			continue;
		ptGame = GameLockByHandle(dwGame);
		if (!ptGame)
			continue;
		if (ptGame->wGameId == wGameId)
		{
			GameUnlock(ptGame);
			return dwGame;
		}
		GameUnlock(ptGame);
	}
	return 0;
}

void __stdcall GameSetDataTable(GAMEDATATBL *ptGameDataTbl  ,
	void *phGameList  )
{
#line 178
	D2ASSERT(ptGameDataTbl);
	sgptGameDataTbl = ptGameDataTbl;
#line 180
	D2ASSERT(phGameList);
}

void __stdcall GameSetEventCallbacks(EVENTCALLBACKTABLE *ptTable)
{
	ptEventCallbackTable = ptTable;
}

DWORD __stdcall GameSetSeed(DWORD dwSeed)
{
	sgdwGameSeed = dwSeed;
	return dwSeed;
}

DWORD __cdecl GameGetFirstSeed(void)
{
	GAME *ptGame;
	DWORD dwSeed;

	ptGame = GameLockFirst();
	if (!ptGame)
		return 0;
	dwSeed = ptGame->dwSeed;
	GameUnlockFirst();
	return dwSeed;
}

BOOL __fastcall GameCheckCreatePacket(DWORD dwClientId, SRVPACKET *ptPacket)
{
	BYTE bClass;
	DWORD dwFlags;
	char *szGameName;
	char *szPlayer;

	bClass = ptPacket->tCreate.bClass;
	dwFlags = ptPacket->tCreate.dwFlags;
	szGameName = ptPacket->tCreate.szGameName;
	szPlayer = ptPacket->tCreate.szPlrName;
	if (ptEventCallbackTable && ptPacket->tCreate.bGameType)
		goto lFalse;
	if (!ArenaStringIsTerminated(szPlayer, 16))
	{
		GameLog(1,
			"[HACKLIST]  <D2CLTSYS_CREATEGAME>  CLIENTID:%d  ERROR: Invalid player name", dwClientId);
		return FALSE;
	}
	if (!ArenaStringIsTerminated(szGameName, 16))
	{
		GameLog(1, "[HACKLIST]  <D2CLTSYS_CREATEGAME>  PLAYER:%s  ERROR: Invalid game name",
			szGameName);
		return FALSE;
	}
	if (!(dwFlags & 6))
	{
		GameLog(1, "[HACKLIST]  <D2CLTSYS_CREATEGAME>  PLAYER:%s  GAME:%s  FLAGS:%x  "
			"ERROR: Invalid Game Flags",
			szPlayer, szGameName, dwFlags);
		return FALSE;
	}
	if ((BOOL)(!ClientsIdIsFree(dwClientId) || !ClientsNameIsFree(szPlayer)))
	{
		GameLog(1,
			"[HACKLIST]  <D2CLTSYS_CREATEGAME>  PLAYER:%s  GAME:%s  ERROR: Client already in a game",
			szPlayer, szGameName);
lFalse:
		return FALSE;
	}
	if (bClass >= GAME_NUMCLASSES)
	{
		GameLog(1, "[HACKLIST]  <D2CLTSYS_CREATEGAME>  PLAYER:%s  GAME:%s  CLASS:%d  "
			"ERROR: Invalid player class",
			szPlayer, szGameName, (int)bClass);
		return FALSE;
	}
	return TRUE;
}

static BOOL GameNameInUse(const char *szName)
{
	GAME *ptGame;
	int i;
	BOOL bInUse;

	bInUse = FALSE;
	for (i = 0; i < GAME_MAXGAMES; i++)
	{
		if (sgdwGameHandles[i])
		{
			ptGame = GameLockByHandle(sgdwGameHandles[i]);
			if (ptGame)
			{
				if (!StormSStrCmp(szName, ptGame->szGameName, 2147483647))
				{
					GameUnlock(ptGame);
					bInUse = TRUE;
					break;
				}
				GameUnlock(ptGame);
			}
		}
	}
	return bInUse;
}

static void __fastcall GameSetUniqueName(GAME *ptGame, char *szName, DWORD dwUnused)
{
	char szNewName[16];
	int nGame;

	if (!GameNameInUse(szName))
	{
		StormSStrCopy(ptGame->szGameName, szName, 2147483647);
		return;
	}
	if (15 - strlen(szName) < 3)
		szName[11] = '\0';
	nGame = 0;
	sprintf(szNewName, "%s~%d", szName, nGame++);
	while (GameNameInUse(szNewName) && nGame < GAME_MAXNAMETRIES)
		sprintf(szNewName, "%s~%d", szName, nGame++);
#line 378
	D2ASSERT(nGame < 100);
	StormSStrCopy(ptGame->szGameName, szNewName, 2147483647);
}

static BOOL GameCreateI(char *szGameName, char *szField36,
	DWORD dwArenaArg, DWORD dwFlags, BYTE bField47,
	BYTE bField48, BYTE bField4a, WORD *pwGameId)
{
	GAME *ptGame;
	DWORD dwGame;
	int nLock;
	int i;
	int j;

#line 469
	D2ASSERT(sgptGameDataTbl);
	if (!GameNextId())
		return FALSE;
	ptGame = sgptGameDataTbl->CreateLocked(&dwGame, &nLock);
	sgptGameDataTbl->ReleaseLock(nLock);
#line 489
	D2ASSERT(ptGame);
#line 492
	ptGame->ptLock = (CRITICAL_SECTION *)D2ALLOCPOOL(NULL, sizeof(CRITICAL_SECTION));
	InitializeCriticalSection(ptGame->ptLock);
	FogMemPoolSystemInit(&ptGame->pMemPool, 32768, 4096);
	memset(ptGame->pUnitList[0], 0, sizeof(ptGame->pUnitList[0]));
	memset(ptGame->pUnitList[1], 0, sizeof(ptGame->pUnitList[1]));
	memset(ptGame->pUnitList[2], 0, sizeof(ptGame->pUnitList[2]));
	memset(ptGame->pUnitList[3], 0, sizeof(ptGame->pUnitList[3]));
	memset(ptGame->pUnitList[4], 0, sizeof(ptGame->pUnitList[4]));
	ptGame->field_20 = 1;
	ptGame->wGameId = GameNextId();
	GameSetUniqueName(ptGame, szGameName, 0);
	StormSStrCopy(ptGame->szField36, szField36, 16);
	ptGame->field_60 = 0;
	ptGame->field_260 = 0;
	ptGame->bField47 = bField47;
	ptGame->bField48 = bField48;
	ptGame->bField4a = bField4a;
	ptGame->bDifficultyLevel = (BYTE)((dwFlags >> 12) & 7);
	if (sgdwGameSeed == (DWORD)-1)
	{
		ptGame->dwSeedFixed = 0;
		ptGame->dwSeed = D2CommonSeedInvent(0);
	}
	else
	{
		ptGame->dwSeedFixed = 1;
		ptGame->dwSeed = sgdwGameSeed;
	}
	EventListCreate(ptGame);
	ArenaNew(ptGame, dwArenaArg, dwFlags, 0);
	ClientsInitGame(ptGame);
	PartySystemCreate(ptGame);
	MonsterRegionsInit(ptGame);
	ptGame->dwObjRegionSeed = ObjectRgnCreate(ptGame);
	SUnitProxyAlloc(ptGame);
	QuestsInit(ptGame);
	GuildsInit(ptGame);
	EnterCriticalSection(&sgtGameListLock);
	for (i = 0; i < GAME_MAXGAMES; i++)
	{
		if (!sgdwGameHandles[i])
		{
			sgdwGameHandles[i] = dwGame;
			break;
		}
	}
	LeaveCriticalSection(&sgtGameListLock);
	TaskQueueGame(&ptGame->tTask, TASK_PROCESSGAME, dwGame);
	ptGame->dwRate = GetTickCount();
	*pwGameId = ptGame->wGameId;
	return TRUE;
}

BOOL __stdcall GameCreate(char *szGameName, char *szField36,
	DWORD dwArenaArg, DWORD dwFlags, BYTE bField47,
	BYTE bField48, BYTE bField4a, WORD *pwGameId)
{
	return GameCreateI(szGameName, szField36, dwArenaArg, dwFlags, bField47, bField48, bField4a, pwGameId);
}

__inline WORD GameNextId(void)
{
	WORD wStart;
	WORD wGameId;

	wStart = sgwNextGameId;
	EnterCriticalSection(&sgtGameListLock);
	wGameId = sgwNextGameId++;
	while (sgdwGameHandles[wGameId])
	{
		if (sgwNextGameId > GAME_MAXGAMES)
			sgwNextGameId = 1;
		wGameId = sgwNextGameId++;
		if (wGameId == wStart)
		{
			LeaveCriticalSection(&sgtGameListLock);
			return 0;
		}
	}
	LeaveCriticalSection(&sgtGameListLock);
	return wGameId;
}

DWORD __stdcall GameReceiveData(DWORD dwClientId, BYTE *pbData,
	WORD wLength, WORD wTotal, BOOL bDrop)
{
	GAME *ptGame;
	CLIENT *ptClient;

	if (wTotal)
	{
		if (!ClientsAppendData(dwClientId, pbData, wLength, wTotal))
			return TRUE;
	}
	if (!wTotal || bDrop)
	{
		GameDropClient(dwClientId);
		return FALSE;
	}
	ptGame = GameLockByClient(dwClientId);
	if (!ptGame)
		return FALSE;
	ptClient = ClientsFindInGame(ptGame, dwClientId);
	if (!ptClient)
	{
		GameUnlock(ptGame);
		return FALSE;
	}
	GameUnlock(ptGame);
	SCmdSendCmd(ptClient, S2C_PING_CHALLENGE);
	return wTotal;
}

void __fastcall GameReceivePing(DWORD dwClientId, DWORD dwTick, DWORD dwDelay)
{
	ClientsUpdatePing(dwClientId, dwTick, dwDelay);
}

extern void GameCreateFromPacketI(DWORD dwClientId, char *szGameName, BYTE bGameType, BYTE bClass,
	char *szPlrName, WORD wArena, DWORD dwFlags, BYTE bArenaType, BYTE bUnused1, BYTE bUnused2,
	BYTE bDifficulty);
__inline void GameCreateFromPacketI(DWORD dwClientId, char *szGameName, BYTE bGameType, BYTE bClass,
	char *szPlrName, WORD wArena, DWORD dwFlags, BYTE bArenaType, BYTE bUnused1, BYTE bUnused2,
	BYTE bDifficulty)
{
	GAME *ptGame;
	CLIENT *ptClient;
	DWORD dwGame;
	int nLock;
	int i;
	int j;

#line 629
	D2ASSERT(sgptGameDataTbl);
	ptGame = sgptGameDataTbl->CreateLocked(&dwGame, &nLock);
	sgptGameDataTbl->ReleaseLock(nLock);
#line 635
	D2ASSERT(ptGame);
#line 638
	ptGame->ptLock = (CRITICAL_SECTION *)D2ALLOCPOOL(NULL, sizeof(CRITICAL_SECTION));
	InitializeCriticalSection(ptGame->ptLock);
	ptGame->pMemPool = NULL;
	memset(ptGame->pUnitList[0], 0, sizeof(ptGame->pUnitList[0]));
	memset(ptGame->pUnitList[1], 0, sizeof(ptGame->pUnitList[1]));
	memset(ptGame->pUnitList[2], 0, sizeof(ptGame->pUnitList[2]));
	memset(ptGame->pUnitList[3], 0, sizeof(ptGame->pUnitList[3]));
	memset(ptGame->pUnitList[4], 0, sizeof(ptGame->pUnitList[4]));
	ptGame->field_20 = 1;
	ptGame->wGameId = GameNextId();
	GameSetUniqueName(ptGame, szGameName, dwClientId);
	ptGame->bGameType = bGameType;
	ptGame->bDifficultyLevel = bDifficulty;
	ptGame->field_60 = 0;
	ptGame->field_260 = 0;
	ptGame->bField47 = bArenaType;
	ptGame->field_268 = 1;
	if (sgdwGameSeed == (DWORD)-1)
	{
		ptGame->dwSeedFixed = 0;
		ptGame->dwSeed = D2CommonSeedInvent(0);
	}
	else
	{
		ptGame->dwSeedFixed = 1;
		ptGame->dwSeed = sgdwGameSeed;
	}
	EventListCreate(ptGame);
	ArenaNew(ptGame, wArena, dwFlags, bArenaType);
	ClientsInitGame(ptGame);
	ptClient = ClientsNew(ptGame, dwClientId, bClass, szPlrName, &sgszEmpty, 0);
	D2NetServerSetValue(dwClientId, dwGame);
	if (ptGame->bGameType == 1)
		ClientSetHost(ptClient);
	PartySystemCreate(ptGame);
	MonsterRegionsInit(ptGame);
	ptGame->dwObjRegionSeed = ObjectRgnCreate(ptGame);
	SUnitProxyAlloc(ptGame);
	QuestsInit(ptGame);
	EnterCriticalSection(&sgtGameListLock);
	for (i = 0; i < GAME_MAXGAMES; i++)
	{
		if (!sgdwGameHandles[i])
		{
			sgdwGameHandles[i] = dwGame;
			break;
		}
	}
	LeaveCriticalSection(&sgtGameListLock);
	SCmdSendGameFlags(ptClient, S2C_SET_ENVIRONMENT_STATE, ptGame);
	SCmdSendCmd(ptClient, S2C_IGNORED_00);
	ClientSetState(ptClient, 1);
	ClientSetAct(ptClient, (BYTE)sgdwStartAct);
	TaskQueueGame(&ptGame->tTask, TASK_PROCESSGAME, dwGame);
	if (ptEventCallbackTable)
	{
#line 734
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)ptEventCallbackTable->fpGetDatabaseCharacter), "ptEventCallbackTable->fpGetDatabaseCharacter");
		((PFNGETDATABASECHARACTER)ptEventCallbackTable->fpGetDatabaseCharacter)(szPlrName, dwClientId);
	}
	else
		SCmdSendCmd(ptClient, S2C_PING_CHALLENGE);
	GameLog(0, "[CLIENT]  <D2CLTSYS_CREATEGAME>  PLAYER:%s  GAME:%s  ERROR: None", szPlrName,
		szGameName);
}

void __fastcall GameCreateFromPacket(DWORD dwClientId, char *szGameName, BYTE bGameType, BYTE bClass,
	char *szPlrName, WORD wArena, DWORD dwFlags, BYTE bArenaType, BYTE bUnused1, BYTE bUnused2,
	BYTE bDifficulty)
{
	GameCreateFromPacketI(dwClientId, szGameName, bGameType, bClass, szPlrName, wArena, dwFlags, bArenaType, bUnused1, bUnused2,
		bDifficulty);
}

void __fastcall GameExchangePlayers(GAME *ptGame, CLIENT *ptClient)
{
	UNIT *hNewUnit;
	CLIENT *ptOther;
	UNIT *hUnit;
	void *pNode;
	DWORD dwItem;

	hNewUnit = ClientGetPlayerUnit(ptClient, FALSE);
#line 755
	D2ASSERT(hNewUnit);
	for (ptOther = ptGame->ptClientList; ptOther; ptOther = ptOther->ptNextInGame)
	{
		if (ptOther == ptClient)
			continue;
		hUnit = ClientGetPlayerUnit(ptOther, FALSE);
		if (!hUnit)
			continue;
		SCmdSendPlayer(ptClient, hUnit, ptOther);
		SCmdSendPlayer(ptOther, hNewUnit, ptClient);
		if (!hUnit->ptInventory)
			continue;
		for (pNode = D2CommonINVENTORYGetTrackedListHead(hUnit->ptInventory); pNode;
			pNode = (void *)D2CommonINVENTORYNodeGetTrackedNext(pNode))
		{
			dwItem = D2CommonINVENTORYNodeGetTrackedKey(pNode);
			if (dwItem != (DWORD)-1)
				SCmdSendCorpse(ptClient, hUnit->dwId, dwItem, 1);
		}
	}
	SCmdSendPlayer(ptClient, hNewUnit, ptClient);
	ArenaSendTagsOnJoin(ptGame, ptClient);
}

void __fastcall GameSendRosterRemoveToAll(GAME *ptGame, CLIENT *ptClient)
{
	UNIT *hUnit;
	CLIENT *ptOther;

	hUnit = ClientGetPlayerUnit(ptClient, FALSE);
	if (!hUnit)
		return;
	for (ptOther = ptGame->ptClientList; ptOther; ptOther = ptOther->ptNextInGame)
		if (ptOther->dwState == 4)
			SCmdSendRosterRemove(ptOther, hUnit->dwId);
}

BOOL __fastcall GameValidateJoinAct(DWORD dwClientId)
{
	if (!ClientsIsInState(dwClientId, 1))
	{
		GameLog(1,
			"[HACKLIST] <D2CLTSYS_JOINACT>  CLIENT:%d  ERROR: Invalid state to join act.", dwClientId);
		return FALSE;
	}
	return TRUE;
}

void __stdcall GameDropClient(DWORD dwClientId)
{
	GAME *ptGame;

	ptGame = GameLockByClient(dwClientId);
	if (ptGame)
	{
		if (ClientsIsInGame(ptGame, dwClientId))
		{
			GameReportPlayers(ptGame);
			ClientsRemove(ptGame, dwClientId);
		}
		GameUnlock(ptGame);
	}
	D2NetServerDisconnect(dwClientId);
}

void __fastcall GameRemoveClient(GAME *ptGame, DWORD dwClientId)
{
	if (ptGame && ClientsIsInGame(ptGame, dwClientId))
	{
		GameReportPlayers(ptGame);
		ClientsRemove(ptGame, dwClientId);
	}
	D2NetServerDisconnect(dwClientId);
}

static void GameEnterAct(GAME *ptGame, CLIENT *ptClient)
{
#line 261
	D2ASSERT(ptClient);
	LevelLoadAct(ptGame, ptClient->bAct);
	LevelSendActInit(ptGame, ptClient);
	GuildsSendSave(ptGame, ptClient);
	ClientSetState(ptClient, 2);
	ClientPlacePlayer(ptClient, ClientGetPlayerUnit(ptClient, TRUE), ptGame, NULL, 0, 0);
	ClientSetState(ptClient, 3);
}

void __fastcall GameOnJoinAct(DWORD dwClientId)
{
	GAME *ptGame;
	CLIENT *ptClient;
	int nError;

	ptGame = GameLockByClient(dwClientId);
	if (!ptGame)
		return;
	ptClient = ClientsFindInGame(ptGame, dwClientId);
	nError = ClientLoadCharacter(ptClient, ptGame, 1);
	if (nError)
	{
		SCmdSendJoinError(dwClientId, nError);
		GameReportPlayers(ptGame);
		ClientsRemove(ptGame, dwClientId);
		D2NetServerDisconnect(dwClientId);
		GameUnlock(ptGame);
		return;
	}
	GameEnterAct(ptGame, ptClient);
	GameUnlock(ptGame);
}
#line 270

extern BOOL GameIsValidPlayerName(const char *psz);
__inline BOOL GameIsValidPlayerName(const char *psz)
{
	int nLength;
	BOOL bSeparator;
	BOOL bValid;

	bSeparator = FALSE;
	nLength = 0;
	bValid = FALSE;
	while (nLength < 16)
	{
		switch (*psz++)
		{
		case '\0':
			bValid = nLength != 0;
			goto lChecked;
		case '\'':
		case '-':
		case '_':
			if (bSeparator || !nLength || !*psz)
				goto lChecked;
			bSeparator = TRUE;
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
		case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
		case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
		case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
		case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
			break;
		default:
			goto lChecked;
		}
		nLength++;
	}
lChecked:
	return bValid;
}

BOOL __fastcall GameJoin(DWORD dwClientId, WORD wGameId, BYTE bClass, BYTE *pbData, DWORD dwToken,
	char *szName, DWORD *pdwResult)
{
	char *szPlayer;

	szPlayer = (char *)pbData;
	if (!ArenaStringIsTerminated(szPlayer, 16))
	{
		GameLog(1, "[HACKLIST] <D2CLTSYS_JOINGAME>  CLIENT:%d  GAMEID:%d  TOKEN:%x  "
			"ERROR: Invalid Player Name",
			dwClientId, (int)wGameId, dwToken);
		return FALSE;
	}
	if (!ClientsIdIsFree(dwClientId))
	{
		GameLog(1, "[HACKLIST] <D2CLTSYS_JOINGAME>  ACCT:%s  CLIENT:%s  GAMEID:%d  TOKEN:%x  "
			"ERROR: Already in a Game",
			szName, szPlayer, (int)wGameId, dwToken);
		return FALSE;
	}
	if (!ClientsNameIsFree(szPlayer))
	{
		GameLog(1, "[HACKLIST] <D2CLTSYS_JOINGAME>  ACCT:%s  CLIENT:%s  GAMEID:%d  TOKEN:%x  "
			"ERROR: Already in a Game",
			szName, szPlayer, (int)wGameId, dwToken);
		return FALSE;
	}
	if (!GameFindById(wGameId))
	{
		GameLog(1, "[HACKLIST] <D2CLTSYS_JOINGAME>  ACCT:%s  CLIENT:%s  GAMEID:%d  TOKEN:%x  "
			"ERROR: Invalid Game ID",
			szName, szPlayer, (int)wGameId, dwToken);
		return FALSE;
	}
	if (bClass >= GAME_NUMCLASSES)
	{
		GameLog(1, "[HACKLIST] <D2CLTSYS_JOINGAME>  ACCT:%s  CLIENT:%s  GAMEID:%d  TOKEN:%x  "
			"ERROR: Invalid Player Class",
			szName, szPlayer, (int)wGameId, dwToken);
		return FALSE;
	}
	if (!GameIsValidPlayerName((char *)pbData))
	{
		GameLog(1, "[HACKLIST] <D2CLTSYS_JOINGAME>  ACCT:%s  CLIENT:%s  GAMEID:%d  TOKEN:%x  "
			"ERROR: Invalid Player Name",
			szName, (char *)pbData, (int)wGameId, dwToken);
		return FALSE;
	}
	if (!(ptEventCallbackTable))
		return TRUE;
#line 1114
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)ptEventCallbackTable->fpFindPlayerToken), "ptEventCallbackTable->fpFindPlayerToken");
	if (!((BOOL (__fastcall *)(char *, DWORD, int, char *, DWORD *))ptEventCallbackTable->fpFindPlayerToken)(
		(char *)pbData, dwToken, wGameId, szName, pdwResult))
	{
		GameLog(1,
			"[HACKLIST] <D2CLTSYS_JOINGAME>  ACCT:%s  CLIENT:%s  GAMEID:%d  TOKEN:%x  "
			"ERROR: Invalid Token",
			szName, (char *)pbData, (int)wGameId, dwToken);
		return FALSE;

	}
	return TRUE;
}
static __inline void GameEndClient(DWORD dwClientId)
{
	SCmdSendSysGameEnd(dwClientId);
	D2NetServerClose(dwClientId);
}

void __fastcall GameJoinDone(DWORD dwClientId, WORD wGameId, BYTE bClass, BYTE *pbData, char *szName,
	DWORD dwResult)
{
	GAME *ptGame;
	CLIENT *ptClient;
	DWORD dwGame;
	GAMEPLAYERMSG tMsg;

#line 1134
	D2ASSERT(sgptGameDataTbl);
	dwGame = GameFindById(wGameId);
	if (!dwGame)
		return;
	ptGame = GameLockByHandle(dwGame);
	if (!ptGame)
		return;
	D2NetServerSetValue(dwClientId, dwGame);
	ptClient = ClientsNew(ptGame, dwClientId, bClass, (char *)pbData, szName, dwResult);
	if (!ptClient)
	{
		if (ptEventCallbackTable)
		{
			GameLog(1,
				"[HACKLIST] <D2CLTSYS_JOINGAME>  ACCT:%s  CLIENT:%s  GAMEID:%d  "
				"ERROR: Unable to add client to game",
				szName, (char *)pbData, (int)wGameId);
			if (ptEventCallbackTable->fpUnlockDatabaseCharacter)
			{
#line 1159
				D2ASSERTMSG(!IsBadCodePtr((FARPROC)ptEventCallbackTable->fpUnlockDatabaseCharacter), "ptEventCallbackTable->fpUnlockDatabaseCharacter");
				((PFNUNLOCKDATABASECHARACTER)ptEventCallbackTable->fpUnlockDatabaseCharacter)((char *)pbData);
			}
		}
		GameUnlock(ptGame);
		GameEndClient(dwClientId);
		return;
	}
	SCmdSendGameFlags(ptClient, S2C_SET_ENVIRONMENT_STATE, ptGame);
	SCmdSendCmd(ptClient, S2C_IGNORED_00);
	ClientSetState(ptClient, 1);
	ptClient->bAct = (BYTE)sgdwStartAct;
	tMsg.bCmd = S2C_ROSTER_EVENT;
	tMsg.bEvent = 2;
	tMsg.bColor = 4;
	tMsg.dwArg = 0;
	tMsg.szAccount[0] = '\0';
	if (szName && strlen(szName))
	{
		StormSStrCopy(tMsg.szAccount, szName, 16);
		tMsg.szAccount[15] = '\0';
	}
	StormSStrCopy(tMsg.szName, ptClient->szName, 16);
	PlrMsgSendSystemMsgAll(ptGame, &tMsg);
	GameUnlock(ptGame);
	if (ptEventCallbackTable)
	{
#line 1194
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)ptEventCallbackTable->fpGetDatabaseCharacter), "ptEventCallbackTable->fpGetDatabaseCharacter");
		((PFNGETDATABASECHARACTER)ptEventCallbackTable->fpGetDatabaseCharacter)((char *)pbData, dwClientId);
	}
	else
		SCmdSendCmd(ptClient, S2C_PING_CHALLENGE);
}

void __fastcall GameFree(DWORD dwGame, GAME *ptGame)
{
	CRITICAL_SECTION *ptLock;
	int i;

	EnterCriticalSection(&sgtGameListLock);
	for (i = 0; i < GAME_MAXGAMES; i++)
		if (sgdwGameHandles[i] == dwGame)
			sgdwGameHandles[i] = 0;
	LeaveCriticalSection(&sgtGameListLock);
	D2CommonMonsterRegionFreeLevelPopulation(ptGame->pMemPool, &ptGame->pMonRegions);
	ObjectRgnFree(ptGame);
	GuildsFree(ptGame);
	QuestsFree(ptGame);
	SUnitProxyFree(ptGame);
	ClientsFreeAll(ptGame);
	LevelFreeGameUnits(ptGame);
	SUnitInactiveFreeAll(ptGame);
	ITEMSFreeUniqueLists(ptGame);
	EventListFree(ptGame);
	PartySystemFree(ptGame);
	ArenaFree(ptGame);
	for (i = 0; i < NUM_ACTS; i++)
	{
		if (ptGame->ptAct[i])
		{
			D2CommonDungeonFreeAct(ptGame->ptAct[i]);
			ptGame->ptAct[i] = NULL;
		}
	}
	if (ptGame->pMemPool)
		FogMemPoolSystemDestroy(ptGame->pMemPool);
	LeaveCriticalSection(ptGame->ptLock);
	ptLock = ptGame->ptLock;
	ptGame->ptLock = NULL;
	DeleteCriticalSection(ptLock);
#line 1257
	D2FREEPOOL(NULL, ptLock);
}

BOOL __fastcall GameValidateEndGame(DWORD dwClientId)
{
	if (!ClientsIsInState(dwClientId, 4))
	{
		GameLog(1,
			"[HACKLIST] <D2CLTSYS_ENDGAME>  CLTID:%d  REASON:Tried to end game they weren't in.",
			dwClientId);
		return FALSE;
	}
	return TRUE;
}

void __fastcall GameReportPlayers(GAME *ptGame)
{
	CLIENT *ptClient;
	CLIENT *ptNext;
	UNIT *hUnit;
	void **ppfnReport;

	for (ptClient = ptGame->ptClientList; ptClient; ptClient = ptNext)
	{
		ptNext = ptClient->ptNextInGame;
		if (ptClient->dwState != 4)
			continue;
		hUnit = ClientGetPlayerUnit(ptClient, FALSE);
		if (!hUnit)
			continue;
		PlrSaveCharacter(ptGame, hUnit, ClientGetName(ptClient));
		if (!ptEventCallbackTable)
			continue;
		ppfnReport = &ptEventCallbackTable->pfnReportPlayer;
		if (!*ppfnReport)
			continue;
		((PFNCALLBACK28)*ppfnReport)(ClientGetName(ptClient), (WORD)hUnit->dwClassId,
			D2CommonStatsUnitGetBaseValue(hUnit, 12), (int)D2CommonStatsUnitGetBaseValue(hUnit, 13),
			ClientGetFlags(ptClient));
	}
	GuildsSave(ptGame);
}

void __fastcall GameLeaveClient(CLIENT *ptClient, DWORD bReason)
{
	GAME *ptGame;
	DWORD dwClientId;
	GAMEPLAYERMSG tMsg;

	if (!ptClient)
		return;
	ptGame = ClientGetGame(ptClient);
	dwClientId = ClientGetId(ptClient);
	tMsg.bCmd = S2C_ROSTER_EVENT;
	tMsg.bEvent = bReason;
	tMsg.bColor = 4;
	tMsg.dwArg = 0;
	ClientGetAccount(ptClient, tMsg.szAccount);
	tMsg.szAccount[15] = '\0';
	StormSStrCopy(tMsg.szName, ClientGetName(ptClient), 16);
	PlrMsgSendSystemMsgAll(ptGame, &tMsg);
	GameReportPlayers(ptGame);
	ClientsRemove(ptGame, ClientGetId(ptClient));
	GameEndClient(dwClientId);
}

void __fastcall GameOnDisconnect(DWORD dwClientId, DWORD bReason)
{
	GAME *ptGame;

	if (!dwClientId)
		return;
	ptGame = GameLockByClient(dwClientId);
	if (!ptGame)
		return;
	GameLeaveClient(ClientsFindInGame(ptGame, dwClientId), (BYTE)bReason);
	GameUnlock(ptGame);
}

void __stdcall GameKickClient(DWORD dwClientId)
{
	GAME *ptGame;
	CLIENT *ptClient;
	GAMEPLAYERMSG tMsg;

	if (!dwClientId)
		return;
	ptGame = GameLockByClient(dwClientId);
	if (!ptGame)
		return;
	ptClient = ClientsFindInGame(ptGame, dwClientId);
	tMsg.bCmd = S2C_ROSTER_EVENT;
	tMsg.bEvent = 0;
	tMsg.bColor = 4;
	tMsg.dwArg = 0;
	tMsg.szAccount[0] = '\0';
	StormSStrCopy(tMsg.szName, ClientGetName(ptClient), 16);
	PlrMsgSendSystemMsgAll(ptGame, &tMsg);
	GameReportPlayers(ptGame);
	ClientsRemove(ptGame, ClientGetId(ptClient));
	GameUnlock(ptGame);
}

static __inline void GameFlushClientData(GAME *ptGame, CLIENT *ptClient)
{
	if (ptGame->bGameType == 1 || ptGame->bGameType == 2)
		while (ClientTestDataSending(ptClient))
			GameSendClientData(ptClient);
}

void __fastcall GameFlushClients(GAME *ptGame)
{
	CLIENT *ptClient;
	CLIENT *ptNext;
	UNIT *hUnit;

	if (ptGame->bGameType != 1 && ptGame->bGameType != 2)
		return;
	for (ptClient = ptGame->ptClientList; ptClient; ptClient = ptNext)
	{
		ptNext = ptClient->ptNextInGame;
		if (ptClient->dwState != 4)
			continue;
		hUnit = ClientGetPlayerUnit(ptClient, FALSE);
		if (!hUnit)
			continue;
		PlrSaveCharacter(ptGame, hUnit, ClientGetName(ptClient));
		GameFlushClientData(ptGame, ptClient);
	}
}

void __fastcall GameSendClientData(CLIENT *ptClient)
{
	GAMEDATAPIECE *ptPiece;
	BYTE *pbFrom;
	BYTE *pbTo;
	int nLength;
	int i;

	if (!ClientTestDataSending(ptClient))
		return;
	if (ptClient->dwSendDelay)
	{
		ptClient->dwSendDelay--;
		return;
	}
	pbFrom = ptClient->pbBuffer + ptClient->dwBufferSent;
	nLength = ptClient->nBufferUsed - ptClient->dwBufferSent;
	ptClient->dwSendDelay = 5;
	if (nLength > 255)
		nLength = 255;
#line 1282
	ptPiece = (GAMEDATAPIECE *)D2ALLOCPOOL(ptClient->ptGame->pMemPool, sizeof(GAMEDATAPIECE));
	ptPiece->bCmd = 4;
	ptPiece->bLength = (BYTE)nLength;
	if (!ptClient->dwBufferSent)
		ptPiece->bFirst = 1;
	else
		ptPiece->bFirst = 0;
	ptPiece->dwTotal = ptClient->nBufferUsed;
	pbTo = ptPiece->abData;
	for (i = 0; i < nLength; i++)
		*pbTo++ = *pbFrom++;
	if (D2NetServerSend(0, ptClient->dwClientId, ptPiece, nLength + 7))
	{
		ptClient->dwBufferSent += nLength;
		if (ptClient->dwBufferSent == (DWORD)ptClient->nBufferUsed)
			ClientFreeBuffer(ptClient);
		ptClient->nSendErrors = 0;
	}
	else
		ptClient->nSendErrors++;
#line 1311
	D2FREEPOOL(ptClient->ptGame->pMemPool, ptPiece);
}

void __fastcall GameOnEndGame(DWORD dwClientId, BOOL bNotify)
{
	GAME *ptGame;
	CLIENT *ptClient;
	DWORD dwAll;
	GAMEPLAYERMSG tMsg;

	ptGame = GameLockByClient(dwClientId);
	if (!ptGame)
		return;
	GameReportPlayers(ptGame);
	ptClient = ClientsFindInGame(ptGame, dwClientId);
	dwAll = ClientIsHost(ptClient);
	while (ptClient)
	{
		SCmdSendCmd(ptClient, S2C_ACT_LEAVE);
		GameFlushClientData(ptGame, ptClient);
		SCmdSendCmd(ptClient, S2C_UI_STATE_GATE);
		SCmdSendSysGameEnd(dwClientId);
		GameSendClientUpdates(ptGame, ptClient, FALSE, 0);
		tMsg.bCmd = S2C_ROSTER_EVENT;
		tMsg.bEvent = 3;
		tMsg.bColor = 4;
		tMsg.dwArg = 0;
		ClientGetAccount(ptClient, tMsg.szAccount);
		tMsg.szAccount[15] = '\0';
		StormSStrCopy(tMsg.szName, ClientGetName(ptClient), 16);
		ClientsRemove(ptGame, dwClientId);
		if (bNotify)
			PlrMsgSendSystemMsgAll(ptGame, &tMsg);
		if (dwAll)
		{
			ptClient = ClientsGetFirst(ptGame);
			if (ptClient)
				dwClientId = ClientGetId(ptClient);
		}
		else
			ptClient = NULL;
		GameLog(0, "[SERVER]  Disconnect: %s", tMsg.szName);
	}
	GuildsSave(ptGame);
	GameUnlock(ptGame);
	D2NetServerClose(dwClientId);
}

void __fastcall GameDropHost(GAME *ptsGame, CLIENT *ptClient)
{
	CLIENT *ptFound;

	ptFound = NULL;
	GameForEachClient(ptsGame, (void (__fastcall *)(CLIENT *, void *))GameFindHostClient, &ptFound);
	if (ptFound)
	{
		GameOnEndGame(ClientGetId(ptFound), 0);
		return;
	}
	if (!ptClient)
	{
		ptFound = NULL;
		GameForEachClient(ptsGame, (void (__fastcall *)(CLIENT *, void *))GameFindFirstClient, &ptFound);
		if (ptFound)
			GameOnEndGame(ClientGetId(ptFound), 0);
		return;
	}
	GameOnEndGame(ClientGetId(ptClient), 0);
}

void __fastcall GameFindHostClient(CLIENT *ptClient, CLIENT **pptFound)
{
	if (ClientIsHost(ptClient))
		*pptFound = ptClient;
}

void __fastcall GameFindFirstClient(CLIENT *ptClient, CLIENT **pptFound)
{
	if (!*pptFound)
		*pptFound = ptClient;
}

BOOL __fastcall GameValidateDisconnect(DWORD dwClientId)
{
	if (!ClientsIdIsFree(dwClientId))
	{
		GameLog(1, "[HACKLIST] <D2CLTSYS_DISCONNECT>  CLTID:%d  REASON:Not in game",
			dwClientId);
		return FALSE;
	}
	return TRUE;
}

DWORD __stdcall GameProcessMessages(void)
{
	LARGE_INTEGER liEnd;
	LARGE_INTEGER liStart;
	BYTE abMessage[512];
	int nLength;
	BOOL bAny;

	bAny = FALSE;
	QueryPerformanceCounter(&liStart);
	while (1)
	{
		nLength = D2NetServerReceiveSystemMessage(abMessage, sizeof(abMessage));
		if (nLength == -1)
			break;
		bAny = TRUE;
		ArenaSrvMsgDispatch((SRVMSG *)abMessage, nLength);
	}
	while (1)
	{
		nLength = D2NetServerReceiveClientCommand(abMessage, sizeof(abMessage));
		if (nLength == -1)
			break;
		bAny = TRUE;
		CCmdProcess((SRVMSG *)abMessage, nLength);
	}
	if (bAny)
	{
		QueryPerformanceCounter(&liEnd);
		liStart.QuadPart = liEnd.QuadPart - liStart.QuadPart;
	}
	else
		liStart.QuadPart = 0;
	return liStart.LowPart;
}

void __fastcall GameRespawnRoom(GAME *ptGame, GAMEROOM *hRoom)
{
	BOOL bManual;
	DWORD i;

	MonsterSpawnCritter(ptGame, hRoom);
	if (!(hRoom->dwFlags & 1))
	{
		bManual = FALSE;
		if (ptGame->nRespawnRooms < GAME_MAXRESPAWNS)
		{
			for (i = 0; i < ptGame->nRespawnRooms; i++)
			{
				if (ptGame->ahRespawnRooms[i] == hRoom)
				{
					OutputDebugStringA("Oh yeah, we got some o'that respawing action. "
						"Going to manual override.\n");
#line 1734
					D2WARN("Respawn: Get Kemmer");
					ptGame->ahRespawnRooms[ptGame->nRespawnRooms] = hRoom;
					ptGame->nRespawnRooms++;
					bManual = TRUE;
					break;
				}
			}
		}
		if (!bManual)
		{
			SUnitSpawnRoomPresets(ptGame, hRoom);
			SUnitInactiveRestoreRoom(ptGame, hRoom);
			ObjectPopulateRoom(ptGame, hRoom);
			MonsterPopulateRoom(ptGame, hRoom);
		}
		hRoom->dwFlags |= 1;
		D2CommonDungeonRoomSetPopulated(hRoom, 1);
		return;
	}
	if (!D2CommonDungeonRoomTestPopulated(hRoom))
	{
		SUnitInactiveRestoreRoom(ptGame, hRoom);
		D2CommonDungeonRoomSetPopulated(hRoom, 1);
	}
}

void __fastcall GameUpdate(GAME *ptGame)
{
	GameCountFrame(ptGame);
	GameRespawnRooms(ptGame);
	EventListRun(ptGame);
	GameUpdateClients(ptGame);
	LevelUpdateChangedUnits(ptGame);
	LevelFreeRoomNodes(ptGame);
	GameUpdateActRooms(ptGame);
	if (!(ptGame->nFrame % 20))
		QuestsUpdateTimers(ptGame);
	if (!(ptGame->nFrame % 12))
		GameReleaseRooms(ptGame);
	if (!(ptGame->nFrame % 11))
		GameFreeIdleLevels(ptGame);
}

__inline void GameCountFrame(GAME *ptGame)
{
	DWORD dwTick;
	DWORD dwElapsed;
	DWORD dwFrames;

	ptGame->nFrame++;
	dwTick = GetTickCount();
	dwElapsed = dwTick - ptGame->dwSecondTick;
	if (dwElapsed >= 1000)
	{
		dwFrames = ptGame->dwFrames;
		ptGame->dwFrames = 0;
		ptGame->dwSecondTick = dwTick;
		ptGame->dwFramesPerSecond = dwFrames * 1000 / dwElapsed;
		if (dwTick - sgdwFpsTick > 10000)
		{
			sgdwFpsTick = dwTick;
			sgdwMeasuredFps = ptGame->dwFramesPerSecond;
		}
	}
	ptGame->dwFrames++;
}

void __fastcall GameReleaseRooms(GAME *ptGame)
{
	GAMEROOM *hRoom;
	GAMEROOM *hNext;
	UNIT *hUnit;
	UNIT *hUnitNext;
	int i;

	for (i = 0; i < NUM_ACTS; i++)
	{
		if (!ptGame->ptAct[i])
			continue;
		for (hRoom = (GAMEROOM *)D2CommonDungeonActGetFirstRoom(ptGame->ptAct[i]); hRoom; hRoom = hNext)
		{
			hNext = hRoom->ptActNext;
			if ((int)D2CommonDungeonRoomTickEmptyCounter(hRoom) <= 10)
				continue;
			if (!D2CommonDungeonRoomCanRelease(ptGame->ptAct[i], hRoom))
				continue;
			for (hUnit = hRoom->ptUnitFirst; hUnit; hUnit = hUnitNext)
			{
				hUnitNext = hUnit->ptRoomNext;
				SUnitInactiveRemoveUnit(ptGame, hUnit);
			}
			D2CommonDungeonActRemoveRoom(ptGame->ptAct[i], hRoom);
		}
	}
}

void __fastcall GameCheckHeartbeats(GAME *ptGame, BOOL *pbDropped)
{
	CLIENT *ptClient;
	CLIENT *ptNext;
	char *szName;
	DWORD dwNow;

	*pbDropped = FALSE;
	dwNow = GetTickCount();
	for (ptClient = ptGame->ptClientList; ptClient; ptClient = ptNext)
	{
		ptNext = ptClient->ptNextInGame;
		if (!sgbHeartbeat || dwNow <= ptClient->dwTick || dwNow - ptClient->dwTick <= GAME_HEARTBEAT
			|| !ptEventCallbackTable)
			continue;
		szName = ClientGetName(ptClient);
		PlrSaveCharacter(ptGame, ClientGetPlayerUnit(ptClient, FALSE), szName);
		*pbDropped = TRUE;
		GameLog(1, "[DISCONNECT]  PLAYER:%s  REASON:Heartbeat Timeout",
			ClientGetName(ptClient));
		GameLeaveClient(ptClient, 0);
	}
	if (*pbDropped)
		GameReportPlayers(ptGame);
}

static int sgnFrameMs = 1000 / sgnFps;

extern void GameStartFrame(DWORD dwNow, int nDelta, BOOL bLimit);
__inline void GameStartFrame(DWORD dwNow, int nDelta, BOOL bLimit)
{
	nDelta -= sgnFrameMs;
	if (bLimit && (DWORD)nDelta >= (DWORD)sgnFrameMs)
		nDelta = sgnFrameMs;
	sgdwLastFrameTime = dwNow - nDelta;
}

DWORD __stdcall GameUpdateAll(BOOL bLimit)
{
	LARGE_INTEGER aliTime[2];
	GAME *ptGame;
	DWORD dwGame;
	DWORD dwNow;
	int nDelta;
	BOOL bAny;
	int i;

	bAny = FALSE;
	QueryPerformanceCounter(&aliTime[1]);
	dwNow = timeGetTime() & 2147483647;
	if (!sgdwLastFrameTime)
		sgdwLastFrameTime = dwNow;
	nDelta = (int)(dwNow - sgdwLastFrameTime);
	if (nDelta < sgnFrameMs)
		return 0;
	GameStartFrame(dwNow, nDelta, bLimit);
	for (i = 0; i < GAME_MAXGAMES; i++)
	{
		dwGame = sgdwGameHandles[i];
		if (!dwGame)
			continue;
		ptGame = GameLockByHandle(dwGame);
		if (!ptGame)
			continue;
		bAny = TRUE;
		if (!sgbFixedRate)
		{
			if (ptGame->dwRateTick)
			{
				ptGame->dwRate = ((dwNow - ptGame->dwRateTick) << 10) / (DWORD)sgnFrameMs;
				if (ptGame->dwRate < 10)
					ptGame->dwRate = 10;
				else if (ptGame->dwRate > 2048)
					ptGame->dwRate = 2048;
			}
			ptGame->dwRateTick = dwNow;
		}
		else
			ptGame->dwRate = 1024;
		GameUpdate(ptGame);
		GameUnlock(ptGame);
	}
	if (bAny)
	{
		QueryPerformanceCounter(&aliTime[0]);
		aliTime[1].QuadPart = aliTime[0].QuadPart - aliTime[1].QuadPart;
	}
	else
		aliTime[1].QuadPart = 0;
	return aliTime[1].LowPart;
}

static void __fastcall GameRespawnRooms(GAME *ptGame)
{
	GAMEROOM *hRoom;
	int i;

#line 1768
	D2WARN(ptGame);
	for (i = 0; i < NUM_ACTS; i++)
	{
		if (!ptGame->ptAct[i] || !ptGame->ptAct[i]->dwRoomsAdded)
			continue;
		for (hRoom = (GAMEROOM *)D2CommonDungeonActGetFirstRoom(ptGame->ptAct[i]); hRoom;
			hRoom = hRoom->ptActNext)
			GameRespawnRoom(ptGame, hRoom);
		ptGame->ptAct[i]->dwRoomsAdded = 0;
	}
}

__inline void __fastcall GameUpdateActRooms(GAME *ptGame)
{
	int i;

#line 1793
	D2ASSERT(ptGame);
	for (i = 0; i < NUM_ACTS; i++)
		if (ptGame->ptAct[i])
			D2CommonDrlgActUpdateRooms(D2CommonDungeonActGetDrlgAct(ptGame->ptAct[i]));
}

static void __fastcall GameFreeIdleLevels(GAME *ptGame)
{
	int i;

	for (i = 0; i < NUM_ACTS; i++)
		if (ptGame->ptAct[i])
			D2CommonDungeonActFreeIdleLevels(ptGame->ptAct[i]);
}

static void __fastcall GameUpdateClients(GAME *ptGame)
{
	CLIENT *ptClient;
	CLIENT *ptNext;
	CLIENT *ptFirst;
	UNIT *hUnit;
	BOOL bReport;
	BOOL bDropped;

	bReport = FALSE;
	bDropped = FALSE;
	if (!(ptGame->nFrame % 8192))
		bReport = TRUE;
	if (!ArenaTestRequiredFlag(ptGame))
	{
#line 1976
		D2FATAL("Tell peter hu if it gets here!");
	}
	GameCheckHeartbeats(ptGame, &bDropped);
	if (bReport && !bDropped)
		GameReportPlayers(ptGame);
	ptFirst = ptGame->ptClientList;
	for (ptClient = ptGame->ptClientList; ptClient; ptClient = ptNext)
	{
		ptNext = ptClient->ptNextInGame;
		switch (ptClient->dwState)
		{
		case 3:
			ClientUpdateRoom(ptGame, ptClient);
			if (!D2CommonDungeonRoomNearAllFlagged(ptClient->hRoom))
				break;
			if (!ClientGetPlayerUnit(ptFirst, FALSE))
				break;
			SCmdSendCmd(ptClient, S2C_ACT_ENTER);
			ClientSetState(ptClient, 4);
			GameExchangePlayers(ptGame, ptClient);
			PlayerListAddPlayer(ptGame, ClientGetPlayerUnit(ptClient, TRUE));
			if (!ptEventCallbackTable)
				break;
			hUnit = ClientGetPlayerUnit(ptFirst, FALSE);
			if (!hUnit || !ptEventCallbackTable->fpEnterGame)
				break;
#line 2031
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)ptEventCallbackTable->fpEnterGame), "ptEventCallbackTable->fpEnterGame");
			((PFNENTERGAME)ptEventCallbackTable->fpEnterGame)(ptGame->wGameId, ptClient->szName,
				(WORD)hUnit->dwClassId, (DWORD)D2CommonStatsUnitGetValue(hUnit, 12), 0);
			break;
		case 4:
			ClientUpdateRoom(ptGame, ptClient);
			break;
		case 5:
			ClientUpdateRoom(ptGame, ptClient);
			if (D2CommonDungeonRoomNearAllFlagged(ptClient->hRoom))
			{
				SCmdSendCmd(ptClient, S2C_ACT_ENTER);
				ClientSetState(ptClient, 4);
			}
			break;
		}
	}
}

static void GameSendClientState(CLIENT *ptClient)
{
	UNIT *hControl;

	hControl = ClientGetPlayerUnit(ptClient, FALSE);
	if (hControl && (int)ptClient->dwUpdateFrames >= 25)
	{
		ptClient->dwUpdateFrames = 0;
#line 2236
		D2ASSERT(UnitGetType ( hControl ) == UNIT_PLAYER);
		PlrMsgSendLifeMana(hControl, hControl);
	}
}

static void GameSendClientNear(GAME *ptGame, CLIENT *ptClient)
{
	UNIT *hControl;
	UNIT *hOther;
	DWORD *pdwUsed;
	int nUpdateSize;

	pdwUsed = (DWORD *)ClientGetQueueHead(ptClient);
	if (!pdwUsed)
		return;
	hControl = ClientGetPlayerUnit(ptClient, FALSE);
	if (!hControl || (int)ptClient->dwUpdateFrames < 25)
		return;
#line 2255
	D2ASSERT(UnitGetType ( hControl ) == UNIT_PLAYER);
	hOther = ClientFindLifeManaUnit(ptGame, ptClient);
	if (hOther)
		ClientSendLifeMana(hOther, ptClient);
	nUpdateSize = (501 - (int)*pdwUsed) / 9;
	if (nUpdateSize > 0)
	{
#line 2265
		D2ASSERT(nUpdateSize <= MAX_UPDATES);
		ClientUpdateNearPlayers(ptClient, nUpdateSize);
		SCmdSendNearList(ptClient);
	}
}

void __fastcall GameSendClientUpdates(GAME *ptGame, CLIENT *ptClient, BOOL bUpdate, DWORD dwUnused)
{
	CLIENTNODE178 *ptNode;
	GAMEQUEUEDRECORD *ptRecord;
	int nSent;
	int nError;
	BOOL bError;
	WORD wSize;

#line 2281
	D2ASSERT(ptGame);
#line 2282
	D2ASSERT(ptClient);
	if (bUpdate && ptClient->dwState == 4)
	{
		if (sgbSendUpdates)
			GameSendClientNear(ptGame, ptClient);
		GameSendClientState(ptClient);
	}
	if (ptGame->bGameType == 1 || ptGame->bGameType == 2)
		GameSendClientData(ptClient);
	bError = FALSE;
	nSent = 0;
	while (!bError)
	{
		ptNode = ClientQueuePop(ptClient);
		if (!ptNode || !ptNode->nUsed)
			return;
		for (ptRecord = (GAMEQUEUEDRECORD *)ptNode->abData;
			(BYTE *)ptRecord < ptNode->abData + ptNode->nUsed;
			ptRecord = (GAMEQUEUEDRECORD *)((BYTE *)ptRecord + wSize))
		{
			wSize = ptRecord->wSize;
			if (ptRecord->bCmd == 130)
				ptRecord->dwTime = GetTickCount() - (DWORD)ptClient->nPingAverage;
		}
		if (D2NetServerSend(1, ptClient->dwClientId, ptNode->abData, ptNode->nUsed))
		{
			ptNode->nUsed = 0;
			ptNode->ptNext = ptClient->ptFreeNodes;
			ptClient->ptFreeNodes = ptNode;
			ptClient->nSendErrors = 0;
			nSent++;
		}
		else
		{
			nError = D2NetGetLastError();
			GameLog(1,
				"[NET] <ERROR SENDING PACKET>  CLTID:%d  CLIENT:%s  #ERRORS:%d  ERROR:%d",
				ptClient->dwClientId, ptClient->szName, ptClient->nSendErrors, nError);
			ptClient->nSendErrors++;
			bError = TRUE;
			ClientQueuePushFront(ptClient, ptNode);
			if (ptClient->nSendErrors >= 3)
			{
				GameLeaveClient(ptClient, 0);
				return;
			}
		}
		if ((ptGame->bGameType == 1 || ptGame->bGameType == 2) && nSent >= 3)
			return;
	}
}
#line 2334

void __fastcall GameSendAllClientUpdates(GAME *ptGame, DWORD dwArg)
{
	CLIENT *ptClient;
	CLIENT *ptNext;

	for (ptClient = ptGame->ptClientList; ptClient; ptClient = ptNext)
	{
		ptNext = ptClient->ptNextInGame;
		GameSendClientUpdates(ptGame, ptClient, TRUE, dwArg);
	}
}

DWORD __stdcall GameSendAll(BOOL fUpdated, BOOL fFinal)
{
	LARGE_INTEGER aliTime[2];
	GAME *ptGame;
	DWORD dwGame;
	DWORD dwNow;
	BOOL bAny;
	int i;

	bAny = FALSE;
	QueryPerformanceCounter(&aliTime[1]);
	dwNow = GetTickCount();
	if (!fUpdated && !fFinal)
	{
		if (dwNow - sgdwLastSendTime < 40)
			return 0;
		sgdwLastSendTime = dwNow;
	}
	for (i = 0; i < GAME_MAXGAMES; i++)
	{
		dwGame = sgdwGameHandles[i];
		if (!dwGame)
			continue;
		ptGame = GameLockByHandle(dwGame);
		if (!ptGame)
			continue;
		bAny = TRUE;
		GameSendAllClientUpdates(ptGame, fUpdated);
		if (!ptGame->nNumClients)
		{
			if (!ptGame->dwEmptyTick)
				ptGame->dwEmptyTick = dwNow;
			if (dwNow - ptGame->dwEmptyTick > 300000 || (ptGame->nFrame > 1500 && ptGame->nFrame < 7500))
			{
				GameUnlock(ptGame);
				GameClose(dwGame);
				continue;
			}
		}
		else
			ptGame->dwEmptyTick = 0;
		GameUnlock(ptGame);
	}
	if (bAny)
	{
		QueryPerformanceCounter(&aliTime[0]);
		aliTime[1].QuadPart = aliTime[0].QuadPart - aliTime[1].QuadPart;
	}
	else
		aliTime[1].QuadPart = 0;
	return aliTime[1].LowPart;
}

GAME * __fastcall GameLockByClient(DWORD dwClientId)
{
	GAME *ptGame;

	ptGame = GameLockByHandle(D2NetServerGetValue(dwClientId));
	if (ptGame)
	{
		if (ClientsIsInGame(ptGame, dwClientId))
			return ptGame;
		GameUnlock(ptGame);
		return NULL;
	}
	return NULL;
}

void __fastcall GameUnlock(GAME *ptGame)
{
#line 2515
	D2ASSERT(ptGame);
#line 2516
	D2ASSERT(ptGame->ptLock != NULL);
	LeaveCriticalSection(ptGame->ptLock);
}

void __fastcall GameClose(DWORD dwGame)
{
	GAME *ptGame;
	int nLock;

	ptGame = sgptGameDataTbl->Lock(dwGame, &nLock, TRUE);
	if (!ptGame)
		return;
	if (ptEventCallbackTable)
	{
		if (ptEventCallbackTable->fpCloseGame)
		{
#line 2533
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)ptEventCallbackTable->fpCloseGame), "ptEventCallbackTable->fpCloseGame");
			((void (__fastcall *)(DWORD))ptEventCallbackTable->fpCloseGame)(ptGame->wGameId);
		}
		GameLog(0, "[SERVER]  Close Game: %s", ptGame->szGameName);
	}
#line 2542
	D2ASSERT(ptGame->ptLock != NULL);
	EnterCriticalSection(ptGame->ptLock);
	GameFree(dwGame, ptGame);
	sgptGameDataTbl->DeleteLocked(ptGame, nLock);
}

GAME * __fastcall GameLockByHandle(DWORD dwGame)
{
	GAME *ptGame;
	int nLock;

	ptGame = sgptGameDataTbl->Lock(dwGame, &nLock, FALSE);
	if (ptGame)
	{
#line 2573
		D2ASSERT(ptGame);
		D2ASSERT(ptGame->ptLock != NULL);
		EnterCriticalSection(ptGame->ptLock);
		sgptGameDataTbl->ReleaseLock(nLock);
	}
	return ptGame;
}

BOOL __fastcall GameReturnTrue(DWORD dwClientId)
{
	return TRUE;
}

void __fastcall GameSendGameList(DWORD dwClientId)
{
	GAME *ptGame;
	DWORD dwGame;
	WORD wGameId;
	WORD wNumClients;
	char szName[16];
	int i;

	for (i = 0; i < GAME_MAXGAMES; i++)
	{
		dwGame = sgdwGameHandles[i];
		if (!dwGame)
			continue;
		ptGame = GameLockByHandle(dwGame);
		if (!ptGame)
			continue;
		wNumClients = (WORD)ptGame->nNumClients;
		wGameId = ptGame->wGameId;
		StormSStrCopy(szName, ptGame->szGameName, 2147483647);
		GameUnlock(ptGame);
		LevelSendSysPlayer(dwClientId, wGameId, wNumClients, szName);
	}
	LevelSendSysPlayer(dwClientId, 65535, 0, &sgszEmpty);
}

BOOL __fastcall GameReceiveDataChunk(DWORD dwClientId, SRVPACKET *ptPacket)
{
	return ClientsAppendData(dwClientId, ptPacket->t5.abData, ptPacket->t5.bLength, ptPacket->t5.dwTotal);
}

void __cdecl GameCloseAll(void)
{
	GAME *ptGame;
	CLIENT *ptClient;
	CLIENT *ptNext;
	UNIT *hUnit;
	DWORD dwGame;
	int i;

	TaskFreeQueues();
	for (i = 0; i < GAME_MAXGAMES; i++)
	{
		dwGame = sgdwGameHandles[i];
		if (!dwGame)
			continue;
		ptGame = GameLockByHandle(dwGame);
#line 2674
		D2ASSERT(ptGame);
		for (ptClient = ptGame->ptClientList; ptClient; ptClient = ptNext)
		{
			SCmdSendSysGameEnd(ptClient->dwClientId);
			ptNext = ptClient->ptNextInGame;
			hUnit = ClientGetPlayerUnit(ptClient, FALSE);
			PlrSaveCharacter(ptGame, hUnit, ClientGetName(ptClient));
			GameFlushClientData(ptGame, ptClient);
			SCmdSendPumpEndNow(ptClient->dwClientId);
			ClientsRemove(ptGame, ptClient->dwClientId);
		}
		GuildsSave(ptGame);
		GameUnlock(ptGame);
		GameClose(dwGame);
	}
}

int __cdecl GameGetCount(void)
{
	int nGames;
	int i;

	nGames = 0;
	for (i = 0; i < GAME_MAXGAMES; i++)
		if (sgdwGameHandles[i])
			nGames++;
	return nGames;
}

DWORD __cdecl GameGetFps(void)
{
	return sgdwMeasuredFps;
}

int __stdcall GameGetNumClients(WORD wGameId)
{
	GAME *ptGame;
	int nNumClients;

	ptGame = GameLockByHandle(GameFindById(wGameId));
#line 2734
	D2ASSERT(ptGame);
	nNumClients = ptGame->nNumClients;
	GameUnlock(ptGame);
	return nNumClients;
}

static __inline int GameCountListUnitsI(UNIT **ppList)
{
	UNIT *hUnit;
	int nUnits;
	int j;

	nUnits = 0;
	for (j = 0; j < GAME_UNITBUCKETS; j++)
	{
		hUnit = ppList[j];
		if (hUnit)
		{
			nUnits++;
			for (hUnit = SUnitGetNextInList(hUnit); hUnit; hUnit = SUnitGetNextInList(hUnit))
				nUnits++;
		}
	}
	return nUnits;
}

BOOL __stdcall GameGetInfo(WORD wGameId, GAMEINFO *ptInfo)
{
	GAME *ptGame;
	UNIT **ppList;

	{
		DWORD dwGame = GameFindById(wGameId);
		ptGame = GameLockByHandle(dwGame);
	}
	if (!ptGame)
		return FALSE;
	ptInfo->dwGameId = ptGame->wGameId;
	ptInfo->dwSeed = ptGame->dwSeed;
	ptInfo->nNumClients = ptGame->nNumClients;
	ptInfo->dwNextPlayerId = ptGame->field_60;
	ptInfo->adwField2c[0] = ptGame->field_260;
	ptInfo->adwField2c[1] = ptGame->field_264;
	ptInfo->adwField2c[2] = ptGame->field_268;
	ptInfo->adwField2c[3] = ptGame->field_26c;
	ptInfo->adwField2c[4] = ptGame->field_270;
	ppList = ptGame->pUnitList[0];
	ptInfo->anUnits[0] = GameCountListUnitsI(ppList);
	ppList = ptGame->pUnitList[1];
	ptInfo->anUnits[1] = GameCountListUnitsI(ppList);
	ppList = ptGame->pUnitList[2];
	ptInfo->anUnits[2] = GameCountListUnitsI(ppList);
	ppList = ptGame->pUnitList[3];
	ptInfo->anUnits[3] = GameCountListUnitsI(ppList);
	ppList = ptGame->pUnitList[4];
	ptInfo->anUnits[4] = GameCountListUnitsI(ppList);
	if (ptGame->adwStats[16])
	{
		ptInfo->adwPercent[0] = ptGame->adwStats[2] * 100 / ptGame->adwStats[16];
		ptInfo->adwPercent[1] = ptGame->adwStats[5] * 100 / ptGame->adwStats[16];
		ptInfo->adwPercent[2] = ptGame->adwStats[6] * 100 / ptGame->adwStats[16];
		ptInfo->adwPercent[3] = ptGame->adwStats[1] * 100 / ptGame->adwStats[16];
		ptInfo->adwPercent[4] = ptGame->adwStats[15] * 100 / ptGame->adwStats[16];
		ptInfo->dwStatBase = ptGame->adwStats[16];
		if (ptGame->adwStats[16] > 200 && ptInfo->dwStatBase)
			memset(ptGame->adwStats, 0, 17 * sizeof(DWORD));
	}
	StormSStrCopy(ptInfo->szGameName, ptGame->szGameName, 16);
	ptInfo->dwMinutes = (GetTickCount() - ptGame->dwRate) / 1000 / 60;
	ptInfo->nFrames100 = ptGame->nFrame / 100;
	ptInfo->dwFramesPerSecond = ptGame->dwFramesPerSecond;
	ptInfo->bField47 = ptGame->bField47;
	ptInfo->bField48 = ptGame->bField48;
	ptInfo->bField4a = ptGame->bField4a;
	StormSStrCopy(ptInfo->szField36, ptGame->szField36, 16);
	ptInfo->dwArenaFlags = ArenaGetFlags(ptGame);
	ptInfo->pMemPool = ptGame->pMemPool;
	GameUnlock(ptGame);
	return TRUE;
}

int __stdcall GameGetIds(WORD *pwIds, int nMax)
{
	GAME *ptGame;
	DWORD dwGame;
	int nIds;
	int i;
	WORD *pwId;

	nIds = 0;
	pwId = pwIds;
	for (i = 0; i < GAME_MAXGAMES; i++)
	{
		if (nIds >= nMax)
			break;
		dwGame = sgdwGameHandles[i];
		if (!dwGame)
			continue;
		ptGame = GameLockByHandle(dwGame);
		if (!ptGame)
			continue;
		*pwId = ptGame->wGameId;
		nIds++;
		pwId++;
		GameUnlock(ptGame);
	}
	return nIds;
}

int __stdcall GameCountPlayers(WORD wGameId)
{
	GAME *ptGame;
	UNIT **ppList;
	int nPlayers;

	ptGame = GameLockByHandle(GameFindById(wGameId));
	nPlayers = 0;
	if (ptGame)
	{
		ppList = ptGame->pUnitList[UNIT_PLAYER];
		nPlayers = GameCountListUnitsI(ppList);
		GameUnlock(ptGame);
	}
	return nPlayers;
}

int __stdcall GameGetPlayerInfo(WORD wGameId, GAMEUNITINFO *ptInfo,
	int nMax)
{
	GAME *ptGame;
	UNIT *hUnit;
	int nPlayers;

	{
		DWORD dwGame;

		dwGame = GameFindById(wGameId);
		ptGame = GameLockByHandle(dwGame);
	}
	nPlayers = 0;
	if (ptGame)
	{
		int i;

		for (i = 0; i < GAME_UNITBUCKETS; i++)
		{
			if (nPlayers >= nMax)
				break;
			for (hUnit = ptGame->pUnitList[UNIT_PLAYER][i]; hUnit && nPlayers < nMax;
				hUnit = SUnitGetNextInList(hUnit))
			{
				SUnitGetInfo(hUnit, &ptInfo[nPlayers++]);
			} }
		GameUnlock(ptGame); }
	return nPlayers;
}

void __fastcall GameDescribePlayer(char *szOut, DWORD dwClassId)
{
	switch (dwClassId)
	{
	case 0:
		strcpy(szOut, "Amazon");
		return;
	case 1:
		strcpy(szOut, "Sorceress");
		return;
	case 2:
		strcpy(szOut, "Necromancer");
		return;
	case 3:
		strcpy(szOut, "Paladin");
		return;
	case 4:
		strcpy(szOut, "Barbarian");
		return;
	default:
		strcpy(szOut, "Invalid Class");
		return;
	}
}

void __fastcall GameDescribeMonster(char *szOut, DWORD dwClassId)
{
	const char *szText;

	szText = (const char *)D2CommonMonsterTblsGetStats(dwClassId);
	if (szText)
		strcpy(szOut, szText);
	else
		sprintf(szOut, "Monster %d", dwClassId);
}

void __fastcall GameDescribeObject(char *szOut, DWORD dwClassId)
{
	const char *szText;

	szText = (const char *)D2CommonObjTblsGetObject(dwClassId);
	if (szText)
		strcpy(szOut, szText);
	else
		sprintf(szOut, "Object %d", dwClassId);
}

void __fastcall GameDescribeItem(char *szOut, DWORD dwClassId)
{
	GAMEITEMSTXT *ptItemsTxt;

	ptItemsTxt = (GAMEITEMSTXT *)D2CommonItemTblsGetItemStats(dwClassId);
	if (ptItemsTxt)
		strcpy(szOut, ptItemsTxt->szName);
	else
		sprintf(szOut, "Item %d", dwClassId);
}

void __fastcall GameDescribeMissile(char *szOut, DWORD dwClassId)
{
	sprintf(szOut, "Missile %d", dwClassId);
}

static void GameCountUnitClass(GAMEUNITCLASS *ptHead, DWORD dwClassId)
{
	GAMEUNITCLASS *ptList;
	GAMEUNITCLASS *ptClass;
	int j;

	ptList = ptHead;
#line 2916
	D2ASSERT(ptList);
	for (; ptList; ptList = ptList->ptNext)
		if (ptList->dwClassId == dwClassId)
		{
			ptList->nCount++;
			return;
		}
#line 2933
	ptClass = (GAMEUNITCLASS *)D2ALLOCPOOL(NULL, sizeof(GAMEUNITCLASS));
	if (ptClass)
	{
		for (j = 0; j < (int)(sizeof(GAMEUNITCLASS) / sizeof(DWORD)); j++)
			((DWORD *)ptClass)[j] = 0;
		ptClass->dwClassId = dwClassId;
		ptClass->field_08 = 0;
		ptClass->nCount = 1;
		ptClass->ptNext = ptHead->ptNext;
		ptHead->ptNext = ptClass;
	}
}

void __stdcall GameCountUnitClasses(WORD wGameId,
	GAMEUNITCLASS *ptHead,
	int eType  )
{
	GAME *ptGame;
	GAMEUNITCLASS *ptClass;
	UNIT **phUnits;
	UNIT *hUnit;
	int i;

	{
		DWORD dwGame;

		dwGame = GameFindById(wGameId);
		ptGame = GameLockByHandle(dwGame);
	}
	phUnits = NULL;
	if (!ptGame)
		return;
	switch (eType)
	{
	case 0:
		phUnits = ptGame->pUnitList[0];
		break;
	case 1:
		phUnits = ptGame->pUnitList[1];
		break;
	case 2:
		phUnits = ptGame->pUnitList[2];
		break;
	case 3:
		phUnits = ptGame->pUnitList[3];
		break;
	case 4:
		phUnits = ptGame->pUnitList[4];
		break;
	}
	for (i = 0; i < GAME_UNITBUCKETS; i++)
	{
		for (hUnit = phUnits[i]; hUnit; hUnit = SUnitGetNextInList(hUnit))
			GameCountUnitClass(ptHead, hUnit->dwClassId);
	}
	GameUnlock(ptGame);
#line 3074
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgpfnGetDescription[eType]), "sgpfnGetDescription[eType]");
	for (ptClass = ptHead->ptNext; ptClass; ptClass = ptClass->ptNext)
		sgpfnGetDescription[eType](ptClass->szDesc, ptClass->dwClassId);
}

int __stdcall GameGetUnitInfo(WORD wGameId, int nCount,
	GAMEUNITINFO *ptInfo, DWORD *peTOUs  ,
	DWORD *pdwIds)
{
	GAME *ptGame;
	UNIT *hUnit;
	int nFound;
	int i;

	{
		DWORD dwGame;

		dwGame = GameFindById(wGameId);
		ptGame = GameLockByHandle(dwGame);
	}
	nFound = 0;
	if (!ptGame)
		return 0;
	for (i = 0; i < nCount; i++)
	{
		hUnit = SUnitFind(ptGame, peTOUs[i], pdwIds[i]);
		if (!hUnit)
			continue;
		SUnitGetInfo(hUnit, &ptInfo[nFound]);
		if (peTOUs[i])
		{
#line 3106
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgpfnGetDescription[peTOUs[i]]), "sgpfnGetDescription[peTOUs[i]]");
			sgpfnGetDescription[peTOUs[i]](ptInfo[nFound].szDesc, hUnit->dwClassId);
		}
		nFound++;
	}
	GameUnlock(ptGame);
	return nFound;
}

void __stdcall GameCountAllUnits(GAMECENSUS *ptCensus)
{
	GAME *ptGame;
	UNIT *hUnit;
	DWORD dwGame;
	int i;
	int j;

	for (i = 0; i < (int)(sizeof(GAMECENSUS) / sizeof(DWORD)); i++)
		((DWORD *)ptCensus)[i] = 0;

	for (i = 0; i < GAME_MAXGAMES; i++)
	{
		dwGame = sgdwGameHandles[i];
		if (!dwGame)
			continue;
		ptGame = GameLockByHandle(dwGame);
		if (!ptGame)
			continue;
		for (j = 0; j < GAME_UNITBUCKETS; j++)
		{
			for (hUnit = ptGame->pUnitList[1][j]; hUnit; hUnit = SUnitGetNextInList(hUnit))
				ptCensus->nMonsters++;
			for (hUnit = ptGame->pUnitList[UNIT_PLAYER][j]; hUnit; hUnit = SUnitGetNextInList(hUnit))
				ptCensus->nPlayers++;
		}
		GameUnlock(ptGame);
		ptCensus->nGames++;
	}
}

void __stdcall GameBroadcast(BOOL bEvent, DWORD dwArg,
	char *szText)
{
	GAME *ptGame;
	GAMEPLAYERMSG tMsg;
	char szBuffer[256];
	DWORD dwGame;
	int i;

	for (i = 0; i < GAME_MAXGAMES; i++)
	{
		dwGame = sgdwGameHandles[i];
		if (!dwGame)
			continue;
		ptGame = GameLockByHandle(dwGame);
#line 3180
		D2ASSERT(ptGame);
		if (bEvent)
		{
			tMsg.dwArg = dwArg;
			tMsg.bCmd = S2C_ROSTER_EVENT;
			tMsg.bEvent = 15;
			tMsg.bColor = 4;
			tMsg.szAccount[0] = '\0';
			PlrMsgSendSystemMsgAll(ptGame, &tMsg);
		}
		else if (strlen(szText) > 255)
		{
			strncpy(szBuffer, szText, 255);
			szBuffer[255] = '\0';
			PlrMsgSendTextAll(ptGame, szBuffer, 1);
		}
		else
			PlrMsgSendTextAll(ptGame, szText, 1);
		GameUnlock(ptGame);
	}
}

void __stdcall GameSendText(WORD wGameId, char *szText)
{
	GAME *ptGame;

	ptGame = GameLockByHandle(GameFindById(wGameId));
#line 3220
	D2ASSERT(ptGame);
	szText[255] = '\0';
	PlrMsgSendTextAll(ptGame, szText, 1);
	GameUnlock(ptGame);
}

void __fastcall GameForEachClient(GAME *ptsGame  ,
	void (__fastcall *pfn)(CLIENT *, void *)  ,
	void *pArg)
{
	CLIENT *ptClient;

#line 3233
	D2ASSERT(ptsGame);
#line 3234
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)pfn), "pfn");
	for (ptClient = ptsGame->ptClientList; ptClient; ptClient = ptClient->ptNextInGame)
		if (ptClient->dwState == 4)
			pfn(ptClient, pArg);
}

GAME * __fastcall GameLockFirst(void)
{
	GAME *ptGame;

	ptGame = GameLockByHandle(GameFindById(1));
	sgptFirstGame = ptGame;
	return ptGame;
}

void __fastcall GameUnlockFirst(void)
{
	GameUnlock(sgptFirstGame);
}

int __stdcall GameSetFps(int nFps)
{
	int nFrameMs;

	nFrameMs = 1000 / nFps;
	sgnFps = nFps;
	sgnFrameMs = nFrameMs;
	return nFrameMs;
}

DWORD __stdcall GameSetStartAct(DWORD dwAct)
{
	sgdwStartAct = dwAct;
	return dwAct;
}

void __fastcall GameUnitGone(UNIT *hUnit, GAME *ptsGame)
{
	GameForEachClient(ptsGame, (void (__fastcall *)(CLIENT *, void *))GameClientUnitGone, hUnit);
	SUnitFree(ptsGame, hUnit);
}

void __fastcall GameClientUnitGone(CLIENT *ptClient, UNIT *hUnit)
{
	GAMEROOM *hRoom;
	GAMEROOM **phClientRooms;
	DWORD nRooms;
	DWORD i;

	if (!hUnit)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	D2CommonDungeonRoomGetNear(ptClient->hRoom, &phClientRooms, &nRooms);
	for (i = 0; i < nRooms; i++)
	{
#line 3307
		D2ASSERT(phClientRooms[i]);
		if (hRoom == phClientRooms[i])
		{
			SCmdSendUnitRemove(ptClient, S2C_UNIT_DESPAWN, (BYTE)hUnit->eType, hUnit->dwId);
			return;
		}
	}
}
