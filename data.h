/*
 * $Id: data.h,v 1.18 2004/07/13 13:52:51 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_DATA_H
#define VDR_TEXT2SKIN_DATA_H

#include "common.h"
#include "file.h"
#include <vdr/tools.h>
#include <vdr/osd.h>
#include <vdr/config.h>

class cText2SkinItem {
private:
	eSkinItem       mItem;
	eSkinDisplay    mDisplay;
	POINT           mPos1;
	POINT           mPos2;
	int             mBpp;
	int             mArc;
	int             mAlpha;
	string          mFg;
	string          mBg;
	string          mSelected;
	string          mMark;
	string          mCurrent;
	const cFont    *mFont;
	string          mName;
	string          mVersion;
	string          mPath;
	string          mAltPath;
	string          mText;
	string          mType;
	string          mFormat;
	eTextAlignment  mAlign;
	eBaseCoordinate mBase;

protected:
	bool ParseItem(const char *Text);

public:
	cText2SkinItem(void);
	~cText2SkinItem();

	bool Parse(const char *Text);

	// writeables
	POINT          &Pos1(void)          { return mPos1; }
	POINT          &Pos2(void)          { return mPos2; }

	// readables
	eSkinItem       Item(void)    const { return mItem; }
	eSkinDisplay    Display(void) const { return mDisplay; }
	eBaseCoordinate Base(void)    const { return mBase; }
	const POINT    &Pos1(void)    const { return mPos1; }
	const POINT    &Pos2(void)    const { return mPos2; }
	int             Bpp(void)     const { return mBpp; }
	int             Arc(void)     const { return mArc; }
	int             Alpha(void)   const { return mAlpha; }
	const cFont    *Font(void)    const { return mFont; }
	const string   &Name(void)    const { return mName; }
	const string   &Version(void) const { return mVersion; }
	const string   &Path(void)    const { return mPath; }
	const string   &AltPath(void) const { return mAltPath; }
	const string   &Text(void)    const { return mText; }
	const string   &Type(void)    const { return mType; }
	const string   &Format(void)  const { return mFormat; }
	eTextAlignment  Align(void)   const { return mAlign; }

	// auto-conversion
	const POINT     Pos(void)     const;
	const SIZE      Size(void)    const;
	const tColor   *Fg(void)      const;
	const tColor   *Bg(void)      const;
	const tColor   *Selected(void)const;
	const tColor   *Mark(void)    const;
	const tColor   *Current(void) const;
};

class cText2SkinData: public cText2SkinFile {
public:
	typedef vector<cText2SkinItem*> tSection;
	typedef tSection::iterator      tIterator;

private:
	eSkinSection mCurrentSection;
	tSection mSections[__SECTION_COUNT__];

protected:
	virtual bool Parse(const char *Text);

public:
	cText2SkinData(const char *Skin);
	virtual ~cText2SkinData();

	tIterator First(eSkinSection Section) { return mSections[Section].begin(); }
	tIterator Last(eSkinSection Section) { return mSections[Section].end(); }

	cText2SkinItem *Get(eSkinSection Section, eSkinItem Item);
};

#endif // VDR_TEXT2SKIN_DATA_H
