/*
 *  $Id: string.h,v 1.9 2005/01/11 18:18:31 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_XML_STRING_H
#define VDR_TEXT2SKIN_XML_STRING_H

#include "xml/type.h"
#include <string>
#include <vector>

enum exToken {
	tDateTime,
	tFreeDiskSpace,
	tAudioTrack,
	tAudioChannel,

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
	// next 1 also in all other displays
	tCurrentRecording,
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
	tReplayIsShuffle,
	tReplayIsLoop,

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

enum exAttrib {
	aNone,
	aNumber,
	aString,
	aClean

#define __COUNT_ATTRIB__ (aClean + 1)
};

struct txAttrib {
	exAttrib    Type;
	std::string Text;
	int         Number;

	txAttrib(const std::string &a): Type(aString), Text(a), Number(0) {}
	txAttrib(int n): Type(aNumber), Text(""), Number(n) {}
	txAttrib(exAttrib t): Type(t), Text(""), Number(0) {}
	txAttrib(void): Type(aNone), Text(""), Number(0) {}

	friend bool operator== (const txAttrib &A, const txAttrib &B);
	friend bool operator<  (const txAttrib &A, const txAttrib &B);
};

inline bool operator== (const txAttrib &A, const txAttrib &B)
{
	return A.Type == B.Type
	    && A.Text == B.Text
	    && A.Number == B.Number;
}

inline bool operator<  (const txAttrib &A, const txAttrib &B)
{
	return A.Type == B.Type
	       ? A.Text == B.Text
		     ? A.Number < B.Number
	         : A.Text < B.Text
	       : A.Type < B.Type;
}

struct txToken {
	exToken     Type;
	uint        Offset;
	txAttrib    Attrib;
	int         Index;
	int         Tab;

	txToken(void): Index(-1), Tab(-1) {}
	txToken(exToken t, uint o, const std::string &a): 
		Type(t), Offset(o), Attrib(a), Index(-1), Tab(-1) {}

	friend bool operator< (const txToken &A, const txToken &B);

	static std::string Token(const txToken &Token);
};

inline bool operator< (const txToken &A, const txToken &B) 
{
	return A.Type == B.Type
	       ? A.Attrib == B.Attrib
	         ? A.Index == B.Index
	           ? A.Tab < B.Tab
	           : A.Index < B.Index
	         : A.Attrib < B.Attrib
	       : A.Type < B.Type;
}

class cxSkin;

class cxString {
private:
	typedef std::vector<cxString*> tStringList;
	static tStringList mStrings;

	std::string          mText;
	std::string          mOriginal;
	std::vector<txToken> mTokens;
	cxSkin              *mSkin;

public:
	static void Reparse(void);

	cxString(cxSkin *Skin);
	~cxString();

	bool Parse(const std::string &Text);
	bool Parse(void) { return mOriginal.length() > 0 ? Parse(mOriginal) : true; }
	cxType Evaluate(void) const;

	void SetListIndex(uint Index, int Tab);
};

inline void cxString::SetListIndex(uint Index, int Tab)
{
	for (uint i = 0; i < mTokens.size(); ++i) {
		mTokens[i].Index = Index;
		mTokens[i].Tab = Tab;
	}
}

#endif // VDR_TEXT2SKIN_XML_STRING_H
