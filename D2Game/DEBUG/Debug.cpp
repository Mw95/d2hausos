#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\DEBUG\\Debug.cpp"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Game.h"
#include "../UNIT/SUnit.h"
#include "Debug.h"
#include "../QUESTS/QUESTS.h"

UNIT *__stdcall DebugFindUnit(DWORD eType, DWORD dwId)
{
	GAME *ptGame;
	UNIT *hUnit;

	ptGame = GameLockFirst();
#line 89
	D2ASSERT(ptGame);
	hUnit = SUnitFind(ptGame, eType, dwId);
	GameUnlockFirst();
	return hUnit;
}

struct ACT *__stdcall DebugGetAct(int nAct)
{
	GAME *ptGame;
	struct ACT *ptAct;

	ptGame = GameLockFirst();
#line 101
	D2ASSERT(ptGame);
	ptAct = ptGame->ptAct[nAct];
	GameUnlockFirst();
	return ptAct;
}

int __cdecl DebugCountMonsters(void)
{
	GAME *ptGame;
	UNIT *hUnit;
	int nCount;
	int i;

	ptGame = GameLockFirst();
#line 119
	D2ASSERT(ptGame);
	nCount = 0;
	for (i = 0; i < 128; i++)
	{
		for (hUnit = ptGame->pUnitList[UNIT_MONSTER][i]; hUnit; hUnit = SUnitGetNextInList(hUnit))
			nCount++;
	}
	GameUnlockFirst();
	return nCount;
}

GAMEROOM *__stdcall DebugFindRoom(BYTE bAct  , int nX, int nY
)
{
	GAME *ptGame;
	GAMEROOM *hRoom;

	ptGame = GameLockFirst();
#line 137
	D2ASSERT(bAct < NUM_ACTS);
	hRoom = (GAMEROOM *)D2CommonDungeonActFindRoomByPoint(ptGame->ptAct[bAct], nX, nY);
	GameUnlockFirst();
	return hRoom;
}

void __cdecl DebugQuestsClearGate(void)
{
	QuestsClearGate();
}

void __fastcall DebugUnitValue(GAME *ptGame, UNIT *hAttacker, UNIT *hTarget,
	int nValue, DWORD dwArg)
{
}
