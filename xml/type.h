/*
 *  $Id: type.h,v 1.5 2004/12/14 20:02:31 lordjaxom Exp $
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
	cxType(void): mType(boolean), mNumber(0) {}
	cxType(const char *String): mType(string), mString(String ?: "") {}
	cxType(std::string String): mType(string), mString(String) {}
	cxType(int Number): mType(number), mNumber(Number) {}
	cxType(time_t Number): mType(number), mNumber(Number) {}
	cxType(bool Value): mType(boolean), mNumber(Value ? 1 : 0) {}

	const std::string &String(void);
	int                Number(void) const;

	operator std::string () { return String(); }
	operator int         () { return Number(); }
	operator bool        () { return Number(); }
};

inline int cxType::Number(void) const { 
	return mType == number ? mNumber : 0;
}

#endif // VDR_TEXT2SKIN_XML_TYPE_H
