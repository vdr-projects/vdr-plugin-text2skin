/*
 * $Id: setup.h,v 1.1 2004/06/07 19:09:20 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_SETUP_H
#define VDR_TEXT2SKIN_SETUP_H

#include "common.h"
#include <vdr/menuitems.h>

class cText2SkinSetupPage: public cMenuSetupPage {
public:
	cText2SkinSetupPage(void);
	virtual ~cText2SkinSetupPage();

	virtual void Store(void) {}
	virtual eOSState ProcessKey(eKeys Key);
};

#endif // VDR_TEXT2SKIN_SETUP_H
