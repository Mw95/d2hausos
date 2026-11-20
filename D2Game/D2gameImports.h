#ifndef D2RE_D2GAME_IMPORTS_WRAPPERS_H
#define D2RE_D2GAME_IMPORTS_WRAPPERS_H

#include "D2game.imports.h"

#define D2gameArenaGetUnwrittenValue() \
	D2gameOrdinal10001()

#define D2gameGameSetDataTable(ptGameDataTbl, phGameList) \
	D2gameOrdinal10002((DWORD)(ptGameDataTbl), (DWORD)(phGameList))

#define D2gameGameProcessMessages() \
	D2gameOrdinal10003()

#define D2gameGameUpdateAll(bLimit) \
	D2gameOrdinal10004((DWORD)(bLimit))

#define D2gameGameSendAll(fUpdated, fFinal) \
	D2gameOrdinal10005((DWORD)(fUpdated), (DWORD)(fFinal))

#define D2gameGameCloseAll() \
	D2gameOrdinal10006()

#define D2gameGameReceiveData(dwClientId, pbData, wLength, wTotal, bDrop) \
	D2gameOrdinal10007((DWORD)(dwClientId), (DWORD)(pbData), (DWORD)(wLength), (DWORD)(wTotal), \
		(DWORD)(bDrop))

#define D2gameGameSetFps(nFps) \
	((int)D2gameOrdinal10008((DWORD)(nFps)))

#define D2gameGameSetStartAct(dwAct) \
	D2gameOrdinal10009((DWORD)(dwAct))

#define D2gameGameSetSeed(dwSeed) \
	D2gameOrdinal10010((DWORD)(dwSeed))

#define D2gameGameGetFirstSeed() \
	D2gameOrdinal10011()

#define D2gameGameGetCount() \
	((int)D2gameOrdinal10012())

#define D2gameGameGetNumClients(wGameId) \
	((int)((DWORD (__stdcall *)(WORD))D2gameOrdinal10013)((WORD)(wGameId)))

#define D2gameGameGetInfo(wGameId, ptInfo) \
	((BOOL)D2gameOrdinal10014((DWORD)(wGameId), (DWORD)(ptInfo)))

#define D2gameGameGetIds(pwIds, nMax) \
	((int)D2gameOrdinal10015((DWORD)(pwIds), (DWORD)(nMax)))

#define D2gameGameCountPlayers(wGameId) \
	((int)((DWORD (__stdcall *)(WORD))D2gameOrdinal10016)((WORD)(wGameId)))

#define D2gameGameGetPlayerInfo(wGameId, ptInfo, nMax) \
	((int)((DWORD (__stdcall *)(WORD, void *, DWORD))D2gameOrdinal10017)((WORD)(wGameId), (void *)(ptInfo), \
		(DWORD)(nMax)))

#define D2gameGameGetUnitInfo(wGameId, nCount, ptInfo, peTOUs, pdwIds) \
	((int)((DWORD (__stdcall *)(WORD, DWORD, void *, void *, void *))D2gameOrdinal10018)((WORD)(wGameId), \
		(DWORD)(nCount), (void *)(ptInfo), (void *)(peTOUs), (void *)(pdwIds)))

#define D2gameGameCountUnitClasses(wGameId, ptList, eType) \
	D2gameOrdinal10019((DWORD)(wGameId), (DWORD)(ptList), (DWORD)(eType))

#define D2gameGameCountAllUnits(ptCensus) \
	D2gameOrdinal10020((DWORD)(ptCensus))

#define D2gameGameBroadcast(bEvent, dwArg, szText) \
	D2gameOrdinal10021((DWORD)(bEvent), (DWORD)(dwArg), (DWORD)(szText))

#define D2gameGameSendText(wGameId, szText) \
	((DWORD (__stdcall *)(WORD, void *))D2gameOrdinal10022)((WORD)(wGameId), (void *)(szText))

#define D2gameGameSetEventCallbacks(ptTable) \
	D2gameOrdinal10023((DWORD)(ptTable))

#define D2gameGameKickClient(dwClientId) \
	GameKickClient((DWORD)(dwClientId))

#define D2gameDebugFindUnit(eType, dwId) \
	((void *)D2gameOrdinal10025((DWORD)(eType), (DWORD)(dwId)))

#define D2gameDebugGetAct(nAct) \
	((void *)D2gameOrdinal10026((DWORD)(nAct)))

#define D2gameDebugCountMonsters() \
	((int)D2gameOrdinal10027())

#define D2gameDebugFindRoom(bAct, nX, nY) \
	((void *)D2gameOrdinal10028((DWORD)(bAct), (DWORD)(nX), (DWORD)(nY)))

#define D2gamePlrSaveCharacterStd(ptGame, hUnit, pszName) \
	D2gameOrdinal10029((DWORD)(ptGame), (DWORD)(hUnit), (DWORD)(pszName))

#define D2gameSUnitDmgRollRange(hUnit, pnFirst, pnSecond) \
	((BOOL)D2gameOrdinal10033((DWORD)(hUnit), (DWORD)(pnFirst), (DWORD)(pnSecond)))

#define D2gameQuestsClearGate() \
	D2gameOrdinal10035()

#define D2gamePlrMsgSetSaveEnabled(dwEnabled) \
	D2gameOrdinal10036((DWORD)(dwEnabled))

#define D2gameQuestsGetPickedSet(ptGame) \
	D2gameOrdinal10037((DWORD)(ptGame))

#define D2gameQuestsTestQuest(ptGame, nQuest) \
	((BOOL)D2gameOrdinal10038((DWORD)(ptGame), (DWORD)(nQuest)))

#define D2gameTaskInitTimer() \
	D2gameOrdinal10039()

#define D2gameTaskFreeQueues() \
	D2gameOrdinal10040()

#define D2gameTaskCreateQueue() \
	((int)((DWORD (__cdecl *)(void))D2gameOrdinal10041)())

#define D2gameTaskGetDue(nQueue, pptTask) \
	((int)D2gameOrdinal10043((DWORD)(nQueue), (DWORD)(pptTask)))

#define D2gameTaskGetWait(nQueue) \
	((int)D2gameOrdinal10044((DWORD)(nQueue)))

#define D2gameTaskProcessGame(nQueue, ptTask) \
	D2gameOrdinal10045((DWORD)(nQueue), (DWORD)(ptTask))

#define D2gameGameServerInit() \
	((BOOL)D2gameOrdinal10046())

#define D2gameGuildsReceiveSave(wGameId, dwUnused, wChunk, wOffset, wTotal, pbChunk) \
	((BOOL)D2gameOrdinal10048((DWORD)(wGameId), (DWORD)(dwUnused), (DWORD)(wChunk), (DWORD)(wOffset), \
		(DWORD)(wTotal), (DWORD)(pbChunk)))

#define D2gameGuildsReceiveRoster(wGameId, dwUnused, wChunk, wOffset, wTotal, pbChunk) \
	((BOOL)D2gameOrdinal10049((DWORD)(wGameId), (DWORD)(dwUnused), (DWORD)(wChunk), (DWORD)(wOffset), \
		(DWORD)(wTotal), (DWORD)(pbChunk)))

#define D2gameGameListFree() \
	((BOOL)D2gameOrdinal10050())

#define D2gameGameGetFps() \
	D2gameOrdinal10051()

#define D2gameArenaGetUnwrittenValueRaw D2gameOrdinal10001
#define D2gameGameSetDataTableRaw D2gameOrdinal10002
#define D2gameGameProcessMessagesRaw D2gameOrdinal10003
#define D2gameGameUpdateAllRaw D2gameOrdinal10004
#define D2gameGameSendAllRaw D2gameOrdinal10005
#define D2gameGameCloseAllRaw D2gameOrdinal10006
#define D2gameGameReceiveDataRaw D2gameOrdinal10007
#define D2gameGameSetFpsRaw D2gameOrdinal10008
#define D2gameGameSetStartActRaw D2gameOrdinal10009
#define D2gameGameSetSeedRaw D2gameOrdinal10010
#define D2gameGameGetFirstSeedRaw D2gameOrdinal10011
#define D2gameGameGetCountRaw D2gameOrdinal10012
#define D2gameGameGetNumClientsRaw D2gameOrdinal10013
#define D2gameGameGetInfoRaw D2gameOrdinal10014
#define D2gameGameGetIdsRaw D2gameOrdinal10015
#define D2gameGameCountPlayersRaw D2gameOrdinal10016
#define D2gameGameGetPlayerInfoRaw D2gameOrdinal10017
#define D2gameGameGetUnitInfoRaw D2gameOrdinal10018
#define D2gameGameCountUnitClassesRaw D2gameOrdinal10019
#define D2gameGameCountAllUnitsRaw D2gameOrdinal10020
#define D2gameGameBroadcastRaw D2gameOrdinal10021
#define D2gameGameSendTextRaw D2gameOrdinal10022
#define D2gameGameSetEventCallbacksRaw D2gameOrdinal10023
#define D2gameGameKickClientRaw D2gameOrdinal10024
#define D2gameDebugFindUnitRaw D2gameOrdinal10025
#define D2gameDebugGetActRaw D2gameOrdinal10026
#define D2gameDebugCountMonstersRaw D2gameOrdinal10027
#define D2gameDebugFindRoomRaw D2gameOrdinal10028
#define D2gamePlrSaveCharacterStdRaw D2gameOrdinal10029
#define D2gameSUnitDmgRollRangeRaw D2gameOrdinal10033
#define D2gameQuestsClearGateRaw D2gameOrdinal10035
#define D2gamePlrMsgSetSaveEnabledRaw D2gameOrdinal10036
#define D2gameQuestsGetPickedSetRaw D2gameOrdinal10037
#define D2gameQuestsTestQuestRaw D2gameOrdinal10038
#define D2gameTaskInitTimerRaw D2gameOrdinal10039
#define D2gameTaskFreeQueuesRaw D2gameOrdinal10040
#define D2gameTaskCreateQueueRaw D2gameOrdinal10041
#define D2gameTaskQueueGameRaw D2gameOrdinal10042
#define D2gameTaskGetDueRaw D2gameOrdinal10043
#define D2gameTaskGetWaitRaw D2gameOrdinal10044
#define D2gameTaskProcessGameRaw D2gameOrdinal10045
#define D2gameGameServerInitRaw D2gameOrdinal10046
#define D2gameGameCreateRaw D2gameOrdinal10047
#define D2gameGuildsReceiveSaveRaw D2gameOrdinal10048
#define D2gameGuildsReceiveRosterRaw D2gameOrdinal10049
#define D2gameGameListFreeRaw D2gameOrdinal10050
#define D2gameGameGetFpsRaw D2gameOrdinal10051

#endif
