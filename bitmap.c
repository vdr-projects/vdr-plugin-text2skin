/*
 * $Id: bitmap.c,v 1.1.1.1 2004/05/23 00:08:03 lordjaxom Exp $
 */

#include <vdr/tools.h>
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
		if (strcmp(Filename + len - 4, ".xpm") == 0)
			return LoadXpm(Filename);
#ifdef HAVE_IMLIB
		else if (strcmp(Filename + len - 4, ".png") == 0)
			return LoadPng(Filename);
#endif
		else
			esyslog("ERROR: text2skin: unknown file format for %s", Filename);
	} else
		esyslog("ERROR: text2skin: filename %s too short to identify format", Filename);
	return false;
}

#ifdef HAVE_IMLIB
bool cText2SkinBitmap::LoadPng(const char *Filename) {
	Imlib_Image image;
	image = imlib_load_image(Filename);
	if (!image)
		return false;
	imlib_context_set_image(image);
	SetSize(imlib_image_get_width(), imlib_image_get_height());
	SetBpp(4);
	uint8_t *data = (uint8_t*)imlib_image_get_data_for_reading_only();
	int pal = 0, pos = 0;
	for (int y = 0; y < Height(); ++y) {
		for (int x = 0; x < Width(); ++x) {
			tColor col = (data[pos + 0] << 24) | (data[pos + 1] << 16) | (data[pos + 2] << 8) | data[pos + 3];
			int res = Index(col);
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
