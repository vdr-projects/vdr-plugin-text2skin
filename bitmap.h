/*
 * $Id: bitmap.h,v 1.9 2004/06/08 20:45:52 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_BITMAP_H
#define VDR_TEXT2SKIN_BITMAP_H

#include "common.h"
#include "cache.h"
#include <vdr/osd.h>

class cText2SkinBitmap;
typedef cText2SkinCache<string,cText2SkinBitmap*> cImageCache;

class cText2SkinBitmap {
private:
	static cImageCache mCache;

	vector<cBitmap*> mBitmaps;
	int              mCurrent;
	time_t           mDelay;
	time_t           mLastGet;

	// disallow direct construction
	cText2SkinBitmap(void);

public:
	static cText2SkinBitmap *Load(const char *Filename);
	static void FlushCache(void) { mCache.Flush(); }
	
	virtual ~cText2SkinBitmap();

	cBitmap &Get(int &UpdateIn);
	void SetColor(int Index, tColor Color) { mBitmaps[mCurrent]->SetColor(Index, Color); }

	bool LoadXpm(const char *Filename);
#ifdef HAVE_IMLIB2
	bool LoadImlib(const char *Filename);
#endif
#ifdef HAVE_IMAGEMAGICK
	bool LoadMagick(const char *Filename);
#endif
};

#endif // VDR_TEXT2SKIN_BITMAP_H
