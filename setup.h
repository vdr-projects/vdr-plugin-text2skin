/*
 * $Id: setup.h,v 1.2 2004/06/12 19:16:11 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_SETUP_H
#define VDR_TEXT2SKIN_SETUP_H

#include "common.h"
#include <vdr/menuitems.h>

class cText2SkinSetup {
public:
	cText2SkinSetup(void);

	bool SetupParse(const char *Name, const char *Value);

	int MaxCacheFill;
};

class cText2SkinSetupPage: public cMenuSetupPage {
private:
	cText2SkinSetup mData;

public:
	cText2SkinSetupPage(void);
	virtual ~cText2SkinSetupPage();

	virtual void Store(void);
};

extern cText2SkinSetup Text2SkinSetup;

#endif // VDR_TEXT2SKIN_SETUP_H
