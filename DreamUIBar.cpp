#include "DreamUIBar.h"

DreamUIBar::DreamUIBar(composite* pComposite, IconFormat& iconFormat, LabelFormat& labelFormat, UIBarFormat& barFormat) :
	UIBar(pComposite, iconFormat, labelFormat, barFormat) 
{
	Initialize();
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

	CR(m_pContext->SetVisible(false));

Error:
	return r;
}