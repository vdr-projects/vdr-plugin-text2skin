/*
 * $Id: data.c,v 1.11 2004/06/01 21:02:38 lordjaxom Exp $
 */

#include "data.h"

eSkinSection cText2SkinItem::mParseSection = sectionUnknown;
	
cText2SkinItem::cText2SkinItem(void) {
	mSection = sectionUnknown;
	mItem    = itemUnknown;
	mPos.x   = -1;
	mPos.y   = -1;
	mSize.w  = 0;
	mSize.h  = 0;
	mBpp     = 4;
	mArc     = 0;
	mFg      = NULL;
	mBg      = NULL;
	mFont    = cFont::GetFont(fontOsd);
	mAlign   = taDefault;
}

cText2SkinItem::~cText2SkinItem() {
	delete mBg;
	delete mFg;
}

bool cText2SkinItem::Parse(const char *Text) {
	char *text = strdup(Text);
	char *ptr = text;

	ptr = text + strlen(text) - 1;
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
	}

	// check if this is an item
	string item;
	if (ParseVar(ptr, "Item", item)) {
		mSection = mParseSection;
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
	ParseVar(Text, "fg",      &mFg);
	ParseVar(Text, "bg",      &mBg);
	ParseVar(Text, "font",    &mFont);
	ParseVar(Text, "path",     mPath);
	ParseVar(Text, "altpath",  mAltPath);
	ParseVar(Text, "text",     mText);
	ParseVar(Text, "type",     mType);
	ParseVar(Text, "align",   &mAlign);
	return true;
}

bool cText2SkinItem::ParseVar(const char *Text, const char *Name, int *Value) {
	string value;
	if (ParseVar(Text, Name, value)) {
		*Value = atoi(value.c_str());
		return true;
	}
	return false;
}

bool cText2SkinItem::ParseVar(const char *Text, const char *Name, string &Value){
	char *ptr1, *ptr2;
	char *str;
	bool res = false;
	asprintf(&str, "%s=", Name);
	if ((ptr1 = strstr(Text, str))) {
		ptr1 += strlen(str);
		if ((ptr2 = strchr(ptr1, ',')) || (ptr2 = strchr(ptr1, ';'))) {
			Value = ptr1;
			Value.erase(ptr2 - ptr1);
			res = true;
		}
	}
	free(str);
	return res;
}

bool cText2SkinItem::ParseVar(const char *Text, const char *Name, tColor **Value) {
	string value;
	if (ParseVar(Text, Name, value) && value[0] == '#') {
		*Value = new tColor(strtoul(value.c_str() + 1, NULL, 16));
		return true;
	}
	return false;
}

bool cText2SkinItem::ParseVar(const char *Text, const char *Name, eTextAlignment *Value) {
	string value;
	if (ParseVar(Text, Name, value)) {
		int v = atoi(value.c_str());
		if (v == 0)
			*Value = (eTextAlignment)(taTop|taLeft);
		else if (v == 1)
			*Value = (eTextAlignment)(taTop|taCenter);
		else if (v == 2)
			*Value = (eTextAlignment)(taTop|taRight);
		return true;
	}
	return false;
}

bool cText2SkinItem::ParseVar(const char *Text, const char *Name, const cFont **Value) {
	string value;
	if (ParseVar(Text, Name, value)) {
		if      (value == "Sml") *Value = cFont::GetFont(fontSml);
		else if (value == "Fix") *Value = cFont::GetFont(fontFix);
		return true;
	}
	return false;
}
	
cText2SkinData::cText2SkinData(const char *Skin) {
	mSkin = strdup(Skin);
}

cText2SkinData::~cText2SkinData() {
	free(mSkin);
}

cText2SkinItem *cText2SkinData::Get(eSkinItem Item) {
	for (cText2SkinItem *it = First(); it; it = Next(it)) {
		if (it->Item() == Item)
			return it;
	}
	return NULL;
}
