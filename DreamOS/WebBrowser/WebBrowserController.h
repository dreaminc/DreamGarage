#ifndef WEB_BROWSER_CONTROLLER_H_
#define WEB_BROWSER_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/WebBrowser/WebBrowser.h

#include <memory>
#include <functional>

#include <vector>

class DOMNode;
class DOMDocument;
class AudioPacket;

struct WebBrowserPoint {
	int x;
	int y;
};

struct WebBrowserRect {
	WebBrowserPoint pt;
	int width;
	int height;
};

struct WebBrowserMouseEvent {
	enum MOUSE_BUTTON {
		LEFT,
		MIDDLE,
		RIGHT,
		INVALID
	};

	MOUSE_BUTTON mouseButton;

	// Relative to top left of browser
	WebBrowserPoint pt;
};

class WebRequest;
class DOMNode;

// TODO: Revisit these functions 
class WebBrowserController {
public:
	enum class PAINT_ELEMENT_TYPE {
		PET_VIEW = 0,
		PET_POPUP,
	};

public:
	class observer {
	public:
		virtual RESULT OnPaint(const WebBrowserRect &rect, const void *pBuffer, int width, int height) = 0;
		virtual RESULT OnAudioPacket(const AudioPacket &pendingAudioPacket) = 0;
		virtual RESULT OnLoadingStateChange(bool fLoading, bool fCanGoBack, bool fCanGoForward) = 0;
		virtual RESULT OnLoadStart() = 0;
		virtual RESULT OnLoadEnd(int httpStatusCode) = 0;
		virtual RESULT OnNodeFocusChanged(DOMNode *pDOMNode) = 0;
	};

public:
	RESULT RegisterWebBrowserControllerObserver(WebBrowserController::observer* pWebBrowserControllerObserver);

public:
	//virtual RESULT OnGetViewRect(WebBrowserRect &webRect) = 0;
	//virtual RESULT OnPaint(PAINT_ELEMENT_TYPE type, const std::vector<WebBrowserRect> &dirtyRects, const void *pBuffer, int width, int height) = 0;

	// Poll for the current frame of the browser.
	// This function can be called by any thread.
	//virtual RESULT PollFrame(std::function<bool(unsigned char *output, unsigned int width, unsigned int height)> pred) = 0;

	// Mouse
	virtual RESULT SendMouseClick(const WebBrowserMouseEvent& webBrowserMouseEvent, bool fMouseUp, int clickCount = 1) = 0;
	virtual RESULT SendMouseMove(const WebBrowserMouseEvent& webBrowserMouseEvent, bool fMouseLeave = false) = 0;
	virtual RESULT SendMouseWheel(const WebBrowserMouseEvent& webBrowserMouseEvent, int deltaX, int deltaY) = 0;

	// Keyboard
	virtual RESULT SendKeyEventChar(char chKey, bool fKeyDown) = 0;
	virtual RESULT SendKeySequence(const std::string& strKeySequence) = 0;

	// Page controls
	virtual void GoBack() = 0;
	virtual void GoForward() = 0;
	virtual bool CanGoBack() = 0;
	virtual bool CanGoForward() = 0;

	// Get the new dirty frames since last time they were polled.
	// returns the number of new dirty frame.
	// This function can be called by any thread.
	virtual RESULT PollNewDirtyFrames(int &rNumFramesProcessed) = 0;

	// Get the pending audio packets
	virtual RESULT PollPendingAudioPackets(int &numAudioPacketsProcessed) = 0;

	// Resize the browser.
	virtual RESULT Resize(unsigned int width, unsigned int height) = 0;

	// Loads a URL
	virtual RESULT LoadURL(const std::string& strURL) = 0;

	// Loads a Web Request
	virtual RESULT LoadRequest(const WebRequest &webRequest) = 0;

	virtual RESULT Shutdown() = 0;


	// frames
	virtual size_t GetFrameCount() = 0;

	// DOM
	//virtual RESULT GetFocusedNode() = 0;

	std::string GetID() {
		return m_strID;
	}

protected:
	WebBrowserController::observer* m_pWebBrowserControllerObserver = nullptr;

private:
	std::string m_strID;
};

#endif // !WEB_BROWSER_CONTROLLER_H_