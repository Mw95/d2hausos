#define D2_THIS_FILE "C:\\D2\\Source\\D2Launch\\Src\\SaveLoad.cpp"
#include "D2Launch.h"
#include "SaveHeader.h"
#include "D2Macros.h"

static SAVEHEADER sgtSaveHeader = { 0 };

#define SAVELOAD_DWORD59(tHeader) \
	((tHeader).dwGuild59)

static BYTE sgabNewGfx[16] =
{
	1, 1, 1, 1, 1, 255, 255, 255, 1, 1, 255, 255, 255, 255, 255, 255
};

void __fastcall SaveLoadNew(const char *pszName, BYTE bClass, BYTE *pbSummary, WORD wFlags)
{
	BYTE *pb;
	DWORD dwFlags;
	int i;

	memset(&sgtSaveHeader, 0, sizeof(sgtSaveHeader));
	sgtSaveHeader.dwMagic = 0xAA55AA55;
	sgtSaveHeader.dwVersion = 71;
	strcpy(sgtSaveHeader.szName, pszName);
	sgtSaveHeader.wField1c = 221;
	sgtSaveHeader.wField1e = 16;
	sgtSaveHeader.wClass = bClass;
	sgtSaveHeader.wSize = 130;
	sgtSaveHeader.wLevel = 1;
	sgtSaveHeader.dwFlags = wFlags | 1;
	pb = pbSummary;
	FogD2QSSocketWriteSevenBitPair(pb, 0);
	pb += 2;
	for (i = 0; i < 16; i++)
	{
		*pb++ = sgabNewGfx[i];
		sgtSaveHeader.abComponentGfx[i] = sgabNewGfx[i];
	}
	bClass++;
	*pb++ = bClass;
	for (i = 0; i < 16; i++)
	{
		*pb++ = 255;
		sgtSaveHeader.abComponentColors[i] = 255;
	}
	dwFlags = sgtSaveHeader.dwFlags;
	*pb++ = 1;
	FogD2QSSocketWriteSevenBitPair(pb, dwFlags);
	pb += 2;
	FogD2QSSocketWriteSevenBitPair(pb, 0);
	pb += 2;
	*pb++ = 255;
	*pb++ = 255;
	*pb++ = 255;
	strcpy((char *)pb, (const char *)&sgtSaveHeader.dwGuildTag);
}

void __fastcall SaveLoadSetExtraFlag(BOOL fSet)
{
	if (fSet)
		sgtSaveHeader.dwFlags |= 2;
}

BOOL __fastcall SaveLoadWrite(const char *pszName, const BYTE *pbExtra)
{
	char szSave[1024];
	char szPath[500];
	DWORD dwWritten;
	DWORD hFile;

	FogSafesockGetSavePath(szSave, 1024);
	sprintf(szPath, "%s%s.d2s", szSave, pszName);
	hFile = (DWORD)FogSafesockCreateFile(szPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, 0);
	if (!hFile)
		return FALSE;
	FogSafesockWriteFile(hFile, &sgtSaveHeader, sizeof(sgtSaveHeader), &dwWritten, 0);
	if (pbExtra)
		FogSafesockWriteFile(hFile, pbExtra, 128, &dwWritten, 0);
	FogSafesockCloseHandle(hFile);
	return TRUE;
}

void __fastcall SaveLoadDelete(const char *pszFileName)
{
	char szSave[1024];
	char szFile[500];
	char szPattern[500];
	WIN32_FIND_DATAA tFind;

#line 210
	D2ASSERT(*pszFileName != '\0');
	FogSafesockGetSavePath(szSave, 1024);
	sprintf(szPattern, "%s%s.*", szSave, pszFileName);
	while (FindFirstFileA(szPattern, &tFind) != INVALID_HANDLE_VALUE)
	{
		sprintf(szFile, "%s%s", szSave, tFind.cFileName);
		if (!DeleteFileA(szFile))
			break;
	}
}

void __fastcall SaveLoadRead(const char *pszFileName, BYTE *pbClass, WORD *pwLevel, BOOL *pfExtra,
	BYTE *pbExtra)
{
	char szSave[1024];
	char szPath[500];
	TPLAYERtSaveHeader tHeader;
	DWORD nRead;
	DWORD nSize;
	DWORD hFile;

#line 238
	D2ASSERT(*pszFileName != '\0');
	FogSafesockGetSavePath(szSave, 1024);
	sprintf(szPath, "%s%s.d2s", szSave, pszFileName);
	hFile = (DWORD)FogSafesockCreateFile(szPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN,
		0);
	FogSafesockReadFile(hFile, &tHeader, sizeof(TPLAYERtSaveHeader), &nSize, 0);
#line 255
	D2ASSERT(nSize == sizeof(TPLAYERtSaveHeader));
	*pbClass = (BYTE)tHeader.wClass;
	if (tHeader.dwFlags & 2)
	{
		*pfExtra = TRUE;
		FogSafesockReadFile(hFile, pbExtra, 128, &nRead, 0);
		if (nRead == 0)
			*pbExtra = 0;
	}
	else
		*pfExtra = FALSE;
	*pwLevel = tHeader.wLevel;
	FogSafesockCloseHandle(hFile);
}

void __fastcall SaveLoadWriteSummary(const char *pszName, const BYTE *pbSummary, const BYTE *pbExtra)
{
	char szSave[1024];
	char szPath[500];
	char szText[4];
	DWORD dwWritten;
	DWORD dwFlags;
	DWORD hFile;
	WORD wGuildFlags;
	WORD wClass;
	WORD wLevel;
	BYTE b7b;
	BYTE b7c;
	BYTE b7d;

	wClass = pbSummary[18];
	wLevel = pbSummary[35];
	dwFlags = FogD2QSSocketReadSevenBitPair(pbSummary + 36);
	wGuildFlags = (WORD)FogD2QSSocketReadSevenBitPair(pbSummary + 38);
	b7b = pbSummary[40];
	b7c = pbSummary[41];
	b7d = pbSummary[42];
	b7b = (BYTE)(b7b == 255 ? 0 : b7b);
	b7c = (BYTE)(b7c == 255 ? 0 : b7c);
	b7d = (BYTE)(b7d == 255 ? 0 : b7d);
	memset(szText, 0, sizeof(szText));
	strcpy(szText, (const char *)(pbSummary + 42));
	dwFlags &= 65535;
	memset(&sgtSaveHeader, 0, sizeof(sgtSaveHeader));
	sgtSaveHeader.wClass = wClass;
	sgtSaveHeader.wLevel = wLevel;
	sgtSaveHeader.abGuildEmblem[0] = b7b;
	sgtSaveHeader.dwMagic = 0xAA55AA55;
	sgtSaveHeader.dwVersion = 71;
	sgtSaveHeader.wField1c = 221;
	sgtSaveHeader.wField1e = 16;
	sgtSaveHeader.wSize = 130;
	sgtSaveHeader.dwFlags = dwFlags;
	sgtSaveHeader.wGuildFlags = wGuildFlags;
	sgtSaveHeader.abGuildEmblem[1] = b7c;
	sgtSaveHeader.abGuildEmblem[2] = b7d;
	memcpy(&sgtSaveHeader.dwGuildTag, szText, 3);
	((char *)&sgtSaveHeader.dwGuildTag)[3] = '\0';
	FogSafesockGetSavePath(szSave, 1024);
	sprintf(szPath, "%s%s.d2s", szSave, pszName);
	hFile = (DWORD)FogSafesockCreateFile(szPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, 0);
	FogSafesockWriteFile(hFile, &sgtSaveHeader, sizeof(sgtSaveHeader), &dwWritten, 0);
	if (pbExtra)
	{
		DWORD dwWritten;

		FogSafesockWriteFile(hFile, pbExtra, 128, &dwWritten, 0);
	}
	FogSafesockCloseHandle(hFile);
}

BOOL __fastcall SaveLoadSummary(const char *pszFileName, BYTE *pbSummary)
{
	char szSave[1024];
	char szPath[500];
	TPLAYERtSaveHeader tHeader;
	FILE *ptFile;
	BOOL fRead;
	BYTE *pb;
	BYTE b7b;
	BYTE b7c;
	BYTE b7d;
	int i;

	fRead = FALSE;
	pb = pbSummary;
#line 378
	D2ASSERT(*pszFileName != '\0');
	FogSafesockGetSavePath(szSave, 1024);
	sprintf(szPath, "%s%s.d2s", szSave, pszFileName);
	ptFile = fopen(szPath, "rb");
	if (!ptFile)
		return FALSE;
	if (fread(&tHeader, sizeof(TPLAYERtSaveHeader), 1, ptFile) && tHeader.dwMagic == 0xAA55AA55
		&& tHeader.dwVersion == 71)
	{
		FogD2QSSocketWriteSevenBitPair(pb, 0);
		pb += 2;
		if (tHeader.abComponentGfx[0] == 0)
		{
			for (i = 0; i < 16; i++)
				*pb++ = sgabNewGfx[i];
		}
		else
		{
			for (i = 0; i < 16; i++)
				*pb++ = tHeader.abComponentGfx[i];
		}
		*pb++ = (BYTE)((BYTE)tHeader.wClass + 1);
		for (i = 0; i < 16; i++)
			*pb++ = tHeader.abComponentColors[i];
		*pb++ = (BYTE)tHeader.wLevel;
		FogD2QSSocketWriteSevenBitPair(pb, tHeader.dwFlags);
		pb += 2;
		FogD2QSSocketWriteSevenBitPair(pb, SAVELOAD_DWORD59(tHeader));
		pb += 2;
		b7b = tHeader.abGuildEmblem[0];
		b7c = tHeader.abGuildEmblem[1];
		b7d = tHeader.abGuildEmblem[2];
		if (tHeader.abGuildEmblem[0] == 0)
			b7b = 255;
		if (tHeader.abGuildEmblem[1] == 0)
			b7c = 255;
		if (tHeader.abGuildEmblem[2] == 0)
			b7d = 255;
		*pb++ = b7b;
		fRead = TRUE;
		*pb++ = b7c;
		*pb = b7d;
		pb[1] = 0;
	}
	fclose(ptFile);
	return fRead;
}

void __fastcall MainMenusBackground(void)
{
	MainMenusClear();
	MainMenusCreateChild(8);
	MainMenusCreateChild(6);
	MainMenusCreateChild(7);
	MainMenusLoadPalette(gszPaletteSkyDat, gszPaletteSkyPl2);
}
