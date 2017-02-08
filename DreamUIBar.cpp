#include "DreamUIBar.h"

DreamUIBar::DreamUIBar(composite* c, UIBarFormat info) :
	UIBar(c, info) 
{
	// empty
}

DreamUIBar::~DreamUIBar() 
{
	// empty
}

RESULT DreamUIBar::RegisterEvent(DreamUIBar::UI_EVENT_TYPE type, std::function<RESULT(void*)> fnCallback, void* pContext) {
	return R_PASS;
}