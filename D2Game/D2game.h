#ifndef D2RE_D2GAME_H
#define D2RE_D2GAME_H

#include <windows.h>
#include "D2CommonImports.h"
#include "Fog.imports.h"
#include "Storm.imports.h"
#include "D2Net.imports.h"
#include "StormTables.h"
#include "Linklist.h"

#define UNIT_PLAYER	0

#define UnitGetType(hUnit)	((hUnit)->eType)

#define NUM_ACTS	4

struct D2SEED
{
 	DWORD		dwLow;
 	DWORD		dwHigh;
};

struct GAME;
struct EVENT;
struct PLAYERLIST;
struct UNIT
{
 	DWORD		eType;
 	DWORD		dwClassId;
 	DWORD		dwId;
 	DWORD		dwMode;
 	DWORD		dwMemPool;
 	BYTE		bAct;
 	BYTE		unobserved_15[24 - 21];
 	DWORD		dwKeptCmdPending;
 	int			nKeptCmd;
 	DWORD		dwKeptTargetType;
 	DWORD		dwKeptTargetId;
 	D2SEED		tSeed;
 	DWORD		dwInitSeed;
 	void		*ptPath;
 	void		*hAnimSequence;
 	int			nSeqLength;
 	int			nSeqPos;
 	int			nSeqRate;
 	BYTE		unobserved_48[76 - 72];
 	int			nAnimPos;
 	int			nAnimLength;
 	int			nAnimRate;
 	BYTE		bAnimEvent;
 	BYTE		unobserved_59[92 - 89];
 	struct UNITANIMREC	*ptAnimRecord;
 	BYTE		unobserved_60[108 - 96];
 	void		*uTypeData;
 	BYTE		abComponentVariations[16];
 	void		*ptInventory;
 	DWORD		field_84;
 	DWORD		field_88;
 	BYTE		field_8c;
 	BYTE		unobserved_8d;
 	WORD		wEvent;
 	struct UNIT	*field_90;
 	struct QUESTLINK	*field_94;
 	BYTE		field_98;
 	BYTE		unobserved_99[160 - 153];
 	struct GAME	*field_a0;
 	BYTE		unobserved_a4[180 - 164];
 	DWORD		dwLinkedUnitType;
 	DWORD		dwLinkedUnitId;
 	DWORD		dwLastAttackerType;
 	DWORD		dwLastAttackerId;
 	void		*ptChatNode;
 	void		*field_c8;
 	struct DAMAGEHOLD	*ptDamageHold;
 	DWORD		dwHitClass;
 	BYTE		unobserved_d4[216 - 212];
 	DWORD		dwDropItemCode;
 	BYTE		unobserved_dc[228 - 220];
 	DWORD		dwFlags0;
 	DWORD		dwFlags;
 	BYTE		unobserved_ec[240 - 236];
 	int			nTargetListNo;
 	DWORD		field_f4;
 	BYTE		unobserved_f8[252 - 248];
 	struct EVENT	**hUnitEvents;
 	struct UNIT	*ptChangedNext;
 	struct UNIT	*ptListNext;
 	struct UNIT	*ptRoomNext;
};
typedef struct UNIT UNIT, *PTUNIT, *HUNIT;

struct CLIENT;
struct GAMERECT
{
 	int		nX;
 	int		nY;
 	int		nWidth;
 	int		nHeight;
};
struct GAMEROOM
{
 	GAMERECT	tRect;
 	GAMERECT	tRect2;
 	BYTE		unobserved_20[44 - 32];
 	UNIT		*ptUnitFirst;
 	UNIT		*ptUnitChanged;
 	BYTE		unobserved_34[72 - 52];
 	DWORD		dwFlags;
 	struct CLIENT	**phClients;
 	DWORD		nNumClients;
 	BYTE		unobserved_54[100 - 84];
 	BYTE		bRingIndex;
 	BYTE		unobserved_65[104 - 101];
 	DWORD		adwRing[4];
 	BYTE		unobserved_78[124 - 120];
 	struct GAMEROOM	*ptActNext;
};

struct SKILLSTXT
{
 	BYTE		unobserved_000[68];
 	int			ClassReq;
 	BYTE		AttackRank;
 	BYTE		unobserved_049[125 - 73];
 	BYTE		Durability;
 	BYTE		unobserved_07e[132 - 126];
 	BYTE		LineOfSight;
 	BYTE		unobserved_085[136 - 133];
 	int			TargetableOnly;
 	BYTE		unobserved_08c[148 - 140];
 	int			SelectDead;
 	BYTE		unobserved_098[156 - 152];
 	BYTE		TargetPet;
 	BYTE		TargetAlly;
 	BYTE		unobserved_09e[172 - 158];
 	int			AttackNoMana;
 	BYTE		unobserved_0b0[300 - 176];
 	BYTE		skillpage;
 	BYTE		unobserved_12d[308 - 301];
 	int			manashift;
 	int			mana;
 	int			lvlmana;
 	BYTE		Interrupt;
 	BYTE		InTown;
 	BYTE		Periodic;
 	BYTE		unobserved_143[324 - 323];
 	int			Param[6];
 	BYTE		unobserved_15c[380 - 348];
 	BYTE		HitShift;
 	BYTE		unobserved_17d[384 - 381];
 	int			MinDam;
 	BYTE		unobserved_184[396 - 388];
 	BYTE		EType;
 	BYTE		unobserved_18d[400 - 397];
 	int			EMin;
 	int			EMax;
 	int			ELev;
 	int			ELen;
 	int			ELevLen;
};

struct BITBUFFER
{
 	BYTE		*pbData;
 	BYTE		unobserved_04[16 - 4];
};

struct PLRINTRO
{
 	BITBUFFER	*ptBits0;
 	BITBUFFER	*ptBits1;
};

struct ARENAUNIT
{
 	WORD		wSortKey;
 	BYTE		unobserved_02[2];
 	DWORD		field_04;
};

struct PLRINFO
{
 	char		szName[16];
 	void		*apQuestRecords[3];
 	WORD		*apwWaypoints[3];
 	BYTE		unobserved_028[1688 - 40];
 	ARENAUNIT	*ptArenaUnit;
 	struct PLAYERLIST	*ptPlayerList;
 	BYTE		unobserved_6a0[1700 - 1696];
 	DWORD		dwPetId;
 	struct PLAYERPETS	*ptPets;
 	DWORD		dwPortalId;
 	BOOL		bBusy;
 	DWORD		dwTradeState;
 	DWORD		dwTradeMsgTick;
 	DWORD		dwTradeEndTick;
 	struct PLRTRADE	*hTrade;
 	PLRINTRO	*ptIntro[3];
 	DWORD		dwBoughtItemId;
 	DWORD		dwRightSkillId;
 	DWORD		dwLeftSkillId;
 	BYTE		unobserved_6dc[1764 - 1756];
 	struct HPTRACK	*ptHpTrack;
 	struct CLIENT	*ptClient;
};
typedef struct PLRINFO PLRINFO, *PTPLRINFO;

struct ARENA
{
 	DWORD		field_00;
 	DWORD		dwFlags;
 	BYTE		bType;
 	BYTE		unobserved_09[3];
};

struct CLIENT;
typedef void (__fastcall *PFNLEAVEGAME)(DWORD dwGameId, WORD wClass, DWORD dwLevel, __int64 nExperience,
	WORD wFlags, const char *pszName, BYTE *pbSummary);
typedef void (__cdecl *PFNSERVERLOGMESSAGE)(int nLevel, const char *pszFormat, ...);
typedef void (__fastcall *PFNSAVEDATABASEGUILD)(const char *pszTag, const BYTE *pbData, int nLength);
struct EVENTCALLBACKTABLE
{
 	void		*fpCloseGame;
 	PFNLEAVEGAME	fpLeaveGame;
 	void		*fpGetDatabaseCharacter;
 	void		*fpSaveDatabaseCharacter;
 	PFNSERVERLOGMESSAGE	fpServerLogMessage;
 	void		*fpEnterGame;
 	void		*fpFindPlayerToken;
 	PFNSAVEDATABASEGUILD	fpSaveDatabaseGuild;
 	void		*fpUnlockDatabaseCharacter;
 	void		*pfnGuildEvent;
 	void		*pfnReportPlayer;
 	void		*pfnLevelUp;
};

#pragma pack(push, 1)
struct SAVEHEADER
{
 	DWORD		dwMagic;
 	DWORD		dwVersion;
 	char		szName[16];
 	DWORD		dwFlags;
 	WORD		wField1c;
 	WORD		wField1e;
 	WORD		wSize;
 	WORD		wClass;
 	WORD		wLevel;
 	BYTE		abComponentGfx[16];
 	BYTE		abComponentColors[16];
 	BYTE		abHotkeys[8][2];
 	BYTE		bLeftSkillId;
 	BYTE		bRightSkillId;
 	BYTE		bAct;
 	WORD		wGuildFlags;
 	DWORD		dwGuildTag;
 	char		szGuildName[28];
 	BYTE		abGuildEmblem[3];
 	DWORD		dwSeed;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CLIENTSUMMARY
{
 	BYTE		abField00[2];
 	BYTE		abGfx[16];
 	BYTE		bClass;
 	BYTE		abColors[16];
 	BYTE		bLevel;
 	BYTE		abFlags[2];
 	BYTE		abGuildFlags[2];
 	BYTE		abGuildEmblem[3];
 	DWORD		dwGuildTag;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SRVPACKETCREATE
{
 	BYTE		bCmd;
 	char		szGameName[16];
 	BYTE		bGameType;
 	BYTE		bClass;
 	BYTE		bArenaType;
 	BYTE		bDifficulty;
 	char		szPlrName[16];
 	WORD		wArena;
 	DWORD		dwFlags;
 	BYTE		bField2b;
 	BYTE		bField2c;
};
struct SRVPACKETJOIN
{
 	BYTE		bCmd;
 	DWORD		dwToken;
 	WORD		wGameId;
 	BYTE		bClass;
 	DWORD		dwField08;
 	BYTE		abData[1];
};
struct SRVPACKET6
{
 	BYTE		bCmd;
 	DWORD		dwTick;
 	DWORD		dwDelay;
};
struct SRVPACKET5
{
 	BYTE		bCmd;
 	BYTE		bLength;
 	DWORD		dwTotal;
 	BYTE		abData[1];
};
union SRVPACKET
{
	BYTE			bCmd;
	SRVPACKETCREATE	tCreate;
	SRVPACKETJOIN	tJoin;
	SRVPACKET5		t5;
	SRVPACKET6		t6;
};
struct SRVMSG
{
 	DWORD		dwClientId;
 	SRVPACKET	uPacket;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CLIENTNEARUNIT
{
 	BYTE		bType;
 	DWORD		dwUnitId;
 	WORD		wX;
 	WORD		wY;
};
#pragma pack(pop)

struct CLIENTNODE178
{
 	int			nUsed;
 	BYTE		abData[512];
 	struct CLIENTNODE178	*ptNext;
};

#pragma pack(push, 1)
struct CLIENT
{
 	DWORD		dwClientId;
 	DWORD		dwState;
 	BYTE		bClass;
 	BYTE		unobserved_09;
 	WORD		wFlags;
 	BYTE		bCharTemplate;
 	char		szName[16];
 	char		szAccount[19];
 	DWORD		dwTokenResult;
 	union
		{
			BYTE		abSummary[256];
			CLIENTSUMMARY	tSummary;
		}		uSummary;
 	DWORD		dwUnitType;
 	DWORD		dwUnitId;
 	UNIT		*hPlayerUnit;
 	BYTE		*pbBuffer;
 	int			nBufferUsed;
 	DWORD		dwSendDelay;
 	DWORD		dwBufferSent;
 	DWORD		dwField150Type;
 	DWORD		dwField150Id;
 	DWORD		dwSentLife;
 	DWORD		dwSentMaxLife;
 	struct GAME	*ptGame;
 	BYTE		bAct;
 	BYTE		bSaveAct;
 	BYTE		unobserved_166[2];
 	DWORD		dwUpdateFrames;
 	struct GAMEROOM	*hRoom;
 	DWORD		dwQueueHead;
 	DWORD		dwQueueTail;
 	CLIENTNODE178	*ptFreeNodes;
 	int			nSendErrors;
 	CLIENTNEARUNIT	atNear[55];
 	BYTE		unobserved_36f;
 	DWORD		nNear;
 	DWORD		dwFlags;
 	DWORD		dwTick;
 	BYTE		abHotkeys[8][2];
 	WORD		wGuildFlags;
 	DWORD		dwGuildTag;
 	char		szGuildName[28];
 	BYTE		abGuildEmblem[3];
 	BYTE		unobserved_3b1[3];
 	struct CLIENT	*ptNextInGame;
 	struct CLIENT	*ptSrvNext;
 	struct CLIENT	*ptSrvByNameNext;
 	DWORD		adwPing[16];
 	__int64		nPingAverage;
 	DWORD		nPings;
 	BYTE		unobserved_40c[4];
};
#pragma pack(pop)
typedef struct CLIENT CLIENT, *PTCLIENT;

#define CLIENT_NUMBUCKETS	256
typedef CLIENT *CLIENTBUCKETS[CLIENT_NUMBUCKETS];

#define TASK_PROCESSGAME	0

struct TASK
{
 	DWORD		dwTime;
 	DWORD		eType;
 	DWORD		dwGameId;
 	LINK		tLink;
};
typedef struct TASK TASK;

#define MAX_UNIQUE_ITEMS	256
struct ITEMUNIQUENODE;

struct GAME : TSHashObject<GAME, HASHKEY_NONE>
{
 	CRITICAL_SECTION	*ptLock;
 	void		*pMemPool;
 	DWORD		field_20;
 	WORD		wGameId;
 	char		szGameName[16];
 	char		szField36[16];
 	BYTE		bGameType;
 	BYTE		bField47;
 	BYTE		bField48;
 	BYTE		bDifficultyLevel;
 	BYTE		bField4a;
 	BYTE		unobserved_04b;
 	DWORD		dwSeed;
 	DWORD		dwObjRegionSeed;
 	DWORD		dwSeedFixed;
 	CLIENT		*ptClientList;
 	int			nNumClients;
 	DWORD		field_60;
 	int			nFrame;
 	DWORD		dwFramesPerSecond;
 	DWORD		dwFrames;
 	DWORD		dwSecondTick;
 	void		*hEvents;
 	struct ACT	*ptAct[NUM_ACTS + 1];
 	BYTE		unobserved_08c[144 - 140];
 	struct INACTIVENODE	*ptInactive[NUM_ACTS];
 	DWORD		dwMonRegionSeed;
 	void		*pMonRegions;
 	BYTE		unobserved_0a8[600 - 168];
 	struct OBJREGION	*ptObjRegion;
 	struct QUESTHEAD	*pQuestHead;
 	DWORD		field_260;
 	DWORD		field_264;
 	DWORD		field_268;
 	DWORD		field_26c;
 	DWORD		field_270;
 	struct TARGET	*aptTargetList[10];
 	UNIT		*pUnitList[5][128];
 	UNIT		*ptRoomTileList;
 	struct ITEMUNIQUENODE	*aptUniqueItems[MAX_UNIQUE_ITEMS];
 	struct SUNITPROXY	*ptProxy;
 	ARENA		*ptArena;
 	struct PARTYSYSTEM	*hParty;
 	BYTE		abSuperUniques[8];
 	DWORD		adwStats[17];
 	DWORD		nRespawnRooms;
 	struct GAMEROOM	*ahRespawnRooms[2000];
 	DWORD		dwRateTick;
 	DWORD		dwRate;
 	DWORD		dwEmptyTick;
 	void		*pGuildData;
 	TASK		tTask;
};
typedef struct GAME GAME, *PTGAME;

struct ACT
{
 	BYTE		unobserved_00[32];
 	DWORD		dwRoomsAdded;
 	DWORD		dwRoomNodesAdded;
 	DWORD		dwUnitsChanged;
 	BYTE		unobserved_2c[48 - 44];
};
typedef struct ACT ACT, *PTACT;

struct HPTRACK
{
 	DWORD		dwUnitId;
 	int			nLife;
 	int			nLevelId;
 	BYTE		bSkipped;
 	BYTE		unobserved_0d[3];
 	struct HPTRACK	*ptNext;
};

typedef TSExportTableSync<GAME, HASHKEY_NONE> GAMEDATATBL;

typedef void (__fastcall *PFNCLOSEGAME)(WORD wGameId);
typedef void (__fastcall *PFNGETDATABASECHARACTER)(char *szName, DWORD dwClientId);
typedef void (__fastcall *PFNSAVEDATABASECHARACTER)(const char *pszName, const char *pszAccount,
	BYTE *pbData, int nLength, DWORD dwTokenResult);
typedef void (__fastcall *PFNENTERGAME)(DWORD dwGameId, char *szName, WORD wClass, DWORD dwLevel,
	DWORD dwArg);
typedef BOOL (__fastcall *PFNFINDPLAYERTOKEN)(char *szName, DWORD dwToken, WORD wGameId, char *szAccount,
	DWORD *pdwResult);
typedef void (__fastcall *PFNUNLOCKDATABASECHARACTER)(char *szName);
typedef void (__fastcall *PFNCALLBACK28)(char *szName, WORD wClass, DWORD dwLevel, __int64 nExperience,
	DWORD dwFlags);

#pragma pack(push, 1)
struct GAMEPLAYERMSG
{
 	BYTE		bCmd;
 	BYTE		bEvent;
 	BYTE		bColor;
 	DWORD		dwArg;
 	BYTE		bSubKind;
 	char		szName[16];
 	char		szAccount[16];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct GAMEDATAPIECE
{
 	BYTE		bCmd;
 	BYTE		bLength;
 	BYTE		bFirst;
 	DWORD		dwTotal;
 	BYTE		abData[256];
};
struct GAMEQUEUEDRECORD
{
 	WORD		wSize;
 	BYTE		bCmd;
 	BYTE		unobserved_03[11 - 3];
 	DWORD		dwTime;
};
#pragma pack(pop)

struct GAMEINFO
{
 	DWORD		dwGameId;
 	DWORD		dwSeed;
 	int			nNumClients;
 	int			anUnits[5];
 	BYTE		unobserved_20[8];
 	DWORD		dwNextPlayerId;
 	DWORD		adwField2c[5];
 	DWORD		adwPercent[5];
 	DWORD		dwStatBase;
 	int			nFrames100;
 	DWORD		dwMinutes;
 	DWORD		dwFramesPerSecond;
 	char		szGameName[16];
 	char		szField36[16];
 	BYTE		bField47;
 	BYTE		bField48;
 	BYTE		bField4a;
 	BYTE		unobserved_87;
 	DWORD		dwArenaFlags;
 	void		*pMemPool;
};

struct GAMEUNITINFO
{
 	DWORD		eType;
 	DWORD		dwClassId;
 	DWORD		dwUnitId;
 	DWORD		dwMode;
 	DWORD		dwX;
 	DWORD		dwY;
 	DWORD		dwLinkedUnitType;
 	DWORD		dwLinkedUnitId;
 	BYTE		unobserved_20[36 - 32];
 	char		szDesc[16];
};

struct GAMEUNITCLASS
{
 	DWORD		dwClassId;
 	int			nCount;
 	DWORD		field_08;
 	char		szDesc[128];
 	struct GAMEUNITCLASS	*ptNext;
};

struct GAMECENSUS
{
 	int			nGames;
 	int			nMonsters;
 	int			field_08;
 	int			nPlayers;
};

struct GAMEITEMSTXT
{
 	BYTE		unobserved_00[46];
 	BYTE		quest;
 	BYTE		unobserved_2f[65 - 47];
 	BYTE		Transmogrify;
 	BYTE		TMogMin;
 	BYTE		TMogMax;
 	BYTE		unobserved_44[100 - 68];
 	int			gemoffset;
 	DWORD		code;
 	BYTE		unobserved_6c[112 - 108];
 	DWORD		ubercode;
 	BYTE		unobserved_74[140 - 116];
 	DWORD		TMogType;
 	char		szName[1];
};

#define ITEMSDataTestCmdFlags(hItem, dwFlag) \
	D2CommonITEMSDataTestCmdFlagsRaw((DWORD)(hItem), (DWORD)(dwFlag))
#define ITEM_CMDFLAG_SETHANDITEM	1048576

struct GAMECOORD
{
 	DWORD		dwX;
 	DWORD		dwY;
};

struct BOOKSTXT
{
 	BYTE		unobserved_00[4];
 	int			spelloffset;
 	int			skillscrolloffset;
 	int			skillbookoffset;
 	BYTE		unobserved_10[24 - 16];
 	DWORD		scrollspellcode;
 	DWORD		bookspellcode;
 	BYTE		unobserved_20[68 - 32];
};
typedef struct BOOKSTXT BOOKSTXT;

struct BELTSTXT
{
 	BYTE		unobserved_00[4];
 	BYTE		numboxes;
 	BYTE		unobserved_05[264 - 5];
};

struct UNITD2GAME5
{
 	BYTE		unobserved_000[196];
 	void		*ptChatNode;
 	BYTE		unobserved_0c8[208 - 200];
 	DWORD		dwHitClass;
};
#define UNIT5(hUnit)	((struct UNITD2GAME5 *)(hUnit))

struct MONSTERUNIQUE;
struct MONSTERDATA
{
 	struct AIGENERAL	*ptAi;
 	struct MONMODEDATA	*ptModeData;
 	struct MONSTERUNIQUE	*ptUnique;
 	void		*pRegionRef;
 	struct THREATNODE	**pptThreats;
 	DWORD		dwTargetCmdArg;
 	int			eTargetCmd;
 	HPTRACK		tHpTrack;
 	void		*ptArea;
 	struct MONSTATSTXT	*ptMonStats;
};
typedef struct MONSTERDATA MONSTERDATA;

struct GAMEROOMD2GAME5
{
 	BYTE		unobserved_000[60];
 	D2SEED		tSeed;
};
#define ROOM5(hRoom)	((struct GAMEROOMD2GAME5 *)(hRoom))

struct MONSTATSTXT
{
 	BYTE		unobserved_000[360];
 	int			nBaseId;
 	BYTE		unobserved_16c[372 - 364];
 	int			anMinHP[3];
 	int			MaxHP[3];
 	BYTE		unobserved_18c[400 - 396];
 	DWORD		dwSizeX;
 	BYTE		unobserved_194[409 - 404];
 	BYTE		OverlayHeight;
 	BYTE		unobserved_19a[412 - 410];
 	int			nVelocity;
 	DWORD		Run;
 	BYTE		unobserved_1a4[424 - 420];
 	int			AC[3];
 	int			Exp[3];
 	BYTE		Level[3];
 	BYTE		bMeleeRng;
 	BYTE		unobserved_1c4;
 	BYTE		bMinGrp;
 	BYTE		bMaxGrp;
 	BYTE		unobserved_1c7[476 - 455];
 	BYTE		AIParam[5];
 	BYTE		unobserved_1e1[483 - 481];
 	BYTE		bModeW;
 	BYTE		bModeGH;
 	BYTE		bModeA1;
 	BYTE		unobserved_1e6[489 - 486];
 	BYTE		bModeS1;
 	BYTE		bModeS2;
 	BYTE		unobserved_1eb[494 - 491];
 	BYTE		bModeKB;
 	BYTE		unobserved_1ef;
 	BYTE		bModeRN;
 	BYTE		ElMode;
 	BYTE		ElType;
 	BYTE		ElPct;
 	BYTE		ElMinD;
 	BYTE		ElMaxD;
 	BYTE		ElDur;
 	BYTE		unobserved_1f7[512 - 503];
 	DWORD		dwMissS1;
 	BYTE		unobserved_204[536 - 516];
 	BYTE		A1Move;
 	BYTE		unobserved_219[3];
 	int			pnAttack1MinDamage[3];
 	int			pnAttack1MaxDamage[3];
 	int			A1ToHit[3];
 	BYTE		A2Move;
 	BYTE		unobserved_241[3];
 	int			A2MinD[3];
 	int			A2MaxD[3];
 	int			A2ToHit[3];
 	BYTE		S1Move;
 	BYTE		unobserved_269[3];
 	int			S1MinD[3];
 	int			S1MaxD[3];
 	int			S1ToHit[3];
 	BYTE		S2Move;
 	BYTE		S3Move;
 	BYTE		S4Move;
 	BYTE		bToBlock;
 	BYTE		bCMove;
 	BYTE		bAlign;
 	BYTE		bIsMelee;
 	BYTE		unobserved_297[667 - 663];
 	BYTE		bIsAtt;
 	BYTE		bPetIgnore;
 	BYTE		bIsNPC;
 	BYTE		bIsCritter;
 	BYTE		bInTown;
 	BYTE		unobserved_2a0[680 - 672];
 	DWORD		Skill1[5];
 	BYTE		Skill1Seq[5];
 	BYTE		Skill1Lvl[5];
 	BYTE		unobserved_2c6[713 - 710];
 	BYTE		DamageResist[3];
 	BYTE		MagicResist[3];
 	BYTE		FireResist[3];
 	BYTE		LightResist[3];
 	BYTE		ColdResist[3];
 	BYTE		PoisonResist[3];
 	BYTE		unobserved_2db;
 	int			nDamageRegen;
 	BYTE		bLUndead;
 	BYTE		bHUndead;
 	BYTE		unobserved_2e2[742 - 738];
 	BYTE		bFlying;
 	BYTE		bOpenDoors;
 	BYTE		unobserved_2e8;
 	BYTE		bSpawnCol;
 	BYTE		unobserved_2ea;
 	BYTE		bInteract;
 	BYTE		bSpawnComponents;
 	BYTE		bSoft;
 	BYTE		unobserved_2ee[752 - 750];
 	DWORD		Heart;
 	DWORD		BodyPart;
 	BYTE		TreasureClass[3][4];
 	BYTE		bSpawnPctBonus;
 	BYTE		bKillable;
 	BYTE		bSwitch;
 	BYTE		bRestore;
 	BYTE		bNeverCount;
 	BYTE		unobserved_309;
 	BYTE		SplEndDeath;
 	BYTE		SplGetModeChart;
 	BYTE		SplEndGeneric;
 	BYTE		unobserved_30d;
 	BYTE		bDeadCollision;
 	BYTE		unobserved_30f[785 - 783];
 	BYTE		bDeathDamage;
};

struct ITEMSTXT
{
 	BYTE		completed;
 	BYTE		rarity;
 	BYTE		level;
 	BYTE		mindam;
 	BYTE		maxdam;
 	BYTE		minmisdam;
 	BYTE		maxmisdam;
 	BYTE		bTwoHandMinDam;
 	BYTE		bTwoHandMaxDam;
 	BYTE		unobserved_09[12 - 9];
 	int			minac;
 	int			maxac;
 	BYTE		unobserved_14[25 - 20];
 	BYTE		block;
 	BYTE		durability;
 	BYTE		nodurability;
 	BYTE		unobserved_1c[42 - 28];
 	BYTE		type;
 	BYTE		subtype;
 	BYTE		unobserved_2c[46 - 44];
 	BYTE		quest;
 	BYTE		unobserved_2f[56 - 47];
 	BYTE		spawnable;
 	BYTE		unobserved_39[68 - 57];
 	BYTE		bWeaponGroup;
 	BYTE		unobserved_45[76 - 69];
 	int			speed;
 	int			bitfield1;
 	BYTE		unobserved_54[96 - 84];
 	int			spelloffset;
 	BYTE		unobserved_64[104 - 100];
 	DWORD		code;
 	BYTE		unobserved_6c[432 - 108];
 	DWORD		BetterGem;
 	BYTE		unobserved_1b4;
 	BYTE		Min[14];
 	BYTE		Max[14];
 	BYTE		MagicMin[14];
 	BYTE		MagicMax[14];
 	BYTE		MagicLvl[14];
 	BYTE		NightmareUpgrade;
 	BYTE		HellUpgrade;
 	BYTE		unobserved_1fd[512 - 509];
};
typedef struct ITEMSTXT ITEMSTXT;

struct ITEMSTATSDATATBLDESC
{
 	int			nSize;
 	ITEMSTXT	*ptItemStats;
 	ITEMSTXT	*ptWeaponsStart;
 	ITEMSTXT	*ptArmorStart;
 	ITEMSTXT	*ptMiscStart;
};

struct MAGICAFFIXTXT
{
 	BYTE		unobserved_00[76];
 	BYTE		spawnable;
 	BYTE		unobserved_4d[80 - 77];
 	int			level;
 	int			group;
 	BYTE		unobserved_58[136 - 88];
};
struct MAGICITEMMODSDATATBLDESC
{
 	int			nSize;
 	MAGICAFFIXTXT	*ptItemMods;
 	MAGICAFFIXTXT	*ptSuffixes;
 	MAGICAFFIXTXT	*ptPrefixes;
};

struct RAREAFFIXTXT
{
 	BYTE		unobserved_00[64];
};
struct RAREITEMMODSDATATBLDESC
{
 	int			nSize;
 	RAREAFFIXTXT	*ptItemMods;
 	RAREAFFIXTXT	*ptSuffixes;
 	RAREAFFIXTXT	*ptPrefixes;
};

struct UNIQUEITEMSTXT
{
 	DWORD		code;
 	BYTE		unobserved_04[248 - 4];
};
struct UNIQUEITEMMODSDATATBLDESC
{
 	DWORD		nSize;
 	UNIQUEITEMSTXT	*ptItemMods;
};

struct SETITEMINFO
{
 	DWORD		code;
 	BYTE		unobserved_04[40 - 4];
};
struct SETITEMSTXT
{
 	BYTE		unobserved_000[140];
 	SETITEMINFO	atItems[6];
 	BYTE		unobserved_17c[880 - 380];
};
struct SETITEMMODSDATATBLDESC
{
 	DWORD		nSize;
 	SETITEMSTXT	*ptItemMods;
};

struct QUALITYITEMSTXT;
struct HIQUALITYITEMMODSDATATBLDESC
{
 	int			nSize;
 	struct QUALITYITEMSTXT	*ptItemMods;
};
struct LOWQUALITYITEMMODSDATATBLDESC
{
 	int			nSize;
 	void		*ptItemMods;
};

struct BOOKITEMMODSDATATBLDESC
{
 	DWORD		nSize;
 	BOOKSTXT	*ptItemMods;
};

struct ITEMRATIOTXT
{
 	int			Unique;
 	int			UniqueDivisor;
 	int			Rare;
 	int			RareDivisor;
 	int			Set;
 	int			SetDivisor;
 	int			Magic;
 	int			MagicDivisor;
 	int			HiQuality;
 	int			HiQualityDivisor;
 	int			Normal;
 	int			NormalDivisor;
};

struct ITEMLEVELDATA
{
 	BYTE		unobserved_00[12];
 	BYTE		abLevel[3];
};

struct COLPOINT
{
 	int			nX;
 	int			nY;
};

struct ITEMCREATE
{
 	UNIT		*hSpawner;
 	D2SEED		*ptSeed;
 	GAME		*ptGame;
 	int			nLevel;
 	DWORD		field_10;
 	DWORD		dwClassId;
 	DWORD		dwMode;
 	int			nX;
 	int			nY;
 	GAMEROOM	*hRoom;
 	WORD		wCreateFlags;
 	BYTE		unobserved_2a[2];
 	DWORD		dwRecreate;
 	DWORD		dwNoSockets;
 	DWORD		dwHiQualityDefault;
 	DWORD		dwQuality;
 	int			nQuantity;
 	int			nDurability;
 	int			nMaxDurability;
 	DWORD		dwFileIndex;
 	DWORD		dwFlags;
 	DWORD		dwSeed;
 	DWORD		dwItemSeed;
 	BYTE		bEarLevel;
 	char		szName[108 - 89];
};
typedef struct ITEMCREATE ITEMCREATE;

struct ITEMUNIQUENODE
{
 	UNIT		*hItem;
 	struct ITEMUNIQUENODE	*ptNext;
};

struct GAMEROOMD2GAME7
{
 	BYTE		unobserved_00[60];
 	D2SEED		tSeed;
};
#define ROOM7(hRoom)	((struct GAMEROOMD2GAME7 *)(hRoom))

struct UNITD2GAME7
{
 	BYTE		unobserved_00[76];
 	DWORD		nAnimPos;
 	BYTE		unobserved_50[216 - 80];
 	DWORD		dwDropItemCode;
};
#define UNIT7(hUnit)	((struct UNITD2GAME7 *)(hUnit))

typedef void (__fastcall *PFNMISSILEMODIFY)(UNIT *hMissile, DWORD dwArg);
struct MISSILECREATE
{
 	DWORD		dwFlags;
 	UNIT		*hSource;
 	UNIT		*hSpawnLocation;
 	UNIT		*hTarget;
 	int			eMissile;
 	int			nX;
 	int			nY;
 	int			nDX;
 	int			nDY;
 	DWORD		field_24;
 	int			nVelocity;
 	int			nLevel;
 	int			nSubLoops;
 	DWORD		field_34;
 	DWORD		field_38;
 	int			nRangeDec;
 	int			nActivate;
 	int			nToHit;
 	int			nRange;
 	PFNMISSILEMODIFY	pfnModify;
 	DWORD		dwModifyArg;
};
typedef struct MISSILECREATE MISSILECREATE;

struct MISSILESTXT
{
 	BYTE		Vel;
 	BYTE		MaxVel;
 	BYTE		unobserved_02[2];
 	int			Accel;
 	int			Range;
 	int			LevRange;
 	BYTE		unobserved_10[22 - 16];
 	BYTE		Activate;
 	BYTE		unobserved_17[88 - 23];
 	BYTE		AnimLen;
 	BYTE		unobserved_59[96 - 89];
 	BYTE		SubLoop;
 	BYTE		SubStart;
 	BYTE		SubStop;
 	BYTE		CollideType;
 	BYTE		Collision;
 	BYTE		CollideKill;
 	BYTE		CollideFriend;
 	BYTE		unobserved_67;
 	BYTE		CanDestroy;
 	BYTE		ClientSend;
 	BYTE		NextHit;
 	BYTE		NextDelay;
 	BYTE		unobserved_6c;
 	BYTE		ToHit;
 	BYTE		AlwaysExplode;
 	BYTE		Explosion;
 	BYTE		NeverDup;
 	BYTE		ReturnFire;
 	BYTE		GetHit;
 	BYTE		KnockBack;
 	BYTE		unobserved_74;
 	BYTE		Qty;
 	BYTE		Pierce;
 	BYTE		unobserved_77;
 	int			Param1;
 	int			Param2;
 	BYTE		unobserved_80[136 - 128];
 	int			Skill;
 	BYTE		HitShift;
 	BYTE		unobserved_8d[3];
 	int			SrcDamage;
 	int			nMinDamage;
 	int			nMaxDamage;
 	int			LevDamage;
 	BYTE		EType;
 	BYTE		unobserved_a1[3];
 	int			EMin;
 	int			EMax;
 	int			ELevel;
 	int			ELen;
 	int			ELevelLen;
 	BYTE		unobserved_b8[196 - 184];
};
typedef struct MISSILESTXT MISSILESTXT;

typedef BOOL (__fastcall *PFNMISSILECOLLIDE)(UNIT *hUnit);

#define D2CommonGetActNum(nLevelId) \
	((BYTE)D2CommonOrdinal10001((DWORD)(nLevelId)))

struct UNITD2GAME8
{
 	BYTE		unobserved_000[76];
 	int			nAnimPos;
 	BYTE		unobserved_050[200 - 80];
 	void		*ptSkills;
};
#define UNIT8(hUnit)	((struct UNITD2GAME8 *)(hUnit))

struct DAMAGE
{
 	DWORD		dwFlags0;
 	WORD		wFlags;
 	BYTE		unobserved_06[2];
 	int			nPhysicalDamage;
 	DWORD		dwDamagePercent;
 	int			nFireDamage;
 	int			nLightningDamage;
 	int			nMagicDamage;
 	int			nColdDamage;
 	int			nPoisonDamage;
 	int			nPoisonLength;
 	int			nColdLength;
 	int			nFreezeLength;
 	int			nLifeDrain;
 	int			nManaDrain;
 	int			nStaminaDrain;
 	int			nStunLength;
 	int			nAbsorbLife;
 	int			nTotalDamage;
 	BYTE		unobserved_48[76 - 72];
 	DWORD		dwWeaponId;
 	DWORD		dwHitClass;
 	DWORD		dwFixedHitClass;
 	int			nOverlayId;
};
typedef struct DAMAGE DAMAGE;

struct MISSILEMODECHART
{
 	PFNMISSILECOLLIDE	pfnUnitTest;
 	DWORD		dwPattern;
};
typedef struct MISSILEMODECHART MISSILEMODECHART;

struct MISSSKILLSTXT
{
 	BYTE		unobserved_000[324];
 	int			Param[6];
};

struct MISSOBJECTSTXT
{
 	char		Name[64];
 	WORD		wszName[64];
 	char		Token[3];
 	BYTE		SpawnMax;
 	BYTE		Selectable[8];
 	BYTE		TrapProb;
 	BYTE		unobserved_cd[3];
 	int			SizeX;
 	int			SizeY;
 	int			FrameCnt[8];
 	int			FrameDelta[8];
 	BYTE		CycleAnim[8];
 	BYTE		Lit[8];
 	BYTE		BlocksLight[8];
 	BYTE		HasCollision[8];
 	BYTE		IsAttackable0;
 	BYTE		Start[8];
 	BYTE		OrderFlag[8];
 	BYTE		EnvEffect;
 	BYTE		IsDoor;
 	BYTE		BlocksVis;
 	BYTE		Orientation;
 	BYTE		PreOperate;
 	BYTE		Trans;
 	BYTE		Mode[8];
 	BYTE		unobserved_157[1];
 	int			Xoffset;
 	int			Yoffset;
 	BYTE		Draw;
 	BYTE		HD;
 	BYTE		TR;
 	BYTE		LG;
 	BYTE		RA;
 	BYTE		LA;
 	BYTE		RH;
 	BYTE		LH;
 	BYTE		SH;
 	BYTE		S[8];
 	BYTE		TotalPieces;
 	BYTE		XSpace;
 	BYTE		YSpace;
 	BYTE		Red;
 	BYTE		Green;
 	BYTE		Blue;
 	BYTE		SubClass;
 	int			NameOffset;
 	BYTE		unobserved_17c[1];
 	BYTE		MonsterOK;
 	BYTE		OperateRange;
 	BYTE		ShrineFunction;
 	BYTE		Act;
 	BYTE		Lockable;
 	BYTE		Gore;
 	BYTE		Restore;
 	BYTE		RestoreVirgins;
 	BYTE		Sync;
 	BYTE		Flicker;
 	BYTE		unobserved_187[1];
 	int			Param[8];
 	BYTE		nTgtFX;
 	BYTE		nTgtFY;
 	BYTE		nTgtBX;
 	BYTE		nTgtBY;
 	BYTE		bDamage;
 	BYTE		CollisionSubst;
 	BYTE		unobserved_1ae[2];
 	int			Left;
 	int			Top;
 	int			Width;
 	int			Height;
 	BYTE		Beta;
 	BYTE		bInitFunction;
 	BYTE		bPopulateFunction;
 	BYTE		bOperateFunction;
 	BYTE		ClientFn;
 	BYTE		Overlay;
 	BYTE		BlockMissile;
 	BYTE		DrawUnder;
 	BYTE		OpenWarp;
 	BYTE		unobserved_1c9[3];
 	int			AutoMap;
};
typedef struct MISSOBJECTSTXT MISSOBJECTSTXT;

struct MISSLEVELDATA
{
 	BYTE		unobserved_00[2];
 	BYTE		Act;
 	BYTE		unobserved_03[12 - 3];
 	BYTE		MonLvl[3];
 	BYTE		unobserved_0f[23 - 15];
 	BYTE		MonSpcWalk;
 	BYTE		Quest;
 	BYTE		unobserved_19[228 - 25];
 	BYTE		Utot;
 	BYTE		unobserved_e5[3];
 	int			U[25];
 	BYTE		unobserved_14c[393 - 332];
 	BYTE		ObjGrp[8];
 	BYTE		ObjPrb[8];
 	BYTE		unobserved_199[692 - 409];
 	DWORD		Themes;
};
typedef struct MISSLEVELDATA MISSLEVELDATA;

struct MONDIFFICULTYTXT
{
 	int			ResistPenalty;
 	int			DeathExpPenalty;
 	BYTE		unobserved_08[16 - 8];
 	int			MonsterSkillBonus;
 	int			nMonsterFreezeDivisor;
 	int			nMonsterColdDivisor;
 	int			nMonsterAICurseDivisor;
};

struct PATHPOINT
{
 	WORD		wX;
 	WORD		wY;
};

struct UNITFINDARGS
{
 	DWORD		dwFlags;
 	DWORD		field_04;
 	UNIT		*hExcluded;
 	int			nX;
 	int			nY;
 	int			nDistance;
 	int			nMaxMatches;
 	int			nMatches;
 	void		*pFieldPair;
 	void		*pfnFind;
 	BYTE		unobserved_28[56 - 40];
};
struct UNITFINDDATA
{
 	void		*pPool;
 	UNIT		**phUnitsArray;
 	void		*hRoom;
 	void		*pfnUnitTest;
 	DWORD		dwData;
 	DWORD		dwFlags;
 	int			nNumUnits;
 	DWORD		nMaxUnits;
 	int			nX;
 	int			nY;
 	int			nRadius;
};

struct SKILLFINDTARGET
{
 	DWORD		dwFlags;
 	UNIT		*hUnit;
 	DWORD		dwUnitId;
 	DWORD		field_0c;
 	DWORD		dwCollisionMask;
 	int			nRange;
 	int			nX;
 	int			nY;
};

struct SKILLSUMMON
{
 	DWORD		dwFlags;
 	UNIT		*hOwner;
 	DWORD		dwClassId;
 	DWORD		dwAiKind;
 	DWORD		dwMode;
 	int			nX;
 	int			nY;
 	DWORD		dwPetType;
};

struct SKILLSTATE
{
 	UNIT		*hSource;
 	UNIT		*hTarget;
 	int			nSkill;
 	int			nLevel;
 	int			nDuration;
 	DWORD		dwStatId;
 	int			nStatValue;
 	DWORD		dwState;
 	DWORD		pfnRemove;
};

struct OBJOPERATE
{
 	GAME		*ptGame;
 	UNIT		*hObject;
 	UNIT		*hPlayer;
 	D2SEED		*ptSeed;
 	DWORD		dwClassId;
};

struct MONSTERREGIONREF
{
 	DWORD		dwPrevMode;
 	DWORD		dwLevelId;
 	DWORD		dwFlags;
};
typedef struct MONSTERREGIONREF MONSTERREGIONREF;

struct MONMODECHANGE
{
 	int			eMode;
 	UNIT		*hUnit;
 	UNIT		*hTarget;
 	int			nX;
 	int			nY;
 	BYTE		bDirection;
 	BYTE		bPathType;
 	BYTE		unobserved_16[2];
 	DWORD		nVelocity;
 	BYTE		bDistMax;
 	BYTE		unobserved_1d[3];
};
typedef struct MONMODECHANGE MONMODECHANGE;

struct MONSTERSCALE
{
 	int			nLifePlayers;
 	int			nExpPlayers;
 	int			nSkillBonus;
 	int			nDifficulty;
};

struct MONPOPSLOT
{
 	int			nClass;
 	BYTE		bRarity;
 	BYTE		unobserved_5[3];
 	DWORD		dwWeight;
 	BYTE		bTotalVariations;
 	BYTE		abVariations[6];
 	BYTE		unobserved_13;
};
typedef struct MONPOPSLOT MONPOPSLOT;

struct THREATNODE
{
 	UNIT		*hUnit;
 	int			nState;
 	struct THREATNODE	*ptNext;
};
typedef struct THREATNODE THREATNODE;

struct MONMODEDATA
{
 	BYTE		field_00;
 	BYTE		field_01;
 	BYTE		unobserved_02[2];
 	DWORD		dwTargetUnit;
 	DWORD		dwTargetX;
 	DWORD		dwTargetY;
 	DWORD		nVelocity;
 	int			field_14;
 	DWORD		dwPathTypeNext;
 	DWORD		nVelocityNext;
 	BYTE		bDistMaxNext;
 	BYTE		unobserved_21[3];
};
typedef struct MONMODEDATA MONMODEDATA;

struct MONREGION
{
 	BYTE		bAct;
 	BYTE		unobserved_01[4 - 1];
 	DWORD		dwRoomsPopulated;
 	DWORD		dwRoomsWithMonsters;
 	int			nRooms;
 	BYTE		bNumLevelSlots;
 	BYTE		bRarity;
 	BYTE		unobserved_12[20 - 18];
 	MONPOPSLOT	atSlots[13];
 	DWORD		dwMonDen;
 	BYTE		bMonUMin;
 	BYTE		bMonUMax;
 	BYTE		unobserved_11e[288 - 286];
 	int			nLevelId;
 	int			nCritters;
 	int			nUniques;
 	int			nMonsters;
 	int			field_130;
 	int			nTrapRoll;
 	BYTE		unobserved_138[316 - 312];
};
typedef struct MONREGION MONREGION;

struct FORMATIONPOINT
{
 	int			nX;
 	int			nY;
 	int			field_08;
};
typedef struct FORMATIONPOINT FORMATIONPOINT;

struct MONSPAWNCTX
{
 	GAME		*ptGame;
 	GAMEROOM	*hRoom;
 	struct DRLGLOGICNODE	*ptArea;
 	int			nClass;
 	DWORD		dwMode;
 	DWORD		dwUnitId;
 	int			nX;
 	int			nY;
 	int			nRadius;
 	WORD		wFlags;
 	BYTE		unobserved_26[2];
};
typedef struct MONSPAWNCTX MONSPAWNCTX;

#define UnitGetClass(hUnit)	((hUnit)->dwClassId)
#define MONSTERCLASS_ROGUEHIREABLE	271

struct OBJECTDATA
{
 	MISSOBJECTSTXT	*ptTypeData;
 	BYTE		field_04;
 	BYTE		bFlags;
 	BYTE		unobserved_06[2];
 	void		*pShrineData;
 	DWORD		dwShrineUser;
 	BYTE		unobserved_10[24 - 16];
 	int			nTargetX;
 	int			nTargetY;
 	int			nLinkedX;
 	int			nLinkedY;
 	BYTE		unobserved_28[56 - 40];
};
typedef struct OBJECTDATA OBJECTDATA;
#define OBJDATA(hUnit)	((OBJECTDATA *)(hUnit)->uTypeData)

struct SHRINESTXT
{
 	BYTE		bCode;
 	BYTE		unobserved_01[3];
 	int			Arg0;
 	int			Arg1;
 	int			nDurationInFrames;
 	BYTE		bResetTimeInMinutes;
 	BYTE		rarity;
 	char		szViewName[32];
 	char		niftyphrase[128];
 	BYTE		effectclass;
 	BYTE		unobserved_b3[1];
 	DWORD		LevelMin;
};
typedef struct SHRINESTXT SHRINESTXT;

struct OBJGROUPTXT
{
 	int			ID[8];
 	BYTE		bDensity[8];
 	BYTE		PROB[8];
 	BYTE		SHRINES;
 	BYTE		WELLS;
 	BYTE		unobserved_32[2];
};
typedef struct OBJGROUPTXT OBJGROUPTXT;

struct OBJLINKNODE
{
 	DWORD		dwUnitId;
 	struct OBJLINKNODE	*ptNext;
};
typedef struct OBJLINKNODE OBJLINKNODE;
struct OBJLINKS
{
 	OBJLINKNODE	*ptFirst;
 	int			nValue;
};
typedef struct OBJLINKS OBJLINKS;

#define OBJRGN_NUM_LEVELS	109

struct SHRINEPOP
{
 	int			NumShrineSubTypes[8];
 	int			*apnShrineIds[8];
};
typedef struct SHRINEPOP SHRINEPOP;

struct OBJRGNSPOT
{
 	int			nX;
 	int			nY;
};
typedef struct OBJRGNSPOT OBJRGNSPOT;

struct OBJRGNLEVEL
{
 	BYTE		bAct;
 	BYTE		unobserved_01[3];
 	int			nRoomsPopulated;
 	int			nRooms;
 	int			field_0c;
 	int			field_10;
 	int			nSpots2;
 	int			nSpots;
 	int			nMonClass;
 	OBJRGNSPOT	atSpots[4];
 	OBJRGNSPOT	atSpots2[10];
};
typedef struct OBJRGNLEVEL OBJRGNLEVEL;

struct OBJRGNNODE
{
 	GAMEROOM	*hRoom;
 	int			nX;
 	int			nY;
 	struct OBJRGNNODE	*ptNext;
};
typedef struct OBJRGNNODE OBJRGNNODE;

struct OBJREGION
{
 	D2SEED		tSeed;
 	SHRINEPOP	tShrinePop;
 	OBJRGNLEVEL	*ptLevels[OBJRGN_NUM_LEVELS];
 	DWORD		field_1fc[50];
 	OBJRGNNODE	*ptNodes;
};
typedef struct OBJREGION OBJREGION;

struct OBJINIT
{
 	GAME		*ptGame;
 	UNIT		*hObject;
 	GAMEROOM	*hRoom;
 	D2SEED		*ptSeed;
 	struct MISSOBJECTSTXT	*ptTypeData;
 	int			nX;
 	int			nY;
};
typedef struct OBJINIT OBJINIT;

struct INITITEM
{
 	DWORD		dwCode;
 	BYTE		bLoc;
 	BYTE		bCount;
 	BYTE		unobserved_06[2];
};
typedef struct INITITEM INITITEM;

struct CHARSTATSTXT
{
 	BYTE		unobserved_000[48];
 	BYTE		str;
 	BYTE		dex;
 	BYTE		Int;
 	BYTE		vit;
 	BYTE		hpadd;
 	BYTE		unobserved_035[57 - 53];
 	BYTE		ManaRegen;
 	BYTE		unobserved_03a[60 - 58];
 	int			ToHitFactor;
 	BYTE		WalkVelocity;
 	BYTE		RunVelocity;
 	BYTE		RunDrain;
 	BYTE		LifePerLevel;
 	BYTE		StaminaPerLevel;
 	BYTE		ManaPerLevel;
 	BYTE		LifePerVitality;
 	BYTE		StaminaPerVitality;
 	BYTE		ManaPerMagic;
 	BYTE		BlockFactor;
 	BYTE		unobserved_04a[84 - 74];
 	INITITEM	atItems[10];
};
typedef struct CHARSTATSTXT CHARSTATSTXT;

#define EPETTYPE_NONE		0
#define NUM_PET_TYPES		9

struct PETDATA
{
 	DWORD		dwSeed;
 	DWORD		dwHireId;
};
typedef struct PETDATA PETDATA;

struct PETNODE
{
 	DWORD		dwUnitId;
 	PETDATA		tData;
 	struct PETNODE	*ptNext;
};
typedef struct PETNODE PETNODE;

struct PETSET
{
 	PETNODE		*ptHead;
 	int			nCurrent;
 	int			nMax;
};
typedef struct PETSET PETSET;

struct PLAYERPETS
{
 	PETSET		atSets[NUM_PET_TYPES];
 	BOOL		fLocked;
};
typedef struct PLAYERPETS PLAYERPETS;

#pragma pack(push, 1)
struct ITEMSAVE
{
 	BYTE		bLocation;
 	BYTE		bNumItemsAttached;
 	int			nLevelId;
 	DWORD		nId;
 	BYTE		bMode;
 	BYTE		bSpecMods;
 	DWORD		nValue;
 	DWORD		dwDurability;
 	DWORD		dwMaxDurability;
 	DWORD		dwX;
 	DWORD		dwY;
 	int			nModClass;
 	DWORD		dwFlags;
 	DWORD		dwInitSeed;
 	DWORD		dwItemSeed;
 	BYTE		bPage;
 	BYTE		bEarLevel;
 	char		szPlayerName[16];
 	BYTE		unobserved_42[72 - 66];
};
#pragma pack(pop)
typedef struct ITEMSAVE ITEMSAVE;

struct ITEMSAVELIST
{
 	ITEMSAVE	atItem[22752 / 72];
 	BYTE		unobserved_58e0[22800 - 22752];
 	int			nCount;
};
typedef struct ITEMSAVELIST ITEMSAVELIST;

struct PLRTRADEITEM
{
 	DWORD		dwItemId;
 	DWORD		dwCopyId;
 	struct PLRTRADEITEM	*pNext;
};
typedef struct PLRTRADEITEM PLRTRADEITEM;
struct PLRTRADE
{
 	int			nSize;
 	BYTE		*pbData;
 	PLRTRADEITEM	*ptItems;
 	DWORD		dwGoldOffered;
 	DWORD		dwStartGold;
 	BYTE		unobserved_014[22824 - 20];
};
typedef struct PLRTRADE PLRTRADE;

struct CHARTEMPLATETXT
{
 	BYTE		unobserved_000[36];
 	int			nClassId;
 	BYTE		level;
 	BYTE		act;
 	BYTE		str;
 	BYTE		dex;
 	BYTE		Int;
 	BYTE		vit;
 	BYTE		Mana;
 	BYTE		Hitpoints;
 	BYTE		ManaRegenBonus;
 	BYTE		Velocity;
 	BYTE		AttackRate;
 	BYTE		OtherRate;
 	int			RightSkill;
 	int			Skill[9];
 	int			SkillLevel[9];
 	INITITEM	tItem[15];
};
typedef struct CHARTEMPLATETXT CHARTEMPLATETXT;

#define UNIT_MONSTER	1

#define MAX_QUEST_STATUS	36

#define QUESTCALLBACK_CHANGEDLEVEL				3
#define QUESTCALLBACK_PLAYERDROPPEDWITHQUESTITEM	9
#define QUESTCALLBACK_PLAYERSTARTEDGAME			13
#define QUESTCALLBACK_PLAYERJOINEDGAME			14
#define NUM_QUESTCALLBACKS		15

struct QUEST;

struct QUESTPARAM
{
 	GAME		*ptGame;
 	int			eCallback;
 	UNIT		*hEventUnit;
 	UNIT		*hPlayer;
 	DWORD		field_10;
 	DWORD		field_14;
 	DWORD		field_18;
};
typedef struct QUESTPARAM QUESTPARAM;

typedef void (__fastcall *PFNQUESTCALLBACK)(struct QUEST *pQuest, QUESTPARAM *pqParam);
typedef BYTE (__fastcall *PFNQUESTSTATUSFILTER)(struct QUEST *pQuest, UNIT *hPlayer, void *pGameRecord,
	void *pPlayerRecord, BYTE *pbStatus);
typedef BYTE (__fastcall *PFNQUESTACTIVEFILTER)(struct QUEST *pQuest, int nNpcClass, UNIT *hPlayer,
	void *pPlayerRecord);
typedef BYTE (__fastcall *PFNQUESTSEQUENCE)(struct QUEST *pQuest);
typedef BYTE (__fastcall *PFNQUESTTIMER)(GAME *ptGame, struct QUEST *pQuest);
typedef void (__fastcall *PFNQUESTINIT)(struct QUEST *pQuest);

typedef BOOL (__fastcall *PFNSUNITITERATE)(GAME *ptGame, UNIT *hUnit, void *pArg);

struct QUESTNPCENTRY
{
 	int			nNpcClass;
 	WORD		wArg;
 	BYTE		unobserved_06[2];
 	DWORD		dwFlag;
};
struct QUESTNPCTABLE
{
 	struct QUESTNPCENTRY	tEntry[16];
 	int			nCount;
};
typedef struct QUESTNPCTABLE QUESTNPCTABLE;

struct QUESTLEVELMSG
{
 	DWORD		eType;
 	DWORD		dwClassId;
};
typedef struct QUESTLEVELMSG QUESTLEVELMSG;

struct QUESTPLAYERS
{
 	DWORD		adwUnitIds[32];
 	WORD		wCount;
 	BYTE		unobserved_82[2];
};
typedef struct QUESTPLAYERS QUESTPLAYERS;

struct QUEST
{
 	int			Id;
 	GAME		*ptGame;
 	BYTE		bAct;
 	BYTE		bActive;
 	BYTE		bCallbacksEnabled;
 	BYTE		bStatus;
 	BYTE		bState;
 	BYTE		bDoneStateMin;
 	BYTE		unobserved_0e[2];
 	DWORD		dwNextQuestId;
 	BYTE		bStatusFlags;
 	BYTE		unobserved_15[3];
 	void		*pQuestRecord;
 	QUESTPLAYERS	tPlayers;
 	PFNQUESTCALLBACK	pCallback[NUM_QUESTCALLBACKS];
 	const QUESTNPCTABLE	*ptNpcTables;
 	int			eFilter;
 	PFNQUESTSTATUSFILTER	pStatusFilter;
 	PFNQUESTACTIVEFILTER	pActiveFilter;
 	PFNQUESTSEQUENCE	pSequence;
 	struct QUEST	*pNext;
};
typedef struct QUEST QUEST;

struct QUESTLINK
{
 	QUEST		*pQuest;
 	struct QUESTLINK	*pNext;
};
typedef struct QUESTLINK QUESTLINK;
struct QUESTTIMER
{
 	PFNQUESTTIMER	pCallback;
 	QUEST		*pQuest;
 	DWORD		dwDue;
 	DWORD		dwInterval;
 	struct QUESTTIMER	*pNext;
};
typedef struct QUESTTIMER QUESTTIMER;
struct QUESTHEAD
{
 	QUEST		*pQuestList;
 	BOOL		bExecuting;
 	BOOL		bPickedSet;
 	void		*pQuestRecord;
 	QUESTTIMER	*pTimers;
 	DWORD		dwTick;
 	D2SEED		tSeed;
 	BYTE		bTrigger;
 	BYTE		unobserved_21[3];
};
typedef struct QUESTHEAD QUESTHEAD;

struct AICOMMAND
{
 	struct AICOMMAND	*ptNext;
 	struct AICOMMAND	*ptPrev;
 	int			nCommand;
 	DWORD		dwArg1;
 	DWORD		dwArg2;
 	DWORD		dwArg3;
 	DWORD		dwArg4;
};
typedef struct AICOMMAND AICOMMAND;

struct A2Q2RECORD
{
 	BYTE		unobserved_00[20];
 	int			nDropCount;
 	int			nShaftCount;
 	int			nStaffCount;
 	int			nCubeCount;
 	int			nAmuletCount;
 	BYTE		bShaftDropped;
 	BYTE		bCubeDropped;
 	BYTE		bStaffMade;
 	BYTE		bMissingReported;
 	DWORD		dwStaffMakerId;
};
typedef struct A2Q2RECORD A2Q2RECORD;

struct A2Q6RECORD
{
 	BYTE		field_00;
 	BYTE		bDurielKilled;
 	BYTE		field_02;
 	BYTE		bPortalTimerPending;
 	BYTE		bOrificePresent;
 	BYTE		bTyraelDoorPresent;
 	BYTE		field_06;
 	BYTE		unobserved_07;
 	BYTE		bStatus1Pending;
 	BYTE		bStatus4Pending;
 	BYTE		bStatus6Pending;
 	BYTE		bObjectsFound;
 	BYTE		bQuest10Finished;
 	BYTE		bOpenPending;
 	BYTE		bStaffPlaced;
 	BYTE		bTownPortalOpened;
 	BYTE		bForcedStatus;
 	BYTE		unobserved_11[3];
 	int			nForcedStatus;
 	DWORD		dwTyraelDoorMode;
 	DWORD		field_1c;
 	DWORD		dwOrificeId;
 	BYTE		unobserved_24[4];
 	DWORD		field_28;
 	DWORD		dwTyraelDoorId;
 	BYTE		unobserved_30[4];
 	DWORD		dwDrawnLevelId;
 	BYTE		bClassesFilled;
 	BYTE		unobserved_39;
 	WORD		wClassIndex;
 	BYTE		bPortalSpotWanted;
 	BYTE		bFinishedPlayerSeen;
 	BYTE		unobserved_3e[2];
 	UNIT		*hTyrael;
 	BYTE		bPlayerNear;
 	BYTE		unobserved_45[3];
 	DWORD		adwClasses[6];
 	GAMEROOM	*hDurielRoom;
};
typedef struct A2Q6RECORD A2Q6RECORD;

struct A2Q3RECORD
{
 	BYTE		unobserved_00;
 	BYTE		bTimerPending;
 	BYTE		bAltCycleOn;
 	BYTE		bAltCyclePending;
 	BYTE		bObjectOperated;
 	BYTE		field_05;
 	BYTE		field_06;
 	BYTE		unobserved_07;
 	DWORD		dwObjectMode;
 	DWORD		dwObjectId;
 	struct GAMEROOM	*hObjectRoom;
 	QUESTPLAYERS	tPlayers;
 	DWORD		dwObjectLevelId;
 	int			nDropCount;
};
typedef struct A2Q3RECORD A2Q3RECORD;
struct A2Q4RECORD
{
 	BYTE		field_00;
 	BYTE		field_01;
 	BYTE		field_02;
 	BYTE		field_03;
 	BYTE		bStatus2Pending;
 	BYTE		unobserved_05[7];
 	BYTE		bFirstJerhynOut;
 	BYTE		bJerhynSpawned;
 	BYTE		bBlockerMode2Set;
 	BYTE		bJerhynArrived;
 	BYTE		bArrivalTaken;
 	BYTE		bBlockerCreated;
 	BYTE		bQuest13State1Seen;
 	BYTE		bJerhynWalking;
 	BYTE		bJerhynPlaced;
 	BYTE		bJerhynSpawnSpotSet;
 	BYTE		bPortalToLevel46Opened;
 	BYTE		unobserved_17;
 	BYTE		field_18;
 	BYTE		field_19;
 	BYTE		bPlayerNear;
 	BYTE		unobserved_1b;
 	DWORD		dwNearPlayerId;
 	int			nBlockerX;
 	int			nBlockerY;
 	int			nJerhynSpawnX;
 	int			nJerhynSpawnY;
 	int			nKaelanX;
 	int			nKaelanY;
 	DWORD		dwBlockerId;
 	DWORD		dwFirstJerhynId;
 	WORD		wBlockerMode;
 	WORD		wArcanePortalMode;
 	WORD		wBasementPortalMode;
 	BYTE		bBlockerMode0To2;
 	BYTE		unobserved_47;
};
typedef struct A2Q4RECORD A2Q4RECORD;

struct A2Q8RECORD
{
 	BYTE		field_00;
 	BYTE		field_01;
};
typedef struct A2Q8RECORD A2Q8RECORD;

struct A3Q0RECORD
{
 	BYTE		bHratliSpawned;
 	BYTE		bSpotHratliSpawned;
 	BYTE		bSpotSet;
 	BYTE		bHratliFound;
 	int			nSpotX;
 	int			nSpotY;
 	DWORD		dwHratliId;
};
typedef struct A3Q0RECORD A3Q0RECORD;

struct A3Q1RECORD
{
 	BYTE		bRewardPending;
 	BYTE		bObjectOperated;
 	BYTE		bBookInGame;
 	BYTE		unobserved_03;
 	BYTE		bPartyHasBook;
 	BYTE		bBookLost;
 	BYTE		unobserved_06[2];
 	int			nBookCount;
 	BYTE		bBookHandedIn;
 	BYTE		unobserved_0d[3];
 	DWORD		dwHandedInById;
 	DWORD		dwObjectId;
 	DWORD		dwObjectMode;
 	QUESTPLAYERS	tPlayers;
};
typedef struct A3Q1RECORD A3Q1RECORD;

struct A3Q2RECORD
{
 	BYTE		bStairsPresent;
 	BYTE		bStatus1Pending;
 	BYTE		unobserved_02[2];
 	DWORD		dwStairsId;
 	DWORD		dwStairsMode;
 	int			nDropCount;
 	int			nEyeCount;
 	int			nBrainCount;
 	int			nHeartCount;
 	int			nFlail1Count;
 	int			nFlail2Count;
 	BYTE		bEyeDropped;
 	BYTE		bBrainDropped;
 	BYTE		bHeartDropped;
 	BYTE		bFlail1Dropped;
 	BYTE		bPartsMissing;
 	BYTE		unobserved_29[3];
 	QUESTPLAYERS	tPlayers;
};
typedef struct A3Q2RECORD A3Q2RECORD;

struct A3Q5RECORD
{
 	BYTE		bStatus2Pending;
 	BYTE		bMonsterAdded;
 	BYTE		unobserved_02[2];
 	DWORD		dwStarterDone;
 	BYTE		bFlailPlaced;
 	BYTE		bFlail1Dropped;
 	BYTE		bCubeDropped;
 	BYTE		unobserved_0b;
 	DWORD		adwMonsterIds[6];
 	BYTE		bOrbSpawned;
 	BYTE		unobserved_25[3];
 	DWORD		dwOrbId;
 	int			nMonsterCount;
 	int			nMonstersLeft;
 	int			nFlailOperations;
 	int			nFlail1DropCount;
 	int			nCubeDropCount;
 	BYTE		bPartsMissingTold;
 	BYTE		unobserved_41[76 - 65];
};
typedef struct A3Q5RECORD A3Q5RECORD;

struct A3Q3RECORD
{
 	BYTE		bItemDropped;
 	BYTE		bStatus2Pending;
 	BYTE		bBossSpawned;
 	BYTE		bObjectOperated;
 	BYTE		bSpawningBoss;
 	BYTE		bTimerPending;
 	BYTE		bOpenGatePending;
 	BYTE		bItemHandedIn;
 	int			nDecoyX;
 	int			nDecoyY;
 	int			nAltarX;
 	int			nAltarY;
 	BYTE		bDecoyPresent;
 	BYTE		unobserved_19[3];
 	int			nItemCount;
 	BYTE		bItemLost;
 	BYTE		unobserved_21[3];
 	DWORD		dwBossId;
 	DWORD		dwAltarId;
 	DWORD		dwAltarMode;
};
typedef struct A3Q3RECORD A3Q3RECORD;

struct A3Q4RECORD
{
 	BYTE		bNpcSkillPending;
 	BYTE		bPickAllowed;
 	BYTE		bMonsterPicked;
 	BYTE		unobserved_03;
 	DWORD		dwPickedMonsterId;
 	BYTE		bStatus5Pending;
 	BYTE		bStatus2Pending;
 	BYTE		bStatus4Pending;
 	BYTE		bStatus3Pending;
 	BYTE		bFigurinePending;
 	BYTE		unobserved_0d[3];
 	int			nItemCount;
 	BYTE		bFigurineDropped;
 	BYTE		bItemLost;
 	BYTE		unobserved_16[2];
 	DWORD		dwRecordBit;
};
typedef struct A3Q4RECORD A3Q4RECORD;

struct A3Q6RECORD
{
 	BYTE		bTimerPending;
 	BYTE		bHellgatePresent;
 	BYTE		bBridgePresent;
 	BYTE		bStatus2Pending;
 	DWORD		dwHellgateId;
 	DWORD		dwBridgeId;
 	DWORD		dwHellgateMode;
 	DWORD		dwBridgeMode;
 	int			nMssDropped;
 	BYTE		bMssDropped;
 	BYTE		unobserved_19[3];
 	int			nCreditedPlayers;
 	BYTE		bNatalyaSpawned;
 	BYTE		unobserved_21[3];
 	DWORD		dwSavedStatus;
 	DWORD		dwNatalyaId;
};
typedef struct A3Q6RECORD A3Q6RECORD;

struct A3Q7RECORD
{
 	BYTE		bSpotSet;
 	BYTE		bDarkWandererPending;
 	BYTE		bSpotMoved;
 	BYTE		unobserved_03;
 	DWORD		dwSpotX;
 	DWORD		dwSpotY;
 	BYTE		bTimerDone;
 	BYTE		bTimerPending;
 	BYTE		unobserved_0e[2];
 	DWORD		dwMonsterId;
};
typedef struct A3Q7RECORD A3Q7RECORD;

struct A4Q1RECORD
{
 	BYTE		unobserved_00[4];
 	GAMECOORD	tDeathSpot;
 	BYTE		field_0c;
 	BYTE		bTimerPending;
 	BYTE		unobserved_0e;
 	BYTE		field_0f;
 	BYTE		bSpawnPending;
 	BYTE		bTyraelMessageDone;
 	BYTE		bIzualMessageDone;
 	BYTE		unobserved_13;
 	UNIT		*hMonster;
 	BYTE		bPlayerNear;
 	BYTE		unobserved_19[3];
};
typedef struct A4Q1RECORD A4Q1RECORD;

struct PARTYMEMBER
{
 	DWORD		dwUnitId;
 	struct PARTYMEMBER	*pNext;
};
typedef struct PARTYMEMBER PARTYMEMBER;

struct PARTY
{
 	WORD		wId;
 	BYTE		unobserved_02[2];
 	PARTYMEMBER	*pMembers;
 	struct PARTY	*pNext;
};
typedef struct PARTY PARTY;

struct PARTYSYSTEM
{
 	WORD		wNextId;
 	BYTE		unobserved_02[2];
 	PARTY		*pFirst;
};
typedef struct PARTYSYSTEM PARTYSYSTEM;

struct A4Q2RECORD
{
 	BYTE		bTyraelMessageDone;
 	BYTE		bTimerPending;
 	BYTE		bSpawnPhase;
 	BYTE		bEndPhase;
 	BYTE		bDropHostPending;
 	BYTE		bWarpPending;
 	BYTE		bSpawnObjectKnown;
 	BYTE		unobserved_07;
 	DWORD		dwSpawnObjectId;
 	BYTE		abSealsOperated[5];
 	BYTE		bDiabloSpawned;
 	BYTE		unobserved_12;
 	BYTE		bMonstersReleased;
 	BYTE		bDiabloKilled;
 	BYTE		bClientsFlushed;
 	BYTE		unobserved_16[2];
 	DWORD		field_18;
 	int			nPlayersCredited;
 	UNIT		*hLastWarpedPlayer;
 	struct GAMECOORD	tFirstSpawnSpot;
 	struct GAMECOORD	tSecondSpawnSpot;
 	struct GAMECOORD	tThirdSpawnSpot;
 	GAMEROOM	*hDiabloDeathRoom;
 	int			nOtherKills;
};
typedef struct A4Q2RECORD A4Q2RECORD;

struct A4Q3RECORD
{
 	WORD		wHellforgeMode;
 	BYTE		bCainMessageDone;
 	BYTE		bGemsPending;
 	BYTE		bFinished;
 	BYTE		unobserved_05[3];
 	int			nDropsLeft;
 	int			nOperations;
 	int			nHammers;
 	int			nGemsPerDrop;
 	BYTE		unobserved_18[4];
};
typedef struct A4Q3RECORD A4Q3RECORD;

struct PLAYERLIST
{
 	DWORD		dwUnitId;
 	DWORD		dwFlags;
 	DWORD		dwState;
 	struct PLAYERLIST	*pNext;
};
typedef struct PLAYERLIST PLAYERLIST;

struct UNITANIMREC
{
 	BYTE		unobserved_00[8];
 	DWORD		dwFrames;
 	BYTE		unobserved_0c[4];
 	BYTE		abFrameEvents[1];
};

struct LVLWARPTXT
{
 	int			SelectX;
 	int			SelectY;
 	int			SelectDX;
 	int			SelectDY;
 	int			ExitWalkX;
 	int			ExitWalkY;
 	int			OffsetX;
 	int			OffsetY;
 	int			LitVersion;
};
typedef struct LVLWARPTXT LVLWARPTXT;

struct PRESETUNIT
{
 	int			nUnitType;
 	int			nClassId;
 	int			nMode;
 	int			nX;
 	int			nY;
 	DWORD		dwFlags;
 	void		*ptPath;
 	struct PRESETUNIT	*ptNext;
};
typedef struct PRESETUNIT PRESETUNIT;

struct DAMAGEHOLD
{
 	GAME		*ptGame;
 	DWORD		eType;
 	DWORD		dwUnitId;
 	DWORD		eTargetType;
 	DWORD		dwTargetId;
 	DAMAGE		tDamage;
 	struct DAMAGEHOLD	*ptNext;
 	struct DAMAGEHOLD	*ptTargetNext;
};
typedef struct DAMAGEHOLD DAMAGEHOLD;

struct NPCITEMSTAT
{
 	BYTE		Min;
 	BYTE		Max;
 	BYTE		MagicMin;
 	BYTE		MagicMax;
 	DWORD		dwClassId;
 	int			nInvCount[2];
 	BYTE		MagicLvl;
 	BYTE		unobserved_11[3];
};
typedef struct NPCITEMSTAT NPCITEMSTAT;

struct NPCITEMLIST
{
 	NPCITEMSTAT	*pItemStats;
 	int			nItemStats;
};
typedef struct NPCITEMLIST NPCITEMLIST;

struct NPCPLAYERINV
{
 	void		*ptInventory;
 	DWORD		dwUnitId;
 	struct NPCPLAYERINV	*ptNext;
};
typedef struct NPCPLAYERINV NPCPLAYERINV;

struct NPCORDER
{
 	DWORD		field_00;
 	DWORD		field_04;
 	DWORD		field_08;
 	DWORD		dwHoldsItem;
 	struct NPCORDER	*ptNext;
};
typedef struct NPCORDER NPCORDER;

struct NPCVISIT
{
 	DWORD		dwUnitId;
 	BYTE		bOwnStock;
 	BYTE		unobserved_05[3];
 	struct NPCVISIT	*ptNext;
};
typedef struct NPCVISIT NPCVISIT;

struct NPCSOLD
{
 	DWORD		dwClassId;
 	DWORD		field_04;
 	DWORD		field_08;
 	DWORD		dwUnitId;
 	DWORD		dwTick;
 	struct NPCSOLD	*ptNext;
};
typedef struct NPCSOLD NPCSOLD;

struct NPCSTATE
{
 	BYTE		bStockFilled;
 	BYTE		bHiresDrawn;
 	BYTE		bAct;
 	BYTE		bVendor;
 	BYTE		field_04;
 	BYTE		field_05;
 	BYTE		field_06;
 	BYTE		bRenewPending;
 	DWORD		dwTick;
 	NPCITEMSTAT	*pItemStats;
 	int			nItemStats;
};
typedef struct NPCSTATE NPCSTATE;

struct NPCPROXY
{
 	DWORD		dwClassId;
 	void		*ptInventory;
 	NPCPLAYERINV	*ptPlayerInvs;
 	DWORD		dwGambler;
 	struct MONPETDESC	*ptHires;
 	NPCORDER	*ptOrders;
 	NPCVISIT	*ptVisits;
 	DWORD		dwTradeOpened;
 	NPCSTATE	ItemList;
 	NPCSOLD		*ptSoldItems;
 	BYTE		abSoldCounts[4];
 	DWORD		field_3c;
 	DWORD		dwUnitId;
};
typedef struct NPCPROXY NPCPROXY;

struct MONPETDESC
{
 	WORD		wHireId;
 	BYTE		unobserved_02[2];
 	DWORD		dwSeed;
 	DWORD		dwHired;
 	DWORD		dwOffered;
};
typedef struct MONPETDESC MONPETDESC;
#define NPCHIRE_COUNT	41

#define MAX_NPC		64

struct SUNITPROXY
{
 	int			nNpcs;
 	NPCPROXY	*ptNpcs;
 	D2SEED		tSeed;
 	DWORD		field_10;
};
typedef struct SUNITPROXY SUNITPROXY;

struct AIRECORDHEAD
{
 	int			field_00;
 	BYTE		unobserved_04[4];
 	WORD		wFlags;
};
typedef struct AIRECORDHEAD AIRECORDHEAD;

#define AIFLAG_01			1
#define AIFLAG_02			2
#define AIFLAG_08			8
#define AIFLAG_20			32
#define AIFLAG_40			64

struct AIMINION
{
 	DWORD		dwUnitId;
 	struct AIMINION	*ptNext;
};
typedef struct AIMINION AIMINION;

struct AIPARMS;
typedef void (__fastcall *PFNAITHINK)(struct GAME *ptGame, struct UNIT *hUnit, struct AIPARMS *ptParms);

struct AIGENERAL
{
 	int			nThinkKind;
 	PFNAITHINK	pfnAi;
 	WORD		wFlags;
 	BYTE		unobserved_0a[2];
 	DWORD		dwRefUnitId;
 	DWORD		dwRefUnitType;
 	DWORD		adwParam[3];
 	AICOMMAND	*ptCommand;
 	AICOMMAND	*ptCommandLast;
 	struct GAME	*ptGame;
 	DWORD		dwOwnerId;
 	DWORD		dwOwnerType;
 	AIMINION	*ptMinions;
 	void		*pCell;
};
typedef struct AIGENERAL AIGENERAL;

typedef void (__fastcall *PFNAIMINIONFN)(UNIT *hMinion, DWORD dwArg1, DWORD dwArg2);

struct DRLGLOGICNODE
{
 	BYTE		unobserved_00[16];
 	RECT		tRect;
 	BOOL		field_20;
 	BOOL		fSkipTargetCheck;

 	int			nLogic;
 	struct DRLGLOGICNODE	*ptNext;
};
typedef struct DRLGLOGICNODE DRLGLOGICNODE;

struct AIPARMS
{
 	AIGENERAL	*ptAi;
 	UNIT		*hTarget;
 	DWORD		field_08;
 	DWORD		field_0c;
 	int			nDist;
 	BOOL		fTargetInReach;
 	MONSTATSTXT	*ptMonStats;
 	WORD		wFlags;
 	BYTE		unobserved_1e[2];
};
typedef struct AIPARMS AIPARMS;

struct PRESETPATHNODE
{
 	int			nAction;
 	int			nX;
 	int			nY;
};
typedef struct PRESETPATHNODE PRESETPATHNODE;
struct PRESETFILELIST
{
 	int			nNodes;
 	PRESETPATHNODE	*pArray;
};
typedef struct PRESETFILELIST PRESETFILELIST;

typedef struct SKILLFINDTARGET SKILLFINDTARGET;
typedef struct SKILLSUMMON SKILLSUMMON;

typedef void (__fastcall *PFNAREAAFFECT)(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nLevel, DWORD dwArg);
struct SKILLEFFECT
{
 	DWORD		dwFlags;
 	UNIT		*hUnit;
 	int			nSkill;
 	int			nX;
 	int			nY;
 	DWORD		field_14;
 	int			nLevel;
 	DWORD		dwAffectArg;
 	PFNAREAAFFECT	pfnAreaAffect;
};
typedef struct SKILLEFFECT SKILLEFFECT;

#endif
