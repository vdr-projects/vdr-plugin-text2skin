/*
 *  $Id: parser.c,v 1.7 2005/01/02 16:53:27 lordjaxom Exp $
 */

#include "xml/parser.h"
#include "xml/xml.h"
#include "xml/skin.h"
#include "common.h"
#include <vdr/tools.h>
#include <stdio.h>
#include <vector>
#include <string>

#define TAG_ERR_REMAIN(_context) do { \
		esyslog("ERROR: Text2Skin: Unexpected tag %s within %s", \
				name.c_str(), _context); \
		return false; \
	} while (0)

#define TAG_ERR_CHILD(_context) do { \
		esyslog("ERROR: Text2Skin: No child tag %s expected within %s", \
				name.c_str(), _context); \
		return false; \
	} while (0)

#define TAG_ERR_END(_context) do { \
		esyslog("ERROR: Text2Skin: Unexpected closing tag for %s within %s", \
				name.c_str(), _context); \
		return false; \
	} while (0)

#define ATTRIB_OPT_STRING(_attr,_target) \
	if (attrs.find(_attr) != attrs.end()) { \
		_target = attrs[_attr]; \
	}

#define ATTRIB_MAN_STRING(_attr,_target) \
	ATTRIB_OPT_STRING(_attr,_target) \
	else { \
		esyslog("ERROR: Text2Skin: Mandatory attribute %s missing in tag %s", \
				_attr, name.c_str()); \
		return false; \
	}

#define ATTRIB_OPT_NUMBER(_attr,_target) \
	if (attrs.find(_attr) != attrs.end()) { \
		char *_e; const char *_t = attrs[_attr].c_str(); \
		long _l = strtol(_t, &_e, 10); \
		if (_e ==_t || *_e != '\0') { \
			esyslog("ERROR: Text2Skin: Invalid numeric value \"%s\" in attribute %s", \
					_t, _attr); \
			return false; \
		} else \
			_target = _l; \
	}
		
#define ATTRIB_MAN_NUMBER(_attr,_target) \
	ATTRIB_OPT_NUMBER(_attr,_target) \
	else { \
		esyslog("ERROR: Text2Skin: Mandatory attribute %s missing in tag %s", \
				_attr, name.c_str()); \
		return false; \
	}

#define ATTRIB_OPT_FUNC(_attr,_func) \
	if (attrs.find(_attr) != attrs.end()) { \
		if (!_func(attrs[_attr])) { \
			esyslog("ERROR: Text2Skin: Unexpected value %s for attribute %s", \
					attrs[_attr].c_str(), _attr); \
			return false; \
		} \
	}

#define ATTRIB_MAN_FUNC(_attr,_func) \
	ATTRIB_OPT_FUNC(_attr,_func) \
	else { \
		esyslog("ERROR: Text2Skin: Mandatory attribute %s missing in tag %s", \
				_attr, name.c_str()); \
		return false; \
	}

static std::vector<std::string> context;
static cxSkin    *skin    = NULL;
static cxDisplay *display = NULL;
static cxObject  *parent  = NULL;
static cxObject  *object  = NULL;
static uint       oindex  = 0;

bool xStartElem(const std::string &name, std::map<std::string,std::string> &attrs) {
	//Dprintf("start element: %s\n", name.c_str());

	if      (context.size() == 0) {
		if (name == "skin") {
			ATTRIB_MAN_STRING("version",    skin->mVersion);
			ATTRIB_MAN_STRING("name",       skin->mTitle);
			ATTRIB_MAN_FUNC  ("screenBase", skin->ParseBase);
		} 
		else
			TAG_ERR_REMAIN("document");
	}
	else if (context[context.size() - 1] == "skin") {
		if (name == "display") {
			display = new cxDisplay(skin);
			ATTRIB_MAN_FUNC  ("id",         display->ParseType);
		} 
		else
			TAG_ERR_REMAIN("skin");
	}
	else if (context[context.size() - 1] == "display"
          || context[context.size() - 1] == "list"
          || context[context.size() - 1] == "block") {
		if      (object != NULL) {
			parent = object;
			object = NULL;
		}

		if      (name == "window") {
			if (display->mNumWindows < MAXOSDAREAS) {
				txWindow window;
				ATTRIB_OPT_NUMBER("x1",  window.pos1.x);
				ATTRIB_OPT_NUMBER("y1",  window.pos1.y);
				ATTRIB_OPT_NUMBER("x2",  window.pos2.x);
				ATTRIB_OPT_NUMBER("y2",  window.pos2.y);
				ATTRIB_OPT_NUMBER("bpp", window.bpp);
				display->mWindows[display->mNumWindows++] = window;
			} else
				esyslog("ERROR: Text2Skin: Too many windows in display");
		}
		else {
			object = new cxObject(display);
			if (object->ParseType(name)) {
				ATTRIB_OPT_NUMBER("x1",        object->mPos1.x);
				ATTRIB_OPT_NUMBER("y1",        object->mPos1.y);
				ATTRIB_OPT_NUMBER("x2",        object->mPos2.x);
				ATTRIB_OPT_NUMBER("y2",        object->mPos2.y);
				ATTRIB_OPT_FUNC  ("condition", object->ParseCondition);

				if      (name == "image") {
					ATTRIB_OPT_NUMBER("x",       object->mPos1.x);
					ATTRIB_OPT_NUMBER("y",       object->mPos1.y);
					ATTRIB_OPT_NUMBER("x",       object->mPos2.x);
					ATTRIB_OPT_NUMBER("y",       object->mPos2.y);
					ATTRIB_OPT_NUMBER("alpha",   object->mAlpha);
					ATTRIB_OPT_NUMBER("colors",  object->mColors);
					ATTRIB_OPT_STRING("color",   object->mFg);
					ATTRIB_OPT_STRING("bgColor", object->mBg);
					ATTRIB_MAN_FUNC  ("path",    object->mPath.Parse);
				}
				else if (name == "text"
				      || name == "marquee"
				      || name == "blink"
				      || name == "scrolltext") {
					ATTRIB_OPT_STRING("color",   object->mFg);
					ATTRIB_OPT_FUNC  ("align",   object->ParseAlignment);
					ATTRIB_OPT_FUNC  ("font",    object->ParseFontFace);

					if      (name == "blink") {
						ATTRIB_OPT_STRING("blinkColor", object->mBg);
						ATTRIB_OPT_NUMBER("delay",      object->mDelay);
						
						if (object->mDelay == 0)
							object->mDelay = 1000;
					}
					else if (name == "marquee") {
						ATTRIB_OPT_NUMBER("delay",      object->mDelay);

						if (object->mDelay == 0)
							object->mDelay = 500;
					}
				}
				else if (name == "rectangle") {
					ATTRIB_OPT_STRING("color",   object->mFg);
				}
				else if (name == "ellipse" || name == "slope") {
					ATTRIB_OPT_STRING("color",   object->mFg);
					ATTRIB_OPT_NUMBER("arc",     object->mArc);
				}
				else if (name == "progress" 
				      || name == "scrollbar") {
					ATTRIB_OPT_STRING("color",   object->mFg);
					ATTRIB_OPT_STRING("bgColor", object->mBg);
					ATTRIB_OPT_STRING("mark",    object->mMark);
					ATTRIB_OPT_STRING("active",  object->mActive);
					ATTRIB_OPT_STRING("keep",    object->mKeep);
					ATTRIB_OPT_FUNC  ("current", object->mCurrent.Parse);
					ATTRIB_OPT_FUNC  ("total",   object->mTotal.Parse);
				}
				else if (name == "item") {
					ATTRIB_MAN_NUMBER("height",  object->mPos2.y);
					--object->mPos2.y;
				}
			} else
				TAG_ERR_REMAIN(context[context.size() - 1].c_str());
		}
	} else 
		TAG_ERR_CHILD(context[context.size() - 1].c_str());
	context.push_back(name);
	return true;
}

bool xCharData(const std::string &text) {
	int start = 0, end = text.length() - 1;

	//Dprintf("char data before: %s\n", text.c_str());

	while (text[start] == '\012' || text[start] == '\015' || text[start] == ' ' || text[start] == '\t')
		++start;

	while (text[end] == '\012' || text[end] == '\015' || text[end] == ' ' || text[end] == '\t')
		--end;
	
	//Dprintf("char data after: %s\n", text.substr(start, end - start + 1).c_str());

	if (end - start + 1 > 0) {
		//Dprintf("context: %s\n", context[context.size() - 1].c_str());
		if      (context[context.size() - 1] == "text"
		      || context[context.size() - 1] == "marquee"
		      || context[context.size() - 1] == "blink"
		      || context[context.size() - 1] == "scrolltext") {
			if (!object->mText.Parse(text.substr(start, end - start + 1)))
				return false;
		} else
			esyslog("ERROR: Bad character data");
	}
	return true;
}

bool xEndElem(const std::string &name) {
	//Dprintf("end element: %s\n", name.c_str());
	if (context[context.size() - 1] == name) {
		if      (name == "display") {
			//display->mNumMarquees = mindex;
			skin->mDisplays[display->Type()] = display;
			display = NULL;
			oindex = 0;
		}
		else if (object != NULL || parent != NULL) {
			if (object == NULL) {
				//Dprintf("rotating parent to object\n");
				object = parent;
				parent = NULL;
			}

			if (object->mCondition == NULL) {
				switch (object->mType) {
				case cxObject::text:
				case cxObject::marquee:
				case cxObject::blink:
				case cxObject::scrolltext:
					object->mCondition = new cxFunction(object->mText);
					break;

				default:
					break;
				}
			}

			object->mIndex = oindex++;
			if (parent != NULL) {
				//Dprintf("pushing to parent\n");
				if (parent->mObjects == NULL)
					parent->mObjects = new cxObjects();
				parent->mObjects->push_back(object);
			}
			else
				display->mObjects.push_back(object);
			object = NULL;
		}
		context.pop_back();
	} else
		TAG_ERR_END(context[context.size() - 1].c_str());
	return true;
}

cxSkin *xmlParse(const std::string &name, const std::string &fileName, cText2SkinI18n *I18n,
                 cText2SkinTheme *Theme) {
	skin = new cxSkin(name, I18n, Theme);
	context.clear();

	XML xml(fileName);
	xml.nodeStartCB(xStartElem);
	xml.nodeEndCB(xEndElem);
	xml.cdataCB(xCharData);
	if (xml.parse() != 0) {
		esyslog("ERROR: Text2Skin: Parse error in %s, line %d", fileName.c_str(), xml.lineNr());
		DELETENULL(skin);
		DELETENULL(display);
		DELETENULL(object);
		return NULL;
	}

	cxSkin *result = skin;
	skin = NULL;
	return result;
}
	
