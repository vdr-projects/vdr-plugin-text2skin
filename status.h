/*
 * $Id: status.h,v 1.2 2004/12/21 18:35:54 lordjaxom Exp $
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
	static cText2SkinStatus *mStatus;

	cText2SkinRender *mRender;
	eReplayMode       mReplayMode;
	tRecordings       mRecordings;
	uint              mCurrentRecording;
	uint              mNextRecording;

protected:
	virtual void Replaying(const cControl *Control, const char *Name);
	virtual void Recording(const cDevice *Device, const char *Name);

public:
	cText2SkinStatus(void);

	void SetRender(cText2SkinRender *Render);

	cxType GetTokenData(const txToken &Token);
};

extern cText2SkinStatus Text2SkinStatus;

#endif // VDR_TEXT2SKIN_STATUS_H
