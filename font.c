/*
 * $Id: font.c,v 1.5 2004/05/30 21:48:21 austriancoder Exp $
 */

#include "font.h"

// ==================================
// constr.
cText2SkinFont::cText2SkinFont()
{
	m_library = 0;
	m_face = 0;

	// init freetype2 lib
	int error = FT_Init_FreeType(&m_library);
	if (error)
	{
		esyslog("ERROR: Could not init freetype library\n");	
	}
}

// ==================================
// deconstr.
cText2SkinFont::~cText2SkinFont()
{
	if (m_face)
	{
		FT_Done_Face(m_face);
	}

	if (m_library)
	{
		FT_Done_FreeType(m_library);
	}
}

// ==================================
// try to load a font
bool cText2SkinFont::LoadFontFile(string Filename)
{
	int error = FT_New_Face(m_library, Filename.c_str(), 0, &m_face);

	// every thing ok?
	if (error == FT_Err_Unknown_File_Format)
	{
		esyslog("ERROR: Font file (%s) could be opened and read, but it appears that its font format is unsupported\n", Filename.c_str());
		return false;
	}
	else if (error)
	{
		esyslog("ERROR: Font file (%s) could be opened or read, or simply it is broken\n", Filename.c_str());
		return false;
	}

	// set slot
	m_slot = m_face->glyph;

	return true;
}

// ==================================
// sets size of font
void cText2SkinFont::SetFontSize(int size)
{
	FT_Set_Char_Size
		(
            m_face,		// handle to face object
            0,			// char_width in 1/64th of points
            size*64,	// char_height in 1/64th of points
            300,		// horizontal device resolution (dpi)
            300			// vertical device resolution (dpi)
		);
}

// ==================================
// write some text :)
void cText2SkinFont::DrawTextTransparent(cOsd *Osd, int x, int y, const char *s, tColor ColorFg, int Width, int Height, int Alignment)
{
	// where to get this infos?
//	int w = Font->Width(s);
//	int h = Font->Height();

	int limit = 0;
	if (Width || Height) 
	{
		int cw = Width ? Width : w;
		limit = x + cw;
		if (Width) 
		{
			if ((Alignment & taLeft) != 0)
				;
			else if ((Alignment & taRight) != 0) 
			{
				if (w < Width)
					x += Width - w;
			}
			else 
			{ 
				// taCentered
				if (w < Width)
					x += (Width - w) / 2;
			}
		}

		if (Height) 
		{
			if ((Alignment & taTop) != 0)
				;
			else if ((Alignment & taBottom) != 0) 
			{
				if (h < Height)
					y += Height - h;
			}
			else 
			{ 
				// taCentered
				if (h < Height)
					y += (Height - h) / 2;
			}
		}
	}

	// write text
	while (s && *s) 
	{
	}
}
