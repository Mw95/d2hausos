#define D2_THIS_FILE "C:\\D2\\Source\\D2Sound\\Src\\D2SoundUtil.cpp"
#include <math.h>
#include <stdlib.h>
#include "D2Sound.h"
#include "D2Macros.h"
#include "D2soundImports.h"

struct D2MUSICTRACK
{
 	const char	*pszFile;
 	BOOL		fPlayed;
};

static D2MUSICTRACK sgatMusicTracks[8] =
{
	{ "data\\global\\music\\common\\options.wav", FALSE },
	{ "data\\global\\music\\act1\\caves.wav", FALSE },
	{ "data\\global\\music\\act1\\monastery.wav", FALSE },
	{ "data\\global\\music\\act1\\crypt.wav", FALSE },
	{ "data\\global\\music\\act2\\harem.wav", FALSE },
	{ "data\\global\\music\\act2\\tombs.wav", FALSE },
	{ "data\\global\\music\\act3\\spider.wav", FALSE },
	{ "data\\global\\music\\act3\\kurastsewer.wav", FALSE },
};
static int sgnMusicTracks = 8;

static D2SAMPLE *sgptMusicSample = 0;
static DWORD sgdwSmpState = 0;
static BOOL sgfMusicFirst = 0;

BOOL __fastcall D2SoundSmpIsStream(D2SAMPLE *ptSample);
BOOL __fastcall D2SoundSmpPlayStream(D2SAMPLE *ptSample, const char *pszFile, DWORD dwPosition,
	BOOL fFlag40000);
int __fastcall D2SoundSmpGetMasterVolume(void);
BOOL __fastcall D2SoundSmpMusicPlay(const char *pszFile, BOOL fFlag40000);

void __fastcall D2SoundSmpStormInit(void)
{
	StormSFileDdaInitialize(D2SoundGetDirectSound());
	sgptMusicSample = D2SoundSmpCreate(SAMPLE_STEREO);
}

DWORD __fastcall D2SoundSmpStormShutdown(void)
{
	if (sgptMusicSample)
	{
		D2SoundSmpFree(sgptMusicSample);
		sgptMusicSample = NULL;
	}
	return StormSFileDdaDestroy();
}

BOOL __fastcall D2SoundSmpMusicPlay(const char *pszFile, BOOL fFlag40000)
{
	if (sgptMusicSample && D2SoundSmpGetMasterVolume() && !D2SoundSmpIsStream(sgptMusicSample) && pszFile)
	{
		D2SoundSmpReset(sgptMusicSample);
		D2SoundSmpSetVolume(sgptMusicSample, 110);
		if (D2SoundSmpPlayStream(sgptMusicSample, pszFile, 0, fFlag40000))
			return TRUE;
	}
	return FALSE;
}

void __fastcall D2SoundSmpMusicStop(void)
{
	if (!sgptMusicSample)
		return;
	sgdwSmpState = 0;
	if (D2SoundSmpIsStream(sgptMusicSample))
		D2SoundSmpStop(sgptMusicSample);
}

void __fastcall D2SoundSmpMusicFade(void)
{
	if (!sgptMusicSample)
		return;
	sgdwSmpState = 0;
	if (D2SoundSmpIsStream(sgptMusicSample))
		D2SoundSmpFade(sgptMusicSample, 0, 200);
}

BOOL __fastcall D2SoundSmpMusicIsPlaying(void)
{
	if (sgptMusicSample && D2SoundSmpIsStream(sgptMusicSample))
		return TRUE;
	return FALSE;
}

static BOOL D2SoundSmpMusicAllPlayed(void)
{
	int i;

	for (i = 0; i < sgnMusicTracks; i++)
	{
		if (!sgatMusicTracks[i].fPlayed)
			return FALSE;
	}
	return TRUE;
}

BOOL __fastcall D2SoundSmpMusicNext(void)
{
	BOOL fCleared;
	BOOL fStarted;
	D2MUSICTRACK *ptTrack;
	const char *pszFile;
	int i;

	EnterCriticalSection(&sgtCriticalSection);
	if (sgptMusicSample && D2SoundSmpGetMasterVolume() && !D2SoundSmpMusicIsPlaying())
	{
		fStarted = FALSE;
		fCleared = FALSE;
		for (;;)
		{
			if (D2SoundSmpMusicAllPlayed())
			{
				if (fCleared)
					break;
				for (i = 0; i < sgnMusicTracks; i++)
					sgatMusicTracks[i].fPlayed = FALSE;
				fCleared = TRUE;
			}
			if (sgfMusicFirst)
			{
				ptTrack = &sgatMusicTracks[0];
				sgfMusicFirst = FALSE;
			}
			else
			{
				i = rand() % sgnMusicTracks;
				while (1)
				{
					ptTrack = &sgatMusicTracks[i];
					if (!ptTrack->fPlayed)
						break;
					i++;
					if (i >= sgnMusicTracks)
						i = 0;
				}
			}
			pszFile = ptTrack->pszFile;
			ptTrack->fPlayed = TRUE;
			if (D2SoundSmpMusicPlay(pszFile, FALSE))
			{
				fStarted = TRUE;
				break;
			}
		}
		if (fStarted)
		{
			LeaveCriticalSection(&sgtCriticalSection);
			return TRUE;
		}
		sgdwSmpState = 0;
	}
	LeaveCriticalSection(&sgtCriticalSection);
	return FALSE;
}

void __fastcall D2SoundSmpMusicSetState(DWORD dwState)
{
	int i;

	if (dwState && !sgdwSmpState)
	{
		for (i = 0; i < sgnMusicTracks; i++)
			sgatMusicTracks[i].fPlayed = FALSE;
		sgfMusicFirst = TRUE;
	}
	sgdwSmpState = dwState;
}

DWORD __fastcall D2SoundSmpGetState(void)
{
	return sgdwSmpState;
}

float __stdcall D2SoundUtilRatioToDecibels(float fMin, float fMax, float fValue)
{
	double fLow;
	double fHigh;
	double fLevel;
	double fRatio;

	if (fMin + 0.0001f >= fValue)
		return -10000.0f;
	if (!(fMax - 0.0001f > fValue))
		return 0.0f;
	fLow = fMin;
	fHigh = fMax;
	fLevel = fValue;
	if (fLevel != 0.0)
		fRatio = (fLow + fHigh) / fLevel;
	else
		fRatio = (fLow + fHigh) / fHigh;
	return log10(fRatio) * -2000.0;
}

float __stdcall D2SoundUtilMillibelsToGain(float fMillibels)
{
	if (fMillibels + 0.0001f > 0.0f)
		return 1.0f;
	if (!(fMillibels - 0.0001f >= -10000.0f))
		return 0.0f;
	return pow(10.0, fMillibels * 0.0005f);
}

void __fastcall D2SoundUtilInitWaveFormat(WAVEFORMATEX *ptFormat, int nChannels)
{
	memset(ptFormat, 0, sizeof(WAVEFORMATEX));
	ptFormat->nBlockAlign = (WORD)(nChannels * 2);
	ptFormat->nChannels = (WORD)nChannels;
	ptFormat->wFormatTag = WAVE_FORMAT_PCM;
	ptFormat->nSamplesPerSec = 22050;
	ptFormat->wBitsPerSample = 16;
	ptFormat->nAvgBytesPerSec = ptFormat->nBlockAlign * 22050;
}

static inline BYTE *D2SoundUtilFindChunk(BYTE *pbChunk, DWORD *pdwSize, DWORD dwId, DWORD *pdwLength)
{
	DWORD *pdwHeader;

	while (1)
	{
		if (*pdwSize < 8)
			return NULL;
		pdwHeader = (DWORD *)pbChunk;
		*pdwSize -= 8;
		pbChunk += 8;
		*pdwLength = pdwHeader[1];
		if (pdwHeader[0] == dwId)
			return pbChunk;
		if (*pdwSize < *pdwLength)
			return NULL;
		pbChunk += *pdwLength;
		*pdwSize -= *pdwLength;
	}
}

void * __fastcall D2SoundUtilParseWave(void *pbWaveData, DWORD *pdwWaveSize, WAVEFORMATEX *ptFormat)
{
	DWORD *pdwRiff;
	BYTE *pbChunk;
	PCMWAVEFORMAT *ptFileFormat;
	DWORD dwLength;

	if (*pdwWaveSize >= 32)
	{
		*pdwWaveSize -= 12;
		pdwRiff = (DWORD *)pbWaveData;
		if (pdwRiff[0] == 1179011410 && pdwRiff[2] == 1163280727)
		{
			pbChunk = D2SoundUtilFindChunk((BYTE *)pbWaveData + 12, pdwWaveSize, 544501094, &dwLength);
			if (pbChunk && dwLength >= sizeof(PCMWAVEFORMAT) && *pdwWaveSize >= dwLength)
			{
				*pdwWaveSize -= dwLength;
				ptFileFormat = (PCMWAVEFORMAT *)pbChunk;
				ptFormat->wFormatTag = ptFileFormat->wf.wFormatTag;
				ptFormat->nChannels = ptFileFormat->wf.nChannels;
				ptFormat->nSamplesPerSec = ptFileFormat->wf.nSamplesPerSec;
				ptFormat->nAvgBytesPerSec = ptFileFormat->wf.nAvgBytesPerSec;
				ptFormat->nBlockAlign = ptFileFormat->wf.nBlockAlign;
				ptFormat->wBitsPerSample = ptFileFormat->wBitsPerSample;
				ptFormat->cbSize = 0;
				pbChunk = D2SoundUtilFindChunk(pbChunk + dwLength, pdwWaveSize, 1635017060, &dwLength);
				if (pbChunk && *pdwWaveSize >= dwLength)
				{
					*pdwWaveSize = dwLength;
					return pbChunk;
				}
			}
		}
	}
	return NULL;
}

void __fastcall D2SoundUtilCalcVolumePan(int *pnVolume, int *pnPan, float fX, float fY, float fZ)
{
	double dSquare;
	float fDistance;
	float fDecibels;
	int nLevel;
	int nVolume;
	int nPan;

	dSquare = fX * fX;
	fDistance = (float)sqrt(dSquare + fY * fY + fZ * fZ);
	if (fDistance > 100.0f)
		fDistance = 100.0f;
	if (fDistance < 2.0f)
		fDecibels = 0.0f;
	else
		fDecibels = (float)(log(fDistance * 0.5f) * 1.442695036924675) * 6.0f;
	nLevel = (int)(__int64)(255.0 / pow(10.0, fDecibels * 0.05f));
	if (nLevel > 0)
		nVolume = nLevel;
	else
		nVolume = 0;
	if (nVolume >= 255)
		nVolume = 255;
	nPan = (int)(__int64)((fX * 127.0f) * 0.8f + 128.0f);
	if (nPan <= 0)
		nPan = 0;
	else if (nPan >= 255)
		nPan = 255;
	*pnVolume = nVolume;
	*pnPan = nPan;
}

D2SOUNDLISTNODE * __fastcall D2SoundUtilListPush(D2SOUNDLISTNODE **pptHead, void *pItem)
{
	D2SOUNDLISTNODE *ptNode;

#line 212
	ptNode = (D2SOUNDLISTNODE *)D2ALLOC2(sizeof(D2SOUNDLISTNODE));
	memset(ptNode, 0,
		sizeof(D2SOUNDLISTNODE));
	ptNode->ptNext = *pptHead;
	ptNode->pItem = pItem;
	*pptHead = ptNode;
	return ptNode;
}

void __fastcall D2SoundUtilListRemove(D2SOUNDLISTNODE **pptHead, void *pItem)
{
	D2SOUNDLISTNODE *ptNode;
	D2SOUNDLISTNODE *ptPrevious;

	ptPrevious = NULL;
	for (ptNode = *pptHead; ptNode; ptNode = ptNode->ptNext)
	{
		if (ptNode->pItem == pItem)
		{
			if (*pptHead == ptNode)
				*pptHead = ptNode->ptNext;
			if (ptPrevious)
				ptPrevious->ptNext = ptNode->ptNext;
#line 240
			D2FREE2(ptNode);
			return;
		}
		ptPrevious = ptNode;
	}
}

BOOL __fastcall D2SoundUtilListContains(D2SOUNDLISTNODE *ptHead, void *pItem)
{
	for (; ptHead; ptHead = ptHead->ptNext)
	{
		if (ptHead->pItem == pItem)
			return TRUE;
	}
	return FALSE;
}

int __fastcall D2SoundUtilListCount(D2SOUNDLISTNODE *ptHead)
{
	int nCount;

	nCount = 0;
	for (; ptHead; ptHead = ptHead->ptNext)
		nCount++;
	return nCount;
}

void __fastcall D2SoundUtilListFree(D2SOUNDLISTNODE **pptHead)
{
	D2SOUNDLISTNODE *ptNode;

	ptNode = *pptHead;
	while (1)
	{
		if (!ptNode)
			break;
		D2SoundUtilListRemove(pptHead, ptNode->pItem);
		ptNode = *pptHead;
		if (!ptNode)
			break;
	}
}

D2SOUNDLISTNODE * __fastcall D2SoundUtilListAppend(D2SOUNDLISTNODE **pptHead, void *pItem)
{
	D2SOUNDLISTNODE *ptNode;
	D2SOUNDLISTNODE *ptTail;

#line 287
	ptNode = (D2SOUNDLISTNODE *)D2ALLOC2(sizeof(D2SOUNDLISTNODE));
	memset(ptNode, 0,
		sizeof(D2SOUNDLISTNODE));
	ptNode->pItem = pItem;
	for (ptTail = *pptHead; ptTail; ptTail = ptTail->ptNext)
	{
		if (!ptTail->ptNext)
			break;
	}
	if (ptTail)
		ptTail->ptNext = ptNode;
	else
		*pptHead = ptNode;
	return ptNode;
}

struct SOUNDVXDRESOURCE
{
 	BYTE		unobserved_00[8];
 	DWORD		dwSize;
};

BOOL __fastcall D2SoundUtilReadVxdVersion(const char *pszFile, DWORD *pdwSize, void *pbBuffer)
{
	HANDLE hFile;
	HANDLE hFileMapping;
	BYTE *pbFile;
	BOOL fResult;
	IMAGE_DOS_HEADER *ptDosHeader;
	DWORD *pdwHeader;
	IMAGE_VXD_HEADER *ptNewExe;
	SOUNDVXDRESOURCE *ptWinRes;
	DWORD dwResourceSize;
	const void *pbInfo;
	DWORD dwError;

	hFile = NULL;
	hFileMapping = NULL;
	pbFile = NULL;
	fResult = FALSE;
	__try
	{
		hFile = CreateFileA(pszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			__leave;
		hFileMapping = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		if (!hFileMapping)
			__leave;
		pbFile = (BYTE *)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
		if (!pbFile)
			__leave;
		ptDosHeader = (IMAGE_DOS_HEADER *)pbFile;
		if (ptDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		{
			SetLastError(ERROR_BAD_FORMAT);
			__leave;
		}
		pdwHeader = (DWORD *)(pbFile + ptDosHeader->e_lfanew);
		if (*pdwHeader != IMAGE_VXD_SIGNATURE)
		{
			SetLastError(ERROR_BAD_FORMAT);
			__leave;
		}
		ptNewExe = (IMAGE_VXD_HEADER *)pdwHeader;
		if (!ptNewExe->e32_winreslen)
		{
			*pdwSize = 0;
			SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
			__leave;
		}
		ptWinRes = (SOUNDVXDRESOURCE *)(pbFile + ptNewExe->e32_winresoff);
		dwResourceSize = ptWinRes->dwSize;
		pbInfo = ptWinRes + 1;
		if (!pbBuffer || *pdwSize < dwResourceSize)
		{
			*pdwSize = dwResourceSize;
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			__leave;
		}
		memset(pbBuffer, 0, *pdwSize);
		memcpy(pbBuffer, pbInfo, dwResourceSize);
		*pdwSize = dwResourceSize;
		SetLastError(ERROR_SUCCESS);
		fResult = TRUE;
	}
	__finally
	{
		dwError = GetLastError();
		if (pbFile)
			UnmapViewOfFile(pbFile);
		if (hFileMapping)
			CloseHandle(hFileMapping);
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
		SetLastError(dwError);
	}
	return fResult;
}

DWORD __fastcall D2SoundUtilGetVxdVersionSize(const char *pszFile)
{
	DWORD dwSize;

	dwSize = 0;
	if (!D2SoundUtilReadVxdVersion(pszFile, &dwSize, NULL) && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		SetLastError(ERROR_SUCCESS);
		return dwSize;
	}
	return 0;
}

BOOL __fastcall D2SoundUtilGetVxdVersion(const char *pszFile, DWORD dwSize, void *pbBuffer)
{
	return D2SoundUtilReadVxdVersion(pszFile, &dwSize, pbBuffer);
}

BOOL __fastcall D2SoundUtilGetDriverVersion(const char *pszFile, DWORD *pdwMajor, DWORD *pdwMinor,
	DWORD *pdwBuild)
{
	DWORD dwSize;
	void *pbVersion;
	VS_FIXEDFILEINFO *ptInfo;
	UINT uLength;

	dwSize = D2SoundUtilGetVxdVersionSize(pszFile);
	if (!dwSize)
		return FALSE;
#line 539
	pbVersion = D2ALLOC2(dwSize);
	if (!D2SoundUtilGetVxdVersion(pszFile, dwSize, pbVersion))
	{
#line 544
		D2FREE2(pbVersion);
		return FALSE;
	}
	if (!VerQueryValueA(pbVersion, "\\", (LPVOID *)&ptInfo, &uLength))
	{
#line 552
		D2FREE2(pbVersion);
		return FALSE;
	}
	*pdwMajor = HIWORD(ptInfo->dwFileVersionMS);
	*pdwMinor = ptInfo->dwFileVersionMS & 65535;
	*pdwBuild = ptInfo->dwFileVersionLS & 65535;
	return TRUE;
}
