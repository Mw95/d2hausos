#ifndef D2RE_D2GAME_PLRINTRO_H
#define D2RE_D2GAME_PLRINTRO_H

#include "../D2game.h"

void __fastcall PlrIntroFree(GAME *ptGame, PLRINTRO *ptIntro);

void __fastcall PlrIntroSetMet(UNIT *hUnit, GAME *ptGame, int nClass);
DWORD __fastcall PlrIntroTestMet(UNIT *hUnit, GAME *ptGame, int nClass);
void __fastcall PlrIntroSetIntroduced(PLRINTRO *ptIntro, int nClass);
DWORD __fastcall PlrIntroTestIntroduced(PLRINTRO *ptIntro, int nClass);

#endif
