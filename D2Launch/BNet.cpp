#define D2_THIS_FILE "C:\\D2\\Source\\D2Launch\\Src\\BNet.cpp"
#include "D2Launch.h"
#include "D2Macros.h"

HMODULE ghBnclientLib = 0;
D2COMINT * gptComInt = 0;
static BOOL sgfConnectCancel = 0;
static BOOL sgfLoggedIn = 0;
static BOOL sgfConnecting = 0;

BOOL __fastcall BNetLoadProvider(INIT_PARAMS *ptParams)
{
	char szText[100];
	FARPROC pfnQueryInterface;

#line 59
	D2ASSERT(ptParams);
	if (ptParams->dwComInt)
		return TRUE;
	ghBnclientLib = LoadLibraryA("Bnclient.dll");
	if (!ghBnclientLib)
	{
		sprintf(szText, "Cannot load Bnclient.dll: Error %d", GetLastError());
		MessageBoxA(NULL, szText, "D2Multi", MB_ICONHAND);
		return FALSE;
	}
	pfnQueryInterface = GetProcAddress(ghBnclientLib, "QueryInterface");
	if (!pfnQueryInterface)
	{
		sprintf(szText, "Unable to QueryInterface for Bnclient.dll: Error %d", GetLastError());
		MessageBoxA(NULL, szText, "D2Multi", MB_ICONHAND);
		return FALSE;
	}
	gptComInt = ((D2COMINT * (__cdecl *)(void))pfnQueryInterface)();
	ptParams->dwComInt = (DWORD)gptComInt;
	return TRUE;
}

void __fastcall FreeBattleNetProvider(void)
{
#line 97
	D2ASSERT(ghBnclientLib);
	if (!FreeLibrary(ghBnclientLib))
		((void (__cdecl *)(const char *))FogErrorManagerLog)("FreeBattleNetProvider: FreeLibrary failed");
	ghBnclientLib = NULL;
	gptComInt = NULL;
}

void __fastcall BNetSetCancel(void)
{
	sgfConnectCancel = TRUE;
}

void __fastcall BNetClearCancel(void)
{
	sgfConnectCancel = FALSE;
}

BOOL __fastcall BNetGetCancel(void)
{
	return sgfConnectCancel;
}

BOOL __fastcall BNetLoggedIn(void)
{
	return sgfLoggedIn;
}

BOOL __stdcall BNetPatchLoop(DWORD dwArg)
{
	if (gptComInt->pfnGetProgress() != 101 && !MainMenusPatchCancelled())
	{
		MainMenusPatchUpdate();
		return TRUE;
	}
	D2WinQuitLoop();
	return TRUE;
}

BOOL __stdcall BNetLoginLoop(DWORD dwArg)
{
	if (gptComInt->pfnLoginResult())
		D2WinQuitLoop();
	if (sgfConnectCancel || !gptComInt->pfnIsConnected() || MainMenusWaitCancelled())
		D2WinQuitLoop();
	return TRUE;
}

BOOL __stdcall BNetNewAccountLoop(DWORD dwArg)
{
	DWORD dwResult;

	if (gptComInt->pfnCreateAccountResult(&dwResult))
		D2WinQuitLoop();
	if (sgfConnectCancel || !gptComInt->pfnIsConnected() || MainMenusWaitCancelled())
	{
		gptComInt->pfnDisconnect();
		D2WinQuitLoop();
	}
	return TRUE;
}

static BOOL __fastcall BNetError(D2LAUNCHERRINFO *errInfo);

BOOL __fastcall BNetConnect(void)
{
	D2LAUNCHERRINFO tPending;
	DWORD dwRestart;
	BOOL fApplied;

	if (!gptComInt)
		return FALSE;
#line 236
	D2ASSERT(!sgfConnecting);
	sgfConnecting = TRUE;
	gfHardcoreAvailable = FALSE;
	MainMenusPatchReset();
	OutputDebugStringA("------------------\nConnect to Battle.net\n------------------\n");
	if (!gptComInt->pfnDisconnect())
	{
		D2LAUNCHERRINFO tError;
		sgfConnecting = FALSE;
		tError.dwReason = 0;
		tError.parm = NULL;
		return BNetError(&tError);
	}
	sgfLoggedIn = FALSE;
	BNetConnectBox();
	D2WinLoop(BNetConnectLoop);
	if (sgfConnectCancel)
	{
		gfConnectBoxCancelled = TRUE;
		MainMenusCloseBox();
		gptComInt->pfnDisconnect();
		sgfConnecting = FALSE;
		return FALSE;
	}
	if (gptComInt->pfnGetProgress() != 101)
		MainMenusPatchScreen();
	D2WinLoop(BNetPatchLoop);
	if (MainMenusPatchCancelled() || gptComInt->pfnGetProgress() != 101)
	{
		gptComInt->pfnDisconnect();
		sgfConnecting = FALSE;
		MainMenusOpen();
		return FALSE;
	}
	dwRestart = 0;
	fApplied = gptComInt->pfnApplyPatch(&dwRestart);
	if (dwRestart)
	{
		StormSRegSaveValue("Diablo II", "UseCmdLine", 0, 1);
		D2WinCloseGraphics();
		gptComInt->pfnDisconnect();
		sgfConnecting = FALSE;
		TerminateProcess(GetCurrentProcess(), 0);
		return FALSE;
	}
	if (!fApplied)
	{
		D2LAUNCHERRINFO tError;
		sgfConnecting = FALSE;
		tError.dwReason = 14;
		tError.parm = NULL;
		return BNetError(&tError);
	}
	gptComInt->pfnGetError(&tPending);
	MainMenusCloseBox();
	sgfConnecting = FALSE;
	if (tPending.dwReason)
		return BNetError(&tPending);
	return gptComInt->pfnIsConnected() != 0;
}

BOOL __fastcall BNetLogin(INIT_PARAMS *ptParams, BOOL fNewAccount)
{
	D2LAUNCHERRINFO tPending;
	DWORD dwResult;
	DWORD dwReason;

	if (!gptComInt->pfnIsConnected())
	{
		MainMenusCancelBox((const WORD *)D2LangStrGetStringByIndex(5347), MainMenusOpen);
		return FALSE;
	}
	if (sgfLoggedIn)
	{
		((void (__cdecl *)(const char *))FogErrorManagerLog)(
			"WARNING: Trying to log in when already logged in!.");
		return TRUE;
	}
	MainMenusWaitBox();
	gptComInt->pfnGetError(&tPending);
	if (fNewAccount && !tPending.dwReason)
	{
		gptComInt->pfnCreateAccount(BNET_ACCOUNT(ptParams), BNET_PASSWORD(ptParams));
		D2WinLoop(BNetNewAccountLoop);
		if (sgfConnectCancel)
		{
			MainMenusCloseBox();
			return FALSE;
		}
		gptComInt->pfnCreateAccountResult(&dwResult);
		if (dwResult && gptComInt->pfnIsConnected())
		{
			switch (dwResult)
			{
			case 2:
				dwReason = 16;
				break;
			case 3:
				dwReason = 17;
				break;
			case 4:
				dwReason = 18;
				break;
			case 5:
				dwReason = 19;
				break;
			case 6:
				dwReason = 20;
				break;
			case 7:
				dwReason = 21;
				break;
			case 8:
				dwReason = 22;
				break;
			default:
				dwReason = 15;
				break;
			}
			{
				D2LAUNCHERRINFO tError;
				tError.dwReason = dwReason;
				tError.parm = NULL;
				BNetError(&tError);
			}
			return FALSE;
		}
	}
	if (gptComInt->pfnIsConnected() && !tPending.dwReason)
	{
		gptComInt->pfnLogin(BNET_ACCOUNT(ptParams), BNET_PASSWORD(ptParams));
		D2WinLoop(BNetLoginLoop);
		if (sgfConnectCancel || MainMenusWaitCancelled())
		{
			MainMenusCloseBox();
			return FALSE;
		}
		gptComInt->pfnGetError(&tPending);
	}
	MainMenusCloseBox();
	if (tPending.dwReason)
		return BNetError(&tPending);
	sgfLoggedIn = TRUE;
	if (gptComInt->pfnIsConnected())
		return TRUE;
	{
		D2LAUNCHERRINFO tError;
		tError.dwReason = 14;
		tError.parm = NULL;
		BNetError(&tError);
	}
	return FALSE;
}

void __cdecl BNetDelete(void *pv)
{
	if (pv)
		StormSMemFree(pv, "delete", -1, 0);
}

void * __cdecl BNetNew(DWORD dwSize)
{
	return StormSMemAlloc(dwSize, "new", -1, 0);
}

static BOOL __fastcall BNetError(D2LAUNCHERRINFO *errInfo)
{

	PFNLAUNCHPENDING pfnAfter;
	BOOL fLeave;
	int nTitle;
	int nText;
	int nText2;
	int nKind;

	Unicode awszText[512];
	Unicode awszText2[512];
	Unicode awszTitle[512];
	fLeave = TRUE;
	pfnAfter = MainMenusOpen;
	nText2 = -1;
	awszTitle[0].wChar = 0;
	awszText2[0].wChar = 0;
	switch (errInfo->dwReason)
	{
	case 1:
		nTitle = 5245;
		nText = 5187;
		nText2 = 5188;
		nKind = 2;
		break;
	case 2:
		nTitle = 5246;
		nText = 5183;
		nKind = 1;
		break;
	case 3:
		nTitle = 5247;
		nText = 5186;
		nText2 = 5194;
		nKind = 2;
		break;
	case 4:
		nTitle = 5247;
		nText = 5199;
		nKind = 1;
		break;
	case 5:
		nTitle = 5247;
		nText = 5202;
		nKind = 1;
		break;
	case 6:
	{

		Unicode awszParm[512];
#line 519
		D2ASSERT(errInfo->parm != NULL);
		Unicode::win2Unicode(awszParm, errInfo->parm, 512);
		D2LangUnicodestrncpy(awszTitle, (const WORD *)D2LangStrGetStringByIndex(5247), 512);
		D2LangUnicodestrncpy(awszText, (const WORD *)D2LangStrGetStringByIndex(5200), 512);
		D2LangUnicodestrncpy(awszText2, (const WORD *)D2LangStrGetStringByIndex(5201), 512);
		MainMenusParmBox((const WORD *)awszTitle, (const WORD *)awszText, (const WORD *)awszParm, (const WORD *)awszText2, MainMenusOpen);
		return FALSE;
	}
	case 7:
		nTitle = 5247;
		nText = 5356;
		nKind = 1;
		break;
	case 8:
		nTitle = 5247;
		nText = 5189;
		nText2 = 5190;
		nKind = 2;
		break;
	case 15:
		nTitle = 5249;
		nText = 5231;
		nKind = 1;
		pfnAfter = MainMenusOpenNewAccount;
		fLeave = FALSE;
		break;
	case 16:
		nTitle = 5249;
		nText = 5232;
		nKind = 1;
		pfnAfter = MainMenusOpenNewAccount;
		fLeave = FALSE;
		break;
	case 17:
		nTitle = 5249;
		nText = 5233;
		nKind = 1;
		pfnAfter = MainMenusOpenNewAccount;
		fLeave = FALSE;
		break;
	case 18:
		nTitle = 5249;
		nText = 5239;
		nKind = 1;
		pfnAfter = MainMenusOpenNewAccount;
		fLeave = FALSE;
		break;
	case 19:
		nTitle = 5249;
		nText = 5234;
		nKind = 1;
		pfnAfter = MainMenusOpenNewAccount;
		fLeave = FALSE;
		break;
	case 20:
		nTitle = 5249;
		nText = 5235;
		nKind = 1;
		pfnAfter = MainMenusOpenNewAccount;
		fLeave = FALSE;
		break;
	case 21:
		nTitle = 5249;
		nText = 5236;
		nKind = 1;
		pfnAfter = MainMenusOpenNewAccount;
		fLeave = FALSE;
		break;
	case 22:
		nTitle = 5249;
		nText = 5237;
		nKind = 1;
		pfnAfter = MainMenusOpenNewAccount;
		fLeave = FALSE;
		break;
	case 9:
		nTitle = 5244;
		nText = 5208;
		nKind = 1;
		fLeave = FALSE;
		pfnAfter = MainMenusOpenLogin;
		break;
	case 10:
		nTitle = 5244;
		nText = 5207;
		nKind = 1;
		fLeave = FALSE;
		pfnAfter = MainMenusOpenLogin;
		break;
	case 11:
		nTitle = 5244;
		nText = 5209;
		nKind = 1;
		fLeave = FALSE;
		pfnAfter = MainMenusOpenLogin;
		break;
	case 12:
		nTitle = 5248;
		nText = 5349;
		nKind = 1;
		break;
	case 13:
		nTitle = 5248;
		nText = 5354;
		nKind = 1;
		break;
	case 14:
		nTitle = 5248;
		nText = 5347;
		nKind = 1;
		break;
	default:
		nTitle = 5248;
		nText = 5189;
		nText2 = 5190;
		nKind = 2;
		break;
	}
	D2LangUnicodestrncpy(awszText, (const WORD *)D2LangStrGetStringByIndex(nText), 512);
	if (nText2 != -1)
		D2LangUnicodestrncpy(awszText2, (const WORD *)D2LangStrGetStringByIndex(nText2), 512);
	if (nTitle != -1)
		D2LangUnicodestrncpy(awszTitle, (const WORD *)D2LangStrGetStringByIndex(nTitle), 512);
	if (nKind == 0)
		MainMenusCancelBox((const WORD *)awszText, pfnAfter);
	else if (nKind == 1)
		MainMenusTitleBox((const WORD *)awszTitle, (const WORD *)awszText, pfnAfter);
	else
		MainMenusTitleBoxLarge((const WORD *)awszTitle, (const WORD *)awszText, (const WORD *)awszText2, pfnAfter);
	if (fLeave)
		gptComInt->pfnDisconnect();
	return FALSE;
}

void __fastcall BNetLeave(void)
{
	if (gptComInt)
	{
		gfHardcoreAvailable = FALSE;
		gptParams->abChannel[0] = 0;
		gptParams->abFirstChannel[0] = 0;
		OutputDebugStringA("Disconnect from BNET\n");
		gptComInt->pfnDisconnect();
	}
}
