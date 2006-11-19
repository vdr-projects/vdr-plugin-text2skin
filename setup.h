/*
 * $Id: setup.h,v 1.1 2004/12/19 22:03:19 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_SETUP_H
#define VDR_TEXT2SKIN_SETUP_H

#include "common.h"

class cText2SkinSetup {
public:
	cText2SkinSetup(void);

	bool SetupParse(const char *Name, const char *Value);
	
	int MenuScrollbar;
	int MarqueeLeftRight;
	int MarqueeReset;
#if VDRVERSNUM >= 10344
	int ShowAux;
	int StripAux;
#endif
#if VDRVERSNUM >= 10330
	int CheckTimerConflict;
#endif
	int MaxCacheFill;
};

extern cText2SkinSetup Text2SkinSetup;

#endif // VDR_TEXT2SKIN_SETUP_H
