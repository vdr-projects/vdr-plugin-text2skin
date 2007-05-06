/*
 * text2skin.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: text2skin.c,v 1.7 2005/05/30 10:09:50 lordjaxom Exp $
 */

#include "text2skin.h"
#include "bitmap.h"
#include "setup.h"
#include "menu.h"
#include "i18n.h"
#include "loader.h"
#include "status.h"

const char *cText2SkinPlugin::VERSION        = "1.1-cvs_ext-0.10";
const char *cText2SkinPlugin::SKINVERSION    = "1.1";
const char *cText2SkinPlugin::DESCRIPTION    = "Loader for text-based skins";

cText2SkinPlugin::cText2SkinPlugin(void) {
}

cText2SkinPlugin::~cText2SkinPlugin() {
}

#if VDRVERSNUM >= 10331
const char **cText2SkinPlugin::SVDRPHelpPages(void)
{
	static const char *HelpPages[] = {
		"FLUS\n"
		"    Flush the image cache (useful if images have changed and the"
		"    current version should be loaded).",
		NULL
		};
  return HelpPages;
}

cString cText2SkinPlugin::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
	if (strcasecmp(Command, "FLUS") == 0) {
		// we use the default reply code here
		cText2SkinBitmap::FlushCache();
		return "image cache flushed.";
	}
	return NULL;
}
#endif

bool cText2SkinPlugin::Start(void) {
	RegisterI18n(Phrases);
	Text2SkinStatus.SetLanguage(Setup.OSDLanguage);
	cText2SkinLoader::Start();
	return true;
}

cMenuSetupPage *cText2SkinPlugin::SetupMenu(void) {
	return new cText2SkinSetupPage;
}

bool cText2SkinPlugin::SetupParse(const char *Name, const char *Value) {
  return Text2SkinSetup.SetupParse(Name, Value);
}

VDRPLUGINCREATOR(cText2SkinPlugin); // Don't touch this!
