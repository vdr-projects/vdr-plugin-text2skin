/*
 * $Id: data.c,v 1.19 2004/06/16 18:46:50 lordjaxom Exp $
 */

#include "data.h"
#include "common.h"

cText2SkinItem::cText2SkinItem(void) {
	mItem    = itemUnknown;
	mDisplay = displayAlways;
	mPos.x   = 0;
	mPos.y   = 0;
	mSize.w  = 0;
	mSize.h  = 0;
	mBpp     = 4;
	mArc     = 0;
	mAlpha   = 0;
	mFont    = cFont::GetFont(fontOsd);
	mAlign   = taDefault;
}

cText2SkinItem::~cText2SkinItem() {
}

bool cText2SkinItem::Parse(const char *Text) {
	const char *text = Text;
	const char *ptr = text;
	bool res = false;

	// check if this is an item
	if (ParseVar(ptr, "Item", &mItem)) {
		if (mItem == itemSkin && (!ParseVar(ptr, "name", mName) || !ParseVar(ptr, "version", mVersion)))
			esyslog("ERROR: text2skin: Item=Skin is missing the name and/or version parameter(s)");
		else if (mItem != itemUnknown) {
			ParseItem(ptr);
			res = true;
		}
	} else 
		esyslog("ERROR: text2skin: unknown item in skin");
	return res;
}

bool cText2SkinItem::ParseItem(const char *Text) {
	ParseVar(Text, "display", &mDisplay);
	ParseVar(Text, "x",       &mPos.x);
	ParseVar(Text, "y",       &mPos.y);
	ParseVar(Text, "width",   &mSize.w);
	ParseVar(Text, "height",  &mSize.h);
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
