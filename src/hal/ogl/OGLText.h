#ifndef OGL_TEXT_H_
#define OGL_TEXT_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLText.h

#include <memory>

#include "OGLObj.h"
#include "OGLTexture.h"

#include "core/text/font.h"
#include "core/text/text.h"

class font;

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLText : public text, public virtual OGLObj {

public:

	OGLText(OGLImp *pParentImp, std::shared_ptr<font> pFont, text::flags textFlags);
	OGLText(OGLImp *pParentImp, std::shared_ptr<font> pFont, const std::string& strText = "", double width = 1.0f, double height = 1.0f, bool fBillboard = false);
	OGLText(OGLImp *pParentImp, std::shared_ptr<font> pFont, texture *pFontTexture, const std::string& strText = "", double width = 1.0f, double height = 1.0f, bool fBillboard = false);
	OGLText(OGLImp *pParentImp, std::shared_ptr<font> pFont, const std::string& strContent, double lineHeightM = 0.25f, text::flags textFlags = text::flags::NONE);
	OGLText(OGLImp *pParentImp, std::shared_ptr<font> pFont, const std::string& strContent, double width = 1.0f, double height = 0.25f, text::flags textFlags = text::flags::NONE);
	
	virtual RESULT SetText(const std::string& text) override;

	virtual RESULT OGLInitialize() override;
	virtual RESULT Update() override;

private:
	//bool m_fChanged = false;
	std::string m_strPendingText;
	bool m_fPendingTextChange = false;
};
#pragma warning(pop)

#endif // ! OGL_TEXT_H_
