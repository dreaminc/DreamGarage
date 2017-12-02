#include "UIControlBar.h"
#include "DreamOS.h"
#include "UIButton.h"

UIControlBar::UIControlBar(HALImp *pHALImp, DreamOS *pDreamOS) :
	UIView(pHALImp,pDreamOS)
{
	RESULT r = R_PASS;

	CR(Initialize());

	Validate();
	return;
Error:
	Invalidate();
	return;

}

UIControlBar::~UIControlBar() {
	// empty
}

RESULT UIControlBar::Initialize() {
	RESULT r = R_PASS;

	// create textures 
	m_pBackTexture = m_pDreamOS->MakeTexture(L"control-view-back.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pForwardTexture = m_pDreamOS->MakeTexture(L"control-view-forward.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pHideTexture = m_pDreamOS->MakeTexture(L"control-view-minimize.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pStopTexture = m_pDreamOS->MakeTexture(L"control-view-stop-sharing.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pShowTexture = m_pDreamOS->MakeTexture(L"control-view-maximize.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pURLTexture = m_pDreamOS->MakeTexture(L"control-view-url.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);

	// create buttons
	m_pBackButton = AddUIButton(m_itemSide, m_itemSide);
	m_pBackButton->GetSurface()->SetDiffuseTexture(m_pBackTexture);

	m_pForwardButton = AddUIButton(m_itemSide, m_itemSide);
	m_pForwardButton->GetSurface()->SetDiffuseTexture(m_pForwardTexture);

	m_pHideButton = AddUIButton(m_itemSide, m_itemSide);
	m_pHideButton->GetSurface()->SetDiffuseTexture(m_pHideTexture);

	m_pStopButton = AddUIButton(m_itemSide, m_itemSide);
	m_pStopButton->GetSurface()->SetDiffuseTexture(m_pStopTexture);

	m_pURLButton = AddUIButton(m_urlWidth, m_itemSide);
	m_pURLButton->GetSurface()->SetDiffuseTexture(m_pURLTexture);

	//TODO: may be useful for alignment
	SetPosition(point(0.0f, -(m_itemSpacing + m_itemSide / 2.0f), 0.0f));

	// set buttons positions based on spec
	point ptStart = point(-m_totalWidth / 2.0f, 0.0f, 0.0f);

	point ptBack = point(m_itemSide / 2.0f, 0.0f, 0.0f);
	m_pBackButton->SetPosition(ptStart + ptBack);

	point ptForward = ptBack + point(m_itemSide + m_itemSpacing, 0.0f, 0.0f);
	m_pForwardButton->SetPosition(ptStart + ptForward);

	// URL button is centered at 0

	point ptHide = point(m_urlWidth / 2.0f + m_itemSpacing + m_itemSide / 2.0f, 0.0f, 0.0f);
	m_pHideButton->SetPosition(ptHide);

	point ptStop = ptHide + point(m_itemSide + m_itemSpacing, 0.0f, 0.0f);
	m_pStopButton->SetPosition(ptStop);

//Error:
	return r;
}

float UIControlBar::GetSpacingOffset() {
	return -(m_itemSpacing + m_itemSide / 2.0f);
}
