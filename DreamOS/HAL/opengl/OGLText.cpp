#include "OGLText.h"
#include "Primitives/font.h"

OGLText::OGLText(OpenGLImp *pParentImp, std::shared_ptr<font> pFont, const std::string& strText, double size, bool fBillboard) :
	text(pParentImp, pFont, strText, size, fBillboard),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();

	std::wstring strFont(L"Fonts/" + pFont->GetGlyphImageFile());
	// Load appropriate glyph texture

	//texture *pColorTexture = new OGLTexture(pParentImp, (wchar_t*)font.c_str(), texture::TEXTURE_TYPE::TEXTURE_COLOR);
	texture *pColorTexture = pParentImp->MakeTexture((wchar_t*)strFont.c_str(), texture::TEXTURE_TYPE::TEXTURE_COLOR);
	SetColorTexture(pColorTexture);
}

OGLText::OGLText(OpenGLImp *pParentImp, std::shared_ptr<font> pFont, texture *pFontTexture, const std::string& strText, double size, bool isBillboard) :
	text(pParentImp, pFont, strText, size, isBillboard),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();

	SetColorTexture(pFontTexture);
}

RESULT OGLText::SetText(const std::string& strText, double size) {
	RESULT r = R_PASS;
	bool fChanged = false;

	CR(text::SetText(strText, size, &fChanged));

	// TODO: need to be able to deal with changing vertex amounts automatically
	if (CheckAndCleanDirty()) {
		CR(ReleaseOGLBuffers());
		CR(OGLInitialize());
	}
	
	if (fChanged) {
		SetDirty();
	}

Error:
	return r;
}

DimObj *OGLText::GetDimObj() {
	return (DimObj*)this;
}
