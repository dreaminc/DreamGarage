#include "UIMenuItem.h"

UIMenuItem::UIMenuItem(composite* c) :
m_context(c) 
{
	Initialize();
}

UIMenuItem::~UIMenuItem() {
	// empty
}

RESULT UIMenuItem::Initialize() {
	return R_PASS;
}