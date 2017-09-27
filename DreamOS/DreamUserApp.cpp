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

	// Subscribers (children)
	//for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
	//	CR(GetDOS()->RegisterEventSubscriber((InteractionEventType)(i), this));
	//}

	SetAppName("DreamUserApp");
	SetAppDescription("A Dream User App");

	// Set up the user asset
	/*
	SetNormalVector(vector(0.0f, 1.0f, 0.0f).Normal());
	m_pScreenQuad = GetComposite()->AddQuad(GetWidth(), GetHeight(), 1, 1, nullptr, GetNormal());
	m_pScreenQuad->SetMaterialAmbient(0.8f);
	*/

	GetComposite()->InitializeOBB();

	m_pVolume = GetComposite()->AddVolume(1.0f);
	CN(m_pVolume);

	m_pOrientationRay = GetComposite()->AddRay(point(0.0f), vector::kVector(-1.0f), 1.0f);
	CN(m_pOrientationRay);
	m_pOrientationRay->SetVisible(false);

	CR(GetDOS()->AddInteractionObject(m_pOrientationRay.get()));

	//GetDOS()->AddObjectToInteractionGraph(GetComposite());

	//for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
	//	CR(GetDOS()->RegisterEventSubscriber(GetComposite(), (InteractionEventType)(i), this));
	//}

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

Error:
	return r;
}

RESULT DreamUserApp::Notify(InteractionObjectEvent *mEvent) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}