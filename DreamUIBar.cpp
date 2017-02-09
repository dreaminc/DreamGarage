#include "DreamUIBar.h"

DreamUIBar::DreamUIBar(composite* pComposite) :
	UIBar(pComposite, UIBarFormat()) 
{
	//Initialize();
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
/*
	UIBarFormat info = UIBarFormat();

	info.menu[""] = { "lorem", "ipsum", "dolor", "sit" };
	info.menu["lorem"] = { "Watch", "Listen", "Play", "Whisper", "Present" };
	info.menu["ipsum"] = { "1", "2", "3" };
	info.menu["Play"] = { "a", "b", "c" };

	m_info = info;
	//*/

	std::shared_ptr<UIMenuLayer> pMenuLayer = CreateMenuLayer();
	CN(pMenuLayer);

	std::shared_ptr<UIMenuItem> pMenuItem = nullptr;

	pMenuItem = pMenuLayer->CreateMenuItem();

	CR(m_pContext->SetVisible(false));

Error:
	return r;
}