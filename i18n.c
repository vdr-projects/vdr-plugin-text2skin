/*
 * $Id: i18n.c,v 1.3 2004/06/07 18:23:11 lordjaxom Exp $
 */

#include "i18n.h"

const tI18nPhrase Phrases[] = {
  { "Flush image cache",
    "Bildspeicher leeren",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    ""
  },
  { "Flushing image cache...",
    "Bildspeicher wird geleert...",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    ""
  },
	{ NULL }
};

cText2SkinI18n::cText2SkinI18n(const char *Skin): cText2SkinFile(Skin) {
	mIdentity   = (string)"text2skin_" + Skin;
	mNumPhrases = 0;
	mPhrases    = (tI18nPhrase*)malloc(sizeof(tI18nPhrase));
	memset(mPhrases[mNumPhrases], 0, sizeof(tI18nPhrase));
}

cText2SkinI18n::~cText2SkinI18n() {
}

bool cText2SkinI18n::Parse(const char *Text) {
	int l = strlen(Text);
	bool result = false;
	if (l) {
		if (strncmp(Text, "Item=Translation,", 17) == 0) {
			int i;
			tI18nPhrase p;
			memset(&p, 0, sizeof(tI18nPhrase));
			Text += 17;


			for (i = 0; i < I18nNumLanguages; ++i) {
				char *langs = strdup(I18nLanguageCode(i));
				char *ptr = langs, *ep;
				string text;
				p[i] = "";
				do {
					if ((ep = strchr(ptr, ',')) != NULL)
						*ep = '\0';
					if (ParseVar(Text, ptr, text))
						p[i] = strdup(text.c_str());
					ptr = ep + 1;
				} while (ep != NULL);
				free(langs);
			}

			int idx = mNumPhrases++;
			for (i = 0; i < I18nNumLanguages; ++i)
				if (!p[i]) p[i] = "";
			mPhrases = (tI18nPhrase*)realloc(mPhrases, (mNumPhrases + 1) * sizeof(tI18nPhrase));
			memcpy(mPhrases[idx], p, sizeof(tI18nPhrase));
			memset(mPhrases[mNumPhrases], 0, sizeof(tI18nPhrase));
			result = true;
		} else
			esyslog("ERROR: text2skin: syntax error");
	}
	return result;
}

bool cText2SkinI18n::Load(const string &Filename) {
	if (cText2SkinFile::Load(Filename)) {
		I18nRegister(mPhrases, mIdentity.c_str());
		return true;
	}
	return false;
}

string cText2SkinI18n::Translate(const string &Text) {
	return I18nTranslate(Text.c_str(), mIdentity.c_str());
}
