/* 
 * $Id: status.c,v 1.10 2005/06/03 08:53:13 lordjaxom Exp $
 */
 
#include "status.h"
#include "render.h"
#include "menu.h"
#include <vdr/timers.h>
#include <vdr/plugin.h>
#include <vdr/menu.h>

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
			if (Name[i]) { // replaying mp3
				mReplayMode      = replayMP3;
				mReplayIsLoop    = Name[1] == 'L';
				mReplayIsShuffle = Name[2] == 'S';
			}
		}
		/*
		I tried the following, but this is not thread-safe and it seems that
		'LastReplayed()' is not allways up to date, when cStatus::Replaying()
		is called:
		
		else if (const cRecording *rec = GetRecordingByFileName(cReplayControl::LastReplayed()))
		{
			mReplay = rec;
			mReplayMode = replayNormal;
		}
		
		so here is a temporary implementation which has the problem, that several
		recordings with the same name cannot be seperated. This is deactivated
		in Enigma (as it is more ore less useless), till there is a decent fix for
		that.
		*/
		else if (const cRecording *rec = GetRecordingByName(Name))
		{
			mReplay = rec;
			mReplayMode = replayNormal;
		}	
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

void cText2SkinStatus::Recording(const cDevice *Device, const char *Name) 
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
	if (Setup.OSDLanguage != mLastLanguage) {
		mLastLanguage = Setup.OSDLanguage;
		cxString::Reparse();
	}
}

void cText2SkinStatus::OsdCurrentItem(const char *Text)
{
	if (mRender != NULL)
	{
		static std::string lastItem;
		
		lastItem = mRender->mUpdate.currentItem;
		mRender->mUpdate.currentItem = Text;
		mRender->mUpdate.resetMarquee = true;
		mRender->mUpdate.foundFirstItem = false;
	}
}

void cText2SkinStatus::UpdateEvents(void)
{
	if (mRender->mUpdate.events)
	{
		mRender->mUpdate.events = false;
		
		mEvents.clear();
		Timers.IncBeingEdited();
		
		for (cTimer *tim = Timers.First(); tim != NULL; tim = Timers.Next(tim))
		{
			if (tim->HasFlags(tfActive))
			{	
				int i = 0;
				cTimer dummy;
				dummy = *tim; // copy the timer
				
				do
				{
					tEvent ev;
					
					ev.title = dummy.File();
					ev.isRecording = dummy.Recording();
					ev.channelName = dummy.Channel()->Name();
					ev.channelNumber = dummy.Channel()->Number();
					ev.startTime = dummy.StartTime();
					ev.stopTime = dummy.StopTime();
					ev.priority = dummy.Priority();
	
					mEvents.push_back(ev);
							
					if (!dummy.IsSingleEvent()) // add 4 additional rep. timer
					{
						do
						{
							dummy.Skip();
							dummy.Matches(); // Refresh start- and end-time
						} while (!dummy.DayMatches(dummy.Day()));
					}
		
					i++;
				} while (!dummy.IsSingleEvent() && i < 5);
			}
		}
		
		Timers.DecBeingEdited();
		std::sort(mEvents.rbegin(), mEvents.rend());
	}
}

cxType cText2SkinStatus::GetTokenData(const txToken &Token)
{
	uint event = 0;
	
	switch (Token.Type) {
	case tReplayMode:
		return ReplayNames[mReplayMode];

	case tCurrentEventsTitle3:
		event++;
	case tCurrentEventsTitle2:
		event++;
	case tCurrentEventsTitle1:
		UpdateEvents();
		return mEvents.size() > event
			? (cxType)mEvents[event].title.c_str()
			: (cxType)false;
		
	case tCurrentEventsStartDateTime3:
		event++;
	case tCurrentEventsStartDateTime2:
		event++;
	case tCurrentEventsStartDateTime1:
		UpdateEvents();
		return mEvents.size() > event
			? (cxType)TimeType(mEvents[event].startTime, Token.Attrib.Text)
			: (cxType)false;
		
	case tCurrentEventsStopDateTime3:
		event++;
	case tCurrentEventsStopDateTime2:
		event++;
	case tCurrentEventsStopDateTime1:
		UpdateEvents();
		return mEvents.size() > event
			? (cxType)TimeType(mEvents[event].stopTime, Token.Attrib.Text)
			: (cxType)false;

	case tCurrentEventsChannelNumber3:
		event++;
	case tCurrentEventsChannelNumber2:
		event++;
	case tCurrentEventsChannelNumber1:
		UpdateEvents();
		return mEvents.size() > event
			? (cxType)mEvents[event].channelNumber
			: (cxType)false;

	case tCurrentEventsChannelName3:
		event++;
	case tCurrentEventsChannelName2:
		event++;
	case tCurrentEventsChannelName1:
		UpdateEvents();
		return mEvents.size() > event
			? (cxType)mEvents[event].channelName.c_str()
			: (cxType)false;

	case tCurrentEventsIsRecording3:
		event++;
	case tCurrentEventsIsRecording2:
		event++;
	case tCurrentEventsIsRecording1:
		UpdateEvents();
		return mEvents.size() > event
			? (cxType)mEvents[event].isRecording
			: (cxType)false;
		
	case tTimerConflict:
		if (Text2SkinSetup.CheckTimerConflict)
		{
			bool conflict;
			
			if (mRender->mUpdate.timerConflict)
			{
				mRender->mUpdate.timerConflict = false;
				
				if (cPluginManager::CallFirstService("CheckTimerConflict-v1.0", &conflict) )
				{
					mTimerConflict = conflict;
				}
				else
				{
					mTimerConflict = false;
				}
			}
			
			return mTimerConflict;
		}
		else
		{
			return false;
		}
	
	case tReplayName:
		return mReplay != NULL
		       ? (cxType)mReplay->Name()
		       : (cxType)false;

	case tReplayDateTime:
		return mReplay != NULL
		       ? (cxType)TimeType(mReplay->start, Token.Attrib.Text)
		       : (cxType)false;
                         
	case tReplayShortText:
		return mReplay != NULL
		       ? (cxType)mReplay->Info()->ShortText()
		       : (cxType)false;

	case tReplayDescription:
		return mReplay != NULL
		       ? (cxType)mReplay->Info()->Description()
		       : (cxType)false;

	case tReplayLanguageCode:
		if (mReplay)
		{
			const cComponents *components = mReplay->Info()->Components();
			if (components)
			{
				int index = Token.Attrib.Number;
				
				// don't return language-code for the video-stream
				for (int i = 0; i < components->NumComponents(); i++)
				{
					const tComponent *c = components->Component(i);					
					if (c->stream != 2) index++; // only audio-streams
					{
						std::string buffer(c->language);
						if (c->type == 1) buffer.append("MONO");
						if (c->type == 2) buffer.append("DUAL");
						if (c->type == 5) buffer.append("DD");
						return (cxType)buffer.c_str();
					}
				}
			}
		}
		return false;

	case tReplayLanguageDescription: 
		if (mReplay)
		{
			const cComponents *components = mReplay->Info()->Components();
			if (components)
			{
				int index = Token.Attrib.Number;
				
				// don't return language-code for the video-stream
				for (int i = 0; i < components->NumComponents(); i++)
				{
					const tComponent *c = components->Component(i);					
					if (c->stream != 2) index++; // only audio-streams
					if (i == index) return (cxType)c->description;
				}
			}
		}
		return false;

		/*
	case tReplayVideoAR: 
		if (mReplay)
		{
			const cComponents *components = mReplay->Info()->Components();
			if (components)
			{
				for (int i = 0; i < components->NumComponents(); i++)
				{
					const tComponent *c = components->Component(i);					
					if (c->stream == 1)
					{
						switch (c->type)
						{
							case 1: return "4:3";
							case 3: return "16:9";
						}
					}
				}
			}
		}
		return false;
		*/

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
