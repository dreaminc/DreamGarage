#include "DreamUserApp.h"
#include "DreamOS.h"

DreamUserApp::DreamUserApp(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamUserApp>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

DreamUserApp* DreamUserApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamUserApp *pDreamApp = new DreamUserApp(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamUserApp::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	SetAppName("DreamUserApp");
	SetAppDescription("A Dream User App");

	GetComposite()->InitializeOBB();

	GetDOS()->AddObjectToInteractionGraph(GetComposite());

	CR(GetDOS()->RegisterEventSubscriber(GetComposite(), ELEMENT_INTERSECT_BEGAN, this));
	CR(GetDOS()->RegisterEventSubscriber(GetComposite(), ELEMENT_INTERSECT_MOVED, this));
	CR(GetDOS()->RegisterEventSubscriber(GetComposite(), ELEMENT_INTERSECT_ENDED, this));

Error:
	return r;
}

RESULT DreamUserApp::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamUserApp::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamUserApp::Update(void *pContext) {
	RESULT r = R_PASS;

	//CR(r);
	auto pCameraNode = GetDOS()->GetCameraNode();
	CN(pCameraNode);

	GetComposite()->SetPosition(pCameraNode->GetPosition());

	quaternion qOrientation = (pCameraNode->GetOrientation());
	qOrientation.Reverse();
	GetComposite()->SetOrientation(qOrientation);

	if (m_pVolume == nullptr) {
		m_pVolume = GetComposite()->AddVolume(1.0f);
		CN(m_pVolume);
		m_pVolume->SetVisible(false);
	}
	
	if (m_pOrientationRay == nullptr) {
		m_pOrientationRay = GetComposite()->AddRay(point(0.0f, 0.0f, -0.75f), vector::kVector(-1.0f), 1.0f);
		CN(m_pOrientationRay);
		m_pOrientationRay->SetVisible(false);
		CR(GetDOS()->AddInteractionObject(m_pOrientationRay.get()));
	}

Error:
	return r;
}

RESULT DreamUserApp::Notify(InteractionObjectEvent *mEvent) {
	RESULT r = R_PASS;

	CBR((mEvent->m_pInteractionObject != m_pOrientationRay.get()), R_SKIPPED);

	int a = 5;

Error:
	return r;
}