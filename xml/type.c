/*
 *  $Id: type.c,v 1.2 2004/12/08 17:13:26 lordjaxom Exp $
 */

#include "xml/type.h"
#include "xml/function.h"
#include <stdio.h>

const std::string &cxType::String(void) {
	static char buffer[50];
	if (mType == number) {
		snprintf(buffer, sizeof(buffer), "%d", mNumber);
		mString = buffer;
		mType = string;
	} else if (mType == boolean) {
		mString = mNumber ? cxFunction::True : cxFunction::False;
		mType = string;
	}
	return mString;
}
