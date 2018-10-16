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

	m_buttonWidth = buttonWidth;
	m_buttonHeight = buttonHeight;
	m_spacingSize = spacingSize;

	// button positioning
	float start = -width / 2.0f - spacingSize/2.0f;

	float backOffset = start + buttonWidth / 2.0f;
	float forwardOffset = backOffset + spacingSize + buttonWidth;
	float closeOffset = forwardOffset + spacingSize + buttonWidth;

	float urlOffset = 0.0f;
	float shareOffset = urlOffset + (m_urlWidth * width / 2.0f) + buttonWidth / 2.0f + spacingSize;
	float openOffset = shareOffset + spacingSize + buttonWidth;
	float hideOffset = openOffset + spacingSize + buttonWidth;

	//GetDOS()->AddObjectToUIGraph(GetComposite());

	m_urlWidth *= width;

	SetPosition(0.0f, 0.0f, m_pParentApp->GetBaseHeight() / 2.0f + 2 * spacingSize + buttonWidth / 2.0f);
	RotateXByDeg(-90.0f);

	SetVisible(true);

	// this can be local, because all values are saved to specific buttons
	std::vector<std::shared_ptr<texture>> controlTextures;
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszBack));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszBackDisabled));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszForward));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszForwardDisabled));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszClose));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszURL));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszOpen));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszStopSharing));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszShare));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszHide));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszShow));

	// TODO: desktop logic
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszKeyboard));

	for (auto pTexture : controlTextures) {
		CN(pTexture);
	}

	m_pBackButton = AddButton(backOffset, buttonWidth, buttonHeight,
		std::bind(&UserAreaControls::HandleBackPressed, this, std::placeholders::_1, std::placeholders::_2),
		controlTextures[0], controlTextures[1]);

	m_pForwardButton = AddButton(forwardOffset, buttonWidth, buttonHeight,
		std::bind(&UserAreaControls::HandleForwardPressed, this, std::placeholders::_1, std::placeholders::_2),
		controlTextures[2], controlTextures[3]);

	m_pCloseButton = AddButton(closeOffset, buttonWidth, buttonHeight,
		std::bind(&UserAreaControls::HandleClosePressed, this, std::placeholders::_1, std::placeholders::_2),
		controlTextures[4]);

// Re-enable for selectability of the URL button
//	CR(AddButton(ControlBarButtonType::URL, urlOffset, m_urlWidth * width, 
//		std::bind(&UIContentControlBar::HandleURLPressed, this, std::placeholders::_1, std::placeholders::_2)));

	m_pURLButton = AddButton(urlOffset, m_urlWidth, buttonHeight, nullptr, controlTextures[5]);

	m_pOpenButton = AddButton(openOffset, buttonWidth, buttonHeight, 
		std::bind(&UserAreaControls::HandleOpenPressed, this, std::placeholders::_1, std::placeholders::_2),
		controlTextures[6]);

	m_pShareButton = AddButton(shareOffset, buttonWidth, buttonHeight,
		std::bind(&UserAreaControls::HandleShareTogglePressed, this, std::placeholders::_1, std::placeholders::_2),
		controlTextures[7], controlTextures[8]);

	m_pMinimizeButton = AddButton(hideOffset, buttonWidth, buttonHeight,
		std::bind(&UserAreaControls::HandleShowTogglePressed, this, std::placeholders::_1, std::placeholders::_2),
		controlTextures[9], controlTextures[10]);

	CN(m_pBackButton);
	CN(m_pForwardButton);
	CN(m_pCloseButton);
	CN(m_pURLButton);
	CN(m_pOpenButton);
	CN(m_pShareButton);
	CN(m_pMinimizeButton);

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
	//CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::BACK));
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
	//CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::FORWARD));
	CR(pBrowser->HandleForwardEvent());

Error:
	return R_PASS;
}

RESULT UserAreaControls::HandleShowTogglePressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	if (!pButtonContext->IsToggled()) {
	//	CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::MAXIMIZE));
		m_pParentApp->Maximize();
	}
	else {
		//CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::MINIMIZE));
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
	CRM(m_pEnvironmentControllerProxy->RequestCloseAsset(pActiveSource->GetCurrentAssetID()), "Failed to share environment asset");

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
		CRM(m_pEnvironmentControllerProxy->RequestStopSharing(pActiveSource->GetCurrentAssetID()), "Failed to share environment asset");
	}
	else {
		CR(m_pParentApp->ForceStopSharing());
		CR(m_pDreamOS->SetSharedContentTexture(pActiveSource->GetSourceTexture()));
		CRM(m_pEnvironmentControllerProxy->RequestShareAsset(pActiveSource->GetCurrentAssetID()), "Failed to share environment asset");
	}

	CR(pButtonContext->Toggle());

Error:
	return r;
}

RESULT UserAreaControls::HandleURLPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);

	// TODO: this button's event is currently unused
	/*
	CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::URL));
		Hide();
		m_pDreamUserApp->SetEventApp(m_pControlView.get());
		m_pControlView->ShowKeyboard();
		m_pControlView->m_fIsShareURL = true;
		//*/
Error:
	return R_PASS;
}

RESULT UserAreaControls::HandleKeyboardPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	// TODO: this button is currently unused
	/*
	CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::KEYBOARD));

		m_pDreamUserApp->SetEventApp(m_pControlView.get());
	//*/
Error:
	return R_PASS;
}

RESULT UserAreaControls::UpdateControlBarButtonsWithType(std::string strContentType) {
	RESULT r = R_PASS;

	// TODO: if source is desktop, switch around buttons
	// doing things like settings visibility was removed from this function in UIControlBar

	if (m_pParentApp != nullptr) {
		bool fIsSharing = (m_pParentApp->GetActiveSource()->GetSourceTexture().get() == m_pDreamOS->GetSharedContentTexture());
		fIsSharing = (fIsSharing && m_pDreamOS->IsSharing());

		CR(SetSharingFlag(fIsSharing));

		auto pBrowser = dynamic_cast<DreamBrowser*>(m_pParentApp->GetActiveSource().get());
		if (pBrowser != nullptr) {
			CR(pBrowser->UpdateNavigationFlags());
		}
	}

Error:
	return r;
}

RESULT UserAreaControls::UpdateNavigationButtons(bool fCanGoBack, bool fCanGoForward) {
	RESULT r = R_PASS;

	CR(m_pBackButton->SetEnabledFlag(fCanGoBack));
	CR(m_pForwardButton->SetEnabledFlag(fCanGoForward));

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