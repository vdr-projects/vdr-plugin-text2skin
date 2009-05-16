/*
 * $Id: i18n.h,v 1.1 2004/12/19 22:03:14 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_I18N_H
#define VDR_TEXT2SKIN_I18N_H

#include "common.h"
#include "file.h"
#include <vdr/i18n.h>

#if VDRVERSNUM < 10507
#define trNOOP(s) (s)

extern const tI18nPhrase Phrases[];

class cText2SkinI18n: public cText2SkinFile {
#else
class cText2SkinI18n {
#endif
private:
	std::string  mIdentity;
#if VDRVERSNUM < 10507
	tI18nPhrase *mPhrases;
	int          mNumPhrases;

protected:
	virtual bool Parse(const char *Text);
#endif

public:
	cText2SkinI18n(const char *Skin);
#if VDRVERSNUM < 10507
	virtual ~cText2SkinI18n();

	virtual bool Load(const std::string &Filename);
#endif
	std::string Translate(const std::string &Text) { return I18nTranslate(Text.c_str(), mIdentity.c_str()); }
};

#endif // VDR_TEXT2SKIN_I18N_H

