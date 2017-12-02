#ifndef UI_CONTROL_BAR_H_
#define UI_CONTROL_BAR_H_

#include "UIView.h"
#include <memory>

class UIButton;

//TODO: member totalWidth variable with this value as default
#define TOTAL_WIDTH 0.6f

//TODO: some of these values / the way they are used are specific to control view
#define ITEM_SIDE 0.0625f
#define URL_WIDTH 0.6875f
#define ITEM_SPACING 0.015625f

class UIControlBar : public UIView {
public:
	UIControlBar(HALImp *pHALImp, DreamOS *pDreamOS);
	~UIControlBar();

	RESULT Initialize();

	float GetSpacingOffset();

private:
	std::shared_ptr<UIButton> m_pBackButton;
	std::shared_ptr<UIButton> m_pForwardButton;
	std::shared_ptr<UIButton> m_pHideButton;
	std::shared_ptr<UIButton> m_pStopButton;

	std::shared_ptr<UIButton> m_pURLButton;

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
};

#endif UI_CONTROL_BAR_H_