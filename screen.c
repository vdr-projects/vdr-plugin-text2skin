/*
 *  $Id: screen.c,v 1.2 2004/07/13 13:52:51 lordjaxom Exp $
 */

#include "screen.h"

cText2SkinScreen::cText2SkinScreen(int x, int y) {
	mOsd = cOsdProvider::NewOsd(x, y);
}

cText2SkinScreen::~cText2SkinScreen() {
	delete mOsd;
} 

eOsdError cText2SkinScreen::SetAreas(const tArea *Areas, int NumAreas) {
	eOsdError result = mOsd->CanHandleAreas(Areas, NumAreas);
	if (result == oeOk) {
		mOsd->SetAreas(Areas, NumAreas);
		mNumRegions = NumAreas;
		for (int i = 0; i < mNumRegions; ++i)
			mRegions[i] = new cBitmap(Areas[i].Width(), Areas[i].Height(), Areas[i].bpp, Areas[i].x1, Areas[i].y1);
		Clear();
	}
	return result;
}

void cText2SkinScreen::Clear(void) {
	for (int i = 0; i < mNumRegions; ++i) {
		mRegions[i]->Reset();
		mRegions[i]->Clean();
		mRegions[i]->DrawRectangle(mRegions[i]->X0(), mRegions[i]->Y0(), mRegions[i]->X0() + mRegions[i]->Width() - 1, mRegions[i]->Y0() + mRegions[i]->Height() - 1, clrTransparent);
	}
}

void cText2SkinScreen::DrawBitmap(int x, int y, const cBitmap &Bitmap, tColor ColorFg, tColor ColorBg) {
	for (int i = 0; i < mNumRegions; ++i)
		mRegions[i]->DrawBitmap(x, y, Bitmap, ColorFg, ColorBg);
}

void cText2SkinScreen::DrawRectangle(int x1, int y1, int x2, int y2, tColor Color) {
	for (int i = 0; i < mNumRegions; ++i)
		mRegions[i]->DrawRectangle(x1, y1, x2, y2, Color);
}

void cText2SkinScreen::DrawText(int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font, int Width, int Height, int Alignment) {
	for (int i = 0; i < mNumRegions; ++i)
		mRegions[i]->DrawText(x, y, s, ColorFg, ColorBg, Font, Width, Height, Alignment);
}

void cText2SkinScreen::DrawEllipse(int x1, int y1, int x2, int y2, tColor Color, int Quadrants) {
	for (int i = 0; i < mNumRegions; ++i)
		mRegions[i]->DrawEllipse(x1, y1, x2, y2, Color, Quadrants);
}

void cText2SkinScreen::DrawSlope(int x1, int y1, int x2, int y2, tColor Color, int Type) {
	for (int i = 0; i < mNumRegions; ++i)
		mRegions[i]->DrawSlope(x1, y1, x2, y2, Color, Type);
}

void cText2SkinScreen::Flush(void) {
	for (int i = 0; i < mNumRegions; ++i)
		mOsd->DrawBitmap(mRegions[i]->X0(), mRegions[i]->Y0(), *mRegions[i]);
	mOsd->Flush();
}

