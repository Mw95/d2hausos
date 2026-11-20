#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\OBJECTS\\OBJEVAL.CPP"
#include "../D2game.h"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "Objects.h"
#include "objrgn.h"
#include "../UNIT/SUnitDmg.h"

BYTE __fastcall ObjectRegenerateModes(void)
{
	return 2;
}

void __fastcall ObjectEvalSetLinkValue(UNIT *hObject, int nValue)
{
	OBJLINKS *ptLinks;

	ptLinks = (OBJLINKS *)D2CommonUnitGetObjectLinks(hObject);
	if (ptLinks)
		ptLinks->nValue = nValue;
}

int __fastcall ObjectEvalGetLinkValue(UNIT *hObject)
{
	OBJLINKS *ptLinks;

	ptLinks = (OBJLINKS *)D2CommonUnitGetObjectLinks(hObject);
	if (ptLinks)
		return ptLinks->nValue;
	return -1;
}

static DWORD ObjectEvalRollDamage(UNIT *hObject,
	DWORD nMinD  , DWORD nMaxD  )
{
	DWORD dwRoll;
	MISSOBJECTSTXT *ptObjectData;
	DWORD dwDamage;

#line 17
	D2ASSERT(nMinD <= nMaxD);
	nMaxD = nMaxD - nMinD + 256;
	if (nMaxD)
		dwRoll = D2CommonSeedRand(&hObject->tSeed, nMaxD);
	else
		dwRoll = 0;
	ptObjectData = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hObject->dwClassId);
	dwDamage = (dwRoll + nMinD) * ptObjectData->bDamage / 100;
#line 24
	D2WARN(ptObjectData->bDamage);
	return dwDamage;
}

void __fastcall ObjectEvalDamageEnemy(GAME *ptGame, UNIT *hObject, UNIT *hEnemy, int nType)
{
	GAMEROOM *hRoom;
	int nLife;
	int nMinD;
	int nMaxD;
	int nStat12;
	int nRoll;
	int nChance;
	DWORD dwDamage;
	DAMAGE tDamage;
	int i;
	D2SEED *ptSeed;

	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hEnemy);
	D2CommonDungeonRoomGetLevelId(hRoom);
	if (D2CommonDungeonRoomIsTown(hRoom))
		return;
	if (D2CommonUnitTestLineBlockedIgnoring(hEnemy, hObject, 2052))
		return;
	ptSeed = &hObject->tSeed;
#line 61
	D2ASSERT(hEnemy != NULL);
	nLife = D2CommonStatsUnitGetValue(hEnemy, 6);
	nRoll = nLife >> 5;
	if (nRoll > 1)
		nMinD = nRoll;
	else
		nMinD = 1;
	nRoll = nMinD + 1;
	if ((nLife >> 3) > nRoll)
		nMaxD = (nLife >> 3);
	else
		nMaxD = nRoll;
	nStat12 = D2CommonStatsUnitGetValue(hEnemy, 12);

	nChance = nStat12 >> 2;
	nChance = ((BYTE)((nChance ? (int)D2CommonSeedRand(ptSeed, nChance) : 0) + nStat12) - (D2CommonStatsUnitGetValue(hEnemy, 2) >> 1) * 5 - nStat12)
		* 2 - D2CommonStatsUnitGetValue(hEnemy, 31) + 125;
	if (nChance < 65)
		nChance = 65;
	if ((int)D2CommonSeedRand(&hObject->tSeed, 100) >= nChance)
		return;
	dwDamage = ObjectEvalRollDamage(hObject, nMinD, nMaxD);
	if (!dwDamage)
		return;
	for (i = 0; i < (int)(sizeof(DAMAGE) / sizeof(DWORD)); i++)
		((DWORD *)&tDamage)[i] = 0;
	switch (nType)
	{
	case 1:
		tDamage.nFireDamage = (int)dwDamage;
		tDamage.dwHitClass = 32;
		break;
	case 2:
		tDamage.nLightningDamage = (int)dwDamage;
		tDamage.dwHitClass = 64;
		break;
	case 3:
		tDamage.nColdDamage = (int)dwDamage;
		tDamage.dwHitClass = 48;
		break;
	default:
		tDamage.nPhysicalDamage = (int)dwDamage;
		break;
	}
	tDamage.wFlags = 1;
	if (!D2CommonStatsGetState(hEnemy, 54))
		tDamage.wFlags |= 4;
	SUnitDmgResist(hObject, hEnemy, &tDamage);
	SUnitDmgApply(ptGame, hObject, hEnemy, 0, &tDamage);
	SUnitDmgReact(ptGame, hObject, hEnemy, &tDamage);
}

void __fastcall ObjectEvalUpdate(GAME *ptGame, UNIT *hObject)
{
	WORD wMode;
	int nEnv;

	wMode = (WORD)hObject->dwMode;
	if (hObject->dwClassId == 399)
	{
		switch (wMode)
		{
		case 0:
		case 2:
			break;
		default:
			return;
		}
		EventAdd(ptGame, hObject, 8, (int)D2CommonSeedRand(
			ObjectRgnGetSeed(ptGame, (GAMEROOM *)D2CommonUnitGetPathRoom(hObject)), 250)
			+ ptGame->nFrame + 25);
		D2CommonUnitSetMode(hObject, 1);
		EventAdd(ptGame, hObject, 1,
			(int)((DWORD)((MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hObject->dwClassId))->FrameCnt[1] >> 8)
			+ ptGame->nFrame + 1);
		return;
	}
	nEnv = D2CommonEnvGetCycleValue(ptGame->ptAct[0]);
	switch (nEnv)
	{
	case 0:
		if (wMode == 2 || wMode == 1)
			D2CommonUnitSetMode(hObject, 0);
		EventAdd(ptGame, hObject, 8, ptGame->nFrame + 1000);
		return;
	case 1:
	case 2:
	case 3:
		if (wMode == 0)
		{
			D2CommonUnitSetMode(hObject, 1);
			EventAdd(ptGame, hObject, 1,
				(int)((DWORD)((MISSOBJECTSTXT *)D2CommonObjTblsGetObject(
					hObject->dwClassId))->FrameCnt[1] >> 8)
				+ ptGame->nFrame + 1);
		}
		EventAdd(ptGame, hObject, 8, ptGame->nFrame + 1000);
		return;
	}
	EventAdd(ptGame, hObject, 8, ptGame->nFrame + 600);
}
