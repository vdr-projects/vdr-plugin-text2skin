/*
 * $Id: render.c,v 1.36 2004/06/25 17:51:34 lordjaxom Exp $
 */

#include "render.h"
#include "loader.h"
#include "i18n.h"
#include "theme.h"
#include "bitmap.h"
#include "status.h"
#include <vdr/channels.h>
#include <vdr/epg.h>
#include <vdr/menu.h>

cText2SkinRender *cText2SkinRender::mRender = NULL;

cText2SkinRender::cText2SkinRender(cText2SkinLoader *Loader, eSkinSection Section) {
	tArea areas[MAXOSDAREAS];
	int numAreas = 0;

	SetDescription("Text2Skin: %s display update", SectionNames[Section].c_str());

	mData              = Loader->Data();
	mI18n              = Loader->I18n();
	mTheme             = Loader->Theme();
	mSection           = Section;
	mOsd               = NULL;
	mScroller          = NULL;
	mChannel           = NULL;
	mChannelNumber     = 0;
	mVolumeCurrent     = -1;
	mVolumeTotal       = -1;
	mVolumeMute        = false;
	mReplayInfo        = false;
	mReplayPlay        = false;
	mReplayForward     = false;
	mReplaySpeed       = 0;
	mReplayCurrent     = 0;
	mReplayTotal       = 0;
	mReplayMarks       = NULL;
	mMessageType       = (eMessageType)-1;
	mChannelPresent    = NULL;
	mChannelFollowing  = NULL;
	mMenuCurrent       = 0;
	mMenuEvent         = NULL;
	mMenuRecording     = NULL;
	mMenuTextFixedFont = false;
	mMenuScroll        = false;
	mMenuScrollUp      = false;
	mMenuScrollPage    = false;
	mActive            = false;
	mUpdateIn          = 0;
	mBase              = baseRelative;

	mRender            = this;

	cText2SkinItem *item = mData->Get(sectionSkin, itemSkin);
	switch (mBase = item->Base()) {
	case baseRelative:
		mOsd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop);
		mBaseSize = SIZE(Setup.OSDWidth, Setup.OSDHeight);
		break;
	case baseAbsolute:
		mOsd = cOsdProvider::NewOsd(0, 0);
		mBaseSize = SIZE(720, 576); //XXX
		break;
	default:
		break;
	}

	Dprintf("base: %d\n", item->Base());

	eOsdError res = oeOk;
	areas[0].x1 = 0; 
	areas[0].y1 = 0;
	areas[0].x2 = mBaseSize.w - 1;
	areas[0].y2 = mBaseSize.h - 1;
	areas[0].bpp = 8;
	Dprintf("trying big area %d:%d:%d:%d\n", areas[0].x1, areas[0].y1, areas[0].x2, areas[0].y2, areas[0].bpp);
	if ((res = mOsd->CanHandleAreas(areas, 1)) == oeOk) {
		mOsd->SetAreas(areas, 1);
		mOsd->DrawRectangle(areas[0].x1, areas[0].y1, areas[0].x2, areas[0].y2, areas[0].bpp);
	} else {
		Dprintf("failed, chunking\n");
		cText2SkinData::tIterator it = mData->First(mSection);
		for (; it != mData->Last(mSection); ++it) {
			if ((*it)->Item() == itemBackground) {
				if (numAreas < MAXOSDAREAS) {
					POINT p = (*it)->Pos();
					SIZE  s = (*it)->Size();
					areas[numAreas].x1 = p.x;
					areas[numAreas].y1 = p.y;
					areas[numAreas].x2 = p.x + s.w - 1;
					areas[numAreas].y2 = p.y + s.h - 1;
					areas[numAreas].bpp = (*it)->Bpp();
					++numAreas;
				} else
					esyslog("ERROR: text2skin: too many background areas\n");
			}
		}
	
		if ((res = mOsd->CanHandleAreas(areas, numAreas)) == oeOk) 
			mOsd->SetAreas(areas, numAreas);
	}

	if (res != oeOk) {
		const char *emsg = NULL;
		switch (res) {
		case oeTooManyAreas:
			emsg = "Too many OSD areas"; break;
		case oeTooManyColors:
			emsg = "Too many Colors"; break;
		case oeBppNotSupported:
			emsg = "Depth not supported"; break;
		case oeAreasOverlap:
			emsg = "Areas are overlapping"; break;
		case oeWrongAlignment:
			emsg = "Areas not correctly aligned"; break;
		case oeOutOfMemory:
			emsg = "OSD memory overflow"; break;
		case oeUnknown:
			emsg = "Unknown OSD error"; break;
		default:
			break;
		}
		esyslog("ERROR: text2skin: OSD provider can't handle skin: %s\n", emsg);
	}
	Start();
}

cText2SkinRender::~cText2SkinRender() {
	if (mActive) {
		mActive = false;
		Flush();
		Cancel(3);
	}
	delete mScroller;
	delete mOsd; 
	cText2SkinBitmap::ResetCache();
	mRender = NULL;
}

void cText2SkinRender::Action(void) {
	mActive = true;
	Lock();
	while (mActive) {
		if (mUpdateIn) mDoUpdate.TimedWait(mMutex, mUpdateIn);
		else           mDoUpdate.Wait(mMutex);

		if (!mActive) break; // fall out if thread to be stopped

		mUpdateIn = 0; // has to be re-set within Update();
		Update();
	}
	Unlock();
}

void cText2SkinRender::Update(void) {
	Dbench(update);

	if (mScroller && !mMenuScroll)
		mOsd->SaveRegion(mScroller->Left(), mScroller->Top(), mScroller->Left() + mScroller->Width() - 1, mScroller->Top() + mScroller->Height() - 1);

	cText2SkinData::tIterator it = mData->First(mSection);
	for (; it != mData->Last(mSection); ++it) {
		switch ((*it)->Display()) {
		case displayAlways:
			DisplayItem(*it);
			break;
		case displayDateTimeF:
		case displayDateTime:
		case displayDate:
		case displayTime:
			DisplayDateTime(*it);
			break;
		case displayChannelNumberName:
		case displayChannelNumber:
		case displayChannelName:
			DisplayChannelNumberName(*it);
			break;
		case displayPresentDateTimeF:
		case displayPresentStartTime:
		case displayPresentDate:
		case displayPresentVPSTime:
		case displayPresentEndTime:
		case displayPresentDuration:
			DisplayPresentTime(*it);
			break;
		case displayPresentVPS:
		case displayPresentRunning:
		case displayPresentTimer:
			DisplayPresentIcon(*it);
			break;
		case displayPresentTitle:
		case displayPresentShortText:
		case displayPresentDescription:
		case displayPresentTextDescription:
			DisplayPresentText(*it);
			break;
		case displayFollowingStartTime:
		case displayFollowingEndTime:
		case displayFollowingDuration:
			DisplayFollowingTime(*it);
			break;
		case displayFollowingTitle:
			DisplayFollowingTitle(*it);
			break;
		case displayFollowingShortText:
			DisplayFollowingShortText(*it);
			break;
		case displayLanguage:
			DisplayLanguage(*it);
			break;
		case displayTeletext:
		case displayAudio:
		case displayDolby:
		case displayEncrypted:
		case displayRecording:
		case displayRadio:
			DisplayChannelIcon(*it);
			break;
		case displayVolumeCurrent:
		case displayVolumeTotal:
			DisplayVolume(*it);
			break;
		case displayMute:
			DisplayMuteIcon(*it);
			break;
		case displayReplayTime:
		case displayReplayDuration:
			DisplayReplayTime(*it);
			break;
		case displayReplayTitle:
			DisplayReplayTitle(*it);
			break;
		case displayReplayPrompt:
			DisplayReplayPrompt(*it);
			break;
		case displayPlay:
		case displayPause:
		case displayFastFwd:
		case displayFastRew:
		case displaySlowFwd:
		case displaySlowRew:
			DisplayReplaySymbol(*it);
			break;
		case displayReplayMode:
			DisplayReplayMode(*it);
			break;
		case displayMessage:
		case displayMessageStatus:
		case displayMessageInfo:
		case displayMessageWarning:
		case displayMessageError:
			DisplayMessage(*it);
			break;
		case displayMenuTitle:
			DisplayMenuTitle(*it);
			break;
		case displayMenuRed:
		case displayMenuGreen:
		case displayMenuYellow:
		case displayMenuBlue:
			DisplayMenuButton(*it);
			break;
		case displayMenuText:
		case displayMenuRecording:
			DisplayMenuText(*it);
			break;
		case displayMenuScrollUp:
		case displayMenuScrollDown:
			DisplayMenuScrollIcon(*it);
			break;
		case displayMenuItems:
		case displayMenuCurrent:
		case displayMenuGroups:
			DisplayMenuItems(*it);
			break;
		default:
			break;
		}
	}
	Dbench(flush);
	mOsd->Flush();
	Dprintf("flush only took %d ms\n", Ddiff(flush));
	Dprintf("complete flush took %d ms\n", Ddiff(update));
}

void cText2SkinRender::DrawBackground(const POINT &Pos, const SIZE &Size, const tColor *Bg, const tColor *Fg, int Alpha, const string &Path) {
	cText2SkinBitmap *bmp = NULL;
	if (Path != "") {
		char *p;
		asprintf(&p, "%s/%s/%s", SkinPath(), mData->Skin().c_str(), Path.c_str());
		if ((bmp = cText2SkinBitmap::Load(p, Alpha)) != NULL) {
			if (Bg) bmp->SetColor(0, *Bg);
			if (Fg) bmp->SetColor(1, *Fg);
		}
		free(p);
	}

	if (bmp)
		mOsd->DrawBitmap(Pos.x, Pos.y, bmp->Get(mUpdateIn));
	else
		mOsd->DrawRectangle(Pos.x, Pos.y, Pos.x + Size.w - 1, Pos.y + Size.h - 1, Bg ? *Bg : 0);
}

void cText2SkinRender::DrawImage(const POINT &Pos, const SIZE &Size, const tColor *Bg, const tColor *Fg, int Alpha, const string &Path) {
	cText2SkinBitmap *bmp;
	char *p;
	asprintf(&p, "%s/%s/%s", SkinPath(), mData->Skin().c_str(), Path.c_str());
	Dprintf("Trying to load image: %s\n", p);
	if ((bmp = cText2SkinBitmap::Load(p, Alpha)) != NULL) {
		if (Bg) bmp->SetColor(0, *Bg);
		if (Fg) bmp->SetColor(1, *Fg);
		mOsd->DrawBitmap(Pos.x, Pos.y, bmp->Get(mUpdateIn));
	}
	free(p);
}

void cText2SkinRender::DrawText(const POINT &Pos, const SIZE &Size, const tColor *Fg, const string &Text, const cFont *Font, int Align) {
	mOsd->DrawText(Pos.x, Pos.y, Text.c_str(), Fg ? *Fg : 0, 0, Font, Size.w, Size.h, Align);
}

void cText2SkinRender::DrawRectangle(const POINT &Pos, const SIZE &Size, const tColor *Fg) {
	mOsd->DrawRectangle(Pos.x, Pos.y, Pos.x + Size.w - 1, Pos.y + Size.h - 1, Fg ? *Fg : 0);
}

void cText2SkinRender::DrawEllipse(const POINT &Pos, const SIZE &Size, const tColor *Fg, int Arc) {
	mOsd->DrawEllipse(Pos.x, Pos.y, Pos.x + Size.w - 1, Pos.y + Size.h - 1, Fg ? *Fg : 0, Arc);
}

void cText2SkinRender::DrawSlope(const POINT &Pos, const SIZE &Size, const tColor *Fg, int Arc) {
	mOsd->DrawSlope(Pos.x, Pos.y, Pos.x + Size.w - 1, Pos.y + Size.h - 1, Fg ? *Fg : 0, Arc);
}

void cText2SkinRender::DrawProgressbar(const POINT &Pos, const SIZE &Size, int Current, int Total, const tColor *Bg, const tColor *Fg, const cMarks *Marks) {
	if (Bg)
		DrawRectangle(Pos, Size, Bg);
	if (Current > Total)
		Current = Total;
	if (Size.w > Size.h) {
		DrawRectangle(Pos, SIZE(Size.w * Current / Total, Size.h), Fg);

		if (Marks) {
			bool start = true;
			for (const cMark *m = Marks->First(); m; m = Marks->Next(m)) {
				POINT pt(Pos.x + m->position * Size.w / Total, Pos.y);
				if (start) {
					const cMark *m2 = Marks->Next(m);
					tColor col = clrRed;
					DrawRectangle(POINT(pt.x, Pos.y + Size.h / 3), SIZE(((m2 ? m2->position : Total) - m->position) * Size.w / Total, Size.h / 3), &col);
				}
				DrawMark(pt, Size, start, m->position == Current, false);
				start = !start;
			}
		}
	} else {
		DrawRectangle(Pos, SIZE(Size.w, Size.h * Current / Total), Fg);
		
		if (Marks) {
			bool start = true;
			for (const cMark *m = Marks->First(); m; m = Marks->Next(m)) {
				POINT pt(Pos.x, Pos.y + m->position * Size.h / Total);
				if (start) {
					const cMark *m2 = Marks->Next(m);
					tColor col = clrRed;
					DrawRectangle(POINT(Pos.x + Size.w / 3, pt.y), SIZE(Size.w / 3, ((m2 ? m2->position : Total) - m->position) * Size.h / Total), &col);
				}
				DrawMark(pt, Size, start, m->position == Current, true);
				start = !start;
			}
		}
	}
}

void cText2SkinRender::DrawMark(const POINT &Pos, const SIZE &Size, bool Start, bool Current, bool Horizontal) {
	tColor mark = clrBlack;
	tColor current = clrRed;
	POINT p1 = Pos;
	if (Horizontal) {
		DrawRectangle(p1, SIZE(Size.w, 1), &mark);
		const int d = Size.w / (Current ? 3 : 9);
		for (int i = 0; i < d; i++) {
			int h = Start ? i : Size.w - 1 - i;
			DrawRectangle(POINT(Pos.x + h, Pos.y - d + i), SIZE(1, (d - i) * 2), Current ? &current : &mark);
		}
	} else {
		DrawRectangle(p1, SIZE(1, Size.h), &mark);
		const int d = Size.h / (Current ? 3 : 9);
		for (int i = 0; i < d; i++) {
			int h = Start ? i : Size.h - 1 - i;
			DrawRectangle(POINT(Pos.x - d + i, Pos.y + h), SIZE((d - i) * 2, 1), Current ? &current : &mark);
		}
	}
}

void cText2SkinRender::DrawScrolltext(const POINT &Pos, const SIZE &Size, const tColor *Fg, const string &Text, const cFont *Font, int Align) {
	if (mScroller == NULL)
		mScroller = new cTextScroller(mOsd, Pos.x, Pos.y, Size.w, Size.h, Text.c_str(), Font, Fg ? *Fg : 0, clrTransparent);
	else if (mMenuScroll) {
		mScroller->Scroll(mMenuScrollUp, mMenuScrollPage);
		mMenuScroll = false;
	} else
		mOsd->RestoreRegion();
}
	
void cText2SkinRender::DrawScrollbar(const POINT &Pos, const SIZE &Size, int Offset, int Shown, int Total, const tColor *Bg, const tColor *Fg) {
	DrawRectangle(Pos, Size, Bg);
	if (Size.h > Size.w) {
		POINT sp = Pos;
		SIZE ss = Size;
		sp.y += Size.h * Offset / Total;
		ss.h = Size.h * Shown / Total + 1;
		DrawRectangle(sp, ss, Fg);
	} else {
		POINT sp = Pos;
		SIZE ss = Size;
		sp.x += Size.w * Offset / Total;
		ss.w = Size.w * Shown / Total + 1;
		DrawRectangle(sp, ss, Fg);
	}
}

void cText2SkinRender::DisplayItem(cText2SkinItem *Item, const ItemData *Data) {
	static ItemData dummyData;
	if (Data == NULL) Data = &dummyData;
	switch (Item->Item()) {
	case itemBackground:
		DrawBackground(Item->Pos(), Item->Size(), ItemBg(Item), ItemFg(Item), Item->Alpha(), Item->Path()); 
		break;
	case itemText:
		DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, Data->text), Item->Font(), Item->Align());
		break;
	case itemScrolltext:
		DrawScrolltext(Item->Pos(), Item->Size(), ItemFg(Item), Data->text, Item->Font(), Item->Align());
		break;
	case itemImage:
		DrawImage(Item->Pos(), Item->Size(), ItemBg(Item), ItemFg(Item), Item->Alpha(), Item->Path());
		break;
	case itemLogo:
	case itemSymbol:
		if (Data->path != "")
			DrawImage(Item->Pos(), Item->Size(), ItemBg(Item), ItemFg(Item), Item->Alpha(), Data->path);
		break;
	case itemRectangle:
		DrawRectangle(Item->Pos(), Item->Size(), ItemFg(Item));
		break;
	case itemEllipse:
		DrawEllipse(Item->Pos(), Item->Size(), ItemFg(Item), Item->Arc());
		break;
	case itemSlope:
		DrawSlope(Item->Pos(), Item->Size(), ItemFg(Item), Item->Arc());
		break;
	case itemProgress:
		DrawProgressbar(Item->Pos(), Item->Size(), Data->current, Data->total, ItemBg(Item), ItemFg(Item), Data->marks);
		break;
	case itemScrollbar:
		DrawScrollbar(Item->Pos(), Item->Size(), Data->current, Data->shown, Data->total, ItemBg(Item), ItemFg(Item));
	default: 
		break;
	}
}

void cText2SkinRender::DisplayDateTime(cText2SkinItem *Item) {
	ItemData data;
	char text[1000];
	time_t t = time(NULL);
	struct tm tm_r, *tm;
	*text = '\0';
	tm = localtime_r(&t, &tm_r);

	switch (Item->Display()) {
	case displayDateTimeF:
		strftime(text, 1000, Item->Format().c_str(), tm);
		break;
	case displayDateTime:
		strftime(text, 1000, "%a %e.%m. %H:%M", tm);
		break;
	case displayDate:
		strftime(text, 1000, "%e.%m.", tm);
		break;
	case displayTime:
		strftime(text, 1000, "%H:%M", tm);
		break;
	default:
		break;
	}

	data.text = text;
	DisplayItem(Item, &data);
}

void cText2SkinRender::DisplayChannelNumberName(cText2SkinItem *Item) {	
	if (mChannel || mChannelNumber) {
		ItemData data;
		data.path = Item->Path() + "/" + ChannelName(mChannel, mChannelNumber) + "." + Item->Type();
		switch (Item->Display()) {
		case displayChannelNumberName:
			data.text = ChannelString(mChannel, mChannelNumber);
			break;
		case displayChannelNumber:
			data.text = ChannelNumber(mChannel, mChannelNumber);
			break;
		case displayChannelName:
			data.text = ChannelName(mChannel, mChannelNumber);
			break;
		default:
			break;
		}
		DisplayItem(Item, &data);
	}
}

void cText2SkinRender::DisplayPresentTime(cText2SkinItem *Item) {
	const cEvent *event = NULL;
	switch (mSection) {
	case sectionChannel:
	case sectionChannelSmall:
		event = mChannelPresent;
		break;
	case sectionMenu:
		event = mMenuEvent;
		break;
	default:
		break;
	}

	if (event && event->StartTime()) {
		ItemData data;
		char text[1000];
		const char *f = "%H:%M";
		time_t t = 0, n = time(NULL);
		struct tm tm_r, *tm;
		*text = '\0';
		data.path = Item->Path();

		switch (Item->Display()) {
		case displayPresentDateTimeF:
			f = Item->Format().c_str();
			t = event->StartTime();
			break;
		case displayPresentStartTime:
			t = event->StartTime();
			break;
		case displayPresentDate:
			f = "%e.%m.";
			t = event->StartTime();
			break;
		case displayPresentVPSTime:
			t = event->StartTime() != event->Vps() ? event->Vps() : 0;
			break;
		case displayPresentEndTime:
			t = event->EndTime();
			break;
		case displayPresentDuration:
			t = event->Duration();
			break;
		default:
			break;
		}
	
		if (t) {
			tm = localtime_r(&t, &tm_r);
			strftime(text, 1000, f, tm);
			data.text = text;
			data.total = event->Duration();
			data.current = n - event->StartTime();
			DisplayItem(Item, &data);
		}
	}
}

void cText2SkinRender::DisplayPresentIcon(cText2SkinItem *Item) {
	const cEvent *event = NULL;
	switch (mSection) {
	case sectionChannel:
	case sectionChannelSmall:
		event = mChannelPresent;
		break;
	case sectionMenu:
		event = mMenuEvent;
		break;
	default:
		break;
	}

	if (event) {
		ItemData data;
		switch (Item->Display()) {
		case displayPresentVPS:
			data.path = event->Vps() && event->Vps() != event->StartTime() ? Item->Path() : Item->AltPath();
			break;
		case displayPresentRunning:
			data.path = event->IsRunning() ? Item->Path() : Item->AltPath();
			break;
		case displayPresentTimer:
			data.path = event->HasTimer() ? Item->Path() : Item->AltPath();
			break;
		default:
			break;
		}
		if (data.path != "")
			DisplayItem(Item, &data);
	}
}

void cText2SkinRender::DisplayPresentText(cText2SkinItem *Item) {
	const cEvent *event = NULL;
	switch (mSection) {
	case sectionChannel:
	case sectionChannelSmall:
		event = mChannelPresent;
		break;
	case sectionMenu:
		event = mMenuEvent;
		break;
	default:
		break;
	}
	
	if (event) {
		string text;
		switch (Item->Display()) {
		case displayPresentTitle:
			text = event->Title() ? event->Title() : "";
			break;
		case displayPresentShortText:
			text = event->ShortText() ? event->ShortText() : "";
			break;
		case displayPresentDescription:
			text = event->Description() ? event->Description() : "";
			break;
		case displayPresentTextDescription:
			if (event->ShortText())
				text += (string)event->ShortText() + "\n\n";
			if (event->Description())
				text += event->Description();
		default:
			break;
		}

		if (text != "") {
			ItemData data;
			data.text = text;
			DisplayItem(Item, &data);
		}
	}
}

void cText2SkinRender::DisplayFollowingTime(cText2SkinItem *Item) {
	if (mChannelFollowing && mChannelFollowing->StartTime()) {
		ItemData data;
		char text[1000];
		time_t t = 0, n = time(NULL);
		struct tm tm_r, *tm;
		*text = '\0';

		switch (Item->Display()) {
		case displayFollowingStartTime:
			t = mChannelFollowing->StartTime();
			break;
		case displayFollowingEndTime:
			t = mChannelFollowing->EndTime();
			break;
		case displayFollowingDuration:
			t = mChannelFollowing->Duration();
			break;
		default:
			break;
		}

		tm = localtime_r(&t, &tm_r);
		strftime(text, 1000, "%H:%M", tm);
		data.text = text;
		data.total = mChannelFollowing->Duration();
		data.current = n - mChannelFollowing->StartTime();
		DisplayItem(Item, &data);
	}
}

void cText2SkinRender::DisplayFollowingTitle(cText2SkinItem *Item) {
	if (mChannelFollowing && mChannelFollowing->Title()) {
		ItemData data;
		data.text = mChannelFollowing->Title();
		DisplayItem(Item, &data);
	}
}

void cText2SkinRender::DisplayFollowingShortText(cText2SkinItem *Item) {
	if (mChannelFollowing && mChannelFollowing->ShortText()) {
		ItemData data;
		data.text = mChannelFollowing->ShortText();
		DisplayItem(Item, &data);
	}
}

void cText2SkinRender::DisplayLanguage(cText2SkinItem *Item) {
	int current;
	const char **tracks = cDevice::PrimaryDevice()->GetAudioTracks(&current);
	if (tracks) {
		Dprintf("Languages: ");
		int i;
		for (i = 0; tracks[i] != NULL; ++i)
			Dprintf("%s%s, ", tracks[i], i == current ? " (current)" : "");
		Dprintf("\n");

		if (current < i) {
			ItemData data;
			data.text = tracks[current];
			data.path = Item->Path() + "/" + tracks[current] + "." + Item->Type();
			DisplayItem(Item, &data);
		}
	}
}

void cText2SkinRender::DisplayChannelIcon(cText2SkinItem *Item) {
	if (mChannel && !mChannel->GroupSep()) {
		ItemData data;
		switch (Item->Display()) {
		case displayTeletext:
			data.path = mChannel->Tpid() ? Item->Path() : Item->AltPath(); 
			break;
		case displayAudio:
			data.path = mChannel->Apid2() ? Item->Path() : Item->AltPath(); 
			break;
		case displayDolby:
			data.path = mChannel->Dpid1() ? Item->Path() : Item->AltPath(); 
			break;
		case displayEncrypted:
			data.path = mChannel->Ca() ? Item->Path() : Item->AltPath(); 
			break;
		case displayRadio:
			data.path = mChannel->Vpid() == 0 || mChannel->Vpid() == 1 || mChannel->Vpid() == 0x1FFF ? Item->Path() : Item->AltPath(); 
			break;
		case displayRecording:
			data.path = cRecordControls::Active() ? Item->Path() : Item->AltPath(); 
			break;
		default:
			break;
		}
		if (data.path != "")
			DisplayItem(Item, &data);
	}
}

void cText2SkinRender::DisplayVolume(cText2SkinItem *Item) {
	if (mVolumeTotal && mVolumeCurrent <= mVolumeTotal && !mVolumeMute) {
		ItemData data;
		char *text = NULL;
		data.total = mVolumeTotal;
		data.current = mVolumeCurrent;
		switch (Item->Display()) {
		case displayVolumeCurrent:
			asprintf(&text, "%d", mVolumeCurrent);
			break;
		case displayVolumeTotal:
			asprintf(&text, "%d", mVolumeTotal);
			break;
		default:
			break;
		}
		if (text) {
			data.text = text;
			free(text);
		}
		DisplayItem(Item, &data);
	}
}

void cText2SkinRender::DisplayMuteIcon(cText2SkinItem *Item) {
	ItemData data;
	data.path = mVolumeMute ? Item->Path() : Item->AltPath();
	if (data.path != "")
		DisplayItem(Item, &data);
}

void cText2SkinRender::DisplayReplayTime(cText2SkinItem *Item) {
	if (mReplayTotal && mReplayCurrent <= mReplayTotal) {
		ItemData data;
		data.total = mReplayTotal;
		data.current = mReplayCurrent;
		data.marks = mReplayMarks;
		switch (Item->Display()) {
		case displayReplayTime:
			data.text = mReplayCurrentText;
			break;
		case displayReplayDuration:
			data.text = mReplayTotalText;
			break;
		default:
			break;
		}
		DisplayItem(Item, &data);
	}
}
				
void cText2SkinRender::DisplayReplayTitle(cText2SkinItem *Item) {
	if (mReplayTitle != "") {
		ItemData data;
		data.text = mReplayTitle;
		DisplayItem(Item, &data);
	}
}
	
void cText2SkinRender::DisplayReplayPrompt(cText2SkinItem *Item) {
	if (mReplayJump != "") {
		ItemData data;
		data.text = mReplayJump;
		DisplayItem(Item, &data);
	}
}

void cText2SkinRender::DisplayReplaySymbol(cText2SkinItem *Item) {
	if (mReplayInfo) {
		ItemData data;
		switch (Item->Display()) {
		case displayPlay:
			data.path = (mReplaySpeed == -1 && mReplayPlay) ? Item->Path() : Item->AltPath(); 
			break;
		case displayPause:
			data.path = (mReplaySpeed == -1 && !mReplayPlay) ? Item->Path() : Item->AltPath(); 
			break;
		case displayFastFwd:
			data.path = (mReplaySpeed != -1 && mReplayPlay && mReplayForward) ? Item->Path() : Item->AltPath(); 
			break;
		case displayFastRew:
			data.path = (mReplaySpeed != -1 && mReplayPlay && !mReplayForward) ? Item->Path() : Item->AltPath(); 
			break;
		case displaySlowFwd:
			data.path = (mReplaySpeed != -1 && !mReplayPlay && mReplayForward) ? Item->Path() : Item->AltPath(); 
			break;
		case displaySlowRew:
			data.path = (mReplaySpeed != -1 && !mReplayPlay && !mReplayForward) ? Item->Path() : Item->AltPath(); 
			break;
		default:
			break;
		}
		DisplayItem(Item, &data);
	}
}

void cText2SkinRender::DisplayReplayMode(cText2SkinItem *Item) {
	if (cText2SkinStatus::ReplayMode() != replayNone) {
		ItemData data;
		Dprintf("Replay Type is %s\n", ReplayNames[cText2SkinStatus::ReplayMode()].c_str());
		data.path = Item->Path() + "/" + ReplayNames[cText2SkinStatus::ReplayMode()] + "." + Item->Type();
		DisplayItem(Item, &data);
	}
}

void cText2SkinRender::DisplayMessage(cText2SkinItem *Item) {
	if (mMessageText != "" && (Item->Display() == displayMessage || (Item->Display() - displayMessageStatus) == mMessageType)) {
		ItemData data;
		data.text = mMessageText;
		DisplayItem(Item, &data);
	}
}

void cText2SkinRender::DisplayMenuTitle(cText2SkinItem *Item) {
	if (mMenuTitle != "") {
		ItemData data;
		data.text = mMenuTitle;
		DisplayItem(Item, &data);
	}
}

void cText2SkinRender::DisplayMenuButton(cText2SkinItem *Item) {
	ItemData data;
	switch (Item->Display()) {
	case displayMenuRed:
		data.text = mMenuRed; 
		break;
	case displayMenuGreen:
		data.text = mMenuGreen; 
		break;
	case displayMenuYellow:
		data.text = mMenuYellow; 
		break;
	case displayMenuBlue:
		data.text = mMenuBlue; 
		break;
	default:
		break;
	}
	if (data.text != "")
		DisplayItem(Item, &data);
}

void cText2SkinRender::DisplayMenuText(cText2SkinItem *Item) {
	ItemData data;

	switch (Item->Display()) {
	case displayMenuText:
		data.text = mMenuText;
		break;
	case displayMenuRecording:
		if (mMenuRecording && mMenuRecording->Summary())
			data.text = mMenuRecording->Summary();
		break;
	default:
		break;
	}
	if (data.text != "") {
		// HACK: make sure the scroller exists!
		if (mScroller) {
			data.current = mScroller->Offset();
			data.shown = mScroller->Shown();
			data.total = mScroller->Total();
		}
		DisplayItem(Item, &data);
	}
}

void cText2SkinRender::DisplayMenuScrollIcon(cText2SkinItem *Item) {
	if (mScroller) {
		ItemData data;
		switch (Item->Display()) {
		case displayMenuScrollUp:
			data.path = mScroller->CanScrollUp() ? Item->Path() : Item->AltPath(); 
			break;
		case displayMenuScrollDown:
			data.path = mScroller->CanScrollDown() ? Item->Path() : Item->AltPath(); 
			break;
		default:
			break;
		}
		if (data.path != "")
			DisplayItem(Item, &data);
	} 
}

void cText2SkinRender::DisplayMenuItems(cText2SkinItem *Item) {
	cText2SkinItem *area = mData->Get(sectionMenu, itemMenuArea);
	cText2SkinItem *item = mData->Get(sectionMenu, itemMenuItem);

	if (item && area) {
		POINT pos = area->Pos1();
		
		int max = area->Size().h / item->Size().h;
		for (int i = 0; i < min((int)mMenuItems.size(), max); ++i) {
			switch (Item->Display()) {
			case displayMenuItems:
				if (i == mMenuCurrent || !mMenuItems[i].sel)
					continue;
				break;
			case displayMenuCurrent:
				if (i != mMenuCurrent)
					continue;
				break;
			case displayMenuGroups:
				if (mMenuItems[i].sel)
					continue;
				break;
			default:
				break;
			}
		
			POINT itempos = pos;
			itempos.y += i * item->Size().h;
			itempos += Item->Pos1();
			if (Item->Item() == itemText) { // draw tabs
				for (int t = 0; t < cSkinDisplayMenu::MaxTabs; ++t) {
					if (mMenuItems[i].tabs[t] != "") {
						ItemData data;
						cText2SkinItem cur = *Item;
						cur.Pos1() = itempos;
						cur.Pos1().x += mMenuTabs[t];
						cur.Pos2() += itempos;
						data.text = mMenuItems[i].tabs[t];
						DisplayItem(&cur, &data);
					}
					if (!mMenuTabs[t + 1])
						break;
				}
			} else {
				ItemData data;
				cText2SkinItem cur = *Item;
				cur.Pos1() = itempos;
				cur.Pos2() += itempos;
				DisplayItem(&cur, &data);
			}
		}
	}
}

string cText2SkinRender::ItemText(cText2SkinItem *Item) {
	return mI18n ? mI18n->Translate(Item->Text()) : Item->Text();
}

string cText2SkinRender::ItemText(cText2SkinItem *Item, const string &Content) {
	if (Item->Text() != "") {
		string s = mI18n ? mI18n->Translate(Item->Text()) : Item->Text();
		int pos;
		while ((pos = s.find("{*}")) != -1)
			s.replace(pos, 3, Content);
		return s;
	}
	return Content;
}

tColor *cText2SkinRender::ItemFg(cText2SkinItem *Item) {
	static tColor Fg;
	if (Item->Fg() != "") {
		if (Item->Fg()[0] == '#')
			Fg = strtoul(Item->Fg().c_str() + 1, NULL, 16);
		else
			Fg = mTheme->Color(Item->Fg());
	} else
		return NULL;
	return &Fg;
}

tColor *cText2SkinRender::ItemBg(cText2SkinItem *Item) {
	static tColor Bg;
	if (Item->Bg() != "") {
		if (Item->Bg()[0] == '#')
			Bg = strtoul(Item->Bg().c_str() + 1, NULL, 16);
		else
			Bg = mTheme->Color(Item->Bg());
	} else
		return NULL;
	return &Bg;
}

int cText2SkinRender::GetEditableWidth(MenuItem Item, bool Current) {
	cText2SkinItem *item;
	item = mData->Get(sectionMenu, itemMenuItem);
	return item->Size().w - mMenuTabs[1];
}
	
POINT cText2SkinRender::Transform(const POINT &Pos) {
	SIZE base = mRender->mBaseSize;
	return POINT(Pos.x < 0 ? base.w + Pos.x : Pos.x, Pos.y < 0 ? base.h + Pos.y : Pos.y);
}

