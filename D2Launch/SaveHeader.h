#ifndef D2LAUNCH_SAVEHEADER_H
#define D2LAUNCH_SAVEHEADER_H
#include <windows.h>
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
 	union
	{
		struct
		{
			WORD	wGuildFlags;
			DWORD	dwGuildTag;
		};
		DWORD	dwGuild59;
	};
 	char		szGuildName[28];
 	BYTE		abGuildEmblem[3];
 	DWORD		dwSeed;
};
#pragma pack(pop)
#endif
