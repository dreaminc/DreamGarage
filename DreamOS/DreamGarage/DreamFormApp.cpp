#include "DreamFormApp.h"
#include "DreamControlView/DreamControlView.h"

#include "WebBrowser/DOMNode.h"
#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"
#include "Cloud/WebRequest.h"
#include "Core/Utilities.h"

#include "UI/UISurface.h"

DreamFormApp::DreamFormApp(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamFormApp>(pDreamOS, pContext)
{
	// empty
}

DreamFormApp::~DreamFormApp()
{
	RESULT r = R_PASS;

	CR(Shutdown());

Error:
	return;
}

// DreamApp Interface
RESULT DreamFormApp::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	return r;
}

RESULT DreamFormApp::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamFormApp::Update(void *pContext) {
	RESULT r = R_PASS;

	if (m_pUserApp == nullptr) {
		UID userAppUID = GetDOS()->GetUniqueAppUID("DreamUserApp");
		m_pUserApp = dynamic_cast<DreamUserApp*>(GetDOS()->CaptureApp(userAppUID, this));
		CNR(m_pUserApp, R_SKIPPED);

		CR(GetDOS()->RegisterEventSubscriber(GetComposite(), INTERACTION_EVENT_MENU, this));
		CR(GetDOS()->RegisterEventSubscriber(GetComposite(), INTERACTION_EVENT_KEY_DOWN, this));


		m_pFormView = GetDOS()->LaunchDreamApp<DreamControlView>(this, false);
		m_pFormView->InitializeWithUserApp(m_pUserApp);

		m_pFormView->GetViewSurface()->RegisterSubscriber(UI_SELECT_BEGIN, this);
		m_pFormView->GetViewSurface()->RegisterSubscriber(UI_SELECT_MOVED, this);
		m_pFormView->GetViewSurface()->RegisterSubscriber(UI_SELECT_ENDED, this);
		m_pFormView->GetViewSurface()->RegisterSubscriber(UI_SCROLL, this);

		//m_pFormView->Hide();
		m_pFormView->GetComposite()->SetVisible(false);
		GetComposite()->SetVisible(false, false);

		//TODO: values from DreamUserControlArea, can be deleted once there is further settings integration
		GetComposite()->SetPosition(point(0.0f, -0.2f, -0.1f));

		float viewAngleRad = m_pUserApp->GetViewAngle() * (float)(M_PI) / 180.0f;
		quaternion qViewQuadOrientation = quaternion::MakeQuaternionWithEuler(viewAngleRad, 0.0f, 0.0f);
		GetComposite()->SetOrientation(qViewQuadOrientation);

		GetComposite()->AddObject(std::shared_ptr<composite>(m_pFormView->GetComposite()));

		m_pUserApp->GetComposite()->AddObject(std::shared_ptr<composite>(GetComposite()));
	}

	// there's fancier code around this in DreamUserControlArea, 
	// but we assume that there is only one piece of content here
	if (m_fInitBrowser) {
		DOSLOG(INFO, "Creating browser app for form: %s", m_strURL);
		m_fInitBrowser = false;

		m_pDreamBrowserForm = GetDOS()->LaunchDreamApp<DreamBrowser>(this);
		CN(m_pDreamBrowserForm);
		CR(m_pDreamBrowserForm->RegisterObserver(this));

		CR(m_pDreamBrowserForm->InitializeWithBrowserManager(m_pUserApp->GetBrowserManager(), m_strURL));

		CR(m_pDreamBrowserForm->SetURI(m_strURL));

		DOSLOG(INFO, "Created browser app for form: %s", m_strURL);
	}
	if (m_fUpdateFormURL) {
		m_fUpdateFormURL = false;

		std::multimap<std::string, std::string> headerMap;
		m_pDreamBrowserForm->CheckForHeaders(headerMap, m_strURL);

		WebRequest webRequest;
		webRequest.SetURL(util::StringToWideString(m_strURL));
		webRequest.SetRequestMethod(WebRequest::Method::GET);

		std::multimap<std::wstring, std::wstring> requestHeaders;
		for (std::multimap<std::string, std::string>::iterator itr = headerMap.begin(); itr != headerMap.end(); ++itr) {

			std::string strKey = itr->first;
			std::string strValue = itr->second;

			requestHeaders.insert(std::pair<std::wstring, std::wstring>(util::StringToWideString(strKey), util::StringToWideString(strValue)));
		}
		webRequest.SetRequestHeaders(requestHeaders);

//		m_pDreamBrowserForm->LoadRequest(webRequest);
		m_pDreamBrowserForm->SetURI(m_strURL);
	}

	if (m_fSetAsActive) {
		m_pUserApp->SetHasOpenApp(true);
		m_pUserApp->SetEventApp(m_pFormView.get());
		m_fSetAsActive = false;
	}

	if (m_fPendShowFormView) {
		GetComposite()->SetVisible(true, false);
		CR(Show());
	}

Error:
	return r;
}

RESULT DreamFormApp::Shutdown(void *pContext) {

	if (m_pDreamBrowserForm != nullptr) {
		m_pDreamBrowserForm->CloseSource();
		GetDOS()->ShutdownDreamApp<DreamBrowser>(m_pDreamBrowserForm);
		m_pDreamBrowserForm = nullptr;
	}

	return R_PASS;
}

DreamFormApp* DreamFormApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamFormApp *pDreamApp = new DreamFormApp(pDreamOS, pContext);
	return pDreamApp;
}

std::string DreamFormApp::StringFromType(FormType type) {

	std::string strType;

	if (type == FormType::SIGN_IN) {
		strType = "FormKey.UsersSignIn";
	}
	else if (type == FormType::SIGN_UP) {
		strType = "FormKey.UsersSignUp";
	}
	else if (type == FormType::SETTINGS) {
		strType = "FormKey.UsersSettings";
	}
	else if (type == FormType::TEAMS_MISSING) {
		strType = "FormKey.TeamsMissing";
	}

	return strType;
}

FormType DreamFormApp::TypeFromString(std::string& strType) {

	FormType type = FormType::DEFAULT;

	if (strType == "FormKey.UsersSignIn") {
		type = FormType::SIGN_IN;
	}
	else if (strType == "FormKey.UsersSignUp") {
		type = FormType::SIGN_UP;
	}
	else if (strType == "FormKey.UsersSettings") {
		type = FormType::SETTINGS;
	}
	else if (strType == "FormKey.TeamsMissing") {
		type = FormType::TEAMS_MISSING;
	}

	return type;
}

RESULT DreamFormApp::UpdateWithNewForm(std::string strURL) {
	RESULT r = R_PASS;

#ifndef _DEBUG
	m_strURL = strURL;
	if (m_pDreamBrowserForm == nullptr) {
		m_fInitBrowser = true;
		DOSLOG(INFO, "Create browser for form: %s", m_strURL);
	}
	else {
		m_fUpdateFormURL = true;
	}
#endif

	return r;
}

RESULT DreamFormApp::HandleAudioPacket(const AudioPacket &pendingAudioPacket, DreamContentSource *pContext) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamFormApp::UpdateControlBarText(std::string& strTitle) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamFormApp::UpdateControlBarNavigation(bool fCanGoBack, bool fCanGoForward) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamFormApp::UpdateContentSourceTexture(std::shared_ptr<texture> pTexture, DreamContentSource *pContext) {
	RESULT r = R_PASS;

	CNR(m_pFormView, R_SKIPPED);
	CR(m_pFormView->GetViewQuad()->SetDiffuseTexture(pTexture.get()));

Error:
	return r;
}

RESULT DreamFormApp::HandleNodeFocusChanged(DOMNode *pDOMNode, DreamContentSource *pContext) {
	RESULT r = R_PASS;

	bool fMaskPasswordEnabled = false;

	UIKeyboard* pKeyboard = dynamic_cast<UIKeyboard*>(m_pUserApp->GetKeyboard());
	CN(pKeyboard);

	CN(pDOMNode);

	if (pDOMNode->GetType() == DOMNode::type::ELEMENT && pDOMNode->IsEditable()) {
		m_pUserApp->SetEventApp(m_pFormView.get());
		fMaskPasswordEnabled = pDOMNode->IsPassword();

		CR(pKeyboard->ShowBrowserButtons());
		CR(m_pFormView->HandleKeyboardUp());

		std::string strTextField = pDOMNode->GetValue();
		pKeyboard->PopulateKeyboardTextBox(strTextField);
	}

	pKeyboard->SetPasswordFlag(fMaskPasswordEnabled);

Error:
	return r;
}

RESULT DreamFormApp::HandleIsInputFocused(bool fIsFocused, DreamContentSource *pContext) {
	RESULT r = R_PASS;

	if (fIsFocused) {
		m_pUserApp->SetEventApp(m_pFormView.get());

		auto pKeyboard = dynamic_cast<UIKeyboard*>(m_pUserApp->GetKeyboard());
		CN(pKeyboard);

		CR(pKeyboard->ShowBrowserButtons());
		CR(m_pFormView->HandleKeyboardUp());
	}
	else {
		CR(m_pDreamBrowserForm->HandleUnfocusEvent());
		CR(m_pFormView->HandleKeyboardDown());
	}

Error:
	return r;
}

RESULT DreamFormApp::HandleDreamFormSuccess() {
	RESULT r = R_PASS;

	m_pUserApp->SetPreviousApp(nullptr);
	CR(Hide());
	CR(GetDOS()->SendDOSMessage(m_strSuccess));

Error:
	return r;
}

RESULT DreamFormApp::HandleDreamFormCancel() {
	RESULT r = R_PASS;

	CR(m_pUserApp->SetHasOpenApp(false));
	CR(Hide());

Error:
	return r;
}

RESULT DreamFormApp::HandleCanTabNext(bool fCanNext) {
	RESULT r = R_PASS;
	
	auto pKeyboard = dynamic_cast<UIKeyboard*>(m_pUserApp->GetKeyboard());
	CN(pKeyboard);
	CR(pKeyboard->UpdateTabNextTexture(fCanNext));

Error:
	return r;
}

RESULT DreamFormApp::HandleCanTabPrevious(bool fCanPrevious) {
	RESULT r = R_PASS;
	
	auto pKeyboard = dynamic_cast<UIKeyboard*>(m_pUserApp->GetKeyboard());
	CN(pKeyboard);
	CR(pKeyboard->UpdateTabPreviousTexture(fCanPrevious));

Error:
	return r;
}

RESULT DreamFormApp::SetAsActive() {
	m_fSetAsActive = true;
	return R_PASS;
}

RESULT DreamFormApp::Notify(InteractionObjectEvent *pEvent) {
	RESULT r = R_PASS;

	DreamUserObserver *pEventApp = m_pUserApp->m_pEventApp;
	CBR(pEventApp == m_pFormView.get(), R_SKIPPED);

	switch (pEvent->m_eventType) {

	case INTERACTION_EVENT_MENU: {
		auto pCloudController = GetDOS()->GetCloudController();
		if (pCloudController != nullptr && 
			pCloudController->IsUserLoggedIn() && 
			pCloudController->IsEnvironmentConnected()) {

			if (m_pUserApp->GetKeyboard()->IsVisible()) {
				CR(m_pDreamBrowserForm->HandleUnfocusEvent());
				CR(m_pFormView->HandleKeyboardDown());
			}
			else {
				CR(m_pUserApp->SetHasOpenApp(false));
				CR(Hide());
			}
		}
		else {
			if (m_pFormView != nullptr && !m_pFormView->GetViewQuad()->IsVisible()) {
				Show();
			}
			m_pUserApp->ResetAppComposite();
		}
		
	} break;

	case INTERACTION_EVENT_KEY_DOWN: {

		char chkey = (char)(pEvent->m_value);
		CBR(chkey != 0x00, R_SKIPPED);	
		CNR(m_pDreamBrowserForm, R_SKIPPED);


		if (chkey == SVK_RETURN) {
			CR(m_pDreamBrowserForm->OnKeyPress(chkey, true));
		}
		else if (chkey == SVK_TAB) {
			CR(m_pDreamBrowserForm->HandleTabEvent());
		}
		else if (chkey == SVK_SHIFTTAB) {
			CR(m_pDreamBrowserForm->HandleBackTabEvent());
		}
		else if (chkey == SVK_CLOSE) {
			CR(m_pDreamBrowserForm->HandleUnfocusEvent());
			CR(m_pFormView->HandleKeyboardDown());
		}
		else {
			CR(m_pDreamBrowserForm->OnKeyPress(chkey, true));
		}

	} break;
	}

Error:
	return r;
}

//TODO: duplicated with UserControlArea, should be a part 
//	of how DreamBrowser and DreamDesktopApp respond to events

WebBrowserPoint DreamFormApp::GetRelativePointofContact(point ptContact) {
	point ptIntersectionContact = ptContact;
	ptIntersectionContact.w() = 1.0f;
	WebBrowserPoint ptRelative;

	// First apply transforms to the ptIntersectionContact 
	//point ptAdjustedContact = inverse(m_pViewQuad->GetModelMatrix()) * ptIntersectionContact;
	point ptAdjustedContact = ptIntersectionContact;
	
	float width = m_pFormView->GetViewQuad()->GetWidth();
	float height = m_pFormView->GetViewQuad()->GetHeight();

	float posX = ptAdjustedContact.x() / (width / 2.0f);	
	float posY = ptAdjustedContact.z() / (height / 2.0f);

	//float posZ = ptAdjustedContact.z();	// 3D browser when

	posX = (posX + 1.0f) / 2.0f;	// flip it
	posY = (posY + 1.0f) / 2.0f;  
	
	ptRelative.x = posX * m_pDreamBrowserForm->GetWidth();
	ptRelative.y = posY * m_pDreamBrowserForm->GetHeight();

	return ptRelative;
}

RESULT DreamFormApp::Notify(UIEvent *pUIEvent) {
	RESULT r = R_PASS;
	
	WebBrowserPoint wptContact;
	point ptContact;

	CNR(m_pFormView, R_SKIPPED);
	CBR(pUIEvent->m_pObj == m_pFormView->GetViewQuad().get(), R_SKIPPED);
	CNR(m_pDreamBrowserForm, R_SKIPPED);

	wptContact = GetRelativePointofContact(pUIEvent->m_ptEvent);
	ptContact = point(wptContact.x, wptContact.y, 0.0f);

	switch (pUIEvent->m_eventType) {
	case UI_SELECT_BEGIN: {
		if (m_pUserApp->GetKeyboard()->IsVisible()) {
			CR(m_pDreamBrowserForm->HandleUnfocusEvent());
			CR(m_pFormView->HandleKeyboardDown());
		}
		CR(m_pDreamBrowserForm->OnClick(ptContact, true));
	} break;

	case UI_SELECT_ENDED: {
		CR(m_pDreamBrowserForm->OnClick(ptContact, false));
	} break;

	case UI_SELECT_MOVED: {
		CR(m_pDreamBrowserForm->OnMouseMove(ptContact));
	} break;

	case UI_SCROLL: {
		CR(m_pDreamBrowserForm->OnScroll(pUIEvent->m_vDelta.x(), pUIEvent->m_vDelta.y(), ptContact));
	} break;
	};

Error:
	return r;
}

RESULT DreamFormApp::Show() {
	RESULT r = R_PASS;

	//CNR(m_pFormView, R_SKIPPED);
	if (m_pFormView == nullptr) {
		m_fPendShowFormView = true;
	}
	else {
		CR(m_pFormView->Show());
		CR(m_pUserApp->SetEventApp(m_pFormView.get()));
		CR(m_pUserApp->SetHasOpenApp(true));	// For login/logout
		m_fFormVisible = true;
		m_fPendShowFormView = false;
	}

Error:
	return r;
}

RESULT DreamFormApp::Hide() {
	RESULT r = R_PASS;

	CNR(m_pFormView, R_SKIPPED);
	CNR(m_pDreamBrowserForm, R_SKIPPED);

	CR(m_pFormView->Hide());
	m_fFormVisible = false;
	CR(m_pFormView->HandleKeyboardDown());
	CR(m_pUserApp->SetEventApp(nullptr));

//	m_pDreamBrowserForm->Shutdown();
	m_pDreamBrowserForm->CloseSource();
	GetDOS()->ShutdownDreamApp<DreamBrowser>(m_pDreamBrowserForm);
	m_pDreamBrowserForm = nullptr;

Error:
	return r;
}
