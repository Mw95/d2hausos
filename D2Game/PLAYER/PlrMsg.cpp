#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\PLAYER\\PlrMsg.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Arena.h"
#include "../GAME/Clients.h"
#include "../GAME/Event.h"
#include "../GAME/Game.h"
#include "../GAME/Level.h"
#include "../GAME/SCmd.h"
#include "../ITEMS/ItemMode.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/MonsterAI.h"
#include "../MONSTER/MonsterUnique.h"
#include "../OBJECTS/ObjMode.h"
#include "PartyScreen.h"
#include "Player.h"
#include "PlayerPets.h"
#include "PlrModes.h"
#include "PlrMsg.h"
#include "PlrSave.h"
#include "PlrTrade.h"
#include "../QUESTS/QUESTS.h"
#include "../UNIT/PlayerList.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "../UNIT/SUNITNPC.h"
#include "../AI/AiGeneral.h"
#include "../GUILDS/Guilds.h"
#include "../SKILLS/Skills.h"
#include "FogImports.h"

#define UNIT_MONSTER			1
#define UNIT_OBJECT				2
#define UNIT_MISSILE			3
#define UNIT_ITEM				4
#define UNIT_ROOMTILE			5
#define NUM_UNIT_TYPES			6

#define PMODE_DEATH				0
#define PMODE_DEAD				17
#define IMODE_ONGROUND			3
#define IMODE_STORED			0
#define IMODE_INBELT			2
#define IMODE_ONCURSOR			4
#define IMODE_EQUIP				1
#define IMODE_SOCKETED			6
#define NUM_BODYLOCS			11
#define BODYLOC_RARM			4
#define BODYLOC_LARM			5
#define PLRMSG_ITEMRANGE		10
#define PLAYER_SORCERESS		1
#define PLAYER_BARBARIAN		4

#define STATS_HITPOINTS			6
#define STATS_MANA				8
#define STATS_STAMINA			10
#define STATS_LEVEL				12
#define STATS_MANARECOVERYBONUS	26
#define STATS_MANARECOVERY		27
#define STATS_VELOCITYPERCENT	67
#define STATS_ATTACKRATE		68
#define STATS_4A				74
#define STATS_AF				175

#define STATE_6A				106
#define STATE_64				100
#define STATE_46				70

#define PLRMSG_RANGE			50

static inline int PlrMsgAbs(int nValue)
{
	if (nValue < 0)
		return -nValue;
	else
		return nValue;
}
#define ABS(nValue)			PlrMsgAbs(nValue)

#define PLRMSG_HACK(pszDesc, nLine) \
	if (ptEventCallbackTable && ptEventCallbackTable->fpServerLogMessage) \
		ptEventCallbackTable->fpServerLogMessage(1, "Client hack detected-  desc:%s  file:%s  line:%d\n", \
			(pszDesc), D2_THIS_FILE, (nLine))

#pragma pack(push, 1)
struct PLRMSGXY
{
 	BYTE		bCmd;
 	WORD		wX;
 	WORD		wY;
};
typedef struct PLRMSGXY PLRMSGXY;

struct PLRMSGUNIT
{
 	BYTE		bCmd;
 	DWORD		dwUnitType;
 	DWORD		dwUnitId;
};
typedef struct PLRMSGUNIT PLRMSGUNIT;

struct PLRMSGINTERACT
{
 	BYTE		bCmd;
 	DWORD		dwUnitType;
 	DWORD		dwUnitId;
 	DWORD		dwFlag;
};
typedef struct PLRMSGINTERACT PLRMSGINTERACT;

struct PLRMSGITEM
{
 	BYTE		bCmd;
 	DWORD		dwItemId;
};
typedef struct PLRMSGITEM PLRMSGITEM;

struct PLRMSGCHAT
{
 	BYTE		bCmd;
 	BYTE		bKind;
 	BYTE		field_02;
 	BYTE		field_03[8];
 	char		szText[256];
 	char		szName[16];
};
typedef struct PLRMSGCHAT PLRMSGCHAT;

struct PLRMSGITEMGRID
{
 	BYTE		bCmd;
 	DWORD		dwItemId;
 	DWORD		dwX;
 	DWORD		dwY;
 	DWORD		dwPage;
};
typedef struct PLRMSGITEMGRID PLRMSGITEMGRID;

struct PLRMSGITEMBODY
{
 	BYTE		bCmd;
 	DWORD		dwItemId;
 	BYTE		bBodyLoc;
 	BYTE		unobserved_06[3];
};
typedef struct PLRMSGITEMBODY PLRMSGITEMBODY;

struct PLRMSGBODYLOC
{
 	BYTE		bCmd;
 	WORD		wBodyLoc;
};
typedef struct PLRMSGBODYLOC PLRMSGBODYLOC;

struct PLRMSGSWAPGRID
{
 	BYTE		bCmd;
 	DWORD		dwItemId;
 	DWORD		dwGridItemId;
 	DWORD		dwX;
 	DWORD		dwY;
};
typedef struct PLRMSGSWAPGRID PLRMSGSWAPGRID;

struct PLRMSGITEMXY
{
 	BYTE		bCmd;
 	DWORD		dwItemId;
 	DWORD		dwX;
 	DWORD		dwY;
};
typedef struct PLRMSGITEMXY PLRMSGITEMXY;

struct PLRMSGTWOIDS
{
 	BYTE		bCmd;
 	DWORD		dwItemId;
 	DWORD		dwOtherId;
};
typedef struct PLRMSGTWOIDS PLRMSGTWOIDS;

struct PLRMSGIDCODE
{
 	BYTE		bCmd;
 	DWORD		dwId;
 	WORD		wCode;
 	BYTE		unobserved_07[2];
};
typedef struct PLRMSGIDCODE PLRMSGIDCODE;

struct PLRMSGNPC
{
 	BYTE		bCmd;
 	DWORD		dwNpcId;
 	DWORD		dwItemId;
 	DWORD		dwField09;
 	DWORD		dwField0d;
};
typedef struct PLRMSGNPC PLRMSGNPC;

struct PLRMSGID
{
 	BYTE		bCmd;
 	DWORD		dwId;
};
typedef struct PLRMSGID PLRMSGID;

struct PLRMSGNPC38
{
 	BYTE		bCmd;
 	DWORD		dwAction;
 	DWORD		dwNpcId;
 	DWORD		dwField09;
};
typedef struct PLRMSGNPC38 PLRMSGNPC38;
#pragma pack(pop)

typedef void (__fastcall *PFNCLIENTCALLBACK)(CLIENT *ptClient, void *pArg);

struct PLRMSGWHISPER
{
 	SCMDCHATDATA	*pChatData;
 	PLRMSGCHAT	*ptMsg;
 	DWORD		dwSpeakerId;
};
typedef struct PLRMSGWHISPER PLRMSGWHISPER;

static BOOL sgbWhisperSent = 0;
static BOOL sgbWhisperRefused = 0;

#define NUM_PLAYER_STATS		16
#define NUM_SKILLS				221
#define PLRMSG_MAXSKILLLEVEL	20
#define PLRMSG_OPERATERANGE		10
#define PLRMSG_FIRSTSOUND		24
#define PLRMSG_LASTSOUND		30
#define PLRMSG_NUMLEVELS		109
#define PLRMSG_NUMMONSTERS		410
#define PLRMSG_NUMHOTKEYS		8
#define PLRMSG_NUMQUESTFLAGS	41
#define PLRMSG_NUMVAULTCMDS		4
#define PLRMSG_NUMPARTYCMDS		10
#define PLRMSG_NUMCOMMANDS		96
#define PLRMSG_RUNRANGE			20
#define PLRMSG_MINRANGE			5
#define PET_HIRELING			7
#define PMODE_NEUTRAL			1
#define PMODE_WALK				2
#define PMODE_RUN				3
#define STATE_6C				108
#define STATE_36				54
#define STATS_QUANTITY			70
#define OBJECT_TOWNPORTAL		59
#define ITEMCODE_EAR			544366949
#define GUILDRANK_LEADER		2

#pragma pack(push, 1)
struct PLRMSGWORD
{
 	BYTE		bCmd;
 	WORD		wValue;
};
typedef struct PLRMSGWORD PLRMSGWORD;

struct PLRMSGSELECTSKILL
{
 	BYTE		bCmd;
 	WORD		wLeft;
 	WORD		wSkill;
};
typedef struct PLRMSGSELECTSKILL PLRMSGSELECTSKILL;

struct PLRMSGOBJECTITEM
{
 	BYTE		bCmd;
 	BYTE		unobserved_01[4];
 	DWORD		dwObjectId;
 	DWORD		dwItemId;
 	WORD		wCall;
 	BYTE		unobserved_0f[2];
};
typedef struct PLRMSGOBJECTITEM PLRMSGOBJECTITEM;

struct PLRMSGWARP
{
 	BYTE		bCmd;
 	DWORD		dwObjectId;
 	WORD		wLevelId;
 	BYTE		unobserved_07[2];
};
typedef struct PLRMSGWARP PLRMSGWARP;

struct PLRMSGPETTARGET
{
 	BYTE		bCmd;
 	DWORD		dwPetId;
 	DWORD		dwUnitId;
 	DWORD		dwUnitType;
};
typedef struct PLRMSGPETTARGET PLRMSGPETTARGET;

struct PLRMSGPETMOVE
{
 	BYTE		bCmd;
 	DWORD		dwPetId;
 	WORD		wX;
 	BYTE		unobserved_07[2];
 	WORD		wY;
 	BYTE		unobserved_0b[2];
};
typedef struct PLRMSGPETMOVE PLRMSGPETMOVE;

struct PLRMSGTRADE
{
 	BYTE		bCmd;
 	WORD		wCmd;
 	WORD		wArgHigh;
 	WORD		wArgLow;
};
typedef struct PLRMSGTRADE PLRMSGTRADE;

struct PLRMSGGOLD
{
 	BYTE		bCmd;
 	DWORD		dwUnitId;
 	DWORD		dwAmount;
};
typedef struct PLRMSGGOLD PLRMSGGOLD;

struct PLRMSGHOTKEY
{
 	BYTE		bCmd;
 	WORD		wHotKey;
 	WORD		wSkill;
 	WORD		wLeft;
};
typedef struct PLRMSGHOTKEY PLRMSGHOTKEY;

struct PLRMSGPARTY
{
 	BYTE		bCmd;
 	BYTE		bCommand;
 	BYTE		bArg;
 	DWORD		dwUnitId;
};
typedef struct PLRMSGPARTY PLRMSGPARTY;

struct PLRMSGPARTYID
{
 	BYTE		bCmd;
 	BYTE		bCommand;
 	DWORD		dwUnitId;
};
typedef struct PLRMSGPARTYID PLRMSGPARTYID;

struct PLRMSGVAULT
{
 	BYTE		bCmd;
 	BYTE		bSubCmd;
 	DWORD		dwItemId;
 	BYTE		bY;
 	BYTE		bX;
};
typedef struct PLRMSGVAULT PLRMSGVAULT;

struct PLRMSGVAULTSWAP
{
 	BYTE		bCmd;
 	BYTE		bSubCmd;
 	DWORD		dwCursorId;
 	DWORD		dwItemId;
 	BYTE		bX;
 	BYTE		bY;
};
typedef struct PLRMSGVAULTSWAP PLRMSGVAULTSWAP;

struct PLRMSGGUILDTEXT
{
 	BYTE		bCmd;
 	BYTE		unobserved_01;
 	char		szText[256];
};
typedef struct PLRMSGGUILDTEXT PLRMSGGUILDTEXT;

struct PLRMSGGUILDSET
{
 	BYTE		bCmd;
 	BYTE		bSubCmd;
 	char		szText[258];
};
typedef struct PLRMSGGUILDSET PLRMSGGUILDSET;

struct PLRMSGGUILDSETTING
{
 	BYTE		bCmd;
 	BYTE		bIcon;
 	BYTE		bColor1;
 	BYTE		bColor2;
};
typedef struct PLRMSGGUILDSETTING PLRMSGGUILDSETTING;

struct PLRMSGGUILDMEMBER
{
 	BYTE		bCmd;
 	BYTE		bSubCmd;
 	int			nRank;
 	char		szName[16];
};
typedef struct PLRMSGGUILDMEMBER PLRMSGGUILDMEMBER;

struct PLRMSGNPCINTERACT
{
 	BYTE		bCmd;
 	DWORD		dwUnitType;
 	DWORD		dwUnitId;
 	DWORD		dwUnitX;
 	DWORD		dwUnitY;
};
typedef struct PLRMSGNPCINTERACT PLRMSGNPCINTERACT;
#pragma pack(pop)

typedef int (__fastcall *PFNPLRMSGPROCESS)(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize);
struct PLRMSGCHART
{
 	PFNPLRMSGPROCESS	pfnProcess;
};
typedef struct PLRMSGCHART PLRMSGCHART;

typedef void (__fastcall *PFNGUILDEVENT)(BYTE *pbTag, BYTE bEvent,
	const char *pszName, DWORD dwValue);

BOOL __fastcall ArenaUnitAlwaysFalse(GAME *ptGame, UNIT *hUnit);
CLIENT * __fastcall ClientGetNext(CLIENT *ptClient);
void __fastcall GameLeaveClient(CLIENT *ptClient, DWORD bReason);
void __fastcall SCmdSendSkill(UNIT *hUnit, WORD wSkill, BYTE bSkillLevel, BYTE bRemove);
int __fastcall SCmdSendText(CLIENT *ptClient, const char *pszText);
void __fastcall ObjectModeObeliskUse(GAME *ptGame, DWORD dwPlayerId, DWORD dwObjectId, DWORD dwItemId,
	WORD wCall);
void __fastcall ObjectModeTownPortalLink(GAME *ptGame, UNIT *hPlayer, DWORD dwPortalId, DWORD dwParam2);
BOOL __fastcall PlrEvalAddStatPoint(UNIT *hPlayer, int nStat);
void __fastcall PlrIntroClearIntroduced(GAME *ptGame, UNIT *hUnit, int nClass);
BOOL __fastcall PlrCanBeInterrupted(UNIT *hUnit, BOOL bCheckSkill, BOOL bCheckChance);
UNIT * __fastcall PlrSaveRecreateItem(GAME *ptGame, ITEMSAVE *ptItemSave);

typedef void (__fastcall *PFNSENDMODE)(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient, int nMode);
struct PLRMSGSENDMODE
{
 	PFNSENDMODE	pfnSendMode;
 	int			nClientMode;
 	int			nClientTargetMode;
};
typedef struct PLRMSGSENDMODE PLRMSGSENDMODE;

void __fastcall PlrModeSendSkill(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient, int nMode);
void __fastcall PlrMsgSendModeTarget(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient, int nMode);
void __fastcall PlrMsgSendModePath(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient, int nMode);
void __fastcall PlrMsgSendModeLife(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient, int nMode);
void __fastcall PlrMsgSendModeDeath(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient, int nMode);
void __fastcall PlrMsgSendModeOther(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient, int nMode);
static PLRMSGSENDMODE sgatSendModes[] =
{
	{ PlrMsgSendModeDeath, 8, 8 },
	{ PlrMsgSendModeOther, 7, 7 },
	{ PlrMsgSendModeTarget, 1, 0 },
	{ PlrMsgSendModeTarget, 23, 24 },
	{ PlrMsgSendModeLife, 6, 6 },
	{ PlrMsgSendModeOther, 7, 7 },
	{ PlrMsgSendModeTarget, 1, 0 },
	{ PlrModeSendSkill, 21, 22 },
	{ PlrModeSendSkill, 21, 22 },
	{ PlrMsgSendModeLife, 18, 18 },
	{ PlrModeSendSkill, 21, 22 },
	{ PlrModeSendSkill, 21, 22 },
	{ PlrModeSendSkill, 21, 22 },
	{ PlrModeSendSkill, 21, 22 },
	{ PlrModeSendSkill, 21, 22 },
	{ PlrModeSendSkill, 21, 22 },
	{ PlrModeSendSkill, 21, 22 },
	{ PlrMsgSendModeLife, 9, 9 },
	{ PlrModeSendSkill, 21, 22 },
	{ PlrMsgSendModePath, 20, 20 },
};

static int sganSentStats[] = { STATS_VELOCITYPERCENT, STATS_ATTACKRATE, STATS_LEVEL, -1 };

int __fastcall PlrMsgSkillAtXY(GAME *ptGame, UNIT *hPlayer, void *ptSkill, int nX, int nY);
void * __fastcall PlrMsgSwapSkill(UNIT *hUnit, void *ptSkill);
int __fastcall PlrMsgSkillAtUnit(GAME *ptGame, UNIT *hPlayer, void *ptSkill, DWORD dwUnitType,
	DWORD dwUnitId, BOOL bFlag);
BOOL __fastcall PlrMsgOutOfRange(UNIT *hUnit, WORD wX, WORD wY, int nRange);
int __fastcall PlrMsgCheckAct(UNIT *hUnit, UNIT *hOther);
int __fastcall PlrMsgCheckUnit(GAME *ptGame, UNIT *hPlayer, DWORD dwUnitType, DWORD dwUnitId, int nRange);
int __fastcall PlrMsgOnLeftSkillAtXY(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize);
int __fastcall PlrMsgOnLeftSkillAtUnitGo(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize);
int __fastcall PlrMsgOnLeftSkillAtUnit(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize);
int __fastcall PlrMsgOnRightSkillAtXY(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize);
int __fastcall PlrMsgOnRightSkillAtUnitGo(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize);
int __fastcall PlrMsgOnRightSkillAtUnit(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize);
UNIT * __fastcall PlrMsgCopyItem(GAME *ptGame, UNIT *hPlayer, UNIT *hItem);
int __fastcall PlrMsgVaultStack(GAME *ptGame, UNIT *hPlayer, UNIT *hVault, UNIT *hCursor, UNIT *hItem,
	int nQuantity);
void __fastcall PlrMsgGuildTextEach(CLIENT *ptClient, void *pArg);

static inline BOOL PlrMsgCheckSize(int nSize, int nExpected)
{
	if (nSize != nExpected)
	{
#line 2587
		PLRMSG_HACK("sent an invalid sized message", __LINE__);
		return FALSE;
	}
	return TRUE;
}

static inline BOOL PlrMsgHasLeftSkill(UNIT *hPlayer)
{
	if (!D2CommonUnitGetLeftSkill(hPlayer))
	{
#line 2726
		PLRMSG_HACK("tried to use invalid skill", __LINE__);
		return FALSE;
	}
	return TRUE;
}

static inline BOOL PlrMsgHasRightSkill(UNIT *hPlayer)
{
	if (!D2CommonUnitGetRightSkill(hPlayer))
	{
#line 2743
		PLRMSG_HACK("tried to use invalid skill", __LINE__);
		return FALSE;
	}
	return TRUE;
}

static inline int PlrMsgCheckUnitMsg(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize, DWORD *pdwUnitType,
	DWORD *pdwUnitId)
{
	PLRMSGUNIT *ptMsg;
	DWORD dwUnitId = 0, dwUnitType = 0;
	int nResult;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGUNIT)))
		return 3;
	ptMsg = (PLRMSGUNIT *)pMsg;
	dwUnitType = ptMsg->dwUnitType;
	dwUnitId = ptMsg->dwUnitId;
	if (dwUnitType >= NUM_UNIT_TYPES)
		return 2;
	nResult = PlrMsgCheckUnit(ptGame, hPlayer, dwUnitType, dwUnitId, PLRMSG_RANGE);
	if (nResult)
		return nResult;
	*pdwUnitType = dwUnitType;
	*pdwUnitId = dwUnitId;
	return 0;
}

void __fastcall PlrModeSendLife(UNIT *hUnit, CLIENT *ptClient)
{
	BYTE bLife;
	BYTE bField;
	int nMax;
	WORD wY;

	bField = (BYTE)hUnit->dwHitClass;
	bLife = 0;
	nMax = D2CommonStatsUnitCalcMaxHp(hUnit);
	if (nMax)
		bLife = (BYTE)(D2CommonStatsUnitGetValue(hUnit, 6) * 100 / nMax);

	SCmdSendUnitModeAt(ptClient, S2C_UNIT_MODE_POS, 0, hUnit->dwId, 19, (WORD)D2CommonUnitGetX(hUnit), ((WORD)D2CommonUnitGetY(hUnit)),
		bField, bLife);
}

void __fastcall PlrModeSendPosition(UNIT *hUnit, CLIENT *ptClient, BOOL fArg)
{
	SCmdSendUnit(ptClient, S2C_UNIT_WARP, (BYTE)hUnit->eType, hUnit->dwId,
		(WORD)D2CommonUnitGetX(hUnit), (WORD)D2CommonUnitGetY(hUnit), (BYTE)fArg);
}

void __fastcall PlrModeSendSkill(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient, int nMode)
{
	void *ptSkill;
	DWORD dwFlags;
	DWORD dwUnitId;
	UNIT *hTarget;
	DWORD dwSkillId;
	BYTE bLevel;
	WORD dwValue;
	WORD wY;

	ptSkill = (void *)D2CommonUnitGetSkillThirdCached(hUnit);
	dwFlags = D2CommonSkillsGetNodeFlags(ptSkill);
	if (!(dwFlags & 4) && hUnit == ClientGetPlayerUnit(ptClient, FALSE))
		return;
	dwUnitId = hUnit->dwId;
	hTarget = SUnitGetTarget(ptGame, hUnit);
	dwSkillId = D2CommonSkillsGetNodeType(ptSkill);
	bLevel = (BYTE)D2CommonSkillsCalcNodeLevel(hUnit, ptSkill, 1);
	dwValue = 0;
	if ((BYTE)dwFlags & 8)
		dwValue = SUnitDmgGetHeldFlags(hUnit, hTarget);
	else if ((BYTE)dwFlags & 16)
		dwValue = D2CommonSkillsGetNodeSlotRaw((DWORD)hUnit, (DWORD)ptSkill);
	if (hTarget && ClientSeesUnit(hTarget, ptClient))
	{
		SCmdSendUnitAt(ptClient, 0, dwUnitId, (BYTE)hTarget->eType, hTarget->dwId, (WORD)dwSkillId,
			(WORD)dwValue, bLevel);
		return;
	}

	SCmdSendTargetPoint(ptClient, 0, dwUnitId, D2CommonPathGetTargetX(hUnit->ptPath), D2CommonPathGetTargetY(hUnit->ptPath), (WORD)dwSkillId,
		(WORD)dwValue, bLevel);
}

void __fastcall PlrMsgSendModeTarget(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient, int nMode)
{
	UNIT *hTarget;
	void *ptPath;

	if (hUnit == ClientGetPlayerUnit(ptClient, FALSE))
		return;
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget && ClientSeesUnit(hTarget, ptClient))
	{
		SCmdSendUnitModeToUnit(ptClient, S2C_UNIT_MODE_POS3, UNIT_PLAYER, hUnit->dwId,
			(BYTE)sgatSendModes[nMode].nClientTargetMode,
			(BYTE)hTarget->eType, hTarget->dwId, (WORD)D2CommonUnitGetX(hUnit),
			(WORD)D2CommonUnitGetY(hUnit));
		return;
	}
	ptPath = hUnit->ptPath;
	SCmdSendUnitModeToPoint(ptClient, S2C_UNIT_MODE_POS2, UNIT_PLAYER, hUnit->dwId,
		(BYTE)sgatSendModes[nMode].nClientMode,
		D2CommonPathGetTargetX(ptPath), D2CommonPathGetTargetY(ptPath), 0,
		(WORD)D2CommonUnitGetX(hUnit), (WORD)D2CommonUnitGetY(hUnit));
}

void __fastcall PlrMsgSendModePath(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient, int nMode)
{
	BYTE bHitClass;
	void *ptPath;

	bHitClass = (BYTE)hUnit->dwHitClass;
	ptPath = hUnit->ptPath;
	SCmdSendUnitModeToPoint(ptClient, S2C_UNIT_MODE_POS2, UNIT_PLAYER, hUnit->dwId,
		(BYTE)sgatSendModes[nMode].nClientMode,
		D2CommonPathGetTargetX(ptPath), D2CommonPathGetTargetY(ptPath), bHitClass,
		(WORD)D2CommonUnitGetX(hUnit), (WORD)D2CommonUnitGetY(hUnit));
}

void __fastcall PlrMsgSendModeLife(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient, int nMode)
{
	BYTE bHitClass;
	BYTE bLife;
	int nSentMode;
	int nMaxLife;

	nSentMode = sgatSendModes[nMode].nClientMode;
	bHitClass = (BYTE)hUnit->dwHitClass;
	bLife = 0;
	nMaxLife = D2CommonStatsUnitCalcMaxHp(hUnit);
	if (nMaxLife)
		bLife = (BYTE)(D2CommonStatsUnitGetValue(hUnit, STATS_HITPOINTS) * 100 / nMaxLife);
	SCmdSendUnitModeAt(ptClient, S2C_UNIT_MODE_POS, UNIT_PLAYER, hUnit->dwId, (BYTE)nSentMode,
		(WORD)D2CommonUnitGetX(hUnit), (WORD)D2CommonUnitGetY(hUnit), bHitClass, bLife);
}

void __fastcall PlrMsgSendModeDeath(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient, int nMode)
{
	BYTE bHitClass;
	BYTE bLife;
	int nSentMode;
	int nMaxLife;

	nSentMode = sgatSendModes[nMode].nClientMode;
	bHitClass = (BYTE)hUnit->dwHitClass;
	bLife = 0;
	nMaxLife = D2CommonStatsUnitCalcMaxHp(hUnit);
	if (nMaxLife)
		bLife = (BYTE)(D2CommonStatsUnitGetValue(hUnit, STATS_HITPOINTS) * 100 / nMaxLife);
	SCmdSendUnitModeAt(ptClient, S2C_UNIT_MODE_POS, UNIT_PLAYER, hUnit->dwId, (BYTE)nSentMode,
		(WORD)D2CommonUnitGetX(hUnit), (WORD)D2CommonUnitGetY(hUnit), bHitClass, bLife);
	if (hUnit == ClientGetPlayerUnit(ptClient, FALSE))
		PlrMsgStatChanged(hUnit, STATS_AF, D2CommonStatsUnitGetValue(hUnit, STATS_AF), hUnit);
}

void __fastcall PlrMsgSendModeOther(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient, int nMode)
{
	BYTE bLife;
	int nClientMode;
	int nMaxLife;

	if (hUnit == ClientGetPlayerUnit(ptClient, FALSE))
		return;
	bLife = 0;
	nMaxLife = D2CommonStatsUnitCalcMaxHp(hUnit);
	if (nMaxLife)
		bLife = (BYTE)(D2CommonStatsUnitGetValue(hUnit, STATS_HITPOINTS) * 100 / nMaxLife);
	nClientMode = sgatSendModes[nMode].nClientMode;
	SCmdSendUnitModeAt(ptClient, S2C_UNIT_MODE_POS, UNIT_PLAYER, hUnit->dwId,
		nClientMode,
		(WORD)D2CommonUnitGetX(hUnit), (WORD)D2CommonUnitGetY(hUnit), 0, bLife);
}

void __fastcall PlrMsgSendMode(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient)
{
	int nMode;
	PFNSENDMODE pfnSendMode;

	nMode = (int)hUnit->dwMode;
	pfnSendMode = sgatSendModes[nMode].pfnSendMode;
	if (pfnSendMode)
	{
#line 307
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pfnSendMode), "pfnSendMode");
		pfnSendMode(ptGame, hUnit, ptClient, nMode);
	}
}

void __fastcall PlrMsgStatChanged(UNIT *hUnit, int nStat, int nValue, UNIT *hPlayer)
{
	CLIENT *hClient;

	hClient = SUnitGetClient(hPlayer);
#line 319
	D2ASSERT(hClient);
	SCmdSendStat(hClient, S2C_SET_STAT, UNIT_PLAYER, hUnit->dwId, 0, (BYTE)nStat, (DWORD)nValue);
}

static inline BYTE PlrMsgManaPercent(UNIT *hUnit)
{
	CHARSTATSTXT *ptPlayerInitStats;
	void *ptStatList;
	GAME *ptGame;
	int nMana, nMaxMana;
	int nFrames, nRegen, nRecovery;
	BYTE bMana;

	nMana = D2CommonStatsUnitGetValue(hUnit, STATS_MANA);
	nMaxMana = D2CommonStatsUnitCalcMaxMana(hUnit);
	ptStatList = (void *)D2CommonStatsFindListByState(hUnit, STATE_6A);
	if (!ptStatList || !nMaxMana)
		return 0;
	ptPlayerInitStats = (CHARSTATSTXT *)D2CommonDataTblsGetCharStats(hUnit->dwClassId);
#line 359
	D2ASSERT(ptPlayerInitStats);
	nFrames = ptPlayerInitStats->ManaRegen * 25;
	if (!nFrames)
		nFrames = 7500;
	if (nMaxMana / nFrames < 1)
		nRegen = 1;
	else
		nRegen = nMaxMana / nFrames;
	nRecovery = D2CommonStatsUnitGetValue(hUnit, STATS_MANARECOVERY);
	if (hUnit->dwClassId == PLAYER_SORCERESS && D2CommonStatsGetState(hUnit, STATE_46))
		nRecovery += D2CommonSkillsCalcParamOneByLevel(SKILL_WARMTH, D2CommonSkillsCalcNodeLevel(hUnit,
			D2CommonSkillsFindByKey(hUnit, 37), 1));
	nRecovery = (nRecovery + 100) * nRegen / 100 + D2CommonStatsUnitGetValue(hUnit,
		STATS_MANARECOVERYBONUS);
	ptGame = SUnitGetGame(hUnit);
	bMana = (BYTE)((((int)D2CommonStatsGetListEndFrame(ptStatList) - ptGame->nFrame) * nRecovery + nMana) *
		100 / nMaxMana);
	if (bMana > 100)
		return 100;
	return bMana;
}

static inline BYTE PlrMsgLifePercent(UNIT *hUnit)
{
	void *ptStatList;
	GAME *ptGame;
	int nLife, nMaxLife;
	int nFrames;
	BYTE bLife;

	nLife = D2CommonStatsUnitGetValue(hUnit, STATS_HITPOINTS);
	nMaxLife = D2CommonStatsUnitCalcMaxHp(hUnit);
	ptStatList = (void *)D2CommonStatsFindListByState(hUnit, STATE_64);
	if (!ptStatList || !nMaxLife)
		return 0;
	ptGame = SUnitGetGame(hUnit);
	nFrames = (int)D2CommonStatsGetListEndFrame(ptStatList) - ptGame->nFrame;
	bLife = (BYTE)(((int)D2CommonStatsListGetValueA(ptStatList, STATS_4A) * nFrames + nLife) * 100 /
		nMaxLife);
	if (bLife > 100)
		return 100;
	return bLife;
}

void __fastcall PlrMsgSendLifeMana(UNIT *hUnit, UNIT *hPlayer)
{
	CLIENT *hClient;
	void *ptPath;
	int nPathX, nPathY;
	BYTE bMana, bLife;

	hClient = SUnitGetClient(hPlayer);
#line 395
	D2ASSERT(hClient);
	ptPath = hUnit->ptPath;
	nPathX = 0;
	nPathY = 0;
	if (ptPath)
	{
		nPathX = (int)D2CommonPathGetTargetXRaw((DWORD)ptPath);
		nPathY = (int)D2CommonPathGetTargetYRaw((DWORD)ptPath);
	}
	bMana = PlrMsgManaPercent(hUnit);
	bLife = PlrMsgLifePercent(hUnit);
	SCmdSendPlayerStatsPacked(hClient, hUnit->dwId, D2CommonStatsUnitGetValue(hUnit, STATS_HITPOINTS),
		D2CommonStatsUnitGetValue(hUnit, STATS_MANA), D2CommonStatsUnitGetValue(hUnit, STATS_STAMINA),
		bLife, bMana, (WORD)D2CommonUnitGetX(hUnit), (WORD)D2CommonUnitGetY(hUnit),
		(WORD)nPathX, (WORD)nPathY);
}

void __fastcall PlrMsgSendStats(UNIT *hUnit, CLIENT *ptClient)
{
	UNIT *hPlayer;
	int i, nStat;

	hPlayer = ClientGetPlayerUnit(ptClient, FALSE);
	if (hPlayer == hUnit)
		return;
	for (i = 0; sganSentStats[i] != -1; i++)
	{
		nStat = sganSentStats[i];
		PlrMsgStatChanged(hUnit, nStat, D2CommonStatsUnitGetBaseValue(hUnit, nStat), hPlayer);
	}
}

int __fastcall PlrMsgInteract(UNIT *hPlayer, DWORD dwUnitType, DWORD dwUnitId, BOOL bFlag, GAME *ptGame)
{
	UNIT *hUnit;
	MISSOBJECTSTXT *ptObjectData;
	MONSTATSTXT *ptMonStats;
	int nDistance;
	int nMode;
	void *hRoom;

	switch (dwUnitType)
	{
	case UNIT_OBJECT:
		hUnit = SUnitFind(ptGame, UNIT_OBJECT, dwUnitId);
		if (!hUnit)
		{
#line 504
			PLRMSG_HACK("operated an invalid object", __LINE__);
			return 1;
		}
		ptObjectData = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hUnit->dwClassId);
#line 509
		D2ASSERT(ptObjectData);
		nDistance = (int)D2CommonUnitCalcEdgeDistance(hPlayer, hUnit);
		if (nDistance > 50)
			return 1;
		if (nDistance <= ptObjectData->OperateRange
			&& !D2CommonUnitTestLineBlockedIgnoring(hPlayer, hUnit, 2052))
		{
			ObjectModeOperateInRange(ptGame, hPlayer, UNIT_OBJECT, dwUnitId);
			return 0;
		}
		nMode = 3;
		hRoom = (void *)D2CommonUnitGetPathRoom(hPlayer);
		if (hRoom && D2CommonDungeonRoomIsTown(hRoom))
			nMode = 6;
		PlrModeStartID(ptGame, hPlayer, NULL, nMode, UNIT_OBJECT, dwUnitId, FALSE);
		D2CommonUnitUtilSetTestCallback(hPlayer, bFlag ? -2 : -1, UNIT_OBJECT, dwUnitId);
		return 0;
	case UNIT_ROOMTILE:
		hUnit = SUnitFind(ptGame, UNIT_ROOMTILE, dwUnitId);
		if (!hUnit)
		{
#line 533
			PLRMSG_HACK("operated an invalid warp", __LINE__);
			return 1;
		}
		nDistance = (int)D2CommonUnitCalcEdgeDistance(hPlayer, hUnit);
		if (nDistance > 50)
			return 1;
		if (nDistance > 4)
		{
			nMode = 3;
			hRoom = (void *)D2CommonUnitGetPathRoom(hPlayer);
			if (hRoom && D2CommonDungeonRoomIsTown(hRoom))
				nMode = 6;
			PlrModeStartID(ptGame, hPlayer, NULL, nMode, UNIT_ROOMTILE, dwUnitId, FALSE);
			D2CommonUnitUtilSetTestCallback(hPlayer, bFlag ? -2 : -1, UNIT_ROOMTILE, dwUnitId);
			return 0;
		}
		SUnitWarp(ptGame, hPlayer, hUnit);
		return 0;
	case UNIT_ITEM:
		hUnit = SUnitFind(ptGame, UNIT_ITEM, dwUnitId);
		if (!hUnit)
			return 1;
		if (hUnit->dwMode != IMODE_ONGROUND)
			return 1;
		nDistance = (int)D2CommonUnitCalcEdgeDistance(hPlayer, hUnit);
		if (nDistance > 50)
			return 1;
		if (nDistance <= 4 && !D2CommonUnitTestLineBlockedIgnoring(hPlayer, hUnit, 2052))
		{ int nResult;
			if (bFlag)
			{
				ITEMSModePickupToCursor(ptGame, hPlayer, dwUnitId, &nResult);
				if (nResult)
#line 584
					PLRMSG_HACK("itemspickupground", __LINE__);
			}
			else
			{
				ITEMSModePickupToInventory(ptGame, hPlayer, dwUnitId, &nResult);
				if (nResult)
#line 590
					PLRMSG_HACK("itemsautopickupground", __LINE__);
			}
			if (nResult)
				return 3;
		}
		else
		{
			nMode = 3;
			hRoom = (void *)D2CommonUnitGetPathRoom(hPlayer);
			if (hRoom && D2CommonDungeonRoomIsTown(hRoom))
				nMode = 6;
			PlrModeStartID(ptGame, hPlayer, NULL, nMode, UNIT_ITEM, dwUnitId, FALSE);
			D2CommonUnitUtilSetTestCallback(hPlayer, bFlag ? -2 : -1, UNIT_ITEM, dwUnitId);
		}
		return 0;
	case UNIT_MONSTER:
		hUnit = SUnitFind(ptGame, UNIT_MONSTER, dwUnitId);
		if (!hUnit)
			return 1;
		nDistance = (int)D2CommonUnitCalcEdgeDistance(hPlayer, hUnit);
		if (nDistance > 50)
			return 1;
		ptMonStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId);
		if (ptMonStats && ptMonStats->bInteract && ptMonStats->bIsNPC)
		{
			D2CommonPathClearSteps(hUnit->ptPath);
			AiSetParam(hUnit, 1, 40);
			EventAdd(ptGame, hUnit, 2, ptGame->nFrame + 1);
		}
		if (nDistance > 6)
		{
			if (nDistance >= 9)
				break;
			nMode = 3;
			hRoom = (void *)D2CommonUnitGetPathRoom(hPlayer);
			if (hRoom && D2CommonDungeonRoomIsTown(hRoom))
				nMode = 6;
			PlrModeStartID(ptGame, hPlayer, NULL, nMode, UNIT_MONSTER, dwUnitId, FALSE);
			D2CommonUnitUtilSetTestCallback(hPlayer, bFlag ? -2 : -1, UNIT_MONSTER, dwUnitId);
		}
		else if (!PlayerIsBusy(hPlayer))
			MonsterAIAddThreat(ptGame, hPlayer, hUnit, ((MONSTERDATA *)hUnit->uTypeData)->pptThreats, 0);
		return 0;
	case UNIT_PLAYER:
		hUnit = SUnitFind(ptGame, UNIT_PLAYER, dwUnitId);
		if (!hUnit)
			return 1;
		nDistance = (int)D2CommonUnitCalcEdgeDistance(hPlayer, hUnit);
		if ((DWORD)nDistance > 50)
			return 1;
		if ((DWORD)nDistance > 8)
		{
			nMode = 3;
			hRoom = (void *)D2CommonUnitGetPathRoom(hPlayer);
			if (hRoom && D2CommonDungeonRoomIsTown(hRoom))
				nMode = 6;
			PlrModeStartID(ptGame, hPlayer, NULL, nMode, UNIT_PLAYER, dwUnitId, FALSE);
			D2CommonUnitUtilSetTestCallback(hPlayer, bFlag ? -2 : -1, UNIT_PLAYER, dwUnitId);
			return 0;
		}
		if (hUnit->dwMode == PMODE_DEAD)
		{
			PlrModePickupCorpse(ptGame, hPlayer, hUnit);
			return 0;
		}
		PlrSaveRequestTrade(ptGame, hPlayer, hUnit);
		break;
	default:
		return 1;
	}
	return 0;
}

int __fastcall PlrMsgOnWalkToXY(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGXY *ptMsg;
	WORD wX, wY;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGXY)))
		return 3;
	ptMsg = (PLRMSGXY *)pMsg;
	wX = ptMsg->wX;
	wY = ptMsg->wY;
	if (PlrMsgOutOfRange(hPlayer, wX, wY, PLRMSG_RANGE))
		return 1;
	PlrModeStartXY(ptGame, hPlayer, NULL, 2, wX, wY, FALSE);
	return 0;
}

int __fastcall PlrMsgOnWalkToUnit(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwUnitType, dwUnitId;
	int nResult;

	nResult = PlrMsgCheckUnitMsg(ptGame, hPlayer, pMsg, nSize, &dwUnitType, &dwUnitId);
	if (nResult)
		return nResult;
	PlrModeStartID(ptGame, hPlayer, NULL, 2, dwUnitType, dwUnitId, FALSE);
	return 0;
}

int __fastcall PlrMsgOnRunToXY(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGXY *ptMsg;
	WORD wX, wY;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGXY)))
		return 3;
	ptMsg = (PLRMSGXY *)pMsg;
	wX = ptMsg->wX;
	wY = ptMsg->wY;
	if (PlrMsgOutOfRange(hPlayer, wX, wY, PLRMSG_RANGE))
		return 1;
	PlrModeStartXY(ptGame, hPlayer, NULL, 3, wX, wY, FALSE);
	return 0;
}

int __fastcall PlrMsgOnRunToUnit(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwUnitType, dwUnitId;
	int nResult;

	nResult = PlrMsgCheckUnitMsg(ptGame, hPlayer, pMsg, nSize, &dwUnitType, &dwUnitId);
	if (nResult)
		return nResult;
	PlrModeStartID(ptGame, hPlayer, NULL, 3, dwUnitType, dwUnitId, FALSE);
	return 0;
}

int __fastcall PlrMsgOnLeftSkillAtXY(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGXY *ptMsg;
	WORD wX, wY;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGXY)))
		return 3;
	ptMsg = (PLRMSGXY *)pMsg;
	wX = ptMsg->wX;
	wY = ptMsg->wY;
	if (PlrMsgOutOfRange(hPlayer, wX, wY, PLRMSG_RANGE))
		return 1;
	if (PlrMsgHasLeftSkill(hPlayer))
	{
		PlrMsgSkillAtXY(ptGame, hPlayer, (void *)D2CommonUnitGetLeftSkill(hPlayer), wX, wY);
		return 0;
	}
	return 3;
}

int __fastcall PlrMsgSkillAtXY(GAME *ptGame, UNIT *hPlayer, void *ptSkill, int nX, int nY)
{
	GAMEPLAYERMSG tMsg;
	SKILLSTXT *ptSkillsTxt;
	int nCost;
	int nMode;
	int i;

	if (hPlayer->dwMode == PMODE_DEATH || hPlayer->dwMode == PMODE_DEAD)
		return 2;
	if (hPlayer->dwClassId == PLAYER_BARBARIAN)
		ptSkill = PlrMsgSwapSkill(hPlayer, ptSkill);
	nCost = D2CommonSkillsTestUsable(hPlayer, ptSkill);
	ptSkillsTxt = (SKILLSTXT *)D2CommonSkillsGetNodeTypeRecord(ptSkill);
	if (nCost > 0 && nCost <= 2 && ptSkillsTxt->AttackNoMana)
	{
		ptSkill = (void *)D2CommonSkillsFindByKey(hPlayer, 0);
		nCost = D2CommonSkillsTestUsable(hPlayer, ptSkill);
	}
	nMode = (int)D2CommonSkillsGetNodeModeIfAny(hPlayer, ptSkill);
	if (!nMode)
		return 2;
	if (nCost)
	{
		if (nCost == 1)
		{
			for (i = 0; i < (int)(sizeof(tMsg) / sizeof(DWORD)); i++)
				((DWORD *)&tMsg)[i] = 0;
			tMsg.bCmd = S2C_ROSTER_EVENT;
			tMsg.bColor = (BYTE)nCost;
			tMsg.dwArg = 0;
			tMsg.bEvent = 14;
			SCmdSendSystemMsg(SUnitGetClient(hPlayer), (SCMDSYSTEMMSG *)&tMsg);
			return 2;
		}
#line 3387
		PLRMSG_HACK("tried to use a skill for which the requirements weren't met", __LINE__);
		return 3;
	}
	PlrModeStartXY(ptGame, hPlayer, ptSkill, nMode, nX, nY, FALSE);
	return 0;
}

void * __fastcall PlrMsgSwapSkill(UNIT *hUnit, void *ptSkill)
{
	void *hSwapSkill;
	void *ptInventory;
	UNIT *hRight, *hLeft;

	hSwapSkill = ptSkill;
	if (!D2CommonSkillsGetNodeType(ptSkill))
	{
		ptInventory = hUnit->ptInventory;
		hRight = (UNIT *)D2CommonINVENTORYGetLocSlot(ptInventory, 4);
		if (hRight && (!D2CommonITEMSTestEffectsActive(hRight) || D2CommonITEMSGetType(hRight) == 38))
			hRight = NULL;
		hLeft = (UNIT *)D2CommonINVENTORYGetLocSlot(ptInventory, 5);
		if (hLeft && (!D2CommonITEMSTestEffectsActive(hLeft) || D2CommonITEMSGetType(hLeft) == 38))
			hLeft = NULL;
		if (hRight && hLeft && D2CommonITEMSIsWeapon(hRight) && D2CommonITEMSIsWeapon(hLeft))
		{
			if (!D2CommonSkillsGetNodeScratchF(ptSkill))
			{
				hSwapSkill = (void *)D2CommonSkillsFindByKey(hUnit, 5);
#line 3310
				D2ASSERT(hSwapSkill);
				D2CommonSkillsSetNodeScratchF(ptSkill, 5);
			}
			else
				D2CommonSkillsSetNodeScratchF(ptSkill, 0);
		}
	}
	return hSwapSkill;
}

int __fastcall PlrMsgOnLeftSkillAtUnitGo(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwUnitType, dwUnitId;
	int nResult;

	nResult = PlrMsgCheckUnitMsg(ptGame, hPlayer, pMsg, nSize, &dwUnitType, &dwUnitId);
	if (nResult)
		return nResult;
	if (!PlrMsgHasLeftSkill(hPlayer))
		return 3;
	PlrMsgSkillAtUnit(ptGame, hPlayer, (void *)D2CommonUnitGetLeftSkill(hPlayer), dwUnitType, dwUnitId, TRUE);
	return 0;
}

int __fastcall PlrMsgSkillAtUnit(GAME *ptGame, UNIT *hPlayer, void *ptSkill, DWORD dwUnitType,
	DWORD dwUnitId, BOOL bFlag)
{
	SKILLSTXT *ptSkillsTxt;
	UNIT *hUnit;
	int nCost;
	int nMode;
	int nSkillId;
	void *hRoom;
	BOOL bStarted;

	if (hPlayer->dwClassId == PLAYER_BARBARIAN)
		ptSkill = PlrMsgSwapSkill(hPlayer, ptSkill);
	nCost = D2CommonSkillsTestUsable(hPlayer, ptSkill);
	ptSkillsTxt = (SKILLSTXT *)D2CommonSkillsGetNodeTypeRecord(ptSkill);
	if (nCost > 0 && nCost <= 2 && ptSkillsTxt->AttackNoMana)
	{
		ptSkill = (void *)D2CommonSkillsFindByKey(hPlayer, 0);
		nCost = D2CommonSkillsTestUsable(hPlayer, ptSkill);
	}
	if (nCost)
		return 2;
	nMode = (int)D2CommonSkillsGetNodeModeIfAny(hPlayer, ptSkill);
	if (!nMode)
		return 2;
	if (D2CommonSkillsGetNodeType(ptSkill) != 43 && D2CommonSkillsGetNodeType(ptSkill) != 90
		&& ((int)dwUnitType < UNIT_PLAYER || ((int)dwUnitType > UNIT_MONSTER && dwUnitType != UNIT_MISSILE)))
		return 2;
	bStarted = FALSE;
	if (bFlag)
	{
		hUnit = SUnitFind(ptGame, dwUnitType, dwUnitId);
		if (hUnit)
		{
			switch (D2CommonSkillsGetNodeRangeForUnit(hPlayer, ptSkill))
			{
			case 1:
				if (D2CommonUnitTestPositionClear(hPlayer, hUnit, D2CommonUnitGetThreeIfInModeGroup(hUnit)))
					break;
			case 4:
				nSkillId = (int)D2CommonSkillsGetNodeType(ptSkill);
				nMode = 3;
				hRoom = (void *)D2CommonUnitGetPathRoom(hPlayer);
				if (hRoom && D2CommonDungeonRoomIsTown(hRoom))
					nMode = 6;
				PlrModeStartID(ptGame, hPlayer, NULL, nMode, dwUnitType, dwUnitId, FALSE);
				D2CommonUnitUtilSetTestCallback(hPlayer, nSkillId, dwUnitType, dwUnitId);
				bStarted = TRUE;
			}
		}
	}
	if (!bStarted)
		PlrModeStartID(ptGame, hPlayer, ptSkill, nMode, dwUnitType, dwUnitId, FALSE);
	return 0;
}

int __fastcall PlrMsgOnLeftSkillAtUnit(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwUnitType, dwUnitId;
	int nResult;

	nResult = PlrMsgCheckUnitMsg(ptGame, hPlayer, pMsg, nSize, &dwUnitType, &dwUnitId);
	if (nResult)
		return nResult;
	if (!PlrMsgHasLeftSkill(hPlayer))
		return 3;
	PlrMsgSkillAtUnit(ptGame, hPlayer, (void *)D2CommonUnitGetLeftSkill(hPlayer), dwUnitType, dwUnitId, FALSE);
	return 0;
}

int __fastcall PlrMsgOnLeftSkillAtXYAgain(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGXY *ptMsg;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGXY)))
		return 3;
	ptMsg = (PLRMSGXY *)pMsg;
	if (PlrMsgOutOfRange(hPlayer, ptMsg->wX, ptMsg->wY, PLRMSG_RANGE))
		return 1;
	if (PlrMsgHasLeftSkill(hPlayer))
	{
		PlrMsgOnLeftSkillAtXY(ptGame, hPlayer, pMsg, nSize);
		return 0;
	}
	return 3;
}

int __fastcall PlrMsgOnLeftSkillAtUnitGoAgain(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwUnitType, dwUnitId;
	int nResult;

	nResult = PlrMsgCheckUnitMsg(ptGame, hPlayer, pMsg, nSize, &dwUnitType, &dwUnitId);
	if (nResult)
		return nResult;
	if (!PlrMsgHasLeftSkill(hPlayer))
		return 3;
	else
	{
		PlrMsgOnLeftSkillAtUnitGo(ptGame, hPlayer, pMsg, nSize);
		return 0;
	}
}

BOOL __fastcall PlrMsgOutOfRange(UNIT *hUnit, WORD wX, WORD wY, int nRange)
{
	int nX, nY;

	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	if (ABS(nX - wX) > nRange || ABS(nY - wY) > nRange)
		return TRUE;
	return FALSE;
}

int __fastcall PlrMsgCheckAct(UNIT *hUnit, UNIT *hOther)
{
	DWORD hRoom;
	int nLevelId;
	BYTE bAct;

	hRoom = D2CommonUnitGetPathRoom(hUnit);
	nLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
	bAct = D2CommonGetActNum(nLevelId);
	hRoom = D2CommonUnitGetPathRoom(hOther);
	nLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
	if (bAct != D2CommonGetActNum(nLevelId))
		return 2;
	return 0;
}

int __fastcall PlrMsgCheckUnit(GAME *ptGame, UNIT *hPlayer, DWORD dwUnitType, DWORD dwUnitId, int nRange)
{
	UNIT *hUnit;
	WORD wX, wY;
	int nResult;

	hUnit = SUnitFind(ptGame, dwUnitType, dwUnitId);
	if (!hUnit)
		nResult = 1;
	else if (dwUnitType == UNIT_ITEM ? ITEMSIsInInventory(hPlayer, hUnit, NULL) : FALSE)
		return 0;
	else
	{
		nResult = PlrMsgCheckAct(hPlayer, hUnit);
		if (nResult)
			return nResult;
		wX = (WORD)D2CommonUnitGetX(hUnit);
		wY = (WORD)D2CommonUnitGetY(hUnit);
		nResult = PlrMsgOutOfRange(hPlayer, wX, wY, nRange);
		return nResult;
	}
	return nResult;
}

int __fastcall PlrMsgOnLeftSkillAtUnitAgain(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwUnitType, dwUnitId;
	int nResult;

	nResult = PlrMsgCheckUnitMsg(ptGame, hPlayer, pMsg, nSize, &dwUnitType, &dwUnitId);
	if (nResult)
		return nResult;
	if (!PlrMsgHasLeftSkill(hPlayer))
		return 3;
	else
	{
		PlrMsgOnLeftSkillAtUnit(ptGame, hPlayer, pMsg, nSize);
		return 0;
	}
}

int __fastcall PlrMsgOnEmpty(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	if (!PlrMsgCheckSize(nSize, 1))
		return 3;
	return 0;
}

int __fastcall PlrMsgOnRightSkillAtXY(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGXY *ptMsg;
	WORD wX, wY;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGXY)))
		return 3;
	ptMsg = (PLRMSGXY *)pMsg;
	wX = ptMsg->wX;
	wY = ptMsg->wY;
	if (PlrMsgOutOfRange(hPlayer, wX, wY, PLRMSG_RANGE))
		return 1;
	if (PlrMsgHasRightSkill(hPlayer))
	{
		PlrMsgSkillAtXY(ptGame, hPlayer, (void *)D2CommonUnitGetRightSkill(hPlayer), wX, wY);
		return 0;
	}
	return 3;
}

int __fastcall PlrMsgOnRightSkillAtUnitGo(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwUnitType, dwUnitId;
	int nResult;

	nResult = PlrMsgCheckUnitMsg(ptGame, hPlayer, pMsg, nSize, &dwUnitType, &dwUnitId);
	if (nResult)
		return nResult;
	if (!PlrMsgHasRightSkill(hPlayer))
		return 3;
	PlrMsgSkillAtUnit(ptGame, hPlayer, (void *)D2CommonUnitGetRightSkill(hPlayer), dwUnitType, dwUnitId, TRUE);
	return 0;
}

int __fastcall PlrMsgOnRightSkillAtUnit(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwUnitType, dwUnitId;
	int nResult;

	nResult = PlrMsgCheckUnitMsg(ptGame, hPlayer, pMsg, nSize, &dwUnitType, &dwUnitId);
	if (nResult)
		return nResult;
	if (!PlrMsgHasRightSkill(hPlayer))
		return 3;
	PlrMsgSkillAtUnit(ptGame, hPlayer, (void *)D2CommonUnitGetRightSkill(hPlayer), dwUnitType, dwUnitId, FALSE);
	return 0;
}

int __fastcall PlrMsgOnRightSkillAtXYAgain(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGXY *ptMsg;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGXY)))
		return 3;
	ptMsg = (PLRMSGXY *)pMsg;
	if (PlrMsgOutOfRange(hPlayer, ptMsg->wX, ptMsg->wY, PLRMSG_RANGE))
		return 1;
	if (PlrMsgHasRightSkill(hPlayer))
	{
		PlrMsgOnRightSkillAtXY(ptGame, hPlayer, pMsg, nSize);
		return 0;
	}
	return 3;
}

int __fastcall PlrMsgOnRightSkillAtUnitGoAgain(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwUnitType, dwUnitId;
	int nResult;

	nResult = PlrMsgCheckUnitMsg(ptGame, hPlayer, pMsg, nSize, &dwUnitType, &dwUnitId);
	if (nResult)
		return nResult;
	if (!PlrMsgHasRightSkill(hPlayer))
		return 3;
	else
	{
		PlrMsgOnRightSkillAtUnitGo(ptGame, hPlayer, pMsg, nSize);
		return 0;
	}
}

int __fastcall PlrMsgOnRightSkillAtUnitAgain(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwUnitType, dwUnitId;
	int nResult;

	nResult = PlrMsgCheckUnitMsg(ptGame, hPlayer, pMsg, nSize, &dwUnitType, &dwUnitId);
	if (nResult)
		return nResult;
	if (!PlrMsgHasRightSkill(hPlayer))
		return 3;
	else
	{
		PlrMsgOnRightSkillAtUnit(ptGame, hPlayer, pMsg, nSize);
		return 0;
	}
}

int __fastcall PlrMsgOnClearStateBit(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	if (!PlrMsgCheckSize(nSize, 1))
		return 3;
	D2CommonStatsUnitSetActiveStateBit(hPlayer, 12, 0);
	return 0;
}

int __fastcall PlrMsgOnUnitText(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGCHAT *ptMsg;
	void *ptNode;
	int nFrame;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGCHAT)))
		return 3;
	if (hPlayer->ptChatNode)
		D2CommonChatNodeFree(ptGame->pMemPool, hPlayer->ptChatNode);
	ptMsg = (PLRMSGCHAT *)pMsg;
	PLRMSGCHAT *ptChat = ptMsg;
	PLRMSGCHAT *ptChatName = ptChat;
	ptChatName->szName[15] = 0;
	if (ptMsg->szText[255]) {
#line 3768
		PLRMSG_HACK("tried to send invalid chat message", __LINE__);
		return 2; }
	ptNode = (void *)D2CommonChatNodeNew(ptGame->pMemPool, ptMsg->szText,
		ptGame->nFrame);
	D2CommonWaypointSetChatNodeMessageByte(ptNode, ptMsg->field_02);
	hPlayer->ptChatNode = ptNode;
	D2CommonUnitRoomAddToChangedList(hPlayer);
	hPlayer->dwFlags0 |= 256;
	nFrame = (int)D2CommonChatNodeGetEndFrame(ptNode);
	if (ptGame->nFrame + 1 > nFrame)
		nFrame = ptGame->nFrame + 1;
	EventAdd(ptGame, hPlayer, 6, nFrame);
	return 0;
}

void __fastcall PlrMsgChatEach(CLIENT *ptClient, void *pArg)
{
	PLRMSGWHISPER *ptWhisper;
	SCMDCHATDATA *pChatData;
	GAME *ptGame;
	UNIT *hSpeaker, *hPlayer;

	ptWhisper = (PLRMSGWHISPER *)pArg;
	ptGame = ClientGetGame(ptClient);
	pChatData = ptWhisper->pChatData;
	if (ptWhisper->dwSpeakerId != (DWORD)-1)
	{
		hSpeaker = SUnitFind(ptGame, UNIT_PLAYER, ptWhisper->dwSpeakerId);
		hPlayer = ClientGetPlayerUnit(ptClient, FALSE);
		if (hSpeaker && hPlayer
			&& (PlayerListTestFlag(hPlayer, hSpeaker, 4) || PlayerListTestFlag(hSpeaker, hPlayer, 2)))
			return;
	}
	SCmdSendChat(ptClient, pChatData);
}

void __fastcall PlrMsgWhisperEach(CLIENT *ptClient, void *pArg)
{
	PLRMSGWHISPER *ptWhisper;
	SCMDCHATDATA *pChatData;
	GAME *ptGame;
	UNIT *hSpeaker, *hPlayer;
	char *pszName;

	ptWhisper = (PLRMSGWHISPER *)pArg;
	ptGame = ClientGetGame(ptClient);
	pChatData = ptWhisper->pChatData;
	pszName = ClientGetName(ptClient);
	if (StormSStrCmpI(pszName, ptWhisper->ptMsg->szName, 2147483647))
		return;
	if (ptWhisper->dwSpeakerId != (DWORD)-1)
	{
		hSpeaker = SUnitFind(ptGame, UNIT_PLAYER, ptWhisper->dwSpeakerId);
		hPlayer = ClientGetPlayerUnit(ptClient, FALSE);
		if (hSpeaker && hPlayer)
		{
			if (PlayerListTestFlag(hPlayer, hSpeaker, 4))
			{
				sgbWhisperRefused = TRUE;
				return;
			}
			if (PlayerListTestFlag(hSpeaker, hPlayer, 2))
			{
				sgbWhisperRefused = TRUE;
				return;
			}
		}
	}
	sgbWhisperSent = TRUE;
	SCmdSendChat(ptClient, pChatData);
}

void __fastcall PlrMsgSendChatEach(CLIENT *ptClient, void *pArg)
{
	SCmdSendChat(ptClient, (SCMDCHATDATA *)pArg);
}

static void PlrMsgWhisperNotice(UNIT *hPlayer, char *szName, BYTE bEvent)
{
	GAMEPLAYERMSG tSystemMsg;

	tSystemMsg.bCmd = S2C_ROSTER_EVENT;
	tSystemMsg.bColor = 0;
	tSystemMsg.dwArg = 0;
	StormSStrCopy(tSystemMsg.szName, szName, 2147483647);
	tSystemMsg.szName[15] = 0;
	tSystemMsg.bEvent = bEvent;
	SCmdSendSystemMsg(SUnitGetClient(hPlayer), (SCMDSYSTEMMSG *)&tSystemMsg);
}

int __fastcall PlrMsgOnChat(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGCHAT tMsg;
	SCMDCHATDATA tChatData;
	SCMDCHATDATA tEcho;
	PLRMSGWHISPER tWhisper;
	char *pszName;
	int nLength;
	BOOL bWhisper;
	PFNCLIENTCALLBACK pfnEach;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGCHAT)))
		return 3;
	tMsg = *(PLRMSGCHAT *)pMsg;
	pszName = ClientGetName(SUnitGetClient(hPlayer));
	if (StormSStrLen(pszName) > 0)
		StormSStrCopy(tChatData.szName, pszName, 2147483647);
	else
		tChatData.szName[0] = 0;
	nLength = (int)StormSStrLen(tMsg.szName);
	if (nLength > 0 && nLength < 16)
		bWhisper = TRUE;
	else
		bWhisper = FALSE;
	tChatData.bLevel = (BYTE)D2CommonStatsUnitGetValue(hPlayer, STATS_LEVEL);
	tMsg.szName[15] = 0;
	if (tMsg.szText[255])
	{
#line 3918
		PLRMSG_HACK("tried to send an invalid chat message", __LINE__);
		return 2;
	}
	memcpy(tChatData.bCustomText, tMsg.szText, sizeof(tChatData.bCustomText));
	tChatData.field_02 = tMsg.field_02;
	tChatData.bColor = 0;
	tChatData.bCmd = S2C_CHAT_MESSAGE;
	tChatData.dwId = 0;
	tChatData.bType = 2;
	if (bWhisper)
	{
		tChatData.bKind = 2;
		pfnEach = PlrMsgWhisperEach;
	}
	else
	{
		tChatData.bKind = 1;
		pfnEach = PlrMsgChatEach;
	}
	sgbWhisperSent = FALSE;
	sgbWhisperRefused = FALSE;
	tWhisper.pChatData = &tChatData;
	tWhisper.ptMsg = &tMsg;
	if (hPlayer && hPlayer->eType == UNIT_PLAYER)
		tWhisper.dwSpeakerId = hPlayer->dwId;
	else
		tWhisper.dwSpeakerId = (DWORD)-1;
	GameForEachClient(ptGame, pfnEach, &tWhisper);
	if (tChatData.bKind != 2)
		return 0;
	if (sgbWhisperRefused)
		PlrMsgWhisperNotice(hPlayer, tMsg.szName, 13);
	else if (!sgbWhisperSent)
		PlrMsgWhisperNotice(hPlayer, tMsg.szName, 4);
	else
	{
		tEcho.bCmd = S2C_CHAT_MESSAGE;
		tEcho.bKind = 6;
		tEcho.dwId = 0;
		tEcho.bType = 2;
		tEcho.bColor = 0;
		StormSStrCopy(tEcho.szName, tMsg.szName, 2147483647);
		tEcho.szName[15] = 0;
		tEcho.field_02 = 0;
		memcpy(tEcho.bCustomText, tMsg.szText, sizeof(tEcho.bCustomText));
		SCmdSendChat(SUnitGetClient(hPlayer), &tEcho);
	}
	return 0;
}

void __fastcall PlrMsgSendChatAll(GAME *ptGame, const char *szText, BYTE bColor)
{
	SCMDCHATDATA tChatData;

	tChatData.dwId = 0;
	tChatData.szName[0] = 0;
	tChatData.bCmd = S2C_CHAT_MESSAGE;
	tChatData.bKind = 1;
	tChatData.bType = 2;
	tChatData.bColor = bColor;
	tChatData.field_02 = 0;
	memcpy(tChatData.bCustomText, szText, sizeof(tChatData.bCustomText));
	GameForEachClient(ptGame, PlrMsgSendChatEach, &tChatData);
}

void __fastcall PlrMsgSendTextAll(GAME *ptGame, char *szText, BYTE bColor)
{
	SCMDCHATDATA tChatData;

	tChatData.bColor = bColor;
	tChatData.bCmd = S2C_CHAT_MESSAGE;
	tChatData.bKind = 4;
	tChatData.dwId = 0;
	tChatData.bType = 0;
	memcpy(tChatData.bCustomText, szText, sizeof(tChatData.bCustomText));
	tChatData.field_02 = 0;
	GameForEachClient(ptGame, PlrMsgSendChatEach, &tChatData);
}

void __fastcall PlrMsgSendSystemMsgEach(CLIENT *ptClient, void *pArg)
{
	SCmdSendSystemMsg(ptClient, (SCMDSYSTEMMSG *)pArg);
}

void __fastcall PlrMsgSendSystemMsgAll(GAME *ptGame, GAMEPLAYERMSG *ptMsg)
{
	GameForEachClient(ptGame, PlrMsgSendSystemMsgEach, ptMsg);
}

int __fastcall PlrMsgOnInteract(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGUNIT *ptMsg;
	DWORD dwUnitType, dwUnitId;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGUNIT)))
		return 3;
	ptMsg = (PLRMSGUNIT *)pMsg;
	dwUnitType = ptMsg->dwUnitType;
	dwUnitId = ptMsg->dwUnitId;
	if (dwUnitType >= NUM_UNIT_TYPES)
		return 2;
	return PlrMsgInteract(hPlayer, dwUnitType, dwUnitId, FALSE, ptGame);
}

int __fastcall PlrMsgOnInteractWithFlag(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGINTERACT *ptMsg;
	DWORD dwUnitType, dwUnitId, dwFlag;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGINTERACT)))
		return 3;
	ptMsg = (PLRMSGINTERACT *)pMsg;
	dwUnitType = ptMsg->dwUnitType;
	dwUnitId = ptMsg->dwUnitId;
	dwFlag = ptMsg->dwFlag;
	if (dwUnitType >= NUM_UNIT_TYPES)
		return 2;
	if (hPlayer->dwId == dwUnitId && dwUnitType == UNIT_PLAYER)
	{
#line 4090
		PLRMSG_HACK("operate on self", __LINE__);
		return 3;
	}
	return PlrMsgInteract(hPlayer, dwUnitType, dwUnitId, dwFlag, ptGame);
}

static inline BOOL PlrMsgIsCursorItem(GAME *ptGame, UNIT *hPlayer, DWORD dwItemId)
{
	UNIT *hItem;

#line 2770
	D2ASSERT(ptGame);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem || hItem->dwMode != IMODE_ONCURSOR
		|| (UNIT *)D2CommonINVENTORYGetCursorItem(hPlayer->ptInventory) != hItem)
		return FALSE;
	return TRUE;
}

int __fastcall PlrMsgOnDropCursorItem(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwItemId;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGITEM)))
		return 3;
	dwItemId = ((PLRMSGITEM *)pMsg)->dwItemId;
	if (!PlrMsgIsCursorItem(ptGame, hPlayer, dwItemId))
		return 1;
	ITEMSModeDropCursorItem(ptGame, hPlayer, dwItemId);
	return 0;
}

int __fastcall sInsertItemGrid(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGITEMGRID *ptMsg;
	DWORD dwItemId, dwPage;
	int nX, nY;
	void *hRoom;
	UNIT *hItem;
	char *pszItemName;
	PLRINFO *ptInfo;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGITEMGRID)))
	{
		FogErrorManagerLog("svalidatemessage failed for sInsertItemGrid()\n");
		return 3;
	}
	ptMsg = (PLRMSGITEMGRID *)pMsg;
	nX = (int)ptMsg->dwX;
	nY = (int)ptMsg->dwY;
	dwItemId = ptMsg->dwItemId;
	dwPage = ptMsg->dwPage;
	if (!PlrMsgIsCursorItem(ptGame, hPlayer, dwItemId))
		return 1;
	if (hPlayer->eType == UNIT_PLAYER)
	{
		if (!PlayerIsBusy(hPlayer) && dwPage)
		{
#line 4143
			FogErrorManagerLog(" tell jon and steve about error at line %d of file %s\n", __LINE__,
				D2_THIS_FILE);
			return 2;
		}
		if (dwPage == 4)
		{
			hRoom = (void *)D2CommonUnitGetPathRoom(hPlayer);
			if (!hRoom || !D2CommonDungeonRoomIsTown(hRoom))
			{

				FogErrorManagerLog(" Player %s insert itemgrid error #1\n", ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->szName);
#line 4152
				PLRMSG_HACK("insert item in bank from outside town", __LINE__);
				return 3;
			}
		}
		else if (dwPage != 3 && dwPage != 2 && dwPage != 0)
			return 2;
	}
	else if (dwPage >= 5)
		return 2;
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (hItem) {
		D2CommonITEMSDataSetPage(hItem, dwPage);
		if (!ITEMSModePlaceCursorItem(ptGame, hPlayer, dwItemId, nX, nY, FALSE, TRUE, NULL))
		{
			pszItemName = ((GAMEITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId))->szName;

			FogErrorManagerLog("Player insert itemgrid error #2 for player %s and item %s\n", ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->szName,
				pszItemName);
#line 4185
			PLRMSG_HACK("itemsinsertinventorygrid", __LINE__);
			return 3;
		}
		return 0;

	}
	return 2;
}

static inline BOOL PlrMsgIsStoredItem(GAME *ptGame, UNIT *hPlayer, DWORD dwItemId)
{
	UNIT *hItem;

#line 2793
	D2ASSERT(ptGame);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem || hItem->dwMode != IMODE_STORED
		|| !D2CommonINVENTORYTestHasItem(hPlayer->ptInventory, hItem))
		return FALSE;
	return TRUE;
}

int __fastcall sRemoveItemGrid(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwItemId;
	GAMEPLAYERMSG tMsg;
	int i;
	int nResult;
	UNIT *hItem;
	PLRINFO *ptInfo;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGITEM)))
	{
		FogErrorManagerLog("svalidatemessage failed for sRemoveItemGrid()\n");
		return 3;
	}
	dwItemId = ((PLRMSGITEM *)pMsg)->dwItemId;
	if (!PlrMsgIsStoredItem(ptGame, hPlayer, dwItemId))
		return 1;
	if (D2CommonINVENTORYGetCursorItem(hPlayer->ptInventory))
	{
		for (i = 0; i < (int)(sizeof(tMsg) / sizeof(DWORD)); i++)
			((DWORD *)&tMsg)[i] = 0;
		tMsg.bCmd = S2C_ROSTER_EVENT;
		tMsg.bColor = 1;
		tMsg.dwArg = 0;
		tMsg.bEvent = 14;
		SCmdSendSystemMsg(SUnitGetClient(hPlayer), (SCMDSYSTEMMSG *)&tMsg);
		return 2;
	}
	if (!PlayerInteract(ptGame, hPlayer, (DWORD)SUnitFind(ptGame, UNIT_ITEM, dwItemId)))
		return 0;
	nResult = 0;
	if (!ITEMSModeTakeToCursor(ptGame, hPlayer, dwItemId, &nResult, 1, FALSE, NULL, FALSE) && nResult)
	{
		hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
		if (hItem)
		{
			ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
			FogErrorManagerLog(" Player sremoveitemgrid error #2\n of item %s from player %s",
				((GAMEITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId))->szName, ptInfo->szName);
		}
		else
			FogErrorManagerLog(" Player sremoveitemgrid error #3\n from player %s",
				((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->szName);
#line 4244
		PLRMSG_HACK("itemsremoveinventorygrid", __LINE__);
		return 3;
	}
	return 0;
}

static inline BOOL PlrMsgIsBodyLoc(int nBodyLoc)
{
	if (nBodyLoc <= 0 || nBodyLoc >= NUM_BODYLOCS)
	{
#line 2838
		PLRMSG_HACK("tried to do something with an invalid body location", __LINE__);
		return FALSE;
	}
	return TRUE;
}

int __fastcall sinsertitembody(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGITEMBODY *ptMsg;
	DWORD dwItemId;
	BYTE bBodyLoc;
	int nResult;
	UNIT *hItem;
	PLRINFO *ptInfo;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGITEMBODY)))
		return 3;
	ptMsg = (PLRMSGITEMBODY *)pMsg;
	dwItemId = ptMsg->dwItemId;
	bBodyLoc = ptMsg->bBodyLoc;
	if (!PlrMsgIsCursorItem(ptGame, hPlayer, dwItemId))
		return 1;
	if (!PlrMsgIsBodyLoc(bBodyLoc))
		return 2;
	nResult = 0;
	if (!ITEMSModeEquipCursorItem(ptGame, hPlayer, dwItemId, bBodyLoc, FALSE, &nResult) && nResult)
	{
		hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
		if (hItem)
		{
			ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
			FogErrorManagerLog("sinsertitembody cheat detected for item %s player %s\n",
				((GAMEITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId))->szName, ptInfo->szName);
		}
		else
			FogErrorManagerLog("sinsertitembody cheat detected for player %s\n",
				((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->szName);
#line 4286
		PLRMSG_HACK("itemsinsertbody", __LINE__);
		return 3;
	}
	return 0;
}

int __fastcall sInsertItemBodyRmvOpp(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGITEMBODY *ptMsg;
	DWORD dwItemId;
	BYTE bBodyLoc;
	int nResult;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGITEMBODY)))
		return 3;
	ptMsg = (PLRMSGITEMBODY *)pMsg;
	dwItemId = ptMsg->dwItemId;
	bBodyLoc = ptMsg->bBodyLoc;
	if (!PlrMsgIsCursorItem(ptGame, hPlayer, dwItemId))
		return 1;
	if (!PlrMsgIsBodyLoc(bBodyLoc))
		return 2;
	nResult = 0;
	if (!ITEMSModeSwapHands(ptGame, hPlayer, dwItemId, bBodyLoc, FALSE, &nResult) && nResult)
	{
		FogErrorManagerLog("Player sInsertItemBodyRmvOpp error #1 for player %s\n",
			((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->szName);
#line 4319
		PLRMSG_HACK("itemsinsertbodyrmvopp", __LINE__);
		return 3;
	}
	return 0;
}

int __fastcall PlrMsgOnRemoveItemBodyLoc(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	int nBodyLoc;
	int nResult;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGBODYLOC)))
		return 3;
	nBodyLoc = ((PLRMSGBODYLOC *)pMsg)->wBodyLoc;
	if (!PlrMsgIsBodyLoc(nBodyLoc))
		return 2;
	if (!PlayerInteract(ptGame, hPlayer, D2CommonINVENTORYGetLocSlot(hPlayer->ptInventory, nBodyLoc)))
		return 0;
	if (nBodyLoc == 8 && !PlrTradeIsFree(ptGame, hPlayer))
		return 0;
	nResult = 0;
	if (!ITEMSModeUnequipToCursor(ptGame, hPlayer, nBodyLoc, FALSE, &nResult) && nResult)
	{
#line 4394
		PLRMSG_HACK("removeitemsbody", __LINE__);
		return 3;
	}
	return 0;
}

int __fastcall sSwapItemBody(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGITEMBODY *ptMsg;
	DWORD dwItemId;
	BYTE bBodyLoc;
	int nResult;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGITEMBODY)))
		return 3;
	ptMsg = (PLRMSGITEMBODY *)pMsg;
	dwItemId = ptMsg->dwItemId;
	bBodyLoc = ptMsg->bBodyLoc;
	if (!PlrMsgIsCursorItem(ptGame, hPlayer, dwItemId))
		return 1;
	if (!PlrMsgIsBodyLoc(bBodyLoc))
		return 2;
	if (!D2CommonINVENTORYGetLocSlot(hPlayer->ptInventory, bBodyLoc))
		return 1;
	if (bBodyLoc == 8 && !PlrTradeIsFree(ptGame, hPlayer))
		return 0;
	if (!PlayerInteract(ptGame, hPlayer, D2CommonINVENTORYGetLocSlot(hPlayer->ptInventory, bBodyLoc)))
		return 0;
	nResult = 0;
	if (!ITEMSModeSwapBodyItem(ptGame, hPlayer, dwItemId, bBodyLoc, FALSE, &nResult) && nResult)
	{
		FogErrorManagerLog("sSwapItemBody Cheat detected for player %s\n",
			((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->szName);
#line 4441
		PLRMSG_HACK("itemsswapbody", __LINE__);
		return 3;
	}
	return 0;
}

static inline BOOL PlrMsgIsArm(int nBodyLoc)
{
	if (nBodyLoc != BODYLOC_RARM && nBodyLoc != BODYLOC_LARM)
	{
#line 2867
		PLRMSG_HACK("tried to do a double manipulation on a part that wasn't an arm", __LINE__);
		return FALSE;
	}
	return TRUE;
}

int __fastcall sDoubleSwapItemBody(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGITEMBODY *ptMsg;
	DWORD dwItemId;
	BYTE bBodyLoc;
	int nResult;
	int nStatus;
	DWORD dwSlot;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGITEMBODY)))
		return 3;
	ptMsg = (PLRMSGITEMBODY *)pMsg;
	dwItemId = ptMsg->dwItemId;
	bBodyLoc = ptMsg->bBodyLoc;
	if (!PlrMsgIsCursorItem(ptGame, hPlayer, dwItemId))
		return 1;
	if (PlrMsgIsBodyLoc(bBodyLoc))
	{
		if (!PlrMsgIsArm(bBodyLoc))
			nStatus = 3;
		else
			nStatus = !D2CommonINVENTORYGetLocSlot(hPlayer->ptInventory, bBodyLoc);
	}
	else
		nStatus = 2;
	if (nStatus)
		return nStatus;
	dwSlot = D2CommonINVENTORYGetLocSlot(hPlayer->ptInventory, bBodyLoc);
	if (!PlayerInteract(ptGame, hPlayer, dwSlot))
		return 0;
	if (!ITEMSModeEquipTwoHands(ptGame, hPlayer, dwItemId, bBodyLoc, &nResult) && nResult)
	{
		FogErrorManagerLog("sDoubleSwapItemBody Cheat detected for player %s\n",
			((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->szName);
#line 4480
		PLRMSG_HACK("itemsdoubleswapbody", __LINE__);
		return 3;
	}
	return 0;
}

int __fastcall sSwapItemGrid(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGSWAPGRID *ptMsg;
	DWORD dwItemId, dwGridItemId;
	int nX, nY;
	int nResult;
	UNIT *hItem, *hGridItem;
	PLRINFO *ptInfo;
	char *pszGridName;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGSWAPGRID)))
		return 3;
	ptMsg = (PLRMSGSWAPGRID *)pMsg;
	nX = (int)ptMsg->dwX;
	nY = (int)ptMsg->dwY;
	dwItemId = ptMsg->dwItemId;
	dwGridItemId = ptMsg->dwGridItemId;
	if (!PlrMsgIsCursorItem(ptGame, hPlayer, dwItemId))
		return 1;
	if (!PlrMsgIsStoredItem(ptGame, hPlayer, dwGridItemId))
		return 1;
	if (!PlayerInteract(ptGame, hPlayer, (DWORD)SUnitFind(ptGame, UNIT_ITEM, dwGridItemId)))
		return 0;
	if (!ITEMSModeSwapGridItem(ptGame, hPlayer, dwItemId, dwGridItemId, nX, nY, &nResult) && nResult)
	{
		hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
		hGridItem = SUnitFind(ptGame, UNIT_ITEM, dwGridItemId);
		ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
		FogErrorManagerLog(
			"sSwapItemGrid Cheat detected of item in hand of %s and item inv %s for player %s\n",
			((GAMEITEMSTXT *)D2CommonItemTblsGetItemStats(
				hItem->dwClassId))->szName, ((GAMEITEMSTXT *)D2CommonItemTblsGetItemStats(hGridItem->dwClassId))->szName, ptInfo->szName);
#line 4528
		PLRMSG_HACK("swapitemgrid", __LINE__);
		return 3;
	}
	return 0;
}

int __fastcall sUseItemGridXY(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGITEMXY *ptMsg;
	DWORD dwItemId, dwX, dwY;
	int nResult;
	UNIT *hItem;
	PLRINFO *ptInfo;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGITEMXY)))
		return 3;
	ptMsg = (PLRMSGITEMXY *)pMsg;
	dwX = ptMsg->dwX;
	dwY = ptMsg->dwY;
	dwItemId = ptMsg->dwItemId;
	if (!PlrMsgIsStoredItem(ptGame, hPlayer, dwItemId))
		return 1;
	if (PlrMsgOutOfRange(hPlayer, (WORD)dwX, (WORD)dwY, PLRMSG_RANGE))
		return 1;
	nResult = 0;
	if (!ITEMSModeUseItem(ptGame, hPlayer, dwItemId, dwX, dwY, &nResult) && nResult)
	{
		hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
		if (hItem)
		{
			ptInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
			FogErrorManagerLog("sUseItemGridXY Cheat detected for item %s player %s\n",
				((GAMEITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId))->szName, ptInfo->szName);
		}
		else
			FogErrorManagerLog("sUseItemGridXY Cheat detected for player %s\n",
				((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->szName);
#line 4572
		PLRMSG_HACK("itemsinventorygriduse", __LINE__);
		return 3;
	}
	return 0;
}

extern BOOL PlrMsgIsOwnItem(GAME *ptGame, UNIT *hPlayer, DWORD dwItemId);
__inline BOOL PlrMsgIsOwnItem(GAME *ptGame, UNIT *hPlayer, DWORD dwItemId)
{
	UNIT *hItem;

#line 2816
	D2ASSERT(ptGame);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem)
		return FALSE;
	if (!D2CommonINVENTORYTestHasItem(hPlayer->ptInventory, hItem)
		&& (UNIT *)D2CommonINVENTORYGetCursorItem(hPlayer->ptInventory) != hItem)
		return FALSE;
	return TRUE;
}

int __fastcall sUseItemAction(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGTWOIDS *ptMsg;
	DWORD dwItemId, dwTargetId;
	int nResult;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGTWOIDS)))
		return 3;
	ptMsg = (PLRMSGTWOIDS *)pMsg;
	dwItemId = ptMsg->dwItemId;
	dwTargetId = ptMsg->dwOtherId;
	if (!PlrMsgIsOwnItem(ptGame, hPlayer, dwItemId))
		return 1;
	if (!PlrMsgIsOwnItem(ptGame, hPlayer, dwTargetId))
		return 1;
	nResult = 0;
	if (!ITEMSModeUseItemOnItem(ptGame, hPlayer, dwItemId, dwTargetId, &nResult) && nResult)
	{
		FogErrorManagerLog("sUseItemAction Cheat detected for player %s\n",
			((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->szName);
#line 4604
		PLRMSG_HACK("itemsinventoryuseaction", __LINE__);
		return 3;
	}
	return 0;
}

int __fastcall PlrMsgOnItemStack(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGTWOIDS *ptMsg;
	DWORD dwItemId, dwTargetId;
	int nResult;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGTWOIDS)))
		return 3;
	ptMsg = (PLRMSGTWOIDS *)pMsg;
	dwItemId = ptMsg->dwItemId;
	dwTargetId = ptMsg->dwOtherId;
	if (!PlrMsgIsOwnItem(ptGame, hPlayer, dwItemId))
		return 1;
	if (!PlrMsgIsOwnItem(ptGame, hPlayer, dwTargetId))
		return 1;
	if (dwItemId == dwTargetId)
	{
#line 4631
		PLRMSG_HACK("tried to stack an item on itself", __LINE__);
		return 3;
	}
	if (!ITEMSModeStackCursorItem(ptGame, hPlayer, dwItemId, dwTargetId, &nResult) && nResult)
	{
#line 4639
		PLRMSG_HACK("itemstack", __LINE__);
		return 3;
	}
	return 0;
}

int __fastcall sRemoveStackItem(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwItemId;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGITEM)))
		return 3;
	dwItemId = ((PLRMSGITEM *)pMsg)->dwItemId;
	if (!PlrMsgIsOwnItem(ptGame, hPlayer, dwItemId))
		return 1;
	if (!ITEMSModeNone(ptGame, hPlayer, dwItemId, (DWORD)&nSize) && nSize)
	{
		FogErrorManagerLog("sRemoveStackItem Cheat detected for player %s\n",
			((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->szName);
#line 4669
		PLRMSG_HACK("itemsremovestackitem", __LINE__);
		return 3;
	}
	return 0;
}

int __fastcall sInsertItemBelt(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGTWOIDS *ptMsg;
	DWORD dwItemId, dwSlot;
	int nResult;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGTWOIDS)))
		return 3;
	ptMsg = (PLRMSGTWOIDS *)pMsg;
	dwItemId = ptMsg->dwItemId;
	dwSlot = ptMsg->dwOtherId;
	if (!PlrMsgIsCursorItem(ptGame, hPlayer, dwItemId))
		return 1;
	nResult = 0;
	if (!ITEMSModePutInBelt(ptGame, hPlayer, dwItemId, dwSlot, FALSE, &nResult) && nResult)
	{
		FogErrorManagerLog("sInsertItemBelt cheat detected %s\n",
			((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->szName);
#line 4700
		PLRMSG_HACK("itemsinsertinventorybelt", __LINE__);
		return 3;
	}
	return 0;
}

extern BOOL PlrMsgIsBeltItem(GAME *ptGame, UNIT *hPlayer, DWORD dwItemId);
__inline BOOL PlrMsgIsBeltItem(GAME *ptGame, UNIT *hPlayer, DWORD dwItemId)
{
	UNIT *hItem;

#line 2880
	D2ASSERT(ptGame);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem || hItem->dwMode != IMODE_INBELT
		|| !D2CommonINVENTORYTestHasItem(hPlayer->ptInventory, hItem))
		return FALSE;
	return TRUE;
}

int __fastcall sRemoveItemBelt(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwItemId;
	int nResult;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGITEM)))
		return 3;
	dwItemId = ((PLRMSGITEM *)pMsg)->dwItemId;
	if (!PlrMsgIsBeltItem(ptGame, hPlayer, dwItemId))
		return 1;
	if (!PlayerInteract(ptGame, hPlayer, 0))
		return 0;
	if (D2CommonINVENTORYGetCursorItem(hPlayer->ptInventory))
	{
		FogErrorManagerLog("sRemoveItemBelt 1 cheat detected for player %s\n",
			((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->szName);
#line 4731
		PLRMSG_HACK("inventorygethanditem", __LINE__);
		return 3;
	}
	nResult = 0;
	if (!ITEMSModeTakeFromBelt(ptGame, hPlayer, dwItemId, &nResult) && nResult)
	{
		FogErrorManagerLog("sRemoveItemBelt 2 cheat detected%s\n",
			((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->szName);
#line 4742
		PLRMSG_HACK("removeinventorybelt", __LINE__);
		return 3;
	}
	return 0;
}

int __fastcall sSwapItemBelt(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGTWOIDS *ptMsg;
	DWORD dwItemId, dwBeltItemId;
	int nResult;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGTWOIDS)))
		return 3;
	ptMsg = (PLRMSGTWOIDS *)pMsg;
	dwItemId = ptMsg->dwItemId;
	dwBeltItemId = ptMsg->dwOtherId;
	if (!PlrMsgIsCursorItem(ptGame, hPlayer, dwItemId))
		return 1;
	if (!PlrMsgIsBeltItem(ptGame, hPlayer, dwBeltItemId))
		return 1;
	nResult = 0;
	if (!ITEMSModeSwapBeltItem(ptGame, hPlayer, dwItemId, dwBeltItemId, &nResult) && nResult)
	{
		FogErrorManagerLog("sSwapItemBelt cheat detected for player %s\n",
			((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->szName);
#line 4775
		PLRMSG_HACK("swapinventorybelt", __LINE__);
		return 3;
	}
	return 0;
}

int __fastcall sUseItemBeltXY(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGITEMXY *ptMsg;
	DWORD dwItemId, dwX, dwY;
	int nResult;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGITEMXY)))
		return 3;
	ptMsg = (PLRMSGITEMXY *)pMsg;
	dwItemId = ptMsg->dwItemId;
	dwX = ptMsg->dwX;
	dwY = ptMsg->dwY;
	if (!PlrMsgIsBeltItem(ptGame, hPlayer, dwItemId))
		return 1;
	if (PlrMsgOutOfRange(hPlayer, (WORD)dwX, (WORD)dwY, PLRMSG_RANGE))
		return 1;
	nResult = 0;
	if (!ITEMSModeUseBeltItem(ptGame, hPlayer, dwItemId, dwX, dwY, &nResult) && nResult)
	{
		FogErrorManagerLog("sUseItemBeltXY cheat detected for player %s\n",
			((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->szName);
#line 4807
		PLRMSG_HACK("inventorybeltuse", __LINE__);
		return 3;
	}
	return 0;
}

int __fastcall PlrMsgOnInsertItemItemInventory(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGTWOIDS *ptMsg;
	DWORD dwItemId, dwTargetId;
	DWORD dwType, dwUnitId;
	UNIT *hUnit;
	int nResult;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGTWOIDS)))
		return 3;
	ptMsg = (PLRMSGTWOIDS *)pMsg;
	dwItemId = ptMsg->dwItemId;
	dwTargetId = ptMsg->dwOtherId;
	if (!PlrMsgIsCursorItem(ptGame, hPlayer, dwItemId))
		return 1;
	if (!PlrMsgIsStoredItem(ptGame, hPlayer, dwTargetId))
		return 1;
	if (SUnitGetInteract(hPlayer, &dwType, &dwUnitId))
	{
		hUnit = SUnitFind(ptGame, dwType, dwUnitId);
		if (hUnit && hUnit->eType == UNIT_PLAYER)
			return 0;
	}
	nResult = 0;
	if (!ITEMSModeSocketGem(ptGame, hPlayer, dwItemId, dwTargetId, &nResult, TRUE, TRUE, TRUE) && nResult)
	{
#line 4848
		PLRMSG_HACK("insertitemiteminventory", __LINE__);
		return 3;
	}
	return 0;
}

static inline int PlrMsgCheckActLevels(UNIT *hUnit, UNIT *hOther)
{
	DWORD hRoom;
	int nLevelId;
	BYTE bAct;

	hRoom = D2CommonUnitGetPathRoom(hUnit);
	nLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
	bAct = D2CommonGetActNum(nLevelId);
	hRoom = D2CommonUnitGetPathRoom(hOther);
	nLevelId = D2CommonDungeonRoomGetLevelId(hRoom);
	if (bAct != D2CommonGetActNum(nLevelId))
		return 2;
	else
		return 0;
}

static int __fastcall PlrMsgCheckItem(GAME *ptGame, UNIT *hPlayer, DWORD dwItemId)
{
	UNIT *hItem;
	WORD wX, wY;

#line 2907
	D2ASSERT(ptGame);
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem)
		return 1;
	switch (hItem->dwMode)
	{
	case IMODE_ONGROUND:
		if (PlrMsgCheckActLevels(hPlayer, hItem))
			return 2;
		wX = (WORD)D2CommonUnitGetX(hItem);
		wY = (WORD)D2CommonUnitGetY(hItem);
		if (PlrMsgOutOfRange(hPlayer, wX, wY, PLRMSG_ITEMRANGE))
			return 1;
		break;
	case IMODE_STORED:
	case 1:
	case IMODE_INBELT:
	case IMODE_ONCURSOR:
		if (!PlrMsgIsOwnItem(ptGame, hPlayer, dwItemId))
			return 1;
		break;
	default:
		return 1;
	}
	return 0;
}
#line 2932

int __fastcall PlrMsgOnAddScrollToBook(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGTWOIDS *ptMsg;
	DWORD dwItemId, dwBookId;
	int nRet;
	int nResult;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGTWOIDS)))
		return 3;
	ptMsg = (PLRMSGTWOIDS *)pMsg;
	dwItemId = ptMsg->dwItemId;
	dwBookId = ptMsg->dwOtherId;
	nRet = PlrMsgCheckItem(ptGame, hPlayer, dwItemId);
	if (nRet)
		return nRet;
	if (!PlrMsgIsStoredItem(ptGame, hPlayer, dwBookId))
		return 1;
	nResult = 0;
	if (!ITEMSModeScrollToTome(ptGame, hPlayer, dwItemId, dwBookId, &nResult) && nResult)
	{
#line 4878
		PLRMSG_HACK("addscrolltobook", __LINE__);
		return 3;
	}
	return 0;
}

int __fastcall PlrMsgOnAddTransmogrifyBox(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGTWOIDS *ptMsg;
	DWORD dwItemId, dwCubeId;
	int nRet;
	int nResult;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGTWOIDS)))
		return 3;
	ptMsg = (PLRMSGTWOIDS *)pMsg;
	dwItemId = ptMsg->dwItemId;
	dwCubeId = ptMsg->dwOtherId;
	nRet = PlrMsgCheckItem(ptGame, hPlayer, dwItemId);
	if (nRet)
		return nRet;
	if (!PlrMsgIsStoredItem(ptGame, hPlayer, dwCubeId))
		return 1;
	nResult = 0;
	if (!ITEMSModePutInCube(ptGame, hPlayer, dwItemId, dwCubeId, &nResult) && nResult)
	{
#line 4907
		PLRMSG_HACK("addtransmogrifybox", __LINE__);
		return 3;
	}
	return 0;
}

int __fastcall PlrMsgOnIgnored(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGITEM)))
		return 3;
	return 0;
}

int __fastcall PlrMsgOnInvalid(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
#line 4947
	PLRMSG_HACK("invalid message", __LINE__);
	return 3;
}

int __fastcall PlrMsgOnInvalidUnselectUnit(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
#line 4978
	PLRMSG_HACK("invalid unselectunit", __LINE__);
	return 3;
}

void __fastcall PlrMsgWarpToLevel(GAME *ptGame, UNIT *hUnit, DWORD dwLevelId, DWORD dwArg)
{
	BYTE bAct, bUnitAct;
	GAMEROOM *hRoom;
	int nX, nY;
	DWORD dwTown;

	bAct = D2CommonGetActNum(dwLevelId);
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	bUnitAct = D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(hRoom));
	if (!dwLevelId)
	{
		dwTown = ((DWORD (__stdcall *)(BYTE))D2CommonDungeonActGetTownLevelIdRaw)(bUnitAct);
		LevelWarpUnit(ptGame, hUnit, dwTown, dwArg);
		return;
	}
	if (bAct == bUnitAct)
	{
		nX = (int)D2CommonUnitGetX(hUnit) - 5;
		nY = (int)D2CommonUnitGetY(hUnit);
		SkillsOpenPortal(ptGame, hUnit, hRoom, nX, nY, dwLevelId, 0, 59, 0);
		return;
	}
	LevelWarpUnit(ptGame, hUnit, dwLevelId, dwArg);
}

static int __fastcall PlrMsgCheckMonster(GAME *ptGame, UNIT *hPlayer, DWORD dwMonsterId, UNIT **phMonster,
	BOOL bInRange)
{
	UNIT *hMonster;
	WORD wX, wY;

	hMonster = SUnitFind(ptGame, UNIT_MONSTER, dwMonsterId);
	*phMonster = hMonster;
	if (!hMonster)
		return 1;
	if (!((MONSTERDATA *)hMonster->uTypeData)->pptThreats)
	{
#line 2957
		PLRMSG_HACK("tried to interact with monster that doesn't interact", __LINE__);
		return 3;
	}
	if (PlrMsgCheckAct(hPlayer, hMonster))
		return 2;
	if (bInRange)
	{
		wX = (WORD)D2CommonUnitGetX(*phMonster);
		wY = (WORD)D2CommonUnitGetY(*phMonster);
		if (PlrMsgOutOfRange(hPlayer, wX, wY, PLRMSG_ITEMRANGE))
			return 1;
	}
	return 0;
}

int __fastcall PlrMsgOnThreatTell(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	UNIT *hMonster;
	int nRet;
	DWORD dwOther;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGTWOIDS)))
		return 3;
	dwOther = ((PLRMSGTWOIDS *)pMsg)->dwOtherId;
	nRet = PlrMsgCheckMonster(ptGame, hPlayer, dwOther, &hMonster, TRUE);
	if (nRet)
		return nRet;
	MonsterAITellThreat(ptGame, hPlayer, hMonster, ((MONSTERDATA *)hMonster->uTypeData)->pptThreats);
	return 0;
}

int __fastcall PlrMsgOnThreatDrop(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	UNIT *hMonster;
	int nRet;
	DWORD dwOther;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGTWOIDS)))
		return 3;
	dwOther = ((PLRMSGTWOIDS *)pMsg)->dwOtherId;
	nRet = PlrMsgCheckMonster(ptGame, hPlayer, dwOther, &hMonster, FALSE);
	if (nRet)
		return nRet;
	MonsterAIDropThreat(ptGame, hPlayer, hMonster, ((MONSTERDATA *)hMonster->uTypeData)->pptThreats);
	return 0;
}

int __fastcall PlrMsgOnQuestMonsterMessage(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGIDCODE *ptMsg;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGIDCODE)))
		return 3;
	ptMsg = (PLRMSGIDCODE *)pMsg;
	QuestsMonsterMessage(ptGame, hPlayer, ptMsg->dwId, ptMsg->wCode);
	return 0;
}

int __fastcall PlrMsgOnNpcBuy(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGNPC *ptMsg;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGNPC)))
		return 3;
	ptMsg = (PLRMSGNPC *)pMsg;
	SUnitNpcBuy(ptGame, hPlayer, ptMsg->dwNpcId, ptMsg->dwItemId, ptMsg->dwField09 >> 16, ptMsg->dwField09,
		ptMsg->dwField0d);
	return 0;
}

int __fastcall PlrMsgOnNpcSell(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGNPC *ptMsg;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGNPC)))
		return 3;
	ptMsg = (PLRMSGNPC *)pMsg;
	SUnitNpcSell(ptGame, hPlayer, ptMsg->dwNpcId, ptMsg->dwItemId, (WORD)ptMsg->dwField09, ptMsg->dwField0d);
	return 0;
}

int __fastcall PlrMsgOnNpcRepairItem(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGNPC *ptMsg;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGNPC)))
		return 3;
	ptMsg = (PLRMSGNPC *)pMsg;
	SUnitNpcRepairItem(ptGame, hPlayer, ptMsg->dwNpcId, ptMsg->dwItemId, (WORD)ptMsg->dwField09,
		ptMsg->dwField0d);
	return 0;
}

int __fastcall PlrMsgOnNpcRepairAll(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGID)))
		return 3;
	SUnitNpcRepairAll(ptGame, hPlayer, ((PLRMSGID *)pMsg)->dwId);
	return 0;
}

int __fastcall PlrMsgOnNpcHire(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGIDCODE *ptMsg;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGIDCODE)))
		return 3;
	ptMsg = (PLRMSGIDCODE *)pMsg;
	SUnitNpcHire(ptGame, hPlayer, ptMsg->dwId, ptMsg->wCode);
	return 0;
}

int __fastcall PlrMsgOnIdentify(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwItemId;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGITEM)))
		return 3;
	dwItemId = ((PLRMSGITEM *)pMsg)->dwItemId;
	if (!SUnitFind(ptGame, UNIT_ITEM, dwItemId))
		return 2;
	if (((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->dwBoughtItemId != dwItemId)
	{
#line 5228
		PLRMSG_HACK("tried to get free identify", __LINE__);
		FogErrorManagerLog("identify cheat detected for player %s\n",
			((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->szName);
		return 3;
	}
	SUnitNpcMarkBought(ptGame, hPlayer, dwItemId);
	return 0;
}

static inline int PlrMsgCheckUnitNear(GAME *ptGame, UNIT *hPlayer, DWORD dwUnitType, DWORD dwUnitId, int nRange)
{
	UNIT *hUnit;
	WORD wX, wY;

	hUnit = SUnitFind(ptGame, dwUnitType, dwUnitId);
	if (!hUnit)
		return 1;
	if (dwUnitType == UNIT_ITEM && ITEMSIsInInventory(hPlayer, hUnit, NULL))
		return 0;
	if (PlrMsgCheckAct(hPlayer, hUnit))
		return 2;
	wX = (WORD)D2CommonUnitGetX(hUnit);
	wY = (WORD)D2CommonUnitGetY(hUnit);
	return PlrMsgOutOfRange(hPlayer, wX, wY, nRange);
}

int __fastcall PlrMsgOnNpcInteract(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGNPC38 *ptMsg;
	DWORD dwAction, dwNpcId, dwItemId;
	int nRet;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGNPC38)))
		return 3;
	ptMsg = (PLRMSGNPC38 *)pMsg;
	dwAction = ptMsg->dwAction;
	dwNpcId = ptMsg->dwNpcId;
	dwItemId = ptMsg->dwField09;
	nRet = PlrMsgCheckUnitNear(ptGame, hPlayer, UNIT_MONSTER, dwNpcId, PLRMSG_RANGE);
	if (nRet)
		return nRet;
	SUnitNpcInteract(ptGame, hPlayer, dwAction, dwNpcId, dwItemId);
	return 0;
}

int __fastcall PlrMsgOnNpcBuyHeal(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGID)))
		return 3;
	SUnitNpcBuyHeal(ptGame, hPlayer, ((PLRMSGID *)pMsg)->dwId);
	return 0;
}

int __fastcall PlrMsgOnAddStatPoint(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	int nStat;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGWORD)))
		return 3;
	nStat = ((PLRMSGWORD *)pMsg)->wValue;
	if (nStat > -1 && nStat < NUM_PLAYER_STATS)
	{
		if (PlrEvalAddStatPoint(hPlayer, nStat))
			return 0;
		else
			return 2;
	}
#line 5293
	PLRMSG_HACK("tried to set an invalid stat", __LINE__);
	return 3;
}

static inline int PlrMsgCheckSkillPoint(UNIT *hPlayer, int nSkill)
{
	void *ptSkillsTxt;

	if (nSkill < 0 || nSkill >= NUM_SKILLS)
	{
#line 2983
		PLRMSG_HACK("client tried to set an invalid skill", __LINE__);
		return 2;
	}
	if (!SkillsIsClassSkill(hPlayer, nSkill))
	{
#line 2988
		PLRMSG_HACK("client tried to set an invalid skill", __LINE__);
		return 3;
	}
	ptSkillsTxt = (void *)D2CommonDataTblsGetSkill(nSkill);
	if (!D2CommonSkillsTestPrerequisites(hPlayer, ptSkillsTxt, nSkill))
	{
#line 2996
		PLRMSG_HACK("client tried to set an invalid skill", __LINE__);
		return 3;
	}
	if (!D2CommonSkillsTestStatRequirements(hPlayer, ptSkillsTxt, nSkill))
	{
#line 3001
		PLRMSG_HACK("client tried to set an invalid skill", __LINE__);
		return 3;
	}
	return 0;
}

int __fastcall PlrMsgOnAddSkillPoint(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	int nSkill, nRet;
	void *ptSkill;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGWORD)))
		return 3;
	nSkill = ((PLRMSGWORD *)pMsg)->wValue;
	nRet = PlrMsgCheckSkillPoint(hPlayer, nSkill);
	if (nRet)
	{
		if (nRet == 2)
			SCmdSendSkill(hPlayer, SKILL_ATTACK, 1, 0);
		return nRet;
	}
	ptSkill = (void *)D2CommonSkillsFindByKey(hPlayer, nSkill);
	if (ptSkill && (int)D2CommonSkillsCalcNodeLevel(hPlayer, ptSkill, 0) >= PLRMSG_MAXSKILLLEVEL)
	{
		SCmdSendSkill(hPlayer, SKILL_ATTACK, 1, 0);
		return 2;
	}
	SkillsAdd(hPlayer, nSkill);
	ITEMSModeRefreshItems(ptGame, hPlayer, TRUE);
	PlayerPetsUpdateMax(hPlayer);
	return 0;
}

extern BOOL PlrMsgCheckSelectSkill(UNIT *hPlayer, int nSkill);
__inline BOOL PlrMsgCheckSelectSkill(UNIT *hPlayer, int nSkill)
{
	void *ptSkill;

	if (nSkill < 0 || nSkill >= NUM_SKILLS)
	{
#line 3014
		PLRMSG_HACK("client tried to set an invalid skill", __LINE__);
		return FALSE;
	}
	ptSkill = (void *)D2CommonSkillsFindByKey(hPlayer, nSkill);
	if (!ptSkill)
	{
#line 3020
		PLRMSG_HACK("client tried to set an invalid skill", __LINE__);
		return FALSE;
	}
	if (!D2CommonSkillsCalcNodeLevel(hPlayer, ptSkill, 1))
	{
#line 3025
		PLRMSG_HACK("client tried to set an invalid skill", __LINE__);
		return FALSE;
	}
	return TRUE;
}

int __fastcall PlrMsgOnSelectSkill(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGSELECTSKILL *ptMsg;
	int nSkill;
	BOOL bLeft;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGSELECTSKILL)))
		return 3;
	ptMsg = (PLRMSGSELECTSKILL *)pMsg;
	nSkill = ptMsg->wSkill;
	bLeft = ptMsg->wLeft;
	if (!PlrMsgCheckSelectSkill(hPlayer, nSkill))
		return 3;
	SkillsSetCurrent(hPlayer, bLeft, nSkill);
	if (bLeft)
		D2CommonUnitSyncPlayerLeftSkill(hPlayer);
	else
		D2CommonUnitSyncPlayerRightSkill(hPlayer);
	return 0;
}

static inline int PlrMsgCheckUnitNearAct(GAME *ptGame, UNIT *hPlayer, DWORD dwUnitType, DWORD dwUnitId, int nRange)
{
	UNIT *hUnit;
	WORD wX, wY;

	hUnit = SUnitFind(ptGame, dwUnitType, dwUnitId);
	if (!hUnit)
		return 1;
	if (dwUnitType == UNIT_ITEM && ITEMSIsInInventory(hPlayer, hUnit, NULL))
		return 0;
	if (PlrMsgCheckActLevels(hPlayer, hUnit))
		return 2;
	wX = (WORD)D2CommonUnitGetX(hUnit);
	wY = (WORD)D2CommonUnitGetY(hUnit);
	return PlrMsgOutOfRange(hPlayer, wX, wY, nRange);
}

int __fastcall PlrMsgOnCloseDoor(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwObjectId;
	int nRet;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGID)))
		return 3;
	dwObjectId = ((PLRMSGID *)pMsg)->dwId;
	nRet = PlrMsgCheckUnitNearAct(ptGame, hPlayer, UNIT_OBJECT, dwObjectId, PLRMSG_OPERATERANGE);
	if (nRet)
		return nRet;
	ObjectModeDoorUpdate(ptGame, hPlayer, dwObjectId);
	return 0;
}

int __fastcall PlrMsgOnQuestUseItem(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwItemId;
	int nRet;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGID)))
		return 3;
	dwItemId = ((PLRMSGID *)pMsg)->dwId;
	nRet = PlrMsgCheckUnitNearAct(ptGame, hPlayer, UNIT_ITEM, dwItemId, PLRMSG_OPERATERANGE);
	if (nRet)
		return nRet;
	QuestsUseItem(ptGame, hPlayer, dwItemId);
	return 0;
}

int __fastcall PlrMsgOnChatSound(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	int nSound;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGWORD)))
		return 3;
	nSound = ((PLRMSGWORD *)pMsg)->wValue;
	if (nSound < PLRMSG_FIRSTSOUND || nSound > PLRMSG_LASTSOUND)
	{
#line 5422
		PLRMSG_HACK("tried to play a non-player chat sound", __LINE__);
		return 0;
	}
	if (!(hPlayer->dwFlags0 & 1024))
	{
		SUnitSetEvent(hPlayer, nSound, NULL);
		return 0;
	}
	return 1;
}

int __fastcall PlrMsgOnQuestsRequest(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	if (!PlrMsgCheckSize(nSize, 1))
		return 3;
	QuestsSendAllStatus(ptGame, hPlayer);
	return 0;
}

int __fastcall PlrMsgOnRestartInTown(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	CLIENT *hClient;
	DWORD dwArg;
	int nLife, nMana, nStamina;

	if (!PlrMsgCheckSize(nSize, 1))
		return 3;
	if (hPlayer->dwMode != PMODE_DEAD)
		return 0;
	hClient = SUnitGetClient(hPlayer);
	if (ClientTestFlags(hClient, 4))
	{
		GameLeaveClient(hClient, 3);
		return 0;
	}
	dwArg = ArenaUnitAlwaysFalse(ptGame, hPlayer);
	SkillsRestorePassives(hPlayer);
	nLife = D2CommonStatsUnitCalcMaxHp(hPlayer);
	nMana = D2CommonStatsUnitCalcMaxMana(hPlayer);
	nStamina = D2CommonStatsUnitCalcMaxStamina(hPlayer);
	D2CommonStatsUnitSetValue(hPlayer, STATS_HITPOINTS, nLife);
	D2CommonStatsUnitSetValue(hPlayer, STATS_MANA, nMana);
	D2CommonStatsUnitSetValue(hPlayer, STATS_STAMINA, nStamina);
	PlrMsgStatChanged(hPlayer, STATS_HITPOINTS, nLife, hPlayer);
	PlrMsgStatChanged(hPlayer, STATS_MANA, nMana, hPlayer);
	PlrMsgStatChanged(hPlayer, STATS_STAMINA, nStamina, hPlayer);
	hPlayer->dwFlags0 |= 2;
	PlrMsgWarpToLevel(ptGame, hPlayer, LEVEL_ID_NONE, dwArg);
	PlrModeStartXY(ptGame, hPlayer, NULL, PMODE_NEUTRAL, 0, 0, TRUE);
	SkillsEventPeriodic(ptGame, hPlayer);
	return 0;
}

int __fastcall PlrMsgOnPartyCmdWithByte(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGPARTY *ptMsg;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGPARTY)))
		return 3;
	if (ArenaHasGuild(ptGame))
	{
#line 5511
		PLRMSG_HACK("weird party message", __LINE__);
		return 3;
	}
	ptMsg = (PLRMSGPARTY *)pMsg;
	if (ptMsg->bCommand > PLRMSG_NUMPARTYCMDS)
		return 2;
	PartyScreenCommand(ptGame, hPlayer, ptMsg->bCommand, ptMsg->dwUnitId, ptMsg->bArg);
	return 0;
}

int __fastcall PlrMsgOnPartyCmd(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGPARTYID *ptMsg;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGPARTYID)))
		return 3;
	if (ArenaHasGuild(ptGame))
	{
#line 5543
		PLRMSG_HACK("weird party message", __LINE__);
		return 3;
	}
	ptMsg = (PLRMSGPARTYID *)pMsg;
	if (ptMsg->bCommand > PLRMSG_NUMPARTYCMDS)
		return 2;
	PartyScreenCommand(ptGame, hPlayer, ptMsg->bCommand, ptMsg->dwUnitId, 0);
	return 0;
}

int __fastcall PlrMsgCmdNone(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	return 0;
}

extern int PlrMsgCheckObeliskUse(GAME *ptGame, UNIT *hPlayer, DWORD dwObjectId, DWORD dwItemId, WORD wCall);
__inline int PlrMsgCheckObeliskUse(GAME *ptGame, UNIT *hPlayer, DWORD dwObjectId, DWORD dwItemId, WORD wCall)
{
	int nRet;

	nRet = PlrMsgCheckUnitNear(ptGame, hPlayer, UNIT_OBJECT, dwObjectId, PLRMSG_OPERATERANGE);
	if (nRet)
		return nRet;
	switch (wCall)
	{
	case 2:
		return 0;
	case 3:
		if (!PlrMsgIsCursorItem(ptGame, hPlayer, dwItemId))
			return 1;
		return 0;
	default:
		return 2;
	}
}

int __fastcall PlrMsgOnUseObject(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGOBJECTITEM *ptMsg;
	DWORD dwObjectId, dwItemId;
	WORD wCall;
	int nRet;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGOBJECTITEM)))
		return 3;
	ptMsg = (PLRMSGOBJECTITEM *)pMsg;
	dwObjectId = ptMsg->dwObjectId;
	dwItemId = ptMsg->dwItemId;
	wCall = ptMsg->wCall;
	nRet = PlrMsgCheckObeliskUse(ptGame, hPlayer, dwObjectId, dwItemId, wCall);
	if (nRet)
		return nRet;
	ObjectModeObeliskUse(ptGame, hPlayer->dwId, dwObjectId, dwItemId, wCall);
	return 0;
}

static inline int PlrMsgCheckPortal(GAME *ptGame, UNIT *hPlayer, DWORD dwObjectId, WORD wLevelId)
{
	UNIT *hObject;
	DWORD dwLevelBit;
	WORD wX, wY;

	hObject = SUnitFind(ptGame, UNIT_OBJECT, dwObjectId);
	if (!hObject)
		return 1;
	if (hObject->dwClassId != OBJECT_TOWNPORTAL)
	{
#line 3068
		PLRMSG_HACK("tried to set warp on a non-portal", __LINE__);
		return 3;
	}
	if (wLevelId >= PLRMSG_NUMLEVELS || wLevelId <= 0)
	{
#line 3074
		PLRMSG_HACK("tried to set warp with an invalid level id", __LINE__);
		return 3;
	}
	dwLevelBit = D2CommonDungeonGetPortalLevelBit(wLevelId);
	if (!(dwLevelBit & D2CommonUnitGetPlayerLevelBits(hPlayer)))
		return 2;
	if (PlrMsgCheckActLevels(hPlayer, hObject))
	{
#line 3087
		PLRMSG_HACK("tried to operate warp gate from different act", __LINE__);
		return 2;
	}
	wX = (WORD)D2CommonUnitGetX(hObject);
	wY = (WORD)D2CommonUnitGetY(hObject);
	if (PlrMsgOutOfRange(hPlayer, wX, wY, PLRMSG_OPERATERANGE))
	{
#line 3095
		PLRMSG_HACK("tried to operate warp gate from far away", __LINE__);
		return 1;
	}
	return 0;
}

int __fastcall PlrMsgOnSetWarp(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGWARP *ptMsg;
	DWORD dwObjectId;
	WORD wLevelId;
	int nRet;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGWARP)))
		return 3;
	ptMsg = (PLRMSGWARP *)pMsg;
	dwObjectId = ptMsg->dwObjectId;
	wLevelId = ptMsg->wLevelId;
	nRet = PlrMsgCheckPortal(ptGame, hPlayer, dwObjectId, wLevelId);
	if (nRet)
		return nRet;
	ObjectModeTownPortalLink(ptGame, hPlayer, dwObjectId, wLevelId);
	return 0;
}

static inline BOOL PlrMsgCommandHireling(GAME *ptGame, UNIT *hPlayer, DWORD dwPetId, int nCommand, DWORD dwArg1,
	DWORD dwArg2)
{
	UNIT *hPet;
	AICOMMAND tCommand;

	hPet = SUnitFind(ptGame, UNIT_MONSTER, dwPetId);
	if (!hPet || PlayerPetsFind(ptGame, hPlayer, PET_HIRELING) != hPet)
		return FALSE;
	AiClearCommands(ptGame, hPet);
	tCommand.nCommand = nCommand;
	tCommand.dwArg1 = dwArg1;
	tCommand.dwArg2 = dwArg2;
	AiPushCommand(ptGame, hPet, &tCommand);
	SUnitSetEvent(hPet, 14, hPlayer);
	return TRUE;
}

int __fastcall PlrMsgOnHirelingAttack(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGPETTARGET *ptMsg;
	DWORD dwPetId, dwUnitId, dwUnitType;
	int nRet;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGPETTARGET)))
		return 3;
	ptMsg = (PLRMSGPETTARGET *)pMsg;
	dwPetId = ptMsg->dwPetId;
	dwUnitId = ptMsg->dwUnitId;
	dwUnitType = ptMsg->dwUnitType;
	if (dwUnitType >= NUM_UNIT_TYPES)
		return 2;
	nRet = PlrMsgCheckUnitNearAct(ptGame, hPlayer, dwUnitType, dwUnitId, PLRMSG_RANGE);
	if (nRet)
		return nRet;
	if (!PlrMsgCommandHireling(ptGame, hPlayer, dwPetId, 12, dwUnitId, dwUnitType))
		return 1;
	return 0;
}

int __fastcall PlrMsgOnHirelingMove(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGPETMOVE *ptMsg;
	DWORD dwPetId;
	WORD wX, wY;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGPETMOVE)))
		return 3;
	ptMsg = (PLRMSGPETMOVE *)pMsg;
	dwPetId = ptMsg->dwPetId;
	wX = ptMsg->wX;
	wY = ptMsg->wY;
	if (PlrMsgOutOfRange(hPlayer, wX, wY, PLRMSG_RANGE))
		return 1;
	PlrMsgCommandHireling(ptGame, hPlayer, dwPetId, 13, wX, wY);
	return 0;
}

int __fastcall PlrMsgOnClearBusy(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	if (!PlrMsgCheckSize(nSize, 1))
		return 3;
	if (!((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->bBusy)
		return 2;
	PlayerClearBusy(hPlayer);
	return 0;
}

extern int PlrMsgCheckWaypoint(GAME *ptGame, UNIT *hPlayer, DWORD dwObjectId, WORD wLevelId);
__inline int PlrMsgCheckWaypoint(GAME *ptGame, UNIT *hPlayer, DWORD dwObjectId, WORD wLevelId)
{
	UNIT *hObject;
	PLRINFO *ptPlayerInfo;
	WORD wX, wY;
	DWORD dwWaypoint;
	WORD *pwWaypoints;

	hObject = SUnitFind(ptGame, UNIT_OBJECT, dwObjectId);
	if (!hObject)
	{
#line 3113
		PLRMSG_HACK("tried to operate an invalid waypoint", __LINE__);
		return 3;
	}
	if (PlrMsgCheckAct(hPlayer, hObject))
	{
#line 3118
		PLRMSG_HACK("tried to operate waypoint from different act", __LINE__);
		return 2;
	}
	wX = (WORD)D2CommonUnitGetX(hObject);
	wY = (WORD)D2CommonUnitGetY(hObject);
	if (PlrMsgOutOfRange(hPlayer, wX, wY, PLRMSG_OPERATERANGE))
	{
#line 3126
		PLRMSG_HACK("tried to operate waypoint from far away", __LINE__);
		return 1;
	}
	if (!wLevelId)
		return 0;
	if (wLevelId < PLRMSG_NUMLEVELS)
	{
		ptPlayerInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
#line 3144
		D2ASSERT(ptPlayerInfo);
		if (!D2CommonWaypointGetLevelIndex(wLevelId, &dwWaypoint))
		{
#line 3150
			PLRMSG_HACK("tried to set waypoint to invalid level", __LINE__);
			return 3;
		}
		pwWaypoints = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apwWaypoints[ptGame->bDifficultyLevel];
		if (!D2CommonWaypointTestField(pwWaypoints, dwWaypoint))
			return 2;
		return 0;
	}
#line 3136
	PLRMSG_HACK("tried to set waypoint with an invalid level id", __LINE__);
	return 3;
}

int __fastcall PlrMsgOnWaypointTravel(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGWARP *ptMsg;
	UNIT *hInteract, *hObject;
	DWORD dwObjectId;
	WORD wLevelId;
	int nRet;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGWARP)))
		return 3;
	ptMsg = (PLRMSGWARP *)pMsg;
	dwObjectId = ptMsg->dwObjectId;
	wLevelId = ptMsg->wLevelId;
	nRet = PlrMsgCheckWaypoint(ptGame, hPlayer, dwObjectId, wLevelId);
	if (nRet)
	{
		hInteract = SUnitGetInteractUnit(ptGame, hPlayer);
		hObject = SUnitFind(ptGame, UNIT_OBJECT, dwObjectId);
		if (hObject && hObject == hInteract)
			SUnitResetInteract(hPlayer);
		return nRet;
	}
	ObjectModeWaypointTravel(ptGame, hPlayer, dwObjectId, wLevelId);
	return 0;
}

int __fastcall PlrMsgOnClearIntroduced(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	int nClass;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGWORD)))
		return 3;
	nClass = ((PLRMSGWORD *)pMsg)->wValue;
	if (nClass >= PLRMSG_NUMMONSTERS)
		return 2;
	PlrIntroClearIntroduced(ptGame, hPlayer, nClass);
	return 0;
}

int __fastcall PlrMsgOnMarkUnitChanged(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGUNIT *ptMsg;
	DWORD dwUnitType, dwUnitId;
	UNIT *hUnit;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGUNIT)))
		return 3;
	ptMsg = (PLRMSGUNIT *)pMsg;
	dwUnitType = ptMsg->dwUnitType;
	dwUnitId = ptMsg->dwUnitId;
	if (dwUnitType >= NUM_UNIT_TYPES)
		return 2;
	hUnit = SUnitFind(ptGame, dwUnitType, dwUnitId);
	if (!hUnit)
		return 1;
	if (!D2CommonUnitFindsTestRoomsNear(hPlayer, hUnit))
		return 1;
	D2CommonUnitRoomAddToChangedList(hUnit);
	hUnit->dwFlags |= 65536;
	return 0;
}

int __fastcall PlrMsgOnTransmogrify(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	DWORD dwItemId;
	UNIT *hItem;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGID)))
		return 3;
	dwItemId = ((PLRMSGID *)pMsg)->dwId;
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem && dwItemId != (DWORD)-1)
		return 1;
	SkillsTransmogrify(ptGame, hPlayer, hItem);
	return 0;
}

int __fastcall PlrMsgOnTradeCommand(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGTRADE *ptMsg;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGTRADE)))
		return 3;
	ptMsg = (PLRMSGTRADE *)pMsg;
	PlrTradeCommand(ptGame, hPlayer, ptMsg->wCmd, ((DWORD)ptMsg->wArgHigh << 16) | ptMsg->wArgLow);
	return 0;
}

int __fastcall PlrMsgOnDropGold(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGGOLD *ptMsg;
	DWORD dwId, dwAmount;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGGOLD)))
		return 3;
	ptMsg = (PLRMSGGOLD *)pMsg;
	dwId = ptMsg->dwUnitId;
	dwAmount = ptMsg->dwAmount;
	if (!PlayerDropGold(ptGame, hPlayer, dwId, dwAmount))
	{
#line 5919
		PLRMSG_HACK("invalid gold drop", __LINE__);
		return 3;
	}
	return 0;
}

int __fastcall PlrMsgOnSetHotkey(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGHOTKEY *ptMsg;
	int nHotKey, nSkill, nLeft;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGHOTKEY)))
		return 3;
	ptMsg = (PLRMSGHOTKEY *)pMsg;
	nHotKey = ptMsg->wHotKey;
	if (nHotKey < 0 || nHotKey >= PLRMSG_NUMHOTKEYS)
	{
#line 5939
		PLRMSG_HACK("bad hotkey set", __LINE__);
		return 3;
	}
	nSkill = ptMsg->wSkill;
	if (nSkill == 65535)
		nSkill = -1;
	if (nSkill != -1)
	{
		if (nSkill < 0 || nSkill >= NUM_SKILLS)
		{
#line 5949
			PLRMSG_HACK("bad hotkey set", __LINE__);
			return 3;
		}
		if (!D2CommonSkillsFindByKey(hPlayer, nSkill))
		{
#line 5955
			PLRMSG_HACK("bad hotkey set", __LINE__);
			return 3;
		}
	}
	nLeft = ptMsg->wLeft;
	if (nLeft != 0 && nLeft != 1)
	{
#line 5963
		PLRMSG_HACK("bad hotkey set", __LINE__);
		return 3;
	}
	if (!PlayerSetClientPair(ptGame, hPlayer, nHotKey, (DWORD)nSkill, (DWORD)nLeft))
	{
#line 5969
		PLRMSG_HACK("bad hotkey set", __LINE__);
		return 3;
	}
	return 0;
}

int __fastcall PlrMsgOnConvert(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGID)))
		return 3;
#line 5987
	PLRMSG_HACK("someone tried to convert?  inconceivable", __LINE__);
	return 3;
}

int __fastcall PlrMsgOnStartRunning(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	if (!PlrMsgCheckSize(nSize, 1))
		return 3;
	if (hPlayer->eType == UNIT_PLAYER && hPlayer->dwMode == PMODE_WALK
		&& D2CommonStatsUnitGetValue(hPlayer, STATS_STAMINA))
		D2CommonUnitSetMode(hPlayer, PMODE_RUN);
	return 0;
}

int __fastcall PlrMsgOnStopRunning(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	if (!PlrMsgCheckSize(nSize, 1))
		return 3;
	if (hPlayer->eType == UNIT_PLAYER && hPlayer->dwMode == PMODE_RUN)
		D2CommonUnitSetMode(hPlayer, PMODE_WALK);
	return 0;
}

int __fastcall PlrMsgOnSetQuestFlag(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	int nQuest;
	void *ptRecord;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGWORD)))
		return 3;
	nQuest = ((PLRMSGWORD *)pMsg)->wValue;
	if (nQuest < PLRMSG_NUMQUESTFLAGS)
	{
		ptRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
		D2CommonQuestRecordSetBit(ptRecord, nQuest, 12);
		return 0;
	}
#line 6077
	PLRMSG_HACK("bad quest message", __LINE__);
	return 2;
}

int __fastcall PlrMsgOnGuildVaultItem(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGVAULT *ptMsg;
	DWORD dwItemId;
	void *hGuild;
	UNIT *hItem, *hVault, *hTrophy, *hFromVault, *hFromTrophy, *hNewItem;
	DWORD dwVaultId, dwTrophyId;
	GAMEITEMSTXT *ptItemStats;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGVAULT)))
		return 3;
	ptMsg = (PLRMSGVAULT *)pMsg;
	dwItemId = ptMsg->dwItemId;
	if (ptMsg->bSubCmd >= PLRMSG_NUMVAULTCMDS)
	{
#line 6096
		PLRMSG_HACK("bad guild message", __LINE__);
		return 3;
	}
	if (hPlayer->eType != UNIT_PLAYER)
	{
#line 6102
		PLRMSG_HACK("bad guild message", __LINE__);
		return 3;
	}
	hGuild = GuildsGetData(ptGame);
	if (!hGuild)
	{
#line 6109
		PLRMSG_HACK("bad guild message", __LINE__);
		return 3;
	}
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem)
		return 2;
	switch (ptMsg->bSubCmd)
	{
	case 0:
		dwVaultId = D2CommonGuildsGetVaultUnitId(hGuild);
		hVault = SUnitFind(ptGame, UNIT_OBJECT, dwVaultId);
		if (!hVault)
		{
#line 6124
			PLRMSG_HACK("bad guild message", __LINE__);
			return 3;
		}
		if (!PlrMsgIsCursorItem(ptGame, hPlayer, dwItemId))
			return 1;
		hNewItem = PlrMsgCopyItem(ptGame, hPlayer, hItem);
		D2CommonITEMSDataSetPage(hNewItem, 0);
		if (!ITEMSModePlaceCursorItem(ptGame, hVault, hNewItem->dwId,
			ptMsg->bX, ptMsg->bY, FALSE, TRUE, NULL))
			return 2;
		hNewItem->dwFlags |= 4;
		D2CommonUnitSetChanged(hVault, 1);
		(void)D2CommonINVENTORYAddDeltaNode(hVault->ptInventory, hNewItem);
		D2CommonITEMSDataSetCmdFlags(hItem, 1, 1);
		D2CommonINVENTORYSetCursorItem(hPlayer->ptInventory, 0);
		SCmdSendCursorCleared(SUnitGetClient(hPlayer), hPlayer);
		break;
	case 1:
		dwVaultId = D2CommonGuildsGetVaultUnitId(hGuild);
		hFromVault = SUnitFind(ptGame, UNIT_OBJECT, dwVaultId);
		if (!hFromVault)
		{
#line 6157
			PLRMSG_HACK("bad guild message", __LINE__);
			return 3;
		}
		if (!PlrMsgIsStoredItem(ptGame, hFromVault, hItem->dwId))
			return 1;
		if (D2CommonINVENTORYGetCursorItem(hPlayer->ptInventory))
		{
#line 6167
			PLRMSG_HACK("bad guild message", __LINE__);
			return 3;
		}
		if (!PlayerInteract(ptGame, hPlayer, D2CommonINVENTORYGetCursorItem(hPlayer->ptInventory)))
			return 0;
		hNewItem = PlrMsgCopyItem(ptGame, hPlayer, hItem);
		{
			DWORD dwFlags;
			CLIENT *hClient;

			hClient = SUnitGetClient(hPlayer);
			dwFlags = 0;
			D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
			D2CommonITEMSDataSetPreviousPage(hItem, D2CommonITEMSDataGetPageRaw((DWORD)hItem));
			SCmdSendItemFromGrid(hClient, hFromVault, hItem, dwFlags);
		}
		ITEMSModeRemoveGridItem(ptGame, hFromVault, hItem, FALSE);
		D2CommonITEMSDataSetPage(hNewItem, 255);
		D2CommonINVENTORYSetCursorItem(hPlayer->ptInventory, hNewItem);
		if (!ITEMSModeSetHandItem(ptGame, hPlayer, hNewItem))
		{
#line 6189
			PLRMSG_HACK("bad guild message", __LINE__);
			return 3;
		}
		return 0;
	case 2:
		dwTrophyId = D2CommonGuildsGetTrophyCaseUnitId(hGuild);
		hTrophy = SUnitFind(ptGame, UNIT_OBJECT, dwTrophyId);
		if (!hTrophy)
		{
#line 6202
			PLRMSG_HACK("bad guild message", __LINE__);
			return 3;
		}
		ptItemStats = (GAMEITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
		if (ptItemStats->code != ITEMCODE_EAR)
			return 2;
		if (!PlrMsgIsCursorItem(ptGame, hPlayer, dwItemId))
			return 1;
		hNewItem = PlrMsgCopyItem(ptGame, hPlayer, hItem);
		D2CommonITEMSDataSetPage(hNewItem, 0);
		if (!ITEMSModePlaceCursorItem(ptGame, hTrophy, hNewItem->dwId,
			ptMsg->bX, ptMsg->bY, FALSE, TRUE, NULL))
			return 2;
		hNewItem->dwFlags |= 4;
		D2CommonUnitSetChanged(hTrophy, 1);
		(void)D2CommonINVENTORYAddDeltaNode(hTrophy->ptInventory, hNewItem);
		D2CommonITEMSDataSetCmdFlags(hItem, 1, 1);
		D2CommonINVENTORYSetCursorItem(hPlayer->ptInventory, 0);
		SCmdSendCursorCleared(SUnitGetClient(hPlayer), hPlayer);
		break;
	case 3:
		dwTrophyId = D2CommonGuildsGetTrophyCaseUnitId(hGuild);
		hFromTrophy = SUnitFind(ptGame, UNIT_OBJECT, dwTrophyId);
		if (!hFromTrophy)
		{
#line 6239
			PLRMSG_HACK("bad guild message", __LINE__);
			return 3;
		}
		ptItemStats = (GAMEITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId);
		if (ptItemStats->code != ITEMCODE_EAR)
			return 2;
		if (!PlrMsgIsStoredItem(ptGame, hFromTrophy, hItem->dwId))
			return 1;
		if (D2CommonINVENTORYGetCursorItem(hPlayer->ptInventory))
		{
#line 6253
			PLRMSG_HACK("bad guild message", __LINE__);
			return 3;
		}
		if (!PlayerInteract(ptGame, hPlayer, D2CommonINVENTORYGetCursorItem(hPlayer->ptInventory)))
			return 0;
		hNewItem = PlrMsgCopyItem(ptGame, hPlayer, hItem);
		{
			DWORD dwFlags;
			CLIENT *hClient;

			hClient = SUnitGetClient(hPlayer);
			dwFlags = 0;
			D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
			D2CommonITEMSDataSetPreviousPage(hItem, D2CommonITEMSDataGetPageRaw((DWORD)hItem));
			SCmdSendItemFromGrid(hClient, hFromTrophy, hItem, dwFlags);
		}
		ITEMSModeRemoveGridItem(ptGame, hFromTrophy, hItem, FALSE);
		D2CommonITEMSDataSetPage(hNewItem, 255);
		D2CommonINVENTORYSetCursorItem(hPlayer->ptInventory, hNewItem);
		if (!ITEMSModeSetHandItem(ptGame, hPlayer, hNewItem))
		{
#line 6275
			PLRMSG_HACK("bad guild message", __LINE__);
			return 3;
		}
		return 0;
	}
	D2CommonGuildsSetChanged(hGuild, 1);
	return 0;
}

UNIT * __fastcall PlrMsgCopyItem(GAME *ptGame, UNIT *hPlayer, UNIT *hItem)
{
	void *pInventory;
	ITEMCREATE tCreate;
	UNIT *hNewItem;
	UNIT *hNewItemItem;
	UNIT *hSocketed;
	void *pNode;
	int nResult;

	ITEMSFillCreate(ptGame, &tCreate, hItem, 1);
	hNewItem = ITEMSRecreate(ptGame, &tCreate);
#line 6034
	D2ASSERT(hNewItem);
	if (!ITEMSModeHasContents(hItem))
		return hNewItem;
	pInventory = hItem->ptInventory;
	for (pNode = D2CommonINVENTORYGetCategoryListHead(pInventory); pNode;
		pNode = (void *)D2CommonINVENTORYNodeGetNext(pNode))
	{
		hSocketed = (UNIT *)D2CommonINVENTORYNodeGetKeyA(pNode);
		ITEMSFillCreate(ptGame, &tCreate, hSocketed, 1);
		hNewItemItem = ITEMSRecreate(ptGame, &tCreate);
#line 6043
		D2ASSERT(hNewItemItem);
		D2CommonUnitSetMode(hNewItemItem, IMODE_ONCURSOR);
		if (!ITEMSModeSocketGem(ptGame, hPlayer, hNewItemItem->dwId, hNewItem->dwId, &nResult, FALSE,
			TRUE, FALSE))
		{
#line 6048
			D2ASSERT(0);
		}
	}
	return hNewItem;
}

int __fastcall PlrMsgOnGuildVaultSwap(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGVAULTSWAP *ptMsg;
	DWORD dwCursorId, dwItemId;
	void *hGuild;
	UNIT *hCursor, *hItem;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGVAULTSWAP)))
		return 3;
	ptMsg = (PLRMSGVAULTSWAP *)pMsg;
	dwCursorId = ptMsg->dwCursorId;
	dwItemId = ptMsg->dwItemId;
	if (ptMsg->bSubCmd >= PLRMSG_NUMVAULTCMDS)
	{
#line 6362
		PLRMSG_HACK("bad guild message", __LINE__);
		return 3;
	}
	if (hPlayer->eType != UNIT_PLAYER)
	{
#line 6367
		PLRMSG_HACK("bad guild message", __LINE__);
		return 3;
	}
	hGuild = GuildsGetData(ptGame);
	if (!hGuild)
	{
#line 6374
		PLRMSG_HACK("bad guild message", __LINE__);
		return 3;
	}
	hCursor = SUnitFind(ptGame, UNIT_ITEM, dwCursorId);
	if (!hCursor)
		return 2;
	hItem = SUnitFind(ptGame, UNIT_ITEM, dwItemId);
	if (!hItem)
		return 2;
	if (!PlrMsgIsCursorItem(ptGame, hPlayer, dwCursorId))
		return 1;
	switch (ptMsg->bSubCmd)
	{
	case 0:
	{
		UNIT *hVault, *hNewItem;
		CLIENT *hClient;
		DWORD dwFlags;

		hVault = SUnitFind(ptGame, UNIT_OBJECT, D2CommonGuildsGetVaultUnitId(hGuild));
		if (!hVault)
		{
#line 6397
			PLRMSG_HACK("bad guild message", __LINE__);
			return 3;
		}
		hNewItem = PlrMsgCopyItem(ptGame, hPlayer, hItem);
		hClient = SUnitGetClient(hPlayer);
		dwFlags = 0;
		D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
		D2CommonITEMSDataSetPreviousPage(hItem, D2CommonITEMSDataGetPageRaw((DWORD)hItem));
		SCmdSendItemFromGrid(hClient, hVault, hItem, dwFlags);
		ITEMSModeRemoveGridItem(ptGame, hVault, hItem, FALSE);
		D2CommonITEMSDataSetPage(hCursor, 0);
		if (!ITEMSModePlaceCursorItem(ptGame, hVault, hCursor->dwId, ptMsg->bX, ptMsg->bY, FALSE, FALSE,
			NULL))
			return 2;
		hCursor->dwFlags |= 4;
		D2CommonUnitSetChanged(hVault, 1);
		(void)D2CommonINVENTORYAddDeltaNode(hVault->ptInventory, hCursor);
		D2CommonITEMSDataSetPage(hNewItem, 255);
		D2CommonINVENTORYSetCursorItem(hPlayer->ptInventory, hNewItem);
		if (!ITEMSModeSetHandItem(ptGame, hPlayer, hNewItem))
		{
#line 6430
			PLRMSG_HACK("bad guild message", __LINE__);
			return 3;
		}
		break;
	}
	case 1:
	{
		UNIT *hVault;
		int nItemQuantity;

		hVault = SUnitFind(ptGame, UNIT_OBJECT, D2CommonGuildsGetVaultUnitId(hGuild));
		if (!hVault)
		{
#line 6441
			PLRMSG_HACK("bad guild message", __LINE__);
			return 3;
		}
		nItemQuantity = D2CommonStatsUnitGetValue(hItem, STATS_QUANTITY);
		if (nItemQuantity < (int)D2CommonITEMSGetTypeMaxStack(hItem))
			PlrMsgVaultStack(ptGame, hPlayer, hVault, hCursor, hItem, 1);
		break;
	}
	case 2:
	{
		UNIT *hVault, *hNewItem, *hNewCursor;
		CLIENT *hClient;
		DWORD dwFlags;
		int nCursorQuantity, nItemQuantity, nMax, nMove, nX, nY;

		hVault = SUnitFind(ptGame, UNIT_OBJECT, D2CommonGuildsGetVaultUnitId(hGuild));
		if (!hVault)
		{
#line 6460
			PLRMSG_HACK("bad guild message", __LINE__);
			return 3;
		}
		hNewItem = PlrMsgCopyItem(ptGame, hPlayer, hItem);
		D2CommonITEMSDataSetPage(hNewItem, 0);
		nCursorQuantity = D2CommonStatsUnitGetValue(hCursor, STATS_QUANTITY);
		nItemQuantity = D2CommonStatsUnitGetValue(hItem, STATS_QUANTITY);
		nMax = (int)D2CommonITEMSGetTypeMaxStack(hItem);
		if (nItemQuantity + nCursorQuantity <= nMax)
		{
			PlrMsgVaultStack(ptGame, hPlayer, hVault, hCursor, hItem, nCursorQuantity);
			break;
		}
		nX = (int)D2CommonUnitGetX(hItem);
		nY = (int)D2CommonUnitGetY(hItem);
		hNewCursor = PlrMsgCopyItem(ptGame, hPlayer, hCursor);
		nMove = nMax - nItemQuantity;
		D2CommonStatsUnitAddValue(hNewItem, STATS_QUANTITY, nMove);
		D2CommonStatsUnitAddValue(hNewCursor, STATS_QUANTITY, -nMove);
		if (!ITEMSModeFreeCursorItem(ptGame, hPlayer, hCursor))
			return 2;
		hClient = SUnitGetClient(hPlayer);
		dwFlags = 0;
		D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
		D2CommonITEMSDataSetPreviousPage(hItem, D2CommonITEMSDataGetPageRaw((DWORD)hItem));
		SCmdSendItemFromGrid(hClient, hVault, hItem, dwFlags);
		ITEMSModeRemoveGridItem(ptGame, hVault, hItem, FALSE);
		D2CommonITEMSDataSetPage(hNewItem, 255);
		if (!ITEMSModeSetHandItem(ptGame, hPlayer, hNewItem))
		{
#line 6507
			PLRMSG_HACK("bad guild message", __LINE__);
			return 3;
		}
		D2CommonITEMSDataSetPage(hNewItem, 0);
		if (!ITEMSModePlaceCursorItem(ptGame, hVault, hNewItem->dwId, nX, nY, FALSE, FALSE, NULL))
			return 2;
		hNewItem->dwFlags |= 4;
		D2CommonUnitSetChanged(hVault, 1);
		(void)D2CommonINVENTORYAddDeltaNode(hVault->ptInventory, hNewItem);
		D2CommonINVENTORYSetCursorItem(hPlayer->ptInventory, hNewCursor);
		D2CommonITEMSDataSetPage(hNewCursor, 255);
		if (!ITEMSModeSetHandItem(ptGame, hPlayer, hNewCursor))
		{
#line 6529
			PLRMSG_HACK("bad guild message", __LINE__);
			return 3;
		}
		break;
	}
	case 3:
	{
		UNIT *hVault, *hNewItem;
		CLIENT *hClient;
		DWORD dwFlags;
		int nX, nY;

		hVault = SUnitFind(ptGame, UNIT_OBJECT, D2CommonGuildsGetTrophyCaseUnitId(hGuild));
		if (!hVault)
		{
#line 6541
			PLRMSG_HACK("bad guild message", __LINE__);
			return 3;
		}
		if (((GAMEITEMSTXT *)D2CommonItemTblsGetItemStats(hCursor->dwClassId))->code != ITEMCODE_EAR)
			return 2;
		if (((GAMEITEMSTXT *)D2CommonItemTblsGetItemStats(hItem->dwClassId))->code != ITEMCODE_EAR)
			return 2;
		hNewItem = PlrMsgCopyItem(ptGame, hPlayer, hItem);
		nX = (int)D2CommonUnitGetX(hItem);
		nY = (int)D2CommonUnitGetY(hItem);
		hClient = SUnitGetClient(hPlayer);
		dwFlags = 0;
		D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
		D2CommonITEMSDataSetPreviousPage(hItem, D2CommonITEMSDataGetPageRaw((DWORD)hItem));
		SCmdSendItemFromGrid(hClient, hVault, hItem, dwFlags);
		ITEMSModeRemoveGridItem(ptGame, hVault, hItem, FALSE);
		D2CommonITEMSDataSetPage(hCursor, 0);
		if (!ITEMSModePlaceCursorItem(ptGame, hVault, hCursor->dwId, nX, nY, FALSE, FALSE, NULL))
			return 2;
		hCursor->dwFlags |= 4;
		D2CommonUnitSetChanged(hVault, 1);
		(void)D2CommonINVENTORYAddDeltaNode(hVault->ptInventory, hCursor);
		D2CommonITEMSDataSetPage(hNewItem, 255);
		D2CommonINVENTORYSetCursorItem(hPlayer->ptInventory, hNewItem);
		if (!ITEMSModeSetHandItem(ptGame, hPlayer, hNewItem))
		{
#line 6583
			PLRMSG_HACK("bad guild message", __LINE__);
			return 3;
		}
		break;
	}
	}
	D2CommonGuildsSetChanged(hGuild, 1);
	return 0;
}

int __fastcall PlrMsgVaultStack(GAME *ptGame, UNIT *hPlayer, UNIT *hVault, UNIT *hCursor, UNIT *hItem,
	int nQuantity)
{
	UNIT *hNewItem;
	CLIENT *hClient;
	int nX, nY;
	DWORD dwFlags;

	hNewItem = PlrMsgCopyItem(ptGame, hPlayer, hItem);
	nX = (int)D2CommonUnitGetX(hItem);
	nY = (int)D2CommonUnitGetY(hItem);
	D2CommonStatsUnitAddValue(hNewItem, STATS_QUANTITY, nQuantity);
	if (!ITEMSModeFreeCursorItem(ptGame, hPlayer, hCursor))
		return 2;
	hClient = SUnitGetClient(hPlayer);
	dwFlags = 0;
	D2CommonITEMSSetOrClearFlags(&dwFlags, 32, 1);
	D2CommonITEMSDataSetPreviousPage(hItem, D2CommonITEMSDataGetPageRaw((DWORD)hItem));
	SCmdSendItemFromGrid(hClient, hVault, hItem, dwFlags);
	ITEMSModeRemoveGridItem(ptGame, hVault, hItem, FALSE);
	D2CommonITEMSDataSetPage(hNewItem, 255);
	if (!ITEMSModeSetHandItem(ptGame, hPlayer, hNewItem))
	{
#line 6326
		PLRMSG_HACK("bad stack message", __LINE__);
		return 3;
	}
	D2CommonITEMSDataSetPage(hNewItem, 0);
	if (!ITEMSModePlaceCursorItem(ptGame, hVault, hNewItem->dwId, nX, nY, FALSE, FALSE, NULL))
		return 2;
	hNewItem->dwFlags |= 4;
	D2CommonUnitSetChanged(hVault, 1);
	(void)D2CommonINVENTORYAddDeltaNode(hVault->ptInventory, hNewItem);
	return 0;
}

int __fastcall PlrMsgOnGuildHallText(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	char *szText;

	if ((DWORD)nSize < 2 || (DWORD)nSize > sizeof(PLRMSGGUILDTEXT) || pMsg[0] != 87 || pMsg[nSize - 1])
		return 2;
	szText = ((PLRMSGGUILDTEXT *)pMsg)->szText;
	GuildsPost(ptGame, SUnitGetClient(hPlayer), szText);
	if (!GuildsGetHall(ptGame))
	{
#line 6637
		PLRMSG_HACK("bad guild message", __LINE__);
		return 3;
	}
	GameForEachClient(ptGame, PlrMsgGuildTextEach, szText);
	return 0;
}

void __fastcall PlrMsgGuildTextEach(CLIENT *ptClient, void *pArg)
{
	UNIT *hPlayer, *hInteract;
	GAME *ptGame;

	hPlayer = ClientGetPlayerUnit(ptClient, FALSE);
	ptGame = ClientGetGame(ptClient);
	if (!hPlayer)
		return;
	hInteract = SUnitGetInteractUnit(ptGame, hPlayer);
	if (hInteract == GuildsGetHall(ptGame))
		SCmdSendText(ptClient, (const char *)pArg);
}

int __fastcall PlrMsgOnStopNpc(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGNPCINTERACT *ptMsg;
	DWORD dwUnitType, dwUnitId, dwUnitX, dwUnitY;
	UNIT *hUnit;
	MONSTATSTXT *ptMonStats;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGNPCINTERACT)))
		return 3;
	ptMsg = (PLRMSGNPCINTERACT *)pMsg;
	dwUnitType = ptMsg->dwUnitType;
	dwUnitId = ptMsg->dwUnitId;
	dwUnitX = ptMsg->dwUnitX;
	dwUnitY = ptMsg->dwUnitY;
	if (dwUnitType >= NUM_UNIT_TYPES)
		return 2;
	hUnit = SUnitFind(ptGame, dwUnitType, dwUnitId);
	if (!hUnit)
		return 1;
	ptMonStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(hUnit->dwClassId);
	if (ptMonStats && ptMonStats->bInteract && ptMonStats->bIsNPC)
	{
		if (hUnit->ptPath)
			D2CommonPathClearSteps(hUnit->ptPath);
		EventAdd(ptGame, hUnit, 2, ptGame->nFrame + 1);
		EventAdd(ptGame, hUnit, 2, ptGame->nFrame + 8);
		AiSetParam(hUnit, 1, 40);
		AiSetParam(hUnit, 2, dwUnitX);
		AiSetParam(hUnit, 3, dwUnitY);
	}
	return 0;
}

int __fastcall PlrMsgOnGuildSetText(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGGUILDSET *ptMsg;
	void *hGuild;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGGUILDSET)))
		return 3;
	hGuild = GuildsGetData(ptGame);
	if (((DWORD (__stdcall *)(WORD))D2CommonGuildsUnpackFieldRaw)(ClientGetGuildField(SUnitGetClient(hPlayer))) != GUILDRANK_LEADER)
	{
#line 6710
		PLRMSG_HACK("bad guild message", __LINE__);
		return 3;
	}
	ptMsg = (PLRMSGGUILDSET *)pMsg;
	switch (ptMsg->bSubCmd)
	{
	case 0:
		ptMsg->szText[218] = 0;
		D2CommonGuildsSetWebLink(hGuild, ptMsg->szText);
		return 0;
	case 1:
		ptMsg->szText[256] = 0;
		D2CommonGuildsSetCharter(hGuild, ptMsg->szText);
		return 0;
	}
	return 2;
}

int __fastcall PlrMsgOnGuildSettings(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGGUILDSETTING *ptMsg;

	if (nSize != sizeof(PLRMSGGUILDSETTING))
		return 2;
	ptMsg = (PLRMSGGUILDSETTING *)pMsg;
	if (ptMsg->bCmd != 91)
		return 2;
	if (!GuildsGetData(ptGame))
		return 2;
	if (((DWORD (__stdcall *)(WORD))D2CommonGuildsUnpackFieldRaw)(ClientGetGuildField(SUnitGetClient(hPlayer))) != GUILDRANK_LEADER)
	{
#line 6755
		PLRMSG_HACK("bad guild message", __LINE__);
		return 3;
	}
	D2CommonGuildsSetEmblem(GuildsGetData(ptGame), ptMsg->bIcon, ptMsg->bColor1, ptMsg->bColor2);
	return 0;
}

int __fastcall PlrMsgOnGuildMember(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGGUILDMEMBER *ptMsg;
	void *hGuild, *hMember;
	int nRank;
	char *szName;
	PFNGUILDEVENT pfnGuildEvent;
	CLIENT *ptClient, *ptNext;
	GAMEPLAYERMSG tMsg;
	WORD wGuildField;

	if ((DWORD)nSize <= 6 || (DWORD)nSize > sizeof(PLRMSGGUILDMEMBER) || pMsg[0] != 92 || pMsg[nSize - 1])
		return 2;
	hGuild = GuildsGetData(ptGame);
	if (!hGuild)
		return 2;
	ptMsg = (PLRMSGGUILDMEMBER *)pMsg;
	nRank = ptMsg->nRank;
	wGuildField = ClientGetGuildField(SUnitGetClient(hPlayer));
	if (((DWORD (__stdcall *)(WORD))D2CommonGuildsUnpackFieldRaw)(wGuildField) != GUILDRANK_LEADER)
	{
#line 6831
		PLRMSG_HACK("bad guild message", __LINE__);
		return 3;
	}
	szName = ptMsg->szName;
	hMember = (void *)D2CommonGuildsListFindByName(hGuild, szName);
	if (!hMember)
		return 2;
	switch (ptMsg->bSubCmd)
	{
	case 0:
		if (nRank < 0 || nRank >= GUILDRANK_LEADER)
			return 2;
		D2CommonGuildsSetMemberRank(hMember, nRank);
		if (ptEventCallbackTable->pfnGuildEvent)
			((PFNGUILDEVENT)ptEventCallbackTable->pfnGuildEvent)(D2CommonGuildsGetTag(hGuild), 0, szName, nRank);
lZero:
		return 0;
	case 1:
		if ((int)D2CommonGuildsGetMemberRank(hMember) >= GUILDRANK_LEADER)
			return 2;
		D2CommonGuildsListRemoveByName(hGuild, szName);
		if (ptEventCallbackTable->pfnGuildEvent)
			((PFNGUILDEVENT)ptEventCallbackTable->pfnGuildEvent)(D2CommonGuildsGetTag(hGuild), 1, szName, 0);
		ptClient = ClientsGetFirst(ptGame);
		if (!ptClient)
			goto lZero;
		for (; ptClient; ptClient = ptNext)
		{
			ptNext = ClientGetNext(ptClient);
			if (strcmpi(ClientGetName(ptClient), szName))
				continue;
			tMsg.bCmd = S2C_ROSTER_EVENT;
			tMsg.bEvent = 11;
			tMsg.bColor = 4;
			tMsg.dwArg = 0;
			tMsg.szAccount[0] = 0;
			StormSStrCopy(tMsg.szName, szName, sizeof(tMsg.szName));
			PlrMsgSendSystemMsgAll(ptGame, &tMsg);
			GameLeaveClient(ptClient, 11);
		}
		return 0;
	}
	return 2;
}

int __fastcall PlrMsgOnLongMove(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	PLRMSGXY *ptMsg;
	int nX, nY, nDistance;
	CLIENT *hClient;
	void *ptPath;
	DWORD dwCollisionMask, dwType, dwTargetX, dwTargetY;
	BYTE bDistMax;
	UNIT *hTarget;
	BOOL bReached;
	int nFrame;
	void *hStatList;
	DWORD dwRoom;

	if (!PlrMsgCheckSize(nSize, sizeof(PLRMSGXY)))
		return 3;
	ptMsg = (PLRMSGXY *)pMsg;
	nX = ptMsg->wX;
	nY = ptMsg->wY;
	hClient = SUnitGetClient(hPlayer);
#line 6966
	D2ASSERT(hClient);
	if (D2CommonUnitGetSkillThirdCached(hPlayer))
		return 0;
	nDistance = D2CommonUnitUtilGetDistance(hPlayer, nX, nY);
	if (nDistance < PLRMSG_MINRANGE)
		return 0;
	if (D2CommonStatsGetState(hPlayer, STATE_6C) || nDistance < PLRMSG_RUNRANGE)
	{
		if (SUnitGetTarget(ptGame, hPlayer))
			return 0;
		PlrModeStartXY(ptGame, hPlayer, NULL, hPlayer->dwMode == PMODE_RUN ? PMODE_RUN : PMODE_WALK, nX, nY,
			FALSE);
		return 0;
	}
	dwRoom = D2CommonUnitGetPathRoom(hPlayer);
	if (D2CommonDungeonFindRoomGame(dwRoom, nX, nY)
		&& !D2CommonStatsGetState(hPlayer, STATE_36) && !D2CommonStatsGetState(hPlayer, STATE_6C))
	{
		ptPath = hPlayer->ptPath;
		dwCollisionMask = D2CommonPathGetCollisionTestMask(ptPath);
		dwType = D2CommonPathGetType(ptPath);
		bDistMax = D2CommonPathGetDistMax(ptPath);
		hTarget = SUnitGetTarget(ptGame, hPlayer);
		dwTargetX = D2CommonPathGetTargetXRaw((DWORD)ptPath);
		dwTargetY = D2CommonPathGetTargetYRaw((DWORD)ptPath);
		D2CommonPathSetCollisionTestMask(ptPath, 1033);
		D2CommonPathSetType(ptPath, 15);
		D2CommonPathSetDistMax(ptPath, 99);
		D2CommonPathSetTargetCoord(ptPath, nX, nY);
		bReached = D2CommonPathCompute(ptPath, hPlayer);
		if (bReached)
			bReached = (DWORD)nX == D2CommonPathGetStepXRaw((DWORD)ptPath)
				&& (DWORD)nY == D2CommonPathGetStepYRaw((DWORD)ptPath);
		D2CommonPathSetCollisionTestMask(ptPath, dwCollisionMask);
		D2CommonPathSetType(ptPath, dwType);
		D2CommonPathSetDistMax(ptPath, bDistMax);
		if (hTarget)
			D2CommonPathSetTarget(ptPath, hTarget);
		else
			D2CommonPathSetTargetCoord(ptPath, dwTargetX, dwTargetY);
		if (bReached && SUnitMoveToRoom(ptGame, hPlayer, NULL, nX, nY, 0, 1))
		{
			nFrame = ptGame->nFrame + 125;
			hStatList = (void *)D2CommonStatsAllocList(ptGame->pMemPool, 2, nFrame, hPlayer->eType,
				hPlayer->dwId);
			D2CommonStatsSetListState(hStatList, STATE_6C);
			D2CommonStatsSetListRemoveCallback(hStatList, PlrMsgStateEnd);
			D2CommonStatsUnitLinkList(hPlayer, hStatList);
			EventAdd(ptGame, hPlayer, 12, nFrame);
			return 0;
		}
	}
	SCmdSendUnit(hClient, S2C_UNIT_WARP, (BYTE)hPlayer->eType, hPlayer->dwId, (WORD)D2CommonUnitGetX(hPlayer),
		(WORD)D2CommonUnitGetY(hPlayer), 0);
	return 0;
}

void __fastcall PlrMsgStateEnd(UNIT *hUnit, DWORD dwState)
{
	D2CommonStatsUnitSetActiveStateBit(hUnit, dwState, 0);
}

static const PLRMSGCHART sgcptIncomingMessageChart[PLRMSG_NUMCOMMANDS] =
{
	{ NULL },
	{ PlrMsgOnWalkToXY },
	{ PlrMsgOnWalkToUnit },
	{ PlrMsgOnRunToXY },
	{ PlrMsgOnRunToUnit },
	{ PlrMsgOnLeftSkillAtXY },
	{ PlrMsgOnLeftSkillAtUnitGo },
	{ PlrMsgOnLeftSkillAtUnit },
	{ PlrMsgOnLeftSkillAtXYAgain },
	{ PlrMsgOnLeftSkillAtUnitGoAgain },
	{ PlrMsgOnLeftSkillAtUnitAgain },
	{ PlrMsgOnEmpty },
	{ PlrMsgOnRightSkillAtXY },
	{ PlrMsgOnRightSkillAtUnitGo },
	{ PlrMsgOnRightSkillAtUnit },
	{ PlrMsgOnRightSkillAtXYAgain },
	{ PlrMsgOnRightSkillAtUnitGoAgain },
	{ PlrMsgOnRightSkillAtUnitAgain },
	{ PlrMsgOnClearStateBit },
	{ PlrMsgOnInteract },
	{ PlrMsgOnUnitText },
	{ PlrMsgOnChat },
	{ PlrMsgOnInteractWithFlag },
	{ PlrMsgOnDropCursorItem },
	{ sInsertItemGrid },
	{ sRemoveItemGrid },
	{ sinsertitembody },
	{ sInsertItemBodyRmvOpp },
	{ PlrMsgOnRemoveItemBodyLoc },
	{ sSwapItemBody },
	{ sDoubleSwapItemBody },
	{ sSwapItemGrid },
	{ sUseItemGridXY },
	{ PlrMsgOnItemStack },
	{ sRemoveStackItem },
	{ sInsertItemBelt },
	{ sRemoveItemBelt },
	{ sSwapItemBelt },
	{ sUseItemBeltXY },
	{ sUseItemAction },
	{ PlrMsgOnInsertItemItemInventory },
	{ PlrMsgOnAddScrollToBook },
	{ PlrMsgOnAddTransmogrifyBox },
	{ PlrMsgOnIgnored },
	{ PlrMsgOnInvalid },
	{ PlrMsgOnInvalidUnselectUnit },
	{ PlrMsgCmdNone },
	{ PlrMsgOnThreatTell },
	{ PlrMsgOnThreatDrop },
	{ PlrMsgOnQuestMonsterMessage },
	{ PlrMsgOnNpcBuy },
	{ PlrMsgOnNpcSell },
	{ PlrMsgOnNpcRepairAll },
	{ PlrMsgOnNpcRepairItem },
	{ PlrMsgOnNpcHire },
	{ PlrMsgOnIdentify },
	{ PlrMsgOnNpcInteract },
	{ PlrMsgOnNpcBuyHeal },
	{ PlrMsgOnAddStatPoint },
	{ PlrMsgOnAddSkillPoint },
	{ PlrMsgOnSelectSkill },
	{ PlrMsgOnCloseDoor },
	{ PlrMsgOnQuestUseItem },
	{ PlrMsgOnChatSound },
	{ PlrMsgOnQuestsRequest },
	{ PlrMsgOnRestartInTown },
	{ PlrMsgCmdNone },
	{ PlrMsgCmdNone },
	{ PlrMsgOnUseObject },
	{ PlrMsgOnSetWarp },
	{ PlrMsgOnHirelingAttack },
	{ PlrMsgOnHirelingMove },
	{ PlrMsgOnClearBusy },
	{ PlrMsgOnWaypointTravel },
	{ NULL },
	{ PlrMsgOnMarkUnitChanged },
	{ PlrMsgOnTransmogrify },
	{ PlrMsgOnClearIntroduced },
	{ NULL },
	{ PlrMsgOnTradeCommand },
	{ PlrMsgOnDropGold },
	{ PlrMsgOnSetHotkey },
	{ PlrMsgOnConvert },
	{ PlrMsgOnStartRunning },
	{ PlrMsgOnStopRunning },
	{ PlrMsgOnGuildVaultItem },
	{ PlrMsgOnGuildVaultSwap },
	{ PlrMsgOnGuildHallText },
	{ PlrMsgOnSetQuestFlag },
	{ PlrMsgOnStopNpc },
	{ PlrMsgOnGuildSetText },
	{ PlrMsgOnGuildSettings },
	{ PlrMsgOnGuildMember },
	{ PlrMsgOnPartyCmdWithByte },
	{ PlrMsgOnPartyCmd },
	{ PlrMsgOnLongMove },
};

int __fastcall PlrMsgHandleCommand(GAME *ptGame, UNIT *hPlayer, BYTE *pMsg, int nSize)
{
	BYTE bCmd;
	BOOL bRefused;

	bRefused = TRUE;
	bCmd = pMsg[0];
	if (!bCmd || bCmd >= PLRMSG_NUMCOMMANDS)
	{
#line 7147
		PLRMSG_HACK("client sent an invalid client command", __LINE__);
		return 3;
	}
	if (!sgcptIncomingMessageChart[bCmd].pfnProcess)
	{
#line 7152
		PLRMSG_HACK("client sent an invalid client command", __LINE__);
		return 3;
	}
	switch (bCmd)
	{
	case 60:
	case 67:
		bRefused = FALSE;
		break;
	case 65:
		if (hPlayer->dwMode == PMODE_DEAD)
			bRefused = FALSE;
		break;
	case 32:
	case 38:
		bRefused = !PlrCanBeInterrupted(hPlayer, TRUE, FALSE);
		break;
	default:
		bRefused = !PlrCanBeInterrupted(hPlayer, TRUE, TRUE);
		break;
	}
	hPlayer->dwKeptCmdPending = 0;
	if (!(bRefused)) {
#line 7212
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgcptIncomingMessageChart[bCmd].pfnProcess), "sgcptIncomingMessageChart[bCmd].pfnProcess");
		return sgcptIncomingMessageChart[bCmd].pfnProcess(ptGame, hPlayer, pMsg, nSize); }
	return 0;
}

void __fastcall PlrMsgSendKilled(GAME *ptGame, UNIT *hKiller, UNIT *hDead)
{
	GAMEPLAYERMSG tMsg;
	char szKiller[16];
	UNIT *hOwner;
	char *szName;
	WORD wUniqueId;

#line 7229
	D2ASSERT(UnitGetType (hDead) == UNIT_PLAYER);
	if (hKiller && hKiller->eType == UNIT_MONSTER)
	{
		hOwner = AiGetOwner(hKiller);
		if (hOwner && hOwner->eType == UNIT_PLAYER)
			hKiller = hOwner;
	}
	tMsg.bCmd = S2C_ROSTER_EVENT;
	tMsg.bEvent = 6;
	tMsg.bColor = 4;
	tMsg.dwArg = hKiller->dwClassId;
	tMsg.bSubKind = (BYTE)hKiller->eType;
	tMsg.szAccount[0] = 0;
	szName = ClientGetName(SUnitGetClient(hDead));
	if (StormSStrLen(szName))
		StormSStrCopy(tMsg.szName, szName, sizeof(tMsg.szName));
	else
		tMsg.szName[0] = 0;
	switch (hKiller->eType)
	{
	case UNIT_PLAYER:
		StormSStrCopy(szKiller, ClientGetName(SUnitGetClient(hKiller)), sizeof(szKiller));
		if (StormSStrLen(szKiller))
			StormSStrCopy(tMsg.szAccount, szKiller, sizeof(tMsg.szAccount));
		break;
	case UNIT_MONSTER:
		wUniqueId = MonsterUniqueGetId(((MONSTERDATA *)hKiller->uTypeData)->ptUnique);
		if (wUniqueId >= 65493)
			*(WORD *)tMsg.szAccount = wUniqueId;
		break;
	}
	PlrMsgSendSystemMsgAll(ptGame, &tMsg);
}

void __fastcall PlrMsgSetSaveEnabled(DWORD dwEnabled)
{
	gdwPlrSaveEnabled = dwEnabled;
}

int __fastcall PlrMsgLoadItem(GAME *ptGame, UNIT *hUnit, ITEMSAVE *ptItemSave, UNIT **phParent,
	int *pnAttached)
{
	UNIT *hItem;

	if (pnAttached)
		*pnAttached = 0;
	hItem = PlrSaveRecreateItem(ptGame, ptItemSave);
	if (!hItem)
		return 12;
	switch (hItem->dwMode)
	{
	case IMODE_STORED:
	{
		DWORD dwX, dwY;
		BYTE bWidth, bHeight;

		D2CommonUnitSetMode(hItem, IMODE_ONCURSOR);
		dwX = ptItemSave->dwX;
		dwY = ptItemSave->dwY;
		D2CommonITEMSGetTypeInvWidthHeight(hItem, &bWidth, &bHeight);
		D2CommonUnitSetX(hItem, dwX);
		D2CommonUnitSetY(hItem, dwY);
		if (!ITEMSModePlaceCursorItem(ptGame, hUnit, hItem->dwId, D2CommonUnitGetX(hItem), D2CommonUnitGetY(hItem),
			FALSE, TRUE, NULL))
			return 12;
		break;
	}
	case IMODE_EQUIP:
	{
		int nResult;

		D2CommonUnitSetMode(hItem, IMODE_ONCURSOR);
		if (!ITEMSModeEquipCursorItem(ptGame, hUnit,
			hItem->dwId, ptItemSave->bLocation, TRUE, &nResult))
			return 12;
		break;
	}
	case IMODE_INBELT:
	{
		int nResult;

		D2CommonUnitSetMode(hItem, IMODE_ONCURSOR);
		D2CommonUnitSetX(hItem, ptItemSave->dwX);
		D2CommonUnitSetY(hItem, ptItemSave->dwY);
		if (!ITEMSModePutInBelt(ptGame, hUnit, hItem->dwId, ptItemSave->dwX, FALSE, &nResult))
			return 12;
		break;
	}
	case IMODE_ONCURSOR:
		D2CommonUnitSetMode(hItem, IMODE_ONCURSOR);
		D2CommonINVENTORYSetCursorItem(hUnit->ptInventory, hItem);
		if (!ITEMSModeSetHandItem(ptGame, hUnit, hItem))
			return 12;
		break;
	case IMODE_SOCKETED:
	{
		int nResult;

		D2CommonUnitSetMode(hItem, IMODE_ONCURSOR);
		if (!ITEMSModeSocketGem(ptGame, hUnit, hItem->dwId, (*phParent)->dwId, &nResult, FALSE, TRUE,
			FALSE))
			return 12;
		break;
	}
	default:
		return 12;
	}
	if (ptItemSave->bNumItemsAttached > 0)
		*phParent = hItem;
	if (pnAttached)
		*pnAttached = ptItemSave->bNumItemsAttached;
	return 0;
}
