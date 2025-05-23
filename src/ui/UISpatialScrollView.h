#ifndef UI_SPATIAL_SCROLL_VIEW_H_
#define UI_SPATIAL_SCROLL_VIEW_H_

#include "core/ehm/EHM.h"

// Dream UI
// dos/src/ui/UISpatialScrollView.h

#include <queue>

#include "UIScrollView.h"

#include "core/types/Subscriber.h"

#include "sense/SenseController.h"

// TODO: Shouldn't refer to cloud - replace this with a ScrollViewNode
#include "Cloud/Menu/MenuNode.h"	

class UIButton;
class DreamOS;
class text;

// TODO: time for config shits
#define MAX_ELEMENTS 4
#define MENU_CENTER_OFFSET -1.5f

#define ITEM_ANGLE_X -30.0f
#define ITEM_ANGLE_Y 12.0f
#define ITEM_START_ANGLE_Y (-1.5f * ITEM_ANGLE_Y)
#define ITEM_HEIGHT 0.0f 

#define ITEM_SCALE (5.0f / 6.0f) // shrink menu items to create space between them

#define TITLE_ANGLE_X 75.0f
#define TITLE_HEIGHT (ITEM_HEIGHT + 0.125f) //TODO: derive out of item angle + defined margin
#define TITLE_WIDTH 0.068f
#define TITLE_ASPECT (3.0f / 4.0f)
#define MENU_TITLE_LINE_HEIGHT 0.055f

#define TITLE_TEXT_OFFSET_X 0.6f
#define TITLE_TEXT_OFFSET_Y 0.005f

#define SCROLL_SCALE 0.2f 
#define SCROLL_ARROW_BIAS 0.3f // Pulls scroll chevrons in towards the menu items
#define SCROLL_ASPECT_RATIO 138.0f / 200.0f

#define FADE_DURATION 0.1f
#define PUSH_DEPTH -0.1f

enum class ScrollState {
	NONE,
	SCROLLING
};

class UISpatialScrollViewObserver {
public:
	virtual RESULT GetNextPageItems() = 0;
};

class UISpatialScrollView : public UIView, public UIScrollView//,
					//public Subscriber<SenseControllerEvent>					
{
public:
	UISpatialScrollView(HALImp *pHALImp, DreamOS *pDreamOS);
	~UISpatialScrollView();

	RESULT Initialize();
	RESULT InitializeWithWidth(float totalWidth);

	RESULT Update();
	RESULT UpdateWithWidth(float totalWidth);
	RESULT UpdateMenuButtons(std::vector<std::shared_ptr<UIButton>> pButtons);
	RESULT PositionMenuButton(float index, std::shared_ptr<UIButton> pButton);

	RESULT SetScrollVisible(bool fVisible);
	bool IsCapturable(UIButton* pButton);

// default behaviors
public:
	RESULT HideObject(DimObj* pObject, bool fDeleteObject = false);
	RESULT ShowObject(DimObj* pObject, color showColor = color(1.0f, 1.0f, 1.0f, 1.0f));
	RESULT HideAndPushButton(UIButton* pButton);
	// pass optional pushButton to have an additional moving back animation
	RESULT HideAllButtons(UIButton* pPushButton = nullptr);
	RESULT ShowTitle();
	RESULT Snap();
	RESULT Show();
	RESULT Hide();

	RESULT OnRotationDelta(int delta);

	RESULT AddScrollViewNode(std::shared_ptr<MenuNode> pMenuNode);
	RESULT UpdateScrollViewNode(MenuNode* pMenuNode);
	RESULT ClearScrollViewNodes();
	std::vector<std::shared_ptr<MenuNode>> GetScrollViewNodes();

	RESULT RegisterObserver(UISpatialScrollViewObserver *pObserver);
	RESULT UnregisterObserver(UISpatialScrollViewObserver *pObserver);

private:
	UISpatialScrollViewObserver* m_pObserver = nullptr;

public:
	ScrollState GetState();
	std::shared_ptr<quad> GetTitleQuad();
	std::shared_ptr<text> GetTitleText();
	std::shared_ptr<UIView> GetMenuItemsView();

	float GetWidth();
	float GetClippingThreshold();
	float GetClippingRate();

public:
	virtual RESULT Notify(SenseControllerEvent *pEvent) override;	

private:
	// button positioning
	int m_itemIndex = 0;
	float m_menuCenterOffset = MENU_CENTER_OFFSET;

	float m_itemAngleX = ITEM_ANGLE_X;
	float m_itemAngleY = ITEM_ANGLE_Y;
	float m_itemStartAngleY = ITEM_START_ANGLE_Y;
	float m_itemHeight = ITEM_HEIGHT;
	
	float m_itemScale = ITEM_SCALE;

	float m_itemWidth;

	// values for UIClipping shader
	float m_clippingThreshold;
	float m_clippingRate;

	float m_titleAngleX = TITLE_ANGLE_X;
	float m_titleHeight = TITLE_HEIGHT;

	float m_titleWidth = TITLE_WIDTH;
	float m_titleAspectRatio = TITLE_ASPECT;
	float m_titleLineHeight = MENU_TITLE_LINE_HEIGHT;
	float m_titleOffsetX = TITLE_TEXT_OFFSET_X;
	float m_titleOffsetY = TITLE_TEXT_OFFSET_Y;

	// scrolling
	float m_scrollScale = SCROLL_SCALE;
	float m_scrollBias = SCROLL_ARROW_BIAS;
	float m_maxElements = MAX_ELEMENTS;
	int m_nextPagePremptBuffer = 15;	// this just happens to be enough that it feels seamless - fairly dependent on max scroll speed.
	float m_yRotation;
	float m_yRotationPerElement;
	//float m_velocity;		// inherited from UIScrollView
	float m_fadeDuration = FADE_DURATION;
	float m_pushDepth = PUSH_DEPTH;
	float m_initialWidth = 0.0f;

	color m_hiddenColor = color(1.0f, 1.0f, 1.0f, 0.0f);
	color m_canScrollColor = color(1.0f, 1.0f, 1.0f, 0.5f);
	color m_visibleColor = color(1.0f, 1.0f, 1.0f, 1.0f);

	// UI objects
	std::shared_ptr<UIView> m_pTitleView = nullptr;
	std::shared_ptr<quad> m_pTitleQuad = nullptr;
	std::shared_ptr<text> m_pTitleText = nullptr;

	std::shared_ptr<UIButton> m_pLeftScrollButton = nullptr; 
	std::shared_ptr<UIButton> m_pRightScrollButton = nullptr;
	ScrollState m_menuState;

	std::vector<std::shared_ptr<UIButton>> m_pPendingButtons;
	std::shared_ptr<UIView> m_pMenuButtonsContainer = nullptr; // used to clear for now
	std::vector<std::shared_ptr<MenuNode>> m_pScrollViewNodes;
	std::deque<std::shared_ptr<UIButton>> m_pButtonDeque;

	bool m_fPendingButton = false;
	bool m_fScrollButtonVisible = false;

	std::queue<DimObj*> m_pendingObjectRemovalQueue;
};

#endif // ! UI_SPATIAL_SCROLL_VIEW_H_