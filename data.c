/*
 * $Id: data.c,v 1.1.1.1 2004/05/23 00:08:03 lordjaxom Exp $
 */

#include "data.h"

eSkinSection cText2SkinItem::mParseSection = sectionUnknown;
	
cText2SkinItem::cText2SkinItem(void) {
	mSection = sectionUnknown;
	mItem    = itemUnknown;
	mX       = -1;
	mY       = -1;
	mWidth   = 0;
	mHeight  = 0;
	mBpp     = 4;
	mFg      = NULL;
	mBg      = NULL;
	mName    = NULL;
	mVersion = NULL;
	mFont    = NULL;
	mPath    = NULL;
	mAltPath = NULL;
	mText    = NULL;
	mAlign   = taDefault;
}

cText2SkinItem::~cText2SkinItem() {
	free(mText);
	free(mPath);
	free(mAltPath);
	free(mFont);
	free(mVersion);
	free(mName);
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
		if      (strcmp(ptr, "Channel")    == 0)  mParseSection = sectionChannel;
		else if (strcmp(ptr, "Menu")       == 0)  mParseSection = sectionMenu;
		else if (strcmp(ptr, "Volume")     == 0)  mParseSection = sectionVolume;
		else if (strcmp(ptr, "ReplayMode") == 0)  mParseSection = sectionReplayMode;
		else if (strcmp(ptr, "Replay")     == 0)  mParseSection = sectionReplay;
		else if (strcmp(ptr, "Message")    == 0)  mParseSection = sectionMessage;
		return true;
	}

	// check if this is an item
	char *item;
	if (ParseVar(ptr, "Item", &item)) {
		mSection = mParseSection;
		if (strcmp(item, "Skin") == 0) { // the Skin item
			if (ParseVar(ptr, "name", &mName) && ParseVar(ptr, "version", &mVersion))
				mItem = itemSkin;
			else
				esyslog("ERROR: text2skin: Skin doesn't contain Item=Skin keyphrase");
		}
		else if (strcmp(item, "Background")         == 0) mItem = itemBackground;
		else if (strcmp(item, "Logo")               == 0) mItem = itemLogo;
		else if (strcmp(item, "Text")               == 0) mItem = itemText;
		else if (strcmp(item, "DateTime")           == 0) mItem = itemDateTime;
		else if (strcmp(item, "Date")               == 0) mItem = itemDate;
		else if (strcmp(item, "Time")               == 0) mItem = itemTime;
		else if (strcmp(item, "ChannelNumberName")  == 0) mItem = itemChannelNumberName;
		else if (strcmp(item, "ChannelNumber")      == 0) mItem = itemChannelNumber;
		else if (strcmp(item, "ChannelName")        == 0) mItem = itemChannelName;
		else if (strcmp(item, "Rectangle")          == 0) mItem = itemRectangle;
		else if (strcmp(item, "Ellipse")            == 0) mItem = itemEllipse;
		else if (strcmp(item, "Timebar")            == 0) mItem = itemTimebar;
		else if (strcmp(item, "PresentTime")        == 0) mItem = itemPresentTime;
		else if (strcmp(item, "PresentTitle")       == 0) mItem = itemPresentTitle;
		else if (strcmp(item, "PresentShortText")   == 0) mItem = itemPresentShortText;
		else if (strcmp(item, "FollowingTime")      == 0) mItem = itemFollowingTime;
		else if (strcmp(item, "FollowingTitle")     == 0) mItem = itemFollowingTitle;
		else if (strcmp(item, "FollowingShortText") == 0) mItem = itemFollowingShortText;
		else if (strcmp(item, "SymbolTeletext")     == 0) mItem = itemSymbolTeletext;
		else if (strcmp(item, "SymbolAudio")        == 0) mItem = itemSymbolAudio;
		else if (strcmp(item, "SymbolDolby")        == 0) mItem = itemSymbolDolby;
		else if (strcmp(item, "SymbolEncrypted")    == 0) mItem = itemSymbolEncrypted;
		else if (strcmp(item, "Volumebar")          == 0) mItem = itemVolumebar;
		else if (strcmp(item, "Mute")               == 0) mItem = itemMute;
		else if (strcmp(item, "Progressbar")        == 0) mItem = itemProgressbar;
		else if (strcmp(item, "MenuArea")           == 0) mItem = itemMenuArea;
		else if (strcmp(item, "MenuItem")           == 0) mItem = itemMenuItem;
		else if (strcmp(item, "MenuCurrent")        == 0) mItem = itemMenuCurrent;
		else
			esyslog("ERROR: text2skin: %s is not a valid theme item\n", item);

		free(item);

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
	ParseVar(Text, "x",       &mX);
	ParseVar(Text, "y",       &mY);
	ParseVar(Text, "width",   &mWidth);
	ParseVar(Text, "height",  &mHeight);
	ParseVar(Text, "bpp",     &mBpp);
	ParseVar(Text, "fg",      &mFg);
	ParseVar(Text, "bg",      &mBg);
	ParseVar(Text, "font",    &mFont);
	ParseVar(Text, "path",    &mPath);
	ParseVar(Text, "altpath", &mAltPath);
	ParseVar(Text, "text",    &mText);
	ParseVar(Text, "align",   &mAlign);
	return true;
}

bool cText2SkinItem::ParseVar(const char *Text, const char *Name, int *Value) {
	char *value;
	if (ParseVar(Text, Name, &value)) {
		*Value = atoi(value);
		return true;
	}
	return false;
}

bool cText2SkinItem::ParseVar(const char *Text, const char *Name, char **Value){
	char *ptr1, *ptr2;
	char *str;
	asprintf(&str, "%s=", Name);
	if ((ptr1 = strstr(Text, str))) {
		ptr1 += strlen(str);
		if ((ptr2 = strchr(ptr1, ',')) || (ptr2 = strchr(ptr1, ';'))) {
			asprintf(Value, "%.*s", ptr2 - ptr1, ptr1);
			free(str);
			return true;
		}
	}
	free(str);
	return false;
}

bool cText2SkinItem::ParseVar(const char *Text, const char *Name, tColor **Value) {
	char *value;
	if (ParseVar(Text, Name, &value) && *value == '#') {
		*Value = new tColor(strtoul(value + 1, NULL, 16));
		return true;
	}
	return false;
}

bool cText2SkinItem::ParseVar(const char *Text, const char *Name, eTextAlignment *Value) {
	char *value;
	if (ParseVar(Text, Name, &value)) {
		int v = atoi(value);
		free(value);
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
