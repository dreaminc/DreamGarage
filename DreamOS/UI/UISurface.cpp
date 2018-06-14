#include "UISurface.h"
#include "DreamOS.h"
#include "WebBrowser/WebBrowserController.h"

UISurface::UISurface(HALImp *pHALImp, DreamOS *pDreamOS) :
	UIView(pHALImp, pDreamOS)
{

}

UISurface::~UISurface() {
	//empty
}

RESULT UISurface::InitializeSurfaceQuad(float width, float height) {
	RESULT r = R_PASS;

	m_pViewQuad = AddQuad(width, height, 1, 1, nullptr);
	CN(m_pViewQuad);

Error:
	return r;
}

RESULT UISurface::UpdateWithMallet(UIMallet *pMallet, bool &fMalletDirty, bool &fMouseDown, HAND_TYPE handType) {
	RESULT r = R_PASS;

	point ptBoxOrigin = m_pViewQuad->GetOrigin(true);
	point ptSphereOrigin = pMallet->GetMalletHead()->GetOrigin(true);
	ptSphereOrigin = (point)(inverse(RotationMatrix(m_pViewQuad->GetOrientation(true))) * (ptSphereOrigin - m_pViewQuad->GetOrigin(true)));

	// if keyboard is up, touching the view quad is always a dismiss
	/*
	if (m_pKeyboardHandle != nullptr && !m_fIsShareURL) {
		if (ptSphereOrigin.y() >= pMallet->GetRadius()) {
			fMalletDirty = false;
		}
		if (ptSphereOrigin.y() < pMallet->GetRadius() && !fMalletDirty) {
			CR(m_pParentApp->HideWebsiteTyping());
			fMalletDirty = true;
		}
	}
	//*/

	if (ptSphereOrigin.y() >= pMallet->GetRadius()) {

		fMalletDirty = false;

		if (fMouseDown) {
			fMouseDown = false;

			if (m_fMouseDrag) {
				m_fMouseDrag = false;
				UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SELECT_ENDED, m_pViewQuad.get(), pMallet->GetMalletHead(), ptSphereOrigin);
				NotifySubscribers(UI_SELECT_ENDED, pUIEvent);
			}
			else {
				UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SELECT_ENDED, m_pViewQuad.get(), pMallet->GetMalletHead(), m_ptLastEvent);
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

	if (ptSphereOrigin.y() < pMallet->GetRadius() && fMouseDown && squaredDistance > m_dragThresholdSquared) {
		m_fMouseDrag = true;
		UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SELECT_MOVED, m_pViewQuad.get(), pMallet->GetMalletHead(), ptSphereOrigin);
		NotifySubscribers(UI_SELECT_MOVED, pUIEvent);
		m_ptClick = ptSphereOrigin;
	}

	// if the sphere is lower than its own radius, there must be an interaction
	if (ptSphereOrigin.y() < pMallet->GetRadius() && !fMalletDirty) {

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

		UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SELECT_BEGIN, m_pViewQuad.get(), pMallet->GetMalletHead(), ptSphereOrigin);
		NotifySubscribers(UI_SELECT_BEGIN, pUIEvent);

		m_ptLastEvent = ptSphereOrigin;
	}

Error:
	return r;
}

std::shared_ptr<quad> UISurface::GetViewQuad() {
	return m_pViewQuad;
}