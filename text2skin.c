/*
 * text2skin.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: text2skin.c,v 1.7 2004/06/01 21:02:38 lordjaxom Exp $
 */

#define __STL_CONFIG_H
#include <vdr/plugin.h>
#undef __STL_CONFIG_H
#include "loader.h"

static const char *VERSION        = "0.0.1-rc3";
static const char *DESCRIPTION    = "Loader for text-based skins";

class cText2SkinPlugin : public cPlugin {
private:
public:
  cText2SkinPlugin(void);
  virtual ~cText2SkinPlugin();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual bool Start(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

cText2SkinPlugin::cText2SkinPlugin(void)
{
}

cText2SkinPlugin::~cText2SkinPlugin()
{
}

bool cText2SkinPlugin::Start(void)
{
	cText2SkinLoader::Start();
  return true;
}

cMenuSetupPage *cText2SkinPlugin::SetupMenu(void)
{
  return NULL;
}

bool cText2SkinPlugin::SetupParse(const char *Name, const char *Value)
{
  return false;
}

VDRPLUGINCREATOR(cText2SkinPlugin); // Don't touch this!
