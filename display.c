/*
 * $Id: display.c,v 1.1.1.1 2004/05/23 00:08:03 lordjaxom Exp $
 */

#include "display.h"
#include "data.h"
#include "render.h"

// --- cText2SkinDisplayChannel -----------------------------------------------

cText2SkinDisplayChannel::cText2SkinDisplayChannel(cText2SkinData *Data, bool WithInfo) {
	printf("cText2SkinDisplayChannel\n");
	mData     = Data;
	mWithInfo = WithInfo;
	mRender   = new cText2SkinRender(mData, sectionChannel);
	mDirty    = false;
}

cText2SkinDisplayChannel::~cText2SkinDisplayChannel() {
	delete mRender;
}

void cText2SkinDisplayChannel::SetChannel(const cChannel *Channel, int Number) {
	printf("SetChannel\n");
	if (mRender->mChannel != Channel || mRender->mNumber != Number) {
		mRender->mChannel = Channel;
		mRender->mNumber = Number;
		mDirty = true;
	}
}

void cText2SkinDisplayChannel::SetEvents(const cEvent *Present, const cEvent *Following) {
	if (mRender->mPresent != Present || mRender->mFollowing != Following) {
		mRender->mPresent = Present;
		mRender->mFollowing = Following;
		mDirty = true;
	}
}

void cText2SkinDisplayChannel::Flush(void) {
	if (mDirty) {
		printf("real flush\n");
		mRender->Flush();
		mDirty = false;
	}
}

// --- cText2SkinDisplayVolume ------------------------------------------------

cText2SkinDisplayVolume::cText2SkinDisplayVolume(cText2SkinData *Data) {
	printf("cText2SkinDisplayVolume\n");
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
		printf("real flush\n");
		mRender->Flush();
		mDirty = false;
	}
}
	
// --- cText2SkinDisplayReplay ------------------------------------------------

cText2SkinDisplayReplay::cText2SkinDisplayReplay(cText2SkinData *Data, bool ModeOnly) {
	printf("cText2SkinDisplayVolume\n");
	mData     = Data;
	mRender   = new cText2SkinRender(mData, ModeOnly ? sectionReplayMode : sectionReplay);
	mDirty    = false;
}

cText2SkinDisplayReplay::~cText2SkinDisplayReplay() {
	delete mRender;
}

void cText2SkinDisplayReplay::SetTitle(const char *Title) {
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

void cText2SkinDisplayReplay::SetCurrent(const char *Current) {
}

void cText2SkinDisplayReplay::SetTotal(const char *Total) {
}

void cText2SkinDisplayReplay::SetJump(const char *Jump) {
	if (mRender->mReplayJump != Jump) {
		mRender->mReplayJump = NULL;
		mDirty = true;
	}
}

void cText2SkinDisplayReplay::Flush(void) {
	if (mDirty) {
		printf("real flush\n");
		mRender->Flush();
		mDirty = false;
	}
}

// --- cText2SkinDisplayMessage -----------------------------------------------

cText2SkinDisplayMessage::cText2SkinDisplayMessage(cText2SkinData *Data) {
	printf("cText2SkinDisplayMessage\n");
	mData     = Data;
	mRender   = new cText2SkinRender(mData, sectionMessage);
	mDirty    = false;
}

cText2SkinDisplayMessage::~cText2SkinDisplayMessage() {
	delete mRender;
}

void cText2SkinDisplayMessage::SetMessage(eMessageType Type, const char *Text) {
	if (mRender->mMessageType != Type || mRender->mMessageText != Text) {
		mRender->mMessageType = Type;
		mRender->mMessageText = Text;
		mDirty = true;
	}
}

void cText2SkinDisplayMessage::Flush(void) {
	if (mDirty) {
		printf("real flush\n");
		mRender->Flush();
		mDirty = false;
	}
}

// --- cText2SkinDisplayMenu --------------------------------------------------

cText2SkinDisplayMenu::cText2SkinDisplayMenu(cText2SkinData *Data) {
	printf("cText2SkinDisplayMenu\n");
	mData     = Data;
	mRender   = new cText2SkinRender(mData, sectionMenu);
	mDirty    = false;
	mMaxItems = 0;

	cText2SkinItem *area = mData->Get(itemMenuArea);
	cText2SkinItem *item = mData->Get(itemMenuItem);
	if (area && item)
		mMaxItems = area->Height() / item->Height();
	else
		esyslog("ERROR: text2skin: Skin is missing the items MenuArea and/or MenuItem");
}

cText2SkinDisplayMenu::~cText2SkinDisplayMenu() {
	delete mRender;
}

void cText2SkinDisplayMenu::Clear(void) {
	mRender->mItems.clear();
	mRender->mCurrent = -1;
	mDirty = true;
}

void cText2SkinDisplayMenu::SetTitle(const char *Title) {
	if (mRender->mTitle != Title) {
		mRender->mTitle = Title;
		mDirty = true;
	}
}

void cText2SkinDisplayMenu::SetButtons(const char *Red, const char *Green, const char *Yellow, const char *Blue) {
}

void cText2SkinDisplayMenu::SetMessage(eMessageType Type, const char *Text) {
}

void cText2SkinDisplayMenu::SetItem(const char *Text, int Index, bool Current, bool Selectable) {
	cText2SkinRender::MenuItem item = { Text, Selectable };
	if ((int)mRender->mItems.size() <= Index) {
		mRender->mItems.push_back(item);
		mDirty = true;
	} else if (mRender->mItems[Index] != item) {
		mRender->mItems[Index] = item;
		mDirty = true;
	}
	if (Current && mRender->mCurrent != Index) {
		mRender->mCurrent = Index;
		mDirty = true;
	}
}

void cText2SkinDisplayMenu::SetEvent(const cEvent *Event) {
}

void cText2SkinDisplayMenu::SetRecording(const cRecording *Recording) {
}

void cText2SkinDisplayMenu::SetText(const char *Text, bool FixedFont) {
}

void cText2SkinDisplayMenu::Flush(void) {
	if (mDirty) {
		printf("real flush\n");
		mRender->Flush();
		mDirty = false;
	}
}

