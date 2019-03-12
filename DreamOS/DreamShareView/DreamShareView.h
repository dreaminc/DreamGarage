#ifndef DREAM_SHARE_VIEW_H_
#define DREAM_SHARE_VIEW_H_

#include "RESULT/EHM.h"

#include "DreamApp.h"
#include "DreamAppHandle.h"
#include "DreamVideoStreamSubscriber.h"
#include "DreamShareViewShareMessage.h"
#include "DreamShareViewPointerMessage.h"
#include "DreamUserApp.h"

#include "DreamGarage/UICommon.h"

class quad;
class texture;
class color;
class font;
class text;
class FlatContext;

class AudioPacket;
class SpatialSoundObject;
class AudioDataMessage;
class UIView;
class UIPointerLabel;

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

private:
	RESULT HandleShareMessage(PeerConnection* pPeerConnection, DreamShareViewShareMessage *pShareMessage);
	RESULT HandlePointerMessage(PeerConnection* pPeerConnection, DreamShareViewPointerMessage *pUpdatePointerMessage);

public:
	// Handle Interface
	RESULT ShowLoadingTexture();
	RESULT ShowCastingTexture();
	RESULT SetCastingTexture(texture* pNewCastTexture);
	texture* GetCastingTexture();
	texture* GetPointingTexture();
	RESULT Show();
	RESULT Hide();

	// Casting/Sharing
	RESULT StartReceiving(PeerConnection *pPeerConnection);
	RESULT PendReceiving();
	RESULT StopReceiving();
	RESULT StopSending();
	RESULT HandleStopEvent();

	// App Messaging
	RESULT BeginStream();
	RESULT BroadcastDreamShareViewMessage(DreamShareViewShareMessage::type msgType, DreamShareViewShareMessage::type ackType = DreamShareViewShareMessage::type::INVALID);

	bool IsStreaming();
	RESULT SetStreamingState(bool fStreaming);

	RESULT IsReceivingStream(bool &fReceivingStream);
	RESULT SetReceivingState(bool fReceiving);

	RESULT SendDOSMessage();

	RESULT BroadcastAudioPacket(const AudioPacket &pendingAudioPacket);

	PeerConnection *GetStreamingPeerConnection();

	// Video Stream Subscriber
	RESULT BroadcastVideoFrame(const void *pBuffer, int width, int height);
	virtual RESULT OnVideoFrame(const std::string &strVideoTrackLabel, PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override;
	RESULT SetupPendingVideoFrame(uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight);
	RESULT UpdateFromPendingVideoFrame();

	// Audio
	RESULT HandleChromeAudioDataMessage(PeerConnection* pPeerConnection, AudioDataMessage *pAudioDataMessage);

	// Environment
	RESULT UpdateScreenPosition(point ptPosition, quaternion qOrientation, float scale);

	// Pointing
	RESULT AllocateSpheres(long userID);
	RESULT AllocateSpheres(long userID, std::string strInitials);
	RESULT DeallocateSpheres(long userID);

	struct PendingFrame {
		bool fPending = false;
		int pxWidth = 0;
		int pxHeight = 0;
		uint8_t *pDataBuffer = nullptr;
		size_t pDataBuffer_n = 0;
	} m_pendingFrame;

	PendingFrame m_overflowFrame;

protected:
	static DreamShareView* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	const wchar_t *k_wszLoadingScreen = L"client-loading-1366-768.png";

private:
	// Quad related members
	std::shared_ptr<quad> m_pCastQuad = nullptr;
	std::shared_ptr<quad> m_pCastBackgroundQuad = nullptr;
	std::shared_ptr<quad> m_pMirrorQuad = nullptr;

	std::shared_ptr<composite> m_pCastQuadComposite = nullptr;
	std::shared_ptr<composite> m_pCastBackgroundQuadComposite = nullptr;

	texture* m_pCastTexture = nullptr;
	std::shared_ptr<texture> m_pCastBackgroundTexture = nullptr;
	std::shared_ptr<texture> m_pVideoCastTexture = nullptr;
	std::shared_ptr<texture> m_pLoadingTexture = nullptr;

	texture* m_pPointingTexture = nullptr;

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

	// flag represents m_fStreaming || m_fReceivingStream
	bool m_fIsActive = false;

	// Dream app message members
	DreamShareViewShareMessage::type m_currentMessageType;
	DreamShareViewShareMessage::type m_currentAckType;

	std::shared_ptr<SpatialSoundObject> m_pSpatialBrowserObject = nullptr;
	PeerConnection *m_pStreamerPeerConnection = nullptr;

	// Pointing members
	std::map<long, std::vector<std::shared_ptr<UIPointerLabel>>> m_pointingObjects; // user id to left/right sphere

	std::queue<std::shared_ptr<UIPointerLabel>> m_pointerViewPool;

	composite *m_pPointerComposite = nullptr;
	std::shared_ptr<FlatContext> m_pPointerContext = nullptr;
};

#endif // ! DREAM_SHARE_VIEW_H_ 
