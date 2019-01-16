#include "UserAreaControls.h"
#include "DreamOS.h"
#include "DreamUserControlArea/DreamUserControlArea.h"
#include "DreamGarage/DreamBrowser.h"

#include "UI/UIButton.h"

#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"

#include "Primitives/font.h"

UserAreaControls::UserAreaControls(HALImp *pHALImp, DreamOS *pDreamOS) :
	UIView(pHALImp,pDreamOS)
{
	RESULT r = R_PASS;
}

UserAreaControls::~UserAreaControls() {
	// empty
}

// DreamApp
RESULT UserAreaControls::Initialize(DreamUserControlArea *pParent) {
	RESULT r = R_PASS;

	m_pParentApp = pParent;

	// ui positioning	
	float width = m_pParentApp->GetBaseWidth();
	float buttonWidth = m_buttonWidth * width;
	float buttonHeight = buttonWidth;
	float spacingSize = m_pParentApp->GetSpacingSize() * width;
	float doubleButtonWidth = buttonWidth * 2 + spacingSize;

	m_buttonWidth = buttonWidth;
	m_buttonHeight = buttonHeight;
	m_spacingSize = spacingSize;

	// button positioning
	float start = -width / 2.0f - spacingSize;

	float backOffset = start + buttonWidth / 2.0f;
	float backSourceOffset = start + doubleButtonWidth / 2.0f;
	float forwardOffset = backOffset + spacingSize + buttonWidth;
	float shareOffset = forwardOffset + spacingSize + buttonWidth;

	float urlOffset = 0.0f;
	float openOffset = urlOffset + (m_urlWidth * width / 2.0f) + buttonWidth / 2.0f + spacingSize;
	float hideOffset = openOffset + spacingSize + buttonWidth;
	float closeOffset = hideOffset + spacingSize + buttonWidth;

	//GetDOS()->AddObjectToUIGraph(GetComposite());

	m_urlWidth *= width;

	SetPosition(0.0f, 0.0f, m_pParentApp->GetBaseHeight() / 2.0f + 2 * spacingSize + buttonWidth / 2.0f);
	RotateXByDeg(-90.0f);

	SetVisible(true);

	// this can be local, because all values are saved to specific buttons
	m_buttonTextureMap[buttonType::BACK] = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszBack));
	m_buttonTextureMap[buttonType::BACK_DISABLED] = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszBackDisabled));
	m_buttonTextureMap[buttonType::FORWARD] = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszForward));
	m_buttonTextureMap[buttonType::FORWARD_DISABLED] = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszForwardDisabled));
	m_buttonTextureMap[buttonType::CLOSE] = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszClose));
	m_buttonTextureMap[buttonType::URL] = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszURL));
	m_buttonTextureMap[buttonType::OPEN] = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszOpen));
	m_buttonTextureMap[buttonType::STOP_SHARING] = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszStopSharing));
	m_buttonTextureMap[buttonType::SHARE] = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszShare));
	m_buttonTextureMap[buttonType::HIDE] = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszHide));
	m_buttonTextureMap[buttonType::SHOW] = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszShow));

	m_buttonTextureMap[buttonType::KEYBOARD] = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszKeyboard));

	m_buttonTextureMap[buttonType::SOURCE_CAMERA] = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszSourceCamera));
	m_buttonTextureMap[buttonType::SOURCE_SHARE] = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszSourceShare));
	m_buttonTextureMap[buttonType::SOURCE_NO_SHARE] = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszSourceNoShare));
	m_buttonTextureMap[buttonType::SEND] = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszSend));
	m_buttonTextureMap[buttonType::STOP_SENDING] = std::shared_ptr<texture>(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszStopSending));

	for (int i = 0; i < (int)(buttonType::INVALID); i++) {
		CN(m_buttonTextureMap[(buttonType)(i)]);
	}

	// browser
	m_pBackButton = AddButton(backOffset, buttonWidth, buttonHeight,
		std::bind(&UserAreaControls::HandleBackPressed, this, std::placeholders::_1, std::placeholders::_2),
		m_buttonTextureMap[buttonType::BACK], m_buttonTextureMap[buttonType::BACK_DISABLED]);

	m_pForwardButton = AddButton(forwardOffset, buttonWidth, buttonHeight,
		std::bind(&UserAreaControls::HandleForwardPressed, this, std::placeholders::_1, std::placeholders::_2),
		m_buttonTextureMap[buttonType::FORWARD], m_buttonTextureMap[buttonType::FORWARD_DISABLED]);

	m_pShareButton = AddButton(shareOffset, buttonWidth, buttonHeight,
		std::bind(&UserAreaControls::HandleShareTogglePressed, this, std::placeholders::_1, std::placeholders::_2),
		m_buttonTextureMap[buttonType::STOP_SHARING], m_buttonTextureMap[buttonType::SHARE]);

	// desktop
	m_pKeyboardButton = AddButton(backSourceOffset, buttonWidth*2.0f + spacingSize, buttonHeight,
		std::bind(&UserAreaControls::HandleKeyboardPressed, this, std::placeholders::_1, std::placeholders::_2),
		m_buttonTextureMap[buttonType::KEYBOARD]);

	m_pKeyboardButton->SetVisible(false);

	// camera
	m_pCameraSourceButton = AddButton(backSourceOffset, buttonWidth*2.0f + spacingSize, buttonHeight,
		std::bind(&UserAreaControls::HandleSourceTogglePressed, this, std::placeholders::_1, std::placeholders::_2),
		m_buttonTextureMap[buttonType::SOURCE_SHARE], m_buttonTextureMap[buttonType::SOURCE_NO_SHARE]);

	m_pSendButton = AddButton(shareOffset, buttonWidth, buttonHeight,
		std::bind(&UserAreaControls::HandleSendTogglePressed, this, std::placeholders::_1, std::placeholders::_2),
		m_buttonTextureMap[buttonType::SEND], m_buttonTextureMap[buttonType::STOP_SENDING]);

	m_pCameraSourceButton->SetVisible(false);
	//m_pCameraSourceButton->SetEnabledFlag(false);
	m_pCameraSourceButton->Toggle();
	m_pSendButton->SetVisible(false);

// Re-enable for selectability of the URL button
//	CR(AddButton(ControlBarButtonType::URL, urlOffset, m_urlWidth * width, 
//		std::bind(&UIContentControlBar::HandleURLPressed, this, std::placeholders::_1, std::placeholders::_2)));

	m_pURLButton = AddButton(urlOffset, m_urlWidth, buttonHeight, nullptr, m_buttonTextureMap[buttonType::URL]);

	m_pOpenButton = AddButton(openOffset, buttonWidth, buttonHeight, 
		std::bind(&UserAreaControls::HandleOpenPressed, this, std::placeholders::_1, std::placeholders::_2),
		m_buttonTextureMap[buttonType::OPEN]);

	m_pMinimizeButton = AddButton(hideOffset, buttonWidth, buttonHeight,
		std::bind(&UserAreaControls::HandleShowTogglePressed, this, std::placeholders::_1, std::placeholders::_2),
		m_buttonTextureMap[buttonType::HIDE], m_buttonTextureMap[buttonType::SHOW]);

	m_pCloseButton = AddButton(closeOffset, buttonWidth, buttonHeight,
		std::bind(&UserAreaControls::HandleClosePressed, this, std::placeholders::_1, std::placeholders::_2),
		m_buttonTextureMap[buttonType::CLOSE]);

	CN(m_pBackButton);
	CN(m_pForwardButton);
	CN(m_pCloseButton);
	CN(m_pURLButton);
	CN(m_pOpenButton);
	CN(m_pShareButton);
	CN(m_pMinimizeButton);
	CN(m_pKeyboardButton);
	CN(m_pCameraSourceButton);
	CN(m_pSendButton);

	// create text for title
	CR(InitializeText());

Error:
	return r;
}

RESULT UserAreaControls::InitializeText() {
	RESULT r = R_PASS;

	auto pFont = m_pDreamOS->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
	pFont->SetLineHeight(m_buttonHeight - (2.0f*m_spacingSize));

	auto textFlags = text::flags::TRAIL_ELLIPSIS | text::flags::RENDER_QUAD;
	m_pURLText = std::shared_ptr<text>(m_pDreamOS->MakeText(pFont,
		"",
		m_urlWidth - m_spacingSize,
		m_buttonHeight - (2.0f*m_spacingSize),
		textFlags));

	m_pURLText->RotateXByDeg(90.0f);
	m_pURLText->SetPosition(point(0.0f, 0.0f, 0.001f));
	m_pURLButton->AddObject(m_pURLText);

Error:
	return r;
}


RESULT UserAreaControls::Update() {

	if (m_fUpdateTitle) {

		GetURLText()->SetText(m_strUpdateTitle);
		GetURLText()->SetDirty();
		m_fUpdateTitle = false;
	}

	return R_PASS;
}

RESULT UserAreaControls::SetSharingFlag(bool fIsSharing) {
	RESULT r = R_PASS;
	
	m_pShareButton->SwitchToTexture(fIsSharing);

Error:
	return r;
}

RESULT UserAreaControls::SetTitleText(const std::string& strTitle) {
	RESULT r = R_PASS;

	m_fUpdateTitle = true;
	m_strUpdateTitle = strTitle;

	return r;
}

// ControlBarObserver
RESULT UserAreaControls::HandleBackPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	auto pBrowser = std::dynamic_pointer_cast<DreamBrowser>(m_pParentApp->GetActiveSource());
	CNR(pBrowser, R_SKIPPED);

	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	CR(pBrowser->HandleBackEvent());

Error:
	return R_PASS;
}

RESULT UserAreaControls::HandleForwardPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	auto pBrowser = std::dynamic_pointer_cast<DreamBrowser>(m_pParentApp->GetActiveSource());
	CNR(pBrowser, R_SKIPPED);

	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	CR(pBrowser->HandleForwardEvent());

Error:
	return R_PASS;
}

RESULT UserAreaControls::HandleShowTogglePressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	if (!pButtonContext->IsToggled()) {
		m_pParentApp->Maximize();
	}
	else {
		m_pParentApp->Minimize();
	}

	CR(pButtonContext->Toggle());

Error:
	return R_PASS;
}

RESULT UserAreaControls::HandleOpenPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	CR(m_pParentApp->Open());
	
	// This call triggers a move to menu, hiding the control bar, so the button needs to be forcefully released
	CR(m_pDreamOS->GetInteractionEngineProxy()->ResetObjects(pButtonContext->GetInteractionObject()));
	CR(m_pDreamOS->GetInteractionEngineProxy()->ReleaseObjects(pButtonContext->GetInteractionObject()));

Error:
	return R_PASS;
}

RESULT UserAreaControls::HandleClosePressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(m_pDreamOS->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	auto pActiveSource = m_pParentApp->GetActiveSource();

	CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");
	CN(pActiveSource);
	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);

	CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");
	CNR(pActiveSource, R_SKIPPED);	// double tapping close? 
	if (pActiveSource->GetContentType() == CAMERA_CONTENT_CONTROL_TYPE) {
		CRM(m_pEnvironmentControllerProxy->RequestCloseCamera(pActiveSource->GetCurrentAssetID()), "Failed to share environment asset");
	}
	else {
		CRM(m_pEnvironmentControllerProxy->RequestCloseAsset(pActiveSource->GetCurrentAssetID()), "Failed to share environment asset");
	}

Error:
	return R_PASS;
}

RESULT UserAreaControls::HandleShareTogglePressed(UIButton *pButtonContext, void *pContext) {
	RESULT r = R_PASS;
	
	auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(m_pDreamOS->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	auto pActiveSource = m_pParentApp->GetActiveSource();

	CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");
	CN(pActiveSource);

	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);

	if (pButtonContext->IsToggled()) {
		CRM(m_pEnvironmentControllerProxy->RequestStopSharing(m_pParentApp->GetCurrentScreenShare()), "Failed to share environment asset");

	}
	else {
		CR(m_pParentApp->ForceStopSharing());
		CR(m_pDreamOS->SetSharedContentTexture(pActiveSource->GetSourceTexture()));
		CRM(m_pEnvironmentControllerProxy->RequestShareAsset(pActiveSource->GetCurrentAssetID(), SHARE_TYPE_SCREEN), "Failed to share environment asset");
	}

	CR(pButtonContext->Toggle());

Error:
	return r;
}

RESULT UserAreaControls::HandleURLPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);

Error:
	return R_PASS;
}

RESULT UserAreaControls::HandleKeyboardPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	CR(m_pParentApp->ShowDesktopKeyboard());

Error:
	return R_PASS;
}

RESULT UserAreaControls::HandleSourceTogglePressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	//CBR(m_pParentApp->IsSharingScreen(), R_SKIPPED);
	CBR(m_fIsSharing, R_SKIPPED);
	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	CR(pButtonContext->Toggle());
	if (pButtonContext->IsToggled()) {
		CR(m_pParentApp->SetVirtualCameraSource(DreamVCam::SourceType::SHARE_SCREEN));
	}
	else {
		CR(m_pParentApp->SetVirtualCameraSource(DreamVCam::SourceType::CAMERA));
	}

Error:
	return r;
}

RESULT UserAreaControls::HandleSendTogglePressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	CR(pButtonContext->Toggle());

	CR(m_pParentApp->MuteVirtualCamera(!pButtonContext->IsToggled()));

Error:
	return r;
}

RESULT UserAreaControls::UpdateControlBarButtonsWithType(std::string strContentType) {
	RESULT r = R_PASS;

	if (m_strCurrentContentType != strContentType) {

		// hide buttons for previous layout, then show buttons for new layout
		CR(UpdateButtonVisibility(m_strCurrentContentType, false));

		m_strCurrentContentType = strContentType;
		CR(UpdateButtonVisibility(m_strCurrentContentType, true));
	}

	if (m_pParentApp != nullptr) {
		bool fIsSharing = (m_pParentApp->GetActiveSource()->GetSourceTexture() == m_pDreamOS->GetSharedContentTexture());
		fIsSharing = (fIsSharing && m_pDreamOS->IsSharing());

		CR(SetSharingFlag(fIsSharing));

		auto pBrowser = dynamic_cast<DreamBrowser*>(m_pParentApp->GetActiveSource().get());
		if (pBrowser != nullptr) {
			CR(pBrowser->UpdateNavigationFlags());
		}

		// TODO: logic for 
	}

Error:
	return r;
}

RESULT UserAreaControls::UpdateButtonVisibility(std::string strContentType, bool fVisible) {
	RESULT r = R_PASS;

	if (strContentType == CONTENT_TYPE_BROWSER) {
		m_pBackButton->SetVisible(fVisible);
		m_pForwardButton->SetVisible(fVisible);
		m_pShareButton->SetVisible(fVisible);
	}

	else if (strContentType == CONTENT_TYPE_CAMERA) {
		m_pCameraSourceButton->SetVisible(fVisible);
		m_pSendButton->SetVisible(fVisible);
	}

	else if (strContentType == CONTENT_TYPE_DESKTOP) {
		m_pShareButton->SetVisible(fVisible);
		m_pKeyboardButton->SetVisible(fVisible);
	}

	
	return r;
}

RESULT UserAreaControls::UpdateNavigationButtons(bool fCanGoBack, bool fCanGoForward) {
	RESULT r = R_PASS;

	CR(m_pBackButton->SetEnabledFlag(fCanGoBack));
	CR(m_pForwardButton->SetEnabledFlag(fCanGoForward));

Error:
	return r;
}

RESULT UserAreaControls::UpdateIsSharing(bool fSharing) {
	RESULT r = R_PASS;

	// TODO: would prefer that SetEnabled/Disabled texture calls didn't exist, however today
	// the UIButton architecture supports buttons with up to two states through toggling,
	// and m_pCameraSourceButton has three states, a toggle conditional on whether there is shared content

	m_fIsSharing = fSharing;

	if (fSharing) {
		m_pCameraSourceButton->SetDisabledTexture(m_buttonTextureMap[buttonType::SOURCE_CAMERA]);
	}
	else {
		m_pCameraSourceButton->SetDisabledTexture(m_buttonTextureMap[buttonType::SOURCE_NO_SHARE]);

		// switch button off if no longer sharing
		if (m_pCameraSourceButton->IsToggled()) {
			CR(m_pCameraSourceButton->Toggle());
		}
	}

Error:
	return r;
}

std::shared_ptr<text> UserAreaControls::GetURLText() {
	return m_pURLText;
}

RESULT UserAreaControls::Show() {
	RESULT r = R_PASS;

	SetVisible(true, false);	

	m_pMinimizeButton->SwitchToTexture(true);

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		this,
		color(1.0f, 1.0f, 1.0f, 1.0f),
		m_pParentApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags()
	));

Error:
	return r;
}

RESULT UserAreaControls::Hide() {
	RESULT r = R_PASS;

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		SetVisible(false, false);

		return r;
	};

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		this,
		color(1.0f, 1.0f, 1.0f, 0.0f),
		m_pParentApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags(),
		nullptr,
		fnEndCallback,
		this
	));

Error:
	return r;
}