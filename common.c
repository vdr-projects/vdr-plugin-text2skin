/*
 * $Id: common.c,v 1.6 2004/06/02 20:43:05 lordjaxom Exp $
 */

#include "data.h"
#include "common.h"
#include <vdr/plugin.h>

const char *SkinPath(void) {
	return cPlugin::ConfigDirectory(PLUGIN_NAME_I18N);
}

void DrawTextTransparent(cOsd *Osd, int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font, int Width, int Height, int Alignment) {
	int w = Font->Width(s);
	int h = Font->Height();
	int limit = 0;
	if (Width || Height) {
		int cw = Width ? Width : w;
		limit = x + cw;
		if (Width) {
			 if ((Alignment & taLeft) != 0)
					;
			 else if ((Alignment & taRight) != 0) {
					if (w < Width)
						 x += Width - w;
					}
			 else { // taCentered
					if (w < Width)
						 x += (Width - w) / 2;
					}
			 }
		if (Height) {
			 if ((Alignment & taTop) != 0)
					;
			 else if ((Alignment & taBottom) != 0) {
					if (h < Height)
						 y += Height - h;
					}
			 else { // taCentered
					if (h < Height)
						 y += (Height - h) / 2;
					}
			 }
		}
	while (s && *s) {
			 const cFont::tCharData *CharData = Font->CharData(*s++);
			 if (limit && int(x + CharData->width) > limit)
					break; // we don't draw partial characters
			 if (int(x + CharData->width) > 0) {
					for (int row = 0; row < h; row++) {
							cFont::tPixelData PixelData = CharData->lines[row];
							for (int col = CharData->width; col-- > 0; ) {
									if (PixelData & 1)
										Osd->DrawRectangle(x + col, y + row, x + col, y + row, ColorFg);
									PixelData >>= 1;
									}
							}
					}
			 x += CharData->width;
			 }
}

void DrawBitmap(cOsd *Osd, int x, int y, cBitmap &Bitmap, tColor ColorFg, tColor ColorBg) {
	if (ColorFg || ColorBg) {
		Bitmap.SetColor(0, ColorBg);
		Bitmap.SetColor(1, ColorFg);
	}
	tColor fill = Bitmap.Color(*Bitmap.Data(0, 0)); // to be sure to grab a USED color
	Osd->DrawRectangle(x, y, x + Bitmap.Width() - 1, y + Bitmap.Height() - 1, fill); // to be sure the palette is reset, if the Bitmap covers an Area
 	for (int iy = 0; iy < Bitmap.Height(); iy++) {
		const tIndex *ptr = Bitmap.Data(0, iy);
		for (int ix = 0; ix < Bitmap.Width(); ix++, ptr += sizeof(tIndex)) {
			// DrawPixel is b0rked
			//Osd->DrawPixel(x + ix, y + iy, Bitmap.Color(*ptr));
			Osd->DrawRectangle(x + ix, y + iy, x + ix, y + iy, Bitmap.Color(*ptr));
		}
	}
}

const char *ChannelNumber(const cChannel *Channel, int Number) {
	static char buffer[256];
	buffer[0] = '\0';
  if (Channel) {
     if (!Channel->GroupSep())
        snprintf(buffer, sizeof(buffer), "%d%s", Channel->Number(), Number ? "-" : "");
     }
  else if (Number)
     snprintf(buffer, sizeof(buffer), "%d-", Number);
  return buffer;
}

const char *ChannelName(const cChannel *Channel, int Number) {
	static char buffer[256];
	buffer[0] = '\0';
  if (Channel) 
		 snprintf(buffer, sizeof(buffer), "%s", Channel->Name());
  else if (!Number)
     snprintf(buffer, sizeof(buffer), "%s", tr("*** Invalid Channel ***"));
  return buffer;
}

string ItemText(cText2SkinItem *Item, const string &Content) {
	string s;
	if (Item->Text() != "") {
		s = Item->Text();
		int pos;
		while ((pos = s.find('$')) != -1)
			s.replace(pos, 1, Content);
	} else
		s = Content;
	return s;
}

bool ParseVar(const char *Text, const char *Name, int *Value) {
	string value;
	if (ParseVar(Text, Name, value)) {
		*Value = atoi(value.c_str());
		return true;
	}
	return false;
}

bool ParseVar(const char *Text, const char *Name, string &Value){
	char *ptr1, *ptr2;
	char *str;
	bool res = false;
	asprintf(&str, "%s=", Name);
	if ((ptr1 = strstr(Text, str))) {
		ptr1 += strlen(str);
		if ((ptr2 = strchr(ptr1, ',')) || (ptr2 = strchr(ptr1, ';'))) {
			Value = ptr1;
			Value.erase(ptr2 - ptr1);
			res = true;
		}
	}
	free(str);
	return res;
}

bool ParseVar(const char *Text, const char *Name, tColor **Value) {
	string value;
	if (ParseVar(Text, Name, value) && value[0] == '#') {
		*Value = new tColor(strtoul(value.c_str() + 1, NULL, 16));
		return true;
	}
	return false;
}

bool ParseVar(const char *Text, const char *Name, eTextAlignment *Value) {
	string value;
	if (ParseVar(Text, Name, value)) {
		int v = atoi(value.c_str());
		if (v == 0)
			*Value = (eTextAlignment)(taTop|taLeft);
		else if (v == 1)
			*Value = (eTextAlignment)(taTop|taCenter);
		else if (v == 2)
			*Value = (eTextAlignment)(taTop|taRight);
		return true;
	}
	return false;
}

bool ParseVar(const char *Text, const char *Name, const cFont **Value) {
	string value;
	if (ParseVar(Text, Name, value)) {
		if      (value == "Sml") *Value = cFont::GetFont(fontSml);
		else if (value == "Fix") *Value = cFont::GetFont(fontFix);
		return true;
	}
	return false;
}

