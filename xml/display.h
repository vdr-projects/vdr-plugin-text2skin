/*
 *  $Id: display.h,v 1.2 2004/12/28 01:24:35 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_DISPLAY_H 
#define VDR_TEXT2SKIN_DISPLAY_H 

#include "xml/object.h"
#include <string>
#include <map>

class cxSkin;

class cxDisplay {
	friend bool xStartElem(const std::string &name, std::map<std::string,std::string> &attrs);
	friend bool xEndElem(const std::string &name);

public:
	enum eType {
		channelInfo,
		channelSmall,
		volume,
		message,
		replayInfo,
		replaySmall,
		menu,
#define __COUNT_DISPLAY__ (menu + 1)
	};

private:
	eType     mType;
	txWindow  mWindows[MAXOSDAREAS];
	int       mNumWindows;
	int       mNumMarquees;
	cxObjects mObjects;
	cxSkin   *mSkin;

public:
	cxDisplay(cxSkin *parent);

	bool ParseType(const std::string &Text);
	const std::string &GetType(void) const;

	eType            Type(void)       const { return mType; }
	const txWindow  *Windows(void)    const { return mWindows; }
	int              NumWindows(void) const { return mNumWindows; }
	cxSkin          *Skin(void)       const { return mSkin; }

	uint             Objects(void)    const { return mObjects.size(); }
	const cxObject  *GetObject(int n) const { return mObjects[n]; }
};

class cxDisplays: public std::map<cxDisplay::eType,cxDisplay*> {
public:
	cxDisplays(void);
	~cxDisplays();
};

#endif // VDR_TEXT2SKIN_DISPLAY_H 
