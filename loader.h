/*
 * $Id: loader.h,v 1.4 2004/06/02 20:43:05 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_LOADER_H
#define VDR_TEXT2SKIN_LOADER_H

#include "common.h"
#include <vdr/skins.h>

class cText2SkinData;
class cText2SkinI18n;
class cText2SkinTheme;

class cText2SkinLoader: public cSkin {
private:
	cText2SkinData  *mData;
	cText2SkinI18n  *mI18n;
	cText2SkinTheme *mTheme;
	string           mDescription;

public:
	static void Start(void);
	static void Load(const char *Skin);

	cText2SkinLoader(cText2SkinData *Data, cText2SkinI18n *I18n, cText2SkinTheme *Theme, const string &Skin, const string &Description);
	~cText2SkinLoader();
  
	virtual const char *Description(void) { return mDescription.c_str(); }
  virtual cSkinDisplayChannel *DisplayChannel(bool WithInfo);
  virtual cSkinDisplayMenu *DisplayMenu(void);
  virtual cSkinDisplayReplay *DisplayReplay(bool ModeOnly);
  virtual cSkinDisplayVolume *DisplayVolume(void);
  virtual cSkinDisplayMessage *DisplayMessage(void);
};

#endif // VDR_TEXT2SKIN_LOADER_H
