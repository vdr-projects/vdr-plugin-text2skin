/*
 * $Id: data.h,v 1.9 2004/05/31 19:54:12 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_DATA_H
#define VDR_TEXT2SKIN_DATA_H

#define __STL_CONFIG_H
#include <vdr/tools.h>
#include <vdr/osd.h>
#include <vdr/config.h>
#undef __STL_CONFIG_H
#include <string>

using std::string;

// sections and items known by skin files

enum eSkinSection {
	sectionUnknown,
	sectionChannelSmall,
	sectionChannel,
	sectionVolume,
	sectionReplayMode,
	sectionReplay,
	sectionMessage,
	sectionMenu,
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
};

struct POINT {
	int x, y;
};

struct SIZE {
	int w, h;
};

class cText2SkinItem: public cListObject {
	friend class cText2SkinRender;

private:
	static eSkinSection mParseSection;

	eSkinSection    mSection;
	eSkinItem       mItem;
	POINT           mPos;
	SIZE            mSize;
	int             mBpp;
	int             mArc;
	tColor         *mFg;
	tColor         *mBg;
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
	bool ParseVar(const char *Text, const char *Name, int *Value);
	bool ParseVar(const char *Text, const char *Name, const cFont **Value);
	bool ParseVar(const char *Text, const char *Name, string &Value);
	bool ParseVar(const char *Text, const char *Name, tColor **Value);
	bool ParseVar(const char *Text, const char *Name, eTextAlignment *Value);

public:
	cText2SkinItem(void);
	~cText2SkinItem();

	bool Parse(const char *Text);

	eSkinSection    Section(void) const { return mSection; }
	eSkinItem       Item(void)    const { return mItem; }
	const POINT    &Pos(void)     const { return mPos; }
	const SIZE     &Size(void)    const { return mSize; }
	int             Bpp(void)     const { return mBpp; }
	int             Arc(void)     const { return mArc; }
	const tColor   *Fg(void)      const { return mFg; }
	const tColor   *Bg(void)      const { return mBg; }
	const cFont    *Font(void)    const { return mFont; }
	const string   &Name(void)    const { return mName; }
	const string   &Version(void) const { return mVersion; }
	const string   &Path(void)    const { return mPath; }
	const string   &AltPath(void) const { return mAltPath; }
	const string   &Text(void)    const { return mText; }
	const string   &Type(void)    const { return mType; }
	eTextAlignment  Align(void)   const { return mAlign; }
};

class cText2SkinData: public cConfig<cText2SkinItem> {
private:
	char *mSkin;

public:
	cText2SkinData(const char *Skin);
	~cText2SkinData();

	cText2SkinItem *Get(eSkinItem Item);

	const char *Skin(void) const { return mSkin; }
};

#endif // VDR_TEXT2SKIN_DATA_H
