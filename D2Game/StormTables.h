#ifndef D2RE_D2GAME_STORMTABLES_H
#define D2RE_D2GAME_STORMTABLES_H

#include <windows.h>
#include "StormImports.h"
class type_info;

#define LIST_UNKNOWNOFFSET	((int)0xCCCCCCCD)
#define LIST_UNSETOFFSET	((int)0xDDDDDDDD)
#define LINKOFFSET(T, m)	((int)((INT_PTR)&((T *)16)->m - 16))
#define LINK_AFTER			1
#define LINK_BEFORE			2

#define STORMLINE_TYPE		(-2)
#define STORM_ALLOC(nSize, pszType, dwFlags) \
	(StormSMemAlloc(nSize, pszType, STORMLINE_TYPE, \
	dwFlags))
#define STORM_REALLOC(p, nSize, pszType, dwFlags) \
	(StormSMemReAlloc(p, nSize, pszType, \
	STORMLINE_TYPE, dwFlags))
#define STORM_FREE(p, pszType) \
	(DWORD)StormSMemFree(p, pszType, STORMLINE_TYPE, 0)

struct SGAMEDATA;
#define TAG_EXPLICITLIST	((const char *)&typeid(TSExplicitList<SGAMEDATA, (int)0xDDDDDDDD>) + 8)

inline void * __cdecl operator new(unsigned int nSize, void *pMemory) { return pMemory; }

template <class T> struct TSLink
{
	TSLink<T>	*m_prevlink;
	T			*m_next;

	TSLink() { m_prevlink = NULL; m_next = NULL; }
	~TSLink() { Unlink(); }
	TSLink<T> *NextLink(int nLinkOffset)
	{
		if ((INT_PTR)m_next < 0)
			return (TSLink<T> *)~(INT_PTR)m_next;
		if (nLinkOffset < 0)
			nLinkOffset = (int)((BYTE *)this - (BYTE *)m_prevlink->m_next);
		return (TSLink<T> *)((BYTE *)m_next + nLinkOffset);
	}
	void Unlink()
	{
		if (m_prevlink)
		{
			NextLink(-1)->m_prevlink = m_prevlink;
			m_prevlink->m_next = m_next;
			m_prevlink = NULL;
			m_next = NULL;
		}
	}
	T *Next() { return (INT_PTR)m_next > 0 ? m_next : NULL; }
};

template <class T> struct TSList
{
	int			m_linkoffset;
	TSLink<T>	m_terminator;

	TSList()
	{
		m_linkoffset = 0;
		InitializeTerminator();
	}
	TSList(const TSList<T> &tList)
	{
		m_linkoffset = tList.m_linkoffset;
		InitializeTerminator();
	}
	~TSList()
	{
		T *ptElement;

		while ((ptElement = m_terminator.Next()) != NULL)
			UnlinkElement(ptElement);
	}
	void InitializeTerminator();
	void SetOffset(int nLinkOffset) { m_linkoffset = nLinkOffset; InitializeTerminator(); }
	void ChangeLinkOffset(int nLinkOffset)
	{
		if (nLinkOffset == m_linkoffset)
			return;
		UnlinkAll();
		SetOffset(nLinkOffset);
	}
	T *Head()
	{
		return m_terminator.Next();
	}
	TSLink<T> *Link(const T *ptElement)
	{
		if (ptElement)
			return (TSLink<T> *)((BYTE *)ptElement + m_linkoffset);
		return &m_terminator;
	}
	T *LinkNext(const T *ptElement) { return Link(ptElement)->m_next; }
	T *FreeElement(T *ptElement);
	void UnlinkElement(T *ptElement) { Link(ptElement)->Unlink(); }
	void UnlinkAll()
	{
		T *ptElement;

		while ((ptElement = Head()) != NULL)
			Link(ptElement)->Unlink();
	}
	void LinkNode(T *ptElement, int nWhere, T *ptRelative)
	{
		TSLink<T> *ptLink;
		TSLink<T> *ptRelativeLink;
		TSLink<T> *ptNeighbour;

		ptLink = ptElement ? Link(ptElement) : &m_terminator;
		if (ptLink->m_prevlink)
			ptLink->Unlink();
		if (ptRelative)
			ptRelativeLink = Link(ptRelative);
		else
			ptRelativeLink = &m_terminator;
		switch (nWhere)
		{
		case LINK_AFTER:
			ptLink->m_prevlink = ptRelativeLink;
			ptLink->m_next = ptRelativeLink->m_next;
			ptNeighbour = ptRelativeLink->NextLink(m_linkoffset);
			ptNeighbour->m_prevlink = ptLink;
			ptRelativeLink->m_next = ptElement;
			break;
		case LINK_BEFORE:
			ptNeighbour = ptRelativeLink->m_prevlink;
			ptLink->m_prevlink = ptNeighbour;
			ptLink->m_next = ptNeighbour->m_next;
			ptNeighbour->m_next = ptElement;
			ptRelativeLink->m_prevlink = ptLink;
			break;
		}
	}
};

template <class T, int nExplicitOffset> class TSExplicitList : public TSList<T>
{
public:
	TSExplicitList() { this->SetOffset(LIST_UNSETOFFSET); }
};

template <class T> struct TSGrowableArray
{
	DWORD		m_alloc;
	DWORD		m_count;
	T			*m_data;
	DWORD		m_chunk;

	void SetCount(DWORD dwCount);
	T &operator[](DWORD dwIndex) { return m_data[dwIndex]; }
};

struct HASHKEY_NONE
{
	BYTE		unobserved_00;

	BOOL operator==(const HASHKEY_NONE &tKey) const { return TRUE; }
};

template <class T, class K> struct TSHashObject
{
	DWORD		m_hashval;
	TSLink<T>	m_linktoslot;
	TSLink<T>	m_linktofull;
	K			m_key;
};

template <class T, class K> struct TSHashTable
{
	TSExplicitList<T, LIST_UNKNOWNOFFSET>	m_fulllist;
	DWORD		m_fullness;
	TSGrowableArray< TSExplicitList<T, LIST_UNKNOWNOFFSET> >	m_slotlistarray;
	DWORD		m_hashmask;

	virtual void DestroyElement(T *ptElement) = 0;
	virtual T *CreateElement(TSExplicitList<T, LIST_UNKNOWNOFFSET> *ptList, DWORD dwExtraBytes,
		DWORD dwFlags) = 0;
	T *AddHashElement(DWORD dwHashVal, const K *ptKey, DWORD dwExtraBytes, DWORD dwFlags)
	{
		T *ptElement;
		DWORD dwSlot;

		dwSlot = dwHashVal & m_hashmask;
		if (m_hashmask < 1023)
		{
			if (m_fullness > 3)
				m_fullness -= 3;
			else
				m_fullness = 0;
			for (T *ptEntry = m_slotlistarray[dwSlot].Head(), *ptDelete = NULL; (INT_PTR)ptEntry > 0;
				ptDelete
					? (ptEntry = m_slotlistarray[dwSlot].FreeElement(ptEntry),
						ptEntry = (INT_PTR)ptDelete > 0 ? ptEntry : NULL,
						ptDelete = NULL,
						ptEntry)
					: ptEntry = m_slotlistarray[dwSlot].Link(ptEntry)->m_next)
			{
				if (++m_fullness > 13)
				{
					m_fullness = 0;
					GrowListArray();
					dwSlot = dwHashVal & m_hashmask;
					break;
				}
			}
		}
		ptElement = CreateElement(&m_slotlistarray[dwSlot], dwExtraBytes, dwFlags);
		m_fulllist.LinkNode(ptElement, LINK_BEFORE, NULL);
		ptElement->m_hashval = dwHashVal;
		ptElement->m_key = *ptKey;
		return ptElement;
	}
	T *Ptr(DWORD dwHashVal, const K *ptKey)
	{
		for (T *ptElement = m_slotlistarray[dwHashVal & m_hashmask].Head(), *ptDelete = NULL; (INT_PTR)ptElement > 0;
			ptDelete
				? (ptElement = m_slotlistarray[dwHashVal & m_hashmask].FreeElement(ptElement),
					ptElement = (INT_PTR)ptDelete > 0 ? ptElement : NULL,
					ptDelete = NULL,
					ptElement)
				: ptElement = m_slotlistarray[dwHashVal & m_hashmask].LinkNext(ptElement))
		{
			DWORD dwElementHash = ptElement->m_hashval;
			const K *ptElementKey = &ptElement->m_key;

			if (dwElementHash == dwHashVal && *ptElementKey == *ptKey)
				return ptElement;
		}
		return NULL;
	}
	void GrowListArray();
	void UnlinkElement(T *ptElement)
	{
		if (ptElement->m_linktoslot.m_next)
		{
			ptElement->m_linktoslot.Unlink();
			ptElement->m_linktofull.Unlink();
		}
	}
	void Delete(T *ptElement)
	{
		UnlinkElement(ptElement);
		DestroyElement(ptElement);
	}
};

template <class T, class K> struct TSHashTableReuse : TSHashTable<T, K>
{
	TSExplicitList<T, LIST_UNKNOWNOFFSET>	m_reuselist;
	DWORD		m_chunksize;
	TSExplicitList<T, 16>	m_chunklist;
};

template <class T, class K> struct TSExportTable : TSHashTableReuse<T, K>
{
	K			m_key;
	DWORD		m_sequence;
	BOOL		m_wrapped;

	T *Find(DWORD dwHashVal);
	DWORD GenerateUniqueHashVal()
	{
		for (;;)
		{
			if (!++m_sequence)
			{
				m_wrapped = TRUE;
				continue;
			}
			if (!m_wrapped || !this->Ptr(m_sequence, &m_key))
				break;
		}
		return m_sequence;
	}
};

struct CCritSect : CRITICAL_SECTION
{
	void Enter(BOOL) { EnterCriticalSection(this); }
	void Leave(BOOL) { LeaveCriticalSection(this); }
};

template <class T, class K> struct TSExportTableSync : TSExportTable<T, K>
{
	CCritSect	m_sync;

	BOOL IsWriteLock(int nLock) { return nLock == 1; }
	T *Lock(DWORD dwHashVal, int *pnLock, BOOL bForWrite)
	{
		T *ptElement;

		SyncEnterLock(pnLock, bForWrite);
		ptElement = TSExportTable<T, K>::Find(dwHashVal);
		if (ptElement == NULL)
		{
			SyncLeaveLock(*pnLock);
			*pnLock = 0;
		}
		return ptElement;
	}
	void SyncEnterLock(int *pnLock, BOOL bForWrite)
	{
		m_sync.Enter(bForWrite);
		*pnLock = bForWrite ? 1 : -1;
	}
	void SyncLeaveLock(int nLock)
	{
		if (nLock)
			m_sync.Leave(IsWriteLock(nLock));
	}
	void ReleaseLock(int nLock) { SyncLeaveLock(nLock); }
	void DeleteLocked(T *ptElement, int nLock)
	{
		this->Delete(ptElement);
		ReleaseLock(nLock);
	}
	T *CreateLocked(DWORD *pdwHashVal, int *pnLock);
};

#endif
