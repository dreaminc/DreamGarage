#include "OGLText.h"

#include "core/text/font.h"

OGLText::OGLText(OGLImp *pParentImp, std::shared_ptr<font> pFont, text::flags textFlags) :
	text(pParentImp, pFont, textFlags),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();
}

OGLText::OGLText(OGLImp *pParentImp, std::shared_ptr<font> pFont, const std::string& strText, double width, double height, bool fBillboard) :
	text(pParentImp, pFont, strText, width, height, fBillboard),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();

	/*
	std::wstring strFont(L"Fonts/" + pFont->GetFontImageFile());
	// Load appropriate glyph texture

	//texture *pColorTexture = new OGLTexture(pParentImp, (wchar_t*)font.c_str(), texture::TEXTURE_TYPE::TEXTURE_COLOR);
	texture *pColorTexture = pParentImp->MakeTexture((wchar_t*)strFont.c_str(), texture::TEXTURE_TYPE::TEXTURE_COLOR);
	SetColorTexture(pColorTexture);
	*/

	//SetColorTexture(pFont->GetTexture().get());
}

OGLText::OGLText(OGLImp *pParentImp, std::shared_ptr<font> pFont, texture *pFontTexture, const std::string& strText, double width, double height, bool fBillboard) :
	text(pParentImp, pFont, strText, width, height, fBillboard),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();

	SetDiffuseTexture(pFontTexture);
}

OGLText::OGLText(OGLImp *pParentImp, std::shared_ptr<font> pFont, const std::string& strContent, double lineHeightM, text::flags textFlags) :
	text(pParentImp, pFont, strContent, lineHeightM, textFlags),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();
	
	//SetColorTexture(pFont->GetTexture().get());
}

OGLText::OGLText(OGLImp *pParentImp, std::shared_ptr<font> pFont, const std::string& strContent, double width, double height, text::flags textFlags) :
	text(pParentImp, pFont, strContent, width, height, textFlags),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();

	// Load appropriate glyph texture
	/*
	std::wstring strFont(L"Fonts/" + pFont->GetFontImageFile());

	//texture *pColorTexture = new OGLTexture(pParentImp, (wchar_t*)font.c_str(), texture::TEXTURE_TYPE::TEXTURE_COLOR);
	texture *pColorTexture = pParentImp->MakeTexture((wchar_t*)strFont.c_str(), texture::TEXTURE_TYPE::TEXTURE_COLOR);
	SetColorTexture(pColorTexture);
	*/

	//SetColorTexture(pFont->GetTexture().get());
}

RESULT OGLText::SetText(const std::string& strText) {
	RESULT r = R_PASS;
	//m_fChanged = false;

	// Update our stuff - prepare it for the next update
	m_strPendingText.assign(strText);

	SetDirty();

Error:
	return r;
}

// OGLText is a composite - no actual internal buffers
RESULT OGLText::OGLInitialize() {
	return R_SKIPPED;
}

RESULT OGLText::Update() {
	RESULT r = R_PASS;

	if (CheckAndCleanDirty()) {

		CR(text::SetText(m_strPendingText));

		//// TODO: need to be able to deal with changing vertex amounts automatically
		//if (CheckAndCleanDirty()) {
		//	CR(ReleaseOGLBuffers());
		//	CR(OGLInitialize());
		//}

		if (r != R_NO_EFFECT) {
			// If the text has changed then do it up
			if (m_pFramebuffer != nullptr && IsRenderToQuad()) {
				CR(RenderToQuad());
			}
		}

		m_fPendingTextChange = false;
	}

Error:
	return r;
}