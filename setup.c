/*
 * $Id: setup.c,v 1.1 2004/06/07 19:09:20 lordjaxom Exp $
 */

#include "setup.h"
#include "bitmap.h"

cText2SkinSetupPage::cText2SkinSetupPage(void) {
	Add(new cOsdItem(tr("Flush image cache"), osUser1));
}

cText2SkinSetupPage::~cText2SkinSetupPage() {
}

eOSState cText2SkinSetupPage::ProcessKey(eKeys Key) {
	eOSState state = cMenuSetupPage::ProcessKey(Key);
	if (state == osUser1) {
		Skins.Message(mtInfo, tr("Flushing image cache..."));
		cText2SkinBitmap::FlushCache();
		return osContinue;
	}
	return state;
}
