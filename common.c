/*
 * $Id: common.c,v 1.8 2004/06/11 15:01:58 lordjaxom Exp $
 */

#include "data.h"
#include "common.h"
#include <vdr/plugin.h>

const string SectionNames[__SECTION_COUNT__] =
	{ "Skin", "ChannelSmall", "Channel", "Volume", "ReplayMode", "Replay", 
	  "Message", "Menu" };

const string ItemNames[__ITEM_COUNT__] = 
	{ "Unknown", "Skin", "Background", "Text", "Scrolltext", "Image", "Rectangle",
	  "Ellipse", "Slope", "Progress", "Logo", "Symbol", "MenuArea", "MenuItem" };

const string DisplayNames[__DISPLAY_COUNT__] = 
	{ "Always", "DateTimeF", "DateTime", "Date", "Time", "ChannelNumberName", 
	  "ChannelNumber", "ChannelName", "Language", "PresentDateTimeF", 
	  "PresentStartTime", "PresentDate", "PresentVPSTime", "PresentEndTime", 
	  "PresentDuration", "PresentVPS", "PresentRunning", "PresentTimer", 
	  "PresentTitle", "PresentShortText", "PresentDescription", 
	  "FollowingStartTime", "FollowingEndTime", "FollowingDuration", 
	  "FollowingTitle", "FollowingShortText", "Teletext", "Audio", "Dolby", 
	  "Encrypted", "Recording", "Radio", "VolumeCurrent", "VolumeTotal", "Mute", 
	  "ReplayTime", "ReplayDuration", "ReplayTitle", "ReplayPrompt", "Play", 
	  "Pause", "FastFwd", "FastRew", "SlowFwd", "SlowRew", "Message", 
	  "MessageStatus", "MessageInfo", "MessageWarning", "MessageError", 
	  "MenuTitle", "MenuRed", "MenuGreen", "MenuYellow", "MenuBlue", "MenuText", 
	  "MenuRecording", "MenuScrollUp", "MenuScrollDown", "MenuItems", 
		"MenuCurrent", "MenuGroups" };
	
const char *SkinPath(void) {
	return cPlugin::ConfigDirectory(PLUGIN_NAME_I18N);
}

const char *ChannelNumber(const cChannel *Channel, int Number) {
	static char buffer[256];
	buffer[0] = '\0';
  if (Channel) {
     if (!Channel->GroupSep())
        snprintf(buffer, sizeof(buffer), "%d%s", Channel->Number(), Number ? "-" : "");
     }
  else if (Number)
     snprintf(buffer, sizeof(buffer), "%d-", Number);
  return buffer;
}

const char *ChannelName(const cChannel *Channel, int Number) {
	static char buffer[256];
	buffer[0] = '\0';
  if (Channel) 
		 snprintf(buffer, sizeof(buffer), "%s", Channel->Name());
  else if (!Number)
     snprintf(buffer, sizeof(buffer), "%s", tr("*** Invalid Channel ***"));
  return buffer;
}

string ItemText(cText2SkinItem *Item, const string &Content) {
	string s;
	if (Item->Text() != "") {
		s = Item->Text();
		int pos;
		while ((pos = s.find('$')) != -1)
			s.replace(pos, 1, Content);
	} else
		s = Content;
	return s;
}

bool ParseVar(const char *Text, const char *Name, eSkinItem *Value) {
	string value;
	if (ParseVar(Text, Name, value)) {
		int i;
		for (i = 0; i < __ITEM_COUNT__; ++i) {
			if (ItemNames[i] == value) {
				*Value = (eSkinItem)i;
				return true;
			}
			if (i == __ITEM_COUNT__)
				esyslog("ERROR: text2skin: unknown item %s", value.c_str());
		}
	}
	return false;
}

bool ParseVar(const char *Text, const char *Name, eSkinDisplay *Value) {
	string value;
	if (ParseVar(Text, Name, value)) {
		int i;
		for (i = 0; i < __DISPLAY_COUNT__; ++i) {
			if (DisplayNames[i] == value) {
				*Value = (eSkinDisplay)i;
				return true;
			}
			if (i == __DISPLAY_COUNT__)
				esyslog("ERROR: text2skin: unknown display parameter %s", value.c_str());
		}
	}
	return false;
}

bool ParseVar(const char *Text, const char *Name, int *Value) {
	string value;
	if (ParseVar(Text, Name, value)) {
		*Value = atoi(value.c_str());
		return true;
	}
	return false;
}

bool ParseVar(const char *Text, const char *Name, string &Value){
	const char *ptr1, *ptr2;
	char *str;
	bool res = false;
	asprintf(&str, ",%s=", Name);
	if ((ptr1 = strstr(Text, str)) || (strncmp(ptr1 = Text, str + 1, strlen(str) - 1) == 0)) {
		if (ptr1 == Text)
			--ptr1;
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

bool ParseVar(const char *Text, const char *Name, tColor **Value) {
	string value;
	if (ParseVar(Text, Name, value) && value[0] == '#') {
		*Value = new tColor(strtoul(value.c_str() + 1, NULL, 16));
		return true;
	}
	return false;
}

bool ParseVar(const char *Text, const char *Name, eTextAlignment *Value) {
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

bool ParseVar(const char *Text, const char *Name, const cFont **Value) {
	string value;
	if (ParseVar(Text, Name, value)) {
		if      (value == "Sml") *Value = cFont::GetFont(fontSml);
		else if (value == "Fix") *Value = cFont::GetFont(fontFix);
		return true;
	}
	return false;
}

