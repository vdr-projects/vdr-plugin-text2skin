/*
 * $Id: common.c,v 1.1.1.1 2004/05/23 00:08:03 lordjaxom Exp $
 */

#include <vdr/plugin.h>
#include "data.h"
#include "common.h"

const char *SkinPath(void) {
	return cPlugin::ConfigDirectory(PLUGIN_NAME_I18N);
}

const cFont *SkinFont(cText2SkinItem *Item) {
	const cFont *font;
	font = cFont::GetFont(fontOsd);
	if (Item->Font()) {
		if      (strcmp(Item->Font(), "Sml") == 0) font = cFont::GetFont(fontSml);
		else if (strcmp(Item->Font(), "Fix") == 0) font = cFont::GetFont(fontFix);
	}
	return font;
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
