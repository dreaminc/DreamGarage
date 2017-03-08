#include "DreamUIBar.h"
#include "DreamOS.h"

DreamUIBar::DreamUIBar(DreamOS *pDreamOS, IconFormat& iconFormat, LabelFormat& labelFormat, UIBarFormat& barFormat) :
	UIBar(pDreamOS, iconFormat, labelFormat, barFormat)
{
	RESULT r = R_PASS;

	CR(DreamUIBar::Initialize());

//Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

DreamUIBar::~DreamUIBar() 
{
	// empty
}

// TODO: Will be implemented or moved once registration architecture is fleshed out
RESULT DreamUIBar::RegisterEvent(UIMenuItemEvent type, std::function<RESULT(void*)> fnCallback, void* pContext) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamUIBar::Initialize() {
	RESULT r = R_PASS;

	CR(SetVisible(false));

Error:
	return r;
}

RESULT DreamUIBar::Update() {
	RESULT r = R_PASS;
	CR(UpdateInteractionPrimitive(GetHandRay()));
Error:
	return r;
}