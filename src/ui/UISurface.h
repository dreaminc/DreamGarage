#ifndef UI_SURFACE_H_
#define UI_SURFACE_H_

#include "core/ehm/EHM.h"

// Dream UI
// dos/src/ui/UISurface.h

// UI Surface is a more specific implementation of UI View
// TODO: Review this design

#include <chrono>

#include "UIView.h"

class DreamOS;
class hand;
class quad;
class InteractionObjectEvent;

#define SCROLL_CONSTANT 10.0f

class UISurface : public UIView {
public:
	UISurface(HALImp *pHALImp, DreamOS *pDreamOS);
	~UISurface() = default;

	RESULT InitializeSurfaceQuad(float width, float height);

public:
	RESULT UpdateWithHand(hand *pMallet, bool &fMalletDirty, bool &fMouseDown, HAND_TYPE handType);

	std::shared_ptr<quad> GetViewQuad();
	point GetLastEvent();
	RESULT ResetLastEvent();

	virtual RESULT Notify(InteractionObjectEvent *pEvent) override;

private:
	point m_ptLeftHover;
	point m_ptRightHover;
	point m_ptLastEvent;
	point m_ptClick;

	float m_dragThresholdSquared;
	bool m_fMouseDrag = false;

protected:
	std::shared_ptr<quad> m_pViewQuad = nullptr;
};

#endif // ! UI_SURFACE_H_