/*
 * $Id: cache.c,v 1.2 2004/06/18 16:08:11 lordjaxom Exp $
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
	item_iterator it = mItems.begin();
	for (; it != mItems.end(); ++it)
		Delete((*it).first, (*it).second);
	mItems.clear();
}

void cText2SkinCache::Reset(void) {
	item_iterator it = mItems.begin();
	for (; it != mItems.end(); ++it)
		(*it).second->Reset();
}

cText2SkinCache::data_type &cText2SkinCache::operator[](const key_type &Key) {
	item_iterator it = mItems.find(Key);
	if (it != mItems.end()) {
		usage_iterator ut = mUsage.begin();
		for (; ut != mUsage.end(); ++ut) {
			if ((*ut) == Key) {
				mUsage.erase(ut);
				break;
			}
		}
		mUsage.push_back(Key);
	} else {
		if ((int)mItems.size() == mMaxItems) {
			usage_iterator ut = mUsage.begin();
			Delete(*ut, mItems[*ut]);
			mItems.erase(*ut);
			mUsage.erase(mUsage.begin());
		}
		it = mItems.insert(item_map::value_type(Key, data_type())).first;
		mUsage.push_back(Key);
	}
	return (*it).second;
}

