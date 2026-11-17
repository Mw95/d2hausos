#ifndef MATCH_D2LAUNCH_IMPORTS_H
#define MATCH_D2LAUNCH_IMPORTS_H
#include <windows.h>
#include "FogImports.h"
#include "StormImports.h"

extern "C" __declspec(dllimport) DWORD gdwInvBitMasks[];

extern "C"
{
DWORD __stdcall D2WinOrdinal10001(DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10002(void);
DWORD __stdcall D2WinOrdinal10003(DWORD);
DWORD __stdcall D2WinOrdinal10004(DWORD);
DWORD __stdcall D2WinOrdinal10005(DWORD);
DWORD __fastcall D2WinOrdinal10006(void);
DWORD __stdcall D2WinOrdinal10007(DWORD);
DWORD __stdcall D2WinOrdinal10008(DWORD);
DWORD __stdcall D2WinOrdinal10010(DWORD);
DWORD __cdecl D2WinOrdinal10011(...);
DWORD __fastcall D2WinOrdinal10012(void);
DWORD __stdcall D2WinOrdinal10013(DWORD);
DWORD __stdcall D2WinOrdinal10014(DWORD);
DWORD __fastcall D2WinOrdinal10018(DWORD, DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10020(DWORD);
DWORD __fastcall D2WinOrdinal10021(DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10023(DWORD, DWORD);
DWORD __stdcall D2WinOrdinal10024(DWORD, DWORD);
DWORD __stdcall D2WinOrdinal10025(DWORD);
DWORD __stdcall D2WinOrdinal10026(DWORD, DWORD);
DWORD __stdcall D2WinOrdinal10027(DWORD);
DWORD __stdcall D2WinOrdinal10029(DWORD);
DWORD __stdcall D2WinOrdinal10031(DWORD);
DWORD __fastcall D2WinOrdinal10034(void);
DWORD __fastcall D2WinOrdinal10035(DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10037(DWORD);
DWORD __fastcall D2WinOrdinal10038(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10040(DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10041(DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10042(DWORD, DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10043(DWORD);
DWORD __fastcall D2WinOrdinal10045(DWORD);
DWORD __fastcall D2WinOrdinal10046(DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10047(DWORD, DWORD, DWORD, BYTE);
DWORD __fastcall D2WinOrdinal10049(DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10050(DWORD);
DWORD __fastcall D2WinOrdinal10051(DWORD);
DWORD __fastcall D2WinOrdinal10055(DWORD, DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10058(DWORD, DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10067(DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10068(DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10071(DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10072(DWORD);
DWORD __stdcall D2WinOrdinal10073(DWORD, DWORD, DWORD);
DWORD __stdcall D2WinOrdinal10074(DWORD, DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10075(DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10076(DWORD, DWORD);
DWORD __stdcall D2WinOrdinal10080(DWORD, DWORD, DWORD);
DWORD __stdcall D2WinOrdinal10081(DWORD, DWORD);
DWORD __stdcall D2WinOrdinal10082(DWORD, DWORD);
DWORD __stdcall D2WinOrdinal10083(DWORD);
DWORD __stdcall D2WinOrdinal10089(DWORD);
DWORD __fastcall D2WinOrdinal10097(DWORD);
DWORD __stdcall D2WinOrdinal10098(DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10099(DWORD);
DWORD __fastcall D2WinOrdinal10106(DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10120(DWORD);
DWORD __fastcall D2WinOrdinal10128(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD __stdcall D2WinOrdinal10135(DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10136(DWORD, DWORD);
DWORD __stdcall D2WinOrdinal10137(DWORD, DWORD, DWORD, DWORD);
DWORD __stdcall D2WinOrdinal10142(DWORD, BYTE);
DWORD __stdcall D2WinOrdinal10144(DWORD, DWORD);
DWORD __stdcall D2WinOrdinal10145(DWORD, DWORD);
DWORD __stdcall D2WinOrdinal10146(DWORD, DWORD);
DWORD __stdcall D2WinOrdinal10149(DWORD, DWORD);
DWORD __stdcall D2WinOrdinal10151(DWORD, BYTE, BYTE, BYTE);
DWORD __stdcall D2WinOrdinal10154(DWORD, DWORD, DWORD);
DWORD __fastcall D2WinOrdinal10164(DWORD);
DWORD __fastcall D2WinOrdinal10165(DWORD);
DWORD __fastcall D2WinOrdinal10166(void);
DWORD __fastcall D2WinOrdinal10173(DWORD);
DWORD __fastcall D2WinOrdinal10175(DWORD);
DWORD __fastcall D2WinOrdinal10178(void);
DWORD __stdcall D2WinCutsceneCloseWindow(DWORD);
DWORD __fastcall D2WinCutsceneCreateWindow(void);
DWORD __stdcall D2WinSetQuitCallback(DWORD);
}

extern "C"
{
DWORD __fastcall D2LangOrdinal10000(DWORD);
DWORD __fastcall D2LangOrdinal10001(void);
DWORD __fastcall D2LangOrdinal10004(WORD);
DWORD __stdcall D2LangOrdinal10006(DWORD, DWORD);
__declspec(dllimport) DWORD __fastcall D2LangOrdinal10017(DWORD);
__declspec(dllimport) DWORD __fastcall D2LangOrdinal10033(DWORD, DWORD);
__declspec(dllimport) DWORD __fastcall D2LangOrdinal10035(DWORD, DWORD);
__declspec(dllimport) DWORD __fastcall D2LangOrdinal10037(DWORD, DWORD);
__declspec(dllimport) DWORD __fastcall D2LangOrdinal10039(DWORD);
__declspec(dllimport) DWORD __fastcall D2LangOrdinal10042(DWORD, DWORD, DWORD);
__declspec(dllimport) DWORD __fastcall D2LangOrdinal10043(DWORD, DWORD, DWORD);
__declspec(dllimport) DWORD __fastcall D2LangOrdinal10044(DWORD, DWORD);
__declspec(dllimport) DWORD __fastcall D2LangOrdinal10049(DWORD, DWORD, DWORD);
__declspec(dllimport) DWORD __fastcall D2LangOrdinal10051(DWORD, DWORD, DWORD);
__declspec(dllimport) DWORD __fastcall D2LangOrdinal10053(DWORD, DWORD, DWORD);
__declspec(dllimport) DWORD __fastcall D2LangOrdinal10055(DWORD, DWORD);
__declspec(dllimport) DWORD __fastcall D2LangOrdinal10059(DWORD, DWORD, DWORD);
}

extern "C"
{
DWORD __fastcall D2gfxOrdinal10029(void);
DWORD __stdcall D2gfxOrdinal10033(DWORD, DWORD, DWORD);
DWORD __stdcall D2gfxOrdinal10035(DWORD, DWORD);
DWORD __stdcall D2gfxOrdinal10036(DWORD);
DWORD __stdcall D2gfxOrdinal10037(DWORD);
DWORD __stdcall D2gfxOrdinal10076(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
}

extern "C"
{
DWORD __fastcall D2soundOrdinal10027(DWORD);
DWORD __fastcall D2soundOrdinal10029(DWORD);
DWORD __fastcall D2soundOrdinal10031(void);
DWORD __fastcall D2soundOrdinal10034(DWORD);
DWORD __fastcall D2soundOrdinal10036(void);
DWORD __fastcall D2soundOrdinal10037(void);
DWORD __fastcall D2soundOrdinal10038(DWORD);
DWORD __fastcall D2soundOrdinal10039(DWORD);
DWORD __fastcall D2soundOrdinal10040(void);
DWORD __fastcall D2soundOrdinal10042(void);
DWORD __fastcall D2soundOrdinal10069(void);
}

extern "C"
{
DWORD __stdcall D2MCPClientOrdinal10000(DWORD);
DWORD __stdcall D2MCPClientOrdinal10001(void);
DWORD __cdecl D2MCPClientOrdinal10002(...);
DWORD __cdecl D2MCPClientOrdinal10003(...);
DWORD __stdcall D2MCPClientOrdinal10008(DWORD, DWORD, WORD);
DWORD __stdcall D2MCPClientOrdinal10009(DWORD);
DWORD __fastcall D2MCPClientOrdinal10021(WORD);
DWORD __stdcall D2MCPClientOrdinal10028(DWORD);
DWORD __cdecl D2MCPClientOrdinal10029(...);
DWORD __stdcall D2MCPClientOrdinal10030(DWORD);
DWORD __cdecl D2MCPClientOrdinal10031(...);
DWORD __stdcall D2MCPClientOrdinal10032(DWORD);
DWORD __cdecl D2MCPClientOrdinal10033(...);
DWORD __cdecl D2MCPClientOrdinal10046(...);
}

extern "C"
{
DWORD __stdcall D2NetOrdinal10000(DWORD, DWORD);
DWORD __cdecl D2NetOrdinal10001(...);
DWORD __stdcall D2NetOrdinal10008(DWORD, DWORD);
DWORD __cdecl D2NetOrdinal10025(...);
}

extern "C"
{
DWORD __fastcall FogOrdinal10012(void);
DWORD __stdcall FogOrdinal10063(DWORD);
BYTE __stdcall FogOrdinal10064(DWORD, DWORD);
DWORD __fastcall FogOrdinal10080(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD __fastcall FogOrdinal10081(DWORD);
DWORD __fastcall FogOrdinal10082(DWORD);
DWORD __fastcall FogOrdinal10083(DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD __fastcall FogOrdinal10084(DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD __fastcall FogOrdinal10086(DWORD, DWORD);
DWORD __fastcall FogOrdinal10136(DWORD);
}

extern "C"
{
DWORD __stdcall StormOrdinal412(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD __stdcall StormOrdinal422(DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD __stdcall StormOrdinal425(DWORD, DWORD, DWORD, DWORD);
DWORD __stdcall StormOrdinal426(DWORD, DWORD, DWORD, DWORD);
DWORD __stdcall StormOrdinal491(DWORD, DWORD, DWORD);
DWORD __stdcall StormOrdinal508(const char *, const char *, DWORD);
DWORD __cdecl StormOrdinal578(...);
}

#define D2WinInitGraphics(dwWindowed, nResolution) \
	((BOOL)D2WinOrdinal10001((DWORD)(dwWindowed), (DWORD)(nResolution)))

#define D2WinCloseGraphics() \
	((BOOL)D2WinOrdinal10002())

#define D2WinResizeWindow(nResolution) \
	((BOOL)D2WinOrdinal10003((DWORD)(nResolution)))

#define D2WinSetBackground(hCelFile) \
	D2WinOrdinal10004((DWORD)(hCelFile))

#define D2WinLoop(pfnPass) \
	D2WinOrdinal10005((DWORD)(pfnPass))

#define D2WinQuitLoop() \
	((BOOL)D2WinOrdinal10006())

#define D2WinSetCursor(hCelFile) \
	((BOOL)D2WinOrdinal10007((DWORD)(hCelFile)))

#define D2WinSetKeyFocus(ptChild) \
	((BOOL)D2WinOrdinal10008((DWORD)(ptChild)))

#define D2WinSetWindowProc(pfnWindowProc) \
	((BOOL)D2WinOrdinal10010((DWORD)(pfnWindowProc)))

#define D2WinCreateParent() \
	((BOOL)D2WinOrdinal10011())

#define D2WinCloseParent() \
	((BOOL)D2WinOrdinal10012())

#define D2WinCreateChild(ptWindow) \
	((void *)D2WinOrdinal10013((DWORD)(ptWindow)))

#define D2WinDestroyChild(pptChild) \
	D2WinOrdinal10014((DWORD)(pptChild))

#define D2WinRegisterMessages(hWnd, ptTable, dwCount) \
	D2WinOrdinal10018((DWORD)(hWnd), (DWORD)(ptTable), (DWORD)(dwCount))

#define D2WinChildGetEnabled(ptChild) \
	D2WinOrdinal10020((DWORD)(ptChild))

#define D2WinChildSetEnabled(ptChild, fEnabled) \
	D2WinOrdinal10021((DWORD)(ptChild), (DWORD)(fEnabled))

#define D2WinChildSetVisible(ptChild, fVisible) \
	D2WinOrdinal10023((DWORD)(ptChild), (DWORD)(fVisible))

#define D2WinPaletteLoad(pszDat, pszPl2) \
	((BOOL)D2WinOrdinal10024((DWORD)(pszDat), (DWORD)(pszPl2)))

#define D2WinShowCursor(fShow) \
	D2WinOrdinal10025((DWORD)(fShow))

#define D2WinChildSetOnTop(ptChild, fOnTop) \
	D2WinOrdinal10026((DWORD)(ptChild), (DWORD)(fOnTop))

#define D2WinSetEscapeButton(ptChild) \
	D2WinOrdinal10027((DWORD)(ptChild))

#define D2WinSetInputChild(ptChild) \
	((BOOL)D2WinOrdinal10029((DWORD)(ptChild)))

#define D2WinDereference(ppValue) \
	((void *)D2WinOrdinal10031((DWORD)(ppValue)))

#define D2WinArchiveGetArchive() \
	D2WinOrdinal10034()

#define D2WinArchiveLoadCel(pszName, eType) \
	D2WinOrdinal10035((DWORD)(pszName), (DWORD)(eType))

#define D2WinArchiveFreeCel(hCelFile) \
	D2WinOrdinal10037((DWORD)(hCelFile))

#define D2WinTextBoxCreate(nX, nY, nWidth, nHeight, nMarginX, nMarginY, hCelFile, pfnCallback, dwTextFlags, \
		ptFont, ptBar) \
	((void *)D2WinOrdinal10038((DWORD)(nX), (DWORD)(nY), (DWORD)(nWidth), (DWORD)(nHeight), \
		(DWORD)(nMarginX), (DWORD)(nMarginY), (DWORD)(hCelFile), (DWORD)(pfnCallback), (DWORD)(dwTextFlags), \
		(DWORD)(ptFont), (DWORD)(ptBar)))

#define D2WinTextBoxAddLineUnicode(hChild, pwszText) \
	((BOOL)((DWORD (__fastcall *)(DWORD, DWORD))D2WinOrdinal10040)((DWORD)(hChild), (DWORD)(pwszText)))

#define D2WinTextBoxAddLineUTF(hChild, pszText) \
	((BOOL)D2WinOrdinal10041((DWORD)(hChild), (DWORD)(pszText)))

#define D2WinTextBoxAddColorLineUTF(hChild, pszText, dwColor) \
	((BOOL)D2WinOrdinal10042((DWORD)(hChild), (DWORD)(pszText), (DWORD)(dwColor)))

#define D2WinClearTextBox(hChild) \
	((BOOL)D2WinOrdinal10043((DWORD)(hChild)))

#define D2WinTextBoxGetSelectedNum(hChild) \
	((int)D2WinOrdinal10045((DWORD)(hChild)))

#define D2WinTextBoxSetSelectedNum(hChild, nLine) \
	((int)D2WinOrdinal10046((DWORD)(hChild), (DWORD)(nLine)))

#define D2WinTextBoxAddColorLineUnicode(hChild, pwszText, dwColor, bBroken) \
	((BOOL)D2WinOrdinal10047((DWORD)(hChild), (DWORD)(pwszText), (DWORD)(dwColor), (DWORD)(bBroken)))

#define D2WinTextBoxSetTop(hChild, nLine) \
	((BOOL)D2WinOrdinal10049((DWORD)(hChild), (DWORD)(nLine)))

#define D2WinTextBoxGetTop(hChild) \
	((int)D2WinOrdinal10050((DWORD)(hChild)))

#define D2WinTextBoxGetVisibleLines(hChild) \
	((int)D2WinOrdinal10051((DWORD)(hChild)))

#define D2WinTextBoxSetColumnX(hChild, nColumn, nX) \
	D2WinOrdinal10055((DWORD)(hChild), (DWORD)(nColumn), (DWORD)(nX))

#define D2WinTextBoxSetLineSpacing(hChild, nBrokenSpacing, nSpacing) \
	((BOOL)D2WinOrdinal10058((DWORD)(hChild), (DWORD)(nBrokenSpacing), (DWORD)(nSpacing)))

#define D2WinEditBoxSetKeyProc(hChild, pfnKeyProc) \
	D2WinOrdinal10067((DWORD)(hChild), (DWORD)(pfnKeyProc))

#define D2WinEditBoxSetKeyPostProc(hChild, pfnKeyPostProc) \
	D2WinOrdinal10068((DWORD)(hChild), (DWORD)(pfnKeyPostProc))

#define D2WinEditBoxSetStringUTF(hChild, pszText) \
	((BOOL)D2WinOrdinal10071((DWORD)(hChild), (DWORD)(pszText)))

#define D2WinEditBoxGetString(hChild) \
	((WORD *)D2WinOrdinal10072((DWORD)(hChild)))

#define D2WinEditBoxNumericCallback(ptEditData, nPos, pchKey) \
	((BOOL)D2WinOrdinal10073((DWORD)(ptEditData), (DWORD)(nPos), (DWORD)(pchKey)))

#define D2WinEditBoxAlphaCallback(ptEditData, nPos, pchKey) \
	((BOOL)D2WinOrdinal10074((DWORD)(ptEditData), (DWORD)(nPos), (DWORD)(pchKey)))

#define D2WinEditBoxSetTabNext(hChild, hNext) \
	D2WinOrdinal10075((DWORD)(hChild), (DWORD)(hNext))

#define D2WinEditBoxSetMaxLength(hChild, nMaxLength) \
	D2WinOrdinal10076((DWORD)(hChild), (DWORD)(nMaxLength))

#define D2WinImageSetPosition(ptImageData, nX, nY) \
	((BOOL)D2WinOrdinal10080((DWORD)(ptImageData), (DWORD)(nX), (DWORD)(nY)))

#define D2WinImageSetCel(ptImageData, nCel) \
	((BOOL)D2WinOrdinal10081((DWORD)(ptImageData), (DWORD)(nCel)))

#define D2WinImageSetDrawMode(ptImageData, dwDrawMode) \
	((BOOL)D2WinOrdinal10082((DWORD)(ptImageData), (DWORD)(dwDrawMode)))

#define D2WinImageGetCels(ptImageData) \
	((int)D2WinOrdinal10083((DWORD)(ptImageData)))

#define D2WinPopupAddEntry(ptEntry) \
	((BOOL)D2WinOrdinal10089((DWORD)(ptEntry)))

#define D2WinAnimImageDeselect(ptAnim) \
	((BOOL)D2WinOrdinal10097((DWORD)(ptAnim)))

#define D2WinAnimImageSetAnimate(ptAnim, fAnimate) \
	((BOOL)D2WinOrdinal10098((DWORD)(ptAnim), (DWORD)(fAnimate)))

#define D2WinAnimImageGetState(ptAnim) \
	((int)D2WinOrdinal10099((DWORD)(ptAnim)))

#define D2WinButtonClick(ptMsg) \
	((DWORD (__fastcall *)(void *))D2WinOrdinal10106)((void *)(ptMsg))

#define D2WinFontSet(eFont) \
	D2WinOrdinal10120((DWORD)(eFont))

#define D2WinListCreate(nX, nY, nWidth, nHeight, ptEntries, ptFont) \
	((void *)D2WinOrdinal10128((DWORD)(nX), (DWORD)(nY), (DWORD)(nWidth), (DWORD)(nHeight), \
		(DWORD)(ptEntries), (DWORD)(ptFont)))

#define D2WinD2CompFree(hCompositeUnit, fFreeUnit) \
	D2WinOrdinal10135((DWORD)(hCompositeUnit), (DWORD)(fFreeUnit))

#define D2WinD2CompSetMode(hCompositeUnit, eMode) \
	((BOOL)D2WinOrdinal10136((DWORD)(hCompositeUnit), (DWORD)(eMode)))

#define D2WinD2CompCreate(eClass, eMode, pbCodes, pbColors) \
	((void *)D2WinOrdinal10137((DWORD)(eClass), (DWORD)(eMode), (DWORD)(pbCodes), (DWORD)(pbColors)))

#define D2WinD2CompSetDirection(ptUnit, bDirection) \
	D2WinOrdinal10142((DWORD)(ptUnit), (DWORD)(bDirection))

#define D2WinD2CompSetFrame(ptUnit, nFrame) \
	D2WinOrdinal10144((DWORD)(ptUnit), (DWORD)(nFrame))

#define D2WinD2CompSetX(ptUnit, nX) \
	D2WinOrdinal10145((DWORD)(ptUnit), (DWORD)(nX))

#define D2WinD2CompSetY(ptUnit, nY) \
	D2WinOrdinal10146((DWORD)(ptUnit), (DWORD)(nY))

#define D2WinD2CompSetFlags(hCompositeUnit, dwFlags) \
	D2WinOrdinal10149((DWORD)(hCompositeUnit), (DWORD)(dwFlags))

#define D2WinD2CompSetEmblem(hCompositeUnit, bIcon, bColorA, bColorB) \
	D2WinOrdinal10151((DWORD)(hCompositeUnit), (DWORD)(bIcon), (DWORD)(bColorA), (DWORD)(bColorB))

#define D2WinD2CompGetTitleOf(eClass, nNumber, fHigher) \
	((const char *)D2WinOrdinal10154((DWORD)(eClass), (DWORD)(nNumber), (DWORD)(fHigher)))

#define D2WinArchiveOpenPlayDisc(pfnRetry) \
	((BOOL)D2WinOrdinal10164((DWORD)(pfnRetry)))

#define D2WinArchiveOpenCinematicsDisc(pfnRetry) \
	((BOOL)D2WinOrdinal10165((DWORD)(pfnRetry)))

#define D2WinArchiveCloseDiscs() \
	D2WinOrdinal10166()

#define D2WinTimerDisableKey(ptTimerData) \
	D2WinOrdinal10173((DWORD)(ptTimerData))

#define D2WinTimerSetPeriodic(ptTimerData) \
	D2WinOrdinal10175((DWORD)(ptTimerData))

#define D2WinArchiveNeedPlayDisc() \
	((BOOL)D2WinOrdinal10178())

#define D2WinCutsceneCloseWindow(nResolution) \
	D2WinCutsceneCloseWindow((DWORD)(nResolution))

#define D2WinCutsceneCreateWindow() \
	D2WinCutsceneCreateWindow()

#define D2WinSetQuitCallback(pfnQuit) \
	D2WinSetQuitCallback((DWORD)(pfnQuit))

#define D2WinD2CompGetTitleOfRaw D2WinOrdinal10154

#define D2WinArchiveOpenPlayDiscRaw D2WinOrdinal10164

#define D2WinArchiveOpenCinematicsDiscRaw D2WinOrdinal10165

#define D2LangStrNewTable() \
	((BOOL)D2LangOrdinal10000())

#define D2LangStrDeleteTable() \
	D2LangOrdinal10001()

#define D2LangStrGetStringByIndex(wStringId) \
	((void *)D2LangOrdinal10004((WORD)(wStringId)))

#define D2LangStrGetLanguageName(pszDst, eField) \
	D2LangOrdinal10006((DWORD)(pszDst), (DWORD)(eField))

#define D2LangUnicodeDefaultConstructorClosure(ptThis) \
	D2LangOrdinal10017((DWORD)(ptThis))

#define D2LangUnicodestrcat(pszDst, pszSrc) \
	((void *)Unicode::strcat((Unicode *)(DWORD)(pszDst), (const Unicode *)(DWORD)(pszSrc)))

#define D2LangUnicodestrcmp(pszStr1, pszStr2) \
	((int)Unicode::strcmp((const Unicode *)(DWORD)(pszStr1), (const Unicode *)(DWORD)(pszStr2)))

#define D2LangUnicodestrcpy(pszDst, pszSrc) \
	((void *)Unicode::strcpy((Unicode *)(DWORD)(pszDst), (const Unicode *)(DWORD)(pszSrc)))

#define D2LangUnicodestrlen(pszStr) \
	((int)Unicode::strlen((const Unicode *)(DWORD)(pszStr)))

#define D2LangUnicodestrncpy(pszDst, pszSrc, nCount) \
	((void *)Unicode::strncpy((Unicode *)(DWORD)(pszDst), (const Unicode *)(DWORD)(pszSrc), (int)(nCount)))

#define D2LangUnicodestrnicmp(pszStr1, pszStr2, nCount) \
	((int)D2LangOrdinal10043((DWORD)(pszStr1), (DWORD)(pszStr2), (DWORD)(nCount)))

#define D2LangUnicodestrstr(pszStr, pszSub) \
	((void *)D2LangOrdinal10044((DWORD)(pszStr), (DWORD)(pszSub)))

#define D2LangUnicodetoUnicode(pszDst, pszSrc, nCount) \
	((void *)Unicode::toUnicode((Unicode *)(DWORD)(pszDst), (const char *)(DWORD)(pszSrc), (int)(nCount)))

#define D2LangUnicodetoUtf(pszDst, pszSrc, nCount) \
	((char *)Unicode::toUtf((char *)(DWORD)(pszDst), (const Unicode *)(DWORD)(pszSrc), (int)(nCount)))

#define D2LangUnicodeunicode2Win(pszDst, pszSrc, nCount) \
	((char *)Unicode::unicode2Win((char *)(DWORD)(pszDst), (const Unicode *)(DWORD)(pszSrc), (int)(nCount)))

#define D2LangUnicodeunicodenwidth(pszSrc, nBytes) \
	((unsigned int)D2LangOrdinal10055((DWORD)(pszSrc), (DWORD)(nBytes)))

#define D2LangUnicodewin2Unicode(pszDst, pszSrc, nCount) \
	((void *)Unicode::win2Unicode((Unicode *)(DWORD)(pszDst), (const char *)(DWORD)(pszSrc), (int)(nCount)))

#define D2LangStrNewTableRaw D2LangOrdinal10000

#define D2gfxWindowGetHandle() \
	((HWND)D2gfxOrdinal10029())

#define D2gfxWindowPlayCutscene(dwFileName, dwMode, dwTickProc) \
	D2gfxOrdinal10033((DWORD)(dwFileName), (DWORD)(dwMode), (DWORD)(dwTickProc))

#define D2gfxWindowOpenSmack(dwFileName, dwVideo) \
	D2gfxOrdinal10035((DWORD)(dwFileName), (DWORD)(dwVideo))

#define D2gfxWindowDrawSmackFrame(dwArg) \
	D2gfxOrdinal10036((DWORD)(dwArg))

#define D2gfxWindowCloseSmack(dwArg) \
	D2gfxOrdinal10037((DWORD)(dwArg))

#define D2gfxScaleDrawCel(dwCelContext, dwX, dwY, dwColor, dwMode, dwPreMap) \
	D2gfxOrdinal10076((DWORD)(dwCelContext), (DWORD)(dwX), (DWORD)(dwY), (DWORD)(dwColor), (DWORD)(dwMode), \
		(DWORD)(dwPreMap))

#define D2SoundSmpSetMasterVolume(nVolume) \
	D2soundOrdinal10027((DWORD)(nVolume))

#define D2SoundSmpMusicStop() \
	((DWORD (__fastcall *)(void))D2soundOrdinal10031)()

#define D2SoundSmpMusicSetState(dwState) \
	D2soundOrdinal10034((DWORD)(dwState))

#define D2SoundFastInit() \
	((DWORD (__fastcall *)(void))D2soundOrdinal10036)()

#define D2SoundFastShutdown() \
	((DWORD (__fastcall *)(void))D2soundOrdinal10037)()

#define D2SoundFastLoad(pszFile) \
	D2soundOrdinal10038((DWORD)(pszFile))

#define D2SoundFastPlay(pszFile) \
	D2soundOrdinal10039((DWORD)(pszFile))

#define D2SoundFastIsPlaying() \
	((BOOL)((DWORD (__fastcall *)(void))D2soundOrdinal10040)())

#define D2SoundFastFadeAll() \
	((DWORD (__fastcall *)(void))D2soundOrdinal10042)()

#define D2SoundGetOptionsMusic() \
	((DWORD (__fastcall *)(void))D2soundOrdinal10069)()

#define D2SoundSmpFadeOutMasterRaw D2soundOrdinal10029

#define D2MCPClientD2MCPConnect(ptConnect) \
	((HANDLE)D2MCPClientOrdinal10000((DWORD)(ptConnect)))

#define D2MCPClientD2MCPClose() \
	D2MCPClientOrdinal10001()

#define D2MCPClientD2MCPIsConnected() \
	((BOOL)D2MCPClientOrdinal10002())

#define D2MCPClientD2MCPPoll() \
	((BOOL)D2MCPClientOrdinal10003())

#define D2MCPClientToMCPCreateCharacter(pszName, dwClass, dwFlags) \
	((BOOL)D2MCPClientOrdinal10008((DWORD)(pszName), (DWORD)(dwClass), (WORD)(dwFlags)))

#define D2MCPClientToMCPSelectCharacter(pszName) \
	((BOOL)D2MCPClientOrdinal10009((DWORD)(pszName)))

#define D2MCPClientToMCPSendDeleteCharacter(wToken) \
	((BOOL)D2MCPClientOrdinal10021((DWORD)(wToken)))

#define D2MCPClientD2MCPSetCreateCharacterReply(f) \
	D2MCPClientOrdinal10028((DWORD)(f))

#define D2MCPClientD2MCPGetCreateCharacterReply() \
	((BOOL)D2MCPClientOrdinal10029())

#define D2MCPClientD2MCPSetSelectCharacterReply(f) \
	D2MCPClientOrdinal10030((DWORD)(f))

#define D2MCPClientD2MCPGetSelectCharacterReply() \
	((BOOL)D2MCPClientOrdinal10031())

#define D2MCPClientD2MCPSetDeleteCharacterReply(f) \
	D2MCPClientOrdinal10032((DWORD)(f))

#define D2MCPClientD2MCPGetDeleteCharacterReply() \
	((BOOL)D2MCPClientOrdinal10033())

#define D2MCPClientD2MCPGetStatus() \
	D2MCPClientOrdinal10046()

#define D2NetClientInit(nMode, pszHost) \
	D2NetOrdinal10000((DWORD)(nMode), (DWORD)(pszHost))

#define D2NetClientClose() \
	((int)D2NetOrdinal10001())

#define D2NetGetCltSysMsg(pbBuffer, dwSize) \
	((int)D2NetOrdinal10008((DWORD)(pbBuffer), (DWORD)(dwSize)))

#define D2NetClientPollConnect() \
	((int)D2NetOrdinal10025())

#define FogSafesockGetLocalAddress() \
	((const char *)FogOrdinal10012())

#define FogD2QSSocketReadSevenBitPair(pbData) \
	((int)FogOrdinal10063((DWORD)(pbData)))

#define FogD2QSSocketWriteSevenBitPair(pbData, dwValue) \
	FogOrdinal10064((DWORD)(pbData), (DWORD)(dwValue))

#define FogSafesockCreateFile(pszFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, \
		dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile) \
	((HANDLE)FogOrdinal10080((DWORD)(pszFileName), (DWORD)(dwDesiredAccess), (DWORD)(dwShareMode), \
		(DWORD)(lpSecurityAttributes), (DWORD)(dwCreationDisposition), (DWORD)(dwFlagsAndAttributes), \
		(DWORD)(hTemplateFile)))

#define FogSafesockDeleteFile(pszFileName) \
	((BOOL)FogOrdinal10081((DWORD)(pszFileName)))

#define FogSafesockCloseHandle(hObject) \
	((BOOL)FogOrdinal10082((DWORD)(hObject)))

#define FogSafesockReadFile(hFile, lpBuffer, dwToRead, lpdwRead, lpOverlapped) \
	((BOOL)FogOrdinal10083((DWORD)(hFile), (DWORD)(lpBuffer), (DWORD)(dwToRead), (DWORD)(lpdwRead), \
		(DWORD)(lpOverlapped)))

#define FogSafesockWriteFile(hFile, lpBuffer, dwToWrite, lpdwWritten, lpOverlapped) \
	((BOOL)FogOrdinal10084((DWORD)(hFile), (DWORD)(lpBuffer), (DWORD)(dwToWrite), (DWORD)(lpdwWritten), \
		(DWORD)(lpOverlapped)))

#define FogSafesockGetSavePath(pszPath, dwSize) \
	((BOOL)FogOrdinal10086((DWORD)(pszPath), (DWORD)(dwSize)))

#define FogCpuIsSlow() \
	((BOOL)((DWORD (__fastcall *)(void))FogOrdinal10136)())

#define StormSMsgDispatchMessage(hWnd, uMsg, wParam, lParam, pfHandled, plResult) \
	((BOOL)StormOrdinal412((DWORD)(hWnd), (DWORD)(uMsg), (DWORD)(wParam), (DWORD)(lParam), \
		(DWORD)(pfHandled), (DWORD)(plResult)))

#define StormSRegLoadString(pszKey, pszValue, dwFlags, pszBuffer, dwBufferChars) \
	((BOOL)StormOrdinal422((DWORD)(pszKey), (DWORD)(pszValue), (DWORD)(dwFlags), (DWORD)(pszBuffer), \
		(DWORD)(dwBufferChars)))

#define StormSRegSaveString(pszKey, pszValue, dwFlags, pszString) \
	((BOOL)StormOrdinal425((DWORD)(pszKey), (DWORD)(pszValue), (DWORD)(dwFlags), (DWORD)(pszString)))

#define StormSRegSaveValue(pszKey, pszValue, dwFlags, dwValue) \
	((BOOL)StormOrdinal426((DWORD)(pszKey), (DWORD)(pszValue), (DWORD)(dwFlags), (DWORD)(dwValue)))

#define StormSMemCopy(pDest, pSource, dwBytes) \
	StormOrdinal491((DWORD)(pDest), (DWORD)(pSource), (DWORD)(dwBytes))

#define StormSStrCmp(pszString1, pszString2, dwMaxLength) \
	((int)StormOrdinal508((const char *)(pszString1), (const char *)(pszString2), (DWORD)(dwMaxLength)))

#define StormSStrPrintf \
	StormOrdinal578

#endif
