/*
 * $Id: common.h,v 1.9 2004/06/12 18:00:05 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_COMMON_H
#define VDR_TEXT2SKIN_COMMON_H

#include <string>
#include <vector>
#include <map>
#include <vdr/osd.h>

using std::string;
using std::vector;
using std::map;

#ifdef DEBUG
#	define Dprintf(x...) fprintf(stderr, x);
#else
#	define Dprintf(x...)
#endif

// sections and items known by skin files

enum eSkinSection {
	sectionSkin,
	sectionChannelSmall,
	sectionChannel,
	sectionVolume,
	sectionReplayMode,
	sectionReplay,
	sectionMessage,
	sectionMenu,
	__SECTION_COUNT__
};

enum eSkinItem {
	itemUnknown,
	itemSkin, // item identifying the Skin itself
	itemBackground,
	itemText,
	itemScrolltext,
	itemImage,
	itemRectangle,
	itemEllipse,
	itemSlope,
	itemProgress,
	itemLogo,
	itemSymbol,
	itemMenuArea,
	itemMenuItem,
	__ITEM_COUNT__
};

enum eSkinDisplay {
	displayAlways,
	displayDateTimeF,                  // exc: text
	displayDateTime,                   // exc: text
	displayDate,                       // exc: text
	displayTime,                       // exc: text
	displayChannelNumberName,          // exc: text
	displayChannelNumber,              // exc: text
	displayChannelName,                // exc: text, logo
	displayLanguage,                   // exc: text, logo
	displayPresentDateTimeF,           // exc: text
	displayPresentStartTime,           // exc: text
	displayPresentDate,                // exc: text
	displayPresentVPSTime,             // exc: text
	displayPresentEndTime,             // exc: text
	displayPresentDuration,            // exc: text, progress
	displayPresentVPS,                 // exc: symbol
	displayPresentRunning,             // exc: symbol
	displayPresentTimer,               // exc: symbol
	displayPresentTitle,               // exc: text
	displayPresentShortText,           // exc: text
	displayPresentDescription,         // exc: text
	displayFollowingStartTime,         // exc: text, progress
	displayFollowingEndTime,           // exc: text, progress
	displayFollowingDuration,          // exc: text, progress
	displayFollowingTitle,             // exc: text
	displayFollowingShortText,         // exc: text
	displayTeletext,                   // exc: symbol
	displayAudio,                      // exc: symbol
	displayDolby,                      // exc: symbol
	displayEncrypted,                  // exc: symbol
	displayRecording,                  // exc: symbol
	displayRadio,                      // exc: symbol
	displayVolumeCurrent,              // exc: text, progress
	displayVolumeTotal,                // exc: text, progress
	displayMute,                       // exc: symbol
	displayReplayTime,                 // exc: text, progress
	displayReplayDuration,             // exc: text
	displayReplayTitle,                // exc: text
	displayReplayPrompt,               // exc: text
	displayPlay,                       // exc: symbol
	displayPause,                      // exc: symbol
	displayFastFwd,                    // exc: symbol
	displayFastRew,                    // exc: symbol
	displaySlowFwd,                    // exc: symbol
	displaySlowRew,                    // exc: symbol
	displayMessage,                    // exc: text
	displayMessageStatus,              // exc: text
	displayMessageInfo,                // exc: text
	displayMessageWarning,             // exc: text
	displayMessageError,               // exc: text
	displayMenuTitle,                  // exc: text (TODO: logo?)
	displayMenuRed,                    // exc: text
	displayMenuGreen,                  // exc: text
	displayMenuYellow,                 // exc: text
	displayMenuBlue,                   // exc: text
	displayMenuText,                   // exc: text
	displayMenuRecording,              // exc: text
	displayMenuScrollUp,               // exc: symbol
	displayMenuScrollDown,             // exc: symbol
	displayMenuItems,                  // exc: text (TODO: logo?)
	displayMenuCurrent,                // exc: text (TODO: logo?)
	displayMenuGroups,                 // exc: text (TODO: logo?)
	displayReplayMode,                 // exc: text, logo
	__DISPLAY_COUNT__
};

enum eReplayMode {
	replayNone,
	replayNormal,
	replayMP3,
	replayMPlayer,
	replayDVD,
	replayVCD,
	__REPLAY_COUNT__
};
	
extern const string SectionNames[__SECTION_COUNT__];
extern const string ItemNames[__ITEM_COUNT__];
extern const string DisplayNames[__DISPLAY_COUNT__];
extern const string ReplayNames[__REPLAY_COUNT__];

// geometrical structures

struct POINT {
	int x, y;
	POINT &operator+=(const POINT &pt) { x += pt.x; y += pt.y; return *this; }
};

struct SIZE {
	int w, h;
};

// class forwards

class cChannel;
class cText2SkinItem;

// helper functions

const char *SkinPath(void);
const char *ChannelNumber(const cChannel *Channel, int Number);
const char *ChannelName(const cChannel *Channel, int Number);
	
bool ParseVar(const char *Text, const char *Name, eSkinItem *Value);
bool ParseVar(const char *Text, const char *Name, eSkinDisplay *Value);
bool ParseVar(const char *Text, const char *Name, int *Value);
bool ParseVar(const char *Text, const char *Name, const cFont **Value);
bool ParseVar(const char *Text, const char *Name, string &Value);
bool ParseVar(const char *Text, const char *Name, tColor **Value);
bool ParseVar(const char *Text, const char *Name, eTextAlignment *Value);

#endif // VDR_TEXT2SKIN_COMMON_H
