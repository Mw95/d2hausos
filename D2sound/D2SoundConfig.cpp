#include "D2Sound.h"
#include "D2soundImports.h"

struct D2SOUNDOPTIONS
{
 	DWORD	dwSoundMixer;
 	DWORD	dwMasterVolume;
 	DWORD	dwMusicVolume;
 	DWORD	dwPositionalBias;
 	DWORD	dwNpcSpeech;
 	DWORD	dwOptionsMusic;
};

static D2SOUNDOPTIONS sgtOptions = { 0 };

void __fastcall D2SoundLoadConfig(void)
{
	DWORD dwValue;

	sgtOptions.dwSoundMixer = 0;
	sgtOptions.dwMusicVolume = 50;
	sgtOptions.dwPositionalBias = 50;
	sgtOptions.dwMasterVolume = 100;
	sgtOptions.dwNpcSpeech = 2;
	sgtOptions.dwOptionsMusic = 1;
	if (StormSRegLoadValue("Diablo II", "Sound Mixer", 0,
		&dwValue) && dwValue < 3)
		sgtOptions.dwSoundMixer = dwValue;
	if (StormSRegLoadValue("Diablo II", "Master Volume", 0,
		&dwValue) && dwValue <= 100)
		sgtOptions.dwMasterVolume = dwValue;
	if (StormSRegLoadValue("Diablo II", "Music Volume", 0,
		&dwValue) && dwValue <= 100)
		sgtOptions.dwMusicVolume = dwValue;
	if (StormSRegLoadValue("Diablo II", "Positional Bias", 0,
		&dwValue) && dwValue <= 100)
		sgtOptions.dwPositionalBias = dwValue;
	if (StormSRegLoadValue("Diablo II", "NPC Speech", 0,
		&dwValue) && dwValue < 3)
		sgtOptions.dwNpcSpeech = dwValue;
	if (StormSRegLoadValue("Diablo II", "Options Music", 0,
		&dwValue) && dwValue <= 1)
		sgtOptions.dwOptionsMusic = dwValue;
}

DWORD __fastcall D2SoundGetSoundMixer(void)
{
	return sgtOptions.dwSoundMixer;
}

DWORD __fastcall D2SoundSetSoundMixer(DWORD dwValue)
{
	StormSRegSaveValue("Diablo II", "Sound Mixer", 0, dwValue);
	sgtOptions.dwSoundMixer = dwValue;
	return dwValue;
}

DWORD __fastcall D2SoundGetMasterVolume(void)
{
	return sgtOptions.dwMasterVolume;
}

DWORD __fastcall D2SoundSetMasterVolume(DWORD dwValue)
{
	StormSRegSaveValue("Diablo II", "Master Volume", 0, dwValue);
	sgtOptions.dwMasterVolume = dwValue;
	return dwValue;
}

DWORD __fastcall D2SoundGetMusicVolume(void)
{
	return sgtOptions.dwMusicVolume;
}

DWORD __fastcall D2SoundSetMusicVolume(DWORD dwValue)
{
	StormSRegSaveValue("Diablo II", "Music Volume", 0, dwValue);
	sgtOptions.dwMusicVolume = dwValue;
	return dwValue;
}

DWORD __fastcall D2SoundGetPositionalBias(void)
{
	return sgtOptions.dwPositionalBias;
}

DWORD __fastcall D2SoundSetPositionalBias(DWORD dwValue)
{
	StormSRegSaveValue("Diablo II", "Positional Bias", 0, dwValue);
	sgtOptions.dwPositionalBias = dwValue;
	return dwValue;
}

DWORD __fastcall D2SoundGetNpcSpeech(void)
{
	return sgtOptions.dwNpcSpeech;
}

DWORD __fastcall D2SoundSetNpcSpeech(DWORD dwValue)
{
	StormSRegSaveValue("Diablo II", "NPC Speech", 0, dwValue);
	sgtOptions.dwNpcSpeech = dwValue;
	return dwValue;
}

DWORD __fastcall D2SoundGetOptionsMusic(void)
{
	return sgtOptions.dwOptionsMusic;
}

DWORD __fastcall D2SoundSetOptionsMusic(DWORD dwValue)
{
	StormSRegSaveValue("Diablo II", "Options Music", 0, dwValue);
	sgtOptions.dwOptionsMusic = dwValue;
	return dwValue;
}
