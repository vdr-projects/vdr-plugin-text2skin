/*
 * $Id: render.c,v 1.1.1.1 2004/05/23 00:08:03 lordjaxom Exp $
 */

#include <vdr/channels.h>
#include <vdr/epg.h>
#include "render.h"
#include "data.h"
#include "bitmap.h"
#include "common.h"

cText2SkinRender::cText2SkinRender(cText2SkinData *Data, eSkinSection Section) {
	tArea areas[MAXOSDAREAS];
	int numAreas = 0;

	mData          = Data;
	mSection       = Section;
	mOsd           = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop);
	mChannel       = NULL;
	mNumber        = 0;
	mVolumeCurrent = 0;
	mVolumeTotal   = 0;
	mVolumeMute    = false;
	mReplayTitle   = NULL;
	mReplayPlay    = false;
	mReplayForward = false;
	mReplaySpeed   = 0;
	mReplayCurrent = 0;
	mReplayTotal   = 0;
	mReplayJump    = NULL;
	mMessageType   = (eMessageType)-1;
	mMessageText   = NULL;
	mPresent       = NULL;
	mFollowing     = NULL;
	mTitle         = NULL;
	mCurrent       = 0;

	cText2SkinItem *item;
	for (item = Data->First(); item; item = Data->Next(item)) {
		if (item->Section() == Section && item->Item() == itemBackground) {
			if (numAreas < MAXOSDAREAS) {
				printf("area item: %d:%d:%d:%d\n", item->X(), item->Y(), item->X() + item->Width() - 1, item->Y() + item->Height() - 1, item->Bpp());
				areas[numAreas].x1 = item->X();
				areas[numAreas].y1 = item->Y();
				areas[numAreas].x2 = item->X() + item->Width() - 1;
				areas[numAreas].y2 = item->Y() + item->Height() - 1;
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
	cText2SkinItem *item;
	for (item = mData->First(); item; item = mData->Next(item)) {
		if (item->Section() == mSection) {
			switch (item->Item()) {
			case itemBackground:
				DisplayBackground(item); break;
			case itemLogo:
				DisplayLogo(item); break;
			case itemText:
				DisplayText(item); break;
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
				DisplaySymbol(item); break;
			case itemVolumebar:
				DisplayVolumebar(item); break;
			case itemMute:
				DisplayMute(item); break;
			case itemProgressbar:
				DisplayProgressbar(item); break;
			case itemReplayTitle:
				DisplayReplayTitle(item); break;
			case itemMenuItem:
				DisplayMenuItems(item); break;
			default:
				break;
			}
		}
	}
	printf("osd flush\n");
	mOsd->Flush();
}

void cText2SkinRender::DisplayBackground(cText2SkinItem *Item) {
	printf("DisplayBackground\n");
	if (Item->Path()) {
		char *p;
		asprintf(&p, "%s/%s/%s", SkinPath(), mData->Skin(), Item->Path());
		cText2SkinBitmap bm(p);
		free(p);
		if (Item->HasBg()) bm.SetColor(0, Item->Bg());
		if (Item->HasFg()) bm.SetColor(1, Item->Fg());
		mOsd->DrawBitmap(Item->X(), Item->Y(), bm);
	} else {
		printf("drawing plain background\n");
	}
}

void cText2SkinRender::DisplayLogo(cText2SkinItem *Item) {
}

void cText2SkinRender::DisplayText(cText2SkinItem *Item) {
	printf("DisplayText\n");
	DrawTextTransparent(mOsd, Item->X(), Item->Y(), Item->Text(), Item->Fg(), Item->Bg(), SkinFont(Item), Item->Width(), Item->Height(), Item->Align());
}

void cText2SkinRender::DisplayDateTime(cText2SkinItem *Item) {
	const char *text = DayDateTime(time(NULL));
	DrawTextTransparent(mOsd, Item->X(), Item->Y(), text, Item->Fg(), Item->Bg(), SkinFont(Item), Item->Width(), Item->Height(), Item->Align());
}

void cText2SkinRender::DisplayDate(cText2SkinItem *Item) {
	char *text = strdup(DayDateTime(time(NULL)));
	text[10] = '\0';
	DrawTextTransparent(mOsd, Item->X(), Item->Y(), text + 5, Item->Fg(), Item->Bg(), SkinFont(Item), Item->Width(), Item->Height(), Item->Align());
	free(text);
}

void cText2SkinRender::DisplayTime(cText2SkinItem *Item) {
	char *text = strdup(DayDateTime(time(NULL)));
	text[18] = '\0'; 
	DrawTextTransparent(mOsd, Item->X(), Item->Y(), text + 13, Item->Fg(), Item->Bg(), SkinFont(Item), Item->Width(), Item->Height(), Item->Align());
}

void cText2SkinRender::DisplayChannelNumberName(cText2SkinItem *Item) {
	DrawTextTransparent(mOsd, Item->X(), Item->Y(), ChannelString(mChannel, mNumber), Item->Fg(), Item->Bg(), SkinFont(Item), Item->Width(), Item->Height(), Item->Align());
}

void cText2SkinRender::DisplayChannelNumber(cText2SkinItem *Item) {
	char *text = strdup(ChannelString(mChannel, mNumber));
	char *ptr = text;
	while (*ptr && *ptr != ' ') ++ptr;
	*ptr = '\0';
	DrawTextTransparent(mOsd, Item->X(), Item->Y(), text, Item->Fg(), Item->Bg(), SkinFont(Item), Item->Width(), Item->Height(), Item->Align());
	free(text);
}

void cText2SkinRender::DisplayChannelName(cText2SkinItem *Item) {
	const char *text = ChannelString(mChannel, mNumber);
	while (*text && *text != ' ') ++text;
	if (strlen(text) > 1)
		DrawTextTransparent(mOsd, Item->X(), Item->Y(), text + 1, Item->Fg(), Item->Bg(), SkinFont(Item), Item->Width(), Item->Height(), Item->Align());
}

void cText2SkinRender::DisplayRectangle(cText2SkinItem *Item) {
	mOsd->DrawRectangle(Item->X(), Item->Y(), Item->X() + Item->Width() - 1, Item->Y() + Item->Height() - 1, Item->Fg());

}

void cText2SkinRender::DisplayEllipse(cText2SkinItem *Item) {
	mOsd->DrawEllipse(Item->X(), Item->Y(), Item->X() + Item->Width() - 1, Item->Y() + Item->Height() - 1, Item->Fg());
}

void cText2SkinRender::DisplayTimebar(cText2SkinItem *Item) {
	time_t now = time(NULL);
	if (mPresent && now > mPresent->StartTime()) {
		int total = mPresent->Duration();
		int current = total - (now - mPresent->StartTime());
		if (Item->Width() > Item->Height()) {
			mOsd->DrawRectangle(Item->X(), Item->Y(), Item->X() + Item->Width() - 1, Item->Y() + Item->Height() - 1, Item->Bg());
			mOsd->DrawRectangle(Item->X() + 2, Item->Y() + 2, Item->X() + (Item->Width() * current / total) - 3, Item->Y() + Item->Height() - 3, Item->Fg());
		} else {
			mOsd->DrawRectangle(Item->X(), Item->Y(), Item->X() + Item->Width() - 3, Item->Y() + Item->Height() - 3, Item->Bg());
			mOsd->DrawRectangle(Item->X() + 2, Item->Y() + 2, Item->X() + Item->Width() - 3, Item->Y() + (Item->Height() * current / total) - 3, Item->Fg());
		}
	}
}

void cText2SkinRender::DisplayPresentTime(cText2SkinItem *Item) {
	if (mPresent) {
		const char *text = DayDateTime(mPresent->StartTime());
		DrawTextTransparent(mOsd, Item->X(), Item->Y(), text + 10, Item->Fg(), Item->Bg(), SkinFont(Item), Item->Width(), Item->Height(), Item->Align());
	}
}

void cText2SkinRender::DisplayPresentTitle(cText2SkinItem *Item) {
	if (mPresent && mPresent->Title())
		DrawTextTransparent(mOsd, Item->X(), Item->Y(), mPresent->Title(), Item->Fg(), Item->Bg(), SkinFont(Item), Item->Width(), Item->Height(), Item->Align());
}

void cText2SkinRender::DisplayPresentShortText(cText2SkinItem *Item) {
	if (mPresent && mPresent->ShortText())
		DrawTextTransparent(mOsd, Item->X(), Item->Y(), mPresent->ShortText(), Item->Fg(), Item->Bg(), SkinFont(Item), Item->Width(), Item->Height(), Item->Align());
}

void cText2SkinRender::DisplayFollowingTime(cText2SkinItem *Item) {
	if (mFollowing) {
		const char *text = DayDateTime(mFollowing->StartTime());
		DrawTextTransparent(mOsd, Item->X(), Item->Y(), text + 10, Item->Fg(), Item->Bg(), SkinFont(Item), Item->Width(), Item->Height(), Item->Align());
	}
}

void cText2SkinRender::DisplayFollowingTitle(cText2SkinItem *Item) {
	if (mFollowing && mFollowing->Title())
		DrawTextTransparent(mOsd, Item->X(), Item->Y(), mFollowing->Title(), Item->Fg(), Item->Bg(), SkinFont(Item), Item->Width(), Item->Height(), Item->Align());
}

void cText2SkinRender::DisplayFollowingShortText(cText2SkinItem *Item) {
	if (mFollowing && mFollowing->ShortText())
		DrawTextTransparent(mOsd, Item->X(), Item->Y(), mFollowing->ShortText(), Item->Fg(), Item->Bg(), SkinFont(Item), Item->Width(), Item->Height(), Item->Align());
}

void cText2SkinRender::DisplaySymbol(cText2SkinItem *Item) {
	const char *image = NULL;
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
		default:
			break;
		}
	}
	if (image) {
		char *p;
		asprintf(&p, "%s/%s/%s", SkinPath(), mData->Skin(), image);
		printf("trying %s\n", p);
		cText2SkinBitmap bm(p);
		free(p);
		if (Item->HasBg()) bm.SetColor(0, Item->Bg());
		if (Item->HasFg()) bm.SetColor(1, Item->Fg());
		mOsd->DrawBitmap(Item->X(), Item->Y(), bm);
	}
}

void cText2SkinRender::DisplayVolumebar(cText2SkinItem *Item) {
	if (mVolumeTotal && mVolumeCurrent <= mVolumeTotal) {
		int total = mVolumeTotal;
		int current = mVolumeCurrent;
		if (Item->Width() > Item->Height()) {
			mOsd->DrawRectangle(Item->X(), Item->Y(), Item->X() + Item->Width() - 1, Item->Y() + Item->Height() - 1, Item->Bg());
			mOsd->DrawRectangle(Item->X() + 2, Item->Y() + 2, Item->X() + (Item->Width() * current / total) - 3, Item->Y() + Item->Height() - 3, Item->Fg());
		} else {
			mOsd->DrawRectangle(Item->X(), Item->Y(), Item->X() + Item->Width() - 3, Item->Y() + Item->Height() - 3, Item->Bg());
			mOsd->DrawRectangle(Item->X() + 2, Item->Y() + 2, Item->X() + Item->Width() - 3, Item->Y() + (Item->Height() * current / total) - 3, Item->Fg());
		}
	}
}

void cText2SkinRender::DisplayMute(cText2SkinItem *Item) {
	if (mVolumeMute) {
		if (Item->Path()) {
			char *p;
			asprintf(&p, "%s/%s/%s", SkinPath(), mData->Skin(), Item->Path());
			cText2SkinBitmap bm(p);
			free(p);
			if (Item->HasBg()) bm.SetColor(0, Item->Bg());
			if (Item->HasFg()) bm.SetColor(1, Item->Fg());
			mOsd->DrawBitmap(Item->X(), Item->Y(), bm);
		} 
		
		if (Item->Text()) {
			DrawTextTransparent(mOsd, Item->X(), Item->Y(), Item->Text(), Item->Fg(), Item->Bg(), SkinFont(Item), Item->Width(), Item->Height(), Item->Align());
		}
	}
}

void cText2SkinRender::DisplayProgressbar(cText2SkinItem *Item) {
	if (mReplayTotal && mReplayCurrent <= mReplayTotal) {
		int total = mReplayTotal;
		int current = mReplayCurrent;
		if (Item->Width() > Item->Height()) {
			mOsd->DrawRectangle(Item->X(), Item->Y(), Item->X() + Item->Width() - 1, Item->Y() + Item->Height() - 1, Item->Bg());
			mOsd->DrawRectangle(Item->X() + 2, Item->Y() + 2, Item->X() + (Item->Width() * current / total) - 3, Item->Y() + Item->Height() - 3, Item->Fg());
		} else {
			mOsd->DrawRectangle(Item->X(), Item->Y(), Item->X() + Item->Width() - 3, Item->Y() + Item->Height() - 3, Item->Bg());
			mOsd->DrawRectangle(Item->X() + 2, Item->Y() + 2, Item->X() + Item->Width() - 3, Item->Y() + (Item->Height() * current / total) - 3, Item->Fg());
		}
	}
}
				
void cText2SkinRender::DisplayReplayTitle(cText2SkinItem *Item) {
	if (mReplayTitle)
		DrawTextTransparent(mOsd, Item->X(), Item->Y(), mReplayTitle, Item->Fg(), Item->Bg(), SkinFont(Item), Item->Width(), Item->Height(), Item->Align());
}

void cText2SkinRender::DisplayMenuItems(cText2SkinItem *Item) {
	cText2SkinItem *area = mData->Get(itemMenuArea);
	cText2SkinItem *current = mData->Get(itemMenuCurrent);
	int xoffs = area->X();
	int yoffs = area->Y();

	if (Item->X() != -1)
		xoffs += Item->X();
	if (Item->Y() != -1)
		yoffs += Item->Y();

	printf("menu items\n");

	int index = 0;
	while (yoffs < area->Height() && index < (int)mItems.size()) {
		if (index == mCurrent)
			DrawTextTransparent(mOsd, xoffs + current->X(), yoffs + current->Y(), mItems[index].mName, current->Fg(), current->Bg(), SkinFont(Item), current->Width(), current->Height(), current->Align());
		else
			DrawTextTransparent(mOsd, xoffs + Item->X(), yoffs + Item->Y(), mItems[index].mName, Item->Fg(), Item->Bg(), SkinFont(Item), Item->Width(), Item->Height(), Item->Align());
		yoffs += Item->Height();
		++index;
	}
}
