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

	CN(m_pDreamOS);
	m_pDreamOS->AddObjectToInteractionGraph(this);

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

	m_pToggleButton = AddUIButton(m_itemSide, m_itemSide);
	m_pToggleButton->GetSurface()->SetDiffuseTexture(m_pHideTexture);

	m_pStopButton = AddUIButton(m_itemSide, m_itemSide);
	m_pStopButton->GetSurface()->SetDiffuseTexture(m_pStopTexture);

	m_pURLButton = AddUIButton(m_urlWidth, m_itemSide);
	m_pURLButton->GetSurface()->SetDiffuseTexture(m_pURLTexture);

	for (auto pButton : GetControlButtons()) {
		CN(pButton);
		CR(pButton->RegisterToInteractionEngine(m_pDreamOS));

		CR(pButton->RegisterEvent(UIEventType::UI_SELECT_BEGIN,
			std::bind(&UIControlBar::HandleTouchStart, this, std::placeholders::_1, std::placeholders::_2)));
	}

	{
		// set buttons positions based on spec
		point ptStart = point(-m_totalWidth / 2.0f, 0.0f, 0.0f);

		point ptBack = point(m_itemSide / 2.0f, 0.0f, 0.0f);
		m_pBackButton->SetPosition(ptStart + ptBack);

		point ptForward = ptBack + point(m_itemSide + m_itemSpacing, 0.0f, 0.0f);
		m_pForwardButton->SetPosition(ptStart + ptForward);

		// URL button is centered at 0

		point ptHide = point(m_urlWidth / 2.0f + m_itemSpacing + m_itemSide / 2.0f, 0.0f, 0.0f);
		m_pToggleButton->SetPosition(ptHide);

		point ptStop = ptHide + point(m_itemSide + m_itemSpacing, 0.0f, 0.0f);
		m_pStopButton->SetPosition(ptStop);
	}

Error:
	return r;
}

RESULT UIControlBar::HandleTouchStart(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	//TODO: very similar to the code in DreamUIBar::HandleTouchStart
	std::shared_ptr<quad> pSurface = nullptr;
	vector vSurface;
	vector vRotation;
	CNR(pButtonContext, R_SKIPPED);
	pSurface = pButtonContext->GetSurface();

	//vector for captured object movement
	quaternion qSurface = pButtonContext->GetOrientation() * (pSurface->GetOrientation());
	qSurface.Reverse();
	vSurface = qSurface.RotateVector(pSurface->GetNormal() * -1.0f);

	//vector for captured object collisions
	quaternion qRotation = pSurface->GetOrientation(true);
	qRotation.Reverse();
	vRotation = qRotation.RotateVector(pSurface->GetNormal() * -1.0f);

	auto pInteractionProxy = m_pDreamOS->GetInteractionEngineProxy();
	pInteractionProxy->ResetObjects(pButtonContext->GetInteractionObject());
	pInteractionProxy->ReleaseObjects(pButtonContext->GetInteractionObject());

	pInteractionProxy->CaptureObject(
		pButtonContext,
		pButtonContext->GetInteractionObject(), 
		pButtonContext->GetContactPoint(), 
		vRotation,
		vSurface,
		m_actuationDepth);
Error:
	return r;
}


float UIControlBar::GetSpacingOffset() {
	return -(m_itemSpacing + m_itemSide / 2.0f);
}

std::shared_ptr<UIButton> UIControlBar::GetBackButton() {
	return m_pBackButton;
}

std::shared_ptr<UIButton> UIControlBar::GetForwardButton() {
	return m_pForwardButton;
}

std::shared_ptr<UIButton> UIControlBar::GetToggleButton() {
	return m_pToggleButton;
}

std::shared_ptr<UIButton> UIControlBar::GetStopButton() {
	return m_pStopButton;
}

std::shared_ptr<UIButton> UIControlBar::GetURLButton() {
	return m_pURLButton;
}

std::vector<std::shared_ptr<UIButton>> UIControlBar::GetControlButtons() {
	return { m_pBackButton, m_pForwardButton, m_pToggleButton, m_pStopButton, m_pURLButton };
}

texture *UIControlBar::GetHideTexture() {
	return m_pHideTexture;
}

texture *UIControlBar::GetShowTexture() {
	return m_pShowTexture;
}