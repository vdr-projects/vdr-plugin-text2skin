/*
 *  $Id: type.c,v 1.3 2004/12/12 20:26:25 lordjaxom Exp $
 */

#include "xml/type.h"
#include "xml/function.h"
#include <stdio.h>

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
