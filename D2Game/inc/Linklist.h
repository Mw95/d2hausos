#ifndef D2RE_LINKLIST_H
#define D2RE_LINKLIST_H

#include <windows.h>

#pragma push_macro("D2_THIS_FILE")
#undef D2_THIS_FILE
#define D2_THIS_FILE	"\\d2\\source\\d2headers\\Linklist.h"
#include "D2Macros.h"

struct LINK
{
 	struct LINK	*ptPrev;
 	struct LINK	*ptNext;
};
typedef struct LINK LINK;

static inline void LinkListInit(LINK *ptList)
{
	ptList->ptPrev = ptList;
	ptList->ptNext = ptList;
}

static inline LINK *LinkListGetHead(LINK *ptList)
{
	if (ptList->ptNext != ptList)
		return ptList->ptNext;
	else
		return NULL;
}

static inline LINK *LinkListGetTail(LINK *ptList)
{
	if (ptList->ptNext != ptList)
		return ptList->ptPrev;
	else
		return NULL;
}

static inline void LinkListInsertAfter(LINK *ptPrev, LINK *ptLink)
{
	D2_OPAQUE(ptPrev);
	D2_OPAQUE(ptLink);
#line 108
	D2ASSERT(ptPrev);
#line 109
	D2ASSERT(ptLink);
	ptLink->ptPrev = ptPrev;
	ptLink->ptNext = ptPrev->ptNext;
	ptLink->ptNext->ptPrev = ptLink;
	ptPrev->ptNext = ptLink;
}

static inline void LinkListInsertHead(LINK *ptList, LINK *ptLink)
{
	D2_OPAQUE(ptList);
	D2_OPAQUE(ptLink);
#line 119
	D2ASSERT(ptList);
#line 120
	D2ASSERT(ptLink);
	ptLink->ptPrev = ptList;
	ptLink->ptNext = ptList->ptNext;
	ptLink->ptNext->ptPrev = ptLink;
	ptList->ptNext = ptLink;
}

static inline void LinkListInsertTail(LINK *ptList, LINK *ptLink)
{
	D2_OPAQUE(ptList);
#line 127
	D2ASSERT(ptList);
	LinkListInsertAfter(ptList->ptPrev, ptLink);
}

static inline LINK *LinkListUnlink(LINK *ptLink)
{
	D2_OPAQUE(ptLink);
#line 135
	D2ASSERT(ptLink);
	ptLink->ptPrev->ptNext = ptLink->ptNext;
	ptLink->ptNext->ptPrev = ptLink->ptPrev;
	return ptLink;
}

#pragma pop_macro("D2_THIS_FILE")

#endif
