#ifndef D2RE_D2GAME_SKILLITEM_H
#define D2RE_D2GAME_SKILLITEM_H

#include "../D2game.h"

typedef BOOL (__fastcall *PFNITEMSPELL)(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget, DWORD dwX,
	DWORD dwY, int nSkill, DWORD dwParam);
typedef WORD (__fastcall *PFNPASSIVEMOD)(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4);
typedef BOOL (__fastcall *PFNITEMSKILL)(GAME *ptGame, UNIT *hUnit, int nSkillId);

BOOL __fastcall SkillsItemOnItemStart(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget, DWORD dwX,
	DWORD dwY, int nSkill, DWORD dwParam);
BOOL __fastcall SkillsItemRestoreLifeMana(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget, DWORD dwX,
	DWORD dwY, int nSkill, DWORD dwParam);
BOOL __fastcall SkillsItemRestoreFull(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget, DWORD dwX,
	DWORD dwY, int nSkill, DWORD dwParam);
BOOL __fastcall SkillsItemOnItem(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget, DWORD dwX,
	DWORD dwY, int nSkill, DWORD dwParam);
BOOL __fastcall SkillsItemInteract(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget, DWORD dwX,
	DWORD dwY, int nSkill, DWORD dwParam);
BOOL __fastcall SkillsItemAddStat(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget, DWORD dwX,
	DWORD dwY, int nSkill, DWORD dwParam);
BOOL __fastcall SkillsItemNoSpell(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget, DWORD dwX,
	DWORD dwY, int nSkill, DWORD dwParam);
BOOL __fastcall SkillsItemPortal(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget, DWORD dwX,
	DWORD dwY, int nSkill, DWORD dwParam);
BOOL __fastcall SkillsItemStamina(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget, DWORD dwX,
	DWORD dwY, int nSkill, DWORD dwParam);
BOOL __fastcall SkillsItemCureSingleState(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget, DWORD dwX,
	DWORD dwY, int nSkill, DWORD dwParam);
BOOL __fastcall SkillsItemCureStates(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget, DWORD dwX,
	DWORD dwY, int nSkill, DWORD dwParam);
BOOL __fastcall SkillsItemHealPotion(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget, DWORD dwX,
	DWORD dwY, int nSkill, DWORD dwParam);
BOOL __fastcall SkillsItemManaPotion(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget, DWORD dwX,
	DWORD dwY, int nSkill, DWORD dwParam);
BOOL __fastcall SkillsItemUseSpell(GAME *ptGame, UNIT *hUnit, UNIT *hItem, UNIT *hTarget, DWORD dwX,
	DWORD dwY);
WORD __fastcall SkillsItemAttackerTakesDamage(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow,
	DAMAGE *ptDamage, DWORD dwUnused4);
WORD __fastcall SkillsItemAttackerTakesLightDmg(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow,
	DAMAGE *ptDamage, DWORD dwUnused4);
WORD __fastcall SkillsItemKnockback(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4);
WORD __fastcall SkillsItemAddClassSkills(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4);
void __fastcall SkillsItemAddSkillLevels(UNIT *hUnit, int nLevels, BOOL fByType,
	BYTE bEType);
WORD __fastcall SkillsItemAddSingleSkill(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4);
WORD __fastcall SkillsItemHowl(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4);
WORD __fastcall SkillsItemStupidity(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4);
WORD __fastcall SkillsItemSlow(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4);
WORD __fastcall SkillsItemFlagOne(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4);
WORD __fastcall SkillsItemReanimate(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4);
WORD __fastcall SkillsItemDamageToMana(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4);
WORD __fastcall SkillsItemAddFireSkills(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4);
WORD __fastcall SkillsItemFreeze(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4);
WORD __fastcall SkillsItemOpenWounds(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4);
WORD __fastcall SkillsItemCrushingBlow(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4);
WORD __fastcall SkillsItemKickDamage(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4);
WORD __fastcall SkillsItemManaAfterKill(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow, DAMAGE *ptDamage,
	DWORD dwUnused4);
WORD __fastcall SkillsItemHealAfterDemonKill(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow,
	DAMAGE *ptDamage, DWORD dwUnused4);
WORD __fastcall SkillsItemApplyPassiveMods(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nEvent,
	DAMAGE *ptDamage, DWORD dwUnused4, BOOL fArg5);
WORD __fastcall SkillsItemRemoveClassSkills(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow,
	DAMAGE *ptDamage, DWORD dwUnused4);
WORD __fastcall SkillsItemRemoveSingleSkill(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow,
	DAMAGE *ptDamage, DWORD dwUnused4);
WORD __fastcall SkillsItemRemoveFireSkills(GAME *ptGame, UNIT *hUnit, UNIT *hTarget, int nRow,
	DAMAGE *ptDamage, DWORD dwUnused4);
void __fastcall SkillsItemRemovePassiveMods(GAME *ptGame, UNIT *hUnit, UNIT *hItem, int nEvent);
BOOL __fastcall SkillsItemUseBook(GAME *ptGame, UNIT *hUnit, int nSkillId);
BOOL __fastcall SkillsItemStart(GAME *ptGame, UNIT *hUnit, int nSkillId, int nLevel);
BOOL __fastcall SkillsItemUse(GAME *ptGame, UNIT *hUnit, int nSkillId, int nLevel);

#endif
