#define D2_THIS_FILE "C:\\D2\\Source\\D2Launch\\Src\\CharSel.cpp"
#include "D2Launch.h"
#include "D2Macros.h"

static BOOL __fastcall CharSelAdd(char *pszName, const BYTE *pbSummary);
static void __fastcall CharSelFill(void);

typedef D2WINCONTROL *D2LAUNCHSLOTS[5];
typedef char D2LAUNCHNAME[128];

static D2WINCONTROL * sgptFrameNew = 0;
static D2WINCONTROL * sgptUp = 0;
static D2WINCONTROL * sgptSelName = 0;
static D2WINCONTROL * sgptFrame = 0;
static D2WINCONTROL * sgptOk = 0;
static D2WINCONTROL * sgptSelClass = 0;
static D2WINCONTROL * sgptSelRealm = 0;
static D2WINCONTROL * sgptDown = 0;
static D2WINCONTROL * sgptDelete = 0;
static D2WINCONTROL * sgptNewImage = 0;
static D2LAUNCHSLOTS sgaptSlot = { 0 };
static D2LAUNCHNAME sgszName = { 0 };
static BOOL sgfOkDisabled = 0;
BOOL gfHardcoreAvailable = 0;
static D2CHARSELENTRY * sgptCharList = 0;
static D2CHARSELENTRY * sgptSelected = 0;
static int sgnCharEntries = 0;
static int sgnTop = 0;
static BOOL sgfNoFreeSlots = 0;
static int sgnFreeSlots = 0;
static int sgnScanned = 0;
static HANDLE sgh1002da68 = 0;
static DWORD sgdwLastClick = 0;

WORD gawRefused[] =
{
	77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91,
	92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106,
	107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 0
};

#define NUM_REFUSED_REALMS	3
char gaszRefusedRealm[NUM_REFUSED_REALMS][128] =
{
	"Testv104", "FatRealm", "BetaWest"
};
static int sgnSelected = -1;

BOOL __stdcall CharSelWaitLoop(DWORD dwArg)
{
	if (WaitForSingleObject(sgh1002da68, 0) == WAIT_OBJECT_0)
		D2WinQuitLoop();
	return TRUE;
}

void __fastcall CharSelNameOfFile(const char *pszFile)
{
	char *pszName;

	pszName = sgszName;
	while (*pszFile != '\0' && *pszFile != '.')
		*pszName++ = *pszFile++;
	*pszName = '\0';
}

void __fastcall CharSelFree(void)
{
	char szName[256];
	D2CHARSELENTRY *ptEntry;
	D2CHARSELENTRY *ptNext;
	D2COMPOSITEUNIT *hUnit;

	ptEntry = sgptCharList;
	while (ptEntry)
	{
		hUnit = ptEntry->hUnit;
		ptNext = ptEntry->ptNext;
		D2WinD2CompFree(hUnit, TRUE);
#line 555
		D2FREE2(ptEntry);
		ptEntry = ptNext;
	}
	sgptCharList = NULL;
	sgnCharEntries = 0;
	sgnTop = 0;
	sgfNoFreeSlots = FALSE;
	if (sgptSelected)
	{
		(void)D2LangUnicodetoUtf(szName, (const WORD *)D2LangStrGetStringByIndex(5145), 256);
		if (strcmp(sgptSelected->szName, szName) != 0)
			D2WinD2CompFree(sgptSelected->hUnit, TRUE);
#line 572
		D2FREE2(sgptSelected);
		sgptSelected = NULL;
	}
}

BOOL __fastcall CharSelNameValid(const char *pszName)
{
	int nLength;

	nLength = strlen(pszName);
	if (nLength < 2 || nLength >= 16 || pszName[nLength - 1] == '-' || pszName[0] == '_')
		return FALSE;
	return TRUE;
}

BOOL __fastcall CharSelScan(void)
{
	char szPattern[1024];
	char szSave[1024];
	BYTE abSummary[256];
	WIN32_FIND_DATAA tFind;
	HANDLE hFind;

	sgnScanned = 0;
	FogSafesockGetSavePath(szSave, 1024);
	sprintf(szPattern, "%s*.d2s", szSave);
	hFind = FindFirstFileA(szPattern, &tFind);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		CharSelNameOfFile(tFind.cFileName);
		if (CharSelNameValid(sgszName) && SaveLoadSummary(sgszName, abSummary))
		{
			CharSelAdd(sgszName, abSummary);
			sgnScanned++;
		}
		while (FindNextFileA(hFind, &tFind))
		{
			CharSelNameOfFile(tFind.cFileName);
			if (CharSelNameValid(sgszName) && SaveLoadSummary(sgszName, abSummary))
			{
				CharSelAdd(sgszName, abSummary);
				sgnScanned++;
			}
		}
		FindClose(hFind);
	}
	return TRUE;
}

static __inline BOOL __cdecl CharSelClassTest(int nClass)
{
	return nClass < 0 || (nClass > 1 && nClass != 7);
}

static BOOL __fastcall CharSelAdd(char *pszName, const BYTE *pbSummary)
{
	char szText[128];
	D2CHARSELENTRY *ptNode;
	D2CHARSELENTRY *ptLast;
	char *pszComma;
	DWORD eMode;
	BYTE bClass;
	WORD wFlags;
	int i;

#line 367
	ptNode = (D2CHARSELENTRY *)D2ALLOC2(sizeof(D2CHARSELENTRY));
	pszComma = strchr(pszName, ',');
	if (pszComma)
	{
		*pszComma = '\0';
		StormSStrCopy(ptNode->szRealm, pszName, 2147483647);
		pszComma++;
		StormSStrCopy(ptNode->szName, pszComma, 2147483647);
	}
	else
	{
		ptNode->szRealm[0] = '\0';
		StormSStrCopy(ptNode->szName, pszName, 2147483647);
	}
	ptNode->wField126 = (WORD)FogD2QSSocketReadSevenBitPair(pbSummary);
	pbSummary += 2;
	for (i = 0; gawRefused[i] != 0; i++)
	{
		if (ptNode->wField126 == gawRefused[i])
		{
			if (ptNode->szRealm[0] != '\0')
			{
				if (MainMenusEnterRealm(ptNode->szName, ptNode->szRealm))
				{
					sprintf(szText, "%s,%s", ptNode->szRealm, ptNode->szName);
					gptComInt->pfnDeleteCharacter(szText);
					SaveLoadDelete(ptNode->szName);
				}
				MainMenusDestroyBoxChildren();
			}
			return FALSE;
		}
	}
	for (i = 0; i < NUM_REFUSED_REALMS; i++)
	{
		if (StormSStrCmp(ptNode->szRealm, gaszRefusedRealm[i], 128) == 0)
		{
			sprintf(szText, "%s,%s", ptNode->szRealm, ptNode->szName);
			gptComInt->pfnDeleteCharacter(szText);
			SaveLoadDelete(ptNode->szName);
			return FALSE;
		}
	}
	for (i = 0; i < 16; i++)
		ptNode->abGfx[i] = *pbSummary++;
	ptNode->bClass = (BYTE)(*pbSummary++ - 1);
	for (i = 0; i < 16; i++)
		ptNode->abColors[i] = *pbSummary++;
	ptNode->ptNext = NULL;
	ptNode->wLevel = *pbSummary++;
	ptNode->wFlags = (WORD)FogD2QSSocketReadSevenBitPair(pbSummary);
	pbSummary += 2;
	ptNode->wGuildFlags = (WORD)FogD2QSSocketReadSevenBitPair(pbSummary);
	pbSummary += 2;
	ptNode->bEmblemIcon = *pbSummary++;
	ptNode->bEmblemIcon = ptNode->bEmblemIcon == 255 ? 0 : ptNode->bEmblemIcon;
	ptNode->bEmblemColor1 = *pbSummary++;
	ptNode->bEmblemColor1 = ptNode->bEmblemColor1 == 255 ? 0 : ptNode->bEmblemColor1;
	ptNode->bEmblemColor2 = *pbSummary++;
	ptNode->bEmblemColor2 = ptNode->bEmblemColor2 == 255 ? 0 : ptNode->bEmblemColor2;
	memcpy(ptNode->szGuildTag, pbSummary, 3);
	ptNode->szGuildTag[3] = '\0';

	wFlags = ptNode->wFlags;
	bClass = ptNode->bClass;
	if (wFlags & 4)
	{
		if (wFlags & 8)
		{
			bClass = CharSelClassTest(ptNode->bClass) ? 7 : 6;
			eMode = 5;
		}
		else
			eMode = 1;
	}
	else
		eMode = 5;
	ptNode->hUnit = (D2COMPOSITEUNIT *)D2WinD2CompCreate(bClass, eMode, ptNode->abGfx,
		ptNode->abColors);
	if (!ptNode->hUnit)
	{
		for (i = 0; i < 16; i++)
		{
			if (i == 1)
				ptNode->abGfx[i] = 1;
			else
				ptNode->abGfx[i] = 0;
			ptNode->abColors[i] = 255;
		}
		ptNode->hUnit = (D2COMPOSITEUNIT *)D2WinD2CompCreate(5, 5, ptNode->abGfx,
			ptNode->abColors);
	}
#line 507
	D2ASSERT(ptNode->hUnit);
	if (ptNode->bEmblemIcon)
		D2WinD2CompSetEmblem(ptNode->hUnit, ptNode->bEmblemIcon, ptNode->bEmblemColor1,
			ptNode->bEmblemColor2);
	D2WinD2CompSetDirection(ptNode->hUnit, 0);
	D2WinD2CompSetFrame(ptNode->hUnit, 0);
	D2WinD2CompSetMode(ptNode->hUnit, 5);
	if (sgptCharList)
	{
		ptLast = sgptCharList;
		while (ptLast->ptNext)
			ptLast = ptLast->ptNext;
		ptLast->ptNext = ptNode;
	}
	else
		sgptCharList = ptNode;
	sgnCharEntries++;
	CharSelFill();
	if ((DWORD)(ptNode->wFlags & 7936) >= 1024 || (ptNode->wFlags & 4))
		gfHardcoreAvailable = TRUE;
	return TRUE;
}
#line 529

static __inline void CharSelClassName(Unicode *pwszDest, BYTE bClass)
{
	switch (bClass)
	{
	case LAUNCHCLASS_AMAZON:
		D2LangUnicodestrcpy(pwszDest, (const WORD *)D2LangStrGetStringByIndex(4011));
		return;
	case LAUNCHCLASS_SORCERESS:
		D2LangUnicodestrcpy(pwszDest, (const WORD *)D2LangStrGetStringByIndex(4010));
		return;
	case LAUNCHCLASS_NECROMANCER:
		D2LangUnicodestrcpy(pwszDest, (const WORD *)D2LangStrGetStringByIndex(4009));
		return;
	case LAUNCHCLASS_PALADIN:
		D2LangUnicodestrcpy(pwszDest, (const WORD *)D2LangStrGetStringByIndex(4008));
		return;
	case LAUNCHCLASS_BARBARIAN:
		D2LangUnicodestrcpy(pwszDest, (const WORD *)D2LangStrGetStringByIndex(4007));
		return;
	}
	D2LangUnicodestrcpy(pwszDest, (const WORD *)D2LangStrGetStringByIndex(5383));
}

static void __fastcall CharSelFill(void)
{
	char szNew[256];
	D2CHARSELENTRY *ptNode;
	const char *pszTitle;
	DWORD dwFlags;
	DWORD fFlag4;
	int nSlot;
	int nY;
	int i;

	ptNode = sgptCharList;
	nY = 140;
	i = 0;
	(void)D2LangUnicodetoUtf(szNew, (const WORD *)D2LangStrGetStringByIndex(5145), 256);
	for (; ptNode; ptNode = ptNode->ptNext, i++)
	{
		char szText[100];

		if (i < sgnTop || i >= sgnTop + 5)
		{
			D2WinD2CompSetFlags(ptNode->hUnit, 0);
			continue;
		}
		if (strcmp(ptNode->szName, szNew) == 0)
		{
			nY += 93;
			D2WinChildSetVisible(sgptNewImage, TRUE);
			D2WinClearTextBox(sgaptSlot[0]);
			if (geMode == LAUNCHMODE_BATTLENET && sgfNoFreeSlots)
			{
				D2WinTextBoxAddColorLineUTF(sgaptSlot[0], ptNode->szName, 1);
				D2WinTextBoxAddColorLineUnicode(sgaptSlot[0], (const WORD *)D2LangStrGetStringByIndex(5142),
					0, 0);
				D2WinTextBoxAddColorLineUnicode(sgaptSlot[0], (const WORD *)D2LangStrGetStringByIndex(5143),
					0, 0);
			}
			else
			{
				D2WinTextBoxAddColorLineUTF(sgaptSlot[0], ptNode->szName, 1);
				D2WinTextBoxAddColorLineUnicode(sgaptSlot[0], (const WORD *)D2LangStrGetStringByIndex(5144),
					0, 0);
				D2WinTextBoxAddColorLineUTF(sgaptSlot[0], ptNode->szRealm, 0);
			}
			continue;
		}
		nSlot = i - sgnTop;
		D2WinD2CompSetX(ptNode->hUnit, 545);
		D2WinD2CompSetY(ptNode->hUnit, nY);
		fFlag4 = ptNode->wFlags & 4;
		if (ptNode->wFlags & 8)
			dwFlags = gdwBitMasks[4] | 1;
		else
			dwFlags = gdwInvBitMasks[4] & 1;
		if (fFlag4)
			dwFlags |= gdwBitMasks[5];
		else
			dwFlags &= gdwInvBitMasks[5];
		D2WinD2CompSetFlags(ptNode->hUnit, dwFlags);
		nY += 93;
		Unicode wszClass[100];
		D2WinClearTextBox(sgaptSlot[nSlot]);
		szText[0] = '\0';
		pszTitle = D2CompTitleOf(ptNode->bClass, (ptNode->wFlags >> 8) & 31, fFlag4);
		if (pszTitle)
			strcat(szText, pszTitle);
		strcat(szText, ptNode->szName);
		if (strlen(ptNode->szGuildTag) != 0)
			sprintf(szText + strlen(szText), " {%s}", ptNode->szGuildTag);
		D2WinTextBoxAddColorLineUTF(sgaptSlot[nSlot], szText, fFlag4 ? 1 : 4);
		CharSelClassName(wszClass, ptNode->bClass);
		Unicode wszLine[100];
		Unicode wszLevel[10];
		sprintf(szText, " %d ", ptNode->wLevel);
		Unicode::win2Unicode(wszLevel, szText, 9);
		Unicode::strcpy(wszLine, (const WORD *)D2LangStrGetStringByIndex(4057));
		Unicode::strcat(wszLine, wszLevel);
		Unicode::strcat(wszLine, wszClass);
		D2WinTextBoxAddColorLineUnicode(sgaptSlot[nSlot], wszLine, 0, 0);
		if (geMode == LAUNCHMODE_BATTLENET)
		{
			if (ptNode->szRealm[0] != '\0')
				D2WinTextBoxAddColorLineUTF(sgaptSlot[nSlot], ptNode->szRealm, 0);
			else
				D2WinTextBoxAddColorLineUnicode(sgaptSlot[nSlot],
					(const WORD *)D2LangStrGetStringByIndex(5164), 0,
					0);
		}
	}
}

BOOL __fastcall CharSelShowSelected(void)
{
	char szNew[256];

	char szText[100];

	D2CHARSELENTRY *ptNode;
	DWORD dwFlags;
	DWORD eMode;
	BOOL fFrame;
	BOOL fFrameShown;
	BYTE bClass;
	int nSlot;
	int i;

	if (sgnCharEntries == 0)
		sgnSelected = -1;
	nSlot = sgnSelected - sgnTop;
	D2WinChildSetEnabled(sgptDelete, sgnSelected > 0);
	D2WinChildSetVisible(sgptNewImage, sgnTop == 0);
	D2WinChildSetEnabled(sgptOk, TRUE);
	fFrameShown = TRUE;
	switch ((DWORD)nSlot)
	{
	case 0:
		if (sgnSelected == 0 && sgfNoFreeSlots && sgnTop == 0)
		{
			D2WinImageSetPosition(sgptFrameNew, 514, 153);
			D2WinChildSetVisible(sgptFrameNew, TRUE);
			D2WinChildSetVisible(sgptFrame, FALSE);
			D2WinChildSetEnabled(sgptOk, FALSE);
			fFrameShown = FALSE;
			break;
		}
		D2WinImageSetPosition(sgptFrame, 514, 153);
		fFrame = TRUE;
		break;
	case 1:
		D2WinImageSetPosition(sgptFrame, 514, 246);
		fFrame = TRUE;
		break;
	case 2:
		D2WinImageSetPosition(sgptFrame, 514, 339);
		fFrame = TRUE;
		break;
	case 3:
		D2WinImageSetPosition(sgptFrame, 514, 432);
		fFrame = TRUE;
		break;
	case 4:
		D2WinImageSetPosition(sgptFrame, 514, 525);
		fFrame = TRUE;
		break;
	default:
		fFrame = FALSE;
		break;
	}
	if (fFrameShown)
	{
		D2WinChildSetVisible(sgptFrame, fFrame);
		D2WinChildSetVisible(sgptFrameNew, FALSE);
	}
	if (sgnTop > 0) D2WinChildSetVisible(sgptUp, 1); else D2WinChildSetVisible(sgptUp, 0);
	if (sgnTop + 5 < sgnCharEntries) D2WinChildSetVisible(sgptDown, 1); else D2WinChildSetVisible(sgptDown, 0);
	ptNode = sgptCharList;
	for (i = 0; ptNode; i++)
	{
		if (sgnSelected == i)
			break;
		ptNode = ptNode->ptNext;
	}
	if (!ptNode)
		return TRUE;
	(void)D2LangUnicodetoUtf(szNew, (const WORD *)D2LangStrGetStringByIndex(5145), 256);
	if (sgptSelected)
	{
		if (strcmp(sgptSelected->szName, szNew) != 0)
			D2WinD2CompFree(sgptSelected->hUnit, TRUE);
#line 739
		D2FREE2(sgptSelected);
		sgptSelected = NULL;
	}
	if (strcmp(ptNode->szName, szNew) == 0)
	{
#line 745
		sgptSelected = (D2CHARSELENTRY *)D2ALLOC2(sizeof(D2CHARSELENTRY));
		StormSMemCopy(sgptSelected, ptNode, sizeof(D2CHARSELENTRY));
		sgptSelected->ptNext = NULL;
		D2WinClearTextBox(sgptSelName);
		D2WinTextBoxAddColorLineUTF(sgptSelName, ptNode->szName, 1);
		D2WinClearTextBox(sgptSelClass);
		D2WinClearTextBox(sgptSelRealm);
		return TRUE;
	}
#line 766
	sgptSelected = (D2CHARSELENTRY *)D2ALLOC2(sizeof(D2CHARSELENTRY));
	StormSMemCopy(sgptSelected, ptNode, sizeof(D2CHARSELENTRY));
	sgptSelected->ptNext = NULL;
	bClass = sgptSelected->bClass;
	if (sgptSelected->wFlags & 4)
	{
		if (sgptSelected->wFlags & 8)
		{
			bClass = CharSelClassTest(bClass) ? 7 : 6;
			eMode = 5;
		}
		else
			eMode = 1;
	}
	else
		eMode = 5;
	sgptSelected->hUnit = (D2COMPOSITEUNIT *)D2WinD2CompCreate(bClass, eMode, sgptSelected->abGfx,
		sgptSelected->abColors);
	if (!sgptSelected->hUnit)
	{
		for (i = 0; i < 16; i++)
		{
			if (i == 1)
				sgptSelected->abGfx[1] = 1;
			else
				sgptSelected->abGfx[i] = 0;
			sgptSelected->abColors[i] = 255;
		}
		sgptSelected->hUnit = (D2COMPOSITEUNIT *)D2WinD2CompCreate(5, 5, sgptSelected->abGfx,
			sgptSelected->abColors);
	}
#line 805
	D2ASSERT(sgptSelected->hUnit);
	if (sgptSelected->bEmblemIcon)
		D2WinD2CompSetEmblem(sgptSelected->hUnit, sgptSelected->bEmblemIcon, sgptSelected->bEmblemColor1,
			sgptSelected->bEmblemColor2);
	D2WinD2CompSetDirection(sgptSelected->hUnit, 0);
	D2WinD2CompSetFrame(sgptSelected->hUnit, 0);
	D2WinD2CompSetMode(sgptSelected->hUnit, 5);
	D2WinD2CompSetX(sgptSelected->hUnit, 280);
	D2WinD2CompSetY(sgptSelected->hUnit, 370);
	if (sgptSelected->wFlags & 8)
		dwFlags = gdwBitMasks[4] | 5;
	else
		dwFlags = gdwInvBitMasks[4] & 5;
	if (sgptSelected->wFlags & 4)
		dwFlags |= gdwBitMasks[5];
	else
		dwFlags &= gdwInvBitMasks[5];
	D2WinD2CompSetFlags(sgptSelected->hUnit, dwFlags);
	Unicode wszClass[100];
	D2WinClearTextBox(sgptSelName);
	if (strlen(sgptSelected->szGuildTag) != 0)
	{
		sprintf(szText, "%s {%s}", sgptSelected->szName, sgptSelected->szGuildTag);
		D2WinTextBoxAddColorLineUTF(sgptSelName, szText, 0);
	}
	else
		D2WinTextBoxAddColorLineUTF(sgptSelName, sgptSelected->szName, 0);
	D2WinClearTextBox(sgptSelClass);
	CharSelClassName(wszClass, sgptSelected->bClass);
	Unicode wszLine[100];
	Unicode wszLevel[10];
	sprintf(szText, " %d ", ptNode->wLevel);
	Unicode::win2Unicode(wszLevel, szText, 9);
	Unicode::strcpy(wszLine, (const WORD *)D2LangStrGetStringByIndex(4057));
	Unicode::strcat(wszLine, wszLevel);
	Unicode::strcat(wszLine, wszClass);
	D2WinTextBoxAddColorLineUnicode(sgptSelClass, wszLine, 0, 0);
	D2WinClearTextBox(sgptSelRealm);
	if (sgptSelected->szRealm[0] != '\0')
		D2WinTextBoxAddColorLineUTF(sgptSelRealm, sgptSelected->szRealm, 0);
	else
		D2WinTextBoxAddColorLineUnicode(sgptSelRealm, (const WORD *)D2LangStrGetStringByIndex(5164), 0, 0);
	return TRUE;
}

BOOL __stdcall CharSelOnSlot(_PARAMS *ptMsg)
{
	DWORD dwNow;
	int nPrevious;
	int nEntry;
	int i;

	nPrevious = sgnSelected;
	for (i = 0; i < 5; i++)
	{
		if (ptMsg->window == sgaptSlot[i])
			break;
	}
	nEntry = sgnTop + i;
	if (sgnCharEntries < nEntry)
		return FALSE;
	sgnSelected = nEntry;
	if (sgnCharEntries <= 0)
		sgnSelected = -1;
	dwNow = GetTickCount();
	if (sgnSelected != -1 && sgnSelected == nPrevious && dwNow - sgdwLastClick < 300)
		return CharSelOnOk(ptMsg);
	sgdwLastClick = dwNow;
	return CharSelShowSelected();
}

D2WINCONTROL * __fastcall CharSelDifficultyBox(int nTitle)
{
	D2WINCONTROL *ptChild;

	MainMenusDestroyBoxChildren();
	ptChild = MainMenusCreateBoxChild(85);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(84);
	D2WinTextBoxAddColorLineUnicode(ptChild, (const WORD *)D2LangStrGetStringByIndex(5157), 7, 0);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(81);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(82);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(83);
	D2WinChildSetEnabled(ptChild, FALSE);
	D2WinChildSetOnTop(ptChild, TRUE);
	if (nTitle >= 8)
		D2WinChildSetEnabled(ptChild, TRUE);
	return MainMenusCreateBoxChild(86);
}

BOOL __stdcall CharSelOnOk(_PARAMS *ptMsg)
{
	D2CHARSELENTRY *ptNode;
	int nTitle;
	int i;

	if (sgfOkDisabled)
	{
		((void (__cdecl *)(const char *))FogErrorManagerLog)("OK disabled");
		return TRUE;
	}
	((void (__cdecl *)(const char *))FogErrorManagerLog)("OK enabled, continuing");
	gptParams->bDifficulty = 0;
	if (sgnSelected == 0)
	{
		if (geMode == LAUNCHMODE_BATTLENET && sgfNoFreeSlots)
			return TRUE;
		MainMenusOnNewCharacter(ptMsg);
		sgnFreeSlots--;
		if (sgnFreeSlots > 0)
			return TRUE;
		sgfNoFreeSlots = TRUE;
		return TRUE;
	}
	if (sgnSelected == -1)
		return TRUE;
	ptNode = sgptCharList; i = 0;
	if (!ptNode)
		return TRUE;
	while (ptNode && sgnSelected != i)
	{
		ptNode = ptNode->ptNext;
		i++;
	}
	if (!ptNode) return TRUE;
	if ((ptNode->wFlags & 4) && (ptNode->wFlags & 8)
		&& (geMode == LAUNCHMODE_SINGLE || geMode == LAUNCHMODE_TCPIP))
	{
		MainMenusOkBox((const WORD *)D2LangStrGetStringByIndex(5304), MainMenusDestroyBoxChildren);
		return TRUE;
	}
	if (geMode == LAUNCHMODE_SINGLE || (geMode == LAUNCHMODE_TCPIP && gptParams->dwGameType == 6))
	{
		nTitle = (ptNode->wFlags >> 8) & 31;
		if (nTitle >= 4)
		{
			CharSelDifficultyBox(nTitle);
			return TRUE;
		}
	}
	MainMenusStart(ptNode->szName, ptNode->wFlags, ptNode->bClass, ptNode->szRealm);
	return TRUE;
}

void __fastcall CharSelStart(DWORD dwUnused, BYTE bDifficulty)
{
	D2CHARSELENTRY *ptNode;
	int i;

	if (sgnSelected == -1)
		return;
	ptNode = sgptCharList;
	i = 0;
	if (!ptNode)
		return;
	while (ptNode && sgnSelected != i)
	{
		ptNode = ptNode->ptNext;
		i++;
	}
	if (!ptNode)
		return;
	gptParams->bDifficulty = bDifficulty;
	MainMenusDestroyBoxChildren();
	MainMenusStart(ptNode->szName, ptNode->wFlags, ptNode->bClass, ptNode->szRealm);
}

BOOL __stdcall CharSelOnDifficultyNormal(_PARAMS *ptMsg)
{
	CharSelStart(0, 0);
	return TRUE;
}

BOOL __stdcall CharSelOnDifficultyNightmare(_PARAMS *ptMsg)
{
	CharSelStart(0, 1);
	return TRUE;
}

BOOL __stdcall CharSelOnDifficultyHell(_PARAMS *ptMsg)
{
	CharSelStart(0, 2);
	return TRUE;
}

BOOL __stdcall CharSelOnDeleteNo(_PARAMS *ptMsg)
{
	MainMenusDestroyBoxChildren();
	return TRUE;
}

BOOL __stdcall CharSelOnDelete(_PARAMS *ptMsg)
{
	D2WINCONTROL *ptChild;

	MainMenusDestroyBoxChildren();
	ptChild = MainMenusCreateBoxChild(108);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(109);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(110);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(111);
	D2WinChildSetOnTop(ptChild, TRUE);
	D2WinTextBoxAddColorLineUnicode(ptChild, (const WORD *)D2LangStrGetStringByIndex(5163), 0, 0);
	return TRUE;
}

BOOL __stdcall CharSelOnDeleteYes(_PARAMS *ptMsg)
{
	char szText[128];
	D2CHARSELENTRY *ptNode;
	const char *pszRealm;
	int i;

	ptNode = sgptCharList;
	i = 0;
	if (ptNode)
	{
		while (ptNode && sgnSelected != i)
		{
			ptNode = ptNode->ptNext;
			i++;
		}
		if (!ptNode)
			return FALSE;
		sgfOkDisabled = TRUE;
		((void (__cdecl *)(const char *))FogErrorManagerLog)("Disabled OK button");
		pszRealm = ptNode->szRealm;
		if (pszRealm && pszRealm[0] != '\0')
		{
			if (MainMenusEnterRealm(ptNode->szName, pszRealm))
			{
				sprintf(szText, "%s,%s", pszRealm, ptNode->szName);
				gptComInt->pfnDeleteCharacter(szText);
				sgfNoFreeSlots = FALSE;
				sgnFreeSlots++;
			}
			else
			{
				MainMenusOkBox((const WORD *)D2LangStrGetStringByIndex(5211), CharSelOpen);
				sgfOkDisabled = FALSE;
				((void (__cdecl *)(const char *))FogErrorManagerLog)("Reenabled OK button");
				return TRUE;
			}
		}
		SaveLoadDelete(ptNode->szName);
		CharSelOpen();
		sgfOkDisabled = FALSE;
		((void (__cdecl *)(const char *))FogErrorManagerLog)("Reenabled OK button");
		return TRUE; }
	return FALSE;
}

BOOL __stdcall CharSelOnUp(_PARAMS *ptMsg)
{
	if (sgnTop > 0)
		sgnTop--;
	if (sgnSelected > 0)
		sgnSelected--;
	CharSelFill();
	CharSelShowSelected();
	return TRUE;
}

BOOL __stdcall CharSelOnDown(_PARAMS *ptMsg)
{
	if (sgnTop + 5 < sgnCharEntries)
	{
		if (sgnSelected < sgnCharEntries - 1)
			sgnSelected++;
		sgnTop++;
		CharSelFill();
		CharSelShowSelected();
	}
	return TRUE;
}

BOOL __stdcall CharSelOnKey(_PARAMS *ptMsg)
{
	if (!(ptMsg->dwMessage == WM_CHAR))
	{
		switch (ptMsg->dwWParam)
		{
		case VK_UP:
			if (sgnSelected > 0)
			{
				sgnSelected--;
				if (sgnSelected < sgnTop)
					sgnTop--;
			}
			break;
		case VK_DOWN:
			if (sgnSelected < sgnCharEntries - 1)
			{
				sgnSelected++;
				if (sgnSelected - sgnTop >= 5)
					sgnTop++;
			}
			break; }
		CharSelFill();
		CharSelShowSelected();
		return TRUE; }
	return FALSE;
}

static __inline int CharSelRealmList(void)
{
	D2COMINTCHARS *ptChars;
	DWORD *pdwField;
	char *pszName;
	char *pszSummary;
	char *pszText;
	D2LAUNCHERRINFO tState;
	int nMax;
	int nLeft;
	int nFree;

	if (gptComInt->pfnIsConnected())
	{
		ptChars = gptComInt->pfnRequestCharacterList(0);
		gptComInt->pfnGetError(&tState);
		if (gptComInt->pfnIsConnected() && tState.dwReason == 0)
		{
			if (!ptChars)
				return 0;
			pdwField = (DWORD *)ptChars;
			pdwField++;
			nMax = *pdwField++;
			nLeft = *pdwField++;
			nFree = nMax - nLeft;
			pszText = (char *)pdwField;
			while (nLeft--)
			{
				pszName = pszText;
				pszText += strlen(pszText) + 1;
				pszSummary = pszText;
				pszText += strlen(pszText) + 1;
				if (!CharSelAdd(pszName, (const BYTE *)pszSummary))
					nFree++;
			}
			return nFree;
		}
	}
	MainMenusCancelBox((const WORD *)D2LangStrGetStringByIndex(5347), MainMenusOpen);
	return 0;
}

void __fastcall CharSelOpen(void)
{
	char szOffRealm[256];
	char szNew[256];
	char szOnRealm[256];
	D2CHARSELENTRY *ptNode;
	D2WINCONTROL *ptExit;
	int i;

	MainMenusClear();
	sgnCharEntries = 0;
	sgnTop = 0;
	sgnSelected = 0;
	gfHardcoreAvailable = FALSE;
	ptExit = MainMenusCreateChild(64);
	sgptDelete = MainMenusCreateChild(65);
	D2WinChildSetEnabled(sgptDelete, FALSE);
	sgptOk = MainMenusCreateChild(66);
	sgptUp = MainMenusCreateChild(74);
	sgptDown = MainMenusCreateChild(75);
	D2WinChildSetVisible(sgptUp, FALSE);
	D2WinChildSetVisible(sgptDown, FALSE);
	sgptSelName = MainMenusCreateChild(76);
	sgptSelClass = MainMenusCreateChild(77);
	sgptSelRealm = MainMenusCreateChild(78);
	sgptNewImage = MainMenusCreateChild(79);
	sgaptSlot[0] = MainMenusCreateChild(67);
	sgaptSlot[1] = MainMenusCreateChild(68);
	sgaptSlot[2] = MainMenusCreateChild(69);
	sgaptSlot[3] = MainMenusCreateChild(70);
	sgaptSlot[4] = MainMenusCreateChild(71);
	sgptFrame = MainMenusCreateChild(72);
	sgptFrameNew = MainMenusCreateChild(73);
	D2WinChildSetVisible(sgptFrameNew, FALSE);
	D2WinSetBackground(ghCelCharSelectBckg);
	MainMenusLoadPalette(gszPaletteSkyDat, gszPaletteSkyPl2);
#line 1462
	ptNode = (D2CHARSELENTRY *)D2ALLOC2(sizeof(D2CHARSELENTRY));
	(void)D2LangUnicodetoUtf(szNew, (const WORD *)D2LangStrGetStringByIndex(5145), 256);
	(void)D2LangUnicodetoUtf(szOnRealm, (const WORD *)D2LangStrGetStringByIndex(5147), 256);
	(void)D2LangUnicodetoUtf(szOffRealm, (const WORD *)D2LangStrGetStringByIndex(5146), 256);
	strcpy(ptNode->szName, szNew);
	strcpy(ptNode->szRealm, geMode == LAUNCHMODE_BATTLENET ? szOnRealm : szOffRealm);
	for (i = 0; i < 16; i++)
		ptNode->abGfx[i] = 0;
	for (i = 0; i < 16; i++)
		ptNode->abColors[i] = 0;
	ptNode->wLevel = 0;
	ptNode->wFlags = 0;
	ptNode->wField126 = 0;
	ptNode->wGuildFlags = 0;
	ptNode->bEmblemIcon = 0;
	ptNode->bEmblemColor1 = 0;
	ptNode->bEmblemColor2 = 0;
	ptNode->hUnit = NULL;
	ptNode->ptNext = NULL;
	ptNode->bClass = 5;
	for (i = 0; i < 16; i++)
	{
		if (i == 1)
			ptNode->abGfx[i] = 1;
		else
			ptNode->abGfx[i] = 0;
		ptNode->abColors[i] = 255;
	}
	ptNode->hUnit = (D2COMPOSITEUNIT *)D2WinD2CompCreate(ptNode->bClass, 5, ptNode->abGfx,
		ptNode->abColors);
	D2WinD2CompSetDirection(ptNode->hUnit, 0);
	D2WinD2CompSetFrame(ptNode->hUnit, 0);
	D2WinD2CompSetMode(ptNode->hUnit, 5);
	sgptCharList = ptNode;
	sgnCharEntries++;
	CharSelFill();
	D2WinChildSetEnabled(ptExit, FALSE);
	sgfNoFreeSlots = FALSE;
	if (geMode == LAUNCHMODE_BATTLENET)
	{
		sgnFreeSlots = CharSelRealmList();
		if (sgnFreeSlots == 0)
			sgfNoFreeSlots = TRUE;
	}
	else if (geMode == LAUNCHMODE_SINGLE || geMode == LAUNCHMODE_OPENBATTLENET
		|| geMode == LAUNCHMODE_TCPIP)
	{
		CharSelScan();
	}
	if (D2SoundGetOptionsMusic())
		D2SoundSmpMusicSetState(1);
	D2WinChildSetEnabled(ptExit, TRUE);
	D2WinChildSetEnabled(sgptDelete, sgnCharEntries > 0);
	CharSelShowSelected();
	CharSelFill();
}
