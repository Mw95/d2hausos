#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\MISSILES\\MissMode.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "../GAME/Targets.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/MonsterMode.h"
#include "../MONSTER/MonsterUnique.h"
#include "../OBJECTS/Objects.h"
#include "Missiles.h"
#include "MissMode.h"
#include "../OBJECTS/ObjMode.h"
#include "../SKILLS/SkillBar.h"
#include "../PLAYER/PlrMsg.h"
#include "../UNIT/SUnit.h"
#include "../UNIT/SUnitDmg.h"
#include "../AI/AiGeneral.h"
#include "../GUILDS/Guilds.h"
#include "../SKILLS/Skills.h"
#include "../SKILLS/SkillSor.h"
#include "../SKILLS/SkilPal.h"

#define UNIT_MONSTER		1
#define UNIT_OBJECT			2
#define UNIT_MISSILE		3
#define NUM_DIFFICULTY_LEVELS	3
#define NUM_MISSILE_CLASSES	385

typedef struct MISSSKILLSTXT MISSSKILLSTXT;
typedef struct MISSOBJECTSTXT MISSOBJECTSTXT;
typedef struct MISSLEVELDATA MISSLEVELDATA;
typedef struct UNITFINDARGS UNITFINDARGS;
typedef struct UNITFINDDATA UNITFINDDATA;
typedef struct SKILLFINDTARGET SKILLFINDTARGET;
typedef struct SKILLSUMMON SKILLSUMMON;
typedef struct SKILLSTATE SKILLSTATE;
typedef struct OBJOPERATE OBJOPERATE;
typedef struct PATHPOINT PATHPOINT;

typedef void (__fastcall *PFNRADAMENTCALLBACK)(GAME *ptGame, UNIT *hMissile, UNIT *hUnit, DWORD dwArg,
	DWORD dwUnused);

#define Objects31480	((PFNMISSILEHIT)ObjectThemeNone)

MISSILEMODECHART gptMissileModeChart[NUM_MISSILE_MODES] =
{
	{ NULL,                    0 },
	{ MissilesCollidePlayer,   132 },
	{ MissilesCollideMonster,  260 },
	{ MissilesCollideUnit,     388 },
	{ NULL,                    0 },
	{ MissilesCollideMonster,  260 },
	{ NULL,                    4 },
	{ MissilesCollideMissile,  64 },
	{ MissilesCollideUnit,     389 },
};

struct MISSILECHART
{
 	PFNMISSILEMODE		pfnMode;
 	PFNMISSILEHIT		pfnHit;
 	PFNMISSILEDAMAGE	pfnDamage;
};
static const MISSILECHART sgptMissileChart[NUM_MISSILE_CLASSES] =
{
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ NULL, NULL, NULL },
	{ NULL, NULL, NULL },
	{ NULL, NULL, NULL },
	{ NULL, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, MissModeDamageIceArrow },
	{ NULL, NULL, NULL },
	{ NULL, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ NULL, NULL, NULL },
	{ NULL, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModePoisonJav, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, MissModeHitFireBall, NULL },
	{ NULL, NULL, NULL },
	{ MissModeModePlagueJavelin, MissModeHitPlagueJavelin, NULL },
	{ MissModeModeDefault, MissModeHitPotion, NULL },
	{ MissModeModeDefault, MissModeHitPotion, NULL },
	{ MissModeModeDefault, MissModeHitPotion, NULL },
	{ MissModeModeDefault, MissModeHitRancidGasPotion, NULL },
	{ MissModeModeDefault, MissModeHitChokingGasPotion, NULL },
	{ MissModeModeDefault, MissModeHitStranglingGasPotion, NULL },
	{ NULL, NULL, NULL },
	{ NULL, NULL, NULL },
	{ NULL, NULL, NULL },
	{ NULL, NULL, NULL },
	{ NULL, NULL, NULL },
	{ MissModeModeDefault, MissModeHitHolyBolt, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, MissModeHitFireBall, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeFire, MissModeHitBlaze, MissModeDamageFire },
	{ MissModeModeFirewallMaker, NULL, NULL },
	{ MissModeModeFire, NULL, MissModeDamageFire },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ NULL, NULL, NULL },
	{ NULL, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ NULL, NULL, NULL },
	{ NULL, NULL, NULL },
	{ MissModeModeDefault, MissModeHitImmolationArrow, NULL },
	{ MissModeModeGuided, MissModeHitGuided, MissModeDamageGuidedArrow },
	{ MissModeModeDefault, MissModeHitFreezingArrow, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, MissModeDamageIceBlast },
	{ MissModeModeDefault, NULL, MissModeDamageBlessedHammer },
	{ MissModeModeDefault, MissModeHitChainLightning, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, MissModeHitGlacialSpike, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ NULL, NULL, NULL },
	{ MissModeModeDefault, MissModeHitMeteor, NULL },
	{ NULL, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeMonBlizCenter, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, MissModeDamageRogue },
	{ MissModeModeDefault, NULL, MissModeDamageRogue },
	{ MissModeModeDefault, NULL, MissModeDamageRogue },
	{ MissModeModeBatLightning, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeFirewallMaker, NULL, NULL },
	{ MissModeModeFire, NULL, MissModeDamageFire },
	{ NULL, NULL, NULL },
	{ MissModeModeDefault, MissModeHitMeteor, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, MissModeHitSpiderGooLay, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, MissModeHitSpiderGoo, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, MissModeHitHowl, NULL },
	{ MissModeModeDefault, MissModeHitShout, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeBlizzardCenter, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeFingerMageSpider, MissModeHitFingerMageSpider, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDiabWallMaker, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeGuided, MissModeHitGuided, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, MissModeHitLightningFury, NULL },
	{ MissModeModeBoneWallMaker, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ NULL, NULL, NULL },
	{ MissModeModeDefault, MissModeHitBattleCry, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeCloud, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeFire, NULL, MissModeDamageFire },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, MissModeHitChainLightning, NULL },
	{ MissModeModeDefault, MissModeHitFistOfTheHeavens, NULL },
	{ MissModeModeDefault, MissModeHitHolyBolt, NULL },
	{ MissModeModeDefault, NULL, MissModeDamageWarcry },
	{ MissModeModeDefault, MissModeHitBattleCommand, NULL },
	{ MissModeModeDefault, MissModeHitBattleOrders, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, MissModeHitPantherPot, NULL },
	{ MissModeModeFire, NULL, MissModeDamageFire },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModePoisonJav, NULL, NULL },
	{ MissModeModePoisonJav, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ NULL, NULL, NULL },
	{ MissModeModeDefault, MissModeHitGrimWardStart, NULL },
	{ MissModeModeGrimWard, Objects31480, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, MissModeHitGrimWardStart, NULL },
	{ MissModeModeGrimWard, Objects31480, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, MissModeHitGrimWardStart, NULL },
	{ MissModeModeGrimWard, Objects31480, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, MissModeHitGrimWardScare, NULL },
	{ MissModeModeFrozenOrb, MissModeHitFrozenOrb, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeFrozenOrbNova, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, MissModeHitChainLightning, NULL },
	{ NULL, NULL, NULL },
	{ NULL, NULL, NULL },
	{ NULL, NULL, NULL },
	{ MissModeModeDefault, MissModeHitMonGlacialSpike, NULL },
	{ NULL, NULL, NULL },
	{ NULL, NULL, NULL },
	{ NULL, NULL, NULL },
	{ NULL, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, MissModeHitFirehead, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, MissModeHitFireBall, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeCairnStones, MissModeHitCairnStones, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ NULL, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeGuided, MissModeHitGuided, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeFire, NULL, NULL },
	{ MissModeModeTowerChestSpawner, MissModeHitTowerChestSpawner, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, MissModeHitHellMeteorDown, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeRadamentDeath, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, MissModeHitOrbMist, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
	{ MissModeModeDefault, NULL, NULL },
};

static inline int MissModeRollStat(UNIT *hMissile, int nStatMin, int nStatMax)
{
	int nMin;
	int nMax;
	int nRange;
	int nRoll;

	nMin = D2CommonStatsUnitGetValue(hMissile, nStatMin);
	nMax = D2CommonStatsUnitGetValue(hMissile, nStatMax);
	if (!nMin && !nMax)
		return 0;
	if (nMin >= nMax)
		nMin = nMax;
	nRange = nMax - nMin;
	if (nRange)
		nRoll = (int)D2CommonSeedRand(&hMissile->tSeed, nRange);
	else
		nRoll = 0;
	return nRoll + nMin;
}

void __fastcall MissModeApplyDamage(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, DAMAGE *ptDamage)
{
	UNIT *hOwner;
	MISSILESTXT *ptTxt;
	DWORD dwResult;

	hOwner = SUnitGetOwner(ptGame, hMissile);
	dwResult = 0;
#line 305
	D2WARN(UnitGetType (hMissile) == UNIT_MISSILE);
	if (!hOwner)
		return;
	SkillsFireMasteryDamage(hOwner, ptDamage);
	ptTxt = (MISSILESTXT *)D2CommonMissileTblsGetMissile(hMissile->dwClassId);
	ptDamage->wFlags |= 1;
	if (hMissile->eType == UNIT_MISSILE)
	{
		ptDamage->wFlags |= 1;
		if (!D2CommonStatsGetState(hTarget, 54))
		{
			if (ptTxt->GetHit)
				ptDamage->wFlags |= 4;
			if (ptTxt->KnockBack && D2CommonSeedRand(&hMissile->tSeed, 100) < ptTxt->KnockBack)
				ptDamage->wFlags |= 8;
		}
	}
	if (ptDamage->nPhysicalDamage)
	{
		dwResult = SUnitDmgBlock(hOwner, hTarget, TRUE, TRUE);
		if ((dwResult & 1) && !ptTxt->GetHit && !ptTxt->KnockBack)
			ptDamage->wFlags = (WORD)((ptDamage->wFlags & 65534) | 16384);
	}
	if (dwResult & 8)
		ptDamage->wFlags |= 512;
	if (dwResult & 2)
		ptDamage->wFlags |= 256;
	if (dwResult & 1)
		ptDamage->wFlags |= 16;
	if (dwResult)
		ptDamage->wFlags &= 65534;
	ptDamage->dwHitClass = D2CommonMissileGetHitClass(hMissile) | (ptDamage->dwHitClass & 240);
	if (!ptDamage->dwHitClass)
		ptDamage->dwFixedHitClass = 1;
	MonsterModeCriticalHit(hOwner, hTarget, ptDamage);
	if (ptDamage->wFlags & 1)
		SUnitDmgApply(ptGame, hOwner, hTarget, 1, ptDamage);
	SUnitDmgReact(ptGame, hOwner, hTarget, ptDamage);
}

int __fastcall MissModeHitBlaze(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	UNIT *hOwner;
	int nResult = 2;

	if (hTarget)
	{
		hOwner = SUnitGetOwner(ptGame, hMissile);
		if (hTarget == hOwner && D2CommonStatsGetState(hOwner, 13))
			nResult = 0;
	}
	return nResult;
}

void __fastcall MissModeDamageIceArrow(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, DAMAGE *ptDamage)
{
	ptDamage->nFreezeLength = D2CommonStatsUnitGetValue(hMissile, 56);
	ptDamage->nColdLength = 0;
}

int __fastcall MissModeHitFireBall(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	MISSILESTXT *ptTxt;
	int nX;
	int nY;
	int nParam;
	DAMAGE tDamage;
	int i;

	ptTxt = (MISSILESTXT *)D2CommonMissileTblsGetMissile(hMissile->dwClassId);
	nX = (int)D2CommonUnitGetX(hMissile);
	nY = (int)D2CommonUnitGetY(hMissile);
	nParam = ptTxt->Param1;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.nFireDamage = MissModeRollStat(hMissile, 48, 49);
	SUnitDmgSplash(ptGame, hMissile, nX, nY, nParam, &tDamage, FALSE, FALSE, NULL, 0);
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.nPhysicalDamage = MissModeRollStat(hMissile, 21, 22);
	tDamage.nMagicDamage = MissModeRollStat(hMissile, 52, 52);
	tDamage.nLightningDamage = MissModeRollStat(hMissile, 50, 51);
	tDamage.nColdDamage = MissModeRollStat(hMissile, 54, 55);
	tDamage.nColdLength = D2CommonStatsUnitGetValue(hMissile, 56);
	tDamage.nPoisonDamage = MissModeRollStat(hMissile, 57, 58);
	tDamage.nPoisonLength = D2CommonStatsUnitGetValue(hMissile, 59);
	if (hTarget)
		MissModeApplyDamage(ptGame, hMissile, hTarget, &tDamage);
	return 1;
}

int __fastcall MissModeHitPotion(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	MISSILESTXT *ptTxt;
	int nX;
	int nY;
	DAMAGE tDamage;
	int i;
	int nParam;

	if (!(hTarget)) {
		ptTxt = (MISSILESTXT *)D2CommonMissileTblsGetMissile(hMissile->dwClassId);
		nX = (int)D2CommonUnitGetX(hMissile);
		nY = (int)D2CommonUnitGetY(hMissile);
		for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
			((DWORD *)&tDamage)[i] = 0;
		tDamage.nPhysicalDamage = MissModeRollStat(hMissile, 21, 22);
		tDamage.nFireDamage = MissModeRollStat(hMissile, 48, 49);
		tDamage.nMagicDamage = MissModeRollStat(hMissile, 52, 52);
		tDamage.nLightningDamage = MissModeRollStat(hMissile, 50, 51);
		tDamage.nColdDamage = MissModeRollStat(hMissile, 54, 55);
		tDamage.nColdLength = D2CommonStatsUnitGetValue(hMissile, 56);
		tDamage.nPoisonDamage = MissModeRollStat(hMissile, 57, 58);
		tDamage.nPoisonLength = D2CommonStatsUnitGetValue(hMissile, 59);
		nParam = ptTxt->Param1;
		if (SUnitDmgSplash(ptGame, hMissile, nX, nY, nParam, &tDamage, FALSE, FALSE, NULL, 0))
			return 1;
		else
			return 3;
	}
	return 0;
}

int __fastcall MissModeHitFreezingArrow(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	DAMAGE tDamage;
	int nParam;
	int nX;
	int nY;
	int i;

	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.nPhysicalDamage = MissModeRollStat(hMissile, 21, 22);
	tDamage.nColdDamage = MissModeRollStat(hMissile, 54, 55);
	tDamage.nFreezeLength = D2CommonStatsUnitGetValue(hMissile, 56);

	if (SUnitDmgSplash(ptGame, hMissile, ((int)D2CommonUnitGetX(hMissile)), ((int)D2CommonUnitGetY(hMissile)), ((int)D2CommonSkillsGetTypeParamOne(SKILL_FREEZING_ARROW)), &tDamage, FALSE, FALSE, NULL, 0))
		return 1;
	else
		return 3;
}

int __fastcall MissModeHitHolyBolt(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	UNIT *hOwner;
	int nMin;
	int nRange;
	int nRoll;
	int nLife;
	int nMaxLife;
	MONSTATSTXT *ptStats;

	if (hTarget)
	{
		hOwner = SUnitGetOwner(ptGame, hMissile);
		if (hOwner && hOwner->eType == UNIT_PLAYER
			&& (SUnitIsTargetPet(ptGame, hOwner, hTarget, SKILL_HOLY_BOLT)
			|| SUnitIsTargetAlly(ptGame, hOwner, hTarget, SKILL_HOLY_BOLT)))
		{
			nMin = D2CommonSkillsCalcParamOneByLevel(SKILL_HOLY_BOLT, D2CommonMissileGetLevel(hMissile)) << 8;
			nRange
				= (D2CommonSkillsCalcParamThreeByLevel(SKILL_HOLY_BOLT,
					D2CommonMissileGetLevel(hMissile)) << 8) - nMin;
			if (nRange)
				nRoll = (int)D2CommonSeedRand(&hMissile->tSeed, nRange);
			else
				nRoll = 0;
			nLife = D2CommonStatsUnitGetValue(hTarget, 6);
			nMaxLife = D2CommonStatsUnitCalcMaxHp(hTarget);
			nLife += nRoll + nMin;
			if (nLife > nMaxLife)
				nLife = nMaxLife;
			D2CommonStatsUnitSetValue(hTarget, 6, nLife);
			D2CommonUnitAddAnimOverlay(hTarget, 26, 0);
			return 1;
		}
		switch (hTarget->eType)
		{
		case UNIT_PLAYER:
		lFour:
			return 4;
		case UNIT_MONSTER:
			ptStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(hTarget->dwClassId);
			if (!ptStats->bLUndead && !ptStats->bHUndead)
				goto lFour;
			return 3;
		default:
			return 4;
		}
	}
	return 1;
}
int __fastcall MissModeHitBattleCommand(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	UNIT *hOwner;

	hOwner = SUnitGetOwner(ptGame, hMissile);
	SkillsBattleCommandApply(ptGame, hTarget, hOwner, (int)D2CommonMissileGetLevel(hMissile));
	return 1;
}

int __fastcall MissModeHitBattleOrders(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	UNIT *hOwner;

	hOwner = SUnitGetOwner(ptGame, hMissile);
	SkillsBattleOrdersApply(ptGame, hTarget, hOwner, (int)D2CommonMissileGetLevel(hMissile));
	return 1;
}

BOOL __fastcall MissModeCreateRingOfSixteen(GAME *ptGame, UNIT *hOwner, UNIT *hSpawn, int eMissile, int nType,
	int nLevel)
{
	MISSILESTXT *ptTxt = (MISSILESTXT *)D2CommonMissileTblsGetMissile(eMissile);
	int anX[16] = { 0, 1, 2, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1 };
	int anY[16] = { 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1, 0, 1, 2, 2 };
	MISSILECREATE tCreate;
	int nStep;
	int i;

	if (!hOwner && !hSpawn)
		return FALSE;
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = 31;
	tCreate.hSource = hOwner;
	tCreate.hSpawnLocation = hSpawn;
	tCreate.eMissile = eMissile;
	tCreate.nX = (int)D2CommonUnitGetX(hSpawn);
	tCreate.nY = (int)D2CommonUnitGetY(hSpawn);
	tCreate.nLevel = nLevel;
	tCreate.nSubLoops = nLevel - 1;
	tCreate.nVelocity = ptTxt->Param1 << 7;
	for (i = 0; i < 16; i += 2)
	{
		tCreate.nDX = anX[i];
		tCreate.nDY = anY[i];
		MissilesCreate(ptGame, &tCreate);
	}
	tCreate.nVelocity = ptTxt->Param2 << 7;
	if (nType == 2)
		nStep = 2;
	else if (nType == 3)
		nStep = 1;
	else
		return TRUE;
	for (i = 1; i < 16; i += nStep)
	{
		tCreate.nDX = anX[i];
		tCreate.nDY = anY[i];
		MissilesCreate(ptGame, &tCreate);
	}
	return TRUE;
}

int __fastcall MissModeHitPlagueJavelin(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{

	if (!hTarget)
		hTarget = hMissile;
	MissModeCreateRingOfSixteen(ptGame, SUnitGetOwner(ptGame, hMissile), hTarget, MISSILE_PLAGUEJAVCLOUD, 3,
		(int)D2CommonMissileGetLevel(hMissile));
	return 3;
}

int __fastcall MissModeHitRancidGasPotion(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{

	if (!hTarget)
		hTarget = hMissile;
	MissModeCreateRingOfSixteen(ptGame, SUnitGetOwner(ptGame, hMissile), hTarget, MISSILE_RANCIDGASCLOUD, 1,
		(int)D2CommonMissileGetLevel(hMissile));
	return 3;
}

int __fastcall MissModeHitChokingGasPotion(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{

	if (!hTarget)
		hTarget = hMissile;
	MissModeCreateRingOfSixteen(ptGame, SUnitGetOwner(ptGame, hMissile), hTarget, MISSILE_CHOKINGGASCLOUD, 2,
		(int)D2CommonMissileGetLevel(hMissile));
	return 3;
}

int __fastcall MissModeHitStranglingGasPotion(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{

	if (!hTarget)
		hTarget = hMissile;
	MissModeCreateRingOfSixteen(ptGame, SUnitGetOwner(ptGame, hMissile), hTarget, MISSILE_STRANGLINGGASCLOUD,
		3, (int)D2CommonMissileGetLevel(hMissile));
	return 3;
}

BOOL __fastcall MissModeCreateRingOfEight(GAME *ptGame, UNIT *hOwner, UNIT *hSpawn, int eMissile, int nType,
	int nLevel)
{
	MISSILESTXT *ptTxt = (MISSILESTXT *)D2CommonMissileTblsGetMissile(eMissile);
	int anX[8] = { 0, 2, 2, 2, 0, -2, -2, -2 };
	int anY[8] = { 2, 2, 0, -2, -2, -2, 0, 2 };
	MISSILECREATE tCreate;
	int i;

	if (!hOwner && !hSpawn)
		return FALSE;
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = 31;
	tCreate.hSource = hOwner;
	tCreate.hSpawnLocation = hSpawn;
	tCreate.eMissile = eMissile;
	tCreate.nX = (int)D2CommonUnitGetX(hSpawn);
	tCreate.nY = (int)D2CommonUnitGetY(hSpawn);
	tCreate.nLevel = nLevel;
	tCreate.nSubLoops = nLevel - 1;
	tCreate.nVelocity = ptTxt->Param1 << 7;
	for (i = 0; i < 8; i++)
	{
		tCreate.nDX = anX[i];
		tCreate.nDY = anY[i];
		MissilesCreate(ptGame, &tCreate);
	}
	return TRUE;
}

int __fastcall MissModeHitPantherPot(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{

	if (!hTarget)
		hTarget = hMissile;
	MissModeCreateRingOfEight(ptGame, SUnitGetOwner(ptGame, hMissile), hTarget, MISSILE_RANCIDGASCLOUD, 1,
		(int)D2CommonMissileGetLevel(hMissile));
	return 3;
}

void __fastcall MissModeDamageRogue(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, DAMAGE *ptDamage)
{
	UNIT *hOwner;

	hOwner = SUnitGetOwner(ptGame, hMissile);
	if (hOwner && (hOwner->dwFlags0 & 512))
	{
		if (!hTarget)
			return;
		if (hTarget->eType == UNIT_MONSTER)
		{
			if (MonsterUniqueIsCounted(((MONSTERDATA *)hTarget->uTypeData)->ptUnique))
				return;
			if (D2CommonSeedRand(&hOwner->tSeed, 100) >= 50)
				return;
		}
		hTarget->dwFlags0 |= 131072;
		return;
	}
	if (hTarget)
		hTarget->dwFlags0 |= 131072;
}

void __fastcall MissModeDamageWarcry(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, DAMAGE *ptDamage)
{
	ptDamage->nStunLength = D2CommonSkillsCalcParamOneByLevel(SKILL_WAR_CRY,
		D2CommonMissileGetLevel(hMissile));
}

int __fastcall MissModeHitImmolationArrow(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	DAMAGE tDamage;
	int nParam;
	int nX;
	int nY;
	int i;

	MissilesCreateInRadius(ptGame, hMissile, (int)D2CommonSkillsGetTypeParamOne(SKILL_IMMOLATION_ARROW),
		MISSILE_IMMOLATIONFIRE);
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.nPhysicalDamage = MissModeRollStat(hMissile, 21, 22);
	tDamage.nFireDamage = MissModeRollStat(hMissile, 48, 49);

	if (SUnitDmgSplash(ptGame, hMissile, ((int)D2CommonUnitGetX(hMissile)), ((int)D2CommonUnitGetY(hMissile)), ((int)D2CommonSkillsGetTypeParamTwo(SKILL_IMMOLATION_ARROW)), &tDamage, FALSE, FALSE, NULL, 0))
		return 1;
	else
		return 3;
}

void __fastcall MissModeDamageIceBlast(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, DAMAGE *ptDamage)
{
	ptDamage->nFreezeLength = D2CommonSkillsCalcParamOneByLevel(SKILL_ICE_BLAST,
		D2CommonMissileGetLevel(hMissile));
}

void __fastcall MissModeDamageBlessedHammer(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, DAMAGE *ptDamage)
{
	MONSTATSTXT *ptStats;

	if (hTarget->eType != UNIT_MONSTER)
		return;
	ptStats = (MONSTATSTXT *)D2CommonMonsterTblsGetStats(hTarget->dwClassId);
	if (ptStats->bLUndead || ptStats->bHUndead)
		ptDamage->nPhysicalDamage += ptDamage->nPhysicalDamage >> 1;
}

int __fastcall MissModeHitChainLightning(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	UNIT *hOwner;
	int nSkill;
	int nX;
	int eClass;
	int nJumps;
	int nY;
	SKILLFINDTARGET tFind;
	MISSILECREATE tCreate;
	UNIT *hPrev;
	UNIT *hBolt;
	int i;

	hPrev = hTarget;
	hOwner = SUnitGetOwner(ptGame, hMissile);
	nJumps = (int)D2CommonMissileGetScratchFirst(hMissile);
	nX = (int)D2CommonUnitGetX(hMissile);
	nY = (int)D2CommonUnitGetY(hMissile);
	eClass = hMissile->dwClassId;
	switch (eClass)
	{
	case 93:
		nSkill = 53;
		break;
	case 232:
		nSkill = 34;
		break;
	default:
		return 3;
	}
	if (nJumps <= 1 || !hTarget || !hOwner)
		return 3;
	for (i = 0; i < (int)(sizeof(SKILLFINDTARGET) / sizeof(DWORD)); i++)
		((DWORD *)&tFind)[i] = 0;
	tFind.hUnit = hOwner;
	tFind.dwUnitId = hTarget->dwId;
	tFind.dwFlags = 3;
	tFind.nX = nX;
	tFind.nY = nY;
	tFind.nRange = (int)D2CommonSkillsGetTypeParamOne(nSkill);
	hTarget = SkillsFindTarget(ptGame, &tFind);
	if (!hTarget || hTarget == hPrev)
		return 3;
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.hSource = hOwner;
	tCreate.dwFlags = 33;
	tCreate.hTarget = hTarget;
	tCreate.eMissile = eClass;
	tCreate.nX = nX;
	tCreate.nY = nY;
	tCreate.nDX = (int)D2CommonUnitGetX(hTarget);
	tCreate.nDY = (int)D2CommonUnitGetY(hTarget);
	tCreate.nLevel = (int)D2CommonMissileGetLevel(hMissile);
	hBolt = MissilesCreate(ptGame, &tCreate);
	if (hBolt)
		D2CommonMissileSetScratchFirst(hBolt, nJumps - 1);
	return 3;
}

int __fastcall MissModeHitFistOfTheHeavens(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	UNIT *hUnit;
	UNIT *hOwner;
	int nX;
	int nY;
	void *hRoom;
	UNITFINDDATA tFind;
	UNITFINDARGS tArgs;
	MISSILECREATE tCreate;
	int nLevel;
	int nMax;
	DWORD dwId;
	int i;

	if (hTarget)
		return 0;

	hUnit = SUnitFind(ptGame, D2CommonMissileGetScratchFirst(hMissile), D2CommonMissileGetScratchSecond(hMissile));
	if (!hUnit)
		return 0;
	hOwner = SUnitGetOwner(ptGame, hMissile);
	if (!hOwner)
		return 0;
	MissModeHitUnit(ptGame, hMissile, hUnit, NULL);
	nX = (int)D2CommonUnitGetX(hUnit);
	nY = (int)D2CommonUnitGetY(hUnit);
	for (i = 0; i < (int)(sizeof(UNITFINDARGS) / sizeof(DWORD)); i++)
		((DWORD *)&tArgs)[i] = 0;
	tArgs.dwFlags = 9859;
	tArgs.hExcluded = hOwner;
	tArgs.nX = nX;
	tArgs.nY = nY;
	tArgs.nDistance = 20;
	tArgs.pFieldPair = (void *)D2CommonFieldTblsAllocPair();
	D2CommonFieldTblsSetPair(tArgs.pFieldPair, nX, nY);
	hRoom = D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hMissile), nX, nY);
	if (!hRoom)
		return 0;
	D2CommonUnitFindsInit(ptGame->pMemPool, &tFind, hRoom, nX, nY, 20, 0,
		&tArgs);
	tFind.dwFlags = 9859;
	D2CommonUnitFindsFindUnits(&tFind);
	nLevel = (int)D2CommonMissileGetLevel(hMissile);
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = 32;
	tCreate.hSource = hOwner;
	tCreate.hSpawnLocation = hUnit;
	tCreate.nLevel = nLevel;
	tCreate.eMissile = 234;
	nMax = D2CommonSkillsCalcParamOneByLevel(SKILL_FIST_OF_THE_HEAVENS, nLevel);
	for (i = 0; i < tFind.nNumUnits && nMax > 0; i++)
	{
		if (SUnitIsHostile(ptGame, hOwner, tFind.phUnitsArray[i]))
		{
			tCreate.nDX = (int)D2CommonUnitGetX(tFind.phUnitsArray[i]);
			tCreate.nDY = (int)D2CommonUnitGetY(tFind.phUnitsArray[i]);
			if (tCreate.nDX != nX || tCreate.nDY != nY)
				MissilesCreate(ptGame, &tCreate);
		}
	}
	D2CommonUnitFindsFreeArray(&tFind);
	D2CommonFieldTblsFreePair(tArgs.pFieldPair);
	return 1;
}

int __fastcall MissModeHitGlacialSpike(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	int nX;
	int nY;
	int nLevel;
	int nParam;
	int nLength;
	DAMAGE tDamage;
	int i;

	nX = (int)D2CommonUnitGetX(hMissile);
	nY = (int)D2CommonUnitGetY(hMissile);
	nLevel = (int)D2CommonMissileGetLevel(hMissile);
	nParam = D2CommonSkillsCalcParamOneByLevel(SKILL_GLACIAL_SPIKE, nLevel);
	nLength = D2CommonSkillsCalcParamThreeByLevel(SKILL_GLACIAL_SPIKE, nLevel);
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.nColdDamage = MissModeRollStat(hMissile, 54, 55);
	tDamage.nFreezeLength = nLength;
	if (SUnitDmgSplash(ptGame, hMissile, nX, nY, nParam, &tDamage, FALSE, FALSE, NULL, 0))
		return 1;
	else
		return 3;
}

int __fastcall MissModeHitMonGlacialSpike(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	int nX;
	int nY;
	int nLevel;
	int nParam;
	int nLength;
	DAMAGE tDamage;
	int i;

	nX = (int)D2CommonUnitGetX(hMissile);
	nY = (int)D2CommonUnitGetY(hMissile);
	nLevel = (int)D2CommonMissileGetLevel(hMissile);
	nParam = D2CommonSkillsCalcParamOneByLevel(SKILL_PRIMESPIKE, nLevel);
	nLength = D2CommonSkillsCalcParamThreeByLevel(SKILL_PRIMESPIKE, nLevel);
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.nColdDamage = MissModeRollStat(hMissile, 54, 55);
	tDamage.nFreezeLength = nLength;
	if (SUnitDmgSplash(ptGame, hMissile, nX, nY, nParam, &tDamage, FALSE, FALSE, NULL, 0))
		return 1;
	else
		return 3;
}

int __fastcall MissModeHitMeteor(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	int nResult = 1;
	int anX[18] = { 2, -2, 0, 0, -3, 0, 3, -1, 1, -1, 2, -4, -3, -1, 0, 1, 3, 4 };
	int anY[18] = { -2, -2, 2, 5, 3, 3, 3, 2, 1, -1, -1, -2, -2, -3, -4, -3, -3, -2 };
	int nSkill;
	int nX;
	int nY;
	int nParam;
	UNIT *hOwner;
	int nLevel;
	int nMin;
	int nRange;
	int nRoll;
	MISSILECREATE tCreate;
	DAMAGE tDamage;
	int i;

	switch (hMissile->dwClassId)
	{
	case 101:
		nSkill = 56;
		break;
	case 133:
		nSkill = 171;
		break;
	default:
		return 1;
	}
	nLevel = (int)D2CommonMissileGetLevel(hMissile);
	nX = (int)D2CommonUnitGetX(hMissile);
	nY = (int)D2CommonUnitGetY(hMissile);
	nParam = D2CommonSkillsCalcParamOneByLevel(nSkill, nLevel);
	hOwner = SUnitGetOwner(ptGame, hMissile);
	if (!hOwner)
		return 1;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	nMin = D2CommonSkillsCalcEMinByLevel(nSkill, nLevel);
	nRange = D2CommonSkillsCalcEMaxByLevel(nSkill, nLevel) - nMin;
	if (nRange)
		nRoll = (int)D2CommonSeedRand(&hMissile->tSeed, nRange);
	else
		nRoll = 0;
	tDamage.nFireDamage = (nRoll + nMin) << 8;
	tDamage.dwFlags0 = 4;
	if (!SUnitDmgSplash(ptGame, hMissile, nX, nY, nParam, &tDamage, FALSE, FALSE, NULL, 1411))
		nResult = 3;
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = 32769;
	tCreate.hSource = hOwner;
	tCreate.nLevel = nLevel;
	tCreate.eMissile = 240;
	tCreate.nRange = D2CommonSkillsCalcParamThreeByLevel(nSkill, nLevel);
	for (i = 0; i < 18; i++)
	{
		tCreate.nX = anX[i] + nX;
		tCreate.nY = anY[i] + nY;
		MissilesCreate(ptGame, &tCreate);
	}
	return nResult;
}

int __fastcall MissModeHitBattleCry(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	UNIT *hOwner;
	int nLevel;
	SKILLSTATE tState;
	void *hList;
	int i;

	hOwner = SUnitGetOwner(ptGame, hMissile);
	nLevel = (int)D2CommonMissileGetLevel(hMissile);
	if (!hOwner || !hTarget)
		return 0;
	if (SUnitIsDead(hTarget))
		return 0;
	if (!SUnitIsHostile(ptGame, hOwner, hTarget))
		return 0;
	for (i = 0; i < (int)(sizeof(SKILLSTATE) / sizeof(DWORD)); i++)
		((DWORD *)&tState)[i] = 0;
	tState.hSource = hOwner;
	tState.hTarget = hTarget;
	tState.nSkill = 146;
	tState.nLevel = nLevel;
	tState.nDuration = D2CommonSkillsCalcParamOneByLevel(SKILL_BATTLE_CRY, nLevel);
	tState.dwStatId = 171;
	tState.nStatValue = D2CommonSkillsCalcParamThreeByLevel(SKILL_BATTLE_CRY, nLevel);
	tState.dwState = 89;
	tState.pfnRemove = 0;
	hList = SkillsSetState(&tState);
	if (hList)
		D2CommonStatsListSetValueB(hList, 25, D2CommonSkillsCalcParamFiveByLevel(SKILL_BATTLE_CRY, nLevel));
	return 0;
}

int __fastcall MissModeModeCloud(GAME *ptGame, UNIT *hMissile)
{
	void *hRoom;
	void *ptPath;
	int nX;
	int nY;

	hRoom = (void *)D2CommonUnitGetPathRoom(hMissile);
	ptPath = hMissile->ptPath;
	nX = (int)D2CommonUnitGetX(hMissile);
	nY = (int)D2CommonUnitGetY(hMissile);
	if (!ptPath || !D2CommonPathGetVelocity(ptPath))
		D2CommonCollisnSetPoint(hRoom, nX, nY, 64);
	return MissModeModeDefault(ptGame, hMissile);
}

int __fastcall MissModeModePoisonJav(GAME *ptGame, UNIT *hMissile)
{
	MISSILECREATE tCreate;
	int nLevel;
	int i;

	if (D2CommonStepTestSquareCrossed(hMissile->ptPath))
	{
		for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.dwFlags = 13;
		tCreate.hSpawnLocation = tCreate.hSource = SUnitGetOwner(ptGame, hMissile);
		tCreate.eMissile = 39;
		tCreate.nX = (int)D2CommonUnitGetX(hMissile);
		tCreate.nY = (int)D2CommonUnitGetY(hMissile);
		nLevel = D2CommonStatsUnitGetValue(hMissile, 12);
		tCreate.nSubLoops = nLevel * 2 - 2;
		tCreate.nLevel = nLevel;
		MissilesCreate(ptGame, &tCreate);
	}
	return MissModeModeDefault(ptGame, hMissile);
}

int __fastcall MissModeModePlagueJavelin(GAME *ptGame, UNIT *hMissile)
{
	MISSILECREATE tCreate;
	int nLevel;
	int i;

	if (D2CommonStepTestSquareCrossed(hMissile->ptPath))
	{
		for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.dwFlags = 13;
		tCreate.hSpawnLocation = tCreate.hSource = SUnitGetOwner(ptGame, hMissile);
		tCreate.eMissile = 221;
		tCreate.nX = (int)D2CommonUnitGetX(hMissile);
		tCreate.nY = (int)D2CommonUnitGetY(hMissile);
		nLevel = D2CommonStatsUnitGetValue(hMissile, 12);
		tCreate.nSubLoops = nLevel * 2 - 2;
		tCreate.nLevel = nLevel;
		MissilesCreate(ptGame, &tCreate);
	}
	return MissModeModeDefault(ptGame, hMissile);
}

int __fastcall MissModeModeFire(GAME *ptGame, UNIT *hMissile)
{
	MISSILESTXT *ptTxt;
	int nFrame;
	int nRange;
	int nRoll;

	ptTxt = (MISSILESTXT *)D2CommonMissileTblsGetMissile(hMissile->dwClassId);
	nFrame = UNIT8(hMissile)->nAnimPos >> 8;
	D2CommonCollisnSetSize(D2CommonUnitGetPathRoom(hMissile), D2CommonUnitGetX(hMissile), D2CommonUnitGetY(hMissile), D2CommonUnitGetSizeX(hMissile), 64);
	if (nFrame == ptTxt->SubStart - 1)
	{
		nRange = ptTxt->SubStop - ptTxt->SubStart;
		if (nRange)
			nRoll = (int)D2CommonSeedRand(&hMissile->tSeed, nRange);
		else
			nRoll = 0;
		UNIT8(hMissile)->nAnimPos = (nRoll + nFrame) << 8;
	}
	else if ((int)D2CommonMissileGetRangeLeft(hMissile) == ptTxt->SubStart)
		UNIT8(hMissile)->nAnimPos = (ptTxt->SubStart - 3) << 8;
	else if ((int)D2CommonMissileGetRangeLeft(hMissile) < (int)ptTxt->SubStart)
	{
		nFrame -= 2;
		if (nFrame < 0)
			nFrame = 0;
		UNIT8(hMissile)->nAnimPos = nFrame << 8;
	}
	return MissModeModeDefault(ptGame, hMissile);
}

void __fastcall MissModeDamageFire(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, DAMAGE *ptDamage)
{
	if (D2CommonSeedRand(&hMissile->tSeed, 100) < 15)
		ptDamage->wFlags |= 16384;
}

int __fastcall MissModeModeFirewallMaker(GAME *ptGame, UNIT *hMissile)
{
	int nX;
	int nY;
	MISSILECREATE tCreate;
	int i;

	if (!SUnitGetOwner(ptGame, hMissile))
		return 2;
	if (D2CommonStepTestSquareCrossed(hMissile->ptPath))
	{
		nX = (int)D2CommonUnitGetX(hMissile);
		nY = (int)D2CommonUnitGetY(hMissile);
		for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.dwFlags = 33;
		tCreate.hSource = SUnitGetOwner(ptGame, hMissile);
		tCreate.nX = nX;
		tCreate.nY = nY;
		tCreate.nLevel = D2CommonStatsUnitGetValue(hMissile, 12);
		if (hMissile->dwClassId != 130)
			tCreate.eMissile = 69;
		else if (tCreate.hSource->dwClassId != 45)
			tCreate.eMissile = 131;
		else
			tCreate.eMissile = 331;
		tCreate.nDX = (int)D2CommonUnitGetX(hMissile);
		tCreate.nDY = (int)D2CommonUnitGetY(hMissile);
		MissilesCreate(ptGame, &tCreate);
	}
	return MissModeModeDefault(ptGame, hMissile);
}

int __fastcall MissModeModeBoneWallMaker(GAME *ptGame, UNIT *hMissile)
{
	UNIT *hTargetUnit;
	UNIT *hOwner;
	SKILLSUMMON tSummon;
	UNIT *hWall;
	int nLevel;
	int nLife;
	int i;

	if (!(!SUnitGetOwner(ptGame, hMissile)))
	{
		if (!D2CommonMissileGetScratchSecond(hMissile))
			return 2;
		if (D2CommonStepTestSquareCrossed(hMissile->ptPath))
		{
			hOwner = SUnitGetOwner(ptGame, hMissile);
			hTargetUnit = SUnitFind(ptGame, UNIT_MONSTER, D2CommonMissileGetScratchFirst(hMissile));
			if (hOwner && hTargetUnit)
			{
				for (i = 0; i < (int)(sizeof(SKILLSUMMON) / sizeof(DWORD)); i++)
					((DWORD *)&tSummon)[i] = 0;
				tSummon.dwFlags = 13;
				tSummon.hOwner = hOwner;
				tSummon.dwClassId = 344;
				tSummon.dwAiKind = 1;
				tSummon.dwMode = 8;
				tSummon.nX = (int)D2CommonUnitGetX(hMissile);
				tSummon.nY = (int)D2CommonUnitGetY(hMissile);
				tSummon.dwPetType = 0;
				hWall = SkillsSummon(ptGame, &tSummon);
				if (hWall)
				{
					AiSetOwner(ptGame, hWall, D2CommonMissileGetScratchFirst(hMissile), 1, 0, 0);
					AiAddMinion(ptGame, hTargetUnit, hWall);
					MonsterUniqueAddMod(ptGame, hWall, 15, 0);
					nLevel = (int)D2CommonSkillsCalcNodeLevel(hOwner,
						D2CommonSkillsFindByKey(hOwner, 78), 1);
					nLife = D2CommonStatsUnitCalcMaxHp(hWall);
					nLife += (int)D2CommonSkillsGetTypeParamOne(SKILL_BONE_WALL) * (nLevel - 1) * nLife / 100;
					D2CommonStatsUnitSetValue(hWall, 6, nLife);
					D2CommonStatsUnitSetValue(hWall, 7, nLife);
					TargetsAddToFurtherList(ptGame, hWall, 0, 9);
					D2CommonMissileSetScratchSecond(hMissile, D2CommonMissileGetScratchSecond(hMissile) - 1);
				}
			}
		}
		return MissModeModeDefault(ptGame, hMissile);
	}
	return 2;
}

int __fastcall MissModeModeGrimWard(GAME *ptGame, UNIT *hMissile)
{
	UNIT *hOwner;
	MISSILECREATE tCreate;
	UNIT *hScare;
	int i;

	if (!((int)D2CommonMissileCalcRangeUsed(hMissile) % 6))
	{
		hOwner = SUnitGetOwner(ptGame, hMissile);
		if (hOwner)
		{
			for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
				((DWORD *)&tCreate)[i] = 0;
			tCreate.dwFlags = 2;
			tCreate.hSource = hOwner;
			tCreate.hSpawnLocation = hMissile;
			tCreate.nLevel = (int)D2CommonMissileGetLevel(hMissile);
			tCreate.eMissile = 259;
			tCreate.nDX = 10 - (int)D2CommonSeedRand(&hMissile->tSeed, 20);
			tCreate.nDY = 10 - (int)D2CommonSeedRand(&hMissile->tSeed, 20);
			hScare = MissilesCreate(ptGame, &tCreate);
			if (hScare)
				D2CommonMissileSetScratchFirst(hScare, hMissile->dwId);
		}
	}
	return MissModeModeDefault(ptGame, hMissile);
}

int __fastcall MissModeHitGrimWardScare(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	UNIT *hWard;
	int nRange;
	DWORD dwDuration;

	if (hTarget && hTarget->eType == UNIT_MONSTER)
	{
		hWard = SUnitFind(ptGame, UNIT_MISSILE, D2CommonMissileGetScratchFirst(hMissile));
		if (hWard)
		{
			nRange = D2CommonSkillsCalcParamOneByLevel(SKILL_GRIM_WARD, D2CommonMissileGetLevel(hMissile));
			if ((int)D2CommonUnitGetProximityMetric(hWard, hTarget) < nRange)
			{
				dwDuration = D2CommonSkillsGetTypeParamSix(SKILL_GRIM_WARD);
				MonsterScare(ptGame, hWard, hTarget, D2CommonSkillsGetTypeParamFive(SKILL_GRIM_WARD),
					dwDuration);
			}
		}
	}
	return 1;
}

int __fastcall MissModeModeFrozenOrb(GAME *ptGame, UNIT *hMissile)
{
	int anX[64] =
	{
		30, 29, 29, 28, 27, 26, 24, 23, 21, 19, 16, 14, 11, 8, 5, 2,
		0, -2, -5, -8, -11, -14, -16, -19, -21, -23, -24, -26, -27, -28, -29, -29,
		-30, -29, -29, -28, -27, -26, -24, -23, -21, -19, -16, -14, -11, -8, -5, -2,
		0, 2, 5, 8, 11, 14, 16, 19, 21, 23, 24, 26, 27, 28, 29, 29,
	};
	int anY[64] =
	{
		0, 2, 5, 8, 11, 14, 16, 19, 21, 23, 24, 26, 27, 28, 29, 29,
		30, 29, 29, 28, 27, 26, 24, 23, 21, 19, 16, 14, 11, 8, 5, 2,
		0, -2, -5, -8, -11, -14, -16, -19, -21, -23, -24, -26, -27, -28, -29, -29,
		-30, -29, -29, -28, -27, -26, -24, -23, -21, -19, -16, -14, -11, -8, -5, -2,
	};
	int nDirections;
	int nDirection;
	UNIT *hOwner;
	MISSILECREATE tCreate;
	int i;

	nDirections = 64;
	hOwner = SUnitGetOwner(ptGame, hMissile);
	if (hOwner)
	{
		for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.dwFlags = 2;
		tCreate.hSource = hOwner;
		tCreate.hSpawnLocation = hMissile;
		tCreate.nLevel = (int)D2CommonMissileGetLevel(hMissile);
		tCreate.eMissile = 261;
		nDirection = (int)D2CommonMissileGetScratchFirst(hMissile);
#line 1567
		D2ASSERT(nDirection < nDirections);
#line 1568
		D2ASSERT(nDirection >= 0);
		tCreate.nDX = anX[nDirection];
		tCreate.nDY = anY[nDirection];
		MissilesCreate(ptGame, &tCreate);
		nDirection = (nDirection + 19) % 64;
		D2CommonMissileSetScratchFirst(hMissile, nDirection);
	}
	return MissModeModeDefault(ptGame, hMissile);
}

int __fastcall MissModeModeFrozenOrbNova(GAME *ptGame, UNIT *hMissile)
{
	int nFrame;
	void *ptPath;
	int nA;
	int nB;
	int nOldA;
	int nNegB;
	int nY;

	nFrame = (int)D2CommonMissileCalcRangeUsed(hMissile);
	if (nFrame < 6 && !(nFrame % 2))
	{
		ptPath = hMissile->ptPath;
		nA = (int)D2CommonMissileGetScratchFirst(hMissile);
		nB = (int)D2CommonMissileGetScratchSecond(hMissile);
		if (nA > 0)
		{
			if (nB > 0)
				nNegB = -nB;
			else
				nNegB = -nB;
		}
		else
			nNegB = -nB;
		nOldA = nA;
		nA = (nA + nNegB) / 2;
		nB = (nOldA + nB) / 2;
		nY = (int)D2CommonUnitGetY(hMissile) + nB;
		D2CommonPathSetTargetCoord(ptPath, D2CommonUnitGetX(hMissile) + nA, nY);
		D2CommonPathCompute(ptPath, hMissile);
		D2CommonMissileSetScratchFirst(hMissile, nA);
		D2CommonMissileSetScratchSecond(hMissile, nB);
	}
	return MissModeModeDefault(ptGame, hMissile);
}

int __fastcall MissModeHitFrozenOrb(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	int anX[64] =
	{
		30, 29, 29, 28, 27, 26, 24, 23, 21, 19, 16, 14, 11, 8, 5, 2,
		0, -2, -5, -8, -11, -14, -16, -19, -21, -23, -24, -26, -27, -28, -29, -29,
		-30, -29, -29, -28, -27, -26, -24, -23, -21, -19, -16, -14, -11, -8, -5, -2,
		0, 2, 5, 8, 11, 14, 16, 19, 21, 23, 24, 26, 27, 28, 29, 29,
	};
	int anY[64] =
	{
		0, 2, 5, 8, 11, 14, 16, 19, 21, 23, 24, 26, 27, 28, 29, 29,
		30, 29, 29, 28, 27, 26, 24, 23, 21, 19, 16, 14, 11, 8, 5, 2,
		0, -2, -5, -8, -11, -14, -16, -19, -21, -23, -24, -26, -27, -28, -29, -29,
		-30, -29, -29, -28, -27, -26, -24, -23, -21, -19, -16, -14, -11, -8, -5, -2,
	};
	UNIT *hOwner;
	MISSILECREATE tCreate;
	UNIT *hShard;
	int i;

	if (hTarget)
		return 3;
	hOwner = SUnitGetOwner(ptGame, hMissile);
	if (!hOwner)
		return 3;
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = 2;
	tCreate.hSource = hOwner;
	tCreate.hSpawnLocation = hMissile;
	tCreate.nLevel = (int)D2CommonMissileGetLevel(hMissile);
	tCreate.eMissile = 262;
	for (i = 0; i < 64; i += 4)
	{
		tCreate.nDX = anX[i];
		tCreate.nDY = anY[i];
		hShard = MissilesCreate(ptGame, &tCreate);
		if (hShard)
		{
			D2CommonMissileSetScratchFirst(hShard, anX[i]);
			D2CommonMissileSetScratchSecond(hShard, anY[i]);
		}
	}
	return 3;
}

int __fastcall MissModeHitGrimWardStart(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	int eMissile;
	UNIT *hOwner;
	int nLevel;
	int nRange;
	MISSILECREATE tCreate;
	int i;

	hOwner = SUnitGetOwner(ptGame, hMissile);
	nLevel = (int)D2CommonMissileGetLevel(hMissile);
	if (!hOwner)
		return 1;
	switch (hMissile->dwClassId)
	{
	case 249:
		eMissile = 250;
		break;
	case 252:
		eMissile = 253;
		break;
	case 255:
		eMissile = 256;
		break;
	default:
		return 1;
	}
	nRange = D2CommonSkillsCalcParamThreeByLevel(SKILL_GRIM_WARD, nLevel);
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = 32769;
	tCreate.hSource = hOwner;
	tCreate.eMissile = eMissile;
	tCreate.nRange = nRange;
	tCreate.nLevel = nLevel;
	tCreate.nX = (int)D2CommonUnitGetX(hMissile);
	tCreate.nY = (int)D2CommonUnitGetY(hMissile);
	MissilesCreate(ptGame, &tCreate);
	return 1;
}

int __fastcall MissModeModeBatLightning(GAME *ptGame, UNIT *hMissile)
{
	int nX;
	int nY;
	MISSILECREATE tCreate;
	int i;

	nX = (int)D2CommonUnitGetX(hMissile);
	nY = (int)D2CommonUnitGetY(hMissile);
	if (D2CommonStepTestSquareCrossed(hMissile->ptPath))
	{
		for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.dwFlags = 1;
		tCreate.hSource = SUnitGetOwner(ptGame, hMissile);
		tCreate.nX = nX;
		tCreate.nY = nY;
		tCreate.nLevel = (int)D2CommonMissileGetLevel(hMissile);
		tCreate.eMissile = 124;
		MissilesCreate(ptGame, &tCreate);
	}
	return MissModeModeDefault(ptGame, hMissile);
}

int __fastcall MissModeModeMonBlizCenter(GAME *ptGame, UNIT *hMissile)
{
	int nLevel;
	int nDelay;
	int nRange;

	nLevel = (int)D2CommonMissileGetLevel(hMissile);
	nRange = nLevel + 5;
	nDelay = 8 - nLevel;
	if (nDelay < 3)
		nDelay = 3;
	MissModeCreateNearby(ptGame, hMissile, nRange, nDelay, MISSILE_BLIZZARD1, 5);
	return MissModeModeDefault(ptGame, hMissile);
}

UNIT *__fastcall MissModeCreateNearby(GAME *ptGame, UNIT *hMissile, int nRange, int nDelay, int eMissile,
	DWORD dwPattern)
{
	UNIT *hOwner;
	MISSILECREATE tCreate;
	int nSpan;
	int nRollX;
	int nRollY;
	DWORD dwSeed;
	D2SEED *ptSeed;
	int i;

#line 176
	D2ASSERT(nDelay);
	if (!((int)D2CommonMissileGetRangeLeft(hMissile) % nDelay))
	{
		hOwner = SUnitGetOwner(ptGame, hMissile);
		if (!hOwner)
			return NULL;
		ptSeed = &hMissile->tSeed;
		dwSeed = D2CommonUnitGetX(hMissile);
		dwSeed += D2CommonMissileGetRangeLeft(hMissile);
		D2CommonSeedSet(ptSeed, dwSeed);
		for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.dwFlags = 3;
		tCreate.hSource = hOwner;
		tCreate.nLevel = (int)D2CommonMissileGetLevel(hMissile);
		tCreate.eMissile = eMissile;
		nRange--;
		nSpan = nRange * 2;
		if (nSpan)
			nRollX = (int)D2CommonSeedRand(ptSeed, nSpan);
		else
			nRollX = 0;
		nRollX = nRange - nRollX;
		if (nSpan)
			nRollY = (int)D2CommonSeedRand(&hMissile->tSeed, nSpan); else
			nRollY = 0;
		tCreate.nX = (int)D2CommonUnitGetX(hMissile) + nRollX;
		tCreate.nY = (int)D2CommonUnitGetY(hMissile) + (nRange - nRollY);
		if (!D2CommonUnitGetPathRoom(hMissile))
			return NULL;
		if (D2CommonCollisnTestPoint(D2CommonUnitGetPathRoom(hMissile), tCreate.nX, tCreate.nY, dwPattern))
			return NULL;
		return MissilesCreate(ptGame, &tCreate); }
	return NULL;
}

int __fastcall MissModeModeBlizzardCenter(GAME *ptGame, UNIT *hMissile)
{

	MissModeCreateNearby(ptGame, hMissile, (int)D2CommonSkillsGetTypeParamOne(SKILL_BLIZZARD),
		(int)D2CommonSkillsGetTypeParamThree(SKILL_BLIZZARD), MISSILE_BLIZZARD1, 5);
	return MissModeModeDefault(ptGame, hMissile);
}
static __inline void MissModeGuidedSteer(UNIT *hFound, UNIT *hMissile)
{
	MISSILESTXT *ptTxt;
	void *ptPath;
	int nRange;
	int nDistance;
	DWORD dwStep;
	int nX;
	int nY;

	ptTxt = (MISSILESTXT *)D2CommonMissileTblsGetMissile(hMissile->dwClassId);
	ptPath = hMissile->ptPath;
	if (hFound && SUnitIsDead(hFound))
		hFound = NULL;
	nRange = ((int)D2CommonMissileGetLevel(hMissile) - 1) * ptTxt->LevRange + ptTxt->Range;
	D2CommonMissileSetRange(hMissile, nRange);
	D2CommonMissileSetRangeLeft(hMissile, nRange);
	if (hFound)
	{
		D2CommonPathSetTarget(ptPath, hFound);
		D2CommonMissileSetScratchFirst(hMissile, 5);
		nDistance = (int)D2CommonUnitGetProximityMetric(hMissile, hFound);
	}
	else
	{
		dwStep = D2CommonMissileGetScratchSecond(hMissile);
		nX = (int)D2CommonUnitGetX(hMissile) + (short)LOWORD(dwStep);
		nY = (int)D2CommonUnitGetY(hMissile) + (short)HIWORD(dwStep);
		D2CommonPathSetTargetCoord(ptPath, nX, nY);
		D2CommonMissileSetScratchFirst(hMissile, 6);
		nDistance = D2CommonUnitUtilGetDistance(hMissile, nX, nY);
	}
	if (nDistance >= 25)
		return;
	D2CommonPathCompute(ptPath, hMissile);
}

static __inline BOOL MissModeGuidedSeek(GAME *ptGame, UNIT *hMissile)
{
	UNIT *hOwner;
	int nRadius;
	SKILLFINDTARGET tFind;
	int i;

	if (D2CommonMissileGetScratchFirst(hMissile) & 4)
		return TRUE;
	hOwner = SUnitGetOwner(ptGame, hMissile);
	if (!hOwner)
		return TRUE;
	switch (hMissile->dwClassId)
	{
	case 86:
		nRadius = (int)D2CommonSkillsGetTypeParamOne(22);
		break;
	case 193:
		nRadius = (int)D2CommonSkillsGetTypeParamOne(93);
		break;
	case 329:
		nRadius = (int)D2CommonSkillsGetTypeParamOne(211);
		break;
	default:
		return TRUE;
	}
	for (i = 0; i < (int)(sizeof(SKILLFINDTARGET) / sizeof(DWORD)); i++)
		((DWORD *)&tFind)[i] = 0;
	tFind.dwFlags = 3;
	tFind.hUnit = hOwner;
	tFind.dwUnitId = 0xFFFFFFFF;
	tFind.nX = (int)D2CommonUnitGetX(hMissile);
	tFind.nY = (int)D2CommonUnitGetY(hMissile);
	tFind.nRange = nRadius;
	MissModeGuidedSteer(SkillsFindTarget(ptGame, &tFind), hMissile);
	return FALSE;
}

int __fastcall MissModeHitGuided(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	void *ptPath;
	WORD wSight;
	DWORD dwFlags;

	ptPath = hMissile->ptPath;
	if (D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hMissile)))
		return 1;
	if (!hTarget && (D2CommonMissileGetScratchFirst(hMissile) & 4))
		return 1;
	wSight = D2CommonPathGetLineOfSight(ptPath);
	if (!hTarget && (wSight & 4))
		return 1;
	dwFlags = D2CommonMissileGetScratchFirst(hMissile);
	if (dwFlags & 1)
	{
		if (hTarget && hTarget != SUnitGetTarget(ptGame, hMissile))
			return 4;
	}
	else if (dwFlags & 2)
	{
		if ((int)D2CommonMissileGetRangeLeft(hMissile) <= 0)
		{
			if (MissModeGuidedSeek(ptGame, hMissile))
				return 1;
			return 4;
		}
		if (dwFlags & 4)
			return 3;
		else
			return 4;
	}
	return 3;
}

int __fastcall MissModeHitLightningFury(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	UNIT *hOwner;
	UNIT *hFinder;
	int nX;
	int nY;
	UNITFINDARGS tArgs;
	UNITFINDDATA tFind;
	void *hRoom;
	int nLevel;
	int nCount;
	int nMax;
	MISSILECREATE tCreate;
	UNIT *hUnit;
	int i;
	int j;

	hOwner = SUnitGetOwner(ptGame, hMissile);
	if (!(!hOwner))
	{
		hFinder = SUnitGetOwner(ptGame, hMissile);
		nX = (int)D2CommonUnitGetX(hMissile);
		nY = (int)D2CommonUnitGetY(hMissile);
		for (i = 0; i < (int)(sizeof(UNITFINDARGS) / sizeof(DWORD)); i++)
			((DWORD *)&tArgs)[i] = 0;
		tArgs.dwFlags = 8835;
		tArgs.hExcluded = hFinder;
		tArgs.nX = nX;
		tArgs.nY = nY;
		tArgs.nDistance = (int)D2CommonSkillsGetTypeParamThree(SKILL_LIGHTNING_FURY);
		tArgs.pFieldPair = (void *)D2CommonFieldTblsAllocPair();
		D2CommonFieldTblsSetPair(tArgs.pFieldPair, nX, nY);
		hRoom = D2CommonDungeonFindRoomGame(D2CommonUnitGetPathRoom(hMissile), nX, nY);
		if (hRoom)
		{
			D2CommonUnitFindsInit(ptGame->pMemPool, &tFind, hRoom, nX, nY,
				D2CommonSkillsGetTypeParamThree(SKILL_LIGHTNING_FURY), 0, &tArgs);
			tFind.dwFlags = 8835;
			D2CommonUnitFindsFindUnits(&tFind);
			D2CommonFieldTblsFreePair(tArgs.pFieldPair);
		}
		nLevel = (int)D2CommonMissileGetLevel(hMissile);
		nCount = 0;
		nMax = D2CommonSkillsCalcParamOneByLevel(SKILL_LIGHTNING_FURY, nLevel);
		for (i = 0; i < tFind.nNumUnits && nCount < nMax; i++)
		{
			hUnit = tFind.phUnitsArray[i];
			if (hTarget != hUnit && SUnitIsHostile(ptGame, hOwner, hUnit))
			{
				for (j = 0; j < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); j++)
					((DWORD *)&tCreate)[j] = 0;
				tCreate.dwFlags = 32;
				tCreate.hSource = hOwner;
				tCreate.hSpawnLocation = hMissile;
				tCreate.nLevel = nLevel;
				tCreate.eMissile = 231;
				tCreate.nDX = (int)D2CommonUnitGetX(hUnit);
				tCreate.nDY = (int)D2CommonUnitGetY(hUnit);
				MissilesCreate(ptGame, &tCreate);
				nCount++;
			}
		}
		D2CommonUnitFindsFreeArray(&tFind);
		return 3;
	}
	return 3;
}

void __fastcall MissModeDamageGuidedArrow(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, DAMAGE *ptDamage)
{
	int nPercent;

	nPercent = D2CommonSkillsCalcParamThreeByLevel(SKILL_GUIDED_ARROW, D2CommonMissileGetLevel(hMissile));
	ptDamage->nPhysicalDamage = ptDamage->nPhysicalDamage * nPercent / 100 + ptDamage->nPhysicalDamage;
}

int __fastcall MissModeModeGuided(GAME *ptGame, UNIT *hMissile)
{
	UNIT *hOwner;
	void *ptPath;
	UNIT *hTarget;
	int nDistance;

	hOwner = SUnitGetOwner(ptGame, hMissile);
	if (!(!hOwner))
	{
		if (SUnitIsDead(hOwner))
			return 2;
		if (D2CommonDungeonRoomIsTown(D2CommonUnitGetPathRoom(hMissile)))
			return 2;
		if (D2CommonMissileGetScratchFirst(hMissile) & 1)
		{
			SUnitUpdateTarget(ptGame, hMissile);
			ptPath = hMissile->ptPath;
			hTarget = SUnitGetTarget(ptGame, hMissile);
			if (hTarget && !SUnitIsDead(hTarget) && SUnitIsHostile(ptGame, hOwner, hTarget)
				&& !((int)D2CommonMissileGetRangeLeft(hMissile) % 5))
			{
				nDistance = (int)D2CommonUnitGetProximityMetric(hMissile, hTarget);
				if (nDistance > 3 && nDistance < 25)
					D2CommonPathCompute(ptPath, hMissile);
			}
		}
		return MissModeModeDefault(ptGame, hMissile);
	}
	return 2;
}

int __fastcall MissModeHitSpiderGooLay(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	int nX;
	int nY;
	MISSILECREATE tCreate;
	int i;

	nX = (int)D2CommonUnitGetX(hMissile);
	nY = (int)D2CommonUnitGetY(hMissile);
	for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
		((DWORD *)&tCreate)[i] = 0;
	tCreate.dwFlags = 1;
	tCreate.hSource = SUnitGetOwner(ptGame, hMissile);
	tCreate.nX = nX;
	tCreate.nY = nY;
	tCreate.nLevel = (int)D2CommonMissileGetLevel(hMissile);
	tCreate.eMissile = 146;
	if (tCreate.hSource)
		MissilesCreate(ptGame, &tCreate);
	return 0;
}

void __fastcall MissModeSpiderGooEnd(UNIT *hUnit, int nState)
{
	D2CommonStatsUnitSetActiveStateBit(hUnit, nState, 0);
	D2CommonUnitUpdateAnimRate(hUnit);
}

int __fastcall MissModeHitSpiderGoo(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	int nFrame;
	void *hList;

	if (!hTarget)
		return 0;
	nFrame = ptGame->nFrame + 10;
	hList = (void *)D2CommonStatsFindListByState(hTarget, 24);
	if (!hList)
	{
		D2CommonStatsUnitSetActiveStateBit(hTarget, 24, 1);
		hList = (void *)D2CommonStatsAllocList(ptGame->pMemPool, 2, nFrame, hTarget->eType,
			hTarget->dwId);
		EventAdd(ptGame, hTarget, 12, nFrame);
		D2CommonStatsSetListState(hList, 24);
		D2CommonStatsSetListRemoveCallback(hList, MissModeSpiderGooEnd);
		D2CommonStatsUnitLinkList(hTarget, hList);
		D2CommonStatsListSetValueA(hList, 67, -100);
		D2CommonUnitUpdateAnimRate(hTarget);
	}
	else
	{
		D2CommonStatsSetListEndFrameSecond(hList, nFrame);
		EventAdd(ptGame, hTarget, 12, nFrame);
		D2CommonUnitUpdateAnimRate(hTarget);
	}
	return 0;
}

int __fastcall MissModeHitHowl(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	UNIT *hOwner;
	int nLevel;
	int nOwnerLevel;
	int nPower;
	DWORD dwDuration;

	if (hTarget)
	{
		hOwner = SUnitGetOwner(ptGame, hMissile);
		if (hOwner && hTarget->eType == UNIT_MONSTER && !D2CommonStatsGetState(hTarget, 56))
		{
			nLevel = (int)D2CommonMissileGetLevel(hMissile);

			nPower = (int)D2CommonSkillsGetTypeParamTwo(SKILL_HOWL) + (nLevel + D2CommonStatsUnitGetValue(hOwner, 12));
			if (nPower > D2CommonStatsUnitGetValue(hTarget, 12))
			{
				dwDuration = D2CommonSkillsCalcParamFiveByLevel(SKILL_HOWL, nLevel);
				MonsterScare(ptGame, hOwner, hTarget, D2CommonSkillsCalcParamThreeByLevel(SKILL_HOWL, nLevel),
					dwDuration);
			}
		}
	}
	return 0;
}

int __fastcall MissModeHitShout(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	UNIT *hOwner;

	hOwner = SUnitGetOwner(ptGame, hMissile);
	if (hTarget && hOwner && SUnitIsAlly(ptGame, hOwner, hTarget))
		SkillsShoutApply(ptGame, hTarget, hOwner, (int)D2CommonMissileGetLevel(hMissile));
	return 0;
}

int __fastcall MissModeHitFirehead(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	UNIT *hOwner;
	MISSILESTXT *ptTxt;
	int nHeal;
	int nLife;
	int nMaxLife;

	if (!hTarget)
		return 1;
	hOwner = SUnitGetOwner(ptGame, hMissile);
	if (!hOwner)
		return 1;
	ptTxt = (MISSILESTXT *)D2CommonMissileTblsGetMissile(hMissile->dwClassId);
	nHeal = MissModeRollStat(hMissile, 48, 49);
	nLife = D2CommonStatsUnitGetValue(hOwner, 6);
	nMaxLife = D2CommonStatsUnitCalcMaxHp(hOwner);
	if (nLife + nHeal > nMaxLife)
		D2CommonStatsUnitSetValue(hOwner, 6, nMaxLife);
	else
		D2CommonStatsUnitAddValue(hOwner, 6, nHeal);
	return (ptTxt->CollideKill != 0) + 2;
}

int __fastcall MissModeModeDiabWallMaker(GAME *ptGame, UNIT *hMissile)
{
	int nX;
	int nY;
	MISSILECREATE tCreate;
	int i;

	if (!SUnitGetOwner(ptGame, hMissile))
		MissModeCollide(ptGame, hMissile, NULL, FALSE);
	if (D2CommonStepTestSquareCrossed(hMissile->ptPath))
	{
		nX = (int)D2CommonUnitGetX(hMissile);
		nY = (int)D2CommonUnitGetY(hMissile);
		for (i = 0; i < (int)(sizeof(MISSILECREATE) / sizeof(DWORD)); i++)
			((DWORD *)&tCreate)[i] = 0;
		tCreate.dwFlags = 33;
		tCreate.hSource = SUnitGetOwner(ptGame, hMissile);
		tCreate.nX = nX;
		tCreate.nY = nY;
		tCreate.nLevel = D2CommonStatsUnitGetValue(hMissile, 12);
		tCreate.eMissile = 180;
		tCreate.nDX = (int)D2CommonUnitGetX(hMissile);
		tCreate.nDY = (int)D2CommonUnitGetY(hMissile);
		MissilesCreate(ptGame, &tCreate);
	}
	return MissModeModeDefault(ptGame, hMissile);
}

int __fastcall MissModeModeFingerMageSpider(GAME *ptGame, UNIT *hMissile)
{
	void *hPath;
	UNIT *hTarget;
	UNIT *hOwner;
	int nStepX;
	int nStepY;

	hPath = hMissile->ptPath;
#line 2299
	D2ASSERT(hPath);
	hTarget = SUnitGetTarget(ptGame, hMissile);
	hOwner = SUnitGetOwner(ptGame, hMissile);
	if (!hTarget)
	{
		if (hOwner)
			hTarget = SUnitGetTarget(ptGame, hOwner);
	}
	if (hTarget && !((int)D2CommonMissileGetRangeLeft(hMissile) % 5)
		&& (int)D2CommonUnitGetProximityMetric(hMissile, hTarget) < 20)
	{
		nStepX = 0;
		nStepY = 0;
		if (D2CommonUnitGetX(hTarget) > D2CommonUnitGetX(hMissile))
			nStepX = 2;
		else
		{
			if (D2CommonUnitGetX(hTarget) < D2CommonUnitGetX(hMissile))
				nStepX = -2;
		}
		if (D2CommonUnitGetY(hTarget) > D2CommonUnitGetY(hMissile))
			nStepY = 2;
		else
		{
			if (D2CommonUnitGetY(hTarget) < D2CommonUnitGetY(hMissile))
				nStepY = -2;
		}
		D2CommonPathSetTargetCoord(hPath, D2CommonUnitGetX(hMissile) + nStepX,
			D2CommonUnitGetY(hMissile) + nStepY);
		D2CommonPathCompute(hPath, hMissile);
	}
	return MissModeModeDefault(ptGame, hMissile);
}

int __fastcall MissModeHitFingerMageSpider(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	UNIT *hOwner;
	MISSSKILLSTXT *ptSkill;
	int nLevel;
	int nSlow;
	int nEnd;
	int nCurrent;
	void *hList;

	ptSkill = (MISSSKILLSTXT *)D2CommonDataTblsGetSkill(SKILL_FINGERMAGESPIDER);
	nLevel = (int)D2CommonMissileGetLevel(hMissile);
	if (!hTarget)
		return 3;
	hOwner = SUnitGetOwner(ptGame, hMissile);
	if (!hOwner)
		return 3;
	nSlow = ptSkill->Param[2] * nLevel;
	nEnd = ptSkill->Param[1] * (nLevel - 1) + ptSkill->Param[0] + ptGame->nFrame;
	hList = (void *)D2CommonStatsFindListByState(hTarget, 84);
	if (!hList)
	{
		hList = (void *)D2CommonStatsAllocList(ptGame->pMemPool, 2, nEnd, hOwner->eType,
			hOwner->dwId);
		D2CommonStatsListSetValueA(hList, 26, -nSlow);
		D2CommonStatsSetListState(hList, 84);
		D2CommonStatsSetListRemoveCallback(hList, SkillsStateRemove);
		D2CommonStatsUnitLinkList(hTarget, hList);
	}
	else
	{
		nCurrent = -(int)D2CommonStatsListGetValueA(hList, 26);
		if (nCurrent > nSlow)
			nSlow = nCurrent;
		D2CommonStatsSetListEndFrame(hList, nEnd);
		D2CommonStatsListSetValueB(hList, 26, -nSlow);
	}
	EventAdd(ptGame, hTarget, 12, nEnd);
	D2CommonStatsUnitSetActiveStateBit(hTarget, 84, 1);
	return 3;
}

int __fastcall MissModeModeCairnStones(GAME *ptGame, UNIT *hMissile)
{
	void *hRoomX;
	int nFrame;
	MISSILESTXT *ptTxt;
	UNIT *hOwner;
	DWORD dwY;
	DWORD dwX;

	nFrame = (int)D2CommonMissileGetRangeLeft(hMissile);
	ptTxt = (MISSILESTXT *)D2CommonMissileTblsGetMissile(MISSILE_CAIRNSTONES);
	if (nFrame < ptTxt->Range - ptTxt->Param1 && nFrame > ptTxt->Param1)
		MissModeCreateNearby(ptGame, hMissile, 17, ptTxt->Param2, MISSILE_CAIRNSTONESSKY, 5);
	if (!D2CommonMissileGetScratchFirst(hMissile) && nFrame <= ptTxt->Param1 + 100)
	{
		hOwner = SUnitGetOwner(ptGame, hMissile);
		D2CommonMissileSetScratchFirst(hMissile, 1);
		SkillsOpenPortal(ptGame, hOwner, (GAMEROOM *)D2CommonUnitGetPathRoom(hMissile),
			(int)D2CommonUnitGetX(hMissile), (int)D2CommonUnitGetY(hMissile), LEVEL_ACT_1_TRISTRAM, 0, 60, 1);
		hRoomX = (void *)D2CommonUnitGetPathRoom(hMissile);
		D2CommonDungeonRoomSetHold(hRoomX, 1);
	}
	return MissModeModeDefault(ptGame, hMissile);
}

int __fastcall MissModeHitCairnStones(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	void *hRoomX;
	UNIT *hOwner;
	DWORD dwY;
	DWORD dwX;

	if (!(hTarget)) {
		if (D2CommonMissileGetScratchFirst(hMissile))
			return 0;
		hOwner = SUnitGetOwner(ptGame, hMissile);
		D2CommonMissileSetScratchFirst(hMissile, 1);
		SkillsOpenPortal(ptGame, hOwner, (GAMEROOM *)D2CommonUnitGetPathRoom(hMissile), (int)D2CommonUnitGetX(hMissile), (int)D2CommonUnitGetY(hMissile),
			LEVEL_ACT_1_TRISTRAM, 0, 60, 1);
		hRoomX = (void *)D2CommonUnitGetPathRoom(hMissile);
		D2CommonDungeonRoomSetHold(hRoomX, 1);
		return 0; }
	return 0;
}

int __fastcall MissModeHitTowerChestSpawner(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	void *hRoom;

	if (!hTarget)
	{
		hRoom = (void *)D2CommonUnitGetPathRoom(hMissile);
#line 2441
		D2ASSERT(hRoom);
		D2CommonDungeonRoomSetHold(hRoom, 1);
	}
	return 0;
}

int __fastcall MissModeModeTowerChestSpawner(GAME *ptGame, UNIT *hMissile)
{
	int nFrame;
	MISSILESTXT *ptTxt;
	UNIT *hObject;
	OBJOPERATE tOperate;
	COLPOINT tFrom;
	COLPOINT tTo;
	GAMEROOM *hRoom;
	MISSLEVELDATA *ptLvlData;
	int nLevel;
	ITEMCREATE tCreate;
	DWORD dwId;
	int i;

	nFrame = (int)D2CommonMissileGetRangeLeft(hMissile);
	ptTxt = (MISSILESTXT *)D2CommonMissileTblsGetMissile(MISSILE_TOWERCHESTSPAWNER);
	if (nFrame == 1)
	{
		hObject = SUnitFind(ptGame, UNIT_OBJECT, D2CommonMissileGetScratchFirst(hMissile));
		if (hObject)
			SUnitSetEvent(hObject, 73, NULL);
	}
	if (nFrame == ptTxt->Range - ptTxt->Param1)
	{
		dwId = D2CommonMissileGetScratchFirst(hMissile);
		tOperate.ptGame = ptGame;
		tOperate.hPlayer = NULL;
		hObject = SUnitFind(ptGame, UNIT_OBJECT, dwId);
		if (hObject)
		{
			tOperate.hObject = hObject;
			tOperate.ptSeed = &hObject->tSeed;
			tOperate.dwClassId = hObject->dwClassId;
			ObjectModeOperateTowerChest(&tOperate);
		}
		D2CommonMissileSetScratchSecond(hMissile, 1);
	}
	if (D2CommonMissileGetScratchSecond(hMissile) && !(nFrame % (ptTxt->Param2 * 4)))
	{
		D2CommonUnitGetPosition(hMissile, &tFrom);
		tFrom.nX = tFrom.nX + (int)D2CommonSeedRand(&hMissile->tSeed, 10) - 5;
		tFrom.nY = tFrom.nY + (int)D2CommonSeedRand(&hMissile->tSeed, 10) - 5;
		hRoom = ITEMSFindDropSpot((GAMEROOM *)D2CommonUnitGetPathRoom(hMissile), &tFrom, &tTo, 1);
		if (hRoom)
		{
			ptLvlData = (MISSLEVELDATA *)D2CommonLvlTblsGetLevelData(D2CommonDungeonRoomGetLevelId(hRoom));
#line 2498
			D2ASSERT(ptLvlData);
#line 2499
			D2ASSERT(ptGame->bDifficultyLevel < NUM_DIFFICULTY_LEVELS);
			nLevel = ptLvlData->MonLvl[ptGame->bDifficultyLevel];
			for (i = 0; i < (int)(sizeof(ITEMCREATE) / sizeof(DWORD)); i++)
				((DWORD *)&tCreate)[i] = 0;
			tCreate.hSpawner = hMissile;
			tCreate.ptSeed = NULL;
			tCreate.ptGame = ptGame;
			tCreate.dwClassId = D2CommonItemTblsFindItemIndexByCode(543452263);
			tCreate.nX = tTo.nX;
			tCreate.nY = tTo.nY;
			tCreate.dwMode = 3;
			tCreate.hRoom = hRoom;
			tCreate.nLevel = nLevel;
			tCreate.wCreateFlags = 1;
			tCreate.dwQuality = 2;
			ITEMSCreateFromRecord(ptGame, &tCreate, 0);
		}
	}
	return MissModeModeDefault(ptGame, hMissile);
}

int __fastcall MissModeModeRadamentDeath(GAME *ptGame, UNIT *hMissile)
{
	int nFrame;
	PFNRADAMENTCALLBACK pfnCallback;
	DWORD dwY;

	nFrame = (int)D2CommonMissileGetRangeLeft(hMissile);
	if (nFrame > 1 && nFrame < 25)
		SkillsAreaEffectAt(ptGame, 12290, (int)D2CommonUnitGetX(hMissile), (int)D2CommonUnitGetY(hMissile), hMissile, SKILL_REDEMPTION,
			1, (PFNAREAAFFECT)MissModeRadamentDeathFirst);
	else if (nFrame == 1)
		SkillsAreaEffectAt(ptGame, 12290, (int)D2CommonUnitGetX(hMissile), (int)D2CommonUnitGetY(hMissile), hMissile, SKILL_REDEMPTION,
			1, (PFNAREAAFFECT)MissModeRadamentDeathLast);
	return MissModeModeDefault(ptGame, hMissile);
}

void __fastcall MissModeRadamentDeathFirst(GAME *ptGame, UNIT *hMissile, UNIT *hUnit, DWORD dwArg,
	DWORD dwUnused)
{
	SkillsRedemptionAffect(ptGame, hMissile, hUnit, (int)dwArg, 0);
}

void __fastcall MissModeRadamentDeathLast(GAME *ptGame, UNIT *hMissile, UNIT *hUnit, DWORD dwArg,
	DWORD dwUnused)
{
	SkillsRedemptionAffect(ptGame, hMissile, hUnit, (int)dwArg, 1);
}

int __fastcall MissModeHitHellMeteorDown(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	DAMAGE tDamage;
	int nParam;
	int nY;
	int nX;
	int i;

	if (!(hTarget)) {
		for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
			((DWORD *)&tDamage)[i] = 0;
		tDamage.nFireDamage = MissModeRollStat(hMissile, 48, 49);
		nParam = ((MISSILESTXT *)D2CommonMissileTblsGetMissile(MISSILE_HELLMETEORDOWN))->Param1;

		if (SUnitDmgSplash(ptGame, hMissile, ((int)D2CommonUnitGetX(hMissile)), ((int)D2CommonUnitGetY(hMissile)), nParam, &tDamage, FALSE, FALSE, NULL, 0))
			return 1;
		else
			return 3;
	}
	return 0;
}

int __fastcall MissModeHitOrbMist(GAME *ptGame, UNIT *hMissile, UNIT *hTarget)
{
	void *hRoomX;
	UNIT *hStairs;

	if (!(hTarget))
	{
		hStairs = SUnitFind(ptGame, UNIT_OBJECT, D2CommonMissileGetScratchFirst(hMissile));
#line 2630
		D2ASSERT(hStairs);
		hRoomX = (void *)D2CommonUnitGetPathRoom(hStairs);
		D2CommonDungeonRoomSetHold(hRoomX, 1);
		if (!hStairs->dwMode)
		{
			D2CommonUnitSetMode(hStairs, 1);
			EventAdd(ptGame, hStairs, 1,
				((DWORD)((MISSOBJECTSTXT *)D2CommonObjTblsGetObject(386))->FrameCnt[1] >> 8) + ptGame->nFrame); }
		return 1; }
	return 0;
}

void __fastcall MissModeHitUnit(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, PFNMISSILEDAMAGE pfnDamage)
{
	DAMAGE tDamage;
	void *hSkill;
	UNIT *hOwner;
	int nChance;
	int i;

#line 3911
	D2ASSERT(hTarget);
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	tDamage.nPhysicalDamage = MissModeRollStat(hMissile, 21, 22);
	tDamage.nFireDamage = MissModeRollStat(hMissile, 48, 49);
	tDamage.nMagicDamage = MissModeRollStat(hMissile, 52, 53);
	tDamage.nLightningDamage = MissModeRollStat(hMissile, 50, 51);
	tDamage.nColdDamage = MissModeRollStat(hMissile, 54, 55);
	tDamage.nPoisonDamage = MissModeRollStat(hMissile, 57, 58);
	tDamage.nPoisonLength = D2CommonStatsUnitGetValue(hMissile, 59);
	tDamage.nColdLength = D2CommonStatsUnitGetValue(hMissile, 56);
	tDamage.nManaDrain = D2CommonStatsUnitGetValue(hMissile, 62);
	tDamage.nLifeDrain = D2CommonStatsUnitGetValue(hMissile, 60);
	hSkill = (void *)D2CommonMissileGetIndexedByteRaw((DWORD)hMissile, 0);
	if (hSkill)
	{
		hOwner = SUnitGetOwner(ptGame, hMissile);
		if (hOwner)
		{
			nChance = D2CommonSkillsCalcCurveParamOne(hSkill, SKILL_CRITICAL_STRIKE);
			if ((int)D2CommonSeedRand(&hOwner->tSeed, 100) < nChance)
			{
				tDamage.nPhysicalDamage = tDamage.nPhysicalDamage + tDamage.nPhysicalDamage;
				if (!SUnitDmgSetFlag(&tDamage, 16))
					D2CommonUnitAddAnimOverlay(hTarget, 54, 0);
			}
		}
	}
	if (pfnDamage)
		pfnDamage(ptGame, hMissile, hTarget, &tDamage);
	MissModeApplyDamage(ptGame, hMissile, hTarget, &tDamage);
}

int __fastcall MissModeModeDefault(GAME *ptGame, UNIT *hMissile)
{
	int nRange;
	int eMode;
	void *ptPath;
	DWORD dwSize;
	WORD wCollide;
	int nSteps;
	PATHPOINT *ptSteps;
	DWORD hRoom;
	const MISSILEMODECHART *ptChart;
	UNIT *hHit;
	int nActivate;
	int i;

	if (D2CommonPathGetVelocity(hMissile->ptPath) && SUnitStepAnim(ptGame, hMissile) == 2)
		return MissModeCollide(ptGame, hMissile, NULL, TRUE);
	nRange = (int)D2CommonMissileGetRangeLeft(hMissile) - 1;
	D2CommonMissileSetRangeLeft(hMissile, nRange);
	if (nRange <= 0)
		return MissModeCollide(ptGame, hMissile, NULL, TRUE);
	D2CommonUnitGetPathRoom(hMissile);
	eMode = hMissile->dwMode;
	if (!eMode)
		return 1;
	ptPath = hMissile->ptPath;
	dwSize = D2CommonUnitGetSizeX(hMissile);
	wCollide = D2CommonPathGetLineOfSight(ptPath);
	if (wCollide & 5)
		return 2;
	if (eMode == 6)
		return 1;

	if ((int)D2CommonMissileGetRangeLeft(hMissile) > ((int)D2CommonMissileGetActivateAt(hMissile)))
		return 1;
	if (!wCollide)
		return 1;
	nSteps = (int)D2CommonPathGetSavedSteps(ptPath, &ptSteps);
	hRoom = D2CommonUnitGetPathRoom(hMissile);
	ptChart = &gptMissileModeChart[eMode];
#line 3997
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)gptMissileModeChart[eMode].pfnUnitTest), "gptMissileModeChart[eMode].pfnUnitTest");
	for (i = 0; i < nSteps; i++)
	{
		wCollide = D2CommonCollisnTestSize(hRoom, ptSteps[i].wX, ptSteps[i].wY, dwSize,
			gptMissileModeChart[eMode].dwPattern);
		if (wCollide)
		{
			hHit = (UNIT *)D2CommonUnitUtilFindUnitAtPoint(hRoom, ptSteps[i].wX, ptSteps[i].wY,
				ptChart->pfnUnitTest, D2CommonUnitGetSizeX(hMissile));
			if (hHit)
				return MissModeCollide(ptGame, hMissile, hHit, FALSE);
			if (wCollide & 4)
			{
				MissModeCollide(ptGame, hMissile, NULL, TRUE);
				return 2;
			}
		}
	}
	return 1;
}

static __inline int MissModeToHitI(UNIT *hMissile)
{
	int nToHit = 0;
	if (hMissile)
		nToHit = D2CommonStatsUnitGetValue(hMissile, 19);
	return nToHit;
}

extern BOOL MissModeCollideKind(DWORD eMode, UNIT *hUnit);
__inline BOOL MissModeCollideKind(DWORD eMode, UNIT *hUnit)
{
	DWORD eType = hUnit->eType;

	switch (eMode)
	{
	case 0:
		return FALSE;
	case 1:
		if (eType != UNIT_PLAYER)
			return FALSE;
		break;
	case 2:
		if (eType != UNIT_MONSTER)
			return FALSE;
		break;
	case 3:
		return TRUE;
	}
	return TRUE;
}

int __fastcall MissModeCollide(GAME *ptGame, UNIT *hMissile, UNIT *hTarget, BOOL fExplode)
{
	int eClass;
	MISSILESTXT *ptTxt;
	UNIT *hOwner;
	int nResult;
	int nHit;
	int nPierce;
	int nFrame;
	void *hList;
	DWORD hRoom;
	DWORD dwSize;
	DWORD dwY;

	eClass = hMissile->dwClassId;
	ptTxt = (MISSILESTXT *)D2CommonMissileTblsGetMissile(eClass);
	hOwner = SUnitGetOwner(ptGame, hMissile);
	nResult = 2;
	nHit = 3;
	if (ptTxt->Explosion)
		nResult = 0;
	if (hTarget)
	{
		if (ptTxt->NextHit && D2CommonStatsGetState(hTarget, 86))
			goto lOne;
		if (D2CommonMissileTestLastCollideUnit(hMissile, hTarget))
			goto lOne;
		D2CommonMissileSetLastCollideUnit(hMissile, hTarget);
		if (!MissModeCollideKind(hMissile->dwMode, hTarget))
			goto lOne;
		if (hOwner && !SUnitIsHostile(ptGame, hOwner, hTarget) && !ptTxt->CollideFriend)
			goto lOne;
	}
	if (hOwner && hTarget
		&& (D2CommonStatsGetState(hOwner, 69) || D2CommonStatsGetTotalValue(hOwner, 156)))
	{
		int nNewHit = 3;
		if (!((MISSILESTXT *)D2CommonMissileTblsGetMissile(D2CommonMissileGetClassId(hMissile)))->Pierce)
			nHit = nNewHit;
		else
		{
			nPierce = (int)D2CommonMissileGetIndexedByteRaw((DWORD)hMissile, 1);
			if (nPierce)
			{
				nNewHit = 2;
				if (D2CommonMissileTestLastCollideUnit(hMissile, hTarget))
					nNewHit = 0;
				D2CommonMissileSetIndexedByte(hMissile, 1, nPierce - 1);
			}
			nHit = nNewHit;
		}
	}
	if (ptTxt->CollideKill)
		nResult |= 1;
	if (hTarget)
	{
		if (ptTxt->ToHit && !SUnitDmgRollHit(hOwner, hTarget, MissModeToHitI(hMissile), 1))
		{
			SkillsChillingArmorHit(ptGame, hMissile, hTarget);
			return 2;
		}
	}
	else if (!fExplode && !ptTxt->AlwaysExplode)
		goto Done;
	if (nHit & 2)
	{
		if (ptTxt->NextHit)
		{
			if (!hTarget)
				goto Hit;
			nFrame = ptTxt->NextDelay + ptGame->nFrame;
			hList = (void *)D2CommonStatsAllocList(ptGame->pMemPool, 2, nFrame, hTarget->eType,
				hTarget->dwId);
			if (hList)
			{
				EventAdd(ptGame, hTarget, 12, nFrame);
				D2CommonStatsSetListState(hList, 86);
				D2CommonStatsSetListRemoveCallback(hList, PlrMsgStateEnd);
				D2CommonStatsUnitLinkList(hTarget, hList);
				D2CommonStatsUnitSetActiveStateBit(hTarget, 86, 1);
			}
		}
		if (hTarget && D2CommonStatsGetState(hTarget, 20))
			SkillsChillingArmorHit(ptGame, hMissile, hTarget);
Hit:
		if (sgptMissileChart[eClass].pfnHit)
		{
#line 4224
			D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgptMissileChart[eClass].pfnHit), "sgptMissileChart[eClass].pfnHit");
			nResult = sgptMissileChart[eClass].pfnHit(ptGame, hMissile, hTarget);
			if (nResult & 4)
				goto lOne;
		}
	}
	if (hTarget && (nResult & 2))
		MissModeHitUnit(ptGame, hMissile, hTarget, sgptMissileChart[eClass].pfnDamage);
Done:
	if (!(nResult & 1) && !fExplode)
		goto lOne;
	if (nHit & 1)
	{
		hRoom = D2CommonUnitGetPathRoom(hMissile);
		if (hRoom)
			D2CommonCollisnClearSize(hRoom, D2CommonUnitGetX(hMissile), D2CommonUnitGetY(hMissile),
				D2CommonUnitGetSizeX(hMissile), 64);
		return 2;
	}
lOne:
	return 1;

}

void __fastcall MissModeRun(GAME *ptGame, UNIT *hMissile, int eEventType)
{
	int eClass;
	DWORD hRoom;
	DWORD dwY;

	eClass = hMissile->dwClassId;
	if (!sgptMissileChart[eClass].pfnMode)
		return;
	hRoom = D2CommonUnitGetPathRoom(hMissile);
	dwY = D2CommonUnitGetY(hMissile);
	if (D2CommonDungeonRoomIsTown(D2CommonDungeonFindRoomGame(hRoom, D2CommonUnitGetX(hMissile), dwY)))
	{
		SUnitFree(ptGame, hMissile);
		return;
	}
#line 4274
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgptMissileChart[eClass].pfnMode), "sgptMissileChart[eClass].pfnMode");
	if (sgptMissileChart[eClass].pfnMode(ptGame, hMissile) == 2)
		SUnitFree(ptGame, hMissile);
}
