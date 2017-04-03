#include "DreamBrowser.h"
#include "DreamOS.h"

#include "PhysicsEngine/CollisionManifold.h"

#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"
#include "WebBrowser/WebBrowserController.h"

DreamBrowser::DreamBrowser(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamBrowser>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

RESULT DreamBrowser::OnPaint(const WebBrowserRect &rect, const void *pBuffer, int width, int height) {
	RESULT r = R_PASS;

	CN(m_pBrowserTexture);
	CR(m_pBrowserTexture->Update((unsigned char*)(pBuffer), width, height, texture::PixelFormat::BGRA));
	
Error:
	return r;
}

// DreamApp Interface
RESULT DreamBrowser::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	std::string strURL = "http://www.google.com";

	int pxWidth = 512;
	int pxHeight = 512;
	std::vector<unsigned char> vectorByteBuffer(pxWidth * pxHeight * 4, 0xFF);

	// Subscribers (children)
	for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
		CR(GetDOS()->RegisterEventSubscriber((InteractionEventType)(i), this));
	}

	SetAppName("DreamContentView");
	SetAppDescription("A Shared Content View");

	// Set up browser manager
	m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
	CN(m_pWebBrowserManager);
	CR(m_pWebBrowserManager->Initialize());

	// Initialize new browser
	m_pWebBrowserController = m_pWebBrowserManager->CreateNewBrowser(pxWidth, pxHeight, strURL);
	CN(m_pWebBrowserController);
	CR(m_pWebBrowserController->RegisterWebBrowserControllerObserver(this));

	// Set up the quad
	SetNormalVector(vector(0.0f, 1.0f, 0.0f).Normal());
	m_pBrowserQuad = GetComposite()->AddQuad(GetWidth(), GetHeight(), 1, 1, nullptr, GetNormal());
	m_pBrowserQuad->SetMaterialAmbient(0.8f);

	// Set up and map the texture
	m_pBrowserTexture = GetComposite()->MakeTexture(texture::TEXTURE_TYPE::TEXTURE_COLOR, pxWidth, pxHeight, texture::PixelFormat::RGBA, 4, &vectorByteBuffer[0], pxWidth * pxHeight * 4);
	m_pBrowserQuad->SetMaterialTexture(DimObj::MaterialTexture::Ambient, m_pBrowserTexture.get());
	m_pBrowserQuad->SetMaterialTexture(DimObj::MaterialTexture::Diffuse, m_pBrowserTexture.get());

	// Set up mouse / hand cursor model
	///*
	m_pPointerCursor = GetComposite()->AddModel(L"\\Models\\mouse-cursor\\mouse-cursor.obj",
												nullptr,
												point(-0.2f, -0.43f, 0.0f),
												0.01f,
												vector(-(float)M_PI_2, 0.0f, 0.0f));
	m_pPointerCursor->SetMaterialAmbient(0.8f);
	//*/

	/*
	m_pHandCursor = GetComposite()->AddModel(L"\\Models\\mouse-hand\\mouse-hand.obj",
											 nullptr,
											 point(0.0f, 0.0f, 0.0f),
											 0.002f,
											 vector(0.0f, 0.0f, 0.0f));
	//*/

	GetDOS()->AddInteractionObject(m_pBrowserQuad.get());

Error:
	return r;
}

RESULT DreamBrowser::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamBrowser::Update(void *pContext) {
	RESULT r = R_PASS;

	ray rCast;

	if (m_pWebBrowserManager != nullptr) {
		CR(m_pWebBrowserManager->Update());
	}

	CR(GetDOS()->GetMouseRay(rCast, 0.0f));

	{
		CollisionManifold manifold = m_pBrowserQuad->Collide(rCast);

		if (manifold.NumContacts() > 0) {
			m_pPointerCursor->SetOrigin(manifold.GetContactPoint(0).GetPoint());
		}
	}

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