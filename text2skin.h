/*
 * $Id: text2skin.h,v 1.2 2004/06/05 16:53:14 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_H
#define VDR_TEXT2SKIN_H

#include "common.h"
#include <vdr/plugin.h>

class cText2SkinPlugin : public cPlugin {
private:
	static const char *VERSION;
	static const char *THEMEVERSION;
	static const char *DESCRIPTION;
public:
  cText2SkinPlugin(void);
  virtual ~cText2SkinPlugin();
  virtual const char *Version(void) { return VERSION; }
	virtual const char *ThemeVersion(void) { return THEMEVERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual bool Start(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
};

#endif // VDR_TEXT2SKIN_H
