#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\Player\\PlrTrade.cpp"
#include <windows.h>
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "../GAME/Game.h"
#include "../GAME/SCmd.h"
#include "../INVENTORY/InvMode.h"
#include "../ITEMS/ItemMode.h"
#include "../ITEMS/Items.h"
#include "../OBJECTS/ObjMode.h"
#include "../QUESTS/a1q4.h"
#include "../QUESTS/a2q3.h"
#include "../QUESTS/a2q4.h"
#include "../QUESTS/a3q3.h"
#include "Player.h"
#include "PlrSave.h"
#include "PlrTrade.h"
#include "../QUESTS/a4q3.h"
#include "../UNIT/SUnit.h"
#include "../GUILDS/Guilds.h"

#define UNIT_PLAYER			0
#define UNIT_OBJECT			2
#define UNIT_ITEM			4
#define STAT_GOLD			14
#define STAT_GOLDBANK		15
#define STAT_QUANTITY		70
#define INVPAGE_TRADE		2
#define INVPAGE_CUBE		3
#define OBJECT_GUILDSTONE	337
#define OBJECT_GUILDVAULT	338
#define OBJECT_GUILD153		339
#define OBJECT_GUILD154		340
#define TRADE_DATA_SIZE		8192

#pragma pack(push, 1)
struct PLRTRADEOPENPACKET
{
 	BYTE		bCmd;
 	char		szName[16];
 	DWORD		dwUnitId;
};
#pragma pack(pop)

CLIENT * __fastcall ClientGetNext(CLIENT *ptClient);
void __fastcall SCmdSendSystemMsg(CLIENT *ptClient, const SCMDSYSTEMMSG *pSystemMsg);
void __fastcall SCmdSendTradeEvent(CLIENT *ptClient, BYTE bValue);
void __fastcall SCmdSendTradeName(CLIENT *ptClient, const void *pPacket);
void __fastcall SCmdSendTradeGold(CLIENT *ptClient, DWORD dwGold,
	BYTE bOwnOffer);
void __fastcall SCmdSendGuildCounter(CLIENT *ptClient, BYTE bCounter,
	WORD wLength,
	const BYTE *pbData);

typedef unsigned __int64 (__stdcall *PFNGUILDSGETCOUNTER)(DWORD hGuildData);
typedef void (__fastcall *PFNGUILDEVENT)(BYTE *pbTag, BYTE bEvent,
	const char *pszName, DWORD dwValue);

int __fastcall PlrSaveOperateStash(OBJOPERATE *pObjectParam)
{
	void *hRoom;

	if (!PlayerIsBusy(pObjectParam->hPlayer) && pObjectParam->dwClassId == 267)
	{
		hRoom = (void *)D2CommonUnitGetPathRoom(pObjectParam->hPlayer);
		if (hRoom && D2CommonDungeonRoomIsTown(hRoom))
		{
			hRoom = (void *)D2CommonUnitGetPathRoom(pObjectParam->hObject);
			if (hRoom && D2CommonDungeonRoomIsTown(hRoom))
			{
				SUnitSetInteract(pObjectParam->hPlayer, 2, pObjectParam->hObject->dwId);
				SCmdSendTradeEvent(SUnitGetClient(pObjectParam->hPlayer), 14);
				ITEMSModeResetBookSkills(pObjectParam->ptGame, pObjectParam->hPlayer);
			}
		}
	}
	return 0;
}

static __inline void PlrSaveSendEventMsg(UNIT *hUnit, BYTE bEvent, char *szName)
{
	SCMDSYSTEMMSG tMsg;

	tMsg.bCmd = S2C_ROSTER_EVENT;
	tMsg.bEvent = bEvent;
	tMsg.bColor = 0;
	tMsg.dwArg = 0;
	if (szName)
	{
		StormSStrCopy(tMsg.bPlayerName, szName, 2147483647);
		tMsg.bPlayerName[sizeof(tMsg.bPlayerName) - 1] = 0;
	}
	else
		tMsg.bPlayerName[0] = 0;
	SCmdSendSystemMsg(SUnitGetClient(hUnit), &tMsg);
}

void __fastcall PlrSaveRequestTrade(GAME *ptGame, UNIT *hUnit, UNIT *hOther)
{
	PLRINFO *ptOtherInfo, *ptPlrInfo;
	DWORD dwTick;

	if (PlayerIsBusy(hUnit))
		return;
	if (A4Q3MonsterKilled(ptGame))
		return;
	dwTick = GetTickCount();
	ptOtherInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hOther);
	ptPlrInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
	if (PlayerIsBusy(hOther))
	{
		if (dwTick - ptPlrInfo->dwTradeMsgTick > 1000)
		{
			PlrSaveSendEventMsg(hUnit, 8, ptOtherInfo->szName);
			ptPlrInfo->dwTradeMsgTick = dwTick;
			return;
		}
	}
	else if (dwTick - ptOtherInfo->dwTradeEndTick < 5000)
	{
		if (ptPlrInfo->dwTradeMsgTick + 1000 < dwTick)
		{
			ptPlrInfo->dwTradeMsgTick = dwTick;
			PlrSaveSendEventMsg(hUnit, 9, ptOtherInfo->szName);
			return;
		}
	}
	else if (!ptPlrInfo->dwTradeState && !ptOtherInfo->dwTradeState)
	{
		if (dwTick - ptPlrInfo->dwTradeEndTick < 5000)
		{
			if (dwTick > ptPlrInfo->dwTradeMsgTick + 1000)
			{
				PlrSaveSendEventMsg(hUnit, 9, NULL);
				ptPlrInfo->dwTradeMsgTick = dwTick;
				return;
			}
		}
		else
		{
			SUnitSetInteract(hUnit, 0, hOther->dwId);
			SUnitSetInteract(hOther, 0, hUnit->dwId);
			SCmdSendTradeEvent(SUnitGetClient(hUnit), 0);
			SCmdSendTradeEvent(SUnitGetClient(hOther), 1);
			ptPlrInfo->dwTradeState = 1;
			ptOtherInfo->dwTradeState = 2;
			return;
		}
	}
}

static BOOL __fastcall PlrTradeCreate(GAME *ptGame, UNIT *hUnit)
{
	PLRINFO *ptPlayerInfo;
	PLRTRADE *hTrade;

	ptPlayerInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
	if (ptPlayerInfo->hTrade)
		return FALSE;
#line 1476
	hTrade = (PLRTRADE *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(PLRTRADE));
#line 1477
	hTrade->pbData = (BYTE *)D2ALLOCPOOL(ptGame->pMemPool, TRADE_DATA_SIZE);
	hTrade->nSize = PlrSaveWriteItems(hUnit, hTrade->pbData, TRADE_DATA_SIZE, FALSE);
	hTrade->ptItems = NULL;
	hTrade->dwGoldOffered = 0;
	hTrade->dwStartGold = D2CommonStatsUnitGetValue(hUnit, STAT_GOLD);
	ptPlayerInfo->hTrade = hTrade;
	return TRUE;
}

static void __fastcall PlrTradeFree(GAME *ptGame, UNIT *hUnit)
{
	PLRINFO *ptPlayerInfo;
	PLRTRADE *hTrade;
	PLRTRADEITEM *ptEntry, *ptNext;

	ptPlayerInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
	hTrade = ptPlayerInfo->hTrade;
	if (!hTrade)
		return;
#line 1505
	D2FREEPOOL(ptGame->pMemPool, hTrade->pbData);
	ptEntry = hTrade->ptItems;
	while (ptEntry)
	{
		ptNext = ptEntry->pNext;
		D2FREEPOOL(ptGame->pMemPool, ptEntry);
		ptEntry = ptNext;
	}
	hTrade->ptItems = NULL;
	D2FREEPOOL(ptGame->pMemPool, hTrade);
	ptPlayerInfo->hTrade = NULL;
}

static __inline void PlrTradeGuildVaultI(GAME *ptGame, UNIT *hUnit, WORD wCmd, DWORD dwId, DWORD dwArg)
{
	UNIT *hObject, *hPlayer;
	CLIENT *ptClient;
	DWORD dwGold, dwMax, dwAmount;
	void *hGuild;
	DWORD dwRest;

	dwAmount = dwArg;
	hGuild = GuildsGetData(ptGame);
	if (!hGuild)
		return;
	hObject = SUnitFind(ptGame, UNIT_OBJECT, dwId);
	if (!hObject)
		return;
	if (hObject->dwClassId != OBJECT_GUILDVAULT)
		return;
	switch (wCmd)
	{
	case 29:
		SUnitResetInteract(hUnit);
		ITEMSModeResetBookSkills(ptGame, hUnit);
		return;
	case 30:
	{
		unsigned __int64 qwCounter, qwAvail;

		dwMax = D2CommonUnitCalcGoldLimit(hUnit);
		dwGold = D2CommonStatsUnitGetValue(hUnit, STAT_GOLD);
		qwAvail = D2CommonGuildsGetCounterA(hGuild);
		if ((unsigned __int64)dwArg > qwAvail)
			dwAmount = (DWORD)qwAvail;
		if (dwGold >= dwMax)
			return;
		if (dwAmount > dwMax - dwGold)
			dwAmount = dwMax - dwGold;
		D2CommonGuildsSubtractCounterA(hGuild, dwAmount);
		qwCounter = D2CommonGuildsGetCounterA(hGuild);
		for (ptClient = ClientsGetFirst(ptGame); ptClient; ptClient = ClientGetNext(ptClient))
		{
			hPlayer = ClientGetPlayerUnit(ptClient, FALSE);
			if (hPlayer && hObject == SUnitGetInteractUnit(ptGame, hPlayer))
				SCmdSendGuildCounter(ptClient, 0, sizeof(qwCounter), (const BYTE *)&qwCounter);
		}
		ITEMSAddGold(ptGame, hUnit, dwAmount);
		return;
	}
	case 31:
	{
		unsigned __int64 qwCounter;

		dwGold = D2CommonStatsUnitGetValue(hUnit, STAT_GOLD);
		if (dwArg > dwGold + (DWORD)D2CommonStatsUnitGetValue(hUnit, STAT_GOLDBANK))
			return;
		dwRest = 0;
		if (dwArg > dwGold)
			dwRest = dwArg - dwGold;
		if ((int)dwArg < (int)dwGold)
			dwGold = dwArg;
		D2CommonStatsUnitAddValue(hUnit, STAT_GOLD, -dwGold);
		if (dwRest)
			D2CommonStatsUnitAddValue(hUnit, STAT_GOLDBANK, -dwRest);
		D2CommonGuildsAddCounterA(hGuild, dwArg);
		qwCounter = D2CommonGuildsGetCounterA(hGuild);
		for (ptClient = ClientsGetFirst(ptGame); ptClient; ptClient = ClientGetNext(ptClient))
		{
			hPlayer = ClientGetPlayerUnit(ptClient, FALSE);
			if (hPlayer && hObject == SUnitGetInteractUnit(ptGame, hPlayer))
				SCmdSendGuildCounter(ptClient, 0, sizeof(qwCounter), (const BYTE *)&qwCounter);
		}
		return;
	}
	}
	return;
}

void __fastcall PlrTradeCommand(GAME *ptGame, UNIT *hUnit, WORD wCmd,
	DWORD dwArg)
{
	DWORD dwType, dwId;
	UNIT *hOther, *hPlayer;
	PLRINFO *ptPlayerInfo1, *ptPlayerInfo2;
	CLIENT *ptClient;
	void *hRoom;
	DWORD dwGold, dwMax, dwAmount, dwTick;

	if (!SUnitGetInteract(hUnit, &dwType, &dwId))
	{
		SCmdSendTradeEvent(SUnitGetClient(hUnit), 12);
		return;
	}
	if (wCmd >= 16 && wCmd <= 18)
	{
		UNIT *hObject;
		DWORD dwBank;

		if (dwType != UNIT_OBJECT)
			return;
		hObject = SUnitFind(ptGame, UNIT_OBJECT, dwId);
		if (hObject->dwClassId != OBJECT_BANK)
			return;
		hRoom = (void *)D2CommonUnitGetPathRoom(hUnit);
		if (!hRoom || !D2CommonDungeonRoomIsTown(hRoom))
			return;
		hRoom = (void *)D2CommonUnitGetPathRoom(hObject);
		if (!hRoom || !D2CommonDungeonRoomIsTown(hRoom))
			return;
		switch (wCmd)
		{
		case 16:
			SUnitResetInteract(hUnit);
			ITEMSModeResetBookSkills(ptGame, hUnit);
			return;
		case 17:
			if (dwArg > (DWORD)D2CommonStatsUnitGetValue(hUnit, STAT_GOLDBANK))
				return;
			ITEMSAddGold(ptGame, hUnit, dwArg);
			D2CommonStatsUnitAddValue(hUnit, STAT_GOLDBANK, -dwArg);
			return;
		case 18:
			if (dwArg > (DWORD)D2CommonStatsUnitGetValue(hUnit, STAT_GOLD))
				return;
			dwBank = D2CommonStatsUnitGetValue(hUnit, STAT_GOLDBANK);
			dwMax = D2CommonUnitCalcGoldBankLimit(hUnit);
			if (dwBank + dwArg > dwMax)
			{
				if (dwBank >= dwMax)
					return;
				D2CommonStatsUnitAddValue(hUnit, STAT_GOLDBANK, dwMax - dwBank);
				D2CommonStatsUnitAddValue(hUnit, STAT_GOLD, dwBank - dwMax);
				return;
			}
			D2CommonStatsUnitAddValue(hUnit, STAT_GOLD, -dwArg);
			D2CommonStatsUnitAddValue(hUnit, STAT_GOLDBANK, dwArg);
			return;
		}
		return;
	}
	if (wCmd >= 21 && wCmd <= 22)
	{
		if (dwType != UNIT_ITEM)
			return;
		switch (wCmd)
		{
		case 21:
			SUnitResetInteract(hUnit);
			ITEMSModeResetBookSkills(ptGame, hUnit);
			return;
		case 22:
			PlrTradeTransmute(ptGame, hUnit);
			return;
		}
		return;
	}
	if (wCmd >= 25 && wCmd <= 26)
	{
		UNIT *hObject;

		if (dwType != UNIT_OBJECT)
			return;
		hObject = SUnitFind(ptGame, UNIT_OBJECT, dwId);
		if (!hObject)
			return;
		if (hObject->dwClassId != OBJECT_GUILDSTONE)
			return;
		switch (wCmd)
		{
		case 25:
			SUnitResetInteract(hUnit);
			ITEMSModeResetBookSkills(ptGame, hUnit);
			ObjectModeGuildStoneRelease(ptGame, hUnit, hObject);
			return;
		case 26:
		{
			void *hGuild;
			unsigned __int64 qwCounter;
			DWORD dwBank, dwRest, dwResult, dwRank, dwRankNew;

			dwGold = D2CommonStatsUnitGetValue(hUnit, STAT_GOLD);
			dwBank = D2CommonStatsUnitGetValue(hUnit, STAT_GOLDBANK);
			hGuild = GuildsGetData(ptGame);
			if (!hGuild)
				return;
			if (dwArg > dwBank + dwGold)
				return;
			dwRest = 0;
			if (dwArg > dwGold)
				dwRest = dwArg - dwGold;
			if ((int)dwArg < (int)dwGold)
				dwGold = dwArg;
			D2CommonStatsUnitAddValue(hUnit, STAT_GOLD, -dwGold);
			if (dwRest)
				D2CommonStatsUnitAddValue(hUnit, STAT_GOLDBANK, -dwRest);
			dwRank = D2CommonGuildsGetRankLevelId(hGuild);
			dwResult = D2CommonGuildsDonateGold(hGuild, D2CommonUnitGetPlayerName(hUnit), dwArg);
			dwRankNew = D2CommonGuildsGetRankLevelId(hGuild);
			qwCounter = D2CommonGuildsGetCounterB(hGuild);
			for (ptClient = ClientsGetFirst(ptGame); ptClient; ptClient = ClientGetNext(ptClient))
			{
				hPlayer = ClientGetPlayerUnit(ptClient, FALSE);
				if (hPlayer && hObject == SUnitGetInteractUnit(ptGame, hPlayer))
					SCmdSendGuildCounter(ptClient, 1, sizeof(qwCounter), (const BYTE *)&qwCounter);
			}
			if (dwRank != dwRankNew)
				GuildsSpawnRankObject(ptGame, (int)dwRank, (BYTE)dwRankNew);
			if (!ptEventCallbackTable->pfnGuildEvent)
				return;
			((PFNGUILDEVENT)ptEventCallbackTable->pfnGuildEvent)(D2CommonGuildsGetTag(hGuild), 4,
				D2CommonUnitGetPlayerName(hUnit), dwResult);
			return;
		}
		}
		return;
	}
	if (wCmd >= 29 && wCmd <= 31)
	{
		if (dwType != UNIT_OBJECT)
			return;
		PlrTradeGuildVaultI(ptGame, hUnit, wCmd, dwId, dwArg);
		return;
	}
	if (wCmd >= 34 && wCmd <= 34)
	{
		if (dwType != UNIT_OBJECT)
			return;
		if (SUnitFind(ptGame, UNIT_OBJECT, dwId)->dwClassId != OBJECT_GUILD153)
			return;
		switch (wCmd)
		{
		case 34:
			SUnitResetInteract(hUnit);
			ITEMSModeResetBookSkills(ptGame, hUnit);
			return;
		}
		return;
	}
	if (wCmd >= 37 && wCmd <= 37)
	{
		if (dwType != UNIT_OBJECT)
			return;
		if (SUnitFind(ptGame, UNIT_OBJECT, dwId)->dwClassId != OBJECT_GUILD154)
			return;
		switch (wCmd)
		{
		case 37:
			SUnitResetInteract(hUnit);
			ITEMSModeResetBookSkills(ptGame, hUnit);
			return;
		}
		return;
	}

	if (dwType != UNIT_PLAYER)
	{
		SCmdSendTradeEvent(SUnitGetClient(hUnit), 13);
		return;
	}
	hOther = SUnitFind(ptGame, UNIT_PLAYER, dwId);
	if (hOther)
	{
		ptPlayerInfo1 = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
		ptPlayerInfo2 = (PLRINFO *)D2CommonUnitGetPlrInfo(hOther);
		switch (wCmd)
		{
		case 2:
			if ((ptPlayerInfo1->dwTradeState == 2 || ptPlayerInfo1->dwTradeState == 1)
				&& (ptPlayerInfo2->dwTradeState == 2 || ptPlayerInfo2->dwTradeState == 1))
			{
				PlrTradeCancel(ptGame, hUnit, hOther, ptPlayerInfo1, ptPlayerInfo2);
				dwTick = GetTickCount();
				ptPlayerInfo2->dwTradeEndTick = dwTick;
				ptPlayerInfo1->dwTradeEndTick = dwTick;
				return;
			}
			if (INVENTORYModeHasItems(ptGame, hUnit))
				return;
			if (INVENTORYModeHasItems(ptGame, hOther))
				return;
			PlrTradeCancel(ptGame, hUnit, hOther, ptPlayerInfo1, ptPlayerInfo2);
			dwTick = GetTickCount();
			ptPlayerInfo2->dwTradeEndTick = dwTick;
			ptPlayerInfo1->dwTradeEndTick = dwTick;
			return;
		case 3:
			if (!INVENTORYModeHasItems(ptGame, hUnit) && !INVENTORYModeHasItems(ptGame, hOther))
			{
				PLRTRADEOPENPACKET tPacket;

				if (A4Q3MonsterKilled(ptGame))
				{
					PlrTradeCancel(ptGame, hUnit, hOther, ptPlayerInfo1, ptPlayerInfo2);
					return;
				}
				if (ptPlayerInfo1->dwTradeState != 2 || ptPlayerInfo2->dwTradeState != 1)
				{
					PlrTradeCancel(ptGame, hUnit, hOther, ptPlayerInfo1, ptPlayerInfo2);
					return;
				}
				SCmdSendTradeEvent(SUnitGetClient(hUnit), 6);
				SCmdSendTradeEvent(SUnitGetClient(hOther), 6);
				ptPlayerInfo1->dwTradeState = 3;
				ptPlayerInfo2->dwTradeState = 3;
				if (!PlrTradeCreate(ptGame, hUnit))
				{
					SCmdSendTradeEvent(SUnitGetClient(hUnit), 12);
					SCmdSendTradeEvent(SUnitGetClient(hOther), 12);
					return;
				}
				if (!PlrTradeCreate(ptGame, hOther))
				{
					PlrTradeFree(ptGame, hUnit);
					SCmdSendTradeEvent(SUnitGetClient(hUnit), 12);
					SCmdSendTradeEvent(SUnitGetClient(hOther), 12);
					return;
				}
				tPacket.bCmd = 107;
				memcpy(tPacket.szName, ClientGetName(SUnitGetClient(hOther)), sizeof(tPacket.szName));
				tPacket.szName[15] = '\0';
				tPacket.dwUnitId = hOther->dwId;
				SCmdSendTradeName(SUnitGetClient(hUnit), &tPacket);
				memcpy(tPacket.szName, ClientGetName(SUnitGetClient(hUnit)), sizeof(tPacket.szName));
				tPacket.szName[15] = '\0';
				tPacket.dwUnitId = hUnit->dwId;
				SCmdSendTradeName(SUnitGetClient(hOther), &tPacket);
				return;
			}
			PlrTradeCancel(ptGame, hUnit, hOther, ptPlayerInfo1, ptPlayerInfo2);
			return;
		case 8:
			if (dwArg > (DWORD)D2CommonStatsUnitGetValue(hUnit, STAT_GOLD))
			{
				if (INVENTORYModeHasItems(ptGame, hUnit))
					return;
				if (INVENTORYModeHasItems(ptGame, hOther))
					return;
				PlrTradeCancel(ptGame, hUnit, hOther, ptPlayerInfo1, ptPlayerInfo2);
				return;
			}
			dwMax = D2CommonUnitCalcGoldLimit(hOther);
			dwGold = D2CommonStatsUnitGetValue(hOther, STAT_GOLD);
			dwAmount = dwArg;
			if (dwGold + dwArg > dwMax)
				dwAmount = dwMax - dwGold;
			ptPlayerInfo1->hTrade->dwGoldOffered = dwAmount;
			SCmdSendTradeGold(SUnitGetClient(hOther), dwAmount, 0);
			SCmdSendTradeGold(SUnitGetClient(hUnit), dwAmount, 1);
			return;
		case 4:
			if (INVENTORYModeHasItems(ptGame, hUnit) || INVENTORYModeHasItems(ptGame, hOther))
			{
				SCmdSendTradeEvent(SUnitGetClient(hUnit), 6);
				return;
			}
			if (A4Q3MonsterKilled(ptGame))
			{
				PlrTradeCancel(ptGame, hUnit, hOther, ptPlayerInfo1, ptPlayerInfo2);
				return;
			}
			SCmdSendTradeEvent(SUnitGetClient(hOther), 5);
			if (ptPlayerInfo1->dwTradeState != 3)
				return;
			ptPlayerInfo1->dwTradeState = 4;
			if (ptPlayerInfo2->dwTradeState != 4)
				return;
			PlrTradeCommit(ptGame, hUnit, hOther, ptPlayerInfo1, ptPlayerInfo2);
			dwTick = GetTickCount();
			ptPlayerInfo2->dwTradeEndTick = dwTick;
			ptPlayerInfo1->dwTradeEndTick = dwTick;
			return;
		case 7:
			if (ptPlayerInfo1->dwTradeState == 4)
				ptPlayerInfo1->dwTradeState = 3;
			if (ptPlayerInfo2->dwTradeState == 4)
				ptPlayerInfo2->dwTradeState = 3;
			SCmdSendTradeEvent(SUnitGetClient(hUnit), 6);
			SCmdSendTradeEvent(SUnitGetClient(hOther), 6);
			return;
		}
		return;
	}
	if (!INVENTORYModeHasItems(ptGame, hUnit))
		SCmdSendTradeEvent(SUnitGetClient(hUnit), 12);
}

void __fastcall PlrTradeTransmute(GAME *ptGame, UNIT *hUnit)
{
	CLIENT *ptClient;
	void *ptNode, *ptNext;
	UNIT *hItem;
	ITEMSTXT *ptItemStats;
	ITEMSTXT *ptItem;
	BOOL bAdded;
	DWORD dwCode;
	int nClassId, nQuality, nItemLevel, nPrefix, nSuffix, nMagicClassId;
	BOOL fFlag800, fNoItem;
	int nGems, nRemoved, nQuestParts;
	int nAmethyst, nGcv, nGfv, nGsv, nGzv, nGpv;
	int nTopaz, nGcy, nGfy, nGsy, nGly, nGpy;
	int nSapphire, nGcb, nGfb, nGsb, nGlb, nGpb;
	int nEmerald, nGcg, nGfg, nGsg, nGlg, nGpg;
	int nRuby, nGcr, nGfr, nGsr, nGlr, nGpr;
	int nDiamond, nGcw, nGfw, nGsw, nGlw, nGpw;
	int nSkull, nSkc, nSkf, nSku, nSkl, nSkz;
	int nQf1, nQey, nQbr, nQhr, nMsf, nVip, nHp, nMp, nSpc, nLeg, nTbk, nKri;
	int nRin, nAmu, nAqv, nCqv, nYps, nWm, nSml, nGplQuantity, nOpmQuantity;
	int nMagic, nRare, nQuestItems;
	int nType0c, nType34, nType8, nType0d, nType12, nType12Tbl, nType13;

	ptClient = SUnitGetClient(hUnit);
	dwCode = 0;
	nClassId = -1;
	nQuality = 2;
	nItemLevel = ITEMSGetUnitLevel(hUnit, LEVEL_ID_NONE) + 4;
	nPrefix = -1;
	nSuffix = -1;
	fFlag800 = FALSE;
	fNoItem = FALSE;
	nMagicClassId = -1;
	nMagic = 0; nRare = 0; nType34 = 0; nType8 = 0; nType12 = 0; nType12Tbl = 0; nType0d = 0; nType0c = 0; nSml = 0; nSpc = 0; nKri = 0;
	nAmethyst = 0; nGcv = 0; nGfv = 0; nGsv = 0; nGzv = 0; nGpv = 0;
	nRuby = 0; nGcr = 0; nGfr = 0; nGsr = 0; nGlr = 0; nGpr = 0;
	nSapphire = 0; nGcb = 0; nGfb = 0; nGsb = 0; nGlb = 0; nGpb = 0;
	nTopaz = 0; nGcy = 0; nGfy = 0; nGsy = 0; nGly = 0; nGpy = 0;
	nEmerald = 0; nGcg = 0; nGfg = 0; nGsg = 0; nGlg = 0; nGpg = 0;
	nDiamond = 0; nGcw = 0; nGfw = 0; nGsw = 0; nGlw = 0; nGpw = 0;
	nSkull = 0; nSkc = 0; nSkf = 0; nSku = 0; nSkl = 0; nSkz = 0;
	nGplQuantity = 0; nOpmQuantity = 0; nHp = 0; nMp = 0; nYps = 0; nWm = 0; nRin = 0; nAmu = 0;
	nAqv = 0; nCqv = 0; nLeg = 0; nTbk = 0; nType13 = 0; nVip = 0; nMsf = 0; nQuestItems = 0; nQf1 = 0; nQhr = 0; nQey = 0; nQbr = 0;
	nGems = 0;

	for (ptNode = D2CommonINVENTORYGetCategoryListHead(hUnit->ptInventory); ptNode;
		ptNode = (void *)D2CommonINVENTORYNodeGetNext(ptNode))
	{
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
		if (D2CommonITEMSDataGetPage(hItem) != INVPAGE_CUBE)
			continue;
		ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
#line 311
		D2ASSERT(ptItemStats);
		switch (D2CommonITEMSDataGetModType(hItem))
		{
		case 4:
			nMagic++;
			nMagicClassId = hItem->dwClassId;
			break;
		case 6:
			nRare++;
			break;
		}
		if (ptItemStats->bWeaponGroup == 12)
			nType0c++;
		else if (ptItemStats->bWeaponGroup == 3 || ptItemStats->bWeaponGroup == 4)
			nType34++;
		else if (ptItemStats->bWeaponGroup == 8)
			nType8++;
		else if (ptItemStats->bWeaponGroup == 13)
			nType0d++;
		else if (ptItemStats->bWeaponGroup == 1 || ptItemStats->bWeaponGroup == 2)
		{
			nType12++;
			if (D2CommonITEMSTestModTypeMagicOrAbove(hItem))
				nType12Tbl++;
		}
		switch (ptItemStats->code)
		{
		case 540108401:
			nQf1++;
			break;
		case 540110957:
		case 540176493:
		case 540242029:
		case 540307565:
		case 540373101:
			nMp++;
			break;
		case 540110952:
		case 540176488:
		case 540242024:
		case 540307560:
		case 540373096:
			nHp++;
			break;
		case 544629607:
			nAmethyst++; nGems++; nGcv++;
			break;
		case 544630375:
			nAmethyst++; nGems++; nGfv++;
			break;
		case 544633703:
			nAmethyst++; nGems++; nGsv++;
			break;
		case 544635495:
			nAmethyst++; nGems++; nGzv++;
			break;
		case 544632935:
			nAmethyst++; nGems++; nGpv++;
			break;
		case 544826215:
			nTopaz++; nGems++; nGcy++;
			break;
		case 544826983:
			nTopaz++; nGems++; nGfy++;
			break;
		case 544830311:
			nTopaz++; nGems++; nGsy++;
			break;
		case 544828519:
			nTopaz++; nGems++; nGly++;
			break;
		case 544829543:
			nTopaz++; nGems++; nGpy++;
			break;
		case 543318887:
			nSapphire++; nGems++; nGcb++;
			break;
		case 543319655:
			nSapphire++; nGems++; nGfb++;
			break;
		case 543322983:
			nSapphire++; nGems++; nGsb++;
			break;
		case 543321191:
			nSapphire++; nGems++; nGlb++;
			break;
		case 543322215:
			nSapphire++; nGems++; nGpb++;
			break;
		case 543646567:
			nEmerald++; nGems++; nGcg++;
			break;
		case 543647335:
			nEmerald++; nGems++; nGfg++;
			break;
		case 543650663:
			nEmerald++; nGems++; nGsg++;
			break;
		case 543648871:
			nEmerald++; nGems++; nGlg++;
			break;
		case 543649895:
			nEmerald++; nGems++; nGpg++;
			break;
		case 544367463:
			nRuby++; nGems++; nGcr++;
			break;
		case 544368231:
			nRuby++; nGems++; nGfr++;
			break;
		case 544371559:
			nRuby++; nGems++; nGsr++;
			break;
		case 544369767:
			nRuby++; nGems++; nGlr++;
			break;
		case 544370791:
			nRuby++; nGems++; nGpr++;
			break;
		case 544695143:
			nDiamond++; nGems++; nGcw++;
			break;
		case 544695911:
			nDiamond++; nGems++; nGfw++;
			break;
		case 544699239:
			nDiamond++; nGems++; nGsw++;
			break;
		case 544697447:
			nDiamond++; nGems++; nGlw++;
			break;
		case 544698471:
			nDiamond++; nGems++; nGpw++;
			break;
		case 543386483:
			nSkull++; nGems++; nSkc++;
			break;
		case 543583091:
			nSkull++; nGems++; nSkf++;
			break;
		case 544566131:
			nSkull++; nGems++; nSku++;
			break;
		case 543976307:
			nSkull++; nGems++; nSkl++;
			break;
		case 544893811:
			nSkull++; nGems++; nSkz++;
			break;
		case 543585133:
			nMsf++;
			break;
		case 543387763:
			nSpc++;
			break;
		case 543647084:
			nLeg++;
			break;
		case 543908468:
			nTbk++;
			break;
		case 543781483:
			nKri++;
			break;
		case 543977575:
			nGplQuantity += (DWORD)D2CommonStatsUnitGetValue(hItem, STAT_QUANTITY);
			break;
		case 543976819:
			if (D2CommonITEMSTestModTypeMagicOrAbove(hItem))
				nSml++;
			break;
		case 544043119:
			nOpmQuantity += (DWORD)D2CommonStatsUnitGetValue(hItem, STAT_QUANTITY);
			break;
		case 544237942:
			nVip++;
			break;
		case 544106866:
			nRin++;
			break;
		case 544367217:
			nQbr++;
			break;
		case 544368753:
			nQhr++;
			break;
		case 544436345:
			nYps++;
			break;
		case 543976823:
		case 544435575:
			nWm++;
			break;
		case 544566625:
			nAmu++;
			break;
		case 544633185:
			nAqv++;
			break;
		case 544633187:
			nCqv++;
			break;
		case 544826737:
			nQey++;
			break;
		default:
			if (ptItemStats->quest)
				nQuestItems++;
			break;
		}
		if (D2CommonITEMSGetType(hItem) == 19)
			nType13++;
	}

	if (nMsf + nVip == 1)
		return;
	nQuestParts = nQey + nQbr + nQhr + nQf1;
	if (nQuestParts != 0 && nQuestParts < 4)
		return;
	if (nQuestItems > 0)
		return;

	if (nHp >= 3 && nMp >= 3 && nGems >= 1)
		dwCode = 543979122;
	else if (nHp >= 3 && nMp >= 3)
		dwCode = 544437874;
	else if (nGpv >= 1 && nGpr >= 1 && nGpb >= 1 && nGpy >= 1 && nGpg >= 1 && nGpw >= 1 && nAmu >= 1)
	{
		dwCode = 544566625;
		nQuality = 4;
		nPrefix = 63;
		nItemLevel = 100;
	}
	else if (nSkz + nGpw + nGpg + nGpy + nGpb + nGpr + nGpv >= 3 && nMagic >= 1)
	{
		nItemLevel = 100;
		nClassId = nMagicClassId;
		nQuality = 4;
	}
	else if (nRin >= 1 && nRuby >= 1 && nOpmQuantity >= 4)
	{
		dwCode = 544106866;
		nQuality = 4;
		nPrefix = 77;
		nItemLevel = 100;
	}
	else if (nRin >= 1 && nSapphire >= 1 && nWm >= 4)
	{
		dwCode = 544106866;
		nQuality = 4;
		nPrefix = 68;
		nItemLevel = 100;
	}
	else if (nRin >= 1 && nTopaz >= 2)
	{
		dwCode = 544106866;
		nQuality = 4;
		nPrefix = 86;
		nItemLevel = 100;
	}
	else if (nRin >= 1 && nEmerald >= 1 && nYps >= 4)
	{
		dwCode = 544106866;
		nQuality = 4;
		nPrefix = 95;
		nItemLevel = 100;
	}
	else if (nType34 >= 1 && nType0c >= 2)
		dwCode = 544760180;
	else if (nType8 >= 1 && nAqv >= 1)
		dwCode = 544629098;
	else if (nRin >= 3)
	{
		dwCode = 544566625;
		nQuality = 4;
		nItemLevel = nItemLevel * 3 / 4;
	}
	else if (nAmu >= 3)
	{
		dwCode = 544106866;
		nQuality = 4;
		nItemLevel = nItemLevel * 3 / 4;
	}
	else if (nLeg >= 1 && nTbk >= 1)
	{
		if (!A1Q4CastAtSpot(ptGame, hUnit))
			return;
		fNoItem = TRUE;
	}
	else if (nGems >= 6 && nType12 >= 1)
	{
		dwCode = 543454060;
		fFlag800 = TRUE;
	}
	else if (nSml >= 1 && nSpc >= 1 && nSkull >= 2)
	{
		dwCode = 543976819;
		nQuality = 4;
		nSuffix = 14;
		nItemLevel = 100;
	}
	else if (nDiamond >= 1 && nKri >= 1 && nType0d >= 1 && nType13 >= 1)
	{
		dwCode = 544366946;
		nQuality = 4;
		nPrefix = 17;
		nItemLevel = 100;
	}
	else if (nGplQuantity >= 1 && nHp >= 1)
		dwCode = 544436345;
	else if (nAqv >= 2)
		dwCode = 544633187;
	else if (nCqv >= 2)
		dwCode = 544633185;
	else if (nMsf >= 1 && nVip >= 1)
	{
		if (A2Q3NoStaff(ptGame, hUnit))
		{
			dwCode = 544502632;
			A2Q4StaffMade(ptGame, hUnit);
			nQuality = 7;
		}
		else
		{
			SUnitSetEvent(hUnit, 19, hUnit);
			return;
		}
	}
	else if (nQf1 == 1 && nQhr == 1 && nQey == 1 && nQbr == 1)
	{
		if (A3Q2NoFlail(ptGame, hUnit))
		{
			dwCode = 540173937;
			A3Q2FlailMade(ptGame, hUnit);
			nQuality = 7;
		}
		else
		{
			SUnitSetEvent(hUnit, 19, hUnit);
			return;
		}
	}
	else if (nGcv >= 3)
		dwCode = 544630375;
	else if (nGfv >= 3)
		dwCode = 544633703;
	else if (nGsv >= 3)
		dwCode = 544635495;
	else if (nGzv >= 3)
		dwCode = 544632935;
	else if (nGcr >= 3)
		dwCode = 544368231;
	else if (nGfr >= 3)
		dwCode = 544371559;
	else if (nGsr >= 3)
		dwCode = 544369767;
	else if (nGlr >= 3)
		dwCode = 544370791;
	else if (nGcb >= 3)
		dwCode = 543319655;
	else if (nGfb >= 3)
		dwCode = 543322983;
	else if (nGsb >= 3)
		dwCode = 543321191;
	else if (nGlb >= 3)
		dwCode = 543322215;
	else if (nGcy >= 3)
		dwCode = 544826983;
	else if (nGfy >= 3)
		dwCode = 544830311;
	else if (nGsy >= 3)
		dwCode = 544828519;
	else if (nGly >= 3)
		dwCode = 544829543;
	else if (nGcg >= 3)
		dwCode = 543647335;
	else if (nGfg >= 3)
		dwCode = 543650663;
	else if (nGsg >= 3)
		dwCode = 543648871;
	else if (nGlg >= 3)
		dwCode = 543649895;
	else if (nGcw >= 3)
		dwCode = 544695911;
	else if (nGfw >= 3)
		dwCode = 544699239;
	else if (nGsw >= 3)
		dwCode = 544697447;
	else if (nGlw >= 3)
		dwCode = 544698471;
	else if (nSkc >= 3)
		dwCode = 543583091;
	else if (nSkf >= 3)
		dwCode = 544566131;
	else if (nSku >= 3)
		dwCode = 543976307;
	else if (nSkl >= 3)
		dwCode = 544893811;

	if (nClassId == -1 && !dwCode && !fNoItem)
		return;
	nRemoved = 0;
	for (ptNode = D2CommonINVENTORYGetCategoryListHead(hUnit->ptInventory); ptNode; ptNode = ptNext)
	{
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
		ptNext = (void *)D2CommonINVENTORYNodeGetNext(ptNode);
		if (D2CommonITEMSDataGetPage(hItem) == INVPAGE_CUBE)
		{
			DWORD dwFlags;

			dwFlags = 0;
			D2CommonITEMSSetOrClearFlags(&dwFlags, 32, TRUE);
			D2CommonITEMSDataSetPreviousPage(hItem, INVPAGE_CUBE);
			SCmdSendItemFromGrid(ptClient, hUnit, hItem, dwFlags);
			ITEMSModeRemoveGridItem(ptGame, hUnit, hItem, FALSE);
			nRemoved++;
		}
	}
	if (!fNoItem)
	{
		if (nClassId == -1)
		{
			ptItem = (ITEMSTXT *)D2CommonItemTblsFindItemByCode(dwCode, &nClassId);
#line 1105
			D2ASSERT(ptItem != NULL);
		}
		ITEMSSetForcedPrefix(nPrefix);
		ITEMSSetForcedSuffix(nSuffix);
		hItem = ITEMSCreate(hUnit, nClassId, ptGame, 4, nQuality, 0, nItemLevel, FALSE, 0, 0);
		D2CommonITEMSDataSetPage(hItem, INVPAGE_CUBE);
		bAdded = ITEMSModePlaceCursorItem(ptGame, hUnit, hItem->dwId, 0, 0, TRUE, TRUE, NULL);
#line 1115
		D2ASSERT(bAdded);
		ITEMSSetForcedPrefix(-1);
		ITEMSSetForcedSuffix(-1);
		D2CommonITEMSDataSetFlags(hItem, 16, TRUE);
		if (fFlag800)
			D2CommonITEMSDataSetFlags(hItem, 2048, TRUE);
	}
	if (nRemoved)
		SUnitSetEvent(hUnit, 4, hUnit);
}

void __fastcall PlrTradeCancel(GAME *ptGame, UNIT *hUnit, UNIT *hOther,
	PLRINFO *ptPlayerInfo1, PLRINFO *ptPlayerInfo2)
{
	DWORD dwState;

	D2CommonUnitSetChanged(hUnit, TRUE);
	D2CommonUnitSetChanged(hOther, TRUE);
	hUnit->dwFlags |= 8388608;
	hOther->dwFlags |= 8388608;
	SUnitResetInteract(hUnit);
	SUnitResetInteract(hOther);
	PlrTradeRestoreItems(ptGame, hUnit, TRUE);
	PlrTradeRestoreItems(ptGame, hOther, TRUE);
	PlrTradeFree(ptGame, hUnit);
	PlrTradeFree(ptGame, hOther);
	dwState = 9;
	ptPlayerInfo1->dwTradeState = dwState;
	ptPlayerInfo2->dwTradeState = dwState;
	ITEMSModeResetBookSkills(ptGame, hUnit);
	ITEMSModeResetBookSkills(ptGame, hOther);
	SCmdSendTradeEvent(SUnitGetClient(hUnit), 12);
	SCmdSendTradeEvent(SUnitGetClient(hOther), 12);
}

void __fastcall PlrTradeRestoreItems(GAME *ptGame, UNIT *hUnit, BOOL fUnused)
{
	PLRINFO *ptPlayerInfo;
	PLRTRADE *hTrade;
	int nRead;
	int i;

	ptPlayerInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
	if (!ptPlayerInfo->hTrade)
		return;
	SCmdSendUnitTypeId(SUnitGetClient(hUnit), 133, 0, hUnit->dwId);
	SUnitForEachPlayer(ptGame, PlrTradeSendUnitGone, hUnit);
	ITEMSModeUnequipAll(ptGame, hUnit);
	for (i = 1; i < 11; i++)
		D2CommonINVENTORYClearLocSlot(hUnit->ptInventory, i);
	hTrade = ptPlayerInfo->hTrade;
	PlrSaveReadItems(ptGame, hUnit, hTrade->pbData, 72, TRADE_DATA_SIZE, FALSE, &nRead, NULL);
	D2CommonStatsUnitAddValue(hUnit, STAT_GOLD, -(DWORD)D2CommonStatsUnitGetValue(hUnit, STAT_GOLD));
	D2CommonStatsUnitAddValue(hUnit, STAT_GOLD, hTrade->dwStartGold);
}

void __fastcall PlrTradeSendUnitGone(GAME *ptGame, UNIT *hPlayer, void *pArg)
{
	if (hPlayer == (UNIT *)pArg)
		return;
	SCmdSendUnitTypeId(SUnitGetClient(hPlayer), 133, 0, ((UNIT *)pArg)->dwId);
}

static __inline BOOL PlrTradeCanTakeItems(GAME *ptGame, UNIT *hFrom, UNIT *hTo)
{
	void *ptNode, *ptNext;
	UNIT *hItem;

	for (ptNode = D2CommonINVENTORYGetCategoryListHead(hFrom->ptInventory); ptNode; ptNode = ptNext)
	{
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
		ptNext = (void *)D2CommonINVENTORYNodeGetNext(ptNode);
		if (D2CommonITEMSDataGetPage(hItem) == INVPAGE_TRADE
			&& !ITEMSModeCanTakeQuestItem(ptGame, hTo, hItem))
			return FALSE;
	}
	return TRUE;
}

void __fastcall PlrTradeCommit(GAME *ptGame, UNIT *hUnit, UNIT *hOther,
	PLRINFO *ptPlayerInfo1, PLRINFO *ptPlayerInfo2)
{
	PLRTRADE *hTrade;
	DWORD dwSum;
	BOOL fFirstFits;

	hTrade = ptPlayerInfo1->hTrade;
	if (hTrade->dwGoldOffered)
	{
		if (D2CommonStatsUnitGetValue(hUnit, STAT_GOLD) < (int)hTrade->dwGoldOffered)
		{
			PlrTradeReset(ptGame, hUnit, hOther, ptPlayerInfo1, ptPlayerInfo2);
			return;
		}

		if (((DWORD)D2CommonStatsUnitGetValue(hOther, STAT_GOLD) + hTrade->dwGoldOffered) > D2CommonUnitCalcGoldLimit(hOther))
		{
			PlrTradeReset(ptGame, hUnit, hOther, ptPlayerInfo1, ptPlayerInfo2);
			SCmdSendTradeEvent(SUnitGetClient(hUnit), 10);
			SCmdSendTradeEvent(SUnitGetClient(hOther), 9);
			return;
		}
	}
	hTrade = ptPlayerInfo2->hTrade;
	if (hTrade->dwGoldOffered)
	{
		if (D2CommonStatsUnitGetValue(hOther, STAT_GOLD) < (int)hTrade->dwGoldOffered)
		{
			PlrTradeReset(ptGame, hUnit, hOther, ptPlayerInfo1, ptPlayerInfo2);
			return;
		}

		if (((DWORD)D2CommonStatsUnitGetValue(hUnit, STAT_GOLD) + hTrade->dwGoldOffered) > D2CommonUnitCalcGoldLimit(hUnit))
		{
			PlrTradeReset(ptGame, hUnit, hOther, ptPlayerInfo1, ptPlayerInfo2);
			SCmdSendTradeEvent(SUnitGetClient(hUnit), 9);
			SCmdSendTradeEvent(SUnitGetClient(hOther), 10);
			return;
		}
	}
	fFirstFits = FALSE;
	if (!D2CommonINVENTORYTestTradeSpace(ptGame->pMemPool, hUnit, hOther, &fFirstFits))
	{
		ptPlayerInfo1->hTrade->dwGoldOffered = 0;
		ptPlayerInfo2->hTrade->dwGoldOffered = 0;
		if (fFirstFits)
		{
			SCmdSendTradeEvent(SUnitGetClient(hUnit), 9);
			SCmdSendTradeEvent(SUnitGetClient(hOther), 10);
		}
		else
		{
			SCmdSendTradeEvent(SUnitGetClient(hUnit), 10);
			SCmdSendTradeEvent(SUnitGetClient(hOther), 9);
		}
		PlrTradeReset(ptGame, hUnit, hOther, ptPlayerInfo1, ptPlayerInfo2);
		return;
	}
	if (PlrTradeCanTakeItems(ptGame, hUnit, hOther))
	{
		if (PlrTradeCanTakeItems(ptGame, hOther, hUnit))
		{
			hUnit->dwFlags |= 8388608;
			hOther->dwFlags |= 8388608;
			ptPlayerInfo1->dwTradeState = 9;
			ptPlayerInfo2->dwTradeState = 9;
			if (!PlrTradeMoveItems(ptGame, hUnit, hOther))
			{
				PlrTradeReset(ptGame, hUnit, hOther, ptPlayerInfo1, ptPlayerInfo2);
				return;
			}
			if (!PlrTradeMoveItems(ptGame, hOther, hUnit))
			{
				PlrTradeReset(ptGame, hUnit, hOther, ptPlayerInfo1, ptPlayerInfo2);
				return;
			}
			hTrade = ptPlayerInfo1->hTrade;
			if (hTrade->dwGoldOffered)
			{
				if (D2CommonStatsUnitGetValue(hUnit, STAT_GOLD) >= (int)hTrade->dwGoldOffered)
					D2CommonStatsUnitAddValue(hUnit, STAT_GOLD, -(int)hTrade->dwGoldOffered);
				ITEMSAddGold(ptGame, hOther, hTrade->dwGoldOffered);
			}
			hTrade = ptPlayerInfo2->hTrade;
			if (hTrade->dwGoldOffered)
			{
				if (D2CommonStatsUnitGetValue(hOther, STAT_GOLD) >= (int)hTrade->dwGoldOffered)
					D2CommonStatsUnitAddValue(hOther, STAT_GOLD, -(int)hTrade->dwGoldOffered);
				ITEMSAddGold(ptGame, hUnit, hTrade->dwGoldOffered);
			}
			SUnitResetInteract(hUnit);
			SUnitResetInteract(hOther);
			PlrTradeFree(ptGame, hUnit);
			PlrTradeFree(ptGame, hOther);
			ITEMSModeResetBookSkills(ptGame, hUnit);
			ITEMSModeResetBookSkills(ptGame, hOther);
			SCmdSendTradeEvent(SUnitGetClient(hUnit), 13);
			SCmdSendTradeEvent(SUnitGetClient(hOther), 13);
			return;
		}
	}
	PlrTradeReset(ptGame, hUnit, hOther, ptPlayerInfo1, ptPlayerInfo2);
}

void __fastcall PlrTradeReset(GAME *ptGame, UNIT *hUnit, UNIT *hOther,
	PLRINFO *ptPlayerInfo1, PLRINFO *ptPlayerInfo2)
{
	D2CommonUnitSetChanged(hUnit, TRUE);
	D2CommonUnitSetChanged(hOther, TRUE);
	hUnit->dwFlags |= 8388608;
	hOther->dwFlags |= 8388608;
	ptPlayerInfo1->hTrade->dwGoldOffered = 0;
	ptPlayerInfo2->hTrade->dwGoldOffered = 0;
	PlrTradeRestoreItems(ptGame, hUnit, TRUE);
	PlrTradeRestoreItems(ptGame, hOther, TRUE);
	SCmdSendTradeGold(SUnitGetClient(hUnit), 0, 0);
	SCmdSendTradeGold(SUnitGetClient(hUnit), 0, 1);
	SCmdSendTradeGold(SUnitGetClient(hOther), 0, 0);
	SCmdSendTradeGold(SUnitGetClient(hOther), 0, 1);
	SCmdSendTradeEvent(SUnitGetClient(hUnit), 6);
	SCmdSendTradeEvent(SUnitGetClient(hOther), 6);
	ptPlayerInfo1->dwTradeState = 5;
	ptPlayerInfo2->dwTradeState = 5;
	ITEMSModeResetBookSkills(ptGame, hUnit);
	ITEMSModeResetBookSkills(ptGame, hOther);
}

BYTE __fastcall PlrTradeMoveItems(GAME *ptGame, UNIT *hFrom, UNIT *hTo)
{
	ITEMCREATE tCreate;
	CLIENT *ptClient;
	void *ptNode, *ptNext;
	UNIT *hItem, *hNewItem;
	DWORD dwClientFlag;

	dwClientFlag = ClientTestFlags(SUnitGetClient(hFrom), 4);
	for (ptNode = D2CommonINVENTORYGetCategoryListHead(hFrom->ptInventory); ptNode; ptNode = ptNext)
	{
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
		ptNext = (void *)D2CommonINVENTORYNodeGetNext(ptNode);
		if (D2CommonITEMSDataGetPage(hItem) != INVPAGE_TRADE)
			continue;
		ITEMSFillCreate(ptGame, &tCreate, hItem, 1);
		hNewItem = ITEMSRecreate(ptGame, &tCreate);
		if (!hNewItem)
			continue;
		if (D2CommonITEMSGetType(hNewItem) == 7)
		{
			D2CommonITEMSSetNameBuffer(hNewItem, D2CommonITEMSGetNameBuffer(hItem));
			D2CommonITEMSDataSetEarLevel(hNewItem, D2CommonITEMSDataGetEarLevelRaw((DWORD)hItem));
			D2CommonITEMSDataSetFlags(hNewItem, 32768, dwClientFlag);
		}
		D2CommonUnitSetMode(hNewItem, 4);
		D2CommonITEMSDataSetPage(hNewItem, 0);
		if (!ITEMSModePlaceCursorItem(ptGame, hTo, hNewItem->dwId, 0, 0, TRUE, TRUE, NULL))
		{
			SCmdSendTradeEvent(SUnitGetClient(hFrom), 10);
			SCmdSendTradeEvent(SUnitGetClient(hTo), 9);
			return FALSE;
		}
		if (ITEMSModeHasContents(hItem) && !ITEMSModeCopySockets(ptGame, hTo, hItem, hNewItem, TRUE))
		{
			SCmdSendTradeEvent(SUnitGetClient(hFrom), 10);
			SCmdSendTradeEvent(SUnitGetClient(hTo), 9);
			return FALSE;
		}
		D2CommonItemTblsGetItemStats(hNewItem->dwClassId);
	}
	ptClient = SUnitGetClient(hFrom);
	for (ptNode = D2CommonINVENTORYGetCategoryListHead(hFrom->ptInventory); ptNode; ptNode = ptNext)
	{
		ptNext = (void *)D2CommonINVENTORYNodeGetNext(ptNode);
		BYTE bPage;
		hItem = (UNIT *)D2CommonINVENTORYNodeGetKeyA(ptNode);
		bPage = D2CommonITEMSDataGetPage(hItem);
		if (bPage == 1 || bPage == INVPAGE_TRADE)
		{
			DWORD dwFlags = 0;
			D2CommonITEMSSetOrClearFlags(&dwFlags, 32, TRUE);
			((DWORD (__stdcall *)(DWORD, BYTE))D2CommonITEMSDataSetPreviousPageRaw)((DWORD)hItem, bPage);
			SCmdSendItemFromGrid(ptClient, hFrom, hItem, dwFlags);
			ITEMSModeRemoveGridItem(ptGame, hFrom, hItem, FALSE);
		}
	}
	return TRUE;
}

void __fastcall PlrTradeLeave(GAME *ptGame, UNIT *hUnit)
{
	DWORD dwType, dwId;
	UNIT *hOther;

	if (!SUnitGetInteract(hUnit, &dwType, &dwId) || dwType != UNIT_PLAYER)
		return;
	hOther = SUnitFind(ptGame, UNIT_PLAYER, dwId);
	if (!hOther)
		return;
	SUnitResetInteract(hUnit);
	if (!SUnitGetInteract(hOther, &dwType, &dwId) || dwType != UNIT_PLAYER || dwId != hUnit->dwId)
		return;
	((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->dwTradeState = 9;
	PlrTradeRestoreItems(ptGame, hUnit, FALSE);
	PlrTradeRestoreItems(ptGame, hOther, TRUE);
	PlrTradeFree(ptGame, hUnit);
	PlrTradeFree(ptGame, hOther);
	SCmdSendTradeEvent(SUnitGetClient(hOther), 12);
	SUnitResetInteract(hOther);
	((PLRINFO *)D2CommonUnitGetPlrInfo(hOther))->dwTradeState = 9;
}

void __fastcall PlrTradeInteractEnd(GAME *ptGame, UNIT *hUnit)
{
	PlrTradeLeave(ptGame, hUnit);
}

void __fastcall PlrTradeEndInteract(GAME *ptGame, UNIT *hUnit)
{
	SUnitResetInteract(hUnit);
	ITEMSModeResetBookSkills(ptGame, hUnit);
}

void __fastcall PlrTradeAddItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	ITEMCREATE tCreate;
	DWORD dwType, dwId;
	UNIT *hOther, *hNewItem;
	DWORD dwClientFlag, dwItemId, dwNewItemId;
	int nX, nY;
	PLRTRADE *hTrade;
	PLRTRADEITEM *ptEntry;
	PLRINFO *ptPlayerInfo1, *ptPlayerInfo2;

	if (SUnitGetInteract(hUnit, &dwType, &dwId) && dwType != UNIT_PLAYER)
	{
		SCmdSendTradeEvent(SUnitGetClient(hUnit), 12);
		return;
	}
	hOther = SUnitFind(ptGame, UNIT_PLAYER, dwId);
	if (!hOther)
	{
		SCmdSendTradeEvent(SUnitGetClient(hUnit), 12);
		return;
	}
	ITEMSFillCreate(ptGame, &tCreate, hItem, 1);
	hNewItem = ITEMSRecreate(ptGame, &tCreate);
	dwClientFlag = ClientTestFlags(SUnitGetClient(hUnit), 4);
	if (D2CommonITEMSGetType(hNewItem) == 7)
	{
		D2CommonITEMSSetNameBuffer(hNewItem, D2CommonITEMSGetNameBuffer(hItem));
		D2CommonITEMSDataSetEarLevel(hNewItem, D2CommonITEMSDataGetEarLevelRaw((DWORD)hItem));
		D2CommonITEMSDataSetFlags(hNewItem, 32768, dwClientFlag);
	}
	nX = tCreate.nX;
	nY = tCreate.nY;
	D2CommonUnitSetX(hNewItem, nX);
	D2CommonUnitSetY(hNewItem, nY);
	D2CommonUnitSetMode(hNewItem, 4);
	D2CommonITEMSDataSetPage(hNewItem, 1);
	if (!ITEMSModePlaceCursorItem(ptGame, hOther, hNewItem->dwId, nX, nY, FALSE, TRUE, NULL))
	{
		ITEMSFree(ptGame, hNewItem);
		ptPlayerInfo1 = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
		ptPlayerInfo2 = (PLRINFO *)D2CommonUnitGetPlrInfo(hOther);
		PlrTradeCancel(ptGame, hUnit, hOther, ptPlayerInfo1, ptPlayerInfo2);
		return;
	}
	if (ITEMSModeHasContents(hItem))
		ITEMSModeCopySockets(ptGame, hOther, hItem, hNewItem, TRUE);
	dwNewItemId = hNewItem->dwId;
	dwItemId = hItem->dwId;
	hTrade = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->hTrade;
#line 2596
	ptEntry = (PLRTRADEITEM *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(PLRTRADEITEM));
	ptEntry->dwItemId = dwItemId;
	ptEntry->dwCopyId = dwNewItemId;
	ptEntry->pNext = hTrade->ptItems;
	hTrade->ptItems = ptEntry;
}

void __fastcall PlrTradeRemoveItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	DWORD dwType, dwId;
	UNIT *hOther;
	DWORD dwItemId;
	PLRTRADEITEM *ptEntry;

	if (SUnitGetInteract(hUnit, &dwType, &dwId) && dwType != UNIT_PLAYER)
	{
		SCmdSendTradeEvent(SUnitGetClient(hUnit), 12);
		return;
	}
	hOther = SUnitFind(ptGame, UNIT_PLAYER, dwId);
	dwItemId = hItem->dwId;
	for (ptEntry = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->hTrade->ptItems; ptEntry;
		ptEntry = ptEntry->pNext)
	{
		if (ptEntry->dwItemId == dwItemId)
		{
			int nResult;
			dwId = ptEntry->dwCopyId;
			if (ITEMSModeTakeToCursor(ptGame, hOther, dwId, &nResult, 1, TRUE, NULL, TRUE))
			{
				ptEntry = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->hTrade->ptItems;
				while (ptEntry)
					ptEntry = ptEntry->pNext;
			}
			return;
		}
	}
}

int __fastcall PlrTradeCopyItems(UNIT *hUnit, BYTE *pbBuffer, int nSize)
{
	PLRTRADE *hTrade;

	hTrade = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->hTrade;
	if (!hTrade)
		return 0;
	if (nSize < hTrade->nSize)
		return 0;
	memcpy(pbBuffer, hTrade->pbData, hTrade->nSize);
	return hTrade->nSize;
}

int __fastcall PlrTradeCanAddItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem)
{
	PLRINFO *ptPlayerInfo1, *ptPlayerInfo2;
	DWORD dwType, dwId;
	UNIT *hOther;
	ITEMSTXT *ptItemStats;
	void *ptNode, *ptNext;
	SCMDSYSTEMMSG tSystemMsg;
	int nResult;

	ptPlayerInfo1 = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
	if (SUnitGetInteract(hUnit, &dwType, &dwId))
	{
		if (dwType != UNIT_PLAYER)
			return 1;
		hOther = SUnitFind(ptGame, UNIT_PLAYER, dwId);
		if (hOther)
		{
			ptPlayerInfo2 = (PLRINFO *)D2CommonUnitGetPlrInfo(hOther);
			if (!ptPlayerInfo2->hTrade)
				return 0;
			if (A4Q3MonsterKilled(ptGame))
				return 0;
			if ((hUnit->dwFlags & 8388608) || (hOther->dwFlags & 8388608))
				return 0;
		#line 2793
			D2ASSERT(ptPlayerInfo1->hTrade != NULL);
			nResult = 1;
			if (ptPlayerInfo1->dwTradeState == 4)
				nResult = 0;
			if (hItem)
			{
				ptItemStats = (ITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
				if (ptItemStats->quest)
				{
					if (ptItemStats->code == 544763746)
					{
						for (ptNode = D2CommonINVENTORYGetCategoryListHead(hUnit->ptInventory); ptNode;
							ptNode = ptNext)
						{
							ptNext = (void *)D2CommonINVENTORYNodeGetNext(ptNode);
							if (D2CommonITEMSDataGetPage(D2CommonINVENTORYNodeGetKeyA(ptNode)) == INVPAGE_CUBE)
							{
								nResult = 0;
								tSystemMsg.bCmd = S2C_ROSTER_EVENT;
								tSystemMsg.bEvent = 10;
								tSystemMsg.bColor = 0;
								tSystemMsg.dwArg = 0;
								tSystemMsg.bPlayerName[0] = 0;
								SCmdSendSystemMsg(SUnitGetClient(hUnit), &tSystemMsg);
								break;
							}
						}
					}
					if (ITEMSFindByType(hOther, ptItemStats->code))
						nResult = 0;
				}
				else if (D2CommonITEMSDataTestFlags(hItem, 4096))
					nResult = 0; }
			hUnit->dwFlags |= 4194304;
			ptPlayerInfo1->dwTradeState = 7;
			ptPlayerInfo2->dwTradeState = 7;
			SCmdSendTradeEvent(SUnitGetClient(hUnit), 6);
			SCmdSendTradeEvent(SUnitGetClient(hOther), 6);
			return nResult;
		}
	}
	if ((int)ptPlayerInfo1->dwTradeState <= 4)
		return 1;
	return 0;
}

BOOL __fastcall PlrTradeIsFree(GAME *ptGame, UNIT *hUnit)
{
	DWORD dwType, dwId;

	if (SUnitGetInteract(hUnit, &dwType, &dwId) && dwType == UNIT_PLAYER)
		return FALSE;
	return TRUE;
}

BOOL __fastcall PlrTradeIsTrading(GAME *ptGame, UNIT *hUnit)
{
	DWORD dwType, dwId;

	if (SUnitGetInteract(hUnit, &dwType, &dwId) && dwType == UNIT_PLAYER
		&& SUnitFind(ptGame, UNIT_PLAYER, dwId))
		return TRUE;
	return FALSE;
}

void __fastcall PlrTradeUpdateAll(GAME *ptGame)
{
	SUnitForEachPlayer(ptGame, PlrTradeUpdate, NULL);
}

void __fastcall PlrTradeUpdate(GAME *ptGame, UNIT *hUnit, void *pArg)
{
	PLRINFO *ptPlayerInfo;

	ptPlayerInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
	if ((int)ptPlayerInfo->dwTradeState <= 4)
		return;
	if (ptPlayerInfo->dwTradeState == 7)
	{
		ptPlayerInfo->dwTradeState = 8;
		return;
	}
	if (ptPlayerInfo->dwTradeState == 8)
	{
		hUnit->dwFlags &= ~4194304;
		ptPlayerInfo->dwTradeState = 3;
		return;
	}
	switch (ptPlayerInfo->dwTradeState)
	{
	case 9:
		ptPlayerInfo->dwTradeState = 10;
		break;
	case 10:
		ptPlayerInfo->dwTradeState = 0;
		hUnit->dwFlags &= ~12582912;
		break;
	case 5:
		ptPlayerInfo->dwTradeState = 6;
		break;
	case 6:
		ptPlayerInfo->dwTradeState = 3;
		hUnit->dwFlags &= ~12582912;
		break;
	}
}
