/*
 * $Id: cache.c,v 1.1 2004/12/19 22:03:09 lordjaxom Exp $
 */

#include "cache.h"
#include "bitmap.h"

cText2SkinCache::cText2SkinCache(int MaxItems) {
	mMaxItems = MaxItems;
}

cText2SkinCache::~cText2SkinCache() {
	Flush();
}

void cText2SkinCache::Delete(const key_type &Key, data_type &Data) {
	delete Data;
}

void cText2SkinCache::Flush(void) {
	mUsage.clear();
	name_iterator it = mNames.begin();
	for (; it != mNames.end(); ++it) {
		item_iterator it2 = (*it).second.begin();
		for (; it2 != (*it).second.end(); ++it2)
			Delete((*it2).first, (*it2).second);
		(*it).second.clear();
	}
	mNames.clear();
}

void cText2SkinCache::Reset(void) {
	name_iterator it = mNames.begin();
	for (; it != mNames.end(); ++it) {
		item_iterator it2 = (*it).second.begin();
		for (; it2 != (*it).second.end(); ++it2)
			(*it2).second->Reset();
	}
}

cText2SkinCache::data_type &cText2SkinCache::operator[](const key_type &Key) {
	name_iterator it = mNames.find(Key.Filename);
	if (it != mNames.end()) {
		item_iterator it2 = (*it).second.find(Key);
		if (it2 != (*it).second.end()) {
			usage_iterator ut = mUsage.begin();
			for (; ut != mUsage.end(); ++ut) {
				if ((*ut) == Key) {
					mUsage.erase(ut);
					break;
				}
			}
			mUsage.push_back(Key);
			return (*it2).second;
		}
	}

	if (it == mNames.end())
		it = mNames.insert(name_map::value_type(Key.Filename, item_map())).first;

	if ((int)mUsage.size() == mMaxItems) {
		usage_iterator ut = mUsage.begin();
		Delete(*ut, (*it).second[*ut]);
		(*it).second.erase(*ut);
		if ((*it).second.size() == 0)
			mNames.erase((*ut).Filename);
		mUsage.erase(mUsage.begin());
	}

	item_iterator it2 = (*it).second.insert(item_map::value_type(Key, data_type())).first;
	mUsage.push_back(Key);
	return (*it2).second;
}

