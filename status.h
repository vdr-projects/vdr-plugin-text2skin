/*
 * $Id: status.h,v 1.2 2004/06/12 19:17:06 lordjaxom Exp $
 */
 
#ifndef VDR_TEXT2SKIN_STATUS_H
#define VDR_TEXT2SKIN_STATUS_H

#include "common.h"
#include <vdr/status.h>

class cText2SkinStatus: public cStatus {
private:
	static cText2SkinStatus mStatus;

	eReplayMode mReplayMode;

protected:
	cText2SkinStatus(void);

	virtual void Replaying(const cControl *Control, const char *Name);

public:
	static eReplayMode ReplayMode(void) { return mStatus.mReplayMode; }
};

#endif // VDR_TEXT2SKIN_STATUS_H
