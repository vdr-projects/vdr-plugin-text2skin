/*
 *  $Id: string.h,v 1.5 2004/12/08 18:47:37 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_XML_STRING_H
#define VDR_TEXT2SKIN_XML_STRING_H

#include "xml/type.h"
#include <string>
#include <vector>

enum exToken {
	tDateTime,

	// Channel Display
	tChannelNumber,
	tChannelName,
	tChannelShortName,
	tChannelBouquet,
	// next 8 also in Menu
	tPresentStartDateTime,
	tPresentVPSDateTime,
	tPresentEndDateTime,
	tPresentDuration,
	tPresentProgress,
	tPresentTitle,
	tPresentShortText,
	tPresentDescription,
	tFollowingStartDateTime,
	tFollowingVPSDateTime,
	tFollowingEndDateTime,
	tFollowingDuration,
	tFollowingTitle,
	tFollowingShortText,
	tFollowingDescription,
	tLanguage,
	tHasTeletext,
	tHasMultilang,
	tHasDolby,
	tIsEncrypted,
	tIsRadio,
	tIsRecording,
	// next 3 also in Menu
	tHasVPS,
	tHasTimer,
	tIsRunning,

	// VolumeDisplay
	tVolumeCurrent,
	tVolumeTotal,
	tIsMute,

	// Message Display
	tMessage,
	tMessageStatus,
	tMessageInfo,
	tMessageWarning,
	tMessageError,

	// Replay Display
	tReplayTitle,
	tReplayPositionIndex,
	tReplayDurationIndex,
	tReplayPrompt,
	tIsPlaying,
	tIsFastForward,
	tIsFastRewind,
	tIsSlowForward,
	tIsSlowRewind,
	tIsPausing,
	tReplayPosition,
	tReplayDuration,
	tReplayMode,

	// Menu Page
	tMenuTitle,
	tMenuGroup,
	tIsMenuGroup,
	tMenuItem,
	tIsMenuItem,
	tMenuCurrent,
	tIsMenuCurrent,
	tMenuText,
	tButtonRed,
	tButtonGreen,
	tButtonYellow,
	tButtonBlue,
	tCanScrollUp,
	tCanScrollDown,

#define __COUNT_TOKEN__ (tCanScrollDown + 1)
};

struct txToken {
	exToken      Type;
	uint         Offset;
	std::string  Attrib;
	int          Index;
	int          Tab;

	txToken(void): Index(-1), Tab(-1) {}
	txToken(exToken t, uint o, const std::string &a): Type(t), Offset(o), Attrib(a), Index(-1), Tab(-1) {}
	static std::string Token(const txToken &Token);
};

class cxString {
private:
	std::string          mText;
	std::vector<txToken> mTokens;

public:
	cxString(void);

	bool Parse(const std::string &Text);
	cxType Evaluate(void) const;

	void SetIndex(uint Index, int Tab);
};

inline void cxString::SetIndex(uint Index, int Tab)
{
	for (uint i = 0; i < mTokens.size(); ++i) {
		mTokens[i].Index = Index;
		mTokens[i].Tab = Tab;
	}
}

#endif // VDR_TEXT2SKIN_XML_STRING_H
