/*
 * $Id: render.h,v 1.12 2004/06/02 20:43:05 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_RENDER_H
#define VDR_TEXT2SKIN_RENDER_H

#include "common.h"
#include "data.h"
#include "i18n.h"
#include "theme.h"
#include <vdr/osd.h>
#include <vdr/skins.h>

class cChannel;
class cEvent;

class cText2SkinRender {
	friend class cText2SkinDisplayChannel;
	friend class cText2SkinDisplayVolume;
	friend class cText2SkinDisplayReplay;
	friend class cText2SkinDisplayMessage;
	friend class cText2SkinDisplayMenu;

private:
	cText2SkinData   *mData;
	cText2SkinI18n   *mI18n;
	cText2SkinTheme  *mTheme;
	eSkinSection      mSection;
	cOsd             *mOsd;

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
		string          name;
		bool            sel;
		bool operator!=(const MenuItem &b) { return b.name != name || b.sel != sel; }
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

protected:
	// Basic operations
	void DrawBackground(const POINT &Pos, const SIZE &Size, const tColor *Bg, const tColor *Fg, const string &Path);
	void DrawImage(const POINT &Pos, const SIZE &Size, const tColor *Bg, const tColor *Fg, const string &Path);
	void DrawText(const POINT &Pos, const SIZE &Size, const tColor *Fg, const string &Text, const cFont *Font, int Align);
	void DrawRectangle(const POINT &Pos, const SIZE &Size, const tColor *Fg);
	void DrawEllipse(const POINT &Pos, const SIZE &Size, const tColor *Fg, int Arc);
	void DrawSlope(const POINT &Pos, const SIZE &Size, const tColor *Fg, int Arc);
	void DrawProgressbar(const POINT &Pos, const SIZE &Size, int Current, int Total, const tColor *Fg, const tColor *Bg, const cMarks *Marks = NULL);
 	void DrawMark(const POINT &Pos, const SIZE &Size, bool Start, bool Current, bool Horizontal);

	// High-level operations
	void DisplayBackground(cText2SkinItem *Item); 
	void DisplayChannelLogo(cText2SkinItem *Item); 
	void DisplayLanguage(cText2SkinItem *Item); 
	void DisplayText(cText2SkinItem *Item); 
	void DisplayImage(cText2SkinItem *Item); 
	void DisplayDateTime(cText2SkinItem *Item); 
	void DisplayDate(cText2SkinItem *Item); 
	void DisplayTime(cText2SkinItem *Item); 
	void DisplayChannelNumberName(cText2SkinItem *Item); 
	void DisplayChannelNumber(cText2SkinItem *Item); 
	void DisplayChannelName(cText2SkinItem *Item); 
	void DisplayRectangle(cText2SkinItem *Item); 
	void DisplayEllipse(cText2SkinItem *Item); 
	void DisplaySlope(cText2SkinItem *Item); 
	void DisplayTimebar(cText2SkinItem *Item); 
	void DisplayPresentTime(cText2SkinItem *Item); 
	void DisplayPresentTitle(cText2SkinItem *Item); 
	void DisplayPresentShortText(cText2SkinItem *Item); 
	void DisplayFollowingTime(cText2SkinItem *Item); 
	void DisplayFollowingTitle(cText2SkinItem *Item); 
	void DisplayFollowingShortText(cText2SkinItem *Item); 
	void DisplaySymbol(cText2SkinItem *Item);
	void DisplayVolumebar(cText2SkinItem *Item); 
	void DisplayMute(cText2SkinItem *Item); 
	void DisplayReplaybar(cText2SkinItem *Item); 
	void DisplayReplayTitle(cText2SkinItem *Item);
	void DisplayReplayCurrent(cText2SkinItem *Item);
	void DisplayReplayTotal(cText2SkinItem *Item);
	void DisplayReplayJump(cText2SkinItem *Item);
	void DisplayMessage(cText2SkinItem *Item);
	void DisplayMenuItems(cText2SkinItem *Item);
	void DisplayMenuTitle(cText2SkinItem *Item);
	void DisplayMenuColorbutton(cText2SkinItem *Item);
	void DisplayMenuMessage(cText2SkinItem *Item);

	// Helpers
	string ItemText(cText2SkinItem *Item);
	string ItemText(cText2SkinItem *Item, const string &Content);
	tColor *ItemFg(cText2SkinItem *Item);
	tColor *ItemBg(cText2SkinItem *Item);

public:
	cText2SkinRender(cText2SkinData *Data, cText2SkinI18n *I18n, cText2SkinTheme *Theme, eSkinSection Section);
	~cText2SkinRender();

	void Flush(void);
};

#endif // VDR_TEXT2SKIN_RENDER_H
