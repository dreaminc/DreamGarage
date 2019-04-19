#include "UISurface.h"
#include "DreamOS.h"
#include "WebBrowser/WebBrowserController.h"

#include "Primitives/hand/hand.h"

UISurface::UISurface(HALImp *pHALImp, DreamOS *pDreamOS) :
	UIView(pHALImp, pDreamOS)
{
	//
}

RESULT UISurface::InitializeSurfaceQuad(float width, float height) {
	RESULT r = R_PASS;

	m_pViewQuad = AddQuad(width, height, 1, 1, nullptr);
	CN(m_pViewQuad);

	m_pDreamOS->RegisterEventSubscriber(this, InteractionEventType::INTERACTION_EVENT_PAD_MOVE, this);

	m_ptLastEvent.x() = -1.0f;
	m_ptLastEvent.y() = -1.0f;

Error:
	return r;
}

RESULT UISurface::UpdateWithHand(hand *pHand, bool &fMalletDirty, bool &fMouseDown, HAND_TYPE handType) {
	RESULT r = R_PASS;

	point ptBoxOrigin = m_pViewQuad->GetOrigin(true);
	point ptSphereOrigin = pHand->GetMalletHead()->GetOrigin(true);
	ptSphereOrigin = (point)(inverse(RotationMatrix(m_pViewQuad->GetOrientation(true))) * (ptSphereOrigin - m_pViewQuad->GetOrigin(true)));
	
	{
		//only allow button presses when visible
		CBR(IsVisible(), R_SKIPPED);
	}

	if (ptSphereOrigin.y() >= pHand->GetMalletRadius()) {

		fMalletDirty = false;

		if (fMouseDown) {
			fMouseDown = false;

			if (m_fMouseDrag) {
				m_fMouseDrag = false;
				UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SELECT_ENDED, m_pViewQuad.get(), pHand->GetMalletHead(), ptSphereOrigin);
				NotifySubscribers(UI_SELECT_ENDED, pUIEvent);
			}
			else {
				UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SELECT_ENDED, m_pViewQuad.get(), pHand->GetMalletHead(), m_ptLastEvent);
				NotifySubscribers(UI_SELECT_ENDED, pUIEvent);
			}
		}

		//TODO: this needs to be used for scroll events
		if (handType == HAND_TYPE::HAND_LEFT) {
			m_ptLeftHover = ptSphereOrigin;
		}
		else {
			m_ptRightHover = ptSphereOrigin;
		}

	}

	float xDistance = ptSphereOrigin.x() - m_ptClick.x();
	float zDistance = ptSphereOrigin.z() - m_ptClick.z();
	float squaredDistance = xDistance * xDistance + zDistance * zDistance;

	if (ptSphereOrigin.y() < pHand->GetMalletRadius() && fMouseDown && squaredDistance > m_dragThresholdSquared) {
		m_fMouseDrag = true;
		UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SELECT_MOVED, m_pViewQuad.get(), pHand->GetMalletHead(), ptSphereOrigin);
		NotifySubscribers(UI_SELECT_MOVED, pUIEvent);
		m_ptClick = ptSphereOrigin;
	}

	// if the sphere is lower than its own radius, there must be an interaction
	if (ptSphereOrigin.y() < pHand->GetMalletRadius() && !fMalletDirty) {

		float quadWidth = m_pViewQuad->GetWidth();
		float quadHeight = m_pViewQuad->GetHeight();

		bool fNotInQuad = ptSphereOrigin.x() > quadWidth / 2.0f || ptSphereOrigin.x() < -quadWidth / 2.0f ||
			ptSphereOrigin.z() > quadHeight / 2.0f || ptSphereOrigin.z() < -quadHeight / 2.0f;

		fMalletDirty = true;

		//TODO: check minimized
//		fNotInQuad
		CBR(!fNotInQuad, R_SKIPPED);

		m_ptClick = ptSphereOrigin;
		fMouseDown = true;

		if (handType == HAND_TYPE::HAND_LEFT) {
			CR(m_pDreamOS->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_LEFT, SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
		}
		else {
			CR(m_pDreamOS->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_RIGHT, SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
		}

		UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SELECT_BEGIN, m_pViewQuad.get(), pHand->GetMalletHead(), ptSphereOrigin);
		NotifySubscribers(UI_SELECT_BEGIN, pUIEvent);

		m_ptLastEvent = ptSphereOrigin;
	}

Error:
	return r;
}

std::shared_ptr<quad> UISurface::GetViewQuad() {
	return m_pViewQuad;
}	
point UISurface::GetLastEvent() {
	return m_ptLastEvent;
}

RESULT UISurface::ResetLastEvent() {
	RESULT r = R_PASS;

	m_ptLastEvent = point(-1.0f, -1.0f, 0.0f);

	UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SELECT_BEGIN, m_pViewQuad.get(), nullptr, m_ptLastEvent);
	CR(NotifySubscribers(UI_SELECT_BEGIN, pUIEvent));
	pUIEvent = new UIEvent(UIEventType::UI_SELECT_ENDED, m_pViewQuad.get(), nullptr, m_ptLastEvent);
	CR(NotifySubscribers(UI_SELECT_ENDED, pUIEvent));

Error:
	return r;
}
	
RESULT UISurface::Notify(InteractionObjectEvent *pEvent) {
	RESULT r = R_PASS;

	ControllerState state = pEvent->m_state;

	CR(UIView::Notify(pEvent));
	CBR(IsVisible(), R_SKIPPED);

	switch (pEvent->m_eventType) {
	case InteractionEventType::INTERACTION_EVENT_PAD_MOVE: {
		point ptScroll;
		VirtualObj *pObj = nullptr;
		if (state.type == CONTROLLER_TYPE::CONTROLLER_LEFT) {
			ptScroll = m_ptLeftHover;
			pObj = m_pDreamOS->GetHand(HAND_TYPE::HAND_LEFT);
		}
		else if (state.type == CONTROLLER_TYPE::CONTROLLER_RIGHT) {
			ptScroll = m_ptRightHover;
			pObj = m_pDreamOS->GetHand(HAND_TYPE::HAND_RIGHT);
			//pObj = nullptr;
		}

		point ptDiff = point(-state.ptTouchpad.x()*SCROLL_CONSTANT, state.ptTouchpad.y()*SCROLL_CONSTANT, 0.0f);
		if (ptScroll.x() < m_pViewQuad->GetWidth()/2.0f && ptScroll.x() > -m_pViewQuad->GetWidth()/2.0f &&
			ptScroll.z() < m_pViewQuad->GetHeight()/2.0f && ptScroll.z() > -m_pViewQuad->GetHeight()/2.0f) {
			UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SCROLL, m_pViewQuad.get(), pObj, ptScroll, ptDiff);
			NotifySubscribers(UI_SCROLL, pUIEvent);
		}
		//*
		else {
			UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SCROLL, m_pViewQuad.get(), pObj, point(0.0f, 0.0f, 0.0f), ptDiff);
			NotifySubscribers(UI_SCROLL, pUIEvent);
		}
		//*/

	} break;
	}
Error:
	return r;
}