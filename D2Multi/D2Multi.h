#ifndef D2RE_D2MULTI_H
#define D2RE_D2MULTI_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "D2WinTypes.h"
#include "D2MultiExt.h"
#include "D2MultiImports.h"
#include "D2Macros.h"

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES	((DWORD)-1)
#endif

#define MAX_CHILDREN		32
#define NUM_CONTROLS		143
#define NUM_LOBBY_CONTROLS	17
#define NUM_LOBBY_BUTTONS	11
#define NUM_HISTORY			10
#define KEEPALIVE_TICKS		15000
#define DOUBLECLICK_TICKS	500
#define NUM_PROFILE_KEYS	4

enum
{
	REQUEST_NONE = 0,
	REQUEST_CREATE = 1,
	REQUEST_JOIN = 2,
	REQUEST_GAMEINFO = 3
};

enum
{
	MULTIMODE_GAME = 1,
	MULTIMODE_FRONTEND = 4
};

typedef BOOL (__stdcall *PFNCHATDONE)(DWORD dwArg);

#define NUM_COMEVENTS		24
#define MAX_CHARNAMES		1024
#define USERS_SHOWN			11

typedef D2WINCONTROL *CHATLOBBY[NUM_LOBBY_CONTROLS];
typedef D2WINCONTROL *CHATBUTTONS[NUM_LOBBY_BUTTONS];
typedef char CHATADTEXT[1024];
typedef char CHATREPLYNAME[256];

struct D2CHATUSER
{
 	DWORD		dwProduct;
 	char		szName[16];
 	char		szCharName[16];
 	char		szRealmName[32];
 	BYTE		abCodes[16];
 	BYTE		abColors[16];
 	BYTE		bClass;
 	BYTE		unobserved_65;
 	WORD		wLevel;
 	WORD		wCharFlags;
 	WORD		field_6a;
 	DWORD		dwFlags;
 	BYTE		abEmblem[3];
 	char		szBracedText[5];
 	D2COMPOSITEUNIT	*hUnit;
 	D2CHATUSER	*ptNext;
};

extern D2WIN_STRUCT gatControls[NUM_CONTROLS];

extern WORD gwNextToken;
extern CHATREPLYNAME gszReplyName;
extern BNCLIENTINTERFACE *gptComInt;
extern CHATADTEXT gszAdText;
extern DWORD gdwAdId;
extern CHATLOBBY gahLobby;
extern CHATBUTTONS gahLobbyButtons;
extern INIT_PARAMS *gptParams;
extern WORD gwRequestToken;
extern WORD gwGameInfoToken;
extern int gnRequest;
extern HCELFILE ghCelAd;
extern D2WINCONTROL *ghUserList;
extern BOOL gfNotify;
extern D2WINCONTROL *ghChatEdit;
extern char gchBlank;
extern D2CHATUSER *gptSelectedUnit;
extern BOOL gfPopupOpen;

void __fastcall ChatDlgClearPanels(void);
void __fastcall ChatDlgLoadCels(void);
void __fastcall ChatDlgFreeCels(void);
BOOL __fastcall ChatDlgFocusChat(void);
BOOL __stdcall ChatDlgCallStored(DWORD dwArg);
BOOL __stdcall ChatDlgSetCallback(DWORD dwUnused, PFNCHATDONE lpCallback);
BOOL __fastcall ChatDlgOkBox(const WORD *pwszText, PFNCHATDONE pfnDone);
void __fastcall ChatDlgClosePanels(void);
BOOL __stdcall ChatDlgIdle(int nPass);
BOOL __stdcall ChatDlgWaitDone(DWORD dwArg);
int __fastcall ChatDlgRealmLost(void);
BOOL __fastcall ChatDlgMain(INIT_PARAMS *ptParams);
BOOL __fastcall ChatDlgCreateGame(D2MCPGAMECREATE *ptGame, const char *pszServerIp);
BOOL __fastcall ChatDlgJoinGame(WORD wGameId, const char *pszName, const char *pszPassword,
	const char *pszServerIp);
int __fastcall ChatDlgRun(INIT_PARAMS *ptParams);
BOOL __fastcall ChatDlgAddLine(const WORD *pwszText, DWORD dwColor);
BOOL __fastcall ChatDlgAddLineTwin(const WORD *pwszText, DWORD dwColor);
BOOL __fastcall ChatDlgSetTitle(const WORD *pwszText);
BOOL __stdcall ChatDlgOnLeave(DWORD dwArg);
BOOL __stdcall ChatDlgChatEnter(const WORD *pwszText);
BOOL __fastcall ChatDlgSendText(char *pszText, BOOL fWhisper);
void __fastcall ChatDlgDestroyPanelChildren(void);
void __fastcall ChatDlgMessageBox(const WORD *pwszText, PFNCHATDONE pfnDone);
void __fastcall ChatDlgMessageBoxCancel(const WORD *pwszText, PFNCHATDONE pfnDone);
void __fastcall ChatDlgGreyRadio(D2WINCONTROL *ptControl);
BOOL __stdcall ChatDlgProfileIdle(int nPass);
BOOL __fastcall ChatDlgGetProfile(const char *pszAccount, char *pszSex, char *pszAge, char *pszLocation,
	char *pszDescription);
int __fastcall ChatDlgSetProfile(const char *pszAccount, const char *pszSex, const char *pszAge,
	const char *pszLocation, const char *pszDescription);

typedef BOOL (__fastcall *PFNCOMEVENT)(DWORD dwFlags, DWORD dwArg, const char *pszName,
	const char *pszText);

BOOL __fastcall ComCallbackPump(void);
void __fastcall ComCallbackScrollUsers(int nLines);
BOOL __stdcall ComCallbackOnUserButton(_PARAMS *ptMsg);
BOOL __stdcall ComCallbackOnSend(_PARAMS *ptMsg);
BOOL __stdcall ComCallbackOnWhisper(_PARAMS *ptMsg);
BOOL __stdcall ComCallbackOnSquelch(_PARAMS *ptMsg);
BOOL __stdcall ComCallbackOnPopup(_PARAMS *ptMsg);
BOOL __stdcall ComCallbackOnUnsquelch(_PARAMS *ptMsg);
BOOL __stdcall ComCallbackOnEmote(_PARAMS *ptMsg);
BOOL __stdcall ComCallbackOnAway(_PARAMS *ptMsg);
BOOL __fastcall ComCallbackPlayTempFile(void);
char * __fastcall ComCallbackChannelName(void);
BOOL __fastcall ComCallbackEventNone(DWORD dwFlags, DWORD dwArg, const char *pszName,
	const char *pszText);
BOOL __fastcall ComCallbackEventUser(DWORD dwFlags, DWORD dwArg, const char *pszName,
	const char *pszText);
BOOL __fastcall ComCallbackEventJoin(DWORD dwFlags, DWORD dwArg, const char *pszName,
	const char *pszText);
void __fastcall ComCallbackFreeUsers(void);
BOOL __fastcall ComCallbackEventLeave(DWORD dwFlags, DWORD dwArg, const char *pszName,
	const char *pszText);
BOOL __fastcall ComCallbackEventChannel(DWORD dwFlags, DWORD dwArg, const char *pszName,
	const char *pszText);
BOOL __fastcall ComCallbackEventFlags(DWORD dwFlags, DWORD dwArg, const char *pszName,
	const char *pszText);
BOOL __fastcall ComCallbackEventText(DWORD dwFlags, DWORD dwArg, const char *pszName,
	const char *pszText);
void __fastcall ComCallbackEchoLine(const char *pszLine);
void __fastcall ComCallbackAddCharName(const char *pszAccount, const char *pszChar);
void __fastcall ComCallbackGetNames(const char *pszAccount, const char **ppszTitle,
	const char **ppszChar);
BOOL __fastcall ComCallbackEventTalk(DWORD dwFlags, DWORD dwArg, const char *pszName,
	const char *pszText);
BOOL __fastcall ComCallbackTrue(void);
BOOL __fastcall ComCallbackEventInfo(DWORD dwFlags, DWORD dwArg, const char *pszName,
	const char *pszText);
BOOL __fastcall ComCallbackEventError(DWORD dwFlags, DWORD dwArg, const char *pszName,
	const char *pszText);
BOOL __fastcall ComCallbackEventEmote(DWORD dwFlags, DWORD dwArg, const char *pszName,
	const char *pszText);
BOOL __fastcall ComCallbackEventWhisper(DWORD dwFlags, DWORD dwArg, const char *pszName,
	const char *pszText);
BOOL __fastcall ComCallbackEventWhisperSent(DWORD dwFlags, DWORD dwArg, const char *pszName,
	const char *pszText);
BOOL __fastcall ComCallbackEventFalse(DWORD dwFlags, DWORD dwArg, const char *pszName,
	const char *pszText);
void __fastcall ComCallbackUpdateButtons(void);
void __fastcall ComCallbackAppendColor(WORD *pszSrc, int nColor);
void __fastcall ComCallbackPrependColor(WORD *pszSrc, int nColor);

struct GATEGAME;
const char * __fastcall GateNextString(const char *pszStr);
BOOL __fastcall GateLevelTooHigh(const char *pszLevel);
void __fastcall GateRequestList(void);
void __fastcall GateAddGame(D2MCPGAMEINFO *ptInfo);
void __fastcall GateOpenJoin(void);
void __fastcall GateRefreshList(void);
void __fastcall GateShowList(void);
void __fastcall GateSetInfoRequest(const char *pszName, WORD wToken);
void __fastcall GateRequestInfo(void);
BOOL __stdcall GateOnListClick(_PARAMS *ptMsg);
void __fastcall GateShowInfo(D2MCPGAMEINFO *ptInfo);
void __fastcall GateCloseJoin(void);
void __fastcall GateOpenCreate(void);
void __fastcall GateCloseCreate(void);
BOOL __stdcall GateOnLevelCheck(_PARAMS *ptMsg);
BOOL __stdcall GateOnPlayersUp(_PARAMS *ptMsg);
BOOL __stdcall GateOnPlayersDown(_PARAMS *ptMsg);
BOOL __stdcall GateOnLevelUp(_PARAMS *ptMsg);
BOOL __stdcall GateOnLevelDown(_PARAMS *ptMsg);
BOOL __stdcall GateCreateIdle(int nPass);
BOOL __stdcall GateOnCreate(_PARAMS *ptMsg);
int __fastcall GateFindGame(const char *pszName, const char *pszPassword,
	GATEGAME *ptGame);
BOOL __stdcall GateOnJoin(_PARAMS *ptMsg);
BOOL __stdcall GateOnJoinButton(_PARAMS *ptMsg);
BOOL __stdcall GateOnCreateButton(_PARAMS *ptMsg);
BOOL __stdcall GateOnColumn(_PARAMS *ptMsg);
BOOL __stdcall GateOnNormal(_PARAMS *ptMsg);
BOOL __stdcall GateOnNightmare(_PARAMS *ptMsg);
BOOL __stdcall GateOnHell(_PARAMS *ptMsg);
void __fastcall GateShowMyProfile(void);
void __fastcall GateShowProfile(const char *pszAccount);
void __fastcall GateClosePanel(void);
BOOL __stdcall GateOnPanelCancel(_PARAMS *ptMsg);
BOOL __stdcall GateOnPanelOk(_PARAMS *ptMsg);
BOOL __fastcall GateOpenPopup(void);
BOOL __fastcall GateClosePopup(void);

#endif
