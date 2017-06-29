#include "FlatContext.h"
#include "HAL/HALImp.h"

#include "Primitives/text.h"
#include "Primitives/framebuffer.h"

FlatContext::FlatContext(HALImp * pHALImp) :
	composite(pHALImp)
{
	// TODO add UI capabilities (alignments, etc)
}

std::shared_ptr<quad> FlatContext::MakeQuad(double width, double height, point ptOrigin) {
	RESULT r = R_PASS;

	std::shared_ptr<quad> pQuad(m_pHALImp->MakeQuad(width, height, ptOrigin));

//Success:
	return pQuad;

//Error:
	return nullptr;
}

std::shared_ptr<quad> FlatContext::AddQuad(double width, double height, point ptOrigin) {
	RESULT r = R_PASS;

	std::shared_ptr<quad> pQuad = MakeQuad(width, height, ptOrigin);
	CR(AddObject(pQuad));

//Success:
	return pQuad;

Error:
	return nullptr;
}

std::shared_ptr<quad> FlatContext::MakeQuad(double width, double height, point ptOrigin, uvcoord uvTopLeft, uvcoord uvBottomRight, vector vNormal) {
	RESULT r = R_PASS;

	std::shared_ptr<quad> pQuad(m_pHALImp->MakeQuad(width, height, ptOrigin, uvTopLeft, uvBottomRight, vNormal));

	//Success:
	return pQuad;

	//Error:
	return nullptr;
}

std::shared_ptr<quad> FlatContext::AddQuad(double width, double height, point ptOrigin, uvcoord uvTopLeft, uvcoord uvBottomRight, vector vNormal) {
	RESULT r = R_PASS;

	std::shared_ptr<quad> pQuad = MakeQuad(width, height, ptOrigin, uvTopLeft, uvBottomRight, vNormal);
	CR(AddObject(pQuad));

	//Success:
	return pQuad;

Error:
	return nullptr;
}

std::shared_ptr<text> FlatContext::MakeText(std::shared_ptr<font> pFont, texture *pFontTexture, const std::string& content, double size, bool fDistanceMap) {
	RESULT r = R_PASS;

	std::shared_ptr<text> pText(m_pHALImp->MakeText(pFont, pFontTexture, content, size, fDistanceMap));
	return pText;
}

std::shared_ptr<text> FlatContext::AddText(std::shared_ptr<font> pFont, texture *pFontTexture, const std::string& content, double size, bool fDistanceMap) {
	RESULT r = R_PASS;

	std::shared_ptr<text> pText = MakeText(pFont, pFontTexture, content, size, fDistanceMap);
	CR(AddObject(pText));

//Success:
	return pText;

Error:
	return nullptr;
}

RESULT FlatContext::RenderToTexture() {
	RESULT r = R_PASS;

	CR(m_pHALImp->RenderToTexture(this, GetCamera()));

Error:
	return r;
}

framebuffer* FlatContext::GetFramebuffer() {
	return m_pFramebuffer;
}

RESULT FlatContext::SetFramebuffer(framebuffer* pFramebuffer) {
	m_pFramebuffer = pFramebuffer;
	return R_PASS;
}
