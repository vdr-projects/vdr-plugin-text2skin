/*
 *  $Id: type.h,v 1.2 2004/12/21 18:35:55 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_XML_TYPE_H
#define VDR_TEXT2SKIN_XML_TYPE_H

#include <stdio.h>
#include <string>

class cxType {
public:
	enum eType {
		string,
		number,
		boolean
	};

private:
	eType       mType;
	std::string mString;
	int         mNumber;
	bool        mNoCache;

public:
	static cxType True;
	static cxType False;

	cxType(void): mType(boolean), mNumber(0), mNoCache(false) {}
	cxType(const char *String): mType(string), mString(String ?: ""), mNoCache(false) {}
	cxType(std::string String): mType(string), mString(String), mNoCache(false) {}
	cxType(int Number): mType(number), mNumber(Number), mNoCache(false) {}
	cxType(time_t Number): mType(number), mNumber(Number), mNoCache(false) {}
	cxType(bool Value): mType(boolean), mNumber(Value ? 1 : 0), mNoCache(false) {}

	const std::string &String(void);
	int Number(void) const { return mType == number ? mNumber : 0; }

	void SetNoCache(bool Enable = true) { mNoCache = Enable; }
	bool NoCache(void) const { return mNoCache; }

	operator std::string () { return String(); }
	operator int         () { return Number(); }
	operator bool        () { return Number(); }
};

#endif // VDR_TEXT2SKIN_XML_TYPE_H
