/*
 *  $Id: string.c,v 1.3 2004/12/21 20:26:25 lordjaxom Exp $
 */

#include "xml/string.h"
#include "render.h"
#include <vdr/tools.h>

static const char *Tokens[__COUNT_TOKEN__] = {
	"DateTime",

	// Channel Display
	"ChannelNumber", "ChannelName", "ChannelShortName", "ChannelBouquet", "ChannelPortal", 
	"ChannelSource", "PresentStartDateTime", "PresentVPSDateTime", "PresentEndDateTime", 
	"PresentDuration", "PresentProgress", "PresentRemaining",
	"PresentTitle", "PresentShortText", "PresentDescription", "FollowingStartDateTime", 
	"FollowingVPSDateTime", "FollowingEndDateTime", "FollowingDuration",
	"FollowingTitle", "FollowingShortText", "FollowingDescription", "Language",
	"HasTeletext", "HasMultilang", "HasDolby", "IsEncrypted", "IsRadio", "IsRecording", 
	"CurrentRecording", "HasVPS", "HasTimer", "IsRunning",
	
	// Volume Display
	"VolumeCurrent", "VolumeTotal", "IsMute",
	
	// Message Display
	"Message", "MessageStatus", "MessageInfo", "MessageWarning",
	"MessageError",
	
	// Replay Display
	"ReplayTitle", "ReplayPositionIndex", "ReplayDurationIndex", "ReplayPrompt", "IsPlaying", 
	"IsFastForward", "IsFastRewind", "IsSlowForward", "IsSlowRewind", "IsPausing",
	"ReplayPosition", "ReplayDuration", "ReplayRemaining", "ReplayMode",

	// Menu Page
	"MenuTitle", "MenuGroup", "IsMenuGroup", "MenuItem", "IsMenuItem", "MenuCurrent", "IsMenuCurrent",
	"MenuText", "ButtonRed", "ButtonGreen", "ButtonYellow", "ButtonBlue", "CanScrollUp", 
	"CanScrollDown"
};

std::string txToken::Token(const txToken &Token) {
	std::string result = (std::string)"{" + Tokens[Token.Type];
	//if (Token.Attrib.length() > 0)
	//	result += ":" + Token.Attrib;
	result += "}";
	
	return result;
}

cxString::cxString(void) {
}

bool cxString::Parse(const std::string &Text) {
	const char *text = Text.c_str();
	const char *ptr = text, *last = text;
	bool inToken = false;
	bool inAttrib = false;
	int offset = 0;

	Dprintf("parsing: %s\n", Text.c_str());

	for (; *ptr; ++ptr) {
		if (inToken && *ptr == '\\') {
			if (*(ptr + 1) == '\0') {
				esyslog("ERROR: Stray \\ in token attribute\n");
				return false;
			}

			++ptr;
			continue;
		} 
		else if (*ptr == '{') {
			if (inToken) {
				esyslog("ERROR: Unexpected '{' in token");
				return false;
			}

			mText.append(last, ptr - last);
			last = ptr + 1;
			inToken = true;
		}
		else if (*ptr == '}' || (inToken && *ptr == ':')) {
			if (!inToken) {
				esyslog("ERROR: Unexpected '}' outside of token");
				return false;
			}

			if (inAttrib) {
				if (*ptr == ':') {
					esyslog("ERROR: Unexpected ':' inside of token attribute");
					return false;
				}

				int pos = -1;
				std::string attr;
				attr.assign(last, ptr - last);
				while ((pos = attr.find('\\', pos + 1)) != -1) {
					switch (attr[pos + 1]) {
					case 'n':
						attr.replace(pos, 2, "\n");
						break;

					default:
						attr.erase(pos, 1);
					}
				}

				txToken &lastToken = mTokens[mTokens.size() - 1];
				if (attr == "clean")
					lastToken.Attrib = aClean;
				else {
					char *end;
					int n = strtol(attr.c_str(), &end, 10);
					Dprintf("attr: %s, n: %d, end: |%s|\n", attr.c_str(), n, end);
					if (end != attr.c_str() && *end == '\0') {
						Dprintf("a number\n");
						lastToken.Attrib = n;
					} else
						lastToken.Attrib = attr;
				} 

				inAttrib = false;
				inToken = false;
			} else {
				int i;
				for (i = 0; i < (int)__COUNT_TOKEN__; ++i) {
					if ((size_t)(ptr - last) == strlen(Tokens[i]) 
							&& memcmp(last, Tokens[i], ptr - last) == 0) {
						txToken token((exToken)i, offset, "");
						mTokens.push_back(token);
						break;
					}
				}

				if (i == (int)__COUNT_TOKEN__) {
					esyslog("ERROR: Unexpected token {%.*s}", (int)(ptr - last), last);
					return false;
				}

				if (*ptr == ':')
					inAttrib = true;
				else
					inToken = false;
			}

			last = ptr + 1;
		}
		else if (!inToken)
			++offset;
	}

	if (inToken) {
		esyslog("ERROR: Expecting '}' in token");
		return false;
	}

	mText.append(last, ptr - last);
	return true;
}

cxType cxString::Evaluate(void) const 
{
	std::string result;
	int offset = 0;

	if (mText.length() == 0 && mTokens.size() == 1)
		return cText2SkinRender::GetToken(mTokens[0]);
	
	for (uint i = 0; i < mTokens.size(); ++i) {
		result.append(mText.c_str() + offset, mTokens[i].Offset - offset);
		result.append(cText2SkinRender::GetToken(mTokens[i]));
		offset = mTokens[i].Offset;
	}
	result.append(mText.c_str() + offset);
	return result;
}

