/*
 * $Id: bitmap.c,v 1.15 2004/06/12 19:16:11 lordjaxom Exp $
 */

#include "bitmap.h"
#include "setup.h"
#include <vdr/tools.h>
#define X_DISPLAY_MISSING
#ifdef HAVE_IMLIB2
#include <Imlib2.h>
#endif
#ifdef HAVE_IMAGEMAGICK
#include <Magick++.h>
using Magick::Image;
using Magick::PixelPacket;
using Magick::Exception;
#endif

template<> 
void cImageCache::Delete(string &key, cText2SkinBitmap *&value) {
	delete value;
}

cImageCache cText2SkinBitmap::mCache(Text2SkinSetup.MaxCacheFill);

cText2SkinBitmap::cText2SkinBitmap(void) {
	mCurrent = 0;
	mLastGet = 0;
}

cText2SkinBitmap::~cText2SkinBitmap() {
	for (int i = 0; i < (int)mBitmaps.size(); ++i)
		delete mBitmaps[i];
}

cBitmap &cText2SkinBitmap::Get(int &UpdateIn) {
	if (mBitmaps.size() == 1)
		return *mBitmaps[0];

	time_t upd, cur = time_ms();
	int diff;
	if (mLastGet == 0) {
		Dprintf("lastget was %d\n", mLastGet);
		mLastGet = cur;
		upd = mDelay;
	} else if ((diff = cur - mLastGet) >= mDelay) {
		mCurrent = (mCurrent + 1) % mBitmaps.size();
		mLastGet = cur;
		upd = mDelay;
	} else {
		upd = mDelay - diff;
	}

	Dprintf("delay: %d, diff %d\n", mDelay, diff);

	if (UpdateIn == 0 || UpdateIn > upd)
		UpdateIn = upd;

	Dprintf("Get: returning frame %d\n", mCurrent);
	return *mBitmaps[mCurrent];
}

cText2SkinBitmap *cText2SkinBitmap::Load(const char *Filename, int Alpha) {
	if (mCache.Contains(Filename)) {
		return mCache[Filename];
	} else {
		cText2SkinBitmap *bmp = new cText2SkinBitmap;
		int len = strlen(Filename);
		bool result = false;
		if (len > 4) {
			if (strcmp(Filename + len - 4, ".xpm") == 0)
				result = bmp->LoadXpm(Filename, Alpha);
			else {
#ifdef HAVE_IMLIB2
				result = bmp->LoadImlib(Filename, Alpha);
#else
#	ifdef HAVE_IMAGEMAGICK
				result = bmp->LoadMagick(Filename, Alpha);
#	endif
#endif
			}
			//else
				//esyslog("ERROR: text2skin: unknown file format for %s", Filename);
		} else
			esyslog("ERROR: text2skin: filename %s too short to identify format", Filename);
	
		if (result)
			return (mCache[Filename] = bmp);
	}
	return false;
}

bool cText2SkinBitmap::LoadXpm(const char *Filename, int Alpha) {
	cBitmap *bmp = new cBitmap(1,1,1);
	if (bmp->LoadXpm(Filename)) {
		int count;
		if (Alpha && bmp->Colors(count)) {
			for (int i = 0; i < count; ++i) 
				bmp->SetColor(i, (bmp->Color(i) & 0x00FFFFFF) | (Alpha << 24));
		}
		mBitmaps.push_back(bmp);
		return true;
	}
	delete bmp;
	return false;
}

#ifdef HAVE_IMLIB2
bool cText2SkinBitmap::LoadImlib(const char *Filename, int Alpha) {
	Imlib_Image image;
	cBitmap *bmp = NULL;
	image = imlib_load_image(Filename);
	if (!image)
		return false;
	Imlib_Context ctx = imlib_context_new();
	imlib_context_push(ctx);
	imlib_context_set_image(image);
	bmp = new cBitmap(imlib_image_get_width(), imlib_image_get_height(), 8);
	uint8_t *data = (uint8_t*)imlib_image_get_data_for_reading_only();
	int pal = 0, pos = 0;
	for (int y = 0; y < bmp->Height(); ++y) {
		for (int x = 0; x < bmp->Width(); ++x) {
			tColor col = (data[pos + 3] << 24) | (data[pos + 2] << 16) | (data[pos + 1] << 8) | data[pos + 0];
			if (Alpha)
				col = (col & 0x00FFFFFF) | (Alpha << 24);
			int res = bmp->Index(col);
			if (pal > 0 && res == 0)
				;//esyslog("ERROR: text2skin: Too many colors used in palette");
			else
				bmp->SetIndex(x, y, res);
			pos += 4;
		}
	}
	imlib_free_image();
	imlib_context_free(ctx);
	mBitmaps.push_back(bmp);
	return true;
}
#endif

#ifdef HAVE_IMAGEMAGICK
bool cText2SkinBitmap::LoadMagick(const char *Filename, int Alpha) {
	vector<Image> images;
	cBitmap *bmp = NULL;
	try {
		int w, h;
		vector<Image>::iterator it;
		readImages(&images, Filename);
		if (images.size() == 0) {
			esyslog("ERROR: text2skin: Couldn't load %s", Filename);
			return false;
		}
		mDelay = images[0].animationDelay() * 10;
		for (it = images.begin(); it != images.end(); ++it) {
			w = (*it).columns();
			h = (*it).rows();
			bmp = new cBitmap(w, h, 8);

			const PixelPacket *ptr = (*it).getConstPixels(0, 0, w, h);
			for (int iy = 0; iy < h; ++iy) {
				for (int ix = 0; ix < w; ++ix) {
					tColor col = (((~ptr->opacity & 0xFF00) << 16) | ((ptr->red & 0xFF00) << 8) | (ptr->green & 0xFF00) | ((ptr->blue & 0xFF00) >> 8));
					if (Alpha)
						col = (col & 0x00FFFFFF) | (Alpha << 24);
					bmp->DrawPixel(ix, iy, col);
					++ptr;
				}
			}
			mBitmaps.push_back(bmp);
		}
	} catch (Exception &e) {
		esyslog("ERROR: text2skin: Couldn't load %s: %s", Filename, e.what());
		delete bmp;
		return false;
	}
	return true;
}
#endif
