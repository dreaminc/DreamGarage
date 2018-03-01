#ifndef DREAM_CONTENT_H_
#define DREAM_CONTENT_H_

#include "RESULT/EHM.h"

#include <memory>

struct WebBrowserPoint;
class texture;

class DreamContent {

	virtual RESULT ClickContent(WebBrowserPoint ptDiff, bool fMouseDown) = 0;
	virtual RESULT SendMouseMoveEvent(WebBrowserPoint mousePoint) = 0;
	virtual RESULT ScrollContentByDiff(int pxXDiff, int pxYDiff, WebBrowserPoint scrollPoint) = 0;

	virtual RESULT SendKeyPressed(char chkey, bool fkeyDown) = 0;

	virtual std::shared_ptr<texture> GetScreenTexture() = 0;

	virtual RESULT SetScope(std::string strScope) = 0;
	virtual RESULT SetPath(std::string strPath) = 0;

	virtual RESULT CloseContent() = 0;

};

#endif // ! DREAM_CONTENT_H_
