/*
 * $Id: render.h,v 1.1.1.1 2004/05/23 00:08:03 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_RENDER_H
#define VDR_TEXT2SKIN_RENDER_H

#include <vdr/osd.h>
#include <vdr/skins.h>
#include "data.h"
#include <vector>

using std::vector;

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
	eSkinSection      mSection;
	cOsd             *mOsd;

	// TODO: rename or restructure items
	// channel display
	const cChannel   *mChannel;
	int               mNumber;
	const cEvent     *mPresent;
	const cEvent     *mFollowing;

	// volume display
	int               mVolumeCurrent;
	int               mVolumeTotal;
	bool              mVolumeMute;

	// replay display
	const char       *mReplayTitle;
	bool              mReplayPlay;
	bool              mReplayForward;
	int               mReplaySpeed;
	int               mReplayCurrent;
	int               mReplayTotal;
	const char       *mReplayJump;

	// message display
	eMessageType      mMessageType;
	const char       *mMessageText;

	// menu
	struct MenuItem {
		const char     *mName;
		bool            mSelectable;
		bool operator!=(const MenuItem &b) { return b.mName != mName || b.mSelectable != mSelectable; }
	};
	const char       *mTitle;
	vector<MenuItem>  mItems;
	int               mCurrent;

protected:
	void DisplayBackground(cText2SkinItem *Item); 
	void DisplayLogo(cText2SkinItem *Item); 
	void DisplayText(cText2SkinItem *Item); 
	void DisplayDateTime(cText2SkinItem *Item); 
	void DisplayDate(cText2SkinItem *Item); 
	void DisplayTime(cText2SkinItem *Item); 
	void DisplayChannelNumberName(cText2SkinItem *Item); 
	void DisplayChannelNumber(cText2SkinItem *Item); 
	void DisplayChannelName(cText2SkinItem *Item); 
	void DisplayRectangle(cText2SkinItem *Item); 
	void DisplayEllipse(cText2SkinItem *Item); 
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
	void DisplayProgressbar(cText2SkinItem *Item); 
	void DisplayReplayTitle(cText2SkinItem *Item);
	void DisplayMenuItems(cText2SkinItem *Item);

public:
	cText2SkinRender(cText2SkinData *Data, eSkinSection Section);
	~cText2SkinRender();

	void Flush(void);
};

#endif // VDR_TEXT2SKIN_RENDER_H
