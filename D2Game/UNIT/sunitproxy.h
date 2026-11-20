#ifndef D2RE_D2GAME_SUNITPROXY_H
#define D2RE_D2GAME_SUNITPROXY_H

#include "../D2game.h"

NPCPROXY *__fastcall SUnitProxyFindNpc(GAME *ptGame, DWORD mc, int *pnIndex);
NPCPROXY *__fastcall SUnitProxyFindNpcOf(GAME *ptGame, UNIT *hNpc, int *pnIndex);
void __fastcall SUnitProxyGiveItem(UNIT *hNpc, GAME *ptGame, UNIT *hItem, DWORD dwArg);
void __fastcall SUnitProxyAlloc(GAME *ptGame);
void __fastcall SUnitProxyFree(GAME *ptGame);
void __fastcall SUnitProxyFreeNpc(GAME *ptGame, NPCPROXY *pInv);
void __fastcall SUnitProxyOrder(UNIT *hNpc, GAME *ptGame, DWORD dwLevel, DWORD dwArg8, DWORD dwArg4);
void __fastcall SUnitProxyChangeLevel(GAME *ptGame, UNIT *hUnit, DWORD dwLevelOld, DWORD dwLevelNew);
void __fastcall SUnitProxyRenew(GAME *ptGame, UNIT *hUnit, BYTE bAct, BOOL bEmpty);
void __fastcall SUnitProxyCountInLevel(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall SUnitProxyPlayerLeft(GAME *ptGame, UNIT *hUnit);
void __fastcall SUnitProxyLevelUp(GAME *ptGame, UNIT *hUnit, int nLevel);
BYTE __fastcall SUnitProxyGetItemKind(UNIT *hItem);
void __fastcall SUnitProxySellItem(GAME *ptGame, UNIT *hNpc, UNIT *hItem, DWORD dwArg);
void __fastcall SUnitProxyCountStock(NPCPROXY *pInv, UNIT *hNpc, UNIT *hItem, GAME *ptGame, int nDelta);
void __fastcall SUnitProxyBuyItem(GAME *ptGame, UNIT *hNpc, UNIT *hItem);
void *__fastcall SUnitProxyGetInventory(GAME *ptGame, DWORD dwClassId);
D2SEED *__fastcall SUnitProxyGetSeed(GAME *ptGame);
void __fastcall SUnitProxyRemovePlayer(GAME *ptGame, UNIT *hPlayer, UNIT *hNpc);
void __fastcall SUnitProxySendItems(GAME *ptGame, UNIT *hNpc, UNIT *hPlayer, CLIENT *ptClient);
void *__fastcall SUnitProxyGetPlayerInventory(GAME *ptGame, UNIT *hPlayer, UNIT *hNpc);
NPCVISIT *__fastcall SUnitProxyAddVisit(GAME *ptGame, NPCPROXY *pInv, UNIT *hPlayer);
void __fastcall SUnitProxyResetNpc(GAME *ptGame, NPCPROXY *pInv, UNIT *hNpc);
void __fastcall SUnitProxyInit(void);
void __fastcall SUnitProxyBuildVendor(int nVendor);
void __fastcall SUnitProxyFreeVendors(void);

extern DWORD gdwItemCqv;
extern DWORD gdwItemAqv;

#endif
