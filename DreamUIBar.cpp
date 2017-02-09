#include "DreamUIBar.h"

DreamUIBar::DreamUIBar(composite* pComposite, UIMenuItem::IconFormat iconFormat, UIMenuItem::LabelFormat labelFormat) :
	UIBar(pComposite, iconFormat, labelFormat) 
{
	Initialize();
}

DreamUIBar::~DreamUIBar() 
{
	// empty
}

RESULT DreamUIBar::RegisterEvent(UIMenuItemEvent type, std::function<RESULT(void*)> fnCallback, void* pContext) {
	return R_PASS;
}

RESULT DreamUIBar::Initialize() {

	RESULT r = R_PASS;

//	std::shared_ptr<UIMenuLayer> pMenuLayer = CreateMenuLayer();

//	CN(pMenuLayer);
	CR(m_pContext->SetVisible(false));

Error:
	return r;
}