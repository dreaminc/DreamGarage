#ifndef UI_KEYBOARD_LAYOUT_H_
#define UI_KEYBOARD_LAYOUT_H_

#include "RESULT/EHM.h"
#include "Primitives/dirty.h"
#include "UIKey.h"

#include <memory>
#include <vector>

class quad;

enum class LayoutType {
	QWERTY,
	QWERTY_UPPER,
	NUM,
	INVALID
};

class UIKeyboardLayout : public dirty {
public:
	UIKeyboardLayout(LayoutType type = LayoutType::QWERTY);
	~UIKeyboardLayout();

public:
	RESULT UpdateKeysWithLayout(LayoutType type = LayoutType::QWERTY);
	RESULT CreateQWERTYLayout();

	RESULT SetLayoutType(LayoutType type);
	std::vector<std::vector<std::shared_ptr<UIKey>>> GetKeys();

private:
	LayoutType m_layoutType;
	std::vector<std::vector<std::shared_ptr<UIKey>>> m_pLayout;
};



#endif // ! UI_KEYBOARD_LAYOUT_H_