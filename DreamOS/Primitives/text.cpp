#include "text.h"
#include "quad.h"
#include <vector>
#include <algorithm>
#include <cctype>

#include "font.h"
#include "HAL/HALImp.h"
#include "Framebuffer.h"


text::text(HALImp *pHALImp, std::shared_ptr<font> pFont, const std::string& strText, double width, double height, bool fBillboard) :
	FlatContext(pHALImp),
	m_pFont(pFont),
	m_width(width),
	m_height(height),
	m_flags(text::flags::NONE)
{
	RESULT r = R_PASS;

	// TODO: This should go into a factory method or something

	CR(SetText(strText));

	Validate();
	return;

Error:
	Invalidate();
	return;
}

text::text(HALImp *pHALImp, std::shared_ptr<font> pFont, const std::string& strText, double lineHeightM, text::flags textFlags) :
	FlatContext(pHALImp),
	m_pFont(pFont),
	m_width(1.0f),
	m_height(1.0f),
	m_flags(textFlags)
{
	RESULT r = R_PASS;

	// TODO: This should go into a factory method or something
	CR(SetFontHeightM(lineHeightM));

	CR(SetText(strText));

	Validate();
	return;

Error:
	Invalidate();
	return;
}

text::text(HALImp *pHALImp, std::shared_ptr<font> pFont, const std::string& strText, double width, double height, text::flags textFlags) :
	FlatContext(pHALImp),
	m_pFont(pFont),
	m_width(width),
	m_height(height),
	m_flags(textFlags)
{
	RESULT r = R_PASS;

	// TODO: This should go into a factory method or something
	CR(SetFontHeightM(pFont->GetLineHeight()));

	CR(SetText(strText));

	Validate();
	return;

Error:
	Invalidate();
	return;
}

text::~text() {
	if (m_pQuad != nullptr) {
		m_pQuad = nullptr;
	}

	ClearChildren();
}

RESULT text::RenderToQuad() {
	RESULT r = R_PASS;

	CR(RenderToTexture());

	CR(ClearChildren());

	if (m_pQuad == nullptr) {
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

		if(!IsScaleToFit()) {
			float uvLeft = m_xOffset / contextWidth;
			float uvRight = (m_width + m_xOffset) / contextWidth;

			float uvTop = m_yOffset / contextHeight;
			float uvBottom = (m_height + m_yOffset) / contextHeight;

			uvTopLeft = uvcoord(uvLeft, uvTop);
			uvBottomRight = uvcoord(uvRight, uvBottom);
		}

		m_pQuad = AddQuad(m_width, m_height, point(0.0f), uvTopLeft, uvBottomRight, vector::jVector(1.0f));
		
		CN(m_pQuad);
	}

	CR(m_pQuad->SetColorTexture(GetFramebuffer()->GetColorTexture()));

Error:
	return r;
}

VirtualObj* text::SetPosition(point pt, VerticalAlignment vAlign, HorizontalAlignment hAlign) {
	uv_precision xOffset = 0.0f;
	uv_precision yOffset = 0.0f;
	
	m_vAlign = vAlign;
	m_hAlign = hAlign;

	// TODO: Change this to use DimObj pivot
	switch (m_hAlign) {
		case HorizontalAlignment::LEFT: {
			xOffset = -m_width / 2;
		} break;

		case HorizontalAlignment::CENTER: {
			// nothing
		} break;

		case HorizontalAlignment::RIGHT: {
			xOffset = m_width / 2;
		} break;
	}

	switch (m_vAlign) {
		case VerticalAlignment::TOP: {
			yOffset = m_height / 2;
		} break;

		case VerticalAlignment::MIDDLE: {
			// nothing
		} break;

		case VerticalAlignment::BOTTOM: {
			yOffset = -m_height / 2;
		} break;
	}

	return this->MoveTo(pt.x() + xOffset, pt.y() + yOffset, pt.z());
}

std::string& text::GetText() {
	return m_strText;
}

std::shared_ptr<font> text::GetFont() { 
	return m_pFont; 
}

float text::GetWidth() {
	return m_width;
}

float text::GetHeight() {
	return m_height;
}

float text::GetDPMM(float mmVal) {
	return m_dpmm * mmVal;
}

float text::GetDPM(float mVal) {
	return m_dpmm * (1000.0f * mVal);
}

float text::GetMMSizeFromDots(float val) {
	return (val / m_dpmm);
}

float text::GetMSizeFromDots(float val) {
	return (val) / (m_dpmm * 1000.0f);
}

// TODO: Update everything
RESULT text::SetWidth(float width) {
	m_width = width;
	return R_PASS;
}

RESULT text::SetHeight(float height) {
	m_height = height;
	return R_PASS;
}

RESULT text::SetDPMM(float dpmm) {
	m_dpmm = dpmm;
	return R_PASS;
}

RESULT text::SetRows(int rows) {
	RESULT r = R_PASS;

	CB((rows > 0));
	m_rows = rows;

Error:
	return r;
}

RESULT text::SetOffset(float xOffset, float yOffset) {
	m_xOffset = xOffset;
	m_yOffset = yOffset;

	return R_PASS;
}


// This is currently a bit of a hack,
// however it will set the height of the text font
// in aspect of meters
RESULT text::SetFontHeightM(float mVal) {
	RESULT r = R_PASS;

	CN(m_pFont);

	{
		// We can use the DPMM to figure out the current meter height of the font
		float effLineHeightM = GetMSizeFromDots(m_pFont->GetFontLineHeight());
		m_scaleFactor = mVal / effLineHeightM;
	}

Error:
	return r;
}

RESULT text::SetFontHeightMM(float mmVal) {
	return SetFontHeightM(mmVal / 1000.0f);
}

// Scale to fit will scale the text to fit the size of the quad
// Note: This may induce warp distortion, so might want to split this
// into two different flags (ScaleToFitHeight and ScaleToFitWidth)

// Incompatible: fit to size
// Compatible: Wrap
RESULT text::SetScaleToFit(bool fScaleToFit) {
	RESULT r = R_PASS;

	if (fScaleToFit) {
		CBM((IsFitToSize() == false), "Scale to fit and fit to size are incompatible");
	}

	m_fScaleToFit = fScaleToFit;

Error:
	return r;
}

// TODO: Wrapping will wrap the text if outside of the width
// set for the object
RESULT text::SetWrap(bool fWrap) {
	RESULT r = R_PASS;

	//Error:
	return r;
}

// TODO: Fit to size will fit the quad to the 
// respective size of the text (fit all)

// Incompatible: Scale to fit
// Compatible: Wrap
RESULT text::SetFitToSize(bool fFitToSize) {
	RESULT r = R_PASS;

	//Error:
	return r;
}

// TODO: This will set whether or not the text is billboarded
RESULT text::SetBillboard(bool fBillboard) {
	return R_NOT_IMPLEMENTED;
}

bool text::IsScaleToFit() {
	return ((m_flags & text::flags::SCALE_TO_FIT) != text::flags::NONE);
}

bool text::IsWrap() {
	return ((m_flags & text::flags::WRAP) != text::flags::NONE);
}

bool text::IsFitToSize() {
	return ((m_flags & text::flags::FIT_TO_SIZE) != text::flags::NONE);
}

bool text::IsBillboard() {
	return ((m_flags & text::flags::BILLBOARD) != text::flags::NONE);
}

bool text::IsTrailingEllipsis() {
	return ((m_flags & text::flags::TRAIL_ELLIPSIS) != text::flags::NONE);
}

// Notes all values are in dots
std::shared_ptr<quad> text::AddGlyphQuad(CharacterGlyph glyph, float posX, float posY) {
	RESULT r = R_PASS;

	float fontImageWidth = static_cast<float>(m_pFont->GetFontTextureWidth());
	float fontImageHeight = static_cast<float>(m_pFont->GetFontTextureHeight());
	float fontBase = static_cast<float>(m_pFont->GetFontBase());

	// UV
	float uvTop = (fontImageHeight - glyph.y) / fontImageHeight;
	float uvBottom = ((fontImageHeight - glyph.y) - glyph.height) / fontImageHeight;

	uvBottom = 1.0f - uvBottom;
	uvTop = 1.0f - uvTop;

	float uvLeft = glyph.x / fontImageWidth;
	float uvRight = (glyph.x + glyph.width) / fontImageWidth;

	uvcoord uvTopLeft = uvcoord(uvLeft, uvTop);
	uvcoord uvBottomRight = uvcoord(uvRight, uvBottom);

	// Position
	float glyphQuadXPosition = posX + ((float)(glyph.width) / 2.0f) + (float)(glyph.bearingX);
	float glyphQuadYPosition = posY - ((float)(fontBase)-(float)(glyph.bearingY) - ((float)(glyph.height) / 2.0f));

	// Apply DPMM and scale factor

	float glyphWidth = GetMSizeFromDots(glyph.width) * m_scaleFactor;
	float glyphHeight = GetMSizeFromDots(glyph.height) * m_scaleFactor;

	glyphQuadXPosition = GetMSizeFromDots(glyphQuadXPosition) * m_scaleFactor;
	glyphQuadYPosition = GetMSizeFromDots(glyphQuadYPosition) * m_scaleFactor;

	point ptGlyph = point(glyphQuadXPosition, 0.0f, glyphQuadYPosition);
	std::shared_ptr<quad> pQuad = AddQuad(glyphWidth, glyphHeight, ptGlyph, uvTopLeft, uvBottomRight);
	pQuad->SetColorTexture(m_pFont->GetTexture().get());

	return pQuad;

	/*
Error:
	if (pQuad != nullptr) {
		pQuad = nullptr;
	}

	return nullptr;*/
}

RESULT text::SetText(const std::string& strText) {
	RESULT r = R_PASS;
	point ptCenter;
	std::vector<std::shared_ptr<quad>> curWordQuads;
	std::vector<std::shared_ptr<quad>> curLineQuads;

	CBR((m_strText.compare(strText) != 0), R_NO_EFFECT);

	// Clear out kids
	CR(ClearChildren());

	m_strText = strText;

	float fontLineHeight = static_cast<float>(m_pFont->GetFontLineHeight());

	// Apply DPMM to the width
	double effectiveDotsWidth = GetDPMM(m_width);
	double effectiveDotsHeight = GetDPMM(m_height);

	// These are in dots 
	float posX = 0.0f;
	float posY = 0.0f;
	float fromStartOfWord = 0.0f;
	float toWord = 0.0f;

	for(char &c : m_strText) {
		CharacterGlyph glyph;
		bool fInWord = false;

		// This triggers a line break
		if (c == '\n') {
			posX = 0.0f;
			posY += fontLineHeight;
		}
		else if (m_pFont->GetGlyphFromChar(c, glyph)) {
			
			if (std::isspace(c)) {
				curWordQuads.clear();
				posX += (float)(glyph.advance);
				toWord = posX;

				fromStartOfWord = 0.0f;
				continue;
			}

			auto pQuad = AddGlyphQuad(glyph, posX, posY);
			
			curWordQuads.push_back(pQuad);
			curLineQuads.push_back(pQuad);

			posX += (float)(glyph.advance);
			fromStartOfWord += (float)(glyph.advance);

			float posXM = (GetMSizeFromDots(posX) * m_scaleFactor);
			float posYM = (GetMSizeFromDots(posX) * m_scaleFactor);

			// Wrapping
			if (IsWrap() && posXM > m_width) {
				// Test for ellipsis 
				if (IsTrailingEllipsis() && (GetMSizeFromDots(posY + fontLineHeight * 2.0f) * m_scaleFactor) > m_height) {
					// TODO: Move to func
					CharacterGlyph periodGlyph;
					m_pFont->GetGlyphFromChar('.', periodGlyph);

					float periodGlyphWidth = GetMSizeFromDots(periodGlyph.width) * m_scaleFactor;

					// Remove characters so we have space for the ellipsis 
					while (posXM > (m_width - (periodGlyphWidth * 3.0f)) && posXM > 0.0f) {
						auto pQuad = curLineQuads.back();
						curLineQuads.pop_back();

						// Remove from flat context
						RemoveChild(pQuad);

						posXM = pQuad->GetPosition().x() - (pQuad->GetWidth() / 2.0f);
					}

					for (int i = 0; i < 3; i++) {
						auto pPeriodQuad = AddGlyphQuad(periodGlyph, posX, posY);

						// Adjust position
						point ptPeriod = pPeriodQuad->GetPosition();
						ptPeriod.x() = posXM + pPeriodQuad->GetWidth() / 2.0f;
						pPeriodQuad->SetPosition(ptPeriod);

						posXM += pPeriodQuad->GetWidth();
					}

					break;
				}

				// Move to func
				if ( curWordQuads.size() > 0 ) {
					float xOffset = GetMSizeFromDots(toWord) * m_scaleFactor;
					float yOffset = GetMSizeFromDots(fontLineHeight) * m_scaleFactor;

					for (auto &pQuad : curWordQuads) {
						pQuad->translateX(-xOffset);
						pQuad->translateZ(yOffset);		// Note this is in Z because of flat context mechanics
					}

					// Convert back from meters to dots 
					posX = fromStartOfWord;
					toWord = 0.0f;
				}

				curLineQuads.clear();
				posY += fontLineHeight;
			}

			posXM = (GetMSizeFromDots(posX) * m_scaleFactor);
			posYM = (GetMSizeFromDots(posX) * m_scaleFactor);

			// Ellipsis
			if (IsTrailingEllipsis() && (posXM > m_width)) {
				CharacterGlyph periodGlyph; 
				m_pFont->GetGlyphFromChar('.', periodGlyph);

				float periodGlyphWidth = GetMSizeFromDots(periodGlyph.width) * m_scaleFactor;

				// Remove characters so we have space for the ellipsis 
				while (posXM > (m_width - (periodGlyphWidth * 3.0f)) && posXM > 0.0f) {
					auto pQuad = curLineQuads.back();
					curLineQuads.pop_back();

					// Remove from flat context
					RemoveChild(pQuad);

					posXM = pQuad->GetPosition().x() - (pQuad->GetWidth()/2.0f);
				}

				for (int i = 0; i < 3; i++) {
					auto pPeriodQuad = AddGlyphQuad(periodGlyph, posX, posY);
					
					// Adjust position
					point ptPeriod = pPeriodQuad->GetPosition();
					ptPeriod.x() = posXM + pPeriodQuad->GetWidth() / 2.0f;
					pPeriodQuad->SetPosition(ptPeriod);

					posXM += pPeriodQuad->GetWidth();
				}

				break;
			}
		}
	}
	
	if (IsFitToSize()) {
		m_width = FlatContext::GetWidth();
		m_height = FlatContext::GetHeight();
	}

	//m_width = maxRight - minLeft;
	//m_height = maxTop - minBottom;

	//ptCenter = point((minLeft + maxRight) / 2.0f, (maxTop + minBottom) / 2.0f, 0.0f);

Error:
	return r;
}