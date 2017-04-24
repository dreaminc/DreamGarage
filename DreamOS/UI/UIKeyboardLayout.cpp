#include "UIKeyboardLayout.h"

UIKey::UIKey() {}

UIKey::UIKey(float left, float width, std::string& letter) 
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
		std::string ch = std::string(1, c); 
		auto k = new UIKey(left, 0.1f, ch);
		row.emplace_back(k);
		left += 0.1f;
	}
	row.pop_back();
	m_pLayout.emplace_back(row);

	row.clear();
	left = 0.05f;
	for (auto c : "asdfghjkl") {
		std::string ch = std::string(1, c); 
		auto k = new UIKey(left, 0.1f, ch);
		row.emplace_back(k);
		left += 0.1f;
	}
	row.pop_back();
	m_pLayout.emplace_back(row);

	row.clear();
	left = 0.15f;
	for (auto c : "zxcvbnm") {
		std::string ch = std::string(1, c); 
		auto k = new UIKey(left, 0.1f, ch);
		row.emplace_back(k);
		left += 0.1f;
	}
	row.pop_back();
	m_pLayout.emplace_back(row);

	row.clear();
	left = 0.25f;
	{
		std::string ch = " ";
		auto k = new UIKey(left, 0.35f, ch);
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
