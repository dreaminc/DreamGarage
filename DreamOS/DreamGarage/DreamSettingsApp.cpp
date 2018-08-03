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
	CR(GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_MENU_UP, this));

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
		if (m_pUserApp != nullptr) {
			m_pUserApp->ResetAppComposite();
		}
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
	CBR(m_fRespondToController, R_SKIPPED);
	CNR(m_pUserApp, R_SKIPPED);

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

RESULT DreamSettingsApp::HandleDreamFormSuccess() {
	RESULT r = R_PASS;

	//CR(DreamFormApp::HandleDreamFormSuccess());
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
		m_fRespondToController = true;
		m_fPendShowFormView = false;
	}

Error:
	return r;
}

RESULT DreamSettingsApp::Hide() {
	RESULT r = R_PASS;

	CR(DreamFormApp::Hide());
	m_fRespondToController = false;

Error:
	return r;
}

std::string DreamSettingsApp::GetSuccessString() {
	return m_strSuccess;
}