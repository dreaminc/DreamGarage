#include "DreamShareView.h" 
#include "DreamOS.h"

#include "Primitives/quad.h"
#include "Primitives/texture.h"
#include "Primitives/color.h"
#include "Primitives/font.h"
#include "Primitives/Framebuffer.h"

// TODO: make enabling PBO (un)pack more portable
#include "HAL/opengl/OGLTexture.h"
#include "HAL/opengl/OGLText.h"

#include "DreamShareViewShareMessage.h"
#include "DreamControlView/UIControlView.h"
#include "DreamUserApp.h"

#include "Sound/AudioPacket.h"
#include "DreamGarage/AudioDataMessage.h"
#include "Sound/SpatialSoundObject.h"

#include "UI/UIView.h"

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

	PathManager *pPathManager = PathManager::instance();
	std::wstring wstrAssetPath;

	int channels = 4;
	int pxSize = m_castpxWidth * m_castpxHeight * channels;
	std::vector<unsigned char> vectorByteBuffer(pxSize, 0xFF);

	// set up the casting quad
	m_aspectRatio = ((float)m_castpxWidth / (float)m_castpxHeight);
	float castWidth = std::sqrt(((m_aspectRatio * m_aspectRatio) * (m_diagonalSize * m_diagonalSize)) / (1.0f + (m_aspectRatio * m_aspectRatio)));
	float castHeight = std::sqrt((m_diagonalSize * m_diagonalSize) / (1.0f + (m_aspectRatio * m_aspectRatio)));
	vector vNormal = vector(0.0f, 0.0f, 1.0f).Normal();

	point ptPosition = point(0.0f, castWidth * m_borderHeight / 2.0f -castWidth * m_bottomBarHeight / 2.0f, 0.0f);

	m_pPointerContext = GetComposite()->MakeFlatContext(1280, 720, 4);
	//m_pCastQuadComposite = m_pPointerContext->AddComposite();
	m_pCastQuadComposite = GetComposite()->AddComposite();
	m_pCastQuad = m_pCastQuadComposite->AddQuad(castWidth, castHeight, 1, 1, nullptr, vNormal);
	CN(m_pCastQuad);

	m_pCastQuad->SetPosition(ptPosition);
	//m_pCastQuad->SetMaterialAmbient(0.90f);
	m_pCastQuad->FlipUVVertical();
	CR(m_pCastQuad->SetVisible(false));

	m_pCastBackgroundQuadComposite = GetComposite()->AddComposite();
	m_pCastBackgroundQuad = m_pCastBackgroundQuadComposite->AddQuad(castWidth * m_borderWidth, castWidth * m_borderHeight, 1, 1, nullptr, vNormal);
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
		PIXEL_FORMAT::BGRA, 
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
	if (GetDOS()->GetSandboxConfiguration().fInitSound) {
		m_pSpatialBrowserObject = GetDOS()->AddSpatialSoundObject(point(0.0f, 0.0f, 0.0f), vector(), vector());
		CN(m_pSpatialBrowserObject);
	}

	pPathManager->GetValuePath(PATH_ASSET, wstrAssetPath);
	m_pPointerLeft = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wszPointerLeftTexture)[0]);
	m_pPointerCenter = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wszPointerCenterTexture)[0]);
	m_pPointerRight = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wszPointerRightTexture)[0]);

	m_pFont = GetDOS()->MakeFont(L"Basis_Grotesque_Black.fnt", true);
	CN(m_pFont);

	m_pMirrorQuad = GetComposite()->AddQuad(castWidth,castHeight);
	m_pMirrorQuad->RotateXByDeg(90.0f);
	m_pMirrorQuad->SetVisible(false);

	for (int i = 0; i < 12; i++) {

		auto pView = GetComposite()->AddFlatContext();
		pView->RotateXByDeg(90.0f);
		pView->RotateYByDeg(-90.0f);
		pView->SetVisible(false, false);

		m_pointerViewPool.push(pView);
		GetDOS()->AddObjectToUIGraph(pView.get(), (SandboxApp::PipelineType::AUX | SandboxApp::PipelineType::MAIN));

	}

	GetDOS()->AddObjectToUIGraph(GetComposite(), (SandboxApp::PipelineType::AUX | SandboxApp::PipelineType::MAIN));

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

	if (m_pointingObjects.size() > 0) {
		
		auto qRotation = m_pCastQuad->GetOrientation(true);

		m_pMirrorQuad = m_pPointerContext->AddQuad(m_pCastQuad->GetWidth(), m_pCastQuad->GetHeight());
		m_pMirrorQuad->SetDiffuseTexture(m_pCastQuad->GetTextureDiffuse());
		float scale = m_pCastQuad->GetScale(true).x();

		for (auto pair : m_pointingObjects) {
			std::vector<std::shared_ptr<FlatContext>> userPointers = pair.second;
			for (std::shared_ptr<FlatContext> labelFlatContext : userPointers) {

				point ptPosition = (point)(inverse(RotationMatrix(m_pCastQuad->GetOrientation(true))) * (labelFlatContext->GetPosition(true) - m_pCastQuad->GetOrigin(true)));

				auto pLabelQuad = labelFlatContext->GetCurrentQuad();
				auto pFlatQuad = m_pPointerContext->AddQuad(pLabelQuad->GetWidth()/scale, pLabelQuad->GetHeight()/scale);
				pFlatQuad->SetDiffuseTexture(labelFlatContext->GetFramebuffer()->GetColorTexture());
				pFlatQuad->FlipUVVertical();
				pFlatQuad->SetVisible(labelFlatContext->IsVisible());

				// TODO: depending on final design of labels, cap positioning of the label quads so that
				// the flat context is not resized
				pFlatQuad->SetPosition(point(ptPosition.x()/scale, 0.0f, ptPosition.y()/scale));
			}
		}

		m_pPointerContext->RenderToQuad(m_pCastQuad->GetWidth(), m_pCastQuad->GetHeight(), 0, 0);

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

RESULT DreamShareView::InitializePointerLabel(std::shared_ptr<FlatContext> pView, std::string strInitials) {
	RESULT r = R_PASS;

	float height = 0.1f;
	float textHeight = 0.075f;
	float pxHeight = 61.0f;
	float pxRight = 19.0f;
	float pxLeft = 33.0f;
	float leftWidth = height * pxLeft / pxHeight;
	float rightWidth = height * pxRight / pxHeight;

	m_pFont->SetLineHeight(textHeight);

	auto pText = std::shared_ptr<text>(GetDOS()->MakeText(
		m_pFont,
		strInitials,
		0.4,
		textHeight,
		text::flags::FIT_TO_SIZE | text::flags::RENDER_QUAD));
	pText->SetPosition(point(0.0f, 0.02f, 0.0f), text::VerticalAlignment::MIDDLE, text::HorizontalAlignment::CENTER);

	// TODO: the text object should have access to the functionality of the update function
	auto oglText = dynamic_cast<OGLText*>(pText.get());
	oglText->Update();

	float width = pText->GetWidth();

	float screenOffset = 0.01f;

	auto pQuadLeft = pView->AddQuad(leftWidth, height);
	auto pQuadCenter = pView->AddQuad(width, height);
	auto pQuadRight = pView->AddQuad(rightWidth, height);

	pQuadLeft->SetDiffuseTexture(m_pPointerLeft);
	pQuadCenter->SetDiffuseTexture(m_pPointerCenter);
	pQuadRight->SetDiffuseTexture(m_pPointerRight);

	pQuadLeft->SetPosition(-(width + leftWidth) / 2.0f, 0.0f, 0.0f);
	pQuadCenter->SetPosition(0.0f, 0.0f, 0.0f);
	pQuadRight->SetPosition((width + rightWidth) / 2.0f, 0.0f, 0.0f);

	pView->AddObject(pText);

	pView->RenderToQuad(leftWidth + width + rightWidth, height, 0, 0);

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

		std::shared_ptr<FlatContext> pPointer;
		long userID = pUpdatePointerMessage->GetSenderUserID();

		CR(AllocateSpheres(userID, pUpdatePointerMessage->m_body.szInitials));

		if (pUpdatePointerMessage->m_body.fLeftHand) {
			pPointer = m_pointingObjects[userID][0];
		}
		else {
			pPointer = m_pointingObjects[userID][1];
		}

		pPointer->SetPosition(pUpdatePointerMessage->m_body.ptPointer + point(-0.01f, 0.0f, 0.0f));
		pPointer->SetVisible(pUpdatePointerMessage->m_body.fVisible, false);
		//pPointer->SetMaterialDiffuseColor(pUpdatePointerMessage->m_body.cColor);
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

texture* DreamShareView::GetPointingTexture() {
	return m_pPointerContext->GetFramebuffer()->GetColorTexture();
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
	CR(SetReceivingState(true));

	CR(BroadcastDreamShareViewMessage(DreamShareViewShareMessage::type::ACK, DreamShareViewShareMessage::type::REQUEST_STREAMING_START));

Error:
	return r;
}

RESULT DreamShareView::PendReceiving() {
	RESULT r = R_PASS;

	//ShowCastingTexture();
	m_pCastQuad->SetDiffuseTexture(m_pVideoCastTexture.get());
	CR(SetReceivingState(true));
	//CR(SetVisible(true));

Error:
	return r;
}

RESULT DreamShareView::StopReceiving() {
	RESULT r = R_PASS;

	m_pStreamerPeerConnection = nullptr;
	CR(SetReceivingState(false));

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
		CR(SetReceivingState(false));
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

	bool fReceiving;
	IsReceivingStream(fReceiving);

	bool fIsActive = m_fStreaming || fReceiving;

	m_fIsActive = fIsActive;
	SendDOSMessage();

Error:
	return r;
}

RESULT DreamShareView::SetReceivingState(bool fReceiving) {
	RESULT r = R_PASS;

	m_fReceivingStream = fReceiving;

	bool fIsActive = IsStreaming() || m_fReceivingStream;

	m_fIsActive = fIsActive;
	SendDOSMessage();

Error:
	return r;
}

bool DreamShareView::IsStreaming() {
	return m_fStreaming;
}

RESULT DreamShareView::SendDOSMessage() {
	RESULT r = R_PASS;

	if (m_fIsActive) {
		std::string strActive = "DreamShareView.IsActive";
		GetDOS()->SendDOSMessage(strActive);
	}
	else {
		std::string strNotActive = "DreamShareView.IsNotActive";
		GetDOS()->SendDOSMessage(strNotActive);
	}

Error:
	return r;
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
		/*
		// Update texture dimensions if needed
		CR(m_pCastTexture->UpdateDimensions(width, height));
		if (r != R_NOT_HANDLED) {
			DEBUG_LINEOUT("Changed chrome texture dimensions");
		}

		CR(m_pCastTexture->UpdateTextureFromBuffer((unsigned char*)(pBuffer), width * height * 4));
		//*/
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
			PIXEL_FORMAT::BGRA,
			4,
			&m_pendingFrame.pDataBuffer[0],
			(int)m_pendingFrame.pDataBuffer_n);
		//*/
		
		CR(m_pVideoCastTexture->UpdateDimensions(m_pendingFrame.pxWidth, m_pendingFrame.pxHeight));

		OGLTexture* pOGLTexture = dynamic_cast<OGLTexture*>(m_pVideoCastTexture.get());
		if (!pOGLTexture->IsOGLPBOUnpackEnabled()) {
			pOGLTexture->EnableOGLPBOUnpack();
		}

		if (r != R_NOT_HANDLED) {
			DEBUG_LINEOUT("Changed texture dimensions");
		}
	}
	else {
		if (m_pCastQuad->GetTextureDiffuse() != m_pVideoCastTexture.get()) {
			m_pCastQuad->SetDiffuseTexture(m_pVideoCastTexture.get());
		}

		CRM(m_pVideoCastTexture->UpdateTextureFromBuffer((unsigned char*)(m_pendingFrame.pDataBuffer), m_pendingFrame.pDataBuffer_n), "Failed to update texture from pending frame");
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

	m_pCastQuadComposite->SetPosition(ptPosition);
	m_pCastQuadComposite->SetOrientation(qOrientation);
	m_pCastQuadComposite->SetScale(scale);

	m_pCastBackgroundQuadComposite->SetPosition(ptPosition);
	m_pCastBackgroundQuadComposite->SetOrientation(qOrientation);
	m_pCastBackgroundQuadComposite->SetScale(scale);

	m_pMirrorQuad->SetPosition(ptPosition);
	m_pMirrorQuad->SetOrientation(qOrientation);
	m_pMirrorQuad->SetScale(scale);

	if (m_pSpatialBrowserObject != nullptr) {
		m_pSpatialBrowserObject->SetPosition(ptPosition);
	}

Error:
	return r;
}

RESULT DreamShareView::AllocateSpheres(long userID, std::string strInitials) {
	RESULT r = R_PASS;
	std::vector<std::shared_ptr<FlatContext>> userPointers;

	std::shared_ptr<text> pText = nullptr;
	std::shared_ptr<FlatContext> pView = nullptr;

	CBR(userID != -1, R_SKIPPED);
	CBR(m_pointingObjects.count(userID) == 0, R_SKIPPED);


	for (int i = 0; i < 2; i++) {
		pView = m_pointerViewPool.front();

		CR(InitializePointerLabel(pView, strInitials));

		userPointers.emplace_back(pView);
		m_pointerViewPool.pop();
	}

	m_pointingObjects[userID] = userPointers;

Error:
	return r;
}

RESULT DreamShareView::AllocateSpheres(long userID) {
	RESULT r = R_PASS;

	std::vector<std::shared_ptr<FlatContext>> userPointers;

	CBR(userID != -1, R_SKIPPED);
	CBR(m_pointingObjects.count(userID) == 0, R_SKIPPED);

	userPointers.emplace_back(m_pointerViewPool.front());
	m_pointerViewPool.pop();
	userPointers.emplace_back(m_pointerViewPool.front());
	m_pointerViewPool.pop();

	m_pointingObjects[userID] = userPointers;

Error:
	return r;
}

RESULT DreamShareView::DeallocateSpheres(long userID) {
	RESULT r = R_PASS;

	std::vector<std::shared_ptr<FlatContext>> userPointers;

	CBR(userID != -1, R_SKIPPED);
	CBR(m_pointingObjects.count(userID) != 0, R_SKIPPED);

	userPointers = m_pointingObjects[userID];

	m_pointerViewPool.push(userPointers[0]);
	m_pointerViewPool.push(userPointers[1]);

	m_pointingObjects.erase(userID);

Error:
	return r;
}
