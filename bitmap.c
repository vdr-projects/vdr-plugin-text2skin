/*
 * $Id: bitmap.c,v 1.5 2004/06/01 17:10:13 lordjaxom Exp $
 */

#define __STL_CONFIG_H
#include <vdr/tools.h>
#undef __STL_CONFIG_H
#include "bitmap.h"
#define X_DISPLAY_MISSING
#include <Imlib2.h>

cText2SkinBitmap::cText2SkinBitmap(void): cBitmap(1, 1, 1) {
  imlib_set_cache_size(4096 * 1024);
}

cText2SkinBitmap::cText2SkinBitmap(const char *Filename): cBitmap(1, 1, 1) {
  imlib_set_cache_size(4096 * 1024);
	Load(Filename);
}

cText2SkinBitmap::~cText2SkinBitmap() {
}

bool cText2SkinBitmap::Load(const char *Filename) {
	int len = strlen(Filename);
	if (len > 4) {
#ifndef HAVE_IMLIB2
		if (strcmp(Filename + len - 4, ".xpm") == 0)
			return LoadXpm(Filename);
#else
		if (strcmp(Filename + len - 4, ".xpm") == 0 || strcmp(Filename + len - 4, ".png") == 0)
			return LoadImlib(Filename);
#endif
		else
			esyslog("ERROR: text2skin: unknown file format for %s", Filename);
	} else
		esyslog("ERROR: text2skin: filename %s too short to identify format", Filename);
	return false;
}

#ifdef HAVE_IMLIB2
bool cText2SkinBitmap::LoadImlib(const char *Filename) {
	Imlib_Image image;
	image = imlib_load_image(Filename);
	if (!image)
		return false;
	imlib_context_set_image(image);
	SetSize(imlib_image_get_width(), imlib_image_get_height());
	SetBpp(8);
	uint8_t *data = (uint8_t*)imlib_image_get_data_for_reading_only();
	int pal = 0, pos = 0;
	for (int y = 0; y < Height(); ++y) {
		for (int x = 0; x < Width(); ++x) {
			tColor col = (data[pos + 3] << 24) | (data[pos + 2] << 16) | (data[pos + 1] << 8) | data[pos + 0];
			int res = Index(col);
			//printf("color: r=%d,g=%d,b=%d,a=%d\n", data[pos], data[pos+1], data[pos+2], data[pos+3]);
			if (pal > 0 && res == 0)
				;//esyslog("ERROR: text2skin: Too many colors used in palette");
			else
				SetIndex(x, y, res);
			pos += 4;
		}
	}
	imlib_free_image();
	return true;
}
#endif
