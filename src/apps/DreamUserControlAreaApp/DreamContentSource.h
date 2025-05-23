#ifndef DREAM_CONTENT_SOURCE_H_
#define DREAM_CONTENT_SOURCE_H_

#include "core/ehm/EHM.h"

// Dream Content Source
// dos/src/app/DreamUserControlAreaApp/DreamContentSource.h

#include <memory>

#define CONTENT_TYPE_BROWSER "ContentControlType.Website"
#define CONTENT_TYPE_CAMERA "ContentControlType.Camera"
#define CONTENT_TYPE_DESKTOP "ContentControlType.Desktop"
#define CONTENT_TYPE_DEFAULT ""	// conforms with the string UIControlBar is expecting

#define SHARE_TYPE_SCREEN "EnvironmentShareType.Screen"
#define SHARE_TYPE_CAMERA "EnvironmentShareType.Camera"

class texture;
class point;

class DreamContentSource {

public:
	virtual RESULT OnClick(point ptDiff, bool fMouseDown) = 0;
	virtual RESULT OnMouseMove(point mousePoint) = 0;
	virtual RESULT OnScroll(float pxXDiff, float pxYDiff, point scrollPoint) = 0;

	virtual RESULT OnKeyPress(char chkey, bool fkeyDown) = 0;

	virtual texture* GetSourceTexture() = 0;

	virtual RESULT SetScope(std::string strScope) = 0;
	virtual RESULT SetPath(std::string strPath) = 0;
	virtual long GetCurrentAssetID() = 0;

	virtual RESULT SendFirstFrame() = 0;
	virtual RESULT CloseSource() = 0;
	
	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;

	virtual std::string GetTitle() = 0;
	virtual std::string GetContentType() = 0;

	virtual std::string GetScheme() = 0;
	virtual std::string GetURL() = 0;
};

#endif // ! DREAM_CONTENT_SOURCE_H_
