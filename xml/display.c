/*
 *  $Id: display.c,v 1.1 2004/12/19 22:03:25 lordjaxom Exp $
 */

#include "xml/display.h"

static const std::string DisplayNames[] = 
	{ "channelInfo", "channelSmall", "volume", "message", "replayInfo", "replaySmall", "menu" };

cxDisplay::cxDisplay(cxSkin *parent):
		mType((eType)__COUNT_DISPLAY__),
		mNumWindows(0),
		mSkin(parent) 
{
}

bool cxDisplay::ParseType(const std::string &Text) 
{
	for (int i = 0; i < (int)__COUNT_DISPLAY__; ++i) {
		if (DisplayNames[i].length() > 0 && DisplayNames[i] == Text) {
			mType = (eType)i;
			return true;
		}
	}
	return false;
}

const std::string &cxDisplay::GetType(void) const 
{
	return DisplayNames[mType];
}

cxDisplays::cxDisplays(void)
{
}

cxDisplays::~cxDisplays()
{
	iterator it = begin();
	while (it != end())
		(delete (*it).second, ++it);
}
