/*
 * $Id: render.c,v 1.22 2004/06/07 19:08:42 lordjaxom Exp $
 */

#include "render.h"
#include "loader.h"
#include "i18n.h"
#include "theme.h"
#include "bitmap.h"
#include <vdr/channels.h>
#include <vdr/epg.h>
#include <vdr/menu.h>

cText2SkinRender::cText2SkinRender(cText2SkinLoader *Loader, eSkinSection Section) {
	tArea areas[MAXOSDAREAS];
	int numAreas = 0;

	SetDescription("Text2Skin: %s display update", cText2SkinData::SectionNames[Section].c_str());

	mData              = Loader->Data();
	mI18n              = Loader->I18n();
	mTheme             = Loader->Theme();
	mSection           = Section;
	mOsd               = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop);
	mScroller          = NULL;
	mChannel           = NULL;
	mChannelNumber     = 0;
	mVolumeCurrent     = 0;
	mVolumeTotal       = 0;
	mVolumeMute        = false;
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

	cText2SkinData::tIterator it = mData->First(mSection);
	for (; it != mData->Last(mSection); ++it) {
		if ((*it)->Item() == itemBackground) {
			if (numAreas < MAXOSDAREAS) {
				areas[numAreas].x1 = (*it)->Pos().x;
				areas[numAreas].y1 = (*it)->Pos().y;
				areas[numAreas].x2 = (*it)->Pos().x + (*it)->Size().w - 1;
				areas[numAreas].y2 = (*it)->Pos().y + (*it)->Size().h - 1;
				areas[numAreas].bpp = (*it)->Bpp();
				++numAreas;
			} else
				esyslog("ERROR: text2skin: too many background areas\n");
		}
	}

	eOsdError res;
	if ((res = mOsd->CanHandleAreas(areas, numAreas)) == oeOk) 
		mOsd->SetAreas(areas, numAreas);
	else {
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

	//Start();
}

cText2SkinRender::~cText2SkinRender() {
	if (mActive) {
		mMutex.Lock();
		mActive = false;
		mDoUpdate.Broadcast();
		mMutex.Unlock();
		Cancel(3);
	}
	delete mScroller;
	delete mOsd; 
}

void cText2SkinRender::Action(void) {
	mActive = true;
	Lock();
	while (mActive) {
		mDoUpdate.Wait(mMutex);
	}
	Unlock();
}

void cText2SkinRender::Flush(void) {
	cText2SkinData::tIterator it = mData->First(mSection);
	for (; it != mData->Last(mSection); ++it) {
		cText2SkinItem *item = (*it);
		switch (item->Item()) {
		case itemBackground:
			DisplayBackground(item); break;
		case itemChannelLogo:
			DisplayChannelLogo(item); break;
		case itemLanguage:
			DisplayLanguage(item); break;
		case itemText:
			DisplayText(item); break;
		case itemImage:
			DisplayImage(item); break;
		case itemDateTime:
		case itemDate:
		case itemTime:
		case itemDateTimeF:
			DisplayDateTime(item); break;
		case itemChannelNumberName:
			DisplayChannelNumberName(item); break;
		case itemChannelNumber:
			DisplayChannelNumber(item); break;
		case itemChannelName:
			DisplayChannelName(item); break;
		case itemRectangle:
			DisplayRectangle(item); break;
		case itemEllipse:
			DisplayEllipse(item); break;
		case itemSlope:
			DisplaySlope(item); break;
		case itemTimebar:
			DisplayTimebar(item); break;
		case itemPresentTime:
			DisplayPresentTime(item); break;
		case itemPresentTitle:
			DisplayPresentTitle(item); break;
		case itemPresentShortText:
			DisplayPresentShortText(item); break;
		case itemFollowingTime:
			DisplayFollowingTime(item); break;
		case itemFollowingTitle:
			DisplayFollowingTitle(item); break;
		case itemFollowingShortText:
			DisplayFollowingShortText(item); break;
		case itemSymbolTeletext:
		case itemSymbolAudio:
		case itemSymbolDolby:
		case itemSymbolEncrypted:
		case itemSymbolRecording:
		case itemSymbolRadio:
		case itemSymbolPlay:
		case itemSymbolPause:
		case itemSymbolFastFwd:
		case itemSymbolFastRew:
		case itemSymbolSlowFwd:
		case itemSymbolSlowRew:
		case itemSymbolScrollUp:
		case itemSymbolScrollDown:
		case itemSymbolEventRunning:
		case itemSymbolEventTimer:
		case itemSymbolEventVPS:
			DisplaySymbol(item); break;
		case itemVolumebar:
			DisplayVolumebar(item); break;
		case itemMute:
			DisplayMute(item); break;
		case itemReplaybar:
			DisplayReplaybar(item); break;
		case itemReplayTitle:
			DisplayReplayTitle(item); break;
		case itemReplayCurrent:
			DisplayReplayCurrent(item); break;
		case itemReplayTotal:
			DisplayReplayTotal(item); break;
		case itemReplayJump:
			DisplayReplayJump(item); break;
		case itemMessageInfo:
		case itemMessageStatus:
		case itemMessageWarning:
		case itemMessageError:
			DisplayMessage(item); break;
		case itemMenuItem:
			DisplayMenuItems(item); break;
		case itemMenuTitle:
			DisplayMenuTitle(item); break;
		case itemMenuRed:
		case itemMenuGreen:
		case itemMenuYellow:
		case itemMenuBlue:
			DisplayMenuColorbutton(item); break;
		case itemMenuText:
			DisplayMenuText(item); break;
		case itemMenuEventTitle:
			DisplayMenuEventTitle(item); break;
		case itemMenuEventShortText:
			DisplayMenuEventShortText(item); break;
		case itemMenuEventDescription:
			DisplayMenuEventDescription(item); break;
		case itemMenuEventTime:
		case itemMenuEventEndTime:
		case itemMenuEventVPSTime:
		case itemMenuEventDate:
		case itemMenuEventDateTimeF:
			DisplayMenuEventTime(item); break;
		case itemMenuRecording:
			DisplayMenuRecording(item); break;
		default:
			break;
		}
	}
	mOsd->Flush();
}

void cText2SkinRender::DrawBackground(const POINT &Pos, const SIZE &Size, const tColor *Bg, const tColor *Fg, const string &Path) {
	cText2SkinBitmap *bmp = NULL;
	if (Path != "") {
		char *p;
		asprintf(&p, "%s/%s/%s", SkinPath(), mData->Skin().c_str(), Path.c_str());
		if ((bmp = cText2SkinBitmap::Load(p)) != NULL) {
			if (Bg) bmp->SetColor(0, *Bg);
			if (Fg) bmp->SetColor(1, *Fg);
		}
		free(p);
	}

	if (bmp)
		mOsd->DrawBitmap(Pos.x, Pos.y, *bmp);
	else
		mOsd->DrawRectangle(Pos.x, Pos.y, Pos.x + Size.w - 1, Pos.y + Size.h - 1, Bg ? *Bg : 0);
}

void cText2SkinRender::DrawImage(const POINT &Pos, const SIZE &Size, const tColor *Bg, const tColor *Fg, const string &Path) {
	cText2SkinBitmap *bmp;
	char *p;
	asprintf(&p, "%s/%s/%s", SkinPath(), mData->Skin().c_str(), Path.c_str());
	Dprintf("Trying to load image: %s\n", p);
	if ((bmp = cText2SkinBitmap::Load(p)) != NULL) {
		if (Bg) bmp->SetColor(0, *Bg);
		if (Fg) bmp->SetColor(1, *Fg);
		mOsd->DrawBitmap(Pos.x, Pos.y, *bmp);
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
		SIZE size = { Size.w * Current / Total, Size.h };
		DrawRectangle(Pos, size, Fg);

		if (Marks) {
			bool Start = true;
			for (const cMark *m = Marks->First(); m; m = Marks->Next(m)) {
				POINT p1 = { Pos.x + m->position * Size.w / Total, Pos.y };
				if (Start) {
					const cMark *m2 = Marks->Next(m);
					tColor col = clrRed;
					POINT p2 = { p1.x, Pos.y + Size.h / 3 };
					SIZE s = { ((m2 ? m2->position : Total) - m->position) * Size.w / Total, Size.h / 3 };
					DrawRectangle(p2, s, &col);
				}
				DrawMark(p1, Size, Start, m->position == Current, false);
				Start = !Start;
			}
		}
	} else {
		SIZE size = { Size.w, Size.h * Current / Total };
		DrawRectangle(Pos, size, Fg);
		
		if (Marks) {
			bool Start = true;
			for (const cMark *m = Marks->First(); m; m = Marks->Next(m)) {
				POINT p1 = { Pos.x, Pos.y + m->position * Size.h / Total };
				if (Start) {
					const cMark *m2 = Marks->Next(m);
					tColor col = clrRed;
					POINT p2 = { Pos.x + Size.w / 3, p1.y };
					SIZE s = { Size.w / 3, ((m2 ? m2->position : Total) - m->position) * Size.h / Total };
					DrawRectangle(p2, s, &col);
				}
				DrawMark(p1, Size, Start, m->position == Current, true);
				Start = !Start;
			}
		}
	}
}

void cText2SkinRender::DrawMark(const POINT &Pos, const SIZE &Size, bool Start, bool Current, bool Horizontal) {
	tColor mark = clrBlack;
	tColor current = clrRed;
	POINT p1 = { Pos.x, Pos.y };
	if (Horizontal) {
		SIZE s1 = { Size.w, 1 };
		DrawRectangle(p1, s1, &mark);
		const int d = Size.w / (Current ? 3 : 9);
		for (int i = 0; i < d; i++) {
			int h = Start ? i : Size.w - 1 - i;
			POINT p2 = { Pos.x + h, Pos.y - d + i };
			SIZE s2 = { 1, (d - i) * 2 };
			DrawRectangle(p2, s2, Current ? &current : &mark);
		}
	} else {
		SIZE s1 = { 1, Size.h };
		DrawRectangle(p1, s1, &mark);
		const int d = Size.h / (Current ? 3 : 9);
		for (int i = 0; i < d; i++) {
			int h = Start ? i : Size.h - 1 - i;
			POINT p2 = { Pos.x - d + i, Pos.y + h };
			SIZE s2 = { (d - i) * 2, 1 };
			DrawRectangle(p2, s2, Current ? &current : &mark);
		}
	}
}

void cText2SkinRender::DrawScrollText(const POINT &Pos, const SIZE &Size, const tColor *Fg, const string &Text, const cFont *Font, int Align) {
	if (mScroller == NULL)
		mScroller = new cTextScroller(mOsd, Pos.x, Pos.y, Size.w, Size.h, Text.c_str(), Font, Fg ? *Fg : 0, clrTransparent);
	else if (mMenuScroll) {
		mScroller->Scroll(mMenuScrollUp, mMenuScrollPage);
		mMenuScroll = false;
	}
}

void cText2SkinRender::DisplayBackground(cText2SkinItem *Item) {
	DrawBackground(Item->Pos(), Item->Size(), ItemBg(Item), ItemFg(Item), Item->Path());
}

void cText2SkinRender::DisplayChannelLogo(cText2SkinItem *Item) {
	if (Item->Type() != "" && mChannel) {
		string path = Item->Path() + "/" + ChannelName(mChannel, mChannelNumber) + "." + Item->Type();
		DrawImage(Item->Pos(), Item->Size(), ItemBg(Item), ItemFg(Item), path);
	}
}

void cText2SkinRender::DisplayLanguage(cText2SkinItem *Item) {
	int current;
	const char **tracks = cDevice::PrimaryDevice()->GetAudioTracks(&current);
	if (Item->Path() != "" && Item->Type() != "" && tracks) {
		Dprintf("Languages: ");
		int i = 0;
		while (tracks[i]) {
			Dprintf("%s%s, ", tracks[i], i == current ? " (current)" : "");
			++i;
		}
		Dprintf("\n");
		if (current < i) {
			string path = Item->Path() + "/" + tracks[current] + "." + Item->Type();
			DrawImage(Item->Pos(), Item->Size(), ItemBg(Item), ItemFg(Item), path);
		}
	}
}

void cText2SkinRender::DisplayText(cText2SkinItem *Item) {
	DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayImage(cText2SkinItem *Item) {
	DrawImage(Item->Pos(), Item->Size(), ItemBg(Item), ItemFg(Item), Item->Path());
}

void cText2SkinRender::DisplayDateTime(cText2SkinItem *Item) {
	char *text = NULL;
	time_t t;
	t = time(NULL);
	switch (Item->Item()) {
	case itemDateTime:
		text = strdup(DayDateTime(t)); break;
	case itemDate:
		text = strdup(DayDateTime(t) + 4); text[5] = '\0'; break;
	case itemTime:
		text = strdup(DayDateTime(t) + 13); text[5] = '\0'; break;
	case itemDateTimeF:
		{
			struct tm tm_r, *tm;
			tm = localtime_r(&t, &tm_r);
			text = MALLOC(char, 1000);
			strftime(text, 1000, Item->Format().c_str(), tm);
		}
		break;
	default:
		break;
	}
	if (text) {
		DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, text), Item->Font(), Item->Align());
		free(text);
	}
}

void cText2SkinRender::DisplayChannelNumberName(cText2SkinItem *Item) {
	DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, ChannelString(mChannel, mChannelNumber)), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayChannelNumber(cText2SkinItem *Item) {
	DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, ChannelNumber(mChannel, mChannelNumber)), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayChannelName(cText2SkinItem *Item) {
	DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, ChannelName(mChannel, mChannelNumber)), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayRectangle(cText2SkinItem *Item) {
	DrawRectangle(Item->Pos(), Item->Size(), ItemFg(Item));

}

void cText2SkinRender::DisplayEllipse(cText2SkinItem *Item) {
	DrawEllipse(Item->Pos(), Item->Size(), ItemFg(Item), Item->Arc());
}

void cText2SkinRender::DisplaySlope(cText2SkinItem *Item) {
	DrawSlope(Item->Pos(), Item->Size(), ItemFg(Item), Item->Arc());
}

void cText2SkinRender::DisplayTimebar(cText2SkinItem *Item) {
	time_t now = time(NULL);
	if (mChannelPresent && mChannelPresent->StartTime() && mChannelPresent->Duration() && now > mChannelPresent->StartTime()) {
		int total = mChannelPresent->Duration();
		int current = now - mChannelPresent->StartTime();
		DrawProgressbar(Item->Pos(), Item->Size(), current, total, ItemBg(Item), ItemFg(Item));
	}
}

void cText2SkinRender::DisplayPresentTime(cText2SkinItem *Item) {
	if (mChannelPresent) {
		const char *text = DayDateTime(mChannelPresent->StartTime());
		DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, text + 10), Item->Font(), Item->Align());
	}
}

void cText2SkinRender::DisplayPresentTitle(cText2SkinItem *Item) {
	if (mChannelPresent && mChannelPresent->Title())
		DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, mChannelPresent->Title()), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayPresentShortText(cText2SkinItem *Item) {
	if (mChannelPresent && mChannelPresent->ShortText())
		DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, mChannelPresent->ShortText()), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayFollowingTime(cText2SkinItem *Item) {
	if (mChannelFollowing) {
		const char *text = DayDateTime(mChannelFollowing->StartTime());
		DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, text + 10), Item->Font(), Item->Align());
	}
}

void cText2SkinRender::DisplayFollowingTitle(cText2SkinItem *Item) {
	if (mChannelFollowing && mChannelFollowing->Title())
		DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, mChannelFollowing->Title()), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayFollowingShortText(cText2SkinItem *Item) {
	if (mChannelFollowing && mChannelFollowing->ShortText())
		DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, mChannelFollowing->ShortText()), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplaySymbol(cText2SkinItem *Item) {
	string image;
	if (mSection == sectionChannel && mChannel) {
		switch (Item->Item()) {
		case itemSymbolTeletext:
			image = mChannel->Tpid() ? Item->Path() : Item->AltPath(); break;
		case itemSymbolAudio:
			image = mChannel->Apid2() ? Item->Path() : Item->AltPath(); break;
		case itemSymbolDolby:
			image = mChannel->Dpid1() ? Item->Path() : Item->AltPath(); break;
		case itemSymbolEncrypted:
			image = mChannel->Ca() ? Item->Path() : Item->AltPath(); break;
		case itemSymbolRadio:
			image = (mChannel->Vpid() == 0 || mChannel->Vpid() == 1 || mChannel->Vpid() == 0x1FFF) ? Item->Path() : Item->AltPath(); break;
		case itemSymbolRecording:
			image = cRecordControls::Active() ? Item->Path() : Item->AltPath(); break;
		default:
			break;
		}
	} else if (mSection == sectionReplay || mSection == sectionReplayMode) {
		switch (Item->Item()) {
		case itemSymbolPlay:
			image = (mReplaySpeed == -1 && mReplayPlay) ? Item->Path() : Item->AltPath(); break;
		case itemSymbolPause:
			image = (mReplaySpeed == -1 && !mReplayPlay) ? Item->Path() : Item->AltPath(); break;
		case itemSymbolFastFwd:
			image = (mReplaySpeed != -1 && mReplayPlay && mReplayForward) ? Item->Path() : Item->AltPath(); break;
		case itemSymbolFastRew:
			image = (mReplaySpeed != -1 && mReplayPlay && !mReplayForward) ? Item->Path() : Item->AltPath(); break;
		case itemSymbolSlowFwd:
			image = (mReplaySpeed != -1 && !mReplayPlay && mReplayForward) ? Item->Path() : Item->AltPath(); break;
		case itemSymbolSlowRew:
			image = (mReplaySpeed != -1 && !mReplayPlay && !mReplayForward) ? Item->Path() : Item->AltPath(); break;
		default:
			break;
		}
	} else if (mSection == sectionMenu) {
		if (mScroller) {
			switch (Item->Item()) {
			case itemSymbolScrollUp:
				image = mScroller->CanScrollUp() ? Item->Path() : Item->AltPath(); break;
			case itemSymbolScrollDown:
				image = mScroller->CanScrollDown() ? Item->Path() : Item->AltPath(); break;
			default:
				break;
			}
		} 
		if (mMenuEvent) {
			switch (Item->Item()) {
			case itemSymbolEventRunning:
				image = mMenuEvent->IsRunning() ? Item->Path() : Item->AltPath(); break;
			case itemSymbolEventTimer:
				image = mMenuEvent->HasTimer() ? Item->Path() : Item->AltPath(); break;
			case itemSymbolEventVPS:
				image = mMenuEvent->Vps() > 0 ? Item->Path() : Item->AltPath(); break;
			default:
				break;
			}
		}
	}
	if (image != "")
		DrawImage(Item->Pos(), Item->Size(), ItemBg(Item), ItemFg(Item), image);
}

void cText2SkinRender::DisplayVolumebar(cText2SkinItem *Item) {
	if (mVolumeTotal && mVolumeCurrent <= mVolumeTotal) {
		int total = mVolumeTotal;
		int current = mVolumeCurrent;
		DrawProgressbar(Item->Pos(), Item->Size(), current, total, ItemBg(Item), ItemFg(Item));
	}
}

void cText2SkinRender::DisplayMute(cText2SkinItem *Item) {
	if (mVolumeMute) {
		if (Item->Path() != "")
			DrawImage(Item->Pos(), Item->Size(), ItemBg(Item), ItemFg(Item), Item->Path());
		if (Item->Text() != "")
			DrawText(Item->Pos(), Item->Size(), ItemFg(Item), Item->Text(), Item->Font(), Item->Align());
	} else if (Item->Path() != "")
		DrawImage(Item->Pos(), Item->Size(), ItemBg(Item), ItemFg(Item), Item->AltPath());
}

void cText2SkinRender::DisplayReplaybar(cText2SkinItem *Item) {
	if (mReplayTotal && mReplayCurrent <= mReplayTotal) {
		int total = mReplayTotal;
		int current = mReplayCurrent;
		DrawProgressbar(Item->Pos(), Item->Size(), current, total, ItemBg(Item), ItemFg(Item), mReplayMarks);
	}
}
				
void cText2SkinRender::DisplayReplayTitle(cText2SkinItem *Item) {
	if (mReplayTitle != "")
		DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, mReplayTitle), Item->Font(), Item->Align());
}
	
void cText2SkinRender::DisplayReplayCurrent(cText2SkinItem *Item) {
	if (mReplayCurrentText != "")
		DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, mReplayCurrentText), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayReplayTotal(cText2SkinItem *Item) {
	if (mReplayTotalText != "")
		DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, mReplayTotalText), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayReplayJump(cText2SkinItem *Item) {
	if (mReplayJump != "")
		DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, mReplayJump), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayMessage(cText2SkinItem *Item) {
	string text;
	switch (Item->Item()) {
	case itemMessageStatus:
		if (mMessageType == mtStatus)  text = mMessageText; break;
	case itemMessageInfo:
		if (mMessageType == mtInfo)    text = mMessageText; break;
	case itemMessageWarning:
		if (mMessageType == mtWarning) text = mMessageText; break;
	case itemMessageError:
		if (mMessageType == mtError)   text = mMessageText; break;
	default:
		break;
	}
	if (text != "")
		DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, text), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayMenuItems(cText2SkinItem *Item) {
	cText2SkinItem *area = mData->Get(sectionMenu, itemMenuArea);
	cText2SkinItem *current = mData->Get(sectionMenu, itemMenuCurrent);
	int xoffs = area->Pos().x;
	int yoffs = area->Pos().y;

	if (Item->Pos().x != -1)
		xoffs += Item->Pos().x;
	if (Item->Pos().y != -1)
		yoffs += Item->Pos().y;

	int index = 0;
	while (yoffs < area->Pos().y + area->Size().h && index < (int)mMenuItems.size()) {
		cText2SkinItem *item = (index == mMenuCurrent) ? current : Item;
		POINT pt = { xoffs, yoffs };
		SIZE size = item->Size();
		if (item->Pos().x != -1)
			pt.x += item->Pos().x;
		if (item->Pos().y != -1)
			pt.y += item->Pos().y;
		if (ItemBg(item))
			DrawRectangle(pt, size, ItemBg(item));

		for (int i = 0; i < cSkinDisplayMenu::MaxTabs; ++i) {
			if (mMenuItems[index].tabs[i] != "") {
				POINT pt2 = { pt.x + mMenuTabs[i], pt.y };
				SIZE sz = { size.w - mMenuTabs[i], size.h };
				DrawText(pt2, sz, ItemFg(item), mMenuItems[index].tabs[i], item->Font(), item->Align());
			}
			if (!mMenuTabs[i + 1])
				break;
		}

		yoffs += Item->Size().h;
		++index;
	}
}

void cText2SkinRender::DisplayMenuTitle(cText2SkinItem *Item) {
	if (mMenuTitle != "")
		DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, mMenuTitle), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayMenuColorbutton(cText2SkinItem *Item) {
	string text;
	switch (Item->Item()) {
	case itemMenuRed:
		text = mMenuRed; break;
	case itemMenuGreen:
		text = mMenuGreen; break;
	case itemMenuYellow:
		text = mMenuYellow; break;
	case itemMenuBlue:
		text = mMenuBlue; break;
	default: break;
	}
	if (text != "")
		DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, text), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayMenuText(cText2SkinItem *Item) {
	if (mMenuText != "")
		DrawScrollText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, mMenuText), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayMenuEventTitle(cText2SkinItem *Item) {
	if (mMenuEvent && mMenuEvent->Title())
		DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, mMenuEvent->Title()), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayMenuEventShortText(cText2SkinItem *Item) {
	if (mMenuEvent && mMenuEvent->ShortText())
		DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, mMenuEvent->ShortText()), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayMenuEventDescription(cText2SkinItem *Item) {
	if (mMenuEvent && mMenuEvent->Description())
		DrawScrollText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, mMenuEvent->Description()), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayMenuEventTime(cText2SkinItem *Item) {
	if (mMenuEvent) {
		char *text = NULL;
		Dprintf("DATE: %s\n", DayDateTime(mMenuEvent->StartTime()));
		switch (Item->Item()) {
		case itemMenuEventTime:
			text = strdup(DayDateTime(mMenuEvent->StartTime()) + 10); break;
		case itemMenuEventEndTime:
			text = strdup(DayDateTime(mMenuEvent->EndTime()) + 10); break;
		case itemMenuEventVPSTime:
			text = mMenuEvent->Vps() != mMenuEvent->StartTime() ? strdup(DayDateTime(mMenuEvent->Vps()) + 10) : NULL; break;
		case itemMenuEventDate:
			text = strdup(DayDateTime(mMenuEvent->StartTime()) + 4); text[5] = '\0'; break;
		case itemMenuEventDateTimeF: 
			{
				time_t t = mMenuEvent->StartTime();
				struct tm tm_r, *tm;
				tm = localtime_r(&t, &tm_r);
				text = MALLOC(char, 1000);
				strftime(text, 1000, Item->Format().c_str(), tm);
			}
			break;
		default:
			break;
		}
		if (text) {
			DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, text), Item->Font(), Item->Align());
			free(text);
		}
	}
}

void cText2SkinRender::DisplayMenuRecording(cText2SkinItem *Item) {
	if (mMenuRecording)
		DrawScrollText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, mMenuRecording->Summary()), Item->Font(), Item->Align());
}

string cText2SkinRender::ItemText(cText2SkinItem *Item) {
	return mI18n ? mI18n->Translate(Item->Text()) : Item->Text();
}

string cText2SkinRender::ItemText(cText2SkinItem *Item, const string &Content) {
	string s;
	if (Item->Text() != "") {
		s = mI18n ? mI18n->Translate(Item->Text()) : Item->Text();
		int pos;
		while ((pos = s.find('$')) != -1)
			s.replace(pos, 1, Content);
	} else
		s = Content;
	return s;
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
	cText2SkinItem *current;
	if (Current)
		current = mData->Get(sectionMenu, itemMenuCurrent);
	else
		current = mData->Get(sectionMenu, itemMenuItem);
	return current->Size().w - mMenuTabs[1];
}
