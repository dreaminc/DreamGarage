#include "DreamShareView.h" 
#include "DreamOS.h"

#include "Primitives/quad.h"
#include "Primitives/texture.h"
#include "DreamShareViewMessage.h"
#include "DreamControlView/DreamControlView.h"

RESULT DreamShareViewHandle::SendCastingEvent() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(ShowCastingTexture());
Error:
	return r;
}

RESULT DreamShareViewHandle::SendLoadingEvent() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(ShowLoadingTexture());
Error:
	return r;
}

RESULT DreamShareViewHandle::SendStopEvent() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(HandleStopEvent());
Error:
	return r;
}

RESULT DreamShareViewHandle::SendShowEvent() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(Show());
Error:
	return r;
}

RESULT DreamShareViewHandle::SendHideEvent() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(Hide());
Error:
	return r;
}

RESULT DreamShareViewHandle::RequestIsReceivingStream(bool &fReceivingStream) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(IsReceivingStream(fReceivingStream));
Error:
	return r;
}

RESULT DreamShareViewHandle::SendCastTexture(std::shared_ptr<texture> pNewCastTexture) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(SetCastingTexture(pNewCastTexture));
Error:
	return r;
}

RESULT DreamShareViewHandle::SendVideoFrame(const void* pBuffer, int width, int height) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(BroadcastVideoFrame(pBuffer, width, height));
Error:
	return r;
}

RESULT DreamShareViewHandle::RequestBeginStream() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(BeginStream());
Error:
	return r;
}

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

	GetComposite()->SetPosition(point(0.0f, 2.0f, -2.0f));

	int channels = 4;
	int pxSize = m_castpxWidth * m_castpxHeight * channels;
	std::vector<unsigned char> vectorByteBuffer(pxSize, 0xFF);

	// set up the casting quad
	m_aspectRatio = ((float)m_castpxWidth / (float)m_castpxHeight);
	float castWidth = std::sqrt(((m_aspectRatio * m_aspectRatio) * (m_diagonalSize * m_diagonalSize)) / (1.0f + (m_aspectRatio * m_aspectRatio)));
	float castHeight = std::sqrt((m_diagonalSize * m_diagonalSize) / (1.0f + (m_aspectRatio * m_aspectRatio)));
	vector vNormal = vector(0.0f, 0.0f, 1.0f).Normal();

	m_pCastQuad = GetComposite()->AddQuad(castWidth, castHeight, 1, 1, nullptr, vNormal);
	CN(m_pCastQuad);

	m_pCastQuad->SetMaterialAmbient(0.90f);
	m_pCastQuad->FlipUVVertical();
	CR(m_pCastQuad->SetVisible(false));

	m_pCastTexture = GetComposite()->MakeTexture(
		texture::TEXTURE_TYPE::TEXTURE_DIFFUSE, 
		m_castpxWidth, 
		m_castpxHeight, 
		PIXEL_FORMAT::RGBA, 
		channels, 
		&vectorByteBuffer[0], 
		pxSize);	
	CN(m_pCastTexture);

	m_pLoadingTexture = std::shared_ptr<texture>(GetDOS()->MakeTexture(k_wszLoadingScreen, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
	CN(m_pLoadingTexture);

	m_pCastQuad->SetDiffuseTexture(m_pLoadingTexture.get());

	GetComposite()->SetMaterialShininess(0.0f, true);
	GetComposite()->SetMaterialSpecularColor(color(0.0f, 0.0f, 0.0f, 1.0f), true);

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

	if (m_pDreamUserHandle == nullptr) {
		auto pDreamOS = GetDOS();
		CNR(pDreamOS, R_OBJECT_NOT_FOUND);
		auto userAppIDs = pDreamOS->GetAppUID("DreamUserApp");
		CBR(userAppIDs.size() == 1, R_OBJECT_NOT_FOUND);
		m_pDreamUserHandle = dynamic_cast<DreamUserApp*>(pDreamOS->CaptureApp(userAppIDs[0], this));
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
	CN(pDreamShareViewMessage);

	//currently, only store the most recent message received
	m_currentMessageType = pDreamShareViewMessage->GetMessageType();
	m_currentAckType = pDreamShareViewMessage->GetAckType();

	switch (pDreamShareViewMessage->GetMessageType()) {
		case DreamShareViewMessage::type::PING: {
			CR(BroadcastDreamShareViewMessage(DreamShareViewMessage::type::ACK, DreamShareViewMessage::type::PING));
		} break;

		case DreamShareViewMessage::type::ACK: {
			switch (pDreamShareViewMessage->GetAckType()) {
				// We get a request streaming start ACK when we requested to start streaming
				// This will begin broadcasting
				case DreamShareViewMessage::type::REQUEST_STREAMING_START: {
					if (IsStreaming()) {
						// For non-changing stuff we need to send the current frame
						CR(GetDOS()->GetCloudController()->BroadcastTextureFrame(m_pCastTexture.get(), 0, PIXEL_FORMAT::BGRA));
					}

				} break;
			}
		} break;

		case DreamShareViewMessage::type::REQUEST_STREAMING_START: {
			CR(StartReceiving(pPeerConnection));
		} break;
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
	m_pCastQuad->SetDiffuseTexture(m_pCastTexture.get());
	return R_PASS;
}

RESULT DreamShareView::SetCastingTexture(std::shared_ptr<texture> pNewCastTexture) {
	RESULT r = R_PASS;

	if (pNewCastTexture == nullptr) {
		CR(ShowLoadingTexture());
	}
	else { // if (!m_fReceivingStream) {
		m_pCastTexture = pNewCastTexture;
	}

Error:
	return r;
}

RESULT DreamShareView::Show() {
	RESULT r = R_PASS;
	CR(GetComposite()->SetVisible(true));
Error:
	return r;
}

RESULT DreamShareView::Hide() {
	RESULT r = R_PASS;
	CR(GetComposite()->SetVisible(false));
Error:
	return r;
}

DreamAppHandle* DreamShareView::GetAppHandle() {
	return (DreamShareViewHandle*)(this);
}

RESULT DreamShareView::StartReceiving(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	//if (m_pDreamUserHandle != nullptr)
	//	m_pDreamUserHandle->SendPreserveSharingState(false);

	ShowCastingTexture();

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

	CR(BroadcastDreamShareViewMessage(DreamShareViewMessage::type::ACK, DreamShareViewMessage::type::REQUEST_STREAMING_START));

Error:
	return r;
}

RESULT DreamShareView::PendReceiving() {
	RESULT r = R_PASS;
	m_fReceivingStream = true;
	//CR(SetVisible(true));

//Error:
	return r;
}

RESULT DreamShareView::StopReceiving() {
	RESULT r = R_PASS;
	m_fReceivingStream = false;
	CR(GetComposite()->SetVisible(false));

	ShowLoadingTexture();

	//CR(	BroadcastDreamBrowserMessage(DreamShareViewMessage::type::ACK, 
	//								 DreamShareViewMessage::type::REPORT_STREAMING_STOP));

Error:
	return r;
}

RESULT DreamShareView::StopSending() {
	RESULT r = R_PASS;

	CR(SetStreamingState(false));

	m_pDreamUserHandle->SendStopSharing();

	ShowLoadingTexture();
	Hide();

	//m_pWebBrowserController->CloseBrowser();
	//m_pWebBrowserController = nullptr;

	// don't stream on the next website load
	m_fShouldBeginStream = false; 
	//CR(m_pWebBrowserController->LoadURL("about:blank"));
	CR(GetComposite()->SetVisible(false));

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

	if (m_fReceivingStream) {
		CR(GetDOS()->UnregisterVideoStreamSubscriber(this));
		m_fReceivingStream = false;
	}

	SetStreamingState(false);

	// TODO: May not be needed, if not streaming no video is actually being transmitted 
	// so unless we want to set up a WebRTC re-negotiation this is not needed anymore
	//CR(GetDOS()->GetCloudController()->StartVideoStreaming(m_browserWidth, m_browserHeight, 30, PIXEL_FORMAT::BGRA));

	//CR(BroadcastDreamBrowserMessage(DreamShareViewMessage::type::PING));
	CR(BroadcastDreamShareViewMessage(DreamShareViewMessage::type::REQUEST_STREAMING_START));
	
	// This is probably redundant!!!
	CR(GetDOS()->GetCloudController()->BroadcastTextureFrame(m_pCastTexture.get(), 0, PIXEL_FORMAT::BGRA));

	SetStreamingState(true);

Error:
	return r;
}

RESULT DreamShareView::SetStreamingState(bool fStreaming) {
	RESULT r = R_PASS;

	m_fStreaming = fStreaming;

	CNR(m_pDreamUserHandle, R_SKIPPED);
	m_pDreamUserHandle->SendStreamingState(fStreaming);

Error:
	return r;
}

bool DreamShareView::IsStreaming() {
	return m_fStreaming;
}


RESULT DreamShareView::BroadcastDreamShareViewMessage(DreamShareViewMessage::type msgType, DreamShareViewMessage::type ackType) {
	RESULT r = R_PASS;

	DreamShareViewMessage *pDreamBrowserMessage = new DreamShareViewMessage(0, 0, GetAppUID(), msgType, ackType);
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
			CR(GetDOS()->GetCloudController()->BroadcastVideoFrame((unsigned char*)(pBuffer), width, height, 4));
		}
	}
Error:
	return r;
}

RESULT DreamShareView::OnVideoFrame(PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) {
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

		if (!GetComposite()->IsVisible()) {
			GetComposite()->SetVisible(true);
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

RESULT DreamShareView::UpdateFromPendingVideoFrame() {
	RESULT r = R_PASS;

	CBM(m_pendingFrame.fPending, "No frame pending");
	CNM(m_pendingFrame.pDataBuffer, "No data buffer");

	//DEBUG_LINEOUT("inframe %d x %d", m_pendingFrame.pxWidth, m_pendingFrame.pxHeight);

	// Update texture dimensions if needed
	CR(m_pCastTexture->UpdateDimensions(m_pendingFrame.pxWidth, m_pendingFrame.pxHeight));
	if (r != R_NOT_HANDLED) {
		DEBUG_LINEOUT("Changed texture dimensions");
	}

	CRM(m_pCastTexture->Update((unsigned char*)(m_pendingFrame.pDataBuffer), m_pendingFrame.pxWidth, m_pendingFrame.pxHeight, PIXEL_FORMAT::BGRA), "Failed to update texture from pending frame");

Error:
	if (m_pendingFrame.pDataBuffer != nullptr) {
		delete [] m_pendingFrame.pDataBuffer;
		m_pendingFrame.pDataBuffer = nullptr;

		memset(&m_pendingFrame, 0, sizeof(PendingFrame));
	}

	return r;
}
