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
	std::string rowChars;

	float left = 0.0f;
	rowChars = "qwertyuiop";
	for (int i = 0; i < rowChars.size(); i++) {
		auto key = new UIKey(left, 0.1f, rowChars[i]);
		row.emplace_back(key);
		left += 0.1f;
	}
	m_pLayout.emplace_back(row);
	row.clear();

	left = 0.05f;
	rowChars = "asdfghjkl";
	for (int i = 0; i < rowChars.size(); i++) {
		auto key = new UIKey(left, 0.1f, rowChars[i]);
		row.emplace_back(key);
		left += 0.1f;
	}
	m_pLayout.emplace_back(row);
	row.clear();

	// Shift
	left = 0.0f;
	{
		auto key = new UIKey(left, 0.125f, SVK_SHIFT);
		row.emplace_back(key);
	}

	left = 0.15f;
	rowChars = "zxcvbnm";
	for (int i = 0; i < rowChars.size(); i++) {
		auto key = new UIKey(left, 0.1f, rowChars[i]);
		row.emplace_back(key);
		left += 0.1f;
	}

	// Backspace
	{
		auto key = new UIKey(0.875f, 0.125f, SVK_BACK);
		row.emplace_back(key);
	}

	m_pLayout.emplace_back(row);
	row.clear();

	left = 0.0f;
	{
		//TODO: using unused values for behaviors not defined by ascii
		auto key = new UIKey(left, 0.125f, SVK_CONTROL); // Number layer
		row.emplace_back(key);
		left += 0.125f;

		//TODO: leaving this out for now, 'www.' doesn't seem too useful
		/*
		key = new UIKey(left, 0.125f, SVK_PRIOR); // spare key
		row.emplace_back(key);
		//*/
		left += 0.125f;

		left += 0.1f;

		key = new UIKey(left, 0.35f, SVK_SPACE);
		row.emplace_back(key);

		left += 0.35f;
		key = new UIKey(left, 0.125f, '.');
		row.emplace_back(key);

		// 'go' or 'enter'
		left += 0.15f;
		key = new UIKey(left, 0.15f, SVK_RETURN);
		row.emplace_back(key);
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
