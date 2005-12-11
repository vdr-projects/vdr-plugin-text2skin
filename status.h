/*
 * $Id: status.h,v 1.7 2005/06/03 08:53:13 lordjaxom Exp $
 */
 
#ifndef VDR_TEXT2SKIN_STATUS_H
#define VDR_TEXT2SKIN_STATUS_H

#ifndef __STL_CONFIG_H
#define __STL_CONFIG_H
#endif

#include "common.h"
#include <vdr/status.h>
#include <algorithm>
#include <vector>

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

	typedef std::string tRecordingInfo;
	typedef std::vector<tRecordingInfo> tRecordings;

	struct tEvent
	{
		std::string    title;
		bool           isRecording;
		std::string    channelName;
		int            channelNumber;
		time_t         startTime;
		time_t         stopTime;
		int            priority;
		
		bool tEvent::operator< (const tEvent &b) const
		{
			int r = startTime - b.startTime;
			r = r == 0 ? b.priority - priority : r;
			return r > 0 ? true : false;
		};
	};
	
	typedef std::vector<tEvent> tEvents;
	
private:
	void UpdateEvents(void);
	
	cText2SkinRender *mRender;
	eReplayMode       mReplayMode;
	bool              mReplayIsLoop;
	bool              mReplayIsShuffle;
	tRecordings       mRecordings;
	tEvents           mEvents;
	const cRecording *mReplay;
	cMutex            mRecordingsLock;
	uint              mCurrentRecording;
	uint              mNextRecording;
	int               mLastLanguage;
	bool              mTimerConflict;

protected:
	virtual void Replaying(const cControl *Control, const char *Name);
	virtual void Recording(const cDevice *Device, const char *Name);
	virtual void OsdClear(void);
	virtual void OsdCurrentItem(const char *Text);

public:
	cText2SkinStatus(void);

	void SetLanguage(int Language) { mLastLanguage = Language; }
	void SetRender(cText2SkinRender *Render);

	cxType GetTokenData(const txToken &Token);

	eReplayMode ReplayMode(void) const { return mReplayMode; }
};

extern cText2SkinStatus Text2SkinStatus;

#endif // VDR_TEXT2SKIN_STATUS_H
