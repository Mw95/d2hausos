#ifndef D2RE_D2GAME_PLAYERPETS_H
#define D2RE_D2GAME_PLAYERPETS_H

#include "../D2game.h"

typedef void (__fastcall *PFNPETCALLBACK)(GAME *ptGame, UNIT *hPlayer, UNIT *hPet, void *pArg);

BOOL __fastcall PlayerPetsTestAnyPlayer(GAME *ptGame, UNIT *hPlayer);
void __fastcall PlayerPetsNew(GAME *ptGame, UNIT *hPlayer);
void __fastcall PlayerPetsClearAll(GAME *ptGame, UNIT *hPlayer);
void __fastcall PlayerPetsFree(GAME *ptGame, UNIT *hPlayer);
void __fastcall PlayerPetsSetMax(GAME *ptGame, UNIT *hPlayer, int eType  ,
	int nMax  );
int __fastcall PlayerPetsGetMax(UNIT *hPlayer, int eType  );
void __fastcall PlayerPetsRemove(GAME *ptGame, UNIT *hPlayer, DWORD dwUnitId, BOOL bKill);
void __fastcall PlayerPetsAdd(GAME *ptGame, UNIT *hPlayer, UNIT *hPet  ,
	int eType  );
void __fastcall PlayerPetsAddData(GAME *ptGame, UNIT *hPlayer, UNIT *hPet  ,
	int eType  , PETDATA *ptData);
int __fastcall PlayerPetsGetType(UNIT *hPlayer, DWORD dwUnitId);
PETDATA * __fastcall PlayerPetsGetData(UNIT *hPlayer, DWORD dwUnitId);
void __fastcall PlayerPetsUpdateMax(UNIT *hPlayer  );
void __fastcall PlayerPetsWarp(GAME *ptGame, UNIT *hPlayer, int nX, int nY);
void __fastcall PlayerPetsSendLife(GAME *ptGame, UNIT *hPlayer);
void __fastcall PlayerPetsForEach(GAME *ptGame, UNIT *hPlayer, PFNPETCALLBACK pfnCallBack
, void *pArg);
void __fastcall PlayerPetsLock(UNIT *hPlayer);
void __fastcall PlayerPetsUnlock(UNIT *hPlayer);
UNIT * __fastcall PlayerPetsFind(GAME *ptGame, UNIT *hPlayer, int ePetType  );
int __fastcall PlayerPetsCount(UNIT *hPlayer);
void __fastcall PlayerPetsSendAll(CLIENT *ptClient, UNIT *hPlayer);

#endif
