#include "DreamSettingsApp.h"

#include "RESULT/EHM.h"

#include "DreamUserControlArea/DreamContentSource.h"
#include "DreamUserControlArea/DreamUserControlArea.h"
#include "DreamControlView/DreamControlView.h"
#include "DreamBrowser.h"
#include "DreamUserApp.h"

#include "UI/UISurface.h"

//#include "CEFBrowser/CEFBrowserManager.h"
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
		m_pFormView->InitializeWithParent(m_pUserApp);

		m_pFormView->GetViewSurface()->RegisterSubscriber(UI_SELECT_BEGIN, this);
		m_pFormView->GetViewSurface()->RegisterSubscriber(UI_SELECT_MOVED, this);
		m_pFormView->GetViewSurface()->RegisterSubscriber(UI_SELECT_ENDED, this);
		m_pFormView->GetViewSurface()->RegisterSubscriber(UI_SCROLL, this);

		GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_PAD_MOVE, this);
		GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_TRIGGER_MOVE, this);
		GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_TRIGGER_DOWN, this);
		GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_TRIGGER_UP, this);
		GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_MENU_UP, this);
//		m_pFormView->Hide();

		//TODO: temporary
		GetComposite()->SetPosition(point(0.0f, -0.2f, 0.1f));

		float viewAngleRad = m_pUserApp->GetViewAngle() * (float)(M_PI) / 180.0f;
		quaternion qViewQuadOrientation = quaternion::MakeQuaternionWithEuler(viewAngleRad, 0.0f, 0.0f);
		GetComposite()->SetOrientation(qViewQuadOrientation);

		GetComposite()->AddObject(std::shared_ptr<composite>(m_pFormView->GetComposite()));
		m_pFormView->Hide();

		m_pUserApp->GetComposite()->AddObject(std::shared_ptr<composite>(GetComposite()));

	}

	// there's fancier code around this in DreamUserControlArea, 
	// but we assume that there is only one piece of content here
	if (m_pForm != nullptr) {
		m_pForm->PendUpdateObjectTextures();
	}
	if (m_pFormView != nullptr && m_pForm != nullptr && m_pForm->GetSourceTexture() != nullptr) {
		//CR(m_pFormView->SetViewQuadTexture(m_pForm->GetSourceTexture()));
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

//	auto pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
//	CNM(pEnvironmentControllerProxy, "Failed to get environment controller proxy");
//	CRM(pEnvironmentControllerProxy->RequestOpenAsset("WebsiteProviderScope.WebsiteProvider", strURL, "website"), "Failed to share environment asset");

	m_strURL = strURL;
	m_strURL = "www.twitch.tv";

	m_pForm = GetDOS()->LaunchDreamApp<DreamBrowser>(this);

	m_pForm->SetScope("WebsiteProviderScope.WebsiteProvider");
	m_pForm->SetPath(m_strURL);

	m_pForm->InitializeWithBrowserManager(m_pUserApp->GetBrowserManager(), m_strURL);
	m_pForm->InitializeWithForm();

//Error:
	return r;
}

RESULT DreamSettingsApp::Show() {
	RESULT r = R_PASS;

	CNR(m_pFormView, R_SKIPPED);
	CNR(m_pForm, R_SKIPPED);

	m_pFormView->Show();
	m_pFormView->HandleKeyboardUp("", point(0.0f, 0.0f, 0.0f));
	//m_pFormView->ResetAppComposite();

Error:
	return r;
}

RESULT DreamSettingsApp::Notify(UIEvent *pUIEvent) {
	RESULT r = R_PASS;
	
	WebBrowserPoint wptContact = GetRelativePointofContact(pUIEvent->m_ptContact);
	point ptContact = point(wptContact.x, wptContact.y, 0.0f);

	CNR(m_pFormView, R_SKIPPED);
	CBR(pUIEvent->m_pObj == m_pFormView->GetViewQuad().get(), R_SKIPPED);
	CNR(m_pForm, R_SKIPPED);

	switch (pUIEvent->m_eventType) {
	case UI_SELECT_BEGIN: {
	//	CR(HideWebsiteTyping());
		//CR(m_pFormView->HandleKeyboardDown());
		CR(m_pForm->OnClick(ptContact, true));
	} break;

	case UI_SELECT_ENDED: {
		CR(m_pForm->OnClick(ptContact, false));
	} break;

	case UI_SELECT_MOVED: {
		CR(m_pForm->OnMouseMove(ptContact));
	} break;
	case UI_SCROLL: {
		CR(m_pForm->OnScroll(pUIEvent->m_ptScroll.x(), pUIEvent->m_ptScroll.y(), ptContact));
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
	
	ptRelative.x = posX * m_pForm->GetWidth();
	ptRelative.y = posY * m_pForm->GetHeight();

	return ptRelative;
}

RESULT DreamSettingsApp::Notify(SenseControllerEvent *pEvent) {
	RESULT r = R_PASS;

	if (pEvent->type == SENSE_CONTROLLER_MENU_UP && pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_RIGHT) {
		auto pUserControllerProxy = dynamic_cast<UserControllerProxy*>(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
		pUserControllerProxy->RequestSetSettings(GetDOS()->GetHardwareID(),"HMDType.OculusRift", m_height, m_depth, m_scale);
	}
	else if (pEvent->type == SENSE_CONTROLLER_PAD_MOVE) {
		float diff = pEvent->state.ptTouchpad.y() * 0.015f;
		if (pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_LEFT) {
			m_pUserApp->UpdateHeight(diff);
			m_height += diff;
		}
		else {
			//TODO scale depth in variable
			m_pUserApp->UpdateDepth(diff/4.0f);
			m_depth += diff/4.0f;
		}
	}
	else if (pEvent->type == SENSE_CONTROLLER_TRIGGER_DOWN) {// && pEvent->state.triggerRange < 0.5f) {
		if (pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_LEFT) {
			m_scale = m_pUserApp->GetWidthScale() + 0.003f;
		}
		else {
			m_scale = m_pUserApp->GetWidthScale() - 0.003f;
		}
		m_pUserApp->UpdateWidthScale(m_scale);
	}
//Error:
	return r;
}