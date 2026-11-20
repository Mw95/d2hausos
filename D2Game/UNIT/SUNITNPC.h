#ifndef D2RE_D2GAME_SUNITNPC_H
#define D2RE_D2GAME_SUNITNPC_H

#include "../D2game.h"

void __fastcall SUnitNpcSendUnitEvent(UNIT *hUnit, CLIENT *ptClient);
void __fastcall SUnitNpcSendHires(GAME *ptGame, CLIENT *ptClient, UNIT *hNpc, BOOL bFirst);
void __fastcall SUnitNpcSell(GAME *ptGame, UNIT *hPlayer, DWORD dwNpcId, DWORD dwItemId, WORD wItemMode,
	DWORD dwUnused4);
void __fastcall SUnitNpcRestoreHire(GAME *ptGame, UNIT *hPlayer, DWORD dwHire, DWORD dwSeed);
void __fastcall SUnitNpcBuy(GAME *ptGame, UNIT *hPlayer, DWORD dwNpcId, DWORD dwItemId, DWORD dwUnused3,
	DWORD dwItemMode, DWORD dwUnused5);
void __fastcall SUnitNpcHire(GAME *ptGame, UNIT *hPlayer, DWORD dwNpcId, WORD wHire);
void __fastcall SUnitNpcRepairItem(GAME *ptGame, UNIT *hPlayer, DWORD dwNpcId, DWORD dwItemId, WORD wItemMode,
	DWORD dwUnused4);
void __fastcall SUnitNpcRepairAll(GAME *ptGame, UNIT *hPlayer, DWORD dwNpcId);
void __fastcall SUnitNpcInteract(GAME *ptGame, UNIT *hPlayer, DWORD dwAction, DWORD dwNpcId,
	DWORD dwItemId);
void __fastcall SUnitNpcMarkBought(GAME *ptGame, UNIT *hPlayer, DWORD dwItemId);
void __fastcall SUnitNpcTellThreat(GAME *ptGame, UNIT *hUnit, UNIT *hNpc);
void __fastcall SUnitNpcBuyHeal(GAME *ptGame, UNIT *hPlayer, DWORD dwNpcId);
void __fastcall SUnitNpcLeave(GAME *ptGame, UNIT *hUnit, UNIT *hNpc);
void __fastcall SUnitNpcGiveHire(GAME *ptGame, UNIT *hUnit, DWORD dwNpcClass);
BOOL __fastcall SUnitNpcCanTakeItem(GAME *ptGame, UNIT *hPlayer, UNIT *hNpc, UNIT *hItem, BOOL fOwn);

void __fastcall SUnitNpcReleaseItem(UNIT *hNPC, UNIT *hItem, GAME *ptGame, BOOL fToNpc);
void __fastcall SUnitNpcInitHires(GAME *ptGame, DWORD dwClassId, NPCPROXY *pInv);
void __fastcall SUnitNpcFillStock(GAME *ptGame, UNIT *hPlayer, UNIT *hNpc, NPCSTATE *pList);
UNIT *__fastcall SUnitNpcCreateStockItem(UNIT *hNPC, DWORD dwRow, GAME *ptGame, BOOL fAdd, int nQuality,
	int nLevel);
UNIT *__fastcall SUnitNpcCopyItem(GAME *ptGame, UNIT *hItem, UNIT *hNpc);
void __fastcall SUnitNpcBuyItem(GAME *ptGame, UNIT *hPlayer, UNIT *hNpc, DWORD dwItemId, DWORD dwUnused3,
	DWORD dwItemMode, DWORD dwUnused5);
void __fastcall SUnitNpcReplaceStock(GAME *ptGame, UNIT *hNpc, UNIT *hItem, UNIT *hPlayer, DWORD dwItemMode);
void __fastcall SUnitNpcRenewHires(GAME *ptGame, DWORD dwClassId, NPCPROXY *pInv);
void __fastcall SUnitNpcOpenTrade(GAME *ptGame, UNIT *hPlayer, UNIT *hNPC, BOOL fFirst, BOOL fOwn);
void __fastcall SUnitNpcFillGamble(GAME *ptGame, UNIT *hNpc, UNIT *hPlayer, NPCPROXY *pInv);
void __fastcall SUnitNpcHeal(GAME *ptGame, UNIT *hPlayer, UNIT *hNpc);
void __fastcall SUnitNpcHealPet(GAME *ptGame, UNIT *hPlayer, UNIT *hPet, void *pArg);

#endif
