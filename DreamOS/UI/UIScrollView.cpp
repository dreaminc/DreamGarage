#include "UIScrollView.h"
#include "UIButton.h"

#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"
#include "Primitives/DimObj.h"
#include "Primitives/VirtualObj.h"

#include "DreamOS.h"

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

	m_pTitleView = AddUIView();

	m_pLeftScrollButton = AddUIButton();
	m_pLeftScrollButton->SetVisible(false);
	PositionMenuButton(-1, m_pLeftScrollButton);

	m_pRightScrollButton = AddUIButton();
	m_pRightScrollButton->SetVisible(false);
	PositionMenuButton(m_maxElements, m_pRightScrollButton);

	m_pMenuButtonsContainer = AddUIView();

	m_pLeftScrollButton->GetSurface()->SetColorTexture(m_pDreamOS->MakeTexture(L"chevron-left-600.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));
	m_pRightScrollButton->GetSurface()->SetColorTexture(m_pDreamOS->MakeTexture(L"chevron-right-600.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));

	for (auto& pButton : { m_pLeftScrollButton, m_pRightScrollButton }) {
		CN(pButton);
		CR(pButton->RegisterToInteractionEngine(m_pDreamOS));
		/*
		CR(pButton->RegisterEvent(UI_HOVER_BEGIN,
			std::bind(&UIScrollView::AnimateScaleUp, this, std::placeholders::_1)));
		CR(pButton->RegisterEvent(UI_HOVER_ENDED,
			std::bind(&UIScrollView::AnimateScaleReset, this, std::placeholders::_1)));
			//*/

		CR(pButton->RegisterEvent(UIEventType::UI_SELECT_ENDED,
			std::bind(&UIScrollView::StopScroll, this, std::placeholders::_1)));
	}

	CR(m_pLeftScrollButton->RegisterEvent(UIEventType::UI_SELECT_BEGIN,
		std::bind(&UIScrollView::StartScrollLeft, this, std::placeholders::_1)));

	CR(m_pRightScrollButton->RegisterEvent(UIEventType::UI_SELECT_BEGIN,
		std::bind(&UIScrollView::StartScrollRight, this, std::placeholders::_1)));

Error:
	return r;
}

RESULT UIScrollView::Update() {
	RESULT r = R_PASS;
	//TODO:
	//- rotate by velocity
	//- update visible scrollbars
	std::vector<std::shared_ptr<VirtualObj>> pChildren;
	CBR(m_pMenuButtonsContainer->HasChildren(), R_PASS);

	float yRotationPerElement =  (float)M_PI / (180.0f / m_itemAngleY);

	pChildren = m_pMenuButtonsContainer->GetChildren();
	if (pChildren.size() > m_maxElements) {
		float maxRotation = (pChildren.size() - m_maxElements) * yRotationPerElement;

		m_yRotation = std::max(0.0f, std::min(m_yRotation + m_velocity, maxRotation));

		// update visible items / index
		m_pLeftScrollButton->SetVisible(m_yRotation > 0.0f);
		m_pRightScrollButton->SetVisible(m_yRotation < maxRotation);

		m_pMenuButtonsContainer->SetOrientation(quaternion::MakeQuaternionWithEuler(0.0f, m_yRotation, 0.0f));

		float diff = m_yRotation - (m_objectIndex * yRotationPerElement);
		if (diff >= yRotationPerElement && m_objectIndex + m_maxElements < pChildren.size()) {

			auto pButton = dynamic_cast<UIButton*>(pChildren[m_objectIndex].get());
			pButton->SetVisible(false);

			pButton = dynamic_cast<UIButton*>(pChildren[m_objectIndex + m_maxElements].get());
			pButton->SetVisible(true);

			m_objectIndex += 1;
		}
		else if (diff <= -m_yRotationPerElement && m_objectIndex > 0) {
			auto pButton = dynamic_cast<UIButton*>(pChildren[m_objectIndex + m_maxElements - 1].get());
			pButton->SetVisible(false);

			pButton = dynamic_cast<UIButton*>(pChildren[m_objectIndex - 1].get());
			pButton->SetVisible(true);

			m_objectIndex -= 1;
		}
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

	return r;
}

RESULT UIScrollView::UpdateMenuButtons(std::vector<std::shared_ptr<UIButton>> pButtons) {
	RESULT r = R_PASS;

	CN(m_pDreamOS);

	m_objectIndex = 0;
	m_yRotation = 0.0f;
	m_velocity = 0.0f;
	//m_pMenuButtonsContainer->SetOrientation(quaternion::MakeQuaternionWithEuler(0.0f, m_yRotation, 0.0f));
	m_pMenuButtonsContainer->ResetRotation();

	if (m_pMenuButtonsContainer->HasChildren()) {
		for (auto& pButton : m_pMenuButtonsContainer->GetChildren()) {
			CR(m_pDreamOS->RemoveObject(pButton.get()));
			CR(m_pDreamOS->UnregisterInteractionObject(pButton.get()));
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

	if (m_pMenuButtonsContainer->GetChildren().size() > m_maxElements) {
		m_pRightScrollButton->SetVisible(true);
	}
	else {
		m_pRightScrollButton->SetVisible(false);
	}
	m_pLeftScrollButton->SetVisible(false);

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
	m_menuState = MenuState::SCROLLING;
	m_velocity = 0.01f;
	return R_PASS;
}

RESULT UIScrollView::StartScrollLeft(void *pContext) {
	m_menuState = MenuState::SCROLLING;
	m_velocity = -0.01f;
	return R_PASS;
}

RESULT UIScrollView::StopScroll(void *pContext) {
	m_menuState = MenuState::NONE;
	m_velocity = 0.0f;
	return R_PASS;
}

MenuState UIScrollView::GetState() {
	return m_menuState;
}

std::shared_ptr<UIView> UIScrollView::GetTitleView() {
	return m_pTitleView;
}

std::shared_ptr<UIView> UIScrollView::GetMenuItemsView() {
	return m_pMenuButtonsContainer;
}
