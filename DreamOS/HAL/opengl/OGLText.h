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

class OGLText : public text, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

public:

	OGLText(OpenGLImp *pParentImp, std::shared_ptr<Font> pFont, const std::string& text) :
		text(pFont, text),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();

		// Load appropriate glyph texture
		texture *pColorTexture = new OGLTexture(pParentImp, L"Fonts/Arial.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);

		SetColorTexture(pColorTexture);
	}

	void SetText(const std::string& text)
	{
		text::SetText(text);

		UpdateOGLBuffers();
	}
};

#endif // ! OGL_TEXT_H_
