/*
 * $Id: bitmap.c,v 1.2 2004/12/21 20:26:25 lordjaxom Exp $
 */

#include "bitmap.h"
#include "quantize.h"
#include "setup.h"
#include <vdr/tools.h>
#define X_DISPLAY_MISSING
#ifdef HAVE_IMLIB2
#include <Imlib2.h>
#endif
#ifdef HAVE_IMAGEMAGICK
#include <Magick++.h>
using namespace Magick;
#endif

cText2SkinCache cText2SkinBitmap::mCache(Text2SkinSetup.MaxCacheFill);

cText2SkinBitmap *cText2SkinBitmap::Load(const std::string &Filename, int Alpha, int height, int width, int colors) {
	tBitmapSpec spec(Filename, Alpha, height, width, colors);

	cText2SkinBitmap *res = NULL;
	if (mCache.Contains(spec))
		res = mCache[spec];
	else {
		res = new cText2SkinBitmap;
		int len = Filename.length();
		bool result = false;
		if (len > 4) {
			if (Filename.substr(len - 4, 4) == ".xpm")
				result = res->LoadXpm(Filename.c_str());
			else {
#ifdef HAVE_IMLIB2
				result = res->LoadImlib(Filename.c_str(),height,width,colors, false);
#else
#	ifdef HAVE_IMAGEMAGICK
				result = res->LoadMagick(Filename.c_str(),height,width,colors, false);
#	else
				esyslog("ERROR: text2skin: unknown file format for %s", Filename);
#	endif
#endif
			}
		} else
			esyslog("ERROR: text2skin: filename %s too short to identify format", Filename.c_str());
	
		if (result) {
			res->SetAlpha(Alpha);
			mCache[spec] = res;
		} else
			DELETENULL(res);
	}
	return res;
}

bool cText2SkinBitmap::Available(const std::string &Filename)
{
	bool res = false;
	if (mCache.Contains(Filename))
		res = true;
	else {
		cText2SkinBitmap *bmp = new cText2SkinBitmap;
		int len = Filename.length();
		if (len > 4) {
			if (Filename.substr(len - 4, 4) == ".xpm")
				res = bmp->LoadXpm(Filename.c_str());
			else {
#ifdef HAVE_IMLIB2
				res = bmp->LoadImlib(Filename.c_str(), 0, 0, 0, true);
#else
#	ifdef HAVE_IMAGEMAGICK
				res = bmp->LoadMagick(Filename.c_str(), 0, 0, 0, true);
#	endif
#endif
			}
		}
		delete bmp;
	}
	return res;
}

cText2SkinBitmap::cText2SkinBitmap(void) {
	mCurrent = 0;
	mLastGet = 0;
}

cText2SkinBitmap::~cText2SkinBitmap() {
	for (int i = 0; i < (int)mBitmaps.size(); ++i)
		delete mBitmaps[i];
	mBitmaps.clear();
}

cBitmap &cText2SkinBitmap::Get(uint &UpdateIn) {
	if (mBitmaps.size() == 1)
		return *mBitmaps[0];

	time_t upd, cur = time_ms();
	int diff;
	if (mLastGet == 0) {
		mLastGet = cur;
		upd = mDelay;
	} else if ((diff = cur - mLastGet) >= mDelay) {
		mCurrent = (mCurrent + 1) % mBitmaps.size();
		mLastGet = cur;
		upd = mDelay - diff > 1 ? mDelay - diff : 1;
	} else {
		upd = mDelay - diff;
	}

	if (UpdateIn == 0 || UpdateIn > (uint)upd)
		UpdateIn = upd;

	return *mBitmaps[mCurrent];
}

void cText2SkinBitmap::SetAlpha(int Alpha) {
	if (Alpha > 0) {
		std::vector<cBitmap*>::iterator it = mBitmaps.begin();
		for (; it != mBitmaps.end(); ++it) {
			int count;
			if ((*it)->Colors(count)) {
				for (int i = 0; i < count; ++i) {
					int alpha = (((*it)->Color(i) & 0xFF000000) >> 24) * Alpha / 255;
					(*it)->SetColor(i, ((*it)->Color(i) & 0x00FFFFFF) | (alpha << 24));
				}
			}
		}
	}
}

bool cText2SkinBitmap::LoadXpm(const char *Filename) {
	cBitmap *bmp = new cBitmap(1,1,1);
	if (bmp->LoadXpm(Filename)) {
		mBitmaps.push_back(bmp);
		return true;
	}
	delete bmp;
	return false;
}

#ifdef HAVE_IMLIB2
bool cText2SkinBitmap::LoadImlib(const char *Filename, int height, int width, int colors, bool Quiet) {
	Imlib_Image image;
        unsigned char * outputImage = NULL;
	unsigned int * outputPalette = NULL;
	cQuantizeWu* quantizer = new cQuantizeWu();
	cBitmap *bmp = NULL;
	image = imlib_load_image(Filename);
	if (!image)
		return false;
	Imlib_Context ctx = imlib_context_new();
	imlib_context_push(ctx);
	if (height != 0 || width != 0){
		imlib_context_set_image(image);
		image = imlib_create_cropped_scaled_image(0,0,imlib_image_get_width(), imlib_image_get_height() ,width , height);
	}
	imlib_context_set_image(image);
	bmp = new cBitmap(imlib_image_get_width(), imlib_image_get_height(), 8);
	uint8_t *data = (uint8_t*)imlib_image_get_data_for_reading_only();
	if ( colors != 0 ){
        	quantizer->Quantize(data, imlib_image_get_width()* imlib_image_get_height(), colors);
		outputImage = quantizer->OutputImage();
		outputPalette = quantizer->OutputPalette();
	}
	int pos = 0;
	for (int y = 0; y < bmp->Height(); ++y) {
		for (int x = 0; x < bmp->Width(); ++x) {
			if ( colors != 0 ){
				bmp->DrawPixel(x, y ,  outputPalette[outputImage[y * bmp->Width() + x]] | 0xFF000000 );
			}else{	
				tColor col = (data[pos + 3] << 24) | (data[pos + 2] << 16) | (data[pos + 1] << 8) | data[pos + 0];
				bmp->DrawPixel(x, y, col);
				pos += 4;
			}
		}
	}
	
	imlib_free_image();
	imlib_context_free(ctx);
	mBitmaps.push_back(bmp);
	delete(quantizer);
	return true;
}
#endif

#ifdef HAVE_IMAGEMAGICK
bool cText2SkinBitmap::LoadMagick(const char *Filename, int height, int width, int colors, bool Quiet) {
	std::vector<Image> images;
	cBitmap *bmp = NULL;
	try {
		int w, h;
		std::vector<Image>::iterator it;
		readImages(&images, Filename);
		if (images.size() == 0) {
			esyslog("ERROR: text2skin: Couldn't load %s", Filename);
			return false;
		}
		mDelay = images[0].animationDelay() * 10;
		for (it = images.begin(); it != images.end(); ++it) {
			if (colors != 0){
				(*it).opacity(OpaqueOpacity);
				(*it).backgroundColor( Color ( 0,0,0,0) );
				(*it).quantizeColorSpace( RGBColorspace );
				(*it).quantizeColors( colors );
				(*it).quantize();
			}
			 if (height != 0 || width != 0)
				(*it).sample(Geometry(width,height));
			w = (*it).columns();
			h = (*it).rows();
			if ((*it).depth() > 8) {
				esyslog("ERROR: text2skin: More than 8bpp images are not supported");
				return false;
			}
			bmp = new cBitmap(w, h, (*it).depth());
			//Dprintf("this image has %d colors\n", (*it).totalColors());

			const PixelPacket *pix = (*it).getConstPixels(0, 0, w, h);
			for (int iy = 0; iy < h; ++iy) {
				for (int ix = 0; ix < w; ++ix) {
					tColor col = (~(pix->opacity * 255 / MaxRGB) << 24) | ((pix->red * 255 / MaxRGB) << 16) | ((pix->green * 255 / MaxRGB) << 8) | (pix->blue * 255 / MaxRGB);
					bmp->DrawPixel(ix, iy, col);
					++pix;
				}
			}
			mBitmaps.push_back(bmp);
		}
	} catch (Exception &e) {
		if (!Quiet)
			esyslog("ERROR: text2skin: Couldn't load %s: %s", Filename, e.what());
		delete bmp;
		return false;
	}
	return true;
}
#endif
