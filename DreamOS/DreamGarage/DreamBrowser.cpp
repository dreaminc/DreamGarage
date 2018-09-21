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

#include "Core/Utilities.h"

#include "Sound/SoundBuffer.h"

#include <thread>

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

	m_soundState = sound::state::STOPPED;

	// Join thread
	if (m_browserAudioProcessingThread.joinable()) {
		m_browserAudioProcessingThread.join();
	}

	/*
	if (m_pWebBrowserManager != nullptr) {
		m_pWebBrowserManager->Shutdown();
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

	if (abs(pxXDiff) > abs(pxYDiff)) {
		pxYDiff = 0.0f;
	}
	else {
		pxXDiff = 0.0f;
	}

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
		
		CR(m_pWebBrowserController->IsInputFocused());

		//CR(PendUpdateObjectTextures());
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

	if (m_pObserver != nullptr) {
		//CR(PendUpdateObjectTextures());
	}

Error:
	return r;
}

RESULT DreamBrowser::OnNodeFocusChanged(DOMNode *pDOMNode) {
	RESULT r = R_PASS;

	UIKeyboardHandle *pKeyboardHandle = nullptr;

	if (m_pObserver != nullptr) {
		CR(m_pObserver->HandleNodeFocusChanged(pDOMNode, this));
	}

	if (pDOMNode->GetType() == DOMNode::type::ELEMENT && pDOMNode->IsEditable()) {
		if (m_pWebBrowserController != nullptr) {
			CR(m_pWebBrowserController->CanTabNext());
			CR(m_pWebBrowserController->CanTabPrevious());
		}
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

	if (GetDOS()->GetCloudController() != nullptr) {
		auto pUserController = dynamic_cast<UserController*>(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));

		if (pUserController != nullptr && pUserController->IsLoggedIn()) {
			// make a copy of strURL
			std::string strLowercaseURL = strURL;
			util::tolowerstring(strLowercaseURL);

			for (std::string& strAuthURL : m_authenticatedURLs) {
				if (strLowercaseURL.find(strAuthURL) == 0) {
					std::string strAccessToken = pUserController->GetSavedAccessToken();
					std::string strFirst = "Authorization";
					std::string strSecond = "Bearer " + strAccessToken;
					headermap.insert(std::pair<std::string, std::string>(strFirst, strSecond));
				}
			}
		}
	}
	

	return r;
}

RESULT DreamBrowser::HandleDreamFormSuccess() {
	RESULT r = R_PASS;

	CNR(m_pObserver, R_SKIPPED);
	CR(m_pObserver->HandleDreamFormSuccess());

Error:
	return r;
}

RESULT DreamBrowser::HandleDreamFormCancel() {
	RESULT r = R_PASS;

	CNR(m_pObserver, R_SKIPPED);
	CR(m_pObserver->HandleDreamFormCancel());

Error:
	return r;
}

RESULT DreamBrowser::HandleDreamFormSetCredentials(std::string& strRefreshToken, std::string& strAccessToken) {
	RESULT r = R_PASS;

	CNR(m_pObserver, R_SKIPPED);
	CR(m_pObserver->HandleDreamFormSetCredentials(strRefreshToken, strAccessToken));

Error:
	return r;
}

RESULT DreamBrowser::HandleDreamFormSetEnvironmentId(int environmentId) {
	RESULT r = R_PASS;

	CNR(m_pObserver, R_SKIPPED);
	CR(m_pObserver->HandleDreamFormSetEnvironmentId(environmentId));

Error:
	return r;
}

RESULT DreamBrowser::HandleIsInputFocused(bool fInputFocused) {
	RESULT r = R_PASS;

	CNR(m_pObserver, R_SKIPPED);

	CR(m_pObserver->HandleIsInputFocused(fInputFocused, this));

Error:
	return r;
}

RESULT DreamBrowser::HandleCanTabNext(bool fCanNext) {
	RESULT r = R_PASS;

	CNR(m_pObserver, R_SKIPPED);
	CR(m_pObserver->HandleCanTabNext(fCanNext));

Error:
	return r;

}

RESULT DreamBrowser::HandleCanTabPrevious(bool fCanPrevious) {
	RESULT r = R_PASS;

	CNR(m_pObserver, R_SKIPPED);
	CR(m_pObserver->HandleCanTabPrevious(fCanPrevious));

Error:
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

RESULT DreamBrowser::HandleTabEvent() {
	return m_pWebBrowserController->TabNext();
}

RESULT DreamBrowser::HandleBackTabEvent() {
	return m_pWebBrowserController->TabPrevious();
}

RESULT DreamBrowser::HandleUnfocusEvent() {
	return m_pWebBrowserController->UnfocusInput();
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
	m_pBrowserTexture = GetComposite()->MakeTexture(texture::type::TEXTURE_2D, pxWidth, pxHeight, PIXEL_FORMAT::RGBA, 4, &vectorByteBuffer[0], pxWidth * pxHeight * 4);
	m_pLoadingScreenTexture = GetComposite()->MakeTexture(texture::type::TEXTURE_2D, (wchar_t*)(L"client-loading-1366-768.png"));
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
	
	// Really strange, we need to send 4 frames for the share to go through? As in OnVideoFrame isn't called on the receiver side until the 4th one is sent
	if (m_fSendFrame && m_sentFrames < 4) {
		DOSLOG(INFO, "POLL FRAME");
		CR(m_pWebBrowserController->PollFrame());
		m_sentFrames++;
	}
	if (m_fSendFrame && m_sentFrames == 4) {	// reset
		m_fSendFrame = false;
		m_sentFrames = 0;
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
	m_pWebBrowserManager->UpdateJobProcesses();
	CN(m_pWebBrowserController);
	CR(m_pWebBrowserController->RegisterWebBrowserControllerObserver(this));

	// Set up the audio system (captures audio from browser, pushes it into a buffer)
	CRM(InitializeDreamBrowserSoundSystem(), "Failed to initialize sound system for browser");

Error:
	return r;
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

RESULT DreamBrowser::SendFirstFrame() {
	RESULT r = R_PASS;
	
	// This'll send an OnPaint which will refresh the frame and broadcast it
	m_fSendFrame = true;

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

	if (m_pObserver != nullptr) {
		CR(m_pObserver->UpdateContentSourceTexture(m_pBrowserTexture, this));
	}

	m_fUpdateObjectTextures = false;

Error:
	return r;
}

RESULT DreamBrowser::UpdateNavigationFlags() {
	RESULT r = R_PASS;

	bool fCanGoBack = m_pWebBrowserController->CanGoBack();
	bool fCanGoForward = m_pWebBrowserController->CanGoForward();

	if (m_pObserver != nullptr) {
		CR(m_pObserver->UpdateControlBarNavigation(fCanGoBack, fCanGoForward));
	}

Error:
	return r;
}

RESULT DreamBrowser::RegisterObserver(DreamBrowserObserver *pObserver) {
	RESULT r = R_PASS;

	CNM((pObserver), "Observer cannot be nullptr");
	CBM((m_pObserver == nullptr), "Can't overwrite browser observer");

	PendUpdateObjectTextures();
	m_pObserver = pObserver;

Error:
	return r;
}

RESULT DreamBrowser::UnregisterObserver(DreamBrowserObserver *pObserver) {
	RESULT r = R_PASS;

	CN(pObserver);
	CBM((m_pObserver == pObserver), "Browser Observer is not set to this object");

	m_pObserver = nullptr;

Error:
	return r;
}

// TODO: Only update the rect
// TODO: Turn off CEF when we're not using it
RESULT DreamBrowser::OnPaint(const WebBrowserRect &rect, const void *pBuffer, int width, int height) {
	RESULT r = R_PASS;

	if (m_pBrowserTexture == nullptr) {
		DOSLOG(INFO, "browser texture not initialized");
	}

	CNR(m_pBrowserTexture, R_SKIPPED);

	// Update texture dimensions if needed
	CR(m_pBrowserTexture->UpdateDimensions(width, height));
	if (r != R_NOT_HANDLED) {
		DEBUG_LINEOUT("Changed chrome texture dimensions");
	}

	CR(m_pBrowserTexture->Update((unsigned char*)(pBuffer), width, height, PIXEL_FORMAT::BGRA));
	DOSLOG(INFO, "Updated browser texture");
	// When the browser gets a paint event, it checks if its texture is currently shared
	// if so, it tells the shared view to broadcast a frame
	CNR(GetDOS()->GetSharedContentTexture().get(), R_SKIPPED);
	CBR(GetSourceTexture().get() == GetDOS()->GetSharedContentTexture().get(), R_SKIPPED);

	GetDOS()->BroadcastSharedVideoFrame((unsigned char*)(pBuffer), width, height);

Error:
	return r;
}

RESULT DreamBrowser::InitializeDreamBrowserSoundSystem() {
	RESULT r = R_PASS;

	int numChannels = 2;
	int samplingRate = 48000;
	sound::type bufferType = sound::type::SIGNED_16_BIT;

	// Set up the render Sound buffer
	CRM(InitializeRenderSoundBuffer(numChannels, samplingRate, bufferType),
		"Failed to initialize dream borwser render sound buffer");

	m_soundState = sound::state::RUNNING;
	m_browserAudioProcessingThread = std::thread(&DreamBrowser::AudioProcess, this);

Error:
	return r;
}

RESULT DreamBrowser::InitializeRenderSoundBuffer(int numChannels, int samplingRate, sound::type bufferType) {
	RESULT r = R_PASS;

	CB((m_pRenderSoundBuffer == nullptr));

	m_pRenderSoundBuffer = SoundBuffer::Make(numChannels, samplingRate, bufferType);
	CN(m_pRenderSoundBuffer);

	DEBUG_LINEOUT("Initialized Dream Browser Render Sound Buffer %d channels type: %s", numChannels, SoundBuffer::TypeString(bufferType));

Error:
	return r;
}

// TODO: This is not cross platform 
// TODO: Need to create a thread platform capability and wrap these functions 
// in there
#include <avrt.h>

RESULT DreamBrowser::AudioProcess() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Dream Browser Audio Process Started");

	int64_t pendingBytes = 0;
	DWORD taskIndex = 0;
	HANDLE hAudioRenderProcessTask = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);

	CBM((m_soundState == sound::state::RUNNING), "Dream Browser Audio Process not running");

	while (m_soundState == sound::state::RUNNING) {
		
		static std::chrono::system_clock::time_point lastUpdateTime = std::chrono::system_clock::now();

		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
		auto diffVal = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastUpdateTime).count();

		//if (m_pRenderSoundBuffer != nullptr ) {
		//if (m_pRenderSoundBuffer != nullptr && diffVal > 9) {
		if (m_pRenderSoundBuffer) {
			
			int audioBufferSampleLength10ms = m_pRenderSoundBuffer->GetSamplingRate() / 100;

			m_pRenderSoundBuffer->LockBuffer();

			{
				pendingBytes = m_pRenderSoundBuffer->NumPendingFrames();

				if (pendingBytes >= audioBufferSampleLength10ms) {

					//DEBUG_LINEOUT("pending %d", (int)pendingBytes)

					lastUpdateTime = timeNow - std::chrono::microseconds(diffVal - 10);

					AudioPacket pendingAudioPacket;

					m_pRenderSoundBuffer->GetAudioPacket(audioBufferSampleLength10ms, &pendingAudioPacket);

					if (m_pObserver != nullptr) {
						if (RCHECK(m_pObserver->HandleAudioPacket(pendingAudioPacket, this)) == false) {
							DOSLOG(INFO, "Handle Audio Packet Failed");
						}
					}
				}
			}

			m_pRenderSoundBuffer->UnlockBuffer();

		}

		// Sleep the thread for 10 ms
		Sleep(10);
		//std::this_thread::sleep_for(std::chrono::milliseconds(10));

	}

Error:
	DEBUG_LINEOUT("Dream Browser Audio Process Ended");

	return r;
}

RESULT DreamBrowser::OnAudioPacket(const AudioPacket &pendingAudioPacket) {
	RESULT r = R_PASS;

	// TODO: Handle this (if streaming we broadcast into webrtc
	// TODO: Either put this back in or move it to a different layer
	//if (m_pObserver != nullptr && GetDOS()->GetSharedContentTexture() == m_pBrowserTexture) {

	if (m_pRenderSoundBuffer != nullptr) {
		CR(m_pRenderSoundBuffer->PushAudioPacket(pendingAudioPacket, true));
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
	CNR(m_pObserver, R_SKIPPED);
	if (strTitle != "") {
		m_strCurrentTitle = strTitle;
		CR(m_pObserver->UpdateControlBarText(strTitle));
	}
	else {
		CR(m_pObserver->UpdateControlBarText(m_strCurrentURL));
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
