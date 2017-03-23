#include "DreamContentView.h"
#include "DreamOS.h"

#include "Cloud/HTTP/HTTPController.h"

DreamContentView::DreamContentView(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamContentView>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

RESULT DreamContentView::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	// Subscribers (children)
	for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
		CR(GetDOS()->RegisterEventSubscriber((InteractionEventType)(i), this));
	}

	SetAppName("DreamContentView");
	SetAppDescription("A Shared Content View");

	// Set up the quad
	SetNormalVector(vector(0.0f, 1.0f, 0.0f).Normal());
	m_pScreenQuad = GetComposite()->AddQuad(GetWidth(), GetHeight(), 1, 1, nullptr, GetNormal());

Error:
	return r;
}

RESULT DreamContentView::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

float DreamContentView::GetWidth() {
	return std::sqrt(((m_aspectRatio * m_aspectRatio) * (m_diagonalSize * m_diagonalSize)) / (1.0f + (m_aspectRatio * m_aspectRatio)));
}

float DreamContentView::GetHeight() {
	return GetWidth() / m_aspectRatio;
}

RESULT DreamContentView::UpdateViewQuad() {
	RESULT r = R_PASS;

	CR(m_pScreenQuad->UpdateParams(GetWidth(), GetHeight(), GetNormal()));
	CR(m_pScreenQuad->SetDirty());

	CR(m_pScreenQuad->InitializeBoundingQuad(point(0.0f, 0.0f, 0.0f), GetWidth(), GetHeight(), GetNormal()));

Error:
	return r;
}

RESULT DreamContentView::SetScreenTexture(texture *pTexture) {
	return m_pScreenQuad->SetColorTexture(pTexture);
}

RESULT DreamContentView::SetScreenURI(const std::string &strURI) {
	RESULT r = R_PASS;

	// Cloud Controller
	HTTPControllerProxy *pHTTPControllerProxy = (HTTPControllerProxy*)GetDOS()->GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE::HTTP);
	CNM(pHTTPControllerProxy, "Failed to get http controller proxy");

	// Set up file request
	DEBUG_LINEOUT("Requesting File %s", strURI.c_str());
	
	CR(pHTTPControllerProxy->RequestFile(strURI));

Error:
	return r;
}

RESULT DreamContentView::SetScreenTexture(const std::wstring &wstrTextureFilename) {
	RESULT r = R_PASS;

	texture *pTexture = GetDOS()->MakeTexture(const_cast<wchar_t*>(wstrTextureFilename.c_str()), texture::TEXTURE_TYPE::TEXTURE_COLOR);
	CN(pTexture);

	CR(SetScreenTexture(pTexture));

Error:
	return r;
}

RESULT DreamContentView::Update(void *pContext) {
	RESULT r = R_PASS;

	int a = 5;
	CR(r);

Error:
	return r;
}
	   
RESULT DreamContentView::Notify(InteractionObjectEvent *event) {
	RESULT r = R_PASS;

	/*
	std::shared_ptr<UIMenuItem> pItem = GetMenuItem(event->m_pObject);
	CBR(pItem != nullptr, R_OBJECT_NOT_FOUND);

	//TODO stupid hack, can be fixed by incorporating 
	// SenseController into the Interaction Engine
	if (event->m_eventType == InteractionEventType::ELEMENT_INTERSECT_ENDED)
		m_pCurrentItem = nullptr;
	else
		m_pCurrentItem = pItem;

	*/

	CR(r);

Error:
	return r;
}

DreamContentView* DreamContentView::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamContentView *pDreamApp = new DreamContentView(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamContentView::SetAspectRatio(float aspectRatio) {
	m_aspectRatio = aspectRatio;

	if (m_pScreenQuad != nullptr)
		return UpdateViewQuad();

	return R_PASS;
}

RESULT DreamContentView::SetDiagonalSize(float diagonalSize) {
	m_diagonalSize = diagonalSize;

	if (m_pScreenQuad != nullptr)
		return UpdateViewQuad();

	return R_PASS;
}

RESULT DreamContentView::SetParams(point ptPosition, float diagonal, AspectRatio aspectRatio, vector vNormal) {
	GetComposite()->SetPosition(ptPosition);
	m_diagonalSize = diagonal;
	m_aspectRatio = k_aspectRatios[aspectRatio];
	m_vNormal = vNormal.Normal();

	if (m_pScreenQuad != nullptr)
		return UpdateViewQuad();

	return R_PASS;
}

RESULT DreamContentView::SetNormalVector(vector vNormal) {
	m_vNormal = vNormal.Normal();

	if(m_pScreenQuad != nullptr)
		return UpdateViewQuad();

	return R_PASS;
}

vector DreamContentView::GetNormal() {
	return m_vNormal;
}

point DreamContentView::GetOrigin() {
	return GetComposite()->GetOrigin();
}