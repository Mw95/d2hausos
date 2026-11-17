#define D2_THIS_FILE "C:\\D2\\Source\\D2Multi\\Src\\Gate.cpp"
#include "D2Multi.h"

#define NUM_GAMES			1000
#define LIST_REFRESH_TICKS	5000
#define DOUBLECLICK_ROW_TICKS	500
#define MAX_PLAYERS			8
#define MAX_LEVEL_DIFF		99

#pragma pack(push, 1)
struct GATEGAME
{
 	BYTE		unobserved_00;
 	char		szName[16];
 	char		szDesc[32];
 	WORD		wNumPlayers;
 	WORD		wId;
 	WORD		unobserved_35;
 	DWORD		dwAddress;
};
#pragma pack(pop)

struct GATEPROFILE
{
 	char		szAge[8];
 	char		szSex[12];
 	char		szLocation[44];
 	char		szDescription[204];
};

typedef GATEGAME GATEGAMES[NUM_GAMES];
typedef char GATEINFONAME[256];
typedef char GATENUMBER[4];

static GATEGAMES sgatShown = { 0 };
static GATEGAMES sgatGames = { 0 };
static GATEINFONAME sgszInfoName = { 0 };
static DWORD sgadw10045738[2] = { 0 };
static D2WINCONTROL *sghGameList = 0;
static D2WINCONTROL *sghGameInfo = 0;
static D2WINCONTROL *sghJoinBckg = 0;
static D2WINCONTROL *sghJoinOk = 0;
static D2WINCONTROL *sghJoinCancel = 0;
static D2WINCONTROL *sghJoinName = 0;
static D2WINCONTROL *sghJoinPassword = 0;
static D2WINCONTROL *sghJoinText35 = 0;
static D2WINCONTROL *sghJoinText36 = 0;
static D2WINCONTROL *sghJoinText37 = 0;
static D2WINCONTROL *sghJoinText38 = 0;
static D2WINCONTROL *sghCreateBckg = 0;
static D2WINCONTROL *sghCreateText57 = 0;
static D2WINCONTROL *sghCreateText58 = 0;
static D2WINCONTROL *sghCreateText59 = 0;
static D2WINCONTROL *sghCreateText60 = 0;
static D2WINCONTROL *sghCreateText61 = 0;
static D2WINCONTROL *sghCreateText62 = 0;
static D2WINCONTROL *sghCreateText63 = 0;
static D2WINCONTROL *sghCreate83 = 0;
static D2WINCONTROL *sghCreate84 = 0;
static D2WINCONTROL *sghPlayersUp = 0;
static D2WINCONTROL *sghPlayersDown = 0;
static D2WINCONTROL *sghLevelUp = 0;
static D2WINCONTROL *sghLevelDown = 0;
static D2WINCONTROL *sghCreate82 = 0;
static D2WINCONTROL *sghCreateOk = 0;
static D2WINCONTROL *sghCreateName = 0;
static D2WINCONTROL *sghCreatePassword = 0;
static D2WINCONTROL *sghCreateDesc = 0;
static D2WINCONTROL *sghCreatePlayers = 0;
static D2WINCONTROL *sghCreateLevel = 0;
static D2WINCONTROL *sghLevelCheck = 0;
static D2WINCONTROL *sghCreate71 = 0;
static D2WINCONTROL *sghCreateCancel = 0;
static D2WINCONTROL *sghNormal = 0;
static D2WINCONTROL *sghNightmare = 0;
static D2WINCONTROL *sghHell = 0;
static D2WINCONTROL *sghNormalText = 0;
static D2WINCONTROL *sghNightmareText = 0;
static D2WINCONTROL *sghHellText = 0;
static D2WINCONTROL *sghProfileBckg = 0;
static D2WINCONTROL *sghProfileAccount = 0;
static D2WINCONTROL *sghProfileSex = 0;
static D2WINCONTROL *sghProfileAge = 0;
static D2WINCONTROL *sghProfileLocation = 0;
static D2WINCONTROL *sghProfileDescription = 0;
static D2WINCONTROL *sghMyProfileBckg = 0;
static D2WINCONTROL *sghMyProfileAccount = 0;
static D2WINCONTROL *sghMyProfileSex = 0;
static D2WINCONTROL *sghMyProfileAge = 0;
static D2WINCONTROL *sghMyProfileLocation = 0;
static D2WINCONTROL *sghMyProfileDescription = 0;
static D2WINCONTROL *sghMyProfileCancel = 0;
static D2WINCONTROL *sghPanelText131 = 0;
static D2WINCONTROL *sghPanelText132 = 0;
static D2WINCONTROL *sghPanelText133 = 0;
static D2WINCONTROL *sghPanelText134 = 0;
static D2WINCONTROL *sghPanelText135 = 0;
static D2WINCONTROL *sghPanelOk = 0;
static D2WINCONTROL *sghPanelText138 = 0;
static int sgnPanel = 0;
static D2WINCONTROL *sghPopupTitle = 0;
static D2WINCONTROL *sghPopupText = 0;
static D2WINCONTROL *sghPopupClose = 0;
static D2WINCONTROL *sghPopupBckg = 0;
BOOL gfPopupOpen = 0;
static int sgnShown = 0;
static int sgnGames = 0;
static int sgnDifficulty = 0;
static DWORD sgdwListTick = 0;
static DWORD sgdwLastRowTick = 0;
static DWORD sgdw10045860 = 0;

static GATENUMBER sgszLevelDiff = "4";
static GATENUMBER sgszMaxPlayers = "4";
static int sgnInfoToken = -1;
static int sgnLastRow = -2;

static BOOL __stdcall GateNameKey(D2WINEDITBOX *ptEdit, int nPos, char *pchKey);
static BOOL __stdcall GateNamePost(DWORD dwZero);
static BOOL __stdcall GateGameNameKey(D2WINEDITBOX *ptEdit, int nPos, char *pchKey);
static BOOL __stdcall GateDescKey(D2WINEDITBOX *ptEdit, int nPos, char *pchKey);
static BOOL __stdcall GatePasswordKey(D2WINEDITBOX *ptEdit, int nPos, char *pchKey);
static BOOL __stdcall GateCreateNamePost(DWORD dwZero);
static BOOL __stdcall GatePlayersPost(DWORD dwZero);
static BOOL __stdcall GateLevelPost(DWORD dwZero);
static void __fastcall GateUpdateCreateOk(void);

const char * __fastcall GateNextString(const char *pszStr)
{
#line 163
	D2ASSERT(pszStr);
	while (*pszStr)
		pszStr++;
	return pszStr + 1;
}

BOOL __fastcall GateLevelTooHigh(const char *pszLevel)
{
	BYTE bLevel;
	WORD wFlags;
	int nStep;

	bLevel = *pszLevel;
	wFlags = gptParams->wCharFlags;
	nStep = ((wFlags >> 10) & 7) + 1;
	if (wFlags & 4)
	{
		if ((int)bLevel - 51 > nStep || bLevel < 51)
			return TRUE;
		return FALSE;
	}
	if ((int)bLevel - 48 > nStep) return TRUE; return FALSE;
}

void __fastcall GateRequestList(void)
{
	BYTE *pbList;
	const char *pszText;
	GATEGAME *ptGame;
	int nIndex;

	pbList = gptComInt->pfnRequest09(NULL, NULL);
	if (pbList)
	{
		sgnGames = *(int *)pbList;
		pbList += 4;
		nIndex = 0;
		ptGame = &sgatGames[0];
		{
			while (nIndex < sgnGames)
			{
				pbList += 12;
				ptGame->wId = 1;
				ptGame->dwAddress = *(DWORD *)pbList;
				pbList += 20;
				strcpy(ptGame->szName, (const char *)pbList);
				if (sghGameList)
					D2WinTextBoxAddLineUTF(sghGameList, pbList);
				pszText = GateNextString((const char *)pbList);
				pszText = GateNextString(pszText);
				if (GateLevelTooHigh(pszText))
					sgnGames--;
				else
				{
					strcpy(ptGame->szDesc, pszText + 1);
					nIndex++;
					ptGame++;
				}
				pbList = (BYTE *)GateNextString(pszText);
			}
		}
	}
	GateShowList();

}

void __fastcall GateAddGame(D2MCPGAMEINFO *ptInfo)
{
	GATEGAME *ptGame;
	int i;
	int nGames;

	nGames = sgnGames;
	for (i = 0; i < nGames; i++)
	{
		if (sgatGames[i].wId == (WORD)ptInfo->dwGameKey)
			return;
	}
	sgatGames[nGames].wId = (WORD)ptInfo->dwGameKey;
	sgatGames[nGames].dwAddress = 0;
	sgatGames[nGames].wNumPlayers = (WORD)ptInfo->dwNumPlayers;
	strcpy(sgatGames[nGames].szName, ptInfo->szGameName);
	strcpy(sgatGames[nGames].szDesc, ptInfo->szDescription);
	sgnGames = nGames + 1;
}

void __fastcall GateOpenJoin(void)
{
	DWORD dwElapsed;

	if (ChatDlgRealmLost() == -1)
	{
		ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5139), NULL);
		return;
	}
	sghJoinBckg = (D2WINCONTROL *)D2WinCreateChild(&gatControls[34]);
	sghJoinText35 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[35]);
	D2WinTextBoxAddLineUnicode(sghJoinText35, (const WORD *)D2LangStrGetStringByIndex(5151));
	sghJoinText36 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[36]);
	D2WinTextBoxAddLineUnicode(sghJoinText36, (const WORD *)D2LangStrGetStringByIndex(5274));
	sghJoinText37 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[37]);
	D2WinTextBoxAddLineUnicode(sghJoinText37, (const WORD *)D2LangStrGetStringByIndex(5225));
	sghJoinText38 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[38]);
	D2WinTextBoxAddLineUnicode(sghJoinText38, (const WORD *)D2LangStrGetStringByIndex(5275));
	sghJoinOk = (D2WINCONTROL *)D2WinCreateChild(&gatControls[42]);
	sghJoinCancel = (D2WINCONTROL *)D2WinCreateChild(&gatControls[90]);
	D2WinSetEscapeButton(sghJoinCancel);
	sghJoinName = (D2WINCONTROL *)D2WinCreateChild(&gatControls[39]);
	D2WinEditBoxSetMaxLength(sghJoinName, 16);
	D2WinEditBoxSetKeyProc(sghJoinName, GateNameKey);
	D2WinEditBoxSetKeyPostProc(sghJoinName, GateNamePost);
	sghJoinPassword = (D2WINCONTROL *)D2WinCreateChild(&gatControls[40]);
	D2WinEditBoxSetMaxLength(sghJoinPassword, 16);
	D2WinEditBoxSetKeyProc(sghJoinPassword, GatePasswordKey);
	D2WinEditBoxSetTabNext(sghJoinName, sghJoinPassword);
	D2WinEditBoxSetTabNext(sghJoinPassword, sghJoinName);
	sghGameList = (D2WINCONTROL *)D2WinCreateChild(&gatControls[41]);
	if (D2MCPClientD2MCPIsConnected())
	{
		D2WinTextBoxSetNumColumns(sghGameList, 2);
		D2WinTextBoxSetColumnX(sghGameList, 1, 135);
	}
	sghGameInfo = (D2WINCONTROL *)D2WinCreateChild(&gatControls[47]);
	dwElapsed = GetTickCount() - sgdwListTick;
	if (dwElapsed >= LIST_REFRESH_TICKS)
	{
		sgdwListTick += dwElapsed;
		GateRefreshList();
	}
	D2WinSetKeyFocus(sghJoinName);
	sgnShown = 0;
	sgnGames = 0;
	GateNamePost(0);
}

void __fastcall GateRefreshList(void)
{
	D2MCPGAMEQUERY tQuery;
	int i;

	gnRequest = REQUEST_GAMEINFO;
	sgnGames = 0;
	if (gptParams->bConnectMode == 2)
	{
		for (i = 0; i < (int)(sizeof(tQuery) / sizeof(DWORD)); i++)
			((DWORD *)&tQuery)[i] = 0;
		if (gptParams->wCharFlags & 4)
			tQuery.dwFlags |= 2048;
		do
			gwNextToken++;
		while (!gwNextToken);
		D2MCPClientToMCPSendGameListQuery(&tQuery, gwNextToken);
		gwRequestToken = gwNextToken;
	}
	else
		GateRequestList();
}

void __fastcall GateShowList(void)
{
	struct
	{
		BOOL fSelected;
		DWORD dwTop;
		char szNumber[16];
		char szSelected[256];
	} t;
	BOOL fNoRealm;
	int nRow;
	int i;
	const WORD *pwszRow;

	if (!sghGameList)
		return;
	pwszRow = D2WinTextBoxGetSelectedLine(sghGameList);
	t.fSelected = FALSE;
	if (pwszRow)
	{
		Unicode::unicode2Win(t.szSelected, pwszRow, sizeof(t.szSelected));
		t.fSelected = TRUE;
	}
	t.dwTop = D2WinTextBoxGetTop(sghGameList);
	D2WinClearTextBox(sghGameList);
	D2WinClearTextBox(sghGameInfo);
	sgnShown = sgnGames;
	if (!sgnGames)
		return;
	fNoRealm = !D2MCPClientD2MCPIsConnected();
	memcpy(sgatShown, sgatGames, sgnGames * sizeof(GATEGAME));
	for (i = 0; i < sgnShown; i++)
	{
		D2WinTextBoxAddLineUTF(sghGameList, sgatShown[i].szName);
		nRow = D2WinTextBoxGetLastLineNum(sghGameList);
		if (!fNoRealm)
		{
			itoa(sgatShown[i].wNumPlayers, t.szNumber, 10);
			D2WinTextBoxAddColumnTextUTF(sghGameList, t.szNumber, nRow, 1);
		}
	}
	if (t.fSelected)
	{
		for (i = 0; i < sgnShown; i++)
		{
			if (!strcmp(t.szSelected, sgatShown[i].szName))
			{
				D2WinTextBoxSetSelectedNum(sghGameList, i);
				break;
			}
		}
		D2WinTextBoxSetTop(sghGameList, t.dwTop);
	}
	else
		D2WinTextBoxSetTop(sghGameList, 0);
}

static BOOL __stdcall GateNameKey(D2WINEDITBOX *ptEdit, int nPos, char *pchKey)
{
	char chKey;

	D2WinTextBoxSetSelectedNum(sghGameList, -1);
	chKey = *pchKey;
	switch (chKey)
	{
	case ' ':
	case '\'':
	case '-':
	case '_':
		return TRUE;
	}
	if (chKey >= '0' && chKey <= '9')
		return TRUE;
	if (chKey >= 'A' && chKey <= 'Z')
		return TRUE;
	if (chKey >= 'a' && chKey <= 'z')
		return TRUE;
	return FALSE;
}

static BOOL __stdcall GateNamePost(DWORD dwZero)
{
	char szName[256];
	const WORD *pwszName;
	BOOL fEnable;

	pwszName = D2WinEditBoxGetString(sghJoinName);
	fEnable = FALSE;
	if (pwszName)
	{
		Unicode::unicode2Win(szName, pwszName, 255);
		if (strlen(szName) >= 1)
			fEnable = TRUE;
	}
	D2WinChildSetEnabled(sghJoinOk, fEnable);
	return TRUE;
}

void __fastcall GateSetInfoRequest(const char *pszName, WORD wToken)
{
	strncpy(sgszInfoName, pszName, sizeof(sgszInfoName));
	sgnInfoToken = wToken;
}

void __fastcall GateRequestInfo(void)
{
	if (sgnInfoToken == -1)
		return;
	D2MCPClientToMCPSendGameDetailsQuery(sgszInfoName, sgnInfoToken);
	gwGameInfoToken = (WORD)sgnInfoToken;
	sgnInfoToken = -1;
}

BOOL __stdcall GateOnListClick(_PARAMS *ptMsg)
{
	struct
	{
		WORD wToken; WORD wPad;
		char szName[256];
	} t;
	int nRow;
	DWORD dwNow;
	const WORD *pwszRow;
	GATEGAME *ptGame;
	const char *pszName;

	nRow = D2WinTextBoxGetSelectedNum(sghGameList);
	D2WinClearTextBox(sghGameInfo);
	if (!(nRow == -1))
	{
		dwNow = GetTickCount();
		if (nRow == sgnLastRow && dwNow < sgdwLastRowTick + DOUBLECLICK_ROW_TICKS)
		{
			GateOnJoin(NULL);
			return FALSE;
		}
		if (D2MCPClientD2MCPIsConnected())
		{
			pwszRow = D2WinTextBoxGetSelectedLine(sghGameList);
			if (pwszRow)
			{
				Unicode::unicode2Win(t.szName, pwszRow, sizeof(t.szName));
				do
					gwNextToken++;
				while (!gwNextToken);
				t.wToken = gwNextToken;
				strncpy(sgszInfoName, t.szName, sizeof(sgszInfoName));
				sgnInfoToken = t.wToken;
				D2WinEditBoxSetStringUnicode(sghJoinName, pwszRow);
				D2WinSetKeyFocus(0);
			}
		}
		else if (sghGameInfo)
		{
			ptGame = &sgatGames[nRow];
			D2WinClearTextBox(sghGameInfo);
			if (strlen(ptGame->szDesc))
			{
				D2WinTextBoxAddLineUTF(sghGameInfo, ptGame->szDesc);
				D2WinTextBoxAddLineUTF(sghGameInfo, &gchBlank);
			}
			D2WinTextBoxSetTop(sghGameInfo, 0);
			pszName = ptGame->szName;
			if (pszName)
			{
				D2WinEditBoxSetStringUTF(sghJoinName, pszName);
				D2WinSetKeyFocus(0);
			} }
		sgnLastRow = nRow;
		sgdwLastRowTick = dwNow;
		return FALSE; }
	return FALSE;
}

void __fastcall GateShowInfo(D2MCPGAMEINFO *ptInfo)
{
	char szLine[256];
	DWORD dwSeconds;
	int nLow;
	int nHigh;
	int i;

	if (!sghGameInfo)
		return;
	D2WinClearTextBox(sghGameInfo);
	if (strlen(ptInfo->szDescription))
	{
		D2WinTextBoxAddLineUTF(sghGameInfo, ptInfo->szDescription);
		D2WinTextBoxAddLineUTF(sghGameInfo, &gchBlank);
	}
	dwSeconds = ptInfo->dwElapsedSeconds;
	sprintf(szLine, "Elapsed Time: %d:%02d:%02d", dwSeconds / 3600, dwSeconds % 3600 / 60, dwSeconds % 60);
	D2WinTextBoxAddLineUTF(sghGameInfo, szLine);
	if (ptInfo->bLevelDifference < 100)
	{
		if (ptInfo->bLevelDifference)
		{
			nLow = (int)ptInfo->bLevel - (int)ptInfo->bLevelDifference;
			if (nLow <= 1)
				nLow = 1;
			nHigh = (int)ptInfo->bLevelDifference + (int)ptInfo->bLevel;
			if (nHigh >= 99)
				nHigh = 99;
			sprintf(szLine, "Level %d to %d", nLow, nHigh);
		}
		else
			sprintf(szLine, "Level %d", ptInfo->bLevel);
		D2WinTextBoxAddLineUTF(sghGameInfo, szLine);
	}
	if (ptInfo->bMaxPlayers > 0 && ptInfo->bMaxPlayers < 8)
	{
		sprintf(szLine, "Up to %d Players", ptInfo->bMaxPlayers);
		D2WinTextBoxAddLineUTF(sghGameInfo, szLine);
	}
	D2WinTextBoxAddLineUTF(sghGameInfo, &gchBlank);
	for (i = 0; i < (int)ptInfo->dwNumPlayers; i++)
	{
		if (!ptInfo->aszPlayerNames[i][0])
			continue;
		D2WinTextBoxAddLineUTF(sghGameInfo, ptInfo->aszPlayerNames[i]);
		switch (ptInfo->awClasses[i])
		{
		case 0:
			sprintf(szLine, "Level %d Amazon", ptInfo->awLevels[i]);
			break;
		case 1:
			sprintf(szLine, "Level %d Sorceress", ptInfo->awLevels[i]);
			break;
		case 2:
			sprintf(szLine, "Level %d Necromancer", ptInfo->awLevels[i]);
			break;
		case 3:
			sprintf(szLine, "Level %d Paladin", ptInfo->awLevels[i]);
			break;
		case 4:
			sprintf(szLine, "Level %d Barbarian", ptInfo->awLevels[i]);
			break;
		default:
			sprintf(szLine, "Level %d", ptInfo->awLevels[i]);
			break;
		}
		D2WinTextBoxAddColorLineUTF(sghGameInfo, szLine, 5);
	}
	D2WinTextBoxSetTop(sghGameInfo, 0);
}

void __fastcall GateCloseJoin(void)
{
	D2WinDestroyChild(&sghJoinBckg);
	D2WinDestroyChild(&sghJoinName);
	D2WinDestroyChild(&sghJoinPassword);
	D2WinDestroyChild(&sghGameList);
	D2WinDestroyChild(&sghGameInfo);
	D2WinDestroyChild(&sghJoinOk);
	D2WinDestroyChild(&sghJoinCancel);
	D2WinDestroyChild(&sghJoinText35);
	D2WinDestroyChild(&sghJoinText36);
	D2WinDestroyChild(&sghJoinText37);
	D2WinDestroyChild(&sghJoinText38);
	D2WinSetEscapeButton(gahLobby[10]);
}

void __fastcall GateOpenCreate(void)
{
	int nSteps;

	if (ChatDlgRealmLost() == -1)
	{
		ChatDlgOkBox((const WORD *)D2LangStrGetStringByIndex(5139), NULL);
		return;
	}
	gptParams->bDifficulty = 0;
	sghCreateBckg = (D2WINCONTROL *)D2WinCreateChild(&gatControls[56]);
	sghCreateText57 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[57]);
	D2WinTextBoxAddLineUnicode(sghCreateText57, (const WORD *)D2LangStrGetStringByIndex(5150));
	sghCreateText58 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[58]);
	D2WinTextBoxAddLineUnicode(sghCreateText58, (const WORD *)D2LangStrGetStringByIndex(5274));
	sghCreateText59 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[59]);
	D2WinTextBoxAddLineUnicode(sghCreateText59, (const WORD *)D2LangStrGetStringByIndex(5256));
	sghCreateText60 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[60]);
	D2WinTextBoxAddLineUnicode(sghCreateText60, (const WORD *)D2LangStrGetStringByIndex(5257));
	if (D2MCPClientD2MCPIsConnected())
	{
		sghCreateText61 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[61]);
		D2WinTextBoxAddLineUnicode(sghCreateText61, (const WORD *)D2LangStrGetStringByIndex(5258));
	}
	sghCreateOk = (D2WINCONTROL *)D2WinCreateChild(&gatControls[69]);
	sghCreateCancel = (D2WINCONTROL *)D2WinCreateChild(&gatControls[90]);
	D2WinSetEscapeButton(sghCreateCancel);
	sghCreateName = (D2WINCONTROL *)D2WinCreateChild(&gatControls[64]);
	D2WinEditBoxSetMaxLength(sghCreateName, 16);
	D2WinEditBoxSetKeyProc(sghCreateName, GateGameNameKey);
	D2WinEditBoxSetKeyPostProc(sghCreateName, GateCreateNamePost);
	sghCreatePassword = (D2WINCONTROL *)D2WinCreateChild(&gatControls[65]);
	D2WinEditBoxSetMaxLength(sghCreatePassword, 16);
	D2WinEditBoxSetKeyProc(sghCreatePassword, GatePasswordKey);
	sghCreateDesc = (D2WINCONTROL *)D2WinCreateChild(&gatControls[66]);
	D2WinEditBoxSetMaxLength(sghCreateDesc, 32);
	D2WinEditBoxSetKeyProc(sghCreateDesc, GateDescKey);
	sprintf(sgszLevelDiff, "%d", 4);
	sprintf(sgszMaxPlayers, "%d", 4);
	if (D2MCPClientD2MCPIsConnected())
	{
		sghCreatePlayers = (D2WINCONTROL *)D2WinCreateChild(&gatControls[67]);
		D2WinEditBoxSetStringUTF(sghCreatePlayers, sgszMaxPlayers);
		D2WinEditBoxSetMaxLength(sghCreatePlayers, 3);
		D2WinEditBoxSetKeyProc(sghCreatePlayers, D2WinEditBoxNumericCallbackRaw);
		D2WinEditBoxSetKeyPostProc(sghCreatePlayers, GatePlayersPost);
		sghLevelCheck = (D2WINCONTROL *)D2WinCreateChild(&gatControls[70]);
		D2WinButtonSetPressed(sghLevelCheck, TRUE);
		sghCreate71 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[71]);
		sghCreateLevel = (D2WINCONTROL *)D2WinCreateChild(&gatControls[68]);
		D2WinEditBoxSetStringUTF(sghCreateLevel, sgszLevelDiff);
		D2WinEditBoxSetMaxLength(sghCreateLevel, 3);
		D2WinEditBoxSetKeyProc(sghCreateLevel, D2WinEditBoxNumericCallbackRaw);
		D2WinEditBoxSetKeyPostProc(sghCreateLevel, GateLevelPost);
		D2WinChildSetEnabled(sghCreateLevel, FALSE);
		sghCreateText62 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[62]);
		D2WinTextBoxAddColorLineUnicode(sghCreateText62, (const WORD *)D2LangStrGetStringByIndex(5259), 5,
			0);
		sghCreateText63 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[63]);
		D2WinTextBoxAddColorLineUnicode(sghCreateText63, (const WORD *)D2LangStrGetStringByIndex(5260), 5,
			0);
		sghPlayersUp = (D2WINCONTROL *)D2WinCreateChild(&gatControls[78]);
		sghPlayersDown = (D2WINCONTROL *)D2WinCreateChild(&gatControls[79]);
		sghLevelUp = (D2WINCONTROL *)D2WinCreateChild(&gatControls[80]);
		sghLevelDown = (D2WINCONTROL *)D2WinCreateChild(&gatControls[81]);
		sghCreate82 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[82]);
		D2WinButtonSetFirstCel(sghPlayersDown, 2);
		D2WinButtonSetFirstCel(sghLevelDown, 2);
		D2WinChildSetVisible(sghCreate82, TRUE);
		D2WinChildSetEnabled(sghCreate82, FALSE);
		D2WinChildSetVisible(sghLevelUp, FALSE);
		D2WinChildSetVisible(sghLevelDown, FALSE);
		D2WinChildSetEnabled(sghLevelUp, FALSE);
		D2WinChildSetEnabled(sghLevelDown, FALSE);
		D2WinEditBoxSetTabNext(sghCreateName, sghCreatePassword);
		D2WinEditBoxSetTabNext(sghCreatePassword, sghCreateDesc);
		D2WinEditBoxSetTabNext(sghCreateDesc, sghCreatePlayers);
		D2WinEditBoxSetTabNext(sghCreatePlayers, sghCreateLevel);
		D2WinEditBoxSetTabNext(sghCreateLevel, sghCreateName);
	}
	else
	{
		D2WinEditBoxSetTabNext(sghCreateName, sghCreatePassword);
		D2WinEditBoxSetTabNext(sghCreatePassword, sghCreateDesc);
		D2WinEditBoxSetTabNext(sghCreateDesc, sghCreateName);
		sghCreateText61 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[61]);
		D2WinTextBoxAddColorLineUnicode(sghCreateText61, (const WORD *)D2LangStrGetStringByIndex(5258), 5,
			0);
		sghCreateText62 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[62]);
		D2WinTextBoxAddColorLineUnicode(sghCreateText62, (const WORD *)D2LangStrGetStringByIndex(5259), 5,
			0);
		sghCreateText63 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[63]);
		D2WinTextBoxAddColorLineUnicode(sghCreateText63, (const WORD *)D2LangStrGetStringByIndex(5260), 5,
			0);
		sghCreate71 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[71]);
		D2WinChildSetVisible(sghCreate71, TRUE);
		sghCreate82 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[82]);
		D2WinChildSetVisible(sghCreate82, TRUE);
		D2WinChildSetEnabled(sghCreate82, FALSE);
		sghCreate83 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[83]);
		D2WinChildSetVisible(sghCreate83, TRUE);
		sghCreate84 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[84]);
		D2WinChildSetVisible(sghCreate84, TRUE);
		D2WinChildSetEnabled(sghCreate84, FALSE);
	}
	nSteps = (gptParams->wCharFlags >> 8) & 31;
	sghHell = (D2WINCONTROL *)D2WinCreateChild(&gatControls[74]);
	sghHellText = (D2WINCONTROL *)D2WinCreateChild(&gatControls[77]);
	sghNightmare = (D2WINCONTROL *)D2WinCreateChild(&gatControls[73]);
	sghNightmareText = (D2WINCONTROL *)D2WinCreateChild(&gatControls[76]);
	if (nSteps >= 8)
		D2WinTextBoxAddColorLineUnicode(sghHellText, (const WORD *)D2LangStrGetStringByIndex(5154), 4, 0);
	else
	{
		D2WinTextBoxAddColorLineUnicode(sghHellText, (const WORD *)D2LangStrGetStringByIndex(5154), 5, 0);
		ChatDlgGreyRadio(sghHell);
	}
	if (nSteps >= 4)
		D2WinTextBoxAddColorLineUnicode(sghNightmareText, (const WORD *)D2LangStrGetStringByIndex(5155), 4,
			0);
	else
	{
		D2WinTextBoxAddColorLineUnicode(sghNightmareText, (const WORD *)D2LangStrGetStringByIndex(5155), 5,
			0);
		ChatDlgGreyRadio(sghNightmare);
	}
	sghNormalText = (D2WINCONTROL *)D2WinCreateChild(&gatControls[75]);
	D2WinTextBoxAddColorLineUnicode(sghNormalText, (const WORD *)D2LangStrGetStringByIndex(5156), 4, 0);
	sghNormal = (D2WINCONTROL *)D2WinCreateChild(&gatControls[72]);
	D2WinButtonSetPressed(sghNormal, TRUE);
	sgnDifficulty = 0;
	D2WinSetKeyFocus(sghCreateName);
	if (D2MCPClientD2MCPIsConnected())
		GateOnLevelCheck(NULL);
}

void __fastcall GateCloseCreate(void)
{
	D2WinDestroyChild(&sghCreateBckg);
	D2WinDestroyChild(&sghCreateText57);
	D2WinDestroyChild(&sghCreateText58);
	D2WinDestroyChild(&sghCreateText59);
	D2WinDestroyChild(&sghCreateText60);
	D2WinDestroyChild(&sghCreateText61);
	D2WinDestroyChild(&sghCreateText62);
	D2WinDestroyChild(&sghCreateText63);
	D2WinDestroyChild(&sghCreateName);
	D2WinDestroyChild(&sghCreatePassword);
	D2WinDestroyChild(&sghCreateDesc);
	D2WinDestroyChild(&sghCreatePlayers);
	D2WinDestroyChild(&sghCreateLevel);
	D2WinDestroyChild(&sghCreateOk);
	D2WinDestroyChild(&sghCreateCancel);
	D2WinDestroyChild(&sghLevelCheck);
	D2WinDestroyChild(&sghNormal);
	D2WinDestroyChild(&sghNightmare);
	D2WinDestroyChild(&sghHell);
	D2WinDestroyChild(&sghNormalText);
	D2WinDestroyChild(&sghNightmareText);
	D2WinDestroyChild(&sghHellText);
	D2WinDestroyChild(&sghCreate71);
	D2WinDestroyChild(&sghPlayersUp);
	D2WinDestroyChild(&sghPlayersDown);
	D2WinDestroyChild(&sghLevelUp);
	D2WinDestroyChild(&sghLevelDown);
	D2WinDestroyChild(&sghCreate82);
	D2WinDestroyChild(&sghCreate83);
	D2WinDestroyChild(&sghCreate84);
	D2WinSetEscapeButton(gahLobby[10]);
	ChatDlgFocusChat();
}

BOOL __stdcall GateOnLevelCheck(_PARAMS *ptMsg)
{
	if (D2WinButtonIsPressed(sghLevelCheck))
	{
		D2WinChildSetVisible(sghCreate71, FALSE);
		D2WinChildSetVisible(sghCreate82, FALSE);
		D2WinChildSetVisible(sghLevelUp, TRUE);
		D2WinChildSetVisible(sghLevelDown, TRUE);
		D2WinChildSetEnabled(sghCreateLevel, TRUE);
		D2WinChildSetEnabled(sghLevelUp, TRUE);
		D2WinChildSetEnabled(sghLevelDown, TRUE);
		D2WinTextBoxAddColorLineUnicode(sghCreateText62, (const WORD *)D2LangStrGetStringByIndex(5259), 7,
			0);
		D2WinTextBoxAddColorLineUnicode(sghCreateText63, (const WORD *)D2LangStrGetStringByIndex(5260), 7,
			0);
	}
	else
	{
		D2WinChildSetVisible(sghCreate71, TRUE);
		D2WinChildSetVisible(sghCreate82, TRUE);
		D2WinChildSetVisible(sghLevelUp, FALSE);
		D2WinChildSetVisible(sghLevelDown, FALSE);
		D2WinChildSetEnabled(sghCreateLevel, FALSE);
		D2WinChildSetEnabled(sghLevelUp, FALSE);
		D2WinChildSetEnabled(sghLevelDown, FALSE);
		D2WinTextBoxAddColorLineUnicode(sghCreateText62, (const WORD *)D2LangStrGetStringByIndex(5259), 5,
			0);
		D2WinTextBoxAddColorLineUnicode(sghCreateText63, (const WORD *)D2LangStrGetStringByIndex(5260), 5,
			0);
	}
	GateUpdateCreateOk();
	return TRUE;
}

BOOL __stdcall GateOnPlayersUp(_PARAMS *ptMsg)
{
	int nValue;

	nValue = atoi(sgszMaxPlayers);
	if (nValue < MAX_PLAYERS)
		nValue++;
	sprintf(sgszMaxPlayers, "%d", nValue);
	D2WinEditBoxSetStringUTF(sghCreatePlayers, sgszMaxPlayers);
	return TRUE;
}

BOOL __stdcall GateOnPlayersDown(_PARAMS *ptMsg)
{
	int nValue;

	nValue = atoi(sgszMaxPlayers);
	if (nValue > 1)
		nValue--;
	sprintf(sgszMaxPlayers, "%d", nValue);
	D2WinEditBoxSetStringUTF(sghCreatePlayers, sgszMaxPlayers);
	return TRUE;
}

BOOL __stdcall GateOnLevelUp(_PARAMS *ptMsg)
{
	int nValue;

	nValue = atoi(sgszLevelDiff);
	if (nValue < MAX_LEVEL_DIFF)
		nValue++;
	sprintf(sgszLevelDiff, "%d", nValue);
	D2WinEditBoxSetStringUTF(sghCreateLevel, sgszLevelDiff);
	return TRUE;
}

BOOL __stdcall GateOnLevelDown(_PARAMS *ptMsg)
{
	int nValue;

	nValue = atoi(sgszLevelDiff);
	if (nValue > 0)
		nValue--;
	sprintf(sgszLevelDiff, "%d", nValue);
	D2WinEditBoxSetStringUTF(sghCreateLevel, sgszLevelDiff);
	return TRUE;
}

static BOOL __stdcall GateGameNameKey(D2WINEDITBOX *ptEdit, int nPos, char *pchKey)
{
	char chKey;

	chKey = *pchKey;
	switch (chKey)
	{
	case ' ':
	case '\'':
	case '-':
	case '_':
		return TRUE;
	}
	if (chKey >= '0' && chKey <= '9')
		return TRUE;
	if (chKey >= 'A' && chKey <= 'Z')
		return TRUE;
	if (chKey >= 'a' && chKey <= 'z')
		return TRUE;
	return FALSE;
}

static BOOL __stdcall GateDescKey(D2WINEDITBOX *ptEdit, int nPos, char *pchKey)
{
	char chKey;

	chKey = *pchKey;
	if (chKey >= ' ' && (int)chKey < 128)
		return TRUE;
	return FALSE;
}

static BOOL __stdcall GatePasswordKey(D2WINEDITBOX *ptEdit, int nPos, char *pchKey)
{
	char chKey;

	chKey = *pchKey;
	if (chKey >= ' ' && (int)chKey < 128)
		return TRUE;
	return FALSE;
}

static BOOL __stdcall GateCreateNamePost(DWORD dwZero)
{
	GateUpdateCreateOk();
	return TRUE;
}

static BOOL __stdcall GatePlayersPost(DWORD dwZero)
{
	GateUpdateCreateOk();
	return TRUE;
}

static BOOL __stdcall GateLevelPost(DWORD dwZero)
{
	GateUpdateCreateOk();
	return TRUE;
}

static void __fastcall GateUpdateCreateOk(void)
{
	struct
	{
		BOOL fName;
		char szText[256];
	} t;
	BOOL fPlayers;
	BOOL fLevel;
	const WORD *pwszText;
	int nLength;
	int nValue;

	t.fName = FALSE;
	fPlayers = FALSE;
	fLevel = FALSE;
	pwszText = D2WinEditBoxGetString(sghCreateName);
	if (pwszText)
	{
		Unicode::unicode2Win(t.szText, pwszText, 16);
		nLength = strlen(t.szText);
		if (t.szText[0] != ' ' && nLength >= 1 && t.szText[nLength - 1] != ' ')
			t.fName = TRUE;
	}
	if (D2MCPClientD2MCPIsConnected())
	{
		pwszText = D2WinEditBoxGetString(sghCreatePlayers);
		if (pwszText)
		{
			Unicode::unicode2Win(t.szText, pwszText, 3);
			if (strlen(t.szText))
			{
				fPlayers = TRUE;
				nValue = atoi(t.szText);
				if (nValue < 1 || nValue > MAX_PLAYERS)
				{
					if (nValue <= 1)
						nValue = 1;
					else if (nValue >= MAX_PLAYERS)
						nValue = MAX_PLAYERS;
					sprintf(t.szText, "%d", nValue);
					D2WinEditBoxSetStringUTF(sghCreatePlayers, t.szText);
				}
			}
		}
		if (D2WinButtonIsPressed(sghLevelCheck))
		{
			pwszText = D2WinEditBoxGetString(sghCreateLevel);
			Unicode::unicode2Win(t.szText, pwszText, 3);
			if (strlen(t.szText) && atoi(t.szText) >= 0)
				fLevel = TRUE;
		}
		else
			fLevel = TRUE;
	}
	else
	{
		fPlayers = TRUE;
		fLevel = TRUE;
	}
	D2WinChildSetEnabled(sghCreateOk, t.fName && fPlayers && fLevel);
}

BOOL __stdcall GateCreateIdle(int nPass)
{
	DWORD dwResult;

	if (gptComInt->pfnGet1C(&dwResult))
		D2WinQuitLoop();
	return TRUE;
}

BOOL __stdcall GateOnCreate(_PARAMS *ptMsg)
{
	struct
	{
		DWORD dwResult;
		char szNumber[8];
		D2MCPGAMECREATE tCreate;
		char szName[256];
		char szStat[512];
	} t;
	const WORD *pwszName;
	const WORD *pwszPassword;
	const WORD *pwszDesc;
	WORD wFlags;

	if (!D2WinChildGetEnabled(sghCreateOk))
		return FALSE;
	pwszName = D2WinEditBoxGetString(sghCreateName);
	if (!pwszName)
		return TRUE;
	Unicode::unicode2Win(t.szName, pwszName, 255);
	if (strlen(t.szName) < 1)
		return FALSE;
	pwszPassword = D2WinEditBoxGetString(sghCreatePassword);
	pwszDesc = D2WinEditBoxGetString(sghCreateDesc);
	if (D2MCPClientD2MCPIsConnected())
	{
		t.dwResult = (DWORD)D2WinEditBoxGetString(sghCreatePlayers);
		t.tCreate.field_44 = 1;
		if (D2WinButtonIsPressed(sghLevelCheck))
		{
			Unicode::unicode2Win(t.szNumber, D2WinEditBoxGetString(sghCreateLevel), 3);
			t.tCreate.bLevelDifference = (BYTE)atoi(t.szNumber);
		}
		else
			t.tCreate.bLevelDifference = 255;
		Unicode::unicode2Win(t.szNumber, (const WORD *)t.dwResult, 3);
		t.tCreate.bMaxPlayers = (BYTE)atoi(t.szNumber);
	}
	Unicode::unicode2Win(t.tCreate.szGameName, pwszName, 16);
	Unicode::unicode2Win(t.tCreate.szPassword, pwszPassword, 16);
	Unicode::unicode2Win(t.tCreate.szDescription, pwszDesc, 32);
	t.tCreate.dwFlags = 0;
	(void)FogStringCapitalize(t.tCreate.szGameName);
	(void)FogStringCapitalize(t.tCreate.szPassword);
	t.tCreate.dwFlags = (t.tCreate.dwFlags & ~28672) | (sgnDifficulty << 12);
	if (strlen(t.tCreate.szGameName) < 1)
		return FALSE;
	if (!D2MCPClientD2MCPIsConnected())
	{
		wFlags = gptParams->wCharFlags;
		t.szStat[0] = (char)(((wFlags & 4) ? 3 : 0) + ((wFlags >> 10) & 7) + '1');
		StormSStrCopy(&t.szStat[1], t.tCreate.szDescription, 2147483647);
		gptComInt->pfnSend1C(t.tCreate.szGameName, t.tCreate.szPassword, t.szStat);
		ChatDlgMessageBoxCancel((const WORD *)D2LangStrGetStringByIndex(5243), NULL);
		D2WinLoop(GateCreateIdle);
		ChatDlgDestroyPanelChildren();
		gptComInt->pfnGet1C(&t.dwResult);
		if (!t.dwResult)
		{
			ChatDlgMessageBoxCancel((const WORD *)D2LangStrGetStringByIndex(5138), NULL);
			return TRUE;
		}
	}
	ChatDlgCreateGame(&t.tCreate, FogSafesockGetLocalAddress());
	GateCloseCreate();
	ChatDlgMessageBoxCancel((const WORD *)D2LangStrGetStringByIndex(5243), NULL);
	return TRUE;
}

int __fastcall GateFindGame(const char *pszName, const char *pszPassword, GATEGAME *ptGame)
{
	BYTE *pbList;
	const char *pszText;
	int nCount;

	pbList = gptComInt->pfnRequest09(pszName, pszPassword);
	if (pbList) {
		nCount = *(int *)pbList;
		if (nCount < 1)
			return 0;
		pbList += 4 + 12;
		ptGame->wId = 1;
		ptGame->dwAddress = *(DWORD *)pbList;
		pbList += 20;
		strcpy(ptGame->szName, (const char *)pbList);
		pszText = GateNextString((const char *)pbList);
		pszText = GateNextString(pszText);
		if (GateLevelTooHigh(pszText))
			return 0;
		strcpy(ptGame->szDesc, pszText);
		GateNextString(pszText);
		return nCount; }
	return 0;
}

BOOL __stdcall GateOnJoin(_PARAMS *ptMsg)
{
	GATEGAME tGame;
	char szName[256];
	char szPassword[256];
	const WORD *pwszName;
	const WORD *pwszPassword;
	int i;

	if (!D2WinChildGetEnabled(sghJoinOk))
		return FALSE;
	pwszName = D2WinEditBoxGetString(sghJoinName);
	pwszPassword = D2WinEditBoxGetString(sghJoinPassword);
	if (!pwszName)
		return FALSE;
	Unicode::unicode2Win(szName, pwszName, sizeof(szName));
	Unicode::unicode2Win(szPassword, pwszPassword, sizeof(szPassword));
	(void)FogStringCapitalize(szName);
	(void)FogStringCapitalize(szPassword);
	if (D2MCPClientD2MCPIsConnected())
	{
		ChatDlgJoinGame(0, szName, szPassword, NULL);
		GateCloseJoin();
		ChatDlgMessageBoxCancel((const WORD *)D2LangStrGetStringByIndex(5243), NULL);
		return TRUE;
	}
	for (i = 0; i < sgnGames; i++)
	{
		if (!strcmp(sgatShown[i].szName, szName))
		{
			ChatDlgJoinGame(sgatShown[i].wId, szName, szPassword,
				FogSafesockAddressToString(sgatShown[i].dwAddress));
			GateCloseJoin();
			ChatDlgMessageBoxCancel((const WORD *)D2LangStrGetStringByIndex(5243), NULL);
			return TRUE;
		}
	}
	if (GateFindGame(szName, szPassword, &tGame))
	{
		if (!(strcmp(tGame.szName, szName))) {
			ChatDlgJoinGame(tGame.wId, szName, szPassword, FogSafesockAddressToString(tGame.dwAddress));
			GateCloseJoin();
			ChatDlgMessageBoxCancel((const WORD *)D2LangStrGetStringByIndex(5243), NULL);
			return TRUE; }
		return FALSE;
	}
	ChatDlgMessageBox((const WORD *)D2LangStrGetStringByIndex(5137), NULL);
	return FALSE;
}

BOOL __stdcall GateOnJoinButton(_PARAMS *ptMsg)
{
	ChatDlgClearPanels();
	gnRequest = REQUEST_GAMEINFO;
	GateOpenJoin();
	D2WinButtonSetPressed(gahLobby[7], FALSE);
	D2WinButtonSetPressed(gahLobby[6], FALSE);
	D2WinButtonSetPressed(gahLobby[9], FALSE);
	return TRUE;
}

BOOL __stdcall GateOnCreateButton(_PARAMS *ptMsg)
{
	ChatDlgClearPanels();
	GateOpenCreate();
	D2WinButtonSetPressed(gahLobby[7], FALSE);
	D2WinButtonSetPressed(gahLobby[8], FALSE);
	D2WinButtonSetPressed(gahLobby[9], FALSE);
	return TRUE;
}

BOOL __stdcall GateOnColumn(_PARAMS *ptMsg)
{
	return TRUE;
}

BOOL __stdcall GateOnNormal(_PARAMS *ptMsg)
{
	D2WinButtonSetPressed(sghNormal, TRUE);
	D2WinButtonSetPressed(sghNightmare, FALSE);
	D2WinButtonSetPressed(sghHell, FALSE);
	sgnDifficulty = 0;
	gptParams->bDifficulty = 0;
	return TRUE;
}

BOOL __stdcall GateOnNightmare(_PARAMS *ptMsg)
{
	D2WinButtonSetPressed(sghNormal, FALSE);
	D2WinButtonSetPressed(sghNightmare, TRUE);
	D2WinButtonSetPressed(sghHell, FALSE);
	sgnDifficulty = 1;
	gptParams->bDifficulty = 1;
	return TRUE;
}

BOOL __stdcall GateOnHell(_PARAMS *ptMsg)
{
	D2WinButtonSetPressed(sghNormal, FALSE);
	D2WinButtonSetPressed(sghNightmare, FALSE);
	D2WinButtonSetPressed(sghHell, TRUE);
	sgnDifficulty = 2;
	gptParams->bDifficulty = 2;
	return TRUE;
}

void __fastcall GateShowMyProfile(void)
{
	GATEPROFILE t;
	const char *pszAccount;

	pszAccount = (const char *)gptParams->abAccountLogin;
	D2WinButtonSetPressed(gahLobby[7], FALSE);
	D2WinButtonSetPressed(gahLobby[6], FALSE);
	D2WinButtonSetPressed(gahLobby[8], FALSE);
	D2WinButtonSetPressed(gahLobby[9], FALSE);
	if (!ChatDlgGetProfile(pszAccount, t.szSex, t.szAge, t.szLocation, t.szDescription))
		return;
	ChatDlgClearPanels();
	sgnPanel = 2;
	sghMyProfileBckg = (D2WINCONTROL *)D2WinCreateChild(&gatControls[119]);
	sghMyProfileAccount = (D2WINCONTROL *)D2WinCreateChild(&gatControls[120]);
	D2WinTextBoxAddLineUTF(sghMyProfileAccount, pszAccount);
	sghMyProfileSex = (D2WINCONTROL *)D2WinCreateChild(&gatControls[121]);
	D2WinEditBoxSetStringUTF(sghMyProfileSex, t.szSex);
	D2WinEditBoxSetMaxLength(sghMyProfileSex, 8);
	sghMyProfileAge = (D2WINCONTROL *)D2WinCreateChild(&gatControls[122]);
	D2WinEditBoxSetStringUTF(sghMyProfileAge, t.szAge);
	D2WinEditBoxSetMaxLength(sghMyProfileAge, 4);
	D2WinEditBoxSetKeyProc(sghMyProfileAge, D2WinEditBoxNumericCallbackRaw);
	sghMyProfileLocation = (D2WINCONTROL *)D2WinCreateChild(&gatControls[123]);
	D2WinEditBoxSetStringUTF(sghMyProfileLocation, t.szLocation);
	D2WinEditBoxSetMaxLength(sghMyProfileLocation, 40);
	sghMyProfileDescription = (D2WINCONTROL *)D2WinCreateChild(&gatControls[124]);
	D2WinEditBoxSetStringUTF(sghMyProfileDescription, t.szDescription);
	D2WinEditBoxSetMaxLength(sghMyProfileDescription, 200);
	sghMyProfileCancel = (D2WINCONTROL *)D2WinCreateChild(&gatControls[136]);
	D2WinSetEscapeButton(sghMyProfileCancel);
	sghPanelOk = (D2WINCONTROL *)D2WinCreateChild(&gatControls[137]);
	sghPanelText131 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[131]);
	D2WinTextBoxAddLineUnicode(sghPanelText131, (const WORD *)D2LangStrGetStringByIndex(5267));
	sghPanelText132 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[132]);
	D2WinTextBoxAddLineUnicode(sghPanelText132, (const WORD *)D2LangStrGetStringByIndex(5268));
	sghPanelText133 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[133]);
	D2WinTextBoxAddLineUnicode(sghPanelText133, (const WORD *)D2LangStrGetStringByIndex(5269));
	sghPanelText134 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[134]);
	D2WinTextBoxAddLineUnicode(sghPanelText134, (const WORD *)D2LangStrGetStringByIndex(5270));
	sghPanelText135 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[135]);
	D2WinTextBoxAddLineUnicode(sghPanelText135, (const WORD *)D2LangStrGetStringByIndex(5271));
	sghPanelText138 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[138]);
	D2WinTextBoxAddLineUnicode(sghPanelText138, (const WORD *)D2LangStrGetStringByIndex(5266));
	D2WinEditBoxSetTabNext(sghMyProfileDescription, sghMyProfileSex);
	D2WinEditBoxSetTabNext(sghMyProfileSex, sghMyProfileAge);
	D2WinEditBoxSetTabNext(sghMyProfileAge, sghMyProfileLocation);
	D2WinEditBoxSetTabNext(sghMyProfileLocation, sghMyProfileDescription);
	D2WinSetKeyFocus(sghMyProfileSex);
}

void __fastcall GateShowProfile(const char *pszAccount)
{
	GATEPROFILE tProfile;
	char *pszLine;

	D2WinButtonSetPressed(gahLobby[7], FALSE);
	D2WinButtonSetPressed(gahLobby[6], FALSE);
	D2WinButtonSetPressed(gahLobby[8], FALSE);
	D2WinButtonSetPressed(gahLobby[9], FALSE);
	if (!ChatDlgGetProfile(pszAccount, tProfile.szSex, tProfile.szAge, tProfile.szLocation,
		tProfile.szDescription))
		return;
	ChatDlgClearPanels();
	sgnPanel = 1;
	sghProfileBckg = (D2WINCONTROL *)D2WinCreateChild(&gatControls[125]);
	sghProfileAccount = (D2WINCONTROL *)D2WinCreateChild(&gatControls[126]);
	D2WinTextBoxAddLineUTF(sghProfileAccount, pszAccount);
	sghProfileSex = (D2WINCONTROL *)D2WinCreateChild(&gatControls[127]);
	D2WinTextBoxAddLineUTF(sghProfileSex, tProfile.szSex);
	sghProfileAge = (D2WINCONTROL *)D2WinCreateChild(&gatControls[128]);
	D2WinTextBoxAddLineUTF(sghProfileAge, tProfile.szAge);
	sghProfileLocation = (D2WINCONTROL *)D2WinCreateChild(&gatControls[129]);
	D2WinTextBoxAddLineUTF(sghProfileLocation, tProfile.szLocation);
	sghProfileDescription = (D2WINCONTROL *)D2WinCreateChild(&gatControls[130]);
	char szDelim[] = "\r\n";
	for (pszLine = strtok(tProfile.szDescription, szDelim); pszLine; pszLine = strtok(NULL, szDelim))
		D2WinTextBoxAddLineUTF(sghProfileDescription, pszLine);
	sghPanelOk = (D2WINCONTROL *)D2WinCreateChild(&gatControls[137]);
	D2WinSetEscapeButton(sghPanelOk);
	sghPanelText131 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[131]);
	D2WinTextBoxAddLineUnicode(sghPanelText131, (const WORD *)D2LangStrGetStringByIndex(5267));
	sghPanelText132 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[132]);
	D2WinTextBoxAddLineUnicode(sghPanelText132, (const WORD *)D2LangStrGetStringByIndex(5268));
	sghPanelText133 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[133]);
	D2WinTextBoxAddLineUnicode(sghPanelText133, (const WORD *)D2LangStrGetStringByIndex(5269));
	sghPanelText134 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[134]);
	D2WinTextBoxAddLineUnicode(sghPanelText134, (const WORD *)D2LangStrGetStringByIndex(5270));
	sghPanelText135 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[135]);
	D2WinTextBoxAddLineUnicode(sghPanelText135, (const WORD *)D2LangStrGetStringByIndex(5271));
	sghPanelText138 = (D2WINCONTROL *)D2WinCreateChild(&gatControls[138]);
	D2WinTextBoxAddLineUnicode(sghPanelText138, (const WORD *)D2LangStrGetStringByIndex(5266));
	ChatDlgFocusChat();
}

void __fastcall GateClosePanel(void)
{
	if (sgnPanel)
	{
		if (sgnPanel == 1)
		{
			D2WinDestroyChild(&sghProfileBckg);
			D2WinDestroyChild(&sghProfileAccount);
			D2WinDestroyChild(&sghProfileSex);
			D2WinDestroyChild(&sghProfileAge);
			D2WinDestroyChild(&sghProfileLocation);
			D2WinDestroyChild(&sghProfileDescription);
		}
		else
		{
			D2WinDestroyChild(&sghMyProfileBckg);
			D2WinDestroyChild(&sghMyProfileAccount);
			D2WinDestroyChild(&sghMyProfileSex);
			D2WinDestroyChild(&sghMyProfileAge);
			D2WinDestroyChild(&sghMyProfileLocation);
			D2WinDestroyChild(&sghMyProfileDescription);
			D2WinDestroyChild(&sghMyProfileCancel);
		}
		sgnPanel = 0;
		D2WinDestroyChild(&sghPanelText131);
		D2WinDestroyChild(&sghPanelText132);
		D2WinDestroyChild(&sghPanelText133);
		D2WinDestroyChild(&sghPanelText134);
		D2WinDestroyChild(&sghPanelText135);
		D2WinDestroyChild(&sghPanelOk);
		D2WinDestroyChild(&sghPanelText138);
	}
	D2WinSetEscapeButton(gahLobby[10]);
	ChatDlgFocusChat();
}

BOOL __stdcall GateOnPanelCancel(_PARAMS *ptMsg)
{
	GateClosePanel();
	return TRUE;
}

BOOL __stdcall GateOnPanelOk(_PARAMS *ptMsg)
{
	struct
	{
		const char *pszAccount;
		GATEPROFILE tProfile;
	} t;
	const WORD *pwszSex;
	const WORD *pwszAge;
	const WORD *pwszLocation;
	const WORD *pwszDescription;

	if (sgnPanel == 2)
	{
		pwszSex = D2WinEditBoxGetString(sghMyProfileSex);
		pwszAge = D2WinEditBoxGetString(sghMyProfileAge);
		pwszLocation = D2WinEditBoxGetString(sghMyProfileLocation);
		pwszDescription = D2WinEditBoxGetString(sghMyProfileDescription);
		t.pszAccount = (const char *)gptParams->abAccountLogin;
		Unicode::unicode2Win(t.tProfile.szSex, pwszSex, 8);
		Unicode::unicode2Win(t.tProfile.szAge, pwszAge, 4);
		Unicode::unicode2Win(t.tProfile.szLocation, pwszLocation, 40);
		Unicode::unicode2Win(t.tProfile.szDescription, pwszDescription, 200);
		if (!ChatDlgSetProfile(t.pszAccount, t.tProfile.szSex, t.tProfile.szAge, t.tProfile.szLocation,
			t.tProfile.szDescription))
			return TRUE;
	}
	GateClosePanel();
	return TRUE;
}

BOOL __fastcall GateOpenPopup(void)
{
	ChatDlgClearPanels();
	D2WinButtonSetPressed(gahLobby[7], FALSE);
	D2WinButtonSetPressed(gahLobby[6], FALSE);
	D2WinButtonSetPressed(gahLobby[8], FALSE);
	D2WinButtonSetPressed(gahLobby[9], FALSE);
	sghPopupBckg = (D2WINCONTROL *)D2WinCreateChild(&gatControls[139]);
	sghPopupTitle = (D2WINCONTROL *)D2WinCreateChild(&gatControls[140]);
	D2WinTextBoxAddLineUnicode(sghPopupTitle, (const WORD *)D2LangStrGetStringByIndex(686));
	sghPopupText = (D2WINCONTROL *)D2WinCreateChild(&gatControls[141]);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(687));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(688));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(689));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(690));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(691));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(692));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(693));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(694));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(695));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(696));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(697));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(698));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(699));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(700));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(701));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(702));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(703));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(704));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(705));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(706));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(707));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(708));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(709));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(710));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(711));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(712));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(713));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(714));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(715));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(716));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(717));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(718));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(719));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(720));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(721));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(722));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(723));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(724));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(725));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(726));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(727));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(728));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(729));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(730));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(731));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(732));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(733));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(734));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(735));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(736));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(737));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(738));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(739));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(740));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(741));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(742));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(743));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(744));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(745));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(746));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(747));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(748));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(749));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(750));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(751));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(752));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(753));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(754));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(755));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(756));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(757));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(758));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(759));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(760));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(761));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(762));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(763));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(764));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(765));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(766));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(767));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(768));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(769));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(770));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(771));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(772));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(773));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(774));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(775));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(776));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(777));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(778));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(779));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(780));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(781));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(782));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(783));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(784));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(785));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(786));
	D2WinTextBoxAddLineUnicode(sghPopupText, (const WORD *)D2LangStrGetStringByIndex(787));
	D2WinTextBoxAddColorLineUTF(sghPopupText, " ", 0);
	sghPopupClose = (D2WINCONTROL *)D2WinCreateChild(&gatControls[142]);
	D2WinSetEscapeButton(sghPopupClose);
	gfPopupOpen = TRUE;
	D2WinTextBoxSetTop(sghPopupText, 0);
	return TRUE;
}

BOOL __fastcall GateClosePopup(void)
{
	D2WinDestroyChild(&sghPopupBckg);
	D2WinDestroyChild(&sghPopupTitle);
	D2WinDestroyChild(&sghPopupText);
	D2WinDestroyChild(&sghPopupClose);
	gfPopupOpen = FALSE;
	D2WinSetEscapeButton(gahLobby[10]);
	return TRUE;
}
