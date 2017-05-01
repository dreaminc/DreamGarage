#include "UIKeyboardLayout.h"
#include "Sense/SenseKeyboard.h"

UIKey::UIKey() {}

UIKey::UIKey(float left, float width, unsigned int letter) 
{
	m_left = left;
	m_width = width;
	m_letter = letter;
}

UIKeyboardLayout::UIKeyboardLayout(LayoutType type) :
m_layoutType(type)
{}

UIKeyboardLayout::~UIKeyboardLayout() {}

RESULT UIKeyboardLayout::UpdateKeysWithLayout(LayoutType type) {
	RESULT r = R_PASS;

	// there will be different layouts for caps, numbers
	switch (type) {
	case LayoutType::QWERTY: CR(CreateQWERTYLayout()); break;
	default: break;
	}
Error:
	return r;
}
RESULT UIKeyboardLayout::CreateQWERTYLayout() {
	RESULT r = R_PASS;

	m_pLayout.clear();
	std::vector<std::shared_ptr<UIKey>> row;
	float left = 0.0f;
	for (auto c : "qwertyuiop") {
		auto k = new UIKey(left, 0.1f, c);
		row.emplace_back(k);
		left += 0.1f;
	}
	row.pop_back();
	m_pLayout.emplace_back(row);

	row.clear();
	left = 0.05f;
	for (auto c : "asdfghjkl") {
		auto k = new UIKey(left, 0.1f, c);
		row.emplace_back(k);
		left += 0.1f;
	}
	row.pop_back();
	m_pLayout.emplace_back(row);

	row.clear();
	// Shift
	left = 0.0f;
	{
		auto k = new UIKey(left, 0.125f, SVK_SHIFT);
		row.emplace_back(k);
	}

	left = 0.15f;
	for (auto c : "zxcvbnm") {
		if (c) {
			auto k = new UIKey(left, 0.1f, c);
			row.emplace_back(k);
			left += 0.1f;
		}
	}

	// Backspace
	{
		auto k = new UIKey(0.875f, 0.125f, SVK_BACK);
		row.emplace_back(k);
	}

	m_pLayout.emplace_back(row);

	row.clear();
	left = 0.0f;
	{
		//TODO: using unused values for keys not defined by ascii
		auto k = new UIKey(left, 0.125f, SVK_CONTROL); // Number layer
		row.emplace_back(k);
		left += 0.125f;

		k = new UIKey(left, 0.125f, SVK_PRIOR); // www.
		row.emplace_back(k);
		left += 0.125f;

		left += 0.1f;

		k = new UIKey(left, 0.35f, SVK_SPACE);
	//	auto k = new UIKey(left, 0.1f, ch);
		row.emplace_back(k);

		left += 0.35f;
		k = new UIKey(left, 0.125f, '.');
		row.emplace_back(k);

		// 'go' or 'enter'
		left += 0.15f;
		k = new UIKey(left, 0.15f, SVK_RETURN);
		row.emplace_back(k);
	}
	m_pLayout.emplace_back(row);

//Error:
	return r;
}

RESULT UIKeyboardLayout::SetLayoutType(LayoutType type) {
	RESULT r = R_PASS;
	m_layoutType = type;
	CR(UpdateKeysWithLayout(m_layoutType));
	CR(SetDirty());
Error:
	return r;
}

std::vector<std::vector<std::shared_ptr<UIKey>>> UIKeyboardLayout::GetKeys() {
	return m_pLayout;
}
