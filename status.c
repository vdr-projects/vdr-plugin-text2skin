/* 
 * $Id: status.c,v 1.2 2004/12/21 18:35:54 lordjaxom Exp $
 */
 
#include "status.h"
#include "render.h"

const std::string ReplayNames[__REPLAY_COUNT__] =
	{ "", "normal", "mp3", "mplayer", "dvd", "vcd", "image" };

cText2SkinStatus *cText2SkinStatus::mStatus = NULL;
cText2SkinStatus Text2SkinStatus;

cText2SkinStatus::cText2SkinStatus(void): 
		mRender(NULL),
		mReplayMode(replayNone),
		mRecordings(),
		mCurrentRecording(0),
		mNextRecording(0)
{
	mStatus = this;
}

void cText2SkinStatus::SetRender(cText2SkinRender *Render)
{
	mRender           = Render;
	mCurrentRecording = 0;
	mNextRecording    = 0;
}

void cText2SkinStatus::Replaying(const cControl* /*Control*/, const char *Name) {
	Dprintf("cText2SkinStatus::Replaying(%s)\n", Name);
	eReplayMode oldMode = mReplayMode;

	if (mRender != NULL)
		mRender->UpdateLock();

	if (Name != NULL) {
		mReplayMode = replayMPlayer;
		if (strlen(Name) > 6 && Name[0]=='[' && Name[3]==']' && Name[5]=='(') {
			int i;
			for (i = 6; Name[i]; ++i) {
				if (Name[i] == ' ' && Name[i-1] == ')')
					break;
			}
			if (Name[i]) // replaying mp3
				mReplayMode = replayMP3;
		} 
		else if (GetRecordingByName(Name) != NULL)
			mReplayMode = replayNormal;
		else if (strcmp(Name, "DVD") == 0)
			mReplayMode = replayDVD;
		else if (strcmp(Name, "VCD") == 0)
			mReplayMode = replayVCD;
		else if (access(Name, F_OK) == 0)
			mReplayMode = replayMPlayer;
		else if (strncmp(Name, "[image]", 7) == 0)
			mReplayMode = replayImage;
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

	if (mRender != NULL) {
		if (mReplayMode != oldMode)
			mRender->SetDirty();
		mRender->UpdateUnlock();
	}
}

void cText2SkinStatus::Recording(const cDevice *Device, const char *Name) 
{
	if (mRender != NULL)
		mRender->UpdateLock();

	if (Name != NULL) {
		tRecordingInfo info(Name, Device);
		mRecordings.push_back(info);
	} else {
		tRecordings::iterator it = mRecordings.begin();
		for (; it != mRecordings.end(); ++it) {
			if ((*it).device == Device && StoppedTimer((*it).name.c_str())) {
				mRecordings.erase(it);
				break;
			}
		}
	}
	
	if (mRender != NULL) {
		mRender->UpdateUnlock();
		mRender->Flush(true);
	}
}

cxType cText2SkinStatus::GetTokenData(const txToken &Token)
{
	switch (Token.Type) {
	case tReplayMode:
		return ReplayNames[mReplayMode];

	case tCurrentRecording:
		if (Token.Attrib.Type == aNumber) {
			return mRecordings.size() > (uint)Token.Attrib.Number
			       ? (cxType)mRecordings[Token.Attrib.Number].name
			       : (cxType)false;
		} else if (mRecordings.size() > 0) {
			uint now = time_ms();
			if (mNextRecording == 0)
				mNextRecording = now + 2000;
			else if (now >= mNextRecording) {
				mCurrentRecording = (mCurrentRecording + 1) % mRecordings.size();
				mNextRecording = now + 2000;
			}
			
			uint next = 0;
			if (mRecordings.size() > 1) {
				next = mNextRecording - now;
				Dprintf("next update in %d ms\n", next);
			}

			cxType res = mRecordings[mCurrentRecording].name;
			res.SetNoCache();
			return res;
		}
		return false;

	default:
		break;
	};

	return cxType::False;
}
