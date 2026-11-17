#ifndef MATCH_D2SOUND_IMPORTS_H
#define MATCH_D2SOUND_IMPORTS_H

#include <windows.h>
#include "StormImports.h"

extern "C" DWORD __stdcall StormOrdinal426(DWORD, DWORD, DWORD, DWORD);

extern "C" DWORD __stdcall StormOrdinal255(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
extern "C" DWORD __stdcall StormOrdinal256(void);
extern "C" DWORD __stdcall StormOrdinal257(DWORD);
extern "C" DWORD __stdcall StormOrdinal258(DWORD, DWORD, DWORD);
extern "C" DWORD __stdcall StormOrdinal260(DWORD);
extern "C" DWORD __fastcall D2gfxOrdinal10029(void);

#define StormSFileDdaBeginAt(hFile, dwBufferSize, dwFlags, dwOffset, lVolume, lPan, pBuffer) \
	((BOOL)StormOrdinal255((DWORD)(hFile), (DWORD)(dwBufferSize), (DWORD)(dwFlags), (DWORD)(dwOffset), \
		(DWORD)(lVolume), (DWORD)(lPan), (DWORD)(pBuffer)))
#define StormSFileDdaDestroy() \
	((BOOL)StormOrdinal256())
#define StormSFileDdaEnd(hFile) \
	((BOOL)StormOrdinal257((DWORD)(hFile)))
#define StormSFileDdaGetPos(hFile, pdwPosition, pdwMaxPosition) \
	((BOOL)StormOrdinal258((DWORD)(hFile), (DWORD)(pdwPosition), (DWORD)(pdwMaxPosition)))
#define StormSFileDdaInitialize(pDirectSound) \
	((BOOL)StormOrdinal260((DWORD)(pDirectSound)))
#define D2gfxWindowGetHandle() \
	((HWND)D2gfxOrdinal10029())

#define StormSRegSaveValue(pszKey, pszValue, dwFlags, dwValue) \
	((BOOL)StormOrdinal426((DWORD)(pszKey), (DWORD)(pszValue), (DWORD)(dwFlags), (DWORD)(dwValue)))

#endif
