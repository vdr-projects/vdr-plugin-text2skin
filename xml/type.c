/*
 *  $Id: type.c,v 1.1 2004/12/19 22:03:28 lordjaxom Exp $
 */

#include "xml/type.h"
#include "xml/function.h"
#include <stdio.h>

cxType cxType::False(false);
cxType cxType::True(true);

const std::string &cxType::String(void) {
	if (mType == number) {
		char *buffer;
		asprintf(&buffer, "%d", mNumber);
		mString = buffer;
		mType = string;
		free(buffer);
	} else if (mType == boolean) {
		mString = mNumber ? cxFunction::True : cxFunction::False;
		mType = string;
	}
	return mString;
}
