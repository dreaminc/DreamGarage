#include "UIPointerLabel.h"
#include "DreamOS.h"

#include "Primitives/font.h"
#include "Primitives/text.h"
#include "Primitives/FlatContext.h"

#include "HAL/opengl/OGLText.h"

UIPointerLabel::UIPointerLabel(HALImp *pHALImp, DreamOS *pDreamOS) :
	UIView(pHALImp,pDreamOS)
{
	RESULT r = R_PASS;

	PathManager *pPathManager = PathManager::instance();
	std::wstring wstrAssetPath;

	pPathManager->GetValuePath(PATH_ASSET, wstrAssetPath);
	m_pPointerLeft = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wszPointerLeftTexture)[0]);
	m_pPointerCenter = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wszPointerCenterTexture)[0]);
	m_pPointerRight = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wszPointerRightTexture)[0]);

	m_pRenderContext = AddFlatContext();
	m_pRenderContext->RotateXByDeg(90.0f);
	m_pRenderContext->RotateYByDeg(-90.0f);
	m_pRenderContext->SetVisible(false, false);

	m_pFont = m_pDreamOS->MakeFont(L"Basis_Grotesque_Black.fnt", true);
	CN(m_pFont);

Error:
	return;
}

UIPointerLabel::~UIPointerLabel() {
	// empty
}

RESULT UIPointerLabel::Initialize(float parentHeight, std::string strInitials) {
	RESULT r = R_PASS;

	float height = parentHeight/2 * 0.0803;
	float textHeight = 0.75f*height;
	float pxHeight = 61.0f;
	float pxRight = 19.0f;
	float pxLeft = 33.0f;
	float leftWidth = height * pxLeft / pxHeight;
	float rightWidth = height * pxRight / pxHeight;

	m_pFont->SetLineHeight(textHeight);

	auto pText = std::shared_ptr<text>(m_pDreamOS->MakeText(
		m_pFont,
		strInitials,
		0.4,
		textHeight,
		text::flags::FIT_TO_SIZE | text::flags::RENDER_QUAD));

	// assuming only capital letters (A-Z) and centering based on those
	CharacterGlyph periodGlyph; 
	m_pFont->GetGlyphFromChar('A', periodGlyph);
	float glyphHeight = pText->GetMSizeFromDots(periodGlyph.height);
	float offset = (textHeight - glyphHeight) / 8.0f;


	// TODO: the text object should have access to the functionality of the update function
	auto oglText = dynamic_cast<OGLText*>(pText.get());
	oglText->Update();

	float width = pText->GetWidth();

	float screenOffset = 0.01f;

	auto pQuadLeft = m_pRenderContext->AddQuad(leftWidth, height);
	auto pQuadCenter = m_pRenderContext->AddQuad(width, height);
	auto pQuadRight = m_pRenderContext->AddQuad(rightWidth, height);

	pQuadLeft->SetDiffuseTexture(m_pPointerLeft);
	pQuadCenter->SetDiffuseTexture(m_pPointerCenter);
	pQuadRight->SetDiffuseTexture(m_pPointerRight);

	pQuadLeft->SetPosition(-(width + leftWidth) / 2.0f, 0.0f, -offset);
	pQuadCenter->SetPosition(0.0f, 0.0f, -offset);
	pQuadRight->SetPosition((width + rightWidth) / 2.0f, 0.0f, -offset);

	pText->SetPosition(point(0.0f, 0.0f, 0.0f));

	m_pRenderContext->AddObject(pText);

	m_pRenderContext->RenderToQuad(leftWidth + width + rightWidth, height, 0, 0);

Error:
	return r;
}

std::shared_ptr<FlatContext> UIPointerLabel::GetContext() {
	return m_pRenderContext;
}

