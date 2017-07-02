//								-*- c++ -*-

#include "screen.h"

cText2SkinScreen::cText2SkinScreen(bool OffScreen):
	mOsd(OffScreen ? NULL : cOsdProvider::NewOsd(0, 0)),
	mScreen(OffScreen ? new cBitmap(720, 576, 8) : NULL),
	mOffScreen(OffScreen)
{
}

cText2SkinScreen::~cText2SkinScreen()
{
	delete mScreen;
	delete mOsd;
}

eOsdError cText2SkinScreen::SetAreas(const tArea *Areas, int NumAreas)
{
	if (!mOffScreen) {
		eOsdError result = mOsd->CanHandleAreas(Areas, NumAreas);
		if (result == oeOk)
			mOsd->SetAreas(Areas, NumAreas);
		else
			return result;
	}

	mNumRegions = NumAreas;
	for (int i = 0; i < mNumRegions; ++i)
		mRegions[i] = new cBitmap(Areas[i].Width(), Areas[i].Height(), Areas[i].bpp, Areas[i].x1, Areas[i].y1);
	Clear();

	return oeOk;
}

void cText2SkinScreen::Clear(void)
{
	for (int i = 0; i < mNumRegions; ++i) {
		mRegions[i]->Reset();
		mRegions[i]->Clean();
		mRegions[i]->DrawRectangle(mRegions[i]->X0(), mRegions[i]->Y0(), mRegions[i]->X0() + mRegions[i]->Width() - 1, mRegions[i]->Y0() + mRegions[i]->Height() - 1, clrTransparent);
	}
}

void cText2SkinScreen::DrawBitmap(int x, int y, const cBitmap &Bitmap, const tColor *ColorMask)
{
	for (int i = 0; i < mNumRegions; ++i)
		//DrawBitmapOverlay(*mRegions[i], x, y, (cBitmap&)Bitmap, ColorMask);
		mRegions[i]->DrawBitmap(x, y, Bitmap);
}

void cText2SkinScreen::DrawRectangle(int x1, int y1, int x2, int y2, tColor Color)
{
	for (int i = 0; i < mNumRegions; ++i)
		mRegions[i]->DrawRectangle(x1, y1, x2, y2, Color);
}

void cText2SkinScreen::DrawText(int x, int y, const char *s, tColor ColorFg, tColor ColorBg,
                                const cFont *Font, int Width, int Height, int Alignment)
{
	for (int i = 0; i < mNumRegions; ++i)
		mRegions[i]->DrawText(x, y, s, ColorFg, ColorBg, Font, Width, Height, Alignment);
}

void cText2SkinScreen::DrawEllipse(int x1, int y1, int x2, int y2, tColor Color, int Quadrants)
{
	for (int i = 0; i < mNumRegions; ++i)
		mRegions[i]->DrawEllipse(x1, y1, x2, y2, Color, Quadrants);
}

void cText2SkinScreen::DrawSlope(int x1, int y1, int x2, int y2, tColor Color, int Type)
{
	for (int i = 0; i < mNumRegions; ++i)
		mRegions[i]->DrawSlope(x1, y1, x2, y2, Color, Type);
}

void cText2SkinScreen::Flush(void)
{
	for (int i = 0; i < mNumRegions; ++i) {
		if (mOffScreen)
			mScreen->DrawBitmap(mRegions[i]->X0(), mRegions[i]->Y0(), *mRegions[i]);
		mOsd->DrawBitmap(mRegions[i]->X0(), mRegions[i]->Y0(), *mRegions[i]);
	}
	if (!mOffScreen)
		mOsd->Flush();
}

void cText2SkinScreen::DrawBitmapOverlay(cBitmap &Dest, int x, int y, cBitmap &Bitmap,
                                         const tColor *ColorMask)
{
	const tIndex *bitmap = Dest.Data(0, 0);
	if (bitmap && Bitmap.Data(0, 0) && Dest.Intersects(x, y, x + Bitmap.Width() - 1, y + Bitmap.Height() - 1)) {
		if (Dest.Covers(x, y, x + Bitmap.Width() - 1, y + Bitmap.Height() - 1))
			Dest.Reset();
		x -= Dest.X0();
		y -= Dest.Y0();
		tIndex Indexes[MAXNUMCOLORS];
		Dest.Take(Bitmap, &Indexes);
		tIndex Mask, *indexMask = NULL;
		if (ColorMask != NULL) {
			Mask = Dest.Index(*ColorMask);
			indexMask = &Mask;
		}
		for (int ix = 0; ix < Bitmap.Width(); ix++) {
			for (int iy = 0; iy < Bitmap.Height(); iy++) {
				if (indexMask == NULL || *indexMask != Indexes[int(*Bitmap.Data(ix, iy))])
					Dest.SetIndex(x + ix, y + iy, Indexes[int(*Bitmap.Data(ix, iy))]);
			}
		}
	}
}
