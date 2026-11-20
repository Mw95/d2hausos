#ifndef D2RE_D2GAME_OBJMODE_H
#define D2RE_D2GAME_OBJMODE_H

#include "../D2game.h"

#define NUM_OPERATE_FUNCTIONS	60

typedef void (__fastcall *PFNOBJEVENT)(GAME *ptGame, UNIT *hObject);
typedef int (__fastcall *PFNOBJECTOPERATE)(OBJOPERATE *pObjectParam);
typedef void (__fastcall *PFNOBJTRAP)(GAME *ptGame, UNIT *hObject);
typedef void (__fastcall *PFNSHRINE)(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData);

void __fastcall ObjectModeEventEndOperate(GAME *ptGame, UNIT *hObject);
void __fastcall ObjectModeEventShrineReset(GAME *ptGame, UNIT *hObject);
void __fastcall ObjectModeEventRegenerate(GAME *ptGame, UNIT *hObject);
void __fastcall ObjectModeEventProximity(GAME *ptGame, UNIT *hObject);
void __fastcall ObjectModeEventTrap(GAME *ptGame, UNIT *hObject);
void __fastcall ObjectModeEventCycle(GAME *ptGame, UNIT *hObject);
void __fastcall ObjectModeEventDrop(GAME *ptGame, UNIT *hObject);
void __fastcall ObjectModeEventDelayedDrop(GAME *ptGame, UNIT *hObject);
void __fastcall ObjectModeEvent(GAME *ptGame, UNIT *hUnit, int eEventType);
void __fastcall ObjectModeSendState(UNIT *hObject, CLIENT *ptClient);
void __fastcall ObjectModeSendToClient(GAME *ptGame, UNIT *hObject, CLIENT *ptClient);

int __fastcall ObjectModeOperateTeleportPad(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateCow(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateDoor(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateTrapDoor(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateStairs(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateStairsR(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateSlimeDoor(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateChest(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateObelisk(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateShrine(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateCasket(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateWirt(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateUrn(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateCrate(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateDelayedDrop(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateSecretDoor(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateBookshelf(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateArmorStand(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateWeaponRack(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateBarrel(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateExplodingBarrel(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateTorch(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateBrazier(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateSanctuaryPortal(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateHellgate(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateGuildStone(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateGuildVault(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateTrophyCase(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateMessageBoard(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateTownPortal(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateLevelPortal(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateWaypoint(OBJOPERATE *pObjectParam);
int __fastcall ObjectModeOperateWell(OBJOPERATE *ptOperate);
int __fastcall ObjectModeOperateTreasureChest(OBJOPERATE *ptOperate);
void __fastcall ObjectModeOperateTowerChest(OBJOPERATE *pObjectParam);

void __fastcall ObjectModeTrapRegionMonster(GAME *ptGame, UNIT *hObject);
void __fastcall ObjectModeTrapFire(GAME *ptGame, UNIT *hObject);
void __fastcall ObjectModeTrapChainLightning(GAME *ptGame, UNIT *hObject);
void __fastcall ObjectModeTrapFireBolt(GAME *ptGame, UNIT *hObject);
void __fastcall ObjectModeTrapPoison(GAME *ptGame, UNIT *hObject);
void __fastcall ObjectModeTrapNova(GAME *ptGame, UNIT *hObject);
void __fastcall ObjectModeTrapSetOff(OBJOPERATE *pObjectParam, BYTE bTrapType);

void __fastcall ObjectModeShrineRefillLife(OBJOPERATE *pObjectParam,
	SHRINESTXT *ptShrineData);
void __fastcall ObjectModeShrineRefillMana(OBJOPERATE *pObjectParam,
	SHRINESTXT *ptShrineData);
void __fastcall ObjectModeShrineRefill(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData);
void __fastcall ObjectModeShrineLifeToMana(OBJOPERATE *pObjectParam,
	SHRINESTXT *ptShrineData);
void __fastcall ObjectModeShrineManaToLife(OBJOPERATE *pObjectParam,
	SHRINESTXT *ptShrineData);
void __fastcall ObjectModeShrineReverse(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData);
void __fastcall ObjectModeShrinePortal(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData);
void __fastcall ObjectModeShrineGem(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData);
BOOL __fastcall ObjectModeCreateGem(GAME *ptGame, UNIT *hPlayer, int nItemIndex, UNIT *hOldItem);
void __fastcall ObjectModeShrineExploding(OBJOPERATE *pObjectParam,
	SHRINESTXT *ptShrineData);
BOOL __fastcall ObjectModeShrineExplodingTest(UNIT *hUnit);
void __fastcall ObjectModeShrineMonster(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData);
BOOL __fastcall ObjectModeShrineMonsterTest(UNIT *hUnit, UNIT *hPlayer);
void __fastcall ObjectModeShrineItemsOpm(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData);
void __fastcall ObjectModeShrineItemsGpm(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData);
void __fastcall ObjectModeShrineCombat(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData);
void __fastcall ObjectModeShrineStatDouble(OBJOPERATE *pObjectParam,
	SHRINESTXT *ptShrineData);
void __fastcall ObjectModeShrineStatDoubleEnd(UNIT *hUnit, DWORD dwState);
void __fastcall ObjectModeShrineStat(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData);
void __fastcall ObjectModeShrineSkill(OBJOPERATE *pObjectParam, SHRINESTXT *ptShrineData);
void __fastcall ObjectModeShrineSkillEnd(UNIT *hUnit, DWORD dwState);

int __fastcall ObjectModeOperate(GAME *ptGame, UNIT *hPlayer, WORD UnitType, DWORD dwUnitId);
void __fastcall ObjectModeOperateInRange(GAME *ptGame, UNIT *hUnit, DWORD dwUnitType, DWORD dwUnitId);
void __fastcall ObjectModeDoorUpdate(GAME *ptGame, UNIT *hUnit, DWORD dwObjectId);
void __fastcall ObjectModeGuildStoneRelease(GAME *ptGame, UNIT *hUnit, UNIT *hObject);
void __fastcall ObjectModeWaypointTravel(GAME *ptGame, UNIT *hPlayer, DWORD dwObjectId, WORD wLevelId);

BOOL __fastcall ObjectModePowerupMaxMana(GAME *ptGame, UNIT *hUnit, int nValue);
BOOL __fastcall ObjectModePowerupEnergy(GAME *ptGame, UNIT *hUnit, int nValue);
BOOL __fastcall ObjectModePowerupDexterity(GAME *ptGame, UNIT *hUnit, int nValue);
BOOL __fastcall ObjectModePowerupVitality(GAME *ptGame, UNIT *hPlayer, int nValue);
BOOL __fastcall ObjectModePowerupStrength(GAME *ptGame, UNIT *hPlayer, int nValue);
BOOL __fastcall ObjectModePowerupLife(GAME *ptGame, UNIT *hPlayer, int nValue);
BOOL __fastcall ObjectModePowerupNewSkills(GAME *ptGame, UNIT *hPlayer, int nValue);

UNIT *__fastcall ObjectModeReleasePlayer(GAME *ptGame, UNIT *hPlayer, UNIT *hObject);

UNIT *__fastcall ObjectDropOne(OBJOPERATE *pObjectParam, int nMode);

void __fastcall ObjectModeReset(GAME *ptGame, UNIT *hObject);

#endif
