/*
 *  $Id: string.h,v 1.8 2004/12/17 19:56:16 lordjaxom Exp $
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
	tChannelPortal,
	tChannelSource,
	// next 8 also in Menu
	tPresentStartDateTime,
	tPresentVPSDateTime,
	tPresentEndDateTime,
	tPresentDuration,
	tPresentProgress,
	tPresentRemaining,
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

	// Message Display (also in all other displays)
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
	tReplayRemaining,
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
	// next four also in Channel and Replay display (if supported by vdr/plugin)
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

	friend bool operator< (const txToken &A, const txToken &B);

	static std::string Token(const txToken &Token);
};

inline bool operator< (const txToken &A, const txToken &B) 
{
	if (A.Type == B.Type) {
		if (A.Attrib == B.Attrib) {
			if (A.Index == B.Index)
				return A.Tab < B.Tab;
			else
				return A.Index < B.Index;
		}
		else
			return A.Attrib < B.Attrib;
	}
	else
		return A.Type < B.Type;
}

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
