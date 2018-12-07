#include "UITabView.h"

#include "DreamUserControlArea/DreamUserControlArea.h"
#include "DreamUserControlArea/DreamContentSource.h"

#include "UI/UIFlatScrollView.h"
#include "UI/UIButton.h"
#include "Primitives/quad.h"

#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"

UITabView::UITabView(HALImp *pHALImp, DreamOS *pDreamOS) :
	UIView(pHALImp, pDreamOS)
{
	// empty
}

UITabView::~UITabView()
{
	// empty
}

RESULT UITabView::Update() {
	RESULT r = R_PASS;

	if (m_pScrollView != nullptr) {
		CR(m_pScrollView->Update());
		point ptOrigin = GetPosition(true);
		quaternion qOrigin = GetOrientation(true);
		m_pScrollView->GetRenderContext()->SetPosition(ptOrigin);
		m_pScrollView->GetRenderContext()->SetOrientation(qOrigin);
	}
	if (!m_pendingSelectTabQueue.empty()) {
		auto selectTabArgs = m_pendingSelectTabQueue.front();
		m_pendingSelectTabQueue.pop();
		CR(SelectTab(selectTabArgs.first, selectTabArgs.second));
	}

	if (m_pTabPendingRemoval != nullptr && m_fAllowObjectRemoval) {
		m_pScrollView->RemoveChild(m_pTabPendingRemoval);

		m_pDreamOS->UnregisterInteractionObject(m_pTabPendingRemoval.get());
		m_pDreamOS->RemoveObjectFromInteractionGraph(m_pTabPendingRemoval.get());
		m_pDreamOS->RemoveObjectFromUIGraph(m_pTabPendingRemoval.get());

		m_pTabPendingRemoval = nullptr;
		m_fAllowObjectRemoval = false;
	}

	if (m_fDeleteAllTabs) {
		CRM(ShutdownAllSources(), "Shutdown of content failed");
	}

Error:
	return r;
}

RESULT UITabView::Initialize(DreamUserControlArea *pParent) {
	RESULT r = R_PASS;

	m_pParentApp = pParent;

	float baseWidth = m_pParentApp->GetBaseWidth();
	float baseHeight = m_pParentApp->GetBaseHeight();
	m_itemSpacing = m_pParentApp->GetSpacingSize() * baseWidth;
	float borderWidth = m_borderWidth * baseWidth;
	float borderHeight = m_borderHeight * baseWidth;

	//m_borderHeight *= baseWidth;
	//m_borderWidth *= baseWidth;

	// ties this app to the control area's size
	float tabWidth = m_tabWidth * baseWidth;
	
	float tabHeight = (borderHeight - 5.0f * m_itemSpacing) / 4.0f;

	m_pScrollView = MakeUIFlatScrollView();
	CN(m_pScrollView);

	//sometimes the top sliver of the fifth tab gets rendered on the bottom
	tabHeight *= 1.001f;
	
	m_tabWidth = tabWidth;
	m_tabHeight = tabHeight;

	m_pBackgroundQuad = AddQuad(borderWidth, borderHeight);
	CN(m_pBackgroundQuad);
	m_pRenderQuad = AddQuad(borderWidth, borderHeight);// -m_itemSpacing);
	CN(m_pRenderQuad);

	{
		auto pRenderContext = MakeFlatContext();
		CN(pRenderContext);
		pRenderContext->SetIsAbsolute(true);
		pRenderContext->SetAbsoluteBounds(m_pRenderQuad->GetWidth(), m_pRenderQuad->GetHeight());

		m_pScrollView->SetRenderQuad(m_pRenderQuad, pRenderContext);
		m_pScrollView->SetScrollSnapDistance(tabHeight + m_itemSpacing);
		m_pScrollView->SetTabWidth(tabWidth);
		m_pScrollView->SetTabHeight(tabHeight);

		//unclear how the object structure should work here, should UIFlatScrollView be a FlatContext?
		m_pScrollView->GetRenderContext()->AddObject(m_pScrollView);
	}

	m_pBackgroundTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszTabBackground);
	CN(m_pBackgroundTexture);

	m_pBackgroundQuad->SetDiffuseTexture(m_pBackgroundTexture);
	m_pBackgroundQuad->SetPosition(point(0.0f, -0.0005f, 0.0f));

	SetPosition(point(2*m_itemSpacing + (baseWidth + borderWidth) / 2.0f, 0.0f, -((baseHeight + 2*m_itemSpacing) - borderHeight)/2.0f));

	m_ptMostRecent = point(0.0f, 0.0f, (-borderHeight / 2.0f) + (tabHeight / 2.0f) + (m_itemSpacing));

Error:
	return r;
}

float UITabView::GetBorderWidth() {
	return m_borderWidth;
}

float UITabView::GetBorderHeight() {
	return m_borderHeight;
}

RESULT UITabView::SetScrollFlag(bool fCanScroll, int index) {
	m_pScrollView->SetScrollFlag(fCanScroll, index);
	return R_PASS;
}

std::shared_ptr<UIButton> UITabView::CreateTab(std::shared_ptr<DreamContentSource> pContent) {
	RESULT r = R_PASS;
	std::shared_ptr<UIButton> pNewTabButton = nullptr;

//	auto pContent = m_pParentApp->GetActiveSource();

	pNewTabButton = m_pScrollView->AddUIButton(m_tabWidth, m_tabHeight);

	pNewTabButton->GetSurface()->SetDiffuseTexture(pContent->GetSourceTexture());
	
	if (!pContent->GetSourceTexture()->IsUVVerticalFlipped()) {
		pNewTabButton->GetSurface()->FlipUVVertical();
	}

	pNewTabButton->SetPosition(m_ptMostRecent);
	pNewTabButton->GetSurface()->RotateXByDeg(-90.0f);

	pNewTabButton->RegisterToInteractionEngine(m_pDreamOS);
	pNewTabButton->RegisterEvent(UIEventType::UI_SELECT_ENDED,
		std::bind(&UITabView::PendSelectTab, this, std::placeholders::_1, std::placeholders::_2));

	return pNewTabButton;
}

RESULT UITabView::AddContent(std::shared_ptr<DreamContentSource> pContent) {
	RESULT r = R_PASS;

	auto pNewTabButton = CreateTab(pContent);
	CN(pNewTabButton);

	for (auto pButton : m_tabButtons) {
		pButton->SetPosition(pButton->GetPosition() + point(0.0f, 0.0f, m_tabHeight + (m_itemSpacing)));
		//TranslateTabDown(pButton.get());
	}

	m_tabButtons.emplace_back(pNewTabButton);
	m_sources.emplace_back(pContent);
	m_appToTabMap[pContent] = pNewTabButton;

Error:
	return r;
}

std::shared_ptr<DreamContentSource> UITabView::RemoveContent() {
	
	RESULT r = R_PASS;
	auto pDreamOS = m_pDreamOS;

	CBR(m_tabButtons.size() > 0, R_SKIPPED);
	{
		auto pActiveContent = m_sources.back();
		auto pButtonToRemove = m_tabButtons.back();

		m_appToTabMap.erase(m_sources.back());
		m_sources.pop_back();
		m_tabButtons.pop_back();

		m_pTabPendingRemoval = pButtonToRemove;
		HideTab(pButtonToRemove.get());

		for (auto pButton : m_tabButtons) {
		//	pButton->SetPosition(pButton->GetPosition() - point(0.0f, 0.0f, m_tabHeight + (m_pParentApp->GetSpacingSize() / 2.0f)));
			TranslateTabUp(pButton.get());
		}
		return pActiveContent;
	}
Error:
	return nullptr;
}

RESULT UITabView::PendSelectTab(UIButton *pButtonContext, void *pContext) {
	RESULT r = R_PASS;

	if (!m_fForceContentFocus) {
		CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	}
	m_fForceContentFocus = false;
	CR(m_pParentApp->HideWebsiteTyping());

	//TODO: currently rejects events if there is a select going on
	if (m_pendingSelectTabQueue.empty()) {
		m_pendingSelectTabQueue.push(std::pair<UIButton*, void*>(pButtonContext, pContext));
	}

Error:
	return r;
}

RESULT UITabView::SelectTab(UIButton *pButtonContext, void *pContext) {
	RESULT r = R_PASS;

	std::shared_ptr<UIButton> pNewTabButton = nullptr;
	auto pContent = m_pParentApp->GetActiveSource();
	auto tabTexture = pContent->GetSourceTexture();
	auto pDreamOS = m_pDreamOS;

	pNewTabButton = CreateTab(pContent);
	CR(ShowTab(pNewTabButton.get()));

	for (int i = (int)m_tabButtons.size() - 1; i >= 0; i--) {
		auto pButton = m_tabButtons[i];
		if (pButton.get() == pButtonContext) {

			m_pTabPendingRemoval = pButton;
			HideTab(pButton.get());

			m_pParentApp->SetActiveSource(m_sources[i]);

			m_appToTabMap.erase(m_sources[i]);
			m_tabButtons.erase(m_tabButtons.begin() + i);
			m_sources.erase(m_sources.begin() + i);

			break;
		} 
		else {
			//pButton->SetPosition(pButton->GetPosition() + point(0.0f, 0.0f, m_tabHeight + (m_itemSpacing)));
			TranslateTabDown(pButton.get());
		}
	}

	m_tabButtons.emplace_back(pNewTabButton);
	m_sources.emplace_back(pContent);
	m_appToTabMap[pContent] = pNewTabButton;

Error:
	return r;
}

RESULT UITabView::SelectByContent(std::shared_ptr<DreamContentSource> pContent) {
	if (m_appToTabMap.count(pContent) > 0) {
		m_fForceContentFocus = true;
		PendSelectTab(m_appToTabMap[pContent].get(), nullptr);
		//SelectTab(m_appToTabMap[pContent].get(), nullptr);
	}
	return R_PASS;
}

RESULT UITabView::UpdateContentTexture(std::shared_ptr<DreamContentSource> pContent) {

	if (m_appToTabMap.count(pContent) > 0) {
		m_appToTabMap[pContent]->GetSurface()->SetDiffuseTexture(pContent->GetSourceTexture());
	}

	return R_PASS;
}

std::vector<std::shared_ptr<DreamContentSource>> UITabView::GetAllSources() {
	return m_sources;
}

RESULT UITabView::FlagShutdownAllSources() {
	RESULT r = R_PASS;

	m_fDeleteAllTabs = true;
	return r;
}

RESULT UITabView::ShutdownAllSources() {
	RESULT r = R_PASS;

	m_fDeleteAllTabs = false;

	for (auto pSource : m_sources) {
		pSource->CloseSource();
		pSource = nullptr;
	}
	m_sources.clear();

	for (auto pButton : m_tabButtons) {
		m_pDreamOS->UnregisterInteractionObject(pButton.get());
		m_pDreamOS->RemoveObjectFromInteractionGraph(pButton.get());
		m_pDreamOS->RemoveObjectFromUIGraph(pButton.get());
		CRM(m_pScrollView->RemoveChild(pButton), "failed to remove content from tab view");
		pButton = nullptr;
	}
	m_tabButtons.clear();

	m_appToTabMap.clear();

Error:
	return r;
}

RESULT UITabView::Hide() {
	RESULT r = R_PASS;

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		SetVisible(false);

		return r;
	};

	for (auto pButton : m_tabButtons) {
		CR(HideTab(pButton.get()));
	}

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
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

RESULT UITabView::Show() {
	RESULT r = R_PASS;

	SetVisible(true);

	for (auto pButton : m_tabButtons) {
		CR(ShowTab(pButton.get()));
	}

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pBackgroundQuad.get(),
		color(1.0f, 1.0f, 1.0f, 1.0f),
		m_pParentApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags()
	));

Error:
	return r;
}

RESULT UITabView::HideTab(UIButton *pTabButton) {
	RESULT r = R_PASS;

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		if (m_pTabPendingRemoval != nullptr) {
			m_pTabPendingRemoval->SetVisible(false);
			m_fAllowObjectRemoval = true;
		}

		return r;
	};

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
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

RESULT UITabView::ShowTab(UIButton *pTabButton) {
	RESULT r = R_PASS;
	
	pTabButton->SetScale(m_pParentApp->GetAnimationScale());

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
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

RESULT UITabView::TranslateTabDown(DimObj *pTabButton) {
	RESULT r = R_PASS;

	point ptDisplacement = point(0.0f, 0.0f, m_tabHeight + (m_itemSpacing));

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		pTabButton,
		pTabButton->GetPosition() + ptDisplacement,
		pTabButton->GetOrientation(),
		pTabButton->GetScale(),
		m_pParentApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags()
	));

Error:
	return r;
}

RESULT UITabView::TranslateTabUp(DimObj *pTabButton) {
	RESULT r = R_PASS;

	point ptDisplacement = point(0.0f, 0.0f, m_tabHeight + (m_pParentApp->GetSpacingSize() / 2.0f));

	/*
	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		pTabButton->GetSurface().get(),
		pTabButton->GetSurface()->GetPosition() - ptDisplacement,
		pTabButton->GetSurface()->GetOrientation(),
		pTabButton->GetSurface()->GetScale(),
		m_pParentApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags()
	));
	//*/
	//*
	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
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
