/*
 * $Id: common.h,v 1.7 2004/06/05 16:52:44 lordjaxom Exp $
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

class cChannel;
class cText2SkinItem;

const char *SkinPath(void);
const char *ChannelNumber(const cChannel *Channel, int Number);
const char *ChannelName(const cChannel *Channel, int Number);
	
bool ParseVar(const char *Text, const char *Name, int *Value);
bool ParseVar(const char *Text, const char *Name, const cFont **Value);
bool ParseVar(const char *Text, const char *Name, string &Value);
bool ParseVar(const char *Text, const char *Name, tColor **Value);
bool ParseVar(const char *Text, const char *Name, eTextAlignment *Value);

#endif // VDR_TEXT2SKIN_COMMON_H
