/*
 * $Id: bitmap.c,v 1.9 2004/06/02 20:43:05 lordjaxom Exp $
 */

#include "bitmap.h"
#include <vdr/tools.h>
#define X_DISPLAY_MISSING
#ifdef HAVE_IMLIB2
#include <Imlib2.h>
#endif
#ifdef HAVE_IMAGEMAGICK
#include <Magick++.h>
#endif

cText2SkinBitmap::cText2SkinBitmap(void): cBitmap(1, 1, 1) {
#ifdef HAVE_IMLIB2
  imlib_set_cache_size(4096 * 1024);
#endif
}

cText2SkinBitmap::cText2SkinBitmap(const char *Filename): cBitmap(1, 1, 1) {
#ifdef HAVE_IMLIB2
  imlib_set_cache_size(4096 * 1024);
#endif
	Load(Filename);
}

cText2SkinBitmap::~cText2SkinBitmap() {
}

bool cText2SkinBitmap::Load(const char *Filename) {
	int len = strlen(Filename);
	if (len > 4) {
		if (strcmp(Filename + len - 4, ".xpm") == 0)
			return LoadXpm(Filename);
#ifdef HAVE_IMLIB2
		else if (strcmp(Filename + len - 4, ".png") == 0)
			return LoadImlib(Filename);
#else
#	ifdef HAVE_IMAGEMAGICK
		else if (strcmp(Filename + len - 4, ".png") == 0)
			return LoadMagick(Filename);
#	endif
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
	Imlib_Context ctx = imlib_context_new();
	imlib_context_push(ctx);
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
	imlib_context_free(ctx);
	return true;
}
#endif

#ifdef HAVE_IMAGEMAGICK
bool cText2SkinBitmap::LoadMagick(const char *Filename) {
	Magick::Image image;
	try {
		int w, h;
		image.read(Filename);
		w = image.columns();
		h = image.rows();
		SetSize(w, h);
		SetBpp(8);

		const Magick::PixelPacket *ptr = image.getConstPixels(0, 0, w, h);
		for (int iy = 0; iy < h; ++iy) {
			for (int ix = 0; ix < w; ++ix) {
				tColor col = (((~ptr->opacity & 0xFF00) << 16) | ((ptr->red & 0xFF00) << 8) | (ptr->green & 0xFF00) | ((ptr->blue & 0xFF00) >> 8));
				DrawPixel(ix, iy, col);
				++ptr;
			}
		}
	} catch (Magick::Exception &e) {
		esyslog("ERROR: text2skin: Couldn't load %s: %s", Filename, e.what());
		return false;
	}
	return true;
}
#endif
