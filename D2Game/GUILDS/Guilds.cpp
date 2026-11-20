#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\Guilds\\Guilds.cpp"
#include <stdio.h>
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Arena.h"
#include "../GAME/Clients.h"
#include "../GAME/Game.h"
#include "../GAME/SCmd.h"
#include "../PLAYER/PlrMsg.h"
#include "../UNIT/SUnit.h"
#include "Guilds.h"
#include "FogImports.h"

#define UNIT_OBJECT				2
#define COMPACTED_ITEM_SIZE		25
#define GUILDS_SAVE_SIZE		6114
#define GUILDS_ROSTER_SIZE		2048
#define GUILDS_POSTS_SIZE		1024
#define GUILDS_PIECE_SIZE		400
#define GUILDS_HALL_CLASS		377

void __fastcall GuildsInit(GAME *ptGame)
{
	if (ArenaHasGuild(ptGame))
		ptGame->pGuildData = (void *)D2CommonGuildsDataNew(ptGame->pMemPool);
}

void __fastcall GuildsFree(GAME *ptGame)
{
	if (!ptGame->pGuildData)
		return;
	D2CommonGuildsDataFree(ptGame->pGuildData);
	ptGame->pGuildData = NULL;
}

void __fastcall GuildsSendSave(GAME *ptGame, CLIENT *ptClient)
{
	BYTE abBuffer[GUILDS_SAVE_SIZE];
	const char *pszTag;
	int nTotal;
	int nLeft;
	WORD wOffset;
	WORD wLength;

	if (!ptGame->pGuildData)
		return;
	nLeft = D2CommonGuildsWriteSave(ptGame->pGuildData, abBuffer, GUILDS_SAVE_SIZE, FALSE);
	wOffset = 0;
	nTotal = nLeft;
	pszTag = (const char *)D2CommonGuildsGetTag(ptGame->pGuildData);
	while (nLeft)
	{
		if (nLeft > GUILDS_PIECE_SIZE)
			wLength = GUILDS_PIECE_SIZE;
		else
			wLength = (WORD)nLeft;
		SCmdSendGuildSaveChunk(ptClient, pszTag, wLength, wOffset, (WORD)nTotal, &abBuffer[wOffset]);
		nLeft -= wLength;
		wOffset += wLength;
	}
}

void __fastcall GuildsSendRoster(GAME *ptGame, CLIENT *ptClient)
{
	BYTE abBuffer[GUILDS_ROSTER_SIZE];
	int nTotal;
	int nLeft;
	WORD wOffset;
	WORD wLength;

	if (!ptGame->pGuildData)
		return;
	nLeft = D2CommonGuildsWriteRoster(ptGame->pGuildData, abBuffer, GUILDS_ROSTER_SIZE);
	nTotal = nLeft;
	wOffset = 0;
	while (nLeft)
	{
		if (nLeft > GUILDS_PIECE_SIZE)
			wLength = GUILDS_PIECE_SIZE;
		else
			wLength = (WORD)nLeft;
		SCmdSendGuildRosterChunk(ptClient, (const char *)D2CommonGuildsGetTag(ptGame->pGuildData), wLength,
			wOffset, (WORD)nTotal, &abBuffer[wOffset]);
		nLeft -= wLength;
		wOffset += wLength;
	}
}

void __fastcall GuildsSendPosts(GAME *ptGame, CLIENT *ptClient)
{
	BYTE abBuffer[GUILDS_POSTS_SIZE];
	void *hGuild;
	int nLength;

	hGuild = GuildsGetData(ptGame);
	if (!hGuild)
		return;
	nLength = (int)D2CommonGuildsCopyPostBuffer(hGuild, abBuffer);
	SCmdSendBuffer(ptClient, (WORD)nLength, abBuffer);
}

void __fastcall GuildsPost(GAME *ptGame, CLIENT *ptClient, char *pszText)
{
	void *hGuild;

	hGuild = GuildsGetData(ptGame);
	if (hGuild)
		D2CommonGuildsPostAppend(hGuild, ClientGetName(ptClient), pszText);
}

UNIT * __fastcall GuildsGetHall(GAME *ptGame)
{
	void *hGuild;

	hGuild = GuildsGetData(ptGame);
	if (!hGuild)
		return NULL;
	return SUnitFind(ptGame, UNIT_OBJECT, D2CommonGuildsGetMessageBoardUnitId(hGuild));
}

void * __fastcall GuildsGetData(GAME *ptGame)
{
	if (!ptGame->pGuildData)
		return NULL;
	if (!D2CommonGuildsGetActive(ptGame->pGuildData))
		return NULL;
	return ptGame->pGuildData;
}

void __fastcall GuildsSpawnRankObject(GAME *ptGame, int nLevelId, BYTE bRank)
{
	void *hGuild;
	GAMEROOM *hRoom;
	UNIT *hObject;
	int nX;
	int nY;

	hGuild = GuildsGetData(ptGame);
	if (!hGuild)
		return;
	D2CommonGuildsGetRank(hGuild);
	hRoom = (GAMEROOM *)D2CommonDungeonGetActStartRoom(ptGame->ptAct[NUM_ACTS], nLevelId, 11, &nX,
		&nY, 3);
#line 202
	D2ASSERT(hRoom);
	GameRespawnRoom(ptGame, hRoom);
	hObject = SUnitCreate(UNIT_OBJECT, GUILDS_HALL_CLASS, nX, nY, ptGame, hRoom, 1, 1, 0);
	D2CommonUnitSetMode(hObject, 1);
	((OBJECTDATA *)hObject->uTypeData)->field_04 = bRank;
}

extern void __fastcall GuildsSaveToDatabase(const char *pszTag, BYTE *pbBuffer, int nLength);
__inline void __fastcall GuildsSaveToDatabase(const char *pszTag, BYTE *pbBuffer, int nLength)
{
	if (!nLength || !ptEventCallbackTable->fpSaveDatabaseGuild)
		return;
#line 236
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)ptEventCallbackTable->fpSaveDatabaseGuild),
		"ptEventCallbackTable->fpSaveDatabaseGuild");
	ptEventCallbackTable->fpSaveDatabaseGuild(pszTag, pbBuffer, nLength);
}

void __fastcall GuildsSave(GAME *ptGame)
{
	char szFile[1024];
	char szPath[1024];
	BYTE abBuffer[GUILDS_SAVE_SIZE];
	const char *pszTag;
	int nLength;
	FILE *pFile;

	if (!ptGame->pGuildData)
		return;
	if (!D2CommonGuildsGetChanged(ptGame->pGuildData))
		return;
	D2CommonGuildsSetChanged(ptGame->pGuildData, FALSE);
	pszTag = (const char *)D2CommonGuildsGetTag(ptGame->pGuildData);
	if (!*pszTag)
		return;
	nLength = D2CommonGuildsWriteSave(ptGame->pGuildData, abBuffer, GUILDS_SAVE_SIZE, TRUE);
	if (ptEventCallbackTable)
	{
		GuildsSaveToDatabase(pszTag, abBuffer, nLength);
		return;
	}
	if (!nLength)
		return;
	FogSafesockGetSavePath(szPath, sizeof(szPath));
	sprintf(szFile, "%s%s.d2g", szPath, pszTag);
	pFile = fopen(szFile, "wb");
	if (!pFile)
		return;
	fwrite(abBuffer, nLength, 1, pFile);
	fclose(pFile);
}

void __fastcall GuildsLoadItems(GAME *ptGame, UNIT *hUnit, BYTE *pbData,
	WORD wLength  )
{

	ITEMSAVE tItemSave;
	UNIT *hParent;
	int nUnused;
	int nAttached;
	WORD dwCount;
	DWORD i;
	int j;
	int k;

	dwCount = *(WORD *)pbData;
	wLength -= 2;
	pbData += 2;
	if (!((int)wLength < (int)(dwCount * COMPACTED_ITEM_SIZE)))
	{
		for (i = 0; i < dwCount; i++)
		{
			for (k = 0; k < (int)(sizeof(ITEMSAVE) / sizeof(DWORD)); k++)
				((DWORD *)&tItemSave)[k] = 0;
			D2CommonItemSaveUnpack(&tItemSave, pbData);
			pbData += COMPACTED_ITEM_SIZE;
#line 334
			D2ASSERT(wLength >= COMPACTED_ITEM_SIZE);
			wLength -= COMPACTED_ITEM_SIZE;
			hParent = NULL;
			PlrMsgLoadItem(ptGame, hUnit, &tItemSave, &hParent, &nAttached);
			for (j = 0; j < nAttached; j++)
			{
				for (k = 0; k < (int)(sizeof(ITEMSAVE) / sizeof(DWORD)); k++)
					((DWORD *)&tItemSave)[k] = 0;
				D2CommonItemSaveUnpack(&tItemSave, pbData);
				pbData += COMPACTED_ITEM_SIZE;
#line 348
				D2ASSERT(wLength >= COMPACTED_ITEM_SIZE);
				wLength -= COMPACTED_ITEM_SIZE;
				PlrMsgLoadItem(ptGame, hUnit, &tItemSave, &hParent, &nUnused); }
		} }
}

void __fastcall GuildsLoadVaultItems(GAME *ptGame, UNIT *hUnit)
{
	BYTE *pbData;
	WORD wLength;

	if (!ptGame->pGuildData)
		return;
	D2CommonGuildsGetVaultItemData(ptGame->pGuildData, &pbData, &wLength);
	if (pbData && wLength > 2)
		GuildsLoadItems(ptGame, hUnit, pbData, wLength);
}

void __fastcall GuildsLoadTrophyCaseItems(GAME *ptGame, UNIT *hUnit)
{
	BYTE *pbData;
	WORD wLength;

	if (!ptGame->pGuildData)
		return;
	D2CommonGuildsGetTrophyCaseItemData(ptGame->pGuildData, &pbData, &wLength);
	if (pbData && wLength > 2)
		GuildsLoadItems(ptGame, hUnit, pbData, wLength);
}

BOOL __stdcall GuildsReceiveSave(WORD wGameId, DWORD dwUnused,
	WORD wChunk, WORD wOffset, WORD wTotal,
	const BYTE *pbChunk)
{
	DWORD dwGame;
	GAME *ptGame;

	if (!wTotal || !wChunk)
		return FALSE;
	dwGame = GameFindById(wGameId);
	if (!dwGame)
		return FALSE;
	ptGame = GameLockByHandle(dwGame);
	if (!ptGame->pGuildData)
		return FALSE;
	D2CommonGuildsReceiveSave(ptGame->pGuildData, dwUnused, wChunk, wOffset, wTotal, pbChunk);
	GameUnlock(ptGame);
	return TRUE;
}

BOOL __stdcall GuildsReceiveRoster(WORD wGameId, DWORD dwUnused,
	WORD wChunk, WORD wOffset, WORD wTotal,
	const BYTE *pbChunk)
{
	DWORD dwGame;
	GAME *ptGame;

	if (!wTotal || !wChunk)
		return FALSE;
	dwGame = GameFindById(wGameId);
	if (!dwGame)
		return FALSE;
	ptGame = GameLockByHandle(dwGame);
	if (!ptGame->pGuildData)
		return FALSE;
	D2CommonGuildsReceiveRoster(ptGame->pGuildData, dwUnused, wChunk, wOffset, wTotal, pbChunk);
	GameUnlock(ptGame);
	return TRUE;
}
