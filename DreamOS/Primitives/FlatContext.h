#ifndef FLAT_CONTEXT_H_
#define FLAT_CONTEXT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/FlatContext.h

#include "Primitives/composite.h"

class font;
class text;
class framebuffer;

class FlatContext : public composite {
public:
	FlatContext(HALImp *pHALImp);

	std::shared_ptr<quad> MakeQuad(double width, double height, point ptOrigin);
	std::shared_ptr<quad> MakeQuad(double width, double height, point ptOrigin, uvcoord uvTopLeft, uvcoord uvBottomRight, vector vNormal = vector::jVector());

	std::shared_ptr<quad> AddQuad(double width, double height, point ptOrigin);
	std::shared_ptr<quad> AddQuad(double width, double height, point ptOrigin, uvcoord uvTopLeft, uvcoord uvBottomRight, vector vNormal = vector::jVector());

	std::shared_ptr<text> MakeText(std::shared_ptr<font> pFont, texture *pFontTexture, const std::string& strContent, double size, bool fDistanceMap = false);
	std::shared_ptr<text> AddText(std::shared_ptr<font> pFont, texture *pFontTexture, const std::string& content, double size, bool fDistanceMap);

	RESULT RenderToTexture();

	float GetWidth();
	float GetHeight();
	float GetLeft(bool fAbsolute = true);
	float GetRight(bool fAbsolute = true);
	float GetTop(bool fAbsolute = true);
	float GetBottom(bool fAbsolute = true);

public:
	framebuffer* GetFramebuffer();
	RESULT SetFramebuffer(framebuffer* pFramebuffer);

protected:
	framebuffer* m_pFramebuffer = nullptr;
};

#endif	// ! FLAT_CONTEXT_H_
