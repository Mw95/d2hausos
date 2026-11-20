#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\INVENTORY\\InvMode.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "../GAME/SCmd.h"
#include "InvMode.h"
#include "../ITEMS/Items.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUNITNPC.h"

#define UNIT_ITEM			4

static DWORD sgadwItemCmdFlags[20] =
{
	2, 4, 8, 16, 32, 64, 128, 256, 262144, 1024,
	2048, 4096, 8192, 512, 16384, 32768, 65536, 131072, 524288, 1048576,
};
static DWORD sgnItemCmdFlags = 20;
static DWORD sgadwItemFlags[8] =
{
	32, 2, 8, 128, 64, 1, 512, 262144,
};
static DWORD sgnItemFlags = 8;
static DWORD PLACEHOLDER_UnreferencedData_100aa290[21] =
{
	1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024,
	2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576,
};
static DWORD PLACEHOLDER_UnreferencedData_100aa2e4[6] =
{
	1, 4, 8, 32, 64, 128,
};

void __fastcall INVENTORYModeSendItems(GAME *ptGame, UNIT *hUnit, CLIENT *ptClient,
	void *hOtherInventory)
{
	void *hInventory;
	void *hItemInventory;
	void *ptNode;
	void *ptItemNode;
	UNIT *hItem;
	UNIT *hSubItem;

	hInventory = hUnit->ptInventory;
#line 235
	D2ASSERT(hInventory);
	if (hOtherInventory)
		hInventory = hOtherInventory;
	for (ptNode = D2CommonINVENTORYGetDeltaListHead(hInventory); ptNode;
		ptNode = (void *)D2CommonINVENTORYNodeGetKeyB(ptNode))
	{
		hItem = SUnitFind(ptGame, UNIT_ITEM, D2CommonINVENTORYNodeGetDeltaItemId(ptNode));
		if (!hItem)
			continue;
		INVENTORYModeSendItem(hUnit, hItem, NULL, ptClient);
		if (!(hItem->dwFlags & 1))
			continue;
		hItemInventory = hItem->ptInventory;
#line 253
		D2ASSERT(hItemInventory);
		for (ptItemNode = D2CommonINVENTORYGetDeltaListHead(hItemInventory); ptItemNode;
			ptItemNode = (void *)D2CommonINVENTORYNodeGetKeyB(ptItemNode))
		{
			hSubItem = SUnitFind(ptGame, UNIT_ITEM, D2CommonINVENTORYNodeGetDeltaItemId(ptItemNode));
			if (hSubItem)
				INVENTORYModeSendItem(hUnit, hItem, hSubItem, ptClient);
		}
	}
}

BOOL __fastcall INVENTORYModeSendItem(UNIT *hUnit, UNIT *hItem, UNIT *hUnused,
	CLIENT *ptClient)
{
	UNIT *hPlayer;
	GAME *ptGame;
	UNIT *hTrade;
	DWORD dwMode;

	hPlayer = ClientGetPlayerUnit(ptClient, FALSE);
	ptGame = ClientGetGame(ptClient);
	hTrade = SUnitGetInteractUnit(ptGame, hPlayer);
	if (hItem->dwFlags & 4)
	{
		if (hTrade == hUnit && SUnitNpcCanTakeItem(ptGame, hPlayer, hTrade, hItem, 0))
			SCmdSendItemCmd(ptClient, hUnit, hItem, 49);
		return TRUE;
	}
	if (hItem->dwFlags & 16)
	{
		if (hTrade == hUnit && SUnitNpcCanTakeItem(ptGame, hPlayer, hTrade, hItem, 1))
			SCmdSendItemCmd(ptClient, hUnit, hItem, 50);
		return TRUE;
	}
	if (ITEMSDataTestCmdFlags(hItem, 64) && hUnit == hPlayer)
	{
		SCmdSendItemToCursor(ptClient, hUnit, hItem, 0);
		return TRUE;
	}
	if (ITEMSDataTestCmdFlags(hItem, 524288) && hUnit == hPlayer)
	{
		SCmdSendItemNoPlace(ptClient, hUnit, hItem, 0);
		return TRUE;
	}
	if ((ITEMSDataTestCmdFlags(hItem, 2) || ITEMSDataTestCmdFlags(hItem, 128)) && hUnit == hPlayer)
	{
		SCmdSendItemToGrid(ptClient, hUnit, hItem, 0);
		return TRUE;
	}
	if (ITEMSDataTestCmdFlags(hItem, 4) && hUnit == hPlayer)
	{
		SCmdSendItemFromGrid(ptClient, hUnit, hItem, 0);
		return TRUE;
	}
	if (ITEMSDataTestCmdFlags(hItem, 8) || ITEMSDataTestCmdFlags(hItem, 512))
	{
		SCmdSendItemEquip(ptClient, hUnit, hItem, 0);
		return TRUE;
	}
	if (ITEMSDataTestCmdFlags(hItem, 65536))
	{
		SCmdSendItemSwapHands(ptClient, hUnit, hItem, 0);
		return TRUE;
	}
	if (ITEMSDataTestCmdFlags(hItem, 16))
	{
		SCmdSendItemUnequip(ptClient, hUnit, hItem, 0);
		return TRUE;
	}
	if (ITEMSDataTestCmdFlags(hItem, 32))
	{
		SCmdSendItemSwapBodyLoc(ptClient, hUnit, hItem, 0);
		return TRUE;
	}
	if (ITEMSDataTestCmdFlags(hItem, 256) && hUnit == hPlayer)
	{
		SCmdSendItemStack(ptClient, hUnit, hItem, 0);
		return TRUE;
	}
	if (ITEMSDataTestCmdFlags(hItem, 262144) && hUnit == hPlayer)
	{
		SCmdSendItemSwapGrid(ptClient, hUnit, hItem, 0);
		return TRUE;
	}
	if ((ITEMSDataTestCmdFlags(hItem, 1024) || ITEMSDataTestCmdFlags(hItem, 8192)) && hUnit == hPlayer)
	{
		SCmdSendItemToBelt(ptClient, hUnit, hItem, 0);
		return TRUE;
	}
	if (ITEMSDataTestCmdFlags(hItem, 2048) && hUnit == hPlayer)
	{
		SCmdSendItemFromBelt(ptClient, hUnit, hItem, 0);
		return TRUE;
	}
	if (ITEMSDataTestCmdFlags(hItem, 4096) && hUnit == hPlayer)
	{
		SCmdSendItemSwapBelt(ptClient, hUnit, hItem, 0);
		return TRUE;
	}
	if (ITEMSDataTestCmdFlags(hItem, ITEM_CMDFLAG_SETHANDITEM) && hUnit == hPlayer)
	{
		SCmdSendSetHandItem(ptClient, hUnit, hItem);
		return TRUE;
	}
	if (ITEMSDataTestCmdFlags(hItem, 16384))
	{
		SCmdSendItemHandToGrid(ptClient, hUnit, hItem, 0);
		return TRUE;
	}
	if (ITEMSDataTestCmdFlags(hItem, 32768))
	{
		SCmdSendItemEquipFromGrid(ptClient, hUnit, hItem, 0);
		return TRUE;
	}
	if (D2CommonITEMSDataTestFlags(hItem, 256))
	{
		if (hUnit != hPlayer && hItem->dwMode != 1)
			return FALSE;
		SCmdSendItemFlag(ptClient, hUnit, hItem, 256, D2CommonITEMSDataTestFlags(hItem, 256));
		return TRUE;
	}
	if (D2CommonITEMSDataTestFlags(hItem, 512))
	{
		if (hUnit != hPlayer && hItem->dwMode != 1)
			return FALSE;
		SCmdSendItemFlag(ptClient, hUnit, hItem, 512, D2CommonITEMSDataTestFlags(hItem, 512));
		return TRUE;
	}
	if (!(!D2CommonITEMSDataTestFlags(hItem, 1)))
	{
		dwMode = hItem->dwMode;
		if (hUnit != hPlayer && (dwMode == 0 || dwMode == 2))
			return FALSE;
		if (dwMode != 0 && dwMode != 1 && dwMode != 2)
			return FALSE;
		if (hUnit == hPlayer && D2CommonITEMSDataTestFlags(hItem, 262144))
			return FALSE;
		SCmdSendItemHeld(ptClient, hUnit, hItem, 0);
		return TRUE;
	}
	return FALSE;
}

void __fastcall INVENTORYModeClearItemFlags(GAME *ptGame, UNIT *hItem)
{
	DWORD i;

	hItem->dwFlags &= ~20;
	for (i = 0; i < sgnItemCmdFlags; i++)
		D2CommonITEMSDataSetCmdFlags(hItem, sgadwItemCmdFlags[i], 0);
	for (i = 0; i < sgnItemFlags; i++)
		D2CommonITEMSDataSetFlags(hItem, sgadwItemFlags[i], 0);
}

BOOL __fastcall INVENTORYModeHasItems(GAME *ptGame, UNIT *hUnit)
{
	void *ptNode;
	void *ptNext;
	UNIT *hItem;

	if (!(hUnit->dwFlags & 12582912))
	{
		for (ptNode = D2CommonINVENTORYGetDeltaListHead(hUnit->ptInventory); ptNode; ptNode = ptNext)
		{
			hItem = SUnitFind(ptGame, UNIT_ITEM, D2CommonINVENTORYNodeGetDeltaItemId(ptNode));
			ptNext = (void *)D2CommonINVENTORYNodeGetKeyB(ptNode);
			if (hItem)
				return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}

void __fastcall INVENTORYModeResetItems(GAME *ptGame, UNIT *hUnit)
{
	void *hInventory;
	void *hItemInventory;
	void *ptNode;
	void *ptItemNode;
	UNIT *hItem;
	UNIT *hSubItem;

	hInventory = hUnit->ptInventory;
	if (!hInventory)
		return;
	D2CommonUnitSetChanged(hUnit, 0);
	for (ptNode = D2CommonINVENTORYGetDeltaListHead(hInventory); ptNode;
		ptNode = (void *)D2CommonINVENTORYNodeGetKeyB(ptNode))
	{
		hItem = SUnitFind(ptGame, UNIT_ITEM, D2CommonINVENTORYNodeGetDeltaItemId(ptNode));
		if (!hItem)
			continue;
		if (ITEMSDataTestCmdFlags(hItem, 16) || ITEMSDataTestCmdFlags(hItem, 16384))
			D2CommonITEMSSetCategory(hItem, 0);
		if (ITEMSDataTestCmdFlags(hItem, 32) && D2CommonITEMSDataTestFlags(hItem, 128))
			D2CommonITEMSSetCategory(hItem, 0);
		INVENTORYModeClearItemFlags(ptGame, hItem);
		if (hItem->dwFlags & 1)
		{
			D2CommonUnitSetChanged(hItem, 0);
			hItemInventory = hItem->ptInventory;
#line 451
			D2ASSERT(hItemInventory);
			for (ptItemNode = D2CommonINVENTORYGetDeltaListHead(hItemInventory); ptItemNode;
				ptItemNode = (void *)D2CommonINVENTORYNodeGetKeyB(ptItemNode))
			{
				hSubItem = SUnitFind(ptGame, UNIT_ITEM, D2CommonINVENTORYNodeGetDeltaItemId(ptItemNode));
				if (hSubItem)
					INVENTORYModeClearItemFlags(ptGame, hSubItem);
			}
			D2CommonINVENTORYClearDeltaList(hItemInventory);
		}
		if (ITEMSDataTestCmdFlags(hItem, 1))
			ITEMSFree(ptGame, hItem);
	}
	D2CommonINVENTORYClearDeltaList(hInventory);
}

void __fastcall INVENTORYModeSendGroundItem(UNIT *hItem, CLIENT *ptClient)
{
	UNIT *hPlayer;

	if (hItem->dwFlags0 & 16 || hItem->dwMode != 3)
		return;
	hPlayer = ClientGetPlayerUnit(ptClient, FALSE);
	if (hItem->dwFlags0 & 4096)
		SCmdSendItemDropped(ptClient, hPlayer, hItem, 0);
	else
		SCmdSendItemOnGround(ptClient, hPlayer, hItem, 0);
}

void __fastcall INVENTORYModeSendChangedItem(UNIT *hItem, CLIENT *ptClient)
{
	UNIT *hPlayer;

	if (!(hItem->dwFlags0 & 1) || hItem->dwFlags0 & 16 || hItem->dwMode != 3)
		return;
	hPlayer = ClientGetPlayerUnit(ptClient, FALSE);
	if (hItem->dwFlags0 & 4096)
		SCmdSendItemDropped(ptClient, hPlayer, hItem, 0);
	else
		SCmdSendItemOnGround(ptClient, hPlayer, hItem, 0);
}
