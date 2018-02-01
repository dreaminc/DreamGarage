#include "DreamBrowser.h"
#include "DreamControlView/DreamControlView.h"
#include "DreamShareView/DreamShareView.h"
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

RESULT DreamBrowserHandle::SetScope(std::string strScope) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(SetBrowserScope(strScope));
Error:
	return r;
}

RESULT DreamBrowserHandle::ScrollTo(int pxXScroll, int pxYScroll) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(ScrollBrowserToPoint(pxXScroll, pxYScroll));
Error: 
	return r;
}

RESULT DreamBrowserHandle::ScrollToX(int pxXScroll) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(ScrollBrowserToX(pxXScroll));
Error:
	return r;
}

RESULT DreamBrowserHandle::ScrollToY(int pyYScroll) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(ScrollBrowserToY(pyYScroll));
Error:
	return r;
}

RESULT DreamBrowserHandle::ScrollByDiff(int pxXDiff, int pxYDiff, WebBrowserPoint scrollPoint) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(ScrollBrowserByDiff(pxXDiff, pxYDiff, scrollPoint));
Error:
	return r;
}

RESULT DreamBrowserHandle::ScrollXByDiff(int pxXDiff) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(ScrollBrowserXByDiff(pxXDiff));
Error:
	return r;
}

RESULT DreamBrowserHandle::ScrollYByDiff(int pxYDiff) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(ScrollBrowserYByDiff(pxYDiff));
Error:
	return r;
}

RESULT DreamBrowserHandle::SetPath(std::string strPath) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(SetBrowserPath(strPath));

Error:
	return r;
}

RESULT DreamBrowserHandle::SendKeyCharacter(char chKey, bool fkeyDown) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(SendKeyPressed(chKey, fkeyDown));
Error:
	return r;
}

RESULT DreamBrowserHandle::SendURI(std::string strURI) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(SetURI(strURI));
Error:
	return r;
}

RESULT DreamBrowserHandle::SendMalletMoveEvent(WebBrowserPoint mousePoint) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(SendMouseMoveEvent(mousePoint));
Error:
	return r;
}

RESULT DreamBrowserHandle::SendBackEvent() {
	RESULT r = R_PASS;
	CB(GetAppState());
	return HandleBackEvent();
Error:
	return r;
}

RESULT DreamBrowserHandle::SendForwardEvent() {
	RESULT r = R_PASS;
	CB(GetAppState());
	return HandleForwardEvent();
Error:
	return r;
}

RESULT DreamBrowserHandle::SendStopEvent() {
	RESULT r = R_PASS;
	CB(GetAppState());
	return HandleStopEvent();
Error:
	return r;
}

RESULT DreamBrowserHandle::SendContactToBrowserAtPoint(WebBrowserPoint ptContact, bool fMouseDown) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(ClickBrowser(ptContact, fMouseDown));
Error:
	return r;
}

int DreamBrowserHandle::GetScrollPixelsX() {
	RESULT r = R_PASS;
	CB(GetAppState());
	return GetScrollX();
Error:
	return -1;
}

int DreamBrowserHandle::GetScrollPixelsY() {
	RESULT r = R_PASS;
	CB(GetAppState());
	return GetScrollY();
Error:
	return -1;
}

// TODO bring through CEF
int DreamBrowserHandle::GetPageHeightFromBrowser() {	
	RESULT r = R_PASS;
	CB(GetAppState());
	return GetPageHeight();
Error:
	return -1;
}

int DreamBrowserHandle::GetPageWidthFromBrowser() {
	RESULT r = R_PASS;
	CB(GetAppState());
	return GetPageWidth();
Error:
	return -1;
}

int DreamBrowserHandle::GetWidthOfBrowser() {
	RESULT r = R_PASS;
	CB(GetAppState());
	return GetBrowserWidth();
Error:
	return -1;
}

int DreamBrowserHandle::GetHeightOfBrowser() {
	RESULT r = R_PASS;
	CB(GetAppState());
	return GetBrowserHeight();
Error:
	return -1;
}

DreamBrowser::DreamBrowser(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamBrowser>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

DreamBrowser::~DreamBrowser(){
	RESULT r = R_PASS;

	CR(Shutdown());

Error:
	return;
}

RESULT DreamBrowser::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	if (m_pWebBrowserManager != nullptr) {
		CR(m_pWebBrowserManager->Shutdown());
		//m_pWebBrowserManager = nullptr;
	}

Error:
	return r;
}

DreamAppHandle* DreamBrowser::GetAppHandle() {
	return (DreamBrowserHandle*)(this);
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

RESULT DreamBrowser::ScrollBrowserByDiff(int pxXDiff, int pxYDiff, WebBrowserPoint scrollPoint) {
	RESULT r = R_PASS;
	
	CNR(m_pWebBrowserController, R_SKIPPED);

	WebBrowserMouseEvent mouseEvent;
	mouseEvent.pt = scrollPoint;

	m_pxXPosition += pxXDiff;
	m_pxYPosition += pxYDiff;

	CR(m_pWebBrowserController->SendMouseWheel(mouseEvent, pxXDiff, pxYDiff));

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

RESULT DreamBrowser::SendKeyPressed(char chKey, bool fkeyDown) {
	RESULT r = R_PASS;
	CNR(m_pWebBrowserController, R_SKIPPED);
	CR(m_pWebBrowserController->SendKeyEventChar(chKey, fkeyDown));
Error:
	return r;
}

RESULT DreamBrowser::SendURL(std::string strURL) {
	RESULT r = R_PASS;

//	SetVisible(true);

	std::string strTitle = "website";
	SetBrowserPath(strURL);
	auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");

	CRM(m_pEnvironmentControllerProxy->RequestShareAsset(m_strScope, m_strPath, strTitle), "Failed to share environment asset");

Error:
	return r;
}

RESULT DreamBrowser::SendMouseMoveEvent(WebBrowserPoint mousePoint) {
	RESULT r = R_PASS;

	WebBrowserMouseEvent mouseEvent;
	mouseEvent.pt.x = mousePoint.x;
	mouseEvent.pt.y = mousePoint.y;

	CR(m_pWebBrowserController->SendMouseMove(mouseEvent));
Error:
	return r;
}

RESULT DreamBrowser::ClickBrowser(WebBrowserPoint ptContact, bool fMouseDown) {
	RESULT r = R_PASS;

	CNR(m_pWebBrowserController, R_SKIPPED);
	WebBrowserMouseEvent mouseEvent;

	mouseEvent.pt = ptContact;
	m_lastWebBrowserPoint = ptContact;

	mouseEvent.mouseButton = WebBrowserMouseEvent::MOUSE_BUTTON::LEFT;
	
	CR(m_pWebBrowserController->SendMouseClick(mouseEvent, fMouseDown, 1));		// mouse down

Error:
	return r;
}

RESULT DreamBrowser::OnLoadingStateChange(bool fLoading, bool fCanGoBack, bool fCanGoForward, std::string strCurrentURL) {
	RESULT r = R_PASS;

	DreamControlViewHandle *pDreamControlViewHandle = nullptr;

	if (!fLoading) {
		m_strCurrentURL = strCurrentURL;
		pDreamControlViewHandle = dynamic_cast<DreamControlViewHandle*>(GetDOS()->RequestCaptureAppUnique("DreamControlView", this));

		CN(pDreamControlViewHandle);

		//pDreamControlViewHandle->SetControlViewTexture(m_pBrowserTexture);
		if (m_strCurrentURL != "") {
			pDreamControlViewHandle->SendURLText(m_strCurrentURL);
		}

	}

Error:
	if (pDreamControlViewHandle != nullptr) {
		GetDOS()->RequestReleaseAppUnique(pDreamControlViewHandle, this);
	}
	return r;
}

RESULT DreamBrowser::OnLoadStart() {
	RESULT r = R_PASS;	
	
	if (m_fShouldBeginStream) {
		//CR(BeginStream());
	} 
	else {
		m_fShouldBeginStream = true;
	}

//Error:
	return r;
}

RESULT DreamBrowser::OnLoadEnd(int httpStatusCode, std::string strCurrentURL) {
	RESULT r = R_PASS;

	DreamControlViewHandle *pDreamControlViewHandle = nullptr;
	DreamShareViewHandle *pDreamShareViewHandle = nullptr;

	m_strCurrentURL = strCurrentURL;
	pDreamControlViewHandle = dynamic_cast<DreamControlViewHandle*>(GetDOS()->RequestCaptureAppUnique("DreamControlView", this));
	pDreamShareViewHandle = dynamic_cast<DreamShareViewHandle*>(GetDOS()->RequestCaptureAppUnique("DreamShareView", this));

	if (pDreamControlViewHandle != nullptr) {
		pDreamControlViewHandle->SetControlViewTexture(m_pBrowserTexture);
	}

	if (pDreamShareViewHandle != nullptr && strCurrentURL != "about:blank") {
		pDreamShareViewHandle->SendCastTexture(m_pBrowserTexture);
		pDreamShareViewHandle->SendCastingEvent();
	}

//Error:
	if (pDreamControlViewHandle != nullptr) {
		GetDOS()->RequestReleaseAppUnique(pDreamControlViewHandle, this);
	}

	if (pDreamShareViewHandle != nullptr) {
		GetDOS()->RequestReleaseAppUnique(pDreamShareViewHandle, this);
	}
	return r;
}

RESULT DreamBrowser::OnNodeFocusChanged(DOMNode *pDOMNode) {
	RESULT r = R_PASS;

	DreamControlViewHandle *pDreamControlViewHandle = nullptr;
	bool fMaskPasswordEnabled = false;
	if (pDOMNode->GetType() == DOMNode::type::ELEMENT && pDOMNode->IsEditable()) {
		pDreamControlViewHandle = dynamic_cast<DreamControlViewHandle*>(GetDOS()->RequestCaptureAppUnique("DreamControlView", this));
		CN(pDreamControlViewHandle);

		if (pDreamControlViewHandle->IsAppVisible()) {
			std::string strTextField = pDOMNode->GetValue();
			point ptTextBox = point(0.0f, m_lastWebBrowserPoint.y, 0.0f);
			CR(pDreamControlViewHandle->HandleKeyboardUp(strTextField, ptTextBox));
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

#ifdef _USE_TEST_APP
	if (pDOMNode->GetType() == DOMNode::type::ELEMENT && pDOMNode->IsEditable()) {
		DEBUG_LINEOUT("editable!");
		m_pPointerCursor->SetVisible(false);
	}
	else {
		DEBUG_LINEOUT("non editable!");
		m_pPointerCursor->SetVisible(true);
	}
#endif

Error:
	if (pDreamControlViewHandle != nullptr) {
		GetDOS()->RequestReleaseAppUnique(pDreamControlViewHandle, this);
	}
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

	// Set up browser manager
	m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
	CN(m_pWebBrowserManager);
	CR(m_pWebBrowserManager->Initialize());
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
	m_pLoadingScreenTexture = GetComposite()->MakeTexture(L"client-loading-1366-768.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	CN(m_pLoadingScreenTexture);

#ifndef _USE_TEST_APP
	//m_pBrowserQuad->SetDiffuseTexture(m_pLoadingScreenTexture.get());
#else
	// Initialize new browser
	m_pWebBrowserController = m_pWebBrowserManager->CreateNewBrowser(pxWidth, pxHeight, strURL);
	CN(m_pWebBrowserController);
	CR(m_pWebBrowserController->RegisterWebBrowserControllerObserver(this));
#endif

	// Set up mouse / hand cursor model
	///*
	GetComposite()->InitializeOBB();
	
#ifdef _USE_TEST_APP
	// Test code
	
	/*
	m_pTestSphereAbsolute = GetDOS()->AddSphere(0.025f, 10, 10);
	m_pTestSphereAbsolute->SetColor(COLOR_RED);
	
	m_pTestSphereRelative = GetComposite()->AddSphere(0.025f, 10, 10);
	m_pTestSphereRelative->SetColor(COLOR_RED);
	//*/

	m_pTestQuad = GetComposite()->AddQuad(1.0f, 1.0f, 1, 1, nullptr, vector::kVector(1.0f));
	CN(m_pTestQuad);
	m_pTestQuad->translateX(GetWidth() + 0.5f + 0.1f);

	m_pPointerCursor = GetComposite()->AddModel(L"\\mouse-cursor\\mouse-cursor.obj");
	CN(m_pPointerCursor);

	m_pPointerCursor->SetPivotPoint(point(-0.2f, -0.43f, 0.0f));
	m_pPointerCursor->SetScale(0.01f);
	m_pPointerCursor->SetOrientationOffset(vector((float)M_PI_2, 0.0f, 0.0f));
	m_pPointerCursor->SetMaterialAmbient(1.0f);
	m_pPointerCursor->SetVisible(false);

	GetDOS()->AddObjectToInteractionGraph(GetComposite());
#endif

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
	DreamControlViewHandle *pDreamControlViewHandle = nullptr;
	DreamShareViewHandle *pDreamShareViewHandle = nullptr;

	if (m_pWebBrowserManager != nullptr) {
		CR(m_pWebBrowserManager->Update());
	}
	else {
		SetVisible(false);
	}

	if (m_pDreamUserHandle == nullptr) {
		auto pDreamOS = GetDOS();
		CNR(pDreamOS, R_OBJECT_NOT_FOUND);
		auto userAppIDs = pDreamOS->GetAppUID("DreamUserApp");
		CBR(userAppIDs.size() == 1, R_OBJECT_NOT_FOUND);
		m_pDreamUserHandle = dynamic_cast<DreamUserApp*>(pDreamOS->CaptureApp(userAppIDs[0], this));
	}
	//*
	if (m_fShowControlView) {
		pDreamControlViewHandle = dynamic_cast<DreamControlViewHandle*>(GetDOS()->RequestCaptureAppUnique("DreamControlView", this));

		if (pDreamControlViewHandle != nullptr) {
			CR(pDreamControlViewHandle->ShowApp());
			pDreamControlViewHandle->SendContentType(m_strContentType);
			m_fShowControlView = false;
		}

		pDreamShareViewHandle = dynamic_cast<DreamShareViewHandle*>(GetDOS()->RequestCaptureAppUnique("DreamShareView", this));

		if (pDreamShareViewHandle != nullptr) {
			CR(pDreamShareViewHandle->SendShowEvent());
			CR(pDreamShareViewHandle->SendLoadingEvent());
		}

		GetComposite()->SetVisible(true);

	}
	//*/
Error:
	if (pDreamControlViewHandle != nullptr) {
		GetDOS()->RequestReleaseAppUnique(pDreamControlViewHandle, this);
	}

	if (pDreamShareViewHandle != nullptr) {
		GetDOS()->RequestReleaseAppUnique(pDreamShareViewHandle, this);
	}
	return r;
}

// TODO: Only update the rect
// TODO: Turn off CEF when we're not using it
RESULT DreamBrowser::OnPaint(const WebBrowserRect &rect, const void *pBuffer, int width, int height) {
	RESULT r = R_PASS;

//	if (m_fReceivingStream == false) {
		CN(m_pBrowserTexture);

		// Update texture dimensions if needed
		CR(m_pBrowserTexture->UpdateDimensions(width, height));
		if (r != R_NOT_HANDLED) {
			DEBUG_LINEOUT("Changed chrome texture dimensions");
		}

		CR(m_pBrowserTexture->Update((unsigned char*)(pBuffer), width, height, PIXEL_FORMAT::BGRA));

		/*
		if (IsStreaming()) {
			CR(GetDOS()->GetCloudController()->BroadcastVideoFrame((unsigned char*)(pBuffer), width, height, 4));
		}
		//*/
//	}

Error:
	return r;
}

RESULT DreamBrowser::OnAudioPacket(const AudioPacket &pendingAudioPacket) {
	RESULT r = R_PASS;

	// TODO: Handle this (if streaming we broadcast into webrtc
	/*
	if (m_fStreaming) {
		CR(GetDOS()->GetCloudController()->BroadcastAudioPacket(kChromeAudioLabel, pendingAudioPacket));
	}
	//*/

//Error:
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

RESULT DreamBrowser::HandleTestQuadInteractionEvents(InteractionObjectEvent *pEvent) {
	RESULT r = R_PASS;
	/*
	switch (pEvent->m_eventType) {
		case ELEMENT_INTERSECT_BEGAN: {
			m_fTestQuadActive = true;
		} break;

		case ELEMENT_INTERSECT_MOVED: {
			// empty
		} break;

		case ELEMENT_INTERSECT_ENDED: {
			m_fTestQuadActive = false;
		} break;

		case INTERACTION_EVENT_SELECT_DOWN: {

			if (m_fReceivingStream) {
				CR(GetDOS()->UnregisterVideoStreamSubscriber(this));
				m_fReceivingStream = false;
			}

			SetStreamingState(false);

			// TODO: May not be needed, if not streaming no video is actually being transmitted 
			// so unless we want to set up a WebRTC re-negotiation this is not needed anymore
			//CR(GetDOS()->GetCloudController()->StartVideoStreaming(m_browserWidth, m_browserHeight, 30, PIXEL_FORMAT::BGRA));

			//CR(BroadcastDreamBrowserMessage(DreamShareViewMessage::type::PING));
			CR(BroadcastDreamBrowserMessage(DreamShareViewMessage::type::REQUEST_STREAMING_START));

			SetStreamingState(true);

		} break;
	}
	//*/
	CR(r);

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

float DreamBrowser::GetHeight() {
	return std::sqrt((m_diagonalSize * m_diagonalSize) / (1.0f + (m_aspectRatio * m_aspectRatio)));
}

float DreamBrowser::GetWidth() {
	return std::sqrt(((m_aspectRatio * m_aspectRatio) * (m_diagonalSize * m_diagonalSize)) / (1.0f + (m_aspectRatio * m_aspectRatio)));
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

RESULT DreamBrowser::SetBrowserScope(std::string strScope) {
	m_strScope = strScope;
	return R_PASS;
}

RESULT DreamBrowser::SetBrowserPath(std::string strPath) {
	m_strPath = strPath;
	return R_PASS;
}

RESULT DreamBrowser::SetEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;

	DreamControlViewHandle *pDreamControlViewHandle = nullptr;
	
	if (m_pWebBrowserController == nullptr) {
		m_pWebBrowserController = m_pWebBrowserManager->CreateNewBrowser(m_browserWidth, m_browserHeight, pEnvironmentAsset->GetURL());
		CN(m_pWebBrowserController);
		CR(m_pWebBrowserController->RegisterWebBrowserControllerObserver(this));
		/*
		m_fShowControlView = true;

		pDreamControlViewHandle = dynamic_cast<DreamControlViewHandle*>(GetDOS()->RequestCaptureAppUnique("DreamControlView", this));
		CN(pDreamControlViewHandle);
		CR(m_pDreamUserHandle->SendPushFocusStack(pDreamControlViewHandle));
		m_pDreamUserHandle->SendPreserveSharingState(false);	
		*/
	}

	m_fShowControlView = true;

	pDreamControlViewHandle = dynamic_cast<DreamControlViewHandle*>(GetDOS()->RequestCaptureAppUnique("DreamControlView", this));
	CN(pDreamControlViewHandle);
	CR(m_pDreamUserHandle->SendPushFocusStack(pDreamControlViewHandle));
	m_pDreamUserHandle->SendPreserveSharingState(false);

	if (pEnvironmentAsset != nullptr) {
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

		UserControllerProxy *pUserControllerProxy = (UserControllerProxy*)GetDOS()->GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE::USER);
		CN(pUserControllerProxy);

		std::multimap<std::wstring, std::wstring> wstrRequestHeaders;
		std::multimap<std::string, std::string> requestHeaders = pEnvironmentAsset->GetHeaders();
		
		for (std::multimap<std::string, std::string>::iterator itr = requestHeaders.begin(); itr != requestHeaders.end(); ++itr) {

			std::string strKey = itr->first;
			std::wstring wstrKey = util::StringToWideString(strKey);
			std::string strValue = itr->second;
			std::wstring wstrValue = util::StringToWideString(strValue);
			
			wstrRequestHeaders.insert(std::pair<std::wstring, std::wstring>(wstrKey, wstrValue));
		}
		
		webRequest.SetRequestHeaders(wstrRequestHeaders);
		
		LoadRequest(webRequest);
		m_currentEnvironmentAssetID = pEnvironmentAsset->GetAssetID();
	}

Error:
	if (pDreamControlViewHandle != nullptr) {
		GetDOS()->RequestReleaseAppUnique(pDreamControlViewHandle, this);
	}
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
