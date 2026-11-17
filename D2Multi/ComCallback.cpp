#define D2_THIS_FILE "C:\\D2\\Source\\D2Multi\\Src\\ComCallback.cpp"
#include "D2Multi.h"

#define PRODUCT_D2DV		1144144982
#define PRODUCT_D2XP		1144145240
#define PRODUCT_D2ST		1144148820
#define PRODUCT_DRHS		1146308690
#define PRODUCT_DRTL		1146246220
#define PRODUCT_DTST		1146377044
#define PRODUCT_SSHR		1397966930
#define PRODUCT_STAR		1398030674
#define PRODUCT_SEXP		1397053520
#define PRODUCT_W2BN		1462911566
#define PRODUCT_CHAT		1128808788
#define PRODUCT_UNKNOWN		1431194446

struct D2CHATCHARNAME
{
 	char		szAccount[16];
 	char		szChar[16];
 	D2CHATCHARNAME	*ptNext;
};

struct D2CHATEVENT
{
 	DWORD		dwEvent;
 	DWORD		dwFlags;
 	DWORD		field_08;
 	DWORD		unobserved_0c[3];
 	char		szName[1];
};

typedef char COMMYNAME[16];
typedef char COMCHANNEL[128];
typedef char COMTEMPFILE[260];

char gchBlank = 0;
static COMMYNAME sgszMyName = { 0 };
static COMCHANNEL sgszChannel = { 0 };
static COMTEMPFILE sgszTempFile = { 0 };
static D2CHATUSER *sgptUsers = 0;
static D2CHATCHARNAME *sgptCharNames = 0;
static int sgnCharNames = 0;
static int sgnUserCount = 0;
static int sgnUserTop = 0;
D2CHATUSER *gptSelectedUnit = 0;
static DWORD sgdwSelectTick = 0;
static DWORD sgdwLastCommandTick = 0;
static BOOL sgfRandSeeded = 0;

static int sgnSelectedRow = -1;

static inline void __fastcall ComCallbackScrollUsersFrom(int nLines, int nOld);
static void __fastcall ComCallbackLayoutUsers(void);
static D2CHATUSER * __fastcall ComCallbackAddUser(const char *pszName, const char *pszText, DWORD dwFlags);
static BYTE __fastcall ComCallbackParseUser(D2CHATUSER *ptNode, const char *pszName, const BYTE *pbText,
	DWORD dwFlags);

static PFNCOMEVENT sgapfnEvents[NUM_COMEVENTS] =
{
	ComCallbackEventNone,
	ComCallbackEventUser,
	ComCallbackEventJoin,
	ComCallbackEventLeave,
	ComCallbackEventWhisper,
	ComCallbackEventTalk,
	ComCallbackEventInfo,
	ComCallbackEventChannel,
	ComCallbackEventFalse,
	ComCallbackEventFlags,
	ComCallbackEventWhisperSent,
	ComCallbackEventFalse,
	ComCallbackEventFalse,
	ComCallbackEventError,
	ComCallbackEventError,
	ComCallbackEventError,
	ComCallbackEventFalse,
	ComCallbackEventFalse,
	ComCallbackEventInfo,
	ComCallbackEventError,
	ComCallbackEventFalse,
	ComCallbackEventInfo,
	ComCallbackEventInfo,
	ComCallbackEventEmote,
};

BOOL __fastcall ComCallbackPump(void)
{
	struct
	{
		int nKind;
		DWORD dwWritten;
		union
		{
			BYTE abData[288];
			D2CHATEVENT tEvent;
			BNAD tAd;
		} u;
		char szTempFile[260];
		char szTempPath[260];
		char szCel[260];
	} t;
	const char *pszText;
	PFNCOMEVENT pfnEvent;
	char *pszExtension;
	HANDLE hFile;

	if (!gptComInt->pfnPop(&t.nKind, t.u.abData))
		return FALSE;
	switch (t.nKind)
	{
	case 0:
		pszText = t.u.tEvent.szName + strlen(t.u.tEvent.szName) + 1;
		pfnEvent = t.u.tEvent.dwEvent < NUM_COMEVENTS ? sgapfnEvents[t.u.tEvent.dwEvent] : NULL;
		if (pfnEvent)
			pfnEvent(t.u.tEvent.dwFlags, t.u.tEvent.field_08, t.u.tEvent.szName, pszText);
		break;
	case 2:
		StormSStrCopy(sgszMyName, t.u.abData, sizeof(sgszMyName));
		break;
	case 3:
		pszExtension = strrchr(t.u.tAd.pszFile, '.') + 1;
		if (*pszExtension == 'd')
		{
			sprintf(t.szCel, "temp");
			if (ghCelAd)
			{
				D2WinArchiveFreeCel(ghCelAd);
				ghCelAd = NULL;
			}
			ghCelAd = (HCELFILE)D2WinArchiveLoadCel(t.szCel, 0);
			D2WinImageSetCelFile(gahLobby[15], ghCelAd, 0);
			FogSafesockDeleteFile(t.szCel);
		}
		else if (*pszExtension == 's')
		{
			D2WinSmackClose(gahLobby[16]);
			if (!GetTempPathA(sizeof(t.szTempPath), t.szTempPath))
				strcpy(t.szTempPath, ".");
			if (GetTempFileNameA(t.szTempPath, "bad", 0, t.szTempFile) > 0)
			{
				DeleteFileA(sgszTempFile);
				hFile = FogSafesockCreateFile(t.szTempFile, GENERIC_WRITE, 0, 0, OPEN_ALWAYS,
					FILE_FLAG_SEQUENTIAL_SCAN, 0);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					FogSafesockWriteFile(hFile, t.u.tAd.pbData, t.u.tAd.dwBytes, &t.dwWritten,
						0);
					FogSafesockCloseHandle(hFile);
				}
				D2WinSmackOpen(gahLobby[16], t.szTempFile);
				strcpy(sgszTempFile, t.szTempFile);
			}
		}
		strncpy(gszAdText, t.u.tAd.pszText, sizeof(gszAdText));
		gdwAdId = t.u.tAd.dwFirst;
#line 801
		D2FREE2(t.u.tAd.pszFile);
#line 802
		D2FREE2(t.u.tAd.pszText);
#line 803
		D2FREE2(t.u.tAd.pbData);
		break;
	}
	return TRUE;
}

static inline void __fastcall ComCallbackScrollUsersFrom(int nLines, int nOld)
{
	int nTop;
	int nLast;
	BOOL fUp;
	BOOL fDown;

	fUp = FALSE;
	fDown = FALSE;
	if (sgnUserCount <= USERS_SHOWN)
		sgnUserTop = 0;
	else
	{
		nLast = sgnUserCount - USERS_SHOWN;
		nTop = nOld + nLines;
		nTop = nTop <= 0 ? 0 : nTop;
		sgnUserTop = nTop < nLast ? nTop : nLast;
		if (sgnUserTop > 0)
			fUp = TRUE;
		if (sgnUserTop < sgnUserCount - USERS_SHOWN)
			fDown = TRUE;
	}
	if (sgnUserTop != nOld)
		ComCallbackLayoutUsers();
	D2WinChildSetEnabled(gahLobby[11], fUp);
	D2WinChildSetEnabled(gahLobby[12], fDown);
	D2WinChildSetEnabled(gahLobby[13], fUp);
	D2WinChildSetEnabled(gahLobby[14], fDown);
}

void __fastcall ComCallbackScrollUsers(int nLines)
{
	ComCallbackScrollUsersFrom(nLines, sgnUserTop);
}

static inline void __fastcall ComCallbackDeselect(void)
{
	DWORD dwFlags;

	if (gptSelectedUnit)
	{
#line 1744
		D2ASSERT(gptSelectedUnit->hUnit);
		dwFlags = D2WinD2CompGetFlags(gptSelectedUnit->hUnit);
		D2WinD2CompSetFlags(gptSelectedUnit->hUnit, gdwInvBitMasks[10] & dwFlags);
		gptSelectedUnit = NULL;
		sgnSelectedRow = -1;
	}
}

BOOL __stdcall ComCallbackOnUserButton(_PARAMS *ptMsg)
{
	D2CHATUSER *ptCurr;
	DWORD dwTick;
	DWORD dwFlags;
	int nButton;
	int nRow;
	int i;
	BOOL fFound;

	fFound = FALSE;
	for (nButton = 0; nButton < NUM_LOBBY_BUTTONS; nButton++)
	{
		if (ptMsg->window == gahLobbyButtons[nButton])
		{
			fFound = TRUE;
			break;
		}
	}
	if (!fFound)
		return FALSE;
	nRow = sgnUserTop + nButton;
	if (ptMsg->dwMessage == WM_RBUTTONUP)
	{
		for (i = 0, ptCurr = sgptUsers; ptCurr; ptCurr = ptCurr->ptNext, i++)
		{
			if (i == nRow)
			{
				GateClosePanel();
				if (!strcmp(ptCurr->szName, (const char *)gptParams->abAccountLogin))
					GateShowMyProfile();
				else
					GateShowProfile(ptCurr->szName);
				return TRUE;
			}
		}
		return TRUE;
	}
	dwTick = GetTickCount();
	if (nRow == sgnSelectedRow)
	{
		ComCallbackDeselect();
		ComCallbackUpdateButtons();
	}
	else if (nRow < sgnUserCount)
	{
		ComCallbackDeselect();
		ComCallbackUpdateButtons();
		for (i = 0, ptCurr = sgptUsers; ptCurr; ptCurr = ptCurr->ptNext, i++)
		{
			if (i != nRow)
				continue;
#line 259
			D2ASSERT(ptCurr->hUnit);
			dwFlags = D2WinD2CompGetFlags(ptCurr->hUnit);
			D2WinD2CompSetFlags(ptCurr->hUnit, gdwBitMasks[10] | dwFlags);
			gptSelectedUnit = ptCurr;
			sgnSelectedRow = nRow;
			sgdwSelectTick = dwTick;
			ComCallbackUpdateButtons();
		}
	}
	return TRUE;
}

BOOL __stdcall ComCallbackOnSend(_PARAMS *ptMsg)
{
	char szLine[256];
	const WORD *pwszLine;

	pwszLine = D2WinEditBoxGetString(ghChatEdit);
	if (pwszLine && Unicode::strlen(pwszLine))
	{
		Unicode::unicode2Win(szLine, pwszLine, sizeof(szLine));
		return ChatDlgSendText(szLine, FALSE);
	}
	return TRUE;
}

BOOL __stdcall ComCallbackOnWhisper(_PARAMS *ptMsg)
{
	char szLine[256];
	const WORD *pwszLine;

	pwszLine = D2WinEditBoxGetString(ghChatEdit);
	if (pwszLine && Unicode::strlen(pwszLine))
	{
		Unicode::unicode2Win(szLine, pwszLine, sizeof(szLine));
		return ChatDlgSendText(szLine, TRUE);
	}
	return TRUE;
}

BOOL __stdcall ComCallbackOnSquelch(_PARAMS *ptMsg)
{
	char szName[256];
	char szLine[256];
	DWORD dwTick;

	if (gptSelectedUnit)
	{
		strcpy(szName, gptSelectedUnit->szName);
		dwTick = GetTickCount();
		if (dwTick >= sgdwLastCommandTick + 1000)
		{
			sgdwLastCommandTick = dwTick;
			sprintf(szLine, "/squelch %s", szName);
			ComCallbackDeselect();
			ComCallbackUpdateButtons();
			if (!gptComInt->pfnIsConnected())
			{
				ChatDlgMessageBox((const WORD *)D2LangStrGetStringByIndex(5351), ChatDlgOnLeave);
				return FALSE;
			}
			gptComInt->pfnSend0E(szLine);
			D2WinEditBoxSetStringUnicode(ghChatEdit, 0);
		}
	}
	return TRUE;
}

BOOL __stdcall ComCallbackOnPopup(_PARAMS *ptMsg)
{
	GateOpenPopup();
	ChatDlgFocusChat();
	return TRUE;
}

BOOL __stdcall ComCallbackOnUnsquelch(_PARAMS *ptMsg)
{
	char szName[256];
	char szLine[256];
	DWORD dwTick;

	if (gptSelectedUnit)
	{
		strcpy(szName, gptSelectedUnit->szName);
		dwTick = GetTickCount();
		if (dwTick >= sgdwLastCommandTick + 1000)
		{
			sgdwLastCommandTick = dwTick;
			sprintf(szLine, "/unsquelch %s", szName);
			ComCallbackDeselect();
			ComCallbackUpdateButtons();
			if (!gptComInt->pfnIsConnected())
			{
				ChatDlgMessageBox((const WORD *)D2LangStrGetStringByIndex(5351), ChatDlgOnLeave);
				return FALSE;
			}
			gptComInt->pfnSend0E(szLine);
			D2WinEditBoxSetStringUnicode(ghChatEdit, 0);
		}
	}
	return TRUE;
}

BOOL __stdcall ComCallbackOnEmote(_PARAMS *ptMsg)
{
	struct
	{
		char szText[256];
		char szLine[256];
	} t;
	const WORD *pwszLine;
	DWORD dwTick;

	pwszLine = D2WinEditBoxGetString(ghChatEdit);
	if (pwszLine && Unicode::strlen(pwszLine))
	{
		Unicode::unicode2Win(t.szText, pwszLine, sizeof(t.szText));
		if (t.szText[0] != '/')
		{
			dwTick = GetTickCount();
			if (dwTick >= sgdwLastCommandTick + 1000)
			{
				sgdwLastCommandTick = dwTick;
				sprintf(t.szLine, "/me %s", t.szText);
				if (!gptComInt->pfnIsConnected())
				{
					ChatDlgMessageBox((const WORD *)D2LangStrGetStringByIndex(5351), ChatDlgOnLeave);
					return FALSE;
				}
				gptComInt->pfnSend0E(t.szLine);
				D2WinEditBoxSetStringUnicode(ghChatEdit, 0);
			}
		}
	}
	return TRUE;
}

BOOL __stdcall ComCallbackOnAway(_PARAMS *ptMsg)
{
	char szLine[256];
	DWORD dwTick;

	dwTick = GetTickCount();
	if (dwTick >= sgdwLastCommandTick + 1000)
	{
		sgdwLastCommandTick = dwTick;
		sprintf(szLine, "/away Away from the keyboard.");
		if (!gptComInt->pfnIsConnected())
		{
			ChatDlgMessageBox((const WORD *)D2LangStrGetStringByIndex(5351), ChatDlgOnLeave);
			return FALSE;
		}
		gptComInt->pfnSend0E(szLine);
		D2WinEditBoxSetStringUnicode(ghChatEdit, 0);
	}
	return TRUE;
}

BOOL __fastcall ComCallbackPlayTempFile(void)
{
	if (sgszTempFile[0] && GetFileAttributesA(sgszTempFile) != INVALID_FILE_ATTRIBUTES)
	{
		D2WinSmackOpen(gahLobby[16], sgszTempFile);
		return TRUE;
	}
	return FALSE;
}

char * __fastcall ComCallbackChannelName(void)
{
	return sgszChannel;
}

BOOL __fastcall ComCallbackEventNone(DWORD dwFlags, DWORD dwArg, const char *pszName, const char *pszText)
{
	return TRUE;
}

static inline int __fastcall ComCallbackPriority(const D2CHATUSER *ptUser)
{
	if (ptUser->dwFlags & 2)
		return 4;
	if (ptUser->dwFlags & 8)
		return 3;
	if (ptUser->dwFlags & 1)
		return 2;
	if (ptUser->dwFlags & 4)
		return 1;
	return 0;
}

static inline DWORD __fastcall ComCallbackUserFlags(const D2CHATUSER *ptUser, BOOL fShown, int i)
{
	DWORD dwFlags;
	int nNumber;
	int nRank;

	dwFlags = fShown ? 15 : 0;
	if (ptUser->wCharFlags & 8)
		dwFlags |= gdwBitMasks[4];
	else
		dwFlags &= gdwInvBitMasks[4];
	if (ptUser->wCharFlags & 4)
		dwFlags |= gdwBitMasks[5];
	else
		dwFlags &= gdwInvBitMasks[5];
	nNumber = (ptUser->wCharFlags >> 8) & 31;
	if (nNumber >= 12)
		nRank = 3;
	else if (nNumber >= 8)
		nRank = 2;
	else
		nRank = nNumber >= 4;
	if (nRank & 1)
		dwFlags |= gdwBitMasks[6];
	else
		dwFlags &= gdwInvBitMasks[6];
	if (nRank & 2)
		dwFlags |= gdwBitMasks[7];
	else
		dwFlags &= gdwInvBitMasks[7];
	if (fShown && (i & 1))
		dwFlags |= gdwBitMasks[8];
	if (ptUser->dwFlags & 32)
		dwFlags |= gdwBitMasks[9];
	else
		dwFlags &= gdwInvBitMasks[9];
	if (ptUser == gptSelectedUnit)
		dwFlags |= gdwBitMasks[10];
	else
		dwFlags &= gdwInvBitMasks[10];
	return dwFlags;
}

static void __fastcall ComCallbackLayoutUsers(void)
{
	D2CHATUSER *ptSorted;
	D2CHATUSER *ptPrev;
	D2CHATUSER *ptCurr;
	D2CHATUSER *ptUser;
	int nPriority;
	BOOL fInserted;
	DWORD dwFlags;
	int nY;
	int i;

	if (sgptUsers)
	{
		ptSorted = sgptUsers;
		sgptUsers = ptSorted->ptNext;
		ptSorted->ptNext = NULL;
		while (sgptUsers)
		{
			nPriority = ComCallbackPriority(sgptUsers);
			ptPrev = NULL;
			fInserted = FALSE;
			for (ptCurr = ptSorted; ptCurr && !fInserted; )
			{
				if (nPriority > ComCallbackPriority(ptCurr))
				{
					ptUser = sgptUsers;
					sgptUsers = ptUser->ptNext;
					ptUser->ptNext = ptCurr;
					if (ptPrev)
						ptPrev->ptNext = ptUser;
					else
						ptSorted = ptUser;
					fInserted = TRUE;
				}
				else
				{
					ptPrev = ptCurr;
					ptCurr = ptCurr->ptNext;
				}
			}
			if (!fInserted)
			{
				ptUser = sgptUsers;
				ptPrev->ptNext = sgptUsers;
				sgptUsers = sgptUsers->ptNext;
				ptUser->ptNext = NULL;
			}
		}
		sgptUsers = ptSorted;
	}
	for (i = 0, ptUser = sgptUsers, nY = 70; ptUser; ptUser = ptUser->ptNext, i++)
	{
		if (i >= sgnUserTop && i < sgnUserTop + USERS_SHOWN)
		{
			D2WinD2CompSetX(ptUser->hUnit, nY);
			dwFlags = ComCallbackUserFlags(ptUser, TRUE, i);
			nY += 60;
		}
		else
			dwFlags = ComCallbackUserFlags(ptUser, FALSE, i);
		D2WinD2CompSetFlags(ptUser->hUnit, dwFlags);
	}
}

BOOL __fastcall ComCallbackEventUser(DWORD dwFlags, DWORD dwArg, const char *pszName, const char *pszText)
{
	ComCallbackAddUser(pszName, pszText, dwFlags);
	return TRUE;
}

static inline void __fastcall ComCallbackClearLook(D2CHATUSER *ptNode)
{
	int i;
	BYTE *pbCodes;
	BYTE *pbColors;

	pbCodes = ptNode->abCodes;
	pbColors = ptNode->abColors;
	for (i = 0; i < (int)sizeof(ptNode->abCodes); i++)
	{
		if (i == 1)
			pbCodes[1] = 1;
		else
			ptNode->abCodes[i] = 0;
		ptNode->abColors[i] = 255;
	}
}

static D2CHATUSER * __fastcall ComCallbackAddUser(const char *pszName, const char *pszText, DWORD dwFlags)
{
	D2CHATUSER *ptNode;
	D2CHATUSER *ptLast;
	BYTE bClass;
	int nMode;
	int nFrames;
	WORD wArg;
	int i;
	char szTitle[256];

	for (ptNode = sgptUsers; ptNode; ptNode = ptNode->ptNext)
	{
		if (strcmp(ptNode->szName, pszName))
			continue;
		ptNode->dwFlags = dwFlags;
		bClass = ComCallbackParseUser(ptNode, pszName, (const BYTE *)pszText, dwFlags);
		nMode = (ptNode->wCharFlags & 4) && !(ptNode->wCharFlags & 8) ? 1 : 5;
		D2WinD2CompReset(ptNode->hUnit, bClass, nMode, ptNode->abCodes,
			ptNode->abColors);
		ComCallbackLayoutUsers();
		return ptNode;
	}
#line 1385
	ptNode = (D2CHATUSER *)D2ALLOC2(sizeof(D2CHATUSER));
	for (i = 0; i < (int)(sizeof(D2CHATUSER) / sizeof(DWORD)); i++)
		((DWORD *)ptNode)[i] = 0;
	strcpy(ptNode->szName, pszName);
	ptNode->dwFlags = dwFlags;
	bClass = ComCallbackParseUser(ptNode, pszName, (const BYTE *)pszText, dwFlags);
	nMode = 5;
	if ((ptNode->wCharFlags & 4) && !(ptNode->wCharFlags & 8))
		nMode = 1;
	ptNode->hUnit = (D2COMPOSITEUNIT *)D2WinD2CompCreate(bClass, nMode, ptNode->abCodes,
		ptNode->abColors);
	if (!ptNode->hUnit)
	{
		ptNode->bClass = 5;
		ComCallbackClearLook(ptNode);
		ptNode->hUnit = (D2COMPOSITEUNIT *)D2WinD2CompCreate(ptNode->bClass, 5, ptNode->abCodes,
			ptNode->abColors);
	}
#line 1430
	D2ASSERT(ptNode->hUnit);
	if (ptNode->abEmblem[0])
		D2WinD2CompSetEmblem(ptNode->hUnit, ptNode->abEmblem[0], ptNode->abEmblem[1],
			ptNode->abEmblem[2]);
	D2WinD2CompSetY(ptNode->hUnit, 571);
	D2WinD2CompSetDirection(ptNode->hUnit, 0);
	if (!sgfRandSeeded)
	{
		srand(GetTickCount());
		sgfRandSeeded = TRUE;
	}
	nFrames = D2WinD2CompGetFrames(ptNode->hUnit) << 8;
	D2WinD2CompSetFrame(ptNode->hUnit, rand() % nFrames);
	Unicode awszName[17];
	Unicode awszChar[16];
	Unicode awszLast[16];
	Unicode awszFirst[32];
	Unicode::win2Unicode(awszName, ptNode->szName, 16);
	Unicode::win2Unicode(awszChar, ptNode->szCharName, 16);
	Unicode::win2Unicode(awszLast, ptNode->szBracedText, 16);
	Unicode::win2Unicode(awszFirst, ptNode->szRealmName, 32);
	wArg = 0;
	if (ptNode->dwProduct == PRODUCT_D2DV || ptNode->dwProduct == PRODUCT_D2XP
		|| ptNode->dwProduct == PRODUCT_D2ST)
		wArg = ptNode->wLevel;
	D2WinD2CompSetUserData(ptNode->hUnit, (const WORD *)awszName, (const WORD *)awszChar,
		(const WORD *)awszLast, (const WORD *)awszFirst, wArg, ptNode->dwProduct);
	if (sgptUsers)
	{
		for (ptLast = sgptUsers; ptLast->ptNext; ptLast = ptLast->ptNext)
			;
		ptLast->ptNext = ptNode;
	}
	else
		sgptUsers = ptNode;
	ComCallbackAddCharName(ptNode->szName, ptNode->szCharName);
	sgnUserCount++;
	sprintf(szTitle, "%s (%d)", sgszChannel, sgnUserCount);
	Unicode awszTitle[576];
	Unicode::win2Unicode(awszTitle, szTitle, 576);
	ChatDlgSetTitle((const WORD *)awszTitle);
	ComCallbackScrollUsersFrom(0, sgnUserTop);
	ComCallbackLayoutUsers();
	return ptNode;
}

static BOOL ComCallbackClassIsMale(int nClass)
{
	if (nClass >= 0 && (nClass <= 1 || nClass == 7))
		return FALSE;
	return TRUE;
}

static BYTE __fastcall ComCallbackParseUser(D2CHATUSER *ptNode, const char *pszName, const BYTE *pbText,
	DWORD dwFlags)
{
	int nDescriptionStrLen;
	BOOL fStats;
	BYTE bIcon;
	char *pch;
	int nClass;
	int i;

	bIcon = 5;
	nDescriptionStrLen = (int)strlen((const char *)pbText);
	ptNode->wCharFlags = 0;
	fStats = FALSE;

	if (dwFlags & 2)
		bIcon = 15;
	else if (dwFlags & 8)
		bIcon = 16;
	else if (dwFlags & 1)
		bIcon = 14;
	else if (dwFlags & 4)
		bIcon = 19;
	else if (dwFlags & 5120)
		bIcon = 17;
	else if (nDescriptionStrLen < 4)
		bIcon = 5;
	else
		fStats = TRUE;
	if ((bIcon == 15 || fStats) && nDescriptionStrLen >= 4)
	{
		ptNode->dwProduct = *(const DWORD *)pbText;
		nDescriptionStrLen -= 4;
		pbText += 4;
		switch (ptNode->dwProduct)
		{
		case PRODUCT_DRHS:
		case PRODUCT_DRTL:
		case PRODUCT_DTST:
			if (fStats)
				bIcon = 8;
			break;
		case PRODUCT_SSHR:
		case PRODUCT_STAR:
			if (fStats)
				bIcon = 11;
			break;
		case PRODUCT_SEXP:
			if (fStats)
				bIcon = 12;
			break;
		case PRODUCT_W2BN:
			if (fStats)
				bIcon = 13;
			break;
		case PRODUCT_CHAT:
			if (fStats)
				bIcon = 18;
			break;
		case PRODUCT_D2DV:
		case PRODUCT_D2XP:
		case PRODUCT_D2ST:
			if (!*pbText)
			{
				if (bIcon == 5)
					bIcon = 20;
				ptNode->dwProduct = PRODUCT_UNKNOWN;
				strcpy(ptNode->szCharName, ptNode->szName);
				break;
			}
#line 1173
			D2ASSERT(nDescriptionStrLen > 0);
			for (pch = ptNode->szRealmName; *pbText != ','; )
				*pch++ = *pbText++;
			pbText++;
			*pch = 0;
			for (pch = ptNode->szCharName; *pbText != ','; )
				*pch++ = *pbText++;
			pbText++;
			*pch = 0;
			FogD2QSSocketReadSevenBitPair(pbText);
			pbText += 2;
			for (i = 0; i < (int)sizeof(ptNode->abCodes); i++)
				ptNode->abCodes[i] = *pbText++;
			if (ptNode->abCodes[0])
			{
				ptNode->bClass = *pbText++ - 1;
				for (i = 0; i < (int)sizeof(ptNode->abColors); i++)
					ptNode->abColors[i] = *pbText++;
				ptNode->wLevel = *pbText++;
				ptNode->wCharFlags = (WORD)FogD2QSSocketReadSevenBitPair(pbText);
				pbText += 2;
				if (fStats)
				{
					if ((ptNode->wCharFlags & 12) == 12)
					{
						nClass = ptNode->bClass;
						bIcon = ComCallbackClassIsMale(nClass) ? 6 : 7;
					}
					else
						bIcon = ptNode->bClass;
				}
				ptNode->field_6a = (WORD)FogD2QSSocketReadSevenBitPair(pbText);
				pbText += 2;
				ptNode->abEmblem[0] = *pbText++;
				ptNode->abEmblem[0] = ptNode->abEmblem[0] == 255 ? 0 : ptNode->abEmblem[0];
				ptNode->abEmblem[1] = *pbText++;
				ptNode->abEmblem[1] = ptNode->abEmblem[1] == 255 ? 0 : ptNode->abEmblem[1];
				ptNode->abEmblem[2] = *pbText++;
				ptNode->abEmblem[2] = ptNode->abEmblem[2] == 255 ? 0 : ptNode->abEmblem[2];
				strcpy(ptNode->szBracedText, (const char *)pbText);
				if (fStats)
					return bIcon;
				break;
			}
		default:
			if (fStats)
			{
				ptNode->dwProduct = PRODUCT_UNKNOWN;
				bIcon = 5;
				strcpy(ptNode->szCharName, ptNode->szName);
			}
			break;
		}
	}
	ComCallbackClearLook(ptNode);
	return bIcon;
}

BOOL __fastcall ComCallbackEventJoin(DWORD dwFlags, DWORD dwArg, const char *pszName, const char *pszText)
{
	D2CHATUSER *ptUser;
	char szLine[500];

	ptUser = ComCallbackAddUser(pszName, pszText, dwFlags);
	if (gfNotify)
	{
		if (strlen(ptUser->szCharName))
			sprintf(szLine, "%s has joined the channel.", ptUser->szCharName);
		else
			sprintf(szLine, "%s has joined the channel.", pszName);

		Unicode awszLine[576];

		Unicode::win2Unicode(awszLine, szLine, 576);
		ChatDlgAddLineTwin((const WORD *)awszLine, 5);
	}
	return TRUE;
}

void __fastcall ComCallbackFreeUsers(void)
{
	D2CHATUSER *ptUser;
	D2CHATUSER *ptNext;
	D2CHATCHARNAME *ptName;
	D2CHATCHARNAME *ptNextName;
	char szTitle[256];

	ComCallbackDeselect();
	ComCallbackUpdateButtons();
	for (ptUser = sgptUsers; ptUser; ptUser = ptNext)
	{
		ptNext = ptUser->ptNext;
		D2WinD2CompFree(ptUser->hUnit, TRUE);
		ptUser->hUnit = NULL;
#line 1682
		D2FREE2(ptUser);
	}
	sgptUsers = NULL;
	for (ptName = sgptCharNames; ptName; ptName = ptNextName)
	{
		ptNextName = ptName->ptNext;
#line 1690
		D2FREE2(ptName);
	}
	sgptCharNames = NULL;
	sgnCharNames = 0;
	sgnUserCount = 0;
	sprintf(szTitle, "%s (%d)", sgszChannel, sgnUserCount);
	Unicode awszTitle[576];
	Unicode::win2Unicode(awszTitle, szTitle, 576);
	ChatDlgSetTitle((const WORD *)awszTitle);
	ComCallbackScrollUsersFrom(0, 0);
}

BOOL __fastcall ComCallbackEventLeave(DWORD dwFlags, DWORD dwArg, const char *pszName, const char *pszText)
{
	D2CHATUSER *ptUser;
	D2CHATUSER *ptPrev;
	void (__fastcall *pfnUpdate)(void);
	const char *pszChar;
	struct
	{
		char szTitle[256];
		char szLine[500];
	} t;

	ptPrev = NULL;
	for (ptUser = sgptUsers; ptUser; ptPrev = ptUser, ptUser = ptUser->ptNext)
		if (!strcmp(ptUser->szName, pszName))
			break;
	if (ptUser)
	{
		if (gfNotify)
		{
			pszChar = ptUser->szCharName;
			if (pszChar && strlen(pszChar))
				sprintf(t.szLine, "%s has left the channel.", pszChar);
			else
				sprintf(t.szLine, "%s has left the channel.", ptUser->szName);
			Unicode awszLine[576];

			Unicode::win2Unicode(awszLine, t.szLine, 576);
			ChatDlgAddLineTwin((const WORD *)awszLine, 5);
		}
		if (ptPrev)
			ptPrev->ptNext = ptUser->ptNext;
		else
			sgptUsers = ptUser->ptNext;
		if (gptSelectedUnit == ptUser)
		{
			ComCallbackDeselect();
			pfnUpdate = ComCallbackUpdateButtons;
			pfnUpdate();
		}
		D2WinD2CompFree(ptUser->hUnit, TRUE);
		ptUser->hUnit = NULL;
#line 1646
		D2FREE2(ptUser);
	}
	sgnUserCount--;
	sprintf(t.szTitle, "%s (%d)", sgszChannel, sgnUserCount);
	{
		Unicode awszLine[576];

		Unicode::win2Unicode(awszLine, t.szTitle, 576);
		ChatDlgSetTitle((const WORD *)awszLine);
	}
	ComCallbackScrollUsersFrom(0, sgnUserTop);
	ComCallbackLayoutUsers();
	return TRUE;
}

BOOL __fastcall ComCallbackEventChannel(DWORD dwFlags, DWORD dwArg, const char *pszName, const char *pszText)
{
	char szLine[500];

	sprintf(szLine, "You have joined channel: %s", pszText);
	Unicode awszLine[576];
	int i;
	Unicode::win2Unicode(awszLine, szLine, 576);
	ChatDlgAddLineTwin((const WORD *)awszLine, 5);
	strcpy(sgszChannel, pszText);
	strcpy((char *)gptParams->abChannel, sgszChannel);
	if (!gptParams->abFirstChannel[0])
		strcpy((char *)gptParams->abFirstChannel, sgszChannel);
	for (i = 0; i < NUM_LOBBY_BUTTONS; i++)
		D2WinChildSetEnabled(gahLobbyButtons[i], TRUE);
	D2WinChildSetVisible(ghUserList, FALSE);
	ComCallbackFreeUsers();
	return TRUE;
}

BOOL __fastcall ComCallbackEventFlags(DWORD dwFlags, DWORD dwArg, const char *pszName, const char *pszText)
{
	D2CHATUSER *ptUser;
	BYTE bClass;
	int nMode;

	for (ptUser = sgptUsers; ptUser; ptUser = ptUser->ptNext)
	{
		if (strcmp(ptUser->szName, pszName))
			continue;
		ptUser->dwFlags = dwFlags;
		bClass = ComCallbackParseUser(ptUser, pszName, (const BYTE *)pszText, dwFlags);
		nMode = (ptUser->wCharFlags & 4) && !(ptUser->wCharFlags & 8) ? 1 : 5;
		D2WinD2CompReset(ptUser->hUnit, bClass, nMode, ptUser->abCodes,
			ptUser->abColors);
		ComCallbackLayoutUsers();
		return TRUE;
	}
	return FALSE;
}

BOOL __fastcall ComCallbackEventText(DWORD dwFlags, DWORD dwArg, const char *pszName, const char *pszText)
{
	Unicode awszLine[576];

	Unicode::win2Unicode(awszLine, pszText, 576);
	ChatDlgAddLineTwin((const WORD *)awszLine, 0);
	return TRUE;
}

static inline void __fastcall ComCallbackShowLine(const char *pszWho, const char *pszText, int nColor,
	int nTextColor)
{
	Unicode awszLine[576];
	Unicode awszText[576];

	Unicode::win2Unicode((WORD *)awszLine, pszWho, 576);
	ComCallbackPrependColor((WORD *)awszLine, nColor);
	ComCallbackAppendColor((WORD *)awszLine, nTextColor);
	Unicode::win2Unicode((WORD *)awszText, pszText, 576);
	Unicode::strcat((WORD *)awszLine, (WORD *)awszText);
	ChatDlgAddLineTwin((const WORD *)awszLine, nTextColor);
}

void __fastcall ComCallbackEchoLine(const char *pszLine)
{
	const char *pszMe;
	const char *pszTitle;
	const char *pszChar;
	D2CHATUSER *ptUser;
	int nColor;
	int nTextColor;
	char szWho[256];

	pszMe = gptComInt->pfnGetName();
	nColor = 4;
	nTextColor = 0;
	for (ptUser = sgptUsers; ptUser; ptUser = ptUser->ptNext)
	{
		if (strcmp(ptUser->szName, pszMe))
			continue;
		if (ptUser->dwFlags & 2)
			nColor = 0;
		else if (ptUser->dwFlags & 1)
		{
			nColor = 3;
			nTextColor = 3;
		}
		break;
	}
	pszChar = NULL;
	pszTitle = NULL;
	ComCallbackGetNames(pszMe, &pszTitle, &pszChar);
	if (pszChar && strlen(pszChar))
	{
		if (pszTitle && strlen(pszTitle))
			sprintf(szWho, "<%s%s> ", pszTitle, pszChar);
		else
			sprintf(szWho, "<%s> ", pszChar);
	}
	else
		sprintf(szWho, "<%s> ", pszMe);
	ComCallbackShowLine(szWho, pszLine, nColor, nTextColor);
}

void __fastcall ComCallbackAddCharName(const char *pszAccount, const char *pszChar)
{
	D2CHATCHARNAME *ptName;
	D2CHATCHARNAME *ptPrev;

	ptPrev = NULL;
	for (ptName = sgptCharNames; ptName; ptPrev = ptName, ptName = ptName->ptNext)
	{
		if (!strcmp(ptName->szAccount, pszAccount))
			break;
	}
	if (ptName)
	{
		strcpy(ptName->szChar, pszChar);
		return;
	}
	if (sgnCharNames >= MAX_CHARNAMES)
	{
		ptName = sgptCharNames;
		sgptCharNames = ptName->ptNext;
#line 1942
		D2FREE2(ptName);
		sgnCharNames--;
	}
#line 1946
	ptName = (D2CHATCHARNAME *)D2ALLOC2(sizeof(D2CHATCHARNAME));
	strcpy(ptName->szAccount, pszAccount);
	strcpy(ptName->szChar, pszChar);
	ptName->ptNext = NULL;
	if (ptPrev)
		ptPrev->ptNext = ptName;
	else
		sgptCharNames = ptName;
	sgnCharNames++;
}

void __fastcall ComCallbackGetNames(const char *pszAccount, const char **ppszTitle, const char **ppszChar)
{
	D2CHATCHARNAME *ptName;
	D2CHATUSER *ptUser;
	DWORD dwFlags;

	*ppszTitle = NULL;
	*ppszChar = NULL;
	for (ptName = sgptCharNames; ptName; ptName = ptName->ptNext)
		if (!stricmp(ptName->szAccount, pszAccount))
			break;
	if (!ptName)
		return;
	for (ptUser = sgptUsers; ptUser; ptUser = ptUser->ptNext)
		if (!stricmp(ptUser->szName, pszAccount))
			break;
	if (ptUser)
	{
		dwFlags = D2WinD2CompGetFlags(ptUser->hUnit);
		if ((gdwBitMasks[6] & dwFlags) || (gdwBitMasks[7] & dwFlags))
			*ppszTitle = D2WinD2CompGetTitle(ptUser->bClass,
				((dwFlags >> 5) & 1) * 3 + ((dwFlags >> 7) & 1) * 2 + ((dwFlags >> 6) & 1));
	}
	*ppszChar = ptName->szChar;
}

BOOL __fastcall ComCallbackEventTalk(DWORD dwFlags, DWORD dwArg, const char *pszName, const char *pszText)
{
	const char *pszChar;
	const char *pszTitle;
	D2CHATUSER *ptUser;
	int nColor;
	int nTextColor;

	pszChar = NULL;
	pszTitle = NULL;
	ComCallbackGetNames(pszName, &pszTitle, &pszChar);
	nColor = 4;
	nTextColor = 0;
	for (ptUser = sgptUsers; ptUser; ptUser = ptUser->ptNext)
	{
		if (strcmp(ptUser->szName, pszName))
			continue;
		if (ptUser->dwFlags & 2)
			nColor = 0;
		else if (ptUser->dwFlags & 1)
		{
			nColor = 3;
			nTextColor = 3;
		}
		break;
	}
	if (pszChar && strlen(pszChar))
	{
		char szPrefix[256];
		char szWho[256];

		if (pszTitle && strlen(pszTitle))
			sprintf(szWho, "%s%s", pszTitle, pszChar);
		else
			strcpy(szWho, pszChar);
		sprintf(szPrefix, "<%s> ", szWho);
		ComCallbackShowLine(szPrefix, pszText, nColor, nTextColor);
	}
	else
	{
		char szPrefix[256];

		sprintf(szPrefix, "<%s> ", pszName);
		ComCallbackShowLine(szPrefix, pszText, nColor, nTextColor);
	}
	return TRUE;
}

BOOL __fastcall ComCallbackTrue(void)
{
	return TRUE;
}

BOOL __fastcall ComCallbackEventInfo(DWORD dwFlags, DWORD dwArg, const char *pszName, const char *pszText)
{
	Unicode awszLine[576];

	Unicode::win2Unicode(awszLine, pszText, 576);
	ChatDlgAddLineTwin((const WORD *)awszLine, 3);
	return TRUE;
}

BOOL __fastcall ComCallbackEventError(DWORD dwFlags, DWORD dwArg, const char *pszName, const char *pszText)
{
	char szLine[500];

	sprintf(szLine, "%s %s", pszText,
		"If you are having problems using Battle.net commands, please click the Help button.");
	Unicode awszLine[576];
	Unicode::win2Unicode(awszLine, szLine, 576);
	ChatDlgAddLineTwin((const WORD *)awszLine, 1);
	return TRUE;
}

BOOL __fastcall ComCallbackEventEmote(DWORD dwFlags, DWORD dwArg, const char *pszName, const char *pszText)
{
	const char *pszChar;
	const char *pszTitle;
	char szWho[256];
	char szLine[500];

	pszChar = NULL;
	pszTitle = NULL;
	ComCallbackGetNames(pszName, &pszTitle, &pszChar);
	if (pszChar && strlen(pszChar))
	{
		if (pszTitle && strlen(pszTitle))
			sprintf(szWho, "%s%s", pszTitle, pszChar);
		else
			strcpy(szWho, pszChar);
		sprintf(szLine, "%s %s", szWho, pszText);
		{
			Unicode awszLine[576];
			Unicode::win2Unicode(awszLine, szLine, 576);
			ChatDlgAddLineTwin((const WORD *)awszLine, 3);
		}
	}
	else
	{
		sprintf(szLine, "%s %s", pszName, pszText);
		{
			Unicode awszLine[576];
			Unicode::win2Unicode(awszLine, szLine, 576);
			ChatDlgAddLineTwin((const WORD *)awszLine, 3);
		}
	}
	return TRUE;
}

static inline void __fastcall ComCallbackWhisperLine(char *pszLine, const char *pszWho, const char *pszName,
	const char *pszText)
{
	strcpy(gszReplyName, pszName);
	if (pszWho)
		sprintf(pszLine, "%s (%s) whispers: %s", pszWho, pszName, pszText);
	else
		sprintf(pszLine, "(%s) whispers: %s", pszName, pszText);
}

BOOL __fastcall ComCallbackEventWhisper(DWORD dwFlags, DWORD dwArg, const char *pszName, const char *pszText)
{
	const char *pszChar;
	const char *pszTitle;
	char szWho[256];
	char szLine[500];

	pszChar = NULL;
	pszTitle = NULL;
	ComCallbackGetNames(pszName, &pszTitle, &pszChar);
	if (pszChar && strlen(pszChar))
	{
		if (pszTitle && strlen(pszTitle))
			sprintf(szWho, "%s%s", pszTitle, pszChar);
		else
			strcpy(szWho, pszChar);
		ComCallbackWhisperLine(szLine, szWho, pszName, pszText);
		{
			Unicode awszLine[576];
			Unicode::win2Unicode(awszLine, szLine, 576);
			ChatDlgAddLineTwin((const WORD *)awszLine, 2);
		}
	}
	else
	{
		ComCallbackWhisperLine(szLine, NULL, pszName, pszText);
		{
			Unicode awszLine[576];
			Unicode::win2Unicode(awszLine, szLine, 576);
			ChatDlgAddLineTwin((const WORD *)awszLine, 2);
		}
	}
	return TRUE;
}

BOOL __fastcall ComCallbackEventWhisperSent(DWORD dwFlags, DWORD dwArg, const char *pszName,
	const char *pszText)
{
	const char *pszChar;
	const char *pszTitle;
	char szWho[256];
	char szLine[500];

	pszChar = NULL;
	pszTitle = NULL;
	ComCallbackGetNames(pszName, &pszTitle, &pszChar);
	if (pszChar && strlen(pszChar))
	{
		if (pszTitle && strlen(pszTitle))
			sprintf(szWho, "%s%s", pszTitle, pszChar);
		else
			strcpy(szWho, pszChar);
		sprintf(szLine, "You whisper to %s: %s", szWho, pszText);
		{
			Unicode awszLine[576];
			Unicode::win2Unicode(awszLine, szLine, 576);
			ChatDlgAddLineTwin((const WORD *)awszLine, 2);
		}
	}
	else
	{
		sprintf(szLine, "You whisper to %s: %s", pszName, pszText);
		{
			Unicode awszLine[576];
			Unicode::win2Unicode(awszLine, szLine, 576);
			ChatDlgAddLineTwin((const WORD *)awszLine, 2);
		}
	}
	return TRUE;
}

BOOL __fastcall ComCallbackEventFalse(DWORD dwFlags, DWORD dwArg, const char *pszName, const char *pszText)
{
	return FALSE;
}

void __fastcall ComCallbackUpdateButtons(void)
{
	const WORD *pwszLine;
	BOOL fLine;
	DWORD dwSquelched;

	pwszLine = D2WinEditBoxGetString(ghChatEdit);
	fLine = FALSE;
	if (pwszLine && Unicode::strlen(pwszLine))
		fLine = TRUE;
	D2WinChildSetEnabled(gahLobby[5], fLine);
	D2WinChildSetEnabled(gahLobby[0], fLine);
	D2WinChildSetEnabled(gahLobby[1], gptSelectedUnit && fLine);
	dwSquelched = 0;
	if (gptSelectedUnit && gptSelectedUnit->hUnit)
	{
		DWORD dwFlags = D2WinD2CompGetFlags(gptSelectedUnit->hUnit);
		dwSquelched = gdwBitMasks[9] & dwFlags;
	}
	D2WinChildSetEnabled(gahLobby[3], gptSelectedUnit && !dwSquelched);
	D2WinChildSetEnabled(gahLobby[4], gptSelectedUnit && dwSquelched);
}

void __fastcall ComCallbackAppendColor(WORD *pszSrc, int nColor)
{
	char szColor[3];
	Unicode awszCode[512];
	Unicode awszColor[3];

	Unicode::strcpy(awszCode, (const WORD *)D2LangStrGetStringByIndex(3994));
	wsprintfA(szColor, "%d", nColor);
	Unicode::toUnicode(awszColor, szColor, 2);
	Unicode::strcat(awszCode, awszColor);
	if (!pszSrc[0])
		Unicode::strcpy(pszSrc, awszCode);
	else
	{
#line 2778
		D2ASSERT(Unicode::strlen(pszSrc) < 500);
		Unicode::strcat(pszSrc, awszCode);
	}
}

void __fastcall ComCallbackPrependColor(WORD *pszSrc, int nColor)
{
	char szColor[3];
	Unicode awszCode[512];
	Unicode awszColor[3];

	Unicode::strcpy(awszCode, (const WORD *)D2LangStrGetStringByIndex(3994));
	wsprintfA(szColor, "%d", nColor);
	Unicode::toUnicode(awszColor, szColor, 2);
	Unicode::strcat(awszCode, awszColor);
	if (!pszSrc[0])
		Unicode::strcpy(pszSrc, awszCode);
	else
	{
#line 2808
		D2ASSERT(Unicode::strlen(pszSrc) < 500);
		Unicode::strcat(awszCode, pszSrc);
		Unicode::strcpy(pszSrc, awszCode);
	}
}
