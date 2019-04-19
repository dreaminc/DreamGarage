#ifndef UI_SURFACE_H_
#define UI_SURFACE_H_

#include "UIView.h"

#include "InteractionEngine/InteractionObjectEvent.h"
#include <chrono>

class DreamOS;
class hand;
class quad;

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