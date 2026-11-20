#ifndef D2RE_D2GAME_TARGETS_H
#define D2RE_D2GAME_TARGETS_H

#include "../D2game.h"

struct TARGET
{
 	UNIT		*hUnit;
 	DWORD		field_04;
 	struct TARGET	*ptNext;
 	struct TARGET	*ptPrev;
};
typedef struct TARGET TARGET;

void __fastcall TargetsNewList(GAME *ptGame, UNIT *hUnit, DWORD dwField04);
void __fastcall TargetsAddToList(GAME *ptGame, UNIT *hUnit, DWORD dwField04, int nListNo);
void __fastcall TargetsAddToFurtherList(GAME *ptGame, UNIT *hUnit, DWORD dwField04,
	int nListNo);
void __fastcall TargetsFreeList(GAME *ptGame, UNIT *hUnit);
void __fastcall TargetsRemove(GAME *ptGame, UNIT *hUnit);

#endif
