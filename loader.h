/*
 * $Id: loader.h,v 1.1.1.1 2004/05/23 00:08:03 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_LOADER_H
#define VDR_TEXT2SKIN_LOADER_H

#include <vdr/skins.h>

class cText2SkinData;

class cText2SkinLoader: public cSkin {
private:
	cText2SkinData *mData;
	const char *mDescription;

public:
	static void Start(void);
	static void Load(const char *Skin);

	cText2SkinLoader(cText2SkinData *Data, const char *Skin, const char *Description);
	~cText2SkinLoader();
  
	virtual const char *Description(void) { return mDescription; };
  virtual cSkinDisplayChannel *DisplayChannel(bool WithInfo);
  virtual cSkinDisplayMenu *DisplayMenu(void);
  virtual cSkinDisplayReplay *DisplayReplay(bool ModeOnly);
  virtual cSkinDisplayVolume *DisplayVolume(void);
  virtual cSkinDisplayMessage *DisplayMessage(void);
};

#endif // VDR_TEXT2SKIN_LOADER_H
