#include "DreamControlView.h"
#include "DreamBrowser.h"
#include "DreamOS.h"
#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"
#include "DreamConsole/DreamConsole.h"

#include "UI/UIMallet.h"
#include "UI/UIView.h"

DreamControlView::DreamControlView(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamControlView>(pDreamOS, pContext)
{
	//empty
}

RESULT DreamControlView::InitializeApp(void *pContext) {
	RESULT r = R_PASS;
	DreamOS *pDreamOS = GetDOS();
	
	m_pView = GetComposite()->AddUIView(pDreamOS);
	CN(m_pView);
	m_pViewQuad = m_pView->AddQuad(.96f, .54f, 1, 1, nullptr);
	m_pViewQuad->SetOrientation(quaternion::MakeQuaternionWithEuler((float)M_PI / 3.0f, 0.0f, 0.0f));
	CN(m_pViewQuad);
	m_pViewQuad->SetMaterialAmbient(0.75f);
	m_pViewQuad->FlipUVVertical();
	CR(m_pViewQuad->SetVisible(false));

	m_viewState = State::HIDDEN;

	m_ptHiddenPosition = point(0.0f, -0.25f, 5.0f);
	m_ptVisiblePosition = point(0.0f, -0.25f, 4.0f);

	m_hiddenScale = 0.2f;
	m_visibleScale = 1.0f;

	m_hideThreshold = 0.20f;
	m_showThreshold = -0.35f;

	m_pLeftMallet = new UIMallet(GetDOS());
	CN(m_pLeftMallet);

	m_pRightMallet = new UIMallet(GetDOS());
	CN(m_pRightMallet);
	m_pRightMallet->Show();
	m_pLeftMallet->Show();

	pDreamOS->AddInteractionObject(m_pLeftMallet->GetMalletHead());
	pDreamOS->AddInteractionObject(m_pRightMallet->GetMalletHead());
	pDreamOS->AddAndRegisterInteractionObject(m_pViewQuad.get(), ELEMENT_COLLIDE_BEGAN, this);
	pDreamOS->RegisterSubscriber(SenseControllerEventType::SENSE_CONTROLLER_PAD_MOVE, this);
Error:
	return r;
}

RESULT DreamControlView::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamControlView::Update(void *pContext) {
	RESULT r = R_PASS;
	DreamOS *pDreamOS = GetDOS();
	vector vLook = GetDOS()->GetCamera()->GetLookVector();

	RotationMatrix qOffset = RotationMatrix();
	hand *pHand = pDreamOS->GetHand(HAND_TYPE::HAND_LEFT);
	CNR(pHand, R_OBJECT_NOT_FOUND);
	qOffset.SetQuaternionRotationMatrix(pHand->GetOrientation());

	if (m_pLeftMallet)
		m_pLeftMallet->GetMalletHead()->MoveTo(pHand->GetPosition() + point(qOffset * m_pLeftMallet->GetHeadOffset()));

	pHand = pDreamOS->GetHand(HAND_TYPE::HAND_RIGHT);
	CNR(pHand, R_OBJECT_NOT_FOUND);

	qOffset = RotationMatrix();
	qOffset.SetQuaternionRotationMatrix(pHand->GetOrientation());

	if (m_pRightMallet)
		m_pRightMallet->GetMalletHead()->MoveTo(pHand->GetPosition() + point(qOffset * m_pRightMallet->GetHeadOffset()));

	switch (m_viewState) {

	case State::VISIBLE: {
		if (vLook.y() > m_hideThreshold)
			Hide();
	} break;
		
	case State::HIDDEN: {
		if (vLook.y() < m_showThreshold)
			Show();
	} break;
	
	}

Error:
	return r;
}

RESULT DreamControlView::Notify(InteractionObjectEvent *pInteractionEvent) {
	RESULT r = R_PASS;
	if (pInteractionEvent->m_pObject == m_pViewQuad.get() &&
		(pInteractionEvent->m_pInteractionObject == m_pLeftMallet->GetMalletHead() || pInteractionEvent->m_pInteractionObject == m_pRightMallet->GetMalletHead())) {
		switch (pInteractionEvent->m_eventType) {
		case (InteractionEventType::ELEMENT_COLLIDE_BEGAN): {
			m_ptContact = pInteractionEvent->m_ptContact[0];
			m_flag = true;	// just for testing?
			CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));

			std::vector<UID> uids = GetDOS()->GetAppUID("DreamBrowser");
			CB(uids.size() == 1);
			UID browserUID = uids[0];

			auto pBrowserHandle = dynamic_cast<DreamBrowserHandle*>(GetDOS()->CaptureApp(browserUID, this));
			CNR(pBrowserHandle, R_OBJECT_NOT_FOUND);

			pBrowserHandle->SetClickParams(GetRelativePointofContact());

			CR(GetDOS()->ReleaseApp(pBrowserHandle, browserUID, this));
		} break;
		}
	}
Error:
	return r;
}

RESULT DreamControlView::Notify(SenseControllerEvent *pEvent) {
	RESULT r = R_PASS;
	switch (pEvent->type) {
	case SenseControllerEventType::SENSE_CONTROLLER_PAD_MOVE: {
		m_velocity.x = pEvent->state.ptTouchpad.x() * BROWSER_SCROLL_CONSTANT;
		m_velocity.y = pEvent->state.ptTouchpad.y() * BROWSER_SCROLL_CONSTANT;
		m_flag = true;
		CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 2.0f, 1));

		std::vector<UID> uids = GetDOS()->GetAppUID("DreamBrowser");
		CB(uids.size() == 1);
		UID browserUID = uids[0];

		auto pBrowserHandle = dynamic_cast<DreamBrowserHandle*>(GetDOS()->CaptureApp(browserUID, this));
		CNR(pBrowserHandle, R_OBJECT_NOT_FOUND);

		pBrowserHandle->SetScrollingParams(m_velocity);

		CR(GetDOS()->ReleaseApp(pBrowserHandle, browserUID, this));
	} break;
	}
Error:
	return r;
}

RESULT DreamControlView::Shutdown(void *pContext) {
	return R_PASS;
}

DreamControlView *DreamControlView::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamControlView *pDreamControlView = new DreamControlView(pDreamOS, pContext);
	return pDreamControlView;
}

RESULT DreamControlView::Show() {
	RESULT r = R_PASS;
	SetSharedViewContext();
	UpdateCompositeWithCameraLook(0.6f, -0.20f);	//depth, height
	m_ptVisiblePosition = GetComposite()->GetPosition();
	m_ptHiddenPosition = GetComposite()->GetPosition() - point(0.0f, 1.0f, 0.0f);

	auto fnStartCallback = [&](void *pContext) {
		GetViewQuad()->SetVisible(true);
		SetViewState(State::SHOW);
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		SetViewState(State::VISIBLE);
		return R_PASS;
	};

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		GetComposite(),
		m_ptVisiblePosition,
		GetComposite()->GetOrientation(),
		vector(m_visibleScale, m_visibleScale, m_visibleScale),
		0.1f,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));


Error:
	return r;
}

RESULT DreamControlView::Hide() {
	RESULT r = R_PASS;

	auto fnStartCallback = [&](void *pContext) {
		SetViewState(State::HIDE);
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		GetViewQuad()->SetVisible(false);
		SetViewState(State::HIDDEN);
		return R_PASS;
	};

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		GetComposite(),
		m_ptHiddenPosition,
		GetComposite()->GetOrientation(),
		vector(m_hiddenScale, m_hiddenScale, m_hiddenScale),
		0.1f,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));

Error:
	return r;
}

RESULT DreamControlView::SetSharedViewContext() {
	RESULT r = R_PASS;
	std::vector<UID> uids = GetDOS()->GetAppUID("DreamBrowser");
	UID browserUID;
	CB(uids.size() == 1);
	browserUID = uids[0];
	
	auto pBrowserHandle = dynamic_cast<DreamBrowserHandle*>(GetDOS()->CaptureApp(browserUID, this));
	CNR(pBrowserHandle, R_OBJECT_NOT_FOUND);

	CR(m_pViewQuad->SetDiffuseTexture(pBrowserHandle->GetBrowserTexture().get()));
	CR(GetDOS()->ReleaseApp(pBrowserHandle, browserUID, this));
Error:
	return r;
}

WebBrowserPoint DreamControlView::GetRelativePointofContact() {
	point ptIntersectionContact = m_ptContact;
	ptIntersectionContact.w() = 1.0f;
	WebBrowserPoint ptRelative;
	// First apply transforms to the ptIntersectionContact 
	point ptAdjustedContact = inverse(m_pViewQuad->GetModelMatrix()) * ptIntersectionContact;
	

	float width = m_pViewQuad->GetWidth();
	float height = m_pViewQuad->GetHeight();

	float posX = ptAdjustedContact.x();
	float posY = ptAdjustedContact.z();
	float posZ = ptAdjustedContact.z();

	posX /= width / 2.0f;
	posY /= height / 2.0f;

	posX = (posX + 1.0f) / 2.0f;
	posY = (posY + 1.0f) / 2.0f;  // flip it
	
	ptRelative.x = posX * 1366;	//probably add these fields to the handle, or we need a way to convert to match browser pixel density
	ptRelative.y = posY * 768;

	return ptRelative;
}

std::shared_ptr<quad> DreamControlView::GetViewQuad() {
	return m_pViewQuad;
}

RESULT DreamControlView::SetViewState(State state) {
	m_viewState = state;
	return R_PASS;
}

WebBrowserPoint DreamControlView::GetScrollVelocity() {
	return m_velocity;
}