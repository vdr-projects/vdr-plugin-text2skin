/*
 * $Id: data.h,v 1.16 2004/06/12 18:00:05 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_DATA_H
#define VDR_TEXT2SKIN_DATA_H

#include "common.h"
#include "file.h"
#include <vdr/tools.h>
#include <vdr/osd.h>
#include <vdr/config.h>

class cText2SkinItem {
	friend class cText2SkinRender;

private:
	eSkinItem       mItem;
	eSkinDisplay    mDisplay;
	POINT           mPos;
	SIZE            mSize;
	int             mBpp;
	int             mArc;
	int             mAlpha;
	string          mFg;
	string          mBg;
	const cFont    *mFont;
	string          mName;
	string          mVersion;
	string          mPath;
	string          mAltPath;
	string          mText;
	string          mType;
	string          mFormat;
	eTextAlignment  mAlign;

protected:
	bool ParseItem(const char *Text);

public:
	cText2SkinItem(void);
	~cText2SkinItem();

	bool Parse(const char *Text);

	eSkinItem       Item(void)    const { return mItem; }
	eSkinDisplay    Display(void) const { return mDisplay; }
	const POINT    &Pos(void)     const { return mPos; }
	const SIZE     &Size(void)    const { return mSize; }
	int             Bpp(void)     const { return mBpp; }
	int             Arc(void)     const { return mArc; }
	int             Alpha(void)   const { return mAlpha; }
	const string   &Fg(void)      const { return mFg; }
	const string   &Bg(void)      const { return mBg; }
	const cFont    *Font(void)    const { return mFont; }
	const string   &Name(void)    const { return mName; }
	const string   &Version(void) const { return mVersion; }
	const string   &Path(void)    const { return mPath; }
	const string   &AltPath(void) const { return mAltPath; }
	const string   &Text(void)    const { return mText; }
	const string   &Type(void)    const { return mType; }
	const string   &Format(void)  const { return mFormat; }
	eTextAlignment  Align(void)   const { return mAlign; }
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
