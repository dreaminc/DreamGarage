#include "DreamShareView.h" 
#include "DreamOS.h"

#include "Primitives/quad.h"
#include "Primitives/texture.h"
#include "Primitives/color.h"

#include "DreamShareViewShareMessage.h"
#include "DreamControlView/UIControlView.h"
#include "DreamUserApp.h"

#include "Sound/AudioPacket.h"
#include "DreamGarage/AudioDataMessage.h"
#include "Sound/SpatialSoundObject.h"

DreamShareView::DreamShareView(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamShareView>(pDreamOS, pContext)
{
	// Empty
}

DreamShareView::~DreamShareView() {
	RESULT r = R_PASS;

	CR(Shutdown());

Error:
	return;
}

RESULT DreamShareView::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	SetAppName("DreamShareView");

	DreamOS *pDreamOS = GetDOS();
	std::shared_ptr<DreamUserApp> pDreamUserApp = pDreamOS->GetUserApp();

	GetDOS()->AddObjectToUIGraph(GetComposite(), (SandboxApp::PipelineType::AUX | SandboxApp::PipelineType::MAIN));

	int channels = 4;
	int pxSize = m_castpxWidth * m_castpxHeight * channels;
	std::vector<unsigned char> vectorByteBuffer(pxSize, 0xFF);

	// set up the casting quad
	m_aspectRatio = ((float)m_castpxWidth / (float)m_castpxHeight);
	float castWidth = std::sqrt(((m_aspectRatio * m_aspectRatio) * (m_diagonalSize * m_diagonalSize)) / (1.0f + (m_aspectRatio * m_aspectRatio)));
	float castHeight = std::sqrt((m_diagonalSize * m_diagonalSize) / (1.0f + (m_aspectRatio * m_aspectRatio)));
	vector vNormal = vector(0.0f, 0.0f, 1.0f).Normal();

	point ptPosition = point(0.0f, castWidth * m_borderHeight / 2.0f -castWidth * m_bottomBarHeight / 2.0f, 0.0f);

	m_pCastQuad = GetComposite()->AddQuad(castWidth, castHeight, 1, 1, nullptr, vNormal);
	CN(m_pCastQuad);

	m_pCastQuad->SetPosition(ptPosition);
	//m_pCastQuad->SetMaterialAmbient(0.90f);
	m_pCastQuad->FlipUVVertical();
	CR(m_pCastQuad->SetVisible(false));

	m_pCastBackgroundQuad = GetComposite()->AddQuad(castWidth * m_borderWidth, castWidth * m_borderHeight, 1, 1, nullptr, vNormal);
	m_pCastBackgroundQuad->SetDiffuseTexture(GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"control-view-main-background.png"));
	m_pCastBackgroundQuad->SetPosition(ptPosition + point(0.0f, 0.0f, -0.001f));
	m_pCastBackgroundQuad->SetVisible(false);

	CR(GetDOS()->AddAndRegisterInteractionObject(m_pCastBackgroundQuad.get(), ELEMENT_INTERSECT_BEGAN, pDreamUserApp.get()));
	CR(GetDOS()->AddAndRegisterInteractionObject(m_pCastBackgroundQuad.get(), ELEMENT_INTERSECT_MOVED, pDreamUserApp.get()));
	CR(GetDOS()->AddAndRegisterInteractionObject(m_pCastBackgroundQuad.get(), ELEMENT_INTERSECT_ENDED, pDreamUserApp.get()));

	m_pVideoCastTexture = GetComposite()->MakeTexture(
		texture::type::TEXTURE_2D,
		m_castpxWidth, 
		m_castpxHeight, 
		PIXEL_FORMAT::RGBA, 
		channels, 
		&vectorByteBuffer[0], 
		pxSize);	
	CN(m_pVideoCastTexture);

	m_pLoadingTexture = std::shared_ptr<texture>(GetDOS()->MakeTexture(texture::type::TEXTURE_2D, k_wszLoadingScreen));
	CN(m_pLoadingTexture);

	m_pCastQuad->SetDiffuseTexture(m_pLoadingTexture.get());

	GetComposite()->SetMaterialShininess(0.0f, true);
	GetComposite()->SetMaterialSpecularColor(color(0.0f, 0.0f, 0.0f, 1.0f), true);

	// Spatial Audio Object
	m_pSpatialBrowserObject = GetDOS()->AddSpatialSoundObject(point(0.0f, 0.0f, 0.0f), vector(), vector());
	CN(m_pSpatialBrowserObject);

	for (int i = 0; i < 12; i++) {

		auto pSphere = GetDOS()->AddSphere(0.025f);
		pSphere->SetVisible(false);
		if (i % 2 == 0) {
			pSphere->SetMaterialDiffuseColor(COLOR_RED);
		}
		else {
			pSphere->SetMaterialDiffuseColor(COLOR_BLUE);
		}

		m_pointerSpherePool.push(pSphere);
	}

Error:
	return r;
}

RESULT DreamShareView::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamShareView::Update(void *pContext) {
	RESULT r = R_PASS;

	if (m_fReceivingStream && m_pendingFrame.fPending) {
		CRM(UpdateFromPendingVideoFrame(), "Failed to update pending frame");
	}

Error:
	return r;
}

RESULT DreamShareView::Shutdown(void *pContext) {
	return R_PASS;
}

RESULT DreamShareView::HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage) {
	RESULT r = R_PASS;

	DreamShareViewMessage *pDreamShareViewMessage = (DreamShareViewMessage*)(pDreamAppMessage);
	CNR(pDreamShareViewMessage, R_SKIPPED);

	CBR(pDreamAppMessage->GetDreamAppName() == GetAppName(), R_SKIPPED);

	CB(pDreamShareViewMessage->GetMessageType() != DreamShareViewMessage::type::INVALID);

	switch (pDreamShareViewMessage->GetMessageType()) {

	case (DreamShareViewMessage::type::SHARE): {
		CR(HandleShareMessage(pPeerConnection, (DreamShareViewShareMessage*)(pDreamAppMessage)));
	} break;

	case (DreamShareViewMessage::type::POINTER): {
		CR(HandlePointerMessage(pPeerConnection, (DreamShareViewPointerMessage*)(pDreamAppMessage)));
	} break;

	}

Error:
	return r;
}

RESULT DreamShareView::HandleShareMessage(PeerConnection* pPeerConnection, DreamShareViewShareMessage *pShareMessage) {
	RESULT r = R_PASS;

	CN(pShareMessage);

	//currently, only store the most recent message received
	m_currentMessageType = pShareMessage->GetShareMessageType();
	m_currentAckType = pShareMessage->GetAckType();

	switch (pShareMessage->GetShareMessageType()) {
		case DreamShareViewShareMessage::type::PING: {
			CR(BroadcastDreamShareViewMessage(DreamShareViewShareMessage::type::ACK, DreamShareViewShareMessage::type::PING));
		} break;

		case DreamShareViewShareMessage::type::ACK: {
			switch (pShareMessage->GetAckType()) {
				// We get a request streaming start ACK when we requested to start streaming
				// This will begin broadcasting
			case DreamShareViewShareMessage::type::REQUEST_STREAMING_START: {
				if (IsStreaming()) {
					// For non-changing stuff we need to send the current frame
					CR(GetDOS()->GetCloudController()->BroadcastTextureFrame(kChromeVideoLabel, m_pCastTexture, 0, PIXEL_FORMAT::BGRA));
				}
			} break;
			}
		} break;

		case DreamShareViewShareMessage::type::REQUEST_STREAMING_START: {
			CR(StartReceiving(pPeerConnection));
		} break;
	}

Error:
	return r;
}

RESULT DreamShareView::HandlePointerMessage(PeerConnection* pPeerConnection, DreamShareViewPointerMessage *pUpdatePointerMessage) {
	RESULT r = R_PASS;

	CN(pUpdatePointerMessage);

	if (m_fReceivingStream || IsStreaming()) {

		sphere *pPointer;
		long userID = pUpdatePointerMessage->GetSenderUserID();

		CR(AllocateSpheres(userID));

		if (pUpdatePointerMessage->m_body.fLeftHand) {
			pPointer = m_pointingObjects[userID][0];
		}
		else {
			pPointer = m_pointingObjects[userID][1];
		}

		pPointer->SetPosition(pUpdatePointerMessage->m_body.ptPointer);
		pPointer->SetVisible(pUpdatePointerMessage->m_body.fVisible);
		pPointer->SetMaterialDiffuseColor(pUpdatePointerMessage->m_body.cColor);
	}

Error:
	return r;
}

DreamShareView* DreamShareView::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamShareView *pDreamApp = new DreamShareView(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamShareView::ShowLoadingTexture() {
	m_pCastQuad->SetDiffuseTexture(m_pLoadingTexture.get());
	return R_PASS;
}

RESULT DreamShareView::ShowCastingTexture() {
	m_pCastQuad->SetDiffuseTexture(m_pCastTexture);
	return R_PASS;
}

RESULT DreamShareView::SetCastingTexture(texture* pNewCastTexture) {
	RESULT r = R_PASS;

	m_pCastTexture = pNewCastTexture;

//Error:
	return r;
}

texture* DreamShareView::GetCastingTexture() {
//	return m_pCastTexture;
	return m_pCastQuad->GetTextureDiffuse();
}

RESULT DreamShareView::Show() {
	RESULT r = R_PASS;
	//CR(GetComposite()->SetVisible(true));
	m_pCastQuad->SetVisible(true);
	m_pCastBackgroundQuad->SetVisible(true);
Error:
	return r;
}

RESULT DreamShareView::Hide() {
	RESULT r = R_PASS;
	//CR(GetComposite()->SetVisible(false));
	m_pCastQuad->SetVisible(false);
	m_pCastBackgroundQuad->SetVisible(false);
Error:
	return r;
}

RESULT DreamShareView::StartReceiving(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	m_pStreamerPeerConnection = pPeerConnection;

	//ShowCastingTexture();
	m_pCastQuad->SetVisible(true);
	m_pCastBackgroundQuad->SetVisible(true);
	m_pCastQuad->SetDiffuseTexture(m_pVideoCastTexture.get());

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

	CR(BroadcastDreamShareViewMessage(DreamShareViewShareMessage::type::ACK, DreamShareViewShareMessage::type::REQUEST_STREAMING_START));

Error:
	return r;
}

RESULT DreamShareView::PendReceiving() {
	RESULT r = R_PASS;

	//ShowCastingTexture();
	m_pCastQuad->SetDiffuseTexture(m_pVideoCastTexture.get());
	m_fReceivingStream = true;
	//CR(SetVisible(true));

Error:
	return r;
}

RESULT DreamShareView::StopReceiving() {
	RESULT r = R_PASS;

	m_pStreamerPeerConnection = nullptr;
	m_fReceivingStream = false;

	//CR(GetComposite()->SetVisible(false));
	CR(Hide());

	ShowLoadingTexture();

	if (GetDOS()->IsRegisteredVideoStreamSubscriber(this)) {
		CR(GetDOS()->UnregisterVideoStreamSubscriber(this));
	}

	//CR(	BroadcastDreamBrowserMessage(DreamShareViewMessage::type::ACK, 
	//								 DreamShareViewMessage::type::REPORT_STREAMING_STOP));

Error:
	return r;
}

RESULT DreamShareView::StopSending() {
	RESULT r = R_PASS;

	CR(SetStreamingState(false));

	//m_pCastTexture = m_pLoadingTexture;
	ShowLoadingTexture();
	Hide();

	//m_pWebBrowserController->CloseBrowser();
	//m_pWebBrowserController = nullptr;

	// don't stream on the next website load
	m_fShouldBeginStream = false; 
	//CR(m_pWebBrowserController->LoadURL("about:blank"));
	//CR(GetComposite()->SetVisible(false));
	CR(Hide());

Error:
	return r;
}

RESULT DreamShareView::IsReceivingStream(bool &fReceivingStream) {
	fReceivingStream = m_fReceivingStream;
	return R_PASS;
}

RESULT DreamShareView::HandleStopEvent() {
	RESULT r = R_PASS;
	auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");

//	CR(m_pEnvironmentControllerProxy->RequestStopSharing(m_currentEnvironmentAssetID, m_strScope, m_strPath));
	CR(SetStreamingState(false));
Error:
	return r;
}

RESULT DreamShareView::BeginStream() {
	RESULT r = R_PASS;

	m_pCastQuad->SetVisible(true);
	m_pCastQuad->SetDiffuseTexture(m_pCastTexture);
	m_pCastBackgroundQuad->SetVisible(true);

	if (m_fReceivingStream) {
		CR(GetDOS()->UnregisterVideoStreamSubscriber(this));
		m_fReceivingStream = false;
	}

	SetStreamingState(false);

	// TODO: May not be needed, if not streaming no video is actually being transmitted 
	// so unless we want to set up a WebRTC re-negotiation this is not needed anymore
	//CR(GetDOS()->GetCloudController()->StartVideoStreaming(m_browserWidth, m_browserHeight, 30, PIXEL_FORMAT::BGRA));

	//CR(BroadcastDreamBrowserMessage(DreamShareViewMessage::type::PING));
	CR(BroadcastDreamShareViewMessage(DreamShareViewShareMessage::type::REQUEST_STREAMING_START));
	SetStreamingState(true);

Error:
	return r;
}

RESULT DreamShareView::SetStreamingState(bool fStreaming) {
	RESULT r = R_PASS;

	m_fStreaming = fStreaming;

Error:
	return r;
}

bool DreamShareView::IsStreaming() {
	return m_fStreaming;
}


RESULT DreamShareView::BroadcastDreamShareViewMessage(DreamShareViewShareMessage::type msgType, DreamShareViewShareMessage::type ackType) {
	RESULT r = R_PASS;

	DreamShareViewShareMessage *pDreamBrowserMessage = new DreamShareViewShareMessage(0, 0, GetAppUID(), msgType, ackType);
	CN(pDreamBrowserMessage);

	CR(BroadcastDreamAppMessage(pDreamBrowserMessage));

Error:
	return r;
}

RESULT DreamShareView::BroadcastVideoFrame(const void *pBuffer, int width, int height) {
	RESULT r = R_PASS;

	if (!m_fReceivingStream) {
		CN(m_pCastTexture);

		// Update texture dimensions if needed
		CR(m_pCastTexture->UpdateDimensions(width, height));
		if (r != R_NOT_HANDLED) {
			DEBUG_LINEOUT("Changed chrome texture dimensions");
		}

		CR(m_pCastTexture->Update((unsigned char*)(pBuffer), width, height, PIXEL_FORMAT::BGRA));

		//*
		if (IsStreaming()) {
			CR(GetDOS()->GetCloudController()->BroadcastVideoFrame(kChromeVideoLabel, (unsigned char*)(pBuffer), width, height, 4));
		}
	}
Error:
	return r;
}

PeerConnection *DreamShareView::GetStreamingPeerConnection() {
	return m_pStreamerPeerConnection;
}

RESULT DreamShareView::BroadcastAudioPacket(const AudioPacket &pendingAudioPacket) {
	RESULT r = R_PASS;

	// TODO: this
	//CBR(m_fStreaming, R_SKIPPED);

	{
		auto pCloudController = GetDOS()->GetCloudController();

		CN(pCloudController);
		CR(pCloudController->BroadcastAudioPacket(kChromeAudioLabel, pendingAudioPacket));
	}

Error:
	return r;
}

RESULT DreamShareView::OnVideoFrame(const std::string &strVideoTrackLabel, PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	// TODO: Create a pending frame thing
	//CR(m_pBrowserTexture->Update((unsigned char*)(pVideoFrameDataBuffer), pxWidth, pxHeight, texture::PixelFormat::RGBA));
	CBR(strVideoTrackLabel == kChromeVideoLabel, R_SKIPPED);

	if (m_fReceivingStream) {
		r = SetupPendingVideoFrame((unsigned char*)(pVideoFrameDataBuffer), pxWidth, pxHeight);

		if (r == R_OVERFLOW) {
			DEBUG_LINEOUT("Overflow frame!");
			return R_PASS;
		}

		CRM(r, "Failed for other reason");

		/*
		if (!GetComposite()->IsVisible()) {
			GetComposite()->SetVisible(true);
		}
		//*/
		if (!m_pCastQuad->IsVisible()) {
			Show();
		}
	}

Error:
	return r;
}

RESULT DreamShareView::SetupPendingVideoFrame(uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) {
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

RESULT DreamShareView::HandleChromeAudioDataMessage(PeerConnection* pPeerConnection, AudioDataMessage *pAudioDataMessage) {
	RESULT r = R_PASS;

	// TODO: Handle channels?
	size_t numFrames = pAudioDataMessage->GetNumFrames();
	size_t channels = pAudioDataMessage->GetNumChannels();

	int16_t *pAudioDataBuffer = (int16_t*)(pAudioDataMessage->GetAudioMessageBuffer());
	CN(pAudioDataBuffer);

	// Play
	if (m_pSpatialBrowserObject != nullptr) {

		// Not sure if we need to allocate new memory here or not

		int16_t *pInt16Soundbuffer = new int16_t[numFrames];
		memcpy((void*)pInt16Soundbuffer, pAudioDataBuffer, sizeof(int16_t) * numFrames);

		if (pInt16Soundbuffer != nullptr) {
			CR(m_pSpatialBrowserObject->PushMonoAudioBuffer((int)numFrames, pInt16Soundbuffer));
		}
	}

Error:
	return r;
}

RESULT DreamShareView::UpdateFromPendingVideoFrame() {
	RESULT r = R_PASS;

	int castBufferSize = m_castpxWidth * m_castpxHeight * 4;
	CBM(m_pendingFrame.fPending, "No frame pending");
	CNM(m_pendingFrame.pDataBuffer, "No data buffer");
	//DEBUG_LINEOUT("inframe %d x %d", m_pendingFrame.pxWidth, m_pendingFrame.pxHeight);

	// Update texture dimensions if needed

	if ((int)m_pendingFrame.pDataBuffer_n != castBufferSize) {
		m_castpxHeight = m_pendingFrame.pxHeight;
		m_castpxWidth = m_pendingFrame.pxWidth;
		//float pxSize = m_pendingFrame.pxWidth * m_pendingFrame.pxHeight * 4;
		//*
		m_pVideoCastTexture = GetComposite()->MakeTexture(
			texture::type::TEXTURE_2D,
			m_pendingFrame.pxWidth,
			m_pendingFrame.pxHeight,
			PIXEL_FORMAT::RGBA,
			4,
			&m_pendingFrame.pDataBuffer[0],
			(int)m_pendingFrame.pDataBuffer_n);
		//*/
		
		CR(m_pVideoCastTexture->UpdateDimensions(m_pendingFrame.pxWidth, m_pendingFrame.pxHeight));

		if (r != R_NOT_HANDLED) {
			DEBUG_LINEOUT("Changed texture dimensions");
		}
	}
	else {
		if (m_pCastQuad->GetTextureDiffuse() != m_pVideoCastTexture.get()) {
			m_pCastQuad->SetDiffuseTexture(m_pVideoCastTexture.get());
		}

		CRM(m_pVideoCastTexture->Update((unsigned char*)(m_pendingFrame.pDataBuffer), m_pendingFrame.pxWidth, m_pendingFrame.pxHeight, PIXEL_FORMAT::BGRA), "Failed to update texture from pending frame");
	}

Error:
	if (m_pendingFrame.pDataBuffer != nullptr) {
		delete [] m_pendingFrame.pDataBuffer;
		m_pendingFrame.pDataBuffer = nullptr;

		memset(&m_pendingFrame, 0, sizeof(PendingFrame));
	}

	return r;
}

RESULT DreamShareView::UpdateScreenPosition(point ptPosition, quaternion qOrientation, float scale) {
	RESULT r = R_PASS;

	GetComposite()->SetPosition(ptPosition);
	GetComposite()->SetOrientation(qOrientation);
	GetComposite()->SetScale(scale);

	if (m_pSpatialBrowserObject != nullptr) {
		m_pSpatialBrowserObject->SetPosition(ptPosition);
	}

Error:
	return r;
}

RESULT DreamShareView::AllocateSpheres(long userID) {
	RESULT r = R_PASS;

	std::vector<sphere*> userPointers;

	CBR(userID != -1, R_SKIPPED);
	CBR(m_pointingObjects.count(userID) == 0, R_SKIPPED);

	userPointers.emplace_back(m_pointerSpherePool.front());
	m_pointerSpherePool.pop();
	userPointers.emplace_back(m_pointerSpherePool.front());
	m_pointerSpherePool.pop();

	m_pointingObjects[userID] = userPointers;

Error:
	return r;
}

RESULT DreamShareView::DeallocateSpheres(long userID) {
	RESULT r = R_PASS;

	std::vector<sphere*> userPointers;

	CBR(userID != -1, R_SKIPPED);
	CBR(m_pointingObjects.count(userID) != 0, R_SKIPPED);

	userPointers = m_pointingObjects[userID];

	m_pointerSpherePool.push(userPointers[0]);
	m_pointerSpherePool.push(userPointers[1]);

	m_pointingObjects.erase(userID);

Error:
	return r;
}
