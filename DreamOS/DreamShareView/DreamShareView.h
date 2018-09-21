#ifndef DREAM_SHARE_VIEW_H_
#define DREAM_SHARE_VIEW_H_

#include "RESULT/EHM.h"

#include "DreamApp.h"
#include "DreamAppHandle.h"
#include "DreamVideoStreamSubscriber.h"
#include "DreamShareViewMessage.h"
#include "DreamUserApp.h"

#include "DreamGarage/UICommon.h"

class quad;
class texture;
class AudioPacket;
class SpatialSoundObject;
class AudioDataMessage;

class DreamShareView :
	public DreamApp<DreamShareView>,
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

	// Handle Interface
	RESULT ShowLoadingTexture();
	RESULT ShowCastingTexture();
	RESULT SetCastingTexture(std::shared_ptr<texture> pNewCastTexture);
	std::shared_ptr<texture> GetCastingTexture();
	RESULT Show();
	RESULT Hide();

	// Casting/Sharing
	RESULT StartReceiving(PeerConnection *pPeerConnection);
	RESULT PendReceiving();
	RESULT StopReceiving();
	RESULT StopSending();
	RESULT IsReceivingStream(bool &fReceivingStream);
	RESULT HandleStopEvent();

	// App Messaging
	RESULT BeginStream();
	RESULT BroadcastDreamShareViewMessage(DreamShareViewMessage::type msgType, DreamShareViewMessage::type ackType = DreamShareViewMessage::type::INVALID);

	bool IsStreaming();
	RESULT SetStreamingState(bool fStreaming);

	RESULT BroadcastAudioPacket(const AudioPacket &pendingAudioPacket);

	// Video Stream Subscriber
	RESULT BroadcastVideoFrame(const void *pBuffer, int width, int height);
	virtual RESULT OnVideoFrame(PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override;
	RESULT SetupPendingVideoFrame(uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight);
	RESULT UpdateFromPendingVideoFrame();

	// Audio
	RESULT HandleChromeAudioDataMessage(PeerConnection* pPeerConnection, AudioDataMessage *pAudioDataMessage);

	// Environment
	RESULT UpdateScreenPosition(point ptPosition, quaternion qOrientation, float scale);

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
	std::shared_ptr<quad> m_pCastBackgroundQuad = nullptr;

	std::shared_ptr<texture> m_pCastTexture = nullptr;
	std::shared_ptr<texture> m_pCastBackgroundTexture = nullptr;
	std::shared_ptr<texture> m_pVideoCastTexture = nullptr;
	std::shared_ptr<texture> m_pLoadingTexture = nullptr;

	int m_castpxWidth = BROWSER_WIDTH;
	int m_castpxHeight = BROWSER_HEIGHT;
	float m_aspectRatio;
	float m_diagonalSize = 9.0f;
	float m_borderWidth = BORDER_WIDTH;
	float m_borderHeight = BORDER_HEIGHT;
	float m_spacingSize = SPACING_SIZE;

	float m_bottomBarHeight = 0.05376f;

	// Streaming members
	bool m_fStreaming = false;
	bool m_fReceivingStream = false;
	bool m_fShouldBeginStream = false;
	bool m_fReadyForFrame = false;

	// Dream app message members
	DreamShareViewMessage::type m_currentMessageType;
	DreamShareViewMessage::type m_currentAckType;

	DreamUserHandle* m_pDreamUserHandle = nullptr;

	std::shared_ptr<SpatialSoundObject> m_pSpatialBrowserObject = nullptr;

private:
//	std::shared_ptr<UIView> 

};

#endif // ! DREAM_SHARE_VIEW_H_ 
