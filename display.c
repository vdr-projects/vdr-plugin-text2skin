/*
 * $Id: display.c,v 1.17 2004/06/16 18:46:50 lordjaxom Exp $
 */

#include "render.h"
#include "data.h"
#include "loader.h"
#include "display.h"

// --- cText2SkinDisplayChannel -----------------------------------------------

cText2SkinDisplayChannel::cText2SkinDisplayChannel(cText2SkinLoader *Loader, bool WithInfo) {
	mWithInfo = WithInfo;
	mRender   = new cText2SkinRender(Loader, WithInfo ? sectionChannel : sectionChannelSmall);
	mDirty    = false;
}

cText2SkinDisplayChannel::~cText2SkinDisplayChannel() {
	delete mRender;
}

void cText2SkinDisplayChannel::SetChannel(const cChannel *Channel, int Number) {
	mRender->Lock();
	if (mRender->mChannel != Channel || mRender->mChannelNumber != Number) {
		mRender->mChannel = Channel;
		mRender->mChannelNumber = Number;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayChannel::SetEvents(const cEvent *Present, const cEvent *Following) {
	mRender->Lock();
	if (mRender->mChannelPresent != Present || mRender->mChannelFollowing != Following) {
		mRender->mChannelPresent = Present;
		mRender->mChannelFollowing = Following;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayChannel::SetMessage(eMessageType Type, const char *Text) {
	if (Text == NULL) Text = "";
	mRender->Lock();
	if (mRender->mMessageType != Type || mRender->mMessageText != Text) {
		mRender->mMessageType = Type;
		mRender->mMessageText = Text;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayChannel::Flush(void) {
	if (mDirty) {
		mRender->Flush();
		mDirty = false;
	}
}

// --- cText2SkinDisplayVolume ------------------------------------------------

cText2SkinDisplayVolume::cText2SkinDisplayVolume(cText2SkinLoader *Loader) {
	mRender   = new cText2SkinRender(Loader, sectionVolume);
	mDirty    = false;
}

cText2SkinDisplayVolume::~cText2SkinDisplayVolume() {
	delete mRender;
}

void cText2SkinDisplayVolume::SetVolume(int Current, int Total, bool Mute) {
	mRender->Lock();
	if (mRender->mVolumeCurrent != Current || mRender->mVolumeTotal != Total || mRender->mVolumeMute != Mute) {
		mRender->mVolumeCurrent = Current;
		mRender->mVolumeTotal   = Total;
		mRender->mVolumeMute    = Mute;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayVolume::Flush(void) {
	if (mDirty) {
		mRender->Flush();
		mDirty = false;
	}
}
	
// --- cText2SkinDisplayReplay ------------------------------------------------

cText2SkinDisplayReplay::cText2SkinDisplayReplay(cText2SkinLoader *Loader, bool ModeOnly) {
	Dprintf("ModeOnly: %d\n", ModeOnly);
	mRender   = new cText2SkinRender(Loader, ModeOnly ? sectionReplayMode : sectionReplay);
	mDirty    = false;
}

cText2SkinDisplayReplay::~cText2SkinDisplayReplay() {
	delete mRender;
}

void cText2SkinDisplayReplay::SetTitle(const char *Title) {
	if (Title == NULL) Title = "";
	mRender->Lock();
	if (mRender->mReplayTitle != Title) {
		mRender->mReplayTitle = Title;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayReplay::SetMode(bool Play, bool Forward, int Speed) {
	mRender->Lock();
	if (mRender->mReplayPlay != Play || mRender->mReplayForward != Forward || mRender->mReplaySpeed != Speed) {
		mRender->mReplayPlay = Play;
		mRender->mReplayForward = Forward;
		mRender->mReplaySpeed = Speed;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayReplay::SetProgress(int Current, int Total) {
	mRender->Lock();
	if (mRender->mReplayCurrent != Current || mRender->mReplayTotal != Total) {
		mRender->mReplayCurrent = Current;
		mRender->mReplayTotal = Total;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayReplay::SetMarks(const cMarks *Marks) {
	mRender->Lock();
	if (mRender->mReplayMarks != Marks) {
		mRender->mReplayMarks = Marks;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayReplay::SetCurrent(const char *Current) {
	if (Current == NULL) Current = "";
	mRender->Lock();
	if (mRender->mReplayCurrentText != Current) {
		mRender->mReplayCurrentText = Current;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayReplay::SetTotal(const char *Total) {
	if (Total == NULL) Total = "";
	mRender->Lock();
	if (mRender->mReplayTotalText != Total) {
		mRender->mReplayTotalText = Total;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayReplay::SetJump(const char *Jump) {
	if (Jump == NULL) Jump = "";
	mRender->Lock();
	if (mRender->mReplayJump != Jump) {
		mRender->mReplayJump = Jump;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayReplay::SetMessage(eMessageType Type, const char *Text) {
	if (Text == NULL) Text = "";
	mRender->Lock();
	if (mRender->mMessageType != Type || mRender->mMessageText != Text) {
		mRender->mMessageType = Type;
		mRender->mMessageText = Text;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayReplay::Flush(void) {
	if (mDirty) {
		mRender->Flush();
		mDirty = false;
	}
}

// --- cText2SkinDisplayMessage -----------------------------------------------

cText2SkinDisplayMessage::cText2SkinDisplayMessage(cText2SkinLoader *Loader) {
	mRender   = new cText2SkinRender(Loader, sectionMessage);
	mDirty    = false;
}

cText2SkinDisplayMessage::~cText2SkinDisplayMessage() {
	delete mRender;
}

void cText2SkinDisplayMessage::SetMessage(eMessageType Type, const char *Text) {
	if (Text == NULL) Text = "";
	mRender->Lock();
	if (mRender->mMessageType != Type || mRender->mMessageText != Text) {
		mRender->mMessageType = Type;
		mRender->mMessageText = Text;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayMessage::Flush(void) {
	if (mDirty) {
		mRender->Flush();
		mDirty = false;
	}
}

// --- cText2SkinDisplayMenu --------------------------------------------------

cText2SkinDisplayMenu::cText2SkinDisplayMenu(cText2SkinLoader *Loader) {
	mRender   = new cText2SkinRender(Loader, sectionMenu);
	mDirty    = false;
	mMaxItems = 0;

	cText2SkinItem *area = Loader->Data()->Get(sectionMenu, itemMenuArea);
	cText2SkinItem *item = Loader->Data()->Get(sectionMenu, itemMenuItem);
	if (area && item)
		mMaxItems = area->Size().h / item->Size().h;
	else
		esyslog("ERROR: text2skin: Skin is missing the items MenuArea and/or MenuItem");
}

cText2SkinDisplayMenu::~cText2SkinDisplayMenu() {
	delete mRender;
}

void cText2SkinDisplayMenu::Clear(void) {
	mRender->Lock();
	mRender->mMenuItems.clear();
	mRender->mMenuTitle     = "";
	mRender->mMenuCurrent   = -1;
	mRender->mMenuRed       = "";
	mRender->mMenuGreen     = "";
	mRender->mMenuYellow    = "";
	mRender->mMenuBlue      = "";
	mRender->mMenuEvent     = NULL;
	mRender->mMenuRecording = NULL;
	mRender->mMenuText      = "";
	//mRender->mMessageText   = "";
	DELETENULL(mRender->mScroller);
	mRender->Unlock();
	mDirty = true;
}

void cText2SkinDisplayMenu::SetTitle(const char *Title) {
	if (Title == NULL) Title = "";
	mRender->Lock();
	if (mRender->mMenuTitle != Title) {
		mRender->mMenuTitle = Title;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayMenu::SetButtons(const char *Red, const char *Green, const char *Yellow, const char *Blue) {
	if (Red    == NULL) Red    = "";
	if (Green  == NULL) Green  = "";
	if (Yellow == NULL) Yellow = "";
	if (Blue   == NULL) Blue   = "";
	mRender->Lock();
	if (mRender->mMenuRed != Red || mRender->mMenuGreen != Green || mRender->mMenuYellow != Yellow || mRender->mMenuBlue != Blue) {
		mRender->mMenuRed    = Red;
		mRender->mMenuGreen  = Green;
		mRender->mMenuYellow = Yellow;
		mRender->mMenuBlue   = Blue;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayMenu::SetMessage(eMessageType Type, const char *Text) {
	if (Text == NULL) Text = "";
	mRender->Lock();
	if (mRender->mMessageType != Type || mRender->mMessageText != Text) {
		mRender->mMessageType = Type;
		mRender->mMessageText = Text;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayMenu::SetItem(const char *Text, int Index, bool Current, bool Selectable) {
	cText2SkinRender::MenuItem item;
	item.text = Text;
	item.sel = Selectable;
	for (int i = 0; i < MaxTabs; ++i) {
		const char *tab = GetTabbedText(Text, i);
		if (tab)
			item.tabs[i] = tab;
		if (!Tab(i + 1))
			break;
	}
	mRender->Lock();
	SetEditableWidth(mRender->GetEditableWidth(item, Current));
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
	mRender->Unlock();
}

void cText2SkinDisplayMenu::SetEvent(const cEvent *Event) {
	mRender->Lock();
	if (mRender->mMenuEvent != Event) {
		mRender->mMenuEvent = Event;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayMenu::SetRecording(const cRecording *Recording) {
	mRender->Lock();
	if (mRender->mMenuRecording != Recording) {
		mRender->mMenuRecording = Recording;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayMenu::SetText(const char *Text, bool FixedFont) {
	if (Text == NULL) Text = "";
	mRender->Lock();
	if (mRender->mMenuText != Text || mRender->mMenuTextFixedFont != FixedFont) {
		mRender->mMenuText = Text;
		mRender->mMenuTextFixedFont = FixedFont;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayMenu::SetTabs(int Tab1, int Tab2, int Tab3, int Tab4, int Tab5) {
	cSkinDisplayMenu::SetTabs(Tab1, Tab2, Tab3, Tab4, Tab5);
	mRender->Lock();
	mRender->mMenuTabs[0] = Tab(0);
	mRender->mMenuTabs[1] = Tab(1);
	mRender->mMenuTabs[2] = Tab(2);
	mRender->mMenuTabs[3] = Tab(3);
	mRender->mMenuTabs[4] = Tab(4);
	mRender->mMenuTabs[5] = Tab(5);
	mRender->Unlock();
}

void cText2SkinDisplayMenu::Scroll(bool Up, bool Page) {
	mRender->Lock();
	if (mRender->mScroller && (Up ? mRender->mScroller->CanScrollUp() : mRender->mScroller->CanScrollDown())) {
		mRender->mMenuScroll = true;
		mRender->mMenuScrollUp = Up;
		mRender->mMenuScrollPage = Page;
		mDirty = true;
	}
	mRender->Unlock();
}

void cText2SkinDisplayMenu::Flush(void) {
	if (mDirty) {
		mRender->Flush();
		mDirty = false;
	}
}

