/*
 * $Id: i18n.h,v 1.1 2004/06/02 20:43:05 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_I18N_H
#define VDR_TEXT2SKIN_I18N_H

#include "common.h"
#include "file.h"
#include <vdr/i18n.h>

class cText2SkinI18n: public cText2SkinFile {
private:
	string       mIdentity;
	tI18nPhrase *mPhrases;
	int          mNumPhrases;

protected:
	virtual bool Parse(const char *Text);

public:
	cText2SkinI18n(const char *Skin);
	virtual ~cText2SkinI18n();

	virtual bool Load(const string &Filename);
	string Translate(const string &Text);
};

#endif // VDR_TEXT2SKIN_I18N_H

