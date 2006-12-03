/*
 * $Id: common.c,v 1.6 2005/01/07 21:46:51 lordjaxom Exp $
 */

#include "common.h"
#include <sstream>
#include <locale.h>
#include <vdr/plugin.h>
#include <vdr/device.h>
#include <sys/ioctl.h>
#include <linux/dvb/frontend.h>

#define FRONTEND_DEVICE "/dev/dvb/adapter%d/frontend%d"

const std::string &SkinPath(void) 
{
	// should never change
	static std::string path = cPlugin::ConfigDirectory(PLUGIN_NAME_I18N);
	return path;
}

const char *ChannelNumber(const cChannel *Channel, int Number) 
{
	static char buffer[256];
	buffer[0] = '\0';
  if (Channel) {
     if (!Channel->GroupSep())
        snprintf(buffer, sizeof(buffer), "%d%s", Channel->Number(), Number ? "-" : "");
     }
  else if (Number)
     snprintf(buffer, sizeof(buffer), "%d-", Number);
  return buffer;
}

const char *ChannelName(const cChannel *Channel, int Number) 
{
	static char buffer[256];
	buffer[0] = '\0';
	if (Channel)
		snprintf(buffer, sizeof(buffer), "%s", Channel->Name());
	else if (!Number)
		snprintf(buffer, sizeof(buffer), "%s", tr("*** Invalid Channel ***"));

#if VDRVERSNUM < 10315
	char *ptr;
	if ((ptr = strchr(buffer, ',')) != NULL
			|| (ptr = strchr(buffer, ';')) != NULL)
		*ptr = '\0';
#endif
	return buffer;
}

const char *ChannelShortName(const cChannel *Channel, int Number) 
{
	static char buffer[256];
	buffer[0] = '\0';
	if (Channel) 
#if VDRVERSNUM < 10315
		snprintf(buffer, sizeof(buffer), "%s", Channel->Name());
#else
		snprintf(buffer, sizeof(buffer), "%s", Channel->ShortName(true));
#endif
	else if (!Number)
		snprintf(buffer, sizeof(buffer), "%s", tr("*** Invalid Channel ***"));

#if VDRVERSNUM < 10315
	char *ptr;
	if ((ptr = strchr(buffer, ',')) != NULL) {
		char *start = ptr + 1;
		if ((ptr = strchr(start, ';')) != NULL)
			*ptr = '\0';
		return start;
	} else if ((ptr = strchr(buffer, ';')) != NULL)
		*ptr = '\0';
#endif
	
	return buffer;
}

const char *EventType(uint Number) 
{
	static char buffer[25];
	buffer[0] = '\0';
	snprintf(buffer, sizeof(buffer), "%d", Number);
	return buffer;
}
/*
const char *ChannelBouquet(const cChannel *Channel, int Number) {
	static char buffer[256];
	buffer[0] = '\0';
	if (Channel) 
#if VDRVERSNUM < 10315
		snprintf(buffer, sizeof(buffer), "%s", Channel->Name());
#else
		snprintf(buffer, sizeof(buffer), "%s", Channel->Provider());
#endif
	else if (!Number)
		snprintf(buffer, sizeof(buffer), "%s", tr("*** Invalid Channel ***"));

#if VDRVERSNUM < 10315
	char *ptr;
	if ((ptr = strchr(buffer, ';')) != NULL)
		return ptr + 1;
	else
		return "";
#else
	return buffer;
#endif
}
*/

bool StoppedTimer(const char *Name) 
{
	cTimer *timer = Timers.First();
	while (timer) {
		if (strcmp(Name, timer->File()) == 0)
			break;
		timer = Timers.Next(timer);
	}
	return timer == NULL || !timer->Recording();
}

const cRecording *GetRecordingByName(const char *Name)
{
	const cRecording *rec = Recordings.First();
	for (; rec != NULL; rec = Recordings.Next(rec)) {
		if (strcmp(rec->Name(), Name) == 0)
			return rec;
	}
	return NULL;
}

const cRecording *GetRecordingByFileName(const char *FileName)
{
	return (FileName) ? Recordings.GetByName(FileName) : NULL;
}

int GetFrontendSTR(void)
{
	uint16_t value = 0;
	char *dev = NULL;
	
	asprintf(&dev, FRONTEND_DEVICE, cDevice::ActualDevice()->CardIndex(), 0);
	int fe = open(dev, O_RDONLY | O_NONBLOCK);
	free(dev);
	if (fe >= 0) {
		CHECK(ioctl(fe, FE_READ_SIGNAL_STRENGTH, &value));
		close(fe);
	}
	return value / 655;
}

int GetFrontendSNR(void)
{
	uint16_t value = 0;
	char *dev = NULL;
	
	asprintf(&dev, FRONTEND_DEVICE, cDevice::ActualDevice()->CardIndex(), 0);
	int fe = open(dev, O_RDONLY | O_NONBLOCK);
	free(dev);
	if (fe >= 0) {
		CHECK(ioctl(fe, FE_READ_SNR, &value));
		close(fe);
	}
	return value / 655;
}

bool GetFrontendHasLock(void)
{
	uint16_t value = 0;
	char *dev = NULL;
	
	asprintf(&dev, FRONTEND_DEVICE, cDevice::ActualDevice()->CardIndex(), 0);
	int fe = open(dev, O_RDONLY | O_NONBLOCK);
	free(dev);
	if (fe >= 0) {
		CHECK(ioctl(fe, FE_READ_STATUS, &value));
		close(fe);
	}
	return value & FE_HAS_LOCK;
}

bool GetFrontendHasSignal(void)
{
	uint16_t value = 0;
	char *dev = NULL;
	
	asprintf(&dev, FRONTEND_DEVICE, cDevice::ActualDevice()->CardIndex(), 0);
	int fe = open(dev, O_RDONLY | O_NONBLOCK);
	free(dev);
	if (fe >= 0) {
		CHECK(ioctl(fe, FE_READ_STATUS, &value));
		close(fe);
	}
	return value & FE_HAS_SIGNAL;
}

std::string AddExtInfoToDescription(const char *Title, const char *ShortText, const char *Description, const char *Aux, bool StripAux)
{
	// max. width so lines don't get wrapped
	#define MAX_CHARS 50
	
	// prepare the description
	std::stringstream desc("");
	
	if (!isempty(Description)) {
		// it seems that sometimes the description ends with a newline
		// and sometimes it does not
		std::string buf(Description);
		while (!buf.empty() && buf[buf.size() - 1] == '\n') buf.erase(buf.size() - 1);
		desc << buf << "\n"; // keep one newline
	}
	
#ifdef DEVELOPMENT_FEATURES
	// try to find a rerun of the show using epgsearch-service
	if (!isempty(Title)) {
		Epgsearch_searchresults_v1_0 data;
		data.query = strdup(Title);
		data.mode = 0;
		data.channelNr = 0;
		data.useTitle = true;
		data.useSubTitle = false;
		data.useDescription = false;
		if (cPluginManager::CallFirstService("Epgsearch-searchresults-v1.0", &data)) {
			cList<Epgsearch_searchresults_v1_0::cServiceSearchResult>* list = data.pResultList;
			if (list) {
				// die aktuelle Sendung wird noch als WH angezeigt !!!
				if (!desc.str().empty()) desc << "\n";
				desc << tr("RERUNS OF THIS SHOW") << ":\n";
				int i = 0;
				for (Epgsearch_searchresults_v1_0::cServiceSearchResult *r = 
				list->First(); r && i < 5; r = list->Next(r)) {
					i++;
					std::stringstream buf;
					buf << " - ";
					buf << *DayDateTime(r->event->StartTime());
					buf << ": " << r->event->Title();
					if (!isempty(r->event->ShortText())) buf << "~" << r->event->ShortText();
					desc << FitToWidth(buf, MAX_CHARS) << "\n";
				}
				delete list;
			}
		}
	}
#endif // DEVELOPMENT_FEATURES 
	
	// Add the AUX-Info of the Recording
	if (Aux) {
		if (StripAux) {
			std::string auxRaw(Aux);
			std::string auxEpgsearch = StripXmlTag(auxRaw, "epgsearch");
			if (!auxEpgsearch.empty()) {
				if (!desc.str().empty()) desc << "\n";
				desc << tr("AUXILIARY INFOS") << ":\n";
				std::stringstream buf;
				buf << " - " << tr("Search timer") << ": " << StripXmlTag(auxRaw, "Search timer");
				desc << FitToWidth(buf, MAX_CHARS) << "\n";
			}
		} else {
			if (!desc.str().empty()) desc << "\n";
			desc << tr("AUXILIARY INFOS") << ":\n";
			desc << Aux << "\n";
		}
	}
	
	return desc.str();
}

int GetRecordingSize(const char *FileName)
#if VDRVERSNUM >= 10338
// use VDR's routine
{
	const cRecording *rec = GetRecordingByFileName(FileName);
	return (rec) ? DirSizeMB(FileName) : 0;
}
#else
// use our own approach
{
	if (FileName != NULL) {
		bool bRet=false;
		long long size = 0;
		int nFiles;
		struct stat fileinfo; // Holds file information structure
		char *cmd = NULL;
#if VDRVERSNUM >= 10318
		cReadLine reader;
#endif
		asprintf(&cmd, "find '%s' -follow -type f -name '*.*'|sort ", FileName);
	
		FILE *p = popen(cmd, "r");
		int ret=0;
		if (p)
		{
			char *s;
			
#if VDRVERSNUM >= 10318
			while ((s = reader.Read(p)) != NULL)
#else
			while ((s = readline(p)) != NULL)
#endif
			{
				if ((ret=stat(s, &fileinfo)) != -1)
				{
					size += (long long)fileinfo.st_size;
					nFiles++;
				}
			}
			
			bRet=true;
		}
		
		pclose(p);
		delete cmd;
		
		return (int)(size / 1024 / 1024); // [MB]
	}
	else
	{
		return 0;
	}
}
#endif

int GetRecordingLength(const char *FileName)
{
	// based on the enAIO-Patch for VDR
	#define INDEXFILESUFFIX   "/index.vdr"
	
	struct tIndex { int offset; uchar type; uchar number; short reserved; };
	tIndex *index;
	char RecLength[21];
	char *filename = NULL;
	int last = -1;
	index = NULL;
	if (FileName) {
	  filename = MALLOC(char, strlen(FileName) + strlen(INDEXFILESUFFIX) + 1);
	  if (filename) {
		  strcpy(filename, FileName);
		  char *pFileExt = filename + strlen(filename);
		  strcpy(pFileExt, INDEXFILESUFFIX);
		  int delta = 0;
		  if (access(filename, R_OK) == 0) {
			  struct stat buf;
			  if (stat(filename, &buf) == 0) {
				  delta = buf.st_size % sizeof(tIndex);
				  if (delta) {
					  delta = sizeof(tIndex) - delta;
					  esyslog("ERROR: invalid file size (%ld) in '%s'", buf.st_size, filename);
					  }
				  last = (buf.st_size + delta) / sizeof(tIndex) - 1;
				  char hour[2], min[3];
#if VDRVERSNUM >= 10318
				  snprintf(RecLength, sizeof(RecLength), "%s", *IndexToHMSF(last, true));
#else
				  snprintf(RecLength, sizeof(RecLength), "%s", IndexToHMSF(last, true));
#endif
				  snprintf(hour, sizeof(hour), "%c", RecLength[0]);
				  snprintf(min, sizeof(min), "%c%c", RecLength[2], RecLength[3]);	
				  return (atoi(hour) * 60) + atoi(min);
				  }
			  }
		  free(filename);
		  }
	  }
	  
	return 0;
}

int GetRecordingCuttedLength(const char *FileName)
{
	cMarks marks;
	double length = 0;
	int totalLength = GetRecordingLength(FileName);
	const double diffIFrame = FRAMESPERSEC / 2; // approx. 1/2 sec.
	
	marks.Load(FileName);
	
	if (marks.Count())
	{
		int start = 1; // first frame
		bool isStart = true;
		
		for (cMark *m = marks.First(); m; m = marks.GetNext(m->position))
		{
			if (isStart)
			{
				start = m->position;
			}
			else
			{
				length += (double)(m->position - start + 1 + diffIFrame) / (FRAMESPERSEC * 60); // [min]
			}
			
			isStart = !isStart;
		}
		
		// if there is no end-mark the last segment goes to the end of the rec.
		if (!isStart) length += totalLength - (double)(start - 1 - diffIFrame) / (FRAMESPERSEC * 60); // [min]
	}
	
	// just to avoid, that the cutted length is bigger than the total length
	return (int)length > totalLength ? totalLength : (int)length;
}

cxType TimeType(time_t Time, const std::string &Format) 
{
	static char result[1000];
	struct tm tm_r, *tm;
	tm = localtime_r(&Time, &tm_r);

	if (Time > 0) {
		if (Format.length() > 0) {
			setlocale(LC_TIME, tr("en_US"));
			strftime(result, sizeof(result), Format.c_str(), tm);
			
			cxType r = result;
			/*if      (Format.find("%s") != -1 || Format.find("%S") != -1
			      || Format.find("%Es") != -1 || Format.find("%ES") != -1
			      || Format.find("%Os") != -1 || Format.find("%OS") != -1)
				r.SetUpdate(1000);
			else if (Format.find("%M") != -1 || Format.find("%EM") != -1
			      || Format.find("%OM") != -1)
				r.SetUpdate(1000*60);*/

			return r;
		} else
			return Time;
	}
	return false;
}

cxType DurationType(uint Index, const std::string &Format) 
{
	static char result[1000];
	if (Index > 0) {
		if (Format.length() > 0) {
			uint update = 0;
			const char *ptr = Format.c_str(); 
			char *res = result;
			enum { normal, format } state = normal;
			int n = 0;
			int f = (Index % FRAMESPERSEC) + 1;
			int s = (Index / FRAMESPERSEC);
			int m = s / 60 % 60;
			int h = s / 3600;
			s %= 60;
			while (*ptr && res < result + sizeof(result)) {
				switch (state) {
				case normal:
					if (*ptr == '%')
						state = format;
					else
						*(res++) = *ptr;
					break;

				case format:
					switch (*ptr) {
					case 'H':
						n = snprintf(res, sizeof(result) - (res - result), "%02d", h);
						break;

					case 'k':
						n = snprintf(res, sizeof(result) - (res - result), "% 2d", h);
						break;

					case 'M':
						n = snprintf(res, sizeof(result) - (res - result), "%02d", m);
						update = 1000*60;
						break;

					case 'm':
						n = snprintf(res, sizeof(result) - (res - result), "%d", m + (h * 60));
						update = 1000*60;
						break;

					case 'S':
						n = snprintf(res, sizeof(result) - (res - result), "%02d", s);
						update = 1000;
						break;
					
					case 'f':
						n = snprintf(res, sizeof(result) - (res - result), "%d", f);
						update = 1000;
						break;

					case '%':
						n = 1;
						*res = '%';
						break;
					}
					res += n;
					state = normal;
					break;
				}
				++ptr;
			}

			cxType r = result;
			r.SetUpdate(update);
			return r;
		} else
			return (int)Index;
	}
	return false;
}

bool ParseVar(const char *Text, const char *Name, std::string &Value) 
{
	const char *ptr1, *ptr2;
	char *str;
	bool res = false;
	asprintf(&str, ",%s=", Name);
	if ((ptr1 = strstr(Text, str)) || (strncmp(ptr1 = Text, str + 1, strlen(str) - 1) == 0)) {
		if (ptr1 == Text)
			--ptr1;
		ptr1 += strlen(str);
		if ((ptr2 = strchr(ptr1, ',')) || (ptr2 = strchr(ptr1, ';'))) {
			int pos;
			Value = ptr1;
			Value.erase(ptr2 - ptr1);
			if (Value[0] == '"')
				SkipQuotes(Value);
			while ((pos = Value.find('$')) != -1)
				Value.replace(pos, 1, "{*}");
			res = true;
		}
	}
	free(str);
	return res;
}

bool ParseVar(const char *Text, const char *Name, tColor *Value) {
	std::string value;
	if (ParseVar(Text, Name, value) && value[0] == '#') {
		*Value = (tColor)strtoul(value.c_str() + 1, NULL, 16);
		return true;
	}
	return false;
}

void SkipQuotes(std::string &Value) {
	char quote = Value[0];
	int i;
	Value.erase(0, 1);
	for (i = 0; i < (int)Value.length() && Value[i] != quote; ++i) {
		if (Value[i] == '\\')
			Value.erase(i, 1);
	}
	if (Value[i] == quote)
		Value.erase(i, 1);
	else
		esyslog("ERROR: text2skin: missing closing %c", quote);
}

std::string FitToWidth(std::string &Line, uint Width) {
	std::string buf(Line);
	if (buf.size() > Width) {
		buf.erase(Width - 3);
		buf.append("...");
	}
	return buf;
}

std::string FitToWidth(std::stringstream &Line, uint Width) {
	std::string buf(Line.str());
	if (buf.size() > Width) {
		buf.erase(Width - 3);
		buf.append("...");
	}
	return buf;
}

std::string StripXmlTag(std::string &Line, const char *Tag) {
	// set the search strings
	std::stringstream strStart, strStop;
	strStart << "<" << Tag << ">";
	strStop << "</" << Tag << ">";
	// find the strings
	std::string::size_type locStart = Line.find(strStart.str());
	std::string::size_type locStop = Line.find(strStop.str());
	if (locStart == std::string::npos || locStop == std::string::npos) return "";
	// extract relevant text
	int pos = locStart + strStart.str().size();
	int len = locStop - pos;
	return len < 0 ? "" : Line.substr(pos, len);
}
