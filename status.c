/* 
 * $Id: status.c,v 1.11 2006/03/17 18:25:57 lordjaxom Exp $
 */
 
#include "status.h"
#include "render.h"

const std::string ReplayNames[__REPLAY_COUNT__] =
	{ "", "normal", "mp3", "mplayer", "dvd", "vcd", "image" };

cText2SkinStatus Text2SkinStatus;

cText2SkinStatus::cText2SkinStatus(void): 
		mRender(NULL),
		mReplayMode(replayNone),
		mReplayIsLoop(false),
		mReplayIsShuffle(false),
		mRecordings(),
		mCurrentRecording(0),
		mNextRecording(0),
		mLastLanguage(0)
{
}

void cText2SkinStatus::SetRender(cText2SkinRender *Render)
{
	mRender           = Render;
	mCurrentRecording = 0;
	mNextRecording    = 0;
}

void cText2SkinStatus::Replaying(const cControl* /*Control*/, const char *Name,
								 const char *FileName, bool On) 
{
	Dprintf("cText2SkinStatus::Replaying(%s)\n", Name);
	eReplayMode oldMode = mReplayMode;

	if (mRender != NULL)
		mRender->UpdateLock();

	if (On) {
		mReplayMode = replayMPlayer;
		if (strlen(Name) > 6 && Name[0]=='[' && Name[3]==']' && Name[5]=='(') {
			int i;
			for (i = 6; Name[i]; ++i) {
				if (Name[i] == ' ' && Name[i-1] == ')')
					break;
			}
			if (Name[i]) { // replaying mp3
				mReplayMode      = replayMP3;
				mReplayIsLoop    = Name[1] == 'L';
				mReplayIsShuffle = Name[2] == 'S';
			}
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
	} else {
	  mReplayMode = replayNone;
	  mReplayIsLoop = false;
	  mReplayIsShuffle = false;
	}

	if (mRender != NULL) {
		if (mReplayMode != oldMode)
			mRender->SetDirty();
		mRender->UpdateUnlock();
	}
}

void cText2SkinStatus::Recording(const cDevice *Device, const char *Name,
								 const char *FileName, bool On) 
{
	if (mRender != NULL)
		mRender->UpdateLock();

	mRecordingsLock.Lock();
	mRecordings.clear();
	cTimer *t = Timers.First();
	for (; t != NULL; t = Timers.Next(t)) {
		if (t->Recording())
			mRecordings.push_back(t->File());
	}
	mRecordingsLock.Unlock();

	if (mRender != NULL) {
		Dprintf("\nFlushing from cStatus\n\n");
		mRender->Flush(true);
		mRender->UpdateUnlock();
	}
}

void cText2SkinStatus::OsdClear(void) 
{
#if VDRVERSNUM >= 10507
	if (I18nCurrentLanguage() != mLastLanguage) {
		mLastLanguage = I18nCurrentLanguage();
#else
	if (Setup.OSDLanguage != mLastLanguage) {
		mLastLanguage = Setup.OSDLanguage;
#endif
		cxString::Reparse();
	}
}

cxType cText2SkinStatus::GetTokenData(const txToken &Token)
{
	switch (Token.Type) {
	case tReplayMode:
		return ReplayNames[mReplayMode];

	case tCurrentRecording:
		Dprintf("token attrib type is: %d, number: %d\n", Token.Attrib.Type, Token.Attrib.Number);
		if (Token.Attrib.Type == aNumber) {
			return mRecordings.size() > (uint)Token.Attrib.Number
			       ? (cxType)mRecordings[Token.Attrib.Number]
			       : (cxType)false;
		} else if (mRecordings.size() > 0) {
			mRecordingsLock.Lock();
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

			cxType res = mRecordings[mCurrentRecording];
			mRecordingsLock.Unlock();
			res.SetUpdate(next);
			return res;
		}
		return false;

	case tReplayIsLoop:
		return mReplayIsLoop;

	case tReplayIsShuffle:
		return mReplayIsShuffle;

	default:
		break;
	};

	return false;
}
