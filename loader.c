/*
 * $Id: loader.c,v 1.11 2004/06/16 18:46:50 lordjaxom Exp $
 */

#include "loader.h"
#include "data.h"
#include "i18n.h"
#include "theme.h"
#include "display.h"
#include "text2skin.h"
#include <vdr/plugin.h>
#include <sys/types.h>
#include <dirent.h>

void cText2SkinLoader::Start(void) {
	DIR *d = opendir(SkinPath());
	if (d) {
		struct dirent *ent;
		while ((ent = readdir(d)) != NULL) {
			char *path;
			struct stat buf;
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
				continue;
			asprintf(&path, "%s/%s", SkinPath(), ent->d_name);
			if (stat(path, &buf) == 0 && S_ISDIR(buf.st_mode))
				Load(ent->d_name);
			free(path);
		}
		closedir(d);
	}
}

void cText2SkinLoader::Load(const char *Skin) {
	cText2SkinI18n *translations = NULL;
	string transfile = (string)SkinPath() + "/" + Skin + "/" + Skin + ".trans";
	if (access(transfile.c_str(), F_OK) == 0) {
		translations = new cText2SkinI18n(Skin);
		if (!translations->Load(transfile))
			DELETENULL(translations);
	}

	cText2SkinTheme *theme = new cText2SkinTheme(Skin);
	string themefile = (string)SkinPath() + "/" + Skin + "/" + Skin + ".colors";
	theme->Load(themefile);
	
	string skinfile = (string)SkinPath() + "/" + Skin + "/" + Skin + ".skin";
	if (access(skinfile.c_str(), F_OK) == 0) {
		cText2SkinData *data = new cText2SkinData(Skin);
		if (data->Load(skinfile)) {
			cText2SkinItem *skin = data->Get(sectionSkin, itemSkin);
			if (skin) {
				if (skin->Version() == cText2SkinPlugin::ThemeVersion()) {
					new cText2SkinLoader(data, translations, theme, Skin, skin->Name());
					return;
				} else
					esyslog("ERROR: text2skin: Skin %s is version %s, expecting %s", Skin, skin->Version().c_str(), cText2SkinPlugin::ThemeVersion());
				return;
			} else
				esyslog("ERROR: text2skin: Item=Skin is missing in Skin %s", Skin);
		}
		delete data;
	} else
		esyslog("ERROR: text2skin: %s/%s is not a valid skin directory", SkinPath(), Skin);
}

cText2SkinLoader::cText2SkinLoader(cText2SkinData *Data, cText2SkinI18n *I18n, cText2SkinTheme *Theme, const string &Skin, const string &Description): cSkin(Skin.c_str(), Theme->Theme()) {
	mData = Data;
	mI18n = I18n;
	mTheme = Theme;
	mDescription = Description;
}

cText2SkinLoader::~cText2SkinLoader() {
	delete mData;
	delete mI18n;
	delete mTheme;
}

cSkinDisplayChannel *cText2SkinLoader::DisplayChannel(bool WithInfo) {
	Dprintf("WithInfo: %d\n", WithInfo);
	return new cText2SkinDisplayChannel(this, WithInfo);
}

cSkinDisplayMenu *cText2SkinLoader::DisplayMenu(void) {
	return new cText2SkinDisplayMenu(this);
}

cSkinDisplayVolume *cText2SkinLoader::DisplayVolume(void) {
	return new cText2SkinDisplayVolume(this);
}

cSkinDisplayReplay *cText2SkinLoader::DisplayReplay(bool ModeOnly) {
	return new cText2SkinDisplayReplay(this, ModeOnly);
}

cSkinDisplayMessage *cText2SkinLoader::DisplayMessage(void) {
	return new cText2SkinDisplayMessage(this);
}
