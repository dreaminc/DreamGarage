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

		std::wstring font(L"Fonts/" + pFont->GetGlyphImageFile());
		// Load appropriate glyph texture

		#pragma message ("bad code - use const std::string")

		texture *pColorTexture = new OGLTexture(pParentImp, (wchar_t*)font.c_str(), texture::TEXTURE_TYPE::TEXTURE_COLOR);

		SetColorTexture(pColorTexture);
	}

	OGLText* SetText(const std::string& text, double size = 1.0)
	{
		text::SetText(text, size);

		SetDirty();

		return this;
	}
};

#endif // ! OGL_TEXT_H_
