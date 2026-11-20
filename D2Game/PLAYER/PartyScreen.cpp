#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\PLAYER\\PartyScreen.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/SCmd.h"
#include "../OBJECTS/objrgn.h"
#include "PartyScreen.h"
#include "Player.h"
#include "../UNIT/Party.h"
#include "../UNIT/Friendly.h"
#include "../UNIT/PlayerList.h"
#include "../UNIT/SUnit.h"

#define PARTY_MSG_CMD			83
#define PARTY_MSG_EVENT			7
#define PARTY_NONE				65535
#define PARTY_MIN_LEVEL			9
#define STAT_LEVEL				12
#define UNIT_PLAYER				0

typedef void (__fastcall *PFNGAMEUNITCALLBACK)(GAME *ptGame, UNIT *hUnit, void *pArg);

void __fastcall PartyScreenTellLeft(GAME *ptGame, UNIT *hPlayer, void *pArg);
void __fastcall PartyScreenInviteEach(GAME *ptGame, UNIT *hPlayer, void *pArg);
void __fastcall PartyScreenResetEach(GAME *ptGame, UNIT *hPlayer, void *pArg);

typedef void (__fastcall *PFNPARTYCOMMAND)(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg);
struct PARTYCOMMAND
{
	PFNPARTYCOMMAND	pfnProcess;
};
typedef struct PARTYCOMMAND PARTYCOMMAND;

void __fastcall PartyScreenOnCancel(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg)
{
	GAMEPLAYERMSG tMsg;
	CLIENT *ptClient;
	DWORD dwUnitId;
	int i;

	PlayerListAdd(ptGame, hUnit, hTarget);
	PlayerListAdd(ptGame, hTarget, hUnit);
	if (PlayerListGetState(hUnit, hTarget) != 4)
	{
		PlayerListSendState(hUnit, hTarget);
		PlayerListSendState(hTarget, hUnit);
		return;
	}
	if (SUnitGetParty(hTarget) == PARTY_NONE)
		PlayerListSetState(hUnit, hTarget, 0);
	else
		PlayerListSetState(hUnit, hTarget, 1);
	if (SUnitGetParty(hUnit) == PARTY_NONE)
		PlayerListSetState(hTarget, hUnit, 0);
	else
		PlayerListSetState(hTarget, hUnit, 1);
	dwUnitId = hUnit->dwId;
	ptClient = SUnitGetClient(hTarget);
	for (i = 0; i < (int)(sizeof(tMsg) / sizeof(DWORD)); i++)
		((DWORD *)&tMsg)[i] = 0;
	tMsg.bCmd = PARTY_MSG_CMD;
	tMsg.bEvent = PARTY_MSG_EVENT;
	tMsg.dwArg = dwUnitId;
	tMsg.bSubKind = 6;
	tMsg.bColor = 9;
	SCmdSendSystemMsg(ptClient, (SCMDSYSTEMMSG *)&tMsg);
	PlayerListSendState(hUnit, hTarget);
	PlayerListSendState(hTarget, hUnit);
}

void __fastcall PartyScreenOnInvite(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg)
{
	GAMEPLAYERMSG tMsg;
	CLIENT *ptClient;
	DWORD dwUnitId;
	int i;

	PlayerListAdd(ptGame, hUnit, hTarget);
	PlayerListAdd(ptGame, hTarget, hUnit);
	if (PlayerListGetState(hUnit, hTarget))
	{
		PlayerListSendState(hUnit, hTarget);
		PlayerListSendState(hTarget, hUnit);
		return;
	}
	PlayerListSetState(hUnit, hTarget, 4);
	PlayerListSetState(hTarget, hUnit, 2);
	dwUnitId = hUnit->dwId;
	ptClient = SUnitGetClient(hTarget);
	for (i = 0; i < (int)(sizeof(tMsg) / sizeof(DWORD)); i++)
		((DWORD *)&tMsg)[i] = 0;
	tMsg.bCmd = PARTY_MSG_CMD;
	tMsg.bEvent = PARTY_MSG_EVENT;
	tMsg.dwArg = dwUnitId;
	tMsg.bSubKind = 5;
	tMsg.bColor = 2;
	SCmdSendSystemMsg(ptClient, (SCMDSYSTEMMSG *)&tMsg);
	PlayerListSendState(hUnit, hTarget);
	PlayerListSendState(hTarget, hUnit);
}

void __fastcall PartyScreenOnLeave(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg)
{
	WORD t79 = SUnitGetParty(hUnit);
	PartyForEach(ptGame, t79, PartyScreenTellLeft, hUnit);
	PartyRemovePlayer(ptGame, hUnit);
}
#line 131

void __fastcall PartyScreenTellLeft(GAME *ptGame, UNIT *hPlayer, void *pArg)
{
	GAMEPLAYERMSG tMsg;
	CLIENT *ptClient;
	UNIT *hLeaver;
	DWORD dwUnitId;
	int i;

	hLeaver = (UNIT *)pArg;
	if (hPlayer == hLeaver)
		return;
	dwUnitId = hLeaver->dwId;
	ptClient = SUnitGetClient(hPlayer);
	for (i = 0; i < (int)(sizeof(tMsg) / sizeof(DWORD)); i++)
		((DWORD *)&tMsg)[i] = 0;
	tMsg.bCmd = PARTY_MSG_CMD;
	tMsg.bEvent = PARTY_MSG_EVENT;
	tMsg.dwArg = dwUnitId;
	tMsg.bSubKind = 9;
	tMsg.bColor = 9;
	SCmdSendSystemMsg(ptClient, (SCMDSYSTEMMSG *)&tMsg);
}

void __fastcall PartyScreenOnHostile(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwArg)
{
	GAMEPLAYERMSG tMsg;
	CLIENT *ptClient;
	DWORD dwUnitId;
	WORD wParty;
	BOOL fDeclared;
	int i;

	PlayerListAdd(ptGame, hUnit, hTarget);
	PlayerListAdd(ptGame, hTarget, hUnit);
	if (PlayerListTestFlag(hUnit, hTarget, 8) == dwArg)
	{
		PlayerListSendFlags(ptGame, hUnit, hTarget);
		PlayerListSendFlags(ptGame, hTarget, hUnit);
		return;
	}
	if (!PlayerListTestFlag(hUnit, hTarget, 8)
		&& D2CommonStatsUnitGetValue(hTarget, STAT_LEVEL) < PARTY_MIN_LEVEL)
		return;
	fDeclared = FALSE;
	if (dwArg)
	{
		wParty = SUnitGetParty(hTarget);
		if (wParty == PARTY_NONE)
		{
			FriendlySetHostile(ptGame, hUnit, hTarget);
			FriendlySetHostile(ptGame, hTarget, hUnit);
			dwUnitId = hUnit->dwId;
			ptClient = SUnitGetClient(hTarget);
			for (i = 0; i < (int)(sizeof(tMsg) / sizeof(DWORD)); i++)
				((DWORD *)&tMsg)[i] = 0;
			tMsg.dwArg = dwUnitId;
			tMsg.bCmd = PARTY_MSG_CMD;
			tMsg.bEvent = PARTY_MSG_EVENT;
			tMsg.bSubKind = 3;
			tMsg.bColor = 8;
			SCmdSendSystemMsg(ptClient, (SCMDSYSTEMMSG *)&tMsg);
		}
		else if (D2CommonStatsUnitGetBaseValue(hUnit, STAT_LEVEL) >= PARTY_MIN_LEVEL)
		{
			PartyForEach(ptGame, wParty, PartyScreenInviteEach, hUnit);
		}
		PlayerRemovePortal(ptGame, hUnit);
		fDeclared = TRUE;
	}
	else
	{
		FriendlyClearHostile(ptGame, hUnit, hTarget);
		dwUnitId = hUnit->dwId;
		ptClient = SUnitGetClient(hTarget);
		for (i = 0; i < (int)(sizeof(tMsg) / sizeof(DWORD)); i++)
			((DWORD *)&tMsg)[i] = 0;
		tMsg.bCmd = PARTY_MSG_CMD;
		tMsg.bEvent = PARTY_MSG_EVENT;
		tMsg.dwArg = dwUnitId;
		tMsg.bSubKind = 4;
		tMsg.bColor = 0;
		SCmdSendSystemMsg(ptClient, (SCMDSYSTEMMSG *)&tMsg);
	}
	if (fDeclared)
	{
		wParty = SUnitGetParty(hUnit);
		if (wParty != PARTY_NONE && wParty == SUnitGetParty(hTarget))
		{
			wParty = SUnitGetParty(hUnit);
			PartyForEach(ptGame, wParty, PartyScreenTellLeft, hUnit);
			PartyRemovePlayer(ptGame, hUnit);
		}
	}
}

void __fastcall PartyScreenInviteEach(GAME *ptGame, UNIT *hPlayer, void *pArg)
{
	GAMEPLAYERMSG tMsg;
	CLIENT *ptClient;
	UNIT *hUnit;
	DWORD dwUnitId;
	int i;

	hUnit = (UNIT *)pArg;
	if (hUnit == hPlayer)
		return;
	if (D2CommonStatsUnitGetBaseValue(hPlayer, STAT_LEVEL) < PARTY_MIN_LEVEL)
		return;
	FriendlySetHostile(ptGame, hUnit, hPlayer);
	FriendlySetHostile(ptGame, hPlayer, hUnit);
	dwUnitId = hUnit->dwId;
	ptClient = SUnitGetClient(hPlayer);
	for (i = 0; i < (int)(sizeof(tMsg) / sizeof(DWORD)); i++)
		((DWORD *)&tMsg)[i] = 0;
	tMsg.bCmd = PARTY_MSG_CMD;
	tMsg.bEvent = PARTY_MSG_EVENT;
	tMsg.dwArg = dwUnitId;
	tMsg.bSubKind = 3;
	tMsg.bColor = 8;
	SCmdSendSystemMsg(ptClient, (SCMDSYSTEMMSG *)&tMsg);
}

void __fastcall PartyScreenReset(GAME *ptGame, UNIT *hUnit)
{
	SUnitForEachPlayer(ptGame, PartyScreenResetEach, hUnit);
}

void __fastcall PartyScreenResetEach(GAME *ptGame, UNIT *hPlayer, void *pArg)
{
	UNIT *hUnit;

	hUnit = (UNIT *)pArg;
	if (hPlayer == hUnit)
	{
		PlayerListSetState(hPlayer, hPlayer, 1);
		SCmdSendPlayerInfo(hPlayer, hPlayer);
		return;
	}
	PlayerListSetState(hPlayer, hUnit, 0);
	if (SUnitGetParty(hPlayer) == PARTY_NONE)
		PlayerListSetState(hUnit, hPlayer, 0);
	else
		PlayerListSetState(hUnit, hPlayer, 1);
	SCmdSendPlayerInfo(hPlayer, hUnit);
	SCmdSendPlayerInfo(hUnit, hPlayer);
}

static const PARTYCOMMAND sgcptPartyCommands[10] =
{
	{ NULL },
	{ PartyScreenOnLoot },
	{ PartyScreenOnHear },
	{ PartyScreenOnSquelch },
	{ PartyScreenOnHostile },
	{ NULL },
	{ PartyScreenOnInvite },
	{ PartyScreenOnCancel },
	{ PartyScreenOnAccept },
	{ PartyScreenOnLeave },
};

void __fastcall PartyScreenCommand(GAME *ptGame, UNIT *hUnit, int eCommand  ,
	DWORD dwUnitId, DWORD dwArg)
{
	UNIT *hTarget;

	hTarget = SUnitFind(ptGame, UNIT_PLAYER, dwUnitId);
	if (!hTarget)
		return;
	if (!sgcptPartyCommands[eCommand].pfnProcess)
		return;
#line 527
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgcptPartyCommands[eCommand].pfnProcess),
		"sgcptPartyCommands[eCommand].pfnProcess");
	sgcptPartyCommands[eCommand].pfnProcess(ptGame, hUnit, hTarget, dwArg);
}
