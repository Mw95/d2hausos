#define D2_THIS_FILE "C:\\D2\\Source\\D2Sound\\Src\\D2SoundSmp.cpp"
#include <string.h>
#include <stdlib.h>
#include "D2Sound.h"
#include "D2Macros.h"
#include "D2soundImports.h"

#define MIN_VOLUME	0
#define MAX_VOLUME	255

#define SmpFloatEqual(a, b)	((a) == (b))

static int sgnMasterVolume = 255;
D2SOUNDLISTNODE *gptSamples = 0;
static HANDLE sghThreadEvent = 0;
static HANDLE sghThreadHandle = 0;
static BOOL sgfIconic = 0;

DWORD __fastcall D2SoundSmpGetState(void);
BOOL __fastcall D2SoundSmpMusicIsPlaying(void);
BOOL __fastcall D2SoundSmpMusicNext(void);
BOOL __fastcall D2SoundSmpWrite(D2SAMPLE *ptSample, DWORD dwReadOffset, DWORD dwWriteOffset,
	DWORD dwWriteLength);
BOOL __fastcall D2SoundSmpRestore(D2SAMPLE *ptSample);
static BOOL __fastcall D2SoundSmpCreateBuffer(D2SAMPLE *ptSample, DWORD dwFlags);
int __fastcall D2SoundSmpGetVolume(D2SAMPLE *ptSample);
void __fastcall D2SoundSmpSetPosition(D2SAMPLE *ptSample, float fX, float fY, float fZ);
void __fastcall D2SoundSmpSetReverbMix(D2SAMPLE *ptSample, float fReverbMix);
BOOL __fastcall D2SoundSmpIsStream(D2SAMPLE *ptSample);
BOOL __fastcall D2SoundSmpPlayStream(D2SAMPLE *ptSample, const char *pszFile, DWORD dwPosition,
	BOOL fFlag40000);
void __fastcall D2SoundSmpStopStream(D2SAMPLE *ptSample);
int __fastcall D2SoundSmpGetMasterVolume(void);
static DWORD WINAPI D2SoundSmpThreadProc(LPVOID pParameter);

void __fastcall D2SoundSmpStartThread(void)
{
	DWORD dwThreadId;

#line 188
	D2ASSERT(!sghThreadHandle);
	sghThreadEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
	sghThreadHandle = CreateThread(NULL, 0, D2SoundSmpThreadProc, NULL, 0, &dwThreadId);
}

static DWORD WINAPI D2SoundSmpThreadProc(LPVOID pParameter)
{
	D2SOUNDLISTNODE *ptNode;
	D2SAMPLE *ptSample;
	BOOL fWasIconic;
	int nNow;
	DWORD dwStreamPosition;
	DWORD dwStreamLength;
	DWORD dwPlay;
	DWORD dwWrite;
	DWORD dwBufferPlay;
	DWORD dwBufferWrite;

	while (WaitForSingleObject(sghThreadEvent, 50) == WAIT_TIMEOUT)
	{
		EnterCriticalSection(&sgtCriticalSection);
		fWasIconic = sgfIconic;
		sgfIconic = IsIconic(D2gfxWindowGetHandle());
		if (D2SoundSmpGetState() && !D2SoundSmpMusicIsPlaying())
			D2SoundSmpMusicNext();
		for (ptNode = gptSamples; ptNode; ptNode = ptNode->ptNext)
		{
			ptSample = (D2SAMPLE *)ptNode->pItem;
			D2SoundSmpRestore(ptSample);
			if (sgfIconic != fWasIconic)
				D2SoundSmpSetVolume(ptSample, ptSample->nVolume);
			if (ptSample->fFading)
			{
				nNow = GetTickCount();
				if (nNow >= ptSample->nFadeStart && nNow <= ptSample->nFadeEnd)
				{
					D2SoundSmpSetVolume(ptSample, (ptSample->nFadeTo - ptSample->nFadeFrom)
						* (nNow - ptSample->nFadeStart) / (ptSample->nFadeEnd - ptSample->nFadeStart)
						+ ptSample->nFadeFrom);
				}
				else
				{
					D2SoundSmpSetVolume(ptSample, ptSample->nFadeTo);
					if (!ptSample->nFadeTo)
						D2SoundSmpStop(ptSample);
					ptSample->fFading = FALSE;
				}
			}
			if (ptSample->hStreamFile)
			{
				if (!StormSFileDdaGetPos(ptSample->hStreamFile, &dwStreamPosition,
					&dwStreamLength))
					D2SoundSmpStop(ptSample);
				continue;
			}
			if (!ptSample->fPlaying)
				continue;
			if (FAILED(ptSample->ptSoundBuffer->GetCurrentPosition(&dwPlay, &dwWrite)))
				continue;
			dwBufferPlay = ptSample->dwPlayTotal & (ptSample->dwBufferBytes - 1);
			if (dwPlay >= dwBufferPlay)
				ptSample->dwPlayTotal = ptSample->dwPlayTotal - dwBufferPlay + dwPlay;
			else
				ptSample->dwPlayTotal = ptSample->dwPlayTotal - dwBufferPlay + ptSample->dwBufferBytes
					+ dwPlay;
			if (!ptSample->fDataEnded)
			{
				if (ptSample->dwWriteTotal - ptSample->dwPlayTotal < ptSample->dwHalfBytes
					|| ptSample->dwPlayTotal > ptSample->dwWriteTotal)
				{
					dwBufferWrite = ptSample->dwWriteTotal & (ptSample->dwBufferBytes - 1);
					D2SoundSmpWrite(ptSample, ptSample->dwWriteTotal, dwBufferWrite, ptSample->dwHalfBytes);
					ptSample->fSecondHalf = dwBufferWrite != 0;
					ptSample->dwEndOffset = 0xFFFFFFFF;
				}
				continue;
			}
			if (ptSample->fLoop)
				continue;
			if (dwPlay < ptSample->dwHalfBytes && !ptSample->fSecondHalf)
			{
				D2SoundSmpClearBuffer(ptSample->ptSoundBuffer, ptSample->dwHalfBytes, ptSample->dwHalfBytes);
				ptSample->fSecondHalf = TRUE;
			}
			if (dwPlay >= ptSample->dwHalfBytes && ptSample->fSecondHalf == TRUE)
			{
				D2SoundSmpClearBuffer(ptSample->ptSoundBuffer, 0, ptSample->dwHalfBytes);
				ptSample->fSecondHalf = FALSE;
			}
			if (ptSample->dwPlayTotal > ptSample->dwRawSize)
			{
				ptSample->ptSoundBuffer->Stop();
				ptSample->fPlaying = FALSE;
			}
		}
		LeaveCriticalSection(&sgtCriticalSection);
	}
	return 0;
}

void __fastcall D2SoundSmpStopThread(void)
{
	if (!sghThreadHandle)
		return;
	if (!sghThreadEvent)
		return;
	SetEvent(sghThreadEvent);
	WaitForSingleObject(sghThreadHandle, INFINITE);
	CloseHandle(sghThreadEvent);
	CloseHandle(sghThreadHandle);
	sghThreadEvent = NULL;
	sghThreadHandle = NULL;
}

BOOL __fastcall D2SoundSmpWrite(D2SAMPLE *ptSample, DWORD dwReadOffset, DWORD dwWriteOffset,
	DWORD dwWriteLength)
{
	void *pbLocked = NULL;
	DWORD dwLocked;
	BYTE *pbWrite;
	DWORD dwCount;

#line 221
	D2ASSERT(ptSample && ptSample->ptSoundBuffer && ptSample->pbRawData && dwWriteLength);
	EnterCriticalSection(&sgtCriticalSection);
	dwLocked = dwWriteLength;
	if (FAILED(ptSample->ptSoundBuffer->Lock(dwWriteOffset, dwWriteLength, &pbLocked, &dwLocked, NULL, NULL,
		0)))
	{
		LeaveCriticalSection(&sgtCriticalSection);
		return FALSE;
	}
	pbWrite = (BYTE *)pbLocked;
	for (;;)
	{
		if (dwReadOffset >= ptSample->dwRawSize)
		{
			dwReadOffset = (dwReadOffset - ptSample->dwLoopBlockStart)
				% (ptSample->dwRawSize - ptSample->dwLoopBlockStart) + ptSample->dwLoopBlockStart;
		}
		dwCount = ptSample->dwRawSize - dwReadOffset;
		if ((int)dwWriteLength < (int)dwCount)
			dwCount = dwWriteLength;
		memcpy(pbWrite, ptSample->pbRawData + dwReadOffset, dwCount);
		ptSample->dwWriteTotal += dwCount;
		pbWrite += dwCount;
		dwReadOffset += dwCount;
		dwWriteLength -= dwCount;
		dwWriteOffset += dwCount;
		if (ptSample->fLoop)
		{
			if (dwWriteLength)
				continue;
			break;
		}
		if (!dwWriteLength)
			break;
		if (ptSample->dwEndOffset != 0xFFFFFFFF)
		{
			if (dwWriteOffset >= ptSample->dwEndOffset)
				break;
			dwWriteLength = ptSample->dwEndOffset - dwWriteOffset;
		}
		if (dwWriteLength)
			memset(pbWrite, 0, dwWriteLength);
		break;
	}
	ptSample->ptSoundBuffer->Unlock(pbLocked, dwLocked, NULL, 0);
	if (!ptSample->fLoop && ptSample->dwWriteTotal >= ptSample->dwRawSize)
		ptSample->fDataEnded = TRUE;
	LeaveCriticalSection(&sgtCriticalSection);
	return TRUE;
}

BOOL __fastcall D2SoundSmpRestore(D2SAMPLE *ptSample)
{
	DWORD dwStatus;
	HRESULT hResult;

#line 290
	D2ASSERT(ptSample->ptSoundBuffer);
	EnterCriticalSection(&sgtCriticalSection);
	hResult = ptSample->ptSoundBuffer->GetStatus(&dwStatus);
	if (FAILED(hResult))
	{
		LeaveCriticalSection(&sgtCriticalSection);
		return FALSE;
	}
	if ((BYTE)dwStatus & DSBSTATUS_BUFFERLOST)
	{
		while (TRUE)
		{
			hResult = ptSample->ptSoundBuffer->Restore();
			if (hResult == DSERR_BUFFERLOST)
			{
				Sleep(10);
			}
			else if (SUCCEEDED(hResult))
			{
				break;
			}
		}
		D2SoundSmpStop(ptSample);
	}
	LeaveCriticalSection(&sgtCriticalSection);
	return TRUE;
}

BOOL __fastcall D2SoundSmpClearBuffer(IDirectSoundBuffer *ptSoundBuffer, DWORD dwOffset, DWORD dwBytes)
{
	void *pbLocked = NULL;

#line 331
	D2ASSERT(ptSoundBuffer);
	EnterCriticalSection(&sgtCriticalSection);
	if (FAILED(ptSoundBuffer->Lock(dwOffset, dwBytes, &pbLocked, &dwBytes, NULL, NULL, 0)))
	{
		LeaveCriticalSection(&sgtCriticalSection);
		return FALSE;
	}
	memset(pbLocked, 0, dwBytes);
	ptSoundBuffer->Unlock(pbLocked, dwBytes, NULL, 0);
	LeaveCriticalSection(&sgtCriticalSection);
	return TRUE;
}

D2SAMPLE * __fastcall D2SoundSmpCreate(DWORD dwFlags)
{
	D2SAMPLE *ptSample;

	if ((dwFlags & SAMPLE_3D) && !D2SoundIs3D())
		return NULL;
	if ((dwFlags & SAMPLE_3D) && (dwFlags & SAMPLE_STEREO))
		return NULL;
#line 461
	ptSample = (D2SAMPLE *)D2ALLOC2(sizeof(D2SAMPLE));
	memset(ptSample, 0, sizeof(D2SAMPLE));
	if (!D2SoundSmpCreateBuffer(ptSample, dwFlags))
	{
#line 468
		D2FREE2(ptSample);
		return NULL;
	}
	EnterCriticalSection(&sgtCriticalSection);
	D2SoundUtilListPush(&gptSamples, ptSample);
	LeaveCriticalSection(&sgtCriticalSection);
	return ptSample;
}

static BOOL __fastcall D2SoundSmpCreateBuffer(D2SAMPLE *ptSample, DWORD dwFlags)
{
	WAVEFORMATEX tFormat;
	DSBUFFERDESC1 tDesc;
	DWORD dwBufferBytes;
	ULONG ulSupport;

	D2SoundUtilInitWaveFormat(&tFormat, (dwFlags & SAMPLE_STEREO) ? 2 : 1);
	dwBufferBytes = (dwFlags & SAMPLE_STEREO) ? 262144 : 131072;
	memset(&tDesc, 0, sizeof(tDesc));
	tDesc.dwSize = sizeof(tDesc);
	tDesc.dwFlags = (dwFlags & SAMPLE_3D)
		? DSBCAPS_LOCHARDWARE | DSBCAPS_CTRL3D | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME
		: DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
	tDesc.dwBufferBytes = dwBufferBytes;
	tDesc.lpwfxFormat = &tFormat;
	if (FAILED(D2SoundGetDirectSound()->CreateSoundBuffer((LPCDSBUFFERDESC)&tDesc, &ptSample->ptSoundBuffer,
		NULL)))
		return FALSE;
	if (dwFlags & SAMPLE_3D)
	{
		if (FAILED(ptSample->ptSoundBuffer->QueryInterface(IID_IDirectSound3DBuffer,
			(LPVOID *)&ptSample->pt3DBuffer)))
		{
			ptSample->ptSoundBuffer->Release();
			return FALSE;
		}
	}
	if ((dwFlags & SAMPLE_3D) && D2SoundIsEaxWanted())
	{
		if (FAILED(ptSample->pt3DBuffer->QueryInterface(IID_IKsPropertySet,
			(LPVOID *)&ptSample->ptPropertySet)))
		{
			ptSample->ptSoundBuffer->Release();
			return FALSE;
		}
		ulSupport = 0;
		if (D2SoundGetEaxVersion() == SOUNDEAX_1)
		{
			if (FAILED(ptSample->ptPropertySet->QuerySupport(gtGuidEax1Buffer, 1, &ulSupport))
				|| (ulSupport & (KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET))
					!= (KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET))
			{
				ptSample->ptPropertySet->Release();
				ptSample->ptSoundBuffer->Release();
				return FALSE;
			}
		}
		else
		{
			if (FAILED(ptSample->ptPropertySet->QuerySupport(gtGuidEax2Buffer, 1, &ulSupport))
				|| (ulSupport & (KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET))
					!= (KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET))
			{
				ptSample->ptPropertySet->Release();
				ptSample->ptSoundBuffer->Release();
				return FALSE;
			}
		}
	}
	ptSample->dwBufferBytes = dwBufferBytes;
	ptSample->dwFlags = dwFlags;
	ptSample->dwHalfBytes = dwBufferBytes >> 1;
	ptSample->dwEndOffset = 0xFFFFFFFF;
	return TRUE;
}

static inline void D2SoundSmpReleaseBuffer(D2SAMPLE *ptSample)
{
	D2SoundSmpStop(ptSample);
	if (ptSample->ptPropertySet)
	{
		ptSample->ptPropertySet->Release();
		ptSample->ptPropertySet = NULL;
	}
	if (ptSample->pt3DBuffer)
	{
		ptSample->pt3DBuffer->Release();
		ptSample->pt3DBuffer = NULL;
	}
	if (ptSample->ptSoundBuffer)
	{
		ptSample->ptSoundBuffer->Release();
		ptSample->ptSoundBuffer = NULL;
	}
	memset(ptSample, 0, sizeof(D2SAMPLE));
}

void __fastcall D2SoundSmpFree(D2SAMPLE *ptSample)
{
#line 517
	D2ASSERT(ptSample);
	EnterCriticalSection(&sgtCriticalSection);
	D2SoundSmpReleaseBuffer(ptSample);
	D2SoundUtilListRemove(&gptSamples, ptSample);
	LeaveCriticalSection(&sgtCriticalSection);
#line 529
	D2FREE2(ptSample);
}

void __fastcall D2SoundSmpReset(D2SAMPLE *ptSample)
{
	DWORD dwFlags;

#line 538
	D2ASSERT(ptSample && ptSample->ptSoundBuffer);
#line 539
	D2ASSERT(!ptSample->hStreamFile);
	EnterCriticalSection(&sgtCriticalSection);
	if (D2SoundKeepsSoundBuffers())
	{
		ptSample->pbRawData = NULL;
		ptSample->dwRawSize = 0;
		ptSample->dwPlayTotal = 0;
		ptSample->dwWriteTotal = 0;
		ptSample->fDataEnded = FALSE;
		ptSample->fLoop = FALSE;
		ptSample->dwLoopBlockStart = 0;
	}
	else
	{
		dwFlags = ptSample->dwFlags;
		D2SoundSmpReleaseBuffer(ptSample);
		D2SoundSmpCreateBuffer(ptSample, dwFlags);
	}
	ptSample->ptSoundBuffer->SetCurrentPosition(0);
	if (ptSample->pt3DBuffer)
	{
		ptSample->pt3DBuffer->SetMaxDistance(100.0f, DS3D_IMMEDIATE);
		ptSample->pt3DBuffer->SetMinDistance(2.0f, DS3D_IMMEDIATE);
	}
	if (ptSample->dwFlags & SAMPLE_3D)
		D2SoundSmpSetPosition(ptSample, 0.0f, 0.0f, 0.0f);
	else
		ptSample->ptSoundBuffer->SetPan(0);
	D2SoundSmpSetReverbMix(ptSample, 0.0f);
	D2SoundSmpSetVolume(ptSample, 255);
	LeaveCriticalSection(&sgtCriticalSection);
}

BOOL __fastcall D2SoundSmpSetWave(D2SAMPLE *ptSample, void *pbWaveData, DWORD dwWaveSize)
{
	WAVEFORMATEX tFormat;
	DWORD dwRawSize;
	BYTE *pbRawData;

#line 587
	D2ASSERT(ptSample && ptSample->ptSoundBuffer && pbWaveData && dwWaveSize);
	EnterCriticalSection(&sgtCriticalSection);
	dwRawSize = dwWaveSize;
	pbRawData = (BYTE *)D2SoundUtilParseWave(pbWaveData, &dwRawSize, &tFormat);
#line 595
	D2ASSERT(ptSample->dwLoopBlockStart < dwRawSize);
	ptSample->dwRawSize = dwRawSize;
	ptSample->pbRawData = pbRawData;
	D2SoundSmpWrite(ptSample, 0, 0, ptSample->dwBufferBytes);
	ptSample->fSecondHalf = TRUE;
	if (!ptSample->fLoop && ptSample->dwRawSize < ptSample->dwBufferBytes)
		ptSample->dwEndOffset = ptSample->dwRawSize;
	else
		ptSample->dwEndOffset = 0xFFFFFFFF;
	LeaveCriticalSection(&sgtCriticalSection);
	return TRUE;
}

BOOL __fastcall D2SoundSmpPlay(D2SAMPLE *ptSample)
{
#line 621
	D2ASSERT(ptSample);
	EnterCriticalSection(&sgtCriticalSection);
	if (FAILED(ptSample->ptSoundBuffer->Play(0, 0, DSBPLAY_LOOPING)))
	{
		LeaveCriticalSection(&sgtCriticalSection);
		return FALSE;
	}
	ptSample->fPlaying = TRUE;
	LeaveCriticalSection(&sgtCriticalSection);
	return TRUE;
}

void __fastcall D2SoundSmpStop(D2SAMPLE *ptSample)
{
#line 646
	D2ASSERT(ptSample && ptSample->ptSoundBuffer);
	EnterCriticalSection(&sgtCriticalSection);
	if (ptSample->hStreamFile)
		D2SoundSmpStopStream(ptSample);
	ptSample->ptSoundBuffer->Stop();
	ptSample->fPlaying = FALSE;
	LeaveCriticalSection(&sgtCriticalSection);
}

BOOL __fastcall D2SoundSmpIsPlaying(D2SAMPLE *ptSample)
{
#line 665
	D2ASSERT(ptSample && ptSample->ptSoundBuffer);
	return ptSample->fPlaying;
}

BOOL __fastcall D2SoundSmpIsBufferPlaying(D2SAMPLE *ptSample)
{
	DWORD dwStatus;

#line 676
	D2ASSERT(ptSample && ptSample->ptSoundBuffer);
	EnterCriticalSection(&sgtCriticalSection);
	if (FAILED(ptSample->ptSoundBuffer->GetStatus(&dwStatus)))
	{
		LeaveCriticalSection(&sgtCriticalSection);
		return FALSE;
	}
	LeaveCriticalSection(&sgtCriticalSection);
	if (dwStatus & DSBSTATUS_PLAYING)
		return TRUE;
	return FALSE;
}

HRESULT __fastcall D2SoundSmpSetVolume(D2SAMPLE *ptSample, int nVolume)
{
	int nScaled;

#line 703
	D2ASSERT(ptSample && ptSample->ptSoundBuffer);
	ptSample->nVolume = nVolume;
	nScaled = sgnMasterVolume * nVolume / 255;
	if (!(ptSample->dwFlags & SAMPLE_REVERB) || D2SoundGetEaxVersion() != SOUNDEAX_2)
		nScaled = (int)(__int64)((1.0 - ptSample->fReverbMix) * nScaled);
	if (sgfIconic)
		nScaled = 0;
	nScaled = (LONG)(__int64)D2SoundUtilRatioToDecibels(0.0f, 255.0f, (float)nScaled);
	return ptSample->ptSoundBuffer->SetVolume(nScaled);
}

int __fastcall D2SoundSmpGetVolume(D2SAMPLE *ptSample)
{
#line 728
	D2ASSERT(ptSample);
	return ptSample->nVolume;
}

HRESULT __fastcall D2SoundSmpSetPan(D2SAMPLE *ptSample, int nPan)
{
	LONG lPan;

#line 739
	D2ASSERT(ptSample);
	if (nPan > 128)
		lPan = (LONG)(__int64)-D2SoundUtilRatioToDecibels(0.0f, 127.0f, (float)(255 - nPan));
	else if (nPan < 128)
		lPan = (LONG)(__int64)D2SoundUtilRatioToDecibels(0.0f, 127.0f, (float)nPan);
	else
		lPan = 0;
	return ptSample->ptSoundBuffer->SetPan(lPan);
}

void __fastcall D2SoundSmpSetPosition(D2SAMPLE *ptSample, float fX, float fY, float fZ)
{
#line 758
	D2ASSERT(ptSample && ptSample->ptSoundBuffer);
	if (ptSample->dwFlags & SAMPLE_3D)
		ptSample->pt3DBuffer->SetPosition(fX, fY, fZ, DS3D_IMMEDIATE);
}

DWORD __fastcall D2SoundSmpGetStreamPosition(D2SAMPLE *ptSample)
{
	DWORD dwStreamPosition;
	DWORD dwStreamLength;

#line 772
	D2ASSERT(ptSample);
	EnterCriticalSection(&sgtCriticalSection);
	if (ptSample->hStreamFile)
	{
		if (StormSFileDdaGetPos(ptSample->hStreamFile, &dwStreamPosition, &dwStreamLength))
		{
			LeaveCriticalSection(&sgtCriticalSection); return dwStreamPosition >> 2;
		}
		LeaveCriticalSection(&sgtCriticalSection); return 0;
	}
	LeaveCriticalSection(&sgtCriticalSection);
	return 0;
}

void __fastcall D2SoundSmpSetLoop(D2SAMPLE *ptSample, BOOL fLoop)
{
#line 803
	D2ASSERT(ptSample);
	ptSample->fLoop = fLoop;
}

void __fastcall D2SoundSmpSetLoopBlockStart(D2SAMPLE *ptSample, DWORD dwLoopBlockStart)
{
#line 814
	D2ASSERT(ptSample);
	ptSample->dwLoopBlockStart = dwLoopBlockStart;
}

void __fastcall D2SoundSmpSetReverbMix(D2SAMPLE *ptSample, float fReverbMix)
{
#line 825
	D2ASSERT(ptSample);
	ptSample->fReverbMix = fReverbMix;
	if (!ptSample->ptPropertySet)
		return;
	if (D2SoundGetEaxVersion() != SOUNDEAX_2)
		return;
	if (!(ptSample->dwFlags & SAMPLE_REVERB) && !SmpFloatEqual(fReverbMix, 0.0f))
		return;
	{
		LONG lValue = (LONG)(__int64)D2SoundUtilRatioToDecibels(0.0f, 1.0f, 1.0f - fReverbMix);
		ptSample->ptPropertySet->Set(gtGuidEax2Buffer, 7, NULL, 0, &lValue, sizeof(lValue));
	}
}

void __fastcall D2SoundSmpSetReverbLevel(D2SAMPLE *ptSample, float fLevel)
{
	LONG lValue;

#line 845
	D2ASSERT(ptSample);
	if (D2SoundIsEaxOff())
		fLevel = 0.0f;
	if (!ptSample->ptPropertySet)
		return;
	if (D2SoundGetEaxVersion() == SOUNDEAX_1)
	{
		if (!SmpFloatEqual(fLevel, 0.0f))
			fLevel = -1.0f;
		ptSample->ptPropertySet->Set(gtGuidEax1Buffer, 1, NULL, 0, &fLevel, sizeof(fLevel));
		return;
	}
	lValue = (LONG)(__int64)D2SoundUtilRatioToDecibels(0.0f, 1.0f, fLevel);
	ptSample->ptPropertySet->Set(gtGuidEax2Buffer, 5, NULL, 0, &lValue, sizeof(lValue));
	ptSample->ptPropertySet->Set(gtGuidEax2Buffer, 4, NULL, 0, &lValue, sizeof(lValue));
}

BOOL __fastcall D2SoundSmpIsStream(D2SAMPLE *ptSample)
{
#line 878
	D2ASSERT(ptSample);
	return ptSample->hStreamFile != NULL;
}

void __fastcall D2SoundSmpFadeOutMaster(int nDuration)
{
	DWORD dwVolume;
	DWORD dwStep;

	if (!D2SoundGetDirectSound())
		return;
#line 891
	D2ASSERT(nDuration > 0);
	dwVolume = D2SoundSmpGetMasterVolume();
	dwStep = dwVolume / ((DWORD)nDuration / 20);
	for (;;)
	{
		if (!dwVolume)
			break;
		Sleep(20);
		if (dwVolume > dwStep)
			dwVolume -= dwStep;
		else
			dwVolume = 0;
		D2SoundSmpSetMasterVolume(dwVolume);
	}
}

void __fastcall D2SoundSmpFade(D2SAMPLE *ptSample, int nVolume, int nDuration)
{
#line 914
	D2ASSERT(ptSample);
	if (!D2SoundGetDirectSound())
		return;
	EnterCriticalSection(&sgtCriticalSection);
	ptSample->fFading = TRUE;
	ptSample->nFadeFrom = ptSample->nVolume;
	ptSample->nFadeTo = nVolume;
	ptSample->nFadeStart = GetTickCount();
	ptSample->nFadeEnd = ptSample->nFadeStart + nDuration;
	LeaveCriticalSection(&sgtCriticalSection);
}

void __fastcall D2SoundSmpSetMasterVolume(int nVolume)
{
	D2SOUNDLISTNODE *ptNode;

	if (!D2SoundGetDirectSound())
		return;
	if (nVolume == sgnMasterVolume)
		return;
#line 941
	D2ASSERT(nVolume >= MIN_VOLUME && nVolume <= MAX_VOLUME);
	EnterCriticalSection(&sgtCriticalSection);
	sgnMasterVolume = nVolume;
	for (ptNode = gptSamples; ptNode != NULL; ptNode = ptNode->ptNext)
	{
		D2SAMPLE *ptSample = (D2SAMPLE *)ptNode->pItem;
		D2SoundSmpSetVolume(ptSample, D2SoundSmpGetVolume(ptSample));
	}
	LeaveCriticalSection(&sgtCriticalSection);
}

int __fastcall D2SoundSmpGetMasterVolume(void)
{
	return D2SoundGetDirectSound() ? sgnMasterVolume : 0;
}

BOOL __fastcall D2SoundSmpPlayStream(D2SAMPLE *ptSample, const char *pszFile, DWORD dwPosition,
	BOOL fFlag40000)
{
	HANDLE hFile;

#line 975
	D2ASSERT(ptSample);
	if (!FogSafesockOpenStormFile(pszFile, &hFile))
		return FALSE;
	EnterCriticalSection(&sgtCriticalSection);
	if (!StormSFileDdaBeginAt(hFile, ptSample->dwBufferBytes, fFlag40000 ? 262144 : 0,
		dwPosition * 4, 2147483647, 2147483647, ptSample->ptSoundBuffer))
	{
		FogSafesockCloseStormFile(hFile);
		LeaveCriticalSection(&sgtCriticalSection);
		return FALSE;
	}
	ptSample->hStreamFile = hFile;
	ptSample->fPlaying = TRUE;
	ptSample->dwEndOffset = 0xFFFFFFFF;
	LeaveCriticalSection(&sgtCriticalSection);
	return TRUE;
}

void __fastcall D2SoundSmpStopStream(D2SAMPLE *ptSample)
{
#line 1012
	D2ASSERT(ptSample && ptSample->ptSoundBuffer);
	EnterCriticalSection(&sgtCriticalSection);
	if (!ptSample->hStreamFile)
	{
		LeaveCriticalSection(&sgtCriticalSection);
		return;
	}
	StormSFileDdaEnd(ptSample->hStreamFile);
	ptSample->ptSoundBuffer->Stop();
	FogSafesockCloseStormFile(ptSample->hStreamFile);
	ptSample->hStreamFile = NULL;
	ptSample->fPlaying = FALSE;
	LeaveCriticalSection(&sgtCriticalSection);
}

void __fastcall D2SoundSmpNothingFirst(void)
{
}

void __fastcall D2SoundSmpNothingSecond(void)
{
}
