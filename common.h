/*
 * $Id: common.h,v 1.5 2004/06/01 21:02:38 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_COMMON_H
#define VDR_TEXT2SKIN_COMMON_H

#include <string>

class cText2SkinItem;

#define precond(x) if ((x)) { esyslog("ERROR: text2skin: "#x " not given"); return; }

const char *SkinPath(void);
void DrawTextTransparent(cOsd *Osd, int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font, int Width, int Height, int Alignment);
void DrawBitmap(cOsd *Osd, int x, int y, cBitmap &Bitmap, tColor ColorFg = 0, tColor ColorBg = 0);
const char *ChannelNumber(const cChannel *Channel, int Number);
const char *ChannelName(const cChannel *Channel, int Number);
string ItemText(cText2SkinItem *Item, const string &Content);

#endif // VDR_TEXT2SKIN_COMMON_H
