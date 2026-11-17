#include "D2Launch.h"

static D2WINCONTROL * sgptConnectStage = 0;
static D2WINCONTROL * sgptConnectDots = 0;
static int sgnConnectString = 0;
static HANDLE sghConnect = 0;
static DWORD sgdwConnectStart = 0;

BOOL __stdcall BNetConnectLoop(DWORD dwArg)
{
	BNetConnectShow();
	if (!sghConnect)
	{
		sghConnect = gptComInt->pfnConnect();
		if (!sghConnect)
		{
			D2WinQuitLoop();
			return TRUE;
		}
		BNetClearCancel();
	}
	if (WaitForSingleObject(sghConnect, 0) == WAIT_OBJECT_0)
		D2WinQuitLoop();
	if (BNetGetCancel())
	{
		gptComInt->pfnDisconnect();
		D2WinQuitLoop();
	}
	return TRUE;
}

void __fastcall BNetClear(void)
{
	sgptConnectStage = NULL;
	sgptConnectDots = NULL;
}

void __fastcall BNetConnectBox(void)
{
	D2WINCONTROL *ptChild;

	sgdwConnectStart = GetTickCount();
	MainMenusDestroyBoxChildren();
	MainMenusSetWaitMark(TRUE);
	ptChild = MainMenusCreateBoxChild(116);
	D2WinSetInputChild(ptChild);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(117);
	D2WinChildSetOnTop(ptChild, TRUE);
	ptChild = MainMenusCreateBoxChild(118);
	D2WinChildSetOnTop(ptChild, TRUE);
	D2WinTextBoxAddColorLineUnicode(ptChild, (const WORD *)D2LangStrGetStringByIndex(5171), 0, 0);
	sgptConnectStage = MainMenusCreateBoxChild(119);
	D2WinChildSetOnTop(sgptConnectStage, TRUE);
	sgptConnectDots = MainMenusCreateBoxChild(120);
	D2WinChildSetOnTop(sgptConnectDots, TRUE);
	sgnConnectString = -1;
	sghConnect = NULL;
	MainMenusSetPending(BNetSetCancel);
	BNetConnectShow();
}

void __fastcall BNetConnectShow(void)
{
	int nStage;
	int nDots;
	char szDots[256];

	if (!sgptConnectStage)
		return;
	gptComInt->pfnGetStage(&nStage);
	switch (nStage)
	{
	case 0:
		sgnConnectString = 5171;
		break;
	case 1:
		sgnConnectString = 5172;
		break;
	case 2:
		sgnConnectString = 5173;
		break;
	case 3:
		sgnConnectString = 5174;
		break;
	case 4:
		sgnConnectString = 5175;
		break;
	case 5:
		sgnConnectString = 5176;
		break;
	case 6:
		sgnConnectString = 5177;
		break;
	default:
		sgnConnectString = -1;
		break;
	}
	D2WinClearTextBox(sgptConnectStage);
	if (sgnConnectString >= 0)
		D2WinTextBoxAddColorLineUnicode(sgptConnectStage,
			(const WORD *)D2LangStrGetStringByIndex(sgnConnectString), 0,
			0);
	nDots = (int)((GetTickCount() - sgdwConnectStart) / 250 % 20 + 1);
	sprintf(szDots, "%.*s", nDots, "....................");
	D2WinClearTextBox(sgptConnectDots);
	D2WinTextBoxAddLineUTF(sgptConnectDots, szDots);
}
