#include "text.h"
#include "quad.h"
#include <vector>
#include <algorithm>
#include <cctype>

#include "font.h"
#include "HAL/HALImp.h"
#include "Framebuffer.h"

#include "UI/UIKeyboardLayout.h"

#include "Core/Utilities.h"

text::text(HALImp *pHALImp, std::shared_ptr<font> pFont, text::flags textFlags) :
	FlatContext(pHALImp),
	m_width(1.0f),
	m_height(1.0f),
	m_pFont(pFont),
	m_flags(textFlags)
{
	RESULT r = R_PASS;

	Validate();
	return;
}

text::text(HALImp *pHALImp, std::shared_ptr<font> pFont, const std::string& strText, double width, double height, bool fBillboard) :
	FlatContext(pHALImp),
	m_width(width),
	m_height(height),
	m_pFont(pFont),
	m_flags(text::flags::NONE)
{
	RESULT r = R_PASS;

	// TODO: This should go into a factory method or something

	//CR(SetText(strText));

	Validate();
	return;
}

text::text(HALImp *pHALImp, std::shared_ptr<font> pFont, const std::string& strText, double lineHeightM, text::flags textFlags) :
	FlatContext(pHALImp),
	m_width(1.0f),
	m_height(1.0f),
	m_pFont(pFont),
	m_flags(textFlags)
{
	RESULT r = R_PASS;

	// TODO: This should go into a factory method or something
	CR(SetFontHeightM(lineHeightM));

	//CR(SetText(strText));

	Validate();
	return;

Error:
	Invalidate();
	return;
}

text::text(HALImp *pHALImp, std::shared_ptr<font> pFont, const std::string& strText, double width, double height, text::flags textFlags) :
	FlatContext(pHALImp),
	m_width(width),
	m_height(height),
	m_pFont(pFont),
	m_flags(textFlags)
{
	RESULT r = R_PASS;

	// TODO: This should go into a factory method or something
	CR(SetFontHeightM(pFont->GetLineHeight()));

	//CR(SetText(strText));

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

//	if (GetFramebuffer() != nullpt)
	//delete GetFramebuffer();


	ClearChildren();
}

RESULT text::RenderToQuad() {
	RESULT r = R_PASS;

	quad::CurveType curveType = quad::CurveType::FLAT;

	if (((m_flags & text::flags::CURVE_QUAD_PARABOLIC) != text::flags::NONE)) {
		curveType = quad::CurveType::PARABOLIC;
	}
	else if (((m_flags & text::flags::CURVE_QUAD_CIRCLE) != text::flags::NONE)) {
		curveType = quad::CurveType::CIRCLE;
	}

	// Render with the appropriate curve
	//CR(r);
	CR(FlatContext::RenderToQuad(m_width, m_height, m_xOffset, m_yOffset, curveType));

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

RESULT text::SetCursorIndex(int index) {
	m_cursorIndex = index;
	return R_PASS;
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

bool text::IsLeadingEllipsis() {
	return ((m_flags & text::flags::LEAD_ELLIPSIS) != text::flags::NONE);
}

bool text::IsPassword() {
	return ((m_flags & text::flags::PASSWORD) != text::flags::NONE);
}

bool text::IsUsingCursor() {
	return ((m_flags & text::flags::USE_CURSOR) != text::flags::NONE);
}

bool text::IsRenderToQuad() {
	return ((m_flags & text::flags::RENDER_QUAD) != text::flags::NONE);
}

bool text::CheckFlag(text::flags checkFlag) {
	return ((m_flags & checkFlag) != text::flags::NONE);
}

bool text::CheckFlagAgainstFlags(text::flags checkFlag, text::flags allFlags) {
	return ((allFlags & checkFlag) != text::flags::NONE);
}

RESULT text::AddFlags(text::flags newFlags) {
	m_flags = m_flags | newFlags;
	return R_PASS;
}

RESULT text::RemoveFlags(text::flags removeFlags) {
	m_flags = m_flags & (~(removeFlags));
	return R_PASS;
}

// Notes all values are in dots
std::shared_ptr<quad> text::AddGlyphQuad(CharacterGlyph glyph, float posX, float posY) {
	RESULT r = R_PASS;

	float fontImageWidth = static_cast<float>(m_pFont->GetFontTextureWidth());
	float fontImageHeight = static_cast<float>(m_pFont->GetFontTextureHeight());
	float fontBase = static_cast<float>(m_pFont->GetFontBase());
	float fontLineHeight = static_cast<float>(m_pFont->GetFontLineHeight());

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
	float glyphQuadYPosition = posY - ((float)(fontBase) - (float)(glyph.bearingY) - ((float)(glyph.height) / 2.0f));
	float glyphBgQuadYPosition = posY - (fontLineHeight - fontBase);

	// Apply DPMM and scale factor

	float glyphWidth = GetMSizeFromDots(glyph.width) * m_scaleFactor;
	float glyphHeight = GetMSizeFromDots(glyph.height) * m_scaleFactor;

	// TODO: Not entirely sure why, but some fonts require this fudge factor
	// due to offsets breaking the line height rules etc
	float glyphBgHeight = GetMSizeFromDots(fontLineHeight * 1.1f) * m_scaleFactor;
	
	if (glyphHeight > glyphBgHeight) {
		glyphHeight = glyphBgHeight;
	}

	glyphQuadXPosition = GetMSizeFromDots(glyphQuadXPosition) * m_scaleFactor;
	glyphQuadYPosition = GetMSizeFromDots(glyphQuadYPosition) * m_scaleFactor;
	glyphBgQuadYPosition = GetMSizeFromDots(glyphBgQuadYPosition) * m_scaleFactor;

	// Create a transparent background quad to ensure sizing 
	point ptGlyphBg = point(glyphQuadXPosition, 0.0f, glyphBgQuadYPosition);
	std::shared_ptr<quad> pQuadBg = AddQuad(glyphWidth, glyphBgHeight, ptGlyphBg, uvcoord(0.0f, 0.0f), uvcoord(0.0f, 0.0f));
	pQuadBg->SetDiffuseTexture(m_pFont->GetTexture().get());
	pQuadBg->SetPosition(ptGlyphBg);

	point ptGlyph = point(glyphQuadXPosition, 0.0f, glyphQuadYPosition);
	std::shared_ptr<quad> pQuad = AddQuad(glyphWidth, glyphHeight, ptGlyph, uvTopLeft, uvBottomRight);
	pQuad->SetDiffuseTexture(m_pFont->GetTexture().get());
	pQuad->SetPosition(ptGlyph);

	return pQuad;

	/*
Error:
	if (pQuad != nullptr) {
		pQuad = nullptr;
	}

	return nullptr;*/
}

RESULT text::CreateLayout(UIKeyboardLayout *pLayout, double marginRatio) {
	RESULT r = R_PASS;

	// Clear out kids
	CR(ClearChildren());

	//float posX = 0.0f;
	float posY = 0.0f;

	float width = 0.0f;
	//float height = 0.0f;

	float fontImageWidth = static_cast<float>(m_pFont->GetFontTextureWidth());
	float fontImageHeight = static_cast<float>(m_pFont->GetFontTextureHeight());
	float fontLineHeight = static_cast<float>(m_pFont->GetFontLineHeight());

	float rowHeight = pLayout->GetRowHeight();

	float effLineHeightM = GetMSizeFromDots(m_pFont->GetFontLineHeight());
	
	util::Clamp<double>(marginRatio, 0.0f, 0.5f);

	m_scaleFactor = (rowHeight / effLineHeightM) * (1.0f - marginRatio);

	// Create the layout in text form
	for (auto &layoutRow : pLayout->GetKeys()) {
		for (auto &pUIKey : layoutRow) {
			// Position
			float glyphQuadXPosition = pUIKey->m_left + (pUIKey->m_width / 2.0f);
			float glyphQuadYPosition = posY + (float)(rowHeight / 2.0f);
			//float glyphQuadYPosition = posY;
			
			if ((pUIKey->m_left + pUIKey->m_width) > width)
				width = (pUIKey->m_left + pUIKey->m_width);

			///*

			point ptGlyph = point(glyphQuadXPosition, 0.0f, glyphQuadYPosition);

			texture *pLayoutBGTexture = nullptr;
			if ((pLayoutBGTexture = pLayout->GetSpecialTexture(pUIKey->m_letter)) != nullptr) {
				std::shared_ptr<quad> pBgQuad = AddQuad(pUIKey->m_width, rowHeight, ptGlyph);
				pBgQuad->SetDiffuseTexture(pLayoutBGTexture);
			}
			else if ((pLayoutBGTexture = pLayout->GetKeyTexture()) != nullptr) {
				std::shared_ptr<quad> pBgQuad = AddQuad(pUIKey->m_width, rowHeight, ptGlyph);
				pBgQuad->SetDiffuseTexture(pLayoutBGTexture);
			}
			//*/

			CharacterGlyph glyph;
			if ( pUIKey->m_letter > 0x20) { 

				m_pFont->GetGlyphFromChar((char)(pUIKey->m_letter), glyph);
				//AddGlyphQuad(glyph, posX, posY);

				// Get UV values 
				float uvTop = (fontImageHeight - glyph.y) / fontImageHeight;
				float uvBottom = ((fontImageHeight - glyph.y) - glyph.height) / fontImageHeight;

				uvBottom = 1.0f - uvBottom;
				uvTop = 1.0f - uvTop;

				float uvLeft = glyph.x / fontImageWidth;
				float uvRight = (glyph.x + glyph.width) / fontImageWidth;

				uvcoord uvTopLeft = uvcoord(uvLeft, uvTop);
				uvcoord uvBottomRight = uvcoord(uvRight, uvBottom);

				// Size
				float glyphWidth = GetMSizeFromDots(glyph.width) * m_scaleFactor;
				float glyphHeight = GetMSizeFromDots(glyph.height) * m_scaleFactor;

				//TODO: account for bearingY like AddGlyphQuad
				// may be possible to use AddGlyphQuad
				std::shared_ptr<quad> pGlyphQuad = AddQuad(glyphWidth, glyphHeight, ptGlyph, uvTopLeft, uvBottomRight);
				pGlyphQuad->SetDiffuseTexture(m_pFont->GetTexture().get());

				//posX += (float)(glyph.advance);
			}
		}

		//posX = 0.0f;
		posY += rowHeight;
	}

	m_width = width;
	m_height = posY;

	FlatContext::SetBounds(m_width, m_height);

Error:
	return r;
}

RESULT text::AddCharacter(const std::string& strChar) {
	RESULT r = R_PASS;

	std::string strText;
	if (!IsUsingCursor()) {
		strText = m_strText;
		strText += strChar;
	}
	else {
		for (int i = 0; i < m_strText.size(); i++) {
			if (i == m_cursorIndex) {
				//TODO: add cursor asset
				strText += strChar;
			}
			strText += m_strText[i];
		}
//		m_strText = strText;
		m_cursorIndex += 1;
	}

	CR(SetText(strText));
Error:
	return r;
}

RESULT text::RemoveCharacter() {
	RESULT r = R_PASS;
	
	std::string strText;
	if (!IsUsingCursor()) {
		strText = m_strText;
		strText.pop_back();
	}
	else {
		for (int i = 0; i < m_strText.size(); i++) {
			if (i != m_cursorIndex - 1) {
				strText += m_strText[i];
			}
			else {
				m_cursorIndex -= 1;
			}
		}
//		m_strText = strText;
	}

	CR(SetText(strText));
Error:
	return r;
}

RESULT text::SetText(const std::string& strText) {
	RESULT r = R_PASS;
	point ptCenter;
	std::vector<std::shared_ptr<quad>> curWordQuads;
	std::vector<std::shared_ptr<quad>> curLineQuads;
	std::string strRender = strText;

	CBR((m_strText.compare(strText) != 0), R_NO_EFFECT);

	// Clear out kids
	CR(ClearChildren());

	m_fVirtualModelMatrix = true;

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

	if (IsPassword()) {
		// set m_strText to all * characters during rendering
		std::string strPassword;
		for (int i = 0; i < strText.size(); i++) {
			strPassword += "*";
		}
		strRender = strPassword;
	}

	if (IsLeadingEllipsis() && !IsWrap() && !IsTrailingEllipsis()) { // TODO: wrap 
		posX = GetDPM(m_width / m_scaleFactor);
		bool fEllipsisUsed = false;

		CharacterGlyph periodGlyph; 
		m_pFont->GetGlyphFromChar('.', periodGlyph);

		float periodGlyphWidth = GetMSizeFromDots(periodGlyph.width) * m_scaleFactor;
		float periodWidth = 0.0f;

		for (int i = (int)(strRender.size()) - 1; i >= 0; i--) {
			char &c = strRender[i];
			CharacterGlyph glyph;
			bool fInWord = false;

			if (m_pFont->GetGlyphFromChar(c, glyph) && !fEllipsisUsed) {

				posX -= (float)(glyph.advance);
				if (posX < (float)(periodGlyph.advance) * 3.0f) {
					posX += (float)(glyph.advance);
					for (int i = 0; i < 3; i++) {
						posX -= (float)(periodGlyph.advance);
						auto pPeriodQuad = AddGlyphQuad(periodGlyph, posX, posY);
						curLineQuads.push_back(pPeriodQuad);
					}
					fEllipsisUsed = true;
					break;
				}
				else {
					auto pQuad = AddGlyphQuad(glyph, posX, posY);
					curLineQuads.push_back(pQuad);
				}
			}
		}
		if (curLineQuads.size() > 0) {
			float left = curLineQuads.back()->GetPosition().x() - (curLineQuads.back()->GetWidth()/2.0f);
			for (auto pQuad : curLineQuads) {
				point ptPosition = pQuad->GetPosition();
				ptPosition.x() -= left;
				//ptPosition.x() += periodWidth * 3.0f;
				pQuad->SetPosition(ptPosition);
			}
		}
	}
	else {
		for (char &c : strRender) {
			CharacterGlyph glyph;
			bool fInWord = false;

			// This triggers a line break
			if (c == '\n') {
				posX = 0.0f;
				posY += fontLineHeight;
			}
			else if (m_pFont->GetGlyphFromChar(c, glyph)) {
				bool fQuitWrap = false;

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
				float posYM = (GetMSizeFromDots(posY) * m_scaleFactor);

				// Wrapping
				if (IsWrap() && posXM > m_width) {
					// Test for ellipsis 
					if (IsTrailingEllipsis() && (GetMSizeFromDots(posY + fontLineHeight * 2.0f) * m_scaleFactor) > m_height) {
						// TODO: Move to func
						AddTrailingEllipsisQuads(posX, posY, posXM, posYM, curLineQuads);

						break;
					}

					// Move to func
					if (curWordQuads.size() > 0) {
						// Convert back from meters to dots 
						if ((GetMSizeFromDots(fromStartOfWord) * m_scaleFactor) >= m_width) {
							// Give up - if ellipsis is on lower test will catch it and break
							// otherwise we give up
							fQuitWrap = true;
						}
						else {
							float xOffset = GetMSizeFromDots(toWord) * m_scaleFactor;
							float yOffset = GetMSizeFromDots(fontLineHeight) * m_scaleFactor;

							for (auto &pQuad : curWordQuads) {
								pQuad->translateX(-xOffset);
								pQuad->translateZ(yOffset);		// Note this is in Z because of flat context mechanics
							}

							posX = fromStartOfWord;
							toWord = 0.0f;
						}
					}

					if (fQuitWrap == false) {
						curLineQuads.clear();
						posY += fontLineHeight;
					}
				}

				posXM = (GetMSizeFromDots(posX) * m_scaleFactor);
				posYM = (GetMSizeFromDots(posY) * m_scaleFactor);

				// Ellipsis
				if (IsTrailingEllipsis() && (posXM > m_width)) {
					AddTrailingEllipsisQuads(posX, posY, posXM, posYM, curLineQuads);

					break;
				}

				// If wrapping has given up quit
				if (fQuitWrap) {
					break;
				}
			}
		}
	}

	if (IsFitToSize()) {
		m_width = FlatContext::GetWidth();
		m_height = FlatContext::GetHeight();
	}
	else {
		// TODO: 
	}

	if (m_pBackgroundQuad != nullptr) {
		CR(SetBackgroundColor(m_backgroundColor));
	}

	//m_width = maxRight - minLeft;
	//m_height = maxTop - minBottom;

	//ptCenter = point((minLeft + maxRight) / 2.0f, (maxTop + minBottom) / 2.0f, 0.0f);

Error:
	return r;
}

RESULT text::AddTrailingEllipsisQuads(float posX, float posY, float posXM, float posYM, std::vector<std::shared_ptr<quad>> curLineQuads) {
	RESULT r = R_PASS;

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

	return r;
}

RESULT text::SetBackgroundQuad() {
	RESULT r = R_PASS;

	if (m_pBackgroundQuad != nullptr) {
		RemoveChild(m_pBackgroundQuad);
		m_pBackgroundQuad = nullptr;
	}

	point ptContextCenter = FlatContext::GetBoundingVolume()->GetCenter();
	float contextWidth = std::abs(FlatContext::GetRight() - FlatContext::GetLeft());
	float contextHeight = std::abs(FlatContext::GetTop() - FlatContext::GetBottom());

	float width = std::max(m_width, contextWidth);
	float height = std::max(m_height, contextHeight);

	point ptQuadCenter = point(width / 2.0f, 0.0f, FlatContext::GetTop() - height / 2.0f);

	///*
	m_pBackgroundQuad = MakeQuad(width, height, ptQuadCenter);
	//*/
	//m_pBackgroundQuad = MakeQuad(FlatContext::GetWidth(), FlatContext::GetHeight(), ptQuadCenter);
	CN(m_pBackgroundQuad);

	AddChild(m_pBackgroundQuad, true);
	UpdateBoundingVolume();

Error:
	return r;
}

RESULT text::SetBackgroundColor(color backgroundColor) {
	RESULT r = R_PASS;

	m_backgroundColor = backgroundColor;

	CR(SetBackgroundQuad());
	CN(m_pBackgroundQuad);

	CR(m_pBackgroundQuad->SetVertexColor(m_backgroundColor));

Error:
	return r;
}

RESULT text::SetBackgroundColorTexture(texture *pColorTexture) {
	RESULT r = R_PASS;

	m_pBackgroundColorTexture = pColorTexture;

	CR(SetBackgroundQuad());
	CN(m_pBackgroundQuad);

	CR(m_pBackgroundQuad->SetDiffuseTexture(m_pBackgroundColorTexture));

Error:
	return r;
}
