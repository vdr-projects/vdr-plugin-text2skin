/*
 *  $Id: parser.h,v 1.1 2004/12/19 22:03:28 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_PARSER_H
#define VDR_TEXT2SKIN_PARSER_H

#include <string>

class cxSkin;

cxSkin *xmlParse(const std::string &name, const std::string &fileName);
	
#endif // VDR_TEXT2SKIN_PARSER_H
