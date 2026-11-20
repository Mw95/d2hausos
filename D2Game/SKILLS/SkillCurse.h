#ifndef D2RE_D2GAME_SKILLCURSE_H
#define D2RE_D2GAME_SKILLCURSE_H

#include "../D2game.h"
#include "Skills.h"

static __inline void *SkillsSetCurseState(UNIT *hSource, UNIT *hTarget, int nSkill, int nLevel, DWORD dwStatId,
	int nStatValue, DWORD dwState)
{
	SKILLSTATE tData;

	memset(&tData, 0, sizeof(tData));
	tData.hSource = hSource;
	tData.hTarget = hTarget;
	tData.nSkill = nSkill;
	tData.nLevel = nLevel;
	tData.nDuration = D2CommonSkillsCalcParamThreeByLevel(nSkill, nLevel);
	tData.dwStatId = dwStatId;
	tData.nStatValue = nStatValue;
	tData.dwState = dwState;
	tData.pfnRemove = 0;
	return SkillsSetState(&tData);
}

#endif
