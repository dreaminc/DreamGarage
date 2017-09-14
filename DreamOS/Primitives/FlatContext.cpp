#include "FlatContext.h"
#include "HAL/HALImp.h"

#include "Primitives/text.h"
#include "Primitives/framebuffer.h"

#include "Primitives/BoundingQuad.h"

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

RESULT FlatContext::SetBounds(float width, float height) {
	return std::static_pointer_cast<BoundingQuad>(m_pBoundingVolume)->SetBounds(width, height);
}

float FlatContext::GetWidth() {
	return std::static_pointer_cast<BoundingQuad>(m_pBoundingVolume)->GetWidth();
}

float FlatContext::GetHeight() {
	return std::static_pointer_cast<BoundingQuad>(m_pBoundingVolume)->GetHeight();
}

// TODO: This is not general and will not work if the text is rotated at all
float FlatContext::GetLeft(bool fAbsolute) {
	return std::static_pointer_cast<BoundingQuad>(m_pBoundingVolume)->GetLeft(fAbsolute);
}

float FlatContext::GetRight(bool fAbsolute) {
	return std::static_pointer_cast<BoundingQuad>(m_pBoundingVolume)->GetRight(fAbsolute);
}

float FlatContext::GetTop(bool fAbsolute) {
	return std::static_pointer_cast<BoundingQuad>(m_pBoundingVolume)->GetTop(fAbsolute);
}

float FlatContext::GetBottom(bool fAbsolute) {
	return std::static_pointer_cast<BoundingQuad>(m_pBoundingVolume)->GetBottom(fAbsolute);
}

RESULT FlatContext::RenderToQuad(quad::CurveType curveType) {
	return RenderToQuad(GetWidth(), GetHeight(), 0.0f, 0.0f, curveType);
}

RESULT FlatContext::RenderToQuad(float width, float height, float xOffset, float yOffset, quad::CurveType curveType) {
	RESULT r = R_PASS;

	CR(RenderToTexture());

	CR(ClearChildren());

	// Remove quad if exists 
	if (m_pQuad != nullptr) {
		RemoveChild(m_pQuad);
		m_pQuad = nullptr;
	}

	{
		uvcoord uvTopLeft = uvcoord(0.0f, 0.0f);
		uvcoord uvBottomRight = uvcoord(1.0f, 1.0f);

		// We map the uvCoordinates per the height/width of the text object 
		// vs the bounding area
		float left = GetLeft();
		float right = GetRight();
		float top = GetTop();
		float bottom = GetBottom();

		float contextWidth = FlatContext::GetWidth();
		float contextHeight = FlatContext::GetHeight();

		if (!IsScaleToFit()) {
			float uvLeft = xOffset / contextWidth;
			float uvRight = (width + xOffset) / contextWidth;

			float uvTop = yOffset / contextHeight;
			float uvBottom = (height + yOffset) / contextHeight;

			uvTopLeft = uvcoord(uvLeft, uvTop);
			uvBottomRight = uvcoord(uvRight, uvBottom);
		}

		if (m_pQuad != nullptr) {
			m_pQuad = nullptr;
		}

		/*
		if (m_pBackgroundQuad != nullptr) {
		m_pBackgroundQuad->SetPosition(point(0.0f, 0.0f, 1.0f));
		AddChild(m_pBackgroundQuad);
		}
		*/

		// Add curved quads
		int divs = 100;


		//m_pQuad = AddQuad(m_width, m_height, point(0.0f, 0.0f, 0.0f), uvTopLeft, uvBottomRight, vector::jVector(1.0f));
		if (curveType == quad::CurveType::PARABOLIC) {
			m_pQuad = Add<quad>(width, height, divs, divs, uvTopLeft, uvBottomRight, quad::CurveType::PARABOLIC, vector::jVector(1.0f));
		}
		else if (curveType == quad::CurveType::CIRCLE) {
			m_pQuad = Add<quad>(width, height, divs, divs, uvTopLeft, uvBottomRight, quad::CurveType::CIRCLE, vector::jVector(1.0f));
		}
		else {
			m_pQuad = Add<quad>(width, height, point(0.0f, 0.0f, 0.0f), uvTopLeft, uvBottomRight, vector::jVector(1.0f));
		}

		CN(m_pQuad);
		CR(m_pQuad->SetDiffuseTexture(GetFramebuffer()->GetColorTexture()));
	}

Error:
	return r;
}