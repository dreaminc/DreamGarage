#ifndef DREAM_DESKTOP_H_
#define DREAM_DESKTOP_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamGarage/DreamApp.h
// The Dream Content view is a rudimentary Dream application 
// that effectively is a single quad that can load / present 
// content of various formats 

#include "DreamApp.h"
#include "DreamAppHandle.h"

#include "Primitives/Subscriber.h"
#include "InteractionEngine/InteractionObjectEvent.h"

#include <map>
#include <vector>

#include "WebBrowser/WebBrowserController.h"

#include "Sense/SenseController.h"

#include "Primitives/TextEntryString.h"

#include "DreamVideoStreamSubscriber.h"

#define DEFAULT_SCROLL_FACTOR 5

class quad;
class sphere;
class texture;

class EnvironmentAsset;
class WebBrowserManager;
class DOMNode;
class DreamUserHandle;
class AudioPacket;

#include "DreamBrowserMessage.h"

class DreamDesktopApp :
	public DreamApp<DreamDesktopApp>,
	public Subscriber<InteractionObjectEvent>,
	public WebBrowserController::observer,
	public DreamVideoStreamSubscriber
{
	friend class DreamAppManager;

public:
	DreamDesktopApp(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamDesktopApp();

	// DreamApp Interface
	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	virtual RESULT HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage) override;

	// Set streaming state in both the browser and the user app
	RESULT SetStreamingState(bool fStreaming);
	bool IsStreaming();

	RESULT BroadcastDreamBrowserMessage(DreamBrowserMessage::type msgType, DreamBrowserMessage::type ackType = DreamBrowserMessage::type::INVALID);

	// InteractionObjectEvent
	virtual RESULT Notify(InteractionObjectEvent *pEvent) override;

	// WebBrowserController Observer
	virtual RESULT OnPaint(const WebBrowserRect &rect, const void *pBuffer, int width, int height) override;
	virtual RESULT OnAudioPacket(const AudioPacket &pendingAudioPacket) override;
	virtual RESULT OnLoadingStateChange(bool fLoading, bool fCanGoBack, bool fCanGoForward, std::string strCurrentURL) override;
	virtual RESULT OnLoadStart() override;
	virtual RESULT OnLoadEnd(int httpStatusCode, std::string strCurrentURL) override;
	virtual RESULT OnNodeFocusChanged(DOMNode *pDOMNode) override;

	RESULT SetPosition(point ptPosition);
	RESULT SetAspectRatio(float aspectRatio);
	RESULT SetDiagonalSize(float diagonalSize);
	RESULT SetNormalVector(vector vNormal);
	RESULT SetParams(point ptPosition, float diagonal, float aspectRatio, vector vNormal);

	RESULT FadeQuadToBlack();

	float GetWidth();
	float GetHeight();
	vector GetNormal();
	point GetOrigin();
//	virtual float GetAspectRatio() override;

	RESULT UpdateViewQuad();

	bool IsVisible();
	RESULT SetVisible(bool fVisible);

	RESULT SetEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);
	RESULT StopSending();
	RESULT StartReceiving();
	RESULT StopReceiving();

	RESULT LoadRequest(const WebRequest &webRequest);

	RESULT SetScrollFactor(int scrollFactor);

	std::shared_ptr<texture> GetScreenTexture();
private:
	RESULT SetScreenTexture(texture *pTexture);

public:
	// Video Stream Subscriber
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
	static DreamDesktopApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::shared_ptr<quad> m_pBrowserQuad = nullptr;
	std::shared_ptr<texture> m_pBrowserTexture = nullptr;

	std::shared_ptr<WebBrowserController> m_pWebBrowserController = nullptr;

	DreamUserHandle* m_pDreamUserHandle = nullptr;

	int m_DesktopWidth = 1366;
	int m_DesktopHeight = 768;
	float m_aspectRatio = 1.0f;
	float m_diagonalSize = 5.0f;
	vector m_vNormal;

	bool m_fStreaming = false;
	bool m_fReceivingStream = false;
};

#endif // ! DREAM_CONTENT_VIEW_H_

