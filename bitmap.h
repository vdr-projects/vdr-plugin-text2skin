/*
 * $Id: bitmap.h,v 1.1.1.1 2004/05/23 00:08:03 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_BITMAP_H
#define VDR_TEXT2SKIN_BITMAP_H

#include <vdr/osd.h>

class cText2SkinBitmap: public cBitmap {
public:
	cText2SkinBitmap(void);
	cText2SkinBitmap(const char *Filename);
	virtual ~cText2SkinBitmap();

	bool Load(const char *Filename);
#ifdef HAVE_IMLIB
	bool LoadPng(const char *Filename);
#endif
};

#endif // VDR_TEXT2SKIN_BITMAP_H
