/*
 * $Id: common.c,v 1.7 2004/06/05 16:52:44 lordjaxom Exp $
 */

#include "data.h"
#include "common.h"
#include <vdr/plugin.h>

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

bool ParseVar(const char *Text, const char *Name, int *Value) {
	string value;
	if (ParseVar(Text, Name, value)) {
		*Value = atoi(value.c_str());
		return true;
	}
	return false;
}

bool ParseVar(const char *Text, const char *Name, string &Value){
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

