/*
 * $Id: font.c,v 1.5 2004/12/08 17:23:17 lordjaxom Exp $
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
	asprintf(&cachename, "%s_%d", Filename.c_str(), Size);
	if (mFontCache.Load(Path + "/" + Filename, cachename, Size))
		res = mFontCache.GetFont(cachename);
	else
		esyslog("ERROR: Text2Skin: Couldn't load font %s:%d", Filename.c_str(), Size);
	free(cachename);
#else
	esyslog("ERROR: Text2Skin: Font engine not enabled at compile time!");
#endif
	return res;
}
