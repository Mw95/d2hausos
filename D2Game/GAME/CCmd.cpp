#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\GAME\\CCmd.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "CCmd.h"
#include "Event.h"
#include "Clients.h"
#include "Game.h"
#include "../PLAYER/PlrMsg.h"

void __fastcall CCmdProcess(SRVMSG *ptMsg, DWORD dwLength)
{
	DWORD dwClientId;
	BYTE *pbCommand;
	GAME *ptGame;
	CLIENT *ptClient;
	UNIT *hUnit;

	dwClientId = ptMsg->dwClientId;
	pbCommand = &ptMsg->uPacket.bCmd;
	ptGame = GameLockByClient(dwClientId);
	if (!ptGame)
		return;
	ptClient = ClientsFindInGame(ptGame, dwClientId);
#line 217
	D2ASSERT(ptClient);
	hUnit = ClientGetPlayerUnit(ptClient, FALSE);
	if (hUnit && hUnit->eType == UNIT_PLAYER
		&& PlrMsgHandleCommand(ptGame, hUnit, pbCommand, (int)dwLength) >= 3)
	{
		GameLog(1, "[HACKLIST]  Client '%s' believed to be cheating.  Command was:%d",
			ptClient->szName,
			(DWORD)*pbCommand);
		GameRemoveClient(ptGame, dwClientId);
	}
	GameUnlock(ptGame);
}
