#ifndef GUID_H_
#define GUID_H_

#include "core/ehm/EHM.h"
#include "core/types/TypeObj.h"

// DREAM OS
// DreamOS/Cloud/core/types/GUID.h
// GUID is a globally unique identifier, different than UID in that UID is only unqiue to the
// Dream OS landscape.

#include <string>

class GUID : public TypeObj {
public:
	GUID();

	//UINT64 GetID();
	//RESULT Dealloc();

	std::string GetGUIDString();

	RESULT SetGUIDFromString(std::string strGUID);

	bool operator==(const GUID& rhs) {
		return ((m_uiIDUpper == rhs.m_uiIDUpper) && (m_uiIDLower == rhs.m_uiIDLower));
	}

	friend inline bool operator==(const GUID& lhs, const GUID& rhs) {
		return ((lhs.m_uiIDUpper == rhs.m_uiIDUpper) && (lhs.m_uiIDLower == rhs.m_uiIDLower));
	}

private:
	UINT64 m_uiIDUpper;
	UINT64 m_uiIDLower;

private:
	static RESULT IncrementUID();
	static RESULT GetNewUID(UINT64 *puiIDCounterLower, UINT64 *puiIDCounterUpper);

	// Initialized in source
	static UINT64 ms_uiIDCounterLower;
	static UINT64 ms_uiIDCounterUpper;

public:
	uint32_t Data1;
	uint16_t Data2;
	uint16_t Data3;
	uint8_t Data4[8];
};

#endif // ! GUID_H_
