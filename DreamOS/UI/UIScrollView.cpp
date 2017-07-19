#include "UIScrollView.h"
#include "UIButton.h"

#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"
#include "Primitives/DimObj.h"
#include "Primitives/VirtualObj.h"

#include "DreamOS.h"
#include "Primitives/font.h"

#include "DreamConsole/DreamConsole.h"

UIScrollView::UIScrollView(HALImp *pHALImp, DreamOS *pDreamOS) :
UIView(pHALImp, pDreamOS)
{
	RESULT r = R_PASS;

	CR(Initialize());

	Validate();
	return;
Error:
	Invalidate();
	return;
}

UIScrollView::~UIScrollView() 
{

}

RESULT UIScrollView::Initialize() {
	RESULT r = R_PASS;


	color scrollColor = color(1.0f, 1.0f, 1.0f, m_canScrollAlpha);

	m_pLeftScrollButton = AddUIButton();
	m_pLeftScrollButton->SetVisible(false);
	m_pLeftScrollButton->GetSurface()->SetScale(vector(m_scrollScale));
	m_pLeftScrollButton->GetMaterial()->SetColors(scrollColor, scrollColor, scrollColor);
	PositionMenuButton(-1, m_pLeftScrollButton);

	m_pRightScrollButton = AddUIButton();
	m_pRightScrollButton->SetVisible(false);
	m_pRightScrollButton->GetSurface()->SetScale(vector(m_scrollScale));
	m_pRightScrollButton->GetMaterial()->SetColors(scrollColor, scrollColor, scrollColor);
	PositionMenuButton(m_maxElements, m_pRightScrollButton);

	//color cTransparent = color(1.0f, 1.0f, 1.0f, 0.0f);
	//m_pLeftScrollButton->GetMaterial()->SetColors(cTransparent, cTransparent, cTransparent);
	//m_pRightScrollButton->GetMaterial()->SetColors(cTransparent, cTransparent, cTransparent);

	//m_pTitleView = AddUIView();
	m_pTitleQuad = AddQuad(0.068f, 0.068f * (3.0f / 4.0f));
	m_pTitleQuad->SetColorTexture(m_pDreamOS->MakeTexture(L"icon-share.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));
	m_pTitleQuad->RotateXByDeg(90.0f);
	m_pTitleQuad->SetPosition(point(-0.485f, m_titleHeight, m_menuDepth));
	auto pFont = m_pDreamOS->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
	pFont->SetLineHeight(0.055f);
	m_pTitleText = std::shared_ptr<text>(m_pDreamOS->MakeText(
		pFont,
		"Share",
		1.0,
		0.055,
		text::flags::TRAIL_ELLIPSIS | text::flags::RENDER_QUAD));

	m_pTitleText->RotateXByDeg(90.0f);
	m_pTitleText->SetPosition(point(0.085f, m_titleHeight - 0.005f, m_menuDepth));
	/*
	m_pTitleText->SetBackgroundColor(COLOR_BLACK);
	m_pTitleText->RenderToQuad();
	//*/

	AddObject(m_pTitleText);
	//m_pTitleView->SetPosition(point(0.0f, m_titleHeight, m_menuDepth));

	m_pMenuButtonsContainer = AddUIView();

	m_pLeftScrollButton->GetSurface()->SetColorTexture(m_pDreamOS->MakeTexture(L"chevron-left-600.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));
	m_pRightScrollButton->GetSurface()->SetColorTexture(m_pDreamOS->MakeTexture(L"chevron-right-600.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));

	m_pDreamOS->RegisterSubscriber(SenseControllerEventType::SENSE_CONTROLLER_PAD_MOVE, this);

	for (auto& pButton : { m_pLeftScrollButton, m_pRightScrollButton }) {
		CN(pButton);
		CR(pButton->RegisterToInteractionEngine(m_pDreamOS));
	}

Error:
	return r;
}

RESULT UIScrollView::Update() {
	RESULT r = R_PASS;

	std::vector<std::shared_ptr<VirtualObj>> pChildren;
	CBR(m_pMenuButtonsContainer->HasChildren(), R_PASS);

	float yRotationPerElement =  (float)M_PI / (180.0f / m_itemAngleY);

	pChildren = m_pMenuButtonsContainer->GetChildren();
	if (pChildren.size() > m_maxElements && m_fScrollButtonVisible) {
		float maxRotation = (pChildren.size() - m_maxElements) * yRotationPerElement;

		m_yRotation = std::max(0.0f, std::min(m_yRotation + m_velocity, maxRotation));

		if (IsVisible()) {
			// update visible items / index
			if (m_yRotation > 0.0f) {// && !m_pLeftScrollButton->IsVisible()) {
				//ShowButton(m_pLeftScrollButton.get());
				m_pLeftScrollButton->SetVisible(true);
			}
			else if (m_yRotation <= 0.0f) {//  && m_pLeftScrollButton->IsVisible()) {
				//HideButton(m_pLeftScrollButton.get());
				m_pLeftScrollButton->SetVisible(false);
			}
			if (m_yRotation < maxRotation) {//  && !m_pRightScrollButton->IsVisible()) {
				//ShowButton(m_pRightScrollButton.get());
				m_pRightScrollButton->SetVisible(true);
			}
			else if (m_yRotation >= maxRotation) {//  && m_pRightScrollButton->IsVisible()) {
				//HideButton(m_pRightScrollButton.get());
				m_pRightScrollButton->SetVisible(false);
			}
		}

		m_pMenuButtonsContainer->SetOrientation(quaternion::MakeQuaternionWithEuler(0.0f, m_yRotation, 0.0f));

		//TODO: approach will need a less naive implementation if animations are used
		int highIndex = (int)(m_yRotation / yRotationPerElement) + m_maxElements;
		int lowIndex = std::ceil(m_yRotation / yRotationPerElement);

		for (int i = 0; i < pChildren.size(); i++) {
			auto pButton = dynamic_cast<UIButton*>(pChildren[i].get());
			if (i >= lowIndex && i < highIndex) {
				pButton->SetVisible(true);
			}
			else {
				pButton->SetVisible(false);
			}
		}
	}
	else {
		m_pLeftScrollButton->SetVisible(false);
		m_pRightScrollButton->SetVisible(false);
	}
Error:
	return r; 
}

// mostly borrowed from UIBar right now, current default values make sense with ray selection
RESULT UIScrollView::PositionMenuButton(int index, std::shared_ptr<UIButton> pButton) {
	RESULT r = R_PASS;

	float radY = (m_itemAngleY * M_PI / 180.0f) * -index;
	radY -= (m_itemStartAngleY * M_PI / 180.0f);

	float yPos = m_itemHeight;
	float zPos = m_menuDepth;
	point ptContext = point(sin(radY) * zPos, yPos, cos(radY) * zPos);

	quaternion qContext = quaternion::MakeQuaternionWithEuler(0.0f, radY, 0.0f);

	pButton->SetPosition(ptContext);
	pButton->SetOrientation(qContext);

	float radX = m_itemAngleX * M_PI / 180.0f;
	quaternion qQuad = quaternion::MakeQuaternionWithEuler(radX, 0.0f, 0.0f);

	pButton->GetSurface()->SetOrientation(qQuad);
	pButton->GetSurfaceComposite()->SetOrientation(qQuad);

	return r;
}

RESULT UIScrollView::UpdateMenuButtons(std::vector<std::shared_ptr<UIButton>> pButtons) {
	RESULT r = R_PASS;

	CN(m_pDreamOS);

	m_yRotation = 0.0f;
	m_velocity = 0.0f;
	//m_pMenuButtonsContainer->SetOrientation(quaternion::MakeQuaternionWithEuler(0.0f, m_yRotation, 0.0f));
	m_pMenuButtonsContainer->ResetRotation();

	if (m_pMenuButtonsContainer->HasChildren()) {
		for (auto& pObj : m_pMenuButtonsContainer->GetChildren()) {
			UIButton* pButton = reinterpret_cast<UIButton*>(pObj.get());

			//TODO: this works for now, but it may be necessary to have some of the individual
			// RemoveObject functions properly cascade the call for future situations
			CR(m_pDreamOS->RemoveObject(pButton->GetSurface().get()));
			CR(m_pDreamOS->RemoveObject(pButton));
			CR(m_pDreamOS->UnregisterInteractionObject(pButton));
		}
	}
	CR(m_pMenuButtonsContainer->ClearChildren());

	int i = 0;
	for (auto& pButton : pButtons) {

		CN(pButton);
		CR(pButton->RegisterToInteractionEngine(m_pDreamOS));

		PositionMenuButton(i, pButton);
		m_pMenuButtonsContainer->AddObject(pButton);

		if (i > m_maxElements-1) {
			pButton->SetVisible(false);
		}

		i++;
	}
	
	/*
	if (m_pMenuButtonsContainer->GetChildren().size() > m_maxElements) {
		m_pRightScrollButton->SetVisible(true);
	//	ShowButton(m_pRightScrollButton.get());
	}
	else if (m_pMenuButtonsContainer->GetChildren().size() <= m_maxElements) {
		m_pRightScrollButton->SetVisible(false);
	//	HideButton(m_pRightScrollButton.get());
	}
	m_pLeftScrollButton->SetVisible(false);
	/*
	if (m_pLeftScrollButton->IsVisible()) {
		HideButton(m_pLeftScrollButton.get());
	}
	//*/

Error:
	return r;
}

RESULT UIScrollView::AnimateScaleUp(void *pContext) {
	RESULT r = R_PASS;

	UIButton *button = reinterpret_cast<UIButton*>(pContext);
	DimObj *pObj = button->GetSurface().get();

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		pObj,
		pObj->GetPosition(),
		pObj->GetOrientation(),
		vector(1.25f, 1.25, 1.25f),
		0.1,
		AnimationCurveType::LINEAR,
		AnimationFlags()
	));

Error:
	return r;
}

RESULT UIScrollView::AnimateScaleReset(void *pContext) {
	RESULT r = R_PASS;

	UIButton *button = reinterpret_cast<UIButton*>(pContext);
	DimObj *pObj = button->GetSurface().get();

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		pObj,
		pObj->GetPosition(),
		pObj->GetOrientation(),
		vector(1.0f, 1.0, 1.0f),
		0.1,
		AnimationCurveType::LINEAR,
		AnimationFlags()
	));

Error:
	return r;
}

RESULT UIScrollView::StartScrollRight(void *pContext) {
	m_menuState = ScrollState::SCROLLING;
	m_velocity = 0.01f;
	return R_PASS;
}

RESULT UIScrollView::StartScrollLeft(void *pContext) {
	m_menuState = ScrollState::SCROLLING;
	m_velocity = -0.01f;
	return R_PASS;
}

RESULT UIScrollView::StopScroll(void *pContext) {
	m_menuState = ScrollState::NONE;
	m_velocity = 0.0f;
	return R_PASS;
}

RESULT UIScrollView::HideAllButtons(UIButton* pPushButton) {
	RESULT r = R_PASS;

	m_fScrollButtonVisible = false;

	for (auto& pButton : m_pMenuButtonsContainer->GetChildren()) {
		auto pObj = reinterpret_cast<UIButton*>(pButton.get());
		if (pObj != pPushButton) {
			CR(HideButton(pObj));
		}
		else {
			CR(HideAndPushButton(pObj));
		}
	}
	m_pLeftScrollButton->SetVisible(false);
	m_pRightScrollButton->SetVisible(false);
	m_pTitleQuad->SetVisible(false);
	m_pTitleText->SetVisible(false);
	//m_pTitleView->SetVisible(false);
	//CR(HideButton(m_pLeftScrollButton.get()));
	//CR(HideButton(m_pRightScrollButton.get()));

Error:
	return r;
}

RESULT UIScrollView::HideButton(UIButton* pScrollButton) {
	RESULT r = R_PASS;

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		DimObj *pObj = reinterpret_cast<DimObj*>(pContext);
		pObj->SetVisible(false);
		return R_PASS;
	};

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		//pScrollButton->GetSurface().get(),
		pScrollButton,
		color(1.0f, 1.0f, 1.0f, 0.0f),
		0.1f,
		AnimationCurveType::LINEAR,
		AnimationFlags(),
		nullptr,
		fnEndCallback,
		pScrollButton));

Error:
	return r;
}

RESULT UIScrollView::ShowButton(UIButton* pScrollButton) {
	RESULT r = R_PASS;

	auto fnStartCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		DimObj *pObj = reinterpret_cast<DimObj*>(pContext);
		pObj->SetVisible(true);
		return R_PASS;
	};

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		pScrollButton->GetSurface().get(),
		color(1.0f, 1.0f, 1.0f, 1.0f),
		0.1f,
		AnimationCurveType::LINEAR,
		AnimationFlags(),
		fnStartCallback,
		nullptr,
		pScrollButton));

Error:
	return r;
}

RESULT UIScrollView::HideAndPushButton(UIButton* pButton) {
	RESULT r = R_PASS;

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		DimObj *pObj = reinterpret_cast<DimObj*>(pContext);
		pObj->SetVisible(false);
		return R_PASS;
	};

	auto pSurface = pButton;
	//auto pSurface = pButton->GetSurface().get();

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		pSurface,
		pSurface->GetPosition() + point(0.0f, 0.0f, -0.1f),
		pSurface->GetOrientation(),
		pSurface->GetScale(),
		color(1.0f, 1.0f, 1.0f, 0.0f),
		0.1f,
		AnimationCurveType::LINEAR,
		AnimationFlags(),
		nullptr,
		fnEndCallback,
		pButton));

Error:
	return r;
}

ScrollState UIScrollView::GetState() {
	return m_menuState;
}

RESULT UIScrollView::SetScrollVisible(bool fVisible) {
	m_fScrollButtonVisible = fVisible;
	return R_PASS;
}

std::shared_ptr<UIView> UIScrollView::GetTitleView() {
	return m_pTitleView;
}

std::shared_ptr<quad> UIScrollView::GetTitleQuad() {
	return m_pTitleQuad;
}

std::shared_ptr<text> UIScrollView::GetTitleText() {
	return m_pTitleText;
}

std::shared_ptr<UIView> UIScrollView::GetMenuItemsView() {
	return m_pMenuButtonsContainer;
}

RESULT UIScrollView::Notify(SenseControllerEvent *pEvent) {
	switch (pEvent->type) {
	case SenseControllerEventType::SENSE_CONTROLLER_PAD_MOVE: {
		m_velocity = pEvent->state.ptTouchpad.x() * PAD_MOVE_CONSTANT;
		OVERLAY_DEBUG_SET("velocity", m_velocity);
	} break;

	}
	return R_PASS;
}
