/*
 * $Id: bitmap.h,v 1.6 2004/06/02 20:43:05 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_BITMAP_H
#define VDR_TEXT2SKIN_BITMAP_H

#include "common.h"
#include <vdr/osd.h>

class cText2SkinBitmap: public cBitmap {
public:
	cText2SkinBitmap(void);
	cText2SkinBitmap(const char *Filename);
	virtual ~cText2SkinBitmap();

	bool Load(const char *Filename);
#ifdef HAVE_IMLIB2
	bool LoadImlib(const char *Filename);
#endif
#ifdef HAVE_IMAGEMAGICK
	bool LoadMagick(const char *Filename);
#endif
};

#endif // VDR_TEXT2SKIN_BITMAP_H
