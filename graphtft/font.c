/*
 *  $Id: font.c,v 1.2 2004/12/06 21:19:07 lordjaxom Exp $
 *
 * Taken from GraphTFT 
 */

#include "font.h"

cGraphtftFont::cGraphtftFont()
{
	_library = 0;
	_face = 0;

	// init freetype2 lib
	int error = FT_Init_FreeType(&_library);
	if (error)
	{
		fprintf(stderr, "ERROR: Could not init freetyie library\n");	
	}
}

cGraphtftFont::~cGraphtftFont()
{
	Clear();

	if (_face)
	{
		FT_Done_Face(_face);
	}

	if (_library)
	{
		FT_Done_FreeType(_library);
	}
}

bool cGraphtftFont::Load(string Filename, string CacheName, int Size, int Width)
{
	if ( _cache.find(CacheName) != _cache.end() )
		return true;	
	
	int error = FT_New_Face(_library, Filename.c_str(), 0, &_face);

	// every thing ok?
	if (error == FT_Err_Unknown_File_Format)
	{
		fprintf(stderr, "ERROR: Font file (%s) could be opened and read, but it appears that its font format is unsupported\n", Filename.c_str());
		return false;
	}
	else if (error)
	{
		fprintf(stderr, "ERROR: Font file (%s) could be opened or read, or simply it is broken\n", Filename.c_str());
		return false;
	}

	// set slot
	_slot = _face->glyph;

	// set Size
	FT_Set_Char_Size
	(
		_face,          // handle to face object
		Width*64,       // char_width in 1/64th of points
		Size*64,	// char_height in 1/64th of points
		0,              // horizontal device resolution (dpi)
		0               // vertical device resolution (dpi)
	);

        /* load glyph image into the slot (erase previous one) */
	error = FT_Load_Char( _face, '_', FT_LOAD_RENDER );
	if ( error )
		return  false;

	cFont::tPixelData value;
	int num_rows_global = (_face->size->metrics.height / 64 )+1;
	int num_rows = num_rows_global + 2;
	
	cFont::tPixelData * font_data = new cFont::tPixelData[225 * num_rows];
	
	for (int i = 0; i < 225; i++)
		for (int j = 0; j < num_rows; j++)
			font_data[(i*num_rows)+j]=0x00;
	
	font_data[0+0]=_slot->bitmap.width+2;
	font_data[0+1]=num_rows_global;

	// Time to put char 33..255 in font_data
	FT_UInt glyph_index;	
	for ( int num_char = 33, num_char_array = 1; num_char < 256; num_char++, num_char_array++ )
	{

		 //Get FT char index 
		 glyph_index = FT_Get_Char_Index( _face, num_char );
		
		 //Load the char
		 error = FT_Load_Glyph( _face, glyph_index, FT_LOAD_DEFAULT ); 
		 if ( error ) continue; /* ignore errors */  
		 
		 // convert to an mono bitmap
		 error = FT_Render_Glyph( _face->glyph, ft_render_mode_mono ); 
		 if ( error ) continue;
		
		// now, convert to vdr font data
		int  width = ((_slot->bitmap.width+2) > (int)sizeof(cFont::tPixelData) * 8) ?  (((int)sizeof(cFont::tPixelData) * 8) - 2) : _slot->bitmap.width;
		int  top = _slot->bitmap_top;
		int  y_off = Size - top;

		font_data[(num_char_array*num_rows)+0]=width+2;
		font_data[(num_char_array*num_rows)+1]=num_rows_global;

		unsigned char *bmp = _slot->bitmap.buffer;
		
      		for (int y = 0; y < _slot->bitmap.rows; ++y , y_off++) {
	        	int pos = 0;
			int bit = 0x80;
			value = 0x00;

			for (int x = 0; x < _slot->bitmap.width; ++x) {
				if (bmp[pos] & bit && x < width)
					value = value | (cFont::tPixelData)1 << _slot->bitmap.width-x+1;
				
				bit >>= 1;
				if (bit == 0) {
					bit = 0x80;
					++pos;
				}
			}
			bmp += _slot->bitmap.pitch;
			font_data[(num_char_array*num_rows)+y_off+2] = value;
		}
		
	}
	// If all was ok, put the vdr font into the cache and return true
	cFont* newFont = NULL;
	newFont = new cFont(font_data);
	if (newFont) {
		_cache[CacheName] = newFont;
		_del[CacheName] = font_data;
		return true;
	}
	delete(font_data);
	// Something went wrong!
	return false;
}

const cFont* cGraphtftFont::GetFont(string CacheName){
	if (CacheName == "Sml") return cFont::GetFont(fontSml);
	else if (CacheName == "Fix") return cFont::GetFont(fontFix);
	else if ( _cache.find(CacheName) != _cache.end() ){
		return _cache[CacheName];
	}
	return cFont::GetFont(fontOsd);
}

void cGraphtftFont::Clear(string CacheName)
{
	Clear();	
}

void cGraphtftFont::Clear()
{
	cache_map::iterator it = _cache.begin();
	for (; it != _cache.end(); ++it)
		delete((*it).second);
	_cache.clear();

	del_map::iterator del_it = _del.begin();
	for (; del_it != _del.end(); ++del_it)
		delete((*del_it).second);
	_del.clear();
}
				
cGraphtftFont GraphtftFont;
