/*
 *  $Id: function.c,v 1.4 2005/01/01 23:44:36 lordjaxom Exp $
 */

#include "xml/function.h"
#include "render.h"
#include "bitmap.h"
#include "common.h"
#include <vdr/plugin.h>
#include <vdr/tools.h>

static const char *Internals[] = {
	"not", "and", "or", "equal", "file", "trans", "plugin", NULL
};

const std::string cxFunction::False = "";
const std::string cxFunction::True  = "1";

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

						case fun_eq:   ++params;
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

const std::string &cxFunction::FunFile(const std::string &Param) const
{
	std::string path = cText2SkinRender::ImagePath(Param);
	Dprintf("checking file(%s) in cache\n", path.c_str());
 	return cText2SkinBitmap::Available(path) ? Param : False;
}

std::string cxFunction::FunPlugin(const std::string &Param) const
{
	cPlugin *p = cPluginManager::GetPlugin(Param.c_str());
	if (p) {
		const char *entry = p->MainMenuEntry();
		if (entry)
			return entry;
	}
	return False;
}

std::string cxFunction::Evaluate(void) const
{
	switch (mType) {
	case string:
		return mString.Evaluate();

	case fun_not:
		return mParams[0]->EvaluateToBool() ? False : True;

	case fun_and:
		for (uint i = 0; i < mNumParams; ++i) {
			if (!mParams[i]->EvaluateToBool())
				return False;
		}
		return True;

	case fun_or:
		for (uint i = 0; i < mNumParams; ++i) {
			if (mParams[i]->EvaluateToBool())
				return True;
		}
		return False;

	case fun_eq:
		return mParams[0]->Evaluate() == mParams[1]->Evaluate() ? True : False;

	case fun_file:
		return FunFile(mParams[0]->Evaluate());

	case fun_trans:
		Dprintf("|%s| translates to |%s|\n", mParams[0]->Evaluate().c_str(), tr(mParams[0]->Evaluate().c_str()));
		return tr(mParams[0]->Evaluate().c_str());
	
	case fun_plugin:
		return FunPlugin(mParams[0]->Evaluate());

	default:
		Dprintf("unknown function code\n");
		esyslog("ERROR: Unknown function code called (this shouldn't happen)");
		break;
	}
	return False;
}

bool cxFunction::EvaluateToBool(void) 
{
	std::string result = Evaluate();
	if (result == False/* || result == "0"*/)
		return false;
	return true;
}

