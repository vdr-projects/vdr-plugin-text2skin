/*
 *  $Id: skin.h,v 1.2 2004/12/21 18:35:55 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_XML_SKIN_H 
#define VDR_TEXT2SKIN_XML_SKIN_H 

#include "xml/display.h"
#include <vdr/osd.h>
#include <expat.h>
#include <map>
#include <string>

// --- cxSkin -----------------------------------------------------------------

class cxSkin {
	friend bool xStartElem(const std::string &name, std::map<std::string,std::string> &attrs);
	friend bool xEndElem(const std::string &name);

	/* Skin Editor */
	friend class VSkinnerView;

public:
	enum eScreenBase {
		relative,
		absolute,
#define __COUNT_BASE__ (absolute + 1)
	};

private:
	eScreenBase mBase;
	txPoint     mBaseOffset;
	txSize      mBaseSize;
	std::string mName;
	std::string mTitle;
	std::string mVersion;
	
	cxDisplays  mDisplays;

public:
	cxSkin(const std::string &Name);

	cxDisplay *Get(cxDisplay::eType Type);

	bool ParseBase(const std::string &Text);
	void SetBase(eScreenBase Base = (eScreenBase)-1);

	eScreenBase        Base(void)       const { return mBase; }
	const txPoint     &BaseOffset(void) const { return mBaseOffset; }
	const txSize      &BaseSize(void)   const { return mBaseSize; }
	const std::string &Name(void)       const { return mName; }
	const std::string &Title(void)      const { return mTitle; }
	const std::string &Version(void)    const { return mVersion; }
};

inline cxDisplay *cxSkin::Get(cxDisplay::eType Type) {
	if (mDisplays.find(Type) != mDisplays.end())
		return mDisplays[Type];
	return NULL;
}

#endif // VDR_TEXT2SKIN_XML_SKIN_H 
