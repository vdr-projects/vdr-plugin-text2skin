/*
 * $Id: cache.h,v 1.1 2004/12/19 22:03:09 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_CACHE_HPP
#define VDR_TEXT2SKIN_CACHE_HPP

#include "common.h"
#include "xml/object.h"
#include <vdr/tools.h>
#include <map>
#include <vector>
#include <string>

class cText2SkinBitmap;

struct tBitmapSpec {
	std::string Filename;
	int         Alpha;
	int         Width;
	int         Height;
	int         Colors;

	tBitmapSpec(const std::string &filename, int alpha, int width, int height, int colors):
		Filename(filename), Alpha(alpha), Width(width), Height(height), Colors(colors) {}

	bool operator<(const tBitmapSpec &Src) const;
	bool operator==(const tBitmapSpec &Src) const;
};

inline bool tBitmapSpec::operator<(const tBitmapSpec &Src) const 
{
	if (Filename == Src.Filename) {
		if (Alpha == Src.Alpha) {
			if (Width == Src.Width) {
				if (Height == Src.Height)
					return Colors < Src.Colors;
				return Height < Src.Height;
			}
			return Width < Src.Width;
		}
		return Alpha < Src.Alpha;
	}
	return Filename < Src.Filename;
}
	
inline bool tBitmapSpec::operator==(const tBitmapSpec &Src) const
{
	return Filename == Src.Filename
	    && Alpha == Src.Alpha
		&& Width == Src.Width
		&& Height == Src.Height
		&& Colors == Src.Colors;
}

class cText2SkinCache {
private:
	typedef std::string       name_type;
	typedef tBitmapSpec       key_type;
	typedef cText2SkinBitmap* data_type;

	typedef std::map<key_type,data_type> item_map;
	typedef item_map::iterator item_iterator;
	typedef std::map<name_type,item_map> name_map;
	typedef name_map::iterator name_iterator;
	typedef std::vector<key_type> usage_list;
	typedef usage_list::iterator usage_iterator;

	name_map   mNames;
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
	bool Contains(const name_type &Name);
	data_type &operator[](const key_type &Key);
	uint Count(void) { return mUsage.size(); }
};

inline bool cText2SkinCache::Contains(const key_type &Key) {
	name_iterator it = mNames.find(Key.Filename);
	if (it != mNames.end())
		return (*it).second.find(Key) != (*it).second.end();
	return false;
}

inline bool cText2SkinCache::Contains(const name_type &Key) {
	return mNames.find(Key) != mNames.end();
}

#endif // VDR_TEXT2SKIN_CACHE_HPP
