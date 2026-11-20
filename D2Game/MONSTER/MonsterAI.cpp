#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\MONSTER\\MonsterAI.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Clients.h"
#include "../GAME/SCmd.h"
#include "../GAME/Targets.h"
#include "Monster.h"
#include "MonsterMode.h"
#include "MonsterUnique.h"
#include "MonsterAI.h"
#include "../PLAYER/Player.h"
#include "../PLAYER/PlayerPets.h"
#include "../QUESTS/QUESTS.h"
#include "../QUESTS/a1q5.h"
#include "../QUESTS/a2q3.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUNITNPC.h"
#include "../AI/AiGeneral.h"
#include "../AI/AiThink.h"
#include "../AI/AiTatics.h"
#include "../SKILLS/Skills.h"

#define UNIT_MONSTER			1
#define NUM_MONSTERTARGETS		5
#define PETTYPE_HIRELING		7

struct MONPETINIT
{
 	DWORD		dwSeed;
 	DWORD		dwHireId;
 	MONKILLSRECORD	tRecord;
};
typedef struct MONPETINIT MONPETINIT;

#pragma pack(push, 1)
struct MONTHREATMSG
{
 	BYTE		bCmd;
 	BYTE		bUnitType;
 	DWORD		dwUnitId;
 	BYTE		abText[34];
};
#pragma pack(pop)
typedef struct MONTHREATMSG MONTHREATMSG;

static inline MONSTERDATA *MonsterAIAlloc(GAME *ptGame)
{
	MONSTERDATA *ptMonster;
	int i;

#line 54
	ptMonster = (MONSTERDATA *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(MONSTERDATA));
	for (i = 0; i < (int)(sizeof(MONSTERDATA) / sizeof(DWORD)); i++)
		((DWORD *)ptMonster)[i] = 0;
	return ptMonster;
}

static inline void MonsterAIFree(GAME *ptGame, MONSTERDATA *ptMonster)
{
#line 78
	D2FREEPOOL(ptGame->pMemPool, ptMonster);
}

static inline THREATNODE **MonsterAIThreatListNew(GAME *ptGame)
{
	THREATNODE **pptThreats;

#line 352
	pptThreats = (THREATNODE **)D2ALLOCPOOL(ptGame->pMemPool, sizeof(THREATNODE *));
	*pptThreats = NULL;
	return pptThreats;
}

static inline void MonsterAIThreatListFree(GAME *ptGame, THREATNODE **pptThreats)
{
	THREATNODE *ptNode;
	THREATNODE *ptNext;

	for (ptNode = *pptThreats; ptNode; ptNode = ptNext)
	{
		ptNext = ptNode->ptNext;
#line 365
		D2FREEPOOL(ptGame->pMemPool, ptNode);
	}
#line 368
	D2FREEPOOL(ptGame->pMemPool, pptThreats);
}

static inline THREATNODE *MonsterAIThreatNodeNew(GAME *ptGame, UNIT *hUnit)
{
	THREATNODE *ptNode;

#line 429
	ptNode = (THREATNODE *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(THREATNODE));
	ptNode->hUnit = hUnit;
	return ptNode;
}

MONSTERDATA * __fastcall MonsterAICreate(GAME *ptGame)
{
	MONSTERDATA *ptMonster;

	ptMonster = MonsterAIAlloc(ptGame);
	ptMonster->ptAi = AiGeneralNew(ptGame);
	ptMonster->ptModeData = MonsterModeCreate(ptGame);
	ptMonster->ptUnique = MonsterUniqueAlloc(ptGame);
	ptMonster->pRegionRef = MonsterAllocRegionRef(ptGame);
	ptMonster->pptThreats = MonsterAIThreatListNew(ptGame);
	return (MONSTERDATA *)ptMonster;
}

void __fastcall MonsterAIDestroy(GAME *ptGame, MONSTERDATA *ptMonsterData)
{
	MONSTERDATA *ptMonster = ptMonsterData;

	AiGeneralFree(ptGame, ptMonster->ptAi);
	MonsterModeFree(ptGame, ptMonster->ptModeData);
	MonsterUniqueFree(ptGame, ptMonster->ptUnique);
	MonsterFreeRegionRef(ptGame, (MONSTERREGIONREF *)ptMonster->pRegionRef);
	MonsterAIThreatListFree(ptGame, ptMonster->pptThreats);
	MonsterAIFree(ptGame, ptMonster);
}

HPTRACK * __fastcall MonsterGetHpTrack(MONSTERDATA *ptMonsterData)
{
	if (ptMonsterData)
		return &ptMonsterData->tHpTrack;
	return NULL;
}

void __fastcall MonsterAIPetInit(GAME *ptGame, UNIT *hPlayer, UNIT *hPet,
	MONPETDESC *ptDesc)
{
	UNIT *hOld;
	MONPETINIT tInit;

	if (!hPlayer || !hPet || !ptDesc || UnitGetType(hPlayer) != UNIT_PLAYER)
		return;
	hPet->dwFlags0 |= 67240448;
	SUnitSetAlignment(hPet, 0);
	TargetsAddToList(ptGame, hPet, 0, hPlayer->nTargetListNo);
	hOld = PlayerPetsFind(ptGame, hPlayer, PETTYPE_HIRELING);
	if (hOld)
	{
		AiSetOwner(ptGame, hOld, (DWORD)-1, 1, 0, 0);
		PlayerPetsRemove(ptGame, hPlayer, hOld->dwId, 0);
		D2CommonDungeonRoomAddNode(D2CommonUnitGetPathRoom(hOld), 1, hOld->dwId);
		SUnitFree(ptGame, hOld);
	}
	tInit.dwSeed = ptDesc->dwSeed;
	tInit.dwHireId = ptDesc->wHireId;
	PlayerPetsAddData(ptGame, hPlayer, hPet, PETTYPE_HIRELING, (PETDATA *)&tInit);
	hPet->dwFlags0 |= 0x80000000;
	AiSetOwner(ptGame, hPet, hPlayer->dwId, hPlayer->eType, 0, 0);
	tInit.tRecord.nKind = D2CommonMonsterKillsLogClassify(0, ptDesc->wHireId);
	D2CommonMonsterKillsLogRollRecord(ptDesc->dwSeed, &tInit.tRecord, ptGame->bDifficultyLevel);
	MonsterUniqueAddMod(ptGame, hPet, 19, 0);
	D2CommonStatsUnitSetValue(hPet, 12, tInit.tRecord.nLevel);
	D2CommonStatsUnitSetValue(hPet, 31, tInit.tRecord.nDefense);
	D2CommonStatsUnitSetValue(hPet, 7, tInit.tRecord.nLife << 8);
	D2CommonStatsUnitSetValue(hPet, 6, tInit.tRecord.nLife << 8);
	if (tInit.tRecord.nHireSpecial == 4)
		SkillsSetLevel(hPet, SKILL_JAB, tInit.tRecord.nSkillLevel, 1);
	if (tInit.tRecord.nHireSpecial == 5)
		D2CommonStatsUnitSetValue(hPet, 45, 100);
	if (tInit.tRecord.nHireSpecial >= 6 && tInit.tRecord.nHireSpecial <= 11)
	{
		if (tInit.tRecord.nHireSpecial >= 9)
			AiSetParams(((MONSTERDATA *)(hPet->uTypeData))->ptAi, -666, 2, -666);
		if (tInit.tRecord.nHireSpecial == 6 || tInit.tRecord.nHireSpecial == 9)
		{
			SkillsSetLevel(hPet, SKILL_PRIMEBOLT, tInit.tRecord.nSkillLevel + 1, 1);
			SkillsSetLevel(hPet, SKILL_PRIMELIGHTNING, tInit.tRecord.nSkillLevel, 1);
			D2CommonSkillsSetNodeMode(D2CommonSkillsFindByKey(hPet, 186), 7);
			D2CommonSkillsSetNodeMode(D2CommonSkillsFindByKey(hPet, 185), 7);
		}
		if (tInit.tRecord.nHireSpecial == 7 || tInit.tRecord.nHireSpecial == 10)
		{
			SkillsSetLevel(hPet, SKILL_PRIMESPIKE, tInit.tRecord.nSkillLevel, 1);
			SkillsSetLevel(hPet, SKILL_MONFROZENARMOR, tInit.tRecord.nSkillLevel, 1);
			D2CommonSkillsSetNodeMode(D2CommonSkillsFindByKey(hPet, 189), 7);
			D2CommonSkillsSetNodeMode(D2CommonSkillsFindByKey(hPet, 209), 7);
		}
		if (tInit.tRecord.nHireSpecial == 8 || tInit.tRecord.nHireSpecial == 11)
		{
			SkillsSetLevel(hPet, SKILL_VAMPIREFIREBALL, tInit.tRecord.nSkillLevel, 1);
			SkillsSetLevel(hPet, SKILL_FETISHINFERNO, tInit.tRecord.nSkillLevel, 1);
			D2CommonSkillsSetNodeMode(D2CommonSkillsFindByKey(hPet, 169), 7);
			D2CommonSkillsSetNodeMode(D2CommonSkillsFindByKey(hPet, 178), 7);
		}
	}
	if (!(tInit.tRecord.wFlags & 1))
	{
		D2CommonStatsUnitSetValue(hPet, 23, tInit.tRecord.nDamageMin);
		D2CommonStatsUnitSetValue(hPet, 24, tInit.tRecord.nDamageMax);
	}
	else
	{
		D2CommonStatsUnitSetValue(hPet, 23, 0);
		D2CommonStatsUnitSetValue(hPet, 24, 0);
	}
	hPet->abComponentVariations[0] = (BYTE)tInit.tRecord.dwComponentHD;
	hPet->abComponentVariations[5] = (BYTE)tInit.tRecord.dwComponentRH;
	hPet->abComponentVariations[6] = (BYTE)tInit.tRecord.dwComponentLH;
	hPet->abComponentVariations[7] = (BYTE)tInit.tRecord.dwComponentSH;
	hPet->abComponentVariations[1] = (BYTE)tInit.tRecord.dwComponentTR;
}

void __fastcall MonsterAIPetLevelUp(GAME *ptGame  ,
	UNIT *hPlayer  )
{
	UNIT *hPet;
	int nPlayerLevel;
	int nPetLevel;
	int nLife;

#line 296
	D2ASSERT(ptGame);
#line 297
	D2ASSERT(hPlayer);
#line 298
	D2ASSERT(UnitGetType (hPlayer) == UNIT_PLAYER);
	hPet = PlayerPetsFind(ptGame, hPlayer, PETTYPE_HIRELING);
	if (!hPet)
		return;
	nPlayerLevel = D2CommonStatsUnitGetValue(hPlayer, 12);
	nPetLevel = D2CommonStatsUnitGetValue(hPet, 12);
	if (nPlayerLevel <= nPetLevel)
		return;
	AiThinkSay(ptGame, hPet, hPlayer, 3452, TRUE);
	D2CommonStatsUnitSetValue(hPet, 12, nPetLevel + 1);
	nLife = D2CommonStatsUnitCalcMaxHp(hPet) + 2560;
	D2CommonStatsUnitSetValue(hPet, 7, nLife);
	D2CommonStatsUnitSetValue(hPet, 6, nLife);
}

int __fastcall MonsterAICountThreats(THREATNODE **pptThreats)
{
	THREATNODE *ptNode;
	int nCount;

	nCount = 0;
	for (ptNode = *pptThreats; ptNode; ptNode = ptNode->ptNext)
	{
		if (ptNode->hUnit)
			nCount++;
	}
	return nCount;
}

BOOL __fastcall MonsterAIHasThreats(THREATNODE **pptThreats)
{
	THREATNODE *ptNode;

	for (ptNode = *pptThreats; ptNode; ptNode = ptNode->ptNext)
	{
		if (ptNode->hUnit)
			return TRUE;
	}
	return FALSE;
}

BOOL __fastcall MonsterAIIsThreat(THREATNODE **pptThreats, UNIT *hUnit)
{
	THREATNODE *ptNode;

	for (ptNode = *pptThreats; ptNode; ptNode = ptNode->ptNext)
	{
		if (ptNode->hUnit == hUnit)
			return TRUE;
	}
	return FALSE;
}

void __fastcall MonsterAIRemoveThreat(GAME *ptGame, UNIT *hUnit, THREATNODE **pptThreats)
{
	THREATNODE *ptPrev;
	THREATNODE *ptNode;

	if (!pptThreats)
		return;
	ptPrev = NULL;
	for (ptNode = *pptThreats; ptNode; ptPrev = ptNode, ptNode = ptNode->ptNext)
	{
		if (ptNode->hUnit == hUnit)
		{
			if (ptNode == *pptThreats)
				*pptThreats = ptNode->ptNext;
			else
				ptPrev->ptNext = ptNode->ptNext;
#line 527
			D2FREEPOOL(ptGame->pMemPool, ptNode);
			return;
		}
	}
}

void __fastcall MonsterAITellThreat(GAME *ptGame, UNIT *hUnit, UNIT *hMonster,
	THREATNODE **pptThreats)
{
	THREATNODE *ptNode;

	for (ptNode = *pptThreats; ptNode; ptNode = ptNode->ptNext)
	{
		if (ptNode->hUnit == hUnit)
		{
			if (!ptNode->nState)
			{
				ptNode->nState = 1;
				SUnitNpcTellThreat(ptGame, hUnit, hMonster);
			}
			return;
		}
	}
}

void __fastcall MonsterAIAckThreat(UNIT *hMonster, UNIT *hUnit)
{
	THREATNODE *ptNode;

	for (ptNode = *((MONSTERDATA *)(hMonster->uTypeData))->pptThreats; ptNode; ptNode = ptNode->ptNext)
	{
		if (ptNode->hUnit == hUnit)
		{
			if (ptNode->nState == 1)
				ptNode->nState = 2;
			return;
		}
	}
}

BOOL __fastcall MonsterAIThreatAcked(UNIT *hMonster, UNIT *hUnit)
{
	THREATNODE *ptNode;

	for (ptNode = *((MONSTERDATA *)(hMonster->uTypeData))->pptThreats; ptNode; ptNode = ptNode->ptNext)
	{
		if (ptNode->hUnit == hUnit)
			return ptNode->nState == 2;
	}
	return FALSE;
}

void __fastcall MonsterAIDropThreat(GAME *ptGame, UNIT *hUnit, UNIT *hMonster,
	THREATNODE **pptThreats)
{
	THREATNODE *ptPrev;
	THREATNODE *ptNode;

	QuestsNpcActivate(ptGame, hUnit, hMonster);
	ptPrev = NULL;
	for (ptNode = *pptThreats; ptNode; ptPrev = ptNode, ptNode = ptNode->ptNext)
	{
		if (ptNode->hUnit != hUnit)
			continue;
		if (ptNode->nState >= 1)
		{
			if (!ptPrev)
				*pptThreats = ptNode->ptNext;
			else
				ptPrev->ptNext = ptNode->ptNext;
			SUnitResetInteract(hUnit);
#line 620
			D2FREEPOOL(ptGame->pMemPool, ptNode);
			if (!*pptThreats)
				A2Q3Nothing(ptGame, hMonster);
			return;
		}
		if (ptNode->nState == 0)
		{
			if (!ptPrev)
				*pptThreats = ptNode->ptNext;
			else
				ptPrev->ptNext = ptNode->ptNext;
			SUnitResetInteract(hUnit);
#line 650
			D2FREEPOOL(ptGame->pMemPool, ptNode);
			return;
		}
	}
}

void __fastcall MonsterAIAddThreat(GAME *ptGame, UNIT *hUnit, UNIT *hMonster,
	THREATNODE **pptThreats, DWORD dwArg)
{
	THREATNODE *ptNode;
	THREATNODE *ptHead;
	CLIENT *ptClient;
	BOOL bFirst;
	DWORD dwText;
	void *pRecord;
	MONTHREATMSG tMsg;

	ptNode = *pptThreats;
	if (SUnitGetInteractUnit(ptGame, hUnit))
		return;
	if (!hMonster->dwMode || hMonster->dwMode == 12)
		return;
	if (PlayerIsBusy(hUnit) == 1)
		return;
	if (!((MONSTATSTXT *)(D2CommonMonsterTblsGetStats(hMonster->dwClassId)))->bInteract)
		return;
	if (hMonster->dwClassId == 146 && A1Q4TestCainActivated(ptGame))
		return;
	for (; ptNode; ptNode = ptNode->ptNext)
	{
		if (ptNode->hUnit == hUnit)
			return;
	}
	ptClient = SUnitGetClient(hUnit);
	ptNode = MonsterAIThreatNodeNew(ptGame, hUnit);
	ptHead = *pptThreats;
	if (ptHead == NULL)
	{
		bFirst = TRUE;
		ptNode->ptNext = ptHead;
	}
	else
	{
		bFirst = FALSE;
		ptNode->ptNext = ptHead;
	}
	*pptThreats = ptNode;
	SUnitNpcSendHires(ClientGetGame(ptClient), ptClient, hMonster, bFirst);
	SUnitSetInteract(hUnit, 1, hMonster->dwId);
	ptNode->nState = 0;
	dwText = D2CommonTextListNew(ptGame->pMemPool);
	QuestsNpcMessage(ptClient, hUnit, hMonster, dwText);
	tMsg.bCmd = 32;
	tMsg.dwUnitId = hMonster->dwId;
	tMsg.bUnitType = (BYTE)hMonster->eType;
	D2CommonTextListPack(dwText, tMsg.abText);
	SCmdSendUnitMessage(ptClient, (const SCMDSYSTEMMSG *)&tMsg);
	D2CommonTextListFree(dwText);
	QuestsSendRecord(ptGame, ptClient);
	pRecord = ((PLRINFO *)D2CommonUnitGetPlrInfo(hUnit))->apQuestRecords[ptGame->bDifficultyLevel];
	SCmdSendQuests(ptClient, S2C_NPC_DIALOG, 1, hMonster->dwId, pRecord, (BYTE)dwArg);
}

BOOL __fastcall MonsterAISetTargetCmd(UNIT *hMonster  ,
	int eTargetCmd  , DWORD dwArg)
{
	MONSTERDATA *ptMonster;

#line 682
	D2ASSERT(UnitGetType (hMonster) == UNIT_MONSTER);
	if (!((MONSTATSTXT *)(D2CommonMonsterTblsGetStats(hMonster->dwClassId)))->bSwitch)
		return FALSE;
#line 690
	D2ASSERT(hMonster);
	ptMonster = ((MONSTERDATA *)(hMonster->uTypeData));
#line 692
	D2ASSERT(ptMonster);
#line 693
	D2ASSERT(eTargetCmd < NUM_MONSTERTARGETS);
	ptMonster->eTargetCmd = eTargetCmd;
	ptMonster->dwTargetCmdArg = dwArg;
	return TRUE;
}

int __fastcall MonsterAIGetTargetCmd(UNIT *hMonster  )
{
	MONSTERDATA *ptMonster;

#line 710
	D2ASSERT(hMonster);
	ptMonster = ((MONSTERDATA *)(hMonster->uTypeData));
#line 712
	D2ASSERT(ptMonster);
	return ptMonster->eTargetCmd;
}

DWORD __fastcall MonsterAIGetTargetCmdArg(UNIT *hMonster  )
{
	MONSTERDATA *ptMonster;

#line 725
	D2ASSERT(hMonster);
	ptMonster = ((MONSTERDATA *)(hMonster->uTypeData));
#line 727
	D2ASSERT(ptMonster);
	return ptMonster->dwTargetCmdArg;
}

void __fastcall MonsterAIClearTargetCmd(UNIT *hMonster  )
{
	MONSTERDATA *ptMonster;

#line 736
	D2ASSERT(UnitGetType (hMonster) == UNIT_MONSTER);
#line 742
	D2ASSERT(hMonster);
	ptMonster = ((MONSTERDATA *)(hMonster->uTypeData));
#line 744
	D2ASSERT(ptMonster);
	ptMonster->eTargetCmd = 0;
	ptMonster->dwTargetCmdArg = 0;
}

BOOL __fastcall MonsterAIThreatTold(UNIT *hMonster  , UNIT *hUnit)
{
	MONSTERDATA *ptMonster;
	THREATNODE *ptNode;

#line 761
	D2ASSERT(hMonster);
	ptMonster = ((MONSTERDATA *)(hMonster->uTypeData));
#line 763
	D2ASSERT(ptMonster);
	for (ptNode = *ptMonster->pptThreats; ptNode; ptNode = ptNode->ptNext)
	{
		if (ptNode->hUnit == hUnit)
			return ptNode->nState >= 1;
	}
#line 784
	D2WARN("something really weird happened in MonsterRecivedClientAck");
	return TRUE;
}

void __fastcall MonsterAIClearThreats(GAME *ptGame, UNIT *hMonster,
	PFNTHREATCALLBACK pfnCallback)
{
	MONSTERDATA *ptMonster;
	THREATNODE **pptThreats;
	THREATNODE *ptNode;
	THREATNODE *ptNext;

	ptMonster = ((MONSTERDATA *)(hMonster->uTypeData));
#line 793
	D2ASSERT(ptMonster);
	pptThreats = ptMonster->pptThreats;
	for (ptNode = *pptThreats; ptNode; ptNode = ptNext)
	{
		ptNext = ptNode->ptNext;
		SCmdSendInteractionStop(SUnitGetClient(ptNode->hUnit), 91, 1, hMonster->dwId, 0);
		SUnitResetInteract(ptNode->hUnit);
		if (pfnCallback)
			pfnCallback(ptNode->hUnit);
#line 808
		D2FREEPOOL(ptGame->pMemPool, ptNode);
	}
	*pptThreats = NULL;
}

int __fastcall MonsterRegionPickClass(MONREGION *ptRegion, GAMEROOM *hRoom, MONSTATSTXT **pptStats,
	DWORD nPercent, BOOL bUnique)
{
	D2SEED *ptSeed;
	MISSLEVELDATA *ptLevel;
	MONPOPSLOT *ptSlot;
	int nIndex;
	int nClass;
	int nRoll;
	int i;
	DWORD nRarity;
	DWORD nTotal;

	if (hRoom)
		ptSeed = &ROOM5(hRoom)->tSeed;
	else
		ptSeed = NULL;
	if (bUnique)
	{
		ptLevel = (MISSLEVELDATA *)D2CommonLvlTblsGetLevelData(ptRegion->nLevelId);
		if (ptLevel->Utot)
		{
			nTotal = ptLevel->Utot;
			if (nTotal)
				nIndex = (int)D2CommonSeedRand(ptSeed, nTotal);
			else
				nIndex = 0;
			nClass = ptLevel->U[nIndex];
			*pptStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(nClass);
			return nClass;
		}
		*pptStats = NULL;
		return 0;
	}
	if (!ptRegion->bNumLevelSlots)
	{
		*pptStats = NULL;
		return 0;
	}
	nRarity = ptRegion->bRarity;
	nRoll = (nRarity ? (int)D2CommonSeedRand(ptSeed, nRarity) : 0) + 1;
	ptSlot = ptRegion->atSlots;
	for (i = 0; i < ptRegion->bNumLevelSlots; i++)
	{
		nRoll -= ptSlot->bRarity;
		if (nRoll <= 0)
			break;
		ptSlot++;
	}
	if (ptSlot->dwWeight && D2CommonSeedRand(ptSeed, 100) <= (BYTE)nPercent)
		nClass = ptSlot->dwWeight;
	else
		nClass = ptSlot->nClass;
	*pptStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(nClass);
	return nClass;
}
