#include "UISpatialScrollView.h"
#include "UIButton.h"
#include "UIMenuItem.h"

#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"
#include "Primitives/DimObj.h"
#include "Primitives/VirtualObj.h"

#include "DreamOS.h"
#include "Primitives/font.h"

#include <chrono>

UISpatialScrollView::UISpatialScrollView(HALImp *pHALImp, DreamOS *pDreamOS) :
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

UISpatialScrollView::~UISpatialScrollView() 
{

}

RESULT UISpatialScrollView::Initialize() {
	RESULT r = R_PASS;

//	m_pDreamOS->AddObjectToUIGraph(this);
//	m_pDreamOS->AddObjectToUIClippingGraph(this);


	m_pMenuButtonsContainer = AddUIView();
	m_pMenuButtonsContainer->SetPosition(0.0f, 0.0f, -m_menuCenterOffset);
	//m_pDreamOS->AddObjectToUIClippingGraph(m_pMenuButtonsContainer.get());
	//m_pMenuButtonsContainer->SetPosition(0.0f, 0.0f, 0.3f);

	m_pLeftScrollButton = AddUIButton();
	m_pDreamOS->AddObjectToUIGraph(m_pLeftScrollButton.get());
	m_pLeftScrollButton->SetVisible(false);
	//TODO: may be worth adding a constructor that exposes surface width / height
	//point ptRightScrollButtonOffset = point(0.0f, -0.05f * cos(m_itemAngleX), 0.05f * sin(m_itemAngleX));

	m_pLeftScrollButton->GetSurface()->SetScale(vector(m_scrollScale * SCROLL_ASPECT_RATIO, m_scrollScale * 16.0f / 9.0f, m_scrollScale));

	m_pRightScrollButton = AddUIButton();
	m_pDreamOS->AddObjectToUIGraph(m_pRightScrollButton.get());
	m_pRightScrollButton->SetVisible(false);
	m_pRightScrollButton->GetSurface()->SetScale(vector(m_scrollScale * SCROLL_ASPECT_RATIO, m_scrollScale * 16.0f / 9.0f, m_scrollScale));

	m_pLeftScrollButton->GetMaterial()->SetColors(m_hiddenColor, m_hiddenColor, m_hiddenColor);
	m_pRightScrollButton->GetMaterial()->SetColors(m_hiddenColor, m_hiddenColor, m_hiddenColor);

	m_pLeftScrollButton->GetSurface()->SetDiffuseTexture(m_pDreamOS->MakeTexture(L"chevron-left.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
	m_pRightScrollButton->GetSurface()->SetDiffuseTexture(m_pDreamOS->MakeTexture(L"chevron-right.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));

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

RESULT UISpatialScrollView::Update() {
	RESULT r = R_PASS;

	std::vector<std::shared_ptr<VirtualObj>> pChildren;
	CBR(m_pMenuButtonsContainer->HasChildren(), R_PASS);

	float yRotationPerElement = (float)M_PI / (180.0f / m_itemAngleY);

	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
	msNow /= 1000.0;

	double tDiff = (msNow - m_frameMs) * (90.0);

	//m_yRotation = m_yRotation + (m_velocity*(float)(tDiff));
	pChildren = m_pMenuButtonsContainer->GetChildren();
	if (pChildren.size() > m_maxElements && m_fScrollButtonVisible) {
		float maxRotation = (pChildren.size() - m_maxElements) * yRotationPerElement;

		if (!m_pDreamOS->GetInteractionEngineProxy()->IsAnimating(m_pMenuButtonsContainer.get())) {
			m_yRotation = std::max(0.0f, std::min(m_yRotation + (m_velocity*(float)(tDiff)), maxRotation));
			m_pMenuButtonsContainer->SetOrientation(quaternion::MakeQuaternionWithEuler(0.0f, m_yRotation, 0.0f));
		}

		if (!m_pDreamOS->GetInteractionEngineProxy()->IsAnimating(m_pLeftScrollButton.get())) {
			color leftColor = m_pLeftScrollButton->GetMaterial()->GetDiffuseColor();
			if (m_yRotation > 0.0f && m_velocity < 0.0f && leftColor != m_visibleColor) {
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

	}


	int index = m_yRotation / yRotationPerElement;
	int arrayMaxIndex = (int)(pChildren.size()) - 1;

	int minIndex = index - 1;
	if (minIndex < 0) {
		minIndex = 0;
	}

	int maxIndex = index + m_maxElements + 1;
	if (maxIndex > arrayMaxIndex) {
		maxIndex = arrayMaxIndex;
	}

	for (int i = minIndex; i <= maxIndex; i++) {
		auto pObj = dynamic_cast<UIButton*>(pChildren[i].get());
		if (!pObj->IsVisible()) {
			pObj->SetVisible(true);
			pObj->RegisterToInteractionEngine(m_pDreamOS);
		}
	}

	// Hide items that are far enough from the view
	if (minIndex - 1 >= 0) {
		auto pObj = dynamic_cast<DimObj*>(pChildren[minIndex - 1].get());
		pObj->SetVisible(false);
	}
	if (maxIndex + 1 <= arrayMaxIndex) {
		auto pObj = dynamic_cast<DimObj*>(pChildren[maxIndex + 1].get());
		pObj->SetVisible(false);
	}


	m_frameMs = msNow;

	while (!m_pendingObjectRemovalQueue.empty()) {
		auto pButton = m_pendingObjectRemovalQueue.front();
		m_pendingObjectRemovalQueue.pop();

		m_pDreamOS->UnregisterInteractionObject(pButton);
		m_pDreamOS->RemoveObjectFromInteractionGraph(pButton);

		m_pDreamOS->RemoveObjectFromUIClippingGraph(pButton);

		if (m_pDreamOS->GetInteractionEngineProxy()->IsAnimating(pButton)) {
			m_pDreamOS->GetInteractionEngineProxy()->RemoveAnimationObject(pButton);
		}

		m_pMenuButtonsContainer->RemoveChild(pButton);

	}

Error:
	return r; 
}

RESULT UISpatialScrollView::InitializeWithWidth(float totalWidth) {
	RESULT r = R_PASS;

	m_initialWidth = totalWidth;

	// treat width as a chord of a circle with radius m_menuCenterOffset;
	float halfChord = totalWidth/2.0f;

	// calculate available space based on chord length and depth
	float theta = 2.0f*asin(halfChord / abs(m_menuCenterOffset));

	m_clippingRate = (theta / m_maxElements) * (1.0f - m_itemScale);
	// scale to add margins in between items
	theta += m_clippingRate;
	m_clippingThreshold = cos(theta / 2.0f);


	// calculate angle between each element
	float itemAngleYRad = theta / m_maxElements;

	// convert to degrees
	m_itemAngleY = itemAngleYRad * 180.0f / (float)(M_PI);

	// update starting angle
	m_itemStartAngleY = -(m_maxElements / 2.0f - 0.5f) * m_itemAngleY;

	// calculate new chord length to get width for future menu items
	m_itemWidth = abs(m_menuCenterOffset) * 2.0f * sin(itemAngleYRad / 2.0f) * m_itemScale;

	m_pTitleView = AddUIView();
	float radY = (m_itemAngleY * M_PI / 180.0f) * -2.0f;

	point ptContext = point(-sin(radY) * m_menuCenterOffset, m_itemHeight, 0.0f);
	m_pTitleView->SetPosition(ptContext);

	float titleWidth = m_titleWidth * totalWidth;
	m_pTitleQuad = m_pTitleView->AddQuad(titleWidth, titleWidth * m_titleAspectRatio);
	m_pTitleQuad->SetDiffuseTexture(m_pDreamOS->MakeTexture(L"icon-share.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
	m_pTitleQuad->RotateXByDeg(90.0f);
	m_pTitleQuad->SetPosition(point(titleWidth / 2.0f, totalWidth * m_titleHeight, 0.0f));
	m_pTitleQuad->SetVisible(false);

	auto pFont = m_pDreamOS->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
	float titleLineHeight = m_titleLineHeight * totalWidth;
	pFont->SetLineHeight(titleLineHeight);
	m_pTitleText = std::shared_ptr<text>(m_pDreamOS->MakeText(
		pFont,
		"Share",
		totalWidth,
		titleLineHeight,
		text::flags::TRAIL_ELLIPSIS | text::flags::RENDER_QUAD));

	m_pTitleText->RotateXByDeg(90.0f);
	m_pTitleText->SetPosition(point(totalWidth * m_titleOffsetX, totalWidth * (m_titleHeight - m_titleOffsetY), 0.0f));
	m_pTitleText->SetVisible(false);

	m_pTitleView->AddObject(m_pTitleText);
	m_pDreamOS->AddObjectToUIGraph(m_pTitleView.get());


	PositionMenuButton(-1.0f + m_scrollBias, m_pLeftScrollButton);
	m_pLeftScrollButton->SetPosition(m_pLeftScrollButton->GetPosition() + m_pMenuButtonsContainer->GetPosition());

	PositionMenuButton(m_maxElements - m_scrollBias, m_pRightScrollButton);
	m_pRightScrollButton->SetPosition(m_pRightScrollButton->GetPosition() + m_pMenuButtonsContainer->GetPosition());

	return r;
}

RESULT UISpatialScrollView::UpdateWithWidth(float totalWidth) {
	RESULT r = R_PASS;

	CBR(m_initialWidth != 0.0f, R_SKIPPED);
	m_initialWidth = totalWidth;
	float objectScale = totalWidth / m_initialWidth;

	// treat width as a chord of a circle with radius m_menuCenterOffset;
	float halfChord = totalWidth/2.0f;

	// calculate available space based on chord length and depth
	float theta = 2.0f*asin(halfChord / abs(m_menuCenterOffset));

	m_clippingRate = (theta / m_maxElements) * (1.0f - m_itemScale);
	// scale to add margins in between items
	theta += m_clippingRate;
	m_clippingThreshold = cos(theta / 2.0f);


	// calculate angle between each element
	float itemAngleYRad = theta / m_maxElements;

	// convert to degrees
	m_itemAngleY = itemAngleYRad * 180.0f / (float)(M_PI);

	// update starting angle
	m_itemStartAngleY = -(m_maxElements / 2.0f - 0.5f) * m_itemAngleY;

	// calculate new chord length to get width for future menu items
	m_itemWidth = abs(m_menuCenterOffset) * 2.0f * sin(itemAngleYRad / 2.0f) * m_itemScale;

	m_pTitleView->SetScale(objectScale);
//	m_pTitleQuad->SetScale(objectScale);

	PositionMenuButton(-1.0f + m_scrollBias, m_pLeftScrollButton);
	m_pLeftScrollButton->SetPosition(m_pLeftScrollButton->GetPosition() + m_pMenuButtonsContainer->GetPosition());

	PositionMenuButton(m_maxElements - m_scrollBias, m_pRightScrollButton);
	m_pRightScrollButton->SetPosition(m_pRightScrollButton->GetPosition() + m_pMenuButtonsContainer->GetPosition());

Error:
	return r;
}

// mostly borrowed from UIBar right now, current default values make sense with ray selection
RESULT UISpatialScrollView::PositionMenuButton(float index, std::shared_ptr<UIButton> pButton) {
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

RESULT UISpatialScrollView::UpdateMenuButtons(std::vector<std::shared_ptr<UIButton>> pButtons) {
	RESULT r = R_PASS;

	CN(m_pDreamOS);

	m_yRotation = 0.0f;
	m_velocity = 0.0f;
	//m_pMenuButtonsContainer->SetOrientation(quaternion::MakeQuaternionWithEuler(0.0f, m_yRotation, 0.0f));
	m_pMenuButtonsContainer->ResetRotation();

	if (m_pMenuButtonsContainer->HasChildren()) {
		for (auto& pObj : m_pMenuButtonsContainer->GetChildren()) {
			
			UIButton* pButton = dynamic_cast<UIButton*>(pObj.get());

			if (pButton != nullptr) {

				//TODO: this works for now, but it may be necessary to have some of the individual
				// RemoveObject functions properly cascade the call for future situations


				m_pDreamOS->UnregisterInteractionObject(pButton);
				m_pDreamOS->RemoveObjectFromInteractionGraph(pButton);

				m_pDreamOS->RemoveObjectFromUIClippingGraph(pButton);

				if (m_pDreamOS->GetInteractionEngineProxy()->IsAnimating(pButton)) {
					m_pDreamOS->GetInteractionEngineProxy()->RemoveAnimationObject(pButton);
				}

				m_pMenuButtonsContainer->RemoveChild(pButton);
				CR(m_pDreamOS->RemoveObject(pButton->GetSurface().get()));
				CR(m_pDreamOS->RemoveObject(pButton));

				//m_pDreamOS->RemoveObjectFromUIClippingGraph(pButton->GetSurface().get());
				//m_pDreamOS->RemoveObjectFromUIClippingGraph(pButton->GetSurfaceComposite().get());

				//m_pMenuButtonsContainer->RemoveChild(pButton);
				/*
				UIMenuItem* pMenuItem = reinterpret_cast<UIMenuItem*>(pObj.get());
				if (pMenuItem) {
					CR(m_pDreamOS->RemoveObject(pMenuItem->GetSurface().get()));
					//CR(m_pDreamOS->RemoveObject(pMenuItem));
				}
				//*/
			}
		}
	}

	CR(m_pMenuButtonsContainer->ClearChildren());

	int i = 0;
	for (auto& pButton : pButtons) {

		CN(pButton);

		if (i < m_maxElements) {
			CR(pButton->RegisterToInteractionEngine(m_pDreamOS));
		}

		//m_pDreamOS->AddObjectToUIClippingGraph(pButton->GetSurface().get());
		//m_pDreamOS->AddObjectToUIClippingGraph(pButton->GetSurfaceComposite().get());
		m_pDreamOS->AddObjectToUIClippingGraph(pButton.get());

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

RESULT UISpatialScrollView::HideAllButtons(UIButton* pPushButton) {
	RESULT r = R_PASS;

	m_fScrollButtonVisible = false;

	for (auto& pButton : m_pMenuButtonsContainer->GetChildren()) {
		auto pObj = dynamic_cast<UIButton*>(pButton.get());
		
		if (pObj != nullptr) {
			// delete menu buttons after they are hidden
			CR(HideObject(pObj, true));
		}

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

RESULT UISpatialScrollView::ShowTitle() {
	RESULT r = R_PASS;

	m_pTitleQuad->SetVisible(true);
	m_pTitleQuad->SetMaterialColors(m_visibleColor, true);
	m_pTitleText->SetVisible(true);
	m_pTitleText->SetMaterialColors(m_visibleColor, true);

	return r;
}

RESULT UISpatialScrollView::Snap() {
	RESULT r = R_PASS;
	
	float yRotationPerElement = (float)M_PI / (180.0f / m_itemAngleY);
	float rawIndex = m_yRotation / yRotationPerElement;
	float endYRotation = (float)(std::round(rawIndex)) * yRotationPerElement;
	float distance = std::abs(std::round(rawIndex) - rawIndex);
	//m_yRotation = endYRotation;
	
	auto fnStartCallback = [&](void *pContext) {
	//	m_pMenuButtonsContainer->SetVisible(true, true);
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		float yRotationPerElement = (float)M_PI / (180.0f / m_itemAngleY);
		int startIndex = std::round(m_yRotation / yRotationPerElement);
		float endYRotation = (float)(startIndex) * yRotationPerElement;
		m_yRotation = endYRotation;
	
		int index = 0;
		for (auto& pChild : m_pMenuButtonsContainer->GetChildren()) {
			if (index < startIndex || index >= startIndex + m_maxElements) {
				auto pButton = dynamic_cast<UIButton*>(pChild.get());
				pButton->SetVisible(false);
				m_pDreamOS->UnregisterInteractionObject(pButton);
				m_pDreamOS->RemoveObjectFromInteractionGraph(pButton);
			}
			index++;
		}
		
		return R_PASS;
	};

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pMenuButtonsContainer.get(),
		m_pMenuButtonsContainer->GetPosition(),
		quaternion::MakeQuaternionWithEuler(0.0f, endYRotation, 0.0f),
		m_pMenuButtonsContainer->GetScale(),
		distance * 0.25f,
		AnimationCurveType::LINEAR,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));

Error:
	return r;
}

RESULT UISpatialScrollView::Show() {
	RESULT r = R_PASS;

	m_pTitleView->SetVisible(true, false);
	m_pMenuButtonsContainer->SetVisible(true, false);

	return r;
}

//needed because these objects may be in different scene graphs
RESULT UISpatialScrollView::Hide() {
	RESULT r = R_PASS;

	m_pTitleView->SetVisible(false, false);
	m_pLeftScrollButton->SetVisible(false);
	m_pRightScrollButton->SetVisible(false);
	m_pMenuButtonsContainer->SetVisible(false, true);

	return r;
}

RESULT UISpatialScrollView::HideObject(DimObj* pObject, bool fDeleteObject) {
	RESULT r = R_PASS;

	std::function<RESULT(void*)> fnEndCallback = nullptr;

	if (fDeleteObject) {
		fnEndCallback = [&](void *pContext) {
			RESULT r = R_PASS;

			DimObj *pObj = reinterpret_cast<DimObj*>(pContext);
			pObj->SetVisible(false);

			// remove object in the next update after the animation is complete
			m_pendingObjectRemovalQueue.push(pObj);

			return R_PASS;
		};
	}
	else {
		fnEndCallback = [&](void *pContext) {
			RESULT r = R_PASS;

			DimObj *pObj = reinterpret_cast<DimObj*>(pContext);
			pObj->SetVisible(false);
			return R_PASS;
		};
	}

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

RESULT UISpatialScrollView::ShowObject(DimObj* pObject, color showColor /* = color(1.0f, 1.0f, 1.0f, 1.0f) */) {
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

RESULT UISpatialScrollView::HideAndPushButton(UIButton* pButton) {
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

ScrollState UISpatialScrollView::GetState() {
	return m_menuState;
}

RESULT UISpatialScrollView::SetScrollVisible(bool fVisible) {
	m_fScrollButtonVisible = fVisible;
	return R_PASS;
}

bool UISpatialScrollView::IsCapturable(UIButton *pButton) {

	if (!m_fScrollButtonVisible) {
		return false;
	}

	float yRotationPerElement = (float)M_PI / (180.0f / m_itemAngleY);
	int highIndex = (int)(m_yRotation / yRotationPerElement) + m_maxElements;
	int lowIndex = std::ceil(m_yRotation / yRotationPerElement);

	auto pChildren = m_pMenuButtonsContainer->GetChildren();
	for (int i = 0; i < pChildren.size(); i++) {
		auto pChildButton = dynamic_cast<UIButton*>(pChildren[i].get());
		if (pChildButton == pButton) {
			if (i >= lowIndex && i < highIndex) {
				return true;
			}
			return false;
		}
	}
	return true;
}

std::shared_ptr<quad> UISpatialScrollView::GetTitleQuad() {
	return m_pTitleQuad;
}

std::shared_ptr<text> UISpatialScrollView::GetTitleText() {
	return m_pTitleText;
}

std::shared_ptr<UIView> UISpatialScrollView::GetMenuItemsView() {
	return m_pMenuButtonsContainer;
}

float UISpatialScrollView::GetWidth() {
	return m_itemWidth;
}

float UISpatialScrollView::GetClippingThreshold() {
	return m_clippingThreshold;
}

float UISpatialScrollView::GetClippingRate() {
	return m_clippingRate;
}

RESULT UISpatialScrollView::Notify(SenseControllerEvent *pEvent) {
	RESULT r = R_PASS;
	switch (pEvent->type) {
	case SenseControllerEventType::SENSE_CONTROLLER_PAD_MOVE: {
		m_velocity = pEvent->state.ptTouchpad.x() * PAD_MOVE_CONSTANT;

		float yRotationPerElement = (float)M_PI / (180.0f / m_itemAngleY);
		if (m_velocity == 0.0f &&
			!m_pDreamOS->GetInteractionEngineProxy()->IsAnimating(m_pMenuButtonsContainer.get())) {

			float endYRotation = (float)(std::round(m_yRotation / yRotationPerElement)) * yRotationPerElement;
			if (m_yRotation != endYRotation)
				CR(Snap());
		}
		else if (m_velocity != 0.0f &&
			m_pDreamOS->GetInteractionEngineProxy()->IsAnimating(m_pMenuButtonsContainer.get()))
		{
			CR(m_pDreamOS->GetInteractionEngineProxy()->RemoveAnimationObject(m_pMenuButtonsContainer.get()));
		}
		if (m_velocity != 0.0f) {
			if (m_pMenuButtonsContainer->HasChildren()) {
				float maxRotation = (m_pMenuButtonsContainer->GetChildren().size() - m_maxElements) * yRotationPerElement;
			}
		}
	} break;

	}

Error:
	return r;
}
