#ifndef D2RE_D2MACROS_H
#define D2RE_D2MACROS_H

#include "Fog.imports.h"
#include "GameData.h"
#define FogAsyncDataLoadRaw FogOrdinal10066
#define FogErrorManagerAllocRaw FogOrdinal10033
#define FogErrorManagerAssertRaw FogOrdinal10021
#define FogErrorManagerFreeRaw FogOrdinal10034
#define FogErrorManagerHaltRaw FogOrdinal10022
#define FogErrorManagerPoolAllocRaw FogOrdinal10140
#define FogErrorManagerPoolFreeRaw FogOrdinal10141
#define FogErrorManagerPoolReallocRaw FogOrdinal10142
#define FogErrorManagerWarnRaw FogOrdinal10023

#include <stdlib.h>
#include <string.h>

#ifndef D2_THIS_FILE
#error "D2_THIS_FILE must be defined per translation unit by the build (CPP-CONVENTIONS 2.3)"
#endif

#define FogAssert(pszExpr, pszFile, nLine)	FogErrorManagerAssertRaw((pszExpr), (pszFile), (nLine))

#define D2ASSERT(expr) \
	if (!(expr)) { FogAssert(#expr, D2_THIS_FILE, __LINE__); exit(-1); }

#define D2ASSERTMSG(expr, pszMessage) \
	if (!(expr)) { FogAssert(pszMessage, D2_THIS_FILE, __LINE__); exit(-1); }

#define D2WARN(expr) \
	if (!(expr)) FogErrorManagerWarnRaw(#expr, D2_THIS_FILE, __LINE__)
#define D2WARNMSG(pszMessage)	FogErrorManagerWarnRaw((pszMessage), D2_THIS_FILE, __LINE__)
#define D2FATAL(pszMessage) \
	{ FogErrorManagerHaltRaw((pszMessage), D2_THIS_FILE, __LINE__); exit(-1); }

#define D2ALLOC2(nSize)	((void *)FogErrorManagerAllocRaw((DWORD)(nSize), (DWORD)D2_THIS_FILE, __LINE__, 0))
#define D2FREE2(p)		FogErrorManagerFreeRaw((DWORD)(p), (DWORD)D2_THIS_FILE, __LINE__, 0)

#define D2ALLOCPOOL(pPool, nSize)	((void *)FogErrorManagerPoolAllocRaw((DWORD)(pPool), (DWORD)(nSize), (DWORD)D2_THIS_FILE, \
	__LINE__, 0))
#define D2FREEPOOL(pPool, p)	FogErrorManagerPoolFreeRaw((DWORD)(pPool), (DWORD)(p), (DWORD)D2_THIS_FILE, __LINE__, 0)
#define D2REALLOCPOOL(pPool, p, nSize)	((void *)FogErrorManagerPoolReallocRaw((DWORD)(pPool), (DWORD)(p), (DWORD)(nSize), \
	(DWORD)D2_THIS_FILE, __LINE__, 0))
#define D2ASYNCLOAD(pPool, pszFile, fAsync, dwOffset, nSize, pbBuffer, pCallback) \
	((void *)(INT_PTR)FogAsyncDataLoadRaw((DWORD)(pPool), (DWORD)(pszFile), (DWORD)(fAsync), (DWORD)(dwOffset), \
	(DWORD)(nSize), (DWORD)(pbBuffer), (DWORD)(pCallback), (DWORD)D2_THIS_FILE, __LINE__))

#define D2_OPAQUE(x)	((void)0)

#define arrsize(a)		(sizeof(a) / sizeof((a)[0]))

#endif
