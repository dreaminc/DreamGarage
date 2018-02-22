#include "DreamTabView.h"

#include "DreamUserControlArea/DreamUserControlArea.h"
#include "DreamGarage/DreamBrowser.h"

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

RESULT DreamTabView::AddBrowser(std::shared_ptr<DreamBrowser> pBrowser) {
	RESULT r = R_PASS;

	auto newTabButton = m_pView->AddUIButton(m_tabWidth, m_tabHeight);
	auto tabTexture = pBrowser->GetScreenTexture().get();
	newTabButton->GetSurface()->SetDiffuseTexture(pBrowser->GetScreenTexture().get());
	newTabButton->GetSurface()->FlipUVVertical();

	newTabButton->SetPosition(m_ptMostRecent);
	newTabButton->GetSurface()->RotateXByDeg(-90.0f);

	for (auto pButton : m_tabButtons) {
		pButton->SetPosition(pButton->GetPosition() + point(0.0f, 0.0f, m_tabHeight + (m_pParentApp->GetSpacingSize() / 2.0f)));
	}

	m_tabButtons.emplace_back(newTabButton);
	m_appToTabMap[pBrowser] = newTabButton;

	return R_PASS;
}
RESULT DreamTabView::RemoveBrowser(std::shared_ptr<DreamBrowser> pBrowser) {
	return R_PASS;
}
RESULT DreamTabView::UpdateBrowserTexture(std::shared_ptr<DreamBrowser> pBrowser) {

	if (m_appToTabMap.count(pBrowser) > 0) {
		m_appToTabMap[pBrowser]->GetSurface()->SetDiffuseTexture(pBrowser->GetScreenTexture().get());
	}

	return R_PASS;
}