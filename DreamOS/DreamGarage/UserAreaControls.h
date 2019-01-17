#ifndef DREAM_CONTROL_BAR_H_
#define DREAM_CONTROL_BAR_H_

#include "RESULT/EHM.h"

#include "UI/UIView.h"

#include "DreamGarage/UICommon.h"

#include "DreamUserControlArea/DreamContentSource.h"

#include <map>

//#include "DreamUserControlArea/DreamUserControlArea.h"
class DreamUserControlArea;
class text;

//All relative to parent app
#define URL_WIDTH 0.5484

class UserAreaControls : public UIView {
public:
	enum class buttonType {
		// Browser
		BACK,
		BACK_DISABLED,
		FORWARD,
		FORWARD_DISABLED,
		CLOSE,
		URL,
		OPEN,
		SHARE,
		STOP_SHARING,
		HIDE,
		SHOW,

		// Desktop
		KEYBOARD,

		// Virtual Camera
		SOURCE_CAMERA,
		SOURCE_SHARE,
		SOURCE_NO_SHARE,
		SEND,
		STOP_SENDING,
		RESET,
		// TODO: Re-centering

		INVALID
	};

public:
	UserAreaControls(HALImp *pHALImp, DreamOS *pDreamOS);
	~UserAreaControls();

	// DreamApp
public:
	RESULT Initialize(DreamUserControlArea *pParent);
	RESULT InitializeText();
	RESULT Update();

	// ControlBarObserver
public:
	RESULT HandleBackPressed(UIButton* pButtonContext, void* pContext);
	RESULT HandleForwardPressed(UIButton* pButtonContext, void* pContext);
	RESULT HandleShowTogglePressed(UIButton* pButtonContext, void* pContext);
	RESULT HandleOpenPressed(UIButton* pButtonContext, void* pContext);
	RESULT HandleClosePressed(UIButton* pButtonContext, void* pContext);
	RESULT HandleShareTogglePressed(UIButton *pButtonContext, void *pContext);
	RESULT HandleURLPressed(UIButton* pButtonContext, void* pContext);

	RESULT HandleKeyboardPressed(UIButton* pButtonContext, void* pContext);

	RESULT HandleSourceTogglePressed(UIButton* pButtonContext, void* pContext);
	RESULT HandleSendTogglePressed(UIButton* pButtonContext, void* pContext);
	RESULT HandleResetPressed(UIButton* pButtonContext, void* pContext);

	// Update functions specific to this type of control bar
	RESULT SetSharingFlag(bool fIsSharing);
	RESULT SetTitleText(const std::string& strTitle);
	RESULT UpdateControlBarButtonsWithType(std::string strContentType);
	RESULT UpdateButtonVisibility(std::string strContentType, bool fVisible);
	RESULT UpdateNavigationButtons(bool fCanGoBack, bool fCanGoForward);
	RESULT UpdateIsActive(bool fActive);

	std::shared_ptr<text> GetURLText();

// Animations
public:
	RESULT Show();
	RESULT Hide();

private:
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

	// keyboard
	const wchar_t *k_wszKeyboard = L"control-view-keyboard.png";

	// camera
	const wchar_t *k_wszSourceCamera = L"control-view-source-camera.png";
	const wchar_t *k_wszSourceShare = L"control-view-source-share.png";
	const wchar_t *k_wszSourceNoShare = L"control-view-source-camera-no-share.png";
	const wchar_t *k_wszSend = L"control-view-send.png";
	const wchar_t *k_wszStopSending = L"control-view-stop-sending.png";
	const wchar_t *k_wszCameraReset = L"camera-reset.png";

private:

	DreamUserControlArea* m_pParentApp = nullptr;

	std::shared_ptr<text> m_pURLText = nullptr;
	bool m_fUpdateTitle = false;
	std::string m_strUpdateTitle = "";

	double m_buttonWidth = ITEM_SIZE;
	double m_buttonHeight = ITEM_SIZE;
	double m_spacingSize;
	double m_urlWidth = URL_WIDTH;
	double m_urlHeight = ITEM_SIZE;

	std::map<buttonType, std::shared_ptr<texture>> m_buttonTextureMap;

	// browser
	std::shared_ptr<UIButton> m_pBackButton = nullptr;
	std::shared_ptr<UIButton> m_pForwardButton = nullptr;
	std::shared_ptr<UIButton> m_pShareButton = nullptr;

	// desktop
	std::shared_ptr<UIButton> m_pKeyboardButton = nullptr;

	// camera
	std::shared_ptr<UIButton> m_pCameraSourceButton = nullptr;
	std::shared_ptr<UIButton> m_pSendButton = nullptr;
	std::shared_ptr<UIButton> m_pResetButton = nullptr;

	// general
	std::shared_ptr<UIButton> m_pURLButton = nullptr;
	std::shared_ptr<UIButton> m_pOpenButton = nullptr;
	std::shared_ptr<UIButton> m_pMinimizeButton = nullptr;
	std::shared_ptr<UIButton> m_pCloseButton = nullptr;

	std::string m_strCurrentContentType = CONTENT_TYPE_BROWSER;

	bool m_fIsActive = false;
};

#endif // ! DREAM_CONTROL_BAR_H_