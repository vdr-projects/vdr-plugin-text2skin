/*
 * $Id: cache.h,v 1.5 2004/06/18 16:08:11 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_CACHE_HPP
#define VDR_TEXT2SKIN_CACHE_HPP

#include "common.h"
#include <vdr/tools.h>

class cText2SkinBitmap;

class cText2SkinCache {
private:
	typedef string            key_type;
	typedef cText2SkinBitmap* data_type;

	typedef map<key_type,data_type> item_map;
	typedef item_map::iterator item_iterator;
	typedef vector<key_type> usage_list;
	typedef usage_list::iterator usage_iterator;

	item_map   mItems;
	usage_list mUsage;
	int        mMaxItems;

protected:
	void Delete(const key_type &Key, data_type &Data);

public:
	cText2SkinCache(int MaxItems);
	~cText2SkinCache();

	void Reset(void);
	void Flush(void);
	bool Contains(const key_type &Key);
	data_type &operator[](const key_type &Key);
	uint Count(void) { return mItems.size(); }
};

inline bool cText2SkinCache::Contains(const key_type &Key) {
	return mItems.find(Key) != mItems.end();
}

#endif // VDR_TEXT2SKIN_CACHE_HPP
