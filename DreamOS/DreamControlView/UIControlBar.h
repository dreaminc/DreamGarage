#ifndef UI_CONTROL_BAR_H_
#define UI_CONTROL_BAR_H_

#include "UI/UIView.h"
#include <memory>

class UIButton;
class text;

//TODO: member totalWidth variable with this value as default
#define TOTAL_WIDTH 0.6f

//TODO: some of these values / the way they are used are specific to control view
#define ITEM_SIDE 0.0625f
#define URL_WIDTH 0.84375f //0.6875f is the width with forward and back
#define ITEM_SPACING 0.015625f

#define ITEM_ACTUATION_DEPTH 0.02f

enum class BarType {
	DEFAULT,
	BROWSER,
	INVALID
};

// Default Observer
class ControlBarObserver {
public:
	virtual RESULT HandleBackPressed(UIButton* pButtonContext, void* pContext) = 0;
	virtual RESULT HandleForwardPressed(UIButton* pButtonContext, void* pContext) = 0;
	virtual RESULT HandleShowTogglePressed(UIButton* pButtonContext, void* pContext) = 0;
	virtual RESULT HandleOpenPressed(UIButton* pButtonContext, void* pContext) = 0;
	virtual RESULT HandleClosePressed(UIButton* pButtonContext, void* pContext) = 0;
	virtual RESULT HandleShareTogglePressed(UIButton *pButtonContext, void *pContext) = 0;
	virtual RESULT HandleURLPressed(UIButton* pButtonContext, void* pContext) = 0;
};

class UIControlBar : public UIView {
public:
	UIControlBar(HALImp *pHALImp, DreamOS *pDreamOS);
	~UIControlBar();

	RESULT Initialize();

	//TODO: currently different control bar layouts are not used
	RESULT UpdateButtonsWithType(BarType type);

	float GetSpacingOffset();

	// Getters used for registering event behavior
	std::shared_ptr<UIButton> GetBackButton();
	std::shared_ptr<UIButton> GetForwardButton();
	std::shared_ptr<UIButton> GetToggleButton();
	std::shared_ptr<UIButton> GetStopButton();
	std::shared_ptr<UIButton> GetURLButton();
	std::vector<std::shared_ptr<UIButton>> GetControlButtons();

	// Wrappers for executing the observer methods
	RESULT BackPressed(UIButton* pButtonContext, void* pContext);
	RESULT ForwardPressed(UIButton* pButtonContext, void* pContext);
	RESULT TogglePressed(UIButton* pButtonContext, void* pContext);
	RESULT OpenPressed(UIButton* pButtonContext, void* pContext);
	RESULT ClosePressed(UIButton* pButtonContext, void* pContext);
	RESULT SharePressed(UIButton* pButtonContext, void* pContext);
	RESULT URLPressed(UIButton* pButtonContext, void* pContext);

	// Getters used for swapping the hide/show texture on the hide button
	texture *GetHideTexture();
	texture *GetShowTexture();

	// for non-default implementations, call these before initialize
	RESULT SetTotalWidth(float totalWidth);
	RESULT SetItemSide(float itemSide);
	RESULT SetURLWidth(float urlWidth);
	RESULT SetItemSpacing(float itemSpacing);

	std::shared_ptr<text> GetURLText();

	RESULT SetObserver(ControlBarObserver *pObserver);

	static BarType ControlBarTypeFromString(const std::string& strContentType);

	static std::shared_ptr<UIControlBar> MakeControlBarWithType(BarType type, std::shared_ptr<UIView> pViewContext);

// common behavior
public:
	RESULT HandleTouchStart(UIButton* pButtonContext, void* pContext);

public:
	const wchar_t *k_wszBack = L"control-view-back.png";
	const wchar_t *k_wszForward = L"control-view-forward.png";
	const wchar_t *k_wszOpen = L"control-view-open.png";
	const wchar_t *k_wszClose = L"control-view-close.png";
	const wchar_t *k_wszShare = L"control-view-share.png";
	const wchar_t *k_wszStopSharing = L"control-view-stop-sharing.png";
	const wchar_t *k_wszHide = L"control-view-minimize.png";
	const wchar_t *k_wszShow = L"control-view-maximize.png";
	const wchar_t *k_wszURL = L"control-view-url.png";

private:
	std::shared_ptr<UIButton> m_pBackButton;
	std::shared_ptr<UIButton> m_pForwardButton;
	std::shared_ptr<UIButton> m_pToggleButton;
	std::shared_ptr<UIButton> m_pCloseButton;
	std::shared_ptr<UIButton> m_pOpenButton;
	std::shared_ptr<UIButton> m_pShareToggleButton;

	std::shared_ptr<UIButton> m_pURLButton;
	std::shared_ptr<text> m_pURLText;

	texture *m_pBackTexture;
	texture *m_pForwardTexture;
	texture *m_pShowTexture;
	texture *m_pHideTexture;
	texture *m_pOpenTexture;
	texture *m_pCloseTexture;
	texture *m_pShareTexture;
	texture *m_pStopSharingTexture;
	texture *m_pURLTexture;

	float m_totalWidth = TOTAL_WIDTH;
	float m_itemSide = m_totalWidth * ITEM_SIDE;
	float m_itemSpacing = m_totalWidth * ITEM_SPACING;
	float m_urlWidth = m_totalWidth * URL_WIDTH;
	float m_actuationDepth = ITEM_ACTUATION_DEPTH;

	BarType m_barType = BarType::DEFAULT;

	ControlBarObserver *m_pObserver = nullptr;

};

#endif UI_CONTROL_BAR_H_