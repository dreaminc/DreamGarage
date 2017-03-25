#include "DreamBrowser.h"
#include "DreamOS.h"

DreamBrowser::DreamBrowser(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamBrowser>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

// DreamApp Interface
RESULT DreamBrowser::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	// Subscribers (children)
	for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
		CR(GetDOS()->RegisterEventSubscriber((InteractionEventType)(i), this));
	}

	SetAppName("DreamContentView");
	SetAppDescription("A Shared Content View");

	// Set up the quad
	SetNormalVector(vector(0.0f, 1.0f, 0.0f).Normal());
	m_pBrowserQuad = GetComposite()->AddQuad(GetWidth(), GetHeight(), 1, 1, nullptr, GetNormal());
	m_pBrowserQuad->SetMaterialAmbient(0.8f);

Error:
	return r;
}

RESULT DreamBrowser::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamBrowser::Update(void *pContext = nullptr) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

// InteractionObjectEvent
RESULT DreamBrowser::Notify(InteractionObjectEvent *event) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}


RESULT DreamBrowser::SetAspectRatio(float aspectRatio) {
	m_aspectRatio = aspectRatio;

	if (m_pBrowserQuad != nullptr)
		return UpdateViewQuad();

	return R_PASS;
}

RESULT DreamBrowser::SetDiagonalSize(float diagonalSize) {
	m_diagonalSize = diagonalSize;

	if (m_pBrowserQuad != nullptr)
		return UpdateViewQuad();

	return R_PASS;
}

RESULT DreamBrowser::SetNormalVector(vector vNormal) {
	m_vNormal = vNormal.Normal();

	if (m_pBrowserQuad != nullptr)
		return UpdateViewQuad();

	return R_PASS;
}

RESULT DreamBrowser::SetParams(point ptPosition, float diagonal, float aspectRatio, vector vNormal) {
	GetComposite()->SetPosition(ptPosition);
	m_diagonalSize = diagonal;
	m_aspectRatio = aspectRatio;
	m_vNormal = vNormal.Normal();

	if (m_pBrowserQuad != nullptr)
		return UpdateViewQuad();

	return R_PASS;
}

float DreamBrowser::GetWidth() {
	return std::sqrt(((m_aspectRatio * m_aspectRatio) * (m_diagonalSize * m_diagonalSize)) / (1.0f + (m_aspectRatio * m_aspectRatio)));
}

float DreamBrowser::GetHeight() {
	return GetWidth() / m_aspectRatio;
}

vector DreamBrowser::GetNormal() {
	return m_vNormal;
}

point DreamBrowser::GetOrigin() {
	return GetComposite()->GetOrigin();
}

RESULT DreamBrowser::UpdateViewQuad() {
	RESULT r = R_PASS;

	CR(m_pBrowserQuad->UpdateParams(GetWidth(), GetHeight(), GetNormal()));
	CR(m_pBrowserQuad->SetDirty());

	CR(m_pBrowserQuad->InitializeBoundingQuad(point(0.0f, 0.0f, 0.0f), GetWidth(), GetHeight(), GetNormal()));

Error:
	return r;
}

RESULT DreamBrowser::SetVisible(bool fVisible) {
	return m_pBrowserQuad->SetVisible(fVisible);
}

RESULT DreamBrowser::SetURI(std::string strURI) {
	// TODO: 
	return R_NOT_IMPLEMENTED;
}


RESULT DreamBrowser::SetScreenTexture(texture *pTexture) {
	m_aspectRatio = (float)pTexture->GetWidth() / (float)pTexture->GetHeight();
	SetParams(GetOrigin(), m_diagonalSize, m_aspectRatio, m_vNormal);

	return m_pBrowserQuad->SetColorTexture(pTexture);
}

DreamBrowser* DreamBrowser::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamBrowser *pDreamApp = new DreamBrowser(pDreamOS, pContext);
	return pDreamApp;
}