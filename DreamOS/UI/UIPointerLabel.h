#ifndef UI_POINTER_LABEL_H_
#define UI_POINTER_LABEL_H_

#include "UIView.h"

#define NUM_POINTS 15

class FlatContext;
class font;
class DreamShareViewPointerMessage;

class UIPointerLabel : public UIView {
public:
	UIPointerLabel(HALImp *pHALImp, DreamOS *pDreamOS);
	~UIPointerLabel();

public:
	RESULT Initialize();
	RESULT RenderLabelWithInitials(std::shared_ptr<quad> pParentQuad, std::string strInitials);

private:
	RESULT RenderLabel();

public:
	RESULT HandlePointerMessage(DreamShareViewPointerMessage *pUpdatePointerMessage);

	std::shared_ptr<FlatContext> GetContext();
	bool IsPointingLeft();

private:
	std::shared_ptr<FlatContext> m_pRenderContext = nullptr;

	// used to help determine where the pointer is on the quad, 
	// and from that the direction of the pointer;
	std::shared_ptr<quad> m_pParentQuad = nullptr;
	bool m_fPointingLeft = true;

	std::shared_ptr<composite> m_pDotComposite = nullptr;
	std::shared_ptr<quad> m_pDotQuad = nullptr;

// Path saving methods
private:
	RESULT UpdateOrientationFromPoints();
	bool OrientationFromAverage(quaternion& qRotation, vector& vDirection);
	bool OrientationFromNormalEquation(quaternion& qRotation, vector& vDirection);

private:
	std::deque<point> m_recentPoints;
	float m_currentAngle;

// Resource strings
private:
	const wchar_t *k_wszPointerLeftTexture = L"texture/shared-view/pointer-left.png";
	const wchar_t *k_wszPointerCenterTexture = L"texture/shared-view/pointer-center.png";
	const wchar_t *k_wszPointerRightTexture = L"texture/shared-view/pointer-right.png";
	const wchar_t *k_wszPointerDotTexture = L"texture/shared-view/pointer-dot.png";

// Resource textures
private:
	texture* m_pPointerLeft = nullptr;
	texture* m_pPointerCenter = nullptr;
	texture* m_pPointerRight = nullptr;
	texture* m_pPointerDot = nullptr;

	std::shared_ptr<font> m_pFont = nullptr;
};

#endif // ! UI_POINTER_LABEL_H_