#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\UNIT\\SUnitMsg.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "../GAME/SCmd.h"
#include "../INVENTORY/InvMode.h"
#include "../MISSILES/Missiles.h"
#include "../MONSTER/Monster.h"
#include "../MONSTER/MonsterMsg.h"
#include "../MONSTER/MonsterUnique.h"
#include "../PLAYER/Player.h"
#include "../PLAYER/PlrMsg.h"
#include "PlayerList.h"
#include "SUnit.h"
#include "SUnitMsg.h"

#define UNIT_PLAYER			0
#define UNIT_OBJECT			2
#define UNIT_MISSILE		3
#define UNIT_ITEM			4
#define UNIT_ROOMTILE		5
#define MMODE_SEQUENCE		14
#define MMODE_NEUTRAL		1
#define NUM_OVERLAY_CLASSES	190
#define NUM_STATES			138
#define OBJECT_PORTAL		59
#define UNITFLAG_7			7
#define STATE_5D			93
#define MONSTER_123			291
#define ITEMMODE_3			3
#define ITEMFLAG_SEND		16
#define ITEMFLAG_OWNED		4096
#define MONFLAG_SEQUENCE	16
#define MONFLAG_FULL		512
#define OBJFLAG_SEND		4
#define PLAYERLIST_4		4
#define PLAYERLIST_2		2
#define STAT_LIFE			6
#define SKILL_BB			187
#define SKILL_2E			46
#define SKILL_AD			173
#define SKILL_77			119
#define STATE_13			13
#define STATE_22			22
#define STATE_29			41
#define STATE_47			47
#define STATE_62			98
#define STATE_97			97

void __fastcall SUnitMsgSendUnit(GAME *ptGame  , UNIT *hUnit
, CLIENT *ptClient  )
{
	GAMECOORD tPos;
	WORD wClassId;
	UNIT *hClientPlayer;
	UNIT *hPlayer;
	UNIT *hTarget;
	DWORD dwTargetId;
	struct MONSTERUNIQUE *ptUnique;
	int eMode;
	void *ptPath;
	void *pInventory;

	wClassId = (WORD)hUnit->dwClassId;
	D2CommonUnitGetPosition(hUnit, &tPos);
	hClientPlayer = ClientGetPlayerUnit(ptClient, FALSE);
	switch (hUnit->eType)
	{
	case UNIT_PLAYER:
		SCmdSendPlayerWithInfo(ptClient, hUnit->dwId, (BYTE)hUnit->dwClassId,
			(const char *)hUnit->uTypeData, (WORD)tPos.dwX, (WORD)tPos.dwY);
		break;
	case UNIT_ITEM:
		if (!(hUnit->dwFlags0 & ITEMFLAG_SEND))
			break;
		if (hUnit->dwMode == ITEMMODE_3 && (hUnit->dwFlags0 & ITEMFLAG_OWNED))
			SCmdSendItemDropped(ptClient, hClientPlayer, hUnit, 0);
		else
			SCmdSendItemNoOwner(ptClient, hUnit, 0);
		break;
	case UNIT_OBJECT:
		SCmdSendObjectAt(ptClient, S2C_OBJECT_STATE, UNIT_OBJECT, hUnit->dwId, (WORD)hUnit->dwClassId,
			(WORD)tPos.dwX,
			(WORD)tPos.dwY, (BYTE)hUnit->dwMode, OBJDATA(hUnit)->field_04);
		if (OBJDATA(hUnit)->ptTypeData->SubClass & OBJFLAG_SEND)
		{
			SCmdSendObject(ptClient, hUnit);
			if (wClassId == OBJECT_PORTAL)
			{
				hTarget = SUnitGetPortalTarget(ptGame, hUnit);
				dwTargetId = (DWORD)-1;
				if (hTarget)
					dwTargetId = hTarget->dwId;
				SCmdSendPortalOwner(ptGame, ptClient, hUnit->dwId, dwTargetId);
			}
		}
		break;
	case UNIT_MONSTER:
		ptUnique = ((MONSTERDATA *)hUnit->uTypeData)->ptUnique;
		eMode = (int)hUnit->dwMode;
		if (!(hUnit->dwFlags0 & MONFLAG_SEQUENCE))
			eMode = MMODE_NEUTRAL;
		else
		{
#line 97
			D2ASSERT(eMode != MMODE_SEQUENCE);
		}
		if (MonsterUniqueHasMods(ptUnique) || MonsterHasComponents(hUnit)
			|| (hUnit->dwFlags0 & MONFLAG_FULL))
		{
			SCmdSendMonster(ptClient, hUnit, (BYTE)eMode);
			SUnitMsgSendStates(hUnit, ptClient, TRUE);
			break;
		}
		{
			int nLife;
			int nMaxLife;
			BYTE bLife;

			nLife = D2CommonStatsUnitGetValue(hUnit, STAT_LIFE);
			nMaxLife = D2CommonStatsUnitCalcMaxHp(hUnit);
			if (nMaxLife)
				bLife = (BYTE)((nLife << 7) / nMaxLife);
			else
				bLife = 128;
			SCmdSendMonsterShort(ptClient, hUnit->dwId, (WORD)hUnit->dwClassId, (WORD)tPos.dwX,
				(WORD)tPos.dwY, bLife, (BYTE)eMode);
		}
		SUnitMsgSendStates(hUnit, ptClient, TRUE);
		break;
	case UNIT_MISSILE:
		ptPath = hUnit->ptPath;
		MissilesSendToClient(ptClient, ptGame, hUnit, D2CommonPathGetVelocity(ptPath));
		break;
	default:
		SCmdSendUnitClass(ptClient, 9, (BYTE)hUnit->eType, hUnit->dwId, (BYTE)hUnit->dwClassId,
			(WORD)tPos.dwX, (WORD)tPos.dwY);
		break;
	}
	switch (hUnit->eType)
	{
	case UNIT_PLAYER:
		if (SUnitIsDead(hUnit) && D2CommonStatsGetState(hUnit, UNITFLAG_7))
		{
			pInventory = hUnit->ptInventory;
			hPlayer = SUnitFind(ptGame, UNIT_PLAYER, D2CommonINVENTORYGetOwnerId(pInventory));
#line 136
			D2ASSERT(hPlayer);
			SCmdSendCorpseOwner(ptClient, hPlayer, hUnit, (hUnit->dwFlags >> 12) & 1);
		}
		else
			PlayerSendItems(hUnit, ptClient);
		SUnitMsgSendStates(hUnit, ptClient, TRUE);
		PlrMsgSendMode(ptGame, hUnit, ptClient);
		SUnitMsgSendChat(hUnit, ptClient);
		PlrMsgSendStats(hUnit, ptClient);
		break;
	case UNIT_MONSTER:
		MonsterSendMode(ptGame, hUnit, ptClient);
		if (D2CommonStatsGetState(hUnit, STATE_5D) || hUnit->dwClassId == MONSTER_123)
			PlayerSendItems(hUnit, ptClient);
		break;
	case UNIT_ITEM:
		INVENTORYModeSendGroundItem(hUnit, ptClient);
		break;
	case UNIT_ROOMTILE:
		return;
	}
}

void __fastcall SUnitMsgSendStates(UNIT *hUnit  , CLIENT *ptClient
, BOOL bAll)
{
	void *hList;
	UNIT *hOwner;
	int nState;
	DWORD dwValue;
	DWORD dwValue2;
	BOOL bList;

	dwValue = 0;
	dwValue2 = 0;
	for (nState = 0; nState < NUM_STATES; nState++)
	{
		if (!D2CommonStatsUnitTestActiveStateBit(hUnit, nState) && !bAll)
			continue;
		if (!D2CommonStatsGetState(hUnit, nState))
			continue;
		bList = TRUE;
		hList = (void *)D2CommonStatsFindListByState(hUnit, nState);
		if (!hList)
			bList = FALSE;
		if (D2CommonStatsTestStateSendsValue(nState))
		{
			switch (nState)
			{
			case STATE_13:
				if (hUnit->eType == UNIT_MONSTER)
					dwValue = D2CommonSkillsCalcNodeLevel(hUnit, D2CommonSkillsFindByKey(hUnit, SKILL_BB), 1);
				else
					dwValue = D2CommonSkillsCalcNodeLevel(hUnit, D2CommonSkillsFindByKey(hUnit, SKILL_2E), 1);
				break;
			case STATE_22:
				dwValue = D2CommonSkillsCalcNodeLevel(hUnit, D2CommonSkillsFindByKey(hUnit, SKILL_AD), 1);
				break;
			case STATE_97:
				dwValue = D2CommonStatsListGetValueA(hList, 71);
				break;
			case STATE_47:
				dwValue = D2CommonSkillsCalcNodeLevel(hUnit, D2CommonSkillsFindByKey(hUnit, SKILL_77), 1);
				break;
			default:
				if (hList)
					dwValue = D2CommonStatsContainerGetEntry(hList, 0);
				if (!bList)
					continue;
				break;
			}
			SCmdSendUnitChangeStateVal(ptClient, (BYTE)hUnit->eType, hUnit->dwId, nState, dwValue);
		}
		else if (D2CommonStatsTestStateSendsTwoValues(nState))
		{
			switch (nState)
			{
			default:
				if (hList)
				{
					dwValue = D2CommonStatsContainerGetEntry(hList, 0);
					dwValue2 = D2CommonStatsContainerGetEntry(hList, 1);
				}
				break;
			case STATE_62:
				hOwner = SUnitGetOwner(SUnitGetGame(hUnit), hUnit);
				if (hOwner)
				{
					dwValue = hOwner->eType;
					dwValue2 = hOwner->dwId;
				}
				else
				{
					dwValue = 6;
					dwValue2 = (DWORD)-1;
				}
				goto sendtwo;
			case STATE_29:
				dwValue = D2CommonStatsListGetValueA(hList, 28);
				dwValue2 = D2CommonStatsListGetValueA(hList, 67);
				break;
			}
			if (!bList)
				continue;
sendtwo:
			SCmdSendUnitChangeStateValVal(ptClient, (BYTE)hUnit->eType, hUnit->dwId, nState, dwValue,
				dwValue2);
		}
	}
	if (D2CommonStatsUnitTestAnyActiveState(hUnit) || bAll)
		SCmdSendUnitChangeState(ptClient, (BYTE)hUnit->eType, hUnit->dwId,
			(const BYTE *)D2CommonStatsUnitGetPendingFlags(hUnit));
}

void __fastcall SUnitMsgSendOverlay(UNIT *hUnit  , CLIENT *ptClient
)
{
	void *hList;
	DWORD dwValue;
	int nOverlay;
	int nFrames;

	hList = (void *)D2CommonStatsFindListByMask(hUnit, 128);
	if (!hList)
		return;
	dwValue = D2CommonStatsListGetValueB(hList, 176);
	nOverlay = dwValue & 255;
	nFrames = (int)dwValue >> 8;
#line 323
	D2ASSERT(nOverlay >= 0 && nOverlay < NUM_OVERLAY_CLASSES);
#line 326
	D2ASSERT(nFrames < 256);
	SCmdSendOverlay(ptClient, (BYTE)hUnit->eType, hUnit->dwId, (BYTE)nOverlay, (BYTE)nFrames);
}

void __fastcall SUnitMsgSendRemove(UNIT *hUnit  , CLIENT *ptClient
)
{
	SCmdSendUnitRemove(ptClient, S2C_UNIT_DESPAWN, (BYTE)hUnit->eType, hUnit->dwId);
}

void __fastcall SUnitMsgSendChat(UNIT *hUnit  , CLIENT *ptClient
)
{
	SCMDCHATDATA tChatData;
	void *hChatNode;
	UNIT *hPlayer;
	int i;

	hChatNode = hUnit->ptChatNode;
	if (hChatNode)
	{
	ClientGetGame(ptClient);
	hPlayer = ClientGetPlayerUnit(ptClient, FALSE);
	if (hUnit && hPlayer && hUnit->eType == UNIT_PLAYER)
	{
		if (PlayerListTestFlag(hPlayer, hUnit, PLAYERLIST_4))
			return;
		if (PlayerListTestFlag(hUnit, hPlayer, PLAYERLIST_2))
			return;
	}
	tChatData.dwId = hUnit->dwId;
	tChatData.bType = (BYTE)hUnit->eType;
	tChatData.bCmd = S2C_CHAT_MESSAGE;
	tChatData.szName[0] = 0;
	tChatData.bKind = 5;
	for (i = 0; i < (int)(sizeof(tChatData.bCustomText) / sizeof(DWORD)); i++)
		((DWORD *)tChatData.bCustomText)[i] = 0;
	D2CommonChatNodeCopyString(hChatNode, tChatData.bCustomText);
	SCmdSendChat(ptClient, &tChatData);
	return;
	}
	SCmdSendUnitTypeId(ptClient, S2C_CLEAR_UNIT_OVERHEAD_TEXT, (BYTE)hUnit->eType, hUnit->dwId);
}
