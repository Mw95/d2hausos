#ifndef D2RE_D2GAME_ITEMMODE_H
#define D2RE_D2GAME_ITEMMODE_H

#include "../D2game.h"

struct ITEMHANDINFO;
struct ITEMCODELIST;

BOOL __fastcall ITEMSModeUpdateBookSkill(UNIT *hUnit, UNIT *hItem,
	BOOL fAdd);
void __fastcall ITEMSModeRemoveEffects(GAME *ptGame, UNIT *hItem, UNIT *hUnit, BOOL fForce,
	BOOL fClamp);
BOOL __fastcall ITEMSModeCanTakeQuestItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
void __fastcall ITEMSModePickupToCursor(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	int *pnResult);
void __fastcall ITEMSModePickupGold(GAME *ptGame, UNIT *hUnit, UNIT *hGold);
BOOL __fastcall ITEMSModeFindBodyLoc(UNIT *hUnit, UNIT *hItem, int *pnLoc);
void __fastcall ITEMSModeGetHandInfo(UNIT *hUnit, UNIT *hItem, ITEMHANDINFO *ptInfo);
BOOL __fastcall ITEMSModeCanHoldTogether(ITEMHANDINFO *ptA,
	ITEMHANDINFO *ptB);
BOOL __fastcall ITEMSModePickupToInventory(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	int *pnResult);
BOOL __fastcall ITEMSModeStackOntoInventory(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
void __fastcall ITEMSModeAddBookCharges(UNIT *hUnit, UNIT *hItem, int nCharges);
BOOL __fastcall ITEMSModeStackItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
BOOL __fastcall ITEMSModePlaceInGrid(GAME *ptGame, UNIT *hUnit, UNIT *hItem, BOOL fFromGround,
	DWORD dwPage);
void __fastcall ITEMSModeDropCursorItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId);
BOOL __fastcall ITEMSModePlaceCursorItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	int nX, int nY, BOOL fAnywhere,
		BOOL fSend,
	void *hOtherInventory);
BOOL __fastcall ITEMSModeTakeToCursor(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	int *pnResult, int nSend, BOOL fFlag20,
	void *hOtherInventory, BOOL fForce);
void __fastcall ITEMSModeRemoveSetBonus(UNIT *hUnit, UNIT *hItem);
void __fastcall ITEMSModeCheckEquipment(GAME *ptGame, UNIT *hUnit, BOOL fSend);
void __fastcall ITEMSModeApplyEffects(GAME *ptGame, UNIT *hItem, UNIT *hUnit, BOOL fForce);
BOOL __fastcall ITEMSModeEquipCursorItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	BYTE bLoc, BOOL fNoFitTest, int *pnResult);
void __fastcall ITEMSModeUpdateWeaponSkills(UNIT *hUnit);
void __fastcall ITEMSModeCheckSkill(UNIT *hUnit, BOOL fLeft, void *hSkillCurr);
void __fastcall ITEMSModeApplySetBonus(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
BOOL __fastcall ITEMSModeSwapHands(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	BYTE bLoc, BOOL fNoFitTest, int *pnResult);
void __fastcall ITEMSModeDropBeltOverflow(GAME *ptGame, UNIT *hUnit, UNIT *hBelt);
BOOL __fastcall ITEMSModeUnequipToCursor(GAME *ptGame, UNIT *hUnit, DWORD dwLoc,
	BOOL fFlag20, int *pnResult);
BOOL __fastcall ITEMSModeSwapBodyItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	BYTE bLoc, BOOL fNoFitTest, int *pnResult);
BOOL __fastcall ITEMSModeEquipTwoHands(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	BYTE bLoc, int *pnResult);
BOOL __fastcall ITEMSModeSwapGridItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwGridItemId, int nX, int nY,
	int *pnResult);
void __fastcall ITEMSModeRemoveGridItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem, BOOL fSend);
void __fastcall ITEMSModeUseUpBeltItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
BOOL __fastcall ITEMSModeUseItemOnItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwTargetId, int *pnResult);
void __fastcall ITEMSModeClearMarkedItems(UNIT *hUnit);
void __fastcall ITEMSModeUseBookCharge(UNIT *hUnit, int nSkill);
BOOL __fastcall ITEMSModeUseItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwX, DWORD dwY, int *pnResult);
BOOL __fastcall ITEMSModeStackCursorItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwTargetId, int *pnResult);
BOOL __fastcall ITEMSModeNone(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwResult);
BOOL __fastcall ITEMSModePutInBelt(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwSlot, BOOL fAnywhere, int *pnResult);
BOOL __fastcall ITEMSModeTakeFromBelt(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	int *pnResult);
BOOL __fastcall ITEMSModeSwapBeltItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwBeltItemId, int *pnResult);
void __fastcall ITEMSModeFreeBeltItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
void __fastcall ITEMSModeBeltShiftDown(GAME *ptGame, UNIT *hUnit, DWORD dwSlot);
BOOL __fastcall ITEMSModeUseBeltItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwX, DWORD dwY, int *pnResult);
void __fastcall ITEMSModeMarkHandItem(UNIT *hUnit, UNIT *hItem);
BOOL __fastcall ITEMSModeFreeCursorItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
BOOL __fastcall ITEMSModeSocketGem(GAME *ptGame, UNIT *hUnit, DWORD dwGemId,
	DWORD dwItemId, int *pnResult, BOOL fSend,
	BOOL fClearCursor, BOOL fCheckMode);
BOOL __fastcall ITEMSModeScrollToTome(GAME *ptGame, UNIT *hUnit, DWORD dwScrollId,
	DWORD dwTomeId, int *pnResult);
BOOL __fastcall ITEMSModePutInCube(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	DWORD dwCubeId, int *pnResult);
BOOL __fastcall ITEMSModeEquipFromGrid(GAME *ptGame, UNIT *hUnit, DWORD dwItemId,
	BYTE bLoc);
void __fastcall ITEMSModeEventExpire(GAME *ptGame, UNIT *hUnit);
void __fastcall ITEMSModeRunEvent(GAME *ptGame, UNIT *hUnit, int eEventType);
BOOL __fastcall ITEMSModeUseKey(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall ITEMSModeTransmogrify(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
BOOL __fastcall ITEMSModeEquipFromGround(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
BOOL __fastcall ITEMSTransferDeadBodyItemsToPlayer(GAME *ptGame, UNIT *hPlayer, UNIT *hDeadBody);
void __fastcall ITEMSModeRefreshItems(GAME *ptGame, UNIT *hUnit, BOOL fSend);
BOOL __fastcall ITEMSModeHasContents(UNIT *hItem);
BOOL __fastcall ITEMSModeCopySockets(GAME *ptGame, UNIT *hUnit, UNIT *hSource,
	UNIT *hDest, BOOL fSend);
UNIT * __fastcall ITEMSModeFindTomeFor(GAME *ptGame, UNIT *hUnit, UNIT *hScroll,
	BOOL fTestOnly);
void __fastcall ITEMSModeBreakItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
void __fastcall ITEMSModeRepairItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
void __fastcall ITEMSModeResetBookSkills(GAME *ptGame, UNIT *hUnit);
BOOL __fastcall ITEMSModeSetHandItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
DWORD __fastcall ITEMSModeDrawCode(GAME *ptGame, ITEMCODELIST *ptList, UNIT *hUnit,
	BOOL fGood);
void __fastcall ITEMSModeUnequipAll(GAME *ptGame, UNIT *hUnit);

#endif
