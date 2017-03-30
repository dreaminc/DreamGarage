#ifndef WEB_BROWSER_CONTROLLER_H_
#define WEB_BROWSER_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/CEFBrowser/WebBrowserController.h

#include <memory>
#include <functional>

// TODO: Revisit these functions 
class WebBrowserController {
public:
	// Poll for the current frame of the browser.
	// This function can be called by any thread.
	//virtual RESULT PollFrame(std::function<bool(unsigned char *output, unsigned int width, unsigned int height)> pred) = 0;

	// Get the new dirty frames since last time they were polled.
	// returns the number of new dirty frame.
	// This function can be called by any thread.
	virtual int PollNewDirtyFrames() = 0;

	// Resize the browser.
	virtual RESULT Resize(unsigned int width, unsigned int height) = 0;

	// Loads a url
	virtual RESULT LoadURL(const std::string& url) = 0;

	// Sending a key sequence one at a time from the collection of chars in keys
	virtual RESULT SendKeySequence(const std::string& keys) = 0;

	std::string GetID() {
		return m_strID;
	}

private:
	std::string m_strID;
};

#endif // !WEB_BROWSER_CONTROLLER_H_