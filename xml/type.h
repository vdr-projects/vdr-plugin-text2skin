/*
 *  $Id: type.h,v 1.6 2004/12/17 19:56:16 lordjaxom Exp $
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

public:
	static cxType True;
	static cxType False;

	cxType(void): mType(boolean), mNumber(0) {}
	cxType(const char *String): mType(string), mString(String ?: "") {}
	cxType(std::string String): mType(string), mString(String) {}
	cxType(int Number): mType(number), mNumber(Number) {}
	cxType(time_t Number): mType(number), mNumber(Number) {}
	cxType(bool Value): mType(boolean), mNumber(Value ? 1 : 0) {}

	const std::string &String(void);
	int Number(void) const { return mType == number ? mNumber : 0; }

	operator std::string () { return String(); }
	operator int         () { return Number(); }
	operator bool        () { return Number(); }
};

#endif // VDR_TEXT2SKIN_XML_TYPE_H
