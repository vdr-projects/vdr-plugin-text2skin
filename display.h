/*
 * $Id: display.h,v 1.5 2004/06/05 18:04:29 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_SKIN_H
#define VDR_TEXT2SKIN_SKIN_H

#include "common.h"
#include <vdr/skins.h>

class cText2SkinData;
class cText2SkinI18n;
class cText2SkinRender;

class cText2SkinDisplayChannel: public cSkinDisplayChannel {
private:
	bool              mWithInfo;
	cText2SkinRender *mRender;
	bool              mDirty;

public:
	cText2SkinDisplayChannel(cText2SkinData *Data, cText2SkinI18n *I18n, cText2SkinTheme *Theme, bool WithInfo);
	virtual ~cText2SkinDisplayChannel();

	virtual void SetChannel(const cChannel *Channel, int Number);
	virtual void SetEvents(const cEvent *Present, const cEvent *Following);
  virtual void SetMessage(eMessageType Type, const char *Text);
	virtual void Flush(void);
};

class cText2SkinDisplayVolume: public cSkinDisplayVolume {
private:
	cText2SkinRender *mRender;
	bool              mDirty;

public:
	cText2SkinDisplayVolume(cText2SkinData *Data, cText2SkinI18n *I18n, cText2SkinTheme *Theme);
	virtual ~cText2SkinDisplayVolume();
	virtual void SetVolume(int Current, int Total, bool Mute);
	virtual void Flush(void);
};

class cText2SkinDisplayReplay: public cSkinDisplayReplay {
private:
	cText2SkinRender *mRender;
	bool              mDirty;
public:
	cText2SkinDisplayReplay(cText2SkinData *Data, cText2SkinI18n *I18n, cText2SkinTheme *Theme, bool ModeOnly);
	virtual ~cText2SkinDisplayReplay();
	virtual void SetTitle(const char *Title);
	virtual void SetMode(bool Play, bool Forward, int Speed);
	virtual void SetProgress(int Current, int Total);
	virtual void SetMarks(const cMarks *Marks);
	virtual void SetCurrent(const char *Current);
	virtual void SetTotal(const char *Total);
	virtual void SetJump(const char *Jump);
  virtual void SetMessage(eMessageType Type, const char *Text);
	virtual void Flush(void);
};

class cText2SkinDisplayMessage: public cSkinDisplayMessage {
private:
	cText2SkinRender *mRender;
	bool              mDirty;

public:
	cText2SkinDisplayMessage(cText2SkinData *Data, cText2SkinI18n *I18n, cText2SkinTheme *Theme);
	virtual ~cText2SkinDisplayMessage();
	virtual void SetMessage(eMessageType Type, const char *Text);
	virtual void Flush(void);
};

class cText2SkinDisplayMenu: public cSkinDisplayMenu {
private:
	cText2SkinRender *mRender;
	bool              mDirty;
	int               mMaxItems;

public:
	cText2SkinDisplayMenu(cText2SkinData *Data, cText2SkinI18n *I18n, cText2SkinTheme *Theme);
	virtual ~cText2SkinDisplayMenu();

  virtual int MaxItems(void) { return mMaxItems; }
  virtual void Clear(void);
	virtual void SetTitle(const char *Title);
	virtual void SetButtons(const char *Red, const char *Green, const char *Yellow, const char *Blue);
	virtual void SetMessage(eMessageType Type, const char *Text);
	virtual void SetItem(const char *Text, int Index, bool Current, bool Selectable);
	virtual void SetEvent(const cEvent *Event);
	virtual void SetRecording(const cRecording *Recording);
	virtual void SetText(const char *Text, bool FixedFont);
	virtual void SetTabs(int Tab1, int Tab2, int Tab3, int Tab4, int Tab5);
	virtual void Scroll(bool Up, bool Page);
	virtual void Flush(void);
};

#endif // VDR_TEXT2SKIN_SKIN_H
