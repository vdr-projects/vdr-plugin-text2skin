/*
 *  $Id: screen.h,v 1.2 2004/07/13 13:52:51 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_SCREEN_H
#define VDR_TEXT2SKIN_SCREEN_H

#include "common.h"
#include <vdr/osd.h>

class cText2SkinScreen {
private:
	cOsd    *mOsd;
	cBitmap *mRegions[MAXOSDAREAS];
	int      mNumRegions;

public:
	cText2SkinScreen(int x, int y);
	~cText2SkinScreen();

	eOsdError SetAreas(const tArea *Areas, int NumAreas);

	void Clear(void);
	void DrawBitmap(int x, int y, const cBitmap &Bitmap, tColor ColorFg = 0, tColor ColorBg = 0);
	void DrawRectangle(int x1, int y1, int x2, int y2, tColor Color);
	void DrawText(int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font, int Width = 0, int Height = 0, int Alignment = taDefault);
	void DrawEllipse(int x1, int y1, int x2, int y2, tColor Color, int Quadrants = 0);
	void DrawSlope(int x1, int y1, int x2, int y2, tColor Color, int Type);

	void Flush(void);
};

#endif // VDR_TEXT2SKIN_SCREEN_H
