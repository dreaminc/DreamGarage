#include "GUID.h"

#include <climits>
#include <string>
#include <sstream>

// Initialize the static value
UINT64 guid::ms_uiIDCounterUpper = (UINT64)(0);
UINT64 guid::ms_uiIDCounterLower = (UINT64)(0);

guid::guid() :
	TypeObj(TYPE_OBJ_GUID)
{
	guid::GetNewUID(&m_uiIDLower, &m_uiIDUpper);
}

std::string guid::GetGUIDString() {
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

RESULT guid::IncrementUID() {
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

RESULT guid::GetNewUID(UINT64 *puiIDCounterLower, UINT64 *puiIDCounterUpper) {
	RESULT r = R_PASS;

	guid::IncrementUID();

	CN(puiIDCounterUpper);
	CN(puiIDCounterLower);

	*puiIDCounterLower = guid::ms_uiIDCounterLower;
	*puiIDCounterUpper = guid::ms_uiIDCounterUpper;

Error:
	return r;
}