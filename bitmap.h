/*
 * $Id: bitmap.h,v 1.7 2004/06/05 01:39:36 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_BITMAP_H
#define VDR_TEXT2SKIN_BITMAP_H

#include "common.h"
#include "cache.h"
#include <vdr/osd.h>

class cText2SkinBitmap;
typedef cText2SkinCache<string,cText2SkinBitmap*> cImageCache;

class cText2SkinBitmap: public cBitmap {
private:
	static cImageCache mCache;

	// disallow direct construction
	cText2SkinBitmap(void);

public:
	static cText2SkinBitmap *Load(const char *Filename);
	
	virtual ~cText2SkinBitmap();

#ifdef HAVE_IMLIB2
	bool LoadImlib(const char *Filename);
#endif
#ifdef HAVE_IMAGEMAGICK
	bool LoadMagick(const char *Filename);
#endif
};

#endif // VDR_TEXT2SKIN_BITMAP_H
