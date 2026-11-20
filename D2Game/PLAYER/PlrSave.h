#ifndef D2RE_D2GAME_PLRSAVE_H
#define D2RE_D2GAME_PLRSAVE_H

#include "../D2game.h"

int __fastcall PlrSaveWriteItems(UNIT *hUnit, BYTE *pbBuffer, int nSize,
	BOOL fTrade);
void __fastcall PlrSaveCharacter(GAME *ptGame, UNIT *hUnit, const char *pszName);
int __fastcall PlrSaveReadItems(GAME *ptGame, UNIT *hUnit, BYTE *pbData,
	DWORD dwVersion, int nSize, BOOL fCorpse,
	int *pnRead, struct ITEMSAVELIST *ptList);
int __fastcall PlrSaveLoadCharacter(GAME *ptGame, CLIENT *ptClient, char *szName, DWORD dwArg,
	UNIT **phUnit);
int __fastcall PlrSaveOperateStash(OBJOPERATE *pObjectParam);
void __fastcall PlrSaveRequestTrade(GAME *ptGame, UNIT *hUnit, UNIT *hOther);
extern DWORD gdwPlrSaveEnabled;

#endif
