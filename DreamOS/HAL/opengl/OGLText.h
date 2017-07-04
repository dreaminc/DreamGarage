#ifndef OGL_TEXT_H_
#define OGL_TEXT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLText.h

#include <memory>

#include "OGLObj.h"
#include "Primitives/font.h"
#include "Primitives/text.h"
#include "OGLTexture.h"

class font;

class OGLText : public text, public OGLObj {
protected:
	virtual DimObj *GetDimObj() override;

public:
	OGLText(OpenGLImp *pParentImp, std::shared_ptr<font> pFont, const std::string& strText = "", double width = 1.0f, double height = 1.0f, bool fBillboard = false);
	OGLText(OpenGLImp *pParentImp, std::shared_ptr<font> pFont, texture *pFontTexture, const std::string& strText = "", double width = 1.0f, double height = 1.0f, bool fBillboard = false);
	OGLText(OpenGLImp *pParentImp, std::shared_ptr<font> pFont, const std::string& strContent, double lineHeightM = 0.25f, text::flags textFlags = text::flags::NONE);
	
	virtual RESULT SetText(const std::string& text) override;
};

#endif // ! OGL_TEXT_H_
