/*
 *  $Id: function.c,v 1.8 2005/01/07 21:49:55 lordjaxom Exp $
 */

#include "xml/function.h"
#include "render.h"
#include "bitmap.h"
#include "common.h"
#include <vdr/plugin.h>
#include <vdr/tools.h>

static const char *Internals[] = {
	"not", "and", "or", "equal", "file", "trans", "plugin", "gt",  "lt", "ge", "le", NULL
};

cxFunction::cxFunction(cxSkin *Skin):
		mSkin(Skin),
		mType(string),
		mString(Skin),
		mNumber(0),
		mNumParams(0) 
{
}

cxFunction::cxFunction(const cxString &String):
		mType(string),
		mString(String),
		mNumber(0),
		mNumParams(0)
{
}

cxFunction::cxFunction(const cxFunction &Src):
		mType(Src.mType),
		mString(Src.mString),
		mNumber(Src.mNumber),
		mNumParams(Src.mNumParams)
{
	for (uint i = 0; i < mNumParams; ++i)
		mParams[i] = new cxFunction(*Src.mParams[i]);
}

cxFunction::~cxFunction()
{
	for (uint i = 0; i < mNumParams; ++i)
		delete mParams[i];
}

bool cxFunction::Parse(const std::string &Text) 
{
	const char *text = Text.c_str();
	const char *ptr = text, *last = text;
	eType type = undefined_function;
	int inExpr = 0;
	cxFunction *expr;

	if (*ptr == '\'' || *ptr == '{') {
		// must be string
		if (strlen(ptr) < 2 
				|| (*ptr == '\'' && *(ptr + strlen(ptr) - 1) != '\'')
				|| (*ptr == '{' && *(ptr + strlen(ptr) - 1) != '}')) {
			esyslog("ERROR: Unmatched string end\n");
			return false;
		}

		std::string temp;
		if (*ptr == '\'')
			temp.assign(ptr + 1, strlen(ptr) - 2);
		else
			temp.assign(ptr);

		int pos = -1;
		while ((pos = temp.find("\\'", pos + 1)) != -1)
			temp.replace(pos, 2, "'");

		if (!mString.Parse(temp))
			return false;

		mType = string;
	}
	else if ((*ptr >= '0' && *ptr <= '9') || *ptr == '-' || *ptr == '+') {
		// must be number
		char *end;
		int num = strtol(ptr, &end, 10);
		if (end == ptr || *end != '\0') {
			esyslog("ERROR: Invalid numeric value\n");
			return false;
		}

		mNumber = num;
		mType = number;
	}
	else {
		// expression
		for (; *ptr; ++ptr) {
			if (*ptr == '(') {
				if (inExpr++ == 0) {
					int i;
					for (i = 0; Internals[i] != NULL; ++i) {
						if ((size_t)(ptr - last) == strlen(Internals[i]) 
								&& memcmp(last, Internals[i], ptr - last) == 0){
							type = (eType)(INTERNAL + 1 + i);
							break;
						}
					}

					if (Internals[i] == NULL) {
						esyslog("ERROR: Unknown function %.*s", (int)(ptr - last), last);
						return false;
					}
					last = ptr + 1;
				}
			}
			else if (*ptr == ',' || *ptr == ')') {
				if (inExpr == 0) {
					esyslog("ERROR: Unmatched '%c' in expression", *ptr);
					return false;
				}

				if (inExpr == 1) {
					expr = new cxFunction(mSkin);
					if (!expr->Parse(std::string(last, ptr - last))) {
						delete expr;
						return false;
					}

					if (mNumParams == MAXPARAMETERS) {
						esyslog("ERROR: Too many parameters to function, maximum is %d",
						        MAXPARAMETERS);
						return false;
					}

					mType = type;
					mParams[mNumParams++] = expr;
					last = ptr + 1;
				}

				if (*ptr == ')') {
					if (inExpr == 1) {
						int params = 0;

						switch (mType) {
						case fun_and: 
						case fun_or:   params = -1; break;

						case fun_eq:   
						case fun_gt:
						case fun_lt:
						case fun_ge:
						case fun_le:   ++params;
						case fun_not:
						case fun_trans:
						case fun_plugin:
						case fun_file: ++params;
						default:       break;
						}

						if (params != -1 && mNumParams != (uint)params) {
							esyslog("ERROR: Text2Skin: Wrong number of parameters to %s, " 
									"expecting %d", Internals[mType - 1 - INTERNAL], params);
							return false;
						}
					}

					--inExpr;
				}
			}
		}

		if (inExpr > 0) {
			esyslog("ERROR: Expecting ')' in expression");
			return false;
		}
	}
	
	return true;
}

cxType cxFunction::FunFile(const cxType &Param) const
{
	std::string path = cText2SkinRender::ImagePath(Param);
	Dprintf("checking file(%s) in cache\n", path.c_str());
	return cText2SkinBitmap::Available(path)
	       ? (cxType)Param
	       : (cxType)false;
}

cxType cxFunction::FunPlugin(const cxType &Param) const
{
	cPlugin *p = cPluginManager::GetPlugin(Param);
	if (p) {
		const char *entry = p->MainMenuEntry();
		if (entry)
			return entry;
	}
	return false;
}

cxType cxFunction::Evaluate(void) const
{
	switch (mType) {
	case string:
		return mString.Evaluate();

	case number:
		return mNumber;

	case fun_not:
		return !mParams[0]->Evaluate();

	case fun_and:
		for (uint i = 0; i < mNumParams; ++i) {
			if (!mParams[i]->Evaluate())
				return false;
		}
		return true;

	case fun_or:
		for (uint i = 0; i < mNumParams; ++i) {
			if (mParams[i]->Evaluate())
				return true;
		}
		return false;

	case fun_eq:
		return mParams[0]->Evaluate() == mParams[1]->Evaluate();

	case fun_gt:
		return mParams[0]->Evaluate() >  mParams[1]->Evaluate();

	case fun_lt:
		return mParams[0]->Evaluate() <  mParams[1]->Evaluate();

	case fun_ge:
		return mParams[0]->Evaluate() >= mParams[1]->Evaluate();

	case fun_le:
		return mParams[0]->Evaluate() <= mParams[1]->Evaluate();

	case fun_file:
		return FunFile(mParams[0]->Evaluate());

	case fun_trans:
		//Dprintf("|%s| translates to |%s|\n", mParams[0]->Evaluate().c_str(), tr(mParams[0]->Evaluate().c_str()));
		//return tr(mParams[0]->Evaluate().c_str());
		return mParams[0]->Evaluate();
	
	case fun_plugin:
		return FunPlugin(mParams[0]->Evaluate());

	default:
		Dprintf("unknown function code\n");
		esyslog("ERROR: Unknown function code called (this shouldn't happen)");
		break;
	}
	return false;
}

#if 0
bool cxFunction::EvaluateToBool(void) 
{
	cxType result = Evaluate();
	if (result == False/* || result == "0"*/)
		return false;
	return true;
}
#endif

