/*
 *  $Id: type.c,v 1.2 2005/01/05 19:32:43 lordjaxom Exp $
 */

#include "xml/type.h"
#include "xml/function.h"
#include <vdr/tools.h>
#include <stdio.h>

cxType cxType::False(false);
cxType cxType::True(true);

std::string cxType::String(void) const {
	if (mType == number)
		return (const char*)itoa(mNumber);
	return mString;

#if 0
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
#endif
}
