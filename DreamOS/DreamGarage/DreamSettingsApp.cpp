#include "DreamSettingsApp.h"

#include "RESULT/EHM.h"

#include "DreamUserControlArea/DreamContentSource.h"
#include "DreamUserControlArea/DreamUserControlArea.h"
#include "DreamControlView/UIControlView.h"

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

	if (m_fPendShowFormView) {
		GetComposite()->SetVisible(true, false);
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

RESULT DreamSettingsApp::Notify(SenseControllerEvent *pEvent) {
	RESULT r = R_PASS;

	//TODO: unregister/register instead of this flag?
	CBR(m_fFormVisible, R_SKIPPED);

Error:
	return r;
}

RESULT DreamSettingsApp::Notify(InteractionObjectEvent *pEvent) {
	RESULT r = R_PASS;

	DreamUserObserver *pEventApp = GetDOS()->GetUserApp()->m_pEventApp;
	CBR(pEventApp == m_pFormView.get(), R_SKIPPED);

	if (pEvent->m_eventType == INTERACTION_EVENT_MENU) {
		auto pCloudController = GetDOS()->GetCloudController();
		if (pCloudController != nullptr &&
			pCloudController->IsUserLoggedIn() &&
			pCloudController->IsEnvironmentConnected()) {

			if (GetDOS()->GetKeyboardApp()->IsVisible()) {
				CR(m_pDreamBrowserForm->HandleUnfocusEvent());
				CR(m_pFormView->HandleKeyboardDown());
			}
			else {
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
	GetDOS()->GetUserApp()->SetPreviousApp(nullptr);
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