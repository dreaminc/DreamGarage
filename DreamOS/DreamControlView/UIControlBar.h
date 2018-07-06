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
#define URL_WIDTH_2 0.84375f //0.6875f is the width with forward and back
#define ITEM_SPACING 0.015625f

#define ITEM_ACTUATION_DEPTH 0.02f

enum class BarType {
	DEFAULT,
	BROWSER,
	DESKTOP,
	KEYBOARD,
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
	virtual RESULT HandleKeyboardPressed(UIButton* pButtonContext, void* pContext) = 0;
	virtual RESULT HandleTabPressed(UIButton* pButtonContext, void* pContext) = 0;
	virtual RESULT HandleBackTabPressed(UIButton* pButtonContext, void* pContext) = 0;
	virtual RESULT HandleDonePressed(UIButton* pButtonContext, void* pContext) = 0;
};

class UIControlBar : public UIView {
public:
	UIControlBar(HALImp *pHALImp, DreamOS *pDreamOS);
	~UIControlBar();

	RESULT Initialize();

	//TODO: currently different control bar layouts are not used
	RESULT UpdateButtonsWithType(BarType type);
	RESULT UpdateNavigationButtons(bool fCanGoBack, bool fCanGoForward);

	float GetSpacingOffset();

	// Getters used for registering event behavior
	std::shared_ptr<UIButton> GetBackButton();
	std::shared_ptr<UIButton> GetForwardButton();
	std::shared_ptr<UIButton> GetToggleButton();
	std::shared_ptr<UIButton> GetShareButton();
	std::shared_ptr<UIButton> GetStopButton();
	std::shared_ptr<UIButton> GetURLButton();
	std::shared_ptr<UIButton> GetKeyboardButton();
	std::shared_ptr<UIButton> GetTabButton();
	std::shared_ptr<UIButton> GetBackTabButton();
	std::shared_ptr<UIButton> GetDoneButton();

	// Wrappers for executing the observer methods
	RESULT BackPressed(UIButton* pButtonContext, void* pContext);
	RESULT ForwardPressed(UIButton* pButtonContext, void* pContext);
	RESULT TogglePressed(UIButton* pButtonContext, void* pContext);
	RESULT OpenPressed(UIButton* pButtonContext, void* pContext);
	RESULT ClosePressed(UIButton* pButtonContext, void* pContext);
	RESULT SharePressed(UIButton* pButtonContext, void* pContext);
	RESULT URLPressed(UIButton* pButtonContext, void* pContext);
	RESULT KeyboardPressed(UIButton* pButtonContext, void* pContext);
	RESULT TabPressed(UIButton* pButtonContext, void* pContext);
	RESULT BackTabPressed(UIButton* pButtonContext, void* pContext);
	RESULT DonePressed(UIButton* pButtonContext, void* pContext);

	// Getters used for swapping the hide/show texture on the hide button
	texture *GetHideTexture();
	texture *GetShowTexture();
	texture *GetShareTexture();
	texture *GetStopTexture();

	texture *GetTabTexture();
	texture *GetCantTabTexture();
	texture *GetBackTabTexture();
	texture *GetCantBackTabTexture();

	// for non-default implementations, call these before initialize
	RESULT SetTotalWidth(float totalWidth);
	RESULT SetItemSide(float itemSide);
	RESULT SetURLWidth(float urlWidth);
	RESULT SetItemSpacing(float itemSpacing);

	std::shared_ptr<text> GetURLText();

	RESULT RegisterObserver(ControlBarObserver *pObserver);
	RESULT UnregisterObserver(ControlBarObserver *pObserver);

	static BarType ControlBarTypeFromString(const std::string& strContentType);

// common behavior
public:
	RESULT HandleTouchStart(UIButton* pButtonContext, void* pContext);

public:
	const wchar_t *k_wszBack = L"control-view-back.png";
	const wchar_t *k_wszBackDisabled = L"browser-control-back-disabled.png";
	const wchar_t *k_wszForward = L"control-view-forward.png";
	const wchar_t *k_wszForwardDisabled = L"browser-control-forward-disabled.png";
	const wchar_t *k_wszOpen = L"control-view-open.png";
	const wchar_t *k_wszClose = L"control-view-close.png";
	const wchar_t *k_wszShare = L"control-view-share.png";
	const wchar_t *k_wszStopSharing = L"control-view-stop-sharing.png";
	const wchar_t *k_wszHide = L"control-view-minimize.png";
	const wchar_t *k_wszShow = L"control-view-maximize.png";
	const wchar_t *k_wszURL = L"control-view-url.png";
	const wchar_t *k_wszKeyboard = L"control-view-keyboard.png";

	//TODO: need new files
	const wchar_t *k_wszTab = L"key-tab-next.png";
	const wchar_t *k_wszCantTab = L"key-tab-next-disabled.png";
	const wchar_t *k_wszBackTab = L"key-tab-previous.png";
	const wchar_t *k_wszCantBackTab = L"key-tab-previous-disabled.png";
	const wchar_t *k_wszDone = L"key-done.png";

private:
	std::shared_ptr<UIButton> m_pBackButton = nullptr;
	std::shared_ptr<UIButton> m_pForwardButton = nullptr;
	std::shared_ptr<UIButton> m_pToggleButton = nullptr;
	std::shared_ptr<UIButton> m_pCloseButton = nullptr;
	std::shared_ptr<UIButton> m_pOpenButton = nullptr;
	std::shared_ptr<UIButton> m_pShareToggleButton = nullptr;
	std::shared_ptr<UIButton> m_pKeyboardButton = nullptr;
	std::shared_ptr<UIButton> m_pTabButton = nullptr;
	std::shared_ptr<UIButton> m_pBackTabButton = nullptr;
	std::shared_ptr<UIButton> m_pDoneButton = nullptr;

	std::shared_ptr<UIButton> m_pURLButton = nullptr;
	std::shared_ptr<text> m_pURLText = nullptr;

	texture *m_pBackTexture = nullptr;
	texture *m_pForwardTexture = nullptr;
	texture *m_pBackDisabledTexture = nullptr;
	texture *m_pForwardDisabledTexture = nullptr;
	texture *m_pKeyboardTexture = nullptr;
	texture *m_pShowTexture = nullptr;
	texture *m_pHideTexture = nullptr;
	texture *m_pOpenTexture = nullptr;
	texture *m_pCloseTexture = nullptr;
	texture *m_pShareTexture = nullptr;
	texture *m_pStopSharingTexture = nullptr;
	texture *m_pURLTexture = nullptr;
	texture *m_pTabTexture = nullptr;
	texture *m_pCantTabTexture = nullptr;
	texture *m_pBackTabTexture = nullptr;
	texture *m_pCantBackTabTexture = nullptr;
	texture *m_pDoneTexture = nullptr;

	float m_totalWidth = TOTAL_WIDTH;
	float m_itemSide = m_totalWidth * ITEM_SIDE;
	float m_itemSpacing = m_totalWidth * ITEM_SPACING;
	float m_urlWidth = m_totalWidth * URL_WIDTH_2;
	float m_actuationDepth = ITEM_ACTUATION_DEPTH;

	BarType m_barType = BarType::DEFAULT;

	ControlBarObserver *m_pObserver = nullptr;
};

#endif UI_CONTROL_BAR_H_