#ifndef D2RE_D2LAUNCH_H
#define D2RE_D2LAUNCH_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "D2WinTypes.h"
#include "Game.h"
#include "D2LaunchImports.h"

#define MAX_CHILDREN		64
#define NUM_CONTROLS		184

enum
{
	LAUNCHMODE_SINGLE = 0,
	LAUNCHMODE_BATTLENET = 1,
	LAUNCHMODE_TCPIP = 2,
	LAUNCHMODE_OPENBATTLENET = 3
};

typedef int (__fastcall *PFNCOMINTVALUE)(void);

struct D2COMINTCHARS;
struct D2LAUNCHERRINFO;
typedef BOOL (__fastcall *PFNCOMINTERROR)(D2LAUNCHERRINFO *errInfo);
typedef BOOL (__fastcall *PFNCOMINTGET)(DWORD *pdwValue);
typedef void (__fastcall *PFNCOMINTSTAGE)(int *pnStage);
typedef D2COMINTCHARS * (__fastcall *PFNCOMINTCHARS)(DWORD dwArg);
typedef void (__fastcall *PFNCOMINTTEXT)(const char *pszText);
typedef BOOL (__fastcall *PFNCOMINTTEXT2)(const char *pszFirst, const char *pszSecond);
typedef HANDLE (__fastcall *PFNCOMINTHANDLE)(void);
typedef void (__fastcall *PFNCOMINTVOID)(void);
struct D2COMINTREALMS;
struct D2COMINTREPLY;
typedef D2COMINTREALMS * (__fastcall *PFNCOMINTREALMS)(const char *pszText);
typedef D2COMINTREPLY * (__fastcall *PFNCOMINTREPLY)(const char *pszRealm,
	const char *pszText);
typedef DWORD (__fastcall *PFNCOMINTFETCH)(const char *pszFile, DWORD dwLength);

struct D2COMINT
{
 	PFNCOMINTHANDLE	pfnConnect;
 	PFNCOMINTTEXT2	pfnLogin;
 	PFNCOMINTVALUE	pfnLoginResult;
 	PFNCOMINTVALUE	pfnDisconnect;
 	DWORD		unobserved_10[5];
 	PFNCOMINTERROR	pfnGetError;
 	DWORD		unobserved_28[5];
 	PFNCOMINTREALMS	pfnRequestRealmList;
 	PFNCOMINTREPLY	pfnRequestRealmLogon;
 	PFNCOMINTGET	pfnApplyPatch;
 	PFNCOMINTVALUE	pfnGetProgress;
 	PFNCOMINTFETCH	pfnDownloadPatch;
 	DWORD		unobserved_50[2];
 	PFNCOMINTVALUE	pfnGetVersion;
 	PFNCOMINTTEXT2	pfnCreateAccount;
 	PFNCOMINTGET	pfnCreateAccountResult;
 	PFNCOMINTCHARS	pfnRequestCharacterList;
 	DWORD		unobserved_68;
 	PFNCOMINTTEXT	pfnDeleteCharacter;
 	PFNCOMINTVALUE	pfnIsConnected;
 	DWORD		unobserved_74;
 	PFNCOMINTVOID	pfnCancelDownload;
 	DWORD		unobserved_7c[3];
 	PFNCOMINTSTAGE	pfnGetStage;
};

struct D2LAUNCHERRINFO
{
 	DWORD		dwReason;
 	const char	*parm;
};

struct D2COMINTCHARS
{
 	DWORD		unobserved_00;
 	int			nMaxCharacters;
 	int			nCount;
 	char		szTexts[1];
};

struct D2COMINTREALMS
{
 	DWORD		unobserved_00;
 	int			nCount;
 	char		abEntries[1];
};

struct D2LAUNCHSHARDINFO
{
 	DWORD		adwInfo[7];
};

struct D2LAUNCHSHARD
{
 	D2LAUNCHSHARDINFO tInfo;
 	char		*pszName;
 	char		*pszDescription;
};

struct D2COMINTREPLY
{
 	DWORD		dwLength;
 	DWORD		field_04;
 	DWORD		dwReason;
};

#define BNET_ACCOUNT(ptParams)		((char *)&(ptParams)->abAccountLogin[0])
#define BNET_PASSWORD(ptParams)		((char *)&(ptParams)->abAccountLogin[24])

struct D2LAUNCHBAR
{
 	D2WINTEXTBOXBAR	tBar;
 	DWORD		field_0c;
};

struct D2CHARSELENTRY
{
 	char		szName[128];
 	char		szRealm[128];
 	BYTE		abGfx[16];
 	BYTE		abColors[16];
 	BYTE		bClass;
 	BYTE		unobserved_121;
 	WORD		wLevel;
 	WORD		wFlags;
 	WORD		wField126;
 	WORD		wGuildFlags;
 	char		szGuildTag[4];
 	BYTE		bEmblemIcon;
 	BYTE		bEmblemColor1;
 	BYTE		bEmblemColor2;
 	BYTE		unobserved_131[3];
 	D2COMPOSITEUNIT	*hUnit;
 	D2CHARSELENTRY	*ptNext;
};

struct SAVEHEADER;
typedef SAVEHEADER TPLAYERtSaveHeader;

#define D2CompTitleOf(eClass, nNumber, fHigher) \
	(((const char * (__stdcall *)(DWORD, DWORD, DWORD))D2WinD2CompGetTitleOfRaw)((eClass), (nNumber), \
		(fHigher)))

#define D2LangStrNewTableArchive(hArchive) \
	(((DWORD (__fastcall *)(DWORD))D2LangStrNewTableRaw)((DWORD)(hArchive)))

typedef void (__fastcall *PFNLAUNCHPENDING)(void);

enum
{
	LAUNCHCLASS_NONE = -1,
	LAUNCHCLASS_AMAZON = 0,
	LAUNCHCLASS_SORCERESS = 1,
	LAUNCHCLASS_NECROMANCER = 2,
	LAUNCHCLASS_PALADIN = 3,
	LAUNCHCLASS_BARBARIAN = 4
};

extern const char * const gszPaletteCharDat;
extern const char * const gszPaletteCharPl2;
extern const char * const gszPaletteSkyDat;
extern const char * const gszPaletteSkyPl2;

extern D2COMINT * gptComInt;
extern BOOL gfHardcoreAvailable;
extern PFNLAUNCHPENDING gpfnPending;
extern INIT_PARAMS * gptParams;
extern int geMode;
extern BOOL gfConnectBoxCancelled;
extern HCELFILE ghCelCharSelectBckg;
extern BOOL gf1002dfb8;

BOOL __fastcall BNetLoadProvider(INIT_PARAMS *ptParams);
void __fastcall FreeBattleNetProvider(void);
void __fastcall BNetSetCancel(void);
void __fastcall BNetClearCancel(void);
BOOL __fastcall BNetGetCancel(void);
BOOL __fastcall BNetLoggedIn(void);
BOOL __stdcall BNetPatchLoop(DWORD dwArg);
BOOL __stdcall BNetLoginLoop(DWORD dwArg);
BOOL __stdcall BNetNewAccountLoop(DWORD dwArg);
BOOL __fastcall BNetConnect(void);
BOOL __fastcall BNetLogin(INIT_PARAMS *ptParams, BOOL fNewAccount);
void __cdecl BNetDelete(void *pv);
void * __cdecl BNetNew(DWORD dwSize);
void __fastcall BNetLeave(void);
BOOL __stdcall BNetConnectLoop(DWORD dwArg);
void __fastcall BNetClear(void);
void __fastcall BNetConnectBox(void);
void __fastcall BNetConnectShow(void);

BOOL __stdcall CharSelWaitLoop(DWORD dwArg);
void __fastcall CharSelNameOfFile(const char *pszFile);
void __fastcall CharSelFree(void);
BOOL __fastcall CharSelNameValid(const char *pszName);
BOOL __fastcall CharSelScan(void);
BOOL __fastcall CharSelShowSelected(void);
BOOL __stdcall CharSelOnSlot(_PARAMS *ptMsg);
D2WINCONTROL * __fastcall CharSelDifficultyBox(int nTitle);
BOOL __stdcall CharSelOnOk(_PARAMS *ptMsg);
void __fastcall CharSelStart(DWORD dwUnused, BYTE bDifficulty);
BOOL __stdcall CharSelOnDifficultyNormal(_PARAMS *ptMsg);
BOOL __stdcall CharSelOnDifficultyNightmare(_PARAMS *ptMsg);
BOOL __stdcall CharSelOnDifficultyHell(_PARAMS *ptMsg);
BOOL __stdcall CharSelOnDeleteNo(_PARAMS *ptMsg);
BOOL __stdcall CharSelOnDelete(_PARAMS *ptMsg);
BOOL __stdcall CharSelOnDeleteYes(_PARAMS *ptMsg);
BOOL __stdcall CharSelOnUp(_PARAMS *ptMsg);
BOOL __stdcall CharSelOnDown(_PARAMS *ptMsg);
BOOL __stdcall CharSelOnKey(_PARAMS *ptMsg);
void __fastcall CharSelOpen(void);
void __fastcall MainMenusSetPending(PFNLAUNCHPENDING pfnPending);
void __fastcall MainMenusSetWaitMark(BOOL fUnused);
BOOL __fastcall MainMenusGetWaitMark(void);

extern D2WIN_STRUCT sgatControls[NUM_CONTROLS];
BOOL __stdcall MainMenusEventLoop(DWORD dwArg);
BOOL __stdcall MainMenusMcpLoopCreateCharacter(DWORD dwArg);
BOOL __stdcall MainMenusMcpLoopSelectCharacter(DWORD dwArg);
BOOL __stdcall MainMenusMcpLoopDeleteCharacter(DWORD dwArg);
BOOL __fastcall MainMenusLoadPalette(const char *pszDat, const char *pszPl2);
D2WINCONTROL * __fastcall MainMenusCreateBoxChild(int nControl);
void __fastcall MainMenusDestroyBoxChildren(void);
D2WINCONTROL * __fastcall MainMenusCreateChild(int nControl);
void __fastcall MainMenusClear(void);
BOOL __stdcall MainMenusOnToLogin(_PARAMS *ptMsg);
void __fastcall MainMenusTitleBox(const WORD *pwszTitle, const WORD *pwszText,
	PFNLAUNCHPENDING pfnAfter);
void __fastcall MainMenusTitleBoxLarge(const WORD *pwszTitle, const WORD *pwszText, const WORD *pwszText2,
	PFNLAUNCHPENDING pfnAfter);
void __fastcall MainMenusParmBox(const WORD *pwszTitle, const WORD *pwszText, const WORD *pwszParm,
	const WORD *pwszText2, PFNLAUNCHPENDING pfnAfter);
void __fastcall MainMenusCancelBox(const WORD *pwszText, PFNLAUNCHPENDING pfnAfter);
void __fastcall MainMenusOkBox(const WORD *pwszText, PFNLAUNCHPENDING pfnAfter);
void __fastcall MainMenusMessageBox(const WORD *pwszText, PFNLAUNCHPENDING pfnAfter);
void __fastcall MainMenusCancelWait(void);
BOOL __fastcall MainMenusWaitCancelled(void);
void __fastcall MainMenusWaitBox(void);
BOOL __stdcall MainMenusOnConnectTimer(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnLoginAgain(_PARAMS *ptMsg);
DWORD __fastcall MainMenusConnectBox(void);
void __fastcall MainMenusCloseBox(void);
void __fastcall MainMenusPatchScreen(void);
void __fastcall MainMenusPatchUpdate(void);
void __fastcall MainMenusPatchReset(void);
BOOL __fastcall MainMenusPatchCancelled(void);
BOOL __stdcall MainMenusOnPatchCancel(_PARAMS *ptMsg);
void __fastcall MainMenusOpen(void);
void __fastcall MainMenusToCharSel(void);
void __fastcall MainMenusOpenLogin(void);
BOOL __stdcall MainMenusOnTcpIpTimer(_PARAMS *ptMsg);
void __fastcall MainMenusOpenTcpIp(void);
BOOL __stdcall MainMenusOnHostHover(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnJoinHover(_PARAMS *ptMsg);
void __fastcall MainMenusOpenNewAccount(void);
BOOL __fastcall MainMenusOpenTerms(void);
BOOL __stdcall MainMenusTermsLoop(DWORD dwArg);
BOOL __stdcall MainMenusOnTermsAgree(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnTermsUp(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnTermsDown(_PARAMS *ptMsg);
void __fastcall MainMenusOpenClassSelectOld(void);
void __fastcall MainMenusOpenHardcore(void);
BOOL __stdcall MainMenusOnHardcoreOk(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnHardcoreCancel(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnNewCharOk(_PARAMS *ptMsg);
void __fastcall MainMenusOpenClassSelect(void);
BOOL __stdcall MainMenusOnHardcoreBox(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnHero(_PARAMS *ptMsg);
BOOL __fastcall MainMenusClassDescribe(BOOL fOver, int nClass, int nNameString,
	int nTextString);
BOOL __stdcall MainMenusOnHoverSorceress(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnHoverAmazon(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnHoverNecromancer(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnHoverBarbarian(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnHoverPaladin(_PARAMS *ptMsg);
void __cdecl MainMenusQuit(void);
BOOL __fastcall MainMenusFreeShards(void);
int __cdecl MainMenusCompareShards(const void *pvFirst, const void *pvSecond);
void __fastcall MainMenusCreateOnRealm(void);
BOOL __stdcall MainMenusLoop(DWORD dwArg);
int __fastcall MainMenusMain(INIT_PARAMS *ptParams);
BOOL __stdcall MainMenusOnSinglePlayer(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnOpenBattleNet(_PARAMS *ptMsg);
BOOL __fastcall MainMenusAnySave(void);
BOOL __stdcall MainMenusOnToCharSel(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnOtherMultiplayer(_PARAMS *ptMsg);
void __fastcall MainMenusCreditsEnd(void);
BOOL __fastcall MainMenusCreditsGapSides(void);
BOOL __fastcall MainMenusCreditsGapCentre(void);
BOOL __stdcall MainMenusOnCredits(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnCreditsTimer(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnCinematics(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnFilmIntro(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnFilmActTwoStart(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnFilmActThreeStart(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnFilmActFourStart(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnFilmEnd(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnCinematicsCancel(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnPlayDiscOk(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnCinematicsDiscOk(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnDiscCancel(_PARAMS *ptMsg);
BOOL __stdcall MainMenusDiscLoop(DWORD dwArg);
void __fastcall MainMenusCinematicsDiscBox(void);
void __fastcall MainMenusPlayDiscBox(void);
BOOL __cdecl MainMenusAskPlayDisc(void);
BOOL __cdecl MainMenusAskCinematicsDisc(void);
BOOL __stdcall MainMenusOnTcpIp(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnBattleNet(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnExit(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnHostGame(_PARAMS *ptMsg);
void __fastcall MainMenusJoinAnswer(const BYTE *pbMessage, int nSize);
BOOL __stdcall MainMenusJoinLoop(DWORD dwArg);
BOOL __stdcall MainMenusLoopOnce(DWORD dwArg);
BOOL __stdcall MainMenusOnJoinGame(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnJoinCancel(_PARAMS *ptMsg);
BOOL __stdcall MainMenusConnectLoop(DWORD dwArg);
BOOL __stdcall MainMenusOnJoinOk(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnCreateOnRealm(_PARAMS *ptMsg);
BOOL __fastcall MainMenusEnterRealm(const char *pszName, const char *pszRealm);
BOOL __fastcall MainMenusStart(const char *pszName, WORD wFlags, BYTE bClass,
	const char *pszRealm);
BOOL __stdcall MainMenusOnNewCharacter(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnIntro(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnTitleEnd(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnCharSelExit(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnBackToMain(_PARAMS *ptMsg);
BOOL __stdcall MainMenusToCharSelSecond(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnCreateLocal(_PARAMS *ptMsg);
BOOL __fastcall MainMenusShowShard(int nIndex, BOOL fSetEdit);
BOOL __stdcall MainMenusOnShard(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnRealmOk(_PARAMS *ptMsg);
BOOL __stdcall MainMenusToCharSelThird(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnRealmCancel(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnBoxButton(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnNewAccount(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnNothing(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnLogin(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnReturn(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnNewAccountOk(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnToLoginScreen(_PARAMS *ptMsg);
BOOL __fastcall MainMenusRealmJoin(void);
BOOL __stdcall MainMenusOnAccountCreate(_PARAMS *ptMsg);
BOOL __stdcall MainMenusOnAccountCancel(_PARAMS *ptMsg);
void __fastcall MainMenusLoadClassCels(void);

BOOL __stdcall MainMenusOnDifficultyCancel(_PARAMS *ptMsg);
void __fastcall SaveLoadNew(const char *pszName, BYTE bClass, BYTE *pbSummary,
	WORD wFlags);
void __fastcall SaveLoadSetExtraFlag(BOOL fSet);
BOOL __fastcall SaveLoadWrite(const char *pszName, const BYTE *pbExtra);
void __fastcall SaveLoadDelete(const char *pszFileName);
void __fastcall SaveLoadRead(const char *pszFileName, BYTE *pbClass, WORD *pwLevel, BOOL *pfExtra,
	BYTE *pbExtra);
void __fastcall SaveLoadWriteSummary(const char *pszName, const BYTE *pbSummary,
	const BYTE *pbExtra);
BOOL __fastcall SaveLoadSummary(const char *pszFileName, BYTE *pbSummary);
void __fastcall MainMenusBackground(void);

#endif
