/*
 * $Id: data.c,v 1.12 2004/06/02 20:43:05 lordjaxom Exp $
 */

#include "data.h"
#include "common.h"

static string SectionNames[__SECTION_COUNT__] =
	{ "Skin", "ChannelSmall", "Channel", "Volume",
	  "ReplayMode", "Replay", "Message", "Menu" };
	
cText2SkinItem::cText2SkinItem(void) {
	mItem    = itemUnknown;
	mPos.x   = -1;
	mPos.y   = -1;
	mSize.w  = 0;
	mSize.h  = 0;
	mBpp     = 4;
	mArc     = 0;
	//mFg      = NULL;
	//mBg      = NULL;
	mFont    = cFont::GetFont(fontOsd);
	mAlign   = taDefault;
}

cText2SkinItem::~cText2SkinItem() {
	//delete mBg;
	//delete mFg;
}

bool cText2SkinItem::Parse(const char *Text) {
	char *text = strdup(Text);
	char *ptr = text;

	/*ptr = text + strlen(text) - 1;
	for (; ptr >= text && *ptr == ' '; --ptr)
		*ptr = '\0';
	ptr = skipspace(text);
	if (*ptr == '\0' || *ptr == '#') // empty line or comment
		return true;
	else if (*ptr == '[' && ptr[strlen(ptr)-1] == ']') { // section
		++ptr;
		ptr[strlen(ptr)-1] = '\0';
		if      (strcmp(ptr, "Channel")      == 0)  mParseSection = sectionChannel;
		else if (strcmp(ptr, "ChannelSmall") == 0)  mParseSection = sectionChannelSmall;
		else if (strcmp(ptr, "Menu")         == 0)  mParseSection = sectionMenu;
		else if (strcmp(ptr, "Volume")       == 0)  mParseSection = sectionVolume;
		else if (strcmp(ptr, "ReplayMode")   == 0)  mParseSection = sectionReplayMode;
		else if (strcmp(ptr, "Replay")       == 0)  mParseSection = sectionReplay;
		else if (strcmp(ptr, "Message")      == 0)  mParseSection = sectionMessage;
		return true;
	}*/

	// check if this is an item
	string item;
	if (ParseVar(ptr, "Item", item)) {
		if (item == "Skin") { // the Skin item
			if (ParseVar(ptr, "name", mName) && ParseVar(ptr, "version", mVersion))
				mItem = itemSkin;
			else
				esyslog("ERROR: text2skin: Skin doesn't contain Item=Skin keyphrase");
		}
		else if (item == "Background")         mItem = itemBackground;
		else if (item == "ChannelLogo")        mItem = itemChannelLogo;
		else if (item == "Language")           mItem = itemLanguage;
		else if (item == "Text")               mItem = itemText;
		else if (item == "Image")              mItem = itemImage;
		else if (item == "DateTime")           mItem = itemDateTime;
		else if (item == "Date")               mItem = itemDate;
		else if (item == "Time")               mItem = itemTime;
		else if (item == "ChannelNumberName")  mItem = itemChannelNumberName;
		else if (item == "ChannelNumber")      mItem = itemChannelNumber;
		else if (item == "ChannelName")        mItem = itemChannelName;
		else if (item == "Rectangle")          mItem = itemRectangle;
		else if (item == "Ellipse")            mItem = itemEllipse;
		else if (item == "Slope")              mItem = itemSlope;
		else if (item == "Timebar")            mItem = itemTimebar;
		else if (item == "PresentTime")        mItem = itemPresentTime;
		else if (item == "PresentTitle")       mItem = itemPresentTitle;
		else if (item == "PresentShortText")   mItem = itemPresentShortText;
		else if (item == "FollowingTime")      mItem = itemFollowingTime;
		else if (item == "FollowingTitle")     mItem = itemFollowingTitle;
		else if (item == "FollowingShortText") mItem = itemFollowingShortText;
		else if (item == "SymbolTeletext")     mItem = itemSymbolTeletext;
		else if (item == "SymbolAudio")        mItem = itemSymbolAudio;
		else if (item == "SymbolDolby")        mItem = itemSymbolDolby;
		else if (item == "SymbolEncrypted")    mItem = itemSymbolEncrypted;
		else if (item == "SymbolRecording")    mItem = itemSymbolRecording;
		else if (item == "SymbolRadio")        mItem = itemSymbolRadio;
		else if (item == "Volumebar")          mItem = itemVolumebar;
		else if (item == "Mute")               mItem = itemMute;
		else if (item == "Replaybar")          mItem = itemReplaybar;
		else if (item == "ReplayTitle")        mItem = itemReplayTitle;
		else if (item == "ReplayCurrent")      mItem = itemReplayCurrent;
		else if (item == "ReplayTotal")        mItem = itemReplayTotal;
		else if (item == "ReplayJump")         mItem = itemReplayJump;
		else if (item == "SymbolPlay")         mItem = itemSymbolPlay;
		else if (item == "SymbolPause")        mItem = itemSymbolPause;
		else if (item == "SymbolFastFwd")      mItem = itemSymbolFastFwd;
		else if (item == "SymbolFastRew")      mItem = itemSymbolFastRew;
		else if (item == "SymbolSlowFwd")      mItem = itemSymbolSlowFwd;
		else if (item == "SymbolFastFwd")      mItem = itemSymbolFastFwd;
		else if (item == "MessageStatus")      mItem = itemMessageStatus;
		else if (item == "MessageInfo")        mItem = itemMessageInfo;
		else if (item == "MessageWarning")     mItem = itemMessageWarning;
		else if (item == "MessageError")       mItem = itemMessageError;
		else if (item == "MenuArea")           mItem = itemMenuArea;
		else if (item == "MenuItem")           mItem = itemMenuItem;
		else if (item == "MenuCurrent")        mItem = itemMenuCurrent;
		else if (item == "MenuTitle")          mItem = itemMenuTitle;
		else if (item == "MenuRed")            mItem = itemMenuRed;
		else if (item == "MenuGreen")          mItem = itemMenuGreen;
		else if (item == "MenuYellow")         mItem = itemMenuYellow;
		else if (item == "MenuBlue")           mItem = itemMenuBlue;
		else
			esyslog("ERROR: text2skin: %s is not a valid theme item\n", item.c_str());

		if (mItem != itemUnknown) {
			if (mItem != itemSkin)
				ParseItem(ptr);
			return true;
		}
	} else 
		esyslog("ERROR: text2skin: Missing Item= in Skin");

	// fall through
	return false;
}

bool cText2SkinItem::ParseItem(const char *Text) {
	ParseVar(Text, "x",       &mPos.x);
	ParseVar(Text, "y",       &mPos.y);
	ParseVar(Text, "width",   &mSize.w);
	ParseVar(Text, "height",  &mSize.h);
	ParseVar(Text, "bpp",     &mBpp);
	ParseVar(Text, "arc",     &mArc);
	ParseVar(Text, "fg",       mFg);
	ParseVar(Text, "bg",       mBg);
	ParseVar(Text, "font",    &mFont);
	ParseVar(Text, "path",     mPath);
	ParseVar(Text, "altpath",  mAltPath);
	ParseVar(Text, "text",     mText);
	ParseVar(Text, "type",     mType);
	ParseVar(Text, "align",   &mAlign);
	return true;
}

// --- cText2SkinData ---------------------------------------------------------
	
cText2SkinData::cText2SkinData(const char *Skin): cText2SkinFile(Skin) {
	mCurrentSection = sectionSkin;
}

cText2SkinData::~cText2SkinData() {
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
