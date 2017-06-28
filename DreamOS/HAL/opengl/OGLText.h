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
	OGLText(OpenGLImp *pParentImp, std::shared_ptr<font> pFont, const std::string& strText = "", double size = 1.0f, bool isBillboard = false);
	OGLText(OpenGLImp *pParentImp, std::shared_ptr<font> pFont, texture *pFontTexture, const std::string& strText = "", double size = 1.0f, bool isBillboard = false);
	RESULT SetText(const std::string& text, double size = 1.0);
};

#endif // ! OGL_TEXT_H_
