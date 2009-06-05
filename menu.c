/*
 * $Id: menu.c,v 1.1 2004/12/19 22:03:14 lordjaxom Exp $
 */

#include "menu.h"
#include "bitmap.h"

// --- cText2SkinSetupPage ----------------------------------------------------

cText2SkinSetupPage::cText2SkinSetupPage(void) {
	mData = Text2SkinSetup;
	Setup();
}

void cText2SkinSetupPage::Setup(void) {
	int current = Current();
	Clear();

	Add(new cMenuEditBoolItem(tr("Show scrollbar in the menus"), &mData.MenuScrollbar, tr("no"), tr("yes")));
	Add(new cMenuEditBoolItem(tr("Scrolling behaviour"), &mData.MarqueeLeftRight, tr("to the left"), tr("left and right")));
	Add(new cMenuEditBoolItem(tr("Reset Marquee for new item"), &mData.MarqueeReset, tr("no"), tr("yes")));
#if VDRVERSNUM >= 10344
	Add(new cMenuEditBoolItem(tr("Show auxiliary infos of recordings"), &mData.ShowAux, tr("no"), tr("yes")));
	if (mData.ShowAux)
		Add(new cMenuEditBoolItem(tr("   Extract known tags"), &mData.StripAux, tr("no"), tr("yes")));
#endif
#if VDRVERSNUM >= 10330
	Add(new cMenuEditBoolItem(tr("Use 'timeline' to check timer-conflicts"), &mData.CheckTimerConflict, tr("no"), tr("yes")));
#endif
	Add(new cOsdItem(tr("Flush image cache"), osUser1));
	Add(new cMenuEditIntItem(tr("Max. image cache size"), &mData.MaxCacheFill));

	SetCurrent(Get(current));
	Display();
}

cText2SkinSetupPage::~cText2SkinSetupPage() {
}

void cText2SkinSetupPage::Store(void) {
	SetupStore("MenuScrollbar", mData.MenuScrollbar);
	SetupStore("MarqueeLeftRight", mData.MarqueeLeftRight);
	SetupStore("MarqueeReset", mData.MarqueeReset);
#if VDRVERSNUM >= 10344
	SetupStore("ShowAux", mData.ShowAux);
	SetupStore("StripAux", mData.StripAux);
#endif
#if VDRVERSNUM >= 10330
	SetupStore("CheckTimerConflict", mData.CheckTimerConflict);
#endif
	SetupStore("MaxCacheFill", mData.MaxCacheFill);
	Text2SkinSetup = mData;
}

eOSState cText2SkinSetupPage::ProcessKey(eKeys Key) {
	int oldShowAux = mData.ShowAux;

	eOSState state = cMenuSetupPage::ProcessKey(Key);
	if (state == osUser1) {
		Skins.Message(mtInfo, tr("Flushing image cache..."));
		cText2SkinBitmap::FlushCache();
		Skins.Message(mtInfo, NULL);
		return osContinue;
	}

	if (mData.ShowAux != oldShowAux)
		Setup();

	return state;
}

