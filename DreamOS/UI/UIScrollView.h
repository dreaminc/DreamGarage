#ifndef UI_SCROLL_VIEW_H_
#define UI_SCROLL_VIEW_H_

#include "UIView.h"

class UIButton;
class DreamOS;

enum class MenuState {
	NONE,
	SCROLLING
};

class UIScrollView : public UIView {
public:
	UIScrollView(HALImp *pHALImp);
	~UIScrollView();

	RESULT Initialize();
	RESULT InitializeWithDOS(DreamOS *pDreamOS);

	RESULT Update();
	RESULT UpdateMenuButtons(std::vector<std::shared_ptr<UIButton>> pButtons);
	RESULT PositionMenuButton(int index, std::shared_ptr<UIButton> pButton);

// default behaviors
public:
	RESULT AnimateScaleUp(void *pContext);
	RESULT AnimateScaleReset(void *pContext);

	//Temporary
	RESULT StartScrollLeft(void *pContext);
	RESULT StartScrollRight(void *pContext);
	RESULT StopScroll(void *pContext);

public:
	MenuState GetState();
	std::shared_ptr<UIView> GetTitleView();
	std::shared_ptr<UIView> GetMenuItemsView();

private:
	// button positioning
	float m_menuDepth;

	float m_itemAngleX;
	float m_itemAngleY;
	float m_itemStartAngleY;
	float m_itemHeight;
	float m_itemScale;
	float m_itemScaleSelected;

	float m_titleAngleX;
	float m_titleHeight;

	// scrolling
	float m_maxElements;
	int m_objectIndex;
	float m_yRotation;
	float m_yRotationPerElement;
	float m_velocity;

	// UI objects
	std::shared_ptr<UIView> m_pTitleView;
	std::shared_ptr<UIButton> m_pLeftScrollButton; 
	std::shared_ptr<UIButton> m_pRightScrollButton;
	MenuState m_menuState;

	std::shared_ptr<UIView> m_pMenuButtonsContainer; // used to clear for now
	std::vector<std::shared_ptr<UIButton>> m_pMenuButtons;

	// context
	DreamOS *m_pDreamOS;
};

#endif // ! UI_SCROLL_VIEW_H_