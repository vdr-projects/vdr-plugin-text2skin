/*
 * $Id: common.h,v 1.4 2004/05/31 19:54:12 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_COMMON_H
#define VDR_TEXT2SKIN_COMMON_H

#include <string>

class cText2SkinItem;

#define precond(x) if ((x)) { esyslog("ERROR: text2skin: "#x " not given"); return; }

const char *SkinPath(void);
void DrawTextTransparent(cOsd *Osd, int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font, int Width, int Height, int Alignment);
const char *ChannelNumber(const cChannel *Channel, int Number);
const char *ChannelName(const cChannel *Channel, int Number);
string ItemText(cText2SkinItem *Item, const string &Content);

#endif // VDR_TEXT2SKIN_COMMON_H
