/*
 * $Id: common.h,v 1.3 2004/12/08 17:13:25 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_COMMON_H
#define VDR_TEXT2SKIN_COMMON_H

#include "xml/string.h"
#include <string>
#include <vdr/osd.h>

#ifdef DEBUG
#	define Dprintf(x...) fprintf(stderr, x);
#	define Dbench(x) time_t bench_##x = time_ms();
#	define Ddiff(x) time_ms() - bench_##x
#else
#	define Dprintf(x...)
#	define Dbench(x)
#	define Ddiff(x)
#endif

/* Check if a channel is a radio station. */
#define ISRADIO(x) ((x)->Vpid()==0||(x)->Vpid()==1||(x)->Vpid()==0x1fff)

// class forwards

class cMarks;
class cChannel;

// helper functions

std::string SkinPath(void);
const char *ChannelNumber(const cChannel *Channel, int Number);
const char *ChannelName(const cChannel *Channel, int Number);
const char *ChannelShortName(const cChannel *Channel, int Number);
const char *ChannelBouquet(const cChannel *Channel, int Number);

cxType TimeType(time_t Time, const std::string &Format);

bool ParseVar(const char *Text, const char *Name, std::string &Value);
bool ParseVar(const char *Text, const char *Name, tColor *Value);
	
void SkipQuotes(std::string &Value);

#endif // VDR_TEXT2SKIN_COMMON_H
