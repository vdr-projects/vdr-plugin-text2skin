/*
 * $Id: common.c,v 1.4 2004/12/14 20:02:31 lordjaxom Exp $
 */

#include "common.h"
#include <vdr/plugin.h>

const std::string &SkinPath(void) {
	// should never change
	static std::string path = cPlugin::ConfigDirectory(PLUGIN_NAME_I18N);
	return path;
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

#if VDRVERSNUM < 10315
	char *ptr;
	if ((ptr = strchr(buffer, ',')) != NULL
			|| (ptr = strchr(buffer, ';')) != NULL)
		*ptr = '\0';
#endif
	return buffer;
}

const char *ChannelShortName(const cChannel *Channel, int Number) {
	static char buffer[256];
	buffer[0] = '\0';
	if (Channel) 
#if VDRVERSNUM < 10315
		snprintf(buffer, sizeof(buffer), "%s", Channel->Name());
#else
		snprintf(buffer, sizeof(buffer), "%s", Channel->ShortName(true));
#endif
	else if (!Number)
		snprintf(buffer, sizeof(buffer), "%s", tr("*** Invalid Channel ***"));

#if VDRVERSNUM < 10315
	char *ptr;
	if ((ptr = strchr(buffer, ',')) != NULL) {
		char *start = ptr + 1;
		if ((ptr = strchr(start, ';')) != NULL)
			*ptr = '\0';
		return start;
	} else if ((ptr = strchr(buffer, ';')) != NULL)
		*ptr = '\0';
#endif
	
	return buffer;
}
/*
const char *ChannelBouquet(const cChannel *Channel, int Number) {
	static char buffer[256];
	buffer[0] = '\0';
	if (Channel) 
#if VDRVERSNUM < 10315
		snprintf(buffer, sizeof(buffer), "%s", Channel->Name());
#else
		snprintf(buffer, sizeof(buffer), "%s", Channel->Provider());
#endif
	else if (!Number)
		snprintf(buffer, sizeof(buffer), "%s", tr("*** Invalid Channel ***"));

#if VDRVERSNUM < 10315
	char *ptr;
	if ((ptr = strchr(buffer, ';')) != NULL)
		return ptr + 1;
	else
		return "";
#else
	return buffer;
#endif
}
*/
cxType TimeType(time_t Time, const std::string &Format) {
	static char result[1000];
	struct tm tm_r, *tm;
	tm = localtime_r(&Time, &tm_r);

	if (Time > 0) {
		if (Format.length() > 0) {
			strftime(result, sizeof(result), Format.c_str(), tm);
			return result;
		} else
			return Time;
	}
	return "";
}

bool ParseVar(const char *Text, const char *Name, std::string &Value) {
	const char *ptr1, *ptr2;
	char *str;
	bool res = false;
	asprintf(&str, ",%s=", Name);
	if ((ptr1 = strstr(Text, str)) || (strncmp(ptr1 = Text, str + 1, strlen(str) - 1) == 0)) {
		if (ptr1 == Text)
			--ptr1;
		ptr1 += strlen(str);
		if ((ptr2 = strchr(ptr1, ',')) || (ptr2 = strchr(ptr1, ';'))) {
			int pos;
			Value = ptr1;
			Value.erase(ptr2 - ptr1);
			if (Value[0] == '"')
				SkipQuotes(Value);
			while ((pos = Value.find('$')) != -1)
				Value.replace(pos, 1, "{*}");
			res = true;
		}
	}
	free(str);
	return res;
}

bool ParseVar(const char *Text, const char *Name, tColor *Value) {
	std::string value;
	if (ParseVar(Text, Name, value) && value[0] == '#') {
		*Value = (tColor)strtoul(value.c_str() + 1, NULL, 16);
		return true;
	}
	return false;
}

void SkipQuotes(std::string &Value) {
	char quote = Value[0];
	int i;
	Value.erase(0, 1);
	for (i = 0; i < (int)Value.length() && Value[i] != quote; ++i) {
		if (Value[i] == '\\')
			Value.erase(i, 1);
	}
	if (Value[i] == quote)
		Value.erase(i, 1);
	else
		esyslog("ERROR: text2skin: missing closing %c", quote);
}
