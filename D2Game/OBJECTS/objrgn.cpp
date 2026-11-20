#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\OBJECTS\\objrgn.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "../GAME/SCmd.h"
#include "objrgn.h"
#include "../UNIT/Party.h"
#include "../UNIT/PlayerList.h"
#include "../UNIT/SUnit.h"

#define PARTY_MSG_CMD			83
#define PARTY_MSG_EVENT			7
#define PARTY_NONE				65535
#define OBJRGN_MONCLASS_UNSET	410
#define OBJRGN_MONCLASS_DEFAULT	234

typedef void (__fastcall *PFNGAMEUNITCALLBACK)(GAME *ptGame, UNIT *hUnit, void *pArg);

struct PARTYJOINARG
{
 	UNIT		*hTarget;
 	UNIT		*hUnit;
 	WORD		wParty;
 	BYTE		unobserved_0a[2];
};
typedef struct PARTYJOINARG PARTYJOINARG;

void __fastcall PartyScreenJoinEach(GAME *ptGame, UNIT *hPlayer, void *pArg);

DWORD __fastcall ObjectRgnCreate(GAME *ptGame  )
{
	OBJREGION *ptRegion;
	OBJRGNLEVEL *ptLevel;
	MISSLEVELDATA *ptLevelTxt;
	SHRINESTXT *ptShrine;
	DWORD dwSeed;
	int anCount[8];
	int nShrines;
	int nLevelId;
	int i, j;

#line 40
	D2ASSERT(ptGame != NULL);
#line 41
	ptRegion = (OBJREGION *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(OBJREGION));
	ptGame->ptObjRegion = ptRegion;
	D2CommonSeedInit(&ptRegion->tSeed);
	dwSeed = D2CommonSeedInvent(0);
	D2CommonSeedSet(&ptRegion->tSeed, dwSeed);
	ptRegion->ptLevels[0] = NULL;
	ptRegion->ptNodes = NULL;
	for (nLevelId = 1; nLevelId < OBJRGN_NUM_LEVELS; nLevelId++)
	{
#line 53
		ptLevel = (OBJRGNLEVEL *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(OBJRGNLEVEL));
		ptLevelTxt = (MISSLEVELDATA *)D2CommonLvlTblsGetLevelData(nLevelId);
		for (i = 0; i < (int)(sizeof(OBJRGNLEVEL) / sizeof(DWORD)); i++)
			((DWORD *)ptLevel)[i] = 0;
		ptLevel->nMonClass = OBJRGN_MONCLASS_UNSET;
		ptLevel->bAct = ptLevelTxt->Act;
		ptLevel->nRooms = 2147483647;
		ptRegion->ptLevels[nLevelId] = ptLevel;
	}

	nShrines = D2CommonObjTblsGetShrineCount();
	for (i = 0; i < 8; i++)
		anCount[i] = 0;
	for (j = 0; j < nShrines; j++)
	{
		BYTE bClass;

		ptShrine = (SHRINESTXT *)D2CommonObjTblsGetShrine(j);
		bClass = ptShrine->effectclass;
		if (bClass < 8)
			anCount[bClass]++;
	}

	memset(ptRegion->field_1fc, 0, (50) * sizeof(ptRegion->field_1fc[0]));
	memset(ptRegion->tShrinePop.apnShrineIds, 0, sizeof(ptRegion->tShrinePop.apnShrineIds));
	for (i = 0; i < 8; i++)
	{
		if (anCount[i] > 0)
		{
#line 83
			ptRegion->tShrinePop.apnShrineIds[i] = (int *)D2ALLOCPOOL(ptGame->pMemPool, anCount[i] * sizeof(int));
			memset(ptRegion->tShrinePop.apnShrineIds[i], 0, anCount[i] * sizeof(int));
		}
		ptRegion->tShrinePop.NumShrineSubTypes[i] = 0;
	}
	for (j = 0; j < nShrines; j++)
	{
		BYTE bClass;

		ptShrine = (SHRINESTXT *)D2CommonObjTblsGetShrine(j);
		bClass = ptShrine->effectclass;
		if (bClass < 8)
		{
			ptRegion->tShrinePop.apnShrineIds[bClass][ptRegion->tShrinePop.NumShrineSubTypes[bClass]] = j;
			ptRegion->tShrinePop.NumShrineSubTypes[bClass]++;
		}
	}
	return dwSeed;
}

void __fastcall ObjectRgnFree(GAME *ptGame  )
{
	OBJREGION *ptRegion;
	OBJRGNNODE *ptNode;
	OBJRGNNODE *ptNext;
	int i;

#line 181
	D2ASSERT(ptGame != NULL);
	ptRegion = ptGame->ptObjRegion;
	if (!ptRegion)
		return;
	ptNode = ptRegion->ptNodes;
	while (ptNode)
	{
		ptNext = ptNode->ptNext;
#line 192
		D2FREEPOOL(ptGame->pMemPool, ptNode);
		ptNode = ptNext;
	}
	ptRegion->ptNodes = NULL;
	for (i = 1; i < OBJRGN_NUM_LEVELS; i++)
	{
#line 225
		D2FREEPOOL(ptGame->pMemPool, ptRegion->ptLevels[i]);
	}
	for (i = 0; i < 8; i++)
	{
		if (ptRegion->tShrinePop.apnShrineIds[i])
		{
#line 231
			D2FREEPOOL(ptGame->pMemPool, ptRegion->tShrinePop.apnShrineIds[i]);
		}
	}
#line 235
	D2FREEPOOL(ptGame->pMemPool, ptGame->ptObjRegion);
}

OBJRGNLEVEL * __fastcall ObjectRgnGetLevel(GAME *ptGame, int nLevelId)
{
	return ptGame->ptObjRegion->ptLevels[nLevelId];
}

D2SEED * __fastcall ObjectRgnGetSeed(GAME *ptGame, GAMEROOM *hRoom)
{
	return &ptGame->ptObjRegion->tSeed;
}

SHRINEPOP * __fastcall ObjectRgnGetShrinePop(GAME *ptGame)
{
	return &ptGame->ptObjRegion->tShrinePop;
}

void __fastcall ObjectRgnAddNode(GAME *ptGame, UNIT *hPlayer, GAMEROOM *hRoom)
{
	OBJREGION *ptRegion;
	OBJRGNNODE *ptNode;

	ptRegion = ptGame->ptObjRegion;
#line 374
	ptNode = (OBJRGNNODE *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(OBJRGNNODE));
	ptNode->nX = D2CommonUnitGetX(hPlayer);
	ptNode->nY = D2CommonUnitGetY(hPlayer);
	ptNode->hRoom = hRoom;
	ptNode->ptNext = ptRegion->ptNodes;
	ptRegion->ptNodes = ptNode;
}

void __fastcall ObjectRgnInitObject(OBJINIT *ptInit)
{
	OBJREGION *ptRegion;
	OBJRGNNODE *ptNode;
	GAMEROOM *hRoom;
	MISSOBJECTSTXT *ptObjectsTxt;

	ptRegion = ptInit->ptGame->ptObjRegion;
	for (ptNode = ptRegion->ptNodes; ptNode; ptNode = ptNode->ptNext)
	{
		hRoom = ptInit->hRoom;
		if (ptNode->hRoom == hRoom)
			goto lFound;
		if (hRoom && ptNode->nX >= hRoom->tRect.nX && ptNode->nX < hRoom->tRect.nX + hRoom->tRect.nWidth
			&& ptNode->nY >= hRoom->tRect.nY && ptNode->nY < hRoom->tRect.nY + hRoom->tRect.nHeight)
			goto lFound;
	}
	if (D2CommonDungeonRoomIsTown(ptInit->hRoom))
		D2CommonUnitSetMode(ptInit->hObject, 2);
	return;
lFound:
	if (!ptInit->hObject->dwMode)
	{
		D2CommonUnitSetMode(ptInit->hObject, 1);
		ptObjectsTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(ptInit->hObject->dwClassId);
		EventAdd(ptInit->ptGame, ptInit->hObject, 1,
			ptInit->ptGame->nFrame + ((DWORD)ptObjectsTxt->FrameCnt[1] >> 8));
	}
#line 410
	D2FREEPOOL(ptInit->ptGame->pMemPool, ptNode);
	ptRegion->ptNodes = NULL;
}

BYTE __fastcall ObjectRgnSpotsFull(GAME *ptGame, int nLevelId)
{
	OBJRGNLEVEL *ptLevel;

	ptLevel = ptGame->ptObjRegion->ptLevels[nLevelId];
	if (ptLevel->nSpots == 4 || ptLevel->nSpots > ptLevel->nRooms / 8)
		return TRUE;
	return FALSE;
}

BYTE __fastcall ObjectRgnLevelPopulated(GAME *ptGame, int nLevelId)
{
	OBJRGNLEVEL *ptLevel;

	ptLevel = ptGame->ptObjRegion->ptLevels[nLevelId];
	if (ptLevel->nRooms > 0 && ptLevel->nRoomsPopulated * 128 / ptLevel->nRooms > 96 && !ptLevel->field_10)
		return TRUE;
	return FALSE;
}

BYTE __fastcall ObjectRgnTestOrientedSpotsFull(GAME *ptGame, int nLevelId)
{
	OBJRGNLEVEL *ptLevel;

	ptLevel = ptGame->ptObjRegion->ptLevels[nLevelId];
	if (ptLevel->nSpots2 == 10 || ptLevel->nSpots2 > ptLevel->nRooms / 8)
		return TRUE;
	return FALSE;
}
void __fastcall ObjectRgnCount(GAME *ptGame, int nLevelId)
{
	ptGame->ptObjRegion->ptLevels[nLevelId]->field_10++;
}

BYTE __fastcall ObjectRgnSpotIsFar(GAME *ptGame, int nLevelId, int nX, int nY)
{
	OBJRGNLEVEL *ptLevel;
	int nDist;
	int i;

	ptLevel = ptGame->ptObjRegion->ptLevels[nLevelId];
	for (i = 0; i < ptLevel->nSpots; i++)
	{
		nDist = ptLevel->atSpots[i].nX - nX;
		if (nDist < 0)
			nDist = -nDist;
		if (nDist < 100)
			return FALSE;
		nDist = ptLevel->atSpots[i].nY - nY;
		if (nDist < 0)
			nDist = -nDist;
		if (nDist < 100)
			return FALSE;
	}
	return TRUE;
}
void __fastcall ObjectRgnAddSpot(GAME *ptGame, int nLevelId, int nX, int nY)
{
	OBJRGNLEVEL *ptLevel;

	ptLevel = ptGame->ptObjRegion->ptLevels[nLevelId];
	if (ptLevel->nSpots < 4)
	{
		ptLevel->atSpots[ptLevel->nSpots].nX = nX;
		ptLevel->atSpots[ptLevel->nSpots].nY = nY;
		ptLevel->nSpots++;
	}
}

BYTE __fastcall ObjectRgnIsOrientedSpotFar(GAME *ptGame, int nLevelId, int nX, int nY)
{
	OBJRGNLEVEL *ptLevel;
	int nDist;
	int i;

	ptLevel = ptGame->ptObjRegion->ptLevels[nLevelId];
	for (i = 0; i < ptLevel->nSpots2; i++)
	{
		nDist = ptLevel->atSpots2[i].nX - nX;
		if (nDist < 0)
			nDist = -nDist;
		if (nDist < 50)
			return FALSE;
		nDist = ptLevel->atSpots2[i].nY - nY;
		if (nDist < 0)
			nDist = -nDist;
		if (nDist < 50)
			return FALSE;
	}
	return TRUE;
}
void __fastcall ObjectRgnAddOrientedSpot(GAME *ptGame, int nLevelId, int nX, int nY)
{
	OBJRGNLEVEL *ptLevel;

	ptLevel = ptGame->ptObjRegion->ptLevels[nLevelId];
	if (ptLevel->nSpots2 < 10)
	{
		ptLevel->atSpots2[ptLevel->nSpots2].nX = nX;
		ptLevel->atSpots2[ptLevel->nSpots2].nY = nY;
		ptLevel->nSpots2++;
	}
}

int __fastcall ObjectRgnGetMonClass(OBJOPERATE *pObjectParam)
{
	int nActBase1;
	int nActBase2;
	int anBase[6];
	int *pnBase;
	OBJRGNLEVEL *ptLevel;
	MONREGION *ptMonRegion;
	GAME *ptGame;
	int nLevelId;
	int nSlots;
	int i, j;
	BYTE bAct;

	nLevelId = D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(pObjectParam->hObject));
	bAct = D2CommonGetActNum(nLevelId);
	ptGame = pObjectParam->ptGame;
	ptLevel = ptGame->ptObjRegion->ptLevels[nLevelId];
	if (ptLevel->nMonClass == OBJRGN_MONCLASS_UNSET)
	{
		nActBase1 = 5;
		nActBase2 = 96;
		anBase[0] = 0;
		anBase[1] = 170;
		anBase[2] = 274;
		anBase[3] = 379;
		anBase[4] = 383;
		anBase[5] = 387;
		pnBase = &nActBase1;
		if (bAct == 1)
			pnBase = &nActBase2;
		ptMonRegion = (MONREGION *)D2CommonMonsterRegionArrayGetDword(&ptGame->pMonRegions, nLevelId);
		ptLevel->nMonClass = OBJRGN_MONCLASS_DEFAULT;
		nSlots = ptMonRegion->bNumLevelSlots;
		for (i = 0; i < nSlots; i++)
		{
			for (j = 0; j < 1; j++)
			{
				if (ptMonRegion->atSlots[i].nClass >= pnBase[j] && ptMonRegion->atSlots[i].nClass < pnBase[j] + 5)
					return ptLevel->nMonClass = pnBase[j];
			}
			for (j = 0; j < 6; j++)
			{
				if (ptMonRegion->atSlots[i].nClass >= anBase[j] && ptMonRegion->atSlots[i].nClass < anBase[j] + 4)
					return ptLevel->nMonClass = anBase[j];
			}
		}
	}
	return ptLevel->nMonClass;
}

void __fastcall PartyScreenOnLoot(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg)
{
	GAMEPLAYERMSG tMsg;
	CLIENT *ptClient;
	DWORD dwUnitId;
	DWORD bType;
	int i;

	PlayerListAdd(ptGame, hUnit, hTarget);
	PlayerListAdd(ptGame, hTarget, hUnit);
	if (PlayerListTestFlag(hUnit, hTarget, 1) != dwArg)
		PlayerListSetFlag(ptGame, hUnit, hTarget, 1, dwArg);
	SCmdSendPlayerInfo(hUnit, hTarget);
	SCmdSendPlayerInfo(hTarget, hUnit);
	if (!dwArg)
		return;
	if (hUnit->dwClassId > 1)
		bType = 1;
	else
		bType = 2;
	dwUnitId = hUnit->dwId;
	ptClient = SUnitGetClient(hTarget);
	for (i = 0; i < (int)(sizeof(tMsg) / sizeof(DWORD)); i++)
		((DWORD *)&tMsg)[i] = 0;
	tMsg.bSubKind = (BYTE)bType;
	tMsg.bCmd = PARTY_MSG_CMD;
	tMsg.bEvent = PARTY_MSG_EVENT;
	tMsg.bColor = 3;
	tMsg.dwArg = dwUnitId;
	switch ((BYTE)bType)
	{
	case 1:
	case 2:
		tMsg.bColor = 4;
		break;
	case 3:
		tMsg.bColor = 8;
		break;
	case 4:
		tMsg.bColor = 0;
		break;
	case 5:
	case 7:
	case 8:
		tMsg.bColor = 2;
		break;
	case 6:
	case 9:
		tMsg.bColor = 9;
		break;
	}
	SCmdSendSystemMsg(ptClient, (SCMDSYSTEMMSG *)&tMsg);
}

void __fastcall PartyScreenOnHear(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg)
{
	PlayerListAdd(ptGame, hUnit, hTarget);
	PlayerListAdd(ptGame, hTarget, hUnit);
	if (PlayerListTestFlag(hUnit, hTarget, 2) != dwArg)
		PlayerListSetFlag(ptGame, hUnit, hTarget, 2, dwArg);
	SCmdSendPlayerInfo(hUnit, hTarget);
	SCmdSendPlayerInfo(hTarget, hUnit);
}

void __fastcall PartyScreenOnSquelch(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg)
{
	PlayerListAdd(ptGame, hUnit, hTarget);
	PlayerListAdd(ptGame, hTarget, hUnit);
	if (PlayerListTestFlag(hUnit, hTarget, 4) != dwArg)
		PlayerListSetFlag(ptGame, hUnit, hTarget, 4, dwArg);
	SCmdSendPlayerInfo(hUnit, hTarget);
	SCmdSendPlayerInfo(hTarget, hUnit);
}
void __fastcall PartyScreenOnAccept(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg)
{
	GAMEPLAYERMSG tMsg;
	PARTYJOINARG tArg;
	CLIENT *ptClient;
	DWORD dwUnitId;
	WORD wParty;
	int i;

	wParty = SUnitGetParty(hTarget);
	PlayerListAdd(ptGame, hUnit, hTarget);
	PlayerListAdd(ptGame, hTarget, hUnit);
	if (PlayerListGetState(hUnit, hTarget) != 2)
	{
		PlayerListSendState(hTarget, hUnit);
		PlayerListSendState(hUnit, hTarget);
		return;
	}
	if (wParty == PARTY_NONE)
	{
		wParty = PartyCreate(ptGame);
		PartyAddPlayer(ptGame, wParty, hTarget);
		PartyAddPlayer(ptGame, wParty, hUnit);
		PlayerListRefresh(ptGame, hTarget);
	}
	else
	{
		PartyAddPlayer(ptGame, wParty, hUnit);
	}
	PlayerListRefresh(ptGame, hUnit);
	dwUnitId = hTarget->dwId;
	ptClient = SUnitGetClient(hUnit);
	for (i = 0; i < (int)(sizeof(tMsg) / sizeof(DWORD)); i++)
		((DWORD *)&tMsg)[i] = 0;
	tMsg.dwArg = dwUnitId;
	tMsg.bCmd = PARTY_MSG_CMD;
	tMsg.bEvent = PARTY_MSG_EVENT;
	tMsg.bSubKind = 8;
	tMsg.bColor = 2;
	SCmdSendSystemMsg(ptClient, (SCMDSYSTEMMSG *)&tMsg);
	for (i = 0; i < (int)(sizeof(tArg) / sizeof(DWORD)); i++)
		((DWORD *)&tArg)[i] = 0;
	tArg.hTarget = hTarget;
	tArg.hUnit = hUnit;
	tArg.wParty = wParty;
	SUnitForEachPlayer(ptGame, PartyScreenJoinEach, &tArg);
}

void __fastcall PartyScreenJoinEach(GAME *ptGame, UNIT *hPlayer, void *pArg)
{
	PARTYJOINARG *ptArg;
	GAMEPLAYERMSG tMsg;
	CLIENT *ptClient;
	UNIT *hOther;
	DWORD dwUnitId;
	WORD wParty;
	int i;

	ptArg = (PARTYJOINARG *)pArg;
	wParty = SUnitGetParty(hPlayer);
	PlayerListAdd(ptGame, hPlayer, ptArg->hTarget);
	PlayerListAdd(ptGame, ptArg->hTarget, hPlayer);
	PlayerListAdd(ptGame, hPlayer, ptArg->hUnit);
	PlayerListAdd(ptGame, ptArg->hUnit, hPlayer);
	hOther = ptArg->hTarget;
	if (hPlayer->dwId != hOther->dwId)
	{
		if (SUnitGetParty(hPlayer) == PARTY_NONE)
		{
			if (PlayerListGetState(hOther, hPlayer) == 4)
			{
				PlayerListSetState(hPlayer, hOther, 2);
			}
			else
			{
				PlayerListSetState(hOther, hPlayer, 0);
				PlayerListSetState(hPlayer, hOther, 1);
			}
		}
		else
		{
			PlayerListSetState(hOther, hPlayer, 1);
			PlayerListSetState(hPlayer, hOther, 1);
		}
	}
	hOther = ptArg->hUnit;
	if (hPlayer->dwId != hOther->dwId)
	{
		if (SUnitGetParty(hPlayer) == PARTY_NONE)
		{
			if (PlayerListGetState(hOther, hPlayer) == 4)
			{
				PlayerListSetState(hPlayer, hOther, 2);
			}
			else
			{
				PlayerListSetState(hOther, hPlayer, 0);
				PlayerListSetState(hPlayer, hOther, 1);
			}
		}
		else
		{
			PlayerListSetState(hOther, hPlayer, 1);
			PlayerListSetState(hPlayer, hOther, 1);
		}
	}
	if (wParty == ptArg->wParty && wParty != PARTY_NONE && hPlayer != ptArg->hUnit)
	{
		dwUnitId = ptArg->hUnit->dwId;
		ptClient = SUnitGetClient(hPlayer);
		for (i = 0; i < (int)(sizeof(tMsg) / sizeof(DWORD)); i++)
			((DWORD *)&tMsg)[i] = 0;
		tMsg.bCmd = PARTY_MSG_CMD;
		tMsg.bEvent = PARTY_MSG_EVENT;
		tMsg.dwArg = dwUnitId;
		tMsg.bSubKind = 7;
		tMsg.bColor = 2;
		SCmdSendSystemMsg(ptClient, (SCMDSYSTEMMSG *)&tMsg);
	}
	PlayerListSendState(hPlayer, ptArg->hTarget);
	PlayerListSendState(hPlayer, ptArg->hUnit);
	SCmdSendAllParty(ptGame, hPlayer);
}
