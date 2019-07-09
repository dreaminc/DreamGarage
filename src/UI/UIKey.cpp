#include "UIKey.h"
#include "Primitives/quad.h"

UIKey::UIKey() {}

UIKey::UIKey(float left, float width, unsigned int letter) 
{
	m_left = left;
	m_width = width;
	m_letter = letter;

	m_state = KeyState::KEY_UP;
}
