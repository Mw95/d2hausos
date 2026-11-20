#ifndef D2RE_D2GAME_ITEMS_H
#define D2RE_D2GAME_ITEMS_H

#include "../D2game.h"

void __fastcall ITEMSInitUnit(UNIT *hItem, GAME *ptGame, DWORD dwUnitId,
	DWORD dwUnused);
GAMEROOM *__fastcall ITEMSFindDropSpot(GAMEROOM *hRoom, COLPOINT *ptCoord,
	COLPOINT *ptSpawn, int nSize);
int __fastcall ITEMSChooseArmor(D2SEED *ptSeed, int nLevelId, int nType,
	BOOL fNoRarity);
BOOL __fastcall ITEMSCanSpawn(D2SEED *ptSeed, ITEMSTXT *ptStat, int nLevelId, int nType,
	BOOL fNoRarity);
int __fastcall ITEMSChooseWeapon(D2SEED *ptSeed, int nLevelId, int nType,
	BOOL fNoRarity);
int __fastcall ITEMSChooseOtherItem(D2SEED *ptSeed, int nLevelId, int nType,
	BOOL fNoRarity, BOOL fType28);
void __fastcall ITEMSRollMagicQuality(UNIT *hUnit, int nLevel, int nDivisor,
	DWORD dwClassId, DWORD *pdwQuality);
BOOL __fastcall ITEMSApplyQuality(GAME *ptGame, UNIT **phItem, ITEMCREATE *ptCreate);
BOOL __fastcall ITEMSIsUniqueInGame(GAME *ptGame, int nClass);
BOOL __fastcall ITEMSAddUniqueToGame(GAME *ptGame, UNIT *hItem, BOOL fRecreate);
BOOL __fastcall ITEMSMakeRare(UNIT *hItem, ITEMCREATE *ptCreate);
int __fastcall ITEMSChooseMagicSuffix(UNIT *hItem, BOOL fForce, BOOL fApply);
int __fastcall ITEMSChooseMagicPrefix(UNIT *hItem, BOOL fForce, BOOL fApply);
BOOL __fastcall ITEMSMakeMagic(UNIT **phItem, ITEMCREATE *ptCreate);
BOOL __fastcall ITEMSMakeSet(GAME *ptGame, UNIT *hItem);
BOOL __fastcall ITEMSMakeHiQuality(GAME *ptGame, UNIT *hItem, ITEMCREATE *ptCreate);
void __fastcall ITEMSMakeNormal(GAME *ptGame, UNIT *hItem, ITEMCREATE *ptCreate);
int __fastcall ITEMSFindBook(UNIT *hItem, BOOL fScroll);
int __fastcall ITEMSPercent(int nValue, int nPercent,
	int nMin);
int __fastcall ITEMSRollQuality(UNIT *hItem, ITEMCREATE *ptCreate);
void __fastcall ITEMSClearMods(UNIT **phItem);
void __fastcall ITEMSMakeNormalSeeded(GAME *ptGame, ITEMCREATE *ptCreate, UNIT **phItem,
	DWORD dwSeed);
BOOL __fastcall ITEMSTryHiQuality(GAME *ptGame, ITEMCREATE *ptCreate, UNIT **phItem,
	DWORD *pdwSeed);
BOOL __fastcall ITEMSTryMagic(GAME *ptGame, ITEMCREATE *ptCreate, UNIT **phItem,
	DWORD *pdwSeed);
BOOL __fastcall ITEMSTryRare(GAME *ptGame, ITEMCREATE *ptCreate, UNIT **phItem,
	DWORD *pdwSeed);
BOOL __fastcall ITEMSInitStats(GAME *ptGame, UNIT **phItem, ITEMCREATE *ptCreate);
UNIT *__fastcall ITEMSCreateFromRecord(GAME *ptGame, ITEMCREATE *ptCreate, BOOL fSeeds);
UNIT *__fastcall ITEMSRecreate(GAME *ptGame, ITEMCREATE *ptCreate);
UNIT *__fastcall ITEMSDropRandom(GAME *ptGame, GAMEROOM *hRoom, COLPOINT *ptCoord,
	ITEMCREATE *ptCreateOut);
int __fastcall ITEMSChooseItem(D2SEED *ptSeed, int nLevelId, int nType,
	BOOL fNoRarity, BOOL fType28);
UNIT *__fastcall ITEMSDropGold(GAME *ptGame, GAMEROOM *hRoom, COLPOINT *ptCoord);
UNIT *__fastcall ITEMSDropArmor(GAME *ptGame, GAMEROOM *hRoom, COLPOINT *ptCoord,
	int nType, BOOL fNoRarity, UNIT *hSpawner);
UNIT *__fastcall ITEMSDropWeapon(GAME *ptGame, GAMEROOM *hRoom, COLPOINT *ptCoord,
	int nType, BOOL fNoRarity, UNIT *hSpawner);
UNIT *__fastcall ITEMSDropOtherItem(GAME *ptGame, GAMEROOM *hRoom, COLPOINT *ptCoord,
	int nType, BOOL fNoRarity, UNIT *hSpawner);
UNIT *__fastcall ITEMSDropFromUnit(GAME *ptGame, UNIT *hUnit, DWORD dwQuality,
	int *pnLevel, ITEMCREATE *ptCreateOut, int nType,
	BOOL fNoRarity);
UNIT *__fastcall ITEMSCreate(UNIT *hSpawner, DWORD dwClassId, GAME *ptGame, DWORD dwMode,
	DWORD dwQuality, DWORD dwNoSockets, int nLevel,
	BOOL fSeeds,
	DWORD dwSeed, DWORD dwItemSeed);
BOOL __fastcall ITEMSIsInInventory(UNIT *hUnit, UNIT *hItem, void *hOtherInventory);
UNIT *__fastcall ITEMSFindByType(UNIT *hUnit, DWORD dwType);
void __fastcall ITEMSSetForcedPrefix(int nPrefix);
void __fastcall ITEMSSetForcedSuffix(int nSuffix);
int __fastcall ITEMSGetUnitLevel(UNIT *hUnit, int eLevelId);
void __fastcall ITEMSFree(GAME *ptGame, UNIT *hItem);
void __fastcall ITEMSFreeUniqueLists(GAME *ptGame);
void __fastcall ITEMSWear(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
void __fastcall ITEMSFillCreate(GAME *ptGame, ITEMCREATE *ptCreate, UNIT *hItem,
	DWORD dwRecreate);
void __fastcall ITEMSDropGoldPiles(GAME *ptGame, UNIT *hUnit, DWORD dwGold);
void __fastcall ITEMSAddGold(GAME *ptGame, UNIT *hUnit, DWORD dwAmount);
void __fastcall ITEMSDropEar(GAME *ptGame, UNIT *hUnit);
DWORD __fastcall ITEMSGetHealingPotion(UNIT *hUnit);
DWORD __fastcall ITEMSGetManaPotion(UNIT *hUnit);
UNIT *__fastcall ITEMSDuplicate(GAME *ptGame, UNIT *hItem, UNIT *hOwner);
void __fastcall ITEMSFreeAll(GAME *ptGame);

#endif
