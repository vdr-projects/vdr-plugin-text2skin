/*
 *  $Id: skin.c,v 1.2 2004/12/08 17:13:26 lordjaxom Exp $
 */

#include "xml/skin.h"
#include <vdr/tools.h>
#include <vdr/config.h>

const std::string ScreenBases[] = { "relative", "absolute" };

cxSkin::cxSkin(const std::string &Name):
		mName(Name) {
}

void cxSkin::SetBase(eScreenBase Base) {
	mBase = Base;
	switch (mBase) {
	case relative:
		mBaseOffset = txPoint(Setup.OSDLeft, Setup.OSDTop);
		mBaseSize   = txSize(Setup.OSDWidth, Setup.OSDHeight);
		break;

	case absolute:
		mBaseOffset = txPoint(0, 0);
		mBaseSize   = txSize(720, 576); //XXX
		break;

	default:
		break;
	}
}

bool cxSkin::ParseBase(const std::string &Text) {
	int i;
	for (i = 0; i < (int)__COUNT_BASE__; ++i) {
		if (ScreenBases[i] == Text)
			break;
	}
	if (i < (int)__COUNT_BASE__) {
		SetBase((eScreenBase)i);
		return true;
	}
	return false;
}
