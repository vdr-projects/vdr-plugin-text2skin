/*
 *  $Id: marquee.h,v 1.1 2004/12/19 22:03:14 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_MARQUEE_H
#define VDR_TEXT2SKIN_MARQUEE_H

#include <vdr/osd.h>
#include <string>

class cFont;
class cText2SkinScreen;

class cText2SkinMarquee {
private:
	cText2SkinScreen *mScreen;
	const cFont      *mFont;
	int               mLeft;
	int               mTop;
	int               mWidth;
	int               mHeight;
	std::string       mText;
	bool              mScrolling;
	int               mOffset;
	int               mDirection;
	tColor            mColorFg;
	tColor            mColorBg;
	uint              mNextTime;

	cText2SkinMarquee(void) {} // disallow direct construction

public:
	cText2SkinMarquee(const cText2SkinMarquee &Src);
	cText2SkinMarquee(cText2SkinScreen *Screen, int Left, int Top, int Width, int Height,
	                  const std::string &Text, const cFont *Font, tColor ColorFg, tColor ColorBg, 
	                  int &UpdateIn);

	void Set(cText2SkinScreen *Screen, int Left, int Top, int Width, int Height,
	         const std::string &Text, const cFont *Font, tColor ColorFg, tColor ColorBg, 
	         int &UpdateIn);
	void DrawText(int &UpdateIn);
};

#endif // VDR_TEXT2SKIN_MARQUEE_H
