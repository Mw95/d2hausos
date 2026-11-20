#ifndef D2RE_D2GAME_QUESTS_H
#define D2RE_D2GAME_QUESTS_H

#include "../D2game.h"

void __fastcall QuestsFreeUnitLinks(GAME *ptGame, QUESTLINK *pHead);
QUEST * __fastcall QuestsFind(GAME *ptGame, int nId);
void __fastcall QuestsAttachLevelQuest(GAME *ptGame, UNIT *hUnit, GAMEROOM *hRoom, BOOL fAttach);
BOOL __fastcall QuestsAttachUnit(GAME *ptGame, UNIT *hUnit, int nQuest);
void __fastcall QuestsNpcTable(QUEST *pQuest, void *pArg, int nNpcClass, int nIndex);
void __fastcall QuestsInit(GAME *ptGame);
void __fastcall QuestsFree(GAME *ptGame);
D2SEED * __fastcall QuestsGetSeed(GAME *ptGame);
void __fastcall QuestsUnitKilled(GAME *ptGame, UNIT *hUnit, UNIT *hKiller);
void __fastcall QuestsUnitCallback(QUESTPARAM *pqParam, BYTE fForce);
void __fastcall QuestsLevelChange(GAME *ptGame, DWORD dwLevelOld, DWORD dwLevelNew, UNIT *hUnit);
void __fastcall QuestsCallback(QUESTPARAM *pqParam, BYTE fForce, BYTE fSameAct);
void __fastcall QuestsPlayerDropped(GAME *ptGame, UNIT *hUnit);
void __fastcall QuestsNpcMessage(CLIENT *ptClient, UNIT *hUnit, UNIT *hNpc, DWORD dwMessage);
void __fastcall QuestsNpcActivate(GAME *ptGame, UNIT *hUnit, UNIT *hNpc);
void __fastcall QuestsItemAcquired(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
void __fastcall QuestsItemDropped(GAME *ptGame, UNIT *hUnit, UNIT *hItem);
void __fastcall QuestsUpdateTimers(GAME *ptGame);
void __fastcall QuestsAddTimer(QUEST *pQuest, PFNQUESTTIMER pfnTimer, DWORD dwInterval);
void __fastcall QuestsGetStatus(QUEST *pQuest, BYTE *pbStatus, void *pRecord);
void __fastcall QuestsSendAllStatus(GAME *ptGame, UNIT *hPlayer);
void __fastcall QuestsDebugMessage(GAME *ptGame, const char *szText, const char *szFile, int nLine);
void __fastcall QuestsRemoveItem(GAME *ptGame, UNIT *hUnit, DWORD dwType);
void __fastcall QuestsSendStatus(GAME *ptGame, UNIT *hPlayer, BYTE bQuest);
void __fastcall QuestsSetStatus(QUEST *pQuest, DWORD dwStatus, void *pArg, PFNSUNITITERATE pfnIterate,
	BYTE fIterate);
void __fastcall QuestsSetState(QUEST *pQuest, int nState, const char *szFile, int nLine);
void __fastcall QuestsMonsterMessage(GAME *ptGame, UNIT *hUnit, DWORD dwMonsterId, WORD wMessage);
void __fastcall QuestsPlayerStarted(GAME *ptGame, UNIT *hUnit, BOOL fJoined);
DWORD __stdcall QuestsGetPickedSet(GAME *ptGame);
BOOL __stdcall QuestsTestQuest(GAME *ptGame, int nQuest);
void __fastcall QuestsSendRecord(GAME *ptGame, CLIENT *ptClient);
BOOL __fastcall QuestsNpcActive(GAME *ptGame, UNIT *hPlayer, UNIT *hNPC);
void __fastcall QuestsSetRecordBit(GAME *ptGame, int nQuest, int nBit);
DWORD __fastcall QuestsGetRecordBit(GAME *ptGame, int nQuest, int nBit);
void __fastcall QuestsObjectSpawned(GAME *ptGame, GAMEROOM *hRoom, int nX, int nY, int nClass);
void __fastcall QuestsUseItem(GAME *ptGame, UNIT *hUnit, DWORD dwItemId);
void __fastcall QuestsInitJerhynMonster(OBJINIT *ptInit);
void __fastcall QuestsInitJerhynNpc(OBJINIT *ptInit);
void __fastcall QuestsInitTaintedSunAltar(OBJINIT *ptInit);
void __fastcall QuestsInitMalus(OBJINIT *ptInit);
void __fastcall QuestsInitGibbet(OBJINIT *ptInit);
void __cdecl QuestsClearGate(void);
void __fastcall QuestsObjectOperate(GAME *ptGame, UNIT *hObject);
void __fastcall QuestsInitLamTome(OBJINIT *ptInit);
void __fastcall QuestsOfferFigurineMonster(GAME *ptGame, UNIT *hMonster);
void __fastcall QuestsReleasePickedMonster(GAME *ptGame, UNIT *hUnit);
UNIT * __fastcall QuestsCreateItem(GAME *ptGame, UNIT *hUnit, DWORD dwCode, int nLevel, DWORD dwQuality);
void __fastcall QuestsSendIntroduced(GAME *ptGame, UNIT *hUnit, BYTE bAct);
void __fastcall QuestsChangedLevel(GAME *ptGame, UNIT *hUnit, QUESTLEVELMSG *ptMsg, DWORD dwLevelNew,
	DWORD dwLevelOld);
void __fastcall QuestsObjectLevel(GAME *ptGame, UNIT *hPlayer, DWORD dwLevelId, UNIT *hObject);
void __fastcall QuestsPlayersAdd(QUESTPLAYERS *pPlayers, DWORD dwUnitId);
BOOL __fastcall QuestsPlayersRemove(QUESTPLAYERS *pPlayers, DWORD dwUnitId);
BOOL __fastcall QuestsPlayersFind(QUESTPLAYERS *pPlayers, DWORD dwUnitId);
BOOL __fastcall QuestsHasPlayer(QUEST *pQuest, DWORD dwUnitId);
void __fastcall QuestsPlayersClear(QUESTPLAYERS *pPlayers);
void __fastcall QuestsPlayerLeft(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall QuestsFindSpot(GAMEROOM *hRoom, GAMECOORD *ptCoord, int nSize, DWORD dwFlags,
	GAMEROOM **phRoom);
void __fastcall QuestsPlayerDone(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall QuestsSendPlayerRecord(GAME *ptGame, UNIT *hUnit);
void __fastcall QuestsPlayersSetStatus(GAME *ptGame, QUESTPLAYERS *pPlayers, int nQuest, DWORD dwEvent);
void __fastcall QuestsSendUnitMessage(UNIT *hPlayer, UNIT *hUnit, WORD wMessage);
void __fastcall QuestsSetAll(GAME *ptGame, BYTE bValue);
BOOL __fastcall QuestsSendAllCb(GAME *ptGame, UNIT *hUnit, void *pArg);
void __fastcall QuestsTextMessage(GAME *ptGame, UNIT *hUnit, UNIT *hNpc);
BOOL __fastcall QuestsPortalSpot(GAME *ptGame, GAMECOORD *ptCoord, int nType, GAMEROOM **phRoom);
BOOL __fastcall QuestsOperateObject(OBJOPERATE *pObjectParam);
void __fastcall QuestsSendFlag(UNIT *hUnit, DWORD bQuest);
void __fastcall QuestsSendFlagInAct(UNIT *hUnit, DWORD dwQuest, DWORD dwAct);
void __fastcall QuestsDropItem(QUEST *pQuest, QUESTPARAM *pqParam, int nTreasureClass);
void __fastcall QuestsIntroInit(QUEST *pQuest);
void __fastcall QuestsIntroMet(QUEST *pQuest, QUESTPARAM *pqParam);
void __fastcall QuestsIntroTalk(QUEST *pQuest, QUESTPARAM *pqParam);
BYTE __fastcall QuestsIntroActive(QUEST *pQuest, int nNpcClass, UNIT *hPlayer, void *pPlayerRecord);
void __fastcall QuestsInitWarriv(QUEST *pQuest);

#endif
