#include "UIPointerLabel.h"
#include "DreamOS.h"

#include "DreamShareView/DreamShareView.h"

#include "Primitives/font.h"
#include "Primitives/text.h"
#include "Primitives/FlatContext.h"

#include "HAL/opengl/OGLText.h"

#include "Core/Utilities.h"

UIPointerLabel::UIPointerLabel(HALImp *pHALImp, DreamOS *pDreamOS) :
	UIView(pHALImp,pDreamOS)
{
	// empty
}

RESULT UIPointerLabel::Initialize() {
	RESULT r = R_PASS;

	PathManager *pPathManager = PathManager::instance();
	std::wstring wstrAssetPath;

	pPathManager->GetValuePath(PATH_ASSET, wstrAssetPath);
	m_pPointerLeft = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wszPointerLeftTexture)[0]);
	m_pPointerCenter = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wszPointerCenterTexture)[0]);
	m_pPointerRight = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wszPointerRightTexture)[0]);
	m_pPointerDot = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wszPointerDotTexture)[0]);

	m_pRenderContext = AddFlatContext();
	m_pRenderContext->RotateXByDeg(90.0f);
	m_pRenderContext->RotateYByDeg(-90.0f);
	m_pRenderContext->SetVisible(false, false);

	m_pFont = m_pDreamOS->MakeFont(L"Basis_Grotesque_Black.fnt", true);
	CN(m_pFont);

Error:
	return r;
}

UIPointerLabel::~UIPointerLabel() {
	// empty
}

RESULT UIPointerLabel::RenderLabelWithInitials(std::shared_ptr<quad> pParentQuad, std::string strInitials) {
	RESULT r = R_PASS;

	m_pParentQuad = pParentQuad;

	float height = pParentQuad->GetHeight()/2 * 0.06;
	float textHeight = 0.75f*height;
	float pxHeight = 65.0f;
	float pxRight = 10.0f;
	float pxLeft = 10.0f;
	float leftWidth = height * pxLeft / pxHeight;
	float rightWidth = height * pxRight / pxHeight;
	float pxDot = 178.0f;
	float dotHeight = height * pxDot / pxHeight;
	m_dotCenterOffset = height * 14.0f / pxHeight;

	m_pFont->SetLineHeight(textHeight);

	auto pText = std::shared_ptr<text>(m_pDreamOS->MakeText(
		m_pFont,
		strInitials,
		0.4,
		textHeight,
		text::flags::FIT_TO_SIZE | text::flags::RENDER_QUAD));

	CN(pText);

	m_pDotComposite = AddComposite();
	m_pDotComposite->SetVisible(false, false);

	m_pDotQuad = m_pDotComposite->AddQuad(dotHeight, dotHeight);
	m_pDotQuad->SetDiffuseTexture(m_pPointerDot);
	m_pDotQuad->RotateZByDeg(90.0f);
	{
		// assuming only capital letters (A-Z) and centering based on those
		CharacterGlyph periodGlyph;
		m_pFont->GetGlyphFromChar('A', periodGlyph);
		float glyphHeight = pText->GetMSizeFromDots(periodGlyph.height);
		float textOffset = (textHeight - glyphHeight) / 8.0f;


		// TODO: the text object should have access to the functionality of the update function
		auto oglText = dynamic_cast<OGLText*>(pText.get());
		oglText->Update();

		float width = pText->GetWidth();
		float totalWidth = leftWidth + width + rightWidth;
		//float totalWidth = leftWidth + width + leftWidth;
		float directionOffset = width/2.0f + leftWidth;

		float screenOffset = 0.01f;

		auto pQuadLeft = m_pRenderContext->AddQuad(leftWidth, height);
		auto pQuadCenter = m_pRenderContext->AddQuad(width, height);
		auto pQuadRight = m_pRenderContext->AddQuad(rightWidth, height);

		CN(pQuadLeft);
		CN(pQuadCenter);
		CN(pQuadRight);

		pQuadLeft->SetDiffuseTexture(m_pPointerLeft);
		pQuadCenter->SetDiffuseTexture(m_pPointerCenter);
		pQuadRight->SetDiffuseTexture(m_pPointerRight);

		pQuadLeft->SetPosition(-(width + leftWidth) / 2.0f, 0.0f, -textOffset);
		pQuadCenter->SetPosition(0.0f, 0.0f, -textOffset);
		pQuadRight->SetPosition((width + rightWidth) / 2.0f, 0.0f, -textOffset);

		pText->SetPosition(point(0.0f, 0.0f, 0.0f));

		m_pRenderContext->AddObject(pText);
		if (m_pRenderContext->GetCurrentQuad() != nullptr) {
			m_pRenderContext->GetCurrentQuad()->SetPosition(0.0f, 0.0f, 0.0f);
		}

		if (m_fPointingLeft) {
			m_pRenderContext->RenderToQuad(totalWidth, height, 0, 0);
		}
		else {
			m_pRenderContext->RenderToQuad(totalWidth, height, (leftWidth - rightWidth) / 2.0f, 0);
		}

		m_pDotComposite->SetPosition(point(-screenOffset * 2.0f, 0.0f, 0.0f));
		m_pRenderContext->SetPosition(point(-screenOffset, 0.0f, 0.0f));
	}

Error:
	return r;
}

RESULT UIPointerLabel::InitializeDot(std::shared_ptr<quad> pParentQuad, int seatPosition) {
	RESULT r = R_PASS;

	PathManager *pPathManager = PathManager::instance();
	std::wstring wstrAssetPath;
	std::wstring wstrSeatPosition;

	pPathManager->GetValuePath(PATH_ASSET, wstrAssetPath);

	// dot texture is a square
	float height = pParentQuad->GetHeight()/2 * 0.06;
	float screenOffset = 0.01f;

	CB(seatPosition != -1);
	m_seatingPosition = seatPosition;

	wstrSeatPosition = util::StringToWideString(std::to_string(m_seatingPosition));

	texture *pPointerDot = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wszPointerDotTexture + wstrSeatPosition + L".png")[0]);
	CN(pPointerDot);

	m_pParentQuad = pParentQuad;

	m_pDotComposite = AddComposite();
	CN(m_pDotComposite);
	m_pDotComposite->SetVisible(false, false);

	m_pDotQuad = m_pDotComposite->AddQuad(height, height);
	CN(m_pDotQuad);
	m_pDotQuad->SetDiffuseTexture(pPointerDot);
	m_pDotQuad->RotateZByDeg(90.0f);

	m_pDotComposite->SetPosition(point(-screenOffset * 2.0f, 0.0f, 0.0f));

Error:
	return r;
}

RESULT UIPointerLabel::RenderLabel() {
	return R_PASS;
}

RESULT UIPointerLabel::HandlePointerMessage(DreamShareViewPointerMessage *pUpdatePointerMessage) {
	RESULT r = R_PASS;

	CN(pUpdatePointerMessage);

	{
		point ptMessage = pUpdatePointerMessage->m_body.ptPointer;

		// smoothing
		float newAmount = 0.3f;
		ptMessage = (1.0f - newAmount) * GetPosition() + (newAmount)* ptMessage;

		// calculate position on the screen quad
		point ptPosition = (point)(inverse(RotationMatrix(m_pParentQuad->GetOrientation(true))) * (ptMessage - m_pParentQuad->GetOrigin(true)));
		float width = m_pParentQuad->GetWidth() * m_pParentQuad->GetScale(true).x();
		float height = m_pParentQuad->GetHeight() * m_pParentQuad->GetScale(true).y();
		std::string strInitials(pUpdatePointerMessage->m_body.szInitials, 2);

		// TODO: update for round implementation
		bool fInBounds = true;

		// left/right bounds check
		if (width / 2.0f - ptPosition.x() < 0 ||
			width / 2.0f + ptPosition.x() < 0) {
			fInBounds = false;
		}
		// bottom/top bounds check
		if (height / 2.0f - ptPosition.y() < m_pRenderContext->GetHeight()/2.0f ||
			height / 2.0f + ptPosition.y() < m_pRenderContext->GetHeight()/2.0f) {
			fInBounds = false;
		}
		m_pRenderContext->SetVisible(fInBounds && pUpdatePointerMessage->m_body.fVisible, false);
		m_pDotComposite->SetVisible(fInBounds && pUpdatePointerMessage->m_body.fVisible, false);

		SetPosition(ptMessage);

		// update saved points queue
		if (m_recentPoints.size() == 0 || ptPosition != m_recentPoints.back()) {
			m_recentPoints.push_back(ptPosition);
		}
		if (m_recentPoints.size() > NUM_POINTS) {
			m_recentPoints.pop_front();
		}

		// calculate orientation
		/*
		if (m_recentPoints.size() == NUM_POINTS) {
			UpdateOrientationFromPoints();
		}
		//*/
	}


Error:
	return r;
}

RESULT UIPointerLabel::UpdateOrientationFromPoints() {
	RESULT r = R_PASS;

	quaternion qRotation;
	vector vDirection;

	CBR(OrientationFromAverage(qRotation, vDirection), R_SKIPPED);

//	CBR(OrientationFromNormalEquation(qRotation), R_SKIPPED);

	vDirection.Normalize();
	vDirection = (m_pDotQuad->GetWidth()/2.0f-m_dotCenterOffset) * vDirection;
	vDirection = vector(-0.02f, -vDirection.y(), vDirection.x());

	m_pDotComposite->SetOrientation(qRotation);
	m_pDotComposite->SetPosition(vDirection);

	m_pRenderContext->SetPosition(vDirection);

Error:
	return r;
}

bool UIPointerLabel::OrientationFromAverage(quaternion& qRotation, vector &vDirection) {
	RESULT r = R_PASS;

	// average velocity, xdiff, ydiff
	float totalX = 0.0f;
	float totalY = 0.0f;

	float velocity = 0.0f;

	for (int i = (int)(m_recentPoints.size()) - 1; i > 0; i--) {
		m_recentPoints[i];
			
		float currentX = m_recentPoints[i].x() - m_recentPoints[i-1].x();
		float currentY = m_recentPoints[i].y() - m_recentPoints[i-1].y();

		totalX += currentX;
		totalY += currentY;

		velocity += sqrt(currentX * currentX + currentY * currentY);
	}

	totalX /= m_recentPoints.size();
	totalY /= m_recentPoints.size();
	velocity /= m_recentPoints.size();

	float theta = atan2(totalY, -totalX);

	CB(velocity > 0.025f);

	if (theta - m_currentAngle >= (float)(M_PI)) {
		m_currentAngle += (float)(2 * M_PI);
	}

	if (theta - m_currentAngle <= -(float)(M_PI)) {
		m_currentAngle -= (float)(2 * M_PI);
	}

	m_currentAngle = 0.1f * theta + 0.9f * m_currentAngle;

	{
		vDirection = vector(-totalX, totalY, 0.0f);
		float mag = vDirection.magnitude();
		vDirection = vector(mag*cos(m_currentAngle), mag*sin(m_currentAngle), 0.0f);
	}
	qRotation = quaternion::MakeQuaternionWithEuler(m_currentAngle, 0.0f, 0.0f);

	return true;
Error:
	return false;
}

bool UIPointerLabel::OrientationFromNormalEquation(quaternion& qRotation, vector& vDirection) {

	RESULT r = R_PASS;

	const unsigned int pts = NUM_POINTS;
	const unsigned int dims = 2;

	// linear normal equation matrices
	matrix<float, pts, dims> mA;
	matrix<float, pts, 1> mb;

	// populate matrices
	// linear
	int i = 0;
	for (auto ptPosition : m_recentPoints) {
		
		mA.element(i, 0) = 1;
		mA.element(i, 1) = ptPosition.x();
		mb.element(i, 0) = ptPosition.y();

		i++;
	}

	// solving for x, the coefficients of a linear function
	// mA * [x] = mb
	matrix<float, dims, pts> mAT = transpose(mA);

	// multiply both sides by the transpose of matrix A
	// (mA^T * mA) * [x] = mA^T * mb
	matrix<float, dims, dims> mATA = mAT * mA;
	matrix<float, dims, 1> mATb = mAT * mb;

	// check invertibility of matrix A times A-transpose
	matrix<float, dims, dims> mI;
	matrix<float, dims, 1> x;

	// if determinant = 0, the matrix is not invertible
	CB(determinant(mATA) != 0);

	mI = inverse(mATA);

	// multiplying both sides by the inverse solves for x
	// [x] = (mA^T * mA)^-1 * mA^T * mb
	x = mI * mATb;

	// linear
	// y = x[0][0] + x[0][1]*t
	float slope = x.element(1, 0);

	// calculate label orientation through 2-dimensional slope
	float theta = atan2(slope,1);

	vDirection = vector(1, slope, 0);
	qRotation = quaternion::MakeQuaternionWithEuler(theta, 0.0f, 0.0f);

	return true;
Error:
	return false;
}

std::shared_ptr<FlatContext> UIPointerLabel::GetContext() {
	return m_pRenderContext;
}

bool UIPointerLabel::IsPointingLeft() {
	return m_fPointingLeft;
}

