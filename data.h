/*
 * $Id: data.h,v 1.11 2004/06/05 01:39:36 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_DATA_H
#define VDR_TEXT2SKIN_DATA_H

#include "common.h"
#include "file.h"
#include <vdr/tools.h>
#include <vdr/osd.h>
#include <vdr/config.h>

// sections and items known by skin files

enum eSkinSection {
	sectionSkin,
	sectionChannelSmall,
	sectionChannel,
	sectionVolume,
	sectionReplayMode,
	sectionReplay,
	sectionMessage,
	sectionMenu,
	__SECTION_COUNT__
};

enum eSkinItem {
	itemUnknown,
	itemSkin, // item identifying the Skin itself
	itemBackground,
	itemText,
	itemImage,
	itemRectangle,
	itemEllipse,
	itemSlope,
	itemDateTime,
	itemDate,
	itemTime,
	itemChannelLogo,
	itemChannelNumberName,
	itemChannelNumber,
	itemChannelName,
	itemLanguage,
	itemTimebar,
	itemPresentTime,
	itemPresentTitle,
	itemPresentShortText,
	itemFollowingTime,
	itemFollowingTitle,
	itemFollowingShortText,
	itemSymbolTeletext,
	itemSymbolAudio,
	itemSymbolDolby,
	itemSymbolEncrypted,
	itemSymbolRecording,
	itemSymbolRadio,
	itemVolumebar,
	itemMute,
	itemReplaybar,
	itemReplayTitle,
	itemReplayCurrent,
	itemReplayTotal,
	itemReplayJump,
	itemSymbolPlay,
	itemSymbolPause,
	itemSymbolFastFwd,
	itemSymbolFastRew,
	itemSymbolSlowFwd,
	itemSymbolSlowRew,
	itemMessageStatus,
	itemMessageInfo,
	itemMessageWarning,
	itemMessageError,
	itemMenuArea,
	itemMenuItem,
	itemMenuCurrent,
	itemMenuTitle,
	itemMenuRed,
	itemMenuGreen,
	itemMenuYellow,
	itemMenuBlue,
	itemMenuText,
	itemSymbolScrollUp,
	itemSymbolScrollDown,
	itemMenuEventTitle,
	itemMenuEventShortText,
	itemMenuEventDescription,
	itemMenuEventTime,
	itemSymbolEventRunning,
	itemSymbolEventTimer,
	itemSymbolEventVPS,
	itemMenuRecording,
	__ITEM_COUNT__
};

struct POINT {
	int x, y;
};

struct SIZE {
	int w, h;
};

class cText2SkinItem {
	friend class cText2SkinRender;

private:
	eSkinItem       mItem;
	POINT           mPos;
	SIZE            mSize;
	int             mBpp;
	int             mArc;
	string          mFg;
	string          mBg;
	const cFont    *mFont;
	string          mName;
	string          mVersion;
	string          mPath;
	string          mAltPath;
	string          mText;
	string          mType;
	eTextAlignment  mAlign;

protected:
	bool ParseItem(const char *Text);

public:
	cText2SkinItem(void);
	~cText2SkinItem();

	bool Parse(const char *Text);

	eSkinItem       Item(void)    const { return mItem; }
	const POINT    &Pos(void)     const { return mPos; }
	const SIZE     &Size(void)    const { return mSize; }
	int             Bpp(void)     const { return mBpp; }
	int             Arc(void)     const { return mArc; }
	const string   &Fg(void)      const { return mFg; }
	const string   &Bg(void)      const { return mBg; }
	const cFont    *Font(void)    const { return mFont; }
	const string   &Name(void)    const { return mName; }
	const string   &Version(void) const { return mVersion; }
	const string   &Path(void)    const { return mPath; }
	const string   &AltPath(void) const { return mAltPath; }
	const string   &Text(void)    const { return mText; }
	const string   &Type(void)    const { return mType; }
	eTextAlignment  Align(void)   const { return mAlign; }
};

class cText2SkinData: public cText2SkinFile {
public:
	typedef vector<cText2SkinItem*> tSection;
	typedef tSection::iterator      tIterator;

private:
	eSkinSection mCurrentSection;
	tSection mSections[__SECTION_COUNT__];

protected:
	virtual bool Parse(const char *Text);

public:
	cText2SkinData(const char *Skin);
	virtual ~cText2SkinData();

	tIterator First(eSkinSection Section) { return mSections[Section].begin(); }
	tIterator Last(eSkinSection Section) { return mSections[Section].end(); }

	cText2SkinItem *Get(eSkinSection Section, eSkinItem Item);
};

#endif // VDR_TEXT2SKIN_DATA_H
