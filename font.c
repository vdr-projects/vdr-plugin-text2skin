/*
 * $Id: font.c,v 1.1 2004/12/19 22:03:13 lordjaxom Exp $
 */

#include "font.h"
#include "render.h"
#include <vdr/tools.h>

#ifdef HAVE_FREETYPE
cGraphtftFont cText2SkinFont::mFontCache;
#endif

cText2SkinFont::cText2SkinFont(void)
{
}

cText2SkinFont::~cText2SkinFont()
{
}

const cFont *cText2SkinFont::Load(const std::string &Path, const std::string &Filename, int Size)
{
	if (Filename == "Osd")
		return cFont::GetFont(fontOsd);
	else if (Filename == "Fix")
		return cFont::GetFont(fontFix);
	else if (Filename == "Sml")
		return cFont::GetFont(fontSml);

	const cFont *res = NULL;
#ifdef HAVE_FREETYPE
	char *cachename;
	asprintf(&cachename, "%s_%d_%d", Filename.c_str(), Size, Setup.OSDLanguage);
	Dprintf("trying now: %s %s\n", (Path + "/" + Filename).c_str(), cachename);
	if (mFontCache.Load(Path + "/" + Filename, cachename, Size, Setup.OSDLanguage))
		res = mFontCache.GetFont(cachename);
	else
		esyslog("ERROR: Text2Skin: Couldn't load font %s:%d", Filename.c_str(), Size);
	free(cachename);
#else
	esyslog("ERROR: Text2Skin: Font engine not enabled at compile time!");
#endif
	return res;
}
