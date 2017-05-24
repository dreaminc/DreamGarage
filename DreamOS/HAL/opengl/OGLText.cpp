#include "OGLText.h"


OGLText::OGLText(OpenGLImp *pParentImp, std::shared_ptr<Font> pFont, const std::string& text, double size, bool isBillboard) :
	text(pFont, text, size, isBillboard),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();

	std::wstring font(L"Fonts/" + pFont->GetGlyphImageFile());
	// Load appropriate glyph texture

	//texture *pColorTexture = new OGLTexture(pParentImp, (wchar_t*)font.c_str(), texture::TEXTURE_TYPE::TEXTURE_COLOR);
	texture *pColorTexture = pParentImp->MakeTexture((wchar_t*)font.c_str(), texture::TEXTURE_TYPE::TEXTURE_COLOR);
	SetColorTexture(pColorTexture);
}

OGLText::OGLText(OpenGLImp *pParentImp, std::shared_ptr<Font> pFont, texture *pFontTexture, const std::string& text, double size, bool isBillboard) :
	text(pFont, text, size, isBillboard),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();

	SetColorTexture(pFontTexture);
}

OGLText* OGLText::SetText(const std::string& text, double size)
{
	bool hasChanged = false;

	text::SetText(text, size, &hasChanged);

	// TODO: need to be able to deal with changing vertex amounts automatically
	if (CheckAndCleanDirty())
	{
		ReleaseOGLBuffers();
		OGLInitialize();
	}
	
	if (hasChanged)
		SetDirty();

	return this;
}
