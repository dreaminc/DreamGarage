#ifndef UI_SCROLL_VIEW_H_
#define UI_SCROLL_VIEW_H_

#include "UIView.h"
#include "Primitives/Subscriber.h"
#include "Sense/SenseController.h"

class UIButton;
class DreamOS;
class text;

#define MAX_ELEMENTS 4
#define MENU_DEPTH -1.5f
#define ITEM_ANGLE_X -30.0f
#define ITEM_ANGLE_Y 12.0f
#define ITEM_START_ANGLE_Y -15.0f
#define ITEM_HEIGHT 0.75
#define ITEM_SCALE 0.25
#define ITEM_SCALE_SELECTED 1.25
#define TITLE_ANGLE_X 75.0f
#define TITLE_HEIGHT 0.875f
#define PAD_MOVE_CONSTANT 0.005f

enum class ScrollState {
	NONE,
	SCROLLING
};

class UIScrollView : public UIView,
					public Subscriber<SenseControllerEvent>					
{
public:
	UIScrollView(HALImp *pHALImp, DreamOS *pDreamOS);
	~UIScrollView();

	RESULT Initialize();

	RESULT Update();
	RESULT UpdateMenuButtons(std::vector<std::shared_ptr<UIButton>> pButtons);
	RESULT PositionMenuButton(int index, std::shared_ptr<UIButton> pButton);

	RESULT SetScrollVisible(bool fVisible);
// default behaviors
public:
	RESULT AnimateScaleUp(void *pContext);
	RESULT AnimateScaleReset(void *pContext);

	//Temporary
	RESULT StartScrollLeft(void *pContext);
	RESULT StartScrollRight(void *pContext);
	RESULT StopScroll(void *pContext);

	RESULT HideButton(UIButton* pScrollButton);
	RESULT ShowButton(UIButton* pScrollButton);
	RESULT HideAndPushButton(UIButton* pButton);
	// pass optional pushButton to have an additional moving back animation
	RESULT HideAllButtons(UIButton* pPushButton = nullptr);

public:
	ScrollState GetState();
	std::shared_ptr<UIView> GetTitleView();
	std::shared_ptr<quad> GetTitleQuad();
	std::shared_ptr<text> GetTitleText();
	std::shared_ptr<UIView> GetMenuItemsView();

public:
	RESULT Notify(SenseControllerEvent *pEvent);

private:
	// button positioning
	float m_menuDepth = MENU_DEPTH;

	float m_itemAngleX = ITEM_ANGLE_X;
	float m_itemAngleY = ITEM_ANGLE_Y;
	float m_itemStartAngleY = ITEM_START_ANGLE_Y;
	float m_itemHeight = ITEM_HEIGHT;
	float m_itemScale = ITEM_SCALE;
	float m_itemScaleSelected = ITEM_SCALE_SELECTED;

	float m_titleAngleX = TITLE_ANGLE_X;
	float m_titleHeight = TITLE_HEIGHT;

	// scrolling
	float m_maxElements = MAX_ELEMENTS;
	int m_objectIndexMin;
	int m_objectIndexMax;
	float m_yRotation;
	float m_yRotationPerElement;
	float m_velocity;

	// UI objects
	std::shared_ptr<UIView> m_pTitleView = nullptr;
	std::shared_ptr<quad> m_pTitleQuad = nullptr;
	std::shared_ptr<text> m_pTitleText = nullptr;

	std::shared_ptr<UIButton> m_pLeftScrollButton = nullptr; 
	std::shared_ptr<UIButton> m_pRightScrollButton = nullptr;
	ScrollState m_menuState;

	std::shared_ptr<UIView> m_pMenuButtonsContainer = nullptr; // used to clear for now
	std::vector<std::shared_ptr<UIButton>> m_pMenuButtons;

	bool m_fScrollButtonVisible = false;
};

#endif // ! UI_SCROLL_VIEW_H_