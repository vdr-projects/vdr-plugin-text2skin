/*
 * $Id: setup.c,v 1.4 2004/06/18 16:08:11 lordjaxom Exp $
 */

#include "setup.h"
#include "bitmap.h"

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

// --- cText2SkinSetupPage ----------------------------------------------------

cText2SkinSetupPage::cText2SkinSetupPage(void) {
	mData = Text2SkinSetup;
	Add(new cOsdItem(tr("Flush image cache"), osUser1));
	Add(new cMenuEditIntItem(tr("Max. image cache size"), &mData.MaxCacheFill));
}

cText2SkinSetupPage::~cText2SkinSetupPage() {
}

void cText2SkinSetupPage::Store(void) {
	SetupStore("MaxCacheFill", mData.MaxCacheFill);
	Text2SkinSetup = mData;
}

eOSState cText2SkinSetupPage::ProcessKey(eKeys Key) {
	eOSState state = cMenuSetupPage::ProcessKey(Key);
	if (state == osUser1) {
		Skins.Message(mtInfo, tr("Flushing image cache..."));
		cText2SkinBitmap::FlushCache();
		Skins.Message(mtInfo, NULL);
		return osContinue;
	}
	return state;
}

