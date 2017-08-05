#include "UIKeyboardLayout.h"
#include "Sense/SenseKeyboard.h"
#include "Primitives/quad.h"

#include "Primitives/texture.h"

UIKeyboardLayout::UIKeyboardLayout(LayoutType type) :
m_layoutType(type)
{}

UIKeyboardLayout::~UIKeyboardLayout() {}

RESULT UIKeyboardLayout::UpdateKeysWithLayout(LayoutType type) {
	RESULT r = R_PASS;

	// there will be different layouts for caps, numbers
	switch (type) {
	case LayoutType::QWERTY: CR(UpdateQWERTYLayout()); break;
	case LayoutType::QWERTY_UPPER: CR(UpdateQWERTYLayout(true, false)); break;
	case LayoutType::QWERTY_NUM: CR(UpdateQWERTYLayout(false, true)); break;
	case LayoutType::QWERTY_SYMBOL: CR(UpdateQWERTYLayout(true, true)); break;
	default: break;
	}

	m_layoutType = type;
Error:
	return r;
}

//TODO: some of the logic around how the keys are created is still hard-coded in CreateQWERTYLayout
std::string UIKeyboardLayout::GenerateQWERTYRow(int rowIndex, bool fUpper, bool fNum) {

	switch (rowIndex) {
	case 0: {
		if (!fNum) {
			if (fUpper) { return "QWERTYUIOP"; }
			else {	return "qwertyuiop"; }
		}
		else { 
			if (fUpper) { return "[]{}#%^*+="; }
			else { return "1234567890"; }
		}
	} break;
	case 1: {
		if (!fNum) {
			if (fUpper) { return "ASDFGHJKL"; }
			else { return "asdfghjkl"; }
		}
		else {
			if (fUpper) { return "_.|~<>..."; } // TODO: periods on unused keys
			else { return "-/:;()$&@"; }
		}
	} break;
	case 2: {
		if (!fNum) {
			if (fUpper) { return "ZXCVBNM"; }
			else { return "zxcvbnm"; }
		}
		else {
			if (fUpper) { return ".,?!'.."; }
			else { return ".,?!'.."; }
		}
	} break;

	default: return ""; break;
	}
	
	return "";
}

RESULT UIKeyboardLayout::UpdateQWERTYLayout(bool fUpper, bool fNum) {
	RESULT r = R_PASS;

	std::string rowChars;

	rowChars = GenerateQWERTYRow(0, fUpper, fNum);
	
	for (int i = 0; i < m_pLayout[0].size(); i++) {
		auto key = m_pLayout[0][i];
		key->m_letter = rowChars[i];
	}

	rowChars = GenerateQWERTYRow(1, fUpper, fNum);

	for (int i = 0; i < m_pLayout[1].size(); i++) {
		auto key = m_pLayout[1][i];
		key->m_letter = rowChars[i];
	}

	rowChars = GenerateQWERTYRow(2, fUpper, fNum);

	for (int i = 1; i < m_pLayout[2].size()-1; i++) {
		auto key = m_pLayout[2][i];
		key->m_letter = rowChars[i-1];
	}
	SetDirty();

//Error:
	return r;
}

RESULT UIKeyboardLayout::CreateQWERTYLayout(bool fUpper, bool fNum) {
	RESULT r = R_PASS;

	m_pLayout.clear();
	std::vector<std::shared_ptr<UIKey>> row;
	std::string rowChars;

	float left = 0.0f;
	rowChars = GenerateQWERTYRow(0, fUpper, fNum);
	for (int i = 0; i < rowChars.size(); i++) {
		auto key = new UIKey(left, 0.1f, rowChars[i]);
		row.emplace_back(key);
		left += 0.1f;
	}
	m_pLayout.emplace_back(row);
	row.clear();

	left = 0.05f;
	rowChars = GenerateQWERTYRow(1, fUpper, fNum);
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
	rowChars = GenerateQWERTYRow(2, fUpper, fNum);
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
		left += 0.35f;

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

LayoutType UIKeyboardLayout::GetLayoutType() {
	return m_layoutType;
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

RESULT UIKeyboardLayout::SetVisible(bool fVisible) {
	RESULT r = R_PASS;
	for (auto row : m_pLayout) {
		for (auto key : row) {
			CR(key->m_pQuad->SetVisible(fVisible));
		}
	}

Error:
	return r;
}

RESULT UIKeyboardLayout::SetRowHeight(float rowHeight) {
	m_rowHeight = rowHeight;
	return R_PASS;
}

float UIKeyboardLayout::GetRowHeight() {
	return m_rowHeight;
}

RESULT UIKeyboardLayout::SetKeyTexture(texture *pKeyTexture) {
	m_pKeyTexture = pKeyTexture;
	return R_PASS;
}

texture* UIKeyboardLayout::GetKeyTexture() {
	return m_pKeyTexture;
}

RESULT UIKeyboardLayout::AddToSpecialTextures(unsigned int keyChar, texture *pKeyTexture) {
	m_specialKeyTextures[keyChar] = pKeyTexture;
	return R_PASS;
}

texture *UIKeyboardLayout::GetSpecialTexture(unsigned int keyChar) {
	if (m_specialKeyTextures.count(keyChar) > 0) {
		return m_specialKeyTextures[keyChar];
	}
	return nullptr;
}