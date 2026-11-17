#ifndef MATCH_D2MULTI_EXT_H
#define MATCH_D2MULTI_EXT_H
#include <windows.h>

#pragma pack(push, 1)
struct INIT_PARAMS
{
 	BYTE		bWindow;
 	BYTE		b3dfx;
 	BYTE		bOpenGL;
 	BYTE		bRave;
 	BYTE		bD3D;
 	BYTE		bPerspective;
 	BYTE		bQuality;
 	DWORD		dwGamma;
 	BYTE		bVSync;
 	DWORD		dwFrameRate;
 	DWORD		dwGameType;
 	WORD		wJoinId;
 	char		szGameName[24];
 	char		szServerIp[24];
 	char		szBattleNetIp[24];
 	char		szMcpIp[24];
 	BYTE		field_076[4];
 	BYTE		bNoPk;
 	BYTE		bOpenC;
 	BYTE		bAmazon;
 	BYTE		bPaladin;
 	BYTE		bSorceress;
 	BYTE		bNecromancer;
 	BYTE		bBarbarian;
 	BYTE		bInvincible;
 	BYTE		abAccountLogin[48];
 	char		szName[16];
 	BYTE		field_0c2[8];
 	BYTE		abSummary[256];
 	BYTE		bCTemp;
 	BYTE		bClass;
 	WORD		wCharFlags;
 	BYTE		bNoMonsters;
 	DWORD		dwMonsterClass;
 	BYTE		bMonsterInfo;
 	DWORD		dwMonsterDebug;
 	BYTE		bRare;
 	BYTE		bUnique;
 	BYTE		field_1da[2];
 	int			nAct;
 	BYTE		bNoPreload;
 	BYTE		bDirect;
 	BYTE		bLowEnd;
 	BYTE		bNoCompress;
 	WORD		wArena;
 	DWORD		dwGameFlags;
 	BYTE		field_1ea[3];
 	BYTE		bDifficulty;
 	BYTE		bLog;
 	BYTE		bMsgLog;
 	BYTE		bSafeMode;
 	BYTE		bNoSave;
 	DWORD		dwSeed;
 	BYTE		bCheats;
 	BYTE		bNoSound;
 	BYTE		bQuests;
 	BYTE		field_1f9;
 	DWORD		dwComInt;
 	BYTE		abGameToken[4];
 	char		szRealm[24];
 	BYTE		bGamePassword;
 	BYTE		field_21b[279];
 	BYTE		bSkipToBnet;
 	BYTE		bConnectMode;
 	BYTE		bShownLogo;
 	BYTE		field_335;
 	BYTE		abChannel[32];
 	BYTE		abFirstChannel[32];
};
#pragma pack(pop)

struct _STDINT
{
	int (__fastcall *pfnRun)(INIT_PARAMS *pOptions);
};

struct BNERRINFO
{
 	DWORD		dwReason;
 	const char	*pszInfo;
};

struct BNREPLY35
{
 	DWORD		dwLength;
 	BYTE		abData[20];
 	DWORD		dwRealmPort;
};

struct BNCLIENTINTERFACE
{
 	HANDLE		(__fastcall *pfnConnect)(void);
 	BOOL		(__fastcall *pfnLogin)(const char *pszFirst, const char *pszSecond);
 	BOOL		(__fastcall *pfnLoginResult)(void);
 	BOOL		(__fastcall *pfnDisconnect)(void);
 	BOOL		(__fastcall *pfnSend0E)(const char *pszLine);
 	char *		(__fastcall *pfnGetName)(void);
 	BOOL		(__fastcall *pfnSend0C)(const char *pszChannel);
 	BYTE *		(__fastcall *pfnRequest0B)(void);
 	DWORD		(__fastcall *pfnPop)(int *pnKind, BYTE *pbData);
 	BOOL		(__fastcall *pfnGetError)(BNERRINFO *errInfo);
 	BOOL		(__fastcall *pfnRequestFile)(const char *pszFile, DWORD dwKind);
 	BOOL		(__fastcall *pfnSend15)(void);
 	BOOL		(__fastcall *pfnSend1C)(const char *pszFirst, const char *pszSecond,
						const char *pszThird);
 	BYTE *		(__fastcall *pfnRequest09)(const char *pszFirst, const char *pszSecond);
 	BOOL		(__fastcall *pfnSend22)(const char *pszFirst, const char *pszSecond);
 	BYTE *		(__fastcall *pfnRequestRealmList)(const char *pszText);
 	BNREPLY35 *	(__fastcall *pfnRequest35)(const char *pszRealm, const char *pszSecond);
 	BOOL		(__fastcall *pfnApplyPatch)(int *pnStatus);
 	DWORD		(__fastcall *pfnGetProgress)(void);
 	unsigned long (__fastcall *pfnDownloadPatch)(const char *pszList, DWORD dwLength);
 	BOOL		(__fastcall *pfnEnterChat)(const char *pszFirst, const char *pszSecond,
						DWORD dwThird, const char *pszFourth);
 	BOOL		(__fastcall *pfnLeaveChannel)(void);
 	int			(__fastcall *pfnZero)(void);
 	BOOL		(__fastcall *pfnCreateAccount)(const char *pszFirst, const char *pszSecond);
 	BOOL		(__fastcall *pfnCreateAccountResult)(DWORD *pdwResult);
 	BYTE *		(__fastcall *pfnRequest37)(DWORD dwFirst);
 	void		(__fastcall *pfnSend38)(const char *pszFirst, const char *pszSecond);
 	void		(__fastcall *pfnSend39)(const char *pszText);
 	BOOL		(__fastcall *pfnIsConnected)(void);
 	BOOL		(__fastcall *pfnSend16)(DWORD dwFirst, BOOL fSecond);
 	void		(__fastcall *pfnCancelDownload)(void);
 	int			(__fastcall *pfnSend27)(const char *pszSubject, DWORD dwCount,
						const char **ppszKeys, const char **ppszValues);
 	BOOL		(__fastcall *pfnSend26)(const char *pszSubject, DWORD dwCount, DWORD dwId,
						const char **ppszKeys);
 	BOOL		(__fastcall *pfnReadUserData)(DWORD dwCount, DWORD dwId, char **ppszValues,
						DWORD *pdwSizes);
 	void		(__fastcall *pfnGetStage)(int *pState);
 	void		(__cdecl *pfnLogPrintf)(const char *pszFormat, ...);
 	BOOL		(__cdecl *pfnLogControl)(int nWhat, int nValue);
 	BOOL		(__fastcall *pfnGet1C)(DWORD *pdwResult);
 	BOOL		(__fastcall *pfnSend02)(void);
};

struct BNAD
{
 	DWORD		dwFirst;
 	DWORD		dwSecond;
 	char		*pszFile;
 	char		*pszText;
 	BYTE		*pbData;
 	DWORD		dwBytes;
};

struct D2QSSOCKET;
typedef void (__fastcall *D2MCPWORDCALLBACK)(WORD wValue, BYTE *pbData);
typedef void (__fastcall *D2MCPLADDERCALLBACK)(DWORD dwCount, DWORD dwSize, BYTE *pbEntries);
typedef void (__fastcall *D2MCPNEWSCALLBACK)(DWORD dwType, DWORD dwLength, BYTE *pbData);
struct D2MCPGAMECREATE
{
 	char		szGameName[16];
 	char		szPassword[16];
 	char		szDescription[32];
 	DWORD		dwFlags;
 	BYTE		field_44;
 	BYTE		bLevelDifference;
 	BYTE		bMaxPlayers;
};

struct D2MCPGAMEQUERY
{
 	char		field_00[20];
 	DWORD		dwFlags;
};

struct D2MCPGAMEINFO
{
 	BOOL		fTrailingData;
 	WORD		wToken;
 	WORD		unobserved_06;
 	DWORD		dwGameKey;
 	DWORD		dwElapsedSeconds;
 	BYTE		bLevel;
 	BYTE		bLevelDifference;
 	BYTE		bMaxPlayers;
 	BYTE		unobserved_13;
 	DWORD		dwNumPlayers;
 	DWORD		dwStatus;
 	char		szGameName[16];
 	char		szDescription[32];
 	WORD		awClasses[16];
 	WORD		awLevels[16];
 	char		aszPlayerNames[16][16];
};

#endif
