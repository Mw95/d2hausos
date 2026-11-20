#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\GAME\\Level.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "Arena.h"
#include "Clients.h"
#include "Event.h"
#include "Level.h"
#include "SCmd.h"
#include "../INVENTORY/InvMode.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/Monster.h"
#include "../QUESTS/a2q3.h"
#include "../MONSTER/MonsterMode.h"
#include "../OBJECTS/ObjMode.h"
#include "../OBJECTS/Objects.h"
#include "../PLAYER/Player.h"
#include "../PLAYER/PlayerPets.h"
#include "../PLAYER/PlrModes.h"
#include "../PLAYER/PlrTrade.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitMsg.h"
#include "../UNIT/sunitproxy.h"
#include "../GUILDS/Guilds.h"
#include "D2NetImports.h"

#define NUM_LEVEL_IDS		109
#define UNIT_MONSTER		1
#define UNIT_OBJECT			2
#define UNIT_MISSILE		3
#define UNIT_ITEM			4
#define UNITFLAG0_SENT		16
#define UNITFLAG_TELEPORTED	65536

struct DUNGEONROOMNODE
{
 	DWORD		dwType;
 	DWORD		dwUnitId;
 	struct DUNGEONROOMNODE	*pNext;
};

static inline void LevelSendChangedUnit(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient)
{
	DWORD eType;
	BOOL fSent;

	D2_OPAQUE(hUnit);
#line 21
	D2ASSERT(hUnit);
	eType = hUnit->eType;
	fSent = FALSE;
	if ((BYTE)hUnit->dwFlags0 & UNITFLAG0_SENT && ClientGetPlayerUnit(ptClient, FALSE) != hUnit)
	{
		if (eType == UNIT_MISSILE)
			return;
		SUnitMsgSendUnit(ptGame, hUnit, ptClient);
		fSent = TRUE;
	}
	switch (hUnit->eType)
	{
	case UNIT_PLAYER:
		PlrModeSendUnit(ptGame, hUnit, ptClient, fSent);
		break;
	case UNIT_MONSTER:
		MonsterModeUpdateClient(ptGame, hUnit, ptClient);
		break;
	case UNIT_OBJECT:
		ObjectModeSendToClient(ptGame, hUnit, ptClient);
		break;
	case UNIT_ITEM:
		INVENTORYModeSendChangedItem(hUnit, ptClient);
		break;
	}
}

void __fastcall LevelSendNearUnits(GAME *ptGame, GAMEROOM *hRoomCenter, CLIENT *ptClient)
{
	GAMEROOM **pptNear;
	DWORD dwNumNear;
	DWORD i;
	GAMEROOM *hRoom;
	UNIT *hUnit;
	UNIT *hNext;

	if (!((ACT *)ptGame->ptAct[ClientGetAct(ptClient)])->dwUnitsChanged)
		return;
	D2CommonDungeonRoomGetNear(hRoomCenter, &pptNear, &dwNumNear);
	for (i = 0; i < dwNumNear; i++)
	{
		hRoom = pptNear[i];
#line 68
		D2ASSERT(hRoom);
		for (hUnit = hRoom->ptUnitChanged; hUnit; hUnit = hNext)
		{
			hNext = hUnit->ptChangedNext;
			LevelSendChangedUnit(ptGame, hUnit, ptClient);
		}
	}
}

void __fastcall LevelSendNearRemovals(GAMEROOM *hRoomCenter, CLIENT *ptClient)
{
	GAMEROOM **pptNear;
	DWORD dwNumNear;
	DWORD i;
	UNIT *hUnit;
	DWORD dwType;
	DWORD dwUnitId;
	DUNGEONROOMNODE *pNode;

	D2CommonDungeonRoomGetNear(hRoomCenter, &pptNear, &dwNumNear);
	hUnit = ClientGetPlayerUnit(ptClient, FALSE);
#line 133
	D2ASSERT(hUnit);
	dwType = hUnit->eType;
	dwUnitId = hUnit->dwId;
	for (i = 0; i < dwNumNear; i++)
	{
		for (pNode = (DUNGEONROOMNODE *)D2CommonDungeonRoomGetNodes(pptNear[i]); pNode; pNode = pNode->pNext)
		{
			if (pNode->dwType != dwType || pNode->dwUnitId != dwUnitId)
				SCmdSendUnitRemove(ptClient, S2C_UNIT_DESPAWN, (BYTE)pNode->dwType, pNode->dwUnitId);
		}
	}
}

void __fastcall LevelFreeRoomNodes(GAME *ptGame)
{
	ACT **pptAct;
	ACT *ptAct;
	GAMEROOM *hRoom;
	int nActs;

	pptAct = (ACT **)ptGame->ptAct;
	for (nActs = NUM_ACTS; nActs; nActs--, pptAct++)
	{
		ptAct = *pptAct;
		if (!ptAct || !ptAct->dwRoomNodesAdded)
			continue;
		for (hRoom = (GAMEROOM *)D2CommonDungeonActGetFirstActiveRoom(ptAct); hRoom;
			hRoom = (GAMEROOM *)D2CommonDungeonActGetNextActiveRoom(ptAct, hRoom))
			D2CommonDungeonRoomFreeNodes(hRoom);
		ptAct->dwRoomNodesAdded = 0;
	}
}

void __fastcall LevelRoomEnterView(GAME *ptGame, GAMEROOM *hRoom, CLIENT *ptClient)
{
	GAMERECT atRect[2];
	UNIT *hUnit;
	UNIT *hPlayer;

#line 204
	D2ASSERT(hRoom);
	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	SCmdSendRoom(ptClient, D2CommonDungeonRoomGetLevelId(hRoom), (WORD)atRect[1].nX,
		(WORD)atRect[1].nY);
	D2CommonDungeonRoomAddClient(hRoom, ptClient);
	if (hRoom->nNumClients <= 1)
	{
		for (hUnit = hRoom->ptUnitFirst; hUnit; hUnit = hUnit->ptRoomNext)
			switch (hUnit->eType) { case UNIT_MONSTER:
				MonsterRestartAi(ptGame, hUnit); break; }
	}
	hUnit = hRoom->ptUnitFirst;
	hPlayer = ClientGetPlayerUnit(ptClient, FALSE);
	for (; hUnit; hUnit = hUnit->ptRoomNext)
		if (hPlayer != hUnit)
			SUnitMsgSendUnit(ptGame, hUnit, ptClient);
}

void __fastcall LevelRoomLeaveView(GAME *ptGame, GAMEROOM *hRoom, CLIENT *ptClient)
{
	GAMERECT atRect[2];
	UNIT *hUnit;

#line 257
	D2ASSERT(hRoom);
	for (hUnit = hRoom->ptUnitFirst; hUnit; hUnit = hUnit->ptRoomNext)
		SUnitMsgSendRemove(hUnit, ptClient);
	D2CommonDungeonRoomRemoveClient(hRoom, ptClient);
	if (!hRoom->nNumClients)
	{
		for (hUnit = hRoom->ptUnitFirst; hUnit; hUnit = hUnit->ptRoomNext)
			switch (hUnit->eType) { case UNIT_MONSTER:
				MonsterRemoveEvents(ptGame, hUnit); break; }
	}
	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	SCmdSendRoomLeave(ptClient, D2CommonDungeonRoomGetLevelId(hRoom), (WORD)atRect[1].nX,
		(WORD)atRect[1].nY);
}

void __fastcall LevelUnitNotifyNear(GAME *ptGame, GAMEROOM *hRoomCenter, UNIT *hUnit)
{
	GAMEROOM **pptNear;
	DWORD dwNumNear;
	DWORD i;
	GAMEROOM *hRoom;
	UNIT *hOther;

	D2CommonDungeonRoomGetNear(hRoomCenter, &pptNear, &dwNumNear);
	for (i = 0; i < dwNumNear; i++)
	{
		hRoom = pptNear[i];
#line 302
		D2ASSERT(hRoom);
		for (hOther = hRoom->ptUnitFirst; hOther; hOther = hOther->ptRoomNext)
		{
			switch (hOther->eType)
			{
			case UNIT_PLAYER:
				PlayerClearTarget(ptGame, hOther, hUnit);
				break;
			case UNIT_MONSTER:
				MonsterClearTarget(ptGame, hOther, hUnit);
				break;
			}
		}
	}
}

void __fastcall LevelRemoveClientNear(GAMEROOM *hRoomCenter, CLIENT *ptClient)
{
	GAMEROOM **pptNear;
	DWORD dwNumNear;
	DWORD i;

	D2CommonDungeonRoomGetNear(hRoomCenter, &pptNear, &dwNumNear);
	for (i = 0; i < dwNumNear; i++)
		D2CommonDungeonRoomRemoveClient(pptNear[i], ptClient);
}

void __fastcall LevelSendActInit(GAME *ptGame, CLIENT *ptClient)
{
	BYTE bAct;
	ACT **pptAct;
	DWORD dwLevelId;
	void *pArenaStart;
	SCMDENVPACKET tPacket;
	int nCycle;
	int nAngle;
	BOOL fAlt;

	bAct = ClientGetAct(ptClient);
	pptAct = &ptGame->ptAct[bAct];
	dwLevelId = D2CommonDungeonActGetInitialLevelId(*pptAct);
	if (bAct == 4)
	{
		pArenaStart = GuildsGetData(ptGame);
		if (pArenaStart)
			dwLevelId = D2CommonGuildsGetRankLevelId(pArenaStart);
	}
	SCmdSendLoadAct(ptClient, S2C_LEVEL_LOAD, bAct, ptGame->dwSeed, ptGame->dwObjRegionSeed, (WORD)dwLevelId);
	D2CommonEnvGetState(*pptAct, &nCycle, &nAngle, &fAlt);
	tPacket.bCmd = S2C_ROOM_OP;
	tPacket.dwCycle = nCycle;
	tPacket.dwAngle = nAngle;
	tPacket.bAlt = (BYTE)fAlt;
	SCmdSendEnv(ptClient, &tPacket);
}

void __fastcall LevelChangeAct(GAME *ptGame, CLIENT *ptClient, DWORD dwLevelId,
	DWORD dwArg)
{
	BYTE bNewAct;
	UNIT *hUnit;
	GAMEROOM *hRoom;
	GAMEROOM *hOldRoom;
	void *ptPath;
	int nX;
	int nY;
	BOOL fRet;

	if (ArenaTestNoActChange(ptGame))
		return;
	bNewAct = D2CommonDrlgGetActNum(0, dwLevelId);
#line 416
	D2ASSERT(bNewAct != ptClient->bAct);
	LevelLoadAct(ptGame, bNewAct);
	ClientSetState(ptClient, 5);
	hUnit = ClientGetPlayerUnit(ptClient, FALSE);
#line 427
	D2ASSERT(hUnit);
	if (hUnit->eType == UNIT_PLAYER)
		PlayerPetsClearAll(ptGame, hUnit);
	hRoom = (GAMEROOM *)D2CommonDungeonGetActStartRoom(ptGame->ptAct[bNewAct], dwLevelId, dwArg, &nX,
		&nY, D2CommonUnitGetSizeX(hUnit));
	if (!hRoom)
		return;
	ptPath = hUnit->ptPath;
	hOldRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	fRet = D2CommonStepTeleportUnit(ptPath, hUnit, 0, 0, 0);
#line 442
	D2ASSERT(fRet);
	SUnitUpdateClients(ptGame, hUnit, hOldRoom);
	fRet = D2CommonStepTeleportUnit(ptPath, hUnit, hRoom, nX, nY);
#line 447
	D2ASSERT(fRet);
	ClientSetRoom(ptClient, NULL);
	SCmdSendCmd(ptClient, S2C_ACT_LEAVE);
	ClientSetAct(ptClient, bNewAct);
	hUnit->bAct = bNewAct;
	LevelSendActInit(ptGame, ptClient);
	ClientSetRoom(ptClient, hRoom);
	D2CommonUnitRoomAddToChangedList(hUnit);
	hUnit->dwFlags |= UNITFLAG_TELEPORTED;
	SUnitUpdateClients(ptGame, hUnit, NULL);
	D2CommonUnitRoomAddToChangedList(hUnit);
}

void __fastcall LevelWarpUnit(GAME *ptGame, UNIT *hUnit, DWORD dwLevelId,
	DWORD dwArg)
{
	BYTE bAct;
	CLIENT *hClient;
	GAMEROOM *hRoom;
	DWORD dwOldLevelId;
	int nX;
	int nY;

	bAct = D2CommonDrlgGetActNum(0, dwLevelId);
	hClient = SUnitGetClient(hUnit);
#line 479
	D2ASSERT(hClient);
	dwOldLevelId = 0;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (hRoom)
		dwOldLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
	if (bAct != ClientGetAct(hClient))
	{
		SUnitProxyChangeLevel(ptGame, hUnit, dwOldLevelId, dwLevelId);
		LevelChangeAct(ptGame, hClient, dwLevelId, dwArg);
		return;
	}
	hRoom = (GAMEROOM *)D2CommonDungeonGetActStartRoom(ptGame->ptAct[bAct], dwLevelId, dwArg, &nX,
		&nY, D2CommonUnitGetSizeX(hUnit));
	if (hRoom)
		SUnitMoveToRoom(ptGame, hUnit, hRoom, nX, nY, 0, 0);
}

extern void LevelInitActI(GAME *ptGame, char bAct);
__inline void LevelInitActI(GAME *ptGame, char bAct)
{
	int eLevelId;

	if (ArenaHasGuild(ptGame))
		eLevelId = D2CommonGuildsGetRankLevelId(ptGame->pGuildData);
	else
		eLevelId = D2CommonDungeonActGetTownLevelId(bAct);
#line 394
	D2ASSERT(eLevelId < NUM_LEVEL_IDS);
	ptGame->ptAct[(BYTE)bAct] = (ACT *)((DWORD (__stdcall *)(BYTE, DWORD, DWORD, GAME *, void *, int, DWORD,
		DWORD))D2CommonOrdinal10037)(bAct, ptGame->dwSeed, 0, ptGame, ptGame->pMemPool, eLevelId, 0, 0);
	A2Q3ActLoaded(ptGame, bAct);
}

void __fastcall LevelLoadAct(GAME *ptGame, BYTE bAct)
{
	if (!ptGame->ptAct[bAct])
		LevelInitActI(ptGame, bAct);
}

void __fastcall LevelFreeGameUnits(GAME *ptGame)
{
	GameFreeMonsterUnits(ptGame);
	ObjectFreeAll(ptGame);
	PlayerFreeAll(ptGame);
	GameFreeMissileUnits(ptGame);
	ITEMSFreeAll(ptGame);
	SUnitFreeTiles(ptGame);
}

void __fastcall LevelUpdateChangedUnits(GAME *ptGame)
{
	ACT **pptAct;
	GAMEROOM *hRoom;
	UNIT *hUnit;
	int nActs;

	pptAct = ptGame->ptAct;
	for (nActs = NUM_ACTS; nActs; nActs--, pptAct++)
	{
		if (!*pptAct || !((ACT *)*pptAct)->dwUnitsChanged)
			continue;
		for (hRoom = (GAMEROOM *)D2CommonDungeonActGetFirstRoom(*pptAct); hRoom; hRoom = hRoom->ptActNext)
		{
			for (hUnit = hRoom->ptUnitChanged; hUnit; hUnit = hUnit->ptChangedNext)
				SUnitReset(ptGame, hUnit);
			D2CommonUnitRoomClearChangedList(hRoom);
		}
		((ACT *)*pptAct)->dwUnitsChanged = 0;
	}
	ArenaClearTagSendFlag(ptGame);
	PlrTradeUpdateAll(ptGame);
}

#pragma pack(push, 1)
struct LEVELSYSPACKET
{
	BYTE		bCmd;
	char		szName[16];
	BYTE		unobserved_11[49 - 17];
	WORD		wNumPlayers;
	WORD		wGameId;
};
#pragma pack(pop)

DWORD __fastcall LevelSendSysPlayer(DWORD dwClientId, WORD wGameId,
	WORD wNumPlayers, const char *szName)
{
	LEVELSYSPACKET tPacket;

	tPacket.wGameId = wGameId;
	tPacket.bCmd = 3;
	tPacket.wNumPlayers = wNumPlayers;
	StormSStrCopy(tPacket.szName, szName, 16);
	return D2NetServerSend(0, dwClientId, &tPacket, sizeof(tPacket));
}
