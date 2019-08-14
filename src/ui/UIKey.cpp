#include "UIKey.h"

#include "core/primitives/quad.h"

UIKey::UIKey() :
	m_state(KeyState::KEY_UP)
{
	// 
}

UIKey::UIKey(float left, float width, unsigned int letter)  :
	m_left(left),
	m_width(width),
	m_letter(letter),
	m_state(KeyState::KEY_UP)
{
	//
}
