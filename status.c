/* 
 * $Id: status.c,v 1.2 2004/06/12 19:17:06 lordjaxom Exp $
 */
 
#include "status.h"

cText2SkinStatus cText2SkinStatus::mStatus;

cText2SkinStatus::cText2SkinStatus(void) {
	mReplayMode = replayNone;
}

void cText2SkinStatus::Replaying(const cControl *Control, const char *Name) {
	if (Name != NULL) {
		mReplayMode = replayNormal;
		if (strlen(Name) > 6 && Name[0]=='[' && Name[3]==']' && Name[5]=='(') {
			int i;
			for (i = 6; Name[i]; ++i) {
				if (Name[i] == ' ' && Name[i-1] == ')')
					break;
			}
			if (Name[i]) // replaying mp3
				mReplayMode = replayMP3;
		} else if (strcmp(Name, "DVD") == 0)
			mReplayMode = replayDVD;
		else if (strcmp(Name, "VCD") == 0)
			mReplayMode = replayVCD;
		else if (access(Name, F_OK) == 0)
			mReplayMode = replayMPlayer;
		else if (strlen(Name) > 7) {
			int i, n;
			for (i = 0, n = 0; Name[i]; ++i) {
				if (Name[i] == ' ' && Name[i-1] == ',' && ++n == 4)
					break;
			}
			if (Name[i]) { // replaying DVD
				mReplayMode = replayDVD;
			}
		}
	} else
	  mReplayMode = replayNone;
}
