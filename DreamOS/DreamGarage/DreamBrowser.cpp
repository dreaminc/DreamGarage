#include "DreamBrowser.h"
#include "DreamControlView/DreamControlView.h"
#include "DreamShareView/DreamShareView.h"
#include "DreamUserControlArea/DreamUserControlArea.h"
#include "DreamOS.h"
#include "Core/Utilities.h"

#include "PhysicsEngine/CollisionManifold.h"

#include "InteractionEngine/AnimationItem.h"

#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"
#include "WebBrowser/WebBrowserController.h"

#include "Cloud/Environment/EnvironmentAsset.h"

#include "Cloud/WebRequest.h"

#include "WebBrowser/DOMNode.h"
#include "Sound/AudioPacket.h"

DreamBrowser::DreamBrowser(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamBrowser>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

DreamBrowser::~DreamBrowser(){
	/*
	RESULT r = R_PASS;

	CR(Shutdown());

Error:
	return;
	//*/
}

RESULT DreamBrowser::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	/*
	if (m_pWebBrowserManager != nullptr) {
		CR(m_pWebBrowserManager->Shutdown());
		//m_pWebBrowserManager = nullptr;
	}
	//*/

//Error:
	return r;
}

RESULT DreamBrowser::ScrollBrowserToPoint(int pxXScroll, int pxYScroll) {
	RESULT r = R_PASS;
	
	CNR(m_pWebBrowserController, R_SKIPPED);

	WebBrowserMouseEvent mouseEvent;
	mouseEvent.pt = m_lastWebBrowserPoint;

	int pxXDiff, pxYDiff;

	if (pxXScroll > m_pxXPosition) {
		pxXDiff = pxXScroll - m_pxXPosition;
	}
	else {
		pxXDiff = m_pxXPosition - pxXScroll;
	}
	
	if (pxYScroll > m_pxYPosition) {
		pxYDiff = pxYScroll - m_pxYPosition;
	}
	else {
		pxYDiff = m_pxYPosition - pxYScroll;
	}

	m_pxXPosition += pxXDiff;
	m_pxYPosition += pxYDiff;

	CR(m_pWebBrowserController->SendMouseWheel(mouseEvent, pxXDiff, pxYDiff));

Error:
	return r;
}

RESULT DreamBrowser::ScrollBrowserToX(int pxXScroll) {
	RESULT r = R_PASS;
	
	CNR(m_pWebBrowserController, R_SKIPPED);

	WebBrowserMouseEvent mouseEvent;
	mouseEvent.pt = m_lastWebBrowserPoint;
	
	int pxXDiff;

	if (pxXScroll > m_pxXPosition) {
		pxXDiff = pxXScroll - m_pxXPosition;
	}
	else {
		pxXDiff = m_pxXPosition - pxXScroll;
	}

	m_pxXPosition += pxXDiff;

	CR(m_pWebBrowserController->SendMouseWheel(mouseEvent, pxXDiff, 0));

Error:
	return r;
}

RESULT DreamBrowser::ScrollBrowserToY(int pxYScroll) {
	RESULT r = R_PASS;
	
	CNR(m_pWebBrowserController, R_SKIPPED);

	WebBrowserMouseEvent mouseEvent;
	mouseEvent.pt = m_lastWebBrowserPoint;

	int pxYDiff;

	if (pxYScroll > m_pxYPosition) {
		pxYDiff = pxYScroll - m_pxYPosition;
	}
	else {
		pxYDiff = m_pxYPosition - pxYScroll;
	}

	m_pxYPosition += pxYDiff;
	
	CR(m_pWebBrowserController->SendMouseWheel(mouseEvent, 0, pxYDiff));

Error:
	return r;
}

RESULT DreamBrowser::OnScroll(float pxXDiff, float pxYDiff, point scrollPoint) {
	RESULT r = R_PASS;
	
	CNR(m_pWebBrowserController, R_SKIPPED);

	WebBrowserPoint ptWebContact;
	ptWebContact.x = (int)(scrollPoint.x());
	ptWebContact.y = (int)(scrollPoint.y());

	WebBrowserMouseEvent mouseEvent;
	mouseEvent.pt = ptWebContact;

	m_pxXPosition += (int)pxXDiff;
	m_pxYPosition += (int)pxYDiff;
	
	m_fScroll = true;
	m_pxYScroll = pxYDiff;
	m_pxXScroll = pxXDiff;
	m_mouseEvent = mouseEvent;
	//CR(m_pWebBrowserController->SendMouseWheel(mouseEvent, (int)pxXDiff, (int)pxYDiff));

Error:
	return r;
}

RESULT DreamBrowser::ScrollBrowserXByDiff(int pxXDiff) {
	RESULT r = R_PASS;

	CNR(m_pWebBrowserController, R_SKIPPED);

	WebBrowserMouseEvent mouseEvent;
	mouseEvent.pt = m_lastWebBrowserPoint;

	m_pxXPosition += pxXDiff;

	CR(m_pWebBrowserController->SendMouseWheel(mouseEvent, pxXDiff, 0));

Error:
	return r;
}

RESULT DreamBrowser::ScrollBrowserYByDiff(int pxYDiff) {
	RESULT r = R_PASS;
	
	CNR(m_pWebBrowserController, R_SKIPPED);

	WebBrowserMouseEvent mouseEvent;
	mouseEvent.pt = m_lastWebBrowserPoint;

	m_pxYPosition += pxYDiff;

	CR(m_pWebBrowserController->SendMouseWheel(mouseEvent, 0, pxYDiff));

Error:
	return r;
}

int DreamBrowser::GetPageHeight() {	// This is incomplete, should be entire page- from CEF
	return -1;
}

int DreamBrowser::GetPageWidth() {
	return -1;
}

int DreamBrowser::GetScrollX() {
	return m_pxXPosition;
}

int DreamBrowser::GetScrollY() {
	return m_pxYPosition;
}

int DreamBrowser::GetBrowserHeight() {
	return m_browserHeight;
}

int DreamBrowser::GetBrowserWidth() {
	return m_browserWidth;
}

RESULT DreamBrowser::OnKeyPress(char chKey, bool fkeyDown) {
	RESULT r = R_PASS;
	CBR(chKey != SVK_SHIFT, R_SKIPPED);		// don't send these key codes to browser (capital letters and such have different values already)
	CBR(chKey != 0, R_SKIPPED);
	CBR(chKey != SVK_CONTROL, R_SKIPPED);

	CNR(m_pWebBrowserController, R_SKIPPED);
	CR(m_pWebBrowserController->SendKeyEventChar(chKey, fkeyDown));
Error:
	return r;
}

RESULT DreamBrowser::CreateBrowserSource(std::string strURL) {
	RESULT r = R_PASS;

//	SetVisible(true);

	std::string strTitle = "website";
	SetPath(strURL);
	auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");

	CRM(m_pEnvironmentControllerProxy->RequestOpenAsset(m_strScope, m_strPath, strTitle), "Failed to share environment asset");

Error:
	return r;
}

RESULT DreamBrowser::OnMouseMove(point mousePoint) {
	RESULT r = R_PASS;

	WebBrowserMouseEvent mouseEvent;
	mouseEvent.pt.x = (int)mousePoint.x();
	mouseEvent.pt.y = (int)mousePoint.y();

	CR(m_pWebBrowserController->SendMouseMove(mouseEvent));
Error:
	return r;
}

RESULT DreamBrowser::OnClick(point ptContact, bool fMouseDown) {
	RESULT r = R_PASS;

	CNR(m_pWebBrowserController, R_SKIPPED);
	WebBrowserMouseEvent mouseEvent;

	WebBrowserPoint ptWebContact;
	ptWebContact.x = (int)(ptContact.x());
	ptWebContact.y = (int)(ptContact.y());

	mouseEvent.pt = ptWebContact;
	m_lastWebBrowserPoint = ptWebContact;

	mouseEvent.mouseButton = WebBrowserMouseEvent::MOUSE_BUTTON::LEFT;
	
	CR(m_pWebBrowserController->SendMouseClick(mouseEvent, fMouseDown, 1));		// mouse down

Error:
	return r;
}

RESULT DreamBrowser::OnAfterCreated() {
	RESULT r = R_PASS;

	return r;
}

RESULT DreamBrowser::OnLoadingStateChange(bool fLoading, bool fCanGoBack, bool fCanGoForward, std::string strCurrentURL) {
	RESULT r = R_PASS;

	if (!fLoading) {
		m_strCurrentURL = strCurrentURL;
		CR(PendUpdateObjectTextures());
	}

Error:
	return r;
}

RESULT DreamBrowser::OnLoadStart() {
	RESULT r = R_PASS;	

	return r;
}

RESULT DreamBrowser::OnLoadEnd(int httpStatusCode, std::string strCurrentURL) {
	RESULT r = R_PASS;
	
	m_strCurrentURL = strCurrentURL;

	if (m_pParentApp != nullptr) {
		CR(PendUpdateObjectTextures());
	}

Error:
	return r;
}

RESULT DreamBrowser::OnNodeFocusChanged(DOMNode *pDOMNode) {
	RESULT r = R_PASS;

	bool fMaskPasswordEnabled = false;
	if (pDOMNode->GetType() == DOMNode::type::ELEMENT && pDOMNode->IsEditable()) {
		if (m_pParentApp != nullptr && m_pParentApp->IsContentVisible()) {
			std::string strTextField = pDOMNode->GetValue();
			point ptTextBox = point(0.0f, m_lastWebBrowserPoint.y, 0.0f);
			CR(m_pParentApp->ShowKeyboard(strTextField, ptTextBox));
		}
		fMaskPasswordEnabled = pDOMNode->IsPassword();
	}

	if (m_pDreamUserHandle != nullptr) {
		auto pKeyboardHandle = m_pDreamUserHandle->RequestKeyboard();
		if (pKeyboardHandle != nullptr) {
			pKeyboardHandle->SendPasswordFlag(fMaskPasswordEnabled);
		}
		m_pDreamUserHandle->SendReleaseKeyboard();
		pKeyboardHandle = nullptr;
	}

Error:
	return r;
}

RESULT DreamBrowser::GetResourceHandlerType(ResourceHandlerType &resourceHandlerType, std::string strURL) {
	RESULT r = R_PASS;
	
	std::map<std::string, ResourceHandlerType>::iterator it;

	if (!m_dreamResourceHandlerLinks.empty()) {
		it = m_dreamResourceHandlerLinks.find(strURL);
		if (it != m_dreamResourceHandlerLinks.end()) {
			resourceHandlerType = it->second;
		}
	}

	else {
		resourceHandlerType = ResourceHandlerType::DEFAULT;
	}

	return r;
}

RESULT DreamBrowser::CheckForHeaders(std::multimap<std::string, std::string> &headermap, std::string strURL) {
	RESULT r = R_PASS;

	std::map<std::string, std::multimap<std::string, std::string>>::iterator it;

	if (!m_headermap.empty()) {
		it = m_headermap.find(strURL);
		if (it != m_headermap.end()) {
			headermap = it->second;
		}
	}

	return r;
}

RESULT DreamBrowser::DreamFormSuccess() {
	RESULT r = R_PASS;

	int a = 5;

	return r;
}

RESULT DreamBrowser::HandleBackEvent() {
	RESULT r = R_PASS;

	CBR(m_pWebBrowserController->CanGoBack(), R_SKIPPED);
	CR(m_pWebBrowserController->GoBack());

Error:
	return r;
}

RESULT DreamBrowser::HandleForwardEvent() {
	RESULT r = R_PASS;

	CBR(m_pWebBrowserController->CanGoForward(), R_SKIPPED);
	CR(m_pWebBrowserController->GoForward());

Error:
	return r;
}

RESULT DreamBrowser::HandleStopEvent() {
	RESULT r = R_PASS;
/*
	auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");

	CR(m_pEnvironmentControllerProxy->RequestStopSharing(m_currentEnvironmentAssetID, m_strScope, m_strPath));
	CR(SetStreamingState(false));
Error:
//*/
	return r;
}

// DreamApp Interface
RESULT DreamBrowser::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	CommandLineManager *pCommandLineManager = nullptr;
	std::string strAPIURL;
	std::string strURL;	

	int pxWidth = m_browserWidth;
	int pxHeight = m_browserHeight;
	m_aspectRatio = ((float)pxWidth / (float)pxHeight);

	std::vector<unsigned char> vectorByteBuffer(pxWidth * pxHeight * 4, 0xFF);

	SetAppName("DreamBrowser");
	SetAppDescription("A Shared Content View");

	// TODO: may want browser to have its own shader at some point, but 
	// after testing with the UI graph, the browser clashed with the menu
	//
//	GetDOS()->AddObjectToUIGraph(GetComposite());	

/*
	// Get loading screen URL
	pCommandLineManager = CommandLineManager::instance();
	CN(pCommandLineManager);
	strAPIURL = pCommandLineManager->GetParameterValue("www.ip");
	strURL = strAPIURL + "/client/loading/";
	SetBrowserPath(strURL);

	// Initialize new browser
	m_pWebBrowserController = m_pWebBrowserManager->CreateNewBrowser(pxWidth, pxHeight, strURL);
	CN(m_pWebBrowserController);
	CR(m_pWebBrowserController->RegisterWebBrowserControllerObserver(this));
*/
	// Set up the quad
	SetNormalVector(vector(0.0f, 1.0f, 0.0f).Normal());

	// Set up and map the texture
	m_pBrowserTexture = GetComposite()->MakeTexture(texture::TEXTURE_TYPE::TEXTURE_DIFFUSE, pxWidth, pxHeight, PIXEL_FORMAT::RGBA, 4, &vectorByteBuffer[0], pxWidth * pxHeight * 4);	
	m_pLoadingScreenTexture = GetComposite()->MakeTexture((wchar_t*)(L"client-loading-1366-768.png"), texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	CN(m_pLoadingScreenTexture);
	//m_pBrowserTexture = m_pLoadingScreenTexture;

	// Set up mouse / hand cursor model
	///*
	GetComposite()->InitializeOBB();
	
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
	if (m_pWebBrowserManager != nullptr) {
		CR(m_pWebBrowserManager->Update());
	}
	else {
		SetVisible(false);
	}

	if (ShouldUpdateObjectTextures()) {
		if (m_pWebBrowserController != nullptr) {
			CR(UpdateNavigationFlags());
		}
		CR(UpdateObjectTextures());
	}

	if (m_pDreamUserHandle == nullptr) {
		auto pDreamOS = GetDOS();
		CNR(pDreamOS, R_OBJECT_NOT_FOUND);
		auto userAppIDs = pDreamOS->GetAppUID("DreamUserApp");
		CBR(userAppIDs.size() == 1, R_SKIPPED);
		m_pDreamUserHandle = dynamic_cast<DreamUserApp*>(pDreamOS->CaptureApp(userAppIDs[0], this));
	}

	if (m_fScroll) {
		CR(m_pWebBrowserController->SendMouseWheel(m_mouseEvent, (int)m_pxXScroll, (int)m_pxYScroll));
		m_fScroll = false;
	}
	
Error:
	return r;
}

RESULT DreamBrowser::InitializeWithBrowserManager(std::shared_ptr<WebBrowserManager> pWebBrowserManager, std::string strURL) {
	RESULT r = R_PASS;

	int pxWidth = m_browserWidth;
	int pxHeight = m_browserHeight;
	m_aspectRatio = ((float)pxWidth / (float)pxHeight);

	CN(pWebBrowserManager);
	CNM(m_pWebBrowserManager == nullptr, "Manager already created");
	m_pWebBrowserManager = pWebBrowserManager;

	m_pWebBrowserController = m_pWebBrowserManager->CreateNewBrowser(pxWidth, pxHeight, strURL);
	CN(m_pWebBrowserController);
	CR(m_pWebBrowserController->RegisterWebBrowserControllerObserver(this));

Error:
	return r;
}

RESULT DreamBrowser::InitializeWithParent(DreamUserControlArea *pParentApp) {
	m_pParentApp = pParentApp;
	PendUpdateObjectTextures();
	return R_PASS;
}

std::shared_ptr<texture> DreamBrowser::GetSourceTexture() {
	return m_pBrowserTexture;
}

long DreamBrowser::GetCurrentAssetID() {
	return m_assetID;
}

RESULT DreamBrowser::SetCurrentAssetID(long assetID) {
	m_assetID = assetID;
	return R_PASS;
}

RESULT DreamBrowser::CloseSource() {
	RESULT r = R_PASS;

	CR(m_pWebBrowserController->CloseBrowser());
	CR(m_pWebBrowserManager->RemoveBrowser(m_pWebBrowserController));
	
Error:
	return r;
}

RESULT DreamBrowser::PendUpdateObjectTextures() {
	m_fUpdateObjectTextures = true;
	return R_PASS;
}

bool DreamBrowser::ShouldUpdateObjectTextures() {
	return m_fUpdateObjectTextures;
}

RESULT DreamBrowser::UpdateObjectTextures() {
	RESULT r = R_PASS;

	if (m_pParentApp != nullptr) {
		if (m_pParentApp->GetActiveSource()->GetSourceTexture().get() == m_pBrowserTexture.get()) {
			CR(m_pParentApp->UpdateContentSourceTexture(m_pBrowserTexture, this));
		}
	}

	m_fUpdateObjectTextures = false;

Error:
	return r;
}

RESULT DreamBrowser::UpdateNavigationFlags() {
	RESULT r = R_PASS;

	bool fCanGoBack = m_pWebBrowserController->CanGoBack();
	bool fCanGoForward = m_pWebBrowserController->CanGoForward();

	if (m_pParentApp != nullptr) {
		CR(m_pParentApp->UpdateControlBarNavigation(fCanGoBack, fCanGoForward));
	}

Error:
	return r;
}

// TODO: Only update the rect
// TODO: Turn off CEF when we're not using it
RESULT DreamBrowser::OnPaint(const WebBrowserRect &rect, const void *pBuffer, int width, int height) {
	RESULT r = R_PASS;

	//CNR(m_pParentApp != nullptr, R_SKIPPED);
	CNR(m_pBrowserTexture, R_SKIPPED);

	// Update texture dimensions if needed
	CR(m_pBrowserTexture->UpdateDimensions(width, height));
	if (r != R_NOT_HANDLED) {
		DEBUG_LINEOUT("Changed chrome texture dimensions");
	}

	CR(m_pBrowserTexture->Update((unsigned char*)(pBuffer), width, height, PIXEL_FORMAT::BGRA));

	// when the browser gets a paint event, it checks if its texture is currently shared
	// if so, it tells the shared view to broadcast a frame
	CNR(GetDOS()->GetSharedContentTexture().get(), R_SKIPPED);
	CBR(GetSourceTexture().get() == GetDOS()->GetSharedContentTexture().get(), R_SKIPPED);
	GetDOS()->BroadcastSharedVideoFrame((unsigned char*)(pBuffer), width, height);

Error:
	return r;
}

RESULT DreamBrowser::OnAudioPacket(const AudioPacket &pendingAudioPacket) {
	RESULT r = R_PASS;

	// TODO: Handle this (if streaming we broadcast into webrtc
	if (m_pParentApp != nullptr && GetDOS()->GetSharedContentTexture() == m_pBrowserTexture) {
		CR(m_pParentApp->HandleAudioPacket(pendingAudioPacket, this));
	}

	/*
	if (m_fStreaming) {
		CR(GetDOS()->GetCloudController()->BroadcastAudioPacket(kChromeAudioLabel, pendingAudioPacket));
	}
	//*/

Error:
	return r;
}

RESULT DreamBrowser::HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamBrowser::BroadcastDreamBrowserMessage(DreamShareViewMessage::type msgType, DreamShareViewMessage::type ackType) {
	RESULT r = R_PASS;

	DreamShareViewMessage *pDreamBrowserMessage = new DreamShareViewMessage(0, 0, GetAppUID(), msgType, ackType);
	CN(pDreamBrowserMessage);

	CR(BroadcastDreamAppMessage(pDreamBrowserMessage));

Error:
	return r;
}


RESULT DreamBrowser::SetPosition(point ptPosition) {
	GetComposite()->SetPosition(ptPosition);
	return R_PASS;
}

RESULT DreamBrowser::SetAspectRatio(float aspectRatio) {
	m_aspectRatio = aspectRatio;

	return R_PASS;
}

RESULT DreamBrowser::SetDiagonalSize(float diagonalSize) {
	m_diagonalSize = diagonalSize;

	return R_PASS;
}

RESULT DreamBrowser::SetNormalVector(vector vNormal) {
	m_vNormal = vNormal.Normal();

	return R_PASS;
}

RESULT DreamBrowser::SetParams(point ptPosition, float diagonal, float aspectRatio, vector vNormal) {
	GetComposite()->SetPosition(ptPosition);
	m_diagonalSize = diagonal;
	m_aspectRatio = aspectRatio;
	m_vNormal = vNormal.Normal();

	return R_PASS;
}

float DreamBrowser::GetHeightFromAspectDiagonal() {
	return std::sqrt((m_diagonalSize * m_diagonalSize) / (1.0f + (m_aspectRatio * m_aspectRatio)));
}

float DreamBrowser::GetWidthFromAspectDiagonal() {
	return std::sqrt(((m_aspectRatio * m_aspectRatio) * (m_diagonalSize * m_diagonalSize)) / (1.0f + (m_aspectRatio * m_aspectRatio)));
}

int DreamBrowser::GetWidth() {
	return m_browserWidth;
}

int DreamBrowser::GetHeight() {
	return m_browserHeight;
}

RESULT DreamBrowser::SetTitle(std::string strTitle) {
	RESULT r = R_PASS;
	CNR(m_pParentApp, R_SKIPPED);
	if (strTitle != "") {
		m_strCurrentTitle = strTitle;
		CR(m_pParentApp->UpdateControlBarText(strTitle));
	}
	else {
		CR(m_pParentApp->UpdateControlBarText(m_strCurrentURL));
	}
Error:
	return r;
}

std::string DreamBrowser::GetTitle() {
	std::string strValidTitle;
	if (m_strCurrentTitle == "") {
		strValidTitle = m_strCurrentURL;
	}
	else {
		strValidTitle = m_strCurrentTitle;
	}
	return strValidTitle;
}

std::string DreamBrowser::GetContentType() {
	return m_strContentType;
}

vector DreamBrowser::GetNormal() {
	return m_vNormal;
}

point DreamBrowser::GetOrigin() {
	return GetComposite()->GetOrigin();
}

bool DreamBrowser::IsVisible() {
	return GetComposite()->IsVisible();
}

RESULT DreamBrowser::SetVisible(bool fVisible) {
	RESULT r = R_PASS;
		
	CR(GetComposite()->SetVisible(fVisible));
Error:
	return r;
}

RESULT DreamBrowser::SetScope(std::string strScope) {
	m_strScope = strScope;
	return R_PASS;
}

RESULT DreamBrowser::SetPath(std::string strPath) {
	m_strPath = strPath;
	return R_PASS;
}

RESULT DreamBrowser::PendEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	if (m_fCanLoadRequest) {
		SetEnvironmentAsset(pEnvironmentAsset);
	}
	else {
		m_pPendingEnvironmentAsset = pEnvironmentAsset;

		if (pEnvironmentAsset != nullptr) {
			m_strScope = pEnvironmentAsset->GetStorageProviderScope();
			m_strPath = pEnvironmentAsset->GetPath();
		}
	}
	return R_PASS;
}

RESULT DreamBrowser::SetEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;

	if (pEnvironmentAsset != nullptr) {
		m_assetID = pEnvironmentAsset->GetAssetID();
		WebRequest webRequest;

		//std::string strEnvironmentAssetURI = pEnvironmentAsset->GetURI();
		std::string strEnvironmentAssetURL = pEnvironmentAsset->GetURL();
		ResourceHandlerType resourceHandlerType = pEnvironmentAsset->GetResourceHandlerType();

		if (resourceHandlerType == ResourceHandlerType::DREAM) {	// Keeping it flexible, it's very possible there's only default and dream
			m_dreamResourceHandlerLinks[strEnvironmentAssetURL] = resourceHandlerType;
		}

		m_strContentType = pEnvironmentAsset->GetContentType();

		//std::wstring wstrAssetURI = util::StringToWideString(strEnvironmentAssetURI);
		std::wstring wstrAssetURL = util::StringToWideString(strEnvironmentAssetURL);
		CR(webRequest.SetURL(wstrAssetURL));
		CR(webRequest.SetRequestMethod(WebRequest::Method::GET));
		//*
		UserControllerProxy *pUserControllerProxy = (UserControllerProxy*)GetDOS()->GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE::USER);
		CN(pUserControllerProxy);

		std::multimap<std::wstring, std::wstring> wstrRequestHeaders;
		std::multimap<std::string, std::string> requestHeaders = pEnvironmentAsset->GetHeaders();
		/*
		for (std::multimap<std::string, std::string>::iterator itr = requestHeaders.begin(); itr != requestHeaders.end(); ++itr) {

			std::string strKey = itr->first;
			std::wstring wstrKey = util::StringToWideString(strKey);
			std::string strValue = itr->second;
			std::wstring wstrValue = util::StringToWideString(strValue);
			
			wstrRequestHeaders.insert(std::pair<std::wstring, std::wstring>(wstrKey, wstrValue));
		}
		webRequest.SetRequestHeaders(wstrRequestHeaders);
		*/

		if (!requestHeaders.empty()) {
			m_headermap[strEnvironmentAssetURL] = requestHeaders;
		}	
		//*/
		
		LoadRequest(webRequest);
		//SetURI(strEnvironmentAssetURL);
		m_currentEnvironmentAssetID = pEnvironmentAsset->GetAssetID();
	}

Error:
	return r;
}

RESULT DreamBrowser::SetURI(std::string strURI) {
	RESULT r = R_PASS;

	CN(m_pWebBrowserController);
	CR(m_pWebBrowserController->LoadURL(strURI));

Error:
	return r;
}

RESULT DreamBrowser::LoadRequest(const WebRequest &webRequest) {
	RESULT r = R_PASS;

	CN(m_pWebBrowserController);
	CR(m_pWebBrowserController->LoadRequest(webRequest));

Error:
	return r;
}

DreamBrowser* DreamBrowser::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamBrowser *pDreamApp = new DreamBrowser(pDreamOS, pContext);
	return pDreamApp;
}


RESULT DreamBrowser::SetScrollFactor(int scrollFactor) {
	m_scrollFactor = scrollFactor;
	return R_PASS;
}
