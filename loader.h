/*
 * $Id: loader.h,v 1.3 2004/05/31 19:54:12 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_LOADER_H
#define VDR_TEXT2SKIN_LOADER_H

#define __STL_CONFIG_H
#include <vdr/skins.h>
#undef __STL_CONFIG_H
#include <string>

using std::string;

class cText2SkinData;

class cText2SkinLoader: public cSkin {
private:
	cText2SkinData *mData;
	string mDescription;

public:
	static void Start(void);
	static void Load(const char *Skin);

	cText2SkinLoader(cText2SkinData *Data, const string &Skin, const string &Description);
	~cText2SkinLoader();
  
	virtual const char *Description(void) { return mDescription.c_str(); }
  virtual cSkinDisplayChannel *DisplayChannel(bool WithInfo);
  virtual cSkinDisplayMenu *DisplayMenu(void);
  virtual cSkinDisplayReplay *DisplayReplay(bool ModeOnly);
  virtual cSkinDisplayVolume *DisplayVolume(void);
  virtual cSkinDisplayMessage *DisplayMessage(void);
};

#endif // VDR_TEXT2SKIN_LOADER_H
