/*
 * $Id: loader.c,v 1.1.1.1 2004/05/23 00:08:03 lordjaxom Exp $
 */

#include <vdr/plugin.h>
#include "loader.h"
#include "data.h"
#include "display.h"
#include "common.h"
#include <sys/types.h>
#include <dirent.h>

static cTheme Theme;

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
	char *skinfile;
	struct stat buf;
	asprintf(&skinfile, "%s/%s/%s.skin", SkinPath(), Skin, Skin);
	if (stat(skinfile, &buf) == 0) {
		cText2SkinData *data = new cText2SkinData(Skin);
		if (data->Load(skinfile)) {
			cText2SkinItem *skin = data->Get(itemSkin);
			if (skin) {
				new cText2SkinLoader(data, Skin, skin->Name());
				return;
			} else
				esyslog("ERROR: Item=Skin is missing in Skin\n");
		}
		delete data;
	} else
		esyslog("ERROR: text2skin: %s/%s is not a valid skin directory\n", SkinPath(), Skin);
}

cText2SkinLoader::cText2SkinLoader(cText2SkinData *Data, const char *Skin, const char *Description): cSkin(Skin, &::Theme) {
	mData = Data;
	mDescription = Description;
}

cText2SkinLoader::~cText2SkinLoader() {
	delete mData;
	// mDescription is part of mData
}

cSkinDisplayChannel *cText2SkinLoader::DisplayChannel(bool WithInfo) {
	return new cText2SkinDisplayChannel(mData, WithInfo);
}

cSkinDisplayMenu *cText2SkinLoader::DisplayMenu(void) {
	return new cText2SkinDisplayMenu(mData);
}

cSkinDisplayVolume *cText2SkinLoader::DisplayVolume(void) {
	return new cText2SkinDisplayVolume(mData);
}

cSkinDisplayReplay *cText2SkinLoader::DisplayReplay(bool ModeOnly) {
	return new cText2SkinDisplayReplay(mData, ModeOnly);
}
cSkinDisplayMessage *cText2SkinLoader::DisplayMessage(void) {
	return new cText2SkinDisplayMessage(mData);
}
