#ifndef D2RE_D2GAME_SCMD_H
#define D2RE_D2GAME_SCMD_H

#include "../D2game.h"

#pragma pack(push, 1)
struct SCMDENVPACKET
{
 	BYTE		bCmd;
 	DWORD		dwCycle;
 	DWORD		dwAngle;
 	BYTE		bAlt;
};

struct SCMDCHATDATA
{
 	BYTE		bCmd;

 	BYTE		bKind;
 	BYTE		field_02;
 	BYTE		bType;
 	DWORD		dwId;
 	BYTE		bColor;
 	BYTE		bLevel;
 	char		szName[16];
 	BYTE		bCustomText[256];
};

struct SCMDSYSTEMMSG
{
 	BYTE		bCmd;
 	BYTE		bEvent;
 	BYTE		bColor;
 	DWORD		dwArg;
 	BYTE		unobserved_07;
 	BYTE		bPlayerName[16];
 	BYTE		unobserved_18[40 - 24];
};
#pragma pack(pop)

DWORD __fastcall SCmdSendSysGameEnd(DWORD dwClientId);
DWORD __fastcall SCmdSendPumpEndNow(DWORD dwClientId);
DWORD __fastcall SCmdSendJoinError(DWORD dwClientId, DWORD dwError);
void __fastcall SCmdSendCmd(CLIENT *ptClient, BYTE bCmd);
int __fastcall SCmdQueue(CLIENT *ptClient, const void *pData, int nSize);
void __fastcall SCmdSendGameFlags(CLIENT *ptClient, BYTE bCmd, GAME *ptGame);
void __fastcall SCmdSendLoadAct(CLIENT *ptClient, BYTE bCmd, BYTE bAct,
	DWORD dwSeed, DWORD dwObjectSeed, WORD wLevelId);
void __fastcall SCmdSendUnitTypeId(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId);
void __fastcall SCmdSendDword(CLIENT *ptClient, BYTE bCmd, DWORD dwValue);
void __fastcall SCmdSendRoom(CLIENT *ptClient, DWORD bLevelId, WORD wX,
	WORD wY);
void __fastcall SCmdSendRoomLeave(CLIENT *ptClient, DWORD bLevelId, WORD wX,
	WORD wY);
void __fastcall SCmdSendUnit(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, WORD wX, WORD wY,
	BYTE bField0a);
void __fastcall SCmdSendUnitRemove(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId);
void __fastcall SCmdSendStat(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, BYTE bField06, BYTE bStat,
	DWORD dwValue);
void __fastcall SCmdSendEnv(CLIENT *ptClient, SCMDENVPACKET *ptPacket);

int __fastcall SCmdSendTargetPoint(CLIENT *ptClient, BYTE bSourceType,
	DWORD dwSourceId,
	WORD wX, WORD wY, WORD wSkillId,
	WORD wField0d, BYTE bSkillLevel);
void __fastcall SCmdSendUnitAt(CLIENT *ptClient, BYTE bSourceType,
	DWORD dwSourceId, BYTE bType, DWORD dwUnitId,
	WORD wSkillId, WORD wField0e, BYTE bSkillLevel);
void __fastcall SCmdSendMonsterMove(CLIENT *ptClient, BYTE bCmd, DWORD dwUnitId,
	BYTE bMode, BYTE bTargetType, DWORD dwTargetId,
	BYTE bStopDist);
void __fastcall SCmdSendMonsterMoveTo(CLIENT *ptClient, BYTE bCmd, DWORD dwUnitId,
	BYTE bMode, WORD wX, WORD wY,
	BYTE bStopDist);
void __fastcall SCmdSendMonsterKnockback(CLIENT *ptClient, BYTE bCmd, DWORD dwUnitId,
	BYTE bMode, BYTE bTargetType, DWORD dwTargetId,
	BYTE bDistLeft, BYTE bLife, BYTE bHitClass);
void __fastcall SCmdSendMonsterKnockbackTo(CLIENT *ptClient, BYTE bCmd, DWORD dwUnitId,
	BYTE bMode, WORD wX, WORD wY,
	BYTE bDistLeft, BYTE bLife, BYTE bHitClass);
void __fastcall SCmdSendMonsterInPlace(CLIENT *ptClient, BYTE bCmd, DWORD dwUnitId,
	BYTE bMode, BYTE bTargetType, DWORD dwTargetId,
	BYTE bField0b);
void __fastcall SCmdSendMonsterInPlaceTo(CLIENT *ptClient, BYTE bCmd, DWORD dwUnitId,
	BYTE bMode, WORD wX, WORD wY,
	BYTE bField0a, BYTE bHitClass);
void __fastcall SCmdSendMonsterAttack(CLIENT *ptClient, BYTE bCmd, DWORD dwUnitId,
	BYTE bMode, BYTE bTargetType, DWORD dwTargetId,
	BYTE bField0b, WORD wX, WORD wY);
void __fastcall SCmdSendMonsterAttackTo(CLIENT *ptClient, BYTE bCmd, DWORD dwUnitId,
	BYTE bMode, WORD wTargetX, WORD wTargetY,
	BYTE bField0a, BYTE bHitClass, WORD wX,
	WORD wY);

void __fastcall SCmdSendPlayer(CLIENT *ptClient, UNIT *hUnit, CLIENT *ptOwner);
void __fastcall SCmdSendRosterRemove(CLIENT *ptClient, DWORD dwValue);
int __fastcall SCmdSendArenaTag(CLIENT *ptClient, DWORD dwUnitId, WORD wTag);
void __fastcall SCmdSendPair(CLIENT *ptClient, DWORD bKey, DWORD bSkillId,
	DWORD bHand);
void __fastcall SCmdSendMonRegionSeed(CLIENT *ptClient, DWORD dwValue);
void __fastcall SCmdSendPing(CLIENT *ptClient, DWORD dwTick);
void __fastcall SCmdSendCorpse(CLIENT *ptClient, DWORD dwUnitId,
	DWORD dwBodyId,
	BYTE bAdd);
void __fastcall SCmdSendNearList(CLIENT *ptClient);
int __fastcall SCmdSendItemStat(CLIENT *ptClient, UNIT *hUnit, BYTE bGate,
	BYTE bStat, DWORD dwValue);
struct SCMDCOPY33;
int __fastcall SCmdSendMissile(CLIENT *ptClient, const struct SCMDCOPY33 *ptPacket);

void __fastcall SCmdSendSkillState(CLIENT *ptClient, BYTE bType,
	DWORD dwUnitId,
	WORD wSkill, BYTE bCount);
void __fastcall SCmdSendItemToCursor(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
void __fastcall SCmdSendItemDropped(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
void __fastcall SCmdSendItemOnGround(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
void __fastcall SCmdSendItemToGrid(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
void __fastcall SCmdSendItemFromGrid(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
void __fastcall SCmdSendItemEquip(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
void __fastcall SCmdSendItemSwapHands(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
void __fastcall SCmdSendItemUnequip(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
void __fastcall SCmdSendItemSwapBodyLoc(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
void __fastcall SCmdSendItemStack(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
void __fastcall SCmdSendItemSwapGrid(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
void __fastcall SCmdSendItemToBelt(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
void __fastcall SCmdSendItemFromBelt(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
void __fastcall SCmdSendItemSwapBelt(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
int __fastcall SCmdSendItemStat(CLIENT *ptClient, UNIT *hUnit, BYTE bGate,
	BYTE bStat,
	DWORD dwValue);
int __fastcall SCmdSendSpellIcon(CLIENT *ptClient, UNIT *hUnit, BYTE bSpellIcon,
	DWORD fFlag,
	DWORD dwUnused, WORD wSkillId);
void __fastcall SCmdSendItemHandToGrid(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
void __fastcall SCmdSendCursorCleared(CLIENT *ptClient, UNIT *hUnit);
void __fastcall SCmdSendSetHandItem(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem);
void __fastcall SCmdSendItemEquipFromGrid(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
int __fastcall SCmdSendItemsRefreshed(CLIENT *ptClient, UNIT *hUnit);
void __fastcall SCmdSendEquipmentChecked(CLIENT *ptClient,
	UNIT *hUnit);
void __fastcall SCmdSendItemHeld(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
void __fastcall SCmdSendItemFlag(CLIENT *ptClient, UNIT *hUnit, UNIT *hOther,
	DWORD dwAction,
	DWORD dwFlagValue);
void __fastcall SCmdSendItemNoPlace(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem,
	DWORD dwFlags);
void __fastcall SCmdSendItemCmd(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, BYTE bCmd);

void __fastcall SCmdSendUnitMode(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, BYTE bMode, BYTE bHitClass,
	BYTE bLife);
void __fastcall SCmdSendUnitMessage(CLIENT *ptClient, const SCMDSYSTEMMSG *pSystemMsg);
int __fastcall SCmdSendQuests(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, void *ptQuestRecord, BYTE bField06);
int __fastcall SCmdSendInteractionStop(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, DWORD dwUnused);
int __fastcall SCmdSendMonsterMods(CLIENT *ptClient, DWORD dwUnitId,
	BYTE bType,
	WORD wNameNumber, WORD wUniqueMods01, WORD wUniqueMods2,
	WORD wChampion);
struct SCMDCOPY7;
int __fastcall SCmdSendUseItemState(CLIENT *ptClient, const struct SCMDCOPY7 *ptPacket);

void __fastcall SCmdSendSystemMsg(CLIENT *ptClient, const SCMDSYSTEMMSG *pSystemMsg);
void __fastcall SCmdSendPet(CLIENT *ptClient, BYTE bAdd, DWORD bPetType,
	DWORD dwPetId, DWORD dwOwnerId, WORD wClassId);
void __fastcall SCmdSendHireling(CLIENT *ptClient, BYTE bUnused,
	DWORD bPetType,
	DWORD dwPetId, DWORD dwOwnerId, WORD wClassId,
	DWORD dwSeed, DWORD dwHireId);
void __fastcall SCmdSendLife(CLIENT *ptClient, UNIT *hUnit);
int __fastcall SCmdSendPlayerInfo(UNIT *hPlayer, UNIT *hUnit);
void __fastcall SCmdSendAllParty(GAME *ptGame, UNIT *hUnit);
void __fastcall SCmdSendParty(UNIT *hPlayer, UNIT *hUnit);
void __fastcall SCmdSendAllCorpse(GAME *ptGame, DWORD dwUnitId,
	DWORD dwBodyId,
	BYTE bAdd);

void __fastcall SCmdSendChat(CLIENT *ptClient, SCMDCHATDATA *pChatData);
void __fastcall SCmdSendUnitModeAt(CLIENT *ptClient, BYTE bCmd, BYTE bTOU, DWORD dwUnitId,
	BYTE bMode, WORD wX, WORD wY,
	BYTE bHitClass, BYTE bLife);
void __fastcall SCmdSendUnitModeToUnit(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, BYTE bMode, BYTE bTargetType,
	DWORD dwTargetId, WORD wX, WORD wY);
void __fastcall SCmdSendUnitModeToPoint(CLIENT *ptClient, BYTE bCmd, BYTE bTOU,
	DWORD dwUnitId,
	BYTE bMode, WORD wTargetX, WORD wTargetY,
	BYTE bHitClass, WORD wX, WORD wY);
void __fastcall SCmdSendPlayerStatsPacked(CLIENT *ptClient, DWORD dwUnitId,
	DWORD dwHitpoints, DWORD dwMana, DWORD dwStamina,
	BYTE bHpRegen, BYTE bManaRecovery, WORD wX,
	WORD wY,
	WORD wTargetX, WORD wTargetY);

struct SCMDCOPY6;
struct SCMDCOPY15;
struct SCMDCOPY26;
int __fastcall SCmdSendQuestRecord(CLIENT *ptClient,
	const void *pPacket);
int __fastcall SCmdSendQuestStatus(CLIENT *ptClient,
	const void *pPacket);
int __fastcall SCmdSendQuestCommand(CLIENT *ptClient, const struct SCMDCOPY15 *ptPacket);
void __fastcall SCmdSendQuestList(CLIENT *ptClient,
	const void *pPacket);
int __fastcall SCmdSendQuestLog(CLIENT *ptClient,
	const void *pPacket);
int __fastcall SCmdSendVideo(CLIENT *ptClient,
	BYTE bValue);
int __fastcall SCmdSendQuestTrigger(CLIENT *ptClient,
	const void *pPacket);
void __fastcall SCmdSendNpcActive(CLIENT *ptClient, const struct SCMDCOPY6 *ptPacket);
int __fastcall SCmdSendNpcIntroduced(CLIENT *ptClient, const struct SCMDCOPY26 *ptPacket);

int __fastcall SCmdSendRosterState(CLIENT *ptClient, DWORD dwUnitId,
	BYTE bButton);
void __fastcall SCmdSendAllRelation(GAME *ptGame, DWORD dwUnitId,
	DWORD dwOtherId,
	WORD wFlags);
int __fastcall SCmdSendRelation(UNIT *hUnit, DWORD dwUnitId, DWORD dwOtherId,
	WORD wFlags);
void __fastcall SCmdSendUnitClass(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, BYTE bClassId, WORD wX,
	WORD wY);
void __fastcall SCmdSendPlayerWithInfo(CLIENT *ptClient, DWORD dwUnitId,
	BYTE bClassId,
	const char *szName, WORD wX, WORD wY);
void __fastcall SCmdSendObjectAt(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, WORD wClassId, WORD wX,
	WORD wY, BYTE bMode, BYTE bField0d);
void __fastcall SCmdSendItemNoOwner(CLIENT *ptClient, UNIT *hItem, DWORD dwFlags);
int __fastcall SCmdSendOverlay(CLIENT *ptClient, BYTE bType,
	DWORD dwUnitId,
	BYTE bOverlay, BYTE bFrames);
int __fastcall SCmdSendUnitChangeState(CLIENT *ptClient, BYTE bType,
	DWORD dwUnitId,
	const BYTE *pbStates);
void __fastcall SCmdSendUnitChangeStateVal(CLIENT *ptClient, BYTE bType,
	DWORD dwUnitId,
	DWORD dwState, DWORD dwValue);
void __fastcall SCmdSendUnitChangeStateValVal(CLIENT *ptClient, BYTE bType,
	DWORD dwUnitId,
	DWORD dwState, DWORD dwValue, DWORD dwValue2);
void __fastcall SCmdSendMonsterShort(CLIENT *ptClient, DWORD dwUnitId,
	WORD wClassId,
	WORD wX, WORD wY, BYTE bLife,
	BYTE bMode);
void __fastcall SCmdSendMonster(CLIENT *ptClient, UNIT *hUnit, BYTE bMode);
int __fastcall SCmdSendObject(CLIENT *ptClient, UNIT *hObject);
void __fastcall SCmdSendCorpseOwner(CLIENT *ptClient, UNIT *hUnit, UNIT *hOther,
	BOOL fGate);
void __fastcall SCmdSendPortalOwner(GAME *ptGame, CLIENT *ptClient, DWORD dwObjectId,
	DWORD dwObjectIdB);

struct SCMDCOPY8;
int __fastcall SCmdSendNpcResult(CLIENT *ptClient, BYTE bCmd, BYTE bResult,
	DWORD dwGold, DWORD dwUnitId, BYTE bField01);
int __fastcall SCmdSendUnitSound(CLIENT *ptClient, const struct SCMDCOPY8 *ptPacket);
int __fastcall SCmdSendHire(CLIENT *ptClient, const struct SCMDCOPY7 *ptPacket);
int __fastcall SCmdSendGuildSaveChunk(CLIENT *ptClient, const char *pszCode,
	WORD wLength, WORD wOffset, WORD wTotalSize,
	const BYTE *pbData);
void __fastcall SCmdSendGuildRosterChunk(CLIENT *ptClient, const char *pszCode,
	WORD wLength, WORD wOffset, WORD wTotalSize,
	const BYTE *pbData);
void __fastcall SCmdSendBuffer(CLIENT *ptClient, WORD wTotal,
	const BYTE *pbData);
void __fastcall SCmdSendSkill(UNIT *hUnit, WORD wSkill, BYTE bSkillLevel,
	BYTE bRemove);
void __fastcall SCmdSendCurrentSkill(CLIENT *ptClient, BYTE bType,
	DWORD dwUnitId,
	BYTE bLeft, WORD wSkillId);

#endif
