/*
 * $Id: font.h,v 1.4 2004/06/02 20:43:05 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_FREETYPE_H
#define VDR_TEXT2SKIN_FREETYPE_H

#include "common.h"
#include "data.h"
#include <ft2build.h>
#include FT_FREETYPE_H


// kannst noch einbauen, dass er die fonts "fontOsd" "fontSml" und "fontFix" aus VDR statt aus freetype nimmt

// ==================================
/* cText2SkinFont
	
	Adds interface to FreeTpye font lib.
*/
class cText2SkinFont
{
public:
	cText2SkinFont();
	~cText2SkinFont();

	// font handling
	bool LoadFontFile(string Filename);
	void SetFontSize(int size);
	void DrawTextTransparent(cOsd *Osd, int x, int y, const char *s, tColor ColorFg, int Width, int Height, int Alignment);

private:
	FT_Library		m_library;
	FT_Face			m_face;
	FT_GlyphSlot	m_slot;

};

#endif /*VDR_TEXT2SKIN_FREETYPE_H*/
