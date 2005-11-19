/*
 * $Id: setup.c,v 1.1 2004/12/19 22:03:19 lordjaxom Exp $
 */

#include "setup.h"

cText2SkinSetup Text2SkinSetup;

// --- cText2SkinSetup --------------------------------------------------------

cText2SkinSetup::cText2SkinSetup(void) {
	MarqueeLeftRight = true;
	MarqueeReset = false;
	CheckTimerConflict = false;
	MaxCacheFill = 25;
}

bool cText2SkinSetup::SetupParse(const char *Name, const char *Value) {
	if      (strcmp(Name, "MarqueeLeftRight") == 0) MarqueeLeftRight = atoi(Value);
	else if (strcmp(Name, "MarqueeReset") == 0) MarqueeReset = atoi(Value);
	else if (strcmp(Name, "CheckTimerConflict") == 0) CheckTimerConflict = atoi(Value);
	else if (strcmp(Name, "MaxCacheFill") == 0) MaxCacheFill = atoi(Value);
	else return false;
	
	return true;
}

