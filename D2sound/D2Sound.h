#ifndef D2RE_D2SOUND_D2SOUND_H
#define D2RE_D2SOUND_D2SOUND_H

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

#define SAMPLE_STEREO	1
#define SAMPLE_3D		2
#define SAMPLE_REVERB	4

struct D2SAMPLE
{
 	IDirectSoundBuffer	*ptSoundBuffer;
 	DWORD		dwBufferBytes;
 	DWORD		dwHalfBytes;
 	IDirectSound3DBuffer	*pt3DBuffer;
 	IKsPropertySet	*ptPropertySet;
 	BYTE		*pbRawData;
 	DWORD		dwRawSize;
 	BOOL		fPlaying;
 	DWORD		dwPlayTotal;
 	DWORD		dwWriteTotal;
 	BOOL		fSecondHalf;
 	BOOL		fDataEnded;
 	DWORD		dwEndOffset;
 	DWORD		dwFlags;
 	int			nVolume;
 	float		fReverbMix;
 	BOOL		fLoop;
 	DWORD		dwLoopBlockStart;
 	HANDLE		hStreamFile;
 	BOOL		fFading;
 	int			nFadeFrom;
 	int			nFadeTo;
 	int			nFadeStart;
 	int			nFadeEnd;
};

struct D2FASTSOUND
{
 	char		szName[80];
 	void		*pbFile;
 	DWORD		dwFileSize;
 	BOOL		fStereo;
 	D2FASTSOUND	*ptNext;
};

struct D2SOUNDLISTNODE
{
 	void		*pItem;
 	D2SOUNDLISTNODE	*ptNext;
};

struct D2SOUNDENVIRONMENT
{
 	DWORD		dwEnviron;
 	DWORD		dwEnvSize;
 	DWORD		dwEnvDiff;
 	LONG		lRoomVol;
 	DWORD		dwRoomHF;
 	DWORD		dwDecayTime;
 	DWORD		dwDecayHF;
 	DWORD		dwReflect;
 	DWORD		dwReflectDelay;
 	DWORD		dwReverb;
 	DWORD		dwRevDelay;
 	DWORD		dwRoomRoll;
 	DWORD		dwAirAbsorb;
};

enum ESOUNDEAX
{
	SOUNDEAX_NONE = 0,
	SOUNDEAX_1 = 1,
	SOUNDEAX_2 = 2,
};

EXTERN_C const GUID gtGuidEax1;
EXTERN_C const GUID gtGuidEax1Buffer;
EXTERN_C const GUID gtGuidEax2;
EXTERN_C const GUID gtGuidEax2Buffer;

extern CRITICAL_SECTION sgtCriticalSection;
void __fastcall D2SoundShutdown(void);
IDirectSound * __fastcall D2SoundGetDirectSound(void);
BOOL __fastcall D2SoundIs3D(void);
BOOL __fastcall D2SoundIsEaxWanted(void);
int __fastcall D2SoundGetEaxVersion(void);
BOOL __fastcall D2SoundIsEaxOff(void);
BOOL __fastcall D2SoundKeepsSoundBuffers(void);

void __fastcall D2SoundFastInit(void);
void __fastcall D2SoundFastShutdown(void);
void __fastcall D2SoundFastLoad(const char *pszFile);
void __fastcall D2SoundFastPlay(const char *pszFile);
BOOL __fastcall D2SoundFastIsPlaying(void);
void __fastcall D2SoundFastStopAll(void);
void __fastcall D2SoundFastFadeAll(void);
BOOL __fastcall D2SoundFastIsActive(void);
void __fastcall D2SoundLoadConfig(void);

void __fastcall D2SoundSmpStartThread(void);
void __fastcall D2SoundSmpStopThread(void);
BOOL __fastcall D2SoundSmpClearBuffer(IDirectSoundBuffer *ptSoundBuffer, DWORD dwOffset,
	DWORD dwBytes);
D2SAMPLE * __fastcall D2SoundSmpCreate(DWORD dwFlags);
void __fastcall D2SoundSmpFree(D2SAMPLE *ptSample);
void __fastcall D2SoundSmpReset(D2SAMPLE *ptSample);
BOOL __fastcall D2SoundSmpSetWave(D2SAMPLE *ptSample, void *pbWaveData,
	DWORD dwWaveSize);
BOOL __fastcall D2SoundSmpPlay(D2SAMPLE *ptSample);
void __fastcall D2SoundSmpStop(D2SAMPLE *ptSample);
BOOL __fastcall D2SoundSmpIsPlaying(D2SAMPLE *ptSample);
HRESULT __fastcall D2SoundSmpSetVolume(D2SAMPLE *ptSample, int nVolume);
void __fastcall D2SoundSmpFade(D2SAMPLE *ptSample, int nVolume, int nDuration);
void __fastcall D2SoundSmpSetMasterVolume(int nVolume);
void __fastcall D2SoundSmpStormInit(void);
DWORD __fastcall D2SoundSmpStormShutdown(void);

void __fastcall D2SoundUtilCalcVolumePan(int *pnVolume, int *pnPan, float fX, float fY, float fZ);
BOOL __fastcall D2SoundUtilListContains(D2SOUNDLISTNODE *ptHead, void *pItem);
int __fastcall D2SoundUtilListCount(D2SOUNDLISTNODE *ptHead);
void __fastcall D2SoundUtilListFree(D2SOUNDLISTNODE **pptHead);
D2SOUNDLISTNODE * __fastcall D2SoundUtilListAppend(D2SOUNDLISTNODE **pptHead, void *pItem);
BOOL __fastcall D2SoundUtilReadVxdVersion(const char *pszFile, DWORD *pdwSize, void *pbBuffer);
DWORD __fastcall D2SoundUtilGetVxdVersionSize(const char *pszFile);
BOOL __fastcall D2SoundUtilGetVxdVersion(const char *pszFile, DWORD dwSize, void *pbBuffer);
float __stdcall D2SoundUtilRatioToDecibels(float fMin, float fMax, float fValue);
float __stdcall D2SoundUtilMillibelsToGain(float fMillibels);
void __fastcall D2SoundUtilInitWaveFormat(WAVEFORMATEX *ptFormat, int nChannels);
void * __fastcall D2SoundUtilParseWave(void *pbWaveData, DWORD *pdwWaveSize, WAVEFORMATEX *ptFormat);
D2SOUNDLISTNODE * __fastcall D2SoundUtilListPush(D2SOUNDLISTNODE **pptHead, void *pItem);
void __fastcall D2SoundUtilListRemove(D2SOUNDLISTNODE **pptHead, void *pItem);
BOOL __fastcall D2SoundUtilGetDriverVersion(const char *pszFile, DWORD *pdwMajor, DWORD *pdwMinor,
	DWORD *pdwBuild);

#endif
