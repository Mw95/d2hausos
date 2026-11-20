#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\GAME\\SCmd.cpp"
#include <string.h>
#include "../D2game.h"
#include "D2Macros.h"
#include "Arena.h"
#include "Clients.h"
#include "Event.h"
#include "SCmd.h"
#include "../MONSTER/MonsterAI.h"
#include "../MONSTER/MonsterUnique.h"
#include "../OBJECTS/Objects.h"
#include "../PLAYER/PlayerPets.h"
#include "../UNIT/Party.h"
#include "../UNIT/PlayerList.h"
#include "../UNIT/SUnit.h"
#include "../AI/AiGeneral.h"
#include "D2NetImports.h"
#include "FogImports.h"

#define NUM_UNITTYPE			6
#define CHAT_MAX_CHAR_LENGTH	256
#define MAX_PLR_NAME_LEN		16
#define UNIT_MONSTER			1
#define UNIT_ITEM				4
#define UNIT_MISSILE			3
#define SCMD_NODESIZE			512

#pragma pack(push, 1)
struct SCMDPACKET1
{
	BYTE		bCmd;
};
struct SCMDPACKETHDR
{
	WORD		wSize;
	BYTE		bCmd;
	BYTE		unobserved_03[512 - 3];
};
struct SCMDPACKET5
{
	BYTE		bCmd;
	DWORD		dwValue;
};
struct SCMDPACKET6
{
	BYTE		bCmd;
	BYTE		bField01;
	DWORD		dwField02;
};
struct SCMDPACKETB850
{
	BYTE		bCmd;
	BYTE		bAct;
	DWORD		dwSeed;
	WORD		wLevelId;
	DWORD		dwObjectSeed;
};
struct SCMDPACKETB8E0
{
	BYTE		bCmd;
	BYTE		bType;
	DWORD		dwUnitId;
	BYTE		bMode;
	BYTE		bField07;
	BYTE		bField08;
};
struct SCMDPACKETB920
{
	BYTE		bCmd;
	BYTE		bType;
	DWORD		dwUnitId;
	BYTE		bField06;
	BYTE		bField07;
	DWORD		dwField08;
};
struct SCMDPACKETB960
{
	BYTE		bCmd;
	BYTE		bTOU;
	DWORD		dwUnitId;
	BYTE		bMode;
	WORD		wField07;
	WORD		wField09;
	BYTE		bField0b;
	BYTE		bLife;
};
struct SCMDPACKETB9E0
{
	BYTE		bCmd;
	BYTE		bType;
	DWORD		dwUnitId;
	BYTE		bMode;
	BYTE		bField07;
	DWORD		dwField08;
	WORD		wX;
	WORD		wY;
};
struct SCMDPACKETBA40
{
	BYTE		bCmd;
	BYTE		bTOU;
	DWORD		dwUnitId;
	BYTE		bMode;
	WORD		wField07;
	WORD		wField09;
	BYTE		bField0b;
	WORD		wX;
	WORD		wY;
};
struct SCMDPACKETMOVE20
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	BYTE		bMode;
	WORD		wX;
	WORD		wY;
	BYTE		bTargetType;
	DWORD		dwTargetId;
	BYTE		bField0f;
	BYTE		bHitClass;
	BYTE		bPathType;
	BYTE		bVelocity;
	BYTE		bField13;
};
struct SCMDPACKETMOVE15
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	BYTE		bMode;
	WORD		wX;
	WORD		wY;
	BYTE		bField0a;
	BYTE		bHitClass;
	BYTE		bPathType;
	BYTE		bVelocity;
	BYTE		bField0e;
};
struct SCMDPACKETBEB0
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	BYTE		bMode;
	BYTE		bTargetType;
	DWORD		dwTargetId;
	BYTE		bField0b;
};
struct SCMDPACKETBF00
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	BYTE		bMode;
	WORD		wTargetX;
	WORD		wTargetY;
	BYTE		bField0a;
	BYTE		bHitClass;
};
struct SCMDPACKETBF60
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	BYTE		bMode;
	BYTE		bTargetType;
	DWORD		dwTargetId;
	BYTE		bField0b;
	WORD		wX;
	WORD		wY;
};
struct SCMDPACKETBFD0
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	BYTE		bMode;
	WORD		wTargetX;
	WORD		wTargetY;
	BYTE		bField0a;
	BYTE		bHitClass;
	WORD		wX;
	WORD		wY;
};
struct SCMDPACKETUNIT
{
	BYTE		bCmd;
	BYTE		bType;
	DWORD		dwUnitId;
	WORD		wX;
	WORD		wY;
	BYTE		bField0a;
};
struct SCMDPACKETROOM
{
	BYTE		bCmd;
	WORD		wX;
	WORD		wY;
	BYTE		bLevelId;
};
struct SCMDPACKETC120
{
	BYTE		bCmd;
	BYTE		bType;
	DWORD		dwUnitId;
	BYTE		bClassId;
	WORD		wX;
	WORD		wY;
};
struct SCMDPACKET52
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	BYTE		bClassId;
	char		szName[16];
	WORD		wX;
	WORD		wY;
};
struct SCMDPACKET68
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	WORD		wParty;
	WORD		wLevel;
	WORD		wFlags;
	WORD		wButton;
};
struct SCMDPACKETC260
{
	BYTE		bCmd;
	BYTE		bType;
	DWORD		dwUnitId;
	WORD		wClassId;
	WORD		wX;
	WORD		wY;
	BYTE		bMode;
	BYTE		bField0d;
};
struct SCMDPACKET1A
{
	BYTE		bCmd;
	BYTE		bType;
	BYTE		bRemove;
	DWORD		dwUnitId;
	WORD		wSkill;
	BYTE		bField09;
	BYTE		bBonus;
	BYTE		bField0b;
};
struct SCMDPACKET1C
{
	BYTE		bCmd;
	BYTE		bType;
	DWORD		dwUnitId;
	BYTE		bLeft;
	WORD		wSkillId;
};
struct SCMDPACKET54
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	BYTE		bClass;
	char		szName[16];
	WORD		wLevel;
	WORD		wParty;
	WORD		wField1a;
	WORD		wButton;
	WORD		wField1e;
	char		szText[256];
};
struct SCMDPACKET6D
{
	BYTE		bCmd;
	BYTE		bAdd;
	BYTE		bPetType;
	WORD		wClassId;
	DWORD		dwOwnerId;
	DWORD		dwUnitId;
};
struct SCMDPACKET74
{
	BYTE		bCmd;
	BYTE		bPetType;
	WORD		wClassId;
	DWORD		dwOwnerId;
	DWORD		dwUnitId;
	DWORD		dwSeed;
	DWORD		dwHireId;
};
struct SCMDPACKETLIFE
{
	BYTE		bCmd;
	BYTE		bIsPlayer;
	BYTE		bLife;
	DWORD		dwUnitId;
	WORD		wLevelId;
};
struct SCMDPACKET83
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	DWORD		dwX;
	DWORD		dwY;
};
struct SCMDPACKET41
{
	BYTE		bCmd;
	BYTE		bType;
	DWORD		dwUnitId;
	DWORD		field_06;
};
struct SCMDPACKETITEM
{
 	BYTE		bCmd;
 	BYTE		bField01;
 	BYTE		bBodyLoc;
 	BYTE		bMode;
 	BYTE		bPage;
 	BYTE		bModType;
 	BYTE		bOwnerType;
 	BYTE		bEarLevel;
 	WORD		wStat48;
 	WORD		wStat49;
 	DWORD		dwOwnerId;
 	DWORD		dwUnitId;
 	WORD		wClass;
 	WORD		wX;
 	WORD		wY;
 	DWORD		dwQuantity;
 	WORD		wUniqueOrSetRow;
 	WORD		wLevelId;
 	WORD		wMagicSuffix;
 	WORD		wMagicPrefix;
 	DWORD		dwFlags;
 	DWORD		dwInitSeed;
 	DWORD		dwSeed;
 	char		szName[16];
};
#pragma pack(pop)

DWORD __fastcall SCmdFillItem(SCMDPACKETITEM *ptPacket, UNIT *hItem);

#define NETQUEUE_SYSTEM			0
#define NETQUEUE_GAME			1

DWORD __fastcall SCmdSendSysGameEnd(DWORD dwClientId)
{
	SCMDPACKET1 tPacket;

	tPacket.bCmd = 1;
	return D2NetServerSend(NETQUEUE_SYSTEM, dwClientId, &tPacket, sizeof(tPacket));
}

DWORD __fastcall SCmdSendPumpEndNow(DWORD dwClientId)
{
	SCMDPACKETHDR tPacket;

	tPacket.wSize = 3;
	tPacket.bCmd = 6;
	return D2NetServerSend(NETQUEUE_GAME, dwClientId, &tPacket, 3);
}

DWORD __fastcall SCmdSendJoinError(DWORD dwClientId, DWORD dwError)
{
	SCMDPACKET5 tPacket;

	tPacket.bCmd = 5;
	tPacket.dwValue = dwError;
	return D2NetServerSend(NETQUEUE_SYSTEM, dwClientId, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendCmd(CLIENT *ptClient, BYTE bCmd)
{
	SCMDPACKET1 tPacket;

	tPacket.bCmd = bCmd;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

int __fastcall SCmdQueue(CLIENT *ptClient, const void *pData, int nSize)
{
	CLIENTNODE178 *ptNode;
	int nUsed;
	int nRecord;
	DWORD i;

	nUsed = 0;
	ptNode = ClientGetQueueTail(ptClient);
	if (ptNode)
		nUsed = ptNode->nUsed;
	nRecord = nSize + 2;
	if (nRecord + nUsed > SCMD_NODESIZE || !ptNode)
	{
		ptNode = ptClient->ptFreeNodes;
		nUsed = 0;
		if (ptNode)
			ptClient->ptFreeNodes = ptNode->ptNext;
		else
		{
#line 132
			ptNode = (CLIENTNODE178 *)D2ALLOCPOOL(NULL, sizeof(CLIENTNODE178));
		}
		for (i = 0; i < sizeof(CLIENTNODE178) / sizeof(DWORD); i++)
			((DWORD *)ptNode)[i] = 0;
		ClientQueueAppend(ptClient, ptNode);
	}
	((GAMEQUEUEDRECORD *)&ptNode->abData[nUsed])->wSize = (WORD)nRecord;
	memcpy(&ptNode->abData[nUsed + 2], pData, nSize);
	ptNode->nUsed += nRecord;
	return ptNode->nUsed;
}

void __fastcall SCmdSendGameFlags(CLIENT *ptClient, BYTE bCmd, GAME *ptGame)
{
	SCMDPACKET6 tPacket;

	tPacket.bCmd = bCmd;
	tPacket.dwField02 = ArenaGetFlags(ptGame);
	tPacket.bField01 = ptGame->bDifficultyLevel;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendLoadAct(CLIENT *ptClient, BYTE bCmd, BYTE bAct,
	DWORD dwSeed, DWORD dwObjectSeed, WORD wLevelId)
{
	SCMDPACKETB850 tPacket;

	tPacket.bCmd = bCmd;
	tPacket.bAct = bAct;
	tPacket.dwSeed = dwSeed;
	tPacket.dwObjectSeed = dwObjectSeed;
	tPacket.wLevelId = wLevelId;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendUnitTypeId(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId)
{
	SCMDPACKET6 tPacket;

	tPacket.bCmd = bCmd;
	tPacket.bField01 = bType;
	tPacket.dwField02 = dwUnitId;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendDword(CLIENT *ptClient, BYTE bCmd, DWORD dwValue)
{
	SCMDPACKET5 tPacket;

	tPacket.bCmd = bCmd;
	tPacket.dwValue = dwValue;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendUnitMode(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, BYTE bMode, BYTE bHitClass,
	BYTE bLife)
{
	SCMDPACKETB8E0 tPacket;

	tPacket.bCmd = bCmd;
	tPacket.bType = bType;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bMode = bMode;
	tPacket.bField07 = bHitClass;
	tPacket.bField08 = bLife;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendUnitModeValue(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, BYTE bMode, BYTE bField07,
	DWORD dwObjectMode)
{
	SCMDPACKETB920 tPacket;

	tPacket.bCmd = bCmd;
	tPacket.bType = bType;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bField06 = bMode;
	tPacket.bField07 = bField07;
	tPacket.dwField08 = dwObjectMode;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendUnitModeAt(CLIENT *ptClient, BYTE bCmd, BYTE bTOU, DWORD dwUnitId,
	BYTE bMode, WORD wX, WORD wY,
	BYTE bHitClass, BYTE bLife)
{
	SCMDPACKETB960 tPacket;

	tPacket.bCmd = bCmd;
	tPacket.bTOU = bTOU;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bMode = bMode;
	tPacket.wField07 = wX;
	tPacket.wField09 = wY;
	tPacket.bField0b = bHitClass;
	tPacket.bLife = bLife;
#line 266
	D2ASSERT(bTOU < NUM_UNITTYPE);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendUnitModeToUnit(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, BYTE bMode, BYTE bTargetType,
	DWORD dwTargetId, WORD wX, WORD wY)
{
	SCMDPACKETB9E0 tPacket;

	tPacket.bCmd = bCmd;
	tPacket.bType = bType;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bMode = bMode;
	tPacket.bField07 = bTargetType;
	tPacket.dwField08 = dwTargetId;
	tPacket.wX = wX;
	tPacket.wY = wY;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendUnitModeToPoint(CLIENT *ptClient, BYTE bCmd, BYTE bTOU,
	DWORD dwUnitId,
	BYTE bMode, WORD wTargetX, WORD wTargetY,
	BYTE bHitClass, WORD wX, WORD wY)
{
	SCMDPACKETBA40 tPacket;

	tPacket.bCmd = bCmd;
	tPacket.bTOU = bTOU;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bMode = bMode;
	tPacket.wField07 = wTargetX;
	tPacket.wField09 = wTargetY;
	tPacket.bField0b = bHitClass;
	tPacket.wX = wX;
	tPacket.wY = wY;
#line 314
	D2ASSERT(bTOU < NUM_UNITTYPE);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendMonsterMove(CLIENT *ptClient, BYTE bCmd, DWORD dwUnitId,
	BYTE bMode, BYTE bTargetType, DWORD dwTargetId,
	BYTE bStopDist)
{
	SCMDPACKETMOVE20 tPacket;
	DWORD i;
	UNIT *hUnit;
	void *ptPath;
	int nVel;

	for (i = 0; i < sizeof(tPacket) / sizeof(DWORD); i++)
		((DWORD *)&tPacket)[i] = 0;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bCmd = bCmd;
	tPacket.bMode = bMode;
	tPacket.bTargetType = bTargetType;
	tPacket.dwTargetId = dwTargetId;
	hUnit = SUnitFind(ClientGetGame(ptClient), UNIT_MONSTER, dwUnitId);
	ptPath = hUnit->ptPath;
	tPacket.wX = (WORD)D2CommonUnitGetX(hUnit);
	tPacket.wY = (WORD)D2CommonUnitGetY(hUnit);
	tPacket.bField0f = bStopDist;
	tPacket.bPathType = (BYTE)D2CommonPathGetType(ptPath);
	if (tPacket.bPathType >= 5)
	{
		if (tPacket.bPathType > 6)
		{
			if (tPacket.bPathType == 8)
				tPacket.bPathType = 11;
		}
		else
			tPacket.bPathType = 2;
	}
	nVel = (int)MonsterModeDataGetVelocity(((MONSTERDATA *)hUnit->uTypeData)->ptModeData);
#line 357
	D2ASSERT(nVel <= 255);
	tPacket.bVelocity = (BYTE)nVel;
	tPacket.bField13 = D2CommonPathGetDistMax(ptPath);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendMonsterMoveTo(CLIENT *ptClient, BYTE bCmd, DWORD dwUnitId,
	BYTE bMode, WORD wX, WORD wY,
	BYTE bStopDist)
{
	SCMDPACKETMOVE15 tPacket;
	UNIT *hUnit;
	void *ptPath;
	int nVel;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.dwUnitId = dwUnitId;
	tPacket.bMode = bMode;
	tPacket.bCmd = bCmd;
	tPacket.wX = wX;
	tPacket.wY = wY;
	hUnit = SUnitFind(ClientGetGame(ptClient), UNIT_MONSTER, dwUnitId);
	ptPath = hUnit->ptPath;
	tPacket.bField0a = bStopDist;
	tPacket.bPathType = (BYTE)D2CommonPathGetType(ptPath);
	if (tPacket.bPathType >= 5)
	{
		if (tPacket.bPathType > 6)
		{
			if (tPacket.bPathType == 8)
				tPacket.bPathType = 11;
		}
		else
		{
			tPacket.bPathType = 1;
			tPacket.wX = D2CommonPathGetStepX(ptPath);
			tPacket.wY = D2CommonPathGetStepY(ptPath);
		}
	}
	nVel = (int)MonsterModeDataGetVelocity(((MONSTERDATA *)hUnit->uTypeData)->ptModeData);
#line 407
	D2ASSERT(nVel <= 255);
	tPacket.bVelocity = (BYTE)nVel;
	tPacket.bField0e = D2CommonPathGetDistMax(ptPath);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendMonsterKnockback(CLIENT *ptClient, BYTE bCmd, DWORD dwUnitId,
	BYTE bMode, BYTE bTargetType, DWORD dwTargetId,
	BYTE bDistLeft, BYTE bLife, BYTE bHitClass)
{
	SCMDPACKETMOVE20 tPacket;
	DWORD i;
	UNIT *hUnit;
	void *ptPath;
	int nVel;

	for (i = 0; i < sizeof(tPacket) / sizeof(DWORD); i++)
		((DWORD *)&tPacket)[i] = 0;
	tPacket.bMode = bMode;
	tPacket.bCmd = bCmd;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bTargetType = bTargetType;
	tPacket.dwTargetId = dwTargetId;
	hUnit = SUnitFind(ClientGetGame(ptClient), UNIT_MONSTER, dwUnitId);
	ptPath = hUnit->ptPath;
	tPacket.wX = (WORD)D2CommonUnitGetX(hUnit);
	tPacket.wY = (WORD)D2CommonUnitGetY(hUnit);
	tPacket.bField0f = bDistLeft;
	tPacket.bHitClass = bHitClass;
	tPacket.bPathType = (BYTE)D2CommonPathGetType(ptPath);
	if (tPacket.bPathType == 8)
		tPacket.bPathType = 11;
	nVel = (int)MonsterModeDataGetVelocity(((MONSTERDATA *)hUnit->uTypeData)->ptModeData);
#line 451
	D2ASSERT(nVel <= 255);
	tPacket.bField13 = bLife;
	tPacket.bVelocity = (BYTE)nVel;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendMonsterKnockbackTo(CLIENT *ptClient, BYTE bCmd, DWORD dwUnitId,
	BYTE bMode, WORD wX, WORD wY,
	BYTE bDistLeft, BYTE bLife, BYTE bHitClass)
{
	SCMDPACKETMOVE15 tPacket;
	UNIT *hUnit;
	void *ptPath;
	int nVel;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bMode = bMode;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bCmd = bCmd;
	tPacket.wX = wX;
	tPacket.wY = wY;
	hUnit = SUnitFind(ClientGetGame(ptClient), UNIT_MONSTER, dwUnitId);
	ptPath = hUnit->ptPath;
	tPacket.bField0a = bDistLeft;
	tPacket.bHitClass = bHitClass;
	tPacket.bPathType = (BYTE)D2CommonPathGetType(ptPath);
	if (tPacket.bPathType >= 5)
	{
		if (tPacket.bPathType > 6)
		{
			if (tPacket.bPathType == 8)
				tPacket.bPathType = 11;
		}
		else
		{
			tPacket.bPathType = 1;
			tPacket.wX = D2CommonPathGetStepX(ptPath);
			tPacket.wY = D2CommonPathGetStepY(ptPath);
		}
	}
	nVel = (int)MonsterModeDataGetVelocity(((MONSTERDATA *)hUnit->uTypeData)->ptModeData);
#line 504
	D2ASSERT(nVel <= 255);
	tPacket.bField0e = bLife;
	tPacket.bVelocity = (BYTE)nVel;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendMonsterInPlace(CLIENT *ptClient, BYTE bCmd, DWORD dwUnitId,
	BYTE bMode, BYTE bTargetType, DWORD dwTargetId,
	BYTE bField0b)
{
	SCMDPACKETBEB0 tPacket;
	DWORD i;

	for (i = 0; i < sizeof(tPacket) / sizeof(DWORD); i++)
		((DWORD *)&tPacket)[i] = 0;
	tPacket.bCmd = bCmd;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bMode = bMode;
	tPacket.bTargetType = bTargetType;
	tPacket.dwTargetId = dwTargetId;
	tPacket.bField0b = bField0b;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendMonsterInPlaceTo(CLIENT *ptClient, BYTE bCmd, DWORD dwUnitId,
	BYTE bMode, WORD wX, WORD wY,
	BYTE bField0a, BYTE bHitClass)
{
	SCMDPACKETBF00 tPacket;
	DWORD i;

	for (i = 0; i < sizeof(tPacket) / sizeof(DWORD); i++)
		((DWORD *)&tPacket)[i] = 0;
	tPacket.bCmd = bCmd;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bMode = bMode;
	tPacket.wTargetX = wX;
	tPacket.wTargetY = wY;
	tPacket.bField0a = bField0a;
	tPacket.bHitClass = bHitClass;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendMonsterAttack(CLIENT *ptClient, BYTE bCmd, DWORD dwUnitId,
	BYTE bMode, BYTE bTargetType, DWORD dwTargetId,
	BYTE bField0b, WORD wX, WORD wY)
{
	SCMDPACKETBF60 tPacket;
	DWORD i;

	for (i = 0; i < sizeof(tPacket) / sizeof(DWORD); i++)
		((DWORD *)&tPacket)[i] = 0;
	tPacket.bCmd = bCmd;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bMode = bMode;
	tPacket.bTargetType = bTargetType;
	tPacket.dwTargetId = dwTargetId;
	tPacket.bField0b = bField0b;
	tPacket.wX = wX;
	tPacket.wY = wY;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendMonsterAttackTo(CLIENT *ptClient, BYTE bCmd, DWORD dwUnitId,
	BYTE bMode, WORD wTargetX, WORD wTargetY,
	BYTE bField0a, BYTE bHitClass, WORD wX,
	WORD wY)
{
	SCMDPACKETBFD0 tPacket;
	DWORD i;

	for (i = 0; i < sizeof(tPacket) / sizeof(DWORD); i++)
		((DWORD *)&tPacket)[i] = 0;
	tPacket.bCmd = bCmd;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bMode = bMode;
	tPacket.wTargetX = wTargetX;
	tPacket.wTargetY = wTargetY;
	tPacket.bField0a = bField0a;
	tPacket.bHitClass = bHitClass;
	tPacket.wX = wX;
	tPacket.wY = wY;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendUnit(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, WORD wX, WORD wY,
	BYTE bField0a)
{
	SCMDPACKETUNIT tPacket;

	tPacket.bCmd = bCmd;
	tPacket.bType = bType;
	tPacket.dwUnitId = dwUnitId;
	tPacket.wX = wX;
	tPacket.wY = wY;
	tPacket.bField0a = bField0a;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendRoom(CLIENT *ptClient, DWORD bLevelId, WORD wX,
	WORD wY)
{
	SCMDPACKETROOM tPacket;

	tPacket.wX = wX;
	tPacket.bCmd = 7;
	tPacket.wY = wY;
	tPacket.bLevelId = (BYTE)bLevelId;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendRoomLeave(CLIENT *ptClient, DWORD bLevelId, WORD wX,
	WORD wY)
{
	SCMDPACKETROOM tPacket;

	tPacket.wX = wX;
	tPacket.bCmd = 8;
	tPacket.wY = wY;
	tPacket.bLevelId = (BYTE)bLevelId;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendUnitClass(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, BYTE bClassId, WORD wX,
	WORD wY)
{
	SCMDPACKETC120 tPacket;

	tPacket.bCmd = bCmd;
	tPacket.bType = bType;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bClassId = bClassId;
	tPacket.wX = wX;
	tPacket.wY = wY;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendPlayerWithInfo(CLIENT *ptClient, DWORD dwUnitId,
	BYTE bClassId,
	const char *szName, WORD wX, WORD wY)
{
	SCMDPACKET52 tPacket;
	SCMDPACKET68 tPlayer;
	UNIT *hPlayer;
	UNIT *hUnit;
	CLIENT *ptPlayerClient;

	tPacket.bCmd = S2C_PLAYER_SPAWN;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bClassId = bClassId;
	StormSStrCopy(tPacket.szName, szName, 2147483647);
	tPacket.wX = wX;
	tPacket.wY = wY;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	hPlayer = ClientGetPlayerUnit(ptClient, FALSE);
	hUnit = SUnitFind(ClientGetGame(ptClient), UNIT_PLAYER, dwUnitId);
	if (!(hUnit == 0))
	{ SCMDPACKET68 tPlayer;
		ptPlayerClient = SUnitGetClient(hPlayer);
		memset(&tPlayer, 0, sizeof(tPlayer));
		tPlayer.bCmd = S2C_PARTY_UPDATE;
		tPlayer.dwUnitId = hUnit->dwId;
		tPlayer.wLevel = (WORD)D2CommonStatsUnitGetValue(hUnit, 12);
		tPlayer.wParty = (WORD)SUnitGetParty(hUnit);
		tPlayer.wFlags = (WORD)PlayerListGetFlags(hPlayer, hUnit);
		tPlayer.wButton = (WORD)PlayerListGetState(hPlayer, hUnit);
		SCmdQueue(ptPlayerClient, &tPlayer, sizeof(tPlayer)); }
}

void __fastcall SCmdSendObjectAt(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, WORD wClassId, WORD wX,
	WORD wY, BYTE bMode, BYTE bField0d)
{
	SCMDPACKETC260 tPacket;
	UNIT *hUnit;

	hUnit = SUnitFind(ClientGetGame(ptClient), bType, dwUnitId);
#line 731
	D2ASSERT(hUnit);
	tPacket.bType = bType;
	tPacket.wClassId = wClassId;
	tPacket.wX = wX;
	tPacket.bCmd = bCmd;
	tPacket.dwUnitId = dwUnitId;
	tPacket.wY = wY;
	tPacket.bMode = bMode;
	tPacket.bField0d = bField0d;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendUnitRemove(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId)
{
	SCMDPACKET6 tPacket;

	if (bType == UNIT_MISSILE)
		return;
	tPacket.bField01 = bType;
	tPacket.bCmd = bCmd;
	tPacket.dwField02 = dwUnitId;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendStat(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, BYTE bField06, BYTE bStat,
	DWORD dwValue)
{
	SCMDPACKETB920 tPacket;

	tPacket.bCmd = bCmd;
	tPacket.bType = bType;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bField06 = bField06;
	tPacket.bField07 = bStat;
	tPacket.dwField08 = dwValue;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendPlayerStatsPacked(CLIENT *ptClient, DWORD dwUnitId,
	DWORD dwHitpoints, DWORD dwMana, DWORD dwStamina,
	BYTE bHpRegen, BYTE bManaRecovery, WORD wX,
	WORD wY, WORD wTargetX, WORD wTargetY)
{
	BITBUFFER tBits;
	BYTE abPacket[21 + 3];

	FogBitManipInitBuffer(&tBits, abPacket, 21);
	FogBitManipWrite(&tBits, 24, 8);
	FogBitManipWrite(&tBits, dwUnitId, 16);
	FogBitManipWrite(&tBits, dwHitpoints, 21);
	FogBitManipWrite(&tBits, dwMana, 21);
	FogBitManipWrite(&tBits, dwStamina, 21);
	FogBitManipWrite(&tBits, bHpRegen, 7);
	FogBitManipWrite(&tBits, bManaRecovery, 7);
	FogBitManipWrite(&tBits, wX, 16);
	FogBitManipWrite(&tBits, wY, 16);
	FogBitManipWrite(&tBits, wTargetX, 16);
	FogBitManipWrite(&tBits, wTargetY, 16);
	SCmdQueue(ptClient, abPacket, FogBitManipGetBytesUsed(&tBits));
}

void __fastcall SCmdSendSkill(UNIT *hUnit, WORD wSkill, BYTE bSkillLevel,
	BYTE bRemove)
{
	SCMDPACKET1A tPacket;

	tPacket.wSkill = wSkill;
	tPacket.bType = (BYTE)hUnit->eType;
	tPacket.bRemove = bRemove;
	tPacket.bCmd = 26;
	tPacket.dwUnitId = hUnit->dwId;
	tPacket.bField09 = bSkillLevel;
	tPacket.bBonus = (BYTE)D2CommonSkillsGetBonusByKey(hUnit, wSkill);
	SCmdQueue(SUnitGetClient(hUnit), &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendSkillState(CLIENT *ptClient, BYTE bType,
	DWORD dwUnitId, WORD wSkill, BYTE bCount)
{
	SCMDPACKET1A tPacket;
	UNIT *hUnit;

	tPacket.bType = bType;
	tPacket.bCmd = S2C_SKILL_UPDATE;
	tPacket.dwUnitId = dwUnitId;
	tPacket.wSkill = wSkill;
	tPacket.bField09 = bCount;
	hUnit = SUnitFind(ClientGetGame(ptClient), bType, dwUnitId);
	if (hUnit && D2CommonStatsGetState(hUnit, 7))
		tPacket.bField0b = 1;
	else
		tPacket.bField0b = 0;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendCurrentSkill(CLIENT *ptClient, BYTE bType,
	DWORD dwUnitId, BYTE bLeft, WORD wSkillId)
{
	SCMDPACKET1C tPacket;

	tPacket.bType = bType;
	tPacket.bLeft = bLeft;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bCmd = S2C_SKILL_TOGGLE;
	tPacket.wSkillId = wSkillId;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendChat(CLIENT *ptClient, SCMDCHATDATA *pChatData)
{
#line 968
	D2ASSERT(strlen((char *)pChatData->bCustomText) < CHAT_MAX_CHAR_LENGTH);
	if (pChatData->bCustomText[CHAT_MAX_CHAR_LENGTH - 1])
		pChatData->bCustomText[CHAT_MAX_CHAR_LENGTH - 1] = 0;
	SCmdQueue(ptClient, pChatData, sizeof(SCMDCHATDATA));
}

void __fastcall SCmdSendSystemMsg(CLIENT *ptClient, const SCMDSYSTEMMSG *pSystemMsg)
{
	SCMDSYSTEMMSG tMsg;

#line 985
	D2ASSERT(strlen((char*)pSystemMsg->bPlayerName) < MAX_PLR_NAME_LEN);
	tMsg = *pSystemMsg;
	SCmdQueue(ptClient, &tMsg, sizeof(tMsg));
}

void __fastcall SCmdSendUnitMessage(CLIENT *ptClient, const SCMDSYSTEMMSG *pSystemMsg)
{
	SCMDSYSTEMMSG tMsg;

	tMsg = *pSystemMsg;
	SCmdQueue(ptClient, &tMsg, sizeof(tMsg));
}

void __fastcall SCmdSendEnv(CLIENT *ptClient, SCMDENVPACKET *ptPacket)
{
	SCmdQueue(ptClient, ptPacket, sizeof(SCMDENVPACKET));
}

void __fastcall SCmdSendPlayer(CLIENT *ptClient, UNIT *hUnit, CLIENT *ptOwner)
{
	SCMDPACKET54 tPacket;
	PLRINFO *ptPlrInfo;
	char *pszNext;
	DWORD i;

	ptPlrInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hUnit);
	for (i = 0; i < sizeof(tPacket) / sizeof(DWORD); i++)
		((DWORD *)&tPacket)[i] = 0;
	tPacket.bCmd = S2C_PLAYER_LIST_ENTRY;
	tPacket.dwUnitId = hUnit->dwId;
	tPacket.bClass = (BYTE)hUnit->dwClassId;
	tPacket.wLevel = (WORD)D2CommonStatsUnitGetValue(hUnit, 12);
	tPacket.wParty = (WORD)SUnitGetParty(hUnit);
	tPacket.wField1a = 0;
	tPacket.wButton = 0;
	tPacket.wField1e = ClientGetGuildField(ptOwner);
	StormSStrCopy(tPacket.szName, ptPlrInfo, 2147483647);
	ClientGetGuildTag(ptOwner, (DWORD *)tPacket.szText);
	pszNext = &tPacket.szText[strlen(tPacket.szText) + 1];
	ClientGetGuildName(ptOwner, pszNext);
	SCmdQueue(ptClient, &tPacket, strlen(pszNext) + (pszNext - tPacket.szText) + 33);
	if (hUnit->eType == UNIT_PLAYER)
		PlayerPetsSendAll(ptClient, hUnit);
}

void __fastcall SCmdSendRosterRemove(CLIENT *ptClient, DWORD dwValue)
{
	SCMDPACKET5 tPacket;

	tPacket.dwValue = dwValue;
	tPacket.bCmd = S2C_ROSTER_REMOVE;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendTradeEvent(CLIENT *ptClient, BYTE bValue)
{
	BYTE abPacket[2];

	abPacket[1] = bValue;
	abPacket[0] = 106;
	SCmdQueue(ptClient, abPacket, sizeof(abPacket));
}

void __fastcall SCmdSendTradeName(CLIENT *ptClient, const void *pPacket)
{
	SCmdQueue(ptClient, pPacket, 21);
}

void __fastcall SCmdSendPet(CLIENT *ptClient, BYTE bAdd, DWORD bPetType,
	DWORD dwPetId, DWORD dwOwnerId, WORD wClassId)
{
	SCMDPACKET6D tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bAdd = bAdd;
	tPacket.bPetType = (BYTE)bPetType;
	tPacket.wClassId = wClassId;
	tPacket.dwUnitId = dwPetId;
	tPacket.bCmd = 109;
	tPacket.dwOwnerId = dwOwnerId;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendHireling(CLIENT *ptClient, BYTE bUnused,
	DWORD bPetType,
	DWORD dwPetId, DWORD dwOwnerId, WORD wClassId,
	DWORD dwSeed, DWORD dwHireId)
{
	SCMDPACKET74 tPacket;
	DWORD i;

	for (i = 0; i < sizeof(tPacket) / sizeof(DWORD); i++)
		((DWORD *)&tPacket)[i] = 0;
	tPacket.bPetType = (BYTE)bPetType;
	tPacket.dwUnitId = dwPetId;
	tPacket.dwOwnerId = dwOwnerId;
	tPacket.wClassId = wClassId;
	tPacket.dwSeed = dwSeed;
	tPacket.bCmd = 116;
	tPacket.dwHireId = dwHireId;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendTradeGold(CLIENT *ptClient, DWORD dwGold,
	BYTE bOwnOffer)
{
	SCMDPACKET6 tPacket;

	tPacket.dwField02 = dwGold;
	tPacket.bCmd = 108;
	tPacket.bField01 = bOwnOffer;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

static inline BOOL SCmdLifeChanged(HPTRACK *ptTrack, int nLife)
{
	int nDiff;

	nDiff = nLife - ptTrack->nLife;
	if (!nDiff)
		return FALSE;
	if (nDiff < 0)
		nDiff = -nDiff;
	if (nDiff > 8)
	{
		ptTrack->bSkipped = 0;
		ptTrack->nLife = nLife;
		return TRUE;
	}
	ptTrack->bSkipped++;
	if (ptTrack->bSkipped > 6)
	{
		ptTrack->bSkipped = 0;
		ptTrack->nLife = nLife;
		return TRUE;
	}
	return FALSE;
}

static inline BOOL SCmdPartyLifeChanged(UNIT *hPlayer, UNIT *hUnit, int nLife,
	int nLevelId)
{
	GAME *ptGame;
	WORD wParty;
	PLRINFO *ptPlrInfo;
	HPTRACK *ptTrack;
	DWORD dwUnitId;
	DWORD i;

	ptGame = SUnitGetGame(hPlayer);
	if (!ptGame)
		return FALSE;
	wParty = PartyGetId(ptGame, hPlayer);
	if (wParty == 65535 || wParty != PartyGetId(ptGame, hUnit))
		return FALSE;
	ptPlrInfo = (PLRINFO *)D2CommonUnitGetPlrInfo(hPlayer);
	if (!ptPlrInfo)
		return FALSE;
	ptTrack = ptPlrInfo->ptHpTrack;
	dwUnitId = hUnit->dwId;
	while (1)
	{
		if (!ptTrack)
			break;
		if (dwUnitId == ptTrack->dwUnitId)
			goto lFound;
		ptTrack = ptTrack->ptNext;
	}
#line 1264
	ptTrack = (HPTRACK *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(HPTRACK));
	if (!ptTrack)
		return FALSE;
	for (i = 0; i < sizeof(HPTRACK) / sizeof(DWORD); i++)
		((DWORD *)ptTrack)[i] = 0;
	ptTrack->ptNext = ptPlrInfo->ptHpTrack;
	ptPlrInfo->ptHpTrack = ptTrack;
	ptTrack->dwUnitId = dwUnitId;
lFound:
	if (ptTrack->nLevelId != nLevelId)
	{
		ptTrack->nLife = nLife;
		ptTrack->nLevelId = nLevelId;
		return TRUE;
	}
	return SCmdLifeChanged(ptTrack, nLife);
}

static inline BOOL SCmdPetLifeChanged(UNIT *hPlayer, UNIT *hUnit, int nLife)
{
	MONSTERDATA *ptMonsterData;
	HPTRACK *ptTrack;

	ptMonsterData = (MONSTERDATA *)hUnit->uTypeData;
	if (AiGetOwner(hUnit) != hPlayer || !ptMonsterData)
		return FALSE;
	ptTrack = MonsterGetHpTrack(ptMonsterData);
	if (!ptTrack || ptTrack->nLife == nLife)
		return FALSE;
	return SCmdLifeChanged(ptTrack, nLife);
}

static inline BOOL SCmdUnitLifeChanged(CLIENT *ptClient, UNIT *hUnit, int nLife,
	int nLevelId)
{
	UNIT *hPlayer;
	MONSTERDATA *ptMonsterData;
	HPTRACK *ptTrack;

	hPlayer = ClientGetPlayerUnit(ptClient, FALSE);
#line 1288
	D2ASSERT(hPlayer);
	switch (hUnit->eType)
	{
	case UNIT_PLAYER:
		return SCmdPartyLifeChanged(hPlayer, hUnit, nLife, nLevelId);
	case UNIT_MONSTER:
		return SCmdPetLifeChanged(hPlayer, hUnit, nLife);
	}
	return FALSE;
}

void __fastcall SCmdSendLife(CLIENT *ptClient, UNIT *hUnit)
{
	SCMDPACKETLIFE tPacket;
	SCMDPACKET83 tPosition;
	int nLife;
	int nMaxLife;
	UNIT *hPlayer;

#line 1313
	D2ASSERT(hUnit);
	nLife = D2CommonStatsUnitGetValue(hUnit, 6);
	nMaxLife = D2CommonStatsUnitCalcMaxHp(hUnit);
	if (nMaxLife)
		nLife = nLife * 100 / nMaxLife;
	else
		nLife = 0;
	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 114;
	if (hUnit->eType == UNIT_PLAYER)
		tPacket.bIsPlayer = TRUE;
	tPacket.dwUnitId = hUnit->dwId;
	tPacket.bLife = (BYTE)nLife;
	tPacket.wLevelId = (WORD)D2CommonDungeonRoomGetLevelId(D2CommonUnitGetPathRoom(hUnit));
	if (SCmdUnitLifeChanged(ptClient, hUnit, nLife, tPacket.wLevelId))
		SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	if (!tPacket.bIsPlayer)
		return;
	hPlayer = ClientGetPlayerUnit(ptClient, FALSE);
	if (D2CommonUnitGetProximityMetric(hPlayer, hUnit) > 50)
	{
		memset(&tPosition, 0, sizeof(tPosition));
		tPosition.bCmd = 131;
		tPosition.dwUnitId = hUnit->dwId;
		tPosition.dwX = D2CommonUnitGetX(hUnit);
		tPosition.dwY = D2CommonUnitGetY(hUnit);
		SCmdQueue(ptClient, &tPosition, sizeof(tPosition));
	}
}

static inline void SCmdSendSocketedItem(CLIENT *ptClient, UNIT *hItem, UNIT *hSocketed, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bOwnerType = (BYTE)hItem->eType;
	tPacket.dwOwnerId = hItem->dwId;
	tPacket.dwUnitId = hSocketed->dwId;
	tPacket.bCmd = 61;
	tPacket.wClass = (WORD)hSocketed->dwClassId;
	tPacket.dwInitSeed = hSocketed->dwInitSeed;
	tPacket.bMode = (BYTE)hSocketed->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hSocketed) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hSocketed);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

static inline BOOL SCmdSendSocketed(CLIENT *ptClient, UNIT *hItem, DWORD dwFlags)
{
	void *pNode;
	UNIT *hSocketed;
	void *pInventory;

	if (!D2CommonITEMSGetTypeHasInv(hItem) || D2CommonITEMSMaskFlags(dwFlags, 32))
		return FALSE;
	pInventory = hItem->ptInventory;
	for (pNode = D2CommonINVENTORYGetCategoryListHead(pInventory); pNode;
		pNode = (void *)D2CommonINVENTORYNodeGetNext(pNode))
	{
		hSocketed = (UNIT *)D2CommonINVENTORYNodeGetKeyA(pNode);
		D2CommonITEMSSetOrClearFlags(&dwFlags, 8, TRUE);
		SCmdSendSocketedItem(ptClient, hItem, hSocketed, dwFlags);
	}
	return TRUE;
}

static inline void SCmdSendItemOwner(CLIENT *ptClient, UNIT *hOwner)
{
	SCMDPACKET41 tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = S2C_INVENTORY_FLAG_SWEEP_LOCAL;
	tPacket.bType = (BYTE)hOwner->eType;
	tPacket.dwUnitId = hOwner->dwId;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendItemNoOwner(CLIENT *ptClient, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;
	BYTE bMode;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 38;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = (WORD)D2CommonUnitGetX(hItem);
	tPacket.wY = (WORD)D2CommonUnitGetY(hItem);
	bMode = (BYTE)hItem->dwMode;
	if (bMode == 3 && D2CommonITEMSDataTestFlags(hItem, 8192))
		bMode = 5;
	tPacket.bMode = bMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	SCmdSendSocketed(ptClient, hItem, dwFlags);
}

DWORD __fastcall SCmdFillItem(SCMDPACKETITEM *ptPacket, UNIT *hItem)
{
	if (D2CommonITEMSGetType(hItem) == 4)
	{
		ptPacket->dwQuantity = D2CommonStatsUnitGetValue(hItem, 14);
		ptPacket->bModType = (BYTE)D2CommonITEMSDataGetModType(hItem);
	}
	else if (D2CommonITEMSGetType(hItem) == 5 || D2CommonITEMSGetType(hItem) == 6)
	{
		ptPacket->dwQuantity = D2CommonStatsUnitGetValue(hItem, 70);
		ptPacket->bModType = (BYTE)D2CommonITEMSDataGetModType(hItem);
	}
	else
	{
		ptPacket->dwQuantity = D2CommonStatsUnitGetValue(hItem, 70);
		ptPacket->wStat49 = (WORD)D2CommonStatsUnitGetBaseValue(hItem, 73);
		ptPacket->wStat48 = (WORD)D2CommonStatsUnitGetValue(hItem, 72);
		ptPacket->bModType = (BYTE)D2CommonITEMSDataGetModType(hItem);
		ptPacket->wUniqueOrSetRow = (WORD)D2CommonITEMSDataGetUniqueOrSetRow(hItem);
		ptPacket->wMagicPrefix = (WORD)D2CommonITEMSDataGetMagicPrefix(hItem);
		ptPacket->wMagicSuffix = (WORD)D2CommonITEMSDataGetMagicSuffix(hItem);
	}
	ptPacket->bPage = D2CommonITEMSDataGetPage(hItem);
	if (D2CommonITEMSDataTestFlags(hItem, 16))
	{
		ptPacket->dwSeed = D2CommonITEMSGetItemSeedValue(hItem);
		ptPacket->wLevelId = (WORD)D2CommonITEMSGetLevelId(hItem);
	}
	else
		ptPacket->dwSeed = 0;
	if (D2CommonITEMSGetType(hItem) == 7)
	{
		strcpy(ptPacket->szName, D2CommonITEMSGetNameBuffer(hItem));
		ptPacket->bEarLevel = D2CommonITEMSDataGetEarLevel(hItem);
	}
	return 0;
}

void __fastcall SCmdSendItemToCursor(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 39;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = 0;
	tPacket.wY = 0;
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	SCmdSendSocketed(ptClient, hItem, dwFlags);
}

void __fastcall SCmdSendItemDropped(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 40;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.bField01 = D2CommonUnitTestEquipSlotConflict(hOwner, hItem);
	tPacket.bBodyLoc = D2CommonITEMSGetCategory(hItem);
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = (WORD)D2CommonUnitGetX(hItem);
	tPacket.wY = (WORD)D2CommonUnitGetY(hItem);
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	SCmdSendSocketed(ptClient, hItem, dwFlags);
}

void __fastcall SCmdSendItemOnGround(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 41;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = (WORD)D2CommonUnitGetX(hItem);
	tPacket.wY = (WORD)D2CommonUnitGetY(hItem);
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	SCmdSendSocketed(ptClient, hItem, dwFlags);
}

void __fastcall SCmdSendItemToGrid(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = S2C_ITEM_EVENT;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = (WORD)D2CommonUnitGetX(hItem);
	tPacket.wY = (WORD)D2CommonUnitGetY(hItem);
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	SCmdSendSocketed(ptClient, hItem, dwFlags);
}

void __fastcall SCmdSendItemFromGrid(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 43;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = (WORD)D2CommonUnitGetX(hItem);
	tPacket.wY = (WORD)D2CommonUnitGetY(hItem);
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	tPacket.bPage = D2CommonITEMSDataGetPreviousPage(hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	SCmdSendSocketed(ptClient, hItem, dwFlags);
}

void __fastcall SCmdSendItemEquip(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = S2C_ITEM_EVENT_2C;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.bField01 = D2CommonUnitTestEquipSlotConflict(hOwner, hItem);
	tPacket.bBodyLoc = D2CommonITEMSGetCategory(hItem);
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = (WORD)D2CommonUnitGetX(hItem);
	tPacket.wY = (WORD)D2CommonUnitGetY(hItem);
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	if (SCmdSendSocketed(ptClient, hItem, dwFlags))
		SCmdSendItemOwner(ptClient, hOwner);
}

void __fastcall SCmdSendItemSwapHands(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 45;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.bField01 = D2CommonUnitTestEquipSlotConflict(hOwner, hItem);
	tPacket.bBodyLoc = D2CommonITEMSGetCategory(hItem);
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = (WORD)D2CommonUnitGetX(hItem);
	tPacket.wY = (WORD)D2CommonUnitGetY(hItem);
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	if (SCmdSendSocketed(ptClient, hItem, dwFlags))
		SCmdSendItemOwner(ptClient, hOwner);
}

void __fastcall SCmdSendItemUnequip(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 46;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.bBodyLoc = D2CommonITEMSGetCategory(hItem);
	tPacket.bField01 = D2CommonUnitTestEquipSlotConflict(hOwner, hItem);
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = (WORD)D2CommonUnitGetX(hItem);
	tPacket.wY = (WORD)D2CommonUnitGetY(hItem);
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	if (SCmdSendSocketed(ptClient, hItem, dwFlags))
		SCmdSendItemOwner(ptClient, hOwner);
}

void __fastcall SCmdSendItemSwapBodyLoc(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 47;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.bField01 = D2CommonUnitTestEquipSlotConflict(hOwner, hItem);
	tPacket.bBodyLoc = D2CommonITEMSGetCategory(hItem);
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = (WORD)D2CommonUnitGetX(hItem);
	tPacket.wY = (WORD)D2CommonUnitGetY(hItem);
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	if (SCmdSendSocketed(ptClient, hItem, dwFlags))
		SCmdSendItemOwner(ptClient, hOwner);
}

void __fastcall SCmdSendItemStack(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 48;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	SCmdSendSocketed(ptClient, hItem, dwFlags);
}

void __fastcall SCmdSendItemSwapGrid(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 51;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.bField01 = D2CommonUnitTestEquipSlotConflict(hOwner, hItem);
	tPacket.bBodyLoc = D2CommonITEMSGetCategory(hItem);
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = (WORD)D2CommonUnitGetX(hItem);
	tPacket.wY = (WORD)D2CommonUnitGetY(hItem);
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	SCmdSendSocketed(ptClient, hItem, dwFlags);
}

void __fastcall SCmdSendItemToBelt(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = S2C_ITEM_EVENT_34;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = (WORD)D2CommonUnitGetX(hItem);
	tPacket.wY = 0;
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	SCmdSendSocketed(ptClient, hItem, dwFlags);
}

void __fastcall SCmdSendItemFromBelt(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 53;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = (WORD)D2CommonUnitGetX(hItem);
	tPacket.wY = 0;
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	SCmdSendSocketed(ptClient, hItem, dwFlags);
}

void __fastcall SCmdSendItemSwapBelt(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 54;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = (WORD)D2CommonUnitGetX(hItem);
	tPacket.wY = 0;
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	SCmdSendSocketed(ptClient, hItem, dwFlags);
}

#define UNIT_OBJECT				2

#pragma pack(push, 1)
struct SCMDPACKET37
{
	BYTE		bCmd;
	BYTE		unobserved_01;
	DWORD		dwUnitId;
	BYTE		bGate;
	BYTE		bStat;
	DWORD		dwValue;
};
struct SCMDPACKET38
{
	BYTE		bCmd;
	BYTE		bSpellIcon;
	DWORD		dwUnitId;
	WORD		wSkillId;
};
struct SCMDPACKET70
{
	BYTE		bCmd;
	BYTE		bType;
	DWORD		dwUnitId;
	DWORD		dwOtherId;
	DWORD		dwAction;
	DWORD		dwFlagValue;
};
struct SCMDPACKET46
{
	BYTE		bCmd;
	BYTE		bSourceType;
	DWORD		dwSourceId;
	WORD		wField06;
	BYTE		bField08;
	WORD		wX;
	WORD		wY;
	WORD		wField0d;
};
struct SCMDPACKET45
{
	BYTE		bCmd;
	BYTE		bSourceType;
	DWORD		dwSourceId;
	WORD		wSkillId;
	BYTE		bSkillLevel;
	BYTE		bType;
	DWORD		dwUnitId;
	WORD		wField0e;
};
struct SCMDPACKETQUESTS
{
	BYTE		bCmd;
	BYTE		bType;
	DWORD		dwUnitId;
	BYTE		bField06;
	BYTE		abQuests[96];
};
struct SCMDPACKET7
{
	BYTE		bCmd;
	BYTE		bType;
	DWORD		dwUnitId;
	BYTE		unobserved_06;
};
struct SCMDPACKET15
{
	BYTE		bCmd;
	BYTE		bField01;
	BYTE		bResult;
	BYTE		unobserved_03[4];
	DWORD		dwUnitId;
	DWORD		dwGold;
};
struct SCMDCOPY8
{
	BYTE		abData[8];
};
struct SCMDCOPY7
{
	BYTE		abData[7];
};
struct SCMDCOPY15
{
	BYTE		abData[15];
};
struct SCMDCOPY21
{
	BYTE		abData[21];
};
struct SCMDCOPY33
{
	BYTE		abData[33];
};
struct SCMDCOPY6
{
	BYTE		abData[6];
};
struct SCMDCOPY26
{
	BYTE		abData[26];
};
struct SCMDPACKET11
{
	BYTE		bCmd;
	BYTE		bType;
	DWORD		dwUnitId;
	BYTE		bOverlay;
	BYTE		bFrames;
};
struct SCMDPACKET12
{
	BYTE		bCmd;
	BYTE		bType;
	DWORD		dwUnitId;
	BYTE		abStates[20];
};
struct SCMDPACKET13
{
	BYTE		bCmd;
	BYTE		bType;
	DWORD		dwUnitId;
	DWORD		dwState;
	DWORD		dwValue;
};
struct SCMDPACKET14
{
	BYTE		bCmd;
	BYTE		bType;
	DWORD		dwUnitId;
	DWORD		dwState;
	DWORD		dwValue;
	DWORD		dwValue2;
};
struct SCMDPACKET4E
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	WORD		wClassId;
	WORD		wX;
	WORD		wY;
	BYTE		bLife;
	BYTE		bMode;
};
struct SCMDPACKET4F
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	WORD		wClass;
	WORD		wX;
	WORD		wY;
	BYTE		bLife;
	BYTE		bMode;
	WORD		wFlags;
	DWORD		dwOwnerId;
	WORD		wNameNumber;
	BYTE		abComponentVariations[16];
	BYTE		abUniqueMods[9];
};
struct SCMDPACKET50
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	BYTE		bType;
	WORD		wNameNumber;
	WORD		wUniqueMods01;
	WORD		wUniqueMods2;
	WORD		wChampion;
};
struct SCMDPACKET59
{
	BYTE		bCmd;
	BYTE		bFlags;
	BYTE		bLevelId;
	DWORD		dwUnitId;
};
struct SCMDPACKET2
{
	BYTE		bCmd;
	BYTE		bValue;
};
struct SCMDPACKET5E
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	WORD		wTag;
};
struct SCMDPACKET67
{
	BYTE		bCmd;
	BYTE		bGate;
	DWORD		dwUnitId;
	DWORD		dwOtherId;
};
struct SCMDPACKET6E
{
	BYTE		bCmd;
	BYTE		bKey;
	BYTE		bSkillId;
	BYTE		bHand;
};
struct SCMDPACKET75
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	char		szName[16];
	DWORD		dwObjectId;
	DWORD		dwObjectIdB;
};
struct SCMDPACKET76
{
	BYTE		bCmd;
	char		szCode[4];
	WORD		wOffset;
	WORD		wLength;
	WORD		wTotalSize;
	BYTE		abData[412 - 11];
};
struct SCMDPACKET77
{
	BYTE		bCmd;
	BYTE		bCounter;
	BYTE		abData[404 - 2];
};
struct SCMDPACKET7E
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	BYTE		bButton;
};
struct SCMDPACKET7F
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	DWORD		dwOtherId;
	WORD		wFlags;
};
struct SCMDPACKET80
{
	BYTE		bCmd;
	DWORD		dwUnitId;
	WORD		wParty;
};
struct SCMDPACKET81
{
	BYTE		bCmd;
	BYTE		bAdd;
	DWORD		dwUnitId;
	DWORD		dwBodyId;
};
struct SCMDPACKET79
{
	BYTE		bCmd;
	WORD		wOffset;
	WORD		wLength;
	WORD		wTotal;
	BYTE		abData[400];
};
struct SCMDPACKET7A
{
	BYTE		bCmd;
	char		szText[404 - 1];
};
struct SCMDDATA82
{
	DWORD		dwTick;
	DWORD		dwTicks;
	BYTE		abField09[24];
};
struct SCMDPACKET82
{
	BYTE		bCmd;
	SCMDDATA82	tData;
};
struct SCMDPACKET16
{
	BYTE		bCmd;
	BYTE		bCount;
	CLIENTNEARUNIT	atNear[1];
};
struct SCMDRECORD16
{
	WORD		wSize;
	SCMDPACKET16	tPacket;
};
#pragma pack(pop)

void __fastcall SCmdQueueRelation(GAME *ptGame, UNIT *hUnit, void *pPacket);
void __fastcall SCmdQueueParty(GAME *ptGame, UNIT *hUnit, void *pPacket);
void __fastcall SCmdQueueCorpse(GAME *ptGame, UNIT *hUnit, void *pPacket);

int __fastcall SCmdSendItemStat(CLIENT *ptClient, UNIT *hUnit, BYTE bGate,
	BYTE bStat, DWORD dwValue)
{
	SCMDPACKET37 tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.dwUnitId = hUnit->dwId;
	tPacket.bGate = bGate;
	tPacket.bStat = bStat;
	tPacket.bCmd = 55;
	tPacket.dwValue = dwValue;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

int __fastcall SCmdSendSpellIcon(CLIENT *ptClient, UNIT *hUnit, BYTE bSpellIcon,
	DWORD fFlag, DWORD dwUnused, WORD wSkillId)
{
	SCMDPACKET38 tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 56;
	if (fFlag)
	{
		tPacket.bSpellIcon = 255;
		if (hUnit)
			tPacket.dwUnitId = hUnit->dwId;
		else
			tPacket.dwUnitId = 0;
	}
	else
	{
		tPacket.bSpellIcon = bSpellIcon;
		if (hUnit)
			tPacket.dwUnitId = hUnit->dwId;
	}
	tPacket.wSkillId = wSkillId;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendItemHandToGrid(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 58;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.bField01 = D2CommonUnitTestEquipSlotConflict(hOwner, hItem);
	tPacket.bBodyLoc = D2CommonITEMSGetCategory(hItem);
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = (WORD)D2CommonUnitGetX(hItem);
	tPacket.wY = (WORD)D2CommonUnitGetY(hItem);
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	if (SCmdSendSocketed(ptClient, hItem, dwFlags))
		SCmdSendItemOwner(ptClient, hOwner);
}

void __fastcall SCmdSendCursorCleared(CLIENT *ptClient, UNIT *hUnit)
{
	SCMDPACKET6 tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 59;
	tPacket.bField01 = (BYTE)hUnit->eType;
	tPacket.dwField02 = hUnit->dwId;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendSetHandItem(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 60;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem);
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendItemEquipFromGrid(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 63;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.bField01 = D2CommonUnitTestEquipSlotConflict(hOwner, hItem);
	tPacket.bBodyLoc = D2CommonITEMSGetCategory(hItem);
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = (WORD)D2CommonUnitGetX(hItem);
	tPacket.wY = (WORD)D2CommonUnitGetY(hItem);
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	if (SCmdSendSocketed(ptClient, hItem, dwFlags))
		SCmdSendItemOwner(ptClient, hOwner);
}

int __fastcall SCmdSendItemsRefreshed(CLIENT *ptClient, UNIT *hUnit)
{
	SCMDPACKET41 tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 64;
	tPacket.bType = (BYTE)hUnit->eType;
	tPacket.dwUnitId = hUnit->dwId;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendEquipmentChecked(CLIENT *ptClient, UNIT *hUnit)
{
	SCMDPACKET41 tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = S2C_INVENTORY_FLAG_SWEEP_LOCAL;
	tPacket.bType = (BYTE)hUnit->eType;
	tPacket.dwUnitId = hUnit->dwId;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendItemHeld(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 66;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.bField01 = D2CommonUnitTestEquipSlotConflict(hOwner, hItem);
	tPacket.bBodyLoc = D2CommonITEMSGetCategory(hItem);
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = (WORD)D2CommonUnitGetX(hItem);
	tPacket.wY = (WORD)D2CommonUnitGetY(hItem);
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	SCmdSendSocketed(ptClient, hItem, dwFlags);
}

void __fastcall SCmdSendItemFlag(CLIENT *ptClient, UNIT *hUnit, UNIT *hOther,
	DWORD dwAction, DWORD dwFlagValue)
{
	SCMDPACKET70 tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 112;
	tPacket.bType = (BYTE)hUnit->eType;
	tPacket.dwUnitId = hUnit->dwId;
	tPacket.dwOtherId = hOther->dwId;
	tPacket.dwAction = dwAction;
	tPacket.dwFlagValue = dwFlagValue;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendItemNoPlace(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, DWORD dwFlags)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bCmd = 67;
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = 0;
	tPacket.wY = 0;
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem) | dwFlags;
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	SCmdSendSocketed(ptClient, hItem, dwFlags);
}

int __fastcall SCmdSendTargetPoint(CLIENT *ptClient, BYTE bSourceType,
	DWORD dwSourceId,
	WORD wX, WORD wY, WORD wSkillId,
	WORD wField0d, BYTE bSkillLevel)
{
	SCMDPACKET46 tPacket;

	tPacket.bSourceType = bSourceType;
	tPacket.dwSourceId = dwSourceId;
	tPacket.wX = wX;
	tPacket.wY = wY;
	tPacket.wField06 = wSkillId;
	tPacket.bCmd = 70;
	tPacket.bField08 = bSkillLevel;
	tPacket.wField0d = wField0d;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendUnitAt(CLIENT *ptClient, BYTE bSourceType,
	DWORD dwSourceId, BYTE bType, DWORD dwUnitId,
	WORD wSkillId, WORD wField0e, BYTE bSkillLevel)
{
	UNIT *hUnit;
	GAMEROOM *hRoom;
	GAMEROOM *hUnitRoom;
	GAMEROOM **phNear;
	DWORD dwNumNear;
	DWORD i;
	WORD wX;
	WORD wY;

	hUnit = SUnitFind(ClientGetGame(ptClient), bType, dwUnitId);
	if (!hUnit)
		return;
	hRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(ClientGetPlayerUnit(ptClient, FALSE));
	if (hRoom)
	{
		D2CommonDungeonRoomGetNear(hRoom, &phNear, &dwNumNear);
		hUnitRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hUnit);
		for (i = 0; i < dwNumNear; i++)
		{
			if (hUnitRoom == phNear[i])
			{
				SCMDPACKET45 tPacket;

				tPacket.bCmd = 69;
				tPacket.bSourceType = bSourceType;
				tPacket.dwSourceId = dwSourceId;
				tPacket.bType = bType;
				tPacket.dwUnitId = dwUnitId;
				tPacket.wSkillId = wSkillId;
				tPacket.bSkillLevel = bSkillLevel;
				tPacket.wField0e = wField0e;
				SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
				return;
			}
		}
	}
	wY = D2CommonPathGetTargetY(hUnit->ptPath);
	wX = D2CommonPathGetTargetX(hUnit->ptPath);
	{
		SCMDPACKET46 tPacket;

		tPacket.bCmd = 70;
		tPacket.bSourceType = bSourceType;
		tPacket.dwSourceId = dwSourceId;
		tPacket.wX = wX;
		tPacket.wY = wY;
		tPacket.wField06 = wSkillId;
		tPacket.bField08 = bSkillLevel;
		tPacket.wField0d = wField0e;
		SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	}
}

void __fastcall SCmdSendItemCmd(CLIENT *ptClient, UNIT *hOwner, UNIT *hItem, BYTE bCmd)
{
	SCMDPACKETITEM tPacket;

	memset(&tPacket, 0, sizeof(tPacket));
	tPacket.bOwnerType = (BYTE)hOwner->eType;
	tPacket.dwOwnerId = hOwner->dwId;
	tPacket.dwFlags = 0;
	tPacket.dwUnitId = hItem->dwId;
	tPacket.wClass = (WORD)hItem->dwClassId;
	tPacket.dwInitSeed = hItem->dwInitSeed;
	tPacket.wX = (WORD)D2CommonUnitGetX(hItem);
	tPacket.wY = (WORD)D2CommonUnitGetY(hItem);
	tPacket.bMode = (BYTE)hItem->dwMode;
	tPacket.bCmd = bCmd;
	tPacket.dwFlags = D2CommonITEMSDataGetFlags(hItem);
	tPacket.dwFlags |= SCmdFillItem(&tPacket, hItem);
	if (bCmd == 49)
		tPacket.bPage = D2CommonITEMSDataGetPage(hItem);
	else
		tPacket.bPage = D2CommonITEMSDataGetPreviousPage(hItem);
	if (D2CommonITEMSTestModTypeMagicOrAbove(hItem) && !D2CommonITEMSDataTestFlags(hItem, 16))
		tPacket.bModType = 2;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
	SCmdSendSocketed(ptClient, hItem, 0);
}

int __fastcall SCmdSendQuests(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, void *ptQuestRecord, BYTE bField06)
{
	SCMDPACKETQUESTS tPacket;

	tPacket.bType = bType;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bCmd = bCmd;
	tPacket.bField06 = bField06;
	D2CommonQuestRecordCopyOut(ptQuestRecord, tPacket.abQuests, sizeof(tPacket.abQuests), 0);
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

int __fastcall SCmdSendInteractionStop(CLIENT *ptClient, BYTE bCmd, BYTE bType,
	DWORD dwUnitId, DWORD dwUnused)
{
	SCMDPACKET7 tPacket;

	tPacket.bCmd = bCmd;
	tPacket.bType = bType;
	tPacket.dwUnitId = dwUnitId;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

int __fastcall SCmdSendQuestRecord(CLIENT *ptClient, const void *pPacket)
{
	return SCmdQueue(ptClient, pPacket, 97);
}

int __fastcall SCmdSendQuestStatus(CLIENT *ptClient, const void *pPacket)
{
	return SCmdQueue(ptClient, pPacket, 5);
}

int __fastcall SCmdSendNpcResult(CLIENT *ptClient, BYTE bCmd, BYTE bResult,
	DWORD dwGold, DWORD dwUnitId, BYTE bField01)
{
	SCMDPACKET15 tPacket;

	tPacket.bCmd = bCmd;
	tPacket.bResult = bResult;
	tPacket.dwGold = dwGold;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bField01 = bField01;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

int __fastcall SCmdSendUnitSound(CLIENT *ptClient, const SCMDCOPY8 *ptPacket)
{
	SCMDCOPY8 tPacket;

	tPacket = *ptPacket;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

int __fastcall SCmdSendHire(CLIENT *ptClient, const SCMDCOPY7 *ptPacket)
{
	SCMDCOPY7 tPacket;

	tPacket = *ptPacket;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

int __fastcall SCmdSendQuestCommand(CLIENT *ptClient, const SCMDCOPY15 *ptPacket)
{
	SCMDCOPY15 tPacket;

	tPacket = *ptPacket;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendQuestList(CLIENT *ptClient, const void *pPacket)
{
	SCmdQueue(ptClient, pPacket, 31);
}

int __fastcall SCmdSendQuestLog(CLIENT *ptClient, const void *pPacket)
{
	return SCmdQueue(ptClient, pPacket, 37);
}

int __fastcall SCmdSendOverlay(CLIENT *ptClient, BYTE bType,
	DWORD dwUnitId, BYTE bOverlay, BYTE bFrames)
{
	SCMDPACKET11 tPacket;

	tPacket.bType = bType;
	tPacket.dwUnitId = dwUnitId;
	tPacket.bOverlay = bOverlay;
	tPacket.bCmd = 17;
	tPacket.bFrames = bFrames;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

int __fastcall SCmdSendUnitChangeState(CLIENT *ptClient, BYTE bType,
	DWORD dwUnitId, const BYTE *pbStates)
{
	SCMDPACKET12 tPacket;

	tPacket.dwUnitId = dwUnitId;
	tPacket.bCmd = 18;
	tPacket.bType = bType;
	memcpy(tPacket.abStates, pbStates, sizeof(tPacket.abStates));
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendUnitChangeStateVal(CLIENT *ptClient, BYTE bType,
	DWORD dwUnitId, DWORD dwState, DWORD dwValue)
{
	SCMDPACKET13 tPacket;

	tPacket.bType = bType;
	tPacket.dwUnitId = dwUnitId;
	tPacket.dwState = dwState;
	tPacket.bCmd = 19;
	tPacket.dwValue = dwValue;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendUnitChangeStateValVal(CLIENT *ptClient, BYTE bType,
	DWORD dwUnitId,
	DWORD dwState, DWORD dwValue, DWORD dwValue2)
{
	SCMDPACKET14 tPacket;

	tPacket.bType = bType;
	tPacket.dwUnitId = dwUnitId;
	tPacket.dwState = dwState;
	tPacket.bCmd = 20;
	tPacket.dwValue = dwValue;
	tPacket.dwValue2 = dwValue2;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendMonsterShort(CLIENT *ptClient, DWORD dwUnitId,
	WORD wClassId,
	WORD wX, WORD wY, BYTE bLife,
	BYTE bMode)
{
	SCMDPACKET4E tPacket;

	tPacket.dwUnitId = dwUnitId;
	tPacket.wClassId = wClassId;
	tPacket.wX = wX;
	tPacket.wY = wY;
	tPacket.bCmd = S2C_MONSTER_SPAWN;
	tPacket.bLife = bLife;
	if (bMode == 8 || bMode == 9)
		tPacket.bMode = bMode;
	else
		tPacket.bMode = 1;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendMonster(CLIENT *ptClient, UNIT *hUnit, BYTE bMode)
{
	SCMDPACKET4F tPacket;
	int nLife;
	int nMaxLife;
	int i;
	struct MONSTERUNIQUE *pRecord;
	DWORD *pdwRecord;
	WORD wFlags;
	BYTE bLife;
	UNIT *hOwner;
	MONSTERDATA *pData;

	tPacket.bCmd = 79;
	tPacket.dwUnitId = hUnit->dwId;
	tPacket.wClass = (WORD)hUnit->dwClassId;
	tPacket.wX = (WORD)D2CommonUnitGetX(hUnit);
	tPacket.wY = (WORD)D2CommonUnitGetY(hUnit);
	if (bMode == 8 || bMode == 9)
		tPacket.bMode = bMode;
	else
		tPacket.bMode = 1;
	nLife = D2CommonStatsUnitGetValue(hUnit, 6);
	nMaxLife = D2CommonStatsUnitCalcMaxHp(hUnit);
	if (nMaxLife)
		bLife = (BYTE)((nLife << 7) / nMaxLife);
	else
		bLife = 128;
	tPacket.bLife = bLife;
	for (i = 0; i < 16; i++)
		tPacket.abComponentVariations[i] = hUnit->abComponentVariations[(BYTE)i];
	pData = (MONSTERDATA *)hUnit->uTypeData;
	pRecord = pData->ptUnique;
	pdwRecord = (DWORD *)MonsterUniqueGetMods(pData->ptUnique);
	for (i = 0; i < 9; i++)
		tPacket.abUniqueMods[i] = (BYTE)pdwRecord[i];
	wFlags = 0;
	if (MonsterUniqueIsChampion(pRecord))
		wFlags = 4;
	if (MonsterUniqueIsCounted(pRecord))
		wFlags |= 1;
	if (MonsterUniqueIsMinion(pRecord))
		wFlags |= 2;
	tPacket.wFlags = wFlags;
	tPacket.dwOwnerId = 0xFFFFFFFF;
	hOwner = AiGetOwner(hUnit);
	if (hOwner && hOwner->eType == UNIT_PLAYER && PlayerPetsGetType(hOwner, tPacket.dwUnitId) == 7)
		tPacket.dwOwnerId = hOwner->dwId;
	tPacket.wNameNumber = MonsterUniqueGetId(pRecord);
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

int __fastcall SCmdSendMonsterMods(CLIENT *ptClient, DWORD dwUnitId,
	BYTE bType,
	WORD wNameNumber, WORD wUniqueMods01, WORD wUniqueMods2,
	WORD wChampion)
{
	SCMDPACKET50 tPacket;

	tPacket.dwUnitId = dwUnitId;
	tPacket.bType = bType;
	tPacket.wNameNumber = wNameNumber;
	tPacket.wUniqueMods01 = wUniqueMods01;
	tPacket.wUniqueMods2 = wUniqueMods2;
	tPacket.bCmd = 80;
	tPacket.wChampion = wChampion;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

int __fastcall SCmdSendUseItemState(CLIENT *ptClient, const SCMDCOPY7 *ptPacket)
{
	SCMDCOPY7 tPacket;

	tPacket = *ptPacket;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

int __fastcall SCmdSendObject(CLIENT *ptClient, UNIT *hObject)
{
	SCMDPACKET59 tPacket;

	tPacket.bCmd = 89;
	tPacket.bFlags = D2CommonUnitGetObjectFlags(hObject);
	tPacket.bLevelId = ((OBJECTDATA *)hObject->uTypeData)->field_04;
	tPacket.dwUnitId = hObject->dwId;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

int __fastcall SCmdSendVideo(CLIENT *ptClient, BYTE bValue)
{
	SCMDPACKET2 tPacket;

	tPacket.bValue = bValue;
	tPacket.bCmd = 90;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendWaypoints(CLIENT *ptClient, const SCMDCOPY21 *ptPacket)
{
	SCMDCOPY21 tPacket;

	tPacket = *ptPacket;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

int __fastcall SCmdSendArenaTag(CLIENT *ptClient, DWORD dwUnitId, WORD wTag)
{
	SCMDPACKET5E tPacket;

	tPacket.dwUnitId = dwUnitId;
	tPacket.bCmd = S2C_ROSTER_FIELD;
	tPacket.wTag = wTag;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

int __fastcall SCmdSendMissile(CLIENT *ptClient, const SCMDCOPY33 *ptPacket)
{
	SCMDCOPY33 tPacket;

	tPacket = *ptPacket;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendCorpseOwner(CLIENT *ptClient, UNIT *hUnit, UNIT *hOther,
	BOOL fGate)
{
	SCMDPACKET67 tPacket;

	tPacket.bGate = (BYTE)fGate;
	tPacket.dwUnitId = hUnit->dwId;
	tPacket.bCmd = 103;
	tPacket.dwOtherId = hOther->dwId;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

int __fastcall SCmdSendPlayerInfo(UNIT *hPlayer, UNIT *hUnit)
{
	SCMDPACKET68 tPlayer;
	CLIENT *ptPlayerClient;

	ptPlayerClient = SUnitGetClient(hPlayer);
	memset(&tPlayer, 0, sizeof(tPlayer));
	tPlayer.bCmd = S2C_PARTY_UPDATE;
	tPlayer.dwUnitId = hUnit->dwId;
	tPlayer.wLevel = (WORD)D2CommonStatsUnitGetValue(hUnit, 12);
	tPlayer.wParty = (WORD)SUnitGetParty(hUnit);
	tPlayer.wFlags = (WORD)PlayerListGetFlags(hPlayer, hUnit);
	tPlayer.wButton = (WORD)PlayerListGetState(hPlayer, hUnit);
	return SCmdQueue(ptPlayerClient, &tPlayer, sizeof(tPlayer));
}

void __fastcall SCmdSendPair(CLIENT *ptClient, DWORD bKey, DWORD bSkillId,
	DWORD bHand)
{
	SCMDPACKET6E tPacket;

	tPacket.bKey = (BYTE)bKey;
	tPacket.bCmd = S2C_CMD_0X6E;
	tPacket.bSkillId = (BYTE)bSkillId;
	tPacket.bHand = (BYTE)bHand;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendMonRegionSeed(CLIENT *ptClient, DWORD dwValue)
{
	SCMDPACKET5 tPacket;

	tPacket.dwValue = dwValue;
	tPacket.bCmd = S2C_CMD_H71;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendPortalOwner(GAME *ptGame, CLIENT *ptClient, DWORD dwObjectId,
	DWORD dwObjectIdB)
{
	SCMDPACKET75 tPacket;
	UNIT *hObject;
	UNIT *hPlayer;

	tPacket.bCmd = 117;
	hObject = SUnitFind(ptGame, UNIT_OBJECT, dwObjectId);
	tPacket.szName[0] = 0;
	if (!hObject)
		return;
	hPlayer = SUnitFind(ptGame, UNIT_PLAYER, (DWORD)ObjectEvalGetLinkValue(hObject));
	if (!hPlayer)
		return;
	StormSStrCopy(tPacket.szName, D2CommonUnitGetPlrInfo(hPlayer), sizeof(tPacket.szName));
	tPacket.dwUnitId = hPlayer->dwId;
	tPacket.dwObjectId = dwObjectId;
	tPacket.dwObjectIdB = dwObjectIdB;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

int __fastcall SCmdSendGuildSaveChunk(CLIENT *ptClient, const char *pszCode,
	WORD wLength, WORD wOffset, WORD wTotalSize,
	const BYTE *pbData)
{
	SCMDPACKET76 tPacket;

	tPacket.bCmd = 118;
	strcpy(tPacket.szCode, pszCode);
	tPacket.wLength = wLength;
	tPacket.wOffset = wOffset;
	tPacket.wTotalSize = wTotalSize;
	memcpy(tPacket.abData, pbData, wLength);
	return SCmdQueue(ptClient, &tPacket, wLength + 11);
}

void __fastcall SCmdSendGuildCounter(CLIENT *ptClient, BYTE bCounter,
	WORD wLength, const BYTE *pbData)
{
	SCMDPACKET77 tPacket;

	tPacket.bCounter = bCounter;
	tPacket.bCmd = 119;
	memcpy(tPacket.abData, pbData, wLength);
	SCmdQueue(ptClient, &tPacket, wLength + 2);
}

void __fastcall SCmdSendGuildRosterChunk(CLIENT *ptClient, const char *pszCode,
	WORD wLength, WORD wOffset, WORD wTotalSize,
	const BYTE *pbData)
{
	SCMDPACKET76 tPacket;

	tPacket.bCmd = 123;
	strcpy(tPacket.szCode, pszCode);
	tPacket.wLength = wLength;
	tPacket.wOffset = wOffset;
	tPacket.wTotalSize = wTotalSize;
	memcpy(tPacket.abData, pbData, wLength);
	SCmdQueue(ptClient, &tPacket, wLength + 11);
}

int __fastcall SCmdSendRosterState(CLIENT *ptClient, DWORD dwUnitId,
	BYTE bButton)
{
	SCMDPACKET7E tPacket;

	tPacket.dwUnitId = dwUnitId;
	tPacket.bCmd = S2C_ROSTER_FLAG;
	tPacket.bButton = bButton;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendAllRelation(GAME *ptGame, DWORD dwUnitId,
	DWORD dwOtherId, WORD wFlags)
{
	SCMDPACKET7F tPacket;

	tPacket.dwUnitId = dwUnitId;
	tPacket.dwOtherId = dwOtherId;
	tPacket.wFlags = wFlags;
	tPacket.bCmd = S2C_ROSTER_PAIR;
	SUnitForEachPlayer(ptGame, SCmdQueueRelation, &tPacket);
}

void __fastcall SCmdQueueRelation(GAME *ptGame, UNIT *hUnit, void *pPacket)
{
	SCmdQueue(SUnitGetClient(hUnit), pPacket, sizeof(SCMDPACKET7F));
}

int __fastcall SCmdSendRelation(UNIT *hUnit, DWORD dwUnitId, DWORD dwOtherId,
	WORD wFlags)
{
	SCMDPACKET7F tPacket;

	tPacket.dwUnitId = dwUnitId;
	tPacket.bCmd = S2C_ROSTER_PAIR;
	tPacket.dwOtherId = dwOtherId;
	tPacket.wFlags = wFlags;
	return SCmdQueue(SUnitGetClient(hUnit), &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendAllParty(GAME *ptGame, UNIT *hUnit)
{
	SCMDPACKET80 tPacket;

	tPacket.bCmd = S2C_ROSTER_FIELD2;
	tPacket.dwUnitId = hUnit->dwId;
	tPacket.wParty = (WORD)SUnitGetParty(hUnit);
	SUnitForEachPlayer(ptGame, SCmdQueueParty, &tPacket);
}

void __fastcall SCmdQueueParty(GAME *ptGame, UNIT *hUnit, void *pPacket)
{
	SCmdQueue(SUnitGetClient(hUnit), pPacket, sizeof(SCMDPACKET80));
}

void __fastcall SCmdSendParty(UNIT *hPlayer, UNIT *hUnit)
{
	SCMDPACKET80 tPacket;

	tPacket.bCmd = S2C_ROSTER_FIELD2;
	tPacket.dwUnitId = hUnit->dwId;
	tPacket.wParty = (WORD)SUnitGetParty(hUnit);
	SCmdQueue(SUnitGetClient(hPlayer), &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendAllCorpse(GAME *ptGame, DWORD dwUnitId,
	DWORD dwBodyId, BYTE bAdd)
{
	SCMDPACKET81 tPacket;

	tPacket.dwUnitId = dwUnitId;
	tPacket.dwBodyId = dwBodyId;
	tPacket.bAdd = bAdd;
	tPacket.bCmd = 129;
	SUnitForEachPlayer(ptGame, SCmdQueueCorpse, &tPacket);
}

void __fastcall SCmdQueueCorpse(GAME *ptGame, UNIT *hUnit, void *pPacket)
{
	SCmdQueue(SUnitGetClient(hUnit), pPacket, sizeof(SCMDPACKET81));
}

void __fastcall SCmdSendCorpse(CLIENT *ptClient, DWORD dwUnitId,
	DWORD dwBodyId, BYTE bAdd)
{
	SCMDPACKET81 tPacket;

	tPacket.dwUnitId = dwUnitId;
	tPacket.bCmd = 129;
	tPacket.dwBodyId = dwBodyId;
	tPacket.bAdd = bAdd;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendBuffer(CLIENT *ptClient, WORD wTotal,
	const BYTE *pbData)
{
	SCMDPACKET79 tPacket;
	int nLeft;

	tPacket.wTotal = wTotal;
	tPacket.bCmd = 121;
	tPacket.wOffset = 0;
	nLeft = wTotal;
	while (nLeft)
	{
		if (nLeft > (int)sizeof(tPacket.abData))
			tPacket.wLength = sizeof(tPacket.abData);
		else
			tPacket.wLength = (WORD)nLeft;
		memcpy(tPacket.abData, pbData + tPacket.wOffset, tPacket.wLength);
		SCmdQueue(ptClient, &tPacket, tPacket.wLength + 7);
		tPacket.wOffset += tPacket.wLength;
		nLeft -= tPacket.wLength;
	}
}

int __fastcall SCmdSendText(CLIENT *ptClient, const char *pszText)
{
	SCMDPACKET7A tPacket;
	int nLength;

	nLength = strlen(pszText);
	if (nLength) {
		tPacket.bCmd = 122;
		memcpy(tPacket.szText, pszText, nLength + 1);
		nLength = SCmdQueue(ptClient, &tPacket, nLength + 2); }
	return nLength;
}

int __fastcall SCmdSendQuestTrigger(CLIENT *ptClient, const void *pPacket)
{
	return SCmdQueue(ptClient, pPacket, 2);
}

void __fastcall SCmdSendNpcActive(CLIENT *ptClient, const SCMDCOPY6 *ptPacket)
{
	SCMDCOPY6 tPacket;

	tPacket = *ptPacket;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendPing(CLIENT *ptClient, DWORD dwTick)
{
	SCMDPACKET82 tPacket;
	DWORD i;

	tPacket.bCmd = S2C_PING_REPLY;
	for (i = 0; i < sizeof(tPacket.tData) / sizeof(DWORD); i++)
		((DWORD *)&tPacket.tData)[i] = 0;
	tPacket.tData.dwTick = dwTick;
	tPacket.tData.dwTicks = GetTickCount() - (DWORD)ptClient->nPingAverage;
	SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

int __fastcall SCmdSendNpcIntroduced(CLIENT *ptClient, const SCMDCOPY26 *ptPacket)
{
	SCMDCOPY26 tPacket;

	tPacket = *ptPacket;
	return SCmdQueue(ptClient, &tPacket, sizeof(tPacket));
}

void __fastcall SCmdSendNearList(CLIENT *ptClient)
{
	int nNear;
	CLIENTNODE178 *ptNode;
	SCMDRECORD16 *ptRecord;
	CLIENTNEARUNIT *ptDst;
	WORD wSize;
	int i;

	nNear = ptClient->nNear;
	if (!nNear)
		return;
	ptNode = ClientGetQueueHead(ptClient);
	wSize = (WORD)(sizeof(SCMDPACKET16) + nNear * sizeof(CLIENTNEARUNIT));
	ptRecord = (SCMDRECORD16 *)&ptNode->abData[ptNode->nUsed];
	ptRecord->wSize = wSize;
	ptRecord->tPacket.bCmd = 22;
	ptRecord->tPacket.bCount = (BYTE)nNear;
	ptDst = ptRecord->tPacket.atNear;
	for (i = 0; i < nNear; i++)
		*ptDst++ = ptClient->atNear[i];
	ptNode->nUsed += wSize;
}

extern void __fastcall SCmdSendUnused(CLIENT *ptClient);
__inline void __fastcall SCmdSendUnused(CLIENT *ptClient)
{
	D2ASSERT(0);
}
