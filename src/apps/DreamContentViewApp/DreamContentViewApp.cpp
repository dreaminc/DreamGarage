#include "DreamContentViewApp.h"

// TODO: Do apps need DreamOS?
#include "os/DreamOS.h"

#include "cloud/HTTP/HTTPController.h"
#include "cloud/Environment/EnvironmentAsset.h"

DreamContentViewApp::DreamContentViewApp(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamContentViewApp>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

DreamContentViewApp* DreamContentViewApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamContentViewApp *pDreamApp = new DreamContentViewApp(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamContentViewApp::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	// Subscribers (children)
	//for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
	//	CR(GetDOS()->RegisterEventSubscriber((InteractionEventType)(i), this));
	//}

	CR(r);

	SetAppName("DreamContentView");
	SetAppDescription("A Shared Content View");

	// Set up the quad
	SetNormalVector(vector(0.0f, 1.0f, 0.0f).Normal());
	m_pScreenQuad = GetComposite()->AddQuad(GetWidth(), GetHeight(), 1, 1, nullptr, GetNormal());
	m_pScreenQuad->SetMaterialAmbient(0.8f);

Error:
	return r;
}

RESULT DreamContentViewApp::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamContentViewApp::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamContentViewApp::Update(void *pContext) {
	RESULT r = R_PASS;

	if (m_pPendingBufferVector != nullptr) {
		uint8_t* pBuffer = &(m_pPendingBufferVector->operator[](0));
		size_t pBuffer_n = m_pPendingBufferVector->size();

		texture *pTexture = GetDOS()->MakeTextureFromFileBuffer(texture::type::TEXTURE_2D, pBuffer, pBuffer_n);
		CN(pTexture);
		CV(pTexture);

		CR(SetScreenTexture(pTexture));
	}

Error:
	if (m_pPendingBufferVector != nullptr) {
		m_pPendingBufferVector = nullptr;
	}

	return r;
}

float DreamContentViewApp::GetWidth() {
	return std::sqrt(((m_aspectRatio * m_aspectRatio) * (m_diagonalSize * m_diagonalSize)) / (1.0f + (m_aspectRatio * m_aspectRatio)));
}

float DreamContentViewApp::GetHeight() {
	return GetWidth() / m_aspectRatio;
}

RESULT DreamContentViewApp::UpdateViewQuad() {
	RESULT r = R_PASS;

	CR(m_pScreenQuad->UpdateParams(GetWidth(), GetHeight(), GetNormal()));
	CR(m_pScreenQuad->SetDirty());

	CR(m_pScreenQuad->InitializeBoundingQuad(point(0.0f, 0.0f, 0.0f), GetWidth(), GetHeight(), GetNormal()));

Error:
	return r;
}

RESULT DreamContentViewApp::SetScreenTexture(texture *pTexture) {
	if (m_fFitTextureAspectRatio) {
		m_aspectRatio = (float)pTexture->GetWidth() / (float)pTexture->GetHeight();
		SetParams(GetOrigin(), m_diagonalSize, m_aspectRatio, m_vNormal);
	}

	return m_pScreenQuad->SetDiffuseTexture(pTexture);
}

RESULT DreamContentViewApp::HandleOnFileResponse(std::shared_ptr<std::vector<uint8_t>> pBufferVector) {
	RESULT r = R_PASS;

	CN(pBufferVector);
	CBM((m_pPendingBufferVector == nullptr), "New buffer already pending");
	
	// This is a thread safe increment - the pending buffer will get 
	// picked up by the update (in the UI safe zone) and a texture will be created / set
	m_pPendingBufferVector = pBufferVector;

Error:
	return r;
}

RESULT DreamContentViewApp::SetEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;

	if (pEnvironmentAsset != nullptr) {
		std::string strEnvironmentAssetURI = pEnvironmentAsset->GetURI();
		CR(SetScreenURI(strEnvironmentAssetURI));
	}

Error:
	return r;
}

RESULT DreamContentViewApp::SetFitTextureAspectRatio(bool fFitTextureAspectRatio) {
	m_fFitTextureAspectRatio = fFitTextureAspectRatio;
	return R_PASS;
}

RESULT DreamContentViewApp::SetVisible(bool fVisible) {
	return m_pScreenQuad->SetVisible(fVisible);
}

RESULT DreamContentViewApp::SetScreenURI(const std::string &strURI) {
	RESULT r = R_PASS;

	// Cloud Controller
	HTTPControllerProxy *pHTTPControllerProxy = (HTTPControllerProxy*)GetDOS()->GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE::HTTP);
	CNM(pHTTPControllerProxy, "Failed to get http controller proxy");

	//UserControllerProxy *pUserControllerProxy = (UserControllerProxy*)GetDOS()->GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE::USER);
	//CNM(pUserControllerProxy, "Failed to get user controller proxy");

	// Set up file request
	DEBUG_LINEOUT("Requesting File %s", strURI.c_str());
	{
		//auto strHeaders = HTTPController::ContentAcceptJson();
		auto strHeaders = HTTPController::ContentHttp();
		
		//std::string strAuthorizationToken = "Authorization: Bearer " + pUserControllerProxy->GetUserToken();
		//strHeaders.push_back(strAuthorizationToken);

		CR(pHTTPControllerProxy->RequestFile(strURI, strHeaders, "", std::bind(&DreamContentViewApp::HandleOnFileResponse, this, std::placeholders::_1)));
	}

Error:
	return r;
}

RESULT DreamContentViewApp::SetScreenTexture(const std::wstring &wstrTextureFilename) {
	RESULT r = R_PASS;

	texture *pTexture = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, const_cast<wchar_t*>(wstrTextureFilename.c_str()));
	CN(pTexture);

	CR(SetScreenTexture(pTexture));

Error:
	return r;
}
	   
RESULT DreamContentViewApp::Notify(InteractionObjectEvent *event) {
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

RESULT DreamContentViewApp::SetAspectRatio(float aspectRatio) {
	m_aspectRatio = aspectRatio;

	if (m_pScreenQuad != nullptr)
		return UpdateViewQuad();

	return R_PASS;
}

RESULT DreamContentViewApp::SetDiagonalSize(float diagonalSize) {
	m_diagonalSize = diagonalSize;

	if (m_pScreenQuad != nullptr)
		return UpdateViewQuad();

	return R_PASS;
}

RESULT DreamContentViewApp::SetParams(point ptPosition, float diagonal, float aspectRatio, vector vNormal) {
	GetComposite()->SetPosition(ptPosition);
	m_diagonalSize = diagonal;
	m_aspectRatio = aspectRatio;
	m_vNormal = vNormal.Normal();

	if (m_pScreenQuad != nullptr)
		return UpdateViewQuad();

	return R_PASS;
}

RESULT DreamContentViewApp::SetParams(point ptPosition, float diagonal, AspectRatio aspectRatio, vector vNormal) {
	return SetParams(ptPosition, diagonal, k_aspectRatios[aspectRatio], vNormal);
}

RESULT DreamContentViewApp::SetNormalVector(vector vNormal) {
	m_vNormal = vNormal.Normal();

	if(m_pScreenQuad != nullptr)
		return UpdateViewQuad();

	return R_PASS;
}

vector DreamContentViewApp::GetNormal() {
	return m_vNormal;
}

point DreamContentViewApp::GetOrigin() {
	return GetComposite()->GetOrigin();
}