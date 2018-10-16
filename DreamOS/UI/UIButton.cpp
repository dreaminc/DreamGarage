#include "UIButton.h"
#include "DreamOS.h"

UIButton::UIButton(HALImp *pHALImp, DreamOS *pDreamOS, float width, float height) :
UIView(pHALImp, pDreamOS, width, height)
{
	RESULT r = R_PASS;

	CR(Initialize());

	Validate();
	return;
Error:
	Invalidate();
	return;
}

UIButton::UIButton(HALImp *pHALImp, 
	DreamOS *pDreamOS, 
	std::shared_ptr<texture> pEnabledTexture,
	std::shared_ptr<texture> pDisabledTexture,
	float width, 
	float height) :

	UIView(pHALImp, pDreamOS, width, height),
	m_pEnabledTexture(pEnabledTexture),
	m_pDisabledTexture(pDisabledTexture)
{
	RESULT r = R_PASS;

	CR(Initialize());

	Validate();
	return;
Error:
	Invalidate();
	return;
}

UIButton::~UIButton() {

}

RESULT UIButton::Initialize() {
	RESULT r = R_PASS;

	for (int i = 0; i < (int)(UIEventType::UI_EVENT_INVALID); i++) {
		CR(RegisterSubscriber((UIEventType)(i), this));
	}

	m_pSurfaceComposite = AddComposite();
	m_pSurface = AddQuad(m_width, m_height, 1, 1, nullptr, vector::kVector());

	CN(m_pSurface);
	CR(InitializeOBB());

	if (m_pEnabledTexture != nullptr) {
		CR(m_pSurface->SetDiffuseTexture(m_pEnabledTexture.get()));
	}

Error:
	return r;
}

RESULT UIButton::RegisterToInteractionEngine(DreamOS *pDreamOS) {
	RESULT r = R_PASS;
	
	for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
		CR(pDreamOS->AddAndRegisterInteractionObject(this, (InteractionEventType)(i), this));
	}

Error:
	return r;
}

RESULT UIButton::RegisterEvent(UIEventType type, std::function<RESULT(UIButton*,void*)> fnCallback, void *pContext) {
	m_callbacks[type] = std::make_pair(fnCallback,pContext);
	return R_PASS;
}

RESULT UIButton::Notify(UIEvent *pEvent) {
	RESULT r = R_PASS;

	m_pInteractionObject = pEvent->m_pInteractionObject;
	m_ptContact = pEvent->m_ptEvent;

	std::function<RESULT(UIButton*,void*)> fnCallback;

	CBR(m_callbacks.count(pEvent->m_eventType) > 0, R_OBJECT_NOT_FOUND);

	fnCallback = m_callbacks[pEvent->m_eventType].first;
	void *pContext = m_callbacks[pEvent->m_eventType].second;

	CN(fnCallback);

	CR(fnCallback(this,pContext));

Error:
	return r;
}

std::shared_ptr<quad> UIButton::GetSurface() {
	return m_pSurface;
}

std::shared_ptr<composite> UIButton::GetSurfaceComposite() {
	return m_pSurfaceComposite;
}

VirtualObj *UIButton::GetInteractionObject() {
	return m_pInteractionObject;
}

point UIButton::GetContactPoint() {
	return m_ptContact;
}

RESULT UIButton::SetTextures(std::shared_ptr<texture> pEnabledTexture, std::shared_ptr<texture> pDisabledTexture) {
	m_pEnabledTexture = pEnabledTexture;
	m_pDisabledTexture = pDisabledTexture;
	return R_PASS;
}

RESULT UIButton::HandleTouchStart(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	//TODO: very similar to the code in DreamUIBar::HandleTouchStart
	std::shared_ptr<quad> pSurface = nullptr;
	vector vSurface;
	vector vRotation;
	quaternion qSurface;
	quaternion qRotation;
	
	// TODO: visibility check could be on trigger
	//CBR(IsVisible(), R_SKIPPED);

	CNR(pButtonContext, R_SKIPPED);
	CBR(pButtonContext->IsVisible(), R_SKIPPED);

	CBR(m_fInteractable, R_SKIPPED);

	pSurface = pButtonContext->GetSurface();

	//vector for captured object movement
	qSurface = pButtonContext->GetOrientation() * (pSurface->GetOrientation());
	qSurface.Reverse();
	vSurface = qSurface.RotateVector(pSurface->GetNormal() * -1.0f);

	//vector for captured object collisions
	qRotation = pSurface->GetOrientation(true);
	qRotation.Reverse();
	vRotation = qRotation.RotateVector(pSurface->GetNormal() * -1.0f);

	InteractionEngineProxy* pInteractionProxy;
	pInteractionProxy = m_pDreamOS->GetInteractionEngineProxy();
	pInteractionProxy->ResetObjects(pButtonContext->GetInteractionObject());
	pInteractionProxy->ReleaseObjects(pButtonContext->GetInteractionObject());

	pInteractionProxy->CaptureObject(
		pButtonContext,
		pButtonContext->GetInteractionObject(),
		pButtonContext->GetContactPoint(),
		vRotation,
		vSurface,
		0.02f);
	//	m_actuationDepth);

Error:
	return r;
}

RESULT UIButton::RegisterTouchStart() {
	RESULT r = R_PASS;

	CR(RegisterEvent(UIEventType::UI_SELECT_BEGIN,
		std::bind(&UIButton::HandleTouchStart, this, std::placeholders::_1, std::placeholders::_2)));
Error:
	return r;
}

RESULT UIButton::SetInteractability(bool fInteractable) {
	RESULT r = R_PASS;

	CR(SwitchToTexture(fInteractable));

	m_fInteractable = fInteractable;

Error:
	return r;
}

RESULT UIButton::Toggle() {
	RESULT r = R_PASS;

	CR(SwitchToTexture(!IsToggled()));

Error:
	return r;
}

bool UIButton::IsToggled() {
	RESULT r = R_PASS;

	std::shared_ptr<quad> pSurface = GetSurface();

	CN(pSurface);
	CN(m_pEnabledTexture);

	return pSurface->GetTextureDiffuse() == m_pEnabledTexture.get();

Error:
	return r;
}

RESULT UIButton::SwitchToTexture(bool fIsEnabledTexture) {
	RESULT r = R_PASS;

	std::shared_ptr<quad> pSurface = GetSurface();

	CN(pSurface);
	CN(m_pEnabledTexture);
	CN(m_pDisabledTexture);

	if (fIsEnabledTexture) {
		CR(pSurface->SetDiffuseTexture(m_pEnabledTexture.get()));
	}
	else {
		CR(pSurface->SetDiffuseTexture(m_pDisabledTexture.get()));
	}

Error:
	return r;
}