/*
 * $Id: display.c,v 1.8 2004/06/01 17:25:38 lordjaxom Exp $
 */

#include "render.h"
#include "data.h"
#include "display.h"

// --- cText2SkinDisplayChannel -----------------------------------------------

cText2SkinDisplayChannel::cText2SkinDisplayChannel(cText2SkinData *Data, bool WithInfo) {
	mData     = Data;
	mWithInfo = WithInfo;
	mRender   = new cText2SkinRender(mData, WithInfo ? sectionChannel : sectionChannelSmall);
	mDirty    = false;
}

cText2SkinDisplayChannel::~cText2SkinDisplayChannel() {
	delete mRender;
}

void cText2SkinDisplayChannel::SetChannel(const cChannel *Channel, int Number) {
	if (mRender->mChannel != Channel || mRender->mChannelNumber != Number) {
		mRender->mChannel = Channel;
		mRender->mChannelNumber = Number;
		mDirty = true;
	}
}

void cText2SkinDisplayChannel::SetEvents(const cEvent *Present, const cEvent *Following) {
	if (mRender->mChannelPresent != Present || mRender->mChannelFollowing != Following) {
		mRender->mChannelPresent = Present;
		mRender->mChannelFollowing = Following;
		mDirty = true;
	}
}

void cText2SkinDisplayChannel::SetMessage(eMessageType Type, const char *Text) {
	if (Text == NULL) Text = "";
	if (mRender->mMessageType != Type || mRender->mMessageText != Text) {
		mRender->mMessageType = Type;
		mRender->mMessageText = Text;
		mDirty = true;
	}
}

void cText2SkinDisplayChannel::Flush(void) {
	if (mDirty) {
		mRender->Flush();
		mDirty = false;
	}
}

// --- cText2SkinDisplayVolume ------------------------------------------------

cText2SkinDisplayVolume::cText2SkinDisplayVolume(cText2SkinData *Data) {
	mData     = Data;
	mRender   = new cText2SkinRender(mData, sectionVolume);
	mDirty    = false;
}

cText2SkinDisplayVolume::~cText2SkinDisplayVolume() {
	delete mRender;
}

void cText2SkinDisplayVolume::SetVolume(int Current, int Total, bool Mute) {
	if (mRender->mVolumeCurrent != Current || mRender->mVolumeTotal != Total || mRender->mVolumeMute != Mute) {
		mRender->mVolumeCurrent = Current;
		mRender->mVolumeTotal   = Total;
		mRender->mVolumeMute    = Mute;
		mDirty = true;
	}
}

void cText2SkinDisplayVolume::Flush(void) {
	if (mDirty) {
		mRender->Flush();
		mDirty = false;
	}
}
	
// --- cText2SkinDisplayReplay ------------------------------------------------

cText2SkinDisplayReplay::cText2SkinDisplayReplay(cText2SkinData *Data, bool ModeOnly) {
	mData     = Data;
	mRender   = new cText2SkinRender(mData, ModeOnly ? sectionReplayMode : sectionReplay);
	mDirty    = false;
}

cText2SkinDisplayReplay::~cText2SkinDisplayReplay() {
	delete mRender;
}

void cText2SkinDisplayReplay::SetTitle(const char *Title) {
	if (Title == NULL) Title = "";
	if (mRender->mReplayTitle != Title) {
		mRender->mReplayTitle = Title;
		mDirty = true;
	}
}

void cText2SkinDisplayReplay::SetMode(bool Play, bool Forward, int Speed) {
	if (mRender->mReplayPlay != Play || mRender->mReplayPlay != Forward || mRender->mReplaySpeed != Speed) {
		mRender->mReplayPlay = Play;
		mRender->mReplayForward = Forward;
		mRender->mReplaySpeed = Speed;
		mDirty = true;
	}
}

void cText2SkinDisplayReplay::SetProgress(int Current, int Total) {
	if (mRender->mReplayCurrent != Current || mRender->mReplayTotal != Total) {
		mRender->mReplayCurrent = Current;
		mRender->mReplayTotal = Total;
		mDirty = true;
	}
}

void cText2SkinDisplayReplay::SetMarks(const cMarks *Marks) {
	if (mRender->mReplayMarks != Marks) {
		mRender->mReplayMarks = Marks;
		mDirty = true;
	}
}

void cText2SkinDisplayReplay::SetCurrent(const char *Current) {
	if (Current == NULL) Current = "";
	if (mRender->mReplayCurrentText != Current) {
		mRender->mReplayCurrentText = Current;
		mDirty = true;
	}
}

void cText2SkinDisplayReplay::SetTotal(const char *Total) {
	if (Total == NULL) Total = "";
	if (mRender->mReplayTotalText != Total) {
		mRender->mReplayTotalText = Total;
		mDirty = true;
	}
}

void cText2SkinDisplayReplay::SetJump(const char *Jump) {
	if (Jump == NULL) Jump = "";
	if (mRender->mReplayJump != Jump) {
		mRender->mReplayJump = Jump;
		mDirty = true;
	}
}

void cText2SkinDisplayReplay::SetMessage(eMessageType Type, const char *Text) {
	if (Text == NULL) Text = "";
	if (mRender->mMessageType != Type || mRender->mMessageText != Text) {
		mRender->mMessageType = Type;
		mRender->mMessageText = Text;
		mDirty = true;
	}
}

void cText2SkinDisplayReplay::Flush(void) {
	if (mDirty) {
		mRender->Flush();
		mDirty = false;
	}
}

// --- cText2SkinDisplayMessage -----------------------------------------------

cText2SkinDisplayMessage::cText2SkinDisplayMessage(cText2SkinData *Data) {
	mData     = Data;
	mRender   = new cText2SkinRender(mData, sectionMessage);
	mDirty    = false;
}

cText2SkinDisplayMessage::~cText2SkinDisplayMessage() {
	delete mRender;
}

void cText2SkinDisplayMessage::SetMessage(eMessageType Type, const char *Text) {
	if (Text == NULL) Text = "";
	if (mRender->mMessageType != Type || mRender->mMessageText != Text) {
		mRender->mMessageType = Type;
		mRender->mMessageText = Text;
		mDirty = true;
	}
}

void cText2SkinDisplayMessage::Flush(void) {
	if (mDirty) {
		mRender->Flush();
		mDirty = false;
	}
}

// --- cText2SkinDisplayMenu --------------------------------------------------

cText2SkinDisplayMenu::cText2SkinDisplayMenu(cText2SkinData *Data) {
	mData     = Data;
	mRender   = new cText2SkinRender(mData, sectionMenu);
	mDirty    = false;
	mMaxItems = 0;

	cText2SkinItem *area = mData->Get(itemMenuArea);
	cText2SkinItem *item = mData->Get(itemMenuItem);
	if (area && item)
		mMaxItems = area->Size().h / item->Size().h;
	else
		esyslog("ERROR: text2skin: Skin is missing the items MenuArea and/or MenuItem");
}

cText2SkinDisplayMenu::~cText2SkinDisplayMenu() {
	delete mRender;
}

void cText2SkinDisplayMenu::Clear(void) {
	mRender->mMenuItems.clear();
	mRender->mMenuCurrent = -1;
	mDirty = true;
}

void cText2SkinDisplayMenu::SetTitle(const char *Title) {
	if (Title == NULL) Title = "";
	if (mRender->mMenuTitle != Title) {
		mRender->mMenuTitle = Title;
		mDirty = true;
	}
}

void cText2SkinDisplayMenu::SetButtons(const char *Red, const char *Green, const char *Yellow, const char *Blue) {
	if (Red    == NULL) Red    = "";
	if (Green  == NULL) Green  = "";
	if (Yellow == NULL) Yellow = "";
	if (Blue   == NULL) Blue   = "";
	if (mRender->mMenuRed != Red || mRender->mMenuGreen != Green || mRender->mMenuYellow != Yellow || mRender->mMenuBlue != Blue) {
		mRender->mMenuRed    = Red;
		mRender->mMenuGreen  = Green;
		mRender->mMenuYellow = Yellow;
		mRender->mMenuBlue   = Blue;
		mDirty = true;
	}
}

void cText2SkinDisplayMenu::SetMessage(eMessageType Type, const char *Text) {
	if (Text == NULL) Text = "";
	if (mRender->mMessageType != Type || mRender->mMessageText != Text) {
		mRender->mMessageType = Type;
		mRender->mMessageText = Text;
		mDirty = true;
	}
}

void cText2SkinDisplayMenu::SetItem(const char *Text, int Index, bool Current, bool Selectable) {
	cText2SkinRender::MenuItem item = { Text, Selectable };
	if ((int)mRender->mMenuItems.size() <= Index) {
		mRender->mMenuItems.push_back(item);
		mDirty = true;
	} else if (mRender->mMenuItems[Index] != item) {
		mRender->mMenuItems[Index] = item;
		mDirty = true;
	}
	if (Current && mRender->mMenuCurrent != Index) {
		mRender->mMenuCurrent = Index;
		mDirty = true;
	}
}

void cText2SkinDisplayMenu::SetEvent(const cEvent *Event) {
	if (mRender->mMenuEvent != Event) {
		mRender->mMenuEvent = Event;
		mDirty = true;
	}
}

void cText2SkinDisplayMenu::SetRecording(const cRecording *Recording) {
	if (mRender->mMenuRecording != Recording) {
		mRender->mMenuRecording = Recording;
		mDirty = true;
	}
}

void cText2SkinDisplayMenu::SetText(const char *Text, bool FixedFont) {
	if (Text == NULL) Text = "";
	if (mRender->mMenuText != Text || mRender->mMenuTextFixedFont != FixedFont) {
		mRender->mMenuText = Text;
		mRender->mMenuTextFixedFont = FixedFont;
		mDirty = true;
	}
}

void cText2SkinDisplayMenu::Flush(void) {
	if (mDirty) {
		mRender->Flush();
		mDirty = false;
	}
}

