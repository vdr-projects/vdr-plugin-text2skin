/*
 *  $Id: type.h,v 1.3 2004/12/21 20:26:25 lordjaxom Exp $
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
	bool        mCache;
	uint        mUpdateIn;

public:
	static cxType True;
	static cxType False;

	cxType(void): mType(boolean), mNumber(0), mUpdateIn(0) {}
	cxType(const char *String): mType(string), mString(String ?: ""), mUpdateIn(0) {}
	cxType(std::string String): mType(string), mString(String), mUpdateIn(0) {}
	cxType(int Number): mType(number), mNumber(Number), mUpdateIn(0) {}
	cxType(time_t Number): mType(number), mNumber(Number), mUpdateIn(0) {}
	cxType(bool Value): mType(boolean), mNumber(Value ? 1 : 0), mUpdateIn(0) {}

	const std::string &String(void);
	int Number(void) const { return mType == number ? mNumber : 0; }

	void SetUpdate(uint UpdateIn) { mUpdateIn = UpdateIn; }
	uint UpdateIn(void) const { return mUpdateIn; }

	operator std::string () { return String(); }
	operator int         () { return Number(); }
	operator bool        () { return Number(); }
};

#endif // VDR_TEXT2SKIN_XML_TYPE_H
