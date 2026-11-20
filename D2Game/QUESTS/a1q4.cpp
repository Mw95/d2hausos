#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\QUESTS\\a1q4.cpp"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "../GAME/SCmd.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/MonsterAI.h"
#include "../MONSTER/MonsterMode.h"
#include "../MONSTER/MonsterUnique.h"
#include "../OBJECTS/ObjMode.h"
#include "QUESTS.h"
#include "a1q4.h"
#include "../UNIT/Party.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitInactive.h"
#include "../SKILLS/Skills.h"

#define A1Q4_CODE_SCROLL	544435042
#define A1Q4_CODE_DECIPHERED	543452002
#define A1Q4_CODE_RING		544106866
#define A1Q4_CODE_POTION	544436342

#pragma pack(push, 1)
struct A1Q4ORDERCMD
{
 	BYTE		bCmd;
 	WORD		wQuest;
 	WORD		wOrder[5];
 	BYTE		unobserved_0d[2];
};
struct A1Q4RELEASECMD
{
 	BYTE		bCmd;
 	BYTE		bQuest;
 	BYTE		bStatusFlags;
 	BYTE		unobserved_03[2];
};
#pragma pack(pop)
typedef struct A1Q4ORDERCMD A1Q4ORDERCMD;
typedef struct A1Q4RELEASECMD A1Q4RELEASECMD;

static char sgszStoneText[88];

static int sgnA1Q4NpcTable[8] = { -1, 0, 1, 2, 3, 4, 5, 6 };
static QUESTNPCTABLE sgtA1Q4Npcs[11] =
{
	{ { { 148, 97, { 0, 0 }, 1 } }, 1 },
	{ { { 148, 99, { 0, 0 }, 2 }, { 150, 98, { 0, 0 }, 2 }, { 154, 100, { 0, 0 }, 2 },
		{ 147, 102, { 0, 0 }, 2 }, { 155, 101, { 0, 0 }, 2 } }, 5 },
	{ { { 150, 105, { 0, 0 }, 2 }, { 155, 107, { 0, 0 }, 2 }, { 154, 103, { 0, 0 }, 2 },
		{ 148, 104, { 0, 0 }, 2 }, { 147, 106, { 0, 0 }, 2 } }, 5 },
	{ { { 150, 108, { 0, 0 }, 2 }, { 155, 111, { 0, 0 }, 2 }, { 154, 109, { 0, 0 }, 2 },
		{ 148, 112, { 0, 0 }, 0 }, { 147, 110, { 0, 0 }, 2 } }, 5 },
	{ { { 150, 113, { 0, 0 }, 2 }, { 155, 116, { 0, 0 }, 2 }, { 154, 114, { 0, 0 }, 2 },
		{ 148, 117, { 0, 0 }, 2 }, { 147, 115, { 0, 0 }, 2 } }, 5 },
	{ { { 150, 119, { 0, 0 }, 2 }, { 155, 122, { 0, 0 }, 2 }, { 154, 121, { 0, 0 }, 2 },
		{ 265, 123, { 0, 0 }, 1 }, { 148, 118, { 0, 0 }, 1 }, { 147, 120, { 0, 0 }, 2 } }, 6 },
	{ { { 265, 125, { 0, 0 }, 1 } }, 1 },
	{ { { 265, 123, { 0, 0 }, 2 }, { 148, 118, { 0, 0 }, 2 }, { 147, 120, { 0, 0 }, 2 } }, 3 },
	{ { { 265, 125, { 0, 0 }, 2 } }, 1 },
	{ { { 146, 124, { 0, 0 }, 1 } }, 1 },
	{ { { 410, 0, { 0, 0 }, 2 } }, 0 },
};

void __fastcall A1Q4ThreatDropped(UNIT *hUnit);
void __fastcall A1Q4DrawOrder(QUEST *pQuest);
void __fastcall A1Q4PartyCredit(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A1Q4SendStatus(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A1Q4CreditInLevel(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A1Q4FlagPlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
BOOL __fastcall A1Q4FindInLevel(GAME *ptGame, UNIT *hUnit, void *pArg);
BYTE __fastcall A1Q4StoneTimer(GAME *ptGame, QUEST *pQuest);
BOOL __fastcall A1Q4UpdatePlayer(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A1Q4PlayerDropped(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q4ResetTree(QUEST *pQuest);
void __fastcall A1Q4Activate(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q4NpcMessage(QUEST *pQuestInfo, QUESTPARAM *pqParam);
void __fastcall A1Q4Talk(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q4PlayerJoined(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q4PlayerLeft(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q4ChangedLevel(QUEST *pQuest, QUESTPARAM *pqParam);
BOOL __fastcall A1Q4FlagStarted(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall A1Q4ItemAcquired(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q4ResetUnlessStonesDone(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall A1Q4Sequence(QUEST *pQuestInfo);
void __fastcall A1Q4PlayerStarted(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall A1Q4UnitKilled(QUEST *pQuest, QUESTPARAM *pqParam);

static __inline void A1Q4QuestStarted(QUEST *pQuest)
{
#line 579
	QuestsDebugMessage(pQuest->ptGame, "Started quest", D2_THIS_FILE, __LINE__);
}

BOOL __fastcall A1Q4FreeCain(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	MONMODECHANGE tModeChange;
	QUEST *pQuest;

	if (!(hUnit->dwClassId != 146))
	{
		if (MonsterAIHasThreats(((MONSTERDATA *)hUnit->uTypeData)->pptThreats))
		{
			MonsterAIClearThreats(ptGame, hUnit, A1Q4ThreatDropped);
			return TRUE;
		}
		MonsterModeInit(hUnit, 0, &tModeChange);
		MonsterModeSet(ptGame, &tModeChange, TRUE);
		pQuest = QuestsFind(ptGame, 4);
		if (pQuest)
			((A1Q4RECORD *)pQuest->pQuestRecord)->bCainFreed = 1;
		return TRUE;
	}
	return FALSE;
}

void __fastcall A1Q4ThreatDropped(UNIT *hUnit  )
{
	A1Q4RELEASECMD tCmd;

	tCmd.bCmd = 86;
	tCmd.bStatusFlags = 1;
	tCmd.bQuest = 4;
	SCmdSendQuestStatus(SUnitGetClient(hUnit), &tCmd);
}

void __fastcall A1Q4DrawOrder(QUEST *pQuest  )
{
	int anClass[5];
	A1Q4RECORD *pQuestRecord;
	D2SEED *ptSeed;
	int *pnClass;
	int nSlot;
	int nDrawn;

	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	anClass[0] = 17;
	anClass[1] = 18;
	anClass[2] = 19;
	anClass[3] = 20;
	anClass[4] = 21;
	memset(pQuestRecord->awOrder, 0, sizeof(pQuestRecord->awOrder));
	nDrawn = 0;
	pQuestRecord->wNumStonesTouched = 0;
	ptSeed = QuestsGetSeed(pQuest->ptGame);
	pnClass = anClass;
	do
	{
		nSlot = D2CommonSeedBetween(ptSeed, 0, 5);
		if (!pQuestRecord->awOrder[nSlot])
		{
			wsprintfA(sgszStoneText, "stone %d is class %d", nSlot, *pnClass);
#line 1298
			QuestsDebugMessage(pQuest->ptGame, sgszStoneText, D2_THIS_FILE, __LINE__);
			pQuestRecord->awOrder[nSlot] = (WORD)*pnClass;
			nDrawn++;
			pnClass++;
		}
	} while (nDrawn < 5);
}

void __fastcall A1Q4UnitAttached(GAME *ptGame  , QUEST *pQuest
, UNIT *hUnit  )
{
	A1Q4RECORD *pQuestRecord;

	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	pQuestRecord->bInvisibleObjectPresent = 1;
	pQuestRecord->dwInvisibleObjectId = hUnit->dwId;
}

BYTE __fastcall A1Q4Active(QUEST *pQuest  , int nNpcClass,
	UNIT *hPlayer  , void *pPlayerRecord)
{
	A1Q4RECORD *pQuestRecord;
	DWORD dwCredited;
	DWORD dwDone;
	DWORD dwStarted;

	dwCredited = D2CommonStepTestGridBit(pPlayerRecord, 4, 13);
	dwDone = D2CommonStepTestGridBit(pPlayerRecord, 4, 0);
	dwStarted = D2CommonStepTestGridBit(pPlayerRecord, 4, 1);
	if (nNpcClass == 148)
	{
		if (pQuest->bState == 4 && !dwDone && !dwStarted && ITEMSFindByType(hPlayer, A1Q4_CODE_SCROLL))
			return TRUE;
		if (pQuest->bState == 1 && !dwDone && !dwStarted)
			return TRUE;
		if (pQuest->bState == 6 && dwCredited == 1 && !dwDone)
			return TRUE;
		if (dwStarted)
			return TRUE;

	}
	if (nNpcClass == 265)
	{
		pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
		if (QuestsPlayersFind(&pQuestRecord->tPlayers1, hPlayer->dwId) == 1)
			return TRUE;
		if (QuestsPlayersFind(&pQuestRecord->tPlayers2, hPlayer->dwId))
			return FALSE;
		if (dwCredited)
			return TRUE;
	}
	return FALSE;
}

void __fastcall A1Q4PartyCredit(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	int nLevelId;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 4, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, 4, 1) == 1)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return;
	nLevelId = (int)D2CommonDungeonRoomGetLevelId(hRoom);
	if (!nLevelId)
		return;
	if (D2CommonGetActNum(nLevelId))
		return;
	D2CommonQuestRecordSetBit(pRecord, 4, 13);
	D2CommonQuestRecordSetBit(pRecord, 4, 1);
}

BYTE __fastcall A1Q4CainTimer(GAME *ptGame  , QUEST *pQuest
)
{
	SUnitIterate(pQuest->ptGame, 1, NULL, A1Q4FreeCain);
	((A1Q4RECORD *)pQuest->pQuestRecord)->bCainTimerPending = 0;
	return TRUE;
}

void __fastcall A1Q4ObjectEvent(GAME *ptGame  , UNIT *hObject)
{
	QUEST *pQuest;
	A1Q4RECORD *pQuestRecord;
	GAMEROOM *hRoom;
	GAMEROOM *hSpotRoom;
	GAMECOORD tSpot;
	UNIT *hMonster;
	UNIT *hPlayer;
	int nX;
	int nY;

	pQuest = QuestsFind(ptGame, 4);
	if (pQuest)
	{
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	if (!pQuest->bActive)
		return;
	if (pQuestRecord->bQuestDone == 1)
		return;
	pQuestRecord->dwGibbetMode = 3;
	D2CommonUnitSetMode(hObject, 3);
	nX = (int)D2CommonUnitGetX(hObject) + 3;
	nY = (int)D2CommonUnitGetY(hObject) + 3;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hObject);
	hMonster = MonsterSpawn(ptGame, hRoom, nX, nY, 146, 1, -1, 0);
	if (!hMonster)
	{
		tSpot.dwX = (DWORD)nX;
		tSpot.dwY = (DWORD)nY;
		QuestsFindSpot(hRoom, &tSpot, 2, 256, &hSpotRoom);
		if (hSpotRoom)
			hMonster = MonsterSpawn(ptGame, hSpotRoom, (int)tSpot.dwX, (int)tSpot.dwY, 146, 1, -1, 0);
	}
	if (hMonster)
	{
		hMonster->dwFlags0 |= 50331648;
		hPlayer = SUnitFind(ptGame, 0, pQuestRecord->dwGibbetPlayerId);
		if (hPlayer)
			SUnitSetEvent(hPlayer, 46, NULL);
	}
	else
	{
#line 1538
		QuestsDebugMessage(ptGame, "Cain not created, look in town, dammit\n", D2_THIS_FILE, __LINE__);
		pQuestRecord->hTristramPlayer = NULL;
		SUnitIterate(ptGame, 0, hObject, A1Q4FindInLevel);
		if (pQuestRecord->hTristramPlayer && !pQuestRecord->bTownPortalOpened)
		{
			if (SkillsOpenPortal(ptGame, pQuestRecord->hTristramPlayer, hRoom, nX + 3, nY + 3,
				LEVEL_ACT_1_TOWN, 0, 59, 0))
				pQuestRecord->bTownPortalOpened = 1;
		}
		if (!pQuestRecord->bTownCainMade)
			pQuestRecord->bTownCainWanted = 1;
		pQuestRecord->bCainNotCreated = 1;
	}
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 6, NULL, A1Q4SendStatus, TRUE);
	SUnitIterate(ptGame, 0, hObject, A1Q4CreditInLevel);
	SUnitIterate(ptGame, 0, hObject, A1Q4FlagPlayer);
		return;
	}
#line 1498
	D2WARN(0);
}

BOOL __fastcall A1Q4SendStatus(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if ((D2CommonStepTestGridBit(pRecord, 4, 0) || D2CommonStepTestGridBit(pRecord, 4, 15))
		&& !D2CommonStepTestGridBit(pRecord, 4, 13) && !D2CommonStepTestGridBit(pRecord, 4, 14))
		return FALSE;
	QuestsSendStatus(ptGame, hUnit, 4);
	return FALSE;
}

BOOL __fastcall A1Q4CreditInLevel(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	GAMEROOM *hRoom;
	WORD wParty;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 4, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 4, 15) == 1)
		return FALSE;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	if (D2CommonDungeonRoomGetLevelId(hRoom) != 38)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 4, 13);
	D2CommonQuestRecordSetBit(pRecord, 4, 1);
	wParty = SUnitGetParty(hUnit);
	if (wParty != 65535)
		PartyForEach(ptGame, wParty, A1Q4PartyCredit, NULL);
	return FALSE;
}

BOOL __fastcall A1Q4FlagPlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 4, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 4, 1) == 1)
		return FALSE;
	D2CommonQuestRecordSetBit(pRecord, 4, 14);
	QuestsSendFlagInAct(hUnit, 4, 0);
	return FALSE;
}

BOOL __fastcall A1Q4FindInLevel(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	GAMEROOM *hRoom;
	QUEST *pQuest;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	if (hRoom) {
		if (D2CommonDungeonRoomGetLevelId(hRoom) != 38)
			return FALSE;
		pQuest = QuestsFind(ptGame, 4);
		if (!pQuest)
			return FALSE;
		((A1Q4RECORD *)pQuest->pQuestRecord)->hTristramPlayer = hUnit;
		return TRUE; }
	return FALSE;
}

int __fastcall A1Q4OperateObject(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuest;
	A1Q4RECORD *pQuestRecord;
	MISSOBJECTSTXT *ptObjectTxt;

	pQuest = QuestsFind(pObjectParam->ptGame, 4);
	pQuestRecord = NULL;
	if (pQuest)
	{
		pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
		if (!pQuest->bActive || pQuestRecord->bQuestDone == 1 || pQuest->bState >= 6)
			return 0;
	}
	ptObjectTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(pObjectParam->dwClassId);
	if (!pObjectParam->hObject->dwMode)
	{
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 1,
			pObjectParam->ptGame->nFrame + ptObjectTxt->FrameCnt[1]);
		if (pQuestRecord)
		{
			pQuestRecord->dwGibbetMode = 3;
			pQuestRecord->dwGibbetPlayerId = pObjectParam->hPlayer->dwId;
		}
		EventAdd(pObjectParam->ptGame, pObjectParam->hObject, 7, pObjectParam->ptGame->nFrame + 17);
	}
	return 0;
}

void __fastcall A1Q4StoneInit(OBJINIT *ptInit  )
{
	QUEST *pQuest;
	A1Q4RECORD *pQuestRecord;
	int nClass;

	pQuest = QuestsFind(ptInit->ptGame, 4);
	if (pQuest)
	{
	nClass = (int)ptInit->hObject->dwClassId;
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	nClass = (int)ptInit->hObject->dwClassId;
	if (pQuest->bActive && !pQuestRecord->field_4c)
	{
		if (pQuestRecord->bQuestDone)
		{
			D2CommonUnitSetMode(ptInit->hObject, 2);
			return;
		}
		if (pQuestRecord->abStones[nClass - 17] == 1)
		{
			pQuestRecord->abStones[nClass - 17] = 0;
			D2CommonUnitSetMode(ptInit->hObject, 0);
		}
		return;
	}
	pQuestRecord->field_4c = 0;
	if (!pQuestRecord->bStonePortalOpened && nClass == 17)
	{
		pQuestRecord->dwAlphaStoneId = ptInit->hObject->dwId;
		if (!pQuestRecord->bStonePortalTimerPending)
		{
			pQuestRecord->bStonePortalTimerPending = 1;
			QuestsAddTimer(pQuest, A1Q4StoneTimer, 1);
		}
	}
	D2CommonUnitSetMode(ptInit->hObject, 2);
	return; }
	if (ptInit->hObject->dwMode != 2)
		D2CommonUnitSetMode(ptInit->hObject, 2);
}

BYTE __fastcall A1Q4StoneTimer(GAME *ptGame  , QUEST *pQuest
)
{
	A1Q4RECORD *pQuestRecord;
	UNIT *hStone;
	GAMEROOM *hRoom;
	int nX;
	int nY;

	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	hStone = SUnitFind(ptGame, 2, pQuestRecord->dwAlphaStoneId);
	if (hStone)
	{
		if (SkillsOpenPortal(ptGame, NULL, ((GAMEROOM *)D2CommonUnitGetPathRoom(hStone)), ((int)D2CommonUnitGetX(hStone) + 4), (int)D2CommonUnitGetY(hStone) + 4, LEVEL_ACT_1_TRISTRAM, 0, 60, 1))
		{
			pQuestRecord->bStonePortalOpened = 1;
			pQuestRecord->bStonePortalTimerPending = 0;
			return TRUE;
		}
		return FALSE;
	}
	pQuestRecord->bStonePortalTimerPending = 0;
	return TRUE;
}

int __fastcall A1Q4OperateStone(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuestInfo;
	A1Q4RECORD *pQuestRecord;
	void *pRecord;
	UNIT *hObject;
	UNIT *hStone;
	UNIT *hPortal;
	GAMEROOM *hRoom;
	int nX;
	int nY;
	GAMECOORD tCoord;
	WORD *pwOrder;
	DWORD *pdwStone;
	int nCount;
	int i;
	GAME *ptGame;
	UNIT *hPlayer;

	pQuestInfo = QuestsFind(pObjectParam->ptGame, 4);
#line 1666
	D2ASSERT(pQuestInfo != NULL);
	pQuestRecord = (A1Q4RECORD *)pQuestInfo->pQuestRecord;
	if (!pQuestRecord->bOrderDrawn)
	{
		pQuestRecord->bOrderDrawn = 1;
		A1Q4DrawOrder(pQuestInfo);
	}
	if (!ITEMSFindByType(pObjectParam->hPlayer, A1Q4_CODE_DECIPHERED))
	{
		if (!(pQuestRecord->wNoScrollTouches % 64))
		{
	hPlayer = pObjectParam->hPlayer;
	ptGame = pObjectParam->ptGame;
			pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
			if (!D2CommonStepTestGridBit(pRecord, 4, 3) && !D2CommonStepTestGridBit(pRecord, 4, 4))
				SUnitSetEvent(pObjectParam->hPlayer, 37, pObjectParam->hPlayer);
		}
		pQuestRecord->wNoScrollTouches++;
		return 0;
	}
	if (!pQuestInfo->bActive)
		return 0;
	if (pQuestInfo->bState >= 6)
		return 0;
	hObject = NULL;
	if (pQuestRecord->bInvisibleObjectPresent == 1)
	{
		hObject = SUnitFind(pObjectParam->ptGame, 2, pQuestRecord->dwInvisibleObjectId);
		if (!pQuestRecord->bScrollDeciphered && hObject && hObject->dwClassId == 61)
		{
			if (!pQuestInfo->bState && pQuestInfo->bActive == 1)
			{
				A1Q4QuestStarted(pQuestInfo);
#line 1709
				QuestsSetState(pQuestInfo, 1, D2_THIS_FILE, __LINE__);
			}
			if (!ITEMSFindByType(pObjectParam->hPlayer, A1Q4_CODE_DECIPHERED))
				return 0;
		}
	}
	if (pQuestRecord->bStonesDone == 1)
		return 0;
	if (pQuestInfo->bState != 5)
	{
		if (!pQuestInfo->bState && pQuestInfo->bActive == 1)
		{
			A1Q4QuestStarted(pQuestInfo);
#line 1727
			QuestsSetState(pQuestInfo, 1, D2_THIS_FILE, __LINE__);
		}
		if (!ITEMSFindByType(pObjectParam->hPlayer, A1Q4_CODE_DECIPHERED))
			return 0;
		pQuestInfo->bState = 5;
	}
	if ((WORD)pObjectParam->dwClassId != pQuestRecord->awOrder[pQuestRecord->wNumStonesTouched])
		return 0;
	pQuestRecord->adwStones[pQuestRecord->wNumStonesTouched] = pObjectParam->hObject->dwId;
	pQuestRecord->wNumStonesTouched++;
	if (pObjectParam->hObject->dwMode)
		return 0;
	nCount = pQuestRecord->wNumStonesTouched;
	if (nCount < 0)
		return 0;
	if (nCount > 4)
	{
		if (nCount != 5)
			return 0;
	D2CommonUnitSetMode(pObjectParam->hObject, 1);
	if (hObject)
		D2CommonUnitSetMode(hObject, 6);
	pQuestRecord->bStonesDone = 1;
	QuestsRemoveItem(pObjectParam->ptGame, pObjectParam->hPlayer, A1Q4_CODE_DECIPHERED);
	pQuestRecord->nScrollsHeld--;
	pwOrder = pQuestRecord->awOrder;
	tCoord.dwX = 0;
	tCoord.dwY = 0;
	pdwStone = pQuestRecord->adwStones;
	for (i = 5; i; i--)
	{
		if (*pwOrder == 21)
		{
			hStone = SUnitFind(pObjectParam->ptGame, 2, *pdwStone);
			if (hStone && hStone->dwClassId == 21)
				D2CommonUnitGetPosition(hStone, &tCoord);
		}
		pdwStone++;
		pwOrder++;
	}
	if (tCoord.dwX)
	{
		nX = (int)tCoord.dwX + 6;
		nY = (int)tCoord.dwY - 3;
		hPortal = SkillsCreateMissileAt(pObjectParam->ptGame, pObjectParam->hPlayer, 1,
			MISSILE_CAIRNSTONES, nX, nY);
		if (hPortal)
		{
			hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hPortal);
			D2CommonDungeonRoomSetHold(hRoom, 0);
		}
	}
	if (pQuestInfo->bStatus < 4)
	{
		pQuestInfo->bStatusFlags = 0;
		QuestsSetStatus(pQuestInfo, 4, pObjectParam->hPlayer, A1Q4SendStatus, TRUE);
		SUnitIterate(pQuestInfo->ptGame, 0, NULL, A1Q4UpdatePlayer);
	}
		QuestsSetAll(pQuestInfo->ptGame, 1);
	}
	else
	{
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		if (hObject)
			D2CommonUnitSetMode(hObject, pQuestRecord->wNumStonesTouched + 1);
	}
	return 0;
}

BOOL __fastcall A1Q4UpdatePlayer(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	QUEST *pQuest;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 4, 0) == 1)
		return FALSE;
	if (D2CommonStepTestGridBit(pRecord, 4, 1) == 1)
		return FALSE;
	pQuest = QuestsFind(ptGame, 4);
	if (!pQuest)
		return FALSE;
	if (pQuest->bState == 2)
	{
		D2CommonQuestRecordSetBit(pRecord, 4, 2);
		return FALSE;
	}
	if (pQuest->bState >= 3 && pQuest->bState <= 5)
		D2CommonQuestRecordSetBit(pRecord, 4, 3);
	return FALSE;
}

int __fastcall A1Q4OperateTree(OBJOPERATE *pObjectParam  )
{
	QUEST *pQuestInfo;
	A1Q4RECORD *pQuestRecord;
	void *pRecord;
	UNIT *hItem;
	int nLevel;
	GAME *ptGame;
	UNIT *hPlayer;

	pQuestInfo = QuestsFind(pObjectParam->ptGame, 4);
#line 1859
	D2ASSERT(pQuestInfo != NULL);
	if (!pQuestInfo->bActive)
	{
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
		return 0;
	}
	if (pQuestInfo->bState >= 6)
		return 0;
	if (pObjectParam->hObject->dwMode)
		return 0;
	hPlayer = pObjectParam->hPlayer;
	ptGame = pObjectParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, 4, 0) == 1)
		return 0;
	if (D2CommonStepTestGridBit(pRecord, 4, 1) == 1)
		return 0;
	if (ITEMSFindByType(pObjectParam->hPlayer, A1Q4_CODE_DECIPHERED)
		|| ITEMSFindByType(pObjectParam->hPlayer, A1Q4_CODE_SCROLL))
	{
		SUnitSetEvent(pObjectParam->hPlayer, 18, pObjectParam->hPlayer);
		return 0;
	}
	pQuestRecord = (A1Q4RECORD *)pQuestInfo->pQuestRecord;
	SUnitSetEvent(pObjectParam->hPlayer, 43, NULL);
	pObjectParam->hObject->dwDropItemCode = A1Q4_CODE_SCROLL;
	A1Q4QuestStarted(pQuestInfo);
#line 1894
	QuestsSetState(pQuestInfo, 4, D2_THIS_FILE, __LINE__);
	hItem = ITEMSDropFromUnit(pObjectParam->ptGame, pObjectParam->hObject, 2, &nLevel, NULL, 44, FALSE);
	if (hItem)
	{
		pQuestInfo->pCallback[QUESTCALLBACK_PLAYERDROPPEDWITHQUESTITEM] = A1Q4PlayerDropped;
		pQuestInfo->bStatusFlags = 0;
		QuestsSetStatus(pQuestInfo, 2, pObjectParam->hPlayer, A1Q4SendStatus, TRUE);
		pQuestRecord->bScrollItemIdSet = 1;
		pQuestRecord->dwScrollItemId = hItem->dwId;
		pQuestRecord->dwTreeMode = 1;
		pQuestRecord->bScrollObtained = 1;
		pQuestRecord->nScrollsHeld++;
		D2CommonUnitSetMode(pObjectParam->hObject, 1);
	}
	pQuestRecord->bTreePresent = 1;
	pQuestRecord->dwTreeId = pObjectParam->hObject->dwId;
	return 0;
}

void __fastcall A1Q4PlayerDropped(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q4RECORD *pQuestRecord;
	DWORD dwCode;

	if (!pQuest->bActive)
		return;
	if (pQuest->bState == 6)
		return;
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	dwCode = D2CommonITEMSGetTypeCode(pqParam->hEventUnit);
	if (dwCode == A1Q4_CODE_DECIPHERED || dwCode == A1Q4_CODE_SCROLL)
		pQuestRecord->nScrollsHeld--;
	if (pQuestRecord->nScrollsHeld)
		return;
	if (pQuest->bState > 5)
		return;
	if (pQuestRecord->bStonesDone == 1)
		return;
	if (pQuestRecord->bTreePresent != 1)
		return;
#line 987
	QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
	A1Q4ResetTree(pQuest);
}

void __fastcall A1Q4ResetTree(QUEST *pQuest  )
{
	A1Q4RECORD *pQuestRecord;
	UNIT *hObject;

	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bTreePresent == 1)
	{
		hObject = SUnitFind(pQuest->ptGame, 2, pQuestRecord->dwTreeId);
		if (hObject && hObject->dwClassId == 30)
			D2CommonUnitSetMode(hObject, 0);
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 1, NULL, A1Q4SendStatus, TRUE);
#line 891
		QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
		pQuestRecord->dwTreeMode = 0;
		SUnitIterate(pQuest->ptGame, 0, NULL, A1Q4UpdatePlayer);
		return;
	}
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 1, NULL, A1Q4SendStatus, TRUE);
#line 899
	QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
	pQuestRecord->dwTreeMode = 0;
}

void __fastcall A1Q4SendOrder(GAME *ptGame  , UNIT *hUnit
)
{
	QUEST *pQuest;
	A1Q4RECORD *pQuestRecord;
	A1Q4ORDERCMD Cmd;
	CLIENT *ptClient;
	int idx;

	pQuest = QuestsFind(ptGame, 4);
	if (!pQuest)
		return;
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	if (!pQuestRecord->bOrderDrawn)
	{
		pQuestRecord->bOrderDrawn = 1;
		A1Q4DrawOrder(pQuest);
	}
	Cmd.bCmd = 73;
	Cmd.wQuest = 4;
	ptClient = SUnitGetClient(hUnit);
	idx = 0;
	while (1)
	{
		Cmd.wOrder[idx] = pQuestRecord->awOrder[idx] - 17;
#line 1946
		D2ASSERT(Cmd.wOrder[idx]<5);
		idx++;
		if (idx >= 5)
			break;
	}
	SCmdSendQuestCommand(ptClient, (const struct SCMDCOPY15 *)&Cmd);
}

void __fastcall A1Q4Init(QUEST *pQuest  )
{
	A1Q4RECORD *pQuestRecord;
	DWORD *pdw;
	int i;

	for (i = 0; i < NUM_QUESTCALLBACKS; i++)
		pQuest->pCallback[i] = NULL;
	pQuest->pCallback[QUESTCALLBACK_CHANGEDLEVEL] = A1Q4ChangedLevel;
	pQuest->pCallback[4] = A1Q4ItemAcquired;
	pQuest->pCallback[6] = A1Q4ResetUnlessStonesDone;
	pQuest->pCallback[QUESTCALLBACK_PLAYERJOINEDGAME] = A1Q4PlayerJoined;
	pQuest->pCallback[QUESTCALLBACK_PLAYERDROPPEDWITHQUESTITEM] = A1Q4PlayerDropped;
	pQuest->pCallback[8] = A1Q4UnitKilled;
	pQuest->ptNpcTables = sgtA1Q4Npcs;
	pQuest->eFilter = 4;
	pQuest->bStatus = 0;
	pQuest->bCallbacksEnabled = 1;
	pQuest->pStatusFilter = NULL;
	pQuest->bDoneStateMin = 6;
	pQuest->pActiveFilter = A1Q4Active;
	pQuest->pCallback[0] = A1Q4Talk;
	pQuest->pCallback[11] = A1Q4NpcMessage;
	pQuest->pCallback[10] = A1Q4PlayerLeft;
	pQuest->pCallback[2] = A1Q4Activate;
	pQuest->pCallback[QUESTCALLBACK_PLAYERSTARTEDGAME] = A1Q4PlayerStarted;
	pQuest->pSequence = A1Q4Sequence;
	pQuest->dwNextQuestId = 3;
#line 2121
	pQuestRecord = (A1Q4RECORD *)D2ALLOCPOOL(pQuest->ptGame->pMemPool, sizeof(A1Q4RECORD));
	pQuest->pQuestRecord = pQuestRecord;
	pdw = (DWORD *)pQuestRecord;
	for (i = 0; i < (int)(sizeof(A1Q4RECORD) / sizeof(DWORD)); i++)
		pdw[i] = 0;
	pQuestRecord->wNumStonesTouched = 0;
	pQuestRecord->bOrderDrawn = 0;
	pQuestRecord->bScrollDeciphered = 0;
	pQuestRecord->bScrollItemIdSet = 0;
	pQuestRecord->wNoScrollTouches = 0;
	pQuestRecord->dwInvisibleObjectId = 0;
	pQuestRecord->bInvisibleObjectPresent = 0;
	pQuestRecord->dwTreeId = 0;
	pQuestRecord->dwScrollItemId = 0;
	pQuestRecord->bTreePresent = 0;
	pQuestRecord->bCainTimerPending = 0;
	pQuestRecord->dwTreeMode = 0;
	pQuestRecord->dwGibbetMode = 0;
	pQuestRecord->bQuestDone = 0;
	pQuestRecord->abStones[0] = 0;
	pQuestRecord->abStones[1] = 0;
	pQuestRecord->abStones[2] = 0;
	pQuestRecord->abStones[3] = 0;
	pQuestRecord->abStones[4] = 0;
	pQuestRecord->bTownCainMade = 0;
	pQuestRecord->bTownCainWanted = 0;
	pQuestRecord->bCainFreed = 0;
	pQuestRecord->bCainNotCreated = 0;
	pQuestRecord->bStatus1Pending = 0;
	pQuestRecord->bStatus3Pending = 0;
	QuestsPlayersClear(&pQuestRecord->tPlayers1);
	QuestsPlayersClear(&pQuestRecord->tPlayers2);
}

void __fastcall A1Q4Activate(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q4RECORD *pQuestRecord;
	WORD wNpcClass;

	wNpcClass = (WORD)pqParam->hEventUnit->dwClassId;
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	if (wNpcClass == 148)
	{
		if (pQuestRecord->bStatus1Pending == 1)
		{
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 1, pqParam->hPlayer, A1Q4SendStatus, TRUE);
			pQuestRecord->bStatus1Pending = 0;
			SUnitIterate(pQuest->ptGame, 0, NULL, A1Q4UpdatePlayer);
		}
		if (pQuestRecord->bStatus3Pending == 1)
		{
			pQuest->bStatusFlags = 0;
			QuestsSetStatus(pQuest, 3, pqParam->hPlayer, A1Q4SendStatus, TRUE);
			SUnitIterate(pQuest->ptGame, 0, NULL, A1Q4UpdatePlayer);
			pQuestRecord->bStatus3Pending = 0;
		}
		return;
	}
	if (wNpcClass == 146)
		pQuestRecord->bCainActivated = 1;
}

void __fastcall A1Q4NpcMessage(QUEST *pQuestInfo  , QUESTPARAM *pqParam
)
{
	A1Q4RECORD *pQuestRecord;
	WORD wNpcClass;
	WORD wMessage;
	void *pRecord;
	UNIT *hItem;
	QUEST *pQuest;
	GAMEROOM *hRoom;
	int nX;
	int nY;
	GAME *ptGame;
	UNIT *hPlayer;

	wNpcClass = (WORD)pqParam->field_14;
	wMessage = (WORD)pqParam->field_18;
	pQuestRecord = (A1Q4RECORD *)pQuestInfo->pQuestRecord;
	ptGame = pqParam->ptGame;
	hPlayer = pqParam->hPlayer;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (wNpcClass == 148)
	{
		if (wMessage == 97)
		{
			pQuestRecord->bStatus1Pending = 1;
#line 329
			QuestsSetState(pQuestInfo, 2, D2_THIS_FILE, __LINE__);
			QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
			return;
		}
		if (wMessage == 112)
		{
			if (!ITEMSFindByType(pqParam->hPlayer, A1Q4_CODE_SCROLL))
				return;
			QuestsRemoveItem(pqParam->ptGame, pqParam->hPlayer, A1Q4_CODE_SCROLL);
			hItem = QuestsCreateItem(pqParam->ptGame, pqParam->hPlayer, A1Q4_CODE_DECIPHERED, 0,
				2);
			if (hItem)
			{
			pQuestRecord->bStatus3Pending = 1;
			pQuestRecord->bScrollItemIdSet = 1;
			pQuestRecord->bScrollDeciphered = 1;
			pQuestRecord->dwScrollItemId = hItem->dwId;
#line 347
			QuestsDebugMessage(pQuestInfo->ptGame, "deciphered scroll", D2_THIS_FILE, __LINE__);
#line 348
			QuestsSetState(pQuestInfo, 5, D2_THIS_FILE, __LINE__);
			pQuestInfo->bStatusFlags = 0;
			QuestsSetStatus(pQuestInfo, 3, pqParam->hPlayer, A1Q4SendStatus, FALSE);
			return;
			}
			pQuestRecord->nScrollsHeld--;
			return;
		}
		if (wMessage != 118)
			return;
		if (!D2CommonStepTestGridBit(pRecord, 4, 1))
			return;
		D2CommonQuestRecordSetBit(pRecord, 4, 0);
		D2CommonQuestRecordClearBit(pRecord, 4, 1);
		QuestsPlayersAdd(&pQuestInfo->tPlayers, pqParam->hPlayer->dwId);
		QuestsSendPlayerRecord(pQuestInfo->ptGame, pqParam->hPlayer);
		QuestsCreateItem(pqParam->ptGame, pqParam->hPlayer, A1Q4_CODE_RING, 7, 4);
		QuestsSendFlag(pqParam->hPlayer, 4);
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		if (D2CommonStepTestGridBit(pRecord, 4, 13))
		{
			if (!QuestsGetRecordBit(pqParam->ptGame, 4, 13))
			{
				QuestsSetRecordBit(pQuestInfo->ptGame, 4, 13);
#line 377
				D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
				pQuestInfo->pSequence(pQuestInfo);
			}
			if (pQuestInfo->bStatus != 13)
				pQuestInfo->bStatus = 13;
			if (pQuestInfo->bState != 6)
			{
#line 386
				QuestsSetState(pQuestInfo, 6, D2_THIS_FILE, __LINE__);
			}
			SUnitIterate(pQuestInfo->ptGame, 1, NULL, A1Q4FreeCain);
			if (!pQuestRecord->bCainFreed)
				SUnitInactiveRemoveClass(pQuestInfo->ptGame, 1, 146, 0);
			if (pQuestRecord->bTownCainMade)
				return;
			if (pQuestRecord->bTownCainWanted)
				return;
			pQuestRecord->field_91 = 1;
			pQuest = QuestsFind(pQuestInfo->ptGame, 4);
			if (pQuest)
				((A1Q4RECORD *)pQuest->pQuestRecord)->bTownCainWanted = 1;
			nY = (int)D2CommonUnitGetY(pqParam->hPlayer);

			A1Q4SpawnNpc(pQuestInfo->ptGame, ((GAMEROOM *)D2CommonUnitGetPathRoom(pqParam->hPlayer)), ((int)D2CommonUnitGetX(pqParam->hPlayer)), nY);
		}
		if (pQuestRecord->bCreditedPlayerSeen == 1)
			QuestsSetRecordBit(pQuestInfo->ptGame, 4, 13);
		return;
	}
	if (wNpcClass != 265)
		return;
	if (wMessage == 125)
	{
		QuestsPlayersAdd(&pQuestInfo->tPlayers, pqParam->hPlayer->dwId);
		QuestsPlayersRemove(&pQuestRecord->tPlayers1, pqParam->hPlayer->dwId);
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
		return;
	}
	if (wMessage == 126 || wMessage == 123)
	{
		QuestsPlayersAdd(&pQuestRecord->tPlayers2, pqParam->hPlayer->dwId);
		QuestsTextMessage(pqParam->ptGame, pqParam->hPlayer, pqParam->hEventUnit);
	}
}

void __fastcall A1Q4Talk(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q4RECORD *pQuestRecord;
	UNIT *hScroll;
	int nNpcClass;
	void *pRecord;
	DWORD dwStarted;
	DWORD dwCredited;
	int nTable;
	GAME *ptGame;
	UNIT *hPlayer;

	hScroll = ITEMSFindByType(pqParam->hPlayer, A1Q4_CODE_SCROLL);
	nNpcClass = (int)pqParam->hEventUnit->dwClassId;
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	if (nNpcClass == 146)
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 9);
	if (pQuestRecord->bStonesDone == 1 && ITEMSFindByType(pqParam->hPlayer, A1Q4_CODE_DECIPHERED))
	{
		QuestsRemoveItem(pqParam->ptGame, pqParam->hPlayer, A1Q4_CODE_DECIPHERED);
		pQuestRecord->nScrollsHeld--;
	}
	dwStarted = D2CommonStepTestGridBit(pRecord, 4, 1);
	dwCredited = D2CommonStepTestGridBit(pRecord, 4, 13);
	if (nNpcClass == 265 && !QuestsPlayersFind(&pQuestRecord->tPlayers2, pqParam->hPlayer->dwId)
		&& dwCredited)
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 5);
		return;
	}
	if (dwStarted)
	{
		if (nNpcClass == 265)
		{
			if (QuestsPlayersFind(&pQuestRecord->tPlayers2, pqParam->hPlayer->dwId))
				QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 7);
			else
				QuestsNpcTable(pQuest, (void *)pqParam->field_14, 265, 5);
			return;
		}
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 5);
		return;
	}
	if (QuestsPlayersFind(&pQuestRecord->tPlayers1, pqParam->hPlayer->dwId) == 1)
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 6);
		return;
	}
	if (QuestsHasPlayer(pQuest, pqParam->hPlayer->dwId) == 1 && nNpcClass == 265
		&& !QuestsPlayersFind(&pQuestRecord->tPlayers2, pqParam->hPlayer->dwId))
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, 265, 5);
		return;
	}
	if (QuestsHasPlayer(pQuest, pqParam->hPlayer->dwId) == 1)
	{
		if (D2CommonStepTestGridBit(pRecord, 4, 14))
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 8);
		else if (D2CommonStepTestGridBit(pRecord, 4, 0))
			QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 7);
		return;
	}
	if (D2CommonStepTestGridBit(pRecord, 4, 14))
		return;
	if (!pQuest->bState)
		return;
	if (D2CommonStepTestGridBit(pRecord, 4, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, 4, 15))
		return;
	if (!hScroll && pQuest->bState == 4)
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 2);
		return;
	}
	if (hScroll)
	{
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, 3);
		return;
	}
	if (pQuest->bState >= 6)
		return;
	nTable = sgnA1Q4NpcTable[pQuest->bState];
	if (nTable != -1 && (DWORD)nTable < 11)
		QuestsNpcTable(pQuest, (void *)pqParam->field_14, nNpcClass, nTable);
}

void __fastcall A1Q4SpawnNpc(GAME *ptGame  , GAMEROOM *hRoom,
	int nX, int nY)
{
	QUEST *pQuest;
	A1Q4RECORD *pQuestRecord;
	RECT atRect[2];
	GAMECOORD tCoord;
	GAMEROOM *hSpotRoom;
	UNIT *hMonster;
	int i;

	pQuest = QuestsFind(ptGame, 4);
	if (!pQuest)
		return;
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	hSpotRoom = hRoom;
	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	tCoord.dwX = (DWORD)nX;
	tCoord.dwY = (DWORD)nY;
	for (i = 0; i <= 20; i++)
	{
		if ((int)tCoord.dwX >= atRect[0].left && (int)tCoord.dwY >= atRect[0].top
			&& (int)tCoord.dwX < atRect[0].right + atRect[0].left - 1
			&& (int)tCoord.dwY < atRect[0].bottom + atRect[0].top - 1)
			goto found;
		tCoord.dwX++;
		tCoord.dwY++;
	}
	tCoord.dwX = (DWORD)nY;
found:
	QuestsFindSpot(hRoom, &tCoord, 2, 256, &hSpotRoom);
	if (!hSpotRoom)
	{
		hSpotRoom = hRoom;
		tCoord.dwX = (DWORD)nX;
		tCoord.dwY = (DWORD)nY;
	}
	hMonster = MonsterSpawn(ptGame, hSpotRoom, (int)tCoord.dwX, (int)tCoord.dwY, 265, 1, 5, 0);
	if (!hMonster)
	{
		for (i = 0; i < 20; )
		{
			tCoord.dwX++;
			tCoord.dwY++;
			hSpotRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(hSpotRoom, tCoord.dwX, tCoord.dwY);
			if (!hSpotRoom)
			{
				hSpotRoom = hRoom;
				tCoord.dwX = (DWORD)nX;
				tCoord.dwY = (DWORD)nY;
			}
			QuestsFindSpot(hSpotRoom, &tCoord, 2, 256, &hSpotRoom);
			if (!hSpotRoom)
			{
				hSpotRoom = hRoom;
				tCoord.dwX = (DWORD)nX;
				tCoord.dwY = (DWORD)nY;
			}
			i++;
			hMonster = MonsterSpawn(ptGame, hSpotRoom, (int)tCoord.dwX, (int)tCoord.dwY, 265, 1, 10, 0);
			if (hMonster)
				break;
		}
		if (!hMonster)
		{
			hMonster = MonsterSpawn(ptGame, hRoom, nX, nY, 265, 1, 15, 0);
			if (!hMonster)
				return;
		}
	}
	hMonster->dwFlags0 |= 50331648;
	pQuestRecord->bTownCainMade = 1;
	pQuestRecord->bTownCainWanted = 0;
	pQuestRecord->dwTownCainId = hMonster->dwId;
}

void __fastcall A1Q4PlayerJoined(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q4RECORD *pQuestRecord;
	UNIT *hDeciphered;
	UNIT *hScroll;

	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	hDeciphered = ITEMSFindByType(pqParam->hPlayer, A1Q4_CODE_DECIPHERED);
	hScroll = ITEMSFindByType(pqParam->hPlayer, A1Q4_CODE_SCROLL);
	if (hDeciphered)
		pQuestRecord->nScrollsHeld++;
	if (hScroll)
		pQuestRecord->nScrollsHeld++;
}

void __fastcall A1Q4PlayerLeft(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q4RECORD *pQuestRecord;

	QuestsPlayerDone(pQuest, pqParam);
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	QuestsPlayersRemove(&pQuestRecord->tPlayers1, pqParam->hPlayer->dwId);
	QuestsPlayersRemove(&pQuestRecord->tPlayers2, pqParam->hPlayer->dwId);
}

void __fastcall A1Q4ChangedLevel(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q4RECORD *pQuestRecord;
	int nLevelNew;
	int nLevelOld;
	void *pRecord;
	UNIT *hObject;
	GAMEROOM *hRoom;
	int nX;
	int nY;
	GAME *ptGame;
	UNIT *hPlayer;

	nLevelNew = (int)pqParam->field_18;
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	nLevelOld = (int)pqParam->field_14;
	if (nLevelNew == 38 && !pQuestRecord->bTownCainMade && !pQuestRecord->bQuestDone && pQuest->bState >= 6)
	{
		A1Q4QuestStarted(pQuest);
#line 1030
		QuestsSetState(pQuest, 5, D2_THIS_FILE, __LINE__);
		pQuest->bStatusFlags = 0;
		QuestsSetStatus(pQuest, 4, pqParam->hPlayer, A1Q4SendStatus, FALSE);
		SUnitIterate(pQuest->ptGame, 0, NULL, A1Q4UpdatePlayer);
	}
	if (nLevelOld == 1)
	{
		QuestsPlayerLeft(pQuest, pqParam);
		QuestsPlayersRemove(&pQuestRecord->tPlayers1, pqParam->hPlayer->dwId);
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
		pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))->apQuestRecords[ptGame->bDifficultyLevel];
		if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0) == 1)
			return;
		if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 1) == 1)
			return;
		if (pQuest->bState == 2)
		{
#line 1050
			QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
		}
	}
	if (nLevelNew == 1)
	{
		pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
		if (!pQuestRecord->bCainStartPresent)
			return;
		hObject = SUnitFind(pqParam->ptGame, 2, pQuestRecord->dwCainStartId);
		if (!hObject)
			return;
		if (pQuestRecord->bTownCainWanted != 1)
			return;
		if (pQuestRecord->bTownCainMade)
			return;
		nY = (int)D2CommonUnitGetY(hObject);

		A1Q4SpawnNpc(pqParam->ptGame, ((GAMEROOM *)D2CommonUnitGetPathRoom(hObject)), ((int)D2CommonUnitGetX(hObject)), nY);
		return;
	}
	if (nLevelNew != 40)
		return;
	hPlayer = pqParam->hPlayer;
	ptGame = pqParam->ptGame;
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 1) == 1)
		return;
	if (pQuestRecord->bQuestDone)
		return;
	if (pQuest->bState >= 6)
		return;
	A1Q4QuestStarted(pQuest);
	A1Q4SetDone(pQuest, 0, 1);
#line 1084
	QuestsSetState(pQuest, 7, D2_THIS_FILE, __LINE__);
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 5, pqParam->hPlayer, A1Q4SendStatus, TRUE);
	QuestsSetRecordBit(pQuest->ptGame, 4, 13);
	SUnitIterate(pQuest->ptGame, 0, NULL, A1Q4FlagStarted);
	pQuestRecord->dwTreeMode = 1;
}

BOOL __fastcall A1Q4FlagStarted(GAME *ptGame  , UNIT *hUnit
, void *pArg)
{
	void *pRecord;
	QUEST *pQuest;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	if (!D2CommonStepTestGridBit(pRecord, 4, 0) && !D2CommonStepTestGridBit(pRecord, 4, 1))
	{
		D2CommonQuestRecordSetBit(pRecord, 4, 14);
		pQuest = QuestsFind(ptGame, 4);
		QuestsPlayersAdd(&((A1Q4RECORD *)pQuest->pQuestRecord)->tPlayers1, hUnit->dwId);
	}
	return FALSE;
}

void __fastcall A1Q4ItemAcquired(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
#line 1194
	QuestsDebugMessage(pQuest->ptGame, "cairn stone item acquired", D2_THIS_FILE, __LINE__);
	if (pQuest->bActive)
		SUnitIterate(pQuest->ptGame, 0, NULL, A1Q4UpdatePlayer);
}

void __fastcall A1Q4ResetUnlessStonesDone(QUEST *pQuest  ,
	QUESTPARAM *pqParam
)
{
	if (((A1Q4RECORD *)pQuest->pQuestRecord)->bStonesDone == 1)
		return;
	if (pQuest->bState > 5)
		return;
#line 1253
	QuestsSetState(pQuest, 3, D2_THIS_FILE, __LINE__);
	A1Q4ResetTree(pQuest);
}

BYTE __fastcall A1Q4Sequence(QUEST *pQuestInfo  )
{
	QUEST *pNextQuest;

	if (!pQuestInfo->bState && pQuestInfo->bActive == 1)
	{
		A1Q4QuestStarted(pQuestInfo);
#line 1964
		QuestsSetState(pQuestInfo, 1, D2_THIS_FILE, __LINE__);
		return TRUE;
	}
	if (pQuestInfo->bState != 6 && pQuestInfo->bActive)
		return TRUE;
	pNextQuest = QuestsFind(pQuestInfo->ptGame, (int)pQuestInfo->dwNextQuestId);
	if (pNextQuest)
	{
#line 1972
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)pQuestInfo->pSequence), "pQuestInfo->pSequence");
		return pNextQuest->pSequence(pNextQuest);
	}
	return FALSE;
}

void __fastcall A1Q4PlayerStarted(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	A1Q4RECORD *pQuestRecord;
	void *pRecord;
	QUEST *pQuest4;
	UNIT *hDeciphered;
	UNIT *hScroll;
	GAME *ptGame = pqParam->ptGame;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(pqParam->hPlayer))
		->apQuestRecords[ptGame->bDifficultyLevel];
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	if (D2CommonStepTestGridBit(pRecord, 4, 0) == 1)
	{
		pQuest4 = QuestsFind(pQuest->ptGame, 4);
		if (pQuest4)
			((A1Q4RECORD *)pQuest4->pQuestRecord)->bTownCainWanted = 1;
		QuestsSetRecordBit(pQuest->ptGame, 4, 13);
		pQuestRecord->dwGibbetMode = 3;
		pQuestRecord->field_4c = 1;
	}
	else if (D2CommonStepTestGridBit(pRecord, 4, 15) == 1)
	{
		pQuest4 = QuestsFind(pQuest->ptGame, 4);
		if (pQuest4)
			((A1Q4RECORD *)pQuest4->pQuestRecord)->bTownCainWanted = 1;
		pQuest4 = QuestsFind(pQuest->ptGame, 4);
		if (pQuest4)
			((A1Q4RECORD *)pQuest4->pQuestRecord)->bCreditedPlayerSeen = 1;
		pQuestRecord->dwGibbetMode = 3;
		pQuestRecord->field_4c = 1;
	}
	else if (D2CommonStepTestGridBit(pRecord, 4, 4) == 1)
	{
		pQuest->bStatus = 4;
		pQuest->bState = 5;
		pQuestRecord->field_4c = 1;
		pQuestRecord->bScrollDeciphered = 1;
		pQuestRecord->bScrollObtained = 1;
		pQuestRecord->dwTreeMode = 1;
	}
	else if (D2CommonStepTestGridBit(pRecord, 4, 3) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 3;
	}
	else if (D2CommonStepTestGridBit(pRecord, 4, 2) == 1)
	{
		pQuest->bStatus = 1;
		pQuest->bState = 2;
	}
	hDeciphered = ITEMSFindByType(pqParam->hPlayer, A1Q4_CODE_DECIPHERED);
	hScroll = ITEMSFindByType(pqParam->hPlayer, A1Q4_CODE_SCROLL);
	if (hDeciphered)
		pQuestRecord->nScrollsHeld++;
	if (hScroll)
		pQuestRecord->nScrollsHeld++;
	if (hDeciphered)
	{
		pQuest->bState = 5;
		pQuest->bStatus = 3;
		pQuestRecord->bScrollDeciphered = 1;
		pQuestRecord->bScrollObtained = 1;
		pQuestRecord->dwTreeMode = 1;
		return;
	}
	if (hScroll)
	{
		pQuestRecord->bScrollObtained = 1;
		pQuest->bState = 4;
		pQuest->bStatus = 2;
		pQuestRecord->dwTreeMode = 1;
	}
}

void __fastcall A1Q4UnitKilled(QUEST *pQuest  , QUESTPARAM *pqParam
)
{
	int nLevel;
	int i;

	QuestsDropItem(pQuest, pqParam, 64);
	pqParam->hEventUnit->dwDropItemCode = A1Q4_CODE_POTION;
	for (i = 0; i < 8; i++)
		ITEMSDropFromUnit(pqParam->ptGame, pqParam->hEventUnit, 0, &nLevel, NULL, 44, FALSE);
}

void __fastcall A1Q4TreeObjInit(OBJINIT *ptInit  )
{
	QUEST *pQuest;
	A1Q4RECORD *pQuestRecord;

	pQuest = QuestsFind(ptInit->ptGame, 4);
	if (pQuest)
	{
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	pQuestRecord->bTreePresent = 1;
	if (!pQuest->bActive || pQuestRecord->bQuestDone)
		pQuestRecord->dwTreeMode = 1;
	D2CommonUnitSetMode(ptInit->hObject, pQuestRecord->dwTreeMode);
	return;
	}
	if (ptInit->hObject->dwMode != 2)
		D2CommonUnitSetMode(ptInit->hObject, 2);
}

void __fastcall A1Q4AttachObjInit(OBJINIT *ptInit  )
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptInit->ptGame, 4);
	if (pQuest)
	{
		QuestsAttachUnit(pQuest->ptGame, ptInit->hObject, 4);
		return;
	}
	if (ptInit->hObject->dwMode != 2)
		D2CommonUnitSetMode(ptInit->hObject, 2);
}

void __fastcall A1Q4CairnObjInit(QUEST *pQuest  , OBJINIT *ptInit
)
{
	A1Q4RECORD *pQuestRecord;

	if (!pQuest)
		return;
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	D2CommonUnitSetMode(ptInit->hObject, pQuestRecord->dwGibbetMode);
	pQuestRecord->bGibbetPresent = 1;
	pQuestRecord->dwGibbetId = ptInit->hObject->dwId;
}

void __fastcall A1Q4RemovePlayer(GAME *ptGame  , UNIT *hUnit
)
{
	QUEST *pQuest;

	pQuest = QuestsFind(ptGame, 4);
	QuestsPlayersRemove(&((A1Q4RECORD *)pQuest->pQuestRecord)->tPlayers1, hUnit->dwId);
}

void __fastcall A1Q4EnteredLevel(GAME *ptGame  , UNIT *hUnit
)
{
	void *pRecord;
	QUEST *pQuest;

	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	pQuest = QuestsFind(ptGame, 4);
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 0) == 1)
		return;
	if (D2CommonStepTestGridBit(pRecord, pQuest->eFilter, 1) == 1)
		return;
	if (((A1Q4RECORD *)pQuest->pQuestRecord)->bQuestDone)
		return;
	if (pQuest->bState >= 6)
		return;
	A1Q4QuestStarted(pQuest);
	A1Q4SetDone(pQuest, 0, 1);
#line 2279
	QuestsSetState(pQuest, 7, D2_THIS_FILE, __LINE__);
	pQuest->bStatusFlags = 0;
	QuestsSetStatus(pQuest, 5, hUnit, A1Q4SendStatus, TRUE);
	QuestsSetRecordBit(ptGame, 4, 13);
	SUnitIterate(ptGame, 0, NULL, A1Q4FlagStarted);
}

void __fastcall A1Q4NpcObjInit(OBJINIT *ptInit  )
{
	QUEST *pQuest;
	A1Q4RECORD *pQuestRecord;

	pQuest = QuestsFind(ptInit->ptGame, 4);
	if (!pQuest)
		return;
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	pQuestRecord->dwCainStartId = ptInit->hObject->dwId;
	pQuestRecord->bCainStartPresent = 1;
	pQuestRecord->nCainStartX = ptInit->nX;
	pQuestRecord->nCainStartY = ptInit->nY;
	if (pQuestRecord->bTownCainWanted == 1 && !pQuestRecord->bTownCainMade)
		A1Q4SpawnNpc(ptInit->ptGame, ptInit->hRoom, ptInit->nX, ptInit->nY);
}

BOOL __fastcall A1Q4CastAtSpot(GAME *ptGame  , UNIT *hUnit
)
{
	GAMEROOM *hRoom;
	GAMEROOM *hSpotRoom;
	GAMECOORD tCoord;
	void *pRecord;

	if (!QuestsGetRecordBit(ptGame, 4, 11)
		&& D2CommonStepTestGridBit(pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))
			->apQuestRecords[ptGame->bDifficultyLevel], 26, 0))
	{
		hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
		if (hRoom && D2CommonDungeonRoomGetLevelId(hRoom) == 1)
		{
			hSpotRoom = NULL;
			D2CommonUnitGetPosition(hUnit, &tCoord);
			QuestsFindSpot(hRoom, &tCoord, 3, 1024, &hSpotRoom);
			if (hSpotRoom && SkillsOpenPortal(ptGame, hUnit, hSpotRoom, (int)tCoord.dwX, (int)tCoord.dwY,
				LEVEL_ACT_1_MOO_MOO_FARM, 0, 60, 0))
			{
				QuestsSetRecordBit(ptGame, 4, 11);
				return TRUE;
			}
		}
	}
	SUnitSetEvent(hUnit, 19, hUnit);
	return FALSE;
}

void __fastcall A1Q4EventObjInit(OBJINIT *ptInit  )
{
	D2CommonUnitSetMode(ptInit->hObject, 1);
	EventAdd(ptInit->ptGame, ptInit->hObject, 7, ptInit->ptGame->nFrame + 25);
}

void __fastcall A1Q4OperateEvent(QUEST *pQuest  , UNIT *hObject
)
{
	A1Q4RECORD *pQuestRecord;

	switch (hObject->dwMode)
	{
	case 1:
		D2CommonUnitSetMode(hObject, 2);
		break;
	case 2:
		pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
		if (D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hObject)) == 1)
		{
			if (++pQuestRecord->nTownPortalTicks > 5)
				pQuestRecord->field_92 = 1;
			if (pQuestRecord->field_92)
				D2CommonUnitSetMode(hObject, 3);
		}
		else if (pQuestRecord->field_91)
			D2CommonUnitSetMode(hObject, 3);
		break;
	case 3:
		D2CommonUnitSetMode(hObject, 4);
		break;
	}
	EventAdd(pQuest->ptGame, hObject, 7, pQuest->ptGame->nFrame + 25);
}

BOOL __fastcall A1Q4GetCainWalkSpot(GAME *ptGame  , UNIT *hUnit
, GAMECOORD *ptCoord)
{
	QUEST *pQuest;
	A1Q4RECORD *pQuestRecord;

	pQuest = QuestsFind(ptGame, 4);
	if (!pQuest || !pQuest->bActive)
		return FALSE;
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	if (!pQuestRecord->bTownCainPortalMade || pQuestRecord->field_92)
		return FALSE;
	ptCoord->dwX = pQuestRecord->tCainWalkSpot.dwX;
	ptCoord->dwY = pQuestRecord->tCainWalkSpot.dwY;
	return TRUE;
}

void __fastcall A1Q4CreateObject(GAME *ptGame  , UNIT *hUnit
)
{
	QUEST *pQuest;
	A1Q4RECORD *pQuestRecord;
	GAMEROOM *hRoom;
	UNIT *hObject;

	pQuest = QuestsFind(ptGame, 4);
	if (!pQuest)
		return;
	pQuestRecord = (A1Q4RECORD *)pQuest->pQuestRecord;
	if (pQuestRecord->bCainPortalMade)
		return;
	hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hUnit),
		pQuestRecord->tCainPortalSpot.dwX, pQuestRecord->tCainPortalSpot.dwY);
	if (!hRoom)
	{
		pQuestRecord->tCainPortalSpot.dwX += 3;
		pQuestRecord->tCainPortalSpot.dwY += 3;
		return;
	}
	hObject = SUnitCreate(2, 189, (int)pQuestRecord->tCainPortalSpot.dwX,
		(int)pQuestRecord->tCainPortalSpot.dwY, ptGame,
		hRoom, 1, 1, 0);
	if (hObject)
	{
		pQuestRecord->bCainPortalMade = 1;
		pQuestRecord->dwCainPortalId = hObject->dwId;
	}
}
