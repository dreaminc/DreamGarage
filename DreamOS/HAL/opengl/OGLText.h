#ifndef OGL_TEXT_H_
#define OGL_TEXT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLText.h

#include <memory>

#include "OGLObj.h"
//#include "Primitives/font.h"
#include "Primitives/text.h"
#include "OGLTexture.h"

class Font;

class OGLText : public text, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

public:
	OGLText(OpenGLImp *pParentImp, std::shared_ptr<Font> pFont, const std::string& text = "", double size = 1.0f, bool isBillboard = false);
	OGLText(OpenGLImp *pParentImp, std::shared_ptr<Font> pFont, texture *pFontTexture, const std::string& text = "", double size = 1.0f, bool isBillboard = false);
	OGLText* SetText(const std::string& text, double size = 1.0);
};

#endif // ! OGL_TEXT_H_
