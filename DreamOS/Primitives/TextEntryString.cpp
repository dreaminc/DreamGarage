#include "TextEntryString.h"
#include "Sense/SenseKeyboard.h"

RESULT TextEntryString::UpdateString(unsigned int keyCode) {
	switch (keyCode) {
	case SVK_CONTROL:
	case SVK_SHIFT: 
		break;
	case SVK_PRIOR: 
		m_string += "www.";
		break;
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
	return R_PASS;
	
}