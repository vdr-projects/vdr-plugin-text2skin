/*
 * $Id: setup.c,v 1.1.1.1 2004/11/19 16:45:31 lordjaxom Exp $
 */

#include "setup.h"

cText2SkinSetup Text2SkinSetup;

// --- cText2SkinSetup --------------------------------------------------------

cText2SkinSetup::cText2SkinSetup(void) {
	MaxCacheFill = 25;
}

bool cText2SkinSetup::SetupParse(const char *Name, const char *Value) {
	if (strcmp(Name, "MaxCacheFill") == 0) MaxCacheFill = atoi(Value);
	else return false;
	return true;
}

