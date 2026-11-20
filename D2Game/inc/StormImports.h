#ifndef D2RE_STORM_IMPORTS_WRAPPERS_H
#define D2RE_STORM_IMPORTS_WRAPPERS_H

#include "Storm.imports.h"

#define StormSNetCreateGame(pszGameName, pszGamePassword, pszGameDescription, dwGameCategory, pGameData, \
		dwGameDataBytes, dwMaxPlayers, pszPlayerName, pszPlayerDescription, pdwPlayerId) \
	((BOOL)StormOrdinal101((DWORD)(pszGameName), (DWORD)(pszGamePassword), (DWORD)(pszGameDescription), \
		(DWORD)(dwGameCategory), (DWORD)(pGameData), (DWORD)(dwGameDataBytes), (DWORD)(dwMaxPlayers), \
		(DWORD)(pszPlayerName), (DWORD)(pszPlayerDescription), (DWORD)(pdwPlayerId)))

#define StormSNetDestroy() \
	((BOOL)StormOrdinal102())

#define StormSNetEnumDevices(pfnCallback) \
	((BOOL)((DWORD (__stdcall *)(DWORD))StormOrdinal103)((DWORD)(pfnCallback)))

#define StormSNetEnumGames(dwCategoryBits, dwCategoryMask, pfnCallback, pdwHintNextCall) \
	((BOOL)((DWORD (__stdcall *)(DWORD, DWORD, DWORD, void *))StormOrdinal104)((DWORD)(dwCategoryBits), \
		(DWORD)(dwCategoryMask), (DWORD)(pfnCallback), (void *)(pdwHintNextCall)))

#define StormSNetEnumProviders(ptMinCaps, pfnCallback) \
	((BOOL)StormOrdinal105((DWORD)(ptMinCaps), (DWORD)(pfnCallback)))

#define StormSNetDropPlayer(dwPlayer, dwFlags) \
	((BOOL)StormOrdinal106((DWORD)(dwPlayer), (DWORD)(dwFlags)))

#define StormSNetGetGameInfo(dwType, pBuffer, dwBufferBytes, pdwBytesWritten) \
	((BOOL)StormOrdinal107((DWORD)(dwType), (DWORD)(pBuffer), (DWORD)(dwBufferBytes), \
		(DWORD)(pdwBytesWritten)))

#define StormSNetGetNetworkLatency(dwType, pdwResult) \
	((BOOL)StormOrdinal108((DWORD)(dwType), (DWORD)(pdwResult)))

#define StormSNetGetNumPlayers(pdwFirstPlayer, pdwLastPlayer, pdwActivePlayers) \
	((BOOL)StormOrdinal109((DWORD)(pdwFirstPlayer), (DWORD)(pdwLastPlayer), (DWORD)(pdwActivePlayers)))

#define StormSNetGetOwnerTurnsWaiting(pdwTurns) \
	((BOOL)StormOrdinal110((DWORD)(pdwTurns)))

#define StormSNetGetPerformanceData(dwCounter, pdwValue, pdwType, pdwScale, ptTime, ptFrequency) \
	((BOOL)StormOrdinal111((DWORD)(dwCounter), (DWORD)(pdwValue), (DWORD)(pdwType), (DWORD)(pdwScale), \
		(DWORD)(ptTime), (DWORD)(ptFrequency)))

#define StormSNetGetPlayerCaps(dwPlayer, ptCaps) \
	((BOOL)StormOrdinal112((DWORD)(dwPlayer), (DWORD)(ptCaps)))

#define StormSNetGetPlayerName(dwPlayer, pszName, dwNameChars) \
	((BOOL)StormOrdinal113((DWORD)(dwPlayer), (DWORD)(pszName), (DWORD)(dwNameChars)))

#define StormSNetGetProviderCaps(ptCaps) \
	((BOOL)StormOrdinal114((DWORD)(ptCaps)))

#define StormSNetGetTurnsInTransit(pdwTurns) \
	((BOOL)StormOrdinal115((DWORD)(pdwTurns)))

#define StormSNetInitializeDevice(dwDeviceId, ptApp, ptUser, ptUi, ptVer) \
	((BOOL)StormOrdinal116((DWORD)(dwDeviceId), (DWORD)(ptApp), (DWORD)(ptUser), (DWORD)(ptUi), \
		(DWORD)(ptVer)))

#define StormSNetInitializeProvider(dwProviderId, ptApp, ptUser, ptUi, ptVer) \
	((BOOL)StormOrdinal117((DWORD)(dwProviderId), (DWORD)(ptApp), (DWORD)(ptUser), (DWORD)(ptUi), \
		(DWORD)(ptVer)))

#define StormSNetJoinGame(dwGameId, pszGameName, pszGamePassword, pszPlayerName, pszPlayerDescription, \
		pdwPlayerId) \
	((BOOL)StormOrdinal118((DWORD)(dwGameId), (DWORD)(pszGameName), (DWORD)(pszGamePassword), \
		(DWORD)(pszPlayerName), (DWORD)(pszPlayerDescription), (DWORD)(pdwPlayerId)))

#define StormSNetLeaveGame(dwExitCode) \
	((BOOL)StormOrdinal119((DWORD)(dwExitCode)))

#define StormSNetPerformUpgrade(pdwStatus) \
	((BOOL)StormOrdinal120((DWORD)(pdwStatus)))

#define StormSNetReceiveMessage(pdwPlayer, ppData, pdwBytes) \
	((BOOL)StormOrdinal121((DWORD)(pdwPlayer), (DWORD)(ppData), (DWORD)(pdwBytes)))

#define StormSNetReceiveTurns(dwFirstPlayer, dwNumPlayers, ppData, pdwBytes, pdwStatus) \
	((BOOL)StormOrdinal122((DWORD)(dwFirstPlayer), (DWORD)(dwNumPlayers), (DWORD)(ppData), \
		(DWORD)(pdwBytes), (DWORD)(pdwStatus)))

#define StormSNetRegisterEventHandler(dwEvent, pfnHandler) \
	((BOOL)StormOrdinal123((DWORD)(dwEvent), (DWORD)(pfnHandler)))

#define StormSNetResetLatencyMeasurements() \
	((BOOL)StormOrdinal124())

#define StormSNetSelectGame(dwFlags, ptApp, ptUser, ptUi, ptVer, pdwPlayerId) \
	((BOOL)StormOrdinal125((DWORD)(dwFlags), (DWORD)(ptApp), (DWORD)(ptUser), (DWORD)(ptUi), (DWORD)(ptVer), \
		(DWORD)(pdwPlayerId)))

#define StormSNetSelectProvider(ptMinCaps, ptApp, ptUser, ptUi, ptVer, pdwProviderId) \
	((BOOL)StormOrdinal126((DWORD)(ptMinCaps), (DWORD)(ptApp), (DWORD)(ptUser), (DWORD)(ptUi), \
		(DWORD)(ptVer), (DWORD)(pdwProviderId)))

#define StormSNetSendMessage(dwPlayer, pData, dwBytes) \
	((BOOL)StormOrdinal127((DWORD)(dwPlayer), (DWORD)(pData), (DWORD)(dwBytes)))

#define StormSNetSendTurn(pData, dwBytes) \
	((BOOL)StormOrdinal128((DWORD)(pData), (DWORD)(dwBytes)))

#define StormSNetSetBasePlayer(dwBasePlayer) \
	((BOOL)StormOrdinal129((DWORD)(dwBasePlayer)))

#define StormSNetSetGameMode(dwGameMode) \
	((BOOL)StormOrdinal130((DWORD)(dwGameMode)))

#define StormSNetUnregisterEventHandler(dwEvent, pfnHandler) \
	((BOOL)StormOrdinal131((DWORD)(dwEvent), (DWORD)(pfnHandler)))

#define StormSNetEnumGamesEx(dwCategoryBits, dwCategoryMask, pfnCallback, pdwHintNextCall) \
	((BOOL)StormOrdinal133((DWORD)(dwCategoryBits), (DWORD)(dwCategoryMask), (DWORD)(pfnCallback), \
		(DWORD)(pdwHintNextCall)))

#define StormSNetSetServerChatCommand(pszCommand) \
	((BOOL)StormOrdinal134((DWORD)(pszCommand)))

#define StormSNetSendDatagram(dwPlayer, pData, dwBytes) \
	((BOOL)StormOrdinal135((DWORD)(dwPlayer), (DWORD)(pData), (DWORD)(dwBytes)))

#define StormSNetReceiveDatagram(pdwPlayer, ppData, pdwBytes) \
	((BOOL)StormOrdinal136((DWORD)(pdwPlayer), (DWORD)(ppData), (DWORD)(pdwBytes)))

#define StormSNetDisconnectAll(dwFlags) \
	((BOOL)StormOrdinal137((DWORD)(dwFlags)))

#define StormSNetCreateLadderGame(pszGameName, pszGamePassword, pszGameDescription, dwGameCategory, \
		dwGameLadder, dwGameFlags, pGameData, dwGameDataBytes, dwMaxPlayers, pszPlayerName, \
		pszPlayerDescription, pdwPlayerId) \
	((BOOL)StormOrdinal138((DWORD)(pszGameName), (DWORD)(pszGamePassword), (DWORD)(pszGameDescription), \
		(DWORD)(dwGameCategory), (DWORD)(dwGameLadder), (DWORD)(dwGameFlags), (DWORD)(pGameData), \
		(DWORD)(dwGameDataBytes), (DWORD)(dwMaxPlayers), (DWORD)(pszPlayerName), \
		(DWORD)(pszPlayerDescription), (DWORD)(pdwPlayerId)))

#define StormSNetReportGameResult(dwFirstPlayer, dwNumPlayers, pdwResults, pszText1, pszText2) \
	((BOOL)StormOrdinal139((DWORD)(dwFirstPlayer), (DWORD)(dwNumPlayers), (DWORD)(pdwResults), \
		(DWORD)(pszText1), (DWORD)(pszText2)))

#define StormSNetCheckDataFile(pszFile, pbData, dwDataBytes, pdwResult) \
	((BOOL)StormOrdinal140((DWORD)(pszFile), (DWORD)(pbData), (DWORD)(dwDataBytes), (DWORD)(pdwResult)))

#define StormSDlgBeginPaint(hWnd, ptPaint) \
	((HDC)StormOrdinal201((DWORD)(hWnd), (DWORD)(ptPaint)))

#define StormSDlgBltToWindowInclusive(hWnd, hRgn, nX, nY, pbBits, prc, ptSize, nReserved, dwPattern, dwRop) \
	((BOOL)StormOrdinal202((DWORD)(hWnd), (DWORD)(hRgn), (DWORD)(nX), (DWORD)(nY), (DWORD)(pbBits), \
		(DWORD)(prc), (DWORD)(ptSize), (DWORD)(nReserved), (DWORD)(dwPattern), (DWORD)(dwRop)))

#define StormSDlgCheckTimers() \
	((BOOL)StormOrdinal203())

#define StormSDlgCreateDialogIndirectParam(hInstance, ptTemplate, hParent, pfnDialog, lParam) \
	((HWND)StormOrdinal204((DWORD)(hInstance), (DWORD)(ptTemplate), (DWORD)(hParent), (DWORD)(pfnDialog), \
		(DWORD)(lParam)))

#define StormSDlgCreateDialogParam(hInstance, pszName, hParent, pfnDialog, lParam) \
	((HWND)StormOrdinal205((DWORD)(hInstance), (DWORD)(pszName), (DWORD)(hParent), (DWORD)(pfnDialog), \
		(DWORD)(lParam)))

#define StormSDlgDefDialogProc(hDlg, uMsg, wParam, lParam) \
	((INT_PTR)StormOrdinal206((DWORD)(hDlg), (DWORD)(uMsg), (DWORD)(wParam), (DWORD)(lParam)))

#define StormSDlgDialogBoxIndirectParam(hInstance, ptTemplate, hParent, pfnDialog, lParam) \
	((INT_PTR)StormOrdinal208((DWORD)(hInstance), (DWORD)(ptTemplate), (DWORD)(hParent), (DWORD)(pfnDialog), \
		(DWORD)(lParam)))

#define StormSDlgDialogBoxParam(hInstance, pszName, hParent, pfnDialog, lParam) \
	((INT_PTR)StormOrdinal209((DWORD)(hInstance), (DWORD)(pszName), (DWORD)(hParent), (DWORD)(pfnDialog), \
		(DWORD)(lParam)))

#define StormSDlgDrawBitmap(hWnd, dwMask, hRgn, nX, nY, prcInset, dwFlags) \
	((BOOL)StormOrdinal210((DWORD)(hWnd), (DWORD)(dwMask), (DWORD)(hRgn), (DWORD)(nX), (DWORD)(nY), \
		(DWORD)(prcInset), (DWORD)(dwFlags)))

#define StormSDlgEndDialog(hDlg, nResult) \
	((BOOL)StormOrdinal211((DWORD)(hDlg), (DWORD)(nResult)))

#define StormSDlgEndPaint(hWnd, ptPaint) \
	((BOOL)StormOrdinal212((DWORD)(hWnd), (DWORD)(ptPaint)))

#define StormSDlgSetBaseFont(nValue0, nValue1, nValue2, nValue3, pszFace) \
	((BOOL)StormOrdinal214((DWORD)(nValue0), (DWORD)(nValue1), (DWORD)(nValue2), (DWORD)(nValue3), \
		(DWORD)(pszFace)))

#define StormSDlgSetBitmapInclusive(hWnd, hParent, pszClass, dwStyle, dwMask, pbBits, prc, nWidth, nHeight, \
		nReserved) \
	((BOOL)StormOrdinal215((DWORD)(hWnd), (DWORD)(hParent), (DWORD)(pszClass), (DWORD)(dwStyle), \
		(DWORD)(dwMask), (DWORD)(pbBits), (DWORD)(prc), (DWORD)(nWidth), (DWORD)(nHeight), \
		(DWORD)(nReserved)))

#define StormSDlgSetControlBitmaps(hDlg, pnIds, pdwStates, pbBits, ptSize, nLayout, nReserved) \
	((BOOL)StormOrdinal216((DWORD)(hDlg), (DWORD)(pnIds), (DWORD)(pdwStates), (DWORD)(pbBits), \
		(DWORD)(ptSize), (DWORD)(nLayout), (DWORD)(nReserved)))

#define StormSDlgSetSystemCursor(hDlg, hCursor, dwId, pdwOld) \
	((BOOL)StormOrdinal217((DWORD)(hDlg), (DWORD)(hCursor), (DWORD)(dwId), (DWORD)(pdwOld)))

#define StormSDlgSetCursor(pbMask, pbImage, ptSize, dwId) \
	((BOOL)StormOrdinal218((DWORD)(pbMask), (DWORD)(pbImage), (DWORD)(ptSize), (DWORD)(dwId)))

#define StormSDlgSetTimer(hWnd, uId, uPeriod, pfnTimer) \
	((UINT)StormOrdinal219((DWORD)(hWnd), (DWORD)(uId), (DWORD)(uPeriod), (DWORD)(pfnTimer)))

#define StormSDlgUpdateCursor() \
	((BOOL)StormOrdinal220())

#define StormSDlgBltToWindow(hWnd, hRgn, nX, nY, pbBits, prc, ptSize, nReserved, dwPattern, dwRop) \
	((BOOL)StormOrdinal221((DWORD)(hWnd), (DWORD)(hRgn), (DWORD)(nX), (DWORD)(nY), (DWORD)(pbBits), \
		(DWORD)(prc), (DWORD)(ptSize), (DWORD)(nReserved), (DWORD)(dwPattern), (DWORD)(dwRop)))

#define StormSDlgSetBitmap(hWnd, hParent, pszClass, dwStyle, dwMask, pbBits, prc, nWidth, nHeight, \
		nReserved) \
	((BOOL)StormOrdinal222((DWORD)(hWnd), (DWORD)(hParent), (DWORD)(pszClass), (DWORD)(dwStyle), \
		(DWORD)(dwMask), (DWORD)(pbBits), (DWORD)(prc), (DWORD)(nWidth), (DWORD)(nHeight), \
		(DWORD)(nReserved)))

#define StormSDlgSetLocale(nCategory, pszLocale) \
	((char *)StormOrdinal223((DWORD)(nCategory), (DWORD)(pszLocale)))

#define StormSFileAuthenticateArchive(hArchive, pdwResult) \
	((BOOL)StormOrdinal251((DWORD)(hArchive), (DWORD)(pdwResult)))

#define StormSFileCloseArchive(hArchive) \
	((BOOL)((DWORD (__stdcall *)(DWORD))StormOrdinal252)((DWORD)(hArchive)))

#define StormSFileCloseFile(hFile) \
	((BOOL)((DWORD (__stdcall *)(DWORD))StormOrdinal253)((DWORD)(hFile)))

#define StormSFileDdaBegin(hFile, dwBufferSize, dwFlags) \
	((BOOL)StormOrdinal254((DWORD)(hFile), (DWORD)(dwBufferSize), (DWORD)(dwFlags)))

#define StormSFileDdaBeginAt(hFile, dwBufferSize, dwFlags, dwOffset, lVolume, lPan, pBuffer) \
	((BOOL)StormOrdinal255((DWORD)(hFile), (DWORD)(dwBufferSize), (DWORD)(dwFlags), (DWORD)(dwOffset), \
		(DWORD)(lVolume), (DWORD)(lPan), (DWORD)(pBuffer)))

#define StormSFileDdaDestroy() \
	((BOOL)((DWORD (__stdcall *)(void))StormOrdinal256)())

#define StormSFileDdaEnd(hFile) \
	((BOOL)((DWORD (__stdcall *)(DWORD))StormOrdinal257)((DWORD)(hFile)))

#define StormSFileDdaGetPos(hFile, pdwPosition, pdwMaxPosition) \
	((BOOL)StormOrdinal258((DWORD)(hFile), (DWORD)(pdwPosition), (DWORD)(pdwMaxPosition)))

#define StormSFileDdaGetVolume(hFile, plVolume, plPan) \
	((BOOL)StormOrdinal259((DWORD)(hFile), (DWORD)(plVolume), (DWORD)(plPan)))

#define StormSFileDdaInitialize(pDirectSound) \
	((BOOL)StormOrdinal260((DWORD)(pDirectSound)))

#define StormSFileDdaSetVolume(hFile, lVolume, lPan) \
	((BOOL)((DWORD (__stdcall *)(DWORD, DWORD, DWORD))StormOrdinal261)((DWORD)(hFile), (DWORD)(lVolume), \
		(DWORD)(lPan)))

#define StormSFileEnableDirectAccess(dwFlags) \
	((BOOL)StormOrdinal263((DWORD)(dwFlags)))

#define StormSFileGetFileArchive(hFile, phArchive) \
	((BOOL)StormOrdinal264((DWORD)(hFile), (DWORD)(phArchive)))

#define StormSFileGetFileSize(hFile, pdwFileSizeHigh) \
	StormOrdinal265((DWORD)(hFile), (DWORD)(pdwFileSizeHigh))

#define StormSFileOpenArchive(pszName, nPriority, dwFlags, phArchive) \
	((BOOL)StormOrdinal266((DWORD)(pszName), (DWORD)(nPriority), (DWORD)(dwFlags), (DWORD)(phArchive)))

#define StormSFileOpenFile(pszName, phFile) \
	((BOOL)StormOrdinal267((DWORD)(pszName), (DWORD)(phFile)))

#define StormSFileOpenFileEx(hArchive, pszName, dwFlags, phFile) \
	((BOOL)StormOrdinal268((DWORD)(hArchive), (DWORD)(pszName), (DWORD)(dwFlags), (DWORD)(phFile)))

#define StormSFileReadFile(hFile, pBuffer, dwBytes, pdwRead, ptOverlapped) \
	((BOOL)StormOrdinal269((DWORD)(hFile), (DWORD)(pBuffer), (DWORD)(dwBytes), (DWORD)(pdwRead), \
		(DWORD)(ptOverlapped)))

#define StormSFileSetBasePath(pszPath) \
	((BOOL)StormOrdinal270((DWORD)(pszPath)))

#define StormSFileSetFilePointer(hFile, lDistance, plDistanceHigh, dwMethod) \
	StormOrdinal271((DWORD)(hFile), (DWORD)(lDistance), (DWORD)(plDistanceHigh), (DWORD)(dwMethod))

#define StormSFileSetLocale(wLocale) \
	StormOrdinal272((DWORD)(wLocale))

#define StormSFileGetBasePath(pszBuffer, dwBufferChars) \
	((BOOL)StormOrdinal273((DWORD)(pszBuffer), (DWORD)(dwBufferChars)))

#define StormSFileSetIoErrorMode(dwMode, pfnCallback) \
	((BOOL)StormOrdinal274((DWORD)(dwMode), (DWORD)(pfnCallback)))

#define StormSFileGetArchiveName(hArchive, pszBuffer, dwBufferChars) \
	((BOOL)StormOrdinal275((DWORD)(hArchive), (DWORD)(pszBuffer), (DWORD)(dwBufferChars)))

#define StormSFileGetFileName(hFile, pszBuffer, dwBufferChars) \
	((BOOL)StormOrdinal276((DWORD)(hFile), (DWORD)(pszBuffer), (DWORD)(dwBufferChars)))

#define StormSFileGetArchiveInfo(hArchive, pnPriority, pfCdRom) \
	((BOOL)StormOrdinal277((DWORD)(hArchive), (DWORD)(pnPriority), (DWORD)(pfCdRom)))

#define StormSFileSetPlatform(bPlatform) \
	StormOrdinal278((DWORD)(bPlatform))

#define StormSFileLoadFile(pszName, ppBuffer, pdwSize, dwExtra, ptOverlapped) \
	((BOOL)StormOrdinal279((DWORD)(pszName), (DWORD)(ppBuffer), (DWORD)(pdwSize), (DWORD)(dwExtra), \
		(DWORD)(ptOverlapped)))

#define StormSFileUnloadFile(pBuffer) \
	((BOOL)StormOrdinal280((DWORD)(pBuffer)))

#define StormSFileLoadFileFrom(hArchive, pszName, ppBuffer, pdwSize, dwExtra, dwFlags, ptOverlapped) \
	((BOOL)StormOrdinal281((DWORD)(hArchive), (DWORD)(pszName), (DWORD)(ppBuffer), (DWORD)(pdwSize), \
		(DWORD)(dwExtra), (DWORD)(dwFlags), (DWORD)(ptOverlapped)))

#define StormSFilePrioritizeRequest(pKey, fUrgent) \
	StormOrdinal282((DWORD)(pKey), (DWORD)(fUrgent))

#define StormSFileCancelRequest(pKey) \
	((BOOL)StormOrdinal283((DWORD)(pKey)))

#define StormSFileSetAsyncRate(dwRate) \
	StormOrdinal284((DWORD)(dwRate))

#define StormSFileSetDataChunkSize(dwSize) \
	StormOrdinal285((DWORD)(dwSize))

#define StormSFileEnableSeekOptimization(fEnable) \
	StormOrdinal286((DWORD)(fEnable))

#define StormSFileReadFileEx(hFile, pBuffer, dwBytes, pdwRead, ptOverlapped, ptCallback) \
	((BOOL)StormOrdinal287((DWORD)(hFile), (DWORD)(pBuffer), (DWORD)(dwBytes), (DWORD)(pdwRead), \
		(DWORD)(ptOverlapped), (DWORD)(ptCallback)))

#define StormSFileCancelRequestChecked(pKey) \
	((BOOL)StormOrdinal288((DWORD)(pKey)))

#define StormDestroy() \
	((BOOL)StormOrdinal301())

#define StormGetInstance() \
	((HINSTANCE)StormOrdinal302())

#define StormGetOption(dwOption, pValue, pdwSize) \
	((BOOL)StormOrdinal303((DWORD)(dwOption), (DWORD)(pValue), (DWORD)(pdwSize)))

#define StormSetOption(dwOption, pValue, dwSize) \
	((BOOL)StormOrdinal304((DWORD)(dwOption), (DWORD)(pValue), (DWORD)(dwSize)))

#define StormSBltROP3Pattern(dwRop, pszBuffer, dwBufferSize, fSolid) \
	((BOOL)StormOrdinal312((DWORD)(dwRop), (DWORD)(pszBuffer), (DWORD)(dwBufferSize), (DWORD)(fSolid)))

#define StormSBltROP3Block(pbDest, pbSource, nWidth, nHeight, nDestPitch, nSourcePitch, dwPattern, dwRop) \
	((BOOL)StormOrdinal313((DWORD)(pbDest), (DWORD)(pbSource), (DWORD)(nWidth), (DWORD)(nHeight), \
		(DWORD)(nDestPitch), (DWORD)(nSourcePitch), (DWORD)(dwPattern), (DWORD)(dwRop)))

#define StormSBltROP3Clipped(pbDest, prcDest, ptDestSize, nDestPitch, pbSource, prcSource, ptSourceSize, \
		nSourcePitch, dwPattern, dwRop) \
	((BOOL)StormOrdinal314((DWORD)(pbDest), (DWORD)(prcDest), (DWORD)(ptDestSize), (DWORD)(nDestPitch), \
		(DWORD)(pbSource), (DWORD)(prcSource), (DWORD)(ptSourceSize), (DWORD)(nSourcePitch), \
		(DWORD)(dwPattern), (DWORD)(dwRop)))

#define StormSBltROP3Tiled(pbDest, prcDest, nDestPitch, pbSource, prcSource, nSourcePitch, nX, nY, \
		dwPattern, dwRop) \
	((BOOL)StormOrdinal315((DWORD)(pbDest), (DWORD)(prcDest), (DWORD)(nDestPitch), (DWORD)(pbSource), \
		(DWORD)(prcSource), (DWORD)(nSourcePitch), (DWORD)(nX), (DWORD)(nY), (DWORD)(dwPattern), \
		(DWORD)(dwRop)))

#define StormSBmpDecodeImage(dwType, pbData, dwSize, pPalette, pbDest, dwDestSize, pdwWidth, pdwHeight, \
		pdwBitDepth) \
	((BOOL)StormOrdinal321((DWORD)(dwType), (DWORD)(pbData), (DWORD)(dwSize), (DWORD)(pPalette), \
		(DWORD)(pbDest), (DWORD)(dwDestSize), (DWORD)(pdwWidth), (DWORD)(pdwHeight), (DWORD)(pdwBitDepth)))

#define StormSBmpLoadImage(pszName, pPalette, pbDest, dwDestSize, pdwWidth, pdwHeight, pdwBitDepth) \
	((BOOL)StormOrdinal323((DWORD)(pszName), (DWORD)(pPalette), (DWORD)(pbDest), (DWORD)(dwDestSize), \
		(DWORD)(pdwWidth), (DWORD)(pdwHeight), (DWORD)(pdwBitDepth)))

#define StormSBmpSaveImage(pszName, pPalette, pbBuffer, nWidth, nHeight, dwBitDepth) \
	((BOOL)StormOrdinal324((DWORD)(pszName), (DWORD)(pPalette), (DWORD)(pbBuffer), (DWORD)(nWidth), \
		(DWORD)(nHeight), (DWORD)(dwBitDepth)))

#define StormSBmpAllocLoadImage(pszName, pPalette, ppbBuffer, pdwWidth, pdwHeight, pdwBitDepth, \
		dwRequestedDepth, pfnAlloc) \
	((BOOL)StormOrdinal325((DWORD)(pszName), (DWORD)(pPalette), (DWORD)(ppbBuffer), (DWORD)(pdwWidth), \
		(DWORD)(pdwHeight), (DWORD)(pdwBitDepth), (DWORD)(dwRequestedDepth), (DWORD)(pfnAlloc)))

#define StormSBmpSaveImageStride(pszName, pPalette, pbBuffer, nWidth, nHeight, dwBitDepth, dwStride) \
	((BOOL)StormOrdinal326((DWORD)(pszName), (DWORD)(pPalette), (DWORD)(pbBuffer), (DWORD)(nWidth), \
		(DWORD)(nHeight), (DWORD)(dwBitDepth), (DWORD)(dwStride)))

#define StormSCodeDelete(hCode) \
	((BOOL)StormOrdinal332((DWORD)(hCode)))

#define StormSCodeGetListing(pszCode, pszBuffer, dwBufferSize) \
	((BOOL)StormOrdinal335((DWORD)(pszCode), (DWORD)(pszBuffer), (DWORD)(dwBufferSize)))

#define StormSDrawInitialize(hInstance, pszClass, pszTitle, pfnWndProc, nScheme, nWidth, nHeight, nBitDepth) \
	((BOOL)StormOrdinal341((DWORD)(hInstance), (DWORD)(pszClass), (DWORD)(pszTitle), (DWORD)(pfnWndProc), \
		(DWORD)(nScheme), (DWORD)(nWidth), (DWORD)(nHeight), (DWORD)(nBitDepth)))

#define StormSDrawCaptureScreen(pszName) \
	((BOOL)StormOrdinal342((DWORD)(pszName)))

#define StormSDrawClearSurface(nSurface) \
	((BOOL)((DWORD (__stdcall *)(DWORD))StormOrdinal343)((DWORD)(nSurface)))

#define StormSDrawDestroy() \
	((BOOL)StormOrdinal344())

#define StormSDrawFlipPage() \
	((BOOL)StormOrdinal345())

#define StormSDrawGetFrameWindow(phWnd) \
	((HWND)StormOrdinal346((DWORD)(phWnd)))

#define StormSDrawGetObjects(pptDraw, pptPrimary, pptBack, pptSystem, pptAlternate, pptPalette, phPalette) \
	((BOOL)StormOrdinal347((DWORD)(pptDraw), (DWORD)(pptPrimary), (DWORD)(pptBack), (DWORD)(pptSystem), \
		(DWORD)(pptAlternate), (DWORD)(pptPalette), (DWORD)(phPalette)))

#define StormSDrawGetScreenSize(pnWidth, pnHeight, pnBitDepth) \
	((BOOL)StormOrdinal348((DWORD)(pnWidth), (DWORD)(pnHeight), (DWORD)(pnBitDepth)))

#define StormSDrawGetServiceLevel(pdwLevel) \
	((BOOL)StormOrdinal349((DWORD)(pdwLevel)))

#define StormSDrawLockSurface(nSurface, prc, ppbBits, pnPitch, dwFlags) \
	((BOOL)StormOrdinal350((DWORD)(nSurface), (DWORD)(prc), (DWORD)(ppbBits), (DWORD)(pnPitch), \
		(DWORD)(dwFlags)))

#define StormSDrawManualInitialize(hWnd, ptDraw, ptPrimary, ptBack, ptSystem, ptAlternate, ptPalette, \
		hPalette) \
	((BOOL)StormOrdinal351((DWORD)(hWnd), (DWORD)(ptDraw), (DWORD)(ptPrimary), (DWORD)(ptBack), \
		(DWORD)(ptSystem), (DWORD)(ptAlternate), (DWORD)(ptPalette), (DWORD)(hPalette)))

#define StormSDrawMessageBox(pszText, pszCaption, uType) \
	((int)StormOrdinal352((DWORD)(pszText), (DWORD)(pszCaption), (DWORD)(uType)))

#define StormSDrawPostClose() \
	((BOOL)StormOrdinal353())

#define StormSDrawRealizePalette() \
	((BOOL)StormOrdinal354())

#define StormSDrawSelectGdiSurface(fGdi, fCopy) \
	((BOOL)((DWORD (__stdcall *)(DWORD, DWORD))StormOrdinal355)((DWORD)(fGdi), (DWORD)(fCopy)))

#define StormSDrawUnlockSurface(nSurface, pbBits, dwRects, prcRects) \
	((BOOL)StormOrdinal356((DWORD)(nSurface), (DWORD)(pbBits), (DWORD)(dwRects), (DWORD)(prcRects)))

#define StormSDrawUpdatePalette(dwFirst, dwCount, ptEntries, fAll) \
	((BOOL)StormOrdinal357((DWORD)(dwFirst), (DWORD)(dwCount), (DWORD)(ptEntries), (DWORD)(fAll)))

#define StormSDrawUpdateScreen(prc) \
	((BOOL)StormOrdinal358((DWORD)(prc)))

#define StormSDrawWaitVerticalBlank() \
	((BOOL)((DWORD (__cdecl *)(void))StormOrdinal359)())

#define StormSEvtDispatch(dwType, dwSubtype, dwId, pData) \
	((BOOL)((DWORD (__stdcall *)(DWORD, DWORD, DWORD, void *))StormOrdinal372)((DWORD)(dwType), \
		(DWORD)(dwSubtype), (DWORD)(dwId), (void *)(pData)))

#define StormSEvtRegisterHandler(dwType, dwSubtype, dwId, dwFlags, pfnHandler) \
	((BOOL)StormOrdinal373((DWORD)(dwType), (DWORD)(dwSubtype), (DWORD)(dwId), (DWORD)(dwFlags), \
		(DWORD)(pfnHandler)))

#define StormSEvtUnregisterHandler(dwType, dwSubtype, dwId, pfnHandler) \
	((BOOL)StormOrdinal374((DWORD)(dwType), (DWORD)(dwSubtype), (DWORD)(dwId), (DWORD)(pfnHandler)))

#define StormSEvtUnregisterType(dwType, dwSubtype) \
	((BOOL)StormOrdinal375((DWORD)(dwType), (DWORD)(dwSubtype)))

#define StormSEvtPopState(dwType, dwSubtype) \
	((BOOL)StormOrdinal376((DWORD)(dwType), (DWORD)(dwSubtype)))

#define StormSEvtPushState(dwType, dwSubtype) \
	((BOOL)StormOrdinal377((DWORD)(dwType), (DWORD)(dwSubtype)))

#define StormSEvtBreakHandlerChain(pData) \
	((BOOL)StormOrdinal378((DWORD)(pData)))

#define StormSGdiBitBlt(pbDest, nX, nY, pbSource, prcSource, nSourceWidth, nSourceHeight, dwColor, dwRop) \
	((BOOL)StormOrdinal381((DWORD)(pbDest), (DWORD)(nX), (DWORD)(nY), (DWORD)(pbSource), (DWORD)(prcSource), \
		(DWORD)(nSourceWidth), (DWORD)(nSourceHeight), (DWORD)(dwColor), (DWORD)(dwRop)))

#define StormSGdiCreateFont(pbBits, nWidth, nHeight, nBitDepth, nCellWidth, nCellHeight, ptChars, pptFont) \
	((BOOL)StormOrdinal382((DWORD)(pbBits), (DWORD)(nWidth), (DWORD)(nHeight), (DWORD)(nBitDepth), \
		(DWORD)(nCellWidth), (DWORD)(nCellHeight), (DWORD)(ptChars), (DWORD)(pptFont)))

#define StormSGdiDeleteObject(ptObj) \
	((BOOL)StormOrdinal383((DWORD)(ptObj)))

#define StormSGdiExtTextOut(pbDest, nX, nY, prcClip, dwColor, nForeground, nBackground, pszText, nLength) \
	((BOOL)StormOrdinal385((DWORD)(pbDest), (DWORD)(nX), (DWORD)(nY), (DWORD)(prcClip), (DWORD)(dwColor), \
		(DWORD)(nForeground), (DWORD)(nBackground), (DWORD)(pszText), (DWORD)(nLength)))

#define StormSGdiImportFont(hFont, pptFont) \
	((BOOL)StormOrdinal386((DWORD)(hFont), (DWORD)(pptFont)))

#define StormSGdiLoadFont(pszName, nCellWidth, nCellHeight, nFixed, ptChars, pptFont) \
	((BOOL)StormOrdinal387((DWORD)(pszName), (DWORD)(nCellWidth), (DWORD)(nCellHeight), (DWORD)(nFixed), \
		(DWORD)(ptChars), (DWORD)(pptFont)))

#define StormSGdiRectangle(pbDest, nLeft, nTop, nRight, nBottom, dwColor) \
	((BOOL)StormOrdinal388((DWORD)(pbDest), (DWORD)(nLeft), (DWORD)(nTop), (DWORD)(nRight), \
		(DWORD)(nBottom), (DWORD)(dwColor)))

#define StormSGdiSelectObject(ptObj) \
	((BOOL)StormOrdinal389((DWORD)(ptObj)))

#define StormSGdiSetPitch(nPitch) \
	((BOOL)StormOrdinal390((DWORD)(nPitch)))

#define StormSGdiTextOut(pbDest, nX, nY, dwColor, pszText, nLength) \
	((BOOL)StormOrdinal391((DWORD)(pbDest), (DWORD)(nX), (DWORD)(nY), (DWORD)(dwColor), (DWORD)(pszText), \
		(DWORD)(nLength)))

#define StormSGdiGetTextExtent(pszText, nLength, ptSize) \
	((BOOL)StormOrdinal393((DWORD)(pszText), (DWORD)(nLength), (DWORD)(ptSize)))

#define StormSMemAlloc(dwBytes, pszFile, nLine, dwFlags) \
	((void *)StormOrdinal401((DWORD)(dwBytes), (DWORD)(pszFile), (DWORD)(nLine), (DWORD)(dwFlags)))

#define StormSMemFree(pMemory, pszFile, nLine, dwFlags) \
	((BOOL)StormOrdinal403((DWORD)(pMemory), (DWORD)(pszFile), (DWORD)(nLine), (DWORD)(dwFlags)))

#define StormSMemGetSize(pMemory, pszFile, nLine) \
	StormOrdinal404((DWORD)(pMemory), (DWORD)(pszFile), (DWORD)(nLine))

#define StormSMemReAlloc(pMemory, dwBytes, pszFile, nLine, dwFlags) \
	((void *)StormOrdinal405((DWORD)(pMemory), (DWORD)(dwBytes), (DWORD)(pszFile), (DWORD)(nLine), \
		(DWORD)(dwFlags)))

#define StormSMsgDispatchMessage(hWnd, uMsg, wParam, lParam, pfHandled, plResult) \
	((BOOL)StormOrdinal412((DWORD)(hWnd), (DWORD)(uMsg), (DWORD)(wParam), (DWORD)(lParam), \
		(DWORD)(pfHandled), (DWORD)(plResult)))

#define StormSMsgRunMessageLoop(pfnIdle, fCleanup) \
	((int)StormOrdinal413((DWORD)(pfnIdle), (DWORD)(fCleanup)))

#define StormSMsgRegisterCommand(hWnd, dwId, pfnHandler) \
	((BOOL)StormOrdinal414((DWORD)(hWnd), (DWORD)(dwId), (DWORD)(pfnHandler)))

#define StormSMsgRegisterKeyDown(hWnd, dwId, pfnHandler) \
	((BOOL)StormOrdinal415((DWORD)(hWnd), (DWORD)(dwId), (DWORD)(pfnHandler)))

#define StormSMsgRegisterKeyUp(hWnd, dwId, pfnHandler) \
	((BOOL)StormOrdinal416((DWORD)(hWnd), (DWORD)(dwId), (DWORD)(pfnHandler)))

#define StormSMsgRegisterMessage(hWnd, dwId, pfnHandler) \
	((BOOL)StormOrdinal417((DWORD)(hWnd), (DWORD)(dwId), (DWORD)(pfnHandler)))

#define StormSMsgPopRegisterState(hWnd) \
	((BOOL)StormOrdinal418((DWORD)(hWnd)))

#define StormSMsgPushRegisterState(hWnd) \
	((BOOL)StormOrdinal419((DWORD)(hWnd)))

#define StormSRegLoadData(pszKey, pszValue, dwFlags, pData, dwBufferSize, pdwDataSize) \
	((BOOL)StormOrdinal421((DWORD)(pszKey), (DWORD)(pszValue), (DWORD)(dwFlags), (DWORD)(pData), \
		(DWORD)(dwBufferSize), (DWORD)(pdwDataSize)))

#define StormSRegLoadString(pszKey, pszValue, dwFlags, pszBuffer, dwBufferChars) \
	((BOOL)StormOrdinal422((DWORD)(pszKey), (DWORD)(pszValue), (DWORD)(dwFlags), (DWORD)(pszBuffer), \
		(DWORD)(dwBufferChars)))

#define StormSRegLoadValue(pszKey, pszValue, dwFlags, pdwValue) \
	((BOOL)StormOrdinal423((DWORD)(pszKey), (DWORD)(pszValue), (DWORD)(dwFlags), (DWORD)(pdwValue)))

#define StormSRegSaveData(pszKey, pszValue, dwFlags, pData, dwDataSize) \
	((BOOL)StormOrdinal424((DWORD)(pszKey), (DWORD)(pszValue), (DWORD)(dwFlags), (DWORD)(pData), \
		(DWORD)(dwDataSize)))

#define StormSRegSaveString(pszKey, pszValue, dwFlags, pszString) \
	((BOOL)StormOrdinal425((DWORD)(pszKey), (DWORD)(pszValue), (DWORD)(dwFlags), (DWORD)(pszString)))

#define StormSRegSaveValue(pszKey, pszValue, dwFlags, dwValue) \
	((BOOL)StormOrdinal426((DWORD)(pszKey), (DWORD)(pszValue), (DWORD)(dwFlags), (DWORD)(dwValue)))

#define StormSRegGetBaseKey(dwFlags, pszBuffer, dwBufferChars) \
	((BOOL)StormOrdinal427((DWORD)(dwFlags), (DWORD)(pszBuffer), (DWORD)(dwBufferChars)))

#define StormSRegDeleteValue(pszKey, pszValue, dwFlags) \
	((BOOL)StormOrdinal428((DWORD)(pszKey), (DWORD)(pszValue), (DWORD)(dwFlags)))

#define StormSTransBlt(pbSurface, nX, nY, nPitch, ptTrans) \
	((BOOL)StormOrdinal431((DWORD)(pbSurface), (DWORD)(nX), (DWORD)(nY), (DWORD)(nPitch), (DWORD)(ptTrans)))

#define StormSTransBltUsingRuns(pbDest, pbSource, nDestPitch, nSourcePitch, ptTrans) \
	((BOOL)StormOrdinal432((DWORD)(pbDest), (DWORD)(pbSource), (DWORD)(nDestPitch), (DWORD)(nSourcePitch), \
		(DWORD)(ptTrans)))

#define StormSTransCreateInclusive(pbBits, nWidth, nHeight, dwBitDepth, prcArea, dwColorKey, pptTrans) \
	((BOOL)StormOrdinal433((DWORD)(pbBits), (DWORD)(nWidth), (DWORD)(nHeight), (DWORD)(dwBitDepth), \
		(DWORD)(prcArea), (DWORD)(dwColorKey), (DWORD)(pptTrans)))

#define StormSTransDelete(ptTrans) \
	((BOOL)StormOrdinal434((DWORD)(ptTrans)))

#define StormSTransDuplicate(ptSource, pptTrans) \
	((BOOL)StormOrdinal436((DWORD)(ptSource), (DWORD)(pptTrans)))

#define StormSTransIntersectDirtyArray(ptTrans, pbDirty, bMask, pptTrans) \
	((BOOL)StormOrdinal437((DWORD)(ptTrans), (DWORD)(pbDirty), (DWORD)(bMask), (DWORD)(pptTrans)))

#define StormSTransInvertMask(ptSource, pptTrans) \
	((BOOL)StormOrdinal438((DWORD)(ptSource), (DWORD)(pptTrans)))

#define StormSTransLoadInclusive(pszName, prcArea, dwColorKey, pptTrans) \
	((BOOL)StormOrdinal439((DWORD)(pszName), (DWORD)(prcArea), (DWORD)(dwColorKey), (DWORD)(pptTrans)))

#define StormSTransSetDirtyArrayInfo(nScreenWidth, nScreenHeight, nCellWidth, nCellHeight) \
	((BOOL)StormOrdinal440((DWORD)(nScreenWidth), (DWORD)(nScreenHeight), (DWORD)(nCellWidth), \
		(DWORD)(nCellHeight)))

#define StormSTransUpdateDirtyArray(pbDirty, bMask, nX, nY, ptTrans, dwReserved) \
	((BOOL)StormOrdinal441((DWORD)(pbDirty), (DWORD)(bMask), (DWORD)(nX), (DWORD)(nY), (DWORD)(ptTrans), \
		(DWORD)(dwReserved)))

#define StormSTransIsPixelInMask(ptTrans, nX, nY) \
	((BOOL)StormOrdinal442((DWORD)(ptTrans), (DWORD)(nX), (DWORD)(nY)))

#define StormSTransCombineMasks(ptTrans1, ptTrans2, nX, nY, dwFlags, pptTrans) \
	((BOOL)StormOrdinal443((DWORD)(ptTrans1), (DWORD)(ptTrans2), (DWORD)(nX), (DWORD)(nY), (DWORD)(dwFlags), \
		(DWORD)(pptTrans)))

#define StormSTransCreateMaskInclusive(pbBits, nWidth, nHeight, dwBitDepth, prcArea, dwColorKey, pptTrans) \
	((BOOL)StormOrdinal444((DWORD)(pbBits), (DWORD)(nWidth), (DWORD)(nHeight), (DWORD)(dwBitDepth), \
		(DWORD)(prcArea), (DWORD)(dwColorKey), (DWORD)(pptTrans)))

#define StormSTransCreate(pbBits, nWidth, nHeight, dwBitDepth, prcArea, dwColorKey, pptTrans) \
	((BOOL)StormOrdinal445((DWORD)(pbBits), (DWORD)(nWidth), (DWORD)(nHeight), (DWORD)(dwBitDepth), \
		(DWORD)(prcArea), (DWORD)(dwColorKey), (DWORD)(pptTrans)))

#define StormSTransCreateMask(pbBits, nWidth, nHeight, dwBitDepth, prcArea, dwColorKey, pptTrans) \
	((BOOL)StormOrdinal446((DWORD)(pbBits), (DWORD)(nWidth), (DWORD)(nHeight), (DWORD)(dwBitDepth), \
		(DWORD)(prcArea), (DWORD)(dwColorKey), (DWORD)(pptTrans)))

#define StormSTransLoad(pszName, prcArea, dwColorKey, pptTrans) \
	((BOOL)StormOrdinal447((DWORD)(pszName), (DWORD)(prcArea), (DWORD)(dwColorKey), (DWORD)(pptTrans)))

#define StormSVidDestroy() \
	((BOOL)StormOrdinal451())

#define StormSVidGetSize(ptVideo, pdwWidth, pdwHeight, pdwBitDepth) \
	((BOOL)StormOrdinal452((DWORD)(ptVideo), (DWORD)(pdwWidth), (DWORD)(pdwHeight), (DWORD)(pdwBitDepth)))

#define StormSVidInitialize(pDirectSound) \
	((BOOL)StormOrdinal453((DWORD)(pDirectSound)))

#define StormSVidPlayBegin(pszName, pvBuffer, prc, ptSize, ptPal, dwFlags, phVideo) \
	((BOOL)StormOrdinal454((DWORD)(pszName), (DWORD)(pvBuffer), (DWORD)(prc), (DWORD)(ptSize), \
		(DWORD)(ptPal), (DWORD)(dwFlags), (DWORD)(phVideo)))

#define StormSVidPlayBeginFromMemory(pbData, dwSize, pvBuffer, prc, ptSize, ptPal, dwFlags, phVideo) \
	((BOOL)StormOrdinal455((DWORD)(pbData), (DWORD)(dwSize), (DWORD)(pvBuffer), (DWORD)(prc), \
		(DWORD)(ptSize), (DWORD)(ptPal), (DWORD)(dwFlags), (DWORD)(phVideo)))

#define StormSVidPlayContinueSingle(ptVideo, fForce, pfEnd) \
	((BOOL)StormOrdinal457((DWORD)(ptVideo), (DWORD)(fForce), (DWORD)(pfEnd)))

#define StormSVidPlayEnd(ptVideo) \
	((BOOL)StormOrdinal458((DWORD)(ptVideo)))

#define StormSVidSetVolume(ptVideo, nVolume, nPan, dwTrack) \
	((BOOL)StormOrdinal459((DWORD)(ptVideo), (DWORD)(nVolume), (DWORD)(nPan), (DWORD)(dwTrack)))

#define StormSErrDisplayError(dwErrCode, pszName, nLine, pszText, fAllowContinue, dwExitCode) \
	((BOOL)StormOrdinal461((DWORD)(dwErrCode), (DWORD)(pszName), (DWORD)(nLine), (DWORD)(pszText), \
		(DWORD)(fAllowContinue), (DWORD)(dwExitCode)))

#define StormSErrGetErrorStr(dwErrCode, pszBuffer, dwBufferChars) \
	((BOOL)StormOrdinal462((DWORD)(dwErrCode), (DWORD)(pszBuffer), (DWORD)(dwBufferChars)))

#define StormSErrGetLastError() \
	((DWORD (__stdcall *)(void))StormOrdinal463)()

#define StormSErrAddMessageSource(wFacility, hModule, dwUnused) \
	((BOOL)StormOrdinal464((DWORD)(wFacility), (DWORD)(hModule), (DWORD)(dwUnused)))

#define StormSErrSetLastError(dwError) \
	StormOrdinal465((DWORD)(dwError))

#define StormSErrReportUnreleasedHandle(pszName) \
	StormOrdinal467((DWORD)(pszName))

#define StormSErrSetFatalGuard(fGuard) \
	StormOrdinal468((DWORD)(fGuard))

#define StormSErrRegisterHandler(pfnHandler) \
	StormOrdinal469((DWORD)(pfnHandler))

#define StormSErrUnregisterHandler(pfnHandler) \
	((DWORD (__stdcall *)(DWORD))StormOrdinal470)((DWORD)(pfnHandler))

#define StormSCmdGetBool(dwId) \
	((BOOL)StormOrdinal472((DWORD)(dwId)))

#define StormSCmdGetNum(dwId) \
	((DWORD (__stdcall *)(DWORD))StormOrdinal473)((DWORD)(dwId))

#define StormSCmdProcess(pszCommandLine, fSkipFirst, pfnExtra, pfnError) \
	((BOOL)StormOrdinal475((DWORD)(pszCommandLine), (DWORD)(fSkipFirst), (DWORD)(pfnExtra), \
		(DWORD)(pfnError)))

#define StormSCmdRegisterArgList(ptArgs, dwCount) \
	((BOOL)StormOrdinal476((DWORD)(ptArgs), (DWORD)(dwCount)))

#define StormSCmdRegisterArgument(dwFlags, dwId, pszName, pVariable, dwVariableSize, dwBits, dwMask, \
		pfnCallback) \
	((BOOL)StormOrdinal477((DWORD)(dwFlags), (DWORD)(dwId), (DWORD)(pszName), (DWORD)(pVariable), \
		(DWORD)(dwVariableSize), (DWORD)(dwBits), (DWORD)(dwMask), (DWORD)(pfnCallback)))

#define StormSCmdIsArgumentSet(dwId) \
	((BOOL)((DWORD (__stdcall *)(DWORD))StormOrdinal478)((DWORD)(dwId)))

#define StormSCmdProcessCommandLine(pfnExtra, pfnError) \
	((BOOL)StormOrdinal479((DWORD)(pfnExtra), (DWORD)(pfnError)))

#define StormSMemFindNextBlock(dwHeap, pPrev, ppNext, ptInfo) \
	((BOOL)StormOrdinal481((DWORD)(dwHeap), (DWORD)(pPrev), (DWORD)(ppNext), (DWORD)(ptInfo)))

#define StormSMemFindNextHeap(dwPrev, pdwNext, ptInfo) \
	((BOOL)StormOrdinal482((DWORD)(dwPrev), (DWORD)(pdwNext), (DWORD)(ptInfo)))

#define StormSMemGetHeapByCaller(pszFile, nLine) \
	StormOrdinal483((DWORD)(pszFile), (DWORD)(nLine))

#define StormSMemGetHeapByPtr(pMemory) \
	StormOrdinal484((DWORD)(pMemory))

#define StormSMemHeapAlloc(dwHeap, dwFlags, dwBytes) \
	((void *)StormOrdinal485((DWORD)(dwHeap), (DWORD)(dwFlags), (DWORD)(dwBytes)))

#define StormSMemHeapCreate(dwOptions, dwInitialSize, dwMaximumSize) \
	StormOrdinal486((DWORD)(dwOptions), (DWORD)(dwInitialSize), (DWORD)(dwMaximumSize))

#define StormSMemHeapDestroy(dwHeap) \
	((BOOL)StormOrdinal487((DWORD)(dwHeap)))

#define StormSMemHeapFree(dwHeap, dwFlags, pMemory) \
	((BOOL)StormOrdinal488((DWORD)(dwHeap), (DWORD)(dwFlags), (DWORD)(pMemory)))

#define StormSMemHeapReAlloc(dwHeap, dwFlags, pMemory, dwBytes) \
	((void *)StormOrdinal489((DWORD)(dwHeap), (DWORD)(dwFlags), (DWORD)(pMemory), (DWORD)(dwBytes)))

#define StormSMemHeapSize(dwHeap, dwFlags, pMemory) \
	((DWORD (__stdcall *)(DWORD, DWORD, void *))StormOrdinal490)((DWORD)(dwHeap), (DWORD)(dwFlags), \
		(void *)(pMemory))

#define StormSMemCopy(pDest, pSource, dwBytes) \
	StormOrdinal491((DWORD)(pDest), (DWORD)(pSource), (DWORD)(dwBytes))

#define StormSMemFill(pDest, dwBytes, bValue) \
	StormOrdinal492((DWORD)(pDest), (DWORD)(dwBytes), (DWORD)(bValue))

#define StormSMemMove(pDest, pSource, dwBytes) \
	((void *)StormOrdinal493((DWORD)(pDest), (DWORD)(pSource), (DWORD)(dwBytes)))

#define StormSMemZero(pDest, dwBytes) \
	StormOrdinal494((DWORD)(pDest), (DWORD)(dwBytes))

#define StormSMemCmp(p1, p2, dwBytes) \
	((int)StormOrdinal495((DWORD)(p1), (DWORD)(p2), (DWORD)(dwBytes)))

#define StormSMemSetDebugFlags(dwFlags, bMask) \
	StormOrdinal496((DWORD)(dwFlags), (DWORD)(bMask))

#define StormSMemDumpState(pfnCallback, pParam) \
	((BOOL)StormOrdinal497((DWORD)(pfnCallback), (DWORD)(pParam)))

#define StormSStrCopy(pszDest, pszSource, dwMaxLength) \
	StormOrdinal501((DWORD)(pszDest), (DWORD)(pszSource), (DWORD)(dwMaxLength))

#define StormSStrHash(pszString, dwFlags, dwSeed) \
	StormOrdinal502((DWORD)(pszString), (DWORD)(dwFlags), (DWORD)(dwSeed))

#define StormSStrAppend(pszDest, pszSource, dwMaxLength) \
	StormOrdinal503((DWORD)(pszDest), (DWORD)(pszSource), (DWORD)(dwMaxLength))

#define StormSStrTokenize(ppszString, pszBuffer, dwBufferChars, pszWhitespace, pfQuoted) \
	StormOrdinal504((DWORD)(ppszString), (DWORD)(pszBuffer), (DWORD)(dwBufferChars), (DWORD)(pszWhitespace), \
		(DWORD)(pfQuoted))

#define StormSStrChrAnyDirection(pszString, chFind, fReverse) \
	((char *)StormOrdinal505((DWORD)(pszString), (DWORD)(chFind), (DWORD)(fReverse)))

#define StormSStrLen(pszString) \
	StormOrdinal506((DWORD)(pszString))

#define StormSStrDup(pszString) \
	((char *)StormOrdinal507((DWORD)(pszString)))

#define StormSStrCmp(pszString1, pszString2, dwMaxLength) \
	((int)StormOrdinal508((DWORD)(pszString1), (DWORD)(pszString2), (DWORD)(dwMaxLength)))

#define StormSStrCmpI(pszString1, pszString2, dwMaxLength) \
	((int)StormOrdinal509((DWORD)(pszString1), (DWORD)(pszString2), (DWORD)(dwMaxLength)))

#define StormSMsgBreakHandlerChain(ptParams) \
	((BOOL)StormOrdinal511((DWORD)(ptParams)))

#define StormSMsgUnregisterCommand(hWnd, dwId, pfnHandler) \
	((BOOL)StormOrdinal512((DWORD)(hWnd), (DWORD)(dwId), (DWORD)(pfnHandler)))

#define StormSMsgUnregisterKeyDown(hWnd, dwId, pfnHandler) \
	((BOOL)StormOrdinal513((DWORD)(hWnd), (DWORD)(dwId), (DWORD)(pfnHandler)))

#define StormSMsgUnregisterKeyUp(hWnd, dwId, pfnHandler) \
	((BOOL)StormOrdinal514((DWORD)(hWnd), (DWORD)(dwId), (DWORD)(pfnHandler)))

#define StormSMsgUnregisterMessage(hWnd, dwId, pfnHandler) \
	((BOOL)StormOrdinal515((DWORD)(hWnd), (DWORD)(dwId), (DWORD)(pfnHandler)))

#define StormSMsgGetDefWindowProc(dwReserved) \
	StormOrdinal516((DWORD)(dwReserved))

#define StormSDrawSetWindow(hWnd) \
	((BOOL)StormOrdinal517((DWORD)(hWnd)))

#define StormSDrawGetWindow() \
	((HWND)((DWORD (__stdcall *)(void))StormOrdinal518)())

#define StormSRgnClear(hRgn) \
	StormOrdinal521((DWORD)(hRgn))

#define StormSRgnCombineRect(hRgn, prc, dwParam, nMode) \
	StormOrdinal523((DWORD)(hRgn), (DWORD)(prc), (DWORD)(dwParam), (DWORD)(nMode))

#define StormSRgnCreate(phRgn, dwReserved) \
	StormOrdinal524((DWORD)(phRgn), (DWORD)(dwReserved))

#define StormSRgnDelete(hRgn) \
	StormOrdinal525((DWORD)(hRgn))

#define StormSRgnDuplicate(hRgn, phNew, dwReserved) \
	StormOrdinal527((DWORD)(hRgn), (DWORD)(phNew), (DWORD)(dwReserved))

#define StormSRgnGetRectParams(hRgn, prc, pdwCount, pdwParams) \
	StormOrdinal528((DWORD)(hRgn), (DWORD)(prc), (DWORD)(pdwCount), (DWORD)(pdwParams))

#define StormSRgnGetRects(hRgn, pdwCount, prcRects) \
	StormOrdinal529((DWORD)(hRgn), (DWORD)(pdwCount), (DWORD)(prcRects))

#define StormSRgnGetBoundingRect(hRgn, prc) \
	StormOrdinal530((DWORD)(hRgn), (DWORD)(prc))

#define StormSRgnIsPointInRegion(hRgn, nX, nY) \
	((BOOL)StormOrdinal531((DWORD)(hRgn), (DWORD)(nX), (DWORD)(nY)))

#define StormSRgnIsRectInRegion(hRgn, prc) \
	((BOOL)StormOrdinal532((DWORD)(hRgn), (DWORD)(prc)))

#define StormSRgnOffset(hRgn, nX, nY) \
	StormOrdinal533((DWORD)(hRgn), (DWORD)(nX), (DWORD)(nY))

#define StormSRgnGetRectParamsf(hRgn, prc, pdwCount, pdwParams) \
	StormOrdinal535((DWORD)(hRgn), (DWORD)(prc), (DWORD)(pdwCount), (DWORD)(pdwParams))

#define StormSRgnGetRectsf(hRgn, pdwCount, prcRects) \
	StormOrdinal536((DWORD)(hRgn), (DWORD)(pdwCount), (DWORD)(prcRects))

#define StormSRgnGetBoundingRectf(hRgn, prc) \
	StormOrdinal537((DWORD)(hRgn), (DWORD)(prc))

#define StormSRgnIsRectInRegionf(hRgn, prc) \
	((BOOL)StormOrdinal539((DWORD)(hRgn), (DWORD)(prc)))

#define StormSLogClose(dwLog) \
	StormOrdinal541((DWORD)(dwLog))

#define StormSLogCreate(pszFile, dwUnused, pdwLog) \
	((BOOL)StormOrdinal542((DWORD)(pszFile), (DWORD)(dwUnused), (DWORD)(pdwLog)))

#define StormSLogSetTimeStamps(dwLog, fTime) \
	StormOrdinal543((DWORD)(dwLog), (DWORD)(fTime))

#define StormSLogDump(dwLog, pbBuffer, dwBytes) \
	StormOrdinal544((DWORD)(dwLog), (DWORD)(pbBuffer), (DWORD)(dwBytes))

#define StormSLogFlush(dwLog) \
	StormOrdinal545((DWORD)(dwLog))

#define StormSLogFlushAll() \
	((DWORD (__stdcall *)(void))StormOrdinal546)()

#define StormSLogPend \
	StormOrdinal547

#define StormSLogSetIndent(dwLog, nIndent) \
	((int)StormOrdinal549((DWORD)(dwLog), (DWORD)(nIndent)))

#define StormSLogAddIndent(dwLog, nDelta) \
	((int)StormOrdinal550((DWORD)(dwLog), (DWORD)(nDelta)))

#define StormSCompCompress(pbDest, pdwDestSize, pbSource, dwSize, dwMethods, nType, nLevel) \
	((BOOL)StormOrdinal551((DWORD)(pbDest), (DWORD)(pdwDestSize), (DWORD)(pbSource), (DWORD)(dwSize), \
		(DWORD)(dwMethods), (DWORD)(nType), (DWORD)(nLevel)))

#define StormSCompDecompress(pbDest, pdwDestSize, pbSource, dwSize) \
	((BOOL)StormOrdinal552((DWORD)(pbDest), (DWORD)(pdwDestSize), (DWORD)(pbSource), (DWORD)(dwSize)))

#define StormSErrDisplayErrorFmt \
	StormOrdinal562

#define StormSErrIsDisplayingError() \
	((BOOL)((DWORD (__stdcall *)(void))StormOrdinal563)())

#define StormSErrSetAssertLocation(pszFile, nLine) \
	StormOrdinal564((DWORD)(pszFile), (DWORD)(nLine))

#define StormSStrChr(pszString, chFind) \
	((char *)StormOrdinal571((DWORD)(pszString), (DWORD)(chFind)))

#define StormSStrChrR(pszString, chFind) \
	((char *)StormOrdinal572((DWORD)(pszString), (DWORD)(chFind)))

#define StormSStrToInt(pszString) \
	StormOrdinal575((DWORD)(pszString))

#define StormSStrToUnsigned(pszString) \
	StormOrdinal576((DWORD)(pszString))

#define StormSStrPrintf \
	StormOrdinal578

#define StormSBigAdd(ptDest, ptA, ptB) \
	StormOrdinal601((DWORD)(ptDest), (DWORD)(ptA), (DWORD)(ptB))

#define StormSBigAnd(ptDest, ptA, ptB) \
	StormOrdinal602((DWORD)(ptDest), (DWORD)(ptA), (DWORD)(ptB))

#define StormSBigCompare(ptA, ptB) \
	((int)StormOrdinal603((DWORD)(ptA), (DWORD)(ptB)))

#define StormSBigCopy(ptDest, ptSource) \
	StormOrdinal604((DWORD)(ptDest), (DWORD)(ptSource))

#define StormSBigDec(ptDest, ptSource) \
	StormOrdinal605((DWORD)(ptDest), (DWORD)(ptSource))

#define StormSBigDel(ptBig) \
	StormOrdinal606((DWORD)(ptBig))

#define StormSBigDiv(ptQuot, ptA, ptB) \
	StormOrdinal607((DWORD)(ptQuot), (DWORD)(ptA), (DWORD)(ptB))

#define StormSBigFindPrime(ptDest, dwBits, ptSeed, ptExponent) \
	StormOrdinal608((DWORD)(ptDest), (DWORD)(dwBits), (DWORD)(ptSeed), (DWORD)(ptExponent))

#define StormSBigFromBinary(ptBig, pbData, dwBytes) \
	StormOrdinal609((DWORD)(ptBig), (DWORD)(pbData), (DWORD)(dwBytes))

#define StormSBigFromStr(ptBig, pszString) \
	StormOrdinal610((DWORD)(ptBig), (DWORD)(pszString))

#define StormSBigFromStuffed(ptBig, pbData, dwBytes, pdwUsed) \
	StormOrdinal611((DWORD)(ptBig), (DWORD)(pbData), (DWORD)(dwBytes), (DWORD)(pdwUsed))

#define StormSBigFromUnsigned(ptBig, dwValue) \
	StormOrdinal612((DWORD)(ptBig), (DWORD)(dwValue))

#define StormSBigGcd(ptDest, ptA, ptB) \
	StormOrdinal613((DWORD)(ptDest), (DWORD)(ptA), (DWORD)(ptB))

#define StormSBigInc(ptDest, ptSource) \
	StormOrdinal614((DWORD)(ptDest), (DWORD)(ptSource))

#define StormSBigInvMod(ptDest, ptA, ptMod) \
	StormOrdinal615((DWORD)(ptDest), (DWORD)(ptA), (DWORD)(ptMod))

#define StormSBigIsEven(ptBig) \
	((BOOL)StormOrdinal616((DWORD)(ptBig)))

#define StormSBigIsOdd(ptBig) \
	((BOOL)StormOrdinal617((DWORD)(ptBig)))

#define StormSBigIsOne(ptBig) \
	((BOOL)StormOrdinal618((DWORD)(ptBig)))

#define StormSBigIsPrime(ptBig) \
	((BOOL)StormOrdinal619((DWORD)(ptBig)))

#define StormSBigIsZero(ptBig) \
	((BOOL)StormOrdinal620((DWORD)(ptBig)))

#define StormSBigMod(ptRem, ptA, ptB) \
	StormOrdinal621((DWORD)(ptRem), (DWORD)(ptA), (DWORD)(ptB))

#define StormSBigMul(ptDest, ptA, ptB) \
	StormOrdinal622((DWORD)(ptDest), (DWORD)(ptA), (DWORD)(ptB))

#define StormSBigMulMod(ptDest, ptA, ptB, ptMod) \
	StormOrdinal623((DWORD)(ptDest), (DWORD)(ptA), (DWORD)(ptB), (DWORD)(ptMod))

#define StormSBigNew(pptBig) \
	StormOrdinal624((DWORD)(pptBig))

#define StormSBigNot(ptDest, ptSource) \
	StormOrdinal625((DWORD)(ptDest), (DWORD)(ptSource))

#define StormSBigOr(ptDest, ptA, ptB) \
	StormOrdinal626((DWORD)(ptDest), (DWORD)(ptA), (DWORD)(ptB))

#define StormSBigPow(ptDest, ptBase, dwExponent) \
	StormOrdinal627((DWORD)(ptDest), (DWORD)(ptBase), (DWORD)(dwExponent))

#define StormSBigPowMod(ptDest, ptBase, ptExponent, ptMod) \
	StormOrdinal628((DWORD)(ptDest), (DWORD)(ptBase), (DWORD)(ptExponent), (DWORD)(ptMod))

#define StormSBigRand(ptDest, ptMod, ptState) \
	StormOrdinal629((DWORD)(ptDest), (DWORD)(ptMod), (DWORD)(ptState))

#define StormSBigSetPowerOfTwo(ptBig, dwBit) \
	StormOrdinal630((DWORD)(ptBig), (DWORD)(dwBit))

#define StormSBigSetOne(ptBig) \
	StormOrdinal631((DWORD)(ptBig))

#define StormSBigSetZero(ptBig) \
	StormOrdinal632((DWORD)(ptBig))

#define StormSBigShl(ptDest, ptSource, dwBits) \
	StormOrdinal633((DWORD)(ptDest), (DWORD)(ptSource), (DWORD)(dwBits))

#define StormSBigShr(ptDest, ptSource, dwBits) \
	StormOrdinal634((DWORD)(ptDest), (DWORD)(ptSource), (DWORD)(dwBits))

#define StormSBigSquare(ptDest, ptA) \
	StormOrdinal635((DWORD)(ptDest), (DWORD)(ptA))

#define StormSBigSubtract(ptDest, ptA, ptB) \
	StormOrdinal636((DWORD)(ptDest), (DWORD)(ptA), (DWORD)(ptB))

#define StormSBigToBinaryArray(ptBig, ptOut, fAppend) \
	StormOrdinal637((DWORD)(ptBig), (DWORD)(ptOut), (DWORD)(fAppend))

#define StormSBigToBinaryBuffer(ptBig, pbOut, dwMax, pdwBytes) \
	StormOrdinal638((DWORD)(ptBig), (DWORD)(pbOut), (DWORD)(dwMax), (DWORD)(pdwBytes))

#define StormSBigToBinaryPtr(ptBig, ppbData, pdwBytes) \
	StormOrdinal639((DWORD)(ptBig), (DWORD)(ppbData), (DWORD)(pdwBytes))

#define StormSBigToStrArray(ptBig, ptOut, fAppend) \
	StormOrdinal640((DWORD)(ptBig), (DWORD)(ptOut), (DWORD)(fAppend))

#define StormSBigToStrBuffer(ptBig, pszOut, dwMax) \
	StormOrdinal641((DWORD)(ptBig), (DWORD)(pszOut), (DWORD)(dwMax))

#define StormSBigToStrPtr(ptBig, ppszOut) \
	StormOrdinal642((DWORD)(ptBig), (DWORD)(ppszOut))

#define StormSBigToStuffedArray(ptBig, ptOut, fAppend) \
	StormOrdinal643((DWORD)(ptBig), (DWORD)(ptOut), (DWORD)(fAppend))

#define StormSBigToStuffedBuffer(ptBig, pbOut, dwMax, pdwBytes) \
	StormOrdinal644((DWORD)(ptBig), (DWORD)(pbOut), (DWORD)(dwMax), (DWORD)(pdwBytes))

#define StormSBigToStuffedPtr(ptBig, ppbData, pdwBytes) \
	StormOrdinal645((DWORD)(ptBig), (DWORD)(ppbData), (DWORD)(pdwBytes))

#define StormSBigToUnsigned(ptBig, pdwValue) \
	StormOrdinal646((DWORD)(ptBig), (DWORD)(pdwValue))

#define StormSBigXor(ptDest, ptA, ptB) \
	StormOrdinal647((DWORD)(ptDest), (DWORD)(ptA), (DWORD)(ptB))

#define StormSNetCreateGameRaw StormOrdinal101
#define StormSNetDestroyRaw StormOrdinal102
#define StormSNetEnumDevicesRaw StormOrdinal103
#define StormSNetEnumGamesRaw StormOrdinal104
#define StormSNetEnumProvidersRaw StormOrdinal105
#define StormSNetDropPlayerRaw StormOrdinal106
#define StormSNetGetGameInfoRaw StormOrdinal107
#define StormSNetGetNetworkLatencyRaw StormOrdinal108
#define StormSNetGetNumPlayersRaw StormOrdinal109
#define StormSNetGetOwnerTurnsWaitingRaw StormOrdinal110
#define StormSNetGetPerformanceDataRaw StormOrdinal111
#define StormSNetGetPlayerCapsRaw StormOrdinal112
#define StormSNetGetPlayerNameRaw StormOrdinal113
#define StormSNetGetProviderCapsRaw StormOrdinal114
#define StormSNetGetTurnsInTransitRaw StormOrdinal115
#define StormSNetInitializeDeviceRaw StormOrdinal116
#define StormSNetInitializeProviderRaw StormOrdinal117
#define StormSNetJoinGameRaw StormOrdinal118
#define StormSNetLeaveGameRaw StormOrdinal119
#define StormSNetPerformUpgradeRaw StormOrdinal120
#define StormSNetReceiveMessageRaw StormOrdinal121
#define StormSNetReceiveTurnsRaw StormOrdinal122
#define StormSNetRegisterEventHandlerRaw StormOrdinal123
#define StormSNetResetLatencyMeasurementsRaw StormOrdinal124
#define StormSNetSelectGameRaw StormOrdinal125
#define StormSNetSelectProviderRaw StormOrdinal126
#define StormSNetSendMessageRaw StormOrdinal127
#define StormSNetSendTurnRaw StormOrdinal128
#define StormSNetSetBasePlayerRaw StormOrdinal129
#define StormSNetSetGameModeRaw StormOrdinal130
#define StormSNetUnregisterEventHandlerRaw StormOrdinal131
#define StormSNetEnumGamesExRaw StormOrdinal133
#define StormSNetSetServerChatCommandRaw StormOrdinal134
#define StormSNetSendDatagramRaw StormOrdinal135
#define StormSNetReceiveDatagramRaw StormOrdinal136
#define StormSNetDisconnectAllRaw StormOrdinal137
#define StormSNetCreateLadderGameRaw StormOrdinal138
#define StormSNetReportGameResultRaw StormOrdinal139
#define StormSNetCheckDataFileRaw StormOrdinal140
#define StormSDlgBeginPaintRaw StormOrdinal201
#define StormSDlgBltToWindowInclusiveRaw StormOrdinal202
#define StormSDlgCheckTimersRaw StormOrdinal203
#define StormSDlgCreateDialogIndirectParamRaw StormOrdinal204
#define StormSDlgCreateDialogParamRaw StormOrdinal205
#define StormSDlgDefDialogProcRaw StormOrdinal206
#define StormSDlgDialogBoxIndirectParamRaw StormOrdinal208
#define StormSDlgDialogBoxParamRaw StormOrdinal209
#define StormSDlgDrawBitmapRaw StormOrdinal210
#define StormSDlgEndDialogRaw StormOrdinal211
#define StormSDlgEndPaintRaw StormOrdinal212
#define StormSDlgKillTimerRaw StormOrdinal213
#define StormSDlgSetBaseFontRaw StormOrdinal214
#define StormSDlgSetBitmapInclusiveRaw StormOrdinal215
#define StormSDlgSetControlBitmapsRaw StormOrdinal216
#define StormSDlgSetSystemCursorRaw StormOrdinal217
#define StormSDlgSetCursorRaw StormOrdinal218
#define StormSDlgSetTimerRaw StormOrdinal219
#define StormSDlgUpdateCursorRaw StormOrdinal220
#define StormSDlgBltToWindowRaw StormOrdinal221
#define StormSDlgSetBitmapRaw StormOrdinal222
#define StormSDlgSetLocaleRaw StormOrdinal223
#define StormSFileAuthenticateArchiveRaw StormOrdinal251
#define StormSFileCloseArchiveRaw StormOrdinal252
#define StormSFileCloseFileRaw StormOrdinal253
#define StormSFileDdaBeginRaw StormOrdinal254
#define StormSFileDdaBeginAtRaw StormOrdinal255
#define StormSFileDdaDestroyRaw StormOrdinal256
#define StormSFileDdaEndRaw StormOrdinal257
#define StormSFileDdaGetPosRaw StormOrdinal258
#define StormSFileDdaGetVolumeRaw StormOrdinal259
#define StormSFileDdaInitializeRaw StormOrdinal260
#define StormSFileDdaSetVolumeRaw StormOrdinal261
#define StormSFileDestroyRaw StormOrdinal262
#define StormSFileEnableDirectAccessRaw StormOrdinal263
#define StormSFileGetFileArchiveRaw StormOrdinal264
#define StormSFileGetFileSizeRaw StormOrdinal265
#define StormSFileOpenArchiveRaw StormOrdinal266
#define StormSFileOpenFileRaw StormOrdinal267
#define StormSFileOpenFileExRaw StormOrdinal268
#define StormSFileReadFileRaw StormOrdinal269
#define StormSFileSetBasePathRaw StormOrdinal270
#define StormSFileSetFilePointerRaw StormOrdinal271
#define StormSFileSetLocaleRaw StormOrdinal272
#define StormSFileGetBasePathRaw StormOrdinal273
#define StormSFileSetIoErrorModeRaw StormOrdinal274
#define StormSFileGetArchiveNameRaw StormOrdinal275
#define StormSFileGetFileNameRaw StormOrdinal276
#define StormSFileGetArchiveInfoRaw StormOrdinal277
#define StormSFileSetPlatformRaw StormOrdinal278
#define StormSFileLoadFileRaw StormOrdinal279
#define StormSFileUnloadFileRaw StormOrdinal280
#define StormSFileLoadFileFromRaw StormOrdinal281
#define StormSFilePrioritizeRequestRaw StormOrdinal282
#define StormSFileCancelRequestRaw StormOrdinal283
#define StormSFileSetAsyncRateRaw StormOrdinal284
#define StormSFileSetDataChunkSizeRaw StormOrdinal285
#define StormSFileEnableSeekOptimizationRaw StormOrdinal286
#define StormSFileReadFileExRaw StormOrdinal287
#define StormSFileCancelRequestCheckedRaw StormOrdinal288
#define StormDestroyRaw StormOrdinal301
#define StormGetInstanceRaw StormOrdinal302
#define StormGetOptionRaw StormOrdinal303
#define StormSetOptionRaw StormOrdinal304
#define StormSBltROP3PatternRaw StormOrdinal312
#define StormSBltROP3BlockRaw StormOrdinal313
#define StormSBltROP3ClippedRaw StormOrdinal314
#define StormSBltROP3TiledRaw StormOrdinal315
#define StormSBmpDecodeImageRaw StormOrdinal321
#define StormSBmpLoadImageRaw StormOrdinal323
#define StormSBmpSaveImageRaw StormOrdinal324
#define StormSBmpAllocLoadImageRaw StormOrdinal325
#define StormSBmpSaveImageStrideRaw StormOrdinal326
#define StormSCodeCompileRaw StormOrdinal331
#define StormSCodeDeleteRaw StormOrdinal332
#define StormSCodeExecuteRaw StormOrdinal334
#define StormSCodeGetListingRaw StormOrdinal335
#define StormSDrawInitializeRaw StormOrdinal341
#define StormSDrawCaptureScreenRaw StormOrdinal342
#define StormSDrawClearSurfaceRaw StormOrdinal343
#define StormSDrawDestroyRaw StormOrdinal344
#define StormSDrawFlipPageRaw StormOrdinal345
#define StormSDrawGetFrameWindowRaw StormOrdinal346
#define StormSDrawGetObjectsRaw StormOrdinal347
#define StormSDrawGetScreenSizeRaw StormOrdinal348
#define StormSDrawGetServiceLevelRaw StormOrdinal349
#define StormSDrawLockSurfaceRaw StormOrdinal350
#define StormSDrawManualInitializeRaw StormOrdinal351
#define StormSDrawMessageBoxRaw StormOrdinal352
#define StormSDrawPostCloseRaw StormOrdinal353
#define StormSDrawRealizePaletteRaw StormOrdinal354
#define StormSDrawSelectGdiSurfaceRaw StormOrdinal355
#define StormSDrawUnlockSurfaceRaw StormOrdinal356
#define StormSDrawUpdatePaletteRaw StormOrdinal357
#define StormSDrawUpdateScreenRaw StormOrdinal358
#define StormSDrawWaitVerticalBlankRaw StormOrdinal359
#define StormSEvtDispatchRaw StormOrdinal372
#define StormSEvtRegisterHandlerRaw StormOrdinal373
#define StormSEvtUnregisterHandlerRaw StormOrdinal374
#define StormSEvtUnregisterTypeRaw StormOrdinal375
#define StormSEvtPopStateRaw StormOrdinal376
#define StormSEvtPushStateRaw StormOrdinal377
#define StormSEvtBreakHandlerChainRaw StormOrdinal378
#define StormSGdiBitBltRaw StormOrdinal381
#define StormSGdiCreateFontRaw StormOrdinal382
#define StormSGdiDeleteObjectRaw StormOrdinal383
#define StormSGdiExtTextOutRaw StormOrdinal385
#define StormSGdiImportFontRaw StormOrdinal386
#define StormSGdiLoadFontRaw StormOrdinal387
#define StormSGdiRectangleRaw StormOrdinal388
#define StormSGdiSelectObjectRaw StormOrdinal389
#define StormSGdiSetPitchRaw StormOrdinal390
#define StormSGdiTextOutRaw StormOrdinal391
#define StormSGdiSetTargetDimensionsRaw StormOrdinal392
#define StormSGdiGetTextExtentRaw StormOrdinal393
#define StormSMemAllocRaw StormOrdinal401
#define StormSMemFreeRaw StormOrdinal403
#define StormSMemGetSizeRaw StormOrdinal404
#define StormSMemReAllocRaw StormOrdinal405
#define StormSMsgDispatchMessageRaw StormOrdinal412
#define StormSMsgRunMessageLoopRaw StormOrdinal413
#define StormSMsgRegisterCommandRaw StormOrdinal414
#define StormSMsgRegisterKeyDownRaw StormOrdinal415
#define StormSMsgRegisterKeyUpRaw StormOrdinal416
#define StormSMsgRegisterMessageRaw StormOrdinal417
#define StormSMsgPopRegisterStateRaw StormOrdinal418
#define StormSMsgPushRegisterStateRaw StormOrdinal419
#define StormSRegLoadDataRaw StormOrdinal421
#define StormSRegLoadStringRaw StormOrdinal422
#define StormSRegLoadValueRaw StormOrdinal423
#define StormSRegSaveDataRaw StormOrdinal424
#define StormSRegSaveStringRaw StormOrdinal425
#define StormSRegSaveValueRaw StormOrdinal426
#define StormSRegGetBaseKeyRaw StormOrdinal427
#define StormSRegDeleteValueRaw StormOrdinal428
#define StormSTransBltRaw StormOrdinal431
#define StormSTransBltUsingRunsRaw StormOrdinal432
#define StormSTransCreateInclusiveRaw StormOrdinal433
#define StormSTransDeleteRaw StormOrdinal434
#define StormSTransDuplicateRaw StormOrdinal436
#define StormSTransIntersectDirtyArrayRaw StormOrdinal437
#define StormSTransInvertMaskRaw StormOrdinal438
#define StormSTransLoadInclusiveRaw StormOrdinal439
#define StormSTransSetDirtyArrayInfoRaw StormOrdinal440
#define StormSTransUpdateDirtyArrayRaw StormOrdinal441
#define StormSTransIsPixelInMaskRaw StormOrdinal442
#define StormSTransCombineMasksRaw StormOrdinal443
#define StormSTransCreateMaskInclusiveRaw StormOrdinal444
#define StormSTransCreateRaw StormOrdinal445
#define StormSTransCreateMaskRaw StormOrdinal446
#define StormSTransLoadRaw StormOrdinal447
#define StormSVidDestroyRaw StormOrdinal451
#define StormSVidGetSizeRaw StormOrdinal452
#define StormSVidInitializeRaw StormOrdinal453
#define StormSVidPlayBeginRaw StormOrdinal454
#define StormSVidPlayBeginFromMemoryRaw StormOrdinal455
#define StormSVidPlayContinueRaw StormOrdinal456
#define StormSVidPlayContinueSingleRaw StormOrdinal457
#define StormSVidPlayEndRaw StormOrdinal458
#define StormSVidSetVolumeRaw StormOrdinal459
#define StormSErrDisplayErrorRaw StormOrdinal461
#define StormSErrGetErrorStrRaw StormOrdinal462
#define StormSErrGetLastErrorRaw StormOrdinal463
#define StormSErrAddMessageSourceRaw StormOrdinal464
#define StormSErrSetLastErrorRaw StormOrdinal465
#define StormSErrReportUnreleasedHandleRaw StormOrdinal467
#define StormSErrSetFatalGuardRaw StormOrdinal468
#define StormSErrRegisterHandlerRaw StormOrdinal469
#define StormSErrUnregisterHandlerRaw StormOrdinal470
#define StormSCmdGetBoolRaw StormOrdinal472
#define StormSCmdGetNumRaw StormOrdinal473
#define StormSCmdGetStringRaw StormOrdinal474
#define StormSCmdProcessRaw StormOrdinal475
#define StormSCmdRegisterArgListRaw StormOrdinal476
#define StormSCmdRegisterArgumentRaw StormOrdinal477
#define StormSCmdIsArgumentSetRaw StormOrdinal478
#define StormSCmdProcessCommandLineRaw StormOrdinal479
#define StormSMemFindNextBlockRaw StormOrdinal481
#define StormSMemFindNextHeapRaw StormOrdinal482
#define StormSMemGetHeapByCallerRaw StormOrdinal483
#define StormSMemGetHeapByPtrRaw StormOrdinal484
#define StormSMemHeapAllocRaw StormOrdinal485
#define StormSMemHeapCreateRaw StormOrdinal486
#define StormSMemHeapDestroyRaw StormOrdinal487
#define StormSMemHeapFreeRaw StormOrdinal488
#define StormSMemHeapReAllocRaw StormOrdinal489
#define StormSMemHeapSizeRaw StormOrdinal490
#define StormSMemCopyRaw StormOrdinal491
#define StormSMemFillRaw StormOrdinal492
#define StormSMemMoveRaw StormOrdinal493
#define StormSMemZeroRaw StormOrdinal494
#define StormSMemCmpRaw StormOrdinal495
#define StormSMemSetDebugFlagsRaw StormOrdinal496
#define StormSMemDumpStateRaw StormOrdinal497
#define StormSStrCopyRaw StormOrdinal501
#define StormSStrHashRaw StormOrdinal502
#define StormSStrAppendRaw StormOrdinal503
#define StormSStrTokenizeRaw StormOrdinal504
#define StormSStrChrAnyDirectionRaw StormOrdinal505
#define StormSStrLenRaw StormOrdinal506
#define StormSStrDupRaw StormOrdinal507
#define StormSStrCmpRaw StormOrdinal508
#define StormSStrCmpIRaw StormOrdinal509
#define StormSMsgBreakHandlerChainRaw StormOrdinal511
#define StormSMsgUnregisterCommandRaw StormOrdinal512
#define StormSMsgUnregisterKeyDownRaw StormOrdinal513
#define StormSMsgUnregisterKeyUpRaw StormOrdinal514
#define StormSMsgUnregisterMessageRaw StormOrdinal515
#define StormSMsgGetDefWindowProcRaw StormOrdinal516
#define StormSDrawSetWindowRaw StormOrdinal517
#define StormSDrawGetWindowRaw StormOrdinal518
#define StormSRgnClearRaw StormOrdinal521
#define StormSRgnCombineRectRaw StormOrdinal523
#define StormSRgnCreateRaw StormOrdinal524
#define StormSRgnDeleteRaw StormOrdinal525
#define StormSRgnDuplicateRaw StormOrdinal527
#define StormSRgnGetRectParamsRaw StormOrdinal528
#define StormSRgnGetRectsRaw StormOrdinal529
#define StormSRgnGetBoundingRectRaw StormOrdinal530
#define StormSRgnIsPointInRegionRaw StormOrdinal531
#define StormSRgnIsRectInRegionRaw StormOrdinal532
#define StormSRgnOffsetRaw StormOrdinal533
#define StormSRgnCombineRectfRaw StormOrdinal534
#define StormSRgnGetRectParamsfRaw StormOrdinal535
#define StormSRgnGetRectsfRaw StormOrdinal536
#define StormSRgnGetBoundingRectfRaw StormOrdinal537
#define StormSRgnIsPointInRegionfRaw StormOrdinal538
#define StormSRgnIsRectInRegionfRaw StormOrdinal539
#define StormSRgnOffsetfRaw StormOrdinal540
#define StormSLogCloseRaw StormOrdinal541
#define StormSLogCreateRaw StormOrdinal542
#define StormSLogSetTimeStampsRaw StormOrdinal543
#define StormSLogDumpRaw StormOrdinal544
#define StormSLogFlushRaw StormOrdinal545
#define StormSLogFlushAllRaw StormOrdinal546
#define StormSLogPendRaw StormOrdinal547
#define StormSLogWriteRaw StormOrdinal548
#define StormSLogSetIndentRaw StormOrdinal549
#define StormSLogAddIndentRaw StormOrdinal550
#define StormSCompCompressRaw StormOrdinal551
#define StormSCompDecompressRaw StormOrdinal552
#define StormSErrFatalFmtRaw StormOrdinal561
#define StormSErrDisplayErrorFmtRaw StormOrdinal562
#define StormSErrIsDisplayingErrorRaw StormOrdinal563
#define StormSErrSetAssertLocationRaw StormOrdinal564
#define StormSStrChrRaw StormOrdinal571
#define StormSStrChrRRaw StormOrdinal572
#define StormSStrToDoubleRaw StormOrdinal573
#define StormSStrToDoubleCopyRaw StormOrdinal574
#define StormSStrToIntRaw StormOrdinal575
#define StormSStrToUnsignedRaw StormOrdinal576
#define StormSStrToInt64Raw StormOrdinal577
#define StormSStrPrintfRaw StormOrdinal578
#define StormSBigAddRaw StormOrdinal601
#define StormSBigAndRaw StormOrdinal602
#define StormSBigCompareRaw StormOrdinal603
#define StormSBigCopyRaw StormOrdinal604
#define StormSBigDecRaw StormOrdinal605
#define StormSBigDelRaw StormOrdinal606
#define StormSBigDivRaw StormOrdinal607
#define StormSBigFindPrimeRaw StormOrdinal608
#define StormSBigFromBinaryRaw StormOrdinal609
#define StormSBigFromStrRaw StormOrdinal610
#define StormSBigFromStuffedRaw StormOrdinal611
#define StormSBigFromUnsignedRaw StormOrdinal612
#define StormSBigGcdRaw StormOrdinal613
#define StormSBigIncRaw StormOrdinal614
#define StormSBigInvModRaw StormOrdinal615
#define StormSBigIsEvenRaw StormOrdinal616
#define StormSBigIsOddRaw StormOrdinal617
#define StormSBigIsOneRaw StormOrdinal618
#define StormSBigIsPrimeRaw StormOrdinal619
#define StormSBigIsZeroRaw StormOrdinal620
#define StormSBigModRaw StormOrdinal621
#define StormSBigMulRaw StormOrdinal622
#define StormSBigMulModRaw StormOrdinal623
#define StormSBigNewRaw StormOrdinal624
#define StormSBigNotRaw StormOrdinal625
#define StormSBigOrRaw StormOrdinal626
#define StormSBigPowRaw StormOrdinal627
#define StormSBigPowModRaw StormOrdinal628
#define StormSBigRandRaw StormOrdinal629
#define StormSBigSetPowerOfTwoRaw StormOrdinal630
#define StormSBigSetOneRaw StormOrdinal631
#define StormSBigSetZeroRaw StormOrdinal632
#define StormSBigShlRaw StormOrdinal633
#define StormSBigShrRaw StormOrdinal634
#define StormSBigSquareRaw StormOrdinal635
#define StormSBigSubtractRaw StormOrdinal636
#define StormSBigToBinaryArrayRaw StormOrdinal637
#define StormSBigToBinaryBufferRaw StormOrdinal638
#define StormSBigToBinaryPtrRaw StormOrdinal639
#define StormSBigToStrArrayRaw StormOrdinal640
#define StormSBigToStrBufferRaw StormOrdinal641
#define StormSBigToStrPtrRaw StormOrdinal642
#define StormSBigToStuffedArrayRaw StormOrdinal643
#define StormSBigToStuffedBufferRaw StormOrdinal644
#define StormSBigToStuffedPtrRaw StormOrdinal645
#define StormSBigToUnsignedRaw StormOrdinal646
#define StormSBigXorRaw StormOrdinal647

#endif
