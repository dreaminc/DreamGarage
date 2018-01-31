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

float DreamBrowserHandle::GetAspectRatioFromBrowser() {
	RESULT r = R_PASS;
	CB(GetAppState());
	return GetAspectRatio();
Error:
	return -1;
}

RESULT DreamBrowserHandle::RequestBeginStream() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(BeginStream());
Error:
	return r;
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

RESULT DreamBrowser::FadeQuadToBlack() {
	RESULT r = R_PASS;

	//Fade to black
	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		//m_pBrowserQuad->SetVisible(false);
		return r;
	};

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pBrowserQuad.get(),
		color(0.0f, 0.0f, 0.0f, 1.0f),
		0.1f,
		AnimationCurveType::LINEAR,
		AnimationFlags(),
		nullptr,
		fnEndCallback,
		this
	));

Error:
	return r;
}

RESULT DreamBrowser::OnLoadStart() {
	RESULT r = R_PASS;	
	
	if (m_fShouldBeginStream) {
		CR(BeginStream());
	} 
	else {
		m_fShouldBeginStream = true;
	}

Error:
	return r;
}

RESULT DreamBrowser::OnLoadEnd(int httpStatusCode, std::string strCurrentURL) {
	RESULT r = R_PASS;

	DreamControlViewHandle *pDreamControlViewHandle = nullptr;
	DreamShareViewHandle *pDreamShareViewHandle = nullptr;

	if (strCurrentURL != "about:blank") {
		m_pBrowserQuad->SetDiffuseTexture(m_pBrowserTexture.get());
	}

	m_strCurrentURL = strCurrentURL;
	pDreamControlViewHandle = dynamic_cast<DreamControlViewHandle*>(GetDOS()->RequestCaptureAppUnique("DreamControlView", this));
	pDreamShareViewHandle = dynamic_cast<DreamShareViewHandle*>(GetDOS()->RequestCaptureAppUnique("DreamShareView", this));

	if (pDreamControlViewHandle != nullptr) {
		pDreamControlViewHandle->SetControlViewTexture(m_pBrowserTexture);
	}

	if (pDreamShareViewHandle != nullptr) {
		pDreamShareViewHandle->SendCastTexture(m_pBrowserTexture);
		pDreamShareViewHandle->SendCastingEvent();
	}

#ifndef _USE_TEST_APP
	m_pBrowserQuad->SetDiffuseTexture(m_pBrowserTexture.get());
#endif

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

	auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");

	CR(m_pEnvironmentControllerProxy->RequestStopSharing(m_currentEnvironmentAssetID, m_strScope, m_strPath));
	CR(SetStreamingState(false));

Error:
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

	SetAppName(DREAM_BROWSER_APP_NAME);
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
	m_pBrowserQuad = GetComposite()->AddQuad(GetWidth(), GetHeight(), 1, 1, nullptr, GetNormal());
	CN(m_pBrowserQuad);

	
	// Flip UV vertically
	///*
	m_pBrowserQuad->FlipUVVertical();
	//*/

	// attempt to give the browser a matte appearance
	m_pBrowserQuad->SetMaterialAmbient(0.9f);

	GetComposite()->SetMaterialShininess(0.0f, true);
	GetComposite()->SetMaterialSpecularColor(color(0.0f, 0.0f, 0.0f, 1.0f), true);


	// Set up and map the texture
	m_pBrowserTexture = GetComposite()->MakeTexture(texture::TEXTURE_TYPE::TEXTURE_DIFFUSE, pxWidth, pxHeight, PIXEL_FORMAT::RGBA, 4, &vectorByteBuffer[0], pxWidth * pxHeight * 4);	
	m_pLoadingScreenTexture = GetComposite()->MakeTexture(L"client-loading-1366-768.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	CN(m_pLoadingScreenTexture);

#ifndef _USE_TEST_APP
	m_pBrowserQuad->SetDiffuseTexture(m_pLoadingScreenTexture.get());
#else
	// Initialize new browser
	m_pWebBrowserController = m_pWebBrowserManager->CreateNewBrowser(pxWidth, pxHeight, strURL);
	CN(m_pWebBrowserController);
	CR(m_pWebBrowserController->RegisterWebBrowserControllerObserver(this));
	m_pBrowserQuad->SetDiffuseTexture(m_pBrowserTexture.get());
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

	CR(GetDOS()->RegisterEventSubscriber(GetComposite(), ELEMENT_INTERSECT_BEGAN, this));
	CR(GetDOS()->RegisterEventSubscriber(GetComposite(), ELEMENT_INTERSECT_MOVED, this));
	CR(GetDOS()->RegisterEventSubscriber(GetComposite(), ELEMENT_INTERSECT_ENDED, this));

	// Mouse related
	CR(GetDOS()->RegisterEventSubscriber(GetComposite(), INTERACTION_EVENT_SELECT_DOWN, this));
	CR(GetDOS()->RegisterEventSubscriber(GetComposite(), INTERACTION_EVENT_SELECT_UP, this));
	CR(GetDOS()->RegisterEventSubscriber(GetComposite(), INTERACTION_EVENT_WHEEL, this));
#endif
	CR(GetDOS()->RegisterEventSubscriber(GetComposite(), INTERACTION_EVENT_KEY_DOWN, this));
	CR(GetDOS()->RegisterEventSubscriber(GetComposite(), INTERACTION_EVENT_KEY_UP, this));

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

	if (m_pWebBrowserManager != nullptr) {
		CR(m_pWebBrowserManager->Update());
	}
	else {
		SetVisible(false);
	}

	if (m_fReceivingStream && m_pendingFrame.fPending) {
		CRM(UpdateFromPendingVideoFrame(), "Failed to update pending frame");
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
		CN(pDreamControlViewHandle);

		CR(pDreamControlViewHandle->ShowApp());
		pDreamControlViewHandle->SendContentType(m_strContentType);
		m_fShowControlView = false;

		m_pBrowserQuad->SetDiffuseTexture(m_pLoadingScreenTexture.get());
		m_pBrowserQuad->SetVisible(true);

	}
	//*/
Error:
	if (pDreamControlViewHandle != nullptr) {
		GetDOS()->RequestReleaseAppUnique(pDreamControlViewHandle, this);
	}
	return r;
}

WebBrowserPoint DreamBrowser::GetRelativeBrowserPointFromContact(point ptIntersectionContact) {
	WebBrowserPoint webPt;

	ptIntersectionContact.w() = 1.0f;

	// First apply transforms to the ptIntersectionContact 
	point ptAdjustedContact = inverse(m_pBrowserQuad->GetModelMatrix()) * ptIntersectionContact;
	
	//m_pTestSphereRelative->SetPosition(ptAdjustedContact);

	float width = GetWidth();
	float height = GetHeight();

	float posX = ptAdjustedContact.x();
	float posY = ptAdjustedContact.y();
	float posZ = ptAdjustedContact.z();

	// TODO: This is a bit of a hack, should be a better way (this won't account for the quad normal, only orientation
	// so it might get confused - technically this should never actually happen otherwise since we can force a dimension
	if (std::abs(posZ) > std::abs(posY)) {
		posY = posZ;
	}

	posX /= width / 2.0f;
	posY /= height / 2.0f;

	posX = (posX + 1.0f) / 2.0f;
	posY = (posY + 1.0f) / 2.0f;  // flip it

	// TODO: push into WebBrowserController
	webPt.x = posX * 1366;
	webPt.y = 768 - (posY * 768);

	//ptAdjustedContact.Print("adj");
	//DEBUG_LINEOUT("%d %d", webPt.x, webPt.y);

	return webPt;
}

// TODO: Only update the rect
// TODO: Turn off CEF when we're not using it
RESULT DreamBrowser::OnPaint(const WebBrowserRect &rect, const void *pBuffer, int width, int height) {
	RESULT r = R_PASS;

	if (m_fReceivingStream == false) {
		CN(m_pBrowserTexture);

		// Update texture dimensions if needed
		CR(m_pBrowserTexture->UpdateDimensions(width, height));
		if (r != R_NOT_HANDLED) {
			DEBUG_LINEOUT("Changed chrome texture dimensions");
		}

		CR(m_pBrowserTexture->Update((unsigned char*)(pBuffer), width, height, PIXEL_FORMAT::BGRA));

		if (IsStreaming()) {
			CR(GetDOS()->GetCloudController()->BroadcastVideoFrame((unsigned char*)(pBuffer), width, height, 4));
		}
	}

Error:
	return r;
}

RESULT DreamBrowser::OnAudioPacket(const AudioPacket &pendingAudioPacket) {
	RESULT r = R_PASS;

	// TODO: Handle this (if streaming we broadcast into webrtc
	if (m_fStreaming) {
		CR(GetDOS()->GetCloudController()->BroadcastAudioPacket(kChromeAudioLabel, pendingAudioPacket));
	}

Error:
	return r;
}

RESULT DreamBrowser::OnVideoFrame(PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	// TODO: Create a pending frame thing
	//CR(m_pBrowserTexture->Update((unsigned char*)(pVideoFrameDataBuffer), pxWidth, pxHeight, texture::PixelFormat::RGBA));

	if (m_fReceivingStream) {
		r = SetupPendingVideoFrame((unsigned char*)(pVideoFrameDataBuffer), pxWidth, pxHeight);

		if (r == R_OVERFLOW) {
			DEBUG_LINEOUT("Overflow frame!");
			return R_PASS;
		}

		CRM(r, "Failed for other reason");

		if (!IsVisible()) {
			SetVisible(true);
		}
	}

Error:
	return r;
}

RESULT DreamBrowser::SetupPendingVideoFrame(uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	// TODO: programmatic 
	int channels = 4;

	CBRM((m_pendingFrame.fPending == false), R_OVERFLOW, "Buffer already pending");

	m_pendingFrame.fPending = true;
	m_pendingFrame.pxWidth = pxWidth;
	m_pendingFrame.pxHeight = pxHeight;

	// Allocate
	// TODO: Might be able to avoid this if the video buffer is not changing size
	// and just keep the memory allocated instead
	m_pendingFrame.pDataBuffer_n = sizeof(uint8_t) * pxWidth * pxHeight * channels;
	//m_pendingFrame.pDataBuffer = (uint8_t*)malloc(m_pendingFrame.pDataBuffer_n);

	m_pendingFrame.pDataBuffer = pVideoFrameDataBuffer;

	CNM(m_pendingFrame.pDataBuffer, "Failed to allocate video buffer mem");

	// Copy
	//memcpy(m_pendingFrame.pDataBuffer, pVideoFrameDataBuffer, m_pendingFrame.pDataBuffer_n);

Error:
	return r;
}

RESULT DreamBrowser::UpdateFromPendingVideoFrame() {
	RESULT r = R_PASS;

	CBM(m_pendingFrame.fPending, "No frame pending");
	CNM(m_pendingFrame.pDataBuffer, "No data buffer");

	//DEBUG_LINEOUT("inframe %d x %d", m_pendingFrame.pxWidth, m_pendingFrame.pxHeight);

	// Update texture dimensions if needed
	CR(m_pBrowserTexture->UpdateDimensions(m_pendingFrame.pxWidth, m_pendingFrame.pxHeight));
	if (r != R_NOT_HANDLED) {
		DEBUG_LINEOUT("Changed texture dimensions");
	}

	CRM(m_pBrowserTexture->Update((unsigned char*)(m_pendingFrame.pDataBuffer), m_pendingFrame.pxWidth, m_pendingFrame.pxHeight, PIXEL_FORMAT::BGRA), "Failed to update texture from pending frame");

Error:
	if (m_pendingFrame.pDataBuffer != nullptr) {
		delete [] m_pendingFrame.pDataBuffer;
		m_pendingFrame.pDataBuffer = nullptr;

		memset(&m_pendingFrame, 0, sizeof(PendingFrame));
	}

	return r;
}

RESULT DreamBrowser::HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage) {
	RESULT r = R_PASS;

	DreamBrowserMessage *pDreamBrowserMessage = (DreamBrowserMessage*)(pDreamAppMessage);
	CN(pDreamBrowserMessage);

	//currently, only store the most recent message received
	m_currentMessageType = pDreamBrowserMessage->GetMessageType();
	m_currentAckType = pDreamBrowserMessage->GetAckType();

	switch (pDreamBrowserMessage->GetMessageType()) {
		case DreamBrowserMessage::type::PING: {
			CR(BroadcastDreamBrowserMessage(DreamBrowserMessage::type::ACK, DreamBrowserMessage::type::PING));
		} break;

		case DreamBrowserMessage::type::ACK: {
			switch (pDreamBrowserMessage->GetAckType()) {
				// We get a request streaming start ACK when we requested to start streaming
				// This will begin broadcasting
				case DreamBrowserMessage::type::REQUEST_STREAMING_START: {
					if (IsStreaming()) {
						// For non-changing stuff we need to send the current frame
						CR(GetDOS()->GetCloudController()->BroadcastTextureFrame(m_pBrowserTexture.get(), 0, PIXEL_FORMAT::BGRA));
					}

				} break;
			}
		} break;

		case DreamBrowserMessage::type::REQUEST_STREAMING_START: {
			CR(StartReceiving(pPeerConnection));
		} break;
	}

Error:
	return r;
}

RESULT DreamBrowser::BroadcastDreamBrowserMessage(DreamBrowserMessage::type msgType, DreamBrowserMessage::type ackType) {
	RESULT r = R_PASS;

	DreamBrowserMessage *pDreamBrowserMessage = new DreamBrowserMessage(0, 0, GetAppUID(), msgType, ackType);
	CN(pDreamBrowserMessage);

	CR(BroadcastDreamAppMessage(pDreamBrowserMessage));

Error:
	return r;
}

RESULT DreamBrowser::HandleTestQuadInteractionEvents(InteractionObjectEvent *pEvent) {
	RESULT r = R_PASS;

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

			//CR(BroadcastDreamBrowserMessage(DreamBrowserMessage::type::PING));
			CR(BroadcastDreamBrowserMessage(DreamBrowserMessage::type::REQUEST_STREAMING_START));

			SetStreamingState(true);

		} break;
	}

	CR(r);

Error:
	return r;
}

RESULT DreamBrowser::BeginStream() {
	RESULT r = R_PASS;

	if (m_fReceivingStream) {
		CR(GetDOS()->UnregisterVideoStreamSubscriber(this));
		m_fReceivingStream = false;
	}

	SetStreamingState(false);

	// TODO: May not be needed, if not streaming no video is actually being transmitted 
	// so unless we want to set up a WebRTC re-negotiation this is not needed anymore
	//CR(GetDOS()->GetCloudController()->StartVideoStreaming(m_browserWidth, m_browserHeight, 30, PIXEL_FORMAT::BGRA));

	//CR(BroadcastDreamBrowserMessage(DreamBrowserMessage::type::PING));
	CR(BroadcastDreamBrowserMessage(DreamBrowserMessage::type::REQUEST_STREAMING_START));
	
	// This is probably redundant!!!
	CR(GetDOS()->GetCloudController()->BroadcastTextureFrame(m_pBrowserTexture.get(), 0, PIXEL_FORMAT::BGRA));

	SetStreamingState(true);

Error:
	return r;
}

RESULT DreamBrowser::SetStreamingState(bool fStreaming) {
	RESULT r = R_PASS;

	m_fStreaming = fStreaming;

	CNR(m_pDreamUserHandle, R_SKIPPED);
	m_pDreamUserHandle->SendStreamingState(fStreaming);

Error:
	return r;
}

bool DreamBrowser::IsStreaming() {
	return m_fStreaming;
}

// InteractionObjectEvent
// Note that all of this will only occur if we're in testing mode
RESULT DreamBrowser::Notify(InteractionObjectEvent *pEvent) {
	RESULT r = R_PASS;

#ifdef _USE_TEST_APP
	bool fUpdateMouse = false;

	//m_pPointerCursor->SetPosition(pEvent->m_ptContact[0]);

	if (pEvent->m_pObject == m_pTestQuad.get() || m_fTestQuadActive) {
		return HandleTestQuadInteractionEvents(pEvent);
	}
#endif

	switch (pEvent->m_eventType) {
#ifdef _USE_TEST_APP
		case ELEMENT_INTERSECT_BEGAN: {
			if (m_pBrowserQuad->IsVisible()) {
				m_pPointerCursor->SetVisible(true);

				WebBrowserMouseEvent webBrowserMouseEvent;

				webBrowserMouseEvent.pt = GetRelativeBrowserPointFromContact(pEvent->m_ptContact[0]);

				CR(m_pWebBrowserController->SendMouseMove(webBrowserMouseEvent, false));

				m_lastWebBrowserPoint = webBrowserMouseEvent.pt;
				m_fBrowserActive = true;

				fUpdateMouse = true;
			}
		} break;

		case ELEMENT_INTERSECT_ENDED: {
			m_pPointerCursor->SetVisible(false);

			WebBrowserMouseEvent webBrowserMouseEvent;

			webBrowserMouseEvent.pt = GetRelativeBrowserPointFromContact(pEvent->m_ptContact[0]);

			CR(m_pWebBrowserController->SendMouseMove(webBrowserMouseEvent, true));

			m_lastWebBrowserPoint = webBrowserMouseEvent.pt;
			m_fBrowserActive = false;

			fUpdateMouse = true;
		} break;

		case ELEMENT_INTERSECT_MOVED: {
			WebBrowserMouseEvent webBrowserMouseEvent;

			webBrowserMouseEvent.pt = GetRelativeBrowserPointFromContact(pEvent->m_ptContact[0]);

			CR(m_pWebBrowserController->SendMouseMove(webBrowserMouseEvent, false));

			m_lastWebBrowserPoint = webBrowserMouseEvent.pt;

			fUpdateMouse = true;
		} break;
#endif

		case INTERACTION_EVENT_SELECT_UP: {
			WebBrowserMouseEvent webBrowserMouseEvent;

			bool fMouseUp = (pEvent->m_eventType == INTERACTION_EVENT_SELECT_UP);

			webBrowserMouseEvent.pt = m_lastWebBrowserPoint;
			webBrowserMouseEvent.mouseButton = WebBrowserMouseEvent::MOUSE_BUTTON::LEFT;

			CR(m_pWebBrowserController->SendMouseClick(webBrowserMouseEvent, fMouseUp, 1));

			m_lastWebBrowserPoint = webBrowserMouseEvent.pt;

			//// Determine focused node
			//CR(m_pWebBrowserController->GetFocusedNode());

		} break;

		case INTERACTION_EVENT_SELECT_DOWN: {
			WebBrowserMouseEvent webBrowserMouseEvent;

			bool fMouseUp = (pEvent->m_eventType == INTERACTION_EVENT_SELECT_UP);

			webBrowserMouseEvent.pt = m_lastWebBrowserPoint;
			webBrowserMouseEvent.mouseButton = WebBrowserMouseEvent::MOUSE_BUTTON::LEFT;

			CR(m_pWebBrowserController->SendMouseClick(webBrowserMouseEvent, fMouseUp, 1));

			m_lastWebBrowserPoint = webBrowserMouseEvent.pt;
		} break;

		case INTERACTION_EVENT_WHEEL: {
			WebBrowserMouseEvent webBrowserMouseEvent;

			webBrowserMouseEvent.pt = m_lastWebBrowserPoint;
			
			int deltaX = 0;
			int deltaY = pEvent->m_value * m_scrollFactor;

			CR(m_pWebBrowserController->SendMouseWheel(webBrowserMouseEvent, deltaX, deltaY));

			m_lastWebBrowserPoint = webBrowserMouseEvent.pt;
		} break;

		// Keyboard
		// TODO: Should be a "typing manager" in between?
		// TODO: haven't seen any issues with KEY_UP being a no-op
		case INTERACTION_EVENT_KEY_UP: break;
		case INTERACTION_EVENT_KEY_DOWN: {

#ifdef _USE_TEST_APP
			if ((pEvent->m_eventType == INTERACTION_EVENT_KEY_DOWN) && (pEvent->m_value == SVK_RETURN)) {
				if (m_fReceivingStream) {
					CR(GetDOS()->UnregisterVideoStreamSubscriber(this));
					m_fReceivingStream = false;
				}

				SetStreamingState(false);

				// TODO: May not be needed, if not streaming no video is actually being transmitted 
				// so unless we want to set up a WebRTC re-negotiation this is not needed anymore
				//CR(GetDOS()->GetCloudController()->StartVideoStreaming(m_browserWidth, m_browserHeight, 30, PIXEL_FORMAT::BGRA));

				//CR(BroadcastDreamBrowserMessage(DreamBrowserMessage::type::PING));
				CR(BroadcastDreamBrowserMessage(DreamBrowserMessage::type::REQUEST_STREAMING_START));

				SetStreamingState(true);
			}
#endif

			/*
			bool fKeyDown = (pEvent->m_eventType == INTERACTION_EVENT_KEY_DOWN);
			std::string strURL = "";

			char chKey = (char)(pEvent->m_value);
			m_strEntered.UpdateString(chKey);
			
			if (pEvent->m_value == SVK_RETURN) {
				SetVisible(true);

				std::string strScope = m_strScope;
				std::string strTitle = "website";
				std::string strPath = strURL;
				auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
				CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");

				CRM(m_pEnvironmentControllerProxy->RequestShareAsset(m_strScope, strPath, strTitle), "Failed to share environment asset");
			}
			//*/

			//CR(m_pWebBrowserController->SendKeyEventChar(chKey, fKeyDown));

		} break;
	}
	
#ifdef _USE_TEST_APP
	// First point of contact
	if (fUpdateMouse) {
		//if (pEvent->m_ptContact[0] != GetDOS()->GetInteractionEngineProxy()->GetInteractionRayOrigin()) {
			//m_pPointerCursor->SetOrigin(pEvent->m_ptContact[0]);
			point ptIntersectionContact = pEvent->m_ptContact[0];
			ptIntersectionContact.w() = 1.0f;

			point ptAdjustedContact = inverse(m_pBrowserQuad->GetModelMatrix()) * ptIntersectionContact;
			m_pPointerCursor->SetOrigin(ptAdjustedContact);
		//}
	}
#endif 

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

	if (m_pBrowserQuad != nullptr) {
		return UpdateViewQuad();
	}

	return R_PASS;
}

RESULT DreamBrowser::SetParams(point ptPosition, float diagonal, float aspectRatio, vector vNormal) {
	GetComposite()->SetPosition(ptPosition);
	m_diagonalSize = diagonal;
	m_aspectRatio = aspectRatio;
	m_vNormal = vNormal.Normal();

if (m_pBrowserQuad != nullptr) {
	return UpdateViewQuad();
}

return R_PASS;
}

float DreamBrowser::GetAspectRatio() {
	return m_aspectRatio;
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

RESULT DreamBrowser::UpdateViewQuad() {
	RESULT r = R_PASS;

	CR(m_pBrowserQuad->UpdateParams(GetWidth(), GetHeight(), GetNormal()));

	// Flip UV vertically
	///*
	if (r != R_SKIPPED) {
		m_pBrowserQuad->FlipUVVertical();
	}
	//*/

	CR(m_pBrowserQuad->SetDirty());

	CR(m_pBrowserQuad->InitializeBoundingQuad(point(0.0f, 0.0f, 0.0f), GetWidth(), GetHeight(), GetNormal()));

Error:
	return r;
}

bool DreamBrowser::IsVisible() {
	return m_pBrowserQuad->IsVisible();
}

RESULT DreamBrowser::SetVisible(bool fVisible) {
	RESULT r = R_PASS;
		
	CR(m_pBrowserQuad->SetVisible(fVisible));
	//CR(m_pPointerCursor->SetVisible(fVisible));
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

RESULT DreamBrowser::StopSending() {
	RESULT r = R_PASS;

	DreamControlViewHandle *pDreamControlViewHandle = nullptr;
	CR(SetStreamingState(false));

	pDreamControlViewHandle = dynamic_cast<DreamControlViewHandle*>(GetDOS()->RequestCaptureAppUnique("DreamControlView", this));

	if (pDreamControlViewHandle != nullptr) {
		pDreamControlViewHandle->HideApp();
		pDreamControlViewHandle->SetControlViewTexture(m_pLoadingScreenTexture);
		GetDOS()->RequestReleaseAppUnique(pDreamControlViewHandle, this);
		
		m_pDreamUserHandle->SendStopSharing();
	}

	m_pBrowserQuad->SetDiffuseTexture(m_pLoadingScreenTexture.get());
	//m_pWebBrowserController->CloseBrowser();
	//m_pWebBrowserController = nullptr;

	// don't stream on the next website load
	m_fShouldBeginStream = false; 
	CR(m_pWebBrowserController->LoadURL("about:blank"));
	CR(SetVisible(false));

Error:
	if (pDreamControlViewHandle != nullptr) {
		GetDOS()->RequestReleaseAppUnique(pDreamControlViewHandle, this);
	}
	return r;
}

RESULT DreamBrowser::StartReceiving(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	m_pDreamUserHandle->SendPreserveSharingState(false);
	m_pBrowserQuad->SetDiffuseTexture(m_pBrowserTexture.get());
	// Switch to input
	if (IsStreaming()) {
		SetStreamingState(false);

		// TODO: Turn off streamer etc
	}

	// Register for Video for the requester peer connection
	// (this buffers against multi-casts that are incorrect)
	if (GetDOS()->IsRegisteredVideoStreamSubscriber(this)) {
		CR(GetDOS()->UnregisterVideoStreamSubscriber(this));
	}

	/*
	// TODO: May not be needed, if not streaming no video is actually being transmitted
	// so unless we want to set up a WebRTC re-negotiation this is not needed anymore
	// Stop video streaming if we're streaming
	if (GetDOS()->GetCloudController()->IsVideoStreamingRunning()) {
		CR(GetDOS()->GetCloudController()->StopVideoStreaming());
	}
	//*/

	CR(GetDOS()->RegisterVideoStreamSubscriber(pPeerConnection, this));
	m_fReceivingStream = true;

	CR(BroadcastDreamBrowserMessage(DreamBrowserMessage::type::ACK, DreamBrowserMessage::type::REQUEST_STREAMING_START));

Error:
	return r;
}

RESULT DreamBrowser::PendReceiving() {
	RESULT r = R_PASS;
	m_fReceivingStream = true;
	//CR(SetVisible(true));

//Error:
	return r;
}

RESULT DreamBrowser::StopReceiving() {
	RESULT r = R_PASS;
	m_fReceivingStream = false;
	CR(SetVisible(false));
	CR(m_pBrowserQuad->SetDiffuseTexture(m_pLoadingScreenTexture.get()));

	//CR(	BroadcastDreamBrowserMessage(DreamBrowserMessage::type::ACK, 
	//								 DreamBrowserMessage::type::REPORT_STREAMING_STOP));
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

std::shared_ptr<texture> DreamBrowser::GetScreenTexture() {
	return m_pBrowserTexture;
}

//TODO: currently unused?
RESULT DreamBrowser::SetScreenTexture(texture *pTexture) {
	m_aspectRatio = (float)pTexture->GetWidth() / (float)pTexture->GetHeight();
	SetParams(GetOrigin(), m_diagonalSize, m_aspectRatio, m_vNormal);

	return m_pBrowserQuad->SetDiffuseTexture(pTexture);
}

DreamBrowser* DreamBrowser::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamBrowser *pDreamApp = new DreamBrowser(pDreamOS, pContext);
	return pDreamApp;
}


RESULT DreamBrowser::SetScrollFactor(int scrollFactor) {
	m_scrollFactor = scrollFactor;
	return R_PASS;
}
