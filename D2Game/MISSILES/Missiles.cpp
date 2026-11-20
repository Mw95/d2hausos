#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\MISSILES\\Missiles.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/SCmd.h"
#include "../GAME/Event.h"
#include "../MONSTER/MonsterUnique.h"
#include "Missiles.h"
#include "MissMode.h"
#include "../UNIT/SUnit.h"
#include "../SKILLS/Skills.h"

#define UNIT_MONSTER		1
#define UNIT_MISSILE		3

#define MISSFLAG_POSITION	1
#define MISSFLAG_TARGETREL	2
#define MISSFLAG_VELOCITY	4
#define MISSFLAG_SUBLOOP	8
#define MISSFLAG_VELOCITY256	16
#define MISSFLAG_TARGETABS	32
#define MISSFLAG_RANGEDEC	512
#define MISSFLAG_ARRIVE		1024
#define MISSFLAG_ACTIVATE	2048
#define MISSFLAG_STAT13		4096
#define MISSFLAG_RANGE		32768

struct MISSILESKILLTXT
{
 	BYTE		unobserved_000[380];
 	BYTE		HitShift;
 	BYTE		SrcDam;
 	BYTE		unobserved_17e[2];
 	int			MinDam;
 	int			MaxDam;
 	int			LevDam;
 	BYTE		EType;
 	BYTE		unobserved_18d[3];
 	int			EMin;
 	int			EMax;
 	int			ELev;
 	int			ELen;
 	int			ELevLen;
};

#pragma pack(push, 1)
struct MISSILEPACKET66
{
 	BYTE		bCmd;
 	BYTE		unobserved_01[4];
 	WORD		wClass;
 	int			nX;
 	int			nY;
 	int			nTargetX;
 	int			nTargetY;
 	WORD		wRangeLeft;
 	BYTE		bOwnerType;
 	DWORD		dwOwnerId;
 	BYTE		bLevel;
 	BYTE		abField1f[2];
};
#pragma pack(pop)

EVENT * __fastcall MissilesUnitInit(GAME *ptGame, UNIT *hUnit, DWORD dwUnitId)
{
	D2CommonMissileCreateMissileData(hUnit);
	hUnit->dwFlags0 &= ~10;
	hUnit->dwId = dwUnitId;
	EventUnitInit(ptGame, hUnit);
	return EventAddPersistent(ptGame, hUnit, 0);
}

void __fastcall MissilesUnitFree(GAME *ptGame, UNIT *hUnit)
{
	D2CommonMissileFreeMissileData(hUnit);
	D2CommonStatsUnitFreeStats(hUnit);
	EventUnitFree(ptGame, hUnit);
}

UNIT *__fastcall MissilesCreate(GAME *ptGame, MISSILECREATE *ptCreate)
{
	MISSILESTXT *ptStats = NULL;
	struct MISSILESKILLTXT *ptSkill;
	GAMEROOM *hRoom;
	UNIT *hMissile, *hSource, *hWeapon;
	void *hPath;
	void *ptStatList;
	DWORD nXYVel;
	int nX, nY, nTargetX, nTargetY, nTX, nTY, nDist, nVel, nVelPct, nRange, nLevel;
	int nHitShift, nSrcDam, nMinDam, nMaxDam, nLevDam, nEMin, nEMax, nELev, nELen, nELevLen, nLevelUp;
	int nPct, nMin, nMax, nSrcMin, nSrcMax, nDmgPct, nELenTot, nPierce, nChance, nPierceMax, i, nSpeed;

	BYTE bEType;

	nVelPct = 100;
	if (!ptCreate->hSource)
	return NULL;
	ptStats = (MISSILESTXT *)D2CommonMissileTblsGetMissile(ptCreate->eMissile);
#line 333
	D2ASSERT(ptStats);
#line 334
	D2WARN(UnitGetType (ptCreate->hSource) == UNIT_PLAYER || UnitGetType (ptCreate->hSource) == UNIT_MONSTER);
	if (ptCreate->dwFlags & MISSFLAG_POSITION)
	{
		nX = ptCreate->nX;
		nY = ptCreate->nY;
	}
	else
	{
#line 351
		D2ASSERT(ptCreate->hSpawnLocation);
		nX = D2CommonUnitGetX(ptCreate->hSpawnLocation);
		nY = D2CommonUnitGetY(ptCreate->hSpawnLocation);
	}
	hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(ptCreate->hSource), nX, nY);
	if (!hRoom)
		return NULL;
	if (ptCreate->dwFlags & MISSFLAG_TARGETREL)
	{
		nTargetX = ptCreate->nDX + nX;
		nTargetY = ptCreate->nDY + nY;
	}
	else if (ptCreate->dwFlags & MISSFLAG_TARGETABS)
	{
		nTargetX = ptCreate->nDX;
		nTargetY = ptCreate->nDY;
	}
	else
	{
		nTargetX = nX;
		nTargetY = nY;
	}
	if (ptCreate->dwFlags & MISSFLAG_VELOCITY)
	{
		nSpeed = ptCreate->nVelocity;
		if (ptCreate->dwFlags & MISSFLAG_VELOCITY256)
			nVel = nSpeed;
		else
			nVel = nSpeed << 8;
	}
	else
		nVel = ptStats->Vel << 8;
	if (!ptStats->Explosion && ptCreate->hSource && D2CommonStatsGetState(ptCreate->hSource, 87))
	{
		ptStatList = (void *)D2CommonStatsFindListByState(ptCreate->hSource, 87);
		if (ptStatList)
		{
			nVelPct = D2CommonStatsListGetValueA(ptStatList, 161);
			nVel = nVel * nVelPct / 100;
		}
	}
	if (nVel)
	{
		nVel = nVel * 75 / 100;
		if (nVel)
		{
			nTX = nTargetX;
			nTY = nTargetY;
			if (ptCreate->hTarget)
			{
				nTX = D2CommonUnitGetX(ptCreate->hTarget);
				nTY = D2CommonUnitGetY(ptCreate->hTarget);
			}
			nDist = nTX - nX;
			if (nDist < 0)
				nDist = -nDist;
			if (nDist >= 100)
				return NULL;
			nDist = nTY - nY;
			if (nDist < 0)
				nDist = -nDist;
			if (nDist >= 100)
				return NULL;
			if (!ptCreate->hTarget && nTargetX == nX && nTargetY == nY)
			{
				nTargetX++;
				nTargetY++;
			}
			else if (ptCreate->hTarget && ptCreate->hSource && ptCreate->hTarget != ptCreate->hSource
				&& nTX == (int)D2CommonUnitGetX(ptCreate->hSource)
				&& nTY == (int)D2CommonUnitGetY(ptCreate->hSource))
			{
				nTargetX++;
				nTargetY++;
				ptCreate->hTarget = NULL;
			}
		}
	}
	hMissile = SUnitCreate(UNIT_MISSILE, ptCreate->eMissile, nX, nY, ptGame, hRoom, 1, ptStats->CollideType,
		0);
	if (!hMissile)
		return NULL;
	if (ptCreate->dwFlags & MISSFLAG_RANGE)
		nRange = ptCreate->nRange;
	else
	{
		nRange = ptCreate->nLevel * ptStats->LevRange + ptStats->Range;
		if ((ptCreate->dwFlags & MISSFLAG_SUBLOOP) && ptStats->SubLoop)
			nRange += (ptStats->SubStop - ptStats->SubStart) * ptCreate->nSubLoops;
		if (nVelPct && nVelPct != 100)
			nRange += nRange * 100 / nVelPct;
	}
	if (ptCreate->dwFlags & MISSFLAG_RANGEDEC)
	{
		nRange -= ptCreate->nRangeDec;
		UNIT7(hMissile)->nAnimPos = ptCreate->nRangeDec << 8;
	}
	D2CommonMissileSetRange(hMissile, nRange);
	D2CommonMissileSetRangeLeft(hMissile, nRange);
	if (ptCreate->dwFlags & MISSFLAG_ACTIVATE)
	{ nRange -= ptCreate->nActivate; D2CommonMissileSetActivateAt(hMissile, nRange); }
	else
	{ nRange -= ptStats->Activate; D2CommonMissileSetActivateAt(hMissile, nRange); }

	hPath = hMissile->ptPath;
#line 461
	D2ASSERT(hPath);
	D2CommonPathSetVelocity(hPath, 0);
	if (ptCreate->hTarget)
		D2CommonPathSetTarget(hPath, ptCreate->hTarget);
	else
		D2CommonPathSetTargetCoord(hPath, nTargetX, nTargetY);
	if (ptStats->Collision) D2CommonPathSetCollisionMask(hPath, 64); else D2CommonPathSetCollisionMask(hPath, 0);
	D2CommonPathSetCollisionTestMask(hPath, gptMissileModeChart[ptStats->CollideType].dwPattern);
	if (ptStats->CanDestroy)
		hMissile->dwFlags0 |= 4;
	if (nVel)
	{
		D2CommonPathSetVelocity(hPath, nVel);
		D2CommonPathCompute(hPath, hMissile);
	}
	D2CommonMissileSetLastCollideUnit(hMissile, ptCreate->hSource);
	D2CommonPathSetAccel(hPath, ptStats->Accel);
	D2CommonPathSetMaxVelocity(hPath, ptStats->MaxVel << 8);
	if (ptCreate->dwFlags & MISSFLAG_ARRIVE)
	{
		nDist = D2CommonUnitUtilGetDistance(hMissile, nTargetX, nTargetY);
		if (!nDist)
			nDist = 1;
		nXYVel = nVel << 4;
#line 495
		D2ASSERT(nXYVel);
		D2CommonMissileSetRangeLeft(hMissile, (nDist << 16) / nXYVel);
	}

	nLevel = ptCreate->nLevel;
	hSource = ptCreate->hSource;
	if (ptStats->Skill)
	{
		ptSkill = (struct MISSILESKILLTXT *)D2CommonDataTblsGetSkill(ptStats->Skill);
		nHitShift = ptSkill->HitShift;
		nSrcDam = ptSkill->SrcDam;
		if (ptStats->SrcDamage == -1)
			nSrcDam = 0;
		nMinDam = ptSkill->MinDam;
		nMaxDam = ptSkill->MaxDam;
		nLevDam = ptSkill->LevDam;
		bEType = ptSkill->EType;
		nEMin = ptSkill->EMin;
		nEMax = ptSkill->EMax;
		nELev = ptSkill->ELev;
		nELen = ptSkill->ELen;
		nELevLen = ptSkill->ELevLen;
	}
	else
	{
		nHitShift = ptStats->HitShift;
		nSrcDam = ptStats->SrcDamage;
		nMinDam = ptStats->nMinDamage;
		nMaxDam = ptStats->nMaxDamage;
		nLevDam = ptStats->LevDamage;
		bEType = ptStats->EType;
		nEMin = ptStats->EMin;
		nEMax = ptStats->EMax;
		nELev = ptStats->ELevel;
		nELen = ptStats->ELen;
		nELevLen = ptStats->ELevelLen;
	}
	D2CommonStatsUnitAllocStats(hMissile, 0);
	D2CommonStatsUnitSetValue(hMissile, 12, nLevel);
	nSrcMin = 0;
	nSrcMax = 0;
	if (nSrcDam)
	{
		nPct = 0;
		hWeapon = NULL;
		if (UnitGetType(hSource) == UNIT_PLAYER)
			hWeapon = (UNIT *)D2CommonUnitGetActiveWeapon(hSource);
		if (hWeapon)
		{
			if (D2CommonITEMSGetTypeStrBonus(hWeapon))
				nPct = D2CommonStatsUnitGetValue(hSource, 0);
			if (D2CommonITEMSGetTypeDexBonus(hWeapon))
				nPct += (DWORD)D2CommonStatsUnitGetValue(hSource, 2);
			if (D2CommonITEMSGetTypeThrowable(hWeapon))
			{
				nSrcMin = D2CommonStatsGetTotalValue(hWeapon, 159) << 8;
				nSrcMax = D2CommonStatsGetTotalValue(hWeapon, 160);
			}
			else
			{
				nSrcMin = (DWORD)D2CommonStatsUnitGetValue(hWeapon, 21) << 8;
				nSrcMax = D2CommonStatsUnitGetValue(hWeapon, 22);
			}
		}
		else
		{
			nSrcMin = (DWORD)D2CommonStatsUnitGetValue(hSource, 21) << 8;
			nSrcMax = D2CommonStatsUnitGetValue(hSource, 22);
		}
		nSrcMin = (nPct * nSrcMin / 100 + nSrcMin) * nSrcDam / 2;
		nSrcMax <<= 8;
		nSrcMax = (nPct * nSrcMax / 100 + nSrcMax) * nSrcDam / 2;
	}
	nLevelUp = nLevel - 1;
	nSrcMin += (nLevelUp * nLevDam + nMinDam) << nHitShift;
	nSrcMax += (nLevelUp * nLevDam + nMaxDam) << nHitShift;
	nDmgPct = D2CommonStatsUnitGetValue(hSource, 25);
	D2CommonStatsUnitSetValue(hMissile, 21, nDmgPct * nSrcMin / 100 + nSrcMin);
	D2CommonStatsUnitSetValue(hMissile, 22, nDmgPct * nSrcMax / 100 + nSrcMax);
	if (bEType)
	{
		nMin = (nLevelUp * nELev + nEMin) << nHitShift;
		nMax = (nLevelUp * nELev + nEMax) << nHitShift;
		nELenTot = nLevelUp * nELevLen + nELen;
		switch (bEType)
		{
		case 1:
			D2CommonStatsUnitSetValue(hMissile, 48, nMin);
			D2CommonStatsUnitSetValue(hMissile, 49, nMax);
			break;
		case 2:
			switch (D2CommonMissileGetClassId(hMissile))
			{
			case 93:
			case 98:
			case 205:
			case 231:
			case 232:
			case 234:
				nMin = 256;
				break;
			}
			D2CommonStatsUnitSetValue(hMissile, 50, nMin);
			D2CommonStatsUnitSetValue(hMissile, 51, nMax);
			break;
		case 3:
			D2CommonStatsUnitSetValue(hMissile, 52, nMin);
			D2CommonStatsUnitSetValue(hMissile, 53, nMax);
			break;
		case 4:
			D2CommonStatsUnitSetValue(hMissile, 54, nMin);
			D2CommonStatsUnitSetValue(hMissile, 55, nMax);
			D2CommonStatsUnitSetValue(hMissile, 56, nELenTot);
			break;
		case 5:
			D2CommonStatsUnitSetValue(hMissile, 57, nMin);
			D2CommonStatsUnitSetValue(hMissile, 58, nMax);
			D2CommonStatsUnitSetValue(hMissile, 59, nELenTot);
			break;
		case 6:
			D2CommonStatsUnitSetValue(hMissile, 60, nMin);
			D2CommonStatsUnitSetValue(hMissile, 61, nMax);
			break;
		case 7:
			D2CommonStatsUnitSetValue(hMissile, 62, nMin);
			D2CommonStatsUnitSetValue(hMissile, 63, nMax);
			break;
		case 8:
			D2CommonStatsUnitSetValue(hMissile, 64, nMin);
			D2CommonStatsUnitSetValue(hMissile, 65, nMax);
			break;
		case 9:
			D2CommonStatsUnitSetValue(hMissile, 66, nELenTot);
			break;
		}
	}
	if (nSrcDam)
	{
		D2CommonUnitApplyPrimaryWeaponSwap(hSource);
		D2CommonStatsUnitSetValue(hMissile, 48,
			((DWORD)D2CommonStatsUnitGetValue(hSource, 48) << 8) + (DWORD)D2CommonStatsUnitGetValue(hMissile, 48));
		D2CommonStatsUnitSetValue(hMissile, 49,
			((DWORD)D2CommonStatsUnitGetValue(hSource, 49) << 8) + (DWORD)D2CommonStatsUnitGetValue(hMissile, 49));
		D2CommonStatsUnitSetValue(hMissile, 50,
			((DWORD)D2CommonStatsUnitGetValue(hSource, 50) << 8) + (DWORD)D2CommonStatsUnitGetValue(hMissile, 50));
		D2CommonStatsUnitSetValue(hMissile, 51,
			((DWORD)D2CommonStatsUnitGetValue(hSource, 51) << 8) + (DWORD)D2CommonStatsUnitGetValue(hMissile, 51));
		D2CommonStatsUnitSetValue(hMissile, 52,
			((DWORD)D2CommonStatsUnitGetValue(hSource, 52) << 8) + (DWORD)D2CommonStatsUnitGetValue(hMissile, 52));
		D2CommonStatsUnitSetValue(hMissile, 53,
			((DWORD)D2CommonStatsUnitGetValue(hSource, 53) << 8) + (DWORD)D2CommonStatsUnitGetValue(hMissile, 53));
		D2CommonStatsUnitSetValue(hMissile, 54,
			((DWORD)D2CommonStatsUnitGetValue(hSource, 54) << 8) + (DWORD)D2CommonStatsUnitGetValue(hMissile, 54));
		D2CommonStatsUnitSetValue(hMissile, 55,
			((DWORD)D2CommonStatsUnitGetValue(hSource, 55) << 8) + (DWORD)D2CommonStatsUnitGetValue(hMissile, 55));
		D2CommonStatsUnitSetValue(hMissile, 56,
			(DWORD)D2CommonStatsUnitGetValue(hSource, 56) + (DWORD)D2CommonStatsUnitGetValue(hMissile, 56));
		D2CommonStatsUnitSetValue(hMissile, 57,
			(DWORD)D2CommonStatsUnitGetValue(hSource, 57) + (DWORD)D2CommonStatsUnitGetValue(hMissile, 57));
		D2CommonStatsUnitSetValue(hMissile, 58,
			(DWORD)D2CommonStatsUnitGetValue(hSource, 58) + (DWORD)D2CommonStatsUnitGetValue(hMissile, 58));
		D2CommonStatsUnitSetValue(hMissile, 59,
			(DWORD)D2CommonStatsUnitGetValue(hSource, 59) + (DWORD)D2CommonStatsUnitGetValue(hMissile, 59));
		D2CommonStatsUnitSetValue(hMissile, 62,
			(DWORD)D2CommonStatsUnitGetValue(hSource, 62) + (DWORD)D2CommonStatsUnitGetValue(hMissile, 62));
		D2CommonStatsUnitSetValue(hMissile, 60,
			(DWORD)D2CommonStatsUnitGetValue(hSource, 60) + (DWORD)D2CommonStatsUnitGetValue(hMissile, 60));
		{
			int nAddMin;
			int nAddMax;

			nAddMin = 0;
			nAddMax = 0;
			D2CommonSkillsAddListRange(hSource, &nAddMin, &nAddMax, 1);
			D2CommonStatsUnitAddValue(hMissile, 48, nAddMin << 8);
			D2CommonStatsUnitAddValue(hMissile, 49, nAddMax << 8);
		}
		D2CommonUnitApplyEquipStatSwap(hSource);
	}
	D2CommonMissileSetLevel(hMissile, ptCreate->nLevel);
	D2CommonUnitSetLinkedUnit(hMissile, ptCreate->hSource);
	hMissile->dwFlags0 &= ~8;
	nPierce = 0;
	i = 0;
	hSource = ptCreate->hSource;
	if ((!hSource || UnitGetType(hSource) == UNIT_PLAYER || UnitGetType(hSource) == UNIT_MONSTER)
		&& ((MISSILESTXT *)D2CommonMissileTblsGetMissile(D2CommonMissileGetClassId(hMissile)))->Pierce)
	{
		ptStatList = (void *)D2CommonStatsFindListByState(hSource, 69);
		if (ptStatList)
			nPierce = D2CommonStatsListGetValueA(ptStatList, 166);
		nPierce += D2CommonStatsGetTotalValue(hSource, 156);
		if (nPierce)
		{
			DWORD dwSeedA, dwSeedB;
			D2SEED *ptSeed;

			nChance = D2CommonSkillsCalcCurveParamOne(nPierce, SKILL_PIERCE);
			ptSeed = &hMissile->tSeed;
			SUnitGetTarget(ptGame, hSource);
			if (SkillsGetTargetSpot(ptGame, hSource, (int *)&dwSeedA, (int *)&dwSeedB))
			{
				D2CommonSeedSet(ptSeed, dwSeedB + dwSeedA);
				nPierceMax = D2CommonSkillsGetTypeParamThree(SKILL_PIERCE);
				while (1)
				{
					if (i >= nPierceMax)
						break;
					if ((int)D2CommonSeedRand(ptSeed, 100) >= nChance)
						break;
					i++;
				}
				D2CommonMissileSetIndexedByte(hMissile, 1, i);
			}
		}
	}
	if (D2CommonStatsGetState(ptCreate->hSource, 64))
		D2CommonMissileSetIndexedByte(hMissile, 0,
			D2CommonSkillsCalcNodeLevel(ptCreate->hSource, D2CommonSkillsFindByKey(ptCreate->hSource, 9),
				1));
	if (ptCreate->pfnModify)
	{
#line 515
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)ptCreate->pfnModify), "ptCreate->pfnModify");
		ptCreate->pfnModify(hMissile, ptCreate->dwModifyArg); }
	if (ptCreate->dwFlags & MISSFLAG_STAT13)
		D2CommonStatsUnitSetValue(hMissile, 19, ptCreate->nToHit);
	MonsterUniqueMissileMods(ptGame, ptCreate->hSource, hMissile);
	return hMissile;
}

void __fastcall MissilesSendToClient(CLIENT *ptClient, GAME *ptGame, UNIT *hMissile,
	BOOL fTarget)
{
	struct MISSILEPACKET66 tPacket;
	UNIT *hOwner;
	DWORD i;
	int j;

	hOwner = SUnitGetOwner(ptGame, hMissile);
	if (!hOwner)
		return;
#line 545
	D2ASSERT(UnitGetType (hMissile) == UNIT_MISSILE);
	if (!((MISSILESTXT *)D2CommonMissileTblsGetMissile(hMissile->dwClassId))->ClientSend)
		return;
	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 102;
	tPacket.wClass = (WORD)D2CommonMissileGetClassId(hMissile);
	tPacket.nX = D2CommonUnitGetX(hMissile);
	tPacket.nY = D2CommonUnitGetY(hMissile);
	if (fTarget)
	{
		tPacket.nTargetX = D2CommonPathGetTargetXRaw((DWORD)hMissile->ptPath);
		tPacket.nTargetY = D2CommonPathGetTargetYRaw((DWORD)hMissile->ptPath);
	}
	tPacket.wRangeLeft = (WORD)D2CommonMissileGetRangeLeft(hMissile);
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.bLevel = (BYTE)D2CommonMissileGetLevel(hMissile);
	for (j = 0; j < 2; j++)
		tPacket.abField1f[j] = D2CommonMissileGetIndexedByte(hMissile, j);
	SCmdSendMissile(ptClient, (const struct SCMDCOPY33 *)&tPacket);
}

BOOL __fastcall MissilesCollideMonster(UNIT *hUnit)
{
	if (UnitGetType(hUnit) != UNIT_MONSTER)
		return FALSE;
	if (hUnit->dwFlags0 & 8)
		return TRUE;
	return FALSE;
}

BOOL __fastcall MissilesCollidePlayer(UNIT *hUnit)
{
	int nType;

	nType = UnitGetType(hUnit);
	if (nType == UNIT_MONSTER && !D2CommonStatsUnitGetAlignment(hUnit))
		return TRUE;
	if (nType != UNIT_PLAYER)
		return FALSE;
	if (hUnit->dwFlags0 & 8)
		return TRUE;
	return FALSE;
}

BOOL __fastcall MissilesCollideUnit(UNIT *hUnit)
{
	if (UnitGetType(hUnit) != UNIT_PLAYER && UnitGetType(hUnit) != UNIT_MONSTER)
		return FALSE;
	if (hUnit->dwFlags0 & 8)
		return TRUE;
	return FALSE;
}

BOOL __fastcall MissilesCollideMissile(UNIT *hUnit)
{
	if (UnitGetType(hUnit) != UNIT_MISSILE)
		return FALSE;
	return ((MISSILESTXT *)D2CommonMissileTblsGetMissile(hUnit->dwClassId))->CanDestroy != 0;
}

static __inline BOOL MissilesInRadiusClear(DWORD *tField, GAMEROOM *hRoom, int *pnX, int *pnY)
{
	while (D2CommonFieldTblsStep(tField, pnX, pnY))
	{
		if (D2CommonCollisnTestPoint(hRoom, *pnX, *pnY, 4) == 4)
			return FALSE;
	}
	return TRUE;
}

void __fastcall MissilesCreateInRadius(GAME *ptGame, UNIT *hUnit, int nRadius,
	int eMissile)
{
	MISSILECREATE tCreate;
	GAMEROOM *hRoom;
	DWORD *tField;
	DWORD i;
	int nX, nY, nRadiusSq, nDX, nDY, nPosX, nPosY, nStepX, nStepY;

	tField = D2CommonFieldTblsAllocPair();
	nX = D2CommonUnitGetX(hUnit);
	nY = D2CommonUnitGetY(hUnit);
	nRadiusSq = nRadius * nRadius;
	D2CommonFieldTblsSetPair(tField, nX, nY);
	for (i = 0; i < sizeof(MISSILECREATE) / sizeof(DWORD); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = MISSFLAG_POSITION;
	tCreate.hSource = SUnitGetOwner(ptGame, hUnit);
	tCreate.nLevel = D2CommonStatsUnitGetValue(hUnit, 12);
	tCreate.eMissile = eMissile;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
	nDX = -nRadius;
	if (nDX <= nRadius)
	{
		nPosX = nDX + nX;
		do
		{
			nDY = -nRadius;
			if (nDY <= nRadius)
			{
				for (nPosY = nDY + nY; nDY <= nRadius; nDY++, nPosY++)
				{
					nStepY = nPosY;
					nStepX = nPosX;
					if (!hRoom || nDY * nDY + nDX * nDX > nRadiusSq)
						continue;
					if (!MissilesInRadiusClear(tField, hRoom, &nStepX, &nStepY))
						continue;
					hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(hRoom, nPosX, nPosY);
					if (hRoom && !D2CommonDungeonRoomIsTown(hRoom))
					{
						tCreate.nX = nPosX;
						tCreate.nY = nPosY;
						MissilesCreate(ptGame, &tCreate);
					}
				}
			}
		} while (++nDX, nPosX++, nDX <= nRadius);
	}
	D2CommonFieldTblsFreePair(tField);
}
