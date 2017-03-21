#ifndef BROWSER_H_
#define BROWSER_H_

// DREAM OS
// DreamOS/Cloud/WebBrowser/browser.h
// 

#include "RESULT/EHM.h"
#include <memory>
#include <functional>

class WebBrowserController {
public:
	// Poll for the current frame of the browser.
	// This function can be called by any thread.
	virtual void PollFrame(std::function<bool(unsigned char *output, unsigned int width, unsigned int height)> pred) = 0;

	// Get the new dirty frames since last time they were polled.
	// returns the number of new dirty frame.
	// This function can be called by any thread.
	virtual int PollNewDirtyFrames(std::function<bool(unsigned char *output, unsigned int width, unsigned int height, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom)> pred) = 0;

	// Resize the browser.
	virtual void Resize(unsigned int width, unsigned int height) = 0;

	// Loads a url
	virtual void LoadURL(const std::string& url) = 0;

	// Sending a key sequence one at a time from the collection of chars in keys
	virtual void SendKeySequence(const std::string& keys) = 0;
};

class WebBrowserService {
public:
	// initialize service
	virtual RESULT Initialize() = 0;

	// creates new web browser
	virtual WebBrowserController* CreateNewWebBrowser(const std::string& url, unsigned int width, unsigned int height) = 0;

	virtual ~WebBrowserService() = 0;
}; 

namespace WebBrowser {
	// creates new web browser service
	// returns nullptr when fails
	std::unique_ptr<WebBrowserService> CreateNewCefBrowserService();
}

#endif // !BROWSER_H_