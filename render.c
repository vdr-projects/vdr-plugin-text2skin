/*
 * $Id: render.c,v 1.16 2004/06/02 20:43:05 lordjaxom Exp $
 */

#include "render.h"
#include "data.h"
#include "bitmap.h"
#include <vdr/channels.h>
#include <vdr/epg.h>
#include <vdr/menu.h>

cText2SkinRender::cText2SkinRender(cText2SkinData *Data, cText2SkinI18n *I18n, cText2SkinTheme *Theme, eSkinSection Section) {
	tArea areas[MAXOSDAREAS];
	int numAreas = 0;

	mData              = Data;
	mI18n              = I18n;
	mTheme             = Theme;
	mSection           = Section;
	mOsd               = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop);
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

	cText2SkinData::tIterator it = Data->First(mSection);
	for (; it != Data->Last(mSection); ++it) {
		cText2SkinItem *item = (*it);
		if (item->Item() == itemBackground) {
			if (numAreas < MAXOSDAREAS) {
				areas[numAreas].x1 = item->Pos().x;
				areas[numAreas].y1 = item->Pos().y;
				areas[numAreas].x2 = item->Pos().x + item->Size().w - 1;
				areas[numAreas].y2 = item->Pos().y + item->Size().h - 1;
				areas[numAreas].bpp = item->Bpp();
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
}

cText2SkinRender::~cText2SkinRender() {
	delete mOsd; 
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
			DisplayDateTime(item); break;
		case itemDate:
			DisplayDate(item); break;
		case itemTime:
			DisplayTime(item); break;
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
		default:
			break;
		}
	}
	mOsd->Flush();
}

void cText2SkinRender::DrawBackground(const POINT &Pos, const SIZE &Size, const tColor *Bg, const tColor *Fg, const string &Path) {
	bool image = false;
	cText2SkinBitmap bm;
	if (Path != "") {
		char *p;
		asprintf(&p, "%s/%s/%s", SkinPath(), mData->Skin().c_str(), Path.c_str());
		if (bm.Load(p)) {
			if (Bg) bm.SetColor(0, *Bg);
			if (Fg) bm.SetColor(1, *Fg);
			image = true;
		}
		free(p);
	}

	if (image)
		DrawBitmap(mOsd, Pos.x, Pos.y, bm);
	else
		mOsd->DrawRectangle(Pos.x, Pos.y, Pos.x + Size.w - 1, Pos.y + Size.h - 1, Bg ? *Bg : 0);
}

void cText2SkinRender::DrawImage(const POINT &Pos, const SIZE &Size, const tColor *Bg, const tColor *Fg, const string &Path) {
	cText2SkinBitmap bm;
	char *p;
	asprintf(&p, "%s/%s/%s", SkinPath(), mData->Skin().c_str(), Path.c_str());
	printf("Trying to load image: %s\n", p);
	if (bm.Load(p)) {
		if (Bg) bm.SetColor(0, *Bg);
		if (Fg) bm.SetColor(1, *Fg);
		//mOsd->DrawRectangle(Pos.x, Pos.y, Pos.x + Size.w - 1, Pos.y + Size.h - 1, bm.Color(0));
		DrawBitmap(mOsd, Pos.x, Pos.y, bm);
	}
	free(p);
}

void cText2SkinRender::DrawText(const POINT &Pos, const SIZE &Size, const tColor *Fg, const string &Text, const cFont *Font, int Align) {
	DrawTextTransparent(mOsd, Pos.x, Pos.y, Text.c_str(), Fg ? *Fg : 0, 0, Font, Size.w, Size.h, Align);
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
		printf("Languages: ");
		int i = 0;
		while (tracks[i]) {
			printf("%s%s, ", tracks[i], i == current ? " (current)" : "");
			++i;
		}
		printf("\n");
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
	const char *text = DayDateTime(time(NULL));
	DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, text), Item->Font(), Item->Align());
}

void cText2SkinRender::DisplayDate(cText2SkinItem *Item) {
	char *text = strdup(DayDateTime(time(NULL)));
	text[9] = '.';
	text[10] = '\0';
	DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, text + 4), Item->Font(), Item->Align());
	free(text);
}

void cText2SkinRender::DisplayTime(cText2SkinItem *Item) {
	char *text = strdup(DayDateTime(time(NULL)));
	text[18] = '\0'; 
	DrawText(Item->Pos(), Item->Size(), ItemFg(Item), ItemText(Item, text + 13), Item->Font(), Item->Align());
	free(text);
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
	} else if (mSection == sectionReplay) {
		switch (Item->Item()) {
			case itemSymbolPlay:
				image = (mReplaySpeed == -1 && mReplayPlay) ? Item->Path() : Item->AltPath(); break;
			case itemSymbolPause:
				image = (mReplaySpeed == -1 && !mReplayPlay) ? Item->Path() : Item->AltPath(); break;
			case itemSymbolFastFwd:
				image = (mReplayPlay && mReplayForward) ? Item->Path() : Item->AltPath(); break;
			case itemSymbolFastRew:
				image = (mReplayPlay && !mReplayForward) ? Item->Path() : Item->AltPath(); break;
			case itemSymbolSlowFwd:
				image = (!mReplayPlay && mReplayForward) ? Item->Path() : Item->AltPath(); break;
			case itemSymbolSlowRew:
				image = (!mReplayPlay && !mReplayForward) ? Item->Path() : Item->AltPath(); break;
			default:
				break;
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
		if (index == mMenuCurrent) {
			POINT pt = { xoffs, yoffs };
			if (current->Pos().x != -1)
				pt.x += current->Pos().x;
			if (current->Pos().y != -1)
				pt.y += current->Pos().y;
			SIZE size = { current->Size().w, current->Size().h };
			if (ItemBg(current))
				DrawRectangle(pt, size, ItemBg(current));
			DrawText(pt, size, ItemFg(current), mMenuItems[index].name.c_str(), current->Font(), current->Align());
		} else {
			POINT pt = { xoffs + Item->Pos().x, yoffs + Item->Pos().y };
			SIZE size = { Item->Size().w, Item->Size().h };
			DrawText(pt, size, ItemFg(Item), mMenuItems[index].name.c_str(), Item->Font(), Item->Align());
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
