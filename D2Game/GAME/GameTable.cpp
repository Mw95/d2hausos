#include "../D2game.h"
#include "../StormNew.h"

template <class T> void TSList<T>::InitializeTerminator()
{
	m_terminator.m_prevlink = &m_terminator;
	m_terminator.m_next = (T *)~(INT_PTR)&m_terminator;
}

inline void __cdecl operator delete(void *, void *) { }
template <class T> void SConstruct(T *ptObject) { new (ptObject) T; }
template <class T> void SCopyConstruct(T *ptObject, const T &tSource) { new (ptObject) T(tSource); }

template <class T> inline void SReallocData(TSGrowableArray<T> *ptArray, DWORD dwAlloc)
{
	T *ptOldData;
	DWORD dwMove;
	DWORD i;

	ptOldData = ptArray->m_data;
	for (i = dwAlloc; i < ptArray->m_count; i++)
		ptOldData[i].~T();
	ptArray->m_alloc = dwAlloc;
	ptArray->m_data = (T *)STORM_REALLOC(ptOldData, dwAlloc * sizeof(T), TAG_EXPLICITLIST, 16);
	if (!ptArray->m_data)
	{
		ptArray->m_data = (T *)STORM_ALLOC(dwAlloc * sizeof(T), TAG_EXPLICITLIST, 0);
		if (ptOldData)
		{
			dwMove = dwAlloc < ptArray->m_count ? dwAlloc : ptArray->m_count;
			for (i = 0; i < dwMove; i++)
			{
				SCopyConstruct(&ptArray->m_data[i], ptOldData[i]);
				ptOldData[i].~T();
			}
			STORM_FREE(ptOldData, TAG_EXPLICITLIST);
		}
	}
}

template <class T> inline DWORD SCalcChunkSize(TSGrowableArray<T> *ptArray, DWORD dwCount)
{
	if (dwCount < 256 / sizeof(T))
	{
		while (dwCount & (dwCount - 1))
			dwCount &= dwCount - 1;
		if (dwCount < 1)
			return 1;
	}
	else
	{
		ptArray->m_chunk = 256 / sizeof(T);
		dwCount = 256 / sizeof(T);
	}
	return dwCount;
}

inline DWORD SRoundUpTo(DWORD dwValue, DWORD dwChunk)
{
	DWORD dwRest;

	dwRest = dwValue % dwChunk;
	if (dwRest)
		dwValue += dwChunk - dwRest;
	return dwValue;
}

template <class T> inline DWORD SWholeChunks(TSGrowableArray<T> *ptArray, DWORD dwCount)
{
	DWORD dwChunk;

	dwChunk = ptArray->m_chunk ? ptArray->m_chunk : SCalcChunkSize(ptArray, dwCount);
	return SRoundUpTo(dwCount, dwChunk);
}

template <class T> inline void SGrowBy(TSGrowableArray<T> *ptArray, DWORD dwCount, BOOL bRound)
{
	dwCount += ptArray->m_count;
	if (dwCount > ptArray->m_alloc)
	{
		if (bRound)
			dwCount = SWholeChunks(ptArray, dwCount);
		SReallocData(ptArray, dwCount);
	}
}

template<> void TSGrowableArray< TSExplicitList<GAME, LIST_UNKNOWNOFFSET> >::SetCount(DWORD dwCount)
{
	DWORD dwGrow;

	if (dwCount > m_count)
	{
		dwGrow = dwCount - m_count;
		SGrowBy(this, dwGrow, TRUE);
		for (DWORD i = m_count; i < dwCount; i++)
			SConstruct(&m_data[i]);
	}
	else if (dwCount < m_count)
	{
		for (DWORD i = dwCount; i < m_count; i++)
			m_data[i].~TSExplicitList<GAME, LIST_UNKNOWNOFFSET>();
	}
	m_count = dwCount;
}

template struct TSList<GAME>;
