#ifndef UI_SURFACE_H_
#define UI_SURFACE_H_

#include "UIView.h"

class DreamOS;
class UIMallet;
class quad;

class UISurface : public UIView {
public:
	UISurface(HALImp *pHALImp, DreamOS *pDreamOS);
	~UISurface();

	RESULT InitializeSurfaceQuad(float width, float height);

public:
	RESULT UpdateWithMallet(UIMallet *pMallet, bool &fMalletDirty, bool &fMouseDown, HAND_TYPE handType);

	std::shared_ptr<quad> GetViewQuad();

private:
	point m_ptLeftHover;
	point m_ptRightHover;
	point m_ptLastEvent;
	point m_ptClick;

	float m_dragThresholdSquared;
	bool m_fMouseDrag = false;

	std::shared_ptr<quad> m_pViewQuad = nullptr;
};

#endif // ! UI_SURFACE_H_