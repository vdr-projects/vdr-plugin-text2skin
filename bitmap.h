/*
 * $Id: bitmap.h,v 1.2 2004/12/08 18:47:37 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_BITMAP_H
#define VDR_TEXT2SKIN_BITMAP_H

#include "common.h"
#include "cache.h"
#include <vdr/osd.h>

class cText2SkinBitmap {
private:
	static cText2SkinCache mCache;

	std::vector<cBitmap*> mBitmaps;
	int                   mCurrent;
	time_t                mDelay;
	time_t                mLastGet;

	// disallow direct construction
	cText2SkinBitmap(void);

public:
	static cText2SkinBitmap *Load(const std::string &Filename, int Alpha = 0);
	static void ResetCache(void) { mCache.Reset(); }
	static void FlushCache(void) { mCache.Flush(); }
	
	virtual ~cText2SkinBitmap();

	void Reset(void) { mCurrent = 0; mLastGet = 0; }
	cBitmap &Get(int &UpdateIn);
	void SetColor(int Index, tColor Color);
	void SetAlpha(int Alpha);

	bool LoadXpm(const char *Filename);
#ifdef HAVE_IMLIB2
	bool LoadImlib(const char *Filename);
#endif
#ifdef HAVE_IMAGEMAGICK
	bool LoadMagick(const char *Filename);
#endif
};

inline void cText2SkinBitmap::SetColor(int Index, tColor Color) {
	mBitmaps[mCurrent]->SetColor(Index, Color);
}

#endif // VDR_TEXT2SKIN_BITMAP_H
