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
	RESULT r = R_PASS;

	m_pButton = m_context->AddComposite();
	m_pQuad = m_pButton->AddQuad(1.0f, 1.0f);

	return r;
}

std::shared_ptr<composite> UIMenuItem::GetButton() {
	return m_pButton;
}

std::shared_ptr<quad> UIMenuItem::GetQuad() {
	return m_pQuad;
}