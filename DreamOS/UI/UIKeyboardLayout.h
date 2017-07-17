#ifndef UI_KEYBOARD_LAYOUT_H_
#define UI_KEYBOARD_LAYOUT_H_

#include "RESULT/EHM.h"
#include "Primitives/dirty.h"
#include "UIKey.h"

#include <memory>
#include <vector>

class quad;
class texture;

enum class LayoutType {
	QWERTY,
	QWERTY_UPPER,
	QWERTY_NUM,
	QWERTY_SYMBOL,
	INVALID
};

class UIKeyboardLayout : public dirty {
public:
	UIKeyboardLayout(LayoutType type = LayoutType::QWERTY);
	~UIKeyboardLayout();

public:
	RESULT UpdateKeysWithLayout(LayoutType type = LayoutType::QWERTY);
	RESULT CreateQWERTYLayout(bool fUpper = false, bool fNum = false);

	LayoutType GetLayoutType();
	RESULT SetLayoutType(LayoutType type);
	RESULT UpdateQWERTYLayout(bool fUpper = false, bool fNum = false);
	std::vector<std::vector<std::shared_ptr<UIKey>>> GetKeys();

	RESULT SetVisible(bool fVisible);
	RESULT SetRowHeight(float rowHeight);
	float GetRowHeight();
	RESULT SetKeyTexture(texture *pKeyTexture);
	texture *GetKeyTexture();

private:
	std::string GenerateQWERTYRow(int rowIndex, bool fUpper = false, bool fNum = false);

private:
	LayoutType m_layoutType;
	std::vector<std::vector<std::shared_ptr<UIKey>>> m_pLayout;
	float m_rowHeight = 0.0625f;
	texture *m_pKeyTexture = nullptr;
};



#endif // ! UI_KEYBOARD_LAYOUT_H_
