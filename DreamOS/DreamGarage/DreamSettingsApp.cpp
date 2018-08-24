#include "DreamSettingsApp.h"

#include "RESULT/EHM.h"

#include "DreamUserControlArea/DreamContentSource.h"
#include "DreamUserControlArea/DreamUserControlArea.h"
#include "DreamControlView/DreamControlView.h"

#include "DreamBrowser.h"
#include "DreamUserApp.h"


DreamSettingsApp::DreamSettingsApp(DreamOS *pDreamOS, void *pContext) :
	DreamFormApp(pDreamOS, pContext)
{
	// empty
}

DreamSettingsApp::~DreamSettingsApp() 
{
	RESULT r = R_PASS;

	CR(DreamFormApp::Shutdown());
	CR(Shutdown());

Error:
	return;
}


// DreamApp Interface
RESULT DreamSettingsApp::InitializeApp(void *pContext) {
	RESULT r = R_PASS;
	
	CR(GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_PAD_MOVE, this));
	CR(GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_TRIGGER_MOVE, this));
	CR(GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_TRIGGER_DOWN, this));
	CR(GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_TRIGGER_UP, this));
	
Error:
	return r;
}

RESULT DreamSettingsApp::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamSettingsApp::Update(void *pContext) {
	RESULT r = R_PASS;

	CR(DreamFormApp::Update());

	if (m_fLeftTriggerDown) {
		m_pUserApp->UpdateScale(m_pUserApp->GetScale() + m_scaleTick);
	}
	else if (m_fRightTriggerDown) {
		m_pUserApp->UpdateScale(m_pUserApp->GetScale() - m_scaleTick);
	}

	if (m_fPendShowFormView) {
		GetComposite()->SetVisible(true, false);
		CR(SetInitialSettingsValues());
		CR(Show());
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

RESULT DreamSettingsApp::SetInitialSettingsValues() {
	RESULT r = R_PASS;

	// Save initial settings values so that if the form is cancelled, 
	// the settings can be reset
	m_initialHeight = m_pUserApp->GetHeight();
	m_initialDepth = m_pUserApp->GetDepth();
	m_initialScale = m_pUserApp->GetScale();

	return r;
}

RESULT DreamSettingsApp::Notify(SenseControllerEvent *pEvent) {
	RESULT r = R_PASS;

	//TODO: unregister/register instead of this flag?
	CBR(m_fFormVisible, R_SKIPPED);
	CNR(m_pUserApp, R_SKIPPED);

	if (pEvent->type == SENSE_CONTROLLER_PAD_MOVE) {
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

	if (pEvent->m_eventType == INTERACTION_EVENT_MENU) {
		auto pCloudController = GetDOS()->GetCloudController();
		if (pCloudController != nullptr &&
			pCloudController->IsUserLoggedIn() &&
			pCloudController->IsEnvironmentConnected()) {

			if (m_pUserApp->GetKeyboard()->IsVisible()) {
				CR(m_pDreamBrowserForm->HandleUnfocusEvent());
				CR(m_pFormView->HandleKeyboardDown());
			}
			else {

				// Reset user app to values at the beginning of the form
				CR(m_pUserApp->UpdateHeight(m_initialHeight));
				CR(m_pUserApp->UpdateDepth(m_initialDepth));
				CR(m_pUserApp->UpdateScale(m_initialScale));

				CR(Hide());
			}
		}

	}
	else {
		CR(DreamFormApp::Notify(pEvent));
	}

Error:
	return r;
}

RESULT DreamSettingsApp::HandleDreamFormSuccess() {
	RESULT r = R_PASS;

	//CR(DreamFormApp::HandleDreamFormSuccess());
	m_pUserApp->SetPreviousApp(nullptr);
	CR(Hide());
	CR(GetDOS()->SendDOSMessage(m_strSuccess));

Error:
	return r;
}

RESULT DreamSettingsApp::Show() {
	RESULT r = R_PASS;

	if (m_pFormView == nullptr) {
		m_fPendShowFormView = true;
	}
	else {
		CR(SetInitialSettingsValues());
		CR(DreamFormApp::Show());
		m_fPendShowFormView = false;
	}

Error:
	return r;
}

RESULT DreamSettingsApp::Hide() {
	RESULT r = R_PASS;

	CR(DreamFormApp::Hide());

Error:
	return r;
}

std::string DreamSettingsApp::GetSuccessString() {
	return m_strSuccess;
}