#ifndef D2RE_D2GAME_STORMTABLEMEMBERS_H
#define D2RE_D2GAME_STORMTABLEMEMBERS_H

#include "StormTables.h"

template <class T> inline T *TSList<T>::FreeElement(T *ptElement)
{
	T *ptNext = Link(ptElement)->Next();
	delete ptElement;
	return ptNext;
}

template <class T, class K> inline void TSHashTable<T, K>::GrowListArray()
{
	T *ptElement;
	DWORD dwOldSize;
	DWORD dwNewSize;
	DWORD i;

	dwOldSize = m_hashmask + 1;
	dwNewSize = 2 * dwOldSize;
	TSExplicitList<T, LIST_UNKNOWNOFFSET> tList;
	tList.ChangeLinkOffset(LINKOFFSET(T, m_linktoslot));
	for (i = 0; i < dwOldSize; i++)
	{
		while ((ptElement = m_slotlistarray[i].Head()) != NULL)
			tList.LinkNode(ptElement, LINK_BEFORE, NULL);
	}
	m_slotlistarray.SetCount(dwNewSize);
	for (i = 0; i < dwNewSize; i++)
		m_slotlistarray[i].ChangeLinkOffset(LINKOFFSET(T, m_linktoslot));
	m_hashmask = dwNewSize - 1;
	while ((ptElement = tList.Head()) != NULL)
		m_slotlistarray[ptElement->m_hashval & m_hashmask].LinkNode(ptElement, LINK_BEFORE, NULL);
}

template <class T, class K> inline T *TSExportTable<T, K>::Find(DWORD dwHashVal)
{
	return this->Ptr(dwHashVal, &m_key);
}

template <class T, class K> inline T *TSExportTableSync<T, K>::CreateLocked(DWORD *pdwHashVal, int *pnLock)
{
	SyncEnterLock(pnLock, TRUE);
	*pdwHashVal = this->GenerateUniqueHashVal();
	return this->AddHashElement(*pdwHashVal, &this->m_key, 0, 0);
}

#endif
