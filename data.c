/*
 * $Id: data.c,v 1.13 2004/06/05 01:39:36 lordjaxom Exp $
 */

#include "data.h"
#include "common.h"

static string SectionNames[__SECTION_COUNT__] =
	{ "Skin", "ChannelSmall", "Channel", "Volume", "ReplayMode", "Replay", 
	  "Message", "Menu" };

static string ItemNames[__ITEM_COUNT__] = 
	{ "Unknown", "Skin", "Background", "Text", "Image", "Rectangle", "Ellipse",
	  "Slope", "DateTime", "Date", "Time", "ChannelLogo", "ChannelNumberName",
	  "ChannelNumber", "ChannelName", "Language", "Timebar", "PresentTime",
	  "PresentTitle", "PresentShortText", "FollowingTime", "FollowingTitle",
	  "FollowingShortText", "SymbolTeletext", "SymbolAudio", "SymbolDolby",
	  "SymbolEncrypted", "SymbolRecording", "SymbolRadio", "Volumebar", "Mute",
	  "Replaybar", "ReplayTitle", "ReplayCurrent", "ReplayTotal", "ReplayJump",
	  "SymbolPlay", "SymbolPause", "SymbolFastFwd", "SymbolFastRew",
	  "SymbolSlowFwd", "SymbolSlowRew", "MessageStatus", "MessageInfo",
	  "MessageWarning", "MessageError", "MenuArea", "MenuItem", "MenuCurrent",
	  "MenuTitle", "MenuRed", "MenuGreen", "MenuYellow", "MenuBlue", "MenuText",
		"SymbolScrollUp", "SymbolScrollDown", "MenuEventTitle", 
		"MenuEventShortText", "MenuEventDescription", "MenuEventTime", 
		"SymbolEventRunning", "SymbolEventTimer", "SymbolEventVPS",
		"MenuRecording" };
	
cText2SkinItem::cText2SkinItem(void) {
	mItem    = itemUnknown;
	mPos.x   = -1;
	mPos.y   = -1;
	mSize.w  = 0;
	mSize.h  = 0;
	mBpp     = 4;
	mArc     = 0;
	mFont    = cFont::GetFont(fontOsd);
	mAlign   = taDefault;
}

cText2SkinItem::~cText2SkinItem() {
}

bool cText2SkinItem::Parse(const char *Text) {
	char *text = strdup(Text);
	char *ptr = text;

	// check if this is an item
	string item;
	if (ParseVar(ptr, "Item", item)) {
		if (item == "Skin") { // the Skin item
			if (ParseVar(ptr, "name", mName) && ParseVar(ptr, "version", mVersion))
				mItem = itemSkin;
			else
				esyslog("ERROR: text2skin: Skin doesn't contain Item=Skin keyphrase");
		} else {
			int i;
			// valid items begin at index two
			for (i = 2; i < __ITEM_COUNT__; ++i) {
				if (ItemNames[i] == item) {
					mItem = (eSkinItem)i;
					break;
				}
			}
			if (i == __ITEM_COUNT__)
				esyslog("ERROR: text2skin: %s is not a valid theme item\n", item.c_str());
		}
		
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
