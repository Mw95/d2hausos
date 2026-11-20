#ifndef D2RE_D2GAME_AITHINK_H
#define D2RE_D2GAME_AITHINK_H

#include "../D2game.h"

union AIFINDRECORD;

void __fastcall AiThinkNone(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkCircleTarget(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkAttackOnce(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkTheSmith(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkDoomKnight(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkAbyssKnight(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkOblivionKnight(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
UNIT * __fastcall AiThinkFindKnight(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, union AIFINDRECORD *ptFind);
void __fastcall AiThinkMoonClan(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkBrute(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkThornedHulk(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkCarrionBird(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkDuneBeast(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
BOOL __fastcall AiThinkEmerge(GAME *ptGame, UNIT *hUnit);
UNIT * __fastcall AiThinkFindHurtUnit(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, union AIFINDRECORD *ptFind);
void __fastcall AiThinkFollowOwner(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
BOOL __fastcall AiThinkMoveToOwner(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, UNIT *hOwner,
	DWORD dwInReach, AIPARMS *ptParms);
void __fastcall AiThinkArach(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSwampDwellerOut(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSwampDweller(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkGhoulLord(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkZakarumite(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSexton(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
UNIT * __fastcall AiThinkFindHurtZakarum(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate,
	union AIFINDRECORD *ptFind);
void __fastcall AiThinkCouncilMember(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
UNIT * __fastcall AiThinkFindWeakestFellow(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate,
	union AIFINDRECORD *ptFind);
void __fastcall AiThinkRatManHitAndBackOff(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkRatManBackOffInSight(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkRatManShaman(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
UNIT * __fastcall AiThinkCountFellows(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, union AIFINDRECORD *ptFind);
void __fastcall AiThinkWaterWatcherLimb(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkWaterWatcherHead(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkPatrolInit(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkPatrol(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkPatrolOnMinion(UNIT *hMinion, BOOL *pbFlag, DWORD dwUnused);
void __fastcall AiThinkGriswold(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSkeleton(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSkeletonArcher(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkGhost(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkFlyingScimitar(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkHuntress(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
UNIT * __fastcall AiThinkKinFindFn(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate, union AIFINDRECORD *ptFind);
void __fastcall AiThinkSlinger(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSandMaggotOut(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSandMaggot(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSandMaggotQueen(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkTombViper(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkBalrog(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkBlunderbore(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkDungSoldier(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSandMaggotYoung(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSandMaggotEgg(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkAndariel(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSummoner(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkGloam(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
BOOL __fastcall AiThinkTestRingUnit(UNIT *hUnit, DWORD dwArgs);
void __fastcall AiThinkFoulCrow(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkAfflicted(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkDarkRanger(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkDarkHunter(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkGuardPlace(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkDarkSpearwoman(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkFallen(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkHollowOne(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
UNIT * __fastcall AiThinkFindUndeadFellow(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate,
	union AIFINDRECORD *ptFind);
void __fastcall AiThinkSandRaider(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
UNIT * __fastcall AiThinkFindNearFellow(GAME *ptGame, UNIT *hUnit, UNIT *hCandidate,
	union AIFINDRECORD *ptFind);
void __fastcall AiThinkQuillRat(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkQuillBear(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkZombie(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkFallenShaman(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkGargoyleTrap(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkFireTower(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkLightningSpire(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkAttackThenDie(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkDriedCorpse(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSandLeaper(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkItchies(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkFoulCrowNestInit(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkFoulCrowNest(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkDuriel(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkBloodravenInit(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkBloodraven(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkMummyGenerator(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSuckerNest(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkDesertWingOut(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkDesertWing(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkReturnedMage(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
typedef BYTE (__fastcall *PFNAINPCNODE)(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms,
	PRESETPATHNODE *ptNode);
BYTE __fastcall AiThinkNpcOnHoldPoint(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms, PRESETPATHNODE *ptNode);
BYTE __fastcall AiThinkNpcOnWalkPoint(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms, PRESETPATHNODE *ptNode);
BYTE __fastcall AiThinkNpcOnFirstSkillPoint(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms,
	PRESETPATHNODE *ptNode);
BYTE __fastcall AiThinkNpcOnSecondSkillPoint(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms,
	PRESETPATHNODE *ptNode);
void __fastcall AiThinkNpc(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
BYTE __fastcall AiThinkNpcPlayer(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
BYTE __fastcall AiThinkNpcCommand(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
BYTE __fastcall AiThinkNpcPath(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkTyrael(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkNpcCommandOrPath(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkHellMeteor(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkKaelan(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkDeckardCain(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
BOOL __fastcall AiThinkWalkToQuestSpotStart(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkResetThinkOut(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkHireling(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkHirelingFight(GAME *ptGame, UNIT *hUnit, int nClass, UNIT *hOwner,
	UNIT *hTarget, D2SEED *ptSeed, AIPARMS *ptParms);
void __fastcall AiThinkHirelingSkill(GAME *ptGame, UNIT *hUnit, int nClass,
	DWORD dwPetSeed, UNIT *hTarget, D2SEED *ptSeed);
void __fastcall AiThinkRogueScout(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkFightOutOfTown(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkAttackInSight(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkNavi(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkWait(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkGroupMember(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkAttackOrStepAside(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkFleeInit(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkFlee(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkChargeUnit(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSpawnBatchInit(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSpawnBatch(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkGroper(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkCorpulent(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
BOOL __fastcall AiThinkTestSoft(UNIT *hUnit, DWORD dwArgs);
void __fastcall AiThinkMephisto(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkDiabloOut(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkDiablo(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
int __fastcall AiThinkCalcFoeScore(UNIT *hUnit, UNIT *hOther, AICOMMAND *ptCommand);
int __fastcall AiThinkRollCommand(AIGENERAL *ptAi, UNIT *hUnit, UNIT *hBest,
	int nBest, int nCount, AICOMMAND *ptCommand);

void __fastcall AiThinkStandUntilExpiredInit(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkStandUntilExpired(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkStandOrSkillMode(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSucker(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkHydra(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkTrapFireBolt(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkTrapPoison(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkTrapNova(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkTrapAlignedX(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkTrapAlignedY(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkPillar(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkTrappedSoul(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkIzual(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSpawnWhenNearInit(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkSpawnWhenNear(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkAttackOnceThenDie(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkYoungdiablo(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkFleshSpawner(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkFleshBeast(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
int __fastcall AiGetKind(AIRECORDHEAD *ptRecord);
void __fastcall AiSetParams(AIGENERAL *ptAi, int nParam0, int nParam1,
	int nParam2);
BOOL __fastcall AiThinkKindApplies(UNIT *hUnit, MONSTATSTXT *ptMonStats, int nKind,
	BOOL bUnique);
void __fastcall AiThinkSet(GAME *ptGame, UNIT *hUnit, AIGENERAL *ptAi, int nKind);
void __fastcall AiThinkDefault(GAME *ptGame, UNIT *hUnit, AIPARMS *ptParms);
void __fastcall AiThinkProcess(GAME *ptGame, UNIT *hUnit);

#endif
