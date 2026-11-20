#ifndef D2RE_D2GAME_STORMNEW_H
#define D2RE_D2GAME_STORMNEW_H

#include "StormImports.h"

inline void __cdecl operator delete(void *pMemory)
{
	if (pMemory)
		StormSMemFree(pMemory, "delete", -1, 0);
}
inline void * __cdecl operator new(unsigned int nSize)
{
	return StormSMemAlloc(nSize, "new", -1, 0);
}

#endif
