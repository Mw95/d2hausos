#define D2_THIS_FILE "C:\\D2\\Source\\D2Launch\\Src\\MainMenus.cpp"
#include "D2Launch.h"
#include "D2Macros.h"

static BOOL __stdcall MainMenusNameFilter(D2WINEDITBOX *ptEdit, int nPos, char *pchKey);
static BOOL __stdcall MainMenusOnNameChange(DWORD dwZero);
static void __fastcall MainMenusNameCheck(void);
static BOOL __fastcall MainMenusSaveExists(void);
static BOOL __fastcall MainMenusSetBoxText(D2WINCONTROL *ptBox, const WORD *pwszText, DWORD dwColor,
	BYTE bBroken);
static void __fastcall MainMenusTcpIpShowAddress(void);

typedef D2WINCONTROL *D2LAUNCHCHILDREN[MAX_CHILDREN];
typedef HCELFILE D2LAUNCHCELS5[5];
typedef HCELFILE D2LAUNCHCELS3[3];
typedef D2WINCONTROL *D2LAUNCHHEROES[5];
typedef char D2LAUNCHREALM[128];

static D2WINCONTROL * sgptTcpIpAddress = 0;
static D2WINCONTROL * sgptShardList = 0;
static D2LAUNCHHEROES sgaptHero = { 0 };
static D2WINCONTROL * sgptNewCharOk = 0;
static D2WINCONTROL * PLACEHOLDER_UnreferencedData_1002dac8 = 0;
static D2WINCONTROL * sgptShardText = 0;
static D2WINCONTROL * sgptTcpIpTimer = 0;
static D2WINCONTROL * sgptNameLabel = 0;
static BOOL sgfPatchCancelled = 0;
static D2LAUNCHCHILDREN sgaptChildren = { 0 };
static D2WINCONTROL * PLACEHOLDER_UnreferencedData_1002dbdc = 0;
static D2COMINTREALMS * sgptRealmList = 0;
static D2WINCONTROL * sgptHardcoreBox = 0;
static D2WINCONTROL * PLACEHOLDER_UnreferencedData_1002dbe8 = 0;
static D2WINCONTROL * sgptLoginOk = 0;
PFNLAUNCHPENDING gpfnPending = { 0 };
static D2LAUNCHCHILDREN sgaptSubChildren = { 0 };
static BOOL sgfConnected = 0;
static D2WINCONTROL * sgptRealmEdit = 0;
static D2WINCONTROL * sgptHardcoreLabel = 0;
static D2WINCONTROL * sgptNameBox = 0;
static D2WINCONTROL * sgptTcpIpHost = 0;
static D2WINCONTROL * sgptLoginPassword = 0;
static D2WINCONTROL * sgptNameEdit = 0;
static D2WINCONTROL * sgptLoginPasswordAgain = 0;
static D2WINCONTROL * sgptLoginAccount = 0;
static D2WINCONTROL * sgptTcpIpLabel = 0;
static D2WINCONTROL * sgptTcpIpJoin = 0;
static D2LAUNCHREALM sgszRealm = { 0 };
INIT_PARAMS * gptParams = 0;
static D2WINCONTROL * sgptClassName = 0;
static D2WINCONTROL * sgptClassText = 0;
static D2WINCONTROL * sgptTcpIpHostHelp = 0;
static D2WINCONTROL * sgptTcpIpJoinHelp = 0;
static int sgnResult = 0;
int geMode = 0;
BOOL gfConnectBoxCancelled = 0;
static HCELFILE sghCelBlizno = { 0 };
static HCELFILE sghCelTrademark = { 0 };
static HCELFILE sghCel1002ddc8 = { 0 };
static HCELFILE sghCelDiablo2 = { 0 };
static HCELFILE sghCelLogoBlackLeft = { 0 };
static HCELFILE sghCelLogoFireLeft = { 0 };
static HCELFILE sghCelLogoBlackRight = { 0 };
static HCELFILE sghCelLogoFireRight = { 0 };
static HCELFILE sghCelTitleScreen = { 0 };
static HCELFILE sghCelCursor = { 0 };
static HCELFILE sghCelCreditsBckg = { 0 };
static HCELFILE sghCel1002ddec = { 0 };
static HCELFILE sghCel1002ddf0 = { 0 };
static HCELFILE sghCelWideButton = { 0 };
static HCELFILE sghCelMediumButton = { 0 };
static HCELFILE sghCelShortButton = { 0 };
static HCELFILE sghCel1002de00 = { 0 };
static HCELFILE sghCelTextBox2 = { 0 };
static HCELFILE PLACEHOLDER_UnreferencedData_1002de08 = 0;
static HCELFILE sghCelTermsOfUseBckg = { 0 };
HCELFILE ghCelCharSelectBckg = { 0 };
static HCELFILE sghCelDifficultyLevels = { 0 };
static HCELFILE sghCelCinematicsSelection = { 0 };
static HCELFILE sghCelCharSelectBox = { 0 };
static HCELFILE sghCelCharSelectBoxGrey = { 0 };
static HCELFILE sghCelPopUpOkCancel = { 0 };
static HCELFILE sghCelPopUpOkCancel2 = { 0 };
static HCELFILE sghCelSelectCharArrows = { 0 };
static HCELFILE sghCelCreateNewChar = { 0 };
static HCELFILE sghCelPopUpOk = { 0 };
static HCELFILE sghCelPopUp340x224 = { 0 };
static HCELFILE sghCelPopUpWide = { 0 };
static HCELFILE sghCelPopUpLargest = { 0 };
static HCELFILE sghCelPopUpOk2 = { 0 };
static HCELFILE sghCelOkCancelButton = { 0 };
static HCELFILE sghCelCancelButton = { 0 };
static HCELFILE sghCelPassword = { 0 };
static HCELFILE sghCel1002de54 = { 0 };
static HCELFILE sghCel1002de58 = { 0 };
static HCELFILE sghCelRealmBckg = { 0 };
static HCELFILE sghCelJoinGameScrollbars = { 0 };
static HCELFILE sghCelTcpIpBckg = { 0 };
static HCELFILE sghCelIpAddressBox = { 0 };
static HCELFILE sghCelDownloadPatchBckg = { 0 };
static HCELFILE sghCelDownloadPatchBall = { 0 };
static D2LAUNCHCELS5 sgahCel1002de74 = { 0 };
static D2LAUNCHCELS5 sgahCel1002de88 = { 0 };
static D2LAUNCHCELS5 sgahCel1002de9c = { 0 };
static D2LAUNCHCELS5 sgahCel1002deb0 = { 0 };
static D2LAUNCHCELS5 sgahCel1002dec4 = { 0 };
static HCELFILE PLACEHOLDER_UnreferencedData_1002ded8[7] = { 0 };
static D2LAUNCHCELS3 sgahCel1002def4 = { 0 };
static HCELFILE PLACEHOLDER_UnreferencedData_1002df00[2] = { 0 };
static D2LAUNCHCELS3 sgahCel1002df08 = { 0 };
static HCELFILE PLACEHOLDER_UnreferencedData_1002df14[2] = { 0 };
static HCELFILE sghCel1002df1c = { 0 };
static RECT PLACEHOLDER_UnreferencedData_1002df20 = { 0 };
static HCELFILE sghCel1002df30 = { 0 };
static HCELFILE PLACEHOLDER_UnreferencedData_1002df34[2] = { 0 };
static BOOL sgfDiscCancel = 0;
static BOOL sgfDiscOk = 0;
static DWORD sgpfnDiscOpen = 0;
static DWORD PLACEHOLDER_UnreferencedData_1002df48[24] = { 0 };
static int sgnNumChildren = 0;
static int sgnNumSubChildren = 0;
static DWORD sgdw1002dfb0 = 0;
static DWORD sgdw1002dfb4 = 0;
BOOL gf1002dfb8 = 0;
static D2WINCONTROL * sgptJoinEdit = 0;
static D2WINCONTROL * sgptCreditsLeft = 0;
static D2WINCONTROL * sgptCreditsRight = 0;
static D2WINCONTROL * sgptCreditsCentre = 0;
static D2WINCONTROL * sgptCreditsTimer = 0;
static int sgnCreditsEnd = 0;
static HANDLE sghMcpConnectThread = 0;
static BOOL sgfPaletteLoaded = 0;
static BOOL sgfWaitCancelled = 0;
static D2WINCONTROL * sgptPatchText = 0;
static D2WINCONTROL * sgptPatchBall = 0;
static D2LAUNCHSHARD ** sgptShardArray = 0;
static int sgnShardListSize = 0;
static BOOL sgfJoined = 0;
extern char gchEmpty;

static BOOL __stdcall MainMenusNewAccountFilter(D2WINEDITBOX *ptEdit, int nPos,
	char *pchKey);
static BOOL __stdcall MainMenusNewPasswordFilter(D2WINEDITBOX *ptEdit, int nPos,
	char *pchKey);
static BOOL __stdcall MainMenusOnLoginChange(DWORD dwZero);
static void __fastcall MainMenusLoadShards(void);
static BOOL __fastcall MainMenusPrefixMatch(const char *pszPrefix, const char *pszText);
static BOOL __stdcall MainMenusOnRealmChange(DWORD dwZero);
static void __fastcall MainMenusAccountConfirm(void);

const char * const gszPaletteCharDat = "palette\\fechar\\pal.dat";
const char * const gszPaletteCharPl2 = "palette\\fechar\\pal.pl2";
const char * const gszPaletteSkyDat = "palette\\sky\\pal.dat";
const char * const gszPaletteSkyPl2 = "palette\\sky\\pal.pl2";

static D2WINFONTDESC sgatFontDesc[9] =
{
	{ 3, 0 }, { 2, 0 }, { 7, 0 }, { 1, 0 }, { 0, 4 }, { 4, 1 }, { 5, 0 }, { 4, 0 }, { 8, 0 },
};

static D2WINANIMSTATE sgatAnimStates[35] =
{
	  { &sgahCel1002de74[0], NULL, 5, 3 },
	  { &sgahCel1002de74[1], NULL, 5, 3 },
	  { &sgahCel1002de74[2], NULL, 5, 1 },
	  { &sgahCel1002de74[3], NULL, 5, 2 },
	  { &sgahCel1002de74[4], NULL, 5, 1 },
	  { &sgahCel1002de88[0], NULL, 5, 3 },
	  { &sgahCel1002de88[1], NULL, 5, 3 },
	  { &sgahCel1002de88[2], &sgahCel1002def4[0], 3, 1 },
	  { &sgahCel1002de88[3], &sgahCel1002def4[1], 3, 2 },
	  { &sgahCel1002de88[4], &sgahCel1002def4[2], 3, 1 },
	  { &sgahCel1002de9c[0], NULL, 5, 2 },
	  { &sgahCel1002de9c[1], NULL, 5, 2 },
	  { &sgahCel1002de9c[2], &sghCel1002df1c, 5, 1 },
	  { &sgahCel1002de9c[3], NULL, 5, 1 },
	  { &sgahCel1002de9c[4], NULL, 5, 1 },
	  { &sgahCel1002deb0[0], NULL, 5, 2 },
	  { &sgahCel1002deb0[1], NULL, 5, 2 },
	  { &sgahCel1002deb0[2], &sgahCel1002df08[0], 3, 1 },
	  { &sgahCel1002deb0[3], &sgahCel1002df08[1], 3, 1 },
	  { &sgahCel1002deb0[4], &sgahCel1002df08[2], 3, 1 },
	  { &sgahCel1002dec4[0], NULL, 5, 2 },
	  { &sgahCel1002dec4[1], NULL, 5, 2 },
	  { &sgahCel1002dec4[2], &sghCel1002df30, 5, 1 },
	  { &sgahCel1002dec4[3], NULL, 5, 2 },
	  { &sgahCel1002dec4[4], NULL, 5, 1 },
	  { &sghCelLogoBlackLeft, &sghCelLogoFireLeft, 3, 1 },
	  { &sghCelLogoBlackLeft, &sghCelLogoFireLeft, 3, 1 },
	  { &sghCelLogoBlackLeft, &sghCelLogoFireLeft, 3, 1 },
	  { &sghCelLogoBlackLeft, &sghCelLogoFireLeft, 3, 1 },
	  { &sghCelLogoBlackLeft, &sghCelLogoFireLeft, 3, 1 },
	  { &sghCelLogoBlackRight, &sghCelLogoFireRight, 3, 1 },
	  { &sghCelLogoBlackRight, &sghCelLogoFireRight, 3, 1 },
	  { &sghCelLogoBlackRight, &sghCelLogoFireRight, 3, 1 },
	  { &sghCelLogoBlackRight, &sghCelLogoFireRight, 3, 1 },
	  { &sghCelLogoBlackRight, &sghCelLogoFireRight, 3, 1 },
};

static D2LAUNCHBAR sgatBars[3] =
{
	{ { 48, 501, 319 }, 0 }, { { 51, 456, 270 }, 0 }, { { 681, 467, 323 }, 0 },
};

D2WIN_STRUCT sgatControls[NUM_CONTROLS] =
{
	  { D2WIN_IMAGE, 0, 479, 640, 480, 0, 0,
		&sghCelBlizno, (DWORD)MainMenusOnIntro, 0, 0, 0 },
	  { D2WIN_TIMER, 8, 0, 0, 0, 0, 0, NULL, (DWORD)MainMenusOnIntro, 0, 0, 0 },
	  { D2WIN_IMAGE, 0, 599, 800, 600, 0, 0,
		&sghCelTrademark, (DWORD)MainMenusOnTitleEnd, 0, (DWORD)MainMenusOnTitleEnd, 0 },
	  { D2WIN_TIMER, 9, 0, 0, 0, 0, 0, NULL, (DWORD)MainMenusOnTitleEnd, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 100, 580, 600, 80, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[6], 2 },
	  { D2WIN_IMAGE, 240, 120, 320, 151, 0, 0, &sghCelDiablo2, 0, 0, 0, 0 },
	  { D2WIN_ANIMIMAGE, 400, 120, 181, 170, 0, 0,
		NULL, 0, (DWORD)&sgatAnimStates[25], 0, 0 },
	  { D2WIN_ANIMIMAGE, 400, 120, 188, 177, 0, 0,
		NULL, 0, (DWORD)&sgatAnimStates[30], 0, 0 },
	  { D2WIN_IMAGE, 0, 599, 800, 600, 0, 0, &sghCelTitleScreen, 0, 0, 0, 0 },
	  { D2WIN_BUTTON, 264, 244, 272, 35, 0, 5106,
		&sghCelWideButton, (DWORD)MainMenusOnSinglePlayer, 0, 0, 0 },
	  { D2WIN_BUTTON, 264, 284, 272, 35, 0, 5107,
		&sghCelWideButton, (DWORD)MainMenusOnBattleNet, 0, 0, 0 },
	  { D2WIN_BUTTON, 264, 324, 272, 35, 0, 5108,
		&sghCelWideButton, (DWORD)MainMenusOnOtherMultiplayer, 0, 0, 0 },
	  { D2WIN_BUTTON, 264, 528, 135, 25, 0, 5110,
		&sghCelShortButton, (DWORD)MainMenusOnCredits, 0, 0, 0 },
	  { D2WIN_BUTTON, 402, 528, 135, 25, 0, 5111,
		&sghCelShortButton, (DWORD)MainMenusOnCinematics, 0, 0, 0 },
	  { D2WIN_BUTTON, 264, 568, 272, 35, 27, 5109,
		&sghCelWideButton, (DWORD)MainMenusOnExit, 0, 0, 0 },
	  { D2WIN_IMAGE, 319, 347, 169, 26, 0, 0, &sghCelTextBox2, 0, 0, 0, 0 },
	  { D2WIN_IMAGE, 319, 401, 169, 26, 0, 0, &sghCelTextBox2, 0, 0, 0, 0 },
	  { D2WIN_IMAGE, 319, 455, 169, 26, 0, 0, &sghCelTextBox2, 0, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 321, 340, 200, 32, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	  { D2WIN_TEXTBOX, 321, 394, 200, 32, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	  { D2WIN_TEXTBOX, 321, 448, 200, 32, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	  { D2WIN_TEXTBOX, 0, 260, 800, 50, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[1], 2 },
	  { D2WIN_TEXTBOX, 250, 350, 300, 100, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 2 },
	  { D2WIN_EDITBOX, 322, 342, 162, 19, 8, 4,
		NULL, 0, (DWORD)MainMenusOnLogin, (DWORD)&sgatFontDesc[6], 0 },
	  { D2WIN_EDITBOX, 322, 396, 162, 19, 8, 4,
		NULL, 0, (DWORD)MainMenusOnLogin, (DWORD)&sgatFontDesc[6], 33 },
	  { D2WIN_BUTTON, 264, 572, 272, 35, 0, 5221,
		&sghCelWideButton, (DWORD)MainMenusOnNewAccount, 0, 0, 0 },
	  { D2WIN_BUTTON, 264, 528, 272, 35, 13, 5288,
		&sghCelWideButton, (DWORD)MainMenusOnLogin, 0, 0, 0 },
	  { D2WIN_BUTTON, 33, 572, 128, 35, 27, 5101,
		&sghCelMediumButton, (DWORD)MainMenusOnReturn, 0, 0, 0 },
	  { D2WIN_IMAGE, 0, 599, 800, 600, 0, 0, &sghCelCreditsBckg, 0, 0, 0, 0 },
	  { D2WIN_BUTTON, 33, 578, 128, 35, 27, 5103,
		&sghCelMediumButton, (DWORD)MainMenusOnReturn, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 560, 615, 250, 610, 0, 0,
		NULL, 0, 0, (DWORD)&sgatFontDesc[5], 144 },
	  { D2WIN_TEXTBOX, 570, 615, 250, 610, 0, 0,
		NULL, 0, 0, (DWORD)&sgatFontDesc[5], 128 },
	  { D2WIN_TEXTBOX, 440, 615, 250, 610, 0, 0,
		NULL, 0, 0, (DWORD)&sgatFontDesc[5], 130 },
	  { D2WIN_TIMER, 50, 0, 0, 0, 0, 0, NULL, (DWORD)MainMenusOnCreditsTimer, 0, 0, 0 },
	  { D2WIN_IMAGE, 237, 555, 326, 341, 0, 0, &sghCelCinematicsSelection, 0, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 262, 283, 272, 35, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[2], 2 },
	  { D2WIN_BUTTON, 262, 316, 272, 35, 0, 4137,
		&sghCelWideButton, (DWORD)MainMenusOnFilmIntro, 0, 0, 0 },
	  { D2WIN_BUTTON, 262, 359, 272, 35, 0, 4138,
		&sghCelWideButton, (DWORD)MainMenusOnFilmActTwoStart, 0, 0, 0 },
	  { D2WIN_BUTTON, 262, 402, 272, 35, 0, 4139,
		&sghCelWideButton, (DWORD)MainMenusOnFilmActThreeStart, 0, 0, 0 },
	  { D2WIN_BUTTON, 262, 445, 272, 35, 0, 4140,
		&sghCelWideButton, (DWORD)MainMenusOnFilmActFourStart, 0, 0, 0 },
	  { D2WIN_BUTTON, 262, 488, 272, 35, 0, 5113,
		&sghCelWideButton, (DWORD)MainMenusOnFilmEnd, 0, 0, 0 },
	  { D2WIN_BUTTON, 334, 538, 128, 35, 27, 5103,
		&sghCelMediumButton, (DWORD)MainMenusOnCinematicsCancel, 0, 0, 0 },
	  { D2WIN_BUTTON, 281, 337, 96, 32, 27, 5103,
		&sghCelCancelButton, (DWORD)MainMenusOnDiscCancel, 0, 0, 2 },
	  { D2WIN_BUTTON, 421, 337, 96, 32, 13, 5102,
		&sghCelCancelButton, (DWORD)MainMenusOnCinematicsDiscOk, 0, 0, 2 },
	  { D2WIN_BUTTON, 351, 337, 96, 32, 13, 5102,
		&sghCelCancelButton, (DWORD)MainMenusOnPlayDiscOk, 0, 0, 2 },
	  { D2WIN_IMAGE, 103, 526, 593, 451, 0, 0, &sghCelTermsOfUseBckg, 0, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 120, 467, 560, 318, 0, 0,
		&sghCelJoinGameScrollbars, 0, (DWORD)&sgatBars[2], (DWORD)&sgatFontDesc[7], 4 },
	  { D2WIN_BUTTON, 525, 513, 128, 35, 0, 5181,
		&sghCelMediumButton, (DWORD)MainMenusOnTermsAgree, 0, 0, 0 },
	  { D2WIN_BUTTON, 133, 513, 128, 35, 13, 5182,
		&sghCelMediumButton, (DWORD)MainMenusOnToLoginScreen, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 103, 173, 570, 69, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 2 },
	  { D2WIN_BUTTON, 900, 900, 128, 35, 38, 0,
		NULL, (DWORD)MainMenusOnTermsUp, 0, 0, 6 },
	  { D2WIN_BUTTON, 900, 900, 128, 35, 40, 0,
		NULL, (DWORD)MainMenusOnTermsDown, 0, 0, 6 },
	  { D2WIN_EDITBOX, 322, 342, 162, 19, 8, 4,
		NULL, 0, (DWORD)MainMenusOnNewAccountOk, (DWORD)&sgatFontDesc[6], 0 },
	  { D2WIN_EDITBOX, 322, 396, 162, 19, 8, 4,
		NULL, 0, (DWORD)MainMenusOnNewAccountOk, (DWORD)&sgatFontDesc[6], 33 },
	  { D2WIN_EDITBOX, 322, 450, 162, 19, 8, 4,
		NULL, 0, (DWORD)MainMenusOnNewAccountOk, (DWORD)&sgatFontDesc[6], 33 },
	  { D2WIN_BUTTON, 627, 572, 128, 35, 13, 5102,
		&sghCelMediumButton, (DWORD)MainMenusOnNewAccountOk, 0, 0, 0 },
	  { D2WIN_BUTTON, 33, 572, 128, 35, 27, 5101,
		&sghCelMediumButton, (DWORD)MainMenusOnToLoginScreen, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 103, 173, 570, 69, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 2 },
	  { D2WIN_TEXTBOX, 110, 480, 570, 318, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[6], 2 },
	  { D2WIN_TEXTBOX, 110, 580, 570, 318, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[6], 2 },
	  { D2WIN_TEXTBOX, 110, 700, 570, 318, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[6], 2 },
	  { D2WIN_BUTTON, 525, 513, 128, 35, 13, 5102,
		&sghCelMediumButton, (DWORD)MainMenusOnAccountCreate, 0, 0, 0 },
	  { D2WIN_BUTTON, 133, 513, 128, 35, 27, 5103,
		&sghCelMediumButton, (DWORD)MainMenusOnAccountCancel, 0, 0, 0 },
	  { D2WIN_BUTTON, 514, 572, 272, 35, 0, 5273,
		&sghCelWideButton, (DWORD)MainMenusOnNewCharacter, 0, 0, 0 },
	  { D2WIN_BUTTON, 33, 572, 128, 35, 27, 5101,
		&sghCelMediumButton, (DWORD)MainMenusOnCharSelExit, 0, 0, 0 },
	  { D2WIN_BUTTON, 193, 572, 128, 35, 0, 5272,
		&sghCelMediumButton, (DWORD)CharSelOnDelete, 0, 0, 0 },
	  { D2WIN_BUTTON, 353, 572, 128, 35, 13, 5102,
		&sghCelMediumButton, (DWORD)CharSelOnOk, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 514, 153, 200, 90, 76, 15,
		NULL, (DWORD)CharSelOnSlot, 0, (DWORD)&sgatFontDesc[3], 32 },
	  { D2WIN_TEXTBOX, 514, 246, 200, 90, 76, 15,
		NULL, (DWORD)CharSelOnSlot, 0, (DWORD)&sgatFontDesc[3], 32 },
	  { D2WIN_TEXTBOX, 514, 339, 200, 90, 76, 15,
		NULL, (DWORD)CharSelOnSlot, 0, (DWORD)&sgatFontDesc[3], 32 },
	  { D2WIN_TEXTBOX, 514, 432, 200, 90, 76, 15,
		NULL, (DWORD)CharSelOnSlot, 0, (DWORD)&sgatFontDesc[3], 32 },
	  { D2WIN_TEXTBOX, 514, 525, 200, 90, 76, 15,
		NULL, (DWORD)CharSelOnSlot, 0, (DWORD)&sgatFontDesc[3], 32 },
	  { D2WIN_IMAGE, 514, 1000, 272, 93, 0, 0,
		&sghCelCharSelectBox, 0, 0, (DWORD)CharSelOnKey, 0 },
	  { D2WIN_IMAGE, 514, 1000, 272, 93, 0, 0,
		&sghCelCharSelectBoxGrey, 0, 0, (DWORD)CharSelOnKey, 0 },
	  { D2WIN_BUTTON, 634, 35, 29, 20, 105, 0,
		&sghCelSelectCharArrows, (DWORD)CharSelOnUp, 0, 0, 0 },
	  { D2WIN_BUTTON, 634, 566, 29, 20, 99, 0,
		&sghCelSelectCharArrows, (DWORD)CharSelOnDown, 2, 0, 0 },
	  { D2WIN_TEXTBOX, 70, 103, 450, 50, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 34 },
	  { D2WIN_TEXTBOX, 86, 143, 400, 50, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[1], 2 },
	  { D2WIN_TEXTBOX, 86, 178, 400, 50, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[1], 2 },
	  { D2WIN_IMAGE, 512, 150, 84, 86, 0, 0,
		&sghCelCreateNewChar, 0, 0, (DWORD)CharSelOnKey, 0 },
	  { D2WIN_TEXTBOX, 86, 300, 400, 200, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[8], 2 },
	  { D2WIN_BUTTON, 264, 297, 272, 35, 0, 5156,
		&sghCelWideButton, (DWORD)CharSelOnDifficultyNormal, 0, 0, 0 },
	  { D2WIN_BUTTON, 264, 340, 272, 35, 0, 5155,
		&sghCelWideButton, (DWORD)CharSelOnDifficultyNightmare, 0, 0, 0 },
	  { D2WIN_BUTTON, 264, 383, 272, 35, 0, 5154,
		&sghCelWideButton, (DWORD)CharSelOnDifficultyHell, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 264, 260, 272, 35, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[2], 2 },
	  { D2WIN_IMAGE, 237, 400, 326, 200, 0, 0, &sghCelDifficultyLevels, 0, 0, 0, 0 },
	  { D2WIN_BUTTON, 900, 900, 10, 10, 27, 0,
		NULL, (DWORD)MainMenusOnDifficultyCancel, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 0, 80, 800, 50, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[1], 2 },
	  { D2WIN_BUTTON, 33, 572, 128, 35, 27, 5101,
		&sghCel1002de00, (DWORD)MainMenusOnToCharSel, 0, 0, 0 },
	  { D2WIN_BUTTON, 627, 572, 128, 35, 13, 5102,
		&sghCel1002de00, (DWORD)MainMenusOnNewCharOk, 0, 0, 0 },
	  { D2WIN_IMAGE, 0, 599, 800, 600, 0, 0, &sghCel1002ddec, 0, 0, 0, 0 },
	  { D2WIN_ANIMIMAGE, 345, 454, 110, 127, 1, 3, &sghCel1002ddf0, 0, 0, 0, 0 },
	  { D2WIN_ANIMIMAGE, 195, 341, 88, 184, 0, 0,
		NULL, (DWORD)MainMenusOnHero, (DWORD)&sgatAnimStates[0], (DWORD)MainMenusOnHoverAmazon, 0 },
	  { D2WIN_ANIMIMAGE, 301, 333, 88, 184, 0, 0,
		NULL, (DWORD)MainMenusOnHero, (DWORD)&sgatAnimStates[5], (DWORD)MainMenusOnHoverNecromancer, 0 },
	  { D2WIN_ANIMIMAGE, 400, 320, 88, 184, 0, 0,
		NULL, (DWORD)MainMenusOnHero, (DWORD)&sgatAnimStates[10], (DWORD)MainMenusOnHoverBarbarian, 0 },
	  { D2WIN_ANIMIMAGE, 521, 344, 88, 184, 0, 0,
		NULL, (DWORD)MainMenusOnHero, (DWORD)&sgatAnimStates[15], (DWORD)MainMenusOnHoverSorceress, 0 },
	  { D2WIN_ANIMIMAGE, 610, 359, 88, 184, 0, 0,
		NULL, (DWORD)MainMenusOnHero, (DWORD)&sgatAnimStates[20], (DWORD)MainMenusOnHoverPaladin, 0 },
	  { D2WIN_TEXTBOX, 0, 180, 800, 100, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[1], 2 },
	  { D2WIN_TEXTBOX, 250, 210, 300, 100, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 2 },
	  { D2WIN_IMAGE, 319, 539, 169, 26, 0, 0, &sghCel1002de54, 0, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 321, 532, 200, 32, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	  { D2WIN_TEXTBOX, 339, 581, 100, 32, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	  { D2WIN_BUTTON, 319, 560, 15, 16, 0, 0,
		&sghCel1002de58, (DWORD)MainMenusOnHardcoreBox, 0, 0, 2 },
	  { D2WIN_EDITBOX, 318, 530, 157, 16, 8, 4,
		NULL, 0, (DWORD)MainMenusOnNewCharOk, (DWORD)&sgatFontDesc[6], 0 },
	  { D2WIN_IMAGE, 268, 350, 264, 176, 0, 0, &sghCelPopUpOkCancel, 0, 0, 0, 0 },
	  { D2WIN_BUTTON, 281, 337, 96, 32, 0, 5103,
		&sghCelOkCancelButton, (DWORD)MainMenusOnHardcoreCancel, 0, 0, 2 },
	  { D2WIN_BUTTON, 421, 337, 96, 32, 0, 5102,
		&sghCelOkCancelButton, (DWORD)MainMenusOnHardcoreOk, 0, 0, 2 },
	  { D2WIN_TEXTBOX, 268, 320, 264, 120, 10, 8,
		NULL, 0, 0, (DWORD)&sgatFontDesc[3], 2 },
	  { D2WIN_IMAGE, 268, 350, 264, 176, 0, 0, &sghCelPopUpOkCancel2, 0, 0, 0, 0 },
	  { D2WIN_BUTTON, 281, 337, 96, 32, 27, 5167,
		&sghCelCancelButton, (DWORD)CharSelOnDeleteNo, 0, 0, 2 },
	  { D2WIN_BUTTON, 421, 337, 96, 32, 0, 5166,
		&sghCelCancelButton, (DWORD)CharSelOnDeleteYes, 0, 0, 2 },
	  { D2WIN_TEXTBOX, 268, 320, 264, 120, 10, 8,
		NULL, 0, 0, (DWORD)&sgatFontDesc[3], 2 },
	  { D2WIN_IMAGE, 268, 350, 264, 176, 0, 0, &sghCelPopUpOk, 0, 0, 0, 0 },
	  { D2WIN_BUTTON, 351, 337, 96, 32, 27, 5103,
		&sghCelCancelButton, (DWORD)MainMenusOnBoxButton, 0, 0, 2 },
	  { D2WIN_TEXTBOX, 268, 337, 264, 120, 10, 8,
		NULL, 0, 0, (DWORD)&sgatFontDesc[2], 2 },
	  { D2WIN_BUTTON, 351, 337, 96, 32, 13, 5102,
		&sghCelCancelButton, (DWORD)MainMenusOnBoxButton, 0, 0, 2 },
	  { D2WIN_IMAGE, 222, 426, 340, 224, 0, 0, &sghCelPopUp340x224, 0, 0, 0, 0 },
	  { D2WIN_BUTTON, 330, 416, 128, 35, 27, 5103,
		&sghCelMediumButton, (DWORD)MainMenusOnBoxButton, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 222, 280, 340, 60, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[1], 2 },
	  { D2WIN_TEXTBOX, 222, 360, 340, 70, 10, 8, NULL, 0, 0, (DWORD)&sgatFontDesc[2], 2 },
	  { D2WIN_TEXTBOX, 222, 370, 340, 20, 10, 4, NULL, 0, 0, (DWORD)&sgatFontDesc[6], 2 },
	  { D2WIN_IMAGE, 195, 422, 410, 245, 0, 0, &sghCelPopUpWide, 0, 0, 0, 0 },
	  { D2WIN_BUTTON, 335, 412, 128, 35, 27, 5103,
		&sghCelMediumButton, (DWORD)MainMenusOnBoxButton, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 195, 237, 410, 40, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[1], 2 },
	  { D2WIN_TEXTBOX, 199, 377, 402, 140, 10, 8,
		NULL, 0, 0, (DWORD)&sgatFontDesc[2], 0 },
	  { D2WIN_IMAGE, 158, 460, 485, 290, 0, 0, &sghCelPopUpLargest, 0, 0, 0, 0 },
	  { D2WIN_BUTTON, 335, 450, 128, 35, 27, 5103,
		&sghCelMediumButton, (DWORD)MainMenusOnBoxButton, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 158, 220, 485, 40, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[1], 2 },
	  { D2WIN_TEXTBOX, 162, 420, 477, 200, 10, 0,
		NULL, 0, 0, (DWORD)&sgatFontDesc[2], 0 },
	  { D2WIN_TEXTBOX, 162, 320, 477, 100, 10, 0,
		NULL, 0, 0, (DWORD)&sgatFontDesc[2], 0 },
	  { D2WIN_TEXTBOX, 162, 420, 477, 100, 10, 0,
		NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	  { D2WIN_IMAGE, 158, 460, 485, 290, 0, 0, &sghCelPopUpLargest, 0, 0, 0, 0 },
	  { D2WIN_BUTTON, 335, 450, 128, 35, 27, 5103,
		&sghCelMediumButton, (DWORD)MainMenusOnBoxButton, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 158, 220, 485, 40, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[1], 2 },
	  { D2WIN_TEXTBOX, 162, 270, 477, 50, 10, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[2], 0 },
	  { D2WIN_TEXTBOX, 158, 310, 485, 40, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[2], 2 },
	  { D2WIN_TEXTBOX, 162, 420, 477, 100, 10, 0,
		NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	  { D2WIN_IMAGE, 268, 350, 264, 176, 0, 0, &sghCelPopUpOk2, 0, 0, 0, 0 },
	  { D2WIN_BUTTON, 351, 337, 96, 32, 27, 5103,
		&sghCelOkCancelButton, (DWORD)MainMenusOnBoxButton, 0, 0, 2 },
	  { D2WIN_TEXTBOX, 268, 320, 264, 120, 10, 8,
		NULL, 0, 0, (DWORD)&sgatFontDesc[2], 2 },
	  { D2WIN_TIMER, 40, 0, 0, 0, 0, 0, NULL, (DWORD)MainMenusOnConnectTimer, 0, 0, 0 },
	  { D2WIN_BUTTON, 351, 337, 96, 32, 27, 5103,
		&sghCelCancelButton, (DWORD)MainMenusOnPatchCancel, 0, 0, 2 },
	  { D2WIN_BUTTON, 351, 337, 96, 32, 13, 5102,
		&sghCelCancelButton, (DWORD)MainMenusOnPatchCancel, 0, 0, 2 },
	  { D2WIN_TEXTBOX, 268, 337, 264, 120, 10, 8,
		NULL, 0, 0, (DWORD)&sgatFontDesc[2], 2 },
	  { D2WIN_IMAGE, 0, 599, 800, 600, 0, 0, &sghCelRealmBckg, 0, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 100, 79, 600, 40, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 2 },
	  { D2WIN_TEXTBOX, 57, 483, 200, 20, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	  { D2WIN_EDITBOX, 126, 505, 240, 20, 0, 0,
		NULL, 0, (DWORD)MainMenusOnRealmOk, (DWORD)&sgatFontDesc[6], 0 },
	  { D2WIN_TEXTBOX, 71, 459, 329, 272, 0, 0,
		&sghCelJoinGameScrollbars, (DWORD)MainMenusOnShard, (DWORD)&sgatBars[1], (DWORD)&sgatFontDesc[6], 5 },
	  { D2WIN_TEXTBOX, 451, 216, 315, 130, 2, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[8], 0 },
	  { D2WIN_TEXTBOX, 451, 317, 315, 20, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	  { D2WIN_TEXTBOX, 451, 501, 315, 188, 8, 5, NULL, 0, 0, (DWORD)&sgatFontDesc[8], 2 },
	  { D2WIN_BUTTON, 45, 570, 128, 35, 27, 5101,
		&sghCelMediumButton, (DWORD)MainMenusOnRealmCancel, 0, 0, 0 },
	  { D2WIN_BUTTON, 639, 570, 128, 35, 13, 5102,
		&sghCelMediumButton, (DWORD)MainMenusOnRealmOk, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 48, 240, 330, 150, 2, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[8], 0 },
	  { D2WIN_TEXTBOX, 48, 184, 315, 20, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 0 },
	  { D2WIN_TEXTBOX, 451, 360, 325, 188, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[8], 0 },
	  { D2WIN_TEXTBOX, 451, 425, 325, 188, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[8], 0 },
	  { D2WIN_BUTTON, 264, 244, 272, 35, 0, 5115,
		&sghCelWideButton, (DWORD)MainMenusOnOpenBattleNet, 0, 0, 0 },
	  { D2WIN_BUTTON, 264, 284, 272, 35, 0, 5116,
		&sghCelWideButton, (DWORD)MainMenusOnTcpIp, 0, 0, 0 },
	  { D2WIN_BUTTON, 264, 568, 272, 35, 27, 5103,
		&sghCelWideButton, (DWORD)MainMenusOnBackToMain, 0, 0, 0 },
	  { D2WIN_IMAGE, 0, 599, 800, 600, 0, 0, &sghCelTcpIpBckg, 0, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 100, 76, 600, 40, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[0], 2 },
	  { D2WIN_TEXTBOX, 270, 156, 272, 40, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 2 },
	  { D2WIN_TEXTBOX, 265, 171, 272, 40, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 2 },
	  { D2WIN_BUTTON, 265, 206, 272, 35, 0, 5118,
		&sghCelWideButton, (DWORD)MainMenusOnHostGame, 0, (DWORD)MainMenusOnHostHover, 0 },
	  { D2WIN_BUTTON, 265, 264, 272, 35, 0, 5119,
		&sghCelWideButton, (DWORD)MainMenusOnJoinGame, 0, (DWORD)MainMenusOnJoinHover, 0 },
	  { D2WIN_TEXTBOX, 265, 520, 272, 210, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[8], 2 },
	  { D2WIN_TEXTBOX, 265, 520, 272, 210, 2, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[8], 2 },
	  { D2WIN_BUTTON, 39, 571, 128, 35, 27, 5103,
		&sghCelMediumButton, (DWORD)MainMenusOnOtherMultiplayer, 0, 0, 0 },
	  { D2WIN_TIMER, 5000, 0, 0, 0, 0, 0, NULL, (DWORD)MainMenusOnTcpIpTimer, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 300, 250, 200, 50, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 2 },
	  { D2WIN_EDITBOX, 300, 268, 213, 20, 0, 0,
		NULL, 0, (DWORD)MainMenusOnJoinOk, (DWORD)&sgatFontDesc[6], 0 },
	  { D2WIN_IMAGE, 291, 270, 218, 26, 0, 0, &sghCelIpAddressBox, 0, 0, 0, 0 },
	  { D2WIN_IMAGE, 268, 350, 264, 176, 0, 0, &sghCelPopUpOkCancel2, 0, 0, 0, 0 },
	  { D2WIN_BUTTON, 281, 337, 96, 32, 27, 5103,
		&sghCelCancelButton, (DWORD)MainMenusOnJoinCancel, 0, 0, 0 },
	  { D2WIN_BUTTON, 421, 337, 96, 32, 13, 5102,
		&sghCelCancelButton, (DWORD)MainMenusOnJoinOk, 0, 0, 0 },
	  { D2WIN_TEXTBOX, 0, 96, 800, 30, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[1], 2 },
	  { D2WIN_TEXTBOX, 323, 134, 300, 30, 0, 0,
		NULL, 0, 0, (DWORD)&sgatFontDesc[1], 16 },
	  { D2WIN_TEXTBOX, 351, 134, 425, 30, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[1], 0 },
	  { D2WIN_TEXTBOX, 0, 599, 200, 40, 0, 0, NULL, 0, 0, (DWORD)&sgatFontDesc[3], 2 },
	  { D2WIN_IMAGE, 0, 599, 800, 600, 0, 0, &sghCelDownloadPatchBckg, 0, 0, 0, 0 },
	  { D2WIN_IMAGE, 400, 300, 207, 172, 0, 0, &sghCelDownloadPatchBall, 0, 0, 0, 0 },
	  { D2WIN_BUTTON, 336, 540, 128, 35, 27, 5103,
		&sghCelMediumButton, (DWORD)MainMenusOnPatchCancel, 0, 0, 0 },
};

static int sgnClassHover = -1;
static int sgnClass = -1;
static int sgnShardSelected = -2;
static int PLACEHOLDER_UnreferencedData_1002a4dc = -1;

static void __fastcall MainMenusFreeCels(void);
static void __fastcall MainMenusLogo(void);
static void __fastcall MainMenusTitle(void);

void __fastcall MainMenusSetPending(PFNLAUNCHPENDING pfnPending)
{
	gpfnPending = pfnPending;
}

void __fastcall MainMenusSetWaitMark(BOOL fUnused)
{
	gf1002dfb8 = TRUE;
}

BOOL __fastcall MainMenusGetWaitMark(void)
{
	return gf1002dfb8;
}

BOOL __stdcall MainMenusEventLoop(DWORD dwArg)
{
	if (WaitForSingleObject(sghMcpConnectThread, 0) == WAIT_OBJECT_0 || sgfWaitCancelled)
		D2WinQuitLoop();
	return TRUE;
}

BOOL __stdcall MainMenusMcpLoopCreateCharacter(DWORD dwArg)
{
	D2MCPClientD2MCPPoll();
	if (D2MCPClientD2MCPGetCreateCharacterReply() || !D2MCPClientD2MCPIsConnected() || sgfWaitCancelled)
	{
		D2MCPClientD2MCPSetCreateCharacterReply(0);
		D2WinQuitLoop();
	}
	return TRUE;
}

BOOL __stdcall MainMenusMcpLoopSelectCharacter(DWORD dwArg)
{
	D2MCPClientD2MCPPoll();
	if (D2MCPClientD2MCPGetSelectCharacterReply() || !D2MCPClientD2MCPIsConnected() || sgfWaitCancelled)
	{
		D2MCPClientD2MCPSetSelectCharacterReply(0);
		D2WinQuitLoop();
	}
	return TRUE;
}

BOOL __stdcall MainMenusMcpLoopDeleteCharacter(DWORD dwArg)
{
	D2MCPClientD2MCPPoll();
	if (D2MCPClientD2MCPGetDeleteCharacterReply() || !D2MCPClientD2MCPIsConnected() || sgfWaitCancelled)
	{
		D2MCPClientD2MCPSetDeleteCharacterReply(0);
		D2WinQuitLoop();
	}
	return TRUE;
}

BOOL __fastcall MainMenusLoadPalette(const char *pszDat, const char *pszPl2)
{
	char szPl2[260];
	char szDat[260];

	sgfPaletteLoaded = TRUE;
	wsprintfA(szDat, "%s\\%s", "DATA\\GLOBAL", pszDat);
	wsprintfA(szPl2, "%s\\%s", "DATA\\GLOBAL", pszPl2);
	D2WinPaletteLoad(szDat, szPl2);
	return TRUE;
}

D2WINCONTROL * __fastcall MainMenusCreateBoxChild(int nControl)
{
	D2WINCONTROL *ptChild;

#line 1173
	D2ASSERT(sgnNumSubChildren < MAX_CHILDREN);
	ptChild = (D2WINCONTROL *)D2WinCreateChild(&sgatControls[nControl]);
	sgaptSubChildren[sgnNumSubChildren] = ptChild;
	sgnNumSubChildren++;
	return ptChild;
}

void __fastcall MainMenusDestroyBoxChildren(void)
{
	int i;

	for (i = 0; i < sgnNumSubChildren; i++)
		D2WinDestroyChild(&sgaptSubChildren[i]);
	sgnNumSubChildren = 0;
}

D2WINCONTROL * __fastcall MainMenusCreateChild(int nControl)
{
	D2WINCONTROL *ptChild;

#line 1196
	D2ASSERT(sgnNumChildren < MAX_CHILDREN);
	ptChild = (D2WINCONTROL *)D2WinCreateChild(&sgatControls[nControl]);
	sgaptChildren[sgnNumChildren] = ptChild;
	sgnNumChildren++;
	return ptChild;
}

static void __stdcall MainMenusRestorePalette(void)
{
	MainMenusLoadPalette(gszPaletteCharDat, gszPaletteCharPl2);
	sgfPaletteLoaded = FALSE;
}

void __fastcall MainMenusClear(void)
{
	int i;

	for (i = 0; i < sgnNumChildren; i++)
		D2WinDestroyChild(&sgaptChildren[i]);
	sgdw1002dfb0 = 0;
	sgdw1002dfb4 = 0;
	sgnNumChildren = 0;
	MainMenusDestroyBoxChildren();
	D2WinSetBackground(0);
	if (sgfPaletteLoaded)
		MainMenusRestorePalette();
	CharSelFree();
	D2WinShowCursor(TRUE);
}

BOOL __stdcall MainMenusOnToLogin(_PARAMS *ptMsg)
{
	gf1002dfb8 = TRUE;
	MainMenusOpenLogin();
	return TRUE;
}

void __fastcall MainMenusTitleBox(const WORD *pwszTitle, const WORD *pwszText, PFNLAUNCHPENDING pfnAfter)
{
	D2WINCONTROL *ptChild;

	MainMenusDestroyBoxChildren();
	ptChild = MainMenusCreateBoxChild(121);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(122);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(123);
	D2WinChildSetOnTop(ptChild, TRUE);
	MainMenusSetBoxText(ptChild, pwszTitle, 0, 0);
	ptChild = MainMenusCreateBoxChild(124);
	D2WinChildSetOnTop(ptChild, TRUE);
	MainMenusSetBoxText(ptChild, pwszText, 0, 0);
	gpfnPending = pfnAfter;
}

void __fastcall MainMenusTitleBoxLarge(const WORD *pwszTitle, const WORD *pwszText, const WORD *pwszText2,
	PFNLAUNCHPENDING pfnAfter)
{
	D2WINCONTROL *ptChild;

	MainMenusDestroyBoxChildren();
	ptChild = MainMenusCreateBoxChild(125);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(126);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(127);
	D2WinChildSetOnTop(ptChild, TRUE);
	MainMenusSetBoxText(ptChild, pwszTitle, 0, 0);
	if (!pwszText2 || !*pwszText2)
	{
		ptChild = MainMenusCreateBoxChild(128);
		D2WinChildSetOnTop(ptChild, TRUE);
		MainMenusSetBoxText(ptChild, pwszText, 0, 0);
	}
	else
	{
		ptChild = MainMenusCreateBoxChild(129);
		D2WinChildSetOnTop(ptChild, TRUE);
		MainMenusSetBoxText(ptChild, pwszText, 0, 0);
		ptChild = MainMenusCreateBoxChild(130);
		D2WinChildSetOnTop(ptChild, TRUE);
		MainMenusSetBoxText(ptChild, pwszText2, 4, 0);
	}
	MainMenusSetPending(pfnAfter);
}

static BOOL __fastcall MainMenusSetBoxText(D2WINCONTROL *ptBox, const WORD *pwszText, DWORD dwColor, BYTE bBroken)
{
	Unicode awszText[1024];
	WORD *pwszLine;
	WORD *pwsz;

	Unicode::strncpy(awszText, pwszText, 1024);
	pwszLine = pwsz = (WORD *)awszText;
	if (*pwsz)
	{
		do
		{
			if (*pwsz == '\n')
			{
				*pwsz = 0;
				D2WinTextBoxAddColorLineUnicode(ptBox, pwszLine, dwColor, bBroken);
				pwszLine = pwsz + 1;
			}
			pwsz++;
		} while (*pwsz);
		if (pwszLine < pwsz)
			D2WinTextBoxAddColorLineUnicode(ptBox, pwszLine, dwColor, bBroken);
	}
	return TRUE;
}

void __fastcall MainMenusParmBox(const WORD *pwszTitle, const WORD *pwszText, const WORD *pwszParm,
	const WORD *pwszText2, PFNLAUNCHPENDING pfnAfter)
{
	D2WINCONTROL *ptChild;

	MainMenusDestroyBoxChildren();
	ptChild = MainMenusCreateBoxChild(131);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(132);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(133);
	D2WinChildSetOnTop(ptChild, TRUE);
	MainMenusSetBoxText(ptChild, pwszTitle, 0, 0);
	ptChild = MainMenusCreateBoxChild(134);
	D2WinChildSetOnTop(ptChild, TRUE);
	MainMenusSetBoxText(ptChild, pwszText, 0, 0);
	ptChild = MainMenusCreateBoxChild(135);
	D2WinChildSetOnTop(ptChild, TRUE);
	MainMenusSetBoxText(ptChild, pwszParm, 0, 0);
	ptChild = MainMenusCreateBoxChild(136);
	D2WinChildSetOnTop(ptChild, TRUE);
	MainMenusSetBoxText(ptChild, pwszText2, 4, 0);
	MainMenusSetPending(pfnAfter);
}

void __fastcall MainMenusCancelBox(const WORD *pwszText, PFNLAUNCHPENDING pfnAfter)
{
	D2WINCONTROL *ptChild;

	MainMenusDestroyBoxChildren();
	ptChild = MainMenusCreateBoxChild(112);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(113);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(114);
	D2WinChildSetOnTop(ptChild, TRUE);
	MainMenusSetBoxText(ptChild, pwszText, 0, 0);
	gpfnPending = pfnAfter;
}

void __fastcall MainMenusOkBox(const WORD *pwszText, PFNLAUNCHPENDING pfnAfter)
{
	D2WINCONTROL *ptChild;

	MainMenusDestroyBoxChildren();
	ptChild = MainMenusCreateBoxChild(112);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(115);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(114);
	D2WinChildSetOnTop(ptChild, TRUE);
	MainMenusSetBoxText(ptChild, pwszText, 0, 0);
	gpfnPending = pfnAfter;
}

void __fastcall MainMenusMessageBox(const WORD *pwszText, PFNLAUNCHPENDING pfnAfter)
{
	D2WINCONTROL *ptChild;

	MainMenusDestroyBoxChildren();
	ptChild = MainMenusCreateBoxChild(137);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(138);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(139);
	D2WinChildSetOnTop(ptChild, TRUE);
	MainMenusSetBoxText(ptChild, pwszText, 0, 0);
	gpfnPending = pfnAfter;
}

void __fastcall MainMenusCancelWait(void)
{
	sgfWaitCancelled = TRUE;
}

BOOL __fastcall MainMenusWaitCancelled(void)
{
	return sgfWaitCancelled;
}

void __fastcall MainMenusWaitBox(void)
{
	D2WINCONTROL *ptChild;

	MainMenusDestroyBoxChildren();
	gf1002dfb8 = TRUE;
	sgfWaitCancelled = FALSE;
	ptChild = MainMenusCreateBoxChild(112);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(113);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(114);
	D2WinChildSetOnTop(ptChild, TRUE);
	D2WinTextBoxAddLineUnicode(ptChild, 	(const WORD *)D2LangStrGetStringByIndex(5243));
	gpfnPending = MainMenusCancelWait;
}

BOOL __stdcall MainMenusOnConnectTimer(_PARAMS *ptMsg)
{
	D2WINCONTROL *ptChild;

	gpfnPending = MainMenusCancelWait;
	MainMenusDestroyBoxChildren();
	ptChild = MainMenusCreateBoxChild(112);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(142);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(143);
	D2WinChildSetOnTop(ptChild, TRUE);
	D2WinTextBoxAddLineUTF(ptChild, "Can't connect to Battle.Net. Try again later.");
	return TRUE;
}

BOOL __stdcall MainMenusOnLoginAgain(_PARAMS *ptMsg)
{
	gpfnPending = MainMenusCancelWait;
	MainMenusDestroyBoxChildren();
	MainMenusOpenLogin();
	return TRUE;
}

DWORD __fastcall MainMenusConnectBox(void)
{
	D2WINCONTROL *ptChild;
	D2WINCONTROL *ptText;

	MainMenusDestroyBoxChildren();
	gf1002dfb8 = TRUE;
	sgfWaitCancelled = FALSE;
	ptChild = MainMenusCreateBoxChild(112);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(141);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptText = MainMenusCreateBoxChild(114);
	D2WinChildSetOnTop(ptText, TRUE);
	MainMenusCreateBoxChild(140);
	return D2WinTextBoxAddLineUnicode(ptText, 	(const WORD *)D2LangStrGetStringByIndex(5243));
}

void __fastcall MainMenusCloseBox(void)
{
	int i;

	for (i = 0; i < sgnNumSubChildren; i++)
		D2WinDestroyChild(&sgaptSubChildren[i]);
	sgnNumSubChildren = 0;
}

void __fastcall MainMenusPatchScreen(void)
{
	char szPercent[100];
	D2WINCONTROL *ptChild;
	int nPercent;

	MainMenusBackground();
	MainMenusCreateChild(181);
	sgptPatchBall = MainMenusCreateChild(182);
	D2WinImageSetDrawMode(sgptPatchBall, 3);
	sgptPatchText = MainMenusCreateChild(178);
	nPercent = gptComInt->pfnGetProgress();
	if (nPercent > 100)
		nPercent = 100;
	ptChild = MainMenusCreateChild(179);
	D2WinTextBoxAddColorLineUnicode(ptChild, (const WORD *)D2LangStrGetStringByIndex(5178), 0, 0);
	ptChild = MainMenusCreateChild(177);
	D2WinTextBoxAddColorLineUnicode(ptChild, (const WORD *)D2LangStrGetStringByIndex(5179), 0, 0);
	MainMenusCreateChild(183);
	D2WinImageSetCel(sgptPatchBall, D2WinImageGetCels(sgptPatchBall) * nPercent / 100);
	sprintf(szPercent, "%d%%", nPercent);
	D2WinTextBoxAddColorLineUTF(sgptPatchText, szPercent, 0);
}

void __fastcall MainMenusPatchUpdate(void)
{
	char szPercent[100];
	int nPercent;

	D2WinClearTextBox(sgptPatchText);
	nPercent = gptComInt->pfnGetProgress();
	if (nPercent > 100)
		nPercent = 100;
	D2WinImageSetCel(sgptPatchBall, D2WinImageGetCels(sgptPatchBall) * nPercent / 100);
	sprintf(szPercent, "%d%%", nPercent);
	if (sgptPatchText)
		D2WinTextBoxAddColorLineUTF(sgptPatchText, szPercent, 0);
}

void __fastcall MainMenusPatchReset(void)
{
	sgfPatchCancelled = FALSE;
}

BOOL __fastcall MainMenusPatchCancelled(void)
{
	return sgfPatchCancelled;
}

BOOL __stdcall MainMenusOnPatchCancel(_PARAMS *ptMsg)
{
	gptComInt->pfnCancelDownload();
	sgfPatchCancelled = TRUE;
	MainMenusOpen();
	return TRUE;
}

void __fastcall MainMenusOpen(void)
{
	char szVersion[32];
	D2WINCONTROL *ptBattleNet;
	D2WINCONTROL *ptOther;
	D2WINCONTROL *ptVersion;
	int nVersion;

	BNetLeave();
	gptParams->szServerIp[0] = 0;
	MainMenusBackground();
	ptBattleNet = MainMenusCreateChild(10);
	MainMenusCreateChild(9);
	ptOther = MainMenusCreateChild(11);
	if (D2WinArchiveNeedPlayDisc())
	{
		D2WinChildSetEnabled(ptBattleNet, FALSE);
		D2WinChildSetEnabled(ptOther, FALSE);
	}
	MainMenusCreateChild(12);
	MainMenusCreateChild(13);
	MainMenusCreateChild(14);
	ptVersion = MainMenusCreateChild(180);
#line 1890
	D2ASSERT(gptComInt != NULL);
	nVersion = gptComInt->pfnGetVersion();
	StormSStrPrintf(szVersion, sizeof(szVersion), "v %d.%02d", nVersion / 100 + 1, nVersion % 100);
	D2WinTextBoxAddColorLineUTF(ptVersion, szVersion, 0);
	if (D2SoundGetOptionsMusic())
		D2SoundSmpMusicSetState(1);
}

void __fastcall MainMenusToCharSel(void)
{
	D2SoundFastFadeAll();
	MainMenusClear();
	CharSelOpen();
}

static BOOL __stdcall MainMenusAccountFilter(D2WINEDITBOX *ptEdit, int nPos, char *pchKey);
static BOOL __stdcall MainMenusPasswordFilter(D2WINEDITBOX *ptEdit, int nPos, char *pchKey);

void __fastcall MainMenusOpenLogin(void)
{
	char szLast[1024];
	D2WINCONTROL *ptChild;
	const WORD *pwszAccount;
	const WORD *pwszPassword;
	BOOL fUsable;
	BOOL fConnected;
	int nAccount;
	int nPassword;

	gf1002dfb8 = TRUE;
	fConnected = gptComInt->pfnIsConnected();
	if (fConnected)
	{
		if (BNetLoggedIn())
		{
			BNetLeave();
			fConnected = FALSE;
		}
	}
	if (!fConnected)
	{
		MainMenusOpen();
		if (!BNetConnect())
			return;
	}
	sgptLoginAccount = NULL;
	sgptLoginPassword = NULL;
	sgptLoginPasswordAgain = NULL;
	sgptLoginOk = NULL;
	MainMenusBackground();
	MainMenusCreateChild(15);
	MainMenusCreateChild(16);
	ptChild = MainMenusCreateChild(18);
	D2WinTextBoxAddLineUnicode(ptChild, 	(const WORD *)D2LangStrGetStringByIndex(5224));
	ptChild = MainMenusCreateChild(19);
	D2WinTextBoxAddLineUnicode(ptChild, 	(const WORD *)D2LangStrGetStringByIndex(5225));
	ptChild = MainMenusCreateChild(21);
	D2WinTextBoxAddColorLineUnicode(ptChild, (const WORD *)D2LangStrGetStringByIndex(5204), 0, 0);
	ptChild = MainMenusCreateChild(22);
	D2WinTextBoxAddColorLineUnicode(ptChild, (const WORD *)D2LangStrGetStringByIndex(5205), 0, 0);
	sgptLoginAccount = MainMenusCreateChild(23);
	D2WinEditBoxSetMaxLength(sgptLoginAccount, 16);
	D2WinEditBoxSetKeyPostProc(sgptLoginAccount, MainMenusOnLoginChange);
	D2WinEditBoxSetKeyProc(sgptLoginAccount, MainMenusAccountFilter);
	sgptLoginPassword = MainMenusCreateChild(24);
	D2WinEditBoxSetMaxLength(sgptLoginPassword, 13);
	D2WinEditBoxSetKeyPostProc(sgptLoginPassword, MainMenusOnLoginChange);
	D2WinEditBoxSetKeyProc(sgptLoginPassword, MainMenusPasswordFilter);
	if (StormSRegLoadString("Diablo II", "Last BNet", 0, szLast, sizeof(szLast)))
	{
		D2WinEditBoxSetStringUTF(sgptLoginAccount, szLast);
		D2WinSetKeyFocus(sgptLoginPassword);
	}
	else
		D2WinSetKeyFocus(sgptLoginAccount);
	D2WinEditBoxSetTabNext(sgptLoginAccount, sgptLoginPassword);
	D2WinEditBoxSetTabNext(sgptLoginPassword, sgptLoginAccount);
	MainMenusCreateChild(25);
	sgptLoginOk = MainMenusCreateChild(26);
	MainMenusCreateChild(27);
	pwszAccount = D2WinEditBoxGetString(sgptLoginAccount);
	pwszPassword = D2WinEditBoxGetString(sgptLoginPassword);
	fUsable = FALSE;
	if (pwszAccount && pwszPassword)
	{
		nAccount = Unicode::strlen(pwszAccount);
		nPassword = Unicode::strlen(pwszPassword);
		if (nAccount >= 1 && nAccount <= 16 && nPassword >= 1 && nPassword <= 13)
			fUsable = TRUE;
	}
	if (sgptLoginOk)
		D2WinChildSetEnabled(sgptLoginOk, fUsable);
	if (D2SoundGetOptionsMusic())
		D2SoundSmpMusicSetState(1);
}

static BOOL __stdcall MainMenusAccountFilter(D2WINEDITBOX *ptEdit, int nPos, char *pchKey)
{
	char ch;

	ch = *pchKey;
	switch (ch)
	{
	case '"':
	case '#':
	case '%':
	case '*':
	case ',':
	case '/':
	case '<':
	case '>':
	case '?':
	case '\\':
		return FALSE;
	default:
		if (ch <= ' ')
			return FALSE;
		return ch < 127;
	}
}

static BOOL __stdcall MainMenusPasswordFilter(D2WINEDITBOX *ptEdit, int nPos, char *pchKey)
{
	return TRUE;
}

BOOL __stdcall MainMenusOnTcpIpTimer(_PARAMS *ptMsg)
{
	const char *pszAddress;
	BOOL fNetwork;

	pszAddress = FogSafesockGetLocalAddress();
	D2WinClearTextBox(sgptTcpIpAddress);
	if (strcmp(pszAddress, "127.0.0.1") == 0)
	{
		D2WinTextBoxAddLineUnicode(sgptTcpIpAddress, 	(const WORD *)D2LangStrGetStringByIndex(5124));
		D2WinChildSetEnabled(sgptTcpIpHost, FALSE);
		D2WinChildSetEnabled(sgptTcpIpJoin, FALSE);
		fNetwork = FALSE;
	}
	else
	{
		D2WinTextBoxAddLineUTF(sgptTcpIpAddress, pszAddress);
		D2WinChildSetEnabled(sgptTcpIpHost, TRUE);
		D2WinChildSetEnabled(sgptTcpIpJoin, TRUE);
		fNetwork = TRUE;
	}
	D2WinChildSetEnabled(sgptTcpIpLabel, fNetwork);
	return TRUE;
}

void __fastcall MainMenusOpenTcpIp(void)
{
	D2WINCONTROL *ptChild;

	gf1002dfb8 = TRUE;
	BNetLeave();
	MainMenusClear();
	MainMenusLoadPalette(gszPaletteSkyDat, gszPaletteSkyPl2);
	MainMenusCreateChild(161);
	ptChild = MainMenusCreateChild(162);
	D2WinTextBoxAddLineUnicode(ptChild, 	(const WORD *)D2LangStrGetStringByIndex(5117));
	sgptTcpIpLabel = MainMenusCreateChild(163);
	D2WinTextBoxAddLineUnicode(sgptTcpIpLabel, 	(const WORD *)D2LangStrGetStringByIndex(5121));
	sgptTcpIpHost = MainMenusCreateChild(165);
	sgptTcpIpJoin = MainMenusCreateChild(166);
	sgptTcpIpHostHelp = MainMenusCreateChild(167);
	D2WinTextBoxAddLineUnicode(sgptTcpIpHostHelp, 	(const WORD *)D2LangStrGetStringByIndex(5122));
	sgptTcpIpJoinHelp = MainMenusCreateChild(168);
	D2WinTextBoxAddLineUnicode(sgptTcpIpJoinHelp, 	(const WORD *)D2LangStrGetStringByIndex(5123));
	D2WinChildSetVisible(sgptTcpIpHostHelp, FALSE);
	D2WinChildSetVisible(sgptTcpIpJoinHelp, FALSE);
	ptChild = MainMenusCreateChild(169);
	D2WinSetEscapeButton(ptChild);
	sgptTcpIpTimer = MainMenusCreateChild(170);
	D2WinTimerDisableKey(sgptTcpIpTimer);
	D2WinTimerSetPeriodic(sgptTcpIpTimer);
	sgptTcpIpAddress = MainMenusCreateChild(164);
	MainMenusTcpIpShowAddress();
}

static void __fastcall MainMenusTcpIpShowAddress(void)
{
	const char *pszAddress;

	pszAddress = FogSafesockGetLocalAddress();
	D2WinClearTextBox(sgptTcpIpAddress);
	if (strcmp(pszAddress, "127.0.0.1") == 0)
	{
		D2WinTextBoxAddLineUnicode(sgptTcpIpAddress, 	(const WORD *)D2LangStrGetStringByIndex(5124));
		D2WinChildSetEnabled(sgptTcpIpHost, FALSE);
		D2WinChildSetEnabled(sgptTcpIpJoin, FALSE);
		D2WinChildSetEnabled(sgptTcpIpLabel, FALSE);
		return;
	}
	D2WinTextBoxAddLineUTF(sgptTcpIpAddress, pszAddress);
	D2WinChildSetEnabled(sgptTcpIpHost, TRUE);
	D2WinChildSetEnabled(sgptTcpIpJoin, TRUE);
	D2WinChildSetEnabled(sgptTcpIpLabel, TRUE);
}

BOOL __stdcall MainMenusOnHostHover(_PARAMS *ptMsg)
{
	if (ptMsg->dwLParam)
		D2WinChildSetVisible(sgptTcpIpHostHelp, TRUE);
	else
		D2WinChildSetVisible(sgptTcpIpHostHelp, FALSE);
	return TRUE;
}

BOOL __stdcall MainMenusOnJoinHover(_PARAMS *ptMsg)
{
	if (ptMsg->dwLParam)
		D2WinChildSetVisible(sgptTcpIpJoinHelp, TRUE);
	else
		D2WinChildSetVisible(sgptTcpIpJoinHelp, FALSE);
	return TRUE;
}

void __fastcall MainMenusOpenNewAccount(void)
{
	D2WINCONTROL *ptChild;
	const char *pszAccount;
	const char *pszPassword;

	MainMenusBackground();
	MainMenusCreateChild(15);
	MainMenusCreateChild(16);
	MainMenusCreateChild(17);
	ptChild = MainMenusCreateChild(18);
	D2WinTextBoxAddLineUnicode(ptChild, 	(const WORD *)D2LangStrGetStringByIndex(5224));
	ptChild = MainMenusCreateChild(19);
	D2WinTextBoxAddLineUnicode(ptChild, 	(const WORD *)D2LangStrGetStringByIndex(5225));
	ptChild = MainMenusCreateChild(20);
	D2WinTextBoxAddLineUnicode(ptChild, 	(const WORD *)D2LangStrGetStringByIndex(5226));
	ptChild = MainMenusCreateChild(21);
	D2WinTextBoxAddColorLineUnicode(ptChild, (const WORD *)D2LangStrGetStringByIndex(5222), 0, 0);
	ptChild = MainMenusCreateChild(22);
	D2WinTextBoxAddColorLineUnicode(ptChild, (const WORD *)D2LangStrGetStringByIndex(5223), 0, 0);
	sgptLoginAccount = MainMenusCreateChild(52);
	pszAccount = BNET_ACCOUNT(gptParams);
	D2WinEditBoxSetKeyProc(sgptLoginAccount, MainMenusNewAccountFilter);
	D2WinEditBoxSetMaxLength(sgptLoginAccount, 16);
	D2WinEditBoxSetStringUTF(sgptLoginAccount, pszAccount);
	pszPassword = BNET_PASSWORD(gptParams);
	sgptLoginPassword = MainMenusCreateChild(53);
	D2WinEditBoxSetKeyProc(sgptLoginPassword, MainMenusNewPasswordFilter);
	D2WinEditBoxSetMaxLength(sgptLoginPassword, 13);
	D2WinEditBoxSetStringUTF(sgptLoginPassword, pszPassword);
	sgptLoginPasswordAgain = MainMenusCreateChild(54);
	D2WinEditBoxSetMaxLength(sgptLoginPasswordAgain, 13);
	if (*pszAccount)
	{ if (*pszPassword)
		D2WinSetKeyFocus(sgptLoginPasswordAgain);
	else
		D2WinSetKeyFocus(sgptLoginPassword); }
	else D2WinSetKeyFocus(sgptLoginAccount);
	D2WinEditBoxSetTabNext(sgptLoginAccount, sgptLoginPassword);
	D2WinEditBoxSetTabNext(sgptLoginPassword, sgptLoginPasswordAgain);
	D2WinEditBoxSetTabNext(sgptLoginPasswordAgain, sgptLoginAccount);
	MainMenusCreateChild(55);
	MainMenusCreateChild(56);
}

static BOOL __stdcall MainMenusNewAccountFilter(D2WINEDITBOX *ptEdit, int nPos, char *pchKey)
{
	char ch;

	ch = *pchKey;
	switch (ch)
	{
	case '"':
	case '#':
	case '%':
	case '*':
	case ',':
	case '/':
	case '<':
	case '>':
	case '?':
	case '\\':
		return FALSE;
	default:
		if (ch <= ' ')
			return FALSE;
		return ch < 127;
	}
}

static BOOL __stdcall MainMenusNewPasswordFilter(D2WINEDITBOX *ptEdit, int nPos, char *pchKey)
{
	return TRUE;
}

static void __fastcall MainMenusSkyStill(void)
{
	D2WINCONTROL *ptChild;

	MainMenusClear();
	ptChild = MainMenusCreateChild(6);
	D2WinAnimImageSetAnimate(ptChild, 0);
	ptChild = MainMenusCreateChild(7);
	D2WinAnimImageSetAnimate(ptChild, 0);
	MainMenusCreateChild(8);
	MainMenusLoadPalette(gszPaletteSkyDat, gszPaletteSkyPl2);
}

BOOL __fastcall MainMenusOpenTerms(void)
{
	char szLanguage[128];
	char szTail[88];
	char szLine[88];
	char szPath[128];
	char achBuffer[88];
	D2WINCONTROL *ptChild;
	D2WINCONTROL *ptText;
	DWORD hFile;
	char *pchRead;
	char *pch;
	int nRead;
	int nToRead;
	int nCount;
	int nInter;
	int nLen;
	BYTE fMore;

	pchRead = achBuffer;
	nInter = 0;
	nToRead = 87;
	fMore = TRUE;
	sgfPatchCancelled = FALSE;
	MainMenusConnectBox();
	D2LangStrGetLanguageName(szLanguage, 2);
	StormSStrCopy(szPath, "tos_", sizeof(szPath));
	StormSStrAppend(szPath, szLanguage, sizeof(szPath));
	StormSStrAppend(szPath, ".txt", sizeof(szPath));
	gptComInt->pfnDownloadPatch(szPath, strlen(szPath));
	D2WinLoop(MainMenusTermsLoop);
	if (sgfPatchCancelled)
	{
		MainMenusDestroyBoxChildren();
		return FALSE;
	}
	if (!gptComInt->pfnIsConnected())
	{
		MainMenusCancelBox((const WORD *)D2LangStrGetStringByIndex(5347), MainMenusOpen);
		return FALSE;
	}
	MainMenusDestroyBoxChildren();
	MainMenusSkyStill();
	MainMenusCreateChild(45);
	ptText = MainMenusCreateChild(46);
	hFile = (DWORD)FogSafesockCreateFile(szPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN,
		0);
	if (hFile == (DWORD)-1)
	{
		MainMenusCancelBox((const WORD *)D2LangStrGetStringByIndex(5347), MainMenusOpen);
		return FALSE;
	}
	while (fMore)
	{
		FogSafesockReadFile(hFile, pchRead, nToRead, &nRead, 0);
		if (nToRead != nRead)
			fMore = FALSE;
		nCount = nInter + nRead;
		nRead = 0;
		pch = achBuffer;
		while (*pch > 13 && nRead < nCount - 1)
		{
			pch++;
			nRead++;
		}
		nLen = nRead;
		if (nRead == nCount - 1)
		{
			pch = &achBuffer[nRead];
			while (*pch != ' ' && *pch != '\n' && *pch != '\r')
			{
				if (nRead <= 1)
					break;
				pch--;
				nRead--;
			}
			if (nRead == 1)
				nRead = nCount;
			nRead++;
			nLen = nRead;
		}
		else
		{
			while (*pch < 14 && nRead < nCount)
			{
				*pch++ = ' ';
				nRead++;
			}
			pch--;
		}
		strncpy(szLine, achBuffer, nLen);
		szLine[nLen] = 0;
		D2WinTextBoxAddColorLineUTF(ptText, szLine, 0);
		nInter = nCount - nRead;
#line 2464
		D2ASSERT(nInter >= 0);
		if (nInter >= 1)
		{
			strncpy(szTail, pch + 1, nInter);
			strncpy(achBuffer, szTail, nInter);
			pchRead = &achBuffer[nInter];
		}
		else
			pchRead = achBuffer;
		nToRead = nRead;
	}
	FogSafesockCloseHandle(hFile);
	FogSafesockDeleteFile(szPath);
	D2WinTextBoxSetTop(ptText, 0);
	MainMenusCreateChild(47);
	MainMenusCreateChild(48);
	ptChild = MainMenusCreateChild(49);
	D2WinTextBoxAddColorLineUnicode(ptChild, (const WORD *)D2LangStrGetStringByIndex(5180), 0, 0);
	MainMenusCreateChild(50);
	MainMenusCreateChild(51);
	return TRUE;
}

BOOL __stdcall MainMenusTermsLoop(DWORD dwArg)
{
	if (gptComInt->pfnGetProgress() == 101 || sgfPatchCancelled)
		D2WinQuitLoop();
	return TRUE;
}

BOOL __stdcall MainMenusOnTermsAgree(_PARAMS *ptMsg)
{
	MainMenusOpenNewAccount();
	return TRUE;
}

BOOL __stdcall MainMenusOnTermsUp(_PARAMS *ptMsg)
{
	D2WINCONTROL *hChild;
	int nTop;
	int i;

	hChild = NULL;
	for (i = 0; i <= sgnNumChildren; i++)
	{
		hChild = sgaptChildren[i];
		if ((DWORD)D2WinDereference(hChild) == 4)
			break;
	}
#line 2538
	D2ASSERT(hChild != NULL);
	nTop = D2WinTextBoxGetTop(hChild);
	if (nTop > 0)
		D2WinTextBoxSetTop(hChild, nTop - 1);
	return TRUE;
}

BOOL __stdcall MainMenusOnTermsDown(_PARAMS *ptMsg)
{
	D2WINCONTROL *hChild;
	int i;

	hChild = NULL;
	for (i = 0; i <= sgnNumChildren; i++)
	{
		hChild = sgaptChildren[i];
		if ((DWORD)D2WinDereference(hChild) == 4)
			break;
	}
#line 2561
	D2ASSERT(hChild != NULL);
	DWORD dwTop = D2WinTextBoxGetTop(hChild);
	D2WinTextBoxSetTop(hChild, (DWORD)dwTop + 1);
	return TRUE;
}

void __fastcall MainMenusOpenClassSelectOld(void)
{
	D2WINCONTROL *ptChild;

	MainMenusClear();
	MainMenusLoadPalette(gszPaletteCharDat, gszPaletteCharPl2);
	MainMenusCreateBoxChild(90);
	ptChild = MainMenusCreateBoxChild(87);
	D2WinTextBoxAddColorLineUnicode(ptChild, (const WORD *)D2LangStrGetStringByIndex(5127), 0, 0);
	MainMenusCreateBoxChild(94);
	MainMenusCreateBoxChild(93);
	MainMenusCreateBoxChild(92);
	MainMenusCreateBoxChild(95);
	MainMenusCreateBoxChild(96);
	MainMenusCreateBoxChild(91);
}

void __fastcall MainMenusOpenHardcore(void)
{
	D2WINCONTROL *ptChild;

	MainMenusDestroyBoxChildren();
	MainMenusOpenClassSelectOld();
	ptChild = MainMenusCreateBoxChild(104);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(105);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(106);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(107);
	D2WinChildSetOnTop(ptChild, TRUE);
	D2WinTextBoxAddColorLineUnicode(ptChild, (const WORD *)D2LangStrGetStringByIndex(5303), 0, 0);
}

BOOL __stdcall MainMenusOnHardcoreOk(_PARAMS *ptMsg)
{
	if (geMode == LAUNCHMODE_BATTLENET)
	{
		MainMenusCreateOnRealm();
		return TRUE;
	}
	if (geMode == LAUNCHMODE_OPENBATTLENET)
	{
		D2MCPClientD2MCPClose();
		SaveLoadSetExtraFlag(FALSE);
		SaveLoadNew(gptParams->szName, gptParams->bClass, gptParams->abSummary, gptParams->wCharFlags);
		SaveLoadWrite(gptParams->szName, NULL);
		gptParams->bConnectMode = 4;
		sgnResult = 3;
		gptParams->dwGameFlags = 2052;
		gptParams->szRealm[0] = 0;
		D2WinQuitLoop();
		return TRUE;
	}
	if (geMode == LAUNCHMODE_SINGLE)
	{
		SaveLoadNew(gptParams->szName, gptParams->bClass, gptParams->abSummary, gptParams->wCharFlags);
		SaveLoadWrite(gptParams->szName, NULL);
		gptParams->bConnectMode = 1;
		gptParams->dwGameType = 0;
		sgnResult = 1;
		gptParams->dwGameFlags = 4;
		if (gptParams->wCharFlags & 4)
			gptParams->dwGameFlags = 2052;
		D2WinQuitLoop();
		return TRUE;
	}
	if (geMode == LAUNCHMODE_TCPIP)
	{
		SaveLoadSetExtraFlag(FALSE);
		SaveLoadNew(gptParams->szName, gptParams->bClass, gptParams->abSummary, gptParams->wCharFlags);
		SaveLoadWrite(gptParams->szName, NULL);
		gptParams->bConnectMode = 3;
		sgnResult = 1;
		gptParams->dwGameFlags = 4;
		if (gptParams->wCharFlags & 4)
			gptParams->dwGameFlags = 2052;
		D2WinQuitLoop();
	}
	return TRUE;
}

BOOL __stdcall MainMenusOnHardcoreCancel(_PARAMS *ptMsg)
{
	MainMenusDestroyBoxChildren();
	MainMenusOpenClassSelect();
	return TRUE;
}

static void __fastcall MainMenusNewChar(void)
{
	char szName[256];

	D2WinChildSetEnabled(sgptNewCharOk, FALSE);
	if (MainMenusSaveExists() == TRUE && geMode != LAUNCHMODE_BATTLENET)
	{
		MainMenusMessageBox((const WORD *)D2LangStrGetStringByIndex(5165), MainMenusDestroyBoxChildren);
		return;
	}
	Unicode::toUtf(szName, D2WinEditBoxGetString(sgptNameEdit), 256);
	sprintf(gptParams->szName, "%s", szName);
	MainMenusClear();
	if (gptParams->wCharFlags & 4)
	{
		MainMenusOpenHardcore();
		return;
	}
	if (geMode == LAUNCHMODE_SINGLE)
	{
		SaveLoadNew(gptParams->szName, gptParams->bClass, gptParams->abSummary, gptParams->wCharFlags);
		SaveLoadWrite(gptParams->szName, NULL);
		gptParams->bConnectMode = 1;
		gptParams->dwGameType = 0;
		sgnResult = 1;
		gptParams->dwGameFlags = 4;
		if (gptParams->wCharFlags & 4)
			gptParams->dwGameFlags = 2052;
		D2WinQuitLoop();
		return;
	}
	if (geMode == LAUNCHMODE_TCPIP)
	{
		SaveLoadSetExtraFlag(FALSE);
		SaveLoadNew(gptParams->szName, gptParams->bClass, gptParams->abSummary, gptParams->wCharFlags);
		SaveLoadWrite(gptParams->szName, NULL);
		gptParams->bConnectMode = 3;
		sgnResult = 1;
		gptParams->dwGameFlags = 4;
		if (gptParams->wCharFlags & 4)
			gptParams->dwGameFlags = 2052;
		D2WinQuitLoop();
		return;
	}
	if (geMode == LAUNCHMODE_OPENBATTLENET)
	{
		D2MCPClientD2MCPClose();
		SaveLoadSetExtraFlag(FALSE);
		SaveLoadNew(gptParams->szName, gptParams->bClass, gptParams->abSummary, gptParams->wCharFlags);
		SaveLoadWrite(gptParams->szName, NULL);
		gptParams->bConnectMode = 4;
		gptParams->szRealm[0] = 0;
		sgnResult = 3;
		gptParams->dwGameFlags = 4;
		if (gptParams->wCharFlags & 4)
			gptParams->dwGameFlags = 2052;
		D2WinQuitLoop();
		return;
	}
	MainMenusCreateOnRealm();
}

BOOL __stdcall MainMenusOnNewCharOk(_PARAMS *ptMsg)
{
	char szPath[1024];
	DWORD dwSectorsPerCluster;
	DWORD dwBytesPerSector;
	DWORD dwFreeClusters;
	DWORD dwClusters;
	DWORD i;

	if (D2WinChildGetEnabled(sgptNewCharOk))
	{
		FogSafesockGetSavePath(szPath, 1024);
		for (i = 0; i < 1024; i++)
		{
			if (szPath[i] == '\\')
			{
				szPath[i + 1] = 0;
				break;
			}
		}
		dwSectorsPerCluster = 0;
		dwBytesPerSector = 0;
		dwFreeClusters = 0;
		dwClusters = 0;
		GetDiskFreeSpaceA(szPath, &dwSectorsPerCluster, &dwBytesPerSector, &dwFreeClusters, &dwClusters);
		if (dwBytesPerSector * dwFreeClusters * dwSectorsPerCluster < 5000)
		{
			MainMenusMessageBox((const WORD *)D2LangStrGetStringByIndex(5149), MainMenusToCharSel);
			return TRUE;
		}
		MainMenusNewChar();
	}
	return TRUE;
}

void __fastcall MainMenusOpenClassSelect(void)
{
	D2WINCONTROL *ptChild;

	MainMenusLoadClassCels();
	MainMenusClear();
	sgnClassHover = LAUNCHCLASS_NONE;
	sgnClass = LAUNCHCLASS_NONE;
	gptParams->wCharFlags = 0;
	MainMenusLoadPalette(gszPaletteCharDat, gszPaletteCharPl2);
	MainMenusCreateChild(90);
	sgptClassName = MainMenusCreateChild(97);
	sgptClassText = MainMenusCreateChild(98);
	ptChild = MainMenusCreateChild(87);
	D2WinTextBoxAddColorLineUnicode(ptChild, (const WORD *)D2LangStrGetStringByIndex(5127), 0, 0);
	ptChild = MainMenusCreateChild(88);
	D2WinSetEscapeButton(ptChild);
	sgptNameBox = MainMenusCreateChild(99);
	sgptNameLabel = MainMenusCreateChild(100);
	D2WinTextBoxAddColorLineUnicode(sgptNameLabel, (const WORD *)D2LangStrGetStringByIndex(5125), 4, 0);
	sgptHardcoreLabel = MainMenusCreateChild(101);
	D2WinTextBoxAddColorLineUnicode(sgptHardcoreLabel, (const WORD *)D2LangStrGetStringByIndex(5126), 4, 0);
	sgptHardcoreBox = MainMenusCreateChild(102);
	sgptNameEdit = MainMenusCreateChild(103);
	D2WinEditBoxSetMaxLength(sgptNameEdit, 16);
	D2WinEditBoxSetKeyProc(sgptNameEdit, MainMenusNameFilter);
	D2WinEditBoxSetKeyPostProc(sgptNameEdit, MainMenusOnNameChange);
	sgptNewCharOk = MainMenusCreateChild(89);
	if (!gfHardcoreAvailable)
	{
		D2WinChildSetEnabled(sgptHardcoreBox, FALSE);
		D2WinChildSetEnabled(sgptHardcoreLabel, FALSE);
	}
	D2WinChildSetVisible(sgptNameBox, FALSE);
	D2WinChildSetVisible(sgptNameLabel, FALSE);
	D2WinChildSetVisible(sgptHardcoreLabel, FALSE);
	D2WinChildSetVisible(sgptHardcoreBox, FALSE);
	D2WinChildSetVisible(sgptNameEdit, FALSE);
	D2WinChildSetVisible(sgptNewCharOk, FALSE);
	sgaptHero[0] = MainMenusCreateChild(94);
	sgaptHero[1] = MainMenusCreateChild(93);
	sgaptHero[2] = MainMenusCreateChild(92);
	sgaptHero[3] = MainMenusCreateChild(95);
	sgaptHero[4] = MainMenusCreateChild(96);
	MainMenusCreateChild(91);
	MainMenusNameCheck();
}

static BOOL __stdcall MainMenusNameFilter(D2WINEDITBOX *ptEdit, int nPos, char *pchKey)
{
	char szName[32];
	int i;
	char ch;

	ch = *pchKey;
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
		return TRUE;
	if (ch != '-' && ch != '_')
		return FALSE;
	if (nPos)
	{
		DWORD dwEdit = (DWORD)D2WinEditBoxGetString(ptEdit);
		(void)D2LangUnicodetoUtf(szName, dwEdit, 32);
		for (i = 0; szName[i]; i++)
		{
			if (szName[i] == '-' || szName[i] == '_')
				return FALSE; }
		return TRUE; }
	return FALSE;
}

static BOOL __stdcall MainMenusOnNameChange(DWORD dwZero)
{
	char szName[32];
	const WORD *pwszName;
	BOOL fUsable;
	int nLength;

	pwszName = D2WinEditBoxGetString(sgptNameEdit);
	fUsable = FALSE;
	if (pwszName)
	{
		nLength = Unicode::strlen(pwszName);
		(void)D2LangUnicodetoUtf(szName, pwszName, 32);
		if (nLength >= 2 && nLength < 16 && szName[nLength - 1] != '_' && szName[0] != '_'
			&& szName[nLength - 1] != '-' && szName[0] != '-')
			fUsable = TRUE;
	}
	D2WinChildSetEnabled(sgptNewCharOk, fUsable);
	return TRUE;
}

static void __fastcall MainMenusNameCheck(void)
{
	char szName[32];
	const WORD *pwszName;
	BOOL fUsable;
	int nLength;

	pwszName = D2WinEditBoxGetString(sgptNameEdit);
	fUsable = FALSE;
	if (pwszName)
	{
		nLength = Unicode::strlen(pwszName);
		(void)D2LangUnicodetoUtf(szName, pwszName, 32);
		if (nLength >= 2 && nLength < 16 && szName[nLength - 1] != '_' && szName[0] != '_'
			&& szName[nLength - 1] != '-' && szName[0] != '-')
			fUsable = TRUE;
	}
	D2WinChildSetEnabled(sgptNewCharOk, fUsable);
}

static BOOL __stdcall MainMenusOnLoginChange(DWORD dwZero)
{
	const WORD *pwszAccount;
	const WORD *pwszPassword;
	BOOL fUsable;
	int nAccount;
	int nPassword;

	pwszAccount = D2WinEditBoxGetString(sgptLoginAccount);
	pwszPassword = D2WinEditBoxGetString(sgptLoginPassword);
	fUsable = FALSE;
	if (pwszAccount && pwszPassword)
	{
		nAccount = Unicode::strlen(pwszAccount);
		nPassword = Unicode::strlen(pwszPassword);
		if (nAccount >= 1 && nAccount <= 16 && nPassword >= 1 && nPassword <= 13)
			fUsable = TRUE;
	}
	if (sgptLoginOk)
		D2WinChildSetEnabled(sgptLoginOk, fUsable);
	return TRUE;
}

BOOL __stdcall MainMenusOnHardcoreBox(_PARAMS *ptMsg)
{
	gptParams->wCharFlags ^= 4;
	return TRUE;
}

BOOL __stdcall MainMenusOnHero(_PARAMS *ptMsg)
{
	int eState;
	int nPrevious;
	int nClass;
	BYTE bClass;
	int i;

	eState = D2WinAnimImageGetState(ptMsg->window);
	for (i = 0; i < 5; i++)
	{
		if (!D2WinAnimImageDeselect(sgaptHero[i]))
			return FALSE;
	}
	nPrevious = sgnClass;
	nClass = LAUNCHCLASS_NONE;
	sgnClass = LAUNCHCLASS_NONE;
	if (nPrevious != LAUNCHCLASS_NONE)
	{
		D2SoundFastFadeAll();
		switch (nPrevious)
		{
		case LAUNCHCLASS_AMAZON:
			D2SoundFastPlay("data\\global\\sfx\\cursor\\intro\\amazon deselect.wav");
			break;
		case LAUNCHCLASS_BARBARIAN:
			D2SoundFastPlay("data\\global\\sfx\\cursor\\intro\\barbarian deselect.wav");
			break;
		case LAUNCHCLASS_PALADIN:
			D2SoundFastPlay("data\\global\\sfx\\cursor\\intro\\paladin deselect.wav");
			break;
		case LAUNCHCLASS_NECROMANCER:
			D2SoundFastPlay("data\\global\\sfx\\cursor\\intro\\necromancer deselect.wav");
			break;
		case LAUNCHCLASS_SORCERESS:
			D2SoundFastPlay("data\\global\\sfx\\cursor\\intro\\sorceress deselect.wav");
			break;
		}
	}
	if (ptMsg->window == sgaptHero[0])
	{
		nClass = LAUNCHCLASS_BARBARIAN;
		if (sgnClass == LAUNCHCLASS_NONE)
			MainMenusClassDescribe(ptMsg->dwLParam, nClass, 4007, 5130);
	}
	else if (ptMsg->window == sgaptHero[1])
	{
		nClass = LAUNCHCLASS_NECROMANCER;
		if (sgnClass == LAUNCHCLASS_NONE)
			MainMenusClassDescribe(ptMsg->dwLParam, nClass, 4009, 5129);
	}
	else if (ptMsg->window == sgaptHero[2])
	{
		nClass = LAUNCHCLASS_AMAZON;
		if (sgnClass == LAUNCHCLASS_NONE)
			MainMenusClassDescribe(ptMsg->dwLParam, nClass, 4011, 5128);
	}
	else if (ptMsg->window == sgaptHero[3])
	{
		nClass = LAUNCHCLASS_SORCERESS;
		if (sgnClass == LAUNCHCLASS_NONE)
			MainMenusClassDescribe(ptMsg->dwLParam, nClass, 4010, 5131);
	}
	else if (ptMsg->window == sgaptHero[4])
	{
		nClass = LAUNCHCLASS_PALADIN;
		if (sgnClass == LAUNCHCLASS_NONE)
			MainMenusClassDescribe(ptMsg->dwLParam, nClass, 4008, 5132);
	}
	if (nClass != nPrevious && eState == ANIMSTATE_OVER)
	{
		switch (nClass)
		{
		case LAUNCHCLASS_AMAZON:
			D2SoundFastPlay("data\\global\\sfx\\cursor\\intro\\amazon select.wav");
			break;
		case LAUNCHCLASS_BARBARIAN:
			D2SoundFastPlay("data\\global\\sfx\\cursor\\intro\\barbarian select.wav");
			break;
		case LAUNCHCLASS_PALADIN:
			D2SoundFastPlay("data\\global\\sfx\\cursor\\intro\\paladin select.wav");
			break;
		case LAUNCHCLASS_NECROMANCER:
			D2SoundFastPlay("data\\global\\sfx\\cursor\\intro\\necromancer select.wav");
			break;
		case LAUNCHCLASS_SORCERESS:
			D2SoundFastPlay("data\\global\\sfx\\cursor\\intro\\sorceress select.wav");
			break;
		}
	}
	if (nClass != nPrevious && eState != ANIMSTATE_SELECTING && eState != ANIMSTATE_SELECTED
		&& eState != ANIMSTATE_DESELECTING)
		sgnClass = nClass;
	bClass = (BYTE)sgnClass;
	gptParams->bClass = bClass;
	gptParams->bAmazon = 0;
	gptParams->bBarbarian = 0;
	gptParams->bNecromancer = 0;
	gptParams->bPaladin = 0;
	gptParams->bSorceress = 0;
	if (bClass == LAUNCHCLASS_AMAZON)
		gptParams->bAmazon = 1;
	else if (bClass == LAUNCHCLASS_BARBARIAN)
		gptParams->bBarbarian = 1;
	else if (bClass == LAUNCHCLASS_NECROMANCER)
		gptParams->bNecromancer = 1;
	else if (bClass == LAUNCHCLASS_PALADIN)
		gptParams->bPaladin = 1;
	else if (bClass == LAUNCHCLASS_SORCERESS)
		gptParams->bSorceress = bClass;
	if (sgnClass == LAUNCHCLASS_NONE && eState != ANIMSTATE_OVER)
	{
		D2WinChildSetVisible(sgptNameBox, FALSE);
		D2WinChildSetVisible(sgptNameLabel, FALSE);
		D2WinChildSetVisible(sgptHardcoreLabel, FALSE);
		D2WinChildSetVisible(sgptHardcoreBox, FALSE);
		D2WinChildSetVisible(sgptNameEdit, FALSE);
		D2WinChildSetVisible(sgptNewCharOk, FALSE);
		D2WinSetKeyFocus(0);
	}
	else
	{
		D2WinChildSetVisible(sgptNameBox, TRUE);
		D2WinChildSetVisible(sgptNameLabel, TRUE);
		if (gfHardcoreAvailable)
		{
			D2WinChildSetVisible(sgptHardcoreLabel, TRUE);
			D2WinChildSetVisible(sgptHardcoreBox, TRUE);
		}
		D2WinChildSetVisible(sgptNameEdit, TRUE);
		D2WinSetKeyFocus(sgptNameEdit);
		D2WinChildSetVisible(sgptNewCharOk, TRUE);
	}
	return TRUE;
}

BOOL __fastcall MainMenusClassDescribe(BOOL fOver, int nClass, int nNameString, int nTextString)
{
	if (sgnClass != LAUNCHCLASS_NONE)
		return TRUE;
	if (fOver)
	{
		D2WinClearTextBox(sgptClassName);
		D2WinTextBoxAddColorLineUnicode(sgptClassName, (const WORD *)D2LangStrGetStringByIndex(nNameString),
			0, 0);
		D2WinClearTextBox(sgptClassText);
		D2WinTextBoxAddColorLineUnicode(sgptClassText, (const WORD *)D2LangStrGetStringByIndex(nTextString),
			0, 0);
		sgnClassHover = nClass;
		return TRUE;
	}
	if (sgnClassHover == nClass)
	{
		D2WinClearTextBox(sgptClassName);
		D2WinClearTextBox(sgptClassText);
	}
	return TRUE;
}

BOOL __stdcall MainMenusOnHoverSorceress(_PARAMS *ptMsg)
{
	return MainMenusClassDescribe(ptMsg->dwLParam, LAUNCHCLASS_SORCERESS, 4010, 5131);
}

BOOL __stdcall MainMenusOnHoverAmazon(_PARAMS *ptMsg)
{
	return MainMenusClassDescribe(ptMsg->dwLParam, LAUNCHCLASS_AMAZON, 4011, 5128);
}

BOOL __stdcall MainMenusOnHoverNecromancer(_PARAMS *ptMsg)
{
	return MainMenusClassDescribe(ptMsg->dwLParam, LAUNCHCLASS_NECROMANCER, 4009, 5129);
}

BOOL __stdcall MainMenusOnHoverBarbarian(_PARAMS *ptMsg)
{
	return MainMenusClassDescribe(ptMsg->dwLParam, LAUNCHCLASS_BARBARIAN, 4007, 5130);
}

BOOL __stdcall MainMenusOnHoverPaladin(_PARAMS *ptMsg)
{
	return MainMenusClassDescribe(ptMsg->dwLParam, LAUNCHCLASS_PALADIN, 4008, 5132);
}

void __cdecl MainMenusQuit(void)
{
	sgnResult = 0;
}

static __inline void MainMenusAddShard(D2LAUNCHSHARD *ptShard)
{
	int i;

#line 3380
	D2ASSERT(sgptShardArray);
	for (i = 0; i < sgnShardListSize; i++)
	{
		if (!sgptShardArray[i])
		{
			sgptShardArray[i] = ptShard;
			break;
		}
	}
}

BOOL __fastcall MainMenusFreeShards(void)
{
	int i;

	if (sgptShardArray)
	{
		for (i = 0; i < sgnShardListSize; i++)
		{
			if (sgptShardArray[i])
			{
#line 3424
				D2ASSERT(sgptShardArray[i]->pszDescription);
				D2FREE2(sgptShardArray[i]->pszDescription);
#line 3426
				D2ASSERT(sgptShardArray[i]->pszName);
				D2FREE2(sgptShardArray[i]->pszName);
#line 3429
				D2FREE2(sgptShardArray[i]);
			}
		}
#line 3432
		D2FREE2(sgptShardArray);
		sgptShardArray = NULL;
	}
	return TRUE;
}

int __cdecl MainMenusCompareShards(const void *pvFirst, const void *pvSecond)
{
	return strcmp((*(D2LAUNCHSHARD * const *)pvFirst)->pszName,
		(*(D2LAUNCHSHARD * const *)pvSecond)->pszName);
}

void __fastcall MainMenusCreateOnRealm(void)
{
	Unicode awszUnused[256];
	D2WINCONTROL *ptChild;
	int i;

	MainMenusBackground();
	if (!gptComInt->pfnIsConnected())
	{
		MainMenusCancelBox((const WORD *)D2LangStrGetStringByIndex(5347), MainMenusOpen);
		return;
	}
	sgnShardSelected = -1;
	MainMenusCreateChild(144);
	ptChild = MainMenusCreateChild(145);
	D2WinTextBoxAddLineUnicode(ptChild, D2LangStrGetStringByIndex(5289));
	ptChild = MainMenusCreateChild(146);
	D2WinTextBoxAddLineUnicode(ptChild, D2LangStrGetStringByIndex(5292));
	sgptRealmEdit = MainMenusCreateChild(147);
	D2WinEditBoxSetMaxLength(sgptRealmEdit, 32);
	D2WinEditBoxSetKeyPostProc(sgptRealmEdit, MainMenusOnRealmChange);
	D2WinSetKeyFocus(sgptRealmEdit);
	ptChild = MainMenusCreateChild(154);
	D2WinTextBoxAddColorLineUnicode(ptChild, D2LangStrGetStringByIndex(5291), 0, 0);
	ptChild = MainMenusCreateChild(155);
	D2WinTextBoxAddLineUnicode(ptChild, D2LangStrGetStringByIndex(5293));
	ptChild = MainMenusCreateChild(156);
	D2WinTextBoxAddColorLineUnicode(ptChild, D2LangStrGetStringByIndex(5276), 1, 0);
	D2WinTextBoxSetLineSpacing(ptChild, -8, 0);
	ptChild = MainMenusCreateChild(157);
	D2WinTextBoxAddColorLineUnicode(ptChild, D2LangStrGetStringByIndex(5277), 1, 0);
	D2WinTextBoxSetLineSpacing(ptChild, -8, 0);
	sgptShardList = MainMenusCreateChild(148);
	ptChild = MainMenusCreateChild(152);
	D2WinSetEscapeButton(ptChild);
	MainMenusCreateChild(153);
	ptChild = MainMenusCreateChild(149);
	D2WinTextBoxAddLineUnicode(ptChild, D2LangStrGetStringByIndex(5300));
	ptChild = MainMenusCreateChild(150);
	D2WinTextBoxAddLineUnicode(ptChild, D2LangStrGetStringByIndex(5297));
	sgptShardText = MainMenusCreateChild(151);
	MainMenusLoadShards();
	if (sgnShardListSize == -1)
	{
		MainMenusCancelBox((const WORD *)D2LangStrGetStringByIndex(5347), MainMenusOpen);
		return;
	}
	if (!sgnShardListSize)
	{
		MainMenusCancelBox((const WORD *)D2LangStrGetStringByIndex(5290), MainMenusToCharSel);
		return;
	}
	for (i = 0; i < sgnShardListSize; i++)
		if (sgptShardArray[i])
			D2WinTextBoxAddLineUTF(sgptShardList, sgptShardArray[i]->pszName);
	D2WinTextBoxSetTop(sgptShardList, 0);
}

static void __fastcall MainMenusLoadShards(void)
{
	const D2LAUNCHSHARDINFO *ptInfo;
	D2LAUNCHSHARD *ptShard;
	const char *pb;
	int nSize;
	int i;

	MainMenusWaitBox();
	D2WinLoop(MainMenusLoopOnce);
	if (gptComInt)
		sgptRealmList = gptComInt->pfnRequestRealmList(NULL);
	MainMenusDestroyBoxChildren();
	if (!sgptRealmList)
	{
		sgnShardListSize = -1;
		return;
	}
	sgnShardListSize = sgptRealmList->nCount;
#line 3489
	D2ASSERT(sgnShardListSize < 512);
	pb = sgptRealmList->abEntries;
	MainMenusFreeShards();
	if (!sgnShardListSize)
		return;
	nSize = sgnShardListSize * sizeof(D2LAUNCHSHARD *);
#line 3501
	sgptShardArray = (D2LAUNCHSHARD **)D2ALLOC2(nSize);
	memset(sgptShardArray, 0, nSize);
	for (i = 0; i < sgnShardListSize; i++)
	{
#line 3507
		ptShard = (D2LAUNCHSHARD *)D2ALLOC2(sizeof(D2LAUNCHSHARD));
		ptInfo = (const D2LAUNCHSHARDINFO *)pb;
		pb += sizeof(D2LAUNCHSHARDINFO);
		ptShard->tInfo.adwInfo[1] = ptInfo->adwInfo[1];
		ptShard->tInfo.adwInfo[0] = ptInfo->adwInfo[0];
		ptShard->tInfo.adwInfo[4] = ptInfo->adwInfo[4];
		ptShard->tInfo.adwInfo[2] = ptInfo->adwInfo[2];
		ptShard->tInfo.adwInfo[3] = ptInfo->adwInfo[3];
		ptShard->tInfo.adwInfo[5] = ptInfo->adwInfo[5];
		ptShard->tInfo.adwInfo[6] = ptInfo->adwInfo[6];
		nSize = strlen(pb) + 1;
#line 3521
		ptShard->pszName = (char *)D2ALLOC2(nSize);
		StormSStrCopy(ptShard->pszName, pb, nSize);
		pb += nSize;
		nSize = strlen(pb) + 1;
#line 3527
		ptShard->pszDescription = (char *)D2ALLOC2(nSize);
		StormSStrCopy(ptShard->pszDescription, pb, nSize);
		pb += nSize;
		MainMenusAddShard(ptShard);
	}
	qsort(sgptShardArray, sgnShardListSize, sizeof(D2LAUNCHSHARD *), MainMenusCompareShards);
}

BOOL __stdcall MainMenusLoop(DWORD dwArg)
{
	return TRUE;
}

static void __fastcall MainMenusLoadCels(void)
{
	char szPath[260];

	wsprintfA(szPath, "%s\\UI\\FrontEnd\\blizno", "DATA\\GLOBAL");
	sghCelBlizno = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\UI\\FrontEnd\\trademark", "DATA\\GLOBAL");
	sghCelTrademark = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\UI\\CURSOR\\ohand", "DATA\\GLOBAL");
	sghCelCursor = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\Diablo2", "DATA\\GLOBAL");
	sghCelDiablo2 = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\D2logoBlackLeft", "DATA\\GLOBAL");
	sghCelLogoBlackLeft = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\D2logoFireLeft", "DATA\\GLOBAL");
	sghCelLogoFireLeft = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\D2logoBlackRight", "DATA\\GLOBAL");
	sghCelLogoBlackRight = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\D2logoFireRight", "DATA\\GLOBAL");
	sghCelLogoFireRight = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\TitleScreen", "DATA\\GLOBAL");
	sghCelTitleScreen = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\CharSelect\\creditsbckg", "DATA\\GLOBAL");
	sghCelCreditsBckg = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\WideButtonBlank", "DATA\\GLOBAL");
	sghCelWideButton = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\MediumButtonBlank", "DATA\\GLOBAL");
	sghCelMediumButton = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\CharSelect\\ShortButtonBlank", "DATA\\GLOBAL");
	sghCelShortButton = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\textbox2", "DATA\\GLOBAL");
	sghCelTextBox2 = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\TermsofUsebckg", "DATA\\GLOBAL");
	sghCelTermsOfUseBckg = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\CharSelect\\charselectbckg", "DATA\\GLOBAL");
	ghCelCharSelectBckg = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\CharSelect\\DifficultyLevels", "DATA\\GLOBAL");
	sghCelDifficultyLevels = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\CinematicsSelection", "DATA\\GLOBAL");
	sghCelCinematicsSelection = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\CharSelect\\charselectbox", "DATA\\GLOBAL");
	sghCelCharSelectBox = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\CharSelect\\charselectboxgrey", "DATA\\GLOBAL");
	sghCelCharSelectBoxGrey = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\CharSelect\\CreateNewCharImage", "DATA\\GLOBAL");
	sghCelCreateNewChar = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\PopUpOK", "DATA\\GLOBAL");
	sghCelPopUpOk = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\PopUp_340x224", "DATA\\GLOBAL");
	sghCelPopUp340x224 = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\PopUpWide", "DATA\\GLOBAL");
	sghCelPopUpWide = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\PopUpLargest", "DATA\\GLOBAL");
	sghCelPopUpLargest = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\PopUpOK2", "DATA\\GLOBAL");
	sghCelPopUpOk2 = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\PopUpOKCancel", "DATA\\GLOBAL");
	sghCelPopUpOkCancel = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\PopUpOKCancel2", "DATA\\GLOBAL");
	sghCelPopUpOkCancel2 = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\OkCancelButtonBlank", "DATA\\GLOBAL");
	sghCelOkCancelButton = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\CancelButtonBlank", "DATA\\GLOBAL");
	sghCelCancelButton = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\CharSelect\\selectchararrows", "DATA\\GLOBAL");
	sghCelSelectCharArrows = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\CharSelect\\password", "DATA\\GLOBAL");
	sghCelPassword = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\UI\\FrontEnd\\realmbckg", "DATA\\GLOBAL");
	sghCelRealmBckg = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	sprintf(szPath, "%s\\UI\\FRONTEND\\joingamescrollbars", "DATA\\GLOBAL");
	sghCelJoinGameScrollbars = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\UI\\FrontEnd\\tcpipbckg", "DATA\\GLOBAL");
	sghCelTcpIpBckg = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\UI\\FrontEnd\\IPAddressBox", "DATA\\GLOBAL");
	sghCelIpAddressBox = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\UI\\FrontEnd\\downloadpatchbckg", "DATA\\GLOBAL");
	sghCelDownloadPatchBckg = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\UI\\FrontEnd\\downloadpatchball2", "DATA\\GLOBAL");
	sghCelDownloadPatchBall = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	D2WinSetCursor(sghCelCursor);
	if (D2MCPClientD2MCPIsConnected())
		D2MCPClientD2MCPClose();
}

int __fastcall MainMenusMain(INIT_PARAMS *ptParams)
{
	BOOL fCharacters;
	BYTE bFirst;

	gptParams = ptParams;
	D2WinCreateParent();
	if (!D2LangStrNewTableArchive(D2WinArchiveGetArchive()))
	{
#line 3676
		D2ASSERT(0);
	}
	MainMenusLoadCels();
	gptParams->abChannel[0] = 0;
	gptParams->abFirstChannel[0] = 0;
	gptParams->szGameName[0] = 0;
	gptParams->bGamePassword = 0;
	if (geMode != LAUNCHMODE_TCPIP)
		gptParams->szServerIp[0] = 0;
	D2SoundFastInit();
	D2SoundFastLoad("data\\global\\sfx\\cursor\\button.wav");
	D2SoundFastLoad("data\\global\\sfx\\cursor\\intro\\amazon select.wav");
	D2SoundFastLoad("data\\global\\sfx\\cursor\\intro\\amazon deselect.wav");
	D2SoundFastLoad("data\\global\\sfx\\cursor\\intro\\barbarian select.wav");
	D2SoundFastLoad("data\\global\\sfx\\cursor\\intro\\barbarian deselect.wav");
	D2SoundFastLoad("data\\global\\sfx\\cursor\\intro\\paladin select.wav");
	D2SoundFastLoad("data\\global\\sfx\\cursor\\intro\\paladin deselect.wav");
	D2SoundFastLoad("data\\global\\sfx\\cursor\\intro\\necromancer select.wav");
	D2SoundFastLoad("data\\global\\sfx\\cursor\\intro\\necromancer deselect.wav");
	D2SoundFastLoad("data\\global\\sfx\\cursor\\intro\\sorceress select.wav");
	D2SoundFastLoad("data\\global\\sfx\\cursor\\intro\\sorceress deselect.wav");
	gptComInt = (D2COMINT *)ptParams->dwComInt;
	bFirst = gptParams->bConnectMode;
	if (bFirst == 2)
	{
		fCharacters = gptComInt->pfnIsConnected();
		if (fCharacters)
		{
			geMode = LAUNCHMODE_BATTLENET;
			D2WinResizeWindow(1);
			CharSelOpen();
		}
		else
		{
			geMode = LAUNCHMODE_BATTLENET;
			D2WinResizeWindow(1);
			MainMenusOpenLogin();
		}
	}
	else if (bFirst == 4)
	{
		fCharacters = gptComInt->pfnIsConnected();
		if (fCharacters)
		{
			geMode = LAUNCHMODE_OPENBATTLENET;
			D2WinResizeWindow(1);
			CharSelOpen();
		}
		else
		{
			geMode = LAUNCHMODE_OPENBATTLENET;
			D2WinResizeWindow(1);
			MainMenusOpenLogin();
		}
	}
	else if (bFirst == 3)
	{
		BNetLoadProvider(gptParams);
		geMode = LAUNCHMODE_TCPIP;
		D2WinResizeWindow(1);
		MainMenusOpenTcpIp();
	}
	else
	{
		BNetLoadProvider(gptParams);
		if (gptParams->bSkipToBnet)
		{
			gptParams->bSkipToBnet = 0;
			gptParams->bShownLogo = 1;
			D2WinResizeWindow(1);
			geMode = LAUNCHMODE_BATTLENET;
			MainMenusOpenLogin();
		}
		else
		{
			if (!gptParams->bShownLogo)
			{
				geMode = LAUNCHMODE_SINGLE;
				D2WinResizeWindow(0);
				MainMenusLogo();
				gptParams->bShownLogo = 1;
			}
			else
			{
				geMode = LAUNCHMODE_SINGLE;
				D2WinResizeWindow(1);
				MainMenusOpen();
			}
		}
	}
	D2WinSetQuitCallback(MainMenusQuit);
	D2WinLoop(MainMenusLoop);
	D2SoundFastFadeAll();
	while (D2SoundFastIsPlaying())
		Sleep(0);
	D2SoundFastShutdown();
	MainMenusClear();
	MainMenusFreeCels();
	D2LangStrDeleteTable();
	D2WinCloseParent();
	return sgnResult;
}

static void __fastcall MainMenusFreeCels(void)
{
	int i;

	D2WinArchiveFreeCel(sghCelBlizno);
	D2WinArchiveFreeCel(sghCelTrademark);
	D2WinArchiveFreeCel(sghCel1002ddc8);
	D2WinArchiveFreeCel(sghCelCursor);
	D2WinArchiveFreeCel(sghCelDiablo2);
	D2WinArchiveFreeCel(sghCelLogoBlackLeft);
	D2WinArchiveFreeCel(sghCelLogoFireLeft);
	D2WinArchiveFreeCel(sghCelLogoBlackRight);
	D2WinArchiveFreeCel(sghCelLogoFireRight);
	D2WinArchiveFreeCel(sghCelTitleScreen);
	D2WinArchiveFreeCel(sghCelCreditsBckg);
	D2WinArchiveFreeCel(sghCelWideButton);
	D2WinArchiveFreeCel(sghCelMediumButton);
	D2WinArchiveFreeCel(sghCelShortButton);
	D2WinArchiveFreeCel(sghCelTextBox2);
	D2WinArchiveFreeCel(sghCelTermsOfUseBckg);
	D2WinArchiveFreeCel(ghCelCharSelectBckg);
	D2WinArchiveFreeCel(sghCelDifficultyLevels);
	D2WinArchiveFreeCel(sghCelCinematicsSelection);
	D2WinArchiveFreeCel(sghCelCharSelectBox);
	D2WinArchiveFreeCel(sghCelCharSelectBoxGrey);
	D2WinArchiveFreeCel(sghCelPopUpOk);
	D2WinArchiveFreeCel(sghCelPopUp340x224);
	D2WinArchiveFreeCel(sghCelPopUpWide);
	D2WinArchiveFreeCel(sghCelPopUpLargest);
	D2WinArchiveFreeCel(sghCelPopUpOk2);
	D2WinArchiveFreeCel(sghCelPopUpOkCancel);
	D2WinArchiveFreeCel(sghCelPopUpOkCancel2);
	D2WinArchiveFreeCel(sghCelOkCancelButton);
	D2WinArchiveFreeCel(sghCelCancelButton);
	D2WinArchiveFreeCel(sghCelSelectCharArrows);
	D2WinArchiveFreeCel(sghCelPassword);
	D2WinArchiveFreeCel(sghCelCreateNewChar);
	if (sghCel1002ddec)
	{
		D2WinArchiveFreeCel(sghCel1002ddec);
		sghCel1002ddec = NULL;
		D2WinArchiveFreeCel(sghCel1002ddf0);
		D2WinArchiveFreeCel(sghCel1002de00);
		D2WinArchiveFreeCel(sghCel1002de54);
		D2WinArchiveFreeCel(sghCel1002de58);
		for (i = 0; i < 5; i++)
			D2WinArchiveFreeCel(sgahCel1002de74[i]);
		for (i = 0; i < 5; i++)
			D2WinArchiveFreeCel(sgahCel1002de88[i]);
		for (i = 0; i < 5; i++)
			D2WinArchiveFreeCel(sgahCel1002de9c[i]);
		for (i = 0; i < 5; i++)
			D2WinArchiveFreeCel(sgahCel1002deb0[i]);
		for (i = 0; i < 5; i++)
			D2WinArchiveFreeCel(sgahCel1002dec4[i]);
		for (i = 0; i < 3; i++)
			D2WinArchiveFreeCel(sgahCel1002df08[i]);
		for (i = 0; i < 3; i++)
			D2WinArchiveFreeCel(sgahCel1002def4[i]);
		D2WinArchiveFreeCel(sghCel1002df1c);
		D2WinArchiveFreeCel(sghCel1002df30);
	}
	D2WinArchiveFreeCel(sghCelRealmBckg);
	D2WinArchiveFreeCel(sghCelJoinGameScrollbars);
	D2WinArchiveFreeCel(sghCelTcpIpBckg);
	D2WinArchiveFreeCel(sghCelIpAddressBox);
	D2WinArchiveFreeCel(sghCelDownloadPatchBckg);
	D2WinArchiveFreeCel(sghCelDownloadPatchBall);
}

static __inline void __fastcall MainMenusFlushInput(void)
{
	MSG tMsg;

	while (PeekMessageA(&tMsg, NULL, 0, 0, PM_REMOVE))
		switch (tMsg.message)
		{
		case WM_QUIT:
		case WM_CHAR:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			break;
		default:
			TranslateMessage(&tMsg);
			DispatchMessageA(&tMsg);
			break;
		}
}

static void __stdcall MainMenusPlayIntro(void)
{
	char szPath[260];
	char szSize[8];
	char szLanguage[4];
	DWORD dwMode;

	MainMenusFlushInput();
	dwMode = 0;
	if (!FogCpuIsSlow())
		strcpy(szSize, "640x292");
	else
	{
		dwMode = 2;
		strcpy(szSize, "640x146");
	}
	D2LangStrGetLanguageName(szLanguage, 0);
	sprintf(szPath, "%s\\video\\%s\\d2intro%s.bik", "DATA\\LOCAL", szLanguage, szSize);
	D2gfxWindowPlayCutscene(szPath, dwMode, 0);
	MainMenusTitle();
}

static void __stdcall MainMenusFirstIntro(void)
{
	char szValue[100];
	BYTE nFirst;
	BYTE nSecond;
	BYTE nThird;
	BYTE nFourth;
	BYTE bProgress;
	BOOL fSeen;

	fSeen = FALSE;
	if (StormSRegLoadString("Diablo II", "Aux Battle.net", 0, szValue, sizeof(szValue)))
	{
		nFourth = 34;
		sscanf(szValue, "%d.%d.%d.%d", &nFirst, &nSecond, &nThird, &nFourth);
		if ((BYTE)nFourth & 16)
			bProgress = 5;
		else if ((BYTE)nFourth & 8)
			bProgress = 4;
		else if ((BYTE)nFourth & 64)
			bProgress = 3;
		else
			bProgress = ((BYTE)nFourth & 4) ? 2 : 1;
		fSeen = bProgress >= 1;
	}
	else
	{
		sprintf(szValue, "%d.%d.%d.%d", 216, 148, 246, 34);
		StormSRegSaveString("Diablo II", "Aux Battle.net", 0, szValue);
	}
	if (!fSeen)
		MainMenusPlayIntro();
}

static void __fastcall MainMenusLogo(void)
{
#line 1721
	D2ASSERT(!gptParams->bShownLogo);
	D2WinShowCursor(FALSE);
	MainMenusClear();
	MainMenusFlushInput();
	if (D2WinCutsceneCreateWindow())
	{
		if (!FogCpuIsSlow())
		{
			D2gfxWindowPlayCutscene("Data\\Local\\Video\\New_BLIZ640x480.bik", 0, 0);
			D2gfxWindowPlayCutscene("Data\\Local\\Video\\BlizNorth640x480.bik", 0, 0);
		}
		else
		{
			D2gfxWindowPlayCutscene("Data\\Local\\Video\\New_BLIZ640x240.bik", 2, 0);
			D2gfxWindowPlayCutscene("Data\\Local\\Video\\BlizNorth640x240.bik", 2, 0);
		}
		MainMenusFirstIntro();
	}
	D2WinCutsceneCloseWindow(TRUE);
	MainMenusTitle();
}

static void __fastcall MainMenusTitle(void)
{
	D2WINCONTROL *ptText;

	MainMenusClear();
	MainMenusCreateChild(2);
	MainMenusCreateChild(3);
	MainMenusCreateChild(6);
	MainMenusCreateChild(7);
	MainMenusLoadPalette(gszPaletteSkyDat, gszPaletteSkyPl2);
	ptText = MainMenusCreateChild(4);
	D2WinTextBoxAddColorLineUnicode(ptText, D2LangStrGetStringByIndex(5104), 4, 0);
	D2WinTextBoxAddColorLineUnicode(ptText, D2LangStrGetStringByIndex(5105), 4, 0);
	D2WinShowCursor(FALSE);
	if (D2SoundGetOptionsMusic())
		D2SoundSmpMusicSetState(1);
}

BOOL __stdcall MainMenusOnSinglePlayer(_PARAMS *ptMsg)
{
	char szPattern[1024];
	char szSave[1024];
	HANDLE hFind;

	geMode = LAUNCHMODE_SINGLE;
	FogSafesockGetSavePath(szSave, 1024);
	sprintf(szPattern, "%s*.d2s", szSave);
	{
		WIN32_FIND_DATAA tFind;

		hFind = FindFirstFileA(szPattern, &tFind);
	}
	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
		D2SoundFastFadeAll();
		MainMenusClear();
		CharSelOpen();
		return TRUE;
	}
	MainMenusOpenClassSelect();
	return TRUE;
}

BOOL __stdcall MainMenusOnOpenBattleNet(_PARAMS *ptMsg)
{
	geMode = LAUNCHMODE_OPENBATTLENET;
	MainMenusOpenLogin();
	return TRUE;
}

BOOL __fastcall MainMenusAnySave(void)
{
	char szPattern[1024];
	char szSave[1024];
	WIN32_FIND_DATAA tFind;
	HANDLE hFind;

	FogSafesockGetSavePath(szSave, 1024);
	sprintf(szPattern, "%s*.d2s", szSave);
	hFind = FindFirstFileA(szPattern, &tFind);
	if (!(hFind == INVALID_HANDLE_VALUE)) {
		FindClose(hFind);
		return TRUE; }
	return FALSE;
}

BOOL __stdcall MainMenusOnToCharSel(_PARAMS *ptMsg)
{
	D2SoundFastFadeAll();
	MainMenusClear();
	CharSelOpen();
	return TRUE;
}

BOOL __stdcall MainMenusOnOtherMultiplayer(_PARAMS *ptMsg)
{
	MainMenusBackground();
	MainMenusCreateChild(158);
	MainMenusCreateChild(159);
	MainMenusCreateChild(160);
	return TRUE;
}

void __fastcall MainMenusCreditsEnd(void)
{
	int i;

	for (i = 50; i != 0; i--)
		D2WinTextBoxAddLineUTF(sgptCreditsCentre, &gchEmpty);
	D2WinTextBoxAddLineUTF(sgptCreditsCentre, "The End");
	sgnCreditsEnd = D2WinTextBoxGetTop(sgptCreditsCentre);
}

char * __fastcall MainMenusCreditsRead(int *pnSize)
{
	char szPath[260];
	char szLanguage[4];
	HANDLE hFile;
	char *ptFile;
	int nSize;
	int i;

	D2LangStrGetLanguageName(szLanguage, 0);
	char szName[12] = "Credits.txt";
	StormSStrPrintf(szPath, sizeof(szPath), "%s\\UI\\%s\\%s", "DATA\\LOCAL", szLanguage, szName);
	if (!FogSafesockOpenStormFile(szPath, &hFile))
		return NULL;
	nSize = (int)FogSafesockGetStormFileSize(hFile, 0);
#line 3958
	ptFile = (char *)D2ALLOC2(nSize);
	D2ASSERT(ptFile);
	memset(ptFile, 0, nSize);
	FogSafesockReadStormFile(hFile, ptFile, nSize, 0, 0, 0);
	FogSafesockCloseStormFile(hFile);
	for (i = 0; i < nSize; i++)
	{
		if (ptFile[i] == '\n' || ptFile[i] == '\r')
			ptFile[i] = 0;
	}
	*pnSize = nSize;
	return ptFile;
}

void __fastcall MainMenusCreditsPad(void)
{
	int nCentre;
	int nLeft;
	int nRight;

	nCentre = D2WinTextBoxGetTop(sgptCreditsCentre);
	nLeft = D2WinTextBoxGetTop(sgptCreditsLeft);
	nRight = D2WinTextBoxGetTop(sgptCreditsRight);
	while (nLeft > nRight)
	{
		D2WinTextBoxAddLineUTF(sgptCreditsRight, &gchEmpty);
		nRight = D2WinTextBoxGetTop(sgptCreditsRight);
	}
	while (nLeft < nRight)
	{
		D2WinTextBoxAddLineUTF(sgptCreditsLeft, &gchEmpty);
		nLeft = D2WinTextBoxGetTop(sgptCreditsLeft);
	}
	while (nCentre > nLeft)
	{
		D2WinTextBoxAddLineUTF(sgptCreditsLeft, &gchEmpty);
		nLeft = D2WinTextBoxGetTop(sgptCreditsLeft);
		D2WinTextBoxAddLineUTF(sgptCreditsRight, &gchEmpty);
	}
	while (nCentre < nLeft)
	{
		D2WinTextBoxAddLineUTF(sgptCreditsCentre, &gchEmpty);
		nCentre = D2WinTextBoxGetTop(sgptCreditsCentre);
	}
	D2WinTextBoxAddLineUTF(sgptCreditsCentre, &gchEmpty);
	D2WinTextBoxAddLineUTF(sgptCreditsLeft, &gchEmpty);
	D2WinTextBoxAddLineUTF(sgptCreditsRight, &gchEmpty);
}

static __inline char *MainMenusCreditsLine(char *pszLine, const char *pszText)
{
	return strcpy(pszLine, pszText);
}

BOOL __fastcall MainMenusCreditsGapSides(void)
{
	D2WinTextBoxAddLineUTF(sgptCreditsLeft, &gchEmpty);
	return D2WinTextBoxAddLineUTF(sgptCreditsRight, &gchEmpty);
}

BOOL __fastcall MainMenusCreditsGapCentre(void)
{
	return D2WinTextBoxAddLineUTF(sgptCreditsCentre, &gchEmpty);
}

BOOL __fastcall MainMenusCreditsLoad(DWORD dwUnused)
{
	char szLine[1024];
	char szRight[256];
	char szLeft[256];
	int nSize;
	char *pszText;
	int nLength;
	int i;

	nSize = 0;
	pszText = MainMenusCreditsRead(&nSize);
	for (i = 50; i != 0; i--)
	{
		D2WinTextBoxAddLineUTF(sgptCreditsCentre, &gchEmpty);
		D2WinTextBoxAddLineUTF(sgptCreditsLeft, &gchEmpty);
		D2WinTextBoxAddLineUTF(sgptCreditsRight, &gchEmpty);
	}
	if (nSize != 0)
	{
		do
		{
			if (*pszText == '*')
			{
				D2WinTextBoxAddLineUTF(sgptCreditsLeft, &gchEmpty);
				D2WinTextBoxAddLineUTF(sgptCreditsRight, &gchEmpty);
				pszText++;
				if (MainMenusCreditsLine(szLine, pszText))
					D2WinTextBoxAddColorLineUTF(sgptCreditsCentre, szLine, 1);
				else
					nSize = 0;
				nLength = strlen(szLine) + 2;
				nSize -= nLength;
				pszText += nLength;
			}
			else
			{
				while (1)
				{
					if (nSize == 0)
						break;
					szLeft[0] = 0;
					szRight[0] = 0;
					if (isalpha(*pszText))
					{
						if (MainMenusCreditsLine(szLine, pszText))
						{
							strcpy(szLeft, szLine);
							nLength = strlen(szLine) + 2;
							nSize -= nLength;
							pszText += nLength;
						}
						else
							nSize = 0;
						if (nSize != 0 && *pszText != '*')
						{
							if (MainMenusCreditsLine(szLine, pszText))
							{
								strcpy(szRight, szLine);
								nLength = strlen(szLine) + 2;
								nSize -= nLength;
								pszText += nLength;
							}
							else
								nSize = 0;
						}
					}
					else
						nSize = 0;
					if (szLeft[0] && szRight[0])
					{
						D2WinTextBoxAddLineUTF(sgptCreditsLeft, szLeft);
						D2WinTextBoxAddLineUTF(sgptCreditsRight, szRight);
					}
					else
						D2WinTextBoxAddLineUTF(sgptCreditsCentre, szLeft);
					D2WinTextBoxAddLineUTF(sgptCreditsCentre, &gchEmpty);
					if (*pszText == '*')
						break;
				}
			}
			MainMenusCreditsPad();
		} while (nSize != 0);
	}
	for (i = 50; i != 0; i--)
		D2WinTextBoxAddLineUTF(sgptCreditsCentre, &gchEmpty);
	D2WinTextBoxAddLineUTF(sgptCreditsCentre, "The End");
	sgnCreditsEnd = D2WinTextBoxGetTop(sgptCreditsCentre);
	return TRUE;
}

BOOL __stdcall MainMenusOnCredits(_PARAMS *ptMsg)
{
	MainMenusClear();
	MainMenusLoadPalette(gszPaletteSkyDat, gszPaletteSkyPl2);
	MainMenusCreateChild(28);
	MainMenusCreateChild(29);
	sgptCreditsLeft = MainMenusCreateChild(30);
	sgptCreditsRight = MainMenusCreateChild(31);
	sgptCreditsCentre = MainMenusCreateChild(32);
	sgptCreditsTimer = MainMenusCreateChild(33);
	D2WinTimerDisableKey(sgptCreditsTimer);
	D2WinTimerSetPeriodic(sgptCreditsTimer);
	MainMenusCreditsLoad(0);
	D2WinTextBoxSetTop(sgptCreditsCentre, 20);
	D2WinTextBoxSetTop(sgptCreditsLeft, 20);
	D2WinTextBoxSetTop(sgptCreditsRight, 20);
	return TRUE;
}

BOOL __stdcall MainMenusOnCreditsTimer(_PARAMS *ptMsg)
{
	int nLine;

	nLine = D2WinTextBoxGetTop(sgptCreditsCentre);
	if (sgnCreditsEnd <= nLine + 10)
		MainMenusOpen();
	return TRUE;
}

BOOL __stdcall MainMenusOnCinematics(_PARAMS *ptMsg)
{
	char szValue[100];
	D2WINCONTROL *ptTitle;
	D2WINCONTROL *ptAct2;
	D2WINCONTROL *ptAct3;
	D2WINCONTROL *ptAct4;
	D2WINCONTROL *ptEnd;
	BYTE nFirst;
	BYTE nSecond;	BYTE nThird;

	BYTE nFourth;
	BYTE bProgress;

	MainMenusClear();
	D2SoundSmpFadeOutMasterRaw(180);
	D2SoundSmpMusicStop();
	D2SoundSmpSetMasterVolume(255);
	MainMenusBackground();
	MainMenusLoadPalette(gszPaletteSkyDat, gszPaletteSkyPl2);
	D2WinArchiveCloseDiscs();
	if (!D2WinArchiveOpenCinematicsDisc(MainMenusAskCinematicsDisc))
	{
		D2WinArchiveCloseDiscs();
		D2WinArchiveOpenPlayDisc(MainMenusAskPlayDisc);
		if (D2SoundGetOptionsMusic())
			D2SoundSmpMusicSetState(1);
		return FALSE;
	}
	MainMenusCreateChild(34);
	ptTitle = MainMenusCreateChild(35);
	D2WinTextBoxAddLineUnicode(ptTitle, 	(DWORD)D2LangStrGetStringByIndex(5114));
	if (StormSRegLoadString("Diablo II", "Aux Battle.net", 0, szValue, sizeof(szValue)))
	{
		nFourth = 34;
		sscanf(szValue, "%d.%d.%d.%d", &nFirst, &nSecond, &nThird, &nFourth);
		if ((BYTE)nFourth & 16)
			bProgress = 5;
		else if ((BYTE)nFourth & 8)
			bProgress = 4;
		else if ((BYTE)nFourth & 64)
			bProgress = 3;
		else
			bProgress = ((BYTE)nFourth & 4) ? 2 : 1;
	}
	else
	{
		bProgress = 1;
		sprintf(szValue, "%d.%d.%d.%d", 216, 148, 246, 34);
		StormSRegSaveString("Diablo II", "Aux Battle.net", 0, szValue);
	}
	MainMenusCreateChild(36);
	ptAct2 = MainMenusCreateChild(37);
	ptAct3 = MainMenusCreateChild(38);
	ptAct4 = MainMenusCreateChild(39);
	ptEnd = MainMenusCreateChild(40);
	switch (bProgress)
	{
	default:
		D2WinChildSetEnabled(ptAct2, FALSE);
	case 2:
		D2WinChildSetEnabled(ptAct3, FALSE);
	case 3:
		D2WinChildSetEnabled(ptAct4, FALSE);
	case 4:
		D2WinChildSetEnabled(ptEnd, FALSE);
	case 5:
		break;
	}
	MainMenusCreateChild(41);
	return TRUE;
}

BOOL __stdcall MainMenusOnFilmIntro(_PARAMS *ptMsg)
{
	MainMenusClear();
	MainMenusDestroyBoxChildren();
	MainMenusFlushInput();
	if (D2WinCutsceneCreateWindow())
	{
		char szPath[260];
		char szSize[8];
		char szLanguage[4];
		DWORD dwMode;

		dwMode = 0;
		if (!FogCpuIsSlow())
			strcpy(szSize, "640x292");
		else
		{
			dwMode = 2;
			strcpy(szSize, "640x146");
		}
		D2LangStrGetLanguageName(szLanguage, 0);
		sprintf(szPath, "%s\\video\\%s\\d2intro%s.bik", "DATA\\LOCAL", szLanguage, szSize);
		D2gfxWindowPlayCutscene(szPath, dwMode, 0);
		D2WinCutsceneCloseWindow(TRUE);
	}
	MainMenusFlushInput();
	MainMenusOnCinematics(ptMsg);
	return TRUE;
}

BOOL __stdcall MainMenusOnFilmActTwoStart(_PARAMS *ptMsg)
{
	MainMenusClear();
	MainMenusDestroyBoxChildren();
	MainMenusFlushInput();
	if (D2WinCutsceneCreateWindow())
	{
		char szPath[260];
		char szSize[8];
		char szLanguage[4];
		DWORD dwMode;

		dwMode = 0;
		if (!FogCpuIsSlow())
			strcpy(szSize, "640x292");
		else
		{
			dwMode = 2;
			strcpy(szSize, "640x146");
		}
		D2LangStrGetLanguageName(szLanguage, 0);
		sprintf(szPath, "%s\\video\\%s\\Act02start%s.bik", "DATA\\LOCAL", szLanguage, szSize);
		D2gfxWindowPlayCutscene(szPath, dwMode, 0);
		D2WinCutsceneCloseWindow(TRUE);
	}
	MainMenusFlushInput();
	MainMenusOnCinematics(ptMsg);
	return TRUE;
}

BOOL __stdcall MainMenusOnFilmActThreeStart(_PARAMS *ptMsg)
{
	MainMenusClear();
	MainMenusDestroyBoxChildren();
	MainMenusFlushInput();
	if (D2WinCutsceneCreateWindow())
	{
		char szPath[260];
		char szSize[8];
		char szLanguage[4];
		DWORD dwMode;

		dwMode = 0;
		if (!FogCpuIsSlow())
			strcpy(szSize, "640x292");
		else
		{
			dwMode = 2;
			strcpy(szSize, "640x146");
		}
		D2LangStrGetLanguageName(szLanguage, 0);
		sprintf(szPath, "%s\\video\\%s\\Act03start%s.bik", "DATA\\LOCAL", szLanguage, szSize);
		D2gfxWindowPlayCutscene(szPath, dwMode, 0);
		D2WinCutsceneCloseWindow(TRUE);
	}
	MainMenusFlushInput();
	MainMenusOnCinematics(ptMsg);
	return TRUE;
}

BOOL __stdcall MainMenusOnFilmActFourStart(_PARAMS *ptMsg)
{
	MainMenusClear();
	MainMenusDestroyBoxChildren();
	MainMenusFlushInput();
	if (D2WinCutsceneCreateWindow())
	{
		char szPath[260];
		char szSize[8];
		char szLanguage[4];
		DWORD dwMode;

		dwMode = 0;
		if (!FogCpuIsSlow())
			strcpy(szSize, "640x292");
		else
		{
			dwMode = 2;
			strcpy(szSize, "640x146");
		}
		D2LangStrGetLanguageName(szLanguage, 0);
		sprintf(szPath, "%s\\video\\%s\\Act04start%s.bik", "DATA\\LOCAL", szLanguage, szSize);
		D2gfxWindowPlayCutscene(szPath, dwMode, 0);
		D2WinCutsceneCloseWindow(TRUE);
	}
	MainMenusFlushInput();
	MainMenusOnCinematics(ptMsg);
	return TRUE;
}

BOOL __stdcall MainMenusOnFilmEnd(_PARAMS *ptMsg)
{
	MainMenusClear();
	MainMenusDestroyBoxChildren();
	MainMenusFlushInput();
	if (D2WinCutsceneCreateWindow())
	{
		char szPath[260];
		char szSize[8];
		char szLanguage[4];
		DWORD dwMode;

		dwMode = 0;
		if (!FogCpuIsSlow())
			strcpy(szSize, "640x292");
		else
		{
			dwMode = 2;
			strcpy(szSize, "640x146");
		}
		D2LangStrGetLanguageName(szLanguage, 0);
		sprintf(szPath, "%s\\video\\%s\\Act04end%s.bik", "DATA\\LOCAL", szLanguage, szSize);
		D2gfxWindowPlayCutscene(szPath, dwMode, 0);
		D2WinCutsceneCloseWindow(TRUE);
	}
	MainMenusFlushInput();
	MainMenusOnCinematics(ptMsg);
	return TRUE;
}

BOOL __stdcall MainMenusOnCinematicsCancel(_PARAMS *ptMsg)
{
	D2WinArchiveCloseDiscs();
	D2WinArchiveOpenPlayDisc(MainMenusAskPlayDisc);
	if (D2SoundGetOptionsMusic())
		D2SoundSmpMusicSetState(1);
	MainMenusOpen();
	return TRUE;
}

BOOL __stdcall MainMenusOnPlayDiscOk(_PARAMS *ptMsg)
{
	sgfDiscOk = TRUE;
	return TRUE;
}

BOOL __stdcall MainMenusOnCinematicsDiscOk(_PARAMS *ptMsg)
{
	sgfDiscOk = TRUE;
	return TRUE;
}

BOOL __stdcall MainMenusOnDiscCancel(_PARAMS *ptMsg)
{
	MainMenusOpen();
	sgfDiscCancel = TRUE;
	return TRUE;
}

BOOL __stdcall MainMenusDiscLoop(DWORD dwArg)
{
	if (sgfDiscCancel || sgfDiscOk)
		D2WinQuitLoop();
	return TRUE;
}

void __fastcall MainMenusCinematicsDiscBox(void)
{
	D2WINCONTROL *ptChild;

	MainMenusDestroyBoxChildren();
	ptChild = MainMenusCreateBoxChild(108);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(42);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(43);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(111);
	D2WinChildSetOnTop(ptChild, TRUE);
	D2WinTextBoxAddColorLineUnicode(ptChild, D2LangStrGetStringByIndex(5168), 0, 0);
	D2WinTextBoxAddColorLineUnicode(ptChild, D2LangStrGetStringByIndex(5170), 0, 0);
}

void __fastcall MainMenusPlayDiscBox(void)
{
	D2WINCONTROL *ptChild;

	MainMenusDestroyBoxChildren();
	ptChild = MainMenusCreateBoxChild(112);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(44);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(111);
	D2WinChildSetOnTop(ptChild, TRUE);
	D2WinTextBoxAddColorLineUnicode(ptChild, D2LangStrGetStringByIndex(5168), 0, 0);
	D2WinTextBoxAddColorLineUnicode(ptChild, D2LangStrGetStringByIndex(5169), 0, 0);
}

BOOL __cdecl MainMenusAskPlayDisc(void)
{
	D2WINCONTROL *ptChild;

	MainMenusDestroyBoxChildren();
	ptChild = MainMenusCreateBoxChild(112);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(44);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(111);
	D2WinChildSetOnTop(ptChild, TRUE);
	D2WinTextBoxAddColorLineUnicode(ptChild, D2LangStrGetStringByIndex(5168), 0, 0);
	D2WinTextBoxAddColorLineUnicode(ptChild, D2LangStrGetStringByIndex(5169), 0, 0);
	sgfDiscCancel = FALSE;
	sgfDiscOk = FALSE;
	sgpfnDiscOpen = (DWORD)D2WinArchiveOpenPlayDiscRaw;
	D2WinLoop(MainMenusDiscLoop);
	MainMenusDestroyBoxChildren();
	return sgfDiscOk;
}

BOOL __cdecl MainMenusAskCinematicsDisc(void)
{
	D2WINCONTROL *ptChild;

	MainMenusDestroyBoxChildren();
	ptChild = MainMenusCreateBoxChild(108);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(42);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(43);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(111);
	D2WinChildSetOnTop(ptChild, TRUE);
	D2WinTextBoxAddColorLineUnicode(ptChild, D2LangStrGetStringByIndex(5168), 0, 0);
	D2WinTextBoxAddColorLineUnicode(ptChild, D2LangStrGetStringByIndex(5170), 0, 0);
	sgfDiscCancel = FALSE;
	sgfDiscOk = FALSE;
	sgpfnDiscOpen = (DWORD)D2WinArchiveOpenCinematicsDiscRaw;
	D2WinLoop(MainMenusDiscLoop);
	MainMenusDestroyBoxChildren();
	return sgfDiscOk;
}

BOOL __stdcall MainMenusOnTcpIp(_PARAMS *ptMsg)
{
	geMode = LAUNCHMODE_TCPIP;
	MainMenusOpenTcpIp();
	return TRUE;
}

BOOL __stdcall MainMenusOnBattleNet(_PARAMS *ptMsg)
{
	geMode = LAUNCHMODE_BATTLENET;
	MainMenusOpenLogin();
	return TRUE;
}

BOOL __stdcall MainMenusOnExit(_PARAMS *ptMsg)
{
	sgnResult = 0;
	D2SoundSmpFadeOutMasterRaw(180);
	D2WinQuitLoop();
	return TRUE;
}

BOOL __stdcall MainMenusOnHostGame(_PARAMS *ptMsg)
{
	gptParams->dwGameType = 6;
	strcpy(gptParams->szServerIp, FogSafesockGetLocalAddress());
	geMode = LAUNCHMODE_TCPIP;
	gptParams->dwGameFlags = 4;
	D2SoundFastFadeAll();
	MainMenusClear();
	CharSelOpen();
	return TRUE;
}

void __fastcall MainMenusJoinAnswer(const BYTE *pbMessage, int nSize)
{
	if (nSize == -1)
		return;
	switch (pbMessage[0])
	{
	case 0:
		sgfJoined = TRUE;
		break;
	case 1:
		sgfJoined = FALSE;
		break;
	}
}

BOOL __stdcall MainMenusJoinLoop(DWORD dwArg)
{
	BYTE abMessage[512];

	if ((DWORD)D2NetGetCltSysMsg(abMessage, sizeof(abMessage)) != (DWORD)-1)
	{
		switch (abMessage[0])
		{
		case 0:
			sgfJoined = TRUE;
			D2WinQuitLoop();
			return TRUE;
		case 1:
			sgfJoined = FALSE;
			break;

		}
	}
	if (sgfJoined || sgfWaitCancelled)
		D2WinQuitLoop();
	return TRUE;
}

BOOL __stdcall MainMenusLoopOnce(DWORD dwArg)
{
	D2WinQuitLoop();
	return TRUE;
}

BOOL __stdcall MainMenusOnJoinGame(_PARAMS *ptMsg)
{
	D2WINCONTROL *ptChild;

	MainMenusLoadPalette(gszPaletteSkyDat, gszPaletteSkyPl2);
	D2WinDestroyChild(&sgptTcpIpTimer);
	ptChild = MainMenusCreateBoxChild(174);
	D2WinSetInputChild(ptChild);
	MainMenusCreateBoxChild(173);
	ptChild = MainMenusCreateBoxChild(171);
	D2WinTextBoxAddLineUnicode(ptChild, 	(const WORD *)D2LangStrGetStringByIndex(5120));
	sgptJoinEdit = MainMenusCreateBoxChild(172);
	D2WinSetKeyFocus(sgptJoinEdit);
	ptChild = MainMenusCreateBoxChild(175);
	D2WinSetEscapeButton(ptChild);
	MainMenusCreateBoxChild(176);
	return TRUE;
}

BOOL __stdcall MainMenusOnJoinCancel(_PARAMS *ptMsg)
{
	MainMenusDestroyBoxChildren();
	sgptTcpIpTimer = MainMenusCreateChild(170);
	D2WinTimerDisableKey(sgptTcpIpTimer);
	D2WinTimerSetPeriodic(sgptTcpIpTimer);
	return TRUE;
}

BOOL __stdcall MainMenusConnectLoop(DWORD dwArg)
{
	DWORD dwState;

	dwState = D2NetClientPollConnect();
	if (dwState == 0)
	{
		sgfConnected = FALSE;
		D2WinQuitLoop();
		return TRUE;
	}
	if (dwState == 2)
	{
		sgfConnected = TRUE;
		D2WinQuitLoop();
	}
	return TRUE;
}

static BOOL __fastcall MainMenusJoinAddress(char *pszAddress)
{
	const char *pszResolved;
	int nPart1, nPart2, nPart3, nPart4;

	Unicode::toUtf(pszAddress, D2WinEditBoxGetString(sgptJoinEdit), 256);
	if (!pszAddress[0])
		return FALSE;
	if (sscanf(pszAddress, "%d.%d.%d.%d", &nPart1, &nPart2, &nPart3, &nPart4) != 4)
	{
		pszResolved = FogSafesockResolve(pszAddress);
		if (pszResolved)
			strcpy(pszAddress, pszResolved);
		else
		{
			D2WinEditBoxSetStringUTF(sgptJoinEdit, &gchEmpty);
			return FALSE;
		}
	}
	return TRUE;
}

BOOL __stdcall MainMenusOnJoinOk(_PARAMS *ptMsg)
{
	char szAddress[1024];

	if (!MainMenusJoinAddress(szAddress))
		return TRUE;
	strcpy(gptParams->szServerIp, szAddress);
	MainMenusWaitBox();
	D2WinLoop(MainMenusLoopOnce);
	sgfConnected = FALSE;
	D2NetClientInit(0, szAddress);
	D2WinLoop(MainMenusConnectLoop);
	if (!sgfConnected)
	{
		D2NetClientClose();
		MainMenusDestroyBoxChildren();
		MainMenusOkBox((const WORD *)D2LangStrGetStringByIndex(5211), MainMenusOpenTcpIp);
		return TRUE;
	}
	MainMenusWaitBox();
	D2WinLoop(MainMenusJoinLoop);
	D2NetClientClose();
	MainMenusDestroyBoxChildren();
	if (MainMenusWaitCancelled())
		return TRUE;
	geMode = LAUNCHMODE_TCPIP;
	gptParams->dwGameType = 7;
	gptParams->wJoinId = 1;
	MainMenusToCharSel();
	return TRUE;
}

BOOL __stdcall MainMenusOnCreateOnRealm(_PARAMS *ptMsg)
{
	MainMenusCreateOnRealm();
	return TRUE;
}

BOOL __fastcall MainMenusEnterRealm(const char *pszName, const char *pszRealm)
{
	strcpy(sgszRealm, pszRealm);
	MainMenusRealmJoin();
	if (sgfWaitCancelled)
	{
		MainMenusDestroyBoxChildren();
		return FALSE;
	}
	if (WaitForSingleObject(sghMcpConnectThread, 0) != WAIT_OBJECT_0)
	{
		D2MCPClientD2MCPClose();
		return FALSE;
	}
	sghMcpConnectThread = NULL;
	if (D2MCPClientD2MCPGetStatus())
	{
		D2MCPClientD2MCPClose();
		return FALSE;
	}
	D2MCPClientToMCPSelectCharacter(pszName);
	D2WinLoop(MainMenusMcpLoopSelectCharacter);
	if (D2MCPClientD2MCPGetStatus() != 70 && D2MCPClientD2MCPIsConnected())
	{
		D2MCPClientToMCPSendDeleteCharacter(0);
		D2WinLoop(MainMenusMcpLoopDeleteCharacter);
		D2MCPClientD2MCPClose();
	}
	return TRUE;
}

static void __fastcall MainMenusSetClass(BYTE bClass)
{
	gptParams->bClass = bClass;
	gptParams->bAmazon = 0;
	gptParams->bBarbarian = 0;
	gptParams->bNecromancer = 0;
	gptParams->bPaladin = 0;
	gptParams->bSorceress = 0;
	if (bClass == LAUNCHCLASS_AMAZON)
		gptParams->bAmazon = 1;
	else if (bClass == LAUNCHCLASS_BARBARIAN)
		gptParams->bBarbarian = 1;
	else if (bClass == LAUNCHCLASS_NECROMANCER)
		gptParams->bNecromancer = 1;
	else if (bClass == LAUNCHCLASS_PALADIN)
		gptParams->bPaladin = 1;
	else if (bClass == LAUNCHCLASS_SORCERESS)
		gptParams->bSorceress = 1;
}

BOOL __fastcall MainMenusStart(const char *pszName, WORD wFlags, BYTE bClass, const char *pszRealm)
{
	BOOL fRealm;

	fRealm = pszRealm[0] != '\0';
	strcpy(gptParams->szName, pszName);
	gptParams->wCharFlags = wFlags;
	MainMenusSetClass(bClass);
	strcpy(sgszRealm, pszRealm);
	if (geMode == LAUNCHMODE_BATTLENET || geMode == LAUNCHMODE_OPENBATTLENET)
	{
		if (!gptComInt->pfnIsConnected())
		{
			MainMenusBackground();
			if (!BNetConnect())
				return FALSE;
			if (!BNetLogin(gptParams, TRUE))
				return FALSE;
		}
	}
	if (geMode == LAUNCHMODE_SINGLE)
	{
		gptParams->bConnectMode = 1;
		sgnResult = 1;
		gptParams->dwGameType = 0;
		gptParams->dwGameFlags = 4;
		if (wFlags & 4)
			gptParams->dwGameFlags = 2052;
		D2WinQuitLoop();
		return TRUE;
	}
	if (geMode == LAUNCHMODE_TCPIP)
	{
		gptParams->bConnectMode = 3;
		sgnResult = 1;
		gptParams->dwGameFlags = 4;
		if (wFlags & 4)
			gptParams->dwGameFlags = 2052;
		D2WinQuitLoop();
		return TRUE;
	}
	if (fRealm)
	{
		MainMenusRealmJoin();
		if (sgfWaitCancelled)
		{
			MainMenusDestroyBoxChildren();
			return FALSE;
		}
		if (WaitForSingleObject(sghMcpConnectThread, 0) != WAIT_OBJECT_0)
		{
			sghMcpConnectThread = NULL;
			MainMenusOkBox((const WORD *)D2LangStrGetStringByIndex(5211), MainMenusToCharSel);
			return TRUE;
		}
		sghMcpConnectThread = NULL;
		if (D2MCPClientD2MCPGetStatus())
		{
			D2MCPClientD2MCPClose();
			switch (D2MCPClientD2MCPGetStatus())
			{
			case 2:
				MainMenusOkBox((const WORD *)D2LangStrGetStringByIndex(5211), MainMenusToCharSel);
				break;
			case 71:
				MainMenusOkBox((const WORD *)D2LangStrGetStringByIndex(5212), MainMenusToCharSel);
				break;
			default:
				MainMenusOkBox((const WORD *)D2LangStrGetStringByIndex(5356), MainMenusToCharSel);
				break;
			}
			return TRUE;
		}
		D2MCPClientToMCPSelectCharacter(gptParams->szName);
		MainMenusWaitBox();
		D2WinLoop(MainMenusMcpLoopSelectCharacter);
		if (D2MCPClientD2MCPGetStatus() == 70 || !D2MCPClientD2MCPIsConnected())
		{
			MainMenusDestroyBoxChildren();
			MainMenusCancelBox((const WORD *)D2LangStrGetStringByIndex(5133), MainMenusToCharSel);
			return FALSE;
		}
		if (sgfWaitCancelled)
		{
			MainMenusDestroyBoxChildren();
			return FALSE;
		}
	}
	else
		gptParams->szRealm[0] = 0;
	if (fRealm)
		gptParams->bConnectMode = 2;
	else
	{
		gptParams->bConnectMode = 4;
		gptParams->dwGameFlags = 4;
		if (wFlags & 4)
			gptParams->dwGameFlags = 2052;
	}
	sgnResult = 3;
	MainMenusWaitBox();
	D2WinLoop(MainMenusLoopOnce);
	D2WinQuitLoop();
	return TRUE;
}

BOOL __stdcall MainMenusOnNewCharacter(_PARAMS *ptMsg)
{
	MainMenusOpenClassSelect();
	return TRUE;
}

BOOL __stdcall MainMenusOnIntro(_PARAMS *ptMsg)
{
	if (D2WinCutsceneCreateWindow())
		MainMenusPlayIntro();
	D2WinCutsceneCloseWindow(TRUE);
	MainMenusTitle();
	return TRUE;
}

BOOL __stdcall MainMenusOnTitleEnd(_PARAMS *ptMsg)
{
	MainMenusOpen();
	return TRUE;
}

BOOL __stdcall MainMenusOnCharSelExit(_PARAMS *ptMsg)
{
	BNetLeave();
	MainMenusOpen();
	return TRUE;
}

BOOL __stdcall MainMenusOnBackToMain(_PARAMS *ptMsg)
{
	MainMenusOpen();
	return TRUE;
}

BOOL __stdcall MainMenusToCharSelSecond(_PARAMS *ptMsg)
{
	D2SoundFastFadeAll();
	MainMenusClear();
	CharSelOpen();
	return TRUE;
}

static BOOL __fastcall MainMenusSaveExists(void)
{
	char szSave[1024];
	char szUtf[256];
	char szPath[260];
	char szName[256];
	DWORD hFile;
	DWORD dwEdit;

	FogSafesockGetSavePath(szSave, 1024);
	dwEdit = (DWORD)D2WinEditBoxGetString(sgptNameEdit);
	(void)D2LangUnicodetoUtf(szUtf, dwEdit, 256);
	sprintf(szName, "%s", szUtf);
	sprintf(szPath, "%s%s.d2s", szSave, szName);
	hFile = (DWORD)FogSafesockCreateFile(szPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN,
		0);
	if (!(hFile == (DWORD)INVALID_HANDLE_VALUE)) {
		FogSafesockCloseHandle(hFile);
		return TRUE; }
	return FALSE;
}

BOOL __stdcall MainMenusOnCreateLocal(_PARAMS *ptMsg)
{
	char szFile[260];
	char szName[32];
	DWORD hFile;

	{
		char szPath[1024];

		FogSafesockGetSavePath(szPath, sizeof(szPath));
		strcpy(szName, gptParams->szName);
		sprintf(szFile, "%s%s.d2s", szPath, szName);
	}
	hFile = (DWORD)FogSafesockCreateFile(szFile, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN,
		0);
	if (hFile != (DWORD)-1)
	{
		FogSafesockCloseHandle(hFile);
		MainMenusCancelBox((const WORD *)D2LangStrGetStringByIndex(5165), CharSelOpen);
		return TRUE;
	}
	D2MCPClientD2MCPClose();
	SaveLoadSetExtraFlag(FALSE);
	SaveLoadNew(gptParams->szName, gptParams->bClass, gptParams->abSummary, gptParams->wCharFlags);
	SaveLoadWrite(gptParams->szName, NULL);
	gptParams->bConnectMode = 2;
	sgnResult = 3;
	D2WinQuitLoop();
	return TRUE;
}

BOOL __fastcall MainMenusShowShard(int nIndex, BOOL fSetEdit)
{
	int i;

	if (nIndex < 0)
	{
		D2WinClearTextBox(sgptShardText);
		if (fSetEdit)
			D2WinEditBoxSetStringUTF(sgptRealmEdit, &gchEmpty);
		return TRUE;
	}
	for (i = 0; i < sgnShardListSize && sgptShardArray[i]; i++)
	{
		if (nIndex == i)
		{
			D2WinClearTextBox(sgptShardText);
			D2WinTextBoxAddLineUTF(sgptShardText, sgptShardArray[i]->pszName);
			D2WinTextBoxAddLineUTF(sgptShardText, &gchEmpty);
			D2WinTextBoxAddLineUTF(sgptShardText, sgptShardArray[i]->pszDescription);
			if (fSetEdit)
				D2WinEditBoxSetStringUTF(sgptRealmEdit, sgptShardArray[i]->pszName);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL __stdcall MainMenusOnShard(_PARAMS *ptMsg)
{
	int nLine;

	nLine = D2WinTextBoxGetSelectedNum(sgptShardList);
	nLine += D2WinTextBoxGetTop(sgptShardList);
	if (nLine == sgnShardSelected)
	{
		if (nLine != -1)
			MainMenusOnRealmOk(ptMsg);
		return FALSE;
	}
	MainMenusShowShard(nLine, TRUE);
	sgnShardSelected = nLine;
	return FALSE;
}

static void __fastcall MainMenusRealmCreated(void)
{
	SaveLoadSetExtraFlag(TRUE);
	gptParams->wCharFlags |= 2;
	gptParams->bConnectMode = 2;
	sgnResult = 3;
}

static BOOL __fastcall MainMenusRealmTyped(void)
{
	const WORD *pwszRealm;
	int i;

	pwszRealm = D2WinEditBoxGetString(sgptRealmEdit);
	if (pwszRealm)
	{
		Unicode::toUtf(sgszRealm, pwszRealm, 256);
		for (i = 0; i < sgnShardListSize && sgptShardArray[i]; i++)
		{
			if (strcmp(sgptShardArray[i]->pszName, sgszRealm) == 0)
				return TRUE;
		}
		for (i = 0; i < sgnShardListSize && sgptShardArray[i]; i++)
		{
			if (MainMenusPrefixMatch(sgszRealm, sgptShardArray[i]->pszName))
				MainMenusShowShard(i, TRUE);
		}
	}
	sgszRealm[0] = 0;
	return FALSE;
}

BOOL __stdcall MainMenusOnRealmOk(_PARAMS *ptMsg)
{
	char szName[80];

	if (sghMcpConnectThread)
		return FALSE;
	if (!MainMenusRealmTyped())
		return TRUE;
	if (!sgszRealm[0])
		return TRUE;
	D2WinChildSetEnabled(sgptRealmEdit, FALSE);
	MainMenusRealmJoin();
	sghMcpConnectThread = NULL;
	if (sgfWaitCancelled)
	{
		D2WinChildSetEnabled(sgptRealmEdit, TRUE);
		MainMenusDestroyBoxChildren();
		FogErrorManagerLog("This is where i got to in Realm screen after hitting cancel");
		return TRUE;
	}
	if (D2MCPClientD2MCPGetStatus() || !D2MCPClientD2MCPIsConnected())
	{
		D2MCPClientD2MCPClose();
		switch (D2MCPClientD2MCPGetStatus())
		{
		case 2:
			MainMenusOkBox((const WORD *)D2LangStrGetStringByIndex(5211), MainMenusToCharSel);
			break;
		case 71:
			MainMenusOkBox((const WORD *)D2LangStrGetStringByIndex(5212), MainMenusToCharSel);
			break;
		case 10:
		case 11:
		case 12:
			MainMenusOkBox((const WORD *)D2LangStrGetStringByIndex(5213), MainMenusOpenClassSelect);
			break;
		default:
			MainMenusOkBox((const WORD *)D2LangStrGetStringByIndex(5356), MainMenusToCharSel);
			break;
		}
		D2WinChildSetEnabled(sgptRealmEdit, TRUE);
		return TRUE;
	}
	FogErrorManagerLog("Authorized by Shard");
	if (D2MCPClientD2MCPIsConnected())
	{
		FogErrorManagerLog("Send Create Character to Shard");
		D2MCPClientToMCPCreateCharacter(gptParams->szName, gptParams->bClass, gptParams->wCharFlags);
		MainMenusWaitBox();
		D2WinLoop(MainMenusMcpLoopCreateCharacter);
		if (sgfWaitCancelled)
		{
			MainMenusDestroyBoxChildren();
			D2WinChildSetEnabled(sgptRealmEdit, TRUE);
			return TRUE;
		}
		if (D2MCPClientD2MCPGetStatus())
		{
			Unicode awszName[100];
			FogErrorManagerLog("Error from MCP trying to create '%s' (err=%d)", gptParams->szName,
				D2MCPClientD2MCPGetStatus());
			D2MCPClientD2MCPClose();
			Unicode awszText[255];
			D2LangUnicodestrcpy(awszText, D2LangStrGetStringByIndex(5214));
			Unicode awszText2[255];
			D2LangUnicodestrcpy(awszText2, D2LangStrGetStringByIndex(5215));
			sprintf(szName, "%s", gptParams->szName);
			D2LangUnicodetoUnicode(awszName, szName, 100);
			D2LangUnicodestrcat(awszText, awszName);
			D2LangUnicodestrcat(awszText, awszText2);
			MainMenusCancelBox((const WORD *)awszText, MainMenusOpenClassSelect);
			return TRUE;
		}
		FogErrorManagerLog("Created Character '%s'", gptParams->szName);
		MainMenusRealmCreated();
	}
	D2WinQuitLoop();
	MainMenusFreeShards();
	return TRUE;
}

static BOOL __fastcall MainMenusPrefixMatch(const char *pszPrefix, const char *pszText)
{
	if (!*pszPrefix)
		return FALSE;
	while (*pszPrefix)
	{
		if (tolower(*pszPrefix) != tolower(*pszText))
			return FALSE;
		pszPrefix++;
		pszText++;
	}
	return TRUE;
}

static BOOL __stdcall MainMenusOnRealmChange(DWORD dwZero)
{
	char szRealm[1024];
	const char *pszTyped;
	const char *pszName;
	BOOL fBegins;
	int nTop;
	int nShown;
	int i;

	D2WinTextBoxSetSelectedNum(sgptShardList, -1);
	D2WinTextBoxSetTop(sgptShardList, 0);
	MainMenusShowShard(-1, FALSE);
	Unicode::toUtf(szRealm, D2WinEditBoxGetString(sgptRealmEdit), 256);
	for (i = 0; i < sgnShardListSize && sgptShardArray[i]; i++)
	{
		pszTyped = szRealm;
		pszName = sgptShardArray[i]->pszName;
		fBegins = FALSE;
		if (*pszTyped)
		{
			fBegins = TRUE;
			while (*pszTyped)
			{
				if (tolower(*pszTyped) != tolower(*pszName))
				{
					fBegins = FALSE;
					break;
				}
				pszTyped++;
				pszName++;
			}
		}
		if (fBegins)
		{
			nTop = D2WinTextBoxGetTop(sgptShardList);
			nShown = D2WinTextBoxGetVisibleLines(sgptShardList);
			if (i < nTop || i - nTop > nShown)
			{
				D2WinTextBoxSetTop(sgptShardList, i);
				D2WinTextBoxSetSelectedNum(sgptShardList, 0);
			}
			else
				D2WinTextBoxSetSelectedNum(sgptShardList, i);
			MainMenusShowShard(i, FALSE);
		}
	}
	return TRUE;
}

BOOL __stdcall MainMenusToCharSelThird(_PARAMS *ptMsg)
{
	D2SoundFastFadeAll();
	MainMenusClear();
	CharSelOpen();
	return TRUE;
}

BOOL __stdcall MainMenusOnRealmCancel(_PARAMS *ptMsg)
{
	MainMenusFreeShards();
	D2SoundFastFadeAll();
	MainMenusClear();
	CharSelOpen();
	return TRUE;
}

BOOL __stdcall MainMenusOnBoxButton(_PARAMS *ptMsg)
{
	if (gpfnPending)
		gpfnPending();
	return TRUE;
}

BOOL __stdcall MainMenusOnNewAccount(_PARAMS *ptMsg)
{
	char szRoot[1024];
	DWORD dwTotal;
	DWORD dwFree;
	DWORD dwBytes;
	DWORD dwSectors;
	const WORD *pwszAccount;
	const WORD *pwszPassword;
	DWORD i;

	pwszAccount = D2WinEditBoxGetString(sgptLoginAccount);
	pwszPassword = D2WinEditBoxGetString(sgptLoginPassword);
	Unicode::unicode2Win(BNET_ACCOUNT(gptParams), pwszAccount, 256);
	Unicode::unicode2Win(BNET_PASSWORD(gptParams), pwszPassword, 256);
	FogSafesockGetSavePath(szRoot, sizeof(szRoot));
	for (i = 0; i < sizeof(szRoot); i++)
	{
		if (szRoot[i] == '\\')
		{
			szRoot[i + 1] = 0;
			break;
		}
	}
	dwSectors = 0;
	dwBytes = 0;
	dwFree = 0;
	dwTotal = 0;
	GetDiskFreeSpaceA(szRoot, &dwSectors, &dwBytes, &dwFree, &dwTotal);
	if (!(dwFree * dwBytes * dwSectors >= 5000)) {
		MainMenusOkBox((const WORD *)D2LangStrGetStringByIndex(5149), MainMenusToCharSel);
		return TRUE; }
	return MainMenusOpenTerms();
}

BOOL __stdcall MainMenusOnNothing(_PARAMS *ptMsg)
{
	return TRUE;
}

BOOL __stdcall MainMenusOnLogin(_PARAMS *ptMsg)
{
	char szPattern[1024];
	char szPath[1024];
	D2COMINTCHARS *ptChars;
	const WORD *pwszAccount;
	const WORD *pwszPassword;
	HANDLE hFind;
	BOOL fCreate;

	if (!ptMsg)
	{
		MainMenusOpen();
		return TRUE;
	}
	if (!D2WinChildGetEnabled(sgptLoginOk))
		return FALSE;
	pwszAccount = D2WinEditBoxGetString(sgptLoginAccount);
	pwszPassword = D2WinEditBoxGetString(sgptLoginPassword);
	Unicode::unicode2Win(BNET_ACCOUNT(gptParams), pwszAccount, 256);
	Unicode::unicode2Win(BNET_PASSWORD(gptParams), pwszPassword, 256);
	if (!BNetLogin(gptParams, FALSE))
		return TRUE;
	StormSRegSaveString("Diablo II", "Last BNet", 0, BNET_ACCOUNT(gptParams));
	fCreate = FALSE;
	if (geMode == LAUNCHMODE_BATTLENET)
	{
		ptChars = gptComInt->pfnRequestCharacterList(0);
		if (!ptChars || !ptChars->nCount)
			fCreate = TRUE;
	}
	else
	{
		FogSafesockGetSavePath(szPath, sizeof(szPath));
		sprintf(szPattern, "%s*.d2s", szPath);
		{ WIN32_FIND_DATAA tFind;
		hFind = FindFirstFileA(szPattern, &tFind); }
		if (hFind != INVALID_HANDLE_VALUE)
			FindClose(hFind);
		else
			fCreate = TRUE;
	}
	if (fCreate)
	{
		MainMenusOpenClassSelect();
		return TRUE;
	}
	D2SoundFastFadeAll();
	MainMenusClear();
	CharSelOpen();
	return TRUE;
}

BOOL __stdcall MainMenusOnReturn(_PARAMS *ptMsg)
{
	MainMenusOpen();
	return TRUE;
}

BOOL __stdcall MainMenusOnNewAccountOk(_PARAMS *ptMsg)
{
	const WORD *pwszAccount;
	const WORD *pwszPassword;
	const WORD *pwszAgain;
	int nLength;

	if (!ptMsg)
	{
		MainMenusOpenLogin();
		return TRUE;
	}
	pwszAccount = D2WinEditBoxGetString(sgptLoginAccount);
	pwszPassword = D2WinEditBoxGetString(sgptLoginPassword);
	pwszAgain = D2WinEditBoxGetString(sgptLoginPasswordAgain);
	D2LangUnicodeunicode2Win(BNET_ACCOUNT(gptParams), pwszAccount, 256);
	D2LangUnicodeunicode2Win(BNET_PASSWORD(gptParams), pwszPassword, 256);
	nLength = D2LangUnicodestrlen(pwszAccount);
	if (nLength < 2) { MainMenusCancelBox((const WORD *)D2LangStrGetStringByIndex(5216), MainMenusOpenNewAccount); return TRUE; }
	if (nLength > 15) { MainMenusCancelBox((const WORD *)D2LangStrGetStringByIndex(5217), MainMenusOpenNewAccount); return TRUE; }
	if (D2LangUnicodestrcmp(pwszPassword, pwszAgain)) { MainMenusCancelBox((const WORD *)D2LangStrGetStringByIndex(5218), MainMenusOpenNewAccount); return TRUE; }
	nLength = D2LangUnicodestrlen(pwszPassword);
	if (nLength < 2) { MainMenusCancelBox((const WORD *)D2LangStrGetStringByIndex(5219), MainMenusOpenNewAccount); return TRUE; }
	if (nLength > 20) { MainMenusCancelBox((const WORD *)D2LangStrGetStringByIndex(5220), MainMenusOpenNewAccount); return TRUE; }
	MainMenusAccountConfirm();
	return TRUE;

}

BOOL __stdcall MainMenusOnToLoginScreen(_PARAMS *ptMsg)
{
	MainMenusOpenLogin();
	return TRUE;
}

BOOL __fastcall MainMenusRealmJoin(void)
{
	D2COMINTREPLY *ptReply;

	if (D2MCPClientD2MCPIsConnected())
		D2MCPClientD2MCPClose();
	if (sgszRealm[0])
	{
	ptReply = NULL;
	if (gptComInt)
		ptReply = gptComInt->pfnRequestRealmLogon(sgszRealm, "password");
	if (!gf1002dfb8)
		return TRUE;
	if (!ptReply)
	{
		void *t07 = D2LangStrGetStringByIndex(5210);
		MainMenusCancelBox((const WORD *)t07, MainMenusToCharSel);
		return TRUE;
	}
	if (ptReply->dwLength == 8)
	{
		if (ptReply->dwReason == 70)
			MainMenusCancelBox((const WORD *)D2LangStrGetStringByIndex(5133), MainMenusToCharSel);
		else
			MainMenusCancelBox((const WORD *)D2LangStrGetStringByIndex(5210), MainMenusToCharSel);
		return TRUE; }
	StormSStrCopy(gptParams->szRealm, sgszRealm, 128);
	sghMcpConnectThread = D2MCPClientD2MCPConnect(&ptReply->field_04);
	MainMenusWaitBox();
	D2WinLoop(MainMenusEventLoop);
	return FALSE;
	}
	gptParams->szRealm[0] = 0; return FALSE;
}

BOOL __stdcall MainMenusOnAccountCreate(_PARAMS *ptMsg)
{
	gfConnectBoxCancelled = FALSE;
	if (!BNetLogin(gptParams, TRUE))
	{
		if (gfConnectBoxCancelled)
			MainMenusOpenLogin();
		return TRUE;
	}
	StormSRegSaveString("Diablo II", "Last BNet", 0, BNET_ACCOUNT(gptParams));
	MainMenusDestroyBoxChildren();
	MainMenusOpenClassSelect();
	return TRUE;
}

BOOL __stdcall MainMenusOnAccountCancel(_PARAMS *ptMsg)
{
	MainMenusDestroyBoxChildren();
	MainMenusOpenLogin();
	return TRUE;
}

static void __fastcall MainMenusAccountConfirm(void)
{
	D2WINCONTROL *ptChild;

	MainMenusSkyStill();
	MainMenusCreateBoxChild(45);
	MainMenusCreateBoxChild(61);
	MainMenusCreateBoxChild(62);
	ptChild = MainMenusCreateBoxChild(57);
	D2WinTextBoxAddColorLineUnicode(ptChild, (const WORD *)D2LangStrGetStringByIndex(5227), 0, 0);
	ptChild = MainMenusCreateBoxChild(58);
	D2WinTextBoxAddColorLineUnicode(ptChild, (const WORD *)D2LangStrGetStringByIndex(5228), 0, 0);
	ptChild = MainMenusCreateBoxChild(59);
	D2WinTextBoxAddColorLineUnicode(ptChild, (const WORD *)D2LangStrGetStringByIndex(5229), 0, 0);
	ptChild = MainMenusCreateBoxChild(60);
	D2WinTextBoxAddColorLineUnicode(ptChild, (const WORD *)D2LangStrGetStringByIndex(5230), 0, 0);
}

void __fastcall MainMenusLoadClassCels(void)
{
	char szPath[260];

	if (sghCel1002ddec)
		return;
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\MediumSelButtonBlank", "DATA\\GLOBAL");
	sghCel1002de00 = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\CharacterCreate", "DATA\\GLOBAL");
	sghCel1002ddec = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\fire", "DATA\\GLOBAL");
	sghCel1002ddf0 = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\textbox", "DATA\\GLOBAL");
	sghCel1002de54 = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\clickbox", "DATA\\GLOBAL");
	sghCel1002de58 = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\amazon\\amnu1", "DATA\\GLOBAL");
	sgahCel1002de74[0] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\amazon\\amnu2", "DATA\\GLOBAL");
	sgahCel1002de74[1] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\amazon\\amfw", "DATA\\GLOBAL");
	sgahCel1002de74[2] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\amazon\\amnu3", "DATA\\GLOBAL");
	sgahCel1002de74[3] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\amazon\\ambw", "DATA\\GLOBAL");
	sgahCel1002de74[4] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\necromancer\\nenu1", "DATA\\GLOBAL");
	sgahCel1002de88[0] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\necromancer\\nenu2", "DATA\\GLOBAL");
	sgahCel1002de88[1] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\necromancer\\nefw", "DATA\\GLOBAL");
	sgahCel1002de88[2] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\necromancer\\nenu3", "DATA\\GLOBAL");
	sgahCel1002de88[3] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\necromancer\\nebw", "DATA\\GLOBAL");
	sgahCel1002de88[4] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\barbarian\\banu1", "DATA\\GLOBAL");
	sgahCel1002de9c[0] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\barbarian\\banu2", "DATA\\GLOBAL");
	sgahCel1002de9c[1] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\barbarian\\bafw", "DATA\\GLOBAL");
	sgahCel1002de9c[2] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\barbarian\\banu3", "DATA\\GLOBAL");
	sgahCel1002de9c[3] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\barbarian\\babw", "DATA\\GLOBAL");
	sgahCel1002de9c[4] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\sorceress\\sonu1", "DATA\\GLOBAL");
	sgahCel1002deb0[0] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\sorceress\\sonu2", "DATA\\GLOBAL");
	sgahCel1002deb0[1] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\sorceress\\sofw", "DATA\\GLOBAL");
	sgahCel1002deb0[2] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\sorceress\\sonu3", "DATA\\GLOBAL");
	sgahCel1002deb0[3] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\sorceress\\sobw", "DATA\\GLOBAL");
	sgahCel1002deb0[4] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\paladin\\panu1", "DATA\\GLOBAL");
	sgahCel1002dec4[0] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\paladin\\panu2", "DATA\\GLOBAL");
	sgahCel1002dec4[1] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\paladin\\pafw", "DATA\\GLOBAL");
	sgahCel1002dec4[2] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\paladin\\panu3", "DATA\\GLOBAL");
	sgahCel1002dec4[3] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\paladin\\pabw", "DATA\\GLOBAL");
	sgahCel1002dec4[4] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\sorceress\\sofws", "DATA\\GLOBAL");
	sgahCel1002df08[0] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\sorceress\\sonu3s", "DATA\\GLOBAL");
	sgahCel1002df08[1] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\sorceress\\sobws", "DATA\\GLOBAL");
	sgahCel1002df08[2] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\necromancer\\nefws", "DATA\\GLOBAL");
	sgahCel1002def4[0] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\necromancer\\nenu3s", "DATA\\GLOBAL");
	sgahCel1002def4[1] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\necromancer\\nebws", "DATA\\GLOBAL");
	sgahCel1002def4[2] = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\barbarian\\bafws", "DATA\\GLOBAL");
	sghCel1002df1c = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
	wsprintfA(szPath, "%s\\ui\\FrontEnd\\paladin\\pafws", "DATA\\GLOBAL");
	sghCel1002df30 = (HCELFILE)D2WinArchiveLoadCel(szPath, 0);
}

BOOL __stdcall MainMenusOnDifficultyCancel(_PARAMS *ptMsg)
{
	MainMenusDestroyBoxChildren();
	return TRUE;
}
