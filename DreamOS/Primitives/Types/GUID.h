#ifndef GUID_H_
#define GUID_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/TypeObj.h"

// DREAM OS
// DreamOS/Cloud/Primitives/Types/GUID.h
// GUID is a globally unique identifier, different than UID in that UID is only unqiue to the
// Dream OS landscape.

#include <string>

class guid : public TypeObj {
public:
	guid();

	//UINT64 GetID();
	//RESULT Dealloc();

	std::string GetGUIDString();

	RESULT SetGUIDFromString(std::string strGUID);

	bool operator==(const guid& rhs) {
		return ((m_uiIDUpper == rhs.m_uiIDUpper) && (m_uiIDLower == rhs.m_uiIDLower));
	}

	friend inline bool operator==(const guid& lhs, const guid& rhs) {
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
};

#endif // ! GUID_H_
