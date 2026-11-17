#define D2_THIS_FILE "C:\\D2\\Source\\D2Sound\\Src\\D2SoundFast.cpp"
#include <string.h>
#include "D2Sound.h"
#include "D2Macros.h"
#include "Archive.h"
#include "D2soundImports.h"

#define FAST_SAMPLES	8

static D2SAMPLE *sgaptFastSamples[FAST_SAMPLES] = { 0 };
static CRITICAL_SECTION sgtFastLock = { 0 };
static BOOL sgfFastActive = 0;
static D2FASTSOUND *sgptFastSounds = 0;

void __fastcall D2SoundFastStopAll(void);

void __fastcall D2SoundFastInit(void)
{
	int i;

	if (!D2SoundGetDirectSound())
		return;
	if (sgfFastActive)
		return;
	InitializeCriticalSection(&sgtFastLock);
	EnterCriticalSection(&sgtFastLock);
	D2SoundSmpSetMasterVolume(255);
	for (i = 0; i < FAST_SAMPLES; i++)
		sgaptFastSamples[i] = D2SoundSmpCreate(i < 4);
	sgfFastActive = TRUE;
	LeaveCriticalSection(&sgtFastLock);
}

void __fastcall D2SoundFastShutdown(void)
{
	int i;
	D2FASTSOUND *ptNext;

	if (!sgfFastActive)
		return;
	EnterCriticalSection(&sgtFastLock);
	D2SoundFastStopAll();
	for (i = 0; i < FAST_SAMPLES; i++)
	{
		D2SoundSmpFree(sgaptFastSamples[i]);
		sgaptFastSamples[i] = NULL;
	}
	while (sgptFastSounds)
	{
		ptNext = sgptFastSounds->ptNext;
		if (sgptFastSounds->pbFile)
#line 103
			D2FREE2(sgptFastSounds->pbFile);
#line 104
		D2FREE2(sgptFastSounds);
		sgptFastSounds = ptNext;
	}
	sgfFastActive = FALSE;
	DeleteCriticalSection(&sgtFastLock);
}

void __fastcall D2SoundFastLoad(const char *pszFile)
{
	DWORD dwFileSize;
	DWORD dwWaveSize;
	WAVEFORMATEX tFormat;
	void *pbFile;
	D2FASTSOUND *ptSound;

	if (!sgfFastActive)
		return;
#line 124
	pbFile = ArchiveAllocBufferAndRead(NULL, pszFile, &dwFileSize, D2_THIS_FILE, __LINE__);
	if (!pbFile)
		return;
	dwWaveSize = dwFileSize;
	D2SoundUtilParseWave(pbFile, &dwWaveSize, &tFormat);
	if (tFormat.nSamplesPerSec != 22050)
		return;
#line 138
	ptSound = (D2FASTSOUND *)D2ALLOC2(sizeof(D2FASTSOUND));
	ptSound->ptNext = sgptFastSounds;
	sgptFastSounds = ptSound;
	ptSound->pbFile = pbFile;
	ptSound->dwFileSize = dwFileSize;
	ptSound->fStereo = tFormat.nChannels == 2;
	strcpy(ptSound->szName, pszFile);
}

void __fastcall D2SoundFastPlay(const char *pszFile)
{
	D2FASTSOUND *ptSound;
	D2SAMPLE *ptSample;
	int i;

	if (!sgfFastActive)
		return;
	EnterCriticalSection(&sgtFastLock);
	for (ptSound = sgptFastSounds; ptSound; ptSound = ptSound->ptNext)
	{
		if (!strcmp(ptSound->szName, pszFile))
		{
			for (i = 0; i < FAST_SAMPLES; i++)
			{
				if (sgaptFastSamples[i]->dwFlags == (DWORD)ptSound->fStereo
					&& !D2SoundSmpIsPlaying(sgaptFastSamples[i]))
					break;
			}
			if (i < FAST_SAMPLES)
			{
				ptSample = sgaptFastSamples[i];
				D2SoundSmpReset(ptSample);
				D2SoundSmpSetWave(ptSample, ptSound->pbFile, ptSound->dwFileSize);
				D2SoundSmpSetVolume(ptSample, 255);
				D2SoundSmpPlay(ptSample);
			}
			break;
		}
	}
	LeaveCriticalSection(&sgtFastLock);
}

BOOL __fastcall D2SoundFastIsPlaying(void)
{
	int i;

	if (sgfFastActive)
	{
		for (i = 0; i < FAST_SAMPLES; i++)
		{
			if (D2SoundSmpIsPlaying(sgaptFastSamples[i]))
				return TRUE;
		}
	}
	return FALSE;
}

void __fastcall D2SoundFastStopAll(void)
{
	int i;

	if (sgfFastActive)
	{
		for (i = 0; i < FAST_SAMPLES; i++)
			D2SoundSmpStop(sgaptFastSamples[i]);
	}
}

void __fastcall D2SoundFastFadeAll(void)
{
	int i;

	if (sgfFastActive)
	{
		for (i = 0; i < FAST_SAMPLES; i++)
		{
			if (D2SoundSmpIsPlaying(sgaptFastSamples[i]))
				D2SoundSmpFade(sgaptFastSamples[i], 0, 200);
		}
	}
}

BOOL __fastcall D2SoundFastIsActive(void)
{
	return sgfFastActive;
}
