#include "DreamTabView.h"

#include "DreamUserControlArea/DreamUserControlArea.h"
#include "DreamUserControlArea/DreamContentSource.h"

#include "UI/UIFlatScrollView.h"
#include "UI/UIButton.h"
#include "Primitives/quad.h"

#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"

DreamTabView::DreamTabView(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamTabView>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

DreamTabView::~DreamTabView(){
	RESULT r = R_PASS;

	CR(Shutdown());

Error:
	return;
}

// DreamApp Interface
RESULT DreamTabView::InitializeApp(void *pContext) {

	GetDOS()->AddObjectToUIGraph(GetComposite());
	m_pView = GetComposite()->AddUIView(GetDOS());
	//m_pScrollView = m_pView->AddUIFlatScrollView();
	m_pScrollView = m_pView->MakeUIFlatScrollView();

	return R_PASS;
}

RESULT DreamTabView::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamTabView::Update(void *pContext) {
	RESULT r = R_PASS;

	if (m_pScrollView != nullptr) {
		CR(m_pScrollView->Update());
	}
	//if (m_pScrollView->GetCurrentTexture() != nullptr) {
	//	m_pRenderQuad->SetDiffuseTexture(m_pScrollView->GetCurrentTexture());
	//}

Error:
	return r;
}

RESULT DreamTabView::Shutdown(void *pContext) {
	return R_PASS;
}

DreamTabView* DreamTabView::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamTabView *pDreamApp = new DreamTabView(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamTabView::InitializeWithParent(DreamUserControlArea *pParent) {
	RESULT r = R_PASS;

	m_pParentApp = pParent;

	float baseWidth = m_pParentApp->GetBaseWidth();
	float baseHeight = m_pParentApp->GetBaseHeight();
	float itemSpacing = m_pParentApp->GetSpacingSize();
	float borderWidth = m_borderWidth * baseWidth;
	float borderHeight = m_borderHeight * baseWidth;

	//m_borderHeight *= baseWidth;
	//m_borderWidth *= baseWidth;

	// ties this app to the control area's size
	m_tabWidth *= baseWidth;
	m_tabHeight *= baseWidth;

	m_pBackgroundQuad = GetComposite()->AddQuad(borderWidth, borderHeight);
	m_pRenderQuad = GetComposite()->AddQuad(borderWidth, borderHeight);

	// wack
	m_pScrollView->SetRenderQuad(m_pRenderQuad);
	m_pScrollView->GetRenderContext()->AddObject(m_pScrollView);
	m_pBackgroundTexture = GetDOS()->MakeTexture(k_wszTabBackground, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pBackgroundQuad->SetDiffuseTexture(m_pBackgroundTexture);
	m_pBackgroundQuad->SetPosition(point(0.0f, -0.0005f, 0.0f));

	GetComposite()->SetPosition(point(baseWidth / 2.0f + itemSpacing + borderWidth / 2.0f, 0.0f, -itemSpacing/2.0f -(baseHeight - borderHeight) / 2.0f));

	m_ptMostRecent = point(0.0f, 0.0f, (-borderHeight / 2.0f) + (m_tabHeight / 2.0f) + (itemSpacing / 2.0f));

	CR(m_pScrollView->SetBounds(borderWidth, borderHeight));

Error:
	return r;
}

float DreamTabView::GetBorderWidth() {
	return m_borderWidth;
}

float DreamTabView::GetBorderHeight() {
	return m_borderHeight;
}

RESULT DreamTabView::SetScrollFlag(bool fCanScroll, int index) {
	m_pScrollView->SetScrollFlag(fCanScroll, index);
	return R_PASS;
}

std::shared_ptr<UIButton> DreamTabView::CreateTab() {
	RESULT r = R_PASS;
	std::shared_ptr<UIButton> pNewTabButton = nullptr;

	auto pContent = m_pParentApp->GetActiveSource();
	auto pDreamOS = GetDOS();

	pNewTabButton = m_pScrollView->AddUIButton(m_tabWidth, m_tabHeight);
	//m_pScrollView->AddObject(pNewTabButton);

	pNewTabButton->GetSurface()->SetDiffuseTexture(pContent->GetSourceTexture().get());
	pNewTabButton->GetSurface()->FlipUVVertical();

	pNewTabButton->SetPosition(m_ptMostRecent);
	pNewTabButton->GetSurface()->RotateXByDeg(-90.0f);

	pNewTabButton->RegisterToInteractionEngine(GetDOS());
	pNewTabButton->RegisterEvent(UIEventType::UI_SELECT_ENDED,
		std::bind(&DreamTabView::SelectTab, this, std::placeholders::_1, std::placeholders::_2));

	return pNewTabButton;
}

RESULT DreamTabView::AddContent(std::shared_ptr<DreamContentSource> pContent) {
	RESULT r = R_PASS;

	auto pNewTabButton = CreateTab();
	CN(pNewTabButton);

	for (auto pButton : m_tabButtons) {
		//pButton->SetPosition(pButton->GetPosition() + point(0.0f, 0.0f, m_tabHeight + (m_pParentApp->GetSpacingSize() / 2.0f)));
		TranslateTabDown(pButton.get());
	}

	m_tabButtons.emplace_back(pNewTabButton);
	m_sources.emplace_back(pContent);
	m_appToTabMap[pContent] = pNewTabButton;

Error:
	return r;
}

std::shared_ptr<DreamContentSource> DreamTabView::RemoveContent() {
	
	RESULT r = R_PASS;
	auto pDreamOS = GetDOS();

	CBR(m_tabButtons.size() > 0, R_SKIPPED);
	{
		auto pActiveContent = m_sources.back();
		auto pButtonToRemove = m_tabButtons.back();

		m_appToTabMap.erase(m_sources.back());
		m_sources.pop_back();
		m_tabButtons.pop_back();

		m_pTabPendingRemoval = pButtonToRemove;
		HideTab(pButtonToRemove.get());
		//m_pView->RemoveChild(pButtonToRemove);
		pDreamOS->UnregisterInteractionObject(pButtonToRemove.get());
		pDreamOS->RemoveObjectFromInteractionGraph(pButtonToRemove.get());
		pDreamOS->RemoveObjectFromUIGraph(pButtonToRemove.get());

		for (auto pButton : m_tabButtons) {
		//	pButton->SetPosition(pButton->GetPosition() - point(0.0f, 0.0f, m_tabHeight + (m_pParentApp->GetSpacingSize() / 2.0f)));
			TranslateTabUp(pButton.get());
		}
		return pActiveContent;
	}
Error:
	return nullptr;
}

RESULT DreamTabView::SelectTab(UIButton *pButtonContext, void *pContext) {
	RESULT r = R_PASS;

	std::shared_ptr<UIButton> pNewTabButton = nullptr;
	auto pContent = m_pParentApp->GetActiveSource();
	auto tabTexture = pContent->GetSourceTexture().get();
	auto pDreamOS = GetDOS();

	if (!m_fForceContentFocus) {
		CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	}
	m_fForceContentFocus = false;
	CR(m_pParentApp->HideWebsiteTyping());

	pNewTabButton = CreateTab();
	ShowTab(pNewTabButton.get());

	for (int i = (int)m_tabButtons.size() - 1; i >= 0; i--) {
		auto pButton = m_tabButtons[i];
		if (pButton.get() == pButtonContext) {

//			m_pView->RemoveChild(pButton);
			m_pTabPendingRemoval = pButton;
			HideTab(pButton.get());

			pDreamOS->UnregisterInteractionObject(pButton.get());
			pDreamOS->RemoveObjectFromInteractionGraph(pButton.get());
			pDreamOS->RemoveObjectFromUIGraph(pButton.get());

			m_pParentApp->SetActiveSource(m_sources[i]);

			m_appToTabMap.erase(m_sources[i]);
			m_tabButtons.erase(m_tabButtons.begin() + i);
			m_sources.erase(m_sources.begin() + i);

			break;
		} 
		else {
			//pButton->SetPosition(pButton->GetPosition() + point(0.0f, 0.0f, m_tabHeight + (m_pParentApp->GetSpacingSize() / 2.0f)));
			TranslateTabDown(pButton.get());
		}
	}

	m_tabButtons.emplace_back(pNewTabButton);
	m_sources.emplace_back(pContent);
	m_appToTabMap[pContent] = pNewTabButton;

Error:
	return r;
}

RESULT DreamTabView::SelectByContent(std::shared_ptr<DreamContentSource> pContent) {
	if (m_appToTabMap.count(pContent) > 0) {
		m_fForceContentFocus = true;
		SelectTab(m_appToTabMap[pContent].get(), nullptr);
	}
	return R_PASS;
}

RESULT DreamTabView::UpdateContentTexture(std::shared_ptr<DreamContentSource> pContent) {

	if (m_appToTabMap.count(pContent) > 0) {
		m_appToTabMap[pContent]->GetSurface()->SetDiffuseTexture(pContent->GetSourceTexture().get());
	}

	return R_PASS;
}

RESULT DreamTabView::Hide() {
	RESULT r = R_PASS;

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		GetComposite()->SetVisible(false);

		return r;
	};

	for (auto pButton : m_tabButtons) {
		CR(HideTab(pButton.get()));
	}

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pBackgroundQuad.get(),
		color(1.0f, 1.0f, 1.0f, 0.0f),
		m_pParentApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags(),
		nullptr,
		fnEndCallback,
		this
	));

Error:
	return r;
}

RESULT DreamTabView::Show() {
	RESULT r = R_PASS;

	GetComposite()->SetVisible(true);

	for (auto pButton : m_tabButtons) {
		CR(ShowTab(pButton.get()));
	}

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pBackgroundQuad.get(),
		color(1.0f, 1.0f, 1.0f, 1.0f),
		m_pParentApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags()
	));

Error:
	return r;
}

RESULT DreamTabView::HideTab(UIButton *pTabButton) {
	RESULT r = R_PASS;

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		if (m_pTabPendingRemoval != nullptr) {
			m_pTabPendingRemoval->SetVisible(false);
			m_pView->RemoveChild(m_pTabPendingRemoval);
			m_pTabPendingRemoval = nullptr;
		}

		return r;
	};

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pTabButton,
		pTabButton->GetPosition(),
		pTabButton->GetOrientation(),
		m_pParentApp->GetAnimationScale(),
		m_pParentApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags(),
		nullptr,
		fnEndCallback,
		this
	));

Error:
	return r;
}

RESULT DreamTabView::ShowTab(UIButton *pTabButton) {
	RESULT r = R_PASS;
	
	pTabButton->SetScale(m_pParentApp->GetAnimationScale());

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pTabButton,
		pTabButton->GetPosition(),
		pTabButton->GetOrientation(),
		vector(1.0f),
		m_pParentApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags()
	));

Error:
	return r;
}

RESULT DreamTabView::TranslateTabDown(UIButton *pTabButton) {
	RESULT r = R_PASS;

	point ptDisplacement = point(0.0f, 0.0f, m_tabHeight + (m_pParentApp->GetSpacingSize() / 2.0f));

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pTabButton->GetSurface().get(),
		pTabButton->GetSurface()->GetPosition() + ptDisplacement,
		pTabButton->GetSurface()->GetOrientation(),
		pTabButton->GetSurface()->GetScale(),
		m_pParentApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags()
	));

Error:
	return r;
}

RESULT DreamTabView::TranslateTabUp(UIButton *pTabButton) {
	RESULT r = R_PASS;

	point ptDisplacement = point(0.0f, 0.0f, m_tabHeight + (m_pParentApp->GetSpacingSize() / 2.0f));

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pTabButton->GetSurface().get(),
		pTabButton->GetSurface()->GetPosition() - ptDisplacement,
		pTabButton->GetSurface()->GetOrientation(),
		pTabButton->GetSurface()->GetScale(),
		m_pParentApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags()
	));
	/*
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pTabButton,
		pTabButton->GetPosition() - ptDisplacement,
		pTabButton->GetOrientation(),
		pTabButton->GetScale(),
		m_pParentApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags()
	));
	//*/

Error:
	return r;
}
