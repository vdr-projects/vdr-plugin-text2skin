/*
 *  $Id: marquee.c,v 1.3 2004/12/28 01:24:35 lordjaxom Exp $
 */

#include "marquee.h"
#include "screen.h"
#include <vdr/tools.h>

/*
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
                                     tColor ColorFg, tColor ColorBg, uint &UpdateIn)
{
	Set(Screen, Left, Top, Width, Height, Text, Font, ColorFg, ColorBg, UpdateIn);
}

void cText2SkinMarquee::Set(cText2SkinScreen *Screen, int Left, int Top, int Width, int Height,
                            const std::string &Text, const cFont *Font, tColor ColorFg, 
                            tColor ColorBg, uint &UpdateIn)
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
*/

void cText2SkinMarquee::DrawText(cText2SkinScreen *Screen, int Left, int Top, int Width, int Height,
                                 const std::string &Text, const cFont *Font, tColor ColorFg, 
                                 tColor ColorBg, uint Delay, int &Offset, int &Direction,
                                 uint &NextTime)
{
	uint now = time_ms();
	bool scrolling = Font->Width(Text.c_str()) > Width;
	
	if (NextTime == 0)
		NextTime = now + 1500;
	else if (now >= NextTime) {
		uint nextin = Delay;
		if (Direction > 0) {
			if (Font->Width(Text.c_str() + Offset) <= Width) {
				--Direction;
				nextin = 1500;
			} else
				++Offset;
		}
		else {
			if (Offset <= 0) {
				++Direction;
				nextin = 1500;
			} else
				--Offset;
		}
		NextTime = now + nextin;
	}

	if (!scrolling)
		NextTime = 0;

	/*
	if (scrolling) {
		uint updatein = NextTime - now;
		if (UpdateIn == 0 || updatein < UpdateIn)
			UpdateIn = updatein;
	}
	*/
		
	Screen->DrawText(Left, Top, Text.c_str() + Offset, ColorFg, ColorBg, Font, Width, Height);
}

/*
void cText2SkinMarquee::DrawText(uint &UpdateIn)
{
	uint now = time_ms();
	if (mNextTime == 0)
		mNextTime = now + 1500;
	else if (now >= mNextTime) {
		uint nextin = 250;
		if (mDirection > 0) {
			if (mFont->Width(mText.c_str() + mOffset) <= mWidth) {
				--mDirection;
				nextin = 1500;
			}
			else
				++mOffset;
		}
		else {
			if (mOffset <= 0) {
				++mDirection;
				nextin = 1500;
			}
			else
				--mOffset;
		}
		mNextTime = now + nextin;
	}

	if (mScrolling) {
		uint updatein = mNextTime - now;
		if (UpdateIn == 0 || updatein < UpdateIn)
			UpdateIn = updatein;
	}
		
	mScreen->DrawText(mLeft, mTop, mText.c_str() + mOffset, mColorFg, mColorBg, mFont, mWidth, 
					  mHeight);
}
*/
