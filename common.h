/*
 * $Id: common.h,v 1.6 2004/06/02 20:43:05 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_COMMON_H
#define VDR_TEXT2SKIN_COMMON_H

#include <string>
#include <vector>
#include <map>
#include <vdr/osd.h>

using std::string;
using std::vector;
using std::map;

#define precond(x) if ((x)) { esyslog("ERROR: text2skin: "#x " not given"); return; }

class cChannel;
class cText2SkinItem;

const char *SkinPath(void);
void DrawTextTransparent(cOsd *Osd, int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font, int Width, int Height, int Alignment);
void DrawBitmap(cOsd *Osd, int x, int y, cBitmap &Bitmap, tColor ColorFg = 0, tColor ColorBg = 0);
const char *ChannelNumber(const cChannel *Channel, int Number);
const char *ChannelName(const cChannel *Channel, int Number);
	
bool ParseVar(const char *Text, const char *Name, int *Value);
bool ParseVar(const char *Text, const char *Name, const cFont **Value);
bool ParseVar(const char *Text, const char *Name, string &Value);
bool ParseVar(const char *Text, const char *Name, tColor **Value);
bool ParseVar(const char *Text, const char *Name, eTextAlignment *Value);

#endif // VDR_TEXT2SKIN_COMMON_H
