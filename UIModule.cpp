#include "UIModule.h"

UIModule::UIModule(composite* c) :
	m_context(c)
{
	// empty
}
UIModule::~UIModule()
{
	// empty
}

RESULT UIModule::Initialize()
{
	return R_PASS;
}

RESULT UIModule::CreateMenuLayer(int numButtons) {
	RESULT r = R_PASS;

	UIMenuLayer* menu = new UIMenuLayer(m_context);
	m_pLayers.emplace_back(menu);

	for (int i = 0; i < numButtons; i++) {
		menu->CreateMenuItem();
	}
	return r;
}