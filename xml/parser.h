/*
 *  $Id: parser.h,v 1.2 2005/01/01 23:44:36 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_PARSER_H
#define VDR_TEXT2SKIN_PARSER_H

#include <string>

class cxSkin;
class cText2SkinI18n;
class cText2SkinTheme;

cxSkin *xmlParse(const std::string &name, const std::string &fileName, cText2SkinI18n *I18n,
                 cText2SkinTheme *Theme);
	
#endif // VDR_TEXT2SKIN_PARSER_H
