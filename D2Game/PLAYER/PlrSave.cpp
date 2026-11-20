#include <windows.h>
#include <stdio.h>
#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\PLAYER\\PlrSave.cpp"
#include "D2Macros.h"
#include "../GAME/Arena.h"
#include "../GAME/Clients.h"
#include "../GAME/Event.h"
#include "../GAME/Game.h"
#include "../GAME/SCmd.h"
#include "../ITEMS/ItemMode.h"
#include "../ITEMS/Items.h"
#include "../OBJECTS/ObjMode.h"
#include "Player.h"
#include "PlayerPets.h"
#include "PlrMsg.h"
#include "PlrSave.h"
#include "PlrTrade.h"
#include "../QUESTS/QUESTS.h"
#include "../QUESTS/a4q3.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitMsg.h"
#include "../GUILDS/Guilds.h"
#include "../SKILLS/Skills.h"
#include "FogImports.h"

#define UNIT_PLAYER			0
#define DIABLO2_NUMACTS		5
#define SAVE_MAGIC			0xAA55AA55
#define SAVE_VERSION		71
#define SAVE_TAG_ITEMS		19786
#define SAVE_BUFFER_SIZE	8192

#pragma pack(push, 1)
struct SAVEQUESTS
{
 	DWORD		dwTag;
 	DWORD		dwField04;
 	WORD		wSize;
 	BYTE		abQuests[3][96];
};
struct SAVEWAYPOINTS
{
 	WORD		wTag;
 	DWORD		dwField02;
 	WORD		wSize;
 	BYTE		abWaypoints[3][24];
};
struct SAVEINTRO
{
 	WORD		wTag;
 	WORD		wSize;
 	DWORD		adwBits0[3][2];
 	DWORD		adwBits1[3][2];
};
struct SAVECORPSE
{
 	DWORD		field_00;
 	int			nX;
 	int			nY;
};
#pragma pack(pop)
struct SAVEPET
{
 	DWORD		dwSeed;
 	WORD		wHireId;
};

BYTE __fastcall ClientGetSaveAct(CLIENT *ptClient);
DWORD __fastcall ClientGetClass(CLIENT *ptClient);
void __fastcall ClientSetClass(CLIENT *ptClient, DWORD dwClass);
void __fastcall ClientSetFlags(CLIENT *ptClient, DWORD dwFlags);
void __fastcall ClientSetPlayerUnit(CLIENT *ptClient, UNIT *hUnit);
BYTE __fastcall ClientGetCharTemplate(CLIENT *ptClient);
BYTE * __fastcall ClientGetBuffer(CLIENT *ptClient);
int __fastcall ClientGetBufferUsed(CLIENT *ptClient);
void __fastcall ClientSetDataSending(CLIENT *ptClient);
void __fastcall ClientSetPair(CLIENT *ptClient, int nIndex, DWORD dwSkillId,
	DWORD dwHand);
void __fastcall ClientGetPair(CLIENT *ptClient, int nIndex, DWORD *pdw0,
	DWORD *pdw1);
void __fastcall ClientSetGuildField(CLIENT *ptClient, WORD wValue);
void __fastcall ClientSetGuildTag(CLIENT *ptClient, const DWORD *pdwValue);
void __fastcall ClientSetGuildName(CLIENT *ptClient, const char *pszValue);
void __fastcall ClientGetGuildEmblem(CLIENT *ptClient, BYTE *pb0, BYTE *pb1,
	BYTE *pb2);
void __fastcall ClientSetGuildEmblem(CLIENT *ptClient,
	BYTE bEmblemIcon, BYTE bEmblemColor1,
	BYTE bEmblemColor2);
void __fastcall SCmdSendSkill(UNIT *hUnit, WORD wSkill, BYTE bSkillLevel,
	BYTE bRemove);
void __fastcall SCmdSendSystemMsg(CLIENT *ptClient, const SCMDSYSTEMMSG *pSystemMsg);
void __fastcall SCmdSendTradeEvent(CLIENT *ptClient, BYTE bValue);
void __fastcall SCmdSendAllCorpse(GAME *ptGame, DWORD dwUnitId,
	DWORD dwBodyId,
	BYTE bAdd);
void __fastcall PlrEvalInit(GAME *ptGame, UNIT *hPlayer, BYTE bAct);
void __fastcall PlrIntroGetMetBits(PLRINTRO *ptIntro, DWORD *pdwBits);
void __fastcall PlrIntroGetIntroducedBits(PLRINTRO *ptIntro, DWORD *pdwBits);
void __fastcall PlrIntroSetMetBits(PLRINTRO *ptIntro, const DWORD *pdwBits);
void __fastcall PlrIntroSetIntroducedBits(PLRINTRO *ptIntro,
	const DWORD *pdwBits);

DWORD gdwPlrSaveEnabled = 1;

static const int sganSkillFirst[5] = { 6, 36, 66, 96, 126 };
static const int sganSkillCount[5] = { 30, 30, 30, 30, 30 };

UNIT * __fastcall PlrSaveRecreateItem(GAME *ptGame, ITEMSAVE *ptItemSave)
{
	ITEMCREATE tCreate;
	UNIT *hItem;
	int i;

	for (i = 0; i < (int)(sizeof(tCreate) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.nLevel = ptItemSave->nLevelId;
	tCreate.dwClassId = ptItemSave->nId;
	tCreate.dwMode = ptItemSave->bMode;
	tCreate.ptGame = ptGame;
	tCreate.nX = ptItemSave->dwX;
	tCreate.nY = ptItemSave->dwY;
	tCreate.dwQuality = ptItemSave->bSpecMods;
	tCreate.nQuantity = ptItemSave->nValue;
	tCreate.nDurability = ptItemSave->dwDurability;
	tCreate.nMaxDurability = ptItemSave->dwMaxDurability;
	tCreate.dwSeed = ptItemSave->dwInitSeed;
	tCreate.hRoom = NULL;
	tCreate.dwItemSeed = ptItemSave->dwItemSeed;
	tCreate.dwFlags = ptItemSave->dwFlags;
	tCreate.dwFileIndex = ptItemSave->nModClass;
	tCreate.wCreateFlags = 1;
	strcpy(tCreate.szName, ptItemSave->szPlayerName);
	tCreate.bEarLevel = ptItemSave->bEarLevel;
	hItem = ITEMSRecreate(ptGame, &tCreate);
	if (!hItem)
		return hItem;
	D2CommonITEMSDataSetUniqueOrSetRow(hItem, ptItemSave->nModClass);
	((void (__stdcall *)(UNIT *, BYTE))D2CommonITEMSDataSetPageRaw)(hItem, ptItemSave->bPage);
	return hItem;
}

void __stdcall PlrSaveCharacterStd(GAME *ptGame, UNIT *hUnit, const char *pszName)
{
	PlrSaveCharacter(ptGame, hUnit, pszName);
}

int __fastcall PlrSaveWriteHeader(GAME *ptGame, UNIT *hUnit, const char *pszName,
	BYTE *pbBuffer, int nSize)
{
	SAVEHEADER tSaveHeader;
	CLIENT *hClient;
	DWORD dwSkillId, dwHand;
	void *hSkill;
	int i;

	if ((DWORD)nSize < sizeof(tSaveHeader))
	{
#line 298
		D2FATAL("Out of memory in save buffer.\n");
	}
	hClient = SUnitGetClient(hUnit);
	memset(&tSaveHeader, 0, sizeof(tSaveHeader));
	if (ArenaHasGuild(ptGame) && GuildsGetData(ptGame))
	{
		if ((int)D2CommonGuildsGetRank(GuildsGetData(ptGame)) >= 1)
			D2CommonGuildsGetEmblem(GuildsGetData(ptGame), &tSaveHeader.abGuildEmblem[0],
				&tSaveHeader.abGuildEmblem[1], &tSaveHeader.abGuildEmblem[2]);
		ClientSetGuildEmblem(hClient, tSaveHeader.abGuildEmblem[0], tSaveHeader.abGuildEmblem[1],
			tSaveHeader.abGuildEmblem[2]);
	}
	else
		ClientGetGuildEmblem(hClient, &tSaveHeader.abGuildEmblem[0], &tSaveHeader.abGuildEmblem[1],
			&tSaveHeader.abGuildEmblem[2]);
	tSaveHeader.dwMagic = SAVE_MAGIC;
	tSaveHeader.dwVersion = SAVE_VERSION;
	strcpy(tSaveHeader.szName, pszName);
	tSaveHeader.wField1c = 221;
	tSaveHeader.wField1e = 16;
	tSaveHeader.wClass = (WORD)ClientGetClass(SUnitGetClient(hUnit));
	tSaveHeader.wSize = sizeof(tSaveHeader);
	tSaveHeader.wLevel = (WORD)D2CommonStatsUnitGetValue(hUnit, 12);
	tSaveHeader.dwFlags = ClientGetFlags(hClient);
	tSaveHeader.dwSeed = ptGame->dwSeed;
	for (i = 0; i < 8; i++)
	{
		ClientGetPair(hClient, i, &dwSkillId, &dwHand);
		tSaveHeader.abHotkeys[i][0] = (BYTE)dwSkillId;
		tSaveHeader.abHotkeys[i][1] = (BYTE)dwHand;
	}
	hSkill = (void *)D2CommonUnitGetLeftSkill(hUnit);
	if (hSkill)
		tSaveHeader.bLeftSkillId = (BYTE)D2CommonSkillsGetNodeType(hSkill);
	hSkill = (void *)D2CommonUnitGetRightSkill(hUnit);
	if (hSkill)
		tSaveHeader.bRightSkillId = (BYTE)D2CommonSkillsGetNodeType(hSkill);
	if (hClient)
	{
		if (ClientGetAct(hClient) != 4)
			tSaveHeader.bAct = ClientGetAct(hClient);
		else
			tSaveHeader.bAct = ClientGetSaveAct(hClient);
	}
#line 368
	D2ASSERT(tSaveHeader.bAct < DIABLO2_NUMACTS);
	tSaveHeader.bAct |= (BYTE)(ptGame->bDifficultyLevel << 4);
	memset(tSaveHeader.abComponentGfx, 255, sizeof(tSaveHeader.abComponentGfx));
	memset(tSaveHeader.abComponentColors, 255, sizeof(tSaveHeader.abComponentColors));
	D2CommonINVENTORYWriteSaveHeaderItems(hUnit, &tSaveHeader);
	tSaveHeader.wGuildFlags = ClientGetGuildField(hClient);
	ClientGetGuildTag(hClient, &tSaveHeader.dwGuildTag);
	ClientGetGuildName(hClient, tSaveHeader.szGuildName);
	memcpy(pbBuffer, &tSaveHeader, sizeof(tSaveHeader));
	return sizeof(tSaveHeader);
}

static int PlrSaveWriteQuests(UNIT *hUnit, BYTE *pbBuffer, int nSize)
{
	SAVEQUESTS tQuests;
	PLRINFO *ptPlrInfo;
	int i;

	if ((DWORD)nSize < sizeof(tQuests))
	{
#line 414
		D2FATAL("Out of memory in save buffer.\n");
	}
	memset(&tQuests, 0, sizeof(tQuests));
	tQuests.dwTag = 560951127;
	tQuests.dwField04 = 6;
	tQuests.wSize = sizeof(tQuests);
	ptPlrInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
	for (i = 0; i < 3; i++)
		D2CommonQuestRecordCopyOut(ptPlrInfo->apQuestRecords[i], tQuests.abQuests[i], 96, 1);
	memcpy(pbBuffer, &tQuests, sizeof(tQuests));
	return sizeof(tQuests);
}

int __fastcall PlrSaveWriteWaypoints(UNIT *hUnit, BYTE *pbBuffer, int nSize)
{
	SAVEWAYPOINTS tWaypoints;
	PLRINFO *ptPlrInfo;
	WORD *pwHistory;
	BYTE *pbRecord;
	int nLength, i;

	nLength = sizeof(tWaypoints);
	if ((DWORD)nSize < (DWORD)nLength)
	{
#line 462
		D2FATAL("Out of memory in save buffer.\n");
	}
	memset(&tWaypoints, 0, nLength);
	tWaypoints.wTag = 21335;
	tWaypoints.dwField02 = 1;
	tWaypoints.wSize = (WORD)nLength;
	ptPlrInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
	for (i = 0; i < 3; i++)
	{
		pwHistory = ptPlrInfo->apwWaypoints[i];
		pbRecord = tWaypoints.abWaypoints[i];
		D2CommonWaypointExportHistory(pwHistory, pbRecord);
	}
	memcpy(pbBuffer, &tWaypoints, nLength);
	return nLength;
}

int __fastcall PlrSaveWriteIntro(UNIT *hUnit, BYTE *pbBuffer, int nSize)
{
	SAVEINTRO tIntro;
	DWORD *pdwBits;
	int nLength, i;

	nLength = sizeof(tIntro);
	if ((DWORD)nSize < (DWORD)nLength)
	{
#line 509
		D2FATAL("Out of memory in save buffer.\n");
	}
	memset(&tIntro, 0, nLength);
	tIntro.wTag = 30465;
	tIntro.wSize = (WORD)nLength;
	for (i = 0; i < 3; i++)
	{
		pdwBits = tIntro.adwBits0[i];
		PlrIntroGetMetBits(((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->ptIntro[i], pdwBits);
		pdwBits = tIntro.adwBits1[i];
		PlrIntroGetIntroducedBits(((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->ptIntro[i], pdwBits);
	}
	memcpy(pbBuffer, &tIntro, nLength);
	return nLength;
}

int __fastcall PlrSaveWriteStats(UNIT *hUnit, BYTE *pbBuffer, int nSize)
{
	BYTE abBits[3];
	int anValues[16];
	int nValue, nCount, i;

	if ((DWORD)nSize < 67)
	{
#line 542
		D2FATAL("Out of memory in save buffer.\n");
	}
	*(WORD *)pbBuffer = 26215;
	pbBuffer += 2;
	memset(abBits, 0, 3);
	i = 0;
	nCount = 0;
	for (; i < 16; i++)
	{
		nValue = D2CommonStatsUnitGetBaseValue(hUnit, i);
		if (nValue)
		{
			abBits[i / 8] |= (BYTE)(1 << (i & 7));
			anValues[nCount] = nValue;
			nCount++;
		}
	}
	memcpy(pbBuffer, abBits, 3);
	memcpy(pbBuffer + 3, anValues, nCount * 4);
	return nCount * 4 + 5;
}

int __fastcall PlrSaveWriteSkills(UNIT *hUnit, BYTE *pbBuffer, int nSize)
{
	CLIENT *hClient;
	int nClass, nSkill;

	if (nSize < 32)
	{
#line 650
		D2FATAL("Out of memory in save buffer.\n");
	}
	*(WORD *)pbBuffer = 26217;
	pbBuffer += 2;
	hClient = SUnitGetClient(hUnit);
#line 659
	D2ASSERT(hClient);
	nClass = (int)ClientGetClass(hClient);
	for (nSkill = sganSkillFirst[nClass]; nSkill < sganSkillFirst[nClass] + sganSkillCount[nClass]; nSkill++)
		*pbBuffer++ = (BYTE)D2CommonSkillsCalcNodeLevel(hUnit, D2CommonSkillsFindByKey(hUnit, nSkill), 0);
	return sganSkillCount[nClass] + 2;
}

int __fastcall PlrSaveWriteItems(UNIT *hUnit, BYTE *pbBuffer, int nSize,
	BOOL fTrade)
{
	int nLength;
	void *pInventory;

	if (fTrade && hUnit->eType == UNIT_PLAYER)
	{
		nLength = PlrTradeCopyItems(hUnit, pbBuffer, nSize);
		if (nLength)
			return nLength;
	}
	pInventory = hUnit->ptInventory;
	return D2CommonItemSavePackList(pInventory, pbBuffer, nSize, fTrade);
}

int __fastcall PlrSaveWriteCorpses(GAME *ptGame, UNIT *hUnit, BYTE *pbBuffer,
	int nSize)
{
	void *pInventory;
	SAVECORPSE tCorpse;
	UNIT *hCorpse;
	void *ptNode;
	WORD wCount;
	int nResult;

	hCorpse = NULL;
	wCount = 0;
	pInventory = hUnit->ptInventory;
	ptNode = D2CommonINVENTORYGetTrackedListHead(pInventory);
	if (ptNode)
	{
		hCorpse = SUnitFind(ptGame, UNIT_PLAYER, D2CommonINVENTORYNodeGetTrackedKey(ptNode));
		if (hCorpse)
			wCount = 1;
	}
	*(WORD *)pbBuffer = SAVE_TAG_ITEMS;
	pbBuffer += 2;
	*(WORD *)pbBuffer = wCount;
	pbBuffer += 2;
	nResult = 4;
	nSize -= nResult;
	if (hCorpse)
	{
		tCorpse.nX = (int)D2CommonUnitGetX(hCorpse);
		tCorpse.nY = (int)D2CommonUnitGetY(hCorpse);
		if ((DWORD)nSize < sizeof(tCorpse))
		{
#line 734
			D2FATAL("Out of memory in save buffer.\n"); }
		nSize -= sizeof(tCorpse);
		*(SAVECORPSE *)pbBuffer = tCorpse;
		pbBuffer += sizeof(tCorpse);
		pInventory = hCorpse->ptInventory;
		nResult = D2CommonItemSavePackList(pInventory, pbBuffer, nSize, 0) + 4 + sizeof(tCorpse); }
	return nResult;
}

int __fastcall PlrSaveWriteCharacter(GAME *ptGame, UNIT *hUnit, const char *pszName,
	BYTE *pbBuffer, int nSize, BOOL fTrade)
{
	SAVEPET tPet;
	PETDATA *ptRecord;
	UNIT *hPet;
	int nLength, nTotal;

	nLength = PlrSaveWriteHeader(ptGame, hUnit, pszName, pbBuffer, nSize);
	if (nLength < 0)
		return 0;
	nSize -= nLength;
	pbBuffer += nLength;
	nTotal = nLength;
	nLength = PlrSaveWriteQuests(hUnit, pbBuffer, nSize);
	nSize -= nLength;
	pbBuffer += nLength;
	nTotal += nLength;
	nLength = PlrSaveWriteWaypoints(hUnit, pbBuffer, nSize);
	if (nLength < 0)
		return 0;
	nSize -= nLength;
	pbBuffer += nLength;
	nTotal += nLength;
	nLength = PlrSaveWriteIntro(hUnit, pbBuffer, nSize);
	if (nLength < 0)
		return 0;
	nSize -= nLength;
	pbBuffer += nLength;
	nTotal += nLength;
	nLength = PlrSaveWriteStats(hUnit, pbBuffer, nSize);
	if (nLength < 0)
		return 0;
	nSize -= nLength;
	pbBuffer += nLength;
	nTotal += nLength;
	nLength = PlrSaveWriteSkills(hUnit, pbBuffer, nSize);
	if (nLength < 0)
		return 0;
	nSize -= nLength;
	pbBuffer += nLength;
	nTotal += nLength;
	nLength = PlrSaveWriteItems(hUnit, pbBuffer, nSize, fTrade);
	if (nLength < 0)
		return 0;
	nSize -= nLength;
	pbBuffer += nLength;
	nTotal += nLength;
	nLength = PlrSaveWriteCorpses(ptGame, hUnit, pbBuffer, nSize);
	if (nLength < 0)
		return 0;
	nTotal += nLength;
	pbBuffer += nLength;
	hPet = PlayerPetsFind(ptGame, hUnit, 7);
	*(WORD *)pbBuffer = SAVE_TAG_ITEMS;
	pbBuffer += 2;
	tPet.dwSeed = 0;
	tPet.wHireId = 0;
	if (hPet)
	{
		ptRecord = PlayerPetsGetData(hUnit, hPet->dwId);
		tPet.wHireId = (WORD)ptRecord->dwHireId;
		tPet.dwSeed = ptRecord->dwSeed;
	}
	*(SAVEPET *)pbBuffer = tPet;
	return nTotal + 8;
}

void __fastcall PlrSaveToDatabase(GAME *ptGame, UNIT *hUnit, const char *pszName,
	const char *pszAccount, BOOL fTrade, DWORD dwTokenResult)
{
	BYTE abBuffer[SAVE_BUFFER_SIZE];
	int nLength;

	if (!gdwPlrSaveEnabled)
		return;
	nLength = PlrSaveWriteCharacter(ptGame, hUnit, pszName, abBuffer + 2, SAVE_BUFFER_SIZE, fTrade);
	if (nLength <= 0)
	{
#line 938
		D2FATAL("Unable to save player to stream.\n");
	}
	nLength += 2;
	*(WORD *)abBuffer = (WORD)nLength;
	if (ptEventCallbackTable)
	{
		if (ptEventCallbackTable->fpSaveDatabaseCharacter)
		{
#line 947
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)ptEventCallbackTable->fpSaveDatabaseCharacter),
				"ptEventCallbackTable->fpSaveDatabaseCharacter");
			((PFNSAVEDATABASECHARACTER)ptEventCallbackTable->fpSaveDatabaseCharacter)(pszName, pszAccount,
				abBuffer, nLength, dwTokenResult);
		}
	}
}

extern void __fastcall PlrSaveToFile(GAME *ptGame, UNIT *hUnit, const char *pszName, BYTE *pbBuffer, BOOL fTrade);
__inline void __fastcall PlrSaveToFile(GAME *ptGame, UNIT *hUnit, const char *pszName, BYTE *pbBuffer, BOOL fTrade)
{
	char szFile[500];
	char szPath[1024];
	FILE *pFile;
	int nLength;

	if (!gdwPlrSaveEnabled)
		return;
	FogSafesockGetSavePath(szPath, sizeof(szPath));
	sprintf(szFile, "%s%s.d2s", szPath, pszName);
	nLength = PlrSaveWriteCharacter(ptGame, hUnit, pszName, pbBuffer, SAVE_BUFFER_SIZE, fTrade);
	if (nLength <= 0)
		return;
	pFile = fopen(szFile, "wb");
	if (pFile)
	{
		fwrite(pbBuffer, nLength, 1, pFile);
		fclose(pFile);
		return;
	}
	FogErrorManagerLog("Unable to open player save file %s", szFile);
}

void __fastcall PlrSaveCharacter(GAME *ptGame, UNIT *hUnit, const char *pszName)
{
	char szAccount[16];
	BYTE abBuffer[SAVE_BUFFER_SIZE];
	CLIENT *ptClient;
	DWORD dwType, dwId, dwTokenResult;
	BOOL fTrade;
	int nLength;

	if (!hUnit)
		return;
	if (ArenaHasGuild(ptGame) && GuildsGetData(ptGame)
		&& !D2CommonGuildsGetEmblemChanged(GuildsGetData(ptGame)) && !(hUnit->dwFlags & 2))
		return;
	hUnit->dwFlags &= ~2;
	if (D2CommonStatsGetState(hUnit, 7))
		FogErrorManagerLog("[SAVE ERROR] Tried to save a dead body!\n");
	fTrade = FALSE;
	dwType = 6;
	if (SUnitGetInteract(hUnit, &dwType, &dwId) && dwType == UNIT_PLAYER)
		fTrade = TRUE;
	ptClient = SUnitGetClient(hUnit);
	ClientGetAccount(ptClient, szAccount);
	ClientGetTokenResult(ptClient, &dwTokenResult);
	if (ptGame->bGameType != 1 && ptGame->bGameType != 2)
	{
		if (ptEventCallbackTable)
		{
			PlrSaveToDatabase(ptGame, hUnit, pszName, szAccount, fTrade, dwTokenResult);
			return;
		}
		PlrSaveToFile(ptGame, hUnit, pszName, abBuffer, fTrade);
		return;
	}
	if (ClientGetBuffer(ptClient))
		ClientFreeBuffer(ptClient);
	nLength = PlrSaveWriteCharacter(ptGame, hUnit, pszName, abBuffer, SAVE_BUFFER_SIZE, fTrade);
#line 1003
	D2ASSERT(nLength > 0);
	DWORD dwClientId = ClientGetId(ptClient);
	ClientsAppendData(dwClientId, abBuffer, nLength, nLength);
	ClientSetDataSending(ptClient);
}

static void PlrSaveAttachUnit(GAME *ptGame, CLIENT *ptClient, UNIT *hUnit)
{
	PLRINFO *ptPlrInfo;

	ClientSetPlayerUnit(ptClient, hUnit);
	SUnitSetClient(hUnit, ptClient);
	D2CommonUnitSetPlayerName(hUnit, ClientGetName(ptClient));
	SUnitInitSeed(hUnit, (D2SEED *)&ptGame->ptAct[NUM_ACTS]);
	D2CommonUnitSetMode(hUnit, 1);
	ptPlrInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
	ptPlrInfo->hTrade = NULL;
	ptPlrInfo->dwTradeState = 0;
	ptPlrInfo->dwTradeEndTick = 0;
	ptPlrInfo->dwTradeMsgTick = 0;
}

int __fastcall PlrSaveReadHeader(GAME *ptGame, CLIENT *ptClient, const BYTE *pbData,
	UNIT **phUnit, BOOL *pfNew, DWORD *pdwVersion,
	DWORD *pdwField1e, DWORD *pdwField1c,
	DWORD *pdwLeftSkillId,
	DWORD *pdwRightSkillId, int *pnRead)
{
	SAVEHEADER tSaveHeader;
	char szGuildName[28];
	char szGuildTag[4];
	UNIT *hUnit;
	DWORD dwLevel;
	BYTE bAct, bDifficulty;
	int i, nClass;

	*pnRead = 0;
	memcpy(&tSaveHeader, pbData, sizeof(tSaveHeader));
	ClientSetFlags(ptClient, 0);
	if (tSaveHeader.dwMagic != SAVE_MAGIC)
	{
		if (!ptEventCallbackTable)
			goto lbOne;
		*pfNew = TRUE;
		return 0;
	}
	*pdwVersion = tSaveHeader.dwVersion;
	if (tSaveHeader.dwVersion != SAVE_VERSION)
	{
		if (!ptEventCallbackTable)
			goto lbOne;
		*pfNew = TRUE;
		return 0;
	}
	if (tSaveHeader.dwFlags & 4)
	{
		if (tSaveHeader.dwFlags & 8)
			return 21;
		if (!(ArenaGetFlags(ptGame) & 2048))
			return 20;
	}
	else if (ArenaGetFlags(ptGame) & 2048)
		return 19;
	dwLevel = (tSaveHeader.dwFlags >> 8) & 31;
	if ((int)dwLevel < 8 && ptGame->bDifficultyLevel > 1)
		return 18;
	if ((int)dwLevel < 4 && ptGame->bDifficultyLevel > 0)
		return 17;
	ClientSetGuildField(ptClient, tSaveHeader.wGuildFlags);
	memcpy(szGuildTag, &tSaveHeader.dwGuildTag, sizeof(szGuildTag));
	memcpy(szGuildName, tSaveHeader.szGuildName, sizeof(szGuildName));
	szGuildTag[3] = 0;
	szGuildName[sizeof(szGuildName) - 1] = 0;
	ClientSetGuildTag(ptClient, (const DWORD *)szGuildTag);
	ClientSetGuildName(ptClient, szGuildName);
	ClientSetGuildEmblem(ptClient, tSaveHeader.abGuildEmblem[0], tSaveHeader.abGuildEmblem[1],
		tSaveHeader.abGuildEmblem[2]);
	if (tSaveHeader.dwFlags & 1)
	{
		*pfNew = TRUE;
		tSaveHeader.dwFlags &= ~1;
		ClientSetFlags(ptClient, tSaveHeader.dwFlags);
		return 0;
	}
	*pfNew = FALSE;
	nClass = tSaveHeader.wClass;
	ClientSetClass(ptClient, nClass);
	ClientSetFlags(ptClient, tSaveHeader.dwFlags);
	bAct = tSaveHeader.bAct & 15;
	bDifficulty = tSaveHeader.bAct >> 4;
	if (bAct >= DIABLO2_NUMACTS || bDifficulty >= 3)
		goto lbNine;
	if (ptGame->bGameType == 3 && !ptGame->dwSeedFixed && ptGame->bDifficultyLevel == bDifficulty)
		ptGame->dwSeed = tSaveHeader.dwSeed;
	if (!ArenaHasGuild(ptGame))
	{
		if (bAct != 4 && ptGame->bDifficultyLevel == bDifficulty)
			ClientSetAct(ptClient, bAct);
		else
			ClientSetAct(ptClient, 0);
	}
	hUnit = SUnitCreate(UNIT_PLAYER, nClass, 0, 0, ptGame, NULL, 0, 0, 0);
	*phUnit = hUnit;
	PlrSaveAttachUnit(ptGame, ptClient, hUnit);
	SUnitMsgSendUnit(ClientGetGame(ptClient), *phUnit, ptClient);
	*pdwField1e = 16;
	*pdwField1c = 221;
	if (tSaveHeader.wField1e > 16 || tSaveHeader.wField1c > 221)
	{
		if (ptEventCallbackTable)
		{
			*pfNew = TRUE;
			return 0;
		}
		goto lbOne;
	}
	if (tSaveHeader.wField1e != 16)
		*pdwField1e = tSaveHeader.wField1e;
	if (tSaveHeader.wField1c != 221)
		*pdwField1c = tSaveHeader.wField1c;
	for (i = 0; i < 8; i++)
		ClientSetPair(ptClient, i, tSaveHeader.abHotkeys[i][0], tSaveHeader.abHotkeys[i][1]);
	*pdwLeftSkillId = tSaveHeader.bLeftSkillId;
	*pdwRightSkillId = tSaveHeader.bRightSkillId;
	*pnRead = sizeof(tSaveHeader);
	return 0;
lbOne:
	return 1;
lbNine:
	return 9;
}

int __fastcall PlrSaveReadStats(GAME *ptGame, UNIT *hUnit, const BYTE *pbData,
	int nStats, DWORD dwVersion, int *pnRead)
{
	const int *pnValue;
	int nCount, i;

	*pnRead = 0;
	if (*(const WORD *)pbData != 26215)
		return 4;
	pbData += 2;
	pnValue = (const int *)(pbData + 3);
	nCount = 0;
	for (i = 0; i < nStats; i++)
	{
		if (pbData[i / 8] & (1 << (i & 7)))
		{
			nCount++;
			D2CommonStatsUnitSetValue(hUnit, i, *pnValue++);
		}
		else
			D2CommonStatsUnitSetValue(hUnit, i, 0);
	}
	nCount = nCount * 4 + 5;
	if (ArenaTestNoActChange(ptGame))
	{
		D2CommonStatsUnitSetValue(hUnit, 6, D2CommonStatsUnitCalcMaxHp(hUnit));
		D2CommonStatsUnitSetValue(hUnit, 8, D2CommonStatsUnitCalcMaxMana(hUnit));
	}
	D2CommonStatsUnitSetValue(hUnit, 10, D2CommonStatsUnitCalcMaxStamina(hUnit));
	*pnRead = nCount;
	return 0;
}

void __fastcall PlrSaveAddSkill(GAME *ptGame, CLIENT *ptClient, UNIT *hUnit, int nSkill,
	BYTE bLevel)
{
	if (!bLevel)
		return;
	SkillsSetLevel(hUnit, nSkill, bLevel, 1);
	SCmdSendSkill(hUnit, (WORD)nSkill, bLevel, 1);
}

int __fastcall PlrSaveReadSkills(GAME *ptGame, CLIENT *ptClient, UNIT *hUnit,
	const BYTE *pbData, DWORD dwUnused, DWORD dwVersion,
	int *pnRead)
{
	int nClass, nSkill, nRead, i;
	BYTE bLevel;
	const BYTE *pbLevel;

	*pnRead = 0;
	if (*(const WORD *)pbData != 26217)
		return 5;
	nClass = (int)ClientGetClass(ptClient);
	pbLevel = pbData + 2;
	i = 0;
	for (nSkill = sganSkillFirst[nClass]; nSkill < sganSkillFirst[nClass] + sganSkillCount[nClass]; nSkill++)
	{
		bLevel = pbLevel[i++];
		if (bLevel)
		{
			SkillsSetLevel(hUnit, nSkill, bLevel, 1);
			SCmdSendSkill(hUnit, (WORD)nSkill, bLevel, 1);
		}
	}
	nRead = sganSkillCount[nClass] + 2;
	PlayerPetsUpdateMax(hUnit);
	*pnRead = nRead;
	return 0;
}

int __fastcall PlrSaveReadItem(GAME *ptGame, UNIT *hUnit, ITEMSAVE *ptItemSave,
	UNIT **phParent, int *pnAttached);
int __fastcall PlrSaveNewCharacter(GAME *ptGame, CLIENT *ptClient, UNIT **phUnit);

extern int __fastcall PlrSaveReadQuests(UNIT *hUnit, BYTE *pbData, int *pnRead);
__inline int __fastcall PlrSaveReadQuests(UNIT *hUnit, BYTE *pbData, int *pnRead)
{
	SAVEQUESTS tQuests;
	SAVEQUESTS *ptQuests;
	PLRINFO *ptPlrInfo;
	int i;

	*pnRead = 0;
	memcpy(&tQuests, pbData, sizeof(tQuests));
	ptQuests = &tQuests;
	if (ptQuests->dwTag != 560951127 || ptQuests->dwField04 != 6)
		return 2;
	ptPlrInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
	for (i = 0; i < 3; i++)
		D2CommonQuestRecordCopyAndSweep(ptPlrInfo->apQuestRecords[i], ptQuests->abQuests[i], 96, 1);
	*pnRead = sizeof(tQuests);
	return 0;
}

extern int __fastcall PlrSaveReadWaypoints(UNIT *hUnit, BYTE *pbData, int *pnRead);
__inline int __fastcall PlrSaveReadWaypoints(UNIT *hUnit, BYTE *pbData, int *pnRead)
{
	SAVEWAYPOINTS tWaypoints;
	SAVEWAYPOINTS *ptWaypoints;
	PLRINFO *ptPlrInfo;
	int i;

	*pnRead = 0;
	memcpy(&tWaypoints, pbData, sizeof(tWaypoints));
	ptWaypoints = &tWaypoints;
	if (ptWaypoints->wTag != 21335)
		return 3;
	ptPlrInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
	for (i = 0; i < 3; i++)
		(void)D2CommonWaypointCopyAndResetHistory(ptPlrInfo->apwWaypoints[i], ptWaypoints->abWaypoints[i]);
	*pnRead = sizeof(tWaypoints);
	return 0;
}

int __fastcall PlrSaveReadItems(GAME *ptGame, UNIT *hUnit, BYTE *pbData,
	DWORD dwVersion, int nSize, BOOL fCorpse,
	int *pnRead, ITEMSAVELIST *ptList)
{
	ITEMSAVE tItemSave;
	ITEMSAVE *ptListed;
	UNIT *hParent;
	DWORD dwItem;
	WORD wCount;
	int nAttached, nInnerAttached, nInnerAttached2, nRead, nResult, i, j;
	BOOL fSkip;

	if (dwVersion && dwVersion < SAVE_VERSION)
		return 1;
	if (*(WORD *)pbData != SAVE_TAG_ITEMS)
		return 14;
	pbData += 2;
	wCount = *(WORD *)pbData;
	pbData += 2;
	nRead = 4;
	nSize -= 4;
	for (dwItem = 0; dwItem < (DWORD)wCount; dwItem++)
	{
		memset(&tItemSave, 0, sizeof(tItemSave));
		if (*(WORD *)pbData != SAVE_TAG_ITEMS)
			return 14;
		pbData += 2;
		D2CommonItemSaveUnpack(&tItemSave, pbData);
		pbData += 25;
		nSize -= 27;
		nRead += 27;
		nAttached = 0;
		hParent = NULL;
		fSkip = FALSE;
		if (fCorpse)
		{
			nResult = PlrSaveReadItem(ptGame, hUnit, &tItemSave, &hParent, &nAttached);
			if (nResult)
				return nResult;
		}
		else
		{
			if (ptList)
			{
				for (j = 0; j < ptList->nCount; j++)
				{
					ptListed = &ptList->atItem[j];
					if (tItemSave.bMode != ptListed->bMode)
						continue;
					switch (tItemSave.bMode)
					{
					case 0:
						if (tItemSave.dwX == ptListed->dwX
							&& tItemSave.dwY == ptListed->dwY
							&& tItemSave.bPage == ptListed->bPage
							&& tItemSave.bLocation == ptListed->bLocation)
							goto listed;
						break;
					case 2:
						if (tItemSave.dwX == ptListed->dwX
							&& tItemSave.bPage == ptListed->bPage
							&& tItemSave.bLocation == ptListed->bLocation)
							goto listed;
						break;
					default:
						if (tItemSave.bPage == ptListed->bPage
							&& tItemSave.bLocation == ptListed->bLocation)
							goto listed;
						break;
					}
				}
				fSkip = TRUE;
				nAttached = tItemSave.bNumItemsAttached;
				goto attached;
			}
listed:
			nResult = PlrMsgLoadItem(ptGame, hUnit, &tItemSave, &hParent, &nAttached);
			if (nResult)
				return nResult;
		}
attached:
		for (i = 0; i < nAttached; i++)
		{
			memset(&tItemSave, 0, sizeof(tItemSave));
			if (*(WORD *)pbData != SAVE_TAG_ITEMS)
				return 14;
			pbData += 2;
			D2CommonItemSaveUnpack(&tItemSave, pbData);
			pbData += 25;
			nSize -= 27;
			nRead += 27;
			if (fCorpse)
			{
				nResult = PlrSaveReadItem(ptGame, hUnit, &tItemSave, &hParent, &nInnerAttached);
				if (nResult)
					return nResult;
			}
			else if (!fSkip)
			{
				nResult = PlrMsgLoadItem(ptGame, hUnit, &tItemSave, &hParent, &nInnerAttached2);
				if (nResult)
					return nResult;
			}
		}
	}
	*pnRead = nRead;
	return 0;
}

int __fastcall PlrSaveReadItem(GAME *ptGame, UNIT *hUnit, ITEMSAVE *ptItemSave,
	UNIT **phParent, int *pnAttached)
{
	UNIT *hItem;

	hItem = PlrSaveRecreateItem(ptGame, ptItemSave);
	if (!hItem)
		return 13;
	if (hItem->dwMode)
	{
		int nResult;

		if (hItem->dwMode != 6)
			return 13;
		if (!*phParent)
			return 13;
		D2CommonUnitSetMode(hItem, 4);
		if (!ITEMSModeSocketGem(ptGame, hUnit, hItem->dwId, (*phParent)->dwId, &nResult, FALSE, TRUE,
			FALSE))
			return 13;
	}
	else
	{
		DWORD dwX, dwY;

		D2CommonUnitSetMode(hItem, 4);
		dwX = ptItemSave->dwX;
		dwY = ptItemSave->dwY;
		D2CommonUnitSetX(hItem, dwX);
		D2CommonUnitSetY(hItem, dwY);
		if (!ITEMSModePlaceCursorItem(ptGame, hUnit, hItem->dwId, (int)D2CommonUnitGetX(hItem),
			(int)D2CommonUnitGetY(hItem), FALSE, FALSE, NULL))
			return 13;
	}
	if (ptItemSave->bNumItemsAttached > 0)
		*phParent = hItem;
	*pnAttached = ptItemSave->bNumItemsAttached;
	return 0;
}

int __fastcall PlrSaveReadCorpses(GAME *ptGame, UNIT *hUnit, BYTE *pbData,
	DWORD dwVersion, int nSize, int *pnRead)
{
	CLIENT *ptClient;
	UNIT *hCorpse;
	DWORD dwCorpse;
	WORD wCount;
	int nRead, nItemsRead, nLength, nResult;

	*pnRead = 0;
	if (dwVersion && dwVersion < SAVE_VERSION)
		return 1;
	if (*(WORD *)pbData != SAVE_TAG_ITEMS)
		return 14;
	pbData += 2;
	wCount = *(WORD *)pbData;
	pbData += 2;
	nSize -= 4;
	nRead = 4;
	dwCorpse = 0;
	if (dwCorpse < (DWORD)wCount)
	while (1)
	{
		hCorpse = SUnitCreate(UNIT_PLAYER, hUnit->dwClassId, 0, 0, ptGame, NULL, 1, 17, 0);
		if (!hCorpse)
			return 8;
		hCorpse->dwFlags0 &= ~4;
		ptClient = SUnitGetClient(hUnit);
		SUnitSetClient(hCorpse, ptClient);
		D2CommonUnitSetPlayerName(hCorpse, ClientGetName(ptClient));
		SUnitInitSeed(hCorpse, (D2SEED *)&ptGame->ptAct[NUM_ACTS]);
		D2CommonStatsUnitSetActiveStateBit(hCorpse, 7, 1);
		hCorpse->dwFlags0 &= ~16;
		D2CommonINVENTORYSetOwnerId(hCorpse->ptInventory, hUnit->dwId);
		nResult = PlrSaveReadItems(ptGame, hCorpse, pbData + 12, dwVersion, nSize - 12, TRUE, &nItemsRead,
			NULL);
		if (nResult)
			return nResult;
		nLength = nItemsRead + 12;
		D2CommonINVENTORYAppendTrackedNode(hUnit->ptInventory, hCorpse->dwId, 0, 1);
		SCmdSendAllCorpse(ptGame, hUnit->dwId, hCorpse->dwId, 1);
		pbData += nLength;
		nSize -= nLength;
		nRead += nLength;
		dwCorpse++;
		if (dwCorpse >= (DWORD)wCount)
			break;
	}
	*pnRead = nRead;
	return 0;
}

static __inline int PlrSaveReadIntro(UNIT *hUnit, BYTE *pbData, int *pnRead)
{
	SAVEINTRO tIntro;
	int i;

	*pnRead = 0;
	memcpy(&tIntro, pbData, sizeof(tIntro));
	if (tIntro.wTag != 30465)
		return 11;
	for (i = 0; i < 3; i++)
	{
		PlrIntroSetMetBits(((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->ptIntro[i], tIntro.adwBits0[i]);
		PlrIntroSetIntroducedBits(((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->ptIntro[i], tIntro.adwBits1[i]);
	}
	*pnRead = sizeof(tIntro);
	return 0;
}

static __inline int PlrSaveReadHireling(UNIT *hUnit, BYTE *pbData, int *pnRead)
{
	*pnRead = 0;
	if (*(WORD *)pbData != SAVE_TAG_ITEMS)
		return 14;
	D2CommonINVENTORYAppendTrackedNode(hUnit->ptInventory, *(DWORD *)(pbData + 2), *(WORD *)(pbData + 6), 0);
	*pnRead = 8;
	return 0;
}

int __fastcall PlrSaveReadCharacter(GAME *ptGame, CLIENT *ptClient, BYTE *pbData,
	int nSize, UNIT **phUnit)
{
	PLRINFO *ptPlrInfo;
	UNIT *hUnit;
	DWORD dwVersion, dwField1e, dwUnused, dwLeftSkillId, dwRightSkillId;
	BOOL fNew;
	int nRead, nResult, nLife, nMana;

	hUnit = NULL;
	*phUnit = NULL;
	nResult = PlrSaveReadHeader(ptGame, ptClient, pbData, &hUnit, &fNew, &dwVersion, &dwField1e, &dwUnused,
		&dwLeftSkillId, &dwRightSkillId, &nRead);
	if (nResult)
		return nResult;
	if (fNew)
		return PlrSaveNewCharacter(ptGame, ptClient, phUnit);
	nSize -= nRead;
	pbData += nRead;
	*phUnit = hUnit;
	nResult = PlrSaveReadQuests(hUnit, pbData, &nRead);
	if (nResult)
		return nResult;
	nSize -= nRead;
	pbData += nRead;
	nResult = PlrSaveReadWaypoints(hUnit, pbData, &nRead);
	if (nResult)
		return nResult;
	nSize -= nRead;
	pbData += nRead;
	nResult = PlrSaveReadIntro(hUnit, pbData, &nRead);
	if (nResult)
		return nResult;
	nSize -= nRead;
	pbData += nRead;
	nResult = PlrSaveReadStats(ptGame, hUnit, pbData, 16, dwVersion, &nRead);
	if (nResult)
		return nResult;
	nSize -= nRead;
	pbData += nRead;
	nLife = D2CommonStatsUnitGetValue(hUnit, 6);
	nMana = D2CommonStatsUnitGetValue(hUnit, 8);
	nResult = PlrSaveReadSkills(ptGame, ptClient, hUnit, pbData, dwUnused, dwVersion, &nRead);
	if (nResult)
		return nResult;
	nSize -= nRead;
	pbData += nRead;
	if (dwLeftSkillId)
		SkillsSetCurrent(hUnit, TRUE, (int)dwLeftSkillId);
	if (dwRightSkillId)
		SkillsSetCurrent(hUnit, FALSE, (int)dwRightSkillId);
	ptPlrInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
	ptPlrInfo->dwLeftSkillId = dwLeftSkillId;
	ptPlrInfo->dwRightSkillId = dwRightSkillId;
	nResult = PlrSaveReadItems(ptGame, hUnit, pbData, dwVersion, nSize, FALSE, &nRead, NULL);
	if (nResult)
		return nResult;
	nSize -= nRead;
	pbData += nRead;
	nResult = PlrSaveReadCorpses(ptGame, hUnit, pbData, dwVersion, nSize, &nRead);
	if (nResult)
		return nResult;
	nResult = PlrSaveReadHireling(hUnit, pbData + nRead, &nRead);
	if (nResult)
		return nResult;
	D2CommonStatsUnitSetValue(hUnit, 6, nLife);
	D2CommonStatsUnitSetValue(hUnit, 8, nMana);
	D2CommonStatsUnitSetValue(hUnit, 68, 100);
	D2CommonStatsUnitSetValue(hUnit, 67, 75);
	D2CommonStatsUnitSetValue(hUnit, 69, 100);
	D2CommonStatsUnitSetValue(hUnit, 30,
		D2CommonDataTblsGetExperience(hUnit->dwClassId, D2CommonStatsUnitGetBaseValue(hUnit, 12)));
	return 0;
}

int __fastcall PlrSaveNewCharacter(GAME *ptGame, CLIENT *ptClient, UNIT **phUnit)
{
	PLRINFO *ptPlrInfo;
	UNIT *hUnit;
	BYTE bAct;

	DWORD dwClass = ClientGetClass(ptClient);
	hUnit = SUnitCreate(UNIT_PLAYER, (int)dwClass, 0, 0, ptGame, NULL, 0, 0, 0);
	PlrSaveAttachUnit(ptGame, ptClient, hUnit);
	SUnitMsgSendUnit(ptGame, hUnit, ptClient);
	bAct = ClientGetAct(ptClient);
	PlrEvalInit(ptGame, hUnit, bAct);
	PlayerAddStartItems(hUnit, ptGame, bAct);
	ptPlrInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
	ptPlrInfo->dwLeftSkillId = 0;
	ptPlrInfo->dwRightSkillId = 0;
	if (!bAct)
	{
		switch (hUnit->dwClassId)
		{
		case 1:
			SkillsSetCurrent(hUnit, FALSE, SKILL_SORCERESS_START);
			ptPlrInfo->dwRightSkillId = 36;
			break;
		case 2:
			SkillsSetCurrent(hUnit, FALSE, SKILL_RAISESKELETON);
			ptPlrInfo->dwRightSkillId = 70;
			break;
		}
	}
	QuestsPlayerStarted(ptGame, hUnit, TRUE);
	*phUnit = hUnit;
	return 0;
}

int __fastcall PlrSaveLoadFile(GAME *ptGame, CLIENT *ptClient, const char *pszName,
	UNIT **phUnit)
{
	char szFile[500];
	char szPath[1024];
	BYTE abBuffer[SAVE_BUFFER_SIZE];
	UNIT *hUnit;
	FILE *pFile;
	DWORD dwRead;
	int nResult;

	nResult = 0;
	hUnit = NULL;
	*phUnit = NULL;
	FogSafesockGetSavePath(szPath, sizeof(szPath));
	sprintf(szFile, "%s%s.d2s", szPath, pszName);
	pFile = fopen(szFile, "rb");
	if (pFile)
	{
		dwRead = fread(abBuffer, 1, SAVE_BUFFER_SIZE, pFile);
		fclose(pFile);
		if (dwRead)
		{
			nResult = PlrSaveReadCharacter(ptGame, ptClient, abBuffer, (int)dwRead, &hUnit);
			if (nResult != 0)
				return nResult;
		}
	}
	*phUnit = hUnit;
	if (hUnit == NULL)
		nResult = 14;
	return nResult;
}

static void PlrSaveTemplateCharacter(GAME *ptGame, CLIENT *ptClient, UNIT **phUnit)
{
	CHARTEMPLATETXT *ptTemplate;
	UNIT *hUnit;
	DWORD dwExperience, dwExperiencePrev;
	int nTemplate, i;

	nTemplate = ClientGetCharTemplate(ptClient);
	ptTemplate = (CHARTEMPLATETXT *)D2CommonArenaGetCharTemplate(nTemplate, ArenaGetType(ptGame));
	hUnit = SUnitCreate(UNIT_PLAYER, ptTemplate->nClassId, 0, 0, ptGame, NULL, 0, 0, 0);
	PlrSaveAttachUnit(ptGame, ptClient, hUnit);
	SUnitMsgSendUnit(ptGame, hUnit, ptClient);
	PlrEvalInit(ptGame, hUnit, 0);
	D2CommonStatsUnitSetValue(hUnit, 0, ptTemplate->str);
	D2CommonStatsUnitSetValue(hUnit, 1, ptTemplate->Int);
	D2CommonStatsUnitSetValue(hUnit, 2, ptTemplate->dex);
	D2CommonStatsUnitSetValue(hUnit, 3, ptTemplate->vit);
	dwExperience = D2CommonDataTblsGetExperience(ptTemplate->nClassId, ptTemplate->level);
	dwExperiencePrev = D2CommonDataTblsGetExperience(ptTemplate->nClassId, ptTemplate->level - 1);
	D2CommonStatsUnitSetValue(hUnit, 30, dwExperience);
	D2CommonStatsUnitSetValue(hUnit, 13, dwExperiencePrev);
	D2CommonStatsUnitSetValue(hUnit, 12, ptTemplate->level);
	D2CommonStatsUnitSetValue(hUnit, 9, ptTemplate->Mana << 8);
	D2CommonStatsUnitSetValue(hUnit, 8, ptTemplate->Mana << 8);
	D2CommonStatsUnitSetValue(hUnit, 7, ptTemplate->Hitpoints << 8);
	D2CommonStatsUnitSetValue(hUnit, 6, ptTemplate->Hitpoints << 8);
	D2CommonStatsUnitSetValue(hUnit, 27, ptTemplate->ManaRegenBonus);
	D2CommonStatsUnitSetValue(hUnit, 67, ptTemplate->Velocity);
	D2CommonStatsUnitSetValue(hUnit, 68, ptTemplate->AttackRate);
	D2CommonStatsUnitSetValue(hUnit, 69, ptTemplate->OtherRate);
	for (i = 0; i < 9; i++)
	{
		if (ptTemplate->Skill[i] && ptTemplate->SkillLevel[i])
			PlrSaveAddSkill(ptGame, ptClient, hUnit, ptTemplate->Skill[i],
				(BYTE)ptTemplate->SkillLevel[i]);
	}
	if (ptTemplate->RightSkill)
		SkillsSetCurrent(hUnit, FALSE, ptTemplate->Skill[ptTemplate->RightSkill - 1]);
	for (i = 0; i < 15; i++)
		PlayerAddStartItem(ptGame, hUnit, &ptTemplate->tItem[i]);
	QuestsPlayerStarted(ptGame, hUnit, TRUE);
	*phUnit = hUnit;
}

static int PlrSaveReadFromClient(GAME *ptGame, CLIENT *ptClient, UNIT **phUnit)
{
	BYTE *pbBuffer;
	int nUsed, nResult;

	*phUnit = NULL;
	pbBuffer = ClientGetBuffer(ptClient);
	nUsed = ClientGetBufferUsed(ptClient);
	if (!nUsed)
		return 14;
	nResult = PlrSaveReadCharacter(ptGame, ptClient, pbBuffer, nUsed, phUnit);
	if (*phUnit)
	{
		ClientFreeBuffer(ptClient);
		QuestsPlayerStarted(ptGame, *phUnit, FALSE);
	}
	return nResult;
}

extern int PlrSaveReadFromDisk(GAME *ptGame, CLIENT *ptClient, char *szName, UNIT **phUnit);
__inline int PlrSaveReadFromDisk(GAME *ptGame, CLIENT *ptClient, char *szName, UNIT **phUnit)
{
	UNIT *hUnit;
	int nResult;

	hUnit = NULL;
	nResult = PlrSaveLoadFile(ptGame, ptClient, szName, &hUnit);
	if (hUnit)
		QuestsPlayerStarted(ptGame, hUnit, FALSE);
	*phUnit = hUnit;
	return nResult;
}

int __fastcall PlrSaveLoadCharacter(GAME *ptGame, CLIENT *ptClient, char *szName, DWORD dwArg,
	UNIT **phUnit)
{
	int nResult;

	nResult = 0;
	if (ArenaTestCharTemplate(ptGame))
	{
		PlrSaveTemplateCharacter(ptGame, ptClient, phUnit);
	}
	else
	{
		if (ptGame->bGameType == 1 || ptGame->bGameType == 2 || ptEventCallbackTable)
		{
			nResult = PlrSaveReadFromClient(ptGame, ptClient, phUnit);
			if (nResult)
				return nResult;
		}
		else
		{
			if (dwArg)
				nResult = PlrSaveReadFromDisk(ptGame, ptClient, szName, phUnit);
			if (!*phUnit)
				nResult = PlrSaveNewCharacter(ptGame, ptClient, phUnit);
		}
#line 2491
		D2ASSERT(*phUnit);
	}
	PlayerPetsUpdateMax(*phUnit);
	return nResult;
}
