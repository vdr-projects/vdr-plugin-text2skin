/*
 * $Id: data.c,v 1.20 2004/06/22 16:48:03 lordjaxom Exp $
 */

#include "data.h"
#include "common.h"
#include "render.h"

cText2SkinItem::cText2SkinItem(void) {
	mItem    = itemUnknown;
	mDisplay = displayAlways;
	mBpp     = 4;
	mArc     = 0;
	mAlpha   = 0;
	mFont    = cFont::GetFont(fontOsd);
	mAlign   = taDefault;
	mBase    = baseRelative;
}

cText2SkinItem::~cText2SkinItem() {
}

bool cText2SkinItem::Parse(const char *Text) {
	const char *text = Text;
	const char *ptr = text;
	bool res = false;

	// check if this is an item
	if (ParseVar(ptr, "Item", &mItem) && mItem != itemUnknown) {
		ParseItem(ptr);

		if (mItem == itemSkin && (mName == "" || mVersion == ""))
			esyslog("ERROR: text2skin: Item=Skin is missing the name and/or version parameter(s)");
		else
			res = true;
	} else 
		esyslog("ERROR: text2skin: unknown item in skin");
	return res;
}

bool cText2SkinItem::ParseItem(const char *Text) {
	ParseVar(Text, "name",     mName);
	ParseVar(Text, "version",  mVersion);
	ParseVar(Text, "base",    &mBase);
	ParseVar(Text, "display", &mDisplay);
	ParseVar(Text, "x1",      &mPos1.x);
	ParseVar(Text, "y1",      &mPos1.y);
	ParseVar(Text, "x2",      &mPos2.x);
	ParseVar(Text, "y2",      &mPos2.y);
	ParseVar(Text, "bpp",     &mBpp);
	ParseVar(Text, "arc",     &mArc);
	ParseVar(Text, "alpha",   &mAlpha);
	ParseVar(Text, "fg",       mFg);
	ParseVar(Text, "bg",       mBg);
	ParseVar(Text, "font",    &mFont);
	ParseVar(Text, "path",     mPath);
	ParseVar(Text, "altpath",  mAltPath);
	ParseVar(Text, "text",     mText);
	ParseVar(Text, "type",     mType);
	ParseVar(Text, "format",   mFormat);
	ParseVar(Text, "align",   &mAlign);
	return true;
}

const POINT cText2SkinItem::Pos(void) const {
	return cText2SkinRender::Transform(mPos1);
}

const SIZE cText2SkinItem::Size(void) const {
	POINT p1 = cText2SkinRender::Transform(mPos1);
	POINT p2 = cText2SkinRender::Transform(mPos2);
	return SIZE(p2.x - p1.x + 1, p2.y - p1.y + 1);
}

// --- cText2SkinData ---------------------------------------------------------
	
cText2SkinData::cText2SkinData(const char *Skin): cText2SkinFile(Skin) {
	mCurrentSection = sectionSkin;
}

cText2SkinData::~cText2SkinData() {
	for (int i = 0; i < __SECTION_COUNT__; ++i) {
		for (int j = 0; j < (int)mSections[i].size(); ++j)
			delete mSections[i][j];
		mSections[i].clear();
	}
}

bool cText2SkinData::Parse(const char *Text) {
	int l = strlen(Text);
	bool result = false;
	if (l) {
		if (Text[0] == '#') // comment
			result = true;
		else if (Text[0] == '[' && Text[l - 1] == ']') { // section
			char *s;
			int i;
			asprintf(&s, "%.*s", l - 2, Text + 1);
			for (i = 0; i < __SECTION_COUNT__; ++i) {
				if (SectionNames[i] == s) {
					mCurrentSection = (eSkinSection)i;
					result = true;
					break;
				}
			}
			if (i == __SECTION_COUNT__)
				esyslog("ERROR: text2skin: Unknown section %s", s);
			free(s);
		} else {
			cText2SkinItem *item = new cText2SkinItem;
			if (item->Parse(Text)) {
				mSections[mCurrentSection].push_back(item);
				result = true;
			} else
				delete item;
		}
	}
	return result;
}

cText2SkinItem *cText2SkinData::Get(eSkinSection Section, eSkinItem Item) {
	vector<cText2SkinItem*>::iterator it = mSections[Section].begin();
	for (; it != mSections[Section].end(); ++it) {
		if ((*it)->Item() == Item)
			return (*it);
	}
	return NULL;
}
