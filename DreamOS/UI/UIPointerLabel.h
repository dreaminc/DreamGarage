#ifndef UI_POINTER_LABEL_H_
#define UI_POINTER_LABEL_H_

#include "UIView.h"

class FlatContext;
class font;

class UIPointerLabel : public UIView {
public:
	UIPointerLabel(HALImp *pHALImp, DreamOS *pDreamOS);
	~UIPointerLabel();

public:
	RESULT Initialize(float parentHeight, std::string strInitials);

public:
	std::shared_ptr<FlatContext> GetContext();

private:
	std::shared_ptr<FlatContext> m_pRenderContext = nullptr;

// Resource strings
private:
	const wchar_t *k_wszPointerLeftTexture = L"texture/shared-view/pointer-left.png";
	const wchar_t *k_wszPointerCenterTexture = L"texture/shared-view/pointer-center.png";
	const wchar_t *k_wszPointerRightTexture = L"texture/shared-view/pointer-right.png";

// Resource textures
private:
	texture* m_pPointerLeft = nullptr;
	texture* m_pPointerCenter = nullptr;
	texture* m_pPointerRight = nullptr;

	std::shared_ptr<font> m_pFont = nullptr;
};

#endif // ! UI_POINTER_LABEL_H_