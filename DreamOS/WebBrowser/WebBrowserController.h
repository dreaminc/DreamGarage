#ifndef WEB_BROWSER_CONTROLLER_H_
#define WEB_BROWSER_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/CEFBrowser/WebBrowserController.h

#include <memory>
#include <functional>

#include <vector>

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

	// Get the new dirty frames since last time they were polled.
	// returns the number of new dirty frame.
	// This function can be called by any thread.
	virtual RESULT PollNewDirtyFrames(int &rNumFramesProcessed) = 0;

	// Resize the browser.
	virtual RESULT Resize(unsigned int width, unsigned int height) = 0;

	// Loads a url
	virtual RESULT LoadURL(const std::string& url) = 0;

	// Sending a key sequence one at a time from the collection of chars in keys
	virtual RESULT SendKeySequence(const std::string& keys) = 0;

	std::string GetID() {
		return m_strID;
	}

protected:
	WebBrowserController::observer* m_pWebBrowserControllerObserver = nullptr;

private:
	std::string m_strID;
};

#endif // !WEB_BROWSER_CONTROLLER_H_