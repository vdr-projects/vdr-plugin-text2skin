/*
 * $Id: theme.h,v 1.1 2004/06/02 20:43:05 lordjaxom Exp $
 */ 

#ifndef VDR_TEXT2SKIN_THEME_H
#define VDR_TEXT2SKIN_THEME_H

#include "common.h"
#include "file.h"
#include <vdr/themes.h>

class cText2SkinTheme: public cText2SkinFile {
private:
	cTheme          mTheme;
	map<string,int> mMap;

protected:
	bool Parse(const char *Text);

public:
	cText2SkinTheme(const char *Skin);
	virtual ~cText2SkinTheme();

	cTheme *Theme(void) { return &mTheme; }
	tColor Color(const string &Name);
};

inline tColor cText2SkinTheme::Color(const string &Name) {
	if (mMap.find(Name) != mMap.end())
		return mTheme.Color(mMap[Name]);
	else
		return 0x00000000;
}

#endif // VDR_TEXT2SKIN_THEME_H
