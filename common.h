/*
 * $Id: common.h,v 1.1.1.1 2004/05/23 00:08:03 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_COMMON_H
#define VDR_TEXT2SKIN_COMMON_H

class cText2SkinItem;

const char *SkinPath(void);
const cFont *SkinFont(cText2SkinItem *Item);
void DrawTextTransparent(cOsd *Osd, int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font, int Width, int Height, int Alignment);

#endif // VDR_TEXT2SKIN_COMMON_H
