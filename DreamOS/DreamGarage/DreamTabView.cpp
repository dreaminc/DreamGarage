#include "DreamTabView.h"

#include "DreamUserControlArea/DreamUserControlArea.h"
#include "DreamUserControlArea/DreamContentSource.h"

#include "UI/UIButton.h"
#include "Primitives/quad.h"

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

	return R_PASS;
}

RESULT DreamTabView::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamTabView::Update(void *pContext) {
	return R_PASS;
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

	// ties this app to the control area's size
	m_tabWidth *= baseWidth;
	m_tabHeight *= baseWidth;

	m_pBackgroundQuad = GetComposite()->AddQuad(borderWidth, borderHeight);
	m_pBackgroundTexture = GetDOS()->MakeTexture(k_wszTabBackground, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pBackgroundQuad->SetDiffuseTexture(m_pBackgroundTexture);
	m_pBackgroundQuad->SetPosition(point(0.0f, -0.0005f, 0.0f));

	GetComposite()->SetPosition(point(baseWidth / 2.0f + itemSpacing + borderWidth / 2.0f, 0.0f, -itemSpacing/2.0f -(baseHeight - borderHeight) / 2.0f));

	m_ptMostRecent = point(0.0f, 0.0f, (-borderHeight / 2.0f) + (m_tabHeight / 2.0f) + (itemSpacing / 2.0f));
//Error:
	return r;
}

RESULT DreamTabView::AddContent(std::shared_ptr<DreamContentSource> pContent) {
	RESULT r = R_PASS;

	auto newTabButton = m_pView->AddUIButton(m_tabWidth, m_tabHeight);
	auto tabTexture = pContent->GetSourceTexture().get();
	newTabButton->GetSurface()->SetDiffuseTexture(pContent->GetSourceTexture().get());
	newTabButton->GetSurface()->FlipUVVertical();

	newTabButton->SetPosition(m_ptMostRecent);
	newTabButton->GetSurface()->RotateXByDeg(-90.0f);
	newTabButton->RegisterToInteractionEngine(GetDOS());
	newTabButton->RegisterEvent(UIEventType::UI_SELECT_ENDED,
		std::bind(&DreamTabView::SelectTab, this, std::placeholders::_1, std::placeholders::_2));
	

	for (auto pButton : m_tabButtons) {
		pButton->SetPosition(pButton->GetPosition() + point(0.0f, 0.0f, m_tabHeight + (m_pParentApp->GetSpacingSize() / 2.0f)));
	}

	m_tabButtons.emplace_back(newTabButton);
	m_sources.emplace_back(pContent);
	m_appToTabMap[pContent] = newTabButton;

	return R_PASS;
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

		pButtonToRemove->SetVisible(false);
		m_pView->RemoveChild(pButtonToRemove);
		pDreamOS->UnregisterInteractionObject(pButtonToRemove.get());
		pDreamOS->RemoveObjectFromInteractionGraph(pButtonToRemove.get());
		pDreamOS->RemoveObjectFromUIGraph(pButtonToRemove.get());

		for (auto pButton : m_tabButtons) {
			pButton->SetPosition(pButton->GetPosition() - point(0.0f, 0.0f, m_tabHeight + (m_pParentApp->GetSpacingSize() / 2.0f)));
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

	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	CR(m_pParentApp->HideWebsiteTyping());

	pNewTabButton = m_pView->AddUIButton(m_tabWidth, m_tabHeight);

	pNewTabButton->GetSurface()->SetDiffuseTexture(pContent->GetSourceTexture().get());
	pNewTabButton->GetSurface()->FlipUVVertical();

	pNewTabButton->SetPosition(m_ptMostRecent);
	pNewTabButton->GetSurface()->RotateXByDeg(-90.0f);

	pNewTabButton->RegisterToInteractionEngine(GetDOS());
	pNewTabButton->RegisterEvent(UIEventType::UI_SELECT_ENDED,
		std::bind(&DreamTabView::SelectTab, this, std::placeholders::_1, std::placeholders::_2));

	for (int i = (int)m_tabButtons.size() - 1; i >= 0; i--) {
		auto pButton = m_tabButtons[i];
		if (pButton.get() == pButtonContext) {

			m_pView->RemoveChild(pButton);
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
			pButton->SetPosition(pButton->GetPosition() + point(0.0f, 0.0f, m_tabHeight + (m_pParentApp->GetSpacingSize() / 2.0f)));
		}
	}

	m_tabButtons.emplace_back(pNewTabButton);
	m_sources.emplace_back(pContent);
	m_appToTabMap[pContent] = pNewTabButton;

Error:
	return r;
}

RESULT DreamTabView::UpdateContentTexture(std::shared_ptr<DreamContentSource> pContent) {

	if (m_appToTabMap.count(pContent) > 0) {
		m_appToTabMap[pContent]->GetSurface()->SetDiffuseTexture(pContent->GetSourceTexture().get());
	}

	return R_PASS;
}