/*
 *  $Id: font.c,v 1.4 2005/01/30 18:09:42 lordjaxom Exp $
 *
 * Taken from GraphTFT 
 */

#include "font.h"
#include <iconv.h>
#include <stdio.h>

cGraphtftFont::cGraphtftFont()
{
	_library = 0;
	_face = 0;

	// init freetype2 lib
	int error = FT_Init_FreeType(&_library);
	if (error)
	{
		esyslog("ERROR: Could not init freetype library");	
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

bool cGraphtftFont::Load(string Filename, string CacheName, int Size, int Language, int Width, int format)
{
	if ( _cache.find(CacheName) != _cache.end() )
		return true;	
	cFont* newFont = cFont::CreateFont(Filename.c_str(), Size);
	if ( newFont == NULL )
		return false;
	_cache[CacheName] = newFont;
	return true;
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
}
