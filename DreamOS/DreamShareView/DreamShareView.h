#ifndef DREAM_SHARE_VIEW_H_
#define DREAM_SHARE_VIEW_H_

#include "RESULT/EHM.h"

#include "DreamApp.h"
#include "DreamAppHandle.h"
#include "DreamVideoStreamSubscriber.h"
#include "DreamShareViewMessage.h"
#include "DreamUserApp.h"

#define PX_WIDTH 1366
#define PX_HEIGHT 768

class quad;
class texture;

class DreamShareViewHandle : public DreamAppHandle {
public:
	// switch between loading screen and casted texture
	RESULT SendLoadingEvent();
	RESULT SendCastingEvent();
	RESULT SendStopEvent();

	RESULT SendShowEvent();
	RESULT SendHideEvent();

	RESULT RequestIsReceivingStream(bool &fReceivingStream);

	// idea here is to set the texture being cast here while the 
	// texture is updated elsewhere
	RESULT SendCastTexture(std::shared_ptr<texture> pNewCastTexture);
	std::shared_ptr<texture> RequestCastTexture();
	RESULT SendVideoFrame(const void* pBuffer, int width, int height);
	
	RESULT RequestBeginStream();

private:
	virtual RESULT ShowLoadingTexture() = 0;
	virtual RESULT ShowCastingTexture() = 0;
	virtual RESULT HandleStopEvent() = 0;
	virtual RESULT Show() = 0;
	virtual RESULT Hide() = 0;
	virtual RESULT SetCastingTexture(std::shared_ptr<texture> pNewCastTexture) = 0;
	virtual std::shared_ptr<texture> GetCastingTexture() = 0;
	virtual RESULT IsReceivingStream(bool &fReceivingStream) = 0;
	virtual RESULT BeginStream() = 0;
	virtual RESULT BroadcastVideoFrame(const void *pBuffer, int width, int height) = 0;
};

class DreamShareView :
	public DreamApp<DreamShareView>,
	public DreamShareViewHandle,
	public DreamVideoStreamSubscriber
{
	friend class DreamAppManager;
	friend class MultiContentTestSuite;

public:
	DreamShareView(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamShareView();

	// DreamApp Interface
	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;
	virtual RESULT HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage) override;

	virtual DreamAppHandle *GetAppHandle() override;

	// Handle Interface
	virtual RESULT ShowLoadingTexture() override;
	virtual RESULT ShowCastingTexture() override;
	virtual RESULT SetCastingTexture(std::shared_ptr<texture> pNewCastTexture) override;
	virtual std::shared_ptr<texture> GetCastingTexture() override;
	virtual RESULT Show() override;
	virtual RESULT Hide() override;

	// Casting/Sharing
	RESULT StartReceiving(PeerConnection *pPeerConnection);
	RESULT PendReceiving();
	RESULT StopReceiving();
	RESULT StopSending();
	virtual RESULT IsReceivingStream(bool &fReceivingStream) override;
	virtual RESULT HandleStopEvent() override;

	// App Messaging
	virtual RESULT DreamShareView::BeginStream() override;
	RESULT BroadcastDreamShareViewMessage(DreamShareViewMessage::type msgType, DreamShareViewMessage::type ackType = DreamShareViewMessage::type::INVALID);

	bool IsStreaming();
	RESULT SetStreamingState(bool fStreaming);

	// Video Stream Subscriber
	virtual RESULT BroadcastVideoFrame(const void *pBuffer, int width, int height) override;
	virtual RESULT OnVideoFrame(PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override;
	RESULT SetupPendingVideoFrame(uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight);
	RESULT UpdateFromPendingVideoFrame();

	struct PendingFrame {
		bool fPending = false;
		int pxWidth = 0;
		int pxHeight = 0;
		uint8_t *pDataBuffer = nullptr;
		size_t pDataBuffer_n = 0;
	} m_pendingFrame;

protected:
	static DreamShareView* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	const wchar_t *k_wszLoadingScreen = L"client-loading-1366-768.png";

private:
	// Quad related members
	std::shared_ptr<quad> m_pCastQuad = nullptr;
	std::shared_ptr<texture> m_pCastTexture = nullptr;
	std::shared_ptr<texture> m_pLoadingTexture = nullptr;

	int m_castpxWidth = PX_WIDTH;
	int m_castpxHeight = PX_HEIGHT;
	float m_aspectRatio;
	float m_diagonalSize = 9.0f;

	// Streaming members
	bool m_fStreaming = false;
	bool m_fReceivingStream = false;
	bool m_fShouldBeginStream = false;
	bool m_fReadyForFrame = false;

	// Dream app message members
	DreamShareViewMessage::type m_currentMessageType;
	DreamShareViewMessage::type m_currentAckType;

	DreamUserHandle* m_pDreamUserHandle = nullptr;

};

#endif // ! DREAM_SHARE_VIEW_H_ 
