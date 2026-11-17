#define D2_THIS_FILE "C:\\D2\\Source\\D2Multi\\Src\\ChatDlg.cpp"
#include "D2Multi.h"

struct D2CHATNAME
{
 	char		szText[512];
 	DWORD		dwKind;
 	D2CHATNAME	*ptNext;
};

struct D2CHATLADDERROW
{
 	DWORD		dwExperienceLo;
 	DWORD		dwExperienceHi;
 	union {
				DWORD	dwPacked;
				WORD	wClassTitle;
			};
 	char		szName[1];
};

typedef D2WINCONTROL *CHATSUBCHILDREN[MAX_CHILDREN];
typedef char CHATHISTORY[NUM_HISTORY][256];
typedef char *CHATPROFILEVALUES[NUM_PROFILE_KEYS];
typedef const char *CHATPROFILEKEYS[NUM_PROFILE_KEYS];

static HANDLE sgh100271e0 = 0;
static CHATPROFILEVALUES sgapszProfileValues = { 0 };
static char sgch100271f4 = 0;
CHATREPLYNAME gszReplyName = { 0 };
static char sgch100272f8 = 0;
static DWORD sgadw100272fc[6] = { 0 };
static BOOL sgfProfileCancel = 0;
BNCLIENTINTERFACE *gptComInt = 0;
static CHATSUBCHILDREN sgaptSubChildren = { 0 };
CHATADTEXT gszAdText = { 0 };
DWORD gdwAdId = 0;
static CHATHISTORY sgaszHistory = { 0 };
CHATLOBBY gahLobby = { 0 };
CHATBUTTONS gahLobbyButtons = { 0 };
INIT_PARAMS *gptParams = 0;
WORD gwRequestToken = 0;
WORD gwGameInfoToken = 0;
int gnRequest = 0;
static int sgnNumSubChildren = 0;
static DWORD sgdwGemActive = 0;
static DWORD sgdw10028224 = 0;
static HCELFILE sghCelBnet = 0;
static HCELFILE sghCelHand = 0;
static HCELFILE sghCelRotate = 0;
static HCELFILE sghCelScrollbar = 0;
static HCELFILE sghCelJoinScrollbars = 0;
HCELFILE ghCelAd = 0;
static HCELFILE sghCelChatLeft = 0;
static HCELFILE sghCelChatLeft1 = 0;
static HCELFILE sghCelChatLeft2 = 0;
static HCELFILE sghCelChatRight = 0;
static HCELFILE sghCelChatRightTop = 0;
static HCELFILE sghCelChatArrows = 0;
D2WINCONTROL *ghUserList = 0;
static D2WINCONTROL *sghGem = 0;
static D2WINCONTROL *sghNamesBox = 0;
static HCELFILE sghCelJoinBckg = 0;
static HCELFILE sghCelBlankBckg2 = 0;
static HCELFILE sghCelGemButton = 0;
static HCELFILE sghCelGameButton = 0;
static HCELFILE sghCelJoinClickBox = 0;
static HCELFILE sghCelGreyLevelBox = 0;
static HCELFILE sghCelRadio = 0;
static HCELFILE sghCelRadioGrey = 0;
static HCELFILE sghCelPopUpOk = 0;
static HCELFILE sghCelCancel = 0;
static HCELFILE sghCelSquelchAura = 0;
static HCELFILE sghCelSelectAura = 0;
static HCELFILE sghCelNumberArrows1 = 0;
static HCELFILE sghCelNumberArrows2 = 0;
static HCELFILE sghCelNumberArrows3 = 0;
static HCELFILE sghCelNumberArrows4 = 0;
static HCELFILE sghCelNumberGreyArrows = 0;
static HCELFILE sghCelCreateJoinBckg = 0;
static HCELFILE sghCelOkayButton = 0;
static D2WINCONTROL *sghSubBckg = 0;
static D2WINCONTROL *sghSubTitle = 0;
static D2WINCONTROL *sghSubEditLabel = 0;
static D2WINCONTROL *sghSubListLabel = 0;
static D2WINCONTROL *sghSubOk = 0;
static D2WINCONTROL *sghSubCancel = 0;
static D2WINCONTROL *sghSubEdit = 0;
static D2WINCONTROL *sgh100282cc = 0;
static D2WINCONTROL *sghSubList = 0;
static D2WINCONTROL *sghLadderList = 0;
static D2WINCONTROL *sgh100282d8 = 0;
static D2WINCONTROL *sghLadderClassRadio = 0;
static D2WINCONTROL *sghLadderAllRadio = 0;
static HCELFILE sghCelCreateGameBckg = 0;
static HCELFILE sghCelCreateGameButton = 0;
static HCELFILE sghCel100282ec = 0;
static HCELFILE sghCelWideButton = 0;
static HCELFILE sgahCel100282f4[4] = { 0 };
static HCELFILE sghCelCancelBlank = 0;
static HCELFILE sghCel10028308 = 0;
static HCELFILE sghCelLadderChoice = 0;
static HCELFILE sghCelHardcoreLadder = 0;
static HCELFILE sghCelPlayerProfile = 0;
static HCELFILE sghCelEditProfile = 0;
static HCELFILE sghCelHelpBckg = 0;
static HCELFILE sghCelLogoBlack = 0;
static HCELFILE sghCelLogo = 0;
static int sgnHistory = 0;
BOOL gfNotify = 0;
static BOOL sgfOpenBattleNet = 0;
static D2WINLISTENTRY sgtListEntry10028338 = { 0 };
static D2WINLISTENTRY sgtListEntry10028348 = { 0 };
static D2WINFONTDESC sgtFont10028358 = { 0 };
static D2WINCONTROL *sghChatBox = 0;
static D2WINCONTROL *sgh10028364 = 0;
D2WINCONTROL *ghChatEdit = 0;
static D2WINCONTROL *sghTitleBox = 0;
static D2WINCONTROL *sghOkBox = 0;
static int sgnNextMode = 0;
static PFNCHATDONE sgpfnDone = 0;
static DWORD sgdwLastClick = 0;
static DWORD sgdwKeepAliveTick = 0;
static DWORD sgdwGameInfoTick = 0;
static DWORD sgdwListTick = 0;
static D2CHATNAME *sgptNames = 0;

static CHATPROFILEKEYS sgapszProfileKeys =
{
	"profile\\sex", "profile\\age", "profile\\location", "profile\\description",
};
WORD gwNextToken = 1;
static int sgnHistoryIndex = -1;
static int sgn10022a48 = -1;

#define ChatStrNewTable(hArchive) \
	(((DWORD (__fastcall *)(DWORD))D2LangStrNewTableRaw)((DWORD)(hArchive)))

static BOOL __stdcall ChatDlgOnQuit(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnUsersPageUp(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnUsersPageDown(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnUsersLineUp(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnUsersLineDown(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnAd(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnChannels(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnLadder(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnChannelOk(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnChannelList(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgChannelKey(D2WINEDITBOX *ptEdit, int nPos, char *pchKey);
static BOOL __stdcall ChatDlgOnOkDone(DWORD dwArg);
static void __fastcall ChatDlgDestroyPanelControls(void);
static D2WINCONTROL * __fastcall ChatDlgCreatePanelChild(int nControl);
static void __fastcall ChatDlgKeepAlive(void);
static BOOL __stdcall ChatDlgChatKey(D2WINEDITBOX *ptEdit, int nPos, char *pchKey);
static BOOL __stdcall ChatDlgChatPost(DWORD dwZero);
static BOOL __stdcall ChatDlgChannelPost(DWORD dwZero);
static BOOL __stdcall ChatDlgOnBoxDone(DWORD dwArg);
static BOOL __stdcall ChatDlgOnBoxButton(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnPanelCancel(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnLadderBack(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnHardcoreLadder(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnStandardLadder(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnLadderCancel(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnHardcoreLadderByClass(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnHardcoreLadderOverall(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnStandardLadderByClass(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnStandardLadderOverall(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnGem(_PARAMS *ptMsg);
static BOOL __stdcall ChatDlgOnPopupClose(_PARAMS *ptMsg);
static void __fastcall ChatDlgOnNews(DWORD dwKind, DWORD dwLength, const char *pszText);
static void __fastcall ChatDlgCreateHardcoreLadder(void);
static void __fastcall ChatDlgCreateStandardLadder(void);
static void __fastcall ChatDlgLadderRows(int nRows, int nNameSize, BYTE *pbRows);
static void __fastcall ChatDlgLadderClassRows(int nRows, int nNameSize, BYTE *pbRows);
static BOOL __stdcall ChatDlgOnProfileCancel(DWORD dwArg);

static D2WINFONTDESC sgatFontDesc[5] =
{
	{ 4, 0 }, { 3, 0 }, { 2, 0 }, { 1, 0 }, { 7, 0 },
};

struct CHATBAR
{
 	D2WINTEXTBOXBAR	tBar;
 	DWORD		unobserved_0c;
};

static CHATBAR sgatBars[7] =
{
	{ { 400, 390, 314 }, 0 }, { { 592, 393, 173 }, 0 }, { { 753, 394, 196 }, 0 },
	{ { 753, 393, 197 }, 0 }, { { 756, 391, 238 }, 0 }, { { 756, 391, 266 }, 0 },
	{ { 753, 391, 266 }, 0 },
};

static D2WINANIMSTATE sgatAnimStates[5] =
{
	{ &sghCelLogoBlack, &sghCelLogo, 3, 1 }, { &sghCelLogoBlack, &sghCelLogo, 3, 1 },
	{ &sghCelLogoBlack, &sghCelLogo, 3, 1 }, { &sghCelLogoBlack, &sghCelLogo, 3, 1 },
	{ &sghCelLogoBlack, &sghCelLogo, 3, 1 },
};

D2WIN_STRUCT gatControls[NUM_CONTROLS] =
{
	{ 6, 27, 470, 80, 20, 0, 5305, &sghCelChatLeft, (DWORD)ComCallbackOnSend, 0, 0, 0 },
	{ 6, 107, 470, 80, 20, 0, 5306, &sghCelChatLeft, (DWORD)ComCallbackOnWhisper, 0, 0, 0 },
	{ 6, 187, 470, 80, 20, VK_F1, 5308, &sghCelChatLeft, (DWORD)ComCallbackOnPopup, 0, 0, 0 },
	{ 6, 27, 490, 72, 20, 0, 5307, &sghCelChatLeft1, (DWORD)ComCallbackOnSquelch, 0, 0, 0 },
	{ 6, 99, 490, 96, 20, 0, 5310, &sghCelChatLeft2, (DWORD)ComCallbackOnUnsquelch, 0, 0, 0 },
	{ 6, 195, 490, 72, 20, 0, 5309, &sghCelChatLeft1, (DWORD)ComCallbackOnEmote, 0, 0, 0 },
	{ 6, 533, 469, 120, 20, 0, 5312, &sghCelChatRightTop, (DWORD)GateOnCreateButton, 0, 0, 1 },
	{ 6, 535, 490, 80, 20, 0, 5314, &sghCelChatRight, (DWORD)ChatDlgOnChannels, 0, 0, 1 },
	{ 6, 652, 469, 120, 20, 0, 5313, &sghCelChatRightTop, (DWORD)GateOnJoinButton, 0, 0, 1 },
	{ 6, 614, 490, 80, 20, 0, 5315, &sghCelChatRight, (DWORD)ChatDlgOnLadder, 0, 0, 1 },
	{ 6, 693, 490, 80, 20, 0, 5316, &sghCelChatRight, (DWORD)ChatDlgOnQuit, 0, 0, 0 },
	{ 6, 725, 583, 25, 30, 0, 0, &sghCelChatArrows, (DWORD)ChatDlgOnUsersPageUp, 0, 0, 0 },
	{ 6, 751, 583, 25, 30, 0, 0, &sghCelChatArrows, (DWORD)ChatDlgOnUsersPageDown, 2, 0, 0 },
	{ 6, 725, 528, 25, 30, 0, 0, &sghCelChatArrows, (DWORD)ChatDlgOnUsersLineUp, 4, 0, 0 },
	{ 6, 751, 528, 25, 30, 0, 0, &sghCelChatArrows, (DWORD)ChatDlgOnUsersLineDown, 6, 0, 0 },
	{ 2, 145, 69, 510, 60, 0, 0, NULL, 0, 0, 0, 0 },
	{ 9, 145, 69, 510, 60, 0, 0, NULL, (DWORD)ChatDlgOnAd, 0, 0, 0 },
	{ 12, 40, 591, 720, 75, 0, 0, NULL, 0, 0, 0, 0 },
	{ 4, 28, 420, 354, 308, 0, 0, &sghCelScrollbar,
		0, (DWORD)&sgatBars[0], (DWORD)&sgatFontDesc[0], 4 },
	{ 1, 28, 450, 335, 32, 0, 0, NULL, 0, (DWORD)ChatDlgChatEnter, (DWORD)&sgatFontDesc[0], 22 },
	{ 4, 28, 138, 354, 60, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 2 },
	{ 3, 84, 52, 147, 68, 0, 0, NULL, 0, (DWORD)&sgatAnimStates[0], 0, 0 },
	{ 2, 415, 446, 373, 373, 0, 0, &sghCelBlankBckg2, 0, 0, 0, 0 },
	{ 4, 425, 200, 333, 20, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 96 },
	{ 3, 726, 52, 147, 68, 0, 0, NULL, 0, (DWORD)&sgatAnimStates[0], 0, 0 },
	{ 6, 398, 474, 12, 13, 0, 0, &sghCelGemButton, (DWORD)ChatDlgOnGem, 0, 0, 4 },
	{ 4, 438, 300, 326, 150, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[2], 2 },
	{ 7, 438, 240, 326, 50, 0, 0, NULL, 0, (DWORD)&sgtListEntry10028338, (DWORD)&sgatFontDesc[2], 2 },
	{ 4, 438, 200, 326, 50, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[2], 2 },
	{ 7, 438, 240, 326, 50, 0, 0, NULL, 0, (DWORD)&sgtListEntry10028348, (DWORD)&sgatFontDesc[2], 2 },
	{ 2, 268, 350, 264, 176, 0, 0, &sghCelPopUpOk, 0, 0, 0, 0 },
	{ 6, 351, 337, 96, 32, VK_RETURN, 5102, &sghCelCancel, (DWORD)ChatDlgOnBoxButton, 0, 0, 2 },
	{ 6, 351, 337, 96, 32, VK_RETURN, 5103, &sghCelCancel, (DWORD)ChatDlgOnBoxButton, 0, 0, 2 },
	{ 4, 268, 320, 264, 120, 10, 8, NULL, 0, 0, (DWORD)&sgatFontDesc[4], 2 },
	{ 2, 415, 446, 373, 373, 0, 0, &sghCelJoinBckg, 0, 0, 0, 0 },
	{ 4, 496, 106, 200, 22, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[2], 2 },
	{ 4, 428, 121, 150, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 4, 602, 121, 150, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 4, 431, 212, 200, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 1, 432, 148, 155, 20, 0, 0, NULL, 0, (DWORD)GateOnJoin, (DWORD)&sgatFontDesc[0], 0 },
	{ 1, 606, 148, 155, 20, 0, 0, NULL, 0, (DWORD)GateOnJoin, (DWORD)&sgatFontDesc[0], 1 },
	{ 4, 432, 393, 160, 173, 0, 0, &sghCelJoinScrollbars,
		(DWORD)GateOnListClick, (DWORD)&sgatBars[1], (DWORD)&sgatFontDesc[0], 5 },
	{ 6, 594, 433, 172, 32, VK_RETURN, 5151, &sghCelGameButton, (DWORD)GateOnJoin, 0, 0, 0 },
	{ 6, 471, 185, 15, 16, 0, 0, &sghCelJoinClickBox, (DWORD)GateOnColumn, 0, 0, 2 },
	{ 6, 554, 185, 15, 16, 0, 0, &sghCelJoinClickBox, (DWORD)GateOnColumn, 0, 0, 2 },
	{ 6, 622, 185, 15, 16, 0, 0, &sghCelJoinClickBox, (DWORD)GateOnColumn, 0, 0, 2 },
	{ 6, 713, 185, 15, 16, 0, 0, &sghCelJoinClickBox, (DWORD)GateOnColumn, 0, 0, 2 },
	{ 4, 609, 393, 143, 194, 0, 0, &sghCelJoinScrollbars, 0, (DWORD)&sgatBars[2],
		(DWORD)&sgtFont10028358, 6 },
	{ 2, 415, 446, 373, 373, 0, 0, &sghCelCreateJoinBckg, 0, 0, 0, 0 },
	{ 4, 496, 114, 200, 22, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[2], 2 },
	{ 4, 427, 135, 150, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 4, 427, 187, 150, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 1, 432, 162, 155, 20, 0, 0, NULL, 0, (DWORD)ChatDlgOnChannelOk, (DWORD)&sgatFontDesc[0], 0 },
	{ 1, 606, 162, 158, 20, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 0 },
	{ 4, 432, 397, 321, 200, 0, 0, &sghCelJoinScrollbars,
		(DWORD)ChatDlgOnChannelList, (DWORD)&sgatBars[3], (DWORD)&sgatFontDesc[0], 5 },
	{ 6, 671, 433, 96, 32, VK_RETURN, 5102, &sghCelCancelBlank, (DWORD)ChatDlgOnChannelOk, 0, 0, 0 },
	{ 2, 415, 446, 373, 373, 0, 0, &sghCelCreateGameBckg, 0, 0, 0, 0 },
	{ 4, 496, 111, 200, 22, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[2], 2 },
	{ 4, 427, 134, 150, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 4, 427, 188, 300, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 4, 427, 243, 300, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 4, 632, 306, 300, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 16 },
	{ 4, 632, 342, 300, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 16 },
	{ 4, 705, 342, 150, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 1, 432, 162, 158, 20, 0, 0, NULL, 0, (DWORD)GateOnCreate, (DWORD)&sgatFontDesc[0], 0 },
	{ 1, 432, 217, 158, 20, 0, 0, NULL, 0, (DWORD)GateOnCreate, (DWORD)&sgatFontDesc[0], 1 },
	{ 1, 432, 268, 333, 20, 0, 0, NULL, 0, (DWORD)GateOnCreate, (DWORD)&sgatFontDesc[0], 0 },
	{ 1, 657, 308, 27, 20, 0, 0, NULL, 0, (DWORD)GateOnCreate, (DWORD)&sgatFontDesc[0], 0 },
	{ 1, 657, 342, 27, 20, 0, 0, NULL, 0, (DWORD)GateOnCreate, (DWORD)&sgatFontDesc[0], 0 },
	{ 6, 594, 433, 172, 32, VK_RETURN, 5150, &sghCelGameButton, (DWORD)GateOnCreate, 0, 0, 0 },
	{ 6, 431, 341, 15, 16, 0, 0, &sghCelJoinClickBox, (DWORD)GateOnLevelCheck, 0, 0, 2 },
	{ 2, 647, 346, 33, 26, 0, 0, &sghCelGreyLevelBox, 0, 0, 0, 0 },
	{ 6, 430, 381, 16, 16, 0, 0, &sghCelRadio, (DWORD)GateOnNormal, 0, 0, 2 },
	{ 6, 555, 381, 16, 16, 0, 0, &sghCelRadio, (DWORD)GateOnNightmare, 0, 0, 2 },
	{ 6, 698, 381, 16, 16, 0, 0, &sghCelRadio, (DWORD)GateOnHell, 0, 0, 2 },
	{ 4, 459, 380, 150, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 4, 579, 380, 150, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 4, 723, 380, 150, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 6, 681, 296, 15, 12, 0, 0, &sghCelNumberArrows3, (DWORD)GateOnPlayersUp, 0, 0, 0 },
	{ 6, 681, 310, 15, 12, 0, 0, &sghCelNumberArrows4, (DWORD)GateOnPlayersDown, 0, 0, 0 },
	{ 6, 681, 332, 15, 12, 0, 0, &sghCelNumberArrows1, (DWORD)GateOnLevelUp, 0, 0, 0 },
	{ 6, 681, 346, 15, 12, 0, 0, &sghCelNumberArrows2, (DWORD)GateOnLevelDown, 0, 0, 0 },
	{ 6, 681, 346, 15, 26, 0, 0, &sghCelNumberGreyArrows, 0, 0, 0, 0 },
	{ 2, 647, 311, 33, 26, 0, 0, &sghCelGreyLevelBox, 0, 0, 0, 0 },
	{ 6, 681, 311, 15, 26, 0, 0, &sghCelNumberGreyArrows, 0, 0, 0, 0 },
	{ 2, 415, 446, 373, 373, 0, 0, &sghCelLadderChoice, 0, 0, 0, 0 },
	{ 4, 496, 112, 200, 22, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[2], 2 },
	{ 6, 463, 238, 272, 35, 0, 5318, &sghCelWideButton, (DWORD)ChatDlgOnHardcoreLadder, 0, 0, 0 },
	{ 6, 463, 238, 272, 35, 0, 1656, &sghCelWideButton, (DWORD)ChatDlgOnLadderCancel, 0, 0, 0 },
	{ 6, 463, 188, 272, 35, 0, 5317, &sghCelWideButton, (DWORD)ChatDlgOnStandardLadder, 0, 0, 0 },
	{ 6, 433, 433, 96, 32, 0, 5103, &sghCelCancelBlank, (DWORD)ChatDlgOnPanelCancel, 0, 0, 0 },
	{ 2, 415, 446, 373, 373, 0, 0, &sghCelHardcoreLadder, 0, 0, 0, 0 },
	{ 4, 421, 112, 350, 22, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[2], 2 },
	{ 4, 436, 137, 150, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 6, 540, 137, 15, 16, 0, 0, &sghCelRadio, (DWORD)ChatDlgOnHardcoreLadderOverall, 0, 0, 2 },
	{ 4, 562, 137, 150, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 6, 668, 137, 15, 16, 0, 0, &sghCelRadio, (DWORD)ChatDlgOnHardcoreLadderByClass, 0, 0, 2 },
	{ 4, 691, 137, 150, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 4, 434, 162, 217, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 0 },
	{ 4, 458, 162, 217, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 0 },
	{ 4, 596, 162, 217, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 0 },
	{ 4, 640, 162, 217, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 0 },
	{ 4, 703, 162, 217, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 0 },
	{ 4, 434, 471, 313, 301, 0, 0, &sghCelJoinScrollbars,
		0, (DWORD)&sgatBars[4], (DWORD)&sgatFontDesc[0], 4 },
	{ 6, 433, 433, 96, 32, 0, 5103, &sghCelCancelBlank, (DWORD)ChatDlgOnLadderBack, 0, 0, 0 },
	{ 2, 415, 446, 373, 373, 0, 0, &sghCelHardcoreLadder, 0, 0, 0, 0 },
	{ 4, 421, 112, 350, 22, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[2], 2 },
	{ 4, 436, 137, 150, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 6, 540, 137, 15, 16, 0, 0, &sghCelRadio, (DWORD)ChatDlgOnStandardLadderOverall, 0, 0, 2 },
	{ 4, 562, 137, 150, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 6, 668, 137, 15, 16, 0, 0, &sghCelRadio, (DWORD)ChatDlgOnStandardLadderByClass, 0, 0, 2 },
	{ 4, 691, 137, 150, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 4, 434, 162, 217, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 0 },
	{ 4, 458, 162, 217, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 0 },
	{ 4, 596, 162, 217, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 0 },
	{ 4, 640, 162, 217, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 0 },
	{ 4, 703, 162, 217, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 0 },
	{ 4, 434, 471, 313, 301, 0, 0, &sghCelJoinScrollbars,
		0, (DWORD)&sgatBars[4], (DWORD)&sgatFontDesc[0], 4 },
	{ 6, 433, 433, 96, 32, 0, 5103, &sghCelCancelBlank, (DWORD)ChatDlgOnLadderBack, 0, 0, 0 },
	{ 2, 415, 446, 373, 373, 0, 0, &sghCelEditProfile, 0, 0, 0, 0 },
	{ 4, 432, 147, 155, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 0 },
	{ 1, 625, 155, 55, 20, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 0 },
	{ 1, 723, 155, 40, 20, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 0 },
	{ 1, 432, 207, 259, 20, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 0 },
	{ 1, 432, 387, 327, 150, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 8 },
	{ 2, 415, 446, 373, 373, 0, 0, &sghCelPlayerProfile, 0, 0, 0, 0 },
	{ 4, 432, 147, 155, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 0 },
	{ 4, 625, 147, 55, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 34 },
	{ 4, 723, 147, 40, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 34 },
	{ 4, 432, 199, 259, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 32 },
	{ 4, 432, 387, 327, 150, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 0 },
	{ 4, 425, 129, 217, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 4, 618, 129, 217, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 4, 716, 129, 217, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 4, 425, 180, 217, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 4, 425, 230, 217, 12, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	{ 6, 433, 433, 96, 32, 0, 5103, &sghCelCancelBlank, (DWORD)GateOnPanelCancel, 0, 0, 0 },
	{ 6, 671, 433, 96, 32, 0, 5102, &sghCelCancelBlank, (DWORD)GateOnPanelOk, 0, 0, 0 },
	{ 4, 400, 112, 400, 22, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[2], 2 },
	{ 2, 415, 446, 373, 373, 0, 0, &sghCelHelpBckg, 0, 0, 0, 0 },
	{ 4, 400, 112, 400, 22, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[2], 2 },
	{ 4, 447, 398, 290, 269, 0, 0, &sghCelJoinScrollbars,
		0, (DWORD)&sgatBars[6], (DWORD)&sgatFontDesc[0], 4 },
	{ 6, 671, 433, 96, 32, VK_RETURN, 5102, &sghCelCancelBlank, (DWORD)ChatDlgOnPopupClose, 0, 0, 0 },
};
static int sgnLastChannelLine = -2;
static int sgn1002460c = -1;

#define CHAT_DATA			"DATA\\GLOBAL"

void __fastcall ChatDlgClearPanels(void)
{
	GateCloseCreate();
	GateCloseJoin();
	ChatDlgDestroyPanelControls();
	ChatDlgDestroyPanelChildren();
	GateClosePanel();
	sghLadderList = NULL;
	D2WinDestroyChild(&sghOkBox);
	D2WinDestroyChild(&sgh10028364);
	if (gfPopupOpen == 1)
		GateClosePopup();
	if (gnRequest == REQUEST_GAMEINFO)
		gnRequest = REQUEST_NONE;
}

void __fastcall ChatDlgLoadCels(void)
{
	char szPath[MAX_PATH];

	sprintf(szPath, "%s\\UI\\BIGMENU\\bnet", CHAT_DATA);
	sghCelBnet = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\CURSOR\\ohand", CHAT_DATA);
	sghCelHand = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\CURSOR\\lrotate", CHAT_DATA);
	sghCelRotate = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\scrollbar", CHAT_DATA);
	sghCelScrollbar = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\FRONTEND\\joingamescrollbars", CHAT_DATA);
	sghCelJoinScrollbars = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\chatleftbuttons", CHAT_DATA);
	sghCelChatLeft = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\chatleftbuttons1", CHAT_DATA);
	sghCelChatLeft1 = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\chatleftbuttons2", CHAT_DATA);
	sghCelChatLeft2 = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\chatrightbuttons", CHAT_DATA);
	sghCelChatRight = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\chatrighttopbuttons", CHAT_DATA);
	sghCelChatRightTop = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\chatarrows", CHAT_DATA);
	sghCelChatArrows = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\joingamebckg", CHAT_DATA);
	sghCelJoinBckg = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\GameButtonBlank", CHAT_DATA);
	sghCelGameButton = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\joingameclickbox", CHAT_DATA);
	sghCelJoinClickBox = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\numberarrows", CHAT_DATA);
	sghCelNumberArrows1 = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\numberarrows", CHAT_DATA);
	sghCelNumberArrows2 = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\numberarrows", CHAT_DATA);
	sghCelNumberArrows3 = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\numberarrows", CHAT_DATA);
	sghCelNumberArrows4 = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\numbergreyarrows", CHAT_DATA);
	sghCelNumberGreyArrows = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\GreyLevelBox", CHAT_DATA);
	sghCelGreyLevelBox = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\createjoinbckg", CHAT_DATA);
	sghCelCreateJoinBckg = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\okaybutton", CHAT_DATA);
	sghCelOkayButton = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\creategamebckg", CHAT_DATA);
	sghCelCreateGameBckg = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\creategamebutton", CHAT_DATA);
	sghCelCreateGameButton = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\widebuttonblank", CHAT_DATA);
	sghCelWideButton = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\CancelButtonBlank", CHAT_DATA);
	sghCelCancelBlank = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\BIGMENU\\D2logoBlackBnet", CHAT_DATA);
	sghCelLogoBlack = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\BIGMENU\\D2logoBnet", CHAT_DATA);
	sghCelLogo = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\blankbckg2", CHAT_DATA);
	sghCelBlankBckg2 = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\gembutton", CHAT_DATA);
	sghCelGemButton = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\LadderChoice", CHAT_DATA);
	sghCelLadderChoice = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\HardcoreLadderBckg", CHAT_DATA);
	sghCelHardcoreLadder = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\EditProfileBckg", CHAT_DATA);
	sghCelEditProfile = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\PlayerProfileBckg", CHAT_DATA);
	sghCelPlayerProfile = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\HelpBckg", CHAT_DATA);
	sghCelHelpBckg = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\radiobutton", CHAT_DATA);
	sghCelRadio = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\BIGMENU\\radiobuttonGrey", CHAT_DATA);
	sghCelRadioGrey = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\BIGMENU\\PopUpOK", CHAT_DATA);
	sghCelPopUpOk = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\BIGMENU\\CancelButtonBlank", CHAT_DATA);
	sghCelCancel = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\UncompOverlays\\SquelchAura", CHAT_DATA);
	sghCelSquelchAura = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\UncompOverlays\\SelectAura", CHAT_DATA);
	sghCelSelectAura = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	D2WinD2CompSetMarkerCels(sghCelSquelchAura, sghCelSelectAura);
}

void __fastcall ChatDlgFreeCels(void)
{
	if (sghCelBnet)
	{
		D2WinArchiveFreeCel(sghCelBnet);
		sghCelBnet = NULL;
	}
	if (sghCelHand)
	{
		D2WinArchiveFreeCel(sghCelHand);
		sghCelHand = NULL;
	}
	if (sghCelRotate)
	{
		D2WinArchiveFreeCel(sghCelRotate);
		sghCelRotate = NULL;
	}
	if (sghCelScrollbar)
	{
		D2WinArchiveFreeCel(sghCelScrollbar);
		sghCelScrollbar = NULL;
	}
	if (sghCelJoinScrollbars)
	{
		D2WinArchiveFreeCel(sghCelJoinScrollbars);
		sghCelJoinScrollbars = NULL;
	}
	if (sghCelChatLeft)
	{
		D2WinArchiveFreeCel(sghCelChatLeft);
		sghCelChatLeft = NULL;
	}
	if (sghCelChatLeft1)
	{
		D2WinArchiveFreeCel(sghCelChatLeft1);
		sghCelChatLeft1 = NULL;
	}
	if (sghCelChatLeft2)
	{
		D2WinArchiveFreeCel(sghCelChatLeft2);
		sghCelChatLeft2 = NULL;
	}
	if (sghCelChatRight)
	{
		D2WinArchiveFreeCel(sghCelChatRight);
		sghCelChatRight = NULL;
	}
	if (sghCelChatRightTop)
	{
		D2WinArchiveFreeCel(sghCelChatRightTop);
		sghCelChatRightTop = NULL;
	}
	if (sghCelChatArrows)
	{
		D2WinArchiveFreeCel(sghCelChatArrows);
		sghCelChatArrows = NULL;
	}
	if (sghCelJoinBckg)
	{
		D2WinArchiveFreeCel(sghCelJoinBckg);
		sghCelJoinBckg = NULL;
	}
	if (sghCelGameButton)
	{
		D2WinArchiveFreeCel(sghCelGameButton);
		sghCelGameButton = NULL;
	}
	if (sghCelJoinClickBox)
	{
		D2WinArchiveFreeCel(sghCelJoinClickBox);
		sghCelJoinClickBox = NULL;
	}
	if (sghCelNumberArrows1)
	{
		D2WinArchiveFreeCel(sghCelNumberArrows1);
		sghCelNumberArrows1 = NULL;
	}
	if (sghCelNumberArrows2)
	{
		D2WinArchiveFreeCel(sghCelNumberArrows2);
		sghCelNumberArrows2 = NULL;
	}
	if (sghCelNumberArrows3)
	{
		D2WinArchiveFreeCel(sghCelNumberArrows3);
		sghCelNumberArrows3 = NULL;
	}
	if (sghCelNumberArrows4)
	{
		D2WinArchiveFreeCel(sghCelNumberArrows4);
		sghCelNumberArrows4 = NULL;
	}
	if (sghCelNumberGreyArrows)
	{
		D2WinArchiveFreeCel(sghCelNumberGreyArrows);
		sghCelNumberGreyArrows = NULL;
	}
	if (sghCelGreyLevelBox)
	{
		D2WinArchiveFreeCel(sghCelGreyLevelBox);
		sghCelGreyLevelBox = NULL;
	}
	if (sghCelCreateJoinBckg)
	{
		D2WinArchiveFreeCel(sghCelCreateJoinBckg);
		sghCelCreateJoinBckg = NULL;
	}
	if (sghCelOkayButton)
	{
		D2WinArchiveFreeCel(sghCelOkayButton);
		sghCelOkayButton = NULL;
	}
	if (sghCelCreateGameBckg)
	{
		D2WinArchiveFreeCel(sghCelCreateGameBckg);
		sghCelCreateGameBckg = NULL;
	}
	if (sghCelCreateGameButton)
	{
		D2WinArchiveFreeCel(sghCelCreateGameButton);
		sghCelCreateGameButton = NULL;
	}
	if (ghCelAd)
	{
		D2WinArchiveFreeCel(ghCelAd);
		ghCelAd = NULL;
	}
	if (sghCelWideButton)
	{
		D2WinArchiveFreeCel(sghCelWideButton);
		sghCelWideButton = NULL;
	}
	if (sghCelCancelBlank)
	{
		D2WinArchiveFreeCel(sghCelCancelBlank);
		sghCelCancelBlank = NULL;
	}
	if (sghCel10028308)
	{
		D2WinArchiveFreeCel(sghCel10028308);
		sghCel10028308 = NULL;
	}
	if (sghCelLogoBlack)
	{
		D2WinArchiveFreeCel(sghCelLogoBlack);
		sghCelLogoBlack = NULL;
	}
	if (sghCelLogo)
	{
		D2WinArchiveFreeCel(sghCelLogo);
		sghCelLogo = NULL;
	}
	if (sghCelBlankBckg2)
	{
		D2WinArchiveFreeCel(sghCelBlankBckg2);
		sghCelBlankBckg2 = NULL;
	}
	if (sghCelGemButton)
	{
		D2WinArchiveFreeCel(sghCelGemButton);
		sghCelGemButton = NULL;
	}
	if (sghCelLadderChoice)
	{
		D2WinArchiveFreeCel(sghCelLadderChoice);
		sghCelLadderChoice = NULL;
	}
	if (sghCelHardcoreLadder)
	{
		D2WinArchiveFreeCel(sghCelHardcoreLadder);
		sghCelHardcoreLadder = NULL;
	}
	if (sghCelPlayerProfile)
	{
		D2WinArchiveFreeCel(sghCelPlayerProfile);
		sghCelPlayerProfile = NULL;
	}
	if (sghCelHelpBckg)
	{
		D2WinArchiveFreeCel(sghCelHelpBckg);
		sghCelHelpBckg = NULL;
	}
	if (sghCelEditProfile)
	{
		D2WinArchiveFreeCel(sghCelEditProfile);
		sghCelEditProfile = NULL;
	}
	if (sghCelRadio)
	{
		D2WinArchiveFreeCel(sghCelRadio);
		sghCelRadio = NULL;
	}
	if (sghCelRadioGrey)
	{
		D2WinArchiveFreeCel(sghCelRadioGrey);
		sghCelRadio = NULL;
	}
	if (sghCelPopUpOk)
	{
		D2WinArchiveFreeCel(sghCelPopUpOk);
		sghCelPopUpOk = NULL;
	}
	if (sghCelCancel)
	{
		D2WinArchiveFreeCel(sghCelCancel);
		sghCelCancel = NULL;
	}
	if (sghCelSquelchAura)
	{
		D2WinArchiveFreeCel(sghCelSquelchAura);
		sghCelSquelchAura = NULL;
	}
	if (sghCelSelectAura)
	{
		D2WinArchiveFreeCel(sghCelSelectAura);
		sghCelSelectAura = NULL;
	}
}

void __cdecl operator delete(void *pMemory)
{
	if (pMemory)
		StormSMemFree(pMemory, "delete", -1, 0);
}

void * __cdecl operator new(unsigned int nSize)
{
	return StormSMemAlloc(nSize, "new", -1, 0);
}

BOOL __fastcall ChatDlgFocusChat(void)
{
	return D2WinSetKeyFocus(ghChatEdit);
}

static BOOL __stdcall ChatDlgOnQuit(_PARAMS *ptMsg)
{
	const WORD *pwszText;
	D2WINCONTROL *ptChild;

	pwszText = (const WORD *)D2LangStrGetStringByIndex(5243);
	ChatDlgDestroyPanelChildren();
	ptChild = ChatDlgCreatePanelChild(30);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = ChatDlgCreatePanelChild(32);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = ChatDlgCreatePanelChild(33);
	D2WinChildSetOnTop(ptChild, TRUE);
	D2WinTextBoxAddColorLineUnicode(ptChild, pwszText, 0, 0);
	sgpfnDone = ChatDlgOnBoxDone;
	sgnNextMode = MULTIMODE_FRONTEND;
	D2WinQuitLoop();
	return TRUE;
}

BOOL __stdcall ChatDlgCallStored(DWORD dwArg)
{
	do
		gwNextToken++;
	while (!gwNextToken);
	sgpfnDone(dwArg);
	return TRUE;
}

BOOL __stdcall ChatDlgSetCallback(DWORD dwUnused, PFNCHATDONE lpCallback)
{
#line 1102
	D2ASSERT(lpCallback);
	ChatDlgClearPanels();
	sgpfnDone = lpCallback;
	return TRUE;
}

static BOOL __stdcall ChatDlgOnUsersPageUp(_PARAMS *ptMsg)
{
	ComCallbackScrollUsers(-10);
	return TRUE;
}

static BOOL __stdcall ChatDlgOnUsersPageDown(_PARAMS *ptMsg)
{
	ComCallbackScrollUsers(10);
	return TRUE;
}

static BOOL __stdcall ChatDlgOnUsersLineUp(_PARAMS *ptMsg)
{
	ComCallbackScrollUsers(-1);
	return TRUE;
}

static BOOL __stdcall ChatDlgOnUsersLineDown(_PARAMS *ptMsg)
{
	ComCallbackScrollUsers(1);
	return TRUE;
}

static BOOL __stdcall ChatDlgOnAd(_PARAMS *ptMsg)
{
	HKEY hKey;
	DWORD dwSize;
	DWORD dwType;
	PROCESS_INFORMATION tProcess;
	STARTUPINFOA tStartup;
	struct
	{
		char szCommand[1024];
		char szKey[1024];
		char szCommandKey[1024];
		char szClass[2048];
	} t;
	int i;

	t.szClass[0] = gchBlank;
	for (i = 1; i < (int)sizeof(t.szClass); i++)
		t.szClass[i] = 0;
	SetForegroundWindow(GetDesktopWindow());
	sprintf(t.szKey, "%s\\.html", "SOFTWARE\\Classes");
	dwSize = 0;
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, t.szKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		dwSize = 1024;
		if (RegQueryValueExA(hKey, NULL, NULL, &dwType, (BYTE *)t.szClass, &dwSize) != ERROR_SUCCESS)
			RegCloseKey(hKey);
		else if (RegCloseKey(hKey) == ERROR_SUCCESS)
		{
			sprintf(t.szCommandKey, "%s\\%s\\shell\\open\\command", "SOFTWARE\\Classes", t.szClass);
			t.szCommand[0] = gchBlank;
			for (i = 1; i < (int)sizeof(t.szCommand); i++)
				t.szCommand[i] = 0;
			dwSize = 0;
			if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, t.szCommandKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
			{
				dwSize = 1024;
				if (RegQueryValueExA(hKey, NULL, NULL, &dwType, (BYTE *)t.szCommand, &dwSize)
					!= ERROR_SUCCESS)
					RegCloseKey(hKey);
				else if (RegCloseKey(hKey) == ERROR_SUCCESS)
				{
					strcat(t.szCommand, " ");
					strcat(t.szCommand, gszAdText);
					StormSMemZero(&tStartup, sizeof(tStartup));
					tStartup.cb = sizeof(tStartup);
					if (CreateProcessA(NULL, t.szCommand, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL,
						NULL, &tStartup, &tProcess))
					{
						CloseHandle(tProcess.hThread);
						CloseHandle(tProcess.hProcess);
					}
				}
			}
		}
	}
	gptComInt->pfnSend16(gdwAdId, FALSE);
	return TRUE;
}

static BOOL __stdcall ChatDlgOnChannels(_PARAMS *ptMsg)
{
	char szName[32];
	const char *pszChannel;
	const WORD *pwszName;
	BOOL fEnabled;

	D2WinButtonSetPressed(gahLobby[8], FALSE);
	D2WinButtonSetPressed(gahLobby[9], FALSE);
	D2WinButtonSetPressed(gahLobby[6], FALSE);
	ChatDlgClearPanels();
	sghSubBckg = (D2WINCONTROL *)D2WinCreateChild(&gatControls[48]);
	sghSubTitle = (D2WINCONTROL *)D2WinCreateChild(&gatControls[49]);
	D2WinTextBoxAddLineUnicode((DWORD)sghSubTitle, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5261));
	sghSubEditLabel = (D2WINCONTROL *)D2WinCreateChild(&gatControls[50]);
	D2WinTextBoxAddLineUnicode((DWORD)sghSubEditLabel, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5262));
	sghSubListLabel = (D2WINCONTROL *)D2WinCreateChild(&gatControls[51]);
	D2WinTextBoxAddLineUnicode((DWORD)sghSubListLabel, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5263));
	sghSubOk = (D2WINCONTROL *)D2WinCreateChild(&gatControls[55]);
	sghSubCancel = (D2WINCONTROL *)D2WinCreateChild(&gatControls[90]);
	D2WinSetEscapeButton(sghSubCancel);
	sghSubEdit = (D2WINCONTROL *)D2WinCreateChild(&gatControls[52]);
	D2WinEditBoxSetMaxLength(sghSubEdit, 32);
	D2WinEditBoxSetKeyProc(sghSubEdit, ChatDlgChannelKey);
	D2WinEditBoxSetKeyPostProc(sghSubEdit, ChatDlgChannelPost);
	sghSubList = (D2WINCONTROL *)D2WinCreateChild(&gatControls[54]);
	pszChannel = (const char *)gptComInt->pfnRequest0B();
	D2WinTextBoxAddLineUTF(sghSubList, gptParams->abFirstChannel);
	while (*pszChannel)
	{
		D2WinTextBoxAddLineUTF(sghSubList, pszChannel);
		while (*pszChannel)
			pszChannel++;
		pszChannel++;
	}
	D2WinEditBoxSetStringUTF(sghSubEdit, ComCallbackChannelName());
	D2WinSetKeyFocus(sghSubEdit);
	D2WinTextBoxSetTop(sghSubList, 0);
	pwszName = D2WinEditBoxGetString(sghSubEdit);
	fEnabled = FALSE;
	if (pwszName)
	{
		Unicode::unicode2Win(szName, pwszName, 31);
		if (strlen(szName) >= 1)
			fEnabled = TRUE;
	}
	D2WinChildSetEnabled(sghSubOk, fEnabled);
	return TRUE;
}

static BOOL __stdcall ChatDlgOnLadder(_PARAMS *ptMsg)
{
	D2WINCONTROL *ptChild;

	D2WinButtonSetPressed(gahLobby[7], FALSE);
	D2WinButtonSetPressed(gahLobby[8], FALSE);
	D2WinButtonSetPressed(gahLobby[6], FALSE);
	ChatDlgClearPanels();
	ChatDlgCreatePanelChild(85);
	ptChild = ChatDlgCreatePanelChild(86);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5317));
	ChatDlgCreatePanelChild(87);
	ChatDlgCreatePanelChild(89);
	ptChild = ChatDlgCreatePanelChild(90);
	D2WinSetEscapeButton(ptChild);
	return TRUE;
}

static BOOL __stdcall ChatDlgOnChannelOk(_PARAMS *ptMsg)
{
	char szChannel[256];
	const WORD *pwszChannel;

	if (!D2WinChildGetEnabled(sghSubOk))
		return FALSE;
	pwszChannel = D2WinEditBoxGetString(sghSubEdit);
	if (pwszChannel)
	{
		Unicode::unicode2Win(szChannel, pwszChannel, 256);
		gptComInt->pfnSend0C(szChannel);
		ChatDlgClosePanels();
	}
	return TRUE;
}

static BOOL __stdcall ChatDlgOnChannelList(_PARAMS *ptMsg)
{
	char szChannel[256];
	const WORD *pwszText;
	int nLine;
	DWORD dwTick;

	nLine = D2WinTextBoxGetSelectedNum(sghSubList);
	if (nLine == -1)
		return FALSE;
	dwTick = GetTickCount();
	if (nLine == sgnLastChannelLine && dwTick < sgdwLastClick + DOUBLECLICK_TICKS)
	{
		pwszText = D2WinEditBoxGetString(sghSubEdit);
		if (!pwszText)
			return FALSE;
		Unicode::unicode2Win(szChannel, pwszText, 256);
		gptComInt->pfnSend0C(szChannel);
		ChatDlgClosePanels();
		return FALSE;
	}
	pwszText = D2WinTextBoxGetSelectedLine(sghSubList);
	if (pwszText)
	{
		D2WinEditBoxSetStringUnicode(sghSubEdit, pwszText);
		D2WinSetKeyFocus(0);
	}
	sgnLastChannelLine = nLine;
	sgdwLastClick = dwTick;
	return FALSE;
}

static BOOL __stdcall ChatDlgChatKey(D2WINEDITBOX *ptEdit, int nPos, char *pchKey)
{
	struct
	{
		char szText[256];
		char szAfter[256];
		char szBefore[256];
	} t;
	const WORD *pwszText;
	int nLength;

	if (*pchKey == 14)
	{
		pwszText = D2WinEditBoxGetString(ghChatEdit);
		if (pwszText && gptSelectedUnit)
		{
			Unicode awszText[256];

			Unicode::unicode2Win(t.szText, pwszText, 256);
			nLength = strlen(t.szText);
			strncpy(t.szBefore, t.szText, nPos);
			t.szBefore[nPos] = 0;
			strncpy(t.szAfter, &t.szText[nPos], nLength - nPos);
			t.szAfter[nLength - nPos] = 0;
			sprintf(t.szText, "%s%s%s", t.szBefore, gptSelectedUnit->szName, t.szAfter);
			Unicode::win2Unicode(awszText, t.szText, 256);
			D2WinEditBoxSetStringUnicode(ghChatEdit, awszText);
		}
		return FALSE;
	}
	if (*pchKey == '\t')
	{
		if (sgnHistory)
		{
			if (++sgnHistoryIndex >= sgnHistory)
				sgnHistoryIndex = 0;
			D2WinEditBoxSetStringUTF(ghChatEdit, sgaszHistory[sgnHistoryIndex]);
		}
		return FALSE;
	}
	return TRUE;
}

static BOOL __stdcall ChatDlgChannelKey(D2WINEDITBOX *ptEdit, int nPos, char *pchKey)
{
	D2WinTextBoxSetSelectedNum(sghSubList, -1);
	return TRUE;
}

BOOL __fastcall ChatDlgOkBox(const WORD *pwszText, PFNCHATDONE pfnDone)
{
	ChatDlgClearPanels();
	D2WinButtonSetPressed(gahLobby[7], FALSE);
	D2WinButtonSetPressed(gahLobby[6], FALSE);
	D2WinButtonSetPressed(gahLobby[8], FALSE);
	D2WinButtonSetPressed(gahLobby[9], FALSE);
	sghOkBox = (D2WINCONTROL *)D2WinCreateChild(&gatControls[26]);
	D2WinTextBoxAddLineUnicode((DWORD)sghOkBox, (DWORD)pwszText);
	sgpfnDone = pfnDone;
	return TRUE;
}

void __fastcall ChatDlgClosePanels(void)
{
	ChatDlgClearPanels();
	D2WinButtonSetPressed(gahLobby[7], FALSE);
	D2WinButtonSetPressed(gahLobby[6], FALSE);
	D2WinButtonSetPressed(gahLobby[8], FALSE);
	D2WinButtonSetPressed(gahLobby[9], FALSE);
	gnRequest = REQUEST_NONE;
}

static inline void __fastcall ChatDlgServerDown(void)
{
	if (!D2MCPClientD2MCPIsConnected())
		ChatDlgMessageBox((const WORD *)D2LangStrGetStringByIndex(5351), ChatDlgOnLeave);
	else
	{
		gnRequest = REQUEST_NONE;
		ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5139), ChatDlgOnOkDone);
	}
}

BOOL __stdcall ChatDlgIdle(int nPass)
{

	RECT tRect;
	BOOL fPolled;
	BOOL fTimer;
	D2MCPGAMEINFO *ptInfo;
	DWORD dwStatus;
	DWORD dwElapsed;
	DWORD dwToken;

	Unicode awszUnused[128];
	SetRect(&tRect, 145, 11, 655, 71);
	if (D2WinMouseInRect(&tRect))
		D2WinSetCursor(sghCelRotate);
	else
		D2WinSetCursor(sghCelHand);
	fPolled = TRUE;
	while (fPolled)
	{
		fPolled = ComCallbackPump();
		fPolled |= D2MCPClientD2MCPPoll();
		switch (gnRequest)
		{
		case REQUEST_CREATE:
			if (!D2MCPClientD2MCPIsConnected())
				break;
			dwToken = D2MCPClientD2MCPGetReplyToken();
			if (!dwToken || dwToken != gwRequestToken)
				break;
			switch (D2MCPClientD2MCPGetStatus())
			{
			case 0:
				gnRequest = REQUEST_JOIN;
				do
					gwNextToken++;
				while (!gwNextToken);
				D2MCPClientToMCPSendJoinGame(gptParams->szGameName, &gptParams->bGamePassword,
					gwNextToken);
				gwRequestToken = gwNextToken;
				break;
			case 30:
				gnRequest = REQUEST_NONE;
				ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5137), ChatDlgOnOkDone);
				break;
			case 31:
				gnRequest = REQUEST_NONE;
				ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5138), ChatDlgOnOkDone);
				break;
			case 32:
				ChatDlgServerDown();
				break;
			default:
				gnRequest = REQUEST_NONE;
				ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5140), ChatDlgOnOkDone);
				break;
			}
			break;
		case REQUEST_JOIN:
			if (!D2MCPClientD2MCPIsConnected())
				break;
			dwToken = D2MCPClientD2MCPGetReplyToken();
			if (!dwToken || dwToken != gwRequestToken)
				break;
			{
				Unicode awszUnused2[512];

				switch (D2MCPClientD2MCPGetStatus())
				{
				case 0:
					gptParams->dwGameType = 3;
					gptParams->wJoinId = (WORD)D2MCPClientD2MCPGetGameId();
					*(DWORD *)gptParams->abGameToken = D2MCPClientD2MCPGetGameToken();
					strcpy(gptParams->szServerIp, D2MCPClientD2MCPGetGameAddress());
					sgnNextMode = MULTIMODE_GAME;
					D2WinSetCursor(0);
					D2WinQuitLoop();
					return TRUE;
				case 42:
					gnRequest = REQUEST_NONE;
					ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5159), ChatDlgOnOkDone);
					break;
				case 41:
					gnRequest = REQUEST_NONE;
					ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5160), ChatDlgOnOkDone);
					break;
				case 43:
					gnRequest = REQUEST_NONE;
					ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5161), ChatDlgOnOkDone);
					break;
				case 44:
					gnRequest = REQUEST_NONE;
					ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5162), ChatDlgOnOkDone);
					break;
				case 113:
					gnRequest = REQUEST_NONE;
					ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5158), ChatDlgOnOkDone);
					break;
				default:
					gnRequest = REQUEST_NONE;
					ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5141), ChatDlgOnOkDone);
					break;
				}
			}
			break;
		case REQUEST_GAMEINFO:
			if (!D2MCPClientD2MCPIsConnected())
				break;
			ptInfo = (D2MCPGAMEINFO *)D2MCPClientD2MCPGetGameInfo();
			if (!ptInfo->wToken)
				break;
			if (ptInfo->wToken == gwRequestToken)
			{
				dwStatus = D2MCPClientD2MCPGetStatus();
				if (dwStatus == 51)
					GateShowList();
				else if (dwStatus == 50)
				{
					GateCloseJoin();
					ChatDlgServerDown();
				}
				else if (ptInfo->fTrailingData)
					GateShowList();
				else
					GateAddGame(ptInfo);
			}
			else if (ptInfo->wToken == gwGameInfoToken)
				GateShowInfo(ptInfo);
			D2MCPClientD2MCPClearGameInfo();
			break;
		}
		fTimer = FALSE;
		dwElapsed = GetTickCount() - sgdwKeepAliveTick;
		if (dwElapsed >= KEEPALIVE_TICKS)
		{
			fTimer = TRUE;
			sgdwKeepAliveTick += dwElapsed;
			gptComInt->pfnSend15();
			ChatDlgKeepAlive();
		}
		if (gnRequest == REQUEST_GAMEINFO)
		{
			dwElapsed = GetTickCount() - sgdwGameInfoTick;
			if (dwElapsed >= KEEPALIVE_TICKS)
			{
				fTimer = TRUE;
				sgdwGameInfoTick += dwElapsed;
				GateRefreshList();
			}
		}
		dwElapsed = GetTickCount() - sgdwListTick;
		if (dwElapsed >= 1000)
		{
			sgdwListTick += dwElapsed;
			GateRequestInfo();
		}
		else if (!fTimer)
			return FALSE;
	}
	return TRUE;
}

static BOOL __stdcall ChatDlgOnOkDone(DWORD dwArg)
{
	return TRUE;
}

static void __fastcall ChatDlgDestroyPanelControls(void)
{
	D2WinDestroyChild(&sghSubBckg);
	D2WinDestroyChild(&sghSubTitle);
	D2WinDestroyChild(&sghSubEditLabel);
	D2WinDestroyChild(&sghSubListLabel);
	D2WinDestroyChild(&sghSubOk);
	D2WinDestroyChild(&sghSubCancel);
	D2WinDestroyChild(&sghSubEdit);
	D2WinDestroyChild(&sghSubList);
	D2WinSetEscapeButton(gahLobby[10]);
	D2WinSetKeyFocus(ghChatEdit);
}

static inline BOOL __fastcall ChatDlgIsDeadHardcore(void)
{
	return (gptParams->wCharFlags & 8) && (gptParams->wCharFlags & 4);
}

static void __fastcall ChatDlgKeepAlive(void)
{
	if (ChatDlgIsDeadHardcore())
		return;
	if (!D2MCPClientD2MCPIsConnected() && ChatDlgRealmLost())
	{
		if (!D2MCPClientD2MCPIsConnected())
		{
			ChatDlgMessageBox((const WORD *)D2LangStrGetStringByIndex(5351), ChatDlgOnLeave);
			return;
		}
		gnRequest = REQUEST_NONE;
		ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5139), ChatDlgOnOkDone);
		return;
	}
	if (gnRequest == REQUEST_NONE && sghOkBox)
		ChatDlgClearPanels();
	if (!ChatDlgIsDeadHardcore())
	{
		D2WinChildSetEnabled(gahLobby[6], TRUE);
		D2WinChildSetEnabled(gahLobby[8], TRUE);
	}
	D2WinChildSetEnabled(gahLobby[9], sgfOpenBattleNet == FALSE);
}

BOOL __stdcall ChatDlgWaitDone(DWORD dwArg)
{
	if (WaitForSingleObject(sgh100271e0, 0) == WAIT_OBJECT_0)
		D2WinQuitLoop();
	return TRUE;
}

int __fastcall ChatDlgRealmLost(void)
{
	if (gptParams->szRealm[0] && !D2MCPClientD2MCPIsConnected())
		return -1;
	return 0;
}

BOOL __fastcall ChatDlgMain(INIT_PARAMS *ptParams)
{
	char szChat[128];
	char szNames[512];
	D2WINCONTROL *ptPicture;
	D2WINCONTROL *ptFrame;
	D2WINCONTROL *ptBanner;
	D2CHATNAME *ptName;
	D2CHATNAME *ptNext;
	DWORD dwStart;
	DWORD dwElapsed;
	DWORD hArchive;
	int nLength;
	int i;

	dwStart = GetTickCount();
	FogErrorManagerLog("[D2MULTI]  Start entering at %d\n", dwStart);
	hArchive = D2WinArchiveGetArchive();
	if (!ChatStrNewTable(hArchive))
		return TRUE;
	sgnNextMode = 0;
	gnRequest = REQUEST_NONE;
	gptComInt = (BNCLIENTINTERFACE *)ptParams->dwComInt;
#line 2246
	D2ASSERT(gptComInt);
	if (gptComInt->pfnIsConnected() && (!ptParams->szRealm[0] || !ChatDlgRealmLost()) && gptComInt)
	{
		ChatDlgLoadCels();
		D2WinSetCursor(sghCelHand);
		sghChatBox = NULL;
		D2WinSetBackground(sghCelBnet);
		for (i = 0; i < NUM_LOBBY_CONTROLS; i++)
			gahLobby[i] = (D2WINCONTROL *)D2WinCreateChild(&gatControls[i]);
		sghTitleBox = (D2WINCONTROL *)D2WinCreateChild(&gatControls[20]);
		sghChatBox = (D2WINCONTROL *)D2WinCreateChild(&gatControls[18]);
		D2WinTextBoxSetLineSpacing(sghChatBox, -6, -1);
		ghChatEdit = (D2WINCONTROL *)D2WinCreateChild(&gatControls[19]);
		D2WinEditBoxSetKeyProc(ghChatEdit, ChatDlgChatKey);
		D2WinEditBoxSetKeyPostProc(ghChatEdit, ChatDlgChatPost);
		D2WinSetKeyFocus(ghChatEdit);
		D2WinEditBoxSetMaxLength(ghChatEdit, 256);
		ComCallbackUpdateButtons();
		D2WinChildSetOnTop(gahLobby[11], TRUE);
		D2WinChildSetOnTop(gahLobby[12], TRUE);
		D2WinChildSetOnTop(gahLobby[13], TRUE);
		D2WinChildSetOnTop(gahLobby[14], TRUE);
		for (i = 0; i < NUM_LOBBY_BUTTONS; i++)
			gahLobbyButtons[i] = (D2WINCONTROL *)D2WinButtonCreate(40 + i * 60, 591, 60, 100, 0,
				ComCallbackOnUserButton, 0, 0, 4, 0, 0);
		ghUserList = (D2WINCONTROL *)D2WinCreateChild(&gatControls[17]);
		D2WinChildSetVisible(ghUserList, FALSE);
		for (i = 0; i < NUM_HISTORY; i++)
			sgaszHistory[i][0] = 0;
		sgnHistoryIndex = -1;
		sgnHistory = 0;
		ChatDlgClosePanels();
		ptPicture = (D2WINCONTROL *)D2WinCreateChild(&gatControls[21]);
		ptFrame = (D2WINCONTROL *)D2WinCreateChild(&gatControls[22]);
		sghNamesBox = (D2WINCONTROL *)D2WinCreateChild(&gatControls[23]);
		D2MCPClientD2MCPSetNewsCallback(ChatDlgOnNews);
		((BOOL (__cdecl *)(void))D2MCPClientToMCPSendNewsRequestRaw)();
		{
			Unicode awszNames[512];

			if (sghNamesBox)
			{
				szNames[0] = 0;
				nLength = 0;
				for (ptName = sgptNames; ptName && nLength < 512; ptName = ptName->ptNext)
				{
					strncat(szNames, ptName->szText, 512 - nLength);
					nLength = strlen(szNames);
				}
				Unicode::win2Unicode(awszNames, szNames, 512);
				D2WinTextBoxAddColorLineUnicode(sghNamesBox, awszNames, 1, 0);
			}
		}
		ptBanner = (D2WINCONTROL *)D2WinCreateChild(&gatControls[24]);
		sghGem = (D2WINCONTROL *)D2WinCreateChild(&gatControls[25]);
		if (sgdwGemActive)
			D2WinButtonSetFirstCel(sghGem, 2);
		if ((gptParams->wCharFlags & 8) && (gptParams->wCharFlags & 4))
		{
			D2WinChildSetEnabled(gahLobby[6], FALSE);
			D2WinChildSetEnabled(gahLobby[8], FALSE);
		}
		D2WinSetEscapeButton(gahLobby[10]);
		sgch100271f4 = 0;
		sgch100272f8 = 0;
		D2SoundFastInit();
		D2SoundFastLoad("data\\global\\sfx\\cursor\\button.wav");
		if (D2SoundGetOptionsMusic())
			D2SoundSmpMusicSetState(1);
		sgfOpenBattleNet = gptParams->bConnectMode == 4;
		D2WinChildSetEnabled(gahLobby[9], sgfOpenBattleNet == FALSE);
		sprintf(szChat, "%s,%s", ptParams->szRealm, ptParams->szName);
		gptComInt->pfnEnterChat(ptParams->szName, szChat, 37, (const char *)ptParams->abChannel);
		dwElapsed = GetTickCount() - dwStart;
		FogErrorManagerLog("[D2MULTI]  Finish entering at %d.  Elapsed: %d\n", GetTickCount(), dwElapsed);
		if (gptComInt->pfnIsConnected())
		{
			D2WinLoop(ChatDlgIdle);
			gptComInt->pfnLeaveChannel();
		}
		else
			sgnNextMode = MULTIMODE_FRONTEND;
		dwStart = GetTickCount();
		FogErrorManagerLog("[D2MULTI]  Start closing at %d\n", dwStart);
		D2SoundFastShutdown();
		ComCallbackFreeUsers();
		D2WinD2CompFreeAll();
		ChatDlgClearPanels();
		for (ptName = sgptNames; ptName; ptName = ptNext)
		{
			ptNext = ptName->ptNext;
#line 5146
			D2FREE2(ptName);
		}
		sgptNames = NULL;
		D2WinDestroyChild(&sghChatBox);
		D2WinDestroyChild(&sghTitleBox);
		D2WinDestroyChild(&ptPicture);
		D2WinDestroyChild(&ptBanner);
		D2WinDestroyChild(&ptFrame);
		D2WinDestroyChild(&sghNamesBox);
		D2WinDestroyChild(&sghGem);
		D2WinDestroyChild(&ghChatEdit);
		for (i = 0; i < NUM_LOBBY_BUTTONS; i++)
			D2WinDestroyChild(&gahLobbyButtons[i]);
		D2WinDestroyChild(&ghUserList);
		for (i = 0; i < NUM_LOBBY_CONTROLS; i++)
			D2WinDestroyChild(&gahLobby[i]);
		D2WinSetBackground(0);
		D2WinSetCursor(0);
		ChatDlgFreeCels();
	}
	else
		sgnNextMode = MULTIMODE_FRONTEND;
	D2LangStrDeleteTable();
	FogErrorManagerLog("[D2MULTI]  Finish closing at %d.  Elapsed: %d\n", GetTickCount(),
		GetTickCount() - dwStart);
	return TRUE;
}

static BOOL __stdcall ChatDlgChatPost(DWORD dwZero)
{
	ComCallbackUpdateButtons();
	return TRUE;
}

BOOL __fastcall ChatDlgCreateGame(D2MCPGAMECREATE *ptGame, const char *pszServerIp)
{
	ChatDlgSetCallback(0, ChatDlgOnOkDone);
	strcpy(gptParams->szGameName, ptGame->szGameName);
	strcpy((char *)&gptParams->bGamePassword, ptGame->szPassword);
	strcpy((char *)&gptParams->field_21b[23], ptGame->szDescription);
	gnRequest = REQUEST_CREATE;
	if (gptParams->szRealm[0])
	{
		if (D2MCPClientD2MCPIsConnected())
		{
			gptParams->dwGameType = 2;
			do
				gwNextToken++;
			while (!gwNextToken);
			D2MCPClientToMCPSendCreateGame(ptGame, gwNextToken);
			gwRequestToken = gwNextToken;
			return TRUE;
		}
		gnRequest = REQUEST_NONE;
		ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5139), ChatDlgOnOkDone);
		return FALSE;
	}
	gptParams->dwGameType = 6;
	strcpy(gptParams->szServerIp, pszServerIp);
	sgnNextMode = MULTIMODE_GAME;
	D2WinSetCursor(0);
	D2WinQuitLoop();
	return TRUE;
}

BOOL __fastcall ChatDlgJoinGame(WORD wGameId, const char *pszName, const char *pszPassword,
	const char *pszServerIp)
{
	gnRequest = REQUEST_JOIN;
	ChatDlgSetCallback(0, ChatDlgOnOkDone);
	gptParams->wJoinId = wGameId;
	strcpy(gptParams->szGameName, pszName);
	strcpy((char *)&gptParams->bGamePassword, pszPassword);
	if (gptParams->szRealm[0])
	{
		if (D2MCPClientD2MCPIsConnected())
		{
			gptParams->dwGameType = 3;
			do
				gwNextToken++;
			while (!gwNextToken);
			D2MCPClientToMCPSendJoinGame(pszName, pszPassword, gwNextToken);
			gwRequestToken = gwNextToken;
			return TRUE;
		}
		gnRequest = REQUEST_NONE;
		ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5139), ChatDlgOnOkDone);
		return FALSE;
	}
	gptParams->dwGameType = 7;
	strcpy(gptParams->szServerIp, pszServerIp);
	sgnNextMode = MULTIMODE_GAME;
	D2WinSetCursor(0);
	D2WinQuitLoop();
	return TRUE;
}

int __fastcall ChatDlgRun(INIT_PARAMS *ptParams)
{
	gptParams = ptParams;
	D2WinCreateParent();
	D2WinResizeWindow(1);
	ChatDlgMain(ptParams);
	D2WinCloseParent();
	return sgnNextMode;
}

BOOL __fastcall ChatDlgAddLine(const WORD *pwszText, DWORD dwColor)
{
	if (sghChatBox)
		D2WinTextBoxAddColorLineUnicode(sghChatBox, pwszText, dwColor, 0);
	return TRUE;
}

BOOL __fastcall ChatDlgAddLineTwin(const WORD *pwszText, DWORD dwColor)
{
	if (sghChatBox)
		D2WinTextBoxAddColorLineUnicode(sghChatBox, pwszText, dwColor, 0);
	return TRUE;
}

BOOL __fastcall ChatDlgSetTitle(const WORD *pwszText)
{
	D2WinClearTextBox(sghTitleBox);
	D2WinTextBoxAddColorLineUnicode(sghTitleBox, pwszText, 3, 0);
	return TRUE;
}

static BOOL __stdcall ChatDlgChannelPost(DWORD dwZero)
{
	char szName[32];
	const WORD *pwszName;
	BOOL fEnabled;

	pwszName = D2WinEditBoxGetString(sghSubEdit);
	fEnabled = FALSE;
	if (pwszName)
	{
		Unicode::unicode2Win(szName, pwszName, 31);
		if (strlen(szName) >= 1)
			fEnabled = TRUE;
	}
	D2WinChildSetEnabled(sghSubOk, fEnabled);
	return TRUE;
}

BOOL __stdcall ChatDlgOnLeave(DWORD dwArg)
{
	sgnNextMode = MULTIMODE_FRONTEND;
	D2WinQuitLoop();
	return TRUE;
}

BOOL __stdcall ChatDlgChatEnter(const WORD *pwszText)
{
	char szText[256];

	sgnHistoryIndex = -1;
	if (!Unicode::strlen(pwszText))
		return FALSE;
#line 3562
	D2ASSERT(gptComInt);
	Unicode::unicode2Win(szText, pwszText, 256);
	{
		char szCopy[256];

		strcpy(szCopy, szText);
		char szDelim[] = " ";
		if (!strtok(szCopy, szDelim))
		{
			D2WinEditBoxSetStringUnicode(ghChatEdit, 0);
			return TRUE;
		}
	}
	return ChatDlgSendText(szText, FALSE);
}

static int __fastcall ChatDlgCommandCompare(const char *pszText, const char *pszCommand)
{
	unsigned int nCount;
	int nDiff;

	if (strlen(pszText) < strlen(pszCommand))
		return -1;
	nCount = strlen(pszCommand);
	while (nCount--)
	{
		nDiff = toupper(*pszText) - toupper(*pszCommand);
		if (nDiff)
		{
			if (nDiff > 0)
				return 1;
			return -1;
		}
		pszText++;
		pszCommand++;
	}
	return 0;
}

static void __fastcall ChatDlgAddHistory(const char *pszText)
{
	memmove(&sgaszHistory[1], &sgaszHistory[0], sizeof(sgaszHistory) - sizeof(sgaszHistory[0]));
	strcpy(sgaszHistory[0], pszText);
}

static BOOL __fastcall ChatDlgHelpCommand(char *pszText, const char *pszCommand)
{
	char szCopy[256];
	const char *pszWord;
	unsigned int nLength;
	int nWords;
	int nHistory;

	nLength = strlen(pszCommand);
	if (ChatDlgCommandCompare(pszText, pszCommand) || (pszText[nLength] != ' ' && pszText[nLength]))
		return FALSE;
	ChatDlgAddHistory(pszCommand);
	nHistory = sgnHistory + 1;
	sgnHistory = nHistory;
	if (nHistory >= NUM_HISTORY)
		sgnHistory = NUM_HISTORY;
	strcpy(szCopy, pszText);
	char szDelim[] = " ";
	nWords = 0;
	for (pszWord = strtok(pszText, szDelim); pszWord && nWords < 2; pszWord = strtok(NULL, szDelim))
		nWords++;
	if (nWords == 1)
	{
		char szHelp[512];
		Unicode awszHelp[512];

		strcpy(szHelp, "Valid Diablo II commands are: /d2notify");
		Unicode::toUnicode(awszHelp, szHelp, 512);
		ChatDlgAddLine((const WORD *)awszHelp, 1);
	}
	if (!gptComInt->pfnIsConnected())
		ChatDlgMessageBox((const WORD *)D2LangStrGetStringByIndex(5351), ChatDlgOnLeave);
	else
		gptComInt->pfnSend0E(szCopy);
	return TRUE;
}

static BOOL __fastcall ChatDlgReplyCommand(char *pszText, const char *pszCommand)
{
	char szPrefix[256];
	char szCopy[256];
	char *pszWord;
	char *pszMessage;
	unsigned int nLength;
	int nWords;
	int nHistory;

	nLength = strlen(pszCommand);
	if (ChatDlgCommandCompare(pszText, pszCommand) || (pszText[nLength] != ' ' && pszText[nLength]))
		return FALSE;
	if (!strlen(gszReplyName))
	{
		char szLine[256];
		Unicode awszLine[256];

		sprintf(szLine, "Nobody to reply to!");
		Unicode::toUnicode(awszLine, szLine, 256);
		ChatDlgAddLine((const WORD *)awszLine, 1);
		return TRUE;
	}
	sprintf(szPrefix, "%s ", pszCommand);
	ChatDlgAddHistory(szPrefix);
	nHistory = sgnHistory + 1;
	sgnHistory = nHistory;
	if (nHistory >= NUM_HISTORY)
		sgnHistory = NUM_HISTORY;
	strcpy(szCopy, pszText);
	char szDelim[] = " ";
	pszWord = strtok(szCopy, szDelim);
	nWords = 0;
	pszMessage = NULL;
	while (pszWord)
	{
		if (++nWords == 2)
		{
			pszMessage = pszWord;
			break;
		}
		pszWord = strtok(NULL, szDelim);
	}
	if (nWords != 2)
	{
		char szLine[256];
		Unicode awszLine[256];

		sprintf(szLine, "Correct usage is: %s (message)", pszCommand);
		Unicode::toUnicode(awszLine, szLine, 256);
		ChatDlgAddLine((const WORD *)awszLine, 1);
		return TRUE;
	}
	sprintf(pszText, "/w %s %s", gszReplyName, pszMessage);
	if (!gptComInt->pfnIsConnected())
		ChatDlgMessageBox((const WORD *)D2LangStrGetStringByIndex(5351), ChatDlgOnLeave);
	else
		gptComInt->pfnSend0E(pszText);
	return TRUE;
}

static BOOL __fastcall ChatDlgNotifyCommand(char *pszText, const char *pszCommand)
{
	unsigned int nLength;
	int nHistory;

	nLength = strlen(pszCommand);
	if (ChatDlgCommandCompare(pszText, pszCommand) || (pszText[nLength] != ' ' && pszText[nLength]))
		return FALSE;
	if (!ChatDlgCommandCompare(pszText, pszCommand))
	{
		char szNotice[256];

		ChatDlgAddHistory(pszCommand);
		nHistory = sgnHistory + 1;
		sgnHistory = nHistory;
		if (nHistory >= NUM_HISTORY)
			sgnHistory = NUM_HISTORY;
		gfNotify = !gfNotify;
		Unicode awszNotice[256];
		if (gfNotify)
			strcpy(szNotice, "Player entering/leaving notifications are enabled.");
		else
			strcpy(szNotice, "Player entering/leaving notifications are disabled.");
		Unicode::toUnicode(awszNotice, szNotice, 512);
		ChatDlgAddLine((const WORD *)awszNotice, 3);
	}
	return TRUE;
}

static void __fastcall ChatDlgSendCommand(char *pszText)
{
	int nHistory;

	if (!ChatDlgReplyCommand(pszText, "/r") && !ChatDlgReplyCommand(pszText, "/reply")
		&& !ChatDlgHelpCommand(pszText, "/help") && !ChatDlgHelpCommand(pszText, "/?")
		&& !ChatDlgNotifyCommand(pszText, "/d2notify"))
	{
		ChatDlgAddHistory(pszText);
		nHistory = sgnHistory + 1;
		sgnHistory = nHistory;
		if (nHistory >= NUM_HISTORY)
			sgnHistory = NUM_HISTORY;
		if (!gptComInt->pfnIsConnected())
			ChatDlgMessageBox((const WORD *)D2LangStrGetStringByIndex(5351), ChatDlgOnLeave);
		else
			gptComInt->pfnSend0E(pszText);
	}
}

BOOL __fastcall ChatDlgSendText(char *pszText, BOOL fWhisper)
{
	BOOL fCommand;

	fCommand = *pszText == '/';
	if (fWhisper && !fCommand && gptSelectedUnit)
	{
		char szWhisper[512];

		sprintf(szWhisper, "/whisper %s %s", gptSelectedUnit->szName, pszText);
		strncpy(pszText, szWhisper, 256);
		fCommand = TRUE;
	}
	if (fCommand)
		ChatDlgSendCommand(pszText);
	else
	{
		if (!gptComInt->pfnIsConnected())
		{
			ChatDlgMessageBox((const WORD *)D2LangStrGetStringByIndex(5351), ChatDlgOnLeave);
			return FALSE;
		}
		gptComInt->pfnSend0E(pszText);
		ComCallbackEchoLine(pszText);
	}
	D2WinEditBoxSetStringUnicode(ghChatEdit, 0);
	return TRUE;
}

static D2WINCONTROL * __fastcall ChatDlgCreatePanelChild(int nControl)
{
	D2WINCONTROL *ptChild;

#line 3805
	D2ASSERT(sgnNumSubChildren < MAX_CHILDREN);
	ptChild = (D2WINCONTROL *)D2WinCreateChild(&gatControls[nControl]);
	sgaptSubChildren[sgnNumSubChildren] = ptChild;
	sgnNumSubChildren++;
	return ptChild;
}

void __fastcall ChatDlgDestroyPanelChildren(void)
{
	int i;

	for (i = 0; i < sgnNumSubChildren; i++)
		D2WinDestroyChild(&sgaptSubChildren[i]);
	sgnNumSubChildren = 0;
	D2WinSetEscapeButton(gahLobby[10]);
}

void __fastcall ChatDlgMessageBox(const WORD *pwszText, PFNCHATDONE pfnDone)
{
	D2WINCONTROL *ptChild;

	ChatDlgDestroyPanelChildren();
	ptChild = ChatDlgCreatePanelChild(30);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = ChatDlgCreatePanelChild(31);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = ChatDlgCreatePanelChild(33);
	D2WinChildSetOnTop(ptChild, TRUE);
	D2WinTextBoxAddColorLineUnicode(ptChild, pwszText, 0, 0);
	if (pfnDone)
		sgpfnDone = pfnDone;
	else
		sgpfnDone = ChatDlgOnBoxDone;
}

void __fastcall ChatDlgMessageBoxCancel(const WORD *pwszText, PFNCHATDONE pfnDone)
{
	D2WINCONTROL *ptChild;

	ChatDlgDestroyPanelChildren();
	ptChild = ChatDlgCreatePanelChild(30);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = ChatDlgCreatePanelChild(32);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = ChatDlgCreatePanelChild(33);
	D2WinChildSetOnTop(ptChild, TRUE);
	D2WinTextBoxAddColorLineUnicode(ptChild, pwszText, 0, 0);
	sgpfnDone = pfnDone;
	if (!pfnDone)
		sgpfnDone = ChatDlgOnBoxDone;
}

static BOOL __stdcall ChatDlgOnBoxDone(DWORD dwArg)
{
	ChatDlgDestroyPanelChildren();
	return TRUE;
}

static BOOL __stdcall ChatDlgOnBoxButton(_PARAMS *ptMsg)
{
	if (sgpfnDone)
		sgpfnDone((DWORD)ptMsg);
	return TRUE;
}

static BOOL __stdcall ChatDlgOnPanelCancel(_PARAMS *ptMsg)
{
	WORD wFlags;

	D2WinButtonSetPressed(gahLobby[7], FALSE);
	D2WinButtonSetPressed(gahLobby[6], FALSE);
	D2WinButtonSetPressed(gahLobby[8], FALSE);
	D2WinButtonSetPressed(gahLobby[9], FALSE);
	wFlags = gptParams->wCharFlags;
	if (!(wFlags & 8) || !(wFlags & 4))
	{
		D2WinChildSetEnabled(gahLobby[6], TRUE);
		D2WinChildSetEnabled(gahLobby[8], TRUE);
	}
	D2WinChildSetEnabled(gahLobby[7], TRUE);
	D2WinChildSetEnabled(gahLobby[9], sgfOpenBattleNet == FALSE);
	ChatDlgClearPanels();
	return TRUE;
}

static BOOL __stdcall ChatDlgOnLadderBack(_PARAMS *ptMsg)
{
	D2WINCONTROL *ptChild;

	D2WinButtonSetPressed(gahLobby[7], FALSE);
	D2WinButtonSetPressed(gahLobby[6], FALSE);
	D2WinButtonSetPressed(gahLobby[8], FALSE);
	ChatDlgClearPanels();
	ChatDlgCreatePanelChild(85);
	ptChild = ChatDlgCreatePanelChild(86);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5317));
	ChatDlgCreatePanelChild(87);
	ChatDlgCreatePanelChild(89);
	ptChild = ChatDlgCreatePanelChild(90);
	D2WinSetEscapeButton(ptChild);
	return TRUE;
}

static BOOL __stdcall ChatDlgOnHardcoreLadder(_PARAMS *ptMsg)
{
	ChatDlgClearPanels();
	ChatDlgCreateHardcoreLadder();
	if (sghLadderClassRadio)
		D2WinButtonSetPressed(sghLadderClassRadio, FALSE);
	if (sghLadderAllRadio)
		D2WinButtonSetPressed(sghLadderAllRadio, TRUE);
	return TRUE;
}

static BOOL __stdcall ChatDlgOnStandardLadder(_PARAMS *ptMsg)
{
	ChatDlgClearPanels();
	ChatDlgCreateStandardLadder();
	if (sghLadderClassRadio)
		D2WinButtonSetPressed(sghLadderClassRadio, FALSE);
	if (sghLadderAllRadio)
		D2WinButtonSetPressed(sghLadderAllRadio, TRUE);
	return TRUE;
}

static BOOL __stdcall ChatDlgOnLadderCancel(_PARAMS *ptMsg)
{
	ChatDlgClearPanels();
	return TRUE;
}

static void __fastcall ChatDlgCreateHardcoreLadder(void)
{
	D2WINCONTROL *ptChild;

	sghLadderAllRadio = NULL;
	sghLadderClassRadio = NULL;
	sghLadderList = NULL;
	if (gptParams->szRealm[0] && !D2MCPClientD2MCPIsConnected())
	{
		ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5139), NULL);
		return;
	}
	ChatDlgCreatePanelChild(91);
	ptChild = ChatDlgCreatePanelChild(92);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5318));
	ptChild = ChatDlgCreatePanelChild(93);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5325));
	sghLadderAllRadio = ChatDlgCreatePanelChild(94);
	ptChild = ChatDlgCreatePanelChild(95);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5326));
	sghLadderClassRadio = ChatDlgCreatePanelChild(96);
	ptChild = ChatDlgCreatePanelChild(97);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5327));
	ptChild = ChatDlgCreatePanelChild(99);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5321));
	ptChild = ChatDlgCreatePanelChild(100);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5322));
	ptChild = ChatDlgCreatePanelChild(102);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5324));
	ptChild = ChatDlgCreatePanelChild(101);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(4057));
	sghLadderList = ChatDlgCreatePanelChild(103);
	ptChild = ChatDlgCreatePanelChild(104);
	D2WinSetEscapeButton(ptChild);
	D2MCPClientToMCPSendLadder(0);
	D2MCPClientD2MCPSetLadderCallback(ChatDlgLadderRows);
}

static void __fastcall ChatDlgLadderRows(int nRows, int nNameSize, BYTE *pbRows)
{
	D2CHATLADDERROW *ptRow;
	const char *pszTitle;
	int nLine;
	int nClass;
	int nNumber;
	int nColor;
	int i;

	if (!sghLadderList)
		return;
	D2WinTextBoxSetNumColumns(sghLadderList, 5);
	D2WinTextBoxSetLineSpacing(sghLadderList, 0, -6);
	D2WinTextBoxSetColumnX(sghLadderList, 0, 0);
	D2WinTextBoxSetColumnAlign(sghLadderList, 0, 16);
	D2WinTextBoxSetColumnX(sghLadderList, 1, 25);
	D2WinTextBoxSetColumnX(sghLadderList, 2, 165);
	D2WinTextBoxSetColumnX(sghLadderList, 3, 195);
	D2WinTextBoxSetColumnAlign(sghLadderList, 3, 16);
	D2WinTextBoxSetColumnX(sghLadderList, 4, 220);
	D2WinTextBoxSetColumnAlign(sghLadderList, 4, 16);
	ptRow = (D2CHATLADDERROW *)pbRows;
	for (i = 0; i < nRows; i++)
	{
		const char *apszClass[7] = { "ama", "sor", "nec", "pal", "bar", "dru", "ass" };
		char szNumber[128];
		char szName[128];

		nNumber = ptRow->dwPacked;
		nClass = ptRow->dwPacked & 15;
		nNumber = (nNumber >> 16) & 255;
		nColor = 4;
		if (ptRow->dwPacked & 16)
			nColor = 5;
		itoa(i + 1, szNumber, 10);
		D2WinTextBoxAddColorLineUTF(sghLadderList, szNumber, nColor);
		nLine = D2WinTextBoxGetLastLineNum(sghLadderList);
		szName[0] = 0;
		pszTitle = D2WinD2CompGetTitleOf(nClass, (ptRow->wClassTitle >> 8) & 31,
			ptRow->dwPacked & 32);
		if (pszTitle)
			strcat(szName, pszTitle);
		strcat(szName, ptRow->szName);
		D2WinTextBoxAddColumnTextUTF(sghLadderList, szName, nLine, 1);
		D2WinTextBoxAddColumnTextUTF(sghLadderList, apszClass[nClass], nLine, 2);
		itoa(nNumber, szNumber, 10);
		D2WinTextBoxAddColumnTextUTF(sghLadderList, szNumber, nLine, 3);
		Unicode awszNumber[128];

		D2LangStrFormatUInt64SuffixRaw((DWORD)awszNumber, ptRow->dwExperienceLo, ptRow->dwExperienceHi,
			127);
		D2WinTextBoxAddColumnTextUnicode(sghLadderList, 	awszNumber, nLine, 4);
		ptRow = (D2CHATLADDERROW *)((BYTE *)ptRow + nNameSize + 12);
	}
	D2WinTextBoxSetTop(sghLadderList, 0);
}

static void __fastcall ChatDlgCreateHardcoreClassLadder(void)
{
	D2WINCONTROL *ptChild;

	if (gptParams->szRealm[0] && !D2MCPClientD2MCPIsConnected())
	{
		ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5139), NULL);
		return;
	}
	ChatDlgCreatePanelChild(91);
	ptChild = ChatDlgCreatePanelChild(92);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5318));
	ptChild = ChatDlgCreatePanelChild(93);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5325));
	sghLadderAllRadio = ChatDlgCreatePanelChild(94);
	ptChild = ChatDlgCreatePanelChild(95);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5326));
	sghLadderClassRadio = ChatDlgCreatePanelChild(96);
	ptChild = ChatDlgCreatePanelChild(97);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5327));
	ptChild = ChatDlgCreatePanelChild(99);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5321));
	ptChild = ChatDlgCreatePanelChild(100);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5322));
	ptChild = ChatDlgCreatePanelChild(102);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5324));
	ptChild = ChatDlgCreatePanelChild(101);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(4057));
	sghLadderList = ChatDlgCreatePanelChild(103);
	ptChild = ChatDlgCreatePanelChild(104);
	D2WinSetEscapeButton(ptChild);
	D2MCPClientToMCPSendLadder(1);
	D2MCPClientToMCPSendLadder(2);
	D2MCPClientToMCPSendLadder(3);
	D2MCPClientToMCPSendLadder(4);
	D2MCPClientToMCPSendLadder(5);
	D2MCPClientD2MCPSetLadderCallback(ChatDlgLadderClassRows);
}

static BOOL __stdcall ChatDlgOnHardcoreLadderByClass(_PARAMS *ptMsg)
{
	ChatDlgClearPanels();
	sghLadderAllRadio = NULL;
	sghLadderClassRadio = NULL;
	sghLadderList = NULL;
	ChatDlgCreateHardcoreClassLadder();
	if (sghLadderAllRadio)
		D2WinButtonSetPressed(sghLadderAllRadio, FALSE);
	if (sghLadderClassRadio)
		D2WinButtonSetPressed(sghLadderClassRadio, TRUE);
	return TRUE;
}

static void __fastcall ChatDlgLadderClassRows(int nRows, int nNameSize, BYTE *pbRows)
{
	D2CHATLADDERROW *ptRow;
	const char *pszTitle;
	int nLine;
	int nClass;
	int nNumber;
	int nColor;
	int i;

	if (!sghLadderList)
		return;
	D2WinTextBoxSetNumColumns(sghLadderList, 5);
	D2WinTextBoxSetLineSpacing(sghLadderList, 0, -6);
	D2WinTextBoxSetColumnX(sghLadderList, 0, 0);
	D2WinTextBoxSetColumnAlign(sghLadderList, 0, 16);
	D2WinTextBoxSetColumnX(sghLadderList, 1, 25);
	D2WinTextBoxSetColumnX(sghLadderList, 2, 165);
	D2WinTextBoxSetColumnX(sghLadderList, 3, 195);
	D2WinTextBoxSetColumnAlign(sghLadderList, 3, 16);
	D2WinTextBoxSetColumnX(sghLadderList, 4, 220);
	D2WinTextBoxSetColumnAlign(sghLadderList, 4, 16);
	if (D2WinTextBoxGetNumLines(sghLadderList))
	{
		for (i = 2; i; i--)
		{
			D2WinTextBoxAddColorLineUTF(sghLadderList, " ", 4);
			nLine = D2WinTextBoxGetLastLineNum(sghLadderList);
			D2WinTextBoxAddColumnTextUTF(sghLadderList, " ", nLine, 1);
			D2WinTextBoxAddColumnTextUTF(sghLadderList, " ", nLine, 2);
			D2WinTextBoxAddColumnTextUTF(sghLadderList, " ", nLine, 3);
			D2WinTextBoxAddColumnTextUTF(sghLadderList, " ", nLine, 4);
		}
	}
	ptRow = (D2CHATLADDERROW *)pbRows;
	for (i = 0; i < nRows; i++)
	{
		const char *apszClass[7] = { "ama", "sor", "nec", "pal", "bar", "dru", "ass" };
		char szNumber[128];
		char szName[128];

		nNumber = ptRow->dwPacked;
		nClass = ptRow->dwPacked & 15;
		nNumber = (nNumber >> 16) & 255;
		nColor = 4;
		if (ptRow->dwPacked & 16)
			nColor = 5;
		itoa(i + 1, szNumber, 10);
		D2WinTextBoxAddColorLineUTF(sghLadderList, szNumber, nColor);
		nLine = D2WinTextBoxGetLastLineNum(sghLadderList);
		szName[0] = 0;
		pszTitle = D2WinD2CompGetTitleOf(nClass, (ptRow->wClassTitle >> 8) & 31,
			ptRow->dwPacked & 32);
		if (pszTitle)
			strcat(szName, pszTitle);
		strcat(szName, ptRow->szName);
		D2WinTextBoxAddColumnTextUTF(sghLadderList, szName, nLine, 1);
		D2WinTextBoxAddColumnTextUTF(sghLadderList, apszClass[nClass], nLine, 2);
		itoa(nNumber, szNumber, 10);
		D2WinTextBoxAddColumnTextUTF(sghLadderList, szNumber, nLine, 3);
		Unicode awszNumber[128];

		D2LangStrFormatUInt64SuffixRaw((DWORD)awszNumber, ptRow->dwExperienceLo, ptRow->dwExperienceHi,
			127);
		D2WinTextBoxAddColumnTextUnicode(sghLadderList, 	awszNumber, nLine, 4);
		ptRow = (D2CHATLADDERROW *)((BYTE *)ptRow + nNameSize + 12);
	}
	D2WinTextBoxSetTop(sghLadderList, 0);
}

static BOOL __stdcall ChatDlgOnHardcoreLadderOverall(_PARAMS *ptMsg)
{
	ChatDlgClearPanels();
	ChatDlgCreateHardcoreLadder();
	if (sghLadderClassRadio)
		D2WinButtonSetPressed(sghLadderClassRadio, FALSE);
	if (sghLadderAllRadio)
		D2WinButtonSetPressed(sghLadderAllRadio, TRUE);
	return TRUE;
}

static void __fastcall ChatDlgCreateStandardLadder(void)
{
	D2WINCONTROL *ptChild;

	sghLadderAllRadio = NULL;
	sghLadderClassRadio = NULL;
	sghLadderList = NULL;
	if (gptParams->szRealm[0] && !D2MCPClientD2MCPIsConnected())
	{
		ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5139), NULL);
		return;
	}
	ChatDlgCreatePanelChild(105);
	ptChild = ChatDlgCreatePanelChild(106);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5317));
	ptChild = ChatDlgCreatePanelChild(107);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5325));
	sghLadderAllRadio = ChatDlgCreatePanelChild(108);
	ptChild = ChatDlgCreatePanelChild(109);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5326));
	sghLadderClassRadio = ChatDlgCreatePanelChild(110);
	ptChild = ChatDlgCreatePanelChild(111);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5327));
	ptChild = ChatDlgCreatePanelChild(113);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5321));
	ptChild = ChatDlgCreatePanelChild(114);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5322));
	ptChild = ChatDlgCreatePanelChild(116);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5324));
	ptChild = ChatDlgCreatePanelChild(115);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(4057));
	sghLadderList = ChatDlgCreatePanelChild(117);
	ptChild = ChatDlgCreatePanelChild(118);
	D2WinSetEscapeButton(ptChild);
	D2MCPClientToMCPSendLadder(9);
	D2MCPClientD2MCPSetLadderCallback(ChatDlgLadderRows);
}

static void __fastcall ChatDlgCreateStandardClassLadder(void)
{
	D2WINCONTROL *ptChild;

	if (gptParams->szRealm[0] && !D2MCPClientD2MCPIsConnected())
	{
		ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5139), NULL);
		return;
	}
	ChatDlgCreatePanelChild(105);
	ptChild = ChatDlgCreatePanelChild(106);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5317));
	ptChild = ChatDlgCreatePanelChild(107);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5325));
	sghLadderAllRadio = ChatDlgCreatePanelChild(108);
	ptChild = ChatDlgCreatePanelChild(109);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5326));
	sghLadderClassRadio = ChatDlgCreatePanelChild(110);
	ptChild = ChatDlgCreatePanelChild(111);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5327));
	ptChild = ChatDlgCreatePanelChild(113);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5321));
	ptChild = ChatDlgCreatePanelChild(114);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5322));
	ptChild = ChatDlgCreatePanelChild(116);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(5324));
	ptChild = ChatDlgCreatePanelChild(115);
	D2WinTextBoxAddLineUnicode((DWORD)ptChild, (DWORD)(const WORD *)D2LangStrGetStringByIndex(4057));
	sghLadderList = ChatDlgCreatePanelChild(117);
	ptChild = ChatDlgCreatePanelChild(118);
	D2WinSetEscapeButton(ptChild);
	D2MCPClientToMCPSendLadder(10);
	D2MCPClientToMCPSendLadder(11);
	D2MCPClientToMCPSendLadder(12);
	D2MCPClientToMCPSendLadder(13);
	D2MCPClientToMCPSendLadder(14);
	D2MCPClientD2MCPSetLadderCallback(ChatDlgLadderClassRows);
}

static BOOL __stdcall ChatDlgOnStandardLadderByClass(_PARAMS *ptMsg)
{
	ChatDlgClearPanels();
	sghLadderAllRadio = NULL;
	sghLadderClassRadio = NULL;
	sghLadderList = NULL;
	ChatDlgCreateStandardClassLadder();
	if (sghLadderAllRadio)
		D2WinButtonSetPressed(sghLadderAllRadio, FALSE);
	if (sghLadderClassRadio)
		D2WinButtonSetPressed(sghLadderClassRadio, TRUE);
	return TRUE;
}

static BOOL __stdcall ChatDlgOnStandardLadderOverall(_PARAMS *ptMsg)
{
	ChatDlgClearPanels();
	ChatDlgCreateStandardLadder();
	if (sghLadderClassRadio)
		D2WinButtonSetPressed(sghLadderClassRadio, FALSE);
	if (sghLadderAllRadio)
		D2WinButtonSetPressed(sghLadderAllRadio, TRUE);
	return TRUE;
}

void __fastcall ChatDlgGreyRadio(D2WINCONTROL *ptControl)
{
	D2WinChildSetCelFile(ptControl, sghCelRadioGrey);
	D2WinChildSetEnabled(ptControl, FALSE);
}

BOOL __stdcall ChatDlgProfileIdle(int nPass)
{
	DWORD adwSizes[NUM_PROFILE_KEYS];

	adwSizes[0] = 8;
	adwSizes[1] = 4;
	adwSizes[2] = 40;
	adwSizes[3] = 200;
	if (gptComInt->pfnReadUserData(NUM_PROFILE_KEYS, 1, sgapszProfileValues, adwSizes) || sgfProfileCancel)
		D2WinQuitLoop();
	return TRUE;
}

BOOL __fastcall ChatDlgGetProfile(const char *pszAccount, char *pszSex, char *pszAge, char *pszLocation,
	char *pszDescription)
{
	sgapszProfileValues[1] = pszAge;
	sgapszProfileValues[0] = pszSex;
	sgapszProfileValues[2] = pszLocation;
	sgapszProfileValues[3] = pszDescription;
	gptComInt->pfnSend26(pszAccount, NUM_PROFILE_KEYS, 1, sgapszProfileKeys);
	sgfProfileCancel = FALSE;
	ChatDlgMessageBoxCancel((const WORD *)D2LangStrGetStringByIndex(5243), ChatDlgOnProfileCancel);
	D2WinLoop(ChatDlgProfileIdle);
	ChatDlgDestroyPanelChildren();
	return sgfProfileCancel == FALSE;
}

static BOOL __stdcall ChatDlgOnProfileCancel(DWORD dwArg)
{
	sgfProfileCancel = TRUE;
	return TRUE;
}

int __fastcall ChatDlgSetProfile(const char *pszAccount, const char *pszSex, const char *pszAge,
	const char *pszLocation, const char *pszDescription)
{
	const char *apszValues[NUM_PROFILE_KEYS];

	apszValues[0] = pszSex;
	apszValues[1] = pszAge;
	apszValues[2] = pszLocation;
	apszValues[3] = pszDescription;
	return gptComInt->pfnSend27(pszAccount, NUM_PROFILE_KEYS, sgapszProfileKeys, apszValues);
}

static BOOL __stdcall ChatDlgOnGem(_PARAMS *ptMsg)
{

	if (sgdwGemActive)
	{
		D2WinButtonSetFirstCel(sghGem, 0);
		sgdwGemActive = 0;
		ChatDlgAddLine((const WORD *)D2LangStrGetStringByIndex(5335), 11);
	}
	else
	{
		D2WinButtonSetFirstCel(sghGem, 2);
		sgdwGemActive = 1;
		if (!(rand() % 1000))
			ChatDlgAddLine((const WORD *)D2LangStrGetStringByIndex(5336), 11);
		else if (!(rand() % 1000000))
		{
			Unicode awszMoo[20];
			Unicode::toUnicode(awszMoo, "Moooooooo!", 12);
			ChatDlgAddLine((const WORD *)awszMoo, 9);
		}
		else
			ChatDlgAddLine((const WORD *)D2LangStrGetStringByIndex(5334), 11);
	}
	return TRUE;
}

static BOOL __stdcall ChatDlgOnPopupClose(_PARAMS *ptMsg)
{
	GateClosePopup();
	return TRUE;
}

static void __fastcall ChatDlgOnNews(DWORD dwKind, DWORD dwLength, const char *pszText)
{
	char szNames[512];
	D2CHATNAME *ptName, *ptNew;
	D2CHATNAME *ptPrev;
	D2CHATNAME *ptNext;
	int nLength;

	for (ptName = sgptNames; ptName; ptName = ptName->ptNext)
		if (ptName->dwKind == dwKind)
			break;
	if (!ptName)
	{
#line 5074
		ptNew = (D2CHATNAME *)D2ALLOC2(sizeof(D2CHATNAME));
		ptNew->dwKind = dwKind;  ptNext = sgptNames;
		if (!ptNext || (int)ptNext->dwKind > (int)dwKind)
		{
			ptNew->ptNext = ptNext;
			sgptNames = ptNew;
		}
		else
		{
			ptPrev = ptNext;
			while (1)
			{
				if ((int)ptNext->dwKind >= (int)dwKind) break;
				ptPrev = ptNext;
				ptNext = ptNext->ptNext;
				if (!ptNext) break;
			}
			ptNew->ptNext = ptNext;
			ptPrev->ptNext = ptNew;
		}
		ptName = ptNew;
	}
	strncpy(ptName->szText, pszText, sizeof(ptName->szText));
	ptName->szText[sizeof(ptName->szText)] = 0;
	Unicode awszNames[512];
	if (sghNamesBox)
	{
		nLength = 0;
		szNames[0] = 0;
		for (ptName = sgptNames; ptName && nLength < 512; ptName = ptName->ptNext)
		{
			strncat(szNames, ptName->szText, 512 - nLength);
			nLength = strlen(szNames);
		}
		Unicode::win2Unicode(awszNames, szNames, 512);
		D2WinTextBoxAddColorLineUnicode(sghNamesBox, (const WORD *)awszNames, 1, 0);
	}
}
