#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\SKILLS\\SkillSor.cpp"
#include "D2Macros.h"
#include "../AI/AiGeneral.h"
#include "../AI/AiThink.h"
#include "../GAME/Event.h"
#include "../ITEMS/ItemMode.h"
#include "../MISSILES/Missiles.h"
#include "../OBJECTS/ObjMode.h"
#include "../PLAYER/Player.h"
#include "../PLAYER/PlrMsg.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "SkillAma.h"
#include "Skills.h"
#include "SkillSor.h"

#define SKILL_SORCERESS_START	36
#define NUM_SORCERESS_SKILLS	30
#define MONSTER_HYDRA2			352
#define MONSTER_HYDRA3			353
#define STATE_FROZENARMOR		10
#define STATE_INFERNO			12
#define STATE_BLAZE				13
#define STATE_ENCHANT			16
#define STATE_CHILLINGARMOR		20
#define STATE_ENERGYSHIELD		30
#define STATE_THUNDERSTORM		38
#define STATE_THUNDERSTRUCK		39
#define STATE_FIREMASTERY		71
#define STATE_SHIVERARMOR		88
#define NUM_ARMOR_STATES		3
#define EVENT_PERIODIC			8
#define EVENT_STATEEND			12
#define MISSFLAG_POSITION		1
#define MISSFLAG_TARGETABS		32
#define MISSFLAG_RANGE			32768
#define UNIT_PLAYER				0
#define UNIT_MONSTER			1
#define UNIT_OBJECT				2
#define UNIT_MISSILE			3
#define UNIT_ITEM				4
#define PLAYERCLASS_SORCERESS	1

static const DWORD sganArmorStates[NUM_ARMOR_STATES] =
{
	STATE_FROZENARMOR, STATE_SHIVERARMOR, STATE_CHILLINGARMOR,
};

static inline void SkillsSorRemoveArmors(UNIT *hUnit)
{
	DWORD hList;
	int i;

	for (i = 0; i < NUM_ARMOR_STATES; i++)
	{
		hList = D2CommonStatsFindListByState(hUnit, sganArmorStates[i]);
		if (hList)
		{
			D2CommonStatsUnitUnlinkList(hUnit, hList);
			D2CommonStatsListFree(hList);
		}
	}
}

BOOL __fastcall SkillsInferno(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreate;
	int nX;
	int nY;
	int nRange;
	int i;

	if (!SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY))
		return FALSE;
	nRange = D2CommonSkillsCalcParamOneByLevel(SKILL_INFERNO, nLevel);
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = MISSFLAG_RANGE | MISSFLAG_TARGETABS;
	tCreate.eMissile = MISSILE_INFERNOFLAME1;
	tCreate.hSource = hUnit;
	tCreate.hSpawnLocation = hUnit;
	tCreate.nLevel = nLevel;
	tCreate.nDX = nX;
	tCreate.nDY = nY;
	tCreate.nRange = nRange >> 1;
	MissilesCreate(ptGame, &tCreate);
	if (D2CommonStatsGetState(hUnit, STATE_INFERNO))
	{
		D2CommonUnitSetAnimSequencePos(hUnit, 8);
		hUnit->nAnimLength = 1280;
	}
	return TRUE;
}

BOOL __fastcall SkillsSorInfernoStart(GAME *ptGame, UNIT *hUnit, int nLevel, int nMissile)
{
	MISSILECREATE tCreate;
	DWORD hState;
	int nEnd;
	int nX;
	int nY;
	int nRange;
	int i;

	hState = D2CommonStatsFindListByState(hUnit, STATE_INFERNO);
	if (!hState)
	{
		nEnd = ptGame->nFrame + 20;
		hState = D2CommonStatsAllocList(ptGame->pMemPool, 2, nEnd, hUnit->eType, hUnit->dwId);
		if (!hState)
			return FALSE;
		EventAdd(ptGame, hUnit, EVENT_STATEEND, nEnd);
		D2CommonStatsUnitLinkList(hUnit, hState);
		D2CommonStatsSetListRemoveCallback(hState, PlrMsgStateEnd);
		D2CommonStatsSetListState(hState, STATE_INFERNO);
		D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_INFERNO, 1);
		return TRUE;
	}
	D2CommonUnitSetAnimSequencePos(hUnit, 8);
	hUnit->nAnimLength = 1280;
	nEnd = ptGame->nFrame + 6;
	D2CommonStatsSetListEndFrame(hState, nEnd);
	EventAdd(ptGame, hUnit, EVENT_STATEEND, nEnd);
	if (SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY))
	{
		nRange = D2CommonSkillsCalcParamOneByLevel(SKILL_INFERNO, nLevel);
		for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.eMissile = nMissile;
		tCreate.nDX = nX;
		tCreate.dwFlags = MISSFLAG_RANGE | MISSFLAG_TARGETABS;
		tCreate.hSource = hUnit;
		tCreate.hSpawnLocation = hUnit;
		tCreate.nLevel = nLevel;
		tCreate.nDY = nY;
		tCreate.nRange = nRange >> 1;
		MissilesCreate(ptGame, &tCreate);
		if (D2CommonStatsGetState(hUnit, STATE_INFERNO))
		{
			D2CommonUnitSetAnimSequencePos(hUnit, 8);
			hUnit->nAnimLength = 1280;
		}
	}
	return TRUE;
}

BOOL __fastcall SkillsInfernoStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	return SkillsSorInfernoStart(ptGame, hUnit, nLevel, MISSILE_INFERNOFLAME1);
}

BOOL __fastcall SkillsFrozenArmor(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLSTATE tData;
	int i;

	SkillsSorRemoveArmors(hUnit);
	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tData)[i] = 0;
	tData.hSource = hUnit;
	tData.hTarget = hUnit;
	tData.nSkill = SKILL_FROZEN_ARMOR;
	tData.nLevel = nLevel;
	tData.nDuration = D2CommonSkillsCalcParamThreeByLevel(SKILL_FROZEN_ARMOR, nLevel);
	tData.dwStatId = 171;
	tData.nStatValue = D2CommonSkillsCalcParamOneByLevel(SKILL_FROZEN_ARMOR, nLevel);
	tData.dwState = STATE_FROZENARMOR;
	return (BOOL)SkillsSetState(&tData);
}

void __fastcall SkillsFrozenArmorHit(GAME *ptGame, UNIT *hUnit, UNIT *hTarget)
{
	DAMAGE tDamage;
	int nSkill;
	int nLevel;
	int i;

	if (hTarget->eType != UNIT_PLAYER && hTarget->eType != UNIT_MONSTER)
		return;
	if (hUnit->eType)
		nSkill = SKILL_MONFROZENARMOR;
	else
		nSkill = SKILL_FROZEN_ARMOR;
	nLevel = (int)D2CommonSkillsCalcNodeLevel(hUnit, D2CommonSkillsFindByKey(hUnit, nSkill), 1);
	if (!nLevel)
		return;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.nFreezeLength = D2CommonSkillsCalcParamFiveByLevel(nSkill, nLevel);
	SUnitDmgApply(ptGame, hUnit, hTarget, TRUE, &tDamage);
	D2CommonUnitAddAnimOverlay(hUnit, 42, 0);
}

BOOL __fastcall SkillsShiverArmor(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLSTATE tData;
	int i;

	SkillsSorRemoveArmors(hUnit);
	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tData)[i] = 0;
	tData.hSource = hUnit;
	tData.hTarget = hUnit;
	tData.nSkill = SKILL_SHIVER_ARMOR;
	tData.nLevel = nLevel;
	tData.nDuration = D2CommonSkillsCalcParamThreeByLevel(SKILL_SHIVER_ARMOR, nLevel);
	tData.dwStatId = 171;
	tData.nStatValue = D2CommonSkillsCalcParamOneByLevel(SKILL_SHIVER_ARMOR, nLevel);
	tData.dwState = STATE_SHIVERARMOR;
	return (BOOL)SkillsSetState(&tData);
}

WORD __fastcall SkillsShiverArmorHit(GAME *ptGame, UNIT *hUnit, UNIT *hTarget  )
{
	DAMAGE tDamage;
	DWORD ptSkill;
	int nLevel;
	int i;

	ptSkill = D2CommonSkillsFindByKey(hUnit, SKILL_SHIVER_ARMOR);
	if (!ptSkill)
		return 0;
	nLevel = (int)D2CommonSkillsCalcNodeLevel(hUnit, ptSkill, 1);
#line 285
	D2ASSERT(hTarget);
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	SkillsElemDamage(hUnit, &tDamage, SKILL_SHIVER_ARMOR, nLevel);
	tDamage.dwHitClass = 61;
	tDamage.dwFixedHitClass = 1;
	tDamage.wFlags = 16385;
	SUnitDmgApply(ptGame, hUnit, hTarget, TRUE, &tDamage);
	SUnitDmgReact(ptGame, hUnit, hTarget, &tDamage);
	D2CommonUnitAddAnimOverlay(hUnit, 43, 0);
	return tDamage.wFlags;
}

BOOL __fastcall SkillsTeleport(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DWORD hRoom;
	int nX;
	int nY;

	SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY);
	hRoom = D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	if (D2CommonDungeonRoomGetLevelId(hRoom) == 73
		&& D2CommonUnitTestLineBlockedToPoint(hUnit, nX, nY, 2052))
		return FALSE;
	return SUnitMoveToRoom(ptGame, hUnit, NULL, nX, nY, FALSE, FALSE);
}

BOOL __fastcall SkillsMeteor(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;

	SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY);
	if (D2CommonCollisnTestPoint(D2CommonUnitGetPathRoom(hUnit), nX, nY, 1))
		return FALSE;
	SkillsCreateMissileAt(ptGame, hUnit, nLevel, MISSILE_METEORCENTER, 0, 0);
	return TRUE;
}

BOOL __fastcall SkillsThunderStormStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DWORD ptSkill;

	ptSkill = D2CommonSkillsFindByKey(hUnit, SKILL_THUNDER_STORM);
	if (!ptSkill)
		return FALSE;
	D2CommonSkillsSetNodeScratchC(ptSkill, 0xFFFFFFFF);
	D2CommonSkillsSetNodeScratchD(ptSkill, 1);
	EventRemoveByType(ptGame, hUnit, EVENT_PERIODIC);
	return TRUE;
}

BOOL __fastcall SkillsThunderStorm(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLSTATE tData2;
	SKILLSTATE tData;
	SKILLSTXT *ptSkillsTxt;
	DWORD ptSkill;
	UNIT *hTarget;
	int nPercent;
	int i;

	ptSkill = D2CommonSkillsFindByKey(hUnit, SKILL_THUNDER_STORM);
	if (!ptSkill)
		return FALSE;
	if (D2CommonStatsGetState(hUnit, STATE_THUNDERSTORM) && !D2CommonSkillsGetNodeScratchD(ptSkill))
	{
		hTarget = SkillsFindNext(ptGame, hUnit, D2CommonSkillsGetNodeScratchC(ptSkill), 15, 0);
		if (hTarget)
		{
			if (!hUnit || (!D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hUnit))
				&& !D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hTarget))))
			{
				for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
					((DWORD *)&tData)[i] = 0;
				tData.hSource = hUnit;
				tData.hTarget = hTarget;
				tData.nSkill = SKILL_THUNDER_STORM;
				tData.nLevel = nLevel;
				tData.nDuration = 2;
				tData.dwStatId = 31;
				tData.nStatValue = 0;
				tData.dwState = STATE_THUNDERSTRUCK;
				SkillsSetState(&tData);
				D2CommonUnitAddAnimOverlay(hTarget, 1, 0);
				SkillsCreateMissileAt(ptGame, hUnit, nLevel, MISSILE_THUNDERSTORM1, (int)D2CommonUnitGetX(hTarget),
					(int)D2CommonUnitGetY(hTarget));
			}
			D2CommonSkillsSetNodeScratchC(ptSkill, hTarget->dwId);
		}
		else
			D2CommonSkillsSetNodeScratchC(ptSkill, 0xFFFFFFFF);
	}
	else
	{
		for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
			((DWORD *)&tData2)[i] = 0;
		tData2.hSource = hUnit;
		tData2.hTarget = hUnit;
		tData2.nSkill = SKILL_THUNDER_STORM;
		tData2.nLevel = nLevel;
		tData2.nDuration = D2CommonSkillsCalcParamOneByLevel(SKILL_THUNDER_STORM, nLevel);
		tData2.dwStatId = 31;
		tData2.nStatValue = 0;
		tData2.dwState = STATE_THUNDERSTORM;
		SkillsSetState(&tData2);
	}
	D2CommonSkillsSetNodeScratchD(ptSkill, 0);
	ptSkillsTxt = (SKILLSTXT *)D2CommonDataTblsGetSkill(SKILL_THUNDER_STORM);
	nPercent = 100 - D2CommonSkillsCalcCurveValue(nLevel, ptSkillsTxt->Param[4],
		ptSkillsTxt->Param[5]);
	EventAdd(ptGame, hUnit, EVENT_PERIODIC,
		ptSkillsTxt->Param[3] * nPercent / 100 + ptSkillsTxt->Param[2] + ptGame->nFrame);
	return TRUE;
}

BOOL __fastcall SkillsEnergyShield(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLSTATE tData;
	int i;

	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tData)[i] = 0;
	tData.hSource = hUnit;
	tData.hTarget = hUnit;
	tData.nSkill = SKILL_ENERGY_SHIELD;
	tData.nLevel = nLevel;
	tData.nDuration = D2CommonSkillsCalcParamOneByLevel(SKILL_ENERGY_SHIELD, nLevel);
	tData.dwStatId = 31;
	tData.nStatValue = 0;
	tData.dwState = STATE_ENERGYSHIELD;
	SkillsSetState(&tData);
	return TRUE;
}

BOOL __fastcall SkillsBlizzard(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	int nX;
	int nY;

	SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY);
	if (D2CommonCollisnTestPoint(D2CommonUnitGetPathRoom(hUnit), nX, nY, 1))
		return FALSE;
	SkillsCreateMissileAt(ptGame, hUnit, nLevel, MISSILE_BLIZZARDCENTER, 0, 0);
	return TRUE;
}

BOOL __fastcall SkillsChillingArmor(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLSTATE tData;
	void *hList;
	int i;

	SkillsSorRemoveArmors(hUnit);
	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tData)[i] = 0;
	tData.hSource = hUnit;
	tData.hTarget = hUnit;
	tData.nSkill = SKILL_CHILLING_ARMOR;
	tData.nLevel = nLevel;
	tData.nDuration = D2CommonSkillsCalcParamThreeByLevel(SKILL_CHILLING_ARMOR, nLevel);
	tData.dwStatId = 171;
	tData.nStatValue = D2CommonSkillsCalcParamOneByLevel(SKILL_CHILLING_ARMOR, nLevel);
	tData.dwState = STATE_CHILLINGARMOR;
	hList = SkillsSetState(&tData);
	if (hList)
		D2CommonStatsListSetValueB(hList, 168, nLevel);
	return (BOOL)hList;
}

BOOL __fastcall SkillsHydraStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DWORD hRoom;
	int nX;
	int nY;

	hRoom = D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	if (!SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY))
		return FALSE;
	hRoom = (DWORD)D2CommonDungeonFindRoomGame(hRoom, nX, nY);
	if (!hRoom)
		return FALSE;
	if (D2CommonDungeonRoomIsTown(hRoom))
		return FALSE;
	else
		return TRUE;
}

BOOL __fastcall SkillsHydra(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLSUMMON tSummon;
	int anClass[3];
	int anOffsetX[3];
	int anOffsetY[3];
	UNIT *hPet;
	DWORD hRoom;
	BOOL bResult;
	int nX;
	int nY;
	int nEnd;
	int i;

	anClass[0] = MONSTER_HYDRA;
	anClass[1] = MONSTER_HYDRA2;
	anClass[2] = MONSTER_HYDRA3;
	anOffsetX[0] = -1;
	anOffsetX[1] = 0;
	anOffsetX[2] = 1;
	anOffsetY[0] = -1;
	anOffsetY[1] = 0;
	anOffsetY[2] = -1;
	hRoom = D2CommonUnitGetPathRoom(hUnit);
	if (!hRoom)
		return FALSE;
	if (!SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY))
		return FALSE;
	hRoom = (DWORD)D2CommonDungeonFindRoomGame(hRoom, nX, nY);
	if (!hRoom)
		return FALSE;
	if (D2CommonDungeonRoomIsTown(hRoom))
		return FALSE;
	nEnd = D2CommonSkillsCalcParamOneByLevel(SKILL_HYDRA, nLevel) + ptGame->nFrame;
	for (i = 0; i < (int)(sizeof(SKILLSUMMON) / sizeof(DWORD)); i++)
		((DWORD *)&tSummon)[i] = 0;
	tSummon.dwFlags |= 1;
	tSummon.hOwner = hUnit;
	tSummon.dwAiKind = 0;
	tSummon.dwMode = 9;
	tSummon.dwPetType = 0;
	bResult = FALSE;
	for (i = 0; i < 3; i++)
	{
		tSummon.dwClassId = anClass[i];
		tSummon.nX = anOffsetX[i] + nX;
		tSummon.nY = anOffsetY[i] + nY;
		hPet = SkillsSummon(ptGame, &tSummon);
		if (hPet)
		{
			bResult = TRUE;
			if (((MONSTERDATA *)hPet->uTypeData)->ptAi)
				AiSetParams(((MONSTERDATA *)hPet->uTypeData)->ptAi, nEnd, -666, -666);
			if (hUnit->eType == UNIT_MONSTER)
				SUnitSetAlignment(hPet, 2);
		}
	}
	return bResult;
}

BOOL __fastcall SkillsFrozenOrb(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hMissile;

	hMissile = SkillsCreateMissile(ptGame, MISSILE_FROZENORB, hUnit, nLevel, 0, 0, 0, 0, 0);
	if (hMissile)
		D2CommonMissileSetScratchFirst(hMissile, 0);
	return TRUE;
}

BOOL __fastcall SkillsSorNova(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsNova(ptGame, hUnit, hUnit, MISSILE_NOVA, nLevel, D2CommonMissileTblsGetVel(MISSILE_NOVA));
	return TRUE;
}

void __fastcall SkillsChargedBoltModify(UNIT *hMissile, DWORD dwArg)
{
	void *ptPath;
	int nRange;

	if (!hMissile)
		return;
	nRange = (int)D2CommonMissileGetRange(hMissile);
	if (nRange >= 100)
	{
		nRange = 99;
		D2CommonMissileSetRange(hMissile, nRange);
	}
	ptPath = hMissile->ptPath;
	D2CommonSeedSet(&hMissile->tSeed, D2CommonPathGetTargetXRaw((DWORD)ptPath) + dwArg);
	D2CommonPathSetType(ptPath, 10);
	D2CommonPathSetDistMax(ptPath, nRange);
	D2CommonPathCompute(ptPath, hMissile);
}

BOOL __fastcall SkillsChargedBolt(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreateMissile;
	int nCount;
	int i;
	int j;

	hUnit->dwFlags0 |= 64;
	nCount = D2CommonSkillsCalcParamOneByLevel(SKILL_CHARGED_BOLT, nLevel);
	for (j = 0; j < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); j++)
		((DWORD *)&tCreateMissile)[j] = 0;
	tCreateMissile.dwFlags = MISSFLAG_POSITION | MISSFLAG_TARGETABS;
	tCreateMissile.hSource = hUnit;
	tCreateMissile.nX = (int)D2CommonUnitGetX(hUnit);
	tCreateMissile.nY = (int)D2CommonUnitGetY(hUnit);
	tCreateMissile.nLevel = nLevel;
	tCreateMissile.eMissile = MISSILE_CHARGEDBOLT;
	tCreateMissile.pfnModify = SkillsChargedBoltModify;
	for (i = 0; i < nCount; i++)
	{
		tCreateMissile.dwModifyArg = (DWORD)i;
		if (SkillsGetTargetSpot(ptGame, hUnit, &tCreateMissile.nDX, &tCreateMissile.nDY))
		{
#line 718
			D2ASSERT(tCreateMissile.nDX && tCreateMissile.nDY);
			MissilesCreate(ptGame, &tCreateMissile);
		}
	}
	return TRUE;
}

BOOL __fastcall SkillsBlaze(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLSTATE tData;
	int i;

	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tData)[i] = 0;
	tData.hSource = hUnit;
	tData.hTarget = hUnit;
	tData.nSkill = SKILL_BLAZE;
	tData.nLevel = nLevel;
	tData.nDuration = D2CommonSkillsCalcCurveParamOne(nLevel, SKILL_BLAZE);
	tData.dwStatId = 0xFFFFFFFF;
	tData.nStatValue = nLevel;
	tData.dwState = STATE_BLAZE;
	SkillsSetState(&tData);
	return TRUE;
}

void __fastcall SkillsBlazeStep(GAME *ptGame, UNIT *hUnit)
{
	MISSILECREATE tCreate;
	DWORD hRoom;
	int nLevel;
	int nX;
	int nY;
	int i;
	void *ptPath;

	ptPath = hUnit->ptPath;
	if (!D2CommonStepTestSquareCrossed(ptPath))
		return;
	if (hUnit->eType == UNIT_MONSTER)
		nLevel = (int)D2CommonSkillsCalcNodeLevel(hUnit, D2CommonSkillsFindByKey(hUnit, SKILL_PRIMEBLAZE), 1);
	else
		nLevel = (int)D2CommonSkillsCalcNodeLevel(hUnit, D2CommonSkillsFindByKey(hUnit, SKILL_BLAZE), 1);
	hRoom = D2CommonUnitGetPathRoom(hUnit);
	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	if (D2CommonDungeonRoomIsTown(hRoom))
		return;
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = MISSFLAG_POSITION | MISSFLAG_TARGETABS;
	tCreate.hSource = hUnit;
	tCreate.nX = nX;
	tCreate.nY = nY;
	tCreate.nLevel = nLevel;
	tCreate.eMissile = MISSILE_BLAZE;
	MissilesCreate(ptGame, &tCreate);
}

BOOL __fastcall SkillsFireWall(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	MISSILECREATE tCreate;
	UNIT *hTarget;
	DWORD hRoom;
	int nX;
	int nY;
	int nCentreX;
	int nCentreY;
	int nDX;
	int nDY;
	int i;

	SkillsGetTargetSpot(ptGame, hUnit, &nX, &nY);
	if (D2CommonCollisnTestPoint(D2CommonUnitGetPathRoom(hUnit), nX, nY, 1))
		return FALSE;
	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget)
	{
		nCentreX = (int)D2CommonUnitGetX(hTarget);
		nCentreY = (int)D2CommonUnitGetY(hTarget);
	}
	else
	{
		nCentreX = (int)D2CommonPathGetTargetXRaw((DWORD)hUnit->ptPath);
		nCentreY = (int)D2CommonPathGetTargetYRaw((DWORD)hUnit->ptPath);
	}
	hRoom = D2CommonUnitGetPathRoom(hUnit);
	if (!D2CommonDungeonRoomIsTown(D2CommonDungeonFindRoomGame(hRoom, nCentreX, nCentreY)))
	{
		for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.dwFlags = MISSFLAG_POSITION | MISSFLAG_TARGETABS;
		tCreate.hSource = hUnit;
		tCreate.nX = nCentreX;
		tCreate.nY = nCentreY;
		tCreate.nLevel = nLevel;
		tCreate.eMissile = MISSILE_FIREWALLMAKER;
		nDX = (int)D2CommonUnitGetX(hUnit) - nCentreX;
		nDY = (int)D2CommonUnitGetY(hUnit) - nCentreY;
		tCreate.nDX = nCentreX - nDY;
		tCreate.nDY = nDX + nCentreY;
		MissilesCreate(ptGame, &tCreate);
		tCreate.nDY = nCentreY - nDX;
		tCreate.nDX = nDY + nCentreX;
		MissilesCreate(ptGame, &tCreate);
		tCreate.eMissile = MISSILE_FIREWALL;
		tCreate.dwFlags = MISSFLAG_POSITION;
		tCreate.nDX = 0;
		tCreate.nDY = 0;
		MissilesCreate(ptGame, &tCreate);
	}
	return TRUE;
}

BOOL __fastcall SkillsEnchant(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SKILLSTATE tData;
	UNIT *hTarget;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (hTarget && !SUnitIsAlly(ptGame, hUnit, hTarget))
		hTarget = hUnit;
	if (!hTarget)
		hTarget = hUnit;
	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tData)[i] = 0;
	tData.hTarget = hTarget;
	tData.hSource = hUnit;
	tData.nSkill = SKILL_ENCHANT;
	tData.nLevel = nLevel;
	tData.nDuration = D2CommonSkillsCalcParamOneByLevel(SKILL_ENCHANT, nLevel);
	tData.dwStatId = 165;
	tData.nStatValue = nLevel;
	tData.dwState = STATE_ENCHANT;
	SkillsSetState(&tData);
	return TRUE;
}

BOOL __fastcall SkillsChainLightning(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hMissile;
	DWORD dwCount;

	dwCount = D2CommonSkillsGetTypeParamTwo(SKILL_CHAIN_LIGHTNING);
	hUnit->dwFlags0 |= 64;
	hMissile = SkillsCreateMissile(ptGame, MISSILE_CHAINLIGHTNING, hUnit, nLevel, 0, 0, 0, 0, 0);
	if (hMissile)
		D2CommonMissileSetScratchFirst(hMissile, dwCount);
	return TRUE;
}

BOOL __fastcall SkillsStaticField(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DWORD pRoom;
	UNITFINDARGS tArgs;
	UNITFINDDATA tFind;
	DAMAGE tDamage;
	SKILLSTXT *ptSkill;
	int nX;
	int nY;
	int nRange;
	int nMin;
	int nPercent;
	int nDamage;
	int i;
	int j;

	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	ptSkill = (SKILLSTXT *)D2CommonDataTblsGetSkill(SKILL_STATIC_FIELD);
#line 956
	D2ASSERT(ptSkill);
	nRange = D2CommonSkillsCalcParamOneByLevel(SKILL_STATIC_FIELD, nLevel);
	nPercent = ptSkill->Param[3];
	nMin = ptSkill->Param[2];
	for (i = 0; i < (int)(sizeof(UNITFINDARGS) / sizeof(DWORD)); i++)
		((DWORD *)&tArgs)[i] = 0;
	tArgs.dwFlags = 1923;
	tArgs.hExcluded = hUnit;
	tArgs.nX = nX;
	tArgs.nY = nY;
	tArgs.nDistance = nRange;
	tArgs.pFieldPair = (void *)D2CommonFieldTblsAllocPair();
	D2CommonFieldTblsSetPair(tArgs.pFieldPair, nX, nY);
	pRoom = D2CommonUnitGetPathRoom(hUnit);
	D2CommonUnitFindsInit(ptGame->pMemPool, &tFind, pRoom, nX, nY,
		nRange, 0, &tArgs);
	D2CommonUnitFindsFindUnits(&tFind);
	D2CommonFieldTblsFreePair(tArgs.pFieldPair);
	for (i = 0; i < tFind.nNumUnits; i++)
	{
		if (!SUnitIsHostile(ptGame, hUnit, tFind.phUnitsArray[i]))
			continue;
		nDamage = D2CommonStatsUnitGetValue(tFind.phUnitsArray[i], 6) * nPercent / 100;
		if (nDamage < nMin)
			nDamage = nMin;
		for (j = 0; j < (int)(sizeof(DAMAGE) / sizeof(DWORD)); j++)
			((DWORD *)&tDamage)[j] = 0;
		tDamage.nLightningDamage = nDamage;
		tDamage.dwHitClass = 77;
		tDamage.dwFixedHitClass = 1;
		tDamage.wFlags = 16385;
		SUnitDmgApply(ptGame, hUnit, tFind.phUnitsArray[i], TRUE, &tDamage);
		SUnitDmgReact(ptGame, hUnit, tFind.phUnitsArray[i], &tDamage);
	}
	D2CommonUnitFindsFreeArray(&tFind);
	return TRUE;
}

BOOL __fastcall SkillsTelekinesisStart(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	UNIT *hTarget;
	int nDistance;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	if (!hTarget)
		return FALSE;
	nDistance = (int)D2CommonUnitGetProximityMetric(hUnit, hTarget);
	return nDistance <= (int)D2CommonSkillsGetTypeParamOne(SKILL_TELEKINESIS);
}

BOOL __fastcall SkillsTelekinesis(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	DAMAGE tDamage;
	UNIT *hTarget;
	BOOL bKnockBack;
	int nResult;
	int nRoll;
	int i;

	hTarget = SUnitGetTarget(ptGame, hUnit);
	nResult = 0;
	bKnockBack = FALSE;
	if (!hTarget)
		return FALSE;
	if (PlayerIsBusy(hUnit))
		return FALSE;
	hUnit->dwFlags0 |= 64;
	switch (hTarget->eType)
	{
	case UNIT_PLAYER:
	case UNIT_MONSTER:
		if (!SUnitIsHostile(ptGame, hUnit, hTarget))
			return FALSE;
		if (D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hTarget)))
			return FALSE;
		if (D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hUnit)))
			return FALSE;
		nRoll = (int)D2CommonSeedRand(&hUnit->tSeed, 100);
		if (nRoll < (int)D2CommonSkillsGetTypeParamTwo(SKILL_TELEKINESIS))
			bKnockBack = TRUE;
		for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
			((DWORD *)&tDamage)[i] = 0;
		SkillsElemDamage(hUnit, &tDamage, SKILL_TELEKINESIS, nLevel);
		if (bKnockBack)
			tDamage.wFlags = 9;
		SUnitDmgApply(ptGame, hUnit, hTarget, TRUE, &tDamage);
		SUnitDmgReact(ptGame, hUnit, hTarget, &tDamage);
		if (bKnockBack)
		{
			D2CommonUnitAddAnimOverlay(hTarget, 148, 0);
			return TRUE;
		}
		break;
	case UNIT_ITEM:
		ITEMSModePickupToInventory(ptGame, hUnit, hTarget->dwId, &nResult);
		return TRUE;
	case UNIT_OBJECT:
		ObjectModeOperate(ptGame, hUnit, (WORD)hTarget->eType, hTarget->dwId);
		break;
	case UNIT_MISSILE:
	default:
		break;
	}
	return TRUE;
}

BOOL __fastcall SkillsFrostNova(GAME *ptGame, UNIT *hUnit, int nLevel)
{
	SkillsNova(ptGame, hUnit, hUnit, MISSILE_FROSTNOVA, nLevel, D2CommonMissileTblsGetVel(MISSILE_FROSTNOVA));
	return TRUE;
}

void __fastcall SkillsEnergyShieldAbsorb(UNIT *hAttacker, UNIT *hUnit,
	DAMAGE *ptDamage  )
{
	DWORD hList;
	int nPercent;
	int nMana;
	int nCost;
	int nOverlay;

	nPercent = D2CommonSkillsCalcCurveParamThree((int)D2CommonSkillsCalcNodeLevel(hUnit,
		D2CommonSkillsFindByKey(hUnit, SKILL_ENERGY_SHIELD), 1), SKILL_ENERGY_SHIELD);
	nMana = nPercent * ptDamage->nTotalDamage / 100;
	ptDamage->nTotalDamage -= nMana;
	nCost = (int)D2CommonSkillsGetTypeParamFive(SKILL_ENERGY_SHIELD) / 4 * nMana;
	nMana = D2CommonStatsUnitGetValue(hUnit, 8);
	if (nMana >= nCost)
		nMana -= nCost;
	else
	{
		ptDamage->nTotalDamage += (nCost - nMana) / 2;
#line 1154
		D2WARN(ptDamage->nTotalDamage >= 0);
		nMana = 0;
	}
	D2CommonStatsUnitSetValue(hUnit, 8, nMana);
	if (hAttacker && hUnit && nCost)
	{
		nOverlay = D2CommonUnitSetPositionExtra(hUnit, (int)D2CommonUnitGetX(hAttacker),
			(int)D2CommonUnitGetY(hAttacker));
		nOverlay = (D2CommonMonsterRegionLookupTableBRaw(nOverlay & 255) & 255) + 87;
		D2CommonUnitAddAnimOverlay(hUnit, nOverlay, 0);
	}
	if (nMana)
		return;
	hList = D2CommonStatsFindListByState(hUnit, STATE_ENERGYSHIELD);
	if (hList)
	{
		D2CommonStatsUnitUnlinkList(hUnit, hList);
		D2CommonStatsListFree(hList);
		return;
	}
	D2CommonStatsUnitSetActiveStateBit(hUnit, STATE_ENERGYSHIELD, 0);
}

void __fastcall SkillsFireMasteryDamage(UNIT *hUnit, DAMAGE *ptDamage)
{
	int nPercent;

	if (!ptDamage->nFireDamage)
		return;
	if (hUnit->eType != UNIT_PLAYER)
		return;
	if (hUnit->dwClassId != PLAYERCLASS_SORCERESS)
		return;
	if (!D2CommonStatsGetState(hUnit, STATE_FIREMASTERY))
		return;
	nPercent = D2CommonSkillsCalcCurveParamOne((int)D2CommonSkillsCalcNodeLevel(hUnit, D2CommonSkillsFindByKey(hUnit, SKILL_FIRE_MASTERY),
		1), SKILL_FIRE_MASTERY);
	ptDamage->nFireDamage += ptDamage->nFireDamage * nPercent / 100;
}

static SKILLCHART sgptSkillChart[NUM_SORCERESS_SKILLS] =
{
	{ NULL, NULL, MISSILE_FIREBOLT },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ NULL, SkillsChargedBolt, SKILLCHART_NOMISSILE },
	{ NULL, NULL, MISSILE_ICEBOLT },
	{ NULL, SkillsFrozenArmor, SKILLCHART_NOMISSILE },
	{ SkillsInfernoStart, SkillsInferno, SKILLCHART_NOMISSILE },
	{ NULL, SkillsStaticField, SKILLCHART_NOMISSILE },
	{ SkillsTelekinesisStart, SkillsTelekinesis, SKILLCHART_NOMISSILE },
	{ NULL, SkillsFrostNova, SKILLCHART_NOMISSILE },
	{ NULL, NULL, MISSILE_ICEBLAST },
	{ NULL, SkillsBlaze, SKILLCHART_NOMISSILE },
	{ NULL, NULL, MISSILE_FIREBALL },
	{ NULL, SkillsSorNova, SKILLCHART_NOMISSILE },
	{ NULL, NULL, MISSILE_LIGHTNINGBOLT },
	{ NULL, SkillsShiverArmor, SKILLCHART_NOMISSILE },
	{ NULL, SkillsFireWall, SKILLCHART_NOMISSILE },
	{ NULL, SkillsEnchant, SKILLCHART_NOMISSILE },
	{ NULL, SkillsChainLightning, SKILLCHART_NOMISSILE },
	{ NULL, SkillsTeleport, SKILLCHART_NOMISSILE },
	{ NULL, NULL, MISSILE_GLACIALSPIKE },
	{ NULL, SkillsMeteor, SKILLCHART_NOMISSILE },
	{ SkillsThunderStormStart, SkillsThunderStorm, SKILLCHART_NOMISSILE },
	{ NULL, SkillsEnergyShield, SKILLCHART_NOMISSILE },
	{ NULL, SkillsBlizzard, SKILLCHART_NOMISSILE },
	{ NULL, SkillsChillingArmor, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ SkillsHydraStart, SkillsHydra, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
	{ NULL, SkillsFrozenOrb, SKILLCHART_NOMISSILE },
	{ NULL, NULL, SKILLCHART_NOMISSILE },
};

BOOL __fastcall SkillsSorStart(GAME *ptGame, UNIT *hUnit, int nSkillId  ,
	int nLevel)
{
#line 1292
	D2ASSERT(nSkillId >= SKILL_SORCERESS_START);
#line 1293
	D2ASSERT(nSkillId - SKILL_SORCERESS_START < NUM_SORCERESS_SKILLS);
	if (!SkillsHasMana(hUnit))
		return FALSE;
	if (sgptSkillChart[nSkillId - SKILL_SORCERESS_START].pfnStart) {
#line 1300
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgptSkillChart[nSkillId - SKILL_SORCERESS_START].pfnStart), "sgptSkillChart[ nSkillId - SKILL_SORCERESS_START ].pfnStart");
		if (sgptSkillChart[nSkillId - SKILL_SORCERESS_START].pfnStart(ptGame, hUnit, nLevel))
			SkillsUseMana(hUnit);
		else
			return FALSE;
	}
	return TRUE;

}
#line 1307

BOOL __fastcall SkillsSorUse(GAME *ptGame, UNIT *hUnit, int nSkillId,
	int nLevel)
{
	MISSILESTXT *ptMissilesTxt;
	int nSorSkillId;

	nSorSkillId = nSkillId - SKILL_SORCERESS_START;
#line 1320
	D2ASSERT(nSorSkillId >= 0);
#line 1321
	D2ASSERT(nSorSkillId < NUM_SORCERESS_SKILLS);
	if (sgptSkillChart[nSorSkillId].pfnUse)
	{
#line 1325
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgptSkillChart[nSorSkillId].pfnUse), "sgptSkillChart[nSorSkillId].pfnUse");
		if (sgptSkillChart[nSorSkillId].pfnUse(ptGame, hUnit, nLevel))
		{
			if (!sgptSkillChart[nSorSkillId].pfnStart)
				SkillsUseMana(hUnit);
		}
		else
			return FALSE;
	}
	if (sgptSkillChart[nSorSkillId].nMissile != SKILLCHART_NOMISSILE)
	{
		ptMissilesTxt = (MISSILESTXT *)D2CommonMissileTblsGetMissile(sgptSkillChart[nSorSkillId].nMissile);
		hUnit->dwFlags0 |= 64;
		SkillsCreateMissile(ptGame, sgptSkillChart[nSorSkillId].nMissile, hUnit, nLevel, 0, 0, 0, 0,
			ptMissilesTxt->Qty);
		if (!sgptSkillChart[nSorSkillId].pfnStart)
			SkillsUseMana(hUnit);
	}
	return TRUE;
}

void __fastcall SkillsChillingArmorHit(GAME *ptGame, UNIT *hMissile, UNIT *hUnit)
{
	MISSILECREATE tCreate;
	UNIT *hOwner;
	DWORD hList;
	int i;

	hOwner = SUnitGetOwner(ptGame, hMissile);
	hList = D2CommonStatsFindListByState(hUnit, STATE_CHILLINGARMOR);
	if (!hOwner || !hList)
		return;
	if (!SUnitIsHostile(ptGame, hUnit, hOwner))
		return;
	if (!((MISSILESTXT *)D2CommonMissileTblsGetMissile(hMissile->dwClassId))->ReturnFire)
		return;
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = MISSFLAG_TARGETABS;
	tCreate.hSource = hUnit;
	tCreate.hSpawnLocation = hUnit;
	tCreate.nLevel = (int)D2CommonStatsListGetValueA(hList, 168);
	tCreate.eMissile = MISSILE_CHILLINGARMORBOLT;
	tCreate.nDX = (int)D2CommonUnitGetX(hOwner);
	tCreate.nDY = (int)D2CommonUnitGetY(hOwner);
	MissilesCreate(ptGame, &tCreate);
}
