#include "UIPointerLabel.h"
#include "DreamOS.h"

#include "DreamShareView/DreamShareView.h"

#include "Primitives/font.h"
#include "Primitives/text.h"
#include "Primitives/FlatContext.h"

#include "HAL/opengl/OGLText.h"

UIPointerLabel::UIPointerLabel(HALImp *pHALImp, DreamOS *pDreamOS) :
	UIView(pHALImp,pDreamOS)
{
	// empty
}

RESULT UIPointerLabel::Initialize() {
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
	return r;
}

UIPointerLabel::~UIPointerLabel() {
	// empty
}

RESULT UIPointerLabel::RenderLabelWithInitials(float parentHeight, std::string strInitials) {
	RESULT r = R_PASS;

	float height = parentHeight/2 * 0.0803;
	float textHeight = 0.75f*height;
	float pxHeight = 84.0f;
	float pxRight = 21.0f;
	float pxLeft = 39.0f;
	float leftWidth = height * pxLeft / pxHeight;
	float rightWidth = height * pxRight / pxHeight;

	m_pFont->SetLineHeight(textHeight);

	auto pText = std::shared_ptr<text>(m_pDreamOS->MakeText(
		m_pFont,
		strInitials,
		0.4,
		textHeight,
		text::flags::FIT_TO_SIZE | text::flags::RENDER_QUAD));

	CN(pText);
	{
		// assuming only capital letters (A-Z) and centering based on those
		CharacterGlyph periodGlyph;
		m_pFont->GetGlyphFromChar('A', periodGlyph);
		float glyphHeight = pText->GetMSizeFromDots(periodGlyph.height);
		float textOffset = (textHeight - glyphHeight) / 8.0f;


		// TODO: the text object should have access to the functionality of the update function
		auto oglText = dynamic_cast<OGLText*>(pText.get());
		oglText->Update();

		float width = pText->GetWidth();
		float totalWidth = leftWidth + width + rightWidth;
		float directionOffset = width/2.0f + leftWidth;

		float screenOffset = 0.01f;

		auto pQuadLeft = m_pRenderContext->AddQuad(leftWidth, height);
		auto pQuadCenter = m_pRenderContext->AddQuad(width, height);
		auto pQuadRight = m_pRenderContext->AddQuad(rightWidth, height);

		CN(pQuadLeft);
		CN(pQuadCenter);
		CN(pQuadRight);

		pQuadLeft->SetDiffuseTexture(m_pPointerLeft);
		pQuadCenter->SetDiffuseTexture(m_pPointerCenter);
		pQuadRight->SetDiffuseTexture(m_pPointerRight);

		pQuadLeft->SetPosition(-(width + leftWidth) / 2.0f, 0.0f, -textOffset);
		pQuadCenter->SetPosition(0.0f, 0.0f, -textOffset);
		pQuadRight->SetPosition((width + rightWidth) / 2.0f, 0.0f, -textOffset);

		pText->SetPosition(point(0.0f, 0.0f, 0.0f));

		m_pRenderContext->AddObject(pText);

		m_pRenderContext->RenderToQuad(totalWidth, height, 0, 0);

		{
			m_pRenderContext->SetPosition(point(-screenOffset, 0.0f, -m_pRenderContext->GetCurrentQuad()->GetWidth()/2.0f));
		}
	}

Error:
	return r;
}

RESULT UIPointerLabel::RenderLabel() {
	return R_PASS;
}

RESULT UIPointerLabel::HandlePointerMessage(DreamShareViewPointerMessage *pUpdatePointerMessage) {
	RESULT r = R_PASS;

	CN(pUpdatePointerMessage);

	//SetPosition(pUpdatePointerMessage->m_body.ptPointer + point(-0.01f, 0.0f, 0.0f));
	SetPosition(pUpdatePointerMessage->m_body.ptPointer);
	m_pRenderContext->SetVisible(pUpdatePointerMessage->m_body.fVisible, false);

Error:
	return r;
}

std::shared_ptr<FlatContext> UIPointerLabel::GetContext() {
	return m_pRenderContext;
}

