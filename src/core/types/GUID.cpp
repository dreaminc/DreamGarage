#include "GUID.h"

#include <climits>
#include <string>
#include <sstream>

// Initialize the static value
UINT64 GUID::ms_uiIDCounterUpper = (UINT64)(0);
UINT64 GUID::ms_uiIDCounterLower = (UINT64)(0);

GUID::GUID() :
	TypeObj(TYPE_OBJ_GUID)
{
	GUID::GetNewUID(&m_uiIDLower, &m_uiIDUpper);
}

std::string GUID::GetGUIDString() {
	std::string strReturn = "";
	uint8_t bytes[16];
	memset(&bytes, 0, sizeof(uint8_t) * 16);

	UINT64 uiAccumulatorLower = m_uiIDLower;
	UINT64 uiAccumulatorUpper = m_uiIDUpper;

	for (int i = 0; i < 8; i++) {
		bytes[15 - i] = uiAccumulatorLower & (0x000000FF);
		bytes[7 - i] = uiAccumulatorUpper & (0x000000FF);

		uiAccumulatorLower = uiAccumulatorUpper >> 8;
		uiAccumulatorUpper = uiAccumulatorUpper >> 8;
	}

	for (int i = 0; i < 16; i++) {
		char pBuff[10];
		memset(pBuff, 0, sizeof(char) * 10);
		snprintf(pBuff, sizeof(pBuff), "%02x", bytes[i]);
		strReturn += pBuff;
	}

	return strReturn;
}

RESULT GUID::SetGUIDFromString(std::string strGUID) {
	RESULT r = R_PASS;

	// Ensure valid string (16 bytes (2 chars x 32))
	CB((strGUID.length() == 32));
	{
		std::string strUpper = strGUID.substr(0, 16);
		std::string strLower = strGUID.substr(16);

		m_uiIDUpper = std::stoull(strUpper, nullptr, 16);
		m_uiIDLower = std::stoull(strLower, nullptr, 16);;
	}

Error:
	return r;
}

RESULT GUID::IncrementUID() {
	RESULT r = R_PASS;
	
	if (ms_uiIDCounterLower < ULLONG_MAX) {
		ms_uiIDCounterLower++;
	}
	else {
		ms_uiIDCounterLower = 0;
		ms_uiIDCounterUpper++;
		
		CBR((ms_uiIDCounterUpper != ULLONG_MAX), R_OVERFLOW);
	}

Error:
	return r;
}

RESULT GUID::GetNewUID(UINT64 *puiIDCounterLower, UINT64 *puiIDCounterUpper) {
	RESULT r = R_PASS;

	GUID::IncrementUID();

	CN(puiIDCounterUpper);
	CN(puiIDCounterLower);

	*puiIDCounterLower = GUID::ms_uiIDCounterLower;
	*puiIDCounterUpper = GUID::ms_uiIDCounterUpper;

Error:
	return r;
}