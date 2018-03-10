#ifndef DREAM_CONTENT_SOURCE_H_
#define DREAM_CONTENT_SOURCE_H_

#include "RESULT/EHM.h"

#include <memory>

class texture;
class point;

class DreamContentSource {

public:
	virtual RESULT OnClick(point ptDiff, bool fMouseDown) = 0;
	virtual RESULT OnMouseMove(point mousePoint) = 0;
	virtual RESULT OnScroll(float pxXDiff, float pxYDiff, point scrollPoint) = 0;

	virtual RESULT OnKeyPress(char chkey, bool fkeyDown) = 0;

	virtual std::shared_ptr<texture> GetSourceTexture() = 0;

	virtual RESULT SetScope(std::string strScope) = 0;
	virtual RESULT SetPath(std::string strPath) = 0;
	virtual long GetCurrentAssetID() = 0;

	virtual RESULT CloseSource() = 0;
	
	virtual int GetPXWidth() = 0;
	virtual int GetPXHeight() = 0;
};

#endif // ! DREAM_CONTENT_SOURCE_H_
