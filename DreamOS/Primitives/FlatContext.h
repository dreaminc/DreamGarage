#ifndef FLAT_CONTEXT_H_
#define FLAT_CONTEXT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/FlatContext.h

#include "Primitives/quad.h"
#include "Primitives/composite.h"

class font;
class text;
class framebuffer;

class FlatContext : public composite {
public:
	FlatContext(HALImp *pHALImp);

	std::shared_ptr<quad> MakeQuad(double width, double height, point ptOrigin = point(0.0f));
	std::shared_ptr<quad> MakeQuad(double width, double height, point ptOrigin, uvcoord uvTopLeft, uvcoord uvBottomRight, vector vNormal = vector::jVector());

	std::shared_ptr<quad> AddQuad(double width, double height, point ptOrigin = point(0.0f));
	std::shared_ptr<quad> AddQuad(double width, double height, point ptOrigin, uvcoord uvTopLeft, uvcoord uvBottomRight, vector vNormal = vector::jVector());

	std::shared_ptr<text> MakeText(std::shared_ptr<font> pFont, texture *pFontTexture, const std::string& strContent, double size, bool fDistanceMap = false);
	std::shared_ptr<text> AddText(std::shared_ptr<font> pFont, texture *pFontTexture, const std::string& content, double size, bool fDistanceMap);

	RESULT RenderToTexture();

	// TODO: This clobbers the rest of thing, might want to move text to quad and 
	// have a flat context internally or something like that
	RESULT RenderToQuad(quad::CurveType curveType = quad::CurveType::FLAT);
	RESULT RenderToQuad(quad* pRenderQuad, float xOffset, float yOffset);

	RESULT RenderToQuad(float width, float height, float xOffset, float yOffset, quad::CurveType curveType);

	float GetWidth();
	float GetHeight();
	float GetLeft(bool fAbsolute = true);
	float GetRight(bool fAbsolute = true);
	float GetTop(bool fAbsolute = true);
	float GetBottom(bool fAbsolute = true);

	// Move flags up to Flatcontext?
	virtual bool IsScaleToFit();
	virtual RESULT SetScaleToFit(bool fScaleToFit);

	RESULT SetAbsoluteBounds(float width, float height);
	RESULT SetBounds(float width, float height);

	RESULT SetIsAbsolute(float fAbsolute);
	bool UseVirtualModelMatrix();

	matrix<virtual_precision, 4, 4> GetModelMatrix(matrix<virtual_precision, 4, 4> childMat = matrix<virtual_precision, 4, 4>(1.0f));

public:
	framebuffer* GetFramebuffer();
	RESULT SetFramebuffer(framebuffer* pFramebuffer);

protected:
	framebuffer* m_pFramebuffer = nullptr;

	bool m_fScaleToFit = false;
	bool m_fAbsolute = false;

	float m_width;
	float m_height;

	//hack used for text rendering, skips DimObj::GetModelMatrix() in OGLProgramFlat
	bool m_fVirtualModelMatrix = false;

	float m_xOffset;
	float m_yOffset;

	std::shared_ptr<quad> m_pQuad = nullptr;
};

#endif	// ! FLAT_CONTEXT_H_
