#ifndef D2RE_D2NET_IMPORTS_WRAPPERS_H
#define D2RE_D2NET_IMPORTS_WRAPPERS_H

#include "D2Net.imports.h"

#define D2NetClientInit(nMode, pszHost) \
	D2NetOrdinal10000((DWORD)(nMode), (DWORD)(pszHost))

#define D2NetClientClose() \
	((int)D2NetOrdinal10001())

#define D2NetServerWait(dwValue) \
	D2NetOrdinal10002((DWORD)(dwValue))

#define D2NetServerInit(nMode, fForceSelect) \
	((BYTE *)D2NetOrdinal10003((DWORD)(nMode), (DWORD)(fForceSelect)))

#define D2NetServerShutdown() \
	D2NetOrdinal10004()

#define D2NetSendToServer(bQueue, pbData, nSize) \
	((int)D2NetOrdinal10005((DWORD)(bQueue), (DWORD)(pbData), (DWORD)(nSize)))

#define D2NetServerSend(bQueue, hSocket, pbData, nSize) \
	D2NetOrdinal10006((DWORD)(bQueue), (DWORD)(hSocket), (DWORD)(pbData), (DWORD)(nSize))

#define D2NetClientReceiveGameMessage(pbBuffer, dwSize) \
	((int)D2NetOrdinal10007((DWORD)(pbBuffer), (DWORD)(dwSize)))

#define D2NetGetCltSysMsg(pbBuffer, dwSize) \
	((int)D2NetOrdinal10008((DWORD)(pbBuffer), (DWORD)(dwSize)))

#define D2NetClientReceiveOtherQueue(pbBuffer, dwSize) \
	((int)D2NetOrdinal10009((DWORD)(pbBuffer), (DWORD)(dwSize)))

#define D2NetServerReceiveClientCommand(pbBuffer, dwSize) \
	((int)D2NetOrdinal10010((DWORD)(pbBuffer), (DWORD)(dwSize)))

#define D2NetServerReceiveSystemMessage(pbBuffer, dwSize) \
	((int)D2NetOrdinal10011((DWORD)(pbBuffer), (DWORD)(dwSize)))

#define D2NetServerNothing(dwUnused1, dwUnused2) \
	((int)D2NetOrdinal10012((DWORD)(dwUnused1), (DWORD)(dwUnused2)))

#define D2NetClientGetLocalAddress(pszAddress) \
	D2NetOrdinal10013((DWORD)(pszAddress))

#define D2NetServerGetAddress(hSocket, pszAddress) \
	D2NetOrdinal10014((DWORD)(hSocket), (DWORD)(pszAddress))

#define D2NetServerDisconnect(dwValue) \
	D2NetOrdinal10015((DWORD)(dwValue))

#define D2NetServerClose(dwValue) \
	D2NetOrdinal10016((DWORD)(dwValue))

#define D2NetGetRemoteConnection() \
	D2NetOrdinal10017()

#define D2NetServerSetBlockCallback(dwCallback) \
	D2NetOrdinal10018((DWORD)(dwCallback))

#define D2NetServerSetUnusedHook(dwValue) \
	D2NetOrdinal10019((DWORD)(dwValue))

#define D2NetServerSetValue(hSocket, dwValue) \
	D2NetOrdinal10020((DWORD)(hSocket), (DWORD)(dwValue))

#define D2NetServerGetValue(hSocket) \
	D2NetOrdinal10021((DWORD)(hSocket))

#define D2NetServerService(dwTimeout) \
	D2NetOrdinal10022((DWORD)(dwTimeout))

#define D2NetServerSetBlocking(dwValue) \
	D2NetOrdinal10023((DWORD)(dwValue))

#define D2NetGetLastError() \
	((int)D2NetOrdinal10024())

#define D2NetClientPollConnect() \
	((int)D2NetOrdinal10025())

#define D2NetServerSetMaxConnections(dwCount) \
	D2NetOrdinal10026((DWORD)(dwCount))

#define D2NetClientInitRaw D2NetOrdinal10000
#define D2NetClientCloseRaw D2NetOrdinal10001
#define D2NetServerWaitRaw D2NetOrdinal10002
#define D2NetServerInitRaw D2NetOrdinal10003
#define D2NetServerShutdownRaw D2NetOrdinal10004
#define D2NetSendToServerRaw D2NetOrdinal10005
#define D2NetServerSendRaw D2NetOrdinal10006
#define D2NetClientReceiveGameMessageRaw D2NetOrdinal10007
#define D2NetGetCltSysMsgRaw D2NetOrdinal10008
#define D2NetClientReceiveOtherQueueRaw D2NetOrdinal10009
#define D2NetServerReceiveClientCommandRaw D2NetOrdinal10010
#define D2NetServerReceiveSystemMessageRaw D2NetOrdinal10011
#define D2NetServerNothingRaw D2NetOrdinal10012
#define D2NetClientGetLocalAddressRaw D2NetOrdinal10013
#define D2NetServerGetAddressRaw D2NetOrdinal10014
#define D2NetServerDisconnectRaw D2NetOrdinal10015
#define D2NetServerCloseRaw D2NetOrdinal10016
#define D2NetGetRemoteConnectionRaw D2NetOrdinal10017
#define D2NetServerSetBlockCallbackRaw D2NetOrdinal10018
#define D2NetServerSetUnusedHookRaw D2NetOrdinal10019
#define D2NetServerSetValueRaw D2NetOrdinal10020
#define D2NetServerGetValueRaw D2NetOrdinal10021
#define D2NetServerServiceRaw D2NetOrdinal10022
#define D2NetServerSetBlockingRaw D2NetOrdinal10023
#define D2NetGetLastErrorRaw D2NetOrdinal10024
#define D2NetClientPollConnectRaw D2NetOrdinal10025
#define D2NetServerSetMaxConnectionsRaw D2NetOrdinal10026

#endif
