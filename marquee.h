/*
 *  $Id: marquee.h,v 1.3 2004/12/28 01:24:35 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_MARQUEE_H
#define VDR_TEXT2SKIN_MARQUEE_H

#include <vdr/osd.h>
#include <string>

class cFont;
class cText2SkinScreen;

class cText2SkinMarquee {
private:
	/*
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
	                  uint &UpdateIn);*/
public:
	static void DrawText(cText2SkinScreen *Screen, int Left, int Top, int Width, int Height,
	                     const std::string &Text, const cFont *Font, tColor ColorFg, tColor ColorBg,
	                     uint Delay, int &Offset, int &Direction, uint &NextTime);
	//static void DrawText(uint &UpdateIn);

	//const std::string &Text(void) const { return mText; }
};

#endif // VDR_TEXT2SKIN_MARQUEE_H
