#include "DreamSettingsApp.h"

#include "RESULT/EHM.h"

#include "DreamUserControlArea/DreamContentSource.h"
#include "DreamUserControlArea/DreamUserControlArea.h"
#include "DreamControlView/DreamControlView.h"
#include "DreamBrowser.h"
#include "DreamUserApp.h"

#include "UI/UISurface.h"

#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"


DreamSettingsApp::DreamSettingsApp(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamSettingsApp>(pDreamOS, pContext)
{
	// empty
}

DreamSettingsApp::~DreamSettingsApp() 
{
	RESULT r = R_PASS;

	CR(Shutdown());

Error:
	return;
}


// DreamApp Interface
RESULT DreamSettingsApp::InitializeApp(void *pContext) {

	return R_PASS;
}

RESULT DreamSettingsApp::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamSettingsApp::Update(void *pContext) {
	RESULT r = R_PASS;

	if (m_pUserApp == nullptr) {
		UID userAppUID = GetDOS()->GetUniqueAppUID("DreamUserApp");
		m_pUserApp = dynamic_cast<DreamUserApp*>(GetDOS()->CaptureApp(userAppUID, this));
		CNR(m_pUserApp, R_SKIPPED);

		m_pFormView = GetDOS()->LaunchDreamApp<DreamControlView>(this, false);
		m_pFormView->InitializeWithUserApp(m_pUserApp);

		m_pFormView->GetViewSurface()->RegisterSubscriber(UI_SELECT_BEGIN, this);
		m_pFormView->GetViewSurface()->RegisterSubscriber(UI_SELECT_MOVED, this);
		m_pFormView->GetViewSurface()->RegisterSubscriber(UI_SELECT_ENDED, this);
		m_pFormView->GetViewSurface()->RegisterSubscriber(UI_SCROLL, this);

		GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_PAD_MOVE, this);
		GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_TRIGGER_MOVE, this);
		GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_TRIGGER_DOWN, this);
		GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_TRIGGER_UP, this);
		GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_MENU_UP, this);

		GetDOS()->RegisterEventSubscriber(GetComposite(), INTERACTION_EVENT_MENU, this);
		GetDOS()->RegisterEventSubscriber(GetComposite(), INTERACTION_EVENT_KEY_DOWN, this);

		m_pFormView->Hide();

		//TODO: temporary
		GetComposite()->SetPosition(point(0.0f, -0.2f, 0.1f));

		float viewAngleRad = m_pUserApp->GetViewAngle() * (float)(M_PI) / 180.0f;
		quaternion qViewQuadOrientation = quaternion::MakeQuaternionWithEuler(viewAngleRad, 0.0f, 0.0f);
		GetComposite()->SetOrientation(qViewQuadOrientation);

		GetComposite()->AddObject(std::shared_ptr<composite>(m_pFormView->GetComposite()));
		//m_pFormView->Hide();

		m_pUserApp->GetComposite()->AddObject(std::shared_ptr<composite>(GetComposite()));

	}

	// there's fancier code around this in DreamUserControlArea, 
	// but we assume that there is only one piece of content here
	if (m_fInitBrowser) {
		m_fInitBrowser = false;

		m_pDreamBrowserForm = GetDOS()->LaunchDreamApp<DreamBrowser>(this);
		CN(m_pDreamBrowserForm);
		CR(m_pDreamBrowserForm->RegisterObserver(this));

		CR(m_pDreamBrowserForm->InitializeWithBrowserManager(m_pUserApp->GetBrowserManager(), m_strURL));
		CR(m_pDreamBrowserForm->SetURI(m_strURL));
	}

	/*
	if (m_pFormView != nullptr && m_pForm != nullptr && m_pForm->GetSourceTexture() != nullptr) {
		CR(m_pFormView->GetViewQuad()->SetDiffuseTexture(m_pForm->GetSourceTexture().get()));
	}
	//*/

	if (m_fLeftTriggerDown) {
		m_pUserApp->UpdateScale(m_pUserApp->GetScale() + m_scaleTick);
	}
	else if (m_fRightTriggerDown) {
		m_pUserApp->UpdateScale(m_pUserApp->GetScale() - m_scaleTick);
	}

Error:
	return r;
}

RESULT DreamSettingsApp::Shutdown(void *pContext) {
	return R_PASS;
}

DreamSettingsApp* DreamSettingsApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamSettingsApp *pDreamApp = new DreamSettingsApp(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamSettingsApp::InitializeSettingsForm(std::string strURL) {
	RESULT r = R_PASS;

	if (m_pDreamBrowserForm == nullptr) {
		m_strURL = strURL;
		m_fInitBrowser = true;
	}

	return r;
}

RESULT DreamSettingsApp::Show() {
	RESULT r = R_PASS;

	CNR(m_pFormView, R_SKIPPED);

	CR(m_pFormView->Show());
	//CR(m_pFormView->HandleKeyboardUp("", point(0.0f, 0.0f, 0.0f)));
	CR(m_pUserApp->SetEventApp(m_pFormView.get()));

	m_fRespondToController = true;

Error:
	return r;
}

RESULT DreamSettingsApp::Hide() {
	RESULT r = R_PASS;

	CNR(m_pFormView, R_SKIPPED);
	CNR(m_pDreamBrowserForm, R_SKIPPED);

	CR(m_pFormView->Hide());
	CR(m_pFormView->HandleKeyboardDown());
	CR(m_pUserApp->SetEventApp(nullptr));
	CR(m_pUserApp->SetHasOpenApp(false));

	m_fRespondToController = false;

Error:
	return r;
}

RESULT DreamSettingsApp::HandleAudioPacket(const AudioPacket &pendingAudioPacket, DreamContentSource *pContext) {
	RESULT r = R_PASS;

	auto pCloudController = GetDOS()->GetCloudController();
	if (pCloudController != nullptr) {
		CR(GetDOS()->BroadcastSharedAudioPacket(pendingAudioPacket));
	}

Error:
	return r;
}

RESULT DreamSettingsApp::UpdateControlBarText(std::string& strTitle) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamSettingsApp::UpdateControlBarNavigation(bool fCanGoBack, bool fCanGoForward) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamSettingsApp::UpdateContentSourceTexture(std::shared_ptr<texture> pTexture, DreamContentSource *pContext) {
	RESULT r = R_PASS;

	CNR(m_pFormView, R_SKIPPED);
	CR(m_pFormView->GetViewQuad()->SetDiffuseTexture(pTexture.get()));

Error:
	return r;
}

RESULT DreamSettingsApp::HandleNodeFocusChanged(std::string strInitial) {
	RESULT r = R_PASS;

	point ptLastEvent = m_pFormView->GetLastEvent();
	
	m_pUserApp->SetEventApp(m_pFormView.get());

	auto pKeyboard = dynamic_cast<UIKeyboard*>(m_pUserApp->GetKeyboard());
	CN(pKeyboard);

	CR(pKeyboard->ShowBrowserButtons());
	CR(m_pFormView->HandleKeyboardUp(strInitial));

Error:
	return r;
}

RESULT DreamSettingsApp::HandleDreamFormSuccess() {
	RESULT r = R_PASS;

	//pUserControllerProxy->RequestSetSettings(GetDOS()->GetHardwareID(),"HMDType.OculusRift", m_height, m_depth, m_scale);
	int a = 5;

	return r;
}

RESULT DreamSettingsApp::HandleCanTabNext(bool fCanNext) {
	RESULT r = R_PASS;
	
	auto pKeyboard = dynamic_cast<UIKeyboard*>(m_pUserApp->GetKeyboard());
	CN(pKeyboard);
	CR(pKeyboard->UpdateTabNextTexture(fCanNext));

Error:
	return r;
}

RESULT DreamSettingsApp::HandleCanTabPrevious(bool fCanPrevious) {
	RESULT r = R_PASS;
	
	auto pKeyboard = dynamic_cast<UIKeyboard*>(m_pUserApp->GetKeyboard());
	CN(pKeyboard);
	CR(pKeyboard->UpdateTabPreviousTexture(fCanPrevious));

Error:
	return r;
}

RESULT DreamSettingsApp::Notify(UIEvent *pUIEvent) {
	RESULT r = R_PASS;
	
	WebBrowserPoint wptContact;
	point ptContact;

	CNR(m_pFormView, R_SKIPPED);
	CBR(pUIEvent->m_pObj == m_pFormView->GetViewQuad().get(), R_SKIPPED);
	CNR(m_pDreamBrowserForm, R_SKIPPED);
	CBR(m_fRespondToController, R_SKIPPED);

	wptContact = GetRelativePointofContact(pUIEvent->m_ptEvent);
	ptContact = point(wptContact.x, wptContact.y, 0.0f);

	switch (pUIEvent->m_eventType) {
	case UI_SELECT_BEGIN: {
	//	CR(HideWebsiteTyping());
		//CR(m_pFormView->HandleKeyboardDown());
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
	}
	};

Error:
	return r;
}

//TODO: duplicated with UserControlArea, should be a part 
//	of how DreamBrowser and DreamDesktopApp respond to events

WebBrowserPoint DreamSettingsApp::GetRelativePointofContact(point ptContact) {
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

RESULT DreamSettingsApp::Notify(SenseControllerEvent *pEvent) {
	RESULT r = R_PASS;

	//TODO: unregister/register instead of this flag?
	CBR(m_fRespondToController, R_SKIPPED);

	if (pEvent->type == SENSE_CONTROLLER_MENU_UP && pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_RIGHT) {
		//auto pUserControllerProxy = dynamic_cast<UserControllerProxy*>(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
		//pUserControllerProxy->RequestSetSettings(GetDOS()->GetHardwareID(),"HMDType.OculusRift", m_height, m_depth, m_scale);
		//CR(Hide());
	}
	else if (pEvent->type == SENSE_CONTROLLER_PAD_MOVE) {
		float diff = pEvent->state.ptTouchpad.y() * 0.015f;
		if (pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_LEFT) {
			m_pUserApp->UpdateHeight(diff);
		}
		else {
			//TODO: scale depth in variable
			m_pUserApp->UpdateDepth(diff/4.0f);
		}
	}
	else if (pEvent->type == SENSE_CONTROLLER_TRIGGER_DOWN) {// && pEvent->state.triggerRange < 0.5f) {
		if (pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_LEFT) {
			m_fLeftTriggerDown = true;
		}

		if (pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_RIGHT) {
			m_fRightTriggerDown = true;
		}
	}
	else if (pEvent->type == SENSE_CONTROLLER_TRIGGER_UP) {
		if (pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_LEFT) {
			m_fLeftTriggerDown = false;
		}
		else {
			m_fRightTriggerDown = false;
		}
	}

Error:
	return r;
}

RESULT DreamSettingsApp::Notify(InteractionObjectEvent *pEvent) {
	RESULT r = R_PASS;

	DreamUserObserver *pEventApp = m_pUserApp->m_pEventApp;
	CBR(pEventApp == m_pFormView.get(), R_SKIPPED);

	switch (pEvent->m_eventType) {
	case INTERACTION_EVENT_MENU: {
		if (m_pUserApp->GetKeyboard()->IsVisible()) {
			CR(m_pFormView->HandleKeyboardDown());
		}
		else {
			CR(Hide());
		}
		
	} break;
	case INTERACTION_EVENT_KEY_DOWN: {

		char chkey = (char)(pEvent->m_value);
		CBR(chkey != 0x00, R_SKIPPED);	


		if (chkey == SVK_RETURN) {
			//CR(m_pFormView->HandleKeyboardDown());
			CR(Hide());
			CR(m_pDreamBrowserForm->OnKeyPress(chkey, true));
		}
		else if (chkey == SVK_TAB) {
			CR(m_pDreamBrowserForm->HandleTabEvent());
		}
		else if (chkey == SVK_SHIFTTAB) {
			CR(m_pDreamBrowserForm->HandleBackTabEvent());
		}
		else if (chkey == SVK_CLOSE) {
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