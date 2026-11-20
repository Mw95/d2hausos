#ifndef D2RE_D2GAME_GUILDS_H
#define D2RE_D2GAME_GUILDS_H

#include "../D2game.h"

BOOL __fastcall AiRecordTestFlags(AIRECORDHEAD *ptRecord, WORD wMask);
void __fastcall AiRecordSetFlags(AIRECORDHEAD *ptRecord, WORD wMask, BOOL bSet);
void __fastcall AiSetMonTeleportFlag(UNIT *hUnit, BOOL bSet);
int __fastcall MonsterGetLifePercent(UNIT *hUnit);
BOOL __fastcall UnitLineClear(UNIT *hUnit, UNIT *hTarget);
BOOL __fastcall MonsterTestPrevModeGetHit(UNIT *hUnit);
void __fastcall AiSetUnitRef(UNIT *hUnit, UNIT *hRef);
void __fastcall MonsterScare(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, DWORD dwUnused, int nLength);
BOOL __fastcall MonsterCanBeAffected(UNIT *hUnit, int nKind);
void __fastcall GuildsInit(GAME *ptGame);
void __fastcall GuildsFree(GAME *ptGame);
void __fastcall GuildsSendSave(GAME *ptGame, CLIENT *ptClient);
void __fastcall GuildsSendRoster(GAME *ptGame, CLIENT *ptClient);
void __fastcall GuildsSendPosts(GAME *ptGame, CLIENT *ptClient);
void __fastcall GuildsPost(GAME *ptGame, CLIENT *ptClient, char *pszText);
UNIT * __fastcall GuildsGetHall(GAME *ptGame);
void * __fastcall GuildsGetData(GAME *ptGame);
void __fastcall GuildsSpawnRankObject(GAME *ptGame, int nLevelId, BYTE bRank);
void __fastcall GuildsSave(GAME *ptGame);
void __fastcall GuildsLoadItems(GAME *ptGame, UNIT *hUnit, BYTE *pbData, WORD wLength);
void __fastcall GuildsLoadVaultItems(GAME *ptGame, UNIT *hUnit);
void __fastcall GuildsLoadTrophyCaseItems(GAME *ptGame, UNIT *hUnit);
BOOL __stdcall GuildsReceiveSave(WORD wGameId, DWORD dwUnused, WORD wChunk, WORD wOffset, WORD wTotal,
	const BYTE *pbChunk);
BOOL __stdcall GuildsReceiveRoster(WORD wGameId, DWORD dwUnused, WORD wChunk, WORD wOffset, WORD wTotal,
	const BYTE *pbChunk);

#endif
