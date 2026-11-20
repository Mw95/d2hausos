#include "../D2game.h"
#define D2_THIS_FILE "C:\\D2\\Source\\D2Game\\OBJECTS\\Objects.cpp"
#include "D2Macros.h"
#include "../GAME/Event.h"
#include "../ITEMS/Items.h"
#include "../MONSTER/MonsterUnique.h"
#include "Objects.h"
#include "objrgn.h"
#include "../QUESTS/QUESTS.h"
#include "../QUESTS/a1q4.h"
#include "../QUESTS/a1q5.h"
#include "../QUESTS/a2q2.h"
#include "../QUESTS/a2q3.h"
#include "../QUESTS/a2q4.h"
#include "../QUESTS/a2q6.h"
#include "../QUESTS/a3q0.h"
#include "../QUESTS/a3q1.h"
#include "../QUESTS/a3q3.h"
#include "../QUESTS/a3q5.h"
#include "../QUESTS/a3q6.h"
#include "../QUESTS/a3q7.h"
#include "../QUESTS/a4q3.h"
#include "../UNIT/SUnit.h"
#include "../GUILDS/Guilds.h"

#define UNIT_PLAYER_TYPE	0
#define UNIT_OBJECT			2
#define NUM_OBJECTMODES		8
#define NUM_OBJECTS			410
#define NUM_LEVEL_IDS		109
#define OBJECTCLASS_ROGUESTAKEDCORPSE2	58
#define NUM_OBJECT_BUCKETS	128

#define OBJ_RAND(ptSeed, n)	((n) ? (int)D2CommonSeedRand(ptSeed, n) : 0)

typedef struct OBJPOINT OBJPOINT;
typedef struct OBJCLUSTER OBJCLUSTER;
typedef struct OBJTHEME OBJTHEME;

struct OBJPOINT
{
 	int			nX;
 	int			nY;
};
struct OBJCLUSTER
{
 	const OBJPOINT	*ptPoints;
 	int			nPoints;
};

typedef void (__fastcall *PTOBJECTINIT)(OBJINIT *ptInit);
typedef UNIT * (__fastcall *PTOBJECTADDTOGAME)(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance);
typedef UNIT * (__fastcall *PFNOBJECTPRESET)(GAME *ptGame, GAMEROOM *hRoom, int nClass, int nX, int nY,
	DWORD dwArg);
typedef BOOL (__fastcall *PFNOBJTHEME)(GAME *ptGame, GAMEROOM *hRoom, GAMERECT *ptRect);
typedef BYTE (__fastcall *PFNOBJFILTER)(GAME *ptGame, int nLevelId, int nX, int nY);
struct OBJTHEME
{
 	BOOL		fEnabled;
 	PFNOBJTHEME	pfnTheme;
};

static int sganDirX[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
static int sganDirY[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };

void __fastcall ObjectInitTownEvent(OBJINIT *ptInit);
void __fastcall ObjectInitGoldPile(OBJINIT *ptInit);
void __fastcall ObjectInitShrine(OBJINIT *ptInit);
void __fastcall ObjectInitModeEvent(OBJINIT *ptInit);
void __fastcall ObjectInitWell(OBJINIT *ptInit);
void __fastcall ObjectInitFloorTrap(OBJINIT *ptInit);
void __fastcall ObjectInitChestLevel(OBJINIT *ptInit);
void __fastcall ObjectInitChestRare(OBJINIT *ptInit);
void __fastcall ObjectInitChest(OBJINIT *ptInit);
void __fastcall ObjectInitChestFlagged(OBJINIT *ptInit);
void __fastcall ObjectInitRandomEvent(OBJINIT *ptInit);
void __fastcall ObjectInitVileDogAfterglow(OBJINIT *ptInit);
void __fastcall ObjectInitTargetLevel(OBJINIT *ptInit);
void __fastcall ObjectInitOpened(OBJINIT *ptInit);
void __fastcall ObjectInitOperating(OBJINIT *ptInit);
void __fastcall ObjectInitOperatingRandom(OBJINIT *ptInit);
void __fastcall ObjectInitTownLevel(OBJINIT *ptInit);
void __fastcall ObjectInitGuildA(OBJINIT *ptInit);
void __fastcall ObjectInitGuildB(OBJINIT *ptInit);
void __fastcall ObjectInitGuildC(OBJINIT *ptInit);
void __fastcall ObjectInitTrappedSoul(OBJINIT *ptInit);
void __fastcall ObjectInitSpawnGroup(OBJINIT *ptInit);
UNIT *__fastcall ObjectPopulateCluster(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance);
UNIT *__fastcall ObjectPopulateCorpses(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance);
UNIT *__fastcall ObjectPopulateScatter(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance);
UNIT *__fastcall ObjectPopulateOne(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance);
UNIT *__fastcall ObjectPopulateScatterExtra(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance);
UNIT *__fastcall ObjectPopulateFiltered(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance);
UNIT *__fastcall ObjectPopulateOriented(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance);
UNIT *__fastcall ObjectPopulateStacks(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance);
UNIT *__fastcall ObjectPopulateGrouped(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance);
BOOL __fastcall ObjectThemeFill(GAME *ptGame, GAMEROOM *hRoom, GAMERECT *ptRect);
BOOL __fastcall ObjectThemeItems(GAME *ptGame, GAMEROOM *hRoom, GAMERECT *ptRect);
BOOL __fastcall ObjectThemeArmorStands(GAME *ptGame, GAMEROOM *hRoom, GAMERECT *ptRect);
BOOL __fastcall ObjectThemeWeaponRacks(GAME *ptGame, GAMEROOM *hRoom, GAMERECT *ptRect);

#define NUM_INIT_FUNCTIONS	62
static PTOBJECTINIT sgInitTable[NUM_INIT_FUNCTIONS] =
{
	NULL,
	ObjectInitShrine,
	ObjectInitChestLevel,
	ObjectInitChest,
	A1Q5ObjInit,
	(PTOBJECTINIT)A2Q2UnitKilled,
	A1Q4StoneInit,
	QuestsInitGibbet,
	ObjectInitOpened,
	A1Q4TreeObjInit,
	ObjectInitTownEvent,
	ObjectInitTownLevel,
	ObjectInitTargetLevel,
	A1Q4AttachObjInit,
	ObjectInitOperating,
	QuestsInitMalus,
	ObjectInitWell,
	ObjectRgnInitObject,
	QuestsInitJerhynMonster,
	QuestsInitJerhynNpc,
	QuestsInitTaintedSunAltar,
	A2Q6InitOrifice,
	ObjectInitModeEvent,
	QuestsInitLamTome,
	ObjectInitFloorTrap,
	A3Q3InitGidbinnDecoy,
	ObjectInitOperating,
	ObjectInitChestRare,
	ObjectInitGoldPile,
	A2Q4ObjInitLevel,
	A2Q4ObjInitCreated,
	(PTOBJECTINIT)A2Q3Nothing,
	(PTOBJECTINIT)A2Q3Nothing,
	(PTOBJECTINIT)A2Q3Nothing,
	ObjectInitOperatingRandom,
	ObjectInitGuildA,
	ObjectInitGuildB,
	A2Q6InitRecordedObject,
	A2Q6InitTyraelDoor,
	A3Q3InitGidbinnAltar,
	ObjectInitGuildC,
	A3Q2InitSewerStairs,
	A3Q2InitSewerLever,
	A3Q7InitDarkWandererStart,
	A3Q6InitHellgate,
	A3Q6InitMephistoBridge,
	ObjectInitSpawnGroup,
	A1Q5AddObject,
	A4Q3InitObjectMode,
	A3Q0ObjInit,
	A3Q1ObjInit,
	ObjectInitTrappedSoul,
	A3Q6InitNatalyaStart,
	A3Q5InitStairs,
	A1Q4NpcObjInit,
	A4Q3InitObject,
	(PTOBJECTINIT)A2Q3Nothing,
	ObjectInitChestFlagged,
	ObjectInitRandomEvent,
	ObjectInitVileDogAfterglow,
	A3Q5InitCompellingOrb,
	A1Q4EventObjInit,
};

static PTOBJECTADDTOGAME sgPopulateTable[10] =
{
	NULL,
	ObjectPopulateCluster,
	ObjectPopulateOriented,
	ObjectPopulateScatter,
	ObjectPopulateStacks,
	ObjectPopulateGrouped,
	ObjectPopulateScatterExtra,
	ObjectPopulateCorpses,
	ObjectPopulateFiltered,
	ObjectPopulateOne,
};
static BYTE sgbMaxGore = 2;

void __fastcall ObjectThemeRow(GAME *ptGame, GAMEROOM *hRoom, GAMERECT *ptRect, int nRow);

UNIT *__fastcall ObjectPresetMakeQuestObject(GAME *ptGame, GAMEROOM *hRoom, int nClass, int nX, int nY,
	DWORD dwArg)
{
	int nLevelId;
	UNIT *hObject;

	nLevelId = (int)D2CommonDungeonRoomGetLevelId(hRoom);
	hObject = MonsterUniqueCreateActObject(ptGame, hRoom, nLevelId == 25 ? 371 : nClass, nX, nY, dwArg);
	if (hObject)
	{
		if (nLevelId < 62 || nLevelId > 64
			|| D2CommonSeedRand(ObjectRgnGetSeed(ptGame, hRoom), 100) <= 50)
		{
			SUnitSetObjectFlags(hObject, 1);
			if (nLevelId == 25)
				OBJDATA(hObject)->field_04 = 3;
		}
		hObject->dwFlags0 |= 128;
		D2CommonUnitSetMode(hObject, 0);
	}
	return hObject;
}

UNIT *__fastcall ObjectPresetMakeQuestClass(GAME *ptGame, GAMEROOM *hRoom, int nClass, int nX, int nY,
	DWORD dwArg)
{
	DWORD t07 = A2Q6GetClass(ptGame);
	return SUnitCreate(UNIT_OBJECT, t07, nX, nY, ptGame, hRoom, 1, 0, 0);
}
#line 203
static PFNOBJECTPRESET sgPresetStuff[9] =
{
	MonsterUniqueCreateShrine,
	MonsterUniqueCreateShrine,
	MonsterUniqueCreateShrine,
	MonsterUniqueCreateShrine,
	MonsterUniqueCreateShrine,
	MonsterUniqueCreateShrine,
	ObjectPresetMakeQuestObject,
	MonsterUniqueCreateActObject,
	ObjectPresetMakeQuestClass,
};
static int sgnPresetStuff = 9;

UNIT *__fastcall ObjectPresetCreate(GAME *ptGame, GAMEROOM *hRoom, int nClass, int nX, int nY,
	DWORD dwArg)
{
	int idx;

	if (nClass <= 410)
		return NULL;
	idx = nClass - 411;
	if (idx >= sgnPresetStuff)
		return NULL;
#line 406
	D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgPresetStuff[idx]), "sgPresetStuff[idx]");
	return sgPresetStuff[idx](ptGame, hRoom, nClass, nX, nY, dwArg);
}

static OBJLINKS *ObjectNewLinks(GAME *ptGame)
{
	OBJLINKS *ptLinks;
	OBJLINKNODE *ptNode;

#line 417
	ptLinks = (OBJLINKS *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(OBJLINKS));
	memset(ptLinks, 0, sizeof(OBJLINKS));
	ptLinks->nValue = -1;
#line 421
	ptNode = (OBJLINKNODE *)D2ALLOCPOOL(ptGame->pMemPool, sizeof(OBJLINKNODE));
	ptNode->dwUnitId = (DWORD)-1;
	ptNode->ptNext = NULL;
	ptLinks->ptFirst = ptNode;
	return ptLinks;
}

void __fastcall ObjectInitUnit(GAME *ptGame, UNIT *hObject, DWORD dwUnitId, GAMEROOM *hRoom,
	int nX, int nY)
{
	MISSOBJECTSTXT *ptTypeData;
	int nMode;
	WORD wClass;
	D2SEED *ptSeed;

	ptTypeData = (MISSOBJECTSTXT *)D2CommonUnitGetObjectClassInfoPtr(hObject);
	hObject->dwId = (WORD)dwUnitId;
	SUnitSetObjectFlags(hObject, 0);
	hObject->dwFlags0 &= ~8u;
	UNIT5(hObject)->ptChatNode = NULL;
	nMode = (int)hObject->dwMode;
#line 553
	D2ASSERT(nMode < NUM_OBJECTMODES);
	if (ptTypeData->IsAttackable0)
		hObject->dwFlags0 |= 4;
	else
		hObject->dwFlags0 &= ~4u;
	OBJDATA(hObject)->field_04 = 0;
	wClass = (WORD)hObject->dwClassId;
	EventUnitInit(ptGame, hObject);
#line 585
	D2ASSERT(wClass < NUM_OBJECTS);
#line 587
	D2ASSERT(ptTypeData->bInitFunction < sizeof(sgInitTable)/sizeof(PTOBJECTINIT));
	ptSeed = ObjectRgnGetSeed(ptGame, hRoom);
	if (ptTypeData->bInitFunction < NUM_INIT_FUNCTIONS && sgInitTable[ptTypeData->bInitFunction])
	{
		OBJINIT tInit;

#line 594
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgInitTable[ptTypeData->bInitFunction]),
			"sgInitTable[ptTypeData->bInitFunction]");
		tInit.ptGame = ptGame;
		tInit.hObject = hObject;
		tInit.hRoom = hRoom;
		tInit.ptSeed = ptSeed;
		tInit.ptTypeData = ptTypeData;
		tInit.nX = nX;
		tInit.nY = nY;
		sgInitTable[ptTypeData->bInitFunction](&tInit);
	}
	if (ptTypeData->Selectable[nMode])
		hObject->dwFlags0 |= 2;
	else
		hObject->dwFlags0 &= ~2u;
	if (ptTypeData->PreOperate && !(hObject->dwFlags0 & 128) && !D2CommonSeedRand(ptSeed, 14))
		D2CommonUnitSetMode(hObject, 2);
	D2CommonUnitSetObjectLinks(hObject, ObjectNewLinks(ptGame));
}

void __fastcall ObjectInitTownEvent(OBJINIT *ptInit)
{
	if (D2CommonDungeonRoomGetLevelId(ptInit->hRoom) == 1)
	{
		EventAdd(ptInit->ptGame, ptInit->hObject, 8, ptInit->ptGame->nFrame + 60);
		return;
	}
	D2CommonUnitSetMode(ptInit->hObject, 2);
	EventAdd(ptInit->ptGame, ptInit->hObject, 0, ptInit->ptGame->nFrame + 25);
}

void __fastcall ObjectInitGoldPile(OBJINIT *ptInit)
{
	COLPOINT tCoord;
	int nCount;
	int nDX;
	int nDY;
	GAMEROOM *hRoom;

	if (ptInit->hObject->dwMode != 0)
		return;
	D2CommonUnitSetMode(ptInit->hObject, 2);
	tCoord.nX = ptInit->nX;
	tCoord.nY = ptInit->nY;
	for (nCount = (int)D2CommonSeedRand(ptInit->ptSeed, 9) + 1; nCount > 0; nCount--)
	{
		nDX = (int)D2CommonSeedRand(ptInit->ptSeed, 4);
		nDY = (int)D2CommonSeedRand(ptInit->ptSeed, 4);
		hRoom = (GAMEROOM *)D2CommonDungeonFindRoomGame(ptInit->hRoom, nDX + tCoord.nX, nDY + tCoord.nY);
		if (hRoom == ptInit->hRoom)
		{
			tCoord.nX = ptInit->nX + nDX;
			tCoord.nY = ptInit->nY + nDY;
			if (!D2CommonCollisnTestArea(hRoom, tCoord.nX, tCoord.nY, 1, 1, 7953))
				ITEMSDropGold(ptInit->ptGame, ptInit->hRoom, &tCoord);
		}
	}
}

static __inline int ObjRandI(D2SEED *ptSeed, int n)
{
	if (n)
		return (int)D2CommonSeedRand(ptSeed, n);
	return 0;
}

int __fastcall ObjectDrawShrine(GAME *ptGame, BYTE bClass, int nLevelId, GAMEROOM *hRoom);

void __fastcall ObjectInitShrine(OBJINIT *ptInit)
{
	int nParm;
	int nLevelId;
	DWORD bClass;
	int nShrine;
	int nTries;
	int nCount;
	SHRINESTXT *ptShrine;

	nParm = ptInit->ptTypeData->Param[0];
	nLevelId = (int)D2CommonDungeonRoomGetLevelId(ptInit->hRoom);
	if (nParm != 0)
	{
		switch (nParm)
		{
		case 1:
			bClass = 2;
			break;
		case 2:
			bClass = 3;
			break;
		default:
			bClass = 4;
			if (!D2CommonSeedRand(ptInit->ptSeed, 10))
				bClass = 1;
			break;
		}
		nShrine = ObjectDrawShrine(ptInit->ptGame, bClass, nLevelId, ptInit->hRoom);
	}
	else
	{
		nTries = 8;
		nLevelId = (int)D2CommonDungeonRoomGetLevelId(ptInit->hRoom);
		do
		{

			nCount = D2CommonObjTblsGetShrineCount() - 1;
			nShrine = ObjRandI(ptInit->ptSeed, nCount) + 1;
			ptShrine = (SHRINESTXT *)D2CommonObjTblsGetShrine(nShrine);
			if ((DWORD)nLevelId >= ptShrine->LevelMin)
				break;
			nTries--;
		} while (nTries > 0);
	}
	if (nShrine == 5)
		nShrine = 3;
	else if (nShrine == 4)
		nShrine = 2;
	else if (nShrine == 16)
		nShrine = 18;
	OBJDATA(ptInit->hObject)->field_04 = (BYTE)nShrine;
	D2CommonUnitSetObjectShrine(ptInit->hObject, D2CommonObjTblsGetShrine(nShrine));
}

int __fastcall ObjectDrawShrine(GAME *ptGame, BYTE bClass, int nLevelId, GAMEROOM *hRoom)
{
	SHRINEPOP *ptShrinePop;
	int nTries;
	int nCount;
	int nIndex;
	D2SEED *ptSeed;
	int nShrine;

	if (bClass == 0 || bClass > 4)
		bClass = 2;
	ptShrinePop = ObjectRgnGetShrinePop(ptGame);
#line 693
	if (ptShrinePop->NumShrineSubTypes[bClass]) {
		nTries = 8;
		do
		{
			nCount = ptShrinePop->NumShrineSubTypes[bClass];
			ptSeed = ObjectRgnGetSeed(ptGame, hRoom);
			nIndex = OBJ_RAND(ptSeed, nCount);
			nShrine = ptShrinePop->apnShrineIds[bClass][nIndex];
			if (!nShrine)
				nShrine = 1;
			if ((DWORD)nLevelId >= ((SHRINESTXT *)D2CommonObjTblsGetShrine(nShrine))->LevelMin)
				return nShrine;
			nTries--;
		} while (nTries > 0);
		return nShrine; }
#line 693
	D2ASSERT(ptShrinePop->NumShrineSubTypes[bClass]);
}
#line 709

void __fastcall ObjectInitModeEvent(OBJINIT *ptInit)
{
	if (ptInit->ptTypeData->Mode[2] && ptInit->hObject->dwMode == 0 && !ptInit->ptTypeData->Mode[0])
		D2CommonUnitSetMode(ptInit->hObject, 2);
	EventAdd(ptInit->ptGame, ptInit->hObject, 0, ptInit->ptGame->nFrame + 25);
}

void __fastcall ObjectInitWell(OBJINIT *ptInit)
{
	OBJDATA(ptInit->hObject)->field_04 = (BYTE)((BYTE)ptInit->ptTypeData->Param[2] << 1);
}

void __fastcall ObjectInitFloorTrap(OBJINIT *ptInit)
{
	EventAdd(ptInit->ptGame, ptInit->hObject, 3, ptInit->ptGame->nFrame + 25);
}

void __fastcall ObjectInitChestLevel(OBJINIT *ptInit)
{
	BYTE bValue;
	MISSLEVELDATA *ptLevel;

	bValue = 0;
	ptLevel = (MISSLEVELDATA *)D2CommonLvlTblsGetLevelData(D2CommonDungeonRoomGetLevelId(ptInit->hRoom));
	if (D2CommonSeedRand(ptInit->ptSeed, 100) < (WORD)((BYTE)(ptLevel->MonLvl[0] >> 3) + 5))
		bValue = (BYTE)D2CommonSeedBetween(ptInit->ptSeed, 1, 9);
	OBJDATA(ptInit->hObject)->field_04 = bValue;
}

void __fastcall ObjectInitChestRare(OBJINIT *ptInit)
{
	BYTE bValue;

	bValue = 0;
	if (D2CommonSeedRand(ptInit->ptSeed, 1000) < 333)
		bValue = 3;
	OBJDATA(ptInit->hObject)->field_04 = bValue;
}

void __fastcall ObjectInitChest(OBJINIT *ptInit)
{
	BYTE bValue;
	MISSLEVELDATA *ptLevel;

	bValue = 0;
	ptLevel = (MISSLEVELDATA *)D2CommonLvlTblsGetLevelData(D2CommonDungeonRoomGetLevelId(ptInit->hRoom));
	if (D2CommonSeedRand(ptInit->ptSeed, 100) < (WORD)((BYTE)(ptLevel->MonLvl[0] >> 3) + 5))
		bValue = (BYTE)D2CommonSeedBetween(ptInit->ptSeed, 1, 9);
	OBJDATA(ptInit->hObject)->field_04 = bValue;
	bValue = OBJDATA(ptInit->hObject)->field_04;
	ptLevel = (MISSLEVELDATA *)D2CommonLvlTblsGetLevelData(D2CommonDungeonRoomGetLevelId(ptInit->hRoom));
	if (ptInit->ptTypeData->Lockable
		&& (int)D2CommonSeedRand(ptInit->ptSeed, 100) < (ptLevel->MonLvl[0] >> 1) + 8)
		bValue |= 128;
	else
		bValue &= 127;
	D2CommonSeedSet(&ptInit->hObject->tSeed,
		D2CommonSeedRand(ptInit->ptSeed, 65534) + 1);
	OBJDATA(ptInit->hObject)->field_04 = bValue;
}

void __fastcall ObjectInitChestFlagged(OBJINIT *ptInit)
{
	BYTE bValue;
	MISSLEVELDATA *ptLevel;

	bValue = 0;
	ptLevel = (MISSLEVELDATA *)D2CommonLvlTblsGetLevelData(D2CommonDungeonRoomGetLevelId(ptInit->hRoom));
	if (D2CommonSeedRand(ptInit->ptSeed, 100) < (WORD)((BYTE)(ptLevel->MonLvl[0] >> 3) + 5))
		bValue = (BYTE)D2CommonSeedBetween(ptInit->ptSeed, 1, 9);
	OBJDATA(ptInit->hObject)->field_04 = bValue;
	bValue = OBJDATA(ptInit->hObject)->field_04;
	ptLevel = (MISSLEVELDATA *)D2CommonLvlTblsGetLevelData(D2CommonDungeonRoomGetLevelId(ptInit->hRoom));
	if (ptInit->ptTypeData->Lockable
		&& (int)D2CommonSeedRand(ptInit->ptSeed, 100) < (ptLevel->MonLvl[0] >> 1) + 8)
		bValue |= 128;
	else
		bValue &= 127;
	D2CommonSeedSet(&ptInit->hObject->tSeed,
		D2CommonSeedRand(ptInit->ptSeed, 65534) + 1);
	OBJDATA(ptInit->hObject)->field_04 = bValue;
	SUnitSetObjectFlags(ptInit->hObject, 1);
}

void __fastcall ObjectInitRandomEvent(OBJINIT *ptInit)
{
	GAME *ptGame;

	ptGame = ptInit->ptGame;
	EventAdd(ptGame, ptInit->hObject, 8,
		(int)D2CommonSeedRand(ObjectRgnGetSeed(ptGame, ptInit->hRoom), 250) + ptGame->nFrame + 25);
}

void __fastcall ObjectInitVileDogAfterglow(OBJINIT *ptInit)
{
	if (ptInit->hObject->dwMode == 0)
	{
		D2CommonUnitSetMode(ptInit->hObject, 1);
		EventAdd(ptInit->ptGame, ptInit->hObject, 7, ptInit->ptGame->nFrame + 27);
	}
}

void __fastcall ObjectInitTargetLevel(OBJINIT *ptInit)
{
	int nLevelId;
	int nDrawn;

	nLevelId = (int)D2CommonDungeonRoomGetLevelId(ptInit->hRoom);
	if (ptInit->hObject->dwMode == 0)
	{
		D2CommonUnitSetMode(ptInit->hObject, 1);
		EventAdd(ptInit->ptGame, ptInit->hObject, 1,
			(int)((DWORD)ptInit->ptTypeData->FrameCnt[1] >> 8) + ptInit->ptGame->nFrame + 1);
	}
	nDrawn = D2CommonDungeonActGetDrawnLevel(ptInit->ptGame->ptAct[1]);
	switch (nLevelId)
	{
	case 1:
		OBJDATA(ptInit->hObject)->field_04 = 39;
		return;
	case 39:
		OBJDATA(ptInit->hObject)->field_04 = 1;
		return;
	case 38:
		OBJDATA(ptInit->hObject)->field_04 = 4;
		return;
	case 4:
		OBJDATA(ptInit->hObject)->field_04 = 38;
		return;
	case 74:
		OBJDATA(ptInit->hObject)->field_04 = 46;
		return;
	case 46:
		OBJDATA(ptInit->hObject)->field_04 = 74;
		return;
	case 73:
		OBJDATA(ptInit->hObject)->field_04 = (BYTE)nDrawn;
		return;
	}
	if (nLevelId == nDrawn)
		OBJDATA(ptInit->hObject)->field_04 = 73;
}

void __fastcall ObjectInitOpened(OBJINIT *ptInit)
{
	D2CommonUnitSetMode(ptInit->hObject, 2);
}

void __fastcall ObjectInitOperating(OBJINIT *ptInit)
{
	D2CommonUnitSetMode(ptInit->hObject, 1);
}

void __fastcall ObjectInitOperatingRandom(OBJINIT *ptInit)
{
	D2SEED *ptSeed = ObjectRgnGetSeed(ptInit->ptGame, ptInit->hRoom);

	if (D2CommonSeedRand(ptSeed, 2))
		D2CommonUnitSetMode(ptInit->hObject, 1);
}

void __fastcall ObjectInitTownLevel(OBJINIT *ptInit)
{
	BYTE bAct;
	int eLevelId;

	bAct = D2CommonGetActNum(D2CommonDungeonRoomGetLevelId(ptInit->hRoom));
	eLevelId = (int)((DWORD (__stdcall *)(BYTE))D2CommonDungeonActGetTownLevelIdRaw)(bAct);
#line 1048
	D2ASSERT(eLevelId <= 255);
	OBJDATA(ptInit->hObject)->field_04 = (BYTE)eLevelId;
	if (ptInit->hObject->dwMode == 1)
		EventAdd(ptInit->ptGame, ptInit->hObject, 1,
			(int)((DWORD)ptInit->ptTypeData->FrameCnt[1] >> 8) + ptInit->ptGame->nFrame + 1);
}

void __fastcall ObjectInitGuildA(OBJINIT *ptInit)
{
	void *ptGuild;
	BOOL fNone;

	ptGuild = GuildsGetData(ptInit->ptGame);
	if (ptGuild)
	{
		fNone = (int)D2CommonGuildsGetVaultUnitId(ptGuild) == -1;
		ptInit->hObject->ptInventory = (void *)D2CommonGuildsGetVaultInventory(ptGuild);
		D2CommonGuildsSetVaultUnitId(ptGuild, ptInit->hObject->dwId);
		if (fNone)
			GuildsLoadVaultItems(ptInit->ptGame, ptInit->hObject);
	}
}

void __fastcall ObjectInitGuildB(OBJINIT *ptInit)
{
	void *ptGuild;
	BOOL fNone;

	ptGuild = GuildsGetData(ptInit->ptGame);
	if (ptGuild)
	{
		fNone = (int)D2CommonGuildsGetVaultUnitId(ptGuild) == -1;
		ptInit->hObject->ptInventory = (void *)D2CommonGuildsGetTrophyCaseInventory(ptGuild);
		D2CommonGuildsSetTrophyCaseUnitId(ptGuild, ptInit->hObject->dwId);
		if (fNone)
			GuildsLoadTrophyCaseItems(ptInit->ptGame, ptInit->hObject);
	}
}

void __fastcall ObjectInitGuildC(OBJINIT *ptInit)
{
	void *ptGuild;

	ptGuild = GuildsGetData(ptInit->ptGame);
	if (ptGuild)
		D2CommonGuildsSetMessageBoardUnitId(ptGuild, ptInit->hObject->dwId);
}

void __fastcall ObjectInitTrappedSoul(OBJINIT *ptInit)
{
	if (ptInit->hObject->dwMode == 2 || ptInit->hObject->dwMode == 1)
		EventAdd(ptInit->ptGame, ptInit->hObject, 9, ptInit->ptGame->nFrame + 35);
}

static __inline BOOL ObjectSpawnClearI(GAMEROOM *hRoom, int nSizeY, COLPOINT *ptPos, int nSizeX, GAMERECT *ptRect)
{
	if (ptRect->nWidth >= nSizeX + 2 && ptRect->nHeight >= nSizeY + 2)
	{
		if ((ptPos->nX & 65535) > ptRect->nX + 1 && (ptPos->nY & 65535) > ptRect->nY + 1
			&& (ptPos->nX & 65535) < ptRect->nX - nSizeX - 2 + ptRect->nWidth
			&& (ptPos->nY & 65535) < ptRect->nY - nSizeY - 2 + ptRect->nHeight
			&& !D2CommonCollisnTestArea(hRoom, ptPos->nX, ptPos->nY, nSizeX + 7, nSizeY + 7, 3073)
			&& !D2CommonCollisnTestArea(hRoom, ptPos->nX, ptPos->nY, nSizeX, nSizeY, 7953))
			return TRUE;
		return FALSE;
	}
	return FALSE;
}

void __fastcall ObjectInitSpawnGroup(OBJINIT *ptInit)
{
	int nR07; GAMERECT atRect[2];
	D2SEED *ptSeed;
	int nCount;
	int nTries;
	COLPOINT tSpot;
	int nClass;
	int nGroup;
	BOOL fFound;
	int nStep;
	int nLimit;
	int nDir;

	if (ptInit->hObject->dwMode != 0)
		return;
	D2CommonDungeonRoomCopyRects(ptInit->hRoom, atRect);
	ptSeed = ObjectRgnGetSeed(ptInit->ptGame, ptInit->hRoom);
	nTries = 12;
	nCount = (((atRect[0].nWidth * atRect[0].nHeight) >> 7) * 30) >> 8;
	while (nCount > 0 && nTries > 0)
	{
		nR07 = atRect[0].nWidth - 4; tSpot.nX = OBJ_RAND(ptSeed, nR07) + atRect[0].nX;
		nR07 = atRect[0].nHeight - 4; tSpot.nY = OBJ_RAND(ptSeed, nR07) + atRect[0].nY;
		if (ObjectSpawnClearI(ptInit->hRoom, 3, &tSpot, 3, atRect))
		{
			nClass = 403;
			if (D2CommonSeedNext(ptInit->ptSeed) & 1)
				nClass = 404;
			if (MonsterSpawn(ptInit->ptGame, ptInit->hRoom, ptInit->nX, ptInit->nY, nClass, 1, -1, 0))
			{
				D2CommonUnitSetMode(ptInit->hObject, 2);
				nCount--;
			}
			nGroup = 1;
			fFound = TRUE;
			while (!ObjRandI(ptSeed, nGroup >> 1) && fFound)
			{
				fFound = FALSE;
				nStep = 0;
				nLimit = (nCount < 4 ? 4 : nCount) * 3;
				while (!fFound && nStep < nLimit)
				{
					nDir = D2CommonSeedNext(ptSeed) & 7;
					tSpot.nX += ((int)D2CommonSeedRand(ptSeed, 5) + 5) * sganDirX[nDir] * 2;
					tSpot.nY += ((int)D2CommonSeedRand(ptSeed, 5) + 5) * sganDirY[nDir] * 2;
					fFound = ObjectSpawnClearI(ptInit->hRoom, 3, &tSpot, 3, atRect);
					nStep++;
				}
				if (fFound)
				{
					nClass = 403;
					if (D2CommonSeedNext(ptInit->ptSeed) & 1)
						nClass = 404;
					nGroup++;
					if (MonsterSpawn(ptInit->ptGame, ptInit->hRoom, tSpot.nX, tSpot.nY, nClass, 1, -1, 0))
					{
						D2CommonUnitSetMode(ptInit->hObject, 2);
						nCount--;
					}
				}
			}
		}
		nTries--;
	}
}

static __inline BOOL ObjectSpawnClearSmallI(GAMEROOM *hRoom, int nSizeX, int nSizeY, COLPOINT *ptPos,
	GAMERECT *ptRect)
{
	if (ptRect->nWidth >= 2 && ptRect->nHeight >= 2)
	{
		if ((ptPos->nX & 65535) > ptRect->nX + 2 && (ptPos->nY & 65535) > ptRect->nY + 2
			&& (ptPos->nX & 65535) < ptRect->nX - nSizeX + ptRect->nWidth
			&& (ptPos->nY & 65535) < ptRect->nY - nSizeY + ptRect->nHeight
			&& !D2CommonCollisnTestArea(hRoom, ptPos->nX, ptPos->nY, nSizeX + 2, nSizeY + 2, 7953))
			return TRUE;
		return FALSE;
	}
	return FALSE;
}

static int sganCluster4f[3] = { 79, 53, 1 };
static int sganCluster03[2] = { 3, 28 };
static int sganCluster04[5] = { 4, 9, 52, 94, 95 };
static int sganCluster59[2] = { 89, 284 };
static int sganClusterD0[2] = { 208, 209 };
static int sgnCluster4f = 3;
static int sgnCluster03 = 2;
static int sgnCluster04 = 5;
static int sgnCluster59 = 2;
static int sgnClusterD0 = 2;

UNIT *__fastcall ObjectPopulateCluster(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance)
{
	MISSOBJECTSTXT *ptTxt;
	int nSizeX;
	int nSizeY;
	GAMERECT atRect[2];
	D2SEED *ptSeed;
	const int *pnList;
	int nList;
	int nRange;
	int nSpan;
	int nBase;
	int nTries;
	BYTE fSmall;
	int nCount;
	UNIT *hResult;
	int nSpawn;
	COLPOINT tSpot;
	int nGroup;
	BOOL fFound;
	int nStep;
	int nLimit;
	int nDir;

	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(nClass);
	nSizeX = ptTxt->SizeX;
	nSizeY = ptTxt->SizeY;
	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	ptSeed = ObjectRgnGetSeed(ptGame, hRoom);
	if (D2CommonSeedRand(ptSeed, 100) > wChance)
		return NULL;
	nTries = 12;
	fSmall = 0;
	if (nClass == 3)
	{
		pnList = sganCluster03;
		nList = sgnCluster03;
		nRange = 5;
		nBase = 5;
		nTries = 18;
	}
	else if (nClass == 79 || nClass == 1)
	{
		pnList = sganCluster4f;
		nList = sgnCluster4f;
		nRange = 5;
		nBase = 5;
	}
	else if (nClass == 4)
	{
		pnList = sganCluster04;
		nList = sgnCluster04;
		nRange = 0;
		nBase = 1;
		fSmall = 1;
	}
	else if (nClass == 89)
	{
		pnList = sganCluster59;
		nList = sgnCluster59;
		nRange = 5;
		nBase = 5;
	}
	else if (nClass == 208 || nClass == 209)
	{
		pnList = sganClusterD0;
		nList = sgnClusterD0;
		nRange = 0;
		nBase = 1;
		fSmall = 1;
	}
	else
		return NULL;
	nCount = (((atRect[0].nWidth * atRect[0].nHeight) >> 7) * bDensity) >> 8;
	hResult = NULL;
	while (nCount > 0 && nTries > 0)
	{
		nSpawn = pnList[OBJ_RAND(ptSeed, nList)];
		nSpan = atRect[0].nWidth - nSizeX - 1;
		tSpot.nX = OBJ_RAND(ptSeed, nSpan) + atRect[0].nX;
		nSpan = atRect[0].nHeight - nSizeY - 1;
		tSpot.nY = OBJ_RAND(ptSeed, nSpan) + atRect[0].nY;
		if (ObjectSpawnClearI(hRoom, ptTxt->SizeY, &tSpot, ptTxt->SizeX, atRect))
		{
			SUnitCreate(UNIT_OBJECT, nSpawn, tSpot.nX, tSpot.nY, ptGame, hRoom, 1, 0, 0);
			nGroup = 1;
			hResult = NULL;
			fFound = TRUE;
			while (!ObjRandI(ptSeed, nGroup >> 1) && fFound)
			{
				nStep = 0;
				fFound = FALSE;
				nLimit = (nCount < 4 ? 4 : nCount) * 3;
				while (!fFound && nStep < nLimit)
				{
					nDir = (int)D2CommonSeedRand(ptSeed, 8);
					tSpot.nX += sganDirX[nDir] * (OBJ_RAND(ptSeed, nRange) + nBase) * 2;
					tSpot.nY += sganDirY[nDir] * (OBJ_RAND(ptSeed, nRange) + nBase) * 2;
					if (fSmall)
						fFound = ObjectSpawnClearSmallI(hRoom, ptTxt->SizeX, ptTxt->SizeY, &tSpot, atRect);
					else
						fFound = ObjectSpawnClearI(hRoom, ptTxt->SizeY, &tSpot, ptTxt->SizeX, atRect);
					nStep++;
				}
				if (fFound)
				{
					nSpawn = pnList[OBJ_RAND(ptSeed, nList)];
					hResult = SUnitCreate(UNIT_OBJECT, nSpawn, tSpot.nX, tSpot.nY, ptGame, hRoom, 1, 0, 0);
					nGroup++;
				}
			}
			nCount--;
		}
		nTries--;
	}
	return hResult;
}

static OBJPOINT sgatPoints0[6] =
	{ { -4, 0 }, { 0, 0 }, { 4, 0 }, { 8, 0 }, { 0, 4 }, { 0, -4 } };
static int sgnPoints0 = 6;
static OBJPOINT sgatPoints1[5] =
	{ { -8, 0 }, { -4, 0 }, { 0, 0 }, { 4, 0 }, { 8, 0 } };
static int sgnPoints1 = 5;
static OBJPOINT sgatPoints2[5] =
	{ { 0, -8 }, { 0, -4 }, { 0, 0 }, { 0, 4 }, { 0, 8 } };
static int sgnPoints2 = 5;
static OBJPOINT sgatPoints3[7] =
	{ { -7, -5 }, { -5, -3 }, { -3, -1 }, { 0, 0 }, { 3, -1 }, { 5, -3 }, { 7, -5 } };
static int sgnPoints3 = 7;
static OBJCLUSTER sgatClusters[4] =
{
	{ sgatPoints0, sgnPoints0 },
	{ sgatPoints1, sgnPoints1 },
	{ sgatPoints2, sgnPoints2 },
	{ sgatPoints3, sgnPoints3 },
};
static int sgnClusters = 4;

UNIT *__fastcall ObjectPopulateCorpses(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance)
{
	D2SEED *ptSeed;
	int nCluster;
	MISSOBJECTSTXT *ptTxt;
	int nSizeX;
	int nSizeY;
	GAMERECT atRect[2];
	int nTries;
	UNIT *hResult;
	COLPOINT tSpot;
	int i;
	COLPOINT tPoint;
	DWORD UnitIDType;
	GAMEROOM *hObjRoom;
	COLPOINT tPos;

	ptSeed = ObjectRgnGetSeed(ptGame, hRoom);
	if (D2CommonSeedRand(ptSeed, 100) > wChance)
		return NULL;
	nCluster = OBJ_RAND(ptSeed, sgnClusters);
	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(nClass);
	nSizeX = ptTxt->SizeX;
	nSizeY = ptTxt->SizeY;
	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	nTries = 8;
	hResult = NULL;
	for (;;)
	{
		tSpot.nX = atRect[0].nX + ObjRandI(ptSeed, atRect[0].nWidth - nSizeX - 1);
		tSpot.nY = atRect[0].nY + ObjRandI(ptSeed, atRect[0].nHeight - nSizeY - 1);
		if (ObjectSpawnClearI(hRoom, 5, &tSpot, 5, atRect))
			break;
		nTries--;
		if (nTries <= 0)
			return NULL;
	}
	for (i = 0; i < sgatClusters[nCluster].nPoints; i++)
	{
		tPoint.nX = sgatClusters[nCluster].ptPoints[i].nX + tSpot.nX;
		tPoint.nY = sgatClusters[nCluster].ptPoints[i].nY + tSpot.nY;
		if (ObjectSpawnClearI(hRoom, nSizeY, &tPoint, nSizeX, atRect))
		{
			UnitIDType = D2CommonSeedRand(ptSeed, 2) + 57;
#line 1918
			D2ASSERT(UnitIDType <= OBJECTCLASS_ROGUESTAKEDCORPSE2);
			hResult = SUnitCreate(UNIT_OBJECT, UnitIDType, tPoint.nX, tPoint.nY, ptGame, hRoom, 1, 0, 0);
			if (hResult)
			{
				hObjRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hResult);
				if (D2CommonSeedRand(ObjectRgnGetSeed(ptGame, hObjRoom), 100) > 70)
				{
					D2CommonUnitGetPosition(hResult, &tPos);
					SUnitCreate(UNIT_OBJECT, 103, tPos.nX, tPos.nY, ptGame, hObjRoom, 1, 0, 0);
				}
			}
		}
	}
	return hResult;
}

UNIT *__fastcall ObjectPlaceInRoom(GAME *ptGame, int nClass, int nSizeX, int nSizeY, GAMEROOM *hRoom);

UNIT *__fastcall ObjectPopulateScatter(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance)
{
	MISSOBJECTSTXT *ptTxt;
	int nSizeX;
	int nSizeY;
	GAMERECT atRect[2];
	int nCount;
	UNIT *hObject;

#line 2052
	D2ASSERT(bDensity <= 128);
	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(nClass);
	nSizeX = ptTxt->SizeX;
	nSizeY = ptTxt->SizeY;
	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	if (D2CommonSeedRand(ObjectRgnGetSeed(ptGame, hRoom), 100) > wChance)
		return NULL;
	nCount = (((atRect[0].nWidth * atRect[0].nHeight) >> 7) * bDensity) >> 8;
	hObject = NULL;
	for (; nCount > 0; nCount--)
	{
		hObject = ObjectPlaceInRoom(ptGame, nClass, nSizeX, nSizeY, hRoom);
		if (hObject)
		{
			if (ptTxt->Selectable[hObject->dwMode])
				hObject->dwFlags0 |= 2;
			else
				hObject->dwFlags0 &= ~2u;
		}
	}
	return hObject;
}

UNIT *__fastcall ObjectPlaceInRoom(GAME *ptGame, int nClass, int nSizeX, int nSizeY, GAMEROOM *hRoom)
{
	GAMERECT atRect[2];
	D2SEED *ptSeed;
	int i;
	int nX;
	int nY;
	int nRange;

	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	if (atRect[0].nWidth < 2 || atRect[0].nHeight < 2)
		return NULL;
	ptSeed = ObjectRgnGetSeed(ptGame, hRoom);
	for (i = 0; i < 5; i++)
	{
		nRange = atRect[0].nWidth - nSizeX - 1;
		nX = OBJ_RAND(ptSeed, nRange) + atRect[0].nX;
		nRange = atRect[0].nHeight - nSizeY - 1;
		nY = OBJ_RAND(ptSeed, nRange) + atRect[0].nY;
		if (nX && nY && (nX & 65535) >= atRect[0].nX + 1 && (nY & 65535) >= atRect[0].nY + 1
			&& (nX & 65535) < atRect[0].nX + atRect[0].nWidth - 1
			&& (nY & 65535) < atRect[0].nY + atRect[0].nHeight - 1
			&& !D2CommonCollisnTestArea(hRoom, nX, nY, nSizeX + 6, nSizeY + 6, 7953))
			break;
	}
	if (i >= 5)
		return NULL;
	return SUnitCreate(UNIT_OBJECT, nClass, nX, nY, ptGame, hRoom, 1, 0, 0);
}

UNIT *__fastcall ObjectPopulateOne(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance)
{
	MISSOBJECTSTXT *ptTxt;
	int nSizeX;
	int nSizeY;
	GAMERECT atRect[2];
	int nCount;
	UNIT *hObject;

#line 2099
	D2ASSERT(bDensity <= 128);
	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(nClass);
	nSizeX = ptTxt->SizeX;
	nSizeY = ptTxt->SizeY;
	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	if (D2CommonSeedRand(hRoom ? &ROOM5(hRoom)->tSeed : NULL, 100) > wChance)
		return NULL;
	for (nCount = (((atRect[0].nWidth * atRect[0].nHeight) >> 7) * bDensity) >> 8; nCount > 0; nCount--)
	{
		hObject = ObjectPlaceInRoom(ptGame, nClass, nSizeX, nSizeY, hRoom);
		if (hObject)
		{
			if (ptTxt->Selectable[hObject->dwMode])
				hObject->dwFlags0 |= 2;
			else
				hObject->dwFlags0 &= ~2u;
			return hObject;
		}
	}
	return NULL;
}

UNIT *__fastcall ObjectPopulateScatterExtra(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance)
{
	UNIT *hObject;
	GAMEROOM *hObjRoom;
	COLPOINT tPos;

	hObject = ObjectPopulateScatter(ptGame, hRoom, bDensity, nClass, wChance);
	if (hObject)
	{
		hObjRoom = (GAMEROOM *)D2CommonUnitGetPathRoom(hObject);
		if (D2CommonSeedRand(ObjectRgnGetSeed(ptGame, hObjRoom), 100) > 70)
		{
			D2CommonUnitGetPosition(hObject, &tPos);
			SUnitCreate(UNIT_OBJECT, 103, tPos.nX, tPos.nY, ptGame, hObjRoom, 1, 0, 0);
		}
	}
	return hObject;
}

static BOOL ObjectFindSpot(GAME *ptGame, GAMEROOM *hRoom, int nLevelId, int nSizeX, int nSizeY,
	PFNOBJFILTER ptFilter  , COLPOINT *ptSpot)
{
	int nRange;
	GAMERECT atRect[2];
	D2SEED *ptSeed;
	int i;
	int nX;
	int nY;

	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	if (atRect[0].nWidth < 2 || atRect[0].nHeight < 2)
		return FALSE;
	if (hRoom)
		ptSeed = &ROOM5(hRoom)->tSeed;
	else
		ptSeed = NULL;
	for (i = 0; i < 5; i++)
	{
		nRange = atRect[0].nWidth - nSizeX - 1;
		nX = OBJ_RAND(ptSeed, nRange) + atRect[0].nX;
		nRange = atRect[0].nHeight - nSizeY - 1;
		nY = OBJ_RAND(ptSeed, nRange) + atRect[0].nY;
		if (!nX || !nY || (nX & 65535) < atRect[0].nX + 1 || (nY & 65535) < atRect[0].nY + 1
			|| (nX & 65535) >= atRect[0].nX + atRect[0].nWidth - 1
			|| (nY & 65535) >= atRect[0].nY + atRect[0].nHeight - 1)
			continue;
#line 1326
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)ptFilter), "ptFilter");
		if (!D2CommonCollisnTestArea(hRoom, nX, nY, nSizeX + 6, nSizeY + 6, 7953)
			&& ptFilter(ptGame, nLevelId, nX, nY))
			break;
	}
	if (i >= 5)
		return FALSE;
	ptSpot->nX = nX;
	ptSpot->nY = nY;
	return TRUE;
}

UNIT *__fastcall ObjectPopulateFiltered(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance)
{
	int nLevelId;
	MISSOBJECTSTXT *ptTxt;
	int nSizeX;
	int nSizeY;
	GAMERECT atRect[2];
	int nCount;
	COLPOINT tSpot;
	UNIT *hObject;
	int nX;
	int nY;

	nLevelId = (int)D2CommonDungeonRoomGetLevelIdChecked(hRoom);
	if (ObjectRgnSpotsFull(ptGame, nLevelId))
		return NULL;
#line 2160
	D2ASSERT(bDensity <= 128);
	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(nClass);
	nSizeX = ptTxt->SizeX;
	nSizeY = ptTxt->SizeY;
	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	if (D2CommonSeedRand(hRoom ? &ROOM5(hRoom)->tSeed : NULL, 100) > wChance)
		return NULL;
	nCount = 1;
	nSizeY = nSizeY * 2 + 1;
	nSizeX = nSizeX * 2 + 1;
	hObject = NULL;
	for (; nCount > 0; nCount--)
	{
		if (!ObjectFindSpot(ptGame, hRoom, nLevelId, nSizeX, nSizeY, ObjectRgnSpotIsFar, &tSpot))
		{
			hObject = NULL;
			continue;
		}
		hObject = SUnitCreate(UNIT_OBJECT, nClass, tSpot.nX, tSpot.nY, ptGame, hRoom, 1, 0, 0);
		if (hObject)
		{
			if (ptTxt->Selectable[hObject->dwMode])
				hObject->dwFlags0 |= 2;
			else
				hObject->dwFlags0 &= ~2u;
			nY = (int)D2CommonUnitGetY(hObject);

			ObjectRgnAddSpot(ptGame, nLevelId, ((int)D2CommonUnitGetX(hObject)), nY);
		}
	}
	return hObject;
}

extern BOOL ObjectFindOrientedSpot(GAME *ptGame, GAMEROOM *hRoom, int nLevelId, int nOrient, int nSizeX,
	int nSizeY, PFNOBJFILTER ptFilter  , COLPOINT *ptSpot);
__inline BOOL ObjectFindOrientedSpot(GAME *ptGame, GAMEROOM *hRoom, int nLevelId, int nOrient, int nSizeX,
	int nSizeY, PFNOBJFILTER ptFilter  , COLPOINT *ptSpot)
{
	int nRange;
	GAMERECT atRect[2];
	D2SEED *ptSeed;
	int i;
	int nX;
	int nY;

	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	if (atRect[0].nWidth < nSizeX + 2 || atRect[0].nHeight < nSizeY + 2)
		return FALSE;
	if (hRoom)
		ptSeed = &ROOM5(hRoom)->tSeed;
	else
		ptSeed = NULL;
	for (i = 0; i < 5; i++)
	{
		switch (nOrient)
		{
		case 1:
			nX = atRect[0].nWidth / 4 + OBJ_RAND(ptSeed, atRect[0].nWidth / 2) + atRect[0].nX;
			nY = D2CommonSeedBetween(ptSeed, 1, 2) + atRect[0].nY;
			break;
		case 2:
			nX = D2CommonSeedBetween(ptSeed, 1, 2) + atRect[0].nX;
			nY = atRect[0].nHeight / 4 + OBJ_RAND(ptSeed, atRect[0].nHeight / 2) + atRect[0].nY;
			break;
		default:
			nRange = atRect[0].nWidth - nSizeX - 1;
			nX = OBJ_RAND(ptSeed, nRange) + atRect[0].nX;
			nRange = atRect[0].nHeight - nSizeY - 1;
			nY = OBJ_RAND(ptSeed, nRange) + atRect[0].nY;
			break;
		}
		if (!nX || !nY || (nX & 65535) < atRect[0].nX + 1 || (nY & 65535) < atRect[0].nY + 1
			|| (nX & 65535) >= atRect[0].nX + atRect[0].nWidth - 1
			|| (nY & 65535) >= atRect[0].nY + atRect[0].nHeight - 1)
			continue;
#line 1497
		D2ASSERTMSG(!IsBadCodePtr((FARPROC)ptFilter), "ptFilter");
		if (!D2CommonCollisnTestArea(hRoom, nX, nY, nSizeX + 6, nSizeY + 6, 7953)
			&& ptFilter(ptGame, nLevelId, nX, nY))
			break;
	}
	if (i >= 5)
		return FALSE;
	ptSpot->nX = nX;
	ptSpot->nY = nY;
	return TRUE;
}

UNIT *__fastcall ObjectPopulateOriented(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance)
{
	MISSOBJECTSTXT *ptTxt;
	int nSizeX;
	int nSizeY;
	GAMERECT atRect[2];
	int nLevelId;
	int nRoll;
	int nTries;
	BYTE fForced;
	COLPOINT tSpot;
	UNIT *hObject;
	int nParm;
	int nNew;
	int nX;
	int nY;

	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(nClass);
	nSizeX = ptTxt->SizeX;
	nSizeY = ptTxt->SizeY;
	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	nLevelId = (int)D2CommonDungeonRoomGetLevelIdChecked(hRoom);
	nRoll = (int)D2CommonSeedRand(hRoom ? &ROOM5(hRoom)->tSeed : NULL, 100);
	nTries = 3;
	fForced = 0;
	if (ObjectRgnLevelPopulated(ptGame, nLevelId))
	{
		fForced = 1;
		nTries = 30;
	}
	else if ((DWORD)nRoll > wChance)
		return NULL;
	if (ObjectRgnTestOrientedSpotsFull(ptGame, nLevelId))
		return NULL;
	for (; nTries > 0; nTries--)
	{
		if (!ObjectFindOrientedSpot(ptGame, hRoom, nLevelId, ptTxt->Orientation, nSizeX, nSizeY,
			ObjectRgnIsOrientedSpotFar, &tSpot))
			continue;
		hObject = SUnitCreate(UNIT_OBJECT, nClass, tSpot.nX, tSpot.nY, ptGame, hRoom, 1, 0, 0);
		if (!hObject)
			continue;
		{
		DWORD *pdwFlags = &hObject->dwFlags0;
		if (ptTxt->Selectable[hObject->dwMode])
			*pdwFlags |= 2;
		else
			*pdwFlags &= ~2u;
		}
		if (OBJDATA(hObject)->field_04 == 2)
			ObjectRgnCount(ptGame, nLevelId);
		else if (fForced)
		{
			nParm = ((MISSOBJECTSTXT *)D2CommonObjTblsGetObject(hObject->dwClassId))->Param[1];
			nNew = 84;
			if (nParm && nParm < NUM_OBJECTS)
				nNew = nParm;
			hObject->dwClassId = nNew;
			ObjectDrawShrine(ptGame, 2, LEVEL_ACT_1_TOWN, hRoom);
			OBJDATA(hObject)->field_04 = 2;
			D2CommonUnitSetObjectShrine(hObject, D2CommonObjTblsGetShrine(2));
			ObjectRgnCount(ptGame, nLevelId);
		}
		nY = (int)D2CommonUnitGetY(hObject);

		ObjectRgnAddOrientedSpot(ptGame, nLevelId, ((int)D2CommonUnitGetX(hObject)), nY);
		return hObject;
	}
	return NULL;
}

UNIT *__fastcall ObjectPopulateStacks(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance)
{
	MISSOBJECTSTXT *ptTxt;
	int nCreated;
	D2SEED *ptSeed;
	GAMERECT atRect[2];
	int nCount;
	UNIT *hResult;
	int nTries;
	int nSpawn;
	COLPOINT tSpot;
	UNIT *hNew;
	int nGroup;
	BOOL fFound;
	int nStep;
	int nDir;

	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(7);
#line 2293
	D2ASSERT(bDensity <= 128);
	nCreated = 0;
	ptSeed = ObjectRgnGetSeed(ptGame, hRoom);
	if (D2CommonSeedRand(ptSeed, 100) > wChance)
		return NULL;
	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	nCount = (((atRect[0].nWidth * atRect[0].nHeight) >> 7) * bDensity) >> 8;
	hResult = NULL;
	nTries = nCount * 2;
	while (nCount > 0 && nTries > 0)
	{
		if (D2CommonSeedRand(ptSeed, 3))
			nSpawn = 7;
		else
			nSpawn = 11;
		tSpot.nX = OBJ_RAND(ptSeed, atRect[0].nWidth) + atRect[0].nX;
		tSpot.nY = OBJ_RAND(ptSeed, atRect[0].nHeight) + atRect[0].nY;
		if (ObjectSpawnClearSmallI(hRoom, ptTxt->SizeX, ptTxt->SizeY, &tSpot, atRect))
		{
			hNew = SUnitCreate(UNIT_OBJECT, nSpawn, tSpot.nX, tSpot.nY, ptGame, hRoom, 1, 0, 0);
			nGroup = 1;
			if (++nCreated >= 8)
				return hNew;
			hResult = NULL;
			fFound = TRUE;
			while (!ObjRandI(ptSeed, nGroup >> 1) && fFound)
			{
				nStep = 0;
				fFound = FALSE;
				while (!fFound && nStep < 15)
				{
					nDir = D2CommonSeedNext(ptSeed) & 7;
					tSpot.nX += sganDirX[nDir] * ptTxt->XSpace;
					tSpot.nY += sganDirY[nDir] * ptTxt->YSpace;
					fFound = ObjectSpawnClearSmallI(hRoom, ptTxt->SizeX, ptTxt->SizeY, &tSpot, atRect);
					nStep++;
				}
				if (fFound)
				{
					if (D2CommonSeedRand(ptSeed, 4))
						nSpawn = 7;
					else
						nSpawn = 11;
					hResult = SUnitCreate(UNIT_OBJECT, nSpawn, tSpot.nX, tSpot.nY, ptGame, hRoom, 1, 0, 0);
					nCreated++;
					nGroup++;
					if (nCreated >= 8)
						return hResult;
				}
			}
			nCount--;
		}
		nTries--;
	}
	return hResult;
}

UNIT *__fastcall ObjectPopulateGrouped(GAME *ptGame, GAMEROOM *hRoom, BYTE bDensity, int nClass,
	WORD wChance)
{
	MISSOBJECTSTXT *ptTxt;
	D2SEED *ptSeed;
	int nCreated;
	GAMERECT atRect[2];
	int nCount;
	int nTries;
	UNIT *hResult;
	int nLimitBase;
	int nSpawn;
	COLPOINT tSpot;
	int nGroup;
	BOOL fFound;
	int nStep;
	int nLimit;
	int nDir;

	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(nClass);
#line 2421
	D2ASSERT(bDensity <= 128);
	ptSeed = ObjectRgnGetSeed(ptGame, hRoom);
	if (D2CommonSeedRand(ptSeed, 100) > wChance)
		return NULL;
	nCreated = 0;
	D2CommonDungeonRoomCopyRects(hRoom, atRect);
	nCount = (((atRect[0].nWidth * atRect[0].nHeight) >> 7) * bDensity) >> 8;
	nTries = nCount * 2;
	hResult = NULL;
	if (nCount > 0)
		nLimitBase = nCount * 4;
	while (nCount > 0 && nTries > 0)
	{
		nSpawn = nClass;
		if (nClass != 46)
			nSpawn = sganCluster04[OBJ_RAND(ptSeed, sgnCluster04)];
		tSpot.nX = OBJ_RAND(ptSeed, atRect[0].nWidth) + atRect[0].nX;
		tSpot.nY = OBJ_RAND(ptSeed, atRect[0].nHeight) + atRect[0].nY;
		if (ObjectSpawnClearSmallI(hRoom, ptTxt->SizeX, ptTxt->SizeY, &tSpot, atRect))
		{
			if (nSpawn != 46)
				nSpawn = sganCluster04[OBJ_RAND(ptSeed, sgnCluster04)];
			SUnitCreate(UNIT_OBJECT, nSpawn, tSpot.nX, tSpot.nY, ptGame, hRoom, 1, 0, 0);
			nGroup = 1;
			hResult = NULL;
			fFound = TRUE;
			while (!ObjRandI(ptSeed, nGroup >> 1) && fFound)
			{
				nStep = 0;
				fFound = FALSE;
				if (nLimitBase < 4)
					nLimit = 4;
				else
					nLimit = nLimitBase;
				while (!fFound && nStep < nLimit)
				{
					nDir = D2CommonSeedNext(ptSeed) & 7;
					tSpot.nX += sganDirX[nDir] * ptTxt->XSpace;
					tSpot.nY += sganDirY[nDir] * ptTxt->YSpace;
					fFound = ObjectSpawnClearSmallI(hRoom, ptTxt->SizeX, ptTxt->SizeY, &tSpot, atRect);
					nStep++;
				}
				if (fFound)
				{
					hResult = SUnitCreate(UNIT_OBJECT, nSpawn, tSpot.nX, tSpot.nY, ptGame, hRoom, 1, 0, 0);
					if (hResult && ++nCreated >= 8)
						return hResult;
					nGroup++;
				}
			}
			nCount--;
			nLimitBase -= 4;
		}
		nTries--;
	}
	return hResult;
}

static OBJTHEME sgatThemes[7] =
{
	{ FALSE, NULL },
	{ TRUE, ObjectThemeNone },
	{ TRUE, ObjectThemeFill },
	{ FALSE, NULL },
	{ TRUE, ObjectThemeItems },
	{ TRUE, ObjectThemeArmorStands },
	{ TRUE, ObjectThemeWeaponRacks },
};

BOOL __fastcall ObjectThemeNone(GAME *ptGame, GAMEROOM *hRoom, GAMERECT *ptRect)
{
	return TRUE;
}

static __inline BOOL ObjectSpawnClearAtI(GAMEROOM *hRoom, int nSizeX, int nSizeY, DWORD dwX, DWORD dwY,
	GAMERECT *ptRect)
{
	if (ptRect->nWidth >= 2 && ptRect->nHeight >= 2)
	{
		if ((int)(dwX & 65535) > ptRect->nX + 2 && (int)(dwY & 65535) > ptRect->nY + 2
			&& (int)(dwX & 65535) < ptRect->nX - nSizeX + ptRect->nWidth
			&& (int)(dwY & 65535) < ptRect->nY - nSizeY + ptRect->nHeight
			&& !D2CommonCollisnTestArea(hRoom, dwX, dwY, nSizeX + 2, nSizeY + 2, 7953))
			return TRUE;
		return FALSE;
	}
	return FALSE;
}

BOOL __fastcall ObjectThemeFill(GAME *ptGame, GAMEROOM *hRoom, GAMERECT *ptRect)
{
	DWORD dwXEnd;
	DWORD dwYEnd;
	int nCreated;
	MISSOBJECTSTXT *ptTxt;
	D2SEED *ptSeed;
	DWORD dwX;
	DWORD dwY;
	int nSpawn;

	dwXEnd = ptRect->nX + ptRect->nWidth + 1;
	nCreated = 0;
	dwYEnd = ptRect->nY + ptRect->nHeight + 1;
	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(7);
	ptSeed = ObjectRgnGetSeed(ptGame, hRoom);
	for (dwX = ptRect->nX; dwX < dwXEnd; dwX += 2)
	{
		for (dwY = ptRect->nY; dwY < dwYEnd; dwY += 2)
		{
			if (D2CommonSeedRand(ptSeed, 4))
				nSpawn = 7;
			else
				nSpawn = 11;
			if (ObjectSpawnClearAtI(hRoom, ptTxt->SizeX, ptTxt->SizeY, dwX, dwY, ptRect))
			{
				SUnitCreate(UNIT_OBJECT, nSpawn, (int)dwX, (int)dwY, ptGame, hRoom, 1, 0, 0);
				if (++nCreated >= 32)
					return TRUE;
			}
		}
	}
	return TRUE;
}

BOOL __fastcall ObjectThemeItems(GAME *ptGame, GAMEROOM *hRoom, GAMERECT *ptRect)
{
	DWORD dwXEnd;
	DWORD dwYEnd;
	int nCreated;
	DWORD dwX;
	DWORD dwY;
	COLPOINT tPoint;

	dwXEnd = ptRect->nX + (ptRect->nWidth > 12 ? 12 : ptRect->nWidth);
	dwYEnd = ptRect->nY + (ptRect->nHeight > 12 ? 12 : ptRect->nHeight);
	nCreated = 0;
	for (dwX = ptRect->nX + 1; dwX < dwXEnd; dwX += 2)
	{
		for (dwY = ptRect->nY + 1; dwY < dwYEnd; dwY += 2)
		{
			tPoint.nX = (int)dwX;
			tPoint.nY = (int)dwY;
			if (ObjectSpawnClearSmallI(hRoom, 1, 1, &tPoint, ptRect))
			{
				ITEMSDropRandom(ptGame, hRoom, &tPoint, NULL);
				if (++nCreated >= 32)
					return TRUE;
			}
		}
	}
	return TRUE;
}

static int sganRowClasses[4] = { 104, 105, 106, 107 };
static int sgnRows = 2;
static int PLACEHOLDER_UnreferencedData_100abd54 = 64;

BOOL __fastcall ObjectThemeArmorStands(GAME *ptGame, GAMEROOM *hRoom, GAMERECT *ptRect)
{
	ObjectThemeRow(ptGame, hRoom, ptRect, 0);
	return TRUE;
}

static __inline BOOL ObjectSpawnClearXYI(GAMEROOM *hRoom, int nSizeY, int nX, int nY, int nSizeX, GAMERECT *ptRect)
{
	if (ptRect->nWidth >= nSizeX + 2 && ptRect->nHeight >= nSizeY + 2)
	{
		if ((nX & 65535) > ptRect->nX + 1 && (nY & 65535) > ptRect->nY + 1
			&& (nX & 65535) < ptRect->nX - nSizeX - 2 + ptRect->nWidth
			&& (nY & 65535) < ptRect->nY - nSizeY - 2 + ptRect->nHeight
			&& !D2CommonCollisnTestArea(hRoom, nX, nY, nSizeX + 7, nSizeY + 7, 3073)
			&& !D2CommonCollisnTestArea(hRoom, nX, nY, nSizeX, nSizeY, 7953))
			return TRUE;
		return FALSE;
	}
	return FALSE;
}

void __fastcall ObjectThemeRow(GAME *ptGame, GAMEROOM *hRoom, GAMERECT *ptRect, int nRow)
{
	D2SEED *ptSeed;
	int nOdd;
	int nBase;
	MISSOBJECTSTXT *ptTxt;
	DWORD dwXEnd;
	DWORD dwYEnd;

	if (nRow >= sgnRows)
		return;
	ptSeed = ObjectRgnGetSeed(ptGame, hRoom);
	nOdd = D2CommonSeedNext(ptSeed) & 1;
	nBase = nRow * 2;
	ptTxt = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(sganRowClasses[nBase + nOdd]);
	dwXEnd = ptRect->nX - ptTxt->SizeX + ptRect->nWidth;
	dwYEnd = ptRect->nHeight - ptTxt->SizeY + ptRect->nY;
	if (!nOdd)
	{
		int nX;
		int nY;

		nY = ptRect->nY + 1;
		nX = ptRect->nX + ptTxt->SizeX;
		for (; (DWORD)nX < dwXEnd; nX += ptTxt->SizeX * 2)
		{
			if (ObjectSpawnClearXYI(hRoom, ptTxt->SizeY, nX, nY, ptTxt->SizeX, ptRect))
				SUnitCreate(UNIT_OBJECT, sganRowClasses[nBase], nX, nY, ptGame, hRoom, 1, 0, 0);
		}
		return;
	}
	{
		int nX;
		int nY;

		nX = ptRect->nX + 1;
		for (nY = ptRect->nY + ptTxt->SizeY; (DWORD)nY < dwYEnd; nY += ptTxt->SizeY * 2)
		{
			if (ObjectSpawnClearXYI(hRoom, ptTxt->SizeY, nX, nY, ptTxt->SizeX, ptRect))
				SUnitCreate(UNIT_OBJECT, sganRowClasses[nBase + nOdd], nX, nY, ptGame, hRoom, 1, 0, 0);
		}
	}
}

BOOL __fastcall ObjectThemeWeaponRacks(GAME *ptGame, GAMEROOM *hRoom, GAMERECT *ptRect)
{
	ObjectThemeRow(ptGame, hRoom, ptRect, 1);
	return TRUE;
}

static BOOL ObjectCountRoom(GAME *ptGame, GAMEROOM *hRoom, OBJRGNLEVEL **pptRgn)
{
	int eLevelId;
	OBJRGNLEVEL *ptRgn;

	if (D2CommonDungeonRoomHasExitFlags(hRoom))
		return FALSE;
	eLevelId = (int)D2CommonDungeonRoomGetLevelIdChecked(hRoom);
	ptRgn = ObjectRgnGetLevel(ptGame, eLevelId);
	if (!eLevelId)
		return FALSE;
	if (D2CommonDungeonRoomGetOutdoorFlag(hRoom))
		return FALSE;
#line 2874
	D2ASSERT(eLevelId < NUM_LEVEL_IDS);
	if (D2CommonDungeonRoomIsTown(hRoom))
		return FALSE;
	if (ptRgn->nRooms == 2147483647)
		ptRgn->nRooms = D2CommonDungeonActCountLevelRooms(ptGame->ptAct[ptRgn->bAct], eLevelId);
	ptRgn->nRoomsPopulated++;
	*pptRgn = ptRgn;
	return TRUE;
}

void __fastcall ObjectPopulateRoom(GAME *ptGame, GAMEROOM *hRoom)
{
	MISSLEVELDATA *ptLevelData;
	OBJRGNLEVEL *ptRgn;
	DWORD dwThemes;
	D2SEED *ptThemeSeed;
	D2SEED *ptSeed;
	int nRoll;
	int nChance;
	GAMERECT atRect[2];
	int anThemes[7];
	int nThemes;
	int nTheme;
	DWORD dwBit;
	int i;
	int nGroup;
	OBJGROUPTXT *ptObjectGroup;
	int nSum;
	int ObjIdx;
	int nObjClass;
	MISSOBJECTSTXT *ptObjectType;

	ptLevelData = (MISSLEVELDATA *)D2CommonLvlTblsGetLevelData(D2CommonDungeonRoomGetLevelId(hRoom));
#line 2947
	D2ASSERT(ptLevelData != NULL);
	if (!ObjectCountRoom(ptGame, hRoom, &ptRgn))
		return;
	dwThemes = ptLevelData->Themes;
	if (dwThemes && ptRgn->nRooms >= ptRgn->field_0c * 10)
	{
		ptThemeSeed = ObjectRgnGetSeed(ptGame, hRoom);
		nRoll = (int)D2CommonSeedRand(ptThemeSeed, 100);
		nChance = 0;
		if (ptRgn->nRoomsPopulated > ptRgn->nRooms / 2)
			nChance = 5;
		if (ptRgn->nRoomsPopulated > ptRgn->nRooms - ptRgn->nRooms / 4)
			nChance += 5;
		nChance += 12;
		if (nRoll < nChance)
		{
			D2CommonDungeonRoomCopyRects(hRoom, atRect);
			for (i = 0; i < 7; i++)
				anThemes[i] = 0;
			nThemes = 0;
			for (dwBit = 1, nTheme = 1, i = 7; i; dwBit <<= 1, nTheme++, i--)
				if (dwThemes & dwBit)
					anThemes[nThemes++] = nTheme;
			i = OBJ_RAND(ptThemeSeed, nThemes);
			nTheme = anThemes[i];
			if (i < nThemes && nTheme < nThemes && sgatThemes[nTheme].fEnabled && sgatThemes[nTheme].pfnTheme
				&& sgatThemes[nTheme].pfnTheme(ptGame, hRoom, atRect) == TRUE)
				return;
		}
	}
	if (hRoom)
		ptSeed = &ROOM5(hRoom)->tSeed;
	else
		ptSeed = NULL;
	for (i = 0; i < 8; i++)
	{
		nGroup = ptLevelData->ObjGrp[i];
		nRoll = (int)D2CommonSeedRand(ptSeed, 100);
		ptRgn = ObjectRgnGetLevel(ptGame, (int)D2CommonDungeonRoomGetLevelIdChecked(hRoom));
		if (ptRgn->nRooms > 0 && (ptRgn->nRoomsPopulated << 7) / ptRgn->nRooms > 96 && ptRgn->field_10 == 0
			&& ((MISSOBJECTSTXT *)D2CommonObjTblsGetObject(nGroup))->SubClass)
			nRoll = 100;
		if (!nGroup || nRoll > ptLevelData->ObjPrb[i])
			continue;
		ptObjectGroup = (OBJGROUPTXT *)D2CommonObjTblsGetObjGroup(nGroup);
		if (!ptObjectGroup)
			return;
		nRoll = (int)D2CommonSeedRand(ptSeed, 100);
		nSum = 0;
		for (ObjIdx = 0; ObjIdx < 8; ObjIdx++)
		{
			nObjClass = ptObjectGroup->ID[ObjIdx];
			if (!nObjClass)
				break;
			nSum += ptObjectGroup->PROB[ObjIdx];
			ptObjectType = (MISSOBJECTSTXT *)D2CommonObjTblsGetObject(nObjClass);
			if (nRoll >= nSum || ptObjectType->Gore > sgbMaxGore)
				continue;
#line 3026
			D2ASSERT(ptObjectType->bPopulateFunction < sizeof(sgPopulateTable)/sizeof(PTOBJECTADDTOGAME));
			if (sgPopulateTable[ptObjectType->bPopulateFunction])
			{
#line 3029
				D2ASSERTMSG(!IsBadCodePtr((FARPROC)sgPopulateTable[ptObjectType->bPopulateFunction]), "sgPopulateTable[ptObjectType->bPopulateFunction]");
#line 3030
				D2ASSERT(ptObjectGroup->bDensity[ObjIdx] < 128);
				sgPopulateTable[ptObjectType->bPopulateFunction](ptGame, hRoom,
					ptObjectGroup->bDensity[ObjIdx], nObjClass, 100);
			}
			break;
		}
	}
}

void __fastcall ObjectFreeUnit(GAME *ptGame, UNIT *hObject)
{
	OBJLINKS *ptLinks;
	OBJLINKNODE *ptNode;
	OBJLINKNODE *ptNext;

	if (UNIT5(hObject)->ptChatNode)
		D2CommonChatNodeFree(ptGame->pMemPool, UNIT5(hObject)->ptChatNode);
	ptLinks = (OBJLINKS *)D2CommonUnitGetObjectLinks(hObject);
	for (ptNode = ptLinks->ptFirst; ptNode; ptNode = ptNext)
	{
		ptNext = ptNode->ptNext;
#line 439
		D2FREEPOOL(ptGame->pMemPool, ptNode);
	}
#line 442
	D2FREEPOOL(ptGame->pMemPool, ptLinks);
	EventUnitFree(ptGame, hObject);
}

void __fastcall ObjectFreeAll(GAME *ptGame)
{
	UNIT **phBucket;
	int nBuckets;
	UNIT *hUnit;
	UNIT *hNext;

	phBucket = ptGame->pUnitList[UNIT_OBJECT];
	for (nBuckets = NUM_OBJECT_BUCKETS; nBuckets; nBuckets--, phBucket++)
	{
		for (hUnit = *phBucket; hUnit; )
		{
			hNext = hUnit;
			hUnit = SUnitGetNextInList(hNext);
			SUnitFree(ptGame, hNext);
		}
	}
}

BYTE __fastcall ObjectRegenerateMax(MISSOBJECTSTXT *ptTypeData)
{
	return (BYTE)(ptTypeData->Param[2] << 1);
}
