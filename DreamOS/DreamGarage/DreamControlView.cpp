#include "DreamControlView.h"
#include "DreamBrowser.h"
#include "DreamOS.h"
#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"
#include "DreamConsole/DreamConsole.h"

#include "UI/UIMallet.h"
#include "UI/UIView.h"

RESULT DreamControlViewHandle::SendTextureToControlView(std::shared_ptr<texture> pBrowserTexture) {
	RESULT r = R_PASS;
	CB(GetAppState());

	return SetControlViewTexture(pBrowserTexture);
Error:
	return r;
}

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

	m_pViewQuad = m_pView->AddQuad(CONTROL_VIEWQUAD_WIDTH, CONTROL_VIEWQUAD_HEIGHT, 1, 1, nullptr);
	m_pViewQuad->SetOrientation(quaternion::MakeQuaternionWithEuler((float)CONTROL_VIEWQUAD_ANGLE, 0.0f, 0.0f));
	CN(m_pViewQuad);

	m_pViewQuad->SetMaterialAmbient(0.75f);
	m_pViewQuad->FlipUVVertical();
	CR(m_pViewQuad->SetVisible(false));

	m_viewState = State::HIDDEN;

	m_hiddenScale = 0.2f;
	m_visibleScale = 1.0f;	// changing this breaks things - change height and width too / instead.

	m_hideThreshold = 0.20f;
	m_showThreshold = -0.35f;

	m_pLeftMallet = new UIMallet(GetDOS());
	CN(m_pLeftMallet);
	m_pLeftMallet->Show();

	m_pRightMallet = new UIMallet(GetDOS());
	CN(m_pRightMallet);
	m_pRightMallet->Show();

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
			point ptContact = pInteractionEvent->m_ptContact[0];

			CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));

			CNR(m_pBrowserHandle, R_OBJECT_NOT_FOUND);

			m_pBrowserHandle->SendClickToBrowserAtPoint(GetRelativePointofContact(ptContact));

		} break;
		}
	}
Error:
	return r;
}

RESULT DreamControlView::Notify(SenseControllerEvent *pEvent) {
	RESULT r = R_PASS;
	if (m_viewState == State::VISIBLE) {
		switch (pEvent->type) {
		case SenseControllerEventType::SENSE_CONTROLLER_PAD_MOVE: {
			int pxXDiff = pEvent->state.ptTouchpad.x() * BROWSER_SCROLL_CONSTANT;
			int pxYDiff = pEvent->state.ptTouchpad.y() * BROWSER_SCROLL_CONSTANT;

			CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 2.0f, 1));

			CNR(m_pBrowserHandle, R_OBJECT_NOT_FOUND);

			m_pBrowserHandle->ScrollByDiff(pxXDiff, pxYDiff);

		} break;
		}
	}
Error:
	return r;
}

RESULT DreamControlView::Shutdown(void *pContext) {
	return R_PASS;
}

DreamAppHandle* DreamControlView::GetAppHandle() {
	return (DreamControlViewHandle*)(this);
}

RESULT DreamControlView::SetControlViewTexture(std::shared_ptr<texture> pBrowserTexture) {
	m_pViewQuad->SetDiffuseTexture(pBrowserTexture.get());	//Control view texture to be set by Browser
	return R_PASS;
}

DreamControlView *DreamControlView::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamControlView *pDreamControlView = new DreamControlView(pDreamOS, pContext);
	return pDreamControlView;
}

RESULT DreamControlView::Show() {
	RESULT r = R_PASS;

	std::vector<UID> uids = GetDOS()->GetAppUID("DreamBrowser");	// capture browser
	CB(uids.size() == 1);
	m_browserUID = uids[0];

	m_pBrowserHandle = dynamic_cast<DreamBrowserHandle*>(GetDOS()->CaptureApp(m_browserUID, this));

	SetSharedViewContext();
	UpdateCompositeWithCameraLook(CONTROL_VIEW_DEPTH, CONTROL_VIEW_HEIGHT);	//depth, height

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

	CR(GetDOS()->ReleaseApp(m_pBrowserHandle, m_browserUID, this)); // release browser
Error:
	return r;
}

///*
RESULT DreamControlView::SetSharedViewContext() {
	RESULT r = R_PASS;

	CNR(m_pBrowserHandle, R_OBJECT_NOT_FOUND);

	CR(m_pViewQuad->SetDiffuseTexture(m_pBrowserHandle->GetBrowserTexture().get()));
	
Error:
	return r;
}
//*/

WebBrowserPoint DreamControlView::GetRelativePointofContact(point ptContact) {
	point ptIntersectionContact = ptContact;
	ptIntersectionContact.w() = 1.0f;
	WebBrowserPoint ptRelative;

	// First apply transforms to the ptIntersectionContact 
	point ptAdjustedContact = inverse(m_pViewQuad->GetModelMatrix()) * ptIntersectionContact;

	float width = m_pViewQuad->GetWidth();
	float height = m_pViewQuad->GetHeight();

	float posX = ptAdjustedContact.x() / (width / 2.0f);	// flip it
	float posY = ptAdjustedContact.z() / (height / 2.0f);
	//float posZ = ptAdjustedContact.z();	// 3D browser when

	posX = (posX + 1.0f) / 2.0f;
	posY = (posY + 1.0f) / 2.0f;  
	
	ptRelative.x = posX * m_pBrowserHandle->GetWidthOfBrowser();
	ptRelative.y = posY * m_pBrowserHandle->GetHeightOfBrowser();

	return ptRelative;
}

std::shared_ptr<quad> DreamControlView::GetViewQuad() {
	return m_pViewQuad;
}

RESULT DreamControlView::SetViewState(State state) {
	m_viewState = state;
	return R_PASS;
}
