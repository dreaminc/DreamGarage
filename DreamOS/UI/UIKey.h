#ifndef UI_KEY_H_
#define UI_KEY_H_

#include <memory>

class quad;

enum class KeyState {
	KEY_DOWN,
	KEY_MAYBE_DOWN,
	KEY_UP,
	KEY_MAYBE_UP,
	KEY_NOT_INTERSECTED
};

class UIKey {
public:
	UIKey();
	UIKey(float left, float width, unsigned int letter);
public:
	float m_left;	// left side of the key
	float m_width;  // width of the key
	unsigned int m_letter; // letter on the key

	std::shared_ptr<quad> m_pQuad; 

	KeyState m_state;
};


#endif // ! UI_KEY_H_