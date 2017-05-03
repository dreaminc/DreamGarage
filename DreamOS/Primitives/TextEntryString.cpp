#include "TextEntryString.h"
#include "Sense/SenseKeyboard.h"

RESULT TextEntryString::UpdateString(unsigned int keyCode) {

	RESULT r = R_PASS;

	switch (keyCode) {
	case SVK_CONTROL:
	case SVK_SHIFT: 
		break;
	/*
	case SVK_PRIOR: 
		m_string += "www.";
		break;
	//*/
	case SVK_BACK:
		if (m_string.size() > 0) m_string.pop_back();
		break;
	case SVK_RETURN:
		m_string.clear();
		break;
	default:
		m_string += keyCode;
		break;
	}

//Error:
	return r;
}

RESULT TextEntryString::clear() {
	m_string.clear();
	return R_PASS;
}

size_t TextEntryString::size() {
	return m_string.size();
}

RESULT TextEntryString::pop_back() {
	m_string.pop_back();
	return R_PASS;
}