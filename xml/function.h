/*
 *  $Id: function.h,v 1.5 2005/01/05 19:32:43 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_XML_FUNCTION_H
#define VDR_TEXT2SKIN_XML_FUNCTION_H

#include "xml/string.h"
#include <string>

#define STRING    0x01000000
#define NUMBER    0x02000000
#define INTERNAL  0x04000000

#define MAXPARAMETERS 512

class cxFunction {
public:
	enum eType {
		undefined_function,

		string     = STRING,
		number     = NUMBER,

		fun_not    = INTERNAL + 1,
		fun_and    = INTERNAL + 2,
		fun_or     = INTERNAL + 3,
		fun_eq     = INTERNAL + 4,
		fun_file   = INTERNAL + 5,
		fun_trans  = INTERNAL + 6,
		fun_plugin = INTERNAL + 7
	};

	static const std::string False;
	static const std::string True;
	
private:
	cxSkin     *mSkin;
	eType       mType;
	cxString    mString;
	int         mNumber;
	cxFunction *mParams[MAXPARAMETERS];
	uint        mNumParams;
	
protected:
	cxType FunFile  (const cxType &Param) const;
	cxType FunPlugin(const cxType &Param) const;

public:
	cxFunction(cxSkin *Skin);
	cxFunction(const cxString &String);
	cxFunction(const cxFunction &Src);
	~cxFunction();

	bool Parse(const std::string &Text);
	cxType Evaluate(void) const;

	void SetListIndex(uint Index, int Tab);
};

inline void cxFunction::SetListIndex(uint Index, int Tab)
{
	mString.SetListIndex(Index, Tab);
	for (uint i = 0; i < mNumParams; ++i)
		mParams[i]->SetListIndex(Index, Tab);
}

#endif // VDR_TEXT2SKIN_XML_FUNCTION_H
