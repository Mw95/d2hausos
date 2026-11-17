#define D2_THIS_FILE "C:\\D2\\Source\\D2Sound\\Src\\D2Sound.cpp"
#include <string.h>
#include "D2Sound.h"
#include "D2Macros.h"
#include "D2soundImports.h"

static BOOL sgfKeepSoundBuffers = TRUE;

static DSCAPS sgtCaps = { 0 };
static LPDIRECTSOUNDBUFFER sglp3DSource = 0;
CRITICAL_SECTION sgtCriticalSection = { 0 };
static LPDIRECTSOUNDBUFFER sglpPrimaryBuffer = 0;
static LPKSPROPERTYSET sglpPropertySet = 0;
static LPDIRECTSOUND3DBUFFER sglp3DBuffer = 0;
static BOOL sgfSoundInitialized = 0;
static LPDIRECTSOUND sglpDirectSound = 0;
static BOOL sgf3D = 0;
static BOOL sgfEaxWanted = 0;
static int sgeEax = 0;
static BOOL sgfEaxOff = 0;
static BOOL sgfCriticalSectionActive = 0;
static BOOL sgfDriverSeen = 0;
static BOOL sgfBadDriver = 0;
static BOOL sgfDriverEmu10k1 = 0;
static BOOL sgfDriverCrystal = 0;

static void __fastcall D2SoundCreateDevice(void);
static void __fastcall D2SoundReleaseEax(void);
BOOL __fastcall D2SoundEaxReset(int eVersion);
const char * __fastcall D2SoundErrorString(HRESULT hResult);

BOOL CALLBACK D2SoundEnumDriversCallback(LPGUID lpGuid, LPCSTR pszDescription, LPCSTR pszModule,
	LPVOID lpContext)
{
	if (!lpGuid || sgfDriverSeen)
		return TRUE;
	FogErrorManagerLog("\tDriver: %s", pszDescription);
	FogErrorManagerLog("\tModule: %s", pszModule);
	if (!strcmp(pszModule, "EMU10K1.VXD"))
		sgfDriverEmu10k1 = TRUE;
	if (!strcmp(pszModule, "es197x.vxd"))
		sgfDriverCrystal = TRUE;
	if (!strcmp(pszDescription, "Crystal SoundFusion(tm)"))
		sgfDriverCrystal = TRUE;
	if (!strcmp(pszModule, "YDSXG.VXD"))
		sgfKeepSoundBuffers = FALSE;
	if (!strcmp(pszModule, "RIPTIDE.VXD"))
		sgfKeepSoundBuffers = FALSE;
	sgfDriverSeen = TRUE;
	return TRUE;
}

void __fastcall D2SoundInit(void)
{
	char szPath[MAX_PATH];
	DWORD dwMajor;
	DWORD dwMinor;
	DWORD dwBuild;

	FogErrorManagerLog("\n----------------------------------------------\n");
	FogErrorManagerLog("Initializing sound engine...");
	sgfDriverSeen = FALSE;
	DirectSoundEnumerateA(D2SoundEnumDriversCallback, NULL);
	if (sgfDriverEmu10k1)
	{
		GetWindowsDirectoryA(szPath, MAX_PATH);
		strcat(szPath, "\\SYSTEM\\EMU10K1.VXD");
		if (D2SoundUtilGetDriverVersion(szPath, &dwMajor, &dwMinor, &dwBuild))
		{
			FogErrorManagerLog("\tVersion of EMU10K1.VXD driver: %i.%i.%i", dwMajor, dwMinor, dwBuild);
			if (dwMajor == 4)
			{
				if (dwMinor == 6)
				{
					if (dwBuild >= 622 && dwBuild <= 710)
						sgfBadDriver = TRUE;
				}
				else if (dwMinor == 11)
				{
					if (dwBuild >= 628 && dwBuild <= 629)
						sgfBadDriver = TRUE;
				}
			}
		}
	}
	if (sgfDriverCrystal)
	{
		GetWindowsDirectoryA(szPath, MAX_PATH);
		strcat(szPath, "\\SYSTEM\\CRLDS3D.VXD");
		if (D2SoundUtilGetDriverVersion(szPath, &dwMajor, &dwMinor, &dwBuild))
		{
			FogErrorManagerLog("\tVersion of CRLDS3D.VXD driver: %i.%i.%i", dwMajor, dwMinor, dwBuild);
			if (dwMajor < 4 || (dwMajor == 4 && (dwMinor < 12 || (dwMinor == 12 && dwBuild < 1035))))
				sgfBadDriver = TRUE;
		}
	}
	if (!sgfSoundInitialized)
	{
		sgf3D = TRUE;
		sgfEaxWanted = TRUE;
		D2SoundCreateDevice();
		if (!sgfSoundInitialized)
		{
			sgf3D = TRUE;
			sgfEaxWanted = FALSE;
			D2SoundCreateDevice();
			if (!sgfSoundInitialized)
			{
				sgf3D = FALSE;
				sgfEaxWanted = FALSE;
				D2SoundCreateDevice();
				if (!sgfSoundInitialized)
					FogErrorManagerLog("\tCouldn't initialize DirectSound");
			}
		}
	}
	FogErrorManagerLog("\n----------------------------------------------\n");
}

static void __fastcall D2SoundCreateDevice(void)
{
	HRESULT hResult;
	DSBUFFERDESC1 tPrimaryDesc;
	WAVEFORMATEX tFormat;
	WAVEFORMATEX tFormat3D;
	DSBUFFERDESC1 tDesc3D;
	DSBCAPS tBufferCaps;
	ULONG ulSupport;

	if (sgf3D && sgfBadDriver)
		return;
#line 76
	D2ASSERT(!sgfCriticalSectionActive);
	InitializeCriticalSection(&sgtCriticalSection);
	sgfCriticalSectionActive = TRUE;
	hResult = DirectSoundCreate(NULL, &sglpDirectSound, NULL);
	if (FAILED(hResult))
	{
		FogErrorManagerLog("DirectSound error: %s returned on DirectSoundCreate",
			D2SoundErrorString(hResult));
		D2SoundShutdown();
		return;
	}
	hResult = sglpDirectSound->SetCooperativeLevel(D2gfxWindowGetHandle(), DSSCL_PRIORITY);
	if (FAILED(hResult))
	{
		FogErrorManagerLog("DirectSound error: %s returned on SetCooperativeLevel",
			D2SoundErrorString(hResult));
		D2SoundShutdown();
		return;
	}
	memset(&tPrimaryDesc, 0, sizeof(tPrimaryDesc));
	tPrimaryDesc.dwSize = sizeof(tPrimaryDesc);
	tPrimaryDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	if (sgf3D)
		tPrimaryDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D;
	tPrimaryDesc.dwBufferBytes = 0;
	tPrimaryDesc.lpwfxFormat = NULL;
	hResult = sglpDirectSound->CreateSoundBuffer((LPCDSBUFFERDESC)&tPrimaryDesc, &sglpPrimaryBuffer, NULL);
	if (FAILED(hResult))
	{
		D2SoundShutdown();
		return;
	}
	sgtCaps.dwSize = sizeof(sgtCaps);
	hResult = sglpDirectSound->GetCaps(&sgtCaps);
	if (FAILED(hResult))
	{
		D2SoundShutdown();
		return;
	}
	if (sgf3D && sgtCaps.dwMaxHw3DAllBuffers < 16)
	{
		D2SoundShutdown();
		return;
	}
	memset(&tFormat, 0, sizeof(tFormat));
	tFormat.wFormatTag = WAVE_FORMAT_PCM;
	tFormat.nChannels = 2;
	tFormat.nSamplesPerSec = 22050;
	tFormat.wBitsPerSample = 16;
	tFormat.nBlockAlign = 4;
	tFormat.nAvgBytesPerSec = 88200;
	hResult = sglpPrimaryBuffer->SetFormat(&tFormat);
	if (FAILED(hResult))
	{
		sglpPrimaryBuffer->Release();
		D2SoundShutdown();
		return;
	}
	sgeEax = SOUNDEAX_NONE;
	if (sgfEaxWanted)
	{
		D2SoundUtilInitWaveFormat(&tFormat3D, 1);
		memset(&tDesc3D, 0, sizeof(tDesc3D));
		tDesc3D.dwSize = sizeof(tDesc3D);
		tDesc3D.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRL3D;
		tDesc3D.dwBufferBytes = 1024;
		tDesc3D.lpwfxFormat = &tFormat3D;
		if (sgeEax == SOUNDEAX_NONE)
		{
			if (FAILED(hResult = sglpDirectSound->CreateSoundBuffer((LPCDSBUFFERDESC)&tDesc3D, &sglp3DSource, NULL))
				|| FAILED(hResult = sglp3DSource->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID *)&sglp3DBuffer))
				|| FAILED(hResult = sglp3DBuffer->QueryInterface(IID_IKsPropertySet, (LPVOID *)&sglpPropertySet))
				|| FAILED(hResult = sglpPropertySet->QuerySupport(gtGuidEax2, 1, &ulSupport))
				|| (ulSupport & (KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET))
					!= (KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET)
				|| !D2SoundEaxReset(SOUNDEAX_2))
				D2SoundReleaseEax();
			else
				sgeEax = SOUNDEAX_2;
		}
		if (sgeEax == SOUNDEAX_NONE)
		{
			if (FAILED(hResult = sglpDirectSound->CreateSoundBuffer((LPCDSBUFFERDESC)&tDesc3D, &sglp3DSource, NULL))
				|| FAILED(hResult = sglp3DSource->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID *)&sglp3DBuffer))
				|| FAILED(hResult = sglp3DBuffer->QueryInterface(IID_IKsPropertySet, (LPVOID *)&sglpPropertySet))
				|| FAILED(hResult = sglpPropertySet->QuerySupport(gtGuidEax1, 0, &ulSupport))
				|| (ulSupport & (KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET))
					!= (KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET)
				|| !D2SoundEaxReset(SOUNDEAX_1))
				D2SoundReleaseEax();
			else
				sgeEax = SOUNDEAX_1;
		}
		if (sgeEax == SOUNDEAX_NONE)
		{
			D2SoundShutdown();
			return;
		}
	}
	memset(&tBufferCaps, 0, sizeof(tBufferCaps));
	tBufferCaps.dwSize = sizeof(tBufferCaps);
	hResult = sglpPrimaryBuffer->GetCaps(&tBufferCaps);
	D2SoundSmpClearBuffer(sglpPrimaryBuffer, 0, tBufferCaps.dwBufferBytes);
	hResult = sglpPrimaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
	D2SoundSmpStartThread();
	D2SoundSmpStormInit();
	D2SoundLoadConfig();
	D2SoundSmpSetMasterVolume(255);
	if (sgf3D)
	{
		if (sgfEaxWanted)
		{
			if (sgeEax == SOUNDEAX_1)
				FogErrorManagerLog("\tDirectSound3D with EAX initialized");
			else
				FogErrorManagerLog("\tDirectSound3D with EAX 2 initialized");
		}
		else
			FogErrorManagerLog("\tDirectSound3D initialized");
		FogErrorManagerLog("\tHardware 3D buffers: %i", sgtCaps.dwMaxHw3DAllBuffers);
	}
	if (sgtCaps.dwFlags & DSCAPS_EMULDRIVER)
		FogErrorManagerLog("\tDirectSound is emulated");
	sgfSoundInitialized = TRUE;
}

void __cdecl operator delete(void *pMemory)
{
	if (pMemory)
		StormSMemFree(pMemory, "delete", -1, 0);
}

void * __cdecl operator new(unsigned int nSize)
{
	return StormSMemAlloc(nSize, "new", -1, 0);
}

void __fastcall D2SoundShutdown(void)
{
	sgfSoundInitialized = FALSE;
	D2SoundSmpSetMasterVolume(0);
	D2SoundSmpStormShutdown();
	D2SoundSmpStopThread();
	D2SoundReleaseEax();
	if (sglpPrimaryBuffer)
	{
		sglpPrimaryBuffer->Release();
		sglpPrimaryBuffer = NULL;
	}
	if (sglpDirectSound)
	{
		sglpDirectSound->Release();
		sglpDirectSound = NULL;
	}
	if (sgfCriticalSectionActive)
	{
		DeleteCriticalSection(&sgtCriticalSection);
		sgfCriticalSectionActive = FALSE;
	}
}

static void __fastcall D2SoundReleaseEax(void)
{
	if (sglpPropertySet)
	{
		sglpPropertySet->Release();
		sglpPropertySet = NULL;
	}
	if (sglp3DBuffer)
	{
		sglp3DBuffer->Release();
		sglp3DBuffer = NULL;
	}
	if (sglp3DSource)
	{
		sglp3DSource->Release();
		sglp3DSource = NULL;
	}
}

IDirectSound * __fastcall D2SoundGetDirectSound(void)
{
	return sglpDirectSound;
}

BOOL __fastcall D2SoundIs3D(void)
{
	return sgf3D;
}

BOOL __fastcall D2SoundIsEaxWanted(void)
{
	return sgfEaxWanted;
}

BOOL __fastcall D2SoundReturnFalse(void)
{
	return FALSE;
}

int __fastcall D2SoundGetEaxVersion(void)
{
	return sgeEax;
}

BOOL __fastcall D2SoundIsEaxOff(void)
{
	return sgfEaxOff;
}

BOOL __fastcall D2SoundKeepsSoundBuffers(void)
{
	return sgfKeepSoundBuffers;
}

BOOL __fastcall D2SoundEaxReset(int eVersion)
{
	if (!sglpPropertySet)
		return FALSE;
	if (eVersion == SOUNDEAX_1)
	{
		float fVolume = 0.0f;
		if (FAILED(sglpPropertySet->Set(gtGuidEax1, 2, NULL, 0, &fVolume, sizeof(fVolume))))
			return FALSE;
	}
	else
	{
		LONG lValue = -10000;
		if (FAILED(sglpPropertySet->Set(gtGuidEax2, 2, NULL, 0, &lValue, sizeof(lValue))))
			return FALSE;
		if (FAILED(sglpPropertySet->Set(gtGuidEax2, 3, NULL, 0, &lValue, sizeof(lValue))))
			return FALSE;
	}
	sgfEaxOff = TRUE;
	return TRUE;
}

void __fastcall D2SoundSetEnvironment(D2SOUNDENVIRONMENT *ptEnvironment)
{
	if (sgfEaxWanted && sglpPropertySet)
	{
		if (sgeEax == SOUNDEAX_1)
		{
			float fGain = (float)D2SoundUtilMillibelsToGain((float)ptEnvironment->lRoomVol);
			sglpPropertySet->Set(gtGuidEax1, 1, NULL, 0, &ptEnvironment->dwEnviron, 4);
			sglpPropertySet->Set(gtGuidEax1, 2, NULL, 0, &fGain, 4);
			sglpPropertySet->Set(gtGuidEax1, 3, NULL, 0, &ptEnvironment->dwDecayTime, 4);
		}
		if (sgeEax == SOUNDEAX_2)
		{
			DWORD dwFlags = 32;
			sglpPropertySet->Set(gtGuidEax2, 11, NULL, 0, &ptEnvironment->dwEnviron, 4);
			sglpPropertySet->Set(gtGuidEax2, 15, NULL, 0, &dwFlags, 4);
			sglpPropertySet->Set(gtGuidEax2, 11, NULL, 0, &ptEnvironment->dwEnviron, 4);
			sglpPropertySet->Set(gtGuidEax2, 12, NULL, 0, &ptEnvironment->dwEnvSize, 4);
			sglpPropertySet->Set(gtGuidEax2, 13, NULL, 0, &ptEnvironment->dwEnvDiff, 4);
			sglpPropertySet->Set(gtGuidEax2, 2, NULL, 0, &ptEnvironment->lRoomVol, 4);
			sglpPropertySet->Set(gtGuidEax2, 3, NULL, 0, &ptEnvironment->dwRoomHF, 4);
			sglpPropertySet->Set(gtGuidEax2, 5, NULL, 0, &ptEnvironment->dwDecayTime, 4);
			sglpPropertySet->Set(gtGuidEax2, 6, NULL, 0, &ptEnvironment->dwDecayHF, 4);
			sglpPropertySet->Set(gtGuidEax2, 7, NULL, 0, &ptEnvironment->dwReflect, 4);
			sglpPropertySet->Set(gtGuidEax2, 8, NULL, 0, &ptEnvironment->dwReflectDelay, 4);
			sglpPropertySet->Set(gtGuidEax2, 9, NULL, 0, &ptEnvironment->dwReverb, 4);
			sglpPropertySet->Set(gtGuidEax2, 10, NULL, 0, &ptEnvironment->dwRevDelay, 4);
			sglpPropertySet->Set(gtGuidEax2, 4, NULL, 0, &ptEnvironment->dwRoomRoll, 4);
			sglpPropertySet->Set(gtGuidEax2, 14, NULL, 0, &ptEnvironment->dwAirAbsorb, 4);
		}
		sgfEaxOff = ptEnvironment->lRoomVol == -10000;
	}
}

const char * __fastcall D2SoundErrorString(HRESULT hResult)
{
	switch (hResult)
	{
	case DS_OK:
		return "DS_OK";
	case DSERR_ALLOCATED:
		return "DSERR_ALLOCATED";
	case DSERR_ALREADYINITIALIZED:
		return "DSERR_ALREADYINITIALIZED";
	case DSERR_BADFORMAT:
		return "DSERR_BADFORMAT";
	case DSERR_BUFFERLOST:
		return "DSERR_BUFFERLOST";
	case DSERR_CONTROLUNAVAIL:
		return "DSERR_CONTROLUNAVAIL";
	case DSERR_GENERIC:
		return "DSERR_GENERIC";
	case DSERR_INVALIDCALL:
		return "DSERR_INVALIDCALL";
	case DSERR_INVALIDPARAM:
		return "DSERR_INVALIDPARAM";
	case DSERR_NOAGGREGATION:
		return "DSERR_NOAGGREGATION";
	case DSERR_NODRIVER:
		return "DSERR_NODRIVER";
	case DSERR_NOINTERFACE:
		return "DSERR_NOINTERFACE";
	case DSERR_OTHERAPPHASPRIO:
		return "DSERR_OTHERAPPHASPRIO";
	case DSERR_OUTOFMEMORY:
		return "DSERR_OUTOFMEMORY";
	case DSERR_PRIOLEVELNEEDED:
		return "DSERR_PRIOLEVELNEEDED";
	case DSERR_UNINITIALIZED:
		return "DSERR_UNINITIALIZED";
	case DSERR_UNSUPPORTED:
		return "DSERR_UNSUPPORTED";
	}
	return NULL;
}

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	return TRUE;
}
