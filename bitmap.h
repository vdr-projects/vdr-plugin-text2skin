/*
 * $Id: bitmap.h,v 1.12 2004/06/18 16:08:11 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_BITMAP_H
#define VDR_TEXT2SKIN_BITMAP_H

#include "common.h"
#include "cache.h"
#include <vdr/osd.h>

class cText2SkinBitmap {
private:
	static cText2SkinCache mCache;

	vector<cBitmap*>   mBitmaps;
	int                mCurrent;
	time_t             mDelay;
	time_t             mLastGet;

	// disallow direct construction
	cText2SkinBitmap(void);

public:
	static cText2SkinBitmap *Load(const char *Filename, int Alpha = 0);
	static void ResetCache(void) { mCache.Reset(); }
	static void FlushCache(void) { mCache.Flush(); }
	
	virtual ~cText2SkinBitmap();

	void Reset(void) { mCurrent = 0; mLastGet = 0; }
	cBitmap &Get(int &UpdateIn);
	void SetColor(int Index, tColor Color);
	void SetAlpha(int Alpha);

	bool LoadXpm(const char *Filename, int Alpha);
#ifdef HAVE_IMLIB2
	bool LoadImlib(const char *Filename, int Alpha);
#endif
#ifdef HAVE_IMAGEMAGICK
	bool LoadMagick(const char *Filename, int Alpha);
#endif
};

inline void cText2SkinBitmap::SetColor(int Index, tColor Color) {
	mBitmaps[mCurrent]->SetColor(Index, Color);
}

#endif // VDR_TEXT2SKIN_BITMAP_H
