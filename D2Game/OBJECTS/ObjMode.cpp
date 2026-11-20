#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\OBJECTS\\ObjMode.cpp"

#include <string.h>
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "../GAME/Level.h"
#include "../GAME/SCmd.h"
#include "../INVENTORY/InvMode.h"
#include "../ITEMS/ItemMode.h"
#include "../ITEMS/Items.h"
#include "../MISSILES/Missiles.h"
#include "../MONSTER/MonsterUnique.h"
#include "Objects.h"
#include "ObjMode.h"
#include "objrgn.h"
#include "../PLAYER/Player.h"
#include "../PLAYER/PlayerPets.h"
#include "../PLAYER/PlrModes.h"
#include "../PLAYER/PlrMsg.h"
#include "../PLAYER/PlrSave.h"
#include "../QUESTS/QUESTS.h"
#include "../QUESTS/a1q3.h"
#include "../QUESTS/a1q4.h"
#include "../QUESTS/a1q5.h"
#include "../QUESTS/a2q3.h"
#include "../QUESTS/a2q4.h"
#include "../QUESTS/a2q5.h"
#include "../QUESTS/a2q6.h"
#include "../QUESTS/a3q1.h"
#include "../QUESTS/a3q2.h"
#include "../QUESTS/a3q3.h"
#include "../QUESTS/a3q5.h"
#include "../QUESTS/a4q3.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitMsg.h"
#include "../UNIT/SUNITNPC.h"
#include "../GUILDS/Guilds.h"
#include "../SKILLS/Skills.h"
#include "FogImports.h"

#define UNIT_MONSTER		1
#define UNIT_OBJECT			2
#define UNIT_MISSILE		3
#define UNIT_ITEM			4
#define UNIT_ROOMTILE		5
#define NUM_EVENTS			13
#define OBJECT_TRAPPEDFIREBOLT			2
#define OBJECT_PARAM_WELL_REGEN_PER_MODE	2

struct SCMDCOPY7;
struct SCMDCOPY21;
void __fastcall SCmdSendWaypoints(CLIENT *ptClient, const struct SCMDCOPY21 *ptPacket);
void __fastcall SCmdSendUnitModeValue(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, BYTE bMode, BYTE bField07,
	DWORD dwObjectMode);
void __fastcall SCmdSendUnitModeAt(CLIENT *ptClient, BYTE bCmd, BYTE bTOU, DWORD dwUnitId,
	BYTE bMode, WORD wX, WORD wY,
	BYTE bHitClass, BYTE bLife);
void __fastcall SCmdSendTradeEvent(CLIENT *ptClient, BYTE bValue);
int __fastcall SCmdSendObject(CLIENT *ptClient, UNIT *hObject);
void __fastcall SCmdSendGuildCounter(CLIENT *ptClient, BYTE bCounter,
	WORD wLength,
	const BYTE *pbData);

typedef unsigned __int64 (__stdcall *PFNGUILDSGETCOUNTER)(DWORD hGuildData);

void __fastcall ObjectModeEventQuest(GAME *ptGame, UNIT *hObject);
static const PFNOBJEVENT scpfnEventFunctions[NUM_EVENTS] =
{
	ObjectModeEventProximity,
	ObjectModeEventEndOperate,
	ObjectModeEventRegenerate,
	ObjectModeEventCycle,
	ObjectModeEventTrap,
	ObjectModeEventShrineReset,
	PlrModeEventChat,
	ObjectModeEventQuest,
	ObjectEvalUpdate,
	ObjectModeEventDrop,
	ObjectModeEventDelayedDrop,
	NULL,
	NULL,
};

static PFNOBJTRAP sgObjectTrapHandlers[] =
{
	NULL,
	ObjectModeTrapChainLightning,
	ObjectModeTrapFireBolt,
	ObjectModeTrapPoison,
	ObjectModeTrapNova,
	ObjectModeTrapFire,
	ObjectModeTrapFireBolt,
	ObjectModeTrapFire,
	ObjectModeTrapRegionMonster,
	ObjectModeTrapRegionMonster,
};
static int sgnObjectTrapHandlers = arrsize(sgObjectTrapHandlers);
static PFNOBJECTOPERATE sgOperateFunctionTable[NUM_OPERATE_FUNCTIONS] =
{
	NULL,
	ObjectModeOperateCasket,
	ObjectModeOperateShrine,
	ObjectModeOperateUrn,
	ObjectModeOperateChest,
	ObjectModeOperateBarrel,
	A1Q5OperateObject,
	ObjectModeOperateExplodingBarrel,
	ObjectModeOperateDoor,
	A1Q4OperateStone,
	A1Q4OperateObject,
	ObjectModeOperateBrazier,
	A1Q4OperateTree,
	ObjectModeOperateTorch,
	ObjectModeOperateCrate,
	ObjectModeOperateTownPortal,
	ObjectModeOperateTrapDoor,
	ObjectModeOperateObelisk,
	ObjectModeOperateSecretDoor,
	ObjectModeOperateArmorStand,
	ObjectModeOperateWeaponRack,
	A1Q3OperateObject,
	ObjectModeOperateWell,
	ObjectModeOperateWaypoint,
	A2Q3OperateObject,
	A2Q6OperateObject,
	ObjectModeOperateBookshelf,
	ObjectModeOperateTeleportPad,
	A3Q1OperateObject,
	ObjectModeOperateSlimeDoor,
	ObjectModeOperateCow,
	A3Q3OperateObject,
	PlrSaveOperateStash,
	ObjectModeOperateWirt,
	ObjectModeOperateSanctuaryPortal,
	ObjectModeOperateGuildStone,
	ObjectModeOperateGuildVault,
	ObjectModeOperateTrophyCase,
	ObjectModeOperateMessageBoard,
	A2Q2OperateBox,
	A2Q2OperateScroll,
	A2Q2OperateShaft,
	A2Q4OperateTome,
	ObjectModeOperateLevelPortal,
	A3Q2OperateTiles,
	A3Q2OperateObject,
	ObjectModeOperateHellgate,
	ObjectModeOperateStairs,
	ObjectModeOperateTreasureChest,
	A4Q3OperateHellforge,
	ObjectModeOperateStairsR,
	ObjectModeOperateDelayedDrop,
	A4Q3OperateObject,
	A3Q5OperateObject,
	A4Q3OperateSealFirst,
	A4Q3OperateSealSecond,
	A4Q3OperateSealThird,
	A3Q2OperateHeartChest,
	A3Q2OperateEyeChest,
	A3Q2OperateBrainChest,
};

struct SHRINECHART
{
 	PFNSHRINE	pfnHandler;
 	int			nStat;
 	int			nState;
};
typedef struct SHRINECHART SHRINECHART;

static const SHRINECHART sgcptShrineChart[] =
{
	{ NULL,                          -1,   0 },
	{ ObjectModeShrineRefill,           -1,   0 },
	{ ObjectModeShrineRefillLife,       -1,   0 },
	{ ObjectModeShrineRefillMana,       -1,   0 },
	{ ObjectModeShrineLifeToMana,       -1,   0 },
	{ ObjectModeShrineManaToLife,       -1,   0 },
	{ ObjectModeShrineStat,             171, 128 },
	{ ObjectModeShrineCombat,           -1,   129 },
	{ ObjectModeShrineStat,             39, 131 },
	{ ObjectModeShrineStat,             43, 132 },
	{ ObjectModeShrineStat,             41, 130 },
	{ ObjectModeShrineStat,             45, 133 },
	{ ObjectModeShrineSkill,            11, 134 },
	{ ObjectModeShrineStat,             27, 135 },
	{ ObjectModeShrineStatDouble,       11, 136 },
	{ ObjectModeShrineStat,             -1,   137 },
	{ ObjectModeShrineReverse,          -1,   0 },
	{ ObjectModeShrinePortal,           -1,   0 },
	{ ObjectModeShrineGem,              -1,   0 },
	{ ObjectModeShrineExploding,        -1,   0 },
	{ ObjectModeShrineMonster,          -1,   0 },
	{ ObjectModeShrineItemsOpm,         -1,   0 },
	{ ObjectModeShrineItemsGpm,         -1,   0 },
};
static int sgnShrineChartSize = arrsize(sgcptShrineChart);

void __fastcall ObjectModeEventEndOperate(GAME *ptGame, UNIT *hObject)
{
	MISSOBJECTSTXT *ptTxt;

	if ((WORD)hObject->dwMode == 1)
	{
		ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hObject->dwClassId);
		if (ptTxt->Mode[2])
		{
			hObject->dwMode = 2;
			if (!ptTxt->HasCollision[2])
				D2CommonUnitClearCollision(hObject);
		}
	}
}

void __fastcall ObjectModeEventShrineReset(GAME *ptGame, UNIT *hObject)
{
	if (D2CommonUnitTestObjectSubclassBitZero(hObject))
	{
		D2CommonUnitRoomAddToChangedList(hObject);
		hObject->dwFlags0 |= 1;
		D2CommonUnitSetMode(hObject, 0);
		OBJDATA(hObject)->dwShrineUser = 0;
	}
}

void __fastcall ObjectModeEventRegenerate(GAME *ptGame, UNIT *hObject)
{
	BYTE bLeft;
	MISSOBJECTSTXT *ptTypeData;
	BYTE bMax;
	BYTE bModes;

	bLeft = OBJDATA(hObject)->field_04;
	ptTypeData = (MISSOBJECTSTXT *)D2CommonUnitGetObjectClassInfoPtr(hObject);

	if (bLeft / ptTypeData->Param[OBJECT_PARAM_WELL_REGEN_PER_MODE] >= ObjectRegenerateModes())
	{
#line 171
		D2ASSERT(bLeft/ptTypeData->Param[OBJECT_PARAM_WELL_REGEN_PER_MODE] < ObjectRegenerateModes());
		return;
	}
	OBJDATA(hObject)->field_04 = bLeft + 1;
	if (ObjectRegenerateMax(ptTypeData) - bLeft < 0)
	{
		OBJDATA(hObject)->field_04 = ObjectRegenerateMax(ptTypeData);
#line 178
		D2ASSERT(ObjectRegenerateMax( ptTypeData ) - bLeft >= 0);
		return;
	}
	bMax = ObjectRegenerateMax(ptTypeData);
	bLeft = bLeft + 1;
	if (bLeft <= bMax && bLeft % (BYTE)(bMax / ObjectRegenerateModes()) == 0)
		D2CommonUnitSetMode(hObject,
			ObjectRegenerateModes() - bLeft / ptTypeData->Param[OBJECT_PARAM_WELL_REGEN_PER_MODE]);
	D2CommonUnitRoomAddToChangedList(hObject);
	hObject->dwFlags0 |= 1;
}

void __fastcall ObjectModeEventProximity(GAME *ptGame, UNIT *hObject)
{
	D2SEED *ptSeed;
	GAMEROOM *hRoom;
	MISSOBJECTSTXT *ptTxt;
	COLPOINT tCoord;
	UNIT *hUnit;

	if (hObject->dwMode == 1)
		hObject->dwMode = 2;
	ptSeed = ObjectRgnGetSeed(ptGame, (GAMEROOM *)D2CommonUnitGetPathRoom(hObject));
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hObject);
	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hObject->dwClassId);
	D2CommonUnitGetPosition(hObject, &tCoord);
	for (hUnit = hRoom->ptUnitFirst; hUnit; hUnit = hUnit->ptRoomNext)
	{
		if (hUnit->eType == UNIT_PLAYER && hUnit->dwMode != 17
			&& D2CommonUnitGetProximityMetric(hObject, hUnit) <= (DWORD)(ptTxt->Param[0] + 1))
			ObjectEvalDamageEnemy(ptGame, hObject, hUnit, TRUE);
	}
	EventAdd(ptGame, hObject, 0, D2CommonSeedRand(ptSeed, 35) + ptGame->nFrame + 15);
}

void __fastcall ObjectModeEventTrap(GAME *ptGame, UNIT *hObject)
{
	int nLevelId;
	BYTE bAttributes;

	nLevelId = (int)D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hObject));
	bAttributes = OBJDATA(hObject)->field_04 & 127;
	if ((int)bAttributes >= sgnObjectTrapHandlers)
		return;
	if (bAttributes == 8)
	{
		if (nLevelId >= 75)
		{
#line 304
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgObjectTrapHandlers[OBJECT_TRAPPEDFIREBOLT]), "sgObjectTrapHandlers[OBJECT_TRAPPEDFIREBOLT]");
			sgObjectTrapHandlers[OBJECT_TRAPPEDFIREBOLT](ptGame, hObject);
			return;
		}
	}
	else if (bAttributes == 3)
	{
		if (nLevelId < 40 && nLevelId != 25)
		{
#line 315
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgObjectTrapHandlers[OBJECT_TRAPPEDFIREBOLT]), "sgObjectTrapHandlers[OBJECT_TRAPPEDFIREBOLT]");
			sgObjectTrapHandlers[OBJECT_TRAPPEDFIREBOLT](ptGame, hObject);
			return;
		}
	}
	else if (bAttributes == 1 || bAttributes == 4)
	{
		if (nLevelId < 40)
		{
#line 325
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgObjectTrapHandlers[OBJECT_TRAPPEDFIREBOLT]), "sgObjectTrapHandlers[OBJECT_TRAPPEDFIREBOLT]");
			sgObjectTrapHandlers[OBJECT_TRAPPEDFIREBOLT](ptGame, hObject);
			return;
		}
	}
#line 330
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgObjectTrapHandlers[bAttributes]), "sgObjectTrapHandlers[bAttributes]");
	sgObjectTrapHandlers[bAttributes](ptGame, hObject);
}

void __fastcall ObjectModeEventCycle(GAME *ptGame, UNIT *hObject)
{
	DWORD dwMode;
	GAMEROOM *hRoom;
	BOOL fArmed;
	COLPOINT tCoord;
	UNIT *hUnit;
	int nTouched;
	BOOL fWarned;

	dwMode = hObject->dwMode;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hObject);
	fArmed = FALSE;
	switch (OBJDATA(hObject)->field_04)
	{
	case 0:
		break;
	case 1:
		fArmed = TRUE;
		break;
	case 2:
		if (dwMode)
		{
			D2CommonUnitSetMode(hObject, 0);
			OBJDATA(hObject)->field_04 = 0;
		}
	default:
		EventAdd(ptGame, hObject, 3, ptGame->nFrame + 15);
		return;
	}
	D2CommonUnitGetPosition(hObject, &tCoord);
	fWarned = FALSE;
	nTouched = 0;
	for (hUnit = hRoom->ptUnitFirst; hUnit; hUnit = hUnit->ptRoomNext)
	{
		if (hUnit->eType == UNIT_PLAYER && hUnit->dwMode != 17
			&& D2CommonUnitGetProximityMetric(hObject, hUnit) <= 0)
		{
			if (fArmed)
			{
				ObjectEvalDamageEnemy(ptGame, hObject, hUnit, FALSE);
				nTouched = 1;
			}
			else
			{
				SUnitSetEvent(hObject, 12, NULL);
				fWarned = TRUE;
			}
		}
	}
	if (fWarned)
	{
		OBJDATA(hObject)->field_04 = 1;
		EventAdd(ptGame, hObject, 3, ptGame->nFrame + 25);
		return;
	}
	if (fArmed)
	{
		if (nTouched == 1)
		{
			OBJDATA(hObject)->field_04 = 2;
			D2CommonUnitSetMode(hObject, 1);
		}
		else
			OBJDATA(hObject)->field_04 = 0;
		EventAdd(ptGame, hObject, 3,
			ptGame->nFrame
				+ ((DWORD)((MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hObject->dwClassId))->FrameCnt[1] >> 8)
				+ 1);
		return;
	}
	EventAdd(ptGame, hObject, 3, ptGame->nFrame + 15);
}

void __fastcall ObjectModeEventQuest(GAME *ptGame, UNIT *hObject)
{
	QuestsObjectOperate(ptGame, hObject);
}

void __fastcall ObjectModeEventDrop(GAME *ptGame, UNIT *hObject)
{
	DWORD dwMode;
	MISSOBJECTSTXT *ptTxt;
	OBJOPERATE tOperate;
	GAMEROOM *hRoom;
	COLPOINT tCoord;
	UNIT *hUnit;
	BYTE bCount;

	dwMode = hObject->dwMode;
	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hObject->dwClassId);
	switch (dwMode)
	{
	case 1:
		EventAdd(ptGame, hObject, 9, ptGame->nFrame + 10);
		break;
	case 2:
		bCount = OBJDATA(hObject)->field_04 + 1;
		OBJDATA(hObject)->field_04 = bCount;
		if (bCount > 1)
		{
			tOperate.ptGame = ptGame;
			tOperate.hObject = hObject;
			tOperate.hPlayer = NULL;
			tOperate.ptSeed = ObjectRgnGetSeed(ptGame, (GAMEROOM *)D2CommonUnitGetPathRoom(hObject));
			tOperate.dwClassId = hObject->dwClassId;
			if (ObjectDropOne(&tOperate, 0))
			{
				D2CommonUnitSetMode(hObject, 3);
				EventAdd(ptGame, hObject, 9, ptGame->nFrame + ((DWORD)ptTxt->FrameCnt[3] >> 8) + 1);
			}
			break;
		}
		ObjectRgnGetSeed(ptGame, (GAMEROOM *)D2CommonUnitGetPathRoom(hObject));
		hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hObject);
		ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hObject->dwClassId);
		D2CommonUnitGetPosition(hObject, &tCoord);
		for (hUnit = hRoom->ptUnitFirst; hUnit; hUnit = hUnit->ptRoomNext)
		{
			if (hUnit->eType == UNIT_PLAYER && hUnit->dwMode != 17
				&& D2CommonUnitGetProximityMetric(hObject, hUnit) <= (DWORD)(ptTxt->Param[0] + 1))
				ObjectEvalDamageEnemy(ptGame, hObject, hUnit, TRUE);
		}
		EventAdd(ptGame, hObject, 9, ptGame->nFrame + 25);
		break;
	case 3:
		D2CommonUnitSetMode(hObject, 4);
		break;
	}
}

void __fastcall ObjectModeEvent(GAME *ptGame, UNIT *hUnit, int eEventType)
{
#line 522
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)scpfnEventFunctions[eEventType]), "scpfnEventFunctions[eEventType]");
	scpfnEventFunctions[eEventType](ptGame, hUnit);
}

void __fastcall ObjectModeSendState(UNIT *hObject, CLIENT *ptClient)
{
	BYTE bSubClass;
	BYTE *pbShrine;
	BYTE bCode;
	DWORD dwUser;

	SCmdSendUnitModeValue(ptClient, S2C_UNIT_MODE_DWORD, UNIT_OBJECT, hObject->dwId, 3,
		(BYTE)((hObject->dwFlags0 >> 1) & 1), (WORD)hObject->dwMode);
	bSubClass = ((MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hObject->dwClassId))->SubClass;
	if (!bSubClass)
		return;
	if (bSubClass & 4)
	{
		SCmdSendObject(ptClient, hObject);
		return;
	}
	if (hObject->dwMode == 1 && (bSubClass & 1))
	{
		pbShrine = (BYTE *)D2CommonUnitGetObjectShrine(hObject);
		bCode = 0;
		if (pbShrine)
			bCode = *pbShrine;
		dwUser = OBJDATA(hObject)->dwShrineUser;
		if (dwUser)
			SCmdSendTargetPoint(ptClient, 2, hObject->dwId, 0, 0, (WORD)(dwUser - 1), 0, bCode);
	}
}

void __fastcall ObjectModeSendToClient(GAME *ptGame, UNIT *hObject, CLIENT *ptClient)
{
	if (hObject->dwFlags0 & 1)
		ObjectModeSendState(hObject, ptClient);
	if (hObject->dwFlags0 & 1024)
		SUnitNpcSendUnitEvent(hObject, ptClient);
	if (hObject->dwFlags0 & 256)
		SUnitMsgSendChat(hObject, ptClient);
	if (hObject->dwFlags & 1)
		INVENTORYModeSendItems(ptGame, hObject, ptClient, NULL);
}

static __inline void ObjectModeWarpToPad(UNIT *hPad, UNIT *hPlayer, GAMEROOM *hDest)
{
	COLPOINT tPoint;
	GAMERECT atRect[2];

	D2CommonUnitGetPosition(hPad, &tPoint);
	if (!D2CommonCollisnFindSpawnPointNear(hDest, &tPoint, 3, 7177, 0))
		return;
	if (!SUnitMoveToRoom(SUnitGetGame(hPlayer), hPlayer, hDest, tPoint.nX, tPoint.nY, 0, 0))
		return;
	D2CommonDungeonRoomCopyRects(hDest, atRect);
	SCmdSendRoom(SUnitGetClient(hPlayer), D2CommonDungeonRoomGetLevelId(hDest), (WORD)atRect[1].nX, (WORD)atRect[1].nY);
	D2CommonUnitRoomAddToChangedList(hPlayer);
	hPlayer->dwFlags |= 65536;
}

int __fastcall ObjectModeOperateTeleportPad(OBJOPERATE *pObjectParam)
{
	GAMEROOM *hRoom;
	UNIT *hUnit;
	DWORD dwClassId;
	GAMEROOM **phNear;
	DWORD dwNear;
	DWORD i;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(pObjectParam->hObject);
	if (!hRoom)
		return 0;
	dwClassId = pObjectParam->hObject->dwClassId;
	for (hUnit = hRoom->ptUnitFirst; hUnit; hUnit = hUnit->ptRoomNext)
	{
		if (hUnit->eType == UNIT_OBJECT && hUnit != pObjectParam->hObject && hUnit->dwClassId == dwClassId)
		{
			ObjectModeWarpToPad(hUnit, pObjectParam->hPlayer, hRoom);
			return 0;
		}
	}
	D2CommonDungeonRoomGetNear(hRoom, &phNear, &dwNear);
	for (i = 0; i < dwNear; i++)
	{
		if (phNear[i] == hRoom)
			continue;
		for (hUnit = phNear[i]->ptUnitFirst; hUnit; hUnit = hUnit->ptRoomNext)
		{
			if (hUnit->eType == UNIT_OBJECT && hUnit != pObjectParam->hObject && hUnit->dwClassId == dwClassId)
			{
				ObjectModeWarpToPad(hUnit, pObjectParam->hPlayer, phNear[i]);
				return 0;
			}
		}
	}
	return 0;
}

int __fastcall ObjectModeOperateCow(OBJOPERATE *pObjectParam)
{
	MISSOBJECTSTXT *ptObjTxt;
	if (!pObjectParam->hObject->dwMode)
	{
		ObjectEvalDamageEnemy(pObjectParam->ptGame, pObjectParam->hObject, pObjectParam->hPlayer, FALSE);
		ObjectEvalDamageEnemy(pObjectParam->ptGame, pObjectParam->hObject, pObjectParam->hPlayer, TRUE);
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		ptObjTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
		EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
			pObjectParam->ptGame->nFrame + ((DWORD)ptObjTxt->FrameCnt[1] >> 8) + 1);
	}
	return 0;
}

int __fastcall ObjectModeOperateDoor(OBJOPERATE *pObjectParam)
{
	GAMEROOM *hRoom;
	COLPOINT tCoord;
	int nSizeX;
	int nSizeY;
	DWORD dwTick;
	DWORD dwMode;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(pObjectParam->hObject);
	D2CommonUnitGetPosition(pObjectParam->hObject, &tCoord);
	nSizeX = (int)D2CommonUnitGetSizeX(pObjectParam->hObject);
	nSizeY = (int)D2CommonUnitGetSizeY(pObjectParam->hObject);
	dwTick = GetTickCount();
	dwMode = pObjectParam->hObject->dwMode;
	if (dwTick < pObjectParam->hObject->field_f4 + 500)
		return 1;
	switch (dwMode)
	{
	case 6:
		if (!ITEMSModeUseKey(pObjectParam->ptGame, pObjectParam->hPlayer))
		{
			SUnitSetEvent(pObjectParam->hPlayer, 21, pObjectParam->hPlayer);
			return 1;
		}
	case 0:
		D2CommonUnitClearCollision(pObjectParam->hObject);
		D2CommonUnitSetMode(pObjectParam->hObject, 2);
		pObjectParam->hObject->field_f4 = dwTick;
		return 1;
	case 2:
	case 5:
		if (!D2CommonCollisnTestArea(hRoom, tCoord.nX, tCoord.nY, nSizeX, nSizeY, 33152))
		{
			D2CommonUnitSetCollision(pObjectParam->hObject, hRoom, tCoord.nX, tCoord.nY);
			D2CommonUnitSetMode(pObjectParam->hObject, 0);
			pObjectParam->hObject->field_f4 = dwTick;
			return 1;
		}
		if (!D2CommonCollisnTestArea(hRoom, tCoord.nX, tCoord.nY, nSizeX, nSizeY, 32768))
		{
			if (dwMode == 5)
				return 1;
			D2CommonUnitSetMode(pObjectParam->hObject, 5);
			pObjectParam->hObject->field_f4 = dwTick;
			return 1;
		}
		D2CommonUnitSetMode(pObjectParam->hObject, 4);
		pObjectParam->hObject->field_f4 = dwTick;
		return 1;
	}
	return 1;
}

int __fastcall ObjectModeOperateTrapDoor(OBJOPERATE *pObjectParam)
{
	COLPOINT tCoord;
	UNIT *hUnit;

	D2CommonUnitGetPathRoom(pObjectParam->hObject);
	D2CommonUnitGetPosition(pObjectParam->hObject, &tCoord);
	D2CommonUnitGetSizeX(pObjectParam->hObject);
	D2CommonUnitGetSizeY(pObjectParam->hObject);
	switch (pObjectParam->hObject->dwMode)
	{
	case 0:
		D2CommonUnitSetMode(pObjectParam->hObject, 2);
		break;
	case 2:
		for (hUnit = ((GAMEROOM *)D2CommonUnitGetPathRoom(pObjectParam->hObject))->ptUnitFirst; hUnit;
			hUnit = hUnit->ptRoomNext)
		{
			if (hUnit->eType == UNIT_ROOMTILE)
				break;
		}
#line 912
		D2ASSERT(hUnit);
		SUnitWarp(pObjectParam->ptGame, pObjectParam->hPlayer, hUnit);
		break;
	}
	return 1;
}

int __fastcall ObjectModeOperateStairs(OBJOPERATE *pObjectParam)
{
	MISSOBJECTSTXT *ptObjTxt;
	GAMEROOM *hRoom;
	COLPOINT tCoord;
	UNIT *hUnit;
	GAMEROOM **phNear;
	DWORD dwNear;
	DWORD i;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(pObjectParam->hObject);
	D2CommonUnitGetPosition(pObjectParam->hObject, &tCoord);
	D2CommonUnitGetSizeX(pObjectParam->hObject);
	D2CommonUnitGetSizeY(pObjectParam->hObject);
	switch (pObjectParam->hObject->dwMode)
	{
	case 0:
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		ptObjTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
		EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
			pObjectParam->ptGame->nFrame + ((DWORD)ptObjTxt->FrameCnt[1] >> 8) + 1);
		break;
	case 2:
		for (hUnit = hRoom->ptUnitFirst; hUnit; hUnit = hUnit->ptRoomNext)
		{
			if (hUnit->eType == UNIT_ROOMTILE)
			{
				SUnitWarp(pObjectParam->ptGame, pObjectParam->hPlayer, hUnit);
				return 1;
			}
		}
		D2CommonDungeonRoomGetNear(hRoom, &phNear, &dwNear);
		for (i = 0; i < dwNear; i++)
		{
			if (phNear[i] == hRoom)
				continue;
			for (hUnit = phNear[i]->ptUnitFirst; hUnit; hUnit = hUnit->ptRoomNext)
			{
				if (hUnit->eType == UNIT_ROOMTILE)
				{
					SUnitWarp(pObjectParam->ptGame, pObjectParam->hPlayer, hUnit);
					return 1;
				}
			}
		}
		break;
	}
	return 1;
}

int __fastcall ObjectModeOperateStairsR(OBJOPERATE *pObjectParam)
{
	WORD wMode = (WORD)pObjectParam->hObject->dwMode;

	if (wMode == 0)
		return 0;
	if (wMode == 2)
		return ObjectModeOperateStairs(pObjectParam);
	return 0;
}

int __fastcall ObjectModeOperateSlimeDoor(OBJOPERATE *pObjectParam)
{
	if (!pObjectParam->hObject->dwMode)
	{
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		D2CommonUnitClearCollision(pObjectParam->hObject);
	}
	return 1;
}

void __fastcall ObjectModeTrapRegionMonster(GAME *ptGame, UNIT *hObject)
{
	OBJOPERATE tOperate;
	int nCount;
	int nClass;
	GAMEROOM *hRoom;

	tOperate.ptSeed = ObjectRgnGetSeed(ptGame, (GAMEROOM *)D2CommonUnitGetPathRoom(hObject));
	tOperate.hObject = hObject;
	tOperate.ptGame = ptGame;
	nCount = (D2CommonSeedNext(tOperate.ptSeed) & 1) + 1;
	nClass = ObjectRgnGetMonClass(&tOperate);
	if (nClass == 410)
		return;
	for (; nCount > 0; nCount--)
	{
		hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(tOperate.hObject);
		if (nClass == 234 && hRoom && !D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom)))
			continue;
		if (!MonsterSpawnFromPopulation(tOperate.ptGame, tOperate.hObject, nClass, 8, -1, 0)
			&& !MonsterSpawnFromPopulation(tOperate.ptGame, tOperate.hObject, nClass, 8, 1, 0))
			MonsterSpawnFromPopulation(tOperate.ptGame, tOperate.hObject, nClass, 8, 3, 0);
	}
}

void __fastcall ObjectModeTrapFire(GAME *ptGame, UNIT *hObject)
{
	COLPOINT tCoord;
	GAMEROOM *hRoom;
	UNIT *hFire;
	GAMERECT atRect[2];
	int nX;

	D2CommonUnitGetPosition(hObject, &tCoord);
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hObject);
	hFire = SUnitCreate(UNIT_OBJECT, 162, tCoord.nX, tCoord.nY, ptGame, hRoom, 1, 1, 0);
	if (hFire)
		SUnitSetObjectFlags(hFire, 2);
	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	nX = tCoord.nX + 1;
	if (nX >= atRect[0].nX && nX < atRect[0].nX + atRect[0].nWidth)
	{
		hFire = SUnitCreate(UNIT_OBJECT, 160, nX, tCoord.nY, ptGame,
			(GAMEROOM *)D2CommonUnitGetPathRoom(hObject), 1, 1, 0);
		SUnitSetObjectFlags(hFire, 2);
	}
}

static __inline void ObjectModeSpawnTrapMonster(GAME *ptGame, UNIT *hObject, int nMonster)
{
	GAMEROOM *hRoom;
	COLPOINT tPoint;

	D2CommonUnitGetPosition(hObject, &tPoint);
	if (!MonsterSpawnAt(ptGame, (GAMEROOM *)D2CommonUnitGetPathRoom(hObject), tPoint.nX, tPoint.nY,
		nMonster, 1, 136))
	{
		D2CommonCollisnFindFreeArea(D2CommonUnitGetPathRoom(hObject), &tPoint, 0, 7953, &hRoom);
		if (hRoom)
			MonsterSpawn(ptGame, hRoom, tPoint.nX, tPoint.nY, nMonster, 1, 3, 8);
	}
}

void __fastcall ObjectModeTrapChainLightning(GAME *ptGame, UNIT *hObject)
{
	ObjectModeSpawnTrapMonster(ptGame, hObject, 330);
}

void __fastcall ObjectModeTrapFireBolt(GAME *ptGame, UNIT *hObject)
{
	ObjectModeSpawnTrapMonster(ptGame, hObject, 326);
}

void __fastcall ObjectModeTrapPoison(GAME *ptGame, UNIT *hObject)
{
	ObjectModeSpawnTrapMonster(ptGame, hObject, 329);
}

void __fastcall ObjectModeTrapNova(GAME *ptGame, UNIT *hObject)
{
	ObjectModeSpawnTrapMonster(ptGame, hObject, 369);
}

void __fastcall ObjectModeOperateTowerChest(OBJOPERATE *pObjectParam)
{
	MISSOBJECTSTXT *ptTxt;
	COLPOINT tCoord;
	DWORD dwCode;
	int nLevel;

	if (pObjectParam->hObject->dwMode)
		return;
	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
	D2CommonUnitGetPosition(pObjectParam->hObject, &tCoord);
	ObjectDropOne(pObjectParam, 4);
	ObjectDropOne(pObjectParam, 4);
	ObjectDropOne(pObjectParam, 4);
	dwCode = ITEMSGetHealingPotion(pObjectParam->hObject);
#line 1253
	D2ASSERT(dwCode);
	UNIT7(pObjectParam->hObject)->dwDropItemCode = dwCode;
	ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 2, &nLevel, NULL, 44, TRUE);
	ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 2, &nLevel, NULL, 44, TRUE);
	dwCode = ITEMSGetManaPotion(pObjectParam->hObject);
#line 1258
	D2ASSERT(dwCode);
	UNIT7(pObjectParam->hObject)->dwDropItemCode = dwCode;
	ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 2, &nLevel, NULL, 44, TRUE);
	ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 2, &nLevel, NULL, 44, TRUE);
	if (ptTxt->Mode[1])
	{
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
			pObjectParam->ptGame->nFrame + ((DWORD)ptTxt->FrameCnt[1] >> 8) + 1);
	}
	else
		D2CommonUnitSetMode(pObjectParam->hObject, 2);
	pObjectParam->hObject->dwFlags0 &= ~2;
}

int __fastcall ObjectModeOperateChest(OBJOPERATE *pObjectParam)
{
	MISSOBJECTSTXT *ptTxt;
	int nExtra;
	BYTE bAttributes;
	BYTE bTrapped;
	DWORD dwTries;
	COLPOINT tCoord;
	BOOL fSpecial;
	int nMode;
	int nRoll;
	int nKept;
	int nMade;
	int i;
	int nGold;
	UNIT *hItem;
	DWORD dwKept;
	DWORD j;
	D2SEED *ptSeed;
	int nMax;

	if (pObjectParam->hObject->dwMode)
		return 1;
	D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(pObjectParam->hObject));
	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
	nExtra = 0;
	bAttributes = OBJDATA(pObjectParam->hObject)->field_04;
	bTrapped = bAttributes & 128;
	if (bTrapped)
	{
		if (!ITEMSModeUseKey(pObjectParam->ptGame, pObjectParam->hPlayer))
		{
			SUnitSetEvent(pObjectParam->hPlayer, 21, pObjectParam->hPlayer);
			return 1;
		}
		SUnitSetEvent(pObjectParam->hPlayer, 11, pObjectParam->hPlayer);
		SUnitNpcSendUnitEvent(pObjectParam->hPlayer, SUnitGetClient(pObjectParam->hPlayer));
		nExtra = 1;
	}
	ptSeed = pObjectParam->ptSeed; nMax = ptTxt->SpawnMax;
	dwTries = (nMax ? D2CommonSeedRand(ptSeed, nMax) : 0)
		+ nExtra
		+ 1;
	D2CommonUnitGetPosition(pObjectParam->hObject, &tCoord);
	fSpecial = FALSE;
	if (SUnitGetObjectFlags(pObjectParam->hObject) & 1)
	{
		fSpecial = TRUE;
		if (D2CommonSeedRand(pObjectParam->ptSeed, 100) < 5)
			nMode = 6;
		else
			nMode = 4;
	}
	else
		nMode = 0;
	if (pObjectParam->hObject->dwClassId == 397)
	{
		nKept = 0;
		nMade = 0;
		nRoll = (int)D2CommonSeedRand(pObjectParam->ptSeed, 10000);
		if (nRoll < 200)
		{
			hItem = ObjectDropOne(pObjectParam, 7);
			if (hItem)
			{
				if (D2CommonITEMSTestModTypeMagicOrAbove(hItem))
					goto done;
				hItem = ObjectDropOne(pObjectParam, 7);
				if (hItem && D2CommonITEMSTestModTypeMagicOrAbove(hItem))
					goto done;
			}
		}
		else if (nRoll < 600)
		{
			hItem = ObjectDropOne(pObjectParam, 5);
			if (hItem)
			{
				if (D2CommonITEMSTestModTypeMagicOrAbove(hItem))
					goto done;
				hItem = ObjectDropOne(pObjectParam, 5);
				if (hItem && D2CommonITEMSTestModTypeMagicOrAbove(hItem))
					goto done;
			}
		}
		else if (nRoll < 1200)
		{
			hItem = ObjectDropOne(pObjectParam, 6);
			if (hItem)
			{
				if (D2CommonITEMSTestModTypeMagicOrAbove(hItem))
					goto done;
				hItem = ObjectDropOne(pObjectParam, 6);
				if (hItem && D2CommonITEMSTestModTypeMagicOrAbove(hItem))
					goto done;
			}
		}
		else if (nRoll < 3200)
		{
			for (i = 0; ; )
			{
				hItem = ObjectDropOne(pObjectParam, 4);
				if (hItem)
				{
					if (D2CommonITEMSTestModTypeMagicOrAbove(hItem))
						nKept++;
					nMade++;
				}
				if (nKept >= 3 || ++i >= 10)
					break;
			}
			if (!nMade) ; else
				goto done;
		}
		else if (nRoll < 6200)
		{
			for (i = 0; ; )
			{
				hItem = ObjectDropOne(pObjectParam, 4);
				if (hItem)
				{
					if (D2CommonITEMSTestModTypeMagicOrAbove(hItem))
						nKept++;
					else
						nMade++;
				}
				if (nKept >= 2 || ++i >= 10)
					break;
			}
			if (!nMade)
			{
				if (ObjectDropOne(pObjectParam, 0))
					nMade = 1;
			}
			else if (nMade >= 7)
				goto done;
			for (nGold = 7 - nMade; nGold > 0; nGold--)
				SkillsDropItem(pObjectParam->ptGame, pObjectParam->hObject, 543452263, 0);
			goto done;
		}
		for (i = 0; i < 10; i++)
		{
			hItem = ObjectDropOne(pObjectParam, 4);
			if (hItem)
			{
				if (D2CommonITEMSTestModTypeMagicOrAbove(hItem))
					break;
				nMade++;
			}
		}
		if (nMade < 4)
		{
			for (i = 4 - nMade; i > 0; i--)
				ObjectDropOne(pObjectParam, 0);
		}
		for (i = 5; i; i--)
			SkillsDropItem(pObjectParam->ptGame, pObjectParam->hObject, 543452263, 0);
		for (i = 2; i; i--)
			SkillsDropItem(pObjectParam->ptGame, pObjectParam->hObject, 540242024, 0);
		for (i = 2; i; i--)
			SkillsDropItem(pObjectParam->ptGame, pObjectParam->hObject, 540242029, 0);
	}
	else
	{
		if (D2CommonSeedRand(pObjectParam->ptSeed, 100) < 25 && !fSpecial && !bTrapped)
			goto done;
		dwKept = 0;
		for (j = 0; j < dwTries; j++)
		{
			hItem = ObjectDropOne(pObjectParam, nMode);
			if (hItem && D2CommonITEMSTestModTypeMagicOrAbove(hItem))
				dwKept++;
		}
		if (fSpecial && !dwKept)
		{
			for (j = 0; j < 10; j++)
			{
				hItem = ObjectDropOne(pObjectParam, nMode);
				if (hItem && D2CommonITEMSTestModTypeMagicOrAbove(hItem))
					break;
			}
		}
	}
done:
	if (ptTxt->Mode[1])
	{
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
			pObjectParam->ptGame->nFrame + ((DWORD)ptTxt->FrameCnt[1] >> 8) + 1);
	}
	else
		D2CommonUnitSetMode(pObjectParam->hObject, 2);
	pObjectParam->hObject->dwFlags0 &= ~2;
	if (UNIT7(pObjectParam->hObject)->dwDropItemCode)
		{
			int nLevel;
			ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 2, &nLevel, NULL, 44, TRUE);
		}
	ObjectModeTrapSetOff(pObjectParam, bAttributes & 127);
	return 1;
}

void __fastcall ObjectModeTrapSetOff(OBJOPERATE *pObjectParam, BYTE bTrapType)
{
	int nClass;
	GAMEROOM *hRoom;

	if ((int)bTrapType >= sgnObjectTrapHandlers || !sgObjectTrapHandlers[bTrapType])
		return;
#line 1213
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgObjectTrapHandlers[bTrapType]), "sgObjectTrapHandlers[bTrapType]");
	if (sgObjectTrapHandlers[bTrapType] == ObjectModeTrapRegionMonster)
	{
		nClass = ObjectRgnGetMonClass(pObjectParam);
		hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(pObjectParam->hObject);
		if (nClass == 234 && hRoom && !D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom)))
			return;
	}
	EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 4, pObjectParam->ptGame->nFrame + 35);
	SUnitSetEvent(pObjectParam->hObject, 12, NULL);
}

int __fastcall ObjectModeOperateObelisk(OBJOPERATE *pObjectParam)
{
	UNIT *hTrade;
	CLIENT *ptClient;
	void *hInventory;
	void *hNode;
	BYTE abPacket[7];
	BOOL fFound;

	hTrade = SUnitGetInteractUnit(pObjectParam->ptGame, pObjectParam->hPlayer);
	ptClient = SUnitGetClient(pObjectParam->hPlayer);
	switch (pObjectParam->hObject->dwMode)
	{
	case 0:
		if (PlayerIsBusy(pObjectParam->hPlayer) == 1)
			break;
		hInventory = (pObjectParam->hPlayer)->ptInventory;
#line 1559
		D2ASSERT(hInventory);
		fFound = FALSE;
		for (hNode = D2CommonINVENTORYGetCategoryListHead(hInventory); hNode;
			hNode = (void *)D2CommonINVENTORYNodeGetNext(hNode))
		{
			if (D2CommonITEMSGetType(D2CommonINVENTORYNodeGetKeyA(hNode)) == 20)
			{
				fFound = TRUE;
				break;
			}
		}
		if (!fFound)
		{
			SUnitSetEvent(pObjectParam->hPlayer, 18, pObjectParam->hPlayer);
			return 1;
		}
		SUnitSetInteract(pObjectParam->hPlayer, 2, pObjectParam->hObject->dwId);
		D2CommonUnitSetMode(pObjectParam->hObject, 3);
		abPacket[0] = 81;
		*(DWORD *)&abPacket[1] = pObjectParam->hObject->dwId;
		abPacket[5] = 0;
		SCmdSendUseItemState(ptClient, (const struct SCMDCOPY7 *)abPacket);
		break;
	case 3:
		if (hTrade == pObjectParam->hObject)
		{
			SUnitResetInteract(pObjectParam->hPlayer);
			D2CommonUnitSetMode(pObjectParam->hObject, 1);
			return 1;
		}
		break;
	}
	return 1;
}

void __fastcall ObjectModeShrineRefillLife(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData)
{
	int nLife;
	int nDelta;

	nLife = D2CommonStatsUnitGetValue(pObjectParam->hPlayer, 6);
	nDelta = D2CommonStatsUnitCalcMaxHp(pObjectParam->hPlayer) - nLife;
	if (nDelta)
		D2CommonStatsUnitAddValue(pObjectParam->hPlayer, 6, nDelta);
}

void __fastcall ObjectModeShrineRefillMana(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData)
{
	int nMana;
	int nDelta;

	nMana = D2CommonStatsUnitGetValue(pObjectParam->hPlayer, 8);
	nDelta = D2CommonStatsUnitCalcMaxMana(pObjectParam->hPlayer) - nMana;
	if (nDelta)
		D2CommonStatsUnitAddValue(pObjectParam->hPlayer, 8, nDelta);
}

void __fastcall ObjectModeShrineRefill(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData)
{
	int nValue;
	int nDelta;

	nValue = D2CommonStatsUnitGetValue(pObjectParam->hPlayer, 6);
	nDelta = D2CommonStatsUnitCalcMaxHp(pObjectParam->hPlayer) - nValue;
	if (nDelta)
		D2CommonStatsUnitAddValue(pObjectParam->hPlayer, 6, nDelta);
	nValue = D2CommonStatsUnitGetValue(pObjectParam->hPlayer, 8);
	nDelta = D2CommonStatsUnitCalcMaxMana(pObjectParam->hPlayer) - nValue;
	if (nDelta)
		D2CommonStatsUnitAddValue(pObjectParam->hPlayer, 8, nDelta);
}

void __fastcall ObjectModeShrineLifeToMana(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData)
{
	DWORD dwTaken;

	dwTaken = D2CommonStatsUnitGetValue(pObjectParam->hPlayer, 6);
	dwTaken = (DWORD)(ptShrineData->Arg0 * dwTaken) / 100;
	D2CommonStatsUnitAddValue(pObjectParam->hPlayer, 6, -(int)dwTaken);
	D2CommonStatsUnitAddValue(pObjectParam->hPlayer, 8, (DWORD)(ptShrineData->Arg1 * dwTaken) / 100);
}

void __fastcall ObjectModeShrineManaToLife(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData)
{
	DWORD dwTaken;

	dwTaken = D2CommonStatsUnitGetValue(pObjectParam->hPlayer, 8);
	dwTaken = (DWORD)(ptShrineData->Arg0 * dwTaken) / 100;
	D2CommonStatsUnitAddValue(pObjectParam->hPlayer, 8, -(int)dwTaken);
	D2CommonStatsUnitAddValue(pObjectParam->hPlayer, 6, (DWORD)(ptShrineData->Arg1 * dwTaken) / 100);
}

void __fastcall ObjectModeShrineReverse(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData)
{
	char *pszName;

	pszName = (char *)D2CommonUnitGetPlrInfo(pObjectParam->hPlayer);
	if (pszName)
		strncpy(pszName, strrev(pszName), 16);
}

void __fastcall ObjectModeShrinePortal(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData)
{
	GAMEROOM *hRoom;
	COLPOINT tPoint;
	BYTE bAct;
	DWORD dwTownLevelId;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(pObjectParam->hPlayer);
	D2CommonUnitGetPosition(pObjectParam->hPlayer, &tPoint);
	bAct = D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom));
	dwTownLevelId = ((DWORD (__stdcall *)(BYTE))D2CommonDungeonActGetTownLevelIdRaw)(bAct);
	if (D2CommonCollisnFindSpawnPointNear(D2CommonUnitGetPathRoom(pObjectParam->hPlayer), &tPoint, 3, 7177,
		0))
		SkillsOpenPortal(pObjectParam->ptGame, pObjectParam->hPlayer, hRoom, tPoint.nX + 5, tPoint.nY + 5,
			dwTownLevelId, 0, 59, 0);
}

void __fastcall ObjectModeShrineGem(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData)
{
	void *hInventory;
	UNIT *hItem;
	BOOL fDone;
	DWORD dwBetterGem;
	int nBetterGemIndex;
	DWORD dwCode;
	int nNewGemIndex;

	hInventory = (pObjectParam->hPlayer)->ptInventory;
	hItem = (UNIT *)D2CommonINVENTORYFindGridItemByType(hInventory, 20, 0);
	fDone = FALSE;
	while (hItem)
	{
		if (fDone)
			return;
		dwBetterGem = ((ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId))->BetterGem;
		if (dwBetterGem != 544108398)
		{
			nBetterGemIndex = D2CommonItemTblsFindItemIndexByCode(dwBetterGem);
#line 1887
			D2ASSERT(nBetterGemIndex != -1);
			if (ObjectModeCreateGem(pObjectParam->ptGame, pObjectParam->hPlayer, nBetterGemIndex, hItem))
			{
				fDone = TRUE;
				hItem = NULL;
			}
		}
		hItem = (UNIT *)D2CommonINVENTORYFindGridItemByType(hInventory, 20, hItem);
	}
	if (fDone)
		return;
	switch (D2CommonSeedRand(&(pObjectParam->hPlayer)->tSeed, 6))
	{
	case 0:
		dwCode = 544695143;
		break;
	case 1:
		dwCode = 544367463;
		break;
	case 2:
		dwCode = 543646567;
		break;
	case 3:
		dwCode = 543318887;
		break;
	case 4:
		dwCode = 544826215;
		break;
	default:
		dwCode = 544629607;
		break;
	}
	nNewGemIndex = D2CommonItemTblsFindItemIndexByCode(dwCode);
#line 1928
	D2ASSERT(nNewGemIndex != -1);
	ObjectModeCreateGem(pObjectParam->ptGame, pObjectParam->hPlayer, nNewGemIndex, NULL);
}

BOOL __fastcall ObjectModeCreateGem(GAME *ptGame, UNIT *hPlayer, int nItemIndex, UNIT *hOldItem)
{
	DWORD dwFlags;
	COLPOINT tSpawn;
	COLPOINT tCoord;
	GAMEROOM *hRoom;
	ITEMCREATE tCreate;
	UNIT *hItem;
	int i;
	BOOL bResult;

	bResult = FALSE;
	D2CommonUnitGetPosition(hPlayer, &tCoord);
	hRoom = ITEMSFindDropSpot((GAMEROOM *)D2CommonUnitGetPathRoom(hPlayer), &tCoord, &tSpawn, 1);
	if (hRoom)
	{
		for (i = 0; i < (int)(sizeof(ITEMCREATE) / sizeof(DWORD)); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.dwClassId = nItemIndex;
		tCreate.nX = tSpawn.nX;
		tCreate.hSpawner = NULL;
		tCreate.ptSeed = NULL;
		tCreate.ptGame = ptGame;
		tCreate.dwMode = 3;
		tCreate.nY = tSpawn.nY;
		tCreate.wCreateFlags = 1;
		tCreate.dwQuality = 2;
		tCreate.hRoom = hRoom;
		tCreate.nLevel = ITEMSGetUnitLevel(hPlayer, LEVEL_ID_NONE);
		hItem = ITEMSCreateFromRecord(ptGame, &tCreate, FALSE);
		bResult = TRUE;
		D2CommonStatsUnitSetValue(hItem, 70, 1);
		D2CommonUnitRoomAddToChangedList(hItem);
		hItem->dwFlags0 |= 1;
		if (hOldItem)
		{
			dwFlags = 0;
			D2CommonITEMSSetOrClearFlags(&dwFlags, 32, TRUE);
			D2CommonITEMSDataSetPreviousPage(hOldItem, D2CommonITEMSDataGetPageRaw((DWORD)hOldItem));
			SCmdSendItemFromGrid(SUnitGetClient(hPlayer), hPlayer, hOldItem, dwFlags);
			ITEMSModeRemoveGridItem(ptGame, hPlayer, hOldItem, FALSE);
		}
	}
	return bResult;
}

void __fastcall ObjectModeShrineExploding(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData)
{
	GAMEROOM *hRoom;
	int nX;
	int nY;
	UNITFINDARGS tArgs;
	UNITFINDDATA tFind;
	UNIT *hUnit;
	int nLife;
	int i;
	int nOuter;
	int nInner;
	MISSILECREATE tCreate;
	int nLevel;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(pObjectParam->hObject);
	nX = (int)D2CommonUnitGetX(pObjectParam->hObject);
	nY = (int)D2CommonUnitGetY(pObjectParam->hObject);
	for (i = 0; i < (int)(sizeof(UNITFINDARGS) / sizeof(DWORD)); i++)
		((DWORD *)&tArgs)[i] = 0;
	tArgs.nY = nY;
	tArgs.nDistance = ptShrineData->Arg1;
	tArgs.dwFlags = 1411;
	tArgs.nX = nX;
	D2CommonUnitFindsInit(pObjectParam->ptGame->pMemPool, &tFind, hRoom, nX, nY,
		ptShrineData->Arg1, ObjectModeShrineExplodingTest, &tArgs);
	D2CommonUnitFindsFindUnits(&tFind);
	for (i = 0; i < tFind.nNumUnits; i++)
	{
		hUnit = tFind.phUnitsArray[i];
		if (hUnit && (hUnit->eType == UNIT_MONSTER || hUnit->eType == UNIT_PLAYER) && !SUnitIsDead(hUnit))
		{
			nLife = D2CommonStatsUnitGetValue(hUnit, 6);
			D2CommonStatsUnitAddValue(hUnit, 6, ptShrineData->Arg0 * nLife / -100);
		}
	}
	D2CommonUnitFindsFreeArray(&tFind);
	for (nOuter = 1; nOuter < 5; nOuter++)
	{
		for (nInner = 1; nInner < 5; nInner++)
		{
			for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
				((DWORD *)&tCreate)[i] = 0;
			tCreate.dwFlags = 3;
			tCreate.nX = (int)D2CommonUnitGetX(pObjectParam->hObject);
			tCreate.nY = (int)D2CommonUnitGetY(pObjectParam->hObject);
			if (nOuter & 1)
				tCreate.nDX = nOuter * 5;
			else
				tCreate.nDX = nOuter * -5;
			if (nInner & 1)
				tCreate.nDY = nInner * 5;
			else
				tCreate.nDY = nInner * -5;
			tCreate.hSource = pObjectParam->hPlayer;
			tCreate.eMissile = 62;
			nLevel = D2CommonStatsUnitGetValue(pObjectParam->hPlayer, 12) / 5;
			if (nLevel < 1)
				nLevel = 1;
			else if (nLevel > 8)
				nLevel = 8;
			tCreate.nLevel = nLevel;
			MissilesCreate(pObjectParam->ptGame, &tCreate);
		}
	}
}

BOOL __fastcall ObjectModeShrineExplodingTest(UNIT *hUnit)
{
	BOOL bResult = TRUE;
	if (hUnit->eType == UNIT_MONSTER)
	{
		if (D2CommonMonsterUnitTestFieldC(hUnit))
			bResult = FALSE;
	}
	else if (hUnit->eType == UNIT_PLAYER)
	{
		if (!hUnit->dwMode || hUnit->dwMode == 17)
			bResult = FALSE;
	}
	else
		bResult = FALSE;
	return bResult;
}

void __fastcall ObjectModeShrineMonster(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData)
{
	int nX;
	int nY;
	UNIT *hMonster;

	hMonster = (UNIT *)D2CommonUnitFindsFindNearestUnit(pObjectParam->hPlayer, ((int)D2CommonUnitGetX(pObjectParam->hPlayer)), ((int)D2CommonUnitGetY(pObjectParam->hPlayer)), 0,
		ObjectModeShrineMonsterTest);
	if (hMonster)
		MonsterUniqueMakeUnique(pObjectParam->ptGame, hMonster);
}

BOOL __fastcall ObjectModeShrineMonsterTest(UNIT *hUnit, UNIT *hPlayer)
{
	MONSTATSTXT *ptStats;
	struct MONSTERUNIQUE *ptUnique;

	if (hUnit == hPlayer)
		return FALSE;
	if (hUnit->eType != UNIT_MONSTER)
		return FALSE;
	if (D2CommonStatsUnitsTestSameAlignment(hPlayer, hUnit) == 1)
		return FALSE;
	if (D2CommonMonsterUnitTestFieldC(hUnit))
		return FALSE;
	if (hUnit->dwMode != 1 && hUnit->dwMode != 2)
		return FALSE;
	ptStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId);
	if (!ptStats->bModeW)
		return FALSE;
	if (D2CommonMonsterRegionUnitIsBossRaw((DWORD)ptStats, (DWORD)hUnit))
		return FALSE;
	if (D2CommonMonsterUnitTestBoss(hUnit))
		return FALSE;
	ptUnique = ((MONSTERDATA *)hUnit->uTypeData)->ptUnique;
	if (ptUnique)
	{
		if (MonsterUniqueIsCounted(ptUnique))
			return FALSE;
		if (MonsterUniqueIsMinion(ptUnique))
			return FALSE;
		if (MonsterUniqueIsChampion(ptUnique))
			return FALSE;
		if (MonsterUniqueIsSuperUnique(ptUnique))
			return FALSE;
		if (MonsterUniqueIsUnique(ptUnique))
			return FALSE;
	}
	return TRUE;
}

void __fastcall ObjectModeShrineItemsOpm(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData)
{
	D2SEED *ptSeed;
	int nRange;
	int nCount;
	int i;
	int nItemIndex;
	COLPOINT tSpawn;
	int anDY[6];
	int anDX[6];
	COLPOINT tCoord;
	GAMEROOM *hRoom;
	UNIT *hItem;
	int nLevel;
	int j;

	ptSeed = &pObjectParam->hObject->tSeed;
	nRange = ptShrineData->Arg1 - ptShrineData->Arg0;
	nCount = ptShrineData->Arg0 + (nRange ? (int)D2CommonSeedRand(ptSeed, nRange) : 0);

	for (i = 0; i < nCount; i++)
	{
		nItemIndex = D2CommonItemTblsFindItemIndexByCode(544043119);
#line 2029
		D2ASSERT(nItemIndex != -1);
		D2CommonUnitGetPosition(pObjectParam->hPlayer, &tCoord);
		hRoom = ITEMSFindDropSpot((GAMEROOM *)D2CommonUnitGetPathRoom(pObjectParam->hPlayer), &tCoord,
			&tSpawn, 1);
		if (hRoom)
		{
			ITEMCREATE tCreate;
			for (j = 0; j < (int)(sizeof(ITEMCREATE) / sizeof(DWORD)); j++)
				((DWORD *)&tCreate)[j] = 0;
			tCreate.ptGame = pObjectParam->ptGame;
			tCreate.dwClassId = nItemIndex;
			tCreate.hSpawner = NULL;
			tCreate.ptSeed = NULL;
			tCreate.dwMode = 3;
			tCreate.nX = tSpawn.nX;
			tCreate.nY = tSpawn.nY;
			tCreate.hRoom = hRoom;
			tCreate.wCreateFlags = 1;
			tCreate.dwQuality = 0;
			tCreate.nLevel = ITEMSGetUnitLevel(pObjectParam->hPlayer, LEVEL_ID_NONE);
			hItem = ITEMSCreateFromRecord(pObjectParam->ptGame, &tCreate, FALSE);
			D2CommonStatsUnitSetValue(hItem, 70, 1);
			D2CommonUnitRoomAddToChangedList(hItem);
			hItem->dwFlags0 |= 1;
		}
	}
	anDX[2] = 0;
	anDX[3] = 0;
	anDX[0] = -6;
	anDX[1] = -6;
	anDX[4] = 6;
	anDX[5] = 6;
	anDY[0] = 6;
	anDY[1] = -6;
	anDY[2] = 6;
	anDY[3] = -6;
	anDY[4] = 6;
	anDY[5] = -6;
	for (i = 0; i < 6; i++)
	{
		MISSILECREATE tMissile;
		for (j = 0; j < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); j++)
			((DWORD *)&tMissile)[j] = 0;
		tMissile.dwFlags = 1312;
		tMissile.nX = (int)D2CommonUnitGetX(pObjectParam->hObject);
		tMissile.nY = (int)D2CommonUnitGetY(pObjectParam->hObject);
		tMissile.field_24 = 1;
		tMissile.nDX = anDX[i] + tMissile.nX;
		tMissile.nDY = anDY[i] + tMissile.nY;
		tMissile.hSource = pObjectParam->hPlayer;
		tMissile.hSpawnLocation = pObjectParam->hObject;
		tMissile.eMissile = 45;
		nLevel = D2CommonStatsUnitGetValue(pObjectParam->hPlayer, 12) / 5;
		if (nLevel < 1)
			nLevel = 1;
		else if (nLevel > 8)
			nLevel = 8;
		tMissile.nLevel = nLevel;
		MissilesCreate(pObjectParam->ptGame, &tMissile);
	}
}

void __fastcall ObjectModeShrineItemsGpm(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData)
{
	D2SEED *ptSeed;
	int nRange;
	int nCount;
	int i;
	int nItemIndex;
	COLPOINT tSpawn;
	int anDY[6];
	int anDX[6];
	COLPOINT tCoord;
	GAMEROOM *hRoom;
	UNIT *hItem;
	int nLevel;
	int j;

	ptSeed = &pObjectParam->hObject->tSeed;
	nRange = ptShrineData->Arg1 - ptShrineData->Arg0;
	nCount = ptShrineData->Arg0 + (nRange ? (int)D2CommonSeedRand(ptSeed, nRange) : 0);

	for (i = 0; i < nCount; i++)
	{
		nItemIndex = D2CommonItemTblsFindItemIndexByCode(544043111);
#line 2093
		D2ASSERT(nItemIndex != -1);
		D2CommonUnitGetPosition(pObjectParam->hPlayer, &tCoord);
		hRoom = ITEMSFindDropSpot((GAMEROOM *)D2CommonUnitGetPathRoom(pObjectParam->hPlayer), &tCoord,
			&tSpawn, 1);
		if (hRoom)
		{
			ITEMCREATE tCreate;
			for (j = 0; j < (int)(sizeof(ITEMCREATE) / sizeof(DWORD)); j++)
				((DWORD *)&tCreate)[j] = 0;
			tCreate.ptGame = pObjectParam->ptGame;
			tCreate.dwClassId = nItemIndex;
			tCreate.hSpawner = NULL;
			tCreate.ptSeed = NULL;
			tCreate.dwMode = 3;
			tCreate.nX = tSpawn.nX;
			tCreate.nY = tSpawn.nY;
			tCreate.hRoom = hRoom;
			tCreate.wCreateFlags = 1;
			tCreate.dwQuality = 0;
			tCreate.nLevel = ITEMSGetUnitLevel(pObjectParam->hPlayer, LEVEL_ID_NONE);
			hItem = ITEMSCreateFromRecord(pObjectParam->ptGame, &tCreate, FALSE);
			D2CommonStatsUnitSetValue(hItem, 70, 1);
			D2CommonUnitRoomAddToChangedList(hItem);
			hItem->dwFlags0 |= 1;
		}
	}
	anDX[2] = 0;
	anDX[3] = 0;
	anDX[0] = -6;
	anDX[1] = -6;
	anDX[4] = 6;
	anDX[5] = 6;
	anDY[0] = 6;
	anDY[1] = -6;
	anDY[2] = 6;
	anDY[3] = -6;
	anDY[4] = 6;
	anDY[5] = -6;
	for (i = 0; i < 6; i++)
	{
		MISSILECREATE tMissile;
		for (j = 0; j < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); j++)
			((DWORD *)&tMissile)[j] = 0;
		tMissile.dwFlags = 1312;
		tMissile.nX = (int)D2CommonUnitGetX(pObjectParam->hObject);
		tMissile.nY = (int)D2CommonUnitGetY(pObjectParam->hObject);
		tMissile.field_24 = 1;
		tMissile.nDX = anDX[i] + tMissile.nX;
		tMissile.nDY = anDY[i] + tMissile.nY;
		tMissile.hSource = pObjectParam->hPlayer;
		tMissile.hSpawnLocation = pObjectParam->hObject;
		tMissile.eMissile = 48;
		nLevel = D2CommonStatsUnitGetValue(pObjectParam->hPlayer, 12) / 5;
		if (nLevel < 1)
			nLevel = 1;
		else if (nLevel > 8)
			nLevel = 8;
		tMissile.nLevel = nLevel;
		MissilesCreate(pObjectParam->ptGame, &tMissile);
	}
}

static inline int ObjectModeShrinePct(UNIT *hPlayer, int nStat, int nArg)
{
	return nArg * D2CommonStatsUnitGetValue(hPlayer, nStat) / 100;
}

void __fastcall ObjectModeShrineCombat(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData)
{
	int idx;
	const SHRINECHART *ptChart;
	DWORD dwBase;
	void *hList;
	int nDamage;
	idx = ptShrineData->bCode;
#line 2229
	D2ASSERT(ptShrineData->bCode < sgnShrineChartSize);
	ptChart = &sgcptShrineChart[idx];
	dwBase = D2CommonUnitGetAttackerIndexedValue(pObjectParam->hPlayer);
	{
		SKILLSTATE tState;
		int i;
		for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
			((DWORD *)&tState)[i] = 0;
		tState.hSource = pObjectParam->hObject;
		tState.hTarget = pObjectParam->hPlayer;
		tState.nSkill = 0;
		tState.nLevel = 0;
		tState.nDuration = ptShrineData->nDurationInFrames;
		tState.dwStatId = 19;
		tState.nStatValue = (DWORD)(ptShrineData->Arg0 * dwBase) / 100;
		tState.dwState = ptChart->nState;
		tState.pfnRemove = 0;
		hList = SkillsSetState(&tState);
	}
	nDamage = ObjectModeShrinePct(pObjectParam->hPlayer, 21, ptShrineData->Arg1);
	if (nDamage < 8)
		nDamage = 8;
	D2CommonStatsListSetValueA(hList, 21, nDamage);
	D2CommonStatsListSetValueA(hList, 22, nDamage);
	D2CommonStatsListSetValueA(hList, 19, ObjectModeShrinePct(pObjectParam->hPlayer, 19, ptShrineData->Arg1));
}

static inline int ObjectModeShrineStatBonus(UNIT *hPlayer, int nStat, int nArg)
{
	switch (nStat)
	{
	case 171:
		return D2CommonUnitCalcArmorClassWithBonus(hPlayer) * nArg / 100;
	case 27:
	case 39:
	case 41:
	case 43:
	case 45:
		return nArg;
	case 11:
		return D2CommonStatsUnitCalcMaxStamina(hPlayer) * nArg / 100;
	}
	return D2CommonStatsUnitGetValue(hPlayer, nStat) * nArg / 100;
}

void __fastcall ObjectModeShrineStatDouble(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData)
{
	int idx;
	const SHRINECHART *ptChart;
	int nBonus;
	idx = ptShrineData->bCode;
#line 2262
	D2ASSERT(ptShrineData->bCode < sgnShrineChartSize);
	ptChart = &sgcptShrineChart[idx];
	D2CommonStatsUnitSetValue(pObjectParam->hPlayer, 10, D2CommonStatsUnitCalcMaxStamina(pObjectParam->hPlayer));
	nBonus = ObjectModeShrineStatBonus(pObjectParam->hPlayer, ptChart->nStat, ptShrineData->Arg0);
	{
		SKILLSTATE tState;
		void *hList;
		int i;
		for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
			((DWORD *)&tState)[i] = 0;
		tState.nSkill = 0;
		tState.nLevel = 0;
		tState.hSource = pObjectParam->hObject;
		tState.hTarget = pObjectParam->hPlayer;
		tState.nDuration = ptShrineData->nDurationInFrames;
		tState.dwStatId = ptChart->nStat;
		tState.nStatValue = nBonus;
		tState.dwState = ptChart->nState;
		tState.pfnRemove = (DWORD)ObjectModeShrineStatDoubleEnd;
		hList = SkillsSetState(&tState);
		D2CommonStatsListSetValueA(hList, 10, nBonus + nBonus);
	}
}

void __fastcall ObjectModeShrineStatDoubleEnd(UNIT *hUnit, DWORD dwState)
{
	SkillsStateRemove(hUnit, dwState);
	D2CommonStatsUnitSetValue(hUnit, 10, D2CommonStatsUnitCalcMaxStamina(hUnit));
}

extern int ObjectModeShrineStatBonusB(int nStat, int nArg, UNIT *hPlayer);
__inline int ObjectModeShrineStatBonusB(int nStat, int nArg, UNIT *hPlayer)
{
	switch (nStat)
	{
	case 27:
	case 39:
	case 41:
	case 43:
	case 45:
		return nArg;
	case 171:
		return D2CommonUnitCalcArmorClassWithBonus(hPlayer) * nArg / 100;
	case 11:
		return D2CommonStatsUnitCalcMaxStamina(hPlayer) * nArg / 100;
	default:
		return D2CommonStatsUnitGetValue(hPlayer, nStat) * nArg / 100;
	}
}

void __fastcall ObjectModeShrineStat(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData)
{
	int idx;
	const SHRINECHART *ptChart;
	DWORD nBonus;
	idx = ptShrineData->bCode;
#line 2279
	D2ASSERT(ptShrineData->bCode < sgnShrineChartSize);
	ptChart = &sgcptShrineChart[idx];
	if (ptChart->nStat != -1)
		nBonus = ObjectModeShrineStatBonusB(ptChart->nStat, ptShrineData->Arg0, pObjectParam->hPlayer);
	else
		nBonus = 0;
	{
		SKILLSTATE tState;
		int i;
		for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
			((DWORD *)&tState)[i] = 0;
		tState.hSource = pObjectParam->hObject;
		tState.hTarget = pObjectParam->hPlayer;
		tState.nSkill = 0;
		tState.nLevel = 0;
		tState.nDuration = ptShrineData->nDurationInFrames;
		tState.dwStatId = ptChart->nStat;
		tState.nStatValue = nBonus;
		tState.dwState = ptChart->nState;
		tState.pfnRemove = 0;
		SkillsSetState(&tState);
	}
}

void __fastcall ObjectModeShrineSkill(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData)
{
	SKILLSTATE tState;
	UNIT *hPlayer;
	int i;

	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tState)[i] = 0;
	tState.hSource = pObjectParam->hObject;
	tState.hTarget = pObjectParam->hPlayer;
	tState.nDuration = ptShrineData->nDurationInFrames;
	tState.dwStatId = 11;
	tState.nSkill = 0;
	tState.nLevel = 0;
	tState.nStatValue = 0;
	tState.dwState = sgcptShrineChart[ptShrineData->bCode].nState;
	tState.pfnRemove = (DWORD)ObjectModeShrineSkillEnd;
	SkillsSetState(&tState);
	hPlayer = pObjectParam->hPlayer;
	if (hPlayer->eType == UNIT_PLAYER)
		PlayerPetsUpdateMax(hPlayer);
	D2CommonSkillsRefreshPassives(hPlayer);
}

void __fastcall ObjectModeShrineSkillEnd(UNIT *hUnit, DWORD dwState)
{
	SkillsStateRemove(hUnit, dwState);
	if (hUnit->eType == UNIT_PLAYER)
		PlayerPetsUpdateMax(hUnit);
	D2CommonSkillsRefreshPassives(hUnit);
}

int __fastcall ObjectModeOperateShrine(OBJOPERATE *pObjectParam)
{
	UNIT *hObject;
	OBJECTDATA *ptData;
	char szText[20];
	SHRINESTXT *ptShrineData;
	int idx;

	hObject = pObjectParam->hObject;
	ptData = OBJDATA(hObject);
	if (ptData->dwShrineUser)
		return 0;
	if (hObject->dwMode)
		return 0;
	ptData->dwShrineUser = (pObjectParam->hPlayer)->dwId + 1;
	D2CommonUnitRoomAddToChangedList(hObject);
	hObject->dwFlags0 |= 1;
	D2CommonUnitSetMode(hObject, 1);
	if (UNIT5(pObjectParam->hObject)->ptChatNode)
		D2CommonChatNodeFree(pObjectParam->ptGame->pMemPool,
			UNIT5(pObjectParam->hObject)->ptChatNode);
	wsprintfA(szText, "%d", (WORD)(OBJDATA(pObjectParam->hObject)->field_04 + 3683));
	UNIT5(pObjectParam->hObject)->ptChatNode = (void *)D2CommonChatNodeNew(
		pObjectParam->ptGame->pMemPool, szText, pObjectParam->ptGame->nFrame);
	D2CommonUnitRoomAddToChangedList(pObjectParam->hObject);
	pObjectParam->hObject->dwFlags0 |= 256;
	EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 6, pObjectParam->ptGame->nFrame + 300);
	ptShrineData = (SHRINESTXT *)D2CommonUnitGetObjectShrine(hObject);
	idx = ptShrineData->bCode;
	if (idx >= 0 && idx < sgnShrineChartSize && sgcptShrineChart[idx].pfnHandler)
	{
#line 2397
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgcptShrineChart[idx].pfnHandler), "sgcptShrineChart[idx].pfnHandler");
		sgcptShrineChart[idx].pfnHandler(pObjectParam, ptShrineData);
	}
	else
		ObjectModeShrineRefill(pObjectParam, ptShrineData);
	if (ptShrineData->bResetTimeInMinutes)
		EventAdd(pObjectParam->ptGame, hObject, 5, ptShrineData->bResetTimeInMinutes * 1200
			+ pObjectParam->ptGame->nFrame + 1);
	return 1;
}

static inline void ObjectModeRegionMonster(OBJOPERATE *pObjectParam)
{
	int nClass;
	GAMEROOM *hRoom;

	nClass = ObjectRgnGetMonClass(pObjectParam);
	if (nClass == 410)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(pObjectParam->hObject);
	if (nClass == 234 && hRoom && !D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom)))
		return;
	if (!MonsterSpawnFromPopulation(pObjectParam->ptGame, pObjectParam->hObject, nClass, 8, -1, 0)
		&& !MonsterSpawnFromPopulation(pObjectParam->ptGame, pObjectParam->hObject, nClass, 8, 1, 0))
		MonsterSpawnFromPopulation(pObjectParam->ptGame, pObjectParam->hObject, nClass, 8, 3, 0);
}

int __fastcall ObjectModeOperateCasket(OBJOPERATE *pObjectParam)
{
	MISSOBJECTSTXT *ptTxt;

	if (pObjectParam->hObject->dwMode)
		return 1;
	if (!ObjectDropOne(pObjectParam, 0))
		return 1;
	D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(pObjectParam->hObject));
	D2CommonUnitSetMode(pObjectParam->hObject, 1);
	pObjectParam->hObject->dwFlags0 &= ~2;
	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
	EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
		pObjectParam->ptGame->nFrame + ((DWORD)ptTxt->FrameCnt[1] >> 8) + 1);
	if ((D2CommonSeedRand(pObjectParam->ptSeed, 10000) & 0xffffe000))
		ObjectModeRegionMonster(pObjectParam);
	if (!ptTxt->HasCollision[1])
		D2CommonUnitClearCollision(pObjectParam->hObject);
	ObjectModeTrapSetOff(pObjectParam, OBJDATA(pObjectParam->hObject)->field_04 & 127);
	return 1;
}

int __fastcall ObjectModeOperateWirt(OBJOPERATE *pObjectParam)
{
	MISSOBJECTSTXT *ptObjTxt;
	int nLevel;

	if (pObjectParam->hObject->dwMode)
		return 1;
	UNIT7(pObjectParam->hObject)->dwDropItemCode = 543647084;
	if (ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 2, &nLevel, NULL, 44, FALSE))
	{
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		ptObjTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
		EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
			pObjectParam->ptGame->nFrame + ((DWORD)ptObjTxt->FrameCnt[1] >> 8) + 1);
		EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 7, pObjectParam->ptGame->nFrame + 10);
	}
	return 1;
}

int __fastcall ObjectModeOperateUrn(OBJOPERATE *pObjectParam)
{
	MISSOBJECTSTXT *ptTxt;

	if (pObjectParam->hObject->dwMode)
		return 1;
	D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(pObjectParam->hObject));
	D2CommonUnitSetMode(pObjectParam->hObject, 1);
	pObjectParam->hObject->dwFlags0 &= ~2;
	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
	EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
		pObjectParam->ptGame->nFrame + ((DWORD)ptTxt->FrameCnt[1] >> 8) + 1);
	if (D2CommonSeedRand(pObjectParam->ptSeed, 100) <= 20)
		ObjectDropOne(pObjectParam, 0);
	if (!ptTxt->HasCollision[1])
		D2CommonUnitClearCollision(pObjectParam->hObject);
	ObjectModeTrapSetOff(pObjectParam, OBJDATA(pObjectParam->hObject)->field_04 & 127);
	return 1;
}

int __fastcall ObjectModeOperateCrate(OBJOPERATE *pObjectParam)
{
	MISSOBJECTSTXT *ptTxt;

	if (pObjectParam->hObject->dwMode)
		return 1;
	if (!ObjectDropOne(pObjectParam, 0))
		return 1;
	D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(pObjectParam->hObject));
	D2CommonUnitSetMode(pObjectParam->hObject, 1);
	pObjectParam->hObject->dwFlags0 &= ~2;
	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
	EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
		pObjectParam->ptGame->nFrame + ((DWORD)ptTxt->FrameCnt[1] >> 8) + 1);
	if (!ptTxt->HasCollision[1])
		D2CommonUnitClearCollision(pObjectParam->hObject);
	ObjectModeTrapSetOff(pObjectParam, OBJDATA(pObjectParam->hObject)->field_04 & 127);
	return 1;
}

void __fastcall ObjectModeEventDelayedDrop(GAME *ptGame, UNIT *hObject)
{
	OBJOPERATE tOperate;

	tOperate.ptGame = ptGame;
	tOperate.hObject = hObject;
	tOperate.ptSeed = ObjectRgnGetSeed(ptGame, (GAMEROOM *)D2CommonUnitGetPathRoom(hObject));
	ObjectDropOne(&tOperate, 0);
}

int __fastcall ObjectModeOperateDelayedDrop(OBJOPERATE *pObjectParam)
{
	MISSOBJECTSTXT *ptTxt;

	if (pObjectParam->hObject->dwMode)
		return 1;
	D2CommonUnitSetMode(pObjectParam->hObject, 1);
	pObjectParam->hObject->dwFlags0 &= ~2;
	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
	EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
		pObjectParam->ptGame->nFrame + ((DWORD)ptTxt->FrameCnt[1] >> 8) + 1);
	EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 10, pObjectParam->ptGame->nFrame + ptTxt->Param[1]
		+ 1);
	if (!ptTxt->HasCollision[1])
		D2CommonUnitClearCollision(pObjectParam->hObject);
	ObjectModeTrapSetOff(pObjectParam, OBJDATA(pObjectParam->hObject)->field_04 & 127);
	return 1;
}

int __fastcall ObjectModeOperateSecretDoor(OBJOPERATE *pObjectParam)
{
	MISSOBJECTSTXT *ptObjTxt;
	if (!pObjectParam->hObject->dwMode)
	{
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		pObjectParam->hObject->dwFlags0 &= ~2;
		ptObjTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
		EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
			pObjectParam->ptGame->nFrame + ((DWORD)ptObjTxt->FrameCnt[1] >> 8) + 1);
		D2CommonUnitClearCollision(pObjectParam->hObject);
	}
	return 1;
}

int __fastcall ObjectModeOperateBookshelf(OBJOPERATE *pObjectParam)
{
	int nLevel;
	COLPOINT tCoord;

	if (!pObjectParam->hObject->dwMode)
	{
		D2CommonUnitSetMode(pObjectParam->hObject, 2);
		pObjectParam->hObject->dwFlags0 &= ~2;
		D2CommonUnitGetPosition(pObjectParam->hObject, &tCoord);
		if ((int)D2CommonSeedRand(pObjectParam->ptSeed, 20) > 12)
		{
			if (D2CommonSeedNext(pObjectParam->ptSeed) & 1)
				UNIT7(pObjectParam->hObject)->dwDropItemCode = 543908457;
			else
				UNIT7(pObjectParam->hObject)->dwDropItemCode = 543908468;
		}
		else
		{
			if (D2CommonSeedNext(pObjectParam->ptSeed) & 1)
				UNIT7(pObjectParam->hObject)->dwDropItemCode = 543388521;
			else
				UNIT7(pObjectParam->hObject)->dwDropItemCode = 543388532;
		}
		ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 2, &nLevel, NULL, 44, FALSE);
	}
	return 1;
}

int __fastcall ObjectModeOperateArmorStand(OBJOPERATE *pObjectParam)
{
	COLPOINT tCoord;

	if (!pObjectParam->hObject->dwMode)
	{
		D2CommonUnitGetPosition(pObjectParam->hObject, &tCoord);
		if (ITEMSDropArmor(pObjectParam->ptGame,
			(GAMEROOM *)D2CommonUnitGetPathRoom(pObjectParam->hObject),
			&tCoord, 44, FALSE, NULL))
		{
			D2CommonUnitSetMode(pObjectParam->hObject, 2);
			pObjectParam->hObject->dwFlags0 &= ~2;
		}
	}
	return 1;
}

int __fastcall ObjectModeOperateWeaponRack(OBJOPERATE *pObjectParam)
{
	COLPOINT tCoord;

	if (!pObjectParam->hObject->dwMode)
	{
		D2CommonUnitGetPosition(pObjectParam->hObject, &tCoord);
		if (ITEMSDropWeapon(pObjectParam->ptGame,
			(GAMEROOM *)D2CommonUnitGetPathRoom(pObjectParam->hObject),
			&tCoord, 44, FALSE, NULL))
		{
			D2CommonUnitSetMode(pObjectParam->hObject, 2);
			pObjectParam->hObject->dwFlags0 &= ~2;
		}
	}
	return 1;
}

int __fastcall ObjectModeOperateBarrel(OBJOPERATE *pObjectParam)
{
	MISSOBJECTSTXT *ptObjTxt;
	if (pObjectParam->hObject->dwMode)
		return 0;
	D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(pObjectParam->hObject));
	D2CommonUnitSetMode(pObjectParam->hObject, 1);
	pObjectParam->hObject->dwFlags0 &= ~2;
	D2CommonUnitClearCollision(pObjectParam->hObject);
	if ((D2CommonSeedRand(pObjectParam->ptSeed, 10000) & 0xffffe000))
		ObjectModeRegionMonster(pObjectParam);
	if (D2CommonSeedRand(pObjectParam->ptSeed, 100) <= 20)
		ObjectDropOne(pObjectParam, 0);
	ptObjTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
	EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
		pObjectParam->ptGame->nFrame + ((DWORD)ptObjTxt->FrameCnt[1] >> 8) + 1);
	return 0;
}

int __fastcall ObjectModeOperateExplodingBarrel(OBJOPERATE *pObjectParam)
{
	MISSOBJECTSTXT *ptObjTxt;
	OBJOPERATE tOperate;
	COLPOINT tCoord;
	UNIT *hUnit;
	DWORD dwUMode;
	DWORD dwUType;

	if (!(pObjectParam->hObject->dwMode))
	{
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		pObjectParam->hObject->dwFlags0 &= ~2;
		tOperate = *pObjectParam;
		D2CommonUnitGetPosition(pObjectParam->hObject, &tCoord);
		for (hUnit = ((GAMEROOM *)D2CommonUnitGetPathRoom(pObjectParam->hObject))->ptUnitFirst; hUnit;
			hUnit = hUnit->ptRoomNext)
		{
			dwUType = hUnit->eType;
			dwUMode = hUnit->dwMode;
			if (hUnit == pObjectParam->hObject || dwUType == UNIT_ROOMTILE)
				continue;
			if (dwUType == UNIT_PLAYER)
			{
				if (dwUMode != 17 && D2CommonUnitUtilIsWithinRange(hUnit, &tCoord, 3))
					ObjectEvalDamageEnemy(pObjectParam->ptGame, pObjectParam->hObject, hUnit, FALSE);
			}
			else if (dwUType == UNIT_MONSTER)
			{
				if (dwUMode != 12 && D2CommonUnitUtilIsWithinRange(hUnit, &tCoord, 3))
					ObjectEvalDamageEnemy(pObjectParam->ptGame, pObjectParam->hObject, hUnit, FALSE);
			}
			else if (dwUType == UNIT_OBJECT && hUnit->dwClassId == 11 && !dwUMode
				&& D2CommonUnitGetProximityMetric(pObjectParam->hObject, hUnit) <= 2)
			{
				tOperate.hObject = hUnit;
				ObjectModeOperateExplodingBarrel(&tOperate);
			} }
		D2CommonUnitClearCollision(pObjectParam->hObject);
		ptObjTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
		EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
			pObjectParam->ptGame->nFrame + ((DWORD)ptObjTxt->FrameCnt[1] >> 8) + 1);
		return 0; }
	return 0;
}

int __fastcall ObjectModeOperateTorch(OBJOPERATE *pObjectParam)
{
	switch (pObjectParam->hObject->dwMode)
	{
	case 0:
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		break;
	case 1:
		D2CommonUnitSetMode(pObjectParam->hObject, 0);
		break;
	}
	return 0;
}

int __fastcall ObjectModeOperateBrazier(OBJOPERATE *pObjectParam)
{
	switch (pObjectParam->hObject->dwMode)
	{
	case 0:
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		pObjectParam->hObject->dwFlags0 &= ~2;
		break;
	case 1:
	case 2:
		D2CommonUnitSetMode(pObjectParam->hObject, 0);
		break;
	}
	return 1;
}

int __fastcall ObjectModeOperate(GAME *ptGame, UNIT *hPlayer, WORD UnitType, DWORD dwUnitId)
{
	UNIT *hObject;
	DWORD dwClassId;
	MISSOBJECTSTXT *ptTypeData;
	OBJOPERATE tObjectParam;

	hObject = SUnitFind(ptGame, UnitType, dwUnitId);
	dwClassId = hObject->dwClassId;
#line 2855
	D2ASSERT(UnitType == UNIT_OBJECT);
	ptTypeData = (MISSOBJECTSTXT *)D2CommonUnitGetObjectClassInfoPtr(hObject);
	if (hPlayer->eType == UNIT_PLAYER && PlayerIsBusy(hPlayer))
		return 0;
	if (ptTypeData->bOperateFunction > NUM_OPERATE_FUNCTIONS
		|| !sgOperateFunctionTable[ptTypeData->bOperateFunction])
	{
#line 2868
		D2ASSERTMSG(ptTypeData->bOperateFunction < NUM_OPERATE_FUNCTIONS, "ptTypeData->bOperateFunction < sizeof( sgOperateFunctionTable ) / sizeof( ptObjectOperate )");
#line 2869
		D2ASSERT(sgOperateFunctionTable[ptTypeData->bOperateFunction] != NULL);
		return 0;
	}
	tObjectParam.ptGame = ptGame;
	tObjectParam.hObject = hObject;
	tObjectParam.hPlayer = hPlayer;
	tObjectParam.ptSeed = ObjectRgnGetSeed(ptGame, (GAMEROOM *)D2CommonUnitGetPathRoom(hObject));
	tObjectParam.dwClassId = dwClassId;
#line 2881
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgOperateFunctionTable[ptTypeData->bOperateFunction]), "sgOperateFunctionTable[ptTypeData->bOperateFunction]");
	return sgOperateFunctionTable[ptTypeData->bOperateFunction](&tObjectParam);
}

void __fastcall ObjectModeOperateInRange(GAME *ptGame, UNIT *hUnit, DWORD dwUnitType, DWORD dwUnitId)
{
	UNIT *hObject;
	DWORD dwClassId;
	int nRange;

	hObject = SUnitFind(ptGame, (WORD)dwUnitType, dwUnitId);
	dwClassId = hObject->dwClassId;
	if (hUnit && hUnit->eType == UNIT_MONSTER
		&& !((MISSOBJECTSTXT *)D2CommonObjTblsGetObject(dwClassId))->MonsterOK)
		return;
	nRange = ((MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hObject->dwClassId))->OperateRange;
	if (hUnit && (int)D2CommonUnitCalcEdgeDistance(hObject, hUnit) > nRange)
		return;
	ObjectModeOperate(ptGame, hUnit, (WORD)dwUnitType, dwUnitId);
}

void __fastcall ObjectModeDoorUpdate(GAME *ptGame, UNIT *hUnit, DWORD dwObjectId)
{
	UNIT *hObject;
	GAMEROOM *hRoom;
	COLPOINT tCoord;
	int nSizeX;
	int nSizeY;
	DWORD dwMode;

	hObject = SUnitFind(ptGame, UNIT_OBJECT, dwObjectId);
#line 2986
	D2ASSERT(hObject != NULL);
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hObject);
	D2CommonUnitGetPosition(hObject, &tCoord);
	nSizeX = (int)D2CommonUnitGetSizeX(hObject);
	nSizeY = (int)D2CommonUnitGetSizeY(hObject);
	dwMode = hObject->dwMode;
	switch (dwMode)
	{
	case 0:
		(void)D2CommonCollisnTestArea(hRoom, tCoord.nX, tCoord.nY, nSizeX, nSizeY, 33152);
		break;
	case 2:
	case 5:
		if (!D2CommonCollisnTestArea(hRoom, tCoord.nX, tCoord.nY, nSizeX, nSizeY, 33152))
		{
			if (dwMode == 2)
				break;
			D2CommonUnitSetMode(hObject, 2);
			break;
		}
		if (dwMode == 5)
			break;
		D2CommonUnitSetMode(hObject, 5);
		break;
	}
}

int __fastcall ObjectModeOperateSanctuaryPortal(OBJOPERATE *pObjectParam)
{
	MISSOBJECTSTXT *ptObjTxt;
	DWORD dwLevelId;

	dwLevelId = D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(pObjectParam->hObject));
	if (!pObjectParam->hObject->dwMode)
	{
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		ptObjTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
		EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
			pObjectParam->ptGame->nFrame + ((DWORD)ptObjTxt->FrameCnt[1] >> 8) + 1);
		return 0;
	}
	if (dwLevelId == 74)
	{
		LevelWarpUnit(pObjectParam->ptGame, pObjectParam->hPlayer, LEVEL_ACT_2_BASEMENT_3, 0);
		A2Q4LevelObjects(pObjectParam->ptGame, LEVEL_ACT_2_ARCANE);
		return 0;
	}
	if (dwLevelId == 54)
	{
		LevelWarpUnit(pObjectParam->ptGame, pObjectParam->hPlayer, LEVEL_ACT_2_ARCANE, 11);
		A2Q4LevelObjects(pObjectParam->ptGame, LEVEL_ACT_2_BASEMENT_3);
	}
	return 0;
}

int __fastcall ObjectModeOperateHellgate(OBJOPERATE *pObjectParam)
{
	DWORD dwLevelId;
	GAME *ptGame;
	UNIT *hPlayer;
	WORD *pwHistory;
	WORD wIndex;

	dwLevelId = D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(pObjectParam->hObject));
	if (pObjectParam->hObject->dwMode)
	{
		QuestsObjectLevel(pObjectParam->ptGame, pObjectParam->hPlayer, dwLevelId, pObjectParam->hObject);
		if (dwLevelId == 102)
		{
			hPlayer = pObjectParam->hPlayer;
			ptGame = pObjectParam->ptGame;
			pwHistory = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apwWaypoints[
				ptGame->bDifficultyLevel];
			if (D2CommonWaypointGetLevelIndex(LEVEL_ACT_4_TOWN, &wIndex))
				D2CommonWaypointSetField(pwHistory, wIndex);
			LevelWarpUnit(pObjectParam->ptGame, pObjectParam->hPlayer, LEVEL_ACT_4_TOWN, 0);
		}
	}
	return 0;
}

int __fastcall ObjectModeOperateGuildStone(OBJOPERATE *pObjectParam)
{
	OBJLINKNODE *ptOwner;
	DWORD dwPlayerId;

	if (PlayerIsBusy(pObjectParam->hPlayer))
		return 0;
	if (pObjectParam->dwClassId != 337)
		return 0;
	ptOwner = ((OBJLINKS *)D2CommonUnitGetObjectLinks(pObjectParam->hObject))->ptFirst;
	if (ptOwner && SUnitFind(pObjectParam->ptGame, UNIT_PLAYER, ptOwner->dwUnitId))
		return 0;
	ptOwner->dwUnitId = (pObjectParam->hPlayer)->dwId;
	ptOwner->ptNext = NULL;
	SUnitSetInteract(pObjectParam->hPlayer, 2, pObjectParam->hObject->dwId);
	GuildsSendSave(pObjectParam->ptGame, SUnitGetClient(pObjectParam->hPlayer));
	GuildsSendRoster(pObjectParam->ptGame, SUnitGetClient(pObjectParam->hPlayer));
	SCmdSendTradeEvent(SUnitGetClient(pObjectParam->hPlayer), 23);
	return 0;
}

void __fastcall ObjectModeGuildStoneRelease(GAME *ptGame, UNIT *hUnit, UNIT *hObject)
{
	OBJLINKNODE *ptOwner;

	if (hObject->dwClassId != 337)
		return;
	ptOwner = ((OBJLINKS *)D2CommonUnitGetObjectLinks(hObject))->ptFirst;
	if (hUnit && (!ptOwner || SUnitFind(ptGame, UNIT_PLAYER, ptOwner->dwUnitId) != hUnit))
		return;
	if (ptOwner)
		ptOwner->dwUnitId = (DWORD)-1;
}

int __fastcall ObjectModeOperateGuildVault(OBJOPERATE *pObjectParam)
{
	UNIT *hObject;
	void *hInventory;
	void *hNode;
	UNIT *hItem;
	void *hNext;
	unsigned __int64 qwCounter;

	if (PlayerIsBusy(pObjectParam->hPlayer))
		return 0;
	if (pObjectParam->dwClassId != 338)
		return 0;
	hObject = pObjectParam->hObject;
	SUnitSetInteract(pObjectParam->hPlayer, 2, hObject->dwId);
	hInventory = hObject->ptInventory;
	if (!hInventory)
	{
		FogErrorManagerLog("guild vault inventory null!\n");
		return 0;
	}
	for (hNode = D2CommonINVENTORYGetCategoryListHead(hInventory); hNode; hNode = hNext)
	{
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(hNode);
		hNext = (void *)D2CommonINVENTORYNodeGetNext(hNode);
		hItem->dwFlags |= 4;
		(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
	}
	D2CommonUnitSetChanged(hObject, TRUE);
	INVENTORYModeSendItems(pObjectParam->ptGame, hObject, SUnitGetClient(pObjectParam->hPlayer), NULL);
	INVENTORYModeResetItems(pObjectParam->ptGame, hObject);
	qwCounter = D2CommonGuildsGetCounterA(GuildsGetData(pObjectParam->ptGame));
	SCmdSendGuildCounter(SUnitGetClient(pObjectParam->hPlayer), 0, 8, (const BYTE *)&qwCounter);
	SCmdSendTradeEvent(SUnitGetClient(pObjectParam->hPlayer), 27);
	return 0;
}

int __fastcall ObjectModeOperateTrophyCase(OBJOPERATE *pObjectParam)
{
	UNIT *hObject;
	void *hInventory;
	void *hNode;
	UNIT *hItem;
	void *hNext;

	if (PlayerIsBusy(pObjectParam->hPlayer))
		return 0;
	if (pObjectParam->dwClassId != 339)
		return 0;
	hObject = pObjectParam->hObject;
	SUnitSetInteract(pObjectParam->hPlayer, 2, hObject->dwId);
	hInventory = hObject->ptInventory;
	if (!hInventory)
	{
		FogErrorManagerLog("trophy case inventory null!\n");
		return 0;
	}
	for (hNode = D2CommonINVENTORYGetCategoryListHead(hInventory); hNode; hNode = hNext)
	{
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(hNode);
		hNext = (void *)D2CommonINVENTORYNodeGetNext(hNode);
		hItem->dwFlags |= 4;
		(void)D2CommonINVENTORYAddDeltaNode(hInventory, hItem);
	}
	D2CommonUnitSetChanged(hObject, TRUE);
	INVENTORYModeSendItems(pObjectParam->ptGame, hObject, SUnitGetClient(pObjectParam->hPlayer), NULL);
	INVENTORYModeResetItems(pObjectParam->ptGame, hObject);
	SCmdSendTradeEvent(SUnitGetClient(pObjectParam->hPlayer), 32);
	return 0;
}

int __fastcall ObjectModeOperateMessageBoard(OBJOPERATE *pObjectParam)
{
	if (!PlayerIsBusy(pObjectParam->hPlayer) && pObjectParam->dwClassId == 340)
	{
		GuildsSendPosts(pObjectParam->ptGame, SUnitGetClient(pObjectParam->hPlayer));
		SUnitSetInteract(pObjectParam->hPlayer, 2, pObjectParam->hObject->dwId);
		SCmdSendTradeEvent(SUnitGetClient(pObjectParam->hPlayer), 35);
	}
	return 0;
}

static __inline void *ObjPlrQuestRecordI(PLRINFO *ptPlrInfo, GAME *ptGame)
{
	return ptPlrInfo->apQuestRecords[ptGame->bDifficultyLevel];
}

int __fastcall ObjectModeOperateTownPortal(OBJOPERATE *pObjectParam)
{
	MISSOBJECTSTXT *ptObjTxt;
	GAMEROOM *hRoom;
	UNIT *hTarget;
	PLRINFO *ptPlrInfo;
	int nLevelId;

	COLPOINT tPoint;
	GAMEROOM *hRoomDest;
	BYTE bFlags;
	GAMEROOM *hRoomFree;
	int nX;
	int nY;
	BOOL fRet;
	CLIENT *ptClient;
	WORD wClass;
	int nFrame;
	void *hList;

	if (PlayerIsBusy(pObjectParam->hPlayer))
		return 0;
	if (PlayerPetsTestAnyPlayer(pObjectParam->ptGame, pObjectParam->hPlayer)
		&& (pObjectParam->hPlayer)->dwId != (DWORD)ObjectEvalGetLinkValue(pObjectParam->hObject))
		return 0;
#line 3308
	D2ASSERT(UnitGetType ( pObjectParam->hPlayer ) == UNIT_PLAYER);
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(pObjectParam->hObject);
	if (hRoom && D2CommonDungeonRoomGetLevelId(hRoom) == 1
		&& OBJDATA(pObjectParam->hObject)->field_04 == 39)
	{ GAME *ptGame;
		ptGame = pObjectParam->ptGame;
		ptPlrInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(pObjectParam->hPlayer);
		if (!D2CommonStepTestGridBit(ObjPlrQuestRecordI(ptPlrInfo, ptGame), 26,
			0))
		{
			SUnitSetEvent(pObjectParam->hPlayer, 18, pObjectParam->hPlayer);
			return 0;
		}
	}
	hTarget = SUnitGetPortalTarget(pObjectParam->ptGame, pObjectParam->hObject);
	if (hTarget->eType != UNIT_OBJECT)
		hTarget = NULL;
	if (!hTarget)
	{ int nX0; int nY0;
		nLevelId = OBJDATA(pObjectParam->hObject)->field_04;
		hRoomDest = (GAMEROOM *)D2CommonDungeonGetActStartRoom(pObjectParam->ptGame->ptAct[
			D2CommonGetActNum(nLevelId)], nLevelId, 0, &nX0, &nY0, 3);
		tPoint.nX = nX0;
		tPoint.nY = nY0;
	}
	else
	{
		tPoint.nX = (int)D2CommonUnitGetX(hTarget);
		tPoint.nY = (int)D2CommonUnitGetY(hTarget);
		hRoomDest = (GAMEROOM *)D2CommonUnitGetPathRoom(hTarget);
	}
	bFlags = D2CommonUnitGetObjectFlags(pObjectParam->hObject) | 5;
	D2CommonUnitSetObjectFlags(pObjectParam->hObject, bFlags);
	if (D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(pObjectParam->hPlayer)))
	{
		QuestsLevelChange(pObjectParam->ptGame,
			D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(pObjectParam->hPlayer)),
			D2CommonDungeonRoomGetLevelId(hRoomDest), pObjectParam->hPlayer);
	}
	hRoomFree = (GAMEROOM *)D2CommonCollisnFindSpawnPointNear(hRoomDest, &tPoint,
		D2CommonUnitGetSizeX(pObjectParam->hPlayer), 7177, 0);
	if (!hRoomFree)
		return 0;
	nX = tPoint.nX;
	nY = tPoint.nY;
	fRet = SUnitMoveToRoom(pObjectParam->ptGame, pObjectParam->hPlayer, hRoomFree, nX, nY, 0, 0);
#line 3389
	D2ASSERT(fRet);
	ptClient = SUnitGetClient(pObjectParam->hPlayer);
	SUnitSetEvent(pObjectParam->hPlayer, 8, NULL);
	PlrModeStartXY(pObjectParam->ptGame, pObjectParam->hPlayer, NULL, 2, nX + 5, nY + 5, FALSE);
	SCmdSendUnitModeAt(ptClient, S2C_UNIT_MODE_POS, (BYTE)(pObjectParam->hPlayer)->eType,
		(pObjectParam->hPlayer)->dwId, 1, (WORD)(nX + 5), (WORD)(nY + 5), 0, 0);
	wClass = (WORD)pObjectParam->hObject->dwClassId;
	if (hTarget && wClass != 60)
	{
		if (PlayerGetPortalId(pObjectParam->hPlayer) == hTarget->dwId)
		{
			hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(pObjectParam->hObject);
			D2CommonDungeonRoomAddNode(hRoom, UNIT_OBJECT, pObjectParam->hObject->dwId);
			SUnitFree(pObjectParam->ptGame, pObjectParam->hObject);
			D2CommonDungeonRoomSetHold(hRoom, TRUE);
			hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hTarget);
			D2CommonDungeonRoomAddNode(hRoom, UNIT_OBJECT, hTarget->dwId);
			SUnitFree(pObjectParam->ptGame, hTarget);
			D2CommonDungeonRoomSetHold(hRoom, TRUE);
		}
	}
	else if (pObjectParam->hObject->dwMode == 1)
	{
		ptObjTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(wClass);
		EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
			pObjectParam->ptGame->nFrame + ((DWORD)ptObjTxt->FrameCnt[1] >> 8) + 1);
	}
	nFrame = pObjectParam->ptGame->nFrame + 75;
	hList = (void *)D2CommonStatsAllocList(pObjectParam->ptGame->pMemPool, 2, nFrame, 0,
		(pObjectParam->hPlayer)->eType);
	if (hList)
	{
		EventAdd(pObjectParam->ptGame, pObjectParam->hPlayer, 12, nFrame);
		D2CommonStatsUnitSetActiveStateBit(pObjectParam->hPlayer, 102, TRUE);
		D2CommonStatsSetListState(hList, 102);
		D2CommonStatsSetListRemoveCallback(hList, SkillsStateRemove);
		D2CommonStatsUnitLinkList(pObjectParam->hPlayer, hList);
	}
	return 0;
}

int __fastcall ObjectModeOperateLevelPortal(OBJOPERATE *pObjectParam)
{
	BYTE bFlags;
	int nLevelId;
	int nX;
	int nY;
	BYTE bAct;
	GAMEROOM *hRoomDest;
	COLPOINT tPoint;
	GAMEROOM *hRoomFree;
	BOOL fRet;

#line 3476
	D2ASSERT(UnitGetType ( pObjectParam->hPlayer ) == UNIT_PLAYER);
	bFlags = D2CommonUnitGetObjectFlags(pObjectParam->hObject) | 5;
	D2CommonUnitSetObjectFlags(pObjectParam->hObject, bFlags);
	nLevelId = 0;
	if (pObjectParam->hObject->dwClassId != 100)
	{
		if (pObjectParam->hObject->dwClassId == 377)
			nLevelId = OBJDATA(pObjectParam->hObject)->field_04;
	}
	else
		nLevelId = 73;
	nX = 0;
	nY = 0;
	bAct = D2CommonGetActNum(nLevelId);
#line 3500
	D2ASSERT(bAct < NUM_ACTS);
	hRoomDest = (GAMEROOM *)D2CommonDungeonGetActStartRoom(pObjectParam->ptGame->ptAct[bAct], nLevelId, 0,
		&nX, &nY, D2CommonUnitGetSizeX(pObjectParam->hPlayer));
#line 3502
	D2ASSERT(hRoomDest);
	tPoint.nX = nX;
	tPoint.nY = nY;
	hRoomFree = (GAMEROOM *)D2CommonCollisnFindSpawnPointNear(hRoomDest, &tPoint,
		D2CommonUnitGetSizeX(pObjectParam->hPlayer), 7177, 0);
	if (hRoomFree)
	{
		fRet = SUnitMoveToRoom(pObjectParam->ptGame, pObjectParam->hPlayer, hRoomFree,
			tPoint.nX, tPoint.nY, 0, 0);
#line 3514
		D2ASSERT(fRet);
	}
	return 0;
}

int __fastcall ObjectModeOperateWaypoint(OBJOPERATE *pObjectParam)
{
	MISSOBJECTSTXT *ptObjTxt;
	DWORD dwLevelId;
	GAME *ptGame;
	UNIT *hPlayer;
	WORD *pwHistory;
	WORD wIndex;
	BYTE abPacket[21];

	dwLevelId = D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(pObjectParam->hObject));
	hPlayer = pObjectParam->hPlayer;
	ptGame = pObjectParam->ptGame;
	pwHistory = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apwWaypoints[
		ptGame->bDifficultyLevel];
	if (D2CommonWaypointGetLevelIndex(dwLevelId, &wIndex))
		D2CommonWaypointSetField(pwHistory, wIndex);
	if (pObjectParam->hObject->dwMode == 0)
	{
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		ptObjTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
		EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
			pObjectParam->ptGame->nFrame + ((DWORD)ptObjTxt->FrameCnt[1] >> 8) + 1);
		return 1;
	}
	else if (pObjectParam->hObject->dwMode == 2 || pObjectParam->hObject->dwMode == 1)
	{
		if (!PlayerIsBusy(pObjectParam->hPlayer))
		{
			abPacket[0] = 92;
			*(DWORD *)&abPacket[1] = pObjectParam->hObject->dwId;
			D2CommonWaypointExportHistory(pwHistory, &abPacket[5]);
			SCmdSendWaypoints(SUnitGetClient(pObjectParam->hPlayer), (const struct SCMDCOPY21 *)abPacket);
			SUnitSetInteract(pObjectParam->hPlayer, 2, pObjectParam->hObject->dwId);
		}
	}
	return 1;
}

void __fastcall ObjectModeWaypointTravel(GAME *ptGame, UNIT *hPlayer, DWORD dwObjectId, WORD wLevelId)
{
	UNIT *hObject;
	int nLevelId;
	int nObjectLevel;
	WORD *pwHistory;
	WORD wIndex;
	DWORD dwArrival;
	GAMEROOM *hRoom;
	int nX;
	int nY;
	CLIENT *ptClient;
	DWORD dwUnitId;
	COLPOINT tCoord;

	hObject = SUnitFind(ptGame, UNIT_OBJECT, dwObjectId);
	if (!hObject)
		return;
	nLevelId = wLevelId;
	nObjectLevel = (int)D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hObject));
	SUnitResetInteract(hPlayer);
	if (nObjectLevel == nLevelId)
		return;
	if (!wLevelId)
		return;
	pwHistory = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apwWaypoints[ptGame->bDifficultyLevel];
	if (!D2CommonWaypointGetLevelIndex(nLevelId, &wIndex))
		return;
	if (!((BOOL (__stdcall *)(DWORD, WORD))D2CommonWaypointTestFieldRaw)((DWORD)pwHistory, wIndex))
		return;
	if (D2CommonDungeonLevelIsTown(nLevelId) || nLevelId == 46 || nLevelId == 74)
		dwArrival = 13;
	else
		dwArrival = 0;
	LevelWarpUnit(ptGame, hPlayer, nLevelId, dwArrival);
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hPlayer);
	if (hRoom && hRoom == (GAMEROOM *)D2CommonDungeonGetWarpRoom(ptGame->ptAct[D2CommonGetActNum(nLevelId)],
		nLevelId, dwArrival, &nX, &nY))
	{
		ptClient = SUnitGetClient(hPlayer);
		dwUnitId = hPlayer->dwId;
		D2CommonUnitGetPosition(hPlayer, &tCoord);
		PlrModeStartXY(ptGame, hPlayer, NULL, 2, tCoord.nX, tCoord.nY, FALSE);
		SCmdSendUnitModeAt(ptClient, S2C_UNIT_MODE_POS, UNIT_PLAYER, dwUnitId, 1, (WORD)(tCoord.nX + 3),
			(WORD)(tCoord.nY + 3), 0, 0);
	}
	ObjectRgnAddNode(ptGame, hPlayer, hRoom);
}

BOOL __fastcall ObjectModePowerupMaxMana(GAME *ptGame, UNIT *hUnit, int nValue)
{
	int nMana;

	nMana = D2CommonStatsUnitCalcMaxMana(hUnit) + (nValue << 8);
	D2CommonStatsUnitSetValue(hUnit, 8, nMana);
	D2CommonStatsUnitSetValue(hUnit, 9, nMana);
	return TRUE;
}

BOOL __fastcall ObjectModePowerupEnergy(GAME *ptGame, UNIT *hUnit, int nValue)
{
	D2CommonStatsUnitAddValue(hUnit, 1, nValue);
	return TRUE;
}

BOOL __fastcall ObjectModePowerupDexterity(GAME *ptGame, UNIT *hUnit, int nValue)
{
	D2CommonStatsUnitAddValue(hUnit, 2, nValue);
	return TRUE;
}

#define UNIT_PLAYER_TYPE	0
#define OBJECTCLASS_TOWNPORTAL	59
#define UnitGetClass(hUnit)	((hUnit)->dwClassId)

typedef struct OBJPOWERUP OBJPOWERUP;
typedef struct OBJACTLEVELS OBJACTLEVELS;
typedef struct OBJMODEPACKET OBJMODEPACKET;

typedef BOOL (__fastcall *PFNOBJPOWERUP)(GAME *ptGame, UNIT *hPlayer, int nValue);
struct OBJPOWERUP
{
 	PFNOBJPOWERUP	pPowerup;
 	DWORD		dwChance;
 	int			nValue;
};

struct OBJACTLEVELS
{
 	int			nStart;
 	int			nEnd;
};

#pragma pack(push, 1)
struct OBJMODEPACKET
{
 	BYTE		bCmd;
 	DWORD		dwObjectId;
 	BYTE		bState;
 	BYTE		bResult;
};
#pragma pack(pop)

static OBJPOWERUP sgObeliskPowerup[] =
{
	{ ObjectModePowerupMaxMana,    100, 1 },
	{ ObjectModePowerupMaxMana,    100, 1 },
	{ ObjectModePowerupMaxMana,    100, 2 },
	{ ObjectModePowerupEnergy,      5, 1 },
	{ ObjectModePowerupEnergy,      10, 1 },
	{ ObjectModePowerupEnergy,      15, 1 },
	{ ObjectModePowerupDexterity,      5, 1 },
	{ ObjectModePowerupDexterity,      10, 1 },
	{ ObjectModePowerupDexterity,      15, 1 },
	{ ObjectModePowerupVitality,  5, 1 },
	{ ObjectModePowerupVitality,  10, 1 },
	{ ObjectModePowerupVitality,  15, 1 },
	{ ObjectModePowerupStrength,  5, 1 },
	{ ObjectModePowerupStrength,  10, 1 },
	{ ObjectModePowerupStrength,  15, 1 },
	{ ObjectModePowerupLife,   100, 1 },
	{ ObjectModePowerupLife,   100, 1 },
	{ ObjectModePowerupLife,   100, 2 },
	{ ObjectModePowerupNewSkills,  3, 1 },
	{ ObjectModePowerupNewSkills,  6, 1 },
	{ ObjectModePowerupNewSkills,  10, 1 },
};
static int sgnObeliskPowerups = arrsize(sgObeliskPowerup);

static DWORD sgadwActTreasure[12] =
{
	48, 49, 50,
	51, 52, 53,
	54, 55, 56,
	57, 57, 57,
};
static int PLACEHOLDER_UnreferencedData_100ac230 = sizeof(sgadwActTreasure) / sizeof(sgadwActTreasure[0]);

static OBJACTLEVELS sgatActLevels[4] =
{
	{ 2, 39 },
	{ 41, 73 },
	{ 76, 102 },
	{ 104, 108 },
};
static int PLACEHOLDER_UnreferencedData_100ac258 = sizeof(sgatActLevels) / sizeof(DWORD);

BOOL __fastcall ObjectModePowerupVitality(GAME *ptGame, UNIT *hPlayer, int nValue)
{
	D2CommonStatsUnitAddValue(hPlayer, 3, nValue);
	return TRUE;
}

BOOL __fastcall ObjectModePowerupStrength(GAME *ptGame, UNIT *hPlayer, int nValue)
{
	D2CommonStatsUnitAddValue(hPlayer, 0, nValue);
	return TRUE;
}

BOOL __fastcall ObjectModePowerupLife(GAME *ptGame, UNIT *hPlayer, int nValue)
{
	int nLife;

	nLife = D2CommonStatsUnitCalcMaxHp(hPlayer) + (nValue << 8);
	D2CommonStatsUnitSetValue(hPlayer, 6, nLife);
	D2CommonStatsUnitSetValue(hPlayer, 7, nLife);
	return TRUE;
}

BOOL __fastcall ObjectModePowerupNewSkills(GAME *ptGame, UNIT *hPlayer, int nValue)
{
	D2CommonStatsUnitAddValue(hPlayer, 5, nValue);
	return TRUE;
}

void __fastcall ObjectModeObeliskUse(GAME *ptGame, DWORD dwPlayerId, DWORD dwObjectId, DWORD dwItemId,
	WORD wCall)
{
	UNIT *hObject;
	UNIT *hPlayer;
	UNIT *hItem;
	CLIENT *ptClient;
	OBJMODEPACKET tPacket;
	ITEMSTXT *ptItemTxt;
	MISSOBJECTSTXT *ptObjTxt;
	BYTE bSubType;
	DWORD dwMode;
	int nResult;

	hObject = SUnitFind(ptGame, UNIT_OBJECT, dwObjectId);
	hPlayer = SUnitFind(ptGame, UNIT_PLAYER_TYPE, dwPlayerId);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (hPlayer && hObject)
	{
		ptClient = SUnitGetClient(hPlayer);
		tPacket.bCmd = 81;
		tPacket.dwObjectId = dwObjectId;
		dwMode = hObject->dwMode;
		switch (wCall)
		{
		case 2:
			if (dwMode == 3)
			{
				tPacket.bState = 1;
				SCmdSendUseItemState(ptClient, (const struct SCMDCOPY7 *)&tPacket);
				D2CommonUnitSetMode(hObject, 0);
				SUnitResetInteract(hPlayer);
				return;
			}
			tPacket.bState = 1;
			SCmdSendUseItemState(ptClient, (const struct SCMDCOPY7 *)&tPacket);
			D2CommonUnitSetMode(hObject, 0);
			SUnitResetInteract(hPlayer);
			return;
		case 3:
			ptItemTxt = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
			if (hObject->dwClassId == 152)
			{
				if (ptItemTxt->code != 544502632)
				{
					tPacket.bState = 4;
					SCmdSendUseItemState(ptClient, (const struct SCMDCOPY7 *)&tPacket);
					break;
				}
			}
			else if (!ITEMSModeFreeCursorItem(ptGame, hPlayer, hItem))
			{
				tPacket.bState = 4;
				SCmdSendUseItemState(ptClient, (const struct SCMDCOPY7 *)&tPacket);
				return;
			}
			tPacket.bState = 5;
			SUnitResetInteract(hPlayer);
			if (hObject->dwClassId == 152)
				tPacket.bResult = 1;
			else
			{
				bSubType = ptItemTxt->subtype;
				if ((int)bSubType < sgnObeliskPowerups
					&& (nResult = D2CommonSeedRand(&hPlayer->tSeed, 100), 1)
					&& (DWORD)nResult < sgObeliskPowerup[bSubType].dwChance)
				{
#line 3763
					D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgObeliskPowerup[bSubType].pPowerup), "sgObeliskPowerup[bSubType].pPowerup");
					nResult = sgObeliskPowerup[bSubType].pPowerup(ptGame, hPlayer,
						sgObeliskPowerup[bSubType].nValue);
				}
				else
					nResult = 0;
				tPacket.bResult = (BYTE)nResult;
			}
			SCmdSendUseItemState(ptClient, (const struct SCMDCOPY7 *)&tPacket);
			if (tPacket.bResult == 1)
			{
				D2CommonUnitSetMode(hObject, 1);
				ptObjTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hObject->dwClassId);
				if (hObject->dwClassId == 152)
				{
					D2CommonUnitSetMode(hObject, 2);
					A2Q6ObjectUsed(ptGame, hPlayer, hObject);
					return;
				}
				EventAdd(ptGame, hObject, 1, (int)((DWORD)ptObjTxt->FrameCnt[1] >> 8) + ptGame->nFrame + 1);
				return;
			}
			D2CommonUnitSetMode(hObject, 0);
			return;
		}
		return;
	}
#line 3783
	D2ASSERT(hPlayer != NULL || hObject != NULL);
}

UNIT *__fastcall ObjectModeReleasePlayer(GAME *ptGame, UNIT *hPlayer, UNIT *hObject)
{
	WORD wClass;
	OBJLINKNODE *ptNode;

	wClass = (WORD)hObject->dwClassId;
	if (wClass == 80 || wClass == 90)
		ObjectModeObeliskUse(ptGame, hPlayer->dwId, hObject->dwId, 0, 2);
	else if (wClass == 337)
	{
		if (hObject->dwClassId == 337)
		{
			ptNode = ((OBJLINKS *)D2CommonUnitGetObjectLinks(hObject))->ptFirst;
			if ((!hPlayer || (ptNode && SUnitFind(ptGame, UNIT_PLAYER_TYPE, ptNode->dwUnitId) == hPlayer))
				&& ptNode)
				ptNode->dwUnitId = (DWORD)-1;
		}
	}
	else if (wClass == 152)
		D2CommonUnitSetMode(hObject, 0);
	return SUnitResetInteract(hPlayer);
}

void __fastcall ObjectModeReset(GAME *ptGame, UNIT *hObject)
{
	OBJLINKNODE *ptNode;

	if (hObject->dwClassId == 337)
	{
		ptNode = ((OBJLINKS *)D2CommonUnitGetObjectLinks(hObject))->ptFirst;
		if (ptNode)
			ptNode->dwUnitId = (DWORD)-1;
		return;
	}
	D2CommonUnitSetMode(hObject, 0);
}

void __fastcall ObjectModeTownPortalLink(GAME *ptGame, UNIT *hPlayer, DWORD dwPortalId,
	DWORD dwParam2  )
{
	UNIT *hSrcPortal;
	UNIT *hDstPortal;
	UNIT *hOtherPortal;
	DWORD dwId;
	DWORD dwByte;
	BYTE bFlags;

#line 3940
	D2ASSERT(ptGame);
	hSrcPortal = SUnitFind(ptGame, UNIT_OBJECT, dwPortalId);
#line 3943
	D2ASSERT(hSrcPortal);
#line 3944
	D2ASSERT(UnitGetClass(hSrcPortal) == OBJECTCLASS_TOWNPORTAL);
	if (D2CommonUnitTestObjectFlags(hSrcPortal, 1))
		return;
	hDstPortal = SUnitGetPortalTarget(ptGame, hSrcPortal);
#line 3948
	D2ASSERT(hDstPortal);
#line 3949
	D2ASSERT(UnitGetClass(hDstPortal) == OBJECTCLASS_TOWNPORTAL);
	dwByte = (BYTE)dwParam2;
#line 3951
	D2ASSERTMSG(dwByte <= 255, "(BYTE)dwParam2 <= 255");
	OBJDATA(hSrcPortal)->field_04 = (BYTE)dwParam2;
	bFlags = (BYTE)(D2CommonUnitGetObjectFlagsRaw((DWORD)hSrcPortal) | 1);
	D2CommonUnitSetObjectFlags(hSrcPortal, bFlags);
	D2CommonUnitSetMode(hSrcPortal, 3);
	D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hDstPortal), 2, hDstPortal->dwId);
	SUnitFree(ptGame, hDstPortal);
	hOtherPortal = SkillsOpenPortalEnd(ptGame, hSrcPortal, dwParam2,
		(int)D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hSrcPortal)));
#line 3966
	D2ASSERT(hOtherPortal);
	bFlags = (BYTE)(D2CommonUnitGetObjectFlagsRaw((DWORD)hOtherPortal) | 3);
	D2CommonUnitSetObjectFlags(hOtherPortal, bFlags);
}

int __fastcall ObjectModeOperateWell(OBJOPERATE *ptOperate)
{
	MISSOBJECTSTXT *ptTxt;
	BYTE bCharges;
	UNIT *hPlayer;
	BOOL fRestored;
	DWORD dwMax;
	DWORD dwValue;
	DWORD dwFlags;
	DWORD dwNew;
	void *ptList;
	BYTE bMax;
	int nCharges;
	BYTE bStep;

	ptTxt = (MISSOBJECTSTXT *)D2CommonUnitGetObjectClassInfoPtr(ptOperate->hObject);
	bCharges = OBJDATA(ptOperate->hObject)->field_04;
	if (!bCharges)
		return 0;
	hPlayer = ptOperate->hPlayer;
	fRestored = FALSE;
	dwMax = D2CommonStatsUnitCalcMaxHp(hPlayer);
	dwValue = D2CommonStatsUnitGetValue(hPlayer, 6);
	dwFlags = (DWORD)ptTxt->Param[3];
	if (dwValue < dwMax && (dwFlags & 2))
	{
		dwNew = ((DWORD)ptTxt->Param[1] * dwMax >> 8) + dwValue;
		if (dwNew > dwMax)
			dwNew = dwMax;
		D2CommonStatsUnitSetValue(hPlayer, 6, dwNew);
		PlrMsgStatChanged(hPlayer, 6, (int)dwNew, hPlayer);
		fRestored = TRUE;
	}
	dwMax = D2CommonStatsUnitCalcMaxMana(hPlayer);
	dwValue = D2CommonStatsUnitGetValue(hPlayer, 8);
	if (dwValue < dwMax && (dwFlags & 1))
	{
		dwNew = ((DWORD)ptTxt->Param[1] * dwMax >> 8) + dwValue;
		if (dwNew > dwMax)
			dwNew = dwMax;
		D2CommonStatsUnitSetValue(hPlayer, 8, dwNew);
		PlrMsgStatChanged(hPlayer, 8, (int)dwNew, hPlayer);
		fRestored = TRUE;
	}
	dwMax = D2CommonStatsUnitCalcMaxStamina(hPlayer);
	dwValue = D2CommonStatsUnitGetValue(hPlayer, 10);
	if (dwValue < dwMax)
	{
		dwNew = ((DWORD)ptTxt->Param[1] * dwMax >> 8) + dwValue;
		if (dwNew > dwMax)
			dwNew = dwMax;
		D2CommonStatsUnitSetValue(hPlayer, 10, dwNew);
		PlrMsgStatChanged(hPlayer, 10, (int)dwNew, hPlayer);
		fRestored = TRUE;
	}
	ptList = (void *)D2CommonStatsFindListByState(hPlayer, 2);
	if (ptList)
	{
		D2CommonStatsUnitUnlinkList(hPlayer, ptList);
		D2CommonStatsListFree(ptList);
		fRestored = TRUE;
	}
	ptList = (void *)D2CommonStatsFindListByState(hPlayer, 1);
	if (ptList)
	{
		D2CommonStatsUnitUnlinkList(hPlayer, ptList);
		D2CommonStatsListFree(ptList);
	}
	else if (!fRestored)
		return 0;
	bCharges--;
	bMax = (BYTE)ObjectRegenerateMax(ptTxt);
	if (bCharges <= bMax)
	{
		nCharges = bCharges;
		bStep = (BYTE)(bMax / ObjectRegenerateModes());
		if (!(nCharges % bStep))
			D2CommonUnitSetMode(ptOperate->hObject,
				ObjectRegenerateModes() - nCharges / ptTxt->Param[2]);
	}
	OBJDATA(ptOperate->hObject)->field_04 = bCharges;
	EventAdd(ptOperate->ptGame, ptOperate->hObject, 2, ptTxt->Param[0] + ptOperate->ptGame->nFrame + 1);
	return 0;
}

int __fastcall ObjectModeOperateTreasureChest(OBJOPERATE *ptOperate)
{
	MISSOBJECTSTXT *ptTxt;

	if (ptOperate->hObject->dwMode != 0)
		return 0;
	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(ptOperate->hObject->dwClassId);
	if (D2CommonSeedRand(ptOperate->ptSeed, 100) < 90)
	{
		if (ObjectDropOne(ptOperate, 0))
			D2CommonUnitSetMode(ptOperate->hObject, 5);
		return 0;
	}
	D2CommonUnitSetMode(ptOperate->hObject, 1);
	EventAdd(ptOperate->ptGame, ptOperate->hObject, 1,
		(int)((DWORD)ptTxt->FrameCnt[1] >> 8) + ptOperate->ptGame->nFrame + 1);
	EventAdd(ptOperate->ptGame, ptOperate->hObject, 9, ptOperate->ptGame->nFrame + 20);
	return 0;
}

UNIT *__fastcall ObjectDropOne(OBJOPERATE *ptOperate, int nArg)
{
	GAME *ptGame;
	GAMEROOM *hRoom;
	int nLevelId;
	MISSLEVELDATA *pObject;
	DWORD dwMonLvl;
	BYTE bAct;
	MISSOBJECTSTXT *ptTypeData;
	MISSLEVELDATA *pStart;
	MISSLEVELDATA *pEnd;
	DWORD dwStart;
	DWORD dwEnd;
	DWORD dwRange;
	int nDiff;
	int nIndex;
	DWORD i;
	DWORD dwLow;
	DWORD dwTcId;
	void *ptTreasureClass;
	DWORD dwCode;

	ptGame = SUnitGetGame(ptOperate->hObject);
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(ptOperate->hObject);
#line 4204
	D2ASSERT(hRoom);
	nLevelId = (int)D2CommonDungeonRoomGetLevelId(hRoom);
	pObject = (MISSLEVELDATA *)D2CommonLvlTblsGetLevelData(nLevelId);
#line 4207
	D2ASSERT(pObject);
	dwMonLvl = pObject->MonLvl[ptGame->bDifficultyLevel];
	bAct = D2CommonGetActNum(nLevelId);
	ptTypeData = (MISSOBJECTSTXT *)D2CommonUnitGetObjectClassInfoPtr(ptOperate->hObject);
#line 4211
	D2ASSERT(ptTypeData);
	pStart = (MISSLEVELDATA *)D2CommonLvlTblsGetLevelData(sgatActLevels[bAct].nStart);
#line 4214
	D2ASSERT(pStart);
	dwStart = pStart->MonLvl[ptGame->bDifficultyLevel];
	pEnd = (MISSLEVELDATA *)D2CommonLvlTblsGetLevelData(sgatActLevels[bAct].nEnd);
#line 4217
	D2ASSERT(pEnd);
	dwEnd = pEnd->MonLvl[ptGame->bDifficultyLevel];
	nDiff = (int)(dwEnd - dwStart);
	if (nDiff < 0)
		nDiff = -nDiff;
	dwRange = nDiff + 1;
	dwRange /= 3;
	if (dwMonLvl < dwStart)
		nIndex = 0;
	else if (dwMonLvl > dwEnd)
	{
		nIndex = 2;
		if (!(D2CommonSeedNext(ptOperate->ptSeed) & 1))
			nIndex = 1;
	}
	else
	{
		for (i = 0, dwLow = dwStart; i < 3; i++, dwLow += dwRange)
		{
			nIndex = i;
			if (dwMonLvl >= dwLow && dwMonLvl < dwLow + dwRange)
			{
				if (i > 0 && !(D2CommonSeedNext(ptOperate->ptSeed) & 1))
					nIndex = i - 1;
				break;
			}
		}
	}
	dwTcId = sgadwActTreasure[bAct * 3 + nIndex];
	ptTreasureClass = (void *)D2CommonMonsterTblsGetTreasureClass(dwTcId);
#line 4248
	D2ASSERT(ptTreasureClass);
	dwCode = ITEMSModeDrawCode(ptOperate->ptGame, (struct ITEMCODELIST *)ptTreasureClass, ptOperate->hObject,
		0);
#line 4252
	D2ASSERT(dwCode);
	ptGame = ptOperate->ptGame;
	return SkillsDropItem(ptGame, ptOperate->hObject, dwCode, nArg);
}
