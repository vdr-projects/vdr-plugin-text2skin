/*
 * $Id: setup.c,v 1.1 2004/12/19 22:03:19 lordjaxom Exp $
 */

#include "setup.h"

cText2SkinSetup Text2SkinSetup;

// --- cText2SkinSetup --------------------------------------------------------

cText2SkinSetup::cText2SkinSetup(void) {
	MenuScrollbar = false;
	MarqueeLeftRight = true;
	MarqueeReset = false;
#if VDRVERSNUM >= 10344
	ShowAux = true;
	StripAux = true;
#endif
#if VDRVERSNUM >= 10330
	CheckTimerConflict = false;
#endif
	MaxCacheFill = 25;
}

bool cText2SkinSetup::SetupParse(const char *Name, const char *Value) {
	if      (strcmp(Name, "MenuScrollbar") == 0) MenuScrollbar = atoi(Value);
	else if (strcmp(Name, "MarqueeLeftRight") == 0) MarqueeLeftRight = atoi(Value);
	else if (strcmp(Name, "MarqueeReset") == 0) MarqueeReset = atoi(Value);
#if VDRVERSNUM >= 10344
	else if (strcmp(Name, "ShowAux") == 0) ShowAux = atoi(Value);
	else if (strcmp(Name, "StripAux") == 0) StripAux = atoi(Value);
#endif
#if VDRVERSNUM >= 10330
	else if (strcmp(Name, "CheckTimerConflict") == 0) CheckTimerConflict = atoi(Value);
#endif
	else if (strcmp(Name, "MaxCacheFill") == 0) MaxCacheFill = atoi(Value);
	else return false;
	return true;
}

