/*
 * $Id: status.h,v 1.5 2005/01/15 21:02:40 lordjaxom Exp $
 */
 
#ifndef VDR_TEXT2SKIN_STATUS_H
#define VDR_TEXT2SKIN_STATUS_H

#include "common.h"
#include <vdr/status.h>

class cText2SkinRender;

class cText2SkinStatus: public cStatus {
public:
	enum eReplayMode {
		replayNone,
		replayNormal,
		replayMP3,
		replayMPlayer,
		replayDVD,
		replayVCD,
		replayImage,

#define __REPLAY_COUNT__ (cText2SkinStatus::replayImage+1)
	};

	struct tRecordingInfo {
		std::string    name;
		const cDevice *device;

		tRecordingInfo(const std::string &n, const cDevice *d): name(n), device(d) {}
	};
	typedef std::vector<tRecordingInfo> tRecordings;

private:
	cText2SkinRender *mRender;
	eReplayMode       mReplayMode;
	bool              mReplayIsLoop;
	bool              mReplayIsShuffle;
	tRecordings       mRecordings;
	uint              mCurrentRecording;
	uint              mNextRecording;
	int               mLastLanguage;

protected:
	virtual void Replaying(const cControl *Control, const char *Name);
	virtual void Recording(const cDevice *Device, const char *Name);
	virtual void OsdClear(void);

public:
	cText2SkinStatus(void);

	void SetLanguage(int Language) { mLastLanguage = Language; }
	void SetRender(cText2SkinRender *Render);

	cxType GetTokenData(const txToken &Token);

	eReplayMode ReplayMode(void) const { return mReplayMode; }
};

extern cText2SkinStatus Text2SkinStatus;

#endif // VDR_TEXT2SKIN_STATUS_H
