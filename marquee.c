/*
 *  $Id: marquee.c,v 1.1 2004/12/19 22:03:14 lordjaxom Exp $
 */

#include "marquee.h"
#include "screen.h"
#include <vdr/tools.h>

cText2SkinMarquee::cText2SkinMarquee(const cText2SkinMarquee &Src):
		mScreen(Src.mScreen),
		mFont(Src.mFont),
		mLeft(Src.mLeft),
		mTop(Src.mTop),
		mWidth(Src.mWidth),
		mHeight(Src.mHeight),
		mText(Src.mText),
		mScrolling(Src.mScrolling),
		mOffset(Src.mOffset),
		mDirection(Src.mDirection),
		mColorFg(Src.mColorFg),
		mColorBg(Src.mColorBg),
		mNextTime(Src.mNextTime)
{
}

cText2SkinMarquee::cText2SkinMarquee(cText2SkinScreen *Screen, int Left, int Top, int Width, 
                                     int Height, const std::string &Text, const cFont *Font, 
                                     tColor ColorFg, tColor ColorBg, int &UpdateIn)
{
	Set(Screen, Left, Top, Width, Height, Text, Font, ColorFg, ColorBg, UpdateIn);
}

void cText2SkinMarquee::Set(cText2SkinScreen *Screen, int Left, int Top, int Width, int Height,
                            const std::string &Text, const cFont *Font, tColor ColorFg, 
                            tColor ColorBg, int &UpdateIn)
{
	mScreen = Screen;
	mFont = Font;
	mLeft = Left;
	mTop = Top;
	mWidth = Width;
	mHeight = Height;
	mText = Text;
	mColorFg = ColorFg;
	mColorBg = ColorBg;
	mOffset = 0;
	mDirection = 1;
	mNextTime = 0;
	mScrolling = mFont->Width(mText.c_str()) > mWidth;
	DrawText(UpdateIn);
}

void cText2SkinMarquee::DrawText(int &mUpdateIn)
{
	uint now = time_ms();
	if (!mScrolling || (mNextTime > 0 && now < mNextTime)) {
		mScreen->DrawText(mLeft, mTop, mText.c_str() + mOffset, mColorFg, mColorBg, mFont, mWidth, 
						  mHeight);
		if (mScrolling) {
			uint updatein = mNextTime - now;
			if (mUpdateIn == 0 || (uint)mUpdateIn > updatein)
				mUpdateIn = updatein;
		}
		return;
	}
	
	int nextupdate = 250;
	if (mDirection > 0) {
		if (mFont->Width(mText.c_str() + mOffset) <= mWidth) {
			--mDirection;
			nextupdate = 1500;
		}
		else
			++mOffset;
	}
	else {
		if (mOffset <= 0) {
			++mDirection;
			nextupdate = 1500;
		}
		else
			--mOffset;
	}

	mNextTime = now + nextupdate;
		
	mScreen->DrawText(mLeft, mTop, mText.c_str() + mOffset, mColorFg, mColorBg, mFont, mWidth, 
	                  mHeight);

	if (mUpdateIn == 0 || mUpdateIn > nextupdate)
		mUpdateIn = nextupdate;
}
