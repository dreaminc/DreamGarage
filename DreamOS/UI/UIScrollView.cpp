#include "UIScrollView.h"
#include "UIButton.h"

#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"
#include "Primitives/DimObj.h"
#include "Primitives/VirtualObj.h"

#include "DreamOS.h"
#include "Primitives/font.h"

#include "DreamConsole/DreamConsole.h"

#include <chrono>

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
	float radY = (m_itemAngleY * M_PI / 180.0f) * -2.0f;

	point ptContext = point(-sin(radY) * m_menuCenterOffset, m_itemHeight, 0.0f);
	m_pTitleView->SetPosition(ptContext);

	//TODO:  clean this up, potentially with curved quad implementation
	m_pTitleQuad = m_pTitleView->AddQuad(0.068f, 0.068f * (3.0f / 4.0f));
	m_pTitleQuad->SetColorTexture(m_pDreamOS->MakeTexture(L"icon-share.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));
	m_pTitleQuad->RotateXByDeg(90.0f);
	m_pTitleQuad->SetPosition(point(0.034f, m_titleHeight, 0.0f));
	auto pFont = m_pDreamOS->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
	pFont->SetLineHeight(0.055f);
	m_pTitleText = std::shared_ptr<text>(m_pDreamOS->MakeText(
		pFont,
		"Share",
		1.0,
		0.055,
		text::flags::TRAIL_ELLIPSIS | text::flags::RENDER_QUAD));

	m_pTitleText->RotateXByDeg(90.0f);
	m_pTitleText->SetPosition(point(0.6f, m_titleHeight - 0.005f, 0.0f));

	m_pTitleView->AddObject(m_pTitleText);

	m_pMenuButtonsContainer = AddUIView();
	m_pMenuButtonsContainer->SetPosition(0.0f, 0.0f, -m_menuCenterOffset);
	//m_pMenuButtonsContainer->SetPosition(0.0f, 0.0f, 0.3f);

	m_pLeftScrollButton = AddUIButton();
	m_pLeftScrollButton->SetVisible(false);

	//TODO: may be worth adding a constructor that exposes surface width / height
	//point ptRightScrollButtonOffset = point(0.0f, -0.05f * cos(m_itemAngleX), 0.05f * sin(m_itemAngleX));
	point ptLeftScrollButtonOffset = point(0.0f, -0.05 * sin(m_itemAngleX), 0.0f);
	point ptRightScrollButtonOffset = point(0.0f, 0.0f, 0.05f * sin(m_itemAngleX));

	m_pLeftScrollButton->GetSurface()->SetScale(vector(m_scrollScale * SCROLL_ASPECT_RATIO, m_scrollScale * 16.0f / 9.0f, m_scrollScale));
	PositionMenuButton(-1.0f + m_scrollBias, m_pLeftScrollButton);
	m_pLeftScrollButton->SetPosition(m_pLeftScrollButton->GetPosition() + m_pMenuButtonsContainer->GetPosition() + ptLeftScrollButtonOffset);

	m_pRightScrollButton = AddUIButton();
	m_pRightScrollButton->SetVisible(false);
	m_pRightScrollButton->GetSurface()->SetScale(vector(m_scrollScale * SCROLL_ASPECT_RATIO, m_scrollScale * 16.0f / 9.0f, m_scrollScale));
	PositionMenuButton(m_maxElements - m_scrollBias, m_pRightScrollButton);
	m_pRightScrollButton->SetPosition(m_pRightScrollButton->GetPosition() + m_pMenuButtonsContainer->GetPosition() + ptRightScrollButtonOffset);

	m_pLeftScrollButton->GetMaterial()->SetColors(m_hiddenColor, m_hiddenColor, m_hiddenColor);
	m_pRightScrollButton->GetMaterial()->SetColors(m_hiddenColor, m_hiddenColor, m_hiddenColor);

	m_pLeftScrollButton->GetSurface()->SetColorTexture(m_pDreamOS->MakeTexture(L"chevron-left.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));
	m_pRightScrollButton->GetSurface()->SetColorTexture(m_pDreamOS->MakeTexture(L"chevron-right.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));

	m_pDreamOS->RegisterSubscriber(SenseControllerEventType::SENSE_CONTROLLER_PAD_MOVE, this);

	for (auto& pButton : { m_pLeftScrollButton, m_pRightScrollButton }) {
		CN(pButton);
		CR(pButton->RegisterToInteractionEngine(m_pDreamOS));
	}

	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	m_frameMs = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
	m_frameMs /= 1000.0;

Error:
	return r;
}

RESULT UIScrollView::Update() {
	RESULT r = R_PASS;

	std::vector<std::shared_ptr<VirtualObj>> pChildren;
	CBR(m_pMenuButtonsContainer->HasChildren(), R_PASS);

	float yRotationPerElement = (float)M_PI / (180.0f / m_itemAngleY);

	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
	msNow /= 1000.0;

	double tDiff = (msNow - m_frameMs) * (90.0);

	pChildren = m_pMenuButtonsContainer->GetChildren();
	if (pChildren.size() > m_maxElements && m_fScrollButtonVisible) {
		float maxRotation = (pChildren.size() - m_maxElements) * yRotationPerElement;

		m_yRotation = std::max(0.0f, std::min(m_yRotation + (m_velocity*(float)(tDiff)) , maxRotation));

		if (!m_pDreamOS->GetInteractionEngineProxy()->IsAnimating(m_pLeftScrollButton.get())) {
			color leftColor = m_pLeftScrollButton->GetMaterial()->GetDiffuseColor();
			if (m_yRotation > 0.0f && m_velocity < 0.0f && leftColor != m_visibleColor ) {
				ShowObject(m_pLeftScrollButton.get(), m_visibleColor);
			}
			else if (m_yRotation > 0.0f && m_velocity >= 0.0f && leftColor != m_canScrollColor) {
				ShowObject(m_pLeftScrollButton.get(), m_canScrollColor);
			}
			else if (m_yRotation <= 0.0f && leftColor != m_hiddenColor) {
				HideObject(m_pLeftScrollButton.get());
			}

		}

		if (!m_pDreamOS->GetInteractionEngineProxy()->IsAnimating(m_pRightScrollButton.get())) {
			color rightColor = m_pRightScrollButton->GetMaterial()->GetDiffuseColor();
			if (m_yRotation < maxRotation && m_velocity > 0.0f && rightColor != m_visibleColor) {
				ShowObject(m_pRightScrollButton.get(), m_visibleColor);
			}
			else if (m_yRotation < maxRotation && m_velocity <= 0.0f && rightColor != m_canScrollColor) {
				ShowObject(m_pRightScrollButton.get(), m_canScrollColor);
			}
			else if (m_yRotation >= maxRotation && rightColor != m_hiddenColor) {
				HideObject(m_pRightScrollButton.get());
			}
		}

		m_pMenuButtonsContainer->SetOrientation(quaternion::MakeQuaternionWithEuler(0.0f, m_yRotation, 0.0f));
		//m_pMenuButtonsContainer->SetPosition(point(0.0f, 0.0f, 0.3f));

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
	m_frameMs = msNow;

Error:
	return r; 
}

// mostly borrowed from UIBar right now, current default values make sense with ray selection
RESULT UIScrollView::PositionMenuButton(float index, std::shared_ptr<UIButton> pButton) {
	RESULT r = R_PASS;

	float radY = (m_itemAngleY * M_PI / 180.0f) * -index;
	radY -= (m_itemStartAngleY * M_PI / 180.0f);

	float yPos = m_itemHeight;
	float zPos = m_menuCenterOffset;
	point ptContext = point(sin(radY) * zPos, yPos, (cos(radY) * zPos));

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

Error:
	return r;
}

RESULT UIScrollView::HideAllButtons(UIButton* pPushButton) {
	RESULT r = R_PASS;

	m_fScrollButtonVisible = false;

	for (auto& pButton : m_pMenuButtonsContainer->GetChildren()) {
		auto pObj = reinterpret_cast<UIButton*>(pButton.get());
		//if (pObj != pPushButton) {
			CR(HideObject(pObj));
		//}
		//else {
		//	CR(HideAndPushButton(pObj));
		//}
	}
	for (auto& pButton : { m_pLeftScrollButton, m_pRightScrollButton }) {
		CR(HideObject(pButton.get()));
	}
	HideObject(m_pTitleText.get());
	HideObject(m_pTitleQuad.get());

Error:
	return r;
}

RESULT UIScrollView::ShowTitle() {
	RESULT r = R_PASS;

	m_pTitleQuad->SetVisible(true);
	m_pTitleQuad->SetMaterialColors(m_visibleColor, true);
	m_pTitleText->SetVisible(true);
	m_pTitleText->SetMaterialColors(m_visibleColor, true);

	return r;
}

RESULT UIScrollView::HideObject(DimObj* pObject) {
	RESULT r = R_PASS;

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		DimObj *pObj = reinterpret_cast<DimObj*>(pContext);
		pObj->SetVisible(false);
		return R_PASS;
	};

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		pObject,
		m_hiddenColor,
		m_fadeDuration,
		AnimationCurveType::LINEAR,
		AnimationFlags(),
		nullptr,
		fnEndCallback,
		pObject));

Error:
	return r;
}

RESULT UIScrollView::ShowObject(DimObj* pObject, color showColor /* = color(1.0f, 1.0f, 1.0f, 1.0f) */) {
	RESULT r = R_PASS;

	auto fnStartCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		DimObj *pObj = reinterpret_cast<DimObj*>(pContext);
		pObj->SetVisible(true);
		return R_PASS;
	};

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		pObject,
		showColor,
		m_fadeDuration,
		AnimationCurveType::LINEAR,
		AnimationFlags(),
		fnStartCallback,
		nullptr,
		pObject));

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
		pSurface->GetPosition() + point(0.0f, 0.0f, m_pushDepth),
		pSurface->GetOrientation(),
		pSurface->GetScale(),
		m_hiddenColor,
		m_fadeDuration,
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
