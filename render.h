/*
 * $Id: render.h,v 1.1 2004/12/19 22:03:17 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_RENDER_H
#define VDR_TEXT2SKIN_RENDER_H

#include "common.h"
#include "scroller.h"
#include "marquee.h"
#include "xml/skin.h"
#include "xml/type.h"
#include <vdr/osd.h>
#include <vdr/skins.h>
#include <vdr/thread.h>

class cChannel;
class cEvent;
class cText2SkinLoader;
class cText2SkinI18n;
class cText2SkinTheme;
class cText2SkinScreen;

class cText2SkinRender: public cThread {
	friend class cText2SkinDisplayChannel;
	friend class cText2SkinDisplayVolume;
	friend class cText2SkinDisplayReplay;
	friend class cText2SkinDisplayMessage;
	friend class cText2SkinDisplayMenu;

	/* Skin Editor */
	friend class VSkinnerScreen;

private:
	typedef std::map<txToken,cxType> tTokenCache;
	typedef std::vector<cText2SkinMarquee> tMarquees;

	static cText2SkinRender *mRender;

	cxSkin             *mSkin;
	cxDisplay          *mDisplay;
	cText2SkinI18n     *mI18n;
	cText2SkinTheme    *mTheme;
	cText2SkinScreen   *mScreen;
	cText2SkinScroller *mScroller;
	tMarquees           mMarquees;
	tTokenCache         mTokenCache;

	std::string         mBasePath;
	bool                mDirty;
	
	// update thread
	bool                mActive;
	
	cCondVar            mDoUpdate;
	cMutex              mDoUpdateMutex;
	cCondVar            mStarted;
	int                 mUpdateIn;

	// coordinate transformation
	txSize              mBaseSize;
	
protected:
	// Update thread
	void UpdateLock(void) { mDoUpdateMutex.Lock(); }
	void UpdateUnlock(void) { mDoUpdateMutex.Unlock(); }
	virtual void Action(void);

	// Drawing operations
	void DrawObject(const cxObject *Object);
	void DrawBackground(const txPoint &Pos, const txSize &Size, const tColor *Bg, const tColor *Fg, 
	                    int Alpha, const std::string &Path);
	void DrawImage(const txPoint &Pos, const txSize &Size, const tColor *Bg, const tColor *Fg, 
	               int Alpha, int Colors, const std::string &Path);
	void DrawText(const txPoint &Pos, const txSize &Size, const tColor *Fg, const std::string &Text,
	              const cFont *Font, int Align);
	void DrawMarquee(const txPoint &Pos, const txSize &Size, const tColor *Fg, 
	                 const std::string &Text, const cFont *Font, int Align, uint Index);
	void DrawRectangle(const txPoint &Pos, const txSize &Size, 
	                   const tColor *Fg);
	void DrawEllipse(const txPoint &Pos, const txSize &Size, const tColor *Fg, int Arc);
	void DrawSlope(const txPoint &Pos, const txSize &Size, const tColor *Fg, int Arc);
	void DrawProgressbar(const txPoint &Pos, const txSize &Size, int Current, int Total, 
	                     const tColor *Fg, const tColor *Bg, const tColor *Selected, 
	                     const tColor *Mark, const tColor *Cur, const cMarks *Marks = NULL);
	void DrawMark(const txPoint &Pos, const txSize &Size, bool Start, bool Current, bool Horizontal,
	              const tColor *Mark, const tColor *Cur);
	void DrawScrolltext(const txPoint &Pos, const txSize &Size, const tColor *Fg,
	                    const std::string &Text, const cFont *Font, int Align);
	void DrawScrollbar(const txPoint &Pos, const txSize &Size, const tColor *Bg, const tColor *Fg);

	void Update(void);

	// all renderers shall return appropriate data for the tokens
	virtual cxType GetTokenData(const txToken &Token);
	// the replay renderer shall return its marks here
	virtual const cMarks *GetMarks(void) const { return NULL; }
	// the menu renderer shall return its tab information here
	virtual int GetTab(int n) { return 0; }
	virtual bool HasTabText(int Index, int n) { return false; }
	virtual void SetEditableWidth(int Width) {}

	// functions for display renderer to control behaviour
	void Flush(bool Force = false);
	void SetDirty(void) { mDirty = true; }
	void Scroll(bool Up, bool Page) { if (mScroller) mScroller->Scroll(Up, Page); }
	void Clear(void) { DELETENULL(mScroller); mMarquees.clear(); }

public:
	cText2SkinRender(cText2SkinLoader *Loader, cxDisplay::eType Section, 
	                 const std::string &BasePath = "", bool OffScreen = false);
	virtual ~cText2SkinRender();

	// functions for object classes to obtain dynamic item information
	static txPoint Transform(const txPoint &Pos);
	static bool ItemColor(const std::string &Color, tColor &Result);
	static std::string ImagePath(const std::string &Filename);
	static cxType GetToken(const txToken &Token);
};

inline void cText2SkinRender::Flush(bool Force) 
{
	if (mDirty || Force) {
		mTokenCache.clear();

		UpdateLock();
		mDoUpdate.Broadcast();
		UpdateUnlock();
		
		mDirty = false;
	}
}

#endif // VDR_TEXT2SKIN_RENDER_H
