/*
 * $Id: bitmap.h,v 1.4 2005/01/26 20:44:06 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_BITMAP_H
#define VDR_TEXT2SKIN_BITMAP_H

#include "common.h"
#include "cache.h"
#include <vdr/osd.h>

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

class cText2SkinBitmap {
private:
	static cxCache<tBitmapSpec,cText2SkinBitmap*> mCache;

	std::vector<cBitmap*> mBitmaps;
	int                   mCurrent;
	time_t                mDelay;
	time_t                mLastGet;

	// disallow direct construction
	cText2SkinBitmap(void);

public:
	static cText2SkinBitmap *Load(const std::string &Filename, int Alpha = 0, int height = 0, 
	                              int width = 0, int colors = 0, bool Quiet = false);
	static bool Available(const std::string &Filename, int Alpha = 0, int height = 0, 
	                      int width = 0, int colors = 0);
	static void ResetCache(void) { mCache.Reset(); }
	static void FlushCache(void) { mCache.Flush(); }
	
	virtual ~cText2SkinBitmap();

	void Reset(void) { mCurrent = 0; mLastGet = 0; }
	cBitmap &Get(uint &UpdateIn, uint Now);
	void SetColor(int Index, tColor Color);
	void SetAlpha(int Alpha);

	bool LoadXpm(const char *Filename);
#ifdef HAVE_IMLIB2
	bool LoadImlib(const char *Filename,int height, int width, int colors, bool Quiet);
#endif
#ifdef HAVE_IMAGEMAGICK
	bool LoadMagick(const char *Filename,int height, int width, int colors, bool Quiet);
#endif
};

inline void cText2SkinBitmap::SetColor(int Index, tColor Color) {
	mBitmaps[mCurrent]->SetColor(Index, Color);
}

template<>
void cxCache<tBitmapSpec,cText2SkinBitmap*>::Delete(const tBitmapSpec &Key, 
                                                    cText2SkinBitmap *&Data);
template<>
void cxCache<tBitmapSpec,cText2SkinBitmap*>::Reset(cText2SkinBitmap *&Data);

#endif // VDR_TEXT2SKIN_BITMAP_H
