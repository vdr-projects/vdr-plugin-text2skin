/*
 * $Id: data.h,v 1.1.1.1 2004/05/23 00:08:03 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_DATA_H
#define VDR_TEXT2SKIN_DATA_H

#include <vdr/tools.h>
#include <vdr/osd.h>
#include <vdr/config.h>

// sections and items known by skin files

enum eSkinSection {
	sectionUnknown,
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
	itemLogo,
	itemText,
	itemDateTime,
	itemDate,
	itemTime,
	itemChannelNumberName,
	itemChannelNumber,
	itemChannelName,
	itemRectangle,
	itemEllipse,
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
	itemVolumebar,
	itemMute,
	itemProgressbar,
	itemReplayTitle,
	itemMenuArea,
	itemMenuItem,
	itemMenuCurrent
};

class cText2SkinItem: public cListObject {
	friend class cText2SkinRender;

private:
	static eSkinSection mParseSection;

	eSkinSection    mSection;
	eSkinItem       mItem;
	int             mX, mY;
	int             mWidth, mHeight;
	int             mBpp;
	tColor         *mFg;
	tColor         *mBg;
	char           *mName;
	char           *mVersion;
	char           *mFont;
	char           *mPath;
	char           *mAltPath;
	char           *mText;
	eTextAlignment  mAlign;

protected:
	bool ParseItem(const char *Text);
	bool ParseVar(const char *Text, const char *Name, int *Value);
	bool ParseVar(const char *Text, const char *Name, char **Value);
	bool ParseVar(const char *Text, const char *Name, tColor **Value);
	bool ParseVar(const char *Text, const char *Name, eTextAlignment *Value);

public:
	cText2SkinItem(void);
	~cText2SkinItem();

	bool Parse(const char *Text);

	eSkinSection    Section(void) const { return mSection; }
	eSkinItem       Item(void)    const { return mItem; }
	int             X(void)       const { return mX; }
	int             Y(void)       const { return mY; }
	int             Width(void)   const { return mWidth; }
	int             Height(void)  const { return mHeight; }
	int             Bpp(void)     const { return mBpp; }
	bool            HasFg(void)   const { return mFg != NULL; }
	tColor          Fg(void)      const { return mFg ? *mFg : 0xFFFFFFFF; }
	bool            HasBg(void)   const { return mBg != NULL; }
	tColor          Bg(void)      const { return mBg ? *mBg : 0xFF000000; }
	const char     *Name(void)    const { return mName; }
	const char     *Version(void) const { return mVersion; }
	const char     *Font(void)    const { return mFont; }
	const char     *Path(void)    const { return mPath; }
	const char     *AltPath(void) const { return mAltPath; }
	const char     *Text(void)    const { return mText; }
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
