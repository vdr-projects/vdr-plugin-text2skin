/*
 * $Id: display.c,v 1.4 2005/01/02 19:55:36 lordjaxom Exp $
 */

#include "render.h"
#include "loader.h"
#include "display.h"
#include "scroller.h"
#include "status.h"
#include "common.h"
#include "xml/string.h"
#include <vdr/menu.h>

// --- cText2SkinDisplayChannel -----------------------------------------------

cText2SkinDisplayChannel::cText2SkinDisplayChannel(cText2SkinLoader *Loader, bool WithInfo):
		cText2SkinRender(Loader, WithInfo ? cxDisplay::channelInfo : cxDisplay::channelSmall),
		mChannel(NULL),
		mNumber(0),
		mPresent(NULL),
		mFollowing(NULL),
		mType(mtStatus),
		mText("") ,
		mButtonRed(""),
		mButtonGreen(""),
		mButtonYellow(""),
		mButtonBlue("")
{
}

cText2SkinDisplayChannel::~cText2SkinDisplayChannel()
{
}

void cText2SkinDisplayChannel::SetChannel(const cChannel *Channel, int Number)
{
	UpdateLock();
	if (mChannel != Channel || mNumber != Number) {
		mChannel = Channel;
		mNumber = Number;
		SetDirty();
	}
	UpdateUnlock();
}

void cText2SkinDisplayChannel::SetEvents(const cEvent *Present, const cEvent *Following)
{
	UpdateLock();
	if (mPresent != Present || mFollowing != Following) {
		mPresent = Present;
		mFollowing = Following;
		SetDirty();
	}
	UpdateUnlock();
}

void cText2SkinDisplayChannel::SetMessage(eMessageType Type, const char *Text)
{
	UpdateLock();
	if (Text == NULL) Text = "";
	if (mType != Type || mText != Text) {
		mType = Type;
		mText = Text;
		SetDirty();
	}
	UpdateUnlock();
}
	
void cText2SkinDisplayChannel::SetButtons(const char *Red, const char *Green, const char *Yellow, const char *Blue)
{
	UpdateLock();
	Dprintf("SetButtons(%s, %s, %s, %s)\n", Red, Green, Yellow, Blue);
	if (Red    == NULL) Red    = "";
	if (Green  == NULL) Green  = "";
	if (Yellow == NULL) Yellow = "";
	if (Blue   == NULL) Blue   = "";
	if (mButtonRed != Red || mButtonGreen != Green || mButtonYellow != Yellow 
			|| mButtonBlue != Blue) {
		mButtonRed    = Red;
		mButtonGreen  = Green;
		mButtonYellow = Yellow;
		mButtonBlue   = Blue;
		SetDirty();
	}
	UpdateUnlock();
}

cxType cText2SkinDisplayChannel::GetTokenData(const txToken &Token)
{
	switch (Token.Type) {
	case tChannelNumber:
		return mChannel != NULL
		       ? (cxType)ChannelNumber(mChannel, mNumber)
		       : (cxType)false;
		
	case tChannelName:
		return mChannel != NULL
		       ? (cxType)ChannelName(mChannel, mNumber)
		       : (cxType)false;

	case tChannelShortName:
		return mChannel != NULL
		       ? (cxType)ChannelShortName(mChannel, mNumber)
		       : (cxType)false;
	
	case tChannelBouquet:
#if VDRVERSNUM < 10315
		return cxType::False;
#else
		return mChannel != NULL
		       ? (cxType)mChannel->Provider()
		       : (cxType)false;
#endif

	case tChannelPortal:
#if VDRVERSNUM < 10315
		return false;
#else
		return mChannel != NULL
		       ? (cxType)mChannel->PortalName()
		       : (cxType)false;
#endif

	case tChannelSource:
		return mChannel != NULL
		       ? (cxType)Sources.Get(mChannel->Source())->Description()
		       : (cxType)false;

	case tPresentStartDateTime:
		return mPresent != NULL
		       ? (cxType)TimeType(mPresent->StartTime(), Token.Attrib.Text)
		       : (cxType)false;

	case tPresentVPSDateTime:
		return mPresent != NULL
		       ? (cxType)TimeType(mPresent->Vps(), Token.Attrib.Text)
		       : (cxType)false;

	case tPresentEndDateTime: return mPresent != NULL
		       ? (cxType)TimeType(mPresent->EndTime(), Token.Attrib.Text)
		       : (cxType)false;

	case tPresentProgress:
		return mPresent != NULL
		       ? (cxType)DurationType((time(NULL) - mPresent->StartTime()) * FRAMESPERSEC, 
		                              Token.Attrib.Text)
		       : (cxType)false;

	case tPresentDuration:
		return mPresent != NULL
		       ? (cxType)DurationType(mPresent->Duration() * FRAMESPERSEC, Token.Attrib.Text)
		       : (cxType)false;

	case tPresentRemaining:
		return mPresent != NULL
		       ? (cxType)DurationType((mPresent->Duration() - (time(NULL) - mPresent->StartTime()))
		                              * FRAMESPERSEC, Token.Attrib.Text)
		       : (cxType)false;

	case tPresentTitle:
		return mPresent != NULL
		       ? (cxType)mPresent->Title()
		       : (cxType)false;

	case tPresentShortText:
		return mPresent != NULL
		       ? (cxType)mPresent->ShortText()
		       : (cxType)false;

	case tPresentDescription:
		return mPresent != NULL
		       ? (cxType)mPresent->Description()
		       : (cxType)false;

	case tFollowingStartDateTime:
		return mFollowing != NULL
		       ? (cxType)TimeType(mFollowing->StartTime(), Token.Attrib.Text)
		       : (cxType)false;

	case tFollowingVPSDateTime:
		return mFollowing != NULL
		       ? (cxType)TimeType(mFollowing->Vps(), Token.Attrib.Text)
		       : (cxType)false;
	
	case tFollowingEndDateTime:
		return mFollowing != NULL
		       ? (cxType)TimeType(mFollowing->EndTime(), Token.Attrib.Text)
		       : (cxType)false;

	case tFollowingDuration:
		return mFollowing != NULL
		       ? (cxType)DurationType(mFollowing->Duration() * FRAMESPERSEC, Token.Attrib.Text)
		       : (cxType)false;
		
	case tFollowingTitle:
		return mFollowing != NULL
		       ? (cxType)mFollowing->Title()
		       : (cxType)false;

	case tFollowingShortText:
		return mFollowing != NULL
		       ? (cxType)mFollowing->ShortText()
		       : (cxType)false;

	case tFollowingDescription:
		return mFollowing != NULL
		       ? (cxType)mFollowing->Description()
		       : (cxType)false;

	case tLanguage: {
#if VDRVERSNUM >= 10318
			// TODO !!!
#else
			int cur;
			const char **tracks = cDevice::PrimaryDevice()->GetAudioTracks(&cur);
			if (tracks) {
				int i = 0;
				while (tracks[i] != NULL)
					++i;
				if (cur < i)
					return tracks[cur];
			}
#endif
		}
		return false;
		
	case tHasTeletext:
		return mChannel != NULL && mChannel->Tpid() != 0;

	case tHasMultilang:
		return mChannel != NULL && mChannel->Apid2() != 0;

	case tHasDolby:
		return mChannel != NULL && mChannel->Dpid1() != 0;

	case tIsEncrypted:
		return mChannel != NULL && mChannel->Ca() != 0;

	case tIsRadio:
		return mChannel != NULL && ISRADIO(mChannel);

	case tIsRecording:
		return cRecordControls::Active();

	case tHasVPS:
		return mPresent != NULL && mPresent->Vps() != 0;

	case tHasTimer:
		return mPresent != NULL && mPresent->HasTimer();
	
	case tIsRunning:
		return mPresent != NULL && mPresent->IsRunning();

	case tMessage:
		return mText;

	case tMessageInfo:
		return mType == mtInfo
		       ? (cxType)mText
		       : (cxType)false;

	case tMessageWarning:
		return mType == mtWarning
		       ? (cxType)mText
		       : (cxType)false;
	
	case tMessageStatus:
		return mType == mtStatus
		       ? (cxType)mText
		       : (cxType)false;
	
	case tMessageError:
		return mType == mtError
		       ? (cxType)mText
		       : (cxType)false;

	case tButtonRed:
		return mButtonRed;

	case tButtonGreen:
		return mButtonGreen;

	case tButtonYellow:
		return mButtonYellow;

	case tButtonBlue:
		return mButtonBlue;

	default:
		return cText2SkinRender::GetTokenData(Token);
	}
}

// --- cText2SkinDisplayVolume ------------------------------------------------

cText2SkinDisplayVolume::cText2SkinDisplayVolume(cText2SkinLoader *Loader):
		cText2SkinRender(Loader, cxDisplay::volume),
		mCurrent(0),
		mTotal(0),
		mMute(false)
{
}

cText2SkinDisplayVolume::~cText2SkinDisplayVolume()
{
}

void cText2SkinDisplayVolume::SetVolume(int Current, int Total, bool Mute)
{
	UpdateLock();
	if (mCurrent != Current || mTotal != Total || mMute != Mute) {
		mCurrent = Current;
		mTotal = Total;
		mMute = Mute;
		SetDirty();
	}
	UpdateUnlock();
}

cxType cText2SkinDisplayVolume::GetTokenData(const txToken &Token) {
	switch (Token.Type) {
	case tVolumeCurrent:
		return mCurrent;

	case tVolumeTotal:
		return mTotal;

	case tIsMute:
		return mMute;

	default:
		return cText2SkinRender::GetTokenData(Token);
	}
}
	
// --- cText2SkinDisplayReplay ------------------------------------------------

cText2SkinDisplayReplay::cText2SkinDisplayReplay(cText2SkinLoader *Loader, bool ModeOnly):
		cText2SkinRender(Loader, ModeOnly ? cxDisplay::replaySmall : cxDisplay::replayInfo),
		mTitle(""),
		mStateInfo(false),
		mPlay(false),
		mForward(false),
		mSpeed(0),
		mCurrent(0),
		mTotal(0),
		mMarks(NULL),
		mPrompt(""),
		mType(mtStatus),
		mText(""),
		mButtonRed(""),
		mButtonGreen(""),
		mButtonYellow(""),
		mButtonBlue("")
{
}

cText2SkinDisplayReplay::~cText2SkinDisplayReplay()
{
}

void cText2SkinDisplayReplay::SetTitle(const char *Title)
{
	UpdateLock();
	if (Title == NULL) Title = "";
	if (mTitle != Title) {
		mTitle = Title;
		SetDirty();
	}
	UpdateUnlock();
}

void cText2SkinDisplayReplay::SetMode(bool Play, bool Forward, int Speed)
{
	Dprintf("SetMode(%d, %d, %d)\n", Play, Forward, Speed);
	UpdateLock();
	if (!mStateInfo || mPlay != Play || mForward != Forward || mSpeed != Speed) {
		mStateInfo = true;
		mPlay = Play;
		mForward = Forward;
		mSpeed = Speed;
		SetDirty();
	}
	UpdateUnlock();
}

void cText2SkinDisplayReplay::SetProgress(int Current, int Total)
{
	UpdateLock();
	if (mCurrent != Current || mTotal != Total) {
		mCurrent = Current;
		mTotal = Total;
		// SetDirty(); TODO: let this cause a display update every frame?
	}
	UpdateUnlock();
}

void cText2SkinDisplayReplay::SetMarks(const cMarks *Marks)
{
	UpdateLock();
	mMarks = Marks;
	SetDirty();
	UpdateUnlock();
}

void cText2SkinDisplayReplay::SetCurrent(const char *Current)
{
	UpdateLock();
	if (Current == NULL) Current = "";
	if (mPosition != Current) {
		mPosition = Current;
		SetDirty();
	}
	UpdateUnlock();
}

void cText2SkinDisplayReplay::SetTotal(const char *Total)
{
	UpdateLock();
	if (Total == NULL) Total = "";
	if (mDuration != Total) {
		mDuration = Total;
		SetDirty();
	}
	UpdateUnlock();
}

void cText2SkinDisplayReplay::SetJump(const char *Jump)
{
	UpdateLock();
	if (Jump == NULL) Jump = "";
	if (mPrompt != Jump) {
		mPrompt = Jump;
		SetDirty();
	}
	UpdateUnlock();
}

void cText2SkinDisplayReplay::SetMessage(eMessageType Type, const char *Text)
{
	UpdateLock();
	if (Text == NULL) Text = "";
	if (mType != Type || mText != Text) {
		mType = Type;
		mText = Text;
		SetDirty();
	}
	UpdateUnlock();
}
	
void cText2SkinDisplayReplay::SetButtons(const char *Red, const char *Green, const char *Yellow,
                                         const char *Blue)
{
	UpdateLock();
	Dprintf("SetButtons(%s, %s, %s, %s)\n", Red, Green, Yellow, Blue);
	if (Red    == NULL) Red    = "";
	if (Green  == NULL) Green  = "";
	if (Yellow == NULL) Yellow = "";
	if (Blue   == NULL) Blue   = "";
	if (mButtonRed != Red || mButtonGreen != Green || mButtonYellow != Yellow || mButtonBlue != Blue) {
		mButtonRed    = Red;
		mButtonGreen  = Green;
		mButtonYellow = Yellow;
		mButtonBlue   = Blue;
		SetDirty();
	}
	UpdateUnlock();
}

cxType cText2SkinDisplayReplay::GetTokenData(const txToken &Token)
{
	switch (Token.Type) {
	case tReplayTitle:
		return mTitle;

	case tReplayPositionIndex:
		return DurationType(mCurrent, Token.Attrib.Text);

	case tReplayDurationIndex:
		return DurationType(mTotal, Token.Attrib.Text);

	case tReplayPosition:
		return mPosition;

	case tReplayDuration:
		return mDuration;

	case tReplayRemaining:
		return DurationType(mTotal - mCurrent, Token.Attrib.Text);

	case tReplayPrompt:
		return mPrompt;

	case tIsPlaying:
		return mStateInfo && mSpeed == -1 && mPlay;
	
	case tIsPausing:
		return mStateInfo && mSpeed == -1 && !mPlay;

	case tIsFastForward:
		if (mStateInfo && mSpeed != -1 && mPlay && mForward) {
			return Token.Attrib.Type == aNumber
			       ? (cxType)(mSpeed == Token.Attrib.Number)
			       : (cxType)true;
		}
		return false;
		
	case tIsFastRewind:
		if (mStateInfo && mSpeed != -1 && mPlay && !mForward) {
			return Token.Attrib.Type == aNumber
			       ? (cxType)(mSpeed == Token.Attrib.Number)
			       : (cxType)true;
		}
		return false;

	case tIsSlowForward:
		if (mStateInfo && mSpeed != -1 && !mPlay && mForward) {
			return Token.Attrib.Type == aNumber
			       ? (cxType)(mSpeed == Token.Attrib.Number)
			       : (cxType)true;
		}
		return false;

	case tIsSlowRewind:
		if (mStateInfo && mSpeed != -1 && !mPlay && !mForward) {
			return Token.Attrib.Type == aNumber
			       ? (cxType)(mSpeed == Token.Attrib.Number)
			       : (cxType)true;
		}
		return false;

	case tMessage:
		return mText;

	case tMessageInfo:
		return mType == mtInfo
		       ? (cxType)mText
		       : (cxType)false;

	case tMessageWarning:
		return mType == mtWarning
		       ? (cxType)mText
		       : (cxType)false;
	
	case tMessageStatus:
		return mType == mtStatus
		       ? (cxType)mText
		       : (cxType)false;
	
	case tMessageError:
		return mType == mtError
		       ? (cxType)mText
		       : (cxType)false;

	case tButtonRed:
		return mButtonRed;

	case tButtonGreen:
		return mButtonGreen;

	case tButtonYellow:
		return mButtonYellow;

	case tButtonBlue:
		return mButtonBlue;

	default:
		return cText2SkinRender::GetTokenData(Token);
	}
}

// --- cText2SkinDisplayMessage -----------------------------------------------

cText2SkinDisplayMessage::cText2SkinDisplayMessage(cText2SkinLoader *Loader):
		cText2SkinRender(Loader, cxDisplay::message),
		mText("") 
{
}

cText2SkinDisplayMessage::~cText2SkinDisplayMessage() 
{
}

void cText2SkinDisplayMessage::SetMessage(eMessageType Type, const char *Text) 
{
	UpdateLock();
	if (Text == NULL) Text = "";
	if (mType != Type || mText != Text) {
		mType = Type;
		mText = Text;
		SetDirty();
	}
	UpdateUnlock();
}

cxType cText2SkinDisplayMessage::GetTokenData(const txToken &Token) 
{
	switch (Token.Type) {
	case tMessage:
		return mText;

	case tMessageInfo:
		return mType == mtInfo
		       ? (cxType)mText
		       : (cxType)false;

	case tMessageWarning:
		return mType == mtWarning
		       ? (cxType)mText
		       : (cxType)false;
	
	case tMessageStatus:
		return mType == mtStatus
		       ? (cxType)mText
		       : (cxType)false;
	
	case tMessageError:
		return mType == mtError
		       ? (cxType)mText
		       : (cxType)false;

	default:
		return cText2SkinRender::GetTokenData(Token);
	}
}

// --- cText2SkinDisplayMenu --------------------------------------------------

cText2SkinDisplayMenu::cText2SkinDisplayMenu(cText2SkinLoader *Loader):
		cText2SkinRender(Loader, cxDisplay::menu),
		mMaxItems(0),
		mTitle(""),
		mButtonRed(""),
		mButtonGreen(""),
		mButtonYellow(""),
		mButtonBlue(""),
		mMessageType(mtStatus),
		mMessageText(""),
		mEvent(NULL),
		mItems(),
		mCurrentItem((uint)-1)
{
	cxDisplay *disp = Loader->Data()->Get(cxDisplay::menu);
	const cxObject *area = NULL;
	for (uint i = 0; i < disp->Objects(); ++i) {
		const cxObject *o = disp->GetObject(i);
		if (disp->GetObject(i)->Type() == cxObject::list) {
			area = o;
			break;
		}
	}

	if (area != NULL) {
		const cxObject *item = area->GetObject(0);
		if (item != NULL && item->Type() == cxObject::item)
			mMaxItems = area->Size().h / item->Size().h;
	}
}

cText2SkinDisplayMenu::~cText2SkinDisplayMenu() 
{
}

void cText2SkinDisplayMenu::Clear(void) 
{
	UpdateLock();
	mItems.clear();
	mCurrentItem = (uint)-1;
	mEvent = NULL;
	mRecording = NULL;
	mText = "";
	cText2SkinRender::Clear();
	SetDirty();
	UpdateUnlock();
}

void cText2SkinDisplayMenu::SetTitle(const char *Title) 
{
	UpdateLock();
	if (Title == NULL) Title = "";
	if (mTitle != Title) {
		mTitle = Title;
		SetDirty();
	}
	UpdateUnlock();
}

void cText2SkinDisplayMenu::SetButtons(const char *Red, const char *Green, const char *Yellow, 
                                       const char *Blue) 
{
	UpdateLock();
	if (Red    == NULL) Red    = "";
	if (Green  == NULL) Green  = "";
	if (Yellow == NULL) Yellow = "";
	if (Blue   == NULL) Blue   = "";
	if (mButtonRed != Red || mButtonGreen != Green || mButtonYellow != Yellow 
			|| mButtonBlue != Blue) {
		mButtonRed    = Red;
		mButtonGreen  = Green;
		mButtonYellow = Yellow;
		mButtonBlue   = Blue;
		SetDirty();
	}
	UpdateUnlock();
}

void cText2SkinDisplayMenu::SetMessage(eMessageType Type, const char *Text) 
{
	UpdateLock();
	if (Text == NULL) Text = "";
	if (mMessageType != Type || mMessageText != Text) {
		mMessageType = Type;
		mMessageText = Text;
		SetDirty();
	}
	UpdateUnlock();
}

void cText2SkinDisplayMenu::SetItem(const char *Text, int Index, bool Current, bool Selectable) 
{
	UpdateLock();
	if (Text == NULL)
		return;

	tListItem *item = new tListItem(Text, Selectable);

	for (int i = 0; i < MaxTabs; ++i) {
		const char *tab = GetTabbedText(Text, i);
		if (tab)
			item->tabs[i] = tab;
		if (!Tab(i + 1))
			break;
	}

	if (mItems.size() <= (uint)Index) {
		mItems.push_back(*item);
		SetDirty();
	}
	else if (mItems[Index] != *item) {
		mItems[Index] = *item;
		SetDirty();
	}
	delete item;

	if (Current && mCurrentItem != (uint)Index) {
		mCurrentItem = Index;
		SetDirty();
	}
	UpdateUnlock();
}

void cText2SkinDisplayMenu::SetEvent(const cEvent *Event) 
{
	UpdateLock();
	if (mEvent != Event) {
		mEvent = Event;
		if (mEvent != NULL)
			SetDirty();
	}
	UpdateUnlock();
}

void cText2SkinDisplayMenu::SetRecording(const cRecording *Recording) 
{
	UpdateLock();
	// yet unused
	if (mRecording != Recording) {
		mRecording = Recording;
		if (mRecording != NULL)
			SetDirty();
	}
	UpdateUnlock();
}

void cText2SkinDisplayMenu::SetText(const char *Text, bool /*FixedFont*/) 
{
	UpdateLock();
	if (Text == NULL) Text = "";
	if (mText != Text) {
		mText = Text;
		SetDirty();
	}
	UpdateUnlock();
}

void cText2SkinDisplayMenu::SetTabs(int Tab1, int Tab2, int Tab3, int Tab4, int Tab5) 
{
	UpdateLock();
	cSkinDisplayMenu::SetTabs(Tab1, Tab2, Tab3, Tab4, Tab5);
	UpdateUnlock();
}

void cText2SkinDisplayMenu::Scroll(bool Up, bool Page) 
{
	UpdateLock();
	cText2SkinRender::Scroll(Up, Page);
	SetDirty();
	UpdateUnlock();
}

cxType cText2SkinDisplayMenu::GetTokenData(const txToken &Token) 
{
	switch (Token.Type) {
	case tMenuItem:
	case tMenuGroup:
		if (Token.Index < 0) return false;
	case tMenuCurrent:
		if (Token.Index >= 0 && Token.Tab == -1) return false;
		break;

	default:
		if (Token.Tab >= 0) return false;
		break;
	}

	switch (Token.Type) {
	case tMenuTitle:
		return mTitle;

	case tMenuItem:
		return mItems.size() > (uint)Token.Index && mItems[Token.Index].sel
		       && mCurrentItem != (uint)Token.Index
		       ? (cxType)mItems[Token.Index].tabs[Token.Tab]
		       : (cxType)false;

	case tIsMenuItem:
		return mItems.size() > (uint)Token.Index && mItems[Token.Index].sel
		       && mCurrentItem != (uint)Token.Index;
	
	case tMenuCurrent:
		if (Token.Index < 0)
			return mItems.size() > mCurrentItem
			       ? (cxType)mItems[mCurrentItem].text
			       : (cxType)false;

		return mItems.size() > (uint)Token.Index && mItems[Token.Index].sel
		       && mCurrentItem == (uint)Token.Index
		       ? (cxType)mItems[Token.Index].tabs[Token.Tab]
		       : (cxType)false;

	case tIsMenuCurrent:
		return mItems.size() > (uint)Token.Index && mItems[Token.Index].sel
		       && mCurrentItem == (uint)Token.Index;

	case tMenuGroup:
		return mItems.size() > (uint)Token.Index && !mItems[Token.Index].sel
		       ? (cxType)mItems[Token.Index].tabs[Token.Tab]
		       : (cxType)false;

	case tIsMenuGroup:
		return mItems.size() > (uint)Token.Index && !mItems[Token.Index].sel;

	case tButtonRed:
		return mButtonRed;

	case tButtonGreen:
		return mButtonGreen;

	case tButtonYellow:
		return mButtonYellow;

	case tButtonBlue:
		return mButtonBlue;

	case tMessage:
		return mMessageText;

	case tMessageInfo:
		return mMessageType == mtInfo
		       ? (cxType)mMessageText
		       : (cxType)false;

	case tMessageWarning:
		return mMessageType == mtWarning
		       ? (cxType)mMessageText
		       : (cxType)false;
	
	case tMessageStatus:
		return mMessageType == mtStatus
		       ? (cxType)mMessageText
		       : (cxType)false;
	
	case tMessageError:
		return mMessageType == mtError
		       ? (cxType)mMessageText
		       : (cxType)false;
	
	case tPresentStartDateTime:
		return mEvent != NULL
		       ? (cxType)TimeType(mEvent->StartTime(), Token.Attrib.Text)
		       : (cxType)false;

	case tPresentVPSDateTime:
		return mEvent != NULL
		       ? (cxType)TimeType(mEvent->Vps(), Token.Attrib.Text)
		       : (cxType)false;

	case tPresentEndDateTime:
		return mEvent != NULL
		       ? (cxType)TimeType(mEvent->EndTime(), Token.Attrib.Text)
		       : (cxType)false;

	case tPresentProgress:
		return mEvent != NULL
		       ? (cxType)DurationType((time(NULL) - mEvent->StartTime()) * FRAMESPERSEC, 
		                              Token.Attrib.Text)
		       : (cxType)false;

	case tPresentDuration:
		return mEvent != NULL
		       ? (cxType)DurationType(mEvent->Duration() * FRAMESPERSEC, Token.Attrib.Text)
		       : (cxType)false;

	case tPresentRemaining:
		return mEvent != NULL
		       ? (cxType)DurationType((mEvent->Duration() - (time(NULL) - mEvent->StartTime()))
		                              * FRAMESPERSEC, Token.Attrib.Text)
		       : (cxType)false;

	case tPresentTitle:
		return mEvent != NULL
		       ? (cxType)mEvent->Title()
		       : (cxType)false;

	case tPresentShortText:
		return mEvent != NULL
		       ? (cxType)mEvent->ShortText()
		       : (cxType)false;

	case tPresentDescription:
		return mEvent != NULL
		       ? (cxType)mEvent->Description()
		       : (cxType)false;
	
	case tHasVPS:
		return mEvent != NULL && mEvent->Vps() != 0;

	case tHasTimer:
		return mEvent != NULL && mEvent->HasTimer();
	
	case tIsRunning:
		return mEvent != NULL && mEvent->IsRunning();

	case tMenuText:
		return mText;

	default:
		return cText2SkinRender::GetTokenData(Token);
	}
}

#if VDRVERSNUM >= 10318
cText2SkinDisplayTracks::cText2SkinDisplayTracks(cText2SkinLoader *Loader, const char *Title,
                                                 int NumTracks, const char * const *Tracks):
		cText2SkinRender(Loader, cxDisplay::audioTracks),
		mTitle(Title),
		mItems(),
		mCurrentItem((uint)-1)
{
}

cText2SkinDisplayTracks::~cText2SkinDisplayTracks()
{
}

void cText2SkinDisplayTracks::SetTrack(int Index, const char * const *Tracks)
{
}

cxType cText2SkinDisplayTracks::GetTokenData(const txToken &Token)
{
	return cText2SkinRender::GetTokenData(Token);
}

#endif
