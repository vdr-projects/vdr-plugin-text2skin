/*
 * $Id: status.h,v 1.2 2004/12/08 17:13:26 lordjaxom Exp $
 */
 
#ifndef VDR_TEXT2SKIN_STATUS_H
#define VDR_TEXT2SKIN_STATUS_H

#include "common.h"
#include <vdr/status.h>

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

#define __REPLAY_COUNT__ (replayImage+1)
	};

private:
	static cText2SkinStatus *mStatus;
	static const std::string ReplayNames[__REPLAY_COUNT__];

	eReplayMode mReplayMode;

protected:
	virtual void Replaying(const cControl *Control, const char *Name);

public:
	cText2SkinStatus(void);

	static const std::string &ReplayMode(void);
};

inline const std::string &cText2SkinStatus::ReplayMode(void) {
	return ReplayNames[mStatus->mReplayMode];
}

#endif // VDR_TEXT2SKIN_STATUS_H
