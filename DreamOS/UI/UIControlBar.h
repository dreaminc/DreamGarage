#ifndef UI_CONTROL_BAR_H_
#define UI_CONTROL_BAR_H_

#include "UIView.h"
#include <memory>

class UIButton;
class text;

//TODO: member totalWidth variable with this value as default
#define TOTAL_WIDTH 0.6f

//TODO: some of these values / the way they are used are specific to control view
#define ITEM_SIDE 0.0625f
#define URL_WIDTH 0.6875f
#define ITEM_SPACING 0.015625f

#define ITEM_ACTUATION_DEPTH 0.02f;

class UIControlBar : public UIView {
public:
	UIControlBar(HALImp *pHALImp, DreamOS *pDreamOS);
	~UIControlBar();

	RESULT Initialize();

	float GetSpacingOffset();

	// Getters used for registering event behavior
	std::shared_ptr<UIButton> GetBackButton();
	std::shared_ptr<UIButton> GetForwardButton();
	std::shared_ptr<UIButton> GetToggleButton();
	std::shared_ptr<UIButton> GetStopButton();
	std::shared_ptr<UIButton> GetURLButton();
	std::vector<std::shared_ptr<UIButton>> GetControlButtons();

	// Getters used for swapping the hide/show texture on the hide button
	texture *GetHideTexture();
	texture *GetShowTexture();

	std::shared_ptr<text> GetURLText();

// common behavior
public:
	RESULT HandleTouchStart(UIButton* pButtonContext, void* pContext);

private:
	std::shared_ptr<UIButton> m_pBackButton;
	std::shared_ptr<UIButton> m_pForwardButton;
	std::shared_ptr<UIButton> m_pToggleButton;
	std::shared_ptr<UIButton> m_pStopButton;

	std::shared_ptr<UIButton> m_pURLButton;
	std::shared_ptr<text> m_pURLText;

	texture *m_pBackTexture;
	texture *m_pForwardTexture;
	texture *m_pHideTexture;
	texture *m_pStopTexture;
	texture *m_pShowTexture;
	texture *m_pURLTexture;

	float m_totalWidth = TOTAL_WIDTH;
	float m_itemSide = m_totalWidth * ITEM_SIDE;
	float m_itemSpacing = m_totalWidth * ITEM_SPACING;
	float m_urlWidth = m_totalWidth * URL_WIDTH;
	float m_actuationDepth = ITEM_ACTUATION_DEPTH;

};

#endif UI_CONTROL_BAR_H_