/*
 * $Id: bitmap.h,v 1.4 2004/06/01 17:10:13 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_BITMAP_H
#define VDR_TEXT2SKIN_BITMAP_H

#define __STL_CONFIG_H
#include <vdr/osd.h>
#undef __STL_CONFIG_H

class cText2SkinBitmap: public cBitmap {
public:
	cText2SkinBitmap(void);
	cText2SkinBitmap(const char *Filename);
	virtual ~cText2SkinBitmap();

	bool Load(const char *Filename);
#ifdef HAVE_IMLIB2
	bool LoadImlib(const char *Filename);
#endif
};

#endif // VDR_TEXT2SKIN_BITMAP_H
