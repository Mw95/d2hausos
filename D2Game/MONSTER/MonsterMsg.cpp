#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\MONSTER\\MonsterMsg.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "D2CMPImports.h"
#include "../GAME/Clients.h"
#include "../GAME/SCmd.h"
#include "../GAME/Targets.h"
#include "Monster.h"
#include "MonsterAI.h"
#include "MonsterEval.h"
#include "MonsterMsg.h"
#include "MonsterUnique.h"
#include "../QUESTS/QUESTS.h"
#include "../QUESTS/a4q2.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "../AI/AiGeneral.h"
#include "../GUILDS/Guilds.h"

struct MONMODEINFO
{
 	DWORD		field_00;
 	DWORD		field_04;
 	DWORD		field_08;
 	DWORD		field_0c;
 	DWORD		field_10;
 	DWORD		field_14;
};
typedef struct MONMODEINFO MONMODEINFO;
static const MONMODEINFO sgatMonModeInfo[16] =
{
	{ 0, 1, 0, 0, 8, 8 },
	{ 0, 0, 0, 0, 7, 7 },
	{ 1, 1, 1, 0, 1, 0 },
	{ 0, 0, 0, 0, 6, 6 },
	{ 1, 1, 0, 1, 11, 10 },
	{ 1, 1, 0, 1, 17, 16 },
	{ 1, 1, 0, 0, 18, 18 },
	{ 1, 1, 0, 1, 4, 5 },
	{ 1, 1, 0, 1, 12, 13 },
	{ 1, 1, 0, 1, 14, 15 },
	{ 1, 1, 0, 1, 26, 27 },
	{ 1, 1, 0, 1, 28, 29 },
	{ 0, 0, 0, 0, 9, 9 },
	{ 0, 1, 1, 0, 20, 20 },
	{ 1, 1, 0, 1, 12, 13 },
	{ 1, 1, 1, 0, 23, 24 },
};

void __fastcall MonsterSendMode(GAME *ptGame, UNIT *hMonster  ,
	CLIENT *hClient  )
{
	const MONMODEINFO *ptModeInfo;
	UNIT *hTarget;
	void *ptPath;
	void *pSkill;
	DWORD dwMode;
	DWORD dwInfo;
	DWORD dwParam1;
	DWORD dwParam2;
	DWORD dwSkillFlags;
	DWORD dwNodeType;
	DWORD dwUnitId;
	WORD wSkillArg;
	int nPathType;
	int nLife;
	int nMaxLife;
	DWORD dwCmd;
	BYTE bField0;
	BYTE bField1;
	BYTE bField2;
	BYTE bField3;
	BYTE bCombined;

	bField0 = 0;
	bField1 = 0;
	bField2 = 0;
	bField3 = 0;
#line 184
	D2ASSERT(hMonster);
#line 185
	D2ASSERT(hClient);
	dwMode = hMonster->dwMode;
	ptModeInfo = &sgatMonModeInfo[dwMode];
	hTarget = SUnitGetTarget(ptGame, hMonster);
	if ((hTarget && !ClientSeesUnit(hTarget, hClient)) || !ptModeInfo->field_00)
		hTarget = NULL;
	if (dwMode != 14 && !D2CommonUnitGetSkillThirdCached(hMonster))
	{

		ptPath = hMonster->ptPath;
		if (hTarget)
		{
			dwInfo = ptModeInfo->field_14;
			dwParam1 = hTarget->eType;
			dwParam2 = hTarget->dwId;
			dwCmd = 97;
			switch (dwMode)
			{
			case 2:
			case 15:
				nPathType = (int)D2CommonPathGetType(ptPath);
				if (nPathType >= 5 && nPathType <= 6)
				{
					dwCmd = 96;
					dwInfo = ptModeInfo->field_10;
					dwParam1 = D2CommonPathGetStepXRaw((DWORD)ptPath);
					dwParam2 = D2CommonPathGetStepYRaw((DWORD)ptPath);
				}
				break;
			}
		}
		else
		{
			dwInfo = ptModeInfo->field_10;
			dwCmd = 96;
			if (ptModeInfo->field_04)
			{
				dwParam1 = D2CommonPathGetStepXRaw((DWORD)ptPath);
				dwParam2 = D2CommonPathGetStepYRaw((DWORD)ptPath);
			}
			else
			{
				dwParam1 = D2CommonPathGetGameX(ptPath);
				dwParam2 = D2CommonPathGetGameY(ptPath);
			}
		}
		switch (dwMode)
		{
		case 6:
			if (!hTarget)
			{
				dwParam1 = 0;
				dwParam2 = (DWORD)-1;
			}
			break;
		case 0:
			bField0 = D2CommonPathGetFacing(ptPath);
			bField1 = (BYTE)UNIT5(hMonster)->dwHitClass;
			break;
		case 12:
			bField0 = D2CommonPathGetFacing(ptPath);
			break;
		case 13:
			bField0 = D2CommonPathGetSquareCountdown(ptPath);
			nLife = D2CommonStatsUnitGetValue(hMonster, 6);
			nMaxLife = D2CommonStatsUnitCalcMaxHp(hMonster);
			if (nMaxLife)
				bField1 = (BYTE)((nLife << 7) / nMaxLife);
			bField2 = (BYTE)UNIT5(hMonster)->dwHitClass;
			break;
		case 3:
			nLife = D2CommonStatsUnitGetValue(hMonster, 6);
			nMaxLife = D2CommonStatsUnitCalcMaxHp(hMonster);
			if (nMaxLife)
			{
				bField0 = (BYTE)((nLife << 7) / nMaxLife);
				if (bField0 > 1)
					bField0--;
			}
			if (MonsterUniqueGetBurstFlag(((MONSTERDATA *)hMonster->uTypeData)->ptUnique))
				bField0 |= 128;
			bField1 = (BYTE)UNIT5(hMonster)->dwHitClass;
			break;
		case 1:
			nLife = D2CommonStatsUnitGetValue(hMonster, 6);
			nMaxLife = D2CommonStatsUnitCalcMaxHp(hMonster);
			if (nMaxLife)
				bField0 = (BYTE)((nLife << 7) / nMaxLife);
			break;
		default:
			bField3 = D2CommonPathGetStopDistance(ptPath);
			break;
		}
		if (dwMode == 13)
		{
			if (dwCmd == 97)
				SCmdSendMonsterKnockback(hClient, 97, hMonster->dwId, (BYTE)dwInfo, (BYTE)dwParam1,
					dwParam2, bField0, bField1, bField2);
			else
				SCmdSendMonsterKnockbackTo(hClient, dwCmd, hMonster->dwId, (BYTE)dwInfo, (WORD)dwParam1,
					(WORD)dwParam2, bField0, bField1, bField2);
			return;
		}
		if (!ptModeInfo->field_08)
		{
			if (ptModeInfo->field_0c)
			{
				if (dwCmd == 97)
				{
					SCmdSendMonsterAttack(hClient, S2C_UNIT_ANIM_POS, hMonster->dwId, (BYTE)dwInfo,
						(BYTE)dwParam1, dwParam2, bField0, (WORD)D2CommonUnitGetX(hMonster),
						(WORD)D2CommonUnitGetY(hMonster));
				}
				else
				{
					SCmdSendMonsterAttackTo(hClient, 100, hMonster->dwId, (BYTE)dwInfo, (WORD)dwParam1,
						(WORD)dwParam2, bField0, bField1, (WORD)D2CommonUnitGetX(hMonster),
						(WORD)D2CommonUnitGetY(hMonster));
				}
				return;
			}
			if (dwCmd == 97)
				SCmdSendMonsterInPlace(hClient, 99, hMonster->dwId, (BYTE)dwInfo, (BYTE)dwParam1, dwParam2,
					bField0);
			else
				SCmdSendMonsterInPlaceTo(hClient, S2C_UNIT_MOVE_SHORT, hMonster->dwId, (BYTE)dwInfo,
					(WORD)dwParam1,
					(WORD)dwParam2, bField0, bField1);
			return;
		}
		if (dwCmd == 97)
			SCmdSendMonsterMove(hClient, 97, hMonster->dwId, (BYTE)dwInfo, (BYTE)dwParam1, dwParam2,
				bField3);
		else
			SCmdSendMonsterMoveTo(hClient, dwCmd, hMonster->dwId, (BYTE)dwInfo, (WORD)dwParam1,
				(WORD)dwParam2, bField3);
		return;
	}
	pSkill = (void *)D2CommonUnitGetSkillThirdCached(hMonster);
	dwSkillFlags = D2CommonSkillsGetNodeFlags(pSkill);
	dwNodeType = D2CommonSkillsGetNodeType(pSkill);
	wSkillArg = 0;
	if (dwSkillFlags & 8)
		wSkillArg = SUnitDmgGetHeldFlags(hMonster, hTarget);
	if (dwSkillFlags & 16)
		wSkillArg = D2CommonSkillsGetNodeSlotRaw((DWORD)hMonster, (DWORD)pSkill);
	bCombined = (BYTE)D2CommonSkillsCalcNodeLevel(hMonster, pSkill, 1);
	dwUnitId = hMonster->dwId;
	if (hTarget && ClientSeesUnit(hTarget, hClient))
	{
		SCmdSendUnitAt(hClient, 1, dwUnitId, (BYTE)hTarget->eType, hTarget->dwId,
			(WORD)dwNodeType, wSkillArg, bCombined);
		return;
	}
	SCmdSendTargetPoint(hClient, 1, dwUnitId, (WORD)D2CommonPathGetTargetXRaw((DWORD)hMonster->ptPath),
		(WORD)D2CommonPathGetTargetYRaw((DWORD)hMonster->ptPath), (WORD)dwNodeType, wSkillArg, bCombined);
}
