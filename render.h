/*
 * $Id: render.h,v 1.26 2004/07/13 13:52:51 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_RENDER_H
#define VDR_TEXT2SKIN_RENDER_H

#include "common.h"
#include "data.h"
#include <vdr/osd.h>
#include <vdr/skins.h>
#include <vdr/thread.h>

class cChannel;
class cEvent;
class cText2SkinLoader;
class cText2SkinData;
class cText2SkinI18n;
class cText2SkinTheme;
class cText2SkinScroller;
class cText2SkinScreen;

class cText2SkinRender: public cThread {
	friend class cText2SkinDisplayChannel;
	friend class cText2SkinDisplayVolume;
	friend class cText2SkinDisplayReplay;
	friend class cText2SkinDisplayMessage;
	friend class cText2SkinDisplayMenu;

private:
	static cText2SkinRender *mRender;

	cText2SkinData     *mData;
	cText2SkinI18n     *mI18n;
	cText2SkinTheme    *mTheme;
	eSkinSection        mSection;
	cText2SkinScreen   *mScreen;
	cText2SkinScroller *mScroller;

	// channel display
	const cChannel   *mChannel;
	int               mChannelNumber;
	const cEvent     *mChannelPresent;
	const cEvent     *mChannelFollowing;

	// volume display
	int               mVolumeCurrent;
	int               mVolumeTotal;
	bool              mVolumeMute;

	// replay display
	string            mReplayTitle;
	bool              mReplayInfo;
	bool              mReplayPlay;
	bool              mReplayForward;
	int               mReplaySpeed;
	int               mReplayCurrent;
	int               mReplayTotal;
	const cMarks     *mReplayMarks;
	string            mReplayCurrentText;
	string            mReplayTotalText;
	string            mReplayJump;

	// message display
	eMessageType      mMessageType;
	string            mMessageText;

	// menu
	struct MenuItem {
		string          text;
		string          tabs[cSkinDisplayMenu::MaxTabs];
		bool            sel;
		bool operator!=(const MenuItem &b) { return b.text != text || b.sel != sel; }
	};
	string            mMenuTitle;
	vector<MenuItem>  mMenuItems;
	int               mMenuCurrent;
	string            mMenuRed;
	string            mMenuGreen;
	string            mMenuYellow;
	string            mMenuBlue;
	const cEvent     *mMenuEvent;
	const cRecording *mMenuRecording;
	string            mMenuText;
	bool              mMenuTextFixedFont;
	bool              mMenuScroll;
	bool              mMenuScrollUp;
	bool              mMenuScrollPage;
	int               mMenuTabs[cSkinDisplayMenu::MaxTabs];
	
	// update thread
	bool              mActive;
	cCondVar          mDoUpdate;
	cCondVar          mStarted;
	cMutex            mMutex;
	int               mUpdateIn;

	// coordinate transformation
	eBaseCoordinate   mBase;
	SIZE              mBaseSize;
	
protected:
	// Update thread
	void Lock(void) { mMutex.Lock(); }
	void Unlock(void) { mMutex.Unlock(); }
	virtual void Action(void);

	// Basic operations
	void DrawBackground(const POINT &Pos, const SIZE &Size, const tColor *Bg, const tColor *Fg, int Alpha, const string &Path);
	void DrawImage(const POINT &Pos, const SIZE &Size, const tColor *Bg, const tColor *Fg, int Alpha, const string &Path);
	void DrawText(const POINT &Pos, const SIZE &Size, const tColor *Fg, const string &Text, const cFont *Font, int Align);
	void DrawRectangle(const POINT &Pos, const SIZE &Size, const tColor *Fg);
	void DrawEllipse(const POINT &Pos, const SIZE &Size, const tColor *Fg, int Arc);
	void DrawSlope(const POINT &Pos, const SIZE &Size, const tColor *Fg, int Arc);
	void DrawProgressbar(const POINT &Pos, const SIZE &Size, int Current, int Total, const tColor *Fg, const tColor *Bg, const tColor *Selected, const tColor *Mark, const tColor *Cur, const cMarks *Marks = NULL);
 	void DrawMark(const POINT &Pos, const SIZE &Size, bool Start, bool Current, bool Horizontal, const tColor *Mark, const tColor *Cur);
	void DrawScrolltext(const POINT &Pos, const SIZE &Size, const tColor *Fg, const string &Text, const cFont *Font, int Align);
	void DrawScrollbar(const POINT &Pos, const SIZE &Size, int Offset, int Shown, int Total, const tColor *Bg, const tColor *Fg);

	// displays a full item
	void DisplayItem(cText2SkinItem *Item, const tItemData *Data = NULL);

	// High-level operations
	void DisplayDateTime(cText2SkinItem *Item);
	void DisplayChannelNumberName(cText2SkinItem *Item);
	void DisplayPresentTime(cText2SkinItem *Item); 
	void DisplayPresentIcon(cText2SkinItem *Item); 
	void DisplayPresentText(cText2SkinItem *Item); 
	void DisplayFollowingTime(cText2SkinItem *Item); 
	void DisplayFollowingTitle(cText2SkinItem *Item); 
	void DisplayFollowingShortText(cText2SkinItem *Item); 
	void DisplayLanguage(cText2SkinItem *Item); 
	void DisplayChannelIcon(cText2SkinItem *Item);
	void DisplayVolume(cText2SkinItem *Item); 
	void DisplayMuteIcon(cText2SkinItem *Item); 
	void DisplayReplayTime(cText2SkinItem *Item);
	void DisplayReplayTitle(cText2SkinItem *Item);
	void DisplayReplayPrompt(cText2SkinItem *Item);
	void DisplayReplaySymbol(cText2SkinItem *Item);
	void DisplayReplayMode(cText2SkinItem *Item);
	void DisplayMessage(cText2SkinItem *Item);
	void DisplayMenuTitle(cText2SkinItem *Item);
	void DisplayMenuButton(cText2SkinItem *Item);
	void DisplayMenuText(cText2SkinItem *Item);
	void DisplayMenuScrollIcon(cText2SkinItem *Item);
	void DisplayMenuItems(cText2SkinItem *Item);

	// Helpers
	string ItemText(cText2SkinItem *Item);
	string ItemText(cText2SkinItem *Item, const string &Content);
	int GetEditableWidth(MenuItem Item, bool Current);
	void Update(void);

public:
	cText2SkinRender(cText2SkinLoader *Loader, eSkinSection Section);
	virtual ~cText2SkinRender();

	static POINT Transform(const POINT &Pos);
	static bool ItemColor(const string &Color, tColor &Result);

	void Flush(void) { Lock(); mDoUpdate.Broadcast(); Unlock(); }
};

#endif // VDR_TEXT2SKIN_RENDER_H
