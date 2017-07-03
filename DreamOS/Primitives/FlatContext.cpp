#include "FlatContext.h"
#include "HAL/HALImp.h"

#include "Primitives/text.h"
#include "Primitives/framebuffer.h"

FlatContext::FlatContext(HALImp * pHALImp) :
	composite(pHALImp)
{
	RESULT r = R_PASS;

	// TODO: add UI capabilities (alignments, etc)

	// TODO: Switch to quad, this should never have a third dimension
	//CR(InitializeOBB());
	CR(InitializeBoundingQuad());

	return;

Error:
	Invalidate();
	return;
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

	CR(m_pHALImp->RenderToTexture(this));

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

// TODO: Fix the sizing (not y) generality 
// This is to do with the normal, not not orientation 

float FlatContext::GetWidth() {
	vector vDiff = m_pBoundingVolume->GetMaxPoint() - m_pBoundingVolume->GetMinPoint();
	return vDiff.x();
}

float FlatContext::GetHeight() {
	vector vDiff = m_pBoundingVolume->GetMaxPoint() - m_pBoundingVolume->GetMinPoint();
	return vDiff.y();
}

float FlatContext::GetLeft() {
	return m_pBoundingVolume->GetMinPoint().x();
}

float FlatContext::GetRight() {
	return m_pBoundingVolume->GetMaxPoint().x();
}

float FlatContext::GetTop() {
	return m_pBoundingVolume->GetMaxPoint().y();
}

float FlatContext::GetBottom() {
	return m_pBoundingVolume->GetMinPoint().y();
}
